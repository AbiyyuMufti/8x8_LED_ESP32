#include <ArduinoJson.h>
#include "8x8LEDHandler.h"


void sendESPStatus(uint32_t periode) {
	static long lastTime = millis();
	static long now;
	now = millis();
	if (now - lastTime >= periode)
	{
		lastTime = now;
		onTXState();
	}
}


void onTXState() {
	// { "ADR": MAC_ADRESS, "BAT": value, "LDR" : value}
	StaticJsonDocument<100> toSend;
	toSend["ADR"] = MAC_ADR;
	toSend["BAT"] = BatteryState;
	toSend["LDR"] = LDRValue;
	char buffer[100];
	size_t n = serializeJson(toSend, buffer);
	client->publish("LED88ESP32/State", buffer, n);
}


void onRxCommand(const String& message) {
	// JSON Message: { "cmd": LightOff, "adr": "FF22DDAA0011" }
	static char msg[50];
	message.toCharArray(msg, 50);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	String cmd = receivedMsg["cmd"];
	String adr = receivedMsg["adr"];
	if (adr == "ALL")
	{
		MAC_CHECK = MAC_ADR;
	}
	else
	{
		MAC_CHECK = adr;
	}

	if (MAC_CHECK == MAC_ADR)
	{
		if (cmd.equals("LightOff")){
			CurrentState = LightOff;
		}
		else if (cmd.equals("TextGenerator")){
			CurrentState = TextGenerator;
		}
		else if (cmd.equals("TapToLight")){
			CurrentState = TapToLight;
		}
		else if (cmd.equals("LightShow")){
			CurrentState = LightShow;
		}
		else if (cmd.equals("SingleColor")) {
			CurrentState = SingleColor;
		}
		else if (cmd.equals("GetAddress")) {
			client->publish("LED88ESP32/Address", MAC_ADR);
		}
		else if (cmd.equals("GetState")){
			onTXState();
		}
		else
		{
			CurrentState = Invalid;
		}
	}

}


void onRxBrightness(const String& message) {
	// JSON Message: "{ "br": 255,  "adp": 1 }"
	static char msg[30];
	message.toCharArray(msg, 20);
	StaticJsonDocument<20> receivedMsg;
	deserializeJson(receivedMsg, msg);
	BRIGHTNESS = receivedMsg["br"];
	if (IS_ADAPTABLE_TO_LIGHT != bool(receivedMsg["adp"]))
	{
		IS_ADAPTABLE_TO_LIGHT = receivedMsg["adp"];
	}
	
}


void onRxTextGenerator(const String& message) {
	// JSON Message: { "txt": "sometext", "r": 255, "g": 255, "b": 255, "spd" : 255 }
	static char msg[256];
	message.toCharArray(msg, 256);

	StaticJsonDocument<256> receivedMsg;
	deserializeJson(receivedMsg, msg);

	const char* txt = receivedMsg["txt"];
	TXT_TEXT = String(txt);
	TXT_COLOR[0] = receivedMsg["r"];
	TXT_COLOR[1] = receivedMsg["g"];
	TXT_COLOR[2] = receivedMsg["b"];
	TXT_SPEED = map(receivedMsg["spd"], 50, 255, 150, 50);
}


void onRxPixels(const String& message) {
	// JSON Message: { "col": 8, "row": 8, "on": 1, "r": 255, "g": 255, "b": 255 }
	static char msg[100];
	message.toCharArray(msg, 100);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	byte col = receivedMsg["col"];
	byte row = receivedMsg["row"];
	PX_SELECT[col][row] = receivedMsg["on"];
	PX_COLORS[col][row][0] = receivedMsg["r"];
	PX_COLORS[col][row][1] = receivedMsg["g"];
	PX_COLORS[col][row][2] = receivedMsg["b"];
}


void onRxLightShow(const String& message) {
	// Json Message: "{"ptr": 16}"
	static char msg[20];
	message.toCharArray(msg, 20);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	PATTERN = receivedMsg["ptr"];

}

void onRxSingleColorSetColor(const String& message) {
	// Json Message: " {"sel": 16, "r": 255, "g": 255, "b": 255} "
	static char msg[50];
	message.toCharArray(msg, 50);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	byte selection = receivedMsg["sel"];
	if (ESP_NO == selection)
	{
		SINGLECOLOR.red = receivedMsg["r"];
		SINGLECOLOR.green = receivedMsg["g"];
		SINGLECOLOR.blue = receivedMsg["b"];
	}
}

void onRxSingleColorSetSequence(const String& message) {
	// Json Message: "{"seq": 3}"
	static char msg[20];
	message.toCharArray(msg, 20);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	SINGLECOLOR.sequence = receivedMsg["seq"];
}
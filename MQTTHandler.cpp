#include <ArduinoJson.h>
#include "8x8LEDHandler.h"


void sendESPStatus(uint32_t periode) {
	static long lastTime = millis();
	static long now;
	static String state[] = { "Invalid", "LightOff", "TextGenerator", "TapToLight", "LightShow", "SingleColor" };
	now = millis();
	if (now - lastTime >= periode)
	{
		Serial.println(state[CurrentState]);
		lastTime = now;
		onTXState();
	}
}


void onTXState() {
	// { "ADR": MAC_ADRESS, "BAT": value, "LDR" : value}
	StaticJsonDocument<100> toSend;
	toSend["BAT"] = ESPINFO.BatteryState;
	toSend["LDR"] = ESPINFO.LDRValue;
	char buffer[100];
	size_t n = serializeJson(toSend, buffer);
	client->publish("LED88ESP32/State", buffer, n);
}


void onRxCommand(const String& message) {
	// JSON Message: { "cmd": LightOff }
	static char msg[50];
	message.toCharArray(msg, 50);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);
	if (FOR_THIS_ESP)
	{
		String cmd = receivedMsg["cmd"];
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
	message.toCharArray(msg, 30);
	StaticJsonDocument<30> receivedMsg;
	deserializeJson(receivedMsg, msg);
	bool adp = receivedMsg["adp"];
	if (FOR_THIS_ESP && !adp)
	{
		BRIGHTNESS = receivedMsg["br"];
	}
	if (IS_ADAPTABLE_TO_LIGHT != adp)
	{
		IS_ADAPTABLE_TO_LIGHT = adp;
	}
}


void onRxTextGenerator(const String& message) {
	// JSON Message: { "txt": "sometext", "r": 255, "g": 255, "b": 255, "spd" : 255 }
	static char msg[256];
	message.toCharArray(msg, 256);

	StaticJsonDocument<256> receivedMsg;
	deserializeJson(receivedMsg, msg);

	if (FOR_THIS_ESP)
	{
		const char* txt = receivedMsg["txt"];
		TEXT.TEXT = String(txt);
		TEXT.COLOR[0] = receivedMsg["r"];
		TEXT.COLOR[1] = receivedMsg["g"];
		TEXT.COLOR[2] = receivedMsg["b"];
		TEXT.SPEED = map(receivedMsg["spd"], 50, 255, 150, 50);
	}
}


void onRxPixels(const String& message) {
	// JSON Message: { "col": 8, "row": 8, "on": 1, "r": 255, "g": 255, "b": 255 }
	static char msg[100];
	message.toCharArray(msg, 100);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	if (true)
	{
		byte col = receivedMsg["col"];
		byte row = receivedMsg["row"];
		PIXELS.SELECT[col][row] = receivedMsg["on"];
		PIXELS.COLORS[col][row][0] = receivedMsg["r"];
		PIXELS.COLORS[col][row][1] = receivedMsg["g"];
		PIXELS.COLORS[col][row][2] = receivedMsg["b"];
	}
}


void onRxLightShow(const String& message) {
	// Json Message: "{"ptr": 16}"
	static char msg[20];
	message.toCharArray(msg, 20);

	StaticJsonDocument<20> receivedMsg;
	deserializeJson(receivedMsg, msg);

	if (FOR_THIS_ESP)
	{
		PATTERN = receivedMsg["ptr"];
	}
}


void onRxSingleColorSetColor(const String& message) {
	// Json Message: " {"sel": 16, "r": 255, "g": 255, "b": 255} "
	static char msg[100];
	message.toCharArray(msg, 100);
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

	StaticJsonDocument<20> receivedMsg;
	deserializeJson(receivedMsg, msg);
	if (FOR_THIS_ESP)
	{
		SINGLECOLOR.sequence = receivedMsg["seq"];
	}
	
}


void onRxESPSelect(const String& message)
{
	// Json Message: "{"sel": [true, true, true, true, true, true, false, true, true, true, false, true, true, false, true, false]}"
	static char msg[120];
	message.toCharArray(msg, 120);
	StaticJsonDocument<256> receivedMsg;
	deserializeJson(receivedMsg, msg);

	JsonArray sel = receivedMsg["sel"];
	bool forThisDevice = sel[ESP_NO - 1];
	if (FOR_THIS_ESP != forThisDevice)
	{
		FOR_THIS_ESP = forThisDevice;
	}
}


void onRxLightShowAdvance(const String& message) {
	// JSON Message: { "sel": 16, "ptr": 20, "r": 255, "g": 255, "b": 255 }
	static char msg[100];
	message.toCharArray(msg, 100);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	byte selection = receivedMsg["sel"];
	byte ptr = receivedMsg["ptr"];
	if (ESP_NO == selection)
	{
		SINGLECOLOR.red = receivedMsg["r"];
		SINGLECOLOR.green = receivedMsg["g"];
		SINGLECOLOR.blue = receivedMsg["b"];
	}
	if (ptr < 3)
	{
		SINGLECOLOR.sequence = ptr;
	}
	else
	{
		PATTERN = ptr - 2;
	}
}
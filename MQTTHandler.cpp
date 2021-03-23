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
	toSend["ADR"] = ESP_NO;
	toSend["BAT"] = ESPINFO.BatteryState;
	toSend["LDR"] = ESPINFO.LDRValue;
	char buffer[100];
	size_t n = serializeJson(toSend, buffer);
	client->publish("LED88ESP32/State", buffer, n);
}


void onRxCommand(const String& message) {
	// JSON Message: { "cmd": LightOff }
	static char msg[100];
	message.toCharArray(msg, 100);

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
		else if (cmd.equals("GetState")){
			onTXState();
		}
		else if (cmd.equals("GoSleep")) {
			String time = receivedMsg["sleeptime"];
			uint16_t h = time.substring(0, 2).toInt();
			uint16_t m = time.substring(3, 5).toInt();
			uint16_t sleeptime = h * 60 * 60 + m * 60;
			matrix->clear();
			matrix->show();
			delay(500);
			beforesleep();
			device_go_to_sleep(sleeptime);
		}
		else
		{
			CurrentState = Invalid;
		}
	}
	IDLETIME = 0;
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
	IDLETIME = 0;
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
	IDLETIME = 0;
}


void onRxPixels(const String& message) {
	// JSON Message: { "col": 8, "row": 8, "on": 1, "r": 255, "g": 255, "b": 255 }
	static char msg[100];
	message.toCharArray(msg, 100);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	if (FOR_THIS_ESP)
	{
		byte col = receivedMsg["col"];
		byte row = receivedMsg["row"];
		PIXELS.COLORS[col][row][0] = receivedMsg["r"];
		PIXELS.COLORS[col][row][1] = receivedMsg["g"];
		PIXELS.COLORS[col][row][2] = receivedMsg["b"];
	}
	IDLETIME = 0;
}


void onRxSetSequence(const String& message) {
	// Json Message: "{ "seq": [0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]}"
	static char msg[200];
	message.toCharArray(msg, 200);

	StaticJsonDocument<384> receivedMsg;
	deserializeJson(receivedMsg, msg);
	IN_SEQUENCE = true;
	JsonArray seq = receivedMsg["seq"];
	ORDER = seq[ESP_NO-1];
	Serial.print("In Sequence: ");
	Serial.println(ORDER);
	IDLETIME = 0;
}


void onRxLightShow(const String& message) {
	// JSON Message: { "sel": 16, "ptr": 20, "r": 255, "g": 255, "b": 255 }
	static char msg[100];
	message.toCharArray(msg, 100);

	StaticJsonDocument<100> receivedMsg;
	deserializeJson(receivedMsg, msg);

	byte selection = receivedMsg["sel"];
	if (ESP_NO == selection)
	{
		LIGHTSHOW.red = receivedMsg["r"];
		LIGHTSHOW.green = receivedMsg["g"];
		LIGHTSHOW.blue = receivedMsg["b"];
		LIGHTSHOW.PATTERN = receivedMsg["ptr"];
	}
	IDLETIME = 0;
}


void onRxESPSelect(const String& message)
{
	// Json Message: "{"sel": [true, true, true, true, true, true, false, true, true, true, false, true, true, false, true, false]}"
	static char msg[200];
	message.toCharArray(msg, 200);
	StaticJsonDocument<384> receivedMsg;
	deserializeJson(receivedMsg, msg);

	JsonArray sel = receivedMsg["sel"];

	bool forThisDevice = sel[ESP_NO - 1];
	Serial.print(forThisDevice);
	Serial.print(" HMMMMMMM\n");
	if (FOR_THIS_ESP != forThisDevice)
	{
		Serial.print("FOOOOOR THIS ESP!!!!!!\n");
		FOR_THIS_ESP = forThisDevice;
	}
	IDLETIME = 0;
}


void onRxCallback(const String& message) {
	IDLETIME = 0;
}
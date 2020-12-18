// 
// 
// 

#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"
#include "Arduino.h"
#include "Esp.h"

void ledRoutine() {
	static int oldBrightness = matrix->getBrightness();
	if (oldBrightness != BRIGHTNESS) {
		matrix->setBrightness(BRIGHTNESS);
	}
	static LEDState oldState = CurrentState;
	if (CurrentState != oldState)
	{
		matrix->clear();
	}
 FastLED.setBrightness(BRIGHTNESS);

	switch (CurrentState)
	{
	case LightOff:
		turnOffLight();
		break;
	case TextGenerator:
		generateText();
		break;
	case TapToLight:
		tapPixels();
		break;
	case Drumpad:
		launchDrumpad();
		break;
	case LightShow:
		launchLightShow();
		break;
	default:
		turnOffLight();
		break;
	}
}

void turnOffLight() {
	static long thisLastTime = millis();
	static long now;
	now = millis();
	if (now - thisLastTime >= 1000)
	{
		clearArray();
		Serial.println("off");
		matrix->clear();
		matrix->show();
		thisLastTime = now;
		digitalWrite(2, LOW);
	}
}

void generateText() {
	static long thisLastTime = millis();
	static long now;
	static int x = matrix->width();
	// Using polling timer! avoid using delay!
	now = millis();
	static byte takt;
	takt = constrain(TXT_SPEED, 50, 255);
	if (now - thisLastTime >= takt) {
		thisLastTime = now;
		matrix->fillScreen(0);
		matrix->setCursor(x, 0);
		matrix->print(TXT_TEXT);
		int n = (-6 * TXT_TEXT.length());
		if (--x < n) {
			x = matrix->width();
			matrix->setTextColor(matrix->Color(TXT_COLOR[0], TXT_COLOR[1], TXT_COLOR[2]));
		}
		matrix->show();
	}
}

void tapPixels() {
	static long thisLastTime = millis();
	static long now;
	now = millis();
	if (now - thisLastTime >= 200)
	{
		matrix->clear();
		for (byte column = 0; column < 8; column++) {
			for (byte row = 0; row < 8; row++) {
				matrix->drawPixel(column, row, matrix->Color(PX_COLORS[column][row][0], PX_COLORS[column][row][1], PX_COLORS[column][row][2]));
			}
		}
		thisLastTime = now;
		matrix->show();
	}
}

void launchLightShow() {
	int select = 0;
	switch (select)
	{
	case 0x00:
		launchLightShow_1();
		break;
	case 0x01:
		launchLightShow_2();
		break;
	case 0x02:
		launchLightShow_3();
		break;
	case 0x03:
		launchLightShow_4();
		break;
	case 0x04:
		launchLightShow_5();
		break;
	case 0x05:
		launchLightShow_6();
		break;
	case 0x06:
		launchLightShow_7();
		break;
	case 0x07:
		launchLightShow_8();
		break;
	case 0x08:
		launchLightShow_9();
		break;
	case 0x09:
		launchLightShow_10();
		break;
	case 0x0A:
		launchLightShow_11();
		break;
	case 0x0B:
		launchLightShow_12();
		break;
	case 0x0C:
		launchLightShow_13();
		break;
	case 0x0D:
		launchLightShow_14();
		break;
	case 0x0E:
		launchLightShow_15();
		break;
	case 0x0F:
		launchLightShow_16();
		break;
	default:
		break;
	}
}

void clearArray() {
	for (byte column = 0; column < 8; column++) {
		for (byte row = 0; row < 8; row++) {
			PX_COLORS[row][column][0] = 0;
			PX_COLORS[row][column][1] = 0;
			PX_COLORS[row][column][2] = 0;
			PX_SELECT[row][column] = 0;
		}
	}

}

void setChipID() {
	static uint64_t chipid;
	chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
	MAC_ADR = String((uint16_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
	Serial.println(MAC_ADR);
}

// 
// 
// 

#include "8x8LEDHandler.h"
#include "Arduino.h"
#include "Esp.h"

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

void launchDrumpad() {

}

void launchLightShow() {

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

void setInitialValue()
{

}

void setChipID() {
	static uint64_t chipid;
	chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
	MAC_ADR = String((uint16_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
	Serial.println(MAC_ADR);
}

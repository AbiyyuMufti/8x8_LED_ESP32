#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"
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

typedef void(*patternList[])();
// TODO: using array pointer may be better
void launchLightShow() {
	static patternList lightShowPattern = {
		launchLightShow_1,
		launchLightShow_2,
		launchLightShow_3,
		launchLightShow_4,
		launchLightShow_5,
		launchLightShow_6,
		launchLightShow_7,
		launchLightShow_8,
		launchLightShow_9,
		launchLightShow_10,
		launchLightShow_11,
		launchLightShow_12,
		launchLightShow_13,
		launchLightShow_14,
		launchLightShow_15,
		launchLightShow_16
	};

	lightShowPattern[PATTERN]();
}


float max3point(float a, float b, float c) {
	if (a > b)
	{
		if (a > c) return a;
		else return c;
	}
	else
	{
		if (b > c) return b;
		else return c;
	}
}

float min3point(float a, float b, float c) {
	if (a < b)
	{
		if (a < c) return a;
		else return c;
	}
	else
	{
		if (b < c) return b;
		else return c;
	}
}

void calculateHSV(const byte& r, const byte& g, const byte& b, uint32_t& h, uint8_t& s, uint8_t& v) {
	float R = (float)r / 255, G = (float)g / 255, B = (float)b / 255;
	float cmin = min3point(R, G, B);
	float cmax = max3point(R, G, B);
	float diff = cmax - cmin;
	int hue;
	if (diff == 0) { hue = 0; }
	else
	{
		if (cmax == R) { hue = 60 * ((G - B) / diff); }
		if (cmax == G) { hue = 60 * (((B - R) / diff) + 2); }
		if (cmax == B) { hue = 60 * (((R - G) / diff) + 4); }
	}
	if (hue < 0) { hue += 360; }
	v = cmax * 255;
	s = (v == 0) ? 0 : 255 * diff / cmax;
	h = map(hue, 0, 360, 0, 65536);
}

void singleColorSeq0(const byte& r, const byte& g, const byte& b) {
	static long last = millis();
	if (millis() - last >= 50)
	{
		matrix->fillScreen(matrix->Color(r, g, b));
		matrix->show();
		last = millis();
	}
}

void singleColorSeq1(const byte& r, const byte& g, const byte& b) {
	static long last = millis();
	static byte brg = 0;
	static bool go_up = true;
	if (millis() - last >= 50)
	{
		matrix->fillScreen(matrix->Color(r, g, b));
		if (go_up)
		{
			brg++;
			if (brg >= 50) { go_up = !go_up; }
		}
		else
		{
			brg--;
			if (brg <= 0) { go_up = !go_up; }
		}
		matrix->setBrightness(brg);
		matrix->show();
		last = millis();
	}
}

void singleColorSeq2(const byte& r, const byte& g, const byte& b) {
	
		/*for (byte b = 0; b < 3; b++)
		{
			matrix->clear();
			// 'c' counts up from 'b' to end of strip in steps of 3...
			for (byte c = b; c < matrix->numPixels(); c += 3)
			{
				matrix->setPixelColor(c, matrix->Color(r, g, b)); // Set pixel 'c' to value 'color'
			}
			matrix->show(); // Update strip with new contents
			delay(100);  // Pause for a moment
		}*/
	
	static byte steps = 0;
	static long last = millis();
	if (millis() - last >= 75)
	{
		matrix->clear();
		for (byte c = steps; c < matrix->numPixels(); c += 3)
		{
			matrix->setPixelColor(c, matrix->Color(r, g, b)); // Set pixel 'c' to value 'color'
		}
		steps++;
		if (steps >= 3) { steps = 0; }
		matrix->show();
		last = millis();
	}
}

void singleColorSeq3(const byte& r, const byte& g, const byte& b) {
	uint32_t hue;
	uint8_t saturation, value;
	static int32_t steps = 0;
	static bool dir = false;
	static long last = millis();
	calculateHSV(r, g, b, hue, saturation, value);
	if (millis() - last >= 2)
	{
		for (int i = -32; i < matrix->numPixels() - 32; i++)
		{
			uint32_t pixelHue = steps + hue + (i * (65536/2) / matrix->numPixels());
			matrix->setPixelColor(i + 32, matrix->gamma32(matrix->ColorHSV(pixelHue, saturation, value)));
		}
		matrix->show();
		Serial.println(steps);
		if (dir)
		{
			steps += 32;
			if (steps >= 65536/8)
			{
				dir = !dir;
			}
		}
		else
		{
			steps -= 32;
			if (steps <= -1*(65536 / 8))
			{
				dir = !dir;
			}
		}

		last = millis();
	}

}

typedef void (*singleColorList[])(const byte& r, const byte& g, const byte& b);
singleColorList listSingleColor = { singleColorSeq0, singleColorSeq1, singleColorSeq2, singleColorSeq3 };

void launchSingleColor() {
	static byte r, g, b, seq;
	if (r != SINGLECOLOR.red || g != SINGLECOLOR.green || b != SINGLECOLOR.blue)
	{
		r = SINGLECOLOR.red;
		g = SINGLECOLOR.green;
		b = SINGLECOLOR.blue;
	}
	if (seq != SINGLECOLOR.sequence)
	{
		seq = SINGLECOLOR.sequence;
	}
	listSingleColor[seq](r, g, b);
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

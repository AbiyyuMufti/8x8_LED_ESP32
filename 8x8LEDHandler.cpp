#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"
#include "Arduino.h"
#include "Esp.h"
#include <SimpleKalmanFilter.h>

void turnOffLight() {
	static long thisLastTime = millis();
	static long now;
	now = millis();
	if (now - thisLastTime >= 100)
	{
		clearArray();
		SINGLECOLOR.blue = 0;
		SINGLECOLOR.green = 0;
		SINGLECOLOR.red = 0;
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
	now = millis();
	static byte takt;
	takt = constrain(TEXT.SPEED, 50, 255);
	if (now - thisLastTime >= takt) {
		thisLastTime = now;
		matrix->fillScreen(0);
		matrix->setCursor(x, 0);
		matrix->setTextColor(matrix->Color(TEXT.COLOR[0], TEXT.COLOR[1], TEXT.COLOR[2]));
		matrix->print(TEXT.TEXT);
		int n = (-6 * TEXT.TEXT.length());
		if (--x < n) {
			x = matrix->width();
		}
		matrix->show();
	}
}

void tapPixels() {
	static long thisLastTime = millis();
	static long now;
	now = millis();
	if (now - thisLastTime >= 100)
	{
		matrix->clear();
		for (byte column = 0; column < 8; column++) {
			for (byte row = 0; row < 8; row++) {
				matrix->drawPixel(column, row, matrix->Color(PIXELS.COLORS[column][row][0], PIXELS.COLORS[column][row][1], PIXELS.COLORS[column][row][2]));
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
			if (brg >= 100) { go_up = !go_up; }
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
	static byte steps = 0;
	static long last = millis();
	if (millis() - last >= 75)
	{
		matrix->clear();
		for (byte c = steps; c < 64; c += 3)
		{
			ledArray[c] = CRGB(r, g, b);
			//matrix->setPixelColor(c, matrix->Color(r, g, b)); // Set pixel 'c' to value 'color'
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
	if (millis() - last >= 20)
	{
		for (int i = -32; i < NUM_LEDS - 32; i++)
		{
			uint32_t pixelHue = steps + hue + i*2;
			//matrix->setPixelColor(i + 32, matrix->gamma32(matrix->ColorHSV(pixelHue, saturation, value)));
			ledArray[i + 32] = CHSV(pixelHue, saturation, value);
		}
		matrix->show();
		Serial.println(steps);
		if (dir)
		{
			steps++;
			if (steps >= 32){dir = !dir;}
		}
		else
		{
			steps--;
			if (steps <= -1*32){dir = !dir;}
		}
		last = millis();
	}

}

typedef void (*singleColorList[])(const byte& r, const byte& g, const byte& b);
singleColorList listSingleColor = { singleColorSeq0, singleColorSeq1, singleColorSeq2, singleColorSeq3 };

void launchSingleColor() {
	static byte r, g, b, seq = 0;
	if (seq != SINGLECOLOR.sequence)
	{
		seq = SINGLECOLOR.sequence;
	}
	listSingleColor[seq](SINGLECOLOR.red, SINGLECOLOR.green, SINGLECOLOR.blue);
}

void clearArray() {
	for (byte column = 0; column < 8; column++) {
		for (byte row = 0; row < 8; row++) {
			PIXELS.COLORS[row][column][0] = 0;
			PIXELS.COLORS[row][column][1] = 0;
			PIXELS.COLORS[row][column][2] = 0;
			PIXELS.SELECT[row][column] = 0;
		}
	}
}

void setInitialValue()
{
	CurrentState = LightShow;
	TEXT.COLOR[0] = 0;
	TEXT.COLOR[1] = 0;
	TEXT.COLOR[2] = 255;
	TEXT.SPEED = 100;
	TEXT.TEXT = String(ESP_NO);
	clearArray();
}

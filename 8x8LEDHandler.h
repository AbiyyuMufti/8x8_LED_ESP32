// 8x8LEDHandler.h

#ifndef _8X8LEDHANDLER_h
#define _8X8LEDHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define PIN_BATT 34
#define PIN_LDR 35
#define PIN_LED 2
#define WIDTH 8
#define HEIGHT 8

// Global Variables
extern String MAC_ADR;
extern String MAC_CHECK;
extern byte BRIGHTNESS;
extern String TXT_TEXT;
extern byte TXT_COLOR[3];
extern byte TXT_SPEED;
extern bool PX_SELECT[8][8];
extern byte PX_COLORS[8][8][3];
extern bool IS_ADAPTABLE_TO_LIGHT;
extern unsigned int BatteryState;
extern unsigned int LDRValue;

extern Adafruit_NeoMatrix *matrix;
extern EspMQTTClient *client;

extern byte ESP_NO;

enum LEDState
{
	Invalid,
	LightOff,
	TextGenerator,
	TapToLight,
	LightShow,
	SingleColor
};

struct SingleColorSetup
{
	byte red;
	byte green;
	byte blue;
	byte sequence;
};

extern struct SingleColorSetup SINGLECOLOR;
extern byte PATTERN;

extern LEDState CurrentState;

void brightnessControl();
void ledRoutine();

void setInitialValue();
void setChipID();

// function that drives sequenz of the led's lighting
void turnOffLight();
void generateText();
void tapPixels();
void launchLightShow();
void launchSingleColor();
void clearArray();


// callback function when massage arrive form the subscirbed topic
void onRxCommand(const String& message);
void onRxBrightness(const String& message);
void onRxTextGenerator(const String& message);
void onRxPixels(const String& message);
void onRxLightShow(const String& message);
void onTXState();
void sendESPStatus(uint32_t periode = 5000);


#endif


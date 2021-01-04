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
#include <FastLED_NeoMatrix.h>

#define PIN_BATT 34
#define PIN_LDR 35
#define PIN_LED 5
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)

// Global Variables

extern CRGBArray<NUM_LEDS> ledArray;

extern bool USE_LDR;
extern byte BRIGHTNESS;
extern bool IS_ADAPTABLE_TO_LIGHT;
extern uint32_t IDLETIME;

extern FastLED_NeoMatrix* matrix;
extern EspMQTTClient *client;

extern byte ESP_NO;
extern bool IN_SEQUENCE;
extern byte ORDER;
extern bool FOR_THIS_ESP;

enum LEDState
{
	Invalid,
	LightOff,
	TextGenerator,
	TapToLight,
	LightShow
};

extern LEDState CurrentState;

struct ESPState 
{
	unsigned int BatteryState;
	unsigned int LDRValue;
};

extern struct ESPState ESPINFO;

struct PixelsSetup
{
	byte COLORS[8][8][3];
};

extern struct PixelsSetup PIXELS;

struct TxtGeneratorSetup
{
	String TEXT;
	byte COLOR[3];
	byte SPEED;
};

extern struct TxtGeneratorSetup TEXT;

struct LightShowSetup {
	byte PATTERN;
	byte red;
	byte green;
	byte blue;
	byte SEQUENCE;
};

extern struct LightShowSetup LIGHTSHOW;


void brightnessControl();
void ledRoutine();
void checkSequence();
void setInitialValue();

// function that drives sequenz of the led's lighting
void turnOffLight();
void generateText();
void tapPixels();
void launchLightShow();
void clearArray();
void clearLightShow();


// callback function when massage arrive form the subscirbed topic
void onRxCommand(const String& message);
void onRxBrightness(const String& message);
void onRxTextGenerator(const String& message);
void onRxPixels(const String& message);
void onRxSetSequence(const String& message);
void onRxLightShow(const String& message);
void onRxESPSelect(const String& message);
void onTXState();
void sendESPStatus(uint32_t periode = 5000);

// energysaving functions
void device_go_to_sleep();

#endif


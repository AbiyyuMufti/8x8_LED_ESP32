
/*
    Name:       8x8_LED_ESP32.ino
*/

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <SimpleKalmanFilter.h>
#include <PID_v1.h>
#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"

#define ESPPOSITION 2 
#define BROKER "192.168.188.225"
#define SSID "FRITZ!Box 7590 VL"
#define PASS "56616967766283031728"


LEDState CurrentState;
bool USE_LDR = 0;
bool IS_ADAPTABLE_TO_LIGHT = false;
byte BRIGHTNESS = 20;
byte ESP_NO = ESPPOSITION;
bool IN_SEQUENCE = false;
byte ORDER = 1;
bool FOR_THIS_ESP = false;

struct ESPState ESPINFO;
struct PixelsSetup PIXELS;
struct TxtGeneratorSetup TEXT;
struct LightShowSetup LIGHTSHOW;

CRGBArray<64> ledArray;

FastLED_NeoMatrix* matrix = new FastLED_NeoMatrix(ledArray, WIDTH, HEIGHT, 1, 1,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);

char clientname[10] = { 'E', 'S', 'P', '3', '2', '-', char(ESPPOSITION + 64) };
EspMQTTClient *client = new EspMQTTClient(SSID, PASS, BROKER, clientname);


void onConnectionEstablished()
{
	client->subscribe("LED88ESP32/Command", onRxCommand);
	client->subscribe("LED88ESP32/ESPSelect", onRxESPSelect);
	client->subscribe("LED88ESP32/Brightness", onRxBrightness);
	client->subscribe("LED88ESP32/TextGenerator", onRxTextGenerator);
	client->subscribe("LED88ESP32/Pixels", onRxPixels);
	client->subscribe("LED88ESP32/LightShow", onRxLightShow);
	client->subscribe("LED88ESP32/PlayInSequence", onRxSetSequence);
}

void setup()
{
	Serial.begin(115200);
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LDR, INPUT);
	pinMode(PIN_BATT, INPUT);

	client->enableDebuggingMessages();
	client->setKeepAlive(60);	// Timeout 1 minute
	
	setInitialValue();

	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(BRIGHTNESS);
	setupFastLED();
}


void loop()
{
	client->loop();
	ledRoutine();
	checkSequence();
}

void ledRoutine() {
	brightnessControl();
	static LEDState oldState = CurrentState;
	if (CurrentState != oldState) {
		matrix->clear();
		oldState = CurrentState;
	}
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
	case LightShow:
		launchLightShow();
		break;
	default:
		turnOffLight();
		break;
	}
	sendESPStatus();
}

static String state[] = { "Invalid", "LightOff", "TextGenerator", "TapToLight", "LightShow" };
void checkSequence() {
	static long last = millis();
	static LEDState nextState;
	static bool once = false; 
	if (IN_SEQUENCE)
	{
		if (CurrentState != LightOff)
		{
			last = millis();
			nextState = CurrentState;
		}
		CurrentState = LightOff;
		if (millis() - last >= ORDER * 1000) {
			CurrentState = nextState;
			IN_SEQUENCE = false;
		}
	}
}
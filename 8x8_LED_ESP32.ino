
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

#define ESPPOSITION 1 
#define BROKER "192.168.0.73"
#define SSID "HomeSweetHome"
#define PASS "1bnAbdillah"

LEDState CurrentState = LightOff;
bool IS_ADAPTABLE_TO_LIGHT = false;
byte BRIGHTNESS = 10;
byte ESP_NO = ESPPOSITION;
bool IN_SEQUENCE = false;
bool FOR_THIS_ESP = false;

struct ESPState ESPINFO;
struct PixelsSetup PIXELS;
struct TxtGeneratorSetup TEXT;
struct LightShowSetup LIGHTSHOW;

CRGBArray<64> ledArray;

FastLED_NeoMatrix* matrix = new FastLED_NeoMatrix(ledArray, WIDTH, HEIGHT, 1, 1,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);

char clientname[10] = { 'E', 'S', 'P', '3', '2', '-', char(ESPPOSITION + 65) };
EspMQTTClient *client = new EspMQTTClient(SSID, PASS, BROKER, clientname);


void onConnectionEstablished()
{
	client->subscribe("LED88ESP32/Command", onRxCommand);
	client->subscribe("LED88ESP32/ESPSelect", onRxESPSelect);
	client->subscribe("LED88ESP32/Brightness", onRxBrightness);
	client->subscribe("LED88ESP32/TextGenerator", onRxTextGenerator);
	client->subscribe("LED88ESP32/Pixels", onRxPixels);
	client->subscribe("LED88ESP32/LightShow", onRxLightShow);
	client->subscribe("LED88ESP32/Sequence", onRxSetSequence);
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
}

void ledRoutine() {
	brightnessControl();
	static LEDState oldState = CurrentState;
	if (CurrentState != oldState) {
		matrix->clear();
		if (!IN_SEQUENCE)
		{
			clearArray();
			clearLightShow();
		}
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
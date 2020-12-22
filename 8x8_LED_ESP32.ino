
/*
    Name:       8x8_LED_ESP32.ino
*/

#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <SimpleKalmanFilter.h>
#include <FastLED.h>
#include <PID_v1.h>
#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"

#define ESPPOSITION 10 
#define BROKER "192.168.0.73"
#define SSID "HomeSweetHome"
#define PASS "1bnAbdillah"

Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN_LED,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
	NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
	NEO_GRB + NEO_KHZ800);

char clientname[10] = { 'E', 'S', 'P', '3', '2', '-', char(ESPPOSITION + 65) };
EspMQTTClient *client = new EspMQTTClient(SSID, PASS, BROKER, clientname);


LEDState CurrentState = LightOff;
bool IS_ADAPTABLE_TO_LIGHT = false;
byte BRIGHTNESS = 10;
byte ESP_NO = ESPPOSITION;
byte PATTERN = 0;
bool FOR_THIS_ESP = 0;

struct ESPState ESPINFO;
struct PixelsSetup PIXELS;
struct TxtGeneratorSetup TEXT;
struct SingleColorSetup SINGLECOLOR;


void onConnectionEstablished()
{
	client->subscribe("LED88ESP32/Command", onRxCommand);
	client->subscribe("LED88ESP32/ESPSelect", onRxESPSelect);
	client->subscribe("LED88ESP32/Brightness", onRxBrightness);
	client->subscribe("LED88ESP32/TextGenerator", onRxTextGenerator);
	client->subscribe("LED88ESP32/Pixels", onRxPixels);
	client->subscribe("LED88ESP32/LightShow", onRxLightShow);
	client->subscribe("LED88ESP32/SingleColor/setColor", onRxSingleColorSetColor);
	client->subscribe("LED88ESP32/SingleColor/setSequence", onRxSingleColorSetSequence);
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
	case SingleColor:
		launchSingleColor();
		break;
	default:
		turnOffLight();
		break;
	}
	sendESPStatus();
}
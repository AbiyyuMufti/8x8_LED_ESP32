
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

#define BROKER "192.168.0.73"// "mqtt.eclipse.org"
#define CLNAME "ESP32Cl"
#define SSID "Vodafone-ED3D"
#define PASS "qHRAAECEnaCP3Aae"

Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN_LED,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
	NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
	NEO_GRB + NEO_KHZ800);

EspMQTTClient *client = new EspMQTTClient(SSID, PASS, BROKER, CLNAME);

byte BRIGHTNESS = 10;
String TXT_TEXT = "Default";
byte TXT_COLOR[3] = { 0, 0, 0 };
byte TXT_SPEED = 100;
byte ESP_NO = 1;
String MAC_ADR = "";
String MAC_CHECK = "";
byte PATTERN = 15;
LEDState CurrentState = LightOff;
bool IS_ADAPTABLE_TO_LIGHT = false;
unsigned int BatteryState = 0;
unsigned int LDRValue = 0;

struct SingleColorSetup SINGLECOLOR;

bool PX_SELECT[8][8] = {};
byte PX_COLORS[8][8][3] = {};

void onConnectionEstablished()
{
	client->publish("LED88ESP32/Address", MAC_ADR);
	client->subscribe("LED88ESP32/Command", onRxCommand);
	client->subscribe("LED88ESP32/Brightness", onRxBrightness);
	client->subscribe("LED88ESP32/TextGenerator", onRxTextGenerator);
	client->subscribe("LED88ESP32/Pixels", onRxPixels);
	client->subscribe("LED88ESP32/LightShow", onRxLightShow);
	client->subscribe("LED88ESP32/SingleColor/setColor", onRxLightShow);
	client->subscribe("LED88ESP32/SingleColor/setSequence", onRxLightShow);
}

void setup()
{
	setInitialValue();
	Serial.begin(115200);
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LDR, INPUT);
	pinMode(PIN_BATT, INPUT);
	setChipID();
	client->enableDebuggingMessages();
	client->setKeepAlive(60);	// Timeout 1 minute

	CurrentState = TextGenerator;
	TXT_TEXT = MAC_ADR;
	TXT_COLOR[2] = 255;

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

// TODO: Struct for Global Informations
// TODO: Check MQTT Capability
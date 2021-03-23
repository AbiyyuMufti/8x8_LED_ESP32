
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

#define ESPPOSITION 16
#define BROKER "192.168.188.225"
#define SSID "FRITZ!Box 7590 VL"
#define PASS "56616967766283031728"
#define USINGLDRSENSOR true
#define DETECTBATTERY false

LEDState CurrentState;
bool USE_LDR = USINGLDRSENSOR;
bool DETECT_BAT = DETECTBATTERY;
bool IS_ADAPTABLE_TO_LIGHT = false;
byte BRIGHTNESS = 20;
uint32_t IDLETIME = 0;
byte ESP_NO = ESPPOSITION;
bool IN_SEQUENCE = false;
byte ORDER = 1;
bool FOR_THIS_ESP = false;
RTC_DATA_ATTR unsigned int bootCount = 0;

struct ESPState ESPINFO;
struct PixelsSetup PIXELS;
struct TxtGeneratorSetup TEXT;
struct LightShowSetup LIGHTSHOW;

CRGBArray<64> ledArray;

FastLED_NeoMatrix* matrix = new FastLED_NeoMatrix(ledArray, 8, 8, 1, 1,
	NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE);

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
	client->subscribe("LED88ESP32/WebActivity", onRxCallback);
}

void setup()
{
	Serial.begin(115200);
	print_wakeup_reason();
	print_wakeup_touchpad();
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LDR, INPUT);
	pinMode(PIN_BATT, INPUT);
	
	++bootCount;
	Serial.println("Boot number: " + String(bootCount));

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
	checkActivity();
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
	sendESPStatus(1000);
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

void checkActivity() {
#define IN_MINUTE(X) X * 60
	static unsigned long lastTime = millis();
	static uint32_t connectionTime = 0;
	static bool once = false;
	if (millis() - lastTime >= 1000)
	{
		Serial.print("connectionTime: ");
		Serial.print(connectionTime);
		Serial.print(" IDLETIME: ");
		Serial.print(IDLETIME);
		Serial.print(" state: ");
		Serial.println(state[CurrentState]);
		
		connectionTime++;
		IDLETIME++;
		lastTime = millis();
		
		
	}
	// device go to sleep when:
	// 1. command to go to sleep
	// 2. no activity and connection after specified time.

	// go to sleep after 1 min of idle time (not connected to web apps)
	if (IDLETIME >= IN_MINUTE(1))
	{
		beforesleep();
		device_go_to_sleep(30);
	}

	if (client->isConnected())
	{
		if (once)
		{
			once = false;
			setInitialValue();
		}
		connectionTime = 0;
	}
	else
	{
		if (connectionTime >= 30)
		{
			once = true;
			CurrentState = TextGenerator;
			TEXT.TEXT = String(16) + String(" is not connected");
			TEXT.SPEED = 100;
		}
	}
}
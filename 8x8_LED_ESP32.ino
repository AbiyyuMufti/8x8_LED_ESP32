
/*
    Name:       8x8_LED_ESP32.ino
*/

// Define User Types below here or use a .h file
//
#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <FastLED.h>
#include "8x8LEDHandler.h"
#include "8x8_LED_Seq.h"


// Define Function Prototypes that use User Types below here or use a .h file
//
#define PIN_BATT 34
#define PIN_LDR 35
#define PIN_LED 2
#define WIDTH 8
#define HEIGHT 8
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS (WIDTH * HEIGHT)
#define FRAMES_PER_SECOND  120


// Define Function Prototypes that use User Types below here or use a .h file
//

// Define Functions below here or use other .ino or cpp files
//
byte BRIGHTNESS = 2;
String TXT_TEXT = "Default";
byte TXT_COLOR[3] = { 0, 0, 0 };
byte TXT_SPEED = 100;
String MAC_ADR = "";
String MAC_CHECK = "";
LEDState CurrentState = LightOff;

bool PX_SELECT[8][8] = {
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0},
	 {0, 0, 0, 0, 0, 0, 0, 0}
};
byte PX_COLORS[8][8][3] = {
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
	{{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
};

const char* broker = "mqtt.eclipse.org";  // MQTT Broker server ip
const char* ssid = "HomeSweetHome";
const char* password = "1bnAbdillah";
const char* clientName = "ESPTESTClient";

Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN_LED,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
	NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
	NEO_GRB + NEO_KHZ800);

EspMQTTClient *client = new EspMQTTClient(ssid, password, broker, clientName);

void onConnectionEstablished()
{
	client->subscribe("LED88ESP32/Command", onRxCommand);
	client->subscribe("LED88ESP32/Brightness", onRxBrightness);
	client->subscribe("LED88ESP32/TextGenerator", onRxTextGenerator);
	client->subscribe("LED88ESP32/Pixels", onRxPixels);
}

void setFastLED() {
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Launchpad 1,3,4,5,6
	FastLED.addLeds<CHIPSET, PIN_LED, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
	FastLED.setBrightness(BRIGHTNESS);
	//Launchpad 2
	//FastLED.addLeds<NEOPIXEL,2>(leds_Func_2, NUM_LEDS);
	//Launchpad 7
	// Initialize our coordinates to some random values
	x = random16();
	y = random16();
	z = random16();
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Launchpad 8
	FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Launchpad 11
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
	///////////////////////////////////////////////////////////////////////////////////////////////////// 
}

void setup()
{
	Serial.begin(115200);
	setChipID();
	client->enableDebuggingMessages();
	client->setKeepAlive(60);	// Timeout 1 minute
	pinMode(2, OUTPUT);
	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(BRIGHTNESS);
	CurrentState = TextGenerator;
	TXT_TEXT = MAC_ADR;
	TXT_COLOR[2] = 255;
	setFastLED();
	
}

// Add the main program code into the continuous loop() function
void loop()
{
	//client->loop();
	//ledRoutine();
	launchLightShow_1();
	//Launchpad_3_Loop();
	//Launchpad_2_Loop();
	//Launchpad_4_Loop();
	//Launchpad_5_Loop();
}

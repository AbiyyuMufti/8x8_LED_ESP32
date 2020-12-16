#ifndef _8X8_LED_SEQ_h
#define _8X8_LED_SEQ_h

#include <FastLED.h>

byte BRIGHTNESS = 2;
#define PIN_BATT 34
#define PIN_LDR 35
#define PIN_LED 2
#define WIDTH 8
#define HEIGHT 8
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS (WIDTH * HEIGHT)
#define FRAMES_PER_SECOND  120

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN_LED, 
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);
  
// Global Variables
int brightness = 100;
int color[3] = {0, 0, 255};

/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 1 & 7
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);
// Param for different pixel layouts
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( y & 0x01) {
    // Odd rows run backwards
    uint8_t reverseX = (WIDTH - 1) - x;
    i = (y * WIDTH) + reverseX;
  } else {
    // Even rows run forwards
    i = (y * WIDTH) + x;
  } 
  return i;
}
uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= WIDTH) return -1;
  if( y >= HEIGHT) return -1;
  return XY(x,y);
}
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 2
CRGBArray<NUM_LEDS> leds_Func_2;
////////////////////////////////////////////
//Setting Parameters for Launchpad Func 3
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3
///////////////////////////////////////////
//Setting Parameters for Launchpad Func 5
//#define FRAMES_PER_SECOND  120
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 6
bool gReverseDirection = false;
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 7
// Params for width and height
const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;
uint16_t speed = 20;
uint16_t scale = 311;
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 8
#define FASTLED_ALLOW_INTERRUPTS 0
#define MAX_POWER_MILLIAMPS 500
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 11
#define UPDATES_PER_SECOND 100
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
////////////////////////////////////////////////////////////////


void launchLightShow_1();
void launchLightShow_2();
void launchLightShow_3();
void launchLightShow_4();
void launchLightShow_5();
void launchLightShow_6();
void launchLightShow_7();
void launchLightShow_8();
void launchLightShow_9();
void launchLightShow_10();
void launchLightShow_11();
void launchLightShow_12();
void launchLightShow_13();
void launchLightShow_14();
void launchLightShow_15();
void launchLightShow_16();





#endif

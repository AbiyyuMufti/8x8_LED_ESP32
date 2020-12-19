#ifndef _8X8_LED_SEQ_h
#define _8X8_LED_SEQ_h

#include <FastLED.h>
#include "8x8LEDHandler.h"

#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS (8 * 8)
#define FRAMES_PER_SECOND  120

/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 2
// CRGBArray<NUM_LEDS> leds_Func_2;
////////////////////////////////////////////
//Setting Parameters for Launchpad Func 3
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 8
#define FASTLED_ALLOW_INTERRUPTS 0
#define MAX_POWER_MILLIAMPS 500
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 11
#define UPDATES_PER_SECOND 100


void setupFastLED();

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

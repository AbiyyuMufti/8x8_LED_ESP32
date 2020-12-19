#include "8x8_LED_Seq.h"
#include "8x8LEDHandler.h"

//Setting Parameters for Launchpad Func 6
bool gReverseDirection = false;


uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);


///////////////////////////////////////////
//Setting Parameters for Launchpad Func 5
//#define FRAMES_PER_SECOND  120
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 6
// bool gReverseDirection = false;
///////////////////////////////////////////
/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 7
// Params for width and height
#define kMatrixWidth 8
#define kMatrixHeight 8
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
#define kMatrixSerpentineLayout 1
// The 32bit version of our coordinates

uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];

CRGBPalette16 myRedWhiteBluePalette;
//const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
//Setting Parameters for Launchpad Func 1 & 7
struct CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
struct CRGB* const leds(leds_plus_safety_pixel + 1);
// Param for different pixel layouts

static uint16_t x, y, z;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setupFastLED()
{
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
}

uint16_t XY(uint8_t x, uint8_t y)
{
	uint16_t i;
	if (y & 0x01) {
		// Odd rows run backwards
		uint8_t reverseX = (WIDTH - 1) - x;
		i = (y * WIDTH) + reverseX;
	}
	else {
		// Even rows run forwards
		i = (y * WIDTH) + x;
	}
	return i;
}

uint16_t XYsafe(uint8_t x, uint8_t y)
{
	if (x >= WIDTH) return -1;
	if (y >= HEIGHT) return -1;
	return XY(x, y);
}


void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    byte lineStartHue = startHue8;
    for (byte y = 0; y < HEIGHT; y++) {
        lineStartHue += yHueDelta8;
        byte pixelHue = lineStartHue;
        for (byte x = 0; x < WIDTH; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}

/*---------------------- Light Show 1 ----------------------*/
///////////////////////////////////////////////
// Launchpad Func 1 Starts here
void launchLightShow_1()
{
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / WIDTH));
    int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / HEIGHT));
    DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if (ms < 5000) {
        FastLED.setBrightness(scale8(BRIGHTNESS, (ms * 256) / 5000));
    }
    else {
        FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}

CRGBArray<NUM_LEDS> leds_Func_2;
/*---------------------- Light Show 2 ----------------------*/
////////////////////////////////////////////////
// Launchpad Func 2 Starts here
void launchLightShow_2()
{
    static uint8_t hue;
    EVERY_N_MILLIS(33) {
        for (int i = 0; i < NUM_LEDS / 2; i++) {
            // fade everything out
            leds_Func_2.fadeToBlackBy(40);

            // let's set an led value
            leds_Func_2[i] = CHSV(hue++, 255, 255);

            // now, let's first 20 leds to the top 20 leds, 
            leds_Func_2(NUM_LEDS / 2, NUM_LEDS - 1) = leds_Func_2(NUM_LEDS / 2 - 1, 0);
            //FastLED.delay(33);
        }
    }
}

////////////////////////////////////////////////
// Launchpad Func 2 Starts here
void launchLightShow_3()
{
    EVERY_N_MILLIS(8) {
        // draw a generic, no-name rainbow
        static uint8_t starthue = 0;
        fill_rainbow(leds + 5, NUM_LEDS - 5, --starthue, 20);

        // Choose which 'color temperature' profile to enable.
        uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
        if (secs < DISPLAYTIME) {
            FastLED.setTemperature(TEMPERATURE_1); // first temperature
            leds[0] = TEMPERATURE_1; // show indicator pixel
        }
        else {
            FastLED.setTemperature(TEMPERATURE_2); // second temperature
            leds[0] = TEMPERATURE_2; // show indicator pixel
        }

        // Black out the LEDs for a few secnds between color changes
        // to let the eyes and brains adjust
        if ((secs % DISPLAYTIME) < BLACKTIME) {
            memset8(leds, 0, NUM_LEDS * sizeof(CRGB));
        }

        FastLED.show();
        //FastLED.delay(8);
    }
}

////////////////////////////////////////////////
// Launchpad Func 4 Starts here
void fade4(){ 
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i].nscale8(250); 
    } 
}

void launchLightShow_4()
{
    static uint8_t hue = 0;
    Serial.print("x");
    // First slide the led in one direction
    EVERY_N_MILLIS(8) {
        for (int i = 0; i < NUM_LEDS; i++) {
            // Set the i'th led to red 
            leds[i] = CHSV(hue++, 255, 255);
            // Show the leds
            FastLED.show();
            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fade4();
            // Wait a little bit before we loop around and do it again
            //delay(10);
        }
    }
    Serial.print("x");
    EVERY_N_MILLIS(10) {
        // Now go in the other direction.  
        for (int i = (NUM_LEDS)-1; i >= 0; i--) {
            // Set the i'th led to red 
            leds[i] = CHSV(hue++, 255, 255);
            // Show the leds
            FastLED.show();
            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fade4();
            // Wait a little bit before we loop around and do it again
            //delay(10);
        }
    }
}

/*---------------------- Light Show 5 ----------------------*/
///////////////////////////////////////////////////////
// Laucnhpad Func 5 Starts here

// List of patterns to cycle through.  Each is defined as a separate function below.


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS - 1);
    leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++) {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

void nextPattern()
{
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void launchLightShow_5()
{
    EVERY_N_MILLIS(1000 / FRAMES_PER_SECOND) {
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[gCurrentPatternNumber]();

        // send the 'leds' array out to the actual LED strip
        FastLED.show();
        // insert a delay to keep the framerate modest
        //FastLED.delay(1000/FRAMES_PER_SECOND); 

        // do some periodic updates
        EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
        EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
    }
}

/*---------------------- Light Show 6 ----------------------*/

#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012()
{
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING) {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
        CRGB color = HeatColor(heat[j]);
        int pixelnumber;
        if (gReverseDirection) {
            pixelnumber = (NUM_LEDS - 1) - j;
        }
        else {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }
}

////////////////////////////////////////////////////////
// Laucnhpad Func 6 Starts here
void launchLightShow_6()
{
    EVERY_N_MILLIS(1000 / FRAMES_PER_SECOND) {
        Fire2012(); // run simulation frame

        FastLED.show(); // display this frame
        //FastLED.delay(1000 / FRAMES_PER_SECOND);
    }
}


/*---------------------- Light Show 7 ----------------------*/
uint16_t speed = 20;
uint16_t scale = 311;
////////////////////////////////////////////////////////
// Laucnhpad Func 7 Starts here
void fillnoise8() {
    for (int i = 0; i < MAX_DIMENSION; i++) {
        int ioffset = scale * i;
        for (int j = 0; j < MAX_DIMENSION; j++) {
            int joffset = scale * j;
            noise[i][j] = inoise8(x + ioffset, y + joffset, z);
        }
    }
    z += speed;
}

void launchLightShow_7() {
    static uint8_t ihue = 0;
    fillnoise8();
    for (int i = 0; i < kMatrixWidth; i++) {
        for (int j = 0; j < kMatrixHeight; j++) {
            // We use the value at the (i,j) coordinate in the noise
            // array for our brightness, and the flipped value from (j,i)
            // for our pixel's hue.
            leds[XY(i, j)] = CHSV(noise[j][i], 255, noise[i][j]);

            // You can also explore other ways to constrain the hue used, like below
            // leds[XY(i,j)] = CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]);
        }
    }
    ihue += 1;

    LEDS.show();
    // delay(10);
}

/*---------------------- Light Show 8 ----------------------*/

CRGBPalette16 pacifica_palette_1 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 =
{ 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
  0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

// Add one layer of waves into the led array
void pacifica_one_layer(CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
    uint16_t ci = cistart;
    uint16_t waveangle = ioff;
    uint16_t wavescale_half = (wavescale / 2) + 20;
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        waveangle += 250;
        uint16_t s16 = sin16(waveangle) + 32768;
        uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
        ci += cs;
        uint16_t sindex16 = sin16(ci) + 32768;
        uint8_t sindex8 = scale16(sindex16, 240);
        CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
        leds[i] += c;
    }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
    uint8_t basethreshold = beatsin8(9, 55, 65);
    uint8_t wave = beat8(7);

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
        wave += 7;
        uint8_t l = leds[i].getAverageLight();
        if (l > threshold) {
            uint8_t overage = l - threshold;
            uint8_t overage2 = qadd8(overage, overage);
            leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
        }
    }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i].blue = scale8(leds[i].blue, 145);
        leds[i].green = scale8(leds[i].green, 200);
        leds[i] |= CRGB(2, 5, 7);
    }
}


void pacifica_loop()
{
    // Increment the four "color index start" counters, one for each wave layer.
    // Each is incremented at a different speed, and the speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    sLastms = ms;
    uint16_t speedfactor1 = beatsin16(3, 179, 269);
    uint16_t speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * speedfactor1) / 256;
    uint32_t deltams2 = (deltams * speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
    sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
    sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
    sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

    // Clear out the LED array to a dim background blue-green
    fill_solid(leds, NUM_LEDS, CRGB(2, 6, 10));

    // Render each of four layers, with different scales and speeds, that vary over time
    pacifica_one_layer(pacifica_palette_1, sCIStart1, beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130), 0 - beat16(301));
    pacifica_one_layer(pacifica_palette_2, sCIStart2, beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80), beat16(401));
    pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, beatsin8(9, 10, 38), 0 - beat16(503));
    pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, beatsin8(8, 10, 28), beat16(601));

    // Add brighter 'whitecaps' where the waves lines up more
    pacifica_add_whitecaps();

    // Deepen the blues and greens a bit
    pacifica_deepen_colors();
}

////////////////////////////////////////////////////////
// Laucnhpad Func 8 Starts here
void launchLightShow_8()
{
    EVERY_N_MILLISECONDS(20) {
        pacifica_loop();
        FastLED.show();
    }
}

/*---------------------- Light Show 9 ----------------------*/
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride()
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16;//gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (NUM_LEDS - 1) - pixelnumber;

        nblend(leds[pixelnumber], newcolor, 64);
    }
}

////////////////////////////////////////////////////////
// Laucnhpad Func 9 Starts here
void launchLightShow_9()
{
    pride();
    FastLED.show();
}

/*---------------------- Light Show 10 ----------------------*/

void DrawOneFrame2(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    byte lineStartHue = startHue8;
    for (byte y = 0; y < kMatrixHeight; y++) {
        lineStartHue += yHueDelta8;
        byte pixelHue = lineStartHue;
        for (byte x = 0; x < kMatrixWidth; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}


////////////////////////////////////////////////////////
// Laucnhpad Func 10 Starts here
void launchLightShow_10()
{
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
    DrawOneFrame2(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if (ms < 5000) {
        FastLED.setBrightness(scale8(BRIGHTNESS, (ms * 256) / 5000));
    }
    else {
        FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}


/*---------------------- Light Show 11 ----------------------*/

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for (int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV(random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green = CHSV(HUE_GREEN, 255, 255);
    CRGB black = CRGB::Black;

    currentPalette = CRGBPalette16(
        green, green, black, black,
        purple, purple, black, black,
        green, green, black, black,
        purple, purple, black, black);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.
void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if (lastSecond != secondHand) {
        lastSecond = secondHand;
        if (secondHand == 0) { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if (secondHand == 10) { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND; }
        if (secondHand == 15) { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if (secondHand == 20) { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if (secondHand == 25) { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if (secondHand == 30) { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if (secondHand == 35) { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if (secondHand == 40) { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if (secondHand == 45) { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if (secondHand == 50) { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND; }
        if (secondHand == 55) { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}


void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

////////////////////////////////////////////////////////
// Laucnhpad Func 11 Starts here
void launchLightShow_11()
{
    EVERY_N_MILLIS(1000 / FRAMES_PER_SECOND) {
        ChangePalettePeriodically();

        static uint8_t startIndex = 0;
        startIndex = startIndex + 1; /* motion speed */

        FillLEDsFromPaletteColors(startIndex);

        FastLED.show();
        // FastLED.delay(1000 / UPDATES_PER_SECOND);
    }
}


/*---------------------- Light Show 12-16 ----------------------*/
void pre_start(int(*logo)[8]) {
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            if (logo[row][column] == 1) {
                matrix->drawPixel(column, row, matrix->Color(0, 255, 0));
            }
        }
    }
}

/*---------------------- Light Show 12 ----------------------*/

void arrow(int(*logo)[8], int delay_time) {
    //matrix->drawPixel(0, 0, matrix->Color(255,0,0)); 
    int column = 0;
    int row = 0;
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            matrix->drawPixel(column, 0, matrix->Color(0, 0, 255));
            if (logo[row][column] == 1) {
                matrix->drawPixel(column, row, matrix->Color(255, 0, 0));
            }
        }
        matrix->show();
        delay(25);
        matrix->drawPixel(column, 0, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 1, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 2, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 3, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 4, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 5, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 6, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 7, matrix->Color(0, 0, 0));
        //  matrix->drawPixel(column, 0, matrix->Color(0,0,0));
        delay(delay_time);
    }
}

void launchLightShow_12()
{
    int delay_time = 150;
    int logo[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0}
    };
    pre_start(logo);
    arrow(logo, delay_time);
}

/*---------------------- Light Show 13 ----------------------*/
void lock(int(*logo)[8], int delay_time)
{
    int column = 0;
    int row = 0;
    for (int column = 0; column < 8; column++)
    {
        for (int row = 0; row < 8; row++)
        {
            if (logo[row][column] == 1 || logo[row][column] == 2)
            {
                matrix->drawPixel(column, row, matrix->Color(255, 0, 0));
            }
            else
            {
                matrix->drawPixel(column, row, matrix->Color(0, 0, 0));
            }

        }
        matrix->show();

    }
    delay(delay_time);
    for (int column = 0; column < 8; column++)
    {
        for (int row = 0; row < 8; row++)
        {
            if (logo[row][column] == 1 || logo[row][column] == 3)
            {
                matrix->drawPixel(column, row, matrix->Color(0, 255, 0));
            }
            else
            {
                matrix->drawPixel(column, row, matrix->Color(0, 0, 0));
            }
        }
        matrix->show();
    }
    delay(delay_time);
}

void launchLightShow_13()
{
    int delay_time = 1000;
    int logo[8][8] = {
         {0, 3, 3, 0, 2, 2, 0, 0},
         {3, 0, 0, 1, 0, 0, 2, 0},
         {3, 0, 0, 1, 0, 0, 2, 0},
         {0, 0, 1, 1, 1, 1, 1, 1},
         {0, 0, 1, 0, 0, 0, 0, 1},
         {0, 0, 1, 0, 0, 0, 0, 1},
         {0, 0, 1, 0, 0, 0, 0, 1},
         {0, 0, 1, 1, 1, 1, 1, 1}
        };
    lock(logo, delay_time);
}

/*---------------------- Light Show 14 ----------------------*/

void sinus(int(*logo)[8], int delay_time) {
    //matrix->drawPixel(0, 0, matrix->Color(255,0,0)); 
    int column = 0;
    int row = 0;
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            matrix->drawPixel(column, 0, matrix->Color(0, 0, 255));
            if (logo[row][column] == 1) {
                matrix->drawPixel(column, row, matrix->Color(255, 0, 0));
            }
        }
        matrix->show();
        delay(0);
        matrix->drawPixel(column, 0, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 1, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 2, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 3, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 4, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 5, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 6, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 7, matrix->Color(0, 0, 0));
        //  matrix->drawPixel(column, 0, matrix->Color(0,0,0));
        delay(delay_time);
    }
}

void launchLightShow_14()
{
    int delay_time = 100;
    int logo[8][8] = {
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 1, 0, 0, 0, 0, 0},
         {0, 1, 0, 1, 0, 0, 0, 0},
         {1, 0, 0, 1, 0, 0, 0, 0},
         {0, 0, 0, 0, 1, 0, 0, 1},
         {0, 0, 0, 0, 1, 0, 1, 0},
         {0, 0, 0, 0, 0, 1, 0, 0}
        };
    pre_start(logo);
    sinus(logo, delay_time);
}

/*---------------------- Light Show 15 ----------------------*/
void steps(int(*logo)[8], int delay_time) {
    //matrix->drawPixel(0, 0, matrix->Color(255,0,0)); 
    int column = 0;
    int row = 0;
    for (int column = 0; column < 8; column++)
    {
        for (int row = 0; row < 8; row++)
        {
            matrix->drawPixel(column, 0, matrix->Color(0, 0, 255));
            if (logo[row][column] == 1)
            {
                matrix->drawPixel(column, row, matrix->Color(255, 0, 0));
            }
        }
        //  delay(1500);
        matrix->show();
        delay(75);

        //  matrix->drawPixel(column, 0, matrix->Color(0,0,0));

    }
    for (int column_delete = 0; column_delete < 8; column_delete++)
    {
        for (int row_delete = 0; row_delete < 8; row_delete++)
        {
            matrix->drawPixel(column_delete, row_delete, matrix->Color(0, 0, 0));
        }
    }
    delay(delay_time);
}

void launchLightShow_15()
{
    int delay_time = 100;
    int logo[8][8] = {
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 1},
         {0, 0, 0, 0, 0, 0, 1, 1},
         {0, 0, 0, 0, 0, 1, 1, 1},
         {0, 0, 0, 0, 1, 1, 1, 1},
         {0, 0, 0, 1, 1, 1, 1, 1},
         {0, 0, 1, 1, 1, 1, 1, 1},
         {0, 1, 1, 1, 1, 1, 1, 1}
    };
    steps(logo, delay_time);
}


/*---------------------- Light Show 16 ----------------------*/
void diamond(int(*logo)[8], int delay_time) {
    //matrix->drawPixel(0, 0, matrix->Color(255,0,0)); 
    int column = 0;
    int row = 0;
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            matrix->drawPixel(column, 0, matrix->Color(0, 0, 255));
            if (logo[row][column] == 1) {
                matrix->drawPixel(column, row, matrix->Color(0, 255, 0));
            }
        }
        matrix->show();
        delay(75);
        matrix->drawPixel(column, 0, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 1, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 2, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 3, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 4, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 5, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 6, matrix->Color(0, 0, 0));
        matrix->drawPixel(column, 7, matrix->Color(0, 0, 0));
        //  matrix->drawPixel(column, 0, matrix->Color(0,0,0));
        delay(delay_time);
    }
}


void launchLightShow_16()
{
    int delay_time = 150;
    int logo[8][8] = {
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 1, 1, 0, 0, 0},
         {0, 0, 1, 0, 0, 1, 0, 0},
         {0, 1, 0, 0, 0, 0, 1, 0},
         {1, 0, 0, 0, 0, 0, 0, 1},
         {0, 1, 0, 0, 0, 0, 1, 0},
         {0, 0, 1, 0, 0, 1, 0, 0},
         {0, 0, 0, 1, 1, 0, 0, 0}
    };
    pre_start(logo);
    diamond(logo, delay_time);
}
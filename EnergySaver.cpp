#include "8x8LEDHandler.h"
#include <SimpleKalmanFilter.h>
#include <PID_v1.h>
/*
e_mea: Measurement Uncertainty
e_est : Estimation Uncertainty
q : Process Noise
*/
SimpleKalmanFilter ldrFilter(10, 2, 0.01);
SimpleKalmanFilter batFilter(10, 2, 0.01);
unsigned int LDRValUnFil = 0;
unsigned int BattUnFil = 0;
unsigned int LDRValFil = 0, BattFil = 0;


void calculateESPState()
{
	BattUnFil = analogRead(PIN_BATT);
	LDRValUnFil = analogRead(PIN_LDR);

	BattFil = batFilter.updateEstimate(analogRead(PIN_BATT));
	LDRValFil = ldrFilter.updateEstimate(analogRead(PIN_LDR));
	if (USE_LDR)
	{
		ESPINFO.BatteryState = map(BattFil, 0, 4094, 0, 100);
		ESPINFO.LDRValue = map(LDRValFil, 0, 4094, 0, 100);
	}
	else
	{
		ESPINFO.BatteryState = 0;
		ESPINFO.LDRValue = 0;
	}
}

void brightnessControl() {
	static double Setpoint, Input, Output;
	static const double Kp = 0.1, Ki = 0.0001, Kd = 0;
	static PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

	static int oldBrightness = BRIGHTNESS;
	static bool called_once = true;
	
	calculateESPState();
	
	Input = (double)LDRValFil;
	if (IS_ADAPTABLE_TO_LIGHT && USE_LDR)
	{
		if (!called_once)
		{
			// Set to automatic
			Serial.println("Set to automatic");
			Setpoint = 10;
			myPID.SetMode(AUTOMATIC);
			myPID.SetOutputLimits(5, 255);
			called_once = true;
		}
		myPID.Compute();
		matrix->setBrightness((uint8_t)Output);
	}
	else
	{
		if (called_once)
		{
			Serial.println("Set to manual");
			myPID.SetMode(MANUAL);
			called_once = false;
			oldBrightness = BRIGHTNESS;
		}
		if (oldBrightness != BRIGHTNESS) {
			matrix->setBrightness(BRIGHTNESS);
			oldBrightness = BRIGHTNESS;
		}
	}
	// Uncomment to see the value inside !!!
	/*Serial.print("Akt: ");
	Serial.print(matrix->gamma8(matrix->getBrightness()));
	Serial.print(" Set: ");
	Serial.print(Setpoint);
	Serial.print(" In: ");
	Serial.print(Input);
	Serial.print(" Out: ");
	Serial.println(matrix->gamma8((uint8_t)Output));*/
	/*Serial.print("Bat - unfiltered: ");
	Serial.print(BattUnFil);
	Serial.print(" filtered: ");
	Serial.print(BattFil);
	Serial.print(" - ");
	Serial.print(ESPINFO.BatteryState);
	Serial.print(" % ");
	Serial.print("\tLDR - unfiltered: ");
	Serial.print(LDRValUnFil);
	Serial.print(" filterd: ");
	Serial.print(LDRValFil);
	Serial.print(" - ");
	Serial.print(ESPINFO.LDRValue);
	Serial.print(" % ");
	Serial.println();*/
}

void callback() {
	//placeholder callback function
}

void beforesleep() {
	String msg = "Going to sleep";
	int n = (-6 * msg.length());
	for (int j = 0; j < 2; j++)
	{
		for (int i = matrix->width(); i >= n; i--)
		{
			matrix->fillScreen(0);
			matrix->setCursor(i, 0);
			matrix->print(msg);
			matrix->setTextColor(matrix->Color(0, 0, 255));
			matrix->show();
			delay(100);
		}
	}
}

void device_go_to_sleep(uint16_t sleeptime_seconds) {
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */	
#define Threshold 50 /* Greater the value, more the sensitivity */	
	// turning off the LED
	matrix->clear();
	matrix->show();
	
	// configure the wake up source
	if (sleeptime_seconds != 0)
	{
		Serial.print("Sleep time: ");
		Serial.println(sleeptime_seconds);
		delay(5000);
		//// wake up with timer
		esp_sleep_enable_timer_wakeup(sleeptime_seconds * uS_TO_S_FACTOR);
	}
	
	// sleep up with touchpad
	touchAttachInterrupt(T2, callback, Threshold);
	esp_sleep_enable_touchpad_wakeup();

	// go to deep sleep
	Serial.println("Going To Sleep");
	esp_deep_sleep_start();
}

void print_wakeup_reason() {
	esp_sleep_wakeup_cause_t wakeup_reason;

	wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason)
	{
	case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
	case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
	case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
	case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
	case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
	default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
	}
}

void print_wakeup_touchpad() {
	touch_pad_t touchPin = esp_sleep_get_touchpad_wakeup_status();

	switch (touchPin)
	{
	case 0: Serial.println("Touch detected on GPIO 4"); break;
	case 1: Serial.println("Touch detected on GPIO 0"); break;
	case 2: Serial.println("Touch detected on GPIO 2"); break;
	case 3: Serial.println("Touch detected on GPIO 15"); break;
	case 4: Serial.println("Touch detected on GPIO 13"); break;
	case 5: Serial.println("Touch detected on GPIO 12"); break;
	case 6: Serial.println("Touch detected on GPIO 14"); break;
	case 7: Serial.println("Touch detected on GPIO 27"); break;
	case 8: Serial.println("Touch detected on GPIO 33"); break;
	case 9: Serial.println("Touch detected on GPIO 32"); break;
	default: Serial.println("Wakeup not by touchpad"); break;
	}
}
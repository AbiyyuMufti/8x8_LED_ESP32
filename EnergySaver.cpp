#include "8x8LEDHandler.h"
#include <SimpleKalmanFilter.h>

/*
e_mea: Measurement Uncertainty
e_est : Estimation Uncertainty
q : Process Noise
*/
SimpleKalmanFilter ldrFilter(10, 2, 0.01);
SimpleKalmanFilter batFilter(10, 2, 0.01);

void brightnessControl() {
	static int oldBrightness = matrix->getBrightness();
	// static unsigned int LDRValUnFil = 0;
	// static unsigned int BattUnFil = 0;
	BatteryState = batFilter.updateEstimate(analogRead(PIN_BATT));
	// BattUnFil = analogRead(PIN_BATT);
	LDRValue = ldrFilter.updateEstimate(analogRead(PIN_LDR));
	// LDRValUnFil = analogRead(PIN_LDR);
	if (oldBrightness != BRIGHTNESS) {
		matrix->setBrightness(BRIGHTNESS);
	}
	/*Serial.print("Bat - unfiltered: ");
	Serial.print(BattUnFil);
	Serial.print(" filtered: ");
	Serial.print(BatteryState);
	Serial.print("\tLDR - unfiltered: ");
	Serial.print(LDRValUnFil);
	Serial.print(" filterd: ");
	Serial.println(LDRValue);*/
}

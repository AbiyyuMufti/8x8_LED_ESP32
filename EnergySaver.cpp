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

void brightnessControl() {
	//Define Variables we'll be connecting to
	static double Setpoint, Input, Output;
	//Specify the links and initial tuning parameters
	static const double Kp = 0.1, Ki = 0.0001, Kd = 0;
	static PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);
	static int oldBrightness = matrix->getBrightness();
	static unsigned int LDRValUnFil = 0;
	static unsigned int BattUnFil = 0;
	static bool called_once = true;
	BatteryState = batFilter.updateEstimate(analogRead(PIN_BATT));
	BattUnFil = analogRead(PIN_BATT);
	LDRValue = ldrFilter.updateEstimate(analogRead(PIN_LDR));
	LDRValUnFil = analogRead(PIN_LDR);
	Input = (double)LDRValue;
	if (IS_ADAPTABLE_TO_LIGHT)
	{
		if (!called_once)
		{
			// Set to automatic
			Serial.println("Set to automatic");
			Setpoint = 15;
			myPID.SetMode(AUTOMATIC);
			myPID.SetOutputLimits(1, 255);
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
			oldBrightness = matrix->getBrightness();
		}
		if (oldBrightness != BRIGHTNESS) {
			matrix->setBrightness(BRIGHTNESS);
		}
	}
	// Uncomment to see the value inside !!!
	/*Serial.print("Akt: ");
	Serial.print(matrix->getBrightness());
	Serial.print(" Set: ");
	Serial.print(Setpoint);
	Serial.print(" In: ");
	Serial.print(Input);
	Serial.print(" Out: ");
	Serial.println(Output);*/
	/*Serial.print("Bat - unfiltered: ");
	Serial.print(BattUnFil);
	Serial.print(" filtered: ");
	Serial.print(BatteryState);
	Serial.print("\tLDR - unfiltered: ");
	Serial.print(LDRValUnFil);
	Serial.print(" filterd: ");
	Serial.println(LDRValue);*/
}

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

	ESPINFO.BatteryState = map(BattFil, 0, 4094, 0, 100);
	ESPINFO.LDRValue = map(LDRValFil, 0, 4094, 0, 100);
}

void brightnessControl() {
	static double Setpoint, Input, Output;
	static const double Kp = 0.1, Ki = 0.0001, Kd = 0;
	static PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

	static int oldBrightness = BRIGHTNESS;
	static bool called_once = true;
	
	calculateESPState();
	
	Input = (double)LDRValFil;
	if (IS_ADAPTABLE_TO_LIGHT)
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

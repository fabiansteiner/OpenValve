/*
* SoilMoistureSensor.c
*
* Created: 06.11.2026 18:38:41
*  Author: SteinerF
*/ 

 
#include "SoilMoistureSensor.h"

// ADC thresholds for VWC:
// [12, 16, 20, 24, 28, 32, 36, 40] %

#if SELECTED_SENSOR == PinoTech_SoilWatch10

	#pragma message("Compiling with Sensor: PinoTech_SoilWatch10")

	static const uint16_t adc_thresholds[8] = {
		378, 488, 577, 649, 709, 759, 803, 843
	};

#elif SELECTED_SENSOR == Truebner_SMT50

	#pragma message("Compiling with Sensor: Truebner_SMT50")

	static const uint16_t adc_thresholds[8] = {
		223, 298, 372, 446, 521, 595, 670, 744
	};

//Define the adc thresholds for the custom sensor here:
/*
#elif SELECTED_SENSOR == CUSTOM_SENSOR

	#pragma message("Compiling with Sensor: CUSTOM_SENSOR")

	static const uint16_t adc_thresholds[8] = {
		100, 200, 300, 400, 500, 600, 700, 800
	};
*/

#endif

uint16_t getSensorThreshold(uint8_t sensorLevel)
{
	// Valid sensorLevel indices: 0 ... 7
	return adc_thresholds[sensorLevel];
}

uint8_t calculateCurrentSoilMoistureLevel(uint16_t soilMoistureADC, uint8_t valveOpen)
{
	for(uint8_t i = 0; i< 8; i++){
		if(soilMoistureADC < adc_thresholds[i] + valveOpen * 25){
			return i+1;
		}
	}
	return 9;
}

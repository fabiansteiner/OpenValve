/*
 * SoilMoistureSensor.h
 *
 * Created: 02.11.2022 16:19:14
 *  Author: SteinerF
 *
 * This file contains the characteristics of different soil moisture sensors.
 * The right sensor must be chosen before compilation.
 * 
 */ 


#ifndef SOILMOISTURESENSOR_H_
#define SOILMOISTURESENSOR_H_



#define PinoTech_SoilWatch10 10
#define Truebner_SMT50 20

//Choose your soil moisture sensor:
#define SELECTED_SENSOR PinoTech_SoilWatch10

// Define function prototype
//uint16_t getSensorTreshold(uint8_t sensorLevel);


//Definition of sensor specific characteristics:

#if SELECTED_SENSOR == PinoTech_SoilWatch10
	#pragma message("Compiling with Sensor: PinoTech_SoilWatch10")
	#define STARTUP_TIME 5

	//These adc_treshold values represent the following volumetric soil content values of the SoilWatch 10 Sensor: [12,16,20,24,28,32,36,40]%
	static const uint16_t adc_trehsolds[8] = {378,488,577,649,709,759,803,843};

	static uint16_t getSensorTreshold(uint8_t sensorLevel) {
		return adc_trehsolds[sensorLevel]; 
	}

	static uint8_t calculateCurrentSoilMoistureLevel(uint16_t soilMoistureADC){
		for(uint8_t i = 0; i< 8; i++){
			if(soilMoistureADC < adc_trehsolds[i]){
				return i+1;
			}
		}
		return 9;
	}
	


#elif SELECTED_SENSOR == Truebner_SMT50
	#pragma message("Compiling with Sensor: Truebner_SMT50")
	#define STARTUP_TIME 300

	#define ADCSTEPS 93		//Because 3.0V = 930/10 = 93

	static inline uint16_t getSensorTreshold(uint8_t sensorLevel) {
		return sensorLevel * ADCSTEPS;
	}

#else
	#error "Unknown option selected"

#endif

#endif /* SOILMOISTURESENSOR_H_ */
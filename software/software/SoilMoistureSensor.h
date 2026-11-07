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

#include <stdint.h>

// Available sensors
#define PinoTech_SoilWatch10 10
#define Truebner_SMT50     20
//#define CUSTOM_SENSOR     30		// <-- define a custom sensor here

//Choose your soil moisture sensor:
#define SELECTED_SENSOR PinoTech_SoilWatch10

// Sensor-specific startup time
#if SELECTED_SENSOR == PinoTech_SoilWatch10
	#define STARTUP_TIME 10

#elif SELECTED_SENSOR == Truebner_SMT50
	#define STARTUP_TIME 300

// Choose startup time (how much milliseconds until the sensor outputs a stable measurement?) Should be below 500ms, otherwise the logic in main does not work

//#elif SELECTED_SENSOR == CUSTOM_SENSOR
//	#define STARTUP_TIME 444

#else
	#error "Unknown soil moisture sensor selected"
#endif


uint16_t getSensorThreshold(uint8_t sensorLevel);

uint8_t calculateCurrentSoilMoistureLevel(uint16_t soilMoistureADC,uint8_t valveOpen);

#endif /* SOILMOISTURESENSOR_H_ */
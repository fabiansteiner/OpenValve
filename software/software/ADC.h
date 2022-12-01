/*
 * ADC.h
 *
 * Created: 02.11.2022 16:19:57
 *  Author: SteinerF
 *
 * Low level hardware access to ADC, used by SoilMoisureSensor.h & valve.h because of current sensing + Battery Sensing
 * States: readSoilMoisture, readCurrent, sleeping
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define PIN_CURRENTSENSING 3		//PORTA
#define PIN_SOILSENSOR 1		//PORTA
#define PIN_SOILSENSORON 2		//PORTA

#define RES_10BIT 0x3FF
#define PIN_CURRENTSENS_CHANNEL 3

typedef uint16_t adc_result_t;
typedef ADC_MUXPOS_t adc_0_channel_t;
typedef enum {CURRSENSING = 1, MEASURESOIL = 2}  ADCstate;

void initADC();

void ADC_0_startMotorCurrentCheck();

uint16_t ADC_0_readSoilMoisture();





#endif /* ADC_H_ */
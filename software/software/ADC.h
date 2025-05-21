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
#define PIN_BATTERYSENSING 4		//PORTA

#define RES_10BIT 0x3FF
#define PIN_CURRENTSENS_CHANNEL 3

//For Battery Level Map Function
#define INPUT_START 620    // The lowest number of the range input.
#define INPUT_END 1023    // The largest number of the range input.
#define OUTPUT_START 2 // The lowest number of the range output.
#define OUTPUT_END 48  // The largest number of the range output.
#define BATTERY_LEVEL_LOW_ADC (uint16_t)(RES_10BIT / MAX_VOL * 8.0)
#define BATTERY_LEVEL_MEDIUM_ADC (uint16_t)(RES_10BIT / MAX_VOL * 8.5)



typedef uint16_t adc_result_t;
typedef ADC_MUXPOS_t adc_0_channel_t;
typedef enum {FREE = 1, OCCUPIED = 0}  ADCstate;

extern uint16_t currentSoilMoistureLevel;


void initADC();



uint16_t ADC_0_readSoilMoisture();

uint16_t ADC_0_readBatteryVoltage();

void prepareReadingCurrent(uint8_t shiftRight);

uint16_t ADC_0_readCurrent(uint8_t shiftRight);

uint16_t getADCValue(uint8_t prescaler, uint8_t accumulation, uint8_t ADC_pin);

uint8_t getBatteryLevel();
uint8_t getBatteryLevel3Indications();






#endif /* ADC_H_ */
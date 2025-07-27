/*
 * ADC.c
 *
 * Created: 30.11.2022 20:30:32
 *  Author: SteinerF
 */ 
#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "ADC.h"
#include "valve.h"
#include "SoilMoistureSensor.h"


uint16_t currentSoilMoistureLevel = 1;

//volatile uint8_t stateADC = FREE;

void initADC(){

	ADC0.CTRLC = 0 << ADC_SAMPCAP_bp; /* Sample Capacitance Selection: disabled */
	
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;	//Accumulate 8 Samples
	
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc  /* 10-bit mode */
	| 0 << ADC_RUNSTBY_bp; /* Run standby mode: disabled */
	
	PORTA_DIRSET = (1<<PIN_SOILSENSORON);
	
	
}

uint16_t read_adc_sample_accumulator(uint8_t shiftRight){
	return ADC0.RES >> shiftRight;

	//return ADC0.RES >> 6;	//Read ADC Result and devide by 64
	//return ADC0.RES >> 3;	//Read ADC Result and devide by 8
	//return ADC0.RES >> 0;	//Read ADC Result and devide by 1
}


uint16_t ADC_0_readBatteryVoltage(){

	//ADC0.CTRLC = 0;
	ADC0.SAMPCTRL = 31;
	return getADCValue(ADC_PRESC_DIV4_gc, ADC_SAMPNUM_ACC16_gc, PIN_BATTERYSENSING);
	
}

void prepareReadingCurrent(uint8_t shiftRight, uint8_t prescaler){
	ADC0.SAMPCTRL = 0;

	ADC0.CTRLC = prescaler								//CLK_PER divided by 16		
	| ADC_REFSEL_VDDREF_gc;										//VDD as reference
	ADC0.CTRLB = shiftRight;
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = PIN_CURRENTSENS_CHANNEL;										//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
}

uint16_t ADC_0_readCurrent(uint8_t shiftRight){

	//ADC0.CTRLC |= ADC_ASDV_ASVON_gc;
	//ADC0.SAMPCTRL = 0;
	//return getADCValue(ADC_PRESC_DIV4_gc, SAMPLE_ACCUM, PIN_CURRENTSENS_CHANNEL);
	uint16_t adc_result = 0;
	
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result = read_adc_sample_accumulator(shiftRight);			//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
	
	return adc_result;
}


uint16_t ADC_0_readSoilMoisture(){
	
	//Reset Sampling time
	ADC0.SAMPCTRL = 0;

	uint16_t adc_result_soil = 0;

	ADC0.CTRLC = ADC_PRESC_DIV4_gc								//CLK_PER divided by 16
	| ADC_REFSEL_VDDREF_gc;										//VDD as reference
	//ADC0.CTRLC = ADC_PRESC_DIV4_gc							/* CLK_PER divided by 16 */
	//| ADC_REFSEL_INTREF_gc;									/* Internal as reference */
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = PIN_SOILSENSOR;								//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
	
	//VREF_CTRLA = VREF_ADC0REFSEL_1V1_gc;						//Set internal ADC Reference to 1.1V
		
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result_soil = read_adc_sample_accumulator(ADC_SAMPNUM_ACC64_gc);									//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
		
	currentSoilMoistureLevel = calculateCurrentSoilMoistureLevel(adc_result_soil);
	
	return adc_result_soil;
	
}

/*
* prescaler: 
*
*/
uint16_t getADCValue(uint8_t prescaler, uint8_t accumulation, uint8_t ADC_pin){
	uint16_t adc_result = 0;
	
	
	ADC0.CTRLC = prescaler								//CLK_PER divided by 16
	| ADC_REFSEL_VDDREF_gc;										//VDD as reference
	ADC0.CTRLB = accumulation;
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = ADC_pin;										//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
	
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result = read_adc_sample_accumulator(accumulation);			//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
	
	
	return adc_result;
	
}
/*
uint8_t getBatteryLevel(){
	uint16_t adcVoltage = ADC_0_readBatteryVoltage();
	
	//Convert adc value to battery level between 2 and 48
	//Battery is on a voltage divider which divides voltage by 3. So if Battery is 9.9V, ADC will read 3.3V
	//Assumption: Battery is empty at 6V and full at 9,9V, which is an ADC Value between 620 and 930
	//Map ADC Values to battery level
	if(adcVoltage < 620){
		return OUTPUT_START;
	}else{
		return OUTPUT_START + ((adcVoltage - INPUT_START) * (OUTPUT_END - OUTPUT_START)) / (INPUT_END - INPUT_START);
	}
	
}
*/
uint8_t getBatteryLevel3Indications(){
	uint16_t adcVoltage = ADC_0_readBatteryVoltage();
	
	//Convert adc value to battery level 1-3
	//Battery is on a voltage divider which divides voltage by 3. So if Battery is 9.9V, ADC will read 3.3V
	//Assumption: Battery is LOW if <8.0V, moderate if between 8.0V and 8.5V and full if >8.5V
	//Map ADC Values to battery level
	if(adcVoltage < BATTERY_LEVEL_LOW_ADC){
		return 1;
	}else if (adcVoltage >= BATTERY_LEVEL_LOW_ADC && adcVoltage <= BATTERY_LEVEL_MEDIUM_ADC){
		return 2;
	}else{
		return 3;
	}
	
}






/*
ISR(ADC0_RESRDY_vect){
	//adc_result_current = read_adc_sample_accumulator();

	if(getValveState() == CLOSING){
		if(adc_result_current >= 1000){	//Was previously 1000
			PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
			setValveState(CLOSED);
			ADC0.CTRLA &= ~(1 << ADC_ENABLE_bp);
			
		}
	}else if(getValveState() == OPENING){
		if(adc_result_current >= 400){ //Was previously 400
			PORTB_OUTCLR = (1<<PIN_MOTORMINUS);
			setValveState(OPEN);
			ADC0.CTRLA &= ~(1 << ADC_ENABLE_bp);
			
		}
	}else{
		//Disable ADC
		ADC0.CTRLA &= ~(1 << ADC_ENABLE_bp);
		
		
	}
	
	
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}
*/

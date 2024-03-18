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


//volatile uint8_t stateADC = FREE;

void initADC(){

	ADC0.CTRLC = 0 << ADC_SAMPCAP_bp; /* Sample Capacitance Selection: disabled */
	
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;	//Accumulate 8 Samples
	
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc  /* 10-bit mode */
	| 0 << ADC_RUNSTBY_bp; /* Run standby mode: disabled */
	
	PORTA_DIRSET = (1<<PIN_SOILSENSORON);
	
	
}

uint16_t read_adc_sample_accumulator()
{
	return ADC0.RES >> 6;	//Read ADC Result and devide by 64
	//return ADC0.RES >> 3;	//Read ADC Result and devide by 8
	//return ADC0.RES >> 0;	//Read ADC Result and devide by 1
}


uint16_t ADC_0_readBatteryVoltage(){
	
	uint16_t adc_result_battery = 0;
	
	
	ADC0.CTRLC = ADC_PRESC_DIV2_gc								//CLK_PER divided by 16 
	| ADC_REFSEL_VDDREF_gc;										//VDD as reference 
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = PIN_BATTERYSENSING;							//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;							//Disable Interrupt Vector
		
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result_battery = read_adc_sample_accumulator();			//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
	
	
	return adc_result_battery;
	
}

uint16_t ADC_0_readCurrent(){
	
	uint16_t adc_result_current = 0;
	
	
	ADC0.CTRLC = ADC_PRESC_DIV2_gc								//CLK_PER divided by 16
	| ADC_REFSEL_VDDREF_gc;										//VDD as reference
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = PIN_CURRENTSENS_CHANNEL;							//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
	
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result_current = read_adc_sample_accumulator();			//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
	
	
	return adc_result_current;
	
}

uint16_t ADC_0_readSoilMoisture(){
	
	uint16_t adc_result_soil = 0;

	ADC0.CTRLC = ADC_PRESC_DIV2_gc								/* CLK_PER divided by 16 */
	| ADC_REFSEL_INTREF_gc;										/* VDD as reference */
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
	//ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
	ADC0.MUXPOS  = PIN_SOILSENSOR;								//Select channel
	//ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
		
	VREF_CTRLA = VREF_ADC0REFSEL_1V1_gc;						//Set internal ADC Reference to 1.1V
		
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
	adc_result_soil = read_adc_sample_accumulator();									//Read Result
	ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
		
	
	
	return adc_result_soil;
	
}

uint8_t getBatteryLevel(){
	uint16_t adcVoltage = ADC_0_readBatteryVoltage();
	
	//Convert adc value to battery level between 2 and 48
	//Battery is on a voltage divider which divides voltage by 3. So if Battery is 9V, ADC will read 3V
	//Assumption: Battery is empty at 6V and full at 9V, which is an ADC Value between 620 and 930
	//Map ADC Values to battery level
	if(adcVoltage > 930){
		return OUTPUT_END;
	}else if(adcVoltage < 620){
		return OUTPUT_START;
	}else{
		return OUTPUT_START + ((adcVoltage - INPUT_START) * (OUTPUT_END - OUTPUT_START)) / (INPUT_END - INPUT_START);
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

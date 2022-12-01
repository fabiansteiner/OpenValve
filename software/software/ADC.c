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
#include "ADC.h"
#include "valve.h"

volatile uint16_t adc_result_current = 0;
volatile uint16_t adc_result_soil = 0;
uint8_t stateADC = MEASURESOIL;

uint16_t read_adc_sample_accumulator()
{
	return ADC0.RES >> 6;	//Read ADC Result and devide by 64
}


void ADC_0_startMotorCurrentCheck()
{
	
	stateADC = CURRSENSING;
	ADC0.CTRLC = ADC_PRESC_DIV16_gc								/* CLK_PER divided by 16 */
	| ADC_REFSEL_VDDREF_gc;										/* VDD as reference */
	ADC0.CTRLA |= 1 << ADC_ENABLE_bp | 1 << ADC_FREERUN_bp;		//Enable ADC, Enable ADC Freerun mode
	ADC0.MUXPOS  = PIN_CURRENTSENS_CHANNEL;						//Select channel
	ADC0.INTCTRL = ADC_RESRDY_bm;								//Activate Interrupt Vector
	ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
}

uint16_t ADC_0_readSoilMoisture(){
	if(stateADC == MEASURESOIL){
		ADC0.CTRLC = ADC_PRESC_DIV16_gc								/* CLK_PER divided by 16 */
		| ADC_REFSEL_INTREF_gc;										/* VDD as reference */
		ADC0.CTRLA |= 1 << ADC_ENABLE_bp;							//Enable ADC
		ADC0.CTRLA &= ~(1 << ADC_FREERUN_bp);						//Disable ADC Freerun mode
		ADC0.MUXPOS  = PIN_SOILSENSOR;								//Select channel
		ADC0.INTCTRL &= ~ADC_RESRDY_bm;								//Disable Interrupt Vector
		
		VREF_CTRLA = VREF_ADC0REFSEL_1V1_gc;						//Set internal ADC Reference to 1.1V
		
		PORTA_OUTSET = (1<<PIN_SOILSENSORON);
		_delay_ms(300);
		
		ADC0.COMMAND = ADC_STCONV_bm;								//Start Conversion
		while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));					//Wait until conversion is done
		adc_result_soil = read_adc_sample_accumulator();									//Read Result
		ADC0.INTFLAGS = ADC_RESRDY_bm;								//Clear interrupt bit
		
		PORTA_OUTCLR = (1<<PIN_SOILSENSORON);						//Turn off transistor
	}
	
	return adc_result_soil;
	
}




void initADC(){

	ADC0.CTRLC = 0 << ADC_SAMPCAP_bp; /* Sample Capacitance Selection: disabled */
	
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;	//Accumulate 64 Samples
	
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc  /* 10-bit mode */
	| 0 << ADC_RUNSTBY_bp; /* Run standby mode: disabled */
	
	PORTA_DIRSET = (1<<PIN_SOILSENSORON); 
	
	
}


ISR(ADC0_RESRDY_vect){
	adc_result_current = read_adc_sample_accumulator();
	if(getValveState() != CLOSED){
		if(adc_result_current >= 1000){
			PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
			setValveState(CLOSED);
		}
	}else{
		//Disable ADC
		ADC0.CTRLA &= ~(1 << ADC_ENABLE_bp);
		stateADC = MEASURESOIL;
		
	}
	
	
	//ADC0.INTFLAGS = ADC_RESRDY_bm;
}

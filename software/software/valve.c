/*
 * valve.c
 *
 * Created: 30.11.2022 13:51:29
 *  Author: SteinerF
 */ 

#define F_CPU 3333333
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "valve.h"
#include "ADC.h"
#include "LEDs.h"

#define PA5_INTERRUPT PORTA.INTFLAGS & PIN5_bm
#define PA5_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS &= PIN5_bm



volatile valveState motState = OPEN;
volatile valveError error = NO_ERROR;

uint16_t voltageADC;
uint16_t currentADC;
volatile float calc_volt = 5.0;
float calc_curr;
float calc_watt;
float short_voltage;
float short_current;
float calc_resistance;
float short_resistance;
uint16_t timeCounter = 0;


void initializeValve(){
	
	PORTB_DIRSET = (1<<PIN_MOTORPLUS);
	PORTA_DIRSET = (1<<PIN_MOTORMINUS);
	
	PORTA.DIR &= ~ PIN5_bm;												//Set as input
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;			//Enable Pull-UP & enable interrupt on falling edge
	
	_delay_ms(100);														//Let the pull-up take effect


	if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
		motState = CLOSED;
	}else{
		//PORTB_OUTSET = (1<<BLUE_LED);
	}

	
}

void stopMotor(){
	//Break for 1 second
	PORTA_OUTSET = (1<<PIN_MOTORMINUS);
	PORTB_OUTSET = (1<<PIN_MOTORPLUS);
	_delay_ms(100);
	//Turn off
	PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
	PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
}


void openValve(){
	if(motState == CLOSED && error == NO_ERROR){

		prepareReadingCurrent(SAMPLE_ACCUM_OPEN);
		//calc_volt = 5.0;
		timeCounter = 0;
		//short_resistance = 50.0;
		//short_voltage = 10.0;
		

		motState = OPENING;
		PORTA_OUTSET = (1<<PIN_MOTORMINUS); // set HIGH;

		_delay_ms(300);

		//Measure Motor Characteristics
		//This can be used when the motor driver is replaces by a driver that can handle that much current and does not switch off before reaching the max current.

		/*
		while(motState == CLOSING && timeCounter < 150){
			voltageADC = getADCValue(ADC_PRESC_DIV4_gc, ADC_SAMPNUM_ACC1_gc, PIN_BATTERYSENSING);
			calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			currentADC = getADCValue(ADC_PRESC_DIV4_gc, ADC_SAMPNUM_ACC1_gc, PIN_CURRENTSENS_CHANNEL);
			calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			
			calc_resistance = (calc_volt/calc_curr);

			if(calc_resistance < short_resistance){
				short_resistance = calc_resistance;
				short_current = calc_curr;
				short_voltage = calc_volt;
			}
			

			timeCounter++;
		}

		timeCounter = 0;
		*/

		//For now: Assume motor short resistance conservatively:
		
		//short_resistance = 5.0;

		//From here, it could be calculated whats the maximum current the motor can reach when supplied with minimum voltage



		while(motState == OPENING && timeCounter <= OPEN_TIMEOUT){

			currentADC = ADC_0_readCurrent(SAMPLE_ACCUM_OPEN);
			//calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			//voltageADC = ADC_0_readBatteryVoltage();
			//calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			

			if(currentADC > OPEN_CURRENT_LIMIT_ADC){
				//stopMotor();

				PORTA_OUTCLR = (1<<PIN_MOTORMINUS); // set HIGH;
				PORTB_OUTSET = (1<<PIN_MOTORPLUS);
				_delay_ms(100);
				//Turn off
				PORTB_OUTCLR = (1<<PIN_MOTORPLUS);

				motState = OPEN;
				break;
			}
			
			timeCounter++;

		}

		stopMotor();
		//PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
		motState = OPEN;
		

		if(timeCounter>=OPEN_TIMEOUT){
			error = VALVE_TIMEOUT;
			//PORTA_OUTSET = (1<<PIN_REDLED);
		}
		
		//PORTB_OUTSET = (1<<BLUE_LED);
		//IGNORE ERROR STATES FOR DEV
		//error = NO_ERROR;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
	}
}


	
void closeValve(){
	
	if(motState == OPEN && error == NO_ERROR){
		
		prepareReadingCurrent(SAMPLE_ACCUM_CLOSE);
		//calc_volt = 5.0;
		timeCounter = 0;
		//short_resistance = 50.0;
		//short_voltage = 10.0;

		
		
		motState = CLOSING;
		PORTB_OUTSET = (1<<PIN_MOTORPLUS); // set HIGH;
		_delay_ms(150);

		//Measure Motor Characteristics
		//This can be used when the motor driver is replaces by a driver that can handle that much current and does not switch off before reaching the max current.

		/*
		while(motState == CLOSING && timeCounter < 150){
			voltageADC = getADCValue(ADC_PRESC_DIV4_gc, ADC_SAMPNUM_ACC1_gc, PIN_BATTERYSENSING);
			calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			currentADC = getADCValue(ADC_PRESC_DIV4_gc, ADC_SAMPNUM_ACC1_gc, PIN_CURRENTSENS_CHANNEL);
			calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			
			calc_resistance = (calc_volt/calc_curr);

			if(calc_resistance < short_resistance){
				short_resistance = calc_resistance;
				short_current = calc_curr;
				short_voltage = calc_volt;
			}
			

			timeCounter++;
		}

		timeCounter = 0;
		*/

		//For now: Assume motor short resistance conservatively:
		
		//short_resistance = 5.0;

		//From here, it could be calculated whats the maximum current the motor can reach when supplied with minimum voltage

		//Check if voltage is already too low
		/*
		if(short_voltage <= 4.1){
			//Break for 1 second
			PORTA_OUTSET = (1<<PIN_MOTORMINUS);
			PORTB_OUTSET = (1<<PIN_MOTORPLUS);
			_delay_ms(100);
			PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
			//_delay_ms(100);
			PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
			motState = CLOSED;
			
			PORTB_OUTSET = (1<<BLUE_LED);
			error = LOW_VOLTAGE;
			return;
		}*/
		

		while(motState == CLOSING && timeCounter <= CLOSE_TIMEOUT){	//Was at 1400

			currentADC = ADC_0_readCurrent(SAMPLE_ACCUM_CLOSE);
			
			//Voltage cannot be read while moving the motor right now, because switching between ADC channels after every measurement takes too much time.
			 
			//calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			//voltageADC = ADC_0_readBatteryVoltage();
			//calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;
			

			if(currentADC > CLOSE_CURRENT_LIMIT_ADC){
					
				stopMotor();
				motState = CLOSED;
				error = HIGH_CURRENT;
				break;
			}

			timeCounter++;

		}
		
		
		if(timeCounter>=CLOSE_TIMEOUT){
			stopMotor();
			motState = CLOSED;
			error = VALVE_TIMEOUT;
			//PORTA_OUTSET = (1<<PIN_REDLED);
		}

		
		if(calc_volt <= MIN_VOL){	//Triggers at 4.05, without cap and increased adc samp time it triggers at 4.5
			//PORTB_OUTSET = (1<<PIN_GREENLED);
			//PORTB_OUTSET = (1<<BLUE_LED);
			motState = CLOSED;
			error = LOW_VOLTAGE;
		}
		
	
		//PORTB_OUTCLR = (1<<BLUE_LED);
		//IGNORE ERROR STATES FOR DEV
		//error = NO_ERROR;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
		
	
	}
	
}

void changeMotorState(){
	if(error == NO_ERROR){
		if(motState == OPEN){
			closeValve();
		}else if(motState == CLOSED){
			openValve();
		}
	}
}


valveState getValveState(){
	return motState;
}

valveError getValveError(){
	return error;
}

uint16_t map(double x, double in_min, double in_max, double out_min, double out_max) {
	return out_min + ((x - in_min) * (out_max - out_min)) / (in_max - in_min);
}

void dynamic_delay(volatile uint16_t delay_variable) {
    while (delay_variable--) {
        _delay_ms(1);  // 1 millisecond delay
    }
}


ISR(PORTA_PORT_vect)
{
	if(PA5_INTERRUPT && motState == CLOSING)
	{
		dynamic_delay(map(calc_volt, MIN_VOL, MAX_VOL, MAX_MOTOR_CLOSE_DELAY, MAX_MOTOR_CLOSE_DELAY/5));
		stopMotor();
		motState = CLOSED;
		
	}
	PA5_CLEAR_INTERRUPT_FLAG;
}



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

#define DI_DT_THRESHOLD     1		// Raw ADC difference threshold 
#define TRIGGER_COUNT_MAX   4		//// Number of consecutive spikes needed to stop

uint16_t voltageADC;
uint16_t currentADC;
uint16_t previousADC = 0;
int16_t deltaCurrent;
uint8_t trigger_count = 0;

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
	PORTA.PIN5CTRL |= PORT_ISC_RISING_gc;			// enable interrupt on rising edge
	
	_delay_ms(100);														//Let the pull-up take effect


	if((PORTA_IN & (1<<PIN_MOTORSTOP))!=0){
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


void delay_us_precise(uint8_t us) {
	while (us--) {
		_delay_us(1);  // May not be cycle-perfect, but close enough for SW PWM
	}
}

void software_pwm_motorPlus(uint8_t high_time, uint8_t low_time) {


	PORTB_OUTSET = (1<<PIN_MOTORPLUS);
	delay_us_precise(high_time);

	PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
	delay_us_precise(low_time);
}

void software_pwm_motorMinus(uint16_t high_time, uint16_t low_time) {


	PORTA_OUTSET = (1<<PIN_MOTORMINUS);
	delay_us_precise(high_time);

	PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
	delay_us_precise(low_time);
}



void openValve(){
	if((motState == CLOSED || motState == UNDEFINED) && error == NO_ERROR){
		
		voltageADC = ADC_0_readBatteryVoltage();
		
		if(voltageADC <= MIN_VOLT_BEFORE_DRIVE_ADC && motState == CLOSED){
			error = LOW_VOLTAGE;
			//PORTB_OUTSET = (1<<BLUE_LED);
			return;
		}

		PORTA_OUTSET = (1<<PIN_SOILSENSORON);
		

		//clearBuffer(&rb);
		prepareReadingCurrent(SAMPLE_ACCUM_OPEN, ADC_PRESC_DIV16_gc);

		timeCounter = 0;
		trigger_count = 0;

		uint16_t filteredCurrent = 512;  // Initialize with a high value to start filtering
		uint8_t enoughSamples = 0;
		uint8_t sampleCounter = 0;

		motState = OPENING;
		//PORTA_OUTSET = (1<<PIN_MOTORMINUS); // set HIGH;
		//_delay_ms(300);

		for (uint8_t duty = 20; duty <= 40; duty++) {
			for (uint8_t i = 0; i < 120 ; i++) {
				software_pwm_motorMinus(duty, 40-duty);
			}
		}

		PORTA_OUTSET = (1<<PIN_MOTORMINUS);
		
		_delay_ms(200);




		//With a prescaler of 16 and a sample accumulator of 64 the loop time is pretty much exactly 4ms, in that time the motor shaft rotates 0.24 degrees at 10RPM
		while(motState == OPENING && timeCounter <= OPEN_TIMEOUT){

			currentADC = ADC_0_readCurrent(SAMPLE_ACCUM_OPEN);

			// Apply lightweight IIR filter
			filteredCurrent = (3 * filteredCurrent + currentADC) / 4;
			//filteredCurrent = newSample;

			// Approximate dI/dt as difference per call (assuming fixed time step)
			deltaCurrent = filteredCurrent - previousADC;
			previousADC = filteredCurrent;

			if(enoughSamples == 0){
				sampleCounter++;
				if(sampleCounter >= 4){
					enoughSamples = 1;
				}
			}
			
			if(enoughSamples){
				// Detect sharp rise
				if (deltaCurrent > DI_DT_THRESHOLD) {
					trigger_count++;
				} else {
					trigger_count = 0;  // Reset on stable current
				}

				if (trigger_count >= TRIGGER_COUNT_MAX || filteredCurrent >= OPEN_CURRENT_LIMIT_ADC) {
					
					//PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
					//_delay_ms(100);
					stopMotor();
					//Carefully open a little
					for (uint8_t duty = 20; duty <= 40; duty++) {
						for (uint8_t i = 0; i < 120 ; i++) {
							software_pwm_motorPlus(duty, 40-duty);
						}
					}
					motState = OPEN;
					break;
				}
			}
			

			
			/*
			if(currentADC > OPEN_CURRENT_LIMIT_ADC){

				//PORTA_OUTCLR = (1<<PIN_MOTORMINUS);
				//_delay_ms(100);
				stopMotor();
				//Carefully open a little
				for (uint8_t duty = 20; duty <= 40; duty++) {
					for (uint8_t i = 0; i < 120 ; i++) {
						software_pwm_motorPlus(duty, 40-duty);
					}
				}

				motState = OPEN;
				break;
			}
			*/
			
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
		PORTA_OUTCLR = (1<<PIN_SOILSENSORON);	//Turn off motor driver
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
	}
}


	
valveError closeValveAttempt(){

	
	
	if(motState == OPEN && error == NO_ERROR){
		
		PORTA_OUTSET = (1<<PIN_SOILSENSORON);	//Turn on Motor Driver
		

		timeCounter = 0;
		uint8_t triggerBatteryLow = 0;
		valveError intermediateError = NO_ERROR;

		
		
		motState = CLOSING;
		//PORTB_OUTSET = (1<<PIN_MOTORPLUS); // set HIGH;
		//_delay_ms(150);

		//PWM Motor for 96ms
		
		for (uint8_t duty = 20; duty <= 40; duty++) {
			for (uint8_t i = 0; i < 120 ; i++) {
				software_pwm_motorPlus(duty, 40-duty);
				if(motState!=CLOSING)
					break;
			}
			if(motState!=CLOSING)
			break;
		}


		if(motState != CLOSING){
			PORTB_OUTCLR = (1<<PIN_MOTORPLUS);
		}else{
			PORTB_OUTSET = (1<<PIN_MOTORPLUS);
		}
		
		_delay_ms(50);


		

		

		while(motState == CLOSING && timeCounter <= CLOSE_TIMEOUT){	//Was at 1400

			
			prepareReadingCurrent(SAMPLE_ACCUM_CLOSE, ADC_PRESC_DIV4_gc);
			currentADC = ADC_0_readCurrent(SAMPLE_ACCUM_CLOSE);
			//calc_curr  = (currentADC * MAX_CUR) / RES_10BIT;
			voltageADC = ADC_0_readBatteryVoltage();
			//calc_volt  = (voltageADC * MAX_VOL) / RES_10BIT;

			if(voltageADC <= MIN_VOLT_ADC)
				triggerBatteryLow = 1;
			

			if(currentADC > CLOSE_CURRENT_LIMIT_ADC){
				stopMotor();
				motState = UNDEFINED;
				intermediateError = HIGH_CURRENT;
				break;
			}

			timeCounter++;

		}
		
		
		if(timeCounter>=CLOSE_TIMEOUT){
			stopMotor();
			motState = UNDEFINED;
			intermediateError = VALVE_TIMEOUT;
			
		}

		
		if(triggerBatteryLow){	//Triggers at 4.05, without cap and increased adc samp time it triggers at 4.5
			intermediateError = LOW_VOLTAGE;
		}
		
		//IGNORE ERROR STATES FOR DEV
		//error = NO_ERROR;
		_delay_ms(100);			//Let the motor calm down before driving it in the other direction
		
		PORTA_OUTCLR = (1<<PIN_SOILSENSORON);

		return intermediateError;
	
	}

	return NO_ERROR;
}

void closeValve(){

	uint8_t closeAttempts = 1;

	valveError closeError = closeValveAttempt();

	while(closeError == HIGH_CURRENT && closeAttempts <= 3){
		openValve();
		closeError = closeValveAttempt();
		closeAttempts++;
	}

	if(error == NO_ERROR) //In case openValve throws a Timeout error already
		error = closeError;

	//Only in Dev Mode:
	/*
	switch(error){
		case LOW_VOLTAGE: PORTB_OUTSET = (1<<BLUE_LED);
			break;
		case HIGH_CURRENT: PORTB.OUTSET = (1<<PIN_GREENLED);
			break;
		case VALVE_TIMEOUT: PORTA_OUTSET = (1<<PIN_REDLED);
			break;
		case NO_ERROR:
			break;
	}
	*/
	
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

void setValveError(valveError err){
	error = err;
}


ISR(PORTA_PORT_vect)
{
	if(PA5_INTERRUPT && motState == CLOSING)
	{
		//dynamic_delay(map(calc_volt, MIN_VOL, MAX_VOL, MAX_MOTOR_CLOSE_DELAY, MAX_MOTOR_CLOSE_DELAY/5));	//Result=43 @ 5.0V
		stopMotor();
		motState = CLOSED;
		
	}
	PA5_CLEAR_INTERRUPT_FLAG;
}



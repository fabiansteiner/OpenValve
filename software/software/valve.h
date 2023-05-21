/*
 * valve.h
 *
 * Created: 02.11.2022 16:19:30
 *  Author: SteinerF
 *
 *	Interface to the motor for opening and closing the valve + Button for valve state feedback.
 *  States: Open, Closed, CurrentSensorError
 *  Peripherals needed: Timer Interrupt, Button Interrupt, ADC
 */ 


#ifndef VALVE_H_
#define VALVE_H_

#include <avr/io.h>

#define PIN_MOTORPLUS 1		//PORTB
#define PIN_MOTORMINUS 6	//PORTA
#define PIN_MOTORSTOP 5		//PORTA


typedef enum {UNDEFINED=1, OPEN=2, CLOSED=5, CURRERROR=10, OPENING = 20, CLOSING = 40}  valveState;
	
	
//Figure out state of valve, if not closed, close it
void initializeValve();

void openValve();

void closeValve();

void changeMotorState();

valveState getValveState();

void setValveState(valveState);



#endif /* VALVE_H_ */
/*
 * LEDs.h
 *
 * Created: 02.11.2022 16:17:20
 *  Author: SteinerF
 */ 


#ifndef LEDS_H_
#define LEDS_H_

#include "UserInterface.h"

#define PIN_GREENLED 3		//PORTB
#define PIN_REDLED 7		//PORTA

void initLEDs();

void animateTransition(UIstate nextState);	//Blink green led two times

void animateBatteryLevel(uint16_t ADCMeasurement);		//From red to green to red to battery state

void animateSelectThreshold();	//Pulsate Brown

void animateSelectInterval();	//Pulsate RED

void animateChangeSoilThreshold(uint16_t currentThresholdLevel);	//Blink as many times as currentThresholdLevel, then stop for a second

void animateChangeInterval(uint16_t currentIntervalLevel);			//Blink led red, the higher the interval level the faster it blinks

void stopLEDs();



#endif /* LEDS_H_ */
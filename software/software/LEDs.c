#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#include "LEDs.h"
#include "UserInterface.h"

void initLEDs(){
	PORTB_DIRSET = (1<<3);
	PORTA_DIRSET = (1<<7);
}

void animateTransition(UIstate nextState){
		//Blink green led two times and then switch to passed state
}

void animateSelectThreshold(){
		//Pulsate Brown
}

void animateSelectInterval(){
		//Pulsate RED
}

void animateChangeSoilThreshold(uint16_t currentThresholdLevel){
		//Blink as many times as currentThresholdLevel, then stop for a second
}

void animateChangeInterval(uint16_t currentIntervalLevel){
				//Blink led red, the higher the interval level the faster it blinks
}

void stopLEDs(){
	
}
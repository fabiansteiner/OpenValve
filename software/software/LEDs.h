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
#define BLUE_LED 2		//PORTB

typedef enum {NO_ANIMATION, A_BATTERY, A_SELECTTHRESHOLD , A_SELECTINTERVAL,A_CHANGETHRESHOLD,A_CHANGEINTERVAL, A_TRANSITIONING} currentLEDAnimation;

void initLEDs();
void cycleLEDAnimation(state_change change);
currentLEDAnimation getLEDAnimation();


#endif /* LEDS_H_ */
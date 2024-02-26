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

#define LED_CONFIRM 1
#define LED_TIMEOUT 0

typedef enum {NO_ANIMATION, A_BATTERY, A_SELECTTHRESHOLD , A_SELECTINTERVAL,A_CHANGETHRESHOLD,A_CHANGEINTERVAL, A_TRANSITIONING_CONF, A_TRANSITIONING_TIMEOUT, A_MANUALIRRIGATION} currentLEDAnimation;

void initLEDs();
void changeLEDAnimation(state_change change);
void cycleLEDAnimation();
currentLEDAnimation getLEDAnimation();


#endif /* LEDS_H_ */
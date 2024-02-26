#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "UserInterface.h"
#include "LEDs.h"
#include "ADC.h"
#include "valve.h"

UIstate uiState = SHOWNOTHING;


uint8_t soilLevel = 4;
uint8_t interval = 5;


thresholds currentThresholds;
uint8_t secondCounter = 0;
uint16_t milliSecondCounter = 0;



void changeThresholds(){
	// Magic number 102 because ADC returns value between 0 and 1023, we have 8 configurable threshold levels and 2 two upper levels are reserved to give the sensor a chance to detect when sensor is wet at the upper levels. 1023/10 = ~102
	// Closing threshold has to be higher because if ADC delivers exactly a value on the borderline its likely that the valve cant decide if it should be open or closed.
	currentThresholds.tresholdOpen = soilLevel * 102;
	currentThresholds.thresholdClose = currentThresholds.tresholdOpen + 102;
}

void initUI(){
	changeThresholds();
}

void increaseThreshold(){
	soilLevel++;
	if(soilLevel >8){ soilLevel = 1;}
	changeThresholds();
	
}

void increaseInterval(){
	interval++;
	if(interval > 5) interval = 1;
}



thresholds getCurrentThresholds(){
	return currentThresholds;
}

uint8_t getCurrentInterval(){
	
	return interval;
}

UIstate getUIState(){
	return uiState;
}



state_change changeUIState(pressType button_press){
	//State machine
	state_change change = NO_CHANGE;

	if(button_press != NONE && button_press != VERYLONG){
		//User Pressed something, reset Timeout Variables
		milliSecondCounter = 0;
		secondCounter = 0;
		
		
			

		switch(uiState) {
			case SHOWNOTHING:
			uiState = SHOWBATTERY;
			change = FROM_SHOWNOTHING_TO_SHOWBATTERY;
			break;
			case SHOWBATTERY:
			if(button_press == LONG){
				uiState = SELECTTHRESHOLD;
				change = FROM_SHOWBATTERY_TO_SELECTTHRESHOLD;
				}else if (button_press == SHORT && getValveState()==CLOSED){
				uiState = MANUALIRRIGATION;
				change = FROM_SHOWBATTERY_TO_MANUALIRRIGATION;
			}
			break;
			case MANUALIRRIGATION:
			if(button_press == SHORT && getValveState()==OPEN){
				uiState = SHOWBATTERY;
				change = FROM_MANUALIRRIGATION_TO_SHOWBATTERY;
			}
			break;
			case SELECTTHRESHOLD:
			if(button_press == SHORT){
				uiState = SELECTINTERVAL;
				change = FROM_SELECTTHRESHOLD_TO_SELECTINTERVAL;
				}else if (button_press == LONG){
				uiState = CHANGETHRESHOLD;
				change = FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD;
			}
			break;
			case SELECTINTERVAL:
			if(button_press == SHORT){
				uiState = SELECTTHRESHOLD;
				change = FROM_SELECTINTERVAL_TO_SELECTTHRESHOLD;
				}else if (button_press == LONG){
				uiState = CHANGEINTERVAL;
				change = FROM_SELECTINTERVAL_TO_CHANGEINTERVAL;
			}
			break;
			case CHANGETHRESHOLD:
			if(button_press == SHORT){
				increaseThreshold();
				change = THRESHOLD_CHANGED;
				}else if (button_press == LONG){
				uiState = SHOWNOTHING;
				change = UI_OFF;
			}
			break;
			case CHANGEINTERVAL:
			if(button_press == SHORT){
				increaseInterval();
				change = INTERVAL_CHANGED;
				}else if (button_press == LONG){
				uiState = SHOWNOTHING;
				change = UI_OFF;
			}
			break;
		}
		
		
	}else if(button_press == VERYLONG){
		uiState = SHOWNOTHING;
		change = UI_SHUTDOWN;
	}

	return change;

	
}


pressType senseMagneticSwitch(){
	static uint16_t buttonTimeCounter = 0;
	static uint16_t buttonTimeCounterForSwitchOFF = 0;
	static uint8_t alreadyPressed = 0;
	pressType press = NONE;

	if((PORTB_IN & (1<<PIN_MAGNETSWITCH))==0){
		buttonTimeCounter++;
		buttonTimeCounterForSwitchOFF++;
		
		if(buttonTimeCounter>=150 && !alreadyPressed){
			press = LONG;
			buttonTimeCounter = 0;
			alreadyPressed = 1;
		}
		
		if(buttonTimeCounterForSwitchOFF>=600){
			press = VERYLONG;
			buttonTimeCounterForSwitchOFF = 0;
			
		}
	}else{
		if(buttonTimeCounter>=0 && !alreadyPressed){
			if(buttonTimeCounter >= 2){
				press = SHORT;
			}
		}
		
		buttonTimeCounter=0;
		buttonTimeCounterForSwitchOFF=0;
		alreadyPressed = 0;
		
	}

	return press;
}

state_change countUITimeOut(){
	state_change change = NO_CHANGE;

	if (uiState != SHOWNOTHING){
		
		milliSecondCounter = milliSecondCounter + MAINLOOP_DELAY;
		
		if(milliSecondCounter >= 1000){
			secondCounter++;
			milliSecondCounter = 0;
		}
		
		if(secondCounter >= 16){
			
			change = UI_OFF_WITHOUT_CONFIRMING;
				
			uiState = SHOWNOTHING;
			
			secondCounter = 0;
			milliSecondCounter = 0;
		}
		
	}else if (milliSecondCounter >= 0 || secondCounter >= 0){
		milliSecondCounter = 0;
		secondCounter = 0;
	}

	return change;
}





#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "UserInterface.h"
#include "LEDs.h"
#include "ADC.h"
#include "valve.h"
#include "SoilMoistureSensor.h"

UIstate uiState = SHOWNOTHING;


uint8_t soilLevel = 4;
uint8_t multiplicator = 1;


thresholds currentThresholds;
uint8_t secondCounter = 0;
uint16_t milliSecondCounter = 0;


void changeThresholds(){
	// Magic number 102 because ADC returns value between 0 and 1023, we have 8 configurable threshold levels and 2 two upper levels are reserved to give the sensor a chance to detect when sensor is wet at the upper levels. 1023/10 = ~102
	// Closing threshold has to be higher because if ADC delivers exactly a value on the borderline its likely that the valve cant decide if it should be open or closed.
	
	currentThresholds.tresholdOpen = getSensorTreshold(soilLevel-1);
	currentThresholds.thresholdClose = currentThresholds.tresholdOpen + 25;

	//This is the linear approach
	/*
	if(LINEAR_SENSOR){
		currentThresholds.tresholdOpen = soilLevel * ADCSTEPS;
		currentThresholds.thresholdClose = currentThresholds.tresholdOpen + ADCSTEPS/2;
	}else{
		currentThresholds.tresholdOpen = adc_trehsolds[soilLevel-1];
		currentThresholds.thresholdClose = adc_trehsolds[soilLevel-1]+25;
	}
	*/
	

	
}

void initUI(){
	changeThresholds();
}

void increaseThreshold(){
	soilLevel++;
	if(soilLevel >8){ soilLevel = 1;}
	changeThresholds();
	
}

void increaseMultiplicator(){
	multiplicator++;
	if(multiplicator > 5) multiplicator = 1;
}



thresholds getCurrentThresholds(){
	return currentThresholds;
}

uint8_t getCurrentMultiplicator(){
	
	return multiplicator;
}

UIstate getUIState(){
	return uiState;
}



state_change changeUIState(pressType button_press){
	//State machine
	state_change change = NO_CHANGE;

	if(button_press != NONE && button_press != VERYLONG){
		
		
		
			

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
				uiState = SELECTMULTIPLICATOR;
				change = FROM_SELECTTHRESHOLD_TO_SELECTMULTIPLICATOR;
				}else if (button_press == LONG){
				uiState = CHANGETHRESHOLD;
				change = FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD;
			}
			break;
			case SELECTMULTIPLICATOR:
			if(button_press == SHORT){
				uiState = SELECTTHRESHOLD;
				change = FROM_SELECTMULTIPLICATOR_TO_SELECTTHRESHOLD;
				}else if (button_press == LONG){
				uiState = CHANGEMULTIPLICATOR;
				change = FROM_SELECTMULTIPLICATOR_TO_CHANGEMULTIPLICATOR;
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
			case CHANGEMULTIPLICATOR:
			if(button_press == SHORT){
				increaseMultiplicator();
				change = MULTIPLICATOR_CHANGED;
				}else if (button_press == LONG){
				uiState = SHOWNOTHING;
				change = UI_OFF;
			}
			break;
			default: break;
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

	if((PORTB_IN & (1<<PIN_MAGNETSWITCH))!=0){
		//User Pressed something, reset Timeout Variables
		milliSecondCounter = 0;
		secondCounter = 0;

		buttonTimeCounter++;
		buttonTimeCounterForSwitchOFF++;
		
		if(buttonTimeCounter>=75 && !alreadyPressed){
			press = LONG;
			buttonTimeCounter = 0;
			alreadyPressed = 1;
		}
		
		if(buttonTimeCounterForSwitchOFF>=300){
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





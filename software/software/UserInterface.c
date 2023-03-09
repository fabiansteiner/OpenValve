#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "UserInterface.h"
#include "LEDs.h"
#include "ADC.h"

#define INPUT_START 516    // The lowest number of the range input.
#define INPUT_END 930    // The largest number of the range input.
#define OUTPUT_START 2 // The lowest number of the range output.
#define OUTPUT_END 48  // The largest number of the range output.

UIstate uiState;
UIstate previousState;


uint8_t soilLevel = 1;
uint8_t interval = 5;

uint16_t buttonTimeCounter = 0;
uint8_t alreadyPressed = 0;
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
	uiState = SHOWNOTHING;
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

uint8_t getBatteryADC(){
	uint16_t adcVoltage = ADC_0_readBatteryVoltage();
	uint8_t batteryLevel;
	
	//Convert adc value to battery level between 2 and 48
	//Battery is on a voltage divider which divides voltage by 3. So if Battery is 9V, ADC will read 3V
	//Assumption: Battery is empty at 5V and full at 9V, which is an ADC Value between 516 and 930
	//Map ADC Values to battery level
	if(adcVoltage > 930){
		return OUTPUT_END;
	}else if(adcVoltage < 516){
		return OUTPUT_START;
	}else{
		batteryLevel = OUTPUT_START + ((adcVoltage - INPUT_START) * (OUTPUT_END - OUTPUT_START)) / (INPUT_END - INPUT_START);
	}
	
	
	return batteryLevel;
}

void changeUIState(uint8_t longpress){
	//State machine
	
	milliSecondCounter = 0;
	secondCounter = 0;

	switch(uiState) {
		case SHOWNOTHING:
			uiState = SHOWBATTERY;
			animateBatteryLevel(getBatteryADC());
			break;

			
		case SHOWBATTERY:
			if(longpress){
				previousState = SHOWBATTERY;
				uiState = TRANSITION;
				animateTransition();
			}
			break;
		case SELECTTHRESHOLD:
			if(longpress==0){
				uiState = SELECTINTERVAL;
				animateSelectInterval();
			}else if (longpress==1){
				previousState = SELECTTHRESHOLD;
				uiState = TRANSITION;
				animateTransition();
			}
			break;
		case SELECTINTERVAL:
			if(longpress==0){
				uiState = SELECTTHRESHOLD;
				animateSelectThreshold();
			}else if (longpress==1){
				previousState = SELECTINTERVAL;
				uiState = TRANSITION;
				animateTransition();
			}
			break;
		case CHANGETHRESHOLD:
			if(!longpress){
				increaseThreshold();
				animateChangeSoilThreshold(soilLevel);
			}else if (longpress){
				previousState = CHANGETHRESHOLD;
				uiState = TRANSITION;
				animateTransition();
			}
			break;
		case CHANGEINTERVAL:
			if(!longpress){
				increaseInterval();
				animateChangeInterval(interval);
			}else if (longpress){
				previousState = CHANGEINTERVAL;
				uiState = TRANSITION;
				animateTransition();
			}
			break;
		case TRANSITION:
			switch(previousState){
				case SHOWBATTERY:
					uiState = SELECTTHRESHOLD;
					animateSelectThreshold();
					break;
				case SELECTTHRESHOLD:
					uiState = CHANGETHRESHOLD;
					animateChangeSoilThreshold(soilLevel);
					break;
				case SELECTINTERVAL:
					uiState = CHANGEINTERVAL;
					animateChangeInterval(interval);
					break;
				case CHANGETHRESHOLD:
					uiState = SHOWNOTHING;
					stopLEDs();
					break;
				case CHANGEINTERVAL:
					uiState = SHOWNOTHING;
					stopLEDs();
					break;
				case TRANSITION:
					uiState = SHOWNOTHING;
					stopLEDs();
					break;
				case SHOWNOTHING:
					uiState = SHOWNOTHING;
					stopLEDs();
					break;
					
			}
			break;
	}

	
}


void senseMagneticSwitch(){
	if((PORTB_IN & (1<<PIN_MAGNETSWITCH))==0){
		buttonTimeCounter++;
		if(buttonTimeCounter>=200 && !alreadyPressed){
			changeUIState(1);
			buttonTimeCounter = 0;
			alreadyPressed = 1;
		}
	}else{
		if(buttonTimeCounter>=0 && !alreadyPressed){
			if(buttonTimeCounter >= 2){
				changeUIState(0);
			}
		}
		
		buttonTimeCounter=0;
		alreadyPressed = 0;
		
	}
}

void countUITimeOut(){
	if (uiState != SHOWNOTHING){
		
		milliSecondCounter = milliSecondCounter + MAINLOOP_DELAY;
		if(milliSecondCounter >= 1000){
			secondCounter++;
			milliSecondCounter = 0;
		}
		if(secondCounter >= 8){
			if(uiState == SHOWBATTERY){
				uiState = SHOWNOTHING;
				stopLEDs();
			}else{
				previousState = SHOWNOTHING;
				uiState = TRANSITION;
				animateTransition();
			}
			
			secondCounter = 0;
			milliSecondCounter = 0;
		}
	}else if (milliSecondCounter >= 0 || secondCounter >= 0){
		milliSecondCounter = 0;
		secondCounter = 0;
	}
}





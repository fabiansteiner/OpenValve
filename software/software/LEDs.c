#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>


#include "LEDs.h"
#include "UserInterface.h"
#include "valve.h"
#include "ADC.h"
#include "common.h"


void (*func_ptr)(void);

volatile currentLEDAnimation ongoingAnimation = NO_ANIMATION;
volatile uint16_t animationCounter = 0;

#define ONESTEP 66
#define HALFSTEP 33
#define MAX_LEVEL 48*ONESTEP
#define MIN_LEVEL 2*ONESTEP

volatile uint16_t batteryLevel;
volatile uint8_t cycle = 0;
volatile uint8_t cycleCounter = 0;
volatile uint16_t greenBrightness = 132;
volatile uint8_t msCounter = 0;
volatile uint8_t countingUp = 1;


void showBlueLED();
void animateBlinking(char color, uint16_t delay);
void animateManualIrrigation();
void animateTransition(uint8_t confirm);	//Blink green led two times
void animateBatteryLevel();		//From red to green to red to battery state
void animateSelectThreshold();	//Glow Orange
void animateSelectMultiplicator();	//Glow Red
void animateChangeSoilThreshold();	//Blink as many times as currentThresholdLevel, then stop for a second
void animateChangeMultiplicator();			//Blink led red, the higher the interval level the faster it blinks
void animateValveErrors();
void stopLEDs();


void cycleBatteryLevelAnimation(){
	
	//if + else = 1ms
	if (cycle == 0){
		PORTB.OUTSET = (1<<PIN_GREENLED);
		PORTA.OUTCLR = (1<<PIN_REDLED);
		TCA0.SINGLE.PERBUF = 3300-greenBrightness;
		cycle = 1;
	}else{
		PORTB.OUTCLR = (1<<PIN_GREENLED);
		PORTA.OUTSET = (1<<PIN_REDLED);
		TCA0.SINGLE.PERBUF = greenBrightness;
		cycle = 0;
	}
	
	cycleCounter++;
	
	if (cycleCounter >= 2) {	//means 1ms passed
		cycleCounter = 0;
		
		if (countingUp != 3)
		{
			msCounter++;
			if(msCounter >= 20){
				msCounter = 0;
				
				if (countingUp==1){
					if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
						greenBrightness = greenBrightness + HALFSTEP;
					}else{
						greenBrightness = greenBrightness + ONESTEP;
					}
					
					if(greenBrightness >= MAX_LEVEL) countingUp = 0;

				}else if (countingUp == 0){
					if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
						greenBrightness = greenBrightness - HALFSTEP;
					}else{
						greenBrightness = greenBrightness - ONESTEP;
					}

					if(greenBrightness <= MIN_LEVEL) countingUp = 2;

				}else if (countingUp == 2){
					if (greenBrightness <= batteryLevel*66) {
						if(greenBrightness >= ONESTEP*10 || greenBrightness <= ONESTEP*40){
							greenBrightness = greenBrightness + HALFSTEP;
						}else{
							greenBrightness = greenBrightness + ONESTEP;
						}
					}else {countingUp = 3;}
				}
			}
		}
		
		
	}
	
}

ISR(TCA0_OVF_vect)
{
	if (ongoingAnimation == A_TRANSITIONING_CONF || ongoingAnimation == A_TRANSITIONING_TIMEOUT){
		if (animationCounter < 4){
			//Wait a little bit
			animationCounter++;
		}else if (animationCounter >=4 && animationCounter<8){
			//Blink 2 times green or red dependend on Timeout or confirmation
			if(ongoingAnimation == A_TRANSITIONING_CONF){PORTB.OUTTGL = (1<<PIN_GREENLED);}
			else{PORTA.OUTTGL = (1<<PIN_REDLED);}
			
			animationCounter++;
		}else if (animationCounter >=8 && animationCounter<12){
			//Wait a little bit
			animationCounter++;
		}else{
			//Turn off counter
			TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
			(*func_ptr)(); //Call appropriate function that was assigned when changing the state
		}
		
	}else if(ongoingAnimation == A_TRANSITIONING_SHUTDOWN){
		if (animationCounter < 2){
				//Wait a little bit
				animationCounter++;
			}else if (animationCounter >=2 && animationCounter<8){
				//Blink 3 times red 
				PORTA.OUTTGL = (1<<PIN_REDLED);
			
				animationCounter++;
			}else{
				//Turn off counter
				TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
				(*func_ptr)(); //Call appropriate function that was assigned when changing the state
		}
	}else if(ongoingAnimation == A_TRANSITIONING_STARTUP){
		if (animationCounter < 2){
			PORTA.OUTTGL = (1<<PIN_REDLED); 
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			PORTB.OUTTGL = (1<<BLUE_LED);
			animationCounter++;
		}else if (animationCounter >=2 && animationCounter<6){
			//Wait a little bit
			animationCounter++;
		}else if (animationCounter >=6 && animationCounter<VERSION_ANIMATION_COUNT1){
			//Toggle RED led to indicate MAJOR version
			PORTA.OUTTGL = (1<<PIN_REDLED); 
			animationCounter++;
		}else if (animationCounter >=VERSION_ANIMATION_COUNT1 && animationCounter<VERSION_ANIMATION_COUNT2){
			//Toggle green led to indicate Minor version
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			animationCounter++;
		}else if (animationCounter >=VERSION_ANIMATION_COUNT2 && animationCounter<VERSION_ANIMATION_COUNT3){
			//Wait a bit
			animationCounter++;
		}else{
			//Turn off counter
			TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
			(*func_ptr)(); //Call appropriate function that was assigned when changing the state
		}
	}else if(ongoingAnimation == A_BATTERY){
		cycleBatteryLevelAnimation();
	}else if(ongoingAnimation == A_CHANGETHRESHOLD){
		if (animationCounter < soilLevel*2){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			//PORTA.OUTTGL = (1<<PIN_REDLED);
			animationCounter++;
		}else{
			animationCounter++;
			if(animationCounter > soilLevel*2+8){
				animationCounter = 0;
			}
		}
	}else if(ongoingAnimation == A_CHANGEMULTIPLICATOR){
		if (animationCounter < multiplicator*2){
			PORTB.OUTTGL = (1<<PIN_GREENLED);
			PORTA.OUTTGL = (1<<PIN_REDLED);
			animationCounter++;
			}else{
			animationCounter++;
			if(animationCounter > multiplicator*2+8){
				animationCounter = 0;
			}
		}
	}else if(ongoingAnimation == A_SHOWERRORS){
		if (animationCounter < getValveError()*2){
			PORTA.OUTTGL = (1<<PIN_REDLED);
			animationCounter++;
		}else{
			animationCounter++;
			if(animationCounter > getValveError()*2+8){
				animationCounter = 0;
			}
		}
	}


	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

void resetTimerSettings(){
	// Hard reset timer
	TCA0.SINGLE.CTRLA = 0;
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESET_gc;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;

	//Reset LEDs
	PORTA.OUTCLR = (1<<PIN_REDLED);
	PORTB.OUTCLR = (1<<PIN_GREENLED);
	//PORTB.OUTCLR = (1<<BLUE_LED);

	//Reset counters used for LED animation
	animationCounter = 0;

	cycle = 0;
	cycleCounter = 0;
	greenBrightness = 132;
	msCounter = 0;
	countingUp = 1;

	//Reinitialize Timer
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;			//Single mode
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;	//Normal operation, no waveform or other extras
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);		//Count steps, not events

	
}

void initLEDs(){
	PORTB_DIRSET = (1<<PIN_GREENLED);
	PORTA_DIRSET = (1<<PIN_REDLED);
	PORTB_DIRSET = (1<<BLUE_LED);

	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;			//Single mode
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;	//Normal operation, no waveform or other extras
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);		//Count steps, not events
}

void animateVersionNumber(){
/*
if (animationCounter < 2){
	PORTA.OUTTGL = (1<<PIN_REDLED);
	PORTB.OUTTGL = (1<<PIN_GREENLED);
	PORTB.OUTTGL = (1<<BLUE_LED);
	animationCounter++;
	}else if (animationCounter >=2 && animationCounter<6){
	//Wait a little bit
	animationCounter++;
	}else if (animationCounter >=6 && animationCounter<VERSION_ANIMATION_COUNT1){
	//Toggle RED led to indicate MAJOR version
	PORTA.OUTTGL = (1<<PIN_REDLED);
	animationCounter++;
	}else if (animationCounter >=VERSION_ANIMATION_COUNT1 && animationCounter<VERSION_ANIMATION_COUNT2){
	//Toggle green led to indicate Minor version
	PORTB.OUTTGL = (1<<PIN_GREENLED);
	animationCounter++;
	}else if (animationCounter >=VERSION_ANIMATION_COUNT2 && animationCounter<VERSION_ANIMATION_COUNT3){
	//Wait a bit
	animationCounter++;
	}else{
	//Turn off counter
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_OVF_bm;
	(*func_ptr)(); //Call appropriate function that was assigned when changing the state
}
*/

	ongoingAnimation = A_TRANSITIONING_STARTUP;

	PORTA.OUTTGL = (1<<PIN_REDLED);
	PORTB.OUTTGL = (1<<PIN_GREENLED);
	PORTB.OUTTGL = (1<<BLUE_LED);

	_delay_ms(300);

	PORTA.OUTTGL = (1<<PIN_REDLED);
	PORTB.OUTTGL = (1<<PIN_GREENLED);
	PORTB.OUTTGL = (1<<BLUE_LED);

	_delay_ms(1000);

	for(uint8_t i = 0; i < VERSION_MAJOR * 2; i++){
		PORTA.OUTTGL = (1<<PIN_REDLED);
		_delay_ms(250);
	}


	for(uint8_t i = 0; i < VERSION_MINOR * 2; i++){
		PORTB.OUTTGL = (1<<PIN_GREENLED);
		_delay_ms(250);
	}
	_delay_ms(750);

	animateBatteryLevel();
}


void changeLEDAnimation(state_change change){
	
	
	switch(change){
		case UI_STARTUP: /*animateVersionNumber();*/ batteryLevel = getBatteryLevel(); func_ptr = &animateBatteryLevel; animateTransition(LED_STARTUP);	//Transition
		break;
		case FROM_SHOWNOTHING_TO_SHOWBATTERY: batteryLevel = getBatteryLevel(); animateBatteryLevel();
		break;
		case FROM_SHOWBATTERY_TO_SELECTTHRESHOLD: func_ptr = &animateSelectThreshold;				animateTransition(LED_CONFIRM);	//Transition
		break;
		case FROM_SHOWBATTERY_TO_MANUALIRRIGATION: animateManualIrrigation();
		break;
		case FROM_MANUALIRRIGATION_TO_SHOWBATTERY: batteryLevel = getBatteryLevel(); animateBatteryLevel();
		break;
		case FROM_SELECTTHRESHOLD_TO_SELECTMULTIPLICATOR: animateSelectMultiplicator();
		break;
		case FROM_SELECTTHRESHOLD_TO_CHANGETHRESHOLD: func_ptr = &animateChangeSoilThreshold;		animateTransition(LED_CONFIRM); //Transition
		break;
		case FROM_SELECTMULTIPLICATOR_TO_SELECTTHRESHOLD: animateSelectThreshold();
		break;
		case FROM_SELECTMULTIPLICATOR_TO_CHANGEMULTIPLICATOR: func_ptr = &animateChangeMultiplicator;				animateTransition(LED_CONFIRM); //Transition
		break;
		case THRESHOLD_CHANGED: animateChangeSoilThreshold();
		break;
		case MULTIPLICATOR_CHANGED: animateChangeMultiplicator();
		break;
		case UI_OFF:  func_ptr = &stopLEDs; animateTransition(LED_CONFIRM);	//Transition
		break;
		case UI_OFF_WITHOUT_CONFIRMING: func_ptr = &stopLEDs; animateTransition(LED_TIMEOUT);	//Transition
		break;
		case UI_SHUTDOWN: func_ptr = &stopLEDs; animateTransition(LED_SHUTDOWN);	//Transition
		break;
		case SHOW_ERROR: animateValveErrors();
		break;
		default:
		break;
		
	}

	

	

}

void cycleLEDAnimation(){
	if(ongoingAnimation == A_MANUALIRRIGATION){
		animateManualIrrigation();
	}else if(ongoingAnimation == A_SELECTMULTIPLICATOR){
		animateSelectMultiplicator();
	}else if(ongoingAnimation == A_SELECTTHRESHOLD){
		animateSelectThreshold();
	}
}


//Blink green led two times and then switch to passed state
void animateTransition(uint8_t confirm){

	//Set overflow interval to 250ms
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;

	if(confirm==LED_CONFIRM){
		ongoingAnimation = A_TRANSITIONING_CONF;
	}else if(confirm==LED_TIMEOUT){
		ongoingAnimation = A_TRANSITIONING_TIMEOUT;
	}else if(confirm==LED_SHUTDOWN){
		ongoingAnimation = A_TRANSITIONING_SHUTDOWN;
		TCA0.SINGLE.PER = 4000;
	}else if(confirm==LED_STARTUP){
		ongoingAnimation = A_TRANSITIONING_STARTUP;
		TCA0.SINGLE.PER = 4000;
	}

	

	
	
}

void animateBatteryLevel(){

	
	

	ongoingAnimation = A_BATTERY;

	//Set overflow interval to 50khz
	resetTimerSettings();
	TCA0.SINGLE.PER = 1000;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}

void animateBlinking(char color, uint16_t delay){
	static uint16_t milliSecondCounter1 = 0;
	
	milliSecondCounter1 += MAINLOOP_DELAY;
	if(milliSecondCounter1 >= delay){

		switch (color)
		{
			case 'R': PORTA.OUTTGL = (1<<PIN_REDLED);
			break;
			case 'G': PORTB.OUTTGL = (1<<PIN_GREENLED);
			break;
			case 'B': PORTB.OUTTGL = (1<<BLUE_LED);
			break;
			case 'O': PORTA.OUTTGL = (1<<PIN_REDLED);PORTB.OUTTGL = (1<<PIN_GREENLED);
			break;
			default:
			/* Your code here */
			break;
		}

		milliSecondCounter1 = 0;
	}
	
}

void animateSelectThreshold(){

	if(ongoingAnimation != A_SELECTTHRESHOLD){
		resetTimerSettings();
		ongoingAnimation = A_SELECTTHRESHOLD;
	}
	
	//animateBlinking('G', 100);
	PORTB.OUTSET = (1<<PIN_GREENLED);

}

void animateSelectMultiplicator(){

	if(ongoingAnimation != A_SELECTMULTIPLICATOR){
		resetTimerSettings();
		ongoingAnimation = A_SELECTMULTIPLICATOR;
	}
	
	//animateBlinking('O', 100);
	PORTB.OUTSET = (1<<PIN_GREENLED);
	PORTA.OUTSET = (1<<PIN_REDLED);


}



void animateManualIrrigation(){
	
	if(ongoingAnimation != A_MANUALIRRIGATION){
		resetTimerSettings();
		ongoingAnimation = A_MANUALIRRIGATION;
	}
	
	animateBlinking('B', 1000);

}

void animateChangeSoilThreshold(){
	ongoingAnimation = A_CHANGETHRESHOLD;
	
	//Blink as many times as currentThresholdLevel, then stop for a second
	resetTimerSettings();
	TCA0.SINGLE.PER = 2700;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

void animateValveErrors(){
	ongoingAnimation = A_SHOWERRORS;
	
	//Blink as many times as currentThresholdLevel, then stop for a second
	resetTimerSettings();
	TCA0.SINGLE.PER = 2700;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

void animateChangeMultiplicator(){
	ongoingAnimation = A_CHANGEMULTIPLICATOR;
	
	//Blink as many times as currentThresholdLevel, then stop for a second
	resetTimerSettings();
	TCA0.SINGLE.PER = 2700;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
	
	//Blink led red, the higher the multiplicator the faster it blinks%
	/*
	resetTimerSettings();
	switch(multiplicator){
		case 1: TCA0.SINGLE.PER = 24000;
		break;
		case 2: TCA0.SINGLE.PER = 12000;
		break;
		case 3: TCA0.SINGLE.PER = 6000;
		break;
		case 4: TCA0.SINGLE.PER = 3000;
		break;
		case 5: TCA0.SINGLE.PER = 1500;
		break;
	}

	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
	*/
	
}

void stopLEDs(){
	resetTimerSettings();
	ongoingAnimation = NO_ANIMATION;
}

currentLEDAnimation getLEDAnimation(){
	return ongoingAnimation;
}
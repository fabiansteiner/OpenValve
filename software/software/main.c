/*
 * software.c
 *
 * Created: 15.10.2022 16:24:59
 * Author : Gus
 */ 

#define F_CPU 3333333

#include "common.h"
#include "ADC.h"
#include "LEDs.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

typedef enum {
	ACTIVE = 1,
	PERIODICWAKEUP = 2,
	SLEEP = 3,
	OFF = 4
}  mainStates;


volatile mainStates mState = SLEEP;
volatile uint8_t wakeUpCycles = 1;
volatile uint8_t sleepCounter = 0;
volatile uint8_t manualIrrigation = 0;
volatile uint8_t buttonSensingOn = 0;


#define PB0_LOW !(PORTB.IN & PIN0_bm)
#define PB0_INTERRUPT PORTB.INTFLAGS & PIN0_bm
#define PB0_CLEAR_INTERRUPT_FLAG PORTB.INTFLAGS &= PIN0_bm

void enablePITInterrupt(){
	RTC.PITINTCTRL |= (1<<0);
}
void disablePITInterrupt(){
	RTC.PITINTCTRL &= ~(1<<0);
}

void enablePORTBInterrupt(){
	PORTB.PIN0CTRL |=  PORT_ISC_BOTHEDGES_gc;
}

void disablePORTBInterrupt(){
	PORTB.PIN0CTRL &=  ~PORT_ISC_BOTHEDGES_gc;
}


ISR(RTC_PIT_vect)
{
	
	if(mState != ACTIVE){
		sleepCounter++;
		if(sleepCounter >= wakeUpCycles){
			disablePORTBInterrupt();
			mState = PERIODICWAKEUP;
		}
		
	}
	RTC.PITINTFLAGS = RTC_PI_bm;
	PB0_CLEAR_INTERRUPT_FLAG;
	
}


ISR(PORTB_PORT_vect)
{
	
	if(PB0_INTERRUPT)
	{
		if(PB0_LOW){
			if(mState != ACTIVE && mState != PERIODICWAKEUP){
				buttonSensingOn = 0;
				disablePITInterrupt();
				
				if(mState == SLEEP){
					
					mState = ACTIVE;
					state_change changeOfState = changeUIState(SHORT);
					changeLEDAnimation(changeOfState);
				}
				
			}
		}else{
			buttonSensingOn = 1;
		}
		PB0_CLEAR_INTERRUPT_FLAG;
		RTC.PITINTFLAGS = RTC_PI_bm;
	}
	
}

void initSleep(void)
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	
	PORTB.DIR &= ~ PIN0_bm;												//Set as input
	PORTB.PIN0CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;			//Enable Pull-UP & enable interrupt on falling edge

	//configure RTC PIT
	RTC.CLKSEL |= RTC_CLKSEL_INT1K_gc;					//Use 1024Hz clock for RTC
	RTC.PITINTCTRL |= (1<<0);							//Enable Periodic Interrupt

	//while((RTC.PITSTATUS & (1<<0)) != 0);				//Wait until a possibly ongoing synchronization of RTC.PITCTRLA register is done.
	//RTC.PITCTRLA |= RTC_PRESCALER_DIV2048_gc | (1<<0);	//Set Periodic Interrupt Cycle to 1024, interrupt should happen now every second.


}

void changePIT(uint8_t prescaler, uint8_t cycles){
	while((RTC.PITSTATUS & (1<<0)) != 0);
	RTC.PITCTRLA = prescaler | (1<<0);
	while((RTC.PITSTATUS & (1<<0)) != 0);
	wakeUpCycles = cycles;
}

void changePITInterval(){
	if(getValveState() == OPEN){
		switch(getCurrentInterval()){
			case SEC4:
				changePIT(RTC_PRESCALER_DIV2048_gc, 1);
				break;
			case SEC16:
				changePIT(RTC_PRESCALER_DIV8192_gc, 1);
				break;
			case MIN1:
				changePIT(RTC_PRESCALER_DIV16384_gc, 2);
				break;
			case MIN5:
				changePIT(RTC_PRESCALER_DIV16384_gc, 10);
				break;
			case MIN60: 
				changePIT(RTC_PRESCALER_DIV16384_gc, 120);
				break;
		}
	}else if(getValveState() == CLOSED){
		switch(getCurrentInterval()){
			case SEC4:
				changePIT(RTC_PRESCALER_DIV2048_gc, 1);
				break;
			default:
				changePIT(RTC_PRESCALER_DIV16384_gc, 120);
		}
	}
	
}

void switchOFF(){
	disablePITInterrupt();
	closeValve();
	while(getLEDAnimation() != NO_ANIMATION);
	mState = OFF;
	sleep_mode();
}


int main(void)
{
	sei();
	
	
	initSleep();
	initLEDs();
	//uart_init(9600);
	initUI();
	initADC();
	initializeValve();
	changePITInterval();
	
	_delay_ms(100);			//Let setting settle in
	uint16_t SM;
	uint8_t motorStateChanged = 0;

	
	
	sleep_mode();
	
    while (1) 
    {
		if (mState == ACTIVE || mState == PERIODICWAKEUP){
			if(mState == ACTIVE){
				
				pressType button_press = NONE;
				
				if(buttonSensingOn){
					button_press = senseMagneticSwitch();
				}
				
				state_change changeOfState = changeUIState(button_press);
				
				//Do not count timeout when in manual irrigation mode
				if(getUIState()!=MANUALIRRIGATION){
					state_change timeOutStateChange = countUITimeOut();
					if(timeOutStateChange == UI_OFF || timeOutStateChange == UI_OFF_WITHOUT_CONFIRMING){changeOfState = timeOutStateChange;}
				}
				
				if(changeOfState == FROM_SHOWBATTERY_TO_MANUALIRRIGATION){
					manualIrrigation = 1;
					openValve();
				}else if (changeOfState == FROM_MANUALIRRIGATION_TO_SHOWBATTERY){
					manualIrrigation = 0;
					closeValve();
				}
				
				changeLEDAnimation(changeOfState);
				cycleLEDAnimation();
				
				if(button_press == VERYLONG){switchOFF(); continue;}
			}
			
			if(manualIrrigation == 0){
				_delay_ms(MAINLOOP_DELAY/2);
				PORTA_OUTSET = (1<<PIN_SOILSENSORON);	//Turn on soil mositure sensor, takes around 5ms to get stable measurement
				_delay_ms(MAINLOOP_DELAY/2);
				SM = ADC_0_readSoilMoisture();
				PORTA_OUTCLR = (1<<PIN_SOILSENSORON);	//Turn off soil mositure sensor
				
				if(SM >= getCurrentThresholds().thresholdClose){
					if(getValveState() == OPEN){
						closeValve();
						motorStateChanged = 1;
					}
					}else if (SM <= getCurrentThresholds().tresholdOpen){
					if(getValveState() == CLOSED){
						openValve();
						motorStateChanged = 1;
					}
				}
			}else{
				_delay_ms(MAINLOOP_DELAY);
			}
			
			
			
			if(mState == PERIODICWAKEUP){
				if (motorStateChanged == 1) {changePITInterval();}
				sleepCounter = 0;
				motorStateChanged = 0;
				mState = SLEEP;
				enablePORTBInterrupt();
				sleep_mode();
				
			}else if(mState == ACTIVE){
				
				if(getLEDAnimation() == NO_ANIMATION){
					changePITInterval();
					sleepCounter = 0;
					motorStateChanged = 0;
					mState = SLEEP;
					enablePITInterrupt();
					sleep_mode();
				}
				
			}
		}else if(mState == SLEEP){
			sleep_mode();
		}else if(mState == OFF){
			
			if(buttonSensingOn == 0){
				
				pressType button_press = senseMagneticSwitch();
				//PORTB.OUTTGL = (1<<BLUE_LED);
				if(button_press == VERYLONG){
					
					mState = ACTIVE;
					changeUIState(SHORT);
					changeLEDAnimation(UI_STARTUP);
				}
				_delay_ms(MAINLOOP_DELAY);
			}else{
				sleep_mode();
			}
		}
		
	}
}

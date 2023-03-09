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
	SLEEP = 3
}  mainStates;


volatile mainStates mState = SLEEP;
volatile uint8_t wakeUpCycles = 1;
volatile uint8_t sleepCounter = 0;


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
			if(mState != ACTIVE){
				disablePITInterrupt();
				mState = ACTIVE;
				changeUIState(0);
			}
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

void changePITInterval(){
	if(getValveState() == OPEN){
		switch(getCurrentInterval()){
			case SEC4:
				while((RTC.PITSTATUS & (1<<0)) != 0);
				RTC.PITCTRLA = RTC_PRESCALER_DIV2048_gc | (1<<0);
				while((RTC.PITSTATUS & (1<<0)) != 0);
				wakeUpCycles = 1;
				break;
			case SEC16:
				while((RTC.PITSTATUS & (1<<0)) != 0);
				RTC.PITCTRLA = RTC_PRESCALER_DIV8192_gc | (1<<0);
				while((RTC.PITSTATUS & (1<<0)) != 0);
				wakeUpCycles = 1;
				break;
			case MIN1:
				while((RTC.PITSTATUS & (1<<0)) != 0);
				RTC.PITCTRLA = RTC_PRESCALER_DIV16384_gc | (1<<0);
				while((RTC.PITSTATUS & (1<<0)) != 0);
				wakeUpCycles = 2;
				break;
			case MIN5:
				while((RTC.PITSTATUS & (1<<0)) != 0);
				RTC.PITCTRLA = RTC_PRESCALER_DIV16384_gc | (1<<0);
				while((RTC.PITSTATUS & (1<<0)) != 0);
				wakeUpCycles = 10;
				break;
			case MIN60: 
				while((RTC.PITSTATUS & (1<<0)) != 0);
				RTC.PITCTRLA = RTC_PRESCALER_DIV16384_gc | (1<<0);
				while((RTC.PITSTATUS & (1<<0)) != 0);
				wakeUpCycles = 120;
				break;
		}
	}else if(getValveState() == CLOSED){
		while((RTC.PITSTATUS & (1<<0)) != 0);
		RTC.PITCTRLA = RTC_PRESCALER_DIV2048_gc | (1<<0);
		while((RTC.PITSTATUS & (1<<0)) != 0);
		wakeUpCycles = 1;
	}
	
}


int main(void)
{
	
	sei();
	
	PORTA_OUTSET = (1<<PIN_SOILSENSORON);	//Switch on soil mositure sensor
	
	
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
				senseMagneticSwitch();
				countUITimeOut();

			}
			
			_delay_ms(MAINLOOP_DELAY);
			SM = ADC_0_readSoilMoisture();
			
			
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
			
			if(mState == PERIODICWAKEUP){
				while(getValveState() != OPEN && getValveState() != CLOSED);	//If motor is still driving prevent going to sleep, cause ADC is disabled in sleep mode
				mState = SLEEP;
				if (motorStateChanged == 1) {changePITInterval();}
				sleepCounter = 0;
				motorStateChanged = 0;
				enablePORTBInterrupt();
				sleep_mode();
				
			}else if(mState == ACTIVE){
				
				if(getUIState() == SHOWNOTHING){
					while(getValveState() != OPEN && getValveState() != CLOSED); //If motor is still driving prevent going to sleep, cause ADC is disabled in sleep mode
					mState = SLEEP;
					changePITInterval();
					sleepCounter = 0;
					motorStateChanged = 0;
					enablePITInterrupt();
					sleep_mode();
				}
				
			}
		}else if(mState == SLEEP){
			sleep_mode();
		}
		
		
		
		
		
		
		
		
	}
}


/*

		//Switch LED with the magnet
		if((PORTB_IN & (1<<0))==0){
			PORTA_OUTSET = (1<<7);
			}else{
			PORTA_OUTCLR = (1<<7);
		}

		//Switch LED when motor stop button is pressed
		if((PORTA_IN & (1<<PIN_MOTORSTOP))==0){
			PORTB_OUTSET = (1<<3);
			}else{
			PORTB_OUTCLR = (1<<3);
		}

		//Switching motor state with the magnet
		if((PORTB_IN & (1<<0))==0){
			 _delay_ms(5);
			 while((PORTB_IN & (1<<0))==0);
			 changeMotorState();
		}
		*/
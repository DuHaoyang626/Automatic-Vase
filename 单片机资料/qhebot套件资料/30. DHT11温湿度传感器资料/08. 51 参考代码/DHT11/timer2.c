#include "timer2.h"

unsigned int timer2_interrupt_counter_V = 0;

void timer2_IR() interrupt 5{
    static unsigned int timer2_interrupt_counter = 0; 
		TF2 = 0;
    timer2_interrupt_counter++;
    if( timer2_interrupt_counter >= timer2_interrupt_counter_V ) {  
        timer2_interrupt_counter = 0;
				TIMER_CallBackTable[2]();
    }
}

void timer2_start(int ms, TIMER_CallBackType AppCBFun){
	timer2_interrupt_counter_V = ms;
	TIMER_CallBackTable[2] = AppCBFun;
	
	T2MOD = 0;		 
	T2CON = 0;		 

	RCAP2L = 0x66;		 
	RCAP2H = 0xFC;		 

	EA = 1;
	ET2 = 1;
	TR2 = 1; 
	PT2 = 1;
	TF2 = 0;
}


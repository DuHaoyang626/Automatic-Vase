#include "timer.h"

int timer0_interrupt_counter_V = 0;

typedef unsigned int u16;
typedef unsigned char u8;

void timer0_deal_f(){
	  static u16 timer0_interrupt_counter = 0;
		TR0 = 0;
    TH0 = 0xFC;
    TL0 = 0x66;
	  TR0 = 1;
    timer0_interrupt_counter++;
    if( timer0_interrupt_counter >= timer0_interrupt_counter_V ) {  
        timer0_interrupt_counter = 0;
				TIMER_CallBackTable[0]();
    }
}


void timer0_IR() interrupt 1{
		{timer0_deal_f();}
}


void timer0_start(int ms, TIMER_CallBackType AppCBFun){
	timer0_interrupt_counter_V = ms;

	TIMER_CallBackTable[0] = AppCBFun;
	
	TMOD &= 0xF0;		 
	TMOD |= 0x01;		 
  TH0 = 0xFC;
  TL0 = 0x66;
  EA = 1;   
  ET0 = 1; 
  TR0 = 1; 	
	PT0 = 1;

	if(timer0_interrupt_counter_V < 1 || timer0_interrupt_counter_V > 60000 ){ TR0  = 0;}
}

void timer0_stop(){
	ET0 = 0; 
  TR0 = 0; 
}

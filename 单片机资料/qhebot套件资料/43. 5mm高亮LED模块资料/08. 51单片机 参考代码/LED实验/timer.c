#include "timer.h"

int timer0_interrupt_counter_V = 0;
int timer1_interrupt_counter_V = 0;
int timer2_interrupt_counter_V = 0;

int timer0_flag = 0;
int timer1_flag = 0;
int timer0_pwm_flag = 0;
int timer1_pwm_flag = 0;

int timer0_reload_pwm_H_level_H_byte_time = 0;
int timer0_reload_pwm_H_level_L_byte_time = 0;
int timer0_reload_pwm_L_level_H_byte_time = 0;
int timer0_reload_pwm_L_level_L_byte_time = 0;
int timer0_PWM_pin_level_flag = 0;

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

void timer0_pwm_deal_f(){
	if(timer0_PWM_pin_level_flag == 1){
		timer0_PWM_pin_level_flag = 0;
		//改变电平为低电平
		//TR0 = 0;
    TH0 = timer0_reload_pwm_L_level_H_byte_time;
    TL0 = timer0_reload_pwm_L_level_L_byte_time;
	  //TR0 = 1;
		PWM0_PIN = 0;
	}else{
		timer0_PWM_pin_level_flag = 1;
		//TR0 = 0;
    TH0 = timer0_reload_pwm_H_level_H_byte_time;
    TL0 = timer0_reload_pwm_H_level_L_byte_time;
	  //TR0 = 1;
		PWM0_PIN = 1;
	}
}


void timer0_IR() interrupt 1{
		if(timer0_flag == 1){timer0_deal_f();}
		else if(timer0_pwm_flag == 1){ timer0_pwm_deal_f(); }
}


void timer0_start(int ms, TIMER_CallBackType AppCBFun){
	timer0_flag = 1;
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

void timer1_IR() interrupt 3{
    static u16 timer1_interrupt_counter = 0;
	  TR1 = 0; 
    TH1 = 0xFC;
    TL1 = 0x6b;
	  TR1 = 1; 
    timer1_interrupt_counter++;
    if( timer1_interrupt_counter >= timer1_interrupt_counter_V ) {  
        timer1_interrupt_counter = 0;
				TIMER_CallBackTable[1]();
    }
}

void timer1_start(int ms, TIMER_CallBackType AppCBFun){
	timer1_flag = 1;
	timer1_interrupt_counter_V = ms;
	TIMER_CallBackTable[1] = AppCBFun;
	
	TMOD &= 0x0f;		 
	TMOD |= 0x10;		 
  TH1 = 0xFC;
  TL1 = 0x6b;
  EA = 1;   
  ET1 = 1; 
  TR1 = 1; 
	
  if(timer1_interrupt_counter_V < 1 || timer1_interrupt_counter_V > 60000 ){ TR1 = 0;}
}

void timer1_stop(){
	ET1 = 0; 
  TR1 = 0; 
}

void timer2_IR() interrupt 5{
    static u16 timer2_interrupt_counter = 0; 
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



void timer0_pwm_start(int frequency, float duty){ 
	long int tmp_t = 1000000*11.0592/12/frequency ;
	long int	timer0_reload_pwm_H_level_H_byte_time_TMP = 0;
	long int	timer0_reload_pwm_H_level_L_byte_time_TMP = 0;
	long int	timer0_reload_pwm_L_level_H_byte_time_TMP = 0;
	long int	timer0_reload_pwm_L_level_L_byte_time_TMP = 0;

	if( frequency >= 3000){tmp_t = (long int)tmp_t*6.8/10;}
	else if( frequency >= 2000){tmp_t = (long int)tmp_t*7.5/10;}
	else if( frequency >= 900){ tmp_t = tmp_t*9/10; }

	timer0_pwm_flag = 1;
	
	if(duty >= 1 && duty <= 99 && frequency >=17){
			timer0_reload_pwm_H_level_H_byte_time_TMP = (long int)(65536 - tmp_t*duty/100 + 0 )/256;
			timer0_reload_pwm_H_level_L_byte_time_TMP = (long int)(65536 - tmp_t*duty/100 + 0 )%256;
			timer0_reload_pwm_L_level_H_byte_time_TMP = (long int)(65536 - tmp_t*(100.0-duty)/100 + 0 )/256;
			timer0_reload_pwm_L_level_L_byte_time_TMP = (long int)(65536 - tmp_t*(100.0-duty)/100 + 0 )%256;
		  if(timer0_reload_pwm_L_level_H_byte_time > 65536 || timer0_reload_pwm_L_level_L_byte_time > 65536 ){
				timer0_reload_pwm_L_level_L_byte_time_TMP = 65536;
				timer0_reload_pwm_L_level_L_byte_time_TMP = 65536;
			}else if(timer0_reload_pwm_H_level_H_byte_time_TMP > 65536 || timer0_reload_pwm_H_level_L_byte_time_TMP > 65536 ){
				timer0_reload_pwm_H_level_H_byte_time_TMP = 65536;
				timer0_reload_pwm_H_level_L_byte_time_TMP = 65536;
			}
			
			timer0_reload_pwm_H_level_H_byte_time = (int)timer0_reload_pwm_H_level_H_byte_time_TMP;
			timer0_reload_pwm_H_level_L_byte_time = (int)timer0_reload_pwm_H_level_L_byte_time_TMP;
			timer0_reload_pwm_L_level_H_byte_time = (int)timer0_reload_pwm_L_level_H_byte_time_TMP;
			timer0_reload_pwm_L_level_L_byte_time = (int)timer0_reload_pwm_L_level_L_byte_time_TMP;
		
			PWM0_PIN = 1;
		  timer0_PWM_pin_level_flag = 1;
		
			TMOD &= 0xF0;		 
			TMOD |= 0x01;		 
			TH0 = timer0_reload_pwm_H_level_H_byte_time;
			TL0 = timer0_reload_pwm_H_level_L_byte_time;
			EA = 1;   
			ET0 = 1; 
			PT0=1;
			TR0 = 1; 	
	}
	else if(duty < 1){	PWM0_PIN = 0; }
	else if(duty > 99){	PWM0_PIN = 1; }
	else if(frequency < 17 && frequency > 0){
		
	}
}
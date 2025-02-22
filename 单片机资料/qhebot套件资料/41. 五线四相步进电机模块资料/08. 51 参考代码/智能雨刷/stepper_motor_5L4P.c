
#include "stepper_motor_5L4P.h"

unsigned char code F_Rotation[4]={0xf1,0xf2,0xf4,0xf8};  
unsigned char code B_Rotation[4]={0xf8,0xf4,0xf2,0xf1};  


bit flag_pos_inv = 0;

int stepper_c  = 0;

void stepper_run(){
		 STEPPER_PIN |= 0X0F;
     if(flag_pos_inv == 0) STEPPER_PIN &= F_Rotation[stepper_c++];  
     else if(flag_pos_inv == 1) STEPPER_PIN &= B_Rotation[stepper_c++];  
		 if(stepper_c >= 4) stepper_c = 0;
}


void stepper_init(unsigned char rot_per_sec, bit flag_pos_inv_tmp ){
	if(rot_per_sec < 2){ rot_per_sec = 2;}
	else if(rot_per_sec > 10){ rot_per_sec = 10;}
	flag_pos_inv = flag_pos_inv_tmp;
	
	timer2_start(rot_per_sec, stepper_run);
	
}
/*
*************************************************************************
*硬件平台: STC89C52RC
*开发平台：keil 4.7
*版权    ：沁和智能科技
*开始时间：2019.7.11
*结束时间：2019.7.11
*作者：    张文超
*备注：    未经允许，用于商业用途，必究责任
*************************************************************************
*/

#include <reg52.h>
#include "DS18B20.h"
#include <stdio.h>


void delay_ms(unsigned int t)
{
    unsigned char a,b;
    while(t--)
    {
      for(b=102;b>0;b--)
      for(a=3;a>0;a--);
    }
}

void InitUART(void) 
{
    TH1 = 0xFD;	 //??11.0592mhz ?????9600
    TL1 = TH1;
    TMOD |= 0x20;	  
    SCON = 0x50;	 
    ES = 1;			  
    TR1 = 1;		 
    TI = 1;    
		PS = 0;
	  EA = 1;	  	 
}

void main(){
	float tmp = 0;
	InitUART();
	DS18B20Init();
	//delay_ms(7000);
	while(1){
		delay_ms(500);
		//tmp = read_temp();
		//tmp = tmp * 0.0625;
		printf("zazaaaa %2f\n", Get18B20Temp() );
	}  
}
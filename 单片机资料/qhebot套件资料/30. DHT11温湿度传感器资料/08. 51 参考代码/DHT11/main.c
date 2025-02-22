/*
*************************************************************************
*硬件平台: STC89C52RC
*开发平台：keil 4.7
*版权    ：沁和智能科技
*开始时间：2019.7.11
*结束时间：2019.7.11
*作者：    张文超
*备注：     --- 未经允许，用于商业用途，必究责任
*************************************************************************
*/


#include <reg52.h>
#include "LCD12864.h"
#include "timer2.h"
#include "DHT11.h"


sbit RUN_LED = P3^5;



void delay(unsigned int i){   //延时函数
	while(i--);
}

void timer2Deal(){
	static unsigned int i = 0;
	unsigned char lcd1602_data[3] = {0};

	if(i++ > 50000) i = 0;
	if(i % 25 == 0){
		RUN_LED = ~RUN_LED;
		
		readData();

		if(temperature_H >= 100) temperature_H = 99;
		lcd1602_data[0] = temperature_H/10 + 48;
		lcd1602_data[1] = temperature_H%10 + 48;
		lcd1602_data[2] = '\0';
		PutStr(2, 3, lcd1602_data );
		
		if(humidity_H >= 100) humidity_H = 99;
		lcd1602_data[0] = humidity_H/10 + 48;
		lcd1602_data[1] = humidity_H%10 + 48;
		lcd1602_data[2] = '\0';
		PutStr(3, 3, lcd1602_data );
		
	} 
}

void main(void){
	
	LcmInit();
	
	LcmClearTXT();	   //LCD12864清屏		
	
	PutStr(2,0, "温度：");
	PutStr(3,0, "湿度：");
	

	delay(100);
	
	timer2_start(20, timer2Deal);

	while(1){


	}
}




/*
*************************************************************************
*硬件平台: STC89C52RC
*开发平台：keil 4.7
*版权    ：沁和智能科技
*作者：    张文超
*备注：    未经允许，用于商业用途，必究责任
*************************************************************************
*/

#include <reg52.h>

sbit LEDR = P1^0;



//小延时函数
void delay(unsigned int i){
	while(i--);
}

//主函数
void main(){
	while(1){
		LEDR = 0;  
		delay(50000); delay(50000); delay(50000); delay(50000);
		LEDR = 1;  
		delay(50000); delay(50000); delay(50000); delay(50000);
	}  
}




#define _DS18B20_C
#include "DS18B20.h"


void Delay_DS18B20(int num)
{
  while(num--) ;
}

/* 
* 函数名：Init_DS18B20
* 描  述：初始化DS18B20复位总线，获取存在脉冲，以启动一次读写操作
*/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ复位
  Delay_DS18B20(8);    //稍做延时
  DQ = 0;         //单片机将DQ拉低
  Delay_DS18B20(80);   //精确延时，大于480us
  DQ = 1;         //拉高总线
  Delay_DS18B20(14);
  x = DQ;           //稍做延时后，如果x=0则初始化成功，x=1则初始化失败
  Delay_DS18B20(20);
}
/*
* 函数名：Write18B20
* 描  述：向DS18B20写入一个字节数据
* 输  入：dat - 待写入的字节数据
* 输  出：无
*/
void Write18B20(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
/*
* 函数名：Read18B20
* 描  述：向DS18B20写入一个字节数据
* 输  入：无
* 输  出：返回值 - 读到的字节数据
*/
unsigned char Read18B20(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}

/*
* 函数名：Get18B20Temp
* 描  述：读取DS18B20的温度值，并开始下一次转换
* 输  入：无
* 输  出：返回值 - 读取到的温度值（只保留整数部分）
*/
float Get18B20Temp(void)
{
  unsigned int Temp_L,Temp_H;
	unsigned int Temputer;
	unsigned char Temp_Flag;
	float tmp = 0;

    Init_DS18B20();
	Write18B20(0xCC); //发Skip ROM命令
	Write18B20(0x44); //发读命令
 	Init_DS18B20();
 	Write18B20(0xCC); //发Skip ROM命令
	Write18B20(0xBE); //发读命令
	Temp_L = Read18B20();  //温度低8位
	Temp_H = Read18B20();  //温度高8位
	
	if((Temp_H & 0xF8) != 0x00)
	{
		Temp_Flag = 1;
		Temputer = ((Temp_H<<8)|Temp_L);
		Temputer = ((~Temputer) + 1);
	}
	else
	{
		Temp_Flag = 0;
		Temputer = (Temp_H<<8)|Temp_L;
	}
	tmp = Temputer;
	tmp = tmp*0.0625;
	return (tmp);
}

/*
* 函数名：DS18B20Init
* 描  述：DS18B20初始化函数，开始一次转换以备直接读取
*/
void DS18B20Init(void)
{
	Init_DS18B20();
	Write18B20(0xCC);  //Skip ROM
	Write18B20(0x44);  //发转换命令
	Init_DS18B20();
 	Write18B20(0xCC); //发Skip ROM命令
	Write18B20(0xBE); //发读命令
}
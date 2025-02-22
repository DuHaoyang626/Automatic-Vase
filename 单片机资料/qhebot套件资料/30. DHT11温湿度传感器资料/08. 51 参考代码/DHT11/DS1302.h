#ifndef DS1302_H
#define DS1302_H

#include <reg52.h>
#include <intrins.h>
//DS1302引脚定义
sbit RST=P2^3;
sbit IO=P2^2;
sbit SCLK=P2^1;

//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

void ds1302_init(void);
void ds1302_write_byte(unsigned char addr, unsigned char d);
unsigned char ds1302_read_byte(unsigned char addr);
void ds1302_write_time(unsigned char year,unsigned char month,unsigned char day,unsigned char week,unsigned char hour,unsigned char min,unsigned char sec);
void ds1302_read_data(unsigned char dat[7]);

#endif

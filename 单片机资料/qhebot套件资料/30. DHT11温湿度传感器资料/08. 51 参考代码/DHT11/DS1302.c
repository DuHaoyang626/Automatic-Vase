#include "DS1302.h"
#include <stdio.h>

//DS1302初始化函数
void ds1302_init(void) {
	RST=0;			//RST脚置低
	SCLK=0;			//SCLK脚置低
}


/********************************************************************
* 名称 : dectobcd(uchar dec)
* 功能 : DEC码转换为BCD码
* 输入 : dec码
* 输出 : bcd码
***********************************************************************/
unsigned char dectobcd(unsigned char dec)
{
	unsigned char bcd;
	bcd = 0;
	while(dec >= 10)
	{              
		dec -= 10;                         
		bcd++;
	} 
	bcd <<= 4;
	bcd |= dec;
	return bcd;
}

//向DS302写入时钟数据
void ds1302_write_time(unsigned char year,unsigned char month,unsigned char day,unsigned char week,unsigned char hour,unsigned char min,unsigned char sec) {
	ds1302_init();
	ds1302_write_byte(ds1302_control_add,0x00);	  //关闭写保护 
	ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	//涓流充电 
	ds1302_write_byte(ds1302_year_add,  dectobcd(year));		//年 
	ds1302_write_byte(ds1302_month_add, dectobcd(month));	  //月 
	ds1302_write_byte(ds1302_date_add,  dectobcd(day));		  //日 
	ds1302_write_byte(ds1302_hr_add,    dectobcd(hour));		//时 
	ds1302_write_byte(ds1302_min_add,   dectobcd(min));		  //分
	ds1302_write_byte(ds1302_sec_add,   dectobcd(sec));		  //秒
	ds1302_write_byte(ds1302_day_add,   dectobcd(week));		//周 
	ds1302_write_byte(ds1302_control_add,0x80);	  //打开写保护     
}


/********************************************************************
* 名称 : bcdtodec(uchar bcd)
* 功能 : BCD码转换为DEC码
* 输入 : bcd码
* 输出 : dec码
***********************************************************************/
unsigned char bcdtodec(unsigned char bcd)
{
	unsigned char data1;
	data1 = bcd & 0x0f;     //取BCD低4位
	bcd = bcd & 0x70;       //剔除BCD的最高位和低4位。
	data1 += bcd >> 1;
	data1 += bcd >> 3;      //用位移代替乘法运算
	return data1;
}

//从DS302读出时钟数据
void ds1302_read_data(unsigned char dat[7])  
{
	dat[0]=bcdtodec (ds1302_read_byte(ds1302_year_add) );		//年 
	dat[1]=bcdtodec (ds1302_read_byte(ds1302_month_add) );		//月 
	dat[2]=bcdtodec (ds1302_read_byte(ds1302_date_add) );		//日 
	dat[3]=bcdtodec (ds1302_read_byte(ds1302_day_add) );		//周 	
	dat[4]=bcdtodec (ds1302_read_byte(ds1302_hr_add) );		//时 
	dat[5]=bcdtodec (ds1302_read_byte(ds1302_min_add) );		//分 
	dat[6]=bcdtodec ((ds1302_read_byte(ds1302_sec_add))&0x7f );//秒，屏蔽秒的第7位，避免超出59
}


//向DS1302写入一字节数据
void ds1302_write_byte(unsigned char addr, unsigned char d){
	unsigned char i;
	RST=1;					//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCLK=1;      //产生时钟
		SCLK=0;
		addr = addr >> 1;
		}	
	//写入数据：d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCLK=1;    //产生时钟
		SCLK=0;
		d = d >> 1;
		}
	RST=0;		//停止DS1302总线
}

//从DS1302读出一字节数据
unsigned char ds1302_read_byte(unsigned char addr) {

	unsigned char i,temp;	
	RST=1;					//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCLK=1;
		SCLK=0;
		addr = addr >> 1;
		}	
	//输出数据：temp
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (IO) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		SCLK=1;
		SCLK=0;
		}	
	RST=0;					//停止DS1302总线
	return temp;
}
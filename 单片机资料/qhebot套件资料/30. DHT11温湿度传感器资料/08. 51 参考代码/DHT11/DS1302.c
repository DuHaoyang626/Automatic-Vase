#include "DS1302.h"
#include <stdio.h>

//DS1302��ʼ������
void ds1302_init(void) {
	RST=0;			//RST���õ�
	SCLK=0;			//SCLK���õ�
}


/********************************************************************
* ���� : dectobcd(uchar dec)
* ���� : DEC��ת��ΪBCD��
* ���� : dec��
* ��� : bcd��
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

//��DS302д��ʱ������
void ds1302_write_time(unsigned char year,unsigned char month,unsigned char day,unsigned char week,unsigned char hour,unsigned char min,unsigned char sec) {
	ds1302_init();
	ds1302_write_byte(ds1302_control_add,0x00);	  //�ر�д���� 
	ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	//������ 
	ds1302_write_byte(ds1302_year_add,  dectobcd(year));		//�� 
	ds1302_write_byte(ds1302_month_add, dectobcd(month));	  //�� 
	ds1302_write_byte(ds1302_date_add,  dectobcd(day));		  //�� 
	ds1302_write_byte(ds1302_hr_add,    dectobcd(hour));		//ʱ 
	ds1302_write_byte(ds1302_min_add,   dectobcd(min));		  //��
	ds1302_write_byte(ds1302_sec_add,   dectobcd(sec));		  //��
	ds1302_write_byte(ds1302_day_add,   dectobcd(week));		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);	  //��д����     
}


/********************************************************************
* ���� : bcdtodec(uchar bcd)
* ���� : BCD��ת��ΪDEC��
* ���� : bcd��
* ��� : dec��
***********************************************************************/
unsigned char bcdtodec(unsigned char bcd)
{
	unsigned char data1;
	data1 = bcd & 0x0f;     //ȡBCD��4λ
	bcd = bcd & 0x70;       //�޳�BCD�����λ�͵�4λ��
	data1 += bcd >> 1;
	data1 += bcd >> 3;      //��λ�ƴ���˷�����
	return data1;
}

//��DS302����ʱ������
void ds1302_read_data(unsigned char dat[7])  
{
	dat[0]=bcdtodec (ds1302_read_byte(ds1302_year_add) );		//�� 
	dat[1]=bcdtodec (ds1302_read_byte(ds1302_month_add) );		//�� 
	dat[2]=bcdtodec (ds1302_read_byte(ds1302_date_add) );		//�� 
	dat[3]=bcdtodec (ds1302_read_byte(ds1302_day_add) );		//�� 	
	dat[4]=bcdtodec (ds1302_read_byte(ds1302_hr_add) );		//ʱ 
	dat[5]=bcdtodec (ds1302_read_byte(ds1302_min_add) );		//�� 
	dat[6]=bcdtodec ((ds1302_read_byte(ds1302_sec_add))&0x7f );//�룬������ĵ�7λ�����ⳬ��59
}


//��DS1302д��һ�ֽ�����
void ds1302_write_byte(unsigned char addr, unsigned char d){
	unsigned char i;
	RST=1;					//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCLK=1;      //����ʱ��
		SCLK=0;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCLK=1;    //����ʱ��
		SCLK=0;
		d = d >> 1;
		}
	RST=0;		//ֹͣDS1302����
}

//��DS1302����һ�ֽ�����
unsigned char ds1302_read_byte(unsigned char addr) {

	unsigned char i,temp;	
	RST=1;					//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
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
	//������ݣ�temp
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
	RST=0;					//ֹͣDS1302����
	return temp;
}
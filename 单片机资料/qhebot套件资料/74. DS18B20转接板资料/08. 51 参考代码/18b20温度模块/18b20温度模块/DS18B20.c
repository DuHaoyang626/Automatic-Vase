
#define _DS18B20_C
#include "DS18B20.h"


void Delay_DS18B20(int num)
{
  while(num--) ;
}

/* 
* ��������Init_DS18B20
* ��  ������ʼ��DS18B20��λ���ߣ���ȡ�������壬������һ�ζ�д����
*/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ��λ
  Delay_DS18B20(8);    //������ʱ
  DQ = 0;         //��Ƭ����DQ����
  Delay_DS18B20(80);   //��ȷ��ʱ������480us
  DQ = 1;         //��������
  Delay_DS18B20(14);
  x = DQ;           //������ʱ�����x=0���ʼ���ɹ���x=1���ʼ��ʧ��
  Delay_DS18B20(20);
}
/*
* ��������Write18B20
* ��  ������DS18B20д��һ���ֽ�����
* ��  �룺dat - ��д����ֽ�����
* ��  ������
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
* ��������Read18B20
* ��  ������DS18B20д��һ���ֽ�����
* ��  �룺��
* ��  ��������ֵ - �������ֽ�����
*/
unsigned char Read18B20(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}

/*
* ��������Get18B20Temp
* ��  ������ȡDS18B20���¶�ֵ������ʼ��һ��ת��
* ��  �룺��
* ��  ��������ֵ - ��ȡ�����¶�ֵ��ֻ�����������֣�
*/
float Get18B20Temp(void)
{
  unsigned int Temp_L,Temp_H;
	unsigned int Temputer;
	unsigned char Temp_Flag;
	float tmp = 0;

    Init_DS18B20();
	Write18B20(0xCC); //��Skip ROM����
	Write18B20(0x44); //��������
 	Init_DS18B20();
 	Write18B20(0xCC); //��Skip ROM����
	Write18B20(0xBE); //��������
	Temp_L = Read18B20();  //�¶ȵ�8λ
	Temp_H = Read18B20();  //�¶ȸ�8λ
	
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
* ��������DS18B20Init
* ��  ����DS18B20��ʼ����������ʼһ��ת���Ա�ֱ�Ӷ�ȡ
*/
void DS18B20Init(void)
{
	Init_DS18B20();
	Write18B20(0xCC);  //Skip ROM
	Write18B20(0x44);  //��ת������
	Init_DS18B20();
 	Write18B20(0xCC); //��Skip ROM����
	Write18B20(0xBE); //��������
}
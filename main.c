#include"reg52.h"
#include "intrins.h"


//���Ͷ���
typedef unsigned int u16;
typedef unsigned char u8;


//�ӿڶ���

#define LCD1602_DATAPORT P0//LCD���ݶ˿�
sbit DIN  = P0^4;	  //ADC����
sbit CS   = P0^5;	  //ADCƬѡ
sbit CLK  = P0^6;	  //ADCʱ��
sbit DOUT = P0^7;	  //ADC���
#define KEY_MATRIX_PORT	P1//������������

sbit PUMP=P2^0;//ˮ�ý���P2^0
sbit SPRAY=P2^1;//��������2^1
sbit LIGHT=P2^2;//LED����ƽ���P2^2
sbit MOISTURE_AIR=P2^3;//����ʪ�ȴ���������P2^3
sbit TEMPATURE=P2^4;//�¶ȴ���������P2^4
sbit LCD1602_RW=P2^5;//LCD��дѡ��
sbit LCD1602_RS=P2^6;//LCD��������ѡ��
sbit LCD1602_E=P2^7; //LCDʹ���ź�

sbit STEPMOTOR1=P3^0;//�������1����P3^0����������
sbit STEPMOTOR2=P3^1;//�������2����P3^1����������
sbit STEPMOTOR3=P3^2;//�������3����P3^2����������
sbit STEPMOTOR4=P3^3;//�������4����P3^3����������

// 
// sbit MOISTURE_SOIL=P3^1;//����ʪ�ȴ���������ADC
// sbit WATER_LEVEL=P3^2;//ˮλ����������ADC^E4
//sbit LIGHT_INTENSITY=P2^4;//����ǿ�ȴ���������ADC^A4
//sbit WIFI_IO=P2^0;//Ԥ��WIFI�ӿ�Ϊ������

//ȫ������

//LCD1602���ݿ�4λ��8λ���壬��Ϊ1����ΪLCD1602��λ���ݿ���������֮Ϊ8λ
#define LCD1602_4OR8_DATA_INTERFACE	1	//Ĭ��ʹ��8λ���ݿ�LCD1602

// ���岽������ٶȣ�ֵԽС���ٶ�Խ��
#define STEPMOTOR_MAXSPEED        100//�����������ٶȣ�����С��1
#define STEPMOTOR_MINSPEED        350//���������С�ٶ�
#define STEPMOTOR_SHIELD_HIGHT    10//�ڹ��߶�
#define STEPMOTOR_SHIELD_RANGE    300//�ڹ���ƶ�ʱ�����
u16 StepMotor_status=0;//��������ڹ��״̬������0Ϊ���λ�ã�1Ϊ���λ��

//��������

//��ʱ����������int x����ʱx/100����
void delay_10us(u16 ten_us){
while(ten_us--);
}

//��ʱ����������int x����ʱx����
void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

//��ģת����������
/*******************************************************************************
* �� �� ��       : xpt2046_wirte_data
* ��������		 : XPT2046д����
* ��    ��       : dat��д�������
* ��    ��    	 : ��
*******************************************************************************/
void xpt2046_wirte_data(u8 dat)
{
	u8 i;

	CLK = 0;
	_nop_();
	for(i=0;i<8;i++)//ѭ��8�Σ�ÿ�δ���һλ����һ���ֽ�
	{
		DIN = dat >> 7;//�ȴ���λ�ٴ���λ
		dat <<= 1;//����λ�Ƶ���λ
		CLK = 0;//CLK�ɵ͵��߲���һ�������أ��Ӷ�д������
		_nop_();	
		CLK = 1;
		_nop_();
	}
}

/*******************************************************************************
* �� �� ��       : xpt2046_read_data
* ��������		 : XPT2046������
* ��    ��       : ��
* ��    ��    	 : XPT2046����12λ����
*******************************************************************************/
u16	xpt2046_read_data(void)
{
	u8 i;
	u16 dat=0;

	CLK = 0;
	_nop_();
	for(i=0;i<12;i++)//ѭ��12�Σ�ÿ�ζ�ȡһλ������һ���ֽ��������Է���ֵ������u16
	{
		dat <<= 1;
		CLK = 1;
		_nop_();
		CLK = 0; //CLK�ɸߵ��Ͳ���һ���½��أ��Ӷ���ȡ����
		_nop_();
		dat |= DOUT;//�ȶ�ȡ��λ���ٶ�ȡ��λ��	
	}
	return dat;	
}

/*******************************************************************************
* �� �� ��       : xpt2046_read_adc_value
* ��������		 : XPT2046��AD����
* ��    ��       : cmd��ָ��
* ��    ��    	 : XPT2046����ADֵ
*******************************************************************************//////////////////////////////////////////////////
u16 xpt2046_read_adc_value(u8 cmd)
{
	u8 i;
	u16 adc_value=0;

	CLK = 0;//������ʱ��
	CS  = 0;//ʹ��XPT2046
	xpt2046_wirte_data(cmd);//����������
	for(i=6; i>0; i--);//��ʱ�ȴ�ת�����
	CLK = 1;
	_nop_();
	CLK = 0;//����һ��ʱ�ӣ����BUSY
	_nop_();
	adc_value=xpt2046_read_data();
	CS = 1;//�ر�XPT2046
	return adc_value;
}

//LCD��������
/*******************************************************************************
* �� �� ��       : lcd1602_write_cmd
* ��������		 : LCD1602д����
* ��    ��       : cmd��ָ��
* ��    ��    	 : ��
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8λLCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//ѡ������
	LCD1602_RW=0;//ѡ��д
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��	
}
#else	//4λLCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//ѡ������
	LCD1602_RW=0;//ѡ��д
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��
	
	LCD1602_DATAPORT=cmd<<4;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��	
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_write_data
* ��������		 : LCD1602д����
* ��    ��       : dat������
* ��    ��    	 : ��
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8λLCD
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//ѡ������
	LCD1602_RW=0;//ѡ��д
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��		
}
#else
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//ѡ������
	LCD1602_RW=0;//ѡ��д
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��
	
	LCD1602_DATAPORT=dat<<4;//׼������
	delay_ms(1);
	LCD1602_E=1;//ʹ�ܽ�E��������д��
	delay_ms(1);
	LCD1602_E=0;//ʹ�ܽ�E���������д��		
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_init
* ��������		 : LCD1602��ʼ��
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************//////////////////////////////////////////////////
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8λLCD
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);//��������8λ����ʾ2�У�5*7����/�ַ�
	lcd1602_write_cmd(0x0c);//��ʾ���ܿ����޹�꣬�����˸
	lcd1602_write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
	lcd1602_write_cmd(0x01);//����	
}
#else
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x28);//��������4λ����ʾ2�У�5*7����/�ַ�
	lcd1602_write_cmd(0x0c);//��ʾ���ܿ����޹�꣬�����˸
	lcd1602_write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
	lcd1602_write_cmd(0x01);//����	
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_clear
* ��������		 : LCD1602����
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

/*******************************************************************************
* �� �� ��       : lcd1602_show_string
* ��������		 : LCD1602��ʾ�ַ�
* ��    ��       : x,y����ʾ���꣬x=0~15��y=0~1;
				   str����ʾ�ַ���
* ��    ��    	 : ��
*******************************************************************************//////////////////////////////////////////////////
void lcd1602_show_string(u8 x,u8 y,u8 *str)
{
	u8 i=0;

	if(y>1||x>15)return;//���в���������ǿ���˳�

	if(y<1)	//��1����ʾ
	{	
		while(*str!='\0')//�ַ�������'\0'��β��ֻҪǰ�������ݾ���ʾ
		{
			if(i<16-x)//����ַ����ȳ�����һ����ʾ��Χ�����ڵڶ��м�����ʾ
			{
				lcd1602_write_cmd(0x80+i+x);//��һ����ʾ��ַ����	
			}
			else
			{
				lcd1602_write_cmd(0x40+0x80+i+x-16);//�ڶ�����ʾ��ַ����	
			}
			lcd1602_write_data(*str);//��ʾ����
			str++;//ָ�����
			i++;	
		}	
	}
	else	//��2����ʾ
	{
		while(*str!='\0')
		{
			if(i<16-x) //����ַ����ȳ����ڶ�����ʾ��Χ�����ڵ�һ�м�����ʾ
			{
				lcd1602_write_cmd(0x80+0x40+i+x);	
			}
			else
			{
				lcd1602_write_cmd(0x80+i+x-16);	
			}
			lcd1602_write_data(*str);
			str++;
			i++;	
		}	
	}				
}

//������̺���
/*******************************************************************************
* �� �� ��       : key_matrix_flip_scan
* ��������		 : ʹ���߷�תɨ�跽���������󰴼��Ƿ��£������򷵻ض�Ӧ��ֵ
* ��    ��       : ��
* ��    ��    	 : key_value��1-16����ӦS1-S16����
				   0������δ����
*******************************************************************************//////////////////////////////////////////////////
u8 key_matrix_flip_scan(void)
{
	static u8 key_value=0;

	KEY_MATRIX_PORT=0x0f;//�������и�ֵ0����ȫΪ1
	if(KEY_MATRIX_PORT!=0x0f)//�жϰ����Ƿ���
	{
		delay_10us(1000);//����
		if(KEY_MATRIX_PORT!=0x0f)
		{
			//������
			KEY_MATRIX_PORT=0x0f;
			switch(KEY_MATRIX_PORT)//������Ϊ0���������º����ֵ	
			{
				case 0x07: key_value=1;break;
				case 0x0b: key_value=2;break;
				case 0x0d: key_value=3;break;
				case 0x0e: key_value=4;break;
			}
			//������
			KEY_MATRIX_PORT=0xf0;
			switch(KEY_MATRIX_PORT)//������Ϊ0���������º�ļ�ֵ	
			{
				case 0x70: key_value=key_value;break;
				case 0xb0: key_value=key_value+4;break;
				case 0xd0: key_value=key_value+8;break;
				case 0xe0: key_value=key_value+12;break;
			}
			while(KEY_MATRIX_PORT!=0xf0);//�ȴ������ɿ�	
		}
	}
	else
		key_value=0;		
	
	return key_value;		
}

//���������������
/*******************************************************************************
* �� �� ��       : step_motor_28BYJ48_send_pulse
* ��������		 : ���һ�����ݸ�ULN2003�Ӷ�ʵ���򲽽��������һ������
* ��    ��       : step��ָ��������ţ���ѡֵ0~7
				   dir������ѡ��,1��˳ʱ��,0����ʱ��
* ��    ��    	 : ��
*******************************************************************************/
void step_motor_28BYJ48_send_pulse(u8 step,u8 dir)
{
	u8 temp=step;
	
	if(dir==0)	//���Ϊ��ʱ����ת
		temp=7-step;//���������ź�
	switch(temp)//8�����Ŀ��ƣ�A->AB->B->BC->C->CD->D->DA
	{
		case 0: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 1: STEPMOTOR1=1;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 2: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 3: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 4: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 5: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=1;break;
		case 6: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		case 7: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		default: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;//ֹͣ����	
	}			
}

//����������1�ϵ����ˣ�0�µ��׶�//////////////////////////////////////////////////////////////////////////////////////////////////
void step_motor_control(u16 input){
    if(input!=StepMotor_status)
    {
        u8 key=0;
	    u8 dir=0;//Ĭ����ʱ�뷽��
	    u8 speed=STEPMOTOR_MAXSPEED;//Ĭ������ٶ���ת
	    u8 step=0;
        u16 time=0;
        u16 usetime=STEPMOTOR_SHIELD_HIGHT*STEPMOTOR_SHIELD_RANGE;

        if(input==1)
        {
            while(time<=usetime){
                step_motor_28BYJ48_send_pulse(step++,dir);
								time++;
								if(step==8)step=0;		
								delay_10us(speed);
            }
			StepMotor_status=1;
        }
        if(input==0)
        {
            dir=!dir;
            while(time<=usetime){
                step_motor_28BYJ48_send_pulse(step++,dir);
								time++;
								if(step==8)step=0;		
								delay_10us(speed);
            }
			StepMotor_status=0;
        }
    }
}

//ˮ�ò������������빩ˮ���ٺ���//////////////////////////////////////////////////////////////////////////////////////////////////
void pump_control(u16 input)
{
    PUMP=1;
    delay_ms(input);
    PUMP=0;
}

//��������������1������0�ر�//////////////////////////////////////////////////////////////////////////////////////////////////
void spray_control(u16 input)
{
    SPRAY=input;
}

//����Ʋ���������1������0�ر�//////////////////////////////////////////////////////////////////////////////////////////////////
void light_control(u16 input)
{
    LIGHT=input;
}

//�¶ȴ�������������
/*******************************************************************************
* �� �� ��         : ds18b20_reset
* ��������		   : ��λDS18B20  
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ds18b20_reset(void)
{
	TEMPATURE=0;	//����DQ
	delay_10us(75);	//����750us
	TEMPATURE=1;	//DQ=1
	delay_10us(2);	//20US
}

/*******************************************************************************
* �� �� ��         : ds18b20_check
* ��������		   : ���DS18B20�Ƿ����
* ��    ��         : ��
* ��    ��         : 1:δ��⵽DS18B20�Ĵ��ڣ�0:����
*******************************************************************************/
u8 ds18b20_check(void)
{
	u8 time_temp=0;

	while(TEMPATURE&&time_temp<20)	//�ȴ�DQΪ�͵�ƽ
	{
		time_temp++;
		delay_10us(1);	
	}
	if(time_temp>=20)return 1;	//�����ʱ��ǿ�Ʒ���1
	else time_temp=0;
	while((!TEMPATURE)&&time_temp<20)	//�ȴ�DQΪ�ߵ�ƽ
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)return 1;	//�����ʱ��ǿ�Ʒ���1
	return 0;
}

/*******************************************************************************
* �� �� ��         : ds18b20_read_bit
* ��������		   : ��DS18B20��ȡһ��λ
* ��    ��         : ��
* ��    ��         : 1/0
*******************************************************************************/
u8 ds18b20_read_bit(void)
{
	u8 dat=0;
	
	TEMPATURE=0;
	_nop_();_nop_();
	TEMPATURE=1;	
	_nop_();_nop_(); //�ö�ʱ�䲻�ܹ�����������15us�ڶ�ȡ����
	if(TEMPATURE)dat=1;	//���������Ϊ1������datΪ1������Ϊ0
	else dat=0;
	delay_10us(5);
	return dat;
} 

/*******************************************************************************
* �� �� ��         : ds18b20_read_byte
* ��������		   : ��DS18B20��ȡһ���ֽ�
* ��    ��         : ��
* ��    ��         : һ���ֽ�����
*******************************************************************************/
u8 ds18b20_read_byte(void)
{
	u8 i=0;
	u8 dat=0;
	u8 temp=0;

	for(i=0;i<8;i++)//ѭ��8�Σ�ÿ�ζ�ȡһλ�����ȶ���λ�ٶ���λ
	{
		temp=ds18b20_read_bit();
		dat=(temp<<7)|(dat>>1);
	}
	return dat;	
}

/*******************************************************************************
* �� �� ��         : ds18b20_write_byte
* ��������		   : дһ���ֽڵ�DS18B20
* ��    ��         : dat��Ҫд����ֽ�
* ��    ��         : ��
*******************************************************************************/
void ds18b20_write_byte(u8 dat)
{
	u8 i=0;
	u8 temp=0;

	for(i=0;i<8;i++)//ѭ��8�Σ�ÿ��дһλ������д��λ��д��λ
	{
		temp=dat&0x01;//ѡ���λ׼��д��
		dat>>=1;//���θ�λ�Ƶ���λ
		if(temp)
		{
			TEMPATURE=0;
			_nop_();_nop_();
			TEMPATURE=1;	
			delay_10us(6);
		}
		else
		{
			TEMPATURE=0;
			delay_10us(6);
			TEMPATURE=1;
			_nop_();_nop_();	
		}	
	}	
}

/*******************************************************************************
* �� �� ��         : ds18b20_start
* ��������		   : ��ʼ�¶�ת��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ds18b20_start(void)
{
	ds18b20_reset();//��λ
	ds18b20_check();//���DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//ת������	
}

/*******************************************************************************
* �� �� ��         : ds18b20_init
* ��������		   : ��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
* ��    ��         : ��
* ��    ��         : 1:�����ڣ�0:����
*******************************************************************************//////////////////////////////////////////////////
u8 ds18b20_init(void)
{
	ds18b20_reset();
	return ds18b20_check();	
}

/*******************************************************************************
* �� �� ��         : ds18b20_read_temperture
* ��������		   : ��ds18b20�õ��¶�ֵ
* ��    ��         : ��
* ��    ��         : �¶�����
*******************************************************************************//////////////////////////////////////////////////
float temperature_read(void)
{
	float temp;
	u8 dath=0;
	u8 datl=0;
	u16 value=0;

	ds18b20_start();//��ʼת��
	ds18b20_reset();//��λ
	ds18b20_check();
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0xbe);//���洢��

	datl=ds18b20_read_byte();//���ֽ�
	dath=ds18b20_read_byte();//���ֽ�
	value=(dath<<8)+datl;//�ϲ�Ϊ16λ����

	if((value&0xf800)==0xf800)//�жϷ���λ�����¶�
	{
		value=(~value)+1; //����ȡ���ټ�1
		temp=value*(-0.0625);//���Ծ���	
	}
	else //���¶�
	{
		temp=value*0.0625;	
	}
	return temp;
}

//����ʪ�ȴ�������������
//��λDHT11
void DHT11_Rst(void)	   
{                 
	MOISTURE_AIR=1;
	delay_10us(1);
	MOISTURE_AIR=0; 	//����DQ
    delay_ms(25);   //��������18ms
    MOISTURE_AIR=1; 	//DQ=1 
	delay_10us(3);  //��������20~40us
}
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	
	while (!MOISTURE_AIR&&retry<100)//�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
	{
		retry++;
		_nop_();
	};
	if(retry>=100)return 1;
	else retry=0;
    while (MOISTURE_AIR&&retry<100)//�жϴӻ����� 80us �ĸߵ�ƽ�Ƿ����?�����?�������������ݽ���״̬
	{
		retry++;
		_nop_();
	};	 
	if(retry>=100)return 1;	    
	return 0;
}


//DHT11��ʼ�� //////////////////////////////////////////////////////////////////////////////////////////////////
//����0����ʼ���ɹ���1��ʧ��
u8 DHT11_Init(void)
{
	MOISTURE_AIR=1;
	DHT11_Rst();	  
	return DHT11_Check();	
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
u8 DHT11_Read_Byte(void)    
{        
    u8 i,temp;
	u8 data_byte=0; 
	u8 retry=0;

  	for(i=0;i<8;i++)//����8bit������ 
  	{ 
//		while(!MOISTURE_AIR);//�ȴ�50us�ĵ͵�ƽ��ʼ�źŽ���
		while (!MOISTURE_AIR&&retry<50)//�ȴ�50us�ĵ͵�ƽ��ʼ�źŽ���
		{
			retry++;
			_nop_();
		};
		retry=0; 
		delay_10us(3);//�ȴ�40us 
		temp=0;//ʱ��Ϊ26us-28us?���ʾ���յ��?����'0' 
		if(MOISTURE_AIR==1) 
			temp=1; //���26us-28us֮��?����?�ߵ�ƽ?����ʾ���յ��?��Ϊ'1' 
//		while(MOISTURE_AIR);//�ȴ������źŸߵ�ƽ??'0'Ϊ26us-28us??'1'Ϊ70us
		while (MOISTURE_AIR&&retry<100)//�ȴ������źŸߵ�ƽ??'0'Ϊ26us-28us??'1'Ϊ70us
		{
			retry++;
			_nop_();
		};
		data_byte<<=1;//���յ�����Ϊ��λ��ǰ?����ƪ? 
		data_byte|=temp; 
  	} 

  	return data_byte;
}

//��DHT11��ȡһ������//////////////////////////////////////////////////////////////////////////////////////////////////
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 air_moisture_read(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		
	}else return 1;
	return 0;	    
}














































u8 keytmp=0;
u8 key=0;
//�¶ȴ�����
u16 stay_time;
int temp_value;
u8 tempbuf[8];

//����ʪ�ȴ�����
u8 temp=0;
u8 humi=0;
u8 i=0;
u8 temp_buf2[3],humi_buf[3];
u8 gsmg_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
				0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

//��ǿ������
u16 adc_light_value=0;
u8 adc_light_buf[4];
				
//
u16 adc_water_level_value=0;
u8 adc_water_level_buf[4];
				
				
void main()
{
	lcd1602_init();
	lcd1602_show_string(0,0,"Self-testing");
	stay_time=0;
	ds18b20_init();
	DHT11_Init();
	spray_control(0);
	light_control(0);
	pump_control(500);
	lcd1602_init();
	lcd1602_show_string(0,0,"Self-test");
	lcd1602_show_string(0,1,"Success!!!");
	while(1)
	{
				
		stay_time++;
		 if(stay_time==22000){
			 
		 	temp_value=temperature_read()*10;//DS18B20
		 	if(temp_value<0)
			{
				temp_value=-temp_value;
				tempbuf[0]='-';	
			}
			else
				tempbuf[0]=' ';
			if(temp_value>=1000)
				tempbuf[1]=temp_value/1000+0x30;
			else
				tempbuf[1]=' ';
			tempbuf[2]=temp_value%1000/100+0x30;
			tempbuf[3]=temp_value%1000%100/10+0x30;
			tempbuf[4]='.';
			tempbuf[5]=temp_value%1000%100%10+0x30;
			tempbuf[6]='C';
			tempbuf[7]='\0';	
					
			adc_light_value=xpt2046_read_adc_value(0xA4);//��������
			adc_light_buf[0]=adc_light_value%1000/100+0x30;
			adc_light_buf[1]=adc_light_value%1000%100/10+0x30;
			adc_light_buf[2]=adc_light_value%1000%100%10+0x30;
			adc_light_buf[3]='\0';
				
			adc_water_level_value=xpt2046_read_adc_value(0xE4);//ˮλ������
			adc_water_level_buf[0]=adc_water_level_value%1000/100+0x30;
			adc_water_level_buf[1]=adc_water_level_value%1000%100/10+0x30;
			adc_water_level_buf[2]=adc_water_level_value%1000%100%10+0x30;
			adc_water_level_buf[3]='\0';			
		 
		 	air_moisture_read(&temp,&humi);//DHT11
		 	temp_buf2[0]=temp/10+0x30;	
		 	temp_buf2[1]=temp%10+0x30;
		 	temp_buf2[2]='\0';

		 	humi_buf[0]=humi/10+0x30;	
		 	humi_buf[1]=humi%10+0x30;
		 	humi_buf[2]='\0';
			
			stay_time=0;
		}
		 


		
		// keytmp=key_matrix_flip_scan();
		// if((key==1||key==2||key==3||key==4||key==5)&&keytmp==0)
		// { }
		// else
		// 	key=keytmp;
		key=key_matrix_flip_scan();
		switch(key)
		{
			case(1):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Temperature:");//��һ����ʾ
				lcd1602_show_string(0,1,tempbuf);//�ڶ�����ʾ
				break;	
			}
			case(2):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Soil Moisture:");//��һ����ʾ
				lcd1602_show_string(0,1,"N/A");//�ڶ�����ʾ
				break;	
			}
			case(3):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Water Level:");//��һ����ʾ
				lcd1602_show_string(0,1,adc_water_level_buf);//�ڶ�����ʾ
				break;	
			}
			case(4):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Air Moisture:");//��һ����ʾ
				lcd1602_show_string(0,1,humi_buf);//�ڶ�����ʾ
				break;	
			}
			case(5):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Light Intensity:");//��һ����ʾ
				lcd1602_show_string(0,1,adc_light_buf);//�ڶ�����ʾ
				break;	
			}
			case(6):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Rising");
				step_motor_control(1);
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Rised");
				break;	
			}
			case(7):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Falling");
				step_motor_control(0);
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Falled");
				break;	
			}
			case(8):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Pump Working");
				pump_control(5000);
				lcd1602_show_string(0,0,"Pump Worked 5s");
				break;	
			}
			case(9):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Spray Work Begin");
				spray_control(1);
				break;	
			}
			case(10):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Spray Work Stop");
				spray_control(0);
				break;	
			}
			case(11):{
				lcd1602_init();
				lcd1602_show_string(0,0,"LED Work Begin");
				light_control(1);
				break;	
			}
			case(12):{
				lcd1602_init();
				lcd1602_show_string(0,0,"LED Work Stop");
				light_control(0);
				break;	
			}
			case(13):{
				lcd1602_init();
				lcd1602_show_string(0,0,"Self-testing");
				stay_time=0;
				ds18b20_init();
				DHT11_Init();
				spray_control(0);
				light_control(0);
				pump_control(500);
				lcd1602_init();
				lcd1602_show_string(0,0,"Self-test");
				lcd1602_show_string(0,1,"Success!!!");
				break;	
			}
			case(14):{
				break;	
			}
			case(15):{
				break;	
			}
			case(16):{
				break;	
			}
			default:{
				break;
			}
		}
	}
}
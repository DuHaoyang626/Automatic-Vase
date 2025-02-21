#include"reg52.h"
#include "intrins.h"


//????
typedef unsigned int u16;
typedef unsigned char u8;


//????

#define LCD1602_DATAPORT P0//LCD????P0^0-P0^3
sbit cs = P0^4;//????,?????
sbit clk = P0^5;//??????
sbit dio = P0^6;//????DI???DO
sbit WIFI_IO=P0^7;//??WiFi???P0^7

#define KEY_MATRIX_PORT	P1//???????

sbit PUMP=P2^0;//????P2^0
sbit SPRAY=P2^1;//?????2^1
sbit LIGHT=P2^2;//LED?????P2^2
sbit MOISTURE_AIR=P2^3;//?????????P2^3
sbit TEMPATURE=P2^4;//???????P2^4
sbit LCD1602_RW=P2^5;//LCD????
sbit LCD1602_RS=P2^6;//LCD??????
sbit LCD1602_E=P2^7; //LCD????

sbit STEPMOTOR1=P3^0;//????1??P3^0
sbit STEPMOTOR2=P3^1;//????2??P3^1
sbit STEPMOTOR3=P3^2;//????3??P3^2
sbit STEPMOTOR4=P3^3;//????4??P3^3
sbit DIN  = P3^4;	  //??ADC??
sbit CS   = P3^5;	  //??ADC??
sbit CLK  = P3^6;	  //??ADC??
sbit DOUT = P3^7;	  //??ADC??

//????

//LCD1602???4??8???,??1,??LCD1602???????,???8?
#define LCD1602_4OR8_DATA_INTERFACE	1	//????4????LCD1602

// ????????,???,????
#define STEPMOTOR_MAXSPEED        100//????????,????1
#define STEPMOTOR_MINSPEED        350//????????
#define STEPMOTOR_SHIELD_HIGHT    10//?????
#define STEPMOTOR_SHIELD_RANGE    300//?????????

//????
//????????
u16 stay_time;

//???????
u16 temp_value;
u8 temp_buf[8];

//?????????
u16 soil_moisture;
u8 soil_moisture_buf[4];

//???????
u16 water_lavel;
u8 water_lavel_buf[4];

//?????????
u8 temp=0;
u8 humi=0;
u8 i=0;
u8 temp_buf2[3],humi_buf[3];

//???????
u16 light_intensity=0;
u8 light_intensity_buf[4];
				
//???????????,0?????,1?????
u16 StepMotor_status=0;

//??????
u8 key=0;


//????

//??2us
void Delay_2us(void)
{
	_nop_();
	_nop_();
}

//????,??int x,??x/100??
void delay_10us(u16 ten_us){
while(ten_us--);
}

//????,??int x,??x??
void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

//????????

//??ADC2046??
/*******************************************************************************
* ? ? ?       : xpt2046_wirte_data
* ????		 : XPT2046???
* ?    ?       : dat:?????
* ?    ?    	 : ?
*******************************************************************************/
void xpt2046_wirte_data(u8 dat)
{
	u8 i;

	CLK = 0;
	_nop_();
	for(i=0;i<8;i++)//??8?,??????,?????
	{
		DIN = dat >> 7;//????????
		dat <<= 1;//???????
		CLK = 0;//CLK???????????,??????
		_nop_();	
		CLK = 1;
		_nop_();
	}
}

/*******************************************************************************
* ? ? ?       : xpt2046_read_data
* ????		 : XPT2046???
* ?    ?       : ?
* ?    ?    	 : XPT2046??12???
*******************************************************************************/
u16	xpt2046_read_data(void)
{
	u8 i;
	u16 dat=0;

	CLK = 0;
	_nop_();
	for(i=0;i<12;i++)//??12?,??????,???????,????????u16
	{
		dat <<= 1;
		CLK = 1;
		_nop_();
		CLK = 0; //CLK???????????,??????
		_nop_();
		dat |= DOUT;//?????,??????	
	}
	return dat;	
}

/*******************************************************************************
* ? ? ?       : xpt2046_read_adc_value
* ????		 : XPT2046?AD??
* ?    ?       : cmd:??
* ?    ?    	 : XPT2046??AD?
*******************************************************************************//////////////////////////////////////////////////
u16 xpt2046_read_adc_value(u8 cmd)
{
	u8 i;
	u16 adc_value=0;

	CLK = 0;//?????
	CS  = 0;//??XPT2046
	xpt2046_wirte_data(cmd);//?????
	for(i=6; i>0; i--);//????????
	CLK = 1;
	_nop_();
	CLK = 0;//??????,??BUSY
	_nop_();
	adc_value=xpt2046_read_data();
	CS = 1;//??XPT2046
	return adc_value;
}

//??ADC0832
/*****************************************
????:??ADC0832??
????:ADC_read_data(bit channel)
????:ch?????,ch=0????0,ch=1????1
????:?????????ADC??,???unsigned char
		  ?????0?,??????
******************************************/
u8 ADC_read_data(bit channel)
{
	u8 i,dat0=0,dat1=0;
  //------?1??????di??,????---------

	cs=0;			//??????,??AD????
	clk=0;			//?????
	
	dio=1;  		//????????
	Delay_2us();
	clk=1;			//???????,??????,??????(DI=1)
	Delay_2us();
  	clk=0;			//?1??????
	dio=1;
	Delay_2us();
	
	clk=1;		    // ?2??????DI=1,??????????
	Delay_2us();	 
  //------??2????,??????????(1:????,0:??????)---------		
	clk=0;	
	dio=channel;         // ?3????,??DI,????;
	Delay_2us();
	clk=1;
	Delay_2us();

   //------??3????,??????????(1:??CH1,0:??CH0)------------	
	
	clk=0;
	dio=1;          //????????,DI??,????? 
	Delay_2us();	
	for(i=0;i<8;i++)                 //?4~11?8???????(MSB->LSB)
	{
		clk=1;
		Delay_2us();
		clk=0;
		Delay_2us();
		dat0=dat0<<1|dio;
	}
	for(i=0;i<8;i++)                 //?11~18?8???????(LSB->MSB)
	{
		dat1=dat1|((u8)(dio)<<i);
		clk=1;
		Delay_2us();
		clk=0;
		Delay_2us();
	} 
	cs=1;				  
	return (dat0==dat1)?dat0:0;	    //??dat0?dat1????
}

//LCD????
/*******************************************************************************
* ? ? ?       : lcd1602_write_cmd
* ????		 : LCD1602???
* ?    ?       : cmd:??
* ?    ?    	 : ?
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8?LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//????
	LCD1602_RW=0;//???
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????	
}
#else	//4?LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//????
	LCD1602_RW=0;//???
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????
	
	LCD1602_DATAPORT=cmd<<4;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????	
}
#endif

/*******************************************************************************
* ? ? ?       : lcd1602_write_data
* ????		 : LCD1602???
* ?    ?       : dat:??
* ?    ?    	 : ?
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8?LCD
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//????
	LCD1602_RW=0;//???
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????		
}
#else
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//????
	LCD1602_RW=0;//???
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????
	
	LCD1602_DATAPORT=dat<<4;//????
	delay_ms(1);
	LCD1602_E=1;//???E??????
	delay_ms(1);
	LCD1602_E=0;//???E????????		
}
#endif

/*******************************************************************************
* ? ? ?       : lcd1602_init
* ????		 : LCD1602???
* ?    ?       : ?
* ?    ?    	 : ?
*******************************************************************************//////////////////////////////////////////////////
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8?LCD
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);//????8?,??2?,5*7??/??
	lcd1602_write_cmd(0x0c);//?????,???,????
	lcd1602_write_cmd(0x06);//??????????,??????
	lcd1602_write_cmd(0x01);//??	
}
#else
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x28);//????4?,??2?,5*7??/??
	lcd1602_write_cmd(0x0c);//?????,???,????
	lcd1602_write_cmd(0x06);//??????????,??????
	lcd1602_write_cmd(0x01);//??	
}
#endif

/*******************************************************************************
* ? ? ?       : lcd1602_clear
* ????		 : LCD1602??
* ?    ?       : ?
* ?    ?    	 : ?
*******************************************************************************/
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

/*******************************************************************************
* ? ? ?       : lcd1602_show_string
* ????		 : LCD1602????
* ?    ?       : x,y:????,x=0~15,y=0~1;
				   str:?????
* ?    ?    	 : ?
*******************************************************************************//////////////////////////////////////////////////
void lcd1602_show_string(u8 x,u8 y,u8 *str)
{
	u8 i=0;

	if(y>1||x>15)return;//???????????

	if(y<1)	//?1???
	{	
		while(*str!='\0')//?????'\0'??,??????????
		{
			if(i<16-x)//???????????????,?????????
			{
				lcd1602_write_cmd(0x80+i+x);//?????????	
			}
			else
			{
				lcd1602_write_cmd(0x40+0x80+i+x-16);//?????????	
			}
			lcd1602_write_data(*str);//????
			str++;//????
			i++;	
		}	
	}
	else	//?2???
	{
		while(*str!='\0')
		{
			if(i<16-x) //???????????????,?????????
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

//??????
/*******************************************************************************
* ? ? ?       : key_matrix_flip_scan
* ????		 : ?????????,??????????,?????????
* ?    ?       : ?
* ?    ?    	 : key_value:1-16,??S1-S16?,
				   0:?????
*******************************************************************************//////////////////////////////////////////////////
u8 key_matrix_flip_scan(void)
{
	static u8 key_value=0;

	KEY_MATRIX_PORT=0x0f;//??????0,???1
	if(KEY_MATRIX_PORT!=0x0f)//????????
	{
		delay_10us(1000);//??
		if(KEY_MATRIX_PORT!=0x0f)
		{
			//???
			KEY_MATRIX_PORT=0x0f;
			switch(KEY_MATRIX_PORT)//????0,????????	
			{
				case 0x07: key_value=1;break;
				case 0x0b: key_value=2;break;
				case 0x0d: key_value=3;break;
				case 0x0e: key_value=4;break;
			}
			//???
			KEY_MATRIX_PORT=0xf0;
			switch(KEY_MATRIX_PORT)//????0,????????	
			{
				case 0x70: key_value=key_value;break;
				case 0xb0: key_value=key_value+4;break;
				case 0xd0: key_value=key_value+8;break;
				case 0xe0: key_value=key_value+12;break;
			}
			while(KEY_MATRIX_PORT!=0xf0);//??????	
		}
	}
	else
		key_value=0;		
	
	return key_value;		
}

//????????
/*******************************************************************************
* ? ? ?       : step_motor_28BYJ48_send_pulse
* ????		 : ???????ULN2003???????????????
* ?    ?       : step:??????,???0~7
				   dir:????,1:???,0:???
* ?    ?    	 : ?
*******************************************************************************/
void step_motor_28BYJ48_send_pulse(u8 step,u8 dir)
{
	u8 temp=step;
	
	if(dir==0)	//????????
		temp=7-step;//??????
	switch(temp)//8?????:A->AB->B->BC->C->CD->D->DA
	{
		case 0: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 1: STEPMOTOR1=1;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 2: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 3: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 4: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 5: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=1;break;
		case 6: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		case 7: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		default: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;//????	
	}			
}

// //????,1????,0????//////////////////////////////////////////////////////////////////////////////////////////////////
// void step_motor_control(u16 input){
//     if(input!=StepMotor_status)
//     {
//         u8 key=0;
// 	    u8 dir=0;//???????
// 	    u8 speed=STEPMOTOR_MAXSPEED;//????????
// 	    u8 step=0;
//         u16 time=0;
//         u16 usetime=STEPMOTOR_SHIELD_HIGHT*STEPMOTOR_SHIELD_RANGE;

//         if(input==1)
//         {
//             while(time<=usetime){
//                 step_motor_28BYJ48_send_pulse(step++,dir);
// 								time++;
// 								if(step==8)step=0;		
// 								delay_10us(speed);
//             }
// 			StepMotor_status=1;
//         }
//         if(input==0)
//         {
//             dir=!dir;
//             while(time<=usetime){
//                 step_motor_28BYJ48_send_pulse(step++,dir);
// 								time++;
// 								if(step==8)step=0;		
// 								delay_10us(speed);
//             }
// 			StepMotor_status=0;
//         }
//     }
// }

//?????????
/*******************************************************************************
* ? ? ?         : ds18b20_reset
* ????		   : ??DS18B20  
* ?    ?         : ?
* ?    ?         : ?
*******************************************************************************/
void ds18b20_reset(void)
{
	TEMPATURE=0;	//??DQ
	delay_10us(75);	//??750us
	TEMPATURE=1;	//DQ=1
	delay_10us(2);	//20US
}

/*******************************************************************************
* ? ? ?         : ds18b20_check
* ????		   : ??DS18B20????
* ?    ?         : ?
* ?    ?         : 1:????DS18B20???,0:??
*******************************************************************************/
u8 ds18b20_check(void)
{
	u8 time_temp=0;

	while(TEMPATURE&&time_temp<20)	//??DQ????
	{
		time_temp++;
		delay_10us(1);	
	}
	if(time_temp>=20)return 1;	//?????????1
	else time_temp=0;
	while((!TEMPATURE)&&time_temp<20)	//??DQ????
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)return 1;	//?????????1
	return 0;
}

/*******************************************************************************
* ? ? ?         : ds18b20_read_bit
* ????		   : ?DS18B20?????
* ?    ?         : ?
* ?    ?         : 1/0
*******************************************************************************/
u8 ds18b20_read_bit(void)
{
	u8 dat=0;
	
	TEMPATURE=0;
	_nop_();_nop_();
	TEMPATURE=1;	
	_nop_();_nop_(); //????????,???15us?????
	if(TEMPATURE)dat=1;	//??????1???dat?1,???0
	else dat=0;
	delay_10us(5);
	return dat;
} 

/*******************************************************************************
* ? ? ?         : ds18b20_read_byte
* ????		   : ?DS18B20??????
* ?    ?         : ?
* ?    ?         : ??????
*******************************************************************************/
u8 ds18b20_read_byte(void)
{
	u8 i=0;
	u8 dat=0;
	u8 temp=0;

	for(i=0;i<8;i++)//??8?,??????,?????????
	{
		temp=ds18b20_read_bit();
		dat=(temp<<7)|(dat>>1);
	}
	return dat;	
}

/*******************************************************************************
* ? ? ?         : ds18b20_write_byte
* ????		   : ??????DS18B20
* ?    ?         : dat:??????
* ?    ?         : ?
*******************************************************************************/
void ds18b20_write_byte(u8 dat)
{
	u8 i=0;
	u8 temp=0;

	for(i=0;i<8;i++)//??8?,?????,?????????
	{
		temp=dat&0x01;//????????
		dat>>=1;//????????
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
* ? ? ?         : ds18b20_start
* ????		   : ??????
* ?    ?         : ?
* ?    ?         : ?
*******************************************************************************/
void ds18b20_start(void)
{
	ds18b20_reset();//??
	ds18b20_check();//??DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//????	
}

/*******************************************************************************
* ? ? ?         : ds18b20_init
* ????		   : ???DS18B20?IO? DQ ????DS???
* ?    ?         : ?
* ?    ?         : 1:???,0:??
*******************************************************************************//////////////////////////////////////////////////
u8 ds18b20_init(void)
{
	ds18b20_reset();
	return ds18b20_check();	
}

/*******************************************************************************
* ? ? ?         : ds18b20_read_temperture
* ????		   : ?ds18b20?????
* ?    ?         : ?
* ?    ?         : ????
*******************************************************************************//////////////////////////////////////////////////
// float temperature_read(void)
// {
// 	float temp;
// 	u8 dath=0;
// 	u8 datl=0;
// 	u16 value=0;

// 	ds18b20_start();//????
// 	ds18b20_reset();//??
// 	ds18b20_check();
// 	ds18b20_write_byte(0xcc);//SKIP ROM
//     ds18b20_write_byte(0xbe);//????

// 	datl=ds18b20_read_byte();//???
// 	dath=ds18b20_read_byte();//???
// 	value=(dath<<8)+datl;//???16???

// 	if((value&0xf800)==0xf800)//?????,???
// 	{
// 		value=(~value)+1; //??????1
// 		temp=value*(-0.0625);//????	
// 	}
// 	else //???
// 	{
// 		temp=value*0.0625;	
// 	}
// 	return temp;
// }

//???????????
//??DHT11
void DHT11_Rst(void)	   
{                 
	MOISTURE_AIR=1;
	delay_10us(1);
	MOISTURE_AIR=0; 	//??DQ
    delay_ms(25);   //????18ms
    MOISTURE_AIR=1; 	//DQ=1 
	delay_10us(3);  //????20~40us
}
//??DHT11???
//??1:????DHT11???
//??0:??
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	
	while (!MOISTURE_AIR&&retry<100)//?????? 80us ????????????
	{
		retry++;
		_nop_();
	};
	if(retry>=100)return 1;
	else retry=0;
    while (MOISTURE_AIR&&retry<100)//?????? 80us ?????????H??????????????
	{
		retry++;
		_nop_();
	};	 
	if(retry>=100)return 1;	    
	return 0;
}


//DHT11??? //////////////////////////////////////////////////////////////////////////////////////////////////
//??0:?????,1:??
u8 DHT11_Init(void)
{
	MOISTURE_AIR=1;
	DHT11_Rst();	  
	return DHT11_Check();	
}

//?DHT11??????
//???:?????
u8 DHT11_Read_Byte(void)    
{        
    u8 i,temp;
	u8 data_byte=0; 
	u8 retry=0;

  	for(i=0;i<8;i++)//??8bit??? 
  	{ 
//		while(!MOISTURE_AIR);//??50us??????????
		while (!MOISTURE_AIR&&retry<50)//??50us??????????
		{
			retry++;
			_nop_();
		};
		retry=0; 
		delay_10us(3);//??40us 
		temp=0;//???26us-28us?1????????'0' 
		if(MOISTURE_AIR==1) 
			temp=1; //??26us-28us???;??????T?????????'1' 
//		while(MOISTURE_AIR);//???????????'0'?26us-28us??'1'?70us
		while (MOISTURE_AIR&&retry<100)//???????????'0'?26us-28us??'1'?70us
		{
			retry++;
			_nop_();
		};
		data_byte<<=1;//??????????????? 
		data_byte|=temp; 
  	} 

  	return data_byte;
}

// //?DHT11??????//////////////////////////////////////////////////////////////////////////////////////////////////
// //temp:???(??:0~50°)
// //humi:???(??:20%~90%)
// //???:0,??;1,????
// u8 air_moisture_read(u8 *temp,u8 *humi)    
// {        
//  	u8 buf[5];
// 	u8 i;
// 	DHT11_Rst();
// 	if(DHT11_Check()==0)
// 	{
// 		for(i=0;i<5;i++)//??40???
// 		{
// 			buf[i]=DHT11_Read_Byte();
// 		}
// 		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
// 		{
// 			*humi=buf[0];
// 			*temp=buf[2];
// 		}
		
// 	}else return 1;
// 	return 0;	    
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//????
float temperature_read(void)
{
	float temp;
	u8 dath=0;
	u8 datl=0;
	u16 value=0;

	ds18b20_start();//????
	ds18b20_reset();//??
	ds18b20_check();
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0xbe);//????

	datl=ds18b20_read_byte();//???
	dath=ds18b20_read_byte();//???
	value=(dath<<8)+datl;//???16???

	if((value&0xf800)==0xf800)//?????,???
	{
		value=(~value)+1; //??????1
		temp=value*(-0.0625);//????	
	}
	else //???
	{
		temp=value*0.0625;	
	}
	return temp;
}

//?????????
u8 soil_moisture_read(void)
{
	u8 soil_moisture_adc;
	soil_moisture_adc=ADC_read_data(0);
	return soil_moisture_adc;
}
//???????
u8 water_lavel_read(void)
{
	u8 water_lavel_adc;
	water_lavel_adc=ADC_read_data(1);
	return water_lavel_adc;
}

//?DHT11??????
//temp:???
//humi:???
u8 air_moisture_read(void)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//??40???
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			humi=buf[0];
			temp=buf[2];
		}
		
	}   
}

//??????
u8 light_intensity_read(void)
{
	u8 light_value_adc;
	light_value_adc=xpt2046_read_adc_value(0xE4);
	return light_value_adc;
}

//???????,1????,0????
void step_motor_control(u16 input){
    if(input!=StepMotor_status)
    {
        u8 key=0;
	    u8 dir=0;//???????
	    u8 speed=STEPMOTOR_MAXSPEED;//????????
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

//??????,????????
void pump_control(u16 input)
{
    PUMP=1;
    delay_ms(input);
    PUMP=0;
}

//???????,1??,0??
void spray_control(u16 input)
{
    SPRAY=input;
}

//???????,1??,0??
void light_control(u16 input)
{
    LIGHT=input;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void all_data_read(void)
{
	//??
	temp_value=temperature_read()*10;

	if(temp_value<0)
	{
		temp_value=-temp_value;
		temp_buf[0]='-';	
	}
	else
		temp_buf[0]=' ';
	if(temp_value>=1000)
		temp_buf[1]=temp_value/1000+0x30;
	else
		temp_buf[1]=' ';
	temp_buf[2]=temp_value%1000/100+0x30;
	temp_buf[3]=temp_value%1000%100/10+0x30;
	temp_buf[4]='.';
	temp_buf[5]=temp_value%1000%100%10+0x30;
	temp_buf[6]='C';
	temp_buf[7]='\0';	

	//????
	soil_moisture=soil_moisture_read();

	soil_moisture_buf[0]=soil_moisture%1000/100+0x30;
	soil_moisture_buf[1]=soil_moisture%1000%100/10+0x30;
	soil_moisture_buf[2]=soil_moisture%1000%100%10+0x30;
	soil_moisture_buf[3]='\0';

	//????
	soil_moisture=soil_moisture_read();

	soil_moisture_buf[0]=soil_moisture%1000/100+0x30;
	soil_moisture_buf[1]=soil_moisture%1000%100/10+0x30;
	soil_moisture_buf[2]=soil_moisture%1000%100%10+0x30;
	soil_moisture_buf[3]='\0';

	//????
	air_moisture_read();
	temp_buf2[0]=temp/10+0x30;	
	temp_buf2[1]=temp%10+0x30;
 	temp_buf2[2]='\0';

	humi_buf[0]=humi/10+0x30;	
	humi_buf[1]=humi%10+0x30;
 	humi_buf[2]='\0';

	//????
	light_intensity=light_intensity_read();
	light_intensity_buf[0]=light_intensity%1000/100+0x30;
	light_intensity_buf[1]=light_intensity%1000%100/10+0x30;
	light_intensity_buf[2]=light_intensity%1000%100%10+0x30;
	light_intensity_buf[3]='\0';
}

void all_init(void)
{
	lcd1602_init();
	lcd1602_show_string(0,0,"Self-testing");
	stay_time=0;
	ds18b20_init();
	DHT11_Init();
	pump_control(500);
	spray_control(0);
	light_control(0);
	lcd1602_init();
	lcd1602_show_string(0,0,"Self-test");
	lcd1602_show_string(0,1,"Success!!!");
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//??????
void test(void)
{
	key=key_matrix_flip_scan();
	switch(key)
	{
		case(1):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Temperature:");//?????
			lcd1602_show_string(0,1,temp_buf);//?????
			break;	
		}
		case(2):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Soil Moisture:");//?????
			lcd1602_show_string(0,1,soil_moisture_buf);//?????
			break;	
		}
		case(3):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Water Level:");//?????
			lcd1602_show_string(0,1,water_lavel_buf);//?????
			break;	
		}
		case(4):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Air Moisture:");//?????
			lcd1602_show_string(0,1,humi_buf);//?????
			break;	
		}
		case(5):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Light Intensity:");//?????
			lcd1602_show_string(0,1,light_intensity_buf);//?????
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
			all_init();
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








int main()
{
	all_init();
	while(1)
	{
		stay_time++;
		if(stay_time==22000)
		{
			all_data_read();
			stay_time=0;
		}
		test();
	}
}
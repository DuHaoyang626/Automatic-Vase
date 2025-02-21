#include"reg52.h"
#include "intrins.h"


//类型定义
typedef unsigned int u16;
typedef unsigned char u8;


//接口定义

#define LCD1602_DATAPORT P0//LCD数据端口P0^0-P0^3
sbit cs = P0^4;//片选使能，低电平有效
sbit clk = P0^5;//芯片时钟输入
sbit dio = P0^6;//数据输入DI与输出DO
sbit WIFI_IO=P0^7;//预留WiFi接口为P0^7

#define KEY_MATRIX_PORT	P1//开发板矩阵键盘

sbit PUMP=P2^0;//水泵接入P2^0
sbit SPRAY=P2^1;//雾化器接入2^1
sbit LIGHT=P2^2;//LED补光灯接入P2^2
sbit MOISTURE_AIR=P2^3;//空气湿度传感器接入P2^3
sbit TEMPATURE=P2^4;//温度传感器接入P2^4
sbit LCD1602_RW=P2^5;//LCD读写选择
sbit LCD1602_RS=P2^6;//LCD数据命令选择
sbit LCD1602_E=P2^7; //LCD使能信号

sbit STEPMOTOR1=P3^0;//步进电机1接入P3^0
sbit STEPMOTOR2=P3^1;//步进电机2接入P3^1
sbit STEPMOTOR3=P3^2;//步进电机3接入P3^2
sbit STEPMOTOR4=P3^3;//步进电机4接入P3^3
sbit DIN  = P3^4;	  //板载ADC输入
sbit CS   = P3^5;	  //板载ADC片选
sbit CLK  = P3^6;	  //板载ADC时钟
sbit DOUT = P3^7;	  //板载ADC输出

//全局数据

//LCD1602数据口4位和8位定义，若为1，则为LCD1602四位数据口驱动，反之为8位
#define LCD1602_4OR8_DATA_INTERFACE	1	//默认使用4位数据口LCD1602

// 定义步进电机速度，值越小，速度越快
#define STEPMOTOR_MAXSPEED        1//步进电机最大速度，不能小于1
#define STEPMOTOR_MINSPEED        5//步进电机最小速度
#define STEPMOTOR_SHIELD_HIGHT    10//遮光板高度
#define STEPMOTOR_SHIELD_RANGE    750//遮光板移动时间比例

//全局变量
//探测间隔时间变量
u16 stay_time;

//温度传感器变量
u16 temp_value;
u8 temp_buf[8];

//土壤湿度传感器变量
u16 soil_moisture;
u8 soil_moisture_buf[4];

//水位传感器变量
u16 water_lavel;
u8 water_lavel_buf[4];

//空气湿度传感器变量
u8 temp=0;
u8 humi=0;
u8 i=0;
u8 temp_buf2[3],humi_buf[3];

//光强传感器变量
u16 light_intensity=0;
u8 light_intensity_buf[4];
				
//步进电机遮光板状态变量，0为最低位置，1为最高位置
u16 StepMotor_status=0;

//按键检测变量
u8 key=0;

//test2允许变量
u16 allow_test2=0;

//pump允许变量
u16 allow_pump=0;

//控制数据
u16 need_temp=230;
u16 need_light_intensity=128;
u16 need_soil_moisture=0;
u16 need_water_lavel=0;
u16 need_air_moisture=23;
//公共函数

//延时2us
void Delay_2us(void)
{
	_nop_();
	_nop_();
}

//延时函数，输入int x，延时x/100毫秒
void delay_10us(u16 ten_us){
while(ten_us--);
}

//延时函数，输入int x，延时x毫秒
void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

//数模转换函数部分

//板载ADC2046转换
/*******************************************************************************
* 函 数 名       : xpt2046_wirte_data
* 函数功能		 : XPT2046写数据
* 输    入       : dat：写入的数据
* 输    出    	 : 无
*******************************************************************************/
void xpt2046_wirte_data(u8 dat)
{
	u8 i;

	CLK = 0;
	_nop_();
	for(i=0;i<8;i++)//循环8次，每次传输一位，共一个字节
	{
		DIN = dat >> 7;//先传高位再传低位
		dat <<= 1;//将低位移到高位
		CLK = 0;//CLK由低到高产生一个上升沿，从而写入数据
		_nop_();	
		CLK = 1;
		_nop_();
	}
}

/*******************************************************************************
* 函 数 名       : xpt2046_read_data
* 函数功能		 : XPT2046读数据
* 输    入       : 无
* 输    出    	 : XPT2046返回12位数据
*******************************************************************************/
u16	xpt2046_read_data(void)
{
	u8 i;
	u16 dat=0;

	CLK = 0;
	_nop_();
	for(i=0;i<12;i++)//循环12次，每次读取一位，大于一个字节数，所以返回值类型是u16
	{
		dat <<= 1;
		CLK = 1;
		_nop_();
		CLK = 0; //CLK由高到低产生一个下降沿，从而读取数据
		_nop_();
		dat |= DOUT;//先读取高位，再读取低位。	
	}
	return dat;	
}

/*******************************************************************************
* 函 数 名       : xpt2046_read_adc_value
* 函数功能		 : XPT2046读AD数据
* 输    入       : cmd：指令
* 输    出    	 : XPT2046返回AD值
*******************************************************************************//////////////////////////////////////////////////
u16 xpt2046_read_adc_value(u8 cmd)
{
	u8 i;
	u16 adc_value=0;

	CLK = 0;//先拉低时钟
	CS  = 0;//使能XPT2046
	xpt2046_wirte_data(cmd);//发送命令字
	for(i=6; i>0; i--);//延时等待转换结果
	CLK = 1;
	_nop_();
	CLK = 0;//发送一个时钟，清除BUSY
	_nop_();
	adc_value=xpt2046_read_data();
	CS = 1;//关闭XPT2046
	return adc_value;
}

//外接ADC0832
/*****************************************
函数简介：获取ADC0832数据
函数名称：ADC_read_data(bit channel)
参数说明：ch为入口参数，ch=0选择通道0，ch=1选择通道1
函数返回：返回读取到的二进制ADC数据，格式为unsigned char
		  当返回一直0时，转换数据有误
******************************************/
u8 ADC_read_data(bit channel)
{
	u8 i,dat0=0,dat1=0;
  //------第1次下降沿之前di置高，启动信号---------

	cs=0;			//片选信号置低，启动AD转换芯片
	clk=0;			//时钟置低平
	
	dio=1;  		//开始信号为高电平
	Delay_2us();
	clk=1;			//产生一个正脉冲,在时钟上升沿，输入开始信号（DI=1）
	Delay_2us();
  	clk=0;			//第1个时钟下降沿
	dio=1;
	Delay_2us();
	
	clk=1;		    // 第2个下降沿输入DI=1，表示双通道单极性输入
	Delay_2us();	 
  //------在第2个下降沿，模拟信号输入模式选择（1：单模信号，0：双模差分信号）---------		
	clk=0;	
	dio=channel;         // 第3个下降沿,设置DI，选择通道;
	Delay_2us();
	clk=1;
	Delay_2us();

   //------在第3个下降沿，模拟信号输入通道选择（1：通道CH1，0：通道CH0）------------	
	
	clk=0;
	dio=1;          //第四个下降沿之前，DI置高，准备接收数 
	Delay_2us();	
	for(i=0;i<8;i++)                 //第4~11共8个下降沿读数据（MSB->LSB）
	{
		clk=1;
		Delay_2us();
		clk=0;
		Delay_2us();
		dat0=dat0<<1|dio;
	}
	for(i=0;i<8;i++)                 //第11~18共8个下降沿读数据（LSB->MSB）
	{
		dat1=dat1|((u8)(dio)<<i);
		clk=1;
		Delay_2us();
		clk=0;
		Delay_2us();
	} 
	cs=1;				  
	return (dat0==dat1)?dat0:0;	    //判断dat0与dat1是否相等
}

//LCD函数定义
/*******************************************************************************
* 函 数 名       : lcd1602_write_cmd
* 函数功能		 : LCD1602写命令
* 输    入       : cmd：指令
* 输    出    	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//选择命令
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入	
}
#else	//4位LCD
void lcd1602_write_cmd(u8 cmd)
{
	LCD1602_RS=0;//选择命令
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=cmd;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入
	
	LCD1602_DATAPORT=cmd<<4;//准备命令
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入	
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_write_data
* 函数功能		 : LCD1602写数据
* 输    入       : dat：数据
* 输    出    	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//选择数据
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入		
}
#else
void lcd1602_write_data(u8 dat) 
{
	LCD1602_RS=1;//选择数据
	LCD1602_RW=0;//选择写
	LCD1602_E=0;
	LCD1602_DATAPORT=dat;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入
	
	LCD1602_DATAPORT=dat<<4;//准备数据
	delay_ms(1);
	LCD1602_E=1;//使能脚E先上升沿写入
	delay_ms(1);
	LCD1602_E=0;//使能脚E后负跳变完成写入		
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_init
* 函数功能		 : LCD1602初始化
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************//////////////////////////////////////////////////
#if (LCD1602_4OR8_DATA_INTERFACE==0)//8位LCD
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38);//数据总线8位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);//显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06);//写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01);//清屏	
}
#else
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x28);//数据总线4位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c);//显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06);//写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01);//清屏	
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_clear
* 函数功能		 : LCD1602清屏
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

/*******************************************************************************
* 函 数 名       : lcd1602_show_string
* 函数功能		 : LCD1602显示字符
* 输    入       : x,y：显示坐标，x=0~15，y=0~1;
				   str：显示字符串
* 输    出    	 : 无
*******************************************************************************//////////////////////////////////////////////////
void lcd1602_show_string(u8 x,u8 y,u8 *str)
{
	u8 i=0;

	if(y>1||x>15)return;//行列参数不对则强制退出

	if(y<1)	//第1行显示
	{	
		while(*str!='\0')//字符串是以'\0'结尾，只要前面有内容就显示
		{
			if(i<16-x)//如果字符长度超过第一行显示范围，则在第二行继续显示
			{
				lcd1602_write_cmd(0x80+i+x);//第一行显示地址设置	
			}
			else
			{
				lcd1602_write_cmd(0x40+0x80+i+x-16);//第二行显示地址设置	
			}
			lcd1602_write_data(*str);//显示内容
			str++;//指针递增
			i++;	
		}	
	}
	else	//第2行显示
	{
		while(*str!='\0')
		{
			if(i<16-x) //如果字符长度超过第二行显示范围，则在第一行继续显示
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

//矩阵键盘函数
/*******************************************************************************
* 函 数 名       : key_matrix_flip_scan
* 函数功能		 : 使用线翻转扫描方法，检测矩阵按键是否按下，按下则返回对应键值
* 输    入       : 无
* 输    出    	 : key_value：1-16，对应S1-S16键，
				   0：按键未按下
*******************************************************************************//////////////////////////////////////////////////
u8 key_matrix_flip_scan(void)
{
	static u8 key_value=0;

	KEY_MATRIX_PORT=0x0f;//给所有行赋值0，列全为1
	if(KEY_MATRIX_PORT!=0x0f)//判断按键是否按下
	{
		delay_10us(1000);//消抖
		if(KEY_MATRIX_PORT!=0x0f)
		{
			//测试列
			KEY_MATRIX_PORT=0x0f;
			switch(KEY_MATRIX_PORT)//保存行为0，按键按下后的列值	
			{
				case 0x07: key_value=1;break;
				case 0x0b: key_value=2;break;
				case 0x0d: key_value=3;break;
				case 0x0e: key_value=4;break;
			}
			//测试行
			KEY_MATRIX_PORT=0xf0;
			switch(KEY_MATRIX_PORT)//保存列为0，按键按下后的键值	
			{
				case 0x70: key_value=key_value;break;
				case 0xb0: key_value=key_value+4;break;
				case 0xd0: key_value=key_value+8;break;
				case 0xe0: key_value=key_value+12;break;
			}
			while(KEY_MATRIX_PORT!=0xf0);//等待按键松开	
		}
	}
	else
		key_value=0;		
	
	return key_value;		
}

//步进电机操作函数
/*******************************************************************************
* 函 数 名       : step_motor_28BYJ48_send_pulse
* 函数功能		 : 输出一个数据给ULN2003从而实现向步进电机发送一个脉冲
* 输    入       : step：指定步进序号，可选值0~7
				   dir：方向选择,1：顺时针,0：逆时针
* 输    出    	 : 无
*******************************************************************************/
void step_motor_28BYJ48_send_pulse(u8 step,u8 dir)
{
	u8 temp=step;
	
	if(dir==0)	//如果为逆时针旋转
		temp=7-step;//调换节拍信号
	switch(temp)//8个节拍控制：A->AB->B->BC->C->CD->D->DA
	{
		case 0: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 1: STEPMOTOR1=1;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 2: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=0;STEPMOTOR4=0;break;
		case 3: STEPMOTOR1=0;STEPMOTOR2=1;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 4: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=0;break;
		case 5: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=1;STEPMOTOR4=1;break;
		case 6: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		case 7: STEPMOTOR1=1;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=1;break;
		default: STEPMOTOR1=0;STEPMOTOR2=0;STEPMOTOR3=0;STEPMOTOR4=0;break;//停止相序	
	}			
}

// //操作函数，1上到顶端，0下到底端//////////////////////////////////////////////////////////////////////////////////////////////////
// void step_motor_control(u16 input){
//     if(input!=StepMotor_status)
//     {
// 	    u8 dir=0;//默认逆时针方向
// 	    u8 speed=STEPMOTOR_MAXSPEED;//默认最大速度旋转
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

//温度传感器操作函数
/*******************************************************************************
* 函 数 名         : ds18b20_reset
* 函数功能		   : 复位DS18B20  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ds18b20_reset(void)
{
	TEMPATURE=0;	//拉低DQ
	delay_10us(75);	//拉低750us
	TEMPATURE=1;	//DQ=1
	delay_10us(2);	//20US
}

/*******************************************************************************
* 函 数 名         : ds18b20_check
* 函数功能		   : 检测DS18B20是否存在
* 输    入         : 无
* 输    出         : 1:未检测到DS18B20的存在，0:存在
*******************************************************************************/
u8 ds18b20_check(void)
{
	u8 time_temp=0;

	while(TEMPATURE&&time_temp<20)	//等待DQ为低电平
	{
		time_temp++;
		delay_10us(1);	
	}
	if(time_temp>=20)return 1;	//如果超时则强制返回1
	else time_temp=0;
	while((!TEMPATURE)&&time_temp<20)	//等待DQ为高电平
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)return 1;	//如果超时则强制返回1
	return 0;
}

/*******************************************************************************
* 函 数 名         : ds18b20_read_bit
* 函数功能		   : 从DS18B20读取一个位
* 输    入         : 无
* 输    出         : 1/0
*******************************************************************************/
u8 ds18b20_read_bit(void)
{
	u8 dat=0;
	
	TEMPATURE=0;
	_nop_();_nop_();
	TEMPATURE=1;	
	_nop_();_nop_(); //该段时间不能过长，必须在15us内读取数据
	if(TEMPATURE)dat=1;	//如果总线上为1则数据dat为1，否则为0
	else dat=0;
	delay_10us(5);
	return dat;
} 

/*******************************************************************************
* 函 数 名         : ds18b20_read_byte
* 函数功能		   : 从DS18B20读取一个字节
* 输    入         : 无
* 输    出         : 一个字节数据
*******************************************************************************/
u8 ds18b20_read_byte(void)
{
	u8 i=0;
	u8 dat=0;
	u8 temp=0;

	for(i=0;i<8;i++)//循环8次，每次读取一位，且先读低位再读高位
	{
		temp=ds18b20_read_bit();
		dat=(temp<<7)|(dat>>1);
	}
	return dat;	
}

/*******************************************************************************
* 函 数 名         : ds18b20_write_byte
* 函数功能		   : 写一个字节到DS18B20
* 输    入         : dat：要写入的字节
* 输    出         : 无
*******************************************************************************/
void ds18b20_write_byte(u8 dat)
{
	u8 i=0;
	u8 temp=0;

	for(i=0;i<8;i++)//循环8次，每次写一位，且先写低位再写高位
	{
		temp=dat&0x01;//选择低位准备写入
		dat>>=1;//将次高位移到低位
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
* 函 数 名         : ds18b20_start
* 函数功能		   : 开始温度转换
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ds18b20_start(void)
{
	ds18b20_reset();//复位
	ds18b20_check();//检查DS18B20
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0x44);//转换命令	
}

/*******************************************************************************
* 函 数 名         : ds18b20_init
* 函数功能		   : 初始化DS18B20的IO口 DQ 同时检测DS的存在
* 输    入         : 无
* 输    出         : 1:不存在，0:存在
*******************************************************************************//////////////////////////////////////////////////
u8 ds18b20_init(void)
{
	ds18b20_reset();
	return ds18b20_check();	
}

/*******************************************************************************
* 函 数 名         : ds18b20_read_temperture
* 函数功能		   : 从ds18b20得到温度值
* 输    入         : 无
* 输    出         : 温度数据
*******************************************************************************//////////////////////////////////////////////////
// float temperature_read(void)
// {
// 	float temp;
// 	u8 dath=0;
// 	u8 datl=0;
// 	u16 value=0;

// 	ds18b20_start();//开始转换
// 	ds18b20_reset();//复位
// 	ds18b20_check();
// 	ds18b20_write_byte(0xcc);//SKIP ROM
//     ds18b20_write_byte(0xbe);//读存储器

// 	datl=ds18b20_read_byte();//低字节
// 	dath=ds18b20_read_byte();//高字节
// 	value=(dath<<8)+datl;//合并为16位数据

// 	if((value&0xf800)==0xf800)//判断符号位，负温度
// 	{
// 		value=(~value)+1; //数据取反再加1
// 		temp=value*(-0.0625);//乘以精度	
// 	}
// 	else //正温度
// 	{
// 		temp=value*0.0625;	
// 	}
// 	return temp;
// }

//空气湿度传感器操作函数
//复位DHT11
void DHT11_Rst(void)	   
{                 
	MOISTURE_AIR=1;
	delay_10us(1);
	MOISTURE_AIR=0; 	//拉低DQ
    delay_ms(25);   //拉低至少18ms
    MOISTURE_AIR=1; 	//DQ=1 
	delay_10us(3);  //主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	
	while (!MOISTURE_AIR&&retry<100)//判断从机发出 80us 的低电平响应信号是否结束
	{
		retry++;
		_nop_();
	};
	if(retry>=100)return 1;
	else retry=0;
    while (MOISTURE_AIR&&retry<100)//判断从机发出 80us 的高电平是否结束?Ｈ缃崾?则主机进入数据接收状态
	{
		retry++;
		_nop_();
	};	 
	if(retry>=100)return 1;	    
	return 0;
}


//DHT11初始化 //////////////////////////////////////////////////////////////////////////////////////////////////
//返回0：初始化成功，1：失败
u8 DHT11_Init(void)
{
	MOISTURE_AIR=1;
	DHT11_Rst();	  
	return DHT11_Check();	
}

//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,temp;
	u8 data_byte=0; 
	u8 retry=0;

  	for(i=0;i<8;i++)//接收8bit的数据 
  	{ 
//		while(!MOISTURE_AIR);//等待50us的低电平开始信号结束
		while (!MOISTURE_AIR&&retry<50)//等待50us的低电平开始信号结束
		{
			retry++;
			_nop_();
		};
		retry=0; 
		delay_10us(3);//等待40us 
		temp=0;//时间为26us-28us?１硎窘邮盏奈?数据'0' 
		if(MOISTURE_AIR==1) 
			temp=1; //如果26us-28us之后?；刮?高电平?Ｔ虮硎窘邮盏氖?据为'1' 
//		while(MOISTURE_AIR);//等待数据信号高电平??'0'为26us-28us??'1'为70us
		while (MOISTURE_AIR&&retry<100)//等待数据信号高电平??'0'为26us-28us??'1'为70us
		{
			retry++;
			_nop_();
		};
		data_byte<<=1;//接收的数据为高位在前?∮乙篇? 
		data_byte|=temp; 
  	} 

  	return data_byte;
}

// //从DHT11读取一次数据//////////////////////////////////////////////////////////////////////////////////////////////////
// //temp:温度值(范围:0~50°)
// //humi:湿度值(范围:20%~90%)
// //返回值：0,正常;1,读取失败
// u8 air_moisture_read(u8 *temp,u8 *humi)    
// {        
//  	u8 buf[5];
// 	u8 i;
// 	DHT11_Rst();
// 	if(DHT11_Check()==0)
// 	{
// 		for(i=0;i<5;i++)//读取40位数据
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

//读取温度
float temperature_read(void)
{
	float temp;
	u8 dath=0;
	u8 datl=0;
	u16 value=0;

	ds18b20_start();//开始转换
	ds18b20_reset();//复位
	ds18b20_check();
	ds18b20_write_byte(0xcc);//SKIP ROM
    ds18b20_write_byte(0xbe);//读存储器

	datl=ds18b20_read_byte();//低字节
	dath=ds18b20_read_byte();//高字节
	value=(dath<<8)+datl;//合并为16位数据

	if((value&0xf800)==0xf800)//判断符号位，负温度
	{
		value=(~value)+1; //数据取反再加1
		temp=value*(-0.0625);//乘以精度	
	}
	else //正温度
	{
		temp=value*0.0625;	
	}
	return temp;
}

//土壤湿度传感器读取
u8 soil_moisture_read(void)
{
	u8 soil_moisture_adc;
	soil_moisture_adc=ADC_read_data(0);
	return soil_moisture_adc;
}
//水位传感器读取
u8 water_lavel_read(void)
{
	u8 water_lavel_adc;
	water_lavel_adc=ADC_read_data(1);
	return water_lavel_adc;
}

//从DHT11读取一次数据
//temp:温度值
//humi:湿度值
void air_moisture_read(void)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
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

//光照强度探测
u8 light_intensity_read(void)
{
	u8 light_value_adc;
	light_value_adc=xpt2046_read_adc_value(0xE4);
	return light_value_adc;
}

//遮光板操作函数，1上到顶端，0下到底端
void step_motor_control(u16 input){
    if(input!=StepMotor_status)
    {
	    u8 dir=0;//默认逆时针方向
	    u8 speed=STEPMOTOR_MAXSPEED;//默认最大速度旋转
	    u8 step=0;
        u16 time=0;
        u16 usetime=STEPMOTOR_SHIELD_HIGHT*STEPMOTOR_SHIELD_RANGE;

        if(input==1)
        {
            while(time<=usetime){
                step_motor_28BYJ48_send_pulse(step++,dir);
								time++;
								if(step==8)step=0;		
								delay_ms(speed);
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
								delay_ms(speed);
            }
			StepMotor_status=0;
        }
    }
}

//水泵操作函数，输入供水多少毫秒
void pump_control(u16 input)
{
    PUMP=1;
    delay_ms(input);
    PUMP=0;
}

//雾化器操作函数，1开启，0关闭
void spray_control(u16 input)
{
    SPRAY=input;
}

//补光灯操作函数，1开启，0关闭
void light_control(u16 input)
{
    LIGHT=input;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void all_data_read(void)
{
	//温度
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

	//土壤湿度
	soil_moisture=soil_moisture_read();

	soil_moisture_buf[0]=soil_moisture%1000/100+0x30;
	soil_moisture_buf[1]=soil_moisture%1000%100/10+0x30;
	soil_moisture_buf[2]=soil_moisture%1000%100%10+0x30;
	soil_moisture_buf[3]='\0';

	//水位高度
	water_lavel=water_lavel_read();

	water_lavel_buf[0]=water_lavel%1000/100+0x30;
	water_lavel_buf[1]=water_lavel%1000%100/10+0x30;
	water_lavel_buf[2]=water_lavel%1000%100%10+0x30;
	water_lavel_buf[3]='\0';

	//空气湿度
	air_moisture_read();
	temp_buf2[0]=temp/10+0x30;	
	temp_buf2[1]=temp%10+0x30;
 	temp_buf2[2]='\0';

	humi_buf[0]=humi/10+0x30;	
	humi_buf[1]=humi%10+0x30;
 	humi_buf[2]='\0';

	//光照强度
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
	step_motor_control(0);
	pump_control(500);
	spray_control(0);
	light_control(0);
	allow_test2=0;
	allow_pump=0;
	lcd1602_init();
	lcd1602_show_string(0,0,"Self-test");
	lcd1602_show_string(0,1,"Success!!!");
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//测试函数翻写
void test_key(void)
{
	key=key_matrix_flip_scan();
	switch(key)
	{
		case(1):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Temperature:");//第一行显示
			lcd1602_show_string(0,1,temp_buf);//第二行显示
			break;	
		}
		case(2):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Soil Moisture:");//第一行显示
			lcd1602_show_string(0,1,soil_moisture_buf);//第二行显示
			break;	
		}
		case(3):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Water Level:");//第一行显示
			lcd1602_show_string(0,1,water_lavel_buf);//第二行显示
			break;	
		}
		case(4):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Air Moisture:");//第一行显示
			lcd1602_show_string(0,1,humi_buf);//第二行显示
			break;	
		}
		case(5):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Light Intensity:");//第一行显示
			lcd1602_show_string(0,1,light_intensity_buf);//第二行显示
			break;	
		}
		case(6):{
			if(StepMotor_status==0){
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Rising");
				step_motor_control(1);
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Rised");
			}
			else{
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Falling");
				step_motor_control(0);
				lcd1602_init();
				lcd1602_show_string(0,0,"Shield Falled");
			}
			break;	
		}
		case(7):{
			lcd1602_init();
			lcd1602_show_string(0,0,"Pump Working");
			pump_control(3000);
			lcd1602_show_string(0,0,"Pump Worked 3s");
			break;	
		}
		case(8):{
			if(SPRAY==0){
				lcd1602_init();
				lcd1602_show_string(0,0,"Spray Work Begin");
				spray_control(1);
			}
			else{
				lcd1602_init();
				lcd1602_show_string(0,0,"Spray Work Stop");
				spray_control(0);
			}
			break;	
		}
		case(9):{
			if(LIGHT==0){
				lcd1602_init();
				lcd1602_show_string(0,0,"LED Work Begin");
				light_control(1);
			}
			else{
				lcd1602_init();
				lcd1602_show_string(0,0,"LED Work Stop");
				light_control(0);
			}
			break;	
		}
		case(10):{
			if(allow_test2==0){
				lcd1602_init();
				lcd1602_show_string(0,0,"AUTO Work Begin");
				allow_test2=1;
			}
			else{
				lcd1602_init();
				lcd1602_show_string(0,0,"AUTO Work Stop");
				allow_test2=0;
			}
			break;	
		}
		case(11):{
			if(allow_pump==0){
				lcd1602_init();
				lcd1602_show_string(0,0,"Pump Allow");
				allow_pump=1;
			}
			else{
				lcd1602_init();
				lcd1602_show_string(0,0,"Pump NOT Allow");
				allow_pump=0;
			}
			break;	
		}
		case(12):{
			all_init();
			break;	
		}
		case(13):{
			
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



void test2(void){
	if(temp_value > need_temp)
		step_motor_control(1);
	if(temp_value < need_temp)
		step_motor_control(0);

	if(light_intensity > need_light_intensity)
		light_control(1);
	if(light_intensity < need_light_intensity)
		light_control(0);

	if(humi < need_air_moisture)
		spray_control(1);
	if(humi > need_air_moisture)
		spray_control(0);

	if(water_lavel<=need_water_lavel&&allow_pump==1)
		pump_control(1000);
		all_data_read();

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
		test_key();
		if(allow_test2)
			test2();

	}
}
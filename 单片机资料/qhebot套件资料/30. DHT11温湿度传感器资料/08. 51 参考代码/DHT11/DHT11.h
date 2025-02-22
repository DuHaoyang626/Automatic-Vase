#ifndef 	_DHT11_h_
#define 	_DHT11_h_
/**************************************************************
               iO口宏定义区
***************************************************************/
sbit DHT_IO=P3^4;

unsigned char  humidity_H,humidity_L,temperature_H,temperature_L,checkdata;

/**
*延时函数：单位 ms
*/
void delay_DH_ms(unsigned int x)
{
    unsigned int i,j;
    for(i=x; i>0; i--)
        for(j=110; j>0; j--);
}

/**
*延时函数： 延时10us
*/
void  Delay_DHT_10us(void)
{
    unsigned char  i; i--; i--; i--; i--; i--; i--;
} 


/**
*根据时序计算温湿度值
*/
unsigned char computeData(){
	
    unsigned char i,U8comdata, U8temp;
    for(i=0; i<8; i++)
    {
        while(DHT_IO==0);				//等待54us低电平
        Delay_DHT_10us();					//延时30us确认高电平是否结束
        Delay_DHT_10us();
        Delay_DHT_10us();
        U8temp=0;
        if(DHT_IO==1)						//如果高电平高过预定0高电平
        {											//值则数据位为 1
            U8temp=1;
        }
        while(DHT_IO==1);			//等待高电平结束
        U8comdata<<=1;
        U8comdata|=U8temp;
    }
    return U8comdata;
}


//--------------------------------
//-----湿度读取子程序 ------------
//--------------------------------
//----以下变量均为全局变量--------
//----温度高8位== temperature_H------
//----温度低8位== temperature_L------
//----湿度高8位== humidity_H-----
//----湿度低8位== humidity_L-----
//----校验 8位 == checkdata-----
//--------------------------------
void readData()
{
    unsigned char  humidity_H_temp,humidity_L_temp,temperature_H_temp,temperature_L_temp,checkdata_temp;
    //主机拉低不少于18ms
    DHT_IO=0;
    delay_DH_ms(20);
		//总线由上拉电阻拉高 主机延时13us最多20us 等待DHT11低电平响应
    DHT_IO=1;
		Delay_DHT_10us();
		Delay_DHT_10us();
    //判断从机是否有低电平响应信号
    if(DHT_IO==0)
    {
        while(DHT_IO==0);				//判断DHT11发出 83us 的低电平响应信号是否结束
        while(DHT_IO==1);				//判断DHT11发出 87us 的高电平是否结束
        //数据接收状态
        humidity_H_temp = computeData();
        humidity_L_temp = computeData();
        temperature_H_temp = computeData();
        temperature_L_temp = computeData();
        checkdata_temp = computeData();
        DHT_IO = 1;		//data拉高释放总线
        //数据校验
        if(checkdata_temp == humidity_H_temp + humidity_L_temp + temperature_H_temp + temperature_L_temp)
        {
            humidity_H = humidity_H_temp;
            humidity_L = humidity_L_temp;
            temperature_H = temperature_H_temp;
            temperature_L = temperature_L_temp;
            checkdata = checkdata_temp;
        }
    }

}


#endif
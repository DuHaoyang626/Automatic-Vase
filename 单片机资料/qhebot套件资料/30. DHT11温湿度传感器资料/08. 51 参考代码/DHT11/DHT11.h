#ifndef 	_DHT11_h_
#define 	_DHT11_h_
/**************************************************************
               iO�ں궨����
***************************************************************/
sbit DHT_IO=P3^4;

unsigned char  humidity_H,humidity_L,temperature_H,temperature_L,checkdata;

/**
*��ʱ��������λ ms
*/
void delay_DH_ms(unsigned int x)
{
    unsigned int i,j;
    for(i=x; i>0; i--)
        for(j=110; j>0; j--);
}

/**
*��ʱ������ ��ʱ10us
*/
void  Delay_DHT_10us(void)
{
    unsigned char  i; i--; i--; i--; i--; i--; i--;
} 


/**
*����ʱ�������ʪ��ֵ
*/
unsigned char computeData(){
	
    unsigned char i,U8comdata, U8temp;
    for(i=0; i<8; i++)
    {
        while(DHT_IO==0);				//�ȴ�54us�͵�ƽ
        Delay_DHT_10us();					//��ʱ30usȷ�ϸߵ�ƽ�Ƿ����
        Delay_DHT_10us();
        Delay_DHT_10us();
        U8temp=0;
        if(DHT_IO==1)						//����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽ
        {											//ֵ������λΪ 1
            U8temp=1;
        }
        while(DHT_IO==1);			//�ȴ��ߵ�ƽ����
        U8comdata<<=1;
        U8comdata|=U8temp;
    }
    return U8comdata;
}


//--------------------------------
//-----ʪ�ȶ�ȡ�ӳ��� ------------
//--------------------------------
//----���±�����Ϊȫ�ֱ���--------
//----�¶ȸ�8λ== temperature_H------
//----�¶ȵ�8λ== temperature_L------
//----ʪ�ȸ�8λ== humidity_H-----
//----ʪ�ȵ�8λ== humidity_L-----
//----У�� 8λ == checkdata-----
//--------------------------------
void readData()
{
    unsigned char  humidity_H_temp,humidity_L_temp,temperature_H_temp,temperature_L_temp,checkdata_temp;
    //�������Ͳ�����18ms
    DHT_IO=0;
    delay_DH_ms(20);
		//������������������ ������ʱ13us���20us �ȴ�DHT11�͵�ƽ��Ӧ
    DHT_IO=1;
		Delay_DHT_10us();
		Delay_DHT_10us();
    //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�
    if(DHT_IO==0)
    {
        while(DHT_IO==0);				//�ж�DHT11���� 83us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
        while(DHT_IO==1);				//�ж�DHT11���� 87us �ĸߵ�ƽ�Ƿ����
        //���ݽ���״̬
        humidity_H_temp = computeData();
        humidity_L_temp = computeData();
        temperature_H_temp = computeData();
        temperature_L_temp = computeData();
        checkdata_temp = computeData();
        DHT_IO = 1;		//data�����ͷ�����
        //����У��
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
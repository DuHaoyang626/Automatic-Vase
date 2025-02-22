#include "lcd9648.h"
#include "font.h"

/*******************************************************************************
* �� �� ��       : lcd9648_spi_write_byte
* ��������		 : LCD9648 SPI��������
* ��    ��       : dat������
* ��    ��    	 : ��
*******************************************************************************/
void lcd9648_spi_write_byte(u8 dat)
{
	u8 i=0;

   	for(i=0;i<8;i++)//ѭ��8�ν�һ���ֽڴ���			
   	{  
      	if((dat&0x80)!=0) 
			SDA = 1;
        else 
			SDA = 0;
      	dat<<= 1;//��д��λ��д��λ
	  	SCL = 0;
      	SCL = 1;//ʱ��������д������			
   	}
}

/*******************************************************************************
* �� �� ��       : lcd9648_write_cmd
* ��������		 : LCD9648д����
* ��    ��       : cmd������
* ��    ��    	 : ��
*******************************************************************************/
void lcd9648_write_cmd(u8 cmd)
{
	CS0 = 0;//ʹ��
	RS  = 0;//ѡ������
	lcd9648_spi_write_byte(cmd);//��������
	CS0 = 1;//ʧ��	
}

/*******************************************************************************
* �� �� ��       : lcd9648_write_dat
* ��������		 : LCD9648д����
* ��    ��       : dat������
* ��    ��    	 : ��
*******************************************************************************/
void lcd9648_write_dat(u8 dat)
{
	CS0 = 0;//ʹ��
	RS  = 1;//ѡ������
	lcd9648_spi_write_byte(dat);//��������
	CS0 = 1;//ʧ��	
}

/*******************************************************************************
* �� �� ��       : lcd9648_init
* ��������		 : LCD9648��ʼ��
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd9648_init(void)
{
	RST=1;  
	delay_ms(10);
	RST=0;
	delay_ms(10);
	RST=1;
	delay_ms(10);//��λ

    lcd9648_write_cmd(0xe2);		  //�����λ
	lcd9648_write_cmd(0xc8);		  //0xC8��ͨ����ѡ��ѡ����0xC0Ϊ��������
	lcd9648_write_cmd(0xa0);		  //0xA0�η���ѡ����������0xA1Ϊ������
	lcd9648_write_cmd(0x2f);
	lcd9648_write_cmd(0x26);
	lcd9648_write_cmd(0x81);		  //������Աȶ�
	lcd9648_write_cmd(0x10);
	lcd9648_write_cmd(0xaf);		  //������ʾ
	lcd9648_write_cmd(0x40);		  //���ù���һ������
}

/*******************************************************************************
* �� �� ��       : lcd9648_clear
* ��������		 : LCD9648����
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd9648_clear(void)
{
	u8 i,j;

	for(i=0;i<9;i++)//һ��9ҳ
	{
		lcd9648_write_cmd(0x40);//���ù���һ������
		lcd9648_write_cmd(0xb0+i);//ҳ��ַ
		lcd9648_write_cmd(0x10);//�и��ֽڵ�ַ
		lcd9648_write_cmd(0x00);//�е��ֽڵ�ַ
		
		for(j = 0; j < LCD_WIDTH; j++)//ÿһ�е����ص�
		{
			lcd9648_write_dat(0x00);//����ʾ	
		}	
	}
}

//��ָ��λ����ʾһ��16*8�ַ�
//x,y:��ʼ���� x:0-96 y:0-2 
//num:Ҫ��ʾ���ַ�:" "--->"~"
void lcd_show_char(u8 x,u8 y,u8 num)
{
	unsigned char i, x1, x2;

	if(x>96||y>2)return;//������Χ��

	y *= 2;
	y += 0xB0;	   			//��ȡY�����ֵ
	x1 = (x >> 4) & 0x0F;   //����X����Ҫ��������ָߵ�4λ������������ȡ����4λ
	x2 = x & 0x0F;          //ȥ����λ

	lcd9648_write_cmd(y);	//ҳ��ַ y����
	lcd9648_write_cmd(0x10+x1);	//�и��ֽڵ�ַ x����
	lcd9648_write_cmd(0x00+x2);	//�е��ֽڵ�ַ

	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

	for(i = 0;i < 8;i++) 
	{
		lcd9648_write_dat(ascii_1608[num][i*2]);
	}
	
	lcd9648_write_cmd(y+1);	//ҳ��ַ y����
	lcd9648_write_cmd(0x10+x1);	//�и��ֽڵ�ַ x����
	lcd9648_write_cmd(0x00+x2);	//�е��ֽڵ�ַ
	
	for(i = 0;i < 8;i++) 
	{
		lcd9648_write_dat(ascii_1608[num][i*2+1]);
	}

}

//��ʾ�ַ���
//x,y:������� x:0-96 y:0-2 
//*p:�ַ�����ʼ��ַ		  
void lcd_show_string(u8 x,u8 y,u8 Num,u8 *p)
{
	u8 i = 0;
	while(i < Num)
	{
		lcd_show_char(x,y,*p);
		x+=8;
		p++;
		i++;
		if(*p == '\0' || i >= 12)
			return;
	}
}

//m^n����
//����ֵ:m^n�η�.
u32 lcd_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//��ʾ����,
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//mode:0-��λ���ַ����  1-��λ��0 
void lcd_show_num(u8 x,u8 y,u32 num,u8 len,u8 mode)
{
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/lcd_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode == 1)
					lcd_show_char(x+(16/2)*t,y,'0');
				else
					lcd_show_char(x+(16/2)*t,y,' ');
 				continue;
			}else enshow=1; 
		 	 
		}
	 	lcd_show_char(x+(16/2)*t,y,temp+'0'); 
	}
}

//��ʾ16*16����
//x,y:������� x:0-96 y:0-2
//*cn:�ַ�����ʼ��ַ		  
void lcd_show_16HZ(u8 x, u8 y, u8 *cn)
{
	u8 j, x1, x2, wordNum;
	
	if(x>96||y>5)return;//������Χ��

	y *= 2;
	y += 0xB0;	   			//��ȡY�����ֵ

	while ( *cn != '\0')	 //��C�������ַ��������ԡ�\0����β
	{
		x1 = (x >> 4) & 0x0F;   //����X����Ҫ��������ָߵ�4λ������������ȡ����4λ
		x2 = x & 0x0F;          //ȥ����λ

		lcd9648_write_cmd(y);	//ҳ��ַ y����
		lcd9648_write_cmd(0x10+x1);	//�и��ֽڵ�ַ x����
		lcd9648_write_cmd(0x00+x2);	//�е��ֽڵ�ַ

		for (wordNum=0; wordNum<30; wordNum++)
		{
			//--��ѯҪд�������ֿ��е�λ��--//
			if ((CnChar16x16[wordNum].Index[0] == *cn)&&(CnChar16x16[wordNum].Index[1] == *(cn+1)))
			{
				for (j=0; j<32; j++) //дһ����
				{
					if (j == 16)	 //����16X16�õ�����Y���꣬�����ڵ���16ʱ���л�����
					{
						lcd9648_write_cmd(y+1);	//ҳ��ַ y����
						lcd9648_write_cmd(0x10+x1);	//�и��ֽڵ�ַ x����
						lcd9648_write_cmd(0x00+x2);	//�е��ֽڵ�ַ
					}
					lcd9648_write_dat(CnChar16x16[wordNum].Msk[j]);
				}
				x += 16;
			}
		}
		cn += 2;	//��һ����
	}
}

//��ʾͼƬ
void lcd_show_Image(u8 *p)
{
	unsigned char i,j;

	for(i=0;i<6;i++)
	{
		lcd9648_write_cmd(0x40);
		lcd9648_write_cmd(0xb0|i);
		lcd9648_write_cmd(0x10);
		lcd9648_write_cmd(0x00);
		for(j = 0;j < 96;j++)
		{
			lcd9648_write_dat(*p);
			p++;
		}
	}
}

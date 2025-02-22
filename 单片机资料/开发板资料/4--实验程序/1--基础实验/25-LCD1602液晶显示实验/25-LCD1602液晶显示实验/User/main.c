/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中

实验名称：LCD1602液晶显示实验
接线说明：	
实验现象：下载程序后，LCD1602上显示字符信息
注意事项：																				  
***************************************************************************************/
#include "public.h"
#include "lcd1602.h"


/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	lcd1602_init();//LCD1602初始化
	
	while(1)
	{
		key=key_matrix_flip_scan();
		switch(key)
		{
			case(1):{
				lcd1602_show_string(0,0,"Temperature:");//?????
				lcd1602_show_string(0,1,temp_buf);//?????
				break;	
			}
			case(2):{
				lcd1602_show_string(0,0,"Soil Moisture:");//?????
				lcd1602_show_string(0,1,humi_buf);//?????
				break;	
			}
			case(3):{
				lcd1602_show_string(0,0,"Water Level:");//?????
				lcd1602_show_string(0,1,"N/A");//?????
				break;	
			}
			case(4):{
				lcd1602_show_string(0,0,"Air Moisture:");//?????
				lcd1602_show_string(0,1,"N/A");//?????
				break;	
			}
			case(5):{
				lcd1602_show_string(0,0,"Light Intensity:");//?????
				lcd1602_show_string(0,1,"N/A");//?????
				break;	
			}
			case(6):{
				lcd1602_show_string(0,0,"Shield Rising");
				step_motor_control(1);
				lcd1602_show_string(0,0,"Shield Rised");
				break;	
			}
			case(7):{
				lcd1602_show_string(0,0,"Shield Falling");
				step_motor_control(0);
				lcd1602_show_string(0,0,"Shield Falled");
				break;	
			}
			case(8):{
				lcd1602_show_string(0,0,"Pump Working");
				pump_control(5000);
				lcd1602_show_string(0,0,"Pump Worked 5s");
				break;	
			}
			case(9):{
				lcd1602_show_string(0,0,"Spray Work Begin");
				spray_control(1);
				break;	
			}
			case(10):{
				lcd1602_show_string(0,0,"Spray Work Stop");
				spray_control(0);
				break;	
			}
			case(11):{
				lcd1602_show_string(0,0,"LED Work Begin");
				light_control(1);
				break;	
			}
			case(12):{
				lcd1602_show_string(0,0,"LED Work Stop");
				light_control(0);
				break;	
			}
			case(13):{
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

/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：LCD9648液晶显示实验
接线说明：	
实验现象：下载程序后，LCD9648上显示字符汉字信息
注意事项：																				  
***************************************************************************************/
#include "public.h"
#include "lcd9648.h"
#include "pic.h"

/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	lcd9648_init();

	delay_ms(300);

	while(1)
	{
		//清屏
		lcd9648_clear();
		
		//显示单个字符
		lcd_show_char(0,0,'H');
		lcd_show_char(8,0,'e');
		lcd_show_char(16,0,'l');
		lcd_show_char(24,0,'l');
		lcd_show_char(32,0,'o');
	
		//显示数字
		lcd_show_num(48,0,12345,5,0);
	
		//显示字符串
		lcd_show_string(0,1,12,"HELLO WORLD");
	
		//显示中文
		lcd_show_16HZ(0,2,"普中科技");
		
		delay_ms(2000);
		//显示图片
		lcd_show_Image(gImage_pic);
		
		delay_ms(2000);
			
	}	
}

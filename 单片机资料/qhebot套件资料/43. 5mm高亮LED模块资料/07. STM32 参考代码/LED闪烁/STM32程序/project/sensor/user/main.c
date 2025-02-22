#include "sysinc.h"

void timer2()
{	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  //���TIM3�����жϷ������
		  PCout(13)= !PCout(13);
	  	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
	}
}

int main(void){
	 
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	
	
	TIM_ms_Init(TIM2, 500, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER2, timer2);	 
	 
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_7, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�	

	PAout(7)= 1;

		
	 while(1){
	
		 PAout(7)= 0;
 		 SYSTICK_DelayMs(1000);
		 
		 PAout(7)= 1;
 		 SYSTICK_DelayMs(1000);

	 }
}
 


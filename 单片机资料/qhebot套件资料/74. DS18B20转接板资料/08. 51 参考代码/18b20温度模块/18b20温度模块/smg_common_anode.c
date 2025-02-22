/*
*************************************************************************
*硬件平台: STC89C52RC
*开发平台：keil 4.7
*版权    ：沁和智能科技
*开始时间：2019.7.11
*结束时间：2019.7.11
*作者：    张文超
*备注：    数码管库函数 --- 未经允许，用于商业用途，必究责任
*************************************************************************
*/

#include "smg_common_anode.h"

#define SMG_WEI_XUAN(i) 	switch(i){ \
													case 0: smg_we1 = 0;  smg_we2 = 1; smg_we3 = 1;  smg_we4 = 1; break; \
													case 1: smg_we1 = 1;  smg_we2 = 0; smg_we3 = 1;  smg_we4 = 1; break;  \
													case 2: smg_we1 = 1;  smg_we2 = 1; smg_we3 = 0;  smg_we4 = 1; break; \
													case 3: smg_we1 = 1;  smg_we2 = 1; smg_we3 = 1;  smg_we4 = 0; break; \
												}	

unsigned char dis_smg_tmp[4] = {0xc0,0xf9,0xa4,0xb0}; 
int pre_change_num = 0;
unsigned char flag_num_change = 0;

void set_change(){
	int i = 0;
	if(flag_num_change == 1){
			for(i=0; i < smg_num; i++){
				dis_smg[i] = dis_smg_tmp[i];
			}
	}
}

/***********************数码显示函数*****************************/
void display()
{
	static unsigned char current_selection = 0;   
	current_selection++;
	if(current_selection >= smg_num) {
		 current_selection = 0;	
			set_change();
	}
	
	SMG_WEI_XUAN(current_selection);           //位选
	P_DATA_SMG = dis_smg[current_selection];	 //段选	     
}

void smg_common_A_init(){
		timer2_start(5, display);
}

//num -- 要显示的数值，整型
//dotnum -- 小数点显示位置（不要时填写10）
void set_smg_change(int num, int dotNum){
	if(pre_change_num != num){
		pre_change_num = num;
		flag_num_change = 1;
		if(num > 9999){
			dis_smg_tmp[0] = smg_du[9];
			dis_smg_tmp[1] = smg_du[9];
			dis_smg_tmp[2] = smg_du[9];
			dis_smg_tmp[3] = smg_du[9];
		}else{
			dis_smg_tmp[0] = smg_du[num/1000];
			dis_smg_tmp[1] = smg_du[(num/100)%10];
			dis_smg_tmp[2] = smg_du[(num/10)%10];
			dis_smg_tmp[3] = smg_du[num%10];
			if( dotNum >= 0 && dotNum < smg_num ){
				 dis_smg_tmp[dotNum] &= smg_du[17];
			}
		}
	}
}

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

#ifndef SMG_COMMON_ANODE_H
#define SMG_COMMON_ANODE_H

#include <reg52.h>
#include <intrins.h>
#include "timer2.h"

//use timer2

//数码管段选定义      0     1    2    3    4    5	6	 7	  8	   9	
static code unsigned char smg_du[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
				  	 0x88,0x83,0xc6,0xa1,0x86,0x8e,0xff, 0x7f};	 //断码

static code unsigned char smg_num = 4;    //显示数码管的个位数

static unsigned char dis_smg[4]   = {0x80,0x80,0x80,0x80};   


sbit smg_we1 = P2^4;	  //数码管位选定义
sbit smg_we2 = P2^5;
sbit smg_we3 = P2^6;
sbit smg_we4 = P2^7;
#define P_DATA_SMG P1

void smg_common_A_init(); 
void set_smg_change(int num, int dotNum);

#endif

//static code smg_wei_order[4][4] = {	{0, 1, 1, 1},
//																		{1, 0, 1, 1},
//																		{1, 1, 0, 1},
//																		{1, 1, 1, 0}
//																	};
//#define SMG_WEI_XUAN(i)  smg_we1 = smg_wei_order[i][0]; smg_we2 = smg_wei_order[i][1]; smg_we3 = smg_wei_order[i][2]; smg_we4 = smg_wei_order[i][3];


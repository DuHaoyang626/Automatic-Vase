#ifndef TIMER_H
#define TIMER_H

#include <reg52.h>

//PWM 最大频率支持3K
sbit PWM0_PIN = P1^0;


sfr T2MOD = 0xC9;

typedef void (*TIMER_CallBackType)(void);

static TIMER_CallBackType TIMER_CallBackTable[3];

void timer0_start(int ms, TIMER_CallBackType AppCBFun);
void timer0_stop();

void timer0_pwm_start(int frequency, float duty);

void timer1_start(int ms, TIMER_CallBackType AppCBFun);
void timer1_stop();

void timer2_start(int ms, TIMER_CallBackType AppCBFun);
void timer2_stop();

#endif

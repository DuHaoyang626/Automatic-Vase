#ifndef TIMER_H
#define TIMER_H

#include <reg52.h>

typedef void (*TIMER_CallBackType)(void);

static TIMER_CallBackType TIMER_CallBackTable[1];

void timer0_start(int ms, TIMER_CallBackType AppCBFun);
void timer0_stop();

#endif

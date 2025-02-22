
#ifndef _DS18B20_H
#define _DS18B20_H

#include <reg52.h>


sbit DQ = P3^6;

void DS18B20Init(void);
float Get18B20Temp(void);



#endif

#include <reg52.h>
#include "timer2.h"

#include "stepper_motor_5L4P.h"


void Delay(unsigned int i){
 while(--i);
}



main(){
 while(1){
		stepper_init(ROT1_SEC2008*2, POSITIVE );
		Delay(50000);
		Delay(50000);
		Delay(50000);
		Delay(50000);
		Delay(50000);

		stepper_init(ROT1_SEC2008*2, INVERTION );
		Delay(50000);
		Delay(50000);
		Delay(50000);
		Delay(50000);
		Delay(50000);
 }
}

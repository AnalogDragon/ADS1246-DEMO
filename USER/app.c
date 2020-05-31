#include "app.h"


void LED_Task(void){
	static u8 i;
	u8 Time = 0;
	if(SysState.err.all)Time = 20;
	else Time = 100;
	
	i++;
	if(i >= Time)i=0;
	if(i<5)LEDR = 1;
	else LEDR = 0;
	
	
}




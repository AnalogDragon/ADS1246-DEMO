#include "app.h"


void LED_Task(void){
	static u8 i;
	i++;
	if(i >= 100)i=0;
	if(i<5)LEDR = 1;
	else LEDR = 0;
	
	
}




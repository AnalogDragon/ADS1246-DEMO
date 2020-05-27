#include "sys.h"



u8 init_all(void){
	Hard_Init();
	delay_init();
	OLED_Init();
	ADS_Init();
	return 0;
}


u32 count = 0;

int main(void){	
	init_all();      //≥ı ºªØ
	
	Display_Asc_String('4',1,0, "Analog Dragon");
	Draw_Rectangle(0xff, 2, 1,254, 17, 62);
	Display_Asc_String('4',20,35, "Value:");
	
	while(1){
		count++;
		if(SysTime.SysTimeFLG1ms){
			SysTime.SysTimeFLG1ms = 0;
		}
		
		if(SysTime.SysTimeFLG10ms){
			LED_Task();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			LEDB = 1;
			Display_Asc_Num('4',215,0,SysTime.SysTimeCNT100ms,5);
			if(SysTime.SysTimeCNT100ms%3 == 0)
				Display_Asc_Num('5',80,28,count,10);
			LEDB = 0;
			SysTime.SysTimeFLG100ms = 0;
		}
		
		if(SysTime.SysTimeFLG1s){
			if(SysTime.SysTimeCNT1s%10 == 0)RLA = RLB = 1;
			else RLA = RLB = 0;
			SysTime.SysTimeFLG1s = 0;
		}
		
		if(SysTime.SysTimeFLG1min){
			SysTime.SysTimeFLG1min = 0;
		}
		
		if(SysTime.SysTimeFLG1h){
			SysTime.SysTimeFLG1h = 0;
		}
		
	}
}


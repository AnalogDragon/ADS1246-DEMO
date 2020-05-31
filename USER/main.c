#include "sys.h"



u8 init_all(void){
	Hard_Init();
	delay_init();
	OLED_Init();
	SysState.err.bit.ADC_ERR = ADS_Init() != 0;

	return SysState.err.all;
}


int main(void){	
	init_all();      //初始化
	
	Display_Asc_String('4',1,0, "Analog Dragon");
	Draw_Rectangle(0xff, 2, 1,254, 17, 62);
	Display_Asc_String('4',20,35, "Temp:");
	Display_Asc_fNum('4',80,35,ADC_Data.Temp,5);
	Display_GB2312_String('2',135,35, "°");
	Display_Asc_String('4',140,35, "C");
	
	//start loop
	while(1){
		
		if(SysTime.SysTimeFLG1ms){			
			LEDB = 1;
			ADC_Task();
			LEDB = 0;
			SysTime.SysTimeFLG1ms = 0;
		}
		
		if(SysTime.SysTimeFLG10ms){
			IWDG_ReloadCounter();
			LED_Task();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			SysTime.SysTimeFLG100ms = 0;
		}
		
		if(SysTime.SysTimeFLG1s){
			if(SysTime.SysTimeCNT1s > 4)break;
			SysTime.SysTimeFLG1s = 0;
		}
		
		if(SysTime.SysTimeFLG1min){
			SysTime.SysTimeFLG1min = 0;
		}
		
		if(SysTime.SysTimeFLG1h){
			SysTime.SysTimeFLG1h = 0;
		}
		
	}
	
	//loop end
	
	
	
	Fill_Block(0x00,1,62,19, 60);
	Display_Asc_String('4',20,23, " ADC Value:");
	Display_Asc_String('4',20,43, "Input Volt:");
	
	
	//main loop
	while(1){
		
		if(SysTime.SysTimeFLG1ms){
			LEDB = 1;
			ADC_Task();
			LEDB = 0;
			SysTime.SysTimeFLG1ms = 0;
		}
		
		if(SysTime.SysTimeFLG10ms){
 			IWDG_ReloadCounter();
			LED_Task();
			SysTime.SysTimeFLG10ms = 0;
		}
		
		if(SysTime.SysTimeFLG100ms){
			Display_Asc_Num('4',120,23,ADC_Data.Filter,10);
			Display_Asc_fNum('4',120,43,(long double)ADC_Data.Filter*6.0/0xFFFFFF,10);	//显示输入电压
			SysTime.SysTimeFLG100ms = 0;
		}
		
		if(SysTime.SysTimeFLG1s){
			Display_Asc_Num('4',215,0,ADC_Data.FQ,3);	//显示ADC采样率
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


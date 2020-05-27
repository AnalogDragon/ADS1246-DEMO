#ifndef _VarDefine_
#define _VarDefine_
#include "sys.h"



struct SysTime_REG
{
	volatile u16 SysTimeCNT1ms;
	volatile u16 SysTimeCNT10ms;
	volatile u16 SysTimeCNT100ms;
	volatile u16 SysTimeCNT1s;
	volatile u16 SysTimeCNT1min;
	volatile u16 SysTimeCNT1h;
		
	volatile u8 SysTimeFLG1ms;
	volatile u8 SysTimeFLG10ms;
	volatile u8 SysTimeFLG100ms;
	volatile u8 SysTimeFLG1s;
	volatile u8 SysTimeFLG1min;
	volatile u8 SysTimeFLG1h;
};  


/*----------------------------*/


struct KeyFlag_BITS{
	u16 IOActu:1;	//实时
	u16 IOLast:1;	//上次状态，用于滤波
	u16 IOFilt:1;	//滤波后的值
	u16 KeyOut:2;	//单次触发位  		//1:按下首次 2:短按弹起 3:长按弹起
	u16 KeyErr:1;	//按键长按忽略
};

union KeyFlag_REG{
	u16											all;
	struct KeyFlag_BITS 		bit;
};

struct AKeyState_REG{
	union KeyFlag_REG Flag;
	u16 PressTime;
	u16 HoldTime;
};

struct KeyState_REG{
	struct AKeyState_REG Key1;
	struct AKeyState_REG Key2;
	struct AKeyState_REG Key3;
	struct AKeyState_REG Key4;
};

/*----------------------------*/

extern struct SysTime_REG SysTime;
extern struct KeyState_REG KeyState;



#endif

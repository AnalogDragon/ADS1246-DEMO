#ifndef __ADS_H
#define __ADS_H	 
#include "sys.h"


/*ADS1246命令码列表*/
#define ADC_CMD_WAKEUP      0x00            //退出睡眠模式
#define ADC_CMD_SLEEP       0x02            //进入睡眠模式
#define ADC_CMD_SYNC        0x04            //同步ADC转换
#define ADC_CMD_RESET       0x06            //芯片复位
#define ADC_CMD_NOP     		0xFF            //空操作
#define ADC_CMD_RDATA       0x12            //单次读取数据
#define ADC_CMD_RDATAC      0x14            //连续读取数据
#define ADC_CMD_SDATAC      0x16            //停止读取数据
#define ADC_CMD_RREG        0x20            //读寄存器
#define ADC_CMD_WREG        0x40            //写寄存器
#define ADC_CMD_SYSOCAL     0x60            //系统偏移校准
#define ADC_CMD_SYSGCAL     0x61            //系统增益校准
#define ADC_CMD_SELFOCAL    0x62            //系统自校准
#define ADC_CMD_RESTRICTED  0xF1            //  

//ADS1246寄存器列表
#define ADC_REG_BCS         0x00  
#define ADC_REG_VBIAS       0x01  
#define ADC_REG_MUX         0x02  
#define ADC_REG_SYS0        0x03  
#define ADC_REG_OFC0        0x04  
#define ADC_REG_OFC1        0x05  
#define ADC_REG_OFC2        0x06  
#define ADC_REG_FSC0        0x07  
#define ADC_REG_FSC1        0x08  
#define ADC_REG_FSC2        0x09  
#define ADC_REG_ID          0x0A  
  
//ADS1246支持的增益列表 
#define ADC_GAIN_1          0x00  
#define ADC_GAIN_2          0x10  
#define ADC_GAIN_4          0x20  
#define ADC_GAIN_8          0x30  
#define ADC_GAIN_16         0x40  
#define ADC_GAIN_32         0x50  
#define ADC_GAIN_64         0x60  
#define ADC_GAIN_128        0x70  
  
//ADS1246支持的转换速率  
#define ADC_SPS_5           0x00  
#define ADC_SPS_10          0x01  
#define ADC_SPS_20          0x02  
#define ADC_SPS_40          0x03  
#define ADC_SPS_80          0x04  
#define ADC_SPS_160         0x05  
#define ADC_SPS_320         0x06  
#define ADC_SPS_640         0x07  
#define ADC_SPS_1000        0x08  
#define ADC_SPS_2000        0x09  
  
// //ADS1246转换模式  
// #define ADC_MODE_SINGLECOV      0x00
// #define ADC_MODE_CONTINUOUS     0x01

//MUX 
#define MUX_NORMAL					0x00
#define MUX_SHUT_MIDD     	0x01
#define MUX_FULL_SCALE   		0x02
#define MUX_TEMPERT     		0x03  

/*------------------------------------*/


#define ADC_BUFF_LEN 64

struct ADC_DTAT_REG{
	u32 	Buff[ADC_BUFF_LEN];	//缓存
	u8 		Point;
	u32 	BuffSUM;
	u32 	Filter;		//滤波后值
	
	u32		TempValue;	//温度度数		(仅上电时读取一次)
	float	Temp;				//温度值
	
	u16 	FQ;
	u16 	count;
	
	u8 		ID;
};


extern struct ADC_DTAT_REG ADC_Data;

u8 ADS_Init(void);
u32 ADS1246_Read(void);

void ADC_Task(void);




#endif

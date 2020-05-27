#ifndef __HARDINIT_H
#define __HARDINIT_H	 
#include "sys.h"

//按键
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取按键2 
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取按键3
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取按键4

//继电器输出
#define RLA  	PBout(14)
#define RLB		PBout(15)

//LED输出
#define LEDR	PBout(6)
#define LEDB	PBout(7)

//SPI
#define MISO  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define MOSI  PAout(7)
#define SCLK  PAout(5)

//ADC
#define A_CS			PBout(5)
#define A_IRQ  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define A_RST			PAout(4)
#define A_START		PAout(8)

//OLED显示组
#define D_CS			PBout(9)
#define D_EN			PBout(0)
#define D_DC			PBout(1)
#define D_RST			PAout(2)

//字库
#define C_CS			PBout(8)



void Hard_Init(void);







#endif

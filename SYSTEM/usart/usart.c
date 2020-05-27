#include "usart.h"



struct USART1_REG USART1_DATA;
  

void printtime(void){		//调试用
	
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1,0xff);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	
}






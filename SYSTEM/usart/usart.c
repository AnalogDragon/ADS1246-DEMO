#include "usart.h"



struct USART1_REG USART1_DATA;
  

void printtime(void){		//������
	
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1,0xff);//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	
}






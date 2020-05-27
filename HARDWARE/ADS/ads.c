#include "ads.h"







//SPI发送并接受1byte
u8 ADS_SEND_READ(u8 wdata){
	u8 i,rdata = 0;
	for(i=0;i<8;i++){
		SCLK = 1;
		rdata <<= 1;
		if(wdata&0x80)MOSI = 1;
		else MOSI = 0;
		SCLK = 0;  //字库时钟拉低 
		if(MISO)
			rdata |= 1;
		wdata <<= 1;
	}
	return rdata;
}


void ADS_WrCMD(void){
	A_CS = 0;
	A_CS = 1;
}

void ADS_Init(void){
	A_CS = 0;
	A_CS = 1;
}













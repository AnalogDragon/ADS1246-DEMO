#include "ads.h"
#include "delay.h"
#include <math.h>


#define ADC_DATA_LEN 16
u8 ADS_BUFF[ADC_DATA_LEN] = {0};

struct ADC_DTAT_REG ADC_Data;

u8 ADS_WR_Byte(u8 wdata){
	u8 i,rdata = 0;
	for(i=0;i<8;i++){
		SCLK = 1;
		MOSI = (wdata&0x80) != 0;
		rdata <<= 1;
		SCLK = 0;
		wdata <<= 1;
		if(MISO)rdata |= 1;
	}
	return rdata;
}


void ADS1246_ReadReg(u8 RegAddr,u8* Buffer,u8 Length)  
{  
		u8 Cmd[2],i;
		Cmd[0]=ADC_CMD_RREG|RegAddr;  
		Cmd[1]=Length-1; 
		ADS_WR_Byte(Cmd[0]);
		ADS_WR_Byte(Cmd[1]);
		for(i=0;i<Length;i++){
			*Buffer = ADS_WR_Byte(ADC_CMD_NOP);
			Buffer++;
		}
}

void ADS1246_WriteReg(u8 RegAddr,u8* Buffer,u8 Length)  
{  
    u8 Cmd[2],i;  
    Cmd[0]=ADC_CMD_WREG|RegAddr;  
    Cmd[1]=Length-1;  
    ADS_WR_Byte(Cmd[0]);
		ADS_WR_Byte(Cmd[1]);
		for(i=0;i<Length;i++){
			ADS_WR_Byte(*Buffer);
			Buffer++;
		}   
}  

//2s超时等待
u8 ADS1246_WaitBusy(void){
    u16 i = 0;
    i=0;  
    while(A_IRQ){
				IWDG_ReloadCounter();
        delay_ms(10);  
        i++; 
				if(i>200)
					return 1;  
    }  
    return 0;  
}  


void ChangeMUX(u8 temp){
		ADS_BUFF[0] = temp&0x3;
		ADS1246_WriteReg(ADC_REG_MUX,ADS_BUFF,1);
}

float GetTemp(u32 Temp){
	long long temp = Temp;
	float temp0;
	temp *= 50000000;
	temp /= 0xFFFFFF;
	temp -= 26180000;
	temp0 = (long double)temp / 4050;
	temp0 += 25.0;
	return temp0;
}	

u8 ADS_Init(void){
	u8 temp = 0;
	
	//硬件复位
	A_RST=0;
	A_START=0;
	delay_ms(10);
	A_RST=1;
	delay_ms(10);
	A_START=1;
	delay_ms(30);
	
	//配置增益
	A_CS=0;
	ADS_WR_Byte(ADC_CMD_RESET);   
	delay_ms(10);
	ADS_BUFF[0] = ADC_GAIN_1|ADC_SPS_160;
	ADS1246_WriteReg(ADC_REG_SYS0,ADS_BUFF,1);
	
	//较准ADC
	ChangeMUX(MUX_NORMAL);
	ADS_WR_Byte(ADC_CMD_SELFOCAL);    	 		//自校准
	if(ADS1246_WaitBusy())goto endl;

	ChangeMUX(MUX_SHUT_MIDD);						//输入断开，连接到1/2 VCC
	ADS_WR_Byte(ADC_CMD_SYSOCAL);       		// 系统偏移校准
	if(ADS1246_WaitBusy())goto endl;

	ChangeMUX(MUX_FULL_SCALE); 					//连接到VREF（full-scale）
	ADS_WR_Byte(ADC_CMD_SYSGCAL);      		// 系统增益校准
	if(ADS1246_WaitBusy())goto endl;
	
	//读取ID
	ADS1246_ReadReg(ADC_REG_ID,ADS_BUFF,1);
	ADC_Data.ID = ADS_BUFF[0]&0xF0;
	
	//切换通道至温度通道
	ChangeMUX(MUX_TEMPERT);
	ADS_WR_Byte(ADC_CMD_RDATA);						//连续读取
	
	//这里获取启动时温度，2s没获取到则跳出
	ADC_Data.TempValue = 0;
	while((ADC_Data.TempValue == 0) && (SysTime.SysTimeCNT1s <= 2)){
		for(ADC_Data.Point = 0; ADC_Data.Point < 8; ADC_Data.Point++){
			if(ADS1246_WaitBusy())temp = 2;
			ADC_Data.TempValue += ADS1246_Read();
			if(ADC_Data.TempValue == 0)break;
		}
	}
	ADS_WR_Byte(ADC_CMD_SDATAC);
	A_CS=1;
	
	ADC_Data.TempValue >>= 3;
	if(SysTime.SysTimeCNT1s > 2 || ADC_Data.TempValue == 0)temp = 1;
	else ADC_Data.Temp = GetTemp(ADC_Data.TempValue);
	
	delay_us(50);
	
	//切换通道到标准输入
	A_CS=0;
	ChangeMUX(MUX_NORMAL);
	ADS_WR_Byte(ADC_CMD_RDATAC);						//连续读取
	A_CS=1;//释放SPI
	
	return temp;
	
endl:	//ADC初始化失败
	A_CS=1;
	return 1;
}


u32 ADS1246_Read(void){
	u8 i;
	u32 temp = 0;

	A_CS = 0;

//	ADS_WR_Byte(ADC_CMD_RDATA);

	for(i=0;i<3;i++)
		temp = (temp<<8) | ADS_WR_Byte(ADC_CMD_NOP);
	
	if(temp&0x800000){
		temp &= 0x7FFFFF;
	}
	else{
		temp += 0x800000;
	}
	
	A_CS = 1;
	
	return temp;  
}  


void ADC_Task(void){
	static u16 TimeBuf = 0;
	if(A_IRQ)return;
	
	ADC_Data.Point++;
	if(ADC_Data.Point >= ADC_BUFF_LEN)
		ADC_Data.Point = 0;
	
	ADC_Data.BuffSUM -=ADC_Data.Buff[ADC_Data.Point];
	ADC_Data.Buff[ADC_Data.Point] = ADS1246_Read();
	ADC_Data.BuffSUM +=ADC_Data.Buff[ADC_Data.Point];
	ADC_Data.Filter = ADC_Data.BuffSUM/ADC_BUFF_LEN;
	
	ADC_Data.count++;
	if(TimeBuf != SysTime.SysTimeCNT1s){
		TimeBuf = SysTime.SysTimeCNT1s;
		ADC_Data.FQ = ADC_Data.count;
		ADC_Data.count = 0;
	}
}



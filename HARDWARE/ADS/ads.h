#ifndef __ADS_H
#define __ADS_H	 
#include "sys.h"


/*ADS1246�������б�*/
#define ADC_CMD_WAKEUP      0x00            //�˳�˯��ģʽ
#define ADC_CMD_SLEEP       0x02            //����˯��ģʽ
#define ADC_CMD_SYNC        0x04            //ͬ��ADCת��
#define ADC_CMD_RESET       0x06            //оƬ��λ
#define ADC_CMD_NOP     		0xFF            //�ղ���
#define ADC_CMD_RDATA       0x12            //���ζ�ȡ����
#define ADC_CMD_RDATAC      0x14            //������ȡ����
#define ADC_CMD_SDATAC      0x16            //ֹͣ��ȡ����
#define ADC_CMD_RREG        0x20            //���Ĵ���
#define ADC_CMD_WREG        0x40            //д�Ĵ���
#define ADC_CMD_SYSOCAL     0x60            //ϵͳƫ��У׼
#define ADC_CMD_SYSGCAL     0x61            //ϵͳ����У׼
#define ADC_CMD_SELFOCAL    0x62            //ϵͳ��У׼
#define ADC_CMD_RESTRICTED  0xF1            //  

//ADS1246�Ĵ����б�
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
  
//ADS1246֧�ֵ������б� 
#define ADC_GAIN_1          0x00  
#define ADC_GAIN_2          0x10  
#define ADC_GAIN_4          0x20  
#define ADC_GAIN_8          0x30  
#define ADC_GAIN_16         0x40  
#define ADC_GAIN_32         0x50  
#define ADC_GAIN_64         0x60  
#define ADC_GAIN_128        0x70  
  
//ADS1246֧�ֵ�ת������  
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
  
// //ADS1246ת��ģʽ  
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
	u32 	Buff[ADC_BUFF_LEN];	//����
	u8 		Point;
	u32 	BuffSUM;
	u32 	Filter;		//�˲���ֵ
	
	u32		TempValue;	//�¶ȶ���		(���ϵ�ʱ��ȡһ��)
	float	Temp;				//�¶�ֵ
	
	u16 	FQ;
	u16 	count;
	
	u8 		ID;
};


extern struct ADC_DTAT_REG ADC_Data;

u8 ADS_Init(void);
u32 ADS1246_Read(void);

void ADC_Task(void);




#endif

#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"

#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column		0x3F			// 256/4-1
#define Max_Row		0x3F			// 64-1
#define	Brightness	0xFF 
#define	Shift		0x1C


     
#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý
//OLED¿ØÖÆÓÃº¯Êý
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Init(void);
void Set_Column_Address(u8 a, u8 b);
void Set_Row_Address(u8 a, u8 b);
void Set_Write_RAM(void);
void Set_Read_RAM(void);
void Fill_RAM(u8 Data);
void Draw_Rectangle(u8 Data, u8 a, u8 b, u8 c, u8 d, u8 e);
void Fill_Block(u8 Data, u8 a, u8 b, u8 c, u8 d);
void Grayscale(void);
void Set_Remap_Format(u8 d);
void Show_Pattern(u8 *Data_Pointer, u8 a, u8 b, u8 c, u8 d);
void Con_4_byte(u8 DATA);
void Set_Partial_Display(u8 a, u8 b, u8 c);
void Set_Display_Offset(u8 d);
void Set_Start_Line(u8 d);
void Set_Master_Current(u8 d);
void Vertical_Scroll(u8 a, u8 b, u8 c);
void Set_Gray_Scale_Table(void);
void Set_Linear_Gray_Scale_Table(void); 

//Ó²×Ö¿âº¯Êý
void ZK_command( u8  data );
u8  get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(u8  AddrHigh,u8  AddrMid,u8  AddrLow,u8  *pBuff,u8  DataLen );
void Display_GB2312_String(u8 zk_num,u16 x,u16 y, u8  text[]);	// ÏÔÊ¾ºº×Ö×Ö·û´®
void Display_GB2312( u8 zk_num,u16 x,u16 y );								 //	ÏÔÊ¾ºº×Ö
void Display_GBasc_String(u8 zk_num,u16 x,u16 y , u8  text[]);	  // ÏÔÊ¾GB2312 ascii ×Ö·û´®
void Display_GBasc( u8 zk_num,u16 x,u16 y  );								  //  ÏÔÊ¾GB2312 ascii 
void Display_Asc_String(u8 zk_num,u16 x,u16 y , u8  text[]);		  //   ÏÔÊ¾ASCII ×Ö·û´®
void Display_Asc( u8 zk_num,u16 x,u16 y);									  //   ÏÔÊ¾ASCII
void Display_Asc_Num(u8 zk_num,u16 x, u16 y, u32 Num, u8 Len);

#endif


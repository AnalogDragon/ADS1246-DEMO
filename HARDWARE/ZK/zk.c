#include "oled.h" 
#include "string.h" 
#include "math.h" 


u8 FontBuf[128];//字库缓存	


/****送指令到字库IC***/
void ZK_command(u8 data){
	u8 i;
	for(i=0;i<8;i++){
		MOSI = (data&0x80) != 0;
		SCLK = 0;  //字库时钟拉低 
		data = data<<1;
		SCLK = 1;  //字库时钟拉高
	}
}

/****从字库IC中取汉字或字符数据（1个字节）***/	  
u8  get_data_from_ROM(void)
{
	u8  i;
	u8  ret_data=0; 		//返回数据初始化
	
	SCLK = 1;						//字库时钟拉高
	for(i=0;i<8;i++){
	 	SCLK = 0;  //字库时钟拉低  
		ret_data <<= 1;
		
		if(MISO)
			ret_data |= 1;
		else
			ret_data |= 0;

		SCLK = 1; //字库时钟拉高
	}
	return ret_data; //返回读出的一个字节
}

/*从相关地址（AddrHigh：地址高字节,AddrMid：地址中字节,AddrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址*/
/*连续读取*/
void get_n_bytes_data_from_ROM(u8  AddrHigh,u8  AddrMid,u8  AddrLow,u8  *pBuff,u8  DataLen )
{
	u8  i;
	C_CS = 0; //字库片选
	SCLK = 0;  //字库时钟拉低
	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);//写地址高字节
	ZK_command(AddrMid);//写地址中字节
	ZK_command(AddrLow);//写地址低字节
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();//读一个字节数据
	C_CS = 1;//取消字库片选
}



 //*************************************************************************
 //****	Display_GB2312_String（）汉字字符串显示程序	***//
 //zk_num定义：汉字库的标示符 1:GB2312_12*12,2:GB2312_15*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x：列坐标 y：行坐标
 //text[]：要显示的汉字
 //**************************************************************************
void Display_GB2312_String(u8 zk_num,u16  x,u16  y, u8  text[])
{
   
	u8  i= 0;
	u8  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	
	u32  FontAddr=0; //字地址
	u32  BaseAdd=0; //字库基地址	
	u8  n= 0, d=0;// 不同点阵字库的计算变量
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x0; n=24 ;d=12;  break;
		case '2':  BaseAdd=0x2C9D0; n=32;d=16;  break;
		case '3':  BaseAdd=0x68190; n=72; d=24; break;
		case '4':  BaseAdd=0xEDF00; n=128;d=32; break;
	}
		
	while((text[i]>0x00))
	{
	    if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//溢出换行

		if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
		{						
			/*国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址*/
		 	FontAddr = (text[i]- 0xA1)*94; 
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/ 
					
			x=x+d; //下一个字坐标
			i+=2;  //下个字符
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		{						
			/*国标简体（GB2312） 字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址*/
			 
			FontAddr = (text[i]- 0xB0)*94; 
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[ ]"*/
			Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/
	 		x=x+d; //下一个字坐标
			i+=2;  //下个字符

		}
		
	}
	
}

//***************************************************************
//  显示12*12 15*16 24*24 32*32 GB2312点阵汉字 2015-11晶奥测试通过
//   zk_num：汉字 库中的类型  GB_1212 1616 2424 3232 
//   x: Start Column  开始列 
//   y: Start Row   开始行 0~63 
//*************************************************************** 
void Display_GB2312( u8 zk_num,u16 x, u16 y )
{
   	u8 j ,x0,y0,n;
	u16 x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=3;y0=11; n=24;  break;
		case '2':  x0=3;y0=15; n=32;  break;
		case '3':  x0=5;y0=23; n=72;  break;
		case '4':  x0=7;y0=31; n=128;  break;
	}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
	}

}





  //*************************************************************************
 //****	Display_GBasc_String（）国标扩展字符字符串显示程序	***//
 //zk_num定义： 字库的标示符 1: 6X12 点国标扩展字符,2:8X16 点国标扩展字符,3:8X16 点国标扩展特殊字符, 4:12X24 点国标扩展字符, 5:16X32 点国标扩展字符
 //x：列坐标 y：行坐标
 //text[]：要显示的字符
 //**************************************************************************
void Display_GBasc_String(u8 zk_num,u16 x, u16 y, u8  text[])
{
   
	u8  i= 0;
	unsigned short  FontCode  ; // 字符内码
	u8  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	u32  FontAddr=0; //字地址
	u32  BaseAdd=0; //字库基地址	
	u8  n= 0, d=0;// 不同点阵字库的计算变量
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DBE0C; n=12 ;d=8;  break;	 //	   6X12 点国标扩展字符
		case '2':  BaseAdd=0x1DD790; n=16;d=8;  break;	 //8X16 点国标扩展字符
		case '3':  BaseAdd=0x1F2880; n=16;d=8;  break;	 //8X16 点国标扩展特殊字符
	    case '4':  BaseAdd=0x1DFF30; n=48; d=12; break;	 //12X24 点国标扩展字符
	    case '5':  BaseAdd=0x1E5A90; n=64;d=16; break;	 // 16X32 点国标扩展字符
			}
		
	while((text[i]>0x00))
	{	
	     if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//溢出换行

 	     FontCode =	0xAA00|(text[i]|0x80);	//ascii最高位为1
		if((FontCode>=0xAAA1) &&(FontCode<=0xAAFE))
		{						
		    FontAddr = 	FontCode-0xAAA1;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*显示一个字符到OLED上 */
			i+=1;
			x+=d;//下一个字坐标
		 
		}
		else
		
		  if((FontCode>=0xABA1) &&(FontCode<=0xABC0))	 
		{						
		    FontAddr = 	(FontCode-0xABA1)+95;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*显示一个字符到OLED上 */
			i+=1;
			x+=d;//下一个字坐标
		 
		}
		 else
			
			if((FontCode>=0xACA1) &&(FontCode<=0xACDF))		// 8X16 点国标扩展特殊字符
		{						
		    FontAddr = 	FontCode-0xACA1;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位/
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位/
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"/
			Display_GBasc(zk_num,x,y);//显示一个字符到OLED上 /
			i+=1;
			x+=d;//下一个字坐标
		 
		}


	}
	
}
 
//***************************************************************
//  显示6*12 8*16 12*24 16*32 GB2312点阵ASCII 2015-11晶奥测试通过
//zk_num定义： 字库的标示符 1: 6X12 点国标扩展字符,2:8X16 点国标扩展字符,3:8X16 点国标扩展特殊字符, 4:12X24 点国标扩展字符, 5:16X32 点国标扩展字符
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
//*************************************************************** 
void Display_GBasc( u8 zk_num,u16 x, u16 y )
{
    //extern u8  FontBuf[128];//字库缓存	
	u8 j ,x0,y0,n;
	u16  x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=1;y0=11; n=12;  break;
		case '2':  x0=1;y0=15; n=16;  break;
		case '3':  x0=1;y0=15; n=16;  break;
		case '4':  x0=3;y0=23; n=48;  break;
		case '5':  x0=3;y0=31; n=64;  break;
	}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
	}

}
 







 //*************************************************************************
 //****	Display_Asc_String  ASCII字符串显示程序	***//
 //zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
 //x：列坐标 y：行坐标
 //text[]：要显示的ASCII
 //**************************************************************************
void Display_Asc_String(u8 zk_num,u16 x, u16 y, u8  text[])
{
   
	u8  i= 0;
	u8  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	u32  FontAddr=0; //字地址
	u32  BaseAdd=0; //字库基地址	
  u8  n= 0, d=0,c=0;// 不同点阵字库的计算变量
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DDF80; n=8 ;d=8 ;c=8;  break;	 //	  5x7 ASCII
		case '2':  BaseAdd=0x1DE280; n=8;d=8; c=8;  break;	 //   7x8 ASCII
		case '3':  BaseAdd=0x1DBE00; n=12; d=8;c=12; break;	 //  6x12 ASCII
		case '4':  BaseAdd=0x1DD780; n=16;d=8;c=16; break;	 //  8x16 ASCII
		case '5':  BaseAdd=0x1DFF00; n=48;d=12; c=24;break;	 //  12x24 ASCII
		case '6':  BaseAdd=0x1E5A50; n=64;d=16;c=32; break;	 //  16x32 ASCII
	}
		
	while((text[i]>0x00)){
	 if(x>(256-d)){
		 y=y+c; 
		 x=0; 
		}//溢出换行
		
		FontAddr = 	text[i]-0x20;
		if((text[i] < 0x20) ||(text[i] > 0x7E)){
			FontAddr = 	'?'-0x20;
		}
		FontAddr = (u32)((FontAddr*n)+BaseAdd);

		AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
		AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
		AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
		get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/

		Display_Asc(zk_num,x,y);/*显示一个ascii到OLED上 */
		i+=1;
		x+=d;//下一个字坐标
	}
	
}


//***************************************************************
//  显示 ASCII 2015-11晶奥测试通过
//zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  开始列  
//   y: Start Row   开始行 0~63 
//*************************************************************** 
void Display_Asc( u8 zk_num,u16 x, u16 y )
{
    //extern u8  FontBuf[128];//字库缓存	
  u8 j ,x0,y0,n,k;
	u16 x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=1;y0=7; n=8; k=0; break;   //	  5x7 ASCII
		case '2':  x0=1;y0=7; n=8; k=1; break;   //	  7x8 ASCII
		case '3':  x0=1;y0=11; n=12;k=1;  break;   //  6x12 ASCII
		case '4':  x0=1;y0=15; n=16; k=1;  break;   //  8x16 ASCII
		case '5':  x0=3;y0=23; n=48; k=1;  break;   //  12x24 ASCII
		case '6':  x0=3;y0=31; n=64; k=1;  break;   //  16x32 ASCII
	}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	
	if(k==0)
	{
		for(j=0;j<n-1;j++){
			 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
		}
		OLED_WR_Byte(0x00,OLED_DATA);	    //写前2列
		OLED_WR_Byte(0x00,OLED_DATA);               //写后2列	  共计4列
	}
	else
	{
	 	for(j=0;j<n;j++)
		{
			Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
		}
	}
}


//***************************************************************
//  显示 数字 皮蛋龙测试
//	 zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  开始列  
//   y: Start Row   开始行 0~63 
//	 Num: 显示数字
//	 Len: 显示位数（取余）
//*************************************************************** 
void Display_Asc_Num(u8 zk_num,u16 x, u16 y, u32 Num, u8 Len){
	static u8 Table[11] = {0};
	u8 i = 0;

	if(Len > 10)Len = 10;
	if(Len == 0)return;

 	memset(Table,0,sizeof(Table));

	switch(Len){
		case 0xA:Table[i++] = (u8)(((Num / 1000000000) % 10) + '0');
		case 0x9:Table[i++] = (u8)(((Num / 100000000) % 10) + '0');
		case 0x8:Table[i++] = (u8)(((Num / 10000000) % 10) + '0');
		case 0x7:Table[i++] = (u8)(((Num / 1000000) % 10) + '0');
		case 0x6:Table[i++] = (u8)(((Num / 100000) % 10) + '0');
		case 0x5:Table[i++] = (u8)(((Num / 10000) % 10) + '0');
		case 0x4:Table[i++] = (u8)(((Num / 1000) % 10) + '0');
		case 0x3:Table[i++] = (u8)(((Num / 100) % 10) + '0');
		case 0x2:Table[i++] = (u8)(((Num / 10) % 10) + '0');
		case 0x1:Table[i++] = (u8)(((Num / 1) % 10) + '0');break;
		default:return;
	}
	
	Display_Asc_String(zk_num,x,y,Table);
}

//***************************************************************
//  显示 浮点数字 皮蛋龙测试
//	 zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  开始列  
//   y: Start Row   开始行 0~63 
//	 Num: 显示数字
//	 Len: 显示位数（指有效位）
//*************************************************************** 
void Display_Asc_fNum(u8 zk_num,u16 x, u16 y,long double Num, u8 Len){
	static u8 Table[20] = {0};
	long long in=Num;
	long long buf=0;
	long long de=0;
	u8 L1=0;
	u8 L2=0;
	u8 i = 0;
	buf=in;

 	memset(Table,0,sizeof(Table));

	if(Len > 16) Len = 16;

	while(buf!=0){                  //整数的长度
    buf = buf / 10;
    L1++;
  }
	
	if(L1>Len){                    //显示不到小数
		buf=(long long)5*pow(10,L1-Len+2);
		if(in<0){
			in=in-buf;  //精度后一位加5
		}else{
			in=in+buf;
		}
		L1=0;
		buf=in;
	  while(buf!=0){                  //整数的长度
      buf = buf / 10;
      L1++;
    }
		goto disp1;
	}
	else if(L1==Len){                    //显示整数位
		if(in<0){
			in+=(Num-in-0.5);
		}else{
			in+=(Num-in+0.5);
		}
		L1=0;
		buf=in;
	  while(buf!=0){                  //整数的长度
      buf = buf / 10;
      L1++;
    }
		if(L1>Len)goto disp1;
		goto disp2;
	}
	else if(L1<Len){                        //显示小数
		if(Num<0)Num-=5*pow(10,-(Len-L1));
		else Num+=5*pow(10,-(Len-L1));
		in=Num;
		de=(Num-in)*pow(10,Len-L1-2);
		if(de<0)de=-de;
		L1=0;
		buf=in;
	  while(buf!=0){                  //整数的长度
      buf = buf / 10;
      L1++;
    }
		if(L1==0)L1=1;
		L2=Len-L1-1;
		if(L1==Len)goto disp2;
		goto disp3;
	}
	
disp1:
	{                            //显示不到小数
	  if(in<0){                            //判断符号
		  Table[i++] = '-';
		  in=-in;
	  }else{
		  Table[i++] = ' ';
	  }
		
		if(Len<5 || L1>16){                   //判断是否超出或者显示不完
			Table[i++] = 'E';
			goto end;
		}
		
		Table[i++] = '0' + (in/(long long)pow(10,L1-1)%10);
		Table[i++] = '.';
		buf=L1;
	  for(Len=Len-2;Len>2;Len--){
			Table[i++] = '0' + (in/(long long)pow(10,buf-2)%10);
			buf--;
	  }
		Table[i++] = 'e';
		if(L1-1 <= 9)
			Table[i++] = '0'+L1-1;
		else 
			Table[i++] = 'A'+L1-10;
		goto end;
	}
	
disp2:
	{
	  if(in<0){                            //判断符号
		  Table[i++] = '-';
		  in=-in;
	  }else{
		  Table[i++] = ' ';
	  }
	  for(Len=Len;Len>0;Len--){
		  Table[i++] = '0' + (in/(long long)pow(10,Len-1)%10);
	  }
		goto end;
  }
	
disp3:
	{
	  if(in<0){                            //判断符号
		  Table[i++] = '-';
		  in=-in;
	  }else{
		  Table[i++] = ' ';
	  }
	  for(L1=L1;L1>0;L1--){
		  Table[i++] = '0' + (in/(long long)pow(10,L1-1)%10);
	  }
		Table[i++] = '.';
	  for(L2=L2;L2>0;L2--){
		  Table[i++] = '0' + (de/(long long)pow(10,L2-1)%10);
	  }
		goto end;
	}

	end:
	if(i>17)
		SysState.err.bit.PROG_ERR = 1;
	Display_Asc_String(zk_num,x,y,Table);
	
}




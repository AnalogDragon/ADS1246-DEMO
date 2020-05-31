#include "oled.h" 
#include "string.h" 
#include "math.h" 


u8 FontBuf[128];//�ֿ⻺��	


/****��ָ��ֿ�IC***/
void ZK_command(u8 data){
	u8 i;
	for(i=0;i<8;i++){
		MOSI = (data&0x80) != 0;
		SCLK = 0;  //�ֿ�ʱ������ 
		data = data<<1;
		SCLK = 1;  //�ֿ�ʱ������
	}
}

/****���ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/	  
u8  get_data_from_ROM(void)
{
	u8  i;
	u8  ret_data=0; 		//�������ݳ�ʼ��
	
	SCLK = 1;						//�ֿ�ʱ������
	for(i=0;i<8;i++){
	 	SCLK = 0;  //�ֿ�ʱ������  
		ret_data <<= 1;
		
		if(MISO)
			ret_data |= 1;
		else
			ret_data |= 0;

		SCLK = 1; //�ֿ�ʱ������
	}
	return ret_data; //���ض�����һ���ֽ�
}

/*����ص�ַ��AddrHigh����ַ���ֽ�,AddrMid����ַ���ֽ�,AddrLow����ַ���ֽڣ�����������DataLen���ֽڵ����ݵ� pBuff�ĵ�ַ*/
/*������ȡ*/
void get_n_bytes_data_from_ROM(u8  AddrHigh,u8  AddrMid,u8  AddrLow,u8  *pBuff,u8  DataLen )
{
	u8  i;
	C_CS = 0; //�ֿ�Ƭѡ
	SCLK = 0;  //�ֿ�ʱ������
	ZK_command(0x03);//дָ��
	ZK_command(AddrHigh);//д��ַ���ֽ�
	ZK_command(AddrMid);//д��ַ���ֽ�
	ZK_command(AddrLow);//д��ַ���ֽ�
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();//��һ���ֽ�����
	C_CS = 1;//ȡ���ֿ�Ƭѡ
}



 //*************************************************************************
 //****	Display_GB2312_String���������ַ�����ʾ����	***//
 //zk_num���壺���ֿ�ı�ʾ�� 1:GB2312_12*12,2:GB2312_15*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x�������� y��������
 //text[]��Ҫ��ʾ�ĺ���
 //**************************************************************************
void Display_GB2312_String(u8 zk_num,u16  x,u16  y, u8  text[])
{
   
	u8  i= 0;
	u8  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ
	
	u32  FontAddr=0; //�ֵ�ַ
	u32  BaseAdd=0; //�ֿ����ַ	
	u8  n= 0, d=0;// ��ͬ�����ֿ�ļ������
	
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
			 	 }//�������

		if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
		{						
			/*������壨GB2312�������� �ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; ������ȡ��ַ*/
		 	FontAddr = (text[i]- 0xA1)*94; 
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GB2312(zk_num,x,y);//��ʾһ�����ֵ�OLED��/ 
					
			x=x+d; //��һ��������
			i+=2;  //�¸��ַ�
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		{						
			/*������壨GB2312�� �ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; ������ȡ��ַ*/
			 
			FontAddr = (text[i]- 0xB0)*94; 
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[ ]"*/
			Display_GB2312(zk_num,x,y);//��ʾһ�����ֵ�OLED��/
	 		x=x+d; //��һ��������
			i+=2;  //�¸��ַ�

		}
		
	}
	
}

//***************************************************************
//  ��ʾ12*12 15*16 24*24 32*32 GB2312������ 2015-11���²���ͨ��
//   zk_num������ ���е�����  GB_1212 1616 2424 3232 
//   x: Start Column  ��ʼ�� 
//   y: Start Row   ��ʼ�� 0~63 
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
	}

}





  //*************************************************************************
 //****	Display_GBasc_String����������չ�ַ��ַ�����ʾ����	***//
 //zk_num���壺 �ֿ�ı�ʾ�� 1: 6X12 �������չ�ַ�,2:8X16 �������չ�ַ�,3:8X16 �������չ�����ַ�, 4:12X24 �������չ�ַ�, 5:16X32 �������չ�ַ�
 //x�������� y��������
 //text[]��Ҫ��ʾ���ַ�
 //**************************************************************************
void Display_GBasc_String(u8 zk_num,u16 x, u16 y, u8  text[])
{
   
	u8  i= 0;
	unsigned short  FontCode  ; // �ַ�����
	u8  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	u32  FontAddr=0; //�ֵ�ַ
	u32  BaseAdd=0; //�ֿ����ַ	
	u8  n= 0, d=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DBE0C; n=12 ;d=8;  break;	 //	   6X12 �������չ�ַ�
		case '2':  BaseAdd=0x1DD790; n=16;d=8;  break;	 //8X16 �������չ�ַ�
		case '3':  BaseAdd=0x1F2880; n=16;d=8;  break;	 //8X16 �������չ�����ַ�
	    case '4':  BaseAdd=0x1DFF30; n=48; d=12; break;	 //12X24 �������չ�ַ�
	    case '5':  BaseAdd=0x1E5A90; n=64;d=16; break;	 // 16X32 �������չ�ַ�
			}
		
	while((text[i]>0x00))
	{	
	     if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//�������

 	     FontCode =	0xAA00|(text[i]|0x80);	//ascii���λΪ1
		if((FontCode>=0xAAA1) &&(FontCode<=0xAAFE))
		{						
		    FontAddr = 	FontCode-0xAAA1;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*��ʾһ���ַ���OLED�� */
			i+=1;
			x+=d;//��һ��������
		 
		}
		else
		
		  if((FontCode>=0xABA1) &&(FontCode<=0xABC0))	 
		{						
		    FontAddr = 	(FontCode-0xABA1)+95;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*��ʾһ���ַ���OLED�� */
			i+=1;
			x+=d;//��һ��������
		 
		}
		 else
			
			if((FontCode>=0xACA1) &&(FontCode<=0xACDF))		// 8X16 �������չ�����ַ�
		{						
		    FontAddr = 	FontCode-0xACA1;
			FontAddr = (u32)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  //��ַ�ĸ�8λ,��24λ/
			AddrMid = (FontAddr&0xff00)>>8;      //��ַ����8λ,��24λ/
			AddrLow = FontAddr&0xff;	     //��ַ�ĵ�8λ,��24λ/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//ȡһ�����ֵ����ݣ��浽"FontBuf[]"/
			Display_GBasc(zk_num,x,y);//��ʾһ���ַ���OLED�� /
			i+=1;
			x+=d;//��һ��������
		 
		}


	}
	
}
 
//***************************************************************
//  ��ʾ6*12 8*16 12*24 16*32 GB2312����ASCII 2015-11���²���ͨ��
//zk_num���壺 �ֿ�ı�ʾ�� 1: 6X12 �������չ�ַ�,2:8X16 �������չ�ַ�,3:8X16 �������չ�����ַ�, 4:12X24 �������չ�ַ�, 5:16X32 �������չ�ַ�
//   x: Start Column  ��ʼ�� ��Χ 0~��256-16��
//   y: Start Row   ��ʼ�� 0~63 
//*************************************************************** 
void Display_GBasc( u8 zk_num,u16 x, u16 y )
{
    //extern u8  FontBuf[128];//�ֿ⻺��	
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
	}

}
 







 //*************************************************************************
 //****	Display_Asc_String  ASCII�ַ�����ʾ����	***//
 //zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
 //x�������� y��������
 //text[]��Ҫ��ʾ��ASCII
 //**************************************************************************
void Display_Asc_String(u8 zk_num,u16 x, u16 y, u8  text[])
{
   
	u8  i= 0;
	u8  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	u32  FontAddr=0; //�ֵ�ַ
	u32  BaseAdd=0; //�ֿ����ַ	
  u8  n= 0, d=0,c=0;// ��ͬ�����ֿ�ļ������
	
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
		}//�������
		
		FontAddr = 	text[i]-0x20;
		if((text[i] < 0x20) ||(text[i] > 0x7E)){
			FontAddr = 	'?'-0x20;
		}
		FontAddr = (u32)((FontAddr*n)+BaseAdd);

		AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
		AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
		AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
		get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/

		Display_Asc(zk_num,x,y);/*��ʾһ��ascii��OLED�� */
		i+=1;
		x+=d;//��һ��������
	}
	
}


//***************************************************************
//  ��ʾ ASCII 2015-11���²���ͨ��
//zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  ��ʼ��  
//   y: Start Row   ��ʼ�� 0~63 
//*************************************************************** 
void Display_Asc( u8 zk_num,u16 x, u16 y )
{
    //extern u8  FontBuf[128];//�ֿ⻺��	
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	
	if(k==0)
	{
		for(j=0;j<n-1;j++){
			 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
		}
		OLED_WR_Byte(0x00,OLED_DATA);	    //дǰ2��
		OLED_WR_Byte(0x00,OLED_DATA);               //д��2��	  ����4��
	}
	else
	{
	 	for(j=0;j<n;j++)
		{
			Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
		}
	}
}


//***************************************************************
//  ��ʾ ���� Ƥ��������
//	 zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  ��ʼ��  
//   y: Start Row   ��ʼ�� 0~63 
//	 Num: ��ʾ����
//	 Len: ��ʾλ����ȡ�ࣩ
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
//  ��ʾ �������� Ƥ��������
//	 zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  ��ʼ��  
//   y: Start Row   ��ʼ�� 0~63 
//	 Num: ��ʾ����
//	 Len: ��ʾλ����ָ��Чλ��
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

	while(buf!=0){                  //�����ĳ���
    buf = buf / 10;
    L1++;
  }
	
	if(L1>Len){                    //��ʾ����С��
		buf=(long long)5*pow(10,L1-Len+2);
		if(in<0){
			in=in-buf;  //���Ⱥ�һλ��5
		}else{
			in=in+buf;
		}
		L1=0;
		buf=in;
	  while(buf!=0){                  //�����ĳ���
      buf = buf / 10;
      L1++;
    }
		goto disp1;
	}
	else if(L1==Len){                    //��ʾ����λ
		if(in<0){
			in+=(Num-in-0.5);
		}else{
			in+=(Num-in+0.5);
		}
		L1=0;
		buf=in;
	  while(buf!=0){                  //�����ĳ���
      buf = buf / 10;
      L1++;
    }
		if(L1>Len)goto disp1;
		goto disp2;
	}
	else if(L1<Len){                        //��ʾС��
		if(Num<0)Num-=5*pow(10,-(Len-L1));
		else Num+=5*pow(10,-(Len-L1));
		in=Num;
		de=(Num-in)*pow(10,Len-L1-2);
		if(de<0)de=-de;
		L1=0;
		buf=in;
	  while(buf!=0){                  //�����ĳ���
      buf = buf / 10;
      L1++;
    }
		if(L1==0)L1=1;
		L2=Len-L1-1;
		if(L1==Len)goto disp2;
		goto disp3;
	}
	
disp1:
	{                            //��ʾ����С��
	  if(in<0){                            //�жϷ���
		  Table[i++] = '-';
		  in=-in;
	  }else{
		  Table[i++] = ' ';
	  }
		
		if(Len<5 || L1>16){                   //�ж��Ƿ񳬳�������ʾ����
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
	  if(in<0){                            //�жϷ���
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
	  if(in<0){                            //�жϷ���
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




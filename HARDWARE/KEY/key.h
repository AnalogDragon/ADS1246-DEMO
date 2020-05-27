#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"



struct AKeyState_REG;


u8 KEY_Scan(u8);  	//°´¼üÉ¨Ãèº¯Êý
void GetAKey(struct AKeyState_REG *KeyBuf,u8 IOState);
void GetKey(void);


#endif


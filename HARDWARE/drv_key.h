#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__


#include "sys.h"

#define K1 PAin(0)
#define K2 PAin(1)

void key_init(void);//IO��ʼ��
u8 key_scan(u8);    //����ɨ�躯��

#endif


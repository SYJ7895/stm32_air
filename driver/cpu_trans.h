#ifndef __CPU_TRANS__H__
#define __CPU_TRANS__H__

#include "ctrlbsp.h"

// 模拟htonl函数，本机字节序转网络字节序
uint32_t mcu_htonl(uint32_t h);

// 模拟ntohl函数，网络字节序转本机字节序
uint32_t mcu_ntohl(uint32_t n);

uint16_t mcu_htons(uint16_t h);
uint16_t mcu_ntohs(uint16_t n);



#endif


#ifndef __CPU_TRANS__H__
#define __CPU_TRANS__H__

#include "ctrlbsp.h"

// ģ��htonl�����������ֽ���ת�����ֽ���
uint32_t mcu_htonl(uint32_t h);

// ģ��ntohl�����������ֽ���ת�����ֽ���
uint32_t mcu_ntohl(uint32_t n);

uint16_t mcu_htons(uint16_t h);
uint16_t mcu_ntohs(uint16_t n);



#endif


#include "cpu_trans.h"

// 短整型大小端互换
#define BigLittleSwap16(A)  ((((unsigned  short)(A) & 0xff00) >> 8) | \
                            (((unsigned  short)(A) & 0x00ff) << 8))
 // 长整型大小端互换
#define BigLittleSwap32(A)  ((((unsigned  int)(A) & 0xff000000) >> 24) | \
                            (((unsigned  int)(A) & 0x00ff0000) >> 8) | \
                            (((unsigned  int)(A) & 0x0000ff00) << 8) | \
                            (((unsigned  int)(A) & 0x000000ff) << 24))

#define IS_BIG_ENDIAN    checkCPUendian()
// 本机大端返回1，小端返回0
unsigned int  checkCPUendian()
{
       union{

              unsigned long int i;

              unsigned char s[4];

       }c;

       c.i = 0x12345678;
      return (0x12 == c.s[0]);

}
// 模拟htonl函数，本机字节序转网络字节序

uint32_t mcu_htonl(uint32_t h)

{
       // 若本机为大端，与网络字节序同，直接返回
       // 若本机为小端，转换成大端再返回
       return IS_BIG_ENDIAN ? h : BigLittleSwap32(h);

}

// 模拟ntohl函数，网络字节序转本机字节序

uint32_t mcu_ntohl(uint32_t n)

{
       // 若本机为大端，与网络字节序同，直接返回
       // 若本机为小端，网络数据转换成小端再返回
       return IS_BIG_ENDIAN ? n : BigLittleSwap32(n);

}

uint16_t mcu_htons(uint16_t h)

{
       // 若本机为大端，与网络字节序同，直接返回
       // 若本机为小端，转换成大端再返回
       return IS_BIG_ENDIAN ? h : BigLittleSwap16(h);

}

// 模拟ntohl函数，网络字节序转本机字节序

uint16_t mcu_ntohs(uint16_t n)

{
       // 若本机为大端，与网络字节序同，直接返回
       // 若本机为小端，网络数据转换成小端再返回
       return IS_BIG_ENDIAN ? n : BigLittleSwap16(n);

}



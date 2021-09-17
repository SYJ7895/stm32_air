#include "cpu_trans.h"

// �����ʹ�С�˻���
#define BigLittleSwap16(A)  ((((unsigned  short)(A) & 0xff00) >> 8) | \
                            (((unsigned  short)(A) & 0x00ff) << 8))
 // �����ʹ�С�˻���
#define BigLittleSwap32(A)  ((((unsigned  int)(A) & 0xff000000) >> 24) | \
                            (((unsigned  int)(A) & 0x00ff0000) >> 8) | \
                            (((unsigned  int)(A) & 0x0000ff00) << 8) | \
                            (((unsigned  int)(A) & 0x000000ff) << 24))

#define IS_BIG_ENDIAN    checkCPUendian()
// ������˷���1��С�˷���0
unsigned int  checkCPUendian()
{
       union{

              unsigned long int i;

              unsigned char s[4];

       }c;

       c.i = 0x12345678;
      return (0x12 == c.s[0]);

}
// ģ��htonl�����������ֽ���ת�����ֽ���

uint32_t mcu_htonl(uint32_t h)

{
       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
       // ������ΪС�ˣ�ת���ɴ���ٷ���
       return IS_BIG_ENDIAN ? h : BigLittleSwap32(h);

}

// ģ��ntohl�����������ֽ���ת�����ֽ���

uint32_t mcu_ntohl(uint32_t n)

{
       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
       // ������ΪС�ˣ���������ת����С���ٷ���
       return IS_BIG_ENDIAN ? n : BigLittleSwap32(n);

}

uint16_t mcu_htons(uint16_t h)

{
       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
       // ������ΪС�ˣ�ת���ɴ���ٷ���
       return IS_BIG_ENDIAN ? h : BigLittleSwap16(h);

}

// ģ��ntohl�����������ֽ���ת�����ֽ���

uint16_t mcu_ntohs(uint16_t n)

{
       // ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
       // ������ΪС�ˣ���������ת����С���ٷ���
       return IS_BIG_ENDIAN ? n : BigLittleSwap16(n);

}



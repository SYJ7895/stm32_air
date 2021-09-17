#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "flash.h"

/*
* 计算数据每一部分的数据的crc值
*/
#define CRC_POLY 			0xedb88320 	//crc种子

unsigned int mcu_crc32(unsigned int crc ,unsigned char * data, unsigned int data_len)
{
    unsigned int data_idx = 0;
    unsigned int  crc32 = crc ^ 0xffffffff, byte_tmp = 0;
    unsigned char bit_loop = 0;
    for( data_idx = 0; data_idx < data_len; data_idx ++){
        byte_tmp = ( unsigned int )( data[ data_idx ] ) & 0x000000ff;
        for( bit_loop = 0; bit_loop < 8; bit_loop++ ){

                if((( crc32 ^ byte_tmp ) & 1 ) != 0 ){
                       crc32 = (( crc32 >> 1 ) & 0x7fffffff ) ^ CRC_POLY;
                }
                else{
                       crc32 = (( crc32 >> 1 ) & 0x7fffffff );
                }
                byte_tmp = ( byte_tmp >> 1 ) & 0x7fffffff;
        }
    }

    return crc32 ^ 0xffffffff;
}



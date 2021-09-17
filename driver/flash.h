#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x_flash.h"
#include "stm32f10x.h"
#include "ctrlbsp.h"
#include <stdarg.h>
#include <stdio.h>  
#include <string.h> 
#include "partition.h"


uint32_t flash_read(uint32_t addr,uint8_t * data,uint32_t len);
uint32_t flash_write(uint32_t addr,uint8_t * data,uint32_t len);


#endif

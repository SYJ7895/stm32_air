#ifndef __PARTITION_H
#define __PARTITION_H

#define STM32_FLASH_SIZE 64
#define STM32_FLASH_BASE 0X08000000

#define BOOT_SIZE        (8*1024)  //8K
#define SOFT_SIZE        (25*1024) //25K
#define CFG_SIZE         (1*1024)  //1K
#define IMAGE_HEAD_SIZE   64

#define SIZE   0x0000F000
#define APINFOLEN    40
#define IPLEN        20

//分区列表
#define BOOT_ADDR              STM32_FLASH_BASE
#define SOFT_ADDR              (BOOT_ADDR+BOOT_SIZE)
#define SOFT_TMP_ADDR          (SOFT_ADDR+SOFT_SIZE)
#define CONFIG_ADDR            (SOFT_TMP_ADDR+SOFT_SIZE)
#define UPGRADE_INFO_ADDR       CONFIG_ADDR
#define ISCONFIG               (UPGRADE_INFO_ADDR+2)
#define CONFIG_DEFAULT_AP      (ISCONFIG+2)
#define SERVER_IP              (CONFIG_DEFAULT_AP+APINFOLEN)





#endif



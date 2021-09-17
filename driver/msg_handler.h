#ifndef __MSG_HANDLER_H
#define __MSG_HANDLER_H

#include "Esp8266.h"
#include "ctrlbsp.h"
#include <stdarg.h>
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

void mac2sn(char* data,ST_ESP_STATE* uc);
//挂载
void Mcu2Iot_mount_req(char* SN,uint16_t sof_ver,uint16_t cfg_ver,uint8_t result);
void Iot2Mcu_mount_rsp(char* data,ST_ESP_STATE* uc);

//心跳
void Mcu2Iot_hb_req(char* SN);
void Iot2Mcu_hb_rsp(ST_ESP_STATE* uc);

//升级
void Mcu2Iot_upgrade_note(ST_ESP_STATE* uc,char * data);
void Mcu2Iot_upgrade_req(ST_ESP_STATE* uc,uint8_t seqnum);
void Iot2Mcu_upgrade_rsp(ST_ESP_STATE* uc,char *data,uint16_t len,uint8_t seqnum);
void Mcu2Iot_upgrade_result(ST_ESP_STATE* uc,ENUM_UPGRADE_RESULT result);

//连接AP 或者切换AP
void Iot2Mcu_join_ap_req(char * data,ST_ESP_STATE* uc);
void Mcu2Iot_join_ap_rsp(char * data,ST_ESP_STATE* uc);
void Mcu2Iot_change_ap_rsp(ST_ESP_STATE* uc);

//信息上报
void Mcu2Iot_report_req(ST_ESP_STATE* uc);
void Iot2Mcu_report_rsp(void);

//远程设置
void Iot2Mcu_set_req(char*data,ST_ESP_STATE* uc);
void Mcu2Iot_set_rsp(char*data,int len);

//远程获取
void Iot2Mcu_get_req(char*data,ST_ESP_STATE* uc);
void Mcu2Iot_get_rsp(char*data,ST_ESP_STATE* uc);












#endif

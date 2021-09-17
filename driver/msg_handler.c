#include "msg_handler.h"
#include "data_struct.h"
#include "ctrlbsp.h"
#include "Sact-misc.h"
#include "crc8.h"
#include "cpu_trans.h"
#include "upgrade.h"
#include "flash.h"




void mac2sn(char* data,ST_ESP_STATE* uc)
{
   char * tmp = NULL;
   char SN[13] = {0};
   int i = 0;
   memset(SN,0,sizeof(SN));
   memset(uc->SN,0,SN_LEN);
   tmp = strstr(data,"\"");

   if(!tmp){
      return ;
   }
   tmp = tmp+1;
   while(*tmp != '\"' ){
	   if(*tmp != ':' && i < (sizeof(SN)-1)){
         SN[i] = *tmp;
		 i++;
	   }
	   tmp = tmp +1;
   }
   
   if(i != sizeof(SN)-1){
     DEBUG("get sn error\r\n");
	 uc->State.getsn = 0;
   }else{
     sprintf((char*)uc->SN,"AIR%s",SN);
     DEBUG("SN:%s\r\n",uc->SN);
	 uc->State.getsn = 1;
   }
   

}

void Mcu2Iot_mount_req(char* SN,uint16_t sof_ver,uint16_t cfg_ver,uint8_t result)
{
   char cmd[100];
   ST_PACK_INFO pack;
   ST_MOUNT_REQ info;
   uint16_t len = sizeof(ST_MOUNT_REQ)+sizeof(ST_PACK_INFO);

   pack.event_id = IOT_MOUNT_REQ;
   pack.len = mcu_htons(len);
   pack.seq = 0;

   memset(cmd,0,sizeof(cmd));
 
   memset(info.SN,0,SN_LEN);
   memcpy(info.SN,SN,SN_LEN);
  
   info.soft_ver = mcu_htons(sof_ver);
   info.cfg_ver = mcu_htons(cfg_ver);
   info.upgradeR = result;

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
   DEBUG("mount req crc:%d,len:%d\r\n",cmd[0],len);
   ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );

}

void Iot2Mcu_mount_rsp(char* data,ST_ESP_STATE* uc)
{
   ST_MOUNT_RSP *info = (ST_MOUNT_RSP *)data;
    
   uint32_t tick = mcu_ntohl(info->timetick);

   info->timetick = mcu_ntohl(info->timetick);

   settick(info->timetick);
  
   uc->hb_prb = info->hb_per;

   DEBUG("MOUNT RSP tick %d,hb_per %d\r\n",tick,info->hb_per);
   //挂载成功，状态设置成网络正常
   ESP8266_Netstate_set(NET_NORMAL);
   
   if(uc->upgradeRest != NOUPGRADE){
       uc->upgradeRest = NOUPGRADE;
	   flash_write(UPGRADE_INFO_ADDR,(uint8_t*)&uc->upgradeRest,sizeof(uc->upgradeRest));
   }
   
}

void Mcu2Iot_hb_req(char* SN)
{
   char cmd[100];
   ST_PACK_INFO pack;
   ST_HB_REQ info;
   uint16_t len = sizeof(ST_HB_REQ)+sizeof(ST_PACK_INFO);

   pack.event_id = IOT_HB_REQ;
   pack.len = mcu_htons(len);
   pack.seq = 0;

   memset(cmd,0,sizeof(cmd));
   memset(info.SN,0,SN_LEN);
   memcpy(info.SN,SN,SN_LEN);

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
   DEBUG("send hb,len:%d\r\n",len);
   ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );


}


void Iot2Mcu_hb_rsp(ST_ESP_STATE* uc)
{
  
   if(uc->NetState == NET_NORMAL){
      uc->hb = 0; //挂载成功后才记录心跳
   }
//   sprintf(cmd,"%d#%d",info.evt_id,info.result);
     DEBUG("recv hb rsp\r\n");
}


void Mcu2Iot_upgrade_note(ST_ESP_STATE* uc,char * data)
{
   ST_UPGRADE_NOTICE * ver = (ST_UPGRADE_NOTICE*)data;
   uc->State.link = 0;
   uc->State.Penetrate = 0;
   uc->State.isUpgrade = 1;
   uc->State.upgrading = 0;
   uc->reqVer = mcu_ntohs(ver->softVer);
   ESP8266_ExitUnvarnishSend (); //退出透传模式
   delay_ms(1000);
   ESP8266_CLOSE_Link();
   ESP8266_Netstate_set(NET_LOSE);
}


void Mcu2Iot_upgrade_req(ST_ESP_STATE* uc,uint8_t seqnum)
{
   char cmd[100];
   ST_PACK_INFO pack;
   ST_UPGRADE_REQ info;
   uint16_t len = sizeof(ST_UPGRADE_REQ)+sizeof(ST_PACK_INFO);

   pack.event_id = IOT_UPGRADE_REQ;
   pack.len = mcu_htons(len);
   pack.seq = 0;

   memset(cmd,0,sizeof(cmd));
   memset(info.SN,0,SN_LEN);
   memcpy(info.SN,uc->SN,SN_LEN);
   info.seqnum = seqnum;
   info.softVer = mcu_htons(uc->reqVer);

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
   DEBUG("send upgrade req,len:%d\r\n",len);
   ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );


}


void Iot2Mcu_upgrade_rsp(ST_ESP_STATE* uc,char *data,uint16_t len,uint8_t seqnum)
{
	static uint8_t numSeq = 0;
	uint8_t ret = 0;
	uint8_t upinfo;
	DEBUG("seqnum %d\r\n",seqnum);
	if(seqnum != numSeq ){
		if(seqnum != SEQEND){
		  if(seqnum == SEQNOEXIST){
		     upinfo = FILENOEXIST;
		     ret = flash_write(UPGRADE_INFO_ADDR,(uint8_t*)&upinfo,sizeof(upinfo));
		     delay_ms(3000);
		     systemRest();
			 return;
		  }else{
	         Mcu2Iot_upgrade_req(uc,numSeq);
		     return;
		  }
		}else {
		  numSeq = SEQEND;
		}
	}
	
    ret = upgrade((void*)data,len,seqnum);
	if(DATA_OK != ret){
	   
	}else{
	  numSeq ++; 
	  if(seqnum == SEQEND){
	     Mcu2Iot_upgrade_result(uc,DOWNLOADOK);
		 delay_ms(3000);
		 systemRest();
		 return;
	  }
	}
	DEBUG("request net seq:%d\r\n",numSeq);
	Mcu2Iot_upgrade_req(uc,numSeq);

}


void Mcu2Iot_upgrade_result(ST_ESP_STATE* uc,ENUM_UPGRADE_RESULT result)
{
   char cmd[100];
   ST_PACK_INFO pack;
   ST_UPGRADE_INFO info;
   uint16_t len = sizeof(ST_UPGRADE_INFO)+sizeof(ST_PACK_INFO);

   pack.event_id = IOT_UPGRADE_RESULT;
   pack.len = mcu_htons(len);
   pack.seq = 0;

   memset(cmd,0,sizeof(cmd));
   memset(info.SN,0,SN_LEN);
   memcpy(info.SN,uc->SN,SN_LEN);
   info.result = result;

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
   DEBUG("send upgrade req,len:%d\r\n",len);
   ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );

}


void Iot2Mcu_join_ap_req(char * data,ST_ESP_STATE* uc)
{
    ST_AP_INFO* ap_info = (ST_AP_INFO*)data;

	memset(uc->ssid_tmp,0,AP_SSID_LEN);
	memset(uc->pass_tmp,0,AP_PASS_LEN);

	if(uc->mode == STA){
		uc->State.change_ap = 1;
		uc->change_ap_result = 0;
		uc->State.join_ap = 0;
		uc->State.link = 0;
		uc->State.Penetrate = 0;
		ESP8266_ExitUnvarnishSend (); //退出透传模式
		ESP8266_CLOSE_Link();
	    ESP8266_Netstate_set(NET_LOSE);
		delay_ms(2000);
	}

	DEBUG("ap_info->ssid :%s ,ap_info->pass:%s\r\n",ap_info->ssid,ap_info->pass);
    memcpy(uc->ssid_tmp,ap_info->ssid,AP_SSID_LEN);
	memcpy(uc->pass_tmp,ap_info->pass,AP_SSID_LEN);
	ESP8266_JoinAP(ap_info->ssid,ap_info->pass);
}

void Mcu2Iot_join_ap_rsp(char * data,ST_ESP_STATE* uc)
{
    char cmd[100];
    ST_PACK_INFO pack;
    ST_JOIN_AP_RSP info ;
	
    uint16_t len = strlen(data);
	uint16_t packlen = sizeof(ST_JOIN_AP_RSP)+sizeof(ST_PACK_INFO);

	pack.event_id = JOIN_AP_RSP;
    pack.len = mcu_htons(packlen);
    pack.seq = 0;
	
	if(strstr(data,"+CWJAP:")){
		info.result = 1;
		info.reason = *(data+strlen("+CWJAP:")) - '0';
	}else{
		info.result = 0; //加入AP成功
		info.reason = 0;
	}

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, packlen-1);
   DEBUG("join ap rsp,len:%d\r\n",packlen);
   ESP8266_SendString ( DISABLE, cmd, packlen, Multiple_ID_0 );
   
}

void Mcu2Iot_change_ap_rsp(ST_ESP_STATE* uc)
{
	char cmd[100];
    ST_PACK_INFO pack;
    ST_JOIN_AP_RSP info ;

	memcpy(info.SN,uc->SN,SN_LEN);

	uint16_t len = sizeof(ST_JOIN_AP_RSP)+sizeof(ST_PACK_INFO);
	

	pack.event_id = CHANGE_AP_RSP;
    pack.len = mcu_htons(len);
    pack.seq = 0;
	

	if(uc->change_ap_result != 0){
		info.result = 1; //改变AP失败
		info.reason = uc->change_ap_result;
	}else{
		info.result = 0; //改变AP成功
		info.reason = 0;
	}

   memcpy(cmd,(char*)&pack,sizeof(pack));
   memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
   cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
   DEBUG("change ap rsp,len:%d\r\n",len);
   ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );
}




void Mcu2Iot_report_req(ST_ESP_STATE* uc)
{
    char cmd[100];
    ST_PACK_INFO pack;
    ST_REPORT_INFO info ;
	uint16_t len = sizeof(ST_REPORT_INFO)+sizeof(ST_PACK_INFO);

	pack.event_id = IOT_REPORT_REQ;
    pack.len = mcu_htons(len);
    pack.seq = 0;
	
	memcpy(info.SN,uc->SN,SN_LEN);
	info.real_tmp = get_real_tmp();
	info.set_tmp = get_set_tmp();
	info.mode = get_mode();
	info.spped = get_speed();
	info.on_off = get_onoff();

	memcpy(cmd,(char*)&pack,sizeof(pack));
	memcpy(cmd+sizeof(pack),(char*)&info,sizeof(info));
	cmd[0] = cal_crc_table((uint8_t*)cmd+1,len-1);
	DEBUG("report,len:%d\r\n",len);
	ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );

}

void Iot2Mcu_report_rsp(void)
{
    DEBUG("recv report rsp\r\n");

}

void Iot2Mcu_set_req(char*data,ST_ESP_STATE* uc)
{
    char cmd[100];
    ST_PACK_INFO pack;
	ST_SET_INFO* info = (ST_SET_INFO*)data;
	ST_SET_RSP rsp_info;
	uint16_t len = sizeof(ST_SET_RSP)+sizeof(ST_PACK_INFO);
	

	pack.event_id = IOT_SET_RSP;
    pack.len = mcu_htons(len);
    pack.seq = 0;
	
	memcpy(rsp_info.SN,uc->SN,SN_LEN);
	rsp_info.mode = info->mode;
	rsp_info.result = 1;

	switch(info->mode){
		case SET_TMPE:
			rsp_info.result = SACT_Temp_Set(info->vaule);
			break;
	    case SET_MODE:
			rsp_info.result = SACT_Mode_set(info->vaule);
			break;
		case SET_SPEED:
			rsp_info.result = SACT_Speed_set(info->vaule);
			break;
		case SET_SWITCH:
			rsp_info.result = SACT_ONOFF_set(info->vaule);
			break;
		default:
		    break;
	}

	memcpy(cmd,(char*)&pack,sizeof(pack));
	memcpy(cmd+sizeof(pack),(char*)&rsp_info,sizeof(rsp_info));
	cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
	
	Mcu2Iot_set_rsp(cmd,len);

}

void Mcu2Iot_set_rsp(char*data,int len)
{
    DEBUG("set rsp ,len:%d\r\n",len);
	ESP8266_SendString ( ENABLE, data,len, Single_ID_0 );

}

void Mcu2Iot_get_rsp(char*data,ST_ESP_STATE* uc)
{
    ST_GET_REQ* req = (ST_GET_REQ*)data;
    ST_GET_RSP rsp;
	char cmd[100];
    ST_PACK_INFO pack;
	uint16_t len = sizeof(ST_GET_RSP)+sizeof(ST_PACK_INFO);
	

	pack.event_id = IOT_GET_RSP;
    pack.len = mcu_htons(len);
    pack.seq = 0;

	rsp.mode = req->mode;
	memcpy(rsp.SN,uc->SN,SN_LEN);

	switch(req->mode){
		//获取当前实际温度
		case GET_REAL_TMPE:
			 rsp.value = get_real_tmp();
             break;
	    //获取设置温度
		case GET_SET_TMPE:
			 rsp.value = get_set_tmp();
             break;
		//获取模式
        case GET_MODE:
			 rsp.value = get_mode();
			 break;
		//获取风速
		case GET_SPEED: 
			 rsp.value = get_speed();
			 break;
        //获取开关状态
	    case GET_SWITCH:
			 rsp.value = get_onoff();
			 break;
		default:
		     break;
	}

	memcpy(cmd,(char*)&pack,sizeof(pack));
	memcpy(cmd+sizeof(pack),(char*)&rsp,sizeof(rsp));
	cmd[0] = cal_crc_table((uint8_t*)cmd+1, len-1);
    DEBUG("get rsp ,len:%d\r\n",len);
	ESP8266_SendString ( ENABLE, cmd,len, Single_ID_0 );
}











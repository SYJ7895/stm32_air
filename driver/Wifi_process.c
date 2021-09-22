#include "Esp8266.h"
#include "ctrlbsp.h"
#include "msg_handler.h"
#include "crc8.h"
#include "cpu_trans.h"
#include "defcfg.h"

extern ST_Esp8266_Fram_Record Wifi_Fram ;
extern ST_ESP_STATE UC_state ;
extern uint8_t gIsConf;
extern uint8_t gNetState;
uint32_t tickrep = 0;
uint8_t configap = 0;
extern uint8_t curjoin;

#define NET_TIMEOUT 10  //网络包超时10s
#define TRY_TIMES  3

void net_check()
{
	static uint8_t retry_t = 0;
	static uint32_t tick_send = 0;
	static uint32_t tick_hb = 0;

	switch (UC_state.NetState){
		case NET_LOSE:
			gNetState = 0;
			break;
	    case NET_MOUNT:
			 gNetState = 1;
			 Mcu2Iot_mount_req((char*)UC_state.SN,SOFT_VERSION,CFG_VERSION,UC_state.upgradeRest);
			 tick_send = gettick();
			 ESP8266_Netstate_set(NET_MOUNTING);
			break;
		case NET_MOUNTING:
			if((tick_send + NET_TIMEOUT) < gettick() ){
               Mcu2Iot_mount_req((char*)UC_state.SN,SOFT_VERSION,CFG_VERSION,UC_state.upgradeRest);
			   tick_send = gettick();
			   retry_t ++;
			}

			if(retry_t > TRY_TIMES){
				ESP8266_ExitUnvarnishSend (); //退出透传模式
				UC_state.State.link = 0; //检测连接
				UC_state.State.Penetrate = 0;
				ESP8266_CLOSE_Link();
				ESP8266_Netstate_set(NET_LOSE);
                retry_t = 0;				
			}
			break;
		case NET_NORMAL:
			break;
		case UPGRADE:
			if(!UC_state.State.upgrading){
				if((tick_send + NET_TIMEOUT) < gettick() ){
				   Mcu2Iot_upgrade_req(&UC_state,0);
					tick_send = gettick();
					retry_t ++;
				}

				if(retry_t > TRY_TIMES){
					retry_t = 0;
					ESP8266_ExitUnvarnishSend (); //退出透传模式
					UC_state.State.link = 0; //检测连接
					UC_state.State.Penetrate = 0;
					ESP8266_CLOSE_Link();
					ESP8266_Netstate_set(NET_LOSE);	
				}
		   }
			break;
		default:
		    break;

	}
	
	if((tickrep != 0) && (UC_state.NetState == NET_NORMAL)){
		if((gettick() - tickrep) > 2){
			Mcu2Iot_report_req(&UC_state);
            tickrep = 0;			
		}
	
	}else{
		tickrep = 0;	
	}
	
	if((gettick() - tick_hb) >= UC_state.hb_prb ){
		tick_hb = gettick();
		
		if(UC_state.NetState == NET_NORMAL){
			Mcu2Iot_hb_req((char*)UC_state.SN);
			Mcu2Iot_report_req(&UC_state);
			UC_state.hb ++;
			//连续HB_LOSE分钟没有收到心跳
			if(UC_state.hb > HB_LOSE ){
				ESP8266_ExitUnvarnishSend (); //退出透传模式
				UC_state.State.link = 0; //检测连接
				UC_state.State.Penetrate = 0;
				UC_state.hb = 0;
				ESP8266_CLOSE_Link();
				ESP8266_Netstate_set(NET_LOSE);	
				DEBUG("no recv hb\r\n");
			}
		}
	}
}


void IPD_process(char * data,uint16_t alllen,ST_ESP_STATE* uc)
{

	ST_PACK_INFO* pack = (ST_PACK_INFO*)data;
	uint8_t crc8 = 0;
	uint16_t len = 0;
	char * tmp = NULL;
	char * tmp2 = NULL;
	
	while( alllen > 0 ){
		
		len = mcu_ntohs(pack->len);
		DEBUG("allen %d ,len %d\r\n",alllen,len);
        alllen = alllen - len;
		if(data){
			crc8 = cal_crc_table((uint8_t*)data+1, len-1);
			if(crc8 != pack->crc8){
			   DEBUG("pack crc error :%d,crc8:%d ,event_id: %d\r\n",pack->crc8,crc8,pack->event_id);
			   if(strstr(data,"busy") || strstr(data,"ERROR")){
				   ESP8266_ExitUnvarnishSend (); //退出透传模式
				   uc->State.link = 0;
				   uc->State.Penetrate = 0;
				   ESP8266_CLOSE_Link();
				   ESP8266_Netstate_set(NET_LOSE);
				}else if(strstr(data,"ssid:")){
					ESP8266_ExitUnvarnishSend (); //退出透传模式
					memset(UC_state.ssid_tmp,0,AP_SSID_LEN);
					memset(UC_state.pass_tmp,0,AP_PASS_LEN);
					tmp = strstr(data,"ssid:") + strlen("ssid:");
					tmp2 = strstr(tmp,"\r\n");
					memcpy(UC_state.ssid_tmp,tmp,(tmp2-tmp));
					tmp = strstr(tmp2,"password:") + strlen("password:");
					tmp2 = strstr(tmp,"\r\n");
					memcpy(UC_state.pass_tmp,tmp,(tmp2-tmp));
					memset(uc->server_ip,0,IPLEN);
				    tmp = strstr(data,"IP:")+strlen("IP:");
				    tmp2 = strstr(tmp,"\r\n");
				    memcpy(uc->server_ip,tmp,(tmp2-tmp));
					DEBUG("get IP %s\r\n",uc->server_ip);
					SaveServerIP(uc);
					
                    ESP8266_Rst();
					delay_ms(1000);
//                    ESP8266_Cmd ("AT+CWQAP");
//					delay_ms(100);					
                    ESP8266_JoinAP(UC_state.ssid_tmp,UC_state.pass_tmp);
                    configap = 1;//开始验证WIFI信息	
					curjoin = 2;
                    uc->State.join_ap = 0;					
				}else if(strstr(data,"reboot")){
					systemRest();
                   				
				}
                
			   return ;
			}
			DEBUG("event_id: %d\r\n",pack->event_id);

			switch(pack->event_id){
				case IOT_REMOUNT:
					Mcu2Iot_mount_req((char*)UC_state.SN,SOFT_VERSION,CFG_VERSION,UC_state.upgradeRest);
					break;
				case IOT_MOUNT_RSP:
					Iot2Mcu_mount_rsp(data+sizeof(ST_PACK_INFO),uc);
					break;
			
				case IOT_HB_RSP:
					Iot2Mcu_hb_rsp(uc);
					break;
				case IOT_UPGRADE_NOTE: //升级通知，进入升级模式
					Mcu2Iot_upgrade_note(uc,data+sizeof(ST_PACK_INFO));
					break;
				case IOT_UPGRADE_RSP:
					Iot2Mcu_upgrade_rsp(uc,data+sizeof(ST_PACK_INFO),len-sizeof(ST_PACK_INFO),pack->seq);
					break;
					
				case JOIN_AP_REQ:
				case CHANGE_AP_REQ:
					Iot2Mcu_join_ap_req(data+sizeof(ST_PACK_INFO),uc);
					break;
				case IOT_REPORT_RSP:
					Iot2Mcu_report_rsp();
					break;
				case IOT_SET_REQ:
					Iot2Mcu_set_req(data+sizeof(ST_PACK_INFO),uc);
					break;
				case IOT_GET_REQ:
					Mcu2Iot_get_rsp(data+sizeof(ST_PACK_INFO),uc);
					break;					
				default:
					break;	
			}
		}
		
		data = data +len;
		pack = (ST_PACK_INFO*)data;
    }

}



void ESP8266_process(void)
{
	char data[1024];
    char * p_data = data;
	char * tmp = NULL;
	uint16_t len = Wifi_Fram .InfBit.FramLength;

    ESP8266_AT_send(&UC_state);
	
	if(UC_state.mode == STA){
		net_check();
	} 
	
	if(Wifi_Fram .InfBit .FramFinishFlag && UC_state.NetState != NET_LOSE){
		
		USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断		
		Wifi_Fram.Data_RX_BUF[len] = '\0';
		memcpy(data,Wifi_Fram.Data_RX_BUF,len);
		DEBUG("process recv:%d,data:%s\r\n",len,data);
		Wifi_Fram .InfBit.FramLength = 0;
	    Wifi_Fram .InfBit .FramFinishFlag = 0;
		USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断

		//USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
		
		tmp = strstr(data,"+IPD,");  //收到网络包，进行处理
		
		if( tmp ){
			tmp = strstr(data,":"); //收到网络包，进行处理
			p_data = tmp+1;
			
		}
		  
		IPD_process(p_data,len,&UC_state);

	  
//	   Wifi_Fram .InfBit.FramLength = 0;
//	   Wifi_Fram .InfBit .FramFinishFlag = 0;
//	   USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
	}
	
	

}


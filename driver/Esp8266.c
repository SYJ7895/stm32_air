#include "Esp8266.h"
#include "ctrlbsp.h"
#include <stdarg.h>
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "msg_handler.h"
#include "Sact-misc.h"
#include "flash.h"

#define IO_TIMEOUT 10

static void                   ESP8266_USART_Config                ( void );
static void                   ESP8266_USART_NVIC_Configuration    ( void );
extern uint8_t gIsConf;


ST_Esp8266_Fram_Record Wifi_Fram = { 0 };
ST_ESP_STATE UC_state = {0};


void ESP8266_USART_INT_FUN(void)
{
	uint8_t ucCh;	
	
	if ( USART_GetITStatus ( ESP8266_USARTx, USART_IT_RXNE ) != RESET ){
		
		ucCh  = USART_ReceiveData( ESP8266_USARTx );
		
		if ( Wifi_Fram .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			Wifi_Fram .Data_RX_BUF [ Wifi_Fram .InfBit .FramLength ++ ]  = ucCh;
        USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志位
	}
	 
    //数据帧接收完毕	
	if ( USART_GetFlagStatus(ESP8266_USARTx, USART_FLAG_IDLE) == SET ){
		Wifi_Fram .InfBit .FramFinishFlag = 1;
		ucCh = USART_ReceiveData( ESP8266_USARTx );  		//由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		UC_state.ucTcpClosedFlag = strstr ( Wifi_Fram .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0; //连接状态获取
	}	
	
}


void ESP8266_Usart(char* data,uint16_t len)
{
   char c ;
	while (len )	 // 判断是否到达字符串结束符
		{										  
			if ( * data == 0x5c )  //'\'
			{	
			    len = len - 2;
				c = *++data;
				switch ( c )
				{
					case 'r':									  //回车符
					USART_SendData(USART1, 0x0d);
					break;
	
					case 'n':									  //换行符
					USART_SendData(USART1, 0x0a);	
					break;
	
					default:
					USART_SendData(USART1, c);
					break;
				}			 
			}else{
			   USART_SendData(USART1, *data);
			}

			data ++;
			len --;
			while ( USART_GetFlagStatus ( USART1, USART_FLAG_TXE ) == RESET );
			
		}



}



void cmd_delay_ms(volatile u16 nms)
{
  volatile u16 i=0;
	while(!Wifi_Fram.InfBit.FramFinishFlag && (nms--))
	{
		i=800;
		while(!Wifi_Fram.InfBit.FramFinishFlag && (i--))
		{
			__NOP();
		}
	}
	//DEBUG("Flag %d %d\r\n",Wifi_Fram.InfBit.FramFinishFlag,nms);
 }




/**
  * @brief  初始化ESP8266用到的 USART
  * @param  无
  * @retval 无
  */
static void ESP8266_USART_Config ( void )
{
	USART_InitTypeDef USART_InitStructure;
	
	/* 使能USART1的时钟 */
    RCC_APB2PeriphClockCmd(ESP8266_USART_CLK, ENABLE);

//	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = ESP8266_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(ESP8266_USARTx, &USART_InitStructure);
	
	
	/* 中断配置 */
	
//	USART_ITConfig (ESP8266_USARTx,  USART_IT_CTS, DISABLE);

	ESP8266_USART_NVIC_Configuration ();
	
	USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
	USART_ITConfig ( ESP8266_USARTx, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	
	
	
	
	USART_Cmd(ESP8266_USARTx, ENABLE);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_IDLE);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_TC);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_TXE);
	USART_ClearFlag(ESP8266_USARTx,USART_FLAG_TC);
	
	
}


/**
  * @brief  配置 ESP8266 USART 的 NVIC 中断
  * @param  无
  * @retval 无
  */
static void ESP8266_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
//	NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_x );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ESP8266_USART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 * 返回  : void
 * 调用  ：被外部调用
 */
void ESP8266_Cmd ( char * cmd)
{ 
    char data[100];
	sprintf(data,"%s\r\n",cmd);
	Wifi_Fram .InfBit .FramLength = 0;               //从新开始接收新的数据包
	//ESP8266_Usart ( "%s\r\n", cmd );
	ESP8266_Usart (data,strlen(data));
	Wifi_Fram .InfBit .FramFinishFlag = 0;
	
}



/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：
 */
void ESP8266_Rst ( void )
{
	
	 ESP8266_Cmd ( "AT+RST"); 
	 delay_ms(1000);
	 ESP8266_Cmd ( "ATE1");
}

//恢复出厂设置
void ESP8266_Rstore ( void )
{
    ESP8266_ExitUnvarnishSend ();
	ESP8266_Cmd ( "AT+RESTORE"); 
	delay_ms(1000);
	 //自动连接AP
	ESP8266_Cmd ( "AT+CWAUTOCONN=1");
	delay_ms(200);

}

/*
 * 函数名：ESP8266_StartConf
 * 描述  ：smartconf
 * 输入  ：无
 * 返回  : 无
 * 调用  ：
 */
void ESP8266_StartConf ( void )
{
	 ESP8266_Cmd ( "AT+CWSTARTSMART=1"); 
	 delay_ms(300);
}


/*
 * 函数名：ESP8266_StoptConf
 * 描述  ：smartconf
 * 输入  ：无
 * 返回  : 无
 * 调用  ：
 */
void ESP8266_StopConf ( void )
{
	 ESP8266_Cmd ( "AT+CWSTOPSMART"); 
	 delay_ms(500);
	 ESP8266_Cmd ( "ATE1");
}



/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
   bool ret = true;
	switch ( enumMode )
	{
		case STA:
		  ESP8266_Cmd ( "AT+CWMODE=1"); 
		  break;
	    case AP:
		  ESP8266_Cmd ( "AT+CWMODE=2"); 
		  break;
		case STA_AP:
		  ESP8266_Cmd ( "AT+CWMODE=3"); 
		  break;
	    default:
		  ret = false;
		  break;
  }
	 return ret;
	
}

/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：创建AP热点
 * 输入  ：pSSID，AP 名称
 *         pPassWord ，ap密码
 *         enunPsdMode，加密类型
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */

void  ESP8266_BuildAP ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",5,%d", pSSID, pPassWord, enunPsdMode );
	
    ESP8266_Cmd ( cCmd);
	
}


/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  :
 * 调用  ：被外部调用
 */
void ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
    ESP8266_Cmd (cCmd);
	
}


void ESP_8266_JoinDefAP(ST_ESP_STATE *uc)
{
   DEBUG("def ap ssid %s ,pass %s\r\n",uc->AP_info.ssid,uc->AP_info.pass);
   ESP8266_JoinAP(uc->AP_info.ssid,uc->AP_info.pass);
}

void ESP_8266_SaveDefAP(ST_ESP_STATE *uc)
{
   uint16_t len = 0;
   memset(uc->AP_info.ssid,0,AP_SSID_LEN);
   memset(uc->AP_info.pass,0,AP_PASS_LEN);
   memcpy(uc->AP_info.ssid,uc->ssid_tmp,strlen(uc->ssid_tmp));
   memcpy(uc->AP_info.pass,uc->pass_tmp,strlen(uc->pass_tmp));
   len = flash_write(CONFIG_DEFAULT_AP,(uint8_t*)&uc->AP_info , sizeof(uc->AP_info));
   DEBUG("save ap ssid_tmp %s ,pass_tmp %s\r\n",uc->ssid_tmp,uc->pass_tmp);
   DEBUG("save ap ssid %s ,pass %s len %d ret:%d\r\n",uc->AP_info.ssid,uc->AP_info.pass,sizeof(uc->AP_info),len);

}

void SaveServerIP(ST_ESP_STATE *uc)
{
	uint8_t isConf = 1;
	char cmd[10] = {0};
	sprintf(cmd,"%s","IPOK");				   
	flash_write(SERVER_IP,(uint8_t*)&uc->server_ip , sizeof(uc->server_ip));
	flash_write(ISCONFIG, (uint8_t*)&isConf, sizeof(isConf));
    ESP8266_SendString ( DISABLE, cmd,strlen(cmd), Multiple_ID_0 ); 
    BL_SetLow;	
	gIsConf = 1;
    delay_ms(1000);	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 
 * 调用  ：被外部调用
 */
void ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{

   char cStr [100] = { 0 }, cCmd [120];

   switch (enumE){

	   case enumTCP:
	   	sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum);
        break;

	   case enumUDP:
	   	sprintf ( cStr, "\"%s\",\"%s\",%s,2223,0", "UDP", ip, ComNum );
		break;

	   default:
	   	break;
   }
   
   DEBUG("str:%s\r\n",cStr);

  if (id < 5){
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);
  }else{
	sprintf ( cCmd, "AT+CIPSTART=%s", cStr );
  }

  ESP8266_Cmd (cCmd);
	
}


/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		ESP8266_Cmd (cCmd1);
		delay_ms(500);
		ESP8266_Cmd (cCmd2);
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );
        ESP8266_Cmd (cCmd1);
	}
	 return true;
	
}



/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	ESP8266_Cmd (cStr);
	return true;
}


/*
 * 函数名：ESP8266_MAXCONN
 * 描述  ：WF-ESP8266模块TCPf服务器最大连接数
 * 输入  ：maxnum，最大连接数（1~5）
 * 返回  : 
 * 调用  ：被外部调用
 */
void ESP8266_MAXCONN ( uint8_t maxnum )
{
	char cStr [30];
	
	if(maxnum > 5){
		maxnum = 5;
	}
	
	sprintf ( cStr, "AT+CIPSERVERMAXCONN=%d", maxnum );

	ESP8266_Cmd (cStr);
	
}



void ESP8266_CLOSE_Link(void)
{
  ESP8266_Cmd ("AT+CIPCLOSE");

}


/*
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  :
 * 调用  ：被外部调用
 */
void ESP8266_UnvarnishSend ( void )
{
	ESP8266_Cmd ("AT+CIPMODE=1") ;	
}


/*
 * 函数名：ESP8266_ExitUnvarnishSend
 * 描述  ：配置WF-ESP8266模块退出透传模式
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_ExitUnvarnishSend ( void )
{
	delay_ms ( 300 );
	
	//ESP8266_Usart ( "+++" );
	ESP8266_Usart ( "+++",strlen("+++") );
	
	delay_ms ( 3000 ); 
	
}


/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		//ESP8266_Usart ( "%s", pStr );
		ESP8266_Usart (pStr,ulStrLength );
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd (cStr);
		delay_ms(300);
        ESP8266_Cmd (pStr);
  }
	
	return bRet;

}

void get_mac2sn()
{
    ESP8266_Cmd ("AT+CIPSTAMAC?");

}


void ESP8266_Netstate_set(uint8_t state)
{
    UC_state.NetState = state;
	DEBUG("set state %d\r\n",state);

}

void ESP8266_AT_rsp(void)
{
	char * data = Wifi_Fram.Data_RX_BUF;
    char * tmp = NULL;
	char * tmp2 = NULL;
 
	
	if(Wifi_Fram .InfBit .FramFinishFlag && Wifi_Fram .InfBit.FramLength){
		
		data[Wifi_Fram .InfBit.FramLength] = '\0';
		DEBUG("rsp date:%d %s\r\n",Wifi_Fram .InfBit.FramLength, data);
		Wifi_Fram .InfBit .FramFinishFlag = 0;
	    Wifi_Fram .InfBit.FramLength = 0;
		
		USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
		
		  if(strstr(data,"smartconfig connected wifi")){
		     ESP8266_StopConf();
			 UC_state.mode = STA_AP;
			 ESP_8266_SaveDefAP(&UC_state);
			 ESP8266_Enable_MultipleId(ENABLE);
  	         delay_ms(100);
	         ESP8266_MAXCONN(1);
	         delay_ms(100);
	         ESP8266_StartOrShutServer(ENABLE,"8080",ESP8266_TcpServer_OverTime);
		  }else if (strstr(data,"WIFI GOT IP") || strstr(data,"WIFI CONNECTED") ||
			(strstr(data,"+CWJAP") && strstr(data,"OK"))){ //成功接入AP
			
			DEBUG("join ap success state\r\n");
			
			if(UC_state.mode == STA){
				DEBUG("sta \r\n");
				UC_state.State.join_ap = 1;  //加入AP成功
				if(UC_state.State.change_ap){
                   ESP_8266_SaveDefAP(&UC_state);
				}
			}
		}else if(strstr(data,"+CWJAP:")&&strstr(data,"FAIL")){  //接入AP失败
			DEBUG("join ap error\r\n");
			if(UC_state.mode == STA && UC_state.State.change_ap == 1){
				tmp = strstr(data,"+CWJAP:");
				UC_state.change_ap_result = *(tmp+strlen("+CWJAP:")) - '0';
			    UC_state.State.change_ap = 0;
				DEBUG(" ESP_8266_JoinDefAP \r\n");
				ESP_8266_JoinDefAP(&UC_state);
				UC_state.State.join_ap = 0;
			}else{
				DEBUG(" join err mode %d change_ap %d \r\n",UC_state.mode,UC_state.State.join_ap);
				Mcu2Iot_join_ap_rsp(data,&UC_state);
			}
		}else if(strstr(data,"+CIPMODE")&&strstr(data,"OK")){  //进入透传模式
               ESP8266_Cmd ("AT+CIPSEND");
			   DEBUG("enter Penetrate mode\r\n");
               UC_state.State.Penetrate = 1;
			   if(!UC_state.State.isUpgrade){
			      ESP8266_Netstate_set(NET_MOUNT);
			   }else{
			      ESP8266_Netstate_set(UPGRADE);
			   }
		}else if(strstr(data,"ALREADY CONNECTED") ||
			     (strstr(data,"+CIPSTART")&&strstr(data,"OK"))){ //成功连接服务器
			     
		    DEBUG("already connect\r\n");
            UC_state.State.link = 1;
		}else if(strstr(data,"+IPD,")){
			tmp = strstr(data,":"); //收到网络包，进行处理
			IPD_process(tmp+1,strlen(tmp)-1,&UC_state);
		}else if(strstr(data,"+CIPSTAMAC:")){  //获取MAC
            mac2sn(data,&UC_state);
		}else if(strstr(data,"ssid:")){
			memset(UC_state.ssid_tmp,0,AP_SSID_LEN);
			memset(UC_state.pass_tmp,0,AP_PASS_LEN);
			tmp = strstr(data,"ssid:") + strlen("ssid:");
			tmp2 = strstr(tmp,"\r\n");
			memcpy(UC_state.ssid_tmp,tmp,(tmp2-tmp));
			tmp = strstr(tmp2,"password:") + strlen("password:");
			tmp2 = strstr(tmp,"\r\n");
			if(!tmp2){
			   memcpy(UC_state.pass_tmp,tmp,strlen(tmp));
			}else{
			   memcpy(UC_state.pass_tmp,tmp,(tmp2-tmp));
			}
			DEBUG("password: %s\r\n",UC_state.pass_tmp);
			DEBUG("ssid %s\r\n",UC_state.ssid_tmp);		
		}
//	   memset(Wifi_Fram.Data_RX_BUF,0,sizeof(Wifi_Fram.Data_RX_BUF));
	   USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
	}
	
}



void ESP8266_AT_send(ST_ESP_STATE *uc)
{

  static uint32_t io_tick = 0;

  if(uc->mode == AUTOCONF || uc->mode == STA_AP ){
    ESP8266_AT_rsp();
	return;
  }

  if(uc->State.join_ap &&
  	 uc->State.link &&
  	 uc->State.Penetrate){

  }else{

     if((io_tick + IO_TIMEOUT) < gettick() ){
         io_tick = gettick();
		
	     if(!uc->State.join_ap){
		 	ESP_8266_JoinDefAP(uc);	
		 }
		 
		 if(uc->State.join_ap && !uc->State.getsn){
		 	 get_mac2sn();
		 }
		 
        
		 if(uc->State.getsn && !uc->State.link){

		 	if(!uc->State.isUpgrade){
			  ESP8266_Link_Server (enumUDP, uc->server_ip, ESP8266_UdpServer_Port, Single_ID_0);
			}else{
			  delay_ms(1000);
			  ESP8266_Link_Server (enumTCP, uc->server_ip, ESP8266_TcpServer_Port, Single_ID_0);
			}
		 }

		 if(uc->State.link && !uc->State.Penetrate){
		 	
		 	ESP8266_UnvarnishSend();
			
		 }

	 }

      ESP8266_AT_rsp();
  }


}




/**
  * @brief  ESP8266初始化函数
  * @param  无
  * @retval 无
  */
void ESP8266_Init ( void )
{ 
	ESP8266_USART_Config (); 
	
	memset(UC_state.server_ip,0,IPLEN);
	memset(UC_state.AP_info.ssid,0,AP_SSID_LEN);
	memset(UC_state.AP_info.pass,0,AP_PASS_LEN);
    
	
	flash_read(CONFIG_DEFAULT_AP, (uint8_t*)&UC_state.AP_info, sizeof(UC_state.AP_info));
	flash_read(SERVER_IP,(uint8_t*)&UC_state.server_ip , sizeof(UC_state.server_ip));
	flash_read(UPGRADE_INFO_ADDR,&UC_state.upgradeRest,sizeof(UC_state.upgradeRest));


	DEBUG("isConf %d,UC_state.AP_info :%s,%s ; server:%s,upgrade:%d\r\n",gIsConf,UC_state.AP_info.ssid,UC_state.AP_info.pass,UC_state.server_ip,UC_state.upgradeRest);
	
	//ESP8266_ExitUnvarnishSend ();
	DEBUG("wifi reset 1\r\n");
	//ESP8266_Rst();
	UC_state.hb_prb = 60;
	UC_state.State.getsn = 0;
	
	if(gIsConf != 1 ){
	  DEBUG("enter auto config\r\n");
	  UC_state.upgradeRest = NOUPGRADE;
	  flash_write(UPGRADE_INFO_ADDR,(uint8_t*)&UC_state.upgradeRest,sizeof(UC_state.upgradeRest));
	  UC_state.mode = AUTOCONF;
	  ESP8266_Rst();
	  DEBUG("wifi reset\r\n");
	  delay_ms(300);
	//  ESP8266_Cmd("AT+CWAUTOCONN = 0");
	  ESP8266_Net_Mode_Choose(STA);
	  DEBUG("wifi set to sta\r\n");
	  delay_ms(300);
	  ESP8266_StartConf();
	}else{
	   delay_ms(100);
	   ESP8266_Rst();
	   delay_ms(100);
       ESP8266_Net_Mode_Choose(STA);
	   delay_ms(300);
	   ESP8266_Enable_MultipleId(DISABLE);
	   delay_ms(300);
       UC_state.mode = STA;
	   ESP_8266_JoinDefAP(&UC_state);
	   delay_ms(300);
	   UC_state.State.isUpgrade = 0;	  
	}
	
}

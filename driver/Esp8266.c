#include "Esp8266.h"
#include "ctrlbsp.h"
#include <stdarg.h>
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "msg_handler.h"
#include "Sact-misc.h"
#include "flash.h"
#include <stdlib.h>

#define IO_TIMEOUT 10

static void                   ESP8266_USART_Config                ( void );
static void                   ESP8266_USART_NVIC_Configuration    ( void );
extern uint8_t gIsConf;
extern uint8_t configap;
uint8_t joinret = 0;
uint8_t curjoin = 0;

static uint32_t io_tick = 0;

ST_Esp8266_Fram_Record Wifi_Fram = { 0 };
ST_ESP_STATE UC_state = {0};
static  char gdata[1024] ={0};

#define CURSSID "ESPConfig"
#define CURPASS "12345678"

void ESP8266_USART_INT_FUN(void)
{
	uint8_t ucCh;	
	
	if ( USART_GetITStatus ( ESP8266_USARTx, USART_IT_RXNE ) != RESET ){
		
		ucCh  = USART_ReceiveData( ESP8266_USARTx );
		
		if ( Wifi_Fram .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //Ԥ��1���ֽ�д������
			Wifi_Fram .Data_RX_BUF [ Wifi_Fram .InfBit .FramLength ++ ]  = ucCh;
        USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־λ
	}
	 
    //����֡�������	
	if ( USART_GetFlagStatus(ESP8266_USARTx, USART_FLAG_IDLE) == SET ){
		Wifi_Fram .InfBit .FramFinishFlag = 1;
		ucCh = USART_ReceiveData( ESP8266_USARTx );  		//�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
		UC_state.ucTcpClosedFlag = strstr ( Wifi_Fram .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0; //����״̬��ȡ
	}	
	
}


void ESP8266_Usart(char* data,uint16_t len)
{
   char c ;
	while (len )	 // �ж��Ƿ񵽴��ַ���������
		{										  
			if ( * data == 0x5c )  //'\'
			{	
			    len = len - 2;
				c = *++data;
				switch ( c )
				{
					case 'r':									  //�س���
					USART_SendData(USART1, 0x0d);
					break;
	
					case 'n':									  //���з�
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
  * @brief  ��ʼ��ESP8266�õ��� USART
  * @param  ��
  * @retval ��
  */
static void ESP8266_USART_Config ( void )
{
	USART_InitTypeDef USART_InitStructure;
	
	/* ʹ��USART1��ʱ�� */
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
	
	
	/* �ж����� */
	
//	USART_ITConfig (ESP8266_USARTx,  USART_IT_CTS, DISABLE);

	ESP8266_USART_NVIC_Configuration ();
	
	USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
	USART_ITConfig ( ESP8266_USARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	
	
	
	
	USART_Cmd(ESP8266_USARTx, ENABLE);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_IDLE);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_TC);
	USART_ClearFlag(ESP8266_USARTx, USART_FLAG_TXE);
	USART_ClearFlag(ESP8266_USARTx,USART_FLAG_TC);
	
	
}


/**
  * @brief  ���� ESP8266 USART �� NVIC �ж�
  * @param  ��
  * @retval ��
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
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 * ����  : void
 * ����  �����ⲿ����
 */
void ESP8266_Cmd ( char * cmd)
{ 
    char data[100];
	sprintf(data,"%s\r\n",cmd);
	Wifi_Fram .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	//ESP8266_Usart ( "%s\r\n", cmd );
	ESP8266_Usart (data,strlen(data));
	Wifi_Fram .InfBit .FramFinishFlag = 0;
	
}



/*
 * ��������ESP8266_Rst
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ��
 */
void ESP8266_Rst ( void )
{
	
	 ESP8266_Cmd ( "AT+RST"); 
	 delay_ms(1000);
	 ESP8266_Cmd ( "ATE1");
}

//�ָ���������
void ESP8266_Rstore ( void )
{
    ESP8266_ExitUnvarnishSend ();
	ESP8266_Cmd ( "AT+RESTORE"); 
	delay_ms(1000);
	 //�Զ�����AP
	ESP8266_Cmd ( "AT+CWAUTOCONN=1");
	delay_ms(200);

}

/*
 * ��������ESP8266_StartConf
 * ����  ��smartconf
 * ����  ����
 * ����  : ��
 * ����  ��
 */
void ESP8266_StartConf ( void )
{
	 ESP8266_Cmd ( "AT+CWSTARTSMART=1"); 
	 delay_ms(300);
}


/*
 * ��������ESP8266_StoptConf
 * ����  ��smartconf
 * ����  ����
 * ����  : ��
 * ����  ��
 */
void ESP8266_StopConf ( void )
{
	 ESP8266_Cmd ( "AT+CWSTOPSMART"); 
	 delay_ms(500);
	 ESP8266_Cmd ( "ATE1");
}



/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_BuildAP
 * ����  ������AP�ȵ�
 * ����  ��pSSID��AP ����
 *         pPassWord ��ap����
 *         enunPsdMode����������
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */

void  ESP8266_BuildAP ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode )
{
	char cCmd [120];
	int p = 0;
	srand(gettick());
    p= rand()%1000;      //����һ��30��50�������
	
	if(UC_state.State.getsn){
		sprintf ( cCmd, "AT+CWSAP=\"%s_%s\",\"%s\",1,%d", pSSID,UC_state.SN, pPassWord, enunPsdMode );
	}else{
		sprintf ( cCmd, "AT+CWSAP=\"%s_%d\",\"%s\",1,%d", pSSID,p, pPassWord, enunPsdMode );
	}
	
    ESP8266_Cmd ( cCmd);
	
}


/*
 * ��������ESP8266_JoinAPCUR
 * ����  ��WF-ESP8266ģ����ʱ�����ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  :
 * ����  �����ⲿ����
 */
void ESP8266_JoinAPCUR ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP_CUR=\"%s\",\"%s\"", pSSID, pPassWord );
	
    ESP8266_Cmd (cCmd);
	
}

/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  :
 * ����  �����ⲿ����
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

void ESP8266_JoinState(void)
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP?");
	
    ESP8266_Cmd (cCmd);
	
}

void ESP8266_GetStaIp(void)
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CIFSR");
	
    ESP8266_Cmd (cCmd);
	
}

void ESP_8266_SaveDefAP(ST_ESP_STATE *uc)
{
   uint8_t isConf = 1;
   memset(uc->AP_info.ssid,0,AP_SSID_LEN);
   memset(uc->AP_info.pass,0,AP_PASS_LEN);
   memcpy(uc->AP_info.ssid,uc->ssid_tmp,strlen(uc->ssid_tmp));
   memcpy(uc->AP_info.pass,uc->pass_tmp,strlen(uc->pass_tmp));
   flash_write(CONFIG_DEFAULT_AP,(uint8_t*)&uc->AP_info , sizeof(uc->AP_info));
   DEBUG("save ap ssid_tmp %s ,pass_tmp %s\r\n",uc->ssid_tmp,uc->pass_tmp);
   DEBUG("save ap ssid %s ,pass %s len %d\r\n",uc->AP_info.ssid,uc->AP_info.pass,sizeof(uc->AP_info));
   flash_write(ISCONFIG, (uint8_t*)&isConf, sizeof(isConf));
}

void SaveServerIP(ST_ESP_STATE *uc)
{					   
	flash_write(SERVER_IP,(uint8_t*)&uc->server_ip , sizeof(uc->server_ip));
}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 
 * ����  �����ⲿ����
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


void ESP8266_udplisten ()
{

   char cCmd [120] = {0};
  
   sprintf ( cCmd, "AT+CIPSTART=\"UDP\",\"127.0.0.1\",2223.2223,2");

   ESP8266_Cmd (cCmd);
	
}

void StarUdpServer(char* ip)
{
	char cCmd [100] = { 0 };
	sprintf ( cCmd, "AT+CIPSTART=\"UDP\",\"%s\",8888,8888,0",ip);
	ESP8266_Cmd (cCmd);

}



/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	ESP8266_Cmd (cStr);
	return true;
}


/*
 * ��������ESP8266_MAXCONN
 * ����  ��WF-ESP8266ģ��TCPf���������������
 * ����  ��maxnum�������������1~5��
 * ����  : 
 * ����  �����ⲿ����
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

/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		ESP8266_Enable_MultipleId(ENABLE);
		delay_ms(100);
		ESP8266_MAXCONN(1);
		delay_ms(100);
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


void ESP8266_CLOSE_Link(void)
{
  ESP8266_Cmd ("AT+CIPCLOSE");

}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  :
 * ����  �����ⲿ����
 */
void ESP8266_UnvarnishSend ( void )
{
	ESP8266_Cmd ("AT+CIPMODE=1") ;	
}


/*
 * ��������ESP8266_ExitUnvarnishSend
 * ����  ������WF-ESP8266ģ���˳�͸��ģʽ
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_ExitUnvarnishSend ( void )
{
	delay_ms ( 300 );
	
	//ESP8266_Usart ( "+++" );
	ESP8266_Usart ( "+++",strlen("+++") );
	
	delay_ms ( 3000 ); 
	
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
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
    char * p_data = gdata;
	char * tmp = NULL;
	uint16_t len = 0;
 
	
	if(Wifi_Fram .InfBit .FramFinishFlag && Wifi_Fram .InfBit.FramLength){
		
		USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
        len = Wifi_Fram .InfBit.FramLength;		
		Wifi_Fram.Data_RX_BUF[len] = '\0';
		memset(gdata,0,1024);
		memcpy(gdata,Wifi_Fram.Data_RX_BUF,len);
		DEBUG("uart recv:%d,data:%s\r\n",len,data);
//		USART_printf(USART3,"uart recv:%d,data:%s\r\n",len,data);
		Wifi_Fram .InfBit.FramLength = 0;
	    Wifi_Fram .InfBit .FramFinishFlag = 0;
		USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
		
    	  if (strstr(gdata,"WIFI GOT IP") || strstr(gdata,"WIFI CONNECTED") ||
			(strstr(gdata,"+CWJAP") && strstr(gdata,"OK")) ||
		     (strstr(gdata,"OK")&& len == 6 && UC_state.State.join_ap == 0) ){ //�ɹ�����AP
			
//			DEBUG("join ap success state\r\n");
			
			if(UC_state.mode == STA){
				DEBUG("sta \r\n");
				UC_state.State.join_ap = 1;  //����AP�ɹ�
				if(UC_state.State.change_ap){
                   ESP_8266_SaveDefAP(&UC_state);
				}
			}else if(UC_state.mode == AUTOCONF){ //����ģʽ
				if(curjoin == 2){
					configap = 0; //��֤����
					joinret = 1;
					ESP_8266_SaveDefAP(&UC_state);//��֤�·�AP��Ϣ�ɹ����������õ�������Ϣ
//					ESP8266_Rst();
//					delay_ms(2000);
					ESP8266_Cmd ("AT+CWQAP");
					delay_ms(1000);	
					ESP8266_JoinAPCUR(CURSSID,CURPASS); //������ʱAP
					io_tick = gettick();
					curjoin = 0;
					UC_state.State.join_ap = 0;
					UC_state.State.link = 0;
					UC_state.State.Penetrate = 0;
				
				}else{
				    curjoin = 1; //������ʱAP�ɹ�
					UC_state.State.join_ap = 1;
				}
			}
		}else if(strstr(gdata,"+CWJAP")&&strstr(gdata,"FAIL")){  //����APʧ��
//			DEBUG("join ap error\r\n");
			if(UC_state.mode == STA && UC_state.State.change_ap == 1){
				tmp = strstr(gdata,"+CWJAP:");
				UC_state.change_ap_result = *(tmp+strlen("+CWJAP:")) - '0';
			    UC_state.State.change_ap = 0;
				DEBUG(" ESP_8266_JoinDefAP \r\n");
				ESP_8266_JoinDefAP(&UC_state);
				io_tick = gettick();
				UC_state.State.join_ap = 0;
			}else if(UC_state.mode == AUTOCONF){
				if(curjoin == 2){
					configap = 0; //��֤����
					curjoin = 0;
					joinret = 2; //��֤�·�AP��Ϣ����
//                    ESP8266_Rst();
//					delay_ms(2000);	
//                    ESP8266_Cmd ("AT+CWQAP");
//					delay_ms(1000);						
					ESP8266_JoinAPCUR(CURSSID,CURPASS); //������ʱAP
					UC_state.State.join_ap = 0;
					UC_state.State.link = 0;
                    UC_state.State.Penetrate = 0;					
				}
			}else{
//				DEBUG(" join err mode %d change_ap %d \r\n",UC_state.mode,UC_state.State.join_ap);
				Mcu2Iot_join_ap_rsp(gdata,&UC_state);
			}
		}else if(strstr(gdata,"+CIPMODE")&&strstr(gdata,"OK")){  //����͸��ģʽ
               ESP8266_Cmd ("AT+CIPSEND");
//			   DEBUG("enter Penetrate mode\r\n");
               UC_state.State.Penetrate = 1;
			   if(!UC_state.State.isUpgrade){
			      ESP8266_Netstate_set(NET_MOUNT);
			   }else{
			      ESP8266_Netstate_set(UPGRADE);
			   }
		}else if(strstr(gdata,"ALREADY CONNECTED") ||
			     (strstr(gdata,"+CIPSTART")&&strstr(gdata,"OK"))){ //�ɹ����ӷ�����
			     
//		    DEBUG("already connect\r\n");
            UC_state.State.link = 1;
		}else if(strstr(gdata,"+CIPSTAMAC:")){  //��ȡMAC
            mac2sn(gdata,&UC_state);
		}else if(strstr(gdata,"+IPD,")){
			tmp = strstr(gdata,":"); //�յ�����������д���
			if( tmp ){
			   tmp = strstr(gdata,":"); //�յ�����������д���
			   p_data = tmp+1;
			}
			IPD_process(p_data,len,&UC_state);
		}else if(strstr(gdata,"ERROR") || strstr(gdata,"wdt reset")){
			ESP8266_ExitUnvarnishSend (); 
			delay_ms(200);
			ESP8266_Rst();
			io_tick = gettick();
			UC_state.State.join_ap = 0;
			UC_state.State.link = 0;
            UC_state.State.Penetrate = 0;
		
		}else{
			
		   IPD_process(p_data,len,&UC_state);
		}
//	   memset(Wifi_Fram.Data_RX_BUF,0,sizeof(Wifi_Fram.Data_RX_BUF));
//	   USART_ITConfig ( ESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
	}
	
}



void ESP8266_AT_send(ST_ESP_STATE *uc)
{
  char cmd[50] = {0};

 
  if(uc->State.join_ap &&
  	 uc->State.link &&
  	 uc->State.Penetrate){
		 if(gIsConf != 1 && (io_tick + IO_TIMEOUT) < gettick()){
    		 io_tick = gettick();
			 if(curjoin == 1 && !configap){
				 LCD_Set_Temp(11,1);
				if (joinret == 1){
					sprintf(cmd,"apok:%s\r\n",uc->SN);
										
				}else if(joinret == 2){		 
					sprintf(cmd,"apfail:%s\r\n",uc->SN);					
				}else{
					sprintf(cmd,"apreq:%s\r\n",uc->SN);
				}
                ESP8266_SendString ( ENABLE,cmd,strlen(cmd), Single_ID_0 );				
			 }
		 }

  }else{

     if((io_tick + IO_TIMEOUT) < gettick() ){
         io_tick = gettick();
		 
		  if( !uc->State.getsn){
		 	 get_mac2sn();
			 return;
		 }
		
	     if(uc->State.getsn && !uc->State.join_ap){
			 if(gIsConf != 1 ){	
				 if(curjoin == 2){
//					 ESP8266_JoinState();
				     ESP8266_JoinAPCUR(UC_state.ssid_tmp,UC_state.pass_tmp);
				 }else{
					 ESP8266_JoinAPCUR("ESPConfig","12345678");; //��������������ʱAP				 
				 }
				 
			 }else{
				 ESP_8266_JoinDefAP(uc);				 
			 }
			 return;
		 	
		 }
		 
		
        
		 if(uc->State.join_ap && !uc->State.link){

		 	if(gIsConf != 1){	
				ESP8266_Link_Server (enumUDP, "255.255.255.255","12345", Single_ID_0); 						
			}else if(!uc->State.isUpgrade){
			  ESP8266_Link_Server (enumUDP, uc->server_ip, ESP8266_UdpServer_Port, Single_ID_0);
			}else{
			  delay_ms(1000);
			  ESP8266_Link_Server (enumTCP, uc->server_ip, ESP8266_TcpServer_Port, Single_ID_0);
			}
			return;
		 }

		 if(uc->State.link && !uc->State.Penetrate){
		 	ESP8266_UnvarnishSend();
		 }
		 

	 }
  }


}




/**
  * @brief  ESP8266��ʼ������
  * @param  ��
  * @retval ��
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


//	DEBUG("isConf %d,UC_state.AP_info :%s,%s ; server:%s,upgrade:%d\r\n",gIsConf,UC_state.AP_info.ssid,UC_state.AP_info.pass,UC_state.server_ip,UC_state.upgradeRest);
	
	//ESP8266_ExitUnvarnishSend ();
//	DEBUG("wifi reset 1\r\n");
	//ESP8266_Rst();
	UC_state.hb_prb = 60;
	UC_state.State.getsn = 0;
	
	
	if(gIsConf != 1 ){
	  curjoin = 0; //��δ������ʱAP
      joinret = 0; //��δ�����·�AP��Ϣ	
//	  DEBUG("enter auto config\r\n");
	  UC_state.upgradeRest = NOUPGRADE;
	  flash_write(UPGRADE_INFO_ADDR,(uint8_t*)&UC_state.upgradeRest,sizeof(UC_state.upgradeRest));
	  UC_state.mode = AUTOCONF;
	  ESP8266_Rstore();
	  delay_ms(200);
	  ESP8266_Net_Mode_Choose(STA);
	  delay_ms(200);   	
	}else{
	   ESP8266_Rst();
	   delay_ms(1000);
       ESP8266_Net_Mode_Choose(STA);
	   delay_ms(300);
	   ESP8266_Enable_MultipleId(DISABLE);
	   delay_ms(300);
       UC_state.mode = STA;
//	   ESP_8266_JoinDefAP(&UC_state);
//	   delay_ms(300);
	   UC_state.State.isUpgrade = 0;	  
	}
	
}

void autoconfig()
{
	static uint32_t tick = 0;
	if(gIsConf != 1 ){	
		if(!curjoin && ((tick + IO_TIMEOUT) < gettick())){
		   tick = gettick();
		    ESP8266_JoinAPCUR("ESPConfig","12345678");; //��������������ʱAP
		}
		if(curjoin == 2 && ((tick + IO_TIMEOUT) < gettick())){
		   tick = gettick();  
		   ESP8266_SendString ( ENABLE,"apreq",5, Single_ID_0 ); //��������ap��Ϣ
		}
		
		if(configap && !joinret && ((tick + 15) < gettick())){
			tick = gettick();  
		   ESP8266_JoinState(); //��ѯ����AP��Ϣ	
		}
		 
		if(curjoin == 1 && ((tick + IO_TIMEOUT) < gettick())){
			tick = gettick(); 
		  ESP8266_SendString ( ENABLE,"apok",4, Single_ID_0 );
					LCD_Set_Temp(11,1);
		}
	}
}

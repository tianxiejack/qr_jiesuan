//#include "std.h"
//#include "sdkdef.h"
//#include "main.h"
//#include "PVEMsgfunsTab.h"


#include "LaserPort.h"

static LaserPort_t LaserObj;
static int ibInit=0;
//static PVE_MSG *pOutMsg=NULL;
//static BYTE g_bSendBuf[256]={0};

static int LaserDistance=-1;
static BOOL LaserPORT_open();
static void LaserPORT_close();

int getLaserDistance()
{
	return (LaserDistance>=0)?(LaserDistance):(0);
}
static void startLaserTimer()
{
//	CTimerCtrl * pCtrlTimer;
//	pCtrlTimer = pOsdCtrlObj->pOsdTimer;
//	pCtrlTimer->SetTimer(pCtrlTimer,eLaser_Timer,LASER_TIMER,LaserPORT_lpfnTimer,NULL);
}
void killLaserTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pOsdCtrlObj->pOsdTimer;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eLaser_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eLaser_Timer);
	}
	//Posd[eMeasureType] = MeasureTypeOsd[0];
#endif
}
//DFU_Handle pDUFObj=NULL;
/*
 *  ======== LaserPORT_initial========
 *
 *  This function is used to initial LaserPORT object
 */
LaserPORT_Handle LaserPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&LaserObj);
#if 0
	ibInit=1;
	LaserObj.pInter.PortType	= Enum_LaserPort;//Enum_TracPort;
	LaserObj.pInter.OpenPort 	= LaserPORT_open;
	LaserObj.pInter.ClosePort	= LaserPORT_close;
	LaserObj.pInter.Write		= LaserPORT_send;
	LaserObj.pInter.Read		= NULL;
	LaserObj.pInter.Control		= NULL;
	LaserObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	LaserObj.UartPort.uartId	= 0xFF;
	LaserObj.UartPort.uartStat	= eUart_SyncFlag;
	LaserObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	LaserObj.UartPort.recvLen	= 0;

//	SDK_SEM_RESET(LaserObj.UartPort.syncObj);

	if(!LaserPORT_open()){
		SDK_SEM_DESTROY(LaserObj.UartPort.syncObj);
		return NULL;
	}

//	pOutMsg = SDK_MALLOC(sizeof(PVE_MSG)+10, EXTERNALHEAP);
#endif
	return (&LaserObj);
}

/*
 *  ======== LaserPORT_unInitial========
 *
 *  This function is used to uninitial LaserPORT object
 */
void LaserPORT_unInitial(LaserPORT_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
	LaserPORT_close();
}

/*
 *  ======== LaserPORT_unInitial========
 *
 *  This function is used to open LaserPORT
 */
static BOOL LaserPORT_open()
{
#if 0
	UartObj* pUartObj=&LaserObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_LASER_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1){
		CloseUart(EUART_LASER_PORT);
		return FALSE;
	}
#endif
	return TRUE;
}

/*
 *  ======== LaserPORT_close========
 *
 *  This function is used to open LaserPORT
 */
static void LaserPORT_close()
{
	UartObj* pUartObj= &LaserObj.UartPort;
#if 0
	if(pUartObj->syncObj!=NULL){
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else CloseUart(pUartObj->uartId);
#endif
	return ;
}


/*
 *  ======== LaserPORT_send========
 *
 *  This function is used to send data through the LaserPORT
 */
int LaserPORT_send(BYTE* pSendBuf,int iLen)
{
	UartObj* pUartObj=&LaserObj.UartPort;
	int iSendlen=0, iCur;
#if 0
	if(iLen>250){
		LOGMSGTRACE(LOCALDEBUG,"Len too large:%d\n",iLen,2,3,4);
	}

	while(pSendBuf!=NULL && iLen!=0){
		
		iCur=SendDataToUart(pUartObj->uartId, pSendBuf+iSendlen,iLen,FALSE);
		if(iCur<0)
			break;
		else
			iSendlen+=iCur;
		iLen-=iCur;

	}	
#endif
	return iSendlen;
}
void LaserPORT_lpfnTimer(void* cbParam)
{
//	CTimerCtrl * pCtrlTimer = pOsdCtrlObj->pOsdTimer;

//	pCtrlTimer->KillTimer(pCtrlTimer,eLaser_Timer);
//	SendMessage(CMD_LASER_FAIL,LASERERR_TIMEOUT);
}

int LaserPORT_requst_TRACE()		 //���Ͳ������
{            
#if 0
	int i;
	BYTE* Pbuf = (BYTE*)pOutMsg;
	
	BYTE buf[4]={0xCC,0x01,0x01};
	buf[3] = (buf[0]^buf[1]^buf[2]);
//	memcpy(LaserObj.UartPort.sendBuf,buf,sizeof(buf));

	pOutMsg->head.SyFlag		= 0xAA;
	pOutMsg->head.eCmdType		= eCT_LaserPort;
	pOutMsg->head.uiVersion		= SOFTWARE_VERSION;
	pOutMsg->head.uiRequestId 	= 1;

	memcpy(pOutMsg->UN_MSG.data,buf,sizeof(buf));
	pOutMsg->head.uiSize = sizeof(PVE_MSG_HEADER)+sizeof(buf)+1;
	Pbuf[pOutMsg->head.uiSize-1]=0x0;
	for(i=0; i<pOutMsg->head.uiSize-1; i++)
	{
		Pbuf[pOutMsg->head.uiSize-1] +=Pbuf[i];
	}

	TRACEPORT_send(Pbuf,pOutMsg->head.uiSize);
#endif
	return 0;
}

int LaserPORT_requst()		//���Ͳ������
{             
#if 0
	int stat=0;

	BYTE buf[4]={0xCC,0x01,0x01};
//	buf[3] = (buf[0]+buf[1]+buf[2]);
	buf[3] = (buf[0]^buf[1]^buf[2]);
	strcpy(LaserObj.UartPort.sendBuf,buf,sizeof(buf));
	stat = LaserPORT_send(LaserObj.UartPort.sendBuf, sizeof(buf));
	if(sizeof(buf) != stat)
		return -1;
	
	LaserPORT_requst_TRACE();
// start a timeout fail
	startLaserTimer();
#endif
	return 0;
}




int LaserPORT_Ack()	//���ȷ��ָ��
{                
#if 0
	int stat=0;
	BYTE buf[4]={0xCC,0x01,0x02};
	buf[3] = (buf[0]^buf[1]^buf[2]);
	killLaserTimer();
	strcpy(LaserObj.UartPort.sendBuf,buf,sizeof(buf));
	stat = LaserPORT_send(LaserObj.UartPort.sendBuf, sizeof(buf));
	if(sizeof(buf) != stat)
		return -1;
#endif	
	return 0;
}
/*
int LaserPORT_switchFov_Big(){                //�л����ӳ�
	int stat=0;
	BYTE buf[4]={0xCC,0x02,0x02};
	buf[3] = (buf[0]^buf[1]^buf[2]);
	strcpy(LaserObj.UartPort.sendBuf,buf,sizeof(buf));
	stat = LaserPORT_send(LaserObj.UartPort.sendBuf, sizeof(buf));
	if(sizeof(buf) != stat)
		return -1;
	
	return 0;
}

int LaserPORT_switchFov_Small(){                //�л�С�ӳ�
	int stat=0;
	BYTE buf[4]={0xCC,0x02,0x02};
	buf[3] = (buf[0]^buf[1]^buf[2]);
	strcpy(LaserObj.UartPort.sendBuf,buf,sizeof(buf));
	stat = LaserPORT_send(LaserObj.UartPort.sendBuf, sizeof(buf));
	if(sizeof(buf) != stat)
		return -1;
	
	return 0;
}
*/

/*
 *  ======== LaserPORT_recv========
 *
 *  This function is used to receive data from uart port
 */
static int LaserPORT_recv(UartObj*pUartObj,int iLen)
{
	int iRcvLen=0,iCurLen=0;
	BYTE *pCurBuf = pUartObj->recvBuf + pUartObj->recvLen;
	int stat = 0;
#if 0
	if(!iLen||pUartObj==NULL){
		return -1;
	}
	while (TRUE){
		if((iCurLen=GetDataFromUart(pUartObj->uartId,pCurBuf,iLen-iRcvLen, &stat))>0){
			if(stat != 0)
				return -2;
			iRcvLen+=iCurLen;
			pCurBuf+=iCurLen;
			if(iRcvLen==iLen){
				break;
			}
		}else{
			return -1;
		}
	}
	
	pUartObj->recvLen+=iRcvLen;
#endif
	return iRcvLen;
}
static int LaserPORT_recvCfg(UartObj*pUartObj,int iLen)
{
#if 0
	BYTE *pCur = pUartObj->recvBuf;
	int ilen = 0, stat;
	while(TRUE){
		if(GetDataFromUart(pUartObj->uartId,pCur,1, &stat)!=1){
			return -1;
		}else{
			ilen ++;
			if(ilen == 1){
				if(0xCC == *pCur)
					break;
				else{
					ilen=0;
				}
			}
		}
	}
	pUartObj->recvLen=1;
#endif
	return 0;
}


BYTE LaserPORT_CheckSum(BYTE* buf){
	BYTE sum=0;
	int i=0;
	for(i=0; i<6; i++){
		sum ^= buf[i];
	}
	
	return sum;
}
int LaserPORT_PhraseBye(BYTE* pRecv)
{
	int value;
	value = pRecv[2]<<8 | pRecv[3];
	if(9999 == value){
		LaserDistance = -1;
//		SendMessage(CMD_LASER_FAIL,LASERERR_NOECHO);
	}else if(0 == value){
		LaserDistance = -1;
//		SendMessage(CMD_LASER_FAIL,LASERERR_NOSAMPLE);//��ȡ��
	}else{
		LaserDistance = value;
//		SendMessage(CMD_LASER_OK,value);//������ֵ
	}
	
//	LaserPORT_Ack();                  //���ȷ��
	//TracePort �����ǰ�ֶ�
	return 0;
}



void * LaserPORT_recvTask(void* prm)
{

//	UartObj* pUartObj=&LaserObj.UartPort;
	//BYTE* pSend= pUartObj->sendBuf;	
//	BYTE* pRecv= pUartObj->recvBuf;

//	while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) //���Task����ʱ����Ҫ��main.c init.c�м���
	while(1)
	{
		//OSA_printf("LaserPORT_recvTask\n");
	//	OSA_waitMsecs(4000);
	#if 0
		pUartObj->recvLen = 0;	
		if(0 != LaserPORT_recvCfg(pUartObj, 1))
		{
			continue;
		}
		if(0 > LaserPORT_recv(pUartObj, 6))
		{
			continue;
		}
		if(pRecv[6] != LaserPORT_CheckSum(pRecv)){
			pUartObj->recvLen = 0;	
			continue;
		}
		
		LaserPORT_PhraseBye(pRecv);
	#endif
	}
	
	return NULL;
}



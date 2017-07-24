
/*
	���ڶ���Ƕȴ�����

	�����ʽ:
	BYTE     HEX      CONTEXT
	1��2     0x5FF5    ��ͷ
	 3    0x00~0xFF   MSB
	 4    0x00~0xFF   LSB(-5 ~ 75)ʵ��ֵ=����ֵ/100
	 5    0x00/0xFF   0x00�Ƕ�Ϊ��0xFF�Ƕ�Ϊ��     

	 �ӿ�:   RS422
	 ������: 19200

	 �ο�TRACEPort������������
*/


//#include "std.h"
//#include "sdkdef.h"
//#include "main.h"
//#include "PVEMsgfunsTab.h"
//#include "byteParser_util.h"
//#include "permanentStorage.h"

#include "app_global.h"
#include "GrenadePort.h"

static GrenadePort_t GrenadeObj;
static int ibInit=0;
static double GrenadeAngle=0.0;

static BOOL GrenadePORT_open();
static void GrenadePORT_close();
void killSelfCheckGrenadeAngleTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eGrenadeAngle_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eGrenadeAngle_Timer);
	}
#endif
}
void SelfCheckGrenadeAngleTimer_cbFxn(void* cbParam)
{
//	killSelfCheckGrenadeAngleTimer();
//	sendCommand(CMD_GENERADE_SENSOR_ERR);
}

void startSelfCheckGrenadeAngle_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eGrenadeAngle_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eGrenadeAngle_Timer,SELFCHECK_TIMER,SelfCheckGrenadeAngleTimer_cbFxn,(void*)(0x01));	
	}
#endif
}

double getGrenadeAngleAbs()
{
	return GrenadeAngle;
}
double getGrenadeAngle()
{
	return 1.0;
//	return GrenadeAngle - gGrenadeKill_ZCTable.Angle;
}//DFU_Handle pDUFObj=NULL;
/*
 *  ======== GrenadePORT_initial========
 *
 *  This function is used to initial GrenadePORT object
 */
GrenadePORT_Handle GrenadePORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&GrenadeObj);

	ibInit=1;
	GrenadeObj.pInter.PortType	= Enum_GrenadePort;//Enum_TracPort;
	GrenadeObj.pInter.OpenPort 	= GrenadePORT_open;
	GrenadeObj.pInter.ClosePort	= GrenadePORT_close;
	GrenadeObj.pInter.Write		= GrenadePORT_send;
	GrenadeObj.pInter.Read		= NULL;
	GrenadeObj.pInter.Control		= NULL;
	GrenadeObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	GrenadeObj.UartPort.uartId	= 0xFF;
	GrenadeObj.UartPort.uartStat	= eUart_SyncFlag;
//	GrenadeObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	GrenadeObj.UartPort.recvLen		= 0;

//	SDK_SEM_RESET(GrenadeObj.UartPort.syncObj);

#if 0
	if(!GrenadePORT_open()){
		SDK_SEM_DESTROY(GrenadeObj.UartPort.syncObj);
		return NULL;
	}
#endif
	return (&GrenadeObj);
}

/*
 *  ======== GrenadePORT_unInitial========
 *
 *  This function is used to uninitial GrenadePORT object
 */
void GrenadePORT_unInitial(GrenadePORT_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
	GrenadePORT_close();
}

/*
 *  ======== GrenadePORT_unInitial========
 *
 *  This function is used to open GrenadePORT
 */
static BOOL GrenadePORT_open()
{
#if 0
	UartObj* pUartObj=&GrenadeObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_GRENADE_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1){
		CloseUart(EUART_GRENADE_PORT);
		return FALSE;
	}
#endif
	return TRUE;
}

/*
 *  ======== GrenadePORT_close========
 *
 *  This function is used to open GrenadePORT
 */
static void GrenadePORT_close()
{
#if 0
	UartObj* pUartObj= &GrenadeObj.UartPort;

	if(pUartObj->syncObj!=NULL){
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else CloseUart(pUartObj->uartId);
#endif
}


/*
 *  ======== GrenadePORT_send========
 *
 *  This function is used to send data through the GrenadePORT
 */
int GrenadePORT_send(BYTE* pSendBuf,int iLen)
{
	UartObj* pUartObj=&GrenadeObj.UartPort;
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


/*
 *  ======== GrenadePORT_recv========
 *
 *  This function is used to receive data from uart port
 */
static int GrenadePORT_recv(UartObj*pUartObj,int iLen)
{
#if 0
	int iRcvLen=0,iCurLen=0;
	BYTE *pCurBuf = pUartObj->recvBuf + pUartObj->recvLen;
	int stat = 0;
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
	return iRcvLen;
#endif
	return 0;
}


static int GrenadePORT_recvCfg(UartObj*pUartObj,int iLen)
{
#if 0
	BYTE *pCur = pUartObj->recvBuf;
	int ilen = 0, stat;
	while(TRUE){
		if(GetDataFromUart(pUartObj->uartId,pCur,1, &stat)!=1){
			return -1;
		}else{
			ilen ++;
			pCur ++;
			if(ilen == 2){
//				if(0x5FF5 == stoh2(pUartObj->recvBuf))
				if(0x8FF8 == stoh2(pUartObj->recvBuf))   //todo: Ϊ���÷�λ������ֵ���ȸĶ�ͬ��ͷ����
					break;
				else{
					ilen=1;
					pCur--;
					memcpy(pUartObj->recvBuf, pUartObj->recvBuf+1, 1);
				}
			}
		}
	}
	pUartObj->recvLen=2;
#endif
	return 0;
}
int GrenadePORT_PhraseByte(BYTE* pRecv)
{
#if 0
	double value;
	if(!bGrenadeSensorOK()){
		sendCommand(CMD_GENERADE_SENSOR_OK);
	}
	killSelfCheckGrenadeAngleTimer();
	startSelfCheckGrenadeAngle_Timer();
	value = (pRecv[2]<<8 | pRecv[3])/100.0;
	
	if(0xFF == pRecv[4]){
		value = -value;//�������Ƕ�Ϊ��
	}else if(0x00 == pRecv[4]){
		//�������Ƕ�Ϊ��
	}else{
		//����쳣
		return -1;
	}
	if(value < -5 || value > 75)
		return -1;
	
	GrenadeAngle = value;
	//д��osd��ֵ�洢�ռ�
	//TracePort �����ǰ�ֶ�
#endif
	return 0;
}



void* GrenadePORT_recvTask(void * prm)
{

//	UartObj* pUartObj=&GrenadeObj.UartPort;
//	BYTE* pRecv= pUartObj->recvBuf;

//	while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) 
	while(1)
	{
	//	OSA_printf("GrenadePORT_recvTask\n");
	//	OSA_waitMsecs(5000);
	#if 0
		pUartObj->recvLen = 0;	
		
		if(0 != GrenadePORT_recvCfg(pUartObj, 2))
		{
			continue;
		}
		if(0 > GrenadePORT_recv(pUartObj, 3))
		{
			continue;
		}
		
		GrenadePORT_send(pUartObj->recvBuf, 5);
		
		GrenadePORT_PhraseByte(pRecv);
	#endif
	}
	return NULL;
}



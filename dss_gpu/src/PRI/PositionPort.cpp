
/*
	��Ǵ����� ����
	�ѽ��յ���ֵ��������㵥Ԫtrajectory_calcular
	��ֱ��д����Ӧ�Ľṹ����

	�����ʽ:
	BYTE     HEX      CONTEXT
	 1      0xAA      ��ͷ
	 2    0x01~0x0A   MSB         
	 3    0x00~0xFF   LSB(X��Ƕ� 0 ~��30)�������0.001/bit
	 2    0x00~0xFF   MSB         
	 3    0x00~0xFF   LSB(Y��Ƕ� 0 ~��30)�������0.001/bit
	 2    0x00~0xFF   MSB         
	 3    0x00~0xFF   LSB(�¶�)�������0.01/bit

	 4    0x00~0xFF   checksum(byte1^ ~ byte7)

	 �ӿ�:   RS422
	 ������: 115200

	 �ο�TRACEPort������������
	��Ҫ����serverPort.c main.c����Ӧ�Ĵ�����������ʼ�������յ����Ʊ�����

	״̬��

*/






//#include "std.h"
//#include "sdkdef.h"
//#include "main.h"
//#include "PVEMsgfunsTab.h"
#include "PositionPort.h"
//#include "byteParser_util.h"
#include "msgDriv.h"

#define abs(a) (((a)>0)?(a):(0-(a)))


extern volatile BOOL tskCapAndDispQuit;
extern int AvtPortSend(BYTE* pSendBuf,int iLen);

static PositionPort_t PositionObj ;
static int ibInit=0;
static BOOL PositionPORT_open();
static void PositionPORT_close();



static double hPositionX=0.0,hPositionY=0.0,Tempre=0.0;

void killSelfCheckDipAngleTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eDipAngle_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eDipAngle_Timer);
	}
#endif
}
void SelfCheckDipAngleTimer_cbFxn(void* cbParam)
{
//	killSelfCheckDipAngleTimer();
//	sendCommand(CMD_DIP_ANGLE_ERR);
}

void startSelfCheckDipAngle_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eDipAngle_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eDipAngle_Timer,SELFCHECK_TIMER,SelfCheckDipAngleTimer_cbFxn,(void*)(0x01));	
	}
#endif
}

double getPlatformPositionX()
{
	return hPositionX;
}
double getPlatformPositionY()
{
	return hPositionY;
}


//DFU_Handle pDUFObj=NULL;
/*
 *  ======== PositionPORT_initial========
 *
 *  This function is used to initial PositionPORT object
 */
PositionPort_Handle PositionPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&PositionObj);
#if 0
	ibInit=1;
	PositionObj.pInter.PortType		= Enum_PositionPort;//Enum_TracPort;
	PositionObj.pInter.OpenPort 	= PositionPORT_open;
	PositionObj.pInter.ClosePort	= PositionPORT_close;
	PositionObj.pInter.Write		= PositionPORT_send;
	PositionObj.pInter.Read			= NULL;
	PositionObj.pInter.Control		= NULL;
	PositionObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	PositionObj.UartPort.uartId		= 0xFF;
	PositionObj.UartPort.uartStat	= eUart_SyncFlag;
	PositionObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	PositionObj.UartPort.recvLen	= 0;

	SDK_SEM_RESET(PositionObj.UartPort.syncObj);

	if(!PositionPORT_open()){
		SDK_SEM_DESTROY(PositionObj.UartPort.syncObj);
		return NULL;
	}
#endif
	return (&PositionObj);
}

/*
 *  ======== PositionPORT_unInitial========
 *
 *  This function is used to uninitial PositionPORT object
 */
void PositionPORT_unInitial(PositionPort_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
//	PositionPORT_close();
}

/*
 *  ======== PositionPORT_unInitial========
 *
 *  This function is used to open PositionPORT
 */
static BOOL PositionPORT_open()
{
#if 0
	UartObj* pUartObj=&PositionObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_POSITION_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1){
		CloseUart(EUART_POSITION_PORT);
		return FALSE;
	}
#endif
	return TRUE;
}

/*
 *  ======== PositionPORT_close========
 *
 *  This function is used to open PositionPORT
 */
static void PositionPORT_close()
{
#if 0
	UartObj* pUartObj= &PositionObj.UartPort;

	if(pUartObj->syncObj!=NULL){
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else CloseUart(pUartObj->uartId);
#endif

}


/*
 *  ======== PositionPORT_send========
 *
 *  This function is used to send data through the PositionPORT
 */
int PositionPORT_send(BYTE* pSendBuf,int iLen)
{

	UartObj* pUartObj=&PositionObj.UartPort;
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
 *  ======== PositionPORT_recv========
 *
 *  This function is used to receive data from uart port
 */
int PositionPORT_recv(UartObj*pUartObj,int iLen)
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

static int PositionPORT_recvCfg(UartObj*pUartObj,int iLen)
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
				if(0xAA == *pCur)
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


BYTE PositionPORT_CheckSum(BYTE* buf)
{
	char sum=0;
	int i;
	for(i=0; i<7; i++){
		sum ^= buf[i];
	}
	
	return sum;
}



int PositionPORT_PhraseBye(BYTE* pRecv)
{

	short PlatformThetaX,PlatformThetaY,Temperature;

	//if(!bDipAngleSensorOK())
	if(1)
	{
		MSGDRIV_send(CMD_DIP_ANGLE_OK,0);
	}
#if 0
	killSelfCheckDipAngleTimer();
	startSelfCheckDipAngle_Timer();
	
	if(pRecv[7] != (PositionPORT_CheckSum(pRecv)))//У���
		return -1;

	PlatformThetaX = (pRecv[1]<<8|pRecv[2]);//*0.001*16.67; osd��ʾʱ�ټ���
	PlatformThetaY = (pRecv[3]<<8|pRecv[4]);//*0.001*16.67;
	Temperature    = (pRecv[5]<<8|pRecv[6]);//*0.01;

	if((abs(PlatformThetaX) > 30000)||(abs(PlatformThetaY) >30000))
		return -1;
	hPositionX = PlatformThetaX*0.001;//*16.67			
	hPositionY = PlatformThetaY*0.001;//*16.67
	Tempre = Temperature*0.01;
	
	//д��osd��ֵ�ڴ�ռ�
	//TracePort �����ǰ�ֶ�
#endif
	return 0;
}



void * PositionPORT_recvTask(void* prm)
{
	//UartObj* pUartObj=&PositionObj.UartPort;
	//BYTE* pRecv= pUartObj->recvBuf;
	//while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) //���Task����ʱ����Ҫ��main.c init.c�м���
	while(1)
	{
		//OSA_printf("PositionPORT_recvTask\n");
	//	OSA_waitMsecs(5000);
	#if 0
		pUartObj->recvLen = 0;	
		
		if(0 != PositionPORT_recvCfg(pUartObj, 1))
		{
			continue;
		}
		if(0 > PositionPORT_recv(pUartObj, 7))
		{
			continue;
		}
		
		PositionPORT_send(pUartObj->recvBuf, 8);
	#endif
		//PositionPORT_PhraseBye(pRecv);
		PositionPORT_PhraseBye(NULL);
	
	}

	return NULL;
}




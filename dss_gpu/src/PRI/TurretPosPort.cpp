#include <osa.h>
#include "TurretPosPort.h"
#include "msgDriv.h"
#include "statCtrl.h"

/*
	������λ�Ƕȴ���������
	BYTE     HEX      CONTEXT
	1��2                ��ͷ
	 3    0x00~0xFF   MSB
	 4    0x00~0xFF   LSB(0 ~ ��180)
	 5    0x00/0xFF   0x00�Ƕ�Ϊ��0xFF�Ƕ�Ϊ��     

	 �ӿ�:   RS422
	 ������: 9600

	 �ο�TRACEPort������������

*/

//#include "std.h"
//#include "sdkdef.h"
//#include "main.h"
//#include "severPort.h"
//#include "PVEMsgfunsTab.h"
//#include "statCtrl.h"
//#include "byteParser_util.h"

#define COUNT_TURRET_VELO 200
static TurretPosPort_t TurretPosObj;
static int ibInit=0;
static unsigned int g_measure_dist_time = 0;
extern int turretTimer;

volatile bool valid_measure = 0;

C_Thetas gTurretTheta= {0.0,0};

static double gTurretVelocity[COUNT_TURRET_VELO]={0};
static int counter=-1;
static BOOL TurretPosPORT_open();
static void TurretPosPORT_close();


void killSelfCheckPosAngleTimer()
{	
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(ePosAngle_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(ePosAngle_Timer);
	}
	return ;
}

void SelfCheckPosAngleTimer_cbFxn(void* cbParam)
{
	killSelfCheckPosAngleTimer();
	MSGDRIV_send(CMD_POSITION_SENSOR_ERR,0);
}

void startSelfCheckPosAngle_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(ePosAngle_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(ePosAngle_Timer,SELFCHECK_TIMER);	
	}
	return ;
}

void markMeasure_dist_Time()
{
	//g_measure_dist_time = turretTimer;
	g_measure_dist_time = OSA_getCurTimeInMsec();
}

static double Measure_dist_time_delta()
{
	//return turretTimer - g_measure_dist_time;
	return (OSA_getCurTimeInMsec()- g_measure_dist_time);
}

void computeMeanVar( double *src, double *mean, double *var, int size )
{
	double vsquared;
	int i;
	double *psrc = src;
#if 1
	vsquared = 0.0;
	*mean = 0.0;
	for ( i = 0; i < size; i++ )
	{
            *mean += psrc[i];
            vsquared += psrc[i] * psrc[i];
	}
	
	*mean /= (double) size; /* mean */
	vsquared /= (double) size; /* mean (x^2) */
	*var = ( vsquared - (*mean * *mean) );//\u6c42\u65b9\u5dee
	*var = (double)sqrt(*var); /* var */
#endif
	return ;
}

double getTurretTheta()
{
	return gTurretTheta.theta;
}

BOOL isTurretVelocityValid()
{
	//return (Measure_dist_time_delta() >= 2000) ;//&& (counter>0);
	if(Measure_dist_time_delta() >= 2000) //&& (counter>0);
	{
		valid_measure = 1;
		return 1;
	}
	else
		return 0;
}

void resetTurretVelocityCounter()
{
	counter = -1;
}

// calculate the avarage velo in the last 2 seconds
int getAverageVelocity(double* value)
{
	
	int i, dataCount = 0, count = 0;
	double mean, var;
	*value = 0;
#if 1
	if(counter <= 0)
		return -1;
	dataCount = counter> COUNT_TURRET_VELO ? COUNT_TURRET_VELO : counter;
	
	computeMeanVar(gTurretVelocity,&mean, &var, dataCount);
	for(i=0; i<dataCount; i++)
	{
		if( abs(gTurretVelocity[i] - mean) <= var)
		{
			*value += gTurretVelocity[i];
			count ++;
		}
	}

	if(count == 0)
		return -1;
	else
		*value = (*value)/count;
#endif	
	return 0;
}
//DFU_Handle pDUFObj=NULL;
/*
 *  ======== TurretPosPORT_initial========
 *
 *  This function is used to initial TurretPosPORT object
 */
TurretPosPORT_Handle TurretPosPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&TurretPosObj);
#if 0
	ibInit=1;
	TurretPosObj.pInter.PortType	= Enum_TurretPosPort;//Enum_TracPort;
	TurretPosObj.pInter.OpenPort 	= TurretPosPORT_open;
	TurretPosObj.pInter.ClosePort	= TurretPosPORT_close;
	TurretPosObj.pInter.Write		= TurretPosPORT_send;
	TurretPosObj.pInter.Read		= NULL;
	TurretPosObj.pInter.Control		= NULL;
	TurretPosObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	TurretPosObj.UartPort.uartId	= 0xFF;
	TurretPosObj.UartPort.uartStat	= eUart_SyncFlag;
	TurretPosObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	TurretPosObj.UartPort.recvLen	= 0;

	SDK_SEM_RESET(TurretPosObj.UartPort.syncObj);

	if(!TurretPosPORT_open()){
		SDK_SEM_DESTROY(TurretPosObj.UartPort.syncObj);
		return NULL;
	}
#endif
	return (&TurretPosObj);
}

/*
 *  ======== TurretPosPORT_unInitial========
 *
 *  This function is used to uninitial TurretPosPORT object
 */
void TurretPosPORT_unInitial(TurretPosPORT_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
//	TurretPosPORT_close();
}

/*
 *  ======== TurretPosPORT_unInitial========
 *
 *  This function is used to open TurretPosPORT
 */
static BOOL TurretPosPORT_open()
{
#if 0
	UartObj* pUartObj=&TurretPosObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_TURRETPOS_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1){
		CloseUart(EUART_TURRETPOS_PORT);
		return FALSE;
	}
#endif
	return TRUE;
}

/*
 *  ======== TurretPosPORT_close========
 *
 *  This function is used to open TurretPosPORT
 */
static void TurretPosPORT_close()
{
	UartObj* pUartObj= &TurretPosObj.UartPort;
#if 0
	if(pUartObj->syncObj!=NULL){
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else CloseUart(pUartObj->uartId);
#endif
}


/*
 *  ======== TurretPosPORT_send========
 *
 *  This function is used to send data through the TurretPosPORT
 */
int TurretPosPORT_send(BYTE* pSendBuf,int iLen)
{
	UartObj* pUartObj=&TurretPosObj.UartPort;
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
 *  ======== TurretPosPORT_recv========
 *
 *  This function is used to receive data from uart port
 */
static int TurretPosPORT_recv(UartObj*pUartObj,int iLen)
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

static int TurretPosPORT_recvCfg(UartObj*pUartObj,int iLen)
{
	BYTE *pCur = pUartObj->recvBuf;
	int ilen = 0, stat;
#if 0
	while(TRUE){
		if(GetDataFromUart(pUartObj->uartId,pCur,1, &stat)!=1){
			return -1;
		}else{
			ilen ++;
			pCur ++;
			if(ilen == 2){
				if(0x8FF8 == stoh2(pUartObj->recvBuf))
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
void TurretPosPORT_PhraseBye(unsigned char *pRecv)
{
	static double value=0.0;
	int current_ms;

	//if(!bPositionSensorOK())
	if (1)
	{
		MSGDRIV_send(CMD_POSITION_SENSOR_OK,0);
	}
#if 0
	killSelfCheckPosAngleTimer();
	startSelfCheckPosAngle_Timer();
	value = ((pRecv[2]<<8)|pRecv[3])*0.01;//;
	if(0xFF == pRecv[4]){
		value = -value;//�������Ƕ�Ϊ��
	}else if(0x00 == pRecv[4]){
		//�������Ƕ�Ϊ��
	}else{
		return;//����쳣
	}
		
	if(abs(value)>180)
		return ;
	
	current_ms = turretTimer;			
	if(isBattleTriggerMeasureVelocity()||isAutoTriggerMeasureVelocity())
	{
		double delta;				// : save velocity to Array				
		delta = value - gTurretTheta.theta;

		if(delta > 180)
			delta -= 360;
		else if(delta < -180)
			delta += 360;
		if(counter >= 0 && (current_ms - gTurretTheta.msecond > 5))
		{
			gTurretVelocity[counter%200] = delta*1000/ (current_ms - gTurretTheta.msecond);//д��osd��ֵ�洢�ռ�
			counter++;
		}
		if(counter < 0)
			counter++;
	}	
	
	gTurretTheta.theta = value;
	gTurretTheta.msecond = current_ms;
#endif
}


void * TurretPosPORT_recvTask(void * prm)
{
	
	//UartObj* pUartObj=&TurretPosObj.UartPort;
//	BYTE* pSend= pUartObj->sendBuf;	
	//BYTE* pRecv= pUartObj->recvBuf;

	//while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) //���Task����ʱ����Ҫ��main.c init.c�м���
	while(1)
	{
		//OSA_printf("TurretPosPORT_recvTask\n");
	//	OSA_waitMsecs(3000);
	#if 0
		if(0 != TurretPosPORT_recvCfg(pUartObj, 2))
		{
			pUartObj->recvLen = 0;	
			continue;
		}
		if(0 > TurretPosPORT_recv(pUartObj, 3))
		{
			pUartObj->recvLen = 0;	
			continue;
		}
		
		TurretPosPORT_send(pUartObj->recvBuf, 5);
	#endif	
		//TurretPosPORT_PhraseBye(pRecv);
	//TurretPosPORT_PhraseBye(NULL);
	//	pUartObj->recvLen = 0;	
	}
	return NULL;
}





/*
	��ǹ���ᴫ��������

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
#include <osa.h>
#include "permanentStorage.h"
#include "MachGunPort.h"
#include "msgDriv.h"


#if 1
#define COUNT_DIP_VELO 200



C_Thetas MachGunAngle={0.0,0};
static double gDipVelocity[COUNT_DIP_VELO]={0};
static int counter=-1;
static unsigned int g_measure_dist_time = 0;
extern int turretTimer;

#else
static double MachGunAngle=0.0;
#endif
static int ibInit=0;

static MachGunPort_t MachGunObj;

static void MachGunPORT_close();
static BOOL MachGunPORT_open();
extern void computeMeanVar( double *src, double *mean, double *var, int size );

void killSelfCheckMachGunAngleTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eMachGunAngle_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eMachGunAngle_Timer);
	}
#endif
}
void SelfCheckMachGunAngleTimer_cbFxn(void* cbParam)
{
//	killSelfCheckMachGunAngleTimer();
//	sendCommand(CMD_MACHINEGUN_SENSOR_ERR);
}

void startSelfCheckMachGunAngle_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eMachGunAngle_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eMachGunAngle_Timer,SELFCHECK_TIMER,SelfCheckMachGunAngleTimer_cbFxn,(void*)(0x01));	
	}
#endif
}

void markMeasure_dip_Time()
{
	g_measure_dist_time =OSA_getCurTimeInMsec();
}
void resetDipVelocityCounter()
{
	counter = -1;
}
double getMachGunAngleAbs()
{
	//printf("********************************************\n");
	//printf("MachGunAngle.theta = %f\n",MachGunAngle.theta);
	return MachGunAngle.theta;
}

double getMachGunAngle()
{
	return MachGunAngle.theta - gMachineGun_ZCTable.Angle;
}

// calculate the avarage velo in the last 2 seconds
int getAverageDipVelocity(double* value)
{
#if 01
	int i, dataCount = 0, count = 0;
	double mean, var;
	*value = 0;
	if(counter <= 0)
		return -1;
	dataCount = counter> COUNT_DIP_VELO ? COUNT_DIP_VELO : counter;
	
	computeMeanVar(gDipVelocity,&mean, &var, dataCount);
	for(i=0; i<dataCount; i++)
	{
		if( abs(gDipVelocity[i] - mean) <= var)
		{
			*value += gDipVelocity[i];
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
 *  ======== MachGunPORT_initial========
 *
 *  This function is used to initial MachGunPORT object
 */
MachGunPORT_Handle MachGunPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&MachGunObj);
#if 0
	ibInit=1;
	MachGunObj.pInter.PortType	= Enum_MachGunPort;//Enum_TracPort;
	MachGunObj.pInter.OpenPort 	= MachGunPORT_open;
	MachGunObj.pInter.ClosePort	= MachGunPORT_close;
	MachGunObj.pInter.Write		= MachGunPORT_send;
	MachGunObj.pInter.Read		= NULL;
	MachGunObj.pInter.Control		= NULL;
	MachGunObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	MachGunObj.UartPort.uartId	= 0xFF;
	MachGunObj.UartPort.uartStat	= eUart_SyncFlag;
	MachGunObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	MachGunObj.UartPort.recvLen		= 0;

	SDK_SEM_RESET(MachGunObj.UartPort.syncObj);

	if(!MachGunPORT_open())
	{
		SDK_SEM_DESTROY(MachGunObj.UartPort.syncObj);
		return NULL;
	}

	return (&MachGunObj);
#endif
}

/*
 *  ======== MachGunPORT_close========
 *
 *  This function is used to open MachGunPORT
 */
static void MachGunPORT_close()
{
#if 0
	UartObj* pUartObj= &MachGunObj.UartPort;

	if(pUartObj->syncObj!=NULL)
	{
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else CloseUart(pUartObj->uartId);
#endif
}


/*
 *  ======== MachGunPORT_unInitial========
 *
 *  This function is used to uninitial MachGunPORT object
 */
void MachGunPORT_unInitial(MachGunPORT_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
//	MachGunPORT_close();
}

/*
 *  ======== MachGunPORT_unInitial========
 *
 *  This function is used to open MachGunPORT
 */
static BOOL MachGunPORT_open()
{
#if 0
	UartObj* pUartObj=&MachGunObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_MACHGUN_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1)
	{
		CloseUart(EUART_MACHGUN_PORT);
		return FALSE;
	}
#if 0
	CmdEntrySet(eCT_Reboot,	TRACVPORT_recvSysRebootMsg,0);						//����
	CmdEntrySet(eCT_Save ,	TRACVPORT_recvSysCfgSaveMsg,0);						//����������Ϣ
	CmdEntrySet(eCT_SystemConfigRequest, TRACVPORT_recvSysCfgReqMsg,0);			//����������Ϣ
	CmdEntrySet(eCT_SystemConfig,TRACVPORT_recvSysCfgUpdateMsg,0);				//�޸�������Ϣ
	CmdEntrySet(eCT_Test, TRACVPORT_recvSysCheckMsg,0);							//ϵͳ�Լ�
	
	CmdEntrySet(eCT_DataOutEnableRequest,TRACVPORT_recvDataOutEnableMsg,0);		//��ݶ˿�ʹ��״̬����
	CmdEntrySet(eCT_DataOutEnable,TRACVPORT_recvDataOutEnableMsg,0);			//��ݶ˿�ʹ��

	CmdEntrySet(eCT_AVTMain,TRACVPORT_recvAvtMainMsg,0);						//AVTά���˿�
	CmdEntrySet(eCT_UserData205, TRACVPORT_recvDramMsg,1);						//205���ģ��
	CmdEntrySet(eCT_Update, TRACVPORT_recvDspVersionUpdateMsg,1);				//��DSP����̼�
#endif
	return TRUE;
#endif
}



/*
 *  ======== MachGunPORT_send========
 *
 *  This function is used to send data through the MachGunPORT
 */
int MachGunPORT_send(BYTE* pSendBuf,int iLen)
{
	UartObj* pUartObj=&MachGunObj.UartPort;
	int iSendlen=0, iCur;
#if 0
	if(iLen>250)
	{
		LOGMSGTRACE(LOCALDEBUG,"Len too large:%d\n",iLen,2,3,4);
	}

	while(pSendBuf!=NULL && iLen!=0)
	{
		
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
 *  ======== MachGunPORT_recv========
 *
 *  This function is used to receive data from uart port
 */
int MachGunPORT_recv(UartObj*pUartObj,int iLen)
{
	int iRcvLen=0,iCurLen=0;
#if 0
	BYTE *pCurBuf = pUartObj->recvBuf + pUartObj->recvLen;
	int stat = 0;
	if(!iLen||pUartObj==NULL)
	{
		return -1;
	}
	while (TRUE)
	{
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



static int MachGunPORT_recvCfg(UartObj*pUartObj,int iLen)
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
int MachGunPORT_PhraseBye(unsigned char *buf)
{

	static double value=0.0;
	BYTE* pRecv= buf;
	int current_ms;
	
	//if( !bMachineGunSensorOK())
	if(1)
	{
		MSGDRIV_send(CMD_MACHINEGUN_SENSOR_OK,0);
	}
#if 0
	killSelfCheckMachGunAngleTimer();
	startSelfCheckMachGunAngle_Timer();
	value = (pRecv[2]<<8 | pRecv[3])/100.0;
	if(0xFF == pRecv[4]){
		value = -value;//�������Ƕ�Ϊ��
	}else if(0x00 == pRecv[4]){
		//�������Ƕ�Ϊ��
	}else{
		//����쳣
		return -1;
	}
	if(value < -5 ||value > 75)
		return -1;

		current_ms = turretTimer;			
	if(isBattleTriggerMeasureVelocity()||isAutoTriggerMeasureVelocity())
	{
		double delta;				// : save velocity to Array				
		delta = value - MachGunAngle.theta;

		if(delta > 180)
			delta -= 360;
		else if(delta < -180)
			delta += 360;
		if(counter >= 0 && (current_ms - MachGunAngle.msecond > 5))
		{
			gDipVelocity[counter%200] = delta*1000/ (current_ms - MachGunAngle.msecond);//д��osd��ֵ�洢�ռ�
			counter++;
		}
		if(counter < 0)
			counter++;
	}	
	MachGunAngle.theta = value;
	MachGunAngle.msecond = current_ms;
//	MachGunAngle = value;
	//д��osd��ֵ�洢�ռ�
	//TracePort �����ǰ�ֶ�
#endif
	return 0;
}



void* MachGunPORT_recvTask(void* prm)
{

	//UartObj* pUartObj=&MachGunObj.UartPort;
	//BYTE* pSend= pUartObj->sendBuf;	

	//while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) //���Task����ʱ����Ҫ��main.c init.c�м���
	while(1)
	{
		//OSA_printf("MachGunPORT_recvTask\n");
	//	OSA_waitMsecs(2000);
	#if 0
		pUartObj->recvLen = 0;	
	
		if(0 != MachGunPORT_recvCfg(pUartObj, 2))
		{
			continue;
		}
		if(0 > MachGunPORT_recv(pUartObj, 3))
		{
			continue;
		}

		MachGunPORT_send(pUartObj->recvBuf, 5);
	#endif	
		//MachGunPORT_PhraseBye(pUartObj->recvBuf);
		MachGunPORT_PhraseBye(NULL);
	
	}
	return NULL;
}


/*
	�����������ư巢�͵Ŀ�������
	����ݸ���ȷ����ƽ̨��ֵ����̨ת�������и���

	ƽ̨������Ϣ����:
		���ٿ�ʼ - ��¼�Ƕ���Ϣ

		�������� - �������

	
	
*/

//#include "std.h"
//#include "sdkdef.h"
//#include "Main.h"
//#include "PVEMsgfunsTab.h"
//#include "statCtrl.h"
//#include "byteParser_util.h"
#include "spiH.h"

#include "WeaponCtrl.h"
#include "osdProcess.h"
#include "statCtrl.h"
#include "msgDriv.h"
#include "UartCanMessage.h"
#if 0
#include "MachGunPort.h"
#include "GrenadePort.h"
#include "LaserPort.h"
#include "statCtrl.h"
#include "msgDriv.h"
#endif

#define CAN_ID_PANEL 	(0x0002)
#define CAN_ID_TURRET	(0x02AC)
#define CAN_ID_MACHGUN (0x02B7)
#define CAN_ID_GRENADE	(0x2C2)
#define CODE_MACHGUN 	(0x37)
#define CODE_GRENADE 	(0x42)
#define CODE_TURRET   	(0x2C)
#define WAIT_DELAY		20*5000

extern int turretTimer;



enum {
	CAN_DEVIC_PANEL,
	CAN_DEVICE_TURRET,
	CAN_DEVICE_MACHGUN,
	CAN_DEVICE_GRENADE
}CAN_DEVICE_TYPE;

BYTE FrameBuf0[7]={0xA0,0x00,0xFF,0xFF,0xFF,0xFF,0x00};
BYTE FrameBuf1[5]={0xA1,0x10,0x44,0xFF,0x00};
BYTE FrameBuf3[4]={0xA3,0x0F,0x00,0x00};
//SIGNAL signal;
static WeaponCtrlPort_t WeaponCtrlObj;
static int ibInit=0;
int servoInit=1;
static BOOL WeaponCtrlPORT_open();
static void WeaponCtrlPORT_close();

BOOL bTraceSend=FALSE;
//static PVE_MSG *pOutMsg=NULL;

static BYTE CANSendbuf[10]={0x01,0x04,0xB1,0x00,};
static BYTE CANSendFrame1[10]={0x01,0x04,0xB0,0x00,};
static BYTE CANSendFrame2[10]={0x01,0x04,0xB2,0x00,};

static char WeaponCtrl[16]={0};
static 	unsigned short panoAngleV=0,panoAngleH=0;



void absPosRequest(BYTE code)
{
	char SERVOPOS[6]   ={0x03,0x00,0x50,0x58,0x00,0x00};
	SERVOPOS[1] = code;
	SendCANBuf(SERVOPOS, sizeof(SERVOPOS));
	usleep(500);
}

void startServo(BYTE code)
{
	char start[6] = {0x03,0x00,0x42,0x47,0x00,0x00};
	start[1] = code;
	SendCANBuf(start, sizeof(start));
}

void startServoServer(BYTE code)
{
#if 1
	char buf[4] = {0x00,0x00,0x01,0x00};
	char MOTOR0[10]	={0x03,0x00,0x4D,0x4F,0x00,0x00,0x00,0x00,0x00,0x00};
	char MOTOR1[10]	={0x03,0x00,0x4D,0x4F,0x00,0x00,0x01,0x00,0x00,0x00};
	char MODE[10]	={0x03,0x00,0x55,0x4D,0x00,0x00,0x05,0x00,0x00,0x00};
	SendCANBuf(buf, sizeof(buf));
	usleep(WAIT_DELAY);
	MOTOR0[1] = code;
	SendCANBuf(MOTOR0, sizeof(MOTOR0));
	usleep(WAIT_DELAY);
	MODE[1] = code;
	SendCANBuf(MODE, sizeof(MODE));
	usleep(WAIT_DELAY);
	MOTOR1[1] = code;
	SendCANBuf(MOTOR1, sizeof(MOTOR1));
	usleep(WAIT_DELAY);
	absPosRequest(code);
	usleep(WAIT_DELAY);
	startServo(code);
	usleep(WAIT_DELAY);
#endif
}


void killServoCheckTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eServoCheck_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eServoCheck_Timer);
	}
}


void startServoCheck_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eServoCheck_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eServoCheck_Timer,SERVOCHECK_TIMER);	
	}
}


void killF6Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF6_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eF6_Timer);
	}
}

void startF6_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF6_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eF6_Timer,FN_TIMER);	
	}
}

void killF5Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF5_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eF5_Timer);
	}
}

void startF5_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF5_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eF5_Timer,FN_TIMER);	
	}
}

void  killF3Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF3_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eF3_Timer);
	}
}

void startF3_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eF3_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eF3_Timer,FN_TIMER);	
	}
}

BOOL isTimerAlive(UINT id)
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	return (pCtrlTimer->GetTimerStat(id)==eTimer_Stat_Run);
}
void killSelfCheckPosServoTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(ePosServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(ePosServo_Timer);
	}
}

void startSelfCheckPosServo_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(ePosServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(ePosServo_Timer,SELFCHECK_TIMER);	
	}
}
void killSelfCheckMachGunServoTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eMachGunServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eMachGunServo_Timer);
	}
}

void startSelfCheckMachGunServo_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eMachGunServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eMachGunServo_Timer,SELFCHECK_TIMER);	
	}
}

void killSelfCheckGrenadeServoTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eGrenadeServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eGrenadeServo_Timer);
	}
}

void startSelfCheckGrenadeServo_Timer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eGrenadeServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eGrenadeServo_Timer,SELFCHECK_TIMER);	
	}
}

BOOL isMachGunUpLocked()
{
	return BIT1(BYTE6(FrameBuf0));
}
BOOL isMachGunDownLocked()
{
	return BIT0(BYTE6(FrameBuf0));
}
BOOL isGrenadeUpLocked()
{
	return BIT5(BYTE6(FrameBuf0));
}
BOOL isGrenadeDownLocked()
{
	return BIT4(BYTE6(FrameBuf0));
}
BOOL isMachGunPosLocked()
{
	return BIT3(BYTE6(FrameBuf0));
}
BOOL isGrenadePosLocked()
{
	return BIT7(BYTE6(FrameBuf0));
}

BYTE getFrameA3Byte1()
{
	return FrameBuf3[1];
}
BYTE getFrameA3Byte2()
{
	return FrameBuf3[2];
}
BYTE getFrameA3Byte3()
{
	return FrameBuf3[3];
}
BOOL isGunIdentify()
{
	return BIT5(BYTE4(FrameBuf1));
}
BOOL isIdentifyGas()
{
	return BIT4(BYTE4(FrameBuf1));
}

char* getWeaponCtrlA1()
{
#if 1
	memset(WeaponCtrl,0,sizeof(WeaponCtrl));
	sprintf(WeaponCtrl,"%02X%02X%02X%02X",FrameBuf1[0],FrameBuf1[1],FrameBuf1[2],FrameBuf1[3]);
	return WeaponCtrl;
#endif
}
char* getFulScrA0()
{
	memset(WeaponCtrl,0,sizeof(WeaponCtrl));
	sprintf(WeaponCtrl,"%02X%02X %02X%02X",FrameBuf0[2],FrameBuf0[3],FrameBuf0[4],FrameBuf0[5]);

	return WeaponCtrl;
}

float getpanoAngleV()
{
	return panoAngleV/100.0;
}
float getpanoAngleH()
{
	if(panoAngleH<27000)
		return -panoAngleH/100.0;
	else
		return -(panoAngleH-36000)/100.0;
}

void setTraceModeSwitch(BOOL context)
{
	bTraceSend = context;
}

void requstServoContrl()
{
	CANSendbuf[3] = (CANSendbuf[3]&0xF0) |0x05 ;
}

void releaseServoContrl(void)
{
	setServoAvailable(FALSE);
	
	CANSendbuf[3] = (CANSendbuf[3]&0xF0);
}

void setGrenadeInPositonFlag(void)
{
	CANSendbuf[3] = (CANSendbuf[3]&0x0F) |0x30 ;
}

void resetGrenadeInPositionFlag(void)
{
	CANSendbuf[3] = (CANSendbuf[3]&0x0F);
}

void processCMD_SERVOTIMER_MACHGUN(LPARAM lParam)
{
	absPosRequest(CODE_GRENADE);
	absPosRequest(CODE_MACHGUN);
	absPosRequest(CODE_TURRET);
}

void processCMD_TIMER_SENDFRAME0(LPARAM lParam)
{
	short TempAngle;
	
	if(isBootUpMode()&&isBootUpSelfCheck())
		return ;
	//todo: check Sensor and Servo is All OK
	TempAngle = (short)(getMachGunAngle() *100.0);
	CANSendbuf[4] = ((TempAngle>>8)&0xFF);
	CANSendbuf[5] = (TempAngle&0xFF);
	
	TempAngle = (short)(getGrenadeAngle()*100.0);
	CANSendbuf[6] = ((TempAngle>>8)&0xFF);
	CANSendbuf[7] = (TempAngle&0xFF);

	TempAngle = (short)(getTurretTheta()*100.0);
	CANSendbuf[8] = ((TempAngle>>8)&0xFF);
	CANSendbuf[9] = (TempAngle&0xFF);
	
	SendCANBuf((char *)CANSendbuf, sizeof(CANSendbuf));
}

void processCMD_TIMER_SENDFRAME1(LPARAM lParam)//����֡1
{
	short TempAngle;
	
	if(isBootUpMode()&&isBootUpSelfCheck())
		return ;

	TempAngle = (short)(getPlatformPositionX() *100.0);
	CANSendFrame1[3] = ((TempAngle>>8)&0xFF);
	CANSendFrame1[4] = (TempAngle&0xFF);
	
	TempAngle = (short)(getPlatformPositionY() *100.0);
	CANSendFrame1[5] = ((TempAngle>>8)&0xFF);
	CANSendFrame1[6] = (TempAngle&0xFF);

	TempAngle = (short)(getLaserDistance() *100.0);
	CANSendFrame1[7] = ((TempAngle>>8)&0xFF);
	CANSendFrame1[8] = (TempAngle&0xFF);
	
	SendCANBuf((char *)CANSendFrame1, 10);
}

void processCMD_TIMER_SENDFRAME2(LPARAM lParam)
{
	if(isBootUpMode()&&isBootUpSelfCheck())
		return ;

	(BYTE9(CANSendFrame2)) &= 0XFE;
	(BYTE9(CANSendFrame2)) |= (0XFE | bMachineGunSensorOK());//5.8�Ƕȴ�����״̬
	(BYTE9(CANSendFrame2)) &= 0XFD;
	(BYTE9(CANSendFrame2)) |= (0XFD | (bGrenadeSensorOK()<<1));//35��Ƕȴ�����״̬
	(BYTE9(CANSendFrame2)) &= 0XFB;
	(BYTE9(CANSendFrame2)) |= (0XFB | (bPositionSensorOK()<<2));//ˮƽ�Ƕȴ�����״̬
	(BYTE9(CANSendFrame2)) &= 0XF7;
	(BYTE9(CANSendFrame2)) |= (0XF7 | (bDipAngleSensorOK()<<3));//��б�Ƕȴ�����״̬
	(BYTE9(CANSendFrame2)) &= 0XEF;
//	(BYTE9(CANSendFrame2)) |= ;//��׼��״̬ todo value
	(BYTE9(CANSendFrame2)) &= 0XDF;
	(BYTE9(CANSendFrame2)) |= (0XDF | (/*bVideoOK()*/1<<5));//��Ƶ����״̬
	
	SendCANBuf((char *)CANSendFrame1, 10);
}

void processCMD_TRACE_SENDFRAME0(LPARAM lParam)
{
#if 0
	int i;
	BYTE* buf = (BYTE*)pOutMsg;
	pOutMsg->head.SyFlag		= 0xAA;
	pOutMsg->head.eCmdType		= eCT_Angle_MsgReply;
	pOutMsg->head.uiVersion		= SOFTWARE_VERSION;
	pOutMsg->head.uiRequestId 	= 1;

	memcpy(pOutMsg->UN_MSG.data,CANSendbuf,sizeof(CANSendbuf));
	pOutMsg->head.uiSize = sizeof(PVE_MSG_HEADER)+sizeof(CANSendbuf)+1;
	
	buf[pOutMsg->head.uiSize-1]=0x0;
	
	for(i=0; i<pOutMsg->head.uiSize-1; i++)
	{
		buf[pOutMsg->head.uiSize-1] +=buf[i];
	}

	TRACEPORT_send(buf,pOutMsg->head.uiSize);
#endif
}

void processCMD_SERVO_INIT(LPARAM lParam)
{
	startServoServer(CODE_GRENADE);
	startServoServer(CODE_MACHGUN);
	startServoServer(CODE_TURRET);
}

Fov_Type getFovState()
{
	return (BIT3_2(BYTE2(FrameBuf1)))?(FOV_SMALL):(FOV_LARGE);
}

void setSysWorkMode()
{
	BYTE1(FrameBuf1) &= ~0x0C;
}

void setJoyStickStat(bool stat)
{
#if 1
	if(stat)
		FrameBuf3[1] = FrameBuf3[1]|0x10 ;
	else
		FrameBuf3[1] = FrameBuf3[1]&0xEF ;
#endif
}

void  updateBulletType(int type)
{
	BYTE1(FrameBuf1) &= 0x8F;
	BYTE1(FrameBuf1) |= ((type&0x03) + 1)<<4;
}

int getBulletType()
{
#if 1
	int i = BIT6_4(BYTE1(FrameBuf1));
	return (i > 0)? i:1 ;
#endif
	return 1;
}


 void startCANSendTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eCAN_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eCAN_Timer,CAN_TIMER);	
	}
}

 void killCANSendTimer()
 {
 	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eCAN_Timer)==eTimer_Stat_Run)
	{
		pCtrlTimer->KillTimer(eCAN_Timer);	
	}
 }

 
//DFU_Handle pDUFObj=NULL;
/*
 *  ======== WeaponCtrlPORT_initial========
 *
 *  This function is used to initial WeaponCtrlPORT object
 */
WeaponCtrlPORT_Handle WeaponCtrlPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int))
{
	if(ibInit)
		return (&WeaponCtrlObj);
#if 0
	ibInit=1;
	WeaponCtrlObj.pInter.PortType		= Enum_WeaponCtrlPort;//Enum_TracPort;//
	WeaponCtrlObj.pInter.OpenPort 	= WeaponCtrlPORT_open;
	WeaponCtrlObj.pInter.ClosePort		= WeaponCtrlPORT_close;
	WeaponCtrlObj.pInter.Write		= WeaponCtrlPORT_send;
	WeaponCtrlObj.pInter.Read		= NULL;
	WeaponCtrlObj.pInter.Control		= NULL;
	WeaponCtrlObj.pInter.MuxPortRecv	= NULL;

	WeaponCtrlObj.UartPort.uartId		= 0xFF;
	WeaponCtrlObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	WeaponCtrlObj.UartPort.recvLen	= 0;

	if(!WeaponCtrlPORT_open()){
		SDK_SEM_DESTROY(WeaponCtrlObj.UartPort.syncObj);
		return NULL;
	}
	
	//pOutMsg = SDK_MALLOC(sizeof(PVE_MSG)+10, EXTERNALHEAP);
#endif	
	return (&WeaponCtrlObj);
}

/*
 *  ======== TurretPosPORT_unInitial========
 *
 *  This function is used to uninitial TurretPosPORT object
 */
void WeaponCtrlPORT_unInitial(WeaponCtrlPORT_Handle handle)
{
	if(!ibInit)
		return;	
	ibInit=0;
//	WeaponCtrlPORT_close();
}

/*
 *  ======== TurretPosPORT_unInitial========
 *
 *  This function is used to open TurretPosPORT
 */
static BOOL WeaponCtrlPORT_open()
{
#if 0
	UartObj* pUartObj=&WeaponCtrlObj.UartPort;
	pUartObj->uartId=OpenUart(EUART_WEAPON_PORT,&uartParams,MAXUARTBUFLEN,512,EXTERNALHEAP);

	if(pUartObj->uartId==-1){
		CloseUart(EUART_WEAPON_PORT);
		return FALSE;
	}
	return TRUE;
#endif
}

/*
 *  ======== WeaponCtrlPORT_close========
 *
 *  This function is used to open WeaponCtrlPORT
 */
static void WeaponCtrlPORT_close()
{
#if 0
	UartObj* pUartObj= &WeaponCtrlObj.UartPort;

	if(pUartObj->syncObj!=NULL)
	{
		SDK_SEM_POST(pUartObj->syncObj);
		CloseUart(pUartObj->uartId);
		SDK_SEM_DESTROY(pUartObj->syncObj);
	}else 
	{
		CloseUart(pUartObj->uartId);
	}
#endif
}


/*
 *  ======== WeaponCtrlPORT_send========
 *
 *  This function is used to send data through the TurretPosPORT
 */
int WeaponCtrlPORT_send(BYTE* pSendBuf,int iLen)
{
#if 0
	UartObj* pUartObj=&WeaponCtrlObj.UartPort;
	int iSendlen=0, iCur;
	static BOOL stat=TRUE;
	while(!stat)
		MYBOARD_waitusec(50);
	stat = FALSE;
	if(iLen>250){
		LOGMSGTRACE(LOCALDEBUG,"Len too large:%d\n",iLen,2,3,4);
	}

	while(pSendBuf!=NULL && iLen!=0){
		
		iCur=SendDataToUart(pUartObj->uartId, pSendBuf+iSendlen,iLen,TRUE);
		if(iCur<0)
			break;
		else
			iSendlen+=iCur;
		iLen-=iCur;

	}
	MYBOARD_waitusec(5000*2);
	stat = TRUE;

	return iSendlen;
#endif
}


/*
 *  ======== WeaponCtrlPORT_recv========
 *
 *  This function is used to receive data from uart port
 */
int WeaponCtrlPORT_recv(UartObj*pUartObj,int iLen)
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
}
int ReadCan(UartObj*pUartObj,int iLen)
{
#if 0
	int stat=0;
	BYTE *pCurBuf = pUartObj->recvBuf;
	if(!iLen||pUartObj==NULL){
		return -1;
	}

	stat = GetDataFromUart(pUartObj->uartId, pCurBuf, iLen, &stat);
	switch (pCurBuf[2]){
	case Frame_Type0:
		if(stat != 8)
			return -1;
		break;
	case Frame_Type1:
		if(stat != 6)
			return -1;
		break;
	case Frame_Type2:
		if(stat != 4)
			return -1;
		break;
	case Frame_Type3:
		if(stat != 7)
			return -1;
		break;
	case Frame_Type4:
		if(stat != 4)
			return -1;
		break;
	case Frame_TEST:
		if(stat != 4)
			return -1;
		break;
	default:
		return -1;
	}
	
	pUartObj->recvLen+=stat;
	return stat;
#endif
}


static int WeaponCtrlPORT_recvCfg(UartObj*pUartObj,int iLen)
{
#if 0
	BYTE *pCur = pUartObj->recvBuf;// + pUartObj->recvLen;
	int ilen = 0, stat, ret = -1;;
	while(TRUE)
	{
		if(GetDataFromUart(pUartObj->uartId,pCur,1, &stat)!=1)
		{
			return ret;
		}
		else
		{
			ilen ++;
			pCur ++;
			if(ilen == 2)
			{
				if(CAN_ID_PANEL == stoh2(pUartObj->recvBuf))
				{
						ret = CAN_DEVIC_PANEL;
						break;
				}
				else if (CAN_ID_TURRET == stoh2(pUartObj->recvBuf))
				{
						ret = CAN_DEVICE_TURRET;
						break;
				}
				else if (CAN_ID_MACHGUN == stoh2(pUartObj->recvBuf))
				{
						ret = CAN_DEVICE_MACHGUN;
						break;
				}
				else if (CAN_ID_GRENADE == stoh2(pUartObj->recvBuf))
				{
						ret = CAN_DEVICE_GRENADE;
						break;
				}
				else
				{
					ilen=1;
					pCur--;
					memcpy(pUartObj->recvBuf, pUartObj->recvBuf+1, 1);
				}
			}
		}
	}
	pUartObj->recvLen=2;
	return ret;
#endif
	return 0;
}
int WeaponCtrlPORT_ParseFrameByte_type0(unsigned char* buf)
{
#if 0
	unsigned char *pbuf=NULL;

//case Frame_Type0: //֡0����
	if(BYTE1(FrameBuf0)!= BYTE3(buf)){
		if(0x05==(BYTE3(buf)))   //�ŷ�����Ȩ���Ƿ�仯
		{
			setServoAvailable(TRUE);
			// Send CMD_Servo_Control_request_accepted
		}
	}
	pbuf = &buf[4];
	panoAngleV = STOH2(pbuf);
	pbuf = &buf[6];
	panoAngleH = STOH2(pbuf);
	if(panoAngleV > 36000||(panoAngleH>36000||(panoAngleH>9000&&panoAngleH<27000))){
		return -2;
	}

	if((BYTE6(FrameBuf0)) != (BYTE8(buf))){
		signal.byte = BYTE8(buf);
	}
	
	memcpy(FrameBuf0,&buf[2],sizeof(FrameBuf0));
//	break;
#endif
	return 0;
}
static int WeaponCtrlPORT_ParseFrame_type0(UartObj*pUartObj)
{
#if 0
	unsigned char *buf = pUartObj->recvBuf; 

	int stat;
	stat = WeaponCtrlPORT_recv(pUartObj,5);
	if(stat!=5)
		return -1;
	
	stat = WeaponCtrlPORT_ParseFrameByte_type0(buf);
//	WeaponCtrlPORT_send(buf, sizeof(FrameBuf0)+2);
	return stat;
#endif
}

void WeaponCtrlPORT_ParseFrameByte_type1(unsigned char* buf)
{
#if 1
	if(BIT3_2(BYTE1(FrameBuf1)) != BIT3_2(BYTE3(buf)))    //У׼/
	{    
		if(BIT3_2(BYTE3(buf)) == 0x01) //У׼
		{
			if(isBattleMode())
				MSGDRIV_send(CMD_BUTTON_CALIBRATION,NULL);
		}	
		else
		{
			if(isCalibrationMode())
				MSGDRIV_send(CMD_BUTTON_BATTLE,0);
		}
	}

	if(BIT1_0(BYTE1(FrameBuf1)) != BIT1_0(BYTE3(buf)))	//ѡ
	{     
		if(BIT1_0(BYTE3(buf)) == 0x00)			//Զװ
			MSGDRIV_send(CMD_BUTTON_BATTLE_AUTO,0);
		else 	//jing jie yu jing
			MSGDRIV_send(CMD_BUTTON_BATTLE_ALERT,0);
	}

	if(BIT7(BYTE1(FrameBuf1)) != BIT7(BYTE3(buf)))// disable/enable shot
	{     
		if(BIT7(BYTE3(buf)) != 0x00) 
			MSGDRIV_send(CMD_FIRING_DISABLE,0);
		else
			MSGDRIV_send(CMD_FIRING_ENABLE,0);
	}
	
	if(BIT6_4(BYTE1(FrameBuf1)) != BIT6_4(BYTE3(buf)))	//  л
	{
		switch(BIT6_4(BYTE3(buf)))
		{
			case 0x00:
			case 0x01:	//5.8
				MSGDRIV_send(CMD_BULLET_SWITCH1,0);
				if(BIT7_6(BYTE2(FrameBuf1)) != BIT7_6(BYTE4(buf)))	 //Ե/Կ Ŀ 
				{    
					if(BIT7_6(BYTE4(buf)) == 0x00)
						MSGDRIV_send(CMD_MODE_AIM_SKY,0);
					else
						MSGDRIV_send(CMD_MODE_AIM_LAND,0);	
				}
				break;
			case 0x02:	//
				MSGDRIV_send(CMD_BULLET_SWITCH2,0);
				MSGDRIV_send(CMD_MODE_AIM_LAND,0);	
				break;
			case 0x03:
				MSGDRIV_send(CMD_BULLET_SWITCH3,0);
				MSGDRIV_send(CMD_MODE_AIM_LAND,0);	
				break;
			default:
			break;
		}
	}
	else
	{
		if(BIT6_4(BYTE1(FrameBuf1)) == 0x01)
		{
			if(BIT7_6(BYTE2(FrameBuf1)) != BIT7_6(BYTE4(buf)))
			{  
				if(BIT7_6(BYTE4(buf)) == 0x00)
					MSGDRIV_send(CMD_MODE_AIM_SKY,0);
				else
					MSGDRIV_send(CMD_MODE_AIM_LAND,0);	
			}
		}
	}

	
	if(BIT5_4(BYTE2(FrameBuf1)) != BIT5_4(BYTE4(buf)))	//35
	{    
		if(BIT5_4(BYTE4(buf)) == 0x00)
			MSGDRIV_send(CMD_MODE_ATTACK_SIGLE,0);
		else if(BIT5_4(BYTE4(buf)) == 0x01)
			MSGDRIV_send(CMD_MODE_ATTACK_MULTI,0);
	}
	
	if(BIT3_2(BYTE2(FrameBuf1)) != BIT3_2(BYTE4(buf)))  //ƵӳСѡ
	{     
		if(BIT3_2(BYTE4(buf)) == 0x01)
			MSGDRIV_send(CMD_MODE_FOV_SMALL,0);
		else
			MSGDRIV_send(CMD_MODE_FOV_LARGE,0);
	}
	
	if(BIT0(BYTE2(FrameBuf1)) != BIT0(BYTE4(buf)))  //5.8ǹʽ
	{   
		if(BIT0(BYTE4(buf)) == 0x00)
			MSGDRIV_send(CMD_MODE_SHOT_SHORT,0);
		else if(BIT0(BYTE4(buf)) == 0x01)
			MSGDRIV_send(CMD_MODE_SHOT_LONG,0);
	}
	
	if(BIT7(BYTE3(FrameBuf1)) != BIT7(BYTE5(buf)))  //Ŵʾ
	{ 
		if(BIT7(BYTE5(buf)) == 0x01)	// tai qi shi qie huan
			MSGDRIV_send(CMD_MODE_SCALE_SWITCH,0);
	}
	if(BIT6(BYTE3(FrameBuf1)) != BIT6(BYTE5(buf)))  //ͼɫ
	{    
		if(BIT6(BYTE5(buf)) == 0x01)	
			MSGDRIV_send(CMD_MODE_PIC_COLOR_SWITCH,0);
	}
	if(BIT5(BYTE3(FrameBuf1)) != BIT5(BYTE5(buf)))	//ǿʾ
	{ 
		if(BIT5(BYTE5(buf)) == 0x01)	// tai qi shi qie huan
		{
			//if(isTimerAlive(eF3_Timer)/*״ֵ̬*/)
			{
				//killF3Timer();
				MSGDRIV_send(CMD_MODE_ENHANCE_SWITCH,0);
			}
		}
		else if(BIT5(BYTE5(buf)) == 0x00)
		{
			//startF3_Timer();
		}
	}
	if(BIT4(BYTE3(FrameBuf1)) != BIT4(BYTE5(buf)))	   //෽ʽ
	{   
		if(BIT4(BYTE5(buf)) == 0x01)
			MSGDRIV_send(CMD_MEASURE_DISTANCE_SWITCH,0);
	}
	if(BIT3(BYTE3(FrameBuf1)) != BIT3(BYTE5(buf)))	//F5
	{	
		if(BIT3(BYTE5(buf)) == 0x01)
		{
			//if(isTimerAlive(eF5_Timer)/*״ֵ̬*/)
			{
				//killF5Timer();
				MSGDRIV_send(CMD_LASERSELECT_SWITCH,0);
			}
		}
		else if(BIT3(BYTE5(buf)) == 0x00)
		{
			//startF5_Timer();
		}
	}
	if(BIT2(BYTE3(FrameBuf1)) != BIT2(BYTE5(buf)))	//F6
	{		
		if(BIT2(BYTE5(buf)) == 0x01)
		{
			//if(isTimerAlive(eF6_Timer)/*״ֵ̬*/)
			{
				//killF6Timer();
				MSGDRIV_send(CMD_SENSOR_SWITCH,0);
			}
		}
		else if(BIT2(BYTE5(buf)) == 0x00)
		{
			//startF6_Timer();
		}
	}
	if(BIT1(BYTE3(FrameBuf1)) != BIT1(BYTE5(buf)))//Զť״̬
	{    
		if(BIT1(BYTE5(buf)) == 0x01)
			MSGDRIV_send(CMD_BUTTON_AUTOCATCH,0);
	}
	if(BIT0(BYTE3(FrameBuf1)) != BIT0(BYTE5(buf)))//ఴť״̬
	{     
		if(BIT0(BYTE5(buf)) == 0x00)// down button to test the velocity
			MSGDRIV_send(CMD_MEASURE_VELOCITY,0);
		else if(BIT0(BYTE5(buf)) == 0x01)//up button to test the distance
		{
			//if(!isMeasureManual())
			MSGDRIV_send(CMD_MEASURE_DISTANCE,0);
		}
	}

	if(BIT4(BYTE4(FrameBuf1)) != BIT4(BYTE6(buf)))
	{
		if(BIT4(BYTE6(buf)) == 0x00)
			MSGDRIV_send(CMD_IDENTIFY_KILL,0);
		else
			MSGDRIV_send(CMD_IDENTIFY_GAS,0);
	}
	
	memcpy(FrameBuf1,&buf[2],sizeof(FrameBuf1));

#endif
}


static int WeaponCtrlPORT_ParseFrame_type1(UartObj*pUartObj)
{
#if 0
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;
	stat = WeaponCtrlPORT_recv(pUartObj,3);
	if(stat!=3)
		return -1;
	
	WeaponCtrlPORT_ParseFrameByte_type1(buf);
//	WeaponCtrlPORT_send(buf, sizeof(FrameBuf1)+2);
#endif
	return 0;
}
void WeaponCtrlPORT_ParseFrameByte_type2(unsigned char* buf)
{
#if 1
	switch (BIT3_0(BYTE3(buf)))
	{
		case Button_Left:
			MSGDRIV_send(CMD_BUTTON_LEFT,0);
			break;
		case Button_Right:
			MSGDRIV_send(CMD_BUTTON_RIGHT,0);
			break;
		case Button_UP:
			MSGDRIV_send(CMD_BUTTON_UP,0);
			//MSGDRIV_send(CMD_BUTTON_DOWN,0);
			break;
		case Button_Down:
			MSGDRIV_send(CMD_BUTTON_DOWN,0);
			//MSGDRIV_send(CMD_BUTTON_UP,0);
			break;
		case Button_Enter:
			MSGDRIV_send(CMD_BUTTON_ENTER,0);
			break;
		case Button_Esc:
			MSGDRIV_send(CMD_BUTTON_QUIT,0);
			break;
		case Button_Unlock:
			MSGDRIV_send(CMD_BUTTON_UNLOCK,0);
			break;
		case Button_AutoCheck:
			if(isBootUpSelfCheckFinished())
				MSGDRIV_send(CMD_BUTTON_AUTOCHECK,0);
			break;
		case Button_Base:
			break;
		default :
		break;
	}

//			break;
#endif
}


static int WeaponCtrlPORT_ParseFrame_type2(UartObj*pUartObj)
{
	#if 1
		unsigned char *buf = pUartObj->recvBuf; 
		int stat;
		stat = WeaponCtrlPORT_recv(pUartObj,1);
		if(stat!=1)
			return -1;
		
		WeaponCtrlPORT_ParseFrameByte_type2(buf);
		//WeaponCtrlPORT_send(buf, 4);
	#endif
		return 0;
}
 

void WeaponCtrlPORT_ParseFrameByte_type3(unsigned char* buf)
{
	
#if 1
	if(BIT4(BYTE1(FrameBuf3)) != BIT4(BYTE3(buf)))
	{
		if(BIT4(BYTE3(buf)) == 0x01)//handle normal
		{
			sendCommand(CMD_JOYSTICK_OK);
		}
		else if(BIT4(BYTE3(buf)) == 0x00)
		{
			sendCommand(CMD_JOYSTICK_ERR);
		}
	}
	
	if(BIT2(BYTE1(FrameBuf3)) != BIT2(BYTE3(buf)))
	{
		if(BIT2(BYTE3(buf)) == 0x01)//zhi dong qi jin suo zhuang tai
		{
			MSGDRIV_send(CMD_DETEND_LOCK,0);	//止动关
		}else if(BIT2(BYTE3(buf)) == 0x00)
		{
			MSGDRIV_send(CMD_DETEND_UNLOCK,0); //止动开
		}
	}
	
	if(BIT1(BYTE1(FrameBuf3)) != BIT1(BYTE3(buf)))
	{
		if(BIT1(BYTE3(buf)) == 0x00)//ά����״̬
		{
			MSGDRIV_send(CMD_MAINTPORT_LOCK, 0);
		}else if(BIT1(BYTE3(buf)) == 0x01)
		{
			MSGDRIV_send(CMD_MAINTPORT_UNLOCK, 0);
		}
	}
	/*******  ADDED 20160218  *********/
	if(BIT3(BYTE1(FrameBuf3)) != BIT3(BYTE3(buf)))
	{
		if(BIT3(BYTE3(buf)) == 0x00)//������� clutch
		{
			MSGDRIV_send(CMD_LIHEQI_CLOSE, 0);
		}else if(BIT3(BYTE3(buf)) == 0x01)//�������
		{
			MSGDRIV_send(CMD_LIHEQI_OPEN, 0);
		}
	}
	if(BIT1(BYTE2(FrameBuf3)) != BIT1(BYTE4(buf)))
	{
		if(BIT1(BYTE4(buf)) == 0x00)//35�������쳣
		{
			MSGDRIV_send(CMD_GRENADE_ERR, 0);
		}else if(BIT1(BYTE4(buf)) == 0x01)//35��������
		{
			MSGDRIV_send(CMD_GRENADE_OK, 0);
		}
	}
	if(BIT2(BYTE2(FrameBuf3)) != BIT2(BYTE4(buf)))
	{
		if(BIT2(BYTE4(buf)) == 0x00)//�����쳣
		{
			MSGDRIV_send(CMD_FIREBUTTON_ERR, 0);
		}else if(BIT2(BYTE4(buf)) == 0x01)//������
		{
			MSGDRIV_send(CMD_FIREBUTTON_OK, 0);
		}
	}
	if(BIT3(BYTE2(FrameBuf3)) != BIT3(BYTE4(buf)))
	{
		if(BIT3(BYTE4(buf)) == 0x00)//ȫ��CANͨ���쳣
		{
			MSGDRIV_send(CMD_FULSCREENCAN_ERR, 0);
		}else if(BIT3(BYTE4(buf)) == 0x01)//ȫ��CANͨ����
		{
			MSGDRIV_send(CMD_FULSCREENCAN_OK, 0);
		}
	}
	if(BIT5(BYTE2(FrameBuf3)) != BIT5(BYTE4(buf)))
	{
		if(BIT5(BYTE4(buf)) == 0x00)//�Կ�CAN0ͨ���쳣
		{
			MSGDRIV_send(CMD_DISCONTRLCAN0_ERR, 0);
		}else if(BIT5(BYTE4(buf)) == 0x01)//�Կ�CAN0ͨ����
		{
			MSGDRIV_send(CMD_DISCONTRLCAN0_OK, 0);
		}
	}
	if(BIT6(BYTE2(FrameBuf3)) != BIT6(BYTE4(buf)))
	{
		if(BIT6(BYTE4(buf)) == 0x00)//�Կ�CAN1ͨ���쳣
		{
			MSGDRIV_send(CMD_DISCONTRLCAN1_ERR, 0);
		}else if(BIT6(BYTE4(buf)) == 0x01)//�Կ�CAN1ͨ����
		{
			MSGDRIV_send(CMD_DISCONTRLCAN1_OK, 0);
		}
	}

	if(BIT0(BYTE3(FrameBuf3)) != BIT0(BYTE5(buf)))
	{
		if(BIT0(BYTE5(buf)) == 0x00)//���-�����״̬�쳣 electro magnet
		{
				MSGDRIV_send(CMD_DIANCITIE_ERR, 0);
		}else if(BIT0(BYTE5(buf)) == 0x01)//���-�����״̬��
		{
				MSGDRIV_send(CMD_DIANCITIE_OK, 0);
		}
	}
	if(BIT1(BYTE3(FrameBuf3)) != BIT1(BYTE5(buf)))
	{
		if(BIT1(BYTE5(buf)) == 0x00)//���-ˮƽ���״̬�쳣
		{
				MSGDRIV_send(CMD_POSMOTOR_ERR, 0);
		}else if(BIT1(BYTE5(buf)) == 0x01)//���-ˮƽ���״̬��
		{
				MSGDRIV_send(CMD_POSMOTOR_OK, 0);
		}
	}
	if(BIT2(BYTE3(FrameBuf3)) != BIT2(BYTE5(buf)))
	{
		if(BIT2(BYTE5(buf)) == 0x00)//���-5.8���״̬�쳣
		{
			MSGDRIV_send(CMD_MACHGUNMOTOR_ERR, 0);
		}else if(BIT2(BYTE5(buf)) == 0x01)//���-5.8���״̬��
		{
			MSGDRIV_send(CMD_MACHGUNMOTOR_OK, 0);
		}
	}
	if(BIT3(BYTE3(FrameBuf3)) != BIT3(BYTE5(buf)))
	{
		if(BIT3(BYTE5(buf)) == 0x00)//���-35����״̬�쳣
		{
			MSGDRIV_send(CMD_GRENADEMOTOR_ERR, 0);
		}else if(BIT3(BYTE5(buf)) == 0x01)//���-35����״̬��
		{
			MSGDRIV_send(CMD_GRENADEMOTOR_OK, 0);
		}
	}
	
//			break;
#endif

	memcpy(FrameBuf3,buf+2,sizeof(FrameBuf3));

}
static int WeaponCtrlPORT_ParseFrame_type3(UartObj*pUartObj)
{
#if 0
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;
	stat = WeaponCtrlPORT_recv(pUartObj,3);
	if(stat!=3)
		return -1;

	WeaponCtrlPORT_ParseFrameByte_type3(buf);
//	WeaponCtrlPORT_send(buf, sizeof(FrameBuf3)+2);
#endif
	return 0;
}
void WeaponCtrlPORT_ParseFrameByte_type4(unsigned char* buf)
{
//		case Frame_Type4://֡4����                   ---�������� /��ǹָ�� /��ǿ��ָ��
#if 1
	switch(BYTE3(buf)){
		case Fire_Gun:
			sendCommand(CMD_SCHEDULE_GUN);
			break;
		case Fire_Strong:
			sendCommand(CMD_SCHEDULE_STRONG);
			break;
		case Fire_Shot:
			sendCommand(CMD_USER_FIRED);
			break;
		case Fire_Reset:
			sendCommand(CMD_SCHEDULE_RESET);
			break;
		case Fire_Base:
			break;
		default:
			break;
	}
//			break;
#endif
}
static int WeaponCtrlPORT_ParseFrame_type4(UartObj*pUartObj)
{
#if 0
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;
	stat = WeaponCtrlPORT_recv(pUartObj,1);
	if(stat!=1)
		return -1;

	WeaponCtrlPORT_ParseFrameByte_type4(buf);
//	WeaponCtrlPORT_send(buf, 4);
#endif
	return 0;
}
void WeaponCtrlPORT_ParseFrameByte_test(unsigned char* buf)
{
//		case Frame_TEST://���Է��洫��������
#if 0
	if(0x00 == BIT0(BYTE3(buf)))
		sendCommand(CMD_GENERADE_SERVO_OK);
	if(0x00 == BIT1(BYTE3(buf)))
		sendCommand(CMD_MACHINEGUN_SERVO_OK);
	if(0x00 == BIT2(BYTE3(buf)))
		sendCommand(CMD_POSITION_SERVO_OK);
	if(0x00 == BIT3(BYTE3(buf)))
		sendCommand(CMD_GENERADE_SENSOR_OK);
	if(0x00 == BIT4(BYTE3(buf)))
		sendCommand(CMD_MACHINEGUN_SENSOR_OK);
	if(0x00 == BIT5(BYTE3(buf)))
		sendCommand(CMD_POSITION_SENSOR_OK);
	if(0x00 == BIT6(BYTE3(buf)))
		sendCommand(CMD_DIP_ANGLE_OK);
	if(0x00 == BIT7(BYTE3(buf)))
		sendCommand(CMD_JOYSTICK_OK);
//			break;
#endif
}
static int WeaponCtrlPORT_ParseFrame_test(UartObj*pUartObj)
{
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;
#if 0
	stat = WeaponCtrlPORT_recv(pUartObj,1);
	if(stat!=1)
		return -1;

	WeaponCtrlPORT_ParseFrameByte_test(buf);
//	WeaponCtrlPORT_send(buf, 4);
#endif
	return 0;
}

static void WeaponCtrlPORT_ParseFrameId(UartObj*pUartObj,BYTE type)
{
#if 0
	switch(type)
	{
		case Frame_Type0:
			WeaponCtrlPORT_ParseFrame_type0(pUartObj);
			break;
		case Frame_Type1:
			WeaponCtrlPORT_ParseFrame_type1(pUartObj);
			break;
		case Frame_Type2:
			WeaponCtrlPORT_ParseFrame_type2(pUartObj);
			break;
		case Frame_Type3:
			WeaponCtrlPORT_ParseFrame_type3(pUartObj);
			MYBOARD_waitusec(20);
			break;
		case Frame_Type4:
			WeaponCtrlPORT_ParseFrame_type4(pUartObj);
			break;
		case Frame_TEST:
			WeaponCtrlPORT_ParseFrame_test(pUartObj);
			break;
		case 0xAC:
			if(!bPositionServoOK())
				sendCommand(CMD_POSITION_SERVO_OK);
			killSelfCheckPosServoTimer();
			startSelfCheckPosServo_Timer();
			break;
		case 0xB7:
			if(!bMachineGunServoOK())
				sendCommand(CMD_MACHINEGUN_SERVO_OK);
			killSelfCheckMachGunServoTimer();
			startSelfCheckMachGunServo_Timer();
			break;
		case 0xC2:
			if(!bGrenadeServoOK())
				sendCommand(CMD_GENERADE_SERVO_OK);
			killSelfCheckGrenadeServoTimer();
			startSelfCheckGrenadeServo_Timer();
			break;
		default:
			break;
	}
#endif
}

static void WeaponCtrlPORT_ParsePanel(UartObj*pUartObj)
{
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;

	if(isBootUpMode()&&isBootUpSelfCheck()&&(!bWeaponCtrlOK()))
		MSGDRIV_send(CMD_WEAPONCTRL_OK,0);
#if 0
	stat = WeaponCtrlPORT_recv(pUartObj,1);
	if(stat!=1)
		return;
	WeaponCtrlPORT_ParseFrameId(pUartObj,BYTE2(buf));
#endif
	return ;
}
void WeaponCtrlPORT_ParseBytePanel(unsigned char *buf)
{
#if 1
	if(isBootUpMode()&&isBootUpSelfCheck()&&(!bWeaponCtrlOK()))
		sendCommand(CMD_WEAPONCTRL_OK);

	switch(buf[2])
	{
	case Frame_Type0:
		if(uart_open_close_flag)
			return ;
		WeaponCtrlPORT_ParseFrameByte_type0(buf);
		break;
	case Frame_Type1:
		if(uart_open_close_flag)
			return ;
		WeaponCtrlPORT_ParseFrameByte_type1(buf);
		break;
	case Frame_Type2:
		if(uart_open_close_flag)
			return ;
		WeaponCtrlPORT_ParseFrameByte_type2(buf);
		break;
	case Frame_Type3:
		WeaponCtrlPORT_ParseFrameByte_type3(buf);
		break;
	case Frame_Type4:
		WeaponCtrlPORT_ParseFrameByte_type4(buf);
		break;
	case Frame_TEST:
		//WeaponCtrlPORT_ParseFrameByte_test(buf);
		break;
	case 0xAC:
		//if(!bPositionServoOK())
			//sendCommand(CMD_POSITION_SERVO_OK);
		//killSelfCheckPosServoTimer();
		//startSelfCheckPosServo_Timer();
		break;
	case 0xB7:
		//if(!bMachineGunServoOK())
			//sendCommand(CMD_MACHINEGUN_SERVO_OK);
		//killSelfCheckMachGunServoTimer();
		//startSelfCheckMachGunServo_Timer();
		break;
	case 0xC2:
		//if(!bGrenadeServoOK())
			//sendCommand(CMD_GENERADE_SERVO_OK);
		//killSelfCheckGrenadeServoTimer();
		//startSelfCheckGrenadeServo_Timer();
		break;
	default:
		break;
	}
#endif
}
static void WeaponCtrlPORT_ParseTurret(UartObj*pUartObj)
{

	//if(!bPositionServoOK())
	if(1)
		MSGDRIV_send(CMD_POSITION_SERVO_OK,0);
#if 0
	killSelfCheckPosServoTimer();
	startSelfCheckPosServo_Timer();
#endif
}
void WeaponCtrlPORT_ParseByteTurret(unsigned char *buf)
{

}
static void WeaponCtrlPORT_ParseMachGun(UartObj*pUartObj)
{

	//if(!bMachineGunServoOK())
	if(1)
		MSGDRIV_send(CMD_MACHINEGUN_SERVO_OK,0);
#if 0
	killSelfCheckMachGunServoTimer();
	startSelfCheckMachGunServo_Timer();
#endif
}
void WeaponCtrlPORT_ParseByteMachGun(unsigned char *buf)
{
	
}

static void WeaponCtrlPORT_ParseGrenade(UartObj*pUartObj)
{

	//if(!bGrenadeServoOK())
	if(1)
		MSGDRIV_send(CMD_GENERADE_SERVO_OK,0);
#if 0
	killSelfCheckGrenadeServoTimer();
	startSelfCheckGrenadeServo_Timer();
#endif
}
void WeaponCtrlPORT_ParseByteGrenade(unsigned char *buf)
{

}

int WeaponCtrlPort_ParseByte(BYTE* buf)
{
#if 0
	switch(stoh2(buf))
	{
	case CAN_ID_PANEL:
		WeaponCtrlPORT_ParseBytePanel(buf);
		break;
	case CAN_ID_TURRET:
		WeaponCtrlPORT_ParseByteTurret(buf);
		break;
	case CAN_ID_MACHGUN:
		WeaponCtrlPORT_ParseByteMachGun(buf);
		break;
	case CAN_ID_GRENADE:
		WeaponCtrlPORT_ParseByteGrenade(buf);
		break;
	default:
		break;
	}
#endif	
	return 0;
}

void* WeaponCtrlPORT_recvTask(void* prm)
{
	int stat=0;
	UartObj* pUartObj = &WeaponCtrlObj.UartPort;

	//while (!pSysCtrlObj->tskArry[TSK_RECVTRACEMSG]) 
	while (1) 
	{	
		WeaponCtrlObj.UartPort.recvLen = 0;	

		stat = WeaponCtrlPORT_recvCfg(pUartObj,2);

		if(stat < 0)
			continue;
		switch (stat)
		{
			case CAN_DEVIC_PANEL:
				WeaponCtrlPORT_ParsePanel(pUartObj);
				break;
			case CAN_DEVICE_TURRET:
				WeaponCtrlPORT_ParseTurret(pUartObj);
				break;
			case CAN_DEVICE_MACHGUN:
				WeaponCtrlPORT_ParseMachGun(pUartObj);
				break;
			case CAN_DEVICE_GRENADE:
				WeaponCtrlPORT_ParseGrenade(pUartObj);
				break;
			default:
				break;
		}
	}
	
return NULL;
}


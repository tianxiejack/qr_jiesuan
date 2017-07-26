
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


#include "WeaponCtrl.h"
#include "MachGunPort.h"
#include "GrenadePort.h"
#include "LaserPort.h"

#define CAN_ID_PANEL (0x0002)
#define CAN_ID_TURRET (0x02AC)
#define CAN_ID_MACHGUN (0x02B7)
#define CAN_ID_GRENADE	(0x2C2)
#define CODE_MACHGUN (0x37)
#define CODE_GRENADE (0x42)
#define CODE_TURRET   (0x2C)
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
static int ibInit=0,servoInit=1;
static BOOL WeaponCtrlPORT_open();
static void WeaponCtrlPORT_close();

static BOOL bTraceSend=FALSE;
//static PVE_MSG *pOutMsg=NULL;

static BYTE CANSendbuf[10]={0x01,0x04,0xB1,0x00,};
static BYTE CANSendFrame1[10]={0x01,0x04,0xB0,0x00,};
static BYTE CANSendFrame2[10]={0x01,0x04,0xB2,0x00,};
//static BYTE CHECKSERVOMACH[6]={0x03,0x42,0x50,0x58,0x00,0x00};
//static BYTE CHECKSERVOPOS[6]   ={0x03,0x42,0x50,0x58,0x00,0x00};
//static BYTE CHECKSERVOGRENADE[6]={0x03,0x42,0x50,0x58,0x00,0x00};
static char WeaponCtrl[16]={0};
static 	unsigned short panoAngleV=0,panoAngleH=0;

void absPosRequest(BYTE code)
{
#if 0
	BYTE SERVOPOS[6]   ={0x03,0x00,0x50,0x58,0x00,0x00};
	SERVOPOS[1] = code;
	WeaponCtrlPORT_send(SERVOPOS, sizeof(SERVOPOS));
	MYBOARD_waitusec(500);
#endif
}

void startServo(BYTE code)
{
//	BYTE start[6] = {0x03,0x00,0x42,0x47,0x00,0x00};
//	start[1] = code;
//	WeaponCtrlPORT_send(start, sizeof(start));
}
void startServoServer(BYTE code)
{
#if 0
	BYTE buf[4] = {0x00,0x00,0x01,0x00};
	BYTE MOTOR0[10]	={0x03,0x00,0x4D,0x4F,0x00,0x00,0x00,0x00,0x00,0x00};
	BYTE MOTOR1[10]	={0x03,0x00,0x4D,0x4F,0x00,0x00,0x01,0x00,0x00,0x00};
	BYTE MODE[10]	={0x03,0x00,0x55,0x4D,0x00,0x00,0x05,0x00,0x00,0x00};
	WeaponCtrlPORT_send(buf, sizeof(buf));
	MYBOARD_waitusec(20*5000);
	MOTOR0[1] = code;
	WeaponCtrlPORT_send(MOTOR0, sizeof(MOTOR0));
	MYBOARD_waitusec(20*5000);
	MODE[1] = code;
	WeaponCtrlPORT_send(MODE, sizeof(MODE));
	MYBOARD_waitusec(20*5000);
	MOTOR1[1] = code;
	WeaponCtrlPORT_send(MOTOR1, sizeof(MOTOR1));
	MYBOARD_waitusec(20*5000);
	absPosRequest(code);
	MYBOARD_waitusec(20*5000);
	startServo(code);
	MYBOARD_waitusec(20*5000);
#endif
}


void killServoCheckTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eServoCheck_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eServoCheck_Timer);
	}
#endif
}
void ServoCheckTimer_cbFxn(void* cbParam)
{
#if 0
	if(isBootUpMode()&&isBootUpSelfCheck())
		return ;
	sendCommand(CMD_SERVOTIMER_MACHGUN);
#endif
}

void startServoCheck_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eServoCheck_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eServoCheck_Timer,SERVOCHECK_TIMER,ServoCheckTimer_cbFxn,(void*)(0x01));	
	}
#endif
}


void killF6Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF6_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eF6_Timer);
	}
#endif
}
void F6Timer_cbFxn(void* cbParam)
{
//	killF6Timer();
//	sendCommand(CMD_CONNECT_SWITCH);
}

void startF6_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF6_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eF6_Timer,FN_TIMER,F6Timer_cbFxn,(void*)(0x01));	
	}
#endif
}

void killF5Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF5_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eF5_Timer);
	}
#endif
}
void F5Timer_cbFxn(void* cbParam)
{
//	killF5Timer();
//	sendCommand(CMD_MIDPARAMS_SWITCH );
}

void startF5_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF5_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eF5_Timer,FN_TIMER,F5Timer_cbFxn,(void*)(0x01));	
	}
#endif
}

void  killF3Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF3_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eF3_Timer);
	}
#endif
}

void F3Timer_cbFxn(void* cbParam)
{
//	killF3Timer();
//	sendCommand(CMD_STABLEVIDEO_SWITCH );
}

void startF3_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eF3_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eF3_Timer,FN_TIMER,F3Timer_cbFxn,(void*)(0x01));	
	}
#endif
}
BOOL isTimerAlive(UINT id)
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	return (pCtrlTimer->GetTimerStat(pCtrlTimer,id)==eTimer_Stat_Run);
#endif
}
void killSelfCheckPosServoTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,ePosServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,ePosServo_Timer);
	}
#endif
}
void SelfCheckPosServoTimer_cbFxn(void* cbParam)
{
//	killSelfCheckPosServoTimer();
//	sendCommand(CMD_POSITION_SERVO_ERR);
}

void startSelfCheckPosServo_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,ePosServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,ePosServo_Timer,SELFCHECK_TIMER,SelfCheckPosServoTimer_cbFxn,(void*)(0x01));	
	}
#endif
}
void killSelfCheckMachGunServoTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eMachGunServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eMachGunServo_Timer);
	}
#endif
}
void SelfCheckMachGunServoTimer_cbFxn(void* cbParam)
{
//	killSelfCheckMachGunServoTimer();
//	sendCommand(CMD_MACHINEGUN_SERVO_ERR);
}

void startSelfCheckMachGunServo_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eMachGunServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eMachGunServo_Timer,SELFCHECK_TIMER,SelfCheckMachGunServoTimer_cbFxn,(void*)(0x01));	
	}
#endif
}
void killSelfCheckGrenadeServoTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eGrenadeServo_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(pCtrlTimer,eGrenadeServo_Timer);
	}
#endif
}
void SelfCheckGrenadeServoTimer_cbFxn(void* cbParam)
{
//	killSelfCheckGrenadeServoTimer();
//	sendCommand(CMD_GENERADE_SERVO_ERR);
}

void startSelfCheckGrenadeServo_Timer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eGrenadeServo_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eGrenadeServo_Timer,SELFCHECK_TIMER,SelfCheckGrenadeServoTimer_cbFxn,(void*)(0x01));	
	}
#endif
}

BOOL isMachGunUpLocked()
{
//	return BIT1(BYTE6(FrameBuf0));
}
BOOL isMachGunDownLocked()
{
//	return BIT0(BYTE6(FrameBuf0));
}
BOOL isGrenadeUpLocked()
{
//	return BIT5(BYTE6(FrameBuf0));
}
BOOL isGrenadeDownLocked()
{
//	return BIT4(BYTE6(FrameBuf0));
}
BOOL isMachGunPosLocked()
{
//	return BIT3(BYTE6(FrameBuf0));
}
BOOL isGrenadePosLocked()
{
//	return BIT7(BYTE6(FrameBuf0));
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
//	return BIT5(BYTE4(FrameBuf1));
}
BOOL isIdentifyGas()
{
//	return BIT4(BYTE4(FrameBuf1));
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
//	bTraceSend = context;
}

void requstServoContrl()
{
//	CANSendbuf[3] = (CANSendbuf[3]&0xF0) |0x05 ;
}

void releaseServoContrl(void)
{
//	setServoAvailable(FALSE);
	
//	CANSendbuf[3] = (CANSendbuf[3]&0xF0);
}

void setGrenadeInPositonFlag(void)
{
//	CANSendbuf[3] = (CANSendbuf[3]&0x0F) |0x30 ;
}

void resetGrenadeInPositionFlag(void)
{
//	CANSendbuf[3] = (CANSendbuf[3]&0x0F);
}
void processCMD_SERVOTIMER_MACHGUN(LPARAM lParam)
{
//	absPosRequest(CODE_GRENADE);
//	absPosRequest(CODE_MACHGUN);
//	absPosRequest(CODE_TURRET);
}
void processCMD_TIMER_SENDFRAME0(LPARAM lParam)
{
#if 0
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
	
	WeaponCtrlPORT_send(CANSendbuf, sizeof(CANSendbuf));
#endif
	
}
void processCMD_TIMER_SENDFRAME1(LPARAM lParam)//����֡1
{
#if 0
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
	
	WeaponCtrlPORT_send(CANSendFrame1, 10);
#endif
}
void processCMD_TIMER_SENDFRAME2(LPARAM lParam)//����֡2
{
#if 0
	if(isBootUpMode()&&isBootUpSelfCheck())
		return ;

	//todo: ���Լ�״̬����ʵʱ���µ�״̬?�Ӷ�ʱ��?
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
	(BYTE9(CANSendFrame2)) |= (0XDF | (bVideoOK()<<5));//��Ƶ����״̬
	
	WeaponCtrlPORT_send(CANSendFrame1, 10);
#endif
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
#if 0
	startServoServer(CODE_GRENADE);
	startServoServer(CODE_MACHGUN);
	startServoServer(CODE_TURRET);
#endif
}
Fov_Type getFovState()
{
//	return (BIT3_2(BYTE2(FrameBuf1)))?(FOV_SMALL):(FOV_LARGE);
}
void setSysWorkMode()
{
//	BYTE1(FrameBuf1) &= ~0x0C;
}
void setJoyStickStat(BOOL stat)
{
#if 0
	if(stat)
		FrameBuf3[1] = FrameBuf3[1]|0x10 ;
	else
		FrameBuf3[1] = FrameBuf3[1]&0xEF ;
#endif
}
void  updateBulletType(int type)
{
//	BYTE1(FrameBuf1) &= 0x8F;
//	BYTE1(FrameBuf1) |= ((type&0x03) + 1)<<4;
}
int getBulletType()
{
#if 0
	int i = BIT6_4(BYTE1(FrameBuf1));
	return (i > 0)? i:1 ;
#endif
}

void CANSend_cbFxn(void* cbParam)
{
#if 0
//	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(servoInit){
		servoInit = 0;
		startServoCheck_Timer();
		sendCommand(CMD_SERVO_INIT);
	}
	
	if(isBootUpMode()&&isBootUpSelfCheck()){
		startServoCheck_Timer();
		return ;
	}
	sendCommand(CMD_TIMER_SENDFRAME0);
	if(bTraceSend)
		sendCommand(CMD_TRACE_SENDFRAME0);
#endif
}

 void startCANSendTimer()
{
#if 0
	CTimerCtrl * pCtrlTimer;
	pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(pCtrlTimer,eCAN_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->SetTimer(pCtrlTimer,eCAN_Timer,CAN_TIMER,CANSend_cbFxn,(void*)(0x01));	
	}
#endif
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
	WeaponCtrlObj.pInter.PortType	= Enum_WeaponCtrlPort;//Enum_TracPort;//
	WeaponCtrlObj.pInter.OpenPort 	= WeaponCtrlPORT_open;
	WeaponCtrlObj.pInter.ClosePort	= WeaponCtrlPORT_close;
	WeaponCtrlObj.pInter.Write		= WeaponCtrlPORT_send;
	WeaponCtrlObj.pInter.Read		= NULL;
	WeaponCtrlObj.pInter.Control		= NULL;
	WeaponCtrlObj.pInter.MuxPortRecv	= MuxPortRecvFun;

	WeaponCtrlObj.UartPort.uartId	= 0xFF;
	WeaponCtrlObj.UartPort.syncObj 	= SDK_SEM_CREATE(0);
	WeaponCtrlObj.UartPort.recvLen	= 0;

	SDK_SEM_RESET(WeaponCtrlObj.UartPort.syncObj);
	if(!WeaponCtrlPORT_open()){
		SDK_SEM_DESTROY(WeaponCtrlObj.UartPort.syncObj);
		return NULL;
	}
	
	pOutMsg = SDK_MALLOC(sizeof(PVE_MSG)+10, EXTERNALHEAP);
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
	while(TRUE){
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
//case Frame_Type1://֡1����
#if 0
	if(BIT3_2(BYTE1(FrameBuf1)) != BIT3_2(BYTE3(buf))){    //У׼/����
		if(BIT3_2(BYTE3(buf)) == 0x01) //У׼
		{
			if(isBattleMode())
			sendCommand(CMD_BUTTON_CALIBRATION);
		}	
		else //if(buf[3]&0x0A == 0x0)����
		{
			if(isCalibrationMode())
			sendCommand(CMD_BUTTON_BATTLE);
		}

	}

	if(BIT1_0(BYTE1(FrameBuf1)) != BIT1_0(BYTE3(buf))){     //����ѡ�� 
		if(BIT1_0(BYTE3(buf)) == 0x00)			//�Զ�װ��
			sendCommand(CMD_BUTTON_BATTLE_AUTO);
		else //if(buf[3]&0x03 == 0x01)	//����Ԥ��
			sendCommand(CMD_BUTTON_BATTLE_ALERT);
	}

	if(BIT7(BYTE1(FrameBuf1)) != BIT7(BYTE3(buf))){     //  ʹ��/��ֹ ���
		if(BIT7(BYTE3(buf)) != 0x00) 
			sendCommand(CMD_FIRING_DISABLE);
		else
			sendCommand(CMD_FIRING_ENABLE);
	}
	
	if(BIT6_4(BYTE1(FrameBuf1)) != BIT6_4(BYTE3(buf))){//  �����л�
		switch(BIT6_4(BYTE3(buf))){
			case 0x00:
			case 0x01:
				sendCommand(CMD_BULLET_SWITCH1);
				if(BIT7_6(BYTE2(FrameBuf1)) != BIT7_6(BYTE4(buf))){     //�Ե�/�Կ� Ŀ��
					if(BIT7_6(BYTE4(buf)) == 0x00)
						sendCommand(CMD_MODE_AIM_SKY);
					else
						sendCommand(CMD_MODE_AIM_LAND);	
				}
				break;
			case 0x02:
				sendCommand(CMD_BULLET_SWITCH2);
				sendCommand(CMD_MODE_AIM_LAND);	
				break;
			case 0x03:
				sendCommand(CMD_BULLET_SWITCH3);
				sendCommand(CMD_MODE_AIM_LAND);	
				break;
			default:
			break;
		}
	}else{
		if(BIT6_4(BYTE1(FrameBuf1)) == 0x01){
			if(BIT7_6(BYTE2(FrameBuf1)) != BIT7_6(BYTE4(buf))){     //�Ե�/�Կ� Ŀ��
				if(BIT7_6(BYTE4(buf)) == 0x00)
					sendCommand(CMD_MODE_AIM_SKY);
				else
					sendCommand(CMD_MODE_AIM_LAND);	
			}
		}
	}
/*
	if(BIT7_6(BYTE2(FrameBuf1)) != BIT7_6(BYTE4(buf))){     //�Ե�/�Կ� Ŀ��
		if(BIT7_6(BYTE4(buf)) == 0x01)			//��
		{
			sendCommand(CMD_MODE_AIM_LAND);	
			if(1)//  ���ָֻ�
			{	switch(BIT6_4(BYTE3(buf))){
					case 0x00:
					case 0x01:
						sendCommand(CMD_BULLET_SWITCH1);
						break;
					case 0x02:
						sendCommand(CMD_BULLET_SWITCH2);
						break;
					case 0x03:
						sendCommand(CMD_BULLET_SWITCH3);
						break;
					default:
					break;
				}
			}
		}else if(BIT7_6(BYTE4(buf)) == 0x00)	
		{	//��
			sendCommand(CMD_MODE_AIM_SKY);
			sendCommand(CMD_BULLET_SWITCH1);
		}

	}else
	{     
		if(BIT7_6(BYTE4(buf)) == 0x00)//�Կ�
		{
			// do nothing
		}else
		{
			if(BIT6_4(BYTE1(FrameBuf1)) != BIT6_4(BYTE3(buf)))//  �����л�
			{	switch(BIT6_4(BYTE3(buf))){
					case 0x00:
					case 0x01:
						sendCommand(CMD_BULLET_SWITCH1);
						break;
					case 0x02:
						sendCommand(CMD_BULLET_SWITCH2);
						break;
					case 0x03:
						sendCommand(CMD_BULLET_SWITCH3);
						break;
					default:
					break;
				}
			}
		}

		// Send CMD_Bullet_switch((buf[3]>>2)&0x07);
	}*/
	
	if(BIT5_4(BYTE2(FrameBuf1)) != BIT5_4(BYTE4(buf))){     //35����������
		if(BIT5_4(BYTE4(buf)) == 0x00)
			sendCommand(CMD_MODE_ATTACK_SIGLE);
		else if(BIT5_4(BYTE4(buf)) == 0x01)
			sendCommand(CMD_MODE_ATTACK_MULTI);
	}
	if(BIT3_2(BYTE2(FrameBuf1)) != BIT3_2(BYTE4(buf))){     //��Ƶ�ӳ���Сѡ��
		if(BIT3_2(BYTE4(buf)) == 0x01)
			sendCommand(CMD_MODE_FOV_SMALL);
		else //if(buf[4]&0x0C == 0x0)
			sendCommand(CMD_MODE_FOV_LARGE);
	}
	if(BIT0(BYTE2(FrameBuf1)) != BIT0(BYTE4(buf))){     //5.8��ǹ������ʽ
		if(BIT0(BYTE4(buf)) == 0x00)
			sendCommand(CMD_MODE_SHOT_SHORT);
		else if(BIT0(BYTE4(buf)) == 0x01)
			sendCommand(CMD_MODE_SHOT_LONG);
	}
	
	if(BIT7(BYTE3(FrameBuf1)) != BIT7(BYTE5(buf))){     //�Ŵ���ʾ
		if(BIT7(BYTE5(buf)) == 0x01)//̧��ʱ�л�
			sendCommand(CMD_MODE_SCALE_SWITCH);
	}
	if(BIT6(BYTE3(FrameBuf1)) != BIT6(BYTE5(buf))){     //ͼ����ɫ����
		if(BIT6(BYTE5(buf)) == 0x01)//̧��ʱ�л�
			sendCommand(CMD_MODE_PIC_COLOR_SWITCH);
	}
	if(BIT5(BYTE3(FrameBuf1)) != BIT5(BYTE5(buf))){     //��ǿ��ʾ����
		if(BIT5(BYTE5(buf)) == 0x01){//̧��ʱ�л�
			if(isTimerAlive(eF3_Timer)/*״ֵ̬*/){
				killF3Timer();
				sendCommand(CMD_MODE_ENHANCE_SWITCH);
			}
		}else if(BIT5(BYTE5(buf)) == 0x00){
			startF3_Timer();
		}
	}
	if(BIT4(BYTE3(FrameBuf1)) != BIT4(BYTE5(buf))){     //��෽ʽ����
		if(BIT4(BYTE5(buf)) == 0x01)//̧��ʱ�л�
			sendCommand(CMD_MEASURE_DISTANCE_SWITCH);
	}
	if(BIT3(BYTE3(FrameBuf1)) != BIT3(BYTE5(buf))){		//F5
		if(BIT3(BYTE5(buf)) == 0x01){//̧��ʱ�л�
			if(isTimerAlive(eF5_Timer)/*״ֵ̬*/){
				killF5Timer();
				sendCommand(CMD_LASERSELECT_SWITCH);
			}
		}else if(BIT3(BYTE5(buf)) == 0x00){//����
			startF5_Timer();
		}
	}
	if(BIT2(BYTE3(FrameBuf1)) != BIT2(BYTE5(buf))){		//F6
		if(BIT2(BYTE5(buf)) == 0x01){//̧��ʱ�л�
			if(isTimerAlive(eF6_Timer)/*״ֵ̬*/){
				killF6Timer();
				sendCommand(CMD_SENSOR_SWITCH);
			}
		}else if(BIT2(BYTE5(buf)) == 0x00){
			startF6_Timer();
		}
	}
	if(BIT1(BYTE3(FrameBuf1)) != BIT1(BYTE5(buf))){     //�Զ�����ť״̬
		if(BIT1(BYTE5(buf)) == 0x01)
			sendCommand(CMD_BUTTON_AUTOCATCH);
	}
	if(BIT0(BYTE3(FrameBuf1)) != BIT0(BYTE5(buf)))		//�����ఴť״̬
	{     
		if(BIT0(BYTE5(buf)) == 0x00)//���� ����
			sendCommand(CMD_MEASURE_VELOCITY);
		else if(BIT0(BYTE5(buf)) == 0x01)//̧�� ���
		{
//			if(!isMeasureManual())
				sendCommand(CMD_MEASURE_DISTANCE);
		}
	}

	if(BIT4(BYTE4(FrameBuf1)) != BIT4(BYTE6(buf))){
		if(BIT4(BYTE6(buf)) == 0x00)
			sendCommand(CMD_IDENTIFY_KILL);
		else
			sendCommand(CMD_IDENTIFY_GAS);
	}
	
	memcpy(FrameBuf1,&buf[2],sizeof(FrameBuf1));

//	break;
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
//		case Frame_Type2://֡2����                    ---ʹ��PostMessage ?
#if 0	
	switch (BIT3_0(BYTE3(buf)))
	{
		case Button_Left:
			sendCommand(CMD_BUTTON_LEFT);
			break;
		case Button_Right:
			sendCommand(CMD_BUTTON_RIGHT);
			break;
		case Button_UP:
			sendCommand(CMD_BUTTON_UP);
			break;
		case Button_Down:
			sendCommand(CMD_BUTTON_DOWN);
			break;
		case Button_Enter:
			sendCommand(CMD_BUTTON_ENTER);
			break;
		case Button_Esc:
			sendCommand(CMD_BUTTON_QUIT);
			break;
		case Button_Unlock:
			sendCommand(CMD_BUTTON_UNLOCK);
			break;
		case Button_AutoCheck:
			if(isBootUpSelfCheckFinished())
			sendCommand(CMD_BUTTON_AUTOCHECK);
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
#if 0
	unsigned char *buf = pUartObj->recvBuf; 
	int stat;
	stat = WeaponCtrlPORT_recv(pUartObj,1);
	if(stat!=1)
		return -1;
	
	WeaponCtrlPORT_ParseFrameByte_type2(buf);
//	WeaponCtrlPORT_send(buf, 4);
#endif
	return 0;
}
void WeaponCtrlPORT_ParseFrameByte_type3(unsigned char* buf)
{
//		case Frame_Type3://֡3����                   ---δ����
#if 0
	if(BIT4(BYTE1(FrameBuf3)) != BIT4(BYTE3(buf)))
	{
		if(BIT4(BYTE3(buf)) == 0x01)//�ֱ���
		{
			sendCommand(CMD_JOYSTICK_OK);
		}else if(BIT4(BYTE3(buf)) == 0x00)
		{
			sendCommand(CMD_JOYSTICK_ERR);
		}
	}
	
	if(BIT2(BYTE1(FrameBuf3)) != BIT2(BYTE3(buf)))
	{
		if(BIT2(BYTE3(buf)) == 0x01)//ֹ��������״̬
		{
			sendCommand(CMD_DETEND_LOCK);//ֹ����
		}else if(BIT2(BYTE3(buf)) == 0x00)
		{
			sendCommand(CMD_DETEND_UNLOCK);//ֹ����
		}
	}
	
	if(BIT1(BYTE1(FrameBuf3)) != BIT1(BYTE3(buf)))
	{
		if(BIT1(BYTE3(buf)) == 0x00)//ά����״̬
		{
			sendCommand(CMD_MAINTPORT_LOCK);
		}else if(BIT1(BYTE3(buf)) == 0x01)
		{
			sendCommand(CMD_MAINTPORT_UNLOCK);
		}
	}
	/*******  ADDED 20160218  *********/
	if(BIT3(BYTE1(FrameBuf3)) != BIT3(BYTE3(buf)))
	{
		if(BIT3(BYTE3(buf)) == 0x00)//������� clutch
		{
			sendCommand(CMD_LIHEQI_CLOSE);
		}else if(BIT3(BYTE3(buf)) == 0x01)//�������
		{
			sendCommand(CMD_LIHEQI_OPEN);
		}
	}
	if(BIT1(BYTE2(FrameBuf3)) != BIT1(BYTE4(buf)))
	{
		if(BIT1(BYTE4(buf)) == 0x00)//35�������쳣
		{
			sendCommand(CMD_GRENADE_ERR);
		}else if(BIT1(BYTE4(buf)) == 0x01)//35��������
		{
			sendCommand(CMD_GRENADE_OK);
		}
	}
	if(BIT2(BYTE2(FrameBuf3)) != BIT2(BYTE4(buf)))
	{
		if(BIT2(BYTE4(buf)) == 0x00)//�����쳣
		{
			sendCommand(CMD_FIREBUTTON_ERR);
		}else if(BIT2(BYTE4(buf)) == 0x01)//������
		{
			sendCommand(CMD_FIREBUTTON_OK);
		}
	}
	if(BIT3(BYTE2(FrameBuf3)) != BIT3(BYTE4(buf)))
	{
		if(BIT3(BYTE4(buf)) == 0x00)//ȫ��CANͨ���쳣
		{
			sendCommand(CMD_FULSCREENCAN_ERR);
		}else if(BIT3(BYTE4(buf)) == 0x01)//ȫ��CANͨ����
		{
			sendCommand(CMD_FULSCREENCAN_OK);
		}
	}
	if(BIT5(BYTE2(FrameBuf3)) != BIT5(BYTE4(buf)))
	{
		if(BIT5(BYTE4(buf)) == 0x00)//�Կ�CAN0ͨ���쳣
		{
			sendCommand(CMD_DISCONTRLCAN0_ERR);
		}else if(BIT5(BYTE4(buf)) == 0x01)//�Կ�CAN0ͨ����
		{
			sendCommand(CMD_DISCONTRLCAN0_OK);
		}
	}
	if(BIT6(BYTE2(FrameBuf3)) != BIT6(BYTE4(buf)))
	{
		if(BIT6(BYTE4(buf)) == 0x00)//�Կ�CAN1ͨ���쳣
		{
			sendCommand(CMD_DISCONTRLCAN1_ERR);
		}else if(BIT6(BYTE4(buf)) == 0x01)//�Կ�CAN1ͨ����
		{
			sendCommand(CMD_DISCONTRLCAN1_OK);
		}
	}

	if(BIT0(BYTE3(FrameBuf3)) != BIT0(BYTE5(buf)))
	{
		if(BIT0(BYTE5(buf)) == 0x00)//���-�����״̬�쳣 electro magnet
		{
			sendCommand(CMD_DIANCITIE_ERR);
		}else if(BIT0(BYTE5(buf)) == 0x01)//���-�����״̬��
		{
			sendCommand(CMD_DIANCITIE_OK);
		}
	}
	if(BIT1(BYTE3(FrameBuf3)) != BIT1(BYTE5(buf)))
	{
		if(BIT1(BYTE5(buf)) == 0x00)//���-ˮƽ���״̬�쳣
		{
			sendCommand(CMD_POSMOTOR_ERR);
		}else if(BIT1(BYTE5(buf)) == 0x01)//���-ˮƽ���״̬��
		{
			sendCommand(CMD_POSMOTOR_OK);
		}
	}
	if(BIT2(BYTE3(FrameBuf3)) != BIT2(BYTE5(buf)))
	{
		if(BIT2(BYTE5(buf)) == 0x00)//���-5.8���״̬�쳣
		{
			sendCommand(CMD_MACHGUNMOTOR_ERR);
		}else if(BIT2(BYTE5(buf)) == 0x01)//���-5.8���״̬��
		{
			sendCommand(CMD_MACHGUNMOTOR_OK);
		}
	}
	if(BIT3(BYTE3(FrameBuf3)) != BIT3(BYTE5(buf)))
	{
		if(BIT3(BYTE5(buf)) == 0x00)//���-35����״̬�쳣
		{
			sendCommand(CMD_GRENADEMOTOR_ERR);
		}else if(BIT3(BYTE5(buf)) == 0x01)//���-35����״̬��
		{
			sendCommand(CMD_GRENADEMOTOR_OK);
		}
	}
	
	memcpy(FrameBuf3,buf+2,sizeof(FrameBuf3));
//			break;
#endif
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
#if 0
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
#if 0
	if(isBootUpMode()&&isBootUpSelfCheck()&&(!bWeaponCtrlOK()))
		sendCommand(CMD_WEAPONCTRL_OK);

	stat = WeaponCtrlPORT_recv(pUartObj,1);
	if(stat!=1)
		return;
	WeaponCtrlPORT_ParseFrameId(pUartObj,BYTE2(buf));
#endif
	return ;
}
void WeaponCtrlPORT_ParseBytePanel(unsigned char *buf)
{
#if 0
	if(isBootUpMode()&&isBootUpSelfCheck()&&(!bWeaponCtrlOK()))
		sendCommand(CMD_WEAPONCTRL_OK);

	switch(buf[2])
	{
	case Frame_Type0:
		WeaponCtrlPORT_ParseFrameByte_type0(buf);
		break;
	case Frame_Type1:
		WeaponCtrlPORT_ParseFrameByte_type1(buf);
		break;
	case Frame_Type2:
		WeaponCtrlPORT_ParseFrameByte_type2(buf);
		break;
	case Frame_Type3:
		WeaponCtrlPORT_ParseFrameByte_type3(buf);
		break;
	case Frame_Type4:
		WeaponCtrlPORT_ParseFrameByte_type4(buf);
		break;
	case Frame_TEST:
		WeaponCtrlPORT_ParseFrameByte_test(buf);
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
static void WeaponCtrlPORT_ParseTurret(UartObj*pUartObj)
{
#if 0
	if(!bPositionServoOK())
		sendCommand(CMD_POSITION_SERVO_OK);
	killSelfCheckPosServoTimer();
	startSelfCheckPosServo_Timer();
#endif
}
void WeaponCtrlPORT_ParseByteTurret(unsigned char *buf)
{

}
static void WeaponCtrlPORT_ParseMachGun(UartObj*pUartObj)
{
#if 0
	if(!bMachineGunServoOK())
		sendCommand(CMD_MACHINEGUN_SERVO_OK);
	killSelfCheckMachGunServoTimer();
	startSelfCheckMachGunServo_Timer();
#endif
}
void WeaponCtrlPORT_ParseByteMachGun(unsigned char *buf)
{
	
}

static void WeaponCtrlPORT_ParseGrenade(UartObj*pUartObj)
{
#if 0
	if(!bGrenadeServoOK())
		sendCommand(CMD_GENERADE_SERVO_OK);
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
		//OSA_printf("WeaponCtrlPORT_recvTask\n");
		//OSA_waitMsecs(5000);
//		memset(buf,0,16);
		WeaponCtrlObj.UartPort.recvLen = 0;	

		//stat = WeaponCtrlPORT_recvCfg(pUartObj,2);

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


#ifndef _WEAPONCTRL_H_
#define _WEAPONCTRL_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "basePort.h"

/** ËÅ·þÏÞÎ»ÐÅºÅ: 0 Î´ÏÞÎ» , 1 ÏÞÎ»**/
typedef union SIGNAL_TAG {
	unsigned char byte;
	struct bit_tag{
	unsigned  MachGunB:1;//»úÇ¹¸©½Ç
	unsigned  MachGunE:1;//»úÇ¹Ñö½Ç
	unsigned  MachGunZ:1;//»úÇ¹¹éÁã
	unsigned  MachGunP:1;//»úÇ¹·½Ïò
	unsigned  GrenadeB:1;//Áñµ¯¸©½Ç
	unsigned  GrenadeE:1;//Áñµ¯Ñö½Ç
	unsigned  GrenadeZ:1;//Áñµ¯¹éÁã
	unsigned  GrenadeP:1;//Áñµ¯·½Ïò
	}bit;
}SIGNAL;
extern SIGNAL signal;


typedef struct _WeaponCtrlPort
{
	PortInter_t pInter;
	UartObj UartPort;
}WeaponCtrlPort_t,*WeaponCtrlPORT_Handle;

typedef enum _FrameType{
	Frame_Type0 = 0xA0,
	Frame_Type1 = 0xA1,
	Frame_Type2 = 0xA2,
	Frame_Type3  =0xA3,
	Frame_Type4 = 0xA4,
	Frame_TEST
}FrameType;

typedef enum _Button_Type{
	Button_Base   = 0x0,	//ï¿½ï¿½Ð§
	Button_Right  = 0x1,	//ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½
	Button_Left   = 0x2,	
	Button_UP     = 0x3,	//ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½
	Button_Down   = 0x4,	//ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½
	Button_Enter  = 0x5,	//È·ï¿½ï¿½
	Button_Esc    = 0x6,	//ï¿½Ë³ï¿½
	Button_Unlock = 0x7,	//ï¿½ï¿½ï¿½ï¿½
	Button_AutoCheck  = 0x8//ï¿½Ô¼ï¿½
}Button_Type;

typedef enum _Fire_Type{
	Fire_Base     = 0x00,
	Fire_Gun      = 0x01,
	Fire_Strong   = 0x02,
	Fire_Shot     = 0x03,
	Fire_Reset    = 0x04
}Fire_Type;

typedef enum _Fov_Type{
	FOV_SMALL,
	FOV_LARGE
}Fov_Type;

typedef struct _pve_msg_header{
	unsigned int SyFlag;
	unsigned int uiVersion;		
	//eCMD_TYPE eCmdType;
	unsigned int uiRequestId;
	unsigned int uiSize;		
}PVE_MSG_HEADER, *PPVE_MSG_HEADER;

typedef struct _pve_msg{
	PVE_MSG_HEADER head;
	union{
		unsigned char data[4];
		int  iReplyValue;
	}UN_MSG;
}PVE_MSG, *PPVE_MSG;


//extern SIGNAL signal;
BOOL isMachGunUpLocked();
BOOL isMachGunDownLocked();
BOOL isGrenadeUpLocked();
BOOL isGrenadeDownLocked();
BOOL isMachGunPosLocked();
BOOL isGrenadePosLocked();



void  updateBulletType(int);
int getBulletType();
Fov_Type getFovState();
void requstServoContrl();
void releaseServoContrl(void);
void setSysWorkMode();
BYTE getFrameA3Byte1();
BYTE getFrameA3Byte2();
BYTE getFrameA3Byte3();
BOOL isGunIdentify();
BOOL isIdentifyGas();

char* getWeaponCtrlA1();
char* getFulScrA0();
float getpanoAngleV();
float getpanoAngleH();

void setJoyStickStat(bool stat);
void setGrenadeInPositonFlag(void);
void resetGrenadeInPositionFlag(void);

WeaponCtrlPORT_Handle WeaponCtrlPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void WeaponCtrlPORT_unInitial(WeaponCtrlPORT_Handle handle);
int WeaponCtrlPORT_send(BYTE* pSendBuf,int iLen);

void * WeaponCtrlPORT_recvTask(void * prm);
void processCMD_SERVOTIMER_MACHGUN(LPARAM lParam);
void processCMD_TIMER_SENDFRAME0(LPARAM lParam);
void processCMD_TRACE_SENDFRAME0(LPARAM lParam);
void processCMD_SERVO_INIT(LPARAM lParam);
void setTraceModeSwitch(BOOL context);
int WeaponCtrlPort_ParseByte(BYTE* buf);

void WeaponCtrlPORT_ParseBytePanel(unsigned char *buf);
void startServoServer(BYTE code);
void  killF3Timer();
void killSelfCheckMachGunServoTimer();
void killSelfCheckGrenadeServoTimer();
void killF5Timer();
void killF6Timer();
void killSelfCheckPosServoTimer();
 void startCANSendTimer();
 void startServoCheck_Timer();

 void TeststartServoServer(unsigned char code);
//int TestSendCANBuf(char *buf, int length);
void TestabsPosRequest(BYTE code);
void teststartServo(BYTE code);


extern BOOL bTraceSend;
extern int servoInit;
#ifdef __cplusplus
}
#endif

#endif



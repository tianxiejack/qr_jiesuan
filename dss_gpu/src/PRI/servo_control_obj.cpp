#include "servo_control_obj.h"
#include "WeaponCtrl.h"
#include "permanentStorage.h"
#include "msgDriv.h"
#include "dx.h"
#include "app_user.h"
#include "GrenadePort.h"
#include "statCtrl.h"
#include "firingCtrl.h"
#include "UartCanMessage.h"
#include "UartMessage.h"
#include "GrenadePort.h"
#include "MachGunPort.h"
#include "osdProcess.h"

static void MachServoMoveOffset(float xOffset, float yOffset);
static void GrenadeServoMoveOffset(float xOffset, float yOffset);
static void MachServoMoveSpeed(float xSpeed, float ySpeed);
static void GrenadeServoMoveSpeed(float xSpeed, float ySpeed);
static void MachServoStop();
static void GrenadeServoStop();
extern bool gGrenadeLoadFireFlag;

static IF_servo_control gMachServoControlObj = {
	MachServoMoveOffset,
	MachServoMoveSpeed,
	MachServoStop
};

static IF_servo_control gGrenadeServoControlObj = {
	GrenadeServoMoveOffset,
	GrenadeServoMoveSpeed,
	GrenadeServoStop
};

static IF_servo_control gTurretServoControlObj = {
	TurretServoMoveOffset,
	TurretServoMoveSpeed,
	TurretServoStop
};

#define sendCommand(MSG_ID) MSGDRIV_send(MSG_ID,0)

static void PseudoMoveOffsetFunc(float x, float y){}
static void PseudoMoveSpeedFunc(float x, float y){}
static void PseudoStopFunc(){}
static IF_servo_control gPseudoServoControlObj={
	PseudoMoveOffsetFunc,
	PseudoMoveSpeedFunc,
	PseudoStopFunc,
};


PIF_servo_control pServoControlObj = &gPseudoServoControlObj;


PIF_servo_control getMachGunServoContrlObj()
{
	return &gMachServoControlObj;
}

PIF_servo_control getGrenadeServoContrlObj()
{
	return &gGrenadeServoControlObj;
}

PIF_servo_control getTurretServoContrlObj()
{
	return &gTurretServoControlObj;
}

//-------------------CAN packets--- speed --------------
static char Turretbuf[]		 = {0x03, 0x42, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  Machbuf[] 		 = {0x03, 0x2c, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  Grenadebuf[]	 = {0x03,0x37,0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
/**************** CAN packets         position**************/
static char Pos_Turretbuf[] 	= {0x03, 0x42, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char Pos_Machbuf[] 		= {0x03, 0x2c, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char Pos_Grenadebuf[] 	= {0x03,0x37,0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};

/*********************TEST buf ************************/
static char  TestMachSpebuf[] 	 = {0x03, 0x2c, 0x53, 0x50, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  TestMachPosbuf[] = {0x03,0x2c,0x50,0x52,0x00,0x00,0x00,0x00,0x00,0x00};
static char  TestGRESpebuf[] 	 = {0x03, 0x37, 0x53, 0x50, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  TestGREPosbuf[] = {0x03,0x37,0x50,0x52,0x00,0x00,0x00,0x00,0x00,0x00};
static char  TestTURSpebuf[] 	 = {0x03, 0x42, 0x53, 0x50, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  TestTURPosbuf[] = {0x03,0x42,0x50,0x52,0x00,0x00,0x00,0x00,0x00,0x00};


#define MACH_SERVO_SPEED_RATE  1067
#define MACH_SERVO_RESOLUTION  4096

#define TURRET_SERVO_SPEED_RATE  1239
#define TURRET_SERVO_RESOLUTION  4096

#define GRENADE_SERVO_SPEED_RATE  1067
#define GRENADE_SERVO_RESOLUTION  4096

//#define DEGREE2CANVALUE(degree,name) ((degree)*2*##name##_SERVO_SPEED_RATE*##name##_SERVO_RESOLUTION/(360*60*60))

#define DEGREE2RADS(degree)					((degree)*3600)

#define FILLBUFFSPEED(canbuf, milsecond) {canbuf[2]=0x4A;\
										  canbuf[3]=0x56;\
										  canbuf[4]=canbuf[5]=0x00;\
										  canbuf[6]=milsecond.c[0];\
										  canbuf[7]=milsecond.c[1];\
										  canbuf[8]=milsecond.c[2];\
										  canbuf[9]=milsecond.c[3];\
										  }
#define FILLBUFFOFFST(canbuf, mils) {canbuf[2]=0x50;\
										  canbuf[3]=0x52;\
										  canbuf[4]=canbuf[5]=0x00;\
										  canbuf[6]=mils.c[0];\
										  canbuf[7]=mils.c[1];\
										  canbuf[8]=mils.c[2];\
										  canbuf[9]=mils.c[3];\
										  }
//#define CAN_CMD_SIZE_SHORT	6
//#define CAN_CMD_SIZE_LONG    	10
//#define CAN_CMD_SIZE_MINI		4

#define FILLBUFFBGIN(canbuf) {canbuf[2]=0x42;\
							   canbuf[3]=0x47;\
							   canbuf[4] = canbuf[5] = 0x00;\
							   }
#define FILLBUFFSTOP(canbuf) {canbuf[2]=0x53;\
							   canbuf[3]=0x54;\
							   canbuf[4] = canbuf[5] = 0x00;\
							   }

#define FILLBUFFENABLE(canbuf, enable){canbuf[2]=0x4D;\
							   canbuf[3]=0x4F;\
							   canbuf[4] = canbuf[5] =canbuf[7]=canbuf[8]=canbuf[9]=0x00;\
							   canbuf[6] = enable;\
							   }
#define FILLBUFFMODE(canbuf, mode){canbuf[2]=0x55;\
							   canbuf[3]=0x4D;\
							   canbuf[4] = canbuf[5] =canbuf[7]=canbuf[8]=canbuf[9]=0x00;\
							   canbuf[6] = mode;\
							   }
#define WAITTIME 10//(2*5000)

long cmd_machservo_moveoffset_tmp = 0;
long cmd_grenadeservo_moveoffset_tmp = 0;
long cmd_turretservo_moveoffset_tmp = 0;

int Rads2CANValue(double degree,int id)
{
	double temp = 0.0;
	int re = 0;
	switch(id)
	{
		case TURRET:
			temp = degree*TURRET_SERVO_RESOLUTION*TURRET_SERVO_SPEED_RATE;
			break;
		case MACH:
			temp = degree*MACH_SERVO_RESOLUTION*MACH_SERVO_SPEED_RATE;
			break;
		case GRENADE:
			temp = degree*GRENADE_SERVO_RESOLUTION*GRENADE_SERVO_SPEED_RATE;
			break;
		default:
			break;
	}
	re = (int)(temp/360.0);
	return re;
}

static void TurretServoMoveOffset(float xOffset,float yOffset)
{
}

static void TurretServoMoveSpeed(float xSpeed,float ySpeed)
{
}

static void TurretServoStop()
{
	processCMD_TURRETSERVO_STOP(0);
}

static void MachServoMoveOffset(float xOffset, float yOffset)
{	
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
	double x = xOffset;//OFFSET_TO_ANGLE(xOffset);
	double y = yOffset;//OFFSET_TO_ANGLE(yOffset);
	/*
	x = DEGREE2RADS(x);
	y = DEGREE2RADS(y);
	*/
	xmils.value = Rads2CANValue(x,TURRET);
	ymils.value = Rads2CANValue(y,MACH);
	processCMD_TURRETSERVO_MOVEOFFSET(xmils.value);
	processCMD_MACHSERVO_MOVEOFFSET(ymils.value);
	return ;
}

static void MachServoMoveSpeed(float xSpeed, float ySpeed)
{
	processCMD_MACHSERVO_MOVESPEED(0);
}

static void MachServoStop()
{
	processCMD_MACHSERVO_STOP(0);
}

static void GrenadeServoMoveOffset(float xOffset, float yOffset)
{
	union {
		unsigned char c[4];
		int value;
	} xmils;
	double x = xOffset;
	xmils.value = Rads2CANValue(x,GRENADE);	
	processCMD_GRENADESERVO_MOVEOFFSET(xmils.value);

	
	return ;
}
static void GrenadeServoMoveSpeed(float xSpeed, float ySpeed)
{
	processCMD_GRENADESERVO_MOVESPEED(0);
}
static void GrenadeServoStop()
{
	processCMD_GRENADESERVO_STOP(0);
}

void processCMD_MACHSERVO_MOVEOFFSET(LPARAM lParam)
{	
	union {
		unsigned char c[4];
		int value;
	} ymils;
	#if 0
	ymils.value = 0x56030;
	FILLBUFFSPEED(Machbuf, ymils);
	Machbuf[2] = 0x53;
	Machbuf[3] = 0x50;
	SendCANBuf(Machbuf,10);
	#endif
	ymils.value = lParam;
	FILLBUFFOFFST(Machbuf, ymils);
	SendCANBuf(Machbuf, CAN_CMD_SIZE_LONG);	
	startServo(CODE_MACHGUN);
}
void processCMD_MACHSERVO_MOVESPEED(LPARAM lParam)
{
	return ;
}
void processCMD_MACHSERVO_STOP(LPARAM lParam)
{
	FILLBUFFSTOP(Machbuf);
	SendCANBuf(Machbuf, CAN_CMD_SIZE_SHORT);
}
void processCMD_GRENADESERVO_MOVEOFFSET(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	} xmils;

	xmils.value = 0x56030;
	FILLBUFFSPEED(Grenadebuf, xmils);
	Grenadebuf[2] = 0x53;
	Grenadebuf[3] = 0x50;
	SendCANBuf(Grenadebuf,10);
	
	xmils.value = lParam;
	FILLBUFFOFFST(Grenadebuf, xmils);
	SendCANBuf(Grenadebuf, CAN_CMD_SIZE_LONG);
	
	startServo(CODE_GRENADE);
}
void processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam)
{
	return ;
}
void processCMD_GRENADESERVO_STOP(LPARAM lParam)
{
	//printf("lParam = %x\n",lParam);
	//printf("*lParam = %d\n",*(int *)(lParam));
	FILLBUFFSTOP(Grenadebuf);
	SendCANBuf(Grenadebuf, CAN_CMD_SIZE_SHORT);
}
void processCMD_TURRETSERVO_MOVEOFFSET(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	} xmils;
	#if 0
		xmils.value = 0x1da38;
		FILLBUFFSPEED(Turretbuf, xmils);
		Turretbuf[2] = 0x53;
		Turretbuf[3] = 0x50;
		SendCANBuf(Turretbuf,10);	
	#endif
	
	xmils.value = lParam;
	FILLBUFFOFFST(Turretbuf, xmils);
	SendCANBuf(Turretbuf, CAN_CMD_SIZE_LONG);	
	
	startServo(CODE_TURRET);
}
void processCMD_TURRETSERVO_MOVESPEED(LPARAM lParam)
{
	#if 0
	union {
		unsigned char c[4];
		int value;
	} milsecond;
	milsecond.value = 1000000;
	FILLBUFFSPEED(Turretbuf, milsecond);	
	SendCANBuf(Turretbuf, CAN_CMD_SIZE_LONG);
	FILLBUFFBGIN(Turretbuf);
	SendCANBuf(Turretbuf, CAN_CMD_SIZE_SHORT);	
	#endif
	return ;
}
void processCMD_TURRETSERVO_STOP(LPARAM lParam)
{
	FILLBUFFSTOP(Turretbuf);
	SendCANBuf(Turretbuf, CAN_CMD_SIZE_SHORT);
}
void processCMD_ALLSERVO_STOP(LPARAM lParam)
{	
	FILLBUFFSTOP(TestGREPosbuf);
	SendCANBuf((TestGREPosbuf), CAN_CMD_SIZE_SHORT);

	FILLBUFFSTOP(TestTURPosbuf);
	SendCANBuf((TestTURPosbuf), CAN_CMD_SIZE_SHORT);

	FILLBUFFSTOP(TestMachPosbuf);
	SendCANBuf((TestMachPosbuf), CAN_CMD_SIZE_SHORT);
	releaseServoContrl();
}

static void initServo(unsigned char *pBuf)
{
}
void initAllServos()
{
}
void setServoControlObj()
{
	if(isMachineGun())
		pServoControlObj=getMachGunServoContrlObj();
	else
		pServoControlObj=getGrenadeServoContrlObj();
}

void testchushihua()
{
	TeststartServoServer(CODE_GRENADE);
	TeststartServoServer(CODE_MACHGUN);
	TeststartServoServer(CODE_TURRET);
	return ;
}

void testjiqiangqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	double x = 0.0,y=0.0;

	x = 10;
	#if 0
	ymilsecond.value = -182800;;
	FILLBUFFSPEED(TestGRESpebuf, ymilsecond);
	TestGRESpebuf[2] = 0x4a;
	TestGRESpebuf[3] = 0x56;
	SendCANBuf(TestGRESpebuf,10);
	//startServo(CODE_GRENADE);
	#endif
	#if 0
	ymilsecond.value = Rads2CANValue(x,GRENADE);
	FILLBUFFOFFST(TestGRESpebuf, ymilsecond);
	TestGRESpebuf[2] = 0x50;
	TestGRESpebuf[3] = 0x52;
	SendCANBuf(TestGRESpebuf,10);

	FILLBUFFSPEED(TestGRESpebuf, ymilsecond);
	TestGRESpebuf[2] = 0x53;
	TestGRESpebuf[3] = 0x50;
	SendCANBuf(TestGRESpebuf,10);
	#endif
	x=-1;
	ymilsecond.value = Rads2CANValue(x,MACH);//TURRET   //GRENADE //MACH
	FILLBUFFOFFST(TestMachPosbuf, ymilsecond);  //TestTURPosbuf   //TestGREPosbuf   //TestMachPosbuf
	TestMachPosbuf[2] = 0x50;
	TestMachPosbuf[3] = 0x52;
	SendCANBuf(TestMachPosbuf,10);

	startServo(CODE_MACHGUN); //CODE_TURRET   //CODE_GRENADE

}

void testliudanqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	double x = 0.0,y=0.0;
	x=3;
	ymilsecond.value = Rads2CANValue(x,GRENADE);
	FILLBUFFOFFST(TestGREPosbuf, ymilsecond);
	TestGREPosbuf[2] = 0x50;
	TestGREPosbuf[3] = 0x52;
	SendCANBuf(TestGREPosbuf,10);

	startServo(CODE_GRENADE);
	#if 0
	FILLBUFFSPEED(TestTURPosbuf, ymilsecond);
	TestTURPosbuf[2] = 0x53;
	TestTURPosbuf[3] = 0x50;
	SendCANBuf(TestTURPosbuf,10);

	
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	double x = 0.0,y=0.0;

	x = DEGREE2RADS(90);
	
	ymilsecond.value = 500;//8000;
	FILLBUFFSPEED(TestGRESpebuf, ymilsecond);
	TestGRESpebuf[2] = 0x4a;
	TestGRESpebuf[3] = 0x56;
	SendCANBuf(TestGRESpebuf,10);
	
	ymilsecond.value = Rads2CANValue(x,GRENADE);
	ymilsecond.value = -ymilsecond.value;
	FILLBUFFOFFST(TestGRESpebuf, ymilsecond);
	//TestMachPosbuf[2] = 0x44;
	//TestMachPosbuf[3] = 0x43;
	SendCANBuf(TestGRESpebuf,10);
	
	startServo(CODE_GRENADE);
	#endif
}

void testturdanqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	double x = 0.0,y=0.0;

	x = DEGREE2RADS(0.8);
	
	ymilsecond.value = 500;//8000;
	FILLBUFFSPEED(TestTURSpebuf, ymilsecond);
	TestTURSpebuf[2] = 0x4a;
	TestTURSpebuf[3] = 0x56;
	SendCANBuf(TestTURSpebuf,10);
	
	ymilsecond.value = Rads2CANValue(x,MACH);
	FILLBUFFOFFST(TestTURSpebuf, ymilsecond);
	//TestMachPosbuf[2] = 0x44;
	//TestMachPosbuf[3] = 0x43;
	SendCANBuf(TestTURSpebuf,10);
	
	startServo(CODE_TURRET);
}

void teststopserver()
{
	FILLBUFFSTOP(TestGREPosbuf);
	SendCANBuf((TestGREPosbuf), CAN_CMD_SIZE_SHORT);
}

void Grenade2Mach_cbFxn(long lParam)
{
	static bool firstgrenademoveflag = 1;

	#if 1
	static int times = 0;
	if(times++ > 20)
	{
		times = 0;
		//gGrenadeLoadFireFlag = 0;
		//display jiu xu
		//OSDCTRL_ItemShow(eReady);
		//startDisplayJiuXuTimer();
		#if 1
		if(isBattleMode() && getProjectileType() == PROJECTILE_GRENADE_KILL && gGrenadeLoadFireFlag)
		{
			servoLookupGetPos();
			killDisplayJiuXutimer();
			double tmp = getGrenadeDestTheta() - getGrenadeAngleAbs();
		printf("111tmp= %f\n",tmp);
		printf("111gGrenadeGetPos= %d\n",gGrenadeGetPos);
		printf("gTurretGetPos= %d\n",gTurretGetPos);
		printf("gMachGetPos= %d\n",gMachGetPos);		
			if(firstgrenademoveflag)
			{
				printf("1111111111111111 \n\n");
				getGrenadeServoContrlObj()->moveOffset(tmp,0);
				firstgrenademoveflag = 0;
			}
			else if(gGrenadeGetPos == -1)  // why return -1????
			{
			printf("22222222222 \n\n");
				if(tmp > 1.5)
					getGrenadeServoContrlObj()->moveOffset(tmp,0);
				else if(tmp < -1.5)
					getGrenadeServoContrlObj()->moveOffset(tmp,0);
				else
				{
					printf("done \n\n");
					gGrenadeLoadFireFlag = 0;
					//display jiu xu
					OSDCTRL_ItemShow(eReady);
					startDisplayJiuXuTimer();
					firstgrenademoveflag = 1;
				}
			}
		}
		#endif
	}
	#endif
	#if 0
	else if(isBattleMode() && getProjectileType() == PROJECTILE_GRENADE_KILL)
	{
		double tmp = getMachGunAngleAbs() - getGrenadeAngleAbs() - getMach2GrenadeAngle();

		#if 0
		printf("\n____________________________________________\n");	
		printf("Mach angle = %f\n ",getMachGunAngleAbs());
		printf("Grenade angle = %f\n ",getGrenadeAngleAbs());	
		printf("\n____________________________________________\n");	
		#endif
		if(tmp > 2)		//when >1   will swing
		{
			//if(tmp > 2)
				getGrenadeServoContrlObj()->moveOffset(1,0);
			//else
			//	getGrenadeServoContrlObj()->moveOffset(0.5,0);
		}
		else if(tmp < -2)
		{
		 	//if(tmp < -2)
				getGrenadeServoContrlObj()->moveOffset(-1,0);
			//else
			//	getGrenadeServoContrlObj()->moveOffset(-0.5,0);
		}
	}
	#endif
	return ;
}

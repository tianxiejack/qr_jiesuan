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
//#include "dramMsgDef.h"
//#include "main.h"

static void MachServoMoveOffset(float xOffset, float yOffset);
static void GrenadeServoMoveOffset(float xOffset, float yOffset);
static void MachServoMoveSpeed(float xSpeed, float ySpeed);
static void GrenadeServoMoveSpeed(float xSpeed, float ySpeed);
static void MachServoStop();
static void GrenadeServoStop();


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
static unsigned char Turretbuf[]		 = {0x03, 0x42, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char  Machbuf[] 		 = {0x03, 0x2c, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char  Grenadebuf[]	 = {0x03,0x37,0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
/**************** CAN packets         position**************/
static unsigned char Pos_Turretbuf[] 	= {0x03, 0x42, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char Pos_Machbuf[] 		= {0x03, 0x2c, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char Pos_Grenadebuf[] 	= {0x03,0x37,0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};

/*********************TEST buf ************************/
static char  TestMachSpebuf[] 	 = {0x03, 0x2c, 0x53, 0x50, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  TestMachPosbuf[] = {0x03,0x2c,0x50,0x52,0x00,0x00,0x00,0x00,0x00,0x00};

#define MACH_SERVO_SPEED_RATE  1139
#define MACH_SERVO_RESOLUTION  4096

#define TURRET_SERVO_SPEED_RATE 1139
#define TURRET_SERVO_RESOLUTION  4096

#define GRENADE_SERVO_SPEED_RATE 1139
#define GRENADE_SERVO_RESOLUTION  4096

//#define DEGREE2CANVALUE(degree,name) ((degree)*2*##name##_SERVO_SPEED_RATE*##name##_SERVO_RESOLUTION/(360*60*60))

#define DEGREE2CANVALUE_MACH(degree)		((degree)*2*MACH_SERVO_SPEED_RATE*MACH_SERVO_RESOLUTION)/(360*60*60)
#define DEGREE2CANVALUE_TURRET(degree)		((degree)*2*TURRET_SERVO_SPEED_RATE*TURRET_SERVO_RESOLUTION)/(360*60*60)
#define DEGREE2CANVALUE_GRENADE(degree)		((degree)*2*GRENADE_SERVO_SPEED_RATE*GRENADE_SERVO_RESOLUTION)/(360*60*60)



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

static int DEGREE2CANVALUE(double degree,int id)
{
	double temp = 0.0;
	int re = 0;
	if(id == TURRET)
	{
		temp = degree*2*TURRET_SERVO_RESOLUTION*TURRET_SERVO_SPEED_RATE;
	}
	else if(id == MACH)
	{
		temp = degree*2*MACH_SERVO_RESOLUTION*TURRET_SERVO_SPEED_RATE;
	}
	else if(id == GRENADE)
	{
		temp = degree*2*GRENADE_SERVO_RESOLUTION*TURRET_SERVO_SPEED_RATE;
	}
	re = (int)(temp/(360*60*60));
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
	
}

static void MachServoMoveOffset(float xOffset, float yOffset)
{
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
	// pixel vert to degree
	double x = OFFSET_TO_ANGLE(xOffset);
	double y = OFFSET_TO_ANGLE(yOffset);


	xmils.value = DEGREE2CANVALUE(x,TURRET);
	ymils.value = DEGREE2CANVALUE(y,MACH);
	MSGDRIV_send(CMD_TURRETSERVO_MOVEOFFSET,&(xmils.value));
	MSGDRIV_send(CMD_MACHSERVO_MOVEOFFSET, &(ymils.value));
	return ;
}

static void MachServoMoveSpeed(float xSpeed, float ySpeed)
{
}

static void MachServoStop()
{
}

static void GrenadeServoMoveOffset(float xOffset, float yOffset)
{
}
static void GrenadeServoMoveSpeed(float xSpeed, float ySpeed)
{
}
static void GrenadeServoStop()
{
}

void processCMD_MACHSERVO_MOVEOFFSET(LPARAM lParam)
{
}
void processCMD_MACHSERVO_MOVESPEED(LPARAM lParam)
{
}
void processCMD_MACHSERVO_STOP(LPARAM lParam)
{
}
void processCMD_GRENADESERVO_MOVEOFFSET(LPARAM lParam)
{
}
void processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam)
{
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
	//ymilsecond.value = 112800;
	ymilsecond.value = 90000;
	FILLBUFFSPEED(TestMachSpebuf, ymilsecond);
	TestMachPosbuf[2] = 0x53;
	TestMachPosbuf[3] = 0x50;
	SendCANBuf(TestMachSpebuf,10);
	ymilsecond.value = 5000;//8000;
	//FILLBUFFOFFST(TestMachPosbuf, ymilsecond);
	//TestMachPosbuf[2] = 0x41;
	//TestMachPosbuf[3] = 0x43;
	//SendCANBuf(TestMachPosbuf,10);
	teststartServo(CODE_MACHGUN);
}

void testliudanqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 1000;
	FILLBUFFSPEED(TestMachSpebuf, ymilsecond);
	TestMachPosbuf[2] = 0x53;
	TestMachPosbuf[3] = 0x50;
	SendCANBuf(TestMachSpebuf,10);
	//ymilsecond.value = 5000;//8000;
	//FILLBUFFOFFST(TestMachPosbuf, ymilsecond);
	//TestMachPosbuf[2] = 0x44;
	//TestMachPosbuf[3] = 0x43;
	//SendCANBuf(TestMachPosbuf,10);
	teststartServo(CODE_MACHGUN);
}


void teststopserver()
{
	FILLBUFFSTOP(TestMachPosbuf);
	SendCANBuf((TestMachPosbuf), CAN_CMD_SIZE_SHORT);
}


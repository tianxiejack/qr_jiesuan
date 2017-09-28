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

//-------------------CAN packets--- speed --------------
static unsigned char Turretbuf[]		 = {0x03, 0x42, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char  Machbuf[] 		 = {0x03, 0x2C, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char  Grenadebuf[]	 = {0x03,0x37,0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
/**************** CAN packets         position**************/
static unsigned char Pos_Turretbuf[] 	= {0x03, 0x42, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char Pos_Machbuf[] 		= {0x03, 0x2C, 0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};
static unsigned char Pos_Grenadebuf[] 	= {0x03,0x37,0x50, 0x52, 0x00, 0x00,0x00,0x00,0x00,0x00};

/*********************TEST buf ************************/
static char  TestMachbuf[] 	 = {0x03, 0x2C, 0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};
static char  TestGrenadebuf[] = {0x03,0x37,0x4A, 0x56, 0x00, 0x00,0x00,0x00,0x00,0x00};



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

static void MachServoMoveOffset(float xOffset, float yOffset)
{
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
	//1.pixel to milGradiant/s X, Y,
	double x = OFFSET_TO_ANGLE(xOffset);
	double y = OFFSET_TO_ANGLE(yOffset);
	{
		static int counter = 11;
		if(counter-- <= 0)
			counter = 10;
		if(counter < 10)
			return;
	}
	

	//mili Radian second
	x= 3600*MIL2DEGREE(x);
	y= 100*MIL2DEGREE(y);
//	y= 3600*MIL2DEGREE(y);
		
	//2.form CAN packet X,
    	//xmils.value = DEGREE2CANVALUE(x,TURRET);
    	xmils.value = DEGREE2CANVALUE_TURRET(x);
/*	FILLBUFFOFFST(Turretbuf, xmils);	
	//3.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
*/	
	//4.form CAN packet Y,
//	ymils.value = DEGREE2CANVALUE(y,MACH);
	//SendMessage(CMD_MACHSERVO_MOVEOFFSET, DEGREE2CANVALUE(y,MACH));
	//MSGDRIV_send(CMD_MACHSERVO_MOVEOFFSET, (void *)DEGREE2CANVALUE(y,MACH));
	cmd_machservo_moveoffset_tmp = DEGREE2CANVALUE_MACH(y);
	MSGDRIV_send(CMD_MACHSERVO_MOVEOFFSET, 0);

	
/*	FILLBUFFOFFST(Machbuf, ymils);
	
	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
*/
}
void processCMD_GRENADESERVO_MOVEOFFSET(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
	//ymils.value = lParam*100;
	//ymils.value = (lParam)*2*(4096/6000.0);
	ymils.value = (cmd_grenadeservo_moveoffset_tmp)*2*(4096/6000.0);
#if 0
	FILLBUFFOFFST(Machbuf, ymils);

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
#elif 0
	FILLBUFFOFFST(Grenadebuf, ymils);

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Grenadebuf);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
#elif 1
	FILLBUFFOFFST(Turretbuf, ymils);//水平角度伺服转动

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	OSA_waitMsecs(10);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	OSA_waitMsecs(10);
#endif
}

void processCMD_MACHSERVO_MOVEOFFSET(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
//	ymils.value = MIL2DEGREE(lParam)*3600*2*(4464640/360/60/60/1090);
	ymils.value = (cmd_machservo_moveoffset_tmp)*2*(4096/6000.0);
#if 0
	FILLBUFFOFFST(Machbuf, ymils);//58机枪伺服转动

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
#elif 1
	FILLBUFFOFFST(Grenadebuf, ymils);//35榴弹伺服转动

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	//Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Grenadebuf);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	//Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
#elif 1
	FILLBUFFOFFST(Turretbuf, ymils);//水平角度伺服转动

	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
#endif
	return ;
}
static void MachServoMoveSpeed(float xSpeed, float ySpeed)
{
/*	{
		static int counter = 6;
		if(counter-- <= 0)
			counter = 5;
		if(counter < 5)
			return;
	}

	//1.pixel to milGradiant/s X, Y,
	double x = OFFSET_TO_ANGLE(xSpeed);
	double y = OFFSET_TO_ANGLE(ySpeed);
	//mili Radian second
	x= 3600*MIL2DEGREE(x);
	y= 3600*MIL2DEGREE(y);
		
	//2.form CAN packet X,
    xmilsecond.value = DEGREE2CANVALUE(x,TURRET);
	FILLBUFFSPEED(Turretbuf, xmilsecond);	
	//3.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	MYBOARD_waitusec(WAITTIME);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	MYBOARD_waitusec(WAITTIME);
	
	//4.form CAN packet Y,
	ymilsecond.value = DEGREE2CANVALUE(y,MACH);
	*/
	union {
		unsigned char c[4];
		int value;
	} xmilsecond, ymilsecond;
	ymilsecond.value = 0x100790;
	FILLBUFFSPEED(Machbuf, ymilsecond);
	
	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}

void processCMD_MACHSERVO_MOVESPEED(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	
	static int counter = 11;
	if(counter-- <= 0)
		counter = 10;
	if(counter < 10)
		return;
	ymilsecond.value = 0x790;
	FILLBUFFSPEED(Machbuf, ymilsecond);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
}

void processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam)
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 0x100790;
	FILLBUFFSPEED(Grenadebuf, ymilsecond);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Grenadebuf);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);	
}

static void MachServoStop()
{
	//1.form CAN packet X,
	FILLBUFFSTOP(Turretbuf);

	//2.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	//3.form CAN packet Y,
	FILLBUFFSTOP(Machbuf);
	
	//4.send CAN packet Y to mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}
void processCMD_MACHSERVO_STOP(LPARAM lParam)
{
	//3.form CAN packet Y,
	FILLBUFFSTOP(Machbuf);
	
	//4.send CAN packet Y to mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}
//----Grenade-------------------
static void GrenadeServoMoveOffset(float xOffset, float yOffset)
{
	union {
		unsigned char c[4];
		int value;
	} xmils, ymils;
//------------First: let the len mounted on machine gun follow up the target--------------
	//1.pixel to milGradiant/s X, Y,
	double x = OFFSET_TO_ANGLE(xOffset);
	double y = OFFSET_TO_ANGLE(yOffset);
	//mili Radian second
	x= 3600*MIL2DEGREE(x);
	y= 3600*MIL2DEGREE(y);
		
	//2.form CAN packet X,
    	//xmils.value = DEGREE2CANVALUE(x,TURRET);
	xmils.value = DEGREE2CANVALUE_TURRET(x);
	FILLBUFFOFFST(Turretbuf, xmils);	
	//3.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	//4.form CAN packet Y machgun,
	//ymils.value = DEGREE2CANVALUE(y,MACH);
	ymils.value = DEGREE2CANVALUE_MACH(y);
	FILLBUFFOFFST(Machbuf, ymils);
	
	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Machbuf);
	WeaponCtrlPORT_send(Machbuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	
//------------second: let the grenade move up to the aiming angle------	
	//y.form CAN packet Y grenade ,
	//delta Y
	y = getGrenadeDestTheta() - getGrenadeAngle();
	if( 0.2 < DEGREE2MIL(y)) // entered shooting threshold
	{
	//grenade is in position
		sendCommand(CMD_GRENADE_LOAD_IN_POSITION);
	}
	//ymils.value = DEGREE2CANVALUE(y,GRENADE);
	ymils.value = DEGREE2CANVALUE_GRENADE(y);
	FILLBUFFOFFST(Grenadebuf, ymils);
	
	//5.send CAN packet Y to Mach gun servo
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Grenadebuf);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);	
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}

static void GrenadeServoMoveSpeed(float xSpeed, float ySpeed)
{
	union {
		unsigned char c[4];
		int value;
	} xmilsecond, ymilsecond;
		
	//1.pixel to milGradiant/s X, Y,
	double x = OFFSET_TO_ANGLE(xSpeed);
	double y = OFFSET_TO_ANGLE(ySpeed);
	//mili Radian second
	x= 3600*MIL2DEGREE(x);
	y= 3600*MIL2DEGREE(y);

	//2.form CAN packet X,
   // xmilsecond.value = DEGREE2CANVALUE(x,TURRET);
	 xmilsecond.value = DEGREE2CANVALUE_TURRET(x);
	FILLBUFFSPEED(Turretbuf, xmilsecond);	
	//3.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Turretbuf);
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);	
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
//  I only need to let Turret rotate to the position. machine gun will not raise, yet grenade shall raise.
//todo: cannot use PID to control grenade speed-- will use position instead which is no easy task....
	//3. form CAN packet Y2,
	//ymilsecond.value = DEGREE2CANVALUE(y,GRENADE);
	ymilsecond.value = DEGREE2CANVALUE_GRENADE(y);
	FILLBUFFSPEED(Grenadebuf, ymilsecond);
	
	//4. send CAN packet Y2 to Machine gun servo
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	FILLBUFFBGIN(Grenadebuf);
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);	
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}

static void GrenadeServoStop()
{
	//1.form CAN packet X,
	FILLBUFFSTOP(Turretbuf);

	//2.sendCAN packet X to Turret servo,
	WeaponCtrlPORT_send(Turretbuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);	
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	//3.form CAN packet Y,
	FILLBUFFSTOP(Grenadebuf);
	
	//4.send CAN packet Y to Grenade servo
	WeaponCtrlPORT_send(Grenadebuf, CAN_CMD_SIZE_SHORT);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
}
static void initServo(unsigned char *pBuf)
{	
//disable Motor;
	FILLBUFFENABLE(pBuf ,0x0);
	WeaponCtrlPORT_send(pBuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	//enable speed mode	
	FILLBUFFMODE(pBuf, 0x02);
	WeaponCtrlPORT_send(pBuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	//enable Motor;	
	FILLBUFFENABLE(pBuf, 0x01);
	WeaponCtrlPORT_send(pBuf, CAN_CMD_SIZE_LONG);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	
}

void initAllServos()
{
// start CAN
	static const unsigned char startCanbuf[]={0x00, 0x00, 0x01, 0x00};
	WeaponCtrlPORT_send((unsigned char*)startCanbuf, CAN_CMD_SIZE_MINI);
	//MYBOARD_waitusec(WAITTIME);
	Dx_setTimer(BOARD_waitusec_TIMER,WAITTIME);
	
	initServo(Turretbuf);
	initServo(Machbuf);
	initServo(Grenadebuf);

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
	//TeststartServoServer(CODE_TURRET);
	return ;
}

void testjiqiangqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 8000;
	//FILLBUFFSPEED(TestMachbuf, ymilsecond);
	ymilsecond.value = (2)*2*(4096/6000.0);
	FILLBUFFOFFST(TestMachbuf,ymilsecond);
	TestSendCANBuf(TestMachbuf,10);
	usleep(500);
	FILLBUFFBGIN(TestMachbuf);
	TestSendCANBuf(TestMachbuf,6);
	usleep(500);
}

void testliudanqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 3500;
	ymilsecond.value = (2)*2*(4096/6000.0);
	FILLBUFFOFFST(TestMachbuf,ymilsecond);
	//FILLBUFFSPEED(TestGrenadebuf, ymilsecond);
	TestSendCANBuf(TestGrenadebuf,10);
	usleep(500);
	FILLBUFFBGIN(TestGrenadebuf);
	TestSendCANBuf(TestGrenadebuf,6);
	usleep(500);
}


void testFjiqiangqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = -8000;
	FILLBUFFSPEED(TestMachbuf, ymilsecond);
	TestSendCANBuf(TestMachbuf,10);
	usleep(500);
	FILLBUFFBGIN(TestMachbuf);
	TestSendCANBuf(TestMachbuf,6);
	usleep(500);
}

void testFliudanqidong()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = -10000;
	FILLBUFFSPEED(TestGrenadebuf, ymilsecond);
	TestSendCANBuf(TestGrenadebuf,10);
	usleep(500);
	FILLBUFFBGIN(TestGrenadebuf);
	TestSendCANBuf(TestGrenadebuf,6);
	usleep(500);
}

void teststopserver()
{
	FILLBUFFSTOP(TestMachbuf);
	TestSendCANBuf((TestMachbuf), CAN_CMD_SIZE_SHORT);
	usleep(5000);

	FILLBUFFSTOP(TestGrenadebuf);
	TestSendCANBuf((TestGrenadebuf), CAN_CMD_SIZE_SHORT);
	usleep(5000);	
}

void testliudanjiasu()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	if(ymilsecond.value <= 112800)
		ymilsecond.value +=5000;
	FILLBUFFSPEED(TestGrenadebuf, ymilsecond);
	TestSendCANBuf(TestGrenadebuf, CAN_CMD_SIZE_LONG);
	usleep(500);
	FILLBUFFBGIN(TestGrenadebuf);
	TestSendCANBuf(TestGrenadebuf, CAN_CMD_SIZE_SHORT);	
	usleep(500);	
}

void testjiqiangjiasu()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 1000;
	if(ymilsecond.value <= 192800)
		ymilsecond.value +=5000;
	FILLBUFFSPEED(TestMachbuf, ymilsecond);
	TestSendCANBuf(TestMachbuf, CAN_CMD_SIZE_LONG);
	usleep(500);
	FILLBUFFBGIN(TestMachbuf);
	TestSendCANBuf(TestMachbuf, CAN_CMD_SIZE_SHORT);	
	usleep(500);	
}

void testliudanjiansu()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 1000;
	if(ymilsecond.value >= -192800)
		ymilsecond.value -=5000;
	FILLBUFFSPEED(TestGrenadebuf, ymilsecond);
	TestSendCANBuf(TestGrenadebuf, CAN_CMD_SIZE_LONG);
	usleep(500);
	FILLBUFFBGIN(TestGrenadebuf);
	TestSendCANBuf(TestGrenadebuf, CAN_CMD_SIZE_SHORT);	
	usleep(500);	
}

void testjiqiangjiansu()
{
	union {
		unsigned char c[4];
		int value;
	}ymilsecond;
	ymilsecond.value = 1000;
	if(ymilsecond.value >= 2000)
		ymilsecond.value -=5000;
	FILLBUFFSPEED(TestMachbuf, ymilsecond);
	TestSendCANBuf(TestMachbuf, CAN_CMD_SIZE_LONG);
	usleep(500);
	FILLBUFFBGIN(TestMachbuf);
	TestSendCANBuf(TestMachbuf, CAN_CMD_SIZE_SHORT);	
	usleep(500);
}






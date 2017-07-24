/*
 *  Copyright 2009 by QianRun Incorporated.
 *  All rights reserved. Property of QianRun Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) 11/9/2009 AVT VERSION 1" */
/*
 *  ======== osd.c ========
 */

//#include <std.h>
//#include "main.h"
//#include "MuxPortTrans.h"
//#include "avtMsgDef.h"
//#include "byteParser_util.h"
//#include "permanentStorage.h"
//#include "statCtrl.h"
//#include "avtDevice.h"
//#include "FiringCtrl.h"

//#include "osd_graph.h"
#include "osdProcess.h"
#include "Tasklist.h"


#define ARGSCALE	 0.0054931640625
#define ARGSPEEDSCAL 0.00274658203125

#define LOFFSET	 0
#define ROFFSET  0
//extern volatile int msgLight;
#define MIDMENU

OSDCTRL_Handle  pOsdCtrlObj		= NULL;
//CVideoDev* pVideoCapObj				=NULL;


char tmparray[12] = "abcdef";
#if 1
const char ModeOsd[9][8]=
{
	"INT","CAG","STO","SCA","SLA","MAN","TRK","ADJ","ATA"
};
const char SensOsd[2][8]=
{
	"TVS","FLR"
};
const char FovOsd[4][4]=
{
	"W","M","N","Z"
};
const char LaserStatOsd[2][4]=
{
	"LD:","RF:"
};

const char FlirOsd[3][16]=
{
	{""},
	{"FLIR OFF"},
 	{"FLIR OK "}
};
const char TrackStatOsd[4][8]=
{
	{"TRK"},{"CST"},{"LST"},{"ARQ"}
};

#endif

volatile unsigned char g_FilrBack=0;

char WorkOsd[10][8];	//����ģʽ
char AimOsd[2][8];
char GunOsd[5][8];	//ǹ������
char ShotOsd[2][8];//���ģʽ:��
char ShotGunOsd[2][8];//���ģʽ:��ǹ
char LaserOsd[2][8];


char EnhanceOsd[2][8]={
	"","V"
};
char MeasureErrOsd[LASERERR_COUNT][8]={
	"TMOV","ECNL","SMNL"
};
char ErrorOsd[19][8]={
	"E1101",//��Ƶ
	"E1102",//�ֱ�
	"E4103",//��λ�Ƕȴ����������쳣
	"E3104",//�ŷ������쳣
	"E2203",//d1 d2
	"E5204",//
	"E4205",
	"E5205",//
	"E4206",//
	"E7207",//
	"E6306",//
	"E5307",//
	"E8308",//
	"E8309",//
	"E9410",//
	"E3455",//
	"",
	"",
	""
};
char GenPramMenu[4][12]={
"0.FieldView",
"1.FireCtrl",
"2.SysServoX",
"3.SysServoY"
};
char FovTypeOsd[2][8];	//�ӳ�����
char SuperOsd[7][8];
char DynamicOsd[9][8];
char ResultOsd[2][8];
char OpenCloseOsd[2][8];
char CalibZeroOsd[3][8];
char MeasureTypeOsd[6][8];
char AngleCorrectOsd[3][8]={
	"GQ","RGQ","NGQ"
};
char SaveYesNoOsd[2][8]={
	"YES>","NO>"
};
char *Posd[OSD_TEXT_SIZE]={NULL};
char CalibGeneralOsd[3][8];
int General[14]={0,0,0,0,0,0,0,0,0,0,0};
int Weather[10]={1,0,4,0,1,0,1,3,2,5};
int Pressure=101325,DistanceManual=0,Temparature=15;//������� ѹ�� ����
int DisValue[4]={0,0,0,0};
double AngleGun=0.0, AngleGrenade=0.0,PositionX=0.0,PositionY=0.0;//ǹ�ڵĶ���Ƕ���Ϣ �¶� �����ˮƽ�͸����Ƕ�ֵ
int n=0,ShinId=eCalibGeneral_XPole;
unsigned char CalibGenPram_VFLD[48]={0};
int CCD_Big[2]={1200000,900000},CCD_Small[2]={430000,330000},IR_Big[2]={11600000,11200000};
unsigned char TimeValue[8]={0};
int MachTime=0,GrenadeTime=180;
int codeX=0,signX='+', codeY=0,signY='+';
float addX=0.01,addY=0.01;
float ServoX[15]={30.0, 3.59, 2.69, 2.0, 0.8, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float ServoY[10]={30.0,3.4, 2.6, 2.0, 0.7, 0.4, 0.0, 0.0, 0.0, 0.0};
BOOL SHINE=FALSE;

//extern PROJECTILE_TYPE gProjectileType;
//extern WeatherItem gWeatherTable;

PROJECTILE_TYPE gProjectileType=PROJECTILE_BULLET;
PROJECTILE_TYPE gProjectileTypeBefore=PROJECTILE_BULLET;
WeatherItem gWeatherTable={15,101325};
SEM_ID osdSem;


OSDText_Obj g_Text[OSD_TEXT_SIZE]=
{
	{eModeId,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	40+LOFFSET,		40,	0,	{0}},
	{eWorkMode,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	10+LOFFSET,		10,	0,	{0}},
	{eAimType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	140+LOFFSET,	10,	0,	{0}},
	{eGunType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,	10,	0,	{0}},
	{eMeasureType,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	400+LOFFSET,	10,	0,	{0}},
	{eVideoErr,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	334+LOFFSET,	200,	0,	{0}},
	{eShotType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	540+LOFFSET,	10,	0,	{0}},
	{eFovType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	630+LOFFSET,	10,	0,	{0}},
	{eEnhance,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	646+LOFFSET,	10,	0,	{0}},
	{eMeasureDis,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	430+LOFFSET,	10,	0,	{0}},
	{eLaserState,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	470+LOFFSET,	10,	0,	{0}},
	{eSuperOrder,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	630+LOFFSET,	470,	0,	{0}},
	{eErrorZone,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	10+LOFFSET,		460,	0,	{0}},
	{eWeather1,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	430+LOFFSET,	505,	0,	{0}},
	{eWeather2,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	430+LOFFSET,	540,	0,	{0}},
	{eAngleH,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	10+LOFFSET,		500,	0,	{0}},
	{eAngleV,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	10+LOFFSET,		540,	0,	{0}},
	{eDynamicZone,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	180+LOFFSET,	520,	0,	{0}},
	{eCorrectionTip,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,	520,	0,	{0}},
	{eF1Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	50+LOFFSET,		532,	0,	{0}},
	{eF2Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	160+LOFFSET,	532,	0,	{0}},
	{eF3Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	275+LOFFSET,	532,	0,	{0}},
	{eF4Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	382+LOFFSET,	532,	0,	{0}},
	{eF5Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500-5+LOFFSET,		532,	0,	{0}},
	{eF6Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	600+LOFFSET,		532,	0,	{0}},
	{eCalibMenu_Weather,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		65,	0,	{0}},
	{eCalibMenu_Zero,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		100,	0,	{0}},
	{eCalibMenu_General,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		135,	0,	{0}},
	{eCalibMenu_Save,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		170,	0,	{0}},
	{eCalibMenu_GenPram,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		205,	0,	{0}},
	{eCalibMenu_Horizen,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		240,	0,	{0}},
	{eCalibMenu_Laser,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		275,	0,	{0}},
	{eCalibMenu_Child,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		205,	0,	{0}},
	
	{eCursorX,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	180+LOFFSET,		170,	0,	{0}},
	{eSaveYesNo,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		170,	0,	{0}},
	{eCalibMenu_SaveOK,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	180+LOFFSET,		65,	0,	{0}},
	{eSelfCheckResult,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		65,	0,	{0}},
#ifdef MIDMENU
	{eSelfCheckState1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		100-3+70,	0,	{0}},
	{eSelfCheckState2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		135-6+70,	0,	{0}},
	{eSelfCheckState3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		170-9+70,	0,	{0}},
	{eSelfCheckState4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		205-12+70,	0,	{0}},
	{eSelfCheckState5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		240-15+70,	0,	{0}},
	{eSelfCheckState6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		275-18+70,	0,	{0}},
	{eSelfCheckState7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		310-21+70,	0,	{0}},
	{eSelfCheckState8,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		345-24+70,	0,	{0}},
	{eSelfCheckState9,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-99+LOFFSET,		380-27+70,	0,	{0}},

#else
	{eSelfCheckState1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		100-3,	0,	{0}},
	{eSelfCheckState2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		135-6,	0,	{0}},
	{eSelfCheckState3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		170-9,	0,	{0}},
	{eSelfCheckState4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		205-12,	0,	{0}},
	{eSelfCheckState5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		240-15,	0,	{0}},
	{eSelfCheckState6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		275-18,	0,	{0}},
	{eSelfCheckState7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		310-21,	0,	{0}},
	{eSelfCheckState8,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		345-24,	0,	{0}},
	{eSelfCheckState9,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		380-27,	0,	{0}},
#endif
	{eCalibZero_D,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	280+LOFFSET,		490,	0,	{0}},
	{eCalibZero_Fx,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	400+LOFFSET,		490,	0,	{0}},
	{eCalibZero_Fy,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	540+LOFFSET,		490,	0,	{0}},
	{eCalibZero_AngleGun,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		150,	0,	{0}},
	{eCalibZero_AngleGrenade,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		185,	0,	{0}},
	{eCalibZero_Angle,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		490,	0,	{0}},

	{eCalibWeather_Tep,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	450+LOFFSET,		100,	0,	{0}},
	{eCalibWeather_Pre,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	450+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_TepPole,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	516+LOFFSET,		100,	0,	{0}},
	{eCalibWeather_TepVal1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	527+LOFFSET,		100,	0,	{0}},
	{eCalibWeather_TepVal2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	538+LOFFSET,		100,	0,	{0}},
	{eCalibWeather_PreVal1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	516+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	527+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	538+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	549+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	560+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	571+LOFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	582+LOFFSET,		135,	0,	{0}},

	{eCalibGeneral_Dis,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	400+LOFFSET,		150,	0,	{0}},
	{eCalibGeneral_X,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	400+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_Y,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	400+LOFFSET,		220,	0,	{0}},
	{eCalibGeneral_DisValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	466+LOFFSET,		150,	0,	{0}},
	{eCalibGeneral_DisValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	477+LOFFSET,		150,	0,	{0}},
	{eCalibGeneral_DisValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	488+LOFFSET,		150,	0,	{0}},
	{eCalibGeneral_DisValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	499+LOFFSET,		150,	0,	{0}},
	{eCalibGeneral_XPole,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	466+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	477+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	488+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	499+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	510+LOFFSET,		185,	0,	{0}},
	{eCalibGeneral_YPole,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	466+LOFFSET,		220,	0,	{0}},
	{eCalibGeneral_YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	477+LOFFSET,		220,	0,	{0}},
	{eCalibGeneral_YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	488+LOFFSET,		220,	0,	{0}},
	{eCalibGeneral_YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	499+LOFFSET,		220,	0,	{0}},
	{eCalibGeneral_YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	510+LOFFSET,		220,	0,	{0}},

	{eCalibGeneral_Rx,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	320+LOFFSET,		490,	0,	{0}},
	{eCalibGeneral_Ry,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		490,	0,	{0}},
	{eCalibHorizen_Menu,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		100,	0,	{0}},
	{eCalibHorizen_Pos,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		135-3,	0,	{0}},
	{eCalibHorizen_Mach,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		170-6,	0,	{0}},
	{eCalibHorizen_Grenade,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		205-9,	0,	{0}},

	{eCalibGenPram_Menu,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		100,	0,	{0}},
	{eCalibGenpram_VFLD0TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+198+LOFFSET,	135-3,	0,	{0}},
	{eCalibGenPram_VFLDOX,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOY,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenpram_VFLD1TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+198+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_VFLD1X,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1Y,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenpram_VFLD2TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+198+LOFFSET,	275-15,	0,	{0}},
	{eCalibGenPram_VFLD2X,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2Y,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLDOXValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOYValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLD1XValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1YValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD2XValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2YValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	159+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	192+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	203+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	214+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	225+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	236+LOFFSET,		310-18,	0,	{0}},

	{eCalibGenPram_MachGunTime,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_GrenadeTime,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_TimeValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	115+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_TimeValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	126+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_TimeValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	137+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_TimeValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_TimeValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	115+LOFFSET,	240-12,	0,	{0}},
	{eCalibGenPram_TimeValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	126+LOFFSET,	240-12,	0,	{0}},
	{eCalibGenPram_TimeValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	137+LOFFSET,	240-12,	0,	{0}},
	{eCalibGenPram_TimeValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	148+LOFFSET,	240-12,	0,	{0}},

	{eCalibGenPram_ServoXMenu,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		200,	0,	{0}},
	{eCalibGenPram_ServoXItem0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXItem1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoXItem2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoXValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	215+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	259+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	325+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	391+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	523+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoXValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	259+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoXValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	325+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoXValue8,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	391+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoXValue9,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoXValue10,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	259+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoXValue11,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	325+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoXValue12,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	391+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoXValue13,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoXValue14,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	523+LOFFSET,		305-9,	0,	{0}},
	{eCalibGenPram_ServoYMenu,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		200,	0,	{0}},
	{eCalibGenPram_ServoYItem0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYItem1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoYValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	215+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	259+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	325+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	391+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	523+LOFFSET,		235-3,	0,	{0}},
	{eCalibGenPram_ServoYValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	259+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoYValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	325+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoYValue8,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	391+LOFFSET,		270-6,	0,	{0}},
	{eCalibGenPram_ServoYValue9,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,		270-6,	0,	{0}},

	{ePlatFormX,					eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	100+LOFFSET,		400,	0,	{0}},
	
#ifdef MIDMENU
	{eCalcNum_Visual,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		80,	0,	{0}},
	{eCalcNum_Traject,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		115-3,	0,	{0}},
	{eCalcNum_Trunion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		150-6,	0,	{0}},
	{eCalcNum_General,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		185-9,	0,	{0}},
	{eCalcNum_Weather,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		220-12,	0,	{0}},
	{eCalcNum_Turret,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		255-15,	0,	{0}},
	{eCalcNum_AlgleD,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		290-18,	0,	{0}},
	{eCalcNum_Posion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		325-21,	0,	{0}},

	{eStateMaintD,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		80+100,	0,	{0}},
	{eStateDetend,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			80+100,	0,	{0}},
	{eStateClutch,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		115-3+100,	0,	{0}},
	{eStateGrenad,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			115-3+100,	0,	{0}},
	{eStateFirer,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		150-6+100,	0,	{0}},
	{eStateFulCAN,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			150-6+100,	0,	{0}},
	{eStateDisCAN0,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		185-9+100,	0,	{0}},
	{eStateDisCAN1,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			185-9+100,	0,	{0}},
	{eStateMagnet,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		220-12+100,	0,	{0}},
	{eStatePosMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			220-12+100,	0,	{0}},
	{eStateMacMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-165+LOFFSET,		255-15+100,	0,	{0}},
	{eStateGreMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,			255-15+100,	0,	{0}},
	
	{eStateWeaponCtrlA1,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		360-24,	0,	{0}},
	{eStateSrvAngle,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		395-27,	0,	{0}},
	{eStateFulScrAngle,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		430-30,	0,	{0}},
	{eLaserSelect,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350-209+LOFFSET,		465-33,	0,	{0}},
#else
	{eCalcNum_Visual,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		80,	0,	{0}},
	{eCalcNum_Traject,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		115-3,	0,	{0}},
	{eCalcNum_Trunion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		150-6,	0,	{0}},
	{eCalcNum_General,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		185-9,	0,	{0}},
	{eCalcNum_Weather,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		220-12,	0,	{0}},
	{eCalcNum_Turret,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		255-15,	0,	{0}},
	{eCalcNum_AlgleD,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		290-18,	0,	{0}},
	{eCalcNum_Posion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		325-21,	0,	{0}},

	{eStateMaintD,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		80,	0,	{0}},
	{eStateDetend,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		80,	0,	{0}},
	{eStateClutch,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		115-3,	0,	{0}},
	{eStateGrenad,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		115-3,	0,	{0}},
	{eStateFirer,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		150-6,	0,	{0}},
	{eStateFulCAN,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		150-6,	0,	{0}},
	{eStateDisCAN0,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		185-9,	0,	{0}},
	{eStateDisCAN1,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		185-9,	0,	{0}},
	{eStateMagnet,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		220-12,	0,	{0}},
	{eStatePosMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		220-12,	0,	{0}},
	{eStateMacMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		255-15,	0,	{0}},
	{eStateGreMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		255-15,	0,	{0}},
	
	{eStateWeaponCtrlA1,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		360-24,	0,	{0}},
	{eStateSrvAngle,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		395-27,	0,	{0}},
	{eStateFulScrAngle,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		430-30,	0,	{0}},
	{eLaserSelect,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		465-33,	0,	{0}},
#endif
	{eAnglePosn,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	260+LOFFSET,		465,	0,	{0}},
	{eAngleDipX,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		465,	0,	{0}},
	{eAngleDipY,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		500,	0,	{0}},
	{eAngleMach,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	160+LOFFSET,		465,	0,	{0}},
	{eAngleGred,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	160+LOFFSET,		500,	0,	{0}},

	{eGunTip,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		65,	0,	{0}},
};

volatile FONT_OBJ g_Font={NULL,WHITECOLOR,1,0,0,0,NULL};

unsigned char g_Trk_infor[128]={0};
static int tick=0;
void resetTickNum()
{
	tick = 40;
}


BOOL isMultiChanged()
{
	if(tick > 0)
		tick--;
	return (tick != 0);
}
static void OSDCTRL_OsdInitial()
{
	sprintf(WorkOsd[0],"%c%c",249,185);//zi dong
	sprintf(WorkOsd[1],"%c%c",198,199);//
	sprintf(WorkOsd[2],"%c%c",197,250);//У׼
	sprintf(WorkOsd[3],"%c%c",197,209);//У��
	sprintf(WorkOsd[4],"%c%c",253,243);//����
	sprintf(WorkOsd[5],"%c%c",219,242);//����
	sprintf(WorkOsd[6],"%c%c",161,162);//����
	sprintf(WorkOsd[7],"%c%c",253,180);//�۲�
	sprintf(WorkOsd[8],"%c%c",186,153);//��ƽ
	sprintf(WorkOsd[9],"%c%c",201,144);//����

	sprintf(AimOsd[0],"%c",186);//��
	sprintf(AimOsd[1],"%c",206);//��

	//sprintf(MeasureTypeOsd[0],"");//��
	sprintf(MeasureTypeOsd[0],"%c:    ",222);
	sprintf(MeasureTypeOsd[1],"%c:    ",201);
	sprintf(MeasureTypeOsd[2],"LSBG");//LSBG
	sprintf(MeasureTypeOsd[3],"TMOV");//TMOV
	sprintf(MeasureTypeOsd[4],"ECNL");//ECNL
	sprintf(MeasureTypeOsd[5],"SMNL");//SMNL

	sprintf(GunOsd[0],"%c%c",200,216);
	sprintf(GunOsd[1],"%c%c",223,224);
	sprintf(GunOsd[2],"%c%c",189,176);
	sprintf(GunOsd[3],"%c%c%c%c?",223,224,216,193);
	sprintf(GunOsd[4],"%c%c%c%c?",189,176,216,193);
//	sprintf(GunOsd[3],"%c%c?",223,224);//ɱ��?
//	sprintf(GunOsd[4],"%c%c?",189,176);//����?


	sprintf(ShotOsd[0],"%c",188);//�� ��
	sprintf(ShotOsd[1],"%c",210);//��

	sprintf(ShotGunOsd[0],"%c",171);//�� ��ǹ
	sprintf(ShotGunOsd[1],"%c",210);//��
	
	sprintf(FovTypeOsd[0],"%c",187);//��
	sprintf(FovTypeOsd[1],"%c",241);//С
	
	sprintf(SuperOsd[0],"%c%c%c",235,216,236);//��ǹ��
	sprintf(SuperOsd[1],"%c%c%c",235,218,228);//��ǿ��
	sprintf(SuperOsd[2],"%c%c%c",235,209,239);//����λ
	sprintf(SuperOsd[3],"%c%c%c",252,218,208);//��ǿ��
	sprintf(SuperOsd[4],"%c%c%c",252,218,194);//��ǿ��
	sprintf(SuperOsd[5],"%c%c%c",158,159,208);//����
	sprintf(SuperOsd[6],"%c%c%c",158,159,194);//�����
	
	
	sprintf(DynamicOsd[0],"%c%c%c%c",221,185,205,229);//��������
	sprintf(DynamicOsd[1],"%c%c%c%c",209,239,205,229);//��λ����
	sprintf(DynamicOsd[2],"%c%c%c%c",179,202,245,183);//����쳣
	sprintf(DynamicOsd[3],"%c%c%c%c",179,231,188,232);//���ٶ�ʱ
	sprintf(DynamicOsd[4],"%c%c%c%c",235,236,205,229);//��������
	sprintf(DynamicOsd[5],"%c%c%c%c",179,202,195,248);//����Զ
	sprintf(DynamicOsd[6],"%c%c%c%c",209,239,203,184);//��λ���
//	sprintf(DynamicOsd[7],"%c%c%c%c",163,164,166,167);//Ŀ��ʧ��
	sprintf(DynamicOsd[8],"%c%c%c%c",163,164,165,166);//Ŀ�궪ʧ

	sprintf(ResultOsd[0],"%c%c",254,183);//��
	sprintf(ResultOsd[1],"%c%c",245,183);//�쳣
	sprintf(OpenCloseOsd[0],"%c",208);//��
	sprintf(OpenCloseOsd[1],"%c",194);//��

	sprintf(CalibZeroOsd[0],"%c%c-58%c ",197,209,200);//У��-5.8��
	sprintf(CalibZeroOsd[1],"%c%c-35%c ",197,209,223);//У��-35ɱ
	sprintf(CalibZeroOsd[2],"%c%c-35%c ",197,209,211);//У��-35��

	sprintf(CalibGeneralOsd[0],"%c%c-%c%c ",253,243,200,216);//����-��ǹ
	sprintf(CalibGeneralOsd[1],"%c%c-35%c ",253,243,223);//����-35ɱ
	sprintf(CalibGeneralOsd[2],"%c%c-35%c ",253,243,211);//����-35��

	sprintf(ErrorOsd[17],"%c%c%c%c",240,196,213,208);//ά���ſ�
	sprintf(ErrorOsd[18],"%c%c%c%c",214,255,185,194);//��ֹ����

	sprintf(LaserOsd[0],"%c",145);
	sprintf(LaserOsd[1],"%c",146);
	
	Posd[eWorkMode] = WorkOsd[0];
	Posd[eAimType] = AimOsd[0];
	Posd[eGunType] = GunOsd[gProjectileType];
	Posd[eFovType] = FovTypeOsd[1];
	Posd[eSuperOrder] = SuperOsd[0];
	Posd[eDynamicZone] = DynamicOsd[0];
	Posd[eSelfCheckResult] = ResultOsd[1];
	Posd[eSelfCheckState1] = ResultOsd[1];
	Posd[eSelfCheckState2] = ResultOsd[1];
	Posd[eSelfCheckState3] = ResultOsd[1];
	Posd[eSelfCheckState4] = ResultOsd[1];
	Posd[eSelfCheckState5] = ResultOsd[1];
	Posd[eSelfCheckState6] = ResultOsd[1];
	Posd[eSelfCheckState7] = ResultOsd[1];
	Posd[eSelfCheckState8] = ResultOsd[1];
	Posd[eSelfCheckState9] = ResultOsd[1];
	Posd[eCalibMenu_Zero] = CalibZeroOsd[0];
	Posd[eCalibMenu_General] = CalibGeneralOsd[0];
	Posd[eLaserState] = LaserOsd[0];
	Posd[eShotType] = ShotOsd[0];
	Posd[eSaveYesNo] = SaveYesNoOsd[0];
	Posd[eCorrectionTip] = AngleCorrectOsd[0];
	Posd[eEnhance] = EnhanceOsd[0];
	Posd[eMeasureType] = MeasureTypeOsd[0];
	Posd[eErrorZone] = ErrorOsd[15];
	Posd[eGunTip] = GunOsd[3];
	Posd[eLaserSelect] = LaserOsd[0];
	Posd[eCalibMenu_Child] = GenPramMenu[0];
}

/*
 *  ======== OSDCTRL_create========
 *
 *  This function is used to create a osdctrl object.
 */
OSDCTRL_Handle OSDCTRL_create()//(MuxPotrTran * pMuxPortTran,HANDLE hAvtDev,HANDLE hTimerCtrl,UINT segId,UINT nBytes)
{
	OSDCTRL_OBJ * pCtrlObj = NULL;

	//SDK_ASSERT(pMuxPortTran!=NULL && hAvtDev !=NULL && hTimerCtrl!=NULL);

	pCtrlObj = (OSDCTRL_OBJ *)OSA_memAlloc(sizeof(OSDCTRL_OBJ));
	if(pCtrlObj!=NULL)
	{
		memset(pCtrlObj,0,sizeof(OSDCTRL_OBJ));
		pCtrlObj->Interface.pMux	= NULL;//pMuxPortTran;
		pCtrlObj->Interface.pCtrlSend = NULL;
		pCtrlObj->Interface.pCtrlRecv = NULL;
		//pCtrlObj->osdSem			= NULL;//&osdSem;
	//	pCtrlObj->pOsdTimer			= (CTimerCtrl*)hTimerCtrl;
		pCtrlObj->uSegId			= 0;//segId;
		pCtrlObj->uTextNum			= 0;
		pCtrlObj->pTextList			= g_Text;
		pCtrlObj->uInit				= 0;
		pCtrlObj->uRtlX				= 0;
		pCtrlObj->uRtlY				= 0;
		pCtrlObj->uRtlM				= 0;
	}
	
	OSDCTRL_OsdInitial();
	

	return pCtrlObj;
}
void OSDCTRL_AlgSelect()
{
	if(Posd[eAimType]==AimOsd[0])
		;
		//AVTCTRL_selectCORR(pAvtCtrlObj);
	else if(Posd[eAimType]==AimOsd[1])
		//AVTCTRL_selectCentroid(pAvtCtrlObj);
		;
}

void OSDCTRL_AllHide()
{
#if 0
	int i;
	for(i=eSuperOrder; i<=eBoreSightLinId; i++){
		OSDCTRL_ItemHide(i);
	}
	//AVTDEV_Normal(pAvtCtrlObj->pAvtDevObj);
	//initilZeroParam();
#endif
}
void OSDCTRL_NoShine()
{
	#if 0
	SHINE = FALSE;
	if(isMeasureManual()&&(eMeasureDis == ShinId))
		OSDCTRL_ItemShow(ShinId);
	else if(isCalibrationSave()&&(eSaveYesNo==ShinId))
		OSDCTRL_ItemShow(ShinId);
	else
		OSDCTRL_ItemHide(ShinId);
	ShinId = 0;
	#endif
}
void OSDCTRL_NoShineShow()
{
	SHINE = FALSE;
	//OSDCTRL_ItemShow(ShinId);
	ShinId = 0;
}
void OSDCTRL_ItemShine(int Id)
{
	SHINE = TRUE;
	ShinId = Id;
}
void OSDCTRL_BattleShow()
{
	#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eWeather1; i<=eAngleV; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wAuto];
	OSDCTRL_updateAreaN();
//	AVTCTRL_setAquire();
	OSDCTRL_NoShine();
	OSDCTRL_AlgSelect();
	#endif

}
void OSDCTRL_CalibMenuShow()
{
	#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibMenu_Weather; i<=eCursorX; i++){
		OSDCTRL_ItemShow(i);
	}
	for(i=eAnglePosn; i<=eAngleGred; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibration];
	OSDCTRL_NoShine();
	OSDCTRL_AlgSelect();
	OSDCTRL_updateMainMenu(getProjectileType());
	#endif
}
void OSDCTRL_CalibSaveShow()
{
#if 0
	OSDCTRL_CalibMenuShow();
	OSDCTRL_ItemShow(eSaveYesNo);
	OSDCTRL_ItemShine(eSaveYesNo);
	Posd[eWorkMode] = WorkOsd[wCalibration];
#endif
}

void OSDCTRL_CalibGenPramMenu0Show()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_Menu; i<=eCalibGenPram_VFLD2YValue7; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
#endif
}

void OSDCTRL_CalibGenPramMenu1Show()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_MachGunTime; i<=eCalibGenPram_TimeValue7; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
#endif
}

void OSDCTRL_CalibGenPramMenu2Show()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_ServoXMenu; i<=eCalibGenPram_ServoXValue14; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
#endif
}

void OSDCTRL_CalibGenPramMenu3Show()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_ServoYMenu; i<=eCalibGenPram_ServoYValue9; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
#endif
}

void OSDCTRL_CalibHorizenShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibHorizen_Menu; i<=eCalibHorizen_Grenade; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibHorizen];
#endif
}

void OSDCTRL_CalibLaserShow()
{
#if 0
	OSDCTRL_AllHide();
	Posd[eWorkMode] = WorkOsd[wCalibLaser];
#endif
}
void OSDCTRL_CalibWeatherShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibWeather_Tep; i<=eCalibWeather_PreVal4; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibWeather];
#endif
}
void OSDCTRL_CalibGeneralShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGeneral_Dis; i<=eCalibGeneral_Ry; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGeneral];
#endif
}

void OSDCTRL_CalibZeroShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibZero_D; i<=eCalibZero_Fy; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibZero];
	if(isGrenadeKill()/**Posd[eGunType] == GunOsd[2]**/)
	{
		for(i=eCalibZero_AngleGun; i<=eCalibZero_Angle; i++){
			OSDCTRL_ItemShow(i);
		}
	}else
	{
		for(i=eCalibZero_AngleGun; i<=eCalibZero_Angle; i++){
			OSDCTRL_ItemHide(i);
		}
	}
	OSDCTRL_NoShine();
#endif
}

void OSDCTRL_BattleAlertShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eWeather1; i<=eAngleV; i++){
		OSDCTRL_ItemShow(i);
	}
	OSDCTRL_ItemHide(eDynamicZone);
	Posd[eWorkMode] = WorkOsd[wAlert];
	OSDCTRL_NoShine();
#endif
}

void OSDCTRL_BattleTrackShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eWeather1; i<=eAngleV; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wTrack];
	OSDCTRL_updateAreaN();
	AVTCTRL_setAquire();
	OSDCTRL_NoShine();
	OSDCTRL_AlgSelect();
#endif
}

void OSDCTRL_EnterBattleMode()
{
#if 0
	if(isStatBattleAuto())
		OSDCTRL_BattleShow();
	else if(isStatBattleAlert())
	{
	   OSDCTRL_BattleAlertShow();
	}
	else 
	{
		assert(FALSE);
	}
#endif
}
void OSDCTRL_CalibGenPramShow()
{
#if 0
	int i = getAdvancedMenuIndex();
	switch(i){
		case 0:
			OSDCTRL_CalibGenPramMenu0Show();
			break;
		case 1:
			OSDCTRL_CalibGenPramMenu1Show();
			break;
		case 2:
			OSDCTRL_CalibGenPramMenu2Show();
			break;
		case 3:
			OSDCTRL_CalibGenPramMenu3Show();
			break;
		default:
			break;
	}
#endif
}

void OSDCTRL_EnterCalibMode()
{
#if 0
	if(isCalibrationMainMenu())
		OSDCTRL_CalibMenuShow();
	else if(isCalibrationGeneral())
		OSDCTRL_CalibGeneralShow();
	else if(isCalibrationWeather())
		OSDCTRL_CalibWeatherShow();
	else if(isCalibrationZero())
		OSDCTRL_CalibZeroShow();
	else if(isCalibrationSave())
	{	
		OSDCTRL_CalibSaveShow();
	}else if(isCalibrationGenPram())
	{	
		OSDCTRL_CalibGenPramShow();
	}else if(isCalibrationHorizen())
	{	
		OSDCTRL_CalibHorizenShow();
	}else if(isCalibrationLaser())
	{	
		OSDCTRL_CalibLaserShow();
	}else 
	{
		assert(FALSE);
	}
#endif 
}

void OSDCTRL_updateAreaN()
{
#if 0
	int i=0;
	BOOL noErr = FALSE;
	if(!Is9stateOK())
	{
		i = getErrCodeId();
		Posd[eErrorZone] = ErrorOsd[i];
	}else if(bDetendClose())
		Posd[eErrorZone] = ErrorOsd[18];
	else if(bMaintPortOpen())
		Posd[eErrorZone] = ErrorOsd[17];
	else
	{
		noErr = TRUE;
	}
	
	if(noErr){
		OSDCTRL_ItemHide(eErrorZone);	
	}else{
		OSDCTRL_ItemShow(eErrorZone);
		startDynamicTimer();
	}
#endif
}
void OSDCTRL_updateMainMenu(int i)
{
#if 0
	Posd[eCalibMenu_Zero] = CalibZeroOsd[i];
	Posd[eCalibMenu_General] = CalibGeneralOsd[i];

/*
	if(isMachineGun()){
		Posd[eCalibMenu_Zero] = CalibZeroOsd[0];
		Posd[eCalibMenu_General] = CalibGeneralOsd[0];
	}else if(isGrenadeKill()){
		Posd[eCalibMenu_Zero] = CalibZeroOsd[0];
		Posd[eCalibMenu_General] = CalibGeneralOsd[0];
	}else if(isGrenadeGas()){

	}*/
#endif
}

void OSDCTRL_updateDistanceValue()
{
#if 0
	if(isMeasureManual())
		OSDCTRL_ItemShow(eMeasureDis);
	else
		OSDCTRL_ItemHide(eMeasureDis);
#endif
}

void OSDCTRL_CheckResultsShow()
{
#if 0
	int i;
	OSDCTRL_AllHide();
	for(i=eSelfCheckState1; i<=eSelfCheckState9; i++){
		OSDCTRL_ItemShow(i);
	}
#endif
}

void OSDCTRL_ItemShow(LPARAM lParam)
{
#if 1
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return;

	pTextObj[lParam].osdState = eOsd_Disp;
#endif
}

void OSDCTRL_CalcNumShow(void)
{
#if 0
	int i=0;
	for(i=eCalcNum_Visual; i<=eCalcNum_Posion; i++){
		OSDCTRL_ItemShow(i);
	}
#endif	
}

void OSDCTRL_CalcNumHide(void)
{
#if 0
	int i=0;
	for(i=eCalcNum_Visual; i<=eCalcNum_Posion; i++){
		OSDCTRL_ItemHide(i);
	}
#endif	
}

void OSDCTRL_FulScrAngleShow()
{
#if 0
	int i=0;
	for(i=eStateWeaponCtrlA1; i<=eLaserSelect; i++){
		OSDCTRL_ItemShow(i);
	}
#endif	
}

void OSDCTRL_FulScrAngleHide()
{
#if 0
	int i=0;
	for(i=eStateWeaponCtrlA1; i<=eLaserSelect; i++){
		OSDCTRL_ItemHide(i);
	}
#endif	
}

void OSDCTRL_BaseMenuShow()
{
#if 0
	int i=0;
	for(i=eF1Button; i<=eF6Button; i++){
		OSDCTRL_ItemShow(i);
	}
#endif	
}

void OSDCTRL_BaseMenuHide()
{
#if 0
	int i=0;
	for(i=eF1Button; i<=eF6Button; i++){
		OSDCTRL_ItemHide(i);
	}
#endif
}

void OSDCTRL_ConnectMenuShow()
{
#if 0
	int i=0;
	for(i=eStateMaintD; i<=eStateGreMotor; i++){
		OSDCTRL_ItemShow(i);
	}
#endif
}

void OSDCTRL_ConnectMenuHide()
{
#if 0
	int i=0;
	for(i=eStateMaintD; i<=eStateGreMotor; i++){
		OSDCTRL_ItemHide(i);
	}
#endif
}

void OSDCTRL_ItemHide(LPARAM lParam)
{
#if 1
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return;

	pTextObj[lParam].osdState = eOsd_Hide;
#endif
}

BOOL OSDCTRL_IsOsdDisplay(LPARAM lParam)
{
#if 0
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		SDK_ASSERT(FALSE);

	return eOsd_Disp == pTextObj[lParam].osdState;
#endif
}
/*
 *  ======== OSDCTRL_destroy========
 *
 *  This function is used to destroy a osdctrl object.
 */
void OSDCTRL_destroy(HANDLE hOsdCtrl)
{
#if 0
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	if(pCtrlObj!=NULL){
		SDK_FREE(sizeof(OSDCTRL_OBJ),pCtrlObj,pCtrlObj->uSegId);
	}
#endif
}

/*
 *  ======== OSDCTRL_Init========
 *
 *  This function is used to init all osd item, both sprit or text.
 */
int OSDCTRL_initText(HANDLE hOsdCtrl,Mat frame)
{
#if 1
	int i=0;
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;

	SDK_ASSERT(pCtrlObj!=NULL);

	for (i=eModeId;i<eBoreSightLinId;i++)
	{
		pCtrlObj->pTextList[i].osdReg=1;

		OSDCTRL_genOsdContext(pCtrlObj,g_Text[i].osdId);

	//	osd_draw_text(frame, (void *)pCtrlObj->pTextList);
	}

	pCtrlObj->uInit=1;
#endif
	return 0;	
}
/*
 *  ======== OSDCTRL_SetOsdContrast========
 *
 *  This function is used to set osd contrast,1 black 0 white.
 */
int OSDCTRL_setOsdContrast(HANDLE hOsdCtrl,unsigned char fColor,unsigned char bColor)
{
	int i=0,iRet=0;
#if 0
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	SDK_ASSERT(pCtrlObj!=NULL);
	
	g_Font.fgColor.yuv   =(fColor==eWhite)?WHITECOLOR:BLACKCOLOR;
	//g_Font.bgColor.yuv =(bColor==eWhite)?WHITECOLOR:BLACKCOLOR;
	g_Font.bgColor.yuv   =OSDCOLOR_NODRAW;

	for(i=eModeId;i<eBoreSightLinId;i++)
	{
		iRet|=OSDCTRL_setTextColor(hOsdCtrl,g_Text[i].osdId,fColor,bColor);
	}
	return iRet;
#endif
}

/*
 *  ======== OSDCTRL_SetOsdDisplay========
 *
 *  This function is used to set osd display or hide osd.
 */
/*
int OSDCTRL_setOsdDisplay(HANDLE hOsdCtrl,UINT uShow)
{
	int iRet=0;
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	SDK_ASSERT(pCtrlObj!=NULL);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eFovId,0x01);

	//�ַ�����ʱFov�ƶ�λ��
	if((uShow&0x0F)==0){
		g_Text[eFovId].osdInitX=160;
		g_Text[eFovId].osdInitY=514;
	}
	else{
		g_Text[eFovId].osdInitX=40;
		g_Text[eFovId].osdInitY=290;	
	}
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eSensorId,		uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eModeId,		uShow&0x0F);	
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eLaserStatId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eLaserShineId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eLaserCodeId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eLaserRFId,		uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eElSpeedId,		uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eAzSpeedId,		uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eElId,			uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eAzId,			uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eBoreSightXId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eBoreSightYId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eTrackStatId,	uShow&0x0F);
	iRet|=OSDCTRL_setTextState(hOsdCtrl,eImgEnhId,		uShow&0x0F);
	
	if(dramMsg.EnPicp==0x01)
	{
		iRet|=OSDCTRL_setTextState(hOsdCtrl,ePicpId,0x01);
	}
	else
	{
		iRet|=OSDCTRL_setTextState(hOsdCtrl,ePicpId,0x00);
	}
	return iRet;
}
*/
/*
 *  ======== OSDCTRL_setTextColor========
 *
 *  This function is used to change text color.
 */
int OSDCTRL_setTextColor(HANDLE hOsdCtrl,UINT uItemId,unsigned char fColor,unsigned char bColor)
{
	int iRet = 0;
#if 0
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	OSDText_Obj * pTextObj = NULL;

	SDK_ASSERT(pCtrlObj!=NULL);

	pTextObj = &pCtrlObj->pTextList[uItemId];
	if(pTextObj->osdBColor != bColor||pTextObj->osdFColor != fColor)
	{
		pTextObj->osdBColor = bColor;
		pTextObj->osdFColor = fColor;
	}
#endif
	return iRet;
}
/*
 *  ======== OSDCTRL_setTextState========
 *
 *  This function is used to change text state, text.
 */
int OSDCTRL_setTextState(HANDLE hOsdCtrl,UINT uItemId, UINT uStat)
{
	int iRet = 0;
#if 0
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	OSDText_Obj * pTextObj = NULL;

	SDK_ASSERT(pCtrlObj!=NULL);
	
	pTextObj = &pCtrlObj->pTextList[uItemId];
	pTextObj->osdState = uStat;
#endif
	return iRet;
}
/*
 *  ======== OSDCTRL_shinOsd========
 *
 *  This function is used to set osd shinning.
 */
 /*
void OSDCTRL_shinOsd(LPARAM lParam)
{
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = NULL;

	SDK_ASSERT(pCtrlObj!=NULL);

	if(pCtrlObj->uInit==0)
		return;

	pTextObj = &pCtrlObj->pTextList[eLaserShineId];
	//pTextObj = &pCtrlObj->pTextList[lParam];

	if(pTextObj->osdState==eOsd_Disp)
	{
		if(pTextObj->osdUpdate==eOsd_Update)
		{
			pTextObj->osdUpdate = eOsd_UnUpdate;
		}
		else if(pTextObj->osdUpdate==eOsd_UnUpdate)
		{
			pTextObj->osdUpdate = eOsd_Update;
		}
	}
}
*/
void Osd_shinItem(int id)
{
	if(OSDCTRL_IsOsdDisplay(id))
	{
		OSDCTRL_ItemHide(id);
	}else
	{
		OSDCTRL_ItemShow(id);
	}
}

/*
 *  ======== OSDCTRL_hideOsd========
 *
 *  This function is 
 */
void OSDCTRL_hideOsd(LPARAM lParam)
{
	//pOsdCtrlObj->pTextList[lParam].osdState=eOsd_Hide;
}
/*
 *  ======== OSDCTRL_lpfnTimer========
 *
 *  This function is a callback fun when Timer is over.
 */
void OSDCTRL_lpfnTimer(void* cbParam)
{
//	CTimerCtrl * pCtrlTimer = pOsdCtrlObj->pOsdTimer;
//	g_FilrBack=dramMsg.EnFilr;

//	pCtrlTimer->KillTimer(pCtrlTimer,eOSD_Timer);
//	SendMessage(MSGID_HIDE_OSD,eInforCodeId);
}

/*
 *  ======== OSDCTRL_draw========
 *
 *  This function is used to draw all the items
 */
void OSDCTRL_draw(Mat frame,OSDCTRL_Handle pCtrlObj)
{
	int i=0;
	static int gun=0;//,type=0;
	//volatile FONT_OBJ *pFont = &g_Font;
	OSDText_Obj * pTextObj = NULL;
	int startx,starty;
	char *ptr;
	UInt32 frcolor,bgcolor;

	//SDK_ASSERT(pCtrlObj!=NULL&&hIMG!=NULL);
	
	//if(1 != pCtrlObj->uInit)
	//	return;

	//pFont->fgColor   = g_Font.fgColor;
	//pFont->bgColor  = g_Font.bgColor;
	//if(SHINE)
	if(0)
	{
		n++;
		if(n%10 == 0)
		{
			Osd_shinItem(ShinId);//��˸
			n = 0;
		}
	}
	#if 0
	if((isGrenadeGas()&&isCalibrationMode()&&isCalibrationMainMenu()&&(1==getXPosition()))
		||(isGrenadeGas()&&isCalibrationZero()&&isCalibrationMode())
		||isGunTypeRequesting())
	#endif
		if(0)
		{
			gun++;
			if(gun%10 == 0){
				Osd_shinItem(eGunType);//��˸
				gun = 0;
			}
		}
		else
		{
			OSDCTRL_ItemShow(eGunType);
		}
/*	
	if(isGunTypeRequesting()){
		type++;
		if(type%15 == 0){
			Osd_shinItem(eGunType);//��˸
			type = 0;
		}
	}else{
		OSDCTRL_ItemShow(eGunType);
	}
	*/
/*
	if(isGunIdentify()
		&&(
			(isGrenadeKill()&&isIdentifyGas())
			||(isGrenadeGas()&&(!isIdentifyGas()))
			)
		){
		identify++;
		if(identify%10 == 0){
			Osd_shinItem(eGunTip);//��˸
			identify = 0;
		}
	}else{
		OSDCTRL_ItemHide(eGunTip);
	}
*/	
	for(i=eModeId;i<eBoreSightLinId;i++){
		pTextObj = &pCtrlObj->pTextList[i];
/*		if(i==eLaserShineId && pTextObj->osdUpdate==eOsd_UnUpdate){
			continue;
		}*/
		if (pTextObj->osdState==eOsd_Disp){
		
			OSDCTRL_genOsdContext(pCtrlObj,i);
			startx   = pTextObj->osdInitX;
			starty   = pTextObj->osdInitY;
			//frcolor  = WHITECOLOR;
			//bgcolor = BGCOLOR;
			ptr   = (char*)pTextObj->osdContext;
			//pFont->cSize   = pTextObj->osdTextLen;
		//	osd_chtext(frame, startx, starty, ptr, frcolor, bgcolor);
		//	osd_draw_text(Mat frame, void * prm);
		}
	}
	//CDC_drawCircle(hIMG,NULL);
	//CDC_drawSLine(hIMG,(HANDLE)pFont);
	//CDC_drawTest(hIMG,(HANDLE)pFont);

}

/*
 *  ======== OSDCTRL_genOsdContext========
 *
 *  This function is used to generation text.
 */
int OSDCTRL_genOsdContext(HANDLE hOsdCtrl,UINT uItemId)
{

   //	int index=0;
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	OSDText_Obj * pTextObj = NULL;
	char * pStr = NULL;
	static double value =0.0;
	static BYTE Bye;

	SDK_ASSERT(pCtrlObj!=NULL);

	pTextObj=&pCtrlObj->pTextList[uItemId];
/*	
	if(pTextObj->osdId != uItemId){
		pCtrlObj->pTextList = g_Text;
	}
*/	
	pStr = pTextObj->osdContext; 
	memset(pStr,0,MAX_CONTEXT_LEN);
		switch(pTextObj->osdId)
		{
		case eWorkMode:
			sprintf(pStr,"%s",Posd[eWorkMode]);//); 	//+32�Զ�
			//printf("**********************pStr = %d    %d\n",*pStr,*(pStr+1));
			break;
		case eAimType:
			sprintf(pStr,"%s",Posd[eAimType]);//); 		//��
			//printf("**********************pStr = %d    %d\n",*pStr,*(pStr+1));
			break;
		case eGunType:
				sprintf(pStr,"%s",Posd[eGunType]);//); 	//��ǹ
			//	printf("**********************pStr = %d    %d\n",*pStr,*(pStr+1));
			break;
		case eMeasureType:
			//if(isMeasureManual())
			if(1)
			{
				sprintf(pStr,"%s",Posd[eMeasureType]); //ren
			}
			else
			{
				int Distance = getLaserDistance();
//				Distance = (Distance < 0)? 0:(Distance);
				if(Distance>1700)
					printf("Distance = %d\n",Distance);
				Distance = (Distance>1700)?0:(Distance);
				//if(isMeasureOsdNormal())
				if (1)
					sprintf(pStr,"%s:%04d",Posd[eLaserState],Distance); //��:
				else
					sprintf(pStr,"%s:%s",Posd[eLaserState],Posd[eMeasureType]); //��/ĩ:
//					sprintf(pStr,"%s",Posd[eMeasureType]); //��:
				OSDCTRL_ItemHide(eMeasureDis);
			}
			break;
		case eMeasureDis:
			if(isMultiChanged())
				//sprintf(pStr,"x%03d",getDisLen()); //����
				sprintf(pStr,"x%03d",333);
			else
				sprintf(pStr,"%04d",DistanceManual);
			break;
		case eLaserState:
			sprintf(pStr,"%s",Posd[eLaserState]); 	//��ĩѡͨ
			break;
		case eVideoErr:
			//sprintf(pStr,"%c%c%c%c",230,215,245,183); 	//��Ƶ�쳣
			break;
		case eShotType:
			//if(isMachineGun())
			if(1)
				Posd[eShotType] = ShotOsd[0];//ShotOsd[getGunShotType()];
				//Posd[eShotType] = tmparray;
			else
				//Posd[eShotType] = ShotGunOsd[getShotType()];
				Posd[eShotType] = tmparray;
			
			sprintf(pStr,"%s",Posd[eShotType]);
			break;
		case eFovType:
			sprintf(pStr,"%s",Posd[eFovType]);//); 		//��
			break;
		case eEnhance:
			sprintf(pStr,"%s",Posd[eEnhance]); 
			break;
		case eSuperOrder:
			sprintf(pStr,"%s",Posd[eSuperOrder]);//); //��ǹ��
			break;
		case eWeather1:
			sprintf(pStr,"T%+02d   P%03dk",gWeatherTable.Temparature,gWeatherTable.Pressure/1000); 
			break;
		case eWeather2:
			//sprintf(pStr,"R:%+05.2f%+05.2f",getAimOffsetX(),getAimOffsetY()); 
			sprintf(pStr,"R:%+05.2f%+05.2f",1.1,2.2); 
/*			if(isTrackingMode())
			{
				sprintf(pStr,"R:%+05.2f%+05.2f",getAimOffsetX(),getAimOffsetY()); 
			}else
			{
				sprintf(pStr,"R:%+05.2f%+05.2f",0.0,0.0); 
			}*/
			break;
		case eErrorZone:
			sprintf(pStr,"%s",Posd[eErrorZone]); 
			break;
		case eDynamicZone:
			sprintf(pStr,"%s",Posd[eDynamicZone]);//); //��������
			break;
		case eAngleH:
/*			PositionX = getPlatformPositionX();
			PositionY = getPlatformPositionY();
			sprintf(pStr,"Q%+03.0f%+03.0f",(PositionX),(PositionY)); 
*/			
			sprintf(pStr,"%c%+03.0f",200,getMachGunAngle());//�� 

			break;
		case eAngleV:
/*			AngleGun = getMachGunAngle();
			AngleGrenade = getGrenadeAngle();
			sprintf(pStr,"G%+03.0f%+03.0f",(AngleGun),(AngleGrenade)); //��λΪ��
*/
			sprintf(pStr,"%c%+03.0f",211,getGrenadeAngle());//�� 
			break;
		case eCorrectionTip:
			sprintf(pStr,"%s",Posd[eCorrectionTip]);
			break;
		case eF1Button:
			sprintf(pStr,"%c %c",143,156);//200,210); 			//ɨ��//����
			break;
		case eF2Button:
			sprintf(pStr,"%c %c",237,178); 			//ͼ��
			break;
		case eF3Button:
			sprintf(pStr,"%c %c",252,218); 			//��ǿ
			break;
		case eF4Button:
			sprintf(pStr,"%c %c",179,202); 			//���
			break;
		case eF5Button:
			sprintf(pStr," %c ",206); 			//��
			break;
		case eF6Button:
			sprintf(pStr," %c ",206); 			//��
			break;
		case eCalibMenu_Weather:
			sprintf(pStr,"%c%c%c%c  ",219,242,180,225); 			//�������
			break;
		case eCalibMenu_Zero:
			sprintf(pStr,"%s",Posd[eCalibMenu_Zero]);//); 			//У��-5.8��
			break;
		case eCalibMenu_General:
			sprintf(pStr,"%s",Posd[eCalibMenu_General]);//); 			//���-5.8��
			break;
		case eCalibMenu_Save:
			sprintf(pStr,"%c%c%c%c  ",180,225,181,182); 			//����洢
			break;
		case eCalibMenu_GenPram:
			sprintf(pStr,"%c%c%c%c  ",253,180,197,250); 			//�۲�У׼
			break;
		case eCalibMenu_Horizen:
			sprintf(pStr,"%c%c%c%c  ",186,153,197,209); 			//��ƽУ��
			break;
		case eCalibMenu_Laser:
			sprintf(pStr,"%c%c%c%c  ",201,144,197,209); 			//����У��
			break;
		case eCalibMenu_Child:
			sprintf(pStr,"%s",Posd[eCalibMenu_Child]);//�Ӳ˵�
			break;
		case eCursorX:
			sprintf(pStr,"X");
			break;
		case eSaveYesNo:
			sprintf(pStr,"%s",Posd[eSaveYesNo]);
			break;
		case eCalibMenu_SaveOK:
			sprintf(pStr,"%c%c%s",181,182,ResultOsd[0]);					//�洢��
			break;
		case eSelfCheckResult:
			//if(Is9stateOK())
			if (1)
				Posd[eSelfCheckResult] = ResultOsd[0];
			else
				Posd[eSelfCheckResult] = ResultOsd[1];
			sprintf(pStr,"%c%c:%s",249,203,Posd[eSelfCheckResult]);//); 					//�Լ�:��-�쳣
			break;
		case eSelfCheckState1:
			//if(bWeaponCtrlOK())
			if (1)
				Posd[eSelfCheckState1] = ResultOsd[0];
			else
				Posd[eSelfCheckState1] = ResultOsd[1];
			sprintf(pStr,"%c %c:        %s",244,207,Posd[eSelfCheckState1]);//); 			//�Կ�:��-�쳣
			break;
		case eSelfCheckState2:
			//if(bJoyStickOK())
			if (1)
				Posd[eSelfCheckState2] = ResultOsd[0];
			else
				Posd[eSelfCheckState2] = ResultOsd[1];
			sprintf(pStr,"%c %c:        %s",227,177,Posd[eSelfCheckState2]);//); 			//�ֱ�:��-�쳣
			break;
		case eSelfCheckState3:
			//if(bDipAngleSensorOK())
			if (1)
				Posd[eSelfCheckState3] = ResultOsd[0];
			else
				Posd[eSelfCheckState3] = ResultOsd[1];
			sprintf(pStr,"%c %c:        %s",220,204,Posd[eSelfCheckState3]);//); 			//���:��-�쳣
			break;
		case eSelfCheckState4:
			//if(bPositionSensorOK())
			if (1)
				Posd[eSelfCheckState4] = ResultOsd[0];
			else
				Posd[eSelfCheckState4] = ResultOsd[1];
			sprintf(pStr,"%c%c%c:       %s",190,239,204,Posd[eSelfCheckState4]);//); 		//��λ��:��-�쳣
			break;
		case eSelfCheckState5:
			//if(bMachineGunSensorOK())
			if (1)
				Posd[eSelfCheckState5] = ResultOsd[0];
			else
				Posd[eSelfCheckState5] = ResultOsd[1];
			sprintf(pStr,"%c%c%c-%c%c:  %s",191,246,204,200,216,Posd[eSelfCheckState5]);//); //������-��ǹ:��-�쳣
			break;
		case eSelfCheckState6:
			//if(bGrenadeSensorOK())
			if (1)
				Posd[eSelfCheckState6] = ResultOsd[0];
			else
				Posd[eSelfCheckState6] = ResultOsd[1];
			sprintf(pStr,"%c%c%c-35%c:  %s",191,246,204,211,Posd[eSelfCheckState6]);//); 	//������-35��:��-�쳣
			break;
		case eSelfCheckState7:
			//if(bPositionServoOK())
			if (1)
				Posd[eSelfCheckState7] = ResultOsd[0];
			else
				Posd[eSelfCheckState7] = ResultOsd[1];
			sprintf(pStr,"%c%c%c%c:     %s",190,239,226,192,Posd[eSelfCheckState7]);//); 		//��λ�ŷ�:��-�쳣
			break;
		case eSelfCheckState8:
			//if(bMachineGunServoOK())
			if (1)
				Posd[eSelfCheckState8] = ResultOsd[0];
			else
				Posd[eSelfCheckState8] = ResultOsd[1];
			sprintf(pStr,"%c%c%c%c-%c%c:%s",191,246,226,192,200,216,Posd[eSelfCheckState8]);//); //�����ŷ�-��ǹ:��-�쳣
			break;
		case eSelfCheckState9:
			//if(bGrenadeServoOK())
			if (1)
				Posd[eSelfCheckState9] = ResultOsd[0];
			else
				Posd[eSelfCheckState9] = ResultOsd[1];
			sprintf(pStr,"%c%c%c%c-35%c:%s",191,246,226,192,211,Posd[eSelfCheckState9]);//); 	//�����ŷ�-35��:��-�쳣
			break;	
		case eCalibZero_D:				
			sprintf(pStr,"D:%04d",500);				//����:500
			break;
		case eCalibZero_Fx:
			//sprintf(pStr,"Fx:%+05d",getCrossX()); 	//Fx
			sprintf(pStr,"Fx:%+05d",1.1); 	
			break;
		case eCalibZero_Fy:
			//sprintf(pStr,"Fy:%+05d",getCrossY()); 	//Fy
			sprintf(pStr,"Fy:%+05d",1.1);
			break;
		case eCalibZero_AngleGun:
			value = getMachGunAngleAbs();
			sprintf(pStr,"%c%cY:%+06.2f",200,216,value); 	//��ǹY 
			break;
		case eCalibZero_AngleGrenade:
			value = getGrenadeAngleAbs();
			sprintf(pStr,"%c%cY:%+06.2f",211,214,value); 	//��Y
			break;
		case eCalibZero_Angle:
			AngleGun = getMachGunAngle();
			AngleGrenade = getGrenadeAngle();
			sprintf(pStr,"%c%+03.0f%c%+03.0f",200,AngleGun,211,AngleGrenade); 	//��Y
			break;
		case eCalibWeather_Tep:
			sprintf(pStr,"%c%c:        %c",219,238,174); 	//����:��
			break;
		case eCalibWeather_Pre:
			sprintf(pStr,"%c%c:       KPa",219,247); 	//��ѹ:bbbbbbb Pa 
			break;
		case eCalibWeather_TepPole:
			if(Weather[0] > 0)
				sprintf(pStr,"+"); 	//+
			else
				sprintf(pStr,"-"); 	//-
			break;
		case eCalibWeather_TepVal1:
			sprintf(pStr,"%01d",Weather[1]); 	
			break;
		case eCalibWeather_TepVal2:
			sprintf(pStr,"%01d",Weather[2]); 	
			break;
		case eCalibWeather_PreVal1:
			sprintf(pStr,"%01d",Weather[3]); 	
			break;
		case eCalibWeather_PreVal2:
			sprintf(pStr,"%01d",Weather[4]); 	
			break;
		case eCalibWeather_PreVal3:
			sprintf(pStr,"%01d",Weather[5]); 	
			break;
		case eCalibWeather_PreVal4:
			sprintf(pStr,"%01d",Weather[6]); 	
			break;
		case eCalibWeather_PreVal5:
			sprintf(pStr,"%01d",Weather[7]); 	
			break;
		case eCalibWeather_PreVal6:
			sprintf(pStr,"%01d",Weather[8]); 	
			break;
		case eCalibWeather_PreVal7:
			sprintf(pStr,"%01d",Weather[9]); 	
			break;
		case eCalibGeneral_Dis:
			sprintf(pStr,"%c%c:        m",202,212);//,General[0]); 		//����:xxxx m
			break;
		case eCalibGeneral_DisValue1:
			sprintf(pStr,"%01d",General[0]); 		//xxxx m
			break;
		case eCalibGeneral_DisValue2:
			sprintf(pStr,"%01d",General[1]); 		//xxxx m
			break;
		case eCalibGeneral_DisValue3:
			sprintf(pStr,"%01d",General[2]); 		//xxxx m
			break;
		case eCalibGeneral_DisValue4:
			sprintf(pStr,"%01d",General[3]); 		//xxxx m
			break;
		case eCalibGeneral_X:
			sprintf(pStr,"%c%cX:      cm",253,243); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_XPole:
			if(General[4]>= 0)
				sprintf(pStr,"+"); 			//+
			else 
				sprintf(pStr,"-"); 			//-
			break;
		case eCalibGeneral_XValue1:
			sprintf(pStr,"%01d",General[5]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_XValue2:
			sprintf(pStr,"%01d",General[6]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_XValue3:
			sprintf(pStr,"%01d",General[7]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_XValue4:
			sprintf(pStr,"%01d",General[8]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_Y:
			sprintf(pStr,"%c%cY:      cm",253,243); 		//����Y:+xxxx cm
			break;
		case eCalibGeneral_YPole:
			if(General[9] >= 0)
				sprintf(pStr,"+"); 			//+
			else 
				sprintf(pStr,"-"); 			//-
			break;
		case eCalibGeneral_YValue1:
			sprintf(pStr,"%01d",General[10]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_YValue2:
			sprintf(pStr,"%01d",General[11]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_YValue3:
			sprintf(pStr,"%01d",General[12]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_YValue4:
			sprintf(pStr,"%01d",General[13]&0xFF); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_Rx:
			value = General[5]*10 + General[6] + General[7]*0.1 + General[8]*0.01;
			if(General[4] < 0)
				value = -value;
			sprintf(pStr,"Rx:%+06.2fm",value); 		//����X:+xxxx cm
			break;
		case eCalibGeneral_Ry:
			value = General[10]*10 + General[11] + General[12]*0.1 + General[13]*0.01;
			if(General[9] < 0)
				value = -value;
			sprintf(pStr,"Ry:%+06.2fm",value); 		//����X:+xxxx cm
			break;
		case eCalibHorizen_Menu:
			sprintf(pStr,"%c%c<->",203,184); 		//���
			break;
		case eCalibHorizen_Pos:
			sprintf(pStr,"%c%c_X: %+08.3f",216,236,getTurretTheta()); 		//����
			break;
		case eCalibHorizen_Mach:
			sprintf(pStr,"%c%c_Y: %+08.3f",200,216,getMachGunAngleAbs()); 		//��ǹ
			break;
		case eCalibHorizen_Grenade:
			sprintf(pStr,"%c%c_Y: %+08.3f",211,175,getGrenadeAngleAbs()); 		//��
			break;
		case eCalibGenPram_Menu:
			sprintf(pStr,"VF-Degree"); 		//���
			break;
		case eCalibGenpram_VFLD0TAG:
			sprintf(pStr,"CCD-Big"); 		//CCD-BIG
			break;
		case eCalibGenPram_VFLDOX:
			sprintf(pStr,"VFLD0-X=   .     ",12.0); 		//VFLD0-X
			break;
		case eCalibGenPram_VFLDOY:
			sprintf(pStr,"VFLD0-Y=   .     ",9.0); 		//VFLD0-Y
			break;
		case eCalibGenpram_VFLD1TAG:
			sprintf(pStr,"CCD-Small"); 		//CCD-Small
			break;
		case eCalibGenPram_VFLD1X:
			sprintf(pStr,"VFLD1-X=   .     ",4.3); 		//VFLD1-X
			break;
		case eCalibGenPram_VFLD1Y:
			sprintf(pStr,"VFLD1-Y=   .     ",3.3); 		//VFLD1-Y
			break;
		case eCalibGenpram_VFLD2TAG:
			sprintf(pStr,"IR-Big"); 		//IR-Big
			break;
		case eCalibGenPram_VFLD2X:
			sprintf(pStr,"VFLD2-X=   .     ",116.0); 		//VFLD2-X
			break;
		case eCalibGenPram_VFLD2Y:
			sprintf(pStr,"VFLD2-Y=   .     ",112.0); 		//VFLD2-Y
			break;
		case eCalibGenPram_VFLDOXValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[0]); 					
			break;
		case eCalibGenPram_VFLDOXValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[1]); 					
			break;
		case eCalibGenPram_VFLDOXValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[2]); 					
			break;
		case eCalibGenPram_VFLDOXValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[3]); 					
			break;
		case eCalibGenPram_VFLDOXValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[4]); 					
			break;
		case eCalibGenPram_VFLDOXValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[5]); 					
			break;
		case eCalibGenPram_VFLDOXValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[6]); 					
			break;
		case eCalibGenPram_VFLDOXValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[7]); 					
			break;
		case eCalibGenPram_VFLDOYValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[8]); 					
			break;
		case eCalibGenPram_VFLDOYValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[9]); 					
			break;
		case eCalibGenPram_VFLDOYValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[10]); 					
			break;
		case eCalibGenPram_VFLDOYValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[11]); 					
			break;
		case eCalibGenPram_VFLDOYValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[12]); 					
			break;
		case eCalibGenPram_VFLDOYValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[13]); 					
			break;
		case eCalibGenPram_VFLDOYValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[14]); 					
			break;
		case eCalibGenPram_VFLDOYValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[15]); 					
			break;
		case eCalibGenPram_VFLD1XValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[16]); 					
			break;
		case eCalibGenPram_VFLD1XValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[17]); 					
			break;
		case eCalibGenPram_VFLD1XValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[18]); 					
			break;
		case eCalibGenPram_VFLD1XValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[19]); 					
			break;
		case eCalibGenPram_VFLD1XValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[20]); 					
			break;
		case eCalibGenPram_VFLD1XValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[21]); 					
			break;
		case eCalibGenPram_VFLD1XValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[22]); 					
			break;
		case eCalibGenPram_VFLD1XValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[23]); 					
			break;
		case eCalibGenPram_VFLD1YValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[24]); 					
			break;
		case eCalibGenPram_VFLD1YValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[25]); 					
			break;
		case eCalibGenPram_VFLD1YValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[26]); 					
			break;
		case eCalibGenPram_VFLD1YValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[27]); 					
			break;
		case eCalibGenPram_VFLD1YValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[28]); 					
			break;
		case eCalibGenPram_VFLD1YValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[29]); 					
			break;
		case eCalibGenPram_VFLD1YValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[30]); 					
			break;
		case eCalibGenPram_VFLD1YValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[31]); 					
			break;
		case eCalibGenPram_VFLD2XValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[32]); 					
			break;
		case eCalibGenPram_VFLD2XValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[33]); 					
			break;
		case eCalibGenPram_VFLD2XValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[34]); 					
			break;
		case eCalibGenPram_VFLD2XValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[35]); 					
			break;
		case eCalibGenPram_VFLD2XValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[36]); 					
			break;
		case eCalibGenPram_VFLD2XValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[37]); 					
			break;
		case eCalibGenPram_VFLD2XValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[38]); 					
			break;
		case eCalibGenPram_VFLD2XValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[39]); 					
			break;
		case eCalibGenPram_VFLD2YValue0:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[40]); 					
			break;
		case eCalibGenPram_VFLD2YValue1:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[41]); 					
			break;
		case eCalibGenPram_VFLD2YValue2:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[42]); 					
			break;
		case eCalibGenPram_VFLD2YValue3:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[43]); 					
			break;
		case eCalibGenPram_VFLD2YValue4:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[44]); 					
			break;
		case eCalibGenPram_VFLD2YValue5:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[45]); 					
			break;
		case eCalibGenPram_VFLD2YValue6:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[46]); 					
			break;
		case eCalibGenPram_VFLD2YValue7:
			sprintf(pStr,"%01d",CalibGenPram_VFLD[47]); 					
			break;
		case eCalibGenPram_MachGunTime:
			sprintf(pStr,"58%c:    ms",200); //58��:0000ms					
			break;
		case eCalibGenPram_GrenadeTime:
			sprintf(pStr,"35%c:    ms",223); //35ɱ:0000ms					
			break;
		case eCalibGenPram_TimeValue0:
			sprintf(pStr,"%01d",TimeValue[0]); 					
			break;
		case eCalibGenPram_TimeValue1:
			sprintf(pStr,"%01d",TimeValue[1]); 					
			break;
		case eCalibGenPram_TimeValue2:
			sprintf(pStr,"%01d",TimeValue[2]); 					
			break;
		case eCalibGenPram_TimeValue3:
			sprintf(pStr,"%01d",TimeValue[3]); 					
			break;
		case eCalibGenPram_TimeValue4:
			sprintf(pStr,"%01d",TimeValue[4]); 					
			break;
		case eCalibGenPram_TimeValue5:
			sprintf(pStr,"%01d",TimeValue[5]); 					
			break;
		case eCalibGenPram_TimeValue6:
			sprintf(pStr,"%01d",TimeValue[6]); 					
			break;
		case eCalibGenPram_TimeValue7:
			sprintf(pStr,"%01d",TimeValue[7]); 					
			break;

		case eCalibGenPram_ServoXMenu:
			sprintf(pStr,"N%02d Add%c%05.2f up=N dn=<> left=+- right=add",codeX,signX,addX); //�˵�					
			break;
		case eCalibGenPram_ServoXItem0:
			sprintf(pStr,"Stable_X 0-5:    "); //stable			
			break;
		case eCalibGenPram_ServoXItem1:
			sprintf(pStr,"Hunter_X 6-9: ==="); //hunter
			break;
		case eCalibGenPram_ServoXItem2:
			sprintf(pStr,"Rshoot 10-14: ==="); //hunter
			break;
		case eCalibGenPram_ServoXValue0:
			sprintf(pStr,"%03.0f",ServoX[0]); 					
			break;
		case eCalibGenPram_ServoXValue1:
			sprintf(pStr,"%+05.2f",ServoX[1]); 					
			break;
		case eCalibGenPram_ServoXValue2:
			sprintf(pStr,"%+05.2f",ServoX[2]); 					
			break;
		case eCalibGenPram_ServoXValue3:
			sprintf(pStr,"%+05.2f",ServoX[3]); 					
			break;
		case eCalibGenPram_ServoXValue4:
			sprintf(pStr,"%+05.2f",ServoX[4]); 					
			break;
		case eCalibGenPram_ServoXValue5:
			sprintf(pStr,"%+05.2f",ServoX[5]); 					
			break;
		case eCalibGenPram_ServoXValue6:
			sprintf(pStr,"%+05.2f",ServoX[6]); 					
			break;
		case eCalibGenPram_ServoXValue7:
			sprintf(pStr,"%+05.2f",ServoX[7]); 					
			break;
		case eCalibGenPram_ServoXValue8:
			sprintf(pStr,"%+05.2f",ServoX[8]); 					
			break;
		case eCalibGenPram_ServoXValue9:
			sprintf(pStr,"%+05.2f",ServoX[9]); 					
			break;
		case eCalibGenPram_ServoXValue10:
			sprintf(pStr,"%+05.1f",ServoX[10]); 					
			break;
		case eCalibGenPram_ServoXValue11:
			sprintf(pStr,"%+05.1f",ServoX[11]); 					
			break;
		case eCalibGenPram_ServoXValue12:
			sprintf(pStr,"%+05.1f",ServoX[12]); 					
			break;
		case eCalibGenPram_ServoXValue13:
			sprintf(pStr,"%+05.1f",ServoX[13]); 					
			break;
		case eCalibGenPram_ServoXValue14:
			sprintf(pStr,"%+05.1f",ServoX[14]); 					
			break;
		case eCalibGenPram_ServoYMenu:
			sprintf(pStr,"N%02d Add%c%05.2f up=N dn=<> left=+- right=add",codeY,signY,addY); //�˵�					
			break;
		case eCalibGenPram_ServoYItem0:
			sprintf(pStr,"Stable_Y 0-5:    "); //stable			
			break;
		case eCalibGenPram_ServoYItem1:
			sprintf(pStr,"Hunter_Y 6-9: ==="); //hunter
			break;
		case eCalibGenPram_ServoYValue0:
			sprintf(pStr,"%03.0f",ServoY[0]); 					
			break;
		case eCalibGenPram_ServoYValue1:
			sprintf(pStr,"%+05.2f",ServoY[1]); 					
			break;
		case eCalibGenPram_ServoYValue2:
			sprintf(pStr,"%+05.2f",ServoY[2]); 					
			break;
		case eCalibGenPram_ServoYValue3:
			sprintf(pStr,"%+05.2f",ServoY[3]); 					
			break;
		case eCalibGenPram_ServoYValue4:
			sprintf(pStr,"%+05.2f",ServoY[4]); 					
			break;
		case eCalibGenPram_ServoYValue5:
			sprintf(pStr,"%+05.2f",ServoY[5]); 					
			break;
		case eCalibGenPram_ServoYValue6:
			sprintf(pStr,"%+05.2f",ServoY[6]); 					
			break;
		case eCalibGenPram_ServoYValue7:
			sprintf(pStr,"%+05.2f",ServoY[7]); 					
			break;
		case eCalibGenPram_ServoYValue8:
			sprintf(pStr,"%+05.2f",ServoY[8]); 					
			break;
		case eCalibGenPram_ServoYValue9:
			sprintf(pStr,"%+05.2f",ServoY[9]); 					
			break;

		case ePlatFormX:
			//sprintf(pStr,"Px:%+06.2f,Py:%+06.2f,Spx:%+03d,Spy:%+03d",getPlatFormX(),getPlatFormY(),getLastSpeed(),getLastSpeedY()); 		//PlatFormX:+xxxx 
			sprintf(pStr,"Px:%+06.2f,Py:%+06.2f,Spx:%+03d,Spy:%+03d",1.1,2.2,3.3,4.4); 
			break;
		case eCalcNum_Visual:
			//sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",172,173,getSumCalibX(),172,173,getSumCalibY()); //��Ԫ:���� ��
			sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",172,173,1.1,172,173,2.2);
			break;
		case eCalcNum_Traject:
			//sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",205,168,getTrajectCalcX(),205,168,getTrajectCalcY()); //������� 
			sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",205,168,1.1,205,168,2.2);
			break;
		case eCalcNum_Trunion:
			//sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",169,170,getTrunionCalibX(),169,170,getTrunionCalibY()); //���������� 
			sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",169,170,1.1,169,170,2.2);
			break;
		case eCalcNum_General:
			//sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",253,243,getCorrectionCalibX(),253,243,getCorrectionCalibY()); 		//���޲���
			sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  mil",253,243,1.1,253,243,2.2); 
			break;
		case eCalcNum_Weather:
			sprintf(pStr,"%c%c  = %+03d   %c %c%c  = %+08.3f  Kpa",219,238,gWeatherTable.Temparature,174,219,247,gWeatherTable.Pressure/1000.0); //���� = +15 ����ѹ = +101.325 Kpa:
			break;
		case eCalcNum_Turret:
			sprintf(pStr,"%c%cX = %+08.2f                      ",190,239,getTurretTheta()); //��λX
			break;
		case eCalcNum_AlgleD:
			sprintf(pStr,"%c%cY = %+08.2f %c%cY = %+08.2f  deg",200,216,getMachGunAngle(),211,214,getGrenadeAngle()); //��ǹY=.. ��Y=..
			break;
		case eCalcNum_Posion:
			sprintf(pStr,"%c%cX = %+08.2f %c%cY = %+08.2f  deg",220,204,getPlatformPositionX(),220,204,getPlatformPositionY()); //���X=.. ���Y=..
			break;

		case eStateMaintD:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :   %s",240,196,213,OpenCloseOsd[BIT1(Bye)]); //ά����
			break;
		case eStateDetend:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :   %s",214,255,185,OpenCloseOsd[1-BIT2(Bye)]); //����ֹ����
			break;
		case eStateClutch:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :   %s",212,150,154,OpenCloseOsd[BIT3(Bye)]); //�����
			break;
			
		case eStateGrenad:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"35%c%c%c: %s",211,149,154,ResultOsd[1-BIT1(Bye)]); //35������
			break;
		case eStateFirer:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%c    : %s",151,149,ResultOsd[1-BIT2(Bye)]); //����
			break;
		case eStateFulCAN:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN : %s",155,152,ResultOsd[1-BIT3(Bye)]); //ȫ��CAN
			break;
		case eStateDisCAN0:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN0: %s",244,207,ResultOsd[1-BIT5(Bye)]); //�Կ�CAN0
			break;
		case eStateDisCAN1:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN1: %s",244,207,ResultOsd[1-BIT6(Bye)]); //�Կ�CAN1
			break;
			
		case eStateMagnet:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"%c%c%c  : %s",148,147,160,ResultOsd[1-BIT0(Bye)]); //�����
			break;
		case eStatePosMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"%c%c%c%c: %s",157,153,148,200,ResultOsd[1-BIT1(Bye)]); //ˮƽ���
			break;
		case eStateMacMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"58%c%c  : %s",148,200,ResultOsd[1-BIT2(Bye)]); //58���
			break;
		case eStateGreMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"35%c%c%c: %s",211,148,200,ResultOsd[1-BIT3(Bye)]); //35����
			break;

		case eStateWeaponCtrlA1:
			sprintf(pStr,"WeaponCtrlA1= %s                ",getWeaponCtrlA1()); //WeaponCtrlA1
			break;
		case eStateSrvAngle:
			sprintf(pStr,"SrvNrmAngle =  %+07.2f %+07.2f %+07.2f",getTurretTheta(),getMachGunAngle(),getGrenadeAngle()); //�ŷ��Ƕ���Ϣ
			break;
		case eStateFulScrAngle:
			sprintf(pStr,"FulScrAngle =%s %+07.2f %+07.2f",getFulScrA0(),getpanoAngleV(),getpanoAngleH()); //ȫ���Ƕ���Ϣ=..
			break;
		case eLaserSelect:
			sprintf(pStr,"%c%c%c%c:      %s                     ",201,144,145,146,Posd[eLaserState]);//������ĩѡͨ
			break;
		case eAnglePosn:
			sprintf(pStr,"%c%c:%5.2f",190,239,getTurretTheta()); //��λ��
			break;
		case eAngleDipX:
			sprintf(pStr,"%cX:%5.2f",220,getPlatformPositionX()); //���X
			break;
		case eAngleDipY:
			sprintf(pStr,"%cY:%5.2f",220,getPlatformPositionY()); //���Y
			break;
		case eAngleMach:
			sprintf(pStr,"%cY:%5.2f",200,getMachGunAngle()); //��ǹY
			break;
		case eAngleGred:
			sprintf(pStr,"%cY:%5.2f",211,getGrenadeAngle()); //��Y
			break;
		case eGunTip:
			sprintf(pStr,"%s",Posd[eGunTip]); //����ȷ����ʾ
			break;
		default:
			break;
		}

	pTextObj->osdTextLen = strlen(pStr);

	return (pTextObj->osdTextLen);
}
/*
 *  ======== OSDCTRL_sendSem========
 *
 *  This function is used to
 */
void OSDCTRL_sendSem(OSDCTRL_OBJ * pCtrlObj)
{

}
/*
 *  ======== TskDrawOsd========
 *
 *  This function is used to
 */
void TskDrawOsd()
{

}
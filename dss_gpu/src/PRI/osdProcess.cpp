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
#include "statCtrl.h"
#include "osdPort.h"
#include "firingCtrl.h"

#define ARGSCALE	 0.0054931640625
#define ARGSPEEDSCAL 0.00274658203125

#define LOFFSET	 0
#define ROFFSET  15
#define W1TOFFSET 9
#define WOFFSET 140
#define CALCOFFSET 20
#define YOFFSET  135
#define WETHEROFFSET 75
#define GENOFFSET 0
//extern volatile int msgLight;
#define MIDMENU
extern int shine_table[10];
//OSDCTRL_Handle  pOsdCtrlObj		= NULL;
//CVideoDev* pVideoCapObj				=NULL;
OSDCTRL_OBJ * pCtrlObj = NULL;

char tmparray[12] = "abcdef";
bool AdCalibMenuFlag = false; 
extern bool isMaintPortOpen;

bool menu_jiancha_flag = 0;

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
char GunOsd[6][8];	//ǹ������
char ShotOsd[2][8];//���ģʽ:��
char ShotGunOsd[2][8];//���ģʽ:��ǹ
char LaserOsd[2][8];
char GuilingOsd[1][8];


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
char DynamicOsd[10][8];
char ResultOsd[2][8];
char OpenCloseOsd[2][8];
char CalibZeroOsd[3][8];
char MeasureTypeOsd[6][8];
char AngleCorrectOsd[3][8]={
	"GQ","RGQ","NGQ"
};
char SaveYesNoOsd[2][8]={
	"YES<","NO>"
};
char erase_blank[8] = {"  "};

char *Posd[OSD_TEXT_SIZE]={NULL};
char CalibGeneralOsd[3][8];
int General[14]={0,0,0,0,0,0,0,0,0,0,0};
int Weather[10]={1,0,4,0,1,0,1,3,2,5};
int Pressure=1013250,DistanceManual=0,Temparature=15;//xiang shu  | zuo biao | ya li | ju li
int DisValue[4]={0,0,0,0};
double AngleGun=0.0, AngleGrenade=0.0,PositionX=0.0,PositionY=0.0;//ǹ�ڵĶ���Ƕ���Ϣ �¶� �����ˮƽ�͸����Ƕ�ֵ
//int n=0,ShinId=eCalibGeneral_XPole;
unsigned char CalibGenPram_VFLD[48]={0};
int CCD_Big[2]={1200000,900000},CCD_Small[2]={430000,330000},IR_Big[2]={10000000,10000000};
unsigned char TimeValue[8]={0};
int MachTime=0,GrenadeTime=180;
int codeX=0,signX='+', codeY=0,signY='+';
float addX=0.01,addY=0.01;
float ServoX[15]={30.0, 3.59, 2.69, 2.0, 0.8, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float ServoY[10]={30.0,3.4, 2.6, 2.0, 0.7, 0.4, 0.0, 0.0, 0.0, 0.0};
bool SHINE=FALSE;
volatile char globalShoweErrorZone = 0 ;
double gGenparmX,gGenparmY;

extern PROJECTILE_TYPE gProjectileType;
extern PROJECTILE_TYPE gProjectileTypeBefore;
//extern WeatherItem gWeatherTable;
SEM_ID osdSem;

  
OSDText_Obj g_Text[OSD_TEXT_SIZE]=
{
	{eModeId,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		40,	0,	{0}},
	{eWorkMode,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		30,	0,	{0}},
	{eAimType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		30,	0,	{0}},
	{eGunType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	270+LOFFSET,		30,	0,	{0}},
	{eMeasureType,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	430+LOFFSET,		30,	0,	{0}},
	{eVideoErr,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	334+LOFFSET,		200,	0,	{0}},
	{eShotType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	575+LOFFSET,		30,	0,	{0}},
	{eFovType,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	660+LOFFSET,		30,	0,	{0}},
	{eEnhance,		eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	646+LOFFSET,		30,	0,	{0}},
	{eMeasureDis,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	466+LOFFSET,		30,	0,	{0}},
	/*
	{eMeasureDis_Value1,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	435+LOFFSET,	30,	0,	{0}},
	{eMeasureDis_Value2,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	446+LOFFSET,	30,	0,	{0}},//+23
	{eMeasureDis_Value3,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	457+LOFFSET,	30,	0,	{0}},
	{eMeasureDis_Value4,	eOsd_Disp,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	468+LOFFSET,	30,	0,	{0}},
	*/
	{eLaserState,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	350+LOFFSET,		30,	0,	{0}},
	{eSuperOrder,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	606+LOFFSET,		436,	0,	{0}},
	{eErrorZone,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		436,	0,	{0}},
	{eWeather1,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	420+LOFFSET+WOFFSET,		470,	0,	{0}},
	{eWeather1_T,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	479+LOFFSET+WOFFSET,		470,	0,	{0}},

	{eWeather2,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	420+LOFFSET+WOFFSET,		500,	0,	{0}},
	{eAngleH,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		468,	0,	{0}},
	{eAngleV,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		500,	0,	{0}},

	{eDynamicZone,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	180+LOFFSET,		500,	0,	{0}},
	{eCorrectionTip,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	360+LOFFSET,		500,	0,	{0}},
	{eF1Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	50+LOFFSET,		500,	0,	{0}},
	{eF2Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	160+LOFFSET,		500,	0,	{0}},
	{eF3Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	275+LOFFSET,		500,	0,	{0}},
	{eF4Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	382+LOFFSET,		500,	0,	{0}},
	{eF5Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500-5+LOFFSET,	500,	0,	{0}},
	{eF6Button,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	600+LOFFSET,		500,	0,	{0}},
	{eCalibMenu_Weather,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		65,	0,	{0}},
	{eCalibMenu_Zero,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		100,	0,	{0}},
	{eCalibMenu_General,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		135,	0,	{0}},
	{eCalibMenu_Save,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		170,	0,	{0}},
	{eCalibMenu_GenPram,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		205,	0,	{0}},
	{eCalibMenu_Horizen,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		240,	0,	{0}},
	{eCalibMenu_Laser,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		275,	0,	{0}},
	{eCalibMenu_Child,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	210+LOFFSET,		205,	0,	{0}},
	
	{eCursorX,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	160+LOFFSET,		170,	0,	{0}},
	{eSaveYesNo,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	200+LOFFSET,		170,	0,	{0}},
	{eCalibMenu_SaveOK,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	300+LOFFSET,		65,	0,	{0}},
	{eSelfCheckResult,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		65,	0,	{0}},
	{eSelfCheckState1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		97,	0,	{0}},
	{eSelfCheckState1_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		97,	0,	{0}},
	{eSelfCheckState2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		129,	0,	{0}},
	{eSelfCheckState2_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		129,	0,	{0}},
	{eSelfCheckState3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		161,	0,	{0}},
	{eSelfCheckState3_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		161,	0,	{0}},
	{eSelfCheckState4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		193,	0,	{0}},
	{eSelfCheckState4_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		193,	0,	{0}},
	{eSelfCheckState5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		225,	0,	{0}},
	{eSelfCheckState5_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		225,	0,	{0}},
	{eSelfCheckState6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		257,	0,	{0}},
	{eSelfCheckState6_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		257,	0,	{0}},
	{eSelfCheckState7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		289,	0,	{0}},
	{eSelfCheckState7_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		289,	0,	{0}},
	{eSelfCheckState8,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		321,	0,	{0}},
	{eSelfCheckState8_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		321,	0,	{0}},
	{eSelfCheckState9,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	42+LOFFSET,		353,	0,	{0}},
	{eSelfCheckState9_status,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	240+LOFFSET,		353,	0,	{0}},

	{eCalibZero_D,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	285+LOFFSET+83,		490,	0,	{0}},
	{eCalibZero_Fx,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	379+LOFFSET+80,		490,	0,	{0}},
	{eCalibZero_Fy,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	560+LOFFSET,		490,	0,	{0}},

	{eCalibZero_jc,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		115,	0,	{0}},
	{eCalibZero_AngleGun,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		150,	0,	{0}},
	{eCalibZero_AngleGrenade,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	500+LOFFSET,		185,	0,	{0}},
	{eCalibZero_Angle,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		490,	0,	{0}},

	{eCalibWeather_Tep,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	450+WETHEROFFSET,		100,	0,	{0}},
	{eCalibWeather_Pre,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	450+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_TepPole,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	508+WETHEROFFSET,		100,	0,	{0}},
	{eCalibWeather_TepVal1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	524+WETHEROFFSET,		100,	0,	{0}},
	{eCalibWeather_TepVal2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	535+WETHEROFFSET,		100,	0,	{0}},
	{eCalibWeather_PreVal1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	506+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	517+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	528+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	550+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	561+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	550+WETHEROFFSET,		135,	0,	{0}},
	{eCalibWeather_PreVal7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	700+WETHEROFFSET,		135,	0,	{0}},

	{eCalibGeneral_Dis,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	470+GENOFFSET,		150,	0,	{0}},
	{eCalibGeneral_X,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	470+GENOFFSET,		150,	0,	{0}},
	{eCalibGeneral_Y,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	470+GENOFFSET,		185,	0,	{0}},
	{eCalibGeneral_DisValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	466+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_DisValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	477+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_DisValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	488+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_DisValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	499+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_XPole,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	460+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	474+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	485+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	507+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	518+GENOFFSET+80,	150,	0,	{0}},
	{eCalibGeneral_YPole,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	460+GENOFFSET+80,	185,	0,	{0}},
	{eCalibGeneral_YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	474+GENOFFSET+80,	185,	0,	{0}},
	{eCalibGeneral_YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	485+GENOFFSET+80,	185,	0,	{0}},
	{eCalibGeneral_YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	507+GENOFFSET+80,	185,	0,	{0}},
	{eCalibGeneral_YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	518+GENOFFSET+80,	185,	0,	{0}},
	{eCalibGeneral_Rx,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	480+LOFFSET,		500,	0,	{0}},
	{eCalibGeneral_Ry,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	590+LOFFSET,		500,	0,	{0}},

	{eCalibHorizen_Menu,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		100,	0,	{0}},
	{eCalibHorizen_Pos,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		132,	0,	{0}},
	{eCalibHorizen_Mach,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		164,	0,	{0}},
	{eCalibHorizen_Grenade,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	60+LOFFSET,		196,	0,	{0}},

	{eCalibGenPram_Menu,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		100,	0,	{0}},
	{eCalibGenpram_VFLD0TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	280+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOX,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOY,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		170-6,	0,	{0}},
	{eCalibGenpram_VFLD1TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	280+LOFFSET,	205-9,	0,	{0}},
	{eCalibGenPram_VFLD1X,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1Y,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		240-12,	0,	{0}},
	{eCalibGenpram_VFLD2TAG,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	280+LOFFSET,	275-15,	0,	{0}},
	{eCalibGenPram_VFLD2X,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2Y,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLDOXValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOXValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		135-3,	0,	{0}},
	{eCalibGenPram_VFLDOYValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLDOYValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		170-6,	0,	{0}},
	{eCalibGenPram_VFLD1XValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1XValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		205-9,	0,	{0}},
	{eCalibGenPram_VFLD1YValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD1YValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		240-12,	0,	{0}},
	{eCalibGenPram_VFLD2XValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2XValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		275-15,	0,	{0}},
	{eCalibGenPram_VFLD2YValue0,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	128+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue1,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	139+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue2,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	150+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue3,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	172+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue4,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	183+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue5,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	194+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue6,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	205+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},
	{eCalibGenPram_VFLD2YValue7,	eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	216+LOFFSET+W1TOFFSET,		310-18,	0,	{0}},

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
	
	{eCalcNum_Visual,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		85+CALCOFFSET,	0,	{0}},
	{eCalcNum_Traject,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		117+CALCOFFSET,	0,	{0}},
	{eCalcNum_Trunion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		149+CALCOFFSET,	0,	{0}},
	{eCalcNum_General,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		181+CALCOFFSET,	0,	{0}},
	{eCalcNum_Weather,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		213+CALCOFFSET,	0,	{0}},
	{eCalcNum_Turret,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		245+CALCOFFSET,	0,	{0}},
	{eCalcNum_AlgleD,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		277+CALCOFFSET,	0,	{0}},
	{eCalcNum_Posion,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		309+CALCOFFSET,	0,	{0}},
	
	{eStateMaintD,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		82,	0,	{0}},
	{eStateDetend,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		82,	0,	{0}},
	{eStateClutch,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		114,	0,	{0}},
	{eStateGrenad,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		114,	0,	{0}},
	{eStateFirer,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		146,	0,	{0}},
	{eStateFulCAN,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		146,	0,	{0}},
	{eStateDisCAN0,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		178,	0,	{0}},
	{eStateDisCAN1,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		178,	0,	{0}},
	{eStateMagnet,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		210,	0,	{0}},
	{eStatePosMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		210,	0,	{0}},
	{eStateMacMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		242,	0,	{0}},
	{eStateGreMotor,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	230+LOFFSET,		242,	0,	{0}},
	
	{eStateWeaponCtrlA1,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		274,	0,	{0}},
	{eStateSrvAngle,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		306,	0,	{0}},
	{eStateFulScrAngle,		eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		338,	0,	{0}},
	{eLaserSelect,			eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		370,	0,	{0}},

	{eAnglePosn,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		500,	0,	{0}},
	{eAngleDipX,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	30+LOFFSET,		465,	0,	{0}},
	{eAngleDipY,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		465,	0,	{0}},
	{eAngleMach,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	170+LOFFSET,		500,	0,	{0}},
	{eAngleGred,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	310+LOFFSET,		500,	0,	{0}},
	{eGunTip,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	270+LOFFSET,		70,	0,	{0}},
	{eLaserjlx,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	420,				500,	0,	{0}},
	{eLaserjly,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	530,				500,	0,	{0}},
	{eReady,					eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	560+LOFFSET,		435,	0,	{0}},

	{eGuiling,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	300,				70,	0,	{0}},
	{eLwOrGen,				eOsd_Hide,	eOsd_Update,	eWhite,	eTransparent,	MAX_CONTEXT_LEN,	180,		500,	0,	{0}},
};

volatile FONT_OBJ g_Font={NULL,WHITECOLOR,1,0,0,0,NULL};

unsigned char g_Trk_infor[128]={0};
static int tick=0;
void resetTickNum()
{
	tick =40;
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
	sprintf(WorkOsd[1],"%c%c",198,199);//alert
	sprintf(WorkOsd[2],"%c%c",197,250);//У׼
	sprintf(WorkOsd[3],"%c%c",197,209);//jiao ling
	sprintf(WorkOsd[4],"%c%c",253,243);//zong xiu
	sprintf(WorkOsd[5],"%c%c",219,242);//qi xiang
	sprintf(WorkOsd[6],"%c%c",161,162);//gen zong
	sprintf(WorkOsd[7],"%c%c",253,180);//zong can
	sprintf(WorkOsd[8],"%c%c",186,153);//di ping
	sprintf(WorkOsd[9],"%c%c",201,144);//ji guang

	sprintf(AimOsd[0],"%c",186);//land
	sprintf(AimOsd[1],"%c",206);//sky

	sprintf(MeasureTypeOsd[0],"%c",201);//laser 
	sprintf(MeasureTypeOsd[1],"%c",222);//manual
	sprintf(MeasureTypeOsd[2],":LSBG");//LSBG
	sprintf(MeasureTypeOsd[3],":TMOV");//TMOV
	sprintf(MeasureTypeOsd[4],":ECNL");//ECNL
	sprintf(MeasureTypeOsd[5],":SMNL");//SMNL

	sprintf(GunOsd[0],"%c%c",200,216);// ji qiang
	sprintf(GunOsd[1],"%c%c",223,224);// sha shang
	sprintf(GunOsd[2],"%c%c",189,176);//fang bao
	sprintf(GunOsd[3],"%c%c?",223,224);//sha shang qiang guan//216,193
	sprintf(GunOsd[4],"%c%c?",189,176);//fang bao qiang guan
	sprintf(GunOsd[5],"       ");//fang bao qiang guan


	sprintf(ShotOsd[0],"%c",188);//dan 	liu dan
	sprintf(ShotOsd[1],"%c",210);//lian	liu dan

	sprintf(ShotGunOsd[0],"%c",210);//lian 	JI QIANG
	sprintf(ShotGunOsd[1],"%c",143);//sao
	
	sprintf(FovTypeOsd[0],"%c",187);//big
	sprintf(FovTypeOsd[1],"%c",241);//little
	
	sprintf(SuperOsd[0],"%c%c%c",235,216,236);//TIAO QIANG TA
	sprintf(SuperOsd[1],"%c%c%c",235,218,228);//TIAO QIANG SHENG
	sprintf(SuperOsd[2],"%c%c%c",235,209,239);//TIAO LING WEI
	sprintf(SuperOsd[3],"%c%c%c",252,218,208);//TIAO QIANG KAI
	sprintf(SuperOsd[4],"%c%c%c",252,218,194);//TIAO QIANG GUAN
	sprintf(SuperOsd[5],"%c%c%c",158,159,208);//WEN XIANG KAI
	sprintf(SuperOsd[6],"%c%c%c",158,159,194);//WEN XIANG GUAN
	
	
	//sprintf(DynamicOsd[0],"%c%c%c%c",221,185,205,229);//QI DONG JIE SUO
	sprintf(DynamicOsd[0],"35%c%c%c",211,205,229);	  //35liu jie suo
	sprintf(DynamicOsd[1],"%c%c%c%c",140,209,205,229);//GUI LIN JIE SUO
	sprintf(DynamicOsd[2],"%c%c%c%c",179,202,245,183);//CE JU YI CHANG
	sprintf(DynamicOsd[3],"%c%c%c%c",179,231,188,232);//CE SU DUAN SHI
	sprintf(DynamicOsd[4],"%c%c%c%c",235,236,205,229);//TIAO TA JIE SUO
	sprintf(DynamicOsd[5],"%c%c%c%c",179,202,195,248);//CE JU GUO YUAN
	sprintf(DynamicOsd[6],"%c%c%c%c",209,239,203,184);//LING WEI JIAN CHA
	sprintf(DynamicOsd[7],"%c%c%c%c",253,243,203,184);//zong xiu jian cha
	sprintf(DynamicOsd[8],"%c%c%c%c",163,164,165,166);//MU BIAO DIU SHI
	sprintf(DynamicOsd[9],"%c%c%c%c?",161,162,209,239);//shi fou gui ling

	sprintf(ResultOsd[0],"%c%c",254,183);//zheng chang
	sprintf(ResultOsd[1],"%c%c",245,183);//yi chang
	sprintf(OpenCloseOsd[0],"%c",208);//OEPN
	sprintf(OpenCloseOsd[1],"%c",194);//CLOSE

	sprintf(CalibZeroOsd[0],"%c%c-58%c",197,209,200);//JIAO LIN  5.8 JI QIANG
	sprintf(CalibZeroOsd[1],"%c%c-35%c",197,209,223);//JIAO LIN  35 SHA
	sprintf(CalibZeroOsd[2],"%c%c-35%c",197,209,211);//JIAO LIN  35 LIU

	sprintf(CalibGeneralOsd[0],"%c%c-%c%c",253,243,200,216);//ZHONG XIU	 JI QIANG
	sprintf(CalibGeneralOsd[1],"%c%c-35%c",253,243,223);//ZHOGN XIU 	35 SHA
	sprintf(CalibGeneralOsd[2],"%c%c-35%c",253,243,211);//ZHONG XIU  	35 LIU

	sprintf(ErrorOsd[17],"%c%c%c%c",240,196,213,208);//WEI HU MEN KAI
	sprintf(ErrorOsd[18],"%c%c%c%c",214,255,185,194);//PAO ZHI DONG GUAN
 
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
	Posd[eSaveYesNo] = SaveYesNoOsd[1];
	Posd[eCorrectionTip] = AngleCorrectOsd[0];
	Posd[eEnhance] = EnhanceOsd[0];
	Posd[eMeasureType] = MeasureTypeOsd[0];
	Posd[eErrorZone] = ErrorOsd[15];
	Posd[eGunTip] = ShotGunOsd[0];
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
	//SDK_ASSERT(pMuxPortTran!=NULL && hAvtDev !=NULL && hTimerCtrl!=NULL);

	pCtrlObj = (OSDCTRL_OBJ *)OSA_memAlloc(sizeof(OSDCTRL_OBJ));
	if(pCtrlObj!=NULL)
	{
		memset(pCtrlObj,0,sizeof(OSDCTRL_OBJ));
		pCtrlObj->Interface.pMux	= NULL;//pMuxPortTran;
		pCtrlObj->Interface.pCtrlSend = NULL;
		pCtrlObj->Interface.pCtrlRecv = NULL;
		//pCtrlObj->osdSem			= NULL;//&osdSem;
		pCtrlObj->uSegId				= 0;
		pCtrlObj->uTextNum			= 0;
		pCtrlObj->pTextList			= g_Text;
		pCtrlObj->uInit				= 0;
		pCtrlObj->uRtlX				= 0;
		pCtrlObj->uRtlY				= 0;
		pCtrlObj->uRtlM				= 0;
	}
	
	OSDCTRL_OsdInitial();
	
	pCtrlObj->uInit=1;

	return pCtrlObj;
}


void OSDCTRL_AlgSelect()
{
	if(Posd[eAimType]==AimOsd[0])
		;
	else if(Posd[eAimType]==AimOsd[1])
		;
}


void OSDCTRL_AllHide()
{
	int i;
	for(i=eLaserState; i<=eBoreSightLinId; i++)
	{
		OSDCTRL_ItemHide(i);
	}
}

void OSDCTRL_NoShine()
{
	int i;
	SHINE = FALSE;

	if(eMeasureDis == ShinId || eGunType == ShinId)
		OSDCTRL_ItemShow(ShinId);
	else if(isCalibrationSave()&&(eSaveYesNo==ShinId))
		OSDCTRL_ItemShow(ShinId);
	else
	{
		OSDCTRL_ItemHide(ShinId);
	}	
	ShinId = 0;


	if(shine_table[0] == eMeasureType)
		OSDCTRL_ItemShow(shine_table[0]);
	else
		OSDCTRL_ItemHide(shine_table[0]);	
	shine_table[0] = 0;
}

void OSDCTRL_NoShineShow()
{
	int i;
	SHINE = FALSE;
	OSDCTRL_ItemShow(ShinId);
	for(i=0;i<10;i++)
	{
		if(shine_table[i])
			OSDCTRL_ItemShow(i);
	}
	memset(shine_table,0,10);
	ShinId = 0;
}

void OSDCTRL_ItemShine(int Id)
{
	SHINE = TRUE;
	ShinId = Id;
	if(ShinId == eMeasureDis)
		shine_table[0] = eMeasureType;
}

void OSDCTRL_BattleShow()
{
	static int num = 0;
	int i;
	OSDCTRL_NoShine();
	OSDCTRL_AllHide();
	if(!num)
		OSDCTRL_ItemShow(eDynamicZone);
	for(i=eWeather1; i<=eAngleV; i++)
	{
		OSDCTRL_ItemShow(i);
	}

	Posd[eWorkMode] = WorkOsd[wAuto];
	OSDCTRL_updateAreaN();
	OSDCTRL_AlgSelect();	
	num = 1;
}


void OSDCTRL_CalibMenuShow()
{
	int i;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;
	
	if(!AdCalibMenuFlag && pTextObj[eCursorX].osdInitY > 170)
		pTextObj[eCursorX].osdInitY = 170;
	
	OSDCTRL_AllHide();	
	for(i=eCalibMenu_Weather; i<=eCalibMenu_Save; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	Posd[eSaveYesNo] = SaveYesNoOsd[1];
	for(i=eCursorX; i<=eSaveYesNo; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	
	for(i=eAnglePosn; i<=eAngleGred; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	OSDCTRL_ItemShow(eErrorZone);
	Posd[eWorkMode] = WorkOsd[wCalibration];

	//AdCalibMenu
	if(AdCalibMenuFlag)
	{
		OSDCTRL_ItemShow(eCalibMenu_GenPram);
		OSDCTRL_ItemShow(eCalibMenu_Horizen);
		OSDCTRL_ItemShow(eCalibMenu_Laser);
	}
	else
	{ 
		OSDCTRL_ItemHide(eCalibMenu_GenPram);
		OSDCTRL_ItemHide(eCalibMenu_Horizen);
		OSDCTRL_ItemHide(eCalibMenu_Laser);
	}
	

	OSDCTRL_NoShine();
	//OSDCTRL_AlgSelect();
	OSDCTRL_updateMainMenu(getProjectileType());
}


void OSDCTRL_CalibSaveShow()
{
#if 1
	OSDCTRL_CalibMenuShow();
	OSDCTRL_ItemShow(eSaveYesNo);
	OSDCTRL_ItemShine(eSaveYesNo);
	Posd[eWorkMode] = WorkOsd[wCalibration];
#endif
}

void OSDCTRL_CalibGenPramMenu0Show()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_Menu; i<=eCalibGenPram_VFLD2YValue7; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
}

void OSDCTRL_CalibGenPramMenu1Show()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_MachGunTime; i<=eCalibGenPram_TimeValue7; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
}

void OSDCTRL_CalibGenPramMenu2Show()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_ServoXMenu; i<=eCalibGenPram_ServoXValue14; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
}

void OSDCTRL_CalibGenPramMenu3Show()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGenPram_ServoYMenu; i<=eCalibGenPram_ServoYValue9; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibGenPram];
}

void OSDCTRL_CalibHorizenShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibHorizen_Menu; i<=eCalibHorizen_Grenade; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibHorizen];
}

void OSDCTRL_CalibLaserShow()
{
	OSDCTRL_AllHide();
	
	OSDCTRL_ItemShow(eErrorZone);
	OSDCTRL_ItemShow(eAngleH);
	OSDCTRL_ItemShow(eAngleV);
	OSDCTRL_ItemShow(eLaserjlx);
	OSDCTRL_ItemShow(eLaserjly);
	
	Posd[eWorkMode] = WorkOsd[wCalibLaser];
}


void OSDCTRL_CalibWeatherShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibWeather_Tep; i<=eCalibWeather_PreVal6; i++){
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibWeather];
}


void OSDCTRL_CalibGeneralShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eCalibGeneral_XPole; i<=eCalibGeneral_Ry; i++)
	{
		OSDCTRL_ItemShow(i);
	}

	OSDCTRL_ItemShow(eCalibGeneral_X);
	OSDCTRL_ItemShow(eCalibGeneral_Y);
	
	OSDCTRL_ItemShow(eErrorZone);
	OSDCTRL_ItemShow(eAngleH);
	OSDCTRL_ItemShow(eAngleV);
	Posd[eWorkMode] = WorkOsd[wCalibGeneral];

}

void OSDCTRL_CalibZeroShow()
{
	int i;
	OSDCTRL_AllHide();
	
	for(i=eCalibZero_D; i<=eCalibZero_Fy; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wCalibZero];

	OSDCTRL_ItemShow(eAngleH);
	OSDCTRL_ItemShow(eAngleV);
	OSDCTRL_ItemShow(eErrorZone);

	if(isMachineGun())
	{
		for(i=eCalibZero_AngleGun; i<=eCalibZero_AngleGrenade/*eCalibZero_Angle*/; i++)
		{
			OSDCTRL_ItemHide(i);
		}
	}
	else
	{
		for(i=eCalibZero_AngleGun; i<=eCalibZero_AngleGrenade/*eCalibZero_Angle*/; i++)
		{

			OSDCTRL_ItemShow(i);
		}
		OSDCTRL_ItemShow(eCalibZero_jc);
	}
	OSDCTRL_ItemShine(eMeasureDis);
	//OSDCTRL_NoShine();
}

void OSDCTRL_BattleAlertShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eWeather1; i<=eAngleV; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	OSDCTRL_ItemHide(eDynamicZone);
	Posd[eWorkMode] = WorkOsd[wAlert];
	OSDCTRL_NoShine();
}

void OSDCTRL_BattleTrackShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eWeather1; i<=eAngleV; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	Posd[eWorkMode] = WorkOsd[wTrack];
	OSDCTRL_updateAreaN();
	//AVTCTRL_setAquire();
	OSDCTRL_NoShine();
	OSDCTRL_AlgSelect();
}

void OSDCTRL_EnterBattleMode()
{
	if(isStatBattleAuto())
		OSDCTRL_BattleShow();
	else if(isStatBattleAlert())
	 	 OSDCTRL_BattleAlertShow();

	else 
	{
		assert(FALSE);
	}
}

void OSDCTRL_CalibGenPramShow()
{
#if 1
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
	if(isCalibrationMainMenu())
		OSDCTRL_CalibMenuShow();
	else if(isCalibrationGeneral())
		OSDCTRL_CalibGeneralShow();
	else if(isCalibrationWeather())
		OSDCTRL_CalibWeatherShow();
	else if(isCalibrationZero())
		OSDCTRL_CalibZeroShow();
	else if(isCalibrationSave())	
		OSDCTRL_CalibSaveShow();
	else if(isCalibrationGenPram())	
		OSDCTRL_CalibGenPramShow();
	else if(isCalibrationHorizen())	
		OSDCTRL_CalibHorizenShow();
	else if(isCalibrationLaser())	
		OSDCTRL_CalibLaserShow();
	else 
		assert(FALSE);
	
}


void OSDCTRL_updateAreaN()
{
#if 1
	int i=0;
	bool noErr = FALSE;

	if(!Is9stateOK())
	{
		i = getErrCodeId();
		Posd[eErrorZone] = ErrorOsd[i];
	}
	else if(bDetendClose())
	{
		Posd[eErrorZone] = ErrorOsd[18];
	}
	else if(bMaintPortOpen())
	{
		Posd[eErrorZone] = ErrorOsd[17];
	}
	else
	{
		noErr = TRUE;
	}
	if(noErr || isBootUpMode())
	{
		OSDCTRL_ItemHide(eErrorZone);	
	}
	else
	{
		OSDCTRL_ItemShow(eErrorZone);
		startDynamicTimer();
	}
#endif
}


void OSDCTRL_updateMainMenu(int i)
{
	Posd[eCalibMenu_Zero] = CalibZeroOsd[i];
	Posd[eCalibMenu_General] = CalibGeneralOsd[i];
	return ;
}

void OSDCTRL_updateDistanceValue()
{
	int i;
	/*
	if(isMeasureManual())
	{
		for(i = eMeasureDis_Value1;i<=eMeasureDis_Value4;i++)
			OSDCTRL_ItemShow(i);
	}	
	else
	{
		for(i = eMeasureDis_Value1;i<=eMeasureDis_Value4;i++)
			OSDCTRL_ItemHide(i);
	}
	*/
	if(isMeasureManual())
		OSDCTRL_ItemShow(eMeasureDis);
	else
		OSDCTRL_ItemHide(eMeasureDis);
}

void OSDCTRL_CheckResultsShow()
{
	int i;
	OSDCTRL_AllHide();
	for(i=eSelfCheckState1; i<=eSelfCheckState9_status; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	OSDCTRL_updateAreaN();
}

void OSDCTRL_ItemShow(LPARAM lParam)
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return;

	pTextObj[lParam].osdState = eOsd_Disp;
}

void OSDCTRL_CalcNumShow(void)
{
	int i=0;
	for(i=eCalcNum_Visual; i<=eCalcNum_Posion; i++)
	{
		OSDCTRL_ItemShow(i);
	}
	return ;
}

void OSDCTRL_CalcNumHide(void)
{
	int i=0;
	for(i=eCalcNum_Visual; i<=eCalcNum_Posion; i++)
	{
		OSDCTRL_ItemHide(i);
	}
	return ;
}

void OSDCTRL_FulScrAngleShow()
{
	int i=0;
	for(i=eStateWeaponCtrlA1; i<=eStateFulScrAngle; i++)
	{
		OSDCTRL_ItemShow(i);
	}
}

void OSDCTRL_FulScrAngleHide()
{
	int i=0;
	for(i=eStateWeaponCtrlA1; i<=eStateFulScrAngle; i++)
	{
		OSDCTRL_ItemHide(i);
	}
}

void OSDCTRL_BaseMenuShow()
{
	int i=0;
	for(i=eF1Button; i<=eF6Button; i++)
	{
		OSDCTRL_ItemShow(i);
	}
}

void OSDCTRL_BaseMenuHide()
{
	int i=0;
	for(i=eF1Button; i<=eF6Button; i++)
	{
		OSDCTRL_ItemHide(i);
	}
}

void OSDCTRL_ConnectMenuShow()
{
	int i=0;
	for(i=eStateMaintD; i<=eStateGreMotor; i++)
	{
		OSDCTRL_ItemShow(i);
	}
}

void OSDCTRL_ConnectMenuHide()
{
	int i=0;
	for(i=eStateMaintD; i<=eStateGreMotor; i++)
	{
		OSDCTRL_ItemHide(i);
	}
}

void OSDCTRL_ItemHide(LPARAM lParam)
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return;

	pTextObj[lParam].osdState = eOsd_Hide;	
}

bool OSDCTRL_IsOsdDisplay(LPARAM lParam)
{
#if 1
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
#if 0
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
	
	for(i=eModeId;i<eBoreSightLinId;i++)
	{
		pTextObj = &pCtrlObj->pTextList[i];

		if (pTextObj->osdState==eOsd_Disp)
		{	
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
#endif
}

/*
 *  ======== OSDCTRL_genOsdContext========
 *
 *  This function is used to generation text.
 */
int OSDCTRL_genOsdContext(HANDLE hOsdCtrl,UINT uItemId)
{

   	//int index=0;
	OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )hOsdCtrl;
	OSDText_Obj * pTextObj = NULL;
	char * pStr = NULL;
	static double value =0.0;
	static BYTE Bye;
	int i ;
int ls1tmp = 0000;
int ls2tmp = 0000;
	unsigned char thousand = 0 ,hundred = 0,tens = 0,unit = 0;

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
				Posd[eGunType] = GunOsd[getProjectileType()];
				sprintf(pStr,"%s",Posd[eGunType]);//); 	//��ǹ
			break;
		case eMeasureType:
			if(isMeasureManual())
			{
				sprintf(pStr,"%s:",Posd[eMeasureType]); //manual
			}
			else
			{	
				int Distance = getLaserDistance();
				int DistanceM = getLaserDistanceM();
				//Distance = (Distance < 0)? 0:(Distance);

				Distance = (Distance>1700)?0:(Distance);
				DistanceM= (DistanceM>1700)?0:(DistanceM);
				
				if(isMeasureOsdNormal())
				{
					if(Posd[eLaserState] == LaserOsd[0])
						sprintf(pStr,"%s:%04d",Posd[eLaserState],Distance); //shou
					else
						sprintf(pStr,"%s:%04d",Posd[eLaserState],DistanceM);
				}
				else
				{
					sprintf(pStr,"%s%s",Posd[eLaserState],Posd[eMeasureType]); //shou or mo :
				}
				//OSDCTRL_ItemHide(eMeasureDis);	
			}
			break;
		case eMeasureDis:
			if(isMultiChanged())
			{
				sprintf(pStr,"x%03d",getDisLen()); //����
			}
			else if(!isMeasureManual())
			{
				//do nothing
			}
			else
			{
				sprintf(pStr,"%04d",DistanceManual);			
			}
			break;
	
		case eLaserState:
			sprintf(pStr,"%s",Posd[eLaserState]); 	//��ĩѡͨ
			break;
		case eVideoErr:
			//sprintf(pStr,"%c%c%c%c",230,215,245,183); 	//��Ƶ�쳣
			sprintf(pStr,"SignalEr");
			break;
		case eShotType:
			if(isMachineGun())
				Posd[eShotType] = ShotGunOsd[getGunShotType()];
			else
				Posd[eShotType] = ShotOsd[getShotType()];	
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
			//sprintf(pStr,"T%+02d P%03dk",Temparature,Pressure/10000);//gWeatherTable.Temparature,gWeatherTable.Pressure/1000); 
			sprintf(pStr,"T%+03d",Temparature);
			break;
		
		case eWeather1_T:
			sprintf(pStr,"P%03dk",Pressure/10000); 
			//sprintf(pStr,"T"); 
			break;	
		case eWeather2:
			sprintf(pStr,"R:%+02.1f%+02.1f",getCorrectionCalibX(),getCorrectionCalibY()); 
			break;
		case eErrorZone:
			sprintf(pStr,"%s",Posd[eErrorZone]); 
			break;
		case eDynamicZone:
			sprintf(pStr,"%s",Posd[eDynamicZone]);//); //��������
			break;
		case eAngleH:
			sprintf(pStr,"%c%+03.0f",200,getMachGunAngleAbs()); 
			break;
		case eAngleV:
			sprintf(pStr,"%c%+03.0f",211,getGrenadeAngleAbs());
			break;
		case eCorrectionTip:
			sprintf(pStr,"%s",Posd[eCorrectionTip]);
			break;
		case eF1Button:
			sprintf(pStr,"%c%c",143,156);//200,210); 			//ɨ��//����
			break;
		case eF2Button:
			sprintf(pStr,"%c%c",237,178); 			//ͼ��
			break;
		case eF3Button:
			sprintf(pStr,"%c%c",252,218); 			//��ǿ
			break;
		case eF4Button:
			sprintf(pStr,"%c%c",179,202); 			//���
			break;
		case eF5Button:
			sprintf(pStr,"%c%c",172,173); 			//206 kong
			break;
		case eF6Button:
			sprintf(pStr,"%c%c",145,146); 			//��
			break;
		case eCalibMenu_Weather:
			sprintf(pStr,"%c%c%c%c",219,242,180,225); 			//�������
			break;
		case eCalibMenu_Zero:
			sprintf(pStr,"%s",Posd[eCalibMenu_Zero]);//); 			//У��-5.8��
			break;
		case eCalibMenu_General:
			sprintf(pStr,"%s",Posd[eCalibMenu_General]);//); 			//���-5.8��
			break;
		case eCalibMenu_Save:
			sprintf(pStr,"%c%c%c%c",180,225,181,182); 			//����洢
			break;
		case eCalibMenu_GenPram:
			sprintf(pStr,"%c%c%c%c",253,180,197,250); 			//�۲�У׼
			break;
		case eCalibMenu_Horizen:
			sprintf(pStr,"%c%c%c%c",186,153,197,209); 			//��ƽУ��
			break;
		case eCalibMenu_Laser:
			sprintf(pStr,"%c%c%c%c",201,144,197,209); 			//����У��
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
			if(Is9stateOK())
				Posd[eSelfCheckResult] = ResultOsd[0];
			else
				Posd[eSelfCheckResult] = ResultOsd[1];
			sprintf(pStr,"%c%c:%s",249,203,Posd[eSelfCheckResult]);					//�Լ�:��-�쳣
			break;


		case eSelfCheckState1:
			sprintf(pStr,"%c %c:",244,207);
			break;
			
		case eSelfCheckState1_status:
			if(bWeaponCtrlOK())
				Posd[eSelfCheckState1] = ResultOsd[0];	//normal
			else
				Posd[eSelfCheckState1] = ResultOsd[1]; 	//abnormal
			sprintf(pStr,"%s",Posd[eSelfCheckState1]);	
			break;
			
		case eSelfCheckState2:
			sprintf(pStr,"%c %c:",227,177);
			break;

		case eSelfCheckState2_status:
			if(bJoyStickOK())
				Posd[eSelfCheckState2] = ResultOsd[0];
			else
				Posd[eSelfCheckState2] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState2]); 	
			break;
			
		case eSelfCheckState3:	
			sprintf(pStr,"%c %c:",220,204); 
			break;
			
		case eSelfCheckState3_status:
			if(bDipAngleSensorOK())
				Posd[eSelfCheckState3] = ResultOsd[0];
			else
				Posd[eSelfCheckState3] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState3]); 	
			break;

		case eSelfCheckState4:
			sprintf(pStr,"%c%c%c:",190,239,204); 		//\u951f\u65a4\u62f7\u4f4d\u951f\u65a4\u62f7:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;

		case eSelfCheckState4_status:
			if(bPositionSensorOK())
				Posd[eSelfCheckState4] = ResultOsd[0];
			else
				Posd[eSelfCheckState4] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState4]);	
			break;
			
		case eSelfCheckState5:
			sprintf(pStr,"%c%c%c-%c%c:",191,246,204,200,216); //\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7-\u951f\u65a4\u62f7\u67aa:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;

		case eSelfCheckState5_status:
			if(bMachineGunSensorOK())
				Posd[eSelfCheckState5] = ResultOsd[0];
			else
				Posd[eSelfCheckState5] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState5]);
			break;
			
		case eSelfCheckState6:
			sprintf(pStr,"%c%c%c-35%c:",191,246,204,211); 	//\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7-35\u951f\u65a4\u62f7:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;

		case eSelfCheckState6_status:
			if(bGrenadeSensorOK())
				Posd[eSelfCheckState6] = ResultOsd[0];
			else
				Posd[eSelfCheckState6] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState6]);	
			break;
			
		case eSelfCheckState7:		
			sprintf(pStr,"%c%c%c%c:",190,239,226,192);		//\u951f\u65a4\u62f7\u4f4d\u951f\u811a\u51e4\u62f7:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;

		case eSelfCheckState7_status:
			if(bPositionServoOK())
				Posd[eSelfCheckState7] = ResultOsd[0];
			else
				Posd[eSelfCheckState7] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState7]);	
			break;
			
		case eSelfCheckState8:	
			sprintf(pStr,"%c%c%c%c-%c%c:",191,246,226,192,200,216); //\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u811a\u51e4\u62f7-\u951f\u65a4\u62f7\u67aa:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;

		case eSelfCheckState8_status:
			if(bMachineGunServoOK())
				Posd[eSelfCheckState8] = ResultOsd[0];
			else
				Posd[eSelfCheckState8] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState8]); 	
			break;

		case eSelfCheckState9:
			sprintf(pStr,"%c%c%c%c-35%c:",191,246,226,192,211); 	//\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u811a\u51e4\u62f7-35\u951f\u65a4\u62f7:\u951f\u65a4\u62f7-\u951f\u5c4a\u5e38
			break;	

		case eSelfCheckState9_status:
			if(bGrenadeServoOK())
				Posd[eSelfCheckState9] = ResultOsd[0];
			else
				Posd[eSelfCheckState9] = ResultOsd[1];
			sprintf(pStr,"%s",Posd[eSelfCheckState9]);	
			break;

		case eCalibZero_D:		
			if(isMeasureManual())
				sprintf(pStr,"D:%04d",DistanceManual);	
			else 
				sprintf(pStr,"D:%04d",LaserDistance);	
			//sprintf(pStr,"D:%04d",500);				//ju li:500
			break;
		case eCalibZero_Fx:
			sprintf(pStr,"Fx:%+05d",getCrossX() - 360); 	//Fx
			break;

		case eCalibZero_Fy:
			sprintf(pStr,"Fy:%+05d",288 - getCrossY()); 	//Fy
			break;
		case eCalibZero_AngleGun:
			value = getMachGunAngle();
			sprintf(pStr,"%c%cY:%+08.3f",200,216,value);
			break;
		
		case eCalibZero_AngleGrenade:
			value = getGrenadeAngle();
			sprintf(pStr,"%c%cY:%+08.3f",211,214,value);
			break;

		case eCalibZero_Angle:
			AngleGun = getMachGunAngle();
			AngleGrenade = getGrenadeAngle();
			sprintf(pStr,"%c%+03.0f%c%+03.0f",200,AngleGun,211,AngleGrenade);
			break;
		case eCalibWeather_Tep:
			sprintf(pStr,"%c%c:  %c",219,238,174); 	
			break;
		case eCalibWeather_Pre:
			sprintf(pStr,"%c%c:   Kpa",219,247); 	
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
			sprintf(pStr,"."); 	
			//sprintf(pStr,"%01d",Weather[8]); 	
			break;
		case eCalibWeather_PreVal7:
			//sprintf(pStr,"%01d",Weather[9]); 	
			break;
		case eCalibGeneral_Dis:
			sprintf(pStr,"%c%c:   m",202,212);//,General[0]); 		//����:xxxx m
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
			//sprintf(pStr,"%c%cX:   cm",253,243); 		//����X:+xxxx cm
			sprintf(pStr,"%c%cX:  . mil",253,243); 
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
			sprintf(pStr,"%c%cY:  . mil",253,243); 		//����Y:+xxxx cm
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
			//value = General[5]*10 + General[6] + General[7]*0.1 + General[8]*0.01;
			//if(General[4] < 0)
			//	value = -value;
			//sprintf(pStr,"Rx:%+06.2fm",value); 		//����X:+xxxx cm
			sprintf(pStr,"X:%+06.2f",gGenparmX); 
			break;

		case eCalibGeneral_Ry:
			//value = General[10]*10 + General[11] + General[12]*0.1 + General[13]*0.01;
			//if(General[9] < 0)
			//	value = -value;
			//sprintf(pStr,"Ry:%+06.2fm",value); 		//����X:+xxxx cm
			sprintf(pStr,"Y:%+06.2f",gGenparmY);
			break;

		case eCalibZero_jc:
			sprintf(pStr,"%c%c",203,184); 
			break;
			
		case eCalibHorizen_Menu:
			if(menu_jiancha_flag)
				sprintf(pStr,"%c%c",181,182); // cun chu
			else
				sprintf(pStr,"%c%c<->",203,184); // jian cha	
			break;
		case eCalibHorizen_Pos:
			sprintf(pStr,"%c%c_X:%+08.3f",216,236,getTurretTheta()); 		//����
			break;
		case eCalibHorizen_Mach:
			sprintf(pStr,"%c%c_Y:%+08.3f",200,216,getMachGunAngleAbs()); 		//��ǹ
			break;
		case eCalibHorizen_Grenade:
			sprintf(pStr,"%c%c_Y:%+08.3f",211,175,getGrenadeAngleAbs()); 		//��
			break;
		case eCalibGenPram_Menu:
			sprintf(pStr,"VF-Degree"); 		//���
			break;
		case eCalibGenpram_VFLD0TAG:
			sprintf(pStr,"CCD-Big"); 		//CCD-BIG
			break;
		case eCalibGenPram_VFLDOX:
			sprintf(pStr,"VFLD0-X=  .  ",12.0); 		//VFLD0-X
			break;
		case eCalibGenPram_VFLDOY:
			sprintf(pStr,"VFLD0-Y=  .  ",9.0); 		//VFLD0-Y
			break;
		case eCalibGenpram_VFLD1TAG:
			sprintf(pStr,"CCD-Small"); 		//CCD-Small
			break;
		case eCalibGenPram_VFLD1X:
			sprintf(pStr,"VFLD1-X=  .  ",4.3); 		//VFLD1-X
			break;
		case eCalibGenPram_VFLD1Y:
			sprintf(pStr,"VFLD1-Y=  .  ",3.3); 		//VFLD1-Y
			break;
		case eCalibGenpram_VFLD2TAG:
			sprintf(pStr,"IR-Big"); 		//IR-Big
			break;
		case eCalibGenPram_VFLD2X:
			sprintf(pStr,"VFLD2-X=  .  ",116.0); 		//VFLD2-X
			break;
		case eCalibGenPram_VFLD2Y:
			sprintf(pStr,"VFLD2-Y=  .  ",112.0); 		//VFLD2-Y
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
			sprintf(pStr,"Px:%+06.2f,Py:%+06.2f,Spx:%+06.2f,Spy:%+06.2f",1.1,2.2,3.3,4.4); 
			break;
		case eCalcNum_Visual:
			sprintf(pStr,"%c%cX=%+08.2f %c%cY=%+08.2fmil",172,173,getSumCalibX(),172,173,getSumCalibY()); //��Ԫ:���� ��
			break;			
		case eCalcNum_Traject:
			sprintf(pStr,"%c%cX=%+08.2f %c%cY=%+08.2fmil",205,168,getTrajectCalcX(),205,168,getTrajectCalcY()); //������� 
			break;
		case eCalcNum_Trunion:
			sprintf(pStr,"%c%cX=%+08.2f %c%cY=%+08.2fmil",169,170,getTrunionCalibX(),169,170,getTrunionCalibY()); //���������� 
			break;			
		case eCalcNum_General:
			sprintf(pStr,"%c%cX=%+08.2f %c%cY=%+08.2fmil",253,243,getCorrectionCalibX(),253,243,getCorrectionCalibY()); 		//���޲���
			break;
		case eCalcNum_Weather:
			sprintf(pStr,"%c%c=%+03d%c %c%c=%+08.3fKpa",219,238,gWeatherTable.Temparature,174,219,247,gWeatherTable.Pressure/10000.0); //���� = +15 ����ѹ = +101.325 Kpa:
			break;
		case eCalcNum_Turret:
			sprintf(pStr,"%c%cX=%+08.2fdeg",190,239,getTurretTheta()); //��λX
			break;
		case eCalcNum_AlgleD:
			sprintf(pStr,"%c%cY=%+08.2f 35%cY=%+08.2fdeg",200,216,getMachGunAngle(),211,getGrenadeAngle()); //��ǹY=.. ��Y=..
			break;
		case eCalcNum_Posion:
			sprintf(pStr,"%c%cX=%+08.2f %c%cY=%+08.2fdeg",220,204,getPlatformPositionX(),220,204,getPlatformPositionY()); //���X=.. ���Y=..
			break;

		case eStateMaintD:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :%s",240,196,213,OpenCloseOsd[BIT1(Bye)]); //ά����
			break;
		case eStateDetend:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :%s",214,255,185,OpenCloseOsd[1-BIT2(Bye)]); //����ֹ����
			break;
		case eStateClutch:
			Bye = getFrameA3Byte1();
			sprintf(pStr,"%c%c%c  :%s",212,150,154,OpenCloseOsd[BIT3(Bye)]); //�����
			break;
			
		case eStateGrenad:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"35%c%c%c :%s",211,149,154,ResultOsd[1-BIT1(Bye)]); //35������
			break;
		case eStateFirer:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%c   :%s",151,149,ResultOsd[1-BIT2(Bye)]); //����
			break;
		case eStateFulCAN:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN :%s",155,152,ResultOsd[1-BIT3(Bye)]); //ȫ��CAN
			break;
		case eStateDisCAN0:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN0 :%s",244,207,ResultOsd[1-BIT5(Bye)]); //�Կ�CAN0
			break;
		case eStateDisCAN1:
			Bye = getFrameA3Byte2();
			sprintf(pStr,"%c%cCAN1 :%s",244,207,ResultOsd[1-BIT6(Bye)]); //�Կ�CAN1
			break;
			
		case eStateMagnet:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"%c%c%c  :%s",148,147,160,ResultOsd[1-BIT0(Bye)]); //�����
			break;
		case eStatePosMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"%c%c%c%c :%s",157,153,148,200,ResultOsd[1-BIT1(Bye)]); //ˮƽ���
			break;
		case eStateMacMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"58%c%c  :%s",148,200,ResultOsd[1-BIT2(Bye)]); //58���
			break;
		case eStateGreMotor:
			Bye = getFrameA3Byte3();
			sprintf(pStr,"35%c%c%c :%s",211,148,200,ResultOsd[1-BIT3(Bye)]); //35����
			break;

		case eStateWeaponCtrlA1:
			sprintf(pStr,"WeaponCtrlA1=%s",getWeaponCtrlA1()); //WeaponCtrlA1
			break;
		case eStateSrvAngle:
			sprintf(pStr,"SrvNrmAngle=%+07.2f %+07.2f %+07.2f",getTurretTheta(),getMachGunAngle(),getGrenadeAngle()); //�ŷ��Ƕ���Ϣ
			break;
		case eStateFulScrAngle:
			sprintf(pStr,"FulScrAngle=%+07.2f %+07.2f",getpanoAngleV(),getpanoAngleH()); //ȫ���Ƕ���Ϣ=..
			//sprintf(pStr,"FulScrAngle=%s %+07.2f %+07.2f",getFulScrA0(),getpanoAngleV(),getpanoAngleH()); //ȫ���Ƕ���Ϣ=..
			break;
		case eLaserSelect:
			sprintf(pStr,"%c%c%c%c:      %s",201,144,145,146,Posd[eLaserState]);//������ĩѡͨ
			break;
		case eAnglePosn:
			sprintf(pStr,"%c%c%+07.2f",190,239,getTurretTheta()); //��λ��
			break;
		case eAngleDipX:
			sprintf(pStr,"%cX:%+07.2f",220,getPlatformPositionX()); //���X
			break;
		case eAngleDipY:
			sprintf(pStr,"%cY:%+07.2f",220,getPlatformPositionY()); //���Y
			break;
		case eAngleMach:
			sprintf(pStr,"%cY:%+07.2f",200,getMachGunAngle()); //��ǹY
			break;
		case eAngleGred:
			sprintf(pStr,"%cY:%+07.2f",211,getGrenadeAngle()); //��Y
			break;
		case eGunTip:
			sprintf(pStr,"%s",Posd[eGunTip]); //����ȷ����ʾ
			break;	
			
		case eLaserjlx:
			sprintf(pStr,"Lx:%+05d",getCrossX());
			break;

		case eLaserjly:
			sprintf(pStr,"Ly:%+05d",getCrossY());
			break;

		case eReady:
			sprintf(pStr,"%c%c",141,142);
			break;
		case eGuiling:
			sprintf(pStr,"%c%c%c%c?",235,236,140,209);
			break;
		case eLwOrGen:
			sprintf(pStr,Posd[eLwOrGen]);
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


void increaseFireViewPram()
{
	int i = ShinId-eCalibGenPram_VFLDOXValue0;	
	if(!SHINE)
		return;

	CalibGenPram_VFLD[i] = (CalibGenPram_VFLD[i] + 1)%10;
}

void increaseFireCtrlPram()
{
	int i = ShinId-eCalibGenPram_TimeValue0;	
	if(!SHINE)
		return;

	TimeValue[i] = (TimeValue[i] + 1)%10;
}

void increaseServoXPram()
{
	codeX = (codeX+1)%15;
}

void increaseServoYPram()
{
	codeY = (codeY+1)%10;
}

void decreaseFireViewPram()
{
	int i = ShinId-eCalibGenPram_VFLDOXValue0;	
	if(!SHINE)
		return;

	CalibGenPram_VFLD[i] = (CalibGenPram_VFLD[i] + 9)%10;
}

void moveLeftServoXPram()
{
	if(signX=='+')
		signX = '-';
	else
		signX = '+';
}


void moveLeftServoYPram()
{
	if(signY=='+')
		signY = '-';
	else
		signY = '+';
}


void isBeyondServoX(int id)
{
	if((1<=id)&&(id<=9)){
		ServoX[id]= (ServoX[id]>8.0)?(8.0):ServoX[id];
		ServoX[id]= (ServoX[id]<0.0)?(0.0):ServoX[id];
	}else if((10<=id)&&(id<=14)){
		ServoX[id]= (ServoX[id]>99.0)?(99.0):ServoX[id];
		ServoX[id]= (ServoX[id]<0.0)?(0.0):ServoX[id];
	}else{
		ServoX[id]= (ServoX[id]>300.0)?(300.0):ServoX[id];
		ServoX[id]= (ServoX[id]<0.0)?(0.0):ServoX[id];
	}
}


void moveRightServoXPram()
{
	if(signX=='+')
		ServoX[codeX] += addX;
	else
		ServoX[codeX] -= addX;
	isBeyondServoX(codeX);
}


void isBeyondServoY(int id)
{
	if(id){
		ServoY[id]= (ServoY[id]>8.0)?(8.0):ServoY[id];
		ServoY[id]= (ServoY[id]<0.0)?(0.0):ServoY[id];
	}else{
		ServoY[id]= (ServoY[id]>300.0)?(300.0):ServoY[id];
		ServoY[id]= (ServoY[id]<0.0)?(0.0):ServoY[id];
	}
}


void moveRightServoYPram()
{
	if(signY=='+'){
		ServoY[codeY] += addY;
	}else{
		ServoY[codeY] -= addY;
	}
	isBeyondServoY(codeY);
}


void processCMD_ADCALIBMENU_SWITCH(long lParam)
{
	AdCalibMenuFlag = !AdCalibMenuFlag;
}


void initilFireViewPram()
{
	CalibGenPram_VFLD[0]   = CCD_Big[0]/10000000%10;
	CalibGenPram_VFLD[1]   = CCD_Big[0]/1000000%10;
	CalibGenPram_VFLD[2]   = CCD_Big[0]/100000%10;
	CalibGenPram_VFLD[3]   = CCD_Big[0]/10000%10;
	CalibGenPram_VFLD[4]   = CCD_Big[0]/1000%10;
	CalibGenPram_VFLD[5]   = CCD_Big[0]/100%10;
	CalibGenPram_VFLD[6]   = CCD_Big[0]/10%10;
	CalibGenPram_VFLD[7]   = CCD_Big[0]/1%10;

	CalibGenPram_VFLD[8]   = CCD_Big[1]/10000000%10;
	CalibGenPram_VFLD[9]   = CCD_Big[1]/1000000%10;
	CalibGenPram_VFLD[10] = CCD_Big[1]/100000%10;
	CalibGenPram_VFLD[11] = CCD_Big[1]/10000%10;
	CalibGenPram_VFLD[12] = CCD_Big[1]/1000%10;
	CalibGenPram_VFLD[13] = CCD_Big[1]/100%10;
	CalibGenPram_VFLD[14] = CCD_Big[1]/10%10;
	CalibGenPram_VFLD[15] = CCD_Big[1]/1%10;

	CalibGenPram_VFLD[16]   = CCD_Small[0]/10000000%10;
	CalibGenPram_VFLD[17]   = CCD_Small[0]/1000000%10;
	CalibGenPram_VFLD[18]   = CCD_Small[0]/100000%10;
	CalibGenPram_VFLD[19]   = CCD_Small[0]/10000%10;
	CalibGenPram_VFLD[20]   = CCD_Small[0]/1000%10;
	CalibGenPram_VFLD[21]   = CCD_Small[0]/100%10;
	CalibGenPram_VFLD[22]   = CCD_Small[0]/10%10;
	CalibGenPram_VFLD[23]   = CCD_Small[0]/1%10;

	CalibGenPram_VFLD[24]   = CCD_Small[1]/10000000%10;
	CalibGenPram_VFLD[25]   = CCD_Small[1]/1000000%10;
	CalibGenPram_VFLD[26] = CCD_Small[1]/100000%10;
	CalibGenPram_VFLD[27] = CCD_Small[1]/10000%10;
	CalibGenPram_VFLD[28] = CCD_Small[1]/1000%10;
	CalibGenPram_VFLD[29] = CCD_Small[1]/100%10;
	CalibGenPram_VFLD[30] = CCD_Small[1]/10%10;
	CalibGenPram_VFLD[31] = CCD_Small[1]/1%10;

	CalibGenPram_VFLD[32]   = IR_Big[0]/10000000%10;
	CalibGenPram_VFLD[33]   = IR_Big[0]/1000000%10;
	CalibGenPram_VFLD[34]   = IR_Big[0]/100000%10;
	CalibGenPram_VFLD[35]   = IR_Big[0]/10000%10;
	CalibGenPram_VFLD[36]   = IR_Big[0]/1000%10;
	CalibGenPram_VFLD[37]   = IR_Big[0]/100%10;
	CalibGenPram_VFLD[38]   = IR_Big[0]/10%10;
	CalibGenPram_VFLD[39]   = IR_Big[0]/1%10;

	CalibGenPram_VFLD[40]   = IR_Big[1]/10000000%10;
	CalibGenPram_VFLD[41]   = IR_Big[1]/1000000%10;
	CalibGenPram_VFLD[42] = IR_Big[1]/100000%10;
	CalibGenPram_VFLD[43] = IR_Big[1]/10000%10;
	CalibGenPram_VFLD[44] = IR_Big[1]/1000%10;
	CalibGenPram_VFLD[45] = IR_Big[1]/100%10;
	CalibGenPram_VFLD[46] = IR_Big[1]/10%10;
	CalibGenPram_VFLD[47] = IR_Big[1]/1%10;
}

void saveFireViewPram()
{
	CCD_Big[0] = CalibGenPram_VFLD[7] + CalibGenPram_VFLD[6]*10 + CalibGenPram_VFLD[5]*100 
		+ CalibGenPram_VFLD[4]*1000 + CalibGenPram_VFLD[3]*10000 + CalibGenPram_VFLD[2]*100000 
		+ CalibGenPram_VFLD[1]*1000000 + CalibGenPram_VFLD[0]*10000000;

	CCD_Big[1] = CalibGenPram_VFLD[15] + CalibGenPram_VFLD[14]*10 + CalibGenPram_VFLD[13]*100 
		+ CalibGenPram_VFLD[12]*1000 + CalibGenPram_VFLD[11]*10000 + CalibGenPram_VFLD[10]*100000 
		+ CalibGenPram_VFLD[9]*1000000 + CalibGenPram_VFLD[8]*10000000;

	CCD_Small[0] = CalibGenPram_VFLD[23] + CalibGenPram_VFLD[22]*10 + CalibGenPram_VFLD[21]*100 
		+ CalibGenPram_VFLD[20]*1000 + CalibGenPram_VFLD[19]*10000 + CalibGenPram_VFLD[18]*100000 
		+ CalibGenPram_VFLD[17]*1000000 + CalibGenPram_VFLD[16]*10000000;

	CCD_Small[1] = CalibGenPram_VFLD[31] + CalibGenPram_VFLD[30]*10 + CalibGenPram_VFLD[29]*100 
		+ CalibGenPram_VFLD[28]*1000 + CalibGenPram_VFLD[27]*10000 + CalibGenPram_VFLD[26]*100000 
		+ CalibGenPram_VFLD[25]*1000000 + CalibGenPram_VFLD[24]*10000000;

	IR_Big[0] = CalibGenPram_VFLD[39] + CalibGenPram_VFLD[38]*10 + CalibGenPram_VFLD[37]*100 
		+ CalibGenPram_VFLD[36]*1000 + CalibGenPram_VFLD[35]*10000 + CalibGenPram_VFLD[34]*100000 
		+ CalibGenPram_VFLD[33]*1000000 + CalibGenPram_VFLD[32]*10000000;

	IR_Big[1] = CalibGenPram_VFLD[47] + CalibGenPram_VFLD[46]*10 + CalibGenPram_VFLD[45]*100 
		+ CalibGenPram_VFLD[44]*1000 + CalibGenPram_VFLD[43]*10000 + CalibGenPram_VFLD[42]*100000 
		+ CalibGenPram_VFLD[41]*1000000 + CalibGenPram_VFLD[40]*10000000;
}

#ifndef _osdProcess_H_
#define _osdProcess_H_
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#ifdef __cplusplus
extern "C"{
#endif

//#include "sdkDef.h"
//#include "sdkErr.h"
//#include "ctimer.h"
//#include "modleCtrl.h"
//#include "avtDevice.h"
//#include "osdCdc.h"
#include "WeaponCtrl.h"
#include "GrenadePort.h"
#include "LaserPort.h"
#include "MachGunPort.h"
#include "PositionPort.h"
#include "TurretPosPort.h"
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/legacy/compat.hpp>
#include "permanentStorage.h"
#include "msgDriv.h"


//#define OSD_TEXT_SIZE		150 //18
#define MAX_CONTEXT_LEN 	64

#define OSD_LINE_NUM		7
#define BORESIGHT_LEN		20
#define FOV_BORDER_LEN		10
#define PICP_BORESIGHT_LEN 	5

#define MAX_PORT_NUM 12 //4

#define sendCommand(MSG_ID) MSGDRIV_send(MSG_ID,0)//PostMessage(MSG_ID)

#define stoh2(buf) ((buf[0]<<8)|(buf[1]))
#define STOH2 stoh2
#define BYTE0(buf)   (buf[0])
#define BYTE1(buf)   (buf[1])
#define BYTE2(buf)   (buf[2])
#define BYTE3(buf)   (buf[3])
#define BYTE4(buf)   (buf[4])
#define BYTE5(buf)   (buf[5])
#define BYTE6(buf)   (buf[6])
#define BYTE7(buf)   (buf[7])
#define BYTE8(buf)   (buf[8])
#define BYTE9(buf)   (buf[9])
#define BYTE10(buf)   (buf[10])

#define BIT0(data)   (((data)&0x01)>>0)
#define BIT1(data)   (((data)&0x02)>>1)
#define BIT2(data)   (((data)&0x04)>>2)
#define BIT3(data)   (((data)&0x08)>>3)
#define BIT4(data)   (((data)&0x10)>>4)
#define BIT5(data)   (((data)&0x20)>>5)
#define BIT6(data)   (((data)&0x40)>>6)
#define BIT7(data)   (((data)&0x80)>>7)
#define BIT6_4(data) (((data)>>4)&0x07)
#define BIT3_2(data) (((data)>>2)&0x03)
#define BIT1_0(data) (((data))&0x03)
#define BIT7_6(data) (((data)>>6)&0x03)
#define BIT5_4(data) (((data)>>4)&0x03)
#define BIT3_0(data) (((data))&0x0F)
#define BIT7_4(data) (((data)>>4)&0x0F)

//extern OSDCTRL_Handle  pOsdCtrlObj	;



#if 0
typedef struct _weather_table{
	int Temparature;
	int Pressure;
}WeatherItem;
#endif

typedef enum _eOsdTypeId{
	eModeId			=	0x00,
	eWorkMode		=	0x01,
	eAimType		=	0x02,
	eGunType		=	0x03,
	eMeasureType	=	0x04,
	eVideoErr		=	0x05,
	eShotType		=	0x06,
	eFovType		=	0x07,
	eEnhance		=	0x08,
	eMeasureDis		=	0x09,

	eMeasureDis_Value1,
	eMeasureDis_Value2,
	eMeasureDis_Value3,
	eMeasureDis_Value4,
	
	eLaserState	,//	=	0x0A,//��Ϊ�������ĩѡͨ��ʾʹ��
	eSuperOrder	,//	=	0x0B,
	eErrorZone	,//	=	0x0C,
	eWeather1	,
	eWeather1_T,
	eWeather1_pole,
	eweather1_tmperature1,
	eweather1_tmperature2,
	eweather1_p,
	eweather1_qiya1,
	eweather1_qiya2,
	eweather1_qiya3,
	eweather1_qiya4,
	eWeather2	,//	=	0x0E,
	eweather2_R,
	eweather2_maohao,
	eweather2_pole1,
	eweather2_offx_1,
	eweather2_offx_2,
	eweather2_offx_3,
	eweather2_pole2,
	eweather2_offy_1,
	eweather2_offy_2,
	eweather2_offy_3,

	eAngleH_char,
	eAngleH_pole,
	eAngleH_s1,
	eAngleH_s2,
	eAngleH		,//	=	0x0F,
	
	eAngleV_char,
	eAngleV_pole,
	eAngleV_s1,
	eAngleV_s2,
	eAngleV		,//	=	0x10,
	eDynamicZone,//	=	0x11,
	eCorrectionTip,//	=	0x12,
	eF1Button	,//	=	0x13,
	eF2Button	,//	=	0x14,
	eF3Button	,//	=	0x15,
	eF4Button	,//	=	0x16,
	eF5Button	,//	=	0x17,
	eF6Button	,//	=	0x18,
	eCalibMenu_Weather	,//=	0x19,
	eCalibMenu_Zero	,//=	0x1A,
	eCalibMenu_General,//=	0x1B,
	eCalibMenu_Save	,//=	0x1C,
	eCalibMenu_GenPram,
	eCalibMenu_Horizen,
	eCalibMenu_Laser,
	eCalibMenu_Child,
	eCursorX		,//=	0x1D,
	eSaveYesNo		,//=	0x1E,
	eCalibMenu_SaveOK,//=	0x1F,
	eSelfCheckResult		,//=	0x20,//40
	eSelfCheckState1,//=	0x21,
	eSelfCheckState2,//=	0x22,
	eSelfCheckState3,//=	0x23,
	eSelfCheckState4,//=	0x24,
	eSelfCheckState5,//=	0x25,
	eSelfCheckState6,//=	0x26,
	eSelfCheckState7,//=	0x27,
	eSelfCheckState8,//=	0x28,
	eSelfCheckState9,//=	0x29,
	eCalibZero_D	,//=	0x2A,
	eCalibZero_D_d,
	eCalibZero_D_mh,
	eCalibZero_D_s1,
	eCalibZero_D_s2,
	eCalibZero_D_s3,
	eCalibZero_D_s4,

	eCalibZero_Fx	,//=	0x2B,
	eCalibZero_Fx_F,
	eCalibZero_Fx_x,
	eCalibZero_Fx_mh,
	eCalibZero_Fx_pole,
	eCalibZero_Fx_s1,
	eCalibZero_Fx_s2,
	eCalibZero_Fx_s3,
	eCalibZero_Fx_s4,

	eCalibZero_Fy_F,
	eCalibZero_Fy_y,
	eCalibZero_Fy_mh,
	eCalibZero_Fy_pole,
	eCalibZero_Fy_s1,
	eCalibZero_Fy_s2,
	eCalibZero_Fy_s3,
	eCalibZero_Fy_s4,
	eCalibZero_Fy	,//=	0x2C,
	
	eCalibZero_AngleGun	,//=0x2D,
	eCalibZero_AngleGun_c,
	eCalibZero_AngleGun_m,
	eCalibZero_AngleGun_pole,
	eCalibZero_AngleGun_s1,
	eCalibZero_AngleGun_s2,
	eCalibZero_AngleGun_d,
	eCalibZero_AngleGun_s3,
	eCalibZero_AngleGun_s4,
	
	eCalibZero_AngleGrenade,//=0x2E,
	eCalibZero_AngleGrenade_c,
	eCalibZero_AngleGrenade_m,
	eCalibZero_AngleGrenade_pole,
	eCalibZero_AngleGrenade_s1,
	eCalibZero_AngleGrenade_s2,
	eCalibZero_AngleGrenade_d,
	eCalibZero_AngleGrenade_s3,
	eCalibZero_AngleGrenade_s4,

	eCalibZero_Angle_c1,
	eCalibZero_Angle_pole1,
	eCalibZero_Angle_s11,
	eCalibZero_Angle_s12,
	eCalibZero_Angle_c2,
	eCalibZero_Angle_pole2,
	eCalibZero_Angle_s21,
	eCalibZero_Angle_s22,

	eCalibZero_Angle,//=	0x2F,
	eCalibWeather_Tep	,//=	0x30,
	eCalibWeather_Pre	,//=	0x31,
	eCalibWeather_TepPole,//=0x32,
	eCalibWeather_TepVal1,//=0x33,
	eCalibWeather_TepVal2,//=0x34,
	eCalibWeather_PreVal1,//=0x35,
	eCalibWeather_PreVal2,//=0x36,
	eCalibWeather_PreVal3,//=0x37,
	eCalibWeather_PreVal4,//=0x38,
	eCalibWeather_PreVal5,//=0x39,
	eCalibWeather_PreVal6,//=0x3A,
	eCalibWeather_PreVal7,//=0x3B,
	eCalibGeneral_Dis,//	= 0x3C,
	eCalibGeneral_X	,//	= 0x3D,
	eCalibGeneral_Y	,//	= 0x3E,
	eCalibGeneral_DisValue1,//=0x3F,
	eCalibGeneral_DisValue2,
	eCalibGeneral_DisValue3,
	eCalibGeneral_DisValue4,
	eCalibGeneral_XPole,//	= 0x40,
	eCalibGeneral_XValue1,//=0x41,
	eCalibGeneral_XValue2,//=0x42,
	eCalibGeneral_XValue3,//=0x43,
	eCalibGeneral_XValue4,//=0x44,
	eCalibGeneral_YPole,//	= 0x45,
	eCalibGeneral_YValue1,//=0x46,
	eCalibGeneral_YValue2,//=0x47,
	eCalibGeneral_YValue3,//=0x48,
	eCalibGeneral_YValue4,//=0x49,

	eCalibGeneral_Rx,
	eCalibGeneral_Rx_R,
	eCalibGeneral_Rx_x,
	eCalibGeneral_Rx_m,
	eCalibGeneral_Rx_pole,
	eCalibGeneral_Rx_s1,
	eCalibGeneral_Rx_s2,
	eCalibGeneral_Rx_d,
	eCalibGeneral_Rx_s3,
	eCalibGeneral_Rx_s4,

	eCalibGeneral_Ry_R,
	eCalibGeneral_Ry_x,
	eCalibGeneral_Ry_m,
	eCalibGeneral_Ry_pole,
	eCalibGeneral_Ry_s1,
	eCalibGeneral_Ry_s2,
	eCalibGeneral_Ry_d,
	eCalibGeneral_Ry_s3,
	eCalibGeneral_Ry_s4,	
	eCalibGeneral_Ry,
	
	eCalibHorizen_Menu,
	eCalibHorizen_Pos,
	eCalibHorizen_Mach,
	eCalibHorizen_Grenade,
	eCalibGenPram_Menu,
	eCalibGenpram_VFLD0TAG,
	eCalibGenPram_VFLDOX,
	eCalibGenPram_VFLDOY,
	eCalibGenpram_VFLD1TAG,
	eCalibGenPram_VFLD1X,
	eCalibGenPram_VFLD1Y,
	eCalibGenpram_VFLD2TAG,
	eCalibGenPram_VFLD2X,
	eCalibGenPram_VFLD2Y,
	eCalibGenPram_VFLDOXValue0,
	eCalibGenPram_VFLDOXValue1,
	eCalibGenPram_VFLDOXValue2,
	eCalibGenPram_VFLDOXValue3,
	eCalibGenPram_VFLDOXValue4,
	eCalibGenPram_VFLDOXValue5,
	eCalibGenPram_VFLDOXValue6,
	eCalibGenPram_VFLDOXValue7,
	eCalibGenPram_VFLDOYValue0,
	eCalibGenPram_VFLDOYValue1,
	eCalibGenPram_VFLDOYValue2,
	eCalibGenPram_VFLDOYValue3,
	eCalibGenPram_VFLDOYValue4,
	eCalibGenPram_VFLDOYValue5,
	eCalibGenPram_VFLDOYValue6,
	eCalibGenPram_VFLDOYValue7,
	eCalibGenPram_VFLD1XValue0,
	eCalibGenPram_VFLD1XValue1,
	eCalibGenPram_VFLD1XValue2,
	eCalibGenPram_VFLD1XValue3,
	eCalibGenPram_VFLD1XValue4,
	eCalibGenPram_VFLD1XValue5,
	eCalibGenPram_VFLD1XValue6,
	eCalibGenPram_VFLD1XValue7,
	eCalibGenPram_VFLD1YValue0,
	eCalibGenPram_VFLD1YValue1,
	eCalibGenPram_VFLD1YValue2,
	eCalibGenPram_VFLD1YValue3,
	eCalibGenPram_VFLD1YValue4,
	eCalibGenPram_VFLD1YValue5,
	eCalibGenPram_VFLD1YValue6,
	eCalibGenPram_VFLD1YValue7,
	eCalibGenPram_VFLD2XValue0,
	eCalibGenPram_VFLD2XValue1,
	eCalibGenPram_VFLD2XValue2,
	eCalibGenPram_VFLD2XValue3,
	eCalibGenPram_VFLD2XValue4,
	eCalibGenPram_VFLD2XValue5,
	eCalibGenPram_VFLD2XValue6,
	eCalibGenPram_VFLD2XValue7,
	eCalibGenPram_VFLD2YValue0,
	eCalibGenPram_VFLD2YValue1,
	eCalibGenPram_VFLD2YValue2,
	eCalibGenPram_VFLD2YValue3,
	eCalibGenPram_VFLD2YValue4,
	eCalibGenPram_VFLD2YValue5,
	eCalibGenPram_VFLD2YValue6,
	eCalibGenPram_VFLD2YValue7,
	eCalibGenPram_MachGunTime,
	eCalibGenPram_GrenadeTime,
	eCalibGenPram_TimeValue0,
	eCalibGenPram_TimeValue1,
	eCalibGenPram_TimeValue2,
	eCalibGenPram_TimeValue3,
	eCalibGenPram_TimeValue4,
	eCalibGenPram_TimeValue5,
	eCalibGenPram_TimeValue6,
	eCalibGenPram_TimeValue7,
	eCalibGenPram_ServoXMenu,
	eCalibGenPram_ServoXItem0,
	eCalibGenPram_ServoXItem1,
	eCalibGenPram_ServoXItem2,
	eCalibGenPram_ServoXValue0,
	eCalibGenPram_ServoXValue1,
	eCalibGenPram_ServoXValue2,
	eCalibGenPram_ServoXValue3,
	eCalibGenPram_ServoXValue4,
	eCalibGenPram_ServoXValue5,
	eCalibGenPram_ServoXValue6,
	eCalibGenPram_ServoXValue7,
	eCalibGenPram_ServoXValue8,
	eCalibGenPram_ServoXValue9,
	eCalibGenPram_ServoXValue10,
	eCalibGenPram_ServoXValue11,
	eCalibGenPram_ServoXValue12,
	eCalibGenPram_ServoXValue13,
	eCalibGenPram_ServoXValue14,
	eCalibGenPram_ServoYMenu,
	eCalibGenPram_ServoYItem0,
	eCalibGenPram_ServoYItem1,
	eCalibGenPram_ServoYValue0,
	eCalibGenPram_ServoYValue1,
	eCalibGenPram_ServoYValue2,
	eCalibGenPram_ServoYValue3,
	eCalibGenPram_ServoYValue4,
	eCalibGenPram_ServoYValue5,
	eCalibGenPram_ServoYValue6,
	eCalibGenPram_ServoYValue7,
	eCalibGenPram_ServoYValue8,
	eCalibGenPram_ServoYValue9,
	ePlatFormX,
	eCalcNum_Visual,
	eCalcNum_Traject,
	eCalcNum_Trunion,
	eCalcNum_General,
	eCalcNum_Weather,
	eCalcNum_Turret,
	eCalcNum_AlgleD,
	eCalcNum_Posion,

	eStateMaintD,//ά����
	eStateDetend ,//= 0xC5,//,//����ֹ����
	eStateClutch,//�����
	eStateGrenad,//35������
	eStateFirer,//����
	eStateFulCAN,//ȫ��CAN
	eStateDisCAN0,//�Կ�CAN0
	eStateDisCAN1,//�Կ�CAN1
	eStateMagnet,//�����
	eStatePosMotor,//ˮƽ���
	eStateMacMotor,//58���
	eStateGreMotor,//35����

	eStateWeaponCtrlA1,
	eStateSrvAngle,
	eStateFulScrAngle,
	eLaserSelect,
	eAnglePosn,
	eAngleDipX,
	eAngleDipY,
	eAngleMach,
	eAngleGred,
	eGunTip,
	

#if 0
	eWeather1_T,
	eWeather1_pole,
	eweather1_tmperature1,
	eweather1_tmperature2,
	eweather1_p,
	eweather1_qiya1,
	eweather1_qiya2,
	eweather1_qiya3,
	eweather1_qiya4,

	eweather2_maohao,
	eweather2_pole,
	eweather2_offx_1,
	eweather2_offx_2,
	eweather2_offx_3,
	eweather2_offx_4,
	eweather2_offy_1,
	eweather2_offy_2,
	eweather2_offy_3,
	eweather2_offy_4,
#endif

	eBoreSightLinId,
	OSD_TEXT_SIZE
}eOsdId;


enum {
	wAuto=0x00,
	wAlert,
	wCalibration,
	wCalibZero,
	wCalibGeneral,
	wCalibWeather,
	wTrack,
	wCalibGenPram,
	wCalibHorizen,
	wCalibLaser
};
enum{
	CORRECTION_GQ,
	CORRECTION_RGQ,
	CORRECTION_NGQ
};
typedef enum _osd_sprit{
	eOsdText		=	0x00,
	eOsdSprit		=	0x01
}eOsdSpritType;

typedef enum _osd_state{
	eOsd_Hide	=0x00,
	eOsd_Disp	=0x01
}eOsdDisplayStat;

typedef enum _osd_up_state{
	eOsd_UnUpdate		=0x00,
	eOsd_Update		=0x01
}eOsdUpdate;

typedef enum _osd_text_color{
	eTransparent	=	0x00,	//͸��
	eBlack			=	0x01,	//��
	eWhite			=	0x02,	//��
	eCurCfg			=	0x03	//�������統ǰ��
}eOsdColor;


enum {
	LASERERR_TIMEOUT,
	LASERERR_NOECHO,
	LASERERR_NOSAMPLE,
	LASERERR_COUNT
};






struct _MuxPotrTran
{
	PortInter_t*mptPortInter[MAX_PORT_NUM];
	int(*mptRegister)(MuxPotrTran* pMux,UINT PortHandle);
	
	int(*mptSendFun)(MuxPotrTran * this_ptr,Enum_MuxPortType ePortType,BYTE *pData,int iLen);
	int(*mptRecvFun)(Enum_MuxPortType ePortType,BYTE*pBuf,int iLen);

	int(*mptFunSet)(Enum_MuxPortType ePortType, MuxPortCallBackFun pRtnEntry, int context); 
};



typedef struct _osd_text_obj{
	unsigned int osdId;
	volatile unsigned char osdState;
	unsigned char osdUpdate;
	unsigned char osdFColor;
	unsigned char osdBColor;
	unsigned char osdTextLen;
	unsigned short osdInitX;
	unsigned short osdInitY;
	unsigned short osdReg;
	char osdContext[MAX_CONTEXT_LEN];
}OSDText_Obj,*OSDText_Handle;

typedef struct _osd_sprit_line{
	unsigned char  osdState;
	unsigned char  osdUpdate;
	unsigned short osdInit;
	//AVT_SpriteData LineObj;
}OSDLine_Obj,*OSDLine_Handle;



typedef struct _osd_ctrl_obj{
	ModleCtrl_t Interface;
	SEM_ID			osdSem;
	FONT_OBJ*		pOsdFont;
	unsigned int	uSegId;
	unsigned int	uInit;

	unsigned int 	uRtlX;
	unsigned int 	uRtlY;
	unsigned int 	uRtlM;

	unsigned int	uTextNum;
	OSDText_Obj*	pTextList;
	OSDLine_Obj		pLineArray[OSD_LINE_NUM];
}OSDCTRL_OBJ,*OSDCTRL_Handle;



extern int Pressure,DistanceManual,Temparature;//������� ѹ�� ����
extern double AngleGun, AngleGrenade,PositionX,PositionY;
extern char MeasureErrOsd[3][8];
extern char SuperOsd[7][8];
extern char DynamicOsd[10][8];
extern char *Posd[OSD_TEXT_SIZE];
extern char ResultOsd[2][8];
extern char GunOsd[6][8];
extern char ErrorOsd[19][8];
extern bool SHINE;
extern int ShinId;
extern char AngleCorrectOsd[3][8];
extern char ShotOsd[2][8];
extern char AimOsd[2][8];
extern char FovTypeOsd[2][8];
extern char EnhanceOsd[2][8];
extern char SaveYesNoOsd[2][8];
extern char MeasureTypeOsd[6][8];
extern char CalibGeneralOsd[3][8];
extern char CalibZeroOsd[3][8];
extern int General[14];
extern int Weather[10];
extern int DisValue[4];
extern char ShotGunOsd[2][8];//���ģʽ:��ǹ
extern char LaserOsd[2][8];
extern char GenPramMenu[4][12];
extern char WorkOsd[10][8];	//����ģʽ



void OSDCTRL_AlgSelect();
void OSDCTRL_AllHide();
void OSDCTRL_BattleShow();
void OSDCTRL_CalibMenuShow();
void OSDCTRL_CalibSaveShow();
void OSDCTRL_CalibWeatherShow();
void OSDCTRL_CalibGeneralShow();
void OSDCTRL_CalibZeroShow();
void OSDCTRL_CalibHorizenShow();
void OSDCTRL_CalibLaserShow();
void OSDCTRL_BattleAlertShow();
void OSDCTRL_BattleTrackShow();
void OSDCTRL_CalibGenPramShow();
void OSDCTRL_ItemShow(LPARAM lParam);
void OSDCTRL_ItemHide(LPARAM lParam);
void OSDCTRL_updateAreaN();
bool OSDCTRL_IsOsdDisplay(LPARAM lParam);
void OSDCTRL_CalcNumShow(void);
void OSDCTRL_CalcNumHide(void);
void OSDCTRL_FulScrAngleShow();
void OSDCTRL_FulScrAngleHide();
void OSDCTRL_updateMainMenu(int i);


void OSDCTRL_EnterBattleMode();

OSDCTRL_Handle OSDCTRL_create();//(MuxPotrTran * pMuxPortTran,HANDLE hAvtDev,HANDLE hTimerCtrl,UINT segId,UINT nBytes);
void OSDCTRL_destroy(HANDLE hOsdCtrl);

//interface
int OSDCTRL_initText(HANDLE hOsdCtrl,Mat frame);
int OSDCTRL_genOsdContext(HANDLE hOsdCtrl,UINT uItemId);
int OSDCTRL_setOsdDisplay(HANDLE hOsdCtrl,UINT uShow);
int OSDCTRL_setOsdContrast(HANDLE hOsdCtrl,unsigned char fColor,unsigned char bColor);

//private function
int OSDCTRL_setTextState(HANDLE hOsdCtrl,UINT uItemId, UINT uStat);
static int OSDCTRL_setTextColor(HANDLE hOsdCtrl,UINT uItemId,unsigned char fColor,unsigned char bColor);

//message process functions
void OSDCTRL_shinOsd(LPARAM lParam);
void OSDCTRL_hideOsd(LPARAM lParam);

void OSDCTRL_lpfnTimer(void* cbParam);

void OSDCTRL_draw(Mat frame,OSDCTRL_Handle pCtrlObj);

void OSDCTRL_sendSem(OSDCTRL_OBJ * pCtrlObj);
void OSDCTRL_TskDrawOsd();
void OSDCTRL_EnterCalibMode();
void OSDCTRL_CheckResultsShow();
void OSDCTRL_ItemShine(int Id);
void OSDCTRL_NoShine();
void OSDCTRL_NoShineShow();
void Osd_shinItem(int id);
int getCrossX();
int getCrossY();

void resetTickNum();
void increaseFireViewPram();
void increaseFireCtrlPram();
void increaseServoXPram();
void increaseServoYPram();

void moveLeftServoXPram();
void moveLeftServoYPram();
void isBeyondServoX(int id);
void moveRightServoXPram();
void isBeyondServoY(int id);
void moveRightServoYPram();



extern volatile char globalShoweErrorZone;
extern WeatherItem gWeatherTable;




#ifdef __cplusplus
}
#endif

#endif

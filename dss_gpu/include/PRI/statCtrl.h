#ifndef STATECTRL_H
#define STATECTRL_H
#ifdef __cplusplus
extern "C" {
#endif
#include "permanentStorage.h"


#define FALSE	0
#define TRUE 	1
//#define BOOL		bool

#define RED 			0xFFFF0000
#define GREEN 		0xFF00FF00
#define BLUE 		0xFF0000FF
#define YELLO 		0xFFFFFF00
#define BLACKCOLOR	0xFF000000
#define WHITECOLOR	0xFFFFFFFF
#define BGCOLOR 	0x00000000

#define FOVDEGREE_VLARGE 6		//8
#define FOVDEGREE_HLARGE 4.5	//6
#define FOVDEGREE_VSMALL 2.15
#define FOVDEGREE_HSMALL 1.65





typedef enum Level_one_state{
	MODE_BOOT_UP 	= 0x0,
	MODE_BATTLE,
	MODE_CALIBRATION
} Level_one_state;


typedef enum Level_two_state{
	STATE_BOOT_UP_SELF_CHECK_FINISH,
	STATE_BOOT_UP_SELF_CHECK,
	STATE_BATTLE_AUTO,
	STATE_BATTLE_ALERT,
	STATE_CALIBRATION_MAIN_MENU,
	STATE_CALIBRATION_ZERO,
	STATE_CALIBRATION_SAVE,
	STATE_CALIBRATION_GENERAL,
	STATE_CALIBRATION_WEATHER,
	STATE_CALIBRATION_GENPRAM,
	STATE_CALIBRATION_HORIZEN,
	STATE_CALIBRATION_LASER
} Level_two_state;

typedef enum Level_three_state{
	Battle_Idle,
	Battle_TriggerMeasureVelocity,
	Battle_TriggerLaser_TriggerAVT,
	Battle_Preparation,
	Battle_LoadFiringTable,
	Battle_Ready,
	
	Auto_Idle,
	Auto_TriggerMeasureVelocity,
	Auto_TriggerLaser,
	Auto_Preparation,
	Auto_LoadFiringTable,
	Auto_Ready,

	Menu_FireView,
	Menu_FireCtrl,
	Menu_ServoX,
	Menu_ServoY
	
}Level_three_state;


typedef enum Level_four_state{
	WaitForFeedBack,
	Laser_OK,
	TrackingOK,
	FiringInputReady,
	FiringTableCalculated,
	FiringTableLoading
	
}Level_four_state;


typedef enum{
	MEASURETYPE_LASER = 0x00,
	MEASURETYPE_MANUAL
}DIS_MEASURE_TYPE;


typedef enum{
	SHOTTYPE_SHORT,
	SHOTTYPE_LONG
}SHOT_TYPE;


typedef enum _Derection{
	DERECTION_LEFT,
	DERECTION_RIGHT,
	DERECTION_DOWN,
	DERECTION_NULL
}Derection_Type;

/*
typedef enum {
	PROJECTILE_BULLET = 0x00,
	PROJECTILE_GRENADE_KILL,
	PROJECTILE_GRENADE_GAS,
	QUESTION_GRENADE_KILL,
	QUESTION_GRENADE_GAS
}PROJECTILE_TYPE;
*/

typedef enum Dram_FovStat{
	eFov_LarFov_Stat	=0x00,
	eFov_MidFov_Stat	=0x01,
	eFov_SmlFov_Stat	=0x02,
	eFov_ZomFov_Stat	=0x03
}eFovStat;




/*************************TIMER*****************************/
typedef enum _time_id
{
	eOSD_Timer = 0x10,
	eCap_Timer ,
	eTrk_Timer ,
	eCST_Timer ,
	eBootUp_Timer,
	eLaser_Timer,
	eRGQ_Timer,
	eAVT_Timer,
	eCAN_Timer,
	eDynamic_Timer,
	eSchedule_Timer,
	eF6_Timer,
	eF5_Timer,
	eF3_Timer,
	eF2_Timer,
	eFxbutton_Timer,
	eDipAngle_Timer,
	eOSD_shine_Timer	,					// the max of the timer number  is 0x20
	ePosAngle_Timer,
	eMachGunAngle_Timer,
	eGrenadeAngle_Timer,
	ePosServo_Timer,
	eMachGunServo_Timer,
	eGrenadeServo_Timer,
	eServoCheck_Timer,
	eWeaponCtrl_Timer,
	eSelftoAuto_timer,
	MAX_TIMER_NUM
}eTimerId;


#define 	FILR_TIMEOUT 			2000
#define 	SELCAP_TIMER 			250
#define 	PICP_TIMER   			9
#define 	TRACK_TIMER  			200
#define 	TRACK_REFINE_TIMER 	2000
#define 	BOOTUP_TIMER 			2000
#define 	LASER_TIMER 			6000
#define 	RGQ_TIMER 				6000//3000
#define 	CAN_TIMER 				500
#define 	DYNAMIC_TIMER 		6000
#define 	AVT_TIMER 				2000
#define 	SCHEDULE_TIMER 		500
#define 	FN_TIMER 				2000
#define 	SELFCHECK_TIMER 		1000
#define 	SERVOCHECK_TIMER 		500

typedef enum timer_stat
{
	eTimer_Stat_Stop 	= 0x00,
	eTimer_Stat_Run	= 0x01
}eTimerStat;


typedef struct 
{
	volatile unsigned int nStat;
	//volatile unsigned int nIDEvent;
	//volatile unsigned int nElapse;
	//volatile unsigned int nClockCout;
	//void * pParam;
	//void (* lpfnTimer)(void *);
}CDTime;

typedef struct 
{
	CDTime pTimeArray[MAX_TIMER_NUM];
	unsigned int timNum;
	int (*startTimer)( unsigned int timerId, unsigned int nMs );
	int (*KillTimer)( unsigned int  timerId );
	int  (*GetTimerStat)(unsigned int  nIDEvent);
	int (*ReSetTimer)(unsigned int timerId, unsigned int nMs);
	//int (*SetTimer)( unsigned int timerId, unsigned int nMs );
	//int  (*InitTimerCtrl)();
	//void (*DestroyTimerCtrl)();
	//void (*RunTimer)();
	
}CTimerCtrl;


/**************************************END************************************/



SHOT_TYPE getGunShotType();
SHOT_TYPE getShotType();

bool isMeasureOsdNormal();
bool isMeasureManual();
bool isCalibrationSave();
bool isCalibrationMainMenu();
bool isCalibrationGeneral();
bool isCalibrationWeather();
bool isCalibrationZero();
bool isCalibrationGenPram();
bool isCalibrationHorizen();
bool isCalibrationLaser();
bool isBattleMode();
bool isCalibrationMode();
bool isBootUpMode();
bool isGrenadeGas();
bool isBootUpSelfCheck();
bool bWeaponCtrlOK();
bool Is9stateOK();
bool bJoyStickOK();
bool bPositionSensorOK();
bool bPositionServoOK();
bool bDipAngleSensorOK();
bool isAllSensorOK();
bool isAllServoOK();
bool bMachineGunSensorOK();
bool bGrenadeSensorOK();
bool bGrenadeServoOK();
bool bMachineGunServoOK();
bool isBootUpSelfCheckFinished();
bool isValidProjectileType();
bool isStatBattleAuto();
bool isAutoReady();
bool isBattleReady();
void enterLevel3CalculatorIdle();
bool isAtSaveYes();
bool isMachineGun();
bool isGrenadeKill();
bool isBlackColor();
bool isStatBattleAlert();
bool isBattleLoadFiringTable();
bool isAutoLoadFiringTable();
bool isFovMachineGun();
bool isFovGrenadeGas();
bool isFovGrenadeKill();
bool isFovSmall();
bool isBattleIdle();
bool isAutoIdle();
bool bDetendClose();
bool bMaintPortOpen();
int getErrCodeId();
bool isWaitforFeedback();
bool isTrackingOK();
bool isBattlePreparation();
bool isAutoPreparation();
void loadFiringTable_Enter();
void loadFiringTable();
bool isLaserOK();
bool isBattleTriggerMeasureVelocity();
bool isAutoTriggerMeasureVelocity();
void setServoAvailable(bool avail);
bool isBeyondDistance();
bool isMachineGunBefore();
bool isGrenadeKillBefore();
bool isGrenadeGasBefore();
void EnterCMD_BULLET_SWITCH1();
void EnterCMD_BULLET_SWITCH2();
void EnterCMD_BULLET_SWITCH3();
bool isAutoCatching();
bool isFovShine();
void EnterCMD_BULLET_SWITCH(int i);

PROJECTILE_TYPE getFovProjectileType();
PROJECTILE_TYPE getProjectileType();
DIS_MEASURE_TYPE getMeasureType();


int  CTIMERCTRL_initTimerCtrl();
void startDynamicTimer();
void DynamicTimer_cbFxn();
int CTIMERCTRL_getTimerStat(unsigned int nIDEvent);
void killDynamicTimer();
void processCMD_FIRING_TABLE_LOAD_OK(long lParam);
void processCMD_FIRING_TABLE_FAILURE(long lParam);

int getAdvancedMenuIndex();
void decreaseAdvancedMenu();
void changeSaveOption();
void increaseAdvancedMenu();
void processCMD_MAINTPORT_LOCK(long lParam);
void processCMD_MAINTPORT_UNLOCK(long lParam);
void processCMD_DETEND_LOCK(long lParam);
void processCMD_DETEND_UNLOCK(long lParam);
void startSelfCheckTimer();
float getAimOffsetX();
float getAimOffsetY();
bool isBeyondDerection(Derection_Type type);
void processCMD_MODE_ATTACK_SIGLE(long lParam);
void processCMD_MODE_ATTACK_MULTI(long lParam);
void processCMD_SCHEDULE_GUN(long lParam);
void SCHEDULE_cbFxn(void* cbParam);
void startSCHEDULEtimer();
void killSCHEDULEtimer();
void startRGQtimer();
void killRGQtimer();
void processCMD_MIDPARAMS_SWITCH(long lParam);
void processCMD_LASERSELECT_SWITCH(long lParam);
void processCMD_SENSOR_SWITCH(long lParam);
void processCMD_CONNECT_SWITCH(long lParam);
void SelfCheckVertoAuto_cbFxn(void* cbParam);
void killSelfCheckVertoAutotimer();
void startSelfCheckVertoAuto();

extern Level_one_state gLevel1Mode,gLevel1LastMode;
extern Level_two_state gLevel2CalibrationState,gLevel2BootUpState,gLevel2BattleState;
extern Level_three_state gLevel3CalibrationState,gLevel3CalculatorState;
extern Level_four_state  gLevel4subCalculatorState;
extern DIS_MEASURE_TYPE gMeasureType;
extern bool isfixingMeasure,isJoyStickOK,isDipAngleSensorOK,isMachineGunSensorOK,isGrenadeSensorOK;	
extern bool isWeaponCtrlOK,isPositionSensorOK,isGrenadeServoOK,isMachineGunServoOK,isPositionServoOK;
extern PROJECTILE_TYPE gProjectileType,gProjectileTypeBefore;
extern SHOT_TYPE gGunShotType;
extern bool bUnlock,AUTOCATCH,isDetendClose;
extern volatile bool finish_laser_measure;
extern WeatherItem gWeatherTable;
extern int BackColor,COUNTER;
extern bool SCHEDULE_GUN,SCHEDULE_RESET,SCHEDULE_STRONG;
extern CTimerCtrl * pTimerObj;


#ifdef __cplusplus
	}
#endif
#endif


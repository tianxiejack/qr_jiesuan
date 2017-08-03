#ifndef STATECTRL_H
#define STATECTRL_H
#ifdef __cplusplus
extern "C" {
#endif
//#include "app_global.h"


#define FALSE	0
#define TRUE 	1
//#define BOOL		bool

#define RED 			0xFFFF0000
#define GREEN 		0xFF00FF00
#define BLUE 		0xFF0000FF
#define YELLO 		0xFFFFFF00
#define BLACKCOLOR	0xFF000000
#define WHITECOLOR	0xFFFFFFFF
#define BGCOLOR 		0x00000000

#define FOVDEGREE_VLARGE 8
#define FOVDEGREE_HLARGE 6
#define FOVDEGREE_VSMALL 2.8
#define FOVDEGREE_HSMALL 2.1





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
	DERECTION_DOWN
}Derection_Type;

typedef enum {
	PROJECTILE_BULLET = 0x00,
	PROJECTILE_GRENADE_KILL,
	PROJECTILE_GRENADE_GAS,
	QUESTION_GRENADE_KILL,
	QUESTION_GRENADE_GAS
}PROJECTILE_TYPE;

typedef enum Dram_FovStat{
	eFov_LarFov_Stat	=0x00,
	eFov_MidFov_Stat	=0x01,
	eFov_SmlFov_Stat	=0x02,
	eFov_ZomFov_Stat	=0x03
}eFovStat;



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


PROJECTILE_TYPE getFovProjectileType();
PROJECTILE_TYPE getProjectileType();

extern Level_one_state gLevel1Mode,gLevel1LastMode;
extern Level_two_state gLevel2CalibrationState,gLevel2BootUpState,gLevel2BattleState;
extern Level_three_state gLevel3CalibrationState,gLevel3CalculatorState;
extern DIS_MEASURE_TYPE gMeasureType;
extern bool isfixingMeasure,isJoyStickOK,isDipAngleSensorOK,isMachineGunSensorOK,isGrenadeSensorOK;	
extern bool isWeaponCtrlOK,isPositionSensorOK,isGrenadeServoOK,isMachineGunServoOK,isPositionServoOK;
extern bool bUnlock;
extern PROJECTILE_TYPE gProjectileType,gProjectileTypeBefore;
extern SHOT_TYPE gGunShotType;//gShotType
extern bool AUTOCATCH;


#ifdef __cplusplus
	}
#endif
#endif


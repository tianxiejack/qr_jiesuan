
#include "statCtrl.h"
#include "osdProcess.h"


float FOVSIZE_V=FOVDEGREE_VLARGE, FOVSIZE_H=FOVDEGREE_HLARGE;
Level_one_state gLevel1Mode = MODE_BOOT_UP,gLevel1LastMode = MODE_BATTLE;
Level_two_state gLevel2BattleState = STATE_BATTLE_AUTO, gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU,
						gLevel2BootUpState = STATE_BOOT_UP_SELF_CHECK;
Level_three_state gLevel3CalculatorState = Auto_Idle,gLevel3CalibrationState   = Menu_FireView;
static Level_four_state  gLevel4subCalculatorState = WaitForFeedBack;
PROJECTILE_TYPE gProjectileType=PROJECTILE_BULLET;
PROJECTILE_TYPE gProjectileTypeBefore=PROJECTILE_BULLET;
int BackColor=WHITECOLOR;
DIS_MEASURE_TYPE gMeasureType=MEASURETYPE_LASER;
SHOT_TYPE gShotType=SHOTTYPE_SHORT,gGunShotType=SHOTTYPE_SHORT;
bool isWeaponCtrlOK=FALSE,isPositionSensorOK=FALSE, isPositionServoOK=FALSE;
bool isJoyStickOK=FALSE,isDipAngleSensorOK=FALSE,isMachineGunSensorOK=FALSE;
bool isGrenadeSensorOK=FALSE,isGrenadeServoOK=FALSE,isMachineGunServoOK=FALSE;
static bool isVideoOK=FALSE;
static bool isDetendClose=TRUE,isMaintPortOpen=TRUE;
static bool FOVSHINE = FALSE;
Derection_Type SHINE_DERECTION=DERECTION_DOWN;
bool isfixingMeasure = FALSE;
extern void setJoyStickStat(BOOL stat);
extern void setServoControlObj();
extern void setPicEnhance(BOOL context);
extern WeatherItem gWeatherTable;
//extern PIF_servo_control pServoControlObj;
extern volatile  int vpstatus;
extern void setJoyStickStat(bool stat);

static double AimOffsetX=0.000,AimOffsetY=0.000;
static bool gTrackingMode=FALSE;
//servo control related
static bool bServoAavailable = FALSE;
bool bUnlock = TRUE;
bool AUTOCATCH = FALSE;

SHOT_TYPE getGunShotType()
{
	return gGunShotType;
}

SHOT_TYPE getShotType()
{
	return gShotType;
}

bool isBattleIdle()
{
	return Battle_Idle == gLevel3CalculatorState;
}

bool isAutoIdle()
{
	return Auto_Idle == gLevel3CalculatorState;
}


bool isMeasureOsdNormal()
{
	return (Posd[eMeasureType]==MeasureTypeOsd[0])||(Posd[eMeasureType]==MeasureTypeOsd[1]);
}


bool isBattleMode()
{
	return MODE_BATTLE == gLevel1Mode;
}

bool isCalibrationMode()
{
	return MODE_CALIBRATION == gLevel1Mode;
}

bool isBootUpMode()
{
	return MODE_BOOT_UP == gLevel1Mode;
}

bool isBootUpSelfCheck()
{
	return STATE_BOOT_UP_SELF_CHECK == gLevel2BootUpState;
}

bool isBlackColor()
{
	return BLACKCOLOR==BackColor;
}

bool isStatBattleAuto()
{
	return STATE_BATTLE_AUTO == gLevel2BattleState;
}

bool isStatBattleAlert()
{
	return STATE_BATTLE_ALERT == gLevel2BattleState;
}

bool isGrenadeGas()
{
	return PROJECTILE_GRENADE_GAS == getProjectileType();
}

bool isAutoLoadFiringTable()
{
	return Auto_LoadFiringTable == gLevel3CalculatorState;
}


bool isBattleLoadFiringTable()
{
	return Battle_LoadFiringTable == gLevel3CalculatorState; 
}



bool isGrenadeKill()
{
	return PROJECTILE_GRENADE_KILL == getProjectileType();
}


bool isMachineGun()
{
	return PROJECTILE_BULLET == getProjectileType();
}

bool bWeaponCtrlOK()
{
	isWeaponCtrlOK =1 ;
	return isWeaponCtrlOK;
}

bool isMeasureManual()
{
	return MEASURETYPE_MANUAL == gMeasureType;
}


bool isCalibrationSave()
{
	return STATE_CALIBRATION_SAVE == gLevel2CalibrationState;
}

bool isCalibrationGenPram()
{
	return STATE_CALIBRATION_GENPRAM == gLevel2CalibrationState;
}

bool isCalibrationHorizen()
{
	return STATE_CALIBRATION_HORIZEN == gLevel2CalibrationState;
}

bool isCalibrationLaser()
{
	return STATE_CALIBRATION_LASER== gLevel2CalibrationState;
}


bool isCalibrationMainMenu()
{
	return STATE_CALIBRATION_MAIN_MENU == gLevel2CalibrationState;
}

bool isCalibrationGeneral()
{
	return STATE_CALIBRATION_GENERAL == gLevel2CalibrationState;
}

bool isCalibrationWeather()
{
	return STATE_CALIBRATION_WEATHER == gLevel2CalibrationState;
}

bool isCalibrationZero()
{
	return STATE_CALIBRATION_ZERO == gLevel2CalibrationState;
}

bool isAtSaveYes()
{
	return Posd[eSaveYesNo] == SaveYesNoOsd[0];
}

bool bJoyStickOK()
{
	return isJoyStickOK;
}

bool bDipAngleSensorOK()
{
	return isDipAngleSensorOK;
}


bool bPositionServoOK()
{
	isPositionServoOK = 1;
	return isPositionServoOK;
}

bool bPositionSensorOK()
{
	return isPositionSensorOK ;
}

bool bGrenadeServoOK()
{
	isGrenadeServoOK = 1;
	return isGrenadeServoOK;
}

bool bDetendClose()
{
	return isDetendClose;
}

bool bMaintPortOpen()
{
	return isMaintPortOpen;
}

bool bMachineGunServoOK()
{
	isMachineGunServoOK = 1;
	return isMachineGunServoOK;
}

void enterLevel3CalculatorIdle()
{
	gLevel3CalculatorState = Battle_Idle;
//	releaseServoContrl();
}

bool isBootUpSelfCheckFinished()
{
	return STATE_BOOT_UP_SELF_CHECK_FINISH == gLevel2BootUpState;
}

bool isAutoReady()
{
	return Auto_Ready == gLevel3CalculatorState;
}

bool isBattleReady()
{
	return Battle_Ready == gLevel3CalculatorState;
}

bool isValidProjectileType()
{
	return (gProjectileType <= PROJECTILE_GRENADE_GAS);
}

bool isFovMachineGun()
{
	return PROJECTILE_BULLET == getFovProjectileType();
}

bool Is9stateOK()
{
	bool ret = isWeaponCtrlOK &&isJoyStickOK &&isPositionSensorOK &&isPositionServoOK \
			&&isDipAngleSensorOK &&isMachineGunSensorOK &&isGrenadeSensorOK \
			&&isGrenadeServoOK &&isMachineGunServoOK;

	return ret;
}

bool bGrenadeSensorOK()
{
	return isGrenadeSensorOK;
}

bool isAllSensorOK()
{
	return isPositionSensorOK &&isDipAngleSensorOK &&isMachineGunSensorOK &&isGrenadeSensorOK;
}

bool isAllServoOK()
{
	return isPositionServoOK &&isGrenadeServoOK &&isMachineGunServoOK;
}

bool bMachineGunSensorOK()
{
	return isMachineGunSensorOK;
}

bool isFovGrenadeGas()
{
	return PROJECTILE_GRENADE_GAS == getFovProjectileType();
}

bool isFovGrenadeKill()
{
	return PROJECTILE_GRENADE_KILL == getFovProjectileType();
}

bool isFovSmall()
{
	return (Posd[eFovType] == FovTypeOsd[1]);
}



PROJECTILE_TYPE getFovProjectileType()
{
	if(gProjectileType > PROJECTILE_GRENADE_GAS)
	{
		return gProjectileTypeBefore;
	}else
		return gProjectileType;
	
}

PROJECTILE_TYPE getProjectileType()
{
	if(gProjectileType > PROJECTILE_GRENADE_GAS)
	{
		return PROJECTILE_BULLET;
	}
	else
		return gProjectileType;	
}


int getErrCodeId()
{
	//isWeaponCtrlOK = isCapStatOK();
	if((!isWeaponCtrlOK)&&(!isJoyStickOK)&&(!isAllSensorOK())&&(!isAllServoOK()))
		return 14;
	else if((!isJoyStickOK)&&(!isAllSensorOK())&&(!isAllServoOK()))
		return 13;
	else if((!isWeaponCtrlOK)&&(!isAllSensorOK())&&(!isAllServoOK()))
		return 12;
	else if((!isWeaponCtrlOK)&&(!isJoyStickOK)&&(!isAllServoOK()))
		return 11;
	else if((!isWeaponCtrlOK)&&(!isJoyStickOK)&&(!isAllSensorOK()))
		return 10;
	else if((!isAllSensorOK())&&(!isAllServoOK()))
		return 9;
	else if((!isJoyStickOK)&&(!isAllServoOK()))
		return 8;
	else if((!isJoyStickOK)&&(!isAllSensorOK()))
		return 7;
	else if((!isWeaponCtrlOK)&&(!isAllServoOK()))
		return 6;
	else if((!isWeaponCtrlOK)&&(!isAllSensorOK()))
		return 5;
	else if((!isWeaponCtrlOK)&&(!isJoyStickOK))
		return 4;
	else if(!isAllServoOK())
		return 3;
	else if(!isAllSensorOK())
		return 2;
	else if(!isJoyStickOK)
		return 1;
	else if(!isWeaponCtrlOK)
		return 0;
	else
		return 15;
}





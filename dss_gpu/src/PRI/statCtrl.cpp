
#include "statCtrl.h"
#include "osdProcess.h"
#include "firingCtrl.h"

#include "WeaponCtrl.h"
#include "GrenadePort.h"
#include "LaserPort.h"
#include "MachGunPort.h"
#include "TurretPosPort.h"
#include "PositionPort.h"

#include "osdPort.h"
#include "servo_control_obj.h"
#include "spiH.h"
#include "dx.h"
#include "osdProcess.h"

float FOVSIZE_V=FOVDEGREE_VLARGE, FOVSIZE_H=FOVDEGREE_HLARGE;
Level_one_state gLevel1Mode = MODE_BOOT_UP,gLevel1LastMode = MODE_BATTLE;
Level_two_state gLevel2BattleState = STATE_BATTLE_AUTO, gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU,
						gLevel2BootUpState = STATE_BOOT_UP_SELF_CHECK;
Level_three_state gLevel3CalculatorState = Auto_Idle,gLevel3CalibrationState   = Menu_FireView;
Level_four_state  gLevel4subCalculatorState = WaitForFeedBack;
PROJECTILE_TYPE gProjectileType=PROJECTILE_BULLET;
PROJECTILE_TYPE gProjectileTypeBefore=PROJECTILE_BULLET;
int BackColor=WHITECOLOR;
DIS_MEASURE_TYPE gMeasureType=MEASURETYPE_LASER;
SHOT_TYPE gShotType=SHOTTYPE_SHORT,gGunShotType=SHOTTYPE_SHORT;
bool isWeaponCtrlOK=FALSE,isPositionSensorOK=FALSE, isPositionServoOK=FALSE;
bool isJoyStickOK=FALSE,isDipAngleSensorOK=FALSE,isMachineGunSensorOK=FALSE;
bool isGrenadeSensorOK=FALSE,isGrenadeServoOK=FALSE,isMachineGunServoOK=FALSE;
bool isDetendClose=TRUE,isMaintPortOpen=TRUE;
static bool isVideoOK=FALSE;
static bool FOVSHINE = FALSE;
bool SCHEDULE_GUN=FALSE,SCHEDULE_RESET=FALSE,SCHEDULE_STRONG=FALSE;
Derection_Type SHINE_DERECTION=DERECTION_NULL;//DERECTION_DOWN;
bool isfixingMeasure = FALSE;
int COUNTER=0;

static bool MIDPARAMS=FALSE;
static bool BASEMENU=FALSE;
static bool CONNECT=FALSE;
static bool ADCalibMenu=FALSE;

extern void setJoyStickStat(BOOL stat);
extern void setServoControlObj();
extern void setPicEnhance(BOOL context);
extern WeatherItem gWeatherTable;
//extern PIF_servo_control pServoControlObj;
extern volatile  int vpstatus;
extern void setJoyStickStat(bool stat);
extern bool gProjectileMachFovlast;
extern bool gProjectileGreFovlast;

static double AimOffsetX=0.000,AimOffsetY=0.000;
static bool gTrackingMode=FALSE;
//servo control related
static bool bServoAavailable = FALSE;
bool bUnlock = TRUE;
bool AUTOCATCH = FALSE;
volatile bool finish_laser_measure = 0;


CTimerCtrl * pTimerObj = (CTimerCtrl*)OSA_memAlloc(sizeof(CTimerCtrl));

DIS_MEASURE_TYPE getMeasureType()
{
	return gMeasureType;
}

SHOT_TYPE getGunShotType()
{
	return gGunShotType;
}

SHOT_TYPE getShotType()
{
	return gShotType;
}

bool isAutoCatching()
{
	return AUTOCATCH;
}

bool isBattleIdle()
{
	return Battle_Idle == gLevel3CalculatorState;
}

bool isAutoIdle()
{
	return Auto_Idle == gLevel3CalculatorState;
}

bool isAutoPreparation()
{
	return Auto_Preparation == gLevel3CalculatorState;
}

bool isBattlePreparation()
{
	return Battle_Preparation == gLevel3CalculatorState;
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

bool isLaserOK()
{
	return Laser_OK == gLevel4subCalculatorState;
}

bool isAutoLoadFiringTable()
{
	return Auto_LoadFiringTable == gLevel3CalculatorState;
}


bool isBattleLoadFiringTable()
{
	return Battle_LoadFiringTable == gLevel3CalculatorState; 
}

bool isBattleTriggerMeasureVelocity()
{
	return Battle_TriggerMeasureVelocity == gLevel3CalculatorState;
}

bool isAutoTriggerMeasureVelocity()
{
	return Auto_TriggerMeasureVelocity == gLevel3CalculatorState;
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
	//isPositionServoOK = 1;
	return isPositionServoOK;
}

bool isGrenadeKillBefore()
{
	return PROJECTILE_GRENADE_KILL == gProjectileTypeBefore;
}

bool isMachineGunBefore()
{
	return PROJECTILE_BULLET == gProjectileTypeBefore;
}

bool bPositionSensorOK()
{
	return isPositionSensorOK ;
}

bool isFovShine()
{
	return FOVSHINE;
}

bool bGrenadeServoOK()
{
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
	//isMachineGunServoOK = 1;
	return isMachineGunServoOK;
}

bool isWaitforFeedback()
{
	return WaitForFeedBack == gLevel4subCalculatorState;
}

bool isTrackingOK()
{
	return TrackingOK == gLevel4subCalculatorState;
}

void enterLevel3CalculatorIdle()
{
	gLevel3CalculatorState = Battle_Idle;
}

void setServoAvailable(bool avail)
{
	bServoAavailable = avail;
	if(avail)
	{
		Posd[eDynamicZone] = DynamicOsd[4];
		OSDCTRL_ItemShow(eDynamicZone);
	}
	else
	{
		OSDCTRL_ItemHide(eDynamicZone);
	}
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


bool isGrenadeGasBefore()
{
	return PROJECTILE_GRENADE_GAS == gProjectileTypeBefore;
}


bool Is9stateOK()
{
	bool ret = isWeaponCtrlOK &&isJoyStickOK &&isPositionSensorOK &&isPositionServoOK \
			&&isDipAngleSensorOK &&isMachineGunSensorOK &&isGrenadeSensorOK \
			&&isGrenadeServoOK &&isMachineGunServoOK;

	if(ret)
		uart_open_close_flag = 0;
	else
		uart_open_close_flag = 1;
	
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

bool isBeyondDistance()
{
	if(isMachineGun())
	{
		if(DistanceManual>1500)
			return TRUE;
	}
	else if(isGrenadeKill())
	{
		if(DistanceManual>1700)
			return TRUE;
	}
	else if(isGrenadeGas())
	{
		if(DistanceManual>360)
			return TRUE;
	}
	
	return FALSE;
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

void output_prm_print(FiringInputs input,FiringOutputs output)
{	
	printf("-----------------------------input--------------------------------------\n");
	putchar(10);
	printf("PlatformXTheta = %f\n",input.PlatformXTheta);
	printf("PlatformYTheta = %f\n",input.PlatformYTheta);
	printf("MachineGunYTheta = %f\n",input.MachineGunYTheta);
	printf("GrenadeYTheta = %f\n",input.GrenadeYTheta);
	printf("TurretDirectionTheta = %f\n",input.TurretDirectionTheta);
	printf("TargetAngularVelocityX = %f\n",input.TargetAngularVelocityX);
	printf("TargetAngularVelocityY = %f\n",input.TargetAngularVelocityY);
	printf("TargetDistance = %f\n",input.TargetDistance);
	printf("Temperature = %f\n",input.Temperature);
	printf("AirPressure = %f\n",input.AirPressure);
	printf("DipAngle = %f\n",input.DipAngle);
	putchar(10);
	putchar(10);
	printf("-----------------------------output--------------------------------------\n");
	printf("AimElevationAngle = %f\n",output.AimElevationAngle);
	printf("BiasAngle = %f\n",output.BiasAngle);
	printf("AimOffsetThetaX = %f\n",output.AimOffsetThetaX);
	printf("AimOffsetThetaY = %f\n",output.AimOffsetThetaY);
	printf("AimOffsetX = %f\n",output.AimOffsetX);
	printf("AimOffsetY = %f\n",output.AimOffsetY);
	printf("correctionData.deltaX = %f\n",output.correctionData.deltaX);
	printf("correctionData.deltaY = %f\n",output.correctionData.deltaY);	
	putchar(10);
	printf("------------------------------------------------------------------------\n");
}


void loadFiringTable_Enter()
{
	int ret;
	FiringInputs input;
	FiringOutputs output;
	memset(&input,0,sizeof(input));
	memset(&output,0,sizeof(output));
	input.AirPressure = gWeatherTable.Pressure;
	input.GrenadeYTheta = DEGREE2MIL(getGrenadeAngle());
	input.MachineGunYTheta = DEGREE2MIL(getMachGunAngle());
	input.PlatformXTheta = DEGREE2MIL(getPlatformPositionX());
	input.PlatformYTheta = DEGREE2MIL(getPlatformPositionY());
	input.ProjectileType = getProjectileType();

	ret = getAverageVelocity(&input.TargetAngularVelocityX);
	if(ret < 0)
		input.TargetAngularVelocityX = 0.0;
//printf("TargetAngularVelocityX = %f \n",input.TargetAngularVelocityX);
	ret = getAverageDipVelocity(&input.TargetAngularVelocityY);
	if(ret < 0)
		input.TargetAngularVelocityY = 0.0;
	{
//		input.TargetAngularVelocityX = 0.0;
//		input.TargetAngularVelocityY = 0.0;
		if(!isMeasureManual())
			input.TargetDistance = getLaserDistance();
		else
		{
			//printf("!!!!!!!!!!!!!!!!input.TargetDistance = DistanceManual\n");
			input.TargetDistance = DistanceManual;
		}
	}	
	assert(input.TargetDistance >= 0);
/*	if(0 == input.TargetDistance){
		sendCommand(CMD_FIRING_TABLE_FAILURE);
		return ;
	}*/
	if(0 == input.TargetDistance)
		input.TargetDistance = 1;
	

	input.Temperature = gWeatherTable.Temparature;
	
	input.TurretDirectionTheta = DEGREE2MIL(getTurretTheta());

	printf("\n--------------------------INPUT-----------------------------------\n");
	printf("PlatformXTheta = %f\n",(input.PlatformXTheta));
	printf("PlatformYTheta = %f\n",(input.PlatformYTheta));
	printf("MachineGunYTheta = %f\n",(input.MachineGunYTheta));
	printf("GrenadeYTheta = %f\n",(input.GrenadeYTheta));
	printf("TargetDistance = %f\n",(input.TargetDistance));
	printf("Temperature = %f\n",(input.Temperature));
	printf("AirPressure = %f\n",input.AirPressure);
	printf("TargetAngularVelocityX = %f\n",(input.TargetAngularVelocityX));
	printf("TurretDirectionTheta = %f\n",(input.TurretDirectionTheta));
	printf("TargetAngularVelocityY = %f\n",(input.TargetAngularVelocityY));
	//printf("DipAngle = %f\n",input.DipAngle);
	printf("\n------------------------------------------------------------------\n");

	ret = FiringCtrl(&input, &output);
	//output_prm_print(input, output);
	printf("\n--------------------------OUTPUT-----------------------------------\n");
	printf("AimOffsetThetaX = %f\n",output.AimOffsetThetaX);
	printf("AimOffsetThetaY = %f\n",output.AimOffsetThetaY);
	printf("AimOffsetX = %d\n",output.AimOffsetX);
	printf("AimOffsetY = %d\n",output.AimOffsetY);

	printf("\n------------------------------------------------------------------\n");
	
	if(PROJECTILE_GRENADE_KILL == input.ProjectileType || PROJECTILE_GRENADE_GAS== input.ProjectileType)
	{
		setGrenadeDestTheta(MIL2DEGREE(output.AimOffsetThetaY) + getMachGunAngle());
	}
	AimOffsetX = (double)output.AimOffsetX;
	AimOffsetY = (double)output.AimOffsetY;

	if(CALC_OK == ret)
	{
		int borX=0,borY=0;
		pTimerObj->startTimer(eRGQ_Timer,RGQ_TIMER);
		
		//todo ValidateOutput(&output);//check offset overflow. 5.6x4.2 16x12

		if(isMachineGun())
		{
			borX = gMachineGun_ZCTable.data.deltaX;
			borY = gMachineGun_ZCTable.data.deltaY;
		}
		else
		{
			borX = gGrenadeKill_ZCTable.data.deltaX;
			borY = gGrenadeKill_ZCTable.data.deltaY;
		}
		if(isFovSmall())
		{
			FOVSIZE_V = FOVDEGREE_VSMALL*(720-borX)/360; 
			FOVSIZE_H = FOVDEGREE_VSMALL*(576-borY)/288; 
		}
		else
		{
			FOVSIZE_V = FOVDEGREE_VLARGE*(720-borX)/360;
			FOVSIZE_H = FOVDEGREE_HLARGE*(576-borY)/288;
		}

		SHINE_DERECTION = DERECTION_NULL;
		
		if(-FOVSIZE_V >  MIL2DEGREE(output.AimOffsetThetaX))
		{
			//zuo ce chao chu
			SHINE_DERECTION = DERECTION_LEFT;
		}
		else if(FOVSIZE_V < MIL2DEGREE(output.AimOffsetThetaX))
		{
			//you ce chao chu
			SHINE_DERECTION = DERECTION_RIGHT;
		}
		else if(isMachineGun()&&(FOVSIZE_H < MIL2DEGREE(output.AimOffsetThetaY)))
		{
			//xia ce chao chu	
			SHINE_DERECTION = DERECTION_DOWN;
		}
		else
		{
			//printf("loadFiringTable_EnteloadFiringTable_EnterrloadFiringTable_Enter\n\n");
			//printf("getMeasureType = %d\n",getMeasureType());
			Posd[eMeasureType] = MeasureTypeOsd[getMeasureType()];


			//: input PID aimoffset
			/*AVTCTRL_ShiftAimOffsetX(output.AimOffsetX);
			if(isMachineGun())
				AVTCTRL_ShiftAimOffsetY(output.AimOffsetY);*/
			moveCrossCenter(output.AimOffsetX,output.AimOffsetY);
			
			if(isMachineGun())
			{
				//SendMessage(CMD_MACHSERVO_MOVEOFFSET, output.AimOffsetY-DEGREE2MIL(getGrenadeAngle()));
				cmd_machservo_moveoffset_tmp = output.AimOffsetY-DEGREE2MIL(getGrenadeAngle());
				processCMD_MACHSERVO_MOVEOFFSET(0);			
			}
			else
			{
				//SendMessage(CMD_GRENADESERVO_MOVEOFFSET, output.AimOffsetX-DEGREE2MIL(getTurretTheta()));
				cmd_grenadeservo_moveoffset_tmp = output.AimOffsetX-DEGREE2MIL(getTurretTheta());
				processCMD_GRENADESERVO_MOVEOFFSET(0);
			}
			processCMD_FIRING_TABLE_LOAD_OK(0);
				
			return ;
		}
		FOVSHINE = TRUE;
		startDynamicTimer();

	}
	else if(CALC_OVER_DISTANCE == ret )
	{
		Posd[eDynamicZone] = DynamicOsd[5];
		OSDCTRL_ItemShow(eDynamicZone);
		startDynamicTimer();
		processCMD_FIRING_TABLE_FAILURE(0);

		return;
	}
	else if( CALC_INVALID_PARAM == ret )
	{
		//SDK_ASSERT(FALSE);
		OSA_printf("CALC_INVALID_PARAM!\n");
	}
	else if(CALC_UNDER_DISTANCE == ret)
	{		
		moveCrossCenter(0,0);
		OSA_printf("%s__%d:CALC_UNDER_DISTANCE!\n",__func__,__LINE__);
	}
	return ;
}

void loadFiringTable()
{
	int ret;
	FiringInputs input;
	FiringOutputs output;
	memset(&input,0,sizeof(input));
	memset(&output,0,sizeof(output));
	input.AirPressure = Pressure;
//	input.DipAngle = 
	input.GrenadeYTheta = DEGREE2MIL(getGrenadeAngle());
	input.MachineGunYTheta = DEGREE2MIL(getMachGunAngle());
	input.PlatformXTheta = DEGREE2MIL(getPlatformPositionX());
	input.PlatformYTheta = DEGREE2MIL(getPlatformPositionY());
	input.ProjectileType = getProjectileType();
	if(!isMeasureManual()) 
	{
		ret = getAverageVelocity(&input.TargetAngularVelocityX);
		if(ret < 0)
			input.TargetAngularVelocityX = 0.0;
		//assert(FALSE);
		ret = getAverageDipVelocity(&input.TargetAngularVelocityY);
		if(ret < 0)
			input.TargetAngularVelocityY = 0.0;
		//assert(FALSE);
		input.TargetDistance = getLaserDistance();
	}else
	{
		input.TargetAngularVelocityX = 0.0;
		input.TargetAngularVelocityY = 0.0;
		input.TargetDistance = DistanceManual;
	}
//	input.TargetAngularVelocityY = 0;
	assert(input.TargetDistance >= 0);

	if(0 == input.TargetDistance)
		input.TargetDistance = 1;
	
	input.Temperature = Temparature;
	input.TurretDirectionTheta = DEGREE2MIL(getTurretTheta());
	
	ret = FiringCtrl( &input, &output);
	//output_prm_print(input, output);
		
//	sendCommand(CMD_SEND_MIDPARAMS);
	if(PROJECTILE_GRENADE_KILL == input.ProjectileType || PROJECTILE_GRENADE_GAS== input.ProjectileType)
	{
		setGrenadeDestTheta(MIL2DEGREE(output.AimOffsetThetaY) + getMachGunAngle());
	}
	AimOffsetX = (double)output.AimOffsetX;
	AimOffsetY = (double)output.AimOffsetY;
	//startAVTtimer();
	if(CALC_OK == ret)
	{
		int borX=0,borY=0;
		//startRGQtimer();
		//todo ValidateOutput(&output);//check offset overflow. 5.6x4.2 16x12
		if(isMachineGun())
		{
			borX = gMachineGun_ZCTable.data.deltaX;
			borY = gMachineGun_ZCTable.data.deltaY;
		}
		else
		{
			borX = gGrenadeKill_ZCTable.data.deltaX;
			borY = gGrenadeKill_ZCTable.data.deltaY;
		}
		if(isFovSmall()){
			FOVSIZE_V = FOVDEGREE_VSMALL*(720-borX)/360; 
			FOVSIZE_H = FOVDEGREE_VSMALL*(576-borY)/288; 
		}
		else
		{
			FOVSIZE_V = FOVDEGREE_VLARGE*(720-borX)/360;
			FOVSIZE_H = FOVDEGREE_HLARGE*(576-borY)/288;
		}

		if(-FOVSIZE_V >  MIL2DEGREE(output.AimOffsetThetaX))
		{
			SHINE_DERECTION = DERECTION_LEFT;
		}
		else if(FOVSIZE_V < MIL2DEGREE(output.AimOffsetThetaX))
		{
			SHINE_DERECTION = DERECTION_RIGHT;
		}
		else if((FOVSIZE_H < MIL2DEGREE(output.AimOffsetThetaY))&&isMachineGun())
		{
			SHINE_DERECTION = DERECTION_DOWN;
		}
		else
		{
			Posd[eMeasureType] = MeasureTypeOsd[getMeasureType()];

			//AVTCTRL_ShiftAimOffsetX(output.AimOffsetX);
			if(isMachineGun())
				;//AVTCTRL_ShiftAimOffsetY(output.AimOffsetY);
			
			moveCrossCenter(output.AimOffsetX,output.AimOffsetY);

			processCMD_FIRING_TABLE_LOAD_OK(0);
			//sendCommand(CMD_FIRING_TABLE_LOAD_OK);
			return ;
		}
		FOVSHINE = TRUE;
		startDynamicTimer();
		//sendCommand(CMD_QUIT_AVT_TRACKING);

	}
	else if(CALC_OVER_DISTANCE == ret )
	{
		Posd[eDynamicZone] = DynamicOsd[5];
		OSDCTRL_ItemShow(eDynamicZone);
		startDynamicTimer();
		processCMD_FIRING_TABLE_FAILURE(0);
		//sendCommand(CMD_QUIT_AVT_TRACKING);

		return;
	}
	else if( CALC_INVALID_PARAM == ret || CALC_UNDER_DISTANCE == ret)
	{
		SDK_ASSERT(FALSE);
	}
	
}


void EnterCMD_BULLET_SWITCH(int i)
{
	//gProjectileType=i;
	UpdataBoreSight();
	if(isCalibrationMode()&& isCalibrationMainMenu())
	{
		//check XPosition before  udateMenuItem_Zero_General()
		//udateMenuItem_Zero_General(PROJECTILE_GRENADE_KILL);
		OSDCTRL_updateMainMenu(i);
	}else 
	{
		if(isCalibrationMode()&&isCalibrationZero())
		{
			OSDCTRL_CalibZeroShow();
		}else if(isCalibrationMode()&&isCalibrationGeneral())
		{
			saveLastAndGetNewGeneralParam(i);
			updateBulletType(i);
		}else if(isBattleMode()&&isMeasureManual()&&isBeyondDistance())
		{
			Posd[eDynamicZone] = DynamicOsd[5];
			OSDCTRL_ItemShow(eDynamicZone);
			startDynamicTimer();
		}
	}
	UpdataBoreSight();
	setServoControlObj();
}



void EnterCMD_BULLET_SWITCH1()
{
	if(isCalibrationMode() && isCalibrationMainMenu())
	{
		OSDCTRL_updateMainMenu(PROJECTILE_BULLET);
	}
	else
	{
		if(isCalibrationMode() && isCalibrationZero())
		{
			saveLastAndGetNewZeroParam(PROJECTILE_BULLET);
		}
		else if(isCalibrationMode()&&isCalibrationGeneral())
		{
			saveLastAndGetNewGeneralParam(PROJECTILE_BULLET);
			updateBulletType(PROJECTILE_BULLET);
		}
		#if 0
		else if(isBattleMode()&&isMeasureManual()&&isBeyondDistance())
		{
			Posd[eDynamicZone] = DynamicOsd[5];
			OSDCTRL_ItemShow(eDynamicZone);
			startDynamicTimer();
		}
		#endif
	}
	if(gProjectileMachFovlast)
		MSGDRIV_send(CMD_MODE_FOV_LARGE, 0);
	else
		MSGDRIV_send(CMD_MODE_FOV_SMALL, 0);
	
	UpdataBoreSight();
	setServoControlObj();

}


void EnterCMD_BULLET_SWITCH2( )
{
	//g_Text[eShotType].osdInitX = 545;
	if(isCalibrationMode()&& isCalibrationMainMenu())
	{
		OSDCTRL_updateMainMenu(PROJECTILE_GRENADE_KILL);
	}
	else 
	{
		if(isCalibrationMode()&&isCalibrationZero())
		{
			saveLastAndGetNewZeroParam(PROJECTILE_GRENADE_KILL);
		}
		else if(isCalibrationMode()&&isCalibrationGeneral())
		{
			saveLastAndGetNewGeneralParam(PROJECTILE_GRENADE_KILL);
			updateBulletType(PROJECTILE_GRENADE_KILL);
		}
		else if(isBattleMode()&&isMeasureManual()&&isBeyondDistance())
		{
			Posd[eDynamicZone] = DynamicOsd[5];
			OSDCTRL_ItemShow(eDynamicZone);
			startDynamicTimer();
		}
	}
	if(gProjectileGreFovlast)
		MSGDRIV_send(CMD_MODE_FOV_LARGE, 0);
	else
		MSGDRIV_send(CMD_MODE_FOV_SMALL, 0);
	//gProjectileType=PROJECTILE_GRENADE_KILL;
	UpdataBoreSight();
	setServoControlObj();
}


void EnterCMD_BULLET_SWITCH3( )
{
	MSGDRIV_send(CMD_MODE_FOV_LARGE,0);
	if(isCalibrationMode()&& isCalibrationMainMenu())
	{
		OSDCTRL_updateMainMenu(PROJECTILE_GRENADE_GAS);
	}
	else 
	{
		if(isCalibrationMode()&&isCalibrationZero())
		{
			saveLastAndGetNewZeroParam(PROJECTILE_GRENADE_GAS);
		}
		else if(isCalibrationMode()&&isCalibrationGeneral())
		{
			saveLastAndGetNewGeneralParam(PROJECTILE_GRENADE_GAS);
			updateBulletType(PROJECTILE_GRENADE_GAS);
		}
		else if(isBattleMode()&&isMeasureManual()&&isBeyondDistance())
		{
			Posd[eDynamicZone] = DynamicOsd[5];
			OSDCTRL_ItemShow(eDynamicZone);
			startDynamicTimer();
		}
	}
	gProjectileType=PROJECTILE_GRENADE_GAS;
	UpdataBoreSight();
	setServoControlObj();
}



/********************************TIMER**********************************/
int  CTIMERCTRL_initTimerCtrl()
{
	CTimerCtrl * pCtrlObj= pTimerObj;
	SDK_ASSERT(pTimerObj!=NULL);
	
	memset(pCtrlObj->pTimeArray,0,sizeof(CDTime)*MAX_TIMER_NUM);
	pCtrlObj->timNum =0;

	pCtrlObj->startTimer		=	start_Timer;
	pCtrlObj->KillTimer 		= 	kill_Timer;
	pCtrlObj->GetTimerStat 	= 	CTIMERCTRL_getTimerStat;
	pCtrlObj->ReSetTimer 		= 	reset_Timer;
	//pCtrlObj->SetTimer		= 	Dx_setTimer;
	//pCtrlObj->RunTimer= NULL;
	//pCtrlObj->DestroyTimerCtrl = NULL;
	//pCtrlObj->InitTimerCtrl = NULL;
	
	
	return 0;
}

int CTIMERCTRL_getTimerStat(unsigned int nIDEvent)
{
	CDTime * pTime =NULL;
	CTimerCtrl * pCtrlObj	= pTimerObj;

	SDK_ASSERT(pTimerObj!=NULL);
	SDK_ASSERT(nIDEvent < MAX_TIMER_NUM);

	pTime = &pCtrlObj->pTimeArray[nIDEvent];

	return	(pTime->nStat);
}


void startDynamicTimer()
{	
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eDynamic_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eDynamic_Timer,DYNAMIC_TIMER);	
	}
}

void DynamicTimer_cbFxn()
{
	int i;
	killDynamicTimer();
	//if(OSDCTRL_IsOsdDisplay(eErrorZone))
	//	OSDCTRL_ItemHide(eErrorZone);
	
	if(OSDCTRL_IsOsdDisplay(eDynamicZone))
	{
		if(Posd[eDynamicZone] != DynamicOsd[0])
			OSDCTRL_ItemHide(eDynamicZone);
	}

	if(!isMeasureOsdNormal())
	{
		Posd[eMeasureType] = MeasureTypeOsd[gMeasureType];
	}
	

	if(isFovShine())
		FOVSHINE = FALSE;
}

void killDynamicTimer()
{	
	CTimerCtrl * pCtrlTimer = pTimerObj;
	pCtrlTimer->KillTimer(eDynamic_Timer);
}



void processCMD_FIRING_TABLE_LOAD_OK(long lParam)
 {
	if(isBattleMode()&& isStatBattleAuto()&&isBattleLoadFiringTable())//&&isTrackingMode())
	{
		
			gLevel3CalculatorState = Battle_Ready;
			//OSDdisplay
		
			Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_GQ];
			//start a timer in 6sec timeout set osd CORRECTION_RGQ			
			OSDCTRL_ItemShow(eCorrectionTip);

			// auto track and shoot.
			requstServoContrl();

			OSDCTRL_ItemShow(ePlatFormX);
			//start a timer in 6sec timeout set osd CORRECTION_RGQ
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoLoadFiringTable())
	{
			gLevel3CalculatorState = Auto_Ready;
			//OSDdisplay
			Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_GQ];
			//start a timer in 6sec timeout set osd CORRECTION_RGQ			
			OSDCTRL_ItemShow(eCorrectionTip);

	}
}


void processCMD_FIRING_TABLE_FAILURE(long lParam)
 {
	if(isBattleMode()&& isStatBattleAuto()&&isBattleLoadFiringTable())
	{
		enterLevel3CalculatorIdle();
		releaseServoContrl();
 		//OSDdisplay  ? 		
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoLoadFiringTable())
	{
		gLevel3CalculatorState = Auto_Idle;
	}
	return ;
 }

int getAdvancedMenuIndex()
{
	int i=(Posd[eCalibMenu_Child] - GenPramMenu[0])/12;
	return i;
}
void decreaseAdvancedMenu()
{
	int i = getAdvancedMenuIndex();
	i = (i+3)%4;
	Posd[eCalibMenu_Child] = GenPramMenu[i];
	gLevel3CalibrationState = (Level_three_state)(i + Menu_FireView);
}

void changeSaveOption()
{
	if(isAtSaveYes())
		Posd[eSaveYesNo] = SaveYesNoOsd[1];
	else
		Posd[eSaveYesNo] = SaveYesNoOsd[0];
}

void increaseAdvancedMenu()
{
	int i = getAdvancedMenuIndex();
	i = (i+1)%4;
	Posd[eCalibMenu_Child] = GenPramMenu[i];
	gLevel3CalibrationState =(Level_three_state)(i + Menu_FireView);
}


void processCMD_MAINTPORT_LOCK(long lParam)
 {
 	isMaintPortOpen=FALSE;
	OSDCTRL_updateAreaN();
	return ;
 }

void processCMD_MAINTPORT_UNLOCK(long lParam)
 {
 	isMaintPortOpen=TRUE;
	OSDCTRL_updateAreaN();
	return ;
 }


void processCMD_DETEND_LOCK(long lParam)
 {
 	isDetendClose=FALSE;
	OSDCTRL_updateAreaN();
	return ;
 }


void processCMD_DETEND_UNLOCK(long lParam)
 {
 	isDetendClose=TRUE;
	OSDCTRL_updateAreaN();
	return ;
 }

void startSelfCheckTimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eBootUp_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eBootUp_Timer,BOOTUP_TIMER);	
	}
}


float getAimOffsetX()
{
	return (AimOffsetX)/100.0;
}

float getAimOffsetY()
{
	return (AimOffsetY)/100.0;
}

bool isBeyondDerection(Derection_Type type)
{
	return type == SHINE_DERECTION;
}


void processCMD_MODE_ATTACK_SIGLE(LPARAM lParam)
 {
	if(!isMachineGun())
		gShotType = SHOTTYPE_SHORT;
	return ;
 }


void processCMD_MODE_ATTACK_MULTI(LPARAM lParam)
 {
	if(!isMachineGun())
		gShotType = SHOTTYPE_LONG;
	return ;
 }

void startSCHEDULEtimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eSchedule_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eSchedule_Timer,SCHEDULE_TIMER);	
	}
}


void killSCHEDULEtimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eSchedule_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eSchedule_Timer);
	}
}


void SCHEDULE_cbFxn(void* cbParam)
{
	static int jsq1 = 0,jsq2 = 0;
	//	killSCHEDULEtimer();
		float x=0.0,y=0.0;
		if(SCHEDULE_GUN/*not timeout and angle not ok*/)
		{
			float tempX = (getpanoAngleH()<180)?(getpanoAngleH()):(getpanoAngleH()-360);
			x = (tempX - getTurretTheta());
			y = (getpanoAngleV() - getMachGunAngle());
			if(((abs(x)<1)&&(abs(y)<1))||(100<COUNTER))
			{
	//			getPelcoServoContrlObj()->stop();
				killSCHEDULEtimer();
				releaseServoContrl();
				SCHEDULE_GUN = FALSE;
				OSDCTRL_ItemHide(eSuperOrder);
				return;
			}
	//		getPelcoServoContrlObj()->moveOffset(x,y);
			startSCHEDULEtimer();
		}
		else if(SCHEDULE_STRONG/*not timeout and angle not ok*/)
		{
			float tempX = (getpanoAngleH()<180)?(getpanoAngleH()-180):(getpanoAngleH()-180);
			x = (tempX - getTurretTheta());
			y = (getpanoAngleV() - getMachGunAngle());
			if(((abs(x)<1)&&(abs(y)<1))||(100<COUNTER))
			{
	//			getPelcoServoContrlObj()->stop();
				killSCHEDULEtimer();
				releaseServoContrl();
				SCHEDULE_STRONG = FALSE;
				OSDCTRL_ItemHide(eSuperOrder);
				return;
			}
	//		getPelcoServoContrlObj()->moveOffset(x,y);
			startSCHEDULEtimer();
		}
		else if(SCHEDULE_RESET/*not timeout and angle not ok*/)
		{			
			x = (getTurretTheta());
			y = (getMachGunAngle());
			if(((abs(x)<1)&&(abs(y)<1))||(100<COUNTER))
			{
	//			getPelcoServoContrlObj()->stop();
				killSCHEDULEtimer();
				releaseServoContrl();
				SCHEDULE_RESET = FALSE;
				OSDCTRL_ItemHide(eSuperOrder);
				return;
			}
	//		getPelcoServoContrlObj()->moveOffset(x,y);
			startSCHEDULEtimer();
		}
		COUNTER++;
}


void processCMD_SCHEDULE_GUN(long lParam)
 {
 	OSDCTRL_NoShine();
	if(isCalibrationMode())
		return;
	// tiao qiang ta
	Posd[eSuperOrder] = SuperOsd[0];
	OSDCTRL_ItemShow(eSuperOrder);
	requstServoContrl();
	SCHEDULE_GUN = TRUE;
	COUNTER = 0;
	startSCHEDULEtimer();
	
	return ;
 }



void startRGQtimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eRGQ_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eRGQ_Timer,RGQ_TIMER);	
	}
}


void killRGQtimer()
{
	CTimerCtrl * pCtrlTimer = pTimerObj;
	if(pCtrlTimer->GetTimerStat(eRGQ_Timer)!=eTimer_Stat_Stop)
	{
		pCtrlTimer->KillTimer(eRGQ_Timer);
	}
}


void processCMD_MIDPARAMS_SWITCH(long lParam)
{
	MIDPARAMS = !MIDPARAMS;
	if(MIDPARAMS)
	{
		CONNECT = FALSE;
		OSDCTRL_ConnectMenuHide();
		OSDCTRL_CalcNumHide();
		OSDCTRL_FulScrAngleShow();

		//add
		OSDCTRL_ConnectMenuShow();
	}
	else
	{ 
		OSDCTRL_CalcNumHide();
		OSDCTRL_FulScrAngleHide();
		OSDCTRL_ConnectMenuHide();
		//add
		
	}
}


void processCMD_LASERSELECT_SWITCH(long lParam)
{
	static bool SELECT = FALSE;
	SELECT = !SELECT;
	if(SELECT)
	{
		Posd[eLaserState] = LaserOsd[0];
	}
	else
	{
		Posd[eLaserState] = LaserOsd[1];
	}
}

void processCMD_SENSOR_SWITCH(long lParam)
{
	
	CTimerCtrl * pCtrlTimer = pTimerObj;
	printf("pCtrlTimer->GetTimerStat(eFxbutton_Timer) = %d\n",pCtrlTimer->GetTimerStat(eFxbutton_Timer));
	if(pCtrlTimer->GetTimerStat(eFxbutton_Timer)==eTimer_Stat_Stop)
	{
		pCtrlTimer->startTimer(eFxbutton_Timer,10000);	

		if(isBattleMode())
		{
			OSDCTRL_ItemHide(eAngleV);
			OSDCTRL_ItemHide(eWeather2);
		}
		else if(isCalibrationMode())
		{
			OSDCTRL_ItemHide(eAnglePosn);
			OSDCTRL_ItemHide(eAngleMach);
			OSDCTRL_ItemHide(eAngleGred);
		}	
		OSDCTRL_BaseMenuShow();	
	}
	else
	{
		pCtrlTimer->KillTimer(eFxbutton_Timer);
		pCtrlTimer->startTimer(eFxbutton_Timer,10000);
	}

	#if 0 
		BASEMENU = !BASEMENU;
		if(BASEMENU)
		{
			OSDCTRL_ItemHide(eAngleV);
			OSDCTRL_ItemHide(eWeather2);
			OSDCTRL_BaseMenuShow();
		}
		else
		{
			BASEMENU = FALSE;
			//OSDCTRL_BaseMenuHide();
	//		OSDCTRL_ConnectMenuHide();
		}
	#endif	
}

void processCMD_CONNECT_SWITCH(long lParam)
{
	CONNECT = !CONNECT;
	if(CONNECT)
	{
		MIDPARAMS = FALSE;
		OSDCTRL_CalcNumHide();
		OSDCTRL_FulScrAngleHide();
		OSDCTRL_ConnectMenuShow();
	}
	else
	{
		//OSDCTRL_BaseMenuHide();
		OSDCTRL_ConnectMenuHide();
	}
}



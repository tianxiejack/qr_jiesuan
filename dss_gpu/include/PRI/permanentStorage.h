#ifndef PERMANENTSTORAGE_H
#define PERMANENTSTORAGE_H

#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

//#include "statCtrl.h"



typedef struct _ThetaDeltas 
{
	double deltaX; //密位
	double deltaY; //密位
} GeneralCorrection, TankTheta;


typedef enum {
	PROJECTILE_BULLET,
	PROJECTILE_GRENADE_KILL,
	PROJECTILE_GRENADE_GAS,
	QUESTION_GRENADE_KILL,
	QUESTION_GRENADE_GAS,
	QUESTION_BULLET
}PROJECTILE_TYPE;



typedef struct _GeneralCorrectionItem{
	int distance;
	GeneralCorrection data;
}GeneralCorrectionItem;


typedef struct _ZeroCorrectionItem{
	double distance;
	GeneralCorrection data;
	double Angle;
} ZeroCorrectionItem;


typedef struct _weather_table{
	int Temparature;
	int Pressure;
}WeatherItem;


typedef struct _gCalibrationParams{
	double Temparature;
	double Pressure;
	ZeroCorrectionItem MachineGun_ZCTable;
	ZeroCorrectionItem GrenadeKill_ZCTable;
	GeneralCorrectionItem MachineGun_GCTable[16];
	GeneralCorrectionItem GrenadeKill_GCTable[19];
	GeneralCorrectionItem GrenadeGas_GCTable[81];
}gCalibrationParams,*gCalibrationParams_Handle;

extern GeneralCorrectionItem gMachineGun_GCParam; 
extern GeneralCorrectionItem gGrenadeKill_GCParam;	
extern GeneralCorrectionItem gGrenadeGas_GCParam;

#define ANGLE_TO_OFFSET(angle)  angleToOffset(angle) 
#define OFFSET_TO_ANGLE(pixels) offsetToAngle(pixels)

#define PI (3.14159)
#define MIL2RADIAN(mil) 	((mil)*360*PI/(180*6000))
#define DEGREE2MIL(degree) 	((degree)*6000.0/360.0)
#define RADIAN2MIL(radian)	(DEGREE2MIL((radian)*(180/PI)))
#define BIAS2MIL(bias, distance)	RADIAN2MIL(((bias)/(distance)))
#define MIL2DEGREE(mil) ((mil)*360/6000.0)
#define RADIAN2DEGREE(r) ((r) * 180/PI)

extern gCalibrationParams hCalibObj;
// todo : use pexelToThetaRateTable in FiringInputs instead
extern GeneralCorrectionItem gMachineGun_GCTable[];
extern GeneralCorrectionItem gGrenadeKill_GCTable[];
extern GeneralCorrectionItem gGrenadeGas_GCTable[];
extern ZeroCorrectionItem gMachineGun_ZCTable;
extern ZeroCorrectionItem gGrenadeKill_ZCTable;
int getGrenadeSize();
int getGrenadeGasSize();
int getMachGunSize();

//综修参数获取
void setGeneralCorrection(double distance, PROJECTILE_TYPE type, GeneralCorrection data);
GeneralCorrection getGeneralCorrection(double distance, PROJECTILE_TYPE type);
GeneralCorrection getGeneralCorrectionTheta(double distance, PROJECTILE_TYPE type);
//获取坦克倾斜角密位
TankTheta getTankTheta();
//int getZeroDistance(PROJECTILE_TYPE type){return 0;}

int angleToOffset(double angleMil);
double offsetToAngle(double pixels);
int ReadParamsFlash();
int WriteParamsFlash();

#ifdef __cplusplus
}
#endif

#endif


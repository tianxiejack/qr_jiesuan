#ifndef _FIRINGCTRL_H
#define _FIRINGCTRL_H

#include "permanentStorage.h"

#ifdef __cplusplus
extern "C" {
#endif

// Firing control error codes definition
#define CALC_OK 0
#define CALC_INVALID_PARAM	(-1)
#define CALC_OVER_DISTANCE (-2)
#define CALC_UNDER_DISTANCE		(-3)


#define MAX_COUNT_ANGULAR_VELO 200
#define PIXEL_TO_THETA_COUNT 100

typedef struct _FiringInputs {
	float PlatformXTheta;
	float PlatformYTheta; 
	float MachineGunYTheta; 
	float GrenadeYTheta;	
	double TurretDirectionTheta;
	double TargetAngularVelocityX;
	double TargetAngularVelocityY; 
	double TargetDistance;
	//float PlatformAngularVelocities[MAX_COUNT_ANGULAR_VELO];
	PROJECTILE_TYPE ProjectileType;
	float PixelToThetaRateTable_narrowView[PIXEL_TO_THETA_COUNT];
	float PixelToThetaRateTable_wideView[PIXEL_TO_THETA_COUNT];
	double Temperature;
	double AirPressure;
	float DipAngle;
} FiringInputs;

typedef struct _FiringOutputs {
	double projectileDuration;
	double AimElevationAngle;
	double BiasAngle;		 
	GeneralCorrection correctionData;

	double AimOffsetThetaX; 
	double AimOffsetThetaY; 
	int AimOffsetX;//pixels
	int AimOffsetY;//pixels

}FiringOutputs;

typedef struct _CorrectionDelta {
	double DeltaThetaX;
	double DeltaThetaY;
}CorrectionDelta;

float getSumCalibX(void);
float getSumCalibY(void);
float getVisualCalibX(void);
float getVisualCalibY(void);
float getTrajectCalcX(void);
float getTrajectCalcY(void);
float getTrunionCalibX(void);
float getTrunionCalibY(void);
float getCorrectionCalibX(void);
float getCorrectionCalibY(void);

// calculate the firing outputs given the firing inputs
int FiringCtrl( FiringInputs *input, FiringOutputs* output);


void setGrenadeDestTheta(double theta);
double getGrenadeDestTheta();
int getParamBuf(unsigned char * buf);
void setUseInterPolation(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _FIRINGCTRL_H */


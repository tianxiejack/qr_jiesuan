#ifndef _osdPort_H_
#define _osdPort_H_

#include "osdProcess.h"
#include "basePort.h"



typedef struct _ThetaDeltas 
{
	double deltaX; //��λ
	double deltaY; //��λ
} GeneralCorrection, TankTheta;


typedef struct _GeneralCorrectionItem{
	int distance;
	GeneralCorrection data;
}GeneralCorrectionItem;


typedef struct _ZeroCorrectionItem{
	double distance;
	GeneralCorrection data;
	double Angle;
} ZeroCorrectionItem;



void moveCrossRight();
void moveCrossLeft();
void moveCrossDown();
void moveCrossUp();
void moveUpXposition();
void moveDownXPosition();
int getXPosition();
void initilZeroParam(int type);

void initilWeatherParam();
void getWeatherParam();
bool isXatSave();
void moveFocusRight();
void moveFocusRightGeneral();
void moveFocusLeft();
void moveFocusLeftGeneral();
void increaseDigitOrSymbolZero();
void decreaseDigitOrSymbolZero();
void decreaseDigitOrSymbolGeneral();
void increaseDigitOrSymbolGeneral();
void saveWeatherParam();
void initilGeneralParam();
void increaseMeasureDis();
void decreaseMeasureDis();





extern FOVCTRL_Handle pFovCtrlObj;

#endif


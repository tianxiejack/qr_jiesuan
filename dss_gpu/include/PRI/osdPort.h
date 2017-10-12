#ifndef _osdPort_H_
#define _osdPort_H_

#include "osdProcess.h"
#include "basePort.h"
#include "permanentStorage.h"


void saveLastAndGetNewZeroParam(int type);
void saveLastAndGetNewGeneralParam(int type);
void increaseMeasureMul();
void decreaseMeasureMul();
void moveCrossCenter(int x, int y);
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
void saveZeroParam();
static void saveLastGeneralParam(GeneralCorrectionItem * Item);
int getLastGeneralDistance();
double getLastGeneralDataX();
double getLastGeneralDataY();
void setNextGeneralParam(GeneralCorrectionItem * Item);
void UpdataBoreSight();
static void setNextZeroData(ZeroCorrectionItem *Item);
void moveLeftFireViewPram();
void moveLeftFireCtrlPram();
void moveRightFireViewPram();
void moveRightFireCtrlPram();
void udateMenuItem_Zero_General(PROJECTILE_TYPE type);
void updateMoveCross();
void moveCrossAbs(int x, int y);
void saveGeneralParam();
int getDisLen();



extern FOVCTRL_Handle pFovCtrlObj;

#if 0
	extern GeneralCorrectionItem gMachineGun_GCParam;
	extern GeneralCorrectionItem gGrenadeKill_GCParam;
	extern GeneralCorrectionItem gGrenadeGas_GCParam;

	extern ZeroCorrectionItem gMachineGun_ZCTable;
	extern ZeroCorrectionItem gGrenadeKill_ZCTable;
	extern WeatherItem gWeatherTable;
#endif

#endif


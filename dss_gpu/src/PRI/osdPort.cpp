

#include "osdPort.h"
#include "osdProcess.h"
#include "statCtrl.h"
#include "cFov.h"

#define abs(a) (((a)>0)?(a):(0-(a)))
#define STEPLEN 1

extern OSDCTRL_OBJ * pCtrlObj;


FOVCTRL_Handle	 pFovCtrlObj = NULL;	
	

GeneralCorrectionItem gMachineGun_GCParam 	={0,		{0,0}};
GeneralCorrectionItem gGrenadeKill_GCParam		={0,		{0,0}};
GeneralCorrectionItem gGrenadeGas_GCParam	={0,		{0,0}};

ZeroCorrectionItem gMachineGun_ZCTable = {500,	{352,288}};
ZeroCorrectionItem gGrenadeKill_ZCTable = {500,	{352,288}};
//WeatherItem gWeatherTable={15,101325};



int getCrossX()
{
	return pFovCtrlObj->fovX;
}


int getCrossY()
{
	return pFovCtrlObj->fovY<<1;
}


void moveCrossUp()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	borY = cthis->fovY<<1;
	borY = (borY- STEPLEN + 576)%576;

	FOVCTRL_updateFovRect(cthis,0,0,borX,(borY)>>1);
	//todo: 移动方位伺服
}


void increaseDigitOrSymbolGeneral()
{
	int i = ShinId-eCalibGeneral_DisValue1;	
	if(!SHINE)
		return;

	if((4 == i) ||(9 == i))
	{
		if(General[i] >= 0)
			General[i]--;
		else
			General[i]++;
	}
	/*else if(0 == i)
	{
		//更新距离、综修参数
		// 更新 修正值
		saveLastAndGetDownGeneralData();
	}*/
	else
	{
		General[i] = (General[i] + 1)%10;
	}
	
}


void saveWeatherParam()
{
	Pressure = Weather[3]*1000000 + Weather[4]*100000 + Weather[5]*10000
				+Weather[6]*1000 +Weather[7]*100 +Weather[8]*10 +Weather[9];
	
	Temparature = (Weather[0])?(Weather[1]*10 +Weather[2]):(-(Weather[1]*10 +Weather[2]));
	
	gWeatherTable.Temparature = Temparature;
	gWeatherTable.Pressure	  = Pressure;
}

void setNextGeneralParam(GeneralCorrectionItem * Item)
{
	int value;
	if(isMeasureManual())
		value = DistanceManual;
	else
		value = getLaserDistance();
	value = (value>0)?(value):500;//如果未设置距离值，默认设置500米

	General[0] = value/1000%10;
	General[1] = value/100%10;
	General[2] = value/10%10;
	General[3] = value%10;
	
	General[4]  = (Item->data.deltaX >= 0)?0:-1;
	value = abs(Item->data.deltaX);
	General[5]  = value/1000%10;
	General[6]  = value/100%10;
	General[7]  = value/10%10;
	General[8]  = value%10;
	
	General[9]  = (Item->data.deltaY >= 0)?0:-1;
	value = abs(Item->data.deltaY);
	General[10]  = value/1000%10;
	General[11]  = value/100%10;
	General[12]  = value/10%10;
	General[13] = value%10;
}


void initilGeneralParam()
{
	if(isMachineGun())
	{
		setNextGeneralParam(&gMachineGun_GCParam);
	}
	else if(isGrenadeKill())
	{
		setNextGeneralParam(&gGrenadeKill_GCParam);
	}
	else if(isGrenadeGas())
	{
		setNextGeneralParam(&gGrenadeGas_GCParam);
	}

}


static void setNextZeroData(ZeroCorrectionItem *Item)
{
	
	//CFOV* cthis = pFovCtrlObj;
	//int borX,borY;
	//borX = cthis->fovX = Item->data.deltaX;
	//borY = cthis->fovY = Item->data.deltaY;

	//FOVCTRL_updateFovRect(cthis,0,0,borX,(borY)>>1);
}



void initilZeroParam(int type)
{
	if(PROJECTILE_BULLET == type /*isMachineGun()*/){
		setNextZeroData(&gMachineGun_ZCTable);
	}else if(PROJECTILE_GRENADE_KILL == type/*isGrenadeGas()*/){
		setNextZeroData(&gGrenadeKill_ZCTable);
	}
//	AngleGun = gMachineGun_ZCTable.Angle;
//	AngleGrenade = gGrenadeKill_ZCTable.Angle;
}


void increaseDigitOrSymbolZero()//weather
{
	if(!SHINE)
		return;
	if(eCalibWeather_TepPole == ShinId)
	{
		if(Weather[0] > 0)
			Weather[0]--;
		else
			Weather[0]++;
	}
	else
	{
		Weather[ShinId-eCalibWeather_TepPole] = (Weather[ShinId-eCalibWeather_TepPole] + 1)%10;
	}
}


void decreaseDigitOrSymbolZero()//weather
{
	if(!SHINE)
		return;
	if(eCalibWeather_TepPole == ShinId)
	{
		if(Weather[0]> 0)
			Weather[0]--;
		else
			Weather[0]++;
	}else
	{
		Weather[ShinId-eCalibWeather_TepPole] = (Weather[ShinId-eCalibWeather_TepPole] + 9)%10;
	}
}



void decreaseDigitOrSymbolGeneral()
{
	int i = ShinId-eCalibGeneral_DisValue1;	
	if(!SHINE)
		return;

	if((4 == i) ||(9 == i))
	{
		if(General[i] >= 0)
			General[i]--;
		else
			General[i]++;
	}
	else
	{
		General[i] = (General[i] + 9)%10;
	}
}



void moveUpXposition()
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	int i;
	SDK_ASSERT(pCtrlObj!=NULL);
	
	//if(pCtrlObj->uInit==0)
	//	return;

	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+3)%4)*35;
	//pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+6)%7)*35;
	return;
}



void moveDownXPosition()
{
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;
	int i;
	SDK_ASSERT(pCtrlObj!=NULL);
	//if(pCtrlObj->uInit==0)
	//	return;
	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	//pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+1)%7)*35;
	pTextObj[eCursorX].osdInitY = pTextObj[eCalibMenu_Weather].osdInitY +((i+1)%4)*35;
}

int getXPosition()
{
	int i;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return -1;
	i = (pTextObj[eCursorX].osdInitY - pTextObj[eCalibMenu_Weather].osdInitY)/35;

	return i;
}

void initilWeatherParam()
{
	getWeatherParam();
	return ;
}

void getWeatherParam()
{
	int value;
	Temparature = gWeatherTable.Temparature;
	Pressure	= gWeatherTable.Pressure;
	Weather[0] = (Temparature>0)?1:0;
	value = abs(Temparature);
	Weather[1] = value/10;
	Weather[2] = value%10;
	Weather[3] = Pressure/1000000%10;
	Weather[4] = Pressure/100000%10;
	Weather[5] = Pressure/10000%10;
	Weather[6] = Pressure/1000%10;
	Weather[7] = Pressure/100%10;
	Weather[8] = Pressure/10%10;
	Weather[9] = Pressure%10;

	return ;
}


bool isXatSave()
{
	//OSDCTRL_OBJ * pCtrlObj = (OSDCTRL_OBJ * )pOsdCtrlObj;
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return FALSE;
	
	return (pTextObj[eCalibMenu_Save].osdInitY == pTextObj[eCursorX].osdInitY);
}


void moveFocusRight()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibWeather_TepPole + (ShinId-eCalibWeather_TepPole + 1)%9;
}


void moveFocusRightGeneral()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGeneral_DisValue1 + (ShinId - eCalibGeneral_DisValue1 +1)%14;
}


void moveFocusLeft()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibWeather_TepPole + (ShinId-eCalibWeather_TepPole + 6)%7;
}


void moveFocusLeftGeneral()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGeneral_DisValue1 + (ShinId - eCalibGeneral_DisValue1+ 13)%14;
}





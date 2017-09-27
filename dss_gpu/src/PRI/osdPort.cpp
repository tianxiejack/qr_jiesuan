

#include "osdPort.h"
#include "osdProcess.h"
#include "statCtrl.h"
#include "cFov.h"



#define abs(a) (((a)>0)?(a):(0-(a)))
#define STEPLEN 4

extern OSDCTRL_OBJ * pCtrlObj;
extern int shine_table[10];


FOVCTRL_Handle	 pFovCtrlObj = NULL;	
	
#if 0
GeneralCorrectionItem gMachineGun_GCParam 	={0,		{0,0}};
GeneralCorrectionItem gGrenadeKill_GCParam		={0,		{0,0}};
GeneralCorrectionItem gGrenadeGas_GCParam	={0,		{0,0}};

ZeroCorrectionItem gMachineGun_ZCTable = {500,	{352,288}};
ZeroCorrectionItem gGrenadeKill_ZCTable = {500,	{352,288}};
WeatherItem gWeatherTable={15,101325};
#endif

static int DISLEN=100;


void saveLastAndGetNewZeroParam(int type)
{
	if(isMachineGunBefore()){
//		gMachineGun_ZCTable.distance = DistanceManual;
		gMachineGun_ZCTable.data.deltaX = getCrossX();
		gMachineGun_ZCTable.data.deltaY = getCrossY();
		//AVTCTRL_UpdateFOV(1,gMachineGun_ZCTable.data.deltaX,gMachineGun_ZCTable.data.deltaY);
		gMachineGun_ZCTable.Angle		= getMachGunAngleAbs();
		gGrenadeKill_ZCTable.Angle	 	= getGrenadeAngleAbs();

	}
	else if(isGrenadeKillBefore()){
//		gGrenadeKill_ZCTable.distance = DistanceManual;
		gGrenadeKill_ZCTable.data.deltaX = getCrossX();
		gGrenadeKill_ZCTable.data.deltaY = getCrossY();
		//AVTCTRL_UpdateFOV(2,gGrenadeKill_ZCTable.data.deltaX,gGrenadeKill_ZCTable.data.deltaY);
		gMachineGun_ZCTable.Angle		= getMachGunAngleAbs();
		gGrenadeKill_ZCTable.Angle		 = getGrenadeAngleAbs();
	}

	if(PROJECTILE_BULLET == type /*isMachineGun()*/)
	{
		setNextZeroData(&gMachineGun_ZCTable);
	}
	else if(PROJECTILE_GRENADE_KILL == type/*isGrenadeGas()*/)
	{
		setNextZeroData(&gGrenadeKill_ZCTable);
	}
}



void saveLastAndGetNewGeneralParam(int type)
{
	if(isGrenadeKillBefore())
	{
		saveLastGeneralParam(&gGrenadeKill_GCParam);
	}
	else if(isGrenadeGasBefore())
	{
		saveLastGeneralParam(&gGrenadeGas_GCParam);
	}
	else if(isMachineGunBefore())
	{
		saveLastGeneralParam(&gMachineGun_GCParam);
	}

	if(PROJECTILE_BULLET == type)
	{
		setNextGeneralParam(&gMachineGun_GCParam);
	}
	else if(PROJECTILE_GRENADE_KILL == type)
	{
		setNextGeneralParam(&gGrenadeKill_GCParam);
	}
	else if(PROJECTILE_GRENADE_GAS == type)
	{
		setNextGeneralParam(&gGrenadeGas_GCParam);
	}
}


static void saveLastGeneralParam(GeneralCorrectionItem * Item)
{
	Item->distance	 = getLastGeneralDistance();
	Item->data.deltaX = getLastGeneralDataX();
	Item->data.deltaY = getLastGeneralDataY();
}


double getLastGeneralDataY()
{
	double value = General[10]*1000 +General[11]*100 + General[12]*10 + General[13];
	if(General[9] >= 0)
		return value;
	else 
		return -value;
}


int getLastGeneralDistance()
{
	return (General[0]*1000 + General[1]*100 + General[2]*10 + General[3]);
}


double getLastGeneralDataX()
{
	double value = General[5]*1000 +General[6]*100 + General[7]*10 + General[8];
	if(General[4] >= 0)
		return value;
	else 
		return -value;
}

void UpdataBoreSight()
{
	if(isMachineGun())
	{
		setNextZeroData(&gMachineGun_ZCTable);
	}
	else if(isGrenadeGas()||isGrenadeKill())
	{
		setNextZeroData(&gGrenadeKill_ZCTable);
	}
	return ;
}


void moveCrossCenter(int x, int y)
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	if(isMachineGun())
	{
		//printf("gun gun gun gun gun   \n");
		borX = gMachineGun_ZCTable.data.deltaX+x;
		//borY = gMachineGun_ZCTable.data.deltaY+(y>>1);
		borY = gMachineGun_ZCTable.data.deltaY+y;
		//printf("x = %d \n",x);
		//printf("y = %d \n",y);
		//printf(" borX = %d\n",borX);
		//printf(" borY = %d\n",borY);
	}
	else
	{
		borX = gGrenadeKill_ZCTable.data.deltaX+x;
		borY = gGrenadeKill_ZCTable.data.deltaY;
	}
	
	//FOVCTRL_updateFovRect(cthis,0,0,borX,(borY>>1));
	#if 1
		if(borX > 0 && borX < 720 && borY >0 && borY < 576)
		{
			FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
		}
	#endif
}


int getCrossX()
{
	return pFovCtrlObj->fovX;
}


int getCrossY()
{
	//return pFovCtrlObj->fovY<<1;
	return pFovCtrlObj->fovY;
}


void updateMoveCross()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	borY = cthis->fovY;
	if(borY > 0 && borY < 576 && borX >0 && borX < 720)
		FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
}



void moveCrossDown()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	//borY = cthis->fovY<<1;
	borY = cthis->fovY;
	borY = (borY + STEPLEN + 576)%576;
	if(borY > 0)
		FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
}


void moveCrossUp()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	//borY = cthis->fovY<<1;
	borY = cthis->fovY;
	borY = (borY - STEPLEN + 576)%576;
	if(borY > 0)
		FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
}


void moveCrossLeft()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	borY = cthis->fovY;
	borX = (borX - STEPLEN + 720)%720;
	if(borX > 0)
		FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
}

void moveCrossRight()
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX;
	borY = cthis->fovY;
	borX = (borX + STEPLEN + 720)%720;
	if(borX < 720)
		FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
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
		//���¾��롢���޲���
		// ���� ����ֵ
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
	//printf("Temparature=%d\n",Temparature);
	//printf("gWeatherTable.Temparature=%d\n",gWeatherTable.Temparature);
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

	value = (value>0)?(value):500;//���δ���þ���ֵ��Ĭ������500��
printf("general[0123] = %d%d%d%d\n",General[0],General[1],General[2],General[3]);
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
printf("general[5678] = %d%d%d%d\n",General[5],General[6],General[7],General[8]);	
	General[9]  = (Item->data.deltaY >= 0)?0:-1;
	value = abs(Item->data.deltaY);
	General[10]  = value/1000%10;
	General[11]  = value/100%10;
	General[12]  = value/10%10;
	General[13] = value%10;
printf("general[10111213] = %d%d%d%d\n",General[10],General[11],General[12],General[13]);	
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
	
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX = cthis->fovX = Item->data.deltaX;
	borY = cthis->fovY = Item->data.deltaY;

	//FOVCTRL_updateFovRect(cthis,0,0,borX,(borY)>>1);
	FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
}



void initilZeroParam(int type)
{
	if(PROJECTILE_BULLET == type)
	{
		setNextZeroData(&gMachineGun_ZCTable);
	}
	else if(PROJECTILE_GRENADE_KILL == type)
	{
		setNextZeroData(&gGrenadeKill_ZCTable);
	}
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


void moveCrossAbs(int x, int y)
{
	CFOV* cthis = pFovCtrlObj;
	int borX,borY;
	borX =  x;
	borY =  y;
	FOVCTRL_updateFovRect(cthis,0,0,borX,borY);
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

int getDisLen()
{
	return DISLEN;
}

void increaseMeasureMul()
{
	if(!SHINE)
		return;
	if(DISLEN<100)
		DISLEN *= 10;
	/*
	if(ShinId > eMeasureDis_Value2)
	{
		OSDCTRL_ItemShow(ShinId);
		ShinId -- ;
	}
	*/
	resetTickNum();
}

void decreaseMeasureMul()
{
	if(!SHINE)
		return;
	if(DISLEN>1)
		DISLEN /= 10;
	/*
	if(ShinId < eMeasureDis_Value4)
	{
		OSDCTRL_ItemShow(ShinId);
		ShinId ++;
	}
	*/
	resetTickNum();
}

void increaseMeasureDis()
{
	int i = ShinId;
	/*
	int flag =0;
	for(i = 0;i<10;i++)
	{	
		if(shine_table[i] == eMeasureDis_Value1)
			flag = 1;
	}
	*/
	if(!SHINE)
		return;

	if(eMeasureDis == i)
	{
		DistanceManual+= DISLEN;
		if(isMachineGun())
		{
			if(DistanceManual >= 1500)
				DistanceManual = 1500;
		}
		else if(isGrenadeKill())
		{
			if(DistanceManual >= 1700)
				DistanceManual = 1700;
		}
		else if(isGrenadeGas())
		{
			if(DistanceManual >= 360)
				DistanceManual = 360;
		}
	}
}


void decreaseMeasureDis()
{
	int i = ShinId;	

	if(!SHINE)
		return;

	if(eMeasureDis == i)	
	{
		DistanceManual -= DISLEN;
		if(DistanceManual <= 0)
			DistanceManual = 0;
	}
}


void saveZeroParam()
{
	if(isMachineGun())
	{	
		gMachineGun_ZCTable.data.deltaX = getCrossX();
		gMachineGun_ZCTable.data.deltaY = getCrossY();
	}
	else if(isGrenadeKill())
	{
		gGrenadeKill_ZCTable.data.deltaX = getCrossX();
		gGrenadeKill_ZCTable.data.deltaY = getCrossY();
	}
	gMachineGun_ZCTable.Angle	     = getMachGunAngleAbs();
	gGrenadeKill_ZCTable.Angle	     = getGrenadeAngleAbs();
}

void saveGeneralParam()
{
	if(isGrenadeKill())
	{
		saveLastGeneralParam(&gGrenadeKill_GCParam);
	}
	else if(isGrenadeGas())
	{
		saveLastGeneralParam(&gGrenadeGas_GCParam);
	}
	else if(isMachineGun())
	{
		saveLastGeneralParam(&gMachineGun_GCParam);
		//putchar(10);
		//printf("gMachineGun_GCParam.data.x=%d\n",getLastGeneralDataX());
		//printf("gMachineGun_GCParam.data.y=%d\n",getLastGeneralDataY());
		
	}
}


void moveLeftFireViewPram()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGenPram_VFLDOXValue0 + (ShinId - eCalibGenPram_VFLDOXValue0+ 47)%48;
}

void moveLeftFireCtrlPram()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGenPram_TimeValue0+ (ShinId - eCalibGenPram_TimeValue0+ 7)%8;
}


void moveRightFireViewPram()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGenPram_VFLDOXValue0 + (ShinId - eCalibGenPram_VFLDOXValue0+ 1)%48;
}

void moveRightFireCtrlPram()
{
	if(!SHINE)
		return;
	OSDCTRL_ItemShow(ShinId);
	ShinId = eCalibGenPram_TimeValue0+ (ShinId - eCalibGenPram_TimeValue0+ 1)%8;
}

void udateMenuItem_Zero_General(PROJECTILE_TYPE type)
{
	
	OSDText_Obj * pTextObj = pCtrlObj->pTextList;

	SDK_ASSERT(pCtrlObj!=NULL);
	if(pCtrlObj->uInit==0)
		return;

	if(pTextObj[eCalibMenu_General].osdInitY == pTextObj[eCursorX].osdInitY)
	{
		Posd[eCalibMenu_General] = CalibGeneralOsd[type];
	}else if(pTextObj[eCalibMenu_Zero].osdInitY == pTextObj[eCursorX].osdInitY)
	{
		//if(type != PROJECTILE_GRENADE_KILL)
		//if(type>=1)
		//	type--;
			Posd[eCalibMenu_Zero] = CalibZeroOsd[type];
	}
}


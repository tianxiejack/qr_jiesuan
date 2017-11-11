
#include "permanentStorage.h"
#include "WeaponCtrl.h"
#include "PositionPort.h"

GeneralCorrectionItem gMachineGun_GCTable[]= 
{
	{0,		{0,0}},
	{100, 	{0,0}},
	{200, 	{0,0}},
	{300, 	{0,0}},
	{400, 	{0,0}},
	{500, 	{0,0}},
	{600, 	{0,0}},
	{700, 	{0,0}},
	{800, 	{0,0}},
	{900, 	{0,0}},
	{1000, 	{0,0}},
	{1100, 	{0,0}},
	{1200, 	{0,0}},
	{1300, 	{0,0}},
	{1400, 	{0,0}},
	{1500, 	{0,0}}
};
GeneralCorrectionItem gMachineGun_GCParam 		={0,		{0,0}};
GeneralCorrectionItem gGrenadeKill_GCParam		={0,		{0,0}};
GeneralCorrectionItem gGrenadeGas_GCParam		={0,		{0,0}};

GeneralCorrectionItem gGrenadeKill_GCTable[]=
{
	{0,		{0,0}},
	{100, 	{0,0}},
	{200, 	{0,0}},
	{300, 	{0,0}},
	{400, 	{0,0}},
	{500, 	{0,0}},
	{600, 	{0,0}},
	{700, 	{0,0}},
	{800, 	{0,0}},
	{900, 	{0,0}},
	{1000, 	{0,0}},
	{1100, 	{0,0}},
	{1200, 	{0,0}},
	{1300, 	{0,0}},
	{1400, 	{0,0}},
	{1500, 	{0,0}},
	{1600, 	{0,0}},
	{1700, 	{0,0}},
	{1791,	{0,0}}
};

GeneralCorrectionItem gGrenadeGas_GCTable[]=
{
		{0,	{0,0}},
		{25, {0,0}},
		{50, {0,0}},
		{75, {0,0}},
		{100, {0,0}},
		{125, {0,0}},
		{150, {0,0}},
		{175, {0,0}},
		{200, {0,0}},
		{225, {0,0}},
		{250, {0,0}},
		{275, {0,0}},
		{300, {0,0}},
		{325, {0,0}},
		{350, {0,0}},
		{364, {0,0}}
};

ZeroCorrectionItem gMachineGun_ZCTable = {500,	{360,288}};
ZeroCorrectionItem gGrenadeKill_ZCTable  = {500,{360,288}};//{500,	{352,288}};
ZeroCorrectionItem gTurret_ZCTable  = {0,{360,288},0};
double gTurret_DP_Angle = 0.0,gMach_DP_Angle = 0.0,gGrenade_DP_Angle = 0.0;
WeatherItem gWeatherTable={15,1013250};


#define  ParameterAddrBase				(0x000000001)

#define ParameterAddrMachGunGC			(ParameterAddrBase)
#define ParameterAddrGrenadeKillGC		(ParameterAddrBase+sizeof(gMachineGun_GCTable)+8)
#define ParameterAddrGrenadeGasGC	 	(ParameterAddrGrenadeKillGC+sizeof(gGrenadeKill_GCTable)+8)
#define ParameterAddrMachGunZC			(ParameterAddrGrenadeGasGC+sizeof(gGrenadeGas_GCTable)+8)
#define ParameterAddrGrenadeKillZC		(ParameterAddrMachGunZC+sizeof(gMachineGun_ZCTable)+8)
#define ParameterAddrWeather			(ParameterAddrGrenadeKillZC+sizeof(gGrenadeKill_ZCTable)+8)
#define ParameterAddrCorrection			(ParameterAddrWeather +sizeof(gWeatherTable)+8)


int ReadParamsFlash()
{	
#if 0
	SetFlashPage(7);
/*	FlashRead(ParameterAddrMachGunGC, (char*)gMachineGun_GCTable, sizeof(gMachineGun_GCTable));
	FlashRead(ParameterAddrGrenadeKillGC, (char*)gGrenadeKill_GCTable, sizeof(gGrenadeKill_GCTable));
	FlashRead(ParameterAddrGrenadeGasGC, (char*)gGrenadeGas_GCTable, sizeof(gGrenadeGas_GCTable));
*/
	FlashRead(ParameterAddrMachGunZC, (char*)&gMachineGun_ZCTable, sizeof(gMachineGun_ZCTable));
	FlashRead(ParameterAddrGrenadeKillZC, (char*)&gGrenadeKill_ZCTable, sizeof(gGrenadeKill_ZCTable));
	FlashRead(ParameterAddrWeather, (char*)&gWeatherTable, sizeof(gWeatherTable));
	FlashRead(ParameterAddrCorrection, (char*)&gMachineGun_GCParam, sizeof(gMachineGun_GCParam));
	FlashRead(ParameterAddrCorrection+sizeof(gMachineGun_GCParam), (char*)&gGrenadeGas_GCParam, sizeof(gMachineGun_GCParam));
	FlashRead(ParameterAddrCorrection+2*sizeof(gMachineGun_GCParam), (char*)&gGrenadeKill_GCParam, sizeof(gMachineGun_GCParam));
	// read struct from mem
#endif
	return 0;
}

int WriteParamsFlash()
{
#if 0
	SetFlashPage(7);
	if((FlashErase(7, FLASH_BASE,  FLASH_SECTORSIZE8_70) == 0xffffffff)||(TestFlashBlank(FLASH_BASE, FLASH_SECTORSIZE8_70) == 0))
	{
		return -1;
	}
/*	if(FlashWrite((char*)gMachineGun_GCTable, ParameterAddrMachGunGC, sizeof(gMachineGun_GCTable)) == 0xffffffff)
		return -2;
	if(FlashWrite((char*)gGrenadeKill_GCTable, ParameterAddrGrenadeKillGC, sizeof(gGrenadeKill_GCTable)) == 0xffffffff)
		return -3;
	if(FlashWrite((char*)gGrenadeGas_GCTable, ParameterAddrGrenadeGasGC, sizeof(gGrenadeGas_GCTable)) == 0xffffffff)
		return -4;
*/
	if(FlashWrite((char*)&gMachineGun_ZCTable, ParameterAddrMachGunZC, sizeof(gMachineGun_ZCTable)) == 0xffffffff)
		return -5;
	if(FlashWrite((char*)&gGrenadeKill_ZCTable, ParameterAddrGrenadeKillZC, sizeof(gGrenadeKill_ZCTable)) == 0xffffffff)
		return -6;
	if(FlashWrite((char*)&gWeatherTable, ParameterAddrWeather, sizeof(gWeatherTable)) == 0xffffffff)
		return -7;
	if(FlashWrite((char*)&gMachineGun_GCParam, ParameterAddrCorrection, sizeof(gMachineGun_GCParam)) == 0xffffffff)
		return -8;
	if(FlashWrite((char*)&gGrenadeGas_GCParam, ParameterAddrCorrection+sizeof(gMachineGun_GCParam), sizeof(gMachineGun_GCParam)) == 0xffffffff)
		return -9;
	if(FlashWrite((char*)&gGrenadeKill_GCParam, ParameterAddrCorrection+2*sizeof(gMachineGun_GCParam), sizeof(gMachineGun_GCParam)) == 0xffffffff)
		return -10;

	AVTCTRL_saveConfig(pAvtCtrlObj);
#endif
	return 0;
}
int getGrenadeSize()
{
	return sizeof(gGrenadeKill_GCTable)/sizeof(gGrenadeKill_GCTable[0]);
}
int getGrenadeGasSize()
{
	return sizeof(gGrenadeGas_GCTable)/sizeof(gGrenadeGas_GCTable[0]);
}
int getMachGunSize()
{
	return sizeof(gMachineGun_GCTable)/sizeof(gMachineGun_GCTable[0]);
}

void setGeneralCorrection(double distance, PROJECTILE_TYPE type, GeneralCorrection data)
{
	GeneralCorrectionItem *pGCTable;
	int TableSize;
	int i;
	if(type == PROJECTILE_BULLET)
	{
		pGCTable = gMachineGun_GCTable;
		TableSize = sizeof(gMachineGun_GCTable)/sizeof(gMachineGun_GCTable[0]);
	}else if(type == PROJECTILE_GRENADE_KILL)
	{
		pGCTable = gGrenadeKill_GCTable;
		TableSize = sizeof(gGrenadeKill_GCTable)/sizeof(gGrenadeKill_GCTable[0]);
	}else if(type == PROJECTILE_GRENADE_GAS)
	{
		pGCTable = gGrenadeGas_GCTable;
		TableSize = sizeof(gGrenadeGas_GCTable)/sizeof(gGrenadeGas_GCTable[0]);
	}

	for(i=0; i<TableSize; i++)
	{
		if(distance == pGCTable[i].distance)
		{
			pGCTable[i].data = data;
			break;		
		}
	}

}

GeneralCorrection getGeneralCorrection(double distance, PROJECTILE_TYPE type)
{
	GeneralCorrectionItem *pGCTable;
	int TableSize;
	int i;
	GeneralCorrection data={0.0,0.0};
	if(type == PROJECTILE_BULLET)
	{
		pGCTable = gMachineGun_GCTable;
		TableSize = sizeof(gMachineGun_GCTable)/sizeof(gMachineGun_GCTable[0]);
	}else if(type == PROJECTILE_GRENADE_KILL)
	{
		pGCTable = gGrenadeKill_GCTable;
		TableSize = sizeof(gGrenadeKill_GCTable)/sizeof(gGrenadeKill_GCTable[0]);
	}else if(type == PROJECTILE_GRENADE_GAS)
	{
		pGCTable = gGrenadeGas_GCTable;
		TableSize = sizeof(gGrenadeGas_GCTable)/sizeof(gGrenadeGas_GCTable[0]);
	}

	for(i=0; i<TableSize; i++)
	{
		if(distance == pGCTable[i].distance)
		{
			data = pGCTable[i].data;
			break;
		}
	}
	return data;
}

GeneralCorrection getGeneralCorrectionTheta(double distance, PROJECTILE_TYPE type)
{
	GeneralCorrection ret = getGeneralCorrection(distance, type);

	if(0.0 != distance)
	{
		ret.deltaX = BIAS2MIL(ret.deltaX/100,distance);
		ret.deltaY = BIAS2MIL(ret.deltaY/100,distance);
	}
	return ret;
}

TankTheta getTankTheta()
{
 	TankTheta th;
	double X = getPlatformPositionX();
	double Y = getPlatformPositionY();
	th.deltaX = DEGREE2MIL(X);
	th.deltaY = DEGREE2MIL(Y);
	return th;
}

int angleToOffset(double angleMil)
{
	int ret = 0;
	Fov_Type fov = getFovState();
	switch(fov)
	{
	// small FOV=5.6x4.2 degree; 704x576	//4.3 x 3.3
		case FOV_SMALL:
			ret = MIL2DEGREE(angleMil)*7200/43;
			break;
	// large FOV=16x12 degree; 704x576		//12 x 9
		case FOV_LARGE:
			ret = MIL2DEGREE(angleMil)*720/12;
			break;
		default:

			break;
	}

	return ret;
}

double offsetToAngle(double pixels)
{
	double ret = 0.0;
	Fov_Type fov = getFovState();
	switch(fov)
	{
	// small FOV=5.6x4.2 degree; 720x576
		case FOV_SMALL:
			ret = DEGREE2MIL(pixels*56/7200);
			break;
	// large FOV=16x12 degree; 720x576
		case FOV_LARGE:
			ret = DEGREE2MIL(pixels*16/720);
			break;
		default:

			break;
	}

	return ret;
}


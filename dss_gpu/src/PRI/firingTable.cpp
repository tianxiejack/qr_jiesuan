#include "firingTable.h"

#define NULL 0
//\u5ea6 + \u5206 + \u5bc6\u4f4d ; \u6362\u7b97\u6210\u5bc6\u4f4d
#define CALC_MIL(degree,minute,mil) (((degree)+(minute)/60.0)*6000/360 + (mil))


//5.8\u673a\u67aa\u5c04\u8868\uff0c\u6309\u8ddd\u79bb\u503c\u5347\u5e8f\u6392\u5217
static const RowItems gMachineGunTable[]= {
	//{0,		0.0,					0.00,	0},
	{100, 	CALC_MIL(0,8,2.3), 		0.12,	0/*BIAS2MIL(0.01,100)*/},
	{200, 	CALC_MIL(0,10,2.7), 	0.25,	0/*BIAS2MIL(0.02,200)*/},
	{300, 	CALC_MIL(0,12,3.5), 	0.39,	RADIAN2MIL(0.000167)/*BIAS2MIL(0.05,300)*/},
	{400, 	CALC_MIL(0,16,4.4), 	0.55,	RADIAN2MIL(0.0003)/*BIAS2MIL(0.12,400)*/},
	{500, 	CALC_MIL(0,20,5.4), 	0.73,	RADIAN2MIL(0.00038)/*BIAS2MIL(0.19,500)*/},
	{600, 	CALC_MIL(0,24,6.7), 	0.93,	RADIAN2MIL(0.00045)/*BIAS2MIL(0.27,600)*/},
	{700, 	CALC_MIL(0,30,8.2), 	1.15,	RADIAN2MIL(0.00051)/*BIAS2MIL(0.36,700)*/},
	{800, 	CALC_MIL(0,36,10.0), 	1.41,	RADIAN2MIL(0.00061)/*BIAS2MIL(0.49,800)*/},
	{900, 	CALC_MIL(0,44,12.2), 	1.70,	RADIAN2MIL(0.0007)/*BIAS2MIL(0.63,900)*/},
	{1000, 	CALC_MIL(0,53,14.8),	2.01,	RADIAN2MIL(0.00079)/*BIAS2MIL(0.79,1000)*/},
	{1100, 	CALC_MIL(1,4,17.9),		2.35,	RADIAN2MIL(0.00087)/*BIAS2MIL(0.96,1100)*/},
	{1200, 	CALC_MIL(1,17,21.3), 	2.71,	RADIAN2MIL(0.00095)/*BIAS2MIL(1.14,1200)*/},
	{1300, 	CALC_MIL(1,31,25.2), 	3.08,	RADIAN2MIL(0.00103)/*BIAS2MIL(1.34,1300)*/},
	{1400, 	CALC_MIL(1,46,29.5), 	3.47,	RADIAN2MIL(0.0011)/*BIAS2MIL(1.54,1400)*/},
	{1500, 	CALC_MIL(2,3,34.3), 	3.89,	RADIAN2MIL(0.00116)/*BIAS2MIL(1.75,1500)*/}
		
};

//35\u69b4\u5f39\u5c04\u8868
//\u69b4\u5f39\u5c04\u8868,\u6309\u8ddd\u79bb\u503c\u5347\u5e8f\u6392\u5217
//todo:\u65f6\u95f4\u76ee\u524d\u6309190\u7c73\u6bcf\u79d2\u521d\u901f\u4f30\u7b97\uff0c\u7b49\u5b9e\u6d4b\u7ea0\u6b63
static const RowItems gGrenadeTable[]= {
	//{0,			0,						0.0,	0.0},
	{100,		CALC_MIL(1,26,0.0),		0.97,	0.26},
	{200,		CALC_MIL(2,14,0.0),		1.51,	0.52},
	{300,		CALC_MIL(3,4,0.0),		2.07,	0.81},
	{400,		CALC_MIL(3,57,0.0),		2.67,	1.10},
	{500,		CALC_MIL(4,54,0.0),		3.31,	1.29},
	{600,		CALC_MIL(5,53,0.0),		3.97,	1.75},
	{700,		CALC_MIL(7,15,0.0),		4.89,	2.11},
	{800,		CALC_MIL(8,29,0.0),		5.72,	2.51},
	{900,		CALC_MIL(9,50,0.0),		6.62,	2.94},
	{1000, 		CALC_MIL(11,19,0.0),		7.61,	3.42},
	{1100,		CALC_MIL(12,56,0.0),		8.68,	3.94},
	{1200,		CALC_MIL(14,43,0.0),		9.85,	4.55},
	{1300,		CALC_MIL(16,44,0.0),		11.16,5.20},
	{1400,		CALC_MIL(19,2,0.0),		12.64,6.00},
	{1500,		CALC_MIL(21,43,0.0),		14.35,6.97},
	{1600,		CALC_MIL(25,2,0.0),		16.41,8.17},
	{1700,		CALC_MIL(29,32,0.0),		19.11,9.91},
	{1791,		CALC_MIL(40,3,0.0),		24.95,14.92}

};

//\u69b4\u5f39\u74e6\u65af\u5c04\u8868\uff0c\u6309\u8ddd\u79bb\u503c\u5347\u5e8f\u6392\u5217,\u5ffd\u7565\u4e86\u964d\u5e8f\u90e8\u5206\u7684\u5c04\u8868
//\u504f\u6d41\u4e0d\u8ba1
static const RowItems gGrenadeTableGas[]= {
	//{0,		 0 ,		0.0 ,		0},
	//{25,	 19.16,		0.32,		0},
	{50,	 38.94,		0.64,		0},
	{75,	 60.42,		1.01,		0},
	{100,	 83.33,		1.38,		0},
	{125,	 107.95,	1.77,		0},
	{150,	 134.50,	2.17,		0},
	{175,	 163.34,	2.61,		0},
	{200,	 194.87,	3.08,		0},
	{225,	 229.66,	3.58,		0},
	{250,	 268.60,	4.13,		0},
	{275,	 313.10,	4.73,		0},
	{300,	 365.56,	5.42,		0},
	{325,	 431.17,	6.24,		0},
	{350,	 527.64,	7.35,		0},
	{364,	 675,		8.91,		0}
};
static FiringTable gFiringTable_MachineGun = {
		sizeof(gMachineGunTable)/sizeof(gMachineGunTable[0]), gMachineGunTable
};
static FiringTable gFiringTable_Grenade = {
		sizeof(gGrenadeTable)/sizeof(gGrenadeTable[0]), gGrenadeTable
};
static FiringTable gFiringTable_GrenadeGas = {
		sizeof(gGrenadeTableGas)/sizeof(gGrenadeTableGas[0]), gGrenadeTableGas
};

FiringTable* getFiringTable(PROJECTILE_TYPE type)
{
	FiringTable * table = NULL;
	switch(type){
		case PROJECTILE_BULLET:
			table = &gFiringTable_MachineGun;
			break;
		case PROJECTILE_GRENADE_KILL:
			table = & gFiringTable_Grenade;
			break;
		case PROJECTILE_GRENADE_GAS:
			table = & gFiringTable_GrenadeGas;
			break;
		default:
			break;
		}
	return table;
}


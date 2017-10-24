
#include "firingCtrl.h"
#include "trajectoryCalc.h"
#include "PolyNomialCalc.h"
#include "LaserPort.h"
#include "MachGunPort.h"
#include <math.h>
//#include "OSDCtrl.h"
#include "permanentStorage.h"

enum {
	DIP_ANGLE_40,
	DIP_ANGLE_35,
	DIP_ANGLE_30,
	DIP_ANGLE_25,
	DIP_ANGLE_20,
	DIP_ANGLE_15,
	DIP_ANGLE_10,
	DIP_ANGLE_5,
	DIP_ANGLE_0,
	DIP_ANGLE_minus_5,
	DIP_ANGLE_minus_10,
	DIP_ANGLE_minus_15,
	DIP_ANGLE_minus_20,
	DIP_ANGLE_minus_25,
	DIP_ANGLE_minus_30,
	
	COUNT_DIP_ANGLE
};

typedef struct _Angle2Dist{
	int deltaDist[COUNT_DIP_ANGLE];
} Angle2Dist; 
typedef struct _DipCorrectionRow {
	int distance;
	Angle2Dist	deltaAtDipAngle;
} DipCorrectionRow;

typedef struct _ParamType{
	double x;
	double y;
}ParamType;
typedef struct _ParamMid{
	ParamType sumCalib;
	ParamType visualCalib;
	ParamType trajectoryCalc;
	ParamType trunnionCalib;
	ParamType correctionCalib;
}PARAMS_TYPE;
PARAMS_TYPE ParamInMid={{0.0,0.0},{0.0,0.0},{0.0,0.0},{0.0,0.0},{0.0,0.0}};
static const int DipAngleTable[] = {40,35,30,25,20,15,10,5,0,-5,-10,-15,-20,-25,-30 };
static const DipCorrectionRow gDipCorrectionTable[] = 
{
	{	0	,{	0,		0,		0,		0,		0,		0,		0,		0,		0	,	0	,	0	,	0	,	0	,	0	,	0	}},
	{	100	,{	-22	,	-17	,	-13	,	-9	,	-6	,	-3	,	-1	,	0	,	0	,	0	,	-1	,	-3	,	-6	,	-9	,	-13	}},          
	{	200	,{	-42	,	-32	,	-24	,	-17	,	-11	,	-6	,	-3	,	-1	,	0	,	-1	,	-3	,	-6	,	-11	,	-17	,	-24	}},      
	{	300	,{	-60	,	-46	,	-34	,	-23	,	-15	,	-8	,	-4	,	-1	,	0	,	-1	,	-4	,	-8	,	-15	,	-24	,	-34	}},      
	{	400	,{	-76	,	-58	,	-42	,	-29	,	-19	,	-10	,	-5	,	-1	,	0	,	-1	,	-5	,	-11	,	-19	,	-29	,	-42	}},      
	{	500	,{	-80	,	-68	,	-49	,	-34	,	-22	,	-12	,	-5	,	-1	,	0	,	-1	,	-5	,	-12	,	-22	,	-34	,	-49	}},      
	{	600	,{	-100	,	-76	,	-58	,	-35	,	-24	,	-14	,	-6	,	-2	,	0	,	-1	,	-6	,	-13	,	-24	,	-38	,	-55	}},    
	{	700	,{	-109	,	-83	,	-60	,	-42	,	-26	,	-15	,	-7	,	-2	,	0	,	-1	,	-6	,	-14	,	-26	,	-40	,	-59	}},    
	{	800	,{	-116	,	-88	,	-64	,	-44	,	-28	,	-16	,	-7	,	-2	,	0	,	-1	,	-6	,	-15	,	-27	,	-42	,	-62	}},    
	{	900	,{	-122	,	-92	,	-67	,	-46	,	-30	,	-17	,	-8	,	-2	,	0	,	-1	,	-6	,	-15	,	-27	,	-43	,	-64	}},    
	{	1000	,{	-127	,	-96	,	-70	,	-49	,	-31	,	-18	,	-8	,	-2	,	0	,	-1	,	-7	,	-16	,	-28	,	-45	,	-66	}},  
	{	1100	,{	-134	,	-102	,	-74	,	-52	,	-33	,	-19	,	-9	,	-2	,	0	,	-2	,	-7	,	-16	,	-30	,	-48	,	-70	}},
	{	1200	,{	-142	,	-108	,	-79	,	-55	,	-35	,	-20	,	-9	,	-2	,	0	,	-2	,	-8	,	-18	,	-32	,	-51	,	-75	}},
	{	1300	,{	-151	,	-114	,	-84	,	-58	,	-37	,	-21	,	-10	,	-3	,	0	,	-2	,	-8	,	-19	,	-35	,	-55	,	-80	}},
	{	1400	,{	-159	,	-121	,	-88	,	-61	,	-39	,	-22	,	-10	,	-3	,	0	,	-2	,	-9	,	-21	,	-38	,	-59	,	-86	}},
	{	1500	,{	-168	,	-127	,	-93	,	-64	,	-41	,	-23	,	-10	,	-2	,	0	,	-3	,	-10	,	-23	,	-41	,	-64	,	-93	}}
};

static double gGrenadeDestTheta = 0.0;
static Bool Use_interPolation = TRUE;

void setUseInterPolation(void)
{
	Use_interPolation = !Use_interPolation;
	if(Use_interPolation)
		printf("trajectoryCalc\n");
	else
		printf("polynomialCalc\n");
}
float getSumCalibX(void)
{
	return ParamInMid.sumCalib.x;
}
float getSumCalibY(void)
{
	return ParamInMid.sumCalib.y;
}
float getVisualCalibX(void)
{
	return ParamInMid.visualCalib.x;
}
float getVisualCalibY(void)
{
	return ParamInMid.visualCalib.y;
}
float getTrajectCalcX(void)
{
	return ParamInMid.trajectoryCalc.x;
}
float getTrajectCalcY(void)
{
	return ParamInMid.trajectoryCalc.y;
}
float getTrunionCalibX(void)
{
	return ParamInMid.trunnionCalib.x;
}
float getTrunionCalibY(void)
{
	return ParamInMid.trunnionCalib.y;
}
float getCorrectionCalibX(void)
{
	return ParamInMid.correctionCalib.x;
}
float getCorrectionCalibY(void)
{
	return ParamInMid.correctionCalib.y;
}
int getParamBuf(BYTE *buf)
{
	if(NULL == buf)
		return -1;
	buf[0] = ((int)(ParamInMid.visualCalib.x *100)&0xFF00)>>8;
	buf[1] = (int)(ParamInMid.visualCalib.x *100)&0x00FF;
	buf[2] = ((int)(ParamInMid.visualCalib.y *100)&0xFF00)>>8;
	buf[3] = (int)(ParamInMid.visualCalib.y *100)&0x00FF;
	buf[4] = ((int)(ParamInMid.trajectoryCalc.x *100)&0xFF00)>>8;
	buf[5] = (int)(ParamInMid.trajectoryCalc.x *100)&0x00FF;
	buf[6] = ((int)(ParamInMid.trajectoryCalc.y *100)&0xFF00)>>8;
	buf[7] = (int)(ParamInMid.trajectoryCalc.y *100)&0x00FF;
	buf[8] = ((int)(ParamInMid.trunnionCalib.x *100)&0xFF00)>>8;
	buf[9] = (int)(ParamInMid.trunnionCalib.x *100)&0x00FF;
	buf[10]= ((int)(ParamInMid.trunnionCalib.y *100)&0xFF00)>>8;
	buf[11]= (int)(ParamInMid.trunnionCalib.y *100)&0x00FF;
	return 0;
}


void setGrenadeDestTheta(double theta)
{
	gGrenadeDestTheta = theta;
}

double getGrenadeDestTheta()
{
	return gGrenadeDestTheta;
}

enum {
		K_minus_One,
		K,
		K_plus_One,
		K_COUNT
	};
	//L(k-1)=(x-x(k))(x-x(k+1))/(x(k-1)-x(k))(x(k-1)-x(k+1))
	//L(k)= (x-x(k-1))(x-x(k+1))/(x(k)-x(k-1))(x(k)-x(k+1))
	//L(k+1)=(x-x(k-1))(x-x(k))/(x(k+1)-x(k-1))(x(k+1)-x(k))
#define L_K_minus1(x, xk, xk_minus1, xk_plus1) ((x-xk)*(x-xk_plus1)/((xk_minus1-xk)*(xk_minus1-xk_plus1)))
#define L_K(x, xk, xk_minus1, xk_plus1) 	((x-xk_minus1)*(x-xk_plus1)/((xk-xk_minus1)*(xk-xk_plus1)))
#define L_K_plus1(x, xk, xk_minus1, xk_plus1)  ((x- xk_minus1)*(x-xk)/((xk_plus1-xk_minus1)*(xk_plus1-xk)))
// return distance correction for Machine gun
// use interpolation for both dipAngle and distance
static  int getMachineGunDipCorrection(double dipAngle, double distance, double*delta)
{
	static const int gDiptableSize = sizeof(gDipCorrectionTable)/sizeof(gDipCorrectionTable[0]);
	static const int angleTableSize = sizeof(DipAngleTable)/sizeof(DipAngleTable[0]);
	DipCorrectionRow *pCorrectionRow = NULL;
	DipCorrectionRow *L[K_COUNT];
	DipCorrectionRow   inputRow;
	int L_angles[K_COUNT];
	int i,j, angleIndex = -1;
	double L_Kx,L_K_MINUS_1x, L_K_PLUS_1x;
	
	//dip angle is between -30 and 40 degree.
	//distance less than 1500 m
	if(dipAngle > 40 || dipAngle < -30 || distance < 0 || distance > 1500.0)
		return -1;// invalid parameters
	//查询表 k-1, k, k+1 列
	for(i = 0; i < gDiptableSize ; i++)
	{
		if(distance == gDipCorrectionTable[i].distance)
		{
			//找到distance 吻合表，直接use row
			pCorrectionRow = (DipCorrectionRow*) &gDipCorrectionTable[i];
			break;
		}
		
		if(distance < gDipCorrectionTable[i].distance)
			break; // found k or k-1
	}
	if(pCorrectionRow != NULL){
		// use the exact matching row;
		inputRow = *pCorrectionRow;
	}
	else
	{

			if(i==1)//临界条件1
			{
				L[K_minus_One] = (DipCorrectionRow*)&gDipCorrectionTable[0];
				L[K] = (DipCorrectionRow*)&gDipCorrectionTable[1];
				L[K_plus_One] = (DipCorrectionRow*)&gDipCorrectionTable[2];
			}
			else if(i == gDiptableSize - 1)//临界条件2
			{
				L[K_minus_One] =(DipCorrectionRow*) &gDipCorrectionTable[i-2];
				L[K] =(DipCorrectionRow*) &gDipCorrectionTable[i-1];
				L[K_plus_One] =(DipCorrectionRow*) &gDipCorrectionTable[i];
			}
			else
			{
				if(distance*2 <= (gDipCorrectionTable[i].distance + gDipCorrectionTable[i-1].distance))
				{	// close to table[i-1]
					L[K_minus_One] = (DipCorrectionRow*)&gDipCorrectionTable[i-2];
					L[K] = (DipCorrectionRow*)&gDipCorrectionTable[i-1];
					L[K_plus_One] = (DipCorrectionRow*)&gDipCorrectionTable[i];
				}
				else 
				{	// close to table[i]
					L[K_minus_One] = (DipCorrectionRow*)&gDipCorrectionTable[i-1];
					L[K] =(DipCorrectionRow*) &gDipCorrectionTable[i];
					L[K_plus_One] =(DipCorrectionRow*)&gDipCorrectionTable[i+1];
				}
			}


	}
// check againt dip angles-----------------------------------------------------
	for(j=0; j<angleTableSize; j++){
		if(dipAngle == DipAngleTable[j]){
			angleIndex = j;
			break;
		}
		if(dipAngle >DipAngleTable[j]){
			break;
		}
	}

	if(angleIndex >= 0){
		// found a matching angle value
		
		if(pCorrectionRow != NULL){// and a matching distance value is directly found
			*delta = pCorrectionRow->deltaAtDipAngle.deltaDist[angleIndex];
		}else{	// do distance interpolation
			L_Kx = L_K(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);
			L_K_MINUS_1x = L_K_minus1(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);
			L_K_PLUS_1x = L_K_plus1(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);

			 	*delta = L[K_minus_One]->deltaAtDipAngle.deltaDist[angleIndex]  * L_K_MINUS_1x 
				+ L[K]->deltaAtDipAngle.deltaDist[angleIndex] * L_Kx
				+ L[K_plus_One]->deltaAtDipAngle.deltaDist[angleIndex] * L_K_PLUS_1x;
		}
	}else{
		if(j==1)//临界条件1
		{
			L_angles[K_minus_One] = 0;
			L_angles[K] = 1;
			L_angles[K_plus_One] = 2;
		}
		else if(j == angleTableSize - 1)//临界条件2
		{
			L_angles[K_minus_One] =j-2;
			L_angles[K] = j-1;
			L_angles[K_plus_One] = j;
		}
		else
		{
			if(dipAngle*2 >= (DipAngleTable[j]+ DipAngleTable[j-1]))
			{	// close to table[j-1]
				L_angles[K_minus_One] = j-2;
				L_angles[K] = j-1;
				L_angles[K_plus_One] = j;
			}else 
			{	// close to table[j]
				L_angles[K_minus_One] =j-1;
				L_angles[K] = j;
				L_angles[K_plus_One] =j+1;
			}
		}

		if(pCorrectionRow != NULL){
			//directly go to angle interpolation
		}else{
			// double interpolation: 
			//1.distance interpolation 
			L_Kx = L_K(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);
			L_K_MINUS_1x = L_K_minus1(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);
			L_K_PLUS_1x = L_K_plus1(distance, L[K]->distance, L[K_minus_One]->distance, L[K_plus_One]->distance);			
			for(j= L_angles[K_minus_One]; j <= L_angles[K_plus_One]; j ++){
				inputRow.deltaAtDipAngle.deltaDist[j] = L[K_minus_One]->deltaAtDipAngle.deltaDist[j]  * L_K_MINUS_1x 
										+ L[K]->deltaAtDipAngle.deltaDist[j] * L_Kx
										+ L[K_plus_One]->deltaAtDipAngle.deltaDist[j] * L_K_PLUS_1x;
			}
			inputRow.distance = distance;
			pCorrectionRow = &inputRow;

			//2.fall to angle interpolation:
		}
		//angle interpolation:
		L_Kx = L_K(dipAngle, DipAngleTable[L_angles[K]], DipAngleTable[L_angles[K_minus_One]], DipAngleTable[L_angles[K_plus_One]]);
		L_K_MINUS_1x = L_K_minus1(dipAngle, DipAngleTable[L_angles[K]], DipAngleTable[L_angles[K_minus_One]], DipAngleTable[L_angles[K_plus_One]]);
		L_K_PLUS_1x = L_K_plus1(dipAngle, DipAngleTable[L_angles[K]], DipAngleTable[L_angles[K_minus_One]], DipAngleTable[L_angles[K_plus_One]]);

		*delta = pCorrectionRow->deltaAtDipAngle.deltaDist[L_angles[K_minus_One] ]  * L_K_MINUS_1x 
			+ pCorrectionRow->deltaAtDipAngle.deltaDist[L_angles[K] ] * L_Kx
			+ pCorrectionRow->deltaAtDipAngle.deltaDist[L_angles[K_plus_One] ] * L_K_PLUS_1x;
	}
	return 0;
	
}

//
static double getGrenadeDipAngleCorrection(double dipAngle)
{
// 在没有修正量时，暂时用实测角度代替
	return RADIAN2MIL(dipAngle);
}

//视差修正
static int visualCorrection(FiringInputs *input, CorrectionDelta* output)
{
	static const int ZeroPointDistance = 500; //getZeroDistance(input->ProjectileType);
	static const double XGapLaser_to_MachineGun = 0.000;//0.005;
	static const double YGapLaser_to_MachineGun = 0.000;//0.01;
	static const double XGapLaser_to_grenade = 1.000;//0.405;
	static const double YGapLaser_to_grenade = 0.000;//0.01;

	double zeroPointThetaX, zeroPointThetaY, expectedThetaX, expectedThetaY;
	if(input->ProjectileType == PROJECTILE_BULLET)
	{
		zeroPointThetaX = XGapLaser_to_MachineGun/ZeroPointDistance;
		zeroPointThetaY = YGapLaser_to_MachineGun/ZeroPointDistance;

		expectedThetaX = XGapLaser_to_MachineGun/input->TargetDistance;
		expectedThetaY = YGapLaser_to_MachineGun/input->TargetDistance;
	}
	else if(input->ProjectileType == PROJECTILE_GRENADE_KILL || 
		input->ProjectileType == PROJECTILE_GRENADE_GAS)
	{
		zeroPointThetaX = XGapLaser_to_grenade/ZeroPointDistance;
		zeroPointThetaY = YGapLaser_to_grenade/ZeroPointDistance;

		expectedThetaX = XGapLaser_to_grenade/input->TargetDistance;
		expectedThetaY = YGapLaser_to_grenade/input->TargetDistance;
		
	}
	
	output->DeltaThetaX = RADIAN2MIL(zeroPointThetaX - expectedThetaX);
	output->DeltaThetaY = RADIAN2MIL(zeroPointThetaY - expectedThetaY);

	return 0;
}

static int trunnionCorrection(FiringInputs *input, FiringOutputs *inputOffsets, CorrectionDelta* output)
{
	double ng =  inputOffsets->BiasAngle + inputOffsets->correctionData.deltaX;

	//epselon =  a + delta e4

	double epselon = inputOffsets->correctionData.deltaY + inputOffsets->AimElevationAngle;
	

	TankTheta tankTht = getTankTheta();

	double theta = acos(cos(MIL2RADIAN(tankTht.deltaX))*cos(MIL2RADIAN(tankTht.deltaY)));
	
	output->DeltaThetaX = ng*cos(theta) + epselon*sin(theta);
	output->DeltaThetaY = epselon*cos(theta) - ng*sin(theta);		//+

	return 0;
}

int FiringCtrl( FiringInputs *input, FiringOutputs* output)
{
	CorrectionDelta visualCalib, trunnionCalib;
	int ret;
	double  thetaDip ;
	TankTheta tankTht = getTankTheta();
//计算高低角，校正机枪射表距离值或榴弹瞄准角
	thetaDip = asin(sin(MIL2RADIAN(tankTht.deltaX)) * sin(MIL2RADIAN(input->TurretDirectionTheta)) + 
					sin(MIL2RADIAN(tankTht.deltaY)) * cos (MIL2RADIAN(input->TurretDirectionTheta)));

//机枪高低角导致距离修正
	if(PROJECTILE_BULLET == input->ProjectileType)
	{
		double distanceDelta = 0.0;
		if(0 == (ret = getMachineGunDipCorrection(RADIAN2DEGREE(thetaDip)+getMachGunAngle(), input->TargetDistance ,&distanceDelta)))
		{
			input->TargetDistance = input->TargetDistance + distanceDelta;
		}
		else
		{
			//todo: output ret log
		}
		
	}
//计算提前量
	if(Use_interPolation)
		ret = trajectoryCalc( input, output);//计算射击诸元，得出飞行时间、提前量等
	else
	 	ret = polynomialCalc(input, output);

	if(ret < 0)
		return ret;

	memset((void *)&ParamInMid,0,sizeof(ParamInMid));
	ParamInMid.correctionCalib.x= output->correctionData.deltaX;
	ParamInMid.correctionCalib.y= output->correctionData.deltaY;
	
	visualCorrection(input, &visualCalib);
	ParamInMid.visualCalib.x 	= visualCalib.DeltaThetaX;
	ParamInMid.visualCalib.y  	= visualCalib.DeltaThetaY;

	trunnionCorrection(input, output, &trunnionCalib);

	ParamInMid.trajectoryCalc.x	= output->AimOffsetThetaX;
	ParamInMid.trajectoryCalc.y	= output->AimOffsetThetaY;
	ParamInMid.trunnionCalib.x 	= trunnionCalib.DeltaThetaX;
	ParamInMid.trunnionCalib.y 	= trunnionCalib.DeltaThetaY;

	output->AimOffsetThetaX = visualCalib.DeltaThetaX + output->AimOffsetThetaX + trunnionCalib.DeltaThetaX;
	output->AimOffsetThetaY = visualCalib.DeltaThetaY + output->AimOffsetThetaY + trunnionCalib.DeltaThetaY;

	
	if(PROJECTILE_GRENADE_KILL == input->ProjectileType || PROJECTILE_GRENADE_GAS == input->ProjectileType)
	{
		output->AimOffsetThetaY = output->AimOffsetThetaY + getGrenadeDipAngleCorrection(thetaDip);
	}
	ParamInMid.sumCalib.x 	= output->AimOffsetThetaX;
	ParamInMid.sumCalib.y  	= output->AimOffsetThetaY;

	output->AimOffsetX = ANGLE_TO_OFFSET(output->AimOffsetThetaX);
	output->AimOffsetY = ANGLE_TO_OFFSET(output->AimOffsetThetaY);


	return 0;
}



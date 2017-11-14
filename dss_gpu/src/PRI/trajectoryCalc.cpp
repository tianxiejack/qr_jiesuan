#include "permanentStorage.h"
#include "trajectoryCalc.h"
#include "firingTable.h"
#include "assert.h"
#include "stdio.h"

#define NULL 0
/*
	通过射击诸元控制块传过来的数据输入值
	使用激光测距得到的距离，取相关所有传感器内容，计算当前武器状态偏差，
		  	同时根据距离、角度、温度、气压、时间等计算射表诸元
		  	使用二次插值法，算出射击动作过程需要的射击角度、射击周期、弹种等选择
		  	影响:
		  		气压、温度对距离修正
		  		气压、温度对瞄准角修正
		  		瞄准角对距离修正

		  		密位修正 怎么做?(1 mil = 0.06°)
		  		
		  	5.8  100 ~ 1500
		  	35榴 10  ~ 360
		  	35杀 100 ~ 1700
		  		
	计算出当前跟踪的角速度，使用角速度*时间得出角度，再利用视场角转换为像素差值AimOffsetX、AimOffsetY
	
*/


//--------------------start of quadra-interpolation---------------------




//二次插值法
//pL【3】:与自变量相邻的3个点
//inputDistance:自变量
//type:弹种
//outputAngle:输出的角度结果
//outputDuration:输出的延时结果
//outputBias:输出的偏流结果
//outputCorrection:输出的综修结果

static void Quadratic_Interpolation(RowItems *pL[K_COUNT], double inputDistance,PROJECTILE_TYPE type , 
					double *outputAngle, double *outputDuration,double *outputBias,
					GeneralCorrection *outputCorrection)
{
	double L_Kx,L_K_MINUS_1x, L_K_PLUS_1x;
	GeneralCorrection correction_K, correction_K_minus1, correction_K_plus1;
	//L(k-1)=(x-x(k))(x-x(k+1))/(x(k-1)-x(k))(x(k-1)-x(k+1))
	//L(k)= (x-x(k-1))(x-x(k+1))/(x(k)-x(k-1))(x(k)-x(k+1))
	//L(k+1)=(x-x(k-1))(x-x(k))/(x(k+1)-x(k-1))(x(k+1)-x(k))
#define L_K_minus1(x, xk, xk_minus1, xk_plus1) ((x-xk)*(x-xk_plus1)/((xk_minus1-xk)*(xk_minus1-xk_plus1)))
#define L_K(x, xk, xk_minus1, xk_plus1) 	((x-xk_minus1)*(x-xk_plus1)/((xk-xk_minus1)*(xk-xk_plus1)))
#define L_K_plus1(x, xk, xk_minus1, xk_plus1)  ((x- xk_minus1)*(x-xk)/((xk_plus1-xk_minus1)*(xk_plus1-xk)))

//calculate L(k)(x), L(k-1)(x), L(k+1)(x)
	L_Kx = L_K(inputDistance, pL[K]->distance, pL[K_minus_One]->distance, pL[K_plus_One]->distance);
	L_K_MINUS_1x = L_K_minus1(inputDistance, pL[K]->distance, pL[K_minus_One]->distance, pL[K_plus_One]->distance);
	L_K_PLUS_1x = L_K_plus1(inputDistance, pL[K]->distance, pL[K_minus_One]->distance, pL[K_plus_One]->distance);

	assert(pL[K]->distance != pL[K_minus_One]->distance);
	assert(pL[K_plus_One]->distance != pL[K_minus_One]->distance);
	assert(pL[K]->distance != pL[K_plus_One]->distance);
//插值时间，瞄准角，偏流
//t(x)=t(k-1)*L(k-1)(x)+t(k)L(k)(x)+t(k+1)L(k+1)(x)
 	*outputAngle = pL[K_minus_One]->elevationAngle * L_K_MINUS_1x 
				+ pL[K]->elevationAngle * L_Kx
				+ pL[K_plus_One]->elevationAngle* L_K_PLUS_1x;
	*outputDuration =  pL[K_minus_One]->duration * L_K_MINUS_1x 
					+ pL[K]->duration * L_Kx
					+ pL[K_plus_One]->duration* L_K_PLUS_1x;	
	*outputBias =  pL[K_minus_One]->bias * L_K_MINUS_1x 
					+ pL[K]->bias * L_Kx
					+ pL[K_plus_One]->bias * L_K_PLUS_1x;
//算综合修正参数
	correction_K =  getGeneralCorrectionTheta(pL[K]->distance, type);
	correction_K_minus1 = getGeneralCorrectionTheta(pL[K_minus_One]->distance, type);
	correction_K_plus1 = getGeneralCorrectionTheta(pL[K_plus_One]->distance, type);
	
	outputCorrection->deltaX = correction_K_minus1.deltaX * L_K_MINUS_1x
								+ correction_K.deltaX * L_Kx
								+ correction_K_plus1.deltaX * L_K_PLUS_1x;
	outputCorrection->deltaY = correction_K_minus1.deltaY * L_K_MINUS_1x
								+ correction_K.deltaY * L_Kx
								+ correction_K_plus1.deltaY * L_K_PLUS_1x;

}


int trajectoryCalc( FiringInputs *input, FiringOutputs* output)
{
	//find the closest three RowItems

	RowItems *L[K_COUNT];
	FiringTable * firingTable = 0;
	int i = 0;
	firingTable = getFiringTable(input->ProjectileType);
	if( firingTable == NULL )
	{
		printf("ERROR  return CALC_INVALID_PARAM \n");
		return CALC_INVALID_PARAM; //非法参数
	}

	if(input->TargetDistance > firingTable->table[firingTable->count - 1].distance)
	{
		printf("ERROR  return CALC_OVER_DISTANCE \n");
		return CALC_OVER_DISTANCE;//超出射击距离
	}
	else if (input->TargetDistance < firingTable->table[0].distance)
	{
		printf("ERROR  return CALC_UNDER_DISTANCE \n");
		return CALC_UNDER_DISTANCE;//小于最低射击距离,
	}

	//查询射表 k-1, k, k+1 列
	for(i = 0; i < firingTable->count ; i++)
	{
		if(input->TargetDistance == firingTable->table[i].distance)
		{
			//找到吻合射表，直接返回现值
			output->projectileDuration = firingTable->table[i].duration;
			output->AimElevationAngle = firingTable->table[i].elevationAngle;
			output->BiasAngle = firingTable->table[i].bias;
			
			output->AimOffsetThetaX = output->projectileDuration*input->TargetAngularVelocityX;
			output->AimOffsetThetaY = output->projectileDuration*input->TargetAngularVelocityY;
			output->AimOffsetX = ANGLE_TO_OFFSET(output->AimOffsetThetaX);
			output->AimOffsetY = ANGLE_TO_OFFSET(output->AimOffsetThetaY);
			output->correctionData = getGeneralCorrectionTheta(input->TargetDistance, input->ProjectileType);
		//	if(getProjectileType() == PROJECTILE_GRENADE_KILL)
		//		output->correctionData = gGrenadeKill_GCParam;
		//	else if(getProjectileType() == PROJECTILE_GRENADE_GAS)
		//		output->correctionData = gGrenadeGas_GCParam;
			return CALC_OK;
		}
		
		if(input->TargetDistance < firingTable->table[i].distance)
			break; // found k or k-1
	}

//准备二次插值: 选择与自变量相邻的3点

	if(i==1)//临界条件1
	{
		L[K_minus_One] = (RowItems*)&firingTable->table[0];
		L[K] = (RowItems*)&firingTable->table[1];
		L[K_plus_One] = (RowItems*)&firingTable->table[2];
	}
	else if(i == firingTable->count - 1)//临界条件2
	{
		L[K_minus_One] = (RowItems*)&firingTable->table[i-2];
		L[K] = (RowItems*)&firingTable->table[i-1];
		L[K_plus_One] = (RowItems*)&firingTable->table[i];
	}
	else
	{
		if(input->TargetDistance*2 <= (firingTable->table[i].distance + firingTable->table[i-1].distance))
		{	// close to table[i-1]
			L[K_minus_One] = (RowItems*)&firingTable->table[i-2];
			L[K] = (RowItems*)&firingTable->table[i-1];
			L[K_plus_One] = (RowItems*)&firingTable->table[i];
		}else 
		{	// close to table[i]
			L[K_minus_One] = (RowItems*)&firingTable->table[i-1];
			L[K] = (RowItems*)&firingTable->table[i];
			L[K_plus_One] = (RowItems*)&firingTable->table[i+1];
		}
	}
//二次插值:
	Quadratic_Interpolation(L,input->TargetDistance,input->ProjectileType,&output->AimElevationAngle, 
							&output->projectileDuration,&output->BiasAngle,
								&output->correctionData);
	output->AimOffsetThetaX = output->projectileDuration*input->TargetAngularVelocityX;
	output->AimOffsetThetaY = output->projectileDuration*input->TargetAngularVelocityY;
	output->AimOffsetX = ANGLE_TO_OFFSET(output->AimOffsetThetaX);
	output->AimOffsetY = ANGLE_TO_OFFSET(output->AimOffsetThetaY);
	return CALC_OK;
}



#include "PolyNomialCalc.h"
#include "permanentStorage.h"
#include "firingTable.h"
#include "app_global.h"


//DESCRIPTION: use polynomial to simulate the projectile curves, output delay, angle etc.

//****************************start of n-th order polynomial simulation*****************
//--------------- machine gun polynomial parameters, sigma << 0.1 mil------------------
static const double gMachGunTheta_params14[]= { 9.11006342e-37 , -1.02283410e-32 ,  5.20539747e-29 , -1.58865128e-25,
   					  3.24151131e-22 , -4.66657861e-19 ,  4.87474951e-16,  -3.74164199e-13,
   					  2.11259926e-10 , -8.69294864e-08 ,  2.55230951e-05, -5.15059362e-03,
  					  6.69614925e-01 , -4.96204141e+01 ,  1.55971087e+03};
static const double gMachGunDelay_params14 []= { -2.64974047e-39 ,  3.27845184e-35 , -1.83015992e-31 ,  6.09796206e-28,
 					   -1.35176973e-24  , 2.10347035e-21 , -2.36237731e-18 ,  1.93860268e-15,
 					    -1.16347098e-12  , 5.05878177e-10,  -1.56016541e-07  , 3.28774640e-05,
  					    -4.43661020e-03 ,  3.40427687e-01 , -1.08999980e+01};
static const double gMachGunTempture_params14[]={2.46276538e-36 , -2.77700606e-32 ,  1.41941909e-28 , -4.35071103e-25,
   					8.91485502e-22 , -1.28862724e-18  , 1.35124576e-15 , -1.04076663e-12,
   					5.89441781e-10 , -2.43177303e-07  , 7.15501547e-05 , -1.44628852e-02,
   					1.88264089e+00 , -1.39654350e+02  , 4.38299486e+03};
static const double gMachGunAirPressure_params14[]={2.14846755e-36 , -2.39576502e-32 ,  1.21067273e-28 , -3.66833853e-25,
   					7.43084257e-22,  -1.06215472e-18,   1.10199460e-15 , -8.40584361e-13,
   					4.72082763e-10 , -1.93458209e-07 ,  5.66559061e-05 , -1.14240451e-02,
   					1.48652054e+00 , -1.10412174e+02 ,  3.47099570e+03};
static const double gMachGunBias_params3[] = {5.50120791e-12 , -1.18117078e-07  , 9.94246136e-04 , -1.28994559e-01};

//--------------grenade kill polynomial parameters note: sigma==0.001909mil-------------
static const double gGenadeKillTheta_params16[]= { 6.06218405e-42 , -8.89909278e-38  , 6.00247384e-34 , -2.46592457e-30,
   					     6.89506841e-27 , -1.38891868e-23  , 2.08066933e-20 , -2.35894922e-17,
   					     2.03995462e-14 , -1.34573538e-11 ,  6.72432961e-09 , -2.50691495e-06,
   					     6.79874229e-04 , -1.28787900e-01 ,  1.59320024e+01 , -1.13658652e+03,
   					     3.46711948e+04};
//sigma == 4.2397620066577478e-05
static const double gGrenadeKillDelay_params16 []= { 2.25866749e-43 , -3.32047062e-39  , 2.24308565e-35 , -9.22966882e-32,
   						2.58500795e-28,  -5.21602071e-25 ,  7.82747305e-22 , -8.89002725e-19,
						7.70144829e-16,  -5.08944167e-13 ,  2.54740138e-10 , -9.51244464e-08,
						2.58367396e-05,  -4.90094188e-03 ,  6.07010858e-01 , -4.33473479e+01,
						1.32337710e+03};
//sigma == 0.000414
static const double gGenadeKillTemptur_params16[]= {2.16804051e-42 , -3.14935388e-38 ,  2.10008275e-34 , -8.52082082e-31,
						2.35058302e-27 , -4.66625667e-24 ,  6.88107869e-21 , -7.67078890e-18,
						6.51527994e-15 , -4.21723165e-12 ,  2.06589885e-09 , -7.54645354e-07,
						2.00488779e-04 , -3.72162890e-02  , 4.51601215e+00 , -3.16618037e+02,
						9.51524535e+03};
//sigma == 0.0018
static const double gGenadeKillAirPressure_params16[]= {-5.38711951e-42  , 7.79870536e-38 , -5.18447925e-34 ,  2.09805777e-30,
						-5.77601030e-27 ,  1.14509466e-23 , -1.68777003e-20 ,  1.88237617e-17,
						-1.60138486e-14 ,  1.03950544e-11 , -5.11355537e-09 ,  1.87826906e-06,
						-5.02406695e-04 ,  9.39933183e-02 , -1.15021712e+01 ,  8.13259183e+02,
						-2.46226017e+04};
// bias: sigma ==0.08
static const double gGrenadeKillBias_params10[] = {1.05330198e-28 , -9.69059122e-25 ,  3.85805664e-21 , -8.69981718e-18,
					     1.22118219e-14 , -1.10403255e-11 ,  6.42701769e-09 , -2.33822407e-06,
					     4.98523687e-04 , -5.22011660e-02 ,  2.29000000e+00};

//--------------grenade gas polynomial parameters note: sigma==4.08e-07mil-------------
static const double gGenadeGasTheta_params14[]= { 9.93883851e-29 , -2.61687708e-25 ,  3.12837264e-22 , -2.24701566e-19,
					     1.08140894e-16 , -3.68134331e-14 ,  9.12007153e-12 , -1.66577039e-09,
					     2.24678918e-07 , -2.21830287e-05 ,  1.57053312e-03 , -7.68415693e-02,
					     2.43728613e+00 , -4.35811240e+01 ,  3.43228254e+02};
//sigma == 9.28e-08
static const double gGrenadeGasDelay_params14 []= {  -5.75005344e-30  , 1.61317199e-26 , -2.04887505e-23 ,  1.55858136e-20,
  						-7.91649879e-18 ,  2.83341264e-15 , -7.34909612e-13 ,  1.39887340e-10,
						-1.95648924e-08 ,  1.99235148e-06 , -1.44677851e-04 ,  7.21974806e-03,
						-2.32172194e-01 ,  4.27305013e+00 , -3.31241377e+01};
static const double gGenadeGasTemptur_params14[15]= {0};
static const double gGenadeGasAirPressure_params14[15]= {0};
// bias: sigma ==?
static const double gGrenadeGasBias_params10[11] = {0};
typedef struct _ParamSet{
	double * pThetaParam;
	double * pDelayParam;
	double * pTemperature;
	double * pAirPressure;
} Theta_Delay;

//use order-th polynomial simulation to caluclate Theta and Delay etc. values
static int doPolynomialCalc(double *inoutvar, double maxVar,Theta_Delay *in,  unsigned int count, Theta_Delay* inout)
{
	int i = 0;
	double variable = *inoutvar,temperature = *inout->pTemperature, airPressure=*inout->pAirPressure;
	*inout->pDelayParam = *inout->pThetaParam = 0.0;
	if(count > 0){
		unsigned int order = count -1;		
		*inout->pDelayParam = in->pDelayParam[0];
		*inout->pThetaParam = in->pThetaParam[0];
		*inout->pTemperature = in->pTemperature[0];
		*inout->pAirPressure = in->pAirPressure[0];
		// calculate the delta distance by temperature and air pressure
		for(i = 0; i < order ; i ++){
			(*inout->pTemperature) *= variable;
			(*inout->pTemperature) += in->pTemperature[1+i];
			(*inout->pAirPressure) *= variable;
			(*inout->pAirPressure) += in->pAirPressure[1+i];			
		}
		//calculate the general distance
#define STEP_TEMPERATURE (10)
#define STEP_AIRPRESSURE (1500)
#define STANDARD_TEMPERATURE (15)
#define STANDARD_AIRPRESSURE (101325)
		variable = variable + (temperature-STANDARD_TEMPERATURE)*(*inout->pTemperature)/STEP_TEMPERATURE 
						+ (airPressure - STANDARD_AIRPRESSURE)*(*inout->pAirPressure)/STEP_AIRPRESSURE;
		// update the distance for bias calculation later
		*inoutvar = variable;
		if(variable >maxVar){
			return CALC_OVER_DISTANCE;//\u8d85\u51fa\u5c04\u51fb\u8ddd\u79bb
		}else if(variable < 0){
			return CALC_UNDER_DISTANCE;
		}
		// use the general distance to calculate Delay and Aim Theta
		for(i = 0; i < order ; i ++){
			(*inout->pDelayParam) *= variable;
			(*inout->pDelayParam) += in->pDelayParam[1+i];
			(*inout->pThetaParam) *= variable;
			(*inout->pThetaParam) += in->pThetaParam[1+i];		
		}
	}
		return CALC_OK;
}
static double PolynomialCalcRet(double *var, double *param,  unsigned int count)
{
	int i = 0;
// ret = p[0]*(var**order) + p[1]*(var**(order-1)) +...+p[order]
	double ret = 0.0;
	if(count > 0){
		unsigned int order = count -1;		
		ret = param[0];
		for(i = 0; i < order ; i ++){
			ret *= (*var);
			ret += param[1+i];
		}
	}

	return ret;
}
static int PolyNomialCalc_bullet( double *inoutvar, double* duration, double*angle, double* bias,double* inoutTemp, double * inoutAirPre)
{
		int ret = CALC_OK;
		unsigned int paramCount = sizeof(gMachGunDelay_params14) / sizeof(gMachGunDelay_params14[0]);
		Theta_Delay out, in;
		out.pDelayParam = duration;
		out.pThetaParam = angle;
		out.pAirPressure = inoutAirPre;
		out.pTemperature = inoutTemp;

		in.pDelayParam = (double*)gMachGunDelay_params14;
		in.pThetaParam =(double*) gMachGunTheta_params14;
		in.pAirPressure =(double*) gMachGunAirPressure_params14;
		in.pTemperature = (double*)gMachGunTempture_params14;

		if(CALC_OK == (ret =  doPolynomialCalc(inoutvar, MACHINEGUN_DISTANCE_MAX,&in, paramCount, &out )))
		{//they are all 14 order;
			paramCount =  sizeof(gMachGunBias_params3) / sizeof(gMachGunBias_params3[0]);
			*bias = PolynomialCalcRet(inoutvar,(double*) gMachGunBias_params3, paramCount);
		}

		return ret;
}
static int PolyNomialCalc_GrenadeKill( double*inoutvar, double* duration, double*angle, double* bias,double* temp, double * airPre)
{
		int ret = CALC_OK;
		unsigned int paramCount = sizeof(gGrenadeKillDelay_params16) / sizeof(gGrenadeKillDelay_params16[0]);
		Theta_Delay out, in;
		out.pDelayParam = duration;
		out.pThetaParam = angle;
		out.pAirPressure = airPre;
		out.pTemperature = temp;

		in.pDelayParam =(double*)gGrenadeKillDelay_params16;
		in.pThetaParam =(double*) gGenadeKillTheta_params16;
		in.pAirPressure = (double*)gGenadeKillAirPressure_params16;
		in.pTemperature = (double*)gGenadeKillTemptur_params16;

		if(CALC_OK == (ret = doPolynomialCalc(inoutvar, GRENADE_KILL_DISTANCE_MAX,&in, paramCount, &out )))
		{//they are all 16 order;
			paramCount =  sizeof(gGrenadeKillBias_params10) / sizeof(gGrenadeKillBias_params10[0]);
			*bias = PolynomialCalcRet(inoutvar, (double*)gGrenadeKillBias_params10, paramCount);
		}
		return ret;
}
static int PolyNomialCalc_GrenadeGas( double*inoutvar, double* duration, double*angle, double* bias,double* temp, double * airPre)
{
		int ret = CALC_OK;
		unsigned int paramCount = sizeof(gGrenadeGasDelay_params14) / sizeof(gGrenadeGasDelay_params14[0]);
		Theta_Delay out, in;
		out.pDelayParam = duration;
		out.pThetaParam = angle;
		out.pAirPressure = airPre;
		out.pTemperature = temp;

		in.pDelayParam =(double*)gGrenadeGasDelay_params14;
		in.pThetaParam =(double*) gGenadeGasTheta_params14;
		in.pAirPressure = (double*)gGenadeGasAirPressure_params14;
		in.pTemperature = (double*)gGenadeGasTemptur_params14;

		if(CALC_OK == (ret = doPolynomialCalc(inoutvar, GRENADE_GAS_DISTANCE_MAX,&in, paramCount, &out )))
		{//they are all 16 order;
			paramCount =  sizeof(gGrenadeGasBias_params10) / sizeof(gGrenadeGasBias_params10[0]);
			*bias = PolynomialCalcRet(inoutvar, (double*)gGrenadeGasBias_params10, paramCount);
		}
		return ret;
}
//--------------------end of polynomial calculation---------------------
//\u4e8c\u6b21\u63d2\u503c\u6cd5
//pL\u30103\u3011:\u4e0e\u81ea\u53d8\u91cf\u76f8\u90bb\u76843\u4e2a\u70b9
//inputDistance:\u81ea\u53d8\u91cf
//type:\u5f39\u79cd
//outputCorrection:\u8f93\u51fa\u7684\u7efc\u4fee\u7ed3\u679c

static void CalculateCorrection(RowItems *pL[K_COUNT], double inputDistance,PROJECTILE_TYPE type , 
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


//\u7b97\u7efc\u5408\u4fee\u6b63\u53c2\u6570
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

int polynomialCalc( FiringInputs *input, FiringOutputs* output)
{
	//find the closest three RowItems

	RowItems *L[K_COUNT];
	FiringTable * firingTable = 0;
	int i = 0;
	int ret = CALC_OK;
	double inputDistance = input->TargetDistance, temperature = input->Temperature, airPressure = input->AirPressure;
	
	firingTable = getFiringTable(input->ProjectileType);
	if(firingTable == NULL)
	{
		return CALC_INVALID_PARAM; //\u975e\u6cd5\u53c2\u6570
	}
	
	if(PROJECTILE_BULLET== input->ProjectileType){
		ret = PolyNomialCalc_bullet(&inputDistance, &output->projectileDuration, 
			&output->AimElevationAngle,&output->BiasAngle,&temperature, &airPressure);
	}else if(PROJECTILE_GRENADE_KILL == input->ProjectileType){
		ret = PolyNomialCalc_GrenadeKill(&inputDistance, &output->projectileDuration, 
			&output->AimElevationAngle,&output->BiasAngle,&temperature, &airPressure);
	}else if( PROJECTILE_GRENADE_GAS == input->ProjectileType){
		ret = PolyNomialCalc_GrenadeGas(&inputDistance, &output->projectileDuration, 
			&output->AimElevationAngle,&output->BiasAngle,&temperature, &airPressure);
	}else{
			assert(FALSE);
	}

	if(ret != CALC_OK){
		return ret;
	}
	for(i = 0; i < firingTable->count ; i++)
	{
		if(inputDistance == firingTable->table[i].distance)
		{
			output->projectileDuration = firingTable->table[i].duration;
			output->AimElevationAngle = firingTable->table[i].elevationAngle;
			output->BiasAngle = firingTable->table[i].bias;
			
			output->AimOffsetThetaX = output->projectileDuration*input->TargetAngularVelocityX;
			output->AimOffsetThetaY = output->projectileDuration*input->TargetAngularVelocityY;
			output->AimOffsetX = ANGLE_TO_OFFSET(output->AimOffsetThetaX);
			output->AimOffsetY = ANGLE_TO_OFFSET(output->AimOffsetThetaY);
			output->correctionData = getGeneralCorrectionTheta(inputDistance, input->ProjectileType);
			return CALC_OK;
		}
		
		if(inputDistance < firingTable->table[i].distance)
			break; // found k or k-1
	}


	if(i==1)//\u4e34\u754c\u6761\u4ef61
	{
		L[K_minus_One] = (RowItems*)&firingTable->table[0];
		L[K] = (RowItems*)&firingTable->table[1];
		L[K_plus_One] = (RowItems*)&firingTable->table[2];
	}
	else if(i == firingTable->count - 1)//\u4e34\u754c\u6761\u4ef62
	{
		L[K_minus_One] = (RowItems*)&firingTable->table[i-2];
		L[K] = (RowItems*)&firingTable->table[i-1];
		L[K_plus_One] = (RowItems*)&firingTable->table[i];
	}
	else
	{
		if(inputDistance*2 <= (firingTable->table[i].distance + firingTable->table[i-1].distance))
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
	CalculateCorrection(L,inputDistance,input->ProjectileType,&output->correctionData);
	output->AimOffsetThetaX = output->projectileDuration*input->TargetAngularVelocityX;
	output->AimOffsetThetaY = output->projectileDuration*input->TargetAngularVelocityY;
	output->AimOffsetX = ANGLE_TO_OFFSET(output->AimOffsetThetaX);
	output->AimOffsetY = ANGLE_TO_OFFSET(output->AimOffsetThetaY);
	return CALC_OK;
}




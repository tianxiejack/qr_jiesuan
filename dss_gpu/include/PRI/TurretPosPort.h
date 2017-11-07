#ifndef _TURRETPOSPORT_H
#define _TURRETPOSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basePort.h"

typedef struct _TurretPosPort
{
	PortInter_t pInter;
	UartObj UartPort;
}TurretPosPort_t,*TurretPosPORT_Handle;


double getTurretTheta();
BOOL isTurretVelocityValid();
void resetTurretVelocityCounter();
int getAverageVelocity(double* value);
double getTurretTheta();
void SelfCheckPosAngleTimer_cbFxn(void* cbParam);


TurretPosPORT_Handle TurretPosPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void TurretPosPORT_unInitial(TurretPosPORT_Handle handle);
int TurretPosPORT_send(BYTE* pSendBuf,int iLen);
void * TurretPosPORT_recvTask(void * prm);
void markMeasure_dist_Time();
void TurretPosPORT_PhraseBye(unsigned char *pRecv);
void killSelfCheckPosAngleTimer();
void startSelfCheckPosAngle_Timer();


void killSelfCheckWeaponCtrlTimer();
void startSelfCheckWeaponCtrl_Timer();
void SelfCheckWeaponCtrlTimer_cbFxn(void* cbParam);
double getTurretThetaDelta();

extern C_Thetas gTurretTheta;
extern volatile bool valid_measure;


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _TURRETPOSPORT_H */


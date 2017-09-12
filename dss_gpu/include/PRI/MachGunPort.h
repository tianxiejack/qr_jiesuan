#ifndef _MACHGUNPORT_H
#define _MACHGUNPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basePort.h"


typedef struct _MachGunPort
{
	PortInter_t pInter;
	UartObj UartPort;
}MachGunPort_t,*MachGunPORT_Handle;
void markMeasure_dip_Time();
void resetDipVelocityCounter();

int MachGunPORT_send(BYTE* pSendBuf,int iLen);


MachGunPORT_Handle MachGunPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void MachGunPORT_unInitial(MachGunPORT_Handle handle);
int MachGunPORT_send(BYTE* pSendBuf,int iLen);
void* MachGunPORT_recvTask(void* prm);
double getMachGunAngle();
double getMachGunAngleAbs();
int MachGunPORT_PhraseBye(unsigned char *buf);
int getAverageDipVelocity(double* value);

void SelfCheckMachGunAngleTimer_cbFxn(void* cbParam);
void startSelfCheckMachGunAngle_Timer();
void killSelfCheckMachGunAngleTimer();


extern C_Thetas MachGunAngle;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _MACHGUNPORT_H */



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




TurretPosPORT_Handle TurretPosPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void TurretPosPORT_unInitial(TurretPosPORT_Handle handle);
int TurretPosPORT_send(BYTE* pSendBuf,int iLen);

void * TurretPosPORT_recvTask(void * prm);
void markMeasure_dist_Time();


void TurretPosPORT_PhraseBye(unsigned char *pRecv);




extern C_Thetas gTurretTheta;



#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _TURRETPOSPORT_H */


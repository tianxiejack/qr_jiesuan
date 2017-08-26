#ifndef _GRENADEPORT_H
#define _GRENADEPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basePort.h"


typedef struct _GrenadePort
{
	PortInter_t pInter;
	UartObj UartPort;
}GrenadePort_t,*GrenadePORT_Handle;



GrenadePORT_Handle GrenadePORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void GrenadePORT_unInitial(GrenadePORT_Handle handle);
int GrenadePORT_send(BYTE* pSendBuf,int iLen);
void* GrenadePORT_recvTask(void *prm);
double getGrenadeAngle();

double getGrenadeAngleAbs();

int GrenadePORT_PhraseByte(BYTE* pRecv);





extern double GrenadeAngle ;


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _GRENADEPORT_H */


#ifndef _POSITIONPORT_H
#define _POSITIONPORT_H

#ifdef __cplusplus
extern "C" {
#endif
//#include "sdkDef.h"
#include "basePort.h"


typedef struct _PositionPort
{
	PortInter_t pInter;
	UartObj UartPort;
}PositionPort_t,*PositionPort_Handle;


PositionPort_Handle PositionPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void PositionPORT_unInitial(PositionPort_Handle handle);
int PositionPORT_send(BYTE* pSendBuf,int iLen);

void* PositionPORT_recvTask(void* prm);

double getPlatformPositionX();
double getPlatformPositionY();

int PositionPORT_PhraseBye(BYTE* pRecv);




extern double hPositionX,hPositionY,Tempre;


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _POSITIONPORT_H */


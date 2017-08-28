#ifndef _LASERPORT_H
#define _LASERPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basePort.h"


typedef struct _LaserPort
{
	PortInter_t pInter;
	UartObj UartPort;
}LaserPort_t,*LaserPORT_Handle;


LaserPORT_Handle LaserPORT_initial(int(*MuxPortRecvFun)(Enum_MuxPortType,BYTE*,int));
void LaserPORT_unInitial(LaserPORT_Handle handle);
int LaserPORT_send(BYTE* pSendBuf,int iLen);

void* LaserPORT_recvTask(void *prm);
int LaserPORT_requst();
int LaserPORT_Ack();
void LaserPORT_lpfnTimer(void* cbParam);
int getLaserDistance();

void killLaserTimer();

int LaserPORT_PhraseBye(BYTE* pRecv);


extern int LaserDistance;




#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _LASERPORT_H */




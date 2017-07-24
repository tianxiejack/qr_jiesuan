
#ifndef _PORT_TASK_H_
#define _PORT_TASK_H_

#include <dx.h>
#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <port.h>

#ifndef IS_SYNC_FLAG 
#define IS_SYNC_FLAG(d) (d == 0x55||d == 0x56||d == 0x60)
#define LEN_BY_FLAG(f)  (f == 0x55||f == 0x56||f == 0x60) ? 8 : 0
#endif

OSA_TskHndl *port_tsk_create(port_handle *port, OSA_TskHndl *recvTsk, Uint16 recvMsgId, void * recvContext);
void port_tsk_destroy(OSA_TskHndl * tsk);
int port_tsk_send(OSA_TskHndl * tsk, Uint8 *data, Int32  nbytes, Uint32 timeout);
int port_tsk_recv(OSA_TskHndl * tsk, Uint8 *data, Int32  nMaxBytes, Uint32 timeout);


#endif

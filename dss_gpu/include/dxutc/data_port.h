
#ifndef _DATA_PORT_H_
#define _DATA_PORT_H_

#include <dx.h>
#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <port.h>

typedef Int32 (*fxnDataPortTransNotify)(void *p, Int32 size, Int32 transLen, Int32 flag);
int data_port_send_file(port_handle *port, char *file, Uint32 timeout, fxnDataPortTransNotify fxnNotify);
int data_port_recv_file(port_handle *port, char *file, Uint32 timeout, fxnDataPortTransNotify fxnNotify);
int data_port_send_data(port_handle *port, Uint8 *data, Int32 size, Uint32 timeout, fxnDataPortTransNotify fxnNotify);
int data_port_recv_data(port_handle *port, Uint8 *data, Int32 size, Uint32 timeout, fxnDataPortTransNotify fxnNotify,Bool isInit);


#endif

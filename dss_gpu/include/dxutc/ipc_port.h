
/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 ChamRun Tech Incorporated - http://www.casevision.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
*  
*
*  \brief .
*
*/

#ifndef _IPC_PORT_H_
#define _IPC_PORT_H_

#include <port.h>

#define IPC_SERVER   0xFE01
#define IPC_CLIENT   0xFE02
Int32 ipc_port_create(port_handle **ppHdl);
Int32 ipc_port_destroy(port_handle *pHdl);
Int32 ipc_port_setByConfigId(void *Hdl, Int32 configId, Int32 value);
Int32 ipc_port_getByConfigId(void *Hdl, Int32 configId, Int32 value);

#endif

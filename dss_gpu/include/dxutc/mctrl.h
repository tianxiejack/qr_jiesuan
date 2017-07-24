/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2011       *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/



#ifndef _MUDUL_CONTRAL_H_
#define _MUDUL_CONTRAL_H_

#include "dx_priv.h"

#define MCSTAT_NULL		0
#define MCSTAT_ONLINE	0x0001
#define MCSTAT_REBOOT	0x0002
#define MCSTAT_READY	0x0004
#define MCSTAT_RECFG        0x0008
#define MCSTAT_FAULT		0x0010

typedef struct MC_Obj{
	OSA_TskHndl tsk;
	Int32 unit;
	Int32 mcId;
	volatile Int32  status;
	Uint32 updateTamp;
	SCRIPT_OBJ  *scripts;
	char sysconfigFile[64];
	Int32 *sysConfig;
	Int32 curStartUpId;
	BOOL sysconfigIsValid;
	Int32 curVer;
	Int32 curRemoteVer;
	Int32 curRemoteId;

	port_handle *port;
	OSA_TskHndl *portTsk;
	OSA_SemHndl  semMsg;
	OSA_MutexHndl mutex;

	Int32 curConfigId;
}MC_Obj, mc_handle;

Int32 mc_create(mc_handle *pMcHdl, Int32 mcId, char *scriptFile, char *sysconfigFile, char *portDevice);
Int32 mc_destroy(mc_handle *mcHdl);
Int32 mc_detect(mc_handle *mcHdl);
Int32 mc_getStat(mc_handle *mcHdl);
Int32 mc_get(mc_handle *mcHdl, Int32 configId, Int32 *value);
Int32 mc_set(mc_handle *mcHdl, Int32 configId, Int32 value);
Int32 mc_save(mc_handle *mcHdl);
Int32 mc_updateLocal2Remote(mc_handle *mcHdl);
Int32 mc_updateRemote2Local(mc_handle *mcHdl);
BOOL mc_IsNeedRebootRemote(mc_handle *mcHdl);

#endif /* _MUDUL_CONTRAL_H_ */

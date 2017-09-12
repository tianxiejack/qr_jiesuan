/*=======================================================
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:		dx.h
* Author:			//author
* Date:			// date
* Description:			// description
* Version:			// version
* Function List:		// funs and description
*     1. -------
* History:			// histroy modfiy record
*     <author>  <time>   <version >   <desc>
*     xavier    14/01/4     1.0     build this moudle
   =======================================================*/
#ifndef __DX_PRIV_H_
#define __DX_PRIV_H_

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <osa_mutex.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include "dx_config.h"



#include "script.h"
#include "sys_config.h"
#include "port_tsk.h"

#ifndef DATA_FILE_0
#define DATA_FILE_0 NULL
#endif
#ifndef DATA_FILE_1
#define DATA_FILE_1 NULL
#endif
#ifndef DATA_FILE_2
#define DATA_FILE_2 NULL
#endif
#ifndef DATA_FILE_3
#define DATA_FILE_3 NULL
#endif
#ifndef DATA_FILE_4
#define DATA_FILE_4 NULL
#endif
#ifndef DATA_FILE_5
#define DATA_FILE_5 NULL
#endif
#ifndef DATA_FILE_6
#define DATA_FILE_6 NULL
#endif
#ifndef DATA_FILE_7
#define DATA_FILE_7 NULL
#endif
#ifndef DATA_FILE_8
#define DATA_FILE_8 NULL
#endif
#ifndef DATA_FILE_9
#define DATA_FILE_9 NULL
#endif
#ifndef DATA_FILE_10
#define DATA_FILE_10 NULL
#endif
#ifndef DATA_FILE_11
#define DATA_FILE_11 NULL
#endif
#ifndef DATA_FILE_12
#define DATA_FILE_12 NULL
#endif
#ifndef DATA_FILE_13
#define DATA_FILE_13 NULL
#endif
#ifndef DATA_FILE_14
#define DATA_FILE_14 NULL
#endif
#ifndef DATA_FILE_15
#define DATA_FILE_15 NULL
#endif

#define DXD_HD_DISPLAY_DEFAULT_STD	( VSYS_STD_1080P_60 )
#define DX_TIMER_MAX				( 40 )

typedef enum {
	DXD_TYPE_PROGRESSIVE,
	DXD_TYPE_INTERLACED
}dxdVMType;

#ifndef SOFTRESET_FXN
#define SOFTRESET_FXN NULL
#endif


typedef Int32 ( *UpdateConfigFxn )( Int32 blkId, Int32 feildId );

#define FIELD_update( configId, notify )	Dx_update_field( CFGID_blkId(configId), CFGID_feildId(configId), notify )

#endif /* CR_DX_DRIVER_DXD_H */
/************************************** The End Of File **************************************/

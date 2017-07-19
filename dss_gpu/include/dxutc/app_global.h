#ifndef __APP_GLOBAL_H__
#define __APP_GLOBAL_H__

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>

#include <osa_sem.h>
#include <osa_mutex.h>
#include <osa_que.h>
#include <osa_msgq.h>
#include <osa_mbx.h>
#include <osa_i2c.h>

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
#include <fcntl.h>      // for open().
#include <dirent.h>
#include <sys/ioctl.h>  // for ioctl().
#include <math.h>       // for fabs().

/*
* *VAR TYPE REDEFINED
 */
#define BYTE   unsigned char
#define UINT   unsigned int
#define UINT32 unsigned int
#define BOOL   Bool

#define WPARAM UINT
#define LPARAM long
#define HANDLE void*
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/** \brief Floor a integer value. */
#define SDK_VsysUtils_floor(val, align)  (((val) / (align)) * (align))
/** \brief Align a integer value. */
#define SDK_VsysUtils_align(val, align)  SDK_VsysUtils_floor(((val) + (align)-1), (align))

/*
* *SDK API REDEFINED
 */
#define SDKAPI  extern

typedef OSA_SemHndl     SEM_ID;
typedef OSA_MutexHndl   LCK_ID;
typedef OSA_QueHndl     QUE_ID;
typedef OSA_MbxHndl     MBX_ID;
typedef OSA_MsgqHndl    MQUE_ID;
typedef OSA_MsgHndl     MSG_ID;

#define TIME_OUT 1000

#define SDK_TIMEOUT_FOREVER                                             OSA_TIMEOUT_FOREVER
#define SDK_TIMEOUT_NONE                                                OSA_TIMEOUT_NONE

#define SDK_MEM_MALLOC(size)                                            OSA_memAlloc(size)
#define SDK_MEM_FREE(ptr)                                               OSA_memFree(ptr)

#define SDK_SEM_CREATE(hndl)                                            OSA_semCreate(hndl,1,0)
#define SDK_SEM_DESTROY(hndl)                                           OSA_semDelete(hndl)
#define SDK_SEM_POST(hndl)                                              OSA_semSignal(hndl)
#define SDK_SEM_PEND(hndl,timeout)                                      OSA_semWait(hndl,timeout)

#define SDK_LOCK_CREATE(hndl)                                           OSA_mutexCreate(hndl)
#define SDK_LOCK_DESTROY(hndl)                                          OSA_mutexDelete(hndl)
#define SDK_LOCK_PEND(hndl)                                             OSA_mutexLock(hndl)
#define SDK_LOCK_POST(hndl)                                             OSA_mutexUnlock(hndl)

#define SDK_QUE_CREATE(hndl,maxLen)                                     OSA_queCreate(hndl,maxLen)
#define SDK_QUE_DESTROY(hndl)                                           OSA_queDelete(hndl)
#define SDK_QUE_GET(hndl,elem,timeout)                                  OSA_queGet(hndl,&elem,timeout)
#define SDK_QUE_PUT(hndl,elem,timeout)                                  OSA_quePut(hndl,elem,timeout)
#define SDK_QUE_ISEMPTY(hndl)                                           OSA_queIsEmpty(hndl)
#define SDK_QUE_GET_COUNT(hndl)                                         OSA_queGetQueuedCount(hndl)
#define SDK_QUE_PEEk(hndl,elem)                                         OSA_quePeek(hndl,&elem)

#define SDK_MSGQUE_CREATE(hndl)                                         OSA_msgqCreate(hndl)
#define SDK_MSGQUE_DESTROY(hndl)                                        OSA_msgqDelete(hndl)
#define SDK_MSGQUE_SEND(to,from,cmd,prm,msgFlags,msg)          OSA_msgqSendMsg(to,from,cmd,prm,msgFlags,msg)
#define SDK_MSGQUE_RECV(hndl,msg,timeout)                               OSA_msgqRecvMsgEx(hndl,msg,timeout)

#if 0
#define SDK_MBX_PEND                MBX_pend
#define SDK_MBX_POST                MBX_post

#define SDK_MBX_PEND                MRngBufGet
#define SDK_MBX_POST                MRngBufPut

#endif

/*
#define OSA_SOK      0  ///< Status : OK
#define OSA_EFAIL   -1  ///< Status : Generic error
*/
#define SDK_ASSERT(f)               assert(f)
#define SDK_SOK                     0           ///< Status : OK
#define SDK_EFAIL                   -1          ///< Status : Generic error

#define SDK_MODULE_UINI             0x00
#define SDK_MODULE_INIT             0x01

#define SDK_INVALID_POINTER         ((void*)0)
#define SDK_INVALID_MEM             SDK_INVALID_POINTER
#define SDK_INVALID_SEM             OSA_EFAIL
#define SDK_INVALID_LCK             OSA_EFAIL
#define SDK_INVALID_QUE             OSA_EFAIL

#define SDK_PEND_SEM_OK             TRUE

#define INVALID_TIMER_ID            -1

#define SDK_TIMEOUT

#define INVALIDATE(Ptr) if(Ptr==NULL) return ERR_POINTER
#define SDK_SAFE_OBJ(OBJPtr) if(OBJPtr!=NULL)free(OBJPtr);OBJPtr=NULL

#define WORD_ALIGNED(x) (_nassert(((int)(x) & 0x3) == 0))

/*
* *SDK ERR
*/
#define ERR_SUCCESS                                         0
#define ERR_FAIL                                            -1
#define ERR_OUTOFMEM                                        1
#define ERR_INVALID_ARGUMENT                                2
#define ERR_NOINTERFACE                                     3
#define ERR_POINTER                                         -4
#define ERR_CLASSNOTAVAILABLE                               5
#define ERR_TIMEOUT                                         6
#define ERR_INVALID_IO                                      7
#define ERR_NOTINIT                                         8
#define ERR_NOTIMPL                                         9
#define ERR_INVALID_HANDLE                                  10
#define ERR_INVALID_FILE                                    11


#define VENC_MAX_CH 2

#define Bit(val,i) (val&(1<<i))
#if 0
typedef enum {
	eSen_TV	= 0x00,
	eSen_FR    = 0x01,
	eSen_Max   = 0x02
}eSenserStat;
#endif
/*
* *PROJ ITEM
*/
//#define PROJ_BASE
#define PROJ_GM2_LGS009	1


#endif

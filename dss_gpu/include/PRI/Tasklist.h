
#ifndef _SECOND_MAIN_H_
#define _SECOND_MAIN_H_
#ifdef __cplusplus
extern "C"{
#endif


#include "WeaponCtrl.h"
#include "GrenadePort.h"
#include "LaserPort.h"
#include "MachGunPort.h"
#include "TurretPosPort.h"
#include "PositionPort.h"

#include "basePort.h"

#define TSK_CAPDISP				0
#define TSK_IDLE					1
#define TSK_MSGDRIV				2
#define TSK_RECVLOGMSG			3
#define TSK_RECVMAINTMSG		4
#define TSK_RECVTRACEMSG		5
#define TSK_SENDLOGMSG			6
#define TSK_DRAWOSD			7

#define TSK_RECVWEAPONMSG		8

#define TASK_NUMBER 			9


//extern OSDCTRL_Handle  pOsdCtrlObj;

#if 0
#define SDK_MEM_MALLOC(size)							                OSA_memAlloc(size)
#define SDK_MEM_FREE(ptr)								                OSA_memFree(ptr)

#define SDK_SEM_CREATE(hndl)							                OSA_semCreate(hndl,1,0)
#define SDK_SEM_DESTROY(hndl)							                OSA_semDelete(hndl)
#define SDK_SEM_POST(hndl)								                OSA_semSignal(hndl)
#define SDK_SEM_PEND(hndl,timeout)						            	OSA_semWait(hndl,timeout)

#define SDK_LOCK_CREATE(hndl) 							              	OSA_mutexCreate(hndl)
#define SDK_LOCK_DESTROY(hndl)							              	OSA_mutexDelete(hndl)
#define SDK_LOCK_PEND(hndl)								                OSA_mutexLock(hndl)
#define SDK_LOCK_POST(hndl)								                OSA_mutexUnlock(hndl)

#define SDK_QUE_CREATE(hndl,maxLen)						            	OSA_queCreate(hndl,maxLen)			
#define SDK_QUE_DESTROY(hndl)							                OSA_queDelete(hndl)
#define SDK_QUE_GET(hndl,elem,timeout)					          		OSA_queGet(hndl,&elem,timeout)
#define SDK_QUE_PUT(hndl,elem,timeout)					         	 	OSA_quePut(hndl,elem,timeout)
#define SDK_QUE_ISEMPTY(hndl)							                OSA_queIsEmpty(hndl)
#define SDK_QUE_GET_COUNT(hndl)							              	OSA_queGetQueuedCount(hndl)
#define SDK_QUE_PEEk(hndl,elem)							              	OSA_quePeek(hndl,&elem)

#define SDK_MSGQUE_CREATE(hndl)							              	OSA_msgqCreate(hndl)
#define SDK_MSGQUE_DESTROY(hndl)						              	OSA_msgqDelete(hndl)
#define SDK_MSGQUE_SEND(to,from,cmd,prm,msgFlags,msg)	    			OSA_msgqSendMsg(to,from,cmd,prm,msgFlags,msg)
#define SDK_MSGQUE_RECV(hndl,msg,timeout)                 				OSA_msgqRecvMsgEx(hndl,msg,timeout)  
#endif

typedef struct _systctrl{
	UINT segId;
	BOOL bLoadFpga;
	volatile BOOL bTrkInfor;
	volatile UINT sysStat;
	volatile UINT videoStat;
	volatile UINT lbeStat;
	volatile UINT memStat;
	volatile UINT tskArry[TASK_NUMBER];
}SYSCTRL_OBJ,*SYSCTRL_Handle;


int mainCtrl(void);
int taskCreate(void);
int taskDestroy(void);





#ifdef __cplusplus
}
#endif


#endif



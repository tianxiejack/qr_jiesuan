/*****************************************************************************************************
  Copyright (C), 2011-2012, CamraRun Tech. Co., Ltd.
  File name:    msgDriv.h
  Author:   xavier       Version:  1.0.0      Date: 2013-4-17
  Description:
  Others:
  Struct list:
    1. MSGTAB_Class,
        this is the message id and process api map store struct;
    2. MSGDRIV_Class
        this is module handle struct
  Function List:
    1. MSGDRIV_create(), module create function,create loop task
    2. MSGDRIV_destroy(), module exit function, free loop task
    3. MSGDRIV_initMsgFunTab(), map message id with message api array
    4. MSGDRIV_ProcTask(void *pPrm),module loop task function
  History:
    1. Date:
       Author:
       Modification:
    2. ...
******************************************************************************************************/
#ifndef _MSG_DRIV_H_
#define _MSG_DRIV_H_

#ifdef __cplusplus
extern "C" {
#endif
//extern int wjtest(int b);
#if 1
#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
//#include "app_global.h"

#define MAX_MSG_NUM         100

#define MSGDRIV_FREE_CMD    0xFFFF

    typedef void (*MsgApiFun)(long lParam);

    typedef struct _Msg_Tab
    {
        int msgId;
        long refContext;
        MsgApiFun pRtnFun;
    } MSGTAB_Class;

    typedef struct msgdriv
    {
    	unsigned short  bIntial;

        OSA_ThrHndl tskHndl;
        unsigned short tskLoop;
        unsigned short istskStopDone;

        OSA_MsgqHndl msgQue;
        MSGTAB_Class msgTab[MAX_MSG_NUM];
    } MSGDRIV_Class, *MSGDRIV_Handle;
#endif

extern MSGDRIV_Class g_MsgDrvObj;
   MSGDRIV_Handle  MSGDRIV_create();

#if 1
    void MSGDRIV_destroy(MSGDRIV_Handle handle);
    void MSGDRIV_attachMsgFun(MSGDRIV_Handle handle, int msgId, MsgApiFun pRtnFun, int context);
    void MSGDRIV_send(int msgId, void *prm);
    void* MSGDRIV_ProcTask(void *pPrm);

 #endif
    #ifdef __cplusplus
    }
    #endif
#endif


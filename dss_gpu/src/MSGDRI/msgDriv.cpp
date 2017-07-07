/********************************************************************************************************
  Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
  File name:    msgDriv.c
  Author:       xavier       Version:  1.0.0      Date: 2013-4-17
  Description:  Message driver module in this file
  Version:      1.0.0
  Function List:
    1.  MSGDRIV_Handle  MSGDRIV_create()
        This function creates MSGDRIV Object.
        Here we init the message api map and create the message queue.
        Then message driver task is created here.
        And we return the MSGDRIV_Handle handle.

    2.  MSGDRIV_destroy(MSGDRIV_Handle handle)
        This function destroy MSGDRIV Object.
        Before MSGDRIV Object destroied, we first set taskQuit = 1 together with the MSGDRIV_FREE_CMD
        cmd to exit MSGDRIV process task thread.
        If no msg is in current message queue, the message receive function is blocked,after MSGDRIV_FREE_CMD put
        into this the message queue, and message receive function process MSGDRIV_FREE_CMD,as taskQuit is set,
        then the task quit,else when current message queue has messages then when the last message is process,
        task will quit,MSGDRIV_FREE_CMD just ignore then.
        Finally message driver task is destroied here.

    3.  MSGDRIV_attachMsgFun(MSGDRIV_Handle handle,int msgId, pMsgProcessFun pRtnFun, int context)
        This funtion init the message process function api with the message id, all this infomation will
        saved in the msgTab[MAX_MSG_NUM] filed of MSGDRIV Object.

    4.  MSGDRIV_ProcTask(void *pPrm)
        This is the message driver loop task, it will read the message from message queue,if
        no message in current, it will be blocked in the receive function, else it will look for the
        msgTab[MAX_MSG_NUM] filed of MSGDRIV Object call the message process api funtion.

  History:
      <author>  <time>   <version >   <desc>
      xavier    13/04/17     1.0     build this moudle
********************************************************************************************************/
//#ifdef __cplusplus
//extern "C" {
//#endif
#include"osa.h"
#include"osa_sem.h"
#include"osa_buf.h"
#include"osa_thr.h"
#include"osa_msgq.h"
//    #ifdef __cplusplus
 //   }
 //   #endif
//#include "app_global.h"
#include "msgDriv.h"
#include "msgProssDef.h"
//#include "MessageProcess.hpp"
MSGDRIV_Class g_MsgDrvObj;

/********************************************************************************************************
  Function:     void MSGDRIV_create()
  Description:  This function creates MSGDRIV Object. and init g_MsgDrvObj, return its ptr.
                message driver loop task also create here.
  Calls:        \MSGAPI_initial()
                \SDK_MSGQUE_CREATE()
                \OSA_thrCreate()
  Called By:    \user init interface
  Input:        null
  Output:       null
  Return:       &g_MsgDrvObj
  Others:       this funtion can only be called one time. multi-time call just return &g_MsgDrvObj,
                no other init work will be done again.
**********************************************************************************************************/
MSGDRIV_Handle  MSGDRIV_create()
{
    int status = SDK_SOK;

    MSGDRIV_Class* pMsgDrvObj = &g_MsgDrvObj;
    memset(pMsgDrvObj, 0, sizeof(MSGDRIV_Class));

    pMsgDrvObj->tskLoop = TRUE;
    pMsgDrvObj->bIntial = TRUE;

    /** < Init message process api function map array */
    
    //MSGAPI_initial(pMsgDrvObj);

    /*create message queue */
    OSA_msgqCreate(&pMsgDrvObj->msgQue);

    /*create message driver loop task */
    status = OSA_thrCreate(&pMsgDrvObj->tskHndl,
                 MSGDRIV_ProcTask,
                 OSA_THR_PRI_DEFAULT+10,
                 0,
                 pMsgDrvObj
             );

    OSA_assert(status == OSA_SOK);

    pMsgDrvObj->istskStopDone = FALSE;

    OSA_printf(" %s done!!! \r\n", __func__);
    return pMsgDrvObj;
}

/********************************************************************************************************
  Function:     void MSGDRIV_destroy(MSGDRIV_Handle handle)
  Description:  This function destroy MSGDRIV Object. Message driver loop task also exit here.
  Calls:        \SDK_MSGQUE_SEND()
                \OSA_thrDelete()
                \OSA_thrCreate()
  Called By:    \SDK_MSGQUE_DESTROY()
  Input:        MSGDRIV_Handle handle
  Output:       null
  Return:       null
  Others:       null
**********************************************************************************************************/
void MSGDRIV_destroy(MSGDRIV_Handle handle)
{
    if (handle != NULL)
    {
        handle->tskLoop = FALSE;

        Uint16 cmd = MSGDRIV_FREE_CMD;

        OSA_msgqSendMsg(&handle->msgQue, &handle->msgQue, cmd, NULL, 0, NULL);

        while (!handle->istskStopDone)
        {
            OSA_waitMsecs(5);
        }

        OSA_thrDelete(&handle->tskHndl);

        OSA_msgqDelete(&handle->msgQue );

        handle = NULL;
    }
    OSA_printf(" %s done!!! \r\n", __func__);
}

/********************************************************************************************************
  Function:     MSGDRIV_attachMsgFun(MSGDRIV_Handle handle,int msgId, MsgApiFun pRtnFun, int context)
  Description:  This function init message process api function map array.
  Calls:        null
  Called By:    \MSGDRIV_create()
  Input:        MSGDRIV_Handle handle,int msgId, MsgApiFun pRtnFun, int context
  Output:       null
  Return:       null
  Others:       null
**********************************************************************************************************/
void MSGDRIV_attachMsgFun(MSGDRIV_Handle handle, int msgId, MsgApiFun pRtnFun, int context)
{
    assert(handle != NULL && msgId < MAX_MSG_NUM);

    handle->msgTab[msgId].msgId = msgId;

    handle->msgTab[msgId].pRtnFun = pRtnFun;

    handle->msgTab[msgId].refContext = context;

   printf("^^^^^^msgId=%d  pRtnFun=%p\n",msgId,pRtnFun);
}

/********************************************************************************************************
  Function:     MSGDRIV_send(MSGDRIV_Handle handle,int msgId, void *prm)
  Description:
  Calls:        null
  Called By:
  Input:
  Output:       null
  Return:       null
  Others:       null
**********************************************************************************************************/
void MSGDRIV_send(int msgId, void *prm)
{
    assert(msgId < MAX_MSG_NUM);
    SDK_MSGQUE_SEND(&g_MsgDrvObj.msgQue, &g_MsgDrvObj.msgQue, msgId, prm, 0, NULL);
}

/********************************************************************************************************
  Function:     MSGDRIV_ProcTask(void *pPrm)
  Description:  This function message driver process loop task.
                It will read message from the message queue till it is empty.
                After message is gotten from message queue, message api map will look up to get the
                message process api to process message.
  Calls:        \SDK_MSGQUE_RECV()
  Called By:    null
  Input:        void *pPrm, MSGDRIV_Class handle, actually is &g_MsgDrvObj
  Output:       null
  Return:       null
  Others:       null
**********************************************************************************************************/
void* MSGDRIV_ProcTask(void *pPrm)
{
    MSGDRIV_Class* pMsgDrvObj = (MSGDRIV_Class*)pPrm;

    MSG_ID msg_recv;

    MSGTAB_Class msg;
    unsigned int icou = 0;

    assert(pMsgDrvObj != NULL);

    OSA_printf("%s start. \n", __func__);
    while (pMsgDrvObj->tskLoop)
    {
        icou++;
        //if(icou%2==0)
     //    printf(" [DEBUG:] %s run %d\r\n",__func__, icou);
        int iret = OSA_msgqRecvMsgEx(&pMsgDrvObj->msgQue, &msg_recv, SDK_TIMEOUT_FOREVER);

        if ((iret != SDK_EFAIL) && (msg_recv.cmd != MSGDRIV_FREE_CMD))
        {
            msg.msgId = msg_recv.cmd;

            msg.refContext = (long)msg_recv.pPrm;

            SDK_ASSERT(msg.msgId >= 0 && msg.msgId < MAX_MSG_NUM);

	 //    printf(" [DEBUG:] %s run   msg.msgId=%d   fun=%p\r\n",__func__, msg.msgId,pMsgDrvObj->msgTab[msg.msgId].pRtnFun );

            if (pMsgDrvObj->msgTab[msg.msgId].pRtnFun != NULL)
            {
                pMsgDrvObj->msgTab[msg.msgId].pRtnFun(msg.refContext);
            }
        }
        else if (msg_recv.cmd == MSGDRIV_FREE_CMD)
        {
            OSA_printf(" %s recieve MSGDRIV_FREE_CMD !!! ", __func__);
        }
    }

    pMsgDrvObj->istskStopDone = TRUE;

    OSA_printf(" %s exit. ", __func__);

    return NULL;
}



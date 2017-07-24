
#include <port_tsk.h>
#include <osa_rng.h>
#include "UartMessage.h"
#define DATAIN_SEM_COUNT            (10)
#define DATAIN_TSK_PRI              (2)
#define DATAIN_TSK_STACK_SIZE       (0)

#define PORT_TSK_PRI				2
#define PORT_TSK_STACK_SIZE  		(0)

typedef struct port_tsk_obj
{
    OSA_TskHndl tsk;
    port_handle *port;
    OSA_TskHndl *recvTsk;
    Uint16 recvMsgId;
    void *recvContext;

    OSA_RngHndl rngRecv;
    OSA_RngHndl rngSend;

    OSA_ThrHndl thrHandleDataIn;
    OSA_SemHndl dataInNotifySem;
    volatile Bool exitDataInThread;
}
port_tsk_obj;

static int port_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState );
static  void * port_dataInFxn(void *prm);
static Void * port_dataInBlockFxn(Void * prm);
static Void * port_dataInuartFxn(Void * prm);
static Void * port_dataoutuartFxn(Void * prm);

static Void  port_tsk_dataInInitThrObj( port_tsk_obj *tsk)
{
    OSA_rngCreate(&tsk->rngRecv, 1024);
    OSA_rngCreate(&tsk->rngSend, 1024);
    OSA_semCreate(&tsk->dataInNotifySem, DATAIN_SEM_COUNT,0);
    tsk->exitDataInThread = FALSE;
    if (tsk->port->portType == PORT_IPC)
    {
        OSA_thrCreate(&tsk->thrHandleDataIn,
                      port_dataInFxn,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      tsk);
    }
    else if(tsk->port->portType == PORT_UART)
    	{

		inituartmessage();
		 OSA_thrCreate(&tsk->thrHandleDataIn,
                      port_dataInuartFxn,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      tsk);
		  OSA_thrCreate(&tsk->thrHandleDataIn,
                      port_dataoutuartFxn,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      tsk);

    	}
    else
    {
        OSA_thrCreate(&tsk->thrHandleDataIn,
                      port_dataInBlockFxn,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      tsk);
    }
}

static Void  port_tsk_dataInDeInitThrObj( port_tsk_obj *tsk)
{
    OSA_assert(tsk != NULL);

    if(tsk->exitDataInThread == FALSE)
    {
        tsk->exitDataInThread = TRUE;
        do
        {
            OSA_waitMsecs(600);
        }
        while(tsk->exitDataInThread);
    }

    OSA_thrDelete(&tsk->thrHandleDataIn);
    OSA_semDelete(&tsk->dataInNotifySem);
    OSA_rngDelete(&tsk->rngRecv);
    OSA_rngDelete(&tsk->rngSend);

}



static Void * port_dataInuartFxn(Void * prm)
{
    Int32 result;
    port_tsk_obj *tsk = ( port_tsk_obj *) prm;
    fd_set rd_set;
    Uint8 *buffer = NULL;
    Int32   buffersize = 1024;
    Uint8 *posCur, *posEnd;
    Int32 rlen;
    Int32 i;
    Int32 pktLen = 0;
    struct timeval timeout;
    UInt32 cnt = 0;

    memset(&timeout, 0, sizeof(timeout));

    buffer = (Uint8*)malloc(buffersize);

    OSA_assert(buffer != NULL);

    OSA_assert(tsk != NULL);
    OSA_assert(tsk->port != NULL);
    OSA_assert(tsk->port->fd != -1);

    posCur = buffer;
    posEnd = buffer + buffersize;

    OSA_printf(" %d: %s   : run  !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    while(!tsk->exitDataInThread && tsk->port != NULL && tsk->port->fd != -1)
    {
        FD_ZERO(&rd_set);
        FD_SET(tsk->port->fd, &rd_set);
        timeout.tv_sec   = 0;
        timeout.tv_usec = 200000;
        result = select(tsk->port->fd+1, &rd_set, NULL, NULL, &timeout);
        //result = select(0, NULL, NULL, NULL, &timeout);
        if(result == -1 ||tsk->exitDataInThread )
        	{
        	OSA_printf("2222222222222222222fd = %d select rnt = %d \n",tsk->port->fd, result);
            break;
        	}
        //if((cnt & 0xF) == 0)
        //	OSA_printf("fd = %d select rnt = %d  cnt = %d\n",tsk->port->fd, result, cnt);
        cnt ++;

        if(result == 0)
        {
            OSA_waitMsecs(5);
	    // OSA_printf("11111111111111111111fd = %d select rnt = %d \n",tsk->port->fd, result);
            continue;
        }

        //OSA_printf("fd = %d select rnt = %d \n",tsk->port->fd, result);
        cnt = 0;

        if(FD_ISSET(tsk->port->fd, &rd_set))
        {
            rlen =  tsk->port->recv(tsk->port, posCur, posEnd-posCur);
            OSA_assert(rlen > 0);
	#if 0
            OSA_printf("R %d > 0x%02x    a0=%x a1=%x a2=%x a3=%x a4=%x a5=%x a6=%x a7=%x\r\n", rlen, *posCur,buffer[0],
				buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
	#endif

	     recv_pth(buffer,rlen);

	   
            
        }
	
    }
    free(buffer);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);
    tsk->exitDataInThread = FALSE;


    return NULL;
}



static Void * port_dataoutuartFxn(Void * prm)
{
    Int32 result;
    port_tsk_obj *tsk = ( port_tsk_obj *) prm;
    fd_set rd_set;
    Uint8 *buffer = NULL;
    Int32   buffersize = 1024;
    Uint8 *posCur, *posEnd;
    Int32 sendlen;
    Int32 i;
    Int32 pktLen = 0;
    struct timeval timeout;
    UInt32 cnt = 0;

    memset(&timeout, 0, sizeof(timeout));

    buffer = (Uint8*)malloc(buffersize);

    OSA_assert(buffer != NULL);

    OSA_assert(tsk != NULL);
    OSA_assert(tsk->port != NULL);
    OSA_assert(tsk->port->fd != -1);

    posCur = buffer;
    posEnd = buffer + buffersize;

    OSA_printf(" %d: %s   : run  !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    while(!tsk->exitDataInThread && tsk->port != NULL && tsk->port->fd != -1)
    {
		sendlen=send_pth(buffer);
		
		tsk->port->send(tsk->port, buffer, sendlen);
	 

    }
    free(buffer);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);
    tsk->exitDataInThread = FALSE;


    return NULL;
}

static Void * port_dataInFxn(Void * prm)
{
    Int32 result;
    port_tsk_obj *tsk = ( port_tsk_obj *) prm;
    fd_set rd_set;
    Uint8 *buffer = NULL;
    Int32   buffersize = 1024;
    Uint8 *posCur, *posEnd;
    Int32 rlen;
    Int32 i;
    Int32 pktLen = 0;
    struct timeval timeout;
    UInt32 cnt = 0;

    memset(&timeout, 0, sizeof(timeout));

    buffer = (Uint8*)malloc(buffersize);

    OSA_assert(buffer != NULL);

    OSA_assert(tsk != NULL);
    OSA_assert(tsk->port != NULL);
    OSA_assert(tsk->port->fd != -1);

    posCur = buffer;
    posEnd = buffer + buffersize;

    OSA_printf(" %d: %s   : run  !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    while(!tsk->exitDataInThread && tsk->port != NULL && tsk->port->fd != -1)
    {
        FD_ZERO(&rd_set);
        FD_SET(tsk->port->fd, &rd_set);
        timeout.tv_sec   = 0;
        timeout.tv_usec = 200000;
        result = select(tsk->port->fd+1, &rd_set, NULL, NULL, &timeout);
        //result = select(0, NULL, NULL, NULL, &timeout);
        if(result == -1 ||tsk->exitDataInThread )
            break;
        //if((cnt & 0xF) == 0)
        //	OSA_printf("fd = %d select rnt = %d  cnt = %d\n",tsk->port->fd, result, cnt);
        cnt ++;

        if(result == 0)
        {
            //OSA_waitMsecs(1);
            continue;
        }

        //OSA_printf("fd = %d select rnt = %d \n",tsk->port->fd, result);
        cnt = 0;

        if(FD_ISSET(tsk->port->fd, &rd_set))
        {
            rlen =  tsk->port->recv(tsk->port, posCur, posEnd-posCur);
            OSA_assert(rlen > 0);
            //OSA_printf("R %d > 0x%02x\r\n", rlen, *posCur);
            posCur += rlen;
            rlen = posCur - buffer;

            while((rlen >= pktLen) && (rlen>0))
            {
                if(pktLen == 0)
                {
                    for(i=0; i<rlen; i++)
                    {
                        if(IS_SYNC_FLAG(buffer[i]))
                        {
                            pktLen = LEN_BY_FLAG(buffer[i]);
                            if( i != 0)
                            {
                                memcpy(buffer, &buffer[i], rlen - i);
                                posCur = buffer + rlen - i;
                                rlen -= i;
                            }
                            break;
                        }
                    }
                }

                if(pktLen == 0)
                {
                    posCur = buffer;
                    rlen = 0;
                }
                else if(rlen >= pktLen)
                {
                    int putLen;
                    putLen = OSA_rngPut(&tsk->rngRecv, (Uint8*)&pktLen, sizeof(pktLen), OSA_TIMEOUT_NONE);
                    if(putLen == sizeof(pktLen))
                    {
                        //OSA_printf("R %d> 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n", pktLen,
                        //	buffer[0], buffer[1], buffer[2], buffer[3],
                        //	buffer[4], buffer[5], buffer[6], buffer[7]);

                        putLen = OSA_rngPut(&tsk->rngRecv, buffer, pktLen, OSA_TIMEOUT_FOREVER);
                        OSA_assert(putLen == pktLen);
                    }
                    if(tsk->recvTsk != NULL && putLen == pktLen)
                    {
                        //OSA_printf("%s %d: send msg.\r\n", __func__, OSA_getCurTimeInMsec());
                        OSA_tskSendMsg(tsk->recvTsk, &tsk->tsk, (Uint16)tsk->recvMsgId,(void*) tsk->recvContext, 0);
                    }
                    else
                    {
                        OSA_printf("WARNINIG: recvTsk = %p putLen = %d(%d)\n",
                                   tsk->recvTsk, putLen, pktLen);
                    }

                    if(rlen > pktLen)
                        memcpy(buffer, &buffer[pktLen], rlen - pktLen);
                    posCur = buffer + rlen - pktLen;
                    rlen -= pktLen;
                    pktLen = 0;
                }
            }
        }
    }
    free(buffer);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);
    tsk->exitDataInThread = FALSE;


    return NULL;
}

static Void * port_dataInBlockFxn(Void * prm)
{
    port_tsk_obj *tsk = ( port_tsk_obj *) prm;
    Uint8 *buffer = NULL;
    Int32   buffersize = 1024;
    Uint8 *posCur, *posEnd;
    Int32 rlen;
    Int32 i;
    Int32 pktLen = 0;
    struct timeval timeout;

    memset(&timeout, 0, sizeof(timeout));

    buffer = (Uint8*)malloc(buffersize);

    OSA_assert(buffer != NULL);

    OSA_assert(tsk != NULL);
    OSA_assert(tsk->port != NULL);
    OSA_assert(tsk->port->fd != -1);

    posCur = buffer;
    posEnd = buffer + buffersize;

    OSA_printf(" %d: %s   : run  !!!\n",
               OSA_getCurTimeInMsec(), __func__);


    while (!tsk->exitDataInThread && tsk->port != NULL && tsk->port->fd != -1)
    {
        rlen =  tsk->port->recv(tsk->port, posCur, posEnd - posCur);

        if(rlen < 0)
        {
            //close
            break;
        }

        OSA_printf("R %d > 0x%02x\r\n", rlen, *posCur);
        //OSA_printf("R %d > 0x%02x\r\n", rlen, *posCur);
        posCur += rlen;
        rlen = posCur - buffer;

        while ((rlen >= pktLen) && (rlen > 0))
        {
            if (pktLen == 0)
            {
                for (i = 0; i < rlen; i++)
                {
                    if (IS_SYNC_FLAG(buffer[i]))
                    {
                        pktLen = LEN_BY_FLAG(buffer[i]);
                        if ( i != 0)
                        {
                            memcpy(buffer, &buffer[i], rlen - i);
                            posCur = buffer + rlen - i;
                            rlen -= i;
                        }
                        break;
                    }
                }
            }

            if (pktLen == 0)
            {
                posCur = buffer;
                rlen = 0;
            }
            else if (rlen >= pktLen)
            {

                int putLen;
                putLen = OSA_rngPut(&tsk->rngRecv, (Uint8*)&pktLen, sizeof(pktLen), OSA_TIMEOUT_NONE);
                if (putLen == sizeof(pktLen))
                {
                    /*
                    OSA_printf("R %d> 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n", pktLen,
                      buffer[0], buffer[1], buffer[2], buffer[3],
                      buffer[4], buffer[5], buffer[6], buffer[7]);
                    */
                    putLen = OSA_rngPut(&tsk->rngRecv, buffer, pktLen, OSA_TIMEOUT_FOREVER);
                    OSA_assert(putLen == pktLen);
                }
                if (tsk->recvTsk != NULL && putLen == pktLen)
                {
                    //OSA_printf("%s %d: send msg.\r\n", __func__, OSA_getCurTimeInMsec());
                    OSA_tskSendMsg(tsk->recvTsk, &tsk->tsk, (Uint16)tsk->recvMsgId, (void*) tsk->recvContext, 0);
                }
                else
                {
                    OSA_printf("WARNINIG: recvTsk = %p putLen = %d(%d)\n",
                               tsk->recvTsk, putLen, pktLen);
                }

                if (rlen > pktLen)
                    memcpy(buffer, &buffer[pktLen], rlen - pktLen);
                posCur = buffer + rlen - pktLen;
                rlen -= pktLen;
                pktLen = 0;
            }
        }
    }

    free(buffer);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);
    tsk->exitDataInThread = FALSE;


    return NULL;
}

OSA_TskHndl *port_tsk_create(port_handle *port, OSA_TskHndl *recvTsk, Uint16 recvMsgId, void *recvContext)
{
    Int32 status;

    port_tsk_obj *tskObj;
    if(port == NULL || port->fd == -1)
        return NULL;

    tskObj = (port_tsk_obj *)malloc(sizeof(port_tsk_obj));

    memset(tskObj, 0, sizeof(*tskObj));

    tskObj->port = port;
    tskObj->recvTsk = recvTsk;
    tskObj->recvMsgId = recvMsgId;
    tskObj->recvContext = recvContext;

    status = OSA_tskCreate(&tskObj->tsk,
                           port_tskMain,
                           PORT_TSK_PRI,
                           PORT_TSK_STACK_SIZE, 0, tskObj);
    OSA_assert(status == OSA_SOK);

    OSA_tskSendMsg(&tskObj->tsk, &tskObj->tsk, (Uint16)DX_MSGID_CREATE, NULL, 0);

    return &tskObj->tsk;
}

void port_tsk_destroy(OSA_TskHndl * tsk)
{
    Int32 retVal;
    port_tsk_obj *tskObj = (port_tsk_obj*)tsk;

    if(tskObj == NULL)
        return;

    retVal = OSA_tskSendMsg(&tskObj->tsk, &tskObj->tsk, (Uint16)DX_MSGID_DELETE, NULL, OSA_MBX_WAIT_ACK);
    OSA_assert(retVal == OSA_SOK);

    OSA_tskDelete(&tskObj->tsk);

    free(tskObj);
}

int port_tsk_send(OSA_TskHndl * tsk, Uint8 *data, Int32  nbytes, Uint32 timeout)
{
    int status;
    static int iPrint = 0;

    port_tsk_obj *tskObj = (port_tsk_obj*)tsk;

    if(tskObj == NULL)
        return OSA_EFAIL;

    status = OSA_rngPut(&tskObj->rngSend, data, nbytes, timeout);

    if(status == nbytes)
        status = OSA_tskSendMsg(tsk, tskObj->recvTsk, (Uint16)DX_MSGID_PORT_DATA, (void*)nbytes, 0);

    if(++iPrint % 200000 == 0)
    {
        OSA_printf("TS %d> 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", status,
                   data[0], data[1], data[2], data[3],
                   data[4], data[5], data[6], data[7]);
    }
    return status;
}
int port_tsk_recv(OSA_TskHndl * tsk, Uint8 *data, Int32  nMaxBytes, Uint32 timeout)
{
    port_tsk_obj *tskObj = (port_tsk_obj*)tsk;
    Int32 len;
    Int status;
    static int iPrint = 0;

    if(tskObj == NULL)
        return OSA_EFAIL;

    status = OSA_rngGet(&tskObj->rngRecv, (Uint8*)&len, sizeof(int), timeout);

    if(status == sizeof(int))
    {
        OSA_assert(len <= nMaxBytes);
        status = OSA_rngGet(&tskObj->rngRecv, data, len, timeout);

        if(++iPrint % 200000 == 0)
        {
            OSA_printf("TR %d> 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", status,
                       data[0], data[1], data[2], data[3],
                       data[4], data[5], data[6], data[7]);
        }

    }
    return status;
}


static int port_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
{
    port_tsk_obj *tskObj = (port_tsk_obj *)pTsk->appData;
    UInt32 cmd;
    Int32 datalen;
    Bool ackMsg, done;
    Int status = 0;
    Uint8 *buffer;
    //Dx_Obj *pObj = (Dx_Obj *) pTsk->appData;

    OSA_printf("%s:Entered", __func__);

    buffer = (Uint8*)malloc(1024);
    OSA_assert(buffer != NULL);

    done = FALSE;
    ackMsg = FALSE;

    port_tsk_dataInInitThrObj(tskObj);

    OSA_tskAckOrFreeMsg(pMsg, status);

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);
        datalen = (long)OSA_msgGetPrm(pMsg);

        switch (cmd)
        {
        case DX_MSGID_DELETE:
            done = TRUE;
            ackMsg = TRUE;
            break;

        case DX_MSGID_PORT_DATA:

            if(tskObj->port != NULL)
            {
                datalen = OSA_rngGet(&tskObj->rngSend, buffer, 1024, OSA_TIMEOUT_NONE);

                if(datalen > 0)
                    tskObj->port->send(tskObj->port, buffer, datalen);
            }

            OSA_tskAckOrFreeMsg(pMsg, status);
            break;

        default:
            OSA_tskAckOrFreeMsg(pMsg, status);
            break;
        }
    }


    free(buffer);

    port_tsk_dataInDeInitThrObj(tskObj);

    if (ackMsg && pMsg != NULL)
        OSA_tskAckOrFreeMsg(pMsg, status);

    //OSA_tskDelete(&tskObj->tsk);

    //free(tskObj);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    return 0;
}




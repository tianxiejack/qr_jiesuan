/* ===========================================================================
**
 *  \file ipc_port.c
 *
 *  \brief
 *
 *
 * ===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <ipc_port.h>
#include <osa_mutex.h>


#include <errno.h>

#include <data_port.h>
#include <dx.h>
#include <dx_priv.h>

#define MP_flag(data) (data[0])
#define MP_blkId(data) (data[1])
#define MP_feildId(data) (data[2])
#define MP_value(data) (*(Int32*)(data+3))
#define MP_check(data) (data[7] = data[1]+data[2]+data[3]+data[4]+data[5]+data[6])
#define MP_checkOk(data) (data[7] == (Uint8)(data[1]+data[2]+data[3]+data[4]+data[5]+data[6]))
#define MP_Q_blkId(data) (data[5])
#define MP_Q_feildId(data) (data[6])

#define MUTEX_LOCK(m)   { if(m) OSA_mutexLock(m);  }
#define MUTEX_UNLOCK(m) { if(m) OSA_mutexUnlock(m);}
typedef struct _ipc_port_handle
{
    port_handle Hdl;
    ipc_open_params openParams;
    OSA_MutexHndl mutex;

} ipc_port_handle;

typedef struct _IPC_MSG_
{
    long msgId;
    Uint8 data[8];
} ipc_msg;

static Int32 ipc_port_open(void *Hdl, void *openParams);
static Int32 ipc_port_close(void *Hdl);
static Int32 ipc_port_send(void *Hdl, UInt8 *data, UInt32 len);
static Int32 ipc_port_recv(void *Hdl, UInt8 *buffer, UInt32 size);

Int32 ipc_port_create(port_handle **ppHdl)
{
    ipc_port_handle *ipcPort;

    ipcPort = (ipc_port_handle*)malloc(sizeof(ipc_port_handle));

    if (ipcPort == NULL)
        return OSA_EFAIL;

    memset(ipcPort, 0, sizeof(ipc_port_handle));

    ipcPort->Hdl.portType = PORT_IPC;

    ipcPort->Hdl.fd = -1;
    ipcPort->Hdl.openParams = &ipcPort->openParams;

    ipcPort->openParams.qid     = 0;
    ipcPort->openParams.msgKey  = 0;
    ipcPort->openParams.IsQuit  = 0;

    ipcPort->Hdl.open  = ipc_port_open;
    ipcPort->Hdl.close = ipc_port_close;
    ipcPort->Hdl.send  = ipc_port_send;
    ipcPort->Hdl.recv  = ipc_port_recv;


    OSA_mutexCreate(&ipcPort->mutex);

    if (ppHdl != NULL)
        *ppHdl = (port_handle*)ipcPort;

    return OSA_SOK;
}

Int32 ipc_port_destroy(port_handle *pHdl)
{
    ipc_port_handle *ipcPort = (ipc_port_handle *)pHdl;

    if (pHdl == NULL)
        return  OSA_EFAIL;

    if (pHdl->fd != -1)
        ipc_port_close(pHdl);

    OSA_mutexDelete(&ipcPort->mutex);

    free(ipcPort);

    return OSA_SOK;
}

/**
* @brief Initialize message queue.

* Initialize message queue.
* @note This API must be used before use any other message driver API.
* @param msgKey [I ] Key number for message queue and share memory.
* @return message ID.
*/
static Int32 ipc_port_open(void *Hdl, void *openParams)
{
    ipc_open_params *ipcOpenParams = (ipc_open_params*)openParams;

    ipc_port_handle *port = (ipc_port_handle*)Hdl;

    int qid = OSA_EFAIL;

    memcpy(&port->openParams,openParams,sizeof(ipc_open_params));
    key_t key = ipcOpenParams->msgKey;

    qid = msgget(key, 0);
    if (  qid < 0 )
    {
        qid = msgget(key, IPC_CREAT | 0666);
        port->openParams.msgId[0] = IPC_SERVER;
        port->openParams.msgId[1] = IPC_CLIENT;
    }
    else
    {
        port->openParams.msgId[0] = IPC_CLIENT;
        port->openParams.msgId[1] = IPC_SERVER;
    }

    port->Hdl.fd = qid;

    OSA_printf("msgKey 0x%x qid id:%d\n",key, qid);

    return qid;
}

/**
* @brief Kill message queue.

* Kill message queue.
* @param qid [I ] Message ID.
* @retval 0 Queue killed.
*/
static Int32 ipc_port_close(void *Hdl)
{
    ipc_port_handle *port = (ipc_port_handle*)Hdl;
    OSA_assert(Hdl != NULL);

    if (Hdl == NULL || port->Hdl.fd < 0)
        return OSA_EFAIL;

    msgctl(port->Hdl.fd, IPC_RMID, NULL);

    OSA_printf("%s:: dev %s  qid %d !!!\n",
               __func__, port->openParams.device, port->Hdl.fd);

    port->Hdl.fd = -1;

    return OSA_SOK;
}

/**
* @brief Send message .

* Excute send message command.
* @param qid [I ] Message ID.
* @param *pdata [I ] pointer to the data.
* @param size [I ] Data size.
* @return send data to message queue.
*/
static Int32 ipc_port_send(void *Hdl, UInt8 *buffer, UInt32 len)
{
    UInt32 nwrite = 0;

    UInt8 msgBuf[128];

    ipc_msg *msg =  (ipc_msg *)msgBuf;
    ipc_port_handle *port = (ipc_port_handle*)Hdl;

    if (port == NULL || port->Hdl.fd < 0 || buffer == NULL || len == 0)
    {
        return OSA_EFAIL;
    }

    MUTEX_LOCK(&port->mutex);

    msg->msgId = port->openParams.msgId[0];
   
    memcpy( msg->data, buffer, len);

    nwrite = msgsnd( port->Hdl.fd, (void*)msgBuf, len, 0 );

    MUTEX_UNLOCK(&port->mutex);

    OSA_printf("%s nwrite %d id %lx data %d\n",__func__,nwrite,msg->msgId,msg->data[0]);
    return  nwrite;
}

/**
* @brief Receive message .

* Excute receive message command.
* @param qid [I ] Message ID.
* @param msg_type [I ] Message type.
* @param *pdata [I ] pointer to the data.
* @param size [I ] Data size.
* @return receive data from message queue.
*/
static Int32 ipc_port_recv(void *Hdl, UInt8 *buffer, UInt32 size)
{
    Int32 nread;
    UInt8 msgBuf[128];
    ipc_msg *msg = (ipc_msg *)&msgBuf;

    ipc_port_handle *port = (ipc_port_handle*)Hdl;


    if (port == NULL || port->Hdl.fd < 0 || buffer == NULL || size == 0)
        return OSA_EFAIL;

    msg->msgId = port->openParams.msgId[1];
    nread =  msgrcv( port->Hdl.fd, msgBuf, size - sizeof(long), msg->msgId, 0);

    if (nread > 0)
    {
        memcpy(buffer, msg->data, nread);
        port->Hdl.cntRecv++;
        port->Hdl.cntRecvBytes += nread;

    }


    OSA_printf("%s nread %d id %lx data %d\n",__func__,nread,msg->msgId,msg->data[0]);

    return  nread;
}

Int32 ipc_port_setByConfigId(void *Hdl, Int32 configId, Int32 value)
{
    Int32 status = OSA_SOK;

    ipc_port_handle *port = (ipc_port_handle*)Hdl;

    ipc_msg msg;

    OSA_assert(configId <= 256 * 16);


    memset(&msg,0,sizeof(msg));

    msg.msgId             = port->openParams.msgId[0];
    MP_flag(msg.data)       = 0x55;
    MP_blkId(msg.data)    = CFGID_blkId(configId);
    MP_feildId(msg.data) = CFGID_feildId(configId);
    MP_value(msg.data)   = value;
    MP_check(msg.data);

    ipc_port_send((void*)port, (Uint8*)&msg.data, sizeof(msg) - sizeof(long));

    return status;
}

Int32 ipc_port_getByConfigId(void *Hdl, Int32 configId, Int32 value)
{
    Int32 status = OSA_SOK;

    ipc_port_handle *port = (ipc_port_handle*)Hdl;

    ipc_msg msg;

    OSA_assert(configId <= 256 * 16);

    memset(&msg,0,sizeof(msg));

    msg.msgId             = port->openParams.msgId[0];
    MP_flag(msg.data)     = 0x56;
    MP_blkId(msg.data)    = CFGID_blkId(configId);
    MP_feildId(msg.data)  = CFGID_feildId(configId);
    MP_value(msg.data)    = value;
    MP_check(msg.data);

    OSA_printf("%s: %d !!!\n",__func__, __LINE__);

    ipc_port_send((void*)port, (Uint8*)&msg.data, sizeof(msg) - sizeof(long));

    return status;
}


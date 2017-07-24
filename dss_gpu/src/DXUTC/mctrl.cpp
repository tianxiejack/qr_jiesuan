#include <mctrl.h>
#include <stdio.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <data_port.h>
#include <dx.h>
#include <dx_priv.h>

#define MC_TSK_PRI		2
#define MC_TSK_STACK_SIZE  (0)
#define MC_TIME_OUT             500

enum{
	MC_MSGID_CREATE = 0,
	MC_MSGID_DELETE,
	MC_MSGID_PORT_DATA,
	MC_MSGID_MAX
};

#define MP_flag(data) (data[0])
#define MP_blkId(data) (data[1])
#define MP_feildId(data) (data[2])
#define MP_value(data) (*(Int32*)(data+3))
#define MP_check(data) (data[7] = data[1]+data[2]+data[3]+data[4]+data[5]+data[6])
#define MP_checkOk(data) (data[7] == (Uint8)(data[1]+data[2]+data[3]+data[4]+data[5]+data[6]))
#define MP_Q_blkId(data) (data[5])
#define MP_Q_feildId(data) (data[6])

static Int32 mc_openPort(mc_handle *mcHdl, char *portDevice);
static Int32 mc_closePort(mc_handle *mcHdl);
static Int32 mc_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState );
static Int32 mc_portDataRecv(mc_handle *pMcHdl);
static Int32 mc_getByConfigId(mc_handle *pMcHdl, Int32 configId, Int32 *value, Uint32 timeout, Int32 maskStatus, Bool bMutex);
static Int32 mc_setByConfigId(mc_handle *pMcHdl, Int32 configId, Int32 value, Uint32 timeout, Int32 maskStatus, Bool bMutex);
static Int32 mc_remote2Local(mc_handle *pMcHdl, Int32 maskStatus);
static Int32 mc_local2Remote(mc_handle *pMcHdl, Int32 maskStatus);

Int32 mc_create(mc_handle *pMcHdl, Int32 mcId, char *scriptFile, char *sysconfigFile, char *portDevice)
{
	Int32 status;

	OSA_printf("%s: port dev %s ...\r\n", __func__, portDevice);

	memset(pMcHdl, 0, sizeof(*pMcHdl));
	pMcHdl->mcId = mcId;

	script_create(scriptFile, &pMcHdl->scripts);
	pMcHdl->sysConfig = (Int32*)malloc(sizeof(Int32)*256*16);
	memset(pMcHdl->sysConfig, 0, sizeof(Int32)*256*16);

	strcpy(pMcHdl->sysconfigFile, sysconfigFile);

	status = syscfg_load(sysconfigFile, pMcHdl->sysConfig, 128*16*4);
	if(status == OSA_SOK){		
		pMcHdl->curStartUpId = pMcHdl->sysConfig[CFGID_SYS_SEL_STARTUP];//MC_FIELD_SYS_SEL_STARTUP(pMcHdl->sysConfig);
		pMcHdl->sysconfigIsValid = TRUE;
		pMcHdl->curVer = pMcHdl->sysConfig[CFGID_SYS_CFG_CFG_VER];
	}

	pMcHdl->sysConfig[CFGID_SYS_CFG_DEVID] = pMcHdl->mcId;

	pMcHdl->updateTamp = OSA_getCurTimeInMsec();

	if(OSA_SOK != (status = mc_openPort(pMcHdl, portDevice)))
	{
		if(pMcHdl->scripts != NULL)
			script_destroy(pMcHdl->scripts);
		pMcHdl->scripts = NULL;
		if(pMcHdl->sysConfig != NULL)
			free(pMcHdl->sysConfig);
		pMcHdl->sysConfig = NULL;	

		pMcHdl->status = OSA_EFAIL;

		OSA_printf("%s: ... Fault! status %x\r\n", __func__, status);
		return status;
	}

	OSA_semCreate(&pMcHdl->semMsg, 1, 0);

	OSA_mutexCreate(&pMcHdl->mutex);

        status = OSA_tskCreate(&pMcHdl->tsk,
                               mc_tskMain,
                               MC_TSK_PRI,
                               MC_TSK_STACK_SIZE, 0, pMcHdl);
        OSA_assert(status == OSA_SOK);

	OSA_tskSendMsg(&pMcHdl->tsk, NULL, MC_MSGID_CREATE, NULL, 0);

	OSA_printf("%s: ... done. status %x\r\n", __func__, status);

	return status;
}
Int32 mc_destroy(mc_handle *pMcHdl)
{
	Int32 status;

	OSA_assert(pMcHdl->status != -1);

	if(pMcHdl->curConfigId != 0){
		OSA_semSignal(&pMcHdl->semMsg);		
	}

	OSA_tskSendMsg(&pMcHdl->tsk, &pMcHdl->tsk, MC_MSGID_DELETE, NULL, OSA_MBX_WAIT_ACK);

	status =  OSA_tskDelete(&pMcHdl->tsk);

	mc_closePort(pMcHdl);

	pMcHdl->status = -1;

	if(pMcHdl->scripts != NULL)
		script_destroy(pMcHdl->scripts);
	pMcHdl->scripts = NULL;
	if(pMcHdl->sysConfig != NULL)
		free(pMcHdl->sysConfig);
	pMcHdl->sysConfig = NULL;

	OSA_semDelete(&pMcHdl->semMsg);

	OSA_mutexDelete(&pMcHdl->mutex);

	return status;
}

Int32 mc_detect(mc_handle *pMcHdl)
{
	Int32 status = OSA_SOK;
	Int32 configId = CFGID_SYS_CFG_DEVID;

	OSA_assert(pMcHdl != NULL);

	if(pMcHdl->status == OSA_EFAIL)
		return OSA_EFAIL;

	OSA_mutexLock(&pMcHdl->mutex);

	pMcHdl->status = MCSTAT_NULL;
	
	{
		Uint8 data[8] = {0x55, 0xff, 0, 0, 0, 0, 0, 0};

		configId = CFGID_SYS_CFG_DEVID;
		MP_Q_blkId(data) = CFGID_blkId(configId);
		MP_Q_feildId(data) = CFGID_feildId(configId);
		MP_check(data);

		OSA_assert(pMcHdl->curConfigId == 0);
		pMcHdl->curConfigId = configId;
		status = port_tsk_send(pMcHdl->portTsk, data, 8, OSA_TIMEOUT_FOREVER);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x send failed!\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
			goto __exit;
		}
		status = OSA_semWait(&pMcHdl->semMsg, MC_TIME_OUT);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x timeout !\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
			goto __exit;
		}
		pMcHdl->curConfigId = 0;

		pMcHdl->status |= MCSTAT_ONLINE;

		pMcHdl->curRemoteId = pMcHdl->sysConfig[configId];
		pMcHdl->sysConfig[CFGID_SYS_CFG_DEVID] = pMcHdl->mcId;
		if(pMcHdl->curRemoteId == pMcHdl->mcId)
		{
			if(pMcHdl->sysconfigIsValid)
			{
				configId = CFGID_SYS_CFG_CFG_VER;
				MP_Q_blkId(data) = CFGID_blkId(configId);
				MP_Q_feildId(data) = CFGID_feildId(configId);
				MP_check(data);

				OSA_assert(pMcHdl->curConfigId == 0);
				pMcHdl->curConfigId = configId;
				status = port_tsk_send(pMcHdl->portTsk, data, 8, OSA_TIMEOUT_FOREVER);
				if(OSA_SOK != status)
				{
					OSA_printf("%s %d: mc%d configId 0x%x send failed!\r\n", 
						__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
					goto __exit;
				}
				status = OSA_semWait(&pMcHdl->semMsg, MC_TIME_OUT);
				if(OSA_SOK != status)
				{
					OSA_printf("%s %d: mc%d configId 0x%x timeout !\r\n", 
						__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
					goto __exit;
				}
				pMcHdl->curConfigId = 0;				
				pMcHdl->curRemoteVer = pMcHdl->sysConfig[CFGID_SYS_CFG_CFG_VER];
				pMcHdl->sysConfig[CFGID_SYS_CFG_CFG_VER] = pMcHdl->curVer;

				if(pMcHdl->curRemoteVer != pMcHdl->curVer)
					pMcHdl->status |= MCSTAT_RECFG;			
			}
		}
		else
		{
			pMcHdl->status |= MCSTAT_RECFG;
		}
	}

	if(!pMcHdl->sysconfigIsValid && pMcHdl->curRemoteId == pMcHdl->mcId)
	{
		status = mc_remote2Local(pMcHdl, MCSTAT_ONLINE);
		if(OSA_SOK == status)
		{
			pMcHdl->sysconfigIsValid = TRUE;
		}
	}

	if(pMcHdl->status & MCSTAT_RECFG && pMcHdl->sysconfigIsValid)
	{
		status = mc_local2Remote(pMcHdl, MCSTAT_ONLINE);
		if(OSA_SOK == status)
		{
			pMcHdl->status &= ~MCSTAT_RECFG;
			pMcHdl->status |= MCSTAT_REBOOT;
		}
	}

	if(OSA_SOK == status && pMcHdl->sysconfigIsValid)
		pMcHdl->status |= MCSTAT_READY;

__exit:
	pMcHdl->curConfigId = 0;
	OSA_mutexUnlock(&pMcHdl->mutex);
	
	return status;
}

Int32 mc_getStat(mc_handle *mcHdl)
{
	OSA_assert(mcHdl != NULL);

	return mcHdl->status;
}

Int32 mc_get(mc_handle *mcHdl, Int32 configId, Int32 *value)
{
	OSA_assert(mcHdl != NULL);

	if(value == NULL)
		return OSA_EFAIL;

	return mc_getByConfigId(mcHdl, configId, value, MC_TIME_OUT, MCSTAT_READY, TRUE);
}

Int32 mc_set(mc_handle *mcHdl, Int32 configId, Int32 value)
{
	OSA_assert(mcHdl != NULL);

	if(mcHdl->sysconfigIsValid)
	{
		if(mcHdl->sysConfig[configId] == value)
			return OSA_SOK;
	}	

	return mc_setByConfigId(mcHdl, configId, value, MC_TIME_OUT, MCSTAT_READY, TRUE);
}

Int32 mc_updateRemote2Local(mc_handle *mcHdl)
{
	return mc_remote2Local(mcHdl, MCSTAT_READY);
}

Int32 mc_updateLocal2Remote(mc_handle *mcHdl)
{
	return mc_local2Remote(mcHdl, MCSTAT_READY);
}

BOOL mc_IsNeedRebootRemote(mc_handle *mcHdl)
{
	return (mcHdl->status & MCSTAT_REBOOT);
}

static Int32 mc_openPort(mc_handle *mcHdl, char *portDevice)
{
	Int32 status = OSA_SOK;

	uart_open_params mUartOpenParams;
	mUartOpenParams.baudrate = 115200;
	mUartOpenParams.databits = 8;
	mUartOpenParams.parity = 'E';
	mUartOpenParams.stopbits = 1;

	status =  port_create(PORT_UART, &mcHdl->port);
	OSA_assert(status == OSA_SOK);
	OSA_assert(mcHdl->port != NULL);
	strcpy(mUartOpenParams.device, portDevice);
	status = mcHdl->port->open(mcHdl->port, &mUartOpenParams);
	OSA_assert(status == OSA_SOK);
	mcHdl->portTsk = port_tsk_create(mcHdl->port, &mcHdl->tsk, MC_MSGID_PORT_DATA, (void*)mcHdl);	
	OSA_assert(mcHdl->portTsk != NULL);

	return status;
}

static Int32 mc_closePort(mc_handle *mcHdl)
{
	Int32 status = OSA_SOK;

	port_tsk_destroy(mcHdl->portTsk);
	mcHdl->portTsk = NULL;
	mcHdl->port->close(mcHdl->port);
	port_destory(mcHdl->port);
	mcHdl->port = NULL;

	return status;
}

static Int32 mc_portDataRecv(mc_handle *pMcHdl)
{
	int status = OSA_SOK;

	Uint8 data[64];

	if(pMcHdl->port == NULL ||pMcHdl->portTsk == NULL )
		return OSA_EFAIL;

	status  = port_tsk_recv(pMcHdl->portTsk, data, 64, OSA_TIMEOUT_NONE);

	if(status <= 0)
		return OSA_EFAIL;

	if(MP_flag(data) == 0x55)
	{
		if(MP_checkOk(data))
		{
			Uint8 blkId, feildId;
			Int32 value;
			Int32 configId;
			if(MP_blkId(data) != 0xFF)
			{
				blkId = MP_blkId(data);
				feildId = MP_feildId(data);
				configId = CFGID_BUILD(blkId, feildId);

				if(configId < 256*16){
					value = MP_value(data);
					pMcHdl->sysConfig[configId] = value;
				}

				//int OSA_semWait(OSA_SemHndl *hndl, Uint32 timeout);
				if(pMcHdl->curConfigId == configId)
					OSA_semSignal(&pMcHdl->semMsg);				
			}
			else
			{
				blkId = MP_Q_blkId(data);
				feildId = MP_Q_feildId(data);
				configId = CFGID_BUILD(blkId, feildId);

				if(configId < 256*16){
				 	value = pMcHdl->sysConfig[configId];
					MP_flag(data) = 0x55;
					MP_blkId(data) = blkId;
					MP_feildId(data) = feildId;
					MP_value(data) = value;
					MP_check(data);
					status = port_tsk_send(pMcHdl->portTsk, data, 8, OSA_TIMEOUT_FOREVER);
				}else{
					status = OSA_EFAIL;
				}				
			}		
		}
	}

	return status;
}

#define MUTEX_LOCK(m)	{ if(m) OSA_mutexLock(m);}
#define MUTEX_UNLOCK(m)	{ if(m) OSA_mutexUnlock(m);}
static Int32 mc_setByConfigId(mc_handle *pMcHdl, Int32 configId, Int32 value, Uint32 timeout, Int32 maskStatus, Bool bMutex)
{
	Int32 status = OSA_SOK;
	
	if(configId >= 256*16)
	{
		return OSA_EFAIL;
	}

	if(pMcHdl->status == OSA_EFAIL)
		return OSA_EFAIL;

	{
		Uint8 data[8];

		MUTEX_LOCK(&pMcHdl->mutex);

		if(!(pMcHdl->status & maskStatus))
		{
			MUTEX_UNLOCK(&pMcHdl->mutex);			
			return OSA_EFAIL;
		}		

		MP_flag(data) = 0x55;
		MP_blkId(data) = CFGID_blkId(configId);
		MP_feildId(data) = CFGID_feildId(configId);
		MP_value(data) = value;
		MP_check(data);

		OSA_assert(pMcHdl->curConfigId == 0);
		pMcHdl->curConfigId = configId;
		status = port_tsk_send(pMcHdl->portTsk, data, 8, OSA_TIMEOUT_FOREVER);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x send failed!\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
			pMcHdl->curConfigId = 0;

			MUTEX_UNLOCK(&pMcHdl->mutex);			

			return status;
		}

		status = OSA_semWait(&pMcHdl->semMsg, timeout);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x timeout !\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
		}
		pMcHdl->curConfigId = 0;

		MUTEX_UNLOCK(&pMcHdl->mutex);			
	}

	return status;
}

static Int32 mc_getByConfigId(mc_handle *pMcHdl, Int32 configId, Int32 *value, Uint32 timeout, Int32 maskStatus, Bool bMutex)
{
	Int32 status = OSA_SOK;
	
	if(configId >= 256*16 || value == NULL)
	{
		return OSA_EFAIL;
	}
	if(configId < 128*16 && pMcHdl->sysconfigIsValid)
	{
		*value = pMcHdl->sysConfig[configId];
		return OSA_SOK;
	}

	if(pMcHdl->status == OSA_EFAIL)
		return OSA_EFAIL;	

	{
		Uint8 data[8] = {0x55, 0xff, 0, 0, 0, 0, 0, 0};

		MUTEX_LOCK(&pMcHdl->mutex);

		if(!(pMcHdl->status & maskStatus))
		{
			MUTEX_UNLOCK(&pMcHdl->mutex);			
			return OSA_EFAIL;
		}

		MP_Q_blkId(data) = CFGID_blkId(configId);
		MP_Q_feildId(data) = CFGID_feildId(configId);
		MP_check(data);

		OSA_assert(pMcHdl->curConfigId == 0);
		pMcHdl->curConfigId = configId;
		status = port_tsk_send(pMcHdl->portTsk, data, 8, OSA_TIMEOUT_FOREVER);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x send failed!\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
			pMcHdl->curConfigId = 0;

			MUTEX_UNLOCK(&pMcHdl->mutex);			

			return status;
		}

		status = OSA_semWait(&pMcHdl->semMsg, timeout);
		if(OSA_SOK != status)
		{
			OSA_printf("%s %d: mc%d configId 0x%x timeout !\r\n", 
				__func__,OSA_getCurTimeInMsec(), pMcHdl->mcId, configId);
		}
		else
		{
			*value = pMcHdl->sysConfig[configId];
		}
		pMcHdl->curConfigId = 0;

		MUTEX_UNLOCK(&pMcHdl->mutex);		
	}

	return status;
}

static Int32 mc_remote2Local(mc_handle *pMcHdl, Int32 maskStatus)
{
	Int32 configId, status = OSA_SOK;

	OSA_printf("%s %d: mc%d ...", __func__,OSA_getCurTimeInMsec(), pMcHdl->mcId);

	for(configId=0; configId < 128*16; configId++)
	{
		status = mc_getByConfigId(pMcHdl, configId, &pMcHdl->sysConfig[configId], MC_TIME_OUT, maskStatus, FALSE);
		if(OSA_SOK != status)
			break;
	}

	if(OSA_SOK == status)
		syscfg_save(pMcHdl->sysconfigFile, pMcHdl->sysConfig, 128*16*4);

	OSA_printf("%s %d: mc%d ... done.\r\n", __func__,OSA_getCurTimeInMsec(), pMcHdl->mcId);

	return status;
}

Int32 mc_save(mc_handle *pMcHdl)
{
	Int32 status = OSA_SOK;

	status = mc_setByConfigId(pMcHdl, CFGID_BUILD(9,7), 1, MC_TIME_OUT, MCSTAT_READY, TRUE);
	
	if(OSA_SOK == status)
		syscfg_save(pMcHdl->sysconfigFile, pMcHdl->sysConfig, 128*16*4);

	return status;
}

static Int32 mc_local2Remote(mc_handle *pMcHdl, Int32 maskStatus)
{
	Int32 configId, status = OSA_SOK;

	OSA_printf("%s %d: mc%d ...", __func__,OSA_getCurTimeInMsec(), pMcHdl->mcId);

	for(configId=0; configId < CFGID_BUILD(8,15); configId++)
	{
		status = mc_setByConfigId(pMcHdl, configId, pMcHdl->sysConfig[configId], MC_TIME_OUT, maskStatus, FALSE);
		if(OSA_SOK != status)
			break;
	}

	for(configId=CFGID_BUILD(10,0); configId < CFGID_BUILD(127,15); configId++)
	{
		status = mc_setByConfigId(pMcHdl, configId, pMcHdl->sysConfig[configId], MC_TIME_OUT, maskStatus, FALSE);
		if(OSA_SOK != status)
			break;
	}

	if(OSA_SOK == status)
	{
		configId = CFGID_SYS_SAVE_STARTUP_AS;
		status = mc_setByConfigId(pMcHdl, configId, 1, MC_TIME_OUT, maskStatus, FALSE);
	}

	OSA_printf("%s %d: mc%d ... done.\r\n", __func__,OSA_getCurTimeInMsec(), pMcHdl->mcId);

	return status;
}

static Int32 mc_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
{
    UInt32 cmd;
    Bool ackMsg, done;
    Int status = 0;
    mc_handle *pMcHdl = (mc_handle *) pTsk->appData;

    OSA_printf("%s: mc%d Entered\r\n", __func__, pMcHdl->mcId);

    done = FALSE;
    ackMsg = FALSE;

    OSA_tskAckOrFreeMsg(pMsg, status);	

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);

        switch (cmd)
        {
            case MC_MSGID_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case MC_MSGID_PORT_DATA:
		status =  mc_portDataRecv( (mc_handle*)OSA_msgGetPrm(pMsg));
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }


    if (ackMsg && pMsg != NULL)
        OSA_tskAckOrFreeMsg(pMsg, status);

    OSA_printf("%s %d: mc%d exit !\r\n", __func__,OSA_getCurTimeInMsec(), pMcHdl->mcId);

    return 0;
}


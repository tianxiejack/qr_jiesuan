#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/watchdog.h>
#include <sys/stat.h>
#include <signal.h>

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <wtd_tsk.h>

#define WTD_TSK_PRI		2
#define WTD_TSK_STACK_SIZE  (0)

typedef struct wtd_tsk_obj{
	OSA_TskHndl tsk;

	int timeout;
}wtd_tsk_obj;

static int fd;
static wtd_tsk_obj *g_WtdTskObj = NULL;

static int wtd_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState );

void catch_int(int signum)
{
	signal(SIGINT, catch_int);

	OSA_printf("In signal handler\n");
	if(0 != close(fd))
		OSA_printf("Close failed in signal handler\n");
	else
		OSA_printf("Close succeeded in signal handler\n");
}

int wtd_tsk_create(Uint16 timeout)
{
	Int32 status;
	Int32 ret_val;
	
	wtd_tsk_obj *tskObj;

	tskObj = (wtd_tsk_obj *)malloc(sizeof(wtd_tsk_obj));

	memset(tskObj, 0, sizeof(*tskObj));

	tskObj->timeout  = (timeout<5)?5:timeout;
	
	status = OSA_tskCreate(&tskObj->tsk,
	                       wtd_tskMain,
	                       WTD_TSK_PRI,
	                       WTD_TSK_STACK_SIZE, 0, tskObj);
    
	OSA_assert(status == OSA_SOK);

	g_WtdTskObj = tskObj;

	 fd=open("/dev/watchdog",O_WRONLY);
        if (fd==-1) {
                perror("watchdog");
                return -1;
        }

        ret_val = ioctl (fd, WDIOC_SETTIMEOUT, &g_WtdTskObj->timeout);
        if (ret_val) {
	        OSA_printf ("Watchdog Timer : WDIOC_SETTIMEOUT failed");
        }
        else {
	        OSA_printf ("Watchdog new timeout value is : %d seconds", g_WtdTskObj->timeout);
        }

	OSA_tskSendMsg(&tskObj->tsk, &tskObj->tsk, (Uint16)WTD_MSGID_CREATE, NULL, 0);	

	return 0;	
}

void wtd_tsk_destroy()
{
	Int32 retVal;
	wtd_tsk_obj *tskObj = g_WtdTskObj;

	if(tskObj == NULL)
		return;

	retVal = OSA_tskSendMsg(&tskObj->tsk, &tskObj->tsk, (Uint16)WTD_MSGID_DELETE, NULL, OSA_MBX_WAIT_ACK);
	OSA_assert(retVal == OSA_SOK);

	OSA_tskDelete(&tskObj->tsk);	

	free(tskObj);
}

int wtd_tsk_clear()
{
	int status;
	wtd_tsk_obj *tskObj = g_WtdTskObj;

	if(tskObj == NULL)
		return OSA_EFAIL;
	
	status = OSA_tskSendMsg( &tskObj->tsk, &tskObj->tsk, (Uint16)WTD_MSGID_CLEAR, NULL, 0);	

	return status;
}

static int wtd_tskMain(struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
{
    UInt32 cmd;
    Int32 datalen;
    Bool ackMsg, done;
    Int status = 0;
    Int icou =0;
    OSA_printf("%s:Entered", __func__);

    done = FALSE;
    ackMsg = FALSE;

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
            case WTD_MSGID_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            case WTD_MSGID_CLEAR:
        		if (1 != write(fd, "\0", 1))
        		{
        			done = 1;
        			OSA_printf("WTD Write failed\n");
        		}
        		else if(icou++%1000==0)
        		{
        			OSA_printf("WTD Write succeeded\n");
        		}
        		OSA_tskAckOrFreeMsg(pMsg, status);
        		break;

            default:
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    close(fd);
	
    if (ackMsg && pMsg != NULL)
        OSA_tskAckOrFreeMsg(pMsg, status);

    OSA_printf(" %d: %s   : exit !!!\n",
               OSA_getCurTimeInMsec(), __func__);

    return 0;
}




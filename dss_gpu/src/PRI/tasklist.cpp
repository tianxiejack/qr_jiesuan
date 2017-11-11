


//#include "Tasklist.h"


#include "osa_thr.h"
#include <glut.h>
#include "osd_graph.h"
#include "app_proj_xgs026.h"
#include "osdProcess.h"
#include "Tasklist.h"
#include "UartMessage.h"



//extern  Multich_graphic grpxChWinPrms;
#if 0
static GrenadePort_t GrenadeObj;
SYSTRC_Handle pSysTrcObj= NULL;
static LaserPort_t LaserObj;
static int LaserDistance=-1;
static PositionPort_t PositionObj = {0};
static TurretPosPort_t TurretPosObj;
static WeaponCtrlPort_t WeaponCtrlObj={0};
#endif

//global vars
#if 0
dramCmdMsg_t 	dramMsg;
MuxPotrTran * 	pMuxTran  			= NULL;
PIPE_Handle 	pDataPipe			= NULL;
AVTPORT_Handle  pAvtPortObj 		= NULL;
DRAMPORT_Handle pDramPortObj		= NULL;
TRACEPORT_Handle pTracePortObj		= NULL;
TestBak1PORT_Handle pTestBak1PortObj	= NULL;
TestBak2PORT_Handle pTestBak2PortObj	= NULL;
TestBak3PORT_Handle pTestBak3PortObj	= NULL;
DRAMCTRL_Handle pDramCtrlObj		= NULL; 
AVTCTRL_Handle  pAvtCtrlObj 		= NULL;

AVTDEV_Handle   pAvtDevObj			= NULL;
SYSTRC_Handle 	pSysTraceObj		=NULL;
FOVCTRL_Handle	 pFovCtrlObj 		= NULL;
IMGSCAL_Handle pScalObj 			= NULL;

#endif

//MSGDRIV_Handle 	pMDrvObj			= NULL;
WeaponCtrlPORT_Handle pWeaponPortObj= NULL;
GrenadePORT_Handle pGrenadePortObj	= NULL;
LaserPORT_Handle pLaserPortObj		= NULL;
MachGunPORT_Handle pMachGunPortObj	= NULL;
TurretPosPORT_Handle pTurretPosPortObj = NULL;
PositionPort_Handle pPositionPort	= NULL;

//OSDCTRL_Handle  pOsdCtrlObj		= NULL;/osdProcess  define

volatile unsigned int g_InterEnable=0;

//Image Strch Buffer
#define IMAGE_STRCHING_TABAADR  	0x90080D00

unsigned char* g_pTab=NULL;
unsigned char g_ImgStrchTab[256];

//volatile IMGSTRCH_OBJ g_IStrch;
//IMGSOBEL_OBJ g_ISobel;
//PICP_Handle picp 	= NULL;
//CTimerCtrl * pTimerObj =NULL;


SYSCTRL_Handle   pSysCtrlObj = NULL;
  


static OSA_ThrHndl GrenadePORTHndl;
static OSA_ThrHndl LaserPORTHndl;
static OSA_ThrHndl PositionPORTHndl;
static OSA_ThrHndl TurretPosPORTHndl;
static OSA_ThrHndl WeaponPORTHndl;
static OSA_ThrHndl MachGunPORTHndl;
static OSA_ThrHndl TestHndl;
static OSA_ThrHndl NetHandl;
static OSA_ThrHndl testCanHandl;



int mainCtrl(void* prm)
{
	taskCreate(prm);
	glutMainLoop();

	taskDestroy();
	return 0;
}

void *testOsd(void* prm)
{
	return NULL;
}


int taskCreate(void * prm)
{
	int status = -1;

	/*
	status = OSA_thrCreate(
				&GrenadePORTHndl,
				GrenadePORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
	*/
	
	/*
	status = OSA_thrCreate(
				&LaserPORTHndl,
				LaserPORT_recvTask,
				0,
				0,
				0
			);
	OSA_assert(status==OSA_SOK);
	*/
	/*
	status = OSA_thrCreate(
				&PositionPORTHndl,
				PositionPORT_recvTask,
				0,
				0,
				0
			);
	//OSA_assert(status==OSA_SOK);
	*/
	/*
		status = OSA_thrCreate(
					&TurretPosPORTHndl,
					TurretPosPORT_recvTask,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);
	*/
	/*
		status = OSA_thrCreate(
					&MachGunPORTHndl,
					MachGunPORT_recvTask,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);
	*/
	/*
		status = OSA_thrCreate(
					&WeaponPORTHndl,
					WeaponCtrlPORT_recvTask,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);
	*/
	/*
		status = OSA_thrCreate(
					&TestHndl,
					fdTest_open,
					//testOsd,
					0,
					0,
					0
				);
		OSA_assert(status==OSA_SOK);
	*/


	/*
		status = OSA_thrCreate(
					&NetHandl,
					tcpTestFunc,
					0,
					0,
					prm
				);
		OSA_assert(status==OSA_SOK);
	*/

	status = OSA_thrCreate(
				&testCanHandl,
				SPI_CAN_process,
				0,
				0,
				prm
			);
	OSA_assert(status==OSA_SOK);

	return 0;
}


int taskDestroy(void)
{

	int status;
	status = OSA_thrDelete(&GrenadePORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&LaserPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&PositionPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&TurretPosPORTHndl);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrDelete(&WeaponPORTHndl);
	OSA_assert(status == OSA_SOK);

	//status = OSA_thrDelete(&MsgdrivHndl);
	//OSA_assert(status == OSA_SOK);

	return 0;
}



int GetDataFromUart(int uartHandle, Uint8 *pData, int iLen, int *pStat)
{
#if 0
	int iRet;
	*pStat = UartObj[uartHandle].IsRxRngLost;
	iRet = rngBufGet(UartObj[uartHandle].RxRng, (char *)pData, iLen);
	UartObj[uartHandle].IsRxRngLost = 0;
	return iRet;
#endif
	return 0;
}





/*=======================================================
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:		dx.c
* Author:			wzk
* Date:			2014-01-14
* Description:		mcfw midle ware, has a message driver task,
*				to recv and dispatch message to the registered callback fun,
*				this midware one point connect to UI, another point connect mcfw firmware
* Version:			version 1.0.0
* Function List:
*                   1.	Int32 Dx_update_field( Int32 blkId, Int32 fieldno, Bool notify )
*				2.	static Int32 Dx_syscfgSaveConfigAs( Int32 blkId, Int32 feildId )
*				3.	Int32 Dx_syscfgSelStartUp( Int32 blkId, Int32 feildId )
*				4.	Int32 Dx_syscfgSaveStartUpAs( Int32 blkId, Int32 feildId )
*				5.	Int32 Dx_syscfgLoadConfigAs( Int32 blkId, Int32 feildId )
*				6.	Int32 Dx_openMport( void )
*				7.	Int32 Dx_closeMport( void )
*				8.	static Int32 config_default_set( void )
*				9.	Int32 Dx_run( )
*				10.	Int32 Dx_stop( )
*				11.	Int32 Dx_sendCfgMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, DX_CFG *pCfg, Bool bAck )
*				12.	Int32 Dx_sendMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, Void *pPrm, Int32 prmSize, Bool bAck )
*				13.	Int32 Dx_postMsg( Int32 msgId, Void *pPrm, Int32 prmSize )
*				14.	int Dx_portDataRecvSend( int iPort )
*				15.	int Dx_tskMain( struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
*
*
* History:			// histroy modfiy record
*     <author>  <time>   <version >   <desc>
*       wzk     14/01/4     1.0.0    build this module
*	xavier	14/01/4	   1.0.1	update this module
   =======================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <dx.h>
#include <dx_priv.h>
#include <data_port.h>
#include "ipc_port.h"
#include "app_user.h"
#include "osdPort.h"
#include "statCtrl.h"

#define DX_TSK_PRI          2
#define DX_TSK_STACK_SIZE   ( 0 )
DX_CFG dxCfg[8];

typedef struct Dx_Obj
{
    OSA_TskHndl     tsk;
    volatile Int32  status;
    SCRIPT_OBJ      *scripts;
    Int32           *sysConfig;
    UpdateConfigFxn *fxnsCfg;

#ifdef INCLUDE_MAIN_PORT

    port_handle *mainPort[3];
    OSA_TskHndl *mainPortTsk[3];
#endif

    port_handle *dataPort;

    port_handle *ipcPort;
    OSA_TskHndl *ipcPortTsk;

    UInt32  tmReg[DX_TIMER_MAX];
    UInt32  tmCnt[DX_TIMER_MAX];
    UInt32  tmBegen[DX_TIMER_MAX];

    UInt32 cntFrame[48];
}
Dx_Obj;

static Dx_Obj   gDxObj ;
void initdxobj()
{
gDxObj.status = -1;

}

volatile Int32	gCurStartUpId	   = 0;
static UInt32	cDataPortTimeout   = 500; //ms

extern int dxTimer_create( int interval, void ( *fnxCall )( void ) );
extern void dxTimer_destroy( );
int Dx_tskMain( struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState );
static void Dx_OnTimer( void );
static Int32 Dx_updateCfgDownloadFile( Int32 blkId, Int32 feildId );
static Int32 Dx_updateCfgUploadFile( Int32 blkId, Int32 feildId );
static Int32 Dx_updateCfgDownloadData( Int32 blkId, Int32 feildId );
static Int32 Dx_updateCfgUploadData( Int32 blkId, Int32 feildId );
static Int32 config_default_set( void );

#if 0
GeneralCorrectionItem gGrenadeKill_GCTable[]=
{
	{0,		{0,0}},
	{100, 	{0,0}},
	{200, 	{0,0}},
	{300, 	{0,0}},
	{400, 	{0,0}},
	{500, 	{0,0}},
	{600, 	{0,0}},
	{700, 	{0,0}},
	{800, 	{0,0}},
	{900, 	{0,0}},
	{1000, 	{0,0}},
	{1100, 	{0,0}},
	{1200, 	{0,0}},
	{1300, 	{0,0}},
	{1400, 	{0,0}},
	{1500, 	{0,0}},
	{1600, 	{0,0}},
	{1700, 	{0,0}},
	{1791,	{0,0}}
};

GeneralCorrectionItem gGrenadeGas_GCTable[]=
{
		{0,	{0,0}},
		{25, {0,0}},
		{50, {0,0}},
		{75, {0,0}},
		{100, {0,0}},
		{125, {0,0}},
		{150, {0,0}},
		{175, {0,0}},
		{200, {0,0}},
		{225, {0,0}},
		{250, {0,0}},
		{275, {0,0}},
		{300, {0,0}},
		{325, {0,0}},
		{350, {0,0}},
		{364, {0,0}}
};
#endif
//ZeroCorrectionItem gMachineGun_ZCTable = {500,	{352,288}};
//ZeroCorrectionItem gGrenadeKill_ZCTable = {500,	{352,288}};
//WeatherItem gWeatherTable={15,101325};

/**
 * @Fun:		Dx_update_field
 * @brief:		update block field value
 * @param:	Int32 blkId: block Id
 * @param:	Int32 fieldno: field no
 * @Output	Bool notify: if notify enable
 * @return:	OSA_SOK
 * @Others:	NULL
 */
Int32 Dx_update_field( Int32 blkId, Int32 fieldno, Bool notify )
{
    Int32 tag;
    OSA_assert( gDxObj.status != -1 );

    gDXD_info.sysConfig[CFGID_BUILD( blkId, 0 )] |= ( 1 << fieldno );

    tag = gDXD_info.sysConfig[CFGID_BUILD( blkId, 0 )];

    if( notify && ( tag & ( 1 << ( fieldno + 16 ) ) ) )
    {
    	
        Int32 cfgId = CFGID_BUILD( blkId, fieldno );
        return Dx_sendMsg( &gDxObj.tsk, DX_MSGID_UPDATE_FIELD, (void *)cfgId, sizeof( cfgId ), FALSE );
    }

    return OSA_SOK;
}

/**
 * @Fun:		Dx_syscfgSaveConfigAs
 * @brief:		Save system configure
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	NULL
 * @return:	return cfg file write status
 * @Others:	NULL
 */
static Int32 Dx_syscfgSaveConfigAs( Int32 blkId, Int32 feildId )
{
    Int32 iRet = OSA_SOK;
    char syscmd[256]={0};

    if(gDXD_info.sysConfig == NULL )
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        return OSA_EFAIL;
    }

    system("cd /opt/dvr_rdk/ti816x");
    system("./zeMount.sh");
	FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 1;
    if( FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) == 1 )
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        iRet = syscfg_save( SYSCONFIG_FILE1, gDXD_info.sysConfig, 128 * 16 * 4 );
        sprintf(syscmd,"cp %s %s",SYSCONFIG_FILE1,SYSCFG_SAVE_FILE1);
    }
    else
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        iRet = syscfg_save( SYSCONFIG_FILE2, gDXD_info.sysConfig, 128 * 16 * 4 );
        sprintf(syscmd,"cp %s %s",SYSCONFIG_FILE1,SYSCFG_SAVE_FILE2);
    }

    OSA_printf("%s dx run %s",__func__,syscmd);
    system(syscmd);
    system("cd /opt/dvr_rdk/ti816x");
    system("./zeUMount.sh");
    return iRet;
}



static Int32 Dx_syscfgSaveConfig026( Int32 blkId, Int32 feildId )
{
    Int32 iRet = OSA_SOK;
    char syscmd[256]={0};
    OSA_printf("****************%s ******************\n",__func__);
    if(gDXD_infoSave.sysConfig == NULL )
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        return OSA_EFAIL;
    }
	FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 1;
    if( FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) == 1 )
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        iRet = syscfg_save( SYSCONFIG_FILE1, gDXD_infoSave.sysConfig, 128*2* 16 * 4 ); 
    }
    else
    {
        FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS) = 0;
        iRet = syscfg_save( SYSCONFIG_FILE2, gDXD_infoSave.sysConfig, 128 *2* 16 * 4 );
    }


    return iRet;
}

/**
 * @Fun:		Dx_syscfgReboot
 * @brief:		Save system configure
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	NULL
 * @return:	return cfg file write status
 * @Others:	NULL
 */
static Int32 Dx_syscfgReboot( Int32 blkId, Int32 feildId )
{
    system("reboot");
    return 0;
}

/**
 * @Fun:	Dx_syscfgWtdEnable
 * @brief:	
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	NULL
 * @return:	return cfg file write status
 * @Others:	NULL
 */
static Int32 Dx_syscfgWtdEnable( Int32 blkId, Int32 feildId )
{

    int configId   = CFGID_BUILD( blkId, feildId );

    OSA_printf( " %s wtd enable stat %d\n", __func__, gDXD_info.sysConfig[configId] );

    return 0;
}

/**
 * @Fun:	Dx_syscfgWtdTimeOut
 * @brief:	
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	NULL
 * @return:	return cfg file write status
 * @Others:	NULL
 */
static Int32 Dx_syscfgWtdTimeOut( Int32 blkId, Int32 feildId )
{
    int configId   = CFGID_BUILD( blkId, feildId );

    OSA_printf( " %s wtd timeout %ds\n", __func__, gDXD_info.sysConfig[configId] );
    return 0;
}

/**
 * @Fun:	Dx_syscfgWtdSleepTime
 * @brief:	
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	NULL
 * @return:	return cfg file write status
 * @Others:	NULL
 */
static Int32 Dx_syscfgWtdSleepTime( Int32 blkId, Int32 feildId )
{
    int configId   = CFGID_BUILD( blkId, feildId );

    OSA_printf( " %s set wtd sleep %ds\n", __func__, gDXD_info.sysConfig[configId] );
    return 0;
}

/**
 * @Fun:		Dx_syscfgSelStartUp
 * @brief:		select system start up configure
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	null
 * @return:	return cfg file write status
 * @Others:	null
 */
static Int32 Dx_syscfgSelStartUp( Int32 blkId, Int32 feildId )
{
    Int32 *data = (Int32*)malloc( sizeof( Int32 ) * 256 * 16 );
    memset( data, 0, sizeof( Int32 ) * 256 * 16 );

    syscfg_load( SYSCONFIG_FILE0, data, 128 * 16 * 4 );
    data[CFGID_BUILD( 9, 5 )]  = FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP);
    gCurStartUpId			   = FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP);
    OSA_printf( " %s set startup Id = %d\n", __func__, gCurStartUpId );

    FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP) = 0;
    return syscfg_save( SYSCONFIG_FILE0, data, 128 * 16 * 4 );
}

/**
 * @Fun:		Dx_syscfgSaveStartUpAs
 * @brief:		 system start up configure save
 * @param:	Int32 blkId:unused
 * @param:	Int32 feildId:unused
 * @Output	null
 * @return:	return cfg file write status
 * @Others:	null
 */
static Int32 Dx_syscfgSaveStartUpAs( Int32 blkId, Int32 feildId )
{
    FIELD_ITEM_SYS(CFGID_SYS_SAVE_CONFIG_AS)   = FIELD_ITEM_SYS(CFGID_SYS_SAVE_STARTUP_AS);
    FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP)	   = FIELD_ITEM_SYS(CFGID_SYS_SAVE_STARTUP_AS);
    FIELD_ITEM_SYS(CFGID_SYS_SAVE_STARTUP_AS)  = 0;
    Dx_syscfgSaveConfigAs( 9, 6 );
    return Dx_syscfgSelStartUp( 9, 5 );
}

/**
 * @Fun:		Dx_syscfgLoadConfigAs
 * @brief:		system start up configure load
 * @param:	Int32 blkId: block Id
 * @param:	Int32 fieldno: field no
 * @Output	null
 * @return:	return cfg file write status
 * @Others:	null
 */
static Int32 Dx_syscfgLoadConfigAs( Int32 blkId, Int32 feildId )
{
    Int32 status = OSA_SOK;

    if( gDXD_info.sysConfig == NULL )
    {
        FIELD_ITEM_SYS(CFGID_SYS_LOAD_FROM) = 0;
        return OSA_EFAIL;
    }

    if( FIELD_ITEM_SYS(CFGID_SYS_LOAD_FROM) == 0 )
    {
        status = syscfg_load( SYSCONFIG_FILE0, gDXD_info.sysConfig, 128 * 16 * 4 );
    }
    else if( FIELD_ITEM_SYS(CFGID_SYS_LOAD_FROM) == 1 )
    {
        status = syscfg_load( SYSCONFIG_FILE1, gDXD_info.sysConfig, 128 * 16 * 4 );
    }
    else
    {
        status = syscfg_load( SYSCONFIG_FILE2, gDXD_info.sysConfig, 128 * 16 * 4 );
    }
    FIELD_ITEM_SYS(CFGID_SYS_LOAD_FROM)	   = 0;
    FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP)  = 0;

    if( gDXD_info.reloadCfg != NULL )
    {
        gDXD_info.reloadCfg( blkId, feildId );
    }

    return status;
}

/**
 * @Fun:	Dx_openMport
 * @brief:	open main port
 * @param:	null
 * @param:	null
 * @Output	null
 * @return:	OSA_SOK,OSA_EFAIL
 * @Others:	null
 */
Int32 Dx_openMport( void )
{
    Int32 status = OSA_SOK;
#ifdef INCLUDE_MAIN_PORT

    {
        ipc_open_params     mIpcOpenParams;
        uart_open_params	mUartOpenParams;
        udp_open_params		mUdpOpenParams;
        mUartOpenParams.baudrate   = 115200;
        mUartOpenParams.databits   = 8;
        mUartOpenParams.parity	   = 'E';
        mUartOpenParams.stopbits   = 1;

        mUdpOpenParams.interfaceIp = 0;
        mIpcOpenParams.msgKey      = 0x82345;




#if 0
        //#ifdef MAIN_PORT1_ENABLE
        status = port_create( PORT_UART, &gDxObj.mainPort[0] );
        OSA_assert( status == OSA_SOK );
        OSA_assert( gDxObj.mainPort[0] != NULL );
        strcpy( mUartOpenParams.device, MAIN_PORT1_DEV );
        status = gDxObj.mainPort[0]->open( gDxObj.mainPort[0], &mUartOpenParams );
        OSA_assert( status == OSA_SOK );
        gDxObj.mainPortTsk[0] = port_tsk_create( gDxObj.mainPort[0], &gDxObj.tsk, DX_MSGID_PORT_DATA, (void*)0 );
        OSA_assert( gDxObj.mainPortTsk[0] != NULL );
#endif

#ifdef MAIN_PORT2_ENABLE

        status = port_create( PORT_UART, &gDxObj.mainPort[1] );
        OSA_assert( status == OSA_SOK );
        strcpy( mUartOpenParams.device, MAIN_PORT2_DEV );
        status = gDxObj.mainPort[1]->open( gDxObj.mainPort[1], &mUartOpenParams );
        OSA_assert( status == OSA_SOK );
        gDxObj.mainPortTsk[1] = port_tsk_create( gDxObj.mainPort[1], &gDxObj.tsk, DX_MSGID_PORT_DATA, (void*)1 );
        OSA_assert( gDxObj.mainPortTsk[1] != NULL );
#endif

#ifdef MAIN_PORT_UDP

        status = port_create( PORT_NET_UDP, &gDxObj.mainPort[2] );
        OSA_assert( status == OSA_SOK );
        mUdpOpenParams.port	   = MAIN_PORT_UDP_IPORT;
        status				   = gDxObj.mainPort[2]->open( gDxObj.mainPort[2], &mUdpOpenParams );
        OSA_assert( status == OSA_SOK );
        gDxObj.mainPortTsk[2] = port_tsk_create( gDxObj.mainPort[2], &gDxObj.tsk, DX_MSGID_PORT_DATA, (void*)2 );
        OSA_assert( gDxObj.mainPortTsk[2] != NULL );
#endif

#ifdef DATA_PORT

        status = port_create( PORT_NET_UDP, &gDxObj.dataPort );
        OSA_assert( status == OSA_SOK );
        mUdpOpenParams.port	   = DATA_PORT_IPORT;
        status				   = gDxObj.dataPort->open( gDxObj.dataPort, &mUdpOpenParams );
        OSA_assert( status == OSA_SOK );
#endif

#ifdef IPC_PORT

        status = port_create( PORT_IPC, &gDxObj.ipcPort);
        OSA_assert( status == OSA_SOK );

        status              = gDxObj.ipcPort->open( gDxObj.ipcPort, &mIpcOpenParams );
        OSA_assert( status != OSA_EFAIL );

        gDxObj.ipcPortTsk = port_tsk_create( gDxObj.ipcPort, &gDxObj.tsk, DX_MSGID_PORT_DATA, (void*)2 );
        gDxObj.mainPort[2] = gDxObj.ipcPort;
        gDxObj.mainPortTsk[2] = gDxObj.ipcPortTsk ;

        OSA_assert( gDxObj.ipcPort != NULL );
#endif

    }
#endif
    return status;
}

/**
 * @Fun:		Dx_closeMport
 * @brief:		close main port
 * @param:	null
 * @param:	null
 * @Output	null
 * @return:	OSA_SOK,OSA_EFAIL
 * @Others:	null
 */
Int32 Dx_closeMport( void )
{
    Int32 status = OSA_SOK;

#ifdef INCLUDE_MAIN_PORT

    {
#ifdef MAIN_PORT1_ENABLE

        port_tsk_destroy( gDxObj.mainPortTsk[0] );
        gDxObj.mainPortTsk[0] = NULL;
        gDxObj.mainPort[0]->close( gDxObj.mainPort[0] );
        port_destory( gDxObj.mainPort[0] );
        gDxObj.mainPort[0] = NULL;

#endif

#ifdef MAIN_PORT2_ENABLE

        port_tsk_destroy( gDxObj.mainPortTsk[1] );
        gDxObj.mainPortTsk[1] = NULL;
        gDxObj.mainPort[1]->close( gDxObj.mainPort[1] );
        port_destory( gDxObj.mainPort[1] );
        gDxObj.mainPort[1] = NULL;
#endif

#ifdef MAIN_PORT_UDP

        port_tsk_destroy( gDxObj.mainPortTsk[2] );
        gDxObj.mainPortTsk[2] = NULL;
        gDxObj.mainPort[2]->close( gDxObj.mainPort[2] );
        port_destory( gDxObj.mainPort[2] );
        gDxObj.mainPort[2] = NULL;
#endif

#ifdef DATA_PORT

        gDxObj.dataPort->close( gDxObj.dataPort );
        port_destory( gDxObj.dataPort );
        gDxObj.dataPort = NULL;
#endif
#ifdef IPC_PORT

        gDxObj.dataPort->close( gDxObj.ipcPort );
        port_tsk_destroy( gDxObj.ipcPortTsk);
        port_destory( gDxObj.ipcPort );
        gDxObj.ipcPort = NULL;
#endif

    }
#endif
    return status;
}

/**
 * @Fun:		config_default_set
 * @brief:		config_default_set
 * @param:	null
 * @param:	null
 * @Output	null
 * @return:	OSA_SOK
 * @Others:	null
 */
static Int32 config_default_set( void )
{
    FIELD_ITEM_SYS(CFGID_SYS_CFG_TAG)	   = -1;
    FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP)  = 1;

    return OSA_SOK;
}

/**
 * @Fun:		Dx_run
 * @brief:		start Dx utils module
 * @param:	null
 * @param:	null
 * @Output	null
 * @return:	return Dx run status, OK or FAILURE
 * @Others:
 */
Int32 Dx_run( )
{
    Int32	status;
    initdxobj();
    Dx_Obj	*pObj = &gDxObj;
    OSA_assert( gDxObj.status == -1 );

    memset( &gDxObj, 0, sizeof( gDxObj ) );

    OSA_printf( "%s: script create ...\n", __func__ );
    script_create( SCRIPT_FILE, &gDxObj.scripts );
    gDXD_info.scripts = (Int32*)gDxObj.scripts;
    OSA_printf( "%s: script create ... done !!!\n", __func__ );
    gDXD_info.sysConfig = gDxObj.sysConfig = (Int32*)malloc( sizeof( Int32 ) * 256 * 16 );
    memset( gDxObj.sysConfig, 0, sizeof( Int32 ) * 256 * 16 );

     gDXD_infoSave.sysConfig = gDXD_infoSave.sysConfig = (Int32*)malloc( sizeof( Int32 ) * 256 * 16 );
    memset( gDXD_infoSave.sysConfig, 0, sizeof( Int32 ) * 256 * 16 );

   
    

    gDXD_info.fxnsCfg = gDxObj.fxnsCfg = (UpdateConfigFxn*)malloc( sizeof( UpdateConfigFxn ) * 256 * 16 );
    memset( gDxObj.fxnsCfg, 0, sizeof( UpdateConfigFxn ) * 256 * 16 );

    config_default_set( );

    if( gDXD_info.OnLoadDefCfg != NULL )
    {
        gDXD_info.OnLoadDefCfg(0 );
    }

    gCurStartUpId = FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP);
    OSA_printf( "%s current startup Id = %d !!! \n", __func__, gCurStartUpId );

    if( FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP) == 1 )
    {
        syscfg_load( SYSCONFIG_FILE1, gDXD_info.sysConfig, 256 * 16 * 4 );
	printf("*************************************%s  loadsysconfig\n",__func__);

    }

    memcpy(gDXD_infoSave.sysConfig,gDXD_info.sysConfig,sizeof( Int32 ) * 256 * 16);
    FIELD_ITEM_SYS(CFGID_SYS_REBOOT) = 0;
    FIELD_ITEM_SYS(CFGID_SYS_AUTO_CHECK) = 0;

    FXN_BIGEN
    //FXN_REG( 4, 1, Dx_updateCfgShotCut );

    FXN_REG( 4,  2, Dx_updateCfgDownloadFile );
    FXN_REG( 4,  3, Dx_updateCfgUploadFile );
    FXN_REG( 4,  4, Dx_updateCfgDownloadData );
    FXN_REG( 4,  5, Dx_updateCfgUploadData );
    FXN_REG( 9,  5, Dx_syscfgSelStartUp );
    FXN_REG( 9,  6, Dx_syscfgSaveConfig026 );
    FXN_REG( 9,  7, Dx_syscfgSaveStartUpAs );
    FXN_REG( 9,  8, Dx_syscfgLoadConfigAs );
    FXN_REG( 9,  9, Dx_syscfgReboot );
    FXN_REG( 9, 10, Dx_syscfgWtdEnable );
    FXN_REG( 9, 11, Dx_syscfgWtdTimeOut );
    FXN_REG( 9, 12, Dx_syscfgWtdSleepTime );
    

    FXN_END

    status = OSA_tskCreate( &pObj->tsk,
                            Dx_tskMain,
                            DX_TSK_PRI,
                            DX_TSK_STACK_SIZE, 0, pObj );
    OSA_assert( status == OSA_SOK );

    Dx_sendMsg( NULL, DX_MSGID_CREATE, NULL, 0, FALSE );

    return status;
}





Int32 Dx_initparam( )
{
	Int32 status=0;
	
	APP_setPrms(0);
	return status;
}
/**
 * @Fun:	Dx_stop
 * @brief:	stop Dx utils module
 * @param:	null
 * @param:	null
 * @Output	null
 * @return:	return Dx run status, OK or Failure
 * @Others:
 */
Int32 Dx_stop( )
{
    Int32	status;
    Dx_Obj	*pObj = &gDxObj;
    OSA_assert( gDxObj.status != -1 );

    Dx_sendMsg( &pObj->tsk, DX_MSGID_DELETE, NULL, 0, TRUE );

    status = OSA_tskDelete( &pObj->tsk );

    gDxObj.status = -1;

    gDXD_info.scripts = NULL;
    if( gDxObj.scripts != NULL )
    {
        script_destroy( gDxObj.scripts );
    }
    gDxObj.scripts = NULL;
    free( gDxObj.sysConfig );
    gDXD_info.sysConfig = gDxObj.sysConfig = NULL;
    free( gDxObj.fxnsCfg );
    gDXD_info.fxnsCfg = gDxObj.fxnsCfg = NULL;

    return status;
}

/**
 * @Fun:		Dx_sendCfgMsg
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
Int32 Dx_sendCfgMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, DX_CFG *pCfg, Bool bAck )
{
    Uint16 flag = 0;
    OSA_assert( gDxObj.status >= 0 );
    OSA_assert( msgId == DX_MSGID_GET_CFG || msgId == DX_MSGID_SET_CFG );
    if( bAck )
    {
        flag = OSA_MBX_WAIT_ACK;
    }

    return OSA_tskSendMsg( &gDxObj.tsk, pPrcFrom, (Uint16)msgId, pCfg, flag );
}

/**
 * @Fun:		Dx_sendMsg
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */

Int32 Dx_sendMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, Void *pPrm, Int32 prmSize, Bool bAck )
{
    Uint16	flag = 0;
    OSA_assert( msgId < DX_MSGID_MAX );
    OSA_assert( gDxObj.status >= 0 );
    if( bAck )
    {
        flag = OSA_MBX_WAIT_ACK;
    }

    return OSA_tskSendMsg( &gDxObj.tsk, pPrcFrom, (Uint16)msgId, pPrm, flag );
}

/**
 * @Fun:		Dx_postMsg
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
Int32 Dx_postMsg( Int32 msgId, Void *pPrm, Int32 prmSize )
{
    Int32 status = 0;
    Int32 	configId;
    DX_CFG *pCfg = NULL;

    OSA_assert( msgId < DX_MSGID_MAX );

    switch( msgId )
    {
    case DX_MSGID_START:
        if( gDXD_info.OnStart != NULL )
        {
            status = gDXD_info.OnStart( *(int*)pPrm );
            if( status == OSA_SOK )
            {
                gDxObj.status = 1;
            }
        }
        Dx_openMport( );
        break;
    case DX_MSGID_STOP:
        Dx_closeMport( );
        if( gDXD_info.OnStop != NULL )
        {
            status		   = gDXD_info.OnStop(0);
            gDxObj.status  = 0;
        }
        break;
    case DX_MSGID_GET_STATUS:
        if( gDXD_info.OnGetStatus != NULL )
        {
            status = gDXD_info.OnGetStatus(0);
        }
        break;
    case DX_MSGID_CTRL:
        if( gDXD_info.OnControl != NULL )
        {
            status = gDXD_info.OnControl( (long)pPrm );
        }
        break;
    case DX_MSGID_GET_CFG:
        pCfg = (DX_CFG *)pPrm;
        configId	= pCfg->configId;
        if( configId < 256 * 16 )
        {
            pCfg->value = gDXD_info.sysConfig[configId];
            status = OSA_SOK;
        }
        else
        {
            OSA_printf(" %s :get cfg msg,Invalid configId %d \n",__func__,configId);
            status = OSA_EFAIL;
        }
        break;
    case DX_MSGID_SET_CFG:
        pCfg = (DX_CFG *)pPrm;
        configId	= pCfg->configId;
        if( configId < 256 * 16 )
        {
            status = OSA_SOK;
            gDXD_info.sysConfig[configId]  = pCfg->value;
            if(gDXD_info.fxnsCfg[configId] != NULL)
            {
                status = gDXD_info.fxnsCfg[configId](CFGID_blkId(configId),CFGID_feildId(configId));
            }
        }
        else
        {
            OSA_printf(" %s :set cfg msg,Invalid configId %d \n",__func__,configId);
            status = OSA_EFAIL;
        }
        break;

    case DX_MSGID_GET_IPC_CFG:
        pCfg = (DX_CFG *)pPrm;
        configId    = pCfg->configId;
        if ( configId < 256 * 16 )
        {
            pCfg->value = gDXD_info.sysConfig[configId];

            ipc_port_getByConfigId(gDxObj.ipcPort,configId,pCfg->value);

            status = OSA_SOK;
        }
        else
        {
            OSA_printf(" %s :get cfg msg,Invalid configId %d \n", __func__, configId);
            status = OSA_EFAIL;
        }
        break;

    case DX_MSGID_SET_IPC_CFG:
        pCfg = (DX_CFG *)pPrm;
        configId    = pCfg->configId;

        if ( configId < 256 * 16 )
        {
            status = OSA_SOK;
            gDXD_info.sysConfig[configId] = pCfg->value;

            status = ipc_port_setByConfigId(gDxObj.ipcPort,configId,pCfg->value);
        }
        else
        {
            OSA_printf(" %s :set cfg msg,Invalid configId %d \n", __func__, configId);
            status = OSA_EFAIL;
        }
        break;
    case DX_MSGID_TIMER:
        if( gDXD_info.OnTimer != NULL )
        {
            status = gDXD_info.OnTimer( (long)pPrm );
        }
        break;
    }

    return status;
}

#ifdef INCLUDE_MAIN_PORT
#define MP_flag( data )			( data[0] )
#define MP_blkId( data )		( data[1] )
#define MP_feildId( data )		( data[2] )
#define MP_value( data )		( *(Int32*)( data + 3 ) )
#define MP_check( data )		( data[7] = data[1] + data[2] + data[3] + data[4] + data[5] + data[6] )
#define MP_checkOk( data )		( data[7] == (Uint8)( data[1] + data[2] + data[3] + data[4] + data[5] + data[6] ) )
#define MP_Q_blkId( data )		( data[5] )
#define MP_Q_feildId( data )	( data[6] )
#endif

#ifdef INCLUDE_MAIN_PORT


/**
 * @Fun:		Dx_portDataRecvSend
 * @brief:		Main port snd and recv data
 * @param:	int iPort: port id
 * @param:
 * @Output	NULL
 * @return:	OSA_SOK or OSA_EFAIL
 * @Others:
 */
static int Dx_portDataRecvSend( int iPort )
{
    static int icout = 0;
    int		status = OSA_SOK;
    Uint8	data[64];

    if(++icout % 200000 == 0x00)
    {
        OSA_printf( "%s %d: enter!!!\r\n", __func__, OSA_getCurTimeInMsec( ) );
    }

    if ( gDxObj.mainPort[iPort] == NULL || gDxObj.mainPortTsk[iPort] == NULL )
    {
        return OSA_EFAIL;
    }

    status = port_tsk_recv( gDxObj.mainPortTsk[iPort], data, 64, OSA_TIMEOUT_NONE );
    if( status <= 0 )
    {
        return OSA_EFAIL;
    }

    if( MP_flag( data ) == 0x55 )
    {
        if( MP_checkOk( data ) )
        {
            Uint8	blkId, feildId;
            Int32	value;
            Int32	configId;
            if( MP_blkId( data ) != 0xFF )
            {
                blkId	   = MP_blkId( data );
                feildId	   = MP_feildId( data );
                configId   = CFGID_BUILD( blkId, feildId );

                if( configId < 256 * 16 )
                {
                    value						   = MP_value( data );
                    gDXD_info.sysConfig[configId]  = value;

                    if( gDXD_info.fxnsCfg[configId] != NULL )
                    {
                        status = gDXD_info.fxnsCfg[configId]( blkId, feildId );
                    }
                }
            }
            else
            {
                blkId	   = MP_Q_blkId( data );
                feildId	   = MP_Q_feildId( data );
                configId   = CFGID_BUILD( blkId, feildId );
                value      = -1;
            }

            if( configId < 256 * 16 )
            {
                value			   = gDXD_info.sysConfig[configId];
                MP_flag( data )    = 0x55;
                MP_blkId( data )   = blkId;
                MP_feildId( data ) = feildId;
                MP_value( data )   = value;
                MP_check( data );
                status = port_tsk_send( gDxObj.mainPortTsk[iPort], data, 8, OSA_TIMEOUT_FOREVER );
            }
            else
            {
                OSA_printf( "%s %d: configId 0x%x blkId 0x%x feildId 0x%x error!\r\n",
                            __func__, OSA_getCurTimeInMsec( ), configId,blkId,feildId );
                status = OSA_EFAIL;
            }
        }
        else
        {
            OSA_printf( "%s %d: check error!\r\n", __func__, OSA_getCurTimeInMsec( ) );
        }
    }

    if ( MP_flag( data ) == 0x56 )
    {
        if( MP_checkOk( data ) )
        {
            Uint8	blkId, feildId;
            Int32	value;
            Int32	configId;

            if( MP_blkId( data ) != 0xFF )
            {
                blkId      = MP_blkId( data );
                feildId    = MP_feildId( data );
                configId   = CFGID_BUILD( blkId, feildId );
            }
            else
            {
                blkId	   = MP_Q_blkId( data );
                feildId	   = MP_Q_feildId( data );
                configId   = CFGID_BUILD( blkId, feildId );
                value      = -1;
            }

            if ( configId < 256 * 16 )
            {
                value			   = gDXD_info.sysConfig[configId];
                MP_flag( data )    = 0x56;
                MP_blkId( data )   = blkId;
                MP_feildId( data ) = feildId;
                MP_value( data )   = value;
                MP_check( data );
                status = port_tsk_send( gDxObj.mainPortTsk[iPort], data, 8, OSA_TIMEOUT_FOREVER );

            }
            else
            {
                OSA_printf( "%s %d: configId 0x%x blkId 0x%x feildId 0x%x error!\r\n",
                            __func__, OSA_getCurTimeInMsec( ), configId,blkId,feildId );
                status = OSA_EFAIL;
            }
        }
    }

    if(icout % 200000 == 0x00)
    {
        OSA_printf( "%s %d: exit.\r\n", __func__, OSA_getCurTimeInMsec( ) );
    }

    return status;
}

#endif


/**
 * @Fun:		Dx_tskMain
 * @brief:		Create Dx task
 * @param:	struct OSA_TskHndl *pTsk: task handle,include task communication msg handle
 * @param:	OSA_MsgHndl *pMsg: The first message recevived
 * @param:	Uint32 curState: UNUSED
 * @Output	NULL
 * @return:	task status
 * @Others:
 */
int Dx_tskMain( struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
{
    UInt32	cmd;
    Int32 	configId;
    DX_CFG *pCfg = NULL;
    Bool	ackMsg, done;
    Int		status = 0;
    //Dx_Obj *pObj = (Dx_Obj *) pTsk->appData;

    OSA_printf( "%s:Entered", __func__ );

    done   = FALSE;
    ackMsg = FALSE;

    /* check this message must be create message */
    cmd = OSA_msgGetCmd( pMsg );
    if( cmd != DX_MSGID_CREATE )
    {
        /* invalid command recived in IDLE status, be in IDLE state and ACK
         * with error status */
        OSA_tskAckOrFreeMsg( pMsg, OSA_EFAIL );
        OSA_printf( "%s:Dx create failure \r\n", __func__ );
        return OSA_EFAIL;
    }

    dxTimer_create( 10, Dx_OnTimer );

    if( gDXD_info.OnCreate != NULL )
    {
        gDXD_info.OnCreate(0);
    }

    OSA_tskAckOrFreeMsg( pMsg, status );

    while( !done )
    {
        status = OSA_tskWaitMsg( pTsk, &pMsg );
        if( status != OSA_SOK )
        {
            break;
        }

        cmd = OSA_msgGetCmd( pMsg );

        switch( cmd )
        {
        case DX_MSGID_DELETE:
            done   = TRUE;
            ackMsg = TRUE;
            break;
        case DX_MSGID_START:
            if( gDXD_info.OnStart != NULL )
            {
                status = gDXD_info.OnStart( *(int*)( pMsg->pPrm ) );
                if( status == OSA_SOK )
                {
                    gDxObj.status = 1;
                }
            }

            Dx_openMport( );

            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_LOAD_CFG:
            if(gDXD_info.OnLoadDefCfg != NULL)
            {
                status = gDXD_info.OnLoadDefCfg(0);
            }
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;

        case DX_MSGID_STOP:
            Dx_closeMport( );
            if( gDXD_info.OnStop != NULL )
            {
                status		   = gDXD_info.OnStop(0 );
                gDxObj.status  = 0;
            }
            OSA_tskAckOrFreeMsg( pMsg, status );

            break;
        case DX_MSGID_GET_STATUS:
            if( gDXD_info.OnGetStatus != NULL )
            {
                status = gDXD_info.OnGetStatus(0 );
            }
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_CTRL:
            if( gDXD_info.OnControl != NULL )
            {
                status = gDXD_info.OnControl( (long)pMsg->pPrm );
            }
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_GET_CFG:
            pCfg = (DX_CFG *)pMsg->pPrm;
            configId	= pCfg->configId;
            if( configId < 256 * 16 )
            {
                pCfg->value = gDXD_info.sysConfig[configId];
            }
            else
            {
                OSA_printf(" %s :get cfg msg,Invalid configId %d \n",__func__,configId);
            }
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_SET_CFG:
            pCfg = (DX_CFG *)pMsg->pPrm;
            configId	= pCfg->configId;
            if( configId < 256 * 16 )
            {
                gDXD_info.sysConfig[configId]  = pCfg->value;
                if(gDXD_info.fxnsCfg[configId] != NULL)
                {
                    status = gDXD_info.fxnsCfg[configId](CFGID_blkId(configId),CFGID_feildId(configId));
                }
            }
            else
            {
                OSA_printf(" %s :set cfg msg,Invalid configId %d \n",__func__,configId);
            }

            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_TIMER:
            if( gDXD_info.OnTimer != NULL )
            {
                status = gDXD_info.OnTimer( (long)pMsg->pPrm );
            }
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        case DX_MSGID_PORT_DATA:
#ifdef INCLUDE_MAIN_PORT

            status = Dx_portDataRecvSend( (long)OSA_msgGetPrm( pMsg ) );
#endif

            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        default:
            OSA_tskAckOrFreeMsg( pMsg, status );
            break;
        }
    }

    if( gDXD_info.OnDestroy != NULL )
    {
        gDXD_info.OnDestroy(0 );
    }

    dxTimer_destroy( );

    if( ackMsg && pMsg != NULL )
    {
        OSA_tskAckOrFreeMsg( pMsg, status );
    }

    OSA_printf( " %d: Dx_tskMain  exit !\r\n", OSA_getCurTimeInMsec( ) );

    return 0;
}

static volatile Int32	transBlkId	   = 0;
static volatile Int32	transFieldId   = 0;

static char				*gFileTab[] = {
                                 APP_FORMWARE,
                                 SCRIPT_FILE,
                                 SYSCONFIG_FILE0,
                                 SYSCONFIG_FILE1,
                                 SYSCONFIG_FILE2,
                                 DATA_FILE_0,
                                 DATA_FILE_1,
                                 DATA_FILE_2,
                                 DATA_FILE_3,
                                 DATA_FILE_4,
                                 DATA_FILE_5,
                                 DATA_FILE_6,
                                 DATA_FILE_7,
                                 DATA_FILE_8,
                                 DATA_FILE_9,
                                 DATA_FILE_10,
                                 DATA_FILE_11,
                                 DATA_FILE_12,
                                 DATA_FILE_13,
                                 DATA_FILE_14,
                                 DATA_FILE_15
                             };


/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
static  Int32 data_transfer_callback( void *p, Int32 size, Int32 transLen, Int32 flag )
{
    if( flag <= 0 )
    {
        FIELD_ITEM_SYS( CFGID_BUILD(transBlkId, transFieldId) )	   = flag;
        transBlkId	= 0;
        transFieldId	= 0;
    }

    return OSA_SOK;
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
static Int32 Dx_updateCfgDownloadFile( Int32 blkId, Int32 feildId )
{
    Int32	status = OSA_EFAIL;
    Int32	itemId = FIELD_ITEM_SYS( CFGID_BUILD(blkId, feildId) );

    if( transBlkId != 0 || transFieldId != 0 )
    {
        return OSA_EFAIL;
    }
    if( itemId < 0 || itemId > 18 )
    {
        return OSA_EFAIL;
    }

    if( gFileTab[itemId] == NULL )
    {
        return OSA_EFAIL;
    }

    transBlkId	   = blkId;
    transFieldId   = feildId;

    status = data_port_send_file( gDxObj.dataPort, gFileTab[itemId], cDataPortTimeout, data_transfer_callback );

    if( status != OSA_SOK )
    {
        FIELD_ITEM_SYS( CFGID_BUILD(transBlkId, transFieldId) )	   = status;
        transBlkId								   = 0;
        transFieldId							   = 0;
    }

    return status;
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:	// 1.input param 1��description
 * @param:	// 2.input param 2��description��
 * @Output	// 1.output param 1��description
 * @return:	// return value
 * @Others:	// others
 */
static Int32 Dx_updateCfgUploadFile( Int32 blkId, Int32 feildId )
{
    Int32	status = OSA_EFAIL;
    Int32	itemId = FIELD_ITEM_SYS( CFGID_BUILD(blkId, feildId) );

    OSA_printf( "%s %d: enter.\r\n", __func__, OSA_getCurTimeInMsec( ) );

    if( transBlkId != 0 || transFieldId != 0 )
    {
        return OSA_EFAIL;
    }
    if( itemId < 0 || itemId > 18 )
    {
        return OSA_EFAIL;
    }

    if( gFileTab[itemId] == NULL )
    {
        return OSA_EFAIL;
    }

    transBlkId	= blkId;
    transFieldId   	= feildId;

    status = data_port_recv_file( gDxObj.dataPort, gFileTab[itemId], cDataPortTimeout, data_transfer_callback );
//status = data_port_recv_file( gDxObj.dataPort, "/mnt/config/config_0.txt", cDataPortTimeout, data_transfer_callback );
    if( status != OSA_SOK )
    {
        FIELD_ITEM_SYS( CFGID_BUILD(transBlkId, transFieldId) )	   = status;
        transBlkId	= 0;
        transFieldId	= 0;
    }

    return status;
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
static Int32 Dx_updateCfgDownloadData( Int32 blkId, Int32 feildId )
{
    Int32	status = OSA_EFAIL;
    Int32	itemId = FIELD_ITEM_SYS( CFGID_BUILD(blkId, feildId) );
    void		*address;
    Int32	length;

    if( transBlkId != 0 || transFieldId != 0 )
    {
        return OSA_EFAIL;
    }
    if( itemId <= 0 || itemId > 7 )
    {
        return OSA_EFAIL;
    }

    itemId -= 1;

    address	   = (void*)FIELD_ITEM_SYS(CFGID_DATABLOCK_ADDR( itemId ));
    length	   = FIELD_ITEM_SYS(CFGID_DATABLOCK_ADDR( itemId ));

    if( address == NULL || length <= 0 )
    {
        return OSA_EFAIL;
    }

    transBlkId	   = blkId;
    transFieldId   = feildId;

    status = data_port_send_data( gDxObj.dataPort,(Uint8 *) address, length, cDataPortTimeout, data_transfer_callback );

    if( status != OSA_SOK )
    {
        FIELD_ITEM_SYS( CFGID_BUILD(transBlkId, transFieldId) )	   = status;
        transBlkId								   = 0;
        transFieldId							   = 0;
    }

    return status;
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
static Int32 Dx_updateCfgUploadData( Int32 blkId, Int32 feildId )
{
    Int32	status = OSA_EFAIL;
    Int32	itemId = FIELD_ITEM_SYS( CFGID_BUILD(blkId, feildId) );
    void	*address;
    Int32	length;

    if( transBlkId != 0 || transFieldId != 0 )
    {
        return OSA_EFAIL;
    }
    if( itemId <= 0 || itemId > 7 )
    {
        return OSA_EFAIL;
    }

    itemId -= 1;

    address	   = (void*)FIELD_ITEM_SYS(CFGID_DATABLOCK_ADDR( itemId ));
    length	   = FIELD_ITEM_SYS(CFGID_DATABLOCK_ADDR( itemId ));

    if( address == NULL || length <= 0 )
    {
        return OSA_EFAIL;
    }

    transBlkId	   = blkId;
    transFieldId   = feildId;

    status = data_port_recv_data( gDxObj.dataPort, (Uint8 *) address, length, cDataPortTimeout, data_transfer_callback,TRUE);

    if( status != OSA_SOK )
    {
        FIELD_ITEM_SYS( CFGID_BUILD(transBlkId, transFieldId) )	   = status;
        transBlkId								   = 0;
        transFieldId							   = 0;
    }

    return status;
}

static UInt32	onTimerCnt	   = 0;
static Uint32	statLed		   = 0;
static Uint32	abortValue	   = 0;
static Uint32	flashInterval  = 50;


/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
static void Dx_OnTimer( void )
{
    int		i;
    UInt32	curTM = OSA_getCurTimeInMsec( );

    onTimerCnt++;
    if( ( onTimerCnt % flashInterval ) == 0 )
    {
        statLed = !statLed;
        if( abortValue != 0 )
        {
            statLed = 1;
        }
    }
    for( i = 0; i < DX_TIMER_MAX; i++ )
    {
        if( gDxObj.tmReg[i] == 0 )
        {
            continue;
        }

        while( curTM >= gDxObj.tmBegen[i] + ( gDxObj.tmCnt[i] + 1 ) * gDxObj.tmReg[i] )
        {
            OSA_tskSendMsg( &gDxObj.tsk, NULL, DX_MSGID_TIMER, (void*)i, 0 );
            gDxObj.tmCnt[i]++;
        }
    }
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
Int32 Dx_setTimer( UInt32 timerId, UInt32 nMs )
{
    if( timerId >= DX_TIMER_MAX )
    {
        return OSA_EFAIL;
    }

    gDxObj.tmReg[timerId]	   = nMs;
    gDxObj.tmBegen[timerId]	   = OSA_getCurTimeInMsec( );
    gDxObj.tmCnt[timerId]	   = 0;

    return OSA_SOK;
}

/**
 * @Fun:		// fun name
 * @brief:		// fun discription
 * @param:		// 1.input param 1��description
 * @param:		// 2.input param 2��description��
 * @Output		// 1.output param 1��description
 * @return:		// return value
 * @Others:		// others
 */
Int32 Dx_killTimer( UInt32 timerId )
{
    if( timerId >= DX_TIMER_MAX )
    {
        return OSA_EFAIL;
    }

    gDxObj.tmReg[timerId] = 0;

    return OSA_SOK;
}

Int32 kill_Timer(UInt32 timerId)
{
	if(pTimerObj->pTimeArray[timerId].nStat == eTimer_Stat_Run)
	{	
		Dx_killTimer(timerId);
		pTimerObj->pTimeArray[timerId].nStat = eTimer_Stat_Stop;
	}
	return 0;
}

Int32 start_Timer(UInt32 timerId,UInt32 nMs)
{
	Dx_setTimer( timerId, nMs );
	pTimerObj->pTimeArray[timerId].nStat = eTimer_Stat_Run;
	return 0;
}


Int32 reset_Timer( UInt32 timerId, UInt32 nMs )
{
	kill_Timer(timerId);
	start_Timer( timerId, nMs );
    	return 0;
}



void button_to_save()
{
	  dxCfg[0].configId=CFGID_SYS_SAVE_CONFIG_AS;
	  dxCfg[0].value=0;

printf("------------------------------------------------------------------\n\n");
printf("*****************************save param****************************\n\n");

	CFGID_CONFIG_SETSAVE(CFGID_Gun_ZC_distance,   		(int)(gMachineGun_ZCTable.distance*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gun_ZC_deltaX,      		(int)(gMachineGun_ZCTable.data.deltaX*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gun_ZC_deltaY,       		(int)(gMachineGun_ZCTable.data.deltaY*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gun_ZC_Angle,        		(int)(gMachineGun_ZCTable.Angle*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_ZC_distance,     		(int)(gGrenadeKill_ZCTable.distance*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_ZC_deltaX,        		(int)(gGrenadeKill_ZCTable.data.deltaX*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_ZC_deltaY,        		(int)(gGrenadeKill_ZCTable.data.deltaY*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_ZC_Angle,         		(int)(gGrenadeKill_ZCTable.Angle*100));

	CFGID_CONFIG_SETSAVE(CFGID_Weather_Temparature,   	gWeatherTable.Temparature);
	CFGID_CONFIG_SETSAVE(CFGID_Weather_Pressure, 	     	gWeatherTable.Pressure);
	CFGID_CONFIG_SETSAVE(CFGID_Gun_GC_distance,     		(int)(gMachineGun_GCParam.distance*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gun_GC_deltaX,        		(int)(gMachineGun_GCParam.data.deltaX*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gun_GC_deltaY,       	 	(int)(gMachineGun_GCParam.data.deltaY*100));

printf("gMachineGun_GCParam.data.deltaX =%d\n",gMachineGun_GCParam.data.deltaX);
printf("gMachineGun_GCParam.data.deltaY =%d\n",gMachineGun_GCParam.data.deltaY);


	CFGID_CONFIG_SETSAVE(CFGID_Gas_GC_distance,     		(int)(gGrenadeGas_GCParam.distance*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gas_GC_deltaX,        		(int)(gGrenadeGas_GCParam.data.deltaX*100));
	CFGID_CONFIG_SETSAVE(CFGID_Gas_GC_deltaY,        		(int)(gGrenadeGas_GCParam.data.deltaY*100));

	CFGID_CONFIG_SETSAVE(CFGID_Kill_GC_distance,     		(int)(gGrenadeKill_GCParam.distance*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_GC_deltaX,        		(int)(gGrenadeKill_GCParam.data.deltaX*100));
	CFGID_CONFIG_SETSAVE(CFGID_Kill_GC_deltaY,        		(int)(gGrenadeKill_GCParam.data.deltaY*100)); 

	Dx_sendCfgMsg(NULL,DX_MSGID_SET_CFG,&dxCfg[0],FALSE);

printf("-----------------------------END----------------------------------\n\n");
	 return ;
}


void button_to_read()
{


	 if( FIELD_ITEM_SYS(CFGID_SYS_SEL_STARTUP) == 1 )
	 {
        	syscfg_load( SYSCONFIG_FILE1, gDXD_info.sysConfig, 256 * 16 * 4 );
	 }
	 
       gMachineGun_ZCTable.distance 	= 	gDXD_info.sysConfig[CFGID_Gun_ZC_distance]*0.01;
	gMachineGun_ZCTable.Angle 		= 	gDXD_info.sysConfig[CFGID_Gun_ZC_Angle]*0.01;
	gMachineGun_ZCTable.data.deltaX	= 	gDXD_info.sysConfig[CFGID_Gun_ZC_deltaX]*0.01;
	gMachineGun_ZCTable.data.deltaY 	= 	gDXD_info.sysConfig[CFGID_Gun_ZC_deltaY]*0.01;

	gGrenadeKill_ZCTable.distance 	=	gDXD_info.sysConfig[CFGID_Kill_ZC_distance]*0.01;
	gGrenadeKill_ZCTable.Angle	 	=	gDXD_info.sysConfig[CFGID_Kill_ZC_Angle]*0.01;
	gGrenadeKill_ZCTable.data.deltaX	=	gDXD_info.sysConfig[CFGID_Kill_ZC_deltaX]*0.01;
	gGrenadeKill_ZCTable.data.deltaY	=	gDXD_info.sysConfig[CFGID_Kill_ZC_deltaY]*0.01;

	gWeatherTable.Temparature		=	gDXD_info.sysConfig[CFGID_Weather_Temparature];
	gWeatherTable.Pressure			=	gDXD_info.sysConfig[CFGID_Weather_Pressure];

	gMachineGun_GCParam.distance	=	gDXD_info.sysConfig[CFGID_Gun_GC_distance]*0.01;
	gMachineGun_GCParam.data.deltaX	=	gDXD_info.sysConfig[CFGID_Gun_GC_deltaX]*0.01;
	gMachineGun_GCParam.data.deltaY	=	gDXD_info.sysConfig[CFGID_Gun_GC_deltaY]*0.01;
	
	gGrenadeGas_GCParam.distance	=	gDXD_info.sysConfig[CFGID_Gas_GC_distance]*0.01;
	gGrenadeGas_GCParam.data.deltaX	=	gDXD_info.sysConfig[CFGID_Gas_GC_deltaX]*0.01;
	gGrenadeGas_GCParam.data.deltaY	=	gDXD_info.sysConfig[CFGID_Gas_GC_deltaY]*0.01;

	gGrenadeKill_GCParam.distance	=	gDXD_info.sysConfig[CFGID_Kill_GC_distance]*0.01;
	gGrenadeKill_GCParam.data.deltaX	=	gDXD_info.sysConfig[CFGID_Kill_GC_deltaX]*0.01;
	gGrenadeKill_GCParam.data.deltaY	=	gDXD_info.sysConfig[CFGID_Kill_GC_deltaY]*0.01;

	//printf("gMachineGun_ZCTable.distance = %f\n\n",gMachineGun_ZCTable.distance);
	return ;
}

/************************************** The End Of File **************************************/

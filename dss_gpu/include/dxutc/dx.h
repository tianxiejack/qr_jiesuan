/*=======================================================
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:			// filename
* Author:			//author
* Date:			// date
* Description:			// description
* Version:			// version
* Function List:		// funs and description
*     1. -------
* History:			// histroy modfiy record
*     <author>  <time>   <version >   <desc>
*     xavier    14/01/4     1.0     build this moudle
   =======================================================*/
#ifndef __DX_H_
#define __DX_H_

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <osa_mutex.h>

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
#include <fcntl.h>

#include "dx_config.h"



//#define SYSCFG_ITEM( cfgId )	    ( gDXD_info.sysConfig[cfgId] )


#define FIELD_ITEM( cfg, cfgId ) 	( cfg[cfgId] )
#define FIELD_ITEM_SYS( cfgId )		( gDXD_info.sysConfig[cfgId] )

//sysconfig
#define CFGID_SYS_CFG_TAG				CFGID_BUILD( 0, 0 )
#define CFGID_SYS_CFG_DEVID			CFGID_BUILD( 0, 1 )
#define CFGID_SYS_CFG_SW_VER			CFGID_BUILD( 0, 2 )
#define CFGID_SYS_CFG_HW_VER			CFGID_BUILD( 0, 3 )
#define CFGID_SYS_CFG_CFG_VER			CFGID_BUILD( 0, 4 )
//realtime control;

//data transfer
#define CFGID_SHOTCUT					CFGID_BUILD( 4, 1 )
#define CFGID_DOWNLOAD_FILE			CFGID_BUILD( 4, 2 )
#define CFGID_UPLOAD_FILE				CFGID_BUILD( 4, 3 )
#define CFGID_DOWNLOAD_DATA			CFGID_BUILD( 4, 4 )
#define CFGID_UPLOAD_DATA				CFGID_BUILD( 4, 5 )
#define CFGID_DATATRANS_REMOTE_IP	CFGID_BUILD( 4, 6 )
#define CFGID_DATATRANS_REMOTE_PORT  CFGID_BUILD( 4, 7 )

//system contorl
#define CFGID_SYS_DEBUG				CFGID_BUILD( 9, 4 )
#define CFGID_SYS_SEL_STARTUP			CFGID_BUILD( 9, 5 )
#define CFGID_SYS_SAVE_CONFIG_AS		CFGID_BUILD( 9, 6 )
#define CFGID_SYS_SAVE_STARTUP_AS	CFGID_BUILD( 9, 7 )
#define CFGID_SYS_LOAD_FROM			CFGID_BUILD( 9, 8 )
#define CFGID_SYS_REBOOT				CFGID_BUILD( 9, 9 )
#define CFGID_SYS_AUTO_CHECK			CFGID_BUILD( 9, 10)
#define CFGID_SYS_WTD_ENABLE      	  	CFGID_BUILD( 9, 11)
#define CFGID_SYS_WTD_TIMEOUT			CFGID_BUILD( 9, 12)
#define CFGID_SYS_WTD_TIME_SLEEP 		CFGID_BUILD( 9, 13)
 
//network eth configure
#define CFGID_NET_ETH0_ENABLE			CFGID_BUILD( 10, 1 )
#define CFGID_NET_ETH0_DHCP			CFGID_BUILD( 10, 2 )
#define CFGID_NET_ETH0_IP				CFGID_BUILD( 10, 3 )
#define CFGID_NET_ETH0_MASK			CFGID_BUILD( 10, 4 )
#define CFGID_NET_ETH0_GATEWAY		CFGID_BUILD( 10, 5 )

#define CFGID_NET_ETH1_ENABLE			CFGID_BUILD( 10, 6 )
#define CFGID_NET_ETH1_DHCP			CFGID_BUILD( 10, 7 )
#define CFGID_NET_ETH1_IP				CFGID_BUILD( 10, 8 )
#define CFGID_NET_ETH1_MASK			CFGID_BUILD( 10, 9 )
#define CFGID_NET_ETH1_GATEWAY		CFGID_BUILD( 10, 10 )

//Symbology Overlay
#define CFGID_SYMBOLOGY_ENABLE    		CFGID_BUILD( 11, 1 )

#define CFGID_CHN_INFO1_WIDTH( chId )	CFGID_BUILD( 129, ( 1 + chId * 2 ) )
#define CFGID_CHN_INFO1_HEIGHT( chId )	CFGID_BUILD( 129, ( 2 + chId * 2 ) )
#define CFGID_CHN_INFO2_WIDTH( chId )	CFGID_BUILD( 130, ( 1 + chId * 2 ) )
#define CFGID_CHN_INFO2_HEIGHT( chId )	CFGID_BUILD( 130, ( 2 + chId * 2 ) )
#define CFGID_CHN_INFO3_WIDTH( chId )	CFGID_BUILD( 131, ( 1 + chId * 2 ) )
#define CFGID_CHN_INFO3_HEIGHT( chId )	CFGID_BUILD( 131, ( 2 + chId * 2 ) )
#define CFGID_CHN_INFO4_WIDTH( chId )	CFGID_BUILD( 132, ( 1 + chId * 2 ) )
#define CFGID_CHN_INFO4_HEIGHT( chId )	CFGID_BUILD( 132, ( 2 + chId * 2 ) )
#define CFGID_CHN_INFO5_WIDTH( chId )	CFGID_BUILD( 133, ( 1 + chId * 2 ) )
#define CFGID_CHN_INFO5_HEIGHT( chId )	CFGID_BUILD( 133, ( 2 + chId * 2 ) )

#define CFGID_CHN_INFO6_COUNT( chId )	CFGID_BUILD( 134, ( 1 + chId ) )
#define CFGID_CHN_INFO7_COUNT( chId )	CFGID_BUILD( 135, ( 1 + chId ) )
#define CFGID_CHN_INFO8_COUNT( chId )	CFGID_BUILD( 136, ( 1 + chId ) )

#define CFGID_DATABLOCK_ADDR( blkId )	CFGID_BUILD( 137, ( 1 + blkId * 2 ) )
#define CFGID_DATABLOCK_LENGTH( blkId ) CFGID_BUILD( 137, ( 2 + blkId * 2 ) )

#define FXN_BIGEN if( gDXD_info.fxnsCfg != NULL ){
#define FXN_REG( blkId, feildId, fxn ) ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, feildId )] = fxn )
#define FXN_REG2( cfgId, fxn ) ( gDXD_info.fxnsCfg[ cfgId ] = fxn )
#define FXN_REG_ALL( blkId, fxn )  \
    ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 0 )] = fxn );     \
      ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 1 )] = fxn );   \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 2 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 3 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 4 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 5 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 6 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 7 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 8 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 9 )] = fxn );  \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 10 )] = fxn ); \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 11 )] = fxn ); \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 12 )] = fxn ); \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 13 )] = fxn ); \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 14 )] = fxn ); \
       ( gDXD_info.fxnsCfg[CFGID_BUILD( blkId, 15 )] = fxn )
#define FXN_END }



typedef Int32 ( *ConfigFxn )( Int32 blkId, Int32 feildId );

typedef struct {
	Int32		*scripts;
	Int32		*sysConfig;
	ConfigFxn 	*fxnsCfg;
	ConfigFxn 	reloadCfg;
	//Int32		*wrkMode;

	Int32 ( *OnLoadDefCfg )( int item );
	Int32 ( *OnCreate )( int item );
	Int32 ( *OnDestroy )( int item );
	Int32 ( *OnStart )( int nType );
	Int32 ( *OnStop )( int item );
	Int32 ( *OnGetStatus )( int item );
	Int32 ( *OnControl )( Int32 cmd );
	Int32 ( *OnTimer )( Int32 timerId );

} DXD_Info;

enum {
	DX_MSGID_CREATE = 0,
	DX_MSGID_DELETE,
	DX_MSGID_START,
	DX_MSGID_LOAD_CFG,
	DX_MSGID_STOP,
	DX_MSGID_GET_STATUS,
	DX_MSGID_CTRL,
	DX_MSGID_GET_CFG,
	DX_MSGID_SET_CFG,
	DX_MSGID_PORT_DATA,
	DX_MSGID_TIMER,
	DX_MSGID_UPDATE_FIELD,
	DX_MSGID_GET_IPC_CFG,
	DX_MSGID_SET_IPC_CFG,
	DX_MSGID_MAX
};

enum {
	DX_CTL_PRINT_INFO     	= 0,
	DX_CTL_PRINT_CORE		= 1,
	DX_CTL_PRINT_BUFFER	    = 2,
	DX_CTL_ALG_LINK_INIT    = 3,
};

enum {
    DX_INTERGER    = 0x00,
    DX_FIXED32     = 0x01,
    DX_IPADDRS     = 0x02,
    DX_BYTES       = 0x03,
    DX_WORDS       = 0x04,
    DX_TIMES       = 0x05,
};

typedef struct DX_CFG {
	Int32	configId;
	Int32	value;
}DX_CFG;

extern DX_CFG dxCfg[8];


extern DXD_Info gDXD_info;
extern DXD_Info gDXD_infoSave;

#define CFGID_FIELD_SET(property,configId)  \
    gDXD_info.sysConfig[configId]= property

#define CFGID_CONFIG_SET(configId,property)  \
    gDXD_info.sysConfig[configId]= property

#define CFGID_CONFIG_SETSAVE(configId,property)  \
      gDXD_infoSave.sysConfig[configId]= property

#define CFGID_FIELD_GET(property,configId)  \
    property = gDXD_info.sysConfig[configId]



#define OSD_PROPERTY_GET(property,blkId,feildId)    \
    property = gDXD_info.sysConfig[CFGID_BUILD( blkId, feildId )]


#define CFG_FIELD_FILL(property,blkId,feildId)       \
     gDXD_info.sysConfig[CFGID_BUILD( blkId, feildId )]= property

Int32 Dx_run( );
void initdxobj();
Int32 Dx_stop( );

Int32 Dx_sendMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, Void *pPrm, Int32 prmSize, Bool bAck );

Int32 Dx_postMsg( Int32 msgId, Void *pPrm, Int32 prmSize );

Int32 Dx_sendCfgMsg( OSA_TskHndl *pPrcFrom, Int16 msgId, DX_CFG *pCfg, Bool bAck );

char Dx_getChar( void );

void Dx_configMenu( Int32 *script );

Int32 Dx_setTimer( UInt32 timerId, UInt32 nMs );

Int32 Dx_killTimer( UInt32 timerId );

Int32 Dx_update_field( Int32 blkId, Int32 fieldno, Bool notify );


Int32 Dx_initparam( );

#endif /* CR_DX_DRIVER_DXD_H */
/************************************** The End Of File **************************************/

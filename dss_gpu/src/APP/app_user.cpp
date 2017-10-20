/*****************************************************************************
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:     app_user.c
* Author:       xavier
* Date:         2014-01-14
* Description:      user app interface
* Version:          v1.0.0
* Function List:
*               char APP_getChar( );
*               Int32 APP_eventHandler(UInt32 eventId, Ptr pPrm, Ptr appData);
*               static Int32 APP_loadDefCfg( );
*               static Int32 APP_create( );
*               static Int32 APP_destroy( );
*               static Int32 APP_start( );
*               static Int32 APP_stop( );
*               static Int32 APP_getStatus( );
*               static Int32 APP_control( );
*               static Int32 APP_getConfig( );
*               static Int32 APP_setConfig( );
*               static Int32 APP_stop( );
*               static Int32 APP_onTimer( );
*
* History:          // histroy modfiy record
*     <author>  <time>   <version >   <desc>
*     xavier    14/01/14     1.0.0     build this moudle
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include "osa.h"
#include "dx.h"
#include "dx_config.h"
#include "app_user.h"
#include "app_global.h"
#include "osd_graph.h"
#include "app_hardctrl.h"
#include "statCtrl.h"
#include "osdProcess.h"
#include "GrenadePort.h"
#include "WeaponCtrl.h"
#include "TurretPosPort.h"


static Int32 APP_loadDefCfg( int item);
static Int32 APP_create( int item);
static Int32 APP_destroy(int item );
static Int32 APP_start( int item);
static Int32 APP_stop( int item);
static Int32 APP_getStatus(int item );
static Int32 APP_control(int item );
static Int32 APP_stop( int item);
static Int32 APP_onTimer( int item);

DXD_Info gDXD_info ;
DXD_Info gDXD_infoSave;
#if 0
=
    {
        .OnLoadDefCfg  = APP_loadDefCfg,
        .OnCreate      = APP_create,
        .OnDestroy     = APP_destroy,
        .OnStart       = APP_start,
        .OnStop        = APP_stop,
        .OnGetStatus   = APP_getStatus,
        .OnControl     = APP_control,
        .OnTimer       = APP_onTimer,
    };
#endif
static Bool  gAPP_Started   = FALSE;
static Int32 gAPP_Usecase   = 0x00;
extern int msgEraseId;
void initgdxd_info()
{
	 gDXD_info.OnLoadDefCfg  = APP_loadDefCfg;
        gDXD_info.OnCreate      = APP_create;
        gDXD_info.OnDestroy     = APP_destroy;
        gDXD_info.OnStart       = APP_start;
        gDXD_info.OnStop        = APP_stop;
        gDXD_info .OnGetStatus   = APP_getStatus;
        gDXD_info.OnControl     = APP_control;
        gDXD_info .OnTimer       = APP_onTimer;

}



/********************************************************************************
 * @brief:      APP_getChar: receive char from terminal
 * @param:
 * @param:
 * @return:
 * @Others:
 *******************************************************************************/
char APP_getChar( )
{
    char buffer[100];

    fflush( stdin );
    fgets( buffer, MAX_INPUT_STR_SIZE, stdin );

    return ( buffer[0] );
}
unsigned char APP_getint( )
{
	char buffer[MAX_INPUT_STR_SIZE];

	fflush( stdin );
	fgets( buffer, MAX_INPUT_STR_SIZE, stdin );
	//atoi(buffer);
	printf("input value=%d\n",atoi(buffer));
	return atoi(buffer);
}
void APP_bootMsDelay()
{
    OSA_printf("Boot delay %d s ...\n", gDXD_info.sysConfig[CFGID_RT_CTRL_BOOT_DELAY]);
    OSA_waitMsecs(gDXD_info.sysConfig[CFGID_RT_CTRL_BOOT_DELAY] * 1000);
}

/********************************************************************************
 * @brief:      APP_eventHandler: mcfw event process fun
 * @param:  UInt32 eventId: mcfw event id
 * @param:      Ptr pPrm: event param  ptr
 * @param:  Ptr appData: event process data
 * @return: 0
 * @Others:
 *******************************************************************************/
/********************************************************************************
 * @brief:      APP_startStop: start or stop user mcfw software user case
 * @param:  int usercase : usercase Id, identify which usercase to start
 * @param:   Bool IsActiveCase : identify start or stop current usercase
 * @return: 0 or -1
 * @Others:
 *******************************************************************************/
int APP_startStop( int usercase, Bool IsActiveCase)
{
    switch ( usercase )
    {
    case MCFW_VCAP_VDIS_CASE:
      

        break;
    case MCFW_VCAP_VENC_CASE:
      
        break;
    case MCFW_IMAGE_PROCESS_BOARD_CASE:
      

        break;
    default:
        return -1;
    }

 
    printf(" [DEBUG ]%s %d \r\n",__func__,__LINE__);

    return 0;
}

/********************************************************************************
 * @Fun:        // fun name
 * @brief:      // fun discription
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_loadDefCfg( int item )
{
    int i = 0;
    //codec param block 32
    for (i = 0; i < 8; i++)
    {
        FIELD_ITEM_SYS(CFGID_CODEC_Stat(i))             = 0x00;
        FIELD_ITEM_SYS(CFGID_CODEC_IFrameQpMax(i))      = 51;
        FIELD_ITEM_SYS(CFGID_CODEC_IFrameQpMin(i))      = 10;
        FIELD_ITEM_SYS(CFGID_CODEC_IFrameQp(i))         = 30;
        FIELD_ITEM_SYS(CFGID_CODEC_PFrameQpMax(i))      = 51;
        FIELD_ITEM_SYS(CFGID_CODEC_PFrameQpMin(i))      = 10;
        FIELD_ITEM_SYS(CFGID_CODEC_PFrameQp(i))         = 30;
        FIELD_ITEM_SYS(CFGID_CODEC_GopLen(i))           = 60;
        FIELD_ITEM_SYS(CFGID_CODEC_FrameRate(i))        = 30;
        FIELD_ITEM_SYS(CFGID_CODEC_RateCtrlType(i))     = 0;
        FIELD_ITEM_SYS(CFGID_CODEC_BitRate(i))          = 3000;
        FIELD_ITEM_SYS(CFGID_CODEC_IDRFrame(i))         = 0;
        FIELD_ITEM_SYS(CFGID_CODEC_EncInfo(i))          = 0;
    }

   

    return OSA_SOK;
}

/********************************************************************************
 * @Fun:        APP_create
 * @brief:      // fun discription
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_create( int item )
{
    int i = 0;

    FXN_BIGEN

    //BLK 0: SYS INFORMATION
    FXN_REG2(CFGID_SYS_CFG_DEVID,                   NULL);
    FXN_REG2(CFGID_SYS_CFG_HW_VER,                  NULL);
    FXN_REG2(CFGID_SYS_CFG_SW_VER,                  NULL);

    //BLK 1: DEVICE PARAMS SET
    FXN_REG2(CFGID_DEV_CFG_IP,                      NULL);
    FXN_REG2(CFGID_DEV_CFG_IP_MASK,                 NULL);
    FXN_REG2(CFGID_DEV_CFG_GATEWAY,                 NULL);
    FXN_REG2(CFGID_DEV_CFG_MSG_UDP_PORT,            NULL);
    FXN_REG2(CFGID_DEV_CFG_PING_TIMEOUT,            NULL);

    //BLK2
    FXN_REG2(CFGID_RT_CTRL_WORK_MOD,                NULL);
    FXN_REG2(CFGID_RT_CTRL_BOOT_DELAY,              NULL);
    FXN_REG2(CFGID_RT_CTRL_BOARD_DATE,              NULL);
    FXN_REG2(CFGID_RT_CTRL_BOARD_TIME,              NULL);
    FXN_REG2(CFGID_RT_CTRL_AUTO_CHECK,              NULL);
    FXN_REG2(CFGID_RT_CTRL_MAIN_SENSOR,             NULL);
    FXN_REG2(CFGID_RT_CTRL_INIT_OSD_ID,             NULL);

    //BLK 10: NETWORK ETH
    FXN_REG2(CFGID_NET_ETH0_ENABLE,                 NULL);
    FXN_REG2(CFGID_NET_ETH0_DHCP,                   NULL);
    FXN_REG2(CFGID_NET_ETH0_IP,                     NULL);
    FXN_REG2(CFGID_NET_ETH0_MASK,                   NULL);
    FXN_REG2(CFGID_NET_ETH0_GATEWAY,                NULL);

    //blk15 video display dev
    FXN_REG2(CFGID_VDIS_HDMI_ENABLE,                NULL);
    FXN_REG2(CFGID_VDIS_HDMI_RESOLUTION,            NULL);
    FXN_REG2(CFGID_VDIS_HDCOMP_ENABLE,              NULL);
    FXN_REG2(CFGID_VDIS_HDCOMP_RESOLUTION,          NULL);
    FXN_REG2(CFGID_VDIS_DVO2_ENABLE,                NULL);
    FXN_REG2(CFGID_VDIS_DVO2_RESOLUTION,            NULL);
    FXN_REG2(CFGID_VDIS_SD_ENABLE,                  NULL);
    FXN_REG2(CFGID_VDIS_SD_RESOLUTION,              NULL);
    FXN_REG2(CFGID_VDIS_VENC_TIED,                  NULL);


    //blk16 sensor tv cfg
    FXN_REG2(CFGID_SENSOR_TV_LAYOUT_PRM_UPDATE,     NULL);

    //blk17 sensor flr cfg
    FXN_REG2(CFGID_SENSOR_FR_LAYOUT_PRM_UPDATE,     NULL);

    //blk18 video crop used DEI0 cfg
    FXN_REG2(CFGID_VID_CROP_DEI0_PRM_UPDATE,        NULL);

    //blk19 video crop use DEI1 cfg
    FXN_REG2(CFGID_VID_CROP_DEI1_PRM_UPDATE,        NULL);

    //blk20 video image enth cfg
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_EN,            NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_EN,            NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_PRM_UPDATE,       NULL);

    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_X,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_Y,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_W,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_H,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_X,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_Y,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_W,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_H,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_MAXW,     NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_TV_RECT_MAXH,     NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_MAXW,     NULL);
    FXN_REG2(CFGID_VIDEO_IMG_ENTH_FR_RECT_MAXH,     NULL);

    //blk21 video image stb cfg

    //blk22 video image rst cfg
    FXN_REG2(CFGID_VIDEO_IMG_RST_TV_EN,             NULL);
    FXN_REG2(CFGID_VIDEO_IMG_RST_FR_EN,             NULL);
    FXN_REG2(CFGID_VIDEO_IMG_RST_PRM_UPDATE,        NULL);
    FXN_REG2(CFGID_VIDEO_IMG_RST_THETA,             NULL);
  

    //blk22~23 video image mmt cfg
    FXN_REG2(CFGID_TV_IMG_MMT_EN,                   NULL);
    FXN_REG2(CFGID_FR_IMG_MMT_EN,                   NULL);
    FXN_REG2(CFGID_IMG_MMT_PRM_UPDATE,              NULL);

    //blk30 video image tv track cfg
    FXN_REG2(CFGID_TRACK_TV_EN,                     NULL);
    FXN_REG2(CFGID_TRACK_TV_PRM_UPDATE,             NULL);
    FXN_REG2(CFGID_TRACK_TV_ACQ_X,                  NULL);
    FXN_REG2(CFGID_TRACK_TV_ACQ_Y,                  NULL);
    FXN_REG2(CFGID_TRACK_TV_ACQ_W,                  NULL);
    FXN_REG2(CFGID_TRACK_TV_ACQ_H,                  NULL);
    FXN_REG2(CFGID_TRACK_TV_ACQ_ENDRAW,             NULL);

    //blk31 video image fr track cfg
    FXN_REG2(CFGID_TRACK_FR_EN,                     NULL);
    FXN_REG2(CFGID_TRACK_FR_PRM_UPDATE,             NULL);
    FXN_REG2(CFGID_TRACK_FR_ACQ_X,                  NULL);
    FXN_REG2(CFGID_TRACK_FR_ACQ_Y,                  NULL);
    FXN_REG2(CFGID_TRACK_FR_ACQ_W,                  NULL);
    FXN_REG2(CFGID_TRACK_FR_ACQ_H,                  NULL);
    FXN_REG2(CFGID_TRACK_FR_ACQ_ENDRAW,             NULL);



    //blk60 osd tv cross axis cfg
    for (i = 0; i < 1; i++)
    {
        FXN_REG2(CFGID_OSD_GRAPH_OBJ_ID(i),         NULL);
        FXN_REG2(CFGID_OSD_GRAPH_UPDATE(i),         NULL);
        FXN_REG2(CFGID_OSD_GRAPH_OBJ_TYPE(i),       NULL);
        FXN_REG2(CFGID_OSD_GRAPH_CENTER_X(i),       NULL);
        FXN_REG2(CFGID_OSD_GRAPH_CENTER_Y(i),       NULL);
        FXN_REG2(CFGID_OSD_GRAPH_WIDTH(i),          NULL);
        FXN_REG2(CFGID_OSD_GRAPH_HEIGHT(i),         NULL);
        FXN_REG2(CFGID_OSD_GRAPH_OBJ_MARK(i),       NULL);
    }

    //blk 94 osd tv system time


    //blk 121 realtime status cfg tv
    FXN_REG2(CFGID_RTS_TV_SEN_COLOR,                NULL);
    FXN_REG2(CFGID_RTS_FR_SEN_COLOR,                NULL);

    //blk 130 tv track state
    FXN_REG2(CFGID_TRACK_TV_STATE,                 NULL/*APP_get_track_state*/);
    FXN_REG2(CFGID_TRACK_TV_TBE_X,                 NULL/*APP_get_track_state*/);
    FXN_REG2(CFGID_TRACK_TV_TBE_Y,                 NULL/*APP_get_track_state*/);

    //blk 131 fr track state
    FXN_REG2(CFGID_TRACK_FR_STATE,                 NULL/*APP_get_track_state*/);
    FXN_REG2(CFGID_TRACK_FR_TBE_X,                 NULL/*APP_get_track_state*/);
    FXN_REG2(CFGID_TRACK_FR_TBE_Y,                 NULL/*APP_get_track_state*/);
	
    //BLK 140: RTSP
    for (i = 0; i < 8; i++)
    {
        FXN_REG2( CFGID_CODEC_Stat(i),              NULL);
        FXN_REG2( CFGID_CODEC_IFrameQpMax(i),       NULL);
        FXN_REG2( CFGID_CODEC_IFrameQpMin(i),       NULL);
        FXN_REG2( CFGID_CODEC_IFrameQp(i),          NULL);
        FXN_REG2( CFGID_CODEC_PFrameQpMax(i),       NULL);
        FXN_REG2( CFGID_CODEC_PFrameQpMin(i),       NULL);
        FXN_REG2( CFGID_CODEC_PFrameQp(i),          NULL);
        FXN_REG2( CFGID_CODEC_GopLen(i),            NULL);
        FXN_REG2( CFGID_CODEC_FrameRate(i),         NULL);
        FXN_REG2( CFGID_CODEC_RateCtrlType(i),      NULL);
        FXN_REG2( CFGID_CODEC_BitRate(i),           NULL);
        FXN_REG2( CFGID_CODEC_IDRFrame(i),          NULL);
        FXN_REG2( CFGID_CODEC_EncInfo(i),           NULL);
    }

    FXN_END
    OSA_printf("%s:%d", __func__, __LINE__);

    return OSA_SOK;
}

/********************************************************************************
 * @Fun:        APP_destroy
 * @brief:      APP free
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_destroy( int item )
{
    Int32 status = OSA_SOK;
    return status;
}

/********************************************************************************
 * @Fun:    // APP_setPrms(  )
 * @brief:  // fun discription
 * @param:  // 1.input param description
 * @param:  // 2.input param description
 * @Output  // 1.output param description
 * @return: // return value
 * @Others: // others
 *******************************************************************************/
void APP_setPrms( int nType )
{

   	

}
/********************************************************************************
 * @Fun:    // fun name
 * @brief:  // fun discription
 * @param:  // 1.input param description
 * @param:  // 2.input param description
 * @Output  // 1.output param description
 * @return: // return value
 * @Others: // others
 *******************************************************************************/
static Int32 APP_start( int nType )
{
    Int32 status = 0;

    OSA_printf("%s:entry case type %d\n", __func__, nType+1);

    APP_setPrms(nType);

    CTIMERCTRL_initTimerCtrl();

    //start usercase
    if ( gAPP_Started )
    {
        gAPP_Usecase = 0x00;
      //  APP_startStop( nType, FALSE);
    }

    gAPP_Usecase = nType;

   // status = APP_startStop( nType, TRUE);

    gAPP_Started = TRUE;



	//	Dx_setTimer(OSD_SHOW_TIMER, OSD_SHOW_TIMER_TICKS);
	Dx_setTimer(GRPX_SHOW_TIMER,GRPX_SHOW_TIME_TICKS);
	

	
	//Dx_setTimer(SYS_INFOR_SHOW_TIMER,SYS_INFOR_SHOW_TIMER_TICKS);
	//Dx_setTimer(ALG_MTD_TIMER,  ALG_MTD_TIMER_TICKS);
	Dx_setTimer(ALG_TRACK_TIMER,ALG_TRACK_TIMER_TICKS);
	Dx_setTimer(GPIO_INSPECT_TIMER,GPIO_INSPECT_TICKS);
	Dx_sendMsg( NULL, DX_MSGID_CTRL, (void*)( DX_CTL_ALG_LINK_INIT ), sizeof( Int32 ), FALSE );

	
	startSelfCheckTimer();
	startCANSendTimer();
	

	OSA_printf("%s:exit", __func__);

    return OSA_SOK;
}

/********************************************************************************
 * @Fun:        APP_stop
 * @brief:      // fun discription
 * @param:      // 1.input param description
 * @param:      // 2.input param description
 * @Output      // 1.output param description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_stop( int item )
{
    Int32 status = 0;

    Dx_killTimer(SYS_INFOR_SHOW_TIMER);
    Dx_killTimer(OSD_SHOW_TIMER);
    //Dx_killTimer(ALG_MTD_TIMER);
    //Dx_killTimer(ALG_TRACK_TIMER);
    Dx_killTimer(GRPX_SHOW_TIMER);


    if ( gAPP_Started )
    {
        status = APP_startStop( gAPP_Usecase, FALSE);
    }
	
    gAPP_Usecase   = 0x00;
    gAPP_Started   = FALSE;

    return OSA_SOK;
}

/********************************************************************************
 * @Fun:        // fun name
 * @brief:      // fun discription
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_getStatus( int item )
{
    return OSA_SOK;
}

/********************************************************************************
 * @Fun:        // fun name
 * @brief:      // fun discription
 * @param:      // 1.input param 1锟斤拷description
 * @param:      // 2.input param 2锟斤拷description锟斤拷
 * @Output      // 1.output param 1锟斤拷description
 * @return:     // return value
 * @Others:     // others
 *******************************************************************************/
static Int32 APP_control( Int32 cmd )
{
  
    return OSA_SOK;
}

/********************************************************************************
 * @Fun:    APP_onTimer
 * @brief:  user timer,timer0 print system information
 * @param:  Int32 timerId : timer Id
 * @param:
 * @Output  NULL
 * @return: OSA_SOK
 * @Others: NULL
 *******************************************************************************/
static Int32 APP_onTimer( Int32 timerId )
{
	int i;
	    //printf("**********************************\n");
	    if( timerId == SYS_INFOR_SHOW_TIMER)
	    {
	    	//no thing
	    }

	    if( timerId == GPIO_INSPECT_TIMER)
	    {	    	
		//APP_getvideostatus();
	    }

	    if(timerId == GRPX_SHOW_TIMER)
	    {
	    	 //Hard_getccdstatus();
	        APP_graphic_timer_alarm();
	    }

	   if(timerId == ALG_TRACK_TIMER)
	   {
		  APP_tracker_timer_alarm();
	   }

	  if(timerId == eDynamic_Timer)
	  {
		DynamicTimer_cbFxn();
	  }
  
	  if(timerId == eAVT_Timer)
	  {
	  	if(pTimerObj->GetTimerStat(eAVT_Timer) == eTimer_Stat_Run)
	  	{
	  		pTimerObj->KillTimer(eAVT_Timer);
	  	}
		OSDCTRL_NoShineShow();
	  }

	   if(timerId == eRGQ_Timer)
	   {
			pTimerObj->KillTimer(eRGQ_Timer);
			if(Posd[eCorrectionTip] == AngleCorrectOsd[CORRECTION_GQ])
			{
				Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_RGQ];
				pTimerObj->startTimer(eRGQ_Timer,RGQ_TIMER);
			}
			else if(Posd[eCorrectionTip] != AngleCorrectOsd[CORRECTION_GQ])
				OSDCTRL_ItemHide(eCorrectionTip);

			if(OSDCTRL_IsOsdDisplay(eSuperOrder))
				OSDCTRL_ItemHide(eSuperOrder);
	   }

	   if(timerId == eLaser_Timer)
	   {
	   		killLaserTimer();
			MSGDRIV_send(CMD_LASER_FAIL,(void*)LASERERR_TIMEOUT);
	   	
	   }

	     if(timerId == eOSD_shine_Timer)
	     {
     			//printf("eOSD_shine_Timer eOSD_shine_Timer eOSD_shine_Timer\n");
    
		    	// OSDCTRL_erase_single(pCtrlObj, eMeasureType);
			msgEraseId = eMeasureType;
     			pTimerObj->KillTimer(eOSD_shine_Timer);
			//Posd[eMeasureType] = MeasureTypeOsd[0];
			OSDCTRL_NoShineShow();	
	     }

		if(timerId == eGrenadeServo_Timer)
		{
			killSelfCheckGrenadeServoTimer();
			sendCommand(CMD_GENERADE_SERVO_ERR);
		}

		if(timerId == eMachGunServo_Timer)
		{
			killSelfCheckMachGunServoTimer();
			sendCommand(CMD_MACHINEGUN_SERVO_ERR);
		}

		if(timerId == ePosServo_Timer)
		{
			killSelfCheckPosServoTimer();
			sendCommand(CMD_POSITION_SERVO_ERR);		
		}

		if(timerId == eF3_Timer)
		{
			killF3Timer();
			sendCommand(CMD_STABLEVIDEO_SWITCH );	
		}

		if(timerId == eF2_Timer)
		{
			killF2Timer();
			sendCommand(CMD_ADCALIBMENU_SWITCH);	
		}

		if(timerId == eF5_Timer)
		{
			killF5Timer();
			sendCommand(CMD_MIDPARAMS_SWITCH );
		}

		if(timerId == eF6_Timer)
		{
			killF6Timer();
			//sendCommand(CMD_CONNECT_SWITCH);
		}

		if(timerId == eServoCheck_Timer)
		{
			if(isBootUpMode()&&isBootUpSelfCheck())
				return 0;
			sendCommand(CMD_SERVOTIMER_MACHGUN);
		}

		if(timerId == eBootUp_Timer)
		{
			CTimerCtrl * pCtrlTimer = pTimerObj;
			if(pCtrlTimer->GetTimerStat(eBootUp_Timer)!=eTimer_Stat_Stop)
			{
				pCtrlTimer->KillTimer(eBootUp_Timer);
			}
			sendCommand(CMD_BOOT_UP_CHECK_COMPLETE);
		}

		if(timerId == eFxbutton_Timer)
		{
			CTimerCtrl * pCtrlTimer = pTimerObj;
			if(pCtrlTimer->GetTimerStat(eFxbutton_Timer)!=eTimer_Stat_Stop)
			{
				pCtrlTimer->KillTimer(eFxbutton_Timer);
			}
			
			OSDCTRL_BaseMenuHide();
			if(isBattleMode())
			{
				OSDCTRL_ItemShow(eAngleV);
				OSDCTRL_ItemShow(eWeather2);
			}
			else if(isCalibrationMode())
			{
				OSDCTRL_ItemShow(eAnglePosn);
				OSDCTRL_ItemShow(eAngleMach);
				OSDCTRL_ItemShow(eAngleGred);
			}	
		}
		
		if(timerId == eCAN_Timer)
		{
			if(servoInit)
			{
				servoInit = 0;
				startServoCheck_Timer();
				sendCommand(CMD_SERVO_INIT);
			}
			
			if(isBootUpMode()&&isBootUpSelfCheck())
			{
				startServoCheck_Timer();
				return 0;
			}
			sendCommand(CMD_TIMER_SENDFRAME0);
			
			//if(bTraceSend)
			//	sendCommand(CMD_TRACE_SENDFRAME0);
		}

		
		if(timerId == eSchedule_Timer)
		{
			SCHEDULE_cbFxn(NULL);
		}	

		if(timerId == ePosAngle_Timer)
		{
			SelfCheckPosAngleTimer_cbFxn(NULL);
		}

		if(timerId == eDipAngle_Timer)
		{
			SelfCheckDipAngleTimer_cbFxn(NULL);
		}

		if(timerId == eMachGunAngle_Timer)
		{
			SelfCheckMachGunAngleTimer_cbFxn(NULL);
		}
		
		 if(timerId == eGrenadeAngle_Timer)
		 {
			killSelfCheckGrenadeAngleTimer();
			MSGDRIV_send(CMD_GENERADE_SENSOR_ERR,0);
		 }

		 
		 if(timerId == eWeaponCtrl_Timer)
		 {
			SelfCheckWeaponCtrlTimer_cbFxn(NULL);
		 }
			
	
    	return OSA_SOK;
}

int cltrl_dis_user(int value,int id)
{
}




/**************************************/

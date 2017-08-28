/*************************************************************************
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:		user_app.h
* Author:			xavier
* Date:			2014-01-14
* Description:		uaer interface api include file
* Version:			V 1.0.0
* Function List:	
*     				char APP_getChar( );
* 				static Int32 APP_loadDefCfg( );
* 				static Int32 APP_create( );
* 				static Int32 APP_destroy( );
* 				static Int32 APP_start( );
* 				static Int32 APP_stop( );
* 				static Int32 APP_getStatus( );
* 				static Int32 APP_control( );
* 				static Int32 APP_getConfig( );
*				static Int32 APP_setConfig( );
* 				static Int32 APP_stop( );
* 				static Int32 APP_onTimer( );
*
* History:			// histroy modfiy record
*     <author>  <time>   <version >   <desc>
*     xavier    14/01/14     1.0     build this moudle
***********************************************************************/
#ifndef __USER_APP_H__
#define __USER_APP_H__
#define MAX_INPUT_STR_SIZE 		( 128 )

#define MCFW_VCAP_VDIS_CASE 	        ( 0 )
#define MCFW_VCAP_VENC_CASE 	        ( 1 )
#define MCFW_IMAGE_PROCESS_BOARD_CASE   ( 2 )


#define SYS_INFOR_SHOW_TIMER			(0x00)
#define SYS_INFOR_SHOW_TIMER_TICKS	(1000)

#define OSD_SHOW_TIMER			    (0x01)
#define OSD_SHOW_TIMER_TICKS		    (40)//ms


//#define ALG_MTD_TIMER			        (0x02)
//#define ALG_MTD_TIMER_TICKS		    (40)//ms

//#define ALG_TRACK_TIMER			    (0x03)
//#define ALG_TRACK_TIMER_TICKS			(40)//ms
#define GPIO_INSPECT_TIMER                    (0x06)
#define GPIO_INSPECT_TICKS		    2000//(300) //ms


#define GRPX_SHOW_TIMER                    (0x04)
#define GRPX_SHOW_TIME_TICKS		    (150) //ms

#define ALG_TRACK_TIMER                    (0x05)
#define ALG_TRACK_TIMER_TICKS		    (40) //ms

#define BOARD_waitusec_TIMER		 	(0x06)
#define BOARD_waitusec_TIMER_TICKS		 (10) //ms


char APP_getChar( );
int cltrl_dis_user(int value,int id);
unsigned char APP_getint( );
void initgdxd_info();
void APP_setPrms( int nType );
#endif
/************************************** The End Of File *********************/

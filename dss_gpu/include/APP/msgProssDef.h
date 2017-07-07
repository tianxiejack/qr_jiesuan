/********************************************************************************
  Copyright (C), 2011-2012, CamRun Tech. Co., Ltd.
  File name:    msgProssDef.h
  Author:   xavier       Version:  1.0.0      Date: 2013-04-17
  Description: this file define the globle message process api function.
                and message id , both form the external interface and internal
                interface.
  Others:
  Function List:
  History:
    1. Date:
       Author:
       Modification:
    2. ...
********************************************************************************/

#ifndef _MSG_PROSS_DEF_H_
#define _MSG_PROSS_DEF_H_

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "app_global.h"
#include "msgDriv.h"

typedef enum _sys_msg_id_ {
    MSGID_SYS_INIT  = 0x00000000,           ///< internal cmd system init.
    MSGID_SYS_RESET,
    //MSGID_AVT_RESET,

    MSGID_EXT_INPUT_SYSMODE = 0x00000010,   ///< external cmd, system work mode.
    MSGID_EXT_INPUT_SENSOR,                 ///< external cmd, switch sensor.

    // AVT21 track
    MSGID_EXT_INPUT_FOVSTAT,                ///< external cmd, switch fov.
    MSGID_EXT_INPUT_CFGSAVE,                ///< external cmd, config save(here only for axis save)
    MSGID_EXT_INPUT_AXISPOS,                ///< external cmd, set pos of cross axis.
    MSGID_EXT_INPUT_TRACKALG,             ///< external cmd, set track alg.
    MSGID_EXT_INPUT_TRACK,                  ///< external cmd, start track or stop.
    MSGID_EXT_INPUT_AIMPOS,                 ///< external cmd, set pos of aim area.
    MSGID_EXT_INPUT_AIMSIZE,                ///< external cmd, set size of aim area.
    //MSGID_EXT_INPUT_SCENELOCK,                ///< external cmd, scene set or clear
    //MSGID_EXT_INPUT_TRKBLEED,               ///< external cmd, close bleed
    MSGID_EXT_INPUT_COAST,              ///< external cmd, coast set or cannel

    // img control
    MSGID_EXT_INPUT_ENPICP = 0x00000020,        ///< external cmd, open picture close.
    MSGID_EXT_INPUT_ENZOOM,                 ///< external cmd, zoom near or far.
    MSGID_EXT_INPUT_ENPICPZOOM,             ///< external cmd, picp zoom near or far.
    MSGID_EXT_INPUT_ENENHAN,                ///< external cmd, open image enhan or close.
    MSGID_EXT_INPUT_ENMTD,                  ///< external cmd, open mtd or close.
    MSGID_EXT_INPUT_MTD_SELECT,              ///< external cmd, select mtd target.
    MSGID_EXT_INPUT_ENSTB,                  ///< external cmd, open image stb or close.
    MSGID_EXT_INPUT_ENRST,                  ///< external cmd, open image rst or close.
    MSGID_EXT_INPUT_RST_THETA,              ///< external cmd, open image rst or close.

    // osd control
    MSGID_EXT_INPUT_DISPGRADE = 0x00000030,  ///< external cmd, osd show or hide
    MSGID_EXT_INPUT_DISPCOLOR,              ///< external cmd, osd color

	MSGID_EXT_INPUT_BLOBDECT,
    // video control
    MSGID_EXT_INPUT_VIDEOCTRL,              ///< external cmd, video record or replay.

}
eSysMsgId, MSG_PROC_ID;

//void MSGAPI_initial(MSGDRIV_Handle handle);
void MSGAPI_unInitial(MSGDRIV_Handle handle);

//void MSGAPI_init_device(LPARAM lParam       /*=NULL*/);
void MSGAPI_reset_device(LPARAM lParam      /*=NULL*/);
void MSGAPI_reset_avt(LPARAM lParam     /*=NULL*/);

void MSGAPI_setSysModeStat(LPARAM lParam     /*=NULL*/);
void MSGAPI_setSensorStat(LPARAM lParam     /*=NULL*/);
void MSGAPI_setFovStat(LPARAM lParam        /*=NULL*/);

void MSGAPI_saveConfig(LPARAM lParam        /*=NULL*/);
void MSGAPI_setCrossRefine(LPARAM lParam        /*=NULL*/);
void MSGAPI_setTrackAlg(LPARAM lParam        /*=NULL*/);
void MSGAPI_setTrackStat(LPARAM lParam      /*=NULL*/);
void MSGAPI_runTrackImgProc(LPARAM lParam      /*=NULL*/);
void MSGAPI_runTrackMtdProc(LPARAM lParam      /*=NULL*/);
//void MSGAPI_setTrackBleed(LPARAM lParam     /*=NULL*/);
void MSGAPI_setAimRefine(LPARAM lParam          /*=NULL*/);
void MSGAPI_setAimSize(LPARAM lParam    /*=NULL*/);
void MSGAPI_setForceCoast(LPARAM lParam /*=NULL*/);

void MSGAPI_setImgPicp(LPARAM lParam            /*=NULL*/);
void MSGAPI_setImgZoom(LPARAM lParam            /*=NULL*/);
void MSGAPI_setImgEnh(LPARAM lParam         /*=NULL*/);
void MSGAPI_setImgMtd(LPARAM lParam         /*=NULL*/);
void MSGAPI_setImgMtdSelect(LPARAM lParam   /*=NULL*/);
void MSGAPI_setImgStb(LPARAM lParam             /*=NULL*/);
void MSGAPI_setImgRst(LPARAM lParam             /*=NULL*/);
void MSGAPI_setImgTheta(LPARAM lParam               /*=NULL*/);

void MSGAPI_setDispGrade(LPARAM lParam      /*=NULL*/);
void MSGAPI_setDispColor(LPARAM lParam      /*=NULL*/);

void MSGAPI_setVideoCtrl(LPARAM lParam      /*=NULL*/);

//#ifdef __cplusplus
//}
//#endif

#endif

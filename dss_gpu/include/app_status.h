/********************************************************************************
  Copyright (C), 2011-2012, CamRun Tech. Co., Ltd.
  File name:    app_status.h
  Author:   xavier       Version:  1.0.0      Date: 2013-04-17
  Description: this file define the globle externel input state struct
              together with the avt ouput state struct.
  Others:
  Function List:
  History:
    1. Date:
       Author:
       Modification:
    2. ...
  History:
    <author>  <time>      <version >   <desc>
      aloysa     17/04/25   1.0     
********************************************************************************/

#ifndef _GLOBAL_STATUS_H_
#define _GLOBAL_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
* *PROJ ITEM
*/
//#define PROJ_BASE
#define PROJ_XGS026	1



#if PROJ_XGS026
#include "app_proj_xgs026.h"
#endif

#ifndef MTD_TARGET_NUM
#define MTD_TARGET_NUM  8
#endif

typedef enum {
	eSen_TV	= 0x00,
	eSen_FR    = 0x01,
	eSen_Max   = 0x02
}eSenserStat;
typedef enum {
	ePicp_top_left = 0x00,
	ePicp_top_right = 0x01,
	ePicp_bot_left = 0x02,
	ePicp_bot_right = 0x03,
}ePicpPosStat;

typedef enum Dram_SysMode
{
    INIT_MODE           = 0x00,
    CHECK_MODE      = 0x01,
    AXIS_MODE           = 0x02,
    NORMAL_MODE     = 0x03,
} eSysMode;

typedef enum Dram_zoomCtrl
{
    eZoom_Ctrl_Pause    = 0x00,
    eZoom_Ctrl_SCAL2    = 0x01,
    eZoom_Ctrl_SCAL4    = 0x02,
    eZoom_Ctrl_ADD      = 0x01,
    eZoom_Ctrl_SUB      = 0x02,
} eZoomCtrl;

typedef enum Dram_ImgAlg
{
    eImgAlg_Disable     = 0x00,
    eImgAlg_Enable      = 0x01,
} eImgAlgStat;  // use by img zoom/enh/stb/mtd/rst

typedef enum Dram_MMTSelect
{
    eMMT_No     = 0x00,
    eMMT_Next       = 0x01,
    eMMT_Prev       = 0x02,
    eMMT_Select = 0x03,
} eMMTSelect;

typedef enum Dram_DispGradeStat
{
    eDisp_hide      = 0x00,
    eDisp_show_rect     = 0x01,
    eDisp_show_text     = 0x02,
    eDisp_show_dbg      = 0x04,
} eDispGrade;

typedef enum Dram_DispGradeColor
{
    ecolor_Default  = 0x0,
    ecolor_Black = 0x1,
    ecolor_White    = 0x2,
    ecolor_Red = 0x03,
    ecolor_Yellow = 0x04,
    ecolor_Blue = 0x05,
    ecolor_Green = 0x06,
} eOSDColor;

typedef enum
{
    eTrk_Acq        = 0x00,
    eTrk_Normal = 0x01,
    eTrk_Assi       = 0x02,
    eTrk_Lost       = 0x03,
} eTrkStat;

typedef enum Dram_trkMode
{
    // mode cmd
    eTrk_mode_acq       = 0x00,
    eTrk_mode_target    = 0x01,
    eTrk_mode_mtd       = 0x02,
    eTrk_mode_sectrk    = 0x03,
    eTrk_mode_search    = 0x04,

    eTrk_mode_switch      = 0x100,

} eTrkMode;

typedef enum Dram_trkRefine
{
    eTrk_ref_no     = 0x00,
    eTrk_ref_left   = 0x01,
    eTrk_ref_right  = 0x02,
    eTrk_ref_up     = 0x01,
    eTrk_ref_down   = 0x02,
} eTrkRefine;

typedef enum Dram_saveMode
{
    eSave_Disable       = 0x00,
    eSave_Enable        = 0x01,
    eSave_Cancel        = 0x02,
} eSaveMode;


/** universal status **/
typedef struct
{
    /***** self stat part *****/
    volatile unsigned int  unitVerNum;      // 1.23=>0x0123

    volatile unsigned int  unitFaultStat;   // bit0:tv input bit1:fr input bit2:avt21
     volatile unsigned int  unitFaultStatpri;   // bit0:tv input bit1:fr input bit2:avt21
    volatile unsigned int  unitWorkMode;    // eSysMode

    volatile unsigned int  unitFovClass;     // fov zoom class
    volatile float  unitFovAngle[eSen_Max];
    volatile int unitLoadAxis[eSen_Max][2][5];
    volatile int unitDyAxis[eSen_Max][2][5];

    volatile unsigned int  unitMtdTgsId[2/*eSen_Max*/][MTD_TARGET_NUM];
    volatile unsigned int  unitMtdMajorId[eSen_Max];
    volatile unsigned int  unitMtdValid;    // 0-disable 1-valid 2-unvalid
    volatile unsigned int  unitMtdPixelX;
    volatile unsigned int  unitMtdPixelY;

    /***** self osd part *****/
    // trk real-time info
    volatile unsigned int unitTrkStatpri;
    volatile unsigned int  unitTrkStat;     // acp/trk/assi/lost
    volatile unsigned int  unitAxisX[eSen_Max];     // pixel
    volatile unsigned int  unitAxisY[eSen_Max];     // pixel
    volatile unsigned int  unitAimW;      // aim size
    volatile unsigned int  unitAimH;      // aim size
    volatile unsigned int  unitAimX;
    volatile unsigned int  unitAimY;
    volatile float unitTrkX;    // for report and osd text
    volatile float unitTrkY;    // for report and osd text
    //volatile unsigned int  unitAimFlicker;  // for assi count

    volatile unsigned int  unitSensorSwitch;    // for hide cross and aim when sens switching

    /***** cmd stat part *****/
    volatile unsigned int  SensorStat;      // eSenserStat
    volatile unsigned int  PicpSensorStat;  // sensor src id range 0~3 or 0xFF no picp sens
    volatile unsigned int  PicpSensorStatpri;  // sensor src id range 0~3 or 0xFF no picp sens
    volatile unsigned int  PicpPosStat;		// ePicpPosStat

    volatile unsigned int  ImgZoomStat[eSen_Max];   // eImgAlgStat
    volatile unsigned int  ImgEnhStat[eSen_Max];    // eImgAlgStat
    volatile unsigned int  ImgMtdStat[eSen_Max];    // eImgAlgStat
    volatile unsigned int  ImgMtdSelect[eSen_Max];  // eMMTSelect or range 0-MTD_TARGET_NUM
    volatile unsigned int  ImgBlobDetect[eSen_Max];    // eImgAlgStat
     volatile unsigned int  ImgFrezzStat[eSen_Max];    // eImgAlgStat
	volatile unsigned int  ImgVideoTrans[eSen_Max];    // eImgAlgStat
    volatile unsigned int  ImgPicp[eSen_Max];    // eImgAlgStat
    volatile unsigned int  TransEncMask;	// cur ver no used
    volatile unsigned int  TransEncFps[eSen_Max];	// cur ver no used

    volatile unsigned int  AvtTrkStat;      // eTrkMode 
    volatile unsigned int  AvtTrkAimSize;       // 0-3
    volatile unsigned int  AvtCfgSave;      // eSaveMode
    //volatile unsigned int  AvtTrkBleed;     // eBleedMode
    volatile unsigned int  AvtTrkCoast;
    volatile unsigned int  AvtTrkAlgStat;   // eTrkAlg

    volatile  int  AvtMoveX;        // eTrkRefine (axis or aim)
    volatile  int  AvtMoveY;        // eTrkRefine (axis or aim)

    volatile  int  AvtPosXTv;        // eTrkRefine (axis or aim)
    volatile  int  AvtPosYTv;        // eTrkRefine (axis or aim)
    
    volatile  int  AvtPosXFir;        // eTrkRefine (axis or aim)
    volatile  int  AvtPosYFir;        // eTrkRefine (axis or aim)

    volatile  int  CollPosXFir;        // eTrkRefine (axis or aim)
    volatile  int  CollPosYFir;        // eTrkRefine (axis or aim)
    
    volatile unsigned int  AvtPixelX;           // for ext designated
    volatile unsigned int  AvtPixelY;           // for ext designated

    volatile unsigned int  AvtImgTrkStat;      // for search pos 0-exit 1-enter


    /***** cmd osd part *****/
    volatile unsigned int  DispGrp[eSen_Max];       // eDispGrade
    volatile unsigned int  DispColor[eSen_Max];       // eOSDColor or eGRPColor
    //volatile unsigned int  DispTimePrm;    // bit0-bit7 second  bit8-bit15 minute  bit16-bit31 hours
    //volatile unsigned int  DispYearPrm;    // bit0-bit7 day  bit8-bit15 month  bit16-bit31 year

#if PROJ_XGS026
    volatile unsigned int  DispWorkMode;    // eWorkMode

    volatile unsigned int  DispPlatAngle[2];       // [0] is pos [1] is pitch
    volatile unsigned int  DispFovAnglePan[eSen_Max];
    volatile float  DispFovAngleTil[eSen_Max];

    volatile unsigned int  DispZoomStat[eSen_Max];
    volatile unsigned int  DispZoomMultiple[eSen_Max];  // step rang 100-400
    volatile unsigned int  DispEnhStat[eSen_Max];       // 0-off 1-alg on  2-ccd on 3-all on

    volatile unsigned int  DispFlirStat;        // bit16 powerstat 0-off 1-on  bit0-bit15 modestat
    volatile unsigned int  DispLaserStat;   // bit16 powerstat 0-off 1-on  bit0-bit15 modestat 000-stop 001-wait 010-once measure ...
    volatile unsigned int  DispDistance;    // laser distance 0-65535
    volatile unsigned int  DispLaserCode;   // laser code values
#endif



	volatile unsigned char CmdType;  // recv cmd id
	volatile unsigned char  TrkCmd; //only for ack trk mod
	volatile unsigned char TrkBomenCtrl; // osd Trk Aim
	volatile  unsigned char  FovMov; // 1:fang wei 2: fu yang 3:all

	volatile unsigned int TempTvCollX;
	volatile unsigned int TempTvCollY;

	volatile	unsigned char  MMTTempStat;	//for ack mmt stat
	volatile unsigned char MtdOffsetXY[20]; //mtd xy
	volatile unsigned char Mtdtargetnum; //mtd xy
	volatile unsigned char ImgZoom;
	volatile unsigned char ImgSmall;
	volatile unsigned int  TvCollimation;   //dianshi zhunzhi
	volatile unsigned int  FrCollimation;   //rexiang zhunzhi

	volatile unsigned char TargetPal; // fr target pal 
	volatile unsigned char SecAcqStat;
	volatile  unsigned int   Proc_ver;		// 1.23=>0x0123
	volatile  unsigned int   Proc_year,Proc_month,Proc_day;	// year: -2000, month:1-12, day:1-31 

	volatile unsigned char  AimScaleStat;	// 0x01 or 0x02
	volatile unsigned char  FovCtrl; 


	volatile unsigned char SecAcqFlag;
	volatile unsigned int  TrkCoastCount;
	volatile unsigned int  FreezeresetCount;
	  volatile unsigned char  FovSelectColor;
	    volatile unsigned int  ImgPixelX[eSen_Max];    // for img trk alg
    volatile unsigned int  ImgPixelY[eSen_Max];    // for img trk alg
        volatile unsigned char  SysMode; 
	 volatile unsigned char  Swdata[5];
	     volatile unsigned char  FovStat;       /* 1 byte ext-input fov:0 Large fov ,1 midle fov,2 small fov,3 electric x2 fov */
		     volatile unsigned char TVSStat; //for ack tv
	    volatile unsigned char FLIRPower;//for ack flir
    volatile unsigned char FLIRStat;//for ack flir
    volatile unsigned char FLIRPal;//for ack flir

	    volatile unsigned char  LRFPowerOn;		// 0-off 1-on
    volatile unsigned char  LRFReady;	
    volatile unsigned char  LRFStat;
    volatile unsigned char  LRFNStat;	 
    volatile unsigned char  LRFBright;
    volatile unsigned char  LRFRun;
    volatile unsigned char  LRFAllow;
    volatile unsigned char  LRFEngLev;
    volatile unsigned int  unitTvCollX;
    volatile unsigned int  unitTvCollY;
    volatile unsigned char  MenuStat;
    volatile int  TarLongitude;//jin du
    volatile int  TarLatitude;//wei du
    volatile short  TarHeight;	


    volatile unsigned char SysMsgId;
    volatile unsigned char SysMsgData[64];  
    volatile unsigned char SysMsgLen;
    volatile short PaneData;
    volatile short TitleData;
    volatile unsigned int  Distant;
    volatile unsigned int  trkerrx;
    volatile unsigned int  trkerry;
    volatile  short  TrkPanev;
    volatile  short  TrkTitlev;
    volatile float ZoomMultiple;

        //   volatile unsigned int  ImgZoomStat[eSen_Max];   // eImgAlgStat
      //       volatile unsigned char TrkBomenCtrl; // osd Trk Aim
 //                volatile unsigned char TrkBomenCtrl; // osd Trk Aim
} CMD_EXT;

typedef struct
{
    union
    {
        unsigned char c[4];
        float         f;
        int           i;
    } un_data;
} ACK_REG;

typedef struct
{
    unsigned int  NumItems;
    unsigned int  Block[16];        // ackBlk
    ACK_REG Data[16];
} ACK_EXT;

typedef enum 
{
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
    MSGID_EXT_INPUT_FOVSELECT,        //FOVSELECT
    MSGID_EXT_INPUT_SEARCHMOD,        //FOVSELECT

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
    MSGID_EXT_INPUT_ENBDT,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_ENFREZZ,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_PICPCROP,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_VIDEOEN,              ///< external cmd, open image bdt or close.

    // osd control
    MSGID_EXT_INPUT_DISPGRADE = 0x00000030,  ///< external cmd, osd show or hide
    MSGID_EXT_INPUT_DISPCOLOR,              ///< external cmd, osd color


     MSGID_EXT_INPUT_COLOR,                 ///< external cmd, switch input video color.

    // video control
    MSGID_EXT_INPUT_VIDEOCTRL,              ///< external cmd, video record or replay.

}MSG_PROC_ID;




typedef struct
{
	
	unsigned int  CaptureWidth  [eSen_Max];    // for width
	unsigned int  CaptureHeight [eSen_Max];    // for height
	unsigned int  CaptureType    [eSen_Max];	   //date type
	unsigned int  DispalyWidth   [eSen_Max];    // dispaly width
	unsigned int  DispalyHeight  [eSen_Max];    // display height




}CMD_SYS;


#ifdef __cplusplus
}
#endif

#endif



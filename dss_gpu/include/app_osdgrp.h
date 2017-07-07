/*************************************************************************
  Copyright (C), 2015-2016, ChamRun Tech. Co., Ltd.
  File name:    Multich_graphic.h
  Author:       zr       Version:  1.0.0      Date: 2016-9-23
  Description:
  Version:      1.0.0
  Function List:
  History:
      <author>  <time>   <version >   <desc>
        zr   16/09/23     1.0     build this moudle
**************************************************************************/

#ifndef _MULTICH_GRAPHIC_H
#define _MULTICH_GRAPHIC_H
#include <osa_sem.h>
#include <osa.h>
#define ALG_LINK_GRPX_MAX_CH	2
#define ALG_LINK_GRPX_MAX_WINDOWS	64

typedef enum
{
	grpx_ObjId_Cross = 0x00,
	grpx_ObjId_Rect,
	grpx_ObjId_Rect_gap,
	grpx_ObjId_Compass,
	grpx_ObjId_Text,

	// for project
	grpx_objId_Hori_Menu ,
	grpx_objId_Vert_Menu ,
       grpx_objId_SecondTrk_Cross ,
       grpx_objId_SecondTrk_Guide ,
       
	grpx_ObjId_Cross_Black_White,
	grpx_ObjId_Rect_Black_White,
	grpx_ObjId_Sectrk,
	grpx_ObjId_Cross_num,

	grpx_ObjId_Max,
	
}GRPX_OBJ_ID;

typedef struct _text_param_fb
{
	UInt32 enableWin;
	UInt32 objType;        //GRPX_OBJ_ID
	UInt32 frcolor;
	UInt32 bgcolor;

	Int16 text_x[3];
	Int16 text_y[3];
	Int16 textLen[3];

	Int16 text_valid;  // bit0-> text0; bit1-> text1; bit2-> text2
	UInt8 textAddr[512];
} Text_Param_fb;

typedef struct  _line_param_fb{
	UInt32 enableWin;
	UInt32 objType;        //GRPX_OBJ_ID
	UInt32 frcolor;
	UInt32 bgcolor;

	Int16 x;		        //center position
	Int16 y;		        //center position
	Int16 res0;

	Int16 width;            //graphic width
	Int16 height;           //graphic height
	Int16 res1;

	Int16 linePixels;		//line width
	Int16 lineGapWidth;     //must be <= width
	Int16 lineGapHeight;    //must be <= height
}Line_Param_fb;

typedef struct
{
	UInt32 chId;
	//VDIS_DEV devId;
	Bool chEnable;

	UInt32 numWindows;
	Text_Param_fb winPrms[ALG_LINK_GRPX_MAX_WINDOWS];
	Text_Param_fb winPrms_pri[ALG_LINK_GRPX_MAX_WINDOWS];
} AlgLink_GrpxChParams;

typedef struct _Multich_graphic_
{
    Bool bGraphicInit;
    Int32 devFb_num;
	
    AlgLink_GrpxChParams  chParam[ALG_LINK_GRPX_MAX_CH];
    OSA_MutexHndl muxLock;
    OSA_SemHndl tskGraphicSem;
    OSA_ThrHndl tskGraphicHndl;
    Bool tskGraphicLoop;
    Bool tskGraphicStopDone;
} Multich_graphic;

#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define YELLO 0xFFFFFF00
#define BLACKCOLOR	0xFF000000
#define WHITECOLOR	0xFFFFFFFF
#define BGCOLOR 0x00000000
#define MAKERGBA(R,G,B,A)		((unsigned int)((((unsigned int)(A)|((unsigned int)((unsigned int)(B))<<8))|(((unsigned int)(unsigned int)(G))<<16))|(((unsigned int)(unsigned int)(R))<<24)))
#define GETRGBA(R,G,B,A, RGBA)	(R) = RGBA & 0xff;	(G) = ((RGBA)>>8) & 0xff;	(B) = ((RGBA)>>16) & 0xff;	(A) = ((RGBA)>>24) & 0xff;


#define BIT_SET(val,bit) (val|(1<<bit))
#define BIT_CLR(val,bit) (val|(~(1<<bit)))
#define BIT_CLRj(val,bit) (val&(~(1<<bit)))

#endif
/*******************************END***************************************/

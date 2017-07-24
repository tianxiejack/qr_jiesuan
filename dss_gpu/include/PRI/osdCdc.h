/*
 *  Copyright 2009 by QianRun Incorporated.
 *  All rights reserved. Property of QianRun Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) 12/16/2009 AVT VERSION CDC.h" */
/*
 *  ========version 1.0 ========
 */
#ifndef _CDC_H_
#define _CDC_H_

#ifdef __cplusplus
extern "C"{
#endif

//#include "sdkDef.h"
//#include "sdkErr.h"
//#include "image.h"

#include "basePort.h"

/* default font */
#define OSDUTIL_FONT_DEFAULT           	NULL
#define OSDUTIL_FONT_DEFAULT_WIDTH     	24//16 //10
#define OSDUTIL_FONT_DEFAULT_HEIGHT    	32//22 //16
#define OSDCOLOR_NODRAW					0

#define WHITECOLOR 						0xFFFFFFFF
#define BLACKCOLOR						0xFF000000
#define BLANKCOLOR						0x00000000


//OSD STATE
#define OSD_HIDE	 0
#define OSD_DISP	 1
#define OSD_UPDATE	 2

typedef enum _osd_obj{
	OSD_HLINE		=0,
	OSD_VLINE		=1,
	OSD_RECT		=2,
	OSD_FONT		=3
}OsdObj_Type;



typedef struct _line_obj{
	int left;
	int top;
	int lineLen;
	int lineBold;
	COLOR yuv;
}LINE_OBJ,*LINE_Handle;

typedef struct _cdc_pix{
	int x;
	int y;
}CPoint;

typedef struct _rect_obj{
	int left;
	int top;
	int right;
	int bottom;
	int width;
	int height;
	int lineBold;
	COLOR bordColor;
	COLOR fillColor;
	bool isFilled;
}RECT_OBJ,*RECT_Handle;


/*
typedef struct _font_obj{
	OSDUTIL_Font *font;
	COLOR fgColor; 
	COLOR bgColor;
	unsigned int OrgX; 
	unsigned int OrgY;
	unsigned int cSize;
	unsigned char* pchar;
}FONT_OBJ,*FONT_Handle;
*/


typedef struct _cdc{
	UINT segId;
	UINT osdType;
	UINT uStat;
	UINT uSize;
	BYTE uParm[4];
}CDC_OBJ,*CDC_Handle;

#if 0
unsigned char gCircle[]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x06,0x0F,0xE0,0x06,0x0F,0xE0,0x06,0x0C,0x60,0x06,0x0C,0x60,0x06,0x0C,0x60,0x7F,0xCC,0x60,0x7F,0xCC,
0x60,0x0E,0x0C,0x60,0x0E,0x0C,0x60,0x0E,0x0C,0x60,0x0E,0x0C,0x60,0x0F,0x8C,0x60,0x1F,0x8C,0x60,0x1E,0xCC,0x60,0x1E,0xCC,0x60,0x3E,0x8C,0x60,0x36,0x18,0x60,0x76,
0x18,0x60,0x66,0x18,0x60,0x26,0x18,0x60,0x06,0x18,0x60,0x06,0x30,0x64,0x06,0x30,0x66,0x06,0x70,0x66,0x06,0xE0,0x7E,0x06,0x60,0x3C,0x06,0x00,0x00,0x00,0x00,0x00,/*"��",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x02,0x00,0x06,0x03,0x80,0x06,0x03,0x00,0x06,0x07,0x80,0x06,0x07,0x80,0x06,0x06,0xC0,0x06,0x0C,0xC0,0x7F,0x8C,0x60,0x7F,0x98,
0x70,0x0E,0x30,0x38,0x0E,0x70,0x1C,0x0E,0x7F,0xFC,0x0E,0x1F,0xF4,0x1F,0x18,0x30,0x1F,0x98,0x30,0x1F,0x98,0x30,0x3E,0xD8,0x30,0x36,0x98,0x30,0x66,0x18,0x30,0x66,
0x18,0xE0,0x06,0x18,0xE0,0x06,0x18,0x80,0x06,0x18,0x00,0x06,0x18,0x0C,0x06,0x18,0x0C,0x06,0x1F,0xFC,0x06,0x0F,0xF8,0x06,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,/*"ǹ",1*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x40,0x06,0x18,0x60,0x06,0x18,0x60,0x03,0x18,0xC0,0x03,0x18,0xC0,0x02,0x18,0x80,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x30,0x00,
0x0C,0x30,0x00,0x0C,0x37,0xFF,0xEC,0x37,0xFF,0xEC,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x07,0xFF,0xE0,0x07,0xFF,0xE0,0x00,0x18,0x00,0x00,0x18,0x00,0x1F,
0xFF,0xF8,0x1F,0xFF,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0x18,0xF0,0x00,0x18,0xC0,0x00,0x18,0x00,0x00,0x00,0x00,/*"��",2*/

};
#endif

void CDC_drawVLine(IMAGE_Handle hIMG, HANDLE LineParm);
void CDC_drawHLine(IMAGE_Handle hIMG, HANDLE LineParm);
void CDC_drawText (IMAGE_Handle hIMG, HANDLE FontParm);
void CDC_drawSLine(IMAGE_Handle hIMG, HANDLE LineParm);
void CDC_drawCircleAndSLine(IMAGE_Handle hIMG, HANDLE LineParm,int Theta);
void CDC_drawTest(IMAGE_Handle hIMG,HANDLE FontParm);

//extern OSDUTIL_Font fontDefault;

#ifdef __cplusplus
}
#endif


#endif



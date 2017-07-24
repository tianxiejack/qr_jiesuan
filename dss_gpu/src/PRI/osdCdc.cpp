/*
 *  Copyright 2009 by QianRun Incorporated.
 *  All rights reserved. Property of QianRun Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) 12/16/2009 AVT VERSION CDC.c" */
/*
 *  ========version 1.0 ========
 */
//#include <std.h>
//#include <csl_dat.h>
//#include <csl_cache.h>

//#include "main.h"
#include "osdCdc.h"
#include "font.h"

#define PI (3.14159)
#define DEGREE2RAD(data) ((data)*PI/180)

//extern unsigned char FONT_LIBRARY[];
//extern unsigned char gCircle[];

OSDUTIL_Font fontDefault = 
{
    OSDUTIL_FONT_DEFAULT_WIDTH, 
    OSDUTIL_FONT_DEFAULT_HEIGHT,
    FONT_LIBRARY
};
/*
 *  ======== CDC_drawVLine========
 *
 *  This function is used to draw vline
 */
void CDC_drawVLine(IMAGE_Handle hIMG, HANDLE LineParm)
{
	Uint8 *pdstY;
	int i;
	Uint16 dat16;
	Uint32 dat32;
	Uint8 *Align8Arry;
	Uint8 YVal;
	LINE_Handle pLine = (LINE_Handle)LineParm;
	int len;

	int VideoWidth =0;
	if(hIMG==NULL || LineParm ==NULL){
		return;
	}
	#if 0
	YVal = pLine->yuv.data[0];
	len  = pLine->lineLen;
	
	pdstY = hIMG->ImgY + pLine->top * hIMG->widthStep + pLine->left;

	VideoWidth = hIMG->widthStep;
	
	Align8Arry = &DstImg[0][0];

	if(pLine->lineBold == 8 && (((int)pdstY&0x07)==0)){	
		Align8Arry[0] = YVal;
		Align8Arry[1] = YVal;
		_amem2(Align8Arry+2) = _amem2(Align8Arry);
		_amem4(Align8Arry+4) = _amem4(Align8Arry);
		
		for(i = 0; i < len; i++){
			_amem8(pdstY) = _amem8(Align8Arry);
			pdstY += VideoWidth;
		}
	}else if(pLine->lineBold == 4 && (((int)pdstY&0x03)==0)) {
		dat32 = (YVal << 24) | (YVal << 16) | (YVal << 8) | YVal;
		for(i = 0; i < len; i++){
			_amem4(pdstY) = dat32;
			pdstY += VideoWidth;
		}	
	}else if(pLine->lineBold == 2 && (((int)pdstY&0x01)==0)){
		dat16 = (YVal << 8) | YVal;
		for(i = 0; i < len; i++){
			_amem2(pdstY) = dat16;
			pdstY += VideoWidth;
		}			
	}else{
		for(i = 0; i < len; i++){
			*pdstY = YVal;
			pdstY += VideoWidth;
		}
	
	}
	#endif
}
/*
 *  ======== CDC_drawHLine========
 *
 *  This function is used to draw hline
 */
void CDC_drawHLine(IMAGE_Handle hIMG, HANDLE LineParm)
{
	Uint8 *pdstY;
	Uint8 *pdstY1;
	int i, y;
	Uint16 dat16;
	Uint32 dat32;
	Uint8 *Align8Arry;
	Uint8 YVal;
	
	int LineW =0;
	int VideoWidth=0;		
	LINE_Handle pLine = (LINE_Handle)LineParm;
	int len =0;

	#if 0
	if(hIMG==NULL || LineParm ==NULL){
		return;
	}

	YVal = pLine->yuv.data[0];
	len  = pLine->lineLen;

	pdstY = hIMG->ImgY + pLine->top * hIMG->widthStep + pLine->left;
	
	VideoWidth = hIMG->widthStep;
	LineW = pLine->lineBold;
	Align8Arry = &DstImg[0][0];

	if((len&7) == 0 && (((int)pdstY)&0x07 == 0)){	
		Align8Arry[0] = YVal;
		Align8Arry[1] = YVal;
		_amem2(Align8Arry+2) = _amem2(Align8Arry);
		_amem4(Align8Arry+4) = _amem4(Align8Arry);
		
		len = len>>3;

		for(y = 0; y < LineW; y++){
			pdstY1 = pdstY + y*VideoWidth;
			for(i = 0; i < len; i++){
				_amem8(pdstY1) = _amem8(Align8Arry);
				pdstY1 += 8;
			}
		}
	}else if((len&3) == 0 && (((int)pdstY)&0x03 == 0)){
		dat32 = (YVal << 24) | (YVal << 16) | (YVal << 8) | YVal;
		len = len>>2;
		for(y = 0; y < LineW; y++){
			pdstY1 = pdstY + y*VideoWidth;
			for(i = 0; i < len; i++){
				_amem4(pdstY1) = dat32;
				pdstY1 += 4;
			}
		}		
	}else if((len&3) == 2 && (((int)pdstY)&0x01 == 0)){
		dat16 = (YVal << 8) | YVal;
		len = len>>1;
		for(y = 0; y < LineW; y++){
			pdstY1 = pdstY + y*VideoWidth;
			for(i = 0; i < len; i++){
				_amem2(pdstY1) = dat16;
				pdstY1 += 2;
			}
		}			
	}else{
		for(y = 0; y < LineW; y++){
			pdstY1 = pdstY + y*VideoWidth;
			for(i = 0; i < len; i++){
				*pdstY1++ = YVal;
			}
		}
	}
	#endif
}

/*
 *  ======== CDC_drawText========
 *
 *  This function is used to draw a font
 */
void CDC_drawText(IMAGE_Handle hIMG,HANDLE FontParm)
{
	OSDUTIL_Font *font=NULL;
	
	unsigned int pitch;
	unsigned int locx,locy;

	char *pString=NULL;
	unsigned char fgColor,bgColor;

	Uint32 charSize;
	Uint32 stringSize;
	Uint8 data;
	Uint32 i, j, k;
	Uint8 *p;
	Uint8 pixColor;
	unsigned char c;
	Uint8 *pFrame=NULL;
	SDK_ASSERT(hIMG!=NULL && FontParm!=NULL);
	
	font	= ((FONT_OBJ*)FontParm)->font;
	fgColor = ((FONT_OBJ*)FontParm)->fgColor.data[0];
	bgColor = ((FONT_OBJ*)FontParm)->bgColor.data[0];
	pString	= (char*)(((FONT_OBJ*)FontParm)->pchar);

	pitch	= hIMG->width;
	pFrame	= hIMG->ImgY;

	if(font == OSDUTIL_FONT_DEFAULT) {
		font =  &fontDefault;
	}
	
	stringSize = ((FONT_OBJ*)FontParm)->cSize;		// see how long the string is
	if (stringSize == 0)
		return;
	
	locx = ((FONT_OBJ*)FontParm)->OrgX;
	locy = ((FONT_OBJ*)FontParm)->OrgY;

	charSize = (font->fontWidth + 7) >> 3;			// number of bytes per horz scan line
	
	pFrame += (locy * pitch) + locx;				// move to start of string
	for (i = 1; i < font->fontHeight-1; i++) {
		p = pFrame;
		for (j = 0; j < stringSize; j++) {
			c = pString[j];
			//c = (c < ' ') ? 0 : c - ' ';
			if(c > 128)
			{
				for (k = 1; k < font->fontWidth-1; k++) 
				{
					if(1==k)
						data = font->fontData[(c * font->fontHeight + i) * charSize + (k >> 3)];
					if ((k & 7) == 0)
						data = font->fontData[(c * font->fontHeight + i) * charSize + (k >> 3)];
					pixColor = (data & 0x80) ? fgColor : bgColor;
					if (pixColor != OSDCOLOR_NODRAW) 
					{
						*p++ = pixColor;
					} else
						p++;				// don't draw these pixels
					data <<= 1;				// next pixel in current scan line
				}
			//p+=2;						// two more for inter-character spacing
			}else
			{
				for (k = 1; k < font->fontWidth/2; k++) 
				{
					if(1==k)
						data = font->fontData[(c * font->fontHeight + i) * charSize + (k >> 3)];
					if ((k & 7) == 0)
						data = font->fontData[(c * font->fontHeight + i) * charSize + (k >> 3)];
					pixColor = (data & 0x80) ? fgColor : bgColor;
					if (pixColor != OSDCOLOR_NODRAW) 
					{
						*p++ = pixColor;
					} else
						p++;				// don't draw these pixels
					data <<= 1;				// next pixel in current scan line
				}
			}
		}
		pFrame += pitch;
	}
}


FONT_OBJ g_FontCircle={NULL,WHITECOLOR,0,0,0,0,NULL};

void CDC_drawTest(IMAGE_Handle hIMG,HANDLE FontParm)
{
#if 1	
 	OSDUTIL_Font font={24,32,gCircle};
	
	unsigned int pitch;
	unsigned int locx,locy;

//	char *pString=NULL;
	unsigned char fgColor,bgColor;

	Uint32 charSize;
	Uint32 stringSize;
	Uint8 data;
	Uint32 i, j,k;
	Uint8 *p;
	Uint8 pixColor;
//	unsigned char c;
	Uint8 *pFrame=NULL;
//	FONT_OBJ pFont;
	SDK_ASSERT(hIMG!=NULL && FontParm!=NULL);

//	pFont->OrgX    = 500;
//	pFont->OrgY    = 300;
//	pFont->pchar   = (unsigned char*)pTextObj->osdContext;
//	pFont->cSize   = 64;
//	pFont->bgColor	 = g_FontCircle.bgColor;
//	pFont->fgColor   = g_FontCircle.fgColor;
//	font	= ((FONT_OBJ*)FontParm)->font;

	fgColor = WHITECOLOR;
	bgColor = BLACKCOLOR;
	//pString	= (char*)(((FONT_OBJ*)FontParm)->pchar);

	pitch	= hIMG->width;
	pFrame	= hIMG->ImgY;

	stringSize = 1;		// see how long the string is
	if (stringSize == 0)
		return;

	locx = 240;
	locy = 65;

	charSize = ((font.fontWidth %8)>0)?((font.fontWidth>>3)+1):(font.fontWidth>>3);			// number of bytes per horz scan line
	
	pFrame += (locy * pitch) + locx;				// move to start of string
	for (i = 1; i <  font.fontHeight-1; i++) {//高度
		p = pFrame;
		for(j=0; j<3; j++){
			for (k = 1; k < font.fontWidth-1; k++) {//宽度
				if(1==k)
					data = font.fontData[(i +j*font.fontHeight)* charSize + (k >> 3)];//每个字符所占的字节数 n
				if ((k % 8) == 0)
					data = font.fontData[(i +j*font.fontHeight)* charSize + (k >> 3)];//每个字符所占的字节数 n
				pixColor = (data & 0x80) ? fgColor : bgColor;
				if (pixColor != OSDCOLOR_NODRAW) {
					*p++ = pixColor;
				} else
					p++;				// don't draw these pixels
				data <<= 1;				// next pixel in current scan line
			}
			//p+=2;						// two more for inter-character spacing
		}
		pFrame += pitch;
	}
#endif
}





void CDC_drawSLine(IMAGE_Handle hIMG, HANDLE LineParm)
{
	Uint8 *pdstY;
	Uint8 *pdstY1;
	int x, i, y;
	Uint8 YVal;
	double Theta = -PI/4;
	
	int VideoWidth=0;		
	int len =0;

	if(hIMG==NULL || LineParm ==NULL){
		return;
	}

	YVal = 0xE2;
	len  = 42;

	pdstY = hIMG->ImgY + 112 * hIMG->widthStep + 652;
	
	VideoWidth = hIMG->widthStep;

	for(i = 0; i < len; i++){
		x = i * cos(Theta);
		y = i * sin(Theta);
		pdstY1 = pdstY + y*VideoWidth +x;
		*pdstY1 = YVal;
	}

	for(i=0; i<360; i++){//画虚线圆
		if((i%30) < 12)
			continue;
		x = 30 * cos(2*PI*i/360);
		y = 30 * sin(2*PI*i/360);
		pdstY1 = pdstY + y*VideoWidth +x;
		*pdstY1 = YVal;
	}
	
	for(i=0; i<12; i++)
	{
		pdstY1 = pdstY + 30 + i;
		*pdstY1 = YVal;
		pdstY1 = pdstY - 30 - i;
		*pdstY1 = YVal;
	}
	for(i=0; i<12; i++)
	{
		pdstY1 = pdstY + (30+i)*VideoWidth;
		*pdstY1 = YVal;
		pdstY1 = pdstY - (30+i)*VideoWidth;
		*pdstY1 = YVal;
	}
}



void CDC_drawCircleAndSLine(IMAGE_Handle hIMG, HANDLE LineParm,int value)
{
	Uint8 *pdstY;
	Uint8 *pdstY1;
	int x, i, y;
	Uint8 YVal;
	double Theta;
	int VideoWidth=0;		
	int len =0;	
	LINE_Handle pLine = (LINE_Handle)LineParm;
	if(hIMG==NULL || LineParm ==NULL){
		return;
	}
	value %= 360;//对360取余
	Theta = DEGREE2RAD(value);//转换为弧度
	Theta -= PI/2;//逆时针旋转90度
	YVal = pLine->yuv.data[0];
	len  = pLine->lineLen;
	pdstY = hIMG->ImgY + pLine->top * hIMG->widthStep + pLine->left;
	VideoWidth = hIMG->widthStep;

	for(i = 0; i < len; i++){//画斜线
		x = i * cos(Theta);
		y = i * sin(Theta);
		pdstY1 = pdstY + y*VideoWidth +x;
		*pdstY1 = YVal;
		if((Theta<1)||(Theta>359)||abs(Theta-180)<1){
			pdstY1 = pdstY + y*VideoWidth +(x+1);
			*pdstY1 = YVal;
		}else{
			pdstY1 = pdstY + (y+1)*VideoWidth +x;
			*pdstY1 = YVal;
		}
	}

	for(i=0; i<360; i++){//画虚线圆
		if((i%18) < 16)
			continue;
		x = 30 * cos(2*PI*i/360);
		y = 30 * sin(2*PI*i/360);
		pdstY1 = pdstY + y*VideoWidth +x;
		*pdstY1 = YVal;
		if((0==i)||(180==i)){
			pdstY1 = pdstY + y*VideoWidth +(x+1);
			*pdstY1 = YVal;
		}else{
			pdstY1 = pdstY + (y+1)*VideoWidth +x;
			*pdstY1 = YVal;
		}
		
	}
	
	for(i=0; i<4; i++)
	{
		pdstY1 = pdstY + (36 + i);
		*pdstY1 = YVal;
		*(pdstY1+1) = YVal;
		pdstY1 = pdstY - (36 + i);
		*pdstY1 = YVal;
		*(pdstY1+1) = YVal;
	}
	for(i=0; i<4; i++)
	{
		pdstY1 = pdstY + (35+i)*VideoWidth;
		*pdstY1 = YVal;
		*(pdstY1+VideoWidth) = YVal;
		pdstY1 = pdstY - (35+i)*VideoWidth;
		*pdstY1 = YVal;
		*(pdstY1+VideoWidth) = YVal;
	}
}



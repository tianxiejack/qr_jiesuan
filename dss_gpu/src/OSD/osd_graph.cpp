
#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osd_graph.h"
#include "app_osdgrp.h"
#include "grpFont.h"
#include "app_status.h"
#include"dx.h"
#include"app_ctrl.h"
#include "msgDriv.h"
#include "statCtrl.h"


//#include "opencv2/opencv.hpp"

Multich_graphic grpxChWinPrms;

/*
 *  ======== OSDCTRL_draw_text========
 *
 *  This function is used to draw all the items
 */
void OSDCTRL_draw_text(Mat frame,OSDCTRL_Handle pCtrlObj)
{
	int i=0;
	//static int gun=0;//,type=0;
	//volatile FONT_OBJ *pFont = &g_Font;
	OSDText_Obj * pTextObj = NULL;
	int startx,starty;
	char *ptr;
	UInt32 frcolor,bgcolor;


	if(isBattleMode()&&isStatBattleAuto()&&(isBattleReady()||isAutoReady())&&(isGrenadeGas()||isGrenadeKill()))
		OSDCTRL_ItemShow(eReady);
	
	for(i=eModeId;i<eBoreSightLinId;i++)
	{
			pTextObj = &pCtrlObj->pTextList[i]; 
				if (pTextObj->osdState==eOsd_Disp)
				{
					if(pTextObj)
					OSDCTRL_genOsdContext(pCtrlObj,i);
					startx   = pTextObj->osdInitX;
					starty   = pTextObj->osdInitY;
					frcolor  = WHITECOLOR;
					bgcolor = BGCOLOR;
					ptr   = (char*)pTextObj->osdContext;
					if(i == eShotType && gProjectileType == PROJECTILE_BULLET && gGunShotType == SHOTTYPE_SHORT)
						frcolor = bgcolor;
					osd_chtext(frame, startx, starty, ptr, frcolor, bgcolor);
				}
		
	}
}


void OSDCTRL_erase_draw_text(Mat frame,OSDCTRL_Handle pCtrlObj)
{
	int i=0;

	OSDText_Obj * pTextObj = NULL;
	int startx,starty;
	char *ptr;
	UInt32 frcolor,bgcolor;
	char * tmpOsd;

	for(i=eModeId;i<=eBoreSightLinId;i++)
	{
		pTextObj = &pCtrlObj->pTextList[i]; 
		//if (pTextObj->osdState==eOsd_Hide)
		{
			if(pTextObj->osdId == eErrorZone)
			{
				tmpOsd = Posd[eErrorZone];
				Posd[eErrorZone] = ErrorOsd[18];
			}

			if(pTextObj)
				OSDCTRL_genOsdContext(pCtrlObj,i);
			else
				continue;	
			startx   = pTextObj->osdInitX;
			starty   = pTextObj->osdInitY;
			frcolor  = WHITECOLOR;
			bgcolor = BGCOLOR;
			ptr   = (char*)pTextObj->osdContext;
			osd_chtext(frame, startx, starty, ptr, bgcolor, bgcolor);

			if(pTextObj->osdId == eErrorZone)
			{
				Posd[eErrorZone] = tmpOsd;
			}

		}
	}
}




void DrawLine(Mat frame, int startx, int starty, int endx, int endy, int width, UInt32 colorRGBA)
{
	char R, G, B, A;
	uchar *p;

	R = 0;
	G = 0;
	B = 0;
	A = 255;

	return ;
}
//RED
void DrawHLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA)
{
	int endx, endy;
	char R, G, B, A;
	uchar *p;

	GETRGBA(R, G, B, A, colorRGBA);
	endx = startx+len;
	endy = starty+width;

	for(int i=starty;i<endy;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}

	return ;
}

void DrawVLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA)
{
	int endx, endy;
	char R, G, B, A;
	uchar *p;

	GETRGBA(R, G, B, A, colorRGBA);
	endx = startx+width;
	endy = starty+len;

	for(int i=starty;i<endy;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}

	return ;
}

void DrawChar(Mat frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor)
{

}

void DrawString(Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor)
{
	uchar *pin, *pChar, *fontData;
	int numchar = strlen(pString);
	UInt32 i,j,k,pixcolor,index, offset;
	int fontWidth, fontHeight;
	unsigned char data;
	bool FontFind = 0;
	int add=0;

	int number=0;
	char *tmpt;
	int lenctl=0;
	static bool lastnumflag = 0;
	static bool lastmhflag = 0;
	static bool lastcharacterflag = 0;
	static bool lastfhflag = 0;
	static bool lastxhxflag = 0;
	static bool lastxsdflag = 0;


	fontWidth 	= 	OSDUTIL_FONT_FLR_DEFAULT_WIDTH_0814;
	fontHeight 	= 	OSDUTIL_FONT_FLR_DEFAULT_HEIGHT_0814;
	fontData         =	FONT_LIBRARY_0814;
	if(fontWidth%8!=0)
	{
			add=1;
	}
	else
	{
			add=0;
	}
	for(i=0; i<fontHeight; i++)
	{
		pin = frame.ptr<uchar>(starty+i);
		lenctl = 0;
		
		for(k=0; k<numchar; k++)
		{
			index = (UInt32)pString[k];			
			
			//index=index-' ';
			//index=128+index;
			//printf("draw str:%c [%d]\n", pString[k], index);
			//if(index < 177)
			if(index >=48 && index <=57)
			{
				if(lastmhflag | lastnumflag | lastcharacterflag |lastxhxflag|lastfhflag | lastxsdflag)
				{
					lenctl +=48;
				}	
				
				lastnumflag = 1;
				lastmhflag   = 0;
				lastcharacterflag = 0;
				lastfhflag = 0;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}
			else if(index == 46) //.
			{	
				lenctl+=48;

				lastxsdflag = 1;
				lastmhflag   = 0;
				lastnumflag = 0;
				lastcharacterflag = 0;
				lastfhflag = 0;
				lastxhxflag = 0;
			}
			else if(index == 58)  //:
			{
				if(lastcharacterflag)
					lenctl += 48;
				if(lastnumflag)
					lenctl += 36;
				
				lastmhflag   = 1;
				lastnumflag = 0;
				lastcharacterflag = 0;
				lastfhflag = 0;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}
			else if(index == 95)
			{
				lastxhxflag = 1;
				lastmhflag   = 0;
				lastnumflag = 0;
				lastcharacterflag = 0;
				lastfhflag = 0;
				lastxsdflag = 0;
			}
			else if((index>=33 && index <=47) | (index>58 && index <=64))
			{
				if(lastmhflag)
					lenctl +=60;
				else if(lastcharacterflag)
					lenctl +=48;
				else if(lastnumflag)
					lenctl +=36;
				else if(lastfhflag)
					lenctl += 36;

		
				lastmhflag = 0;
				lastnumflag = 0;
				lastcharacterflag = 0;
				lastfhflag = 1;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}
			else if(index>=65 && index<=90)
			{
				if(lastnumflag)
					lenctl +=48;
				else if(lastcharacterflag)
					lenctl +=48;  
				else if(lastfhflag)
					lenctl +=24;
				else if(lastmhflag)
					lenctl +=48;
				else if(lastxhxflag)
					lenctl += 24;
				
				lastmhflag = 0;
				lastnumflag = 0;
				lastcharacterflag = 1;
				lastfhflag = 0;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}
			else if(index >=97 && index <122)
			{
				if(lastnumflag)
					lenctl +=48;
				else if(lastcharacterflag)
					lenctl +=48;  
				else if(lastfhflag)
					lenctl +=24;
				else if(lastmhflag)
					lenctl +=48;
				
				lastmhflag = 0;
				lastnumflag = 0;
				lastcharacterflag = 1;
				lastfhflag = 0;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}
			else
			{
				if(lastnumflag |lastfhflag |lastmhflag)
				{
					lenctl +=48;
				}
					
				lastmhflag = 0;
				lastnumflag = 0;
				lastcharacterflag = 0;
				lastfhflag = 0;
				lastxhxflag = 0;
				lastxsdflag = 0;
			}

			
			if(1)
			{
				pChar = &fontData[i*(fontWidth/8+add)+index*(fontWidth/8+add)*fontHeight];
				for(j=startx+k*fontWidth; j<startx+k*fontWidth+fontWidth; j++)
				{
								
					offset 	= j-startx-k*fontWidth;
					data 	= *(pChar + offset/8);
					data 	<<= (offset%8);

					if(frcolor == 0)
						pixcolor 		= (data&0x80)?frcolor:bgcolor;
					else						
						pixcolor		= (data&0x80)?frcolor:0xff000000;//0x50000000;

					if(i == 0 && k == numchar-1)
					{
						pixcolor	= 0x000000000;
					}

					if(k == numchar-1)
					{
						lastmhflag = 0;
						lastnumflag = 0;
						lastcharacterflag = 0;
						lastfhflag = 0;
					}
					
					//pixcolor		= (data&0x80)?frcolor:bgcolor;
					if(	k == numchar-1 && 
						((index >=48  && index <=57) |
						(index >=33  && index <=47) | 
						//(index >=58  && index <=64) |
						index == 58 |
						(index >=65  && index <=90) |
						(index >=97 && index <122))
					  )
					{
					  	if(j<startx+k*fontWidth+fontWidth-12)
					  	{
					  		*(pin+j*4+0-lenctl)	= pixcolor & 0xFF;
							*(pin+j*4+1-lenctl)	= (pixcolor >> 8) & 0xFF;
							*(pin+j*4+2-lenctl)	= (pixcolor >> 16) & 0xFF;
							*(pin+j*4+3-lenctl)	= (pixcolor >> 24) & 0xFF;
					  	}	
					}	
					else
					{	
						*(pin+j*4+0-lenctl)	= pixcolor & 0xFF;
						*(pin+j*4+1-lenctl)	= (pixcolor >> 8) & 0xFF;
						*(pin+j*4+2-lenctl)	= (pixcolor >> 16) & 0xFF;
						*(pin+j*4+3-lenctl)	= (pixcolor >> 24) & 0xFF;
					}
				}
				
			}
			
		#if 0
			if(	k == numchar-1 && 
				((index >=48  && index <=57) |
				(index >=33  && index <=47) | 
				//(index >=58  && index <=64) |
				index == 58 |
				(index >=65  && index <=90) |
				(index >=97 && index <122))
			  )
			{
				pixcolor	= 0x00000000;
				for(j=startx+k*fontWidth+fontWidth-12; j<startx+k*fontWidth+fontWidth; j++)
				{
					*(pin+j*4+0-lenctl)	= pixcolor & 0xFF;
					*(pin+j*4+1-lenctl)	= (pixcolor >> 8) & 0xFF;
					*(pin+j*4+2-lenctl)	= (pixcolor >> 16) & 0xFF;
					*(pin+j*4+3-lenctl)	= (pixcolor >> 24) & 0xFF;						
				}
			}
		#endif
			#if 0
				pChar = &fontData[i*(fontWidth/8+add)+index*(fontWidth/8+add)*fontHeight];
				for(j=startx+k*fontWidth; j<startx+k*fontWidth+fontWidth; j++)
				{
					offset 	= j-startx-k*fontWidth;
					data 	= *(pChar + offset/8);
					data 	<<= (offset%8);

					pixcolor		= (data&0x80)?frcolor:bgcolor;
					*(pin+j*4)		= pixcolor & 0xFF;
					*(pin+j*4+1)	= (pixcolor >> 8) & 0xFF;
					*(pin+j*4+2)	= (pixcolor >> 16) & 0xFF;
					*(pin+j*4+3)	= (pixcolor >> 24) & 0xFF;
				}
			#endif
		
		}
	}

}

void osd_draw_cross(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;

	


}

void osd_draw_rect(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
//return ;
	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

	//up Hline
	iX = pObj->x-(pObj->width>>1);
	iY = pObj->y-(pObj->height>>1);
	len = pObj->width;
	DrawHLine(frame,iX,iY,width,len,iColor);

	//right Vline
	iX += pObj->width-pObj->linePixels;
	len = pObj->height;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//left Vline
	iX -= pObj->width-pObj->linePixels;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//bottom Hline
	iY += pObj->height-width;
	len = pObj->width;
	DrawHLine(frame,iX,iY,width,len,iColor);

	return ;
}

void osd_draw_rect_gap(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
//return ;
	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

	//left top line
	iX = pObj->x-pObj->width/2;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x-pObj->width/2;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//right top line
	iX = pObj->x+pObj->width/2-pObj->lineGapWidth;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x+pObj->width/2-width;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//right bottom line
	iX = pObj->x+pObj->width/2-pObj->lineGapWidth;
	iY = pObj->y+pObj->height/2-width;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x+pObj->width/2-width;
	iY = pObj->y+pObj->height/2-pObj->lineGapHeight;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//left bottom line
	iX = pObj->x-pObj->width/2;
	iY = pObj->y+pObj->height/2-width;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x-pObj->width/2;
	iY = pObj->y+pObj->height/2-pObj->lineGapHeight;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	return ;
}


void osd_chtext(Mat frame, int startx, int starty, char * pString, UInt32 frcolor, UInt32 bgcolor)
{

	DrawString(frame, startx, starty, pString, frcolor, bgcolor);
	return ;	
}




void osd_draw_text(Mat frame, void *prm)
{
	Text_Param_fb * pObj = (Text_Param_fb *)prm;
	UInt32 fontfrColor,fontbgColor;
return ;
	//if(pObj == NULL)
	//	return ;
printf("111111\n");
Scalar color = Scalar(255,255,255,255);	
fontfrColor =WHITECOLOR;
fontbgColor =BGCOLOR; 
//process_draw_line(frame,100,100,200,2,255,255,255,255);
//return ;
char WorkOsd[10] ;
sprintf(WorkOsd,"%c%c",249,185);
DrawString(frame, 100, 100, WorkOsd, fontfrColor, fontbgColor);
putText(frame,"haha",cvPoint(30,30),CV_FONT_HERSHEY_SIMPLEX,0.8,color);
return ;
	int i;
	int x[3];
	int y[3];
	int vaild[3];
	char textStr[3][128];
	int textLenPrev[3];

	for (i = 0; i < 3; i++)
	{
		x[i] = pObj->text_x[i];
		y[i] = pObj->text_y[i];
	}
	vaild[0] = pObj->text_valid&0x0001;
	vaild[1] = (pObj->text_valid >> 1)&0x0001;
	vaild[2] = (pObj->text_valid >> 2)&0x0001;
	textLenPrev[0] = 0;
	textLenPrev[1] = pObj->textLen[0];
	textLenPrev[2] = pObj->textLen[0] + pObj->textLen[1];
	OSA_assert( textLenPrev[0] < 128 && textLenPrev[1] < 128 && textLenPrev[2] < 128 );

	memcpy(textStr[0], pObj->textAddr, pObj->textLen[0]);
	textStr[0][pObj->textLen[0]] = '\0';
	memcpy(textStr[1], pObj->textAddr + textLenPrev[1], pObj->textLen[1]);
	textStr[1][pObj->textLen[1]] = '\0';
	memcpy(textStr[2], pObj->textAddr + textLenPrev[2], pObj->textLen[2]);
	textStr[2][pObj->textLen[2]] = '\0';
	for (i = 0; i < 3; i++)
	{
		if(vaild[i] != 0)
		{
			fontfrColor = WHITECOLOR;//pObj->frcolor;
			fontbgColor = BGCOLOR;//pObj->bgcolor;
		}
		else
		{
			fontfrColor = WHITECOLOR;//pObj->bgcolor;
			fontbgColor =BGCOLOR;// pObj->bgcolor;
		}
		//DrawString(frame, x[i], y[i], "123", fontfrColor, fontbgColor);
		
		
		//DrawString(frame, x[i], y[i], textStr[i], fontfrColor, fontbgColor);

		
		//printf("x[%d]=%d,y[%d]=%d,testStr[%d]=%s\n",i,x[i],i,y[i],i,textStr[i]);
		//DrawString(frame, x[i], y[i], textStr[i], iColor);
	}
	
	
	return ;


	#if 0
	Point start,end;
	start.x=100;
	start.y=100;
	end.x=300;
	end.y=300;
	char numbuf[200];
	Scalar color = Scalar(255,255,255,255);	
	//if(pObj == NULL)
		//return ;
	fontfrColor =WHITECOLOR;
	fontbgColor =BGCOLOR; 
	//process_draw_line(frame,100,100,200,50,0,0,255,255);

	//Scalar color = Scalar(255,255,255);	
	//line(frame,100,200,color,1,LINE_AA);
	//DrawString(frame, 100, 200, "ABC", fontfrColor, fontbgColor);
	//line(frame, start, end, color, 1, 8, 0 ); 
	//putText(frame,text,Point(200,200),CV_FONT_HERSHEY_COMPLEX,1.0,Scalar(122,255,255),3,8);
	//sprintf(numbuf,"%s","auto");
	//putText(frame,numbuf,cvPoint(30,30),CV_FONT_HERSHEY_SIMPLEX,0.8,color);
	
	
	//DrawString(frame, 100, 200, "ABC", fontfrColor, fontbgColor);
	
	//DrawString(frame, x[i], y[i], textStr[i], fontfrColor, fontbgColor);
	return ;
	#endif

}


void osd_draw_cross_black_white(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
//return ;
	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

        //left horizonal line
        len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
        iX = pObj->x - (pObj->width>>1);
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,BLACKCOLOR);
        DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //middle horizonal line
        len = pObj->linePixels;
        iX = pObj->x;
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,WHITECOLOR);
	 DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //right horizonal line
        len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
        iX = pObj->x+(pObj->lineGapWidth>>1)+width;		// ?
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,BLACKCOLOR);
        DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //top vertical line
        height = pObj->height;
        len = (height>>1)-(pObj->lineGapHeight>>1);
        iX = pObj->x;
        iY = pObj->y - (height>>1);
        DrawVLine(frame,iX,iY,width,len,WHITECOLOR);
        DrawVLine(frame,iX+width,iY,width,len,BLACKCOLOR);
        //bottom vertical line
        height = pObj->height;
        len = (height>>1)-(pObj->lineGapHeight>>1);
        iX = pObj->x;
        iY = pObj->y+(pObj->lineGapHeight>>1)+width;	// ?
        DrawVLine(frame,iX,iY,width,len,WHITECOLOR);
        DrawVLine(frame,iX+width,iY,width,len,BLACKCOLOR);

	return ;
}






int osd_draw_Hori_Menu(Mat frame, void *prm)
{
	int color = 0;
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
	//pObj->linePixels=pObj->linePixels*2;//TEST BLINK;
	//pObj->height=pObj->height+20;
	//pObj->y=65*2;//TEST BLINK;
	//pObj->x=pObj->x+9;
	//pObj->y=pObj->y-2;
//return 0;
	iColor=pObj->frcolor;
	iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->lineGapWidth;
	//printf("the x=%d y=%d \n",iX,iY);
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->lineGapWidth;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	// return 0;

	
	 iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width*2;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width*3;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);
	 
	 iX=pObj->x+pObj->width*4;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

 	iX=pObj->x+pObj->width*5-2;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	




    return 0;
}

int osd_draw_Vert_Menu(Mat frame, void *prm)
{
	int color = 0;

	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
	//pParamFb->color = 0x00EEEEEE;
//return 0;
	//printf(" %s the x =%d   the y=%d  the width=%d  the len=%d\n",__func__, pObj->x,pObj->y,pObj->linePixels,pObj->width);

/*
-----|
-----|
-----|
-----|
-----|
*/

	iColor=pObj->frcolor;
	iX=pObj->x-pObj->width;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	
	
	iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);


	 iX=pObj->x;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*2;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height*2;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*3;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height*3;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*4-2;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

#if 0
	 int addw=100;
	 int addlen=400;
	 DrawHLine(frame,1280/2-addlen,1024/2-addlen,2,addw,iColor);
	 DrawHLine(frame,1280/2+addlen-addw,1024/2-addlen,2,addw,iColor);
	 DrawHLine(frame,1280/2-addlen,1024/2+addlen,2,addw,iColor);
	 DrawHLine(frame,1280/2+addlen-addw,1024/2+addlen,2,addw,iColor);

	 DrawVLine(frame,1280/2-addlen,1024/2-addlen,2,addw,iColor);	
	 DrawVLine(frame,1280/2+addlen,1024/2-addlen,2,addw,iColor);	
	 DrawVLine(frame,1280/2-addlen,1024/2+addlen-addw,2,addw,iColor);	
	 DrawVLine(frame,1280/2+addlen,1024/2+addlen-addw,2,addw,iColor);	
	 
#endif

    return 0;
}





int process_draw_line(Mat frame,int startx,int starty,int endx,int linewidth,char R,char G,char B,char A)
{
	uchar *p;
	for(int i=starty;i<starty+linewidth;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}
	return 0;
}

int process_draw_text(Mat frame,int startx,int starty,char *pstring,int frcolor,int bgcolor)
{

	uchar *q,*pin,c;
	q=FONT_LIBRARY_0814;
	int numchar=strlen(pstring);
	int i,j,k,pixcolor,data,index;
	for(i=starty;i<starty+14;i++)
	{
		pin=frame.ptr<uchar>(i);
		for(k=0;k<numchar;k++)
		{
			c=pstring[k];
			index=c-' ';
			data=q[i-starty+index*14];
			for(j=startx+k*8;j<startx+k*8+8;j++)
			{
				pixcolor=(data&0x80)?frcolor:bgcolor;
				*(pin+j*4)=pixcolor & 0xFF;
				*(pin+j*4+1)=(pixcolor >> 8) & 0xFF;
				*(pin+j*4+2)=(pixcolor >> 16) & 0xFF;
				*(pin+j*4+3)=(pixcolor >> 24) & 0xFF;
				data<<=1;
			}
		}
	}




	return 0;
}

static void* MultichGrpx_task(void *pPrm)
{
    int status = 0;
    int chId = 0;
    osdprocess_CB fun=(osdprocess_CB )pPrm;
    OSA_printf(" %s task start!!! \r\n", __func__);
    Multich_graphic *pMultGraphicObj = &grpxChWinPrms;
    
    while (pMultGraphicObj->tskGraphicLoop == TRUE)
    {
        status = OSA_semWait(&pMultGraphicObj->tskGraphicSem, OSA_TIMEOUT_FOREVER);

        if (pMultGraphicObj->tskGraphicLoop == FALSE)
            break;

       OSA_mutexLock(&pMultGraphicObj->muxLock);
	//OSA_printf(" %s task start!!! \r\n", __func__);
	fun(NULL);
	//for(chId = 0; chId < pMultGraphicObj->devFb_num; chId++)
	//{
		//MultichGrpx_draw_chwins(&pMultGraphicObj->chParam[chId]);
	//}

        OSA_mutexUnlock(&pMultGraphicObj->muxLock);
    }

    pMultGraphicObj->tskGraphicStopDone = TRUE;

    OSA_printf(" %s task exit!!! \r\n", __func__);

    return NULL;
}


#if 1
void  osdgraph_init(osdprocess_CB fun,Mat frame)
{
	int status=0;
	int i, devId, winId;
	Text_Param_fb * textParam = NULL;
	Line_Param_fb * lineParam = NULL;
	memset(&grpxChWinPrms, 0, sizeof(Multich_graphic));

	

#if PROJ_XGS026
	grpxChWinPrms.devFb_num = 1;
	for ( devId = 0; devId < grpxChWinPrms.devFb_num; devId++)
	{
		grpxChWinPrms.chParam[devId].chId = devId;
		grpxChWinPrms.chParam[devId].chEnable = TRUE;
		grpxChWinPrms.chParam[devId].numWindows = WINID_MAX/2;

		for(i = WINID_TV_CROSSAXIS; i < WINID_MAX; i++)
		{
			winId = i/2;
			textParam = &grpxChWinPrms.chParam[devId].winPrms[winId];
			lineParam = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms[winId];
			
			if(winId == WINID_TV_CROSSAXIS)
			{
				lineParam->enableWin = 1;
				lineParam->objType = grpx_ObjId_Cross;
				//lineParam->objType = grpx_ObjId_Cross_Black_White;
				lineParam->frcolor = WHITECOLOR;
				lineParam->x = 
					vdisWH[devId][0]/2;
				lineParam->y = vdisWH[devId][1]/2;
				//lineParam->x = vdisWH[0][0];
				//lineParam->y = vdisWH[0][1];
				lineParam->width = 60;
				lineParam->height = 40;
				lineParam->linePixels = 2;
				lineParam->lineGapWidth = lineParam->width/4;
				lineParam->lineGapHeight = lineParam->height/4;
			}

			/*if(winId == GRPX_WINID_TV_FOVAREA)
			{
				lineParam->enableWin = 1;
				lineParam->objType = grpx_ObjId_Rect_gap;
				lineParam->frcolor = WHITECOLOR;
				lineParam->x = vdisWH[devId][0]/2;
				lineParam->y = vdisWH[devId][1]/2;
				lineParam->width = 300;
				lineParam->height = 200;
				lineParam->linePixels = 2;
				lineParam->lineGapWidth = lineParam->width/6;
				lineParam->lineGapHeight = lineParam->height/4;
			}*/

			if(winId == WINID_TV_AIMAREA)
			{
				lineParam->enableWin = 0;
				lineParam->objType = grpx_ObjId_Rect;
				//lineParam->objType = grpx_ObjId_Rect_gap;
				lineParam->frcolor = WHITECOLOR;
				lineParam->x = vdisWH[devId][0]/2;
				lineParam->y = vdisWH[devId][1]/2;
				lineParam->width = 60;
				lineParam->height = 40;
				lineParam->linePixels = 12;
				lineParam->lineGapWidth = lineParam->width;
				lineParam->lineGapHeight = lineParam->height;
			}

			/*if(winId == GRPX_WINID_TV_WORK_FOV_SEN_ALG)
			{
				textParam->enableWin = 0;
				textParam->objType = grpx_ObjId_Text;
				textParam->frcolor = RED;
				textParam->bgcolor = BGCOLOR;
				textParam->text_x[0] = 100;
				textParam->text_y[0] = 200;
				textParam->textLen[0] = 2;
				textParam->text_valid = 7;
				sprintf((char*)(textParam->textAddr+textParam->textLen[0]+1),"TV");
				textParam->text_x[1] = 120;
				textParam->text_y[1] = 220;
				textParam->textLen[1] = 4;
				sprintf((char*)(textParam->textAddr+textParam->textLen[1]+1),"TVFR");
				textParam->text_x[2] = 160;
				textParam->text_y[2] = 260;
				textParam->textLen[2] = 3;
				sprintf((char*)textParam->textAddr,"TRr");
			}*/
		}
	}
#endif


    grpxChWinPrms.tskGraphicLoop = TRUE;
    grpxChWinPrms.tskGraphicStopDone = FALSE;

    status = OSA_semCreate(&grpxChWinPrms.tskGraphicSem, 1, 0);
    OSA_assert(status == OSA_SOK);

    status = OSA_thrCreate(
                 &grpxChWinPrms.tskGraphicHndl,
                 MultichGrpx_task,
                 OSA_THR_PRI_DEFAULT,
                 0,
                (void *)fun
             );



    OSA_assert(status == OSA_SOK);

    grpxChWinPrms.bGraphicInit = TRUE;

    OSA_printf(" %s done.\n", __func__);

}
#endif


int timesta=0;
static int timecount=0;
void  set_graph()
{
	timesta=1;
}
void MultichGrpx_update_sem_post(void)
{

    //int timecount=5;
    if(timesta==0)
    	{
		return ;
    	}
    Multich_graphic *pMultGraphicObj = &grpxChWinPrms;

    if (!pMultGraphicObj->bGraphicInit)
        return ;

	if(timecount==0)
	{
		  OSA_semSignal(&pMultGraphicObj->tskGraphicSem);
		   //printf("!!!!!!!!!!!!%s,line : %d\n",__func__,__LINE__);
	}
	else
			timecount++;
	if(timecount>=1)
		timecount=0;

}

static int APP_get_Rgb_Color(int colorId)
{
	int iRtnColor;

	if(colorId == ecolor_White)
		iRtnColor = 0xFEFEFE;
	else if(colorId == ecolor_Black)
		iRtnColor = 0x000000;
	else if(colorId==ecolor_Red)
		iRtnColor = 0xFE0000;
	else if(colorId==ecolor_Yellow)
		iRtnColor = 0xFEFE00;
	else if(colorId==ecolor_Blue)
		iRtnColor = 0x0000FE;
	else if(colorId==ecolor_Green)
		iRtnColor = 0x00FE00;
	else
		iRtnColor = 0xFEFEFE;	// ecolor_Default is white
	return iRtnColor;
}

#if 1
Int32 APP_set_graphic_parms_fb( Int32 blkId, Int32 fieldId,Multich_graphic *pMultGrap,int winid)
{
    int configId   = CFGID_BUILD(blkId, 2);
    int graph_id   = GET_GRAPHIC_ID(gDXD_info.sysConfig[configId]);
    int windows_id = GET_WINDOWS_ID(gDXD_info.sysConfig[configId]);
    int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);

    int user_draw_id = 0;
	
    Text_Param_fb * textLinkParam = NULL;

    int disMask[eSen_Max];
    disMask[eSen_TV] = gDXD_info.sysConfig[CFGID_RTS_TV_SEN_GRAPH];
    disMask[eSen_FR] = gDXD_info.sysConfig[CFGID_RTS_FR_SEN_GRAPH];

    int osdlevel[eSen_Max],i = 0;
    osdlevel[eSen_TV] = gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]  % 5;
    osdlevel[eSen_FR] = gDXD_info.sysConfig[CFGID_OC25_FR_OSD_LEVEL]  % 5;

    OSA_assert( sensors_id == eSen_TV || sensors_id == eSen_FR);
    //OSA_assert( windows_id < WINID_MAX / 2);
    OSA_assert( graph_id   >= grpx_ObjId_Cross && graph_id <= grpx_ObjId_Text);

  
   // OSA_printf(" %s blkId %03d sensors_id 0x%04x windows_id 0x%02x graph_id 0x%02x, osd_id 0x%06x",
    //        __func__,blkId,sensors_id,windows_id,graph_id,gDXD_info.sysConfig[configId]);

   windows_id = (blkId - 94)/2;
    OSA_assert( windows_id < ALG_LINK_GRPX_MAX_WINDOWS);

	if(sensors_id)
		return OSA_SOK;
   
     
     textLinkParam = &pMultGrap->chParam[0].winPrms[windows_id];
     OSA_assert(textLinkParam != NULL);
     user_draw_id = (blkId-94) % 2;
	// graph_id=grpx_ObjId_Text;
	//printf("**********************************the graphid=%d\n",graph_id); 
    switch (graph_id)
    {
    	case grpx_ObjId_Text:
        {
		//textLinkParam->enableWin=1;
		textLinkParam->bgcolor=BGCOLOR;
		textLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		textLinkParam->objType = grpx_ObjId_Text;
			
		//printf("*********************************************  valid=%x\n",textLinkParam->text_valid);
	    OSD_PROPERTY_GET(textLinkParam->enableWin,     blkId, 1);
          //  OSD_PROPERTY_GET(textLinkParam->frcolor,      blkId, 3);
            OSD_PROPERTY_GET(textLinkParam->text_valid, blkId, 6);
            OSD_PROPERTY_GET(textLinkParam->text_x[0],  blkId, 7);
            OSD_PROPERTY_GET(textLinkParam->text_y[0],  blkId, 8);
            OSD_PROPERTY_GET(textLinkParam->textLen[0], blkId, 9);
            OSD_PROPERTY_GET(textLinkParam->text_x[1],  blkId, 10);
            OSD_PROPERTY_GET(textLinkParam->text_y[1],  blkId, 11);
            OSD_PROPERTY_GET(textLinkParam->textLen[1], blkId, 12);
            OSD_PROPERTY_GET(textLinkParam->text_x[2],  blkId, 13);
            OSD_PROPERTY_GET(textLinkParam->text_y[2],  blkId, 14);
            OSD_PROPERTY_GET(textLinkParam->textLen[2], blkId, 15);


            APP_text_genContext(sensors_id, windows_id,
                                (char*)textLinkParam->textAddr, SHMEM_LEN,
                                textLinkParam->textLen);

		//printf("textLen[0]=%x textLen[1]=%x textLen[2]=%x\n", textLinkParam->textLen[0],textLinkParam->textLen[1],textLinkParam->textLen[2]);
            CFG_FIELD_FILL(textLinkParam->text_valid, blkId,  6);
            CFG_FIELD_FILL(textLinkParam->textLen[0], blkId,  9);
            CFG_FIELD_FILL(textLinkParam->textLen[1], blkId, 12);
            CFG_FIELD_FILL(textLinkParam->textLen[2], blkId, 15);
#if 0
		if (windows_id%(WINID_GRAPHIC_MAX/2)*2==WINID_TV_AXIS)
{
		textLinkParam->text_valid=7;
		textLinkParam->text_x[0]=30;
		textLinkParam->text_y[0]=50;
	       OSA_printf("blkId %d ,x[0]: %d,y[0] :%d len[0]=%d  value=%d\n",blkId, textLinkParam->text_x[0],textLinkParam->text_y[0],textLinkParam->textLen[0],textLinkParam->text_valid);
		OSA_printf("x[1]: %d,y[1] : %d len[1]=%d value=%d\n", textLinkParam->text_x[1],textLinkParam->text_y[1],textLinkParam->textLen[1],textLinkParam->text_valid);
		OSA_printf("x[2]: %d,y[2] : %d len[2]=%d value=%d\n", textLinkParam->text_x[2],textLinkParam->text_y[2],textLinkParam->textLen[2],textLinkParam->text_valid);
}

	#endif		
			

         //   textLinkParam->frcolor  = APP_get_Rgb_Color(textLinkParam->frcolor);
	 //    textLinkParam->enableWin = (disMask[sensors_id] & eDisp_show_text) ? textLinkParam->enableWin : 0;
     #if 1
	     if(osdlevel[sensors_id] > 0x02)
	     {
		textLinkParam->text_valid= 0;
	     }
	     else if(osdlevel[sensors_id] == 0x02)
	     {
		   if((blkId-94) == (WINID_TV_UPDATE_MENU_0 + sensors_id) ||
		   	    (blkId-94) == (WINID_TV_UPDATE_MENU_1+ sensors_id))
			textLinkParam->text_valid = 0;
	     }	
	#endif
        }
        break;

    default:
        return OSA_EFAIL;
    }
    return OSA_SOK;
}

Int32 APP_get_colour( int sensorid)
{
	// int configId   = CFGID_BUILD(blkId, fieldId);
	// int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);
	int configId ;
	if(sensorid==eSen_TV)
		configId=CFGID_RTS_TV_SEN_COLOR;
	else
		configId=CFGID_RTS_FR_SEN_COLOR;
	
	//if((gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]==3)||(gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]==4))
	//	{
	//		return BGCOLOR;

	//	}
	switch(gDXD_info.sysConfig[configId])
		{

		case 1:
			return BLACKCOLOR;
			break;
		case 2:
			return WHITECOLOR;
			break;
		case 3:
			return RED;
			break;
		case 4:
			return YELLO;
			break;
		case 5:
			return BLUE;
			break;
		case 6:
			return GREEN;
			break;
		default:
			return WHITECOLOR;
			break;




		}



}




Int32 APP_set_graphic_parms_line_fb( Int32 blkId, Int32 fieldId,Multich_graphic *pMultGrap)
{
    int configId   = CFGID_BUILD(blkId, 2);
    int graph_id   = GET_GRAPHIC_ID(gDXD_info.sysConfig[configId]);
    int windows_id = GET_WINDOWS_ID(gDXD_info.sysConfig[configId]);
    int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);
	
    int user_draw_id = 0;
	
    Line_Param_fb * lineLinkParam = NULL;

    int disMask[eSen_Max];
    disMask[eSen_TV] = gDXD_info.sysConfig[CFGID_RTS_TV_SEN_GRAPH];
    disMask[eSen_FR] = gDXD_info.sysConfig[CFGID_RTS_FR_SEN_GRAPH];
	
    int osdlevel[eSen_Max],i = 0;
    osdlevel[eSen_TV] = gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]  % 5;
    osdlevel[eSen_FR] = gDXD_info.sysConfig[CFGID_OC25_FR_OSD_LEVEL]  % 5;

    OSA_assert( sensors_id == eSen_TV || sensors_id == eSen_FR);
    OSA_assert( windows_id < WINID_MAX / 2);
    OSA_assert( graph_id   >= grpx_ObjId_Cross && graph_id <= grpx_objId_Vert_Menu);

	if(blkId == 84)
	{
		windows_id = WINID_TEXT_MAX/2 + 0;
	}
	else if(blkId == 86)
	{
		windows_id = WINID_TEXT_MAX/2 + 1;
	}
	else
		return OSA_SOK;
     
    /*OSA_printf(" %s blkId %03d sensors_id 0x%04x windows_id 0x%02x graph_id 0x%02x, osd_id 0x%06x",
            __func__,blkId,sensors_id,windows_id,graph_id,gDXD_info.sysConfig[configId]);*/
     lineLinkParam = (Line_Param_fb*)&pMultGrap->chParam[0].winPrms[windows_id];//lineParamFb[blkId-84];
     OSA_assert(lineLinkParam != NULL);
     user_draw_id = (blkId-84) % 2 ;
	 
    switch (graph_id)
    {
        case grpx_objId_Hori_Menu:
		//printf("***********************************************************grpx_objId_Hori_Menu\n");
		lineLinkParam->bgcolor=BGCOLOR;
		lineLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		lineLinkParam->objType = grpx_objId_Hori_Menu;
		
            OSD_PROPERTY_GET(lineLinkParam->enableWin,       blkId, 1);
          //  OSD_PROPERTY_GET(lineLinkParam->color,        blkId, 3);
            OSD_PROPERTY_GET(lineLinkParam->x,            blkId, 6);
            OSD_PROPERTY_GET(lineLinkParam->y,            blkId, 7);
            OSD_PROPERTY_GET(lineLinkParam->width,        blkId, 8);
            OSD_PROPERTY_GET(lineLinkParam->height,       blkId, 9);
            OSD_PROPERTY_GET(lineLinkParam->linePixels,   blkId, 11);
            OSD_PROPERTY_GET(lineLinkParam->lineGapWidth, blkId, 12);
            OSD_PROPERTY_GET(lineLinkParam->lineGapHeight,blkId, 13);
	         if(osdlevel[sensors_id] > 0x02)
	   {
	       lineLinkParam->frcolor=BGCOLOR;// lineLinkParam->bgcolor;
	   }
	   else if(osdlevel[sensors_id] == 0x02)  
	   {
    	     //if((blkId-84) == (WINID_TV_HORI_MENU +sensors_id))
    	     if(((blkId-84)==0))
    	        lineLinkParam->frcolor= lineLinkParam->bgcolor;
	   }	



			
			break;
        case grpx_objId_Vert_Menu:	
        {


		lineLinkParam->bgcolor=BGCOLOR;
		lineLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		lineLinkParam->objType = grpx_objId_Vert_Menu;
		
            OSD_PROPERTY_GET(lineLinkParam->enableWin,       blkId, 1);
          //  OSD_PROPERTY_GET(lineLinkParam->color,        blkId, 3);
            OSD_PROPERTY_GET(lineLinkParam->x,            blkId, 6);
            OSD_PROPERTY_GET(lineLinkParam->y,            blkId, 7);
            OSD_PROPERTY_GET(lineLinkParam->width,        blkId, 8);
            OSD_PROPERTY_GET(lineLinkParam->height,       blkId, 9);
            OSD_PROPERTY_GET(lineLinkParam->linePixels,   blkId, 11);
            OSD_PROPERTY_GET(lineLinkParam->lineGapWidth, blkId, 12);
            OSD_PROPERTY_GET(lineLinkParam->lineGapHeight,blkId, 13);
			//break;
        	//}

#if 1
           // lineLinkParam->color  = APP_get_Rgb_Color(lineLinkParam->color);
           // lineLinkParam->enableWin = (disMask[sensors_id] &eDisp_show_rect)?lineLinkParam->enableWin:0;

	    if(osdlevel[sensors_id] > 0x02)
	   {
	       lineLinkParam->frcolor=BGCOLOR;// lineLinkParam->bgcolor;
	   }
	   else if(osdlevel[sensors_id] == 0x02)  
	   {
    	     //if((blkId-84) == (WINID_TV_HORI_MENU +sensors_id))
    	     if(((blkId-84)==0))
    	        lineLinkParam->frcolor= lineLinkParam->bgcolor;
	   }	
	   #endif
       }
        break;

    default:
        return OSA_EFAIL;
    }
    return OSA_SOK;
}
/***********************************END**************************************/

#endif



void Draw_graph_osd(Mat frame, void *tParam,void *lParam)
{


		Text_Param_fb * textParam = (Text_Param_fb *)tParam;
		Line_Param_fb * lineParam = (Line_Param_fb *)lParam;
		//if(text)

		switch(textParam->objType)
		switch(4)
		{
		case grpx_ObjId_Cross:
			//osd_draw_cross(frame, lineParam);
			break;
		case grpx_ObjId_Rect:
			//osd_draw_rect(frame, lineParam);
			break;
		case grpx_ObjId_Rect_gap:
			//osd_draw_rect_gap(frame, lineParam);
			break;
		case grpx_ObjId_Compass:
			break;
		case grpx_ObjId_Text:
			osd_draw_text(frame, textParam);
			break;

		// for project
		case grpx_ObjId_Cross_Black_White:
			osd_draw_cross_black_white(frame, lineParam);
			break;
		case grpx_ObjId_Rect_Black_White:
			break;
		case grpx_objId_Hori_Menu:
			osd_draw_Hori_Menu(frame, lineParam);
			break;
		case grpx_objId_Vert_Menu:
			osd_draw_Vert_Menu(frame, lineParam);
			break;

		default:
			break;
		}

}

void EraseDraw_graph_osd(Mat frame, void *Param,void *Parampri)
{

	Text_Param_fb * textParam = (Text_Param_fb *)Param;
	Line_Param_fb * lineParam = (Line_Param_fb *)Param;
	Text_Param_fb * textParampri = (Text_Param_fb *)Parampri;
	Line_Param_fb * lineParampri = (Line_Param_fb *)Parampri;
	int i=0;
	int eraseflag[3];
	memset(eraseflag,0,3*sizeof(int));
	int eraseflag1=0;
	if(textParam->objType==grpx_ObjId_Text)
		{
			for(i=0;i<3;i++)
				{
			if(textParam->text_x[i]!=textParampri->text_x[i])
				{
					eraseflag[i]++;
				}

			if(textParam->text_y[i]!=textParampri->text_y[i])
				{
					eraseflag[i]++;
				}
			//BIT_CLRj(textParampri->text_valid,i);
			//if(eraseflag[i]!=0)
			textParampri->text_valid=0;
			
				}

		}
	else
		{


#if 1
			if(lineParam->x!=lineParampri->x)
				{
					eraseflag1++;
					printf("%s   line=%d  x=%d\n",__func__,__LINE__,lineParam->x);
				}
			if(lineParam->y!=lineParampri->y)
				{
					eraseflag1++;
					printf("%s   line=%d  y=%d\n",__func__,__LINE__,lineParam->y);
				}
			
			if(lineParam->width!=lineParampri->width)
				{
					eraseflag1++;
					printf("%s   line=%d\n",__func__,__LINE__);
				}
			if(lineParam->height!=lineParampri->height)
				{
					eraseflag1++;
					printf("%s   line=%d\n",__func__,__LINE__);
				}
			if(eraseflag1==0)
				{
					//printf("%s   line=%d\n",__func__,__LINE__);
					return ;

				}
#endif
			lineParampri->frcolor=lineParam->bgcolor;
			//printf("%s   line=%d\n",__func__,__LINE__);

		}
	Draw_graph_osd(frame,textParampri,lineParampri);



}
void APP_graphic_timer_alarm()
{

    int i;
    int blkId;

    if(msgextInCtrl==NULL)
		return ;
     CMD_EXT *pIStuts = msgextInCtrl;

	    if(pIStuts->TrkCoastCount)
    {
	pIStuts->TrkCoastCount--;
	if(pIStuts->TrkCoastCount == 0)
	{
		pIStuts->AvtTrkCoast = 0;
		MSGDRIV_send(MSGID_EXT_INPUT_COAST, 0);
	}
    }
    if(pIStuts->FreezeresetCount)
    	{
		pIStuts->FreezeresetCount--;
		if((pIStuts->FreezeresetCount==0)&&(pIStuts->FrCollimation==0x01))
			{

				pIStuts->FrCollimation=0x02;
				
				//pIStuts->PicpSensorStatpri=0x02;
				MSGDRIV_send(MSGID_EXT_INPUT_ENFREZZ, 0);
				app_ctrl_ack();
			}

    	}

	if((pIStuts->unitFaultStatpri&0x03)!=(pIStuts->unitFaultStat&0x03))
		{
			pIStuts->unitFaultStatpri=pIStuts->unitFaultStat;
			MSGDRIV_send(MSGID_EXT_INPUT_VIDEOEN, 0);
			//MSGAPI_AckSnd( AckCheck);
			printf("MSGID_EXT_INPUT_VIDEOEN**********\n");
		}

	
	//printf("!!!!!!!!!!!!!!!!!!!!!!!!unitFaultStatpri=%d  unitFaultStat=%d\n",pIStuts->unitFaultStatpri,pIStuts->unitFaultStat);
	
#if 0
    for(i = WINID_TV_MMT_0; i <= WINID_FR_MMT_9; i++)
    {
        blkId = CFGID_blkId(CFGID_OSD_GRAPH_OBJ_ID(i));

        APP_set_graphic_parms(blkId, 2);
    }
#endif


	
    for(i = WINID_TV_SOFTVER; i < WINID_TEXT_MAX; i++)
    {

        blkId = CFGID_blkId(CFGID_OSD_TEXT_OBJ_ID(i));
	//APP_set_graphic_parms_fb(blkId, 2,&grpxChWinPrms,i);
    }


    for(i = WINID_TV_HORI_MENU; i <= WINID_FR_VERT_MENU; i++)
    {
        blkId = CFGID_blkId(CFGID_OSD_GRAPH_OBJ_ID(i));
	// APP_set_graphic_parms_line_fb(blkId, 2,&grpxChWinPrms);
    }
    MultichGrpx_update_sem_post();


}



void  APP_tracker_timer_alarm()
{

	 if(msgextInCtrl==NULL)
		return ;
        CMD_EXT *pIStuts = msgextInCtrl;
	 if(pIStuts->TrkCoastCount>0)
	 	{
			pIStuts->TrkCoastCount--;
	 	}
	 else
	 	{
			pIStuts->TrkCoastCount=0;
			pIStuts->AvtTrkCoast=0;

	 	}
}


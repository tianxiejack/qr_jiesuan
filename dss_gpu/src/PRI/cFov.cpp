#include "cFov.h"
#include "statCtrl.h"
#include "osd_cv.h"
#include "process021.hpp"

#define xmin(a,b) 		(((a) < (b)) ? (a) : (b))
#define xmax(a,b) 		(((a) > (b)) ? (a) : (b))

static float g_FovRat[2][3]=
{
	{TV_MFOV_ARG/TV_LFOV_ARG,	  TV_NFOV_ARG/TV_MFOV_ARG,		TV_EFOV_ARG/TV_NFOV_ARG},
	{FILR_MFOV_ARG/FILR_LFOV_ARG, FILR_NFOV_ARG/FILR_MFOV_ARG,	FILR_EFOV_ARG/FILR_NFOV_ARG}
};

FOVCTRL_Handle FOVCTRL_create( UINT Sens,UINT fovElem,UINT x,UINT y)
{
	CFOV * pCtrlObj = NULL;

	pCtrlObj = (CFOV *)OSA_memAlloc(sizeof(CFOV));
	if(pCtrlObj!=NULL)
	{
		memset(pCtrlObj,0,sizeof(CFOV));
		pCtrlObj->Interface.pMux		= NULL;
		pCtrlObj->Interface.pCtrlSend	= NULL;
		pCtrlObj->Interface.pCtrlRecv	= NULL;
		pCtrlObj->bFovDraw			= TRUE;
		pCtrlObj->fovScal				= 100;

		pCtrlObj->fovBrush.lineColor	= WHITECOLOR;
		pCtrlObj->fovBrush.cornerXLen	= DEFAULT_FOV_XCONERLEN;
		pCtrlObj->fovBrush.cornerYLen	= DEFAULT_FOV_YCONERLEN;
		pCtrlObj->fovBrush.boresightLen	= DEFAULT_FOV_BORESIGHTLEN;
		pCtrlObj->fovBrush.midLen		= DEFAULT_FOV_MIDLEN;
		pCtrlObj->fovBrush.lineBord		= DEFAULT_WIDTH_PIXLS;

		pCtrlObj->crosswidth			=60;
		pCtrlObj->crossheight			=60;
		pCtrlObj->frcolor				=2;
		pCtrlObj->linew				=1;
		
		FOVCTRL_updateFovRect(pCtrlObj,Sens,fovElem,x,y);
	}

	return pCtrlObj;
}


void FOVCTRL_updateFovRect(HANDLE hFov,char Sens,char fovElem,short x,short y)
{
	CFOV * cthis  = (CFOV * )hFov;
	PRECT  pRect = NULL;
	float ftmp =1.0;
	unsigned int fovWidth =0;
	unsigned int fovHeight=0; 
	
	SDK_ASSERT(cthis!=NULL);

	cthis->fovSens = Sens;
	cthis->fovElem = fovElem;
	cthis->fovX    = x;
	cthis->fovY   	= y;
	cthis->fovW	= 720;
	cthis->fovH	= 576;

	ftmp = (float)cthis->fovScal/100.0;
	fovWidth = (int)(g_FovRat[Sens][fovElem]*FOV_WIDTH);
	fovHeight= (fovWidth*3)>>3;
	pRect = &cthis->fovRect;
	
	pRect->left		= x-(fovWidth>>1);
	pRect->right		= x+(fovWidth>>1);
	pRect->top		= y-(fovHeight>>1);
	pRect->bottom	= y+(fovHeight>>1);

	cthis->fovBrush.rect.top	= xmax(1,(int)((y<<1)-fovHeight*ftmp));
	cthis->fovBrush.rect.bottom = xmin(575,(int)((y<<1)+fovHeight*ftmp));
	cthis->fovBrush.rect.left	= xmax(1,(int)(x-(fovWidth>>1)*ftmp));
	cthis->fovBrush.rect.right	= xmin(703,(int)(x+(fovWidth>>1)*ftmp));
}


void FOVCTRL_destroy(HANDLE hFovCtrl)
{
	if(hFovCtrl!=NULL)
	{
		free(hFovCtrl);
	}
}


void FOVCTRL_updateFovScal(HANDLE hFov,int curScal)
{
	CFOV * cthis  = (CFOV * )hFov;
	SDK_ASSERT(cthis!=NULL);

	cthis->fovScal = curScal;
	FOVCTRL_updateFovRect(cthis,cthis->fovSens,cthis->fovElem,cthis->fovX,cthis->fovY);
}


void FOVCTRL_erase_draw(Mat frame,HANDLE hFov)
{	
	CFOV * cthis = (	CFOV *)hFov;
	if(cthis->bFovDraw==FALSE)
		return;

	cthis->frcolor = 0;
	cthis->linew = 2;

	if(cthis->drawflag & (1<<0))
		DrawjsCross(frame,cthis);	
	if(cthis->drawflag & (1<<1))
		DrawjsRuler(frame,cthis);
	if(cthis->drawflag & (1<<2))
		DrawjsCompass(frame,cthis);
	if(cthis->drawflag & (1<<3))
		DrawjsLeftFrame(frame,cthis);
	if(cthis->drawflag & (1<<4))
		DrawjsRightFrame(frame,cthis);		
	if(cthis->drawflag & (1<<5))
		DrawjsBottomFrame(frame,cthis);
	if(cthis->drawflag & (1<<6))
		DrawjsAlertFrame(frame,cthis);
	if(cthis->drawflag & (1<<7))
		DrawjsAngleFrame(frame,cthis,cthis->last_angle);

	//DrawjsGrenadeLoadOK(frame,cthis);	
	return ;
}


void FOVCTRL_draw(Mat frame,HANDLE hFov)
{
	CFOV * cthis = (	CFOV *)hFov;
	static int shin=0;
	SDK_ASSERT(cthis!=NULL);
	if(cthis->bFovDraw==FALSE)
		return;
	
	cthis->drawflag = 0;
	cthis->last_angle = 0;
 	
	cthis->last_angle = getGrenadeAngle()-getMachGunAngle();
	DrawjsAngleFrame(frame,cthis,cthis->last_angle);
	cthis->drawflag |= 1<<7;

	if(isCalibrationMode() && isBootUpMode())
	{
		//no draw
	}
	else if(isCalibrationMode())//&&(isCalibrationGeneral()||isCalibrationWeather()))
	{
		
		if(isCalibrationZero())
		{
			DrawjsCompass(frame,cthis);
			cthis->drawflag |= 1<<2;
			cthis->linew = 1;
			DrawjsCross(frame, cthis);
			cthis->drawflag |= 1<<0;
			DrawjsRuler(frame,cthis);
			cthis->drawflag |= 1<<1;
						
		}
		if(isCalibrationLaser())
		{
			cthis->linew = 2;
			DrawjsCross(frame, cthis);		
			cthis->drawflag |= 1<<0;
		}
	
	}
	else if(isBattleMode())//&&isStatBattleAlert())
	{
		cthis->linew = 1;
		if(isStatBattleAuto())
		{
			DrawjsCompass(frame,cthis);
			cthis->drawflag |= 1<<2;
			DrawjsCross(frame, cthis);
			cthis->drawflag |= 1<<0;
			DrawjsRuler(frame,cthis);
			cthis->drawflag |= 1<<1;
		}
		else if(isStatBattleAlert())
		{
			
		}
		
	}
	else
	{
		
		
	}
		
	if(/*isFovShine() && */(((shin++)%6)<3) )//yue jie kuang
	{
		if(isBeyondDerection(DERECTION_LEFT))
		{
			DrawjsLeftFrame(frame,cthis);
			cthis->drawflag |= 1<<3;
		}
		else if(isBeyondDerection(DERECTION_RIGHT))
		{
			DrawjsRightFrame(frame,cthis);
			cthis->drawflag |= 1<<4;
		}
		else if(isBeyondDerection(DERECTION_DOWN))
		{
			DrawjsBottomFrame(frame,cthis);
			cthis->drawflag |= 1<<5;
		}
	}	

	//if(isBattleMode()&&isStatBattleAuto()&&(isBattleReady()||isAutoReady())&&(isGrenadeGas()||isGrenadeKill()))
	//	DrawjsGrenadeLoadOK(frame,cthis);
	
	if(isStatBattleAlert() && isAutoCatching())
	{
		static int i=0;
		DrawjsAlertFrame(frame,cthis);
		cthis->drawflag |= 1<<6;
		
		#if 0
			if((i++)%8 == 0)
				FOVCTRL_clearAlertMovingAim(hFov,pImg);
			else
				FOVCTRL_drawAlertMovingAim(hFov,pImg);
		#endif
//		FOVCTRL_drawAimBolder(hFov, pImg, 5,-5,32,32);
	}

	//FOVSYMBOL_drawTrk(handle,pImg);
}


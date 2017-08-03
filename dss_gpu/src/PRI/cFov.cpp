#include "cFov.h"
#include "statCtrl.h"
#include "osd_cv.h"

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
		
		FOVCTRL_updateFovRect(pCtrlObj,Sens,fovElem,x,y);
	}

	return pCtrlObj;
}


void FOVCTRL_updateFovRect(HANDLE hFov,char Sens,char fovElem,short x,short y)
{
#if 1
	CFOV * cthis  = (CFOV * )hFov;
	PRECT  pRect = NULL;
	float ftmp =1.0;
	unsigned int fovWidth =0;
	unsigned int fovHeight=0; 
	
	SDK_ASSERT(cthis!=NULL);

	cthis->fovSens = Sens;
	cthis->fovElem = fovElem;
	cthis->fovX    = x;
	cthis->fovY    = y;
	cthis->fovW	   = 704;
	cthis->fovH	   = 576;

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
#endif	
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
	
	DrawjsCompass(frame,cthis);
	DrawjsCross(frame,cthis);
	DrawjsRuler(frame,cthis);

	return ;
}


void FOVCTRL_draw(Mat frame,HANDLE hFov)
{
#if 1
	CFOV * cthis = (	CFOV *)hFov;
	static int shin=0;
	SDK_ASSERT(cthis!=NULL);
	if(cthis->bFovDraw==FALSE)
		return;
	
#if 0
	
	 DrawjsRuler(frame,&lineParam);

#endif

	if(isCalibrationMode() && isBootUpMode())
	{
		//no draw
	}
	else if(isCalibrationMode())//&&(isCalibrationGeneral()||isCalibrationWeather()))
	{
		if(isCalibrationZero())
		{
			DrawjsCompass(frame,cthis);
			DrawjsCross(frame, cthis);		
		}
	
	}
	else if(isBattleMode())//&&isStatBattleAlert())
	{
		if(isStatBattleAuto())
		{
			DrawjsCompass(frame,cthis);
			DrawjsCross(frame, cthis);
			DrawjsRuler(frame,cthis);
		}
		else if(isStatBattleAlert())
		{
			
		}
		
		//Drawjs_leftFrame(frame,cthis);
		//Drawjs_rightFrame(frame, cthis);
		//Drawjs_bottomFrame(frame, cthis);
	}
	else
	{
		
		
	}
		
	/*	
	if(isFovShine() && (((shin++)%50)<25) )//yue jie kuang
	{
		if(isBeyondDerection(DERECTION_LEFT))
			FOVCTRL_drawLeftFrame(hFov,pImg);
		else if(isBeyondDerection(DERECTION_RIGHT))
			FOVCTRL_drawRightFrame(hFov,pImg);
		else if(isBeyondDerection(DERECTION_DOWN))
			FOVCTRL_drawBottomFrame(hFov,pImg);
	}
	

	if(!(isBattleMode()&&isStatBattleAlert())){
		FOVCTRL_drawRulerFrame(hFov,pImg);//biao chi
		if(isCalibrationMode()&& isCalibrationZero())
			FOVCTRL_drawBoresightZero(hFov,pImg);
		else if((isMeasureManual())||(isBattleMode()&& isStatBattleAuto()&& isDistanceMeasured()))
			FOVCTRL_drawBoresightAim(hFov,pImg);
		else
			FOVCTRL_drawBoresight(hFov,pImg);
	}
	*/
	//if(isBattleMode()&&isStatBattleAuto()&&(isBattleReady()||isAutoReady())&&(isGrenadeGas()||isGrenadeKill()))
	//	FOVCTRL_drawGrenadeLoadOK(hFov,pImg);
	
#if 0
	if(isStatBattleAlert() && isAutoCatching())//\u951f\u7686\u8bb9\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u793a\u951f\u65a4\u62f7\u951f\u65a4\u62f7\u951f?
	{
		static int i=0;
		FOVCTRL_drawAlertFrame(hFov,pImg);
		if((i++)%8 == 0)
			FOVCTRL_clearAlertMovingAim(hFov,pImg);
		else
			FOVCTRL_drawAlertMovingAim(hFov,pImg);
//		FOVCTRL_drawAimBolder(hFov, pImg, 5,-5,32,32);
	/**
		if(cthis->fovElem==eFov_SmlFov_Stat)
		{
			FOVCTRL_drawFovCorner(hFov,pImg);
		}
		else if(cthis->fovElem==eFov_LarFov_Stat||cthis->fovElem==eFov_MidFov_Stat)
		{
			FOVCTRL_drawFovCorner(hFov,pImg);
			FOVCTRL_drawFovMid(hFov,pImg);
		}
		**/
	}
#endif
	//FOVSYMBOL_drawTrk(handle,pImg);
#endif
}


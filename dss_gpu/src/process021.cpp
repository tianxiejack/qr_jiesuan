/*
 * process021.cpp
 *
 *  Created on: May 5, 2017
 *      Author: ubuntu
 */
#include <glut.h>
#include <vector>
#include "process021.hpp"
#include "vmath.h"
//#include "grpFont.h"
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "dx_main.h"
#include "UartMessage.h"
#include "msgDriv.h"
#include"app_ctrl.h"
#include"dx.h"
#include"osd_cv.h"
#include "statCtrl.h"
#include "cFov.h"

#include "servo_control_obj.h"
#include "app_user.h"
//extern Level_one_state gLevel1Mode;
//extern Level_one_state gLevel1LastMode;

#define VIDEO1280X1024 1

using namespace std;
using namespace cv;

//extern OSDCTRL_Handle pOsdCtrlObj;

extern OSDCTRL_Handle pCtrlObj;
OSDCTRL_Handle pCtrlObjbefore = (OSDCTRL_OBJ *)OSA_memAlloc(sizeof(OSDCTRL_OBJ));
FOVCTRL_Handle  pFovCtrlBeforObj = (FOVCTRL_OBJ *)OSA_memAlloc(sizeof(FOVCTRL_OBJ));	

int n=0,ShinId=eCalibGeneral_XPole;

UInt32 interfaceflag;
	
CProcess021 * CProcess021::sThis = NULL;
CProcess021::CProcess021()
{
	memset(&rcTrackBak, 0, sizeof(rcTrackBak));
	memset(tgBak, 0, sizeof(tgBak));
	memset(&blob_rectBak, 0, sizeof(blob_rectBak));
	memset(&extInCtrl, 0, sizeof(CMD_EXT));
	memset(&extOutAck, 0, sizeof(ACK_EXT));
	prisensorstatus=0;//tv
	m_castTm=0;
	m_bCast=false;
	rememflag=false;
	rememtime=0;
	// default cmd value
	CMD_EXT *pIStuts = &extInCtrl;

	pIStuts->unitAxisX[0]      = 360;
	pIStuts->unitAxisY[0]      = 288;
#ifdef VIDEO1280X1024
	pIStuts->unitAxisX[1]      = 640;
	pIStuts->unitAxisY[1]      = 512;
#else
	pIStuts->unitAxisX[1]      = 360;
	pIStuts->unitAxisY[1]      = 288;
#endif


	pIStuts->unitAimW = 64;
	pIStuts->unitAimH = 64;
	pIStuts->unitAimX=360;
	pIStuts->unitAimY=288;
	pIStuts->SensorStat     = eSen_TV;
	pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat = 0xFF;

	pIStuts->DispGrp[0] = 1;//(eDisp_show_rect | eDisp_show_text/* | eDisp_show_dbg*/);
	pIStuts->DispGrp[1] =1;// (eDisp_show_rect | eDisp_show_text/* | eDisp_show_dbg*/);


	pIStuts->DispColor[0]=2;
	pIStuts->DispColor[1]=2;

	extInCtrl.TrkBomenCtrl=1;

	pIStuts->unitWorkMode = NORMAL_MODE; // self init check end
	crossBak.x=extInCtrl.unitAxisX[pIStuts->SensorStat ];
	crossBak.y=extInCtrl.unitAxisY[pIStuts->SensorStat ];
	pIStuts->AvtTrkAimSize=2;

	pIStuts->ImgPixelX[0]=360;
	pIStuts->ImgPixelY[0]=288;

	pIStuts->AvtPosXTv=360;
	pIStuts->AvtPosYTv=288;

	pIStuts->FovStat=1;
#ifdef VIDEO1280X1024
	pIStuts->ImgPixelX[1]=800;
	pIStuts->ImgPixelY[1]=800;

	pIStuts->AvtPosXFir=360;
	pIStuts->AvtPosYFir=288;
#else
	pIStuts->ImgPixelX[1]=360;
	pIStuts->ImgPixelY[1]=288;

	pIStuts->AvtPosXFir=360;
	pIStuts->AvtPosYFir=288;
	
#endif
	pIStuts->FrCollimation=2;
	pIStuts->PicpSensorStatpri=2;
	tvcorx=720-100;
	tvcory=576-100;
	memset(secBak,0,sizeof(secBak));

	memset(Osdflag,0,sizeof(Osdflag));

	
	Mmtsendtime=0;

	

	//for(i=0;i<4;i++)
	//
	rendpos[0].x=vdisWH[0][0]*2/3;
	rendpos[0].y=vdisWH[0][1]*2/3;
	rendpos[0].w=vdisWH[0][0]/3;
	rendpos[0].h=vdisWH[0][1]/3;

	rendpos[1].x=vdisWH[0][0]*2/3;
	rendpos[1].y=0;
	rendpos[1].w=vdisWH[0][0]/3;
	rendpos[1].h=vdisWH[0][1]/3;

	rendpos[2].x=0;
	rendpos[2].y=0;
	rendpos[2].w=vdisWH[0][0]/3;
	rendpos[2].h=vdisWH[0][1]/3;

	rendpos[3].x=0;
	rendpos[3].y=vdisWH[0][1]*2/3;
	rendpos[3].w=vdisWH[0][0]/3;
	rendpos[3].h=vdisWH[0][1]/3;

	msgextInCtrl=&extInCtrl;
	sThis = this;
	
}

CProcess021::~CProcess021()
{
	sThis=NULL;
}

int  CProcess021::WindowstoPiexlx(int x,int channel)
{
	int ret=0;
	ret= cvRound(x*1.0/vdisWH[0][0]*vcapWH[channel][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][0])
	 	{
			ret=vcapWH[channel][0];
	 	}


	  return ret;
}


int  CProcess021::WindowstoPiexly(int y,int channel)
{
	 int ret=0;
	 ret= cvRound(y*1.0/vdisWH[0][1]*vcapWH[channel][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][1])
	 	{
			ret=vcapWH[channel][1];
	 	}
	return  ret;
}



int  CProcess021::PiexltoWindowsx(int x,int channel)
{
	int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][0])
	 	{
			ret=vdisWH[0][0];
	 	}


	  return ret;
}

int  CProcess021::PiexltoWindowsy(int y,int channel)
{
	 int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][1])
	 	{
			ret=vdisWH[0][1];
	 	}
	return  ret;
}

void CProcess021::OnCreate()
{
	osdgraph_init(process_osd,sThis->m_dc);
	
       //pCtrlObj = OSDCTRL_create();
	OSDCTRL_create();
	pFovCtrlObj = FOVCTRL_create(0,0,360,288);  //crossaxis's position initialize
	MSGDRIV_create();
	MSGAPI_initial();
	//App_dxmain();

	MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	
	#if 1
	CMD_EXT *pIStuts = &extInCtrl;

	#if 0
	CFGID_FIELD_GET(pIStuts->unitAxisX ,CFGID_IMAGE_AXIX1);
	CFGID_FIELD_GET(pIStuts->unitAxisY ,CFGID_IMAGE_AXIY1);
	if(pIStuts->unitAxisX<0||pIStuts->unitAxisX>1280)
		pIStuts->unitAxisX      = 640;
	if(pIStuts->unitAxisY<0||pIStuts->unitAxisY>1024)
		pIStuts->unitAxisY      = 512;

	pIStuts->unitAimX=pIStuts->unitAxisX ;
	pIStuts->unitAimY=pIStuts->unitAxisY ;
	m_ImageAxisx=pIStuts->unitAxisX ;
	m_ImageAxisy=pIStuts->unitAxisY;
	#endif

	CFGID_FIELD_GET(pIStuts->DispColor[0] ,CFGID_RTS_TV_SEN_COLOR);
	CFGID_FIELD_GET(pIStuts->DispColor[1] ,CFGID_RTS_FR_SEN_COLOR);
	if(pIStuts->DispColor[0]<0||pIStuts->DispColor[0]>6)
		pIStuts->DispColor[0]=2;

	CFGID_FIELD_GET(pIStuts->PicpPosStat,CFGID_SENSOR_TV_PICP_POS);
	if(pIStuts->PicpPosStat<0||pIStuts->PicpPosStat>3)
		pIStuts->PicpPosStat=0;
	//pIStuts->PicpPosStat=0;
	#endif



	//startSelfCheckTimer();

};

void CProcess021::OnDestroy(){};
void CProcess021::OnInit(){};
void CProcess021::OnConfig(){};
void CProcess021::OnRun(){};
void CProcess021::OnStop(){};
void CProcess021::Ontimer(){

	//msgdriv_event(MSGID_EXT_INPUT_VIDEOEN,NULL);
};
bool CProcess021::OnPreProcess(int chId, Mat &frame)
{
	set_graph();

	
	if(m_bCast){
		Uint32 curTm = OSA_getCurTimeInMsec();
		Uint32 elapse = curTm - m_castTm;
		MSGAPI_AckSnd( AckTrkErr);
		if(elapse < 2000){
			return false;
		}
		else
			{
				m_bCast=false;
			}
	}

	

	return true;
}


int onece=0;
void CProcess021::process_osd(void *pPrm)
{
   
	//int devId=0;
	Mat frame=sThis->m_dc;//.m_imgOsd;
	//D_EXT *pIStuts = &sThis->extInCtrl;
	//int winId;
	//Text_Param_fb * textParam = NULL;
	//Line_Param_fb * lineParam = NULL;
	//Text_Param_fb * textParampri = NULL;
	//Line_Param_fb * lineParampri = NULL;
	static int flag = 0;
	int i ;
	OSDText_Obj * pTextObj = NULL;
	Line_Param_fb lineParam ={0};
	lineParam.x = 720/2;
	lineParam.y = 576/2;
	lineParam.width = 60;
	lineParam.height = 60;
	lineParam.frcolor = 2;


	Osd_cvPoint start,end;
	//erase
	//if(!flag)
		//memcpy(pCtrlObjbefore,pCtrlObj,sizeof(OSDCTRL_OBJ));
	

	if(SHINE)
	{
		n++;
		if(n%3 == 0)
		{
			Osd_shinItem(ShinId);//��˸
			n = 0;
		}
	}



	if(flag)
	{
		OSDCTRL_erase_draw_text(frame,pCtrlObjbefore);
	}
	//OSDCTRL_ItemShow(eErrorZone);

	OSDCTRL_draw_text(frame,pCtrlObj);
	memcpy(pCtrlObjbefore,pCtrlObj,sizeof(OSDCTRL_OBJ));


	//beside the text
	//jiao zhun & ji jian --hide
	if(flag)
	{
		FOVCTRL_erase_draw(frame, pFovCtrlBeforObj);
	}
	FOVCTRL_draw(frame,pFovCtrlObj);
	memcpy(pFovCtrlBeforObj,pFovCtrlObj,sizeof(FOVCTRL_OBJ));
	
	flag = 1;

	sThis->m_display.UpDateOsd(0);
	
	return ;
}




void CProcess021::osd_mtd_show(TARGET tg[], bool bShow)
{
	int i;
	
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	unsigned char Alpha = (bShow) ? frcolor : 0;
	CvScalar colour=GetcvColour(Alpha);

	for(i=0;i<MAX_TARGET_NUMBER;i++)
	{
		if(tg[i].valid)
		{
			cv::Rect result;
			result.width = 32;
			result.height = 32;
			result.x = ((int)tg[i].cur_x) % 1280;
			result.y = ((int)tg[i].cur_y ) % 1024;
			result.x = result.x - result.width/2;
			result.y = result.y - result.height/2;
			rectangle( m_dccv,
				Point( result.x, result.y ),
				Point( result.x+result.width, result.y+result.height),
				colour, 1, 8);
		}
	}
}

void CProcess021::DrawBlob(BlobRect blobRct,  bool bShow /*= true*/)
{
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	unsigned char Alpha = (bShow) ? frcolor : 0;
	CvScalar colour=GetcvColour(Alpha);
	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	int sensor=0;

	if(bShow)
		{
			sensor=extInCtrl.SensorStat;

		}
	else
		{
			sensor=prisensorstatus;

		}

	if(blobRct.bvalid){
//		RotatedRect box = RotatedRect( blobRct.center, blobRct.size, blobRct.angle);
//		ellipse(m_dc, box, cvScalar(0,0,255, Alpha), 4, CV_AA);
	//	ellipse(m_dc, blobRct.center, blobRct.size*0.5f, blobRct.angle, 0, 360, colour, 4/*, CV_AA*/);

		Point pt1,pt2,center;
		center.x = blobRct.center.x;
		center.y = blobRct.center.y;

	 	startx=PiexltoWindowsx(center.x,sensor);
	 	starty=PiexltoWindowsy(center.y,sensor);
	 	//endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
	  	//endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);
		pt1.x=startx-2;pt1.y=starty;
		pt2.x=startx+2;pt2.y=starty;
		line(m_dccv, pt1, pt2, colour, 4, 8, 0 );
		pt1.x=startx;pt1.y=starty-2;
		pt2.x=startx;pt2.y=starty+2;
		line(m_dccv, pt1, pt2, colour, 4, 8, 0 );

		extInCtrl.TempTvCollX=center.x;
		extInCtrl.TempTvCollY=center.y;
		if(m_bBlobDetect)
		MSGAPI_AckSnd( AckTvCollErr);
	}
}


void CProcess021::DrawCross(int x,int y,int fcolour ,bool bShow /*= true*/)
{
	
	unsigned char colour = (bShow) ?fcolour : 0;
	Line_Param_fb lineparm;
	lineparm.x=x;
	lineparm.y=y;
	lineparm.width=50;
	lineparm.height=50;
	lineparm.frcolor=colour;
	
	Drawcvcross(m_dccv,&lineparm);
}



int majormmtid=0;
int primajormmtid=0;

void CProcess021::erassdrawmmt(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);

			tempdata=primajormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[primajormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[primajormmtid].cur_x) % 1280;
							tempmmty=result.y = ((int)tg[primajormmtid].cur_y ) % 1024;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							
							 startx=PiexltoWindowsx(result.x,prisensorstatus);
							 starty=PiexltoWindowsy(result.y,prisensorstatus);
							 endx=PiexltoWindowsx(result.x+result.width,prisensorstatus);
						 	 endy=PiexltoWindowsy(result.y+result.height,prisensorstatus);

							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
							
						}
						
						else if(tg[tempdata].valid)
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[tempdata].cur_x) % 1280;
								tempmmty=result.y = ((int)tg[tempdata].cur_y ) % 1024;

								 startx=PiexltoWindowsx(result.x,prisensorstatus);
								 starty=PiexltoWindowsy(result.y,prisensorstatus);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",(tempdata+MAX_TARGET_NUMBER-primajormmtid)%MAX_TARGET_NUMBER);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								
								
							}
				
				
						tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}


}


#if 1
void CProcess021::drawmmt(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);
			
			//memset(extInCtrl.MtdOffsetXY,0,20);
			for(i=0;i<20;i++)
				{
					extInCtrl.MtdOffsetXY[i]=0;
				}
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(tg[majormmtid].valid==0)
						{
							//majormmtid++;
							majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}
					if(tg[i].valid==1)
						{
							extInCtrl.Mtdtargetnum++;
							

						}

				}

			//OSA_printf("the num detect=%d  majormmtid=%d\n",extInCtrl.Mtdtargetnum,majormmtid);
			primajormmtid=tempdata=majormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[majormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[majormmtid].cur_x) % 1280;
							tempmmty=result.y = ((int)tg[majormmtid].cur_y ) % 1024;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							
							//OSA_printf("the num  majormmtid=%d\n",majormmtid);
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
							 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
							 endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
						 	 endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);

							//OSA_printf("the AvtTrkStat=%d  DispGrp=%d\n",extInCtrl.AvtTrkStat,extInCtrl.DispGrp[extInCtrl.SensorStat]);
							if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=9)&&(extInCtrl.SysMode!=2))
								{
							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
								}
								//OSA_printf("******************the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								extInCtrl.MtdOffsetXY[j]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3]=(tempmmty>>8)&0xff;
								//j++;

								//tempdata=(tempdata+1)%MAX_TARGET_NUMBER;
									
							//memcpy(extInCtrl.MtdOffsetXY,tempmmtx,sizeof(tempmmtx));
							//memcpy(extInCtrl.MtdOffsetXY+2,tempmmty,sizeof(tempmmty));
							
						}
						
						else if(tg[tempdata].valid)
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[tempdata].cur_x) % 1280;
								tempmmty=result.y = ((int)tg[tempdata].cur_y ) % 1024;

								//OSA_printf("+++++++++++++++the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								//result.x = result.x - result.width/2;
								//result.y = result.y - result.height/2;
								//OSA_printf("the num  majormmtid=%d\n",tempdata);

								 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
								 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
									if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=9)&&(extInCtrl.SysMode!=2))
								{
								//DrawCross(result.x,result.y,frcolor,bShow);
								//trkimgcross(frame,result.x,result.y,16);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",(tempdata+MAX_TARGET_NUMBER-majormmtid)%MAX_TARGET_NUMBER);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								}
								//memcpy(extInCtrl.MtdOffsetXY+testid*4,tempmmtx,sizeof(tempmmtx));
								//memcpy(extInCtrl.MtdOffsetXY+2+testid*4,tempmmty,sizeof(tempmmty));
								

								extInCtrl.MtdOffsetXY[j+testid*4]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1+testid*4]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2+testid*4]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3+testid*4]=(tempmmty>>8)&0xff;
								//j++;
								
							}
				
				
						tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}

			

			if(Mmtsendtime==0)
				MSGAPI_AckSnd( AckMtdInfo);
			Mmtsendtime++;
			if(Mmtsendtime==1)
				{
					Mmtsendtime=0;
				}
			


}
#endif


void CProcess021::erassdrawmmtnew(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);

			tempdata=primajormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[primajormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[primajormmtid].cur_x) % 1280;
							tempmmty=result.y = ((int)tg[primajormmtid].cur_y ) % 1024;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							
							 startx=PiexltoWindowsx(result.x,prisensorstatus);
							 starty=PiexltoWindowsy(result.y,prisensorstatus);
							 endx=PiexltoWindowsx(result.x+result.width,prisensorstatus);
						 	 endy=PiexltoWindowsy(result.y+result.height,prisensorstatus);

							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
							
						}
						
					if((tg[i].valid)&&(i!=primajormmtid))
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[i].cur_x) % 1280;
								tempmmty=result.y = ((int)tg[i].cur_y ) % 1024;

								 startx=PiexltoWindowsx(result.x,prisensorstatus);
								 starty=PiexltoWindowsy(result.y,prisensorstatus);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",i+1);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								
								
							}
				
				
						//tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}


}


void CProcess021::drawmmtnew(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);
			
			//memset(extInCtrl.MtdOffsetXY,0,20);
			for(i=0;i<20;i++)
				{
					extInCtrl.MtdOffsetXY[i]=0;
				}
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(tg[majormmtid].valid==0)
						{
							//majormmtid++;
							majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}
					if(tg[i].valid==1)
						{
							extInCtrl.Mtdtargetnum++;
							

						}

				}

			//OSA_printf("the num detect=%d  majormmtid=%d\n",extInCtrl.Mtdtargetnum,majormmtid);
			primajormmtid=tempdata=majormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[majormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[majormmtid].cur_x) % 1280;
							tempmmty=result.y = ((int)tg[majormmtid].cur_y ) % 1024;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							//printf("mtd thex =%d they=%d\n",result.x,result.y);
							//OSA_printf("the num  majormmtid=%d\n",majormmtid);
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
							 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
							 endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
						 	 endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);

							//OSA_printf("the AvtTrkStat=%d  DispGrp=%d\n",extInCtrl.AvtTrkStat,extInCtrl.DispGrp[extInCtrl.SensorStat]);
							if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=8)&&(extInCtrl.SysMode!=2))
								{
							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
								Osdflag[osdindex]=1;
								}
								//OSA_printf("******************the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								extInCtrl.MtdOffsetXY[j]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3]=(tempmmty>>8)&0xff;

								extInCtrl.MtdOffsetXY[j]    =PiexltoWindowsx(extInCtrl.MtdOffsetXY[j],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+1]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+1],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+2]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+2],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+3]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+3],extInCtrl.SensorStat);

								
								//j++;

								//tempdata=(tempdata+1)%MAX_TARGET_NUMBER;
									
							//memcpy(extInCtrl.MtdOffsetXY,tempmmtx,sizeof(tempmmtx));
							//memcpy(extInCtrl.MtdOffsetXY+2,tempmmty,sizeof(tempmmty));
							
						}
						
						if((tg[i].valid)&&(i!=majormmtid))
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[i].cur_x) % 1280;
								tempmmty=result.y = ((int)tg[i].cur_y ) % 1024;

								//OSA_printf("+++++++++++++++the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								//result.x = result.x - result.width/2;
								//result.y = result.y - result.height/2;
								//OSA_printf("the num  majormmtid=%d\n",tempdata);

								 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
								 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
									if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=8)&&(extInCtrl.SysMode!=2))
								{
								//DrawCross(result.x,result.y,frcolor,bShow);
								//trkimgcross(frame,result.x,result.y,16);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",i+1);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								}
								//memcpy(extInCtrl.MtdOffsetXY+testid*4,tempmmtx,sizeof(tempmmtx));
								//memcpy(extInCtrl.MtdOffsetXY+2+testid*4,tempmmty,sizeof(tempmmty));
								

								extInCtrl.MtdOffsetXY[j+testid*4]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1+testid*4]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2+testid*4]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3+testid*4]=(tempmmty>>8)&0xff;

								extInCtrl.MtdOffsetXY[j+testid*4]    =PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+1+testid*4]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+1+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+2+testid*4]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+2+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+3+testid*4]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+3+testid*4],extInCtrl.SensorStat);
								//j++;
								
							}
				
				
					//	tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}

			

			if(Mmtsendtime==0)
				MSGAPI_AckSnd( AckMtdInfo);
			Mmtsendtime++;
			if(Mmtsendtime==1)
				{
					Mmtsendtime=0;
				}
			


}



void CProcess021::DrawMeanuCross(int lenx,int leny,int fcolour , bool bShow )
{
	int templenx=lenx;
	int templeny=leny;
	int lenw=25;
	unsigned char colour = (bShow) ?fcolour : 0;
	int centerx=640;//vdisWH[0][0]/2;
	int centery=512;//vdisWH[0][1]/2;
	Osd_cvPoint start;
	Osd_cvPoint end;

	////v
	
	start.x=centerx-templenx;
	start.y=centery-templeny-100;
	end.x=start.x + lenw;
	end.y=start.y;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx+templenx-lenw;
	start.y=centery-templeny-100;
	end.x=start.x + lenw;
	end.y=start.y;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx - templenx;
	start.y=centery + templeny;
	end.x=start.x + lenw;
	end.y=start.y;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx+templenx-lenw;
	start.y=centery+templeny;
	end.x=start.x+lenw;
	end.y=start.y;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	//h
	start.x=centerx-templenx-100;
	start.y=centery-templeny;
	end.x=start.x;
	end.y=start.y+lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx;
	start.y=centery-templeny;
	end.x=start.x;
	end.y=start.y +lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx-templenx-100;
	start.y=centery+templeny-lenw;
	end.x=start.x;
	end.y=start.y + lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx;
	start.y=centery+templeny-lenw;
	end.x=start.x;
	end.y=start.y + lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);


}

void CProcess021::DrawdashCross(int x,int y,int fcolour ,bool bShow /*= true*/)
{

	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	unsigned char colour = (bShow) ?fcolour : 0;
	Line_Param_fb lineparm;
	startx=PiexltoWindowsx(extInCtrl.ImgPixelX[extInCtrl.SensorStat],extInCtrl.SensorStat);
	starty=PiexltoWindowsy(extInCtrl.ImgPixelY[extInCtrl.SensorStat],extInCtrl.SensorStat);
	lineparm.x=startx;//extInCtrl.ImgPixelX;
	lineparm.y=starty;//extInCtrl.ImgPixelY;
	lineparm.width=50;
	lineparm.height=50;
	lineparm.frcolor=colour;

	int dashlen=2;

	Point start,end;

	// startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
	// starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
	// endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
 	// endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);
	
	//Drawcvcross(m_dc,&lineparm);
	if(!bShow)
	{
			//startx=PiexltoWindowsx(secBak[1].x,extInCtrl.SensorStat);
			//starty=PiexltoWindowsy(secBak[1].y,extInCtrl.SensorStat);
			lineparm.x=secBak[1].x;
			lineparm.y=secBak[1].y;
			DrawcvDashcross(m_dccv,&lineparm,dashlen,dashlen);
			startx=secBak[0].x;//PiexltoWindowsx(secBak[0].x,extInCtrl.SensorStat);
			starty=secBak[0].y;//PiexltoWindowsy(secBak[0].y,extInCtrl.SensorStat);
			endx=secBak[1].x;//PiexltoWindowsx(secBak[1].x,extInCtrl.SensorStat);
				endy=secBak[1].y;//PiexltoWindowsy(secBak[1].y,extInCtrl.SensorStat);
			
			drawdashlinepri(m_dccv,startx,starty,endx,endy,dashlen,dashlen,colour);
	}

	else if((extInCtrl.SecAcqFlag)&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3))
	{
			//printf("ImgPixelX=%d,ImgPixelY=%d  extInCtrl.DispGrp[extInCtrl.SensorStat]=%d \n",extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,extInCtrl.DispGrp[extInCtrl.SensorStat]);
			DrawcvDashcross(m_dccv,&lineparm,dashlen,dashlen);
			startx=PiexltoWindowsx(extInCtrl.unitAxisX[extInCtrl.SensorStat ],extInCtrl.SensorStat);
			starty=PiexltoWindowsy(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);
			endx=lineparm.x;//PiexltoWindowsx(extInCtrl.ImgPixelX[extInCtrl.SensorStat],extInCtrl.SensorStat);
				endy=lineparm.y;//PiexltoWindowsy(extInCtrl.ImgPixelY[extInCtrl.SensorStat],extInCtrl.SensorStat);
			
			drawdashlinepri(m_dccv,startx,starty,endx,endy,dashlen,dashlen,colour);

			secBak[0].x=startx;
			secBak[0].y=starty;
			secBak[1].x=endx;
			secBak[1].y=endy;
			
			Osdflag[osdindex]=1;
			
	      	
			//DrawcvDashliner(m_dc,extInCtrl.unitAxisX,extInCtrl.unitAxisX,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,4,4,colour);
			//drawdashcross(frame,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,50,4,4);
			//drawdashline(frame,extInCtrl.unitAxisX,extInCtrl.unitAxisX,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,4,4);
			
	}
}

bool CProcess021::OnProcess(int chId, Mat &frame)
{









	
	//track
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	int crossshiftx=cvRound(vdisWH[0][0]/3);
	int crossshifty=cvRound(vdisWH[0][1]/3);
	
	CvScalar colour=GetcvColour(2);
//putText(frame,"heolo",Point(200,200),CV_FONT_HERSHEY_COMPLEX,1.0,Scalar(0,0,255),3,8);
	osdindex=0;
	
osdindex++;
	{
		 UTC_RECT_float rcResult = m_rcTrack;
		 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
		 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
		 #if 1
		 startx=rcTrackBak.x;//PiexltoWindowsx(rcTrackBak.x,extInCtrl.SensorStat);
		 starty=rcTrackBak.y;//PiexltoWindowsy(rcTrackBak.y,extInCtrl.SensorStat);
		 endx=rcTrackBak.x+rcTrackBak.width;//PiexltoWindowsx(rcTrackBak.x+rcTrackBak.width,extInCtrl.SensorStat);
		 endy=rcTrackBak.y+rcTrackBak.height;//PiexltoWindowsy(rcTrackBak.y+rcTrackBak.height,extInCtrl.SensorStat);
		rectangle( m_dc,
			Point( startx, starty ),
			Point( endx, endy),
			cvScalar(0,0,0, 0), 1, 8 );
		
		 if(m_bTrack)
		 {
			 startx=PiexltoWindowsx(rcResult.x,extInCtrl.SensorStat);
			 starty=PiexltoWindowsy(rcResult.y,extInCtrl.SensorStat);
			 endx=PiexltoWindowsx(rcResult.x+rcResult.width,extInCtrl.SensorStat);
		 	 endy=PiexltoWindowsy(rcResult.y+rcResult.height,extInCtrl.SensorStat);

			// printf("the x=%d y=%d w=%f h=%f\n",startx,starty,rcResult.width,rcResult.height);
		 	
			if( m_iTrackStat == 1)
				rectangle( m_dc,
					Point( startx, starty ),
					Point( endx, endy),
					colour, 1, 8 );
			else
				rectangle( m_dc,
				Point( startx, starty ),
				Point( endx, endy),
				cvScalar(0,255,0, 255), 1, 8 );
			//rcTrackBak = rcResult;
			rcTrackBak.x=startx;
			rcTrackBak.y=starty;
			rcTrackBak.width=endx-startx;
			rcTrackBak.height=endy-starty;
			extInCtrl.unitAimX=rcResult.x+rcResult.width/2;
			extInCtrl.unitAimY=rcResult.y+rcResult.height/2;
		 }
		 
		 
		// printf("rcResult.x =%f rcResult.y=%f w=%f h=%f\n",rcResult.x,rcResult.y,rcResult.width,rcResult.height);
		 #else
		if(Osdflag[osdindex]==1)
	 			{
				rectangle( m_dccv,
					Point( rcTrackBak.x, rcTrackBak.y ),
					Point( rcTrackBak.x+rcTrackBak.width, rcTrackBak.y+rcTrackBak.height),
					cvScalar(0,0,0, 0), 1, 8 );
				Osdflag[osdindex]=0;
			}
		 if((m_bTrack)&&(extInCtrl.TrkBomenCtrl==1))
		 {


			 startx=PiexltoWindowsx(rcResult.x+rcResult.width/2-aimw/2,extInCtrl.SensorStat);
			
			 starty=PiexltoWindowsy(rcResult.y+rcResult.height/2-aimh/2 ,extInCtrl.SensorStat);
			 endx=PiexltoWindowsx(rcResult.x+rcResult.width/2+aimw/2,extInCtrl.SensorStat);
		 	 endy=PiexltoWindowsy(rcResult.y+rcResult.height/2+aimh/2 ,extInCtrl.SensorStat);
			
			if( m_iTrackStat == 1)
				
				rectangle( m_dccv,
					Point( startx, starty ),
					Point( endx, endy),
					colour, 1, 8 );
			else
				rectangle( m_dccv,
					Point( startx, starty),
					Point( endx, endy),
				colour, 1, 8 );

			Osdflag[osdindex]=1;
			rcTrackBak = rcResult;
			rcTrackBak.x=startx;
			rcTrackBak.y=starty;
			rcTrackBak.width=endx-startx;
			rcTrackBak.height=endy-starty;
			extInCtrl.unitAimX=rcResult.x+rcResult.width/2;
			extInCtrl.unitAimY=rcResult.y+rcResult.height/2;
			//printf("the errx=%d error y=%d\n",extInCtrl.unitAimX,extInCtrl.unitAimY);
		 }
		 
		 if(m_bTrack)
		 	{
		 	extInCtrl.unitTrkStat=m_iTrackStat;
			if(m_iTrackStat == 1)
				{
					rememflag=false;
				}
			else if(m_iTrackStat == 2)
				{
					if(!rememflag)
						{
							rememflag=true;
							rememtime=OSA_getCurTimeInMsec();
						}
					
					if((OSA_getCurTimeInMsec()-rememtime)>5000)
						{							
							extInCtrl.unitTrkStat=3;
						}
					else
						{

							extInCtrl.unitTrkStat=2;
						}
				}
		 	 if((extInCtrl.unitTrkStat == 1)||(extInCtrl.unitTrkStat == 2))
		 	 	{
		 	 		//rememflag=false;
					extInCtrl.trkerrx=extInCtrl.unitTrkX =rcResult.x+rcResult.width/2;
					extInCtrl.trkerry=extInCtrl.unitTrkY = rcResult.y+rcResult.height/2;
					extInCtrl.trkerrx=PiexltoWindowsx(extInCtrl.trkerrx ,extInCtrl.SensorStat);
					extInCtrl.trkerry=PiexltoWindowsy(extInCtrl.trkerry ,extInCtrl.SensorStat);
					
					MSGAPI_AckSnd( AckTrkErr);
		 	 	}
		if(extInCtrl.unitTrkStat!=extInCtrl.unitTrkStatpri)
		{
			extInCtrl.unitTrkStatpri=extInCtrl.unitTrkStat;
			MSGAPI_AckSnd( AckTrkType);
		}

		 	}
		 else
		 	{
				rememflag=false;

		 	}



		 #endif
	}


	return 0;
	//mtd
osdindex++;
	{
		//osd_mtd_show(tgBak, false);
		if(Osdflag[osdindex]==1)
	 			{
					erassdrawmmtnew(tgBak, false);
					Osdflag[osdindex]=0;
				}
		 if(m_bMtd){
			//osd_mtd_show(m_mtd[chId]->tg, true);
			drawmmtnew(m_mtd[chId]->tg, true);
			memcpy(tgBak, m_mtd[chId]->tg, sizeof(TARGET)*MAX_TARGET_NUMBER);
			
		 }

		// drawmmt(chId,false);
	}

osdindex++;
	// blob detect
	{
		if(Osdflag[osdindex]==1)
		{
			DrawBlob(blob_rectBak, false);
			Osdflag[osdindex]=0;
		}
		if(m_bBlobDetect)
		{
			DrawBlob(m_blobRect, true);
			memcpy(&blob_rectBak, &m_blobRect, sizeof(BlobRect));
			Osdflag[osdindex]=1;
		}
	}
osdindex++;
//return true;

	{
		if(Osdflag[osdindex]==1)
			{
				DrawdashCross(0,0,frcolor,false);
				Osdflag[osdindex]=0;
			}
		DrawdashCross(0,0,frcolor,true);
		
	}

	osdindex++;	

	//printf("!!!!!!osdflag[%d] = %d\n",osdindex,Osdflag[osdindex]);	//osdindex =5 ;osdflag[5] =0,1,1,1,1...... ;
	//cross aim
	{
		startx=crossBak.x;//PiexltoWindowsx(crossBak.x,extInCtrl.SensorStat);
	 	starty=crossBak.y;//PiexltoWindowsy(crossBak.y,extInCtrl.SensorStat);
	 	if(Osdflag[osdindex]==1)
		{
			DrawCross(startx,starty,frcolor,false);
			Osdflag[osdindex]=0;
 		}
		startx=PiexltoWindowsx(extInCtrl.unitAxisX[extInCtrl.SensorStat],extInCtrl.SensorStat);
	 	starty=PiexltoWindowsy(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);
		//OSA_printf("unitAxisX=%d  unitAxisY=%d sensor=%d  prix=%d  prix=%d\n",startx,starty,extInCtrl.SensorStat,extInCtrl.unitAxisX[extInCtrl.SensorStat],extInCtrl.unitAxisY[extInCtrl.SensorStat ]);
		//printf("!!!!!!extInCtrl.DispGrp[extInCtrl.SensorStat] = %d\n",extInCtrl.DispGrp[extInCtrl.SensorStat]);	// 1
		if(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)
		{
			DrawCross(startx,starty,frcolor,true);
			Osdflag[osdindex]=1;
		}
		//DrawCross(extInCtrl.unitAxisX,extInCtrl.unitAxisY,1,true);
		crossBak.x=PiexltoWindowsx(extInCtrl.unitAxisX[extInCtrl.SensorStat ],extInCtrl.SensorStat);
		crossBak.y=PiexltoWindowsy(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);
		//OSA_printf("unitAxisX=%d  unitAxisY=%d\n",extInCtrl.unitAxisX,extInCtrl.unitAxisY);
		//memcpy(&crossBak, &m_blobRect, sizeof(BlobRect));
	}

	osdindex++;
	
	{
		if(extInCtrl.FrCollimation==1)
			{
				//printf("!!!!!!!!!!!!!!!2323223232!!!!!!!!!!\n");  no enter
				//CFGID_FIELD_GET(tvcorx ,CFGID_TRACK_TV_AXIX);
				//CFGID_FIELD_GET(tvcory ,CFGID_TRACK_TV_AXIY);
				tvcorx=extInCtrl.CollPosXFir;
				tvcory=extInCtrl.CollPosYFir;
				//printf("fcorpri*******the frcol is x=%d y=%d\n",tvcorx,tvcory);
				tvcorx=tvcorx+crossshiftx;
				tvcory=tvcory-crossshifty;
				if((tvcorx<1280*2/3)||(tvcorx>1280))
					{
						tvcorx=cvRound(1280*5.0/6);
					}
				if((tvcory<0)||(tvcory>1024*1/3))
					{
						tvcory=cvRound(1024*1.0/6);
					}

				startx=tvcorx;
				starty=tvcory;
				//startx=PiexltoWindowsx(tvcorx,extInCtrl.SensorStat);
	 			//starty=PiexltoWindowsy(tvcory,extInCtrl.SensorStat);

				//printf("fcor*******the frcol is x=%d y=%d\n",startx,starty);
				if(Osdflag[osdindex]==1)
					{
						DrawCross(freezecrossBak.x,freezecrossBak.y,frcolor,false);
						Osdflag[osdindex]=0;
					}
				DrawCross(startx,starty,frcolor,true);
				freezecrossBak.x=startx;
				freezecrossBak.y=starty;
				
				Osdflag[osdindex]=1;
				//if((tvcorx+1280/3)
				//DrawCross(1280*5/6,1024*1/6,frcolor,true);
				
			}
		else
			{
				startx=PiexltoWindowsx(tvcorx,extInCtrl.SensorStat);
	 			starty=PiexltoWindowsy(tvcory,extInCtrl.SensorStat);
				if(Osdflag[osdindex]==1)
				{
					DrawCross(freezecrossBak.x,freezecrossBak.y,frcolor,false);
					Osdflag[osdindex]=0;
				}
				//DrawCross(1280*5/6,1024*1/6,frcolor,false);
			}




	}
///fov
	osdindex++;
//  
	if(Osdflag[osdindex]==1)
	{
		DrawMeanuCross(rectfovBak.x,rectfovBak.y,frcolor,false);
		Osdflag[osdindex]=0;
	}
	if(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)
	{
		int fovw=0;
		int fovh=0;
		double ratiox=0.0;
		double ratioy=0.0;
		#if 0
		switch(extInCtrl.FovStat)
			{
			case 1:
				fovw=FOVw[extInCtrl.SensorStat][extInCtrl.FovStat-1][0]/2;
				fovh=FOVw[extInCtrl.SensorStat][extInCtrl.FovStat-1][1]/2;
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			default :
				break;

			}
		#endif
		if(extInCtrl.FovStat<5)
		{
			ratiox=FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat)%5][0]/FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat-1)%5][0];
			ratioy=FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat)%5][1]/FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat-1)%5][1];
			fovw=80;//vdisWH[0][0]/2*ratiox;
			fovh=80;//vdisWH[0][1]/2*ratioy;
			Fovpri[extInCtrl.SensorStat]=extInCtrl.FovStat;
			DrawMeanuCross(fovw,fovh,frcolor,true);
			Osdflag[osdindex]=1;
			rectfovBak.x=fovw;
			rectfovBak.y=fovh;
		}
	}
	
	//process_osd_test(NULL);
	
	
	prisensorstatus=extInCtrl.SensorStat;
	sThis->m_display.UpDateOsd(1);
	return true;
}

static inline void my_rotate(GLfloat result[16], float theta)
{
	float rads = float(theta/180.0f) * CV_PI;
	const float c = cosf(rads);
	const float s = sinf(rads);

	memset(result, 0, sizeof(GLfloat)*16);

	result[0] = c;
	result[1] = -s;
	result[4] = s;
	result[5] = c;
	result[10] = 1.0f;
	result[15] = 1.0f;
}

void CProcess021::OnMouseLeftDwn(int x, int y){};
void CProcess021::OnMouseLeftUp(int x, int y){};
void CProcess021::OnMouseRightDwn(int x, int y){};
void CProcess021::OnMouseRightUp(int x, int y){};

void CProcess021::OnKeyDwn(unsigned char key)
{
	CMD_EXT *pIStuts = &extInCtrl;

	if(key == 'a' || key == 'A')
	{
		pIStuts->SensorStat = (pIStuts->SensorStat + 1)%3;
		//msgdriv_event(MSGID_EXT_INPUT_SENSOR, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_SENSOR,NULL);
	}

	if(key == 'b' || key == 'B')
	{
		pIStuts->PicpSensorStat = (pIStuts->PicpSensorStat + 1) % (eSen_Max+1);
		//msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENPICP,NULL);
	}

	if(key == 'c'|| key == 'C')
	{
		if(pIStuts->AvtTrkStat)
			pIStuts->AvtTrkStat = eTrk_mode_acq;
		else
			pIStuts->AvtTrkStat = eTrk_mode_target;
		//msgdriv_event(MSGID_EXT_INPUT_TRACK, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_TRACK,NULL);
	}

	if(key == 'd'|| key == 'D')
	{
		if(pIStuts->ImgMtdStat[pIStuts->SensorStat])
			pIStuts->ImgMtdStat[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgMtdStat[pIStuts->SensorStat] = eImgAlg_Enable;
		//msgdriv_event(MSGID_EXT_INPUT_ENMTD, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENMTD,NULL);
	}

	if (key == 'e' || key == 'E')
	{
		if(pIStuts->ImgEnhStat[pIStuts->SensorStat])
			pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Enable;
		//msgdriv_event(MSGID_EXT_INPUT_ENENHAN, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENENHAN,NULL);
	}


	
	if (key == 'f' || key == 'F')
	{
		if(pIStuts->ImgFrezzStat[pIStuts->SensorStat])
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Enable;
		
		//msgdriv_event(MSGID_EXT_INPUT_ENFREZZ, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENFREZZ,NULL);
	}
	
	
	
	if (key == 'g')
	{
		MSGDRIV_send(CMD_BUTTON_CALIBRATION,NULL);//	jiao zhun
		//msgdriv_event(MSGID_EXT_INPUT_COAST, NULL);
		//MSGDRIV_send(MSGID_EXT_INPUT_COAST,NULL);
	}

	if (key == 'G')
	{
		MSGDRIV_send(CMD_BUTTON_BATTLE,NULL);
		//msgdriv_event(MSGID_EXT_INPUT_COAST, NULL);
		//MSGDRIV_send(MSGID_EXT_INPUT_COAST,NULL);
	}

	if (key == 'h')
	{
		MSGDRIV_send(CMD_BUTTON_AUTOCHECK, NULL);
	}

	if (key == 'H')
	{
		MSGDRIV_send(CMD_BUTTON_ENTER, NULL);
	}

	if (key == 'i')
	{	
		MSGDRIV_send(CMD_BULLET_SWITCH0, NULL);
	}

	if (key == 'I')
	{
		
		MSGDRIV_send(CMD_BUTTON_UNLOCK, NULL);
	}
		
	if (key == 'j')
	{
		MSGDRIV_send(CMD_MODE_AIM_LAND,NULL);
	}

	if (key == 'J')
	{
		MSGDRIV_send(CMD_MODE_AIM_SKY,NULL);
	}

		
	if (key == 'L')
		MSGDRIV_send(CMD_EXIT_SELF_CHECK,NULL);

	if (key == 'o' || key == 'O')
	{
		if(pIStuts->ImgBlobDetect[pIStuts->SensorStat])
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Enable;
		//msgdriv_event(MSGID_EXT_INPUT_ENBDT, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENBDT,NULL);
	}


	if (key == 'p'|| key == 'P')
	{
		
		pIStuts->PicpPosStat=(pIStuts->PicpPosStat+1)%4;
		//msgdriv_event(MSGID_EXT_INPUT_PICPCROP, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_PICPCROP,NULL);
	}

	if(key == 'Q' || key == 'q')
	{
		gLevel3CalculatorState = Auto_LoadFiringTable;
		if(MachGunAngle.theta>75)
			MachGunAngle.theta = 0;
		MachGunAngle.theta += 18;

		hPositionX+= 50;
		hPositionY+= 60;
		
		loadFiringTable_Enter();
	}

	if(key == 'R' || key == 'r')
	{
		gLevel3CalculatorState = Auto_LoadFiringTable;
		if(MachGunAngle.theta>75)
			MachGunAngle.theta = 0;
		MachGunAngle.theta += 18;

		hPositionX+= 50;
		hPositionY+= 60;
		DistanceManual += 10;
		loadFiringTable();
	}
	

		
	if (key == 't' || key == 'T')
	{
		if(pIStuts->ImgVideoTrans[pIStuts->SensorStat])
			pIStuts->ImgVideoTrans[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgVideoTrans[pIStuts->SensorStat] = eImgAlg_Enable;
		//msgdriv_event(MSGID_EXT_INPUT_RST_THETA, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_RST_THETA,NULL);
	}
	
	

	if (key == 'w')
	{
		MSGDRIV_send(CMD_BUTTON_QUIT, NULL);
	}

	if (key == 'x'|| key == 'X')
	{
		button_to_save();
	}
	
	if (key == 'z'|| key == 'Z')
	{
		
		pIStuts->ImgZoomStat[0]=(pIStuts->ImgZoomStat[0]+1)%2;
		pIStuts->ImgZoomStat[1]=(pIStuts->ImgZoomStat[1]+1)%2;
		//msgdriv_event(MSGID_EXT_INPUT_ENZOOM, NULL);
		MSGDRIV_send(MSGID_EXT_INPUT_ENZOOM,NULL);
	}


	if(key == 101)//up
	{
		MSGDRIV_send(CMD_BUTTON_UP, NULL);
	}

	if(key == 103)//down
	{
		MSGDRIV_send(CMD_BUTTON_DOWN, NULL);
	}		

	if(key == 100)//left
	{
		MSGDRIV_send(CMD_BUTTON_LEFT, NULL);
	}

	if(key == 102)//right
	{
		MSGDRIV_send(CMD_BUTTON_RIGHT, NULL);
	}

	
}


void CProcess021::msgdriv_event(MSG_PROC_ID msgId, void *prm)
{
	int tempvalue=0;
	CMD_EXT *pIStuts = &extInCtrl;
	CMD_EXT *pInCmd = NULL;
printf("*************x=%d y=%d\n",pIStuts->unitAxisX[extInCtrl.SensorStat ],pIStuts->unitAxisY[extInCtrl.SensorStat ]);
	if(msgId == MSGID_EXT_INPUT_SENSOR || msgId == MSGID_EXT_INPUT_ENPICP)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->SensorStat = pInCmd->SensorStat;
			pIStuts->PicpSensorStat = pInCmd->PicpSensorStat;
		}
		int itmp;
		//chage acq;
		m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
		m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
		m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
		m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];

		printf("recv   the rctrack x=%f y=%f w=%f h=%f  sensor=%d picpsensor=%d\n",m_rcAcq.x,m_rcAcq.y,m_rcAcq.width,m_rcAcq.height,pIStuts->SensorStat
			,pIStuts->PicpSensorStat);
		
		itmp = pIStuts->SensorStat;
		dynamic_config(VP_CFG_MainChId, itmp, NULL);

#if 1//change the sensor picp change too
		if(pIStuts->PicpSensorStat==0||pIStuts->PicpSensorStat==1)
			{
				if(pIStuts->SensorStat==0)
					{
						pIStuts->PicpSensorStat=1;
					}
				else
					{
						pIStuts->PicpSensorStat=0;
					}
				if(pIStuts->ImgPicp[pIStuts->SensorStat]==1||pIStuts->ImgPicp[pIStuts->SensorStat^1]==1)
					pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat;
				
				
			}
#endif

		itmp = pIStuts->PicpSensorStat;//freeze change
		dynamic_config(VP_CFG_SubChId, itmp, NULL);
////enhance 

		if(pIStuts->ImgEnhStat[pIStuts->SensorStat^1] ==0x01)
		{
			int ENHStatus=0;
			if(pIStuts->ImgEnhStat[pIStuts->SensorStat] ==0x00)
				{
					
					ENHStatus=pIStuts->ImgEnhStat[pIStuts->SensorStat]=pIStuts->ImgEnhStat[pIStuts->SensorStat^1];
					
					printf("the enhstaus=%d  pIStuts->SensorStat=%d\n",ENHStatus,pIStuts->SensorStat);
					dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
				}
				ENHStatus=0;
				dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat^1, &ENHStatus);
				pIStuts->ImgEnhStat[pIStuts->SensorStat^1]=0;
				
		}
		
//sec track sync
		//if( pIStuts->SecAcqStat=)
		if(pIStuts->SensorStat==0)
			{
				pIStuts->ImgPixelX[pIStuts->SensorStat] =PiexltoWindowsx( pIStuts->ImgPixelX[pIStuts->SensorStat^1],pIStuts->SensorStat^1);
				pIStuts->ImgPixelY[pIStuts->SensorStat] =PiexltoWindowsy( pIStuts->ImgPixelY[pIStuts->SensorStat^1],pIStuts->SensorStat^1);
			}
		else
			{

				pIStuts->ImgPixelX[pIStuts->SensorStat] =WindowstoPiexlx( pIStuts->ImgPixelX[pIStuts->SensorStat^1],pIStuts->SensorStat);
				pIStuts->ImgPixelY[pIStuts->SensorStat] =WindowstoPiexly( pIStuts->ImgPixelY[pIStuts->SensorStat^1],pIStuts->SensorStat);
			}


	

 			//pIStuts->ImgPixelX[pIStuts->SensorStat^1] = pIStuts->ImgPixelX[pIStuts->SensorStat];
			//pIStuts->ImgPixelY[pIStuts->SensorStat^1] = pIStuts->ImgPixelY[pIStuts->SensorStat] ;

//sensor 1 rect

		DS_Rect lay_rect;
	#if 1
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/2-lay_rect.w/2;
		lay_rect.y = vdisWH[0][1]/2-lay_rect.h/2;
	#endif
		if(pIStuts->PicpSensorStat==1)
			{
			#if 1
				lay_rect.w = vcapWH[1][0]/3;
				lay_rect.h = vcapWH[1][1]/3;
				lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
				lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
			#endif

			}
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
//picp position
		lay_rect=rendpos[pIStuts->PicpPosStat];
		m_ImageAxisx=pIStuts->unitAxisX[extInCtrl.SensorStat ];
		m_ImageAxisy=pIStuts->unitAxisY[extInCtrl.SensorStat ];

		//OSA_printf("%s: lay_rect: %d, %d,  %d x %d\n", __func__, lay_rect.x, lay_rect.y, lay_rect.w, lay_rect.h);
		
		
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &lay_rect);
///sensor zoom

		if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
			{
				memset(&lay_rect, 0, sizeof(DS_Rect));
				//if(pIStuts->SensorStat==0)
				//	{
					lay_rect.w = vcapWH[pIStuts->SensorStat][0]/2;
					lay_rect.h = vcapWH[pIStuts->SensorStat][1]/2;
					lay_rect.x = vcapWH[pIStuts->SensorStat][0]/4;
					lay_rect.y = vcapWH[pIStuts->SensorStat][1]/4;
					

				//	}
				
				m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);
				if(pIStuts->PicpSensorStat==1)
					{
						
						lay_rect.w = vcapWH[1][0]/6;
						lay_rect.h = vcapWH[1][1]/6;
						lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					}
				if(pIStuts->PicpSensorStat==0)
					{
						
						lay_rect.w = vcapWH[0][0]/6;
						lay_rect.h = vcapWH[0][1]/6;
						lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					}


				
			}

		
		
	
	}

	if(msgId == MSGID_EXT_INPUT_TRACK)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->AvtTrkStat = pInCmd->AvtTrkStat;
		}

		char procStr[][10] = {"ACQ", "TARGET", "MTD", "SECTRK", "SEARCH", "ROAM", "SCENE", "IMGTRK"};

		if (pIStuts->AvtTrkStat == eTrk_mode_acq)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

			dynamic_config(VP_CFG_TrkEnable, 0);
			pIStuts->unitAimX = pIStuts->unitAxisX[extInCtrl.SensorStat ] ;//- pIStuts->unitAimW/2;
			if(pIStuts->unitAimX<0)
				{
					pIStuts->unitAimX=0;
				}
			
			pIStuts->unitAimY = pIStuts->unitAxisY[extInCtrl.SensorStat ];// - pIStuts->unitAimH/2;
				if(pIStuts->unitAimY<0)
				{
					pIStuts->unitAimY=0;
				}

			return ;
		}

		int iSens = (pIStuts->SensorStat+1)%eSen_Max;
		 if (pIStuts->AvtTrkStat == eTrk_mode_sectrk)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

			pIStuts->AvtTrkStat = eTrk_mode_sectrk;
			 pIStuts->unitAimX = pIStuts->ImgPixelX[extInCtrl.SensorStat];
		  	 pIStuts->unitAimY = pIStuts->ImgPixelY[extInCtrl.SensorStat] ;
			// pIStuts->unitAxisX[extInCtrl.SensorStat ]=pIStuts->unitAimX ;
		  	// pIStuts->unitAxisY[extInCtrl.SensorStat ]=pIStuts->unitAimY ;
			//pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		else if (pIStuts->AvtTrkStat == eTrk_mode_search)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

		   pIStuts->AvtTrkStat = eTrk_mode_search;
		   pIStuts->unitAimX = pIStuts->ImgPixelX[extInCtrl.SensorStat];
		   pIStuts->unitAimY = pIStuts->ImgPixelY[extInCtrl.SensorStat] ;
		//   pIStuts->unitAxisX[extInCtrl.SensorStat ]=pIStuts->unitAimX ;
		//   pIStuts->unitAxisY[extInCtrl.SensorStat ]=pIStuts->unitAimY ;
		   //pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		else if (pIStuts->AvtTrkStat == eTrk_mode_mtd)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);


			pIStuts->AvtTrkStat = eTrk_mode_target;
			pIStuts->unitAimX=pIStuts->unitMtdPixelX;
			pIStuts->unitAimY=pIStuts->unitMtdPixelX;

				if(pIStuts->unitMtdValid)
			{
				tempvalue=pIStuts->unitMtdPixelX;
					//- pIStuts->unitAimW/2;
				
				if(tempvalue<0)
					{
						pIStuts->unitAimX=0;
					}
				else
					{
						pIStuts->unitAimX=tempvalue;

					}
				tempvalue=pIStuts->unitMtdPixelY ;
				//- pIStuts->unitAimH/2;
				if(tempvalue<0)
					{
						pIStuts->unitAimY=0;
					}
				else
					{
						pIStuts->unitAimY=tempvalue;

					}
				
		//		OSA_printf(" %d:%s set track to x =%f y=%f  mtdx=%d mtdy=%d  w=%d  h=%d\n", OSA_getCurTimeInMsec(), __func__,
		//				pIStuts->unitAimX,pIStuts->unitAimY, pIStuts->unitMtdPixelX,pIStuts->unitMtdPixelY,pIStuts->unitAimW/2,pIStuts->unitAimH/2);
			}
			else
			{
				pIStuts->unitAimX = pIStuts->unitAxisX[extInCtrl.SensorStat ] - pIStuts->unitAimW/2;
				pIStuts->unitAimY = pIStuts->unitAxisY[extInCtrl.SensorStat ] - pIStuts->unitAimH/2;
			}
			//pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);
		UTC_RECT_float rc;
		rc.x=pIStuts->unitAimX-pIStuts->unitAimW/2;
		rc.y=pIStuts->unitAimY-pIStuts->unitAimH/2;
		rc.width=pIStuts->unitAimW;
		rc.height=pIStuts->unitAimH;
		dynamic_config(VP_CFG_TrkEnable, 1,&rc);
		if((pIStuts->AvtTrkStat == eTrk_mode_sectrk)||(pIStuts->AvtTrkStat == eTrk_mode_search))
			{
				m_intervalFrame=2;
				m_rcAcq=rc;
				pIStuts->AvtTrkStat = eTrk_mode_target;
				
				printf("***********************set sec track\n ");
				
			}
	//	printf("the rc.x=%d rc.y=%d ,unitAimX=%d  unitAimY=%d \n",rc.x,rc.y,pIStuts->unitAimX,pIStuts->unitAimY);
	//	printf("w=%d h=%d\n",pIStuts->unitAimW,pIStuts->unitAimH);
	//	printf("*************x=%d y=%d\n",pIStuts->unitAxisX[extInCtrl.SensorStat ],pIStuts->unitAxisY[extInCtrl.SensorStat ]);
		
 	}

	if(msgId == MSGID_EXT_INPUT_ENMTD)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgMtdStat[0] = pInCmd->ImgMtdStat[0];
			pIStuts->ImgMtdStat[1] = pInCmd->ImgMtdStat[1];
		}

		int MTDStatus = (pIStuts->ImgMtdStat[pIStuts->SensorStat]&0x01) ;

//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);

		if(MTDStatus)
			dynamic_config(VP_CFG_MtdEnable, 1);
		else
			dynamic_config(VP_CFG_MtdEnable, 0);
	}

	if(msgId == MSGID_EXT_INPUT_ENENHAN)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgEnhStat[0] = pInCmd->ImgEnhStat[0];
			pIStuts->ImgEnhStat[1] = pInCmd->ImgEnhStat[1];
		}

		int ENHStatus = (pIStuts->ImgEnhStat[pIStuts->SensorStat]&0x01) ;

		OSA_printf(" %d:%s set mtd enMask %d\n", OSA_getCurTimeInMsec(),__func__,ENHStatus);

		if(ENHStatus)
			dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
		else
			dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
	}

	if(msgId == MSGID_EXT_INPUT_ENBDT)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgBlobDetect[0] = pInCmd->ImgBlobDetect[0];
			pIStuts->ImgBlobDetect[1] = pInCmd->ImgBlobDetect[1];
		}

		int BlobStatus = (pIStuts->ImgBlobDetect[pIStuts->SensorStat]&0x01) ;

		//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);

		if(BlobStatus)
			dynamic_config(VP_CFG_BlobEnable, 1);
		else
			dynamic_config(VP_CFG_BlobEnable, 0);
	}

	if(msgId == MSGID_EXT_INPUT_RST_THETA)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
		}

		int videoTrans = (pIStuts->ImgVideoTrans[pIStuts->SensorStat]&0x01) ;

//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);
		GLfloat result[16];

		if(videoTrans)
		{
			my_rotate(result, 45.0);
			dynamic_config(CDisplayer::DS_CFG_VideoTransMat, pIStuts->SensorStat, result);
		}
		else
		{
			my_rotate(result, 0.0);
			dynamic_config(CDisplayer::DS_CFG_VideoTransMat, pIStuts->SensorStat, result);
		}
	}


	if(msgId == MSGID_EXT_INPUT_AIMPOS || msgId == MSGID_EXT_INPUT_AIMSIZE)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->AvtTrkAimSize = pInCmd->AvtTrkAimSize;
			pIStuts->AvtMoveX = pInCmd->AvtMoveX;
			pIStuts->AvtMoveY = pInCmd->AvtMoveY;
		}

		if(pIStuts->AvtTrkAimSize<0||pIStuts->AvtTrkAimSize>4)
			pIStuts->AvtTrkAimSize=2;

		pIStuts->unitAimW = trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
		pIStuts->unitAimH= trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
		if(pIStuts->AvtTrkStat == eTrk_mode_target)
		{

			UTC_RECT_float rc;
			if(msgId == MSGID_EXT_INPUT_AIMSIZE)
				{
					rc.x=pIStuts->unitAimX-pIStuts->unitAimW/2;
					rc.y=pIStuts->unitAimY-pIStuts->unitAimH/2;
				}
			else
				{
					rc.x=pIStuts->unitAimX-pIStuts->unitAimW/2+pIStuts->AvtMoveX;
					rc.y=pIStuts->unitAimY-pIStuts->unitAimH/2+pIStuts->AvtMoveY;

				}
			rc.width=pIStuts->unitAimW;
			rc.height=pIStuts->unitAimH;
			//dynamic_config(VP_CFG_TrkEnable, 0);
			//OSA_waitMsecs(40);
			//usleep(1);
			//dynamic_config(VP_CFG_TrkEnable, 1,&rc);
			m_intervalFrame=1;
			m_rcAcq=rc;
			OSA_printf(" %d:%s refine move (%d, %d), wh(%f, %f)  aim(%d,%d) rc(%f,%f)\n", OSA_getCurTimeInMsec(), __func__,
						pIStuts->AvtMoveX, pIStuts->AvtMoveY, 
						rc.width, rc.height,pIStuts->unitAimX,pIStuts->unitAimY,rc.x,rc.y);
		}

		return ;
	}


		if(msgId == MSGID_EXT_INPUT_ENZOOM)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgZoomStat[0] = pInCmd->ImgZoomStat[0];
			pIStuts->ImgZoomStat[1] = pInCmd->ImgZoomStat[1];
			pIStuts->DispZoomMultiple[0] = pInCmd->DispZoomMultiple[0];
			pIStuts->DispZoomMultiple[1] = pInCmd->DispZoomMultiple[1];
		}
#if 1
		DS_Rect lay_rect;
		
		if(pIStuts->SensorStat==0)//tv
			{
					memset(&lay_rect, 0, sizeof(DS_Rect));
					if(pIStuts->ImgZoomStat[0])
					{
						lay_rect.w = vdisWH[0][0]/2;
						lay_rect.h = vdisWH[0][1]/2;
						lay_rect.x = vdisWH[0][0]/4;
						lay_rect.y = vdisWH[0][1]/4;
						printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					
					m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);


					memset(&lay_rect, 0, sizeof(DS_Rect));
					
					if(pIStuts->ImgZoomStat[0]&&(pIStuts->PicpSensorStat==1))
					{
						
						lay_rect.w = vcapWH[1][0]/6;
						lay_rect.h = vcapWH[1][1]/6;
						lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
						printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
				
					else
						{

							lay_rect.w = vcapWH[1][0]/3;
							lay_rect.h = vcapWH[1][1]/3;
							lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
							lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
							
						}
					
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					
					
			}
		else
			{
		
					memset(&lay_rect, 0, sizeof(DS_Rect));
					#if 1
					if(pIStuts->ImgZoomStat[1])
					{
						//lay_rect.w = vdisWH[0][0]/2;
						//lay_rect.h = vdisWH[0][1]/2;
						//lay_rect.x = vdisWH[0][0]/4;
						//lay_rect.y = vdisWH[0][1]/4;
						lay_rect.w = vcapWH[pIStuts->SensorStat][0]/2;
						lay_rect.h = vcapWH[pIStuts->SensorStat][1]/2;
						lay_rect.x = vcapWH[pIStuts->SensorStat][0]/4;
						lay_rect.y = vcapWH[pIStuts->SensorStat][1]/4;

						printf("*************************fir  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);
					#endif

					memset(&lay_rect, 0, sizeof(DS_Rect));
					if(pIStuts->ImgZoomStat[0]&&(pIStuts->PicpSensorStat==0))
					{
						
						lay_rect.w = vcapWH[0][0]/6;
						lay_rect.h = vcapWH[0][1]/6;
						lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
						printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					else 
						{

							lay_rect.w = vcapWH[0][0]/3;
							lay_rect.h = vcapWH[0][1]/3;
							lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
							lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
							
						}
					
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
			}

		
#endif
		return ;
	}
	if(msgId ==MSGID_EXT_INPUT_ENFREZZ)
		{
			int freeze=pIStuts->ImgFrezzStat[pIStuts->SensorStat];
			DS_Rect rendposr;
			rendposr.x=vdisWH[0][0]*2/3;
			rendposr.y=vdisWH[0][1]*2/3;
			rendposr.w=vdisWH[0][0]/3;
			rendposr.h=vdisWH[0][1]/3;
			if(freeze)
				m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendposr);
			else
				{
				rendposr=rendpos[pIStuts->PicpPosStat];
				m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendposr);
				}
			//tui chu freeze
			m_display.dynamic_config(CDisplayer::DS_CFG_FreezeEnable, 1, &freeze);
			m_display.dynamic_config(CDisplayer::DS_CFG_FreezeEnable, 0, &freeze);

		}
	if(msgId ==MSGID_EXT_INPUT_PICPCROP)
		{

			//DS_Rect rendpos;
			//rendposr.x=0;
			//rendpos.y=0;
			//rendpos.w=vdisWH[1][0]/3;
			//rendpos.w=vdisWH[1][1]/3;

			//printf("the x=%d y=%d w=%d h=%d  pIStuts->PicpPosStat=%d\n",rendpos[pIStuts->PicpPosStat].x,rendpos[pIStuts->PicpPosStat].y,
			//rendpos[pIStuts->PicpPosStat].w,rendpos[pIStuts->PicpPosStat].h,pIStuts->PicpPosStat);
		
			
			m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendpos[pIStuts->PicpPosStat]);

		}
	if(msgId ==MSGID_EXT_INPUT_COAST)
		{
			m_castTm=OSA_getCurTimeInMsec();
			m_bCast=true;
			

		}
	if(msgId ==MSGID_EXT_INPUT_VIDEOEN)
		{
			int status=pIStuts->unitFaultStat&0x01;
			status^=1;
			m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, 0, &status);
			printf("MSGID_EXT_INPUT_VIDEOEN status0=%d\n",status);
			 status=(pIStuts->unitFaultStat>1)&0x01;
			 status^=1;
			m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, 1, &status);
			printf("MSGID_EXT_INPUT_VIDEOEN status1=%d\n",status);
			

		}
	
	/*if(msgId == MSGID_EXT_INPUT_DISPGRADE)
	{
		if(pIStuts->DispGrp[0] == eDisp_hide)
			pThis->m_display.m_bOsd = false;
		else
			pThis->m_display.m_bOsd = true;
	}*/
}


/////////////////////////////////////////////////////
//int majormmtid=0;

 int  CProcess021::MSGAPI_initial()
{
   OSA_printf("msgapi_initial\n");
   
   MSGDRIV_Handle handle=&g_MsgDrvObj;
    assert(handle != NULL);
    memset(handle->msgTab, 0, sizeof(MSGTAB_Class) * MAX_MSG_NUM);
//MSGID_EXT_INPUT_MTD_SELECT
    MSGDRIV_attachMsgFun(handle,    MSGID_SYS_INIT,           				   MSGAPI_init_device,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_SENSOR,           	   MSGAPI_inputsensor,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_PICPCROP,      		   MSGAPI_croppicp,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_TRACK,          		   MSGAPI_inputtrack,     		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENMTD,                      MSGAPI_inpumtd,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_MTD_SELECT,     	   MSGAPI_inpumtdSelect,    		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_AIMPOS,          	  	   MSGAPI_setAimRefine,    		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENENHAN,           	   MSGAPI_inpuenhance,       	    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENBDT,           		   MSGAPI_inputbdt,         		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENZOOM,           	   MSGAPI_inputzoom,                     0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENFREZZ,           	   MSGAPI_inputfrezz,                      0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_MTD_SELECT,      	   MSGAPI_inputmmtselect,              0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_AXISPOS,     	  	   MSGAPI_inputpositon,                   0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_COAST,             	   MSGAPI_inputcoast,                      0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_FOVSELECT,                MSGAPI_inputfovselect,                 0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_FOVSTAT,                   MSGAPI_inputfovchange,               0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_SEARCHMOD,             MSGAPI_inputsearchmod,              0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_VIDEOEN,            	   MSGAPI_inputvideotect,                 0);
	
//add function
    MSGDRIV_attachMsgFun(handle,	MSGID_SYS_RESET,                                     MSGAPI_reset_device,	           0);
#if 0
    MSGDRIV_attachMsgFun(handle,	MSGID_EXT_INPUT_SYSMODE,           	           DRAMCTRL_setSysModeStat,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_GRAYSTAT,                            DRAMCTRL_crossColorState ,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_OSDSTAT,                              DRAMCTRL_osdState ,	           0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_CHECKINFOR,                         DRAMCTRL_sysCheckResult ,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_BOREREFINE,                          DRAMCTRL_boreRefine ,	           0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_CHECKSTART,                        DRAMCTRL_sysCheckStart,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_ENPICP,                                 DRAMCTRL_setPicp,	                  0);
    MSGDRIV_attachMsgFun(handle,	MSGID_DRAM_PICPMODE,                            DRAMCTRL_changePicpMode,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_CTRL_ZOOM,                                    DRAMCTRL_setWrapMode ,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_FLIR_STATE,                                     DRAMCTRL_dispFlirStat ,	           0);
    MSGDRIV_attachMsgFun(handle,	MSGID_SAVE_ARIX,                                     DRAMCTRL_saveCorrArix,	           0);
    MSGDRIV_attachMsgFun(handle,	MSGID_SEL_NEXTTAR,                                  DRAMCTRL_selectNextTarget ,      0);
    MSGDRIV_attachMsgFun(handle,	MSGID_SEL_PICPSENS,                                DRAMCTRL_selectPicpSen,  	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_BORESIGHT_CHANGE,                       AVTCTRL_boreSightUpdate ,	    0);
    MSGDRIV_attachMsgFun(handle,	MSGID_HIDE_OSD,                                      OSDCTRL_hideOsd  ,	                  0);
    MSGDRIV_attachMsgFun(handle,	MSGID_TRA_ENLOCK,                                  DRAMCTRL_settrackBreakLock ,    0);
#endif
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_AUTOCHECK,                         processCMD_BUTTON_AUTOCHECK ,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_BOOT_UP_CHECK_COMPLETE,              processCMD_BOOT_UP_CHECK_COMPLETE,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_EXIT_SELF_CHECK,                             processCMD_EXIT_SELF_CHECK ,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_CALIBRATION,		          processCMD_BUTTON_CALIBRATION ,	0); // У׼��ť
	
    MSGDRIV_attachMsgFun(handle,	CMD_POSITION_SENSOR_OK,		          onPositionSensorOK,	0); // ��λ��������
    MSGDRIV_attachMsgFun(handle,	CMD_POSITION_SENSOR_ERR,		          onPositionSensorERR,	0); // ��λ��������
	
    MSGDRIV_attachMsgFun(handle,	CMD_DISPLAY_ERR,				          onDisplayErr,	0); // OSD��ʾ�쳣
    MSGDRIV_attachMsgFun(handle,	CMD_DISPLAY_OK,				          onDisplayOK,	0); // OSD��ʾ��
    MSGDRIV_attachMsgFun(handle,	CMD_JOYSTICK_OK,				          onJoyStickOK,	0); // �ֱ������?    MSGDRIV_attachMsgFun(handle,	CMD_JOYSTICK_ERR,				          onJoyStickErr,	0); // �ֱ������?    MSGDRIV_attachMsgFun(handle,	CMD_WEAPONCTRL_OK,			          onWeaponCtrlOK,		0); //�Կ���
    
    MSGDRIV_attachMsgFun(handle,	CMD_DIP_ANGLE_OK,				          onDipAngleSensorOK,	0); // ����Ǵ�������?    MSGDRIV_attachMsgFun(handle,	CMD_DIP_ANGLE_ERR,				          onDipAngleSensorERR,	0); // ����Ǵ�������?    
    MSGDRIV_attachMsgFun(handle,	CMD_MACHINEGUN_SENSOR_OK,	          onMachineGunSensorOK,	0); // ��ǹ��������
    MSGDRIV_attachMsgFun(handle,	CMD_MACHINEGUN_SENSOR_ERR,	          onMachineGunSensorERR,	0); // ��ǹ��������
    MSGDRIV_attachMsgFun(handle,	CMD_MACHINEGUN_SERVO_OK,		          onMachineGunServoOK,	0); // ��ǹ�ŷ���
    MSGDRIV_attachMsgFun(handle,	CMD_MACHINEGUN_SERVO_ERR,		          onMachineGunServoERR,	0); // ��ǹ�ŷ���
    
    MSGDRIV_attachMsgFun(handle,	CMD_POSITION_SERVO_OK,			   	   onPositionServoOK,	0); // ��λ�ŷ���
    MSGDRIV_attachMsgFun(handle,	CMD_POSITION_SERVO_ERR,	     	          onPositionServoERR,	0); // ��λ�ŷ���
    
    MSGDRIV_attachMsgFun(handle,	CMD_GENERADE_SENSOR_OK,		          onGrenadeSensorOK,	0); // 35�񴫸�����
    MSGDRIV_attachMsgFun(handle,	CMD_GENERADE_SENSOR_ERR,		          onGrenadeSensorERR,	0); // 35�񴫸�����
    MSGDRIV_attachMsgFun(handle,	CMD_GENERADE_SERVO_OK,		          onGrenadeServoOK,	0); // 35���ŷ���
    MSGDRIV_attachMsgFun(handle,	CMD_GENERADE_SERVO_ERR,		          onGrenadeServoERR,	0); // 35���ŷ���

    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_BATTLE,				processCMD_BUTTON_BATTLE,	0); // qie huan wei zhan dou mo shi
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_QUIT,				processCMD_BUTTON_QUIT,	0); //tui chu an jian
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_UNLOCK,			processCMD_BUTTON_UNLOCK,	0); // �����?    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_UP,					processCMD_BUTTON_UP,	0); // ���ϰ���
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_DOWN,				processCMD_BUTTON_DOWN,	0); // ���°���
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_UP,					processCMD_BUTTON_UP,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_LEFT,				processCMD_BUTTON_LEFT,	0); // ���󰴼�
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_RIGHT,				processCMD_BUTTON_RIGHT,	0); // ���Ұ���
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_ENTER,				processCMD_BUTTON_ENTER,	0); // ȷ�ϰ���
    MSGDRIV_attachMsgFun(handle,	CMD_BULLET_SWITCH0,			processCMD_BULLET_SWITCH0,	0); // ��ǹ0����
    MSGDRIV_attachMsgFun(handle,	CMD_BULLET_SWITCH1,			processCMD_BULLET_SWITCH1,	0); // ��ǹ1����
    MSGDRIV_attachMsgFun(handle,	CMD_BULLET_SWITCH2,			processCMD_BULLET_SWITCH2,	0); // ��ǹ2����
    MSGDRIV_attachMsgFun(handle,	CMD_BULLET_SWITCH3,			processCMD_BULLET_SWITCH3,	0); // ��ǹ3����
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_AUTOCATCH,			processCMD_BUTTON_AUTOCATCH,0); // �Զ�����
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_BATTLE_AUTO,		processCMD_BUTTON_BATTLE_AUTO,	0); // ս��ģʽ
    MSGDRIV_attachMsgFun(handle,	CMD_BUTTON_BATTLE_ALERT,		processCMD_BUTTON_BATTLE_ALERT,	0); // ����ģʽ
    MSGDRIV_attachMsgFun(handle,	CMD_USER_FIRED,					processCMD_USER_FIRED,	0); // ȷ�Ϸ���
    MSGDRIV_attachMsgFun(handle,	CMD_DETEND_LOCK,				processCMD_DETEND_LOCK,	0); // ֹ������
    MSGDRIV_attachMsgFun(handle,	CMD_DETEND_UNLOCK,			processCMD_DETEND_UNLOCK,	0); // ֹ������
    MSGDRIV_attachMsgFun(handle,	CMD_MAINTPORT_LOCK,			processCMD_MAINTPORT_LOCK,	0); //ά���Ź�
    MSGDRIV_attachMsgFun(handle,	CMD_MAINTPORT_UNLOCK,			processCMD_MAINTPORT_UNLOCK,	0); // ά���ſ�
    
    MSGDRIV_attachMsgFun(handle,	CMD_MEASURE_DISTANCE_SWITCH,			processCMD_MEASURE_DISTANCE_SWITCH,	0); //�л���෽�?    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_SAVE,		processCMD_CALIBRATION_SWITCH_TO_SAVE,	0); // У׼-�л�����
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_ZERO,		processCMD_CALIBRATION_SWITCH_TO_ZERO,	0); // У׼-�л�У��
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_WEATHER,	processCMD_CALIBRATION_SWITCH_TO_WEATHER,	0); // У׼-�л�����
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_GENERAL,	processCMD_CALIBRATION_SWITCH_TO_GENERAL,	0); // У׼-�л�����
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_GENPRAM,	processCMD_CALIBRATION_SWITCH_TO_GENPRAM,	0); // У׼-�۲�У׼
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_HORIZEN,	processCMD_CALIBRATION_SWITCH_TO_HORIZEN,	0); // У׼-��ƽУ��
    MSGDRIV_attachMsgFun(handle,	CMD_CALIBRATION_SWITCH_TO_LASER,		processCMD_CALIBRATION_SWITCH_TO_LASER,	0); // У׼-����У��
    MSGDRIV_attachMsgFun(handle,	CMD_LASER_FAIL,							processCMD_LASER_FAIL,		0); // ������ʧ��
    MSGDRIV_attachMsgFun(handle,	CMD_TRACKING_FAIL,						processCMD_TRACKING_FAIL,	0); // �Զ�����ʧ��
    MSGDRIV_attachMsgFun(handle,	CMD_VELOCITY_FAIL,						processCMD_VELOCITY_FAIL,	0); // ����ʧ��
    MSGDRIV_attachMsgFun(handle,	CMD_MEASURE_VELOCITY,					processCMD_MEASURE_VELOCITY,0); // ����ָ��
    MSGDRIV_attachMsgFun(handle,	CMD_MEASURE_DISTANCE,					processCMD_MEASURE_DISTANCE,0); // ���ָ��?    
    MSGDRIV_attachMsgFun(handle,	CMD_LASER_OK,							processCMD_LASER_OK,		0); // ��������
    MSGDRIV_attachMsgFun(handle,	CMD_TRACKING_OK,						processCMD_TRACKING_OK,			0); // ������
    MSGDRIV_attachMsgFun(handle,	CMD_FIRING_TABLE_LOAD_OK,				processCMD_FIRING_TABLE_LOAD_OK,		0); // �����Ԫװ����?    MSGDRIV_attachMsgFun(handle,	CMD_FIRING_TABLE_FAILURE,				processCMD_FIRING_TABLE_FAILURE,		0); // �����Ԫװ��ʧ��?    MSGDRIV_attachMsgFun(handle,	CMD_MODE_AIM_LAND,				      		processCMD_MODE_AIM_LAND,		0); // �л��Ե�Ŀ��
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_AIM_SKY,						processCMD_MODE_AIM_SKY,		0); // �л��Կ�Ŀ��
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_ATTACK_SIGLE,				processCMD_MODE_ATTACK_SIGLE,		0); // �л�Ϊ����
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_ATTACK_MULTI,				processCMD_MODE_ATTACK_MULTI,		0); // �л�Ϊ����
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_FOV_SMALL,					processCMD_MODE_FOV_SMALL,		0); // �л�ΪС�ӳ�
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_FOV_LARGE,					processCMD_MODE_FOV_LARGE,		0); // �л�Ϊ���ӳ�
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_SCALE_SWITCH,				processCMD_MODE_SCALE_SWITCH,		0); // �л��Ŵ�
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_PIC_COLOR_SWITCH,			processCMD_MODE_PIC_COLOR_SWITCH,		0); // �л�ͼ��
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_ENHANCE_SWITCH,				processCMD_MODE_ENHANCE_SWITCH,		0); // ��Ƶ��ǿ�л�
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_SHOT_SHORT,					processCMD_MODE_SHOT_SHORT,		0); // �л�Ϊ�̵��䡢������
    MSGDRIV_attachMsgFun(handle,	CMD_MODE_SHOT_LONG,					processCMD_MODE_SHOT_LONG,		0); // �л�Ϊ�����䡢����
    MSGDRIV_attachMsgFun(handle,	CMD_SCHEDULE_GUN,						processCMD_SCHEDULE_GUN,		0); // ��ǹ��
    MSGDRIV_attachMsgFun(handle,	CMD_SCHEDULE_STRONG,					processCMD_SCHEDULE_STRONG,		0); // ��ǿ��
    MSGDRIV_attachMsgFun(handle,	CMD_SCHEDULE_RESET,					processCMD_SCHEDULE_RESET,		0); // ����λ
    
    MSGDRIV_attachMsgFun(handle,	CMD_TIMER_SENDFRAME0,					processCMD_TIMER_SENDFRAME0,		0); //send frame0 through CAN
    MSGDRIV_attachMsgFun(handle,	CMD_TRACE_SENDFRAME0,					processCMD_TRACE_SENDFRAME0,		0); //send frame0 through TracePort
    MSGDRIV_attachMsgFun(handle,	CMD_TRIGGER_AVT,						processCMD_TRIGGER_AVT,		0); //����AVT������������
    MSGDRIV_attachMsgFun(handle,	CMD_QUIT_AVT_TRACKING,					quitAVTtrigger,		0); //����AVT�˳���������
    MSGDRIV_attachMsgFun(handle,	CMD_SEND_MIDPARAMS,					processCMD_SEND_MIDPARAMS,		0); //�����м����?    MSGDRIV_attachMsgFun(handle,	CMD_GRENADE_LOAD_IN_POSITION,			processCMD_GRENADE_LOAD_IN_POSITION,		0); //35��װ�?λָʾ��
    MSGDRIV_attachMsgFun(handle,	CMD_CALCNUM_SHOW,						processCMD_CALCNUM_SHOW,		0); //�����м�ֵ��ʾ
    MSGDRIV_attachMsgFun(handle,	CMD_CALCNUM_HIDE,						processCMD_CALCNUM_HIDE,		0); //�����м�ֵ����
    MSGDRIV_attachMsgFun(handle,	CMD_MIDPARAMS_SWITCH,					processCMD_MIDPARAMS_SWITCH,		0); //����F5
    MSGDRIV_attachMsgFun(handle,	CMD_LASERSELECT_SWITCH,				processCMD_LASERSELECT_SWITCH,		0); //F5
    MSGDRIV_attachMsgFun(handle,	CMD_STABLEVIDEO_SWITCH,				processCMD_STABLEVIDEO_SWITCH,		0); //����F3(��Ƶ��ǿ)
    
    MSGDRIV_attachMsgFun(handle,	CMD_SENSOR_SWITCH,					processCMD_SENSOR_SWITCH,		0); //F6
    MSGDRIV_attachMsgFun(handle,	CMD_CONNECT_SWITCH,					processCMD_CONNECT_SWITCH,		0); //����F6
    
    MSGDRIV_attachMsgFun(handle,	CMD_IDENTIFY_KILL,						processCMD_IDENTIFY_KILL,		0); //�����ʾ��ϢΪKILL
    MSGDRIV_attachMsgFun(handle,	CMD_IDENTIFY_GAS,						processCMD_IDENTIFY_GAS,		0); //�����ʾ��ϢΪGAS
    MSGDRIV_attachMsgFun(handle,	CMD_SERVO_INIT,							processCMD_SERVO_INIT,		0); //��ʼ���ŷ�����������
    MSGDRIV_attachMsgFun(handle,	CMD_SERVOTIMER_MACHGUN,				processCMD_SERVOTIMER_MACHGUN,	0); //��ʼ���ŷ�����������
    MSGDRIV_attachMsgFun(handle,	CMD_MACHSERVO_MOVESPEED,				processCMD_MACHSERVO_MOVESPEED,	0); //��ǹ�ŷ��ٶȿ���
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADESERVO_MOVESPEED,			processCMD_GRENADESERVO_MOVESPEED,	0); //���ŷ��ٶȿ���
    MSGDRIV_attachMsgFun(handle,	CMD_MACHSERVO_STOP,					processCMD_MACHSERVO_STOP,	0); //��ǹ�ŷ�ֹͣ
    MSGDRIV_attachMsgFun(handle,	CMD_MACHSERVO_MOVEOFFSET,				processCMD_MACHSERVO_MOVEOFFSET,	0); //��ǹ�ŷ�λ�ÿ���
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADESERVO_MOVEOFFSET,			processCMD_GRENADESERVO_MOVEOFFSET,	0); //��λ�ŷ�λ�ÿ���
    
    MSGDRIV_attachMsgFun(handle,	CMD_LIHEQI_CLOSE,				processCMD_LIHEQI_CLOSE,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_LIHEQI_OPEN,				processCMD_LIHEQI_OPEN,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADE_ERR,				processCMD_GRENADE_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADE_OK,					processCMD_GRENADE_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_FIREBUTTON_ERR,			processCMD_FIREBUTTON_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_FIREBUTTON_OK,			processCMD_FIREBUTTON_OK,	0);

    MSGDRIV_attachMsgFun(handle,	CMD_FULSCREENCAN_ERR,				processCMD_FULSCREENCAN_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_FULSCREENCAN_OK,				processCMD_FULSCREENCAN_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DISCONTRLCAN0_ERR,			processCMD_DISCONTRLCAN0_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DISCONTRLCAN0_OK,				processCMD_DISCONTRLCAN0_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DISCONTRLCAN1_ERR,			processCMD_DISCONTRLCAN1_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DISCONTRLCAN1_OK,				processCMD_DISCONTRLCAN1_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DIANCITIE_ERR,							processCMD_DIANCITIE_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_DIANCITIE_OK,							processCMD_DIANCITIE_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_POSMOTOR_ERR,						processCMD_POSMOTOR_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_POSMOTOR_OK,						processCMD_POSMOTOR_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_MACHGUNMOTOR_ERR,			processCMD_MACHGUNMOTOR_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_MACHGUNMOTOR_OK,			processCMD_MACHGUNMOTOR_OK,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADEMOTOR_ERR,			processCMD_GRENADEMOTOR_ERR,	0);
    MSGDRIV_attachMsgFun(handle,	CMD_GRENADEMOTOR_OK,				processCMD_GRENADEMOTOR_OK,	0);

    return 0;
}



 #if 1
 void CProcess021::MSGAPI_init_device(long lParam )
{
	
	//sThis->msgdriv_event(MSGID_SYS_INIT,NULL);
	OSA_printf("%s,line:%d ... MSGAPI_init_device!",__func__,__LINE__);
	return ;
}



 void CProcess021::MSGAPI_inputsensor(long lParam )
{
	#if 1
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	//	pIStuts->SensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
	sThis->msgdriv_event(MSGID_EXT_INPUT_SENSOR,NULL);
	#endif
	OSA_printf("%s,line:%d ... MSGAPI_inputsensor!",__func__,__LINE__);
	return ;
}



 void CProcess021::MSGAPI_picp(long lParam )
{
	#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		if(pIStuts->PicpSensorStat == 0xFF)
			pIStuts->PicpSensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
		else
			pIStuts->PicpSensorStat = 0xFF;
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP,NULL);
	#endif
	OSA_printf("%s,line:%d ... MSGAPI_picp!",__func__,__LINE__);
	return ;
}



 void CProcess021::MSGAPI_croppicp(long lParam )
{
	#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	//	if(pIStuts->PicpSensorStat == 0xFF)
	//		pIStuts->PicpSensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
	//	else
	//		pIStuts->PicpSensorStat = 0xFF;
	if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x04)
		{
			return ;

		}

	if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x01)
		{
		pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat = (pIStuts->SensorStat+1) % (eSen_Max);
		sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);

		}
	else if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x02)
		{
		      pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat =2;
		      sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);

		}
	else if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x03)
		{
			pIStuts->PicpPosStat=(pIStuts->PicpPosStat+1)%4;
			sThis->msgdriv_event(MSGID_EXT_INPUT_PICPCROP,NULL);

		}		
	#endif
	OSA_printf("%s,line:%d ... MSGAPI_croppicp!",__func__,__LINE__);
	return ;
}


   
 void CProcess021::MSGAPI_inputtrack(long lParam )
{
	#if 1
	printf("%s^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",__func__);
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->AvtTrkStat)
		//	pIStuts->AvtTrkStat = eTrk_mode_acq;
		//else
		//	pIStuts->AvtTrkStat = eTrk_mode_target;
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_TRACK,NULL);
	#endif
	OSA_printf("%s,line:%d ... MSGAPI_inputtrack",__func__,__LINE__);
	return ;
}



 void CProcess021::MSGAPI_inpumtd(long lParam )
{
	//CMD_EXT *pIStuts = &sThis->extInCtrl;
	//sThis->msgdriv_event(MSGID_EXT_INPUT_ENMTD,NULL);
	OSA_printf("%s,line:%d ... MSGAPI_inpumtd",__func__,__LINE__);
	return ;
}

 
 void CProcess021::MSGAPI_inpumtdSelect(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inpumtdSelect",__func__,__LINE__);
	#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	int i;
	if(pIStuts->MMTTempStat==3)
		{

			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(sThis->m_mtd[pIStuts->SensorStat]->tg[majormmtid].valid==1)
						{
							//majormmtid++;
							majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}

				}

			
		}
		else if(pIStuts->MMTTempStat==4)
		{

			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(sThis->m_mtd[pIStuts->SensorStat]->tg[majormmtid].valid==1)
						{
							//majormmtid++;
							if(majormmtid>0)
								majormmtid=(majormmtid-1);
							else
								{
									majormmtid=MAX_TARGET_NUMBER-1;

								}

						}

				}


		}
	//majormmtid=majormmtid;
	#endif	
	return ;
}


void CProcess021::MSGAPI_inpuenhance(long lParam )
{
	//	CMD_EXT *pIStuts = &pThis->extInCtrl;
	//	if(pIStuts->ImgEnhStat[pIStuts->SensorStat])
	//		pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Disable;
	//	else
	//		pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Enable;
	//sThis->msgdriv_event(MSGID_EXT_INPUT_ENENHAN,NULL);
	OSA_printf("%s,line:%d ... MSGAPI_inpuenhance",__func__,__LINE__);
	return ;
}

void CProcess021::MSGAPI_setAimRefine(long lParam          /*=NULL*/)
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	OSA_printf("%s msgextInCtrl->TrkBomenCtrl=%d pIStuts->AvtMoveY=%d\n",__func__,pIStuts->AvtMoveX,pIStuts->AvtMoveY);
	if(pIStuts->AvtMoveX==eTrk_ref_left)
		{
			pIStuts->AvtMoveX=-1;
		}
	else if(pIStuts->AvtMoveX==eTrk_ref_right)
		{
			pIStuts->AvtMoveX=1;
		}
		if(pIStuts->AvtMoveY==eTrk_ref_up)
		{
			pIStuts->AvtMoveY=-1;
		}
	else if(pIStuts->AvtMoveY==eTrk_ref_down)
		{
			pIStuts->AvtMoveY=1;
		}
	sThis->msgdriv_event(MSGID_EXT_INPUT_AIMPOS,NULL);
#endif
	OSA_printf("%s,line:%d ... MSGAPI_setAimRefine",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputbdt(long lParam )
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		if(pIStuts->ImgBlobDetect[pIStuts->SensorStat])
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Enable;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENBDT,NULL);
#endif	
	OSA_printf("%s,line:%d ... MSGAPI_inputbdt",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputzoom(long lParam )
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENZOOM,NULL);
#endif
	OSA_printf("%s,line:%d ... MSGAPI_inputzoom",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputfrezz(long lParam )
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	
	
	if( pIStuts->FrCollimation==1)
		{
			//OSA_printf("the*****************************************enable \n");
			
			pIStuts->PicpSensorStat=0;//tv picp sensor
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
			//dong jie chuang kou
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Enable;
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);

		}
	else
		{
		
		if((pIStuts->PicpSensorStatpri!=0))//tui picp the sensor is tv
				{
					
					pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat=2;//tui chu picp
					sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
					OSA_printf("MSGAPI_inputfrezz*****************************************disable \n");
					//sThis->m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &(sThis->rendpos[pIStuts->PicpPosStat]));
				}
		else
				{
					//sThis->m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &(sThis->rendpos[pIStuts->PicpPosStat]));
					pIStuts->PicpSensorStat=0;
				}
			//tui chu dong jie chuang kou
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Disable;
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);
			
			OSA_printf("the*****************************************disable PicpSensorStatpri=%d\n",pIStuts->PicpSensorStatpri);
		}
#endif
			
	OSA_printf("%s,line:%d ... MSGAPI_inputfrezz",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputmmtselect(long lParam )
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	//pThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);
	if(pIStuts->ImgMtdSelect[pIStuts->SensorStat]  ==eMMT_Next)
		majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;
	else if(pIStuts->ImgMtdSelect[pIStuts->SensorStat]  ==  eMMT_Prev)
		{
			majormmtid=(majormmtid-1+MAX_TARGET_NUMBER)%MAX_TARGET_NUMBER;
		}
#endif

	OSA_printf("%s,line:%d ... MSGAPI_inputmmtselect",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputpositon(long lParam )
{
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	if(pIStuts->SensorStat==0)
		{
	if((pIStuts->AvtPosXTv>=50)&&(pIStuts->AvtPosXTv<=vcapWH[pIStuts->SensorStat][0]-50))
		{
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXTv;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
	if((pIStuts->AvtPosYTv>=50)&&(pIStuts->AvtPosYTv<=vcapWH[pIStuts->SensorStat][1]-50))
		{
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYTv;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}
		}
	else if(pIStuts->SensorStat==1)
		{
#ifdef VIDEO1280X1024
	if((pIStuts->AvtPosXFir>=50)&&(pIStuts->AvtPosXFir<=1230))
		{
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXFir;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
	if((pIStuts->AvtPosYFir>=50)&&(pIStuts->AvtPosYFir<=974))
		{
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYFir;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}


#else
		if((pIStuts->AvtPosXFir>=25)&&(pIStuts->AvtPosXFir<=vcapWH[pIStuts->SensorStat][0]-25))
		{
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXFir;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
			if((pIStuts->AvtPosYFir>=25)&&(pIStuts->AvtPosYFir<=vcapWH[pIStuts->SensorStat][1]-25))
		{
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYFir;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}
#endif

		}
	printf("%s   THE=unitAimX=%d unitAxisY=%d\n",__func__,pIStuts->unitAxisX[pIStuts->SensorStat ],pIStuts->unitAxisY[pIStuts->SensorStat ]);
#endif	
	OSA_printf("%s,line:%d ... MSGAPI_inputpositon",__func__,__LINE__);
	return ;
}



void CProcess021::MSGAPI_inputcoast(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inputcoast",__func__,__LINE__);
	
	//sThis->msgdriv_event(MSGID_EXT_INPUT_COAST,NULL);
	return ;
}



void CProcess021::MSGAPI_inputfovselect(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inputfovselect",__func__,__LINE__);
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	sThis->m_intervalFrame=1;
	sThis->m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
	sThis->m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
	sThis->m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
	sThis->m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
#endif
	return ;
}



void CProcess021::MSGAPI_inputfovchange(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inputfovchange",__func__,__LINE__);
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	sThis->m_intervalFrame=1;
	sThis->m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
	sThis->m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
	sThis->m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
	sThis->m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
#endif
	return ;
}


void CProcess021::MSGAPI_inputsearchmod(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inputsearchmod",__func__,__LINE__);
#if 0
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	float panenv=pIStuts->TrkPanev/4000.0;
	float TrkTitlev=pIStuts->TrkTitlev/4000.0;
	if(panenv>5)
		{
			//sThis->m_searchmod=1;
		}
	else if(panenv<-5)
		{
			//sThis->m_searchmod=2;
		}
	else 
		{
			//sThis->m_searchmod=0;
		}

	// printf("the TrkPanev=%f TrkTitlev=%f\n",pIStuts->TrkPanev/4000.0,pIStuts->TrkTitlev/4000.0);
#endif		
	return ;
}



 void CProcess021::MSGAPI_inputvideotect(long lParam )
{
	OSA_printf("%s,line:%d ... MSGAPI_inputvideotect",__func__,__LINE__);
	
	//sThis->msgdriv_event(MSGID_EXT_INPUT_VIDEOEN,NULL);
	return ;
}



void CProcess021::MSGAPI_reset_device(LPARAM lParam      /*=NULL*/)
{
	OSA_printf("%s,line:%d ... MSGAPI_reset_device",__func__,__LINE__);
	#if 0
    pAppPrt = (APP_OBJ *)lParam;
    SDK_ASSERT(pAppPrt);

    CMD_ext *pIStuts = (CMD_ext*)&pAppPrt->extInCtrl;
    int i, iSens;

    /** < select a8 display */
    msgfunc_checkSwmsSens(pIStuts, pAppPrt->dbgCtrl.nullSrcStat);
    IMGCTRL_swmsSetLayout(pIStuts->unitSensorStat, pIStuts->unitPicpSensorStat);

    // mtd default
    for (iSens = 0; iSens < eSen_Max; iSens++)
    {
        pIStuts->pUnitMtdTgs[iSens] = IMGCTRL_mtdPrmGet(iSens);
        if (pIStuts->pUnitMtdTgs[iSens] == NULL)
            continue;
        for (i = 0; i < MMT_TARGET_NUM; i++)
            pIStuts->unitMtdTgsId[iSens][i] = i;
    }

    /** < set video formula */
    if (pIStuts->SensorStat == 0)
    {
        GPORT_set_avt_winx_pos(iAvtCapPosX[pIStuts->SensorStat]);
        GPORT_set_avt_winy_pos(iAvtCapPosY[pIStuts->SensorStat]);
        GPORT_fifo_reset();
        GPORT_fifo_unreset();
    }
    GPORT_select_avt_sens(pIStuts->SensorStat);     // use current sens
    GPORT_select_avt_dis(pAppPrt->dbgCtrl.avtInOutStat);
#if 0
    /** < set avt21 input config */
    if (AVTCTRL_loadConfig(pAppPrt->avtCtrl) != SDK_SOK)
    {
        AVTCTRL_destroy();
        pAppPrt->avtCtrl = NULL;
    }
#endif
    /** < start report timer */
    OSA_waitMsecs(100);
    TIMER_start(eChk_Timer);
    if (pAppPrt->avtCtrl == NULL)
        TIMER_start(eOsd_Timer);

    OSA_waitMsecs(100);
 //   printf(" [DEBUG:] %s done\n", __func__);
    pAppPrt->dbgCtrl.disCaseSet = 0;
    pAppPrt->extInCtrl.unitWorkMode = NORMAL_MODE;  // self init check end
	#endif
   return ;
}


//ADD PLUS

 void CProcess021::MSGAPI_setSysModeStat(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_setSysModeStat",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_osdState(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_osdState",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_sysCheckResult(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_sysCheckResult",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_boreRefine(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_boreRefine",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_sysCheckStart(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_sysCheckStart",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_setPicp(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_setPicp",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_changePicpMode(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_changePicpMode",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_setWrapMode(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_setWrapMode",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_dispFlirStat(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_dispFlirStat",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_saveCorrArix(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_saveCorrArix",__func__,__LINE__);
	return ;
 }



void CProcess021::MSGAPI_selectNextTarget(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_selectNextTarget",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_selectPicpSen(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_selectPicpSen",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_boreSightUpdate(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_boreSightUpdate",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_hideOsd(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_hideOsd",__func__,__LINE__);
	return ;
 }


void CProcess021::MSGAPI_settrackBreakLock(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... MSGAPI_settrackBreakLock",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_AUTOCHECK(LPARAM lParam)
 {
#if 1
 	if(!isBootUpMode())
	{
		if(isCalibrationMode()&&!isCalibrationMainMenu())
		{
			if(isCalibrationZero())
			{
				;//saveZeroParam();
			}
			else if(isCalibrationGeneral())
			{
				;//saveGeneralParam();
			}
			else if(isCalibrationWeather())
			{
				;//saveWeatherParam();
			}		
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
		//	OSDCTRL_NoShine();
		}	
		gLevel1LastMode = gLevel1Mode;
		gLevel1Mode = MODE_BOOT_UP;
	//	releaseServoContrl();
	}
#endif	
	OSDCTRL_CheckResultsShow();
 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_AUTOCHECK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BOOT_UP_CHECK_COMPLETE(LPARAM lParam)
 {
 	if(isBootUpMode())
	{
		gLevel2BootUpState = STATE_BOOT_UP_SELF_CHECK_FINISH;
		// to do : update OSD to show result OK/abnormal;
		if(Is9stateOK())
			Posd[eSelfCheckResult] = ResultOsd[0];
		OSDCTRL_ItemShow(eSelfCheckResult);
	}
	//ReadParamsFlash();//read data from flash
	
 	//OSA_printf("%s,line:%d ... processCMD_BOOT_UP_CHECK_COMPLETE",__func__,__LINE__);
	return ;
 }

 
void CProcess021::processCMD_EXIT_SELF_CHECK(LPARAM lParam)
 {
	gLevel1Mode = gLevel1LastMode;
	//update OSD elements.

	if(isBattleMode())
	{	
		OSDCTRL_EnterBattleMode();
	}
	else if(isCalibrationMode())
	{
		OSDCTRL_EnterCalibMode();
	}
	OSDCTRL_updateAreaN();

	if(bUnlock)
	{
		bUnlock = !bUnlock;
		Posd[eDynamicZone] = DynamicOsd[0];
		OSDCTRL_ItemShow(eDynamicZone);
		startDynamicTimer();
	}

 	//OSA_printf("%s,line:%d ... processCMD_EXIT_SELF_CHECK",__func__,__LINE__);
	return ;
 }



void CProcess021::processCMD_BUTTON_CALIBRATION(LPARAM lParam)
 {
	if( isBattleMode())
	{
		//if(isStatBattleAlert())
		//	return;
		gLevel1LastMode = gLevel1Mode;
		gLevel1Mode = MODE_CALIBRATION;
		gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
		gLevel3CalibrationState   = Menu_FireView;
		//releaseServoContrl();
		// update OSD to calibration display
	}
	else
	{
		return ;
		//assert(FALSE);
	}
	//OSDCTRL_updateMainMenu(gProjectileType);
			
	OSDCTRL_EnterCalibMode();
 	OSA_printf("%s,line:%d ... processCMD_BUTTON_CALIBRATION",__func__,__LINE__);
	return ;
 }



void CProcess021::onPositionSensorOK(LPARAM lParam)
 {
	isPositionSensorOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}

	//OSA_printf("%s,line:%d ... onPositionSensorOK",__func__,__LINE__);
	return ;
 }


void CProcess021::onPositionSensorERR(LPARAM lParam)
 {
 	isPositionSensorOK = FALSE;
	
 	//OSA_printf("%s,line:%d ... onPositionSensorERR",__func__,__LINE__);
	return ;
 }


void CProcess021::onDisplayErr(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... onDisplayErr",__func__,__LINE__);
	return ;
 }


void CProcess021::onDisplayOK(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... onDisplayOK",__func__,__LINE__);
	return ;
 }


void CProcess021::onJoyStickOK(LPARAM lParam)
 {
	isJoyStickOK = TRUE;
	setJoyStickStat(isJoyStickOK);
	
	if(isBootUpMode()&&isBootUpSelfCheck())
	{	
		if(Is9stateOK())
   		{
   	   		sendCommand(CMD_BOOT_UP_CHECK_COMPLETE);
   		}
		return;
	}
	// update area N 
	OSDCTRL_updateAreaN();
	//OSA_printf("%s,line:%d ... onJoyStickOK",__func__,__LINE__);
	return ;
 }


void CProcess021::onJoyStickErr(LPARAM lParam)
 {	
	isJoyStickOK = FALSE;
	setJoyStickStat(isJoyStickOK);
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		return;
	}
	// update area N 
	OSDCTRL_updateAreaN();

 	//OSA_printf("%s,line:%d ... onJoyStickErr",__func__,__LINE__);
	return ;
 }


void CProcess021::onWeaponCtrlOK(LPARAM lParam)
 {
 	isWeaponCtrlOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
   		{
   	   		MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
   		}
		return;
	}
	//OSDCTRL_updateAreaN();
 	//OSA_printf("%s,line:%d ... onWeaponCtrlOK",__func__,__LINE__);
	return ;
 }

void CProcess021::onWeaponCtrlErr(LPARAM lParam)
{
	isWeaponCtrlOK = FALSE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		return;
	}
	// update area N 
	//OSDCTRL_updateAreaN();
	return ;
}


void CProcess021::onDipAngleSensorOK(LPARAM lParam)
 {
	isDipAngleSensorOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}

	//OSA_printf("%s,line:%d ... onDipAngleSensorOK",__func__,__LINE__);
	return ;
 }


void CProcess021::onDipAngleSensorERR(LPARAM lParam)
 {
 	isDipAngleSensorOK = FALSE;
	
	//OSA_printf("%s,line:%d ... onDipAngleSensorERR",__func__,__LINE__);
	return ;
 }





void CProcess021::onMachineGunSensorOK(LPARAM lParam)
 {
	isMachineGunSensorOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
 	  	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}

	//OSA_printf("%s,line:%d ... onMachineGunSensorOK",__func__,__LINE__);
	return ;
 }



void CProcess021::onMachineGunSensorERR(LPARAM lParam)
 {
 	isMachineGunSensorOK = FALSE;
 	//OSA_printf("%s,line:%d ... onMachineGunSensorERR",__func__,__LINE__);
	return ;
 }



void CProcess021::onMachineGunServoOK(LPARAM lParam)
 {
	isMachineGunServoOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}
	//OSA_printf("%s,line:%d ... onMachineGunServoOK",__func__,__LINE__);
	return ;
 }



void CProcess021::onMachineGunServoERR(LPARAM lParam)
 {
 	isMachineGunServoOK = FALSE;
	
 	//OSA_printf("%s,line:%d ... onMachineGunServoERR",__func__,__LINE__);
	return ;
 }



void CProcess021::onPositionServoOK(LPARAM lParam)
 {
 	isPositionServoOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}
	
 	//OSA_printf("%s,line:%d ... onPositionServoOK",__func__,__LINE__);
	return ;
 }



void CProcess021::onPositionServoERR(LPARAM lParam)
 {
 	isPositionServoOK = FALSE;
	
 	//OSA_printf("%s,line:%d ... onPositionServoERR",__func__,__LINE__);
	return ;
 }



void CProcess021::onGrenadeSensorOK(LPARAM lParam)
 {
	isGrenadeSensorOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}
 	
 	//OSA_printf("%s,line:%d ... onGrenadeSensorOK",__func__,__LINE__);
	return ;
 }



void CProcess021::onGrenadeSensorERR(LPARAM lParam)
 {
 	isGrenadeSensorOK = FALSE;
 	OSA_printf("%s,line:%d ... onGrenadeSensorERR",__func__,__LINE__);
	return ;
 }


void CProcess021::onGrenadeServoOK(LPARAM lParam)
 {
	isGrenadeServoOK = TRUE;
	if(isBootUpMode()&&isBootUpSelfCheck())
	{
		if(Is9stateOK())
	       {
	   	   MSGDRIV_send(CMD_BOOT_UP_CHECK_COMPLETE,0);
	       }
	}
	
 	//OSA_printf("%s,line:%d ... onGrenadeServoOK",__func__,__LINE__);
	return ;
 }


void CProcess021::onGrenadeServoERR(LPARAM lParam)
 {
 	isGrenadeServoOK = FALSE;
	
 	//OSA_printf("%s,line:%d ... onGrenadeServoERR",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_BATTLE(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		gLevel1Mode = MODE_BATTLE;
	}
	else if(isBootUpMode())
	{
		gLevel1Mode = MODE_BATTLE;
	}
	else
	{
		return ;
		//assert(FALSE);
	}

	OSDCTRL_EnterBattleMode();

	//OSA_printf("%s,line:%d ... processCMD_BUTTON_BATTLE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_QUIT(LPARAM lParam)
 {
 
	if(isBootUpMode()&& isBootUpSelfCheckFinished())
	{
		processCMD_EXIT_SELF_CHECK(0);
	}
	else if(!isValidProjectileType())
	{
		processCMD_BULLET_SWITCH1(0);
	}
	else if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			
			return;
		}
		else if(isCalibrationZero()||isCalibrationWeather()||isCalibrationGeneral()||isCalibrationGenPram()||isCalibrationHorizen()||isCalibrationLaser())
		{
			if(isCalibrationZero())
			{
				//saveZeroParam();
			}
			else if(isCalibrationGeneral())
			{
				//saveGeneralParam();
			}
			else if(isCalibrationWeather())
			{
				//saveWeatherParam();
			}
			else if(isCalibrationGenPram())
			{
				switch(gLevel3CalibrationState)
				{
					case Menu_FireView:
						//saveFireViewPram();
						break;
					case Menu_FireCtrl:
						//saveFireCtrlPram();
						break;
					case Menu_ServoX:
						//saveServoXPram();
						break;
					case Menu_ServoY:
						//saveServoYPram();
						break;
					default:
						break;
				}
			}
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
			// update OSDdisplay
			OSDCTRL_CalibMenuShow();
			Posd[eGunType] = GunOsd[getBulletType()-1];
		}
		else if(isCalibrationSave())
		{
			//todo: read flash to mem
			//wait for add the function to read from the file
			//ReadParamsFlash();
			processCMD_EXIT_SELF_CHECK(0);
		}
		SHINE = FALSE;
		OSDCTRL_ItemHide(ShinId);
		ShinId=0;
	}
	else if(isBattleMode()&& isStatBattleAuto()&&isBattleReady())
	{
		enterLevel3CalculatorIdle();
		OSDCTRL_BattleShow();
		//AVTCTRL_setAquire();
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoReady())
	{
		gLevel3CalculatorState = Auto_Idle;
		OSDCTRL_BattleShow();
	}

 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_QUIT",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_UNLOCK(LPARAM lParam)
 {
	if(isBootUpMode()&& isBootUpSelfCheckFinished())
	{
		processCMD_EXIT_SELF_CHECK(0);
	}
	else if(isCalibrationMode())
	{
		if(isCalibrationZero()||isCalibrationWeather()||isCalibrationGeneral())
		{
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
			// update OSDdisplay
			OSDCTRL_CalibMenuShow();
		}
		else if(isCalibrationSave())
		{
			//todo: read flash to mem
		}
		OSDCTRL_NoShine();
	}
	else if(isBattleMode()&& isStatBattleAuto()&&(isBattleReady()||isBattleLoadFiringTable()))
	{
		//OSDCTRL_ItemHide(eDynamicZone);
		enterLevel3CalculatorIdle();
		
	}
	else if(isBattleMode()&& isStatBattleAuto()&&(isAutoReady()||isAutoLoadFiringTable()))
	{
		gLevel3CalculatorState = Auto_Idle;
	}
	else if(isBattleMode()&& isStatBattleAuto())
	{
		if(OSDCTRL_IsOsdDisplay(eDynamicZone))
			OSDCTRL_ItemHide(eDynamicZone);
	}

 
 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_UNLOCK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_UP(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			moveUpXposition();
		}
		else if(isCalibrationZero())
		{
			if(isGrenadeGas())
				return ;
			moveCrossUp();  //move the axis up
		}
		else if(isCalibrationWeather())
		{
			increaseDigitOrSymbolZero();
		}
		else if(isCalibrationGeneral())
		{
			increaseDigitOrSymbolGeneral();
		}
		else if(isCalibrationGenPram())
		{
		//	increaseGenPram();
			switch(gLevel3CalibrationState){
				case Menu_FireView:
					increaseFireViewPram();
					break;
				case Menu_FireCtrl:
					increaseFireCtrlPram();
					break;
				case Menu_ServoX:
					increaseServoXPram();
					break;
				case Menu_ServoY:
					increaseServoYPram();
					break;
				default:
					break;
				}
		}
		else if(isCalibrationSave())
		{
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
			Posd[eSaveYesNo] = SaveYesNoOsd[1];
			OSDCTRL_NoShine();
			// update OSDdisplay
			OSDCTRL_CalibMenuShow();
			moveUpXposition();
		}	
	}
	else if(isBattleMode())
	{
		if(isfixingMeasure && (MEASURETYPE_MANUAL == gMeasureType))
		{
			increaseMeasureDis();
			loadFiringTable_Enter();
		}
	}

 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_UP",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_DOWN(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			moveDownXPosition();
		}
		else if(isCalibrationZero())
		{
			if(isGrenadeGas())
				return ;
			moveCrossDown();
		}
		else if(isCalibrationWeather())
		{
			decreaseDigitOrSymbolZero();
		}
		else if(isCalibrationGeneral())
		{
			decreaseDigitOrSymbolGeneral();
		}
		else if(isCalibrationGenPram())
		{
			switch(gLevel3CalibrationState)
			{
				case Menu_FireView:
					//decreaseFireViewPram();
					break;
				case Menu_FireCtrl:
					//decreaseFireCtrlPram();
					break;
				case Menu_ServoX:
					//decreaseServoXPram();
					break;
				case Menu_ServoY:
					//decreaseServoYPram();
					break;
				default:
					break;
				}
		}
		else if(isCalibrationSave())
		{
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
			Posd[eSaveYesNo] = SaveYesNoOsd[1];
			OSDCTRL_NoShine();
			// update OSDdisplay
			OSDCTRL_CalibMenuShow();
			moveDownXPosition();
		}
	}
	else if(isBattleMode())
	{
		if(isfixingMeasure && (MEASURETYPE_MANUAL == gMeasureType))
		{
			decreaseMeasureDis();
			loadFiringTable_Enter();
		}
	}

	//OSA_printf("%s,line:%d ... processCMD_BUTTON_DOWN",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_LEFT(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			//do nothing
			if(4 == getXPosition())
			{
				//decreaseAdvancedMenu();
			}
		}
		else if(isCalibrationZero())
		{
			if(isGrenadeGas())
				return ;
			moveCrossLeft();
		}
		else if(isCalibrationWeather())
		{
			moveFocusLeft();
		}
		else if(isCalibrationGeneral())
		{
			moveFocusLeftGeneral();
		}
		else if(isCalibrationGenPram())
		{
			switch(gLevel3CalibrationState)
			{
				case Menu_FireView:
					//moveLeftFireViewPram();
					break;
				case Menu_FireCtrl:
					//moveLeftFireCtrlPram();
					break;
				case Menu_ServoX:
					//moveLeftServoXPram();
					break;
				case Menu_ServoY:
					//moveLeftServoYPram();
					break;
				default:
					break;
			}
		}
		else if(isCalibrationSave())
		{
			//changeSaveOption();
		}
	}
	else if(isBattleMode())
	{
		if(isfixingMeasure && (MEASURETYPE_MANUAL == gMeasureType))
		{
			increaseMeasureMul();
		}
	}

	//OSA_printf("%s,line:%d ... processCMD_BUTTON_LEFT",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_RIGHT(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			if(isXatSave())
			{
				Posd[eSaveYesNo] = SaveYesNoOsd[0];
				processCMD_CALIBRATION_SWITCH_TO_SAVE(0);	
			}
			else if(4 == getXPosition())
			{
				//increaseAdvancedMenu();
			}
		}
		else if(isCalibrationZero())
		{
			if(isGrenadeGas())
				return ;
			moveCrossRight();
		}
		else if(isCalibrationWeather())
		{
			moveFocusRight();
		}
		else if(isCalibrationGeneral())
		{
			moveFocusRightGeneral();
		}
		else if(isCalibrationGenPram())
		{
			switch(gLevel3CalibrationState)
			{
				case Menu_FireView:
					//moveRightFireViewPram();
					break;
				case Menu_FireCtrl:
					//moveRightFireCtrlPram();
					break;
				case Menu_ServoX:
					//moveRightServoXPram();
					break;
				case Menu_ServoY:
					//moveRightServoYPram();
					break;
				default:
					break;
				}
		}
		else if(isCalibrationSave())
		{
			//changeSaveOption();
		}
	}
	else if(isBattleMode())
	{
		if(isfixingMeasure && (MEASURETYPE_MANUAL == gMeasureType))
		{
			decreaseMeasureMul();
		}
	}

 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_RIGHT",__func__,__LINE__);
	return ;
 }



void CProcess021::processCMD_BUTTON_ENTER(LPARAM lParam)
{

	if(!ValidateGunType())
		return;

	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			
			int cmdId;//,i;
			// assign cmdId according to XPosition
			cmdId = CMD_CALIBRATION_SWITCH_TO_WEATHER + getXPosition();
			//if((1 == getXPosition())&&(Posd[eCalibMenu_Zero] == CalibZeroOsd[2]))
			//{
			//	return ;
			//}
			
			//MSGDRIV_send(cmdId,0);//msg driv bug,cann't send the second msg
			switch(cmdId)
			{
				case CMD_CALIBRATION_SWITCH_TO_WEATHER :
					processCMD_CALIBRATION_SWITCH_TO_WEATHER(0);
					break;

				case CMD_CALIBRATION_SWITCH_TO_ZERO:
					processCMD_CALIBRATION_SWITCH_TO_ZERO(0);
					break;

				case CMD_CALIBRATION_SWITCH_TO_GENERAL:
					processCMD_CALIBRATION_SWITCH_TO_GENERAL(0);
					break;

				default:
					break;				
			}				
		}
		else if(isCalibrationZero()||isCalibrationWeather()||isCalibrationGeneral()
		||isCalibrationGenPram()||isCalibrationHorizen()||isCalibrationLaser())
		{
			if(isCalibrationZero())
			{
				saveZeroParam();
				//udateMenuItem_Zero_General(getProjectileType());
			}
			else if(isCalibrationGeneral())
			{
				//saveGeneralParam();
				//udateMenuItem_Zero_General(getProjectileType());
			}
			else if(isCalibrationWeather())
			{
				saveWeatherParam();
			}
			else if(isCalibrationGenPram())
			{
				switch(gLevel3CalibrationState)
				{
					case Menu_FireView:
						//saveFireViewPram();
						break;
					case Menu_FireCtrl:
						//saveFireCtrlPram();
						break;
					case Menu_ServoX:
						//saveServoXPram();
						break;
					case Menu_ServoY:
						//saveServoYPram();
						break;
					default:
						break;
				}
			}
			gLevel2CalibrationState = STATE_CALIBRATION_MAIN_MENU;
			OSDCTRL_updateMainMenu(gProjectileType);
			OSDCTRL_NoShine();
			// update OSDdisplay
			OSDCTRL_CalibMenuShow();
			Posd[eGunType] = GunOsd[getBulletType()-1];
		}
		else if(isCalibrationSave())
		{
			if(isAtSaveYes())
			{
				//todo: write data to file
				//add the function that write to the file
				OSDCTRL_ItemShow(eCalibMenu_SaveOK);
				//OSDCTRL_ItemShow(eSaveYesNo);
			}
			else
			{
				//todo: read flash to mem
				//ReadParamsFlash();
				//OSDCTRL_ItemHide(eSaveYesNo);
			}
			OSDCTRL_NoShine();
		}
	}
	else if(isBattleMode())
	{
		if(MEASURETYPE_MANUAL == gMeasureType)
		{
			if(SHINE &&(eMeasureDis == ShinId)){
				OSDCTRL_NoShine();
				isfixingMeasure = FALSE;
				if(0 == DistanceManual)
				{
					Posd[eDynamicZone] = DynamicOsd[6];
					OSDCTRL_ItemShow(eDynamicZone);
					startDynamicTimer();
				}
				else if(isBeyondDistance()/*!isMachineGun()&&(DistanceManual>365)*/)
				{
					Posd[eDynamicZone] = DynamicOsd[5];
					OSDCTRL_ItemShow(eDynamicZone);
					startDynamicTimer();
				}
				else
					OSDCTRL_ItemHide(eDynamicZone);
			}
		}
	}
}


void CProcess021::processCMD_BULLET_SWITCH0(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		if(isCalibrationMainMenu())
		{
			//check XPosition before  udateMenuItem_Zero_General()
			//udateMenuItem_Zero_General();
		}
		/*else if(isCalibrationGeneral())
		{
			gProjectileType = (gProjectileType + 1)%3;
			//todo: \u66f4\u65b0\u8ddd\u79bb\u3001\u7efc\u4fee\u53c2\u6570
		}else if(isCalibrationZero())
		{
			gProjectileType = (gProjectileType + 1)%2;
			//todo: \u66f4\u65b0\u8ddd\u79bb\u3001\u7efc\u4fee\u53c2\u6570

		}*/
	}



	#if 0
		if(isCalibrationMode()||isBattleMode())
		{
			if(gProjectileType <= PROJECTILE_GRENADE_GAS)
				gProjectileTypeBefore = gProjectileType;
			
			gProjectileType =(PROJECTILE_TYPE) ((gProjectileType + 1)%3);
			Posd[eGunType] = GunOsd[gProjectileType];
		
			OSDCTRL_updateMainMenu(gProjectileType);		
			
		}
	#endif
	
 	
 	//OSA_printf("%s,line:%d ... processCMD_BULLET_SWITCH0",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BULLET_SWITCH1(LPARAM lParam)
 {
	if(gProjectileType <= PROJECTILE_GRENADE_GAS)
	gProjectileTypeBefore = gProjectileType;

	gProjectileType = PROJECTILE_BULLET;
	Posd[eGunType] = GunOsd[PROJECTILE_BULLET];
	EnterCMD_BULLET_SWITCH1();

 	//OSA_printf("%s,line:%d ... processCMD_BULLET_SWITCH1",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BULLET_SWITCH2(LPARAM lParam)
 {
	if(gProjectileType <= PROJECTILE_GRENADE_GAS)
		gProjectileTypeBefore = gProjectileType;

	gProjectileType=(PROJECTILE_TYPE)(PROJECTILE_GRENADE_KILL+2);
	Posd[eGunType] = GunOsd[PROJECTILE_GRENADE_KILL+2];

 	//OSA_printf("%s,line:%d ... processCMD_BULLET_SWITCH2",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BULLET_SWITCH3(LPARAM lParam)
 {
 	if(gProjectileType <= PROJECTILE_GRENADE_GAS)
		gProjectileTypeBefore = gProjectileType;

	gProjectileType=(PROJECTILE_TYPE)(PROJECTILE_GRENADE_GAS+2);
	Posd[eGunType] = GunOsd[PROJECTILE_GRENADE_GAS+2];
	
 	//OSA_printf("%s,line:%d ... processCMD_BULLET_SWITCH3",__func__,__LINE__);
	return ;
 }



void CProcess021::processCMD_BUTTON_AUTOCATCH(LPARAM lParam)
 {
	if(isBattleMode()&&isStatBattleAlert())
	{
		AUTOCATCH = !AUTOCATCH;
		if(AUTOCATCH)
		{
		//	AVTCTRL_setAquire();
		//	AVTCTRL_selectMTD(pAvtCtrlObj);
//		//	AVTCTRL_setAtuoTack();
		//	AVTCTRL_regMTT(pAvtCtrlObj);
		}
		else
		{
		//	OSDCTRL_AlgSelect();
		//	AVTCTRL_setAquire();
		//	AVTCTRL_regCancelMTT(pAvtCtrlObj);
		}
	}else if(isBattleMode()&&isStatBattleAuto()){	//�Զ������л����Զ�װ�?�Զ�����
		if(isBattleIdle()){
			gLevel3CalculatorState = Auto_Idle;
			OSDCTRL_BattleShow();
		}else if(isAutoIdle()){
			gLevel3CalculatorState = Battle_Idle;
			OSDCTRL_BattleTrackShow();
		}
	}

	//OSA_printf("%s,line:%d ... processCMD_BUTTON_AUTOCATCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_BATTLE_AUTO(LPARAM lParam)
 {
 	if(isBattleMode())
	{
		gLevel2BattleState = STATE_BATTLE_AUTO;
		gLevel3CalculatorState = Auto_Idle;
		OSDCTRL_EnterBattleMode();
		Posd[eDynamicZone] = DynamicOsd[0];
		OSDCTRL_ItemShow(eDynamicZone);
	}
	
 	//OSA_printf("%s,line:%d ... processCMD_BUTTON_BATTLE_AUTO",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_BUTTON_BATTLE_ALERT(LPARAM lParam)
 {
	if(isBattleMode())
	{
		gLevel2BattleState = STATE_BATTLE_ALERT;
		OSDCTRL_EnterBattleMode();
	}

	//OSA_printf("%s,line:%d ... processCMD_BUTTON_BATTLE_ALERT",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_USER_FIRED(LPARAM lParam)
{
	//killRGQtimer();
	if(isBattleMode()&& isStatBattleAuto()&&isBattleReady())
	{
		OSDCTRL_ItemHide(eCorrectionTip);
		if(MEASURETYPE_MANUAL == getMeasureType())
		{
			Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_NGQ];
			OSDCTRL_ItemShow(eCorrectionTip);
			//startRGQtimer();
		}
	}
	else if(isBattleMode()&& isStatBattleAuto()&&isAutoReady())
	{
		gLevel3CalculatorState = Auto_Idle;
		OSDCTRL_ItemHide(eCorrectionTip);
		if(MEASURETYPE_MANUAL == getMeasureType())
		{
			Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_NGQ];
			OSDCTRL_ItemShow(eCorrectionTip);
			//startRGQtimer();
		}

	}
	else 
	{
		Posd[eCorrectionTip] = AngleCorrectOsd[CORRECTION_NGQ];
		OSDCTRL_ItemShow(eCorrectionTip);
		//startRGQtimer();
	}

	return ;
}


void CProcess021::processCMD_DETEND_LOCK(LPARAM lParam)
 {
 	isDetendClose=TRUE;
	OSDCTRL_updateAreaN();
 	//OSA_printf("%s,line:%d ... processCMD_DETEND_LOCK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_DETEND_UNLOCK(LPARAM lParam)
 {
 	isDetendClose=FALSE;
	OSDCTRL_updateAreaN();
 	//OSA_printf("%s,line:%d ... processCMD_DETEND_UNLOCK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MAINTPORT_LOCK(LPARAM lParam)
 {
 	isDetendClose=FALSE;
	OSDCTRL_updateAreaN();
 	//OSA_printf("%s,line:%d ... processCMD_MAINTPORT_LOCK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MAINTPORT_UNLOCK(LPARAM lParam)
 {
 	isDetendClose=FALSE;
	OSDCTRL_updateAreaN();
 	//OSA_printf("%s,line:%d ... processCMD_MAINTPORT_UNLOCK",__func__,__LINE__);
	return ;
 }



void CProcess021::processCMD_MEASURE_DISTANCE_SWITCH(LPARAM lParam)
 {
	if(isBattleMode()&&isStatBattleAuto())
	{
		gMeasureType =(DIS_MEASURE_TYPE)(MEASURETYPE_MANUAL - gMeasureType);
		Posd[eMeasureType] = MeasureTypeOsd[gMeasureType];
		if(MEASURETYPE_MANUAL == gMeasureType)
		{
			OSDCTRL_ItemShine(eMeasureDis);
			isfixingMeasure = TRUE;
		}
		else
		{
			SHINE = FALSE;
			ShinId = 0;
			isfixingMeasure = FALSE;
		}
	}
 	
 	//OSA_printf("%s,line:%d ... processCMD_MEASURE_DISTANCE_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_SAVE(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		gLevel2CalibrationState = STATE_CALIBRATION_SAVE;
		//update OSDdisplay
		OSDCTRL_EnterCalibMode();
	}

 	//OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_SAVE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_ZERO(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		gLevel2CalibrationState = STATE_CALIBRATION_ZERO;
		// update OSDdisplay
	//	initilZeroParam(gProjectileType);
		OSDCTRL_EnterCalibMode();
	}

 	//OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_ZERO",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_WEATHER(LPARAM lParam)
 {
 	if(isCalibrationMode())
	{
		gLevel2CalibrationState = STATE_CALIBRATION_WEATHER;
		//update OSDdisplay
		initilWeatherParam();
		OSDCTRL_ItemShine(eCalibWeather_TepPole);
		OSDCTRL_EnterCalibMode();

	}	
 	//OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_WEATHER",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_GENERAL(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		gLevel2CalibrationState = STATE_CALIBRATION_GENERAL;
		//update OSDdisplay
		initilGeneralParam();
		OSDCTRL_ItemShine(eCalibGeneral_DisValue1);
		OSDCTRL_EnterCalibMode();
	}

	//OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_GENERAL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_GENPRAM(LPARAM lParam)
 {
	if(isCalibrationMode())
	{
		gLevel2CalibrationState = STATE_CALIBRATION_GENPRAM;
		//update OSDdisplay
		switch(gLevel3CalibrationState){
			case Menu_FireView:
				//initilFireViewPram();
				OSDCTRL_ItemShine(eCalibGenPram_VFLDOXValue0);
				break;
			case Menu_FireCtrl:
				//initFireCtrlPram();
				OSDCTRL_ItemShine(eCalibGenPram_TimeValue0);
				break;
			case Menu_ServoX:
				//initServoXPram();
				break;
			case Menu_ServoY:
				//initServoYPram();
				break;
			default:
				break;
		}
		OSDCTRL_EnterCalibMode();
	}
}



void CProcess021::processCMD_CALIBRATION_SWITCH_TO_HORIZEN(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_HORIZEN",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALIBRATION_SWITCH_TO_LASER(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_CALIBRATION_SWITCH_TO_LASER",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_LASER_FAIL(LPARAM lParam)
 {
	if(isBattleMode()&&isStatBattleAuto()&&(isBattlePreparation()))
	{
		//enterLevel3CalculatorIdle();
		gLevel3CalculatorState = Battle_Idle;
		Posd[eMeasureType] = MeasureTypeOsd[lParam+3];
		OSDCTRL_ItemShow(eDynamicZone);
		Posd[eDynamicZone] = DynamicOsd[2];
		//startDynamicTimer();

		sendCommand(CMD_QUIT_AVT_TRACKING);
	}
	else if(isBattleMode()&&isStatBattleAuto()&&isAutoPreparation())
	{
		gLevel3CalculatorState = Auto_Idle;
		Posd[eMeasureType] = MeasureTypeOsd[lParam+3];
		OSDCTRL_ItemShow(eDynamicZone);
		Posd[eDynamicZone] = DynamicOsd[2];
		//startDynamicTimer();
	}

 	
 	OSA_printf("%s,line:%d ... processCMD_LASER_FAIL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_TRACKING_FAIL(LPARAM lParam)
 {
 	if(isBattleMode()&&isStatBattleAuto())
	{
		enterLevel3CalculatorIdle();
		releaseServoContrl();
	}
 	//OSA_printf("%s,line:%d ... processCMD_TRACKING_FAIL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_VELOCITY_FAIL(LPARAM lParam)
 {
 	if(isBattleMode()&&isStatBattleAuto())
	{
		Posd[eDynamicZone]=DynamicOsd[3];
		OSDCTRL_ItemShow(eDynamicZone);
		startDynamicTimer();
	}
 	//OSA_printf("%s,line:%d ... processCMD_VELOCITY_FAIL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MEASURE_VELOCITY(LPARAM lParam)
 {

	if(isBattleMode()&& isStatBattleAuto()&& isBattleIdle())
	{
		//trigger get Velocity
		gLevel3CalculatorState = Battle_TriggerMeasureVelocity;
		resetTurretVelocityCounter();
		markMeasure_dist_Time();
		resetDipVelocityCounter();
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoIdle())
	{
		gLevel3CalculatorState = Auto_TriggerMeasureVelocity;
		resetTurretVelocityCounter();
		markMeasure_dist_Time();
		resetDipVelocityCounter();
	}
 
 	//OSA_printf("%s,line:%d ... processCMD_MEASURE_VELOCITY",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MEASURE_DISTANCE(LPARAM lParam)
 {	printf("@@@@@@@@@  \n");	
	if(isBattleMode()&& isStatBattleAuto()&& isBattleTriggerMeasureVelocity())
	{
		if(1||isMeasureManual())   
		{
			if(!isTurretVelocityValid())
			{	
				processCMD_VELOCITY_FAIL(0);
				//sendCommand(CMD_VELOCITY_FAIL);
			}	
			
			Posd[eMeasureType] = MeasureTypeOsd[2];	//LSBG
			OSDCTRL_ItemShine(eMeasureType);
			
			//todo: trigger Laser and AVT
			gLevel3CalculatorState = Battle_Preparation;
			gLevel4subCalculatorState = WaitForFeedBack;
			if(isMeasureManual())
				processCMD_LASER_OK(0);
				//sendCommand(CMD_LASER_OK);
			else
				LaserPORT_requst();

			//sendCommand(CMD_TRIGGER_AVT);
		}
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoTriggerMeasureVelocity())
	{
		//printf("%%%%%%%%%%%%%%%%%%%  \n");
		if(!isTurretVelocityValid())
			processCMD_VELOCITY_FAIL(0);
			//sendCommand(CMD_VELOCITY_FAIL);
		
		Posd[eMeasureType] = MeasureTypeOsd[2];//LSBG
		OSDCTRL_ItemShine(eMeasureType);
		pTimerObj->SetTimer(eAVT_Timer,AVT_TIMER);

		
		//todo: trigger Laser and AVT
		gLevel3CalculatorState = Auto_Preparation;
		if(isMeasureManual())
			processCMD_LASER_OK(0);
			//sendCommand(CMD_LASER_OK);
		else
			LaserPORT_requst();
	}
 	//OSA_printf("%s,line:%d ... processCMD_MEASURE_DISTANCE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_LASER_OK(LPARAM lParam)
 {
	if(isBattleMode()&& isStatBattleAuto()&&isBattlePreparation())
	{
		if(isTrackingOK())
		{
			gLevel3CalculatorState = Battle_LoadFiringTable;
			loadFiringTable();
		}
		else if(isWaitforFeedback())
		{
			killLaserTimer();
			gLevel4subCalculatorState = Laser_OK;			
			//sendCommand(CMD_TRACKING_OK);
			processCMD_TRACKING_OK(0);
		}
		else
		{
			gLevel3CalculatorState = Battle_Idle;
		}
	}
	else if(isBattleMode()&& isStatBattleAuto()&& isAutoPreparation())
	{
		gLevel3CalculatorState = Auto_LoadFiringTable;
		loadFiringTable_Enter();
	}

	//OSA_printf("%s,line:%d ... processCMD_LASER_OK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_TRACKING_OK(LPARAM lParam)
 {
	if(isBattleMode()&& isStatBattleAuto()&&isBattlePreparation())
	{
		if(isLaserOK())
		{
			gLevel3CalculatorState = Battle_LoadFiringTable;
			//\u89e3\u7b97
			loadFiringTable();
		}else if(isWaitforFeedback())
		{
			gLevel4subCalculatorState = TrackingOK;
		}
	}
	
 	OSA_printf("%s,line:%d ... processCMD_TRACKING_OK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_FIRING_TABLE_LOAD_OK(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_FIRING_TABLE_LOAD_OK",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_FIRING_TABLE_FAILURE(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_FIRING_TABLE_FAILURE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_ATTACK_SIGLE(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_ATTACK_SIGLE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_ATTACK_MULTI(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_ATTACK_MULTI",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_FOV_SMALL(LPARAM lParam)
 {
	Posd[eFovType] = FovTypeOsd[1];
	pFovCtrlObj->fovElem=eFov_SmlFov_Stat;

	//OSA_printf("%s,line:%d ... processCMD_MODE_FOV_SMALL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_FOV_LARGE(LPARAM lParam)
 {
 	Posd[eFovType] = FovTypeOsd[0];
	pFovCtrlObj->fovElem=eFov_LarFov_Stat;
	
 	//OSA_printf("%s,line:%d ... processCMD_MODE_FOV_LARGE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_SCALE_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_SCALE_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_PIC_COLOR_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_PIC_COLOR_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_ENHANCE_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_ENHANCE_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_SHOT_SHORT(LPARAM lParam)
 {
 	if(isMachineGun())
	{
		gGunShotType = SHOTTYPE_SHORT;
	}
	
 	//OSA_printf("%s,line:%d ... processCMD_MODE_SHOT_SHORT",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MODE_SHOT_LONG(LPARAM lParam)
 {
 	if(isMachineGun())
	{
		gGunShotType = SHOTTYPE_LONG;
	}
	
 	//OSA_printf("%s,line:%d ... processCMD_MODE_SHOT_LONG",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SCHEDULE_GUN(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SCHEDULE_GUN",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SCHEDULE_STRONG(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SCHEDULE_STRONG",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SCHEDULE_RESET(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SCHEDULE_RESET",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_TIMER_SENDFRAME0(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_TIMER_SENDFRAME0",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_TRACE_SENDFRAME0(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_TRACE_SENDFRAME0",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_TRIGGER_AVT(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_TRIGGER_AVT",__func__,__LINE__);
	return ;
 }



void CProcess021::quitAVTtrigger(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... quitAVTtrigger",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SEND_MIDPARAMS(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SEND_MIDPARAMS",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_GRENADE_LOAD_IN_POSITION(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_GRENADE_LOAD_IN_POSITION",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALCNUM_SHOW(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_CALCNUM_SHOW",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CALCNUM_HIDE(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_CALCNUM_HIDE",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MIDPARAMS_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MIDPARAMS_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_LASERSELECT_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_LASERSELECT_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_STABLEVIDEO_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_STABLEVIDEO_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SENSOR_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SENSOR_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_CONNECT_SWITCH(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_CONNECT_SWITCH",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_IDENTIFY_KILL(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_IDENTIFY_KILL",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_IDENTIFY_GAS(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_IDENTIFY_GAS",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SERVO_INIT(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SERVO_INIT",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_SERVOTIMER_MACHGUN(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_SERVOTIMER_MACHGUN",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_MACHSERVO_MOVESPEED(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MACHSERVO_MOVESPEED",__func__,__LINE__);
	return ;
 }


void CProcess021::processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_GRENADESERVO_MOVESPEED",__func__,__LINE__);
	return ;
 }
void CProcess021::processCMD_MACHSERVO_STOP(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MACHSERVO_STOP",__func__,__LINE__);
	return ;
 }
 

void CProcess021::processCMD_MODE_AIM_LAND(LPARAM lParam)
 {		
 	Posd[eAimType] = AimOsd[0];
	return ;
 }


void CProcess021::processCMD_MODE_AIM_SKY(LPARAM lParam)
 {
 	OSA_printf("%s,line:%d ... processCMD_MODE_AIM_SKY",__func__,__LINE__);
 	Posd[eAimType] = AimOsd[1];
	return ;
 }


void CProcess021::processCMD_LIHEQI_CLOSE(long lParam )
{
	OSA_printf("%s,line:%d ... processCMD_LIHEQI_CLOSE",__func__,__LINE__);
	return ;
}

void CProcess021::processCMD_LIHEQI_OPEN(long lParam )
{
	OSA_printf("%s,line:%d ... processCMD_LIHEQI_OPEN",__func__,__LINE__);
	return ;
}


 void CProcess021::processCMD_GRENADE_ERR(long lParam )
 {
 	OSA_printf("%s,line:%d ... processCMD_GRENADE_ERR",__func__,__LINE__);
 	return ;
 }

 void CProcess021::processCMD_GRENADE_OK(long lParam )
 {
 	OSA_printf("%s,line:%d ... processCMD_GRENADE_OK",__func__,__LINE__);
 	return ;
 }

 void CProcess021::processCMD_FIREBUTTON_ERR(long lParam )
 {
 	OSA_printf("%s,line:%d ... processCMD_FIREBUTTON_ERR",__func__,__LINE__);
 	return ;
 }

 void CProcess021::processCMD_FIREBUTTON_OK(long lParam )
 {
 	OSA_printf("%s,line:%d ... processCMD_FIREBUTTON_OK",__func__,__LINE__);
 	return ;
 }

  void CProcess021::processCMD_FULSCREENCAN_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_FULSCREENCAN_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_FULSCREENCAN_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_FULSCREENCAN_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DISCONTRLCAN0_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DISCONTRLCAN0_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DISCONTRLCAN0_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DISCONTRLCAN0_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DISCONTRLCAN1_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DISCONTRLCAN1_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DISCONTRLCAN1_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DISCONTRLCAN1_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DIANCITIE_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DIANCITIE_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_DIANCITIE_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_DIANCITIE_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_POSMOTOR_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_POSMOTOR_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_POSMOTOR_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_POSMOTOR_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_MACHGUNMOTOR_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_MACHGUNMOTOR_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_MACHGUNMOTOR_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_MACHGUNMOTOR_OK",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_GRENADEMOTOR_ERR(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_GRENADEMOTOR_ERR",__func__,__LINE__);
  	return ;
  }

  void CProcess021::processCMD_GRENADEMOTOR_OK(long lParam )
  {
  	OSA_printf("%s,line:%d ... processCMD_GRENADEMOTOR_OK",__func__,__LINE__);
  	return ;
  }
  

void CProcess021::updateCMD_BUTTON_SWITCH(int param)
{
	if(PROJECTILE_BULLET == param)
	{
		EnterCMD_BULLET_SWITCH1();
	}
	else if(PROJECTILE_GRENADE_KILL== param)
	{
		EnterCMD_BULLET_SWITCH2();
	}
	else if(PROJECTILE_GRENADE_GAS== param)
	{
		EnterCMD_BULLET_SWITCH3();
	}
}

bool CProcess021::ValidateGunType()
{
	if(gProjectileType > PROJECTILE_GRENADE_GAS)
	{
		if(gProjectileType == QUESTION_GRENADE_KILL)
			gProjectileType = PROJECTILE_GRENADE_KILL;
		else if(gProjectileType == QUESTION_GRENADE_GAS)
			gProjectileType = PROJECTILE_GRENADE_GAS;
		//gProjectileType -=2;
		Posd[eGunType] = GunOsd[gProjectileType];
		updateCMD_BUTTON_SWITCH(gProjectileType);
		return FALSE;
	}
	return TRUE;
}

 #endif



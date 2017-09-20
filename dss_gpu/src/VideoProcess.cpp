/*
 * CVideoProcess.cpp
 *
 *  Created on: 2017
 *      Author: sh
 */
#include <glut.h>
#include "VideoProcess.hpp"
#include "vmath.h"
//#include "grpFont.h"
#include "arm_neon.h"

#include "osd_cv.h"

using namespace vmath;

int CVideoProcess::m_mouseEvent = 0;
int CVideoProcess::m_mousex = 0;
int CVideoProcess::m_mousey = 0;
CVideoProcess * CVideoProcess::pThis = NULL;
bool CVideoProcess::m_bTrack = false;
bool CVideoProcess::m_bMtd = false;
bool CVideoProcess::m_bBlobDetect = false;
bool CVideoProcess::m_bMoveDetect = false;
int CVideoProcess::m_iTrackStat = 0;
int64 CVideoProcess::tstart = 0;
static int count=0;


int CVideoProcess::MAIN_threadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainProcThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainProcThrObj.exitProcThread = false;

	mainProcThrObj.initFlag = true;

	mainProcThrObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainProcThrObj.thrHandleProc, mainProcTsk, 0, 0, &mainProcThrObj);
	


	//OSA_waitMsecs(2);

	return iRet;
}

void CVideoProcess::main_proc_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);


	while(mainProcThrObj.exitProcThread ==  false)
	{
	/******************************************/
		
		OSA_semWait(&mainProcThrObj.procNotifySem, OSA_TIMEOUT_FOREVER);

		//OSA_printf("%s:pp = %d ,\n",__func__, mainProcThrObj.pp);
		Mat frame = mainFrame[mainProcThrObj.pp^1];
		bool bTrack = mainProcThrObj.cxt[mainProcThrObj.pp^1].bTrack;
		bool bMtd = mainProcThrObj.cxt[mainProcThrObj.pp^1].bMtd;
		bool bBlobDetect = mainProcThrObj.cxt[mainProcThrObj.pp^1].bBlobDetect;
		bool bMoveDetect = mainProcThrObj.cxt[mainProcThrObj.pp^1].bMoveDetect;
		int chId = mainProcThrObj.cxt[mainProcThrObj.pp^1].chId;
		int iTrackStat = mainProcThrObj.cxt[mainProcThrObj.pp^1].iTrackStat;
		
		int channel = frame.channels();
		Mat frame_gray;


		mainProcThrObj.pp ^=1;
		if(!OnPreProcess(chId, frame))
			continue;


		if(!m_bTrack && !m_bMtd && !m_bBlobDetect && !m_bMoveDetect)
		{
			OnProcess(chId, frame);
			continue;
		}

		if(chId != m_curChId)
			continue;

		frame_gray = Mat(frame.rows, frame.cols, CV_8UC1);

		//OSA_printf("%s:chId = %d , w=%d, h=%d \n",__func__, chId, frame.cols, frame.rows);

		if(channel == 2)
		{
		//	tstart = getTickCount();
			//frame_gray = Mat(frame.rows, frame.cols, CV_8UC1, m_grayMem[0]);
			//cutColor(frame, frame_gray, CV_YUV2GRAY_YUYV);
			extractUYVY2Gray(frame, frame_gray);
			//extractYUYV2Gray(frame, frame_gray);

		//	OSA_printf("chId = %d, YUV2GRAY: time = %f sec \n", chId, ( (getTickCount() - tstart)/getTickFrequency()) );
		}
		else
		{
			memcpy(frame_gray.data, frame.data, frame.cols * frame.rows*channel*sizeof(unsigned char));
		}

		
		if(bTrack)
		{
			iTrackStat = ReAcqTarget();
//			tstart = getTickCount();
			//printf("********x=%d y=%d w=%d h=%d\n",m_rcTrack.x,m_rcTrack.y,m_rcTrack.width,m_rcTrack.height);
		
			//
			if(m_iTrackStat==2)
			{
				//m_searchmod=1;
				#if 0
					switch( m_searchmod)
						{
						case 0:
							UtcSetSerchMode(m_track,SEARCH_MODE_ALL);
							break;
						case 1:
							UtcSetSerchMode(m_track,SEARCH_MODE_LEFT);
							break;
						case 2:
							UtcSetSerchMode(m_track,SEARCH_MODE_RIGHT);
							break;
						default:
							UtcSetSerchMode(m_track,SEARCH_MODE_ALL);
							break;
						}
				#endif

			}
			else
			{
					m_searchmod=0;
			}
			m_iTrackStat = process_track(iTrackStat, frame_gray, m_dc, m_rcTrack);
			//printf("********m_iTrackStat=%d\n",m_iTrackStat);
//			OSA_printf("ALL-Trk: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
		}
		else if(bMtd)
		{
//			tstart = getTickCount();
			if(m_mtd[chId]->state == 0)
			{
				if(OpenTarget(m_mtd[chId], frame.cols, frame.rows) != -1)
				{
					m_mtd[chId]->state = 1;
					printf(" %d:%s chId %d start mtd\n", OSA_getCurTimeInMsec(),__func__, chId);
				}
			}
			process_mtd(m_mtd[chId], frame_gray, frame);
//			OSA_printf("ALL-MTD: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
		}
		else if (bBlobDetect)
		{
//			tstart = getTickCount();
			BlobDetect(frame_gray, adaptiveThred, m_blobRect);
//			OSA_printf("ALL-BlobDetect: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
		}
		else if(bMoveDetect)
		{
			if(m_pMovDetector != NULL)
				m_pMovDetector->setFrame(frame_gray, chId);	
			OSA_assert(chId == 0);
/*			m_pMovDetector->getMoveTarget(detect_vect,0);
			detect_rec.x 		= detect_vect[0].targetRect.x;
			detect_rec.y 		= detect_vect[0].targetRect.y;
			detect_rec.width	= detect_vect[0].targetRect.width;
			detect_rec.height	 = detect_vect[0].targetRect.height;	

			printf(" x  =%d \n", detect_vect[0].targetRect.x);
			printf(" y  =%d \n", detect_vect[0].targetRect.y);
			printf(" width  =%d \n", detect_vect[0].targetRect.width);
			printf(" height  =%d \n", detect_vect[0].targetRect.height);*/
		}

		if(chId != m_curChId)
			continue;
		
		OnProcess(chId, frame);

	/************************* while ********************************/
	}
	OSA_printf("%s: Main Proc Tsk Is Exit...\n",__func__);
}

int CVideoProcess::MAIN_threadDestroy(void)
{
	int iRet = OSA_SOK;

	mainProcThrObj.exitProcThread = true;
	OSA_semSignal(&mainProcThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainProcThrObj.thrHandleProc);

	mainProcThrObj.initFlag = false;
	OSA_semDelete(&mainProcThrObj.procNotifySem);

	return iRet;
}


CVideoProcess::CVideoProcess()
	:m_track(NULL),m_curChId(0),m_curSubChId(-1),adaptiveThred(180)
{
	pThis = this;

	memset(m_mtd, 0, sizeof(m_mtd));
//	memset(m_grayMem, 0, sizeof(m_grayMem));

	memset(&mainProcThrObj, 0, sizeof(MAIN_ProcThrObj));

	m_ImageAxisx=640;
	m_ImageAxisy=512;
	m_intervalFrame = 0;
	m_intervalFrame_change = 0;
	m_bakChId = m_curChId;
	trackchange=0;
	m_searchmod=0;
	m_pMovDetector = NULL;
}

CVideoProcess::~CVideoProcess()
{
	//destroy();
	pThis = NULL;
}

int CVideoProcess::creat()
{
	int i = 0;

/*****************************
		cudaError_t ret = cudaSuccess;
		ret = cudaHostAlloc((void**)&m_grayMem[0], 1920*1080, cudaHostAllocDefault);
		OSA_assert(ret == cudaSuccess);
		ret = cudaHostAlloc((void**)&m_grayMem[1], 1920*1080, cudaHostAllocDefault);
		OSA_assert(ret == cudaSuccess);
/***********************************/

	m_display.create();
	MultiCh.m_user = this;
	MultiCh.m_usrFunc = callback_process;
	MultiCh.creat();

	//BigChannel.creat();

	MAIN_threadCreate();
	

	OSA_mutexCreate(&m_mutex);
	

	OnCreate();

	//mov detect init
	if(m_pMovDetector == NULL)
		m_pMovDetector = MvDetector_Create();
	OSA_assert(m_pMovDetector != NULL);
	
	
	
	return 0;
}

int CVideoProcess::destroy()
{
	stop();
	OSA_mutexDelete(&m_mutex);
	MAIN_threadDestroy();

	//BigChannel.destroy();
	MultiCh.destroy();
	m_display.destroy();

	OnDestroy();
	DeInitMvDetect();

/*	if(m_grayMem[0] != NULL){
			cudaFreeHost(m_grayMem[0]);
			cudaFreeHost(m_grayMem[1]);
			m_grayMem[0] = NULL;
			m_grayMem[1] = NULL;
		}*/

	return 0;
}

void CVideoProcess::mouse_event(int button, int state, int x, int y)
{
	m_mousex = x;
	m_mousey = y;
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
			pThis->OnMouseLeftDwn(x, y);
		else
			pThis->OnMouseLeftUp(x, y);
	}

	if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
			pThis->OnMouseRightDwn(x, y);
		else
			pThis->OnMouseRightUp(x, y);
	}
}

void CVideoProcess::keyboard_event(unsigned char key, int x, int y)
{
	pThis->OnKeyDwn(key);

	if(key == 27){
		pThis->destroy();
		exit(0);
	}
}

void CVideoProcess::keySpecial_event( int key, int x, int y)
{
	pThis->OnKeyDwn((unsigned char)key);
}

void CVideoProcess::visibility_event(int state)
{
	OSA_printf("visibility event: %d\n", state);
}

void CVideoProcess::close_event(void)
{
	OSA_printf("close event\n");
	pThis->destroy();
}

int CVideoProcess::init()
{
	DS_InitPrm dsInit;

	memset(&dsInit, 0, sizeof(DS_InitPrm));
	dsInit.mousefunc = mouse_event;
	dsInit.keyboardfunc = keyboard_event;
	dsInit.keySpecialfunc = keySpecial_event;
	dsInit.timerfunc = call_run;
	//dsInit.idlefunc = call_run;
	dsInit.visibilityfunc = visibility_event;
	dsInit.timerfunc_value = 0;
	dsInit.timerInterval = 20;//ms
	dsInit.closefunc = close_event;
	dsInit.bFullScreen = true;
	dsInit.winPosX = 200;
	dsInit.winPosY = 100;
	dsInit.winWidth = 720;//720;
	dsInit.winHeight = 576;//576;
	
	m_display.init(&dsInit); 

	m_display.m_bOsd = true;//false;//true;
	m_dc = m_display.m_imgOsd[0];
	m_dccv=m_display.m_imgOsd[1];
	OnInit();
	initMvDetect();

	return 0;
}


int CVideoProcess::dynamic_config(int type, int iPrm, void* pPrm)
{
	int iret = 0;

	OSA_mutexLock(&m_mutex);

	if(type < CDisplayer::DS_CFG_Max){
		iret = m_display.dynamic_config((CDisplayer::DS_CFG)type, iPrm, pPrm);
	}

	switch(type)
	{
		case VP_CFG_MainChId:
			m_curChId = iPrm;
			m_iTrackStat = 0;
			mainProcThrObj.bFirst = true;
			m_display.dynamic_config(CDisplayer::DS_CFG_ChId, 0, &m_curChId);
			break;
		case VP_CFG_SubChId:
			m_curSubChId = iPrm;
			m_display.dynamic_config(CDisplayer::DS_CFG_ChId, 1, &m_curSubChId);
			break;
		case VP_CFG_TrkEnable:
			m_bTrack = iPrm;
			m_bakChId = m_curChId;
			m_iTrackStat = 0;
			mainProcThrObj.bFirst = true;
			if(pPrm == NULL)
			{
				UTC_RECT_float rc;
				//rc.x = (int)(m_mousex*frame.cols/m_display.m_mainWinWidth) - 20;
				//rc.y = (int)(m_mousey*frame.rows/m_display.m_mainWinHeight) - 15;
				rc.x = m_ImageAxisx - 30;
				rc.y = m_ImageAxisx - 30;
				rc.width = 60;
				rc.height = 60;
				m_rcTrack = rc;
				m_rcAcq = rc;
				printf("*****************************************************\n");
			}
			else
			{
				m_rcTrack = *(UTC_RECT_float*)pPrm;
				printf("**********************************************@@@@@@@\n");
				//m_rcAcq = *(UTC_RECT_float*)pPrm;
			}
			break;
		case VP_CFG_MtdEnable:
			m_bMtd = iPrm;
			break;
		case VP_CFG_BlobEnable:
			m_bBlobDetect = iPrm;
			break;
		case VP_CFG_MvDetect:
			m_bMoveDetect = iPrm;
			break;
		default:
			break;
	}

	if(iret == 0)
		OnConfig();

	OSA_mutexUnlock(&m_mutex);

	return iret;
}

int CVideoProcess::run()
{
	MultiCh.run();
	//BigChannel.run();
	m_display.run();
	m_track = CreateUtcTrk();

	for(int i=0; i<MAX_CHAN; i++){
		m_mtd[i] = (target_t *)malloc(sizeof(target_t));
		if(m_mtd[i] != NULL)
			memset(m_mtd[i], 0, sizeof(target_t));

		OSA_printf(" %d:%s mtd malloc %p\n", OSA_getCurTimeInMsec(),__func__, m_mtd[0]);
	}

	OnRun();
	
	return 0;
}

int CVideoProcess::stop()
{
	if(m_track != NULL)
		DestroyUtcTrk(m_track);
	m_track = NULL;
	
	for(int i=0; i<MAX_CHAN; i++){
		if(m_mtd[i] != NULL)
		{
			if(m_mtd[i]->state > 0)
				CloseTarget(m_mtd[i]);
			free(m_mtd[i]);
			m_mtd[i] = NULL;
		}
	}
	
	m_display.stop();
	MultiCh.stop();
	//BigChannel.stop();

	OnStop();

	return 0;
}

void CVideoProcess::call_run(int value)
{
	pThis->process_event(0, 0, NULL);
}

void CVideoProcess::process_event(int type, int iPrm, void *pPrm)
{

	Ontimer();
	if(type == 0)//timer event from display
	{
//		MultiCh.process(callback_process, this);	// img alg part
		//process_status();		// stauts part
		//process_osd(0);			// osd part
	}
	
}

int CVideoProcess::callback_process(void *handle, int chId, Mat frame)
{
	CVideoProcess *pThis = (CVideoProcess*)handle;
	return pThis->process_frame(chId, frame);
}

static void extractYUYV2Gray(Mat src, Mat dst)
{
	int ImgHeight, ImgWidth,ImgStride, stride16x8;

	ImgWidth = src.cols;
	ImgHeight = src.rows;
	ImgStride = ImgWidth*2;
	stride16x8 = ImgStride/16;

	OSA_assert((ImgStride&15)==0);
//#pragma omp parallel for
	for(int y = 0; y < ImgHeight; y++)
	{
		uint8x8_t  * __restrict__ pDst8x8_t;
		uint8_t * __restrict__ pSrc8_t;
		pSrc8_t = (uint8_t*)(src.data+ ImgStride*y);
		pDst8x8_t = (uint8x8_t*)(dst.data+ ImgWidth*y);
		for(int x=0; x<stride16x8; x++)
		{
			uint8x8x2_t d;
			d = vld2_u8((uint8_t*)(pSrc8_t+16*x));
			pDst8x8_t[x] = d.val[0];
		}
	}
}

void CVideoProcess::extractUYVY2Gray(Mat src, Mat dst)
{
	int ImgHeight, ImgWidth,ImgStride;

	ImgWidth = src.cols;
	ImgHeight = src.rows;
	ImgStride = ImgWidth*2;
	uint8_t  *  pDst8_t;
	uint8_t *  pSrc8_t;

	pSrc8_t = (uint8_t*)(src.data);
	pDst8_t = (uint8_t*)(dst.data);
//#pragma UNROLL 4
//#pragma omp parallel for
	for(int y = 0; y < ImgHeight*ImgWidth; y++)
	{
		pDst8_t[y] = pSrc8_t[y*2+1];
	}
}

int CVideoProcess::ReAcqTarget()
{
	int iRet = m_iTrackStat;
	if(m_bakChId != m_curChId){
		iRet = 0;
		m_rcTrack = m_rcAcq;
		m_bakChId = m_curChId;
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++1=\n");
	}
	
	if(m_intervalFrame > 0){
		m_intervalFrame--;
		if(m_intervalFrame == 0){
			iRet = 0;
			m_rcTrack = m_rcAcq;
			printf("++++++++++++++++++++++++++++++++++++++++++++++++++2=\n");
		}
	}

	return iRet;

}
extern void cutColor(cv::Mat src, cv::Mat &dst, int code);

#define TM
#undef TM 
int CVideoProcess::process_frame(int chId, Mat frame)
{
	int format = -1;
	if(frame.cols<=0 || frame.rows<=0)
		return 0;

//	tstart = getTickCount();
	int  channel= frame.channels();

#ifdef TM
	cudaEvent_t	start, stop;
		float elapsedTime;
		( (		cudaEventCreate	(	&start)	) );
		( (		cudaEventCreate	(	&stop)	) );
		( (		cudaEventRecord	(	start,	0)	) );
#endif

	if(channel == 2){
//		cvtColor(frame,frame,CV_YUV2BGR_YUYV);
		format = CV_YUV2BGR_UYVY;
	}
	else {
//		cvtColor(frame,frame,CV_GRAY2BGR);
		format = CV_GRAY2BGR;
	}

	OSA_mutexLock(&m_mutex);

	if(chId == m_curChId /*&& (m_bTrack ||m_bMtd )*/)
	{
		mainFrame[mainProcThrObj.pp] = frame;
		mainProcThrObj.cxt[mainProcThrObj.pp].bTrack = m_bTrack;
		mainProcThrObj.cxt[mainProcThrObj.pp].bMtd = m_bMtd;
		mainProcThrObj.cxt[mainProcThrObj.pp].bBlobDetect = m_bBlobDetect;
		mainProcThrObj.cxt[mainProcThrObj.pp].bMoveDetect = m_bMoveDetect;
		mainProcThrObj.cxt[mainProcThrObj.pp].iTrackStat = m_iTrackStat;
		mainProcThrObj.cxt[mainProcThrObj.pp].chId = chId;
		if(mainProcThrObj.bFirst)
		{
			mainFrame[mainProcThrObj.pp^1] = frame;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bTrack = m_bTrack;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bMtd = m_bMtd;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bBlobDetect = m_bBlobDetect;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bMoveDetect = m_bMoveDetect;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].iTrackStat = m_iTrackStat;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].chId = chId;
			mainProcThrObj.bFirst = false;
		}
		OSA_semSignal(&mainProcThrObj.procNotifySem);
	}

#if 0
	Mat frame_gray, frame_tmp;
	//step1:  track process
	if(m_bTrack && chId == m_curChId){
		UTC_RECT_float rc;
		//rc.x = (int)(m_mousex*frame.cols/m_display.m_mainWinWidth) - 20;
		//rc.y = (int)(m_mousey*frame.rows/m_display.m_mainWinHeight) - 15;
		rc.x = (int)(frame.cols/2) - 40;
		rc.y = (int)(frame.rows/2) - 30;
		rc.width = 80;
		rc.height = 60;
		tstart = getTickCount();

		if(channel == 2){
//			tstart = getTickCount();

			frame_gray = Mat(frame.rows, frame.cols, CV_8UC1, m_grayMem[0]);
//			extractYUYV2Gray2(frame, frame_gray);
			cutColor(frame, frame_gray, CV_YUV2GRAY_YUYV);

			OSA_printf("chId = %d, YUV2GRAY: time = %f sec \n", chId, ( (getTickCount() - tstart)/getTickFrequency()) );
		}else if(channel == 1){
			frame_gray = frame;
		}
		m_iTrackStat = process_track(m_iTrackStat, frame_gray, frame, rc);
		OSA_printf("ALL-Trk: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
	}
	else if(m_bMtd && chId == m_curChId)
	{
		tstart = getTickCount();

		if(channel == 2){
//			tstart = getTickCount();
//			frame_tmp = Mat(frame.rows, frame.cols, CV_8UC1, m_grayMem[0]);
			frame_gray = Mat(frame.rows, frame.cols, CV_8UC1);//, m_grayMem[0]);

//			extractYUYV2Gray2(frame, frame_gray);
			cutColor(frame, frame_gray, CV_YUV2GRAY_YUYV);
//			memcpy(frame_gray.data, frame_tmp.data, frame.rows*frame.cols*sizeof(unsigned char));

			OSA_printf("chId = %d, YUV2GRAY: time = %f sec \n", chId, ( (getTickCount() - tstart)/getTickFrequency()) );
		}else if(channel == 1){
			frame_gray = frame;
		}
		if(m_mtd[chId]->state == 0)
		{
			if(OpenTarget(m_mtd[chId], frame.cols, frame.rows) != -1)
			{
				m_mtd[chId]->state = 1;
				printf(" %d:%s chId %d start mtd\n", OSA_getCurTimeInMsec(),__func__, chId);
			}
		}
		process_mtd(m_mtd[chId], frame_gray, frame);
		OSA_printf("ALL-MTD: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
	}

	if(!OnProcess(chId, frame)){
		OSA_mutexUnlock(&m_mutex);
		return 0;
	}

#endif
//to xie tian
	//if(chId == m_curChId || chId == m_curSubChId)
		m_display.display(frame,  chId, format);

	OSA_mutexUnlock(&m_mutex);

//	OSA_printf("process_frame: chId = %d, time = %f sec \n",chId,  ( (getTickCount() - tstart)/getTickFrequency()) );
	//获得结束时间，并显示结果

#ifdef TM
		(	(	cudaEventRecord(	stop,	0	)	)	);
		(	(	cudaEventSynchronize(	stop)	)	);

		(	cudaEventElapsedTime(	&elapsedTime,	start,	stop)	);
		printf("ChId = %d, Time to YUV2RGB:	%3.1f	ms \n", chId, elapsedTime);

		(	(	cudaEventDestroy(	start	)	)	);
		(	(	cudaEventDestroy(	stop	)	)	);
#endif
	return 0;
}

int CVideoProcess::process_track(int trackStatus, Mat frame_gray, Mat frame_dis, UTC_RECT_float &rcResult)
{
	IMG_MAT image;

	image.data_u8 = frame_gray.data;
	image.width = frame_gray.cols;
	image.height = frame_gray.rows;
	image.channels = 1;
	image.step[0] = image.width;
	image.dtype = 0;

	if(trackStatus != 0)
	{
		rcResult = UtcTrkProc(m_track, image, &trackStatus);
	}
	else
	{
		//printf("track********x=%f y=%f w=%f h=%f  ax=%d xy=%d\n",rcResult.x,rcResult.y,rcResult.width,rcResult.height);
		UTC_ACQ_param acq;
		acq.axisX =m_ImageAxisx;// image.width/2;//m_ImageAxisx;//
		acq.axisY =m_ImageAxisy;//image.height/2;//m_ImageAxisy;//
		acq.rcWin.x = (int)(rcResult.x);
		acq.rcWin.y = (int)(rcResult.y);
		acq.rcWin.width = (int)(rcResult.width);
		acq.rcWin.height = (int)(rcResult.height);

		if(acq.rcWin.width<0)
			{
				acq.rcWin.width=0;

			}
		else if(acq.rcWin.width>= image.width)
			{
				acq.rcWin.width=60;
			}
		if(acq.rcWin.height<0)
			{
				acq.rcWin.height=0;

			}
		else if(acq.rcWin.height>= image.height)
			{
				acq.rcWin.height=60;
			}
		if(acq.rcWin.x<0)
			{
				acq.rcWin.x=0;
			}
		else if(acq.rcWin.x>image.width-acq.rcWin.width)
			{

				acq.rcWin.x=image.width-acq.rcWin.width;
			}
		if(acq.rcWin.y<0)
			{
				acq.rcWin.y=0;
			}
		else if(acq.rcWin.y>image.height-acq.rcWin.height)
			{

				acq.rcWin.y=image.height-acq.rcWin.height;
			}
		rcResult = UtcTrkAcq(m_track, image, acq);
		trackStatus = 1;
	}

	return trackStatus;
}

int CVideoProcess::process_mtd(ALGMTD_HANDLE pChPrm, Mat frame_gray, Mat frame_dis)
{
	Mat  medium;
	int i;

//	imshow("Gray", frame_gray);
//	waitKey(1);
	if(pChPrm != NULL && (pChPrm->state > 0))
	{
//		medium.create(frame_gray.rows, frame_gray.cols, frame_gray.type());

//		MediumFliter(frame_gray.data, medium.data, pChPrm->i_width, pChPrm->i_height);

		GaussFliter(frame_gray.data, pChPrm->Img[0], pChPrm->i_width, pChPrm->i_height);

		for(i = 0; i < MAX_SCALER; i++)
		{
			DownSample(pChPrm, pChPrm->Img[i], pChPrm->Img[i+1],
										pChPrm->i_width>>i, pChPrm->i_height>>i);
		}

		IMG_sobel(pChPrm->Img[1], pChPrm->sobel, pChPrm->i_width>>1,	 pChPrm->i_height>>1);

		AutoDetectTarget(pChPrm, frame_gray.data);

		FilterMultiTarget(pChPrm);

	}

	return 0;
}


void	CVideoProcess::initMvDetect()
{
	int	i;
	OSA_printf("%s:mvDetect start ", __func__);
	OSA_assert(m_pMovDetector != NULL);

	m_pMovDetector->init(NotifyFunc, (void*)this);
	
	std::vector<cv::Point> polyWarnRoi ;
	polyWarnRoi.resize(4);
	polyWarnRoi[0]	= cv::Point(200,100);
	polyWarnRoi[1]	= cv::Point(520,100);
	polyWarnRoi[2]	= cv::Point(520,476);
	polyWarnRoi[3]	= cv::Point(200,476);
	for(i=0; i<DETECTOR_NUM; i++){
		m_pMovDetector->setWarningRoi(polyWarnRoi,	i);
		m_pMovDetector->setDrawOSD(m_dccv, i);
		m_pMovDetector->enableSelfDraw(true, i);
		m_pMovDetector->setWarnMode(WARN_MOVEDETECT_MODE, i);
	} 
}  

void	CVideoProcess::DeInitMvDetect()
{
	if(m_pMovDetector != NULL)
		m_pMovDetector->destroy();
}

void CVideoProcess::NotifyFunc(void *context, int chId)
{
	//int num;
	CVideoProcess *pParent = (CVideoProcess*)context;
	pParent->m_display.m_bOsd = true;

	pThis->m_pMovDetector->getMoveTarget(pThis->detect_vect,0);
/*
	num = detect_vect.size();
	for(int i = 0;i<num;i++)
	{
		detect_rec.x 		= detect_vect[i].targetRect.x;
		detect_rec.y 		= detect_vect[i].targetRect.y;
		detect_rec.width	= detect_vect[i].targetRect.width;
		detect_rec.height	= detect_vect[i].targetRect.height;	
		
		DrawjsRect(frame, detect_recBak,0);	
		DrawjsRect(frame, detect_rec,2);
		memcpy(&detect_recBak,&detect_rec,sizeof(UTC_Rect));
	}
*/		
	pParent->m_display.UpDateOsd(1);
}


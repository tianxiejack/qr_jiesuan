/*
 * osd_cv.h
 *
 *  Created on: 2017年5月12日
 *      Author: s123
 */

#ifndef OSD_CV_H_
#define OSD_CV_H_
#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include <osa_sem.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include "app_osdgrp.h"
#include "UtcTrack.h"
using namespace cv;

#include "basePort.h"


typedef struct
{
	int x;
	int y;

}Osd_cvPoint;

void DrawjsCross(Mat frame,CFOV * fovOsdCtrl);
void process_cvosd(Mat frame,void *pPrm);
void Drawcvcross(Mat frame,Line_Param_fb *lineparm);
 CvScalar  GetcvColour(int colour);
 void DrawcvDashcross(Mat frame,Line_Param_fb *lineparm,int linelength,int dashlength);
 void DrawcvDashliner(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int frcolor);
 void drawdashlinepri(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int colour);
 void DrawcvLine(Mat frame,Osd_cvPoint *start,Osd_cvPoint *end,int frcolor,int linew);
void DrawjsRuler(Mat frame,CFOV * fovOsdCtrl);
void DrawjsCompass(Mat frame,CFOV * fovOsdCtrl);
void DrawjsOblinqueLine(Mat frame,Line_Param_fb* lineparm);

void DrawjsXLine(Mat frame,CFOV * fovOsdCtrl);
void Drawjs_leftFrame(Mat frame,CFOV * fovOsdCtrl);
void Drawjs_rightFrame(Mat frame,CFOV * fovOsdCtrl);
void Drawjs_bottomFrame(Mat frame,CFOV * fovOsdCtrl);

void DrawjsGrenadeLoadOK(Mat frame,CFOV * fovOsdCtrl);
void DrawjsAngleFrame(Mat frame,CFOV * fovOsdCtrl,double angle);
void DrawjsAlertFrame(Mat frame,CFOV * fovOsdCtrl);

void DrawjsRightFrame(Mat frame,CFOV * fovOsdCtrl);
void DrawjsLeftFrame(Mat frame,CFOV * fovOsdCtrl);
void DrawjsBottomFrame(Mat frame,CFOV * fovOsdCtrl);

void DrawjsRect(Mat frame,UTC_Rect rec,int frcolor);
void drawcvrect(Mat frame,int x,int y,int width,int height,int frcolor);

#endif /* OSD_CV_H_ */

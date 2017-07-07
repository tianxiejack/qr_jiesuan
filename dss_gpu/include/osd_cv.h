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
using namespace cv;



typedef struct
{
	int x;
	int y;

}Osd_cvPoint;

void process_cvosd(Mat frame,void *pPrm);
void Drawcvcross(Mat frame,Line_Param_fb *lineparm);
 CvScalar  GetcvColour(int colour);
 void DrawcvDashcross(Mat frame,Line_Param_fb *lineparm,int linelength,int dashlength);
 void DrawcvDashliner(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int frcolor);
 void drawdashlinepri(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int colour);
 void DrawcvLine(Mat frame,Osd_cvPoint *start,Osd_cvPoint *end,int frcolor,int linew);

#endif /* OSD_CV_H_ */

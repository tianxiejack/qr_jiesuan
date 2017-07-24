#ifndef  __OSD_GRAPH_H__
#define __OSD_GRAPH_H__
#include "app_osdgrp.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osdProcess.h"
using namespace std;
using namespace cv;
extern Multich_graphic grpxChWinPrms;
void  DrawLine(Mat frame, int startx, int starty, int endx, int endy, int width, UInt32 colorRGBA);
void DrawHLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
void DrawVLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
void DrawString(Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor);
void osd_draw_cross(Mat frame, void *prm);
void osd_draw_rect(Mat frame, void *prm);
void osd_draw_rect_gap(Mat frame, void *prm);
void osd_draw_text(Mat frame, void *prm);
void osd_draw_cross_black_white(Mat frame, void *prm);

int osd_draw_Hori_Menu(Mat frame, void *prm);
int osd_draw_Vert_Menu(Mat frame, void *prm);

int process_draw_line(Mat frame,int startx,int starty,int endx,int linewidth,char R,char G,char B,char A);

int process_draw_text(Mat frame,int startx,int starty,char *pstring,int frcolor,int bgcolor);

void Draw_graph_osd(Mat frame, void *tParam,void *lParam);



void EraseDraw_graph_osd(Mat frame, void *Param,void *Parampri);
void osd_chtext(Mat frame, int startx, int starty, char * pString, UInt32 frcolor, UInt32 bgcolor);
typedef void (* osdprocess_CB)(void *pPrm);
void  osdgraph_init(osdprocess_CB fun,Mat frame);
//void CVideoProcess::process_osd(int devId)
void MultichGrpx_update_sem_post(void);
void  APP_tracker_timer_alarm();
#define SHMEM_LEN 128


#define GET_GRAPHIC_ID(osd_id) ((osd_id &0x000000FF) )
#define GET_WINDOWS_ID(osd_id) ((osd_id &0x0000FF00)>>8)
#define GET_SENSORS_ID(osd_id) ((osd_id &0xFFFF0000)>>16)

void APP_graphic_timer_alarm();
Int32 APP_get_colour( int sensorid);

void  set_graph();
void OSDCTRL_draw_text(Mat frame,OSDCTRL_Handle pCtrlObj);


#endif

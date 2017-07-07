/*
 * process021.hpp
 *
 *  Created on: May 5, 2017
 *      Author: ubuntu
 */

#ifndef PROCESS021_HPP_
#define PROCESS021_HPP_

#include "VideoProcess.hpp"
#include "osd_graph.h"
#include"osd_cv.h"
class CProcess021 : public CVideoProcess
{
	UTC_RECT_float rcTrackBak;
	TARGET tgBak[MAX_TARGET_NUMBER];
	BlobRect blob_rectBak;
	Osd_cvPoint crossBak;
	Osd_cvPoint freezecrossBak;
	Osd_cvPoint crosspicpBak;
	Osd_cvPoint rectfovBak;
	Osd_cvPoint secBak[2];
	DS_Rect rendpos[4];
	int Osdflag[20];
	int osdindex;
	int Mmtsendtime;
	int prisensorstatus;
	int Fovpri[2];
public:
	CProcess021();
	~CProcess021();

	void OnCreate();
	void OnDestroy();
	void OnInit();
	void OnConfig();
	void OnRun();
	void OnStop();
	void Ontimer();
	bool OnPreProcess(int chId, Mat &frame);
	bool OnProcess(int chId, Mat &frame);
	void OnMouseLeftDwn(int x, int y);
	void OnMouseLeftUp(int x, int y);
	void OnMouseRightDwn(int x, int y);
	void OnMouseRightUp(int x, int y);
	void OnKeyDwn(unsigned char key);

	CMD_EXT extInCtrl;

	static CProcess021 *sThis;
	static void process_osd(void *pPrm);
	void process_osd_test(void *pPrm);

protected:
	void msgdriv_event(MSG_PROC_ID msgId, void *prm);
	void osd_mtd_show(TARGET tg[], bool bShow = true);
	void DrawBlob(BlobRect blobRct, bool bShow = true);
	void DrawCross(int x,int y,int fcolour , bool bShow = true);
	void drawmmt(TARGET tg[], bool bShow = true);
	void drawmmtnew(TARGET tg[], bool bShow = true);
	void erassdrawmmt(TARGET tg[], bool bShow = true);
	void erassdrawmmtnew(TARGET tg[], bool bShow = true);
	void DrawdashCross(int x,int y,int fcolour , bool bShow = true);
	void DrawMeanuCross(int x,int y,int fcolour , bool bShow = true);
	int  PiexltoWindowsx(int x,int channel);
	int  PiexltoWindowsy(int y,int channel);
	int  WindowstoPiexlx(int x,int channel);
	int  WindowstoPiexly(int y,int channel);


	 static int  MSGAPI_initial(void);
	 static void MSGAPI_init_device(long lParam );
	 static void MSGAPI_inputsensor(long lParam );
	 static void MSGAPI_picp(long lParam );
	 static void MSGAPI_inputtrack(long lParam );
	 static void MSGAPI_inpumtd(long lParam );
	 static void MSGAPI_inpuenhance(long lParam );
	 static void MSGAPI_inputbdt(long lParam );
	 static void MSGAPI_inputzoom(long lParam );
	 static void  MSGAPI_setAimRefine(long lParam          /*=NULL*/);
	 static void MSGAPI_inputfrezz(long lParam );
	 static void MSGAPI_inputmmtselect(long lParam );
	 static void MSGAPI_croppicp(long lParam );
	 static void MSGAPI_inpumtdSelect(long lParam );
	 static void MSGAPI_inputpositon(long lParam );
	 static void MSGAPI_inputcoast(long lParam );
	 static void MSGAPI_inputfovselect(long lParam );
	 static void MSGAPI_inputfovchange(long lParam );
	 static void MSGAPI_inputsearchmod(long lParam );
	 static void MSGAPI_inputvideotect(long lParam );

private:
	ACK_EXT extOutAck;
	bool     m_bCast;
	UInt32 m_castTm;
	//Multich_graphic grpxChWinPrms;

	int tvcorx;
	int tvcory;

	void process_status(void);

	void osd_init(void);
	void process_osd(int devId);
	void DrawLine(Mat frame, int startx, int starty, int endx, int endy, int width, UInt32 colorRGBA);
	void DrawHLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
	void DrawVLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
	void DrawChar(Mat frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor);
	void DrawString(Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor);
	void osd_draw_cross(Mat frame, void *prm);
	void osd_draw_rect(Mat frame, void *prm);
	void osd_draw_rect_gap(Mat frame, void *prm);
	void osd_draw_text(Mat frame, void *prm);
	void osd_draw_cross_black_white(Mat frame, void *prm);

	int process_draw_line(Mat frame, int startx, int starty, int endx, int linewidth,char R, char G, char B, char A);
	int process_draw_text(Mat frame,int startx,int starty,char *pstring,int frcolor,int bgcolor);
	int process_draw_instance(Mat frame);
	int draw_circle_display(Mat frame);


	
};



#endif /* PROCESS021_HPP_ */

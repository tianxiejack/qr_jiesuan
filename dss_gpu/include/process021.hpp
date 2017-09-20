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
#include "osdProcess.h"
#include "osdPort.h"

#define LPARAM long



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

	static void MSGAPI_reset_device(LPARAM lParam      /*=NULL*/);
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

	//add plus
	 static void MSGAPI_setSysModeStat(LPARAM lParam);
        static void MSGAPI_osdState(LPARAM lParam);
        static void MSGAPI_sysCheckResult(LPARAM lParam);
        static void MSGAPI_boreRefine(LPARAM lParam);
        static void MSGAPI_sysCheckStart(LPARAM lParam);
        static void MSGAPI_setPicp(LPARAM lParam);
        static void MSGAPI_changePicpMode(LPARAM lParam);
        static void MSGAPI_setWrapMode(LPARAM lParam);
        static void MSGAPI_dispFlirStat(LPARAM lParam);
        static void MSGAPI_saveCorrArix(LPARAM lParam);
        static void MSGAPI_selectNextTarget(LPARAM lParam);
        static void MSGAPI_selectPicpSen(LPARAM lParam);
        static void MSGAPI_boreSightUpdate(LPARAM lParam);
        static void MSGAPI_hideOsd(LPARAM lParam);
        static void MSGAPI_settrackBreakLock(LPARAM lParam);
        static void processCMD_BUTTON_AUTOCHECK(LPARAM lParam);
        static void processCMD_BOOT_UP_CHECK_COMPLETE(LPARAM lParam);
        static void processCMD_EXIT_SELF_CHECK(LPARAM lParam);
        static void processCMD_BUTTON_CALIBRATION(LPARAM lParam);
        static void onPositionSensorOK(LPARAM lParam);
        static void onPositionSensorERR(LPARAM lParam);
        static void onDisplayErr(LPARAM lParam);
        static void onDisplayOK(LPARAM lParam);
        static void onJoyStickOK(LPARAM lParam);
        static void onJoyStickErr(LPARAM lParam);
        static void onWeaponCtrlOK(LPARAM lParam);
	 static void onWeaponCtrlErr(LPARAM lParam);
        static void onDipAngleSensorOK(LPARAM lParam);
        static void onDipAngleSensorERR(LPARAM lParam);
        static void onMachineGunSensorOK(LPARAM lParam);
        static void onMachineGunSensorERR(LPARAM lParam);
        static void onMachineGunServoOK(LPARAM lParam);
        static void onMachineGunServoERR(LPARAM lParam);
        static void onPositionServoOK(LPARAM lParam);
        static void onPositionServoERR(LPARAM lParam);
        static void onGrenadeSensorOK(LPARAM lParam);
        static void onGrenadeSensorERR(LPARAM lParam);
        static void onGrenadeServoOK(LPARAM lParam);
        static void onGrenadeServoERR(LPARAM lParam);
        static void processCMD_BUTTON_BATTLE(LPARAM lParam);
        static void processCMD_BUTTON_QUIT(LPARAM lParam);
        static void processCMD_BUTTON_UNLOCK(LPARAM lParam);
        static void processCMD_BUTTON_UP(LPARAM lParam);
        static void processCMD_BUTTON_DOWN(LPARAM lParam);
        static void processCMD_BUTTON_LEFT(LPARAM lParam);
        static void processCMD_BUTTON_RIGHT(LPARAM lParam);
        static void processCMD_BUTTON_ENTER(LPARAM lParam);
        static void processCMD_BULLET_SWITCH0(LPARAM lParam);
        static void processCMD_BULLET_SWITCH1(LPARAM lParam);
        static void processCMD_BULLET_SWITCH2(LPARAM lParam);
        static void processCMD_BULLET_SWITCH3(LPARAM lParam);
        static void processCMD_BUTTON_AUTOCATCH(LPARAM lParam);
        static void processCMD_BUTTON_BATTLE_AUTO(LPARAM lParam);
        static void processCMD_BUTTON_BATTLE_ALERT(LPARAM lParam);
        static void processCMD_USER_FIRED(LPARAM lParam);
       
        static void processCMD_MEASURE_DISTANCE_SWITCH(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_SAVE(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_ZERO(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_WEATHER(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_GENERAL(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_GENPRAM(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_HORIZEN(LPARAM lParam);
        static void processCMD_CALIBRATION_SWITCH_TO_LASER(LPARAM lParam);
        static void processCMD_LASER_FAIL(LPARAM lParam);
        static void processCMD_TRACKING_FAIL(LPARAM lParam);
        static void processCMD_VELOCITY_FAIL(LPARAM lParam);
        static void processCMD_MEASURE_VELOCITY(LPARAM lParam);
        static void processCMD_MEASURE_DISTANCE(LPARAM lParam);
        static void processCMD_LASER_OK(LPARAM lParam);
        static void processCMD_TRACKING_OK(LPARAM lParam);

        static void processCMD_MODE_FOV_SMALL(LPARAM lParam);
        static void processCMD_MODE_FOV_LARGE(LPARAM lParam);
        static void processCMD_MODE_SCALE_SWITCH(LPARAM lParam);
        static void processCMD_MODE_PIC_COLOR_SWITCH(LPARAM lParam);
        static void processCMD_MODE_ENHANCE_SWITCH(LPARAM lParam);
        static void processCMD_MODE_SHOT_SHORT(LPARAM lParam);
        static void processCMD_MODE_SHOT_LONG(LPARAM lParam);
        static void processCMD_SCHEDULE_STRONG(LPARAM lParam);
        static void processCMD_SCHEDULE_RESET(LPARAM lParam);
        static void processCMD_TRACE_SENDFRAME0(LPARAM lParam);
        static void processCMD_TRIGGER_AVT(LPARAM lParam);
        static void quitAVTtrigger(LPARAM lParam);
        static void processCMD_SEND_MIDPARAMS(LPARAM lParam);
        static void processCMD_GRENADE_LOAD_IN_POSITION(LPARAM lParam);
        static void processCMD_CALCNUM_SHOW(LPARAM lParam);
        static void processCMD_CALCNUM_HIDE(LPARAM lParam);
        static void processCMD_MIDPARAMS_SWITCH(LPARAM lParam);
        static void processCMD_LASERSELECT_SWITCH(LPARAM lParam);
        static void processCMD_STABLEVIDEO_SWITCH(LPARAM lParam);
        static void processCMD_SENSOR_SWITCH(LPARAM lParam);
        static void processCMD_CONNECT_SWITCH(LPARAM lParam);
        static void processCMD_IDENTIFY_KILL(LPARAM lParam);
        static void processCMD_IDENTIFY_GAS(LPARAM lParam);
        static void processCMD_SERVO_INIT(LPARAM lParam);
		
        static void processCMD_MACHSERVO_MOVESPEED(LPARAM lParam);
        static void processCMD_GRENADESERVO_MOVESPEED(LPARAM lParam);
        static void processCMD_MACHSERVO_STOP(LPARAM lParam);
 	 static void processCMD_MODE_AIM_SKY(LPARAM lParam);
        static void processCMD_MODE_AIM_LAND(LPARAM lParam);
	 
        static void processCMD_LIHEQI_CLOSE(LPARAM lParam);
        static void processCMD_LIHEQI_OPEN(LPARAM lParam);
        static void processCMD_GRENADE_ERR(LPARAM lParam);
        static void processCMD_GRENADE_OK(LPARAM lParam);
        static void processCMD_FIREBUTTON_ERR(long lParam );
        static void processCMD_FIREBUTTON_OK(long lParam );
        static void processCMD_FULSCREENCAN_ERR(long lParam );
        static void processCMD_FULSCREENCAN_OK(long lParam );
        static void processCMD_DISCONTRLCAN0_ERR(long lParam );
        static void processCMD_DISCONTRLCAN0_OK(long lParam );
        static void processCMD_DISCONTRLCAN1_ERR(long lParam );
        static void processCMD_DISCONTRLCAN1_OK(long lParam );
        static void processCMD_DIANCITIE_ERR(long lParam );
        static void processCMD_DIANCITIE_OK(long lParam );
        static void processCMD_POSMOTOR_ERR(long lParam );
        static void processCMD_POSMOTOR_OK(long lParam );
        static void processCMD_MACHGUNMOTOR_ERR(long lParam );
        static void processCMD_MACHGUNMOTOR_OK(long lParam );
        static void processCMD_GRENADEMOTOR_ERR(long lParam );
        static void processCMD_GRENADEMOTOR_OK(long lParam );
	 static void updateCMD_BUTTON_SWITCH(int param);	
	 static bool ValidateGunType();

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

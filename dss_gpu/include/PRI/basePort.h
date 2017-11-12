#ifndef _BASEPORT_H_
#define _BASEPORT_H_

#ifdef __cplusplus
extern "C"{
#endif
//#include "sdkDef.h"
#include "app_global.h"
#include "app_status.h"
#include <glut.h>
#include "utilport.h"

#define MAXUARTBUFLEN 512

typedef struct CurrentTheta {
	double theta;
	int msecond;
}C_Thetas;

typedef struct _MuxPotrTran MuxPotrTran;
typedef int(*MuxPortCallBackFun)(BYTE *pData,UINT uLen,int context);
typedef struct _modleCtrl{
	MuxPotrTran * pMux;
	int (*pCtrlSend)(BYTE *pData,UINT uLen,int context);
	int (*pCtrlRecv)(BYTE *pData,UINT uLen,int context);
}ModleCtrl_t;

typedef struct _sdk_rect {
	unsigned short	left;
	unsigned short	top;
	unsigned short	right;
	unsigned short	bottom;
} RECT, *PRECT;

typedef struct _brush{
	int cornerXLen;			// DEFAULT_FOV_XCONERLEN;
	int cornerYLen;			// DEFAULT_FOV_YCONERLEN;
	int boresightLen;		// DEFAULT_FOV_BORESIGHTLEN;
	int midLen;				// DEFAULT_FOV_MIDLEN;
	int lineBord;			// DEFAULT_WIDTH_PIXLS;
	int lineColor;
	RECT rect;
}CBrush;


typedef struct _fov_ctrl
{
	ModleCtrl_t		Interface;	
	unsigned int		segId;
	unsigned int    	bFovDraw;
	unsigned int		fovScal;
	unsigned int		fovSens;
	unsigned int		fovElem;		
	unsigned short	fovX;				
	unsigned short	fovY;				
	unsigned short  	fovW;	
	unsigned short  	fovH;	
	int 				crosswidth;
	int 				crossheight;
	int				theta;
	int 				frcolor;
	int				linew;
	RECT 			fovRect;						
	CBrush 			fovBrush;

	bool fovbig;		// 1--big  0 --small
	bool fovdraw;	// 1-drawing 0-erasing
	int lastStepGap ;  // Ruler step
	int lastLen;		//Ruler Len --1 mil
	unsigned int drawflag ;		
	double last_angle;
/*	

--------drawflag ---------
	DrawjsCross	  		0
	DrawjsRuler 	 		1
	DrawjsCompass		2
	DrawjsLeftFrame		3
	DrawjsRightFrame 		4
	DrawjsBottomFrame	5
	DrawjsAlertFrame		6
	DrawjsAngleFrame		7
	DrawjsLittleCross		8
	DrawjsZeroCross		9
	DrawjsBigFovCross		10
	DrawjsGenCross		11
	
*/	

#if 0
	ModleCtrl_t		Interface;	
	unsigned int		segId;
	unsigned int    	bFovDraw;
	
	unsigned int		fovScal;
	unsigned int		fovSens;			/*   *0___*4___1*	*/
	unsigned int		fovElem;			/*   |	  |	    |	*/	

	unsigned short	fovX;				/*	7*	 =*=  *5	*/
	unsigned short	fovY;				/*   |	  |	    |	*/
	unsigned short  	fovW;				/*   *3___*6___2*	*/
	unsigned short  	fovH;	
	int 				crosswidth;
	int 				crossheight;
	int				theta;
	int 				frcolor;
	int				linew;

	RECT 			fovRect;						
	CBrush 			fovBrush;
#endif
}CFOV,FOVCTRL_OBJ,*FOVCTRL_Handle;



typedef struct _im{
	int nSize;
	int width;                  
	int height;
	int widthStep;
	int imageSize;
	BYTE *imageData;
	BYTE *ImgY;
	BYTE *ImgU;
	BYTE *ImgV;
}IMAGE_OBJ,*IMAGE_Handle,*IMG_Handle;

typedef union _color_yuv{
	UINT yuv;
	BYTE data[4];
}COLOR,*COLOR_Ptr;



typedef struct _font_obj{
	OSDUTIL_Font *font;
	COLOR fgColor; 
	COLOR bgColor;
	unsigned int OrgX; 
	unsigned int OrgY;
	unsigned int cSize;
	unsigned char* pchar;
}FONT_OBJ,*FONT_Handle;


typedef enum _uartType{
	EUART_TESTBAK3_PORT	=0,		
	EUART_TESTBAK2_PORT	=1,	
	EUART_POSITION_PORT	=2,
	EUART_GRENADE_PORT	=3,
	EUART_LASER_PORT	=4,
	EUART_MACHGUN_PORT	=5,
	EUART_TESTBAK1_PORT	=6,
	EUART_TRACE_PORT	=7,	
	EUART_TURRETPOS_PORT=8,	
	EUART_MAINT_PORT	=9,	
	EUART_LOG_PORT 	 	=10,	
	EUART_WEAPON_PORT	=11
}eUartType;

typedef enum _MuxPort{
	Enum_TestBak3Port	=	EUART_TESTBAK3_PORT,		
	Enum_PositionPort	=	EUART_POSITION_PORT,	
	Enum_TracPort		=	EUART_TRACE_PORT,	
	Enum_GrenadePort	=	EUART_GRENADE_PORT,	
	Enum_LaserPort		=	EUART_LASER_PORT,	
	Enum_MachGunPort	=	EUART_MACHGUN_PORT,	
	Enum_TestBak1Port	=	EUART_TESTBAK1_PORT,
	Enum_TestBak2Port	=	EUART_TESTBAK2_PORT,	
	Enum_TurretPosPort	=	EUART_TURRETPOS_PORT,	
	Enum_AVTPort		=	EUART_MAINT_PORT,
	Enum_DRamPort		=	EUART_LOG_PORT,
	Enum_WeaponCtrlPort	=	EUART_WEAPON_PORT
}Enum_MuxPortType;

typedef enum _uartStat{
	eUart_SyncFlag	 =0,
	eUart_MsgHead	 =1,
	eUart_MsgData	 =2
}eUartStat;

typedef struct _UartObj {
	int  uartId;
	int  recvLen;
	eUartStat uartStat;
	//SEM_Handle syncObj;
	BYTE sendBuf[MAXUARTBUFLEN];
	BYTE recvBuf[1024];
}UartObj;

typedef struct _PortInter {
	Enum_MuxPortType PortType;
	BOOL(*OpenPort)();
	void(*ClosePort)();
	int (*Read)();
	int (*Write)(BYTE *pSendBuf,int iLen);
	int (*Control)();
	int (*MuxPortRecv)(Enum_MuxPortType ePortType,BYTE*pBuf,int iLen);
}PortInter_t;

#if 0


typedef enum {
    UARTHW_BAUD_9600           = 195,//163,//326,//160,//192,
    UARTHW_BAUD_115200         = 16,//14,//27,//10,//16,
} UARTHW_Baud;
UARTHW_Baud   abcde;

typedef enum {
    UARTHW_WORD5                         = 0x00,        /* 5-bit */
    UARTHW_WORD6                         = 0x01,       /* 6-bit */
    UARTHW_WORD7                         = 0x02,       /* 7-bit */
    UARTHW_WORD8                         = 0x03        /* 8-bit */
} UARTHW_WordLen;

typedef enum {
    UARTHW_STOP1                       = 0,    /* 1 stop bit */
    UARTHW_STOP2                       = 1     /* 2 stop bits */
} UARTHW_StopBits;

typedef enum {
    UARTHW_DISABLE_PARITY      = 0x0,  /* disable parity checking */
    UARTHW_ODD_PARITY          = 0x2,  /* enable odd parity */
    UARTHW_EVEN_PARITY         = 0x3,  /* enable even parity */
    UARTHW_MARK_PARITY         = 0x5,  /* force mark (1) parity */
    UARTHW_SPACE_PARITY        = 0x7   /* force space (0) parity */
} UARTHW_Parity;
#endif


#ifdef __cplusplus
}
#endif

#endif


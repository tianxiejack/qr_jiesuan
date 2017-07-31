#ifndef _CFOV_H_
#define _CFOV_H_
#include "osdProcess.h"
#ifdef __cplusplus
extern "C"{
#endif

#include "basePort.h"

#define FOV_WIDTH		720
#define FOV_HEIGHT		288

#define TV_LFOV_ARG		11.0
#define TV_MFOV_ARG		3.0
#define TV_NFOV_ARG		0.8
#define TV_EFOV_ARG		0.4

#define FILR_LFOV_ARG	24.0
#define FILR_MFOV_ARG	4.8
#define FILR_NFOV_ARG	1.2
#define FILR_EFOV_ARG	0.6

#define DEFAULT_WIDTH_PIXLS  		2
#define DEFAULT_SYMBOL_COLOR 		0xFF

#define DEFAULT_FOV_BORESIGHTLEN	30
#define DEFAULT_FOV_XCONERLEN		10
#define DEFAULT_FOV_YCONERLEN		10
#define DEFAULT_FOV_MIDLEN			10

#define HORI_VERT_SCAL  (4/3)





void FOVCTRL_destroy(HANDLE hFovCtrl);
FOVCTRL_Handle FOVCTRL_create(UINT Sens,UINT fovElem,UINT x,UINT y);
void FOVCTRL_updateFovRect(HANDLE hFov,char Sens,char fovElem,short x,short y);
void FOVCTRL_updateFovScal(HANDLE hFov,int curScal);
void FOVCTRL_draw(HANDLE hFov);





#ifdef __cplusplus
}
#endif
#endif



#ifndef DISPLAYER_HPP_
#define DISPLAYER_HPP_

#define DS_RENDER_MAX		(9)
#define DS_CHAN_MAX         (4)
#define DS_CUSTREAM_CNT		(4)

#define DS_DC_CNT		(2)

#include <cuda.h>
#include "cuda_runtime_api.h"

using namespace std;
using namespace cv;

typedef struct _ds_size{
	int w;
	int h;
	int c;
}DS_Size;

typedef struct _ds_rect{
	int x;
	int y;
	int w;
	int h;
}DS_Rect;

typedef struct _ds_frect{
	float x;
	float y;
	float w;
	float h;
}DS_fRect;

typedef struct _ds_render
{
	int video_chId;
	DS_Rect displayrect;
	bool bCrop;
	DS_Rect croprect;
	DS_fRect bindrect;
	bool bBind;
	float transform[4][4];
	bool bFreeze;
	bool videodect;
}DS_Render;

typedef struct _ds_init_param{
	bool bFullScreen;
	int winPosX;
	int winPosY;
	int winWidth;
	int winHeight;
	bool bScript;
	char szScriptFile[256];
	int initloyerId;
	//void (*displayfunc)(void);
	void (*mousefunc)(int button, int state, int x, int y);
	//void (*reshapefunc)(int width, int height);
	void (*keyboardfunc)(unsigned char key, int x, int y);
	void (*keySpecialfunc)( int, int, int );
	void (*visibilityfunc)(int state);
	void (*timerfunc)(int value);
	void (*idlefunc)(void);
	void (*closefunc)(void);
	int timerfunc_value;//context
	int timerInterval;//ms
}DS_InitPrm;

class CDisplayer 
{
public:
	CDisplayer();
	~CDisplayer();
	int create();
	int destroy();
	int init(DS_InitPrm *pPrm);
	void run();
	void stop();

	typedef enum{
		DS_CFG_ChId = 0,
		DS_CFG_RenderPosRect,
		DS_CFG_EnhEnable,
		DS_CFG_CropEnable,
		DS_CFG_CropRect,
		DS_CFG_VideoTransMat,
		DS_CFG_ViewTransMat,
		DS_CFG_BindRect,
		DS_CFG_FreezeEnable,
		DS_CFG_VideodetEnable,
		DS_CFG_RenderPosinit,
		DS_CFG_Renderdisplay,
		DS_CFG_Max
	}DS_CFG;

	int dynamic_config(DS_CFG type, int iPrm, void* pPrm);
	int get_videoSize(int chId, DS_Size &size);
	void display(Mat frame, int chId, int code = -1);/*CV_YUV2BGR_UYVY*/
	GLuint async_display(int chId, int width, int height, int channels);
	int setFullScreen(bool bFull);
	void reDisplay(void);
	void UpDateOsd(int idc);

	int m_mainWinWidth;
	int m_mainWinHeight;
	bool m_bRun;
	bool m_bFullScreen;
	bool m_bOsd;
	Mat m_imgOsd[DS_DC_CNT];
	DS_Size m_videoSize[DS_CHAN_MAX];
	GLuint buffId_input[DS_CHAN_MAX];
	bool m_bEnh[DS_CHAN_MAX];
	bool  Videoeable[DS_CHAN_MAX];
	//bool firvideo;
protected:
	DS_InitPrm m_initPrm;
	DS_Render m_renders[DS_RENDER_MAX];
	int m_renderCount;
	Mat m_img[DS_CHAN_MAX];
	Mat m_img_novideo;
	int initRender(bool bInitBind = true);
	void uninitRender();

protected:
	static void _display(void);
	static void _timeFunc(int value);
	static void _reshape(int width, int height);
	static void _close(void);
	void gl_resize(void);

protected:
	GLint	m_glProgram;
	GLfloat m_glvVerts[DS_RENDER_MAX][8];
	GLfloat m_glvTexCoords[DS_RENDER_MAX][8];
	bool m_bUpdateVertex;
	GLfloat m_glmat44fTrans[DS_CHAN_MAX][16];
	GLuint textureId_input[DS_CHAN_MAX];
	//GLuint textureId_osd;//[DS_DC_CNT];

	GLuint textureId_osd[DS_DC_CNT];
	GLuint buffId_osd[DS_DC_CNT];
	unsigned char *dev_pbo_osd[DS_DC_CNT];
	GLboolean updata_osd[DS_DC_CNT];
	
	struct cudaGraphicsResource *cuda_pbo_resource[DS_CHAN_MAX];


	int gl_create();
	void gl_destroy();
	void gl_init();
	void gl_uninit();
	void gl_display();
	void gl_textureLoad();
	int gl_updateVertex();
	void gltLoadShaderSrc(const char *szShaderSrc, GLuint shader);
	bool gltLoadShaderFile(const char *szFile, GLuint shader);
	GLuint gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);

	Mat m_temp;

private:
	OSA_MutexHndl m_mutex;
	//unsigned char *m_dev_src_rgb[DS_CHAN_MAX];
	cudaStream_t m_cuStream[DS_CUSTREAM_CNT];

	cudaEvent_t	m_startEvent, m_stopEvent;
};


#endif /* DISPLAYER_HPP_ */


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>

//#include <gl.h>
#include <glew.h>
#include <glut.h>
#include <freeglut_ext.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include "cuda_runtime_api.h"
#include "osa.h"
#include "osa_mutex.h"
#include "osa_tsk.h"
#include "Displayer.hpp"
#include "cuProcess.hpp"

#include "cuda_mem.cpp"


#ifdef _WIN32
#pragma comment (lib, "glew32.lib")
#endif

static CDisplayer *gThis = NULL;

GLint Uniform_tex_in = -1;
GLint Uniform_tex_osd = -1;
GLint Uniform_tex_pbo = -1;
GLint Uniform_osd_enable = -1;
GLint Uniform_mattrans = -1;
static GLfloat m_glmat44fTransDefault[16] ={
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
static GLfloat m_glmat44fTransDefault2[16] ={
	1.1f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.1f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
static GLfloat m_glvVertsDefault[8] = {-1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f};
static GLfloat m_glvTexCoordsDefault[8] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

CDisplayer::CDisplayer()
:m_mainWinWidth(720),m_mainWinHeight(576),m_renderCount(0),
m_bRun(false),m_bFullScreen(false),m_bOsd(false),
 m_glProgram(0), m_bUpdateVertex(false)
{
	int i;
	gThis = this;
	memset(&m_initPrm, 0, sizeof(m_initPrm));
	memset(cuda_pbo_resource, 0, sizeof(cuda_pbo_resource));
	//memset(m_dev_src_rgb, 0, sizeof(m_dev_src_rgb));
	memset(m_cuStream, 0, sizeof(m_cuStream));
	memset(m_bEnh, 0, sizeof(m_bEnh));
	for(i=0; i<DS_DC_CNT; i++)
		buffId_osd[i] = -1;
	memset(updata_osd, 0, sizeof(updata_osd));
}

CDisplayer::~CDisplayer()
{
	//destroy();
	gThis = NULL;
}

int CDisplayer::create()
{
	int i;
	unsigned char *d_src_rgb_novideo = NULL;
	cudaError_t et;
	memset(m_renders, 0, sizeof(m_renders));
	for(i=0; i<4; i++){
		memcpy(m_renders[i].transform, m_glmat44fTransDefault, sizeof(float)*16);
	}
	for(; i<DS_RENDER_MAX; i++){
		memcpy(m_renders[i].transform, m_glmat44fTransDefault2, sizeof(float)*16);
	}
	memset(m_videoSize, 0, sizeof(m_videoSize));

	for(i=0; i<DS_CHAN_MAX; i++){
		m_img[i].cols = 0;
		m_img[i].rows = 0;
	}
	m_img_novideo.cols=0;
	m_img_novideo.rows=0;

	for(i=0; i<DS_CUSTREAM_CNT; i++){
		et = cudaStreamCreate(&m_cuStream[i]);
		OSA_assert(et == cudaSuccess);
	}

	gl_create();

	for(i=0; i<DS_CHAN_MAX; i++){
		Videoeable[i]=1;
		}
	//tvvideo=1;
	//firvideo=1;

	unsigned int byteCount = m_mainWinWidth * m_mainWinHeight * 3 * sizeof(unsigned char);
	cudaMalloc_share((void**)&d_src_rgb_novideo, byteCount, 5 + DS_CHAN_MAX);
	m_img_novideo= cv::Mat(m_mainWinWidth,m_mainWinHeight, CV_8UC3, d_src_rgb_novideo);

	OSA_mutexCreate(&m_mutex);

	return 0;
}

int CDisplayer::destroy()
{
	int i;
	cudaError_t et;

	stop();

	gl_destroy();

	uninitRender();

	OSA_mutexDelete(&m_mutex);

	for(i=0; i<SHAREARRAY_CNT; i++)
		cudaFree_share(NULL, i);

	for(i=0; i<RESOURCE_CNT; i++){
		cudaResource_unmapBuffer(i);
		cudaResource_UnregisterBuffer(i);
	}

	for(i=0; i<DS_CUSTREAM_CNT; i++){
		if(m_cuStream[i] != NULL){
			et = cudaStreamDestroy(m_cuStream[i]);
			OSA_assert(et == cudaSuccess);
			m_cuStream[i] = NULL;
		}
	}

	return 0;
}

int CDisplayer::initRender(bool bInitBind)
{	
	m_renders[0].video_chId = 0;
	m_renders[0].displayrect.x = 0;
	m_renders[0].displayrect.y = 0;
	m_renders[0].displayrect.w = m_mainWinWidth;
	m_renders[0].displayrect.h = m_mainWinHeight;

	m_renders[1].video_chId = -1;
	m_renders[1].displayrect.x = m_mainWinWidth*2/3;
	m_renders[1].displayrect.y = m_mainWinHeight*2/3;
	m_renders[1].displayrect.w = m_mainWinWidth/3;
	m_renders[1].displayrect.h = m_mainWinHeight/3;

	m_renderCount = 2;

	m_renders[0].croprect.x=0;
	m_renders[0].croprect.y=0;
	m_renders[0].croprect.w=0;
	m_renders[0].croprect.h=0;

	m_renders[1].croprect.x=0;
	m_renders[1].croprect.y=0;
	m_renders[1].croprect.w=0;
	m_renders[1].croprect.h=0;

	m_renders[0].videodect=1;
	m_renders[1].videodect=1;

	for(int chId=0; chId<DS_CHAN_MAX; chId++)
	{
		m_img[chId].cols =0 ;
		m_img[chId].rows=0;
	}
	m_img_novideo.cols=0;
	m_img_novideo.rows=0;
	
	return 0;
}

void CDisplayer::uninitRender()
{
	m_renderCount = 0;
}

void CDisplayer::_display(void)
{
	static unsigned int count = 0;
	if((count & 1) == 1)
		gThis->gl_display();
	else 
		gThis->gl_textureLoad();
	count ++;
}

void CDisplayer::_timeFunc(int value)
{
	if(!gThis->m_bRun){
		return ;
	}
	glutTimerFunc(gThis->m_initPrm.timerInterval, _timeFunc, value);
	gThis->_display();
	//if(gThis->m_initPrm.timerfunc != NULL)
	//	gThis->m_initPrm.timerfunc(value);
}

void CDisplayer::_reshape(int width, int height)
{
	assert(gThis != NULL);
	glViewport(0, 0, width, height);
	gThis->m_mainWinWidth = width;
	gThis->m_mainWinHeight = height;
	gThis->initRender(false);
	gThis->gl_updateVertex();
	gThis->gl_resize();
}
void CDisplayer::gl_resize()
{
	//glGenBuffers(1, pixBuffObjs);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	//glBufferData(GL_PIXEL_PACK_BUFFER,
	//	m_mainWinWidth*m_mainWinHeight*3,
	//	NULL, GL_DYNAMIC_COPY);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void CDisplayer::_close(void)
{
	if(gThis->m_initPrm.closefunc != NULL)
		gThis->m_initPrm.closefunc();
}

int CDisplayer::init(DS_InitPrm *pPrm)
{
	if(pPrm != NULL)
		memcpy(&m_initPrm, pPrm, sizeof(DS_InitPrm));

	if(m_initPrm.winWidth > 0)
		m_mainWinWidth = m_initPrm.winWidth;
	if(m_initPrm.winHeight > 0)
		m_mainWinHeight = m_initPrm.winHeight;

	if(m_initPrm.timerInterval<=0)
		m_initPrm.timerInterval = 40;

    //glutInitWindowPosition(m_initPrm.winPosX, m_initPrm.winPosY);
    glutInitWindowSize(m_mainWinWidth, m_mainWinHeight);
    glutCreateWindow("DSS");
	glutDisplayFunc(&_display);
	//glutFullScreen();
	if(m_initPrm.idlefunc != NULL)
		glutIdleFunc(m_initPrm.idlefunc);
	glutReshapeFunc(_reshape);

	//
	if(m_initPrm.keyboardfunc != NULL)
		glutKeyboardFunc(m_initPrm.keyboardfunc);
	if(m_initPrm.keySpecialfunc != NULL)
		glutSpecialFunc(m_initPrm.keySpecialfunc);

	//mouse event:
	if(m_initPrm.mousefunc != NULL)
		glutMouseFunc(m_initPrm.mousefunc);//GLUT_LEFT_BUTTON GLUT_MIDDLE_BUTTON GLUT_RIGHT_BUTTON; GLUT_DOWN GLUT_UP
	//glutMotionFunc();//button down
	//glutPassiveMotionFunc();//button up
	//glutEntryFunc();//state GLUT_LEFT, GLUT_ENTERED

	if(m_initPrm.visibilityfunc != NULL)
		glutVisibilityFunc(m_initPrm.visibilityfunc);
	if(m_initPrm.bFullScreen){
		glutFullScreen();
		m_bFullScreen = true;
	}

	glutCloseFunc(_close);

	initRender();

	gl_updateVertex();

	gl_init();

	return 0;
}

int CDisplayer::get_videoSize(int chId, DS_Size &size)
{
	if(chId < 0 || chId >= DS_CHAN_MAX)
		return -1;
	size = m_videoSize[chId];

	return 0;
}
int CDisplayer::dynamic_config(DS_CFG type, int iPrm, void* pPrm)
{
	int iRet = 0;
	int chId;
	bool bEnable;
	DS_Rect *rc;
	DS_fRect *frc;
	
	if(type == DS_CFG_ChId){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		chId = *(int*)pPrm;

		m_renders[iPrm].video_chId = chId;
	}
	
	if(type == DS_CFG_RenderPosRect){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		rc = (DS_Rect*)pPrm;
		OSA_mutexLock(&m_mutex);
		m_renders[iPrm].displayrect= *rc;
		OSA_mutexUnlock(&m_mutex);
		printf("the DS_CFG_RenderPosRect x=%d y=%d w=%d h=%d  pIStuts->PicpPosStat=%d\n",m_renders[iPrm].displayrect.x,m_renders[iPrm].displayrect.y,
			m_renders[iPrm].displayrect.w,m_renders[iPrm].displayrect.h,iPrm);
	}




	if(type == DS_CFG_EnhEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		m_bEnh[iPrm] = bEnable;
		printf("the video enhanceiPrm=%d =%d\n",iPrm,m_bEnh[iPrm]);
	}

	if(type == DS_CFG_CropEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		m_renders[iPrm].bCrop = bEnable;
	}

	if(type == DS_CFG_CropRect){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		rc = (DS_Rect*)pPrm;
		m_renders[iPrm].croprect = *rc;

		gl_updateVertex();
	}

	if(type == DS_CFG_VideoTransMat){ //src transform
		if(iPrm >= DS_CHAN_MAX || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		memcpy(m_glmat44fTrans[iPrm], pPrm, sizeof(float)*16);
	}

	if(type == DS_CFG_ViewTransMat){ //dst transform
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		memcpy(m_renders[iPrm].transform , pPrm, sizeof(float)*16);
		gl_updateVertex();
	}

	if(type == DS_CFG_BindRect){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		frc = (DS_fRect*)pPrm;
		m_renders[iPrm].bindrect = *frc;
		//initRender(false);
		gl_updateVertex();
	}

	if(type == DS_CFG_FreezeEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		m_renders[iPrm].bFreeze = bEnable;
	}

	if(type == DS_CFG_VideodetEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		m_renders[iPrm].videodect = bEnable;
		printf("DS_CFG_VideodetEnableiPrm=%d status=%d\n",iPrm,bEnable);
	}

	if(type ==DS_CFG_Renderdisplay)
		{
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		Videoeable[iPrm]=bEnable;
		}

	return iRet;
}

extern "C" int uyvy2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);

extern "C" int yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);
void CDisplayer::display(Mat frame, int chId, int code)
{
	int i;
	unsigned char *d_src = NULL;
	unsigned char *d_src_rgb = NULL;
	
	int nChannel = frame.channels();
	unsigned int byteCount = frame.rows * frame.cols * nChannel * sizeof(unsigned char);

	assert(chId>=0 && chId<DS_CHAN_MAX);

	#if 0
	cv::cvtColor(frame, frame, CV_YUV2BGR_UYVY);
	if(chId==2)
		{
	char wjbuf[12];
	sprintf(wjbuf,"%d",chId);
	cv::imshow(wjbuf,frame);
	cv::waitKey(1);
		}
	return ;
	#endif
	//cv::cvtColor(frame, frame, CV_BGR2BGRA);
	//for(int i=0; i<frame.cols*frame.rows; i++)
	//	frame.data[i*4+3] = 150;
	//frame.copyTo(m_img[chId]);
	//Mat midframe(frame, Range(0, (frame.rows&(~3))), Range(0, (frame.cols&(~3))));
	//midframe.copyTo(frame);

	if(nChannel == 1 || code == -1){

		cudaMalloc_share((void**)&d_src_rgb, byteCount, chId + DS_CHAN_MAX);
		//cudaMalloc_share((void**)&d_src_rgb_novideo, byteCount, 5 + DS_CHAN_MAX);

		OSA_mutexLock(&m_mutex);

		//cudaMemcpy_share(d_src_rgb, frame.data, byteCount, cudaMemcpyHostToDevice, chId, 1);
		cudaMemcpyAsync(d_src_rgb, frame.data, byteCount, cudaMemcpyHostToDevice,  m_cuStream[0]);
//		cudaMemcpyAsync(d_src_rgb+ (byteCount>>1), frame.data+ (byteCount>>1), byteCount>>1, cudaMemcpyHostToDevice,  m_cuStream[5]);

		//m_img[chId] = cv::Mat(frame.rows, frame.cols, CV_MAKETYPE(CV_8U,nChannel), d_src_rgb);
		m_img[chId] = cv::Mat(frame.rows, frame.cols, CV_8UC1, d_src_rgb);

		OSA_mutexUnlock(&m_mutex);

		cudaFree_share(d_src_rgb, chId + DS_CHAN_MAX);
		//cudaFree_share(d_src_rgb_novideo, 5 + DS_CHAN_MAX);
		//frame.copyTo(m_img[chId]);

	}else{

		unsigned int byteCount_rgb = frame.rows * frame.cols * 3* sizeof(unsigned char);
		cudaMalloc_share((void**)&d_src, byteCount, chId);
		cudaMalloc_share((void**)&d_src_rgb, byteCount_rgb, chId + DS_CHAN_MAX);

		OSA_mutexLock(&m_mutex);

		

#if 0
		cudaMemcpyAsync_share(d_src, frame.data, byteCount, cudaMemcpyHostToDevice, chId, 1, m_cuStream[0]);

		if(code == CV_YUV2BGR_UYVY)
			uyvy2bgr_(d_src_rgb, d_src, frame.cols, frame.rows, m_cuStream[0]);
#else

#if 0
		cudaMemcpyAsync(d_src + (byteCount>>2)*0, frame.data + (byteCount>>2)*0, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[0]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*1, frame.data + (byteCount>>2)*1, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[1]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*2, frame.data + (byteCount>>2)*2, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[2]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*3, frame.data + (byteCount>>2)*3, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[3]);

		if(code == CV_YUV2BGR_YUYV){
			yuyv2bgr_(d_src_rgb + (byteCount_rgb>>2)*0, d_src + (byteCount>>2)*0, frame.cols, (frame.rows>>2), m_cuStream[0]);
			yuyv2bgr_(d_src_rgb + (byteCount_rgb>>2)*1, d_src + (byteCount>>2)*1, frame.cols, (frame.rows>>2), m_cuStream[1]);
			yuyv2bgr_(d_src_rgb + (byteCount_rgb>>2)*2, d_src + (byteCount>>2)*2, frame.cols, (frame.rows>>2), m_cuStream[2]);
			yuyv2bgr_(d_src_rgb + (byteCount_rgb>>2)*3, d_src + (byteCount>>2)*3, frame.cols, (frame.rows>>2), m_cuStream[3]);
		}
		if(code == CV_YUV2BGR_UYVY){
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*0, d_src + (byteCount>>2)*0, frame.cols, (frame.rows>>2), m_cuStream[0]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*1, d_src + (byteCount>>2)*1, frame.cols, (frame.rows>>2), m_cuStream[1]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*2, d_src + (byteCount>>2)*2, frame.cols, (frame.rows>>2), m_cuStream[2]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*3, d_src + (byteCount>>2)*3, frame.cols, (frame.rows>>2), m_cuStream[3]);
		}

#endif

		cudaMemcpyAsync(d_src + (byteCount>>2)*0, frame.data + (byteCount>>2)*0, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[0]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*1, frame.data + (byteCount>>2)*1, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[1]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*2, frame.data + (byteCount>>2)*2, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[2]);
		cudaMemcpyAsync(d_src + (byteCount>>2)*3, frame.data + (byteCount>>2)*3, (byteCount>>2), cudaMemcpyHostToDevice, m_cuStream[3]);

		if(code == CV_YUV2BGR_UYVY){
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*0, d_src + (byteCount>>2)*0, frame.cols, (frame.rows>>2), m_cuStream[0]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*1, d_src + (byteCount>>2)*1, frame.cols, (frame.rows>>2), m_cuStream[1]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*2, d_src + (byteCount>>2)*2, frame.cols, (frame.rows>>2), m_cuStream[2]);
			uyvy2bgr_(d_src_rgb + (byteCount_rgb>>2)*3, d_src + (byteCount>>2)*3, frame.cols, (frame.rows>>2), m_cuStream[3]);
		}
		
#endif

		m_img[chId] = cv::Mat(frame.rows, frame.cols, CV_8UC3, d_src_rgb);

		OSA_mutexUnlock(&m_mutex);

		cudaFree_share(d_src, chId);
		cudaFree_share(d_src_rgb, chId + DS_CHAN_MAX);
	}
}

GLuint CDisplayer::async_display(int chId, int width, int height, int channels)
{
	assert(chId>=0 && chId<DS_CHAN_MAX);

	if(m_videoSize[chId].w  == width  && m_videoSize[chId].h == height && m_videoSize[chId].c == channels )
		return buffId_input[chId];

	//OSA_printf("%s: w = %d h = %d (%dx%d) cur %d\n", __FUNCTION__, width, height, m_videoSize[chId].w, m_videoSize[chId].h, buffId_input[chId]);

	if(m_videoSize[chId].w != 0){
		glDeleteBuffers(1, &buffId_input[chId]);
		glGenBuffers(1, &buffId_input[chId]);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffId_input[chId]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*channels, NULL, GL_DYNAMIC_COPY);//GL_STATIC_DRAW);//GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_videoSize[chId].w = width;
	m_videoSize[chId].h = height;
	m_videoSize[chId].c = channels;

	gl_updateVertex();

	//OSA_printf("%s: w = %d h = %d (%dx%d) out %d\n", __FUNCTION__, width, height, m_videoSize[chId].w, m_videoSize[chId].h, buffId_input[chId]);
	return buffId_input[chId];
}

void CDisplayer::run()
{
	m_bRun = true;
	//glutSetOption();
	//glutMainLoopEvent();
	//glutMainLoop();
	glutTimerFunc(0, _timeFunc, m_initPrm.timerfunc_value);
}

void CDisplayer::stop()
{
	m_bRun = false;
}

/*
glutGet(Glenum state);
GLUT_WINDOW_X ... GLUT_SCREEN_WIDTH ... 
int glutDeviceGet(GLenum info);
GLUT_HAS_KEYBOARD GLUT_HAS_MOUSE GLUT_NUM_MOUSE_BUTTONS
*/
int CDisplayer::setFullScreen(bool bFull)
{
	if(bFull)
		glutFullScreen();
	else{

	}
	m_bFullScreen = bFull;

	return 0;
}
void CDisplayer::reDisplay(void)
{
	glutPostRedisplay();
}

void CDisplayer::UpDateOsd(int idc)
{
	if(idc<0 || idc>=DS_DC_CNT )
		return;

	updata_osd[idc] = true;
	//unsigned int byteCount = m_imgOsd[idc].cols*m_imgOsd[idc].rows*m_imgOsd[idc].channels()*sizeof(unsigned char);
	//cudaMemcpy(dev_pbo_osd[idc], m_imgOsd[idc].data, byteCount, cudaMemcpyHostToDevice);
}

/***********************************************************************/



static char glName[] = {"DS_RENDER"}; 

int CDisplayer::gl_create()
{
	char *argv[1] = {glName};
	int argc = 1;
   // GLUT init
    glutInit(&argc, argv);  
	//Double, Use glutSwapBuffers() to show
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
   // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//Single, Use glFlush() to show
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );

	// Blue background
	glClearColor(0.0f, 0.0f, 0.01f, 0.0f );

	cudaEventCreate(&m_startEvent);
	cudaEventCreate(&m_stopEvent);

	return 0;
}
void CDisplayer::gl_destroy()
{
	gl_uninit();
	glutLeaveMainLoop();
	cudaEventDestroy(m_startEvent);
	cudaEventDestroy(m_stopEvent);
}

#define TEXTURE_ROTATE (0)
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
void CDisplayer::gl_init()
{
	int i;

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return;
	}

	m_glProgram = gltLoadShaderPairWithAttributes("Shader.vsh", "Shader.fsh", 2, 
		ATTRIB_VERTEX, "vVertex", ATTRIB_TEXTURE, "vTexCoords");
	//m_glProgram = gltLoadShaderPairWithAttributes("Shader.vsh", "Shader.fsh", 0);

	glGenBuffers(DS_CHAN_MAX, buffId_input);
	glGenTextures(DS_CHAN_MAX, textureId_input);
	for(i=0; i<DS_CHAN_MAX; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureId_input[i]);
		assert(glIsTexture(textureId_input[i]));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST);//GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//GL_NEAREST);//GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//GL_CLAMP);//GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//GL_CLAMP);//GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1280, 1024, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
	}


	glGenBuffers(DS_DC_CNT, buffId_osd);
	glGenTextures(DS_DC_CNT, textureId_osd); 

	for(i=0; i<DS_DC_CNT; i++)
	{
		m_imgOsd[i] = cv::Mat(m_mainWinHeight, m_mainWinWidth, CV_8UC4, cv::Scalar(0,0,0,0));
		glBindTexture(GL_TEXTURE_2D, textureId_osd[i]);
		assert(glIsTexture(textureId_osd[i]));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_imgOsd[i].cols, m_imgOsd[i].rows, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffId_osd[i]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_imgOsd[i].cols*m_imgOsd[i].rows*m_imgOsd[i].cols*m_imgOsd[i].channels(), m_imgOsd[i].data, GL_DYNAMIC_COPY);//GL_STATIC_DRAW);//GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	}
	


	//glGenTextures(1, &textureId_pbo);
	//glBindTexture(GL_TEXTURE_2D, textureId_pbo);
	//assert(glIsTexture(textureId_pbo));
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//m_imgOsd = cv::Mat(1024, 1280, CV_8UC4, cv::Scalar(0,0,0,0));
	/*rectangle( m_imgOsd,
	Point( 600, 200 ),
	Point( 700, 560), 
	cvScalar(0,0,255,255), 2, 8 );
	imshow("dd",m_imgOsd);
	waitKey(5);*/

	for(i=0; i<DS_CHAN_MAX; i++){
		memcpy(m_glmat44fTrans[i], m_glmat44fTransDefault, sizeof(m_glmat44fTransDefault));
	}

	//glGenBuffers(1, pixBuffObjs);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	//glBufferData(GL_PIXEL_PACK_BUFFER,
	//	m_mainWinWidth*m_mainWinHeight*4,
	//	NULL, GL_DYNAMIC_COPY);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glClear(GL_COLOR_BUFFER_BIT);
}

void CDisplayer::gl_uninit()
{
	int i;
	if(m_glProgram != 0)
		glDeleteProgram(m_glProgram);
	m_glProgram = 0;

	glDeleteTextures(DS_CHAN_MAX, textureId_input);
	glDeleteBuffers(DS_CHAN_MAX, buffId_input);
	//glDeleteTextures(1, &textureId_osd); 
	//glDeleteTextures(1, &textureId_pbo);
	//glDeleteBuffers(1, pixBuffObjs);
	glDeleteTextures(DS_DC_CNT, textureId_osd);
	glDeleteBuffers(DS_DC_CNT, buffId_osd);
}

int CDisplayer::gl_updateVertex(void)
{
	int iRet = 0;
	int winId, chId, i;
	DS_Rect rc;
	//GLfloat ftmp;

	for(winId=0; winId<m_renderCount; winId++)
	{
		m_glvVerts[winId][0] = -1.0f; m_glvVerts[winId][1] = 1.0f;
		m_glvVerts[winId][2] = 1.0f; m_glvVerts[winId][3] = 1.0f;
		m_glvVerts[winId][4] = -1.0f; m_glvVerts[winId][5] = -1.0f;
		m_glvVerts[winId][6] = 1.0f; m_glvVerts[winId][7] = -1.0f;

		for(i=0; i<4; i++){
			float x = m_glvVerts[winId][i*2+0];
			float y = m_glvVerts[winId][i*2+1];
			m_glvVerts[winId][i*2+0] = m_renders[winId].transform[0][0] * x + m_renders[winId].transform[0][1] * y + m_renders[winId].transform[0][3];
			m_glvVerts[winId][i*2+1] = m_renders[winId].transform[1][0] * x + m_renders[winId].transform[1][1] * y + m_renders[winId].transform[1][3];
		}

		if(m_renders[winId].bBind){
			GLfloat subw, subh;
			subw = m_glvVerts[winId][2] - m_glvVerts[winId][0];
			subh = m_glvVerts[winId][5] - m_glvVerts[winId][1];
			m_glvVerts[winId][0] += m_renders[winId].bindrect.x*subw;
			m_glvVerts[winId][1] += m_renders[winId].bindrect.y*subh;
			m_glvVerts[winId][2] = m_glvVerts[winId][0] + m_renders[winId].bindrect.w*subw;
			m_glvVerts[winId][3] = m_glvVerts[winId][1];
			m_glvVerts[winId][4] = m_glvVerts[winId][0];
			m_glvVerts[winId][5] = m_glvVerts[winId][1] + m_renders[winId].bindrect.h*subh;
			m_glvVerts[winId][6] = m_glvVerts[winId][0] + m_renders[winId].bindrect.w*subw;
			m_glvVerts[winId][7] = m_glvVerts[winId][1] + m_renders[winId].bindrect.h*subh;
		}

		m_glvTexCoords[winId][0] = 0.0; m_glvTexCoords[winId][1] = 0.0;
		m_glvTexCoords[winId][2] = 1.0; m_glvTexCoords[winId][3] = 0.0;
		m_glvTexCoords[winId][4] = 0.0; m_glvTexCoords[winId][5] = 1.0;
		m_glvTexCoords[winId][6] = 1.0; m_glvTexCoords[winId][7] = 1.0;
	}

	for(winId=0; winId<m_renderCount; winId++)
	{
		chId = m_renders[winId].video_chId;
		if(chId < 0 || chId >= DS_CHAN_MAX)
			continue;
		rc = m_renders[winId].croprect;

		if(m_videoSize[chId].w<=0 || m_videoSize[chId].h<=0){
			iRet ++;
			continue;
		}
		if(rc.w == 0 && rc.h == 0){
			continue;
		}
		//OSA_printf("%s: crop : %d,%d  %dx%d\n", __func__, rc.x, rc.y, rc.w, rc.h);
		//OSA_printf("%s: crop : curvideo %d  %d x %d\n", __func__, chId, m_videoSize[chId].w, m_videoSize[chId].h);
		m_glvTexCoords[winId][0] = (GLfloat)rc.x/m_videoSize[chId].w; 
		m_glvTexCoords[winId][1] = (GLfloat)rc.y/m_videoSize[chId].h;

		m_glvTexCoords[winId][2] = (GLfloat)(rc.x+rc.w)/m_videoSize[chId].w;
		m_glvTexCoords[winId][3] = (GLfloat)rc.y/m_videoSize[chId].h;

		m_glvTexCoords[winId][4] = (GLfloat)rc.x/m_videoSize[chId].w;
		m_glvTexCoords[winId][5] = (GLfloat)(rc.y+rc.h)/m_videoSize[chId].h;

		m_glvTexCoords[winId][6] = (GLfloat)(rc.x+rc.w)/m_videoSize[chId].w;
		m_glvTexCoords[winId][7] = (GLfloat)(rc.y+rc.h)/m_videoSize[chId].h;
	}

	return iRet;
}

static int64 tstart = 0;
static int64 tstartBK = 0;
static float offtime = 0;

void CDisplayer::gl_textureLoad(void)
{
	
	int winId, chId;
	unsigned int mask = 0;
	float elapsedTime;

	tstart = getTickCount();
	if(tstartBK != 0){
		offtime = (tstart - tstartBK)/getTickFrequency();
		//OSA_printf("chId = %d, gl_display offtime: time = %f sec \n", chId,  offtime);
	}
	tstartBK = tstart;
	cudaEventRecord(m_startEvent, 0);

	for(winId=0; winId<m_renderCount; winId++)
	{
		chId = m_renders[winId].video_chId;
		if(chId < 0 || chId >= DS_CHAN_MAX)
			continue;

		if(m_img[chId].cols <=0 || m_img[chId].rows <=0 || m_img[chId].channels() == 0)
			continue;

		//glUniformMatrix4fv(Uniform_mattrans, 1, GL_FALSE, m_glmat44fTrans[chId]);
		//glUniform1i(Uniform_osd_enable, m_bOsd);

		//glUniform1i(Uniform_tex_in, 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureId_input[chId]);

		if(!((mask >> chId)&1))
		{
		///
//			OSA_mutexLock(&m_mutex);
			if(!m_renders[winId].bFreeze)
			{
				//for(int i=0; i<DS_CUSTREAM_CNT; i++)
				//	cudaStreamSynchronize(m_cuStream[i]);

				GLuint pbo = async_display(chId, m_img[chId].cols, m_img[chId].rows, m_img[chId].channels());
				unsigned int byteCount = m_img[chId].cols*m_img[chId].rows*m_img[chId].channels()*sizeof(unsigned char);
				unsigned char *dev_pbo = NULL;
				size_t tmpSize;
				OSA_assert(pbo == buffId_input[chId]);
				cudaResource_RegisterBuffer(chId, pbo, byteCount);
				cudaResource_mapBuffer(chId, (void **)&dev_pbo, &tmpSize);
				if(tmpSize != byteCount)
					{
							printf("tmpSize=%d  byteCount=%d\n",tmpSize,byteCount);

					}
				//assert(tmpSize == byteCount);
				
				if(m_bEnh[chId])
				{
					//m_bEnh[chId^1]=m_bEnh[chId];
					Mat dst = m_img[chId];
					dst.data = dev_pbo;
					cuHistEn( m_img[chId], dst);
				}
				else
				{
				
					//m_bEnh[chId^1]=m_bEnh[chId];
					//printf("wjm_img_novideom_renders[chId=%d winId=%d].videodect=%d\n",chId,winId,m_renders[winId].videodect);
					//if(m_renders[chId].videodect)
					   if(1)
						{	
							cudaMemcpy(dev_pbo, m_img[chId].data, byteCount, cudaMemcpyDeviceToDevice);
						}
					else
						{
							printf("m_img_novideo chid =%d\n",chId);
							cudaMemcpy(dev_pbo, m_img_novideo.data, byteCount, cudaMemcpyDeviceToDevice);
						}

						
				}
				cudaResource_unmapBuffer(chId);
				cudaResource_UnregisterBuffer(chId);
			}

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffId_input[chId]);
			
			glBindTexture(GL_TEXTURE_2D, textureId_input[chId]);
			if(m_img[chId].channels() == 1){
				glTexImage2D(GL_TEXTURE_2D, 0, m_videoSize[chId].c, m_videoSize[chId].w, m_videoSize[chId].h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
				//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_videoSize[chId].w, m_videoSize[chId].h, GL_RED, GL_UNSIGNED_BYTE, NULL);
			}else{
				//glTexImage2D(GL_TEXTURE_2D, 0, m_videoSize[chId].c, m_videoSize[chId].w, m_videoSize[chId].h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_videoSize[chId].w, m_videoSize[chId].h, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
			}
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

//			OSA_mutexUnlock(&m_mutex);
			///
			mask |= (1<<chId);
		}
	}

	if(m_bOsd)
	{
		for(int i=0; i<DS_DC_CNT;  i++)
		{
			if(updata_osd[i])
				{
				glBindTexture(GL_TEXTURE_2D, textureId_osd[i]);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_imgOsd[i].cols, m_imgOsd[i].rows, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_imgOsd[i].data);
				updata_osd[i] = false;
			}			
		}
		
	}

	cudaEventRecord(m_stopEvent, 0);
	cudaEventSynchronize(m_stopEvent);
	cudaEventElapsedTime(	&elapsedTime, m_startEvent, m_stopEvent);
	//if(elapsedTime > 5.0f)
	//	OSA_printf("%s: -------elapsed %.3f ms.\n", __func__, elapsedTime);

	float telapse = ( (getTickCount() - tstart)/getTickFrequency());
}

void CDisplayer::gl_display(void)
{	
	int winId, chId;
	unsigned int mask = 0;

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_glProgram);
	
	Uniform_tex_in = glGetUniformLocation(m_glProgram, "tex_in");
	//Uniform_tex_osd = glGetUniformLocation(m_glProgram, "tex_osd");
	//Uniform_tex_pbo = glGetUniformLocation(m_glProgram, "tex_pbo");
	//Uniform_osd_enable = glGetUniformLocation(m_glProgram, "bOsd");
	Uniform_mattrans = glGetUniformLocation(m_glProgram, "mTrans");


	for(winId=0; winId<m_renderCount; winId++)
	{
		chId = m_renders[winId].video_chId;
		if(chId < 0 || chId >= DS_CHAN_MAX)
			continue;

		if(m_img[chId].cols <=0 || m_img[chId].rows <=0 || m_img[chId].channels() == 0)
			continue;

		glUniformMatrix4fv(Uniform_mattrans, 1, GL_FALSE, m_glmat44fTrans[chId]);
		//glUniform1i(Uniform_osd_enable, m_bOsd);

		glUniform1i(Uniform_tex_in, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId_input[chId]);

		glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, m_glvVerts[winId]);
		glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, m_glvTexCoords[winId]);
		glEnableVertexAttribArray(ATTRIB_VERTEX);
		glEnableVertexAttribArray(ATTRIB_TEXTURE);

		//glEnable(GL_MULTISAMPLE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//OSA_mutexLock(&m_mutex);
		#if 1
		glViewport(m_renders[winId].displayrect.x,
				m_renders[winId].displayrect.y,
				m_renders[winId].displayrect.w, m_renders[winId].displayrect.h);
		

		#endif
		//OSA_mutexUnlock(&m_mutex);

		#if 0
		if(chId==0)
			{ 
			if(Videoeable[chId])
			   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		else if(chId==1)
			{
			if(Videoeable[chId])
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		else
			{
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			}
		#endif
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		//glDisable(GL_MULTISAMPLE);
		//glDisable(GL_BLEND);
	}

	if(m_bOsd)
	{
		glUniformMatrix4fv(Uniform_mattrans, 1, GL_FALSE, m_glmat44fTransDefault);

		glUniform1i(Uniform_tex_in, 0);
		glActiveTexture(GL_TEXTURE0);

		glEnable(GL_BLEND);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for(int i=0; i<DS_DC_CNT; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureId_osd[i]);
			glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, m_glvVertsDefault);
			glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, m_glvTexCoordsDefault);
			glEnableVertexAttribArray(ATTRIB_VERTEX);
			glEnableVertexAttribArray(ATTRIB_TEXTURE);

			glViewport(0, 0, m_mainWinWidth, m_mainWinHeight);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		//glEnable(GL_MULTISAMPLE);
	
		//glDisable(GL_MULTISAMPLE);
		glDisable(GL_BLEND);
	}

	//glValidateProgram(m_glProgram);
	glutSwapBuffers();
	//glFlush();
}


//////////////////////////////////////////////////////////////////////////
// Load the shader from the source text
void CDisplayer::gltLoadShaderSrc(const char *szShaderSrc, GLuint shader)
{
	GLchar *fsStringPtr[1];

	fsStringPtr[0] = (GLchar *)szShaderSrc;
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);
}

#define MAX_SHADER_LENGTH   8192
static GLubyte shaderText[MAX_SHADER_LENGTH];
////////////////////////////////////////////////////////////////
// Load the shader from the specified file. Returns false if the
// shader could not be loaded
bool CDisplayer::gltLoadShaderFile(const char *szFile, GLuint shader)
{
	GLint shaderLength = 0;
	FILE *fp;

	// Open the shader file
	fp = fopen(szFile, "r");
	if(fp != NULL)
	{
		// See how long the file is
		while (fgetc(fp) != EOF)
			shaderLength++;

		// Allocate a block of memory to send in the shader
		assert(shaderLength < MAX_SHADER_LENGTH);   // make me bigger!
		if(shaderLength > MAX_SHADER_LENGTH)
		{
			fclose(fp);
			return false;
		}

		// Go back to beginning of file
		rewind(fp);

		// Read the whole file in
		if (shaderText != NULL){
			size_t ret = fread(shaderText, 1, (size_t)shaderLength, fp);
			OSA_assert(ret == shaderLength);
		}

		// Make sure it is null terminated and close the file
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	else
		return false;    

	// Load the string
	gltLoadShaderSrc((const char *)shaderText, shader);

	return true;
}   


/////////////////////////////////////////////////////////////////
// Load a pair of shaders, compile, and link together. Specify the complete
// source text for each shader. After the shader names, specify the number
// of attributes, followed by the index and attribute name of each attribute
GLuint CDisplayer::gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...)
{
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader; 
	GLuint hReturn = 0;   
	GLint testVal;

	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load them. If fail clean up and return null
	// Vertex Program
	if(gltLoadShaderFile(szVertexProg, hVertexShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		fprintf(stderr, "The shader at %s could ot be found.\n", szVertexProg);
		return (GLuint)NULL;
	}

	// Fragment Program
	if(gltLoadShaderFile(szFragmentProg, hFragmentShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		fprintf(stderr,"The shader at %s  could not be found.\n", szFragmentProg);
		return (GLuint)NULL;
	}

	// Compile them both
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for errors in vertex shader
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hVertexShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szVertexProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	// Check for errors in fragment shader
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hFragmentShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szFragmentProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	// Create the final program object, and attach the shaders
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);


	// Now, we need to bind the attribute names to their specific locations
	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProg);

	// Iterate over this argument list
	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
	{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(hReturn, index, szNextArg);
	}
	va_end(attributeList);

	// Attempt to link    
	glLinkProgram(hReturn);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);  

	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetProgramInfoLog(hReturn, 1024, NULL, infoLog);
		fprintf(stderr,"The programs %s and %s failed to link with the following errors:\n%s\n",
			szVertexProg, szFragmentProg, infoLog);
		glDeleteProgram(hReturn);
		return (GLuint)NULL;
	}

	// All done, return our ready to use shader program
	return hReturn;  
}   

/*
 * cuProcess.cpp
 *
 *  Created on: May 11, 2017
 *      Author: ubuntu
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "cuProcess.hpp"
#include "cuda_mem.cpp"
#include "ENHIST/histogram_common.h"
#include "math.h"

using namespace std;
using namespace cv;

static cudaError_t et;

CCudaProcess::CCudaProcess()
{
	int i;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	for(i=0; i<CUSTREAM_CNT; i++){
		et = cudaStreamCreate(&m_cuStream[i]);
		OSA_assert(et == cudaSuccess);
	}
	initHistogram256();
	cudaMalloc((void **)&d_Histogram, HISTOGRAM256_BIN_COUNT * sizeof(uint));
}

CCudaProcess::~CCudaProcess()
{
	int i;

	cudaEventDestroy(	start	);
	cudaEventDestroy(	stop	);

	for(i=0; i<CUSTREAM_CNT; i++){
		if(m_cuStream[i] != NULL){
			et = cudaStreamDestroy(m_cuStream[i]);
			OSA_assert(et == cudaSuccess);
			m_cuStream[i] = NULL;
		}
	}
	for(i=0; i<SHAREARRAY_CNT; i++)
			cudaFree_share(NULL, i);
	cudaFree(d_Histogram);
	closeHistogram256();
}

unsigned char* CCudaProcess::load(cv::Mat frame, int memChn)
{
	int i;
	unsigned int byteCount = frame.rows * frame.cols * frame.channels() * sizeof(unsigned char);
	unsigned int byteBlock = byteCount/CUSTREAM_CNT;
	unsigned char *d_mem = NULL;

	et = cudaMalloc_share((void**)&d_mem, byteCount, memChn);
	OSA_assert(et == cudaSuccess);

	for(i = 0; i<CUSTREAM_CNT; i++){
		et = cudaMemcpyAsync(d_mem + byteBlock*i, frame.data + byteBlock*i, byteBlock, cudaMemcpyHostToDevice, m_cuStream[i]);
		OSA_assert(et == cudaSuccess);
	}

	et = cudaFree_share(d_mem, memChn);
	OSA_assert(et == cudaSuccess);

	return d_mem;
}

extern "C" int yuyv2gray_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);
void CCudaProcess::cutColor(cv::Mat src, cv::Mat &dst, int code)
{
	int i, chId = 0;
	unsigned char *d_src = load(src, chId);
	unsigned char *d_dst = NULL;
	unsigned int byteCount_src = src.rows * src.cols * src.channels() * sizeof(unsigned char);;
	unsigned int byteBlock_src = byteCount_src/CUSTREAM_CNT;
	unsigned int byteCount_dst;
	unsigned int byteBlock_dst;

	OSA_assert(code == CV_YUV2GRAY_YUYV);
	{
		//dst = Mat(src.rows, src.cols, CV_8UC1);
		byteCount_dst = dst.rows * dst.cols * sizeof(unsigned char);
		byteBlock_dst = byteCount_dst/CUSTREAM_CNT;
		cudaMalloc_share((void**)&d_dst, byteCount_dst, chId+1);

		for(i = 0; i<CUSTREAM_CNT; i++){
			yuyv2gray_(d_dst + byteBlock_dst*i,
					d_src + byteBlock_src*i,
					src.cols, (src.rows/CUSTREAM_CNT), m_cuStream[i]);
			cudaMemcpyAsync(dst.data + byteBlock_dst*i, d_dst + byteBlock_dst*i, byteBlock_dst, cudaMemcpyDeviceToHost, m_cuStream[i]);
		}
		cudaFree_share(d_dst, chId+1);
		for(i=0; i<CUSTREAM_CNT; i++)
			cudaStreamSynchronize(m_cuStream[i]);
	}
}

extern "C" int yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);
void CCudaProcess::YUVV2RGB(cv::Mat src, cv::Mat &dst, int code)
{
/*	cudaEventRecord	(	start,	0);*/

	int i,chId = 2;
	unsigned char *d_src = load(src,chId);
	unsigned char *d_dst = NULL;
	unsigned int byteCount_src = src.rows * src.cols * src.channels() * sizeof(unsigned char);;
	unsigned int byteBlock_src = byteCount_src/CUSTREAM_CNT;
	unsigned int byteCount_dst;
	unsigned int byteBlock_dst;

	OSA_assert(code == CV_YUV2BGR_YUYV);
	{
		//dst = Mat(src.rows, src.cols, CV_8UC1);
		byteCount_dst = dst.rows * dst.cols *3* sizeof(unsigned char);
		byteBlock_dst = byteCount_dst/CUSTREAM_CNT;
		cudaMalloc_share((void**)&d_dst, byteCount_dst, chId+1);

		for(i = 0; i<CUSTREAM_CNT; i++){
			yuyv2bgr_(d_dst + byteBlock_dst*i,
					d_src + byteBlock_src*i,
					src.cols, (src.rows/CUSTREAM_CNT), m_cuStream[i]);
		}

//		for(i=0; i<CUSTREAM_CNT; i++){
//			cudaMemcpy(dst.data + byteBlock_dst*i, d_dst + byteBlock_dst*i, byteBlock_dst, cudaMemcpyDeviceToHost);
//		}
		cudaMemcpy(dst.data, d_dst, byteCount_dst, cudaMemcpyDeviceToHost);
		cudaFree_share(d_dst, chId+1);
	}

/*		cudaEventRecord(	stop,	0	);
		cudaEventSynchronize(	stop);
		cudaEventElapsedTime(	&elapsedTime,	start,	stop);
		printf("YUVV2RGB:	%.8f	ms \n", elapsedTime);*/
}

extern "C" int yuyv2yuvplan_(
	unsigned char *dsty, unsigned char *dstu,unsigned char *dstv,const unsigned char *src,
	int width, int height, cudaStream_t stream);
void CCudaProcess::YUVV2YUVPlan(cv::Mat src, cv::Mat &dst)
{
/*	cudaEventRecord	(	start,	0);*/

	int i,chId = 4;
	unsigned char *d_src = load(src,chId);
	unsigned char *d_dst = NULL;
	unsigned int imagesize = src.rows*src.cols;
	unsigned int byteCount_src = src.rows * src.cols * src.channels() * sizeof(unsigned char);
	unsigned int byteBlock_src = byteCount_src/CUSTREAM_CNT;
	unsigned int byteCount_dst;
	unsigned int byteBlock_dst;

	{
		//dst = Mat(src.rows, src.cols, CV_8UC1);
		byteCount_dst = src.rows * src.cols * src.channels()* sizeof(unsigned char);
		byteBlock_dst = byteCount_dst/CUSTREAM_CNT;
		cudaMalloc_share((void**)&d_dst, byteCount_dst, chId+1);

		for(i = 0; i<CUSTREAM_CNT; i++){
			yuyv2yuvplan_(	d_dst + (byteBlock_dst>>1)*i,	d_dst + imagesize + (byteBlock_dst>>2)*i, 	d_dst + imagesize + (imagesize>>1) + (byteBlock_dst>>2)*i,
											d_src + byteBlock_src*i,
											src.cols, (src.rows/CUSTREAM_CNT), m_cuStream[i]);
		}

//		for(i=0; i<CUSTREAM_CNT; i++){
//			cudaMemcpy(dst.data + byteBlock_dst*i, d_dst + byteBlock_dst*i, byteBlock_dst, cudaMemcpyDeviceToHost);
//		}

		dst = cv::Mat(src.rows, src.cols, CV_8UC2, d_dst);

		cudaFree_share(d_dst, chId+1);
	}

/*		cudaEventRecord(	stop,	0	);
		cudaEventSynchronize(	stop);
		cudaEventElapsedTime(	&elapsedTime,	start,	stop);
		printf("YUVV2RGB:	%.8f	ms \n", elapsedTime);*/
}

static CCudaProcess proc;

void cutColor(Mat src, Mat &dst, int code)
{
	proc.cutColor(src, dst, code);
}

//Internal memory allocation
extern "C" void initHistogram256(void);

//Internal memory deallocation
extern "C" void closeHistogram256(void);

extern "C" void histogram256(
    uint *d_Histogram,
    void *d_Data,
    uint byteCount
);

extern "C" int rgbHist_(
		unsigned char *dst,const unsigned char *src,uint  *d_Histogram,
	int width, int height);
extern "C" int grayHist_(
		unsigned char *dst,const unsigned char *src,uint  *d_Histogram,
	int width, int height);

void CCudaProcess::cuHistEn(cv::Mat src, cv::Mat dst)
{
/*	cudaEventRecord	(	start,	0);*/

	histogram256(d_Histogram, (void *)src.data, src.rows*src.cols*src.channels());

	uint Histogram[HISTOGRAM256_BIN_COUNT];

	 et = cudaMemcpy(Histogram, d_Histogram, HISTOGRAM256_BIN_COUNT*sizeof(uint), cudaMemcpyDeviceToHost);
	 OSA_assert(et == cudaSuccess);

	int k, pixelsum = 0, sum = 0;
	for(k=0; k<HISTOGRAM256_BIN_COUNT; k++)
	{
//		Histogram[k] = (uint)(10.0*log((double)Histogram[k]+1));
		pixelsum += Histogram[k] ;
	}
	for(k=0; k<HISTOGRAM256_BIN_COUNT; k++)
	{
		sum += Histogram[k];
		Histogram[k] = (uint)( (255.0 * sum)/(pixelsum - Histogram[k])+0.5f );
	}

	et = cudaMemcpy(d_Histogram, Histogram, HISTOGRAM256_BIN_COUNT*sizeof(uint), cudaMemcpyHostToDevice);
	OSA_assert(et == cudaSuccess);

	if(src.channels() == 3)
		rgbHist_(dst.data, src.data, d_Histogram, src.cols, src.rows);
	else if(src.channels() == 1)
		grayHist_(dst.data,src.data, d_Histogram, src.cols, src.rows);

/*
	cudaEventRecord(	stop,	0	);
	cudaEventSynchronize(	stop);
	cudaEventElapsedTime(	&elapsedTime,	start,	stop);
	printf("cuHistEn:	%.8f	ms \n", elapsedTime);*/
}

void cuHistEn(Mat src, Mat dst)
{
	proc.cuHistEn(src,dst);
}

void cvtBigVideo(cv::Mat src, cv::Mat &dst, int type)
{
	proc.YUVV2RGB(src, dst, type);
}

void cvtBigVideo_plan(cv::Mat src,cv::Mat &dst)
{
	proc.YUVV2YUVPlan(src, dst);
}


/*
 * cuProcess.hpp
 *
 *  Created on: May 11, 2017
 *      Author: ubuntu
 */

#ifndef CUPROCESS_HPP_
#define CUPROCESS_HPP_

#include <glew.h>
#include <glut.h>
#include <freeglut_ext.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

#define CUSTREAM_CNT		(8)

class CCudaProcess
{
	cudaStream_t m_cuStream[CUSTREAM_CNT];
	uint  *d_Histogram;
public:
	CCudaProcess();
	virtual ~CCudaProcess();

	void cutColor(cv::Mat src, cv::Mat &dst, int code);
	void cuHistEn(cv::Mat src, cv::Mat dst);
	void YUVV2RGB(cv::Mat src, cv::Mat &dst, int code);
	void YUVV2YUVPlan(cv::Mat src, cv::Mat &dst);

protected:
	unsigned char* load(cv::Mat frame, int memChn = 0);

private:
	cudaEvent_t	start, stop;
	float elapsedTime;
};


extern void cutColor(cv::Mat src, cv::Mat &dst, int code);

extern void cuHistEn(cv::Mat src, cv::Mat dst);

extern void cvtBigVideo(cv::Mat src,cv::Mat &dst,  int type);

extern void cvtBigVideo_plan(cv::Mat src,cv::Mat &dst);

#endif /* CUPROCESS_HPP_ */

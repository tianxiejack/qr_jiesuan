/*
 * BlobDetector.h
 *
 *  Created on: 2017年4月28日
 *      Author: fsmdn113
 */

#ifndef BLOBDETECTOR_H_
#define BLOBDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#undef		_debug_view
using namespace cv;

typedef struct _blob_rect_{
	bool 		bvalid;
	Point2f center; //< the rectangle mass center
	Size2f 	size;    //< width and height of the rectangle
	float 		angle;
}BlobRect;

bool BlobDetect(cv::Mat GraySrc, int adaptiveThred, BlobRect &blob_rect);


#endif /* BLOBDETECTOR_H_ */

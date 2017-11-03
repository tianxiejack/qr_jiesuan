/*
 * cuProcess.hpp
 *
 *  Created on: May 11, 2017
 *      Author: ubuntu
 */

#ifndef __ENH_HPP_
#define __ENH_HPP_






extern void cuHistEnh(cv::Mat src, cv::Mat dst);

//extern void cuClahe(cv::Mat src, cv::Mat dst, unsigned int uiNrX = 8, unsigned int uiNrY = 8, float fCliplimit = 2.5);

extern void cuClahe(cv::Mat src, cv::Mat dst, unsigned int uiNrX = 8, unsigned int uiNrY = 8, float fCliplimit = 2.5, int procType = 0);

extern void cuUnhazed(cv::Mat src, cv::Mat dst);


#endif 

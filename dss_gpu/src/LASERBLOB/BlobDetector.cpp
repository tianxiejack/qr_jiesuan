
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include <iostream>

#include <math.h>
#include <time.h>
#include <vector>

#include "BlobDetector.h"

using namespace std;
using namespace cv;

#define cvQueryHistValue_1D( hist, idx0 )  ((float)cvGetReal1D( (hist)->bins, (idx0)))  

int otsu(IplImage* src)  
{
	CvScalar cs = cvSum(src);  

	int hist_size = 256;
	int hist_height = 256;    
	float range[] = {0,255};
	float* ranges[]={range};    

	CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);    

	cvCalcHist(&src,gray_hist,0,0);    

	cvNormalizeHist(gray_hist,1.0);    

	int Width = src->width;  
	int Height = src->height;  
	double U_t = cs.val[0]/Width*Height;
	int threshold = 0;  
	double delta = 0;  
	for(int k=0; k<256; k++)  
	{  
		double u0 = 0;  
		double u1 = 0;  
		double w0 = 0;  
		double w1 = 0;  
		double delta_tmp = 0;  
		for(int i=0; i<=k; i++)  
		{  
			u0 += cvQueryHistValue_1D(gray_hist,i)*i;
			w0 += cvQueryHistValue_1D(gray_hist,i);
		}  
		for(int j=k+1; j<256; j++)  
		{  
			u1 += cvQueryHistValue_1D(gray_hist,j)*j;
			w1 += cvQueryHistValue_1D(gray_hist,j);
		}  
		delta_tmp = w0*pow(u0-U_t,2) + w1*pow(u1-U_t,2);
		if(delta_tmp > delta)  
		{  
			delta = delta_tmp;  
			threshold = k;  
		}  
	}  
	return threshold;  
}  

bool BlobDetect(Mat GraySrc, int adaptiveThred, BlobRect &blob_rect)
{
	CV_Assert(GraySrc.channels() == 1);
	Mat  BinIMG;

	threshold(GraySrc, BinIMG, adaptiveThred, 255, CV_THRESH_BINARY/*CV_THRESH_OTSU*/);
	//threshold( GraySrc, BinIMG, 0, 255, CV_THRESH_BINARY| CV_THRESH_OTSU);

#ifdef _debug_view
	Mat postIMG;
	resize(BinIMG, postIMG, Size(640,480));
	imshow("postIMG", postIMG);
	waitKey(1);
#endif

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

//	findContours( BinIMG, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	findContours(BinIMG, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	if( contours.size() == 0 ){
		blob_rect.bvalid = false;
		return false;
	}

	// iterate through all the top-level contours,
	// draw each connected component with its own random color
	int idx = 0, largestComp = 0;
	double maxArea = 0;
	int maxCount = 0;

	 for( idx= 0; idx< contours.size(); idx++ )
	 {
		 size_t count = contours[idx].size();
		 const vector<Point>& c = contours[idx];
		 double area = fabs(contourArea(Mat(c)));
		 if( area < 100 || area > (BinIMG.rows*BinIMG.cols>>2))
			 continue;

		 if( area > maxArea )
		 {
			 maxArea = area;
			 largestComp = idx;
			 maxCount = count;
		 }
	 }
	 if(maxCount < 6){
		 blob_rect.bvalid = false;
		 return false;
	 }

	 Mat pointsf;
	 Mat(contours[largestComp]).convertTo(pointsf, CV_32F);
	 RotatedRect box = fitEllipse(pointsf);

	 blob_rect.bvalid = true;
	 blob_rect.center = box.center;
	 blob_rect.size = box.size;
	 blob_rect.angle = box.angle;

#if _debug_view
	 Mat RGBSrc;
	 cvtColor(GraySrc, RGBSrc, CV_GRAY2BGR);
	 drawContours(RGBSrc, contours, largestComp, Scalar(255,0,0), 2, 8);
	 ellipse(RGBSrc, box, Scalar(0,0,255), 4, CV_AA);
//	 ellipse(RGBSrc, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,0), 4, CV_AA);

	 Point pt1,pt2,center;
	 center.x = box.center.x;
	 center.y = box.center.y;
	 pt1.x=center.x-1;pt1.y=center.y; 
	 pt2.x=center.x+1;pt2.y=center.y; 
	 line(RGBSrc, pt1, pt2, CV_RGB(255,0,0), 2, CV_AA, 0 ); 
	 pt1.x=center.x;pt1.y=center.y-1; 
	 pt2.x=center.x;pt2.y=center.y+1; 
	 line(RGBSrc, pt1, pt2, CV_RGB(255,0,0), 2, CV_AA, 0 );
	
	 Mat scaleBlob;
	 resize(RGBSrc, scaleBlob, Size(640,480));
	 imshow("BlobDetect",scaleBlob);
	 waitKey(1);
#endif

	 return true;
}

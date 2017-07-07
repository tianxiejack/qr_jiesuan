#include <math.h>
#include "multitarget.h"
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

extern "C"
{
	#include "malloc_align.h"
	 void* MallocAlign( int nSize);
	 void FreeAlign(void *p);
}

void test(int a)
{
	//return 0;
	//printf("the hello \n");
}

int OpenTarget(target_t *t, int VideoWidth, int VideoHeight)
{
	int i, j;
	float sum;
	float tmp[4][4];

	t->i_width = VideoWidth;
	t->i_height = VideoHeight;
	
	t->Img[0] =	(unsigned char *)MallocAlign( t->i_height * t->i_width * sizeof(char));
	if(t->Img[0] == MEM_ILLEGAL ) 
		return -1;
	t->Img[1] =	(unsigned char *)MallocAlign( (t->i_height>>1) * (t->i_width>>1) * sizeof(char));
	if(t->Img[1] == MEM_ILLEGAL ) 
	{
		FreeAlign(t->Img[0]);

		return -1;
	}
	t->Img[2] =	(unsigned char *)MallocAlign((t->i_height>>2) * (t->i_width>>2) * sizeof(char));
	if(t->Img[2] == MEM_ILLEGAL ) 
	{
		FreeAlign(t->Img[0]);
		FreeAlign(t->Img[1] );

		return -1;
	}
	t->Img[3] = (unsigned char *)MallocAlign((t->i_height>>3) * (t->i_width>>3) * sizeof(char));
	if(t->Img[3] == MEM_ILLEGAL ) 
	{
		FreeAlign( t->Img[0] );
		FreeAlign(  t->Img[1]);
		FreeAlign( t->Img[2]);

		return -1;
	}
	t->Img[4] = (unsigned char *)MallocAlign((t->i_height>>4) * (t->i_width>>4) * sizeof(char));
	if(t->Img[4] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1] );
		FreeAlign(  t->Img[2] );
		FreeAlign(  t->Img[3]);

		return -1;
	}
	t->Img[5] = (unsigned char *)MallocAlign((t->i_height>>5) * (t->i_width>>5) * sizeof(char));
	if(t->Img[5] == MEM_ILLEGAL ) 
	{
		FreeAlign( t->Img[0]);
		FreeAlign( t->Img[1] );
		FreeAlign( t->Img[2] );
		FreeAlign(  t->Img[3] );
		FreeAlign( t->Img[4] );

		return -1;
	}
	t->FtImg[0] = (unsigned char *)MallocAlign((t->i_height>>1) * (t->i_width>>1) * sizeof(char));
	if(t->FtImg[0] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1]);
		FreeAlign(  t->Img[2]);
		FreeAlign(  t->Img[3]);
		FreeAlign(  t->Img[4]);
		FreeAlign( t->Img[5]);

		return -1;
	}
	t->FtImg[1] = (unsigned char *)MallocAlign((t->i_height>>2) * (t->i_width>>2) * sizeof(char));
	if(t->FtImg[1] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1]);
		FreeAlign(  t->Img[2]);
		FreeAlign(  t->Img[3]);
		FreeAlign(  t->Img[4]);
		FreeAlign(  t->Img[5]);

		FreeAlign(  t->FtImg[0]);

		return -1;
	}
	t->FtImg[2] = (unsigned char *)MallocAlign((t->i_height>>3) * (t->i_width>>3) * sizeof(char));
	if(t->FtImg[2] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1] );
		FreeAlign(  t->Img[2] );
		FreeAlign(  t->Img[3] );
		FreeAlign(  t->Img[4] );
		FreeAlign(  t->Img[5] );

		FreeAlign(  t->FtImg[0]);
		FreeAlign(  t->FtImg[1]);

		return -1;
	}
	t->FtImg[3] = (unsigned char *)MallocAlign((t->i_height>>4) * (t->i_width>>4) * sizeof(char));
	if(t->FtImg[3] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0] );
		FreeAlign(  t->Img[1]);
		FreeAlign(  t->Img[2]);
		FreeAlign(  t->Img[3] );
		FreeAlign(  t->Img[4] );
		FreeAlign(  t->Img[5]);

		FreeAlign(  t->FtImg[0]);
		FreeAlign(  t->FtImg[1] );
		FreeAlign(  t->FtImg[2]);

		return -1;
	}

	t->FtImg[4] = (unsigned char *)MallocAlign((t->i_height>>5) * (t->i_width>>5) * sizeof(char));
	if(t->FtImg[4] == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1]);
		FreeAlign(  t->Img[2]);
		FreeAlign(  t->Img[3]);
		FreeAlign(  t->Img[4]);
		FreeAlign(  t->Img[5]);

		FreeAlign(  t->FtImg[0]);
		FreeAlign(  t->FtImg[1]);
		FreeAlign(  t->FtImg[2]);
		FreeAlign(  t->FtImg[3]);

		return -1;
	}
	t->sobel =	(unsigned char *)MallocAlign((t->i_height>>1) * (t->i_width>>1) * sizeof(char));
	if(t->sobel == MEM_ILLEGAL ) 
	{
		FreeAlign(  t->Img[0]);
		FreeAlign(  t->Img[1]);
		FreeAlign(  t->Img[2]);
		FreeAlign(  t->Img[3]);
		FreeAlign(  t->Img[4]);
		FreeAlign(  t->Img[5]);

		FreeAlign(  t->FtImg[0]);
		FreeAlign(  t->FtImg[1]);
		FreeAlign(  t->FtImg[2]);
		FreeAlign(  t->FtImg[3]);
		FreeAlign(  t->FtImg[4]);

		return -1;
	}
	
	sum = 0;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			tmp[i][j] = exp(-(((float)i-1.5)*((float)i-1.5)+((float)j-1.5)*((float)j-1.5))/(3.0*3.0));//(3.0*3.0));
			sum += tmp[i][j];
		}
	}
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			t->gaussCoefDown.coef[i][j] = (unsigned char)( (tmp[i][j]*128.0)/sum + 0.5);
		}
	}
	t->gaussCoefDown.shift = 7;

	for(i = 0; i < 6; i++)
	{
		t->ft[i].total = 0;
	}
	
	//清除所有的目标
	for(i = 0; i < MAX_TARGET_NUMBER; i++)
	{
		t->tg[i].valid = 0;
	}
	
	t->state = 0; // nothing to do
	DeletTarget(t, -1);

	return 0;
}

int CloseTarget(target_t *t)
{
	t->state = 0; // nothing to do

	FreeAlign( t->Img[0]);
	FreeAlign( t->Img[1]);
	FreeAlign( t->Img[2]);
	FreeAlign( t->Img[3]);
	FreeAlign( t->Img[4]);
	FreeAlign( t->Img[5]);

	FreeAlign( t->FtImg[0]);
	FreeAlign( t->FtImg[1]);
	FreeAlign(t->FtImg[2]);
	FreeAlign( t->FtImg[3] );
	FreeAlign( t->FtImg[4] );

	FreeAlign( t->sobel);

	return 0;
}

void MediumFliter(unsigned char *in, unsigned char *out, int w, int h)
{
	Mat		inIMG(h, w, CV_8UC1, in);
	Mat		outIMG(h,w, CV_8UC1, out);
	cv::medianBlur(inIMG, outIMG, 3);

// 	imshow("median", outIMG);
// 	waitKey(1);
}

static const unsigned char gaussmask[3][3] = {
		{1,2,1},
		{2,4,2},
		{1,2,1}
};
     
void GaussFliter(unsigned char *in, unsigned char *out, int w, int h)
{
	Mat		inIMG(h, w, CV_8UC1, in);
	Mat		outIMG(h,w, CV_8UC1, out);
#if 0
	cv::GaussianBlur(inIMG, outIMG,cv::Size(3,3), 0, 0);
//	Mat mask=(Mat_<char>(3,3)<<1,2,1, 2,4,2,  1,2,1);
//	cv::filter2D(inIMG, outIMG, inIMG.type(), mask);//采用float型
#else

	int O;    /* Sum of horizontal and vertical masks   */
	int i;    /* Input pixel offset                     */
	int o;    /* Output pixel offset.                   */
	int xy;   /* Loop counter.                          */
	int i00, i01, i02;
	int i10, i11,  i12;
	int i20, i21, i22;

	short cols, rows;
	cols = w, rows = h;

	for (xy = 0, i = cols + 1, o = 1;    xy < cols*(rows-2) - 2;    xy++, i++, o++)
	{
	   i00=in[i-cols-1]; i01=in[i-cols]; i02=in[i-cols+1];
	   i10=in[i     -1];     i11=in[i];          i12=in[i     +1];
	   i20=in[i+cols-1]; i21=in[i+cols]; i22=in[i+cols+1];

	   O = i00*gaussmask[0][0]+i01*gaussmask[0][1]+i02*gaussmask[0][2]+
			   i10*gaussmask[1][0]+i11*gaussmask[1][1]+i12*gaussmask[1][2]+
			   i20*gaussmask[2][0]+i21*gaussmask[2][1]+i22*gaussmask[2][2];

	   O >>=4;

	  O = (O > 255) ? 255 : O;

	   out[o] = O;
	}
//	imshow("GaussFliter", outIMG);
// 	waitKey(1);

#endif
}

//image down sample 
void DownSample(target_t *t, unsigned char *in, unsigned char *out, int w, int h)
{
#if 1
	int	x, y; 
	for(y=0;y<h/2;y++)
	{
		for(x=0;x<w/2;x++)
		{
			out[y*w/2+x]=in[(y<<1)*w+(x<<1)];
		}
	}
#else
	Mat		inIMG(h, w, CV_8UC1, in);
	Mat		outIMG(h/2,w/2, CV_8UC1, out);
	cv::pyrDown(inIMG, outIMG, cv::Size(w/2, h/2));

// 	char strname[32];
// 	sprintf(strname, "pyrDown_%dx%d", w/2, h/2);
// 	imshow(strname, outIMG);
// 	waitKey(1);
#endif
}

static void _IMG_sobel                                                  
	           (                                                               
	               const unsigned char *in,       /* Input image data  */      
	               unsigned char       *out,      /* Output image data */      
	               short cols, short rows         /* Image dimensions  */      
	           )                                                               
	           {                                                               
	               
	               int H;    /* Horizontal mask result                 */      
	               int V;    /* Vertical mask result                   */      
	               int O;    /* Sum of horizontal and vertical masks   */      
	               int i;    /* Input pixel offset                     */      
	               int o;    /* Output pixel offset.                   */      
	               int xy;   /* Loop counter.                          */      
	                                                                           	             
	               int i00, i01, i02;                                          
	               int i10,      i12;                                          
	               int i20, i21, i22;                                          
	                                                                           	          
	               for (xy = 0, i = cols + 1, o = 1;    xy < cols*(rows-2) - 2;    xy++, i++, o++)                                        
	               {                                                           
	                   i00=in[i-cols-1]; i01=in[i-cols]; i02=in[i-cols+1];     
	                   i10=in[i     -1];                 i12=in[i     +1];     
	                   i20=in[i+cols-1]; i21=in[i+cols]; i22=in[i+cols+1];     
	                                                                          
	                   H = -i00 - 2*i01 -   i02 +   i20 + 2*i21 + i22;         
	                                                                           
	                   V = -i00 +   i02 - 2*i10 + 2*i12 -   i20 + i22;         
	                                                                           
	                   O = abs(H) + abs(V);                                    
	                                                                           
	                   if (O > 255) O = 255;                                   
	                                                                           
	                   out[o] = O;                                             
	               }                                                           
	           }                                                               
	                                                                           

void IMG_sobel(unsigned char *in, unsigned char *out, int w, int h)
{
#if 0
	Mat		inIMG(h, w, CV_8UC1, in);
	Mat		outIMG(h,w, CV_8UC1, out);
	cv::Sobel(inIMG, outIMG, CV_8UC1, 1,1);
#else
	_IMG_sobel(in, out, w, h);

#endif
// 	Mat		outIMG(h,w, CV_8UC1, out);
//   	imshow("sobel", outIMG);
//  	waitKey(1);
}

int palt[3][3] =
{
	{-1, -1, -1},
	{-1,  8, -1},
	{-1, -1, -1}
};

void CalLightArea(unsigned char *img, unsigned char *val, int w, int h)
{
	int x, y;
	int sum, sum0;

	//avg = avgnum = 0;
	for(x = 0; x < w; x++)
		val[x] = 0;
	for(x = 0; x < w; x++)
		val[(h-1)*w+x] = 0;
	for(y = 1; y < h-1; y++)
	{
		val[w*y] = val[y*w+w-1] = 0;
		for(x = 1; x < w-1; x++)
		{
			sum =  palt[0][0] * img[(y-1)*w+x-1] + palt[0][1] * img[(y-1)*w+x] + palt[0][2] * img[(y-1)*w+x+1] 
				 + palt[1][0] * img[(y)  *w+x-1] + palt[1][1] * img[(y)  *w+x] + palt[1][2] * img[(y)  *w+x+1]
				 + palt[2][0] * img[(y+1)*w+x-1] + palt[2][1] * img[(y+1)*w+x] + palt[2][2] * img[(y+1)*w+x+1];

			sum0 = ((sum < 0 ? -sum : sum) +4)>> 3  ;
			val[y*w+x] = sum0 > 255 ? 255 : sum0;

		}
	}
}

void calAreaConst(FPOINT *fp, unsigned char *pImg, int w, int h)
{
	int x, y, x0, y0;
	unsigned int BriVal, DarkVal, MidVal, BriSum, sum, Brightest, Darkest, DarkSum;
	x0 = fp->x>>1;
	y0 = fp->y>>1;
	
	MidVal = 0;
	sum = 0;
	for(y = (y0-(TARGET_YSIZE>>2)); y < (y0+(TARGET_YSIZE>>2)); y++)
	{
		for(x = (x0-(TARGET_XSIZE>>2)); x < (x0+(TARGET_XSIZE>>2)); x++)
		{
			MidVal += pImg[y*w+x];
			sum++;
		}
	}
	
	MidVal = MidVal/((TARGET_YSIZE*TARGET_XSIZE)>>2);

	BriSum = 0;
	BriVal = DarkVal = 0;
	for(y = (y0-(TARGET_YSIZE>>2)); y < (y0+(TARGET_YSIZE>>2)); y++)
	{
		for(x = (x0-(TARGET_XSIZE>>2)); x < (x0+(TARGET_XSIZE>>2)); x++)
		{
			if( pImg[y*w+x] > MidVal )
			{
				BriVal += pImg[y*w+x];
				BriSum++;
			}
			else
				DarkVal += pImg[y*w+x];
		}
	}

	BriVal = BriVal/(BriSum+1);
	DarkVal = DarkVal/( ((TARGET_YSIZE*TARGET_XSIZE)>>2) - BriSum);

	Brightest = Darkest = 0;
	BriSum = DarkSum = 0;
	for(y = (y0-(TARGET_YSIZE>>2)); y < (y0+(TARGET_YSIZE>>2)); y++)
	{
		for(x = (x0-(TARGET_XSIZE>>2)); x < (x0+(TARGET_XSIZE>>2)); x++)
		{
			if(pImg[y*w+x] > BriVal)
			{
				Brightest += pImg[y*w+x];
				BriSum++;//pImg[y*w+x] = 0xff;
			}
			else if(pImg[y*w+x] < DarkVal)
			{
				Darkest += pImg[y*w+x];
				DarkSum++;
				//pImg[y*w+x] = 0;
			}
		}
	}

	Brightest = Brightest/(BriSum+1);
	Darkest = Darkest/(DarkSum+1);

	fp->Const = ((Brightest - Darkest)*100)>>8;
	fp->AreaRate = (BriSum*100)/((TARGET_YSIZE*TARGET_XSIZE)>>2);

	return;
}

void FindMaxArea(target_t *t, unsigned char *img, int w, int h, int imagesize)
{
	int i, x, y;
	int maxX, maxY, maxV;
	int winX, winY;
	int offX, offY;

	offX = (TARGET_XSIZE>>imagesize) < 4 ? 4: (TARGET_XSIZE>>imagesize);
	offY = (TARGET_YSIZE>>imagesize) < 4 ? 4: (TARGET_YSIZE>>imagesize);
	winX = (TARGET_XSIZE>>(imagesize+1)) < 4 ? 4: (TARGET_XSIZE>>(imagesize+1));
	winY = (TARGET_YSIZE>>(imagesize+1)) < 4 ? 4: (TARGET_YSIZE>>(imagesize+1));

	for(i = 0; i < SEARCH_TARGET_NUMBER; i++)
	{
		maxV = 0;
		maxX = 0;
		maxY = 0;
		for(y = offY; y < h-offY; y++)
		{
			for(x = offX; x < w-offX; x++)
			{
				if( maxV < img[y*w+x] )
				{
					maxV = img[y*w+x];
					maxX = x; 
					maxY = y;
				}
			}
		}
		if(maxV == 0)			
			break;
		
		for(y = maxY-winY; y < maxY+winY; y++)
		{
			for(x = maxX-winX; x < maxX+winX; x++)
			{
				img[y*w+x] = 0;
			}
		}

		t->ft[imagesize].fp[i].ftv = maxV;
		t->ft[imagesize].fp[i].x = maxX << imagesize;
		t->ft[imagesize].fp[i].y = maxY << imagesize;

		calAreaConst(&(t->ft[imagesize].fp[i]), t->Img[1], t->i_width>>1, t->i_height>>1);
	}

	t->ft[imagesize].total = i;
}

int calEdgePoint(unsigned char *pSob, int w, int h, int x0, int y0)
{
	int x, y;
	int sum, cnt, hvok;

	hvok = 0;
	x0 = x0>>1;
	y0 = y0>>1;

	for(y = (y0-(TARGET_YSIZE>>2)); y < (y0+(TARGET_YSIZE>>2)); y++)
	{
		cnt = 0;
		sum = 0;
		for(x = (x0-(TARGET_XSIZE>>2)); x < (x0+(TARGET_XSIZE>>2)); x++)
		{
			if( pSob[y*w+x] > EDGE_DETECT_THRED )
			{
				sum++;
				if(sum > cnt) cnt = sum;
			}
			else						sum = 0;
		}
		if(cnt > 5)
		{
			hvok++;
			break;
		}
	}

	for(x = (x0-(TARGET_XSIZE>>1)); x < (x0+(TARGET_XSIZE>>1)); x++)
	{
		cnt = 0;
		sum = 0;
		for(y = (y0-(TARGET_YSIZE>>1)); y < (y0+(TARGET_YSIZE>>1)); y++)
		{
			if( pSob[y*w+x] > EDGE_DETECT_THRED )
			{
				sum++;
				if(sum > cnt) cnt = sum;
			}
			else						sum = 0;
		}
		if(cnt > 3)
		{
			hvok++;
			break;
		}
	}

	return hvok;
}

void GetMaxFtp(target_t *t)
{
	FPOINT tmpFP[SEARCH_TARGET_NUMBER*(MAX_SCALER-MIN_SCALER+1)];
	int tmpFpSum;
	int i, j, k, maxIdx, pnum, maxV, edgp;

	tmpFpSum = 0;
	for(k = MIN_SCALER; k <= MAX_SCALER; k++)
	{
//		printf("%s:%dlay total = %d\n", __func__, k,  t->ft[k].total);
		for(j = 0; j < t->ft[k].total; j++)
		{
			if(tmpFpSum == 0)
			{
				tmpFP[tmpFpSum].ftv      = t->ft[k].fp[j].ftv;
				tmpFP[tmpFpSum].AreaRate = t->ft[k].fp[j].AreaRate;
				tmpFP[tmpFpSum].Const    = t->ft[k].fp[j].Const;
				tmpFP[tmpFpSum].x        = t->ft[k].fp[j].x;
				tmpFP[tmpFpSum].y        = t->ft[k].fp[j].y;
				tmpFP[tmpFpSum].num      = 1;
				tmpFpSum++;
			}
			else
			{
				for(i = 0; i < tmpFpSum; i++)
				{
					if(  ((t->ft[k].fp[j].x - tmpFP[i].x)*(t->ft[k].fp[j].x - tmpFP[i].x) 
								+ (t->ft[k].fp[j].y - tmpFP[i].y)*(t->ft[k].fp[j].y - tmpFP[i].y)) 
								< (TARGET_XSIZE*TARGET_XSIZE+TARGET_YSIZE*TARGET_YSIZE )  ) //距离约束已经考虑尺度
					{
						if(t->ft[k].fp[j].Const > tmpFP[i].Const)
						{
							tmpFP[i].ftv      = t->ft[k].fp[j].ftv;
							tmpFP[i].AreaRate = t->ft[k].fp[j].AreaRate;
							tmpFP[i].Const    = t->ft[k].fp[j].Const;
							tmpFP[i].x        = (t->ft[k].fp[j].x + tmpFP[i].x)/2;
							tmpFP[i].y        = (t->ft[k].fp[j].y+tmpFP[i].y)/2;
							tmpFP[i].num ++;
						}
						break;
					}
				}//i
				if( i == tmpFpSum)
				{
					tmpFP[tmpFpSum].ftv      = t->ft[k].fp[j].ftv;
					tmpFP[tmpFpSum].AreaRate = t->ft[k].fp[j].AreaRate;
					tmpFP[tmpFpSum].Const    = t->ft[k].fp[j].Const;
					tmpFP[tmpFpSum].x        = t->ft[k].fp[j].x;
					tmpFP[tmpFpSum].y        = t->ft[k].fp[j].y;
					tmpFP[tmpFpSum].num      = 1;
					tmpFpSum++;
					
				}
			}
		}//j
	}//k

//	printf("%s:tmpFpSum=%d \n",__func__, tmpFpSum);

	for(pnum = 0; pnum < 4; )
	{
		maxV = 0;
		maxIdx = 0;
		for(k = 0; k < tmpFpSum; k++)
		{
			if(tmpFP[k].num > maxV)
			{
				maxV = tmpFP[k].num;
				maxIdx = k;
			}
		}
		
		tmpFP[maxIdx].num = 0;
		if(maxV < 3)		break;

		edgp = calEdgePoint(t->sobel, t->i_width>>1, t->i_height>>1, tmpFP[maxIdx].x, tmpFP[maxIdx].y);
		if(edgp > 1)
		{
			t->ft[0].fp[pnum].Const = tmpFP[maxIdx].Const;
			t->ft[0].fp[pnum].x = tmpFP[maxIdx].x;
			t->ft[0].fp[pnum].y = tmpFP[maxIdx].y;
			pnum++;
		}
		tmpFP[maxIdx].Const = 0;
	}

//	printf("%s:pnum1=%d \n",__func__, pnum);

	for( ; pnum < SEARCH_TARGET_NUMBER; )
	{
		maxV = 0;
		maxIdx = 0;
		for(k = 0; k < tmpFpSum; k++)
		{
			if(tmpFP[k].Const > maxV)
			{
				maxV = tmpFP[k].Const;
				maxIdx = k;
			}
		}
		tmpFP[maxIdx].Const = 0;
		
		if(maxV == 0)	break;
		
		edgp = calEdgePoint(t->sobel, t->i_width>>1, t->i_height>>1, tmpFP[maxIdx].x, tmpFP[maxIdx].y);
		if(edgp > 1)
		{
			t->ft[0].fp[pnum].Const = maxV;
			t->ft[0].fp[pnum].x = tmpFP[maxIdx].x;
			t->ft[0].fp[pnum].y = tmpFP[maxIdx].y;
			pnum++;
		}
	}

	t->ft[0].total = pnum;

//	printf("%s:pnum2=%d \n",__func__, pnum);

	return ;		
}

void WriteCross(FEAT *ftp, unsigned char *img, int w, int h, int imagesize)
{
	int x, y;
	int i;
	int ftX, ftY;
	
	for(i = 0; i < ftp->total; i++)
	{
		
		ftX = ftp->fp[i].x>>imagesize;
		ftY = ftp->fp[i].y>>imagesize;

		for(x= 0; x < 8; x++)
		{
			img[ftY*w+ftX-3+x] = 0xef;
			img[(ftY+1)*w+ftX-3+x] = 0x10;
		}
		for(y=0;y<8;y++)
		{
			img[(ftY-3+y)*w+ftX] = 0xef;
			img[(ftY-3+y)*w+ftX+1] = 0x10;
		}
	}
}

void AutoDetectTarget(target_t *t, unsigned char *img)
{
	int i;
	for(i = MIN_SCALER; i <= MAX_SCALER; i++)
	{
		CalLightArea(t->Img[i], t->FtImg[i-1], (t->i_width>>i), (t->i_height>>i) );
		FindMaxArea(t, t->FtImg[i-1], (t->i_width>>i), (t->i_height>>i), i );
	}
	GetMaxFtp(t);
}

int GetTargetNumber(target_t *t)
{
	int i;
	int sum;

	sum = 0;

	for(i = 0; i < MAX_TARGET_NUMBER; i++)
	{
		if(t->tg[i].valid)	sum++;
	}

	return sum;
}
static void addtg(TARGET *dst_tg, TARGET *src_tg)
{

	dst_tg->valid = src_tg->valid;
	dst_tg->live = src_tg->live;
	dst_tg->mode = 0;
	dst_tg->manual = src_tg->manual;
	dst_tg->cur_x = src_tg->cur_x;
	dst_tg->cur_y = src_tg->cur_y;
	dst_tg->arearate = src_tg->arearate;
	dst_tg->constract = src_tg->constract;
	dst_tg->xspeed = src_tg->xspeed;
	dst_tg->yspeed = src_tg->yspeed;
}

int InsertTarget(target_t *t, TARGET *target, int Index)
{
	int i;

	if(Index == -1)
	{
		for(i = 0; i < MAX_TARGET_NUMBER; i++)
		{
			if((t->tg[i].valid != 0) || (t->tg[i].live != 0))	continue;
			else
			{
				addtg(&(t->tg[i]), target);
				break;;
			}
		}
		if(i == MAX_TARGET_NUMBER)
			return -1;
		else
			return i;
	}
	else if( (Index >= 0) && (Index < MAX_TARGET_NUMBER) )
	{
		addtg(&(t->tg[Index]), target);
		return Index;
	}
	else
	{
		return -1;
	}
	
}

int	DeletFp(FEAT *ft, int index)
{
	int i;

	if(ft->total < 1)	return -1;
	if(index >=  ft->total)	return -1;

	for(i = index; i < ft->total-1; i++)
	{
		ft->fp[i].ftv = ft->fp[i+1].ftv;
		ft->fp[i].AreaRate = ft->fp[i+1].AreaRate;
		ft->fp[i].Const = ft->fp[i+1].Const;
		ft->fp[i].x = ft->fp[i+1].x;
		ft->fp[i].y = ft->fp[i+1].y;
	}
	ft->total = ft->total - 1;

	return 0;

}

//删除一个目

//Index指定目标的位置，如果，则删除所有的目标

//删除成功，
int DeletTarget(target_t *t, int Index)
{
	int i;

	if(Index == -1)
	{
		for(i = 0; i < MAX_TARGET_NUMBER; i++)
		{
			t->tg[i].valid = 0;
			t->tg[i].live = 0;
		}
		return 0;
	}
	else if( (Index >= 0) && (Index < MAX_TARGET_NUMBER) )
	{
			if(t->tg[Index].valid )
			{
				t->tg[Index].valid = 0;
				t->tg[Index].live = 0;
				t->tg[Index].xspeed = 0;
				t->tg[Index].yspeed = 0;

				return Index;
			}
			else
				return -1;
	}
	else
	{
		return -1;
	}
	
}

void FilterMultiTarget(target_t *t)
{
	TARGET newtg;
	int AvgConst;
	int i, j;
				
	AvgConst = 0;
	for(i = 0; i < t->ft[0].total; i++)
	{
		AvgConst += t->ft[0].fp[i].Const;
	}	
	AvgConst = AvgConst / (t->ft[0].total+1);
	AvgConst = AvgConst > 10 ? 10 : AvgConst;

	for(i = 0; i < MAX_TARGET_NUMBER; i++)
	{
		int minDist, minIndx, dist;
				
		if((t->tg[i].valid) || (t->tg[i].live > 0))
		{
			minDist = 100000000;
			minIndx = 0;
			for(j = 0; j < t->ft[0].total; j++)
			{
				dist =   (t->tg[i].cur_x - t->ft[0].fp[j].x) *(t->tg[i].cur_x - t->ft[0].fp[j].x) 
					   + (t->tg[i].cur_y - t->ft[0].fp[j].y) *(t->tg[i].cur_y - t->ft[0].fp[j].y);
							 
				if(minDist > dist)
				{
					minDist = dist;
					minIndx = j;
				}
			}
			if(minDist < (TARGET_XSIZE*TARGET_XSIZE + TARGET_YSIZE*TARGET_YSIZE))
			{
				float new_x, new_y;
						
				if(t->tg[i].live  < TARGET_INTERRUPT_TIME)
				{	
					t->tg[i].live++;
				}

				newtg.valid = t->tg[i].live < TARGET_CAPTURE_TIME ? t->tg[i].valid : 1;

				newtg.manual = 0;
				newtg.live = t->tg[i].live;
				new_x = (1.0 - TRACK_SPEED_COEF) * (t->tg[i].cur_x)
						     +  TRACK_SPEED_COEF * ((float)(t->ft[0].fp[minIndx].x));
				new_y = (1.0 - TRACK_SPEED_COEF) * (t->tg[i].cur_y) 
						     +  TRACK_SPEED_COEF * ((float)(t->ft[0].fp[minIndx].y));
				newtg.xspeed = (1.0 - PRED_SPEED_COEF) * (t->tg[i].xspeed) 
								     + PRED_SPEED_COEF * ((float)(t->ft[0].fp[minIndx].x) - t->tg[i].cur_x);
				newtg.yspeed = (1.0 - PRED_SPEED_COEF) * (t->tg[i].yspeed) 
								     + PRED_SPEED_COEF * ((float)(t->ft[0].fp[minIndx].y) - t->tg[i].cur_y);
				newtg.cur_x = new_x;
				newtg.cur_y = new_y;
				newtg.constract = t->ft[0].fp[minIndx].Const;
				newtg.arearate = t->ft[0].fp[minIndx].AreaRate;
				InsertTarget(t, &newtg, i);
				DeletFp(&(t->ft[0]), minIndx);
			}
			else
			{	
				t->tg[i].live --;
				if(t->tg[i].live <= 0)
				{
					DeletTarget(t, i);
				}
				else
				{
					newtg.valid = t->tg[i].valid;
					newtg.live = t->tg[i].live;
					newtg.manual = 0;
					newtg.cur_x = t->tg[i].cur_x + t->tg[i].xspeed;
					newtg.cur_y = t->tg[i].cur_y + t->tg[i].yspeed;
					newtg.xspeed = t->tg[i].xspeed;
					newtg.yspeed = t->tg[i].yspeed;

					newtg.constract = t->tg[i].constract;
					newtg.arearate = t->tg[i].arearate;
					if(
						  (newtg.cur_x < TARGET_XSIZE) || (newtg.cur_x > (t->i_width - TARGET_XSIZE))
						||(newtg.cur_y < TARGET_YSIZE) || (newtg.cur_y > (t->i_width - TARGET_YSIZE))
					  )
					{
						DeletTarget(t, i);
					}
					else
					{
						InsertTarget(t, &newtg, i);
					}
				}
			}
		}
	}
	for(i = 0; i < t->ft[0].total; i++)
	{
		newtg.valid = 0;
		newtg.manual = 0;
		newtg.live = 1;
		newtg.xspeed = 0.0;
		newtg.yspeed = 0.0;
		newtg.cur_x = (float)t->ft[0].fp[i].x;
		newtg.cur_y = (float)t->ft[0].fp[i].y;
		newtg.constract = t->ft[0].fp[i].Const;
		newtg.arearate = t->ft[0].fp[i].AreaRate;
		InsertTarget(t, &newtg, -1);
	}
}

#if 0
int SetTarget(bool bStart)
{
	if(bStart)
		tg.state = 1;
	else
		tg.state = 0;
	return 0;
}
int settarget(int bStart)
{

	if(bStart)
		tg.state = 1;
	else
		tg.state = 0;
	

	return 0;
}

int GetTargetPosX(int iTagId)
{
	int iTar=0;
	if(tg.state)
	{
		iTar = max(0, iTagId - 1);
		return ((int)tg.tg[iTar].cur_x) % tg.i_width;
	}
	return 0;
}

int GetTargetPosY(int iTagId)
{
	int iTar=0;
	if(tg.state)
	{
		iTar = max(0, iTagId - 1);
		return  ((int)tg.tg[iTar].cur_y ) % tg.i_height;
	}
	return 0;
}
#endif





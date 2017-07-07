//#include <stdio.h>
#include <assert.h>
#include <cuda_runtime.h>
//#include "device_launch_parameters.h"

#define clip(minv, maxv, value)  ((value)<minv) ? minv : (((value)>maxv) ? maxv : (value))


__global__ void kernel_scale(
		float *scale, const unsigned char *src, int src_Width, int src_Height, int scale_step)
{
	const int x = blockDim.x * blockIdx.x + threadIdx.x;
	const int y = blockDim.y * blockIdx.y + threadIdx.y;

	int c_step=((src_Width)>>scale_step);
	int sx = (x<<scale_step);
	int sy = (y<<scale_step);

	int src_step = src_Width*4;

	const unsigned char *pS;

	if(x >= (src_Width>>scale_step) || y >= (src_Height>>scale_step))
		return;

	{
		unsigned char R, G, B;
		pS = src + (sy*src_step + sx*4);
		B = pS[0] ;//=  128;//clip(0, 255, (src[src_y*src_step + src_x*3] * exposure[y*dst_Width + x]));
		G = pS[1];// = 128;//clip(0, 255, (src[src_y*src_step + src_x*3 +1] * exposure[y*dst_Width + x]));
		R = pS[2];// = 128;//clip(0, 255, (src[src_y*src_step + src_x*3 +2] * exposure[y*dst_Width + x]));
		//if((y&c_mask)==0 && (x&c_mask)==0)
		{
			scale[y*c_step+x] = 0.299*R + 0.587*G + 0.114*B;
		}
	}
}

#define DESCALE(x, n)    (((x) + (1 << ((n)-1)))>>(n))
#define COEFFS_0 		(22987)
#define COEFFS_1 		(-11698)
#define COEFFS_2 		(-5636)
#define COEFFS_3 		(29049)
__global__ void kernel_uyvy2bgr(
	unsigned char *dst, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= (width>>1) || y >= height)
		return;

	{
	int Y1, Y2, U, V;
	int r, g, b;

	int si = y*width*2 + x*4;
	int di = y*width*3 + x*6;

	U =  src[si+0];
	Y1 = src[si+1];
	V =  src[si+2];
	Y2 = src[si+3];

	b = DESCALE((U - 128)*COEFFS_3, 14);
	g = DESCALE((U - 128)*COEFFS_2 + (V - 128)*COEFFS_1, 14);
	r = DESCALE((V - 128)*COEFFS_0, 14);

	dst[di+0] = clip(0, 255, Y1 + b);//B
	dst[di+1] = clip(0, 255, Y1 + g);//G
	dst[di+2] = clip(0, 255, Y1 + r);//R
	dst[di+3] = clip(0, 255, Y2 + b);//B
	dst[di+4] = clip(0, 255, Y2 + g);//G
	dst[di+5] = clip(0, 255, Y2 + r);//R
	}
}

__global__ void kernel_yuyv2bgr(
	unsigned char *dst, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= (width>>1) || y >= height)
		return;

	{
		int Y1, Y2, U, V;
		int r, g, b;

		int si = y*width*2 + x*4;
		int di = y*width*3 + x*6;

		Y1 =  src[si+0];
		U = src[si+1];
		Y2 =  src[si+2];
		V = src[si+3];

		b = DESCALE((U - 128)*COEFFS_3, 14);
		g = DESCALE((U - 128)*COEFFS_2 + (V - 128)*COEFFS_1, 14);
		r = DESCALE((V - 128)*COEFFS_0, 14);

		dst[di+0] = clip(0, 255, Y1 + b);//B
		dst[di+1] = clip(0, 255, Y1 + g);//G
		dst[di+2] = clip(0, 255, Y1 + r);//R
		dst[di+3] = clip(0, 255, Y2 + b);//B
		dst[di+4] = clip(0, 255, Y2 + g);//G
		dst[di+5] = clip(0, 255, Y2 + r);//R
	}
}

__device__ void deInterlace(unsigned char top, unsigned char bot, unsigned char mid, unsigned char *dst)
{
    *dst = clip(0, 255, (int)top + bot + mid- min(min(top, bot), mid) - max(max(top, bot), mid));
}

__device__ void deInterlaceUV(int top, int bot, int mid, unsigned char *dst)
{
    *dst = clip(0, 255, top + bot + mid- min(min(top, bot), mid) - max(max(top, bot), mid));
}

__device__ void deInterlaceY(unsigned char top[], unsigned char bot[], unsigned char mid, unsigned char *dst)
{
	const int thred = 50;
	int grd = abs(top[0] - mid) + abs(top[1] - mid) + abs(top[2] - mid) +
					   abs(bot[0] - mid) + abs(bot[1] - mid) + abs(bot[2] - mid);

	if(grd > thred){
		int grda = abs(top[0] - bot[2]);
		int grdb = abs(top[1] - bot[1]);
		int grdc = abs(top[2] - bot[0]);

		if( (grda < grdb) && (grda < grdc) )
		{
			*dst = top[0] + bot[2] + mid -min(min(top[0], bot[2]), mid) - max(max(top[0], bot[2]), mid);//medthr(a_1, b1, d);
		}
		else if( (grdc < grda) && (grdc < grdb) )
		{
			*dst = top[2] + bot[0] + mid - min(min(top[2], bot[0]), mid) - max(max(top[2], bot[0]), mid);//medthr(a1, b_1, d);
		}
		else
		{
			*dst = top[1] + bot[1] + mid- min(min(top[1], bot[1]), mid) - max(max(top[1], bot[1]), mid);//medthr(a, b, d);
		}
	}
	else
	{
		*dst = mid;
	}
}

#if 0
__global__ void kernel_dei( unsigned char *src, int width, int height)
{
	//dei
	int x = blockIdx.x * (blockDim.x ) + (threadIdx.x );
	int y = blockIdx.y * (blockDim.y )  + (threadIdx.y);

	if((x+1) >= width || (y+1) >= height)
		return;

	//return;
	width = width * 2;
	x = x*2;//x*4;
	y = y*2;

	int offset_top = (y + 0) * width + x;     //top point
    int offset_mid = (y + 1) * width + x;     //mid point
    int offset_bot = (y + 2) * width + x;     //bot point

    unsigned char topY[3], botY[3], midY;
    unsigned char topUV, botUV, midUV;

    topUV = src[offset_top + 0];       //U or V
    botUV = src[offset_bot + 0];
    midUV = src[offset_mid + 0];

    topY[0] = src[offset_top - 1];       //Y
    topY[1] = src[offset_top + 1];
    topY[2] = src[offset_top + 3];
    botY[0] = src[offset_bot - 1];
    botY[1] = src[offset_bot + 1];
    botY[2] = src[offset_bot + 3];
    midY = src[offset_mid + 1];

	//select the middle pixel to replace the mid one
    //deInterlaceUV(topUV, botUV , midUV, &src[offset_mid + 0]);
    deInterlaceY(topY, botY , midY, &src[offset_mid + 1]);

}
#else
__global__ void kernel_dei(unsigned char *src, int width, int height)
{
	//dei
	int x = blockIdx.x * (blockDim.x ) + (threadIdx.x );
	int y = blockIdx.y * (blockDim.y )  + (threadIdx.y);

	if((x+1) >= width || (y+1) >= height)
		return;

	//return;
	width = width * 2;
	x = x * 4;
	y = y*2;

	int offset_top = (y + 0) * width + x;     //top point
    int offset_mid = (y + 1) * width + x;     //mid point
    int offset_bot = (y + 2) * width + x;     //bot point

    unsigned char top[4], bot[4], mid[4];

    top[0] = src[offset_top + 0];       //U
    bot[0] = src[offset_bot + 0];
    mid[0] = src[offset_mid + 0];

    top[1] = src[offset_top + 1];       //Y
    bot[1] = src[offset_bot + 1];
    mid[1] = src[offset_mid + 1];

    top[2] = src[offset_top + 2];       //V
    bot[2] = src[offset_bot + 2];
    mid[2] = src[offset_mid + 2];

    top[3] = src[offset_top + 3];       //Y
    bot[3] = src[offset_bot + 3];
    mid[3] = src[offset_mid + 3];

	//select the middle pixel to replace the mid one
//    deInterlace(top[0], bot[0] , mid[0], &src[offset_mid + 0]);
    deInterlace(top[1], bot[1] , mid[1], &src[offset_mid + 1]);
//    deInterlace(top[2], bot[2] , mid[2], &src[offset_mid + 2]);
    deInterlace(top[3], bot[3] , mid[3], &src[offset_mid + 3]);

}
#endif

extern "C" int uyvydei_(
	unsigned char *dst,
	int width, int height)
{
	dim3 block((width/2+31)/32,(height/2+31)/32);
	dim3 thread(32, 32);
	//dim3 block((dst_Width+127)/128,(dst_Height+127)/128);
	//dim3 thread(128, 128);
	kernel_dei<<<block, thread>>>(dst, width, height);

	return 0;
}

#if 1
extern "C" int uyvy2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream)
{
	//dim3 block((width/2+15)/16, (height+63)/64);
	//dim3 thread(16, 64);
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);
	//dim3 block((width/2+127)/128,(height+127)/128);
	//dim3 thread(128, 128);

	//kernel_uyvy2bgr_and_sphere_tp_erect<<<block, thread>>>(dst, src, width, height);
	kernel_uyvy2bgr<<<block, thread, 0, stream>>>(dst, src, width, height);

	return 0;
}
extern "C" int yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream)
{
	//dim3 block((width/2+15)/16, (height+63)/64);
	//dim3 thread(16, 64);
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);
	//dim3 block((width/2+127)/128,(height+127)/128);
	//dim3 thread(128, 128);

	//kernel_uyvy2bgr_and_sphere_tp_erect<<<block, thread>>>(dst, src, width, height);
	kernel_yuyv2bgr<<<block, thread, 0, stream>>>(dst, src, width, height);

	return 0;
}
#else
extern "C" int uyvy2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height)
{
	dim3 block((width/2+15)/16, (height+63)/64);
	dim3 thread(16, 64);

	cudaChannelFormatDesc desc = cudaCreateChannelDesc<unsigned char>();
	cudaBindTexture2D(NULL, texIn, src, desc, width*2, height, width*2);

	kernel_uyvy2bgr<<<block, thread>>>(dst, src, width, height);

	cudaUnbindTexture(texIn);

	return 0;
}
#endif

extern "C" int kernel_scale_(
		float *scale, const unsigned char *src,
	int width, int height, int scale_step)
{
	dim3 block(((width>>scale_step)+31)/32,((height>>scale_step)+31)/32);
	dim3 thread(32, 32);
	//dim3 block((dst_Width+127)/128,(dst_Height+127)/128);
	//dim3 thread(128, 128);

	kernel_scale<<<block, thread>>>(scale, src, width, height, scale_step);

	return 0;
}

__global__ void kernel_yuyv2bgr_ext(
	unsigned char *dst, const unsigned char *src, unsigned char *gray,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= (width>>1) || y >= height)
		return;

	{
		int Y1, Y2, U, V;
		int r, g, b;

		int si = y*width*2 + x*4;
		int di = y*width*3 + x*6;

		Y1 =  src[si+0];
		U = src[si+1];
		Y2 =  src[si+2];
		V = src[si+3];

		gray[y*width + x*2 + 0] = Y1;
		gray[y*width + x*2 + 1] = Y2;

		b = DESCALE((U - 128)*COEFFS_3, 14);
		g = DESCALE((U - 128)*COEFFS_2 + (V - 128)*COEFFS_1, 14);
		r = DESCALE((V - 128)*COEFFS_0, 14);

		dst[di+0] = clip(0, 255, Y1 + b);//B
		dst[di+1] = clip(0, 255, Y1 + g);//G
		dst[di+2] = clip(0, 255, Y1 + r);//R
		dst[di+3] = clip(0, 255, Y2 + b);//B
		dst[di+4] = clip(0, 255, Y2 + g);//G
		dst[di+5] = clip(0, 255, Y2 + r);//R
	}
}

extern "C" int yuyv2bgr_ext_(
	unsigned char *dst, const unsigned char *src, unsigned char *gray,
	int width, int height, cudaStream_t stream)
{
	//dim3 block((width/2+15)/16, (height+63)/64);
	//dim3 thread(16, 64);
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);
	//dim3 block((width/2+127)/128,(height+127)/128);
	//dim3 thread(128, 128);

	//kernel_uyvy2bgr_and_sphere_tp_erect<<<block, thread>>>(dst, src, width, height);
	kernel_yuyv2bgr_ext<<<block, thread, 0, stream>>>(dst, src, gray, width, height);

	return 0;
}


__global__ void kernel_yuyv2gray(
	unsigned char *dst, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= width || y >= height)
		return;

	dst[y*width + x] = src[y*width*2 + x*2];
}

extern "C" int yuyv2gray_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream)
{
	dim3 block((width+31)/32,(height+31)/32);
	dim3 thread(32, 32);

	kernel_yuyv2gray<<<block, thread, 0, stream>>>(dst, src, width, height);

	return 0;
}

__global__ void kernel_yuyv2yuvplan(
	unsigned char *dsty, unsigned char *dstu, unsigned char *dstv, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;
	int uvwidth = (width>>1);

	if(x >= (width>>1) || y >= height)
		return;

	dsty[y*width + 2*x] 		= 		src[y*width*2 + x*4];
	dstu[y*uvwidth + x] 	= 		src[y*width*2 + x*4+1];
	dsty[y*width + 2*x+1] = 		src[y*width*2 + x*4+2];
	dstv[y*uvwidth + x] 		= 		src[y*width*2 + x*4+3];
}

extern "C" int yuyv2yuvplan_(
	unsigned char *dsty, unsigned char *dstu,unsigned char *dstv,const unsigned char *src,
	int width, int height, cudaStream_t stream)
{
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);

	kernel_yuyv2yuvplan<<<block, thread, 0, stream>>>(dsty,dstu,dstv, src, width, height);

	return 0;
}




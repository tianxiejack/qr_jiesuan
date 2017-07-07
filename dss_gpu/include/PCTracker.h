
/*
 * PCTracker.h
 *
 *  Created on: 2014-5-9
 *      Author: Zhong
 */

#ifndef PCTRACKER_H_
#define PCTRACKER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "osa.h"

#ifndef bool
#define bool unsigned char
#define false 0
#define true 1
#endif
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned char Uint8;

//#define UTILS_assert assert
#define Vps_printf printf

#ifndef __INLINE__
#define __INLINE__ static
#endif

/************************************/
// here is same as alglink in trackLink.h

#define MAX_DIM	3
#define MAT_float 	1
#define MAT_u8		0
typedef struct _mat_t{
	int dtype; //0: u8, 1: float
	int size;
	int width, height;
	int channels;
	int step[MAX_DIM];
	union{
	float *data;
	unsigned char *data_u8;
	};
}IMG_MAT, IMG_MAT_FLOAT, IMG_MAT_UCHAR;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif /* PCTRACKER_H_ */

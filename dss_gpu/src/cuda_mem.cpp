#include <glew.h>
#include <glut.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
//#include <cuda_runtime.h>
#include "cuda_runtime_api.h"
#include "osa_tsk.h"

typedef struct _share{
	int iRef;
	int iWrite;
	void *devPtr;
	size_t size;
}SHARE;
#define SHAREARRAY_CNT		(12)
#define SHAREARRAY_PRECHN_CNT		(5)
static SHARE shareArray[SHAREARRAY_CNT];
static bool bInitShare = false;
static cudaError_t cudaMalloc_share(void **devPtr, size_t size, int chn)
{
	cudaError_t ret = cudaSuccess;
	OSA_assert(chn >= 0 && chn < SHAREARRAY_CNT);
	OSA_assert(devPtr != NULL);
	OSA_assert(size>0);

	if(!bInitShare){
		memset(shareArray, 0, sizeof(shareArray));
		bInitShare = true;
	}
	if(shareArray[chn].size != size && shareArray[chn].devPtr != NULL){
		cudaFree(shareArray[chn].devPtr);
		shareArray[chn].devPtr = NULL;
	}
	shareArray[chn].size = size;
	if(shareArray[chn].devPtr == NULL){
		ret = cudaMalloc(&shareArray[chn].devPtr, (size+7)&(~7));
		OSA_assert(ret == cudaSuccess);
		OSA_assert(shareArray[chn].devPtr != NULL);
		shareArray[chn].iRef = 1;
		shareArray[chn].iWrite = 0;
		//OSA_printf("cudaMalloc_share:  ch%02d size = %d\n", chn, (int)size);
	}
	shareArray[chn].iRef ++;

	*devPtr = shareArray[chn].devPtr;

	return ret;
}
static cudaError_t cudaMemcpy_share(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, int chn, int flag)
{
	cudaError_t ret = cudaSuccess;
	OSA_assert(chn >= 0 && chn < SHAREARRAY_CNT);
	OSA_assert(dst != NULL);

	if(kind == cudaMemcpyDeviceToHost ){
		ret = cudaMemcpy(dst, src, count, kind);
	}else if(kind == cudaMemcpyDeviceToHost || shareArray[chn].iWrite == 0 || flag != 0){
		shareArray[chn].iWrite ++;
		ret = cudaMemcpy(dst, src, count, kind);
	}
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaMemcpy failed line[%d]!", __LINE__);
	}
	return ret;
}

static cudaError_t cudaFree_share(void *devPtr, int chn)
{
	cudaError_t ret = cudaSuccess;
	int bfree = 0;
	OSA_assert(chn >= 0 && chn < SHAREARRAY_CNT);
	//OSA_assert(devPtr != NULL);
	if(!bInitShare)
		return ret;

	if(shareArray[chn].devPtr != NULL){
		//OSA_assert(devPtr == devPtr);
		shareArray[chn].iRef --;
		if(shareArray[chn].iRef == 0){
			ret = cudaFree(shareArray[chn].devPtr);
			shareArray[chn].devPtr = NULL;
			shareArray[chn].iRef = 0;
			shareArray[chn].iWrite = 0;
			bfree = 1;
		}
	}else{
		ret = cudaFree(devPtr);
		bfree = 1;
	}
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaFree failed line[%d]!", __LINE__);
	}
	//if(bfree)
	//	printf("%s [%d]: free mId = %d\n ", __FUNCTION__, __LINE__ ,  chn);
	return ret;
}

static cudaError_t cudaAlloc_Map(void **devPtr, void **hostPtr, size_t size)
{
	cudaError_t ret = cudaSuccess;
	void *pHost = NULL;
	void *pDevice = NULL;

	ret = cudaHostAlloc(&pHost, size, cudaHostAllocDefault | cudaHostAllocMapped);
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaHostAlloc failed line[%d]!", __LINE__);
	        OSA_assert(0);
	        return ret;
	}

	ret = cudaHostGetDevicePointer(&pDevice, pHost, 0);
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaHostGetDevicePointer failed line[%d]!", __LINE__);
	        OSA_assert(0);
	}

	*devPtr = pDevice;
	*hostPtr = pHost;

	return ret;
}

static cudaError_t cudaAlloc_unMap(void *pHost)
{
	cudaError_t ret = cudaSuccess;
	ret = cudaFreeHost(pHost);

	if (ret != cudaSuccess) {
			fprintf(stderr, "cudaFreeHost failed line[%d]!", __LINE__);
	}
	return ret;
}

#define RESOURCE_CNT		(8)
#define INVALID_BUFFER    (0xFFFFFFFF)
typedef struct _resource_share{
	struct cudaGraphicsResource *resource;
	int iReg;
	int imap;
	void *devPtr;
	size_t size;
	GLuint buffer;
}RES_SHARE;
static RES_SHARE resourceArray[RESOURCE_CNT];
static bool bInitResourceShare = false;
static cudaError_t cudaResource_RegisterBuffer(int index, GLuint buffer, size_t size)
{
	cudaError_t ret = cudaSuccess;
	int i;
	OSA_assert(index>=0 && index<RESOURCE_CNT);

	if(!bInitResourceShare){
		memset(resourceArray, 0, sizeof(resourceArray));
		for(i=0; i<RESOURCE_CNT; i++)
			resourceArray[i].buffer = INVALID_BUFFER;
		bInitResourceShare = true;
	}

	if(resourceArray[index].buffer == buffer && resourceArray[index].size == size){
		resourceArray[index].iReg ++;
		return ret;
	}

	if(resourceArray[index].buffer != INVALID_BUFFER){
		cudaGraphicsUnmapResources(1, &resourceArray[index].resource, 0);
		cudaGraphicsUnregisterResource(resourceArray[index].resource);
		memset(&resourceArray[index], 0, sizeof(RES_SHARE));
		resourceArray[index].buffer = INVALID_BUFFER;
	}

	ret = cudaGraphicsGLRegisterBuffer(&resourceArray[index].resource, buffer, cudaGraphicsMapFlagsWriteDiscard);
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaGraphicsGLRegisterBuffer failed line[%d]!", __LINE__);
	        OSA_assert(0);
	}
	resourceArray[index].iReg = 1;
	resourceArray[index].buffer = buffer;
	resourceArray[index].size = size;

	return ret;
}

static cudaError_t cudaResource_UnregisterBuffer(int index)
{
	cudaError_t ret = cudaSuccess;
	OSA_assert(index>=0 && index<RESOURCE_CNT);
	OSA_assert(bInitResourceShare);

	if(resourceArray[index].iReg <= 0)
		return ret;

	resourceArray[index].iReg--;
	if(resourceArray[index].iReg==0){
		ret = cudaGraphicsUnregisterResource(resourceArray[index].resource);
		if (ret != cudaSuccess) {
		        fprintf(stderr, "cudaGraphicsUnregisterResource failed line[%d]!", __LINE__);
		        //OSA_assert(0);
		}
		memset(&resourceArray[index], 0, sizeof(RES_SHARE));
		resourceArray[index].buffer = INVALID_BUFFER;
	}
	return ret;
}

static cudaError_t cudaResource_mapBuffer(int index, void **devPtr, size_t *size)
{
	cudaError_t ret = cudaSuccess;
	OSA_assert(index>=0 && index<RESOURCE_CNT);
	OSA_assert(bInitResourceShare);

	if(resourceArray[index].imap > 0){
		*devPtr = resourceArray[index].devPtr;
		*size = resourceArray[index].size;
		resourceArray[index].imap ++;
		return ret;
	}

	ret = cudaGraphicsMapResources(1, &resourceArray[index].resource, 0);
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaGraphicsMapResources failed line[%d]!", __LINE__);
	        OSA_assert(0);
	}
	ret = cudaGraphicsResourceGetMappedPointer(&resourceArray[index].devPtr, &resourceArray[index].size,  resourceArray[index].resource);
	if (ret != cudaSuccess) {
	        fprintf(stderr, "cudaGraphicsResourceGetMappedPointer failed line[%d]!", __LINE__);
	        OSA_assert(0);
	}
	*devPtr = resourceArray[index].devPtr;
	*size = resourceArray[index].size;
	resourceArray[index].imap = 1;

	return ret;
}

static cudaError_t cudaResource_unmapBuffer(int index)
{
	cudaError_t ret = cudaSuccess;
	OSA_assert(index>=0 && index<RESOURCE_CNT);
	OSA_assert(bInitResourceShare);

	if(resourceArray[index].imap <= 0)
		return ret;

	resourceArray[index].imap--;
	if(resourceArray[index].imap==0){
		ret = cudaGraphicsUnmapResources(1, &resourceArray[index].resource, 0);
		if (ret != cudaSuccess) {
				fprintf(stderr, "cudaGraphicsUnmapResources failed line[%d]!", __LINE__);
				//OSA_assert(0);
		}
	}

	return ret;
}

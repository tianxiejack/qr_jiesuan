/*
 * MultiChVideo.cpp
 *
 *  Created on:
 *      Author: sh
 */

#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "MultiChVideo.hpp"

MultiChVideo::MultiChVideo():m_usrFunc(NULL),m_user(NULL)
{
	memset(m_thrCap, 0, sizeof(m_thrCap));
	memset(VCap, 0, MAX_CHAN*sizeof(void*));
}

MultiChVideo::~MultiChVideo()
{
	destroy();
}


int MultiChVideo::creat()
{
	/*for(int i=0; i<MAX_CHAN; i++){
		VCap[i] = new v4l2_camera(3);
		VCap[i]->creat();
	}*/
	VCap[0] = new v4l2_camera(0);
	VCap[0]->creat();
//VCap[1] = new v4l2_camera(1);
//	VCap[1]->creat();
//	VCap[2] = new v4l2_camera(3);
//	VCap[2]->creat();
//	VCap[3] = new v4l2_camera(4);
//	VCap[3]->creat();

	return 0;
}

int MultiChVideo::destroy()
{
	for(int i=0; i<MAX_CHAN; i++){
		if(VCap[i] != NULL)
		{
			stop();
			VCap[i]->stop();
			delete VCap[i];
			VCap[i] = NULL;
		}
	}
	return 0;
}

int MultiChVideo::run()
{
	int iRet = 0;

	for(int i=0; i<MAX_CHAN; i++){
		VCap[i]->run();
		m_thrCxt[i].pUser = this;
		m_thrCxt[i].chId = i;
		iRet = OSA_thrCreate(&m_thrCap[i], capThreadFunc, 0, 0, &m_thrCxt[i]);
	}
	for(int i=0; i<4; i++){
	
		m_palthrCxt[i].pUser = this;
		m_palthrCxt[i].chId = i;
		iRet = OSA_thrCreate(&m_palthrCap[i], cappal4ThreadFunc, 0, 0, &m_palthrCxt[i]);
	}

	return iRet;
}

int MultiChVideo::stop()
{
	for(int i=0; i<MAX_CHAN; i++){
		VCap[i]->stop();
		OSA_thrDelete(&m_thrCap[i]);
	}

	return 0;
}

int MultiChVideo::run(int chId)
{
	int iRet;
	if(chId<0 || chId>=MAX_CHAN)
		return -1;

	VCap[chId]->run();
	m_thrCxt[chId].pUser = this;
	m_thrCxt[chId].chId = chId;
	iRet = OSA_thrCreate(&m_thrCap[chId], capThreadFunc, 0, 0, &m_thrCxt[chId]);

	return iRet;
}

int MultiChVideo::stop(int chId)
{
	if(chId<0 || chId>=MAX_CHAN)
		return -1;

	OSA_thrDelete(&m_thrCap[chId]);
	VCap[chId]->stop();

	return 0;
}

void MultiChVideo::process()
{
	int chId;
	fd_set fds;
	struct timeval tv;
	int ret;

	Mat frame;

	FD_ZERO(&fds);

	for(chId=0; chId<MAX_CHAN; chId++){
		if(VCap[chId]->m_devFd != -1 &&VCap[chId]->bRun )
			FD_SET(VCap[chId]->m_devFd, &fds);
		//OSA_printf("MultiChVideo::process: FD_SET ch%d -- fd %d", chId, VCap[chId]->m_devFd);
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	ret = select(FD_SETSIZE, &fds, NULL, NULL, &tv);

	//OSA_printf("MultiChVideo::process: ...");

	if(-1 == ret)
	{
		if (EINTR == errno)
			return;
	}else if(0 == ret)
	{
		return;
	}

	for(chId=0; chId<MAX_CHAN; chId++){
		if(VCap[chId]->m_devFd != -1 && FD_ISSET(VCap[chId]->m_devFd, &fds)){
			struct v4l2_buffer buf;
			memset(&buf, 0, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory =V4L2_MEMORY_USERPTR;//V4L2_MEMORY_MMAP;//

			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_DQBUF, &buf))
			{
				fprintf(stderr, "cap ch%d DQBUF Error!\n", chId);
			}
			else
			{
				
				if(m_usrFunc != NULL){
					frame = cv::Mat(VCap[chId]->imgheight, VCap[chId]->imgwidth, VCap[chId]->imgtype,
							VCap[chId]->buffers[buf.index].start);

					m_usrFunc(m_user, chId, frame);
				}
				
				if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_QBUF, &buf)){
					fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
		}
	}

}

void MultiChVideo::process(int chId)
{
	fd_set fds;
	struct timeval tv;
	int ret;

	Mat frame;

	FD_ZERO(&fds);

	FD_SET(VCap[chId]->m_devFd, &fds);

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	ret = select(VCap[chId]->m_devFd+1, &fds, NULL, NULL, &tv);

	//OSA_printf("MultiChVideo::process: ...");

	if(-1 == ret)
	{
		if (EINTR == errno)
			return;
	}else if(0 == ret)
	{
		return;
	}

	if(VCap[chId]->m_devFd != -1 && FD_ISSET(VCap[chId]->m_devFd, &fds))
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;//V4L2_MEMORY_MMAP

		if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_DQBUF, &buf))
		{
			fprintf(stderr, "cap ch%d DQBUF Error!\n", chId);
		}
		else
		{
			if(m_usrFunc != NULL){

				for(int i=0; i<MAX_CHAN; i++){
					 VCap[i]->parse_line_header2(4,(unsigned char *)VCap[chId]->buffers[buf.index].start);
					}

				#if 0
				frame = cv::Mat(VCap[chId]->imgheight, VCap[chId]->imgwidth, VCap[chId]->imgtype,
						VCap[chId]->buffers[buf.index].start);
				

				m_usrFunc(m_user, chId, frame);
				#endif
			}
			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_QBUF, &buf)){
				fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}
}

void MultiChVideo::pal4process(int chid)
{

	int status;
	unsigned char *bufdata  = NULL;
	int bufId,CHID;
	int width, height;
	Mat frame;
	int nLost = 0;
	int nProcess = 0;
	int chId=0;
	Uint32 fullCnt;

	for(;;)
	{
		//sleep(10);
		//OSA_waitMsecs(100);
		//break;
		nProcess = 0;
		{

			#if 0
			fullCnt = OSA_queGetQueuedCount(&VCap[chId]->m_bufferHndl->fullQue);
			if(fullCnt>VCap[chId]->m_bufferHndl->numBuf-4){
				status = OSA_bufGetFull(VCap[chId]->m_bufferHndl, &bufId, OSA_TIMEOUT_NONE/*OSA_TIMEOUT_FOREVER*/);
				OSA_assert(status == OSA_SOK);
				//OSA_printf("%s: cap ch%d buffer frame lose!\n", __func__, *BUFFER_CHID(VCap->m_bufferHndl->bufInfo[bufId].virtAddr));
				OSA_bufPutEmpty(VCap[chId]->m_bufferHndl, bufId);
			}
			#endif
			
			status = OSA_bufGetFull(VCap[chId]->m_bufferHndl, &bufId, OSA_TIMEOUT_FOREVER);

			if(status == 0){
			//	printf("****wjGetFull ChId[%d] bufId=%d****\n", chId, bufId);
				bufdata	= BUFFER_DATA(VCap[chId]->m_bufferHndl->bufInfo[bufId].virtAddr);
				CHID = *BUFFER_CHID(VCap[chId]->m_bufferHndl->bufInfo[bufId].virtAddr);
				width	= VCap[chId]->m_bufferHndl->bufInfo[bufId].width;
				height	= VCap[chId]->m_bufferHndl->bufInfo[bufId].height;

			//	OSA_printf("bufId = %d chId = %d w x h (%d x %d)", bufId, CHID, width, height);
				
				//frame = cv::Mat(VCap[chId]->imgheight, VCap[chId]->imgwidth, VCap[chId]->imgtype,
				//		bufdata);
				//to xie tian
				frame = cv::Mat(576, 720, VCap[chId]->imgtype,
						bufdata);
				
				m_usrFunc(m_user, CHID, frame);
				

				//frame = cv::Mat(height, width, CV_8UC2, bufdata);

				///usrFunc(user, chId, frame, VCap->m_bufferHndl->bufInfo[bufId].timestamp);
				OSA_bufPutEmpty(VCap[chId]->m_bufferHndl, bufId);
				nProcess ++;
			}
		}
		//if(nProcess==0 || nProcess>=CAP_CH_NUM)
		//	break;
	}
	
}





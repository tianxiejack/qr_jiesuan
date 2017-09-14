/*
 * mvdectInterface.h
 *
 *  Created on: 2017年9月4日
 *      Author: cr
 */

#ifndef MVDECTINTERFACE_H_
#define MVDECTINTERFACE_H_

#include "infoHead.h"

typedef void ( *LPNOTIFYFUNC)(void *context, int chId);

class	CMvDectInterface
{
public:
	CMvDectInterface(){};
	virtual ~CMvDectInterface(){};

public:
	virtual	int	init(LPNOTIFYFUNC	notifyFunc, void *context){return 1;};
	virtual	int destroy(){return 1;};
	virtual	void	setFrame(cv::Mat	src, 	int chId = 0){};//输入视频帧
	virtual	void	enableSelfDraw(bool	bEnable, int chId = 0){};
	virtual	void   setROIScalXY(float scaleX = 1.0, float scaleY = 1.0, int chId = 0){};//设置缩放系数，默认为1.0

	virtual	void	clearWarningRoi(int chId	= 0){};//清除警戒区
	virtual	void	setWarningRoi(std::vector<cv::Point2i>	warnRoi,	int chId	= 0){};//设置缩放前的警戒区域
	virtual	void	setTrkThred(TRK_THRED		trkThred,	int chId	= 0){};
	virtual	void	setDrawOSD(cv::Mat	dispOSD, int chId = 0){};
	virtual	void	setWarnMode(WARN_MODE	warnMode,	int chId	= 0){};//设置警戒模式

	virtual	void	getLostTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//丢失目标
	virtual	void	getInvadeTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//入侵目标
	virtual	void	getMoveTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//移动目标
	virtual	void	getBoundTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//越界目标
	virtual	void	getWarnTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//警戒区周边所有目标

};

CMvDectInterface *MvDetector_Create();
void MvDetector_Destory(CMvDectInterface *obj);


#endif /* MVDECTINTERFACE_H_ */

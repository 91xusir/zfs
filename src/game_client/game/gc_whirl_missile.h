
#ifndef GC_WHIRL_MISSILE_H
#define GC_WHIRL_MISSILE_H

#include "gc_circle_missile.h"


//const float DEFAULT_WHIRL_MISSILE_SPEED=400.0f;


class CWhirlMissile:public CCircleMissile
{
public:
	CWhirlMissile(DWORD vSourceID,DWORD vTargetID,char vpGraphName[]);
	~CWhirlMissile(){};

	virtual void OnRun(float vSecond);
	virtual void OnArrive();

private:
	void	Revise(DWORD vSourceID,DWORD vTargetID);
	void    DealOffsetMove(float vSecond);
	bool	GetSourcePosition(DWORD vSourceID,RtgVertex3& vrPosition);
	bool    GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition);


	bool	mFirstRevise;					//是否是第一次矫正
	float   mReviseTime;					//矫正时间
	DWORD	mSourceID;						//源ID
	DWORD	mTargetID;						//目标ID
	RtgVertex3	mOldSourcePosition;			//最早的源位置
	RtgVertex3	mOldTargetPosition;			//最早的目标位置
	RtgVertex3  mOffset;					//偏移总的路程
	RtgVertex3  mEachOffsetSpeed;			//各个轴向上的偏移分速度
};







#endif//GC_WHIRL_MISSILE_H

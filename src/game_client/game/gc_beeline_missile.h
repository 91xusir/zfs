

#ifndef GC_BEELINE_MISSILE_H
#define GC_BEELINE_MISSILE_H
#include "gc_missile.h"

const float DEFAULT_BEELINE_MISSILE_SPEED=100.0f;

class CBeelineMissile:public CMissile
{
public:
	CBeelineMissile(RtgVertex3& vrSourcePosition,RtgVertex3& vrTargetPosition,
		char vpGraphName[],CMissileListener* vpListener=NULL,float vSpeed=DEFAULT_BEELINE_MISSILE_SPEED,bool isParbola = false);

	~CBeelineMissile(){};

	void SetTargetEffect(char vpEffectName[]);				//到达目的地的特效

protected:
	void SetTargetPosition(RtgVertex3& vrTargetPosition);	//设置目标位置和Missile的旋转位置
	void DealMove(float vSecond);
	virtual void OnRun(float vSecond);

	float		mTime;						//还剩多少时间到目的点
	float       mTotalTime;
	float       mCurrentMatrix_Z;
	bool         mParabola;//  产生抛物线标志
private:
	
	virtual void OnArrive();

	
	float		mSpeed;						//物体的移动速度
	RtgVertex3	mTargetPosition;			//目标的位置
	RtgVertex3  mEachSpeed;					//各个轴向上的分速度
	std::string	mTargetEffectName;			//目标点要播放的特效的名字
};



#endif//GC_BEELINE_MISSILE_H
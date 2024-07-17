

#ifndef GC_CIRCLE_MISSILE_H
#define GC_CIRCLE_MISSILE_H
#include "gc_missile.h"

const float DEFAULT_CIRCLE_MISSILE_SPEED=240.0f;			//默认的速度
const float DEFAULT_LONG_WIDTH_RATIO=0.3f;					//默认的椭圆的长宽比
const float DEFAULT_SELF_RAD=1080.0f;						//默认的自转速度[度数]

//如果是椭圆只将轨迹变成椭圆，速度处理不变，即线速度会改变[简化起见]
class CCircleMissile:public CMissile
{
public:
	CCircleMissile(
        RtgVertex3& vrSourcePosition,
        RtgVertex3& vrTargetPosition,
		char vpGraphName[],
        float vRatio=DEFAULT_LONG_WIDTH_RATIO,
        float vSelfRad=DEFAULT_SELF_RAD,
		CMissileListener* vpListener=NULL,
        float vSpeed=DEFAULT_CIRCLE_MISSILE_SPEED);

	~CCircleMissile(){};

	void SetTargetEffect(char vpEffectName[]);				//到达目的地的特效

	void SetTargetPosition(RtgVertex3& vrTargetPosition);	//设置目标位置
protected:
	
	void DealMove(float vSecond);
	virtual void OnRun(float vSecond);

	bool		mPassMiddleTime;			//是否经过了中间时间
	float		mTime;						//还剩多少时间到目的点
	float		mPassTime;					//经过了多少时间

	virtual void OnArrive();
private:

	float		mRatio;						//椭圆的长宽比
	float		mSpeed;						//圆周运动的线速度
	RtgVertex3	mTargetPosition;			//目标的位置
	RtgVertex3	mCenter;					//圆的中心位置
	float		mStartDegree;				//起始角度
	float		mRad;						//角速度[度数]
	float		mSelfRad;					//自转角速度[弧度]
	float		mR;							//半径
	std::string	mTargetEffectName;			//目标点要播放的特效的名字
};



#endif//GC_CIRCLE_MISSILE_H

#include "gc_include.h"
#include "gc_beeline_missile.h"
#include "game_assistant.h"
#include "gc_bomb_missile.h"

//const float MIN_DISTANCE=0.01;//最小距离
const float EXIST_TIME=10.0;//存活时间


CBeelineMissile::CBeelineMissile(RtgVertex3& vrSourcePosition,RtgVertex3& vrTargetPosition,char vpGraphName[],
								 CMissileListener* vpListener,float vSpeed,bool isParbola)
:CMissile(vrSourcePosition,vpGraphName,vpListener,EXIST_TIME),
mTime(0.0f),
mSpeed(vSpeed),
mTargetPosition(),
mTargetEffectName(),	
mParabola(isParbola)
{
	SetTargetPosition(vrTargetPosition);
	mCurrentMatrix_Z=mCurrentMatrix._32;
}

void CBeelineMissile::SetTargetPosition(RtgVertex3& vrTargetPosition)
{
	//距离
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));

	if(mSpeed!=0.0f)
	{
		//移动过去的时间
		mTime=Distance/mSpeed;
		mTotalTime=mTime;
		if(mTime!=0.0f)
		{
			//计算各个方向的速度
			mEachSpeed.x =(vrTargetPosition.x-mCurrentMatrix._30)/mTime;
			mEachSpeed.y =(vrTargetPosition.y-mCurrentMatrix._31)/mTime;
			mEachSpeed.z =(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//mEachSpeed.z =abs(4*mEachSpeed.x)-(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//改变方向
			mCurrentMatrix.FaceToVertex(vrTargetPosition);
		}
	}
	mTargetPosition=vrTargetPosition;
}

void CBeelineMissile::DealMove(float vSecond)
{
	mTime-=vSecond;

	if(mTime<=0.0f)
	{//到目标的时间已经过了//直接设到目标点
		mCurrentMatrix._30=mTargetPosition.x;
		mCurrentMatrix._31=mTargetPosition.y;
		mCurrentMatrix._32=mTargetPosition.z;
		//到达了目标点
		Arrive();
		DeleteSelf();
	}
	else
	{//线性处理移动
		mCurrentMatrix._30+=vSecond*mEachSpeed.x;
		mCurrentMatrix._31+=vSecond*mEachSpeed.y;
		// modified by fancui for parabola fly @2005-4-25
	    if(mParabola)
		{
            float fDiff = mTotalTime-mTime;
            if (fDiff!=0.f && mTotalTime!=0.f)
            {
                mCurrentMatrix._32=fDiff*abs(2*mEachSpeed.x)-abs(2*mEachSpeed.x)/mTotalTime*fDiff*fDiff+mCurrentMatrix_Z;
            }
		}
		else
		{
            mCurrentMatrix._32+=vSecond*mEachSpeed.z;
		}
		// end of by fancui for parabola fly @2005-4-25
	}

	//mCurrentMatrix=*GetPlayer()->GetMatrix();
}


void CBeelineMissile::OnRun(float vSecond)
{
	DealMove(vSecond);
	UpdateGraph();
}

void CBeelineMissile::OnArrive()
{
	//到达目的点的爆炸
	if(mTargetEffectName!="")
	{
		RT_NEW CBombMissile(mTargetPosition,(char*)mTargetEffectName.c_str());
	}
}

void CBeelineMissile::SetTargetEffect(char vpEffectName[])				//到达目的地的特效
{
	mTargetEffectName=vpEffectName;
}


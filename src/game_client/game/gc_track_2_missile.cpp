
#include "gc_include.h"
#include "gc_track_2_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.1;//每多少秒校正一次

CTrack2Missile::CTrack2Missile(RtgVertex3& vrSourcePosition,DWORD vTargetID,
							 char vpGraphName[],CMissileListener* vpListener,float vSpeed)
							 :CCircleMissile(vrSourcePosition,vrSourcePosition,vpGraphName),

mFirstRevise(true),				
mReviseTime(0.0f),
mTargetID(vTargetID),
mOldTargetPosition(0,0,0),		
mOffset(0,0,0),					
mEachOffsetSpeed(0,0,0)			
{
	//如果目标存在从新设置目标的位置
	Revise(mTargetID);		//如果目标存在从新设置目标的位置

}

void CTrack2Missile::OnRun(float vSecond)
{
	//校正目标位置时间到
	mReviseTime+=vSecond;
	if(mReviseTime>=REVISE_TIME)
	{
		mReviseTime=0.0f;
		Revise(mTargetID);
	}
	DealMove(vSecond);
	DealOffsetMove(vSecond);

	UpdateGraph();
}


bool    CTrack2Missile::GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition)
{
	GcActor* p=FindActor(vTargetID);
	if(p==NULL)
	{
		return false;
	}

	vrPosition= p->mBaseActor.GetGraph()->p()->GetBoundingBox()->vPos;

	vrPosition.x += p->GetGraph()->p()->GetWorldMatrix()->_30;
	vrPosition.y += p->GetGraph()->p()->GetWorldMatrix()->_31;
	vrPosition.z += p->GetGraph()->p()->GetWorldMatrix()->_32;
	return true;
}

void CTrack2Missile::DealOffsetMove(float vSecond)
{

	//计算偏移总量
	mOffset.x+=vSecond*mEachOffsetSpeed.x;
	mOffset.y+=vSecond*mEachOffsetSpeed.y;
	mOffset.z+=vSecond*mEachOffsetSpeed.z;


	//线性处理移动
	mCurrentMatrix._30+=mOffset.x;
	mCurrentMatrix._31+=mOffset.y;
	mCurrentMatrix._32+=mOffset.z;

	return;
}

void	CTrack2Missile::Revise(DWORD vTargetID)
{
	if(mFirstRevise)
	{
		mFirstRevise=false;
		if(!GetTargetPosition(vTargetID,mOldTargetPosition))
		{//找不到目标
			return;
		}
		//设置目标点
		SetTargetPosition(mOldTargetPosition);
	}
	else
	{//以后的矫正
		RtgVertex3	Position;
		if(GetTargetPosition(vTargetID,Position))
		{//得到好目标的位置
			//计算分速度//对原始位置的偏移并扣除已经走过的偏移，再除还有的时间
			mEachOffsetSpeed.x=((Position.x-mOldTargetPosition.x)-mOffset.x)/mTime;
			mEachOffsetSpeed.y=((Position.y-mOldTargetPosition.y)-mOffset.y)/mTime;
			mEachOffsetSpeed.z=((Position.z-mOldTargetPosition.z)-mOffset.z)/mTime;
		}
	}
}

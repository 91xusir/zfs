
#include "gc_include.h"
#include "gc_track_2_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.1;//ÿ������У��һ��

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
	//���Ŀ����ڴ�������Ŀ���λ��
	Revise(mTargetID);		//���Ŀ����ڴ�������Ŀ���λ��

}

void CTrack2Missile::OnRun(float vSecond)
{
	//У��Ŀ��λ��ʱ�䵽
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

	//����ƫ������
	mOffset.x+=vSecond*mEachOffsetSpeed.x;
	mOffset.y+=vSecond*mEachOffsetSpeed.y;
	mOffset.z+=vSecond*mEachOffsetSpeed.z;


	//���Դ����ƶ�
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
		{//�Ҳ���Ŀ��
			return;
		}
		//����Ŀ���
		SetTargetPosition(mOldTargetPosition);
	}
	else
	{//�Ժ�Ľ���
		RtgVertex3	Position;
		if(GetTargetPosition(vTargetID,Position))
		{//�õ���Ŀ���λ��
			//������ٶ�//��ԭʼλ�õ�ƫ�Ʋ��۳��Ѿ��߹���ƫ�ƣ��ٳ����е�ʱ��
			mEachOffsetSpeed.x=((Position.x-mOldTargetPosition.x)-mOffset.x)/mTime;
			mEachOffsetSpeed.y=((Position.y-mOldTargetPosition.y)-mOffset.y)/mTime;
			mEachOffsetSpeed.z=((Position.z-mOldTargetPosition.z)-mOffset.z)/mTime;
		}
	}
}

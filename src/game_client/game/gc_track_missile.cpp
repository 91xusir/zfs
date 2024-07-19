
#include "gc_include.h"
#include "gc_track_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.1;//ÿ������У��һ��

CTrackMissile::CTrackMissile(RtgVertex3& vrSourcePosition,DWORD vTargetID,
							 char vpGraphName[],CMissileListener* vpListener,float vSpeed)
:CBeelineMissile(vrSourcePosition,vrSourcePosition,vpGraphName,vpListener,vSpeed),
mReviseTime(0.0f),
mTargetID(vTargetID)
{
	//���Ŀ����ڴ�������Ŀ���λ��
	SetTargetFromID(mTargetID);

}

void CTrackMissile::OnRun(float vSecond)
{
	//У��Ŀ��λ��ʱ�䵽
	mReviseTime+=vSecond;
	if(mReviseTime>=REVISE_TIME)
	{
		mReviseTime=0.0f;
		SetTargetFromID(mTargetID);
	}

	CBeelineMissile::OnRun(vSecond);
}

void	CTrackMissile::SetTargetFromID(DWORD vTargetID)
{
	GcActor* p=FindActor(vTargetID);
	if(p==NULL)
	{
		return ;
	}

    RtgVertex3	TargetVertex3 = p->mBaseActor.GetGraph()->p()->GetBoundingBox()->vPos;

    TargetVertex3.x += p->GetGraph()->p()->GetWorldMatrix()->_30;
	TargetVertex3.y += p->GetGraph()->p()->GetWorldMatrix()->_31;
	TargetVertex3.z += p->GetGraph()->p()->GetWorldMatrix()->_32;

    SetTargetPosition(TargetVertex3);
	//UpdateGraph();

	return ;
}

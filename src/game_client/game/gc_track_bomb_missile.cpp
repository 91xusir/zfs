
#include "gc_include.h"
#include "gc_track_bomb_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.03;//每多少秒校正一次

CTrackBombMissile::CTrackBombMissile(DWORD vTargetID,char vpGraphName[])
:CBombMissile(RtgVertex3(0.0f,0.0f,0.0f),vpGraphName),
mReviseTime(0.0f),
mTargetID(vTargetID)
{
	//如果目标存在从新设置目标的位置
	SetTargetFromID(mTargetID);
}




void CTrackBombMissile::OnRun(float vSecond)
{
	//校正目标位置时间到
	mReviseTime+=vSecond;
	if(mReviseTime>=REVISE_TIME)
	{
		mReviseTime=0.0f;
		SetTargetFromID(mTargetID);
		UpdateGraph();
	}

	CBombMissile::OnRun(vSecond);

}


void	CTrackBombMissile::SetTargetFromID(DWORD vTargetID)
{
	GcActor* p=FindActor(vTargetID);
	if(p==NULL)
	{
		return ;
	}

	mCurrentMatrix._30 = p->GetMatrix()->_30;
	mCurrentMatrix._31 = p->GetMatrix()->_31;
	mCurrentMatrix._32 = p->GetMatrix()->_32;
	
	return ;
}



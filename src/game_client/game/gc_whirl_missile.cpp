
#include "gc_include.h"
#include "gc_whirl_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.01;//每多少秒校正一次


CWhirlMissile::CWhirlMissile(DWORD vSourceID,DWORD vTargetID,char vpGraphName[])
							 :CCircleMissile(RtgVertex3(0,0,0),RtgVertex3(0,0,0),vpGraphName),
							mFirstRevise(true),					//是否是第一次矫正
							mReviseTime(0.0f),					//矫正时间
							mSourceID(vSourceID),						//源ID
							mTargetID(vTargetID),				//目标ID
							mOldSourcePosition(),		//上一次的源位置
							mOldTargetPosition(),		//上一次的目标位置
							mOffset(),
							mEachOffsetSpeed()			//各个轴向上的偏移分速度
{
	mEachOffsetSpeed.Zero();	//各个轴向上的偏移分速度
	mOffset.Zero();
	Revise(mSourceID,mTargetID);//如果目标存在从新设置目标的位置

}

void CWhirlMissile::OnRun(float vSecond)
{
	//校正目标位置时间到
	mReviseTime+=vSecond;
	if(mReviseTime>=REVISE_TIME)
	{
		mReviseTime=0.0f;
		Revise(mSourceID,mTargetID);
	}
	DealMove(vSecond);
	DealOffsetMove(vSecond);
	UpdateGraph();
}


void    CWhirlMissile::DealOffsetMove(float vSecond)
{
	//计算偏移总量
	mOffset.x+=vSecond*mEachOffsetSpeed.x;
	mOffset.y+=vSecond*mEachOffsetSpeed.y;
	mOffset.z+=vSecond*mEachOffsetSpeed.z;

	//圆的位置是每次都从新计算的，所以要加总的偏移量
	mCurrentMatrix._30+=mOffset.x;
	mCurrentMatrix._31+=mOffset.y;
	//mCurrentMatrix._32+=mOffset.z;
	return;
}


void	CWhirlMissile::Revise(DWORD vSourceID,DWORD vTargetID)
{
	if(mFirstRevise)
	{//第一次矫正目标
		mFirstRevise=false;
		if(!GetSourcePosition(vSourceID,mOldSourcePosition))
		{//找不到源
			return;
		}

		if(!GetTargetPosition(vTargetID,mOldTargetPosition))
		{//找不到目标
			return;
		}

		//当前的位置
		mCurrentMatrix._30=mOldSourcePosition.x;
		mCurrentMatrix._31=mOldSourcePosition.y;
		mCurrentMatrix._32=mOldSourcePosition.z;

		//设置目标点
		SetTargetPosition(mOldTargetPosition);
	}
	else
	{//以后的矫正
		float ToMiddleTime=(mTime-mPassTime)/2;//到中点还需要的时间

		if(ToMiddleTime>0.0f)
		{//没过中间时间，矫正去的目标

			RtgVertex3	Position;
			if(GetTargetPosition(vTargetID,Position))
			{//得到好目标的位置
				//计算分速度//对原始位置的偏移并扣除已经走过的偏移，再除还有的时间
				mEachOffsetSpeed.x=((Position.x-mOldTargetPosition.x)-mOffset.x)/ToMiddleTime;
				mEachOffsetSpeed.y=((Position.y-mOldTargetPosition.y)-mOffset.y)/ToMiddleTime;
				mEachOffsetSpeed.z=((Position.z-mOldTargetPosition.z)-mOffset.z)/ToMiddleTime;
			}
		}
		else
		{//如果是过了中间时间，则矫正回来的目标
			RtgVertex3 Position;
			if(GetSourcePosition(vSourceID,Position))
			{//得到源的位置

				if(mTime!=0.0f)
				{
					//计算分速度//对原始位置的偏移并扣除已经走过的偏移，再除还有的时间
					mEachOffsetSpeed.x=((Position.x-mOldSourcePosition.x)-mOffset.x)/mTime;
					mEachOffsetSpeed.y=((Position.y-mOldSourcePosition.y)-mOffset.y)/mTime;
					mEachOffsetSpeed.z=((Position.z-mOldSourcePosition.z)-mOffset.z)/mTime;
				}
			}
		}
	}
	return;
}






bool	CWhirlMissile::GetSourcePosition(DWORD vSourceID,RtgVertex3& vrPosition)
{
	GcActor* pSourceActor=FindActor(vSourceID);
	if(pSourceActor==NULL)
	{
		return false;
	}

	//源的矩阵
	const RtgMatrix12* pSourceMatrix;
	const RtgMatrix12* pBaseMatrix=pSourceActor->GetGraph()->p()->GetWorldMatrix();
	
	
	if(pSourceActor->mItem.IsEquip(CItemContainer::WEAPON_1))
	{
		CRT_ActorInstance* p;
		p=pSourceActor->mItem.GetGraph(CItemContainer::WEAPON_1)->p();
		if(p==NULL)
		{
			p=pSourceActor->mItem.GetGraph(CItemContainer::WEAPON_2)->p();
		}


		if(p==NULL)
		{
			pSourceMatrix=pBaseMatrix;
		}
		else
		{
			pSourceMatrix=p->GetWorldMatrix();
		}
	}
	else
	{
		pSourceMatrix=pBaseMatrix;
	}
	vrPosition.x=pSourceMatrix->_30;
	vrPosition.y=pSourceMatrix->_31;
	vrPosition.z=pSourceMatrix->_32;

	return true;
}

void CWhirlMissile::OnArrive()
{
	//将偏移量清零
	mOffset.Zero();
	CCircleMissile::OnArrive();
}



bool   CWhirlMissile::GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition)
{
	//目标
	GcActor* pActor=FindActor(vTargetID);
	if(pActor==NULL)
	{
		return false;
	}

	vrPosition = pActor->GetGraph()->p()->GetBoundingBox()->vPos;

	vrPosition.x += pActor->GetGraph()->p()->GetWorldMatrix()->_30;
	vrPosition.y += pActor->GetGraph()->p()->GetWorldMatrix()->_31;
	vrPosition.z += pActor->GetGraph()->p()->GetWorldMatrix()->_32;

	return true;
}














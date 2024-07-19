
#include "gc_include.h"
#include "gc_whirl_missile.h"
#include "game_assistant.h"

const float REVISE_TIME=0.01;//ÿ������У��һ��


CWhirlMissile::CWhirlMissile(DWORD vSourceID,DWORD vTargetID,char vpGraphName[])
							 :CCircleMissile(RtgVertex3(0,0,0),RtgVertex3(0,0,0),vpGraphName),
							mFirstRevise(true),					//�Ƿ��ǵ�һ�ν���
							mReviseTime(0.0f),					//����ʱ��
							mSourceID(vSourceID),						//ԴID
							mTargetID(vTargetID),				//Ŀ��ID
							mOldSourcePosition(),		//��һ�ε�Դλ��
							mOldTargetPosition(),		//��һ�ε�Ŀ��λ��
							mOffset(),
							mEachOffsetSpeed()			//���������ϵ�ƫ�Ʒ��ٶ�
{
	mEachOffsetSpeed.Zero();	//���������ϵ�ƫ�Ʒ��ٶ�
	mOffset.Zero();
	Revise(mSourceID,mTargetID);//���Ŀ����ڴ�������Ŀ���λ��

}

void CWhirlMissile::OnRun(float vSecond)
{
	//У��Ŀ��λ��ʱ�䵽
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
	//����ƫ������
	mOffset.x+=vSecond*mEachOffsetSpeed.x;
	mOffset.y+=vSecond*mEachOffsetSpeed.y;
	mOffset.z+=vSecond*mEachOffsetSpeed.z;

	//Բ��λ����ÿ�ζ����¼���ģ�����Ҫ���ܵ�ƫ����
	mCurrentMatrix._30+=mOffset.x;
	mCurrentMatrix._31+=mOffset.y;
	//mCurrentMatrix._32+=mOffset.z;
	return;
}


void	CWhirlMissile::Revise(DWORD vSourceID,DWORD vTargetID)
{
	if(mFirstRevise)
	{//��һ�ν���Ŀ��
		mFirstRevise=false;
		if(!GetSourcePosition(vSourceID,mOldSourcePosition))
		{//�Ҳ���Դ
			return;
		}

		if(!GetTargetPosition(vTargetID,mOldTargetPosition))
		{//�Ҳ���Ŀ��
			return;
		}

		//��ǰ��λ��
		mCurrentMatrix._30=mOldSourcePosition.x;
		mCurrentMatrix._31=mOldSourcePosition.y;
		mCurrentMatrix._32=mOldSourcePosition.z;

		//����Ŀ���
		SetTargetPosition(mOldTargetPosition);
	}
	else
	{//�Ժ�Ľ���
		float ToMiddleTime=(mTime-mPassTime)/2;//���е㻹��Ҫ��ʱ��

		if(ToMiddleTime>0.0f)
		{//û���м�ʱ�䣬����ȥ��Ŀ��

			RtgVertex3	Position;
			if(GetTargetPosition(vTargetID,Position))
			{//�õ���Ŀ���λ��
				//������ٶ�//��ԭʼλ�õ�ƫ�Ʋ��۳��Ѿ��߹���ƫ�ƣ��ٳ����е�ʱ��
				mEachOffsetSpeed.x=((Position.x-mOldTargetPosition.x)-mOffset.x)/ToMiddleTime;
				mEachOffsetSpeed.y=((Position.y-mOldTargetPosition.y)-mOffset.y)/ToMiddleTime;
				mEachOffsetSpeed.z=((Position.z-mOldTargetPosition.z)-mOffset.z)/ToMiddleTime;
			}
		}
		else
		{//����ǹ����м�ʱ�䣬�����������Ŀ��
			RtgVertex3 Position;
			if(GetSourcePosition(vSourceID,Position))
			{//�õ�Դ��λ��

				if(mTime!=0.0f)
				{
					//������ٶ�//��ԭʼλ�õ�ƫ�Ʋ��۳��Ѿ��߹���ƫ�ƣ��ٳ����е�ʱ��
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

	//Դ�ľ���
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
	//��ƫ��������
	mOffset.Zero();
	CCircleMissile::OnArrive();
}



bool   CWhirlMissile::GetTargetPosition(DWORD vTargetID,RtgVertex3& vrPosition)
{
	//Ŀ��
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














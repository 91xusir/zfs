
#include "gc_include.h"
#include "gc_beeline_missile.h"
#include "game_assistant.h"
#include "gc_bomb_missile.h"

//const float MIN_DISTANCE=0.01;//��С����
const float EXIST_TIME=10.0;//���ʱ��


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
	//����
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));

	if(mSpeed!=0.0f)
	{
		//�ƶ���ȥ��ʱ��
		mTime=Distance/mSpeed;
		mTotalTime=mTime;
		if(mTime!=0.0f)
		{
			//�������������ٶ�
			mEachSpeed.x =(vrTargetPosition.x-mCurrentMatrix._30)/mTime;
			mEachSpeed.y =(vrTargetPosition.y-mCurrentMatrix._31)/mTime;
			mEachSpeed.z =(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//mEachSpeed.z =abs(4*mEachSpeed.x)-(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//�ı䷽��
			mCurrentMatrix.FaceToVertex(vrTargetPosition);
		}
	}
	mTargetPosition=vrTargetPosition;
}

void CBeelineMissile::DealMove(float vSecond)
{
	mTime-=vSecond;

	if(mTime<=0.0f)
	{//��Ŀ���ʱ���Ѿ�����//ֱ���赽Ŀ���
		mCurrentMatrix._30=mTargetPosition.x;
		mCurrentMatrix._31=mTargetPosition.y;
		mCurrentMatrix._32=mTargetPosition.z;
		//������Ŀ���
		Arrive();
		DeleteSelf();
	}
	else
	{//���Դ����ƶ�
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
	//����Ŀ�ĵ�ı�ը
	if(mTargetEffectName!="")
	{
		RT_NEW CBombMissile(mTargetPosition,(char*)mTargetEffectName.c_str());
	}
}

void CBeelineMissile::SetTargetEffect(char vpEffectName[])				//����Ŀ�ĵص���Ч
{
	mTargetEffectName=vpEffectName;
}


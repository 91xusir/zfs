


#include "gc_include.h"
#include "gc_complex_missile.h"
#include "game_assistant.h"


const float REVISE_TIME=0.1;
const float DEFAULT_FOLLOW_DISTANCE=250.0f;
const float DEFAULT_SPEED=200.0f;
const float DEFAULT_ATTACK_DISTANCE=40.0f;
const float DEFAULT_MIN_READY_ATTACK_DISTANCE=80.0f;
const float DEFAULT_MAX_READY_ATTACK_DISTANCE=120.0f;


CComplexMissile::CComplexMissile()
:CMissile(RtgVertex3(0,0,0),"siw81.act"),
mpMaster(NULL),
mTime(0.0f),
mReviseTime(0.0f),
mMaxFollowDistance(DEFAULT_FOLLOW_DISTANCE)	,			//���ĸ������
mSpeed(DEFAULT_SPEED),							//�ƶ��ٶ�
mTargetPosition(),				//Ŀ���λ��
mEachSpeed(),						//���������ϵķ��ٶ�
mAttackDistance(DEFAULT_ATTACK_DISTANCE),				//��������
mMaxReadyAttackDistance(DEFAULT_MAX_READY_ATTACK_DISTANCE),		//���׼����������
mMinReadyAttackDistance(DEFAULT_MIN_READY_ATTACK_DISTANCE),		//��С׼����������
mAttackID(0),						//��ǰ����Ŀ��
mpFSM(NULL),					//״̬��//������ǰ��״̬
mState(FOLLOW_STATE)					//��ǰ״̬
{
	
	
	mpFSM=Game::CFSM::mManager.Get(string("trump_state"));
	if(mpFSM==NULL)
	{
		ERR("�޷��õ�״̬��[trump_state]");
	}
	mState=FOLLOW_STATE;

	RtgVertex3 Color;
	Color.Set(0.5);
	Graph().p()->SetBaseColor(Color);
	//mCurrentMatrix.RotateLX(PI*3/4);//�ض��Ĵ���ѷɻ���ƽ
}

void CComplexMissile::SetMaster(GcBaseActor*	vpMaster)
{
	mpMaster=vpMaster;
}

/*
void CComplexMissile::SetAttackID(int	vAttackID)
{
	mAttackID=vAttackID;
}*/


//����ƽʱ��������ߣ���������
//����������ĳ��ʱ������ֱ�߷���Ŀ����Χ�ɹ�����Χ�ڵ�ĳ���㣬
//Ȼ���룬��Ŀ����Χ������λ��С��Χ�ڵ�ĳ���㣬
//Ȼ��׼����������Ŀ����Χ������λ��С�����Χ֮���ĳ���㣬
//Ȼ�󹥻�����Ŀ����Χ�ɹ�����Χ�ڵ�ĳ����
//Ŀ��������Ŀ�곬��������룬��ص��������


void CComplexMissile::OnRun(float vSecond)
{
	ECommands Command =DealCommand();
	DealState(Command);
	DealMove(vSecond);
}


void CComplexMissile::DealMove(float vSecond)
{
	switch(mState)
	{
	case FOLLOW_STATE:
		{
			if(mpMaster==NULL)
			{
				return ;
			}

			const RtgMatrix16*  pMatrix = mpMaster->GetMatrix();

			SelfRoation(vSecond);

			mCurrentMatrix._30=pMatrix->_30;
			mCurrentMatrix._31=pMatrix->_31;//+20.0f;
			mCurrentMatrix._32=pMatrix->_32;//+40.0f;
		}
		break;
	case ATTACK_STATE:
	case RECOIL_STATE:
	case READY_ATTACK_STATE:
		{
			Move(vSecond);
		}
		break;
	case BACK_HOME_STATE:
		{
			//׷������
			//У��Ŀ��λ��ʱ�䵽
			mReviseTime+=vSecond;
			if(mReviseTime>=REVISE_TIME)
			{
				mReviseTime=0.0f;
				SetTargetToMaster();
			}

			//�ƶ�����
			Move(vSecond);
		}
		break;
	default:
		break;
	}
	UpdateGraph();
}


const float SELF_ROATION=3*PI;
void CComplexMissile::SelfRoation(float vSecond)
{
	mCurrentMatrix.RotateLY(vSecond*SELF_ROATION);
}

void CComplexMissile::Move(float vSecond)
{
	//�ƶ���Ŀ���
	mTime-=vSecond;

	if(mTime<=0.0f)
	{//��Ŀ���ʱ���Ѿ�����//ֱ���赽Ŀ���
		mCurrentMatrix._30=mTargetPosition.x;
		mCurrentMatrix._31=mTargetPosition.y;
		mCurrentMatrix._32=mTargetPosition.z;
	}
	else
	{//���Դ����ƶ�
		mCurrentMatrix._30+=vSecond*mEachSpeed.x;
		mCurrentMatrix._31+=vSecond*mEachSpeed.y;
		mCurrentMatrix._32+=vSecond*mEachSpeed.z;
	}
}

RtgVertex3 CComplexMissile::GetActorPosition()
{

	RtgVertex3 Temp=mpAttackActor->GetGraph()->GetBoundingBox()->vPos;

	Temp.x+=mpAttackActor->GetMatrix()->_30;
	Temp.y+=mpAttackActor->GetMatrix()->_31;
	Temp.z+=mpAttackActor->GetMatrix()->_32;

	return Temp;
}


void CComplexMissile::DealState(ECommands vCommand)
{
	EStates				State;	//��ǰ״̬

	State=(EStates)mpFSM->Transition(vCommand,mState);
	if(State!=mState)
	{//״̬�б䶯
		mState=State;
		switch(mState)
		{
		case FOLLOW_STATE:
			break;
		case ATTACK_STATE:
			//���ù���Ŀ���
			{
				int Degree=rand()%360;
				int r=mAttackDistance*(rand()%100)/100;
				
				RtgVertex3 Temp=GetActorPosition();
				Temp.x+=r*QuickSin(Degree);
				Temp.y+=r*QuickCos(Degree);
				SetTargetPosition(Temp);
			}
			break;
		case RECOIL_STATE:
			//���ó���Ŀ���
			{
				int Degree=rand()%360;
				int r=mMinReadyAttackDistance*(rand()%100)/100;

				RtgVertex3 Temp=GetActorPosition();
				Temp.x+=r*QuickSin(Degree);
				Temp.y+=r*QuickCos(Degree);
				SetTargetPosition(Temp);
			}
			
			break;
		case READY_ATTACK_STATE:
			//����׼������Ŀ���
			{
				int Degree=rand()%360;
				int r=(mMaxReadyAttackDistance-mMinReadyAttackDistance)*(rand()%100)/100+mMinReadyAttackDistance;

				RtgVertex3 Temp=GetActorPosition();
				Temp.x+=r*QuickSin(Degree);
				Temp.y+=r*QuickCos(Degree);
				SetTargetPosition(Temp);
			}
			break;
		case BACK_HOME_STATE:
			break;
		default:
			break;
		}
	}
	return ;
}



CComplexMissile::ECommands CComplexMissile::DealCommand()
{
	ECommands Command=NULL_COMMAND;

	if(mAttackID!=0)
	{
		//�޷��ҵ�Ŀ��,��Ŀ��ID��Ϊ��
		mpAttackActor=FindActor(mAttackID);
		if(mpAttackActor==NULL)
		{
			mAttackID=0;
		}
		else
		{//����ͬ�����ľ��볬������������룬��Ŀ��ID��Ϊ��
			const RtgMatrix16* pMatrix=mpMaster->GetMatrix();
			if(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
				pMatrix->_30,pMatrix->_31,pMatrix->_32) > mMaxFollowDistance*mMaxFollowDistance)
			{
				mAttackID=0;
			}
		}
	}

	//Ŀ�겻���ڣ�ֹͣ����
	if(mAttackID==0)
	{
		switch(mState)
		{
		case FOLLOW_STATE:
		case ATTACK_STATE:
		case RECOIL_STATE:
		case READY_ATTACK_STATE:
			Command=STOP_COMMAND;
			break;
		case BACK_HOME_STATE:
			//��������//�´��������
			if(mTime<=0.0f)
			{
				Command=FOLLOW_COMMAND;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(mState)
		{
		case FOLLOW_STATE:
			{
				//�´﹥������
				Command=ATTACK_COMMAND;
			}
			break;
		case ATTACK_STATE:
			{
				//����Ŀ���//�´ﳷ������
				if(mTime<=0.0f)
				{
					Command=RECOIL_COMMAND;
				}
			}
			break;
		case RECOIL_STATE:
			{
				//����Ŀ���//�´�׼����������
				if(mTime<=0.0f)
				{
					Command=READY_ATTACK_COMMAND;
				}
			}
			break;
		case READY_ATTACK_STATE:
			{
				//����Ŀ���//�´﹥������
				if(mTime<=0.0f)
				{
					Command=ATTACK_COMMAND;
				}
			}
			break;
		case BACK_HOME_STATE:
			{
				//�´﹥������
				Command=ATTACK_COMMAND;
			}
			break;
		default:
			break;
		}
	}

	return Command;
}

void CComplexMissile::SetTargetPosition(RtgVertex3& vrTargetPosition)
{
	//����
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));

	if(mSpeed!=0.0f)
	{
		//�ƶ���ȥ��ʱ��
		mTime=Distance/mSpeed;
		if(mTime!=0.0f)
		{
			//�������������ٶ�
			mEachSpeed.x =(vrTargetPosition.x-mCurrentMatrix._30)/mTime;
			mEachSpeed.y =(vrTargetPosition.y-mCurrentMatrix._31)/mTime;
			mEachSpeed.z =(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//�ı䷽��
			//mCurrentMatrix.FaceToVertex(vrTargetPosition);
		}
	}
	mTargetPosition=vrTargetPosition;
}

void CComplexMissile::SetAttackID(DWORD vAttackID)
{
	mAttackID=vAttackID;
}
void CComplexMissile::SetTargetToMaster()
{
	if(mpMaster==NULL)
	{
		return ;
	}

	RtgVertex3	TargetVertex3 = mpMaster->GetGraph()->p()->GetBoundingBox()->vPos;

	TargetVertex3.x += mpMaster->GetGraph()->p()->GetWorldMatrix()->_30;
	TargetVertex3.y += mpMaster->GetGraph()->p()->GetWorldMatrix()->_31;
	TargetVertex3.z += mpMaster->GetGraph()->p()->GetWorldMatrix()->_32;

	SetTargetPosition(TargetVertex3);
}

#if DO_CONSOLE
const int LINE_HEIGHT=20;
void CComplexMissile::DebugInfo(int vX,int vY)
{
	string DebugString;
	int Y=vY;
	//��ǰ״̬
	switch(mState)
	{
	case NULL_STATE:
		DebugString="NULL_STATE";
		break;
	case FOLLOW_STATE:
		DebugString="FOLLOW_STATE";
		break;
	case ATTACK_STATE:
		DebugString="ATTACK_STATE";
		break;
	case RECOIL_STATE:
		DebugString="RECOIL_STATE";
		break;
	case READY_ATTACK_STATE:
		DebugString="READY_ATTACK_STATE";
		break;
	case BACK_HOME_STATE:
		DebugString="BACK_HOME_STATE";
		break;
	default:
		DebugString="UNKNOW_STATE!!!";
		break;
	}

	DebugString="Trump:["+DebugString+"]";
	GetDevice()->DrawString(vX,Y,0xffffffff,DebugString.c_str());
	Y+=LINE_HEIGHT;
	DebugString="TargetID:[";
	DebugString=DebugString+(int)mAttackID;
	DebugString=DebugString+"]";

	GetDevice()->DrawString(vX,Y,0xffffffff,DebugString.c_str());
}

#endif//DO_CONSOLE
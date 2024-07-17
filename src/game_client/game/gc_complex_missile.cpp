


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
mMaxFollowDistance(DEFAULT_FOLLOW_DISTANCE)	,			//最大的跟随距离
mSpeed(DEFAULT_SPEED),							//移动速度
mTargetPosition(),				//目标的位置
mEachSpeed(),						//各个轴向上的分速度
mAttackDistance(DEFAULT_ATTACK_DISTANCE),				//攻击距离
mMaxReadyAttackDistance(DEFAULT_MAX_READY_ATTACK_DISTANCE),		//最大准备攻击距离
mMinReadyAttackDistance(DEFAULT_MIN_READY_ATTACK_DISTANCE),		//最小准备攻击距离
mAttackID(0),						//当前攻击目标
mpFSM(NULL),					//状态机//法宝当前的状态
mState(FOLLOW_STATE)					//当前状态
{
	
	
	mpFSM=Game::CFSM::mManager.Get(string("trump_state"));
	if(mpFSM==NULL)
	{
		ERR("无法得到状态机[trump_state]");
	}
	mState=FOLLOW_STATE;

	RtgVertex3 Color;
	Color.Set(0.5);
	Graph().p()->SetBaseColor(Color);
	//mCurrentMatrix.RotateLX(PI*3/4);//特定的代码把飞环躺平
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


//宝物平时在宿主身边，跟随宿主
//当宿主进攻某人时，宝物直线飞向目标周围可攻击范围内的某个点，
//然后撤离，到目标周围攻击就位最小范围内的某个点，
//然后准备进攻，到目标周围攻击就位最小和最大范围之间的某个点，
//然后攻击，到目标周围可攻击范围内的某个点
//目标死亡或，目标超过跟随距离，则回到宿主身边


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
			//追踪宿主
			//校正目标位置时间到
			mReviseTime+=vSecond;
			if(mReviseTime>=REVISE_TIME)
			{
				mReviseTime=0.0f;
				SetTargetToMaster();
			}

			//移动处理
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
	//移动到目标点
	mTime-=vSecond;

	if(mTime<=0.0f)
	{//到目标的时间已经过了//直接设到目标点
		mCurrentMatrix._30=mTargetPosition.x;
		mCurrentMatrix._31=mTargetPosition.y;
		mCurrentMatrix._32=mTargetPosition.z;
	}
	else
	{//线性处理移动
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
	EStates				State;	//当前状态

	State=(EStates)mpFSM->Transition(vCommand,mState);
	if(State!=mState)
	{//状态有变动
		mState=State;
		switch(mState)
		{
		case FOLLOW_STATE:
			break;
		case ATTACK_STATE:
			//设置攻击目标点
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
			//设置撤退目标点
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
			//设置准备攻击目标点
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
		//无法找到目标,将目标ID变为零
		mpAttackActor=FindActor(mAttackID);
		if(mpAttackActor==NULL)
		{
			mAttackID=0;
		}
		else
		{//法宝同宿主的距离超过了最大跟随距离，将目标ID变为零
			const RtgMatrix16* pMatrix=mpMaster->GetMatrix();
			if(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
				pMatrix->_30,pMatrix->_31,pMatrix->_32) > mMaxFollowDistance*mMaxFollowDistance)
			{
				mAttackID=0;
			}
		}
	}

	//目标不存在，停止命令
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
			//到达宿主//下达跟随命令
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
				//下达攻击命令
				Command=ATTACK_COMMAND;
			}
			break;
		case ATTACK_STATE:
			{
				//到达目标点//下达撤退命令
				if(mTime<=0.0f)
				{
					Command=RECOIL_COMMAND;
				}
			}
			break;
		case RECOIL_STATE:
			{
				//到达目标点//下达准备攻击命令
				if(mTime<=0.0f)
				{
					Command=READY_ATTACK_COMMAND;
				}
			}
			break;
		case READY_ATTACK_STATE:
			{
				//到达目标点//下达攻击命令
				if(mTime<=0.0f)
				{
					Command=ATTACK_COMMAND;
				}
			}
			break;
		case BACK_HOME_STATE:
			{
				//下达攻击命令
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
	//距离
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));

	if(mSpeed!=0.0f)
	{
		//移动过去的时间
		mTime=Distance/mSpeed;
		if(mTime!=0.0f)
		{
			//计算各个方向的速度
			mEachSpeed.x =(vrTargetPosition.x-mCurrentMatrix._30)/mTime;
			mEachSpeed.y =(vrTargetPosition.y-mCurrentMatrix._31)/mTime;
			mEachSpeed.z =(vrTargetPosition.z-mCurrentMatrix._32)/mTime;
			//改变方向
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
	//当前状态
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
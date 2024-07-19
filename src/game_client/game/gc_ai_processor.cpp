
#include "gc_include.h"
#include "gc_ai_processor.h"
#include "gc_complex_missile.h"
#include "game_assistant.h"

#include <string>


const float IDLE_TIME=15.0;
const int	HURT_RATE=50;//受伤概率[0-100]

using  std::string;


//攻击部分用
/*

CAIProcessor::CAIProcessor(GcActor* vpMaster,GcBaseActor* vpActor,char vpAIType[],float vThinkCycleTime)
	:mpMaster(vpMaster),
	mpBaseActor(vpActor),
	mThinkPassTime(0.0f),				
	mThinkCycleTime(vThinkCycleTime),				
	mLastCommand(GcActor::NULL_COMMAND),
	mLastCommandPassTime(0.0f),			
	mAttackTargetID(NULL),	
	mSkillID(NULL),
	mAttackRange(0.0f),
	mAttackState(NULL_STATE),
	mpAttackFSM(NULL)		
{
	mpAttackFSM=Game::CFSM::mManager.Get(string(vpAIType));
}

CAIProcessor::~CAIProcessor()
{
}


void CAIProcessor::Run(float fSecond)
{
	if(mThinkPassTime>=mThinkCycleTime)
	{
		Think();
		mThinkPassTime=0.0;
	}
	else
	{
		mThinkPassTime+=fSecond;
	}
	mLastCommandPassTime+=fSecond;
	Doing();
	return ;
}

void CAIProcessor::OnCommand(GcActor::ECommands vCommand,GcActor* vpPet)//角色的命令发出时通知AI处理
{
	switch(vCommand)
	{
	case GcActor::RIDE_COMMAND:
		//LOG("AI得到的命令:RIDE_COMMAND\n");
		//简单直接调用基本角色
		if(!mpBaseActor->IsRide(vpPet))
		{
			mpBaseActor->Ride(vpPet);
			DealAttackState(STOP_COMMAND);//攻击AI的停止
		}
		break;
	default:
		break;
	}

	mLastCommand=vCommand;			//最后一次的命令是什么
	mLastCommandPassTime=0.0f;		//最后一次命令的经过了多长的时间//不足一个Thing周期按一个Thing周期
}

void CAIProcessor::OnCommand(GcActor::ECommands vCommand,int vParameter1,int vParameter2)		//角色的命令发出时通知AI处理
{
	switch(vCommand) 
	{
	case GcActor::NET_WALK_COMMAND:
		//LOG1("[%d]AI得到的命令:NET_WALK_COMMAND\n",mpMaster->ID());
		//简单直接调用基本角色
		if(mpBaseActor->GetState()==GcBaseActor::READY_BORN_STATE)
		{//上个状态是准备状态，所以发出生结束命令
			mpBaseActor->AddCommand(GcBaseActor::BORN_END_COMMAND);
		}

		mpBaseActor->AddCommand(GcBaseActor::WALK_COMMAND,vParameter1,vParameter2);
		break;
	case GcActor::WALK_COMMAND:
		//LOG1("[%d]AI得到的命令:WALK_COMMAND\n",mpMaster->ID());
		//简单直接调用基本角色
		mpBaseActor->AddCommand(GcBaseActor::WALK_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		break;
	case GcActor::CARRY_COMMAND:
		//简单直接调用基本角色
		//LOG1("[%d]AI得到的命令:CARRY_COMMAND\n",mpMaster->ID());
		mpBaseActor->AddCommand(GcBaseActor::CARRY_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		break;
	case GcActor::MUST_CARRY_COMMAND:
		//简单直接调用基本角色
		//LOG1("[%d]AI得到的命令:MUST_CARRY_COMMAND\n",mpMaster->ID());
		mpBaseActor->AddCommand(GcBaseActor::MUST_CARRY_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		break;
	case GcActor::ATTACK_COMMAND:
		//复杂由AI处理
		//LOG1("[%d]AI得到的命令:ATTACK_COMMAND\n",mpMaster->ID());
		if(Attack(vParameter1,vParameter2))
		{
			//法宝的攻击
			if(mpMaster->mpTrump!=NULL)
			{
				mpMaster->mpTrump->SetAttackID(vParameter1);
			}
		}
		break;
	case GcActor::CHANGE_SKILL_COMMAND:
		if(mAttackState!=NULL_STATE)
		{//正在攻击中
			if(!Attack(mAttackTargetID,vParameter1))
			{
				mpMaster->Command(GcActor::STOP_COMMAND);
			}
		}
		break;
	case GcActor::NET_ATTACK_COMMAND:
		{
			GcActor* pTarget=FindActor(vParameter1);
			if(pTarget!=NULL)
			{
				//LOG1("[%d]AI得到的命令:NET_ATTACK_COMMAND\n",mpMaster->ID());
				mpMaster->mBaseActor.FaceTo(pTarget->Matrix()->_30,pTarget->Matrix()->_31);//面朝向
				mpMaster->mBaseActor.AddCommand(GcBaseActor::ATTACK_COMMAND,vParameter1,vParameter2);//调用基本角色的攻击
				if(mpMaster->mpTrump!=NULL)
				{
					mpMaster->mpTrump->SetAttackID(vParameter1);//法宝
				}
			}
			else
			{
				//LOG2("[%d]AI得到的命令:NET_ATTACK_COMMAND:未找到[%d]\n",mpMaster->ID(),mAttackTargetID);
			}
		}
		break;
	case GcActor::HURT_COMMAND:
		{
			if(rand()%100>=HURT_RATE)
			{
				return;
			}
			mpBaseActor->AddCommand(GcBaseActor::HURT_COMMAND,vParameter1,vParameter2);
			//DealAttackState(STOP_COMMAND)//并不终止AI的攻击
		}
		break;
	case GcActor::STOP_COMMAND:
		//LOG1("[%d]AI得到的命令:STOP_COMMAND\n",mpMaster->ID());
		//简单直接调用基本角色
		mpBaseActor->AddCommand(GcBaseActor::STOP_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		break;
	case GcActor::STOP_EXCEPTION_COMMAND:
		//LOG1("[%d]AI得到的命令:STOP_EXCEPTION_COMMAND\n",mpMaster->ID());
		//简单直接调用基本角色
		mpBaseActor->AddCommand(GcBaseActor::STOP_COMMAND,vParameter1,vParameter2);
		break;
	case GcActor::UNRIDE_COMMAND:
		//简单直接调用基本角色
		//LOG1("[%d]AI得到的命令:UNRIDE_COMMAND\n",mpMaster->ID());
		mpBaseActor->UnRide();
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		break;
	case GcActor::EQUIP_COMMAND:
		//简单直接调用道具部分
		if(mpBaseActor->GetRideState()!=GcBaseActor::RIDE_STATE)
		{
//				bool res = mpMaster->mItem.Equip( SItemID(vParameter2,vParameter1) );
////				mpMaster->mItem.Equip( SItemID(ITEM_MASK_WEAPON_1,100) );	// vParameter1 is id, vParameter2 is level
//				SItemID item(vParameter2, vParameter1, 0x00, 0x01);
//				if ( res )
//				{
//					CG_CmdPacket* pPacket = NetBeginWrite();
//					pPacket->WriteShort(c2r_player_equipitem);
//					pPacket->WriteLong(vParameter1);
//					pPacket->WriteLong(vParameter2);
//					pPacket->WriteByte(0x00);
//					pPacket->WriteByte(0x01);
//					pPacket->WriteByte(CItemContainerBase::WEAPON_1);
//					NetSend(pPacket);
//				}
		}
		//LOG("AI得到的命令:EQUIP_COMMAND\n");
		break;
	case GcActor::UNEQUIP_COMMAND:
		//简单直接调用道具部分
		if(mpBaseActor->GetRideState()!=GcBaseActor::RIDE_STATE)
		{
//				mpMaster->mItem.UnEquip( (CItemContainer::EEquipParts) vParameter1);
		}
		break;
	case GcActor::READY_BORN_COMMAND:			//准备出生
		mpBaseActor->AddCommand(GcBaseActor::READY_BORN_COMMAND);
		break;
	case GcActor::BORN_COMMAND:				//出生命令
		mpBaseActor->AddCommand(GcBaseActor::BORN_COMMAND);
		break;
	case GcActor::DIE_COMMAND:
		DealAttackState(STOP_COMMAND);//攻击AI的停止
		mpBaseActor->AddCommand(GcBaseActor::DIE_COMMAND);
		break;
	default:
		break;
	}

	mLastCommand=vCommand;			//最后一次的命令是什么
	mLastCommandPassTime=0.0f;		//最后一次命令的经过了多长的时间//不足一个Think周期按一个Think周期
	return ;
}

void CAIProcessor::Think()//AI的思考
{

	//发呆动作的处理
	switch(mLastCommand)
	{
	case GcActor::UNRIDE_COMMAND:				//下兽命令
	case GcActor::CARRY_COMMAND:
	case GcActor::MUST_CARRY_COMMAND:
	case GcActor::STOP_COMMAND:
		if(mLastCommandPassTime>=IDLE_TIME)
		{//如果最后一个命令是停止命令并且IDLE时间超过了IDLE时间那么向人物发IDLE命令
			mpBaseActor->AddCommand(GcBaseActor::IDLE_COMMAND);
			mLastCommand=GcActor::NULL_COMMAND;
		}
		break;
	default:
		break;
	}


	//攻击的处理
	if(mAttackState!=NULL_STATE)
	{
		bool StopAttack=false;

		GcBaseActor* pTarget=Find(mAttackTargetID);
		if(pTarget==NULL)
		{
			StopAttack=true;
		}
		//得到攻击的范围
		float AttackRange = mAttackRange;
		if(mSkillID==0)
		{//正常攻击
		}
		else
		{//技能攻击
			SSkill *pSkill = Skill()->FindSkill(mSkillID);
			if(pSkill==NULL)
			{
				StopAttack=true;
			}
			else
			{
				AttackRange = pSkill->wARange;	
			}
		}
		
		if(StopAttack)
		{
			//目标不存在或者技能找不到
			DealAttackState(STOP_COMMAND);
			//mpBaseActor->Command(GcBaseActor::STOP_COMMAND);
			LOG("目标寻找不到，攻击被中断\n");
		}
		else
		{
					
			if(DistanceToBoundingBox(pTarget) <= (AttackRange*AttackRange))
			{//如果目标在攻击范围内//准备攻击命令
				mpBaseActor->FaceTo(pTarget->Matrix()->_30,pTarget->Matrix()->_31);//面朝向
				DealAttackState(READY_ATTACK_COMMAND);
				//LOG("AI中的攻击命令");
			}
			else
			{//如果目标在攻击范围外//搜索命令
				DealAttackState(SEARCH_COMMAND);
			}
		}
	}
}


void CAIProcessor::Doing()
{

	//攻击部分的处理
	switch(mAttackState)
	{
	case SEARCH_STATE:

		MoveToTarget();
		DealAttackState(MOVE_COMMAND);//移动命令
		break;
	case READY_ATTACK_STATE:
		mpBaseActor->AddCommand(GcBaseActor::ATTACK_COMMAND,mAttackTargetID,mSkillID);
		DealAttackState(ATTACK_COMMAND);//攻击命令
		break;
	case ATTACK_STATE:
		mpBaseActor->AddCommand(GcBaseActor::ATTACK_COMMAND,mAttackTargetID,mSkillID);
		break;
	default:
		break;
	}
}



void CAIProcessor::MoveToTarget()
{
	GcBaseActor* pTarget=Find(mAttackTargetID);
	RtgMatrix16* pMatrix; 
	pMatrix=pTarget->Matrix();
	mpBaseActor->AddCommand(GcBaseActor::WALK_NEAR_TARGET_COMMAND,pMatrix->_30,pMatrix->_31,pMatrix->_32);
	return;
}

bool CAIProcessor::DealAttackState(EAttackCommands vCommand)					//处理AI的状态
{
	EAttackStates LastState=mAttackState;
	mAttackState=(EAttackStates)mpAttackFSM->Transition(vCommand,mAttackState);

	if(LastState==mAttackState)
	{//状态未发生变化
		return false;
	}
	else
	{//状态发生变化
		return true;
	}
}


bool CAIProcessor::Attack(DWORD vTargetID,int vSkillID)
{
	SSkill* pSkill= Skill()->FindSkill(vSkillID);

	if(pSkill!=NULL)
	{
		if(pSkill->SkillType==SKILL_TYPE_DAMAGE)
		{//有害
			if(vTargetID==GetWorld()->m_pPet->ID())
			{//是自己的兽
				return false;
			}
		}
		else if(pSkill->SkillType==SKILL_TYPE_RECOVER)
		{//有益
			if(vTargetID!=GetWorld()->m_pPet->ID())
			{//不是自己的兽
				return false;
			}
		}
		else
		{//不明
			return false;
		}
        return false;
	}

	mAttackTargetID=vTargetID;
	mSkillID=vSkillID;
	mAttackState=MOVE_STATE;
	return true;
}


GcBaseActor* CAIProcessor::Find(DWORD vID)
{
	static GcSynchroActorManager* pActorManager = &(GetWorld()->m_ActorManager);
	GcActor* p = pActorManager->Find(vID);
	if(p==NULL)
	{
		if(GetWorld()->m_pPlayer->ID()==vID)
		{
			return &(GetWorld()->m_pPlayer->mBaseActor);
		}
		else if(GetWorld()->m_pPet->ID()==vID)
		{
			return &(GetWorld()->m_pPet->mBaseActor);
		}

		return NULL;
	}
	else
	{
		return &(p->mBaseActor);
	}
}

float CAIProcessor::DistanceSquare(GcBaseActor* vpTargetActor)				//同目标的距离的平方
{
	RtgMatrix16* pTarget=vpTargetActor->Matrix();
	RtgMatrix16* pBaseActor=mpBaseActor->Matrix();

	//距离的平方
	return ((pTarget->_30-pBaseActor->_30)*(pTarget->_30-pBaseActor->_30)
		+(pTarget->_31-pBaseActor->_31)*(pTarget->_31-pBaseActor->_31)
		+(pTarget->_32-pBaseActor->_32)*(pTarget->_32-pBaseActor->_32));

}

float CAIProcessor::DistanceToBoundingBox(GcBaseActor* vpTargetActor)
{
	float dis = DistanceSquare(vpTargetActor);
	float boundx = vpTargetActor->GetGraph()->Graph()->GetBoundingBox()->vExt.x;
	float boundy = vpTargetActor->GetGraph()->Graph()->GetBoundingBox()->vExt.y;
	float bound = boundx*boundx+boundy*boundy;
	dis = sqrt(dis) - sqrt(bound);
	if(dis < 0) return 0;
	return dis*dis;
}

void CAIProcessor::OnNetUploadAttack()
{
	GetWorld()->OnNetUploadAttack(mAttackTargetID,mSkillID);
}

*/
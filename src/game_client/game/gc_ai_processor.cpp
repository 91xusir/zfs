
#include "gc_include.h"
#include "gc_ai_processor.h"
#include "gc_complex_missile.h"
#include "game_assistant.h"

#include <string>


const float IDLE_TIME=15.0;
const int	HURT_RATE=50;//���˸���[0-100]

using  std::string;


//����������
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

void CAIProcessor::OnCommand(GcActor::ECommands vCommand,GcActor* vpPet)//��ɫ�������ʱ֪ͨAI����
{
	switch(vCommand)
	{
	case GcActor::RIDE_COMMAND:
		//LOG("AI�õ�������:RIDE_COMMAND\n");
		//��ֱ�ӵ��û�����ɫ
		if(!mpBaseActor->IsRide(vpPet))
		{
			mpBaseActor->Ride(vpPet);
			DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		}
		break;
	default:
		break;
	}

	mLastCommand=vCommand;			//���һ�ε�������ʲô
	mLastCommandPassTime=0.0f;		//���һ������ľ����˶೤��ʱ��//����һ��Thing���ڰ�һ��Thing����
}

void CAIProcessor::OnCommand(GcActor::ECommands vCommand,int vParameter1,int vParameter2)		//��ɫ�������ʱ֪ͨAI����
{
	switch(vCommand) 
	{
	case GcActor::NET_WALK_COMMAND:
		//LOG1("[%d]AI�õ�������:NET_WALK_COMMAND\n",mpMaster->ID());
		//��ֱ�ӵ��û�����ɫ
		if(mpBaseActor->GetState()==GcBaseActor::READY_BORN_STATE)
		{//�ϸ�״̬��׼��״̬�����Է�������������
			mpBaseActor->AddCommand(GcBaseActor::BORN_END_COMMAND);
		}

		mpBaseActor->AddCommand(GcBaseActor::WALK_COMMAND,vParameter1,vParameter2);
		break;
	case GcActor::WALK_COMMAND:
		//LOG1("[%d]AI�õ�������:WALK_COMMAND\n",mpMaster->ID());
		//��ֱ�ӵ��û�����ɫ
		mpBaseActor->AddCommand(GcBaseActor::WALK_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		break;
	case GcActor::CARRY_COMMAND:
		//��ֱ�ӵ��û�����ɫ
		//LOG1("[%d]AI�õ�������:CARRY_COMMAND\n",mpMaster->ID());
		mpBaseActor->AddCommand(GcBaseActor::CARRY_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		break;
	case GcActor::MUST_CARRY_COMMAND:
		//��ֱ�ӵ��û�����ɫ
		//LOG1("[%d]AI�õ�������:MUST_CARRY_COMMAND\n",mpMaster->ID());
		mpBaseActor->AddCommand(GcBaseActor::MUST_CARRY_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		break;
	case GcActor::ATTACK_COMMAND:
		//������AI����
		//LOG1("[%d]AI�õ�������:ATTACK_COMMAND\n",mpMaster->ID());
		if(Attack(vParameter1,vParameter2))
		{
			//�����Ĺ���
			if(mpMaster->mpTrump!=NULL)
			{
				mpMaster->mpTrump->SetAttackID(vParameter1);
			}
		}
		break;
	case GcActor::CHANGE_SKILL_COMMAND:
		if(mAttackState!=NULL_STATE)
		{//���ڹ�����
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
				//LOG1("[%d]AI�õ�������:NET_ATTACK_COMMAND\n",mpMaster->ID());
				mpMaster->mBaseActor.FaceTo(pTarget->Matrix()->_30,pTarget->Matrix()->_31);//�泯��
				mpMaster->mBaseActor.AddCommand(GcBaseActor::ATTACK_COMMAND,vParameter1,vParameter2);//���û�����ɫ�Ĺ���
				if(mpMaster->mpTrump!=NULL)
				{
					mpMaster->mpTrump->SetAttackID(vParameter1);//����
				}
			}
			else
			{
				//LOG2("[%d]AI�õ�������:NET_ATTACK_COMMAND:δ�ҵ�[%d]\n",mpMaster->ID(),mAttackTargetID);
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
			//DealAttackState(STOP_COMMAND)//������ֹAI�Ĺ���
		}
		break;
	case GcActor::STOP_COMMAND:
		//LOG1("[%d]AI�õ�������:STOP_COMMAND\n",mpMaster->ID());
		//��ֱ�ӵ��û�����ɫ
		mpBaseActor->AddCommand(GcBaseActor::STOP_COMMAND,vParameter1,vParameter2);
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		break;
	case GcActor::STOP_EXCEPTION_COMMAND:
		//LOG1("[%d]AI�õ�������:STOP_EXCEPTION_COMMAND\n",mpMaster->ID());
		//��ֱ�ӵ��û�����ɫ
		mpBaseActor->AddCommand(GcBaseActor::STOP_COMMAND,vParameter1,vParameter2);
		break;
	case GcActor::UNRIDE_COMMAND:
		//��ֱ�ӵ��û�����ɫ
		//LOG1("[%d]AI�õ�������:UNRIDE_COMMAND\n",mpMaster->ID());
		mpBaseActor->UnRide();
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		break;
	case GcActor::EQUIP_COMMAND:
		//��ֱ�ӵ��õ��߲���
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
		//LOG("AI�õ�������:EQUIP_COMMAND\n");
		break;
	case GcActor::UNEQUIP_COMMAND:
		//��ֱ�ӵ��õ��߲���
		if(mpBaseActor->GetRideState()!=GcBaseActor::RIDE_STATE)
		{
//				mpMaster->mItem.UnEquip( (CItemContainer::EEquipParts) vParameter1);
		}
		break;
	case GcActor::READY_BORN_COMMAND:			//׼������
		mpBaseActor->AddCommand(GcBaseActor::READY_BORN_COMMAND);
		break;
	case GcActor::BORN_COMMAND:				//��������
		mpBaseActor->AddCommand(GcBaseActor::BORN_COMMAND);
		break;
	case GcActor::DIE_COMMAND:
		DealAttackState(STOP_COMMAND);//����AI��ֹͣ
		mpBaseActor->AddCommand(GcBaseActor::DIE_COMMAND);
		break;
	default:
		break;
	}

	mLastCommand=vCommand;			//���һ�ε�������ʲô
	mLastCommandPassTime=0.0f;		//���һ������ľ����˶೤��ʱ��//����һ��Think���ڰ�һ��Think����
	return ;
}

void CAIProcessor::Think()//AI��˼��
{

	//���������Ĵ���
	switch(mLastCommand)
	{
	case GcActor::UNRIDE_COMMAND:				//��������
	case GcActor::CARRY_COMMAND:
	case GcActor::MUST_CARRY_COMMAND:
	case GcActor::STOP_COMMAND:
		if(mLastCommandPassTime>=IDLE_TIME)
		{//������һ��������ֹͣ�����IDLEʱ�䳬����IDLEʱ����ô�����﷢IDLE����
			mpBaseActor->AddCommand(GcBaseActor::IDLE_COMMAND);
			mLastCommand=GcActor::NULL_COMMAND;
		}
		break;
	default:
		break;
	}


	//�����Ĵ���
	if(mAttackState!=NULL_STATE)
	{
		bool StopAttack=false;

		GcBaseActor* pTarget=Find(mAttackTargetID);
		if(pTarget==NULL)
		{
			StopAttack=true;
		}
		//�õ������ķ�Χ
		float AttackRange = mAttackRange;
		if(mSkillID==0)
		{//��������
		}
		else
		{//���ܹ���
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
			//Ŀ�겻���ڻ��߼����Ҳ���
			DealAttackState(STOP_COMMAND);
			//mpBaseActor->Command(GcBaseActor::STOP_COMMAND);
			LOG("Ŀ��Ѱ�Ҳ������������ж�\n");
		}
		else
		{
					
			if(DistanceToBoundingBox(pTarget) <= (AttackRange*AttackRange))
			{//���Ŀ���ڹ�����Χ��//׼����������
				mpBaseActor->FaceTo(pTarget->Matrix()->_30,pTarget->Matrix()->_31);//�泯��
				DealAttackState(READY_ATTACK_COMMAND);
				//LOG("AI�еĹ�������");
			}
			else
			{//���Ŀ���ڹ�����Χ��//��������
				DealAttackState(SEARCH_COMMAND);
			}
		}
	}
}


void CAIProcessor::Doing()
{

	//�������ֵĴ���
	switch(mAttackState)
	{
	case SEARCH_STATE:

		MoveToTarget();
		DealAttackState(MOVE_COMMAND);//�ƶ�����
		break;
	case READY_ATTACK_STATE:
		mpBaseActor->AddCommand(GcBaseActor::ATTACK_COMMAND,mAttackTargetID,mSkillID);
		DealAttackState(ATTACK_COMMAND);//��������
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

bool CAIProcessor::DealAttackState(EAttackCommands vCommand)					//����AI��״̬
{
	EAttackStates LastState=mAttackState;
	mAttackState=(EAttackStates)mpAttackFSM->Transition(vCommand,mAttackState);

	if(LastState==mAttackState)
	{//״̬δ�����仯
		return false;
	}
	else
	{//״̬�����仯
		return true;
	}
}


bool CAIProcessor::Attack(DWORD vTargetID,int vSkillID)
{
	SSkill* pSkill= Skill()->FindSkill(vSkillID);

	if(pSkill!=NULL)
	{
		if(pSkill->SkillType==SKILL_TYPE_DAMAGE)
		{//�к�
			if(vTargetID==GetWorld()->m_pPet->ID())
			{//���Լ�����
				return false;
			}
		}
		else if(pSkill->SkillType==SKILL_TYPE_RECOVER)
		{//����
			if(vTargetID!=GetWorld()->m_pPet->ID())
			{//�����Լ�����
				return false;
			}
		}
		else
		{//����
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

float CAIProcessor::DistanceSquare(GcBaseActor* vpTargetActor)				//ͬĿ��ľ����ƽ��
{
	RtgMatrix16* pTarget=vpTargetActor->Matrix();
	RtgMatrix16* pBaseActor=mpBaseActor->Matrix();

	//�����ƽ��
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
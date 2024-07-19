//===========================================================
//		�ļ����ƣ� gc_ai_processor.h
//		��    �ߣ� ����
//		ʱ    �䣺 2004.6.30
//		��    �ݣ� ��Ϸ�е�AI������
//===========================================================
#ifndef GC_AI_PROCESSOR_H
#define GC_AI_PROCESSOR_H
#include "game_states_machine.h"
class GcBaseActor;

const float DEFAULT_THINK_CYCLE_TIME=0.0f;



class CAIProcessor
{
public:

	enum EAttackCommands
	{
		STOP_COMMAND,			//ֹͣ����
		READY_ATTACK_COMMAND,	//׼����������
		ATTACK_COMMAND,			//��������
		SEARCH_COMMAND,			//��������
		MOVE_COMMAND,			//�ƶ�����
	};
	enum EAttackStates
	{
		//����״̬
		NULL_STATE,				//��״̬
		MOVE_STATE,				//�ƶ�״̬
		SEARCH_STATE,			//����Ŀ��
		READY_ATTACK_STATE,		//׼������
		ATTACK_STATE,			//����Ŀ��
	};

public:

	//AI��������AI���ƵĻ�����ɫ��AI�����ͺ͸�AI��˼��ʱ��ļ��
	CAIProcessor(GcActor* vpMaster,GcBaseActor* vpActor,
					char vpAITpye[],float vThinkCycleTime=DEFAULT_THINK_CYCLE_TIME);
	
	virtual ~CAIProcessor();


	void Run(float fSecond);															//����
	
	void OnCommand(GcActor::ECommands vCommand,int vParameter1,int vParameter2);		//��ɫ�������ʱ֪ͨAI����
	void OnCommand(GcActor::ECommands vCommand,GcActor* vpPet);							//��ɫ�������ʱ֪ͨAI����


	void OnNetUploadAttack();
	void SetAttackRange(float vAttackRange){mAttackRange=vAttackRange;};
	EAttackStates GetAttackState(){return mAttackState;};
private:
	//�������������Լ�ʹ��
	
	void Think();										//���ݵ�ǰ�����������
	void Doing();										//����AI�ĵ�ǰ״̬������

	float DistanceSquare(GcBaseActor* vpTargetActor);	//ͬĳ�˵ľ���
	float DistanceToBoundingBox(GcBaseActor* vpTargetActor); //ͬĳ�˵ľ���

	void MoveToTarget();								//��Ŀ���ƶ�
	bool Attack(DWORD vTargetID,int AttackType);		//����ĳ��
	GcBaseActor* Find(DWORD vID);						//�ҵ�ĳID��ָ��
	bool DealAttackState(EAttackCommands vCommand);		//����AI��״̬


	//��Ա����
	GcActor*		mpMaster;							//��AI������
	GcBaseActor*	mpBaseActor;						//��AI���ƵĶ���	

	float			mThinkPassTime;						//�Ѿ�������ʱ��
	float			mThinkCycleTime;					//��AI��˼��ʱ����
	GcActor::ECommands		mLastCommand;				//���һ�ε�����
	float			mLastCommandPassTime;				//���һ�������ȥ��ʱ��

	//����������
	DWORD			mAttackTargetID;					//Ŀ��ID
	int				mSkillID;							//���ܵ�����
	float			mAttackRange;						//��������

	EAttackStates	mAttackState;						//��ǰ״̬
	Game::CFSM*		mpAttackFSM;						//״̬��


};



#endif//GC_AI_PROCESSOR_H




#ifndef GC_COMPLEX_MISSILE_H
#define GC_COMPLEX_MISSILE_H
#include "gc_missile.h"
#include "game_states_machine.h"
class GcBaseActor;
class GcActor;

class CComplexMissile:public CMissile
{
	//����ƽʱ��������ߣ���������
	//����������ĳ��ʱ������ֱ�߷���Ŀ����Χ�ɹ�����Χ�ڵ�ĳ���㣬
	//Ȼ���룬��Ŀ����Χ׼��������С��Χ�ڵ�ĳ���㣬
	//Ȼ��׼����������Ŀ����Χ׼��������С�����Χ֮���ĳ���㣬
	//Ȼ�󹥻�����Ŀ����Χ�ɹ�����Χ�ڵ�ĳ����
	//Ŀ��������Ŀ�곬��������룬��ص��������

public:

	enum ECommands
	{
		NULL_COMMAND,
		FOLLOW_COMMAND,
		ATTACK_COMMAND,				//��������
		RECOIL_COMMAND,				//��������
		READY_ATTACK_COMMAND,		//׼����������
		STOP_COMMAND,				//ֹͣ����
	};

	enum EStates
	{
		NULL_STATE,					//��״̬
		FOLLOW_STATE,				//����״̬
		ATTACK_STATE,				//����״̬
		RECOIL_STATE,				//����״̬
		READY_ATTACK_STATE,			//׼������״̬
		BACK_HOME_STATE,
	};




	CComplexMissile();
	virtual ~CComplexMissile(){};

	void SetMaster(GcBaseActor*	vpMaster);
	void Delete(){DeleteSelf();}//����
	void SetAttackID(DWORD vAttackID);

#if DO_CONSOLE
	void DebugInfo(int vX,int vY);
#endif//DO_CONSOLE




protected:
private:
	virtual void OnRun(float vSecond);
	virtual void OnArrive(){};

	void SetTargetToMaster();
	void SetTargetPosition(RtgVertex3& vrTargetPosition);	
	void DealState(ECommands vCommand);
	ECommands DealCommand();
	void DealMove(float vSecond);
	void Move(float vSecond);
	RtgVertex3 GetActorPosition();
	void SelfRoation(float vSecond);




	GcBaseActor*	mpMaster;

	float		mTime;							//��ʣ����ʱ�䵽Ŀ�ĵ�
	float		mReviseTime;					
	float		mMaxFollowDistance;				//���ĸ������
	float		mSpeed;							//�ƶ��ٶ�
	RtgVertex3	mTargetPosition;				//Ŀ���λ��
	RtgVertex3  mEachSpeed;						//���������ϵķ��ٶ�
	float		mAttackDistance;				//��������
	float		mMaxReadyAttackDistance;		//���׼����������
	float		mMinReadyAttackDistance;		//��С׼����������
	DWORD		mAttackID;						//��ǰ����Ŀ��
	GcActor*	mpAttackActor;					//��ǰ����Ŀ���ָ��

	
	Game::CFSM*			mpFSM;					//״̬��//������ǰ��״̬
	EStates				mState;					//��ǰ״̬
};



//�����������˶���λ���˶���Ŀ���λ���˶���������

//�������У����������ˣ���������ŭ

//Ŀ����˶��У������Լ�������
//׷�ٵ���

//λ���˶��У�ֱ���˶���
//�����˶�����ת���񶯣�
//���ȱ���ƽʱ�������Χ����
				
				
				//����������¿����ӱ������������˶�

//Ȼ�󿴼����˾͹���
//������ϻ���




#endif//GC_COMPLEX_MISSILE_H

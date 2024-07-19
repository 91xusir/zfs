//===========================================================
//		文件名称： gc_ai_processor.h
//		作    者： 吴麟
//		时    间： 2004.6.30
//		内    容： 游戏中的AI处理器
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
		STOP_COMMAND,			//停止命令
		READY_ATTACK_COMMAND,	//准备攻击命令
		ATTACK_COMMAND,			//攻击命令
		SEARCH_COMMAND,			//搜索命令
		MOVE_COMMAND,			//移动命令
	};
	enum EAttackStates
	{
		//攻击状态
		NULL_STATE,				//空状态
		MOVE_STATE,				//移动状态
		SEARCH_STATE,			//搜索目标
		READY_ATTACK_STATE,		//准备攻击
		ATTACK_STATE,			//攻击目标
	};

public:

	//AI的宿主和AI控制的基本角色和AI的类型和该AI的思考时间的间隔
	CAIProcessor(GcActor* vpMaster,GcBaseActor* vpActor,
					char vpAITpye[],float vThinkCycleTime=DEFAULT_THINK_CYCLE_TIME);
	
	virtual ~CAIProcessor();


	void Run(float fSecond);															//运行
	
	void OnCommand(GcActor::ECommands vCommand,int vParameter1,int vParameter2);		//角色的命令发出时通知AI处理
	void OnCommand(GcActor::ECommands vCommand,GcActor* vpPet);							//角色的命令发出时通知AI处理


	void OnNetUploadAttack();
	void SetAttackRange(float vAttackRange){mAttackRange=vAttackRange;};
	EAttackStates GetAttackState(){return mAttackState;};
private:
	//辅助函数仅供自己使用
	
	void Think();										//根据当前情况发出命令
	void Doing();										//根据AI的当前状态做处理

	float DistanceSquare(GcBaseActor* vpTargetActor);	//同某人的距离
	float DistanceToBoundingBox(GcBaseActor* vpTargetActor); //同某人的距离

	void MoveToTarget();								//向目标移动
	bool Attack(DWORD vTargetID,int AttackType);		//攻击某人
	GcBaseActor* Find(DWORD vID);						//找到某ID的指针
	bool DealAttackState(EAttackCommands vCommand);		//处理AI的状态


	//成员变量
	GcActor*		mpMaster;							//该AI的宿主
	GcBaseActor*	mpBaseActor;						//该AI控制的东西	

	float			mThinkPassTime;						//已经经过的时间
	float			mThinkCycleTime;					//该AI的思考时间间隔
	GcActor::ECommands		mLastCommand;				//最后一次的命令
	float			mLastCommandPassTime;				//最后一次命令过去的时间

	//攻击部分用
	DWORD			mAttackTargetID;					//目标ID
	int				mSkillID;							//技能的类型
	float			mAttackRange;						//攻击距离

	EAttackStates	mAttackState;						//当前状态
	Game::CFSM*		mpAttackFSM;						//状态机


};



#endif//GC_AI_PROCESSOR_H


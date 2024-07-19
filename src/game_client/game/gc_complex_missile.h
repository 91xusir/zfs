

#ifndef GC_COMPLEX_MISSILE_H
#define GC_COMPLEX_MISSILE_H
#include "gc_missile.h"
#include "game_states_machine.h"
class GcBaseActor;
class GcActor;

class CComplexMissile:public CMissile
{
	//宝物平时在宿主身边，跟随宿主
	//当宿主进攻某人时，宝物直线飞向目标周围可攻击范围内的某个点，
	//然后撤离，到目标周围准备攻击最小范围内的某个点，
	//然后准备进攻，到目标周围准备攻击最小和最大范围之间的某个点，
	//然后攻击，到目标周围可攻击范围内的某个点
	//目标死亡或，目标超过跟随距离，则回到宿主身边

public:

	enum ECommands
	{
		NULL_COMMAND,
		FOLLOW_COMMAND,
		ATTACK_COMMAND,				//攻击命令
		RECOIL_COMMAND,				//撤离命令
		READY_ATTACK_COMMAND,		//准备攻击命令
		STOP_COMMAND,				//停止命令
	};

	enum EStates
	{
		NULL_STATE,					//空状态
		FOLLOW_STATE,				//跟随状态
		ATTACK_STATE,				//攻击状态
		RECOIL_STATE,				//撤离状态
		READY_ATTACK_STATE,			//准备攻击状态
		BACK_HOME_STATE,
	};




	CComplexMissile();
	virtual ~CComplexMissile(){};

	void SetMaster(GcBaseActor*	vpMaster);
	void Delete(){DeleteSelf();}//丢弃
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

	float		mTime;							//还剩多少时间到目的点
	float		mReviseTime;					
	float		mMaxFollowDistance;				//最大的跟随距离
	float		mSpeed;							//移动速度
	RtgVertex3	mTargetPosition;				//目标的位置
	RtgVertex3  mEachSpeed;						//各个轴向上的分速度
	float		mAttackDistance;				//攻击距离
	float		mMaxReadyAttackDistance;		//最大准备攻击距离
	float		mMinReadyAttackDistance;		//最小准备攻击距离
	DWORD		mAttackID;						//当前攻击目标
	GcActor*	mpAttackActor;					//当前攻击目标的指针

	
	Game::CFSM*			mpFSM;					//状态机//法宝当前的状态
	EStates				mState;					//当前状态
};



//宝物有自身运动和位置运动和目标点位置运动和自身动作

//自身动作有，正常，高兴，警觉，愤怒

//目标点运动有，跟随自己的主人
//追踪敌人

//位置运动有，直线运动，
//自身运动有旋转，振动，
//首先宝物平时在玩家周围不动
				
				
				//不动的情况下可能视宝物的心情决定运动

//然后看见敌人就攻击
//攻击完毕回来




#endif//GC_COMPLEX_MISSILE_H

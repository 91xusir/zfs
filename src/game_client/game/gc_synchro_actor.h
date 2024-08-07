

/*----------------------------------------------------------------------------
-   同步的角色，所有和服务器交互的角色都是使用这个类的
-       这里不包含玩家自己
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
-   GcSynchroActorManager 角色管理
----------------------------------------------------------------------------*/
#ifndef GC_SYNCHRO_ACTOR_H
#define GC_SYNCHRO_ACTOR_H

#include "gc_actor.h"
#include "gc_enums.h"
#include "game_listener.h"


using fox::CListenerManager;
class RtScene;
class GcSynchroActor;

class GcSynchroActorManager
{
public:
    GcSynchroActorManager();
    virtual ~GcSynchroActorManager();

    bool Init();
    bool Clear();

    void OnBeginDownloadSnapshot(); // 开始网络刷新位置等信息
    void OnEndDownloadSnapshot();   // 结束网络刷新位置等信息，如果有的物件没有被刷新就会被Disable
    void OnDownloadActorSnapshot(GcSynchroActor* pActor, bool bBeforBorn);

    void OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
    void Run(float fSecond);
	void UpdateHUD();

	GcSynchroActor* FindAll(DWORD vID);
    GcSynchroActor* FindActive(DWORD vID, bool bDead=false);

	GcSynchroActor* NewActor(DWORD vID, short vModelID, short vHeadModelID, bool bBeforBorn, short wNpcID);
	void DeleteActor(GcSynchroActor* pActor);
    void ActorDie(DWORD vID,char dead = 0);
	void ActorDie(GcSynchroActor* pActor);
	void ActorRebirth(DWORD vID);
    void ActorBorn(DWORD vID);

    GcActor* HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, float fOutMinDist, bool bHitTestTouch);

    void SetMaxDisplayChar(short sMax);
    short GetMaxDisplayChar()           { return m_sMaxDisplayChar; }
    void OnHUDConfigChanged();

    void OnMapChanged();
	
	void HideHudHp(DWORD vID);
public:
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>   m_ActorMap;     // 所有角色索引

protected:
    float                   m_fDownloadTime;    // 下载数据的标志
    short                   m_sMaxDisplayChar;

};

/*----------------------------------------------------------------------------
-   GcSynchroActor 角色
----------------------------------------------------------------------------*/

class GcSynchroActor : public GcActor
{
public:
    friend class GcSynchroActorManager;
    enum ELifeState
    {
        EPrenatal   , // 出生前
        EBorn       , // 出生中
        ELife       , // 活着
        EDeathlike  , // 已经死了, 还没有播动画
        EDie        , // 正在拨放动画
        EPosthumous , // 死后, 可以删除
    };
	bool IsActive()             { return m_eActiveState==AAS_ACTIVE;}
	void SetDownloadWaitTime(float fTime) { m_fDownloadWaitTime = fTime; }

protected:
    GcSynchroActor();
    virtual ~GcSynchroActor();

    bool BindID(DWORD vID, short vModelID, short vHeadModelID, bool bBeforBorn);
    bool Die(float fSecond);
	virtual void OnDie();
    virtual void OnBorn();

protected:
	float               m_fDownloadTime;		// 下载数据的标志
	float               m_fDownloadWaitTime;	// 等待下载数据时间
    EActorActiveState   m_eActiveState;
    float               m_fCurDieTime;
	ELifeState          m_eLifeState;

};

#endif//GC_SYNCHRO_ACTOR_H

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

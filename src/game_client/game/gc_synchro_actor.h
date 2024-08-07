

/*----------------------------------------------------------------------------
-   ͬ���Ľ�ɫ�����кͷ����������Ľ�ɫ����ʹ��������
-       ���ﲻ��������Լ�
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
-   GcSynchroActorManager ��ɫ����
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

    void OnBeginDownloadSnapshot(); // ��ʼ����ˢ��λ�õ���Ϣ
    void OnEndDownloadSnapshot();   // ��������ˢ��λ�õ���Ϣ������е����û�б�ˢ�¾ͻᱻDisable
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
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>   m_ActorMap;     // ���н�ɫ����

protected:
    float                   m_fDownloadTime;    // �������ݵı�־
    short                   m_sMaxDisplayChar;

};

/*----------------------------------------------------------------------------
-   GcSynchroActor ��ɫ
----------------------------------------------------------------------------*/

class GcSynchroActor : public GcActor
{
public:
    friend class GcSynchroActorManager;
    enum ELifeState
    {
        EPrenatal   , // ����ǰ
        EBorn       , // ������
        ELife       , // ����
        EDeathlike  , // �Ѿ�����, ��û�в�����
        EDie        , // ���ڲ��Ŷ���
        EPosthumous , // ����, ����ɾ��
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
	float               m_fDownloadTime;		// �������ݵı�־
	float               m_fDownloadWaitTime;	// �ȴ���������ʱ��
    EActorActiveState   m_eActiveState;
    float               m_fCurDieTime;
	ELifeState          m_eLifeState;

};

#endif//GC_SYNCHRO_ACTOR_H

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

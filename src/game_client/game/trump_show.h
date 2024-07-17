#ifndef __MAGIC_WEAPON_H__
#define __MAGIC_WEAPON_H__

#include "core/rt2_core.h"
#include "graph/rt_graph.h"
using namespace std;
#include "character/actor.h"
class GcBaseActor;
struct SSkill;
class CTrumpShow:public CRT_PoseNotify
{
public:
	CTrumpShow();
	virtual ~CTrumpShow();

	enum TrumpState {
		STATE_NULL,
		STATE_IDLE,
		STATE_ATTACK,
		STATE_SKILL,
		STATE_REFINE,
		STATE_DIE
	};

	void Tick(float delta);
	void Render(RtgDevice *dev,RTGRenderMask vMask);
    void RenderHighLight(RtgDevice *dev, RtgVertex3 &color,RtgVertex3 &scale);

	// new interface
	void Init(GcBaseActor *Host,const char *Model, SItemID &item, STrump *pTrump = NULL); 
	void DoAttack(DWORD TargetId, bool vLoop, float fSpeed);
	void DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill *skill);

	void ChangeModel(const char *NewModel, SItemID &item);

	bool Die();
	bool Refine();

	bool PlayPose(const char vpPoseName[], bool loop=false, float fSpeed=1.f);

	virtual void OnPoseBegin(SRT_Pose *pose); 
	virtual void OnPoseEnd(SRT_Pose *pose) ;
	virtual void OnPoseEvent(SRT_Pose *pose,SRT_PoseEvent *event);
	virtual void OnPoseBreak(SRT_Pose *oldPose,SRT_Pose *newPose);

    string      m_modelName;
	SItemID		m_item;
	TrumpState  m_state;
	GcBaseActor *m_host;
	CRT_ActorInstance *m_body;
    CRT_ActorInstance *m_way;
	CRT_ActorInstance *m_skillWay;
	CRT_ActorInstance *m_skillEffect;

	CRT_ActorInstance *m_FlySwordRib;		// ·É½£µÄµ¶¹â

	CRT_ActorInstance *m_body1;
	CRT_ActorInstance *m_body2;
	RtgMatrix12 m_worldMatrix1;
	RtgMatrix12 m_worldMatrix2;

	RtgMatrix12 m_worldMatrix;
	float m_wayScale;

	DWORD m_delta;
	DWORD m_idleDelta;
	DWORD m_life;
	DWORD m_targetId;
	bool m_bRender;
	// list<RtgMatrix12> m_oldPos;
};

#endif

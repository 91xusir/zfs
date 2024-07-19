#ifndef __WEAPON_SHOW_H__
#define __WEAPON_SHOW_H__

#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "character/actor.h"

class GcBaseActor;
struct SSkill;
using namespace std;

class CWeaponShow
{
public:
	CWeaponShow(void);
	virtual ~CWeaponShow(void);

	enum WeaponState {
		STATE_NULL,
		STATE_IDLE,
		STATE_GUARD,
		STATE_GO,
		STATE_ATTACK,
		STATE_SKILL,
		STATE_RETURN
	};

	void Tick(DWORD delta);
	void Render(RtgDevice *dev,RTGRenderMask vMask);
	void RenderHighLight(RtgDevice *dev, RtgVertex3 &color,RtgVertex3 &scale);
	void SetWeaponState( WeaponState state );

	// new interface
	void Init(GcBaseActor *Host,const char *Model, SWeapon *weapon, SItemID &item); // 最多支持link 3个body
	void DoAttack(DWORD TargetId, const char vpPoseName[],bool vLoop, float fSpeed);
	void DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill *skill);
	bool PlayPose(const char vpPoseName[], bool loop=false, float fSpeed=1.f);

	void ChangeModel(const char *NewModel, SItemID &item);

	void RevertLinkModel();

	string      m_modelName;
	string      m_wayName;

	SSkill		*m_skill;
	SItemID		m_item;
	SWeapon		*m_Weapon;
	WeaponState m_state;
	GcBaseActor *m_host;
	CRT_ActorInstance *m_body;
	CRT_ActorInstance *m_way;
	CRT_ActorInstance *m_skillEffect;

	CRT_ActorInstance *m_FlySwordRib;		// 飞剑的刀光

	RtgMatrix12 m_worldMatrix;
	vector<RtgMatrix12> m_vWorldMatrixOther;

	float m_wayScale;

	DWORD m_delta;
	DWORD m_idleDelta;
	DWORD m_life;
	DWORD m_targetId;

	bool m_bRender;
};

#endif
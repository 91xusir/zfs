#include "gc_include.h"
#include "game_assistant.h"
#include "region_client.h"
#include "item_base.h"
#include "trump_show.h"
#include <string.h>

CTrumpShow::CTrumpShow()
{
	m_body		= NULL;
	m_host		= NULL;
	m_way		= NULL;
	m_skillWay	= NULL;
	m_skillEffect = NULL;
	m_state		= STATE_IDLE;
	m_life		= 0;
	m_delta		= 0;
	m_idleDelta = 0;

	m_body1		= NULL;
	m_body2		= NULL;
	m_FlySwordRib = NULL;
	m_bRender = true;
}

CTrumpShow::~CTrumpShow()
{
	Safe_ReleaseActor(m_body);
	Safe_ReleaseActor(m_body1);
	Safe_ReleaseActor(m_body2);
	Safe_ReleaseActor(m_way);
	Safe_ReleaseActor(m_skillWay);
	Safe_ReleaseActor(m_skillEffect);
	Safe_ReleaseActor(m_FlySwordRib);
}

void CTrumpShow::Tick(float delta)
{
	guard;

	if(!m_way || !m_body || !m_host) return;
	if (!m_host->GetGraph()->p()) return;
	RtgMatrix12 tmp;
	RtgMatrix12 host;
	RtgMatrix12 pos;

    m_way->Tick(delta);

	switch(m_state)
	{
		case STATE_IDLE:
			{
				Safe_ReleaseActor(m_skillWay);
				if (m_host->m_pMaster->m_cIsDeadNet)
				{
					Die();
				}
				SItemID item = m_host->m_pMaster->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
				if(!m_way->IsPlayingPose())
				{
					m_state = STATE_IDLE;
					if (m_host->m_pMaster->GetRideState() == GcActor::ERS_ACTOR)
					{
						m_way->PlayPose("ride",false,1);
					}
					else
					{
						if(rand()%100 > 80)
							m_way->PlayPose("waiting",false,1);
						else
							m_way->PlayPose("wait",false,1);
					}
				}
				m_idleDelta += delta;

                RtgMatrix12 _mat;
                if (!m_way->GetBoneMat(&_mat, "Blink0"))
                    _mat.Unit();
				m_worldMatrix = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
				break;
			}

		case STATE_SKILL:
			if(!m_way->IsPlayingPose())
			{
				m_skillWay = NULL;
				Safe_ReleaseActor(m_skillEffect);
				

				m_state = STATE_IDLE;
				if(rand()%100 > 50)
					m_way->PlayPose("waiting",false,1);
				else
					m_way->PlayPose("wait",false,1);
				return;
			}

            if (!m_way->GetBoneMat(&tmp, "Blink0"))
                tmp.Unit();
			tmp._31 *= m_wayScale;
			m_worldMatrix = tmp * (*m_host->GetGraph()->p()->GetWorldMatrix());
			break;
		case STATE_REFINE:
            {
			if (m_host->m_pMaster->m_eIdlePose != GcBaseActor::POSE_IDLE)
			{
				if(!m_way->IsPlayingPose())
				{
					PlayPose("magic",false);
				}
			}
			else
			{
				m_host->m_pMaster->SetWeaponTrumpVisible(true,true);
				m_state = STATE_IDLE;
			}
			m_idleDelta += delta;

            RtgMatrix12 _mat;
            if (!m_way->GetBoneMat(&_mat, "Blink0"))
                _mat.Unit();
			m_worldMatrix = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
            }
			break;
		case STATE_DIE:
            {
			if (m_host->m_pMaster->m_cIsDeadNet)
			{
			}
			else
			{
				m_state = STATE_IDLE;
			}
			m_idleDelta += delta;

            RtgMatrix12 _mat;
            if (!m_way->GetBoneMat(&_mat, "Blink0"))
                _mat.Unit();
			m_worldMatrix = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
            }
			break;
	}

	m_delta += delta;

	m_body->SetWorldMatrix(m_worldMatrix);
	if(m_skillEffect)
		m_skillEffect->SetWorldMatrix(m_worldMatrix);
	
	RtgVertex3 vColor;
	g_pScene->GetTerrainColor(m_worldMatrix._30, m_worldMatrix._31, vColor);
	if(vColor.x < 0.3) vColor.x = 0.3;
	if(vColor.y < 0.3) vColor.y = 0.3;
	if(vColor.z < 0.3) vColor.z = 0.3;
	m_body->SetBaseColor(vColor);

	m_body->Tick(delta);

	if (m_skillWay)
		m_skillWay->Tick(delta);
	if (m_skillEffect)
		m_skillEffect->Tick(delta);
	if (m_FlySwordRib)
		m_FlySwordRib->Tick(delta);
	unguard;
}

bool CTrumpShow::Die()
{
	if (m_state == STATE_DIE)
	{
		return false;
	}
	m_state = STATE_DIE;
	return PlayPose("body");
}
bool CTrumpShow::Refine()
{
	if (m_state == STATE_REFINE)
	{
		return false;
	}
	m_state = STATE_REFINE;
	m_host->m_pMaster->SetWeaponTrumpVisible(false,true);
	return PlayPose("magic",false);
}

void CTrumpShow::Render(RtgDevice *dev,RTGRenderMask vMask)
{
	guard;

	if(!m_way || !m_body || !m_host || !m_bRender || m_host->m_pMaster->m_cShapeshift) return;
	
	RtgMatrix12 tmp,bak;
	m_body->Render(dev,vMask);
	bak = *m_body->GetMatrix();

	switch(m_state)
	{
		case STATE_SKILL:
			m_skillWay->Render(dev,vMask);
			if(m_skillEffect)
			{
				m_skillEffect->Render(dev,vMask);
			}
            RtgMatrix12 _mat;
			if (m_skillWay->GetBoneMat(&_mat, "Blink0"))
			{
				_mat._31 *= m_wayScale;
				_mat = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
                m_body->SetWorldMatrix(_mat);
				//*m_body->GetWorldMatrix() = tmp;
				m_body->SetVisible(0.6);
				m_body->Render(dev,vMask,false);
			}
            m_body->SetWorldMatrix(bak);
			//*m_body->GetWorldMatrix() = bak;
			break;
	}
	m_body->SetVisible(m_host->GetGraph()->p()->GetVisible());

	unguard;
}

void CTrumpShow::RenderHighLight(RtgDevice *dev, RtgVertex3 &color,RtgVertex3 &scale)
{
	guard;

    if(!m_way || !m_body || !m_host || !m_bRender || m_host->m_pMaster->m_cShapeshift) return;

    RtgMatrix12 tmp,bak;
    m_body->RenderHighLight(dev,color, scale);
    bak = *m_body->GetMatrix();

    switch(m_state)
    {
    case STATE_SKILL:
        m_skillWay->RenderHighLight(dev,color, scale);
        if(m_skillEffect)
        {
            m_skillEffect->RenderHighLight(dev,color, scale);
        }

        RtgMatrix12 _mat;
        if (m_skillWay->GetBoneMat(&_mat, "Blink0"))
        {
            _mat._31 *= m_wayScale;
            _mat = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
            m_body->SetWorldMatrix(_mat);
            m_body->SetVisible(0.6);
            m_body->RenderHighLight(dev,color, scale);
        }
        m_body->SetWorldMatrix(bak);
        break;
    }
    m_body->SetVisible(m_host->GetGraph()->p()->GetVisible());

	unguard;
}

void CTrumpShow::DoAttack(DWORD TargetId, bool vLoop, float fSpeed)
{
}

void CTrumpShow::DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill *skill)
{
	guard;

	if(!m_way || !m_body || !m_host || m_host->m_pMaster->m_cShapeshift) return;
	// if(m_state == STATE_ATTACK) return;

	RtgVertex3 src,tar;

	// get range and do scale
    if (bPosition)
    {
        tar = fPos;
        tar.z += 20;
    }else
    {
        GcActor* pActor = FindActor(TargetId,true);
        if(!pActor)
            return;
        if (pActor->GetGraph()->p()==NULL)
            return;
        tar = pActor->GetGraph()->p()->GetWorldMatrix()->Position();
    }
    if (m_host->GetGraph()->p()==NULL)
        return;
    src = m_host->GetGraph()->p()->GetWorldMatrix()->Position();

	float range = src.Distance(tar);
	m_wayScale = range / 160;

    Safe_ReleaseActor(m_skillEffect);


	m_skillWay = m_way;

	if(!m_skillWay)
		return;

	m_skillWay->LinkParent(m_host->GetGraph()->p());
	m_skillWay->PlayPoseInTime(skill->szWayName,skill->iRTime,false);

	m_skillEffect = RtcGetActorManager()->CreateActor(skill->szFlyEffect);
	if(m_skillEffect)
	{
		m_skillEffect->PlayPose("effect",true,1);
	}
	m_state = STATE_SKILL;

	unguard;
}

void CTrumpShow::Init(GcBaseActor *Host,const char *Model, SItemID &item, STrump *pTrump)
{
	guard;

	if ( !Host || !Model || !ItemIsTrump(item) )	return;

	if(!m_body)
	{	
		m_body = RtcGetActorManager()->CreateActor(Model);
		if(!m_body) 
            return;
	}

	if(!m_way)
	{
		m_way = RtcGetActorManager()->CreateActor(pTrump->Way);
		if (!m_way)		
            return;
		m_way->RegisterNotify(this);
	}

	m_host = Host;
    m_modelName = Model;
	m_item = item;
	m_state = STATE_IDLE;
	m_delta = 0;

	unguard;
}

void CTrumpShow::ChangeModel(const char *NewModel, SItemID &item)
{
	if ( !m_host || !NewModel || !ItemIsTrump(item) )	return;

    if (m_modelName==NewModel) // NewModel
		return;

	SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
	STrump *pTrump = (STrump *)pItemBase;

    if (m_body)
    {
        RtcGetActorManager()->ReleaseActor(m_body);
        m_body = NULL;
    }

    if(!m_body)
    {	
        m_body = RtcGetActorManager()->CreateActor(NewModel);
        if(!m_body) 
            return;
	}

	if (m_way)
	{
		RtcGetActorManager()->ReleaseActor(m_way);
		m_way = NULL;
	}

	if(!m_way)
	{
		m_way = RtcGetActorManager()->CreateActor(pTrump->Way);
		if (!m_way)		
            return;

	}

    m_item = item;
}

void CTrumpShow::OnPoseBegin(SRT_Pose *pose)
{
	const char *str = _tcsstr(pose->Name.c_str(),"skill");
	if (str)
	{
		m_host->OnPoseBegin(pose);
	}
}

void CTrumpShow::OnPoseBreak(SRT_Pose *oldPose,SRT_Pose *newPose)
{
	const char *str = _tcsstr(oldPose->Name.c_str(),"skill");
	if (str)
	m_host->OnPoseBreak(oldPose,newPose);
}

void CTrumpShow::OnPoseEnd(SRT_Pose *pose)
{
	const char *str = _tcsstr(pose->Name.c_str(),"skill");
	if (str)
		m_host->OnPoseEnd(pose);
}

void CTrumpShow::OnPoseEvent(SRT_Pose *pose,SRT_PoseEvent *event)
{
	const char *str = _tcsstr(pose->Name.c_str(),"skill");
	if (str)
		m_host->OnPoseEvent(pose,event);
}

bool CTrumpShow::PlayPose(const char vpPoseName[], bool loop/* =false */, float fSpeed/* =1.f */)
{
	if ( !m_way )
	{
		return false;
	}
	return m_way->PlayPose(vpPoseName,loop,fSpeed);
}

#include "gc_include.h"
#include "game_assistant.h"
#include "region_client.h"
#include "item_base.h"
#include "weapon_show.h"

CWeaponShow::CWeaponShow(void)
{
	m_body		= NULL;
	m_host		= NULL;
	m_way		= NULL;
	m_skillEffect = NULL;
	m_state		= STATE_NULL;
	m_life		= 0;
	m_delta		= 0;
	m_idleDelta = 0;
	m_FlySwordRib = NULL;
	m_skill = NULL;
	m_bRender = true;
}

CWeaponShow::~CWeaponShow(void)
{
	guard;
	if ( !ItemIsWeapon_NeedLink(m_item) )
	{
		RtcGetActorManager()->ReleaseActor(m_body);
		RtcGetActorManager()->ReleaseActor(m_way);
	}else
	{
		m_body = NULL;
		m_way = NULL;
	}
	RtcGetActorManager()->ReleaseActor(m_skillEffect);
	RtcGetActorManager()->ReleaseActor(m_FlySwordRib);
	unguard;
}

void CWeaponShow::Init(GcBaseActor *Host,const char *Model, SWeapon *weapon, SItemID &item)
{
	guard;

	m_host = Host;
	m_Weapon = weapon;

	m_modelName = Model;
	m_item = item;
	m_state = STATE_IDLE;
	m_delta = 0;

	if(!m_body)
	{
		if ( ItemIsWeapon_NeedLink(item) )
		{
			if (ItemIsWeapon_Riband(item))
				m_body = Host->m_pMaster->GetGraph()->GetLinkActor("Box03")->p();
			else if (ItemIsWeapon_Hoop(item))
				m_body = Host->m_pMaster->GetGraph()->GetLinkActor("Bip01 R Hand")->p();
			else
				m_body = Host->m_pMaster->GetGraph()->GetLinkActor("Box01")->p();
			if(!m_body) return;
		}else
		{
			m_body = RtcGetActorManager()->CreateActor(Model);
		
            if(!m_body) 
                return;

			m_body->PlayPose("effect",true);
		}
	}

	if( !m_way )
	{
		if ( ItemIsWeapon_NeedLink(item) )
		{
			m_way = m_body;
		}else
		{
			m_way = RtcGetActorManager()->CreateActor(m_Weapon->Way);
			if (!m_way)		
                return;
			m_wayName = m_Weapon->Way;
		}
	}

	if ( !m_FlySwordRib )
	{
		m_FlySwordRib = RtcGetActorManager()->CreateActor(m_Weapon->FlySwordRib);
		if (m_FlySwordRib)
		{
			m_FlySwordRib->PlayPose("effect",true);
		}
	}

	for (int i = 0;i<m_Weapon->Models1Num[0];i++)
	{
		RtgMatrix12 matrix;
		m_vWorldMatrixOther.push_back(matrix);
	}

	unguard;
}

void CWeaponShow::RevertLinkModel()
{
	if ( ItemIsWeapon_NeedLink(m_item) )
	{
		if (ItemIsWeapon_Riband(m_item))
			m_body = m_host->m_pMaster->GetGraph()->GetLinkActor("Box03")->p();
		else if (ItemIsWeapon_Hoop(m_item))
			m_body = m_host->m_pMaster->GetGraph()->GetLinkActor("Bip01 R Hand")->p();
		else
			m_body = m_host->m_pMaster->GetGraph()->GetLinkActor("Box01")->p();
		if(!m_body) return;
	}
	if ( ItemIsWeapon_NeedLink(m_item) )
	{
		m_way = m_body;
	}
}

void CWeaponShow::ChangeModel(const char *NewModel, SItemID &item)
{
	if(!m_way || !m_body || !m_host) return;

	if (m_modelName==NewModel) // NewModel
		return;

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
	m_item = item;
}

void CWeaponShow::DoAttack(DWORD TargetId, const char vpPoseName[],bool vLoop, float fSpeed)
{
	guard;

	if(!m_way || !m_body || !m_host) return;

	GcActor* pActor = FindActor(TargetId,true);
	if(!pActor)
		return;

	// get range and do scale
	RtgVertex3 src,tar;
	src = m_host->GetGraph()->p()->GetWorldMatrix()->Position();
	tar = pActor->GetGraph()->p()->GetWorldMatrix()->Position();

	if (!m_host || !m_host->m_pMaster)
		return; 

	SItemID itemWea = m_host->m_pMaster->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
	if (!ItemID_IsValid(itemWea))
		return;

	if ( !ItemIsWeapon_NeedLink(m_item) )
	{
		m_way->LinkParent(m_host->GetGraph()->p());
	}

	float range = src.Distance(tar);
	m_wayScale = range / 160.f;
	if ( !ItemIsWeapon_NeedLink(m_item) )
	{
		if(GetWorld()->m_pPlayer == m_host->m_pMaster)
		{
			//m_way->PlayPoseInTime(vpPoseName,m_host->m_pMaster->m_core.GetAttackInterval(),false);
			m_way->PlayPose(vpPoseName,false,fSpeed);
		}
		else 
		{
			m_way->PlayPose(vpPoseName,false,fSpeed);
		}
	}else if (ItemIsWeapon_Riband(m_item))
	{
		m_way->PlayPose(vpPoseName,false,fSpeed);
	}
	m_state = STATE_ATTACK;

	if (m_FlySwordRib)
	{
		m_FlySwordRib->ResetRibbonEffect();
		if ( ItemIsWeapon_NeedLink(m_item) )
		{
			m_FlySwordRib->LinkParent(m_way,"Box06");
		}else
		{
			m_FlySwordRib->LinkParent(m_way,"Blink0");
		}
		m_FlySwordRib->m_bRenderByParent = true;
	}
	unguard;
}

void CWeaponShow::DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill *skill)
{
	guard;

	if(!m_way || !m_body || !m_host) return;

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

	m_skillEffect = RtcGetActorManager()->CreateActor(skill->szFlyEffect);
	if(m_skillEffect)
	{
		m_skillEffect->PlayPose("effect",true,1);
	}
	m_state = STATE_SKILL;
	m_skill = skill;

	if (m_FlySwordRib)
	{
		m_FlySwordRib->ResetRibbonEffect();
		if ( ItemIsWeapon_NeedLink(m_item) )
		{
			m_FlySwordRib->LinkParent(m_way,"Box06");
		}else
		{
			m_FlySwordRib->LinkParent(m_way,"Blink0");
		}
		m_FlySwordRib->m_bRenderByParent = true;
	}
	unguard;
}

void CWeaponShow::Render(RtgDevice *dev,RTGRenderMask vMask)
{
	guard;

	if(!m_way || !m_body || !m_host || !m_bRender || m_host->m_pMaster->m_cShapeshift) return;

	RtgMatrix12 tmp,bak;

	if ( m_Weapon->Models1Num[0] > 1 )
	{
		for (int i = 0;i<m_Weapon->Models1Num[0];i++)
		{
			char link[10];
			rt2_snprintf(link,10,"Blink%d",i);
            RtgMatrix12 _mat;
            if (!m_way->GetBoneMat(&_mat, link))
               _mat.Unit();
			_mat = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
			m_body->SetWorldMatrix(_mat);
			m_body->Render(dev,vMask);
			m_vWorldMatrixOther[i] = _mat;
		}
	}
	else if ( !ItemIsWeapon_NeedLink(m_item) )
	{
		m_body->Render(dev,vMask);
	}
	m_way->Render(dev,vMask);

	switch(m_state)
	{
	case STATE_IDLE:
		{
		}
		break;

	case STATE_ATTACK:
		{
		}
		break;

	case STATE_SKILL:
		{
			if(m_skillEffect)
			{
				m_skillEffect->Render(dev,vMask);
			}
		}
		break;
	}


	unguard;
}

void CWeaponShow::RenderHighLight(RtgDevice *dev, RtgVertex3 &color,RtgVertex3 &scale)
{
	guard;

	if(!m_way || !m_body || !m_host || !m_bRender || m_host->m_pMaster->m_cShapeshift) return;

	RtgMatrix12 tmp,bak;

	if ( m_Weapon->Models1Num[0] > 1 )
	{
		for (int i= 0;i<m_Weapon->Models1Num[0];i++)
		{
			char link[10];
			rt2_sprintf(link,"Blink%d",i);

            RtgMatrix12 _mat;
            if (!m_way->GetBoneMat(&_mat, link))
                _mat.Unit();
			_mat = _mat * (*m_host->GetGraph()->p()->GetWorldMatrix());
			m_body->SetWorldMatrix(_mat);
			m_body->RenderHighLight(dev,color, scale);
		}
	}
	else if ( ItemIsWeapon_Sword(m_item) || ItemIsWeapon_Wheel(m_item) || ItemIsWeapon_Ball(m_item))
	{
		m_body->RenderHighLight(dev,color, scale);
	}

	switch(m_state)
	{
	case STATE_IDLE:
		{
		}
		break;
	case STATE_ATTACK:
		{

		}
		break;
	case STATE_SKILL:
		{
			if(m_skillEffect)
			{
				m_skillEffect->RenderHighLight(dev,color, scale);
			}
		}
		break;
	}

	unguard;
}

void CWeaponShow::Tick(DWORD delta)
{
	guard;

	if(!m_way || !m_body || !m_host || m_host->m_pMaster->m_pCollectTool[0]) return;

	if (!m_host->GetGraph()->p())return;

	RtgMatrix12 tmp;
	RtgMatrix12 host;
	RtgMatrix12 pos;

	tmp.Unit();
	
	if ( ItemIsWeapon_NeedLink(m_item) )
	{
        RtgMatrix12 _mat;
        if (m_way->GetBoneMat(&_mat, "Box06"))
            tmp = _mat * tmp;
	}else
	{
        RtgMatrix12 _mat;
        if (m_way->GetBoneMat(&_mat, "Blink0"))
            tmp = _mat * tmp;
	}

	switch(m_state)
	{
	case STATE_IDLE:
		{
			if (m_FlySwordRib && m_FlySwordRib->m_bRenderByParent)
			{
				m_FlySwordRib->UnlinkParent();
				m_FlySwordRib->m_bRenderByParent = false;
			}
			m_idleDelta += delta;
			if ( !ItemIsWeapon_NeedLink(m_item) )
			{
				m_way->UnlinkParent();
			}
			if ( ItemIsWeapon_NeedLink(m_item) )
			{
				tmp = tmp * (*m_body->GetWorldMatrix());
			}else
			{
				tmp = tmp * (*m_host->GetGraph()->p()->GetWorldMatrix());
			}
			m_worldMatrix = tmp;
			if(m_skillEffect)
			{
				m_skillEffect->SetVisible(0);
                Safe_ReleaseActor(m_skillEffect);
			}
		}
		break;
	case STATE_GUARD:
		{
			m_way->UnlinkParent();
			tmp = tmp * (*m_host->GetGraph()->p()->GetWorldMatrix());
			m_worldMatrix = tmp;
			if(m_skillEffect)
			{
				m_skillEffect->SetVisible(0);
                Safe_ReleaseActor(m_skillEffect);
			}
		}
		break;
	case STATE_ATTACK:
		{
			tmp._31 *= m_wayScale;
			if ( ItemIsWeapon_NeedLink(m_item) )
			{
				tmp = tmp * (*m_body->GetWorldMatrix());
			}else
			{
				tmp = tmp * (*m_host->GetGraph()->p()->GetWorldMatrix());
			}
			m_worldMatrix = tmp;
			if(m_skillEffect)
			{
				m_skillEffect->SetVisible(0);
                Safe_ReleaseActor(m_skillEffect);
			}
		}
		break;
	case STATE_SKILL:
		{
			tmp._31 *= m_wayScale;
			if ( ItemIsWeapon_NeedLink(m_item) )
			{
				tmp = tmp * (*m_body->GetWorldMatrix());
			}else
			{
				tmp = tmp * (*m_host->GetGraph()->p()->GetWorldMatrix());
			}
			m_worldMatrix = tmp;

			if(m_skillEffect)
				m_skillEffect->SetWorldMatrix(m_worldMatrix);
		}
		break;
	}

	m_delta += delta;
	if ( !ItemIsWeapon_NeedLink(m_item) )
	{
		m_body->SetWorldMatrix(m_worldMatrix);
	}

	if (!ItemIsWeapon_NeedLink(m_item))
	{
		
		RtgVertex3 vColor;
		g_pScene->GetTerrainColor(m_worldMatrix._30, m_worldMatrix._31, vColor);
		if(vColor.x < 0.3) vColor.x = 0.3;
		if(vColor.y < 0.3) vColor.y = 0.3;
		if(vColor.z < 0.3) vColor.z = 0.3;
		m_body->SetBaseColor(vColor);

	}
	m_way->Tick(delta);
	m_body->Tick(delta);

	if (m_skillEffect)
		m_skillEffect->Tick(delta);
	if (m_FlySwordRib)
		m_FlySwordRib->Tick(delta);
	unguard;
}

void CWeaponShow::SetWeaponState(WeaponState state)
{
	m_state = state;
}

bool CWeaponShow::PlayPose(const char vpPoseName[], bool loop, float fSpeed)
{
	guard;
	if ( !m_way || !m_bRender)
	{
		return false;
	}
	return m_way->PlayPose(vpPoseName,loop,fSpeed);
	unguard;
}
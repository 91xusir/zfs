//
// Actor implementation file
//
#include "character2/actor.h"

RT_IMPLEMENT_DYNCREATE(CRT_ActorInstance, CRT_ObjectInst, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_SkinInstance, CRT_ObjectInst, NULL, "")
RT_IMPLEMENT_DYNAMIC(CRT_MtlInstance, CRT_ObjectInst, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlLibInstance, CRT_ObjectInst, NULL, "")
RT_IMPLEMENT_DYNAMIC(CRT_EffectInstance, CRT_ObjectInst, NULL, "")

const int BOUND_MIN = 10;
/*
EActorQuality QuaActor = QUALITY_HIGH;
EActorQuality QuaSkin  = QUALITY_HIGH;
EActorQuality QuaMtl   = QUALITY_HIGH;
EActorQuality QuaEft   = QUALITY_HIGH;

void ActorSetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect)
{
	QuaActor = actor;
	QuaSkin  = skin;
	QuaMtl   = mtl;
	QuaEft   = effect;
}

void ActorGetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect)
{
	actor   = QuaActor; 
	skin	= QuaSkin;   
	mtl		= QuaMtl;    
	effect	= QuaEft; 
}
*/
// --------------------------------------------------------------------------------------------
// global function
// --------------------------------------------------------------------------------------------
bool RenderShadowMap(RtgDevice *dev,vector<SRT_RenderSurface> &rsList,vector<SRT_StaticVtx> &verList)
{
	DWORD sVB,sIB;
	BYTE *vb,*ib;
	unsigned int i;

	// fix later...
	if(verList.size() == 0) return false;
	// update vb
	vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,verList.size()*sizeof(RtgVertexVNT),
			FALSE,sizeof(RtgVertexVNT));

	for(i=0; i<verList.size(); i++)
	{
		memcpy(vb,&verList[i].pos,sizeof(RtgVertexVNT));
		vb += sizeof(RtgVertexVNT);
	}
	dev->m_pVB->EndUpload(sVB);
	dev->SetVertexFormat(RTG_VNT);

	for(i=0; i<rsList.size(); i++)
	{
		if(rsList[i].idxVec.size() == 0) continue;
		// upload ib
		ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, rsList[i].idxVec.size()*sizeof(short),
			FALSE, sizeof(short));

		memcpy(ib,&rsList[i].idxVec[0],rsList[i].idxVec.size()*sizeof(short));
		dev->m_pIB->EndUpload(sIB);

		// draw
		dev->m_pVB->Bind(0, 0);
		dev->m_pIB->Bind(0, 0);
		dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,
			0,
			verList.size(),
			0,
			rsList[i].idxVec.size()/3);
	}
	return true;
}

// --------------------------------------------------------------------------------------------
// actor instance
// --------------------------------------------------------------------------------------------
CRT_ActorInstance *CRT_ActorInstance::m_rootValid   = NULL;
CRT_ActorInstance *CRT_ActorInstance::m_rootUnvalid = NULL;

CRT_ActorInstance::CRT_ActorInstance():m_poseMap(NULL),m_notify(NULL),m_curPose(NULL),
m_curFrame(0),m_oldFrame(0),m_bLoop(false),m_lastFrame(-1),m_actor(NULL),m_parentSlotMat(NULL),
m_parent(NULL),m_son(NULL),m_next(NULL),m_prev(NULL),m_bValid(false)
{
	// bounding box
	m_boundingBox.vPos.Zero();
	m_boundingBox.vExt.Zero();

	m_worldMat.Unit();
	m_localMat.Unit();
	m_finalMat.Unit();
}

CRT_ActorInstance::~CRT_ActorInstance()
{

}

void CRT_ActorInstance::InstanceInit() 
{
	CRT_ObjectInst::InstanceInit();
	m_actor = (CRT_Actor*)m_data;
	m_poseMap = &m_actor->m_poseMap;

	// runtime option
	m_animSpeed	= 1.0;
	m_visible		= 1.0;
	m_scale.Set(1.0,1.0,1.0);
	m_offset.Zero();
	SetValid(true,true);
}

void CRT_ActorInstance::InstanceExit()
{
	CRT_ObjectInst::InstanceExit();
	CoreUnlinkParent();

	// move all son to root
	CRT_ActorInstance *tmp;
	if(m_son)
	{
		tmp = m_son;
		while(tmp)
		{
			tmp->m_parent = m_rootValid;
			tmp = tmp->m_next;
		}
		tmp = m_son;
		while(tmp->m_next)
			tmp = tmp->m_next;

		if(m_rootValid->m_son)
		{
			m_rootValid->m_son->m_prev = tmp; 
			tmp->m_next = m_rootValid->m_son;
			m_rootValid->m_son = m_son;
		}
		else
		{
			m_rootValid->m_son = m_son;
		}
	}

	for(int i=0; i<m_skinVec.size(); i++)
		ACTOR()->DeleteInstance(m_skinVec[i]);

	for(int i=0; i<m_effectVec.size(); i++)
		ACTOR()->DeleteInstance(m_effectVec[i]);

	// ACTOR()->Unload(m_actor);
}

void CRT_ActorInstance::OnDataChange(DWORD p1, DWORD p2)
{
	vector<CRT_SkinInstance*>::iterator skinIt;
	vector<CRT_EffectInstance*>::iterator eftIt;
	int idx;

	switch(p1)
	{
		case ON_ACTOR_SKIN_ADD:
			CRT_SkinInstance *skin;
			skin = (CRT_SkinInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_SKIN, 
				m_actor->m_skinVec[p2]);
			if(!skin) return;
			skin->Create(this, m_renderDev);
			m_skinVec.push_back(skin);
			break;
		
		case ON_ACTOR_SKIN_DEL:
			idx = 0;
			for(skinIt=m_skinVec.begin(); skinIt!=m_skinVec.end(); skinIt++,idx++)
			{
				if(idx==p2)
				{
					ACTOR()->DeleteInstance(*skinIt);
					m_skinVec.erase(skinIt);
					return;
				}
			}
			break;
		
		case ON_ACTOR_EFFECT_ADD:
			CRT_EffectInstance *eff;
			eff = (CRT_EffectInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_EFFECT, 
				m_actor->m_eftVec[p2]);
			if(!eff) return;
			eff->Init(m_renderDev, this);
			m_effectVec.push_back(eff);
			break;
		
		case ON_ACTOR_EFFECT_DEL:
			idx = 0;
			for(eftIt=m_effectVec.begin(); eftIt!=m_effectVec.end(); eftIt++,idx++)
			{
				if(idx==p2)
				{
					ACTOR()->DeleteInstance(*eftIt);
					m_effectVec.erase(eftIt);
					return;
				}
			}
			break;
		
		case ON_ACTOR_MATERIAL_CHANGE:
			idx = 0;
			for(skinIt=m_skinVec.begin(); skinIt!=m_skinVec.end(); skinIt++,idx++)
			{
				if(idx == p2)
				{
					(*skinIt)->ChangeMaterial(m_actor->m_skinVec[idx]->m_mtlLib);
					return;
				}
			}
			break;
	}
}
//void CRT_ActorInstance::OnMLoad(CRT_ObjectData* pData, EActorResType type)
//{
//    switch(type) 
//	{
//    case ACTOR_RES_TYPE_SKIN:
//
//		CRT_SkinInstance *skin = (CRT_SkinInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_SKIN, pData);
//		if(!skin) return;
//		skin->Create(this, m_renderDev);
//		m_skinVec.push_back(skin);
//    	break;
//    default:
//    }
//    
//}
void CRT_ActorInstance::Update(unsigned long delta)
{
	rtClock(PFM()->phyTimer)

	CRT_ActorInstance *p;
	for(p=m_rootValid->m_son; p; p=p->m_next)
	{
		p->Tick(delta);
	}

	rtUnclock(PFM()->phyTimer);
}

void CRT_ActorInstance::PrintValidObject(const char* szFilename)
{
	/*
    FILE* fp;
    if (szFilename)
    {
        fp = fopen(szFilename, "wt");
        if (fp==NULL) return;
    }
    if (szFilename)
    {
        fprintf(fp, "Dump valid object\n");
    }else
    {
        RtCoreLog().Info("Dump valid object\n");
    }
    int iCnt = 0;
	CRT_ActorInstance *p;
	for(p=m_rootValid->m_son;p;p=p->m_next)
	{
        if (szFilename)
        {
            fprintf(fp, "    %s\n", p->m_core->m_poTag.c_str());
        }else
        {
            RtCoreLog().Info("    %s\n",p->m_core->m_poTag.c_str());
        }
        iCnt ++;
	}
    if (szFilename)
    {
        fprintf(fp, "Total object = %d\n", iCnt);
        fclose(fp);
    }else
    {
        RtCoreLog().Info("Total object = %d\n",iCnt);
    }
	*/
}

void CRT_ActorInstance::SetValid(bool valid, bool force)
{

	if((m_bValid == valid) && !force) return;
	m_bValid = valid;
	
	if(m_bValid)
	{
		if(m_parent && m_parent!=m_rootUnvalid) return;
		CoreUnlinkParent();
		CoreLinkParent(m_rootValid,NULL);
	}
	else
	{
		if(m_parent && m_parent!=m_rootValid) return;
		CoreUnlinkParent();
		CoreLinkParent(m_rootUnvalid,NULL);
	}
}

bool CRT_ActorInstance::CoreLinkParent(CRT_ActorInstance *parent, const char *slot)
{
	funguard;
	if(!parent || m_parent) return false;

	RtgMatrix12 *t;
	if(slot)
	{
		t = parent->GetSlotMatrix(slot);
		if(!t) return false;
	}

	if(slot)
		m_parentSlotMat = t;
	else
		m_parentSlotMat = NULL;

	if(slot) m_parentSlot = slot;
	else m_parentSlot = "";
	m_parent = parent;

	m_next = parent->m_son;
	m_prev = NULL;
	if(m_next) m_next->m_prev = this;
	parent->m_son = this;

	return true;
	fununguard;
}

void CRT_ActorInstance::CoreUnlinkParent()
{
	if(!m_parent) return;

	if(m_prev)
	{
		m_prev->m_next = m_next;
		if(m_next)
			m_next->m_prev = m_prev; 
	}
	else
	{
		m_parent->m_son = m_next;
		if(m_next)
			m_next->m_prev = NULL;
	}

	m_parent		 = NULL;
	m_parentSlotMat  = NULL;
	m_parentSlot	 = "";
	m_prev = NULL;
	m_next = NULL;
}

bool CRT_ActorInstance::LinkParent(CRT_ActorInstance *parent, const char *slot)
{
	CoreUnlinkParent();
	if(CoreLinkParent(parent,slot))
	{
		return true;
	}
	else
	{
		// link to root
		SetValid(true,true);
		return false;
	}
}

void CRT_ActorInstance::UnlinkParent()
{
	CoreUnlinkParent();

	// link to root
	SetValid(true,true);
}

SRT_Pose *CRT_ActorInstance::GetPose(const char *name)
{
	TPoseMap::iterator it;
	it = m_poseMap->find(name);
	if(it != m_poseMap->end())
	{
		return &((*it).second);
	}
    return NULL;
}

SRT_Pose *CRT_ActorInstance::GetCurrentPose()
{
	return m_curPose;
}

bool CRT_ActorInstance::PlayPose(const char *name,bool loop,float speed)
{
	SRT_Pose *pose = GetPose(name);
	if(!pose) return false;

	if(m_curPose && m_notify)
		m_notify->OnPoseBreak(m_curPose,pose);

    m_animSpeed = speed;
	m_bLoop   = loop;
	m_curPose = pose;
	m_curFrame = pose->startFrm;
	m_oldFrame = pose->startFrm;
	UseFrame(m_curFrame);
	if(m_notify)
		m_notify->OnPoseBegin(m_curPose);

	return true;
}

bool CRT_ActorInstance::PlayPoseInTime(const char *name,int mills,bool loop)
{
	SRT_Pose *pose = GetPose(name);
	if(!pose) return false;

	float speed = (pose->endFrm - pose->startFrm) / 30.0f * 1000.0f / mills;
	return PlayPose(name,loop,speed);
}

void CRT_ActorInstance::StopPose()
{
	m_curPose = NULL;
}

void CRT_ActorInstance::RegisterNotify(CRT_PoseNotify *notify)
{
	m_notify = notify;
}

void CRT_ActorInstance::Tick(unsigned long delta)
{
	funguard;
	PFM()->actorNum++;

	// update local matrix
	m_localMat.Unit();
	m_localMat._00 = m_actor->m_scale.x * m_scale.x;
	m_localMat._11 = m_actor->m_scale.y * m_scale.y;
	m_localMat._22 = m_actor->m_scale.z * m_scale.z;
	m_localMat._30 = m_actor->m_offset.x + m_offset.x;
	m_localMat._31 = m_actor->m_offset.y + m_offset.y;
	m_localMat._32 = m_actor->m_offset.z + m_offset.z;

	funguard_name(phy_update);
	// update matrix
	if(m_parent && m_parent!=m_rootValid)
	{
		if(m_parentSlotMat)
		{
			m_parentSlotMat->NormalizeMatrix();
            m_finalMat = m_localMat * (*m_parentSlotMat) * (m_parent->m_finalMat);
		}			
		else
			m_finalMat = m_localMat * (m_parent->m_finalMat);
	}
	else
	{
		m_finalMat = m_localMat * m_worldMat;
	}
    fununguard;
	if(ACTOR()->m_bUpdateBoundingBox)
		UpdateBoundingBox();

    float framestart = 0;
	funguard_name(pose_update);
	
	if(m_curPose)
	{
		SRT_Pose *CurPose = m_curPose;
		float OldFrame = m_oldFrame;
		// get current frame
		m_curFrame += (delta/1000.0f*30.0f*GetFinalAnimSpeed());
		m_oldFrame  = m_curFrame;
		framestart = m_curPose->startFrm;
		if(m_curFrame >= m_curPose->endFrm)
		{
			if(m_bLoop)
			{
				ProcessPoseEvent( OldFrame,m_curPose->endFrm+1,CurPose);
				m_curFrame = m_curPose->startFrm;
				m_oldFrame = m_curFrame;
			}
			else
			{
				ProcessPoseEvent(OldFrame,m_curPose->endFrm+1,CurPose);
				m_curFrame = m_curPose->endFrm;
				m_curPose  = NULL;
				m_oldFrame = m_curFrame;

				// careful,maybe call PlayPose in OnPoseEnd
				if(m_notify) m_notify->OnPoseEnd(CurPose);
			}
		}
		else
		{
			ProcessPoseEvent(OldFrame,m_curFrame,CurPose);
			
		}
	}
	fununguard;
	RealUseFrame(m_curFrame,framestart);

	int i;
	funguard_name(skin_update);
	for(i=0; i<m_skinVec.size(); i++)
		if(m_skinVec[i]) m_skinVec[i]->Tick(delta);
	fununguard;

    funguard_name(eft_update);
	for(i=0; i<m_effectVec.size(); i++)
		if(m_effectVec[i]->RequestTick()) m_effectVec[i]->Tick(delta);
	fununguard;

	funguard_name(son_tick);
	CRT_ActorInstance *p;
	for(p=m_son;p;p=p->m_next)
	{
		p->Tick(delta);
	}
	fununguard;

	fununguard;
}

void CRT_ActorInstance::ProcessPoseEvent(float oldFrame,float curFrame,SRT_Pose *curPose)
{
	// check pose event
	int num;
	// if(!m_notify || (num=curPose->EventList.size()) <= 0)
	if((num=curPose->eventVec.size()) <= 0)
		return;

    for(int i=0; i<num; i++)
	{
		SRT_PoseEvent *event = &curPose->eventVec[i];
		long key = event->keyFrame;
		if(oldFrame <= key && curFrame > key)
		{
			// default action
			char buf[32];
			float rate;
			if(event->action == "sound")
			{
				if(sscanf(event->param.c_str(),"%s %f",buf,&rate) != 2)
				{
					RtCoreLog().Info("DefaultPoseEvent: sound param error\n");
					return;
				}
				int ran = rand() << 16;
				if(ran%1000000 < rate*1000000)
				{
					// play sound
					int len = strlen(buf);
					buf[len++] = '.';
					buf[len++] = 'w';
					buf[len++] = 'a';
					buf[len++] = 'v';
					buf[len++] = '\0';
					g_pSoundMgr->PlayOnce(buf,false,0,RtgVertex3(m_finalMat._30,m_finalMat._31,m_finalMat._32));
				}
			}
			else if(event->action == "camera light")
			{
				// param
				RtgVertex3 color;
				if(sscanf(event->param.c_str(), "%f %f %f", &color.m_v[0], &color.m_v[1], &color.m_v[2]) != 3)
				{
					RtCoreLog().Info("DefaultPoseEvent: camera light param error\n");
					return;
				}
				// g_pDevice->m_pCamera->SetColoredGlass(RtgCamera::COLOR_ADD,RtgVectorToColor(color,1));
			}
			else if (event->action == "camera quake")
			{
				float fScope, fCycle, fTime;
				if(sscanf(event->param.c_str(), "%f %f %f", &fScope, &fCycle, &fTime) != 3)
				{
					RtCoreLog().Info("DefaultPoseEvent: camera quake param error\n");
					return;
				}
				// g_pDevice->m_pCamera->Tremble(fScope, fCycle, fTime);
			}
			else if(m_notify)
			{
				// careful,maybe call PlayPose in OnPoseEvent
				m_notify->OnPoseEvent(curPose,&curPose->eventVec[i]);
				// RtCoreLog().Info("base pose event %f %f key = %d\n",oldFrame,curFrame,key);
			}
		}
	}
}

void CRT_ActorInstance::LoadSkin(const char *name,bool breload /*= false*/ )
{				
	//tim.yang   解决人物穿两套衣服的bug
	string fullname = name;
	fullname = fullname + ACTOR()->GetFileExtByType(ACTOR_RES_TYPE_SKIN);
	for(vector<CRT_SkinInstance*>::iterator it = m_skinVec.begin();it != m_skinVec.end();++it )
	{
		CRT_SkinInstance *skin = *it;
		if(skin->m_skin->m_poTag == fullname)    
		{
			return;
		}
	}
	//end
	CRT_SkinInstance *skin = (CRT_SkinInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_SKIN, name , NULL , NULL , breload );
	if(!skin) return;
	skin->Create(this, m_renderDev);
	m_skinVec.push_back(skin);
}

void CRT_ActorInstance::UnloadSkin(const char *name)
{
	string fullname = name;
	fullname = fullname + ACTOR()->GetFileExtByType(ACTOR_RES_TYPE_SKIN);
	
	vector<CRT_SkinInstance*>::iterator it;
	for(it=m_skinVec.begin(); it!=m_skinVec.end(); it++)
	{
        CRT_SkinInstance *skin = *it;
		if(skin->m_skin->m_poTag == fullname)
		{
			m_skinVec.erase(it);
            ACTOR()->DeleteInstance(skin);
            return;
		}
	}
}

bool CRT_ActorInstance::ApplyEffect(const char *name)
{
	// const char *real = GetSkinFileName(name);
	// CRT_Effect *effect = LoadEffect(name);
	CRT_EffectInstance *effect = (CRT_EffectInstance*)ACTOR()->CreateInstance(
		ACTOR_RES_TYPE_EFFECT,name);
	if(effect)
	{
		effect->Init(m_renderDev, this);
		m_effectVec.push_back(effect);
		return true;
	}
	return false;
}

bool CRT_ActorInstance::RemoveEffect(const char *name)
{
	string fullname = name;
	fullname = fullname + ACTOR()->GetFileExtByType(ACTOR_RES_TYPE_EFFECT);
	
	vector<CRT_EffectInstance*>::iterator it;
	for(it=m_effectVec.begin(); it!=m_effectVec.end(); it++)
	{
		CRT_EffectInstance *ef = *it;
		if(ef->m_effect->m_poTag == fullname)
		{
			m_effectVec.erase(it);
            ACTOR()->DeleteInstance(ef);
			return true;
		}
	}
	return false;
}

bool CRT_ActorInstance::Create(RtgDevice *dev,RtgVertex3 scale)
{
    if(!m_actor) return false;
	m_renderDev = dev;

	// m_core->Build();
	m_actor->UseFrame(0,NULL);

	// fill slot list
	for(int i=0; i<m_actor->m_boneVec.size(); i++)
	{
		SSlot slot;
		slot.name = m_actor->m_boneVec[i].name;
		slot.mat  = m_actor->m_boneVec[i].skinMat;
		m_slotVec.push_back(slot);
	}

    // create all skin
	for(int i=0; i<m_actor->m_skinVec.size(); i++)
	{
		CRT_SkinInstance *tmp = (CRT_SkinInstance*)ACTOR()->CreateInstance(
			ACTOR_RES_TYPE_SKIN, m_actor->m_skinVec[i]);
        tmp->Create(this,dev);
        m_skinVec.push_back(tmp);
	}

    // create all effect
	for(int i=0; i<m_actor->m_eftVec.size(); i++)
	{
        CRT_EffectInstance *eft = (CRT_EffectInstance*)ACTOR()->CreateInstance(
			ACTOR_RES_TYPE_EFFECT, m_actor->m_eftVec[i]);
		if(eft)
        {
       		m_effectVec.push_back(eft);
            eft->Init(dev,this);
        }
        else
        {
            // log error
            RtCoreLog().Error("%s: create effect failed\n",__FUNCTION__);
        }
	}

	SetScale(scale);

    return true;
}

void CRT_ActorInstance::SetOffset(RtgVertex3 offset)
{
	m_offset = offset;
}

RtgVertex3 CRT_ActorInstance::GetFinialScale()
{
	RtgVertex3 v;
	v.x = m_actor->m_scale.x * m_scale.x;
	v.y = m_actor->m_scale.y * m_scale.y;
	v.z = m_actor->m_scale.z * m_scale.z;
	return v;
}

void CRT_ActorInstance::SetScale(RtgVertex3 scale)
{
	m_scale = scale; 
	InitBoundingBox();
}

bool CRT_ActorInstance::IsScaled()
{
	RtgVertex3 v;
	v = GetFinialScale();
	if(abs(v.x-1)>0.01 || abs(v.y-1)>0.01 || abs(v.z-1)>0.01)
		return true;
	else
		return false;
}

float CRT_ActorInstance::GetFinalVisible()
{
	return m_visible*m_actor->m_visible;
}

float CRT_ActorInstance::GetFinalAnimSpeed()
{
	return m_animSpeed*m_actor->m_animSpeed;
}

RtgMatrix12 *CRT_ActorInstance::GetLocalMatrix() 
{ 
	return &m_localMat;
}

RtgMatrix12 *CRT_ActorInstance::GetWorldMatrix() 
{ 
	return &m_worldMat; 
}

int TexPostProcessRenderTarget = 0;
void CRT_ActorInstance::Render(RtgDevice *dev,RTGRenderMask mask,bool bEffect)
{
	if(!ACTOR()->m_bRenderActor) return;
	
	//dev->SetRenderState(RTGRS_FULLSCRENE_ANTIALIASING,FALSE);
	rtClock(PFM()->renderTimer);
	 dev->SetTextureFilterMin(RTGTEXF_LINEAR);
	 dev->SetTextureFilterMag(RTGTEXF_LINEAR);
	 dev->SetTextureFilterMip(RTGTEXF_LINEAR);

	RtgMatrix16 world;
	world.Set4X3Matrix(m_finalMat);
	dev->SetMatrix(RTGTS_WORLD,&world);
	if(IsScaled())
		dev->SetRenderState(RTGRS_NORMALIZENORMALS,TRUE);
	else
		dev->SetRenderState(RTGRS_NORMALIZENORMALS,FALSE);

	    
    if(m_actor->m_lightingMode == LIGHTING_MODE_FULL)
		dev->SetRenderState(RTGRS_LIGHTING,TRUE);
	else
		dev->SetRenderState(RTGRS_LIGHTING,FALSE);

	if(!m_actor->m_bZTest)
		dev->SetRenderState(RTGRS_Z_TEST,FALSE);

	for(int i=0;i<m_skinVec.size();i++)
	{
		if(!m_skinVec[i]) continue;
		if(m_skinVec[i]->m_skin->m_bHelper && !ACTOR()->m_bRenderHelpSkin) continue;
        m_skinVec[i]->Render(dev,mask);
	}
	// rtUnclock(PFM()->renderTimer);

	if(!m_actor->m_bZTest)
		dev->SetRenderState(RTGRS_Z_TEST,TRUE);

	if(mask != RTGRM_NOBLENDOBJ_ONLY)
	{
		if(bEffect)
		{
			for(int i=0;i<m_effectVec.size();i++)
			{
				if(!m_effectVec[i]) continue;
				m_effectVec[i]->Render(dev,mask);
			}
		}
	}
	rtUnclock(PFM()->renderTimer);
}
void CRT_ActorInstance::RenderHighLight(RtgDevice *dev, RtgVertex3 &color, RtgVertex3 &scale)
{
}
void CRT_ActorInstance::RenderShadow(RtgDevice *dev , float fShadowFadeFactor/* = 0.5f*/)
{
	if(!ACTOR()->m_bRenderShadow) 
	{
		return;
	}
	if(m_actor->m_shadowMode != SHADOW_MODE_DYNAMIC)
	{
		return;
	}
	
	rtClock(PFM()->shadowTimer);
	// set world matrix
	RtgMatrix16 world;
	world.Set4X3Matrix(m_finalMat);
	dev->SetMatrix(RTGTS_WORLD,&world);

	for(int i=0;i<m_skinVec.size();i++)
	{
		if(m_skinVec[i]->m_skin->m_bHelper)
		{
			continue;
		}
		m_skinVec[i]->m_mtlLib->RenderShadow(dev , fShadowFadeFactor);
	}
	rtUnclock(PFM()->shadowTimer);
}

bool CRT_ActorInstance::ComputeVC(RtgMatrix16 &world,vector<DWORD> &vcVec,RtgVertex3 vSkyLight, RtgLightItem* pLights)
{
	// if(!IsEnableVC()) return false;
	int num = 0;
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		num += m_skinVec[i]->m_verVec.size();
	}
	vcVec.reserve(num);

	int offset = 0;
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		if(m_skinVec[i]->m_skin->m_bHelper) continue;
		m_skinVec[i]->ComputeVC(world,vcVec,offset,vSkyLight,pLights);
		offset += m_skinVec[i]->m_verVec.size();
	}
	return true;
}

bool CRT_ActorInstance::SetVC(vector<DWORD> &vcVec)
{
	// if(!IsEnableVC()) return false;
	int num = 0;
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		if(m_skinVec[i]->m_skin->m_bHelper) continue;
		num += m_skinVec[i]->m_verVec.size();
	}
	if(vcVec.size() != num)
	{
		RtCoreLog().Error("SetVC Error: obj = %s,ver = %d,vc = %d\n",
			m_actor->m_poTag.c_str(), num, vcVec.size());
		return false;
	}

	int offset = 0;
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		if(m_skinVec[i]->m_skin->m_bHelper) continue;
		m_skinVec[i]->SetVC(vcVec,offset);
		offset += m_skinVec[i]->m_verVec.size();
	}
	return true;
}

RtgMatrix12 *CRT_ActorInstance::GetSlotMatrix(const char *name)
{
	for(int i=0; i<m_slotVec.size(); i++)
	{
		if(m_slotVec[i].name == name)
			return &m_slotVec[i].mat;
	}
	return NULL;
}

void CRT_ActorInstance::RealUseFrame(float frame ,float framestart)
{
#ifndef ACTOR_EDITOR
	if(frame == m_lastFrame) return;
#endif
	rtClock(PFM()->boneTimer);
	if(!m_actor->UseFrame(frame,NULL)) 
	{
		rtUnclock(PFM()->boneTimer);
		return;
	}
	rtUnclock(PFM()->boneTimer);
	m_boundingBox = m_actor->m_boundBoxVec[frame];

	m_curFrame  = frame;
	m_lastFrame = frame;	
	
	// RtCoreLog().Info("real use frame %f\n",frame);
	int i;
	// copy slot matrix
	for(i=0; i<m_slotVec.size(); i++)
		m_slotVec[i].mat = m_actor->m_boneVec[i].skinMat;

	for(i=0; i<m_skinVec.size(); i++)
		m_skinVec[i]->UseFrame(frame);
	
	for(i=0; i<m_effectVec.size(); i++)
		if(m_effectVec[i]->RequestUseFrame()) m_effectVec[i]->UseFrame(frame,framestart);
}

void CRT_ActorInstance::UseFrame(float frame)
{
	m_curFrame = frame;
}

void CRT_ActorInstance::SetVisible(float visible)
{
	/*
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		m_skinVec[i]->SetVisible(visible);
	}
	*/
	m_visible = visible;
}

RtgAABB *CRT_ActorInstance::GetBoundingBox()
{
	return &m_boundingBox;
}

RtgAABB *CRT_ActorInstance::GetStaticBoundingBox()
{
	return &m_staticBoundingBox;
}

RtgAABB *CRT_ActorInstance::GetWorldBoundingBox()
{
	return &m_worldBoundingBox;
}

RtgAABB *CRT_ActorInstance::GetWorldStaticBoundingBox()
{
	return &m_worldStaticBoundingBox;
}

void CRT_ActorInstance::UpdateBoundingBox()
{
	RtgMatrix16 mat;
	mat.Unit();
	mat = m_finalMat;

	m_worldBoundingBox.vPos = m_boundingBox.vPos * mat;
	m_worldBoundingBox.vExt = m_boundingBox.vExt * mat;

	m_worldStaticBoundingBox.vPos = m_staticBoundingBox.vPos * mat;
	m_worldStaticBoundingBox.vExt = m_staticBoundingBox.vExt * mat;
}

void CRT_ActorInstance::InitBoundingBox()
{
	RtgRangeAABB tmp;
	float min[3],max[3];
	min[0] = -10;
	min[1] = -10;
	min[2] = 0;
	max[0] = 10;
	max[1] = 10;
	max[2] = 40;

	tmp.vMin.Set(min);
	tmp.vMax.Set(max);
#ifdef CHAR_VIEWER
	// min bounding box to [-10,10],some effect has no bounding box
	tmp.vMin.Set(-10.f);
	tmp.vMax.Set(10.f);
#endif
	
	m_boundingBox = m_actor->m_boundBoxVec[0];

	m_boundingBox.vPos.x=m_boundingBox.vPos.x*m_scale.x;
	m_boundingBox.vPos.y=m_boundingBox.vPos.y*m_scale.y;
	m_boundingBox.vPos.z=m_boundingBox.vPos.z*m_scale.z;
	m_boundingBox.vExt.x=m_boundingBox.vExt.x*m_scale.x;
	m_boundingBox.vExt.y=m_boundingBox.vExt.y*m_scale.y;
	m_boundingBox.vExt.z=m_boundingBox.vExt.z*m_scale.z;

	if(m_boundingBox.vExt.x < BOUND_MIN)
		m_boundingBox.vExt.x = BOUND_MIN;
	if(m_boundingBox.vExt.y < BOUND_MIN)
		m_boundingBox.vExt.y = BOUND_MIN;
	if(m_boundingBox.vExt.z < BOUND_MIN)
		m_boundingBox.vExt.z = BOUND_MIN;

	m_staticBoundingBox = m_actor->m_boundBoxVec[0];

	m_staticBoundingBox.vPos.x=m_staticBoundingBox.vPos.x*m_scale.x;
	m_staticBoundingBox.vPos.y=m_staticBoundingBox.vPos.y*m_scale.y;
	m_staticBoundingBox.vPos.z=m_staticBoundingBox.vPos.z*m_scale.z;
	m_staticBoundingBox.vExt.x=m_staticBoundingBox.vExt.x*m_scale.x;
	m_staticBoundingBox.vExt.y=m_staticBoundingBox.vExt.y*m_scale.y;
	m_staticBoundingBox.vExt.z=m_staticBoundingBox.vExt.z*m_scale.z;

	if(m_staticBoundingBox.vExt.x < BOUND_MIN)
		m_staticBoundingBox.vExt.x = BOUND_MIN;
	if(m_staticBoundingBox.vExt.y < BOUND_MIN)
		m_staticBoundingBox.vExt.y = BOUND_MIN;
	if(m_staticBoundingBox.vExt.z < BOUND_MIN)
		m_staticBoundingBox.vExt.z = BOUND_MIN;
}

void CRT_ActorInstance::SetBaseColor(RtgVertex3 &color)
{
	for(int i=0; i<m_skinVec.size(); i++)
	{
		if(!m_skinVec[i]) continue;
		m_skinVec[i]->SetBaseColor(color);
	}
}

void CRT_ActorInstance::OnMaterialChange()
{
	for(int i=0; i<m_skinVec.size(); i++)
		m_skinVec[i]->OnMaterialChange();
}

vector<RtgLight*> CRT_ActorInstance::GetLightingList()
{
	vector<RtgLight*> lightList;
	/*
	for(int i=0; i<m_effectList.size(); i++)
	{
		if(m_effectList[i]->GetRuntimeClass() == RT_RUNTIME_CLASS(CRT_EffectLight))
		{
			CRT_EffectLight *el = (CRT_EffectLight*)m_effectList[i];
			if(!el->m_light) continue;
			lightList.push_back(el->m_light);
		}
	}
	*/
	return lightList;
}

bool CRT_ActorInstance::IsLineInsertActor(RtgVertex3 vOrig,RtgVertex3 vDir,RtgDevice* dev,RtgVertex3& v1,RtgVertex3& v2,RtgVertex3& v3)
{
	for(int i=0;i<m_skinVec.size();i++)
	{
		for(int j=0;j < m_skinVec[i]->m_skin->m_rsVec.size();j++)
		{
			for(int k =0;k< m_skinVec[i]->m_skin->m_rsVec[j].idxVec.size();k+=3)
			{
				short k1 = m_skinVec[i]->m_skin->m_rsVec[j].idxVec[k];
				short k2 = m_skinVec[i]->m_skin->m_rsVec[j].idxVec[k+1];
				short k3 = m_skinVec[i]->m_skin->m_rsVec[j].idxVec[k+2];

				v1 = m_skinVec[i]->m_verVec[k1].pos;
				v2 = m_skinVec[i]->m_verVec[k2].pos;
				v3 = m_skinVec[i]->m_verVec[k3].pos;

				if(rtgIsLineIntersectTriangle(vOrig.m_v,vDir.m_v,v1.m_v,v2.m_v,v3.m_v))
				{
					return true;
				}
			}
		}
	}
		
	return false;
}
// ----------------------------------------------------------------------------------
// skin instance
// ----------------------------------------------------------------------------------
CRT_SkinInstance::CRT_SkinInstance()
{
	m_skin      = NULL;
	m_vcVec     = NULL;
	m_vcOffset	= NULL;
	m_visible   = 1.0;
	m_mtlLib	= NULL;
	m_updateNor = false;
}

CRT_SkinInstance::~CRT_SkinInstance()
{
	
}

void CRT_SkinInstance::InstanceInit() 
{
	CRT_ObjectInst::InstanceInit();
	m_skin = (CRT_Skin*)m_data;
}

void CRT_SkinInstance::InstanceExit() 
{
	// ACTOR()->Unload(m_skin);
	if(m_mtlLib) 
	{
		ACTOR()->DeleteInstance(m_mtlLib);
		m_mtlLib = NULL;
	}
}

void CRT_SkinInstance::GetRelatedRes(vector<SRelatedRes> &res)
{
	if( m_skin )
	{
		m_skin->GetRelatedRes( res );
	}	
	if( m_mtlLib == NULL )
	{
		m_mtlLib = (CRT_MtlLibInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_MTLLIB, m_skin->m_mtlLib);
		if( m_mtlLib && m_mtlLib->m_mtlLib )
		{
			m_mtlLib->m_mtlLib->GetRelatedRes( res );
		}
		ACTOR()->DeleteInstance(m_mtlLib);
		m_mtlLib = NULL;
	}else
	{
		if( m_mtlLib && m_mtlLib->m_mtlLib )
		{
			m_mtlLib->m_mtlLib->GetRelatedRes( res );
		}	
	}
    
}
void CRT_SkinInstance::ChangeMaterial(CRT_ObjectData *data)
{
    CRT_MtlLibInstance *lib = (CRT_MtlLibInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_MTLLIB,data);
    if(!lib) return;
	
    if(m_mtlLib) 
    {
        ACTOR()->DeleteInstance(m_mtlLib);
        m_mtlLib = NULL;
    }
    m_mtlLib = lib;
    OnMaterialChange();
}

void CRT_SkinInstance::OnMaterialChange()
{
    m_mtlLib->Init(m_renderDev, this);
	m_updateNor = m_mtlLib->RequestUpdateNormal();
}

bool CRT_SkinInstance::Create(CRT_ActorInstance *actor, RtgDevice *dev)
{
	m_renderDev = dev;
	m_actor = actor;
    if(m_mtlLib) 
    {
        ACTOR()->DeleteInstance(m_mtlLib);
        m_mtlLib = NULL;
    }

	m_skin->LinkActor(actor->m_actor);
	m_skin->Create(m_verVec);
    if(m_skin->m_bHelper && !m_skin->m_bHelpDynamic)
	{
		float visible;
		m_skin->GetFrame(0,m_verVec,visible,false);
	}
	
	m_mtlLib = (CRT_MtlLibInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_MTLLIB, m_skin->m_mtlLib);
	OnMaterialChange();
    return true;
}

void CRT_SkinInstance::Render(RtgDevice *dev,RTGRenderMask mask)
{
	if(m_skin->m_bHelper)
	{
		// RtgShader ro;
		dev->SetRenderState(RTGRS_FILL_MODE,RTG_FILLWIREFRAME);
		for(int i=0; i<m_skin->m_rsVec.size(); i++)
		{
			SRT_RenderSurface *rs = &m_skin->m_rsVec[i];
			
			RtgShader ro;
			ro.iNumStage = 1;
			ro.dwTextureFactor = RtgVectorToColor(RtgVertex3(0,1,0),1);
			ro.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
			ro.Shaders[0].eColorArg1 = RTGTA_TFACTOR;
			
			DWORD sVB,sIB;
			BYTE *vb,*ib;

			// update vb
			vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_verVec.size()*sizeof(RtgVertexVNT),
				FALSE,sizeof(RtgVertexVNT));
			for(int i=0; i<m_verVec.size(); i++)
			{
				memcpy(vb,&m_verVec[i],sizeof(RtgVertexVNT));
				vb += sizeof(RtgVertexVNT);
			}
			dev->m_pVB->EndUpload(sVB);
			dev->m_pVB->Bind(0, 0);

			// upload ib
			ib = (BYTE*)dev->m_pIB->BeginUpload(sIB,rs->idxVec.size()*sizeof(short),FALSE,sizeof(short));
			memcpy(ib,&rs->idxVec[0],rs->idxVec.size()*sizeof(short));
			dev->m_pIB->EndUpload(sIB);
			dev->m_pIB->Bind(0, 0);
			
			dev->SetShader(&ro);
			dev->SetVertexFormat(RTG_VNT);
			dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,m_verVec.size(),0,rs->idxVec.size()/3);
			dev->RestoreShader(&ro);
			
			/*
			for(int j=0; j<rs->idxVec.size(); j+=3)
			{
				dev->DrawTriangle(m_verVec[rs->idxVec[j]].pos,
					m_verVec[rs->idxVec[j+1]].pos,
					m_verVec[rs->idxVec[j+2]].pos);
			}
			*/
		}
		dev->SetRenderState(RTGRS_FILL_MODE,RTG_FILLSOLID);
		return;
	}

	// 1. if(bill board) set board matrix
	// 2. set vb,ib
	// 3. base shader render
	// 4. effect shader render

	RtgMatrix16 old;
	// if board,reset world matrix
	if(m_skin->m_bBoard)
	{
		dev->GetMatrix(RTGTS_WORLD,&old);
		RtgMatrix16 board;
		board.Unit();
		board = dev->m_pCamera->m_matBillboard;
		RtgVertex3 pos;
		m_skin->GetBoardPos(pos);
		
		// test code 
		pos = pos * old;
		board._30 = pos.x;
		board._31 = pos.y;
		board._32 = pos.z;
        
		// board._30 = old._30 + pos.x;
		// board._31 = old._31 + pos.y;
		// board._32 = old._32 + pos.z;
		dev->SetMatrix(RTGTS_WORLD,&board);
	}

    rtClock(PFM()->coreRenderTimer);
    m_mtlLib->Render(dev,mask);
	rtUnclock(PFM()->coreRenderTimer);
	//if(mask != RTGRM_ALL && (m_bsList[i]->IsTransparent() ^ (mask == RTGRM_BLENDOBJ_ONLY)))
	
    if(m_skin->m_bBoard)
	{
		dev->SetMatrix(RTGTS_WORLD,&old);
	}
}

void CRT_SkinInstance::RenderShadow(RtgDevice *dev , float fShadowFadeFactor)
{
	//if(colorkey)
	//{
	//	sha.dwTextureFactor = RtgVectorToColor(color,1.0);
	//	sha.Shaders[0].GetTexture() = 0;
	//	sha.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
	//	sha.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
	//	sha.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
	//	sha.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
	//	dev->SetShader(&sha);
	//}
	//else
	//{
	//	sha.dwTextureFactor = RtgVectorToColor(color,1.0);
	//	sha.Shaders[0].GetTexture() = 0;
	//	sha.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
	//	sha.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
	//	sha.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
	//	sha.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
	//	dev->SetShader(&sha);
	//}
	RenderShadowMap(dev,m_skin->m_rsVec,m_verVec);
}

/*
void CRT_SkinInstance::SetVisible(float visible)
{
	m_visible = visible;
	
    float ret = m_frameVisible * visible;
	if(Abs(m_visible - visible) > 0.01 || Abs(m_frameVisible - m_oldFrameVisible) > 0.01)
	{
		m_visible = visible;
		m_oldFrameVisible = m_frameVisible;
        for(int i=0; i<m_mtlLib->m_mtlList.size(); i++)
		{
            m_mtlLib->m_mtlList[i]->SetVisibleGlobal(ret);
		}
	}
}
*/

float CRT_SkinInstance::GetFinalVisible()
{
	return m_visible * m_actor->GetFinalVisible(); 
}

void CRT_SkinInstance::UseFrame(float frame)
{
	float visible;
	if(m_skin->m_bHelper && !m_skin->m_bHelpDynamic)
	{
		return;
	}

	rtClock(PFM()->skinTimer)
	if(m_skin->GetFrame(frame, m_verVec, visible, m_updateNor))
	{
		// m_frameVisible = visible;
		SetVisible(visible);
 	}
	rtUnclock(PFM()->skinTimer)
	
	if(!m_skin->m_bHelper)
	{
	// in editor, check every frame
#ifdef ACTOR_EDITOR
		m_updateNor = m_mtlLib->RequestUpdateNormal();
#endif
		m_mtlLib->UseFrame(frame);
	}
}

void CRT_SkinInstance::Tick(unsigned long delta)
{
	if(m_skin->m_bHelper) return;
    m_mtlLib->Tick(delta);
}

void CRT_SkinInstance::Output()
{
	m_skin->Output();
}

// for scene render
void CRT_SkinInstance::GetBoundingBox(RtgRangeAABB &box)
{
	for(int i=0; i<m_verVec.size(); i++)
	{
		box.Extend(m_verVec[i].pos);
	}
}

void CRT_SkinInstance::SetBaseColor(RtgVertex3 &color)
{	
	RtgVertex4 emi;

    for(int i=0; i<m_mtlLib->m_mtlVec.size(); i++)
	{
		emi.x = color.x;
		emi.y = color.y;
		emi.z = color.z;
		m_mtlLib->m_mtlVec[i]->SetBaseColor(emi);
	}
}

bool CRT_SkinInstance::ComputeVC(RtgMatrix16 &world,vector<DWORD> &vcVec,int offset,RtgVertex3 vSkyLight, RtgLightItem* pLights)
{
	DWORD vc;
	RtgMatrix16 nor = world;
	RtgVertex3 v,n;
	nor._30 = nor._31 = nor._32 = 0.0f;
	RtgVertex4 amb,dif,emi,spe;
	RtgVertex3 amb3,dif3,emi3,spe3;

	// default use frame 0 to compute vc 
	UseFrame(0);
	for(int i=0; i<m_verVec.size(); i++)
	{
		v = m_verVec[i].pos * world;
		n = m_verVec[i].nor * nor;
		
		amb.Set(1.0,1.0,1.0,1.0);
		dif.Set(1.0,1.0,1.0,1.0);
		emi.Set(0.0,0.0,0.0,0.0);
		
        // m_mtlLib->m_mtlList[0]->GetBaseInfo(amb,dif,emi);
		if(m_mtlLib->m_mtlVec[0]->IsKindOf(RT_RUNTIME_CLASS(CRT_MtlInstStandard)))
		{
			((CRT_MtlInstStandard*)m_mtlLib->m_mtlVec[0])->GetBaseInfo(amb,dif,emi);
		}
		amb3.x=amb.x; amb3.y=amb.y; amb3.z=amb.z;
		dif3.x=dif.x; dif3.y=dif.y; dif3.z=dif.z;
		emi3.x=emi.x; emi3.y=emi.y; emi3.z=emi.z;
		vc = RtgComputeVertexColor(v,n,amb3,dif3,emi3,vSkyLight,pLights);

		// RtgFog fog;
		// fog.Col.x = 1;
		// fog.Col.y = 1;
		// fog.Col.z = 1;
		// fog.Start = -20;
		// fog.End   = 40;
		// vc = RtgComputeFogVertexColor(v,fog);
		vcVec.push_back(vc);
	}
	return true;
}

bool CRT_SkinInstance::SetVC(vector<DWORD> &vcVec,int offset)
{
	m_vcVec = &vcVec;
	m_vcOffset = offset;
	return true;
}

/*
// -------------------------------------------------------------
// clone object
// -------------------------------------------------------------
CRT_ActorInstance *LoadActorInstance(const char *name)
{
	if(name == NULL) return NULL;
	CRT_ActorInstance *actor;
	CRT_Actor *core;

	core = (CRT_Actor*)ActorPool()->Load(GetActorFileName(name),RT_RUNTIME_CLASS(CRT_Actor));
	if(core) {
		actor = RT_NEW CRT_ActorInstance;
		actor->SetCoreObject(core);
		// actor->Create(g_pDevice);
		actor->SetValid(true,true);
		return actor;
	}
	return NULL;
}

CRT_SkinInstance *LoadSkinInstance(const char *name)
{
	if(name == NULL) return NULL;
	CRT_SkinInstance *skin;
	CRT_Skin *core;

	core = (CRT_Skin*)ActorPool()->Load(GetSkinFileName(name),RT_RUNTIME_CLASS(CRT_Skin));
	if(core) {
		skin = RT_NEW CRT_SkinInstance;
		skin->SetCoreObject(core);
        return skin;
	}
	RtCoreLog().Info("LoadSkinInstance: can't find skin %s\n",name);
	return NULL;
}

CRT_Effect *LoadEffect(const char *name)
{
	if(name == NULL) return NULL;
	
	CRT_Effect *effect;
	effect = (CRT_Effect*)ActorPool()->Load(GetEffectFileName(name),RT_RUNTIME_CLASS(CRT_Effect));
	return effect;
}

CRT_PoolObject *ActorPrecache(const char *name,const char *type)
{
	CRT_PoolObject *ob = NULL;
	if(strcmp(type,"actor")==0)
	{
		ob = ActorPool()->Load(GetActorFileName(name),RT_RUNTIME_CLASS(CRT_Actor));
	}
	else if(strcmp(type,"skin")==0)
	{
		ob = ActorPool()->Load(GetSkinFileName(name),RT_RUNTIME_CLASS(CRT_Skin));
	}
	else if(strcmp(type,"mtllib")==0)
	{
		ob = ActorPool()->Load(GetMtlLibFileName(name),RT_RUNTIME_CLASS(CRT_MaterialLib));
	}
	else if(strcmp(type,"effect")==0)
	{
		ob = ActorPool()->Load(GetEffectFileName(name),RT_RUNTIME_CLASS(CRT_Effect));
	}
	if(ob)
		ob->Precache();
	return ob;
}

void ActorUncache(CRT_PoolObject *ob)
{
	ob->Uncache();
	ActorPool()->Unload(ob);
}
*/
// --------------------------------------------------------------------------------------------
//scene interface
//--------------------------------------------------------------------------------------------
RtsSceneUtil *baseScene = NULL;

RtsSceneUtil *ActorGetScene()
{
	return baseScene;
}

void ActorSetScene(RtsSceneUtil *scene)
{
	baseScene = scene;
}


// --------------------------------------------------------------------------------------------
// MaterialLib
// --------------------------------------------------------------------------------------------
CRT_MtlLibInstance::CRT_MtlLibInstance() {}
CRT_MtlLibInstance::~CRT_MtlLibInstance() {}

void CRT_MtlLibInstance::InstanceInit() 
{
	CRT_ObjectInst::InstanceInit();
	m_mtlLib = (CRT_MaterialLib*)m_data;
}

void CRT_MtlLibInstance::InstanceExit()
{
	for(int i=0; i<m_mtlVec.size(); i++)
		ACTOR()->DeleteInstance(m_mtlVec[i]);
	m_mtlVec.clear();
}

bool CRT_MtlLibInstance::RequestTick()
{
    return true;
}

void CRT_MtlLibInstance::Tick(unsigned long delta)
{
    for(int i=0; i<m_mtlVec.size(); i++)
    {
        if(m_mtlVec[i]->RequestTick())
            m_mtlVec[i]->Tick(delta);
    }
}

bool CRT_MtlLibInstance::RequestUseFrame()
{
    return true;
}

void CRT_MtlLibInstance::UseFrame(unsigned long frame)
{
	 for(int i=0; i<m_mtlVec.size(); i++)
	 {
		 if(m_mtlVec[i]->RequestUseFrame())
			 m_mtlVec[i]->UseFrame(frame);
	 }
}

bool CRT_MtlLibInstance::RequestUpdateNormal()
{
	for(int i=0; i<m_mtlVec.size(); i++)
	{
		if(m_mtlVec[i]->RequestUpdateNormal())
			return true;
	}
	return false;
}

CRT_MtlInstance *CRT_MtlLibInstance::GetMaterial(const char *name)
{
    for(int i=0; i<m_mtlVec.size(); i++)
        if(m_mtlVec[i]->GetName() == name) return m_mtlVec[i];

    return NULL;
}

CRT_MtlInstance *CRT_MtlLibInstance::GetMaterial(int idx)
{
	if(idx < 0 || idx >= m_mtlVec.size()) return NULL;
	return m_mtlVec[idx];  
}

bool CRT_MtlLibInstance::Init(RtgDevice *dev,CRT_SkinInstance *skin)
{
	for(int i=0; i<m_mtlVec.size(); i++)
	{
		ACTOR()->DeleteInstance(m_mtlVec[i]);
		// if(m_mtlVec[i]->IsActive())
           // m_mtlVec[i]->Exit();
	}
	m_mtlVec.clear();
    for(int i=0; i<skin->m_skin->m_rsVec.size(); i++)
    {
		CRT_ObjectData *data = m_mtlLib->GetMtlByName(skin->m_skin->m_rsVec[i].material.c_str());
		if(data)
		{
			CRT_MtlInstance *mtl = (CRT_MtlInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_MTL,data);
	        if(mtl)
			{
				mtl->SetActive(true);
				mtl->Init(dev,skin,&skin->m_skin->m_rsVec[i]);
				m_mtlVec.push_back(mtl);
			}
		}
    }    
	m_skin = skin;
    return true;
}

void CRT_MtlLibInstance::Render(RtgDevice *dev,RTGRenderMask mask)
{
	for(int i=0; i<m_mtlVec.size(); i++)
	{
		if(m_mtlVec[i]->IsActive() && 
			(mask==RTGRM_ALL || (m_mtlVec[i]->IsTransparent() ^ (mask==RTGRM_NOBLENDOBJ_ONLY))))
		{
            m_mtlVec[i]->Render(dev,mask);
		}
	}
}

void CRT_MtlLibInstance::RenderShadow(RtgDevice *dev ,float fShadowFadeFactor /*= 0.5f*/)
{
	DWORD sVB,sIB;
	BYTE *vb,*ib;
	unsigned int i;

	if(m_skin->m_verVec.size() == 0) 
	{
		return;
	}	
	// update vb
	vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_skin->m_verVec.size()*sizeof(RtgVertexVNT),FALSE,sizeof(RtgVertexVNT));

	for(i=0; i<m_skin->m_verVec.size(); i++)
	{
		memcpy(vb,&m_skin->m_verVec[i].pos,sizeof(RtgVertexVNT));
		vb += sizeof(RtgVertexVNT);
	}
	dev->m_pVB->EndUpload(sVB);
	dev->SetVertexFormat(RTG_VNT);

	int alphaMode,texId;
	bool bshadow;
	SRT_RenderSurface *rs;
	for(int i=0; i<m_mtlVec.size(); i++)
	{
		if(!m_mtlVec[i]->IsActive()) 
		{
			continue;
		}
		if(!m_mtlVec[i]->GetGeometry(alphaMode,texId,&rs,bshadow)) 
		{
			continue;
		}
		if(rs->idxVec.size() == 0) 
		{
			continue;
		}
		//if(!bshadow)continue;

		RtgShader sd;		
		RtgVertex3 color( fShadowFadeFactor, fShadowFadeFactor, fShadowFadeFactor );
		switch(alphaMode)
		{
			case ALPHA_MASK:
			case ALPHA_NULL:
				sd.iNumStage = 1;
				sd.dwTextureFactor = RtgVectorToColor(color,1);
				sd.Shaders[0].SetTexture(0);
				sd.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
				sd.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
				sd.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
				if(alphaMode == ALPHA_MASK)
				{
					sd.Shaders[0].SetTexture(texId);
					sd.bColorKey = true;
				}
				break;

			case ALPHA_SUB:
			case ALPHA_ADD:
				sd.iNumStage = 1;
				sd.dwTextureFactor = RtgVectorToColor(color,1);
				sd.Shaders[0].SetTexture(texId);
				sd.Shaders[0].eColorOp   = RTGTOP_MODULATE;
                sd.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
				sd.Shaders[0].eColorArg2 = RTGTA_TEXTURE;
				sd.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
				sd.bEnableBlendFunc = true;
				sd.eBlendSrcFunc = RTGBLEND_ONE;
				sd.eBlendDstFunc = RTGBLEND_ONE;
				continue;
				break;

			case ALPHA_BLEND:
				sd.iNumStage = 1;
				sd.dwTextureFactor = RtgVectorToColor(color,1);
				sd.Shaders[0].SetTexture(texId);
				sd.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
				sd.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
				sd.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
				sd.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
				sd.bEnableBlendFunc = true;
				sd.eBlendSrcFunc = RTGBLEND_SRCALPHA;
				sd.eBlendDstFunc = RTGBLEND_ONE;
				break;
		}
		sd.bTwoSide = true;
		dev->SetShader(&sd);		
		// upload ib
		ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, rs->idxVec.size()*sizeof(short),
			FALSE, sizeof(short));

		memcpy(ib,&rs->idxVec[0],rs->idxVec.size()*sizeof(short));
		dev->m_pIB->EndUpload(sIB);

		// draw
		dev->m_pVB->Bind(0, 0);
		dev->m_pIB->Bind(0, 0);
		dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,
			0,
			m_skin->m_verVec.size(),
			0,
			rs->idxVec.size()/3);
	}
}

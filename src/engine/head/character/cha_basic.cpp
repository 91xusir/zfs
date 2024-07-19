
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_basic.cpp
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#include "character/actor.h"
#include "cha_anim.h"


RT_IMPLEMENT_DYNAMIC(CRT_PoolObject,    RtObject,       NULL,   "")
RT_IMPLEMENT_DYNAMIC(CRT_Effect,        CRT_PoolObject, NULL,   "")
RT_IMPLEMENT_DYNAMIC(CRT_EffectBase,    CRT_Effect,     NULL,   "")
RT_IMPLEMENT_DYNAMIC(CRT_Skin,          CRT_PoolObject, NULL,   "")
RT_IMPLEMENT_DYNAMIC(CRT_Material,      CRT_PoolObject, NULL,   "")
RT_IMPLEMENT_DYNCREATE(CRT_MaterialLib, CRT_PoolObject, NULL,   "")
RT_IMPLEMENT_DYNCREATE(CRT_VaSkin,      CRT_Skin,       NULL,   "")
RT_IMPLEMENT_DYNCREATE(CRT_SkelSkin,    CRT_Skin,       NULL,   "")
RT_IMPLEMENT_DYNCREATE(CRT_Actor,       CRT_PoolObject, NULL,   "")

namespace {

    inline bool IsDump(const RtgVertex2 &a, const RtgVertex2 &b)
    {
	    return Abs(a.x-b.x)<0.01f && Abs(a.y-b.y)<0.01f;
    }

    inline bool IsDump(const RtgVertex3 &a, const RtgVertex3 &b)
    {
	    return Abs(a.x-b.x)<0.01f && Abs(a.y-b.y)<0.01f && Abs(a.z-b.z)<0.01f;
    }
}

void SRT_Bone::UseFrame(float frame, CRT_ActorInstance* _actor)
{
    RTASSERT(_actor);

    size_t childNum = childList.size();
    RtgMatrix12 parent;
    parent.Unit(); 

    for (size_t i = 0; i < childNum; ++i)
        if (childList[i] < _actor->m_bones.size())
            _actor->m_bones[childList[i]].UseFrame(parent, frame, _actor);
}

void SRT_Bone::UseFrame(RtgMatrix12& parent, float frame, CRT_ActorInstance* _actor)
{
    RTASSERT(_actor);

    if (frame >= aryTrans.size())
        return;

    RtgVertex3* v = NULL;
	int _frame = (int)frame;

    if (!AbsMode && !ExportTranslate)
		v = &aryTrans[0];
	else
		v = &aryTrans[_frame];

	RtgVertex4* r = &aryRotas[_frame];
	rtgQuatToM9(skinMat.m_m, &r->x, r->w);
	skinMat._30 = v->x;
	skinMat._31 = v->y;
	skinMat._32 = v->z;

	if (!AbsMode)
		skinMat = skinMat * parent;
	normMat = skinMat;
	normMat._30 = normMat._31 = normMat._32 = 0.0f;

	size_t childNum = childList.size();
	for (size_t i = 0; i < childNum; ++i)
        if (childList[i] < _actor->m_bones.size())
		    _actor->m_bones[childList[i]].UseFrame(skinMat, frame, _actor);
}

void SRT_Pose::Active(CRT_ActorInstance* _ParentAct)
{
    RTASSERT(_ParentAct);
    pParentAct = _ParentAct;
    vecTagActs.clear();

    for (size_t i = 0; i < EventList.size(); ++i)
    {
        SRT_PoseEvent& e = EventList[i];

        if (e.Action == "animation")
        {
            CRT_ActorInstance* act = RtcGetActorManager()->CreateActor(e.Param.c_str(), false);

            if (act)
            {
                if (e.Param2.empty())
                    e.Param2 = "root";
                act->SetTagPoseNotify();
                act->Enable(false, false);
                act->m_bRenderByParent = true;
                act->LinkParent(pParentAct, e.Param2.c_str());
                e.nTagActIndex = vecTagActs.size();
                vecTagActs.push_back(act);
            }
            else
            {
                e.nTagActIndex = -1;
            }
        }
    }
}

void SRT_Pose::UnActive()
{
    for (size_t i = 0; i < vecTagActs.size(); ++i)
        RtcGetActorManager()->ReleaseActor(vecTagActs[i]);
    vecTagActs.clear();
}

bool SRT_Pose::ResourceReady() const
{
    for (size_t i = 0; i < vecTagActs.size(); ++i)
        if (!vecTagActs[i]->ResourceReady())
            return false;
    return true;
}

bool RtcSkinSubset::Create()
{
    RTASSERT(m_pib == NULL);
    void* ib = NULL;

    RtgSetCreateBufferDebugInfo();
    m_pib = RtGetRender()->GetBuffMgr()->CreateBuffer(
        faceList.buffer_size(),
        D3DFMT_INDEX16,
        0,
        D3DPOOL_MANAGED,
        faceList.pointer());
        
    if (!m_pib)
        return false;
    return true;
}

void RtcSkinSubset::Destroy()
{
    RtgReleaseBuffer(m_pib);
}

long CRT_Skin::Serialize(RtArchive& Ar)
{
    SetState(Object_State_Loading);

    bool no_use0 = true;
    bool no_use1 = true;

    char szVersion[] = "tooth0808";

    if (Ar.IsLoading())
    {
        int pos = Ar.Tell();
        memset (m_szSkinVersion, 0, 10);
        Ar.Serialize(m_szSkinVersion, 9);

        if (strcmp(m_szSkinVersion, szVersion) == 0)
        {
            Ar << m_bBoard << m_bStatic << no_use0 << m_mtlLib << m_name << m_rsList << no_use1;
        }
        else
        {
            Ar.Seek(pos, RtArchive::SeekBegin);
            Ar << m_name << m_bBoard << m_bStatic << m_rsList << m_mtlLib;
        }
    }
    else
    {
        Ar.Serialize(szVersion, 9);
        Ar << m_bBoard << m_bStatic << no_use0 << m_mtlLib << m_name << m_rsList << no_use1;
    }

    BEGIN_VERSION_CONVERT(0)
    Ar << m_bDynShadow;
    BEGIN_VERSION_CONVERT(1)
    END_VERSION_CONVERT
    END_VERSION_CONVERT
    return Ar.IsError() ? 0 : 1;
}

void CRT_Skin::OnLoadFinish()
{
    if (ReourceReady())
        return;

    for (size_t i = 0; i < m_rsList.size(); ++i)
    {
        if (m_rsList[i].faceList.size() > 0)
        {
            m_rsList[i]._Id = m_mtlLib->GetMaterial_Id(m_rsList[i].Material.c_str());
            CRT_Material* mtl = m_mtlLib->GetMaterial(m_rsList[i]._Id);

            if (mtl->IsKindOf(RT_RUNTIME_CLASS(CRT_MtlMu)))
            {
                CRT_MtlMu* mtlmu = (CRT_MtlMu*)mtl;
                if (mtlmu->m_texBase == "USE BASE")
                    mtlmu->m_texBase = m_rsList[i].Texture;
            }
        }
        else
        {
            RtCoreLog().Error("actor (id : %u) not has faces\n", i);
        }
    }

    m_mtlLib->OnLoadFinish();
}

bool CRT_Skin::Create(CRT_Actor* act)
{
    RTASSERT(GetState() < Object_State_Creating);
    SetState(Object_State_Creating);

    for (size_t i = 0; i < m_rsList.size(); ++i)
        if (m_rsList[i].faceList.size() > 0)
            m_rsList[i].Create();

    m_mtlLib->Create();
    SetState(Object_State_Ready);

    return true;
}

void CRT_Skin::Destroy()
{
    for (size_t i = 0; i < m_rsList.size(); ++i)
        m_rsList[i].Destroy();
    m_rsList.clear();
    RtcGetActorManager()->ReleaseMtl(m_mtlLib);
    m_mtlLib = NULL;
}

bool CRT_VaSkin::OnCreateInstance(CRT_SkinInstance* skin)
{
    RTASSERT(skin);
    if (!m_bStatic)
        (*skin->Get_VertexArray()) = m_aryVertex;
    return true;
}

bool CRT_VaSkin::Create(CRT_Actor* act)
{
    if (!CRT_Skin::Create(act))
        return true;

    if (m_bStatic)
    {
        RTASSERT(m_pvb == NULL);
        void* vb = NULL;

        RtgSetCreateBufferDebugInfo();
        m_pvb = RtGetRender()->GetBuffMgr()->CreateBuffer(
            m_aryVertex.buffer_size(),
            SRT_StaticVtx::fvf,
            0,
            D3DPOOL_MANAGED,
            m_aryVertex.pointer()
            );

        if (!m_pvb)
            return false;
    }

    SetState(Object_State_Ready);
    
    return true;
}

void CRT_VaSkin::Destroy()
{
    CRT_Skin::Destroy();
    RtgReleaseBuffer(m_pvb);
}

RtgBufferItem* CRT_VaSkin::RequestVB(CRT_SkinInstance* skin, DWORD* _Offset, DWORD* _Size)
{
    RTASSERT(skin);

    (*_Offset) = 0;

    if (!m_bStatic)
    {
        RtgShareBufferItem vbItem;
        (*_Size) = (DWORD)skin->Get_VertexArray()->buffer_size();
        void* vb = RtGetRender()->GetBuffMgr()->Lock_Shared_vb((*_Size), &vbItem);
        memcpy(vb, skin->Get_VertexArray()->pointer(), (*_Size));
        RtGetRender()->GetBuffMgr()->Unlock(&vbItem);
        (*_Offset) = vbItem.Offset;
        return vbItem.pShareBuf->bufItem;
        /*RtgBufferItem* pvb =  RtGetRender()->GetBuffMgr()->GetShared_vb();
        RtGetRender()->GetBuffMgr()->UpdateBuffer(
            pvb, 
            skin->Get_VertexArray()->pointer(),
            skin->Get_VertexArray()->buffer_size(), 
            0,
            true);
        return pvb;*/
    }
    else
    {
        (*_Size) = m_pvb->dwSize;
    }

    return m_pvb;
}

bool CRT_VaSkin::UseFrame(CRT_SkinInstance* skin, float frame, float* visible, bool bUpdateNom)
{
    RTASSERT(skin);

    if (m_bStatic) 
        return true;

    int _frame = (int)(frame + 0.5f);
    if (_frame >= m_frmList.size())
        return false;

    SRT_VaFrame& vframe = m_frmList[_frame];

    if (m_bBoard) 
        m_boardPos = m_boardPosList[_frame];

    (*visible) = vframe.visible;

    for (size_t i = 0; i < vframe.verList.size(); ++i)
    {
        SRT_StaticVtx& v = skin->m_aryVertex[i];
        vframe.verList[i].GetValue(v.pos); 
        if (m_bNor)
            vframe.norList[i].GetValue(v.nor);
        if (m_bUV)
            vframe.uvList[i].GetValue(v.uv);
    }

    return true;
}

void CRT_VaSkin::Output()
{
}

long CRT_VaSkin::PushVertex( const SRT_StaticVtx& v )
{
#ifdef MAX_PLUGIN_EXPORTS
	unsigned int i;
	for(i=0; i<m_aryVertex.size(); i++)
	{
		if(IsDump(m_aryVertex[i].pos,v.pos) && IsDump(m_aryVertex[i].nor,v.nor)
			&& IsDump(m_aryVertex[i].uv,v.uv))
			return i;
	}
	m_aryVertex.push_back(v);
	return i;
#else
	return 0;
#endif
}

bool CRT_SkelSkin::OnCreateInstance(CRT_SkinInstance* skin)
{
    RTASSERT(skin);

    skin->m_aryVertex.resize(m_verVertex.size());
    
    for (size_t i = 0; i < m_verVertex.size(); ++i)
    {
        skin->m_aryVertex[i].nor = m_verVertex[i].nor;
        skin->m_aryVertex[i].uv  = m_verVertex[i].uv;
    }
    
    Update(skin);
    
    return true;
}

bool CRT_SkelSkin::Create(CRT_Actor* act)
{
    RTASSERT(!m_bStatic);

    if (!CRT_Skin::Create(act))
        return true;

    SetState(Object_State_Ready);

    return true;
}

void CRT_SkelSkin::Destroy()
{
    CRT_Skin::Destroy();
}

RtgBufferItem* CRT_SkelSkin::RequestVB(CRT_SkinInstance* skin, DWORD* _Offset, DWORD* _Size)
{
    RTASSERT(skin);

    RtgShareBufferItem vbItem;

    (*_Size) = (DWORD)skin->Get_VertexArray()->buffer_size();
    void* vb = RtGetRender()->GetBuffMgr()->Lock_Shared_vb((*_Size), &vbItem);
    memcpy(vb, skin->Get_VertexArray()->pointer(), (*_Size));
    RtGetRender()->GetBuffMgr()->Unlock(&vbItem);
    (*_Offset) = vbItem.Offset;

    return vbItem.pShareBuf->bufItem;

    /*RtgBufferItem* pvb =  RtGetRender()->GetBuffMgr()->GetShared_vb();
    RtGetRender()->GetBuffMgr()->UpdateBuffer(
        pvb, 
        skin->Get_VertexArray()->pointer(),
        skin->Get_VertexArray()->buffer_size(), 
        0,
        true);
    return pvb;*/
}

bool CRT_SkelSkin::UseFrame(CRT_SkinInstance* skin, float frame, float* visible, bool bUpdateNom)
{
    RTASSERT(skin);
    (*visible) = 1.f;
    if (bUpdateNom)
        UpdateWithNormal(skin);
    else
        Update(skin);
    return true;
}

void CRT_SkelSkin::Update(CRT_SkinInstance* skin)
{
    RTASSERT(skin);
    RTASSERT(skin->m_actor);

	size_t verNum = m_verVertex.size();

	for(size_t i = 0; i < verNum; ++i)
	{
        SRT_DynamicVtx& d = m_verVertex[i];
        SRT_StaticVtx&  s = skin->m_aryVertex[i];
        RtgVertex3 t;
		size_t bonNum = d.boneList.size();

        s.pos.Set(0.f, 0.f, 0.f);

        if (bonNum == 1) 
        {
            RtgMatrix12* skinMat = skin->m_actor->GetBoneMat(d.boneList[0].bondIndex);
            if (skinMat)
                rtgV3MultiplyM12(s.pos.m_v, d.boneList[0].PosOffset.m_v, skinMat->m_m);
		}
		else
		{
			for (size_t j = 0; j < bonNum; ++j) 
            {
                RtgMatrix12* skinMat = skin->m_actor->GetBoneMat(d.boneList[j].bondIndex);
                if (!skinMat)
                    continue;
				rtgV3MultiplyM12(t.m_v, d.boneList[j].PosOffset.m_v, skinMat->m_m);
				s.pos = s.pos + d.boneList[j].Weight * t;
			}
		}
	}
}

void CRT_SkelSkin::UpdateWithNormal(CRT_SkinInstance* skin)
{
    size_t verNum = m_verVertex.size();

    for(size_t i = 0; i < verNum; ++i)
    {
        SRT_DynamicVtx& d = m_verVertex[i];
        SRT_StaticVtx&  s = skin->m_aryVertex[i];
        RtgVertex3 t;
        size_t bonNum = d.boneList.size();

        s.pos.Set(0.f, 0.f, 0.f);
        s.nor.Set(0.f, 0.f, 0.f);

        if (bonNum == 1) 
        {
            RtgMatrix12* skinMat = skin->m_actor->GetBoneMat(d.boneList[0].bondIndex);
            if (skinMat)
                rtgV3MultiplyM12(s.pos.m_v, d.boneList[0].PosOffset.m_v, skinMat->m_m);

            RtgMatrix12* normMat = skin->m_actor->GetBoneNormalMat(d.boneList[0].bondIndex);
            if (normMat)
                rtgV3MultiplyM12(s.nor.m_v, d.boneList[0].NorOffset, normMat->m_m);
 
        }
        else
        {
            for (size_t j = 0; j < bonNum; ++j) 
            {
                RtgMatrix12* skinMat = skin->m_actor->GetBoneMat(d.boneList[j].bondIndex);
                if (skinMat)
                {
                    rtgV3MultiplyM12(t.m_v, d.boneList[j].PosOffset.m_v, skinMat->m_m);
                    s.pos = s.pos + d.boneList[j].Weight * t;
                }
                RtgMatrix12* normMat = skin->m_actor->GetBoneNormalMat(d.boneList[j].bondIndex);
                if (normMat)
                {
                    rtgV3MultiplyM12(t.m_v, d.boneList[j].NorOffset, normMat->m_m);
                    s.nor = s.nor + d.boneList[j].Weight * t;
                }
            }
        }
    }
}

void CRT_SkelSkin::Output()
{
    RtCoreLog().Info("SkelSkin Info\n");
	RtCoreLog().Info("name = %s,rs = %d,ver = %d\n",m_name.c_str(),m_rsList.size(),
		m_verVertex.size());

	for (size_t i = 0; i < m_verVertex.size(); ++i)
		m_verVertex[i].Output();
}

#ifdef MAX_PLUGIN_EXPORTS
// for max export
long CRT_SkelSkin::PushVertex( const SRT_DynamicVtx& v )
{
	unsigned int i;
	for(i=0; i<m_verVertex.size(); i++)
	{
		if(IsDump(m_verVertex[i].pos,v.pos) && IsDump(m_verVertex[i].nor,v.nor)
			&& IsDump(m_verVertex[i].uv,v.uv))
			return i;
	}
	m_verVertex.push_back(v);
	return i;
}
#endif

//
//bool CRT_SkelSkin::LinkActor(CRT_Actor *actor)
//{
//    if (!CRT_Skin::LinkActor(actor))
//        return false;
//
//    for (size_t i = 0; i < m_verVertex.size(); ++i)
//    {
//        for (vector<SRT_BoneRef>::iterator it = m_verVertex[i].boneList.begin(); 
//            it != m_verVertex[i].boneList.end();
//            )
//        {
//            SRT_BoneRef *ref = &(*it);
//            ref->bone = actor->GetBone(ref->bondIndex);
//
//            if (!ref->bone) 
//            {
//                RtCoreLog().Error("%s: skin = %s,can't find bone [%d]\n",__FUNCTION__,m_poTag.c_str(),ref->bondIndex);
//                it = m_verVertex[i].boneList.erase(it);
//                continue;
//            }
//
//            ++it;
//        }
//    }
//
//    return true;
//}

CRT_Actor::CRT_Actor()
    : m_bUseLight(true),
      m_bUseVC(true),
      m_bZTest(true),
      m_animSpeed(1.f),
      m_bDynamicShadow(true)
{
	CM_MEMPROTECTOR(m_szVersion, 10)
    SetState(Object_State_New);
}

CRT_Actor::CRT_Actor(const CRT_Actor& _actor)
{
    m_animSpeed = _actor.m_animSpeed;
    m_poseMap = _actor.m_poseMap;
    m_frameNum = _actor.m_frameNum;
    m_bUseLight = _actor.m_bUseLight;
    m_bUseVC = _actor.m_bUseVC;
    m_bZTest = _actor.m_bZTest;
    m_bDynamicShadow = _actor.m_bDynamicShadow;
    m_animType = _actor.m_animType;
    m_bones = _actor.m_bones;
    m_skinList = _actor.m_skinList;
    m_eftList = _actor.m_eftList;
    m_boundBoxList = _actor.m_boundBoxList;
    CM_MEMPROTECTOR(m_szVersion, 10);
    SetState(Object_State_New);

}

CRT_Actor::~CRT_Actor() 
{
	CM_MEMUNPROTECTOR(m_szVersion)

    RtcAcotrManager* pMgr = RtcGetActorManager();

	for (size_t i = 0; i < m_skinList.size(); ++i)
        pMgr->ReleasePoolObject(m_skinList[i]);

    for (size_t i = 0; i < m_eftList.size(); ++i)
		DEL_ONE(m_eftList[i]);
}

long CRT_Actor::Serialize(RtArchive& Ar)
{
	bool nouse0 = false;
	bool nouse1 = false;

	char szVersion[] = "tooth0708";
	
    if (Ar.IsLoading())
	{
		int pos = Ar.Tell();
		memset (m_szVersion, 0, 10);
		Ar.Serialize(m_szVersion, 9);

		if (strcmp(m_szVersion, szVersion) == 0)
		{
			Ar<<m_animSpeed
              <<m_animType
			  <<m_bDynamicShadow
              <<m_bones
              <<m_boundBoxList
              <<nouse0
              <<m_bUseVC
              <<m_bUseLight
              <<m_bZTest
			  <<m_eftList
              <<m_frameNum
              <<m_poseMap
              <<nouse1
              <<m_skinList;
		}
		else
		{
			Ar.Seek(pos, RtArchive::SeekBegin);
			Ar<<m_animType
              <<m_frameNum
			  <<m_bUseLight
              <<m_bUseVC
              <<m_bZTest
              <<m_bDynamicShadow
              <<m_animSpeed
			  <<m_poseMap
              <<m_bones
              <<m_skinList
              <<m_eftList
              <<m_boundBoxList;
		}
	}
	else
	{
		Ar.Serialize(szVersion, 9);
		Ar<<m_animSpeed
            <<m_animType
		    <<m_bDynamicShadow
            <<m_bones
            <<m_boundBoxList
            <<nouse0
            <<m_bUseVC
            <<m_bUseLight
            <<m_bZTest
		    <<m_eftList
            <<m_frameNum
            <<m_poseMap
            <<nouse1
            <<m_skinList;
	}

    BEGIN_VERSION_CONVERT(1)
    END_VERSION_CONVERT

	if (Ar.IsLoading())
	{
		for(int i = 0; i < m_skinList.size(); ++i)
        {
			m_skinList[i]->m_poType = Pool_Type_Memory;
            m_skinList[i]->m_poRef = 1;
        }

#if defined (_MT) && defined (_DLL) 

        boneArray_t _temp = m_bones;
        m_bones.clear();

        for (size_t i = 0; i < _temp.size(); ++i)
        {
            size_t j = 0;

            while (j < m_bones.size())
            {
                if (m_bones[j].Name == _temp[i].Name)
                    break;
                ++j;
            }

            if (j == m_bones.size())
                m_bones.push_back(_temp[i]);
        }

#endif

		RebuildBoneMap();
	}

	return Ar.IsError() ? 0 : 1;
}

void CRT_Actor::OnLoadFinish()
{
    if (GetState() >= Object_State_Creating)
        return;

    for (size_t i = 0; i < m_skinList.size(); ++i)
        m_skinList[i]->OnLoadFinish();

    for (size_t i = 0; i < m_bones.size(); ++i)
    {
        SRT_Bone* _bone = &m_bones[i];

        if (_bone->ParentName == "NULL") 
            continue;

        SRT_Bone* _parent = GetBone(_bone->ParentName.c_str());

        if (_parent)
            _parent->childList.push_back(i);
        else
            RtCoreLog().Error("%s: [%s] can't get parent bone [%s]\n", __FUNCTION__,m_poTag.c_str(), _bone->ParentName.c_str());
    }
}

bool CRT_Actor::RequestCreate()
{
    if (GetState() >= Object_State_Creating)
        return true;

    SetState(Object_State_Creating);

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        RTASSERT(m_skinList[i]->GetState() != Object_State_Ready);
        m_skinList[i]->Create(this);
    }

    SetState(Object_State_Ready);

    return true;
}

//bool CRT_Actor::UseFrame(long frame)
//{
//	if (m_frameNum == 1 && frame != 0)
//        return false;
//
//	if(m_boneList.size() == 0) 
//        return false;
//
//    if (frame < 0 || frame >= m_frameNum)
//        return false;
//
//    m_boneList[0].UseFrame(frame);
//
//	return true;
//}

void CRT_Actor::Output()
{

}

bool CRT_Actor::MergeEvent(CRT_Actor* act)
{
	TPoseMap::iterator it1;
	for(it1=m_poseMap.begin(); it1!=m_poseMap.end(); it1++)
	{
		string name = it1->first;
		TPoseMap::iterator it2;
		it2 = act->m_poseMap.find(name);
		if(it2 != act->m_poseMap.end())
		{
			vector<SRT_PoseEvent> eventVec = ((*it2).second).EventList;
			for(int i=0; i<eventVec.size(); i++)
			{
				float per = 0;
				float total = (it2->second).EndFrm - (it2->second).StartFrm;
				if(total != 0)
				{
					per = (eventVec[i].KeyFrame - (it2->second).StartFrm) / total;
				}
				eventVec[i].KeyFrame = (it1->second).StartFrm + per*((it1->second).EndFrm - (it1->second).StartFrm); 
			}
			(it1->second).EventList = eventVec;
		}
	}
	return true;
}


bool CRT_Actor::Merge2(CRT_Actor* act)
{
    if (!act)
        return false;

    size_t sizeOld = m_bones.size();

    for (size_t i = 0; i < act->m_bones.size(); ++i)
    {
        size_t j = 0;

        while (j < sizeOld)
        {
            if (m_bones[j].Name == act->m_bones[i].Name)
                break;
            ++j;
        }   

        if (j == sizeOld)
            m_bones.push_back(act->m_bones[i]);
    }

    for (size_t i = 0; i < m_bones.size(); ++i)
        m_bones[i].childList.clear();

    m_boneMap.clear();
    for (int i = (int)m_bones.size() - 1; i >= 0; --i)
        m_boneMap[m_bones[i].Name] = i;  

    for (size_t i = 0; i < m_bones.size(); ++i)
    {
        SRT_Bone* _bone = &m_bones[i];

        if (_bone->ParentName == "NULL") 
            continue;

        SRT_Bone* _parent = GetBone(_bone->ParentName.c_str());

        if (_parent)
            _parent->childList.push_back(i);
        else
            RtCoreLog().Error("%s: [%s] can't get parent bone [%s]\n", __FUNCTION__, m_poTag.c_str(), _bone->ParentName.c_str());
    }

    for (size_t i = 0; i < act->m_boundBoxList.size(); ++i)
        m_boundBoxList.push_back(act->m_boundBoxList[i]);

    for (TPoseMap::iterator it = act->m_poseMap.begin();
         it != act->m_poseMap.end(); ++it)
    {
        SRT_Pose *pose = &(*it).second;
        TPoseMap::iterator itfind = m_poseMap.find(pose->Name);
        
        if (itfind == m_poseMap.end())
            m_poseMap[pose->Name] = *pose;
        else
        {
            itfind->second.StartFrm = min(pose->StartFrm, itfind->second.StartFrm);
            itfind->second.EndFrm = max(pose->EndFrm, itfind->second.EndFrm);
        }
    }

    m_frameNum = max(m_frameNum, act->m_frameNum);

    return true;
}


// 将第二个文件整体合并入第一个文件
bool CRT_Actor::Merge(CRT_Actor *act)
{
	// merge frame
	for(int i=0; i<m_bones.size(); i++)
	{
		SRT_Bone *b1 = &m_bones[i];
		SRT_Bone *b2 = act->GetBone(b1->Name.c_str());
		
        if(!b2) 
			continue;

        b1->aryRotas.merge(b2->aryRotas);
        b1->aryTrans.merge(b2->aryTrans);

		for(int j=0; j<b2->ribbonFrame.size(); j++)
			b1->ribbonFrame.push_back(b2->ribbonFrame[j]);
	}

	// merge bounding box
	for(int i=0; i<act->m_boundBoxList.size(); i++)
	{
		m_boundBoxList.push_back(act->m_boundBoxList[i]);
	}
	
	// merge pose
	TPoseMap::iterator it;
	for(it=act->m_poseMap.begin();it!=act->m_poseMap.end();it++)
	{
		SRT_Pose *pose = &(*it).second;
		pose->StartFrm += m_frameNum;
		pose->EndFrm += m_frameNum;
		for(int i=0;i<pose->EventList.size();i++)
		{
			pose->EventList[i].KeyFrame += m_frameNum;
		}
		m_poseMap[pose->Name] = *pose;
	}
	m_frameNum += act->m_frameNum;

	return true;
}

CRT_MaterialLib::CRT_MaterialLib()
{

}

CRT_MaterialLib::~CRT_MaterialLib()
{
    for (size_t i = 0; i < m_mtlList.size(); ++i)
        RtcGetActorManager()->ReleasePoolObject(m_mtlList[i]);
    m_mtlList.clear();
}

void CRT_MaterialLib::OnLoadFinish()
{
    for (size_t i = 0; i < m_mtlList.size(); ++i)
        m_mtlList[i]->OnLoadFinish();
}

bool CRT_MaterialLib::Create()
{
    for (size_t i = 0; i < m_mtlList.size(); ++i)
        m_mtlList[i]->Create();
    return true;
}

CRT_PoolObject* CRT_MaterialLib::NewObject()
{ 
    CRT_MaterialLib* lib = (CRT_MaterialLib*)RtcGetActorManager()->NewObject(RT_RUNTIME_CLASS(CRT_MaterialLib));
    lib->m_poType = Pool_Type_Memory;

    for (size_t i = 0; i < m_mtlList.size(); ++i)
    {
        CRT_Material* mtl = (CRT_Material*)m_mtlList[i]->NewObject();
        lib->m_mtlList.push_back(mtl);
    }

    return lib;
}		

bool CRT_MaterialLib::DelMaterial(const char* name)
{
    for (vector<CRT_Material*>::iterator it = m_mtlList.begin(); 
        it != m_mtlList.end(); ++it)
    {
        if((*it)->GetName() == name)
        {
            RtcGetActorManager()->ReleasePoolObject(*it);
            m_mtlList.erase(it);
            return true;
        }
    }
    return false;
}

#ifdef CHAR_VIEWER
void CRT_MaterialLib::AttachGrid(void *p)
{
    CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pItemMtl,*pItemList;
	// CXTPPropertyGridItemConstraints  *pList;

	int i;
	CString tmp;
	
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("Merge Material"));
	pItem->SetFlags(xtpGridItemHasExpandButton);

    pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("Add Material"));
	pItem->SetFlags(xtpGridItemHasExpandButton);
	for(i=0; i<m_mtlList.size(); i++)
	{
		CRT_Material *mtl;
		mtl = m_mtlList[i];

		// tmp.Format("%d",i);
		// pItemMtl = pRoot->AddChildItem(new CXTPPropertyGridItem("Mtl Index",tmp));
		// pItemMtl->SetFlags(xtpGridItemHasExpandButton);
        tmp.Format("%d",i);
		pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("Mtl Idx",tmp));
		pItem->SetFlags(xtpGridItemHasExpandButton);
		
		tmp.Format("%s",mtl->GetName().c_str());
        pItem->AddChildItem(new CXTPPropertyGridItem("Mtl Name",tmp));
		// pItem->SetFlags(xtpGridItemHasExpandButton);
        // add other info
        // ...
		pItem->Expand();
	}

	pRoot->Expand();
}

bool CRT_MaterialLib::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			if(pItem->GetCaption() == "Mtl Name")
			{
				CRT_Material *mtl = GetMaterial(atol(pItem->GetParentItem()->GetValue()));
				std::string tmp = pItem->GetValue();
				if(mtl) mtl->SetName(tmp);
			}
			return true;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "Mtl Idx") 
			{
				if(MessageBox(NULL,"确实要删除吗？","提示",MB_OKCANCEL)!=IDOK)
					return false;
				CRT_Material *tmp = GetMaterial(atol(pItem->GetValue()));
				if(tmp)
				{
					std::string name = tmp->GetName();
					DelMaterial(name.c_str());
				}
				return true;
			}
			else if(pItem->GetCaption() == "Add Material")
			{
				CDlgNewShader dlg;
				dlg.DoModal();
				return true;
			}
			else if(pItem->GetCaption() == "Merge Material")
			{
				// TODO: Add your command handler code here
				char filter[] = "Actor Files (*.mtl)|*.mtl||";
				char bak[255];

				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK) 
				{
					SetCurrentDirectory(bak);
					return false;
				}
				SetCurrentDirectory(bak);
				CString path = dlg.GetPathName();
				if(path.GetLength() <=0 ) return false;
				
				RtArchive *ar = RtCoreFile().CreateFileReader(path);
				if(!ar) return false;
				RtObject *ob = ar->ReadObject(NULL);
				if(!ob || !ob->IsKindOf(RT_RUNTIME_CLASS(CRT_MaterialLib)))
				{
					RtCoreLog().Error("read mtl lib file error\n");
					return false;
				}
				CRT_MaterialLib *lib = (CRT_MaterialLib*)ob;
				for(int i=0; i<lib->m_mtlList.size(); i++)
				{
					CRT_Material *mtl = (CRT_Material*)lib->m_mtlList[i]->NewObject();
					if(!AddMaterial(mtl))
						DEL_ONE(mtl);
				}
				DEL_ONE(lib);
				return true;
			}
			break;
	}
	return false;
}

CRT_Material *CRT_MaterialLib::GetSelectedMtl(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;
	if(pItem->GetCaption() == "Mtl Index")
	{
		// get mtl idx
		int idx = atol(pItem->GetValue());
		if(idx<0 || idx>=m_mtlList.size()) return NULL;
		return m_mtlList[idx];
	}
	return NULL;
}

bool CRT_MaterialLib::ConvertMaterial(const std::string strName, const std::string strNewType)
{
	CRT_Material* pOldMat = GetMaterial(strName.c_str());
	if (!pOldMat)	return false;

	if (strcmp(pOldMat->GetRuntimeClass()->m_lpszClassName, strNewType.c_str()) == 0)
		return false;

	RtRuntimeClass* pClass = rtFindRuntimeClass(strNewType.c_str());
	if (pClass == NULL)	return false;

	CRT_Material* pMat = (CRT_Material*)pClass->CreateObject();
	if (!pMat)	return false;

	if (!pMat->CopyFrom(pOldMat))
	{
		DEL_ONE(pMat);
		return false;
	}

	if (RemoveMaterial(strName))
	{
		DEL_ONE(pOldMat);
        pMat->Destroy();
        pMat->Create();
		return AddMaterial(pMat);
	}
	else
		return false;
}

/*
int CRT_MaterialLib::GetChannelNum()
{
	return m_mtlList.size();
}

const char *CRT_MaterialLib::GetChannelName(int channel)
{
	return m_mtlList[channel]->GetName().c_str(); 
}

int CRT_MaterialLib::GetKeyNum(int channel)
{
	return m_mtlList[channel]->GetKeyNum();
}

int CRT_MaterialLib::GetKeyFrame(int channel,int key)
{
	return m_mtlList[channel]->GetKeyFrame(key);
}

ETV_RET CRT_MaterialLib::OnNotify(ETV_OP op,long param1,long param2)
{
	switch(op)
	{
		case OP_MODIFY_CHANNEL:
			// m_mtlList[param1]->AttachGrid(0);
			break;

	}
	return RET_NONE;
}
*/
#endif

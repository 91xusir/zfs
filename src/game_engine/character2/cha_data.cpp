//
// base implementation file
//
#include "character2/actor.h"

int  SSerialize::ms_nCurrentFileVersion = ACTOR_FILE_VERSION_1;
bool SSerialize::ms_bIsLoadingSkin      = false;

bool IsDump(RtgVertex2 &a,RtgVertex2 &b)
{
	return Abs(a.x-b.x)<0.01f && Abs(a.y-b.y)<0.01f;
}

bool IsDump(RtgVertex3 &a,RtgVertex3 &b)
{
	return Abs(a.x-b.x)<0.01f && Abs(a.y-b.y)<0.01f && Abs(a.z-b.z)<0.01f;
}

RT_IMPLEMENT_DYNAMIC(CRT_ObjectData, RtObject, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_Actor, CRT_ObjectData, NULL, "")
RT_IMPLEMENT_DYNAMIC(CRT_Effect, CRT_ObjectData, NULL, "")

RT_IMPLEMENT_DYNAMIC(CRT_Skin, CRT_ObjectData, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_VaSkin, CRT_Skin, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_SkelSkin, CRT_Skin, NULL, "")

RT_IMPLEMENT_DYNAMIC(CRT_Material, CRT_ObjectData, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlStandard, CRT_Material, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlBlend, CRT_Material, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MaterialLib, CRT_ObjectData, NULL, "")

RT_IMPLEMENT_DYNCREATE(CRT_EffectEmitter, CRT_Effect, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectRibbon, CRT_Effect, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectBlur, CRT_Effect, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectProjector, CRT_Effect, NULL, "")

RT_IMPLEMENT_DYNAMIC(CRT_ForceField, RtObject, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_FFVortex, CRT_ForceField, NULL, "")

// --------------------------------------------------------------------------------------------
// object data
// --------------------------------------------------------------------------------------------
void CRT_ObjectData::DataChange(DWORD p1, DWORD p2)
{
	list<CRT_ObjectInst*>::iterator it;
	for(it=m_instList.begin(); it!=m_instList.end(); it++)
		(*it)->OnDataChange(p1,p2);
}

// --------------------------------------------------------------------------------------------
// bone
// --------------------------------------------------------------------------------------------
void SRT_Bone::UseFrame(float frame)
{
	RtgMatrix12 parent;
	parent.Unit();
	int i,num = (int)childList.size();
	for(i=0;i<num;i++)
		childList[i]->UseFrame(parent,frame);
}

void SRT_Bone::UseFrame(RtgMatrix12 &parent, float frame)
{
	long frm = frame;
	if(mode == BONE_MODE_FULL_MATRIX)
	{
		skinMat = matrixVec[frm];
	}
	normMat = skinMat;
	normMat._30 = normMat._31 = normMat._32 = 0.0f;

	int i,num = (int)childList.size();
	for(i=0;i<num;i++)
		childList[i]->UseFrame(skinMat,frame);
}

CRT_ObjectInst *CRT_Skin::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_SkinInstance;
#else
	return NULL;
#endif
}

// --------------------------------------------------------------------------------------------
// base skin
// --------------------------------------------------------------------------------------------
CRT_Skin::CRT_Skin()
{
	m_bBoard = false; 
	m_mtlLib = NULL; 

	m_bHelper = false;
	m_bHelpDynamic = false;
}

CRT_Skin::~CRT_Skin() 
{
	//delete m_mtlLib;
	DEL_ONE( m_mtlLib );
}
struct SBoolStruct
{
	char cVersion : 4;
	char bBoard   : 4;
	friend RtArchive& operator<<(RtArchive& ar, SBoolStruct& s) 
	{		
		ar.Serialize( &s , 1 );
		return ar;
	}
};

long CRT_Skin::Serialize(RtArchive& ar)
{
	if( ar.IsLoading() )
	{
		SBoolStruct bBorad;
        ar << m_name ;
		ar << bBorad;
		m_bBoard = bBorad.bBoard;
		if( bBorad.cVersion == 0 )
		{
            SSerialize::ms_nCurrentFileVersion = ACTOR_FILE_VERSION_0;
		}
		if( bBorad.cVersion == 1 )
		{
            SSerialize::ms_nCurrentFileVersion = ACTOR_FILE_VERSION_1;
		}
	}
	if( ar.IsStoring() )
	{
		SBoolStruct bBorad;
		bBorad.bBoard   = m_bBoard;
        bBorad.cVersion = 1;
        ar << m_name ;
		ar << bBorad;
	}	
	ar << m_rsVec << m_mtlLib;
	ar << m_dict;
	ar << m_bHelper << m_bHelpDynamic;
	BEGIN_VERSION_CONVERT(1)
	// read/write extra data
	END_VERSION_CONVERT

	if(ar.IsLoading())
	{
		m_mtlLib->m_poType = POOL_TYPE_OBJ_SUB;
	}
	return ar.IsError() ? 0 : 1;
}

// --------------------------------------------------------------------------------------------
// vertex animation skin
// --------------------------------------------------------------------------------------------
long CRT_VaSkin::Serialize(RtArchive& ar)
{
#ifdef FLIP_V
#ifndef MAX_UTILITY
	if(ar.IsStoring()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif
#endif

	CRT_Skin::Serialize(ar);
	ar << m_bNor << m_bUV << m_verList << m_frmList;
	if(m_bBoard)		
		ar << m_boardPos << m_boardPosList;

#ifdef FLIP_V
#ifndef MAX_UTILITY
	if(ar.IsStoring()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif
#endif

	BEGIN_VERSION_CONVERT(0)
	ar << m_bZup;
	BEGIN_VERSION_CONVERT(1)
	// read/write extra data
	END_VERSION_CONVERT
	END_VERSION_CONVERT

#ifdef FLIP_V
	if(ar.IsLoading()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif
	
	return ar.IsError() ? 0 : 1;
}

bool CRT_VaSkin::GetFrame(float frame, vector<SRT_StaticVtx> &verList, float &visible, bool updateNor)
{
	// if(m_bStatic) return false;
	if(m_frmList.size() == 0) return false;

	long frm = frame;
	SRT_VaFrame &vf = m_frmList[frm];
	if(m_bBoard) 
		m_boardPos = m_boardPosList[frm];
	visible = vf.visible;

	SRT_StaticVtx *m = &m_verList[0];
	SRT_StaticVtx *v = &verList[0];
	SRT_VaVtxPos *vp = &vf.verList[0]; 
	SRT_VaVtxNor *vn = &vf.norList[0];
	SRT_VaVtxUV  *vu = &vf.uvList[0];

	int num = vf.verList.size();
	PFM()->vaVtxNum += num;
	SRT_StaticVtx tmp;
	for(int i=0; i<num; i++,v++,vp++,vn++,vu++,m++)
	{
		vp->GetValue(v->pos);
		if(m_bNor)
			vn->GetValue(v->nor);
		else
			v->nor = m->nor;
		if(m_bUV)
			vu->GetValue(v->uv);
		else
			v->uv = m->uv;
	}
	return true;
}

void CRT_VaSkin::Output()
{
	RtCoreLog().Info("VaSkin Info\n");
	RtCoreLog().Info("name = %s,rs = %d,ver = %d\n", m_name.c_str(), m_rsVec.size(),
		m_verList.size());
}

long CRT_VaSkin::PushVertex(SRT_StaticVtx &v)
{
	unsigned int i;
	for(i=0; i<m_verList.size(); i++)
	{
		if(IsDump(m_verList[i].pos,v.pos) && IsDump(m_verList[i].nor,v.nor)
			&& IsDump(m_verList[i].uv,v.uv))
		return i;
	}
	m_verList.push_back(v);
	return i;
}

// --------------------------------------------------------------------------------------------
// skeleton animation skin
// --------------------------------------------------------------------------------------------
// from RtObject
long CRT_SkelSkin::Serialize(RtArchive& ar)
{
#ifdef FLIP_V
#ifndef MAX_UTILITY
	if(ar.IsStoring()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif
#endif	
	SSerialize::ms_bIsLoadingSkin = false;
	CRT_Skin::Serialize(ar);
	ar << m_verList;

#ifdef FLIP_V
#ifndef MAX_UTILITY
	if(ar.IsStoring()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif      
#endif      

	BEGIN_VERSION_CONVERT(1)
	// read/write extra data
	END_VERSION_CONVERT

#ifdef FLIP_V
	if(ar.IsLoading()) { for(int i=0; i<m_verList.size(); i++) m_verList[i].uv.y = 1-m_verList[i].uv.y; }
#endif
	return ar.IsError() ? 0 : 1;
}

bool CRT_SkelSkin::GetFrame(float frame,vector<SRT_StaticVtx> &verList,float &visible,bool updateNor)
{
	visible = 1.0f;
	if(updateNor)
		UpdateWithNormal(verList);
	else
		Update(verList);
	return true;
}

void CRT_SkelSkin::Update(vector<SRT_StaticVtx> &verList)
{
	RtgVertex3 v,n,t;
	int max = m_verList.size();
	PFM()->skelVtxNum += max;

	SRT_DynamicVtx *d = &m_verList[0];
	for(int i=0; i<max; i++)
	{
		int num = d->boneVec.size();
		v.Set(0.0f,0.0f,0.0f);
		n = d->nor;
		if(num == 1) {
			rtgV3MultiplyM12(v.m_v,d->boneVec[0].posOffset.m_v,d->boneVec[0].bone->skinMat.m_m);
		}
		else
		{
			for(int j=0; j<num; j++) {
				rtgV3MultiplyM12(t.m_v,d->boneVec[j].posOffset.m_v,d->boneVec[j].bone->skinMat.m_m);
				v = v + d->boneVec[j].weight * t;
			}
		}
		verList[i].pos = v;
		verList[i].nor = n;
		d++;
	}
}

void CRT_SkelSkin::UpdateWithNormal(vector<SRT_StaticVtx> &verList)
{
	RtgVertex3 v,n,t;
	int max = m_verList.size();
	PFM()->skelVtxNum += max;

	SRT_DynamicVtx *d = &m_verList[0];
	for(int i=0; i<max; i++)
	{
		int num = d->boneVec.size();
		v.Set(0.0f,0.0f,0.0f);
		n.Set(0.0f,0.0f,0.0f);
		if(num == 1) {
			rtgV3MultiplyM12(v.m_v,d->boneVec[0].posOffset.m_v, d->boneVec[0].bone->skinMat.m_m);
			rtgV3MultiplyM12(n.m_v,d->boneVec[0].norOffset, d->boneVec[0].bone->normMat.m_m);
		}
		else
		{
			for(int j=0; j<num; j++) {
				rtgV3MultiplyM12(t.m_v, d->boneVec[j].posOffset.m_v, d->boneVec[j].bone->skinMat.m_m);
				v = v + d->boneVec[j].weight*t;
				rtgV3MultiplyM12(t.m_v,d->boneVec[j].norOffset, d->boneVec[j].bone->normMat.m_m);
				n = n + d->boneVec[j].weight*t;
			}
		}
		verList[i].pos = v;
		verList[i].nor = n;
		d++;
	}
}

void CRT_SkelSkin::Output()
{
    RtCoreLog().Info("SkelSkin Info\n");
	RtCoreLog().Info("name = %s, rs = %d, ver = %d\n",m_name.c_str(),m_rsVec.size(),
		m_verList.size());

	int total=0;
	for(int i=0; i<m_rsVec.size(); i++)
	{
		total += m_rsVec[i].idxVec.size()/3;
		RtCoreLog().Info("texture = %s, tri = %d\n", m_rsVec[i].texture.c_str(),
			m_rsVec[i].idxVec.size()/3);
	}
	RtCoreLog().Info("total tri = %d\n", total);

	for(int i=0; i<m_verList.size(); i++)
	{
		// m_verList[i].Output();
	}
}

#ifdef MAX_UTILITY
// for max export
long CRT_SkelSkin::PushVertex(SRT_DynamicVtx &v)
{
	for(int i=0; i<(int)m_verList.size(); i++)
	{
		if(IsDump(m_verList[i].pos,v.pos) && IsDump(m_verList[i].nor,v.nor)
			&& IsDump(m_verList[i].uv,v.uv))
			return i;
	}
	m_verList.push_back(v);
	return i;
}
#endif

bool CRT_SkelSkin::LinkActor(CRT_Actor *actor)
{
	if(m_actor && m_actor != actor)
    {
        RtCoreLog().Error("%s: multi link actor error\n",__FUNCTION__);
	    return false;
    }

	for(int i=0; i<m_verList.size(); i++)
	{
		vector<SRT_BoneRef>::iterator it;
		for(it=m_verList[i].boneVec.begin(); it!=m_verList[i].boneVec.end();)
		{
			SRT_BoneRef *ref = &(*it);
			ref->bone = actor->GetBone(ref->idx);
			if(!ref->bone) 
			{
				// remove this bone
                RtCoreLog().Error("%s: skin = %s,can't find bone [%d]\n", __FUNCTION__, m_poTag.c_str(), ref->idx);
				it = m_verList[i].boneVec.erase(it);
				continue;
			}
			it++;
		}
	}
	m_actor = actor;
	return true;
}

// --------------------------------------------------------------------------------------------
// actor class
// --------------------------------------------------------------------------------------------
CRT_Actor::CRT_Actor()
{ 
	m_shadowMode = SHADOW_MODE_DYNAMIC;
	m_lightingMode = LIGHTING_MODE_FULL;

	m_strLightingMode = "Full";
	m_strShadowMode = "Dynamic";
	m_animSpeed = 1.0; 
	m_bZTest = true;
	m_bZWrite = true;
	m_bFog = true;
	m_visible = 1.0;
	m_scale.Set(1,1,1);
	m_offset.Zero();
}

CRT_Actor::~CRT_Actor() 
{
	int i;
	for(i=0; i<m_skinVec.size(); i++)
		if(m_skinVec[i])
		{
			//delete m_skinVec[i];
			
			DEL_ONE( m_skinVec[i] );
		}

	for(i=0; i<m_eftVec.size(); i++)
	{
		//delete m_eftVec[i];
		DEL_ONE( m_eftVec[i] );
	}
}

CRT_ObjectInst *CRT_Actor::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_ActorInstance;
#else
	return NULL;
#endif
}

void CRT_Actor::GetRelatedRes(vector<SRelatedRes> &res)
{
	int i;
	for(i=0; i<m_skinVec.size(); i++)
		m_skinVec[i]->m_mtlLib->GetRelatedRes(res);
	for(i=0; i<m_eftVec.size(); i++)
		m_eftVec[i]->GetRelatedRes(res);
}

SRT_Pose *CRT_Actor::FindPose(const char *name)
{
	TPoseMap::iterator it;
	it = m_poseMap.find(name);
	if(it==m_poseMap.end())
		return NULL;
	else
		return &(it->second);
}

long CRT_Actor::Serialize(RtArchive& ar)
{
	SSerialize::ms_bIsLoadingSkin = false;
	if(ar.IsStoring())
		return Save(ar);
	else
		return Load(ar);
}

bool CRT_Actor::Save(RtArchive& ar)
{
	long ver = ACTOR_FILE_VERSION_1 ;
	ar << ver;	
	SerializeData(ar);
	return !ar.IsError();
}

bool CRT_Actor::Load(RtArchive& ar)
{
	long ver;
	ar << ver;
	SSerialize::ms_nCurrentFileVersion = ver;
	// version check
	if(ver == ACTOR_FILE_VERSION_0 || ver == ACTOR_FILE_VERSION_1 )
		SerializeData(ar);
	else
		return false;
		//SerializeOldData(ar);
	
	return !ar.IsError();
}
//bool CRT_Actor::SerializeOldData(RtArchive& ar)
//{
//    bool m_bUseLight,m_bUseVC,m_bDynamicShadow;
//	ar << m_animType << m_frameNum;
//	ar << m_bUseLight << m_bUseVC << m_bZTest << m_bDynamicShadow << m_animSpeed;
//    ar << m_poseMap << m_boneVec << m_skinVec << m_eftVec << m_boundBoxVec;
//
//	m_shadowMode = m_bDynamicShadow?0:1;
//	m_lightingMode = m_bUseLight?0:1;
//
//	BEGIN_VERSION_CONVERT(1)
//    // read/write extra data
//    END_VERSION_CONVERT
//
//	if(ar.IsLoading())
//	{
//		int i;
//		for(i=0; i<m_skinVec.size(); i++)
//			m_skinVec[i]->m_poType = POOL_TYPE_OBJ_SUB;
//
//		for(i=0; i<m_eftVec.size(); i++)
//			m_eftVec[i]->m_poType = POOL_TYPE_OBJ_SUB;
//		if(!Build()) return false; 
//	}
//	return ar.IsError() ? 0 : 1;
//
//}
bool CRT_Actor::SerializeData(RtArchive& ar)
{
	if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
	{
        std::vector<SShortBBox>  kaShortBBoxArray;	
		ar << m_animType << m_frameNum;
		ar << m_poseMap << m_boneVec << m_skinVec << m_eftVec << kaShortBBoxArray;
		ar << m_relatedActFile << m_dict;

		ar << m_shadowMode << m_lightingMode << m_animSpeed;
		ar << m_bZTest << m_bZWrite << m_bFog << m_visible;
		ar << m_scale << m_offset;

		int nBoxSize = kaShortBBoxArray.size();
		for( int i=0;i<nBoxSize;i++)
		{
			RtgAABB kInsert;
			ConvertShortAABBToRtgAABB( kaShortBBoxArray[ i ] , kInsert );
			m_boundBoxVec.push_back( kInsert );
		}

		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
	}
	if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
	{
        std::vector<SShortBBox>  kaShortBBoxArray;
		int nBoxSize = m_boundBoxVec.size();
		for( int i=0;i<nBoxSize;i++)
		{
			SShortBBox kInsert;
			ConvertRtgAABBToShortAABB( m_boundBoxVec[ i ] , kInsert );
			kaShortBBoxArray.push_back( kInsert );
		}
		ar << m_animType << m_frameNum;
		ar << m_poseMap << m_boneVec << m_skinVec << m_eftVec << kaShortBBoxArray;
		ar << m_relatedActFile << m_dict;

		ar << m_shadowMode << m_lightingMode << m_animSpeed;
		ar << m_bZTest << m_bZWrite << m_bFog << m_visible;
		ar << m_scale << m_offset;

		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
	}
	if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
	{
        ar << m_animType << m_frameNum;
		ar << m_poseMap << m_boneVec << m_skinVec << m_eftVec << m_boundBoxVec;
		ar << m_relatedActFile << m_dict;

		ar << m_shadowMode << m_lightingMode << m_animSpeed;
		ar << m_bZTest << m_bZWrite << m_bFog << m_visible;
		ar << m_scale << m_offset;

		ar << m_strLightingMode << m_strShadowMode;

		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
	}
	if( ar.IsLoading() )
	{
		int i;
		for(i=0; i<m_skinVec.size(); i++)
			m_skinVec[i]->m_poType = POOL_TYPE_OBJ_SUB;

		for(i=0; i<m_eftVec.size(); i++)
			m_eftVec[i]->m_poType = POOL_TYPE_OBJ_SUB;

		if(!Build()) return false; 
	}
	return ar.IsError() ? 0:1;
}

bool CRT_Actor::LoadRelatedActorFile()
{
	DWORD beg = rtMilliseconds();
	// load related act file
	// append anim,pose
	string path;
	char tmp[512];
	rt2_strcpy(tmp,m_diskFile.c_str());
	int size = m_diskFile.size();
	while(size--)
	{
		if(tmp[size]=='\\' || tmp[size]=='/')
		{
			tmp[size+1] = '\0';
			break;
		}
	}
		
	path = tmp;
	string file;
	CRT_Actor *act;
	for(int i=0; i<m_relatedActFile.size(); i++)
	{
		file = tmp + m_relatedActFile[i];
		act = (CRT_Actor*)ACTOR()->LoadFromFile(file.c_str(),ACTOR_RES_TYPE_ACT);
		if(!act)
		{
#ifdef ACTOR_EDITOR
			//static char msg[1024];
			S_MEMDEF(msg, 1024)
			S_MEMPROTECTOR(msg, 1024, bMP)
			rt2_sprintf(msg, "找不到文�?%s", m_relatedActFile[i].c_str());
			CMainFrame *mn;
			mn = (CMainFrame*)AfxGetMainWnd();
			mn->MessageBox(msg);
#endif
			continue;
		}	
	
		Merge(act);
		//delete act;
		DEL_ONE( act );
	}

	RtCoreLog().Info("load related act file used [%d] ms\n", rtMilliseconds()-beg);
	return true;
}

// 将第二个文件的关键帧信息合并入第一个文�?
bool CRT_Actor::MergeEvent(CRT_Actor *act)
{
	TPoseMap::iterator it1;
	for(it1=m_poseMap.begin(); it1!=m_poseMap.end(); it1++)
	{
		string name = it1->first;
		TPoseMap::iterator it2;
		it2 = act->m_poseMap.find(name);
		if(it2 != act->m_poseMap.end())
		{
			vector<SRT_PoseEvent> eventVec = ((*it2).second).eventVec;
			for(int i=0; i<eventVec.size(); i++)
			{
				float per = 0;
				float total = (it2->second).endFrm - (it2->second).startFrm;
				if(total != 0)
				{
					per = (eventVec[i].keyFrame - (it2->second).startFrm) / total;
				}
				eventVec[i].keyFrame = (it1->second).startFrm + per*((it1->second).endFrm - (it1->second).startFrm); 
			}
			(it1->second).eventVec = eventVec;
		}
	}
	return true;
}

// 将第二个文件整体合并入第一个文�?
bool CRT_Actor::Merge(CRT_Actor *act)
{
	// merge frame
	for(int i=0; i<m_boneVec.size(); i++)
	{
		SRT_Bone *b1 = &m_boneVec[i];
		SRT_Bone *b2 = act->GetBone(b1->name.c_str());
		if(!b2) 
		{
#ifdef ACTOR_EDITOR
			//static char msg[1024];
			S_MEMDEF(msg, 1024)
			S_MEMPROTECTOR(msg, 1024, bMP)
			rt2_sprintf(msg,"找不到骨�?%s",b1->name.c_str());
			CMainFrame *mn;
			mn = (CMainFrame*)AfxGetMainWnd();
            mn->MessageBox(msg);
#endif
			// 填入数据，避免当�?
			for(int j=0; j<b2->matrixVec.size(); j++)
				b1->matrixVec.push_back(b1->matrixVec.back());

			for(int j=0; j<b2->ribbonFrame.size(); j++)
				b1->ribbonFrame.push_back(b1->ribbonFrame.back());
			continue;
		}

		for(int j=0; j<b2->matrixVec.size(); j++)
			b1->matrixVec.push_back(b2->matrixVec[j]);

		// merge ribbon2 effect info
		for(int j=0; j<b2->ribbonFrame.size(); j++)
			b1->ribbonFrame.push_back(b2->ribbonFrame[j]);
	}

	// merge bounding box
	for(int i=0; i<act->m_boundBoxVec.size(); i++)
	{
		m_boundBoxVec.push_back(act->m_boundBoxVec[i]);
	}
	
	// merge pose
	TPoseMap::iterator it;
	for(it=act->m_poseMap.begin();it!=act->m_poseMap.end();it++)
	{
		SRT_Pose *pose = &(*it).second;
		pose->startFrm += m_frameNum;
		pose->endFrm += m_frameNum;
		for(int i=0;i<pose->eventVec.size();i++)
		{
			pose->eventVec[i].keyFrame += m_frameNum;
		}
		m_poseMap[pose->name] = *pose;
	}
	m_frameNum += act->m_frameNum;

	return true;
}

SRT_Bone *CRT_Actor::GetBone(const char *name)
{
	int max = m_boneVec.size();
	for(int i=0; i<max; i++)
	{
		if(m_boneVec[i].name == name) return &m_boneVec[i];
	}
	return NULL;
}

SRT_Bone *CRT_Actor::GetBone(unsigned char idx)
{
	if(idx >= m_boneVec.size()) return NULL;
	return &m_boneVec[idx];
}

bool CRT_Actor::GetBoneIndex(const char *name, unsigned char &idx)
{
	int max = m_boneVec.size();
	for(int i=0; i<max; i++)
	{
		if(m_boneVec[i].name == name) 
		{
			idx = i;
			return true;
		}
	}
	return false;
}

bool CRT_Actor::Build()
{
	SRT_Bone *now,*tmp;
	for(int i=0; i<m_boneVec.size(); i++)
	{
		now = &m_boneVec[i];
		if(now->parentName == "NULL") continue;
		
		tmp = GetBone(now->parentName.c_str());
		if(tmp)
		{
			tmp->childList.push_back(now);
		}
		else
		{
			RtCoreLog().Error("%s: [%s] can't get parent bone [%s]\n",
				__FUNCTION__, m_poTag.c_str(), now->parentName.c_str());
		}
	}
	return true;
}

bool CRT_Actor::UseFrame(float frame,RtgMatrix12 *m)
{
	// check if static object
	if(m_frameNum==1 && frame!=0) return false;
	if(frame<0 || frame>=m_frameNum) return false;
	if(m_boneVec.size()==0) return true;

	PFM()->skelBoneNum += m_boneVec.size();
	m_boneVec[0].UseFrame(frame);
	return true;
}

void CRT_Actor::Output()
{
	int i;
    RtCoreLog().Info("Actor Info:\n");
	RtCoreLog().Info("\tBone = %d\n",m_boneVec.size());
	for(i=0; i<(int)m_boneVec.size(); i++)
	{
		m_boneVec[i].Output();
	}
	RtCoreLog().Info("\tSkin = %d\tEffect = %d\n",m_skinVec.size(),m_eftVec.size());

	for(i=0; i<(int)m_skinVec.size(); i++)
	{
		m_skinVec[i]->Output();
	}
}

void CRT_Actor::GetInfo(SActorInfo &info)
{
	memset(&info, 0, sizeof(info));
	info.skinNum = m_skinVec.size();
	info.eftNum  = m_eftVec.size();
	info.boneNum = m_boneVec.size();

	for(int i=0; i<m_skinVec.size(); i++)
	{
		CRT_Skin *skin = m_skinVec[i];
		info.verNum += skin->GetVertexNum();

		for(int j=0; j<skin->m_rsVec.size(); j++)
		{
			info.mtlNum ++;
			info.triNum += skin->m_rsVec[j].idxVec.size()/3;
		}
	}
}

// --------------------------------------------------------------------------------------------
// material lib
// --------------------------------------------------------------------------------------------
CRT_MaterialLib::CRT_MaterialLib() {}

CRT_MaterialLib::~CRT_MaterialLib()
{
	for(int i=0; i<m_mtlVec.size(); i++)
	{
		//delete m_mtlVec[i];
		DEL_ONE( m_mtlVec[i] );
	}
}

CRT_ObjectInst *CRT_MaterialLib::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_MtlLibInstance;
#else
	return NULL;
#endif
}

void CRT_MaterialLib::GetRelatedRes(vector<SRelatedRes> &res)
{
	for(int i=0; i<m_mtlVec.size(); i++)
		m_mtlVec[i]->GetRelatedRes(res);
}

long CRT_MaterialLib::Serialize(RtArchive& ar)
{
	ar << m_mtlVec << m_dict;
	BEGIN_VERSION_CONVERT(1)
	// read/write extra data
	END_VERSION_CONVERT

	if(ar.IsLoading())
	{
		for(int i=0; i<m_mtlVec.size(); i++)
			m_mtlVec[i]->m_poType = POOL_TYPE_OBJ_SUB;
	}
	return ar.IsError() ? 0 : 1;
}

int CRT_MaterialLib::GetMtlNum() 
{
	return int(m_mtlVec.size());
}

CRT_Material *CRT_MaterialLib::GetMtlByIdx(int idx) 
{
	if(idx<0 || idx>=m_mtlVec.size())
		return NULL;
	return m_mtlVec[idx];
}

CRT_Material *CRT_MaterialLib::GetMtlByName(const char *name) 
{
	for(int i=0; i<m_mtlVec.size(); i++) 
	{
		if(m_mtlVec[i]->GetName() == name)
			return m_mtlVec[i];
	}
	return NULL;
}

bool CRT_MaterialLib::AddMaterial(CRT_Material *mtl)
{
	if(GetMtlByName(mtl->GetName().c_str()))
		return false;
	m_mtlVec.push_back(mtl);
	return true;
}

void CRT_MaterialLib::DelMaterial(const char *name)
{
	vector<CRT_Material*>::iterator it;
	for(it=m_mtlVec.begin(); it!=m_mtlVec.end(); it++) 
	{
		if((*it)->GetName() == name) {
			m_mtlVec.erase(it);
			return;
		}
	}
}

void CRT_MaterialLib::Output()
{

}

// --------------------------------------------------------------------------------------------
// mtl standard
// --------------------------------------------------------------------------------------------
CRT_ObjectInst *CRT_MtlStandard::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_MtlInstStandard;
#else
	return NULL;
#endif
}

void CRT_MtlStandard::GetRelatedRes(vector<SRelatedRes> &res)
{
	SRelatedRes tmp;
	tmp.type = RELATED_RES_TEXTURE;
	if(m_texBase.length() > 0) {
		tmp.res  = m_texBase;
		res.push_back(tmp);
	}
	if(m_texHl.length() > 0) {
		tmp.res  = m_texHl;
		res.push_back(tmp);
	}
	if(m_texEffect1.length() > 0) {
		tmp.res  = m_texEffect1;
		res.push_back(tmp);
	}
	if(m_texEffectMask1.length() > 0) {
		tmp.res  = m_texEffectMask1;
		res.push_back(tmp);
	}
	if(m_texEffect2.length() > 0) {
		tmp.res  = m_texEffect2;
		res.push_back(tmp);
	}
	if(m_texEffectMask2.length() > 0) {
		tmp.res  = m_texEffectMask2;
		res.push_back(tmp);
	}
}

// --------------------------------------------------------------------------------------------
// mtl blend
// --------------------------------------------------------------------------------------------
CRT_ObjectInst *CRT_MtlBlend::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_MtlInstBlend;
#else
	return NULL;
#endif
}

void CRT_MtlBlend::GetRelatedRes(vector<SRelatedRes> &res)
{
	SRelatedRes tmp;
	tmp.type = RELATED_RES_TEXTURE;
	if(m_tex1.length() > 0) {
		tmp.res  = m_tex1;
		res.push_back(tmp);
	}
	if(m_tex2.length() > 0) {
		tmp.res  = m_tex2;
		res.push_back(tmp);
	}
	if(m_texMix.length() > 0) {
		tmp.res  = m_texMix;
		res.push_back(tmp);
	}
}

// --------------------------------------------------------------------------------------------
// effect emitter
// --------------------------------------------------------------------------------------------
CRT_EffectEmitter::CRT_EffectEmitter() 
{
	m_strGenMode = "Normal";
	m_strPathMode = "Normal";
	m_strGeometryMode = "Board";
	m_strBoardMode = "Full";
	m_strAlphaMode = "Add";
	m_strBoardCenter = "X0Y0";

	m_genMode = PARTICLE_GEN_MODE_NORMAL;
	m_pathMode = PARTICLE_PATH_MODE_NORMAL;
	m_geometryMode = PARTICLE_GEOMETRY_BOARD;
	m_alphaMode = ALPHA_ADD;
	m_boardMode = PARTICLE_BILLBOARD_FULL;
	m_boardCenter = PARTICLE_BOARDCENTER_X0Y0;
	m_autoDeleteModelPar = false;
	m_modelPose = "effect";
	m_bModelLoop = true;

	m_parLife = 2000;			
	m_emiSpeed = 10; 			
	m_midVal = 0.5;

	m_opaqueBeg = 1.0;
	m_opaqueMid = 1.0;
	m_opaqueEnd = 1.0;

	m_colorBeg.Set(1.0,1.0,1.0);
	m_colorMid.Set(1.0,1.0,1.0);
	m_colorEnd.Set(1.0,1.0,1.0);

	m_scaleBeg.Set(1.0,1.0,1.0);
	m_scaleMid.Set(1.0,1.0,1.0);
	m_scaleEnd.Set(1.0,1.0,1.0);

	m_sizeMin.Set(5.0,5.0,1.0);
	m_sizeMax.Set(5.0,5.0,1.0);

	m_bAlwaysFront   = false;
	m_bUseParentDir  = true;
	m_bUseParentCoor = false;
	m_speedMin.Set(0,0,20);
	m_speedMax.Set(0,0,20);
	
	m_accMin.Zero();
	m_accMax.Zero();

	m_texBeg = 1;
	m_texMid = 1;
	m_texEnd = 1;

	m_rotateBeg.Set(0.0,0.0,0.0);
	m_rotateMid.Set(0.0,0.0,0.0);
	m_rotateEnd.Set(0.0,0.0,0.0);

	m_texUSub = 1;
	m_texVSub = 1;
	m_bTexSub  = false;
	m_bTexRand = false;

	m_emiSizeMin.Zero();
	m_emiSizeMax.Zero();

	m_destSpeedRand = 0;
	m_destTime = 0;
}

CRT_EffectEmitter::~CRT_EffectEmitter() 
{
	for(int i=0; i<m_forceVec.size(); i++)
	{		
		DEL_ONE( m_forceVec[i] );
	}
}

long CRT_EffectEmitter::Serialize(RtArchive& ar)
{
	CRT_Effect::Serialize(ar);
	if( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_1 && ar.IsLoading() && SSerialize::ms_bIsLoadingSkin == false )
	{
        short sGenMode(0),sGeomMode(0),sPathMode(0),sAlphaMode(0),sBoradMode(0);
		ar << sGenMode << sGeomMode << sPathMode << sAlphaMode << sBoradMode;
		m_genMode      = sGenMode;
		m_geometryMode = sGeomMode;
		m_pathMode     = sPathMode;
		m_alphaMode    = sAlphaMode;
		m_boardMode    = sBoradMode;
		short sboardCenter( 0 );
		ar <<  sboardCenter;
		m_boardCenter = sboardCenter;

		ar << m_parLife << m_emiSpeed;
		ar << m_midVal << m_bAlwaysFront;
		ar << m_opaqueBeg << m_opaqueMid << m_opaqueEnd; 

		long nColorBeg(0);
		long nColorMid(0); 
		long nColorEnd(0);
		ar << nColorBeg << nColorMid << nColorEnd;
		m_colorBeg = LongToVector3_Color( nColorBeg );
		m_colorMid = LongToVector3_Color( nColorMid );
		m_colorEnd = LongToVector3_Color( nColorEnd );

		ar << m_texBeg << m_texMid << m_texEnd;

		SShortVec3 sScalBeg ;
		SShortVec3 sScalMid ;
		SShortVec3 sScalEnd ;
		ar << sScalBeg << sScalMid << sScalEnd;

		m_scaleBeg = Vector3ToShort3_Vector_100_Inverse( sScalBeg );
		m_scaleMid = Vector3ToShort3_Vector_100_Inverse( sScalMid );
		m_scaleEnd = Vector3ToShort3_Vector_100_Inverse( sScalEnd );


		ar << m_texture;
		ar << m_texUSub << m_texVSub << m_bTexSub << m_bTexRand;

		SShortVec3 SEmiSizeMin ;
		SShortVec3 SEmiSizeMax ;
		SShortVec3 SSizeMin    ;
		SShortVec3 SSizeMax    ;
		ar << SEmiSizeMin << SEmiSizeMax;
		ar << SSizeMin << SSizeMax;
		m_emiSizeMin = Vector3ToShort3_Vector_10_Inverse ( SEmiSizeMin );
		m_emiSizeMax = Vector3ToShort3_Vector_10_Inverse ( SEmiSizeMax );
		m_sizeMin    = Vector3ToShort3_Vector_10_Inverse ( SSizeMin );
		m_sizeMax    = Vector3ToShort3_Vector_10_Inverse ( SSizeMax );
		ar << m_keyVec;
		// par dif
		SShortVec3 SSpeedMin ;
		SShortVec3 SSpeedMax ;
		SShortVec3 SAccMin   ;
		SShortVec3 SAccMax   ;
		ar << SSpeedMin << SSpeedMax << SAccMin << SAccMax;
		ar << m_parent << m_bUseParentDir << m_bUseParentCoor;

		m_speedMin = Vector3ToShort3_Vector_10_Inverse ( SSpeedMin );
		m_speedMax = Vector3ToShort3_Vector_10_Inverse ( SSpeedMax );
		m_accMin   = Vector3ToShort3_Vector_10_Inverse ( SAccMin );
		m_accMax   = Vector3ToShort3_Vector_10_Inverse ( SAccMax );

		// par com
		ar << m_dest << m_destTime << m_destSpeedRand;
		ar << m_forceVec;
		ar << m_model;

		BEGIN_VERSION_CONVERT(0)
			ar << m_autoDeleteModelPar;
		BEGIN_VERSION_CONVERT(0)
			ar << m_bModelLoop << m_modelPose;
		BEGIN_VERSION_CONVERT(0)
			ar << m_rotateBeg << m_rotateMid << m_rotateEnd;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT	
	}
	if( ar.IsStoring() && SSerialize::ms_bIsLoadingSkin == false )
	{
        short sGenMode(m_genMode),sGeomMode(m_geometryMode),sPathMode(m_pathMode),sAlphaMode(m_alphaMode),sBoradMode(m_boardMode);
		ar << sGenMode << sGeomMode << sPathMode << sAlphaMode << sBoradMode;

		short sboardCenter( m_boardCenter );
		ar <<  sboardCenter;

		ar << m_parLife << m_emiSpeed;
		ar << m_midVal << m_bAlwaysFront;
		ar << m_opaqueBeg << m_opaqueMid << m_opaqueEnd; 

		long nColorBeg = Vector3ToLong_Color( m_colorBeg );
		long nColorMid = Vector3ToLong_Color( m_colorMid );
		long nColorEnd = Vector3ToLong_Color( m_colorEnd );
		ar << nColorBeg << nColorMid << nColorEnd;

		ar << m_texBeg << m_texMid << m_texEnd;


		SShortVec3 sScalBeg = Vector3ToShort3_Vector_100( m_scaleBeg );
		SShortVec3 sScalMid = Vector3ToShort3_Vector_100( m_scaleMid );
		SShortVec3 sScalEnd = Vector3ToShort3_Vector_100( m_scaleEnd );
		ar << sScalBeg << sScalMid << sScalEnd;

		ar << m_texture;
		ar << m_texUSub << m_texVSub << m_bTexSub << m_bTexRand;

        SShortVec3 SEmiSizeMin = Vector3ToShort3_Vector_10 ( m_emiSizeMin );
		SShortVec3 SEmiSizeMax = Vector3ToShort3_Vector_10 ( m_emiSizeMax );
		SShortVec3 SSizeMin    = Vector3ToShort3_Vector_10 ( m_sizeMin );
		SShortVec3 SSizeMax    = Vector3ToShort3_Vector_10 ( m_sizeMax );
		ar << SEmiSizeMin << SEmiSizeMax;
		ar << SSizeMin << SSizeMax;

		ar << m_keyVec;

		// par dif
		SShortVec3 SSpeedMin = Vector3ToShort3_Vector_10 ( m_speedMin );
		SShortVec3 SSpeedMax = Vector3ToShort3_Vector_10 ( m_speedMax );
		SShortVec3 SAccMin   = Vector3ToShort3_Vector_10 ( m_accMin );
		SShortVec3 SAccMax   = Vector3ToShort3_Vector_10 ( m_accMax );
		ar << SSpeedMin << SSpeedMax << SAccMin << SAccMax;
		ar << m_parent << m_bUseParentDir << m_bUseParentCoor;

		// par com
		ar << m_dest << m_destTime << m_destSpeedRand;
		ar << m_forceVec;
		ar << m_model;

		BEGIN_VERSION_CONVERT(0)
			ar << m_autoDeleteModelPar;
		BEGIN_VERSION_CONVERT(0)
			ar << m_bModelLoop << m_modelPose;
		BEGIN_VERSION_CONVERT(0)
			ar << m_rotateBeg << m_rotateMid << m_rotateEnd;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT
	}
	if( ( SSerialize::ms_nCurrentFileVersion == ACTOR_FILE_VERSION_0 && ar.IsLoading() )  || SSerialize::ms_bIsLoadingSkin )
	{
        ar << m_strGenMode << m_strPathMode << m_strGeometryMode; 
		ar << m_strAlphaMode <<	m_strBoardMode;

		ar << m_genMode << m_geometryMode << m_pathMode << m_alphaMode << m_boardMode;
		ar << m_strBoardCenter << m_boardCenter;

		ar << m_parLife << m_emiSpeed;
		ar << m_midVal << m_bAlwaysFront;
		ar << m_opaqueBeg << m_opaqueMid << m_opaqueEnd; 
		ar << m_colorBeg << m_colorMid << m_colorEnd;
		ar << m_texBeg << m_texMid << m_texEnd;
		ar << m_scaleBeg << m_scaleMid << m_scaleEnd;

		ar << m_texture;
		ar << m_texUSub << m_texVSub << m_bTexSub << m_bTexRand;

		ar << m_emiSizeMin << m_emiSizeMax;
		ar << m_sizeMin << m_sizeMax;

		ar << m_keyVec;

		// par dif
		ar << m_speedMin << m_speedMax << m_accMin << m_accMax;
		ar << m_parent << m_bUseParentDir << m_bUseParentCoor;

		// par com
		ar << m_dest << m_destTime << m_destSpeedRand;
		ar << m_forceVec;
		ar << m_model;

		BEGIN_VERSION_CONVERT(0)
			ar << m_autoDeleteModelPar;
		BEGIN_VERSION_CONVERT(0)
			ar << m_bModelLoop << m_modelPose;
		BEGIN_VERSION_CONVERT(0)
			ar << m_rotateBeg << m_rotateMid << m_rotateEnd;
		BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT
		END_VERSION_CONVERT
	}
	return ar.IsError() ? 0 : 1;
}

CRT_ObjectInst *CRT_EffectEmitter::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_EftInstEmitter;
#else
	return NULL;
#endif
}

bool CRT_EffectEmitter::CopyFrom(CRT_ObjectData *from)
{
	*this = *(CRT_EffectEmitter*)from;
	m_forceVec.clear();
	return true;
}

void CRT_EffectEmitter::GetRelatedRes(vector<SRelatedRes> &res)
{
	SRelatedRes tmp;
	tmp.type = RELATED_RES_TEXTURE;
	if(m_texture.length() > 0) {
		tmp.res  = m_texture;
		res.push_back(tmp);
	}
}

// --------------------------------------------------------------------------------------------
// effect ribbon
// --------------------------------------------------------------------------------------------
CRT_ObjectInst *CRT_EffectRibbon::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_EftInstRibbon;
#else
	return NULL;
#endif
}

bool CRT_EffectRibbon::CopyFrom(CRT_ObjectData *from)
{
	*this = *(CRT_EffectRibbon*)from;
	return true;
}

void CRT_EffectRibbon::GetRelatedRes(vector<SRelatedRes> &res)
{
	SRelatedRes tmp;
	tmp.type = RELATED_RES_TEXTURE;
	if(m_tex1.length() > 0) {
		tmp.res  = m_tex1;
		res.push_back(tmp);
	}
	if(m_tex2.length() > 0) {
		tmp.res  = m_tex2;
		res.push_back(tmp);
	}
}

// --------------------------------------------------------------------------------------------
// effect blur
// --------------------------------------------------------------------------------------------
CRT_ObjectInst *CRT_EffectBlur::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_EftInstBlur;
#else
	return NULL;
#endif
}

bool CRT_EffectBlur::CopyFrom(CRT_ObjectData *from)
{
	*this = *(CRT_EffectBlur*)from;
	return true;
}

void CRT_EffectBlur::GetRelatedRes(vector<SRelatedRes> &res)
{

}

// --------------------------------------------------------------------------------------------
// effect blur
// --------------------------------------------------------------------------------------------
CRT_ObjectInst *CRT_EffectProjector::CreateInstance()
{
#ifndef MAX_UTILITY
	return RT_NEW CRT_EftInstProjector;
#else
	return NULL;
#endif
}

bool CRT_EffectProjector::CopyFrom(CRT_ObjectData *from)
{
	*this = *(CRT_EffectProjector*)from;
	return true;
}

void CRT_EffectProjector::GetRelatedRes(vector<SRelatedRes> &res)
{
	SRelatedRes tmp;
	tmp.type = RELATED_RES_TEXTURE;
	if(m_tex.length() > 0) {
		tmp.res  = m_tex;
		res.push_back(tmp);
	}
}


#include "gc_include.h"
#include "gc_actor_graph.h"


GcActorGraph::GcActorGraph()
{
    m_bDisplay = false;
    m_pActor = NULL;
}

GcActorGraph::~GcActorGraph()
{
    guard;
    ClearActor();
    unguard;
}

void GcActorGraph::Reset()
{
    guard;
    ClearActor();
    m_bDisplay = false;
    m_pActor = NULL;
    unguard;
}

void GcActorGraph::ClearActor()
{
    guard;
    if (m_pActor!=NULL)
    {
        RtcGetActorManager()->ReleaseActor(m_pActor);
        m_pActor = NULL;
    }

    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {
		DEL_ONE((*it).second);
    }
    m_mapChildren.clear();
    unguard;
}

void GcActorGraph::SetPosition(float vX,float vY,float vZ)
{
    if (m_pActor)
        m_pActor->SetMatrix_Row(3, RtgVertex3(vX, vY, vZ));
    //guard;
    //if (m_pActor==NULL) return;
    //m_pActor->GetMatrix()->_30=vX;
    //m_pActor->GetMatrix()->_31=vY;
    //m_pActor->GetMatrix()->_32=vZ;
    //return ;
    //unguard;
}

void GcActorGraph::RegisterActorNotify(CRT_PoseNotify* vNotify)
{
    guard;
    if (m_pActor!=NULL)
    {
        m_pActor->RegisterNotify(vNotify);
    }
    unguard;
}

void GcActorGraph::SetRotation(RtgMatrix16& vrMatrix16)
{
    if (m_pActor)
    {
        m_pActor->SetMatrix_Row(0, RtgVertex3(vrMatrix16._00, vrMatrix16._01, vrMatrix16._02));
        m_pActor->SetMatrix_Row(1, RtgVertex3(vrMatrix16._10, vrMatrix16._11, vrMatrix16._12));
        m_pActor->SetMatrix_Row(2, RtgVertex3(vrMatrix16._20, vrMatrix16._21, vrMatrix16._22));
    }

    //guard;
    //if (m_pActor==NULL) return;

    //RtgMatrix12* p= m_pActor->GetMatrix();
    //p->_00 = vrMatrix16._00; p->_01 = vrMatrix16._01; p->_02 = vrMatrix16._02;
    //p->_10 = vrMatrix16._10; p->_11 = vrMatrix16._11; p->_12 = vrMatrix16._12;
    //p->_20 = vrMatrix16._20; p->_21 = vrMatrix16._21; p->_22 = vrMatrix16._22;
    //unguard;
}


void GcActorGraph::SetRotation(RtgMatrix12& vrMatrix12)
{
    if (m_pActor)
    {
        m_pActor->SetMatrix_Row(0, RtgVertex3(vrMatrix12._00, vrMatrix12._01, vrMatrix12._02));
        m_pActor->SetMatrix_Row(1, RtgVertex3(vrMatrix12._10, vrMatrix12._11, vrMatrix12._12));
        m_pActor->SetMatrix_Row(2, RtgVertex3(vrMatrix12._20, vrMatrix12._21, vrMatrix12._22));
    }

    //guard;
    //if (m_pActor==NULL) return;

    //RtgMatrix12* p= m_pActor->GetMatrix();
    //p->_00 = vrMatrix12._00; p->_01 = vrMatrix12._01; p->_02 = vrMatrix12._02;
    //p->_10 = vrMatrix12._10; p->_11 = vrMatrix12._11; p->_12 = vrMatrix12._12;
    //p->_20 = vrMatrix12._20; p->_21 = vrMatrix12._21; p->_22 = vrMatrix12._22;
    //return;
    //unguard;
}


 void GcActorGraph::ApplyShaderEffect(const char vName[])
{
    guard;
	if (m_pActor!=NULL)
	{
		// m_pActor->ApplyShaderEffect(vName);
	}
    unguard;
}

void GcActorGraph::RemoveShaderEffect(const char vName[])
{
    guard;
	if (m_pActor!=NULL)
	{
		// m_pActor->RemoveShaderEffect(vName);
	}
    unguard;
}

void GcActorGraph::ApplyEffect(const char vName[])
{
    guard;
	if(m_pActor!=NULL)
	{
		m_pActor->ApplyEffect(vName);
	}
    unguard;
}

void GcActorGraph::RemoveEffect(const char vName[])
{
    guard;
	if (m_pActor!=NULL)
	{
		m_pActor->RemoveEffect(vName);
	}
    unguard;
}

RtgAABB* GcActorGraph::GetBoundingBox()
{
    guard;
	if (m_pActor!=NULL)
	{
		return m_pActor->GetBoundingBox();
	}
	return NULL;
    unguard;
}

void GcActorGraph::FrameMove(unsigned long _estime)
{
    if (!m_pActor)
        return;

    m_pActor->Tick(_estime);
}

void GcActorGraph::Render(RtgDevice& vrDevice,RTGRenderMask vMask,bool vShadow)
{

    if(m_pActor==NULL || !m_bDisplay)
	{
		return ;
	}


    m_pActor->Render(&vrDevice,vMask);

    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {
        (*it).second->Render(vrDevice,vMask,false);
    }
    return;

}

void GcActorGraph::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    guard;
    if(m_pActor==NULL || !m_bDisplay)
    {
        return ;
    }
    m_pActor->RenderHighLight(&inDevice,color,scale);
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {
        (*it).second->RenderHighLight(inCamera,inDevice,color,scale);
    }
    unguard;
}

void GcActorGraph::UnloadSkin(const char *vpFileName)
{
    guard;
	if(m_pActor==NULL)
	{
		return;
	}
	m_pActor->UnloadSkin(vpFileName);
    unguard;
}

void GcActorGraph::LoadSkin(const char *file, bool bDisableMip)
{
    guard;
	if(m_pActor==NULL)
	{
		return;
	}
	m_pActor->LoadSkin(file, bDisableMip);
    unguard;
}

// #define SHADOW_TEST
RtgTextItem* ShadowRenderTarget  = NULL;
RtgTextItem* ShadowFadeOutTex    = NULL;
RtgTextItem* SimpleShadowTexture = NULL;
RtgTextItem* SelectedTexture	  = NULL;


//<rem by fox for memory leak>
RtgCamera *ShadowCamera   = NULL;
//</rem by fox for memory leak>

//<add by fox for memory leak>
// #include <memory>
// using std::auto_ptr;
// auto_ptr<RtgCamera> ShadowCamera;
//<add by fox for memory leak>

const int MAX_CLIP_TRI   = 1024;
RtgVertex3 ClipVer[MAX_CLIP_TRI*3];
short ClipIdx[MAX_CLIP_TRI*3];
int ClipVerNum = 0;
int ClipIdxNum = 0;

void GcActorGraph::RenderDynamicShadow(RtgDevice &vrDevice)
{
    guard;
	if(m_pActor==NULL) return;
	if(!m_pActor->GetCore()->m_bDynamicShadow) return;

	int i;
	bool bFadeOut;
	CRT_ActorInstance *cha = m_pActor;
	RtgDeviceD3D9 *dev = RtGetRender();

	if(!cha->m_bEnableShadow) return;
	// prepare render target
	if(ShadowRenderTarget == 0)
		ShadowRenderTarget = RtGetRender()->GetTextMgr()->CreateFrameBufferTexture(256, 256);
        //dev->m_Texture.CreateTextureRenderTarget(256,256,true);
	if(ShadowRenderTarget == 0)
		return;
	
	if(ShadowFadeOutTex == 0)
		ShadowFadeOutTex = RtGetRender()->GetTextMgr()->CreateTextureFromFile("shadow_alpha");
        //dev->m_Texture.CreateTexture("shadow_alpha",RTGTBT_AUTO, true,1);
	
	if(dev->m_Ability.iMaxMultiTextures < 2 || !ShadowFadeOutTex)
		bFadeOut = false;
	else 
		bFadeOut = true;
    // bFadeOut = false;

	// camera
	if(ShadowCamera == NULL)
		ShadowCamera = RT_NEW RtgCamera;

    RtgAABB *bound = cha->GetBoundingBox();
    float seg;
    seg = bound->vExt.x+bound->vPos.x;
    if(bound->vExt.y+bound->vPos.y > seg)
        seg = bound->vExt.y+bound->vPos.y;
    if(bound->vExt.z+bound->vPos.z > seg)
        seg = bound->vExt.z+bound->vPos.z;
    seg /= 2;

    if (seg==0.f) return;

	// change render target
	RtGetRender()->SetRenderTarget(ShadowRenderTarget);
    //dev->SetRenderTarget2(ShadowRenderTarget,0);
#ifdef SHADOW_TEST
	dev->ClearRenderTarget(false,true,false,0x00ff0000);
#else
	RtGetRender()->ClearRenderTarget(false,true,false,0x00000000);
#endif
	
	RtgVertex3 eye,at;
	ShadowCamera->SetProjParams(DegreeToRadian(12.5f), 1.3, 10.0f, 3000.0f);
	ShadowCamera->SetViewParams(RtgVertex3(cha->m_matrix._30+seg*2*2.5, 
		cha->m_matrix._31+seg*2*2.5,
		cha->m_matrix._32+seg*2*6), 
		RtgVertex3(cha->m_matrix._30+0.0f,cha->m_matrix._31+0.0f,cha->m_matrix._32+seg/2),
		RtgVertex3(0.0f, 0.0f, 1.0f));
	ShadowCamera->UpdateMatrix();

	RtgCamera *src = dev->m_pCamera;
	dev->SetCamera(ShadowCamera);
	dev->SetMatrixFromCamera();

	RtgMatrix16 uv;
	uv  = ShadowCamera->GetViewMatrix();
	uv  = uv * ShadowCamera->GetProjMatrix();

	// prepare draw shadow
	RtgShader sha;
	sha.iNumStage = 1;
	RtgVertex3 color;
	if(bFadeOut)
		color.Set(0.5,0.5,0.5);
	else
		color.Set(0.5,0.5,0.5);
	
	sha.dwTextureFactor = RtgVectorToColor(color,1.0);
	sha.Shaders[0].SetTexture(0);
	sha.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
	sha.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
	sha.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
	sha.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
	dev->SetShader(&sha);

	dev->SetRenderState(RTGRS_Z_TEST,FALSE);
	dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	// disable fog
	BOOL fogEnable = dev->GetFogEnable();
	dev->SetFogEnable(false);
	
	// render shadow
	cha->RenderShadow(dev);

	// GetDevice()->Blur(ShadowRenderTarget);

	// restore render target
	dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);

	dev->RestoreRenderTarget();
	dev->SetCamera(src);
	dev->SetMatrixFromCamera();

	// prepare draw shadow
	sha.bTwoSide = true;
	if(bFadeOut)
	{
		sha.iNumStage = 2;
		sha.eBlendSrcFunc = RTGBLEND_ZERO;
		sha.eBlendDstFunc = RTGBLEND_INVSRCCOLOR;
		sha.Shaders[0].SetTexture(ShadowRenderTarget);
		sha.Shaders[0].eColorOp    = RTGTOP_SELECTARG1;
		sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE; 
		// sha.Shaders[0].eAlphaOp    = RTGTOP_SELECTARG1;
		// sha.Shaders[0].eAlphaArg1  = RTGTA_TEXTURE;
		sha.Shaders[0].bUseTextureMatrix = false;
		sha.Shaders[1].SetTexture(ShadowFadeOutTex);
		sha.Shaders[1].eColorOp    = RTGTOP_MODULATE;
		sha.Shaders[1].eColorArg1  = RTGTA_CURRENT; 
		sha.Shaders[1].eColorArg2  = RTGTA_TEXTURE;
		// sha.Shaders[1].eAlphaOp    = RTGTOP_SELECTARG1;
		// sha.Shaders[1].eAlphaArg1  = RTGTA_CURRENT;
		sha.Shaders[1].bUseTextureMatrix = false;
	}
	else
	{
		sha.iNumStage = 1;
		sha.eBlendSrcFunc = RTGBLEND_ZERO;
		sha.eBlendDstFunc = RTGBLEND_INVSRCCOLOR;
		sha.Shaders[0].SetTexture(ShadowRenderTarget);
		sha.Shaders[0].eColorOp    = RTGTOP_SELECTARG1;
		sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE; 
		sha.Shaders[0].eAlphaOp    = RTGTOP_SELECTARG1;
		sha.Shaders[0].eAlphaArg1  = RTGTA_CURRENT;
		sha.Shaders[0].bUseTextureMatrix = false;
	}
	sha.bEnableBlendFunc = true;

#ifdef SHADOW_TEST
	sha.eBlendSrcFunc = RTGBLEND_ONE;
	sha.eBlendDstFunc = RTGBLEND_ONE;
#else
	
#endif
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
	dev->SetShader(&sha);
	
	if(dev->m_Ability.bTextureAddressBorder)
        dev->SetTextureAddress(RTGTADD_BORDER);
	else
		dev->SetTextureAddress(RTGTADD_CLAMP);

#ifdef SHADOW_TEST
	dev->SetTextureBorderColor(0x00ffffff);
#else
	dev->SetTextureBorderColor(0x00000000);
#endif

	// draw
	RtgVertex4 v4;
	RtgVertexVT2 v;
	seg *= 2;
	
	RtgAABB aabb;
    rtgV3MultiplyM12(aabb.vPos,aabb.vPos,m_pActor->m_matrix);
	// aabb.vPos.x += m_pActor->m_matrix._30;
	// aabb.vPos.y += m_pActor->m_matrix._31;
	aabb.vPos.z = m_pActor->m_matrix._32;
	aabb.vExt.Set(seg,seg,seg);

	// try clip scene
	if(!ClipScene(aabb)) return;
   
	//DWORD sVB,sIB;
	BYTE *vb,*ib;
	
	// update vb
    RtgShareBufferItem _vbItem;
	vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(ClipVerNum*sizeof(RtgVertexVT2), &_vbItem);

	for(i=0; i<ClipVerNum; i++)
	{
		v4.Set(ClipVer[i].x,ClipVer[i].y,ClipVer[i].z,1);
		v4 = v4 * uv;
		v.t0.Set(v4.x/v4.w/2+0.5,1-(v4.y/v4.w/2+0.5));
		v.t1.Set(v4.x/v4.w/2+0.5,1-(v4.y/v4.w/2+0.5));
		v.v.Set(ClipVer[i].x,ClipVer[i].y,ClipVer[i].z+1);
		memcpy(vb,&v,sizeof(RtgVertexVT2));
		vb += sizeof(RtgVertexVT2);
	}
	dev->GetBuffMgr()->Unlock(&_vbItem);
	dev->SetVertexFormat(RTG_VT2);

	// upload ib
    RtgShareBufferItem _ibItem;
	ib = (BYTE*)dev->GetBuffMgr()->Lock_Shared_ib( ClipIdxNum*sizeof(short), &_ibItem);        
	memcpy(ib,ClipIdx,ClipIdxNum*sizeof(short));
	dev->GetBuffMgr()->Unlock(&_ibItem);

	RtgMatrix16 m1;
	m1.Unit();
	dev->SetMatrix(RTGTS_WORLD,&m1);

	// SetFogWithAdd(dev);
	// draw
    dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
    dev->SetFogEnable(FALSE);
    dev->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        &_vbItem,
        sizeof(RtgVertexVT2),
        &_ibItem,
        ClipIdxNum / 3);
    /*
    dev->SetStreamSource(0, dev->GetBuffMgr()->GetShared_vb(), sizeof(RtgVertexVT2));
    dev->SetIndices(dev->GetBuffMgr()->GetShared_ib());
    dev->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        0,
        ClipVerNum,
        0,
        ClipIdxNum/3);
        */
	dev->SetTextureAddress(RTGTADD_WRAP);
	dev->SetRenderState(RTGRS_LIGHTING,TRUE);
	dev->SetFogEnable(TRUE);
	// RestoreFog(dev);
	dev->SetFogEnable(fogEnable);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);


    unguard;
}

bool GcActorGraph::ClipScene(RtgAABB &aabb)
{
    guard;
	int verNum = MAX_CLIP_TRI*3;
	int idxNum = MAX_CLIP_TRI*3;

	if(g_pScene->TakeGeometry(aabb,ClipVer,verNum,ClipIdx,idxNum))
	{
		ClipVerNum = verNum;
		ClipIdxNum = idxNum;
        if(ClipVerNum!=0)
        {
            return true;
        }
	}

	// make poly
	RtgVertex3 v3;
	int i=0;
	ClipVer[i++].Set(-aabb.vExt.x+aabb.vPos.x, aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(0+aabb.vPos.x, aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(aabb.vExt.x+aabb.vPos.x, aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(-aabb.vExt.x+aabb.vPos.x, 0+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(0+aabb.vPos.x, 0+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(aabb.vExt.x+aabb.vPos.x, 0+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(-aabb.vExt.x+aabb.vPos.x, -aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(0+aabb.vPos.x, -aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVer[i++].Set(aabb.vExt.x+aabb.vPos.x, -aabb.vExt.y+aabb.vPos.y, aabb.vPos.z);
	ClipVerNum = i;
			
	short idx[24] = {0,3,4, 0,4,1, 1,4,5, 1,5,2, 3,6,7, 3,7,4, 4,7,8, 4,8,5};
	memcpy(ClipIdx,idx,24*sizeof(short));
	ClipIdxNum = 24;
	return true;
    unguard;
}

void GcActorGraph::RenderFloatTexture(RtgDevice &_dev,RtgTextItem* texItem,int alphaType,RtgVertex3 &color,float radius)
{
    RtgDeviceD3D9& dev = *RtGetRender();

    if (!texItem || texItem->state != Object_State_Ready)
        return;

	// draw
	RtgVertex4 v4;
	RtgVertexVT2 v;

	// camera
	if(ShadowCamera == NULL)
		ShadowCamera = RT_NEW RtgCamera;

	RtgAABB *bound = m_pActor->GetStaticBoundingBox();
	float seg;
	seg = bound->vExt.x;
	if(bound->vExt.y > seg)
		seg = bound->vExt.y;
	if (seg==0.f) return;
	
	RtgAABB aabb;
	aabb = *bound;
	rtgV3MultiplyM12(aabb.vPos,aabb.vPos,m_pActor->m_matrix);
	aabb.vPos.x  = m_pActor->m_matrix._30;
	aabb.vPos.y  = m_pActor->m_matrix._31;
	aabb.vPos.z  = m_pActor->m_matrix._32;
	aabb.vExt.x  = seg * 1.5;
	aabb.vExt.y  = seg * 1.5;
	aabb.vExt.z  = 0;

	aabb.vExt.x = radius;
	aabb.vExt.y = radius;
	aabb.vExt.z = radius;

	// try clip scene
	if(!ClipScene(aabb)) return;

	//DWORD sVB,sIB;
	BYTE *vb,*ib;

	// update vb
    RtgShareBufferItem _vbItem;
	vb = (BYTE*)dev.GetBuffMgr()->Lock_Shared_vb(ClipVerNum*sizeof(RtgVertexVT2), &_vbItem);
        

	for(int i=0; i<ClipVerNum; i++)
	{
		v.t0.Set((aabb.vExt.x - aabb.vPos.x + ClipVer[i].x)/(aabb.vExt.x*2),
			(aabb.vExt.y - aabb.vPos.y + ClipVer[i].y)/(aabb.vExt.y*2));
		v.v.Set(ClipVer[i].x,ClipVer[i].y,ClipVer[i].z+2);
		memcpy(vb,&v,sizeof(RtgVertexVT2));
		vb += sizeof(RtgVertexVT2);
	}

	dev.GetBuffMgr()->Unlock(&_vbItem);
	dev.SetVertexFormat(RTG_VT2);

	RtgShader sha;
	sha.dwTextureFactor = RtgVectorToColor(color,1);
	sha.iNumStage = 1;

	switch(alphaType)
	{
		case ALPHA_MODULATE:
			sha.eBlendSrcFunc = RTGBLEND_ZERO;
			sha.eBlendDstFunc = RTGBLEND_SRCCOLOR;
			sha.Shaders[0].eColorOp    = RTGTOP_SELECTARG1;
			sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE; 
			dev.SetFogEnable(FALSE);
			break;
			
		case ALPHA_ADD:
			sha.eBlendSrcFunc = RTGBLEND_ONE;
			sha.eBlendDstFunc = RTGBLEND_ONE;
			sha.Shaders[0].eColorOp    = RTGTOP_MODULATE;
			sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE; 
			sha.Shaders[0].eColorArg2  = RTGTA_TFACTOR; 
			SetFogWithAdd(&dev);
			break;
	}

	sha.bEnableBlendFunc = true;
	sha.Shaders[0].SetTexture(texItem);
	sha.Shaders[0].bUseTextureMatrix = false;
	dev.SetShader(&sha);

	// upload ib
    RtgShareBufferItem _ibItem;
	ib = (BYTE*)dev.GetBuffMgr()->Lock_Shared_ib(ClipIdxNum*sizeof(short), &_ibItem);
    
	memcpy(ib,ClipIdx,ClipIdxNum*sizeof(short));
	dev.GetBuffMgr()->Unlock(&_ibItem);

	RtgMatrix16 m1;
	m1.Unit();
	dev.SetMatrix(RTGTS_WORLD,&m1);

    dev.SetRenderState(RTGRS_Z_WRITE,FALSE);
    dev.SetTextureAddress(RTGTADD_CLAMP);
    dev.DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        &_vbItem,
        sizeof(RtgVertexVT2),
        &_ibItem,
        ClipIdxNum / 3);
    /*
    dev.SetStreamSource(0, dev.GetBuffMgr()->GetShared_vb(), sizeof(RtgVertexVT2));
    dev.SetIndices(dev.GetBuffMgr()->GetShared_ib());
    dev.DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
        0,
        0,
        ClipVerNum,
        0,
        ClipIdxNum/3);
	*/

	dev.SetTextureAddress(RTGTADD_WRAP);
	dev.SetRenderState(RTGRS_Z_WRITE,TRUE);
	dev.SetRenderState(RTGRS_LIGHTING,TRUE);
	
	switch(alphaType)
	{
	case ALPHA_MODULATE:
		dev.SetFogEnable(TRUE);
		break;

	case ALPHA_ADD:
		RestoreFog(&dev);
		break;
	}

}

void GcActorGraph::RenderSimpleShadow(RtgDevice &vrDevice,float radius)
{
    guard;
	if(SimpleShadowTexture == 0)
		SimpleShadowTexture = RtGetRender()->GetTextMgr()->CreateTextureFromFile("shadow");
        //vrDevice.m_Texture.CreateTexture("shadow",RTGTBT_AUTO, true,1);
	if(SimpleShadowTexture == 0)
		return;

	RenderFloatTexture(vrDevice,SimpleShadowTexture,ALPHA_MODULATE,RtgVertex3(1,1,1),radius);
    unguard;
}

void GcActorGraph::RenderShadow(RtgDevice &vrDevice, int bDynamicShadow, float radius)
{
    guard;
	if (m_pActor == NULL) 
        return;
	if(m_pActor->IsEnableDynShadow() && m_pActor->m_bEnableShadow)
	{
        if (bDynamicShadow)
        {
            RenderDynamicShadow(vrDevice);
        }else
        {
            RenderSimpleShadow(vrDevice,radius);
        }
	}
    unguard;
}

void GcActorGraph::RenderSelect(RtgDevice &vrDevice,RtgVertex3 &color,float radius)
{
    guard;
	if(SelectedTexture == 0)
		SelectedTexture = RtGetRender()->GetTextMgr()->CreateTextureFromFile("select");
        //vrDevice.m_Texture.CreateTexture("select",RTGTBT_AUTO, true,1);
	if(SelectedTexture == 0)
		return;

	RenderFloatTexture(vrDevice,SelectedTexture,ALPHA_ADD,color, radius);
    unguard;
}

bool GcActorGraph::HaveLink()
{
    guard;
    return m_mapChildren.size()!=0;
    unguard;
}

void GcActorGraph::SetBaseColor(RtgVertex3& vrColor)
{
    guard;
	if(m_pActor==NULL)
	{
		return;
	}
	m_pActor->SetBaseColor(vrColor);

    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {
        (*it).second->SetBaseColor(vrColor);
    }
    unguard;
}

void GcActorGraph::EnableShadow(bool vEnable)
{
    guard;
	if(m_pActor==NULL)
	{
		return ;
	}
	m_pActor->m_bEnableShadow = vEnable;
    unguard;
}

const char* GcActorGraph::FileName()
{
    guard;
	if (m_pActor==NULL)
	{
        return "";
	}
	return m_pActor->GetCore()->m_poTag.c_str();
    unguard;
}

bool GcActorGraph::PlayPose(const char vpPoseName[],bool vLoop, float fSpeed)
{
    guard;
	if(m_pActor==NULL)
	{
		return true;
	}
	return m_pActor->PlayPose(vpPoseName, vLoop, fSpeed);
    unguard;
}

void GcActorGraph::DisplayActor(bool vDisplay)
{
    guard;
	if(m_pActor==NULL)
	{
		return;
	}
	m_bDisplay = vDisplay;			//是否显示
    m_pActor->Enable(m_bDisplay, m_bDisplay);
	//m_pActor->SetValid(m_bDisplay); //设置底层图形是否失效

    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {
        (*it).second->DisplayActor(vDisplay);
    }
    unguard;
}

bool GcActorGraph::LoadActor(const char* szActorName)
{
    guard;
    CHECK(m_pActor==NULL); // 如果原来已经有了，请使用ReplaceActor替换当前的模型
    m_pActor = RtcGetActorManager()->CreateActor(szActorName);
    if (m_pActor==NULL)
    {
        m_pActor = RtcGetActorManager()->CreateActor("mon005");
        LOG1("错误: 载入模型文件[%s]失败, 使用默认模型\n", szActorName);
        if (m_pActor==NULL)
        {
#ifdef _DEBUG
            rt2_sprintf(g_strStaticBuffer, "载入模型文件[%s]失败\n", szActorName);
            CHECKEX(g_strStaticBuffer);
#endif
            return false;
        }
    }

    if (m_pActor)
    {
      /*  if(!m_pActor->Create())
        {
            ShowErr(R(AMSG_CREATE_ACTOR_FAIL));
            LOG1("错误:模型[%s]创建失败\n", m_pActor->m_core->m_poTag.c_str());
            return false;
        }*/
        this->DisplayActor(m_bDisplay);
    }
    return true;
    unguard;
}

void GcActorGraph::UnloadAllSkin()
{
    if (m_pActor)
        m_pActor->UnloadAllSkin();
}

// 替换当前的模型，并且不改变Link的关系
bool GcActorGraph::ReplaceActor(const char* szActorName)
{
    guard;
    if (m_pActor)// 如果当前有演员对象
    {
		if (m_pActor->GetCore())// 如果当前演员对象有核心对象
		{
			char buf[100];
            // 将核心对象的标签字符串拷贝到 buf 中，限制为 100 个字符
			rt2_strncpy(buf, m_pActor->GetCore()->m_poTag.c_str(), 100);
			//buf[99] = 0;
            // 查找字符 '.' 并将其替换为 '\0'，以分离标签的名称部分
			char* p = strchr(buf, '.');
			if (p) *p = 0;
            // 如果当前演员的标签与要替换的演员名称相同，则返回 true
			if (stricmp(buf, szActorName)==0)
			{
				return true;
			}
		}
        // 如果标签不匹配或核心对象为空，释放当前演员对象
        RtcGetActorManager()->ReleaseActor(m_pActor);
		
        m_pActor = NULL;
    }
    // 加载新演员对象
    LoadActor(szActorName);
    // 遍历子演员图谱
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
    for (; it!=m_mapChildren.end(); it++)
    {// 将新演员对象链接到每个子演员图谱
        (*it).second->m_pActor->LinkParent(m_pActor, (*it).first.c_str());
    }
    return true;// 成功替换演员
    unguard;
}

bool GcActorGraph::Link(const char* szLinkName, const char* szActorName)
{
    // 参数说明：
    // szLinkName - link点名称，表示要链接的link点的名称。如果为空，则使用默认名称 "none"。
    // szActorName - actor名称，表示要链接的模型的名称。如果为空，则不进行链接操作。

    guard;
    if (m_pActor==NULL) return false;

    GcActorGraph* pChild;
    std::string strLinkName;
    if (szLinkName) strLinkName = szLinkName;
    else            strLinkName = "none";


    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.find(strLinkName);
    if (it!=m_mapChildren.end())
    {
        pChild = (*it).second;
        if (strcmp(szActorName, pChild->m_pActor->GetCore()->m_poTag.c_str())==0)
        {
            return true; // 已经存在
        }
        if (!UnLink(strLinkName.c_str()))//卸载之前的出错
        {
            return false;
        }
    }
    if (szActorName && szActorName[0])//模型名称有效
    {
        pChild = RT_NEW GcActorGraph;
        if (!pChild->LoadActor(szActorName))
        {
            DEL_ONE(pChild);
            return false;
        }
        if (pChild->m_pActor->LinkParent(this->m_pActor, szLinkName))
        {
            RtgMatrix12 _m12;
            _m12.Unit();
            pChild->m_pActor->SetMatrix(_m12);
        }else
        {
            DEL_ONE(pChild);
            return false;
        }
        pChild->DisplayActor(m_bDisplay);
        m_mapChildren[strLinkName] = pChild;
        return true;
    }
    return false;
    unguard;
}

bool GcActorGraph::UnLink(const char* szLinkName)
{
    guard;
    if(m_pActor==NULL) return false;
    GcActorGraph* pChild;
    std::string strLinkName(szLinkName);
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.find(strLinkName);
    if (it!=m_mapChildren.end())
    {
        pChild = (*it).second;
        m_mapChildren.erase(it);
        pChild->m_pActor->UnlinkParent();
        DEL_ONE(pChild);
        return true;
    }
    return false;
    unguard;
}

bool GcActorGraph::IsLink(const char* szLinkName)
{
    guard;
    if(m_pActor==NULL) return false;
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.find(szLinkName);
    return (it!=m_mapChildren.end());
    unguard;
}

const char* GcActorGraph::GetLinkActorName(const char* szLinkName)
{
    guard;
    if(m_pActor==NULL) return NULL;
    GcActorGraph* pChild;
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.find(szLinkName);
    if (it!=m_mapChildren.end())
    {
        pChild = (*it).second;
        return pChild->m_pActor->GetCore()->m_poTag.c_str();
    }
    return NULL;
    unguard;
}

GcActorGraph* GcActorGraph::GetLinkActor(const char* szLinkName)
{
    guard;
    if(m_pActor==NULL) return NULL;
    GcActorGraph* pChild;
    std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.find(szLinkName);
    if (it!=m_mapChildren.end())
    {
        pChild = (*it).second;
        return pChild;
    }
    return NULL;
    unguard;
}

void GcActorGraph::SetVisible(float visible)
{
	guard;

	if (m_pActor)
		m_pActor->SetVisible(visible);

	std::map<std::string, GcActorGraph*>::iterator it = m_mapChildren.begin();
	for (; it!=m_mapChildren.end(); it++)
	{
		(*it).second->SetVisible(visible);
	}

	unguard;
}

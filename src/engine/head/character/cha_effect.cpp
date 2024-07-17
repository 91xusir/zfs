
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_effect.cpp
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#include "character/actor.h"

RT_IMPLEMENT_DYNCREATE(CRT_EffectEmitter,   CRT_EffectBase, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectRibbon,    CRT_EffectBase, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectRibbon2,   CRT_EffectBase, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_Effect_Dynamic_Ribbon, CRT_EffectBase, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EffectBlur,      CRT_EffectBase, NULL,"")

CRT_EffectEmitter::CRT_EffectEmitter()
    : m_pActive(NULL), 
      m_pDead(NULL),
      m_bUseParentRotate(false),
      m_bUseParentCoor(false),
      m_parLife(0),
      m_emiSpeed(0.f),
      m_midVal(0.5f),
      m_opaqueBeg(1.f),
      m_opaqueMid(1.f),
      m_opaqueEnd(1.f),
      m_colorBeg(1.f, 1.f, 1.f),
      m_colorMid(1.f, 1.f, 1.f),
      m_colorEnd(1.f, 1.f, 1.f),
      m_scaleBeg(1.f),
      m_scaleMid(1.f),
      m_scaleEnd(1.f),
      m_sizeMin(5.f),
      m_sizeMax(5.f),
      m_delta(0),
      m_bAlwaysFront(false),
      m_speedMin(0.f, 0.f, 0.f),
      m_speedMax(0.f, 0.f, 0.f),
      m_accMin(0.f, 0.f, 0.f),
      m_accMax(0.f, 0.f, 0.f),
      m_texBeg(1),
      m_texMid(1),
      m_texEnd(1),
      m_texUSub(1),
      m_texVSub(1),
      m_bTexSub(false),
      m_bTexRand(false),
      m_bSquirt(false),
      m_emiSizeMin(0.f, 0.f, 0.f),
      m_emiSizeMax(0.f, 0.f, 0.f),
      m_destSpeedRand(0),
      m_destTime(0),
      m_boardOpt("Full"),
      m_alphaOpt("Add"),
      m_typeOpt("扩散型"),
      m_lastFrame(0)
{
	CM_MEMPROTECTOR(m_texPath, Text_File_Path_Size)
    (*m_texPath) = 0;
}

void CRT_EffectEmitter::Destroy()
{
    SParticle* p = m_pActive;
    SParticle* t = NULL;

	while (p)
	{
		t = p;
		p = t->Next;
		DEL_ONE(t);
	}

	p = m_pDead;
	
    while(p)
	{
		t = p;
		p = t->Next;
		DEL_ONE(t);
	}

    RtGetRender()->DeleteShader(&m_ro);
}

void CRT_EffectEmitter::OnLoadFinish()
{
    ConvertOption();

    m_delta = 0;
    m_rtEnable = true;
    m_rtEmiSpeed = m_emiSpeed;
    m_rtSquirtNum = 0;
    m_ro.bTwoSide = true;
    m_ro.bColorKey = false;
    m_ro.bEnableBlendFunc = false;
    m_ro.bEnableAlphaTest = false;
    m_ro.dwTextureFactor = 0xFFFFFFFF;
    m_ro.Shaders[0].bUseTextureMatrix = false;
    m_ro.bEnableBlendFunc = true;
    m_ro.Shaders[0].eColorOp   = RTGTOP_MODULATE;
    m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
    m_ro.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;
    m_ro.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
    m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;

    if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texPath, m_texture.c_str()))
        m_ro.iNumStage = 1;
    else
        m_ro.iNumStage = 0;
    RTASSERT(strlen(m_texPath) < Text_File_Path_Size);

    switch(m_alphaType)
    {
    case ALPHA_BLEND :
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
        m_ro.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
        m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
        m_ro.Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
        break;

    case ALPHA_ADD :
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;

    case ALPHA_SUB :
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;

    case ALPHA_MODULATE :
        m_ro.eBlendSrcFunc = RTGBLEND_ZERO;
        m_ro.eBlendDstFunc = RTGBLEND_SRCCOLOR;
        break;
    default :
        break;
    }
}

bool CRT_EffectEmitter::Create()
{
    if (m_ro.iNumStage)
        m_ro.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texPath));
	return true;
}

void CRT_EffectEmitter::Reset()
{
    m_pActive = NULL;
    m_pDead = NULL;
    m_lastFrame = 0;
}

void CRT_EffectEmitter::UseFrame(unsigned long frame)
{
	SEmitterKey k;
	if(m_keyList.GetKeyFrame(frame,k))
	{
		m_rtEmiSpeed = k.EmiSpeed;
		m_rtEnable   = k.bEnable;
	}

	// get squirt key	
	m_rtSquirtNum = 0;
	if(m_bSquirt && frame > m_lastFrame)
	{
		SEmitterKey *k = NULL;
		for(int i=m_lastFrame; i<frame; i++)	
		{
			k = m_keyList.GetKeyFrame(i);
			if(k) break;
		}
		if(k)
			m_rtSquirtNum = k->EmiSpeed;
	}
	m_lastFrame = frame;
}

void CRT_EffectEmitter::Tick(float deltaMill)
{
	m_delta += deltaMill;

    RtgMatrix12 _parentMat;
	RtgMatrix12 _mat;

	switch (m_type)
	{
		case PAR_TYPE_DIF :
            {
			    if (m_actor->GetBoneMat(&_parentMat, m_parent.c_str()))
			    {
				    if (!m_bUseParentCoor)
				    {
					    _mat = _parentMat * (*m_actor->GetWorldMatrix());
	
					    if (m_actor->m_bDrawScale)
					    {
						    RtgMatrix12 scale;
						    scale.Unit();
						    scale._00 = m_actor->m_drawScale.x;
						    scale._11 = m_actor->m_drawScale.y;
						    scale._22 = m_actor->m_drawScale.z;
						    _mat = scale * _mat;
					    }
				    }
                    else
                        _mat.Unit();
			    }
			    else
			    {
				    if (!m_bUseParentCoor)
					    _mat = *m_actor->GetWorldMatrix();
                    else
                        _mat.Unit();
			    }
            }
			break;

		case PAR_TYPE_COM :
            {
			    if (m_actor->GetBoneMat(&_parentMat, m_dest.c_str()))
				    _mat = _parentMat * (*m_actor->GetWorldMatrix());
			    else 
				    _mat = *m_actor->GetWorldMatrix();
            }
			break;

        default :
            {
                _mat.Unit();
            }
            break;
	}
	
	SParticle *t = NULL;
	int num = 0;
	if(m_bSquirt)
	{
		num = m_rtSquirtNum;
	}
	else 
	{
		num = m_delta/1000.0f*m_rtEmiSpeed;
	}	

	if(!m_rtEnable) m_delta = 0;
	// m_delta = 0;
	if(num > 100) num = 100; // max to 100
	if(num > 0 && m_rtEnable)
	{
		m_delta = 0;
	    while(num--)
		{
			t = NewParticle();
			t->Size  = m_sizeMin +  (m_sizeMax-m_sizeMin)*(rand()%100/100.0f);
			t->Pos.x = _mat._30 + m_emiSizeMin.x + (m_emiSizeMax.x-m_emiSizeMin.x)*(rand()%100/100.0f);
			t->Pos.y = _mat._31 + m_emiSizeMin.y + (m_emiSizeMax.y-m_emiSizeMin.y)*(rand()%100/100.0f);
			t->Pos.z = _mat._32 + m_emiSizeMin.z + (m_emiSizeMax.z-m_emiSizeMin.z)*(rand()%100/100.0f);

			switch(m_type)
			{
				case PAR_TYPE_DIF:
					t->Speed.x = m_speedMin.x + (m_speedMax.x-m_speedMin.x)*(rand()%100/100.0f);
					t->Speed.y = m_speedMin.y + (m_speedMax.y-m_speedMin.y)*(rand()%100/100.0f);
					t->Speed.z = m_speedMin.z + (m_speedMax.z-m_speedMin.z)*(rand()%100/100.0f);

					t->Acc.x   = m_accMin.x + (m_accMax.x-m_accMin.x)*(rand()%100/100.0f);
					t->Acc.y   = m_accMin.y + (m_accMax.y-m_accMin.y)*(rand()%100/100.0f);
					t->Acc.z   = m_accMin.z + (m_accMax.z-m_accMin.z)*(rand()%100/100.0f);
					if(m_bUseParentRotate)
						rtgV3MultiplyM9(t->Speed.m_v,t->Speed.m_v,_mat.m_m);
					break;

				case PAR_TYPE_COM:
					t->Acc.Zero();
					t->Speed.x = (_mat._30-t->Pos.x)/(m_destTime/1000.0f);
					t->Speed.y = (_mat._31-t->Pos.y)/(m_destTime/1000.0f);
					t->Speed.z = (_mat._32-t->Pos.z)/(m_destTime/1000.0f);
					if(m_destSpeedRand!=0)
					{
						float b = 1-(rand()%m_destSpeedRand)/100.0f;
						t->Speed.x *= b; 
						t->Speed.y *= b;
						t->Speed.z *= b;
					}
					break;
			}
			if(m_bTexSub)
			{
				if(m_bTexRand)
					t->TexIdx = (rand()%m_texUSub*m_texVSub) + 1;
			}
			t->Next = m_pActive;
			m_pActive = t;
		}
	}

	// update active particle
	SParticle *p = m_pActive,*prev = NULL;
	while(p)
	{
		// delete life over particle
		if(p->Life >= m_parLife) 
		{
			if(prev)
				prev->Next = p->Next;
			else
				m_pActive = p->Next;
			t = p;
			p = p->Next;
			DelParticle(t);
			continue;
		}
		p->Life += (UINT)deltaMill;
		p->Speed += deltaMill/1000.0f * p->Acc;
		p->Pos += deltaMill/1000.0f * p->Speed;

		if(p->Life <= m_midVal*m_parLife)
		{
			float b = p->Life/(m_midVal*m_parLife); 
			p->Opaque = m_opaqueBeg + b*(m_opaqueMid - m_opaqueBeg);
			p->Color  = m_colorBeg  + b*(m_colorMid  - m_colorBeg);
			p->Scale  = m_scaleBeg  + b*(m_scaleMid  - m_scaleBeg);
			if(m_bTexSub)
			{
				if(m_bTexRand)
	                p->TexIdx = rand()%(m_texUSub*m_texVSub) + 1;	// ???
				else
					p->TexIdx = m_texBeg + b*(m_texMid - m_texBeg);
			}
		}
		else
		{
			float b = (p->Life-m_midVal*m_parLife)/((1-m_midVal)*m_parLife); 
			p->Opaque = m_opaqueMid + b*(m_opaqueEnd - m_opaqueMid);
			p->Color  = m_colorMid  + b*(m_colorEnd - m_colorMid);
			p->Scale  = m_scaleMid  + b*(m_scaleEnd - m_scaleMid);
			if(m_bTexSub)
			{
				if(m_bTexRand)
					p->TexIdx = (rand()%m_texUSub*m_texVSub) + 1; // ???
				else
					p->TexIdx = m_texMid + b*(m_texEnd - m_texMid);
			}
		}
		prev = p;
		p = p->Next;
	}
}

void CRT_EffectEmitter::Render(RtgDevice *_dev,RTGRenderMask mask)
{
    if (!ResourceReady())
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

	RtgMatrix16 world;
    RtgMatrix12 _parentMat;

	SParticle *p = m_pActive;
	RtgVertexVCT v[4];
	short idxMod[6] = {3,1,0,0,2,3};
	short idx[6];

	RtgVertex3 offset;
	BYTE *vb,*ib;
	unsigned int i;

	DWORD Light = dev->GetRenderState(RTGRS_LIGHTING);
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	dev->SetShader(&m_ro);
	dev->SetVertexFormat(RTG_VCT);
	
	if(m_alphaType == ALPHA_ADD)
		SetFogWithAdd(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		SetFogWithAdd(dev);
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
	}

	DWORD op;
	op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
	dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	if(m_bAlwaysFront)
        dev->SetRenderState(RTGRS_Z_TEST,FALSE);

	int batchNum = 100,cnt = 0;
	RtgMatrix12 m;
	m.Unit();
	if(m_bUseParentRotate)
	{
		if (m_actor->GetBoneMat(&_parentMat, m_parent.c_str()))
			m = _parentMat * (*m_actor->GetWorldMatrix());
		else 
			m = *m_actor->GetWorldMatrix();
		
		/*
		if(m_actor->m_bDrawScale)
		{
            RtgMatrix16 scale;
			scale.Unit();
			scale._00 = m_actor->m_drawScale.x;
			scale._11 = m_actor->m_drawScale.y;
			scale._22 = m_actor->m_drawScale.z;
			m = scale * m;
		}
		*/
	}

    RtgShareBufferItem vbItem;
    RtgShareBufferItem ibItem;

	while(p)
	{
		if(cnt == 0)
		{
            vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(batchNum*4*sizeof(RtgVertexVCT), &vbItem);
			ib = (BYTE*)dev->GetBuffMgr()->Lock_Shared_ib( batchNum*6*sizeof(short), &ibItem);

		}
		world.Unit();

		switch(m_boardType)
		{
			case BILLBOARD_FULL:
				world = dev->m_pCamera->m_matBillboard;
				break;
			case BILLBOARD_X:
				world.FillRotateY(DegreeToRadian(90));
                if (m_bUseParentRotate)
                {
                    RtgMatrix16 t;
                    t.Set4X3Matrix(m);
                    world = world * t;
                }
				break;
			case BILLBOARD_Y:
                world.FillRotateX(DegreeToRadian(90));
                if (m_bUseParentRotate)
                {
                    RtgMatrix16 t;
                    t.Set4X3Matrix(m);
                    world = world * t;
                }
				break;
			case BILLBOARD_Z:
				break;
			case BILLBOARD_NONE:
				if(m_bUseParentRotate)
					world.Set4X3Matrix(m);
				break;
		}

		world._30 = p->Pos.x;
		world._31 = p->Pos.y;
		world._32 = p->Pos.z;
		switch(m_type)
		{
			case PAR_TYPE_DIF:
				if(m_bUseParentCoor)
				{
					RtgVertex3 vv(world._30,world._31,world._32);
					if (m_actor->GetBoneMat(&_parentMat, m_parent.c_str()))
					{
						RtgMatrix12 m = _parentMat * (*m_actor->GetWorldMatrix());	
						rtgV3MultiplyM12(vv,vv,m.m_m);
					}
					else
						rtgV3MultiplyM12(vv,vv,m_actor->GetWorldMatrix()->m_m);
					world._30 = vv.x;
					world._31 = vv.y;
					world._32 = vv.z;
				}
				break;
		}

		float FinalSize = p->Size;
		v[0].c = RtgVectorToColor(p->Color,p->Opaque);
		if(m_bTexSub) 
		{
			int ui = p->TexIdx % m_texUSub;
			int vi = p->TexIdx / m_texVSub;
			v[0].t.x = ((float)ui) / m_texUSub;
			v[0].t.y = 1-((float)vi) / m_texVSub;
		}
		else
		{
			v[0].t.x = 0.0;
			v[0].t.y = 1-0.0;
		}
		offset.Set(-FinalSize,FinalSize,0);
		v[0].v = offset*p->Scale*world;

		v[1].c = v[0].c;
		if(m_bTexSub)
		{
			int ui = p->TexIdx % m_texUSub + 1;
			int vi = p->TexIdx / m_texVSub;
			v[1].t.x = ((float)ui) / m_texUSub;
			v[1].t.y = 1-((float)vi) / m_texVSub;
		}
		else
		{
			v[1].t.x = 1.0;
			v[1].t.y = 1-0.0;
		}
		offset.Set(FinalSize,FinalSize,0);
		v[1].v = offset*p->Scale*world;

		v[2].c = v[0].c;
		if(m_bTexSub)
		{
			int ui = p->TexIdx % m_texUSub;
			int vi = p->TexIdx / m_texVSub + 1;
			v[2].t.x = ((float)ui) / m_texUSub;
			v[2].t.y = 1-((float)vi) / m_texVSub;
		}
		else
		{
			v[2].t.x = 0.0;
			v[2].t.y = 1-1.0;
		}
		offset.Set(-FinalSize,-FinalSize,0);
		v[2].v = offset*p->Scale*world;

		v[3].c = v[0].c;
		if(m_bTexSub)
		{
			int ui = p->TexIdx % m_texUSub + 1;
			int vi = p->TexIdx / m_texVSub + 1;
			v[3].t.x = ((float)ui) / m_texUSub;
			v[3].t.y = 1-((float)vi) / m_texVSub;
		}
		else
		{
			v[3].t.x = 1.0;
			v[3].t.y = 1-1.0;
		}
		offset.Set(FinalSize,-FinalSize,0);
		v[3].v = offset*p->Scale*world;

		for(i=0; i<4; i++)
		{
			memcpy(vb,&v[i],sizeof(RtgVertexVCT));
			vb += sizeof(RtgVertexVCT);
		}

		for(i=0; i<6; i++)
			idx[i] = idxMod[i] + cnt*4;
		memcpy(ib,idx,6*sizeof(short));
		ib += 6*sizeof(short);
		cnt++;
		if(cnt == batchNum)
		{
			world.Unit();
            dev->GetBuffMgr()->Unlock(&vbItem);
            dev->GetBuffMgr()->Unlock(&ibItem);
            dev->SetMatrix(RTGTS_WORLD,&world);
            dev->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                &vbItem,
                sizeof(RtgVertexVCT),
                &ibItem,
                cnt * 2);
			cnt = 0;
            RtGetPref()->partNum += cnt;
		}
		p = p->Next;
	}

	if(cnt != 0)
	{
		world.Unit();
        dev->GetBuffMgr()->Unlock(&vbItem);
        dev->GetBuffMgr()->Unlock(&ibItem);
        dev->SetMatrix(RTGTS_WORLD,&world);
        dev->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            &vbItem,
            sizeof(RtgVertexVCT),
            &ibItem,
            cnt * 2);
        RtGetPref()->partNum += cnt;
	}

	dev->RestoreShader(&m_ro);
	dev->SetRenderState(RTGRS_LIGHTING,Light);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
	if(m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	if(m_alphaType == ALPHA_ADD)
		RestoreFog(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		RestoreFog(dev);
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
	}
}

void CRT_EffectEmitter::ConvertOption()
{
	m_alphaType = ALPHA_BLEND;
	if(m_alphaOpt == "Blend") m_alphaType = ALPHA_BLEND;
	else if(m_alphaOpt == "Add") m_alphaType = ALPHA_ADD;
	else if(m_alphaOpt == "Sub") m_alphaType = ALPHA_SUB;
	else if(m_alphaOpt == "Modulate") m_alphaType = ALPHA_MODULATE;

	m_boardType = BILLBOARD_FULL;
	if(m_boardOpt == "None") m_boardType = BILLBOARD_NONE;
	else if(m_boardOpt == "Full") m_boardType = BILLBOARD_FULL;
	else if(m_boardOpt == "X") m_boardType = BILLBOARD_X;
	else if(m_boardOpt == "Y") m_boardType = BILLBOARD_Y;
	else if(m_boardOpt == "Z") m_boardType = BILLBOARD_Z;

	m_type = PAR_TYPE_DIF;
	if(m_typeOpt == "扩散型") m_type = PAR_TYPE_DIF;
	else if(m_typeOpt == "收缩型") m_type = PAR_TYPE_COM;
}

long CRT_EffectEmitter::Serialize(RtArchive& Ar)
{
	Ar<<m_typeOpt
	 <<m_parLife 
     <<m_emiSpeed
	 <<m_midVal 
     <<m_bAlwaysFront
	 <<m_opaqueBeg 
     <<m_opaqueMid 
     <<m_opaqueEnd
	 <<m_colorBeg
     <<m_colorMid
     <<m_colorEnd
	 <<m_texBeg 
     <<m_texMid
     <<m_texEnd
	 <<m_scaleBeg
     <<m_scaleMid
     <<m_scaleEnd
	 <<m_texture
	 <<m_texUSub
     <<m_texVSub
     <<m_bTexSub 
     <<m_bTexRand
	 <<m_alphaOpt	
	 <<m_emiSizeMin
     <<m_emiSizeMax
	 <<m_sizeMin
     <<m_sizeMax
	 <<m_bSquirt
	 <<m_boardOpt
	 <<m_keyList
	 <<m_speedMin 
     <<m_speedMax 
     <<m_accMin 
     <<m_accMax
	 <<m_parent 
     <<m_bUseParentRotate 
     <<m_bUseParentCoor
	 <<m_dest 
     <<m_destTime 
     <<m_destSpeedRand;

	BEGIN_VERSION_CONVERT(0)
    CRT_EffectBase::Serialize(Ar);
    BEGIN_VERSION_CONVERT(1)
    END_VERSION_CONVERT
	END_VERSION_CONVERT

	return Ar.IsError() ? 0 : 1;
}


#ifdef CHAR_VIEWER
    void CRT_EffectEmitter::AttachPropGrid(void *p)
    {
        CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
        CXTPPropertyGridItem *pItem,*pSubItem;
        CXTPPropertyGridItemConstraints  *pList;

        pItem = pRoot->AddChildItem(new CCustomItemString("粒子类型",&m_typeOpt));
        pList = pItem->GetConstraints();
        pList->AddConstraint("扩散型");
        pList->AddConstraint("收缩型");
        pItem->SetFlags(xtpGridItemHasComboButton);

        // base prop,all particle have
        pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("基本属性"));
        // pItem->SetReadOnly(TRUE);

        pSubItem = pItem->AddChildItem(new CXTPPropertyGridItemNumber("生命周期(ms)",m_parLife,&m_parLife));
        pSubItem->SetDescription("粒子的生命周期，单位是秒。");
        pSubItem = pItem->AddChildItem(new CCustomItemFloat("重生数量(每秒)",&m_emiSpeed));
        pSubItem->SetDescription("每秒重新释放多少个粒子。");
        pSubItem = pItem->AddChildItem(new CCustomItemBool("爆裂模式",&m_bSquirt));
        pSubItem->SetDescription("是否使用爆裂模式模式，在爆裂模式下，只在关键帧中发射粒子。");
        pItem->AddChildItem(new CCustomItemV3("重生范围(min)",&m_emiSizeMin));
        pItem->AddChildItem(new CCustomItemV3("重生范围(max)",&m_emiSizeMax));
        pItem->AddChildItem(new CCustomItemFloat("边长(min)",&m_sizeMin));
        pItem->AddChildItem(new CCustomItemFloat("边长(max)",&m_sizeMax));

        pItem->AddChildItem(new CCustomItemBool("始终在最前画",&m_bAlwaysFront));
        pSubItem = pItem->AddChildItem(new CCustomItemString("Board模式",&m_boardOpt));
        pSubItem->SetDescription("粒子面朝的方向,Full始终面向镜头。");
        pList = pSubItem->GetConstraints();
        pList->AddConstraint("None");
        pList->AddConstraint("Full");
        pList->AddConstraint("X");
        pList->AddConstraint("Y");
        pList->AddConstraint("Z");
        pSubItem->SetFlags(xtpGridItemHasComboButton);
        pSubItem->Expand();
        pItem->Expand();

        pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("贴图信息"));
        pItem->AddChildItem(new CCustomItemString("贴图",&m_texture));
        pItem->AddChildItem(new CCustomItemBool("贴图动画",&m_bTexSub));
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("U Sub",m_texUSub,&m_texUSub));
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("V Sub",m_texVSub,&m_texVSub));
        pSubItem = pItem->AddChildItem(new CCustomItemBool("随机贴图",&m_bTexRand));
        pSubItem->SetDescription("每次随机选取一个贴图。");
        pSubItem = pItem->AddChildItem(new CCustomItemString("Alpha模式",&m_alphaOpt));
        pList = pSubItem->GetConstraints();
        pList->AddConstraint("Blend");
        pList->AddConstraint("Add");
        pList->AddConstraint("Modulate");
        pList->AddConstraint("Sub");
        pSubItem->SetFlags(xtpGridItemHasComboButton);
        pSubItem->Expand();
        pItem->Expand();

        pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("插值信息"));
        pItem->AddChildItem(new CCustomItemFloat("中间值(0-1)",&m_midVal));
        pItem->AddChildItem(new CCustomItemFloat("透明度(起始)",&m_opaqueBeg));
        pItem->AddChildItem(new CCustomItemFloat("透明度(中间)",&m_opaqueMid));
        pItem->AddChildItem(new CCustomItemFloat("透明度(结束)",&m_opaqueEnd));

        pItem->AddChildItem(new CCustomItemColor3("颜色(起始)",&m_colorBeg));
        pItem->AddChildItem(new CCustomItemColor3("颜色(中间)",&m_colorMid));
        pItem->AddChildItem(new CCustomItemColor3("颜色(结束)",&m_colorEnd));

        pItem->AddChildItem(new CCustomItemFloat("缩放(起始)",&m_scaleBeg));
        pItem->AddChildItem(new CCustomItemFloat("缩放(中间)",&m_scaleMid));
        pItem->AddChildItem(new CCustomItemFloat("缩放(结束)",&m_scaleEnd));

        pItem->AddChildItem(new CXTPPropertyGridItemNumber("贴图(起始)",m_texBeg,&m_texBeg));
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("贴图(中间)",m_texMid,&m_texMid));
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("贴图(结束)",m_texEnd,&m_texEnd));
        pItem->Expand();

        // prop base on type
        if(m_type == PAR_TYPE_DIF)
        {
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("扩散型粒子"));
            pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("父节点",m_parent.c_str()));
            pSubItem->SetFlags(xtpGridItemHasExpandButton);

            pItem->AddChildItem(new CCustomItemBool("使用父节点的旋转",&m_bUseParentRotate));
            pSubItem = pItem->AddChildItem(new CCustomItemBool("使用父节点的坐标系",&m_bUseParentCoor));
            pSubItem->SetDescription("默认使用世界坐标系。");
            pItem->AddChildItem(new CCustomItemV3("加速度(min)",&m_accMin));
            pItem->AddChildItem(new CCustomItemV3("加速度(max)",&m_accMax));
            pItem->AddChildItem(new CCustomItemV3("速度(min)",&m_speedMin));
            pItem->AddChildItem(new CCustomItemV3("速度(max)",&m_speedMax));
            pItem->Expand();
        }
        else if(m_type == PAR_TYPE_COM)
        {
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("收缩型粒子"));
            pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("目标点",m_dest.c_str()));
            pSubItem->SetFlags(xtpGridItemHasExpandButton);
            pItem->AddChildItem(new CXTPPropertyGridItemNumber("飞行时间(ms)",m_destTime,&m_destTime));
            pItem->AddChildItem(new CXTPPropertyGridItemNumber("速度随机(%)",m_destSpeedRand,&m_destSpeedRand));
            pItem->Expand();
        }
        pRoot->Expand();
    }

    bool CRT_EffectEmitter::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
    {
        // bool bRebuild = false;
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
        switch(wParam)
        {
        case XTP_PGN_ITEMVALUE_CHANGED:
            Recreate();
            if(pItem->GetCaption() == "粒子类型")
                return true;
            return false;

        case XTP_PGN_INPLACEBUTTONDOWN:
            if(pItem->GetCaption() == "父节点")
            {
                CDlgGetBone dlg;
                if(dlg.DoModal())
                {
                    m_parent = dlg.m_selectedBone;
                    pItem->SetValue(m_parent.c_str());
                }
            }
            else if(pItem->GetCaption() == "目标点")
            {
                CDlgGetBone dlg;
                if(dlg.DoModal())
                {
                    m_dest = dlg.m_selectedBone;
                    pItem->SetValue(m_dest.c_str());
                }
            }
            Recreate();
            break;
        }
        return false;
    }

    void CRT_EffectEmitter::AttachKeyGrid(int key,void *p)
    {
        CRT_EffectEmitter *s = this;
        CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
        CXTPPropertyGridItem *pItem;

        SEmitterKey *k;
        CString tmp;

        k = m_keyList.GetKeyFrame(key);
        if(!k) return;
        tmp.Format("%d",key);
        pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数",tmp));
        pItem->SetReadOnly(TRUE);
        pRoot->AddChildItem(new CCustomItemBool("有效",&k->bEnable));
        pRoot->AddChildItem(new CCustomItemFloat("重生速度(s)",&k->EmiSpeed));

        pRoot->Expand();
    }

    bool CRT_EffectEmitter::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
    {
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
        switch(wParam)
        {
        case XTP_PGN_ITEMVALUE_CHANGED: break;
        case XTP_PGN_INPLACEBUTTONDOWN: break;
        }
        return false;
    }

    int CRT_EffectEmitter::TVGetKeyNum()
    {
        return m_keyList.Size();
    }

    int CRT_EffectEmitter::TVGetKeyFrame(int key)
    {
        unsigned long frame = 0;
        m_keyList.GetKeyFrame(key,frame);
        return frame;
    }

    void CRT_EffectEmitter::TVOnNotify(ETV_OP op,long param1,long param2)
    {
        CDlgChannelInfo prop;
        CDlgKeyInfo keyInfo;
        SEmitterKey k;
        SEmitterKey *old;

        switch(op) 
        {
        case OP_MODIFY_PROP:
            prop.Init(this);
            prop.DoModal();
            break;

        case OP_MODIFY_KEY:
            keyInfo.Init(param1,this);
            keyInfo.DoModal();
            break;

        case OP_ADD_KEY:
            m_keyList.AddKeyFrame(param1,k);
            keyInfo.Init(param1,this);
            keyInfo.DoModal();
            break;

        case OP_DEL_KEY:
            m_keyList.DelKeyFrame(param1);
            break;

        case OP_MOVE_KEY:
            old = m_keyList.GetKeyFrame(param1);
            if(!old) break;
            m_keyList.AddKeyFrame(param2,*old);
            m_keyList.DelKeyFrame(param1);
            break;	

        case OP_COPY_KEY:
            old = m_keyList.GetKeyFrame(param1);
            if(!old) break;
            m_keyList.AddKeyFrame(param2,*old);
            break;	
        }
    }
#endif


CRT_EffectRibbon::CRT_EffectRibbon()
: 	m_pActive(NULL),
    m_pDead(NULL),
    m_bSrcValid(false),
    m_interval(0),
    m_delta(0),
    m_colorBeg(0.f, 0.f, 0.f),
    m_colorEnd(0.f, 0.f, 0.f),
    m_opaqueBeg(1.f),
    m_opaqueEnd(0.f),
    m_life(0),
    m_rtEnable(true),
    m_bAlwaysFront(false)
{
}

void CRT_EffectRibbon::Destroy()
{
	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		DelPiece(*it);
	}
	m_list.clear();
    RtGetRender()->DeleteShader(&m_ro);
}

long CRT_EffectRibbon::Serialize(RtArchive& Ar)
{
	Ar<<m_head
      <<m_mid
      <<m_tail
	  <<m_life
	  <<m_opaqueBeg
      <<m_opaqueEnd
      <<m_colorBeg
      <<m_colorEnd
	  <<m_interval
	  <<m_alphaOpt
	  <<m_keyList;

	BEGIN_VERSION_CONVERT(0)
    CRT_EffectBase::Serialize(Ar);
    BEGIN_VERSION_CONVERT(0)
    Ar<<m_bAlwaysFront;
    BEGIN_VERSION_CONVERT(1)
    END_VERSION_CONVERT
    END_VERSION_CONVERT
	END_VERSION_CONVERT
	return Ar.IsError() ? 0 : 1;
}

void CRT_EffectRibbon::UseFrame(unsigned long frame)
{
	SRibbonKey k;
	if(m_keyList.GetKeyFrame(frame,k))
	{
		m_rtEnable = k.bEnable;
	}
}

void CRT_EffectRibbon::OnLoadFinish()   
{
    RTASSERT(m_actor);

    ConvertOption();

    m_rtEnable = true;
    m_ro.iNumStage = 1;
    m_ro.bTwoSide  = true;
    m_ro.bColorKey = false;
    m_ro.bEnableAlphaTest = false;
    m_ro.dwTextureFactor = 0xFFFFFFFF;
    m_ro.Shaders[0].bUseTextureMatrix = false;
    if(m_alphaType==ALPHA_NULL)
        m_ro.bEnableBlendFunc = false;
    else
        m_ro.bEnableBlendFunc = true;
    m_ro.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    m_ro.Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
    m_ro.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
    m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    switch(m_alphaType)
    {
    case ALPHA_BLEND:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
        break;

    case ALPHA_ADD:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;
    }
}

bool CRT_EffectRibbon::Create()
{



	return true;
}

void CRT_EffectRibbon::Tick(float deltaMill)
{
    if (!m_actor)
        return;

    RtgMatrix12 _headMat;
    RtgMatrix12 _midMat;
    RtgMatrix12 _tailMat;

    bool bhead = m_actor->GetBoneMat(&_headMat, m_head.c_str());
    bool bmid  = m_actor->GetBoneMat(&_midMat, m_mid.c_str());
    bool btail = m_actor->GetBoneMat(&_tailMat, m_tail.c_str());

	m_delta += deltaMill;
	if(m_delta > m_interval)
	{
		m_delta = 0;
		if(m_rtEnable)
		{
			RtgMatrix12 head,mid,tail;
			if(bhead && bmid && btail)
			{
				head = _headMat*m_actor->m_matrix;
				mid  = _midMat*m_actor->m_matrix;
				tail = _tailMat*m_actor->m_matrix;
			}
			else
			{
				head = m_actor->m_matrix;
				mid  = m_actor->m_matrix;
				tail = m_actor->m_matrix;
			}

			if(deltaMill < 1000 && m_bSrcValid)
			{
				// generate from src pos and cur pos
				SPiece *p = NewPiece();
				p->V[0] = m_srcHeadPos;
				p->V[1] = m_srcMidPos;
				p->V[2] = m_srcTailPos;

				p->V[3] = RtgVertex3(head._30,head._31,head._32);
				p->V[4] = RtgVertex3(mid._30,mid._31,mid._32);
				p->V[5] = RtgVertex3(tail._30,tail._31,tail._32);
				p->Color = m_colorBeg;
				p->Visible = m_opaqueBeg;
				m_list.push_back(p);
			}

			// save src pos
			m_srcHeadPos = RtgVertex3(head._30,head._31,head._32);
			m_srcMidPos  = RtgVertex3(mid._30,mid._31,mid._32);
			m_srcTailPos = RtgVertex3(tail._30,tail._31,tail._32);
			// LOG6("head (%.3f,%.3f,%.3f),mid (%.3f,%.3f,%.3f)\n",
			//	m_srcTailPos.x,m_srcTailPos.y,m_srcTailPos.z,
			//	m_srcMidPos.x,m_srcMidPos.y,m_srcMidPos.z);
			m_bSrcValid = true;
		}
		else
		{
			m_bSrcValid = false;
		}
	}

	// update old piece
	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end();)
	{
		SPiece *p = *it;
		p->Life += deltaMill;
		if(p->Life > m_life)
		{
            it = m_list.erase(it);
			DelPiece(p);
			continue;
		}

		float b = ((float)p->Life)/m_life; 
		p->Visible = m_opaqueBeg + b*(m_opaqueEnd - m_opaqueBeg);
		p->Color   = m_colorBeg + b*(m_colorEnd - m_colorBeg);
		// p->Scale  = m_scaleBeg + b*(m_scaleMid - m_scaleBeg);

		it++;
	}
}

void CRT_EffectRibbon::Render(RtgDevice* _dev,RTGRenderMask mask)
{
	if(m_list.size() == 0) return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

	RtgVertexVCT v;
	short idxMod[12] = {0,1,3, 1,3,4, 2,1,4, 2,4,5};
	short idx[12];

	RtgVertex3 offset;
	BYTE *vb,*ib;
	unsigned int i;

	// don't use lighting on particle
	DWORD Light = dev->GetRenderState(RTGRS_LIGHTING);
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	dev->SetShader(&m_ro);
	dev->SetVertexFormat(RTG_VCT);

	DWORD op;
	op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
	dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	if(m_alphaType == ALPHA_ADD)
		SetFogWithAdd(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		SetFogWithAdd(dev);
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
	}

	if(m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,FALSE);

    RtgShareBufferItem vbItem;
    RtgShareBufferItem ibItem;

    vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(m_list.size()*6*sizeof(RtgVertexVCT), &vbItem);
    ib = (BYTE*)dev->GetBuffMgr()->Lock_Shared_ib(m_list.size()*12*sizeof(short), &ibItem);

	list<SPiece*>::iterator it;
	int cnt = 0;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		SPiece *p = *it;

		for(i=0; i<6; i++)
		{
			v.v = p->V[i];
			v.c = RtgVectorToColor(p->Color,p->Visible);
			memcpy(vb,&v,sizeof(RtgVertexVCT));
			vb += sizeof(RtgVertexVCT);
		}

		// prepare index
		for(i=0; i<12; i++)
		{
			idx[i] = idxMod[i] + cnt*6;
		}
		memcpy(ib,idx,12*sizeof(short));
		ib += 12*sizeof(short);
		cnt++;
	}	

	RtgMatrix16 world;
	world.Unit();
    dev->GetBuffMgr()->Unlock(&vbItem);
    dev->GetBuffMgr()->Unlock(&ibItem);
    dev->SetMatrix(RTGTS_WORLD,&world);
    dev->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        &vbItem,
        sizeof(RtgVertexVCT),
        &ibItem,
        cnt * 4
        );
	dev->RestoreShader(&m_ro);
	dev->SetRenderState(RTGRS_LIGHTING,Light);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
	if(m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	if(m_alphaType == ALPHA_ADD)
		RestoreFog(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
		RestoreFog(dev);
	}
}

void CRT_EffectRibbon::ConvertOption()
{
	m_alphaType = ALPHA_BLEND;
	if(m_alphaOpt == "Blend") m_alphaType = ALPHA_BLEND;
	else if(m_alphaOpt == "Add") m_alphaType = ALPHA_ADD;
	else if(m_alphaOpt == "None") m_alphaType = ALPHA_NULL;
	else if(m_alphaOpt == "Sub") m_alphaType = ALPHA_SUB;
}

#ifdef CHAR_VIEWER
void CRT_EffectRibbon::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPPropertyGridItemConstraints  *pList;

	// pItem = pRoot->AddChildItem(new CCustomItemString("名字",&s->m_name));
	// pItem->SetReadOnly(TRUE);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("节点(head)",m_head.c_str()));
	pItem->SetFlags(xtpGridItemHasExpandButton);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("节点(mid)",m_mid.c_str()));
	pItem->SetFlags(xtpGridItemHasExpandButton);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("节点(tail)",m_tail.c_str()));
	pItem->SetFlags(xtpGridItemHasExpandButton);

	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("生命周期(ms)",m_life,&m_life));
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("采样率(ms)",m_interval,&m_interval));
	pRoot->AddChildItem(new CCustomItemBool("始终在最前画",&m_bAlwaysFront));
	
	pSubItem = pRoot->AddChildItem(new CCustomItemString("Alpha模式",&m_alphaOpt));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("None");
	pList->AddConstraint("Sub");
	pSubItem->SetFlags(xtpGridItemHasComboButton);
	pSubItem->Expand();

	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("插值信息"));
	pItem->AddChildItem(new CCustomItemFloat("透明度(起始)",&m_opaqueBeg));
	pItem->AddChildItem(new CCustomItemFloat("透明度(结束)",&m_opaqueEnd));
	pItem->AddChildItem(new CCustomItemColor3("颜色(起始)",&m_colorBeg));
	pItem->AddChildItem(new CCustomItemColor3("颜色(结束)",&m_colorEnd));

	pItem->Expand();
	pRoot->Expand();
}

bool CRT_EffectRibbon::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			Recreate();
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "节点(head)")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_head = dlg.m_selectedBone;
					pItem->SetValue(m_head.c_str());
				}
			}
			else if(pItem->GetCaption() == "节点(mid)")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_mid = dlg.m_selectedBone;
					pItem->SetValue(m_mid.c_str());
				}
			}
			else if(pItem->GetCaption() == "节点(tail)")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_tail = dlg.m_selectedBone;
					pItem->SetValue(m_tail.c_str());
				}
			}
			Recreate();
			break;
	}
	return false;
}

void CRT_EffectRibbon::AttachKeyGrid(int key,void *p)
{
	CRT_EffectRibbon *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	SRibbonKey *k;
	CString tmp;

	k = m_keyList.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数",tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(new CCustomItemBool("有效",&k->bEnable));
	pRoot->Expand();
}

bool CRT_EffectRibbon::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED: break;
		case XTP_PGN_INPLACEBUTTONDOWN: break;
	}
	return false;
}

int CRT_EffectRibbon::TVGetKeyNum()
{
	return m_keyList.Size();
}

int CRT_EffectRibbon::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyList.GetKeyFrame(key,frame);
	return frame;
}

void CRT_EffectRibbon::TVOnNotify(ETV_OP op,long param1,long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SRibbonKey k;
	SRibbonKey *old;

	switch(op) 
	{
		case OP_MODIFY_PROP:
			prop.Init(this);
			prop.DoModal();
			break;

		case OP_MODIFY_KEY:
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_ADD_KEY:
			m_keyList.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyList.DelKeyFrame(param1);
			break;

		case OP_MOVE_KEY:
			old = m_keyList.GetKeyFrame(param1);
			if(!old) break;
			m_keyList.AddKeyFrame(param2,*old);
			m_keyList.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyList.GetKeyFrame(param1);
			if(!old) break;
			m_keyList.AddKeyFrame(param2,*old);
			break;	
	}
}
#endif

CRT_Effect_Dynamic_Ribbon::CRT_Effect_Dynamic_Ribbon()
    : m_LastFrame(-1),
      m_pNodeHead(NULL),
      m_pNodeTail(NULL),
      m_fSize(10.f),
      m_fLife(10.f),
      m_alphaType(ALPHA_ADD),
      m_alphaOpt("Add"),
      m_bAlwaysFront(true),
      m_nNodeNum(0),
      m_Color(1, 1, 1)
{
	CM_MEMPROTECTOR(m_texPath, Text_File_Path_Size)
}

void CRT_Effect_Dynamic_Ribbon::Destroy()
{
    RtGetRender()->DeleteShader(&m_ro);
    Clear();
}

void CRT_Effect_Dynamic_Ribbon::Clear()
{
    while (m_pNodeHead)
    {
        Node* p = m_pNodeHead;
        m_pNodeHead = m_pNodeHead->next;
        DEL_ONE(p);
    }

    m_pNodeHead = NULL;
    m_pNodeTail = NULL;
    m_nNodeNum = 0;
}

void CRT_Effect_Dynamic_Ribbon::OnLoadFinish()
{
    RTASSERT(m_actor);

    m_ro.bTwoSide  = true;
    m_ro.bColorKey = false;
    m_ro.bEnableAlphaTest = false;
    m_ro.dwTextureFactor = 0xFFFFFFFF;
    m_ro.Shaders[0].bUseTextureMatrix = false;

    if(m_alphaType == ALPHA_NULL)
        m_ro.bEnableBlendFunc = false;
    else
        m_ro.bEnableBlendFunc = true;

    m_ro.Shaders[0].eColorOp   = RTGTOP_MODULATE;
    m_ro.Shaders[0].eColorArg0 = RTGTA_DIFFUSE;
    m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
    m_ro.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
    m_ro.Shaders[0].eAlphaArg0 = RTGTA_DIFFUSE;
    m_ro.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;

    if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texPath, m_texture.c_str()))
        m_ro.iNumStage = 1;
    else
        m_ro.iNumStage = 0;
    RTASSERT(strlen(m_texPath) < Text_File_Path_Size);

    switch(m_alphaType)
    {
    case ALPHA_BLEND:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
        break;

    case ALPHA_ADD:
        m_ro.eBlendSrcFunc = RTGBLEND_ONE;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;

    case ALPHA_SUB:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;
    }
}

bool CRT_Effect_Dynamic_Ribbon::Create()
{
    if (m_ro.iNumStage)
        m_ro.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texPath));
    return true;
}

void CRT_Effect_Dynamic_Ribbon::UseFrame(unsigned long frame) 
{
    if (m_LastFrame == -1)
    {
        m_LastFrame = frame;
        return;
    }

    int df = (int)frame - m_LastFrame;

    if (df <= 0)
    {
        m_LastFrame = frame;

        if (df < 0)
            Clear();

        return;
    }

    Node* pNewHead = NULL;
    Node* p = m_pNodeHead;

    m_nNodeNum = 0;

    while (p)
    {
        if ((p->life -= df) > 0 && !pNewHead)
            pNewHead = p;
        p = p->next;
        ++m_nNodeNum;
    }

    p = m_pNodeHead;

    while (m_pNodeHead != pNewHead)
    {
        p = m_pNodeHead;
        m_pNodeHead = m_pNodeHead->next;
        DEL_ONE(p);
        --m_nNodeNum;
    }

    if (!(m_pNodeHead = pNewHead))
        m_pNodeTail = NULL;

    RtgVertex3 v0 = RtgVertex3(-m_fSize / 2, 0.f, 0.f);
    RtgVertex3 v1 = RtgVertex3( m_fSize / 2, 0.f, 0.f);

    RtgMatrix12 _Mat = m_actor->m_matrix;
    RtgMatrix12 _linkMat;

    if (m_actor->GetBoneMat(&_linkMat, m_linkBone.c_str()))
        _Mat = _linkMat * _Mat;

    rtgV3MultiplyM12(v0, v0, _Mat);
    rtgV3MultiplyM12(v1, v1, _Mat);

    p = RT_NEW Node(v0, v1, m_fLife, NULL);

    if (m_pNodeTail)
    {
        m_pNodeTail->next = p;
        m_pNodeTail = p;
    }
    else
    {
        m_pNodeHead = p;
        m_pNodeTail = p;
    }

    ++m_nNodeNum;

    m_LastFrame = frame;
}

void CRT_Effect_Dynamic_Ribbon::Render(RtgDevice* _dev,RTGRenderMask mask)
{
    if (m_nNodeNum < 2)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    RtgVertexVCT* vb = NULL;
    Node* p = m_pNodeHead;

    DWORD Light = dev->GetRenderState(RTGRS_LIGHTING);
    RTGTextureAddress ta = dev->GetTextureAddress();
    DWORD op;
    RtgMatrix16 world;
    DWORD color;
    float step = 1.f / (m_nNodeNum - 1);

    dev->SetTextureAddress(RTGTADD_CLAMP);
    dev->SetRenderState(RTGRS_LIGHTING,FALSE);
    dev->SetShader(&m_ro);
    dev->SetVertexFormat(RTG_VCT);
    op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
    dev->SetRenderState(RTGRS_Z_WRITE,FALSE);

    switch (m_alphaType)
    {
    case ALPHA_ADD :
        SetFogWithAdd(dev);
        break;
    case ALPHA_SUB :
        SetFogWithAdd(dev);
        dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
        break;
    default :
        break;
    }

    if (m_bAlwaysFront)
        dev->SetRenderState(RTGRS_Z_TEST, FALSE);

    RtgShareBufferItem vbItem;

    vb = (RtgVertexVCT*)dev->GetBuffMgr()->Lock_Shared_vb(m_nNodeNum * 2 *sizeof(RtgVertexVCT), &vbItem);
       
    if (!vb)
        return;

    for (int i = 0; i < m_nNodeNum; ++i)
    {
        color = RtgVectorToColor(m_Color, p->life / m_fLife);
        vb->v = p->v0;
        vb->c = color;
        vb->t.x = 0.f;
        vb->t.y = max(1.f - i * step, 0.f);
        ++vb;
        vb->v = p->v1;
        vb->c = color;
        vb->t.x = 1.f;
        vb->t.y = max(1.f - i * step, 0.f);
        p = p->next;
        ++vb;
    }

    dev->GetBuffMgr()->Unlock(&vbItem);
    world.Unit();
    dev->SetMatrix(RTGTS_WORLD,&world);
    dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, &vbItem, sizeof(RtgVertexVCT), m_nNodeNum * 2 - 2);

    dev->SetTextureAddress(ta);
    dev->RestoreShader(&m_ro);
    dev->SetRenderState(RTGRS_LIGHTING,Light);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
    dev->SetRenderState(RTGRS_Z_WRITE,TRUE);

    if(m_bAlwaysFront)
        dev->SetRenderState(RTGRS_Z_TEST,TRUE);

    switch (m_alphaType)
    {
    case ALPHA_ADD :
        RestoreFog(dev);
        break;

    case ALPHA_SUB :
        dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
        RestoreFog(dev);
        break;

    default :
        break;
    }
}

void CRT_Effect_Dynamic_Ribbon::ResetRibbonDynamic()
{
	Clear();
}

#ifdef CHAR_VIEWER

void CRT_Effect_Dynamic_Ribbon::AttachPropGrid(void *p)
{
    CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
    CXTPPropertyGridItem *pItem,*pSubItem;
    CXTPPropertyGridItemConstraints  *pList;

    pItem = pRoot->AddChildItem(new CCustomItemString("贴图",&m_texture));
    pItem = pRoot->AddChildItem(new CCustomItemColor3("颜色",&m_Color));
    pItem = pRoot->AddChildItem(new CCustomItemFloat("大小",&m_fSize));
    pItem = pRoot->AddChildItem(new CCustomItemFloat("生命",&m_fLife));
    pItem = pRoot->AddChildItem (new CXTPPropertyGridItem("Link", m_linkBone.c_str()));
    pItem->SetFlags(xtpGridItemHasExpandButton);
    pRoot->AddChildItem(new CCustomItemBool("始终在最前画",&m_bAlwaysFront));
    pSubItem = pRoot->AddChildItem(new CCustomItemString("Alpha模式",&m_alphaOpt));
    pList = pSubItem->GetConstraints();
    pList->AddConstraint("Blend");
    pList->AddConstraint("Add");
    pList->AddConstraint("Sub");
    pList->AddConstraint("None");
    pSubItem->SetFlags(xtpGridItemHasComboButton);
    pSubItem->Expand();

    pItem->Expand();
    pRoot->Expand();
}


bool CRT_Effect_Dynamic_Ribbon::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
    CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

    switch(wParam)
    {
    case XTP_PGN_ITEMVALUE_CHANGED:

        if (pItem->GetCaption() == "贴图")
        {
            Recreate();
        }
        if (pItem->GetCaption() == "Alpha模式")
        {   
            if (pItem->GetValue() == "Blend")
            {
                m_alphaType = ALPHA_BLEND;
            }
            else if (pItem->GetValue() == "Add")
            {
                m_alphaType = ALPHA_ADD;
            }
            else if (pItem->GetValue() == "Sub")
            {
                m_alphaType = ALPHA_SUB;
            }
            else 
            {
                m_alphaType = ALPHA_NULL;
            }
            Recreate();
        }
        return false;

    case XTP_PGN_INPLACEBUTTONDOWN:
      
        if (pItem->GetCaption() == "Link")
        {
            CDlgGetBone dlg;
            if(dlg.DoModal())
            {
                m_linkBone = dlg.m_selectedBone;
                pItem->SetValue(m_linkBone.c_str());
            }
        }

        break;
    }
    return false;
}

#endif


CRT_EffectRibbon2::CRT_EffectRibbon2()
    : m_bAlwaysFront(false),
      m_lastFrame(0),
      m_pieceNum(0)
{
	CM_MEMPROTECTOR(m_texPath, Text_File_Path_Size)
}

void CRT_EffectRibbon2::Destroy()
{
    RtGetRender()->DeleteShader(&m_ro);

	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		DelPiece(*it);
	}
	m_list.clear();
}

void CRT_EffectRibbon2::GenTri(int frame,SPiece *p)
{
    // done


	if (!m_actor || !m_actor->GetCore()) 
        return;

	SRT_Bone *head=NULL,*tail=NULL;
	int find = 0;
	for(int i=0; i<m_actor->m_bones.size() && find<2; i++)
	{
		SRT_Bone *tmp = &m_actor->m_bones[i];
		if(tmp->Name == m_head.c_str())
		{
			head = tmp;
			find++;
		}
		else if(tmp->Name == m_tail.c_str())
		{
			tail = tmp;
			find++;
		}
	}
}

void CRT_EffectRibbon2::UseFrame(unsigned long frame)
{
	
	if(frame == m_lastFrame) return;
	SRibbonKey2 *k = NULL;
	if(frame > m_lastFrame)
	{
		for(int i=m_lastFrame; i<frame; i++)	
		{
			k = m_keyList.GetKeyFrame(i);
			if(k) break;
		}
	}
	
	if(k)
	{
		if(frame < k->endFrame)
		{
			// generate a new ribbon
			SPiece *p   = NewPiece();
			p->keyFrame = *k;
			p->begFrame = frame;
			p->endFrame = k->endFrame;
			m_list.push_back(p);
		}
	}

	// update all piece here, not in tick
	list<SPiece*>::iterator it;
	if(frame < m_lastFrame)
	{

		//<add by fox for memory leak>
		list<SPiece*>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); it++)
		{
			DelPiece(*it);
		}
		//</add by fox for memory leak>
		m_list.clear();
	}

	if(frame > m_lastFrame && frame < m_lastFrame + 100)
	{
		for(int i=m_lastFrame; i<frame; i++)
		{
			for(it=m_list.begin(); it!=m_list.end();)
			{
				SPiece *p = *it;
				if(p->endFrame < i)
				{
					it = m_list.erase(it);
					DelPiece(p);
					continue;
				}
				GenTri(i,p);
				it++;
			}
		}
	}
	m_lastFrame = frame;
}

void CRT_EffectRibbon2::OnLoadFinish()
{
    ConvertOption();

    m_ro.bTwoSide  = true;
    m_ro.bColorKey = false;
    m_ro.bEnableAlphaTest = false;
    m_ro.dwTextureFactor = 0xFFFFFFFF;
    m_ro.Shaders[0].bUseTextureMatrix = false;

    if(m_alphaType==ALPHA_NULL)
        m_ro.bEnableBlendFunc = false;
    else
        m_ro.bEnableBlendFunc = true;
    m_ro.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
    m_ro.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
    m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    switch(m_alphaType)
    {
    case ALPHA_BLEND:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
        break;

    case ALPHA_ADD:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;

    case ALPHA_SUB:
        m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ro.eBlendDstFunc = RTGBLEND_ONE;
        break;
    }

    if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texPath, m_texture.c_str()))
        m_ro.iNumStage = 1;
    else
        m_ro.iNumStage = 0;
    RTASSERT(strlen(m_texPath) < Text_File_Path_Size);

    for(int i=0; i<m_actor->m_bones.size(); i++)
    {
        SRT_Bone *tmp = &m_actor->m_bones[i];
        if(tmp->Name == m_head.c_str())
        {
            m_headBone = tmp;
        }
        else if(tmp->Name == m_tail.c_str())
        {
            m_tailBone = tmp;
        }
    }
}

bool CRT_EffectRibbon2::Create()
{
    if (m_ro.iNumStage)
        m_ro.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texPath));
	return true;
}

void CRT_EffectRibbon2::Tick(float deltaMill)
{

}

void CRT_EffectRibbon2::Render(RtgDevice * _dev,RTGRenderMask mask)
{
	if(m_list.empty()) return;



	if(!(m_headBone && m_tailBone)) return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

	RtgVertexVCT v;
	short idxMod[6] = {0,3,2, 0,1,3};

	RtgVertex3 offset;
	BYTE *vb;

	// don't use lighting on ribbon
	DWORD Light = dev->GetRenderState(RTGRS_LIGHTING);
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	dev->SetShader(&m_ro);
	dev->SetVertexFormat(RTG_VCT);

	DWORD op;
	op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
	dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	if(m_alphaType == ALPHA_ADD)
		SetFogWithAdd(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		SetFogWithAdd(dev);
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
	}

	if(m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,FALSE);

	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		SPiece *p = *it;
		
		// get visible vertex, > beginFrame && < fadeIn+life+fadeOut
		int delta = (p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut)*30/1000 + 1;
		if(delta > m_lastFrame-p->begFrame)
			delta = m_lastFrame-p->begFrame;

		int verNum = 0;
		for(int j=m_lastFrame-1; j>=m_lastFrame-delta; j--)
		{
			verNum += (m_headBone->ribbonFrame[j].segNum+1);
		}
		verNum *= 2;
		
		if(verNum == 0) continue;

        RtgShareBufferItem vbItem;
		vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(verNum*sizeof(RtgVertexVCT), &vbItem);
            
		// make vertex buffer
		for(int j=m_lastFrame-1; j>=m_lastFrame-delta; j--)
		{
			int seg = m_headBone->ribbonFrame[j].segNum;
			for(int k=seg; k>=0; k--)
			{
				// position
				m_headBone->ribbonFrame[j].pos[k].GetValue(v.v);
				
				// visilbe
				float validTime = (m_lastFrame-j)*1000.0f/30 - k*(1000.0f/30/(seg+1));

				// if global fadeout,get global visible
				float gvis = 1.0f;

				// if(j>p->keyFrame.fadeOutFrame) gvis = 0;
				if( j>p->keyFrame.fadeOutFrame && p->endFrame-p->keyFrame.fadeOutFrame>0)
				{
					gvis = 1 - ((1.0f*j-p->keyFrame.fadeOutFrame)*(seg+1)+k)/((p->endFrame - p->keyFrame.fadeOutFrame)*(seg+1));			
				}
                
				float visible = 0;	
				if(validTime < 0)
				{
					visible = 0;
				}
				else if(validTime < p->keyFrame.fadeIn)
				{
					visible = validTime/p->keyFrame.fadeIn;
				}
				else if(validTime < p->keyFrame.fadeIn + p->keyFrame.life)
				{
					visible = 1;
				}
				else if(validTime < p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut)
				{
					visible = 1 - ((validTime - p->keyFrame.fadeIn - p->keyFrame.life)/p->keyFrame.fadeOut);
				}
				visible *= gvis;
				v.c = RtgVectorToColor(RtgVertex3(1,1,1),visible);
			    
				// texture uv
				v.t.x = 0.01;
				v.t.y = ( (m_lastFrame-j)*1000.0f/30 - k*(1000.f/30/(seg+1))) 
					/ (p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut);
				if(v.t.y < 0) v.t.y = 0;
				if(v.t.y > 1) v.t.y = 1;

				// copy to buffer
				memcpy(vb,&v,sizeof(RtgVertexVCT));
				vb += sizeof(RtgVertexVCT);
				// LOG3("uv = %f,%f,visible = %f\n",v.t.x,v.t.y,visible);

				// another vertex
				m_tailBone->ribbonFrame[j].pos[k].GetValue(v.v);
				v.t.x = 0.99f;

				memcpy(vb,&v,sizeof(RtgVertexVCT));
				vb += sizeof(RtgVertexVCT);
				// LOG3("uv = %f,%f,visible = %f\n",v.t.x,v.t.y,visible);
			}
		}

		RtgMatrix16 world;
		world.Unit();
		world.Set4X3Matrix(m_actor->m_matrix.m_m);

        dev->GetBuffMgr()->Unlock(&vbItem);
        //dev->SetStreamSource(0, dev->GetBuffMgr()->GetShared_vb(), sizeof(RtgVertexVCT));
		dev->SetMatrix(RTGTS_WORLD,&world);
        dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, &vbItem, sizeof(RtgVertexVCT), verNum-2);
        //dev->DrawPrimitive(
        //    D3DPT_TRIANGLESTRIP,
        //    0,
        //    verNum-2);

	}	

	dev->RestoreShader(&m_ro);
	dev->SetRenderState(RTGRS_LIGHTING,Light);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
	if(m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	if(m_alphaType == ALPHA_ADD)
		RestoreFog(dev);
	else if(m_alphaType == ALPHA_SUB)
	{
		dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
		RestoreFog(dev);
	}
}

void CRT_EffectRibbon2::ConvertOption()
{
	m_alphaType = ALPHA_BLEND;
	if(m_alphaOpt == "Blend") m_alphaType = ALPHA_BLEND;
	else if(m_alphaOpt == "Add") m_alphaType = ALPHA_ADD;
	else if(m_alphaOpt == "None") m_alphaType = ALPHA_NULL;
	else if(m_alphaOpt == "Sub") m_alphaType = ALPHA_SUB;
	// else if(m_alphaOpt == "Modulate") m_alphaType = ALPHA_MODULATE;
}

#ifdef CHAR_VIEWER
void CRT_EffectRibbon2::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPPropertyGridItemConstraints  *pList;

	pItem = pRoot->AddChildItem(new CCustomItemString("贴图",&m_texture));
	// pItem = pRoot->AddChildItem(new CXTPPropertyGridItemNumber("分割数",m_pieceNum,&m_pieceNum));
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("节点(head)",m_head.c_str()));
	pItem->SetFlags(xtpGridItemHasExpandButton);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("节点(tail)",m_tail.c_str()));
	pItem->SetFlags(xtpGridItemHasExpandButton);
	pRoot->AddChildItem(new CCustomItemBool("始终在最前画",&m_bAlwaysFront));
	pSubItem = pRoot->AddChildItem(new CCustomItemString("Alpha模式",&m_alphaOpt));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("None");
	pSubItem->SetFlags(xtpGridItemHasComboButton);
	pSubItem->Expand();

	pItem->Expand();
	pRoot->Expand();
}

bool CRT_EffectRibbon2::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		Recreate();
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		if(pItem->GetCaption() == "节点(head)")
		{
			CDlgGetBone dlg;
			if(dlg.DoModal())
			{
				m_head = dlg.m_selectedBone;
				pItem->SetValue(m_head.c_str());
			}
		}
		else if(pItem->GetCaption() == "节点(tail)")
		{
			CDlgGetBone dlg;
			if(dlg.DoModal())
			{
				m_tail = dlg.m_selectedBone;
				pItem->SetValue(m_tail.c_str());
			}
		}
		Recreate();
		break;
	}
	return false;
}

void CRT_EffectRibbon2::AttachKeyGrid(int key,void *p)
{
	CRT_EffectRibbon2 *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	SRibbonKey2 *k;
	CString tmp;

	k = m_keyList.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("关键帧",tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("淡出帧数",k->fadeOutFrame,&k->fadeOutFrame));
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("结束帧数",k->endFrame,&k->endFrame));
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("淡入时间",k->fadeIn,&k->fadeIn));
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("持续时间",k->life,&k->life));
	pRoot->AddChildItem(new CXTPPropertyGridItemNumber("淡出时间",k->fadeOut,&k->fadeOut));
	pRoot->Expand();
}

bool CRT_EffectRibbon2::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED: break;
	case XTP_PGN_INPLACEBUTTONDOWN: break;
	}
	return false;
}

int CRT_EffectRibbon2::TVGetKeyNum()
{
	return m_keyList.Size();
}

int CRT_EffectRibbon2::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyList.GetKeyFrame(key,frame);
	return frame;
}

void CRT_EffectRibbon2::TVOnNotify(ETV_OP op,long param1,long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SRibbonKey2 k;
	SRibbonKey2 *old;

	switch(op) 
	{
	case OP_MODIFY_PROP:
		prop.Init(this);
		prop.DoModal();
		break;

	case OP_MODIFY_KEY:
		keyInfo.Init(param1,this);
		keyInfo.DoModal();
		break;

	case OP_ADD_KEY:
		m_keyList.AddKeyFrame(param1,k);
		keyInfo.Init(param1,this);
		keyInfo.DoModal();
		break;

	case OP_DEL_KEY:
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_MOVE_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if(!old) break;
		m_keyList.AddKeyFrame(param2,*old);
		m_keyList.DelKeyFrame(param1);
		break;	

	case OP_COPY_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if(!old) break;
		m_keyList.AddKeyFrame(param2,*old);
		break;	
	}
}
#endif


CRT_EffectBlur::CRT_EffectBlur()
    : m_delta(0),
      m_bAlwaysFront(false),
      m_blurNum(0),
      m_blurInterval(0),
      m_visible(1.f),
      m_bEnable(true),
      m_frameDelta(0)
{
}

CRT_EffectBlur::~CRT_EffectBlur()
{

}

void CRT_EffectBlur::Render(RtgDevice *dev, RTGRenderMask mask)
{
    if (!m_actor || !m_bEnable) return;

    if(m_bAlwaysFront)
        dev->SetRenderState(RTGRS_Z_TEST,FALSE);

    // backup actor info
    float oldVis = m_actor->GetVisible();
    RtgMatrix12 oldMat = *(m_actor->GetWorldMatrix());

    m_actor->SetVisible(m_visible);
    list<SFrame>::iterator it;
    for(it=m_frameList.begin(); it!=m_frameList.end(); it++)
    {
        SFrame *tmp = &(*it);
        m_actor->SetWorldMatrix(tmp->WorldMat);
        m_actor->Render(dev,RTGRM_ALL,false);
    }

    m_actor->SetVisible(oldVis);
    m_actor->SetWorldMatrix(oldMat);

    if(m_bAlwaysFront)
        dev->SetRenderState(RTGRS_Z_TEST,TRUE);
}

#ifdef CHAR_VIEWER
    void CRT_EffectBlur::AttachPropGrid(void *p)
    {
        CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
        CXTPPropertyGridItem *pItem,*pSubItem;
        CXTPPropertyGridItemConstraints  *pList;

        // pItem = pRoot->AddChildItem(new CCustomItemString("名字",&s->m_name));
        // pItem->SetReadOnly(TRUE);
        pRoot->AddChildItem(new CXTPPropertyGridItemNumber("残影数量",m_blurNum,&m_blurNum));
        pRoot->AddChildItem(new CXTPPropertyGridItemNumber("残影间隔(ms)",m_blurInterval,&m_blurInterval));
        pRoot->AddChildItem(new CCustomItemBool("始终在前",&m_bAlwaysFront));
        pRoot->AddChildItem(new CCustomItemFloat("透明",&m_visible));
        pRoot->Expand();
    }

    bool CRT_EffectBlur::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
    {
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
        switch(wParam)
        {
        case XTP_PGN_ITEMVALUE_CHANGED:
            Recreate();
            break;

        case XTP_PGN_INPLACEBUTTONDOWN:
            break;
        }
        return false;
    }

    void CRT_EffectBlur::AttachKeyGrid(int key,void *p)
    {
        CRT_EffectBlur *s = this;
        CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
        CXTPPropertyGridItem *pItem;

        SBlurKey *k;
        CString tmp;

        k = m_keyList.GetKeyFrame(key);
        if(!k) return;
        tmp.Format("%d",key);
        pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数",tmp));
        pItem->SetReadOnly(TRUE);
        pRoot->AddChildItem(new CCustomItemBool("有效",&k->bEnable));
        pRoot->Expand();
    }

    bool CRT_EffectBlur::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
    {
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
        switch(wParam)
        {
        case XTP_PGN_ITEMVALUE_CHANGED: break;
        case XTP_PGN_INPLACEBUTTONDOWN: break;
        }
        return false;
    }

    int CRT_EffectBlur::TVGetKeyNum()
    {
        return m_keyList.Size();
    }

    int CRT_EffectBlur::TVGetKeyFrame(int key)
    {
        unsigned long frame = 0;
        m_keyList.GetKeyFrame(key,frame);
        return frame;
    }

    void CRT_EffectBlur::TVOnNotify(ETV_OP op,long param1,long param2)
    {
        CDlgChannelInfo prop;
        CDlgKeyInfo keyInfo;
        SBlurKey k;
        SBlurKey *old;

        switch(op) 
        {
        case OP_MODIFY_PROP:
            prop.Init(this);
            prop.DoModal();
            break;

        case OP_MODIFY_KEY:
            keyInfo.Init(param1,this);
            keyInfo.DoModal();
            break;

        case OP_ADD_KEY:
            m_keyList.AddKeyFrame(param1,k);
            keyInfo.Init(param1,this);
            keyInfo.DoModal();
            break;

        case OP_DEL_KEY:
            m_keyList.DelKeyFrame(param1);
            break;

        case OP_MOVE_KEY:
            old = m_keyList.GetKeyFrame(param1);
            if(!old) break;
            m_keyList.AddKeyFrame(param2,*old);
            m_keyList.DelKeyFrame(param1);
            break;	

        case OP_COPY_KEY:
            old = m_keyList.GetKeyFrame(param1);
            if(!old) break;
            m_keyList.AddKeyFrame(param2,*old);
            break;	
        }
    }
#endif
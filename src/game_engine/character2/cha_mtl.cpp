//
// Material implementation file
//
#include "character2/actor.h"

bool  FogEnable;
DWORD FogColor;
void SetFogWithAdd(RtgDevice *dev)
{
	FogEnable = dev->GetFogEnable()==TRUE;
	FogColor  = dev->GetFogColor();
	dev->SetFogColor(0);
	// dev->SetFogEnable(FALSE);
}

void RestoreFog(RtgDevice *dev)
{		
	dev->SetFogEnable(FogEnable);
	dev->SetFogColor(FogColor);
}

void SetupAlphaMode(long alpha, RtgShader &ro, RtgDevice *dev, float visible)
{
	ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
	ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
	switch(alpha)
	{
		case ALPHA_NULL:
			break;
		case ALPHA_MASK:
			ro.bColorKey = true;
			dev->SetRenderState(RTGRS_ALPHA_REF, (DWORD)128*visible);
			break;
		case ALPHA_ADD:
			ro.bEnableBlendFunc = true;
			// ro.eBlendSrcFunc = RTGBLEND_ONE;
			ro.eBlendDstFunc = RTGBLEND_ONE;
			SetFogWithAdd(dev);
			break;
		case ALPHA_SUB:
			ro.bEnableBlendFunc = true;
			// ro.eBlendSrcFunc = RTGBLEND_ONE;
			ro.eBlendDstFunc = RTGBLEND_ONE;
			SetFogWithAdd(dev);
			dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
			break;
		case ALPHA_MODULATE:
			ro.bEnableBlendFunc = true;
			ro.eBlendSrcFunc = RTGBLEND_DESTCOLOR;
			ro.eBlendDstFunc = RTGBLEND_ZERO;
			break;
		case ALPHA_MODULATE2X:
			ro.bEnableBlendFunc = true;
			ro.eBlendSrcFunc = RTGBLEND_DESTCOLOR;
			ro.eBlendDstFunc = RTGBLEND_SRCCOLOR;
			break;
		default:
			ro.bEnableBlendFunc = true;
			//SetFogWithAdd(dev);
			break;
	}
}

void RestoreAlphaMode(long alpha, RtgShader &ro, RtgDevice *dev, float visible)
{
	// recover render option
	switch(alpha)
	{
		case ALPHA_NULL:
			break;
		case ALPHA_MASK:
			ro.bColorKey = false;
			break;
		case ALPHA_ADD:
			RestoreFog(dev);
			break;
		case ALPHA_SUB:
			dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
			RestoreFog(dev);
			break;
		case ALPHA_MODULATE:
			break;
		case ALPHA_MODULATE2X:
			break;
	}
}

RT_IMPLEMENT_DYNCREATE(CRT_MtlInstStandard, CRT_MtlInstance, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlInstBlend, CRT_MtlInstance, NULL, "")

// -------------------------------------------------------------------------------------------
// Mtl Standard
// -------------------------------------------------------------------------------------------
CRT_MtlInstStandard::CRT_MtlInstStandard()
{
	m_delta = 0;
    m_skin = NULL;
	m_rs = NULL;
	
	m_visible = 1.0f;
	m_baseStage = -1;
	m_hlStage = -1;
	m_effectStage1 = -1;
	m_effectStage2 = -1;
	m_effectMaskStage1 = -1;
	m_effectMaskStage2 = -1;
}

CRT_MtlInstStandard::~CRT_MtlInstStandard()
{

}

void CRT_MtlInstStandard::InstanceInit()
{
	CRT_MtlInstance::InstanceInit();
	m_mtlStandard = (CRT_MtlStandard*)m_data;
}

void CRT_MtlInstStandard::InstanceExit()
{
	m_renderDev->DeleteShader(&m_ro);
}

void CRT_MtlInstStandard::BuildEffectTexStage(string &effectTex, string &effectMaskTex, bool matrix, 
     char uvMode, bool useFactor, long blendMode, int &effectStage, int &effectMaskStage)
{
	bool useTmp = false;
	// check if can render in one pass
	if(m_renderDev->m_Ability.bTextureOpMultiplyAdd)
	{
		if(effectTex.length() > 0)
		{
			effectStage = m_ro.iNumStage;
			m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = matrix;
			m_ro.Shaders[m_ro.iNumStage].SetTexture(m_renderDev->m_Texture.CreateTexture(effectTex.c_str(),RTGTBT_AUTO,true,1));
			if(useFactor)
			{
				m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE;
				m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEXTURE;
				m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TFACTOR;
			}
			else
			{
				m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_SELECTARG1;
				m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEXTURE;
			}
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
			m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_TEMP;
			useTmp = true;
			switch(uvMode)
			{
				case ENV_UV_BASE: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0; break;
				case ENV_UV_XY:
				case ENV_UV_YZ:
				case ENV_UV_ZX:
					m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU1; break;
				case ENV_UV_CPOS: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION; break;
				case ENV_UV_CNOR: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACENORMAL; break;
				case ENV_UV_CREL: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR; break;
			}
			m_ro.iNumStage++;
		}
		if(effectMaskTex.length() > 0)
		{
			effectMaskStage = m_ro.iNumStage;
			m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = false;
			m_ro.Shaders[m_ro.iNumStage].SetTexture(m_renderDev->m_Texture.CreateTexture(effectMaskTex.c_str(),RTGTBT_AUTO,true,1));
			m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE;
			m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEMP;
			m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEXTURE;
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
			m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_TEMP;
			m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0;
			m_ro.iNumStage++;
			useTmp = true;
		}
		if(useTmp)
		{
			m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_ADD;
			switch(blendMode)
			{ 
				case ALPHA_ADD:			m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_ADD; break;
				case ALPHA_SUB:			m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_SUBTRACT; break;
				case ALPHA_MODULATE:	m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE; break;
				case ALPHA_MODULATE2X:	m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE2X; break;
			}
			m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_CURRENT;
			m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEMP;
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
			m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_CURRENT;
			m_ro.iNumStage++;
		}
	}
}

void CRT_MtlInstStandard::OnDataChange(DWORD p1, DWORD p2)
{
	m_hlStage       = -1;
	m_baseStage		= -1;
	m_effectStage1	= -1;
	m_effectStage2	= -1;
	m_effectMaskStage1 = -1;
	m_effectMaskStage2 = -1;

	m_renderDev->DeleteShader(&m_ro);
	RtgDevice *dev = m_renderDev;

	m_key = m_mtlStandard->m_defKey;

    // setup render option
	m_ro.bTwoSide  = m_mtlStandard->m_bTwoSide;
	m_ro.bColorKey = false;
	m_ro.bEnableBlendFunc = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.Shaders[0].bUseTextureMatrix = false;
	
	m_ro.iNumStage = 0;
	if(m_mtlStandard->m_texBase.length() > 0)
	{
		m_baseStage = m_ro.iNumStage;
		// m_ro.Shaders[m_ro.iNumStage].mMatrix.Unit();
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = m_mtlStandard->m_bBaseTexMatrix;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(dev->m_Texture.CreateTexture(m_mtlStandard->m_texBase.c_str(),RTGTBT_AUTO,true,1));
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_DIFFUSE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_CURRENT;
		if(m_mtlStandard->m_alphaMode==ALPHA_MASK || m_mtlStandard->m_alphaMode==ALPHA_BLEND)
		{
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_MODULATE;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TEXTURE;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg2 = RTGTA_TFACTOR;
		}
		else
		{
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TFACTOR;
		}
		m_ro.iNumStage++;
	}

	if(!dev->m_Ability.bTextureOpMultiplyAdd) return;

	if(m_mtlStandard->m_hlMode == HL_MODE_TEXTURE && m_mtlStandard->m_texHl.length() > 0)
	{
		m_hlStage = m_ro.iNumStage;
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = false;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(dev->m_Texture.CreateTexture(m_mtlStandard->m_texHl.c_str(),RTGTBT_AUTO,true,1));
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MULTIPLYADD;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_SPECULAR;
		m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg0 = RTGTA_CURRENT;

		m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
		m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0;
		m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_CURRENT;
		m_ro.iNumStage++;
	}

	BuildEffectTexStage(m_mtlStandard->m_texEffect1, m_mtlStandard->m_texEffectMask1, 
		m_mtlStandard->m_bEffectMatrix1, m_mtlStandard->m_effectUV1, true, m_mtlStandard->m_effectBlendMode1, m_effectStage1, m_effectMaskStage1);
	BuildEffectTexStage(m_mtlStandard->m_texEffect2, m_mtlStandard->m_texEffectMask2, 
 		m_mtlStandard->m_bEffectMatrix2, m_mtlStandard->m_effectUV2, false, m_mtlStandard->m_effectBlendMode2, m_effectStage2, m_effectMaskStage2);
}

bool CRT_MtlInstStandard::Init(RtgDevice *dev,CRT_SkinInstance *skin,SRT_RenderSurface *rs)
{
	m_renderDev = dev;
	m_skin = skin;
	m_rs   = rs;
	
	OnDataChange(0,0);
	return true;
}

bool CRT_MtlInstStandard::UploadVB(RtgDevice *dev)
{
	DWORD sVB;
	BYTE *vb;
	DWORD i;

	if(m_mtlStandard->m_texEffect1.length()>0 && 
		(m_mtlStandard->m_effectUV1>=ENV_UV_XY && m_mtlStandard->m_effectUV1<=ENV_UV_YZ))
	{
		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_skin->m_verVec.size()*sizeof(RtgVertexVNT2),
			FALSE,sizeof(RtgVertexVNT2));
		for(i=0; i<m_skin->m_verVec.size(); i++)
		{
			memcpy(vb,&m_skin->m_verVec[i].pos,2*sizeof(RtgVertex3));
			vb += 2*sizeof(RtgVertex3);
			memcpy(vb,&m_skin->m_verVec[i].uv,2*sizeof(float));vb += 2*sizeof(float);
			switch(m_mtlStandard->m_effectUV1) {
			case ENV_UV_XY:
				float t;
				t = m_skin->m_verVec[i].nor.x; ///2+0.5;
				memcpy(vb,&t,sizeof(float));vb += sizeof(float);
				t = m_skin->m_verVec[i].nor.y; ///2+0.5;
				memcpy(vb,&t,sizeof(float));vb += sizeof(float);
				break;
			case ENV_UV_YZ:
				memcpy(vb,&m_skin->m_verVec[i].nor.y,sizeof(float));vb += sizeof(float);
				memcpy(vb,&m_skin->m_verVec[i].nor.z,sizeof(float));vb += sizeof(float);
				break;
			case ENV_UV_ZX:
				memcpy(vb,&m_skin->m_verVec[i].nor.z,sizeof(float));vb += sizeof(float);
				memcpy(vb,&m_skin->m_verVec[i].nor.x,sizeof(float));vb += sizeof(float);
				break;
			}
		}
		dev->SetVertexFormat(RTG_VNT2);
	}
	else
	{	
		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_skin->m_verVec.size()*sizeof(RtgVertexVNT),
			FALSE,sizeof(RtgVertexVNT));
		for(int k=0; k<m_skin->m_verVec.size(); k++)
		{
			memcpy(vb,&m_skin->m_verVec[k].pos,sizeof(RtgVertexVNT));
			vb += sizeof(RtgVertexVNT);
		}
		dev->SetVertexFormat(RTG_VNT);
	}
	dev->m_pVB->EndUpload(sVB);
	dev->m_pVB->Bind(0, 0);
	return true;
}

bool CRT_MtlInstStandard::UploadIB(RtgDevice *dev)
{
	DWORD sIB;
	BYTE *ib;

	ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, m_rs->idxVec.size()*sizeof(short),
		FALSE, sizeof(short));
	memcpy(ib, &m_rs->idxVec[0],m_rs->idxVec.size()*sizeof(short));
	dev->m_pIB->EndUpload(sIB);
	dev->m_pIB->Bind(0, 0);
	return true;
}

void CRT_MtlInstStandard::Render(RtgDevice *dev, RTGRenderMask mask)
{
	if(m_skin->m_verVec.size()==0 || m_rs->idxVec.size()==0)
		return;

	SetVisible(m_key.visible);	
	///!Hard code for spePower
	m_key.spePower = 50;
	dev->SetMaterial(m_key.dif, m_key.amb, m_key.spe, m_key.emi+m_baseColor, m_key.spePower);
	
	if(m_mtlStandard->m_hlMode == HL_MODE_VERTEX) {
		dev->SetRenderState(RTGRS_HEIGHTLIGHT,TRUE);
	}

	m_ro.dwTextureFactor = RtgVectorToColor(m_key.envColor,GetFinialVisible());
	if(IsTransparent()) 
	{
		m_ro.bEnableBlendFunc = true;
		dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	}
	
	m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
	RtgMatrix2D9 m2;
	RtgMatrix16  m16;

	SetupAlphaMode(m_mtlStandard->m_alphaMode, m_ro, dev, GetFinialVisible());

	if(dev->m_Ability.bTextureOpMultiplyAdd)
	{
		if(m_baseStage != -1 && m_mtlStandard->m_bBaseTexMatrix)
		{
			m2.Unit();
			m2.Translate(m_key.baseTranslate.x, m_key.baseTranslate.y);
			m2.RotateLZ(m_key.baseRotate.x);
			m2.Scale(m_key.baseScale.x, m_key.baseScale.y);
			m2.SaveTo4x4(m_ro.Shaders[m_baseStage].mMatrix16);
		}
		if(m_hlStage != -1 && m_mtlStandard->m_hlMode==HL_MODE_TEXTURE && m_mtlStandard->m_texHl.length() > 0)
		{
			m_ro.Shaders[m_hlStage].eTexCoordIndex = RTGTC_PASSTHRU0;
		}
		if(m_effectStage1!=-1 && m_mtlStandard->m_bEffectMatrix1)
		{
			switch(m_mtlStandard->m_effectUV1)
			{
				case ENV_UV_BASE:
				case ENV_UV_XY:
				case ENV_UV_YZ:
				case ENV_UV_ZX:
					m2.Unit();
					m2.Translate(m_key.effectTranslate1.x, m_key.effectTranslate1.y);
					m2.RotateLZ(m_key.effectRotate1.x);
					m2.Scale(m_key.effectScale1.x,m_key.effectScale1.y);
					m2.SaveTo4x4(m_ro.Shaders[m_effectStage1].mMatrix16);
					break;
				case ENV_UV_CPOS:
				case ENV_UV_CNOR:
				case ENV_UV_CREL:
					m16.Unit();
					m16.Translate(m_key.effectTranslate1.x, m_key.effectTranslate1.y, m_key.effectTranslate1.z);
					m16.Scale(m_key.effectScale1.x,m_key.effectScale1.y,m_key.effectScale1.z);
					m16.RotateZ(m_key.effectRotate1.x);
					m_ro.Shaders[m_effectStage1].mMatrix16 = m16;
					break;
			}
		}
		if(m_effectStage2!=-1 && m_mtlStandard->m_bEffectMatrix2)
		{
			switch(m_mtlStandard->m_effectUV2)
			{
				case ENV_UV_BASE:
				case ENV_UV_XY:
				case ENV_UV_YZ:
				case ENV_UV_ZX:
					m2.Unit();
					m2.Translate(m_key.effectTranslate2.x, m_key.effectTranslate2.y);
					m2.RotateLZ(m_key.effectRotate2.x);
					m2.Scale(m_key.effectScale2.x,m_key.effectScale2.y);
					m2.SaveTo4x4(m_ro.Shaders[m_effectStage2].mMatrix16);
					break;
				case ENV_UV_CPOS:
				case ENV_UV_CNOR:
				case ENV_UV_CREL:
					m16.Unit();
					m16.Translate(m_key.effectTranslate2.x, m_key.effectTranslate2.y, m_key.effectTranslate2.z);
					m16.Scale(m_key.effectScale2.x,m_key.effectScale2.y,m_key.effectScale2.z);
					m16.RotateZ(m_key.effectRotate2.x);
					m_ro.Shaders[m_effectStage2].mMatrix16 = m16;
					break;
			}
		}
	}
	else
	{

	}
	
	if(!m_skin->m_vcVec)
	{
		UploadVB(dev);
		UploadIB(dev);
	}
	else
	{
		DWORD sVB;
		BYTE *vb;
		// combine vc and dx light
		// material emi should add to vc in precompute or will lost
		dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_MATERIAL);
		dev->SetRenderState(RTGRS_AMBIENTMATERIALSOURCE,RTGMCS_MATERIAL);
		dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE,RTGMCS_COLOR1);

		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_skin->m_verVec.size()*sizeof(RtgVertexVNCT),
			FALSE,sizeof(RtgVertexVNCT));

		int vcOffset = m_skin->m_vcOffset;
		vector<DWORD> *vcList = m_skin->m_vcVec;
		for(int i=0; i<m_skin->m_verVec.size(); i++)
		{
			memcpy(vb,&m_skin->m_verVec[i].pos,2*sizeof(RtgVertex3));
			vb += 2*sizeof(RtgVertex3);
			memcpy(vb,&((*vcList)[vcOffset++]),sizeof(DWORD));
			vb += sizeof(DWORD);
			memcpy(vb,&m_skin->m_verVec[i].uv,sizeof(RtgVertex2));
			vb += sizeof(RtgVertex2);
		}
		dev->m_pVB->EndUpload(sVB);
		dev->m_pVB->Bind(0, 0);
		UploadIB(dev);
		dev->SetVertexFormat(RTG_VNCT);
	}

	// do render
	PFM()->actorRenderTri += m_rs->idxVec.size()/3;
	PFM()->skinRenderTri  += m_rs->idxVec.size()/3;
	dev->SetShader(&m_ro);
	dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,m_skin->m_verVec.size(),0,m_rs->idxVec.size()/3);
	dev->RestoreShader(&m_ro);

	RestoreAlphaMode(m_mtlStandard->m_alphaMode, m_ro, dev, GetFinialVisible());
	if(IsTransparent()) 
	{
		m_ro.bEnableBlendFunc = false;
		dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
	}
	if(m_mtlStandard->m_hlMode == HL_MODE_VERTEX) {
		dev->SetRenderState(RTGRS_HEIGHTLIGHT,FALSE);
	}
}

bool CRT_MtlInstStandard::RequestTick()
{
	return (!m_mtlStandard->m_bSyncActorFrame && m_mtlStandard->m_keyVec.Size()!=0);
}

void CRT_MtlInstStandard::Tick(unsigned long delta)
{
	unsigned long beg,end;
	beg = m_mtlStandard->m_keyVec.GetBegFrame();
	end = m_mtlStandard->m_keyVec.GetEndFrame();
	m_delta += delta;
	if(beg == end) return;
	UseFrame(beg + (m_delta/33)%(end-beg));
}

bool CRT_MtlInstStandard::RequestUseFrame()
{
	return (m_mtlStandard->m_bSyncActorFrame && m_mtlStandard->m_keyVec.Size()!=0);
}

void CRT_MtlInstStandard::UseFrame(unsigned long frame)
{
	SMtlStandardKey key;
	int size = m_mtlStandard->m_keyVec.Size();
	if(!m_mtlStandard->m_keyVec.GetKeyFrame(frame,key))
		return;
	
	Update(key);
}

void CRT_MtlInstStandard::Update(SMtlStandardKey &key)
{
	// update by key
	m_key = key;
	// SetVisible(key.visible);
}

float CRT_MtlInstStandard::GetFinialVisible()
{
	return m_skin->GetFinalVisible() * m_visible;
}

bool CRT_MtlInstStandard::IsTransparent()
{
	switch(m_mtlStandard->m_alphaMode)
	{
		case ALPHA_ADD:
		case ALPHA_BLEND:
		case ALPHA_MODULATE:
		case ALPHA_SUB:
		case ALPHA_MODULATE2X:
			return true;
	}
	
	float visible = GetFinialVisible();
	if(Abs(visible - 1.0f) >= 0.05f)
		return true;
	return false;
}


// -------------------------------------------------------------------------------------------
// Mtl Blend
// -------------------------------------------------------------------------------------------
CRT_MtlInstBlend::CRT_MtlInstBlend()
{
	m_delta = 0;
    m_skin = NULL;
	m_rs = NULL;
	
	m_visible = 1.0f;
}

CRT_MtlInstBlend::~CRT_MtlInstBlend()
{

}

void CRT_MtlInstBlend::InstanceInit()
{
	CRT_MtlInstance::InstanceInit();
	m_mtlBlend = (CRT_MtlBlend*)m_data;
}

void CRT_MtlInstBlend::InstanceExit()
{
	m_renderDev->DeleteShader(&m_ro);
}

void CRT_MtlInstBlend::OnDataChange(DWORD p1, DWORD p2)
{
	m_renderDev->DeleteShader(&m_ro);

	RtgDevice *dev = m_renderDev;
	m_key = m_mtlBlend->m_defKey;

    // setup render option
	m_ro.bTwoSide  = m_mtlBlend->m_fo.twoSide;
	m_ro.bColorKey = false;
	m_ro.bEnableBlendFunc = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.Shaders[0].bUseTextureMatrix = false;
	
	m_ro.iNumStage = 0;
	if(m_mtlBlend->m_texMix.length() > 0)
	{
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = m_mtlBlend->m_bTexMatrixMix;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(dev->m_Texture.CreateTexture(m_mtlBlend->m_texMix.c_str(),RTGTBT_AUTO,true,4));
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_TEMP;
		m_ro.iNumStage++;
	}
	else
	{
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = m_mtlBlend->m_bTexMatrixMix;
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TFACTOR;
		m_ro.Shaders[m_ro.iNumStage].eResultArg = RTGTA_TEMP;
		m_ro.iNumStage++;
	}

	if(m_mtlBlend->m_tex1.length() > 0)
	{
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = m_mtlBlend->m_bTexMatrix1;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(dev->m_Texture.CreateTexture(m_mtlBlend->m_tex1.c_str(),RTGTBT_AUTO,true,4));
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEXTURE;
		if(m_mtlBlend->m_fo.alphaMode == ALPHA_NULL)
		{
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TFACTOR;
		}
		else
		{
			m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_MODULATE;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TEXTURE;
			m_ro.Shaders[m_ro.iNumStage].eAlphaArg2 = RTGTA_TFACTOR;
		}
		m_ro.iNumStage++;
	}

	if(m_mtlBlend->m_tex2.length() > 0)
	{
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = m_mtlBlend->m_bTexMatrix2;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(dev->m_Texture.CreateTexture(m_mtlBlend->m_tex2.c_str(),RTGTBT_AUTO,true,4));
		m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_LERP;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_CURRENT;
        m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg0 = RTGTA_TEMP;
		
		m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
		m_ro.iNumStage++;
	}

	m_ro.Shaders[m_ro.iNumStage].eColorOp = RTGTOP_MODULATE;
	m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_CURRENT;
	m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_DIFFUSE;
	m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
	m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_CURRENT;
	m_ro.iNumStage++;
}

bool CRT_MtlInstBlend::Init(RtgDevice *dev,CRT_SkinInstance *skin,SRT_RenderSurface *rs)
{
	m_renderDev = dev;
	m_skin = skin;
	m_rs   = rs;
	
	OnDataChange(0,0);
	return true;
}

bool CRT_MtlInstBlend::UploadVB(RtgDevice *dev)
{
	DWORD sVB;
	BYTE *vb;
	// DWORD i;

	vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_skin->m_verVec.size()*sizeof(RtgVertexVNT),
		FALSE,sizeof(RtgVertexVNT));
	for(int k=0; k<m_skin->m_verVec.size(); k++)
	{
		memcpy(vb,&m_skin->m_verVec[k].pos,sizeof(RtgVertexVNT));
		vb += sizeof(RtgVertexVNT);
	}
	dev->SetVertexFormat(RTG_VNT);

	dev->m_pVB->EndUpload(sVB);
	dev->m_pVB->Bind(0, 0);
	return true;
}

bool CRT_MtlInstBlend::UploadIB(RtgDevice *dev)
{
	DWORD sIB;
	BYTE *ib;

	ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, m_rs->idxVec.size()*sizeof(short),
		FALSE, sizeof(short));
	memcpy(ib, &m_rs->idxVec[0],m_rs->idxVec.size()*sizeof(short));
	dev->m_pIB->EndUpload(sIB);
	dev->m_pIB->Bind(0, 0);
	return true;
}

void CRT_MtlInstBlend::Render(RtgDevice *dev, RTGRenderMask mask)
{
	if(m_skin->m_verVec.size()==0 || m_rs->idxVec.size()==0)
		return;

	/*
	if(m_mtlBlendStandard->m_hlMode == HL_MODE_VERTEX) {
		dev->SetRenderState(RTGRS_HEIGHTLIGHT,TRUE);
	}
	*/

	SetVisible(m_key.ro.visible);
	dev->SetMaterial(m_key.ro.dif, m_key.ro.amb, m_key.ro.spe, m_key.ro.emi+m_baseColor, m_key.ro.spePower);
    m_ro.dwTextureFactor = RtgVectorToColor(RtgVertex3(m_key.mixAmount,m_key.mixAmount,m_key.mixAmount),
			GetFinialVisible());

	if(IsTransparent()) 
	{
		m_ro.bEnableBlendFunc = true;
		dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	}
	
	m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
	RtgMatrix16  m16;
	RtgMatrix2D9 m2d;

	SetupAlphaMode(m_mtlBlend->m_fo.alphaMode, m_ro, dev, GetFinialVisible());
	if(dev->m_Ability.bTextureOpMultiplyAdd)
	{
		m_ro.iNumStage = 0;
		if(m_mtlBlend->m_texMix.length() > 0)
		{
			if(m_mtlBlend->m_bTexMatrixMix)
			{
				if(m_mtlBlend->m_uvMix==TEX_UV_BASE)
				{	
					m2d.Unit();
					m2d.Translate(m_key.stage[2].translate.x, m_key.stage[2].translate.y);
					m2d.RotateLZ(m_key.stage[2].rotate.x);
					m2d.Scale(m_key.stage[2].scale.x, m_key.stage[2].scale.y);
					m2d.SaveTo4x4(m_ro.Shaders[m_ro.iNumStage].mMatrix16);
				}
				else
				{
					m16.Unit();
					m16.Translate(m_key.stage[2].translate.x, m_key.stage[2].translate.y, m_key.stage[2].translate.z);
					m16.RotateLZ(m_key.stage[2].rotate.x);
					m16.Scale(m_key.stage[2].scale.x, m_key.stage[2].scale.y, m_key.stage[2].scale.z);
					m_ro.Shaders[m_ro.iNumStage].mMatrix16 = m16;
				}
			}
			switch(m_mtlBlend->m_uvMix)
			{
				case TEX_UV_BASE: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0;break;
				case TEX_UV_CPOS: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;break;
				case TEX_UV_CNOR: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACENORMAL;break;
				case TEX_UV_CREL: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR;break;
			}
			m_ro.iNumStage++;
		}
		else
		{
			m_ro.iNumStage++;
		}

		if(m_mtlBlend->m_tex1.length() > 0)
		{
			if(m_mtlBlend->m_bTexMatrix1)
			{
				if(m_mtlBlend->m_uvMix==TEX_UV_BASE)
				{	
					m2d.Unit();
					m2d.Translate(m_key.stage[0].translate.x, m_key.stage[0].translate.y);
					m2d.RotateLZ(m_key.stage[0].rotate.x);
					m2d.Scale(m_key.stage[0].scale.x, m_key.stage[0].scale.y);
					m2d.SaveTo4x4(m_ro.Shaders[m_ro.iNumStage].mMatrix16);
				}
				else
				{
					m16.Unit();
					m16.Translate(m_key.stage[0].translate.x, m_key.stage[0].translate.y, m_key.stage[0].translate.z);
					m16.RotateLZ(m_key.stage[0].rotate.x);
					m16.Scale(m_key.stage[0].scale.x, m_key.stage[0].scale.y, m_key.stage[0].scale.z);
					m_ro.Shaders[m_ro.iNumStage].mMatrix16 = m16;
				}
			}
			switch(m_mtlBlend->m_uv1)
			{
				case TEX_UV_BASE: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0;break;
				case TEX_UV_CPOS: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;break;
				case TEX_UV_CNOR: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACENORMAL;break;
				case TEX_UV_CREL: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR;break;
			}
			m_ro.iNumStage++;
		}
		if(m_mtlBlend->m_tex2.length() > 0)
		{
			if(m_mtlBlend->m_bTexMatrix2)
			{
				if(m_mtlBlend->m_uvMix==TEX_UV_BASE)
				{	
					m2d.Unit();
					m2d.Translate(m_key.stage[1].translate.x, m_key.stage[1].translate.y);
					m2d.RotateLZ(m_key.stage[1].rotate.x);
					m2d.Scale(m_key.stage[1].scale.x, m_key.stage[1].scale.y);
					m2d.SaveTo4x4(m_ro.Shaders[m_ro.iNumStage].mMatrix16);
				}
				else
				{
					m16.Unit();
					m16.Translate(m_key.stage[1].translate.x, m_key.stage[1].translate.y, m_key.stage[1].translate.z);
					m16.RotateLZ(m_key.stage[1].rotate.x);
					m16.Scale(m_key.stage[1].scale.x, m_key.stage[1].scale.y, m_key.stage[1].scale.z);
					m_ro.Shaders[m_ro.iNumStage].mMatrix16 = m16;
				}
			}
			switch(m_mtlBlend->m_uv2)
			{
				case TEX_UV_BASE: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_PASSTHRU0;break;
				case TEX_UV_CPOS: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;break;
				case TEX_UV_CNOR: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACENORMAL;break;
				case TEX_UV_CREL: m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR;break;
			}
			m_ro.iNumStage++;
		}
		m_ro.iNumStage++;
	}
	else
	{

	}
	UploadVB(dev);
	UploadIB(dev);

	// do render
	PFM()->actorRenderTri += m_rs->idxVec.size()/3;
	PFM()->skinRenderTri  += m_rs->idxVec.size()/3;
	dev->SetShader(&m_ro);
	dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,m_skin->m_verVec.size(),0,m_rs->idxVec.size()/3);
	dev->RestoreShader(&m_ro);

	// recover render option
	RestoreAlphaMode(m_mtlBlend->m_fo.alphaMode, m_ro, dev, GetFinialVisible());
	if(IsTransparent()) 
	{
		m_ro.bEnableBlendFunc = false;
		dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
	}
	//if(m_mtlBlend->m_hlMode == HL_MODE_VERTEX) {
	//	dev->SetRenderState(RTGRS_HEIGHTLIGHT,FALSE);
	//}
}

bool CRT_MtlInstBlend::RequestTick()
{
	return (!m_mtlBlend->m_bSyncActorFrame && m_mtlBlend->m_keyVec.Size()!=0);
}

void CRT_MtlInstBlend::Tick(unsigned long delta)
{
	unsigned long beg,end;
	beg = m_mtlBlend->m_keyVec.GetBegFrame();
	end = m_mtlBlend->m_keyVec.GetEndFrame();
	m_delta += delta;
	if(beg == end) return;
	UseFrame(beg + (m_delta/33)%(end-beg));
}

bool CRT_MtlInstBlend::RequestUseFrame()
{
	return (m_mtlBlend->m_bSyncActorFrame && m_mtlBlend->m_keyVec.Size()!=0);
}

void CRT_MtlInstBlend::UseFrame(unsigned long frame)
{
	SMtlBlendKey key;
	int size = m_mtlBlend->m_keyVec.Size();
	if(!m_mtlBlend->m_keyVec.GetKeyFrame(frame,key))
		return;
	
	Update(key);
}

void CRT_MtlInstBlend::Update(SMtlBlendKey &key)
{
	// update by key
	m_key = key;
	// SetVisible(key.visible);
}

float CRT_MtlInstBlend::GetFinialVisible()
{
	return m_skin->GetFinalVisible() * m_visible;
}

bool CRT_MtlInstBlend::IsTransparent()
{
	switch(m_mtlBlend->m_fo.alphaMode)
	{
		case ALPHA_ADD:
		case ALPHA_BLEND:
		case ALPHA_MODULATE:
		case ALPHA_MODULATE2X:
		case ALPHA_SUB:
			return true;
	}
	
	float visible = GetFinialVisible();
	if(Abs(visible - 1.0f) >= 0.05f)
		return true;
	return false;
}

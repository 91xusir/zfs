//
// Material implementation file
//
#include "character/actor.h"
#include "graph/rt_graph.h"


bool  FogEnable;
DWORD FogColor;
void SetFogWithAdd(RtgDevice* dev)
{
	FogEnable = dev->GetFogEnable() == TRUE;
	FogColor = dev->GetFogColor();
	dev->SetFogColor(0);
	// dev->SetFogEnable(FALSE);
}

void RestoreFog(RtgDevice* dev)
{
	dev->SetFogEnable(FogEnable);
	dev->SetFogColor(FogColor);
}

RT_IMPLEMENT_DYNCREATE(CRT_MtlStandard, CRT_Material, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlMu, CRT_Material, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_MtlBlend, CRT_Material, NULL, "")

CRT_MtlStandard::CRT_MtlStandard()
	: m_delta(0),
	m_bUseFrame(false),
	m_bMask(false),
	m_bTwoSide(false),
	m_bAlpha(false),
	m_alphaType(ALPHA_BLEND),
	m_visible(1.f),
	m_visibleGlobal(1.f),
	m_visibleLocal(1.f),
	m_amb(1.f, 1.f, 1.f, 1.f),
	m_dif(1.f, 1.f, 1.f, 1.f),
	m_emi(0.f, 0.f, 0.f, 0.f),
	m_spe(0.f, 0.f, 0.f, 0.f),
	m_bSpec(false),
	m_spePower(0.f),
	m_bTexMatrix(false),
	m_baseRotate(0.f, 0.f, 0.f),
	m_baseTranslate(0.f, 0.f, 0.f),
	m_baseScale(0.f, 0.f, 0.f),
	m_rtAmb(1.f, 1.f, 1.f, 1.f),
	m_rtDif(1.f, 1.f, 1.f, 1.f),
	m_rtEmi(0.f, 0.f, 0.f, 0.f),
	m_rtBaseEmi(0.f, 0.f, 0.f, 0.f),
	m_rtVisible(1.f)
{
	CM_MEMPROTECTOR(m_texPath, Text_File_Path_Size)
		(*m_texPath) = 0;
}

CRT_MtlStandard::CRT_MtlStandard(const CRT_MtlStandard& object)
{
	memcpy(this, &object, sizeof(CRT_MtlStandard));
	CM_MEMPROTECTOR(m_texPath, Text_File_Path_Size)
}

CRT_MtlStandard::~CRT_MtlStandard()
{
	CM_MEMUNPROTECTOR(m_texPath)
		Destroy();
}

void CRT_MtlStandard::OnLoadFinish()
{
	ConvertOption();

	m_rtAmb = m_amb;
	m_rtDif = m_dif;
	m_rtEmi = m_emi;
	m_visibleLocal = m_visible;
	m_ro.bTwoSide = m_bTwoSide;
	m_ro.bColorKey = m_bMask;
	m_ro.bEnableBlendFunc = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.dwTextureFactor = 0xFFFFFFFF;
	m_ro.Shaders[0].bUseTextureMatrix = false;

	if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texPath, m_texture.c_str()))
	{
		m_ro.iNumStage = 1;
	}
	else
	{
		m_ro.iNumStage = 0;
		(*m_texPath) = 0;
	}

	RTASSERT(strlen(m_texPath) < Text_File_Path_Size);

	UpdateVisible(true);

	if (m_bTexMatrix)
	{
		m_texMtx.Unit();
		m_texMtx.Translate(m_baseTranslate.x, m_baseTranslate.y);
		m_texMtx.RotateLZ(m_baseRotate.x);
		m_texMtx.Scale(m_baseScale.x, m_baseScale.y);
		m_ro.Shaders[0].bUseTextureMatrix = true;
		m_texMtx.SaveTo4x4(m_ro.Shaders[0].mMatrix16);
	}

}

bool CRT_MtlStandard::Create()
{
	if (m_ro.iNumStage)
		m_ro.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texPath));

	return true;
}

void CRT_MtlStandard::Destroy()
{
	RtGetRender()->DeleteShader(&m_ro);
}

void CRT_MtlStandard::Reset()
{
	m_visibleLocal = m_visible;
	m_visibleGlobal = 1.0f;

	m_rtAmb = m_amb;
	m_rtDif = m_dif;
	m_rtEmi = m_emi;
	m_delta = 0;

	UpdateVisible(true);
}

bool CRT_MtlStandard::Apply(RtgDevice* _Dev)
{
	_Dev->SetMaterial(m_rtDif, m_rtAmb, m_spe, m_rtBaseEmi + m_rtEmi, m_spePower);

	if (m_bSpec)
		_Dev->SetRenderState(RTGRS_HEIGHTLIGHT, TRUE);

	if (IsTransparent())
		_Dev->SetRenderState(RTGRS_Z_WRITE, FALSE);

	if (m_bAlpha)
	{
		switch (m_alphaType)
		{
		case ALPHA_ADD:
			SetFogWithAdd(_Dev);
			break;
		case ALPHA_SUB:
			SetFogWithAdd(_Dev);
			_Dev->SetRenderState(RTGRS_BLENDOP, RTGBOP_REVSUBTRACT);
			break;
		default:
			break;
		}
	}

	_Dev->SetShader(&m_ro);

	return true;
}

void CRT_MtlStandard::Restore(RtgDevice* _Dev)
{
	_Dev->RestoreShader(&m_ro);

	if (m_bAlpha)
	{
		switch (m_alphaType)
		{
		case ALPHA_ADD:
			RestoreFog(_Dev);
			break;
		case ALPHA_SUB:
			_Dev->SetRenderState(RTGRS_BLENDOP, RTGBOP_ADD);
			RestoreFog(_Dev);
			break;
		default:
			break;
		}
	}

	if (IsTransparent())
		_Dev->SetRenderState(RTGRS_Z_WRITE, TRUE);

	if (m_bSpec)
		_Dev->SetRenderState(RTGRS_HEIGHTLIGHT, FALSE);
}

long CRT_MtlStandard::Serialize(RtArchive& Ar)
{
	Ar << m_name
		<< m_bUseFrame
		<< m_bMask
		<< m_bTwoSide
		<< m_bAlpha
		<< m_alphaOpt
		<< m_visible
		<< m_amb
		<< m_dif
		<< m_emi
		<< m_bSpec
		<< m_spe
		<< m_spePower
		<< m_texture
		<< m_bTexMatrix
		<< m_baseRotate
		<< m_baseTranslate
		<< m_baseScale
		<< m_keyList;

	BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT

		return Ar.IsError() ? 0 : 1;
}

void CRT_MtlStandard::ConvertOption()
{
	m_alphaType = ALPHA_BLEND;

	if (m_alphaOpt == "Blend")
		m_alphaType = ALPHA_BLEND;
	else if (m_alphaOpt == "Add")
		m_alphaType = ALPHA_ADD;
	else if (m_alphaOpt == "Sub")
		m_alphaType = ALPHA_SUB;
	else if (m_alphaOpt == "Modulate")
		m_alphaType = ALPHA_MODULATE;
}

void CRT_MtlStandard::Update(SStandardKey& key)
{
	SetVisibleLocal(key.Visible);

	m_rtAmb = key.Amb;
	m_rtDif = key.Dif;
	m_rtEmi = key.Emi;

	if (m_bTexMatrix)
	{
		m_texMtx.Unit();
		m_texMtx.Translate(key.Translate.x, key.Translate.y);
		m_texMtx.RotateLZ(key.Rotate.x);
		m_texMtx.Scale(key.Scale.x, key.Scale.y);
		m_ro.Shaders[0].bUseTextureMatrix = true;
		m_texMtx.SaveTo4x4(m_ro.Shaders[0].mMatrix16);
	}
}

void CRT_MtlStandard::UpdateVisible(bool force)
{
	float vis = m_visibleGlobal * m_visibleLocal;

	if (vis > 1.0)
		vis = 1.0;

	m_rtVisible = vis;

	m_ro.bEnableBlendFunc = false;
	m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE;
	m_ro.Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
	m_ro.Shaders[0].eColorArg2 = RTGTA_TEXTURE;
	m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
	m_ro.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;

	if (m_rtVisible < 0.99999)
	{
		m_ro.bEnableBlendFunc = true;
		m_ro.dwTextureFactor = m_rtVisible * 255;
		m_ro.dwTextureFactor <<= 24;
		m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
		m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;

		if (m_bMask)
		{
			m_ro.Shaders[0].eAlphaOp = RTGTOP_MODULATE;
			m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
			m_ro.Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
		}
	}

	if (m_bAlpha)
	{
		m_ro.bEnableBlendFunc = true;
		m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;

		switch (m_alphaType)
		{
		case ALPHA_BLEND:
		{
			m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
			m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;

			if (Abs(m_rtVisible - 1.0f) >= 0.01f)
			{
				m_ro.Shaders[0].eAlphaOp = RTGTOP_MODULATE;
				m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
				m_ro.Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
			}
		}

		break;

		case ALPHA_ADD:
		{
			m_ro.eBlendSrcFunc = RTGBLEND_ONE;
			m_ro.eBlendDstFunc = RTGBLEND_ONE;

			if (Abs(m_rtVisible - 1.0f) >= 0.01f)
			{
				m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ro.eBlendDstFunc = RTGBLEND_ONE;
				m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
				m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
			}
		}

		break;

		case ALPHA_SUB:
		{
			m_ro.eBlendSrcFunc = RTGBLEND_ONE;
			m_ro.eBlendDstFunc = RTGBLEND_ONE;

			if (Abs(m_rtVisible - 1.0f) >= 0.01f)
			{
				m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ro.eBlendDstFunc = RTGBLEND_ONE;
				m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
				m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
			}
		}

		break;

		case ALPHA_MODULATE:
		{
			m_ro.eBlendSrcFunc = RTGBLEND_ZERO;
			m_ro.eBlendDstFunc = RTGBLEND_SRCCOLOR;
		}

		break;
		}
	}
}

#ifdef CHAR_VIEWER

void CRT_MtlStandard::AttachPropGrid(void* p)
{
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;
	CXTPPropertyGridItemConstraints* pList;

	CRT_MtlStandard* s = this;
	pItem = pRoot->AddChildItem(new CCustomItemString("名字", &s->m_name));
	pItem->SetReadOnly(TRUE);
	pItem = pRoot->AddChildItem(new CCustomItemBool("同步动画", &s->m_bUseFrame));
	pItem = pRoot->AddChildItem(new CCustomItemString("贴图", &s->m_texture));
	pItem = pRoot->AddChildItem(new CCustomItemBool("双面", &s->m_bTwoSide));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("漫反射", &s->m_dif));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("环境光", &s->m_amb));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("自发光", &s->m_emi));

	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("高光"));
	pItem->AddChildItem(new CCustomItemBool("使用高光", &s->m_bSpec));
	pItem->AddChildItem(new CCustomItemColor4("高光", &s->m_spe));
	pItem->AddChildItem(new CCustomItemFloat("锐度", &s->m_spePower));
	pItem->Expand();

	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Mask", &s->m_bMask));
	pItem = pRoot->AddChildItem(new CCustomItemFloat("半透明", &s->m_visible));
	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Alpha", &s->m_bAlpha));
	pItem = pRoot->AddChildItem(new CCustomItemString("Alpha模式", &s->m_alphaOpt));
	pList = pItem->GetConstraints();
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	// pList->AddConstraint("Modulate");
	pItem->SetFlags(xtpGridItemHasComboButton);

	pItem = pRoot->AddChildItem(new CCustomItemBool("修正贴图坐标", &s->m_bTexMatrix));
	pItem->AddChildItem(new CCustomItemV3("平移", &s->m_baseTranslate));
	pItem->AddChildItem(new CCustomItemV3("旋转", &s->m_baseRotate));
	pItem->AddChildItem(new CCustomItemV3("缩放", &s->m_baseScale));
	pItem->Expand();
	pRoot->Expand();
}

void CRT_MtlStandard::AttachKeyGrid(int key, void* p)
{
	CRT_MtlStandard* s = this;
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;

	SStandardKey* k;
	CString tmp;

	k = m_keyList.GetKeyFrame(key);
	if (!k) return;
	tmp.Format("%d", key);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数", tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(new CCustomItemFloat("半透明", &k->Visible));
	pRoot->AddChildItem(new CCustomItemV3("平移", &k->Translate));
	pRoot->AddChildItem(new CCustomItemV3("旋转", &k->Rotate));
	pRoot->AddChildItem(new CCustomItemV3("缩放", &k->Scale));
	pRoot->AddChildItem(new CCustomItemColor4("漫反射", &k->Dif));
	pRoot->AddChildItem(new CCustomItemColor4("环境光", &k->Amb));
	pRoot->AddChildItem(new CCustomItemColor4("自发光", &k->Emi));
	pRoot->Expand();
}

bool CRT_MtlStandard::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		m_visibleLocal = m_visible;
		Recreate();
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

bool CRT_MtlStandard::OnKeyGridNotify(int key, WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

const char* CRT_MtlStandard::TVGetName()
{
	return GetName().c_str();
}

int CRT_MtlStandard::TVGetKeyNum()
{
	return m_keyList.Size();
}

int CRT_MtlStandard::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyList.GetKeyFrame(key, frame);
	return frame;
}

void CRT_MtlStandard::TVOnNotify(ETV_OP op, long param1, long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SStandardKey k;
	SStandardKey* old;

	switch (op)
	{
	case OP_MODIFY_PROP:
		prop.Init(this);
		prop.DoModal();
		break;

	case OP_MODIFY_KEY:
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_ADD_KEY:
		m_keyList.AddKeyFrame(param1, k);
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_DEL_KEY:
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_MOVE_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_COPY_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		break;
	}
}

bool CRT_MtlStandard::CopyFrom(CRT_Material* pMat)
{
	string strType = pMat->GetRuntimeClass()->m_lpszClassName;
	string strName = pMat->GetName();
	this->SetName(strName);

	int alphaMode = 0;
	RtgTextItem* texItem = NULL;
	RtcSkinSubset* pSr = NULL;
	pMat->GetGeometry(alphaMode, texItem);
	if (alphaMode != ALPHA_NULL)
	{
		if (alphaMode == ALPHA_BLEND)
			m_alphaOpt = "Blend";
		else if (alphaMode == ALPHA_ADD)
			m_alphaOpt = "Add";
		else if (alphaMode == ALPHA_SUB)
			m_alphaOpt = "Sub";
		else if (alphaMode == ALPHA_MODULATE)
			m_alphaOpt = "Modulate";
		m_bAlpha = true;
	}
	else
		m_bAlpha = false;

	m_texture = pMat->TextureName(0);

	return true;
}

#endif

CRT_MtlMu::CRT_MtlMu()
	: m_color(1.f, 1.f, 1.f),
	m_colorType(OP_MODULATE),
	m_delta(0),
	m_bUseFrame(false),
	m_bMask(false),
	m_bTwoSide(false),
	m_bAlpha(false),
	m_alphaType(ALPHA_BLEND),
	m_visible(1.f),
	m_visibleGlobal(1.f),
	m_visibleLocal(1.f),
	m_amb(1.f, 1.f, 1.f, 1.f),
	m_dif(1.f, 1.f, 1.f, 1.f),
	m_emi(0.f, 0.f, 0.f, 0.f),
	m_rtAmb(1.f, 1.f, 1.f, 1.f),
	m_rtDif(1.f, 1.f, 1.f, 1.f),
	m_rtEmi(0.f, 0.f, 0.f, 0.f),
	m_rtVisible(1.f),
	m_rtColor(1.f, 1.f, 1.f)
{
	CM_MEMPROTECTOR(m_texBasePath, Text_File_Path_Size)
		CM_MEMPROTECTOR(m_texSpecPath, Text_File_Path_Size)
		CM_MEMPROTECTOR(m_texSpec2Path, Text_File_Path_Size)
		m_rtSpecMat.Unit();
	m_rtSpecMat2.Unit();
	(*m_texBasePath) = 0;
	(*m_texSpecPath) = 0;
	(*m_texSpec2Path) = 0;
}

CRT_MtlMu::CRT_MtlMu(const CRT_MtlMu& object)
{
	memcpy(this, &object, sizeof(CRT_MtlMu));
	CM_MEMPROTECTOR(m_texBasePath, Text_File_Path_Size)
		CM_MEMPROTECTOR(m_texSpecPath, Text_File_Path_Size)
		CM_MEMPROTECTOR(m_texSpec2Path, Text_File_Path_Size)
}

CRT_MtlMu::~CRT_MtlMu()
{
	CM_MEMUNPROTECTOR(m_texBasePath)
		CM_MEMUNPROTECTOR(m_texSpecPath)
		CM_MEMUNPROTECTOR(m_texSpec2Path)
		Destroy();
}

void CRT_MtlMu::OnLoadFinish()
{
	ConvertOption();

	m_rtDif = m_dif;
	m_rtEmi = m_emi;
	m_rtAmb = m_amb;
	m_visibleLocal = m_visible;
	m_ro.iNumStage = 0;

	float vis = m_visibleGlobal * m_visibleLocal;
	if (vis > 1.0)
		vis = 1.0;
	m_rtVisible = vis;

	if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texSpecPath, m_texSpec.c_str()))
		++m_ro.iNumStage;
	RTASSERT(strlen(m_texSpecPath) < Text_File_Path_Size);

	if (RtGetRender()->GetTextMgr()->GetTextFilePath(m_texBasePath, m_texBase.c_str()))
		++m_ro.iNumStage;
	RTASSERT(strlen(m_texBasePath) < Text_File_Path_Size);
}

bool CRT_MtlMu::Create()
{
	begin_log_tick(0);

	switch (m_ro.iNumStage)
	{
	case 1:
		m_ro.Shaders[0].SetTexture(
			RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texBasePath));
		break;
	case 2:
		m_ro.Shaders[0].SetTexture(
			RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texSpecPath));
		m_ro.Shaders[1].SetTexture(
			RtGetRender()->GetTextMgr()->CreateTextureFromFile_Direct(m_texBasePath));
		break;
	default:
		break;
	}

	end_log_tick(0, "CRT_MtlStandard::Create()");

	return true;
}

void CRT_MtlMu::Destroy()
{
	RtGetRender()->DeleteShader(&m_ro);
}

void CRT_MtlMu::Reset()
{
	m_visibleLocal = m_visible;
	m_visibleGlobal = 1.0f;
	m_delta = 0;

	float vis = m_visibleGlobal * m_visibleLocal;
	if (vis > 1.0)
		vis = 1.0;
	m_rtVisible = vis;
}

bool CRT_MtlMu::Apply(RtgDevice* _Dev)
{
	RtgVertex4 spe = RtgVertex4(0.f, 0.f, 0.f, 0.f);
	float spePower = 0;

	_Dev->SetMaterial(m_rtDif, m_rtAmb, spe, m_rtEmi, spePower);
	m_rtVisible = m_visibleGlobal * m_visibleLocal;

	if (IsTransparent())
		_Dev->SetRenderState(RTGRS_Z_WRITE, FALSE);

	m_ro.bEnableBlendFunc = IsTransparent();
	m_ro.bTwoSide = m_bTwoSide;
	m_ro.bColorKey = m_bMask;
	m_ro.bEnableAlphaTest = false;
	m_ro.dwTextureFactor = RtgVectorToColor(m_rtColor, m_rtVisible);

	switch (m_colorType)
	{
	case OP_MODULATE:
		m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE;
		break;
	case OP_MODULATE2X:
		m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE2X;
		break;
	case OP_MODULATE4X:
		m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE4X;
		break;
	default:
		m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE;
		break;
	}

	m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
	m_ro.Shaders[0].eColorArg2 = RTGTA_TFACTOR;
	m_ro.Shaders[0].bUseTextureMatrix = true;
	m_rtSpecMat.SaveTo4x4(m_ro.Shaders[0].mMatrix16);
	m_ro.Shaders[1].eColorOp = RTGTOP_MULTIPLYADD;
	m_ro.Shaders[1].eColorArg0 = RTGTA_CURRENT;
	m_ro.Shaders[1].eColorArg1 = RTGTA_TEXTURE;
	m_ro.Shaders[1].eColorArg2 = RTGTA_DIFFUSE;
	m_ro.Shaders[1].bUseTextureMatrix = false;
	m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
	m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
	m_ro.Shaders[0].eTexCoordIndex = RTGTC_PASSTHRU0;
	m_ro.Shaders[1].eTexCoordIndex = RTGTC_PASSTHRU0;

	if (m_bMask)
	{
		m_ro.Shaders[1].eAlphaOp = RTGTOP_MODULATE;
		m_ro.Shaders[1].eAlphaArg1 = RTGTA_CURRENT;
		m_ro.Shaders[1].eAlphaArg2 = RTGTA_TEXTURE;
	}
	else
	{
		m_ro.Shaders[1].eAlphaOp = RTGTOP_SELECTARG1;
		m_ro.Shaders[1].eAlphaArg1 = RTGTA_CURRENT;
	}

	switch (m_uvType)
	{
	case UV_CPOS:
		m_ro.Shaders[0].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;
		break;
	case UV_CNOR:
		m_ro.Shaders[0].eTexCoordIndex = RTGTC_CAMERASPACENORMAL;
		break;
	case UV_CREL:
		m_ro.Shaders[0].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR;
		break;
	default:
		break;
	}

	_Dev->SetShader(&m_ro);

	return true;
}

void CRT_MtlMu::Restore(RtgDevice* _Dev)
{
	if (IsTransparent())
		_Dev->SetRenderState(RTGRS_Z_WRITE, TRUE);
}

void CRT_MtlMu::ConvertOption()
{
	m_alphaType = ALPHA_BLEND;

	if (m_alphaOpt == "Blend")
		m_alphaType = ALPHA_BLEND;

	m_uvType = UV_BASE;

	if (m_uvOpt == "Base")
		m_uvType = UV_BASE;
	else if (m_uvOpt == "XY")
		m_uvType = UV_XY;
	else if (m_uvOpt == "YZ")
		m_uvType = UV_YZ;
	else if (m_uvOpt == "ZX")
		m_uvType = UV_ZX;
	else if (m_uvOpt == "CPOS")
		m_uvType = UV_CPOS;
	else if (m_uvOpt == "CNOR")
		m_uvType = UV_CNOR;
	else if (m_uvOpt == "CREL")
		m_uvType = UV_CREL;

	m_colorType = OP_MODULATE;

	if (m_colorOpt == "1X")
		m_colorType = OP_MODULATE;
	else if (m_colorOpt == "2X")
		m_colorType = OP_MODULATE2X;
	else if (m_colorOpt == "4X")
		m_colorType = OP_MODULATE4X;

	m_uvType2 = UV_BASE;

	if (m_uvOpt2 == "Base")
		m_uvType = UV_BASE;
	else if (m_uvOpt2 == "XY")
		m_uvType2 = UV_XY;
	else if (m_uvOpt2 == "YZ")
		m_uvType2 = UV_YZ;
	else if (m_uvOpt2 == "ZX")
		m_uvType2 = UV_ZX;
	else if (m_uvOpt2 == "CPOS")
		m_uvType2 = UV_CPOS;
	else if (m_uvOpt2 == "CNOR")
		m_uvType2 = UV_CNOR;
	else if (m_uvOpt2 == "CREL")
		m_uvType2 = UV_CREL;

	m_colorType2 = OP_MODULATE;

	if (m_colorOpt2 == "1X")
		m_colorType = OP_MODULATE;
	else if (m_colorOpt2 == "2X")
		m_colorType2 = OP_MODULATE2X;
	else if (m_colorOpt2 == "4X")
		m_colorType2 = OP_MODULATE4X;
}

long CRT_MtlMu::Serialize(RtArchive& Ar)
{
	Ar << m_name
		<< m_bUseFrame
		<< m_bMask
		<< m_bTwoSide
		<< m_bAlpha
		<< m_alphaOpt
		<< m_visible
		<< m_amb
		<< m_dif
		<< m_emi
		<< m_colorOpt
		<< m_uvOpt
		<< m_texBase
		<< m_texSpec
		<< m_keyList;

	BEGIN_VERSION_CONVERT(1)
		END_VERSION_CONVERT

		return Ar.IsError() ? 0 : 1;
}

#ifdef CHAR_VIEWER

void CRT_MtlMu::AttachPropGrid(void* p)
{
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;
	CXTPPropertyGridItemConstraints* pList;

	pItem = pRoot->AddChildItem(new CCustomItemString("名字", &m_name));
	pItem->SetReadOnly(TRUE);

	pItem = pRoot->AddChildItem(new CCustomItemBool("同步动画", &m_bUseFrame));
	pItem = pRoot->AddChildItem(new CCustomItemString("基本贴图", &m_texBase));

	pItem = pRoot->AddChildItem(new CCustomItemBool("双面", &m_bTwoSide));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("漫反射", &m_dif));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("环境光", &m_amb));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("自发光", &m_emi));

	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Mask", &m_bMask));
	pItem = pRoot->AddChildItem(new CCustomItemFloat("半透明", &m_visible));
	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Alpha", &m_bAlpha));
	pItem = pRoot->AddChildItem(new CCustomItemString("Alpha模式", &m_alphaOpt));
	pList = pItem->GetConstraints();
	pList->AddConstraint("Blend");
	// pList->AddConstraint("Add");
	// pList->AddConstraint("Sub");
	// pList->AddConstraint("Modulate");
	pItem->SetFlags(xtpGridItemHasComboButton);

	pItem = pRoot->AddChildItem(new CCustomItemString("高光贴图", &m_texSpec));
	pItem = pRoot->AddChildItem(new CCustomItemString("UV模式", &m_uvOpt));
	pList = pItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("XY");
	pList->AddConstraint("YZ");
	pList->AddConstraint("ZX");
	pList->AddConstraint("CPOS");
	pList->AddConstraint("CNOR");
	pList->AddConstraint("CREL");
	pItem->SetFlags(xtpGridItemHasComboButton);
	pItem = pRoot->AddChildItem(new CCustomItemString("颜色模式", &m_colorOpt));
	pList = pItem->GetConstraints();
	pList->AddConstraint("1X");
	pList->AddConstraint("2X");
	pList->AddConstraint("4X");
	pItem->SetFlags(xtpGridItemHasComboButton);

	pItem = pRoot->AddChildItem(new CCustomItemString("高光贴图2", &m_texSpec2));
	pItem = pRoot->AddChildItem(new CCustomItemString("UV模式", &m_uvOpt2));
	pList = pItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("XY");
	pList->AddConstraint("YZ");
	pList->AddConstraint("ZX");
	pList->AddConstraint("CPOS");
	pList->AddConstraint("CNOR");
	pList->AddConstraint("CREL");
	pItem->SetFlags(xtpGridItemHasComboButton);
	pItem = pRoot->AddChildItem(new CCustomItemString("颜色模式", &m_colorOpt2));
	pList = pItem->GetConstraints();
	pList->AddConstraint("1X");
	pList->AddConstraint("2X");
	pList->AddConstraint("4X");
	pItem->SetFlags(xtpGridItemHasComboButton);

	pRoot->Expand();
}

void CRT_MtlMu::AttachKeyGrid(int key, void* p)
{
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;

	CRT_MtlMu* s = this;
	SMuKey* k;
	CString tmp;
	k = m_keyList.GetKeyFrame(key);
	if (!k) return;
	tmp.Format("%d", key);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数", tmp));
	pItem->SetReadOnly(TRUE);

	pRoot->AddChildItem(new CCustomItemV3("平移", &k->Translate));
	pRoot->AddChildItem(new CCustomItemV3("旋转", &k->Rotate));
	pRoot->AddChildItem(new CCustomItemV3("缩放", &k->Scale));
	pRoot->AddChildItem(new CCustomItemColor3("颜色", &k->Color));

	pRoot->AddChildItem(new CCustomItemV3("平移2", &k->Translate2));
	pRoot->AddChildItem(new CCustomItemV3("旋转2", &k->Rotate2));
	pRoot->AddChildItem(new CCustomItemV3("缩放2", &k->Scale2));
	pRoot->AddChildItem(new CCustomItemColor3("颜色2", &k->Color2));

	pRoot->Expand();
}

bool CRT_MtlMu::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		m_visibleLocal = m_visible;
		Destroy();
		Create();
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

bool CRT_MtlMu::OnKeyGridNotify(int key, WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

const char* CRT_MtlMu::TVGetName()
{
	return GetName().c_str();
}

int CRT_MtlMu::TVGetKeyNum()
{
	return m_keyList.Size();
}

int CRT_MtlMu::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyList.GetKeyFrame(key, frame);
	return frame;
}

void CRT_MtlMu::TVOnNotify(ETV_OP op, long param1, long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SMuKey k;
	SMuKey* old;

	switch (op)
	{
	case OP_MODIFY_PROP:
		prop.Init(this);
		prop.DoModal();
		break;

	case OP_MODIFY_KEY:
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_ADD_KEY:
		m_keyList.AddKeyFrame(param1, k);
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_DEL_KEY:
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_MOVE_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_COPY_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		break;
	}
}

bool CRT_MtlMu::CopyFrom(CRT_Material* pMat)
{
	string strType = pMat->GetRuntimeClass()->m_lpszClassName;
	string strName = pMat->GetName();
	this->SetName(strName);
	m_texBase = "USE BASE";

	int alphaMode = 0;
	RtgTextItem* texItem = NULL;
	RtcSkinSubset* pSr = NULL;
	pMat->GetGeometry(alphaMode, texItem);

	if (alphaMode != ALPHA_NULL)
	{
		if (alphaMode == ALPHA_BLEND)
			m_alphaOpt = "Blend";
		else if (alphaMode == ALPHA_ADD)
			m_alphaOpt = "Add";
		else if (alphaMode == ALPHA_SUB)
			m_alphaOpt = "Sub";
		else if (alphaMode == ALPHA_MODULATE)
			m_alphaOpt = "Modulate";
		m_bAlpha = true;
	}
	else
		m_bAlpha = false;

	m_texBase = pMat->TextureName(0);
	m_texSpec = pMat->TextureName(1);
	m_texSpec2 = pMat->TextureName(2);

	return true;
}

#endif

// -------------------------------------------------------------------------------------------
// Mtl Blend
// -------------------------------------------------------------------------------------------
CRT_MtlBlend::CRT_MtlBlend()
{
	m_delta = 0;
	m_skin = NULL;
	m_rs = NULL;

	m_bUseFrame = false;
	m_bMask = false;
	m_bTwoSide = false;
	m_bAlpha = false;
	m_visible = 1.0f;
	m_visibleGlobal = 1.0f;
	m_visibleLocal = 1.0f;

	m_amb.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_dif.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_emi.Set(0.0f, 0.0f, 0.0f, 0.0f);

	m_rtAmb.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_rtDif.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_rtEmi.Set(0.0f, 0.0f, 0.0f, 0.0f);
	m_rtVisible = 1.0f;
	m_bInit = false;

	for (int i = 0; i < MAX_BLEND_CHANNEL; i++)
	{
		m_channel[i].texItem = NULL;
	}
}

CRT_MtlBlend::~CRT_MtlBlend()
{
	Destroy();
}

#ifdef CHAR_VIEWER
void CRT_MtlBlend::AttachPropGrid(void* p)
{
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;
	//	CXTPPropertyGridItemConstraints  *pList;

	pItem = pRoot->AddChildItem(new CCustomItemString("名字", &m_name));
	pItem->SetReadOnly(TRUE);

	pItem = pRoot->AddChildItem(new CCustomItemBool("同步动画", &m_bUseFrame));
	pItem = pRoot->AddChildItem(new CCustomItemBool("双面", &m_bTwoSide));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("漫反射", &m_dif));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("环境光", &m_amb));
	pItem = pRoot->AddChildItem(new CCustomItemColor4("自发光", &m_emi));

	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Mask", &m_bMask));
	pItem = pRoot->AddChildItem(new CCustomItemBool("使用Alpha", &m_bAlpha));
	pItem = pRoot->AddChildItem(new CCustomItemFloat("半透明", &m_visible));
	for (int i = 0; i < MAX_BLEND_CHANNEL; i++)
	{
		CString tmp;
		tmp.Format("channel %d", i);
		pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(tmp));
		pItem->AddChildItem(new CCustomItemString("贴图", &m_channel[i].Tex));
		// pItem->AddChildItem(new CCustomItemBool("使用Mask",&m_channel[i].bMask));
		// pItem->AddChildItem(new CCustomItemBool("使用Alpha",&m_channel[i].bAlpha));
	}
	pRoot->Expand();
}

void CRT_MtlBlend::AttachKeyGrid(int key, void* p)
{
	CXTPPropertyGridItem* pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem* pItem;

	CRT_MtlBlend* s = this;
	SBlendKey* k;
	CString tmp;
	k = m_keyList.GetKeyFrame(key);
	if (!k) return;
	tmp.Format("%d", key);
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("帧数", tmp));
	pItem->SetReadOnly(TRUE);

	pRoot->AddChildItem(new CCustomItemFloat("1", &k->w1));
	pRoot->AddChildItem(new CCustomItemFloat("2", &k->w2));
	pRoot->AddChildItem(new CCustomItemFloat("3", &k->w3));
	pRoot->Expand();
}

bool CRT_MtlBlend::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		m_visibleLocal = m_visible;
		Destroy();
		Create();
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

bool CRT_MtlBlend::OnKeyGridNotify(int key, WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch (wParam)
	{
	case XTP_PGN_ITEMVALUE_CHANGED:
		return false;

	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	}
	return false;
}

const char* CRT_MtlBlend::TVGetName()
{
	return GetName().c_str();
}

int CRT_MtlBlend::TVGetKeyNum()
{
	return m_keyList.Size();
}

int CRT_MtlBlend::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyList.GetKeyFrame(key, frame);
	return frame;
}

void CRT_MtlBlend::TVOnNotify(ETV_OP op, long param1, long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SBlendKey k;
	SBlendKey* old;

	switch (op)
	{
	case OP_MODIFY_PROP:
		prop.Init(this);
		prop.DoModal();
		break;

	case OP_MODIFY_KEY:
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_ADD_KEY:
		m_keyList.AddKeyFrame(param1, k);
		keyInfo.Init(param1, this);
		keyInfo.DoModal();
		break;

	case OP_DEL_KEY:
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_MOVE_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		m_keyList.DelKeyFrame(param1);
		break;

	case OP_COPY_KEY:
		old = m_keyList.GetKeyFrame(param1);
		if (!old) break;
		m_keyList.AddKeyFrame(param2, *old);
		break;
	}
}
bool CRT_MtlBlend::CopyFrom(CRT_Material* pMat)
{
	string strType = pMat->GetRuntimeClass()->m_lpszClassName;
	string strName = pMat->GetName();
	this->SetName(strName);

	int alphaMode = 0;
	RtgTextItem* texItem = NULL;
	RtcSkinSubset* pSr = NULL;
	pMat->GetGeometry(alphaMode, texItem);
	if (alphaMode != ALPHA_NULL)
		m_bAlpha = true;
	else
		m_bAlpha = false;

	return true;
}
#endif
//
//bool CRT_MtlBlend::Init(RtgDevice *dev,CRT_SkinInstance *skin,RtcSkinSubset *rs)
//{
//    CRT_Material::Init(dev,skin,rs);
//    if(skin && rs)
//    {
//        m_skin = skin;
//        m_rs = rs;
//    }
//
//    if(!m_skin) return false;
//    m_bInit = true;
//    m_visibleLocal = m_visible;
//    ConvertOption();
//    for(int i=0; i<MAX_BLEND_CHANNEL; i++)
//    {
//        //m_channel[i].TexId = dev->m_Texture.CreateTexture(m_channel[i].Tex.c_str(),RTGTBT_AUTO,true,1);
//        m_channel[i].texItem = RtGetRender()->GetTextMgr()->CreateTextureFromFile(m_channel[i].Tex.c_str());
//    }
//    return true;
//}
//
//bool CRT_MtlBlend::Exit()
//{
//    if (!RtGetRender())
//        return false;
//
//    m_bInit = false;
//    m_skin = NULL;
//    m_rs = NULL;
//    for(int i=0; i<MAX_BLEND_CHANNEL; i++)
//    {
//        if (m_channel[i].texItem)
//            RtGetRender()->GetTextMgr()->ReleaseTexture(m_channel[i].texItem);
//        m_channel[i].texItem = NULL;
//
//        //      if (m_device && m_channel[i].TexId)
//        //    m_device->m_Texture.DeleteTexture(m_channel[i].TexId);
//        //m_channel[i].TexId = 0;
//    }
//    return true;
//}
//
//bool CRT_MtlBlend::IsActive()
//{
//    return m_bInit;
//}
//

bool CRT_MtlBlend::Apply(RtgDevice* _Dev)
{
	RtgVertex4 spe = RtgVertex4(0.f, 0.f, 0.f, 0.f);
	float spePower = 0;
	_Dev->SetMaterial(m_rtDif, m_rtAmb, spe, m_rtEmi, spePower);

	if (IsTransparent())
		_Dev->SetRenderState(RTGRS_Z_WRITE, FALSE);

	m_rtVisible = m_visibleLocal * m_visibleGlobal;

	for (int i = 0; i < MAX_BLEND_CHANNEL; ++i)
	{
		if (!m_channel[i].texItem || m_channel[i].Weight < 0.01f)
			continue;

		m_ro.iNumStage = 1;
		m_ro.bTwoSide = m_bTwoSide;
		m_ro.bColorKey = m_bMask;

		if (i == 0)
			m_ro.bEnableBlendFunc = m_bAlpha;
		else
			m_ro.bEnableBlendFunc = true;
		m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
		m_ro.dwTextureFactor = RtgVectorToColor(RtgVertex3(1, 1, 1), m_rtVisible * m_channel[i].Weight);

		m_ro.Shaders[0].SetTexture(m_channel[i].texItem);
		m_ro.Shaders[0].eColorOp = RTGTOP_MODULATE;
		m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
		m_ro.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;

		if (m_bAlpha)
		{
			m_ro.Shaders[0].eAlphaOp = RTGTOP_MODULATE;
			m_ro.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
			m_ro.Shaders[0].eAlphaArg2 = RTGTA_TFACTOR;
		}
		else
		{
			m_ro.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
			m_ro.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
		}

		_Dev->SetShader(&m_ro);
	}

	return true;
}

void CRT_MtlBlend::Restore(RtgDevice* _Dev)
{
	RTASSERT(false);
}

void CRT_MtlBlend::ConvertOption()
{

}

void CRT_MtlBlend::Update(SBlendKey& key)
{
	m_channel[0].Weight = key.w1;
	m_channel[1].Weight = key.w2;
	m_channel[2].Weight = key.w3;
}

bool CRT_MtlBlend::RequestTick()
{
	return (!m_bUseFrame && m_keyList.Size() != 0);
}

void CRT_MtlBlend::Tick(float deltaMill)
{
    unsigned long beg,end;
    beg = m_keyList.GetBegFrame();
    end = m_keyList.GetEndFrame();
    m_delta += deltaMill;
    if(beg == end) return;
    UseFrame(beg + (UINT)(m_delta/33)%(end-beg));
}

bool CRT_MtlBlend::RequestUseFrame()
{
	return (m_bUseFrame && m_keyList.Size() != 0);
}

void CRT_MtlBlend::UseFrame(unsigned long frame)
{
	SBlendKey key;
	//插值算帧
	if (!m_keyList.GetKeyFrame(frame, key))
		return;
	//渲染差值帧
	Update(key);
}

bool CRT_MtlBlend::SetBaseColor(RtgVertex3& color)
{
	m_rtEmi.x = color.x;
	m_rtEmi.y = color.y;
	m_rtEmi.z = color.z;
	return true;
}

bool CRT_MtlBlend::GetBaseColor(RtgVertex3& color)
{
	color.x = m_rtEmi.x;
	color.y = m_rtEmi.y;
	color.z = m_rtEmi.z;
	return true;
}

long CRT_MtlBlend::Serialize(RtArchive& Ar)
{
	Ar << m_name << m_bUseFrame << m_bTwoSide << m_bMask << m_bAlpha;
	Ar << m_visible;
	Ar << m_amb << m_dif << m_emi;
	Ar << m_keyList;
	for (int i = 0; i < MAX_BLEND_CHANNEL; i++)
		Ar << m_channel[i];

	BEGIN_VERSION_CONVERT(1)
		// read/write extra data
		END_VERSION_CONVERT
		return Ar.IsError() ? 0 : 1;
}

bool CRT_MtlBlend::IsTransparent()
{
	if (Abs(m_rtVisible - 1.0f) >= 0.01f || m_bAlpha)
		return true;

	return false;
}

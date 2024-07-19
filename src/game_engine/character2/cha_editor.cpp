
#ifdef ACTOR_EDITOR
#include "actor.h"
extern bool g_bsave;

struct SAlphaMap  
{
	const char *strAlpha;
	long alpha;
};

SAlphaMap alphaMap[] = 
{
	{"None",ALPHA_NULL},
	{"Null",ALPHA_NULL},
	{"Mask",ALPHA_MASK},
	{"Blend",ALPHA_BLEND},
	{"Add",ALPHA_ADD},
	{"Sub",ALPHA_SUB},
	{"Modulate",ALPHA_MODULATE},
	{"Modulate2X",ALPHA_MODULATE2X},
};

long ConvertAlphaMode(string &strAlpha)
{
	for(int i=0; i<countof(alphaMap); i++)
	{
		if(strAlpha == alphaMap[i].strAlpha)
			return alphaMap[i].alpha;
	}
	return ALPHA_NULL;
}

const char *ConvertAlphaMode(long alpha)
{
	for(int i=0; i<countof(alphaMap); i++)
	{
		if(alpha == alphaMap[i].alpha)
			return alphaMap[i].strAlpha;
	}
	return "None";
}

void AttachPropGrid(SFrameBufferOption &fo, void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints *pList;

    pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("���ѡ��"));
	pItem->SetReadOnly(TRUE);
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("˫��",  &fo.twoSide));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��Ӱ",  &fo.shadow));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ZTest", &fo.ztest));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alpha", &fo.strAlphaMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Mask");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);
	pItem->Expand();
}

bool OnPropGridNotify(SFrameBufferOption &fo, WPARAM wParam, LPARAM lParam)
{
	fo.alphaMode = ConvertAlphaMode(fo.strAlphaMode);
	return false;
}

void AttachPropGrid(SRenderOption &ro, void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��Ⱦѡ��"));
	pItem->SetReadOnly(TRUE);

	pItem->AddChildItem(RT_NEW CCustomItemFloat("��͸��",&ro.visible));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&ro.dif));

	pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&ro.amb));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("�Է���",&ro.emi));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("�߹�",&ro.spe));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("�߹�ǿ��",&ro.spePower));

	pItem->Expand();
}

void AttachKeyGrid(SRenderOption &ro, void *p)
{
	AttachPropGrid(ro, p);
}

/*
void AttachKeyGrid(SRenderStage &rs, void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Stageѡ��"));
	pItem->SetReadOnly(TRUE);

	pItem->AddChildItem(RT_NEW CCustomItemV3("ƽ��",&rs.translate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת",&rs.rotate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����",&rs.scale));

	pItem->Expand();
}
*/

// -------------------------------------------------------------------------------------------
// material lib
// -------------------------------------------------------------------------------------------
void CRT_MaterialLib::AttachGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pItemMtl,*pItemList;
	// CXTPItemConstraints  *pList;

	int i;
	CString tmp;

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Merge Material"));
	pItem->SetFlags(pgiHasExpandButton);

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Add Material"));
	pItem->SetFlags(pgiHasExpandButton);
	for(i=0; i<m_mtlVec.size(); i++)
	{
		CRT_Material *mtl;
		mtl = m_mtlVec[i];

		tmp.Format("%d",i);
		pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Mtl Idx",tmp));
		pItem->SetFlags(pgiHasExpandButton);

		tmp.Format("%s",mtl->GetName().c_str());
		pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Mtl Name",tmp));
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
				CRT_Material *mtl = GetMtlByIdx(atol(pItem->GetParentItem()->GetValue()));
				std::string tmp = pItem->GetValue();
				if(mtl) mtl->SetName(tmp);
			}
			return true;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "Mtl Idx") 
			{
				if(MessageBox(NULL,"ȷʵҪɾ����","��ʾ",MB_OKCANCEL)!=IDOK)
					return false;
				CRT_Material *tmp = GetMtlByIdx(atol(pItem->GetValue()));
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

				CRT_MaterialLib *lib = (CRT_MaterialLib*)ACTOR()->LoadFromFile(path, ACTOR_RES_TYPE_MTLLIB);
				if(!lib)	
				{
					RtCoreLog().Info("load mtl lib file error");
					return false;
				}
				for(int i=0; i<lib->m_mtlVec.size(); i++)
				{
					CRT_Material *mtl = lib->m_mtlVec[i];
					AddMaterial(mtl);
				}
				// how to delete it ?
				// ...
				// delete lib;
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
		if(idx<0 || idx>=m_mtlVec.size()) return NULL;
		return m_mtlVec[idx];
	}
	return NULL;
}

// -------------------------------------------------------------------------------------------
// material standard
// -------------------------------------------------------------------------------------------
void CRT_MtlStandard::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;
	CRT_MtlStandard *s = this;
// add by faya for autosave 2/1/2007----------------------------------------------------------
	RtIni ini;
	const char* tmp;
	if (ini.OpenFile("save.ini") && g_bsave )
	{
		ini.GetEntry("Graph","SyncActor",&s->m_bSyncActorFrame);
        ini.GetEntry("Graph","TwoSide",&s->m_bTwoSide);
		ini.GetEntry("Graph","Shadow",&s->m_bShadow);
		ini.GetEntry("Graph","ZTest",&s->m_bZTest);
		tmp = ini.GetEntry("Graph","Alpha");
		s->m_strAlphaMode = tmp;


	    ini.GetEntry("Graph","DiffuseA",&s->m_defKey.dif.w);
        ini.GetEntry("Graph","DiffuseR",&s->m_defKey.dif.x);
		ini.GetEntry("Graph","DiffuseG",&s->m_defKey.dif.y);
		ini.GetEntry("Graph","DiffuseB",&s->m_defKey.dif.z);

		ini.GetEntry("Graph","AmbientA",&s->m_defKey.amb.w);
		ini.GetEntry("Graph","AmbientR",&s->m_defKey.amb.x);
		ini.GetEntry("Graph","AmbientG",&s->m_defKey.amb.y);
		ini.GetEntry("Graph","AmbientB",&s->m_defKey.amb.z);

		ini.GetEntry("Graph","EmissionA",&s->m_defKey.emi.w);
		ini.GetEntry("Graph","EmissionR",&s->m_defKey.emi.x);
		ini.GetEntry("Graph","EmissionG",&s->m_defKey.emi.y);
		ini.GetEntry("Graph","EmissionB",&s->m_defKey.emi.z);

		ini.GetEntry("Graph","Visible",&s->m_defKey.visible);

		ini.CloseFile();
		DataChange(0,0);
	}

//------------------------------------------------------------------------------

	
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������Ϣ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("����",&s->m_name));
	pSubItem->SetReadOnly(TRUE);

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ͬ������",&s->m_bSyncActorFrame));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("˫��",&s->m_bTwoSide));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��Ӱ",&s->m_bShadow));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ZTest",&s->m_bZTest));

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ",&s->m_strAlphaMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Mask");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&s->m_defKey.dif));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&s->m_defKey.amb));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("�Է���",&s->m_defKey.emi));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemFloat("͸����",&s->m_defKey.visible));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������ͼ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texBase));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bBaseTexMatrix));
	pItem->Expand();

	// effect tex1
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Ч����ͼ1"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("���ģʽ",&s->m_strEftBlendMode1));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);

	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texEffect1));
	pItem->AddChildItem(RT_NEW CCustomItemString("Mask��ͼ",&s->m_texEffectMask1));
	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ͼ��ɫ",&s->m_defKey.envColor));
	pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bEffectMatrix1));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ����ģʽ",&s->m_strEffectUv1));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("XY");
	pList->AddConstraint("YZ");
	pList->AddConstraint("ZX");
	pList->AddConstraint("Camera Pos");
	pList->AddConstraint("Camera Nor");
	pList->AddConstraint("Camera Rel");
	pSubItem->SetFlags(pgiHasComboButton);
	pItem->Expand();

	// effect tex2
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Ч����ͼ2"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("���ģʽ",&s->m_strEftBlendMode2));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);

	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texEffect2));
	pItem->AddChildItem(RT_NEW CCustomItemString("Mask��ͼ",&s->m_texEffectMask2));
	// pItem->AddChildItem(RT_NEW CCustomItemColor3("��ͼ��ɫ",&s->m_defKey.envColor));
	pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bEffectMatrix2));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ����ģʽ",&s->m_strEffectUv2));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("XY");
	pList->AddConstraint("YZ");
	pList->AddConstraint("ZX");
	pList->AddConstraint("Camera Pos");
	pList->AddConstraint("Camera Nor");
	pList->AddConstraint("Camera Rel");
	pSubItem->SetFlags(pgiHasComboButton);
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�߹���Ϣ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("ģʽ",&s->m_strHlMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Vertex");
	pList->AddConstraint("Texture");
	pSubItem->SetFlags(pgiHasComboButton);

	pItem->AddChildItem(RT_NEW CCustomItemColor4("��ɫ",&s->m_defKey.spe));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("���",&s->m_defKey.spePower));
	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texHl));
	pItem->Expand();

	pRoot->Expand();
}

void CRT_MtlStandard::AttachKeyGrid(int key,void *p)
{
	CRT_MtlStandard *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	SMtlStandardKey *k;
	CString tmp;

	k = m_keyVec.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("֡��",tmp));
	pItem->SetReadOnly(TRUE);


	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������Ϣ"));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("��͸��",&k->visible));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&k->dif));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&k->amb));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("�Է���",&k->emi));
	pItem->AddChildItem(RT_NEW CCustomItemColor4("�߹�",&k->spe));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("�߹�ǿ��",&k->spePower));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������ͼ��Ϣ"));
	pItem->AddChildItem(RT_NEW CCustomItemV3("ƽ��",&k->baseTranslate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת",&k->baseRotate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����",&k->baseScale));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Ч����ͼ��Ϣ1"));
	pItem->AddChildItem(RT_NEW CCustomItemV3("ƽ��",&k->effectTranslate1));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת",&k->effectRotate1));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����",&k->effectScale1));
	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ɫ",&k->envColor));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Ч����ͼ��Ϣ2"));
	pItem->AddChildItem(RT_NEW CCustomItemV3("ƽ��",&k->effectTranslate2));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת",&k->effectRotate2));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����",&k->effectScale2));
	pItem->Expand();

	pRoot->Expand();
}

bool CRT_MtlStandard::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
//add by faya for autoupdate config----------------------------------------------------------------------
	RtIni ini;
#define INI_ADD_ENTRY(s, i, v)  if (!ini.SetEntry(s, i, v)) ini.AddEntry(s, i, v)
		const char* strGraph = "Graph";
	if (ini.OpenFile("save.ini", true) && g_bsave)
	{
		RtCoreLog().Info(" д�������ļ� [save.ini]\n");


	if (!ini.FindSection(strGraph)) ini.AddSection(strGraph);
	INI_ADD_ENTRY(strGraph, "syncActor",(long)m_bSyncActorFrame );
	INI_ADD_ENTRY(strGraph, "TwoSide", (long)m_bTwoSide);
	INI_ADD_ENTRY(strGraph, "Shadow", (long)m_bShadow);
	INI_ADD_ENTRY(strGraph, "ZTest", (long)m_bZTest);
	INI_ADD_ENTRY(strGraph, "Alpha", m_strAlphaMode.c_str());
	INI_ADD_ENTRY(strGraph,"DiffuseA",m_defKey.dif.w);
    INI_ADD_ENTRY(strGraph,"DiffuseR",m_defKey.dif.x);
    INI_ADD_ENTRY(strGraph,"DiffuseG",m_defKey.dif.y);
	INI_ADD_ENTRY(strGraph,"DiffuseB",m_defKey.dif.z);
	INI_ADD_ENTRY(strGraph,"EmissionA",m_defKey.emi.w);
	INI_ADD_ENTRY(strGraph,"EmissionR",m_defKey.emi.x);
	INI_ADD_ENTRY(strGraph,"EmissionG",m_defKey.emi.y);
	INI_ADD_ENTRY(strGraph,"EmissionB",m_defKey.emi.z);
	INI_ADD_ENTRY(strGraph,"AmbientA",m_defKey.amb.w);
	INI_ADD_ENTRY(strGraph,"AmbientR",m_defKey.amb.x);
	INI_ADD_ENTRY(strGraph,"AmbientG",m_defKey.amb.y);
	INI_ADD_ENTRY(strGraph,"AmbientB",m_defKey.amb.z);
    INI_ADD_ENTRY(strGraph,"Visible",m_defKey.visible);

	ini.CloseFile();
	}
//---------------------------------------------------------------------------------------------------------
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			m_alphaMode = ConvertAlphaMode(m_strAlphaMode);

			if(m_strHlMode == "Null")			{ m_hlMode = HL_MODE_NULL; }
			else if(m_strHlMode == "Texture")	{ m_hlMode = HL_MODE_TEXTURE; }
			else if(m_strHlMode == "Vertex")	{ m_hlMode = HL_MODE_VERTEX; }

			if(m_strEffectUv1 == "Base")			{ m_effectUV1 = ENV_UV_BASE; }
			else if(m_strEffectUv1 == "XY")			{ m_effectUV1 = ENV_UV_XY; }
			else if(m_strEffectUv1 == "YZ")			{ m_effectUV1 = ENV_UV_YZ; }
			else if(m_strEffectUv1 == "ZX")			{ m_effectUV1 = ENV_UV_ZX; }
			else if(m_strEffectUv1 == "Camera Pos")	{ m_effectUV1 = ENV_UV_CPOS; }
			else if(m_strEffectUv1 == "Camera Nor")	{ m_effectUV1 = ENV_UV_CNOR; }			
			else if(m_strEffectUv1 == "Camera Rel")	{ m_effectUV1 = ENV_UV_CREL; }			

			if(m_strEftBlendMode1 == "Add")				{ m_effectBlendMode1 = ALPHA_ADD; }
			else if(m_strEftBlendMode1 == "Sub")		{ m_effectBlendMode1 = ALPHA_SUB; }
			else if(m_strEftBlendMode1 == "Modulate")   { m_effectBlendMode1 = ALPHA_MODULATE; }
			else if(m_strEftBlendMode1 == "Modulate2X") { m_effectBlendMode1 = ALPHA_MODULATE2X; }
			
			if(m_strEffectUv2 == "Base")			{ m_effectUV2 = ENV_UV_BASE; }
			else if(m_strEffectUv2 == "XY")			{ m_effectUV2 = ENV_UV_XY; }
			else if(m_strEffectUv2 == "YZ")			{ m_effectUV2 = ENV_UV_YZ; }
			else if(m_strEffectUv2 == "ZX")			{ m_effectUV2 = ENV_UV_ZX; }
			else if(m_strEffectUv2 == "Camera Pos")	{ m_effectUV2 = ENV_UV_CPOS; }
			else if(m_strEffectUv2 == "Camera Nor")	{ m_effectUV2 = ENV_UV_CNOR; }			
			else if(m_strEffectUv2 == "Camera Rel")	{ m_effectUV2 = ENV_UV_CREL; }			

			if(m_strEftBlendMode2 == "Add")				{ m_effectBlendMode2 = ALPHA_ADD; }
			else if(m_strEftBlendMode2 == "Sub")		{ m_effectBlendMode2 = ALPHA_SUB; }
			else if(m_strEftBlendMode2 == "Modulate")   { m_effectBlendMode2 = ALPHA_MODULATE; }
			else if(m_strEftBlendMode2 == "Modulate2X") { m_effectBlendMode2 = ALPHA_MODULATE2X; }

			DataChange(0,0);

			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			break;
	}
	return false;
}

bool CRT_MtlStandard::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			break;
	}
	return false;
}

const char *CRT_MtlStandard::TVGetName()
{
	m_tvName = "Material Standard";
	m_tvName += " (";
	m_tvName += m_name;
	m_tvName += " )";
	return m_tvName.c_str();
}

int CRT_MtlStandard::TVGetKeyNum()
{
	return m_keyVec.Size();
}

int CRT_MtlStandard::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyVec.GetKeyFrame(key,frame);
	return frame;
}

void CRT_MtlStandard::TVOnNotify(ETV_OP op,long param1,long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;

	SMtlStandardKey k;
	SMtlStandardKey *old;

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
			m_keyVec.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyVec.DelKeyFrame(param1);
            break;

		case OP_MOVE_KEY:
			if(param1 == param2) break;
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			m_keyVec.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			break;	
	}
}

// -------------------------------------------------------------------------------------------
// material blend
// -------------------------------------------------------------------------------------------
void CRT_MtlBlend::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;

	CRT_MtlBlend *s = this;
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������Ϣ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("����",&s->m_name));
	pSubItem->SetReadOnly(TRUE);

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ͬ������",&s->m_bSyncActorFrame));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("˫��",&s->m_fo.twoSide));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��Ӱ",&s->m_fo.shadow));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ZTest",&s->m_fo.ztest));

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ",&s->m_strAlphaMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Mask");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&s->m_defKey.ro.dif));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("������",&s->m_defKey.ro.amb));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemColor4("�Է���",&s->m_defKey.ro.emi));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemFloat("͸����",&s->m_defKey.ro.visible));
	pItem->Expand();

	// tex1
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼ1"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_tex1));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bTexMatrix1));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ����ģʽ",&s->m_strUv1));
	pItem->Expand();
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("Camera Pos");
	pList->AddConstraint("Camera Nor");
	pList->AddConstraint("Camera Rel");
	pSubItem->SetFlags(pgiHasComboButton);

	// tex2
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼ2"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_tex2));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bTexMatrix2));
	pItem->Expand();
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ����ģʽ",&s->m_strUv2));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("Camera Pos");
	pList->AddConstraint("Camera Nor");
	pList->AddConstraint("Camera Rel");
	pSubItem->SetFlags(pgiHasComboButton);

	// tex mix
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�����ͼ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texMix));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ��������",&s->m_bTexMatrixMix));
	pItem->Expand();
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ����ģʽ",&s->m_strUvMix));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Base");
	pList->AddConstraint("Camera Pos");
	pList->AddConstraint("Camera Nor");
	pList->AddConstraint("Camera Rel");
	pSubItem->SetFlags(pgiHasComboButton);

	// pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ʹ�û�ϱ���",&s->m_bUseMixAmount));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemFloat("��ϱ���",&s->m_defKey.mixAmount));

	/*
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�߹���Ϣ"));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("ģʽ",&s->m_strHlMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Vertex");
	pList->AddConstraint("Texture");
	pSubItem->SetFlags(pgiHasComboButton);

	pItem->AddChildItem(RT_NEW CCustomItemColor4("��ɫ",&s->m_defKey.spe));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("���",&s->m_defKey.spePower));
	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&s->m_texHl));
	pItem->Expand();
	*/
	pRoot->Expand();
}

void CRT_MtlBlend::AttachKeyGrid(int key,void *p)
{
	CRT_MtlBlend *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	SMtlBlendKey *k;
	CString tmp;

	k = m_keyVec.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("֡��",tmp));
	pItem->SetReadOnly(TRUE);

	pRoot->AddChildItem(RT_NEW CCustomItemFloat("��͸��",&k->ro.visible));
	pRoot->AddChildItem(RT_NEW CCustomItemColor4("������",&k->ro.dif));
	pRoot->AddChildItem(RT_NEW CCustomItemColor4("������",&k->ro.amb));
	pRoot->AddChildItem(RT_NEW CCustomItemColor4("�Է���",&k->ro.emi));
	pRoot->AddChildItem(RT_NEW CCustomItemColor4("�߹�",&k->ro.spe));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("�߹�ǿ��",&k->ro.spePower));

    pRoot->AddChildItem(RT_NEW CCustomItemFloat("��ϱ���",&k->mixAmount));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ1ƽ��",&k->stage[0].translate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ1��ת",&k->stage[0].rotate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ1����",&k->stage[0].scale));

	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ2ƽ��",&k->stage[1].translate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ2��ת",&k->stage[1].rotate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("��ͼ2����",&k->stage[1].scale));

	pRoot->AddChildItem(RT_NEW CCustomItemV3("�����ͼƽ��",&k->stage[2].translate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("�����ͼ��ת",&k->stage[2].rotate));
	pRoot->AddChildItem(RT_NEW CCustomItemV3("�����ͼ����",&k->stage[2].scale));

	pRoot->Expand();
}

bool CRT_MtlBlend::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			m_fo.alphaMode = ConvertAlphaMode(m_strAlphaMode);

			if(m_strUv1 == "Base")			{ m_uv1 = TEX_UV_BASE; }
			else if(m_strUv1 == "Camera Pos")	{ m_uv1 = TEX_UV_CPOS; }
			else if(m_strUv1 == "Camera Nor")	{ m_uv1 = TEX_UV_CNOR; }			
			else if(m_strUv1 == "Camera Rel")	{ m_uv1 = TEX_UV_CREL; }			
			
			if(m_strUv2 == "Base")			{ m_uv2 = TEX_UV_BASE; }
			else if(m_strUv2 == "Camera Pos")	{ m_uv2 = TEX_UV_CPOS; }
			else if(m_strUv2 == "Camera Nor")	{ m_uv2 = TEX_UV_CNOR; }			
			else if(m_strUv2 == "Camera Rel")	{ m_uv2 = TEX_UV_CREL; }			
			
			if(m_strUvMix == "Base")			{ m_uvMix = TEX_UV_BASE; }
			else if(m_strUvMix == "Camera Pos")	{ m_uvMix = TEX_UV_CPOS; }
			else if(m_strUvMix == "Camera Nor")	{ m_uvMix = TEX_UV_CNOR; }			
			else if(m_strUvMix == "Camera Rel")	{ m_uvMix = TEX_UV_CREL; }			
			
			DataChange(0,0);
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			break;
	}
	return false;
}

bool CRT_MtlBlend::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			break;
	}
	return false;
}

const char *CRT_MtlBlend::TVGetName()
{
	m_tvName = "Material Blend";
	m_tvName += " (";
	m_tvName += m_name;
	m_tvName += " )";
	return m_tvName.c_str();
}

int CRT_MtlBlend::TVGetKeyNum()
{
	return m_keyVec.Size();
}

int CRT_MtlBlend::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyVec.GetKeyFrame(key,frame);
	return frame;
}

void CRT_MtlBlend::TVOnNotify(ETV_OP op,long param1,long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;

	SMtlBlendKey k;
	SMtlBlendKey *old;

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
			m_keyVec.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyVec.DelKeyFrame(param1);
            break;

		case OP_MOVE_KEY:
			if(param1 == param2) break;
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			m_keyVec.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			break;	
	}
}

// -------------------------------------------------------------------------------------------
// actor
// -------------------------------------------------------------------------------------------
void CRT_Actor::AttachPoseGrid(void *p)
{
	// if(!m_poseMap) return;
	
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	CString tmp;
	TPoseMap::iterator it;
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Add Pose"));
	pItem->SetFlags(pgiHasEdit|pgiHasExpandButton);

	for(it=m_poseMap.begin(); it!=m_poseMap.end(); it++)
	{
		SRT_Pose *pose = &(*it).second;
	
		pItem = pRoot->AddChildItem(RT_NEW CCustomItemString("Pose",&pose->name));
		pItem->SetFlags(pgiHasExpandButton);
		pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("Start",pose->startFrm,&pose->startFrm));
		pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("End",pose->endFrm,&pose->endFrm));
		tmp.Format("%d",pose->eventVec.size());
		pItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Event",tmp));
		pItem->SetFlags(pgiHasExpandButton);
		pItem->Expand();
	}
	pRoot->Expand();
}

bool CRT_Actor::OnPoseGridNotify(WPARAM wParam, LPARAM lParam)
{
	// if(!m_poseMap) return false;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	CString tmp;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			break;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "Add Pose")
			{
				tmp = pItem->GetValue();
				if(tmp.GetLength() == 0) return false;
				SRT_Pose pose;
				pose.startFrm = 0;
				pose.endFrm = 0;
				pose.name = tmp;
				// check if pose exist
				if(FindPose(tmp)) return false;
				m_poseMap[pose.name] = pose;
				return true;	
			}
			else if(pItem->GetCaption() == "Pose")
			{
				tmp = pItem->GetValue();
				if(tmp.GetLength() == 0) return false;
				if(MessageBox(NULL,"ȷʵҪɾ����","��ʾ",MB_OKCANCEL)!=IDOK)
					return false;
				m_poseMap.erase((const char*)tmp);
				return true;
			}
			else if(pItem->GetCaption() == "Event")
			{
				if(!pItem->GetParentItem()) return false;
				SRT_Pose *pose = FindPose(pItem->GetParentItem()->GetValue());
                if(!pose) return false;
				CDlgPose dlg;
				dlg.m_pose = pose; 
				dlg.DoModal();
				CString tmp;
				tmp.Format("%d",pose->eventVec.size());
				pItem->SetValue(tmp);
				return false;
			}
			break;

		case XTP_PGN_SELECTION_CHANGED:
			// pItem->GetCaption() == "Pose";
			break;
	}
	return false;
}

string CRT_Actor::GetSelectedPose(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;
	if(pItem->GetCaption() == "Pose")
		return (const char*)pItem->GetValue();

	return "";
}

CRT_Effect *CRT_Actor::GetSelectedEffect(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;
	if(pItem->GetCaption() == "Effect Index")
	{
		// get effect idx
		int idx = atol(pItem->GetValue());
		if(idx < 0 || idx >= m_eftVec.size()) return NULL;
		return m_eftVec[idx];
	}
	return NULL;
}

// attach to actor info grid
void CRT_Actor::AttachGrid(void *p)
{	
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pItemSkin,*pItemList,*pSubItem;
	CXTPItemConstraints  *pList;

	int i,j;
	CString tmp;
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������Ϣ"));
	// pItem->SetReadOnly(TRUE);
	if(m_animType==ACTOR_ANIM_VA)
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������","���㶯��"));
	else if(m_animType==ACTOR_ANIM_STATIC)
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������","��̬"));
	else if(m_animType==ACTOR_ANIM_SKEL)
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������","��������"));
	else 
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������","����"));
	pSubItem->SetReadOnly(TRUE);

	SActorInfo info;
	GetInfo(info);
	tmp.Format("%d", info.verNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������",tmp));
	pSubItem->SetReadOnly(TRUE);

	tmp.Format("%d", info.triNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("����������",tmp));
	pSubItem->SetReadOnly(TRUE);

	tmp.Format("%d", info.boneNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������",tmp));
	pSubItem->SetReadOnly(TRUE);

	tmp.Format("%d", info.skinNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Ƥ������",tmp));
	pSubItem->SetReadOnly(TRUE);

	tmp.Format("%d", info.mtlNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��������",tmp));
	pSubItem->SetReadOnly(TRUE);

	tmp.Format("%d", info.eftNum);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("��Ч����",tmp));
	pSubItem->SetReadOnly(TRUE);
	pItem->Expand();
	
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��������"));
	// pItem->SetReadOnly(TRUE);
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("����",&m_strLightingMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Full");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->SetDescription("ѡ��ʹ�õĹ���ģ�͡�");
   
	// pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("Enable VC",&m_core->m_bUseVC));
	// pSubItem->SetDescription("�Ƿ�ʹ�ö���ɫ��");
	
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ZTest",&m_bZTest));
	pSubItem->SetDescription("�Ƿ�����Ȳ���(false=ʼ������ǰ)��");
	
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("��Ӱ",&m_strShadowMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Null");
	pList->AddConstraint("Static");
	pList->AddConstraint("Dynamic");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->SetDescription("ѡ��ʹ�õ���Ӱģ�͡�");
	pItem->Expand();
	
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemV3("����",&m_scale));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemV3("ƫ��",&m_offset));

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("����"));
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Import Skin"));
	pSubItem->SetDescription("�������ļ��е���һ��Skin��");
	pSubItem->SetFlags(pgiHasExpandButton);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Import Effect"));
	pSubItem->SetDescription("�������ļ��е���һ��Effect��");
	pSubItem->SetFlags(pgiHasExpandButton);
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("New Effect"));
	pSubItem->SetDescription("�½�һ��Effect");
	pSubItem->SetFlags(pgiHasExpandButton);
	pItem->Expand();

	for(i=0; i<m_skinVec.size(); i++)
	{
		CRT_Skin *skin;
		skin = m_skinVec[i];

		tmp.Format("%d",i);
		pItemSkin = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Skin Index",tmp));
		pItemSkin->SetFlags(pgiHasExpandButton);
		pItem = pItemSkin->AddChildItem(RT_NEW CCustomItemString("Skin Name",&skin->m_name));
		pItem->SetReadOnly(TRUE);
		// pItemSkin->AddChildItem(RT_NEW CCustomItemBool("��Ӱ",&skin->m_bDynShadow));

		pItem = pItemSkin->AddChildItem(RT_NEW CXTPPropertyGridItem("Change Mtl Lib"));
        pItem->SetFlags(pgiHasExpandButton);
		// pItem = pItemSkin->AddChildItem(RT_NEW CCustomItemBool("Board",&skin->m_core->m_bBoard));
        for(j=0; j<skin->m_rsVec.size(); j++)
		{
			// rs list
			SRT_RenderSurface *rs = &skin->m_rsVec[j];
			tmp.Format("%d",j);
			// pItem = pItemList->AddChildItem(RT_NEW CXTPPropertyGridItem("Render Surface Index",tmp));
			// pItem->SetReadOnly(TRUE);
			// pItem = pItemList->AddChildItem(RT_NEW CCustomItemString("Base Texture",&rs->Texture));
			pItem = pItemSkin->AddChildItem(RT_NEW CCustomItemString("Material",&rs->material));
            if(!skin->m_mtlLib->GetMtlByName(rs->material.c_str()))
            {
				pItem = pItemSkin->AddChildItem(RT_NEW CXTPPropertyGridItem("state","Not Found"));
				pItem->SetReadOnly(TRUE);
			}
			else 
            {
				pItem = pItemSkin->AddChildItem(RT_NEW CXTPPropertyGridItem("state","Active"));
				pItem->SetReadOnly(TRUE);
			}
		}
		// pItemSkin->Expand();
	}

	// effect list
	CXTPPropertyGridItem *pItemEffect;
	for(i=0; i<m_eftVec.size(); i++)
	{
		CRT_Effect *effect;
		effect = m_eftVec[i];

		tmp.Format("%d",i);
		pItemEffect = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Effect Index",tmp));
		pItemEffect->SetFlags(pgiHasExpandButton);
		tmp.Format("%s",effect->GetName().c_str());
		pItemEffect->AddChildItem(RT_NEW CXTPPropertyGridItem("Name",tmp));
	}
	pRoot->Expand();
}

bool CRT_Actor::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			if(m_strLightingMode == "Null")		  { m_lightingMode = LIGHTING_MODE_NULL; }
			else if(m_strLightingMode == "Full")  { m_lightingMode = LIGHTING_MODE_FULL; }
		
			if(m_strShadowMode == "Null")		  { m_shadowMode = SHADOW_MODE_NULL; }
			else if(m_strShadowMode == "Static")  { m_shadowMode = SHADOW_MODE_STATIC; }
			else if(m_strShadowMode == "Dynamic") { m_shadowMode = SHADOW_MODE_DYNAMIC; }
			
			if(pItem->GetCaption()=="Material")
			{
				DataChange(ON_ACTOR_MATERIAL_CHANGE,0);
			}
			else if(pItem->GetCaption() == "Name")
			{
				CRT_Effect *eft = m_eftVec[atol(pItem->GetParentItem()->GetValue())];
				std::string tmp = pItem->GetValue();
				if(eft) eft->SetName(tmp);
			}
			break;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "Import Skin")
			{
				char filter[] = "Skin Files (*.ski)|*.ski||";
				char bak[255];

				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();

				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;
				file = "creature\\actor\\" + file;
				CRT_Skin *skin = (CRT_Skin*)ACTOR()->LoadFromFile(file,ACTOR_RES_TYPE_SKIN);
				if(!skin) return false;
				m_skinVec.push_back(skin);
				DataChange(ON_ACTOR_SKIN_ADD, m_skinVec.size()-1);
				return true;
			}
			else if(pItem->GetCaption() == "Change Mtl Lib")
			{
				char filter[] = "MtlLib Files (*.mtl)|*.mtl||";
				char bak[255];

				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();

				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;
				file = "creature\\material\\" + file;
				// replace old lib
				// m_skinVec[atol(pItem->GetParentItem()->GetValue())]->ChangeMaterial(file);
				/*string fullname;
				fullname = file;
				fullname = fullname + ACTOR()->GetFileExtByType(ACTOR_RES_TYPE_MTLLIB);*/
				CRT_MaterialLib *lib = (CRT_MaterialLib*)ACTOR()->LoadFromFile(file,ACTOR_RES_TYPE_MTLLIB);
				if(lib)
				{
					ACTOR()->Unload(m_skinVec[atol(pItem->GetParentItem()->GetValue())]->m_mtlLib);
					m_skinVec[atol(pItem->GetParentItem()->GetValue())]->m_mtlLib = lib;
					DataChange(ON_ACTOR_MATERIAL_CHANGE,0);
				}
				return true;
			}
			else if(pItem->GetCaption() == "New Effect")
			{
				int num;
				num = m_eftVec.size();
				CDlgCreateEffect dlg;
				dlg.DoModal();
				if(m_eftVec.size() > num)
				{
					DataChange(ON_ACTOR_EFFECT_ADD, m_eftVec.size()-1);
				}
				return true;
			}
			else if(pItem->GetCaption() == "Import Effect")
			{
				int num;
				num = m_eftVec.size();
				char filter[] = "Effect Files (*.eft)|*.eft||";
				char bak[255];
				
				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();
				file = "creature\\effect\\" + file;
				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;
				CRT_Effect *eff = (CRT_Effect*)ACTOR()->LoadFromFile(file,ACTOR_RES_TYPE_EFFECT);
				if(!eff) return false;
				m_eftVec.push_back(eff);
				if(m_eftVec.size() > num)
				{
					DataChange(ON_ACTOR_EFFECT_ADD, m_eftVec.size()-1);
				}
				return true;
			}
			else if(pItem->GetCaption() == "Skin Index")
			{
				if(MessageBox(NULL,"ȷʵҪɾ����","��ʾ",MB_OKCANCEL)!=IDOK)
					return false;

				// del skin
				int skin = atol(pItem->GetValue());
				if(skin < 0 || skin >= m_skinVec.size()) return false;

				// delete it
				// vector<CRT_SkinInstance*>::iterator it1 = m_skinList.begin();
				vector<CRT_Skin*>::iterator it2 = m_skinVec.begin();
				for(int i=0;i!=skin;i++)
				{
					// it1++;
					it2++;
				}
				m_skinVec.erase(it2);
				DataChange(ON_ACTOR_SKIN_DEL, skin);
				return true;
			}
			else if(pItem->GetCaption() == "Effect Index")
			{
				if(MessageBox(NULL,"ȷʵҪɾ����","��ʾ",MB_OKCANCEL)!=IDOK)
					return false;

				// del effect
				int eft = atol(pItem->GetValue());
				if(eft < 0 || eft >= m_eftVec.size()) return false;

				// delete it
				vector<CRT_Effect*>::iterator it1 = m_eftVec.begin();
				for(int i=0;i!=eft;i++)
				{
					it1++;
				}
				ACTOR()->Unload(*it1);
				m_eftVec.erase(it1);
				DataChange(ON_ACTOR_EFFECT_DEL, eft);
				return true;
			}
			break;
	}
	
	return false;
}

CRT_Material *CRT_Actor::GetSelectedMaterial(void *p)
{
    return NULL;
}

CRT_Skin *CRT_Actor::GetSelectedSkin(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;

	if(pItem->GetCaption() == "Skin Index")
	{
		// get skin idx
		int skin = atol(pItem->GetValue());
		if(skin < 0 || skin >= m_skinVec.size()) return NULL;
		return m_skinVec[skin];
	}
	return NULL;
}

/*
bool CRT_Actor::Save(RtArchive *ar)
{

	bool ret = true;

	// only use in edit
	// update all material and effect
	vector<CRT_Skin*>   tmpSkinList = m_core->m_skinList;
	vector<CRT_Effect*> tmpEftList  = m_core->m_eftList;
	
	m_core->m_skinList.clear();
	vector<CRT_MaterialLib*> tmpMtlLib; 
	m_core->m_eftList = m_effectList;
	for(int i=0; i<m_skinList.size(); i++)
	{
		m_core->m_skinList.push_back(m_skinList[i]->m_core);
		tmpMtlLib.push_back(m_core->m_skinList[i]->m_mtlLib);
		m_core->m_skinList[i]->m_mtlLib = m_skinList[i]->m_mtlLib;
	}

	// save actor
	RtArchive *local;
	local = ar;
	if(!local)
		local = RtCoreFile().CreateFileWriter(NULL,m_core->m_diskFile.c_str());
	if(!local) return false;
	ret = local->WriteObject(m_core);
	if(!ar)
		delete local;

	for(int i=0; i<m_skinList.size(); i++)
	{
		m_core->m_skinList[i]->m_mtlLib = tmpMtlLib[i];
	}
	m_core->m_skinList = tmpSkinList;
	m_core->m_eftList  = tmpEftList;
	
	return ret;
}
*/

bool CRT_Actor::UpdateSkin(RtgDevice *dev)
{
	return true;
}

bool CRT_Actor::UpdateEffect(RtgDevice *dev)
{
	return true;
}

// -------------------------------------------------------------------------------------------
// effect emitter
// -------------------------------------------------------------------------------------------
void CRT_EffectEmitter::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;

	//
	// base prop,all particle have
	//
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��������"));
	pItem->SetReadOnly(TRUE);
	
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("����ģʽ",&m_strGenMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Normal");
	pList->AddConstraint("Squirt");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->SetDescription("�ڱ���ģʽ�£�ֻ�ڹؼ�֡�з������ӡ�");

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("·��ģʽ",&m_strPathMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Normal");
	pList->AddConstraint("Dest");
	pList->AddConstraint("Path");
	pSubItem->SetFlags(pgiHasComboButton);

	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("����ģʽ",&m_strGeometryMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("Board");
	pList->AddConstraint("Model");
	pList->AddConstraint("Model Clone");
	pSubItem->SetFlags(pgiHasComboButton);

	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��������(ms)",m_parLife,&m_parLife));
	pSubItem->SetDescription("���ӵ��������ڣ���λ�Ǻ��롣");
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemFloat("��������(ÿ��)",&m_emiSpeed));
	pSubItem->SetDescription("ÿ���ͷŶ��ٸ����ӡ�");
	pItem->AddChildItem(RT_NEW CCustomItemV3("�߳�(min)",&m_sizeMin));
	pItem->AddChildItem(RT_NEW CCustomItemV3("�߳�(max)",&m_sizeMax));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("���ĵ�",&m_strBoardCenter));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("X0Y0");
	pList->AddConstraint("X0Y1");
	pSubItem->SetFlags(pgiHasComboButton);

	pItem->AddChildItem(RT_NEW CCustomItemV3("������Χ(min)",&m_emiSizeMin));
	pItem->AddChildItem(RT_NEW CCustomItemV3("������Χ(max)",&m_emiSizeMax));

	pItem->AddChildItem(RT_NEW CCustomItemBool("ʼ������ǰ��",&m_bAlwaysFront));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Boardģʽ",&m_strBoardMode));
	pSubItem->SetDescription("�����泯�ķ���,Fullʼ������ͷ��");
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Full");
	pList->AddConstraint("X");
	pList->AddConstraint("Y");
	pList->AddConstraint("Z");
	pList->AddConstraint("SpeedX");
	pList->AddConstraint("SpeedY");
	pList->AddConstraint("SpeedZ");

	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->Expand();
	pItem->Expand();

	//
	// model info
	//
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("ģ����Ϣ"));
	pItem->SetReadOnly(TRUE);
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("ģ���ļ�",&m_model));
	pSubItem->SetDescription("ģ���ļ���,����.actβ׺��");
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("ģ�Ͷ���",&m_modelPose));
	pSubItem->SetDescription("ģ�Ͷ��������֡�");
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ѭ������",&m_bModelLoop));
	pSubItem->SetDescription("�Ƿ�ѭ������ģ�Ͷ�����");
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("�Զ�ɾ��",&m_autoDeleteModelPar));
	pSubItem->SetDescription("����������ɺ��Զ�ɾ��,���ʹ����ѭ����������Ч��");
	pItem->Expand();

	//
	// tex info
	//
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼ��Ϣ"));
	pItem->SetReadOnly(TRUE);
	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ",&m_texture));
	pItem->AddChildItem(RT_NEW CCustomItemBool("��ͼ����",&m_bTexSub));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("U Sub",m_texUSub,&m_texUSub));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("V Sub",m_texVSub,&m_texVSub));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("�����ͼ",&m_bTexRand));
	pSubItem->SetDescription("ÿ�����ѡȡһ����ͼ��");
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ",&m_strAlphaMode));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->Expand();
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("������Ϣ"));
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("�������"));
	pSubItem->SetFlags(pgiHasExpandButton);

	for(int i=0; i<m_forceVec.size(); i++)
	{
		CString tmp;
		tmp.Format("%d",i);
		CRT_ForceField *ff = m_forceVec[i];
		switch(ff->m_ffType)
		{
			case FORCE_FIELD_VORTEX:
				CRT_FFVortex *vox;
				vox = (CRT_FFVortex*)ff;
				pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Force Idx", tmp));
				pSubItem->SetFlags(pgiHasExpandButton);
				pItem->AddChildItem(RT_NEW CCustomItemFloat("�뾶", &vox->m_radius ));
				pItem->AddChildItem(RT_NEW CCustomItemFloat("�ٶ�", &vox->m_speed));
				pItem->AddChildItem(RT_NEW CCustomItemFloat("������", &vox->m_centripetal));
				break;
		}
	}
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ֵ��Ϣ"));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("�м�ֵ(0-1)",&m_midVal));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("͸����(��ʼ)",&m_opaqueBeg));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("͸����(�м�)",&m_opaqueMid));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("͸����(����)",&m_opaqueEnd));

	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ɫ(��ʼ)",&m_colorBeg));
	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ɫ(�м�)",&m_colorMid));
	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ɫ(����)",&m_colorEnd));

	pItem->AddChildItem(RT_NEW CCustomItemV3("����(��ʼ)",&m_scaleBeg));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����(�м�)",&m_scaleMid));
	pItem->AddChildItem(RT_NEW CCustomItemV3("����(����)",&m_scaleEnd));

	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת(��ʼ)",&m_rotateBeg));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת(�м�)",&m_rotateMid));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת(����)",&m_rotateEnd));

	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��ͼ(��ʼ)",m_texBeg,&m_texBeg));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��ͼ(�м�)",m_texMid,&m_texMid));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��ͼ(����)",m_texEnd,&m_texEnd));
	pItem->Expand();

	// prop base on type
	if(m_strPathMode == "Normal")
	{
		pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��������"));
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("���ڵ�",m_parent.c_str()));
		pSubItem->SetFlags(pgiHasExpandButton);

		pItem->AddChildItem(RT_NEW CCustomItemBool("ʹ�ø��ڵ�ķ���",&m_bUseParentDir));
		pSubItem = pItem->AddChildItem(RT_NEW CCustomItemBool("ʹ�ø��ڵ������ϵ",&m_bUseParentCoor));
		pSubItem->SetDescription("Ĭ��ʹ����������ϵ��");
		pItem->AddChildItem(RT_NEW CCustomItemV3("���ٶ�(min)",&m_accMin));
		pItem->AddChildItem(RT_NEW CCustomItemV3("���ٶ�(max)",&m_accMax));
		pItem->AddChildItem(RT_NEW CCustomItemV3("�ٶ�(min)",&m_speedMin));
		pItem->AddChildItem(RT_NEW CCustomItemV3("�ٶ�(max)",&m_speedMax));
		pItem->Expand();
	}
	else if(m_strPathMode == "Dest")
	{
		pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("Ŀ�������"));
		pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("Ŀ���",m_dest.c_str()));
		pSubItem->SetFlags(pgiHasExpandButton);
		pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����ʱ��(ms)",m_destTime,&m_destTime));
		pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("�ٶ����(%)",m_destSpeedRand,&m_destSpeedRand));
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
			if(m_strGenMode == "Normal") m_genMode = PARTICLE_GEN_MODE_NORMAL;
			else if(m_strGenMode == "Squirt") m_genMode = PARTICLE_GEN_MODE_SQUIRT;

			if(m_strGeometryMode == "Board") m_geometryMode = PARTICLE_GEOMETRY_BOARD;
			else if(m_strGeometryMode == "Model") m_geometryMode = PARTICLE_GEOMETRY_MODEL;
			else if(m_strGeometryMode == "Model Clone") m_geometryMode = PARTICLE_GEOMETRY_MODEL_CLONE;

			if(m_strPathMode == "Normal") m_pathMode = PARTICLE_PATH_MODE_NORMAL;
			else if(m_strPathMode == "Dest") m_pathMode = PARTICLE_PATH_MODE_DEST;
			else if(m_strPathMode == "Path") m_pathMode = PARTICLE_PATH_MODE_PATH;

			if(m_strAlphaMode == "None") m_alphaMode = ALPHA_NULL;
			else if(m_strAlphaMode == "Add") m_alphaMode = ALPHA_ADD;
			else if(m_strAlphaMode == "Sub") m_alphaMode = ALPHA_SUB;
			else if(m_strAlphaMode == "Modulate") m_alphaMode = ALPHA_MODULATE;
			else if(m_strAlphaMode == "Blend") m_alphaMode = ALPHA_BLEND;

			if(m_strBoardMode == "None") m_boardMode = PARTICLE_BILLBOARD_NONE;
			else if(m_strBoardMode == "Full") m_boardMode = PARTICLE_BILLBOARD_FULL;
			else if(m_strBoardMode == "X") m_boardMode = PARTICLE_BILLBOARD_X;
			else if(m_strBoardMode == "Y") m_boardMode = PARTICLE_BILLBOARD_Y;
			else if(m_strBoardMode == "Z") m_boardMode = PARTICLE_BILLBOARD_Z;
			else if(m_strBoardMode == "SpeedX") m_boardMode = PARTICLE_BILLBOARD_SPEEDX;
			else if(m_strBoardMode == "SpeedY") m_boardMode = PARTICLE_BILLBOARD_SPEEDY;
			else if(m_strBoardMode == "SpeedZ") m_boardMode = PARTICLE_BILLBOARD_SPEEDZ;

			if(m_strBoardCenter == "X0Y0") m_boardCenter = PARTICLE_BOARDCENTER_X0Y0;
			else if(m_strBoardCenter == "X0Y1") m_boardCenter = PARTICLE_BOARDCENTER_X0Y1;

			DataChange(0,0);
			if(pItem->GetCaption()=="��������" || pItem->GetCaption()=="·��ģʽ")
				return true;
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "���ڵ�") 
			{
				CDlgGetBone dlg;
				if(dlg.DoModal()) 
				{
                    m_parent = dlg.m_selectedBone;
					pItem->SetValue(m_parent.c_str());
				}
			}
			else if(pItem->GetCaption() == "Ŀ���")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_dest = dlg.m_selectedBone;
					pItem->SetValue(m_dest.c_str());
				}
			}
			else if(pItem->GetCaption() == "�������")
			{
				CDlgCreateForceField dlg;
				if(dlg.DoModal())
				{
					if(dlg.m_result)
					{
						m_forceVec.push_back(dlg.m_result);
						return true;
					}
				}
			}
			else if(pItem->GetCaption() == "Force Idx")
			{
				int idx;
				idx = atol(pItem->GetValue());
				if(idx<0 || idx>=m_forceVec.size())
					return false;
				
				if(MessageBox(NULL,"ȷʵҪɾ����","��ʾ",MB_OKCANCEL)!=IDOK)
					return false;

				int tmp = 0;
				vector<CRT_ForceField*>::iterator it;
				for(it=m_forceVec.begin(); it!=m_forceVec.end(); tmp++,it++)
				{
					if(tmp!=idx) continue;
					m_forceVec.erase(it);
					return true;
				}
			}
			DataChange(0,0);
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

	k = m_keyVec.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("֡��",tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(RT_NEW CCustomItemBool("��Ч",&k->bEnable));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("�����ٶ�(s)",&k->emiSpeed));

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

const char *CRT_EffectEmitter::TVGetName() 
{ 
	m_tvName = "EffectEmitter";
	m_tvName += " (";
	m_tvName += m_name;
	m_tvName += " )";
	return m_tvName.c_str();
}

int CRT_EffectEmitter::TVGetKeyNum()
{
	return m_keyVec.Size();
}

int CRT_EffectEmitter::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyVec.GetKeyFrame(key,frame);
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
			m_keyVec.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyVec.DelKeyFrame(param1);
			break;

		case OP_MOVE_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			m_keyVec.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			break;	
	}
}

// -------------------------------------------------------------------------------------------
// effect ribbon
// -------------------------------------------------------------------------------------------
void CRT_EffectRibbon::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�ڵ�(head)",m_head.c_str()));
	pItem->SetFlags(pgiHasExpandButton);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�ڵ�(tail)",m_tail.c_str()));
	pItem->SetFlags(pgiHasExpandButton);
	pRoot->AddChildItem(RT_NEW CCustomItemBool("ZTest",&m_bZTest));

	// pRoot->AddChildItem(RT_NEW CCustomItemFloat("��͸��",&m_visible));
	// pRoot->AddChildItem(RT_NEW CCustomItemColor3("��ɫ",&m_color));

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼ����"));
	pItem->AddChildItem(RT_NEW CCustomItemBool("�Ƿ�ʹ��",&m_bTexSub));
	pItem->AddChildItem(RT_NEW CCustomItemBool("�������",&m_bTexRand));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("U", m_texUSub, &m_texUSub));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("V", m_texVSub, &m_texVSub));
	pItem->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("�����ٶ�", m_texSpeed, &m_texSpeed));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼ��Ϣ"));
	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ1",&m_tex1));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ1",&m_strAlphaMode1));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->Expand();

	pItem->AddChildItem(RT_NEW CCustomItemString("��ͼ2",&m_tex2));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ2",&m_strAlphaMode2));
	pList = pSubItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pList->AddConstraint("Modulate2X");
	pSubItem->SetFlags(pgiHasComboButton);
	pSubItem->Expand();

	pItem->Expand();
	pRoot->Expand();
}

bool CRT_EffectRibbon::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			m_alphaMode1 = ConvertAlphaMode(m_strAlphaMode1);
			m_alphaMode2 = ConvertAlphaMode(m_strAlphaMode2);
			DataChange(0,0);
			return false;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "�ڵ�(head)")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_head = dlg.m_selectedBone;
					pItem->SetValue(m_head.c_str());
				}
			}
			else if(pItem->GetCaption() == "�ڵ�(tail)")
			{
				CDlgGetBone dlg;
				if(dlg.DoModal())
				{
					m_tail = dlg.m_selectedBone;
					pItem->SetValue(m_tail.c_str());
				}
			}
			DataChange(0,0);
			break;
	}
	return false;
}

void CRT_EffectRibbon::AttachKeyGrid(int key,void *p)
{
	CRT_EffectRibbon *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;
	CXTPItemConstraints  *pList;

	SRibbonKey *k;
	CString tmp;

	k = m_keyList.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("�ؼ�֡",tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����֡��",k->fadeOutFrame,&k->fadeOutFrame));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����֡��",k->endFrame,&k->endFrame));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����ʱ��",k->fadeIn,&k->fadeIn));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����ʱ��",k->life,&k->life));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("����ʱ��",k->fadeOut,&k->fadeOut));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("��͸��",&k->visible));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("��ɫ�ָ��",&k->mid));
	pRoot->AddChildItem(RT_NEW CCustomItemColor3("��ɫ��ʼ",&k->colorBeg));
	pRoot->AddChildItem(RT_NEW CCustomItemColor3("��ɫ�м�",&k->colorMid));
	pRoot->AddChildItem(RT_NEW CCustomItemColor3("��ɫ����",&k->colorEnd));

	/*pItem = pRoot->AddChildItem(RT_NEW CCustomItemString("Alphaģʽ",&m_strAlphaMode));
	pList = pItem->GetConstraints();
	pList->AddConstraint("None");
	pList->AddConstraint("Blend");
	pList->AddConstraint("Add");
	pList->AddConstraint("Sub");
	pList->AddConstraint("Modulate");
	pItem->SetFlags(pgiHasComboButton);
	pItem->Expand();*/

	pRoot->Expand();
}

bool CRT_EffectRibbon::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			break;

		case XTP_PGN_INPLACEBUTTONDOWN: break;
	}
	return false;
}

const char *CRT_EffectRibbon::TVGetName() 
{ 
	m_tvName = "EffectRibbon";
	m_tvName += " (";
	m_tvName += m_name;
	m_tvName += " )";
	return m_tvName.c_str();
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

// -------------------------------------------------------------------------------------------
// effect ribbon
// -------------------------------------------------------------------------------------------
void CRT_EffectBlur::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;

	// pItem = pRoot->AddChildItem(RT_NEW CCustomItemString("����",&s->m_name));
	// pItem->SetReadOnly(TRUE);
	// pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��Ӱ����",m_blurNum,&m_blurNum));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��Ӱ�������(ms)",m_blurInterval,&m_blurInterval));
	pRoot->AddChildItem(RT_NEW CXTPPropertyGridItemNumber("��Ӱ��������(ms)",m_life,&m_life));
	pRoot->AddChildItem(RT_NEW CCustomItemBool("ʼ����ǰ",&m_bAlwaysFront));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("��ʼ͸����",&m_visibleBeg));
	pRoot->AddChildItem(RT_NEW CCustomItemFloat("����͸����",&m_visibleEnd));
	pRoot->AddChildItem(RT_NEW CCustomItemString("͸��ģ��",&m_blurModel));

	pRoot->Expand();
}

bool CRT_EffectBlur::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			DataChange(0,0);
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

	k = m_keyVec.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("֡��",tmp));
	pItem->SetReadOnly(TRUE);
	pRoot->AddChildItem(RT_NEW CCustomItemBool("��Ч",&k->bEnable));
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
	return m_keyVec.Size();
}

int CRT_EffectBlur::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyVec.GetKeyFrame(key,frame);
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
			m_keyVec.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyVec.DelKeyFrame(param1);
			break;

		case OP_MOVE_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			m_keyVec.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			break;	
	}
}

// -------------------------------------------------------------------------------------------
// effect projector
// -------------------------------------------------------------------------------------------
void CRT_EffectProjector::AttachPropGrid(void *p)
{
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pSubItem;
	CXTPItemConstraints  *pList;

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͷѡ��"));
	pItem->SetReadOnly(TRUE);
	pItem->AddChildItem(RT_NEW CCustomItemFloat("Fov",&m_fov));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("Aspect",&m_aspect));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("Near",&m_near));
	pItem->AddChildItem(RT_NEW CCustomItemFloat("Far",&m_far));
	pItem->AddChildItem(RT_NEW CCustomItemV3("ƽ��",&m_offsetPos));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ת",&m_offsetRotate));
	pSubItem = pItem->AddChildItem(RT_NEW CXTPPropertyGridItem("���ڵ�",m_parent.c_str()));
	pSubItem->SetFlags(pgiHasExpandButton);
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("ͶӰѡ��"));
	pItem->SetReadOnly(TRUE);
	pItem->AddChildItem(RT_NEW CCustomItemBool("ͶӰ�ر�",&m_bClipTerrain));
	pItem->AddChildItem(RT_NEW CCustomItemBool("ͶӰ��̬����",&m_bClipStaticActor));
	pItem->AddChildItem(RT_NEW CCustomItemBool("ͶӰ��̬����",&m_bClipDynamicActor));
	pItem->AddChildItem(RT_NEW CCustomItemBool("ͶӰ�Լ�",&m_bProjectSelf));
	pItem->AddChildItem(RT_NEW CCustomItemBool("��ͶӰ����",&m_bCullBackFaces));
	pItem->AddChildItem(RT_NEW CCustomItemBool("��̬ͶӰ��",&m_bStaticProjector));
	pItem->Expand();

	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼѡ��"));
	pItem->SetReadOnly(TRUE);
	pItem->AddChildItem(RT_NEW CCustomItemString("ͶӰ��ͼ",&m_tex));
	pItem->AddChildItem(RT_NEW CCustomItemString("������ͼ",&m_texFadeOut));
	pItem->AddChildItem(RT_NEW CCustomItemBool("���ظ���ͼ",&m_bTexClampMode));
	pItem->Expand();

	::AttachPropGrid(m_fo, p);
	::AttachPropGrid(m_defKey.ro, p);
	pRoot->Expand();
}

bool CRT_EffectProjector::OnPropGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			::OnPropGridNotify(m_fo, wParam, lParam);
			DataChange(0,0);
			break;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "���ڵ�") 
			{
				CDlgGetBone dlg;
				if(dlg.DoModal()) 
				{
					m_parent = dlg.m_selectedBone;
					pItem->SetValue(m_parent.c_str());
				}
			}
			DataChange(0,0);
			break;
	}
	return false;
}

void CRT_EffectProjector::AttachKeyGrid(int key,void *p)
{
	CRT_EffectProjector *s = this;
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	SProjectorKey *k;
	CString tmp;

	k = m_keyVec.GetKeyFrame(key);
	if(!k) return;
	tmp.Format("%d",key);
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("֡��",tmp));
	pItem->SetReadOnly(TRUE);
	::AttachKeyGrid(k->ro, p);
	
	pItem = pRoot->AddChildItem(RT_NEW CXTPPropertyGridItem("��ͼѡ��"));
	pItem->SetReadOnly(TRUE);
	pItem->AddChildItem(RT_NEW CCustomItemColor3("��ɫ",&k->color));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ͼƽ��",&k->stage.translate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ͼ��ת",&k->stage.rotate));
	pItem->AddChildItem(RT_NEW CCustomItemV3("��ͼ����",&k->stage.scale));
	pItem->Expand();
	pRoot->Expand();
}

bool CRT_EffectProjector::OnKeyGridNotify(int key,WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED: break;
		case XTP_PGN_INPLACEBUTTONDOWN: break;
	}
	return false;
}

int CRT_EffectProjector::TVGetKeyNum()
{
	return m_keyVec.Size();
}

int CRT_EffectProjector::TVGetKeyFrame(int key)
{
	unsigned long frame = 0;
	m_keyVec.GetKeyFrame(key,frame);
	return frame;
}

void CRT_EffectProjector::TVOnNotify(ETV_OP op,long param1,long param2)
{
	CDlgChannelInfo prop;
	CDlgKeyInfo keyInfo;
	SProjectorKey k;
	SProjectorKey *old;

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
			m_keyVec.AddKeyFrame(param1,k);
			keyInfo.Init(param1,this);
			keyInfo.DoModal();
			break;

		case OP_DEL_KEY:
			m_keyVec.DelKeyFrame(param1);
			break;

		case OP_MOVE_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			m_keyVec.DelKeyFrame(param1);
			break;	

		case OP_COPY_KEY:
			old = m_keyVec.GetKeyFrame(param1);
			if(!old) break;
			m_keyVec.AddKeyFrame(param2,*old);
			break;	
	}
}
#endif


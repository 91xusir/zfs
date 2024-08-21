//
// MainFrm.cpp : CMainFrame 类的实现
//
#include "character/actor.h"
#include "stdafx.h"
#include "tool_character.h"
#include "EditorFrame.h"
#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "DlgLink.h"
#include "CustomItems.h"
#include "MainFrm.h"
// #define CFrameWnd CFrameWnd
constexpr auto PGI_EXPAND_BORDER = 10;

void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    // small hack.
    class CGridView : public CXTPPropertyGridView {
        friend class CCoolGridPage;
    };

    CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lpDrawItemStruct->itemData;
    COLORREF              clrWindow = GetSysColor(COLOR_WINDOW);
    ASSERT(pItem != NULL);
    CGridView*     pGrid = (CGridView*)pItem->GetGrid();
    CXTPWindowRect rcWindow(pGrid);
    CRect          rc = lpDrawItemStruct->rcItem;

    CXTPBufferDC dc(lpDrawItemStruct->hDC, rc);
    CFont        defaultFont;
    defaultFont.Attach((HFONT)GetStockObject(DEFAULT_GUI_FONT));
    CXTPFontDC font(&dc, &defaultFont);
    //CXTPFontDC font(&dc, XTPPaintManager()->GetRegularFont());

    dc.FillSolidRect(rc, clrWindow);
    dc.SetBkMode(TRANSPARENT);

    CRect    rcCaption(rc.left, rc.top, rc.left + pGrid->GetDividerPos(), rc.bottom);
    COLORREF clrLine = GetSysColor(COLOR_3DFACE);
    COLORREF clrFore = GetSysColor(COLOR_BTNTEXT);
    COLORREF clrShadow = GetSysColor(COLOR_3DSHADOW);

    if (pItem->IsCategory()) {
        dc.FillSolidRect(rc, clrLine);
        CXTPFontDC font(&dc, &defaultFont);
        //CXTPFontDC font(&dc, XTPPaintManager()->GetRegularBoldFont());

        if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
            (lpDrawItemStruct->itemState & ODS_FOCUS)) {
            dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc.FillSolidRect(CRect(PGI_EXPAND_BORDER, rc.top, rc.right, rc.bottom),
                             ::GetSysColor(COLOR_HIGHLIGHT));
        }

        CRect rcText(PGI_EXPAND_BORDER + 3, rc.top, rc.right, rc.bottom);
        dc.DrawText(pItem->GetCaption(), rcText, DT_SINGLELINE | DT_VCENTER);

        dc.FillSolidRect(PGI_EXPAND_BORDER - 1, rc.bottom - 1, rc.Width(), 1, clrShadow);
    } else {
        dc.SetTextColor(pItem->GetReadOnly() ? GetSysColor(COLOR_GRAYTEXT) : clrFore);

        CRect rcValue(rc);
        rcValue.left = rcCaption.right + 4;
        rcValue.bottom -= 1;

        BOOL bSelected = ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
                          (lpDrawItemStruct->itemState & ODS_SELECTED));
        //		BOOL bFocused = bSelected && GetFocus() && ((GetFocus() == pGrid) || (GetFocus()->GetParent() == pGrid) || (GetFocus()->GetOwner() == pGrid));
        BOOL bFocused = FALSE;
        if (bFocused) {
            dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc.FillSolidRect(rcCaption, ::GetSysColor(COLOR_HIGHLIGHT));
        } else if (bSelected) {
            dc.FillSolidRect(rcCaption, clrLine);
            // dc.FillSolidRect(rcValue, RGB(0xff,0x00,0x00));
        }
        dc.FillSolidRect(rc.left, rc.top, PGI_EXPAND_BORDER, rc.Height(), clrLine);

        CXTPPenDC pen(dc, clrLine);
        dc.MoveTo(0, rc.bottom - 1);
        dc.LineTo(rc.right, rc.bottom - 1);
        dc.MoveTo(rcCaption.right, rc.top);
        dc.LineTo(rcCaption.right, rc.bottom - 1);

        dc.FillSolidRect(PGI_EXPAND_BORDER - 1, rc.top, 1, rc.Width(), clrShadow);
        CRect rcText(rc);
        rcText.left = pItem->GetIndent() * PGI_EXPAND_BORDER + 3;
        rcText.right = rcCaption.right - 1;
        rcText.bottom -= 1;
        dc.DrawText(pItem->GetCaption(), rcText, DT_SINGLELINE | DT_VCENTER);

        if (!pItem->OnDrawItemValue(dc, rcValue)) {
            dc.DrawText(pItem->GetValue(), rcValue, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
        }
        if (bSelected) {
            CXTPPenDC pen1(dc, RGB(0xff, 0x00, 0x00));
            dc.MoveTo(rcCaption.right, rcValue.top);
            dc.LineTo(rcValue.right - 2, rcValue.top);
            dc.MoveTo(rcCaption.right, rcValue.bottom - 1);
            dc.LineTo(rcValue.right - 2, rcValue.bottom - 1);
            dc.MoveTo(rcCaption.right, rcValue.top);
            dc.LineTo(rcCaption.right, rcValue.bottom - 1);
            dc.MoveTo(rcValue.right - 2, rcValue.top);
            dc.LineTo(rcValue.right - 2, rcValue.bottom - 1);
        }
    }

    CXTPPropertyGridItem* pNext =
        (CXTPPropertyGridItem*)pGrid->GetItemDataPtr(lpDrawItemStruct->itemID + 1);
    if (((DWORD)pNext == (DWORD)(-1)) || (pNext && pNext->IsCategory()))
        dc.FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, clrShadow);

    if (pItem->HasChilds()) {
        CRgn rgn;
        rgn.CreateRectRgnIndirect(&rcCaption);
        dc.SelectClipRgn(&rgn);

        CRect rcSign(CPoint(PGI_EXPAND_BORDER / 2 - 5, rc.CenterPoint().y - 4), CSize(9, 9));
        if (pItem->GetIndent() > 1)
            rcSign.OffsetRect((pItem->GetIndent() - 1) * PGI_EXPAND_BORDER, 0);

        CPoint      pt = rcSign.CenterPoint();
        CXTPBrushDC brush(dc, pItem->IsCategory() ? clrLine : clrWindow);
        CXTPPenDC   pen(dc, clrFore);
        dc.Rectangle(rcSign);

        dc.MoveTo(pt.x - 2, pt.y);
        dc.LineTo(pt.x + 3, pt.y);

        if (!pItem->IsExpanded()) {
            dc.MoveTo(pt.x, pt.y - 2);
            dc.LineTo(pt.x, pt.y + 3);
        }
        dc.SelectClipRgn(NULL);
    }
}

string g_openFile;
bool   g_bRelatedMode = false;

#include "rt_build_count.h"

bool GetActorFile(string& name, string& path) {
    // TODO: Add your command handler code here
    char filter[] = "Actor Files (*.act)|*.act||";
    char bak[255];

    // save path
    GetCurrentDirectory(255, bak);
    CFileDialog dlg(TRUE, NULL, NULL, NULL, filter, NULL);
    if (dlg.DoModal() != IDOK)
        return false;
    CString file = dlg.GetFileName();

    // restore path
    SetCurrentDirectory(bak);
    if (file.GetLength() <= 0)
        return false;
    name = file;
    path = dlg.GetPathName();
    return true;
}

#define MAX_FILE_BUFFER 4096

bool GetMultiActorFile(vector<CString>& path) {
    path.clear();
    // TODO: Add your command handler code here
    char filter[] = "Actor Files (*.act)|*.act||";
    char bak[255];

    char tmp[MAX_FILE_BUFFER];
    memset(tmp, 0, MAX_FILE_BUFFER);
    // save path
    GetCurrentDirectory(255, bak);
    CFileDialog dlg(TRUE, NULL, NULL, NULL, filter, NULL);

    dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
    dlg.m_ofn.nMaxFile = MAX_FILE_BUFFER;
    dlg.m_ofn.lpstrFile = tmp;

    if (dlg.DoModal() != IDOK)
        return false;

    // restore path
    SetCurrentDirectory(bak);

    POSITION pos = dlg.GetStartPosition();
    if (pos == NULL)
        return false;
    while (pos) {
        CString tmp = dlg.GetNextPathName(pos);
        path.push_back(tmp);
    }
    return true;
}

vector<CToolActor*> g_actorList;
CToolActor*         g_activeActor = NULL;
int                 g_objectId = 0;

void SetActiveActor(CToolActor* ob) {
    g_activeActor = ob;
}

void SetActiveObject(int id) {
    for (int i = 0; i < g_actorList.size(); i++) {
        if (g_actorList[i]->GetId() == id)
            g_activeActor = g_actorList[i];
    }
}

void DelActiveActor() {
    if (!g_activeActor)
        return;
    vector<CToolActor*>::iterator it;
    for (it = g_actorList.begin(); it != g_actorList.end(); it++) {
        if ((*it)->GetId() == g_activeActor->GetId()) {
            delete *it;
            g_actorList.erase(it);
            break;
        }
    }
    g_activeActor = NULL;
    CMainFrame* mn;
    mn = (CMainFrame*)AfxGetMainWnd();
    mn->m_viewKey.SetHost(NULL);
}

CToolActor::CToolActor() {
    //m_weapon = NULL;
    m_pSelectedSkin = NULL;
}

CToolActor::~CToolActor() {
    // Modified by Wayne Wong 2010-11-09
    //   for (size_t i = 0; i < m_vSubActor.size(); ++i)
    //       delete m_vSubActor[i];
    //   m_vSubActor.clear();

    //if(m_inst)
    //	delete m_inst;
    //	//ACTOR()->DeleteInstance(m_inst);
    for (size_t i = 0; i < m_vSubActor.size(); ++i)
        Safe_ReleaseActor(m_vSubActor[i]);
    m_vSubActor.clear();

    if (m_inst)
        Safe_ReleaseActor(m_inst);
}

CToolActor* CreateToolActor() {
    CToolActor* ob = new CToolActor;
    ob->m_matrix.Unit();
    ob->SetId(g_objectId++);
    g_actorList.push_back(ob);
    return ob;
}

void CToolActor::ModifyWeapon() {
    CDlgLink dlg;
    if (!dlg.DoModal())
        return;

    //if(m_weapon)
    //{
    //	m_weapon->PlayPose(dlg.m_poseName.c_str(),true);
    //	m_weapon->LinkParent(m_inst,dlg.m_weaSlot.c_str());
    //       m_vSubActor.push_back(m_weapon);
    //}
}

void CToolActor::DelWeapon() {
    // Modified by Wayne Wong 2010-11-09
    for (size_t i = 0; i < m_vSubActor.size(); ++i)
        Safe_ReleaseActor(m_vSubActor[i]);
    m_vSubActor.clear();
    //if(m_weapon)
    //{
    //	delete m_weapon;
    //	//ACTOR()->DeleteInstance(m_weapon);
    //	m_weapon = NULL;
    //}
}

void CToolActor::OnPoseEnd(SRT_Pose* pose) {
    if (pose->Name == m_pose1)
        PlayPose(m_pose2);
}

void CToolActor::OnPoseBegin(SRT_Pose* pose) {}

void CToolActor::OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event) {
    if (event->Action == "spawn") {
        char buf[32];
        int  link;
        if (sscanf(event->Param.c_str(), "%s %d", buf, &link) != 2) {
            RtCoreLog().Info("PoseEvent: spawn effect param error\n");
            return;
        }

        // Modified by Wayne Wong 2010-11-09
        //CRT_ActorInstance *ef = LoadActorInstance(buf);/*
        //	(CRT_ActorInstance*)ACTOR()->CreateInstance(
        //				ACTOR_RES_TYPE_ACT,buf);*/
        CRT_ActorInstance* ef = RtcGetActorManager()->CreateActor(buf, true);

        if (!ef)
            return;
        // ef->m_bEnableShadow = false;
        // Modified by Wayne Wong 2010-11-09
        //ef->Create(RtGetRender());
        ef->Create();
        ef->PlayPose("effect");
        m_eftList.push_back(ef);

        // ef->GetMatrix()->Unit();
        if (link) {
            ef->LinkParent(m_inst);
        } else {
            // Modified by Wayne Wong 2010-11-09
            //(*ef->GetWorldMatrix()) = (*m_inst->GetWorldMatrix());
            ef->SetWorldMatrix(*m_inst->GetWorldMatrix());
        }
    }
}

void CToolActor::Render(RtgDevice* dev, RTGRenderMask mask) {
    if (g_bRenderShadow && mask == RTGRM_NOBLENDOBJ_ONLY)
        RenderShadow(dev);

    // *m_inst->GetWorldMatrix() = m_matrix;
    m_inst->Render(dev, mask);

    for (size_t i = 0; i < m_vSubActor.size(); ++i)
        m_vSubActor[i]->Render(dev, mask);
    //if(m_weapon)
    //	m_weapon->Render(dev,mask);

    // check effect
    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end();) {
        CRT_ActorInstance* ef = *it;
        // Modified by Wayne Wong 2010-11-09
        //if(!ef->GetCurrentPose())
        if (!ef->GetCurrentPose().IsVaild()) {
            //delete ef;
            Safe_ReleaseActor(ef);
            //ACTOR()->DeleteInstance(ef);
            it = m_eftList.erase(it);
            continue;
        }
        ef->Render(dev, mask);
        it++;
    }
}

// #define USE_PROJECTED_TEXTURE
RtgCamera* shadowCamera = NULL;

void CToolActor::RenderShadow(RtgDevice* dev) {

    //    /*
    //// 	if(m_data->m_shadowMode == SHADOW_MODE_NULL)
    ////         return;
    //	unsigned int i;
    //	if(m_data->m_bDynamicShadow)
    //	{
    //		// prepare render target
    //		if(shadowRenderTarget == 0)
    //		{
    //			shadowRenderTarget = dev->m_Texture.CreateTextureRenderTarget(256,256);
    //		}
    //		if(shadowRenderTarget == 0)
    //			return;
    //
    //		// camera
    //		if(shadowCamera == NULL)
    //			shadowCamera = new RtgCamera;
    //
    //		// change render target
    //		dev->SetRenderTarget2(shadowRenderTarget);
    //		dev->ClearRenderTarget(false,true,false,0x00ffffff);
    //
    //		RtgVertex3 eye,at;
    //		shadowCamera->SetProjParams(DegreeToRadian(12.5f), 4.0f/3.0f, 10.0f, 3000.0f);
    //
    //		RtgAABB *bound = m_inst->GetBoundingBox();
    //		shadowCamera->SetViewParams(RtgVertex3(m_inst->GetWorldMatrix()->_30+(bound->vPos.z+bound->vExt.z)*2,
    //			m_inst->GetWorldMatrix()->_31+(bound->vPos.z+bound->vExt.z)*2,
    //			m_inst->GetWorldMatrix()->_32+(bound->vPos.z+bound->vExt.z)*6),
    //			RtgVertex3(m_inst->GetWorldMatrix()->_30+0.1f,m_inst->GetWorldMatrix()->_31+0.0f,
    //			m_inst->GetWorldMatrix()->_32+bound->vPos.z),
    //			RtgVertex3(0.0f, 0.0f, 1.0f));
    //		shadowCamera->UpdateMatrix();
    //
    //		RtgCamera *src = dev->m_pCamera;
    //		dev->SetCamera(shadowCamera);
    //		dev->SetMatrixFromCamera();
    //
    //		RtgMatrix16 uv;
    //		uv = shadowCamera->GetViewMatrix();
    //		uv = uv * shadowCamera->GetProjMatrix();
    //
    //		RtgMatrix16 uv1;
    //		uv1.Unit();
    //		uv1._00 = 0.5;
    //		uv1._11 = -0.5;
    //		uv1._22 = 0.5;
    //		uv1._30 = 0.5;
    //		uv1._31 = 0.5;
    //		uv1._32 = 0.5;
    //		uv = uv * uv1;
    //
    //		// uv.Scale(0.5,0.5,0.5);
    //		// uv.Translate(0.5,0.5,0.5);
    //		uv = src->GetViewMatrix().Inverse() * uv;
    //
    //		//dev->SetTextureFilterMag(RTGTEXF_LINEAR);
    //		//dev->SetTextureFilterMin(RTGTEXF_LINEAR);
    //
    //		// prepare draw shadow
    //		RtgShader sha;
    //		sha.iNumStage = 1;
    //		RtgVertex3 color(0.0,0.0,0.0);
    //		sha.dwTextureFactor = RtgVectorToColor(color,1);
    //		sha.Shaders[0].SetTexture(0);
    //		sha.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    //		sha.Shaders[0].eColorArg1 = RTGTA_TFACTOR;
    //		sha.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
    //		dev->SetShader(&sha);
    //
    //		// dev->SetTextureFilterMag(RTGTEXF_POINT);
    //		// dev->SetTextureFilterMin(RTGTEXF_POINT);
    //		color.Set(0.5,0.5,0.5);
    //		sha.dwTextureFactor = RtgVectorToColor(color,1);
    //		dev->SetShader(&sha);
    //		dev->SetRenderState(RTGRS_Z_TEST,FALSE);
    //		dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
    //
    //		m_inst->RenderShadow(dev);
    //		for(CRT_ActorInstance *p=m_inst->m_son; p; p=p->m_next)
    //			p->RenderShadow(dev);
    //		dev->SetRenderState(RTGRS_Z_TEST,TRUE);
    //		dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
    //
    //		// dev->SetTextureFilterMag(RTGTEXF_POINT);
    //		// dev->SetTextureFilterMin(RTGTEXF_POINT);
    //
    //		// save pic
    //		// dev->SaveRenderTargetToFile("c:\\shadow.bmp");
    //
    //		// restore render target
    //		dev->SetCamera(src);
    //		dev->SetMatrixFromCamera();
    //
    //		// prepare draw shadow
    //		sha.iNumStage = 1;
    //		sha.bEnableBlendFunc = true;
    //		sha.bColorKey = false;
    //		sha.eBlendSrcFunc = RTGBLEND_ZERO;
    //		sha.eBlendDstFunc = RTGBLEND_SRCCOLOR;
    //
    //		dev->SetRenderState(RTGRS_LIGHTING,FALSE);
    //		sha.Shaders[0].SetTexture(shadowRenderTarget);
    //		sha.Shaders[0].eColorOp    = RTGTOP_SELECTARG1;
    //		sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE;
    //		sha.Shaders[0].eAlphaOp    = RTGTOP_DISABLE;
    //
    //		sha.Shaders[0].bUseTextureMatrix = true;
    //		sha.Shaders[0].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;
    //		sha.Shaders[0].mMatrix16 = uv;
    //		sha.Shaders[0].dwTexTransformFlag = RTGTTF_COUNT4|RTGTTF_PROJECTED;
    //
    //		dev->SetShader(&sha);
    //		dev->SetTextureAddress(RTGTADD_BORDER);
    //		//dev->SetTextureAddress(RTGTADD_CLAMP);
    //		dev->SetTextureBorderColor(0x00ffffff);
    //
    //		dev->RestoreRenderTarget();
    //
    //	//dev->ResetDefaultRenderTarget();
    //
    //		// draw
    //		DWORD sVB,sIB;
    //		BYTE *vb,*ib;
    //
    //		RtgVertex4 v4;
    //		RtgVertexVT v;
    //		RtgVertex3 v3;
    //		vector<RtgVertex3> verList;
    //		float seg;
    //		seg = m_inst->GetBoundingBox()->vExt.x;
    //		if(m_inst->GetBoundingBox()->vExt.y > seg)
    //			seg = m_inst->GetBoundingBox()->vExt.y;
    //		if(m_inst->GetBoundingBox()->vExt.z > seg)
    //			seg = m_inst->GetBoundingBox()->vExt.z;
    //
    //		seg *= 2;
    //
    //
    //		v3.Set(-seg,seg,0);	verList.push_back(v3);
    //		v3.Set(0,seg,0);	verList.push_back(v3);
    //		v3.Set(seg,seg,0);	verList.push_back(v3);
    //
    //		v3.Set(-seg,0,0);	verList.push_back(v3);
    //		v3.Set(0,0,0);		verList.push_back(v3);
    //		v3.Set(seg,0,0);	verList.push_back(v3);
    //
    //		v3.Set(-seg,-seg,0);verList.push_back(v3);
    //		v3.Set(0,-seg,0);	verList.push_back(v3);
    //		v3.Set(seg,-seg,0);	verList.push_back(v3);
    //
    //		for(i=0; i<verList.size(); i++)
    //		{
    //			verList[i].x += m_inst->GetWorldMatrix()->_30;
    //			verList[i].y += m_inst->GetWorldMatrix()->_31;
    //			verList[i].z += m_inst->GetWorldMatrix()->_32;
    //		}
    //
    //		// update vb
    //		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,9*sizeof(RtgVertexVT),
    //			FALSE,sizeof(RtgVertexVT));
    //		float tt=0;
    //		for(i=0; i<verList.size(); i++)
    //		{
    //			/*v4.Set(verList[i].x,verList[i].y,verList[i].z,1);
    //			v4 = v4 * uv;*/
    //			v.t.Set(1,1);
    //			// v.t.Set(v4.x/v4.w/2+0.5,1-(v4.y/v4.w/2+0.5));
    //			v.v.Set(verList[i].x,verList[i].y,verList[i].z);
    //			memcpy(vb,&v,sizeof(RtgVertexVT));
    //			vb += sizeof(RtgVertexVT);
    //		}
    //		dev->m_pVB->EndUpload(sVB);
    //		dev->SetVertexFormat(RTG_VT);
    //
    //		// upload ib
    //		ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, 24*sizeof(short),
    //			FALSE, sizeof(short));
    //
    //		short idx[24] = {0,3,4, 0,4,1, 1,4,5, 1,5,2, 3,6,7, 3,7,4, 4,7,8, 4,8,5};
    //		memcpy(ib,idx,24*sizeof(short));
    //		dev->m_pIB->EndUpload(sIB);
    //
    //		RtgMatrix16 m1;
    //		m1.Unit();
    //		dev->SetMatrix(RTGTS_WORLD,&m1);
    //
    //		// draw
    //		dev->m_pVB->Bind(0, 0);
    //		dev->m_pIB->Bind(0, 0);
    //		dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,9,0,8);
    //
    //		dev->SetTextureAddress(RTGTADD_WRAP);
    //		dev->SetRenderState(RTGRS_LIGHTING,TRUE);
    //	}
    //	else/* if(m_data->m_shadowMode == SHADOW_MODE_STATIC)*/
    //	{
    //		if(shadowFloat == 0)
    //		{
    //			shadowFloat = dev->m_Texture.CreateTexture("shadow",RTGTBT_AUTO,true,1);
    //		}
    //		if(shadowFloat == 0)
    //			return;
    //
    //		if(shadowCamera == NULL)
    //			shadowCamera = new RtgCamera;
    //
    //		RtgVertex3 eye,at;
    //		shadowCamera->SetProjParams(DegreeToRadian(12.5f), 4.0f/3.0f, 10.0f, 3000.0f);
    //
    //		RtgAABB *bound = m_inst->GetBoundingBox();
    //		shadowCamera->SetViewParams(RtgVertex3(m_inst->GetWorldMatrix()->_30/*+(bound->vPos.z+bound->vExt.z)*2*/,
    //			m_inst->GetWorldMatrix()->_31/*+(bound->vPos.z+bound->vExt.z)*2*/,
    //			m_inst->GetWorldMatrix()->_32+(bound->vPos.z+bound->vExt.z)*6),
    //			RtgVertex3(m_inst->GetWorldMatrix()->_30+0.1f,m_inst->GetWorldMatrix()->_31+0.0f,
    //			m_inst->GetWorldMatrix()->_32+bound->vPos.z),
    //			RtgVertex3(0.0f, 0.0f, 1.0f));
    //		shadowCamera->UpdateMatrix();
    //
    //		RtgCamera *src = dev->m_pCamera;
    //		dev->SetCamera(shadowCamera);
    //		dev->SetMatrixFromCamera();
    //
    //		RtgMatrix16 uv;
    //		uv = shadowCamera->GetViewMatrix();
    //		uv = uv * shadowCamera->GetProjMatrix();
    //
    //		RtgMatrix16 uv1;
    //		uv1.Unit();
    //		uv1._00 = 0.5;
    //		uv1._11 = -0.5;
    //		uv1._22 = 0.5;
    //		uv1._30 = 0.5;
    //		uv1._31 = 0.5;
    //		uv1._32 = 0.5;
    //		uv = uv * uv1;
    //
    //		uv = src->GetViewMatrix().Inverse() * uv;
    //
    //
    //		dev->SetCamera(src);
    //		dev->SetMatrixFromCamera();
    //
    //		// prepare draw shadow
    //		RtgShader sha;
    //		sha.iNumStage = 1;
    //		sha.bEnableBlendFunc = true;
    //		sha.bColorKey = false;
    //		sha.eBlendSrcFunc = RTGBLEND_ZERO;
    //		sha.eBlendDstFunc = RTGBLEND_SRCCOLOR;
    //
    //		dev->SetRenderState(RTGRS_LIGHTING,FALSE);
    //		sha.Shaders[0].SetTexture(shadowFloat);
    //		sha.Shaders[0].eColorOp    = RTGTOP_SELECTARG1;
    //		sha.Shaders[0].eColorArg1  = RTGTA_TEXTURE;
    //		sha.Shaders[0].eAlphaOp    = RTGTOP_DISABLE;
    //
    //		sha.Shaders[0].bUseTextureMatrix = true;
    //		sha.Shaders[0].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;
    //		sha.Shaders[0].mMatrix16 = uv;
    //		sha.Shaders[0].dwTexTransformFlag = RTGTTF_COUNT4|RTGTTF_PROJECTED;
    //
    //
    //		dev->SetShader(&sha);
    //		dev->SetTextureAddress(RTGTADD_BORDER);
    //		//dev->SetTextureAddress(RTGTADD_CLAMP);
    //		dev->SetTextureBorderColor(0x00ffffff);
    //
    //		DWORD sVB,sIB;
    //		BYTE *vb,*ib;
    //
    //		RtgVertex4 v4;
    //		RtgVertexVT v;
    //		RtgVertex3 v3;
    //		vector<RtgVertex3> verList;
    //		float seg;
    //		seg = m_inst->GetBoundingBox()->vExt.x;
    //		if(m_inst->GetBoundingBox()->vExt.y > seg)
    //			seg = m_inst->GetBoundingBox()->vExt.y;
    //		if(m_inst->GetBoundingBox()->vExt.z > seg)
    //			seg = m_inst->GetBoundingBox()->vExt.z;
    //
    //		seg *= 2;
    //
    //
    //		v3.Set(-seg,seg,0);	verList.push_back(v3);
    //		v3.Set(0,seg,0);	verList.push_back(v3);
    //		v3.Set(seg,seg,0);	verList.push_back(v3);
    //
    //		v3.Set(-seg,0,0);	verList.push_back(v3);
    //		v3.Set(0,0,0);		verList.push_back(v3);
    //		v3.Set(seg,0,0);	verList.push_back(v3);
    //
    //		v3.Set(-seg,-seg,0);verList.push_back(v3);
    //		v3.Set(0,-seg,0);	verList.push_back(v3);
    //		v3.Set(seg,-seg,0);	verList.push_back(v3);
    //
    //		for(i=0; i<verList.size(); i++)
    //		{
    //			verList[i].x += m_inst->GetWorldMatrix()->_30;
    //			verList[i].y += m_inst->GetWorldMatrix()->_31;
    //			verList[i].z += m_inst->GetWorldMatrix()->_32;
    //		}
    //
    //		// update vb
    //		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,9*sizeof(RtgVertexVT),
    //			FALSE,sizeof(RtgVertexVT));
    //		float tt=0;
    //		for(i=0; i<verList.size(); i++)
    //		{
    //			/*v4.Set(verList[i].x,verList[i].y,verList[i].z,1);
    //			v4 = v4 * uv;*/
    //			v.t.Set(1,1);
    //			// v.t.Set(v4.x/v4.w/2+0.5,1-(v4.y/v4.w/2+0.5));
    //			v.v.Set(verList[i].x,verList[i].y,verList[i].z);
    //			memcpy(vb,&v,sizeof(RtgVertexVT));
    //			vb += sizeof(RtgVertexVT);
    //		}
    //		dev->m_pVB->EndUpload(sVB);
    //		dev->SetVertexFormat(RTG_VT);
    //
    //		// upload ib
    //		ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, 24*sizeof(short),
    //			FALSE, sizeof(short));
    //
    //		short idx[24] = {0,3,4, 0,4,1, 1,4,5, 1,5,2, 3,6,7, 3,7,4, 4,7,8, 4,8,5};
    //		memcpy(ib,idx,24*sizeof(short));
    //		dev->m_pIB->EndUpload(sIB);
    //
    //		RtgMatrix16 m1;
    //		m1.Unit();
    //		dev->SetMatrix(RTGTS_WORLD,&m1);
    //
    //		// draw
    //		dev->m_pVB->Bind(0, 0);
    //		dev->m_pIB->Bind(0, 0);
    //		dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,9,0,8);
    //
    //		dev->SetTextureAddress(RTGTADD_WRAP);
    //		dev->SetRenderState(RTGRS_LIGHTING,TRUE);
    //
    //	}
    //
}

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
ON_WM_CREATE()
ON_WM_TIMER()
ON_WM_PAINT()
ON_WM_CLOSE()
// ON_COMMAND(ID_VIEW_TREE_VIEW, OnViewTreeView)
// ON_COMMAND(ID_VIEW_PROPERTY_VIEW, OnViewPropertyView)
// ON_COMMAND(ID_VIEW_SLIDER_VIEW, OnViewSliderView)
ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
ON_NOTIFY(TVN_SELCHANGED, ID_TREE_OBJ_VIEW, OnTreeCtrlSelectChanged)
ON_XTP_CREATECONTROL()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR,  // 状态行指示器
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构
CMainFrame::CMainFrame() {
    // TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame() {}

void CMainFrame::OnPaint() {
    if (RtGetRender())
        RtGetRender()->RenderScene();
    CWnd::OnPaint();
}

CXTPControlComboBox* CMainFrame::GetPoseList() {
    for (int i = 0; i < m_toolBar->GetControls()->GetCount(); i++) {
        CXTPControl* pControl = m_toolBar->GetControl(i);
        if (pControl->GetID() == IDC_POSE_LIST)
            return (CXTPControlComboBox*)pControl;
    }
    return NULL;
}

void CMainFrame::RebuildPoseList() {
    if (!g_activeActor)
        return;

    m_pPoseList = GetPoseList();

    if (!m_pPoseList)
        return;

    if (!g_activeActor->m_data)
        return;

    set<string> set_pose;
    m_pPoseList->ResetContent();
    TPoseMap::iterator it;
    for (it = g_activeActor->m_data->m_poseMap.begin();
         it != g_activeActor->m_data->m_poseMap.end(); it++) {
        set_pose.insert(it->first);
    }

    for (set<string>::iterator i = set_pose.begin(); i != set_pose.end(); ++i)
        m_pPoseList->AddString(i->c_str());
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
        TRACE0("未能创建状态栏\n");
        return -1;  // 未能创建
    }

    if (!InitCommandBars())
        return -1;

    SetTimer(0, 500, NULL);
    SetTimer(1, 100, NULL);
    CXTPPaintManager::SetTheme(xtpThemeOffice2000);
    CXTPCommandBars* pCommandBars = GetCommandBars();
    pCommandBars->SetMenu(_T("菜单栏"), IDR_MAINFRAME);
    CXTPToolBar* pStandardBar = (CXTPToolBar*)pCommandBars->Add(_T("标准"), xtpBarTop);
    if (!pStandardBar || !pStandardBar->LoadToolBar(IDR_MAINFRAME)) {
        TRACE0("未能创建工具栏\n");
        return -1;  // 未能创建
    }
    m_toolBar = pStandardBar;
    XTPImageManager()->SetMaskColor(RGB(255, 0, 255));
    XTPImageManager()->SetIcons(IDR_TOOLBAR_TOOLS);

    //
    // docking pane
    //
    m_paneManager.InstallDockingPanes(this);
    m_paneManager.SetTheme(xtpPaneThemeGrippered);
    int nIDIcons[] = {ID_VIEW_TREE_VIEW, ID_VIEW_PROPERTY_VIEW, ID_VIEW_SLIDER_VIEW, 4, 5, 6, 7};
    m_paneManager.SetIcons(IDR_TOOLBAR_TOOLS, nIDIcons, sizeof(nIDIcons) / sizeof(nIDIcons[0]),
                           RGB(255, 0, 255));
    XTPPaintManager()->RefreshMetrics();

    // grid
    CXTPDockingPane* paneEnv =
        m_paneManager.CreatePane(ID_GRID_ENV, CRect(0, 0, 200, 120), xtpPaneDockRight);
    CXTPDockingPane* paneActor =
        m_paneManager.CreatePane(ID_GRID_ACTOR, CRect(0, 0, 200, 120), xtpPaneDockRight);
    CXTPDockingPane* paneMtlLib =
        m_paneManager.CreatePane(ID_GRID_MTLLIB, CRect(0, 0, 200, 120), xtpPaneDockRight);
    CXTPDockingPane* panePose =
        m_paneManager.CreatePane(ID_GRID_POSE, CRect(0, 0, 200, 120), xtpPaneDockRight);
    CXTPDockingPane* paneLinkBox =
        m_paneManager.CreatePane(ID_GRID_LINKBOX, CRect(0, 0, 200, 120), xtpPaneDockRight);

    paneEnv->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    paneActor->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    paneMtlLib->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    panePose->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    paneLinkBox->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    // 显示当前选中物体的动画信息
    // CXTPDockingPane* paneSlider = m_paneManager.CreatePane(ID_VIEW_SLIDER_VIEW, CRect(0, 0, 200, 160), dockBottomOf);
    // view
    CXTPDockingPane* paneKey =
        m_paneManager.CreatePane(ID_VIEW_KEY, CRect(0, 0, 200, 160), xtpPaneDockBottom);
    paneKey->SetOptions(xtpPaneNoCloseable | xtpPaneNoHideable);
    paneKey->Attach(CreateViewKey());
    // paneSlider->Attach(CreateViewSlider());

    CXTPDockingPane* paneProp =
        m_paneManager.CreatePane(ID_GRID_CHANNEL_PROP, CRect(0, 0, 240, 120), xtpPaneDockRight);
    paneProp->SetOptions(xtpPaneNoHideable);

    m_paneManager.AttachPane(paneActor, paneEnv);
    m_paneManager.AttachPane(paneMtlLib, paneActor);
    m_paneManager.AttachPane(panePose, paneActor);
    // 把关联框放到这里
    m_paneManager.AttachPane(paneLinkBox, paneEnv);
    // m_paneManager.DockPane(paneProp,xtpPaneDockBottom,paneEnv);
    m_paneManager.ClosePane(paneProp);

    // m_paneManager.AttachPane(paneSlider, paneKey);
    m_paneManager.ShowPane(paneEnv);
    // m_paneManager.ShowPane(paneActor);
    m_paneManager.ShowPane(paneKey);

    m_selectedMtlLib = NULL;
    m_selectedChannel = 0;
    return 0;
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl) {
    if (lpCreateControl->controlType == xtpControlButton && lpCreateControl->nID == IDC_POSE_LIST) {
        CXTPControlComboBox* m_pPoseList =
            (CXTPControlComboBox*)CXTPControlComboBox::CreateObject();
        m_pPoseList->SetDropDownListStyle(FALSE);
        m_pPoseList->SetWidth(200);
        // m_pPoseList->SetCaption(_T("动作"));
        m_pPoseList->SetStyle(xtpComboLabel);

        // m_pPoseList->SetDropDownWidth(100);

        lpCreateControl->pControl = m_pPoseList;
        return TRUE;
    }
    return FALSE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;
    // TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
    // 样式

    return TRUE;
}

void CMainFrame::OnClose() {
    if (MessageBox("退出", "确认", MB_OKCANCEL) == IDCANCEL)
        return;
    CFrameWnd::OnClose();
}

BOOL CMainFrame::DestroyWindow() {
    // Exit Core
    RtcGetActorManager()->Exit();
    return CFrameWnd::DestroyWindow();
}

bool s_bPostProcessEffect = false;

//
// create all gird
//
CWnd* CMainFrame::CreateGridEnv() {
    if (m_gridEnv.GetSafeHwnd() == 0) {
        m_gridEnv.Create(CRect(0, 0, 0, 0), this, 0);
        m_gridEnv.ModifyStyle(0, XTP_PGS_OWNERDRAW);

        // m_gridEnv.ShowToolBar(TRUE);
        m_gridEnv.SetOwner(this);

        CFont font;
        font.CreatePointFont(60, _T("Marlett"));

        LOGFONT lf;
        font.GetLogFont(&lf);

        CXTPPropertyGridItem*            pRoot;
        CXTPPropertyGridItem*            pItem;
        CXTPPropertyGridItemConstraints* pList;
        {
            m_gridEnv.ResetContent();
            CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;

            // 环境信息
            pRoot = m_gridEnv.AddCategory(_T("环境信息"));
            pItem =
                pRoot->AddChildItem(new CCustomItemColor3(_T("背景颜色"), &pEditor->m_clearColor));
            pItem->SetDescription(_T("设置窗口的背景颜色."));
            pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染阴影"), &g_bRenderShadow));
            // pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染边框"), &g_bRenderFrame));
            pItem = pRoot->AddChildItem(
                new CCustomItemBool(_T("渲染碰撞盒"), &pEditor->m_bShowBoundBox));
            pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染坐标系"), &g_bRenderCoor));
            pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染地板"), &g_bRenderFloor));
            pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染信息"), &g_bRenderInfo));
            /*
						pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染帮助物体"), &ACTOR()->m_bRenderHelpSkin));
						pItem = pRoot->AddChildItem(new CCustomItemBool(_T("渲染投影机"), &ACTOR()->m_bRenderProjector));
			*/
            pRoot->Expand();

            pRoot = m_gridEnv.AddCategory(_T("雾"));
            pItem = pRoot->AddChildItem(new CCustomItemBool(_T("使用"), &pEditor->m_bFog));
            pItem->SetDescription(_T("是否打开雾."));
            pItem =
                pRoot->AddChildItem(new CCustomItemColor3(_T("雾的颜色"), &pEditor->m_fogColor));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemNumber(
                _T("最近距离"), pEditor->m_fogNear, &pEditor->m_fogNear));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemNumber(
                _T("最远距离"), pEditor->m_fogFar, &pEditor->m_fogFar));
            pRoot->Expand();

            pRoot = m_gridEnv.AddCategory(_T("灯光"));
            pItem = pRoot->AddChildItem(new CCustomItemColor3(_T("环境光"), &pEditor->m_emi));
            pItem = pRoot->AddChildItem(new CCustomItemColor3(_T("平行光"), &pEditor->m_dirLight));
            pItem =
                pRoot->AddChildItem(new CCustomItemBool(_T("镜头灯光"), &pEditor->m_bCameraLight));

            // pItem = pRoot->AddChildItem(new CCustomItemV3(_T("模型缩放"), &pEditor->m_actorScale));
            pRoot->Expand();

            // 图形质量
            pRoot = m_gridEnv.AddCategory(_T("图形质量"));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(_T("贴图质量"), _T("高")));
            pList = pItem->GetConstraints();
            pList->AddConstraint(_T("高"));
            pList->AddConstraint(_T("中"));
            pList->AddConstraint(_T("低"));
            pItem->SetFlags(xtpGridItemHasComboButton | xtpGridItemHasEdit);
            pRoot->Expand();

            /*
			pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(_T("几何质量"), _T("高")));
			pList = pItem->GetConstraints();
			pList->AddConstraint(_T("高"));
			pList->AddConstraint(_T("中"));
			pList->AddConstraint(_T("低"));
			pItem->SetFlags(xtpGridItemHasComboButton | xtpGridItemHasEdit);
			pItem = pRoot->AddChildItem(new CCustomItemBool(_T("后期处理"), &s_bPostProcessEffect));
			*/
        }
        pRoot->Expand();
    }
    return &m_gridEnv;
}

CWnd* CMainFrame::CreateGridActor(bool force) {
    if (m_gridActor.GetSafeHwnd() == 0) {
        m_gridActor.Create(CRect(0, 0, 0, 0), this, 0);
        m_gridActor.SetOwner(this);
        CFont font;
        font.CreatePointFont(60, _T("Marlett"));
        LOGFONT lf;
        font.GetLogFont(&lf);
    }

    if (force) {
        m_gridActor.ResetContent();
        if (!g_activeActor)
            return &m_gridActor;
        CXTPPropertyGridItem* pRoot;
        pRoot = m_gridActor.AddCategory(ID_GRID_ACTOR);
        g_activeActor->AttachGrid(pRoot);
    }
    return &m_gridActor;
}

CWnd* CMainFrame::CreateGridMtlLib(bool force) {
    if (m_gridMtlLib.GetSafeHwnd() == 0) {
        m_gridMtlLib.Create(CRect(0, 0, 0, 0), this, 0);
        // m_gridMtlLib.ShowToolBar(TRUE);
        m_gridMtlLib.SetOwner(this);

        CFont font;
        font.CreatePointFont(60, _T("Marlett"));
        LOGFONT lf;
        font.GetLogFont(&lf);
    }

    CRT_MaterialLib* lib = GetSelectedMtlLib();

    if (force || lib != m_selectedMtlLib) {
        m_selectedMtlLib = lib;
        m_gridMtlLib.ResetContent();
        CXTPPropertyGridItem* pRoot;
        pRoot = m_gridMtlLib.AddCategory(ID_GRID_MTLLIB);
        if (!lib)
            return &m_gridMtlLib;
        lib->AttachGrid(pRoot);
    }
    return &m_gridMtlLib;
}

CWnd* CMainFrame::CreateGridPose(bool force) {
    if (m_gridPose.GetSafeHwnd() == 0) {
        m_gridPose.Create(CRect(0, 0, 0, 0), this, 0);
        // m_gridPose.ShowToolBar(TRUE);
        m_gridPose.SetOwner(this);

        CFont font;
        font.CreatePointFont(60, _T("Marlett"));
        LOGFONT lf;
        font.GetLogFont(&lf);
    }

    if (force) {
        m_gridPose.ResetContent();
        if (!g_activeActor)
            return &m_gridPose;

        CXTPPropertyGridItem* pRoot;
        pRoot = m_gridPose.AddCategory(ID_GRID_POSE);
        g_activeActor->AttachPoseGrid(pRoot);
    }
    return &m_gridPose;
}

CWnd* CMainFrame::CreateGridLinkBox(bool force) {
    if (m_gridLinkBox.GetSafeHwnd() == 0) {
        m_gridLinkBox.Create(CRect(0, 0, 0, 0), this, 0);
        // m_gridPose.ShowToolBar(TRUE);
        m_gridLinkBox.SetOwner(this);

        CFont font;
        font.CreatePointFont(60, _T("Marlett"));
        LOGFONT lf;
        font.GetLogFont(&lf);
    }

    if (force) {
        m_gridLinkBox.ResetContent();
        if (!g_activeActor)
            return &m_gridLinkBox;

        CXTPPropertyGridItem* pRoot;
        pRoot = m_gridLinkBox.AddCategory(ID_GRID_LINKBOX);
        g_activeActor->AttachLinkBoxGrid(pRoot);
    }
    return &m_gridLinkBox;
}

CWnd* CMainFrame::CreateGridProp(bool force) {
    if (m_gridProp.GetSafeHwnd() == 0) {
        m_gridProp.Create(CRect(0, 0, 0, 0), this, 0);
        // m_gridMtlLib.ShowToolBar(TRUE);
        m_gridProp.SetOwner(this);

        CFont font;
        font.CreatePointFont(60, _T("Marlett"));
        LOGFONT lf;
        font.GetLogFont(&lf);
    }

    if (force) {
        CXTPPropertyGridItem* pRoot;
        m_gridProp.ResetContent();
        pRoot = m_gridProp.AddCategory(ID_GRID_CHANNEL_PROP);
        if (GetSelectedEffect()) {
            GetSelectedEffect()->AttachPropGrid(pRoot);
            m_selectedChannel = (DWORD)GetSelectedEffect();
        } else if (GetSelectedMtl()) {
            CXTPPropertyGridItem*            pItem;
            CXTPPropertyGridItemConstraints* pList;
            CRT_Material*                    pMat = GetSelectedMtl();
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(
                "Material type:", pMat->GetRuntimeClass()->m_lpszClassName));
            pList = pItem->GetConstraints();
            pList->AddConstraint("CRT_MtlStandard");
            pList->AddConstraint("CRT_MtlMu");
            pList->AddConstraint("CRT_MtlBlend");
            pItem->SetFlags(xtpGridItemHasComboButton);
            GetSelectedMtl()->AttachPropGrid(pRoot);
            m_selectedChannel = (DWORD)GetSelectedMtl();
        }
    }
    return &m_gridProp;
}

//
// create all view
//
CWnd* CMainFrame::CreateViewKey() {
    if (m_viewKey.GetSafeHwnd() == 0)
        m_viewKey.Create(NULL, "ViewKey", WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), this, 0);
    return &m_viewKey;
}

CWnd* CMainFrame::CreateViewSlider() {
    /*
	if(m_viewSlider.GetSafeHwnd() == 0)
	{
		if(!m_viewSlider.Create(CDlgSliderView::IDD, this))
			return 0;
	}
	return &m_viewSlider;
	*/
    return NULL;
}

// CMainFrame 诊断
#ifdef _DEBUG
void CMainFrame::AssertValid() const {
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
    CFrameWnd::Dump(dc);
}
#endif  //_DEBUG

// CMainFrame 消息处理程序
LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam) {
    if (wParam == XTP_DPN_SHOWWINDOW) {
        CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
        switch (pPane->GetID()) {
                // grid
            case ID_GRID_ACTOR:
                pPane->Attach(CreateGridActor(false));
                break;

            case ID_GRID_ENV:
                pPane->Attach(CreateGridEnv());
                break;

            case ID_GRID_POSE:
                pPane->Attach(CreateGridPose(false));
                break;

            case ID_GRID_LINKBOX:
                pPane->Attach(CreateGridLinkBox(false));
                break;

            case ID_GRID_MTLLIB:
                pPane->Attach(CreateGridMtlLib(false));
                break;

            case ID_GRID_CHANNEL_PROP:
                pPane->Attach(CreateGridProp(false));
                break;

                // view
            case ID_VIEW_KEY:
                pPane->Attach(CreateViewKey());
                break;

            case ID_VIEW_SLIDER_VIEW:
                // pPane->Attach(CreateViewSlider());
                break;
        }
        return 1;
    }
    return 0;
}

#define C_COLOR(x) ((GetRValue(x) << 16) | (GetGValue(x) << 8) | GetBValue(x))

LRESULT CMainFrame::OnGridNotify(WPARAM wParam, LPARAM lParam) {

    ASSERT(this != nullptr);
    if (!this) {
        TRACE("OnGridNotify called with NULL this pointer.\n");
        return 0;
    }

    CString               tmp;
    CEditorFrame*         pEditor = (CEditorFrame*)g_pAppCase;
    CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

    if (wParam == XTP_PGN_DRAWITEM) {
        DrawItem((LPDRAWITEMSTRUCT)lParam);
        return TRUE;
    }

    if (wParam == XTP_PGN_ITEMVALUE_CHANGED) {
        if (pItem->GetCaption() == "贴图质量") {
            /*if(pItem->GetValue() == "中")
				RtGetRender()->m_Texture.SetTextureQuality(2);
			else if(pItem->GetValue() == "低")
				RtGetRender()->m_Texture.SetTextureQuality(4);
			else
				RtGetRender()->m_Texture.SetTextureQuality(0);*/
        } else if (pItem->GetCaption() == "后期处理") {
            // RtGetRender()->UsePostProcessEffect(s_bPostProcessEffect);
        } else if (pItem->GetCaption() == "Material type:") {
            //TODO: Change material type here.
            CRT_Material* pMat = GetSelectedMtl();
            if (!pMat)
                return 0;
            string strMatName = pMat->GetName();
            RtCoreLog().Info("Change material %s type from %s to %s\n", pMat->GetName().c_str(),
                             pMat->GetRuntimeClass()->m_lpszClassName,
                             pItem->GetValue().GetBuffer());
            //string strParentValue = pItem->GetParentItem()->GetValue().GetBuffer();
            CXTPPropertyGridItem* pActorItem = m_gridActor.GetSelectedItem();
            CRT_SkinInstance*     pSkin = GetSelectedSkinInstance();
            //g_activeActor->m_inst->GetSelectedSkin(pActorItem->GetParentItem());
            if (!pSkin)
                return 0;
            // Modified by Wayne Wong 2010-11-09
            //CRT_MaterialLib* pMatLib = pSkin->m_mtlLib;
            CRT_MaterialLib* pMatLib = pSkin->GetMaterialLib();
            if (!pMatLib)
                return 0;
            if (pMatLib->ConvertMaterial(strMatName, pItem->GetValue().GetBuffer())) {
                g_activeActor->m_inst->GetSelectedSkin(pActorItem->GetParentItem())
                    ->m_mtlLib->ConvertMaterial(strMatName, pItem->GetValue().GetBuffer());
                m_selectedChannel = (DWORD)pMatLib->GetMaterial(strMatName.c_str());
                pSkin->OnMaterialChange();
                m_viewKey.SetHost(pMatLib->GetMaterial(strMatName.c_str()));
                m_bRebuildGridProp = true;
                //pMatLib->m_skin->OnMaterialChange();
            }
            return 0;
        }
    }

    CXTPPropertyGridItem* find = pItem;

    while (find) {
        if (find->GetID() > 32850)
            return 0;
        CXTPPropertyGridItem* parent = find->GetParentItem();
        if (!parent)
            break;
        find = parent;
    }

    switch (find->GetID()) {
        case ID_GRID_MTLLIB:
            if (GetSelectedMtlLib())
                m_bRebuildGridMtlLib = GetSelectedMtlLib()->OnGridNotify(wParam, lParam);
            break;

        case ID_GRID_ACTOR:
            if (!g_activeActor)
                break;
            if (g_activeActor->OnGridNotify(wParam, lParam)) {
                m_bRebuildGridActor = true;
                m_bRebuildGridPose = true;
                m_bRebuildGridLinkBox = true;
            }
            break;

        case ID_GRID_POSE:
            if (!g_activeActor)
                break;
            if (g_activeActor->OnPoseGridNotify(wParam, lParam))
                m_bRebuildGridPose = true;
            break;

        case ID_GRID_LINKBOX:
            if (!g_activeActor)
                break;
            if (g_activeActor->OnLinkBoxGridNotify(wParam, lParam))
                m_bRebuildGridLinkBox = true;
            break;

        case ID_GRID_CHANNEL_PROP:
            if (m_selectedChannel == 0)
                break;
            RtObject* ob = (RtObject*)m_selectedChannel;
            if (ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material))) {
                CRT_Material* mtl = (CRT_Material*)ob;
                if (mtl->OnPropGridNotify(wParam, lParam)) {
                    m_bRebuildGridProp = true;
                }
            } else if (ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect))) {
                CRT_Effect* eft = (CRT_Effect*)ob;
                if (eft->OnPropGridNotify(wParam, lParam)) {
                    m_bRebuildGridProp = true;
                }
            }
            break;

            /*
		case ID_GRID_EFFECT:
			if(GetSelectedEffect())
				m_bRebuildGridEft = GetSelectedEffect()->OnGridNotify(wParam,lParam));
			break;
		*/
    }
    return 0;
}

void CMainFrame::OnTreeCtrlSelectChanged(NMHDR* pNotifyStruct, LRESULT* result) {}

/*
void CMainFrame::OnViewTreeView()
{
	m_paneManager.ShowPane(ID_VIEW_TREE_VIEW);
}

void CMainFrame::OnViewPropertyView()
{
	m_paneManager.ShowPane(ID_VIEW_PROPERTY_VIEW);
}

void CMainFrame::OnViewSliderView()
{
	m_paneManager.ShowPane(ID_VIEW_SLIDER_VIEW);
}
*/

#define VER "2.00"

void CMainFrame::OnTimer(UINT nIDEvent) {
    if (nIDEvent == 0) {
        char title[256], render[32], build[32];
#ifdef USE_D3D8
        strcpy(render, "DX8");
#elif USE_D3D9
        strcpy(render, "DX9");
#else
        strcpy(render, "Unknown");
#endif
        if (g_iBuildCount > 1000)
            sprintf(build, "%d", g_iBuildCount);
        else if (g_iBuildCount > 100)
            sprintf(build, "0%d", g_iBuildCount);
        else if (g_iBuildCount > 10)
            sprintf(build, "00%d", g_iBuildCount);
        else
            sprintf(build, "000%d", g_iBuildCount);

        if (g_bRelatedMode)
            sprintf(title, "ZFS Actor Editor [ver.%s][build.%s][%s] %s [关联模式]", VER, build,
                    render, g_openFile.c_str());
        else
            sprintf(title, "ZFS Actor Editor [ver.%s][build.%s][%s] %s", VER, build, render,
                    g_openFile.c_str());
        SetWindowText(title);
    } else if (nIDEvent == 1) {
        if (m_bRebuildGridActor) {
            CreateGridActor(true);
            m_bRebuildGridActor = false;
        }
        if (m_bRebuildGridMtlLib) {
            if (g_activeActor)
                g_activeActor->m_inst->OnMaterialChange();
            CreateGridMtlLib(true);
            m_bRebuildGridMtlLib = false;
        }
        if (m_bRebuildGridPose) {
            CreateGridPose(true);
            m_bRebuildGridPose = false;
            RebuildPoseList();
        }
        if (m_bRebuildGridLinkBox) {
            CreateGridLinkBox(true);
            m_bRebuildGridLinkBox = false;
            //RebuildLinkBoxList();
        }

        if (GetSelectedEffect() &&
            (m_bRebuildGridProp || (DWORD)GetSelectedEffect() != m_selectedChannel)) {
            CXTPDockingPane* paneProp;
            paneProp = m_paneManager.FindPane(ID_GRID_CHANNEL_PROP);
            // if(!paneProp)
            //	paneProp = m_paneManager.CreatePane(ID_GRID_CHANNEL_PROP, CRect(0, 0,200, 120), dockLeftOf);
            paneProp->Attach(CreateGridProp(true));
            m_paneManager.ShowPane(paneProp);
            m_paneManager.RedrawPanes();
            m_bRebuildGridProp = false;
        } else if (GetSelectedMtl() &&
                   (m_bRebuildGridProp || (DWORD)GetSelectedMtl() != m_selectedChannel)) {
            CXTPDockingPane* paneProp;
            paneProp = m_paneManager.FindPane(ID_GRID_CHANNEL_PROP);
            // if(!paneProp)
            //	paneProp = m_paneManager.CreatePane(ID_GRID_CHANNEL_PROP, CRect(0, 0,200, 120), dockLeftOf);
            paneProp->Attach(CreateGridProp(true));
            m_paneManager.ShowPane(paneProp);
            m_paneManager.RedrawPanes();
            m_bRebuildGridProp = false;
        } else if (!GetSelectedEffect() && !GetSelectedMtl()) {
            m_selectedChannel = 0;
            m_gridProp.ResetContent();
            // m_gridProp.SetViewBackColor(RGB(128,128,128));
            // m_paneManager.ClosePane(ID_GRID_CHANNEL_PROP);
        }

        if (GetSelectedEffect()) {
            m_viewKey.SetHost(GetSelectedEffect());
        } else {
            m_viewKey.SetHost(GetSelectedMtl());
        }

        string pose = GetGridSelectedPose();
        if (pose.length() > 0 && pose != m_oldSelectedPose) {
            m_pPoseList = GetPoseList();
            m_pPoseList->SetEditText(pose.c_str());
            m_oldSelectedPose = pose;
        }
    }
}

//
// get select object
//
CRT_Material* CMainFrame::GetSelectedMtl() {
    if (!g_activeActor)
        return NULL;
    CXTPPropertyGridItem* pItem = m_gridActor.GetSelectedItem();
    if (!pItem)
        return NULL;

    if (pItem->GetCaption() == "Material") {
        // careful,keep parent deep with ui
        CString skin = pItem->GetParentItem()->GetValue();
        int     idx = atol(skin);
        if (idx < 0 || idx >= g_activeActor->m_inst->m_skinList.size())
            return NULL;
        return g_activeActor->m_inst->m_skinList[idx]->GetMaterialLib()->GetMaterial(
            pItem->GetValue());
    }

    CXTPDockingPane* pane = m_paneManager.FindPane(ID_GRID_MTLLIB);
    if (pane && pane->IsSelected()) {
        CXTPPropertyGridItem* pItem = m_gridMtlLib.GetSelectedItem();
        if (!pItem)
            return NULL;
        CRT_MaterialLib* lib = GetSelectedMtlLib();
        if (!lib)
            return NULL;
        return lib->GetMaterial(pItem->GetValue());
    }
    return NULL;
}

CRT_MaterialLib* CMainFrame::GetSelectedMtlLib() {
    CRT_Skin* skin = GetSelectedSkin();
    if (!skin)
        return NULL;
    return skin->m_mtlLib;
}

CRT_Effect* CMainFrame::GetSelectedEffect() {
    if (!g_activeActor)
        return NULL;

    CXTPPropertyGridItem* pItem = m_gridActor.GetSelectedItem();
    if (!pItem)
        return NULL;
    return g_activeActor->GetSelectedEffect(pItem);
}

CRT_Skin* CMainFrame::GetSelectedSkin() {
    if (!g_activeActor)
        return NULL;

    CXTPPropertyGridItem* pItem = m_gridActor.GetSelectedItem();
    if (!pItem)
        return NULL;

    return g_activeActor->m_inst->GetSelectedSkin(pItem);
}

CRT_SkinInstance* CMainFrame::GetSelectedSkinInstance() {
    if (!g_activeActor)
        return NULL;

    CXTPPropertyGridItem* pItem = m_gridActor.GetSelectedItem();

    if (!pItem || pItem->GetCaption() != "Skin Index")
        pItem = pItem->GetParentItem();

    if (!pItem || pItem->GetCaption() != "Skin Index")
        return NULL;

    int index = atol(pItem->GetValue());
    if (index >= g_activeActor->m_inst->m_skinList.size())
        return NULL;

    return g_activeActor->m_inst->m_skinList[index];
}

string CMainFrame::GetGridSelectedPose() {
    if (!g_activeActor)
        return "";

    CXTPPropertyGridItem* pItem = m_gridPose.GetSelectedItem();
    if (!pItem)
        return "";

    return g_activeActor->GetSelectedPose(pItem);
}

string CMainFrame::GetListSelectedPose() {
    m_pPoseList = GetPoseList();
    return (const char*)m_pPoseList->GetText();
}

void CMainFrame::SaveAsActor() {
    if (!g_activeActor)
        return;

    // C [12/24/2008 Louis.Huang]
    //	CRT_Actor *cha = g_activeActor->m_data;

    // TODO: Add your command handler code here
    char filter[] = "Actor Files (*.act)|*.act||";
    char bak[255];

    // save path
    GetCurrentDirectory(255, bak);
    CFileDialog dlg(FALSE, NULL, NULL, NULL, filter, NULL);
    if (dlg.DoModal() != IDOK) {
        // restore path
        SetCurrentDirectory(bak);
        return;
    }
    CString file = dlg.GetPathName();
    if (file[file.GetLength() - 1] != 't' || file[file.GetLength() - 2] != 'c' ||
        file[file.GetLength() - 3] != 'a' || file[file.GetLength() - 4] != '.') {
        file += ".act";
    }

    // restore path
    SetCurrentDirectory(bak);
    if (file.GetLength() <= 0)
        return;

    RtArchive* ar = RtCoreFile().CreateFileWriter(NULL, file);
    if (!ar)
        return;
    //	ar->WriteObject(cha);
    if (g_activeActor->m_inst->Save(ar))
        MessageBox("保存成功！");
    else
        MessageBox("保存失败！");
    ar->Close();
    delete ar;
}

//void CMainFrame::AutoSave()
//{
//	if(!g_activeActor) return;
//
//	CRT_Actor *cha = g_activeActor->m_data;
//
//	char bak[255];
//
//	// save path
//	GetCurrentDirectory(255,bak);
//
//    CString file = bak;
//
//	file += "\creature\autoback\autoback.act";
//
//	RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,file);
//	if(!ar) return;
//	ar->WriteObject(cha);
//	ar->Close();
//	delete ar;
//
//}

void CMainFrame::OnDestroy() {
    CFrameWnd::OnDestroy();
    /*
	m_wndPropertyShader.ResetContent();
	m_wndPropertyShaderKey.ResetContent();
	m_wndPropertyView.ResetContent();
	*/
}

void CMainFrame::SaveAsSkin() {
    if (!g_activeActor)
        return;

    // C [12/24/2008 Louis.Huang]
    /*
CRT_Skin *skin = GetSelectedSkin();
	if(!skin) return;
*/

    // TODO: Add your command handler code here
    char filter[] = "Skin Files (*.ski)|*.ski||";
    char bak[255];

    // save path
    GetCurrentDirectory(255, bak);
    CFileDialog dlg(FALSE, NULL, NULL, NULL, filter, NULL);
    if (dlg.DoModal() != IDOK) {
        // restore path
        SetCurrentDirectory(bak);
        return;
    }
    CString file = dlg.GetPathName();
    if (file[file.GetLength() - 1] != 'i' || file[file.GetLength() - 2] != 'k' ||
        file[file.GetLength() - 3] != 's' || file[file.GetLength() - 4] != '.') {
        file += ".ski";
    }
    // restore path
    SetCurrentDirectory(bak);
    if (file.GetLength() <= 0)
        return;

    RtArchive* ar = RtCoreFile().CreateFileWriter(NULL, file);
    if (!ar)
        return;

    CRT_SkinInstance* pSkin = GetSelectedSkinInstance();
    if (!pSkin)
        return;

    if (pSkin->Save(ar))
        MessageBox("保存成功！");
    else
        MessageBox("保存失败！");
    //ar->WriteObject(skin);
    ar->Close();
    delete ar;
}

void CMainFrame::CreateAllGrid() {
    CreateGridEnv();
    CreateGridActor(true);
    CreateGridMtlLib(true);
    CreateGridPose(true);
    CreateGridLinkBox(true);
}


#include "stdafx.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"

RtgShader       m_AreaShader;

//BOOL RtScene::m_bRenderObjectBoundbox = FALSE;

RtSceneTool::RtSceneTool()
{
//    guard;
    m_bRender = TRUE;

    m_bDrawWireframe = FALSE;
    m_bDrawGridBox = FALSE;
    m_bDrawNormal = FALSE;
    m_bDrawAttrWalk = FALSE;
    m_bDrawAttrFly = FALSE;
    m_bDrawAttrDig = FALSE;
#if RTS_DRAW_HELPER
    m_bDrawHelper = TRUE;
#endif
    m_bDrawMinimap = FALSE;

    m_bAreaTool = FALSE;
	m_bPathPointTool = FALSE;//tim.yang  是否是路点工具
    m_pSelectedArea = NULL;
    m_fScale = 1.f;
    m_fViewPosX = 0.f;
    m_fViewPosY = 0.f;
    m_bIsPaint = FALSE;
    m_iHittestOrder = 0;

    m_iCurrentBlockX = -1;
    m_iCurrentBlockY = -1;

    m_bUpdateViewAreas = FALSE;

    m_AreaShader.bEnableBlendFunc = true;
    m_AreaShader.iNumStage = 0;
    m_fPervX = -12345.f;
    m_fPervY = -12345.f;

	m_pBlockMap = NULL;

  //  unguard;
}

RtSceneTool::~RtSceneTool()
{
}

bool RtSceneTool::ToolRender(RtgCamera& inCamera, RtgDevice& inDevice)
{

   // guard;
    if (!m_bRender) return true;

    bool bResult = false;

    if (!m_bAreaTool)
    {
        RtGetRender()->m_dwClearColor = 0x3030A030;
        // 渲染场景
        if (m_bDrawWireframe && !m_bRenderMiniMapMode)
        {
            inDevice.SetRenderState(RTGRS_FILL_MODE, RTG_FILLWIREFRAME);
        }

        bResult = this->Render(inCamera, RTGRM_NOBLENDOBJ_ONLY);

		if(RtGetRender()->IsUsePostProcessObjectHighLight()&& RtGetRender()->IsUsePostProcessObjectHighLightOn())
		{
            static RtgTextItem* m_texPostProcessRenderTarget = 0;
			static RtgTextItem* m_texPostProcessRenderBlur   = 0;
			static DWORD gw = 0,gh = 0;

            if(gw != RtGetRender()->m_iWndWidth || gh != RtGetRender()->m_iWndHeight)
            {
                RtGetRender()->GetTextMgr()->ReleaseTexture(m_texPostProcessRenderTarget);
                m_texPostProcessRenderTarget = RtGetRender()->GetTextMgr()->CreateFrameBufferTexture(
                    RtGetRender()->m_iWndWidth, RtGetRender()->m_iWndHeight);
				//RtGetRender()->m_Texture.DeleteTexture(m_texPostProcessRenderTarget);
			 //   m_texPostProcessRenderTarget = RtGetRender()->m_Texture.CreateTextureRenderTarget( RtGetRender()->m_iWndWidth
				//	, RtGetRender()->m_iWndHeight, true);
				gw = RtGetRender()->m_iWndWidth;
				gh = RtGetRender()->m_iWndHeight;
            }
			if (m_texPostProcessRenderBlur==0)
			{
				m_texPostProcessRenderBlur = 
                    RtGetRender()->GetTextMgr()->CreateFrameBufferTexture(256, 256);
                    
                    //RtGetRender()->m_Texture.CreateTextureRenderTarget(256,256,true);
			}

            RtGetRender()->SetRenderTarget(m_texPostProcessRenderTarget);
            RtGetRender()->ClearRenderTarget(false, true, false);
			
			RtGetRender()->SetTextureFilterMag(RTGTEXF_LINEAR);
			RtGetRender()->SetTextureFilterMin(RTGTEXF_LINEAR);

            RtGetRender()->SetRenderState(RTGRS_Z_WRITE,FALSE);
            this->RenderSceneObjectHighLight(inCamera);
			RtGetRender()->SetRenderState(RTGRS_Z_WRITE,TRUE);
			// g_pDevice->SaveRenderTargetToFile("c:\\c1.bmp");
			RtGetRender()->RestoreRenderTarget();
	
            // render to 256 tex
			RtgNVertexVCT p[4];
			DWORD dwColor = 0xFFFFFFFF;
			RtgShader shader;
			shader.bTwoSide = false;
			shader.iNumStage = 1;
			RtGetRender()->SetRenderTarget(m_texPostProcessRenderBlur);
			p[0] = RtgNVertexVCT( 0,  0,    dwColor, 0, 0);
			p[1] = RtgNVertexVCT( 0,  255,  dwColor, 0, 1);
			p[2] = RtgNVertexVCT( 255, 255, dwColor, 1, 1);
			p[3] = RtgNVertexVCT( 255, 0,   dwColor, 1, 0);

			shader.Shaders[0].SetTexture(m_texPostProcessRenderTarget);
			shader.bEnableBlendFunc = false;
		   	shader.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
			shader.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
			shader.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
			shader.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;

			BOOL bBakFog = RtGetRender()->GetFogEnable();
			RtGetRender()->SetFogEnable(FALSE);
			RtGetRender()->SetShader(&shader);

			RtGetRender()->SetVertexFormat(RTG_TRANSFORMED_VCT);
			RtGetRender()->SetRenderState(RTGRS_Z_TEST,FALSE);
			RtGetRender()->SetRenderState(RTGRS_Z_WRITE,FALSE);
			RtGetRender()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVCT), 2);
			RtGetRender()->SetRenderState(RTGRS_Z_TEST,TRUE);
			
			// render 256 blur tex to backgroud
			RtGetRender()->RestoreRenderTarget();
			RtGetRender()->Blur(m_texPostProcessRenderBlur);
           
            p[0] = RtgNVertexVCT( 0,  0,    dwColor, 0, 0);
            p[1] = RtgNVertexVCT( 0,  RtGetRender()->m_iWndHeight-1, dwColor, 0, 1);
            p[2] = RtgNVertexVCT( RtGetRender()->m_iWndWidth-1, RtGetRender()->m_iWndHeight-1, dwColor, 1, 1);
            p[3] = RtgNVertexVCT( RtGetRender()->m_iWndWidth-1, 0,    dwColor, 1, 0);

			shader.Shaders[0].SetTexture(m_texPostProcessRenderBlur);
            shader.bEnableBlendFunc = true;
            shader.eBlendDstFunc = RTGBLEND_ONE;
            shader.eBlendSrcFunc = RTGBLEND_SRCALPHA;
            shader.dwTextureFactor = RtgVectorToColor(RtgVertex3(0.5,0.5,0.5),0.5f);
            shader.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
            shader.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
            shader.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
            shader.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;

			RtGetRender()->SetShader(&shader);
            RtGetRender()->SetVertexFormat(RTG_TRANSFORMED_VCT);
			RtGetRender()->SetRenderState(RTGRS_Z_TEST,FALSE);
            RtGetRender()->SetRenderState(RTGRS_Z_WRITE,FALSE);
            RtGetRender()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVCT), 2);
			RtGetRender()->SetRenderState(RTGRS_Z_WRITE,TRUE);
			RtGetRender()->SetRenderState(RTGRS_Z_TEST,TRUE);
            RtGetRender()->SetFogEnable(bBakFog);
		}
        // */
		bResult = this->Render(inCamera, RTGRM_BLENDOBJ_ONLY);

        if (m_bDrawWireframe && !m_bRenderMiniMapMode)
        {
            inDevice.SetRenderState(RTGRS_FILL_MODE, RTG_FILLSOLID);
        }

        inDevice.EndPostProcess();

        if (!m_bRenderMiniMapMode)
        {
            // 画法线
            if (m_bDrawNormal)
            {
                //DrawNormal(inCamera, inDevice, 0xFFFF0000, 10.f);
            }

            // 画地表
            if (m_bDrawAttrWalk || m_bDrawAttrFly || m_bDrawAttrDig)
            {
                BYTE cAttrMask = 0;
                if (m_bDrawAttrWalk) cAttrMask |= 0x01;
                if (m_bDrawAttrFly)  cAttrMask |= 0x02;
                if (m_bDrawAttrDig)  cAttrMask |= 0x04;
                DrawTerrain(inCamera, cAttrMask, 0xFF88FF88, 0xFFFF0000, 0.5f);
            }

            // 渲染Grid的网格
            if (m_bDrawGridBox)
            {
                DWORD dwDrawGridColor = 0xFF0000FF;
                for (gridlist_t::iterator i = m_listRenderGrid.begin(); i != m_listRenderGrid.end(); ++i)
                {
                    RtsSGrid* pGrid = (*i);
                    RtGetRender()->DrawBox(pGrid->vMin, pGrid->vMax, &dwDrawGridColor);
                }
            }

            if (m_bDrawMinimap)
            {
                CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
				SetMinimapMaskTexture("minimap_mask.tga");
             	//DrawMiniMap(5, 5, RtGetRender()->m_iWndWidth-10, RtGetRender()->m_iWndHeight-10, g_fSceneBlockWidth*0.5f/m_fScale, pEditor->m_vCharPos.x, pEditor->m_vCharPos.y);
				DrawMiniMap(RtGetRender()->m_iWndWidth - RtGetRender()->m_iWndHeight/2, 5, RtGetRender()->m_iWndHeight/2, RtGetRender()->m_iWndHeight/2, g_fSceneBlockWidth*0.5f/m_fScale, pEditor->m_vCharPos.x, pEditor->m_vCharPos.y);
            }
        }

    }else
    {
        RtGetRender()->m_dwClearColor = 0x00000000;
        float fSceneWidth = g_fSceneBlockWidth*0.5f/m_fScale;
        float fSceneHeight = fSceneWidth * RtGetRender()->m_iWndHeight / RtGetRender()->m_iWndWidth;
        DrawMiniMap(0, 0, RtGetRender()->m_iWndWidth, RtGetRender()->m_iWndHeight, fSceneWidth, m_fViewPosX, m_fViewPosY);

        m_listViewAreas.clear();

        float fSceneWidth2 = fSceneWidth * 0.5f;
        float fSceneHeight2 = fSceneHeight * 0.5f;
        float fMinX, fMinY, fMaxX, fMaxY;
        RtsAreaHelper* pArea;
        for (pArea=m_Areas.GetFirst(); pArea; pArea=m_Areas.GetNext())
        {
            if (pArea->lMinX>(m_fViewPosX+fSceneWidth2)) continue;
            if (pArea->lMinY>(m_fViewPosY+fSceneHeight2)) continue;
            if (pArea->lMaxX<=(m_fViewPosX-fSceneWidth2)) continue;
            if (pArea->lMaxY<=(m_fViewPosY-fSceneHeight2)) continue;

            fMinX = pArea->lMinX - (m_fViewPosX-fSceneWidth2);
            fMinY = pArea->lMinY - (m_fViewPosY-fSceneHeight2);
            fMaxX = pArea->lMaxX - (m_fViewPosX-fSceneWidth2);
            fMaxY = pArea->lMaxY - (m_fViewPosY-fSceneHeight2);

            if (fMinX<0.f) fMinX = 0.f;
            if (fMinY<0.f) fMinY = 0.f;
            if (fMaxX>fSceneWidth) fMaxX = fSceneWidth;
            if (fMaxY>fSceneHeight) fMaxY = fSceneHeight;

            fMinX = (fMinX/fSceneWidth)*RtGetRender()->m_iWndWidth;
            fMinY = (1.f-fMinY/fSceneHeight)*RtGetRender()->m_iWndHeight;
            fMaxX = (fMaxX/fSceneWidth)*RtGetRender()->m_iWndWidth;
            fMaxY = (1.f-fMaxY/fSceneHeight)*RtGetRender()->m_iWndHeight;

            if (m_pSelectedArea==pArea)
            {
                DrawArea(fMinX, fMaxY, fMaxX, fMinY, 0x80FFFFFF);
            }else
            {
                DrawArea(fMinX, fMaxY, fMaxX, fMinY, 0x80FF0000);
            }

            m_listViewAreas.push_front(pArea);
        }

        if (m_bIsPaint)
        {
            fMinX = m_iPaintbrushMinX<m_iPaintbrushMaxX ? m_iPaintbrushMinX : m_iPaintbrushMaxX;
            fMinY = m_iPaintbrushMinY<m_iPaintbrushMaxY ? m_iPaintbrushMinY : m_iPaintbrushMaxY;
            fMaxX = m_iPaintbrushMinX>m_iPaintbrushMaxX ? m_iPaintbrushMinX : m_iPaintbrushMaxX;
            fMaxY = m_iPaintbrushMinY>m_iPaintbrushMaxY ? m_iPaintbrushMinY : m_iPaintbrushMaxY;
            DrawArea(fMinX, fMinY, fMaxX, fMaxY, 0x8000FF00);
        }

        m_bUpdateViewAreas = TRUE;
    }

    return bResult;
    //unguard;
}

RtsAreaHelper* RtSceneTool::AreaInPoint(int iMouseX, int iMouseY, int iOrder)
{
    float fX, fY;
    float fSceneWidth = g_fSceneBlockWidth*0.5f/m_fScale;
    float fSceneHeight = fSceneWidth * RtGetRender()->m_iWndHeight / RtGetRender()->m_iWndWidth;
    fX = (m_fViewPosX-fSceneWidth*0.5f) + iMouseX*fSceneWidth/RtGetRender()->m_iWndWidth;
    fY = (m_fViewPosY-fSceneHeight*0.5f) + (RtGetRender()->m_iWndHeight-iMouseY)*fSceneHeight/RtGetRender()->m_iWndHeight;

    std::list<RtsAreaHelper*> mHitList;
    RtsAreaHelper* pArea;
    std::list<RtsAreaHelper*>::iterator it;
    for (it=m_listViewAreas.begin(); it!=m_listViewAreas.end(); it++)
    {
        pArea = (*it);
        if (fX<pArea->lMinX || fX>=pArea->lMaxX || fY<pArea->lMinY || fY>=pArea->lMaxY)
        {
            continue;
        }
        mHitList.push_back(pArea);
    }

    if (mHitList.size()==0) return NULL;

    iOrder %= mHitList.size();
    int i=0;
    for (it=mHitList.begin(); it!=mHitList.end(); it++)
    {
        if (i==iOrder)
        {
            return (*it);
        }
        i ++;
    }
    return NULL;
}

void RtSceneTool::DrawArea(long lMinX, long lMinY, long lMaxX, long lMaxY, DWORD dwColor)
{
    RtgNVertexVCT p[5*2*3];

    dwColor &= 0x00FFFFFF;
    DWORD dwLineColor = 0xA0000000 | dwColor;
    DWORD dwAreaColor = 0x30000000 | dwColor;

    int l=1; // 线的宽度

    // area
    p[ 0] = RtgNVertexVCT( lMinX, lMinY, dwAreaColor, 0.f, 0.f);
    p[ 1] = RtgNVertexVCT( lMinX, lMaxY, dwAreaColor, 0.f, 1.f);
    p[ 2] = RtgNVertexVCT( lMaxX, lMaxY, dwAreaColor, 1.f, 1.f);
    p[ 3] = RtgNVertexVCT( lMinX, lMinY, dwAreaColor, 0.f, 0.f);
    p[ 4] = RtgNVertexVCT( lMaxX, lMaxY, dwAreaColor, 1.f, 1.f);
    p[ 5] = RtgNVertexVCT( lMaxX, lMinY, dwAreaColor, 1.f, 0.f);

    // left line
    p[ 6] = RtgNVertexVCT( lMinX  , lMinY  , dwLineColor, 0.f, 0.f);
    p[ 7] = RtgNVertexVCT( lMinX  , lMaxY  , dwLineColor, 0.f, 1.f);
    p[ 8] = RtgNVertexVCT( lMinX+l, lMaxY  , dwLineColor, 1.f, 1.f);
    p[ 9] = RtgNVertexVCT( lMinX  , lMinY  , dwLineColor, 0.f, 0.f);
    p[10] = RtgNVertexVCT( lMinX+l, lMaxY  , dwLineColor, 1.f, 1.f);
    p[11] = RtgNVertexVCT( lMinX+l, lMinY  , dwLineColor, 1.f, 0.f);

    // top line
    p[12] = RtgNVertexVCT( lMinX  , lMinY  , dwLineColor, 0.f, 0.f);
    p[13] = RtgNVertexVCT( lMinX  , lMinY+l, dwLineColor, 0.f, 1.f);
    p[14] = RtgNVertexVCT( lMaxX  , lMinY+l, dwLineColor, 1.f, 1.f);
    p[15] = RtgNVertexVCT( lMinX  , lMinY  , dwLineColor, 0.f, 0.f);
    p[16] = RtgNVertexVCT( lMaxX  , lMinY+l, dwLineColor, 1.f, 1.f);
    p[17] = RtgNVertexVCT( lMaxX  , lMinY  , dwLineColor, 1.f, 0.f);

    // right line
    p[18] = RtgNVertexVCT( lMaxX-l, lMinY  , dwLineColor, 0.f, 0.f);
    p[19] = RtgNVertexVCT( lMaxX-l, lMaxY  , dwLineColor, 0.f, 1.f);
    p[20] = RtgNVertexVCT( lMaxX  , lMaxY  , dwLineColor, 1.f, 1.f);
    p[21] = RtgNVertexVCT( lMaxX-l, lMinY  , dwLineColor, 0.f, 0.f);
    p[22] = RtgNVertexVCT( lMaxX  , lMaxY  , dwLineColor, 1.f, 1.f);
    p[23] = RtgNVertexVCT( lMaxX  , lMinY  , dwLineColor, 1.f, 0.f);

    // bottom line
    p[24] = RtgNVertexVCT( lMinX  , lMaxY-l, dwLineColor, 0.f, 0.f);
    p[25] = RtgNVertexVCT( lMinX  , lMaxY  , dwLineColor, 0.f, 1.f);
    p[26] = RtgNVertexVCT( lMaxX  , lMaxY  , dwLineColor, 1.f, 1.f);
    p[27] = RtgNVertexVCT( lMinX  , lMaxY-l, dwLineColor, 0.f, 0.f);
    p[28] = RtgNVertexVCT( lMaxX  , lMaxY  , dwLineColor, 1.f, 1.f);
    p[29] = RtgNVertexVCT( lMaxX  , lMaxY-l, dwLineColor, 1.f, 0.f);

    RtGetRender()->SetShader(&m_AreaShader);
    RtGetRender()->SetVertexFormat(RTG_TRANSFORMED_VCT);
    RtGetRender()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, &p, sizeof(RtgNVertexVCT), 5*2);
    RtGetRender()->RestoreShader(&m_AreaShader);
}

void RtSceneTool::AddAreaFromScreen()
{
    long lMinX, lMinY, lMaxX, lMaxY;
    lMinX = m_iPaintbrushMinX<m_iPaintbrushMaxX ? m_iPaintbrushMinX : m_iPaintbrushMaxX;
    lMinY = m_iPaintbrushMinY<m_iPaintbrushMaxY ? m_iPaintbrushMinY : m_iPaintbrushMaxY;
    lMaxX = m_iPaintbrushMinX>m_iPaintbrushMaxX ? m_iPaintbrushMinX : m_iPaintbrushMaxX;
    lMaxY = m_iPaintbrushMinY>m_iPaintbrushMaxY ? m_iPaintbrushMinY : m_iPaintbrushMaxY;

    float fSceneWidth = g_fSceneBlockWidth*0.5f/m_fScale;
    float fSceneHeight = fSceneWidth * RtGetRender()->m_iWndHeight / RtGetRender()->m_iWndWidth;

    RtsAreaHelper* pArea = m_Areas.AddArea(RtString(""));
    pArea->lMinX = lMinX*fSceneWidth/RtGetRender()->m_iWndWidth  + m_fViewPosX - fSceneWidth*0.5f;
    pArea->lMaxY = (RtGetRender()->m_iWndHeight-lMinY)*fSceneHeight/RtGetRender()->m_iWndHeight + m_fViewPosY - fSceneHeight*0.5f;
    pArea->lMaxX = lMaxX*fSceneWidth/RtGetRender()->m_iWndWidth  + m_fViewPosX - fSceneWidth*0.5f;
    pArea->lMinY = (RtGetRender()->m_iWndHeight-lMaxY)*fSceneHeight/RtGetRender()->m_iWndHeight + m_fViewPosY - fSceneHeight*0.5f;

    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->AddArea(pArea);
}

void RtSceneTool::DeleteArea(RtsAreaHelper* pArea)
{
    m_Areas.RemoveArea(pArea);
    if (m_pSelectedArea==pArea)
    {
        m_pSelectedArea = NULL;
    }
    m_bUpdateViewAreas = FALSE;
}

void RtSceneTool::SelectArea(RtsAreaHelper* pArea)
{
    m_pSelectedArea = pArea;
}

void RtSceneTool::MoveToArea(RtsAreaHelper* pArea)
{
    m_fViewPosX = pArea->lMinX;
    m_fViewPosY = pArea->lMinY;
}

bool RtSceneTool::ToolSave_CurrBlock()
{
    int _blockx = 0;
    int _blocky = 0;

    GetBlockMapbyPos(RtgVertex2(m_pDevice->m_pCamera->m_vEyePt.x, m_pDevice->m_pCamera->m_vEyePt.y), _blockx, _blocky);
    RtsSceneBlockMap* _blockMap = GetBlockMapbyIndex(_blockx, _blocky);

    if (!_blockMap || !_blockMap->ReourceReady())
        return false;

    ((RtSceneBlock*)_blockMap->m_pBlock)->RecomputeObjectId();
    return _blockMap->Save(m_szFilePath);
}

bool RtSceneTool::ToolNewScene(int iTileSizeX, int iTileSizeY)
{
    //guard;
    g_pSoundMgr->ClearAll();
    this->ClearMap();
    bool bResult = this->NewMap("unname_map.rts");

	//--------------jiang modify---------------------------//
	CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	pEditor->m_NpcBrushList.clear();

	//-----------------------------------------------------//

    if (bResult)
    {
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        pEditor->m_vCharPos.Set(1.f);
        this->GetTerrainHeight(pEditor->m_vCharPos.x, pEditor->m_vCharPos.y, pEditor->m_vCharPos.z);
        RtGetRender()->m_pCamera->SetPosition(pEditor->m_vCharPos);
        pEditor->SetDefaultCamera();
    }else
    {
        //ERR("创建新场景失败.");
    }

    return bResult;
    //unguard;
}

bool RtSceneTool::ToolLoadScene(const char* szFilename)
{
    //guard;
    g_pSoundMgr->ClearAll();
    this->ClearMap();
	//jin.wang 载入全部地图
    bool bResult = this->LoadMap(szFilename, false);

	//--------------jiang modify---------------------------//
	CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	pEditor->m_NpcBrushList.clear();
	//-----------------------------------------------------//
    if (bResult)
    {
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        pEditor->m_vCharPos.Set(1.f);
        this->OnPositionChanged(pEditor->m_vCharPos.x, pEditor->m_vCharPos.y);
        this->GetTerrainHeight(pEditor->m_vCharPos.x, pEditor->m_vCharPos.y, pEditor->m_vCharPos.z);
        RtGetRender()->m_pCamera->SetPosition(pEditor->m_vCharPos);
        pEditor->SetDefaultCamera();
		
		// Added by Wayne Wong 2010-12-27
		m_pBlockMap = m_blockMapArray._blockArray;
    }else
    {
        //ERR("载入场景文件失败.");
        ::MessageBox(NULL, "载入场景文件失败, 文件不存在或者不可读.", "错误", MB_OK);
    }
    return bResult;
    //unguard;
}
bool RtSceneTool::ToolLoadScene(const char* szFilename,int flag)
{
	g_pSoundMgr->ClearAll();
    this->ClearMap();
    bool bResult = this->LoadMap(szFilename, false);
	if (bResult)
	{
	}else
	{
		::MessageBox(NULL, "载入场景文件失败, 文件不存在或者不可读.", "错误", MB_OK);
	}
    return bResult;
}

bool RtSceneTool::ToolSaveScene(const char* szFilename)
{
    //guard;
    bool bResult = this->SaveMap(szFilename);
    if (!bResult)
    {
        //ERR("保存场景文件失败.");
        ::MessageBox(NULL, "保存场景文件失败, 文件可能是只读或者不可写.", "错误", MB_OK);
    }else
    {
        //::MessageBox(NULL, "保存场景文件完成", "错误", MB_OK);
    }
    return bResult;
    //unguard;
}

void RtSceneTool::OnBlockChanged(RtsSceneBlockMap* pNewBlock, RtsSceneBlockMap* pOldBlock)
{
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;

    strcpy(pEditor->m_szFileName, pNewBlock->m_szFileName);
    strcpy(pEditor->m_szBlockName, pNewBlock->m_szBlockName);

    pEditor->m_dwEnvironmentLighting    = pNewBlock->m_dwEnvironmentLighting;
    pEditor->m_dwSunLighting            = pNewBlock->m_dwSunLighting;
    pEditor->m_dwFogColor               = pNewBlock->m_dwFogColor;
    pEditor->m_fFogNear                 = pNewBlock->m_fFogNear;
    pEditor->m_fFogFar                  = pNewBlock->m_fFogFar;

    pEditor->m_bPostProcessEffect               = pNewBlock->m_bPostProcessEffect;
    pEditor->m_fPostProcessEffectContrast       = pNewBlock->m_fPostProcessEffectContrast;
    pEditor->m_fPostProcessEffectIntensity      = pNewBlock->m_fPostProcessEffectIntensity;
    pEditor->m_bPostProcessObjectHL             = pNewBlock->m_bPostProcessObjectHL;
    pEditor->m_fPostProcessObjectHLScale        = pNewBlock->m_fPostProcessObjectHLScale;
    pEditor->m_dwPostProcessObjectHLColorEmi    = pNewBlock->m_dwPostProcessObjectHLColorEmi;
    pEditor->m_dwPostProcessObjectHLColorDif    = pNewBlock->m_dwPostProcessObjectHLColorDif;
    pEditor->m_dwPostProcessObjectHLColorSpe    = pNewBlock->m_dwPostProcessObjectHLColorSpe;
    pEditor->m_fPostProcessObjectHLColorSpePower= pNewBlock->m_fPostProcessObjectHLColorSpePower;
    pEditor->m_fSunInclination                  = pNewBlock->m_fSunInclination;
    pEditor->m_fSunRotation                     = pNewBlock->m_fSunRotation;

    strcpy(pEditor->m_szBlockFlagTexture, pNewBlock->m_szBlockFlagTexture);

    pEditor->m_fBackMusicStartTime      = pNewBlock->m_fBackMusicStartTime;
    pEditor->m_fBackMusicDifferenceTime = pNewBlock->m_fBackMusicDifferenceTime;
    strcpy(pEditor->m_szBackMusic, pNewBlock->m_szBackMusic);
	//faya add for temp

	if(pNewBlock->m_szBackMusic)//if(StrCmp(pNewBlock->m_szBackMusic,pOldBlock->m_szBackMusic))
		g_pMusicThread->Play(pNewBlock->m_szBackMusic);
	
    // Update
    pEditor->UpdateProperty();

    if (IsShowBlockFlag())
    {
        if (pNewBlock==NULL) return;
		RtCoreLog().Info("GcPlayer::OnBlockChanged 进入新的块 [%s]\n", pNewBlock->GetBlockName());
        if (pNewBlock->GetFlagTexture()[0])
        {
            g_pScene->ShowSceneFlagTexture(pNewBlock->GetFlagTexture());
        }
    }
}

void RtSceneTool::GridItemChanged()
{
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    RtsSceneBlockMap* pBlock = (m_iCurrentBlockX<0)?(NULL):(m_pBlockMap+(m_iCurrentBlockY*m_iBlockXCnt+m_iCurrentBlockX));
    if (pBlock==NULL) return;

    strcpy(pBlock->m_szFileName, pEditor->m_szFileName);
    strcpy(pBlock->m_szBlockName, pEditor->m_szBlockName);

    pBlock->m_dwEnvironmentLighting    = pEditor->m_dwEnvironmentLighting;
    pBlock->m_dwSunLighting            = pEditor->m_dwSunLighting;

    pBlock->m_fPostProcessEffectContrast  = pEditor->m_fPostProcessEffectContrast;
    pBlock->m_fPostProcessEffectIntensity = pEditor->m_fPostProcessEffectIntensity;

    pBlock->SetSunDir(pEditor->m_fSunInclination, pEditor->m_fSunRotation);

    pBlock->m_bPostProcessEffect = pEditor->m_bPostProcessEffect;
    if (pEditor->m_bPostProcessObjectHL)
    {
        pBlock->m_fPostProcessObjectHLScale         = pEditor->m_fPostProcessObjectHLScale;
        pBlock->m_dwPostProcessObjectHLColorEmi     = pEditor->m_dwPostProcessObjectHLColorEmi;
        pBlock->m_dwPostProcessObjectHLColorDif     = pEditor->m_dwPostProcessObjectHLColorDif;
        pBlock->m_dwPostProcessObjectHLColorSpe     = pEditor->m_dwPostProcessObjectHLColorSpe;
        pBlock->m_fPostProcessObjectHLColorSpePower = pEditor->m_fPostProcessObjectHLColorSpePower;
    }else
    {
        pBlock->m_fPostProcessObjectHLScale = 1.f;
        pBlock->m_dwPostProcessObjectHLColorEmi = 0x00222222;
        pBlock->m_dwPostProcessObjectHLColorDif = 0x00555555;
        pBlock->m_dwPostProcessObjectHLColorSpe = 0x00FFFFFF;
        pBlock->m_fPostProcessObjectHLColorSpePower = 8.f;
    }
    pBlock->m_bPostProcessObjectHL = pEditor->m_bPostProcessObjectHL;

    pBlock->m_dwFogColor               = pEditor->m_dwFogColor;
    pBlock->m_fFogNear                 = pEditor->m_fFogNear;
    pBlock->m_fFogFar                  = pEditor->m_fFogFar;

    strcpy(pBlock->m_szBlockFlagTexture, pEditor->m_szBlockFlagTexture);

    pBlock->m_fBackMusicStartTime      = pEditor->m_fBackMusicStartTime;
    pBlock->m_fBackMusicDifferenceTime = pEditor->m_fBackMusicDifferenceTime;
    strcpy(pBlock->m_szBackMusic, pEditor->m_szBackMusic);

    RecreateGrass((RtSceneBlock*)pBlock->m_pBlock);
    g_pScene->RequestEnvUpdate(RtGetRender()->m_pCamera->GetEyePt());
}

void RtSceneTool::GenerateMiniMap(int x, int y)
{
    const float fMinmapfar = 5000.f;
    char szFileName[256];
    DWORD dwRenderTargetTexture = NULL;
    RtsSceneBlockMap* pMap = GetBlockMap(x, y);

    if (pMap==NULL || pMap->m_szFileName[0] == 0) 
        return;

    RequestBlockReadyForSM(Int2(x + m_iCenterPosX, y + m_iCenterPosY));
    
    RtgVertex3 vPos;
    this->SetRenderMode(TRUE);
    float fFar = RtGetRender()->m_pCamera->GetFarPlane();
    RtGetRender()->m_pCamera->SetFarPlane(fMinmapfar);

    RtgViewWindowInfo infoSave = RtGetRender()->GetViewWindowInfo(RTGVWM_WINDOWED);
    RtGetRender()->SetViewWindowInfo(RTGVWM_WINDOWED, 260,260, 32, 0);
    RtGetRender()->SetViewMode(RTGVWM_WINDOWED, false);

    for (int i = 0; i < 4; ++i)
    {
        int j = strlen(pMap->m_szFileName) - 4;
        strncpy(pMap->m_szMiniMapFile[i], pMap->m_szFileName, j);
        sprintf(pMap->m_szMiniMapFile[i]+j, "%d.dds",  i);
        vPos.x = x * g_fSceneBlockWidth  + (i%2)*(g_fSceneBlockWidth *0.5f) + g_fSceneBlockWidth *0.25f;
        vPos.y = y * g_fSceneBlockHeight + (i/2)*(g_fSceneBlockHeight*0.5f) + g_fSceneBlockWidth *0.25f;
        vPos.z = ((RtSceneBlock*)pMap->m_pBlock)->m_vMax.z + 128.f;
        RtGetRender()->m_pCamera->SetPosition(vPos);
        RtGetRender()->RenderScene();
        sprintf(szFileName, "scene/minimap/%s", pMap->m_szMiniMapFile[i]);
        RtGetRender()->SaveRenderTargetToFile(szFileName, false);
    }

    ((CFrameWnd*)AfxGetMainWnd())->RecalcLayout();
    this->SetRenderMode(FALSE);
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    SendMessage((HWND)pEditor->GetWndHandle(), WM_EXITSIZEMOVE, 0, 0);
    RtGetRender()->m_pCamera->SetFarPlane(fFar);

}

void RtSceneTool::OnPositionChanged(float fX, float fY)
{
    if (fX == m_fPervX && fY == m_fPervY)
        return;
    m_fPervX = fX;
    m_fPervY = fY;


    int bx, by;
    g_pScene->RtsSceneDoc::GetBlockByPos(fX, fY, bx, by);
    if (!(bx>=0 && bx<g_pScene->GetBlockCntX() && by>=0 && by<g_pScene->GetBlockCntY()))
        return;

	//g_pSceneThread->Moveto(RtgVertex3(fX, fY, 0.f));
    g_pScene->RequestBlockReady(RtgVertex3(fX, fY, 0.f));

    if (bx!=m_iCurrentBlockX || by!=m_iCurrentBlockY)
    {
        RtsSceneBlockMap* pBlockMap = g_pScene->GetBlockMap(bx-g_pScene->GetCenterX(), by-g_pScene->GetCenterY());
        RtsSceneBlockMap* pOldBlockMap = (m_iCurrentBlockX<0)?(NULL):(g_pScene->GetBlockMap(m_iCurrentBlockX-g_pScene->GetCenterX(), m_iCurrentBlockY-g_pScene->GetCenterY()));
        m_iCurrentBlockX = bx;
        m_iCurrentBlockY = by;
        OnBlockChanged(pBlockMap, pOldBlockMap);
    }
}


/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

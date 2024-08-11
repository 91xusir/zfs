#include "stdafx.h"
#include "tool_scene.h"
#include "BrushMapView.h"
#include "BrushVertexView.h"
#include "BrushColorView.h"
#include "area_list_view.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
//tim.yang 路点列表头文件
#include "pathpoint_list_view.h"
#include "MainFrame.h"
#include "core/rt2_core.h"

#include <windows.h>
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "audio/rt_audio.h"
#include "brush_tool.h"
#include "brush_tool_object.h"
#include "EditorFrame.h"
#include "GridItems.h"

using namespace std;
RT_IMPLEMENT_DYNCREATE(CEditorFrame, CRtgAppFrame, NULL, "")
RtSceneTool* g_pSceneTool;

CEditorFrame::CEditorFrame()
{
    //guard;
    m_pMainFrame = NULL;
    m_iCameraMode = 0;
    m_iFillTerrain = 0;
    m_iLastBrushMouseBtn = 0;
    m_iCameraPos.Set(0.f, 0.f);
    m_bDrawCharPos = false;
    m_vCharPos.Set(0.f);
    m_bShowStatAudio = false;
    m_bShowStatChar = false;
    m_bShowStatScene = false;

    m_bCheckSnapToGrid = FALSE;

    m_szFileName[0]             = 0;
    m_szBlockName[0]            = 0;
    m_szBlockFlagTexture[0]     = 0;
    m_szBackMusic[0]            = 0;
    m_fBackMusicStartTime       = 0.f;
    m_fBackMusicDifferenceTime  = 0.f;
    m_dwEnvironmentLighting     = 0;
    m_dwSunLighting             = 0;
    m_dwClearColor              = 0;
    m_dwFogColor                = 0;
    m_dwSunSpecular             = 0;
    m_dwTerrainAmbient          = 0;
    m_fSunInclination           = 0.f;
    m_fSunRotation              = 0.f;
    m_fFogNear = m_fFogFar      = 0.f;

    m_dwCurLightColor = RGB(0x50, 0x50,0x50);
    m_fCurLightRange = 200.f;
    m_fCurLightConstantAttenuation = 0.f;
    m_fCurLightLinearAttenuation = 0.003f;
    m_fCurLightQuadraticAttenuation = 0.f;

    m_bPostProcessEffect = FALSE;
    m_bPostProcessObjectHL = FALSE;
    m_fPostProcessEffectContrast = 0;
    m_fPostProcessEffectIntensity = 0.5f;
    m_fPostProcessObjectHLScale = 1.f;
    m_dwPostProcessObjectHLColorEmi = 0x00222222;
    m_dwPostProcessObjectHLColorDif = 0x00555555;
    m_dwPostProcessObjectHLColorSpe = 0x00FFFFFF;
    m_fPostProcessObjectHLColorSpePower = 8;

    m_iTakeGeometryVertexNum = 0;
	m_bAddNpc = false;
	m_NpcBrushList.clear();
    //unguard;
}

CEditorFrame::~CEditorFrame()
{
    //guard;
    //unguard;
}

void CEditorFrame::LoadConfigFile(const char* szFilename)
{
    //guard;
    RtIni ini;

    // Camera
    float fCameraFOV             = 45.0f;
    float fCameraAspect          = 4.f/3.f;
    float fCameraNear            = 10.f;
    float fCameraFar             = 5000.f;

    m_fCameraDefaultYaw = 0.f;
    m_fCameraDefaultDegree   = 45.f;
    m_fCameraDefaultDistance = 100.f;

    // Light
    RtgVertex3 c3(0.0f, 1.0f, 0.0f);
    m_Light.eMode = RTG_LIGHT_DIRECTION;
    m_Light.vSpecular = m_Light.vDiffuse = m_Light.vAmbient = c3;
    m_Light.vDirection.x = -1.0f; // 从右到左为 -1 -> 1
    m_Light.vDirection.y =  0.5f; // 从后到前为 -1 -> 1
    m_Light.vDirection.z = -0.4f; // 从上到下为 -1 -> 1
    m_Light.vDirection.Normalize();

    bool bLoad = true;
	if (!ini.OpenFile(szFilename))
    {
       // ERR1("错误: 不能打开场景编辑器配置文件[%s].尝试载入[clt_game.ini]\n", szFilename);
        if (!ini.OpenFile("clt_game.ini"))
        {
           // ERR("错误: 尝试载入[game.ini]失败，摄像机参数将使用默认值\n");
            bLoad = false;
        }
    }

    if (bLoad)
    {
        // Camera
		ini.GetEntry("Camera", "FOV", &fCameraFOV);
        ini.GetEntry("Camera", "Aspect", &fCameraAspect);
        ini.GetEntry("Camera", "Near", &fCameraNear);
        ini.GetEntry("Camera", "Far", &fCameraFar);
        ini.GetEntry("Camera", "DefaultYaw", &m_fCameraDefaultYaw);
        ini.GetEntry("Camera", "DefaultDegree", &m_fCameraDefaultDegree);
        ini.GetEntry("Camera", "DefaultDistance", &m_fCameraDefaultDistance);

        // Direction Light
        ini.GetEntry("DirectionLight", "DirX", &(m_Light.vDirection.x));
        ini.GetEntry("DirectionLight", "DirY", &(m_Light.vDirection.y));
        ini.GetEntry("DirectionLight", "DirZ", &(m_Light.vDirection.z));

		ini.CloseFile();
    }

    RtGetRender()->m_pCamera->SetOrthoParams(0, 0, 2000, 2000, fCameraNear, fCameraFar);
    RtGetRender()->m_pCamera->SetProjParams(DegreeToRadian(fCameraFOV), fCameraAspect, fCameraNear, fCameraFar);
    RtGetRender()->m_pCamera->SetViewParams(RtgVertex3(10.0f, 10.0f, 10.0f), RtgVertex3(0.0f, 0.0f, 200.0f), RtgVertex3(0.0f, 0.0f, 1.0f));
    SetDefaultCamera();
    //unguard;
}

void CEditorFrame::SetDefaultCamera()
{
    //guard;
    RtGetRender()->m_pCamera->UpdateMatrix();
    RtGetRender()->m_pCamera->SetPitch(0.f);
    RtGetRender()->m_pCamera->SetYaw(DegreeToRadian(m_fCameraDefaultYaw));
    RtGetRender()->m_pCamera->SetRoll(DegreeToRadian(m_fCameraDefaultDegree));
    RtGetRender()->m_pCamera->MoveUp(m_fCameraDefaultDistance);
    //unguard;
}

void CEditorFrame::SaveConfigFile(const char* szFilename)
{
    return;
    //guard;
    RtIni ini;
    if (ini.OpenFile(szFilename, true))
    {
        ini.AddEntry("Sun", "aTime", (long)10);
        ini.CloseFile(true);
    }
    //unguard;
}

#define C_COLOR(x) ((GetRValue(x)<<16)|(GetGValue(x)<<8)|GetBValue(x))

bool CEditorFrame::OnFrameInit()
{
    //guard;
    char szFirstCmd[20] = "> stat fps";
    DoConsoleCommand(szFirstCmd);

    LoadConfigFile("tool_scene.ini");

    // Render State & View check
    RtGetRender()->m_bEnableViewCheck = FALSE;

    m_ShaderNull.iNumStage = 0;

    // Texture Path
    RtGetRender()->GetTextMgr()->AddTextSearchPath("scene/texture");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("scene/texture/sm");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("language/chinese/texture");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("creature/texture");

    //RtGetRender()->m_Texture.SetTexturePath("language\\chinese\\texture", 1);
    //RtGetRender()->m_Texture.SetTexturePath("creature\\texture", 2);
    //RtGetRender()->m_Texture.SetTexturePath("scene\\texture", 3);
    //RtGetRender()->m_Texture.SetTexturePath("scene\\texture\\sm", 4);

    g_pSceneTool = new RtSceneTool;
    g_pScene = g_pSceneTool;
    // Scene
    if (!g_pSceneTool->OnceInit(RtGetRender()))
    {
        CHECKEX("ERROR: Scene init fail!");
        return false;
    }

    // 设置默认值
    /*
    g_pSceneTool->SetSkyLighting(C_COLOR(m_dwEnvColor));

    g_pSceneTool->m_lightDirect.vDiffuse.x = GetRValue(m_dwSunColor)/255.f;
    g_pSceneTool->m_lightDirect.vDiffuse.y = GetGValue(m_dwSunColor)/255.f;
    g_pSceneTool->m_lightDirect.vDiffuse.z = GetBValue(m_dwSunColor)/255.f;

    g_pSceneTool->m_lightDirect.vSpecular.x = GetRValue(m_dwSunSpecular)/255.f;
    g_pSceneTool->m_lightDirect.vSpecular.y = GetGValue(m_dwSunSpecular)/255.f;
    g_pSceneTool->m_lightDirect.vSpecular.z = GetBValue(m_dwSunSpecular)/255.f;

    this->m_dwFogColor = g_pDevice->GetFogColor();
    this->m_lFogNear = g_pDevice->GetFogNear();
    this->m_lFogFar = g_pDevice->GetFogFar();
    */

    RtgMatrix16 m16;
    m16.Unit();
    m16.RotateLZ(DegreeToRadian(m_fSunRotation));
    m16.RotateLX(DegreeToRadian(m_fSunInclination+180.f));
    m16.GetRow(2, g_pSceneTool->m_lightDirect.vDirection);

    g_pSceneTool->m_lightTerrainAmbient.x = GetRValue(m_dwTerrainAmbient)/255.f;
    g_pSceneTool->m_lightTerrainAmbient.y = GetGValue(m_dwTerrainAmbient)/255.f;
    g_pSceneTool->m_lightTerrainAmbient.z = GetBValue(m_dwTerrainAmbient)/255.f;

    this->m_bFog = RtGetRender()->GetFogEnable();

    return true;
    //unguard;
}

void CEditorFrame::OnFrameClose()
{
    //guard;
    if (g_pSceneTool)
    {
        SaveConfigFile("scene_editor_cfg.ini");
        g_pSceneTool->Clear();
    }
    CRtgAppFrame::OnFrameClose();
   // unguard;
}

void CEditorFrame::OnFrameMove(float fDifSecond)
{
    
   // lyymark 简单限制帧率
    int millisecondsPerFrame = 1000 / 30;  // 30 FPS  
    Sleep(millisecondsPerFrame);  

    CHECK(RtGetRender()->m_pCamera!=NULL);
    static float fLastTime = 0.f;
    float fCurTime = RtGetRender()->GetAppTime();

    if (!g_pSceneTool->m_bAreaTool)
    {
        m_BrushTool.OnFrameMove(fCurTime-fLastTime);

        if ((fCurTime-fLastTime)>0.1f)
        {
            if (g_pSceneTool->GetBlockCntX())
            {
                float fDist;
                RtgVertex3 vOrig, vDir;
                int iOutX, iOutY;
                RtGetRender()->GetPickRayFromScreen(RtGetRender()->m_iWndWidth/2, RtGetRender()->m_iWndHeight*2/3, vOrig, vDir);
                RtsObject* pObj = g_pSceneTool->PickRay(vOrig, vDir, fDist, iOutX, iOutY, PICK_TERRAIN);
                if ( pObj && fDist>0.f )
                {
                    m_bDrawCharPos = true;
                    m_vCharPos = vOrig + vDir*fDist;
                    g_pSceneTool->OnPositionChanged(m_vCharPos.x, m_vCharPos.y);

                    g_pSceneTool->UpdateAudioListener(m_vCharPos);
                    g_pSoundMgr->UpdateAll(&m_vCharPos, fCurTime);
                }else
                {
                    RtgVertex3 vEye = RtGetRender()->m_pCamera->GetEyePt();
                    g_pSceneTool->OnPositionChanged(vEye.x, vEye.y);
                }
            }
            fLastTime = fCurTime;
        }

        g_pSceneTool->OnFrameMove(fDifSecond);
    }

}

void CEditorFrame::OnMouseLDown(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
        if (GetKeyState(VK_CONTROL)&0xFF00)
        {
            RtsAreaHelper* pArea = g_pSceneTool->AreaInPoint(x, y, g_pSceneTool->m_iHittestOrder);
            if (pArea)
            {
                SelectArea(pArea);
            }
        }else
        {
            g_pSceneTool->m_bIsPaint = TRUE;
            g_pSceneTool->m_iPaintbrushMaxX = g_pSceneTool->m_iPaintbrushMinX = x;
            g_pSceneTool->m_iPaintbrushMaxY = g_pSceneTool->m_iPaintbrushMinY = y;
        }
    }else
    {
		//tim.yang 将鼠标点击的路点坐标转换后保存下来
		if(g_pSceneTool->m_bPathPointTool)
		{
			m_pMainFrame->m_wndPathPointView.OnMouseLDown(x, y);
		}
		else
		{
			//end
			m_iLastBrushMouseBtn = 1;
			OnBrushBegin(iButton, x, y);
		}
		CRtgAppFrame::OnMouseLDown( x, y);

    }
    //unguard;
}

void CEditorFrame::OnMouseLDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
        g_pSceneTool->m_iPaintbrushMaxX = x;
        g_pSceneTool->m_iPaintbrushMaxY = y;
    }else
    {
        if (m_iLastBrushMouseBtn==1) OnBrushMove(iButton, x, y);
        CRtgAppFrame::OnMouseLDrag( x, y, increaseX, increaseY);
    }
    //unguard;
}

void CEditorFrame::OnMouseLUp(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
        if (g_pSceneTool->m_bIsPaint)
        {
            g_pSceneTool->m_bIsPaint = FALSE;
            if (g_pSceneTool->m_iPaintbrushMinX!=g_pSceneTool->m_iPaintbrushMaxX && g_pSceneTool->m_iPaintbrushMinY!=g_pSceneTool->m_iPaintbrushMaxY)
            {
                g_pSceneTool->m_iPaintbrushMaxX = x;
                g_pSceneTool->m_iPaintbrushMaxY = y;
                g_pSceneTool->AddAreaFromScreen();
            }
        }
    }else
    {
        if (m_iLastBrushMouseBtn==1) m_BrushTool.OnEndBrush(iButton, x, y);
        CRtgAppFrame::OnMouseLUp( x, y);
    }
    //unguard;
}

void CEditorFrame::OnMouseRDown(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
    }else
    {
        m_iLastBrushMouseBtn = 2;
        OnBrushBegin(iButton, x, y);
        CRtgAppFrame::OnMouseRDown( x, y);
    }
    //unguard;
}

void CEditorFrame::OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
        if (!g_pSceneTool->m_bIsPaint)
        {
            g_pSceneTool->m_fViewPosX -= increaseX * 10.f;
            g_pSceneTool->m_fViewPosY += increaseY * 10.f;
        }
    }else
    {
        if (m_iLastBrushMouseBtn==2) OnBrushMove(iButton, x, y);
        CRtgAppFrame::OnMouseRDrag( x, y, increaseX, increaseY);
    }
    //unguard;
}

void CEditorFrame::OnMouseRUp(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
    }else
    {
        if (m_iLastBrushMouseBtn==2) m_BrushTool.OnEndBrush(iButton, x, y);
        CRtgAppFrame::OnMouseRUp( x, y);
    }
    //unguard;
}

void CEditorFrame::OnMouseMDown(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
    }else
    {
        m_iLastBrushMouseBtn = 4;
        OnBrushBegin(iButton, x, y);
        CRtgAppFrame::OnMouseMDown( x, y);
    }
    //unguard;
}

void CEditorFrame::OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
    }else
    {
        if (m_iLastBrushMouseBtn==4) OnBrushMove(iButton, x, y);
        CRtgAppFrame::OnMouseMDrag( x, y, increaseX, increaseY);
    }
    //unguard;
}

void CEditorFrame::OnMouseMUp(int iButton, int x, int y)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
    }else
    {
        if (m_iLastBrushMouseBtn==4) m_BrushTool.OnEndBrush(iButton, x, y);
        CRtgAppFrame::OnMouseMUp( x, y);
    }
    //unguard;
}

void CEditorFrame::OnMouseWheel(int iButton, long vDelta)
{
    //guard;
    if (g_pSceneTool->m_bAreaTool)
    {
        if (GetKeyState(VK_CONTROL)&0xFF00)
        {
            g_pSceneTool->m_iHittestOrder ++;
            POINT ptMouse;
            GetCursorPos(&ptMouse);
            ((CSceneEditorView*)m_pView)->ScreenToClient(&ptMouse);
            RtsAreaHelper* pArea = g_pSceneTool->AreaInPoint(ptMouse.x, ptMouse.y, g_pSceneTool->m_iHittestOrder);
            if (pArea)
            {
                SelectArea(pArea);
            }
        }else
        {
            g_pSceneTool->m_fScale += vDelta*0.05f;
            if (g_pSceneTool->m_fScale>3.f) g_pSceneTool->m_fScale = 3.f;
            else if (g_pSceneTool->m_fScale<0.3f) g_pSceneTool->m_fScale = 0.3f;
        }
    }else
    {
        if (GetKeyState('C')&0xFF00) // 'C' Key
        {
            if (m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT || m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT_MOVE)
            {
                RtgMatrix16 m16 = ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_OffsetMatrix;
                RtgVertex3 v3 = m16.Position();
                m16._30 = 0.f;
                m16._31 = 0.f;
                m16._32 = 0.f;
                m16.RotateLZ(PI*0.25f*vDelta);
                m16.SetRow(3, v3);
                ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_OffsetMatrix = m16;
            }
            return;
        }else if (GetKeyState('X')&0xFF00) // 'X' Key
        {
            if (m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT || m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT_MOVE)
            {
                ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_OffsetMatrix._32 += 5.f*vDelta;
            }
            return;
        }else if (GetKeyState('Z')&0xFF00) // 'Z' Key
        {
			if (m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT || m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT_MOVE)
            {
                float fScale = 0.1f*vDelta;
                if (((CBrusherObject*)m_BrushTool.m_pBrusher)->m_lObjectType==3) // Model
                {
                    float fSrc = ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_fObjectScale;
                    fScale += fSrc;
                    ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_fObjectScale = fScale;
                    ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_OffsetMatrix.NormalizeMatrix();
                    ((CBrusherObject*)m_BrushTool.m_pBrusher)->m_OffsetMatrix.Scale(fScale, fScale, fScale);
                }
            }
            return;
        }else if (GetKeyState('M')&0xFF00) // 'M' Key
        {
            g_pScene->SetMinimapRotate(g_pScene->GetMinimapRotate()+5.f*vDelta);
			g_pBackMusic->Play("mountain.ogg");
            return;
        }
		//CRtgAppFrame::OnMouseWheel(iButton, vDelta);
        CRtgAppFrame::MouseWheel( vDelta,0,0);
    }
    //unguard;
}

void CEditorFrame::OnBrushBegin(int iButton, int x, int y)
{
    funguard;
    if (!m_BrushTool.OnBeginBrush(m_iLastBrushMouseBtn, iButton, x, y))
    {
        m_iLastBrushMouseBtn = 0;		
    }
	else
	{
		/*char* tempName;
	    char tempPath[256];
	    GetCurrentDirectory(256,tempPath);
	    tempName = strcat(tempPath,"\\scene\\temp\\temp.rts");
		((RtSceneTool*)g_pScene)->ToolSaveScene(tempName); 
		tmp= RtGetRender()->m_pCamera->GetViewMatrix();
		t_eye= RtGetRender()->m_pCamera->GetEyePt();*/
	}
    fununguard;
}

void CEditorFrame::OnBrushMove(int iButton, int x, int y)
{
    funguard;
    m_BrushTool.OnBrushMove(iButton, x, y);
    fununguard;
}

void CEditorFrame::OnKeyDown(int iButton, int iKey)
{
    funguard;
    if (g_pSceneTool->m_bAreaTool)
    {
        switch (iKey)
        {
        case VK_DELETE:
            RemoveArea(g_pSceneTool->m_pSelectedArea);
            break;
        }
    }else
    {
        switch (iKey)
        {
        case VK_F1:
            m_iCameraMode = !m_iCameraMode;
            if (m_iCameraMode)
            {
                m_LastEditorCamera = *((RtgCameraEditor*)(RtGetRender()->m_pCamera));
            } 
            break;
        case VK_F2:
            if (RtGetRender()->m_pCamera->IsPerspective())
            {
                g_pScene->SetRenderMode(TRUE);
            }else
            {
                g_pScene->SetRenderMode(FALSE);
            }
            break;
        case VK_F9:
            switch (RtGetRender()->GetViewWindowMode())
            {
			case RTGVWM_WINDOWED:

				RtGetRender()->SetViewWindowInfo(RTGVWM_WINDOWED,800,600,32,0);
				RtGetRender()->SetViewMode(RTGVWM_WINDOWED);
				break;
            /*case RTGVWM_WINDOWED:   RtGetRender()->SetViewMode(RTGVWM_TOPWINDOW);   break;
            case RTGVWM_TOPWINDOW:  RtGetRender()->SetViewMode(RTGVWM_FULLSCREEN);  break;
            case RTGVWM_FULLSCREEN: RtGetRender()->SetViewMode(RTGVWM_WINDOWED);    break;*/
            }
            break;
        case VK_ESCAPE:
            if (m_BrushTool.GetBrushType()!=CBrushTool::BT_NONE)
            {
                m_BrushTool.SetBrushType(CBrushTool::BT_NONE);
            }
            break;

        // Brush Command

        case VK_DELETE:
            m_BrushTool.Command(1);

			break;

        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_PRIOR:      // Page Up
        case VK_NEXT:       // Page Down
        case VK_OEM_PLUS:   // +
        case VK_OEM_MINUS:  // -
        case VK_OEM_4:      // {
        case VK_OEM_6:      // }
            if (m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT_ADJUST || m_BrushTool.GetBrushType()==CBrushTool::BT_WATER)
            {
                m_BrushTool.Command(2, iKey);
            }
            break;
        }
    }
    fununguard;
}

void CEditorFrame::UpdateProperty()
{
    if (m_pMainFrame)
    {
        if (m_pMainFrame->m_wndBrushPropertyGrid.GetSafeHwnd()==0) return;
        CXTPPropertyGridItem* pItem;
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BLOCK_FILENAME);
        if (pItem) ((CGridItemSizeString*)pItem)->SetString(m_szFileName);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BLOCK_NAME);
        if (pItem) ((CGridItemSizeString*)pItem)->SetString(m_szBlockName);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_ENVIRONMENT);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwEnvironmentLighting);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_ENV_SUN_COLOR);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwSunLighting);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_FOG_COLOR);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwFogColor);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_FOG_MIN);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fFogNear);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_FOG_MAX);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fFogFar);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BACKMUSIC);
        if (pItem) ((CGridItemSizeString*)pItem)->SetString(m_szBackMusic);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BACKMUSIC_START_TIME);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fBackMusicStartTime);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BACKMUSIC_DIFF_TIME);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fBackMusicDifferenceTime);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_BLOCK_FLAG_TEXTURE);
        if (pItem) ((CGridItemSizeString*)pItem)->SetString(m_szBlockFlagTexture);

        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_POSTPROCESS);
        if (pItem) ((CXTPPropertyGridItemBool*)pItem)->SetBool(m_bPostProcessEffect);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT);
        if (pItem) ((CXTPPropertyGridItemBool*)pItem)->SetBool(m_bPostProcessObjectHL);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_POSTPROCESS_CONTRAST);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fPostProcessEffectContrast);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_POSTPROCESS_INTENSITY);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fPostProcessEffectIntensity);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SCALE);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fPostProcessObjectHLScale);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_EMI);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwPostProcessObjectHLColorEmi);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_DIF);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwPostProcessObjectHLColorDif);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_SPE);
        if (pItem) ((CGridItemColor*)pItem)->SetMyColor(&m_dwPostProcessObjectHLColorSpe);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SPE_POWER);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fPostProcessObjectHLColorSpePower);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_ENV_SUN_INCLINATION);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fSunInclination);
        pItem = m_pMainFrame->m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_ENV_SUN_ROTATION);
        if (pItem) ((CGridItemFloat*)pItem)->SetFloat(&m_fSunRotation);

    }
}

void CEditorFrame::UpdateAreaList()
{
    m_pMainFrame->m_wndAreaView.UpdateAreaList();
}

void CEditorFrame::AddArea(RtsAreaHelper* pArea)
{
    m_pMainFrame->m_wndAreaView.AddArea(pArea);
}

void CEditorFrame::RemoveArea(RtsAreaHelper* pArea)
{
    m_pMainFrame->m_wndAreaView.RemoveArea(pArea);
}

void CEditorFrame::SelectArea(RtsAreaHelper* pArea)
{
    m_pMainFrame->m_wndAreaView.SelectArea(pArea);
}

void CEditorFrame::OnRender()
{

    funguard;
#ifdef _DEBUG
    //rtSleep(0.01f);
#endif
    CRtgAppFrame::OnRender();

    RtgMatrix16 m16;
    m16.Unit();
    RtGetRender()->SetMatrix(RTGTS_WORLD, &m16);

    // Light
    m_BrushTool.OnBeginRender(RtGetRender(), *g_pSceneTool);

    // 渲染场景
    if (m_iCameraMode)
    {
        g_pSceneTool->ToolRender(m_LastEditorCamera, *RtGetRender());

		if (g_pSceneTool->m_bDrawAttrWalk || g_pSceneTool->m_bDrawAttrFly || g_pSceneTool->m_bDrawAttrDig)
			m_pMainFrame->m_wndPathPointView.RenderLinks(m_LastEditorCamera, *RtGetRender());
    }
	else
    {
        g_pSceneTool->ToolRender(*RtGetRender()->m_pCamera, *RtGetRender());
		if (g_pSceneTool->m_bDrawAttrWalk || g_pSceneTool->m_bDrawAttrFly || g_pSceneTool->m_bDrawAttrDig)
			m_pMainFrame->m_wndPathPointView.RenderLinks(*RtGetRender()->m_pCamera, *RtGetRender());
    }
    //g_pDevice->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);

    if (g_pSceneTool->IsRenderMiniMapMode()) return;

    RtGetRender()->SetMatrix(RTGTS_WORLD, &m16);
    RtGetRender()->SetRenderState(RTGRS_LIGHTING, FALSE);
    RtGetRender()->SetShader(&m_ShaderNull);
    RtGetRender()->DrawAxis(m16, 1000.f);

    DWORD dwColor = 0xFFFF0000;
    //g_pDevice->DrawLine(m_vLastStart, m_vLastEnd, &dwColor);
    if (m_bDrawCharPos && g_pScene->m_bDrawHelper)
    {
        m16.Unit();
        m16.SetRow(3, m_vCharPos);
        RtGetRender()->DrawAxis(m16, 20);
    }

    m_BrushTool.OnRender(RtGetRender(), *g_pSceneTool);
    RtGetRender()->RestoreShader(&m_ShaderNull);
    m_BrushTool.OnEndRender(RtGetRender(), *g_pSceneTool);

    m_BrushTool.DrawBrushString(RtGetRender());

   
    char szOutputString[512];

    sprintf(szOutputString, "(%d, %d) (%.f, %.f , %.f)", m_BrushTool.m_iLastMoveTerrainX, m_BrushTool.m_iLastMoveTerrainY, m_BrushTool.m_vLastCursorPos.x, m_BrushTool.m_vLastCursorPos.y , m_BrushTool.m_vLastCursorPos.z);
    RtGetRender()->DrawString(2, RtGetRender()->m_iWndHeight-17, 0xFF000000, szOutputString);
    RtGetRender()->DrawString(2, RtGetRender()->m_iWndHeight-18, 0xFFFF00FF, szOutputString);

    if (m_bShowStatAudio)
    {
        sprintf(szOutputString, "Move: [%d, %d]", m_BrushTool.m_iLastSelX, m_BrushTool.m_iLastSelY);
        RtGetRender()->DrawString(2, 26, 0xFF00FFFF, szOutputString);

        sprintf(szOutputString, "Audio: Cnt:%d, Mem:%d, Enabled: %d", g_pSoundMgr->GetUseAudioCnt(), g_pSoundMgr->GetUseAudioMem(), g_pSoundMgr->GetLastChannel());
        RtGetRender()->DrawString(250, 26, 0xFF00FFFF, szOutputString);
    }

    if (g_pSceneTool->m_bPathPointTool)
        m_pMainFrame->m_wndPathPointView.RenderTips(*RtGetRender());

    fununguard;
}

bool CEditorFrame::OnConsoleCommand(const char* szCommand)
{
    char seps[]   = " ,\t\n";
    char *token;

    token = strtok( (char*)szCommand, seps );
    if (token==NULL) return false;
    if (stricmp(token, "stat")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "scene")==0)
            {
                m_bShowStatScene = !m_bShowStatScene;
                return true;
            }else if (stricmp(token, "char")==0)
            {
                m_bShowStatChar = !m_bShowStatChar;
                return true;
            }else if (stricmp(token, "audio")==0)
            {
                m_bShowStatAudio = !m_bShowStatAudio;
                return true;
            }else if (stricmp(token, "allon")==0)
            {
                m_bShowStatScene  = true;
                m_bShowStatChar   = true;
                m_bShowStatAudio  = true;
                return true;
            }else if (stricmp(token, "alloff")==0)
            {
                m_bShowStatScene  = false;
                m_bShowStatChar   = false;
                m_bShowStatAudio  = false;
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command List: [scene], [char], [client], [allon], [alloff]");
        return true;
    }else if (stricmp(token, "render")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "scene")==0)
            {
                RtScene::m_bRenderScene = !RtScene::m_bRenderScene;
                return true;
            }else if (stricmp(token, "water")==0)
            {
                RtScene::m_bRenderSceneWater = !RtScene::m_bRenderSceneWater;
                return true;
            }/*
            else if (stricmp(token, "char")==0)
                        {
            				ACTOR()->m_bRenderActor = !ACTOR()->m_bRenderActor;
                            return true;
                        }else if (stricmp(token, "shadow")==0)
                        {
            				ACTOR()->m_bRenderShadow = !ACTOR()->m_bRenderShadow;
                            return true;
                        }*/
            else if (stricmp(token, "boundbox")==0)
            {
#if RTS_DRAW_HELPER
                RtScene::m_bRenderObjectBoundbox = !RtScene::m_bRenderObjectBoundbox;
#endif
                return true;
            }else if (stricmp(token, "CharBoundBox")==0)
            {
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command List: [scene/water/char/shadow/boundbox/CharBoundBox]");
        return true;
    }else
    {
		m_pConsole->AddConsoleString("  Command List: [stat], [render]");
    }
    return false;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

#include "gc_include.h"
#include "gc_cursor.h"
#include "gc_login.h"
#include "gc_camera.h"
#include "ui_form_skill_study.h"
#include "ui_form_msg.h"
#include "ui_form_textMsg.h"

#include <time.h>

DWORD CGameClientFrame::m_sTimerRender = 0;
DWORD CGameClientFrame::m_sTimerRun = 0;
DWORD CGameClientFrame::m_sTimerUI = 0;
DWORD CGameClientFrame::m_sTimerEvent = 0;

static float s_fClientSleepPreFrame = 0;
BOOL bRenderUI = TRUE;
static float	g_SecondsPerCycle = 0.0;

RT_IMPLEMENT_DYNCREATE(CGameClientFrame,CRtgAppFrame,NULL,"")

void CGameClientFrame::ResetTimer()
{
    guard;
    m_sTimerRender = 0;
    m_sTimerRun = 0;
    m_sTimerUI = 0;
    m_sTimerEvent = 0;
    m_bCanCaptureVideo = false;
	m_bMouseCapture = false;
    unguard;
}

CGameClientFrame::CGameClientFrame()
{
    guard;
    m_bNextGameFlow = -1;
    m_bUIInit = false;
    m_bShowStatNet = false;
    m_bShowStatAudio = false;
    m_bShowStatFile = false;
    m_bShowStatScene = false;
    m_bShowStatChar = false;
    m_bShowStatClient = false;
    m_pLogin = NULL;
    m_pWorld = NULL;
	m_pItemManager = NULL;
    m_pCurrentProcess = NULL;
    m_bChangeGameFlowNextFrame = FALSE;
    unguard;
}

CGameClientFrame::~CGameClientFrame()
{
    guard;
    unguard;
}

bool CGameClientFrame::OnDeviceInit()
{
    guard;
    GetDevice()->m_pCamera->SetProjParams(DegreeToRadian(60.0f), 4.f/3.f, 10.0f, 1000.0f);
    GetDevice()->m_pCamera->SetViewParams(RtgVertex3(100.0f, 10.0f, 10.0f), RtgVertex3(0.0f, 0.0f, 0.0f), RtgVertex3(0.0f, 0.0f, 1.0f));
    GetDevice()->m_pCamera->UpdateMatrix();
    GetDevice()->m_pCamera->SetPitch(0.f);
    GetDevice()->m_pCamera->SetYaw(0.f);
    GetDevice()->m_pCamera->SetRoll(DegreeToRadian(38.f));
    GetDevice()->m_pCamera->SetUp(150.f);
    GetDevice()->m_pCamera->MoveRight(-0.f);
    GetDevice()->m_pCamera->MoveForward(100.f);

    GetDevice()->m_bEnableViewCheck = FALSE;

    GetDevice()->GetTextMgr()->AddTextSearchPath("creature/texture");
    GetDevice()->GetTextMgr()->AddTextSearchPath("scene/texture");
    GetDevice()->GetTextMgr()->AddTextSearchPath("ui_texture");
    GetDevice()->GetTextMgr()->AddTextSearchPath(R(RES_LANGUAGE_TEXTURE_PATH));
    GetDevice()->GetTextMgr()->AddTextSearchPath("scene/texture/sm");

    //GetDevice()->m_Texture.SetTexturePath("creature\\texture", 2);
    //GetDevice()->m_Texture.SetTexturePath("scene\\texture", 1);
    //GetDevice()->m_Texture.SetTexturePath("ui_texture", 3);
    //GetDevice()->m_Texture.SetTexturePath(R(RES_LANGUAGE_TEXTURE_PATH), 4);
    //GetDevice()->m_Texture.SetTexturePath("scene\\texture\\sm", 5);
	GetDevice()->m_dwClearColor = 0xFF000000;

	// 注释 [3/16/2009 tooth.shi]
    //RtwXmlLoader::SetSearchPath(0, R(RES_LANGUAGE_UI_PATH));

    return true;
    unguard;
}
//lyymark 3.Frame.OnFrameInit 初始化 包括鼠标 UI初始化 应用Graph配置 
bool CGameClientFrame::OnFrameInit()
{

    GameInitCursor();

    // RS Load
    RS_Load();
	if (!SkillInit(R(RES_TABLE_SKILL), R(RES_TABLE_PASSIVE), R(RES_TABLE_EFFECT), R(RES_TABLE_MUTEX))) return false;
	LoadPKConfig(R(INI_PK));
	m_pItemManager = RT_NEW CItemManager;
	if (!m_pItemManager->Init())
	{
		ERR("道具初始化失败\n");
		return false;
	}
    UILayer::Initialize();
    m_bUIInit = TRUE;

    GetGameIni()->GetEntry("Graph", "Sleep", &s_fClientSleepPreFrame);
    /*
    long lFullScreen = FALSE, lFullScreenSizeX=800, lFullScreenSizeY=600, lColorDepth=32;
    GetGameIni()->GetEntry("Graph", "FullScreenSizeX", &lFullScreenSizeX);
    GetGameIni()->GetEntry("Graph", "FullScreenSizeY", &lFullScreenSizeY);
    GetGameIni()->GetEntry("Graph", "ColorDepth", &lColorDepth);
    GetGameIni()->GetEntry("Graph", "FullScreen", &lFullScreen);
    GetDevice()->SetViewWindowInfo(RTGVWM_TOPWINDOW, lFullScreenSizeX, lFullScreenSizeY, lColorDepth, 0);
    if (lFullScreen)
    {
        GetDevice()->SetViewMode(RTGVWM_TOPWINDOW, true);
    }
    */

// 注释 [3/25/2009 tooth.shi]
/*
    if (!OnEnterLogin())
    {
        return false;
    }

    GetDevice()->RenderScene();
*/
	return true;
    unguard;
}

void CGameClientFrame::OnFrameClose()
{
    guard;
    if (m_pLogin!=NULL)
    {
        CHECK(m_pWorld==NULL);
        OnLeaveLogin();
    }else if(m_pWorld!=NULL)
    {
        CHECK(m_pLogin==NULL);
        OnLeaveGame();
    }else
    {
        CHECKEX("flow fail");
    }

	SkillClean();
    RS_Clear();
	DEL_ONE(m_pItemManager);
    m_bUIInit = FALSE;
	
	UIFormTextMsg::Clear();
	UIFormMsg::Clear(true);
	UILayer::Clear();

#ifndef _DEBUG
    rtSleep(1.f); // 退出延时1秒
#endif
    unguard;
}
//lyymark 3.Frame.OnEnterLogin 进入login逻辑帧
bool CGameClientFrame::OnEnterLogin()
{
    guard;
    // 检查 m_pLogin 是否为空，以确保不重复初始化
    CHECK(m_pLogin==NULL);
    // 创建新的 GcLogin 实例并赋值给 m_pLogin
    m_pLogin = RT_NEW GcLogin(this);

    if (m_pLogin==NULL)
    {
        return false;
    }
    // 初始化登录对象
    if (!m_pLogin->InitOnce())
    {
        // 初始化失败，删除 m_pLogin 实例并设置当前过程为空
        DEL_ONE(m_pLogin);
        // 设置当前过程为空
        m_pCurrentProcess = NULL;
        return false;
    }
    // 设置GCLogin状态为正在加载
    m_pLogin->SetLoginState(GcLogin::GLS_LOADING);

    // 设置当前场景的世界为空
    ((GcScene*)g_pScene)->SetWorld(NULL);

    // 设置当前过程为 Login
    m_pCurrentProcess = m_pLogin;

    return true;
    unguard;
}

void CGameClientFrame::OnLeaveLogin()
{
    guard;
    m_pLogin->ClearOnce();
    DEL_ONE(m_pLogin);
    m_pCurrentProcess = NULL;
    unguard;
}
//lyymark 3.Frame.OnEnterLogin 进入开始游戏逻辑帧
bool CGameClientFrame::OnEnterGame()
{
    guard;

    CHECK(m_pWorld==NULL);
    m_pWorld = RT_NEW GcWorld(this);
    if (m_pWorld==NULL)
    {
        return false;
    }

    if (!m_pWorld->InitOnce())
    {
        DEL_ONE(m_pWorld);
        m_pCurrentProcess = NULL;
        return false;
    }
	m_pWorld->m_pItemManager = m_pItemManager;

    ((GcScene*)g_pScene)->SetWorld(m_pWorld);
    m_pCurrentProcess = m_pWorld;

    m_pWorld->EnterGame();

    // 重新连接Region Server
    m_pWorld->EnterRegionServer();

    return true;
    unguard;
}

void CGameClientFrame::OnLeaveGame()
{
    guard;
    m_pWorld->LeaveRegion();
    m_pWorld->LeaveGame();
    m_pWorld->ClearOnce();
    DEL_ONE(m_pWorld);
    m_pCurrentProcess = NULL;
    unguard;
}

bool CGameClientFrame::OnRestoreDevice()				//恢复驱动
{
     guard;
// 	mFont.RestoreDevice();
// 	g_pDevice2D->RestoreDevice();
// 	g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);
	g_workspace.OnRestoreDevice();
	return true;
    unguard;
}

bool CGameClientFrame::OnInvalidateDevice()				//驱动失效
{
    guard;
	g_workspace.OnInvalidateDevice();
// 	mFont.InvalidateDevice();
// 	g_pDevice2D->InvalidateDevice();
	return true;
    unguard;
}

void CGameClientFrame::OnMouseMove( int x, int y, int increaseX, int increaseY)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseMove(x, y);
	if(!g_workspace.getMouseHover())
	{
        if (m_pCurrentProcess)
        {
            m_pCurrentProcess->OnMouseMove(0,x,y,increaseX,increaseY);
        }
	}else if(m_pCurrentProcess == GetWorld() && GetWorld())
	{
		GetWorld()->OnMouseHoverMove(0,x,y,increaseX,increaseY);
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseWheel(/*int iButton,*/ long vDelta,int x,int y)
{
    guard;
	rtClock(m_sTimerEvent);
	if(!g_workspace.MouseWheel(vDelta,x,y))
	{
        if (m_pCurrentProcess)
        {
            m_pCurrentProcess->OnMouseWheel(buttonMiddle, vDelta);
        }
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseLDown(/*int iButton, */int x, int y)
{
    guard;

    if (g_WorldMapRenderer.GetRenderFlag())
    {
        g_WorldMapRenderer.SetRenderFlag(false);
        return;
    }

    rtClock(m_sTimerEvent);
	if(!g_workspace.MouseDown(buttonLeft))
	{
        if (m_pCurrentProcess)
        {
            PushCallStack(csn_CGameClientFrame_OnMouseLDown);
            m_pCurrentProcess->OnMouseLDown(buttonLeft,x,y);
            PopCallStack();
        }
	}
    rtUnclock(m_sTimerEvent);

    /*
    if (m_bMouseCapture)
    {
        CancelMouseCapture();
    }
    */

    return;
    unguard;
}

void CGameClientFrame::OnMouseLUp(/*int iButton, */int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseUp(buttonLeft);
	//m_pCurrentProcess->OnMouseLUp(buttonLeft,x,y);
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseLDrag(/*int iButton,*/ int x, int y, int increaseX, int increaseY)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseMove(x, y);
	if(GetWorld())
	{
		GetWorld()->OnMouseHoverMove(0,x,y,increaseX,increaseY);
		GetWorld()->OnMouseDragMove(0,x,y,increaseX,increaseY);
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseLDClick(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	if(!g_workspace.MouseDClick(buttonLeft))
	{
        if (m_pCurrentProcess)
        {
            m_pCurrentProcess->OnMouseLDClick(buttonLeft,x,y);
        }
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseMDown(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);

    /*if (m_bMouseCapture)
    {
        CancelMouseCapture();
        return;
    }*/

    g_workspace.MouseDown(buttonMiddle);

    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseMUp(/*int iButton, */int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseUp(buttonMiddle);
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseMDrag(/*int iButton,*/ int x, int y, int increaseX, int increaseY)
{
    guard;
    rtClock(m_sTimerEvent);	
	if (m_pCurrentProcess)
	{
		m_pCurrentProcess->OnMouseMDrag(0,x,y,increaseX,increaseY);
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseMDClick(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseDClick(buttonMiddle);
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseRDown(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);

    /*if (m_bMouseCapture)
    {
        CancelMouseCapture();
        return;
    }*/

	if(!g_workspace.MouseDown(buttonRight))
	{
        if (m_pCurrentProcess)
        {
            m_pCurrentProcess->OnMouseRDown(buttonRight,x,y);
        }
	}
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseRUp(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseUp(buttonRight);
	if(g_layerMain && g_layerMain->m_formMiddleMap)
		g_layerMain->SetMiddleMapMouseState(false);
	rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseRDrag(/*int iButton,*/ int x, int y, int increaseX, int increaseY)
{
    guard;
    rtClock(m_sTimerEvent);
	//RtwWidgetManager::s_manager.MouseRDrag(iButton, x, y);
    if (m_pCurrentProcess)
    {
        m_pCurrentProcess->OnMouseRDrag(0,x,y,increaseX,increaseY);
    }
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnMouseRDClick(/*int iButton,*/ int x, int y)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.MouseDClick(buttonRight);
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

bool CGameClientFrame::SetMouseCapture(int iID, enum GameCursor eCursor, void* pData, const char* szMsg)
{
    //if (m_bMouseCapture==true) return false;
    m_bMouseCapture     = true;
    m_iMouseCaptureID   = iID;
    m_pMouseCaptureData = pData;
    m_szMouseCaptureMsg = szMsg;
	if (iID != 2)
	{
	    GameSetCursor(eCursor);
	}

    return true;
}

void CGameClientFrame::CancelMouseCapture()
{
    if (m_bMouseCapture)
    {
		m_iMouseCaptureID = -1;
        m_bMouseCapture = false;
        GameSetCursor(GAME_CURSOR_NORMAL);
    }
}

void CGameClientFrame::OnKeyDown(/*int iButton,*/ int iKey, bool bAltDown)
{
    guard;

    if (g_WorldMapRenderer.GetRenderFlag())
    {
        g_WorldMapRenderer.SetRenderFlag(false);
        return;
    }

    rtClock(m_sTimerEvent);
    if (m_pCurrentProcess)
    {
        m_pCurrentProcess->OnKeyDown(iKey,bAltDown);//暂时先拿出来
    }
	g_workspace.KeyDown(iKey);
    rtUnclock(m_sTimerEvent);
	return;

    unguard;
}
// lyymark 3.Frame.GameScreenSnapShot 截屏
void GameScreenSnapShot()
{
    guard;
    static bool bFirst = true;
    static int iCurCnt = 0;

	//change by yz 2010-6-11
    //static char szFilename[64] = "capture\\yyyymmdd_cnt.jpg";
	S_MEMDEF(szFilename, 64)
	S_MEMPROTECTOR(szFilename, 64, bDefined)
	//rt2_strncpy(szFilename, "capture\\yyyymmdd_cnt.jpg", 64);
	//end yz

    static char* pNameFormat = "%03d.jpg";
    if (bFirst)
    {
        bFirst = false;
        time_t t;
        time(&t);
        struct tm* ptm = localtime(&t);
        rt2_snprintf(szFilename, 56, "capture\\%04d%02d%02d_", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);

        if (!RtCoreFile().FileExist("capture"))
        {
            RtCoreFile().MakeDirectory("capture");
        }

        iCurCnt = 0;
        do
        {
            iCurCnt ++;
            rt2_snprintf(szFilename+17, 47, pNameFormat, iCurCnt);
        }while (RtCoreFile().FileExist(szFilename));
    }

    rt2_snprintf(szFilename+17, 47, pNameFormat, iCurCnt++);
    GetDevice()->SaveRenderTargetToFile(szFilename);
    unguard;
}

#if DO_CONSOLE
static bool bCaptureVideo = false;
static int iVideoFrameCount = 0;
//static char szVideoDir[20];
G_MEMDEF(szVideoDir, 20)

void GameCaptureVideo()
{
    guard;
	G_MEMPROTECTOR(szVideoDir, 20)   //add by yz

    char szFilename[50];
    rt2_sprintf(szFilename, "vedio/%s/v%05d.jpg", szVideoDir, iVideoFrameCount++);
    GetDevice()->SaveRenderTargetToFile(szFilename);
    unguard;
}
#endif

void CGameClientFrame::OnKeyUp( int iKey, bool bAltDown)
{
    guard;
    rtClock(m_sTimerEvent);
    if (iKey==VK_F11)
    {
        GameScreenSnapShot();
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("screen_cap.wav", false, 0, pos);
	}
#if DO_CONSOLE
    /*if (m_bCanCaptureVideo)
    {
        if (iKey==VK_F11)
        {
            bCaptureVideo = !bCaptureVideo;
            if (bCaptureVideo)
            {
                if (!RtCoreFile().FileExist("vedio"))
                {
                    RtCoreFile().MakeDirectory("vedio");
                }

                time_t t;
                time(&t);
                struct tm* ptm = localtime(&t);
                sprintf(szVideoDir, "%04d%02d%02d_%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

                char szFilename[50];
                sprintf(szFilename, "vedio/%s", szVideoDir);
                if (!RtCoreFile().FileExist(szFilename))
                {
                    RtCoreFile().MakeDirectory(szFilename);
                }
                iVideoFrameCount = 0;
            }
        }
    }*/
#endif
	g_workspace.KeyUp(iKey);
    if (m_pCurrentProcess)
    {
        m_pCurrentProcess->OnKeyUp(iKey,bAltDown);
    }
    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

void CGameClientFrame::OnKeyChar(/*int iButton,*/ int iChar, bool bAltDown)
{
    guard;
    rtClock(m_sTimerEvent);
	g_workspace.KeyChar(iChar);
	if (GetWorld()!=NULL)
	{
		GetWorld()->OnKeyChar(iChar);
	}

    rtUnclock(m_sTimerEvent);
	return;
    unguard;
}

bool VaildCamera()
{
    guard;
    return (GetDevice()->m_pCamera!=NULL);
    unguard;
}

//lyymark 3.Frame.OnFrameMove逻辑帧
void CGameClientFrame::OnFrameMove(float fDifTime)
{
    guard;
    rtClock(m_sTimerRun);
    CHECK(GetDevice()->m_pCamera!=NULL);
    if (m_bChangeGameFlowNextFrame)
    {
        m_bChangeGameFlowNextFrame = FALSE;
        if (m_bNextGameFlow)
        {
			this->OnLeaveLogin();
            this->OnEnterGame();
        }else
        {
            this->OnLeaveGame();
            this->OnEnterLogin();
        }
    }

    if (!VaildCamera())
    {
        return;
    }

    GetTimer()->Run(GetDevice()->GetAppTime());
    if (m_pCurrentProcess)
    {
        m_pCurrentProcess->OnRun(fDifTime);
    }
    rtUnclock(m_sTimerRun);
    unguard;
}

bool VaildCamera2()
{
    guard;
    return (GetDevice()->m_pCamera!=NULL);
    unguard;
}

void CGameClientFrame::OnBeginRender()
{
    guard;
    rtClock(m_sTimerRender);
    if (m_pCurrentProcess==NULL) return;
    m_pCurrentProcess->OnBeginRender();
    if (s_fClientSleepPreFrame>0.f)
    {
        rtSleep(s_fClientSleepPreFrame);
    }
    unguard;
}

void CGameClientFrame::OnEndRender()
{
    guard;
    if (m_pCurrentProcess==NULL) return;
    m_pCurrentProcess->OnEndRender();
	CRtgAppFrame::OnEndRender();
	rtUnclock(m_sTimerRender);

    if (!VaildCamera2())
    {
        return;
    }

    {
        DWORD dwStrColor = 0xFFFF0000;
        //static char szOutputString[1024];
		S_MEMDEF(szOutputString, 1024)
		S_MEMPROTECTOR(szOutputString, 1024, bDefined)

      /*  if (m_bShowStatChar)
        {
            RtcActorPerformance *p = CRT_ActorInstance::GetActorPerformance();
            sprintf(szOutputString,
                "------------------------\n"
                "Render:     %2.3f(ms)\n"
                "CoreRender: %2.3f(ms)\n"
                "Phy:        %2.3f(ms)\n"
                "Bone:       %2.3f(ms)\n"
                "Skin:       %2.3f(ms)\n"
                "Shadow:     %2.3f(ms)\n"
                "Actor:      %d\n"
                "VaVtx:      %d\n"
                "SkelVtx:    %d\n"
                "SkelBone:   %d\n",
                p->RenderTimer      * g_SecondsPerCycle * 1000,
                p->CoreRenderTimer  * g_SecondsPerCycle * 1000,
                p->PhyTimer         * g_SecondsPerCycle * 1000,
                p->BoneTimer        * g_SecondsPerCycle * 1000,
                p->SkinTimer        * g_SecondsPerCycle * 1000,
                p->ShadowTimer      * g_SecondsPerCycle * 1000,
                p->ActorNum,
                p->VaVtxNum,
                p->SkelVtxNum,
                p->SkelBoneNum
                );
            GetDevice()->DrawString(5, 155, 0xFF00FF00, szOutputString);
            CRT_ActorInstance::ResetAcotrPerformance();
        }*/

        //if (m_bShowStatScene)
        //{
        //    sprintf(szOutputString,
        //        "------------------------\n"
        //        "SceneTotal:   %2.3f(ms)\n"
        //        "PreTile:      %2.3f(ms)\n"
        //        "RenderTile:   %2.3f(ms) [cnt %d]\n"
        //        "RenderObject: %2.3f(ms) [cnt %d]\n"
        //        "RenderWater:  %2.3f(ms)\n"
        //        "UseTexture:   %d",
        //        RtScene::m_sTimerTotalRender   * g_SecondsPerCycle * 1000,
        //        RtScene::m_sTimerPreRenderTile * g_SecondsPerCycle * 1000,
        //        RtScene::m_sTimerRenderTile    * g_SecondsPerCycle * 1000, RtScene::m_sCountRenderGrid,
        //        RtScene::m_sTimerRenderObject  * g_SecondsPerCycle * 1000, RtScene::m_sCountRenderObject,
        //        RtScene::m_sTimerRenderWater   * g_SecondsPerCycle * 1000,
        //        RtScene::m_sCountUseTexture
        //        );
        //    GetDevice()->DrawString(5, 330, 0xFF00FF00, szOutputString);
        //    RtScene::ResetTimer();
        //}

        if(m_bShowStatFile)
        {
            NetSession* pSession = GetNet();
            if (pSession)
            {
                rt2_sprintf(szOutputString,
                    "------------------------\n"
                    "AvgFluxIn:        %d\n"
                    "AvgFluxOut:       %d\n"
                    "BytesIn:          %d\n"
                    "BytesOut:         %d\n"
					"SendComRate:      %.2f\n"
					"RecvComRate:      %.2f\n",
                    pSession->GetAvgFluxIn(), pSession->GetAvgFluxOut(),
                    pSession->GetBytesIn(), pSession->GetBytesOut(),
					pSession->GetSendCompressRate(),pSession->GetRecvCompressRate());
            }
			else
            {
                rt2_sprintf(szOutputString,
                    "------------------------\n"
                    "Disconnection\n");
            }
            GetDevice()->DrawString(200, 40, 0xFF00FF00, szOutputString);
        }

        if (m_bShowStatFile)
        {
            rt2_sprintf(szOutputString,
                "------------------------\n"
                "ReadByte:     %s\n"
                "WriteByte:    %s\n",
                rtEncodeIntegerNumber(szOutputString+200, 0, RtCoreFile().TotalReadByte()),
                rtEncodeIntegerNumber(szOutputString+300, 0, RtCoreFile().TotalWriteByte())
                );
            GetDevice()->DrawString(5, 442, 0xFF00FF00, szOutputString);
        }

        if (m_bShowStatAudio)
        {
            rt2_sprintf(szOutputString,
                "------------------------\n"
                "Audio Cnt:    %d\n"
                "Memory:       %d\n"
                "Enable:       %d",
                g_pSoundMgr->GetUseAudioCnt(),
                g_pSoundMgr->GetUseAudioMem(),
                g_pSoundMgr->GetLastChannel());
            GetDevice()->DrawString(5, 490, 0xFF00FF00, szOutputString);
        }

        if (m_bShowStatClient)
        {
// 				// 注释 [3/16/2009 tooth.shi]
//             sprintf(szOutputString,
//                 "------------------------\n"
//                 "ClientRender: %2.3f(ms)\n"
//                 "ClientRun:    %2.3f(ms)\n"
//                 "ClientUI:     %2.3f(ms)\n"
//                 "ClientEvent:  %2.3f(ms)\n"
// 				"ImageTotal:   %2.3f(ms) / %d\n"
// 				"ImageBorder:  %2.3f(ms) / %d\n"
// 				"ImageStretch: %2.3f(ms) / %d\n"
// 				"Font:         %2.3f(ms) / %d/%d\n"
// 				"IndexChar:    %2.3f(ms)\n"
// 				"DrawBuffer:   %2.3f(ms) / %d\n"
// 				"BufferToDevice: %2.3f(ms)\n",
//                 m_sTimerRender * g_SecondsPerCycle * 1000,
//                 m_sTimerRun    * g_SecondsPerCycle * 1000,
//                 m_sTimerUI     * g_SecondsPerCycle * 1000,
//                 m_sTimerEvent  * g_SecondsPerCycle * 1000,
// 
// 				g_pDevice2D->m_measureImageTotal	* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_measureImageTotalNumber,
// 
// 				g_pDevice2D->m_measureBorderImage	* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_measureBorderImageNumber,
// 
// 				g_pDevice2D->m_measureStretchImage	* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_measureStretchImageNumber,
// 
// 				g_pDevice2D->m_measureString		* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_measureStringNumber, g_pDevice2D->m_measureCharNumber,
// 
// 				g_pDevice2D->m_measureIndexString	* g_SecondsPerCycle * 1000,
// 
// 				g_pDevice2D->m_measureToBuffer		* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_meausreToBufferNumber,
// 
// 				g_pDevice2D->m_measureToDevice		* g_SecondsPerCycle * 1000,
// 				g_pDevice2D->m_measureToDeviceNumber
//                 );
// 			g_pDevice2D->ResetMeasure();
            GetDevice()->DrawString(550, 20, 0xFF00FF00, szOutputString);
            CGameClientFrame::ResetTimer();
        }

        /*
        if (GetWorld()->m_pPlayer)
        {
            RtgMatrix16* pM16 = GetWorld()->m_pPlayer->Matrix();
            sprintf(szOutputString,
                "------------------------\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n",
                pM16->_00, pM16->_01, pM16->_02, pM16->_03,
                pM16->_10, pM16->_11, pM16->_12, pM16->_13,
                pM16->_20, pM16->_21, pM16->_22, pM16->_23,
                pM16->_30, pM16->_31, pM16->_32, pM16->_33);
            GetDevice()->DrawString(550, 20, 0xFF00FF00, szOutputString);

            RtgMatrix16 m16 = *(GetWorld()->m_pPlayer->Matrix());
            BYTE cDir = rtgEncodeUprightDirection(m16);
            rtgDecodeUprightDirection(m16, cDir);
            sprintf(szOutputString,
                "--- %d --------------------\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n"
                "%2.3f %2.3f %2.3f %2.3f\n",
                cDir,
                m16._00, m16._01, m16._02, m16._03,
                m16._10, m16._11, m16._12, m16._13,
                m16._20, m16._21, m16._22, m16._23,
                m16._30, m16._31, m16._32, m16._33);
            GetDevice()->DrawString(550, 100, 0xFF00FF00, szOutputString);
        }
        */

        //g_pDevice2D->DrawFontTest(10, 10, 250, 250);
    }
    unguard;
}

void CGameClientFrame::OnRender()
{
    guard;
    // 如果当前进程为空，则返回，不进行渲染
    if (m_pCurrentProcess==NULL) return;

    // 获取设备对象的经过时间，并存储在 fElapsedTime 中
    float fElapsedTime = GetDevice()->GetElapsedTime();

    if (g_WorldMapRenderer.GetRenderFlag())
    { // 如果世界地图渲染器的渲染标志为真，则调用其 Render 方法进行渲染
        g_WorldMapRenderer.Render();
        return;
    }
    // 获取设备对象的摄像机，并调用其 Render 方法进行渲染，传递经过时间
    GetDevice()->m_pCamera->Render(fElapsedTime);

    // 调用当前进程的 OnRender 方法进行渲染，传递经过时间
    m_pCurrentProcess->OnRender(fElapsedTime);

    // 结束设备对象的后处理过程
    GetDevice()->EndPostProcess();

    // 开始计时，用于 UI 渲染时间测量
    rtClock(m_sTimerUI);

    // 如果定义了 DO_CONSOLE 并且正在捕获视频，则调用 GameCaptureVideo 方法进行视频捕捉
#if DO_CONSOLE
    if (bCaptureVideo)
    {
        GameCaptureVideo();
    }
#endif
    // 停止计时，结束 UI 渲染时间测量
    rtUnclock(m_sTimerUI);

    unguard;
}



void CGameClientFrame::OnRender2D()
{
    //extern double g_RenderUiUseTime;
    if (g_LoadingMapRenderer.GetRenderFlag())
    {
        g_LoadingMapRenderer.Render();
        return;
    }


    if (bRenderUI)
    {
        RtGetPref()->Ui_Render = rtMilliseconds();
        g_workspace.Render();
        RtGetPref()->Ui_Render = rtMilliseconds() - RtGetPref()->Ui_Render;
    }

}

#if DO_CONSOLE

bool CGameClientFrame::OnConsoleCommand(const char* szCommand)
{
    guard;
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
            }else if (stricmp(token, "client")==0)
            {
                m_bShowStatClient = !m_bShowStatClient;
                return true;
            }else if (stricmp(token, "file")==0)
            {
                m_bShowStatFile = !m_bShowStatFile;
                return true;
            }else if (stricmp(token, "audio")==0)
            {
                m_bShowStatAudio = !m_bShowStatAudio;
                return true;
            }else if (stricmp(token, "net")==0)
            {
                m_bShowStatNet = !m_bShowStatNet;
                return true;
            }else if (stricmp(token, "allon")==0)
            {
                m_bShowStatScene  = true;
                m_bShowStatChar   = true;
                m_bShowStatClient = true;
                m_bShowStatFile   = true;
                m_bShowStatAudio  = true;
                m_bShowStatNet    = true;
                return true;
            }else if (stricmp(token, "alloff")==0)
            {
                m_bShowStatScene  = false;
                m_bShowStatChar   = false;
                m_bShowStatClient = false;
                m_bShowStatFile   = false;
                m_bShowStatAudio  = false;
                m_bShowStatNet    = false;
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
            }else if (stricmp(token, "char")==0)
            {
                CRT_ActorInstance::m_bGlobalRenderActor = !CRT_ActorInstance::m_bGlobalRenderActor;
                return true;
            }else if (stricmp(token, "shadow")==0)
            {
                CRT_ActorInstance::m_bGlobalRenderShadow = !CRT_ActorInstance::m_bGlobalRenderShadow;
                return true;
            }else if (stricmp(token, "ui")==0)
            {
                bRenderUI = !bRenderUI;
                return true;
            }else if (stricmp(token, "boundbox")==0)
            {
#if RTS_DRAW_HELPER
                RtScene::m_bRenderObjectBoundbox = !RtScene::m_bRenderObjectBoundbox;
#endif
                return true;
            }else if (stricmp(token, "CharBoundBox")==0)
            {
                GcBaseActor::m_bDrawBoundBox = !GcBaseActor::m_bDrawBoundBox;
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command List: [scene/water/char/shadow/ui/boundbox/charboundbox]");
        return true;
    }else if (stricmp(token, "exit")==0)
    {
        PostQuitMessage(0);
        return true;
    }else if (stricmp(token, "ui") == 0)
	{
		size_t length = strlen(token) + 1;
		token = (char*)szCommand + length;
		if (*token)
		{
//			RTW_WIDGET_MANAGER()->ExecCommand(token);
			return true;
		}
        m_pConsole->AddConsoleString("  Command Format: > ui string");
        return true;
	}else if (stricmp(token, "show") == 0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "SkillStudy")==0)
            {
                g_layerMain->m_fromSkillStudy->Show();
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command List: show [SkillStudy/CharBoundBox]");
        return true;
    }else if (stricmp(token, "fullscreen") == 0)
    {
        if (GetDevice()->IsWindowed())
        {
            //GetDevice()->FullscreenToggle();
        }
        return true;
    }else if (stricmp(token, "window") == 0)
    {
        if (!GetDevice()->IsWindowed())
        {
            //GetDevice()->FullscreenToggle();
        }
        return true;
    }else if (stricmp(token, "music") == 0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "volume")==0)
            {
                token = strtok( NULL, seps );
                if (token)  g_pBackMusic->SetVolume(atof(token));
                else        m_pConsole->AddConsoleString("  Command Format: > music volume number");
                return true;
            }else if (stricmp(token, "load")==0)
            {
                token = strtok( NULL, seps );
                if (token)  g_pBackMusic->Play(token);
                else        m_pConsole->AddConsoleString("  Command Format: > music load filename");
                return true;
            }else if (stricmp(token, "quiet")==0)
            {
                token = strtok( NULL, seps );
                if (token)
                {
                    if (stricmp(token, "true")==0)          { g_pBackMusic->Quiet(true); return true;}
                    else if (stricmp(token, "false")==0)    { g_pBackMusic->Quiet(false); return true;}
                }
                m_pConsole->AddConsoleString("  Command Format: > music quiet [true/false]");
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command List: [volume], [load], [quiet]");
        return true;
    }else if (stricmp(token, "texture") == 0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            GetDevice()->GetTextMgr()->SetTextureQuality(atoi(token));
            return true;
        }
        m_pConsole->AddConsoleString("  Command Format: texture number");
        return true;
    }else if (stricmp(token, "pos") == 0)
    {
        if (GetWorld()->m_pPlayer)
        {
            int iPx, iPy;
            char szPosition[200];
            const RtgMatrix16* pM16 = GetWorld()->m_pPlayer->GetMatrix();
            g_pScene->GetTerrainByPosFast(pM16->_30, pM16->_31, iPx, iPy);
            rt2_sprintf(szPosition, "  Position: (%.2f, %.2f, %.2f) [%d, %d], Dir: [%d]",
                pM16->_30, pM16->_31, pM16->_32, iPx, iPy, rtgEncodeUprightDirection(*pM16));
            m_pConsole->AddConsoleString(szPosition);
            return true;
        }
        m_pConsole->AddConsoleString("  Must in game.");
        return true;
    }
	//------------add start by tony 05.07.19------------------------//
	else if(stricmp(token, "addnpc") == 0)
	{
		FILE*	fp = NULL;

		if((fp = fopen("c:\\guarder_temp.txt", "a")) == NULL)
		{
			return false;
		}

		std::string str_pos = "";

		if(GetWorld()->m_pPlayer)
		{
			const RtgMatrix16* pM16 = GetWorld()->m_pPlayer->GetMatrix();

			str_pos = std::string(rtFormatNumber((long)pM16->_30));
			str_pos += ";";
			str_pos += std::string(rtFormatNumber((long)pM16->_31));
			str_pos += "	";
			str_pos += std::string(rtFormatNumber((int)rtgEncodeUprightDirection(*pM16)));
			str_pos += "\n";
		}

		fputs((char*)str_pos.c_str(), fp);

		fclose(fp);

		return true;
	}
	//------------add end   by tony 05.07.19------------------------//
	else if(stricmp(token,"actor")==0)
	{
		token = strtok( NULL, seps );
		if (token)
		{
			if(stricmp(token,"on")==0)
			{
				GcActor::Debug(true);
				return true;
			}else if(stricmp(token,"off")==0)
			{
				GcActor::Debug(false);
				return true;
			}
		}
		m_pConsole->AddConsoleString("  Command Format: actor [on/off]");
		return true;
    }else if(stricmp(token,"fog")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if(stricmp(token,"on")==0)
            {
                GetDevice()->SetFogEnable(TRUE);
                return true;
            }else if(stricmp(token,"off")==0)
            {
                GetDevice()->SetFogEnable(FALSE);
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command Format: fog [on/off]");
        return true;
    }else if(stricmp(token,"camera_mode")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if(stricmp(token,"lock")==0)
            {
                GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_LOCK);
                return true;
            }else if(stricmp(token,"free")==0)
            {
                GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_FREE);
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command Format: camera_mode [lock/free]");
        return true;
    }else if(stricmp(token,"block")==0)
	{
		token =strtok(NULL,seps);
		if(token)
		{
			if(stricmp(token,"on")==0)
			{
				GetWorld()->DebugBlockTerrain(true);
				return true;
			}
			else if(stricmp(token,"off")==0)
			{
				GetWorld()->DebugBlockTerrain(false);
				return true;
			}
		}
		m_pConsole->AddConsoleString("  Command Format:block [on/off]");
		return true;
    }else if (stricmp(token,"device")==0)
    {
        token =strtok(NULL,seps);
        if(token)
        {
            if(stricmp(token,"info")==0)
            {
                rt2_sprintf(rtStaticString1024(), "  Device: %s", GetDevice()->GetDeviceDesc());
                m_pConsole->AddConsoleString(rtStaticString1024());
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command Format:device [info]");
        return true;
    }else if (stricmp(token,"object")==0)
    {
        /*
        std::map<string, int> mapObj;
        std::map<string, int>::iterator it;
        token =strtok(NULL,seps);
        if(token)
        {
            if(stricmp(token,"list")==0)
            {
                RtsObject** plistObj = NULL;
                int objNum = 0;

                g_pScene->GetRenderObject(plistObj, objNum);

                for (int i = 0; i < objNum; ++i)
                {
                    RtsObject* pObject = plistObj[i];

                    if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                    {
                        mapObj[((RtsActor*)pObject)->GetActor()->GetCore()->m_poTag] ++;
                    }

                }



                //RtsObject* pObject = g_pScene->m_pRenderObjHead;
                //while (pObject)
                //{
                //    if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                //    {
                //        mapObj[((RtsActor*)pObject)->GetActor()->m_core->m_poTag] ++;
                //    }
                //    pObject = pObject->m_pPrevRenderObject;
                //}
                int iIdx = 0;
                for (it=mapObj.begin(); it!=mapObj.end(); it++)
                {
                    sprintf(rtStaticString1024(), "%2d [%2d] %s", ++iIdx, (*it).second, (*it).first.c_str());
                    m_pConsole->AddConsoleString(rtStaticString1024());
                }
                return true;
            }
            m_pConsole->AddConsoleString("  Command Format:object [list]");

            
            return true;
        }
        */
    }else if (stricmp(token,"move")==0)
    {
        int x, y;
        token =strtok(NULL,seps);
        if(token)
        {
            x = atoi(token);
            token =strtok(NULL,seps);
            if(token)
            {
				y = atoi(token);
				//GetWorld()->m_ActorManager.Clear();
				//GetWorld()->m_pPlayer->mBaseActor.RemoveSelf();
                GetPlayer()->MoveToNow(x, y);
				
                return true;
            }
        }
        m_pConsole->AddConsoleString("  Command Format:move x y");
        return true;
    }else if (stricmp(token,"print")==0)
    {
        token =strtok(NULL,seps);
        if(token)
        {
            if (stricmp(token,"actor")==0)
            {
                if (GetWorld()->m_pPlayer)
                {
                    // done
                    //CRT_ActorInstance::PrintValidObject("DumpActor.txt");
                }
                return true;
            }else if (stricmp(token,"renderobj")==0)
            {/*
                RtsObject** plistObj = NULL;
                int objNum = 0;

                g_pScene->GetRenderObject(plistObj, objNum);

                for (int i = 0; i < objNum; ++i)
                {
                    RtsObject* pObject = plistObj[i];
                    if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                    {
                        LOG2("RenderActor: %d, %s\n", i, ((RtsActor*)pObject)->GetActor()->GetCore()->m_poTag.c_str());
                    }

                }*/



            /*    RtsObject* pObject = g_pScene->m_pRenderObjHead;
                int iIdx=1;
                while (pObject)
                {
                    if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                    {
                        LOG2("RenderActor: %d, %s\n", iIdx++, ((RtsActor*)pObject)->GetActor()->m_core->m_poTag.c_str());
                    }
                    pObject = pObject->m_pPrevRenderObject;
                }*/
                return true;
            }else if (stricmp(token,"effect")==0)
            {
                stlforeach(std::list<SSkillEffect*>, itEffect, GetPlayer()->m_Skill.m_listSkillEffect)
                {
                    m_pConsole->AddConsoleString((*itEffect)->szName);
                }
            }
        }
        m_pConsole->AddConsoleString("  Command Format:print [actor/renderobj/effect]");
        return true;
    }else
    {
        m_pConsole->AddConsoleString("  Command List: [stat], [render], [py], [exit], [music], [fullscreen], [window] ");
        m_pConsole->AddConsoleString("    [texture], [pos], [actor], [block], [device], [object], [effect], [print]");
        m_pConsole->AddConsoleString("    [fog], [camera_mode]");
    }
    return false;
    unguard;
}

void CGameClientFrame::OnKillFocus()
{
	if (GetWorld() != NULL)
	{
		GetWorld()->OnKillFocus();
	}
}

void CGameClientFrame::OnSetFocus()
{
	if (GetWorld() != NULL)
	{
		GetWorld()->OnSetFocus();
	}
}

CSceneMusicApp::CSceneMusicApp()
{
	if (!rtMusicInit())
	{
		CHECKEX("初始化背景音乐失败");
	}
	g_pBackMusic->SetMusicPath("music");
	m_nLoopTime = 0;
	m_lNextLoopTime = 0;
	CM_MEMPROTECTOR(m_szMusicName, 40)
}
CSceneMusicApp::~CSceneMusicApp()
{
	CM_MEMUNPROTECTOR(m_szMusicName)
}
void CSceneMusicApp::Play(const char* filename,bool loop,int looptime)
{
	//LockThread();
	m_bloop = false;
	rt2_sprintf(m_szMusicName,filename);
	m_bChangeMusic = true;
	m_nLoopTime = looptime;
	//if ( m_nLoopTime != 0 )
		m_bloop = loop;
	//UnlockThread();
}

void CSceneMusicApp::Stop()
{
	m_szMusicName[0] = 0;
	g_pBackMusic->Stop();
}

int CSceneMusicApp::OnRun()
{
	while (true)
	{
		if (m_bChangeMusic) 
		{
			if ( g_pBackMusic->IsPlaying() )
				g_pBackMusic->Stop();
			g_pBackMusic->Play(m_szMusicName,m_bloop);
			m_bChangeMusic = false;
		}
		if ( !m_bloop && m_nLoopTime && !m_lNextLoopTime && !g_pBackMusic->IsPlaying() )
		{
			m_lNextLoopTime = rtGetMilliseconds() + m_nLoopTime * 1000;
		}
		if( m_lNextLoopTime != 0 && m_lNextLoopTime <= rtGetMilliseconds() )
		{
			m_lNextLoopTime = 0;
			Play(m_szMusicName,m_bloop,m_nLoopTime);
		}
		Sleep(1);
	}


	return 0;
}

#endif

/*----------------------------------------------------------------------------
The End.
----------------------------------------------------------------------------*/

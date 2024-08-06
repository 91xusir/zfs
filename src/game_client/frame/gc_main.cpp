//#include <io.h>
#include "gc_include.h"            // 包含游戏核心的公共头文件
#include "gc_camera.h"             // 包含游戏中摄像机相关的头文件
//#include "gc_log_msg.h"            // 包含游戏日志消息相关的头文件
#include "filepack/memory_file.h"  // 包含文件打包和内存文件相关的头文件
#include "gc_login.h"              // 包含游戏登录相关的头文件
//#include "resource.h"              // 包含资源相关的头文件
//#include <CrashReport/Export.h>    // 包含崩溃报告导出功能相关的头文件

// 全局场景指针
extern RtScene* g_pScene;

// 全局变量：
static HINSTANCE g_hGCInst = 0;  // 应用程序实例句柄
//static TCHAR   g_szGCTitle[100]        = "FSO Client"; // 窗口标题
//static TCHAR   g_szGCWindowClass[100] = "RT3D_FSO_CLIENT"; // 窗口类名
G_MEMDEF(g_szGCTitle, 100)        // 定义一个100字符长度的全局标题字符串
G_MEMDEF(g_szGCWindowClass, 100)  // 定义一个100字符长度的全局窗口类名字符串

static CRtgAppFrame* g_pGameClientFrame = 0;  // 游戏客户端框架指针
static RtIni* g_iniConfig;  // 配置文件指针，指向 "game.ini"，此指针不会被删除

bool g_bLogEnable = false;  // 日志功能是否启用的标志

// 由yz于2010-6-11添加：声明内存保护器注册函数
void RegisterMemProtector();
// 结束

GcTimer*        g_pTimer = 0;        // 计时器指针
CSceneMusicApp* g_pMusicThread = 0;  // 音乐线程指针

RtgDeviceD3D9* GetDevice() {
    return RtGetRender();
}

GcTimer* GetTimer() {
    if (g_pTimer == NULL)
        g_pTimer = RT_NEW GcTimer();  // 如果计时器为空，则创建一个新的GcTimer实例
    return g_pTimer;                  // 返回计时器实例
}

//HWND GetWndHandle() {
//    return GetDevice()->m_hWnd;
//}

CRtgAppFrame* GetApp() {
    return g_pGameClientFrame;
}

RtIni* GetGameIni() {
    return g_iniConfig;
}

bool AutoResize(void* sender, int i) {
    guard;
    RtwWidget* pWidget = (RtwWidget*)sender;
    if (!pWidget)
        return false;
    RtwRect rc;
    rc = pWidget->GetClientRect();
    if (i == 0) {
        int nLength = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
        int nLineCount = nLength / 240 + 1;
        if (nLineCount > 1) {
            rc.right = rc.left + 240 - 1;
            rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                        RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
        } else {
            rc.right = rc.left + nLength + RtwTextBox::m_LeftSpace * 2 + 3;
            rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                        RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
        }
        pWidget->Move(rc);
        return true;
    } else if (i == 1) {
        int nLength = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
        int nLineCount = ((RtwTextBox*)pWidget)->getLineCount();
        rc.right = rc.left + 180;
        rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                    6 * (nLineCount + 1) - 1;
        pWidget->Move(rc);
        return true;
    } else
        return false;
    unguard;
}

//获取各个对象的函数
GcWorld* GetWorld() {
    if (!g_pGameClientFrame || !((CGameClientFrame*)g_pGameClientFrame)->m_pWorld)
        return NULL;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld;
}
GcLogin* GetLogin() {
    if (!g_pGameClientFrame || !((CGameClientFrame*)g_pGameClientFrame)->m_pLogin)
        return NULL;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_pLogin;
}
GcPlayer* GetPlayer() {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL)
        return NULL;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_pPlayer;
}
GcPet* GetPlayerPet() {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL)
        return NULL;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_pGcPet;
}
NetSession* GetNet() {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL)
        return NULL;
    return &(((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_Net);
}

// 判断是否已经初始化
bool WorldIsInited() {
    if (g_pGameClientFrame == NULL)
        return false;
    if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL)
        return false;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->IsInit();
}
bool UIIsInited() {
    if (g_pGameClientFrame == NULL)
        return false;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_bUIInit != FALSE;
}

void ChangeGameFlow(bool bGame) {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow != (long)bGame) {
        ((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow = bGame;
        ((CGameClientFrame*)g_pGameClientFrame)->m_bChangeGameFlowNextFrame = TRUE;
    }
}
void GameClientGuardErrorCallBack(const char* szMsg) {
    // 恢复屏幕配置
    RtgDevice::ResumeDefaultDisplayMode();

#ifdef PUBLISH
    MessageBox(NULL, R(MSG_GAME_CRASH), g_szGCTitle, 0);
#else
    MessageBox(NULL, szMsg, g_szGCTitle, 0);
#endif
}

//static char g_szGuideServerHostIP[40] = "guide.zfs.gamigo.com.cn";
G_MEMDEF(g_szGuideServerHostIP, 40)
static int g_iGuideServerHostPort = 6620;

void GetGuideHostInfo(char** ppIP, int* piPort) {
    *ppIP = g_szGuideServerHostIP;
    *piPort = g_iGuideServerHostPort;
}

DWORD WINAPI callbackAccelerateCheck(LPVOID lpThreadParameter) {
    int   iMax = 2;
    int   iCur = 0;
    float fRate;
    DWORD dwOld, dwNew;
    while (1) {
        dwOld = rtGetMilliseconds();
        rtSleep(3.f);
        dwNew = rtGetMilliseconds();
        fRate = ((float)(dwNew - dwOld)) / 3000.f;
        if (fabs(fRate - 1.f) > 0.1f) {
#ifdef _DEBUG
            ERR2("Accelerated, Rate=%.3f, Count=%d\n", fRate, iCur);
#endif
            iCur++;
            if (iCur > iMax) {
#ifdef _DEBUG
                ERR("Hacker Deteched\n");
#endif
                if (s_isHacker == 0)
                    s_isHacker = 1;
            }
        } else {
            iCur--;
            if (iCur < 0) {
                iCur = 0;
            }
        }
        //rtSleep(rtRandom(120, 60));
    }
}

//用来阻止打开总日志的类
class CCcxyCoreFactory : public RtCoreFactory {
   public:
    RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

    virtual void InitLogRoot(RtLogType rootLog) {}
};

RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")
G_MEMDEF(s_szDetechUsername, 40)

//lyymark 1.Game.Init 程序main函数入口
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // CrashRP::Start();

    P_OPEN_CONSOLE();

    // add by yz 2010-6-11:注册全局内存保护
    RegisterMemProtector();
    //end

    if (!*lpCmdLine)
        lpCmdLine = "tooth.updaterun;127.0.0.1";

    int iResult = FALSE;
    rtRandomSeed(time(NULL));
    char* szMemoffset = RT_NEW char[((rtRandom() % 10) + 1) * 32];

    {

        // 命令行配置
        RtTextConfig cfgCommandLine(lpCmdLine);

        // 核心库初始化
#ifdef _DEBUG
        if (!rtCoreInit("clt_engine.ini")) {
            ShowMessage("Rt Core init failed.\nExit now!");
            return FALSE;
        }
#else

        RtRuntimeClass*   _CoreFactory = RT_RUNTIME_CLASS(CCcxyCoreFactory);
        std::string       cmdLine(lpCmdLine);
        const std::string keyword = "showlog;";
        if (cmdLine.find(keyword) == 0) {
            cmdLine.erase(0, keyword.length());
            _CoreFactory = nullptr;
        } else {
            //重写CoreFactory来阻止打开日志
            _CoreFactory = RT_RUNTIME_CLASS(CCcxyCoreFactory);
        }
        g_bLogEnable = (_CoreFactory == nullptr);

        if (!rtCoreInit("clt_engine.ini", _CoreFactory)) {
            ShowMessage("Rt Core init failed.\nExit now!");
            return FALSE;
        }
        ///!不需要生成memory dump
        RtMemoryAllocator::Instance().SetDumpMemLeak(false);

#endif
        // 打包管理添加
        CRtPackManager* pPackManager = RT_NEW CRtPackManager;  //ac.ma
        pPackManager->OpenPack("game_client.pak", false);

        CRtPackAndAnsiManager* pAllManager =
            RT_NEW             CRtPackAndAnsiManager(pPackManager, &RtCoreFile());
        RtCore::Instance().pFileManager = pAllManager;

    

        //游戏的最开始的配置文件
        string ErrorString;
        if (!LoadConfig(ErrorString)) {
            MessageBox(NULL, ErrorString.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }

        //lyymark 1.Game.Init 得到"Strmap.csv"文件
        const char* pStrmapPath = GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath");
        if (pStrmapPath == NULL) {
            MessageBox(NULL, "Can't find [strmap.csv] path !", "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }

        // 字符替换程序
        InitMapString(pStrmapPath);

        // 游戏配置文件
        g_iniConfig = RT_NEW RtIni();
        if (g_iniConfig->OpenFile(R(INI_GAME))) {
            g_iniConfig->CloseFile();
        } else {
            MessageBox(NULL,
                       "Error: Cannot open game setting file [game.ini]\n Please check if the "
                       "directory is correct or re-install the game",
                       "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }

        // 配置信息
        long lIniResWidth = 1024, lIniResHeight = 768, lIniResColorDepth = 32;
        long lIniFullscreen = 0;      // 全屏模式   关
        long lIniTextureQuality = 0;  // 贴图质量   高
        long lIniEffectQuality = 1;   // 特效质量   中
        long lIniShadowQuality = 1;   // 阴影质量   中
        long lIniFsaa = 1;            // 全屏抗锯齿 关
        long lIniEnvref = 1;          // 环境反射   开
        long lIniObjref = 1;          // 反射人物   开
        long lIniPosteffect = 1;      // 后期处理   关
        long lIniObjeffect = 1;       // 后期加强   关
        long lIniMusicVolume = 50;    // 音乐音量   50%
        long lIniSoundVolume = 60;    // 音效音量   60%
        long lUse3DAudio = 1;         // 用3D音效   开
        long lCaptureVideo = 0;
        bool bBackGroundMusicIsSlience = false;
        bool bSoundIsSlience = false;

        GetGameIni()->GetEntry("graph_default", "FullScreen", &lIniFullscreen);
        GetGameIni()->GetEntry("graph_default", "WindowSizeX", &lIniResWidth);
        GetGameIni()->GetEntry("graph_default", "WindowSizeY", &lIniResHeight);
        GetGameIni()->GetEntry("graph_default", "ColorDepth", &lIniResColorDepth);
        GetGameIni()->GetEntry("graph_default", "texture", &lIniTextureQuality);
        GetGameIni()->GetEntry("graph_default", "effect", &lIniEffectQuality);
        GetGameIni()->GetEntry("graph_default", "shadow", &lIniShadowQuality);
        GetGameIni()->GetEntry("graph_default", "fsaa", &lIniFsaa);
        GetGameIni()->GetEntry("graph_default", "envref", &lIniEnvref);
        GetGameIni()->GetEntry("graph_default", "objref", &lIniObjref);
        GetGameIni()->GetEntry("graph_default", "posteffect", &lIniPosteffect);
        GetGameIni()->GetEntry("graph_default", "objeffect", &lIniObjeffect);
        GetGameIni()->GetEntry("Audio", "DefaultMusicVal", &lIniMusicVolume);
        GetGameIni()->GetEntry("Audio", "DefaultSoundVal", &lIniSoundVolume);
        GetGameIni()->GetEntry("Audio", "Audio3d", &lUse3DAudio);

        RtIni iniUser;
        if (iniUser.OpenFile(R(INI_USER))) {
            iniUser.GetEntry("graph", "fullscreen", &lIniFullscreen);
            iniUser.GetEntry("graph", "width", &lIniResWidth);
            iniUser.GetEntry("graph", "height", &lIniResHeight);
            iniUser.GetEntry("graph", "color", &lIniResColorDepth);
            iniUser.GetEntry("graph", "texture", &lIniTextureQuality);
            iniUser.GetEntry("graph", "effect", &lIniEffectQuality);
            iniUser.GetEntry("graph", "shadow", &lIniShadowQuality);
            iniUser.GetEntry("graph", "fsaa", &lIniFsaa);
            iniUser.GetEntry("graph", "envref", &lIniEnvref);
            iniUser.GetEntry("graph", "objref", &lIniObjref);
            iniUser.GetEntry("graph", "posteffect", &lIniPosteffect);
            iniUser.GetEntry("graph", "objeffect", &lIniObjeffect);
            iniUser.GetEntry("graph", "capture", &lCaptureVideo);
            iniUser.GetEntry("audio", "music", &lIniMusicVolume);
            iniUser.GetEntry("audio", "BackGroundMusicIsSlience", &bBackGroundMusicIsSlience);
            iniUser.GetEntry("audio", "sound", &lIniSoundVolume);
            iniUser.GetEntry("audio", "SoundIsSlience", &bSoundIsSlience);
            iniUser.GetEntry("audio", "use3d", &lUse3DAudio);
            iniUser.CloseFile();
        }

#ifndef _DEBUG
        //if (!cfgCommandLine.GetValue("tooth.updaterun"))
        if (std::string::npos == ((std::string)lpCmdLine).find("tooth.updaterun")) {
            MessageBox(NULL, R(MSG_RUNEXE_FSO), R(G_INFO), MB_OK | MB_ICONINFORMATION);
            DEL_ARRAY(szMemoffset);
            return 0;
        }
#endif

        // 取得向导服务器的IP地址和端口，并且去取得GameWorld服务器列表
        const char *szGuideIP, *szGuidePort;
        int         semicolon = ((std::string)lpCmdLine).find(";");
        std::string xx =
            ((std::string)lpCmdLine).substr(semicolon + 1, sizeof(lpCmdLine) - semicolon);
        szGuideIP = xx.c_str();

        if (szGuideIP == "") {
            szGuideIP = GetGameIni()->GetEntry("net", "GuideIP");
            if (szGuideIP == NULL)
                szGuideIP = R(RES_DEFAULT_GUIDE_SERVER_IP);
        }
        if (!cfgCommandLine.GetValue("port", &szGuidePort)) {
            szGuidePort = GetGameIni()->GetEntry("net", "GuidePort");
            if (szGuidePort == NULL)
                szGuidePort = R(RES_DEFAULT_GUIDE_SERVER_PORT);
        }
        if (szGuideIP) {
            rt2_strncpy(g_szGuideServerHostIP, szGuideIP, 40);
        }
        if (szGuidePort)
            g_iGuideServerHostPort = atoi(szGuidePort);

        GcLogin::StartGetGameWorldServer();

        rtSetGuardErrorCallback(GameClientGuardErrorCallBack);

        ReadVersionFile(R(INI_VERSION));

        // 图形系统初始化
        RTGDeviceType eDeviceType;

        eDeviceType = RTG_DEVICE_D3D9;

        if (!rtGraphInit(/*"RT3D-Engine", 0, eDeviceType*/))  // 注释 [3/16/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return FALSE;
        }

        if (!GetDevice()) {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return FALSE;
        }
        //lyytodo 替换json
        //lyymark 1.Game.Init 整个游戏的初始化入口 图形标题配置加载
        if (!GetDevice()->Init(
                hInstance, RT_RUNTIME_CLASS(CGameClientFrame), RT_RUNTIME_CLASS(GcCamera),
                "clt_graph.ini",
                "user.ini"))  // change form GcCamera to RtgCameraEditor [3/24/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            //return FALSE;
        }

        // 声音系统初始化,图形系统的初始化中要使用到声音

        if (!rtSoundInit("RT3D-Audio", (HWND)GetDevice()->GetWndHandle())) {
            ShowMessage(R(MSG_CANNOTINIT_AUDIO));
            return FALSE;
        }
        if (!rtMusicInit()) {
            ShowMessage(R(MSG_CANNOTINIT_AUDIO));
            return FALSE;
        }
        g_pBackMusic->SetMusicPath("music");
        g_pSoundMgr->SetAudioPath("audio");
        // lyymark 1.Game.Init 创建音乐线程
        g_pMusicThread = RT_NEW CSceneMusicApp();
        if (g_pMusicThread) {
            g_pMusicThread->Start();
        }

        // 创建程序框架
        g_pGameClientFrame = ((CGameClientFrame*)GetDevice()->GetEvent());
        g_pGameClientFrame->EnableNotifyOnMouseMove(true);
        if (g_pGameClientFrame == NULL) {
            ShowMessage(R(MSG_MEMORY_FAIL));
            return FALSE;
        }

        if (lCaptureVideo) {
            ((CGameClientFrame*)g_pGameClientFrame)->m_bCanCaptureVideo = true;
        }

#ifdef _DEBUG
        {
            long lTotalTextureMem;
            if (GetGameIni()->GetEntry("Graph", "TotalTextureMem", &lTotalTextureMem)) {
                //GetDevice()->m_Texture.SetTotalTextureMem(lTotalTextureMem);
            }
        }
#endif

        if (lIniFullscreen) {
            GetDevice()->SetViewMode(RTGVWM_TOPWINDOW);
        }

        g_pBackMusic->SetMusicPath("music");
        g_pSoundMgr->SetAudioPath("audio");

        // 人物系统初始化
        if (!ActorInit()) {
            CHECKEX(R(MSG_CANNOTINIT_ACTOR));
        }

        RtcGetActorManager()->AddSearchPath("creature/actor/");
        RtcGetActorManager()->AddSearchPath("creature/material/");
        RtcGetActorManager()->AddSearchPath("creature/effect/");
        RtcGetActorManager()->AddSearchPath("scene/actor/");
        RtcGetActorManager()->AddSearchPath("scene/material/");
        RtcGetActorManager()->AddSearchPath("scene/effect/");

        // 场景系统初始化
        if (!rtSceneInit("RT3D-Scene")) {
            CHECKEX(R(MSG_CANNOTINIT_SCENE));
        }
        g_pScene = RT_NEW GcScene;
        if (!g_pScene->OnceInit(GetDevice())) {
            CHECKEX("ERROR: Scene initialization fail!");
            return false;
        }

        ActorSetScene(g_pScene);

        // add [3/16/2009 tooth.shi]
        if (!((CGameClientFrame*)g_pGameClientFrame)->OnEnterLogin()) {
            return false;
        }

        // 初始化d3d场景渲染
        GetDevice()->RenderScene();

#ifdef USE_LUA_CHECK
        LuaInit();
#endif

        // 用户ini初始化
        GetDevice()->GetTextMgr()->SetTextureQuality(lIniTextureQuality * 2);
        EActorQuality eqActor, eqSkin, eqMtl, eqEffect;
        switch (lIniEffectQuality) {
            case 2:
                eqEffect = QUALITY_LOW;
                break;
            default:
            case 1:
                eqEffect = QUALITY_MIDDLE;
                break;
            case 0:
                eqEffect = QUALITY_HIGH;
                break;
        }
        eqActor = eqSkin = eqMtl = eqEffect;
        ActorSetQuality(eqActor, eqSkin, eqMtl, eqEffect);
        switch (lIniShadowQuality) {
            case 2:
                GcRenderActorBase::s_iShadowQuality = 1;
                break;
            default:
            case 1:
                GcRenderActorBase::s_iShadowQuality = 2;
                break;
            case 0:
                GcRenderActorBase::s_iShadowQuality = 3;
                break;
        }
        GetDevice()->SetRenderState(RTGRS_FULLSCRENE_ANTIALIASING, lIniFsaa);
        g_pScene->m_bReflectSceneObject = lIniEnvref;
        g_pScene->m_bReflectOtherActor = lIniObjref;
        //GetDevice()->SetPostProcessEffectOnoff(lIniPosteffect!=0);
        //GetDevice()->SetPostProcessObjectHighLightOnoff(lIniObjeffect!=0);

        if (!bBackGroundMusicIsSlience)
            g_pBackMusic->SetVolume(lIniMusicVolume / 100.f);
        else
            g_pBackMusic->SetVolume(0.f);
        if (!bSoundIsSlience)
            g_pSoundMgr->SetWholeVolume(lIniSoundVolume / 100.f);
        else
            g_pSoundMgr->SetWholeVolume(0.f);
        g_pSoundMgr->SetUse3dAudio(lUse3DAudio != 0);

#if DO_CONSOLE
        long lCanShowConsole;
        if (!GetGameIni()->GetEntry("Graph", "Console", &lCanShowConsole)) {
            lCanShowConsole = 0;
        }
        g_pGameClientFrame->ShowConsole(1);
#endif

        // 应用程序框架初始化
        if (!g_pGameClientFrame->Init(GetDevice() /*hInstance, g_hGCWnd, g_hGCWnd*/)) {
            ShowMessage(R(MSG_CANNOTINIT_GAME));
            return FALSE;
        }
    }

    {

        //lyymark 1.Game.Init 启动游戏引擎，客户端主循环入口
        iResult = GetDevice()->RunEngine();
    }

    {

        // 应用程序框架退出
        g_pGameClientFrame->Close();

        // 场景系统退出
        P_LOGINFO("Exit Pre Scene\n");
        rtScenePreExit();

        P_LOGINFO("Exit Scene\n");
        rtSceneExit();

        // 人物系统退出
        //LOG("Exit Actor\n");
        P_LOGINFO("Exit Actor\n");
        ActorExit();

        g_pMusicThread->Terminate(0);
        DEL_ONE(g_pMusicThread);
        g_pMusicThread = nullptr;

        // 声音系统退出
        P_LOGINFO("Exit Pre Audio\n");
        rtAudioPreExit();
        P_LOGINFO("Exit Audio\n");
        rtAudioExit();

        // 图形系统退出
        P_LOGINFO("Exit Graph\n");
        rtGraphExit();

        // 删除应用程序框架
        P_LOGINFO("Exit Frame\n");
        DEL_ONE(g_pGameClientFrame);

        ClearMapString();

        // 核心库退出
        P_LOGINFO("Exit Core\n");
        rtCoreExit();
        P_CLOSE_CONSOLE();
    }

#ifdef _DEBUG
    {
        //         int iGetRefObjCount = RtwRefObject::getRefCount();
        //         int iGetRefObjRefCount = RtwRefObject::getRefCount();
        //         CHECK(iGetRefObjCount==0);
        //         CHECK(iGetRefObjRefCount==0);
    }
#endif

    DEL_ARRAY(szMemoffset);
    switch (iResult) {
        case 22: {
            //const char* szNewUserCard = GetGameIni()->GetEntry("net", "NewUserCard");
            //if (szNewUserCard==NULL)
            //{
            //    szNewUserCard = "http://zfs.gamigo.com.cn/member/mem_reg.aspx?active_code=%s&active_password=%s";
            //}
            //std::string szCmd = "explorer.exe \"";
            //sprintf(g_strStaticBuffer, szNewUserCard, GcLogin::m_szAccountUsername, GcLogin::m_szAccountPassword);
            //szCmd += g_strStaticBuffer;
            //szCmd += "\"";
            //WinExec(szCmd.c_str(), SW_SHOW);
        } break;
        case 23: {
            //const char* szHackerDeteched = GetGameIni()->GetEntry("net", "HackerDetected");
            //if (szHackerDeteched)
            //{
            //    std::string szCmd = "explorer.exe \"";
            //    szCmd += szHackerDeteched;
            //    if (s_szDetechUsername[0])
            //    {
            //        szCmd += "?name=";
            //        szCmd += s_szDetechUsername;
            //    }
            //    szCmd += "\"";
            //    WinExec(szCmd.c_str(), SW_SHOW);
            //}
        } break;
        default: {
            //const char* szNewUserCard = NULL;
            //if (szNewUserCard==NULL)
            //{
            //    szNewUserCard = "http://www.baidu.com";
            //}
            //std::string szCmd = "explorer.exe \"";
            ////sprintf(g_strStaticBuffer, szNewUserCard, GcLogin::m_szAccountUsername, GcLogin::m_szAccountPassword);
            //szCmd += szNewUserCard;
            //szCmd += "\"";
            //WinExec(szCmd.c_str(), SW_SHOW);
        } break;
    }

    //CrashRP::Stop();
    return iResult ? iResult : 0;
}

//add by yz 2010-6-11:全局内存保护注册
void RegisterMemProtector() {
#ifdef ENABLE_OPERATOR_NEW
    rt2_ActiveSafestr();
#endif

    RegisterSharedMemProtector();

    G_MEMPROTECTOR(g_szGCTitle, 100)
    rt2_strncpy(g_szGCTitle, "FSO Client", 100);
    G_MEMPROTECTOR(g_szGCWindowClass, 100)
    rt2_strncpy(g_szGCWindowClass, "RT3D_FSO_CLIENT", 100);

    G_MEMPROTECTOR(g_szGuideServerHostIP, 40)
    rt2_strncpy(g_szGuideServerHostIP, "guide.zfs.gamigo.com.cn", 40);
    G_MEMPROTECTOR(s_szDetechUsername, 40)

    G_MEMPROTECTOR(s_szRegionHost, 80)  //gc_global.cpp
    G_MEMPROTECTOR(s_szStringTemp, STRING_LENGTH)

    G_MEMPROTECTOR(s_strCheckProcessName, 200)  //gc_world.cpp
}

//end

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

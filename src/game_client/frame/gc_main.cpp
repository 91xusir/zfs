#include "gc_include.h"            // 包含游戏核心的公共头文件
#include "gc_camera.h"             // 包含游戏中摄像机相关的头文件
#include "filepack/memory_file.h"  // 包含文件打包和内存文件相关的头文件
#include "gc_login.h"              // 包含游戏登录相关的头文件
#include <preConsole.h>
#include <minwindef.h>

// -------------------------全局变量------------------------------
extern RtScene*      g_pScene;                // 全局场景指针
static CRtgAppFrame* g_pGameClientFrame = 0;  // 游戏客户端框架指针
static RtIni* g_iniConfig;  // 配置文件指针，指向 "game.ini"，此指针不会被删除

bool g_bLogEnable = false;  // release模式模式下cmd读取命令判断是否开启log，在这里记录

GcTimer*        g_pTimer       = 0;  // 计时器指针
CSceneMusicApp* g_pMusicThread = 0;  // 音乐线程指针

G_MEMDEF(g_szGuideServerHostIP, 40) // 向导服务器IP
static int g_iGuideServerHostPort = 6620; 


//-------------------------全局函数------------------------------
//自动调整大小
bool AutoResize(void* sender, int i) {
    guard;
    RtwWidget* pWidget = (RtwWidget*)sender;
    if (!pWidget)
        return false;
    RtwRect rc;
    rc = pWidget->GetClientRect();
    if (i == 0) {
        int nLength    = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
        int nLineCount = nLength / 240 + 1;
        if (nLineCount > 1) {
            rc.right  = rc.left + 240 - 1;
            rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                        RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
        } else {
            rc.right  = rc.left + nLength + RtwTextBox::m_LeftSpace * 2 + 3;
            rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                        RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
        }
        pWidget->Move(rc);
        return true;
    } else if (i == 1) {
        int nLength    = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
        int nLineCount = ((RtwTextBox*)pWidget)->getLineCount();
        rc.right       = rc.left + 180;
        rc.bottom      = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() +
                    6 * (nLineCount + 1) - 1;
        pWidget->Move(rc);
        return true;
    } else
        return false;
    unguard;
}

//获取各个对象的函数
RtgDeviceD3D9* GetDevice() {
    return RtGetRender();
}

GcTimer* GetTimer() {
    if (g_pTimer == NULL)
        g_pTimer = RT_NEW GcTimer();
    return g_pTimer;                 
}

CRtgAppFrame* GetApp() {
    return g_pGameClientFrame;
}

RtIni* GetGameIni() {
    return g_iniConfig;
}

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

bool IsUIInited() {
    if (g_pGameClientFrame == NULL)
        return false;
    return ((CGameClientFrame*)g_pGameClientFrame)->m_bUIInit;
}

// 用于切换游戏流程
void ChangeGameFlow(bool bGame) {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow != (long)bGame) {
        ((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow            = bGame;
        ((CGameClientFrame*)g_pGameClientFrame)->m_bChangeGameFlowNextFrame = TRUE;
    }
}

// 获取向导服务器的IP和端口
void GetGuideHostInfo(char** ppIP, int* piPort) {
    *ppIP   = g_szGuideServerHostIP;
    *piPort = g_iGuideServerHostPort;
}

//用来重写阻止打开总日志的类
class CCcxyCoreFactory : public RtCoreFactory {
   public:
    RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

    virtual void InitLogRoot(RtLogType rootLog) {}
};
RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

namespace {
static std::vector<std::string> cmdLines;  // 简单存储所有命令的列表

static void ParseCommandLine(const std::string& lpCmdLine) {
    std::stringstream ss(lpCmdLine);
    std::string       command;
    while (std::getline(ss, command, ';')) {
        cmdLines.push_back(command);
    }
}

static bool IsValidIpAddress(const std::string& ipAddress) {
    std::vector<std::string> parts;
    std::stringstream        ss(ipAddress);
    std::string              part;
    while (std::getline(ss, part, '.')) {
        parts.push_back(part);
    }
    if (parts.size() != 4) {
        return false;
    }
    for (const std::string& part : parts) {
        if (part.empty() || part.size() > 3 || !std::all_of(part.begin(), part.end(), ::isdigit)) {
            return false;
        }
        int num = std::stoi(part);
        if (num < 0 || num > 255) {
            return false;
        }
    }
    return true;
}
}  // namespace

//lyymark 1.Game.Init 程序main函数入口
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    auto start         = std::chrono::high_resolution_clock::now();
    auto printDuration = [start]() {
        auto end = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> duration = end - start; 
        std::cout << "加载用时: " << duration.count() << " 秒" << std::endl; 
    };

    ParseCommandLine(std::string(lpCmdLine));  //解析命令行

    //开发中暂时不用
    //if (!cmdLines.size()) {//请执行 zfs.exe 来进行游戏
    //    MessageBox(NULL, R(MSG_RUNEXE_FSO), R(G_INFO), MB_OK | MB_ICONINFORMATION);
    //    return 0;
    //}

    P_OPEN_CONSOLE();  //开启preview控制台

    //--------------------Core核心库初始化-------------------
#ifdef _DEBUG
    if (!rtCoreInit("config/game.ini")) {
        ShowMessage("Rt Core init failed.\nExit now!");
        return 0;
    }
#else
    RtRuntimeClass* _CoreFactory = RT_RUNTIME_CLASS(CCcxyCoreFactory);
    _CoreFactory                 = RT_RUNTIME_CLASS(CCcxyCoreFactory);
    g_bLogEnable                 = (_CoreFactory == nullptr);
    if (!rtCoreInit("config/game.ini", _CoreFactory)) {
        ShowMessage("Rt Core init failed.\nExit now!");
        return 0;
    }
#endif
    // ------------------包管理和文件系统初始化---------------
    {
        CRtPackManager* pPackManager = RT_NEW CRtPackManager;
        pPackManager->OpenPack("game_client.pak", false);
        CRtPackAndAnsiManager* pAllManager =
            RT_NEW             CRtPackAndAnsiManager(pPackManager, &RtCoreFile());
        RtCore::Instance().pFileManager = pAllManager;
    }
    // -------------------配置文件加载------------------------
    rt_graph::RtgDevice::SUserConfig m_userConfig;
    rt_graph::RtgDevice::SConfig     m_deviceConfig;
    long                             lCaptureVideo             = 0;
    long                             lIniMusicVolume           = 50;  // 音乐音量   50%
    long                             lIniSoundVolume           = 60;  // 音效音量   60%
    long                             lUse3DAudio               = 1;   // 用3D音效   开
    long                             lIniTextureQuality        = 0;   // 贴图质量   高
    long                             lIniFsaa                  = 1;   // 全屏抗锯齿 关
    long                             lIniEnvref                = 1;   // 环境反射   开
    long                             lIniObjref                = 1;   // 反射人物   开
    long                             lIniEffectQuality         = 1;   // 特效质量   中
    bool                             bBackGroundMusicIsSlience = false;
    bool                             bSoundIsSlience           = false;
    long                             lIniShadowQuality         = 1;  // 阴影质量   中
    {
        string ErrorString;
        if (!LoadConfig(ErrorString)) {  //config_boot.ini加载
            MessageBox(NULL, ErrorString.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
        //lyymark 1.Game.Init "Strmap.csv"文件初始化
        const char* pStrmapPath = GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath");
        if (pStrmapPath == NULL) {
            MessageBox(NULL, "Can't find [strmap.csv] path !", "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
        InitMapString(pStrmapPath);
        // game.ini加载
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
        // 版本信息初始化
        VersionInit();
        long        lIniResWidth = 1024, lIniResHeight = 768, lIniResColorDepth = 32;
        long        lIniFullscreen  = 0;  // 全屏模式   关
        long        lIniPosteffect  = 1;  // 后期处理   关
        long        lIniObjeffect   = 1;  // 后期加强   关
        long        EdgeAntialias   = 0;
        long        MultiSampleWnd  = 0;
        long        MultiSampleFsc  = 0;
        long        LockFps         = 30;
        long        Vsync           = 0;
        long        GeometryQuality = 0;
        const char* graph_default   = "graph_default";
        const char* audio           = "audio";
        const char* Window          = "Window";
        const char* Camera          = "Camera";
        GetGameIni()->GetEntry(graph_default, "FullScreen", &lIniFullscreen);
        GetGameIni()->GetEntry(graph_default, "WindowSizeX", &lIniResWidth);
        GetGameIni()->GetEntry(graph_default, "WindowSizeY", &lIniResHeight);
        GetGameIni()->GetEntry(graph_default, "ColorDepth", &lIniResColorDepth);
        GetGameIni()->GetEntry(graph_default, "texture", &lIniTextureQuality);
        GetGameIni()->GetEntry(graph_default, "effect", &lIniEffectQuality);
        GetGameIni()->GetEntry(graph_default, "shadow", &lIniShadowQuality);
        GetGameIni()->GetEntry(graph_default, "fsaa", &lIniFsaa);
        GetGameIni()->GetEntry(graph_default, "envref", &lIniEnvref);
        GetGameIni()->GetEntry(graph_default, "objref", &lIniObjref);
        GetGameIni()->GetEntry(graph_default, "posteffect", &lIniPosteffect);
        GetGameIni()->GetEntry(Window, "StartX", &(m_deviceConfig.iDefaultStartX));
        GetGameIni()->GetEntry(Window, "StartY", &(m_deviceConfig.iDefaultStartY));
        GetGameIni()->GetEntry(Window, "Title", m_deviceConfig.szWindowTitle, 256);
        GetGameIni()->GetEntry(Camera, "FOV", &(m_deviceConfig.fCameraFOV));
        GetGameIni()->GetEntry(Camera, "Aspect", &(m_deviceConfig.fCameraAspect));
        GetGameIni()->GetEntry(Camera, "Near", &(m_deviceConfig.fCameraNear));
        GetGameIni()->GetEntry(Camera, "Far", &(m_deviceConfig.fCameraFar));
        GetGameIni()->GetEntry(Camera, "EnableFog", &(m_deviceConfig.bEnableFog));
        GetGameIni()->GetEntry(audio, "DefaultMusicVal", &lIniMusicVolume);
        GetGameIni()->GetEntry(audio, "DefaultSoundVal", &lIniSoundVolume);
        GetGameIni()->GetEntry(audio, "Audio3d", &lUse3DAudio);
        RtIni iniUser;
        if (iniUser.OpenFile(R(INI_USER))) {
            iniUser.GetEntry(graph_default, "FullScreen", &lIniFullscreen);
            iniUser.GetEntry(graph_default, "WindowSizeX", &lIniResWidth);
            iniUser.GetEntry(graph_default, "WindowSizeY", &lIniResHeight);
            iniUser.GetEntry(graph_default, "ColorDepth", &lIniResColorDepth);
            iniUser.GetEntry(graph_default, "texture", &lIniTextureQuality);
            iniUser.GetEntry(graph_default, "effect", &lIniEffectQuality);
            iniUser.GetEntry(graph_default, "shadow", &lIniShadowQuality);
            iniUser.GetEntry(graph_default, "fsaa", &lIniFsaa);
            iniUser.GetEntry(graph_default, "envref", &lIniEnvref);
            iniUser.GetEntry(graph_default, "objref", &lIniObjref);
            iniUser.GetEntry(graph_default, "posteffect", &lIniPosteffect);
            iniUser.GetEntry(graph_default, "objeffect", &lIniObjeffect);
            iniUser.GetEntry(graph_default, "capture", &lCaptureVideo);
            iniUser.GetEntry(graph_default, "MultiSampleWnd", &MultiSampleWnd);
            iniUser.GetEntry(graph_default, "MultiSampleFsc", &MultiSampleFsc);
            iniUser.GetEntry(graph_default, "EdgeAntialias", &EdgeAntialias);
            iniUser.GetEntry(graph_default, "LockFps", &LockFps);
            iniUser.GetEntry(graph_default, "Vsync", &Vsync);
            iniUser.GetEntry(graph_default, "GeometryQuality", &GeometryQuality);
            iniUser.GetEntry(audio, "music", &lIniMusicVolume);
            iniUser.GetEntry(audio, "BackGroundMusicIsSlience", &bBackGroundMusicIsSlience);
            iniUser.GetEntry(audio, "sound", &lIniSoundVolume);
            iniUser.GetEntry(audio, "SoundIsSlience", &bSoundIsSlience);
            iniUser.GetEntry(audio, "use3d", &lUse3DAudio);
            iniUser.CloseFile();
        }
        m_userConfig.bFullscreen                    = lIniFullscreen;
        m_userConfig.lWndWidth                      = lIniResWidth;
        m_userConfig.lWndHeight                     = lIniResHeight;
        m_userConfig.lWndColorDepth                 = lIniResColorDepth;
        m_userConfig.bFSAA                          = lIniFsaa;
        m_userConfig.bEdgeAntialias                 = EdgeAntialias;  //边缘抗锯齿
        m_userConfig.bPosteffect                    = lIniPosteffect;
        m_userConfig.bObjEffect                     = lIniObjeffect;
        m_userConfig.dwMaxMultiSampleTypeWindowed   = MultiSampleWnd;
        m_userConfig.dwMaxMultiSampleTypeFullscreen = MultiSampleFsc;
        m_userConfig.lMillisecondPerFrame           = LockFps;
        m_userConfig.bvsync                         = Vsync;
        m_userConfig.lTextureQuality                = lIniTextureQuality;
        m_userConfig.lGeometryQuality               = GeometryQuality;
    }

    // -------------------网络初始化加载------------------------
    std::string strGuideIP   = "127.0.0.1";
    int         strGuidePort = 6620;
    {
        // 取得向导服务器的IP地址和端口，并且去取得GameWorld服务器列表
        strGuideIP   = (std::string)(*g_iniConfig)["net"]["GuideIP"];
        strGuidePort = (int)(*g_iniConfig)["net"]["GuidePort"];
        if (cmdLines.size() > 0 && cmdLines[0] == "tooth.updaterun") {
            if (cmdLines.size() > 1 && !cmdLines[1].empty()) {
                strGuideIP = cmdLines[1];
            }
            if (cmdLines.size() > 2 && !cmdLines[2].empty()) {
                try {
                    strGuidePort = std::stoi(cmdLines[2]);
                } catch (const std::invalid_argument& e) {
                    P_LOGINFO(e.what());
                }
            }
        }
        rt2_strncpy(g_szGuideServerHostIP, strGuideIP.c_str(), 40);
        g_iGuideServerHostPort = strGuidePort;
        GcLogin::StartGetGameWorldServer();
    }

    // -------------------图形系统初始化------------------------
    {
        if (!rtGraphInit()) {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return 0;
        }
        //lyymark 1.Game.Init 整个游戏的初始化入口 图形标题配置加载
        if (!GetDevice()->Init(
                hInstance, RT_RUNTIME_CLASS(CGameClientFrame), RT_RUNTIME_CLASS(GcCamera),
                m_deviceConfig,
                m_userConfig))  // change form GcCamera to RtgCameraEditor [3/24/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
        }
    }
    // -------------------创建程序窗口框架初始化------------------------
    {
        g_pGameClientFrame = (CGameClientFrame*)(GetDevice()->GetEvent());
        g_pGameClientFrame->EnableNotifyOnMouseMove(true);
        if (g_pGameClientFrame == NULL) {
            ShowMessage(R(MSG_MEMORY_FAIL));
            return 0;
        }
        if (lCaptureVideo) {
            ((CGameClientFrame*)g_pGameClientFrame)->m_bCanCaptureVideo = true;
        }

        // 应用程序框架初始化
       /* if (!g_pGameClientFrame->Init(GetDevice())) {
            ShowMessage(R(MSG_CANNOTINIT_GAME));
            return FALSE;
        }*/
    }
    // -----------------声音、人物、场景初始化----------------
    {
        if (!rtSoundInit("RT3D-Audio", (HWND)GetDevice()->GetWndHandle())) {
            ShowMessage(R(MSG_CANNOTINIT_AUDIO));
            return 0;
        }
        if (!rtMusicInit()) {
            ShowMessage(R(MSG_CANNOTINIT_AUDIO));
            return 0;
        }
        g_pBackMusic->SetMusicPath("music");
        g_pSoundMgr->SetAudioPath("audio");
        // lyymark 1.Game.Init 创建音乐线程
        g_pMusicThread = RT_NEW CSceneMusicApp();
        if (g_pMusicThread) {
            g_pMusicThread->Start();
        }
        g_pBackMusic->SetMusicPath("music");
        g_pSoundMgr->SetAudioPath("audio");
  
        // 人物系统初始化
        if (!ActorInit()) {
            CHECKEX(R(MSG_CANNOTINIT_ACTOR));
        }
        // 场景系统初始化
        if (!rtSceneInit("RT3D-Scene")) {
            CHECKEX(R(MSG_CANNOTINIT_SCENE));
        }
        g_pScene = RT_NEW GcScene;
        if (!g_pScene->OnceInit(GetDevice())) {
            CHECKEX("ERROR: Scene initialization fail!");
            return false;
        }
        //设置actor场景为g_pScene
        ActorSetScene(g_pScene);
    }

    // lua初始化
#ifdef USE_LUA_CHECK
    LuaInit();
#endif

    // -------------------用户ini部分配置应用-------------
    {
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
        g_pScene->m_bReflectOtherActor  = lIniObjref;
        if (!bBackGroundMusicIsSlience)
            g_pBackMusic->SetVolume(lIniMusicVolume / 100.f);
        else
            g_pBackMusic->SetVolume(0.f);
        if (!bSoundIsSlience)
            g_pSoundMgr->SetWholeVolume(lIniSoundVolume / 100.f);
        else
            g_pSoundMgr->SetWholeVolume(0.f);
        g_pSoundMgr->SetUse3dAudio(lUse3DAudio != 0);
    }

  

    {
        //设置场景为登录
        if (!((CGameClientFrame*)g_pGameClientFrame)->OnEnterLogin()) {
            return false;
        }
        //lyymark 1.Game.Init 启动游戏引擎，客户端主循环入口
        GetDevice()->RunEngine();
    }

    {  //退出时后台释放内存
        P_LOGINFO("应用程序框架开始关闭");
        g_pGameClientFrame->Close();
        rtScenePreExit();
        rtSceneExit();
        P_LOGINFO("场景关闭");

        ActorExit();
        P_LOGINFO("人物系统退出");

        g_pMusicThread->Terminate(0);
        DEL_ONE(g_pMusicThread);
        g_pMusicThread = nullptr;
        rtAudioPreExit();
        rtAudioExit();
        P_LOGINFO("声音系统退出");

        rtGraphExit();
        P_LOGINFO("图形系统退出");

        DEL_ONE(g_pGameClientFrame);
        ClearMapString();
        P_LOGINFO("删除应用程序框架");

        rtCoreExit();
        P_LOGINFO("核心库退出");
        P_CLOSE_CONSOLE();
    }
    return 0;
}

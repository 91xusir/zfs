#include "gc_include.h"            // ������Ϸ���ĵĹ���ͷ�ļ�
#include "gc_camera.h"             // ������Ϸ���������ص�ͷ�ļ�
#include "filepack/memory_file.h"  // �����ļ�������ڴ��ļ���ص�ͷ�ļ�
#include "gc_login.h"              // ������Ϸ��¼��ص�ͷ�ļ�
#include <preConsole.h>
#include <minwindef.h>

// -------------------------ȫ�ֱ���------------------------------
extern RtScene*      g_pScene;                // ȫ�ֳ���ָ��
static CRtgAppFrame* g_pGameClientFrame = 0;  // ��Ϸ�ͻ��˿��ָ��
static RtIni* g_iniConfig;  // �����ļ�ָ�룬ָ�� "game.ini"����ָ�벻�ᱻɾ��

bool g_bLogEnable = false;  // releaseģʽģʽ��cmd��ȡ�����ж��Ƿ���log���������¼

GcTimer*        g_pTimer       = 0;  // ��ʱ��ָ��
CSceneMusicApp* g_pMusicThread = 0;  // �����߳�ָ��

G_MEMDEF(g_szGuideServerHostIP, 40) // �򵼷�����IP
static int g_iGuideServerHostPort = 6620; 


//-------------------------ȫ�ֺ���------------------------------
//�Զ�������С
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

//��ȡ��������ĺ���
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

// �ж��Ƿ��Ѿ���ʼ��
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

// �����л���Ϸ����
void ChangeGameFlow(bool bGame) {
    if (((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow != (long)bGame) {
        ((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow            = bGame;
        ((CGameClientFrame*)g_pGameClientFrame)->m_bChangeGameFlowNextFrame = TRUE;
    }
}

// ��ȡ�򵼷�������IP�Ͷ˿�
void GetGuideHostInfo(char** ppIP, int* piPort) {
    *ppIP   = g_szGuideServerHostIP;
    *piPort = g_iGuideServerHostPort;
}

//������д��ֹ������־����
class CCcxyCoreFactory : public RtCoreFactory {
   public:
    RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

    virtual void InitLogRoot(RtLogType rootLog) {}
};
RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

namespace {
static std::vector<std::string> cmdLines;  // �򵥴洢����������б�

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

//lyymark 1.Game.Init ����main�������
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    auto start         = std::chrono::high_resolution_clock::now();
    auto printDuration = [start]() {
        auto end = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> duration = end - start; 
        std::cout << "������ʱ: " << duration.count() << " ��" << std::endl; 
    };

    ParseCommandLine(std::string(lpCmdLine));  //����������

    //��������ʱ����
    //if (!cmdLines.size()) {//��ִ�� zfs.exe ��������Ϸ
    //    MessageBox(NULL, R(MSG_RUNEXE_FSO), R(G_INFO), MB_OK | MB_ICONINFORMATION);
    //    return 0;
    //}

    P_OPEN_CONSOLE();  //����preview����̨

    //--------------------Core���Ŀ��ʼ��-------------------
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
    // ------------------��������ļ�ϵͳ��ʼ��---------------
    {
        CRtPackManager* pPackManager = RT_NEW CRtPackManager;
        pPackManager->OpenPack("game_client.pak", false);
        CRtPackAndAnsiManager* pAllManager =
            RT_NEW             CRtPackAndAnsiManager(pPackManager, &RtCoreFile());
        RtCore::Instance().pFileManager = pAllManager;
    }
    // -------------------�����ļ�����------------------------
    rt_graph::RtgDevice::SUserConfig m_userConfig;
    rt_graph::RtgDevice::SConfig     m_deviceConfig;
    long                             lCaptureVideo             = 0;
    long                             lIniMusicVolume           = 50;  // ��������   50%
    long                             lIniSoundVolume           = 60;  // ��Ч����   60%
    long                             lUse3DAudio               = 1;   // ��3D��Ч   ��
    long                             lIniTextureQuality        = 0;   // ��ͼ����   ��
    long                             lIniFsaa                  = 1;   // ȫ������� ��
    long                             lIniEnvref                = 1;   // ��������   ��
    long                             lIniObjref                = 1;   // ��������   ��
    long                             lIniEffectQuality         = 1;   // ��Ч����   ��
    bool                             bBackGroundMusicIsSlience = false;
    bool                             bSoundIsSlience           = false;
    long                             lIniShadowQuality         = 1;  // ��Ӱ����   ��
    {
        string ErrorString;
        if (!LoadConfig(ErrorString)) {  //config_boot.ini����
            MessageBox(NULL, ErrorString.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
        //lyymark 1.Game.Init "Strmap.csv"�ļ���ʼ��
        const char* pStrmapPath = GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath");
        if (pStrmapPath == NULL) {
            MessageBox(NULL, "Can't find [strmap.csv] path !", "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
        InitMapString(pStrmapPath);
        // game.ini����
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
        // �汾��Ϣ��ʼ��
        VersionInit();
        long        lIniResWidth = 1024, lIniResHeight = 768, lIniResColorDepth = 32;
        long        lIniFullscreen  = 0;  // ȫ��ģʽ   ��
        long        lIniPosteffect  = 1;  // ���ڴ���   ��
        long        lIniObjeffect   = 1;  // ���ڼ�ǿ   ��
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
        m_userConfig.bEdgeAntialias                 = EdgeAntialias;  //��Ե�����
        m_userConfig.bPosteffect                    = lIniPosteffect;
        m_userConfig.bObjEffect                     = lIniObjeffect;
        m_userConfig.dwMaxMultiSampleTypeWindowed   = MultiSampleWnd;
        m_userConfig.dwMaxMultiSampleTypeFullscreen = MultiSampleFsc;
        m_userConfig.lMillisecondPerFrame           = LockFps;
        m_userConfig.bvsync                         = Vsync;
        m_userConfig.lTextureQuality                = lIniTextureQuality;
        m_userConfig.lGeometryQuality               = GeometryQuality;
    }

    // -------------------�����ʼ������------------------------
    std::string strGuideIP   = "127.0.0.1";
    int         strGuidePort = 6620;
    {
        // ȡ���򵼷�������IP��ַ�Ͷ˿ڣ�����ȥȡ��GameWorld�������б�
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

    // -------------------ͼ��ϵͳ��ʼ��------------------------
    {
        if (!rtGraphInit()) {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return 0;
        }
        //lyymark 1.Game.Init ������Ϸ�ĳ�ʼ����� ͼ�α������ü���
        if (!GetDevice()->Init(
                hInstance, RT_RUNTIME_CLASS(CGameClientFrame), RT_RUNTIME_CLASS(GcCamera),
                m_deviceConfig,
                m_userConfig))  // change form GcCamera to RtgCameraEditor [3/24/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
        }
    }
    // -------------------�������򴰿ڿ�ܳ�ʼ��------------------------
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

        // Ӧ�ó����ܳ�ʼ��
       /* if (!g_pGameClientFrame->Init(GetDevice())) {
            ShowMessage(R(MSG_CANNOTINIT_GAME));
            return FALSE;
        }*/
    }
    // -----------------���������������ʼ��----------------
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
        // lyymark 1.Game.Init ���������߳�
        g_pMusicThread = RT_NEW CSceneMusicApp();
        if (g_pMusicThread) {
            g_pMusicThread->Start();
        }
        g_pBackMusic->SetMusicPath("music");
        g_pSoundMgr->SetAudioPath("audio");
  
        // ����ϵͳ��ʼ��
        if (!ActorInit()) {
            CHECKEX(R(MSG_CANNOTINIT_ACTOR));
        }
        // ����ϵͳ��ʼ��
        if (!rtSceneInit("RT3D-Scene")) {
            CHECKEX(R(MSG_CANNOTINIT_SCENE));
        }
        g_pScene = RT_NEW GcScene;
        if (!g_pScene->OnceInit(GetDevice())) {
            CHECKEX("ERROR: Scene initialization fail!");
            return false;
        }
        //����actor����Ϊg_pScene
        ActorSetScene(g_pScene);
    }

    // lua��ʼ��
#ifdef USE_LUA_CHECK
    LuaInit();
#endif

    // -------------------�û�ini��������Ӧ��-------------
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
        //���ó���Ϊ��¼
        if (!((CGameClientFrame*)g_pGameClientFrame)->OnEnterLogin()) {
            return false;
        }
        //lyymark 1.Game.Init ������Ϸ���棬�ͻ�����ѭ�����
        GetDevice()->RunEngine();
    }

    {  //�˳�ʱ��̨�ͷ��ڴ�
        P_LOGINFO("Ӧ�ó����ܿ�ʼ�ر�");
        g_pGameClientFrame->Close();
        rtScenePreExit();
        rtSceneExit();
        P_LOGINFO("�����ر�");

        ActorExit();
        P_LOGINFO("����ϵͳ�˳�");

        g_pMusicThread->Terminate(0);
        DEL_ONE(g_pMusicThread);
        g_pMusicThread = nullptr;
        rtAudioPreExit();
        rtAudioExit();
        P_LOGINFO("����ϵͳ�˳�");

        rtGraphExit();
        P_LOGINFO("ͼ��ϵͳ�˳�");

        DEL_ONE(g_pGameClientFrame);
        ClearMapString();
        P_LOGINFO("ɾ��Ӧ�ó�����");

        rtCoreExit();
        P_LOGINFO("���Ŀ��˳�");
        P_CLOSE_CONSOLE();
    }
    return 0;
}

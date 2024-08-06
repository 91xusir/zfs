//#include <io.h>
#include "gc_include.h"            // ������Ϸ���ĵĹ���ͷ�ļ�
#include "gc_camera.h"             // ������Ϸ���������ص�ͷ�ļ�
//#include "gc_log_msg.h"            // ������Ϸ��־��Ϣ��ص�ͷ�ļ�
#include "filepack/memory_file.h"  // �����ļ�������ڴ��ļ���ص�ͷ�ļ�
#include "gc_login.h"              // ������Ϸ��¼��ص�ͷ�ļ�
//#include "resource.h"              // ������Դ��ص�ͷ�ļ�
//#include <CrashReport/Export.h>    // �����������浼��������ص�ͷ�ļ�

// ȫ�ֳ���ָ��
extern RtScene* g_pScene;

// ȫ�ֱ�����
static HINSTANCE g_hGCInst = 0;  // Ӧ�ó���ʵ�����
//static TCHAR   g_szGCTitle[100]        = "FSO Client"; // ���ڱ���
//static TCHAR   g_szGCWindowClass[100] = "RT3D_FSO_CLIENT"; // ��������
G_MEMDEF(g_szGCTitle, 100)        // ����һ��100�ַ����ȵ�ȫ�ֱ����ַ���
G_MEMDEF(g_szGCWindowClass, 100)  // ����һ��100�ַ����ȵ�ȫ�ִ��������ַ���

static CRtgAppFrame* g_pGameClientFrame = 0;  // ��Ϸ�ͻ��˿��ָ��
static RtIni* g_iniConfig;  // �����ļ�ָ�룬ָ�� "game.ini"����ָ�벻�ᱻɾ��

bool g_bLogEnable = false;  // ��־�����Ƿ����õı�־

// ��yz��2010-6-11��ӣ������ڴ汣����ע�ắ��
void RegisterMemProtector();
// ����

GcTimer*        g_pTimer = 0;        // ��ʱ��ָ��
CSceneMusicApp* g_pMusicThread = 0;  // �����߳�ָ��

RtgDeviceD3D9* GetDevice() {
    return RtGetRender();
}

GcTimer* GetTimer() {
    if (g_pTimer == NULL)
        g_pTimer = RT_NEW GcTimer();  // �����ʱ��Ϊ�գ��򴴽�һ���µ�GcTimerʵ��
    return g_pTimer;                  // ���ؼ�ʱ��ʵ��
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

//��ȡ��������ĺ���
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
    // �ָ���Ļ����
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

//������ֹ������־����
class CCcxyCoreFactory : public RtCoreFactory {
   public:
    RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

    virtual void InitLogRoot(RtLogType rootLog) {}
};

RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")
G_MEMDEF(s_szDetechUsername, 40)

//lyymark 1.Game.Init ����main�������
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // CrashRP::Start();

    P_OPEN_CONSOLE();

    // add by yz 2010-6-11:ע��ȫ���ڴ汣��
    RegisterMemProtector();
    //end

    if (!*lpCmdLine)
        lpCmdLine = "tooth.updaterun;127.0.0.1";

    int iResult = FALSE;
    rtRandomSeed(time(NULL));
    char* szMemoffset = RT_NEW char[((rtRandom() % 10) + 1) * 32];

    {

        // ����������
        RtTextConfig cfgCommandLine(lpCmdLine);

        // ���Ŀ��ʼ��
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
            //��дCoreFactory����ֹ����־
            _CoreFactory = RT_RUNTIME_CLASS(CCcxyCoreFactory);
        }
        g_bLogEnable = (_CoreFactory == nullptr);

        if (!rtCoreInit("clt_engine.ini", _CoreFactory)) {
            ShowMessage("Rt Core init failed.\nExit now!");
            return FALSE;
        }
        ///!����Ҫ����memory dump
        RtMemoryAllocator::Instance().SetDumpMemLeak(false);

#endif
        // ����������
        CRtPackManager* pPackManager = RT_NEW CRtPackManager;  //ac.ma
        pPackManager->OpenPack("game_client.pak", false);

        CRtPackAndAnsiManager* pAllManager =
            RT_NEW             CRtPackAndAnsiManager(pPackManager, &RtCoreFile());
        RtCore::Instance().pFileManager = pAllManager;

    

        //��Ϸ���ʼ�������ļ�
        string ErrorString;
        if (!LoadConfig(ErrorString)) {
            MessageBox(NULL, ErrorString.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }

        //lyymark 1.Game.Init �õ�"Strmap.csv"�ļ�
        const char* pStrmapPath = GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath");
        if (pStrmapPath == NULL) {
            MessageBox(NULL, "Can't find [strmap.csv] path !", "Error", MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }

        // �ַ��滻����
        InitMapString(pStrmapPath);

        // ��Ϸ�����ļ�
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

        // ������Ϣ
        long lIniResWidth = 1024, lIniResHeight = 768, lIniResColorDepth = 32;
        long lIniFullscreen = 0;      // ȫ��ģʽ   ��
        long lIniTextureQuality = 0;  // ��ͼ����   ��
        long lIniEffectQuality = 1;   // ��Ч����   ��
        long lIniShadowQuality = 1;   // ��Ӱ����   ��
        long lIniFsaa = 1;            // ȫ������� ��
        long lIniEnvref = 1;          // ��������   ��
        long lIniObjref = 1;          // ��������   ��
        long lIniPosteffect = 1;      // ���ڴ���   ��
        long lIniObjeffect = 1;       // ���ڼ�ǿ   ��
        long lIniMusicVolume = 50;    // ��������   50%
        long lIniSoundVolume = 60;    // ��Ч����   60%
        long lUse3DAudio = 1;         // ��3D��Ч   ��
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

        // ȡ���򵼷�������IP��ַ�Ͷ˿ڣ�����ȥȡ��GameWorld�������б�
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

        // ͼ��ϵͳ��ʼ��
        RTGDeviceType eDeviceType;

        eDeviceType = RTG_DEVICE_D3D9;

        if (!rtGraphInit(/*"RT3D-Engine", 0, eDeviceType*/))  // ע�� [3/16/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return FALSE;
        }

        if (!GetDevice()) {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            return FALSE;
        }
        //lyytodo �滻json
        //lyymark 1.Game.Init ������Ϸ�ĳ�ʼ����� ͼ�α������ü���
        if (!GetDevice()->Init(
                hInstance, RT_RUNTIME_CLASS(CGameClientFrame), RT_RUNTIME_CLASS(GcCamera),
                "clt_graph.ini",
                "user.ini"))  // change form GcCamera to RtgCameraEditor [3/24/2009 tooth.shi]
        {
            ShowMessage(R(MSG_CANNOTINIT_GRAPH));
            //return FALSE;
        }

        // ����ϵͳ��ʼ��,ͼ��ϵͳ�ĳ�ʼ����Ҫʹ�õ�����

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
        // lyymark 1.Game.Init ���������߳�
        g_pMusicThread = RT_NEW CSceneMusicApp();
        if (g_pMusicThread) {
            g_pMusicThread->Start();
        }

        // ����������
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

        // ����ϵͳ��ʼ��
        if (!ActorInit()) {
            CHECKEX(R(MSG_CANNOTINIT_ACTOR));
        }

        RtcGetActorManager()->AddSearchPath("creature/actor/");
        RtcGetActorManager()->AddSearchPath("creature/material/");
        RtcGetActorManager()->AddSearchPath("creature/effect/");
        RtcGetActorManager()->AddSearchPath("scene/actor/");
        RtcGetActorManager()->AddSearchPath("scene/material/");
        RtcGetActorManager()->AddSearchPath("scene/effect/");

        // ����ϵͳ��ʼ��
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

        // ��ʼ��d3d������Ⱦ
        GetDevice()->RenderScene();

#ifdef USE_LUA_CHECK
        LuaInit();
#endif

        // �û�ini��ʼ��
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

        // Ӧ�ó����ܳ�ʼ��
        if (!g_pGameClientFrame->Init(GetDevice() /*hInstance, g_hGCWnd, g_hGCWnd*/)) {
            ShowMessage(R(MSG_CANNOTINIT_GAME));
            return FALSE;
        }
    }

    {

        //lyymark 1.Game.Init ������Ϸ���棬�ͻ�����ѭ�����
        iResult = GetDevice()->RunEngine();
    }

    {

        // Ӧ�ó������˳�
        g_pGameClientFrame->Close();

        // ����ϵͳ�˳�
        P_LOGINFO("Exit Pre Scene\n");
        rtScenePreExit();

        P_LOGINFO("Exit Scene\n");
        rtSceneExit();

        // ����ϵͳ�˳�
        //LOG("Exit Actor\n");
        P_LOGINFO("Exit Actor\n");
        ActorExit();

        g_pMusicThread->Terminate(0);
        DEL_ONE(g_pMusicThread);
        g_pMusicThread = nullptr;

        // ����ϵͳ�˳�
        P_LOGINFO("Exit Pre Audio\n");
        rtAudioPreExit();
        P_LOGINFO("Exit Audio\n");
        rtAudioExit();

        // ͼ��ϵͳ�˳�
        P_LOGINFO("Exit Graph\n");
        rtGraphExit();

        // ɾ��Ӧ�ó�����
        P_LOGINFO("Exit Frame\n");
        DEL_ONE(g_pGameClientFrame);

        ClearMapString();

        // ���Ŀ��˳�
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

//add by yz 2010-6-11:ȫ���ڴ汣��ע��
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

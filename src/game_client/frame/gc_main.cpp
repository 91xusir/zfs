#include <io.h>
#include "gc_include.h" // ������Ϸ���ĵĹ���ͷ�ļ�
#include "gc_camera.h" // ������Ϸ���������ص�ͷ�ļ�
#include "gc_log_msg.h" // ������Ϸ��־��Ϣ��ص�ͷ�ļ�
#include "gc_log_sender.h" // ������Ϸ��־��������ص�ͷ�ļ�
#include "filepack/memory_file.h" // �����ļ�������ڴ��ļ���ص�ͷ�ļ�
#include "gc_login.h" // ������Ϸ��¼��ص�ͷ�ļ�
#include "resource.h" // ������Դ��ص�ͷ�ļ�
#include <CrashReport/Export.h> // �����������浼��������ص�ͷ�ļ�

// ����ָ��
extern RtScene* g_pScene;
//lyytodo ����δ֪
#ifdef _UNWG_
#   ifdef _DEBUG
#       pragma comment(lib, "NesClient_d.lib") // �ڵ���ģʽ�����ӵ��԰汾��NesClient��
#   else
#       pragma comment(lib, "NesClient.lib") // �ڷǵ���ģʽ�����ӷ����汾��NesClient��
#   endif
#endif

#ifndef WM_MOUSEWHEEL
#   define WM_MOUSEWHEEL 0x020A // ������������Ϣ
#endif
#ifndef WHEEL_DELTA
#   define WHEEL_DELTA 120 // ��������������
#endif

// ȫ�ֱ�����
static HINSTANCE        g_hGCInst = 0;		// Ӧ�ó���ʵ�����
//static TCHAR   g_szGCTitle[100]        = "FSO Client"; // ���ڱ���
//static TCHAR   g_szGCWindowClass[100] = "RT3D_FSO_CLIENT"; // ��������
G_MEMDEF(g_szGCTitle, 100)					// ����һ��100�ַ����ȵ�ȫ�ֱ����ַ���
G_MEMDEF(g_szGCWindowClass, 100)			// ����һ��100�ַ����ȵ�ȫ�ִ��������ַ���

static HWND             g_hGCWnd = 0;		// ���ھ��
static CRtgAppFrame* g_pGameClientFrame = 0;// ��Ϸ�ͻ��˿��ָ��
static RtIni* g_iniConfig;					// �����ļ�ָ�룬ָ�� "game.ini"����ָ�벻�ᱻɾ��
static CLogSender		g_LogSender;		// ��־������ʵ��

bool	g_bLogEnable = false;				// ��־�����Ƿ����õı�־

// ��yz��2010-6-11��ӣ������ڴ汣����ע�ắ��
void RegisterMemProtector();
// ����

// add [3/14/2009 tooth.shi]
//RtgDevice       *g_pDevice		= 0; // �豸ָ��
GcTimer* g_pTimer = 0;					 // ��ʱ��ָ��
//RtgDevice *g_render = 0;				 // ��Ⱦ�豸ָ��
CSceneMusicApp* g_pMusicThread = 0;		 // �����߳�ָ��

RtgDeviceD3D9* GetDevice()
{
	return RtGetRender();
}

GcTimer* GetTimer()
{
	if (g_pTimer == NULL)
		g_pTimer = RT_NEW GcTimer();// �����ʱ��Ϊ�գ��򴴽�һ���µ�GcTimerʵ��
	return g_pTimer;// ���ؼ�ʱ��ʵ��
}

HWND GetWndHandle()
{
	return g_hGCWnd;
}

CRtgAppFrame* GetApp()
{
	return g_pGameClientFrame;
}

RtIni* GetGameIni()
{
	return g_iniConfig;
}
bool AutoResize(void* sender, int i)
{
	guard;
	RtwWidget* pWidget = (RtwWidget*)sender;
	if (!pWidget)
		return false;
	RtwRect rc;
	rc = pWidget->GetClientRect();
	if (i == 0)
	{
		int nLength = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
		int nLineCount = nLength / 240 + 1;
		if (nLineCount > 1)
		{
			rc.right = rc.left + 240 - 1;
			rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() + RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
		}
		else
		{
			rc.right = rc.left + nLength + RtwTextBox::m_LeftSpace * 2 + 3;
			rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() + RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
		}
		pWidget->Move(rc);
		return true;
	}
	else if (i == 1)
	{
		int nLength = g_workspace.getFontManager()->GetStringWidth(pWidget->getText());
		int nLineCount = ((RtwTextBox*)pWidget)->getLineCount();
		rc.right = rc.left + 180;
		rc.bottom = rc.top + nLineCount * g_workspace.getFontManager()->getFontSize() + 6 * (nLineCount + 1) - 1;
		pWidget->Move(rc);
		return true;
	}
	else
		return false;
	unguard;
}
GcWorld* GetWorld()
{
	if (!g_pGameClientFrame || !((CGameClientFrame*)g_pGameClientFrame)->m_pWorld)
		return NULL;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld;
}

// ��ȡ��¼����ĺ���
GcLogin* GetLogin()
{
	if (!g_pGameClientFrame || !((CGameClientFrame*)g_pGameClientFrame)->m_pLogin)
		return NULL;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_pLogin;
}

GcPlayer* GetPlayer()
{
	if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL) return NULL;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_pPlayer;
}

GcPet* GetPlayerPet()
{
	if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL) return NULL;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_pGcPet;
}

NetSession* GetNet()
{
	if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL) return NULL;
	return &(((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->m_Net);
}

bool WorldIsInited()
{
	if (g_pGameClientFrame == NULL) return false;
	if (((CGameClientFrame*)g_pGameClientFrame)->m_pWorld == NULL) return false;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_pWorld->IsInit();
}

bool UIIsInited()
{
	if (g_pGameClientFrame == NULL) return false;
	return ((CGameClientFrame*)g_pGameClientFrame)->m_bUIInit != FALSE;
}

void ChangeGameFlow(bool bGame)
{
	if (((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow != (long)bGame)
	{
		((CGameClientFrame*)g_pGameClientFrame)->m_bNextGameFlow = bGame;
		((CGameClientFrame*)g_pGameClientFrame)->m_bChangeGameFlowNextFrame = TRUE;
	}
}

CLogSender* GetLogSender()
{
	return &g_LogSender;
}

//static char g_szLogServerHostIP[40] = "log.zfs.gamigo.com.cn";
G_MEMDEF(g_szLogServerHostIP, 40)
static int  g_iLogServerHostPort = LOGSERVER_DEFORT_PORT;

void GameClientGuardErrorCallBack(const char* szMsg)
{
	// �ָ���Ļ����
	RtgDevice::ResumeDefaultDisplayMode();

	// �ϴ�������ջ
	GetLogSender()->SendString("Error", (char*)szMsg);

#ifdef PUBLISH
	MessageBox(NULL, R(MSG_GAME_CRASH), g_szGCTitle, 0);
#else
	MessageBox(NULL, szMsg, g_szGCTitle, 0);
#endif
}

//static char g_szGuideServerHostIP[40] = "guide.zfs.gamigo.com.cn";
G_MEMDEF(g_szGuideServerHostIP, 40)
static int g_iGuideServerHostPort = 6620;

void GetGuideHostInfo(char** ppIP, int* piPort)
{
	*ppIP = g_szGuideServerHostIP;
	*piPort = g_iGuideServerHostPort;
}

DWORD WINAPI callbackAccelerateCheck(LPVOID lpThreadParameter)
{
	int iMax = 2;
	int iCur = 0;
	float fRate;
	DWORD dwOld, dwNew;
	while (1)
	{
		dwOld = rtGetMilliseconds();
		rtSleep(3.f);
		dwNew = rtGetMilliseconds();
		fRate = ((float)(dwNew - dwOld)) / 3000.f;
		if (fabs(fRate - 1.f) > 0.1f)
		{
#ifdef _DEBUG
			ERR2("Accelerated, Rate=%.3f, Count=%d\n", fRate, iCur);
#endif
			iCur++;
			if (iCur > iMax)
			{
#ifdef _DEBUG
				ERR("Hacker Deteched\n");
#endif
				if (s_isHacker == 0) s_isHacker = 1;
			}
		}
		else
		{
			iCur--;
			if (iCur < 0)
			{
				iCur = 0;
			}
		}
		//rtSleep(rtRandom(120, 60));
	}
}

void SendSystemInfo()
{
	//     RtSystemInfo sysInfo;
	//     if (sysInfo.Init())
	//     {
	//         GetLogSender()->SendSysInfo(sysInfo.GetSystemInfo());
	//     }
}

/*
bool DisableInfuser()
{
	DWORD dwNewFlg, dwOldFlg, dwWriteSize;
	unsigned char pNewMember[20] = {0, 0, 0, 0};
	LPVOID pEntryPoint;
	HANDLE hProcess;

	hProcess = GetCurrentProcess();
	dwNewFlg = PAGE_READWRITE;
	dwWriteSize = 8;

	// �޸������ڴ��
	pEntryPoint = (LPVOID)WriteProcessMemory; // �����ַ��������������ģ������滻�ҵĴ���

	// �޸��ڴ�����
	if (!VirtualProtectEx(hProcess, (LPVOID)pEntryPoint, dwWriteSize, dwNewFlg, &dwOldFlg))
	{
		return false;
	}

	// �����ҵĴ��뵽�����Ĵ�����, ��ַ���ҵĴ����е���ת
	if (!WriteProcessMemory(hProcess, pEntryPoint, &pNewMember, dwWriteSize, NULL))//&dwWrited)
	{
		return false;
	}

	// �ָ�����
	if (!VirtualProtectEx(hProcess, (LPVOID)pEntryPoint, dwWriteSize, dwOldFlg, &dwNewFlg))
	{
		return false;
	}
	return true;
}
*/

class CCcxyCoreFactory : public RtCoreFactory
{
public:
	RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")
		virtual void InitLogRoot(RtLogType rootLog)
	{
	}
};
RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

// Foward declarations of functions included in this code module:
ATOM				FSOClientMainRegisterClass(HINSTANCE hInstance);
BOOL				FSOClientInitInstance(HINSTANCE, int, long, long);
LRESULT CALLBACK	FSOClientWndProc(HWND, UINT, WPARAM, LPARAM);
//static char s_szDetechUsername[40] = "";
G_MEMDEF(s_szDetechUsername, 40)

// Ԥ��
#ifdef _PREVIEW
static void OpenConsole()
{
	// ���Է������̨
	if (AllocConsole()) {
		// ��ȡ����̨�ı�׼����������
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);  // ����׼����ض��򵽿���̨
		freopen_s(&fp, "CONIN$", "r", stdin);   // ����׼�����ض��򵽿���̨
		freopen_s(&fp, "CONOUT$", "w", stderr);  // ����׼�����ض��򵽿���̨

		std::cout << "Preview����̨�Ѵ�" << std::endl;
	}
	else {
		std::cerr << "�޷��������̨" << std::endl;
	}
}
static void CloseConsole()
{
	// �ͷſ���̨
	FreeConsole();
}
#endif
//lyymark 1.Game.Init ����main�������
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//GameClientGuardErrorCallBack("test error callback");
#ifdef _PREVIEW
	OpenConsole();
#endif
	// add by yz 2010-6-11:ע��ȫ���ڴ汣��
	RegisterMemProtector();
	//end

	CrashRP::Start();

#ifdef _PREVIEW
	if (!*lpCmdLine)
		lpCmdLine = "tooth.updaterun;127.0.0.1";
#endif

	int iResult = FALSE;
	rtRandomSeed(time(NULL));
	char* szMemoffset = RT_NEW char[((rtRandom() % 10) + 1) * 32];
	{
		guard_name(WinMain::Init);
		// ����������
		RtTextConfig cfgCommandLine(lpCmdLine);

		// ���Ŀ��ʼ��
#ifdef _DEBUG
		// ע�� [3/16/2009 tooth.shi]
//         rtCreateConsole(640, 480);
//         RtOutputDevice* pMyLog = NULL;
//         RtOutputDevice* pMyErr = NULL;
//         if (cfgCommandLine.GetValue("noconsole"))
//         {
//             pMyLog = (RtOutputDevice*)-1;
//             pMyErr = (RtOutputDevice*)-1;
//         }
//         if (cfgCommandLine.GetValue("nolog"))
//         {
//             pMyLog = (RtOutputDevice*)-3;
//         }
//         if (cfgCommandLine.GetValue("noerr"))
//         {
//             pMyErr = (RtOutputDevice*)-3;
//         }
// 		rtCoreInit("game_client", 0, pMyLog, pMyErr, 0);
		if (!rtCoreInit("clt_engine.ini"))
		{
			ShowMessage("Rt Core init failed.\nExit now!");
			return FALSE;
		}

		//rtCoreInit("game_client", 0, (RtOutputDevice*)-3, (RtOutputDeviceError*)-3, 0);
#else
		//rtCoreInit("game_client", 0, (RtOutputDevice*)-1, (RtOutputDeviceError*)-1, 0);
		// ע�� [3/16/2009 tooth.shi]
		//rtCoreInit("game_client", 0, (RtOutputDevice*)-3, (RtOutputDevice*)-3, 0);

		RtRuntimeClass* _CoreFactory = RT_RUNTIME_CLASS(CCcxyCoreFactory);

		if (strstr(lpCmdLine, "showlog;") == lpCmdLine)
		{
			lpCmdLine += strlen("showlog;");
			_CoreFactory = NULL;
		}

		g_bLogEnable = true;

		if (_CoreFactory)
		{
			g_bLogEnable = false;
		}

		if (!rtCoreInit("clt_engine.ini", _CoreFactory))
		{
			ShowMessage("Rt Core init failed.\nExit now!");
			return FALSE;
		}
		///!����Ҫ����memory dump
		RtMemoryAllocator::Instance().SetDumpMemLeak(false);

#endif
		// ע�� [3/16/2009 tooth.shi]

		// ����������
		CRtPackManager* pPackManager = RT_NEW CRtPackManager;			//ac.ma
		pPackManager->OpenPack("game_client.pak", false);

		/*
		try
		{
			DisableInfuser();
		}catch(...)
		{
		}
		*/

		CRtPackAndAnsiManager* pAllManager = RT_NEW CRtPackAndAnsiManager(pPackManager, &RtCoreFile());
		RtCore::Instance().pFileManager = pAllManager;
		//#include "filepack/normal_pack.h"
		//		CNormalPack *pPack = (CNormalPack *)pPackManager->PackManager()->GetPack("game_client.pak");
		//		pPack->GetEnd();
		long sizePack = pPackManager->FileSize("clt_engine.ini");
		//		CRtPackManager* pPackManager=new CRtPackManager;

		// 		char szPack[20];
		// 		char szTemp[20];
		// 		for(int i=1;i<10;i++)
		// 		{
		// 			strcpy(szPack,"data");
		// 			itoa(i,szTemp,10);
		// 			strcat(szPack,szTemp);
		// 			strcat(szPack,".pak");
		//
		// 			if(!(access(szPack,0)==-1))
		// 			{
		// 				pPackManager->OpenPack(szPack,false);
		// 			}
		// 		}

				//<add by fox for version>
				//��Ϸ���ʼ�������ļ�
		string ErrorString;
		if (!LoadConfig(ErrorString))
		{
			MessageBox(NULL, ErrorString.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}

		//lyymark 1.Game.Init �õ�"Strmap.csv"�ļ�
		const char* pStrmapPath = GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath");
		if (pStrmapPath == NULL)
		{
			MessageBox(NULL, "Can't find [strmap.csv] path !", "Error", MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}

		// �ַ��滻����
		InitMapString(pStrmapPath);
		//</add by fox for version>

		// ��Ϸ�����ļ�
		g_iniConfig = RT_NEW RtIni();
		if (g_iniConfig->OpenFile(R(INI_GAME)))
		{
			g_iniConfig->CloseFile();
		}
		else
		{
			MessageBox(NULL, "Error: Cannot open game setting file [game.ini]\n Please check if the directory is correct or re-install the game", "Error", MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}

		// ������Ϣ

		long lIniResWidth = 1024, lIniResHeight = 768, lIniResColorDepth = 32;
		long lIniFullscreen = 0;      // ȫ��ģʽ   ��
		long lIniTextureQuality = 0;  // ��ͼ����   ��
		long lIniEffectQuality = 1; // ��Ч����   ��
		long lIniShadowQuality = 1; // ��Ӱ����   ��
		long lIniFsaa = 1;          // ȫ������� ��
		long lIniEnvref = 1;        // ��������   ��
		long lIniObjref = 1;        // ��������   ��
		long lIniPosteffect = 1;    // ���ڴ���   ��
		long lIniObjeffect = 1;     // ���ڼ�ǿ   ��
		long lIniMusicVolume = 50;    // ��������   50%
		long lIniSoundVolume = 60;    // ��Ч����   60%
		long lUse3DAudio = 1;       // ��3D��Ч   ��
		long lCaptureVideo = 0;
		bool bBackGroundMusicIsSlience = false;
		bool bSoundIsSlience = false;
		//char szLanguage[100] = "language/chinese/strmap.csv";//rem by fox for version

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
		if (iniUser.OpenFile(R(INI_USER)))
		{
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

			//<rem by fox for version>
			//const char* pLang = GetGameIni()->GetEntry("game", "language");
			//if (pLang)
			//{
			//    sprintf(szLanguage, "language/%s/strmap.csv", pLang);
			//}
			//</rem by fox for version>

			iniUser.CloseFile();
		}

		// �ַ��滻����
//        InitMapString(szLanguage);//rem by fox for version

#ifndef _DEBUG
		//if (!cfgCommandLine.GetValue("tooth.updaterun"))
		if (std::string::npos == ((std::string)lpCmdLine).find("tooth.updaterun"))
		{
			MessageBox(NULL, R(MSG_RUNEXE_FSO), R(G_INFO), MB_OK | MB_ICONINFORMATION);
			DEL_ARRAY(szMemoffset);
			return 0;
		}
#endif

		// ȡ���򵼷�������IP��ַ�Ͷ˿ڣ�����ȥȡ��GameWorld�������б�
		const char* szGuideIP, * szGuidePort;
		int semicolon = ((std::string)lpCmdLine).find(";");
		std::string xx = ((std::string)lpCmdLine).substr(semicolon + 1, sizeof(lpCmdLine) - semicolon);
		szGuideIP = xx.c_str();
		//if (!cfgCommandLine.GetValue("ip", &szGuideIP))
		if (szGuideIP == "")
		{
			szGuideIP = GetGameIni()->GetEntry("net", "GuideIP");
			if (szGuideIP == NULL) szGuideIP = R(RES_DEFAULT_GUIDE_SERVER_IP);
		}
		if (!cfgCommandLine.GetValue("port", &szGuidePort))
		{
			szGuidePort = GetGameIni()->GetEntry("net", "GuidePort");
			if (szGuidePort == NULL) szGuidePort = R(RES_DEFAULT_GUIDE_SERVER_PORT);
		}
		if (szGuideIP) { rt2_strncpy(g_szGuideServerHostIP, szGuideIP, 40); /*g_szGuideServerHostIP[39] = 0;*/ }
		if (szGuidePort) g_iGuideServerHostPort = atoi(szGuidePort);
		GcLogin::StartGetGameWorldServer();

		// ȡ��Log Server��IP��ַ�Ͷ˿�
		const char* szLogIP, * szLogPort;
		if (!cfgCommandLine.GetValue("LogServerIP", &szLogIP))
		{
			szLogIP = GetGameIni()->GetEntry("net", "LogServerIP");
			if (szLogIP == NULL) szLogIP = R(RES_DEFAULT_LOG_SERVER_IP);
		}
		if (!cfgCommandLine.GetValue("LogServerPort", &szLogPort))
		{
			szLogPort = GetGameIni()->GetEntry("net", "LogServerPort");
			if (szLogPort == NULL) szLogPort = R(RES_DEFAULT_LOG_SERVER_PORT);
		}
		if (szLogIP) rt2_strncpy(g_szLogServerHostIP, szLogIP, 40);
		if (szLogPort) g_iLogServerHostPort = atoi(szLogPort);
		rtSetGuardErrorCallback(GameClientGuardErrorCallBack);

		// LogSender��ʼ��
		if (!GetLogSender()->Init(g_szLogServerHostIP, g_iLogServerHostPort))
		{
			ERR("Init LogSender Failed, Ignore.\n");
		}

		ReadVersionFile(R(INI_VERSION));

		// ����ע�������ǵ�һ�����У��ͷ���PC Info
		/*CGameRegistry WinReg;
		WinReg.Init();
		if (!iniUser.GetEntry("game", "log"))
		{
			if (iniUser.OpenFile(R(INI_USER), true))
			{
				if (!iniUser.FindSection("game"))
				{
					iniUser.AddSection("game");
				}
				iniUser.AddEntry("game", "log", (long)1);
				iniUser.CloseFile();
				SendSystemInfo();
			}
		}*/

		// ���ڳ�ʼ��
		// ע�� [3/16/2009 tooth.shi]
//         srand(time(NULL));
//         FSOClientMainRegisterClass(hInstance);
//         if (!FSOClientInitInstance(hInstance, nCmdShow, lIniResWidth, lIniResHeight))
//         {
//             ShowMessage(R(MSG_CANNOTCREATE_WND));
//             return FALSE;
//         }

//         // ����������
//         g_pGameClientFrame = new CGameClientFrame();
//         if (g_pGameClientFrame==NULL)
//         {
//             ShowMessage(R(MSG_MEMORY_FAIL));
//             return FALSE;
//         }
//
//         if (lCaptureVideo)
//         {
//             ((CGameClientFrame*)g_pGameClientFrame)->m_bCanCaptureVideo = true;
//         }

		// ͼ��ϵͳ��ʼ��
		RTGDeviceType eDeviceType;

		eDeviceType = RTG_DEVICE_D3D9;

		if (!rtGraphInit(/*"RT3D-Engine", 0, eDeviceType*/))// ע�� [3/16/2009 tooth.shi]
		{
			ShowMessage(R(MSG_CANNOTINIT_GRAPH));
			return FALSE;
		}

		if (!GetDevice())
		{
			ShowMessage(R(MSG_CANNOTINIT_GRAPH));
			return FALSE;
		}

		//lyymark 1.Game.Init ������Ϸ�ĳ�ʼ����� ͼ�α������ü���
		if (!GetDevice()->Init(hInstance, RT_RUNTIME_CLASS(CGameClientFrame),
			RT_RUNTIME_CLASS(GcCamera), "clt_graph.ini", "user.ini"))// change form GcCamera to RtgCameraEditor [3/24/2009 tooth.shi]
		{
			ShowMessage(R(MSG_CANNOTINIT_GRAPH));
			//return FALSE;
		}

		// ����ϵͳ��ʼ��,ͼ��ϵͳ�ĳ�ʼ����Ҫʹ�õ�����

		if (!rtSoundInit("RT3D-Audio", (HWND)GetDevice()->GetWndHandle()))
		{
			ShowMessage(R(MSG_CANNOTINIT_AUDIO));
			return FALSE;
		}
		if (!rtMusicInit())
		{
			ShowMessage(R(MSG_CANNOTINIT_AUDIO));
			return FALSE;
		}
		g_pBackMusic->SetMusicPath("music");
		g_pSoundMgr->SetAudioPath("audio");
		// lyymark 1.Game.Init ���������߳�
		g_pMusicThread = RT_NEW CSceneMusicApp();
		if (g_pMusicThread)
		{
			g_pMusicThread->Start();
		}

		//// �򿪱�������
		//const char* szMusicFileName = GetGameIni()->GetEntry("Audio", "LoginMusic");
		//if (szMusicFileName)
		//{
		//	g_pBackMusic->Play(szMusicFileName, true);
		//	//g_pMusicThread->Play(szMusicFileName, false, 30);
		//}

		// ����������
		g_pGameClientFrame = ((CGameClientFrame*)GetDevice()->GetEvent());
		g_pGameClientFrame->EnableNotifyOnMouseMove(true);
		if (g_pGameClientFrame == NULL)
		{
			ShowMessage(R(MSG_MEMORY_FAIL));
			return FALSE;
		}

		if (lCaptureVideo)
		{
			((CGameClientFrame*)g_pGameClientFrame)->m_bCanCaptureVideo = true;
		}

#ifdef _DEBUG
		{
			long lTotalTextureMem;
			if (GetGameIni()->GetEntry("Graph", "TotalTextureMem", &lTotalTextureMem))
			{
				//GetDevice()->m_Texture.SetTotalTextureMem(lTotalTextureMem);
			}
		}
#endif

		long lMSWindowed = 0;
		long lMSFullscreen = 0;
		if (lIniFsaa)
		{
			GetGameIni()->GetEntry("Graph", "MultiSampleTypeWindowed", &lMSWindowed);
			GetGameIni()->GetEntry("Graph", "MultiSampleTypeFullscreen", &lMSFullscreen);
		}
		// ע�� [3/16/2009 tooth.shi]
//         g_pDevice->m_dwMaxMultiSampleTypeWindowed = lMSWindowed;
//         g_pDevice->m_dwMaxMultiSampleTypeFullscreen = lMSFullscreen;
//
//         g_pDevice->BindViewWindow(RTGVWM_WINDOWED, g_hGCWnd);
//         g_pDevice->BindViewWindow(RTGVWM_TOPWINDOW, g_hGCWnd);
//         g_pDevice->SetViewWindowInfo(RTGVWM_WINDOWED, lIniResWidth, lIniResHeight, lIniResColorDepth, 0);
//         g_pDevice->SetViewWindowInfo(RTGVWM_TOPWINDOW, lIniResWidth, lIniResHeight, lIniResColorDepth, 0);
		if (lIniFullscreen)
		{
			GetDevice()->SetViewMode(RTGVWM_TOPWINDOW);
		}
		//         g_pDevice->Init(g_pGameClientFrame, new GcCamera, hInstance);
		//         g_pDevice->LockFps(30);
		//
		//         // ����ϵͳ��ʼ��
		// 		if (!rtAudioInit("RT3D-Engine", g_hGCWnd))
		// 		{
		// 			ShowMessage(R(MSG_CANNOTINIT_AUDIO));
		// 			return FALSE;
		// 		}
		g_pBackMusic->SetMusicPath("music");
		g_pSoundMgr->SetAudioPath("audio");

		// ����ϵͳ��ʼ��
		if (!ActorInit())
		{
			CHECKEX(R(MSG_CANNOTINIT_ACTOR));
		}

		RtcGetActorManager()->AddSearchPath("creature/actor/");
		RtcGetActorManager()->AddSearchPath("creature/material/");
		RtcGetActorManager()->AddSearchPath("creature/effect/");
		RtcGetActorManager()->AddSearchPath("scene/actor/");
		RtcGetActorManager()->AddSearchPath("scene/material/");
		RtcGetActorManager()->AddSearchPath("scene/effect/");

		// ����ϵͳ��ʼ��
		if (!rtSceneInit("RT3D-Scene"))
		{
			CHECKEX(R(MSG_CANNOTINIT_SCENE));
		}
		g_pScene = RT_NEW GcScene;
		if (!g_pScene->OnceInit(GetDevice()))
		{
			CHECKEX("ERROR: Scene initialization fail!");
			return false;
		}

		ActorSetScene(g_pScene);

		// add [3/16/2009 tooth.shi]
		if (!((CGameClientFrame*)g_pGameClientFrame)->OnEnterLogin())
		{
			return false;
		}

		// ��ʼ��d3d������Ⱦ
		GetDevice()->RenderScene();

#ifdef USE_LUA_CHECK
		LuaInit();
#endif

		// ��ӡע����ͼ
#ifdef _DEBUG
		// ע�� [3/16/2009 tooth.shi]	�ο�	rtDumpObjectClass
		//RtRegistrationClass::DumpObjectClass(g_pLog);
#endif

		// �û�ini��ʼ��
		GetDevice()->GetTextMgr()->SetTextureQuality(lIniTextureQuality * 2);
		EActorQuality eqActor, eqSkin, eqMtl, eqEffect;
		switch (lIniEffectQuality)
		{
		case 2: eqEffect = QUALITY_LOW; break;
		default:
		case 1: eqEffect = QUALITY_MIDDLE; break;
		case 0: eqEffect = QUALITY_HIGH; break;
		}
		eqActor = eqSkin = eqMtl = eqEffect;
		ActorSetQuality(eqActor, eqSkin, eqMtl, eqEffect);
		switch (lIniShadowQuality)
		{
		case 2: GcRenderActorBase::s_iShadowQuality = 1; break;
		default:
		case 1: GcRenderActorBase::s_iShadowQuality = 2; break;
		case 0: GcRenderActorBase::s_iShadowQuality = 3; break;
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
		if (!GetGameIni()->GetEntry("Graph", "Console", &lCanShowConsole))
		{
			lCanShowConsole = 0;
		}
		g_pGameClientFrame->ShowConsole(lCanShowConsole != 0);
#endif

		// Ӧ�ó����ܳ�ʼ��
		if (!g_pGameClientFrame->Init(GetDevice()/*hInstance, g_hGCWnd, g_hGCWnd*/))
		{
			ShowMessage(R(MSG_CANNOTINIT_GAME));
			return FALSE;
		}

		int start = ((std::string)lpCmdLine).find(";");
		start = ((std::string)lpCmdLine).find(" ", start + 1);
		int end = ((std::string)lpCmdLine).find(" ", start + 1);
		if (std::string::npos != end)
		{
			std::string xx = ((std::string)lpCmdLine).substr(start + 1, end - start - 1);
			GetLogin()->m_hallName = xx;
			start = end + 1;
			xx = ((std::string)lpCmdLine).substr(start);
			GetLogin()->m_hallKey = xx;
			if (!GetLogin()->m_hallName.empty() && !GetLogin()->m_hallKey.empty())
			{
				GetLogin()->m_ishallLogin = true;
			}
		}

#ifndef _DEBUG
		//CREATE_THREAD(callbackAccelerateCheck, 0);
#endif
		finalunguard;
		}

	{
		LOG2("\n\n"
			"============== App Start: %s"
			"                  Cmd = %s\n\n", rtTimestamp(), lpCmdLine);

		// ִ�г���
		guard_name(WinMain::Run);
		//         iResult = g_pGameClientFrame->Run();
		//lyymark 1.Game.Init ������Ϸ���棬�ͻ�����ѭ�����
		iResult = GetDevice()->RunEngine();// change [3/25/2009 tooth.shi]
		finalunguard;

		LOG1("\n\n============== App End: %s\n\n", rtTimestamp());
	}

	//#ifndef _DEBUG
	//	const char* szBug ="http://hd.wtgame.net/x/bug/";
	//	std::string szCmd = "explorer.exe \"";
	//	szCmd += szBug;
	//	szCmd += "\"";
	//	WinExec(szCmd.c_str(), SW_SHOW);
	//#endif

	{
		guard_name(WinMain::Exit);

	
		GcLogin::ms_lGameWorldServerCount = 0;

		// Ӧ�ó������˳�
		g_pGameClientFrame->Close();

		// ����ϵͳ�˳�
		LOG("Exit Pre Scene\n");
		rtScenePreExit();

		LOG("Exit Scene\n");
		rtSceneExit();

		// ����ϵͳ�˳�
		LOG("Exit Actor\n");
		ActorExit();

		g_pMusicThread->Terminate(0);
		DEL_ONE(g_pMusicThread);
		g_pMusicThread = NULL;

		// ����ϵͳ�˳�
		LOG("Exit Pre Audio\n");
		rtAudioPreExit();
		LOG("Exit Audio\n");
		rtAudioExit();

		// ͼ��ϵͳ�˳�
		LOG("Exit Graph\n");
		rtGraphExit();

		// ɾ��Ӧ�ó�����
		LOG("Exit Frame\n");
		DEL_ONE(g_pGameClientFrame);

		ClearMapString();

		// ���Ŀ��˳�
		LOG("Exit Core\n");
		rtCoreExit();
#ifdef _PREVIEW
		FreeConsole();
#endif
		finalunguard;
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
	switch (iResult)
	{
	case 22:
	{
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
	}
	break;
	case 23:
	{
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
	}
	break;
	default:
	{
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
	}
	break;
	}

	CrashRP::Stop();
	return iResult;
	//guard_end;// ע�� [3/16/2009 tooth.shi]
	return 0;
	}

ATOM FSOClientMainRegisterClass(HINSTANCE hInstance)
{
	guard;
	g_szGCWindowClass[rand() % (int)strlen(g_szGCWindowClass)] = '0' + rand() % 10;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)FSOClientWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON_MAIN);
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName	= (LPCSTR)IDC_AZ3D_TEST1;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = g_szGCWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON_MAIN);
	wcex.hIconSm = 0;
	return RegisterClassEx(&wcex);
	finalunguard;
}

BOOL FSOClientInitInstance(HINSTANCE hInstance, int nCmdShow, long lCreateWidth, long lCreateHeight)
{
	guard;
	g_hGCInst = hInstance; // Store instance handle in our global variable

	DWORD dwCreateStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE; // | WS_THICKFRAME ;
	DWORD dwCreateLeft = 0;
	DWORD dwCreateTop = 0;

	//GetGameIni()->GetEntry("Graph", "WindowSizeX", &lCreateWidth);
	//GetGameIni()->GetEntry("Graph", "WindowSizeY", &lCreateHeight);

	RECT rc;
	SetRect(&rc, dwCreateLeft, dwCreateTop, lCreateWidth + dwCreateLeft - 1, lCreateHeight + dwCreateTop - 1);
	AdjustWindowRect(&rc, dwCreateStyle, FALSE);

	rc.right += dwCreateLeft - rc.left;
	rc.left = dwCreateLeft;
	rc.bottom += dwCreateTop - rc.top;
	rc.top = dwCreateTop;

	rt2_strncpy(g_szGCTitle, R(G_GAME_TITLE), 100);
#ifdef _DEBUG
	rt2_strcat(g_szGCTitle, " Debug [");
#else
	rt2_strcat(g_szGCTitle, " [");
#endif
	int iB = strlen(g_szGCTitle);
	rtMD5FileString((char*)rtExePathName(), g_szGCTitle + iB);
	rt2_strcat(g_szGCTitle, "]");
	srand(time(NULL) / 86400);
	g_szGCTitle[iB + 8 + rand() % 8] = '0' + rand() % 10; // ����Ĵ������֣�����ֱ����������

	g_hGCWnd = CreateWindow(g_szGCWindowClass, g_szGCTitle, dwCreateStyle,
		rc.left, rc.top, rc.right - rc.left + 1, rc.bottom - rc.top + 1, NULL, NULL, hInstance, NULL);

	if (!g_hGCWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hGCWnd, nCmdShow);
	UpdateWindow(g_hGCWnd);

	return TRUE;
	finalunguard;
}

//   FUNCTION: FSOClientWndProc(HWND, unsigned, WORD, LONG)
//
//   PURPOSE:  Processes messages for the main window.
//
//   WM_COMMAND	- process the application menu
//   WM_PAINT	- Paint the main window
//   WM_DESTROY	- post a quit message and return

// ע�� [3/16/2009 tooth.shi]
LRESULT CALLBACK FSOClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	guard;
	static POINT ptMouse, ptCurPos;
	static bool bMouseCapture = false;
	static int iSendQuitMsgRate = 0;
	static DWORD iIsQuited = 0;
	if (GetWindowLong(hWnd, GWL_WNDPROC) != (LONG)FSOClientWndProc)
	{
		if (s_isHacker == 0) s_isHacker = 3;
	}
	if (s_isHacker)
	{
#ifdef _DEBUG
		ERR("Hacker Detected\n");
#endif
		iSendQuitMsgRate = rtRandom(5, 3);
		rt2_strncpy(s_szDetechUsername, GcLogin::m_szAccountUsername.substr(0, 39).c_str(), 40);
		//s_szDetechUsername[39] = 0;
	}
	if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST)
	{
		ptCurPos.x = (short)LOWORD(lParam);
		ptCurPos.y = (short)HIWORD(lParam);
	}
	switch (message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_SCREENSAVE)
		{
			return TRUE;
		}
		break;
	case WM_CHAR:
		g_pGameClientFrame->OnKeyChar(lParam, wParam);
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:				//��ť����
		g_pGameClientFrame->OnKeyDown(lParam, wParam);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:					//��ť����
		g_pGameClientFrame->OnKeyUp(lParam, wParam);
		break;
	case WM_LBUTTONDOWN:			//������������
		ptMouse = ptCurPos;
		//         g_pGameClientFrame->MouseLDown(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_MBUTTONDOWN:
		ptMouse = ptCurPos;
		//         g_pGameClientFrame->MouseMDown(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_RBUTTONDOWN:			//����Ҽ�������
		ptMouse = ptCurPos;
		//         g_pGameClientFrame->MouseRDown(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_LBUTTONUP:				//����������
		//         g_pGameClientFrame->MouseLUp(wParam, ptCurPos.x, ptCurPos.y);
		if (GetCapture()) ReleaseCapture();
		break;
	case WM_MBUTTONUP:
		//         g_pGameClientFrame->MouseMUp(wParam, ptCurPos.x, ptCurPos.y);
		if (GetCapture()) ReleaseCapture();
		break;
	case WM_RBUTTONUP:				//����Ҽ�������
		//         g_pGameClientFrame->MouseRUp(wParam, ptCurPos.x, ptCurPos.y);
		if (GetCapture())
		{
			RECT rt;
			GetClientRect(hWnd, &rt);
			ReleaseCapture();
			if (!PtInRect(&rt, ptCurPos))
				return 0;
		}
		break;
	case WM_LBUTTONDBLCLK:			//������˫��
		//       g_pGameClientFrame->MouseLDClick(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_MBUTTONDBLCLK:
		//         g_pGameClientFrame->MouseMDClick(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_RBUTTONDBLCLK:			//����Ҽ�˫��
		//         g_pGameClientFrame->MouseRDClick(wParam, ptCurPos.x, ptCurPos.y);
		break;
	case WM_MOUSEWHEEL:
		//         g_pGameClientFrame->MouseWheel((short)LOWORD(wParam), ((short)HIWORD(wParam))/WHEEL_DELTA,ptCurPos.x,ptCurPos.y);
		break;
	case WM_MOUSEMOVE:		//����ƶ���Ϣ
		if (wParam & MK_LBUTTON)
		{
			//             g_pGameClientFrame->MouseLDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
			SetCapture(hWnd);
		}
		else if (wParam & MK_MBUTTON)
		{
			//             g_pGameClientFrame->MouseMDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
			SetCapture(hWnd);
		}
		else if (wParam & MK_RBUTTON)
		{
			//             g_pGameClientFrame->MouseRDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
			SetCapture(hWnd);
		}
		else
		{
			//             g_pGameClientFrame->MouseMove(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
		}
		ptMouse = ptCurPos;
		break;
	case WM_QUIT:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SETCURSOR:
		return 0;
	}

	if (iSendQuitMsgRate)
	{
		if (rtRandom(iSendQuitMsgRate, 0) == 0)
		{
			iIsQuited = rtGetMilliseconds();
			PostQuitMessage(23);
			return 0;
		}
	}
	if (iIsQuited)
	{
		if ((rtGetMilliseconds() - iIsQuited) > 10000)
		{
#ifdef _DEBUG
			CHECKEX("�����Ѿ�������Ϣ�˳�[10��]�����ǻ����յ���Ϣ");
#endif
			/*    delete g_pDevice;
				g_pDevice = 0;*/
		}
		}

	if (GetDevice()) return GetDevice()->DeviceMsgProc(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
	finalunguard;
	}

//add by yz 2010-6-11:ȫ���ڴ汣��ע��
void RegisterMemProtector()
{
#ifdef ENABLE_OPERATOR_NEW
	rt2_ActiveSafestr();
#endif

	RegisterSharedMemProtector();

	G_MEMPROTECTOR(g_szGCTitle, 100)
		rt2_strncpy(g_szGCTitle, "FSO Client", 100);
	G_MEMPROTECTOR(g_szGCWindowClass, 100)
		rt2_strncpy(g_szGCWindowClass, "RT3D_FSO_CLIENT", 100);

	G_MEMPROTECTOR(g_szLogServerHostIP, 40)
		rt2_strncpy(g_szLogServerHostIP, "log.zfs.gamigo.com.cn", 40);
	G_MEMPROTECTOR(g_szGuideServerHostIP, 40)
		rt2_strncpy(g_szGuideServerHostIP, "guide.zfs.gamigo.com.cn", 40);
	G_MEMPROTECTOR(s_szDetechUsername, 40)

		G_MEMPROTECTOR(s_szRegionHost, 80)          //gc_global.cpp
		G_MEMPROTECTOR(s_szStringTemp, STRING_LENGTH)

		G_MEMPROTECTOR(s_strCheckProcessName, 200)  //gc_world.cpp
}
//end

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

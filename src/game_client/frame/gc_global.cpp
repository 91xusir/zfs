
#include "gc_include.h"
#include "ui_form_msg.h"

/*----------------------------------------------------------------------------
-   全局.
----------------------------------------------------------------------------*/

int s_isHacker = 0; // 1 加速器, 2 按键拦截, 3 窗口消息拦截
std::list<int>  s_listCallStack;

static long s_lRegionCharID;
static long s_lRegionSeed;
//static char s_szRegionHost[80];
G_MEMDEF(s_szRegionHost, 80)
static short s_sRegionPort;

void SetRegionServerInfo(long lCharID, long lSeed, const char* szRegionHost, short sRegionPort)
{
    s_lRegionCharID = lCharID;
    s_lRegionSeed   = lSeed;
    s_sRegionPort   = sRegionPort;
    rt2_strncpy(s_szRegionHost, szRegionHost, 80);
}

void GetRegionServerInfo(long& lCharID, long& lSeed, char** szRegionHost, short& sRegionPort)
{
    lCharID = s_lRegionCharID;
    lSeed   = s_lRegionSeed;
    sRegionPort = s_sRegionPort;
    *szRegionHost = s_szRegionHost;
}

//const int STRING_LENGTH=1024;
//static char s_szStringTemp[STRING_LENGTH];
G_MEMDEF(s_szStringTemp, STRING_LENGTH)

void ShowErr(const char* szContent)
{
	return ShowMessage(szContent,R(G_ERROR));
}

void ShowErr(const char* szContent1,short vContent2)
{
	rt2_sprintf(s_szStringTemp,"%s [%d] \n",szContent1,vContent2);
	return ShowMessage(s_szStringTemp,R(G_ERROR));
}

void ShowErr(const char* szContent1,const char* szContent2)
{
	rt2_sprintf(s_szStringTemp,"%s [%s] \n",szContent1,szContent2);
	return ShowMessage(s_szStringTemp,R(G_ERROR));
}


void ShowErr(const char* szContent1,const char* szContent2,const char* szContent3)
{
	rt2_sprintf(s_szStringTemp,szContent1,szContent2,szContent3);
	return ShowMessage(s_szStringTemp,R(G_ERROR));
}

void ShowMessage(const char* szContent, const char* szCaption)
{
    if (!UIIsInited())
    {
        HWND hWnd = NULL;
        if (GetApp())
        {
            hWnd = (HWND)(GetApp()->GetWndHandle());
        }
        if (szCaption==0) szCaption = R(G_ERROR);
        MessageBox(hWnd, szContent, szCaption, MB_ICONINFORMATION);
    }else
    {
		UIFormMsg::ShowStatic(szContent, UIFormMsg::TYPE_OK, true, "ErrMsgBox",false);
    }
}

// iLevel 0普通级别, 10顶级(会弹出Dialog)
void PlayerFailOperate(const char* szOutputString,bool vIgnoreRepeated, const char* szSoundFile, int iLevel)
{
    RtgVertex3 pos;
    if (szSoundFile)
    {
        g_pSoundMgr->PlayOnce(szSoundFile, false, 0, pos);
    }else
    {
        g_pSoundMgr->PlayOnce("ui_fail.wav", false, 0, pos);
    }
    if (iLevel==10)
    {
        ShowMessage(szOutputString, R(G_FAIL));
    }else
    {
        if (szOutputString)
        {
            if (WorldIsInited())
            {
                GetWorld()->m_Chat.SystemMessage(szOutputString,vIgnoreRepeated);
            }
        }
    }
}

void ShowBattleInfoMessage(const char* szOutputMessage)
{
	GetWorld()->m_Chat.BattleInfoMessage(szOutputMessage);
}
// 显示系统信息
void ShowSystemMessage(const char* szOutputMessage,bool vIgnoreRepeated)
{
    GetWorld()->m_Chat.SystemMessage(szOutputMessage,vIgnoreRepeated);
}

// 显示GM的信息
void ShowGameMasterMessage(const char* szOutputMessage)
{
    ShowSystemMessage(szOutputMessage);
}

// 显示重要信息
void ShowImportantMessage(const char* szOutputMessage)
{
    ShowSystemMessage(szOutputMessage);
}

// 显示提示信息, 简单信息, 详细信息
void ShowHelperMessage(const char* szSimpleMsg, const char* szDetailedMsg)
{
    ShowSystemMessage(szSimpleMsg);
}

/*----------------------------------------------------------------------------
-   版本信息
----------------------------------------------------------------------------*/

static std::string s_strVersion = "未知版本";
static std::string s_strPrimaryVersion = "未知";
static std::string s_strSecondaryVersion = "未知";
static long s_lVersionNumFirst = 0;
static long s_lVersionNumSecondly = 0;

bool ReadVersionFile(const char* szFilename)
{
    RtIni ini;
    if (ini.OpenFile(szFilename))
    {
        s_strPrimaryVersion   = ini.GetEntry("Version", "PrimaryVersion");
        s_strSecondaryVersion = ini.GetEntry("Version", "SecondaryVersion");
        ini.GetEntry("Version", "VersionNumFirst",    &s_lVersionNumFirst);
        ini.GetEntry("Version", "VersionNumSecondly", &s_lVersionNumSecondly);
        ini.CloseFile();

        rt2_sprintf(g_strStaticBuffer, "%s %d.%02d", s_strSecondaryVersion.c_str(), s_lVersionNumFirst, s_lVersionNumSecondly);
        s_strVersion = g_strStaticBuffer;
        return true;
    }
    return false;
}

const char* GetPrimaryVersion()
{
    return s_strPrimaryVersion.c_str();
}

const char* GetSecondaryVersion()
{
    return s_strSecondaryVersion.c_str();
}

int GetVersionNumFirst()
{
    return s_lVersionNumFirst;
}

int GetVersionNumSecondly()
{
    return s_lVersionNumSecondly;
}

const char* GetGameVersion()
{
    return s_strVersion.c_str();
}
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

void ReadHtml(const char* szOutPutHtmlString,int szHintWitch)
{
	guard;
	g_workspace.SetHintText(szOutPutHtmlString, true);
	g_workspace.SetHintSize(szHintWitch, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}
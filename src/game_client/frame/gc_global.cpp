
#include "gc_include.h"
#include "ui_form_msg.h"
#include <iosfwd>
#include <string>

/*----------------------------------------------------------------------------
-   ȫ��.
----------------------------------------------------------------------------*/

int            s_isHacker = 0;   // 1 ������, 2 ��������, 3 ������Ϣ����
std::list<int> s_listCallStack;  // ������ö�ջ���ڵ���

static long s_lRegionCharID;
static long s_lRegionSeed;
//static char s_szRegionHost[80];
G_MEMDEF(s_szRegionHost, 80)
static short s_sRegionPort;

void SetRegionServerInfo(long lCharID, long lSeed, const char* szRegionHost, short sRegionPort) {
    s_lRegionCharID = lCharID;
    s_lRegionSeed   = lSeed;
    s_sRegionPort   = sRegionPort;
    rt2_strncpy(s_szRegionHost, szRegionHost, 80);
}

void GetRegionServerInfo(long& lCharID, long& lSeed, char** szRegionHost, short& sRegionPort) {
    lCharID       = s_lRegionCharID;
    lSeed         = s_lRegionSeed;
    sRegionPort   = s_sRegionPort;
    *szRegionHost = s_szRegionHost;
}

//const int STRING_LENGTH=1024;
//static char s_szStringTemp[STRING_LENGTH];
G_MEMDEF(s_szStringTemp, STRING_LENGTH)

void ShowErr(const char* szContent) {
    return ShowMessage(szContent, R(G_ERROR));
}

void ShowErr(const char* szContent1, short vContent2) {
    rt2_sprintf(s_szStringTemp, "%s [%d] \n", szContent1, vContent2);
    return ShowMessage(s_szStringTemp, R(G_ERROR));
}

void ShowErr(const char* szContent1, const char* szContent2) {
    rt2_sprintf(s_szStringTemp, "%s [%s] \n", szContent1, szContent2);
    return ShowMessage(s_szStringTemp, R(G_ERROR));
}

void ShowErr(const char* szContent1, const char* szContent2, const char* szContent3) {
    rt2_sprintf(s_szStringTemp, szContent1, szContent2, szContent3);
    return ShowMessage(s_szStringTemp, R(G_ERROR));
}

void ShowMessage(const char* szContent, const char* szCaption) {
    if (!IsUIInited()) {
        HWND hWnd = NULL;
        if (GetApp()) {
            hWnd = (HWND)(GetApp()->GetWndHandle());
        }
        if (szCaption == 0)
            szCaption = R(G_ERROR);
        MessageBox(hWnd, szContent, szCaption, MB_ICONINFORMATION);
    } else {
        //lyymark 1.Global.UI.ShowErrorMessage
        UIFormMsg::ShowStatic(szContent, UIFormMsg::TYPE_OK, true, "ErrMsgBox", false);
    }
}

// iLevel 0��ͨ����, 10����(�ᵯ��Dialog)
void PlayerFailOperate(const char* szOutputString, bool vIgnoreRepeated, const char* szSoundFile,
                       int iLevel) {
    RtgVertex3 pos;
    if (szSoundFile) {
        g_pSoundMgr->PlayOnce(szSoundFile, false, 0, pos);
    } else {
        g_pSoundMgr->PlayOnce("ui_fail.wav", false, 0, pos);
    }
    if (iLevel == 10) {
        ShowMessage(szOutputString, R(G_FAIL));
    } else {
        if (szOutputString) {
            if (WorldIsInited()) {
                GetWorld()->m_Chat.SystemMessage(szOutputString, vIgnoreRepeated);
            }
        }
    }
}

void ShowBattleInfoMessage(const char* szOutputMessage) {
    GetWorld()->m_Chat.BattleInfoMessage(szOutputMessage);
}

// ��ʾϵͳ��Ϣ
void ShowSystemMessage(const char* szOutputMessage, bool vIgnoreRepeated) {
    GetWorld()->m_Chat.SystemMessage(szOutputMessage, vIgnoreRepeated);
}

// ��ʾGM����Ϣ
void ShowGameMasterMessage(const char* szOutputMessage) {
    ShowSystemMessage(szOutputMessage);
}

// ��ʾ��Ҫ��Ϣ
void ShowImportantMessage(const char* szOutputMessage) {
    ShowSystemMessage(szOutputMessage);
}

// ��ʾ��ʾ��Ϣ, ����Ϣ, ��ϸ��Ϣ
void ShowHelperMessage(const char* szSimpleMsg, const char* szDetailedMsg) {
    ShowSystemMessage(szSimpleMsg);
}

/*----------------------------------------------------------------------------
-   �汾��Ϣ
----------------------------------------------------------------------------*/

static std::string s_strVersion = "δ֪�汾";
static std::string VersionName  = "";
static std::string VersionNum   = "";

bool VersionInit() {
    if (GetGameIni()) {
        VersionName  = GetGameIni()->GetEntry("Version", "VersionName");
        VersionNum   = GetGameIni()->GetEntry("Version", "VersionNum");
        s_strVersion = VersionName + " " + VersionNum;
        return true;
    }
    return false;
}

// �Ƚϰ汾�ŵĺ���
bool isVersionOlderThan(const std::string& oldVersion, const std::string& newVersion) {
    std::istringstream oldStream(oldVersion), newStream(newVersion);
    int                oldNum, newNum;
    while (true) {
        if (!(oldStream >> oldNum))
            oldNum = 0;
        if (!(newStream >> newNum))
            newNum = 0;
        if (oldNum < newNum)
            return true;
        if (oldNum > newNum)
            return false;
        if (oldStream.peek() == '.')
            oldStream.ignore();
        if (newStream.peek() == '.')
            newStream.ignore();
        if (oldStream.eof() && newStream.eof())
            break;
    }
    return false;
}

const char* GetGameVersion() {
    return s_strVersion.c_str();
}

const std::string& GetGameVersionNum() {
    return VersionNum;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

void ReadHtml(const char* szOutPutHtmlString, int szHintWitch) {
    guard;
    g_workspace.SetHintText(szOutPutHtmlString, true);
    g_workspace.SetHintSize(szHintWitch, 30);
    g_workspace.AutoHintSize();
    g_workspace.AutoHintPosition();
    unguard;
}



/*----------------------------------------------------------------------------
-   全局函数.
----------------------------------------------------------------------------*/
#ifndef GC_GLOBAL_H
#define GC_GLOBAL_H

class rt_graph::CRtgAppFrame;
class GcLogin;
class GcWorld;
class GcPlayer;
class GcPet;
class NetSession;
class rt2_core::RtIni;
extern std::list<int> s_listCallStack;
class CSceneMusicApp;
extern CSceneMusicApp* g_pMusicThread;

const int STRING_LENGTH = 1024;

extern G_MEMDEF(s_szRegionHost, 80) extern G_MEMDEF(s_szStringTemp, STRING_LENGTH)

    enum ECallStackNum {
        csn_begin = 19790213,
        csn_CGameClientFrame_OnMouseLDown,
        csn_GcActor_AddCommandMoveTo,
        csn_GcActor_AttackSomebody,
        csn_GcActor_UseTargetSkill,
    };

#define PushCallStack(xx) s_listCallStack.push_back(xx)
#define CheckCallStack(xx, yy, nn)                                                                 \
    if (s_listCallStack.back() != (xx)) {                                                          \
        char* p = (char*)(yy);                                                                     \
        for (int i = 0; i < nn; i++)                                                               \
            *p++ = 0;                                                                              \
    }
#define PopCallStack() s_listCallStack.pop_back()

extern int s_isHacker;

void          GetGuideHostInfo(char** ppIP, int* piPort);
CRtgAppFrame* GetApp();
RtIni*        GetGameIni();
GcLogin*      GetLogin();
GcWorld*      GetWorld();
GcPlayer*     GetPlayer();
GcPet*        GetPlayerPet();
NetSession*   GetNet();
bool          AutoResize(void* sender, int i);

bool        VersionInit();
bool        isVersionOlderThan(const std::string& v1, const std::string& v2);
const char* GetGameVersion();
const std::string& GetGameVersionNum();

void ChangeGameFlow(bool bGame);  // bGame=true到游戏状态,bGame=false到登录状态
void SetRegionServerInfo(long lCharID, long lSeed, const char* szRegionHost, short sRegionPort);
void GetRegionServerInfo(long& lCharID, long& lSeed, char** szRegionHost, short& sRegionPort);

bool WorldIsInited();
bool IsUIInited();

void ShowErr(const char* szContent);
void ShowErr(const char* szContent1, short vContent2);
void ShowErr(const char* szContent1, const char* szContent2);
void ShowErr(const char* szContent1, const char* szContent2, const char* szContent3);
void ShowMessage(const char* szContent, const char* szCaption = NULL);

void PlayerFailOperate(const char* szOutputString, bool vIgnoreRepeated = false,
                       const char* szSoundFile = NULL,
                       int         iLevel = 0);  // iLevel 0普通级别, 10顶级(会弹出Dialog)
void ShowBattleInfoMessage(const char* szOutputMessage);  // 显示战斗信息
void ShowSystemMessage(const char* szOutputMessage, bool vIgnoreRepeated = false);  // 显示系统信息
void ShowGameMasterMessage(const char* szOutputMessage);  // 显示GM的信息
void ShowImportantMessage(const char* szOutputMessage);   // 显示重要信息
void ShowHelperMessage(const char* szSimpleMsg,
                       const char* szDetailedMsg);  // 显示提示信息, 简单信息, 详细信息
void ReadHtml(const char* szOutPutHtmlString, int szHintWitch = 200);  //解析HTML信息

#endif  //GC_GLOBAL_H

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

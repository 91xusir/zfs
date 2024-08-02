#ifndef _INC_GC_LOGIN_H_
#define _INC_GC_LOGIN_H_
#include "gc_login_session.h"

// 这里处理登陆，选人等非游戏进行的流程
class GcLogin : public GcUserInput, public GcLoginSession, public CRT_PoseNotify {
   public:
    //状态
    enum EStatus {
        GLS_NONE,                     // 初始状态，表示没有特定状态
        GLS_LOADING,                  //初始加载
        GLS_SELECT_GAMEWORLD_SERVER,  //选择游戏世界服务器
        GLS_LOGIN,                    //登录
        GLS_SELECT_CHAR,              //选角色
        GLS_CREATE_CHAR,              //创建角色
        GLS_ENTER_GAME_LOADING,       //进入游戏
        GLS_MAX                       //状态的最大值
    };

    //错误信息
    enum EErrMsg {
        GLEM_NONE,
        GLEM_DISCONNECT,
        GLEM_CANNOT_CONNECT,
        GLEM_CONNECT_FAIL,
        GLEM_LOGIN_FAIL,
        GLEM_LOGIN_FAIL_USER_ONLINE,
        GLEM_LOGIN_FAIL_ACCOUNT_STOP,
        GLEM_LOGIN_FAIL_ACCOUNT_PAUSE,
        GLEM_LOGIN_FAIL_POT_NOT_ENOUGH,
        GLEM_LOGIN_FAIL_PWD_WRONG,
        GLEM_LOGIN_FAIL_ACCOUNT_WRONG5,  //由于连续输入帐号密码错误，为保障用户信息安全，锁定帐号10分钟
        GLEM_LOGIN_FAIL_NOT_FOUND,
        GLEM_GET_ACCOUNT_FAIL,
        GLEM_CREATE_RESULT_FAIL,
        GLEM_CREATE_RESULT_EXIST,
        GLEM_CREATE_RESULT_INVALID_NAME,
        GLEM_CREATE_BANNED_NAME,  //角色名称含违禁字符
        GLEM_CREATE_RESULT_NAME_DISABLED,
        GLEM_CREATE_FACTION_ERROR,
        GLEM_RESTORE_RESULT_FAIL,  //恢复人物失败
        GLEM_DELETE_RESULT_FAIL,
        GLEM_LOGIN_FAIL_SERVER_FULL,
        GLEM_LOGIN_FAIL_SERVER_STOP,
        GLEM_LOGIN_FAIL_NEW_CARD,
        GLEM_LOGIN_FAIL_PUBWIN_CENTER,
        GLEM_LOGIN_FAIL_IS_LOCKED,
    };

   public:
    GcLogin(CGameClientFrame* pGameClientFrame);
    ~GcLogin();

    // GcLogin初始化一次
    bool InitOnce();
    // 清除一次
    bool ClearOnce();
    // 逻辑渲染
    virtual void OnRun(float fSecond);
    // 图形渲染前
    virtual void OnBeginRender();
    // 图形渲染
    virtual void OnRender(float fSecond);
    // 图形渲染后
    virtual void OnEndRender();
    // 鼠标移动
    virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
    //处理鼠标滚轮事件
    virtual void OnMouseWheel(int iButton, long vDelta);
    //左键按下
    virtual void OnMouseLDown(int iButton, int x, int y);

    //按钮抬起
    virtual void OnMouseUp(int iButton, int x, int y) {}

    //左键双击
    virtual void OnMouseLDClick(int iButton, int x, int y);
    //右键按下
    virtual void OnMouseRDown(int iButton, int x, int y);
    //右键拖动
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    //鼠标中键拖动
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    //键盘按下事件
    virtual void OnKeyDown(int iButton, int iKey);
    //键盘释放事件
    virtual void OnKeyUp(int iButton, int iKey);

    //获取当前选中角色
    inline int GetCurSelectChar() { return m_iCurSelectChar; }

    //设置当前选中角色
    inline void SetCurSelectChar(int iSel) { m_iCurSelectChar = iSel; }

    //获取当前状态
    inline EStatus GetStatus() { return m_eStatus; }

    //错误信息
    void LoginErrMsg(EErrMsg eMsg, const char* szRetStr = NULL, short sRetCode = 0);
    //选择游戏世界服务器
    void SelectGameWorld(int iIdx);

    // 登录账号
    void Login(const std::string& szUsername, const std::string& szPassword);
    //发送登录请求
    void OnNetLogin(int result, const char* szRetStr, short sRetCode, char cPointType, long lPoint);
    //网络连接状态回调
    void OnNetConnect(bool bSucceeded);
    //设置强制登录
    void SetForceLogin(bool bForceLogin);
    //网络连接断开
    void OnNetDisconnect();
    //被踢出
    void OnNetKick(int iCause);
    // 获取账户信息的回调函数
    void OnNetGetAccountInfo();
    // 选择角色完毕
    void OnNetSelectCharDone();
    // 关闭网络连接的函数
    void NetClose();

    //是否保存帐号
    void SetSaveAccount(bool bSave) { bSaveAccount = bSave; }

    bool GetSaveAccount() { return bSaveAccount; }

    //人物角色左旋
    void OnLeftRotation();
    void OnLeftRotationDown();
    void OnLeftRotationUp();
    //人物角色右旋
    void OnRightRotation();
    //进入游戏
    void OnSelectUser();
    void OnSelectUserWithPwd();
    void OnRestoreChar();
    void OnRestoreCharSuccess();
    void OnDeleteUser();
    void OnRandomCreateUser();
    void OnCreateUser();
    void OnSetCharPassword(const char* password);
    void OnChangeCharPassword(const char* old, const char* newpassword);
    void OnDelCharPassword(const char* password);
    void OnCharPasswordConfirm(const char* password);
    void ResetButtonPos();

    void OnNetDeleteUser(long id, char hasDel);

    void SetLoginState(EStatus eState);
    void UpdateSelectChar();

    //选择商或者周  lyy  2024.8.3
    void SetSelectShangOrZhou(int iSei);

    void SetSelectUser(int iSel);

    bool GetSelectUserWithPwd() { return bSelectUserWithPwd; }  //密码进入游戏

    void SetSelectUserWithPwd(bool bSel) { bSelectUserWithPwd = bSel; }

    virtual void OnConnect(bool bSucceeded);
    virtual void OnDisconnect();

    // 向导的网络，本来应该作为普通成员，但是为了在系统初始化之前就去取服务器列表，就把他作为静态成员
   public:
    // 开始获取游戏世界服务器的函数
    static void StartGetGameWorldServer();

    // 结束获取游戏世界服务器的函数
    static void EndGetGameWorldServer();

    // 处理网络指南进程的函数
    static void OnGuideNetProcess(float fSecond);

    // 获取游戏世界服务器列表的函数，返回下一次获取的时间
    static float OnGuideGetGameWorldServerList(CG_CmdPacket* pPacket);

    // 获取指定 IP 的 ping 值的函数
    static int GetPing(char* zIP);

    // 更新游戏世界服务器列表的 UI 的函数
    void OnUIUpdateGameWorldServerList() const;

    // 设置最后选择的角色 ID 的函数
    void SetLastSelectCharID(int iID);

    // 游戏世界服务器列表结构体
    //lyymark 注意 如果使用c++标准库的vector容器的话
    // 不要使用RtString类型 因为vector会自动管理内存
    struct SGWServerList {
        //RtString szName;       // 服务器名称
        //RtString szIP;         // 服务器 IP 地址
        std::string szName;       // 服务器名称
        std::string szIP;         // 服务器 IP 地址
        long        lPort;        // 服务器端口
        long        lEvaluation;  // 服务器评估值
        long        ping;         // 服务器 ping 值
    };

    // 网络向导会话的静态指针
    static CG_TCPSession* ms_pGuideNet;

    // 游戏世界服务器列表
    static std::vector<SGWServerList> ms_pGameWorldServerList;

    // 向导IP 地址的静态字符串
    static std::string ms_szGuideIP;

    // 向导端口的静态变量
    static long ms_lGuidePort;

    // 账号用户名的静态字符串
    static std::string m_szAccountUsername;

    // 账号密码的静态字符串
    static std::string m_szAccountPassword;

    // 大厅登陆用户名的字符串
    std::string m_hallName;

    // 大厅登陆 KEY 的字符串
    std::string m_hallKey;

    // 是否是在大厅登陆
    bool m_ishallLogin;

    // 是否强制登陆 (0---正常登录, 1---强制登录)
    bool m_bForceLogin;

   private:
    // 加载界面
    void EnterLoading();
    bool LeaveLoading();

    // 选择GameWorld服务器界面
    void EnterSelectGameWorldServer();
    bool LeaveSelectGameWorldServer();

    //从文件中读取账号密码
    void readAccountFromFile();

    // 登陆界面
    void EnterLogin();
    bool LeaveLogin();

    // 选人界面，包括删除任务
    void EnterSelectChar();
    bool LeaveSelectChar();

    // 创建人物界面
    void EnterCreateChar();
    bool LeaveCreateChar();
    void OnEnterCreateChar();
    void OnLeaveCreateChar();

    int                GetCharIndexByActorID(short actorID);
    const char*        GetCameraPoseName(int iSel, bool bSelectChar);
    CRT_ActorInstance* GetSelectedActorByActorID(short actorID);

   public:
    void UpdateCameraPos();
    void Lyy_UpdateCameraPos();
    void OnLoading();
    void OnRenderMask(RTGRenderMask mask, float fSecond);
    void UpdateGraphConfig(const char* szName);

    // 在当前m_mapActor寻找指定name的actor
    CRT_ActorInstance* FindModel(const std::string& szName);

    virtual void OnPoseBegin(SRT_Pose* pose);
    virtual void OnPoseEnd(SRT_Pose* pose);
    virtual void OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
    virtual void OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);

    void SetCharSex(bool bSex);         //人物角色性别---男女
    void ChangeCharAnimal(bool bNext);  //改变人物角色生肖名称
    void ChangeCharHair(bool bNext);    //改变人物角色发型名称
    void ChangeCharImage(bool bNext);   //改变人物角色发型名称

   private:
    EStatus           m_eStatus;  //
    EStatus           m_eNextStatus;
    EStatus           m_ePrevStatus;
    CGameClientFrame* m_pGameClientFrame;
    int               m_iCurSelectChar;
    bool              m_bSelectCharDone;
    bool              m_bCanInput;
    bool              m_bLight2;
    RtgLightItem      m_lightDirect;
    RtgLightItem      m_lightDirect2;
    long              m_lSkyFog;
    RtIni             m_ini;

    std::string m_szGameWorldServerName;  // 当前选择的游戏世界服务器
    std::string m_szGameWorldServerIP;
    long        m_lGameWorldServerPort;

    std::vector<GcActor*> m_listSelGcActor;  // 选人界面的UI上面的人物 这个是存可选玩家角色
    std::vector<CRT_ActorInstance*> m_listSelActor;  // 选人界面的UI上面的人物 这个是存可以创建人物
    std::vector<short>
        m_listSelectChar;  // 可选的人物，这里保存ID，具体的数据可以从g_TableUserActor中读出

    char m_bSex;  //0男,1女
    bool m_bLoading;

    //界面 Actor组件列表 登录 选角 创建
    std::map<std::string, CRT_ActorInstance*> m_mapActor;

    CRT_ActorInstance*                        m_pBody;
    CRT_ActorInstance*                        m_pCamera;
    CRT_ActorInstance*                        m_pWeaponWT;
    CRT_ActorInstance*                        m_pWeaponFL;
    CRT_ActorInstance*                        m_pWeaponFLWay;
    CRT_ActorInstance*                        m_pWeaponHL;
    CRT_ActorInstance*                        m_pWeaponMJ;
    std::map<std::string, CRT_ActorInstance*> m_mapLogin;
    std::map<std::string, CRT_ActorInstance*> m_mapSelectChar;
    std::map<std::string, CRT_ActorInstance*> m_mapCreateChar;
    std::map<DWORD, GcActor*>                 m_mapSelectActor;
    std::map<DWORD, CRT_ActorInstance*>       m_mapCreateActor;
    RtgMatrix12                               mWeaponMatrix;

    int m_HeadModelIndex[4] = {};
    int m_HeadImageIndex[4] = {};

    std::string m_strCharPassword;
    RtwRect     rectBtnSetChar;     //设置角色密码矩形
    RtwRect     rectBtnChangeChar;  //变更角色密码矩形
    int         iAnimalIndex;
    short       headModelID;
    short       headImageID;
    bool        bRandom;  //随机创建
    int         iRandomAnimalIndex;
    short       headRandomModelID;
    short       headRandomImageID;
    int         m_ePrevHeadID;
    int         m_eNextHeadID;
    long        m_iLastServer;        //记录上一个选中的服务器
    bool        bSelectUserWithPwd;   //密码登入游戏
    int         m_nDisconnectCause;   //网络断开原因
    bool        bSaveAccount;         //是否保存账号
    int         m_selectFaction = 1;  //阵营 1商 2周
};

#endif  // _INC_GC_LOGIN_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

#pragma once
#include "gc_login_session.h"
#include <gc_frame.h>

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

    static void LoadLoginSection(RtIni* pIni, const std::string& szSectionName,
                                 std::unordered_map<std::string, CRT_ActorInstance*>& mapActor);

    // GcLogin初始化一次
    bool InitOnce();
    bool ClearOnce();
    bool LeaveCurrentState(EStatus state);
    void EnterNewState(EStatus state);
    void ConfigureForState(const std::unordered_map<std::string, CRT_ActorInstance*>& actorMap,
                           const std::string&                                         graphConfig);
    // 逻辑渲染
    virtual void OnRun(float fSecond);
    // 图形渲染前后回调
    virtual void OnBeginRender();
    virtual void OnRender(float fSecond);
    virtual void OnEndRender();
    //处理鼠标事件
    virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
    //滚轮事件
    virtual void OnMouseWheel(int iButton, long vDelta);
    virtual void OnMouseLDown(int iButton, int x, int y);

    virtual void OnMouseUp(int iButton, int x, int y) {}

    virtual void OnMouseLDClick(int iButton, int x, int y);
    virtual void OnMouseRDown(int iButton, int x, int y);
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnKeyDown(int iButton, int iKey);
    virtual void OnKeyUp(int iButton, int iKey);

    //获取当前选中角色
    inline int GetCurSelectChar() const { return m_iCurSelectChar; }

    //设置当前选中角色
    inline void SetCurSelectChar(int iSel) { m_iCurSelectChar = iSel; }

    //获取当前状态
    inline EStatus GetStatus() { return m_eCurrentStatus; }

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
    inline void SetSaveAccount(bool bSave) { bSaveAccount = bSave; }

    inline bool GetSaveAccount() const { return bSaveAccount; }

    // 人物角色左旋
    void OnLeftRotation();
    void OnLeftRotationDown();
    void OnLeftRotationUp();
    // 人物角色右旋
    void OnRightRotation();
    // 确定选角后进入游戏
    void OnSelectUserDone();
    void OnSelectUserWithPwd();
    //恢复角色 废弃
    void OnRestoreChar();
    void OnRestoreCharSuccess();
    //删除角色
    void OnDeleteUser();
    //角色删除回调 用于删除本地图形
    void OnNetDeleteUser(long id, char hasDel);
    void OnRandomCreateUser();  //随机创建角色 废弃
    void OnCreateUser();        //创人回调
    //设置角色密码 废弃
    void OnSetCharPassword(const char* password);
    void OnChangeCharPassword(const char* old, const char* newpassword);
    void OnDelCharPassword(const char* password);
    void OnCharPasswordConfirm(const char* password);

    //输入角色密码进入游戏 废弃
    inline bool GetSelectUserWithPwd() { return bSelectUserWithPwd; }

    inline void SetSelectUserWithPwd(bool bSel) { bSelectUserWithPwd = bSel; }

    //设置登录状态
    void SetLoginState(EStatus eState);
    //选择商或者周  lyy  2024.8.3
    void SetSelectShangOrZhou(int iSei);
    void UpdateCreateChar();  //更新创建界面角色
    void UpdateSelectChar();  //更新选人界面角色
    void SetSelectUser(int iSel);

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

    static CG_TCPSession*             ms_pGuideNet;             // 网络向导会话的静态指针
    static std::vector<SGWServerList> ms_pGameWorldServerList;  // 游戏世界服务器列表
    static std::string                ms_szGuideIP;             // 向导IP 地址的静态字符串
    static long                       ms_lGuidePort;            // 向导端口的静态变量
    static std::string                m_szAccountUsername;      // 账号用户名的静态字符串
    static std::string                m_szAccountPassword;      // 账号密码的静态字符串
    // 命令行登陆用户名的字符串
    std::string m_hallName = "";
    // 命令行登陆 KEY 的字符串
    std::string m_hallKey = "";
    // 是否是在命令行登陆
    bool m_ishallLogin = false;
    // 是否强制登陆 (0---正常登录, 1---强制登录)
    bool m_bForceLogin = false;

   private:
    // 加载界面
    void EnterLoading();
    bool LeaveLoading();
    // 选择GameWorld服务器界面
    void EnterSelectGameWorldServer();
    bool LeaveSelectGameWorldServer();
    //从文件中读取账号密码
    void ReadAccountFromFile();
    // 登陆界面
    void EnterLogin();
    bool LeaveLogin();
    // 选人界面，包括删除角色
    void EnterSelectChar();
    bool LeaveSelectChar();
    // 创建人物界面
    void EnterCreateChar();
    bool LeaveCreateChar();
    void ChangeCharHead(int charCsvId, int headIndex);
    bool LoadModel(const std::string& modelName, CRT_ActorInstance** model,
                   std::string linkName = "", CRT_ActorInstance** parent = nullptr);

    int                GetCharIndexByActorID(short actorID);
    CRT_ActorInstance* GetSelectedActorByActorID(short actorID);

   public:  //图形相关
    void        Lyy_UpdateCameraPos();
    void        OnLoading();
    void        OnRenderMask(RTGRenderMask mask, float fSecond);
    void        UpdateGraphConfig(const char* szName);
    static bool DetectIntersection(const int& x, const int& y, CRT_ActorInstance* actor);
    // 在当前m_mapActor寻找指定name的actor实例
    CRT_ActorInstance* FindModel(const std::string& szName);
    virtual void       OnPoseBegin(SRT_Pose* pose);
    virtual void       OnPoseEnd(SRT_Pose* pose);
    virtual void       OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
    virtual void       OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);
    void               ChangeCharHair(bool bNext);  //改变人物角色发型名称
   private:
    EStatus           m_eCurrentStatus = GLS_NONE;   //当前状态
    CGameClientFrame* m_pGameClientFrame = nullptr;  //游戏客户端指针
    int               m_iCurSelectChar = -1;         //当前选中的人物
    bool              m_bSelCharNetSucceed = false;  //判断选角服务器响应是否成功
    bool m_bCanInput = false;  //是否允许鼠标键盘输入 这里不包括输入框UI类 只是当前类
    //环境光特效之类的
    bool         m_bLight2;
    RtgLightItem m_lightDirect;
    RtgLightItem m_lightDirect2;
    long         m_lSkyFog;
    RtIni        m_ini;                                //ini文件读取
    std::string  m_szGameWorldServerName;              // 当前选择的游戏世界服务器
    std::string  m_szGameWorldServerIP;                //IP
    long         m_lGameWorldServerPort;               //端口
    std::vector<CRT_ActorInstance*> m_creatActorList;  // 创人界面人物
    std::vector<short> m_selectUserCharIds;  // 可选的人物，这里保存数据表里的唯一id
    char m_bSex = SEX_MALE;                  //0男,1女
    bool m_bLoading = false;                 //判断是否初始化加载

    //创建角色界面的人物装备模型
    CRT_ActorInstance* m_pBody = nullptr;
    CRT_ActorInstance* m_pZsDao = nullptr;       //战士武器1
    CRT_ActorInstance* m_pZsDao2 = nullptr;      //战士武器2
    CRT_ActorInstance* m_pSsJian = nullptr;      //术士武器
    CRT_ActorInstance* m_pSsJianPath = nullptr;  //术士武器轨迹
    CRT_ActorInstance* m_pMmGong = nullptr;      //MM武器
    CRT_ActorInstance* m_pDsLun = nullptr;       //道士武器
    CRT_ActorInstance* m_pDsLunPath = nullptr;   //道士武器//iw0401huan_04  waiting_l1
    //login.ini 场景动画物件加载保存在这里
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapLogin = {};       //登入
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapSelectChar = {};  //选人
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapCreateChar = {};  //创人
    //用于存入上面的场景 做替换
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapActor = {};

    int         m_HeadModelIndex[4] = {};  //头模型数组
    int         m_HeadImageIndex[4] = {};  //头像数组 待删除
    std::string m_strCharPassword = "";    //角色密码 待删除
    int         iAnimalIndex = 0;
    short       headModelID = 0;
    short       headImageID = 0;
    bool        bRandom = false;  //是否随机创建
    int         iRandomAnimalIndex = 0;
    short       headRandomModelID = 0;
    short       headRandomImageID = 0;
    int         m_ePrevHeadID = 0;
    int         m_eNextHeadID = 0;
    long        m_iLastServer = -1;          //记录上一个选中的服务器
    bool        bSelectUserWithPwd = false;  //密码登入游戏
    int         m_nDisconnectCause = 0;      //网络断开原因
    bool        bSaveAccount = false;        //是否保存账号
   public:
    //直接public了 原则上要封装 实际懒得  add by lyy
    //角色选择
    std::unordered_map<uint32_t, int>      m_selectChar_IDMapIndex = {};
    std::unordered_map<uint32_t, GcActor*> m_selectChar_IDMapGcActor = {};
    int                                    m_curSelCharIndex = -1;

    //角色创建
    enum Faction { None = 0, Shang = 1, Zhou = 2 };

    int m_selectFaction = None;  //阵营  None代表没选  用于判断二阶段返回
    std::unordered_map<int, CRT_ActorInstance*> m_crtChar_csvIdMapActIns = {};
    std::unordered_map<int, vector<SHeadModel>> m_crtChar_csvIdMapHeads = {};
    int                                         headIndex = 0;
    int                                         m_curCrtCharCsvID = -1;
    std::vector<int>                            actSoltInTray = {3, 2, 1};
    int                                         charAct[4] = {};  //数组索引作角色排序
};

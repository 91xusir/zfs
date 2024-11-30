#pragma once
#include "gc_login_session.h"
#include <gc_frame.h>
#include <vector>
#include <net/g_tcpsession.h>
#include <unordered_map>
#include <character/cha_anim.h>
#include "gc_types.h"
#include <graph/rtg_enum.h>
#include <character/cha_basic.h>
#include "gc_actor.h"
#include <region_client.h>
#include <cstdint>
#include <graph/rtg_vertex.h>

/*
lyy 2024.8.4 �ع�
*/
// ���ﴦ���½��ѡ�˵ȷ���Ϸ���е�����
class GcLogin : public GcUserInput, public GcLoginSession, public CRT_PoseNotify {
   public:
    //״̬
    enum EStatus {
        GLS_NONE,                     // ��ʼ״̬����ʾû���ض�״̬
        GLS_LOADING,                  //��ʼ����
        GLS_SELECT_GAMEWORLD_SERVER,  //ѡ����Ϸ���������
        GLS_LOGIN,                    //��¼
        GLS_SELECT_CHAR,              //ѡ��ɫ
        GLS_CREATE_CHAR,              //������ɫ
        GLS_ENTER_GAME_LOADING,       //������Ϸ
        GLS_MAX                       //״̬�����ֵ
    };

    //������Ϣ
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
        GLEM_LOGIN_FAIL_ACCOUNT_WRONG5,  //�������������ʺ��������Ϊ�����û���Ϣ��ȫ�������ʺ�10����
        GLEM_LOGIN_FAIL_NOT_FOUND,
        GLEM_GET_ACCOUNT_FAIL,
        GLEM_CREATE_RESULT_FAIL,
        GLEM_CREATE_RESULT_EXIST,
        GLEM_CREATE_RESULT_INVALID_NAME,
        GLEM_CREATE_BANNED_NAME,  //��ɫ���ƺ�Υ���ַ�
        GLEM_CREATE_RESULT_NAME_DISABLED,
        GLEM_CREATE_FACTION_ERROR,
        GLEM_RESTORE_RESULT_FAIL,  //�ָ�����ʧ��
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

    // GcLogin��ʼ��һ��
    bool InitOnce();
    bool ClearOnce();
    bool LeaveCurrentState(EStatus state);
    void EnterNewState(EStatus state);
    void ConfigureForState(const std::unordered_map<std::string, CRT_ActorInstance*>& actorMap,
                           const std::string&                                         graphConfig);
    // �߼���Ⱦ
    virtual void OnRun(float fSecond);
    // ͼ����Ⱦǰ��ص�
    virtual void OnBeginRender();
    virtual void OnRender(float fSecond);
    virtual void OnEndRender();
    //��������¼�
    virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseWheel(int iButton, long vDelta);
    virtual void OnMouseLDown(int iButton, int x, int y);

    virtual void OnMouseUp(int iButton, int x, int y) {}

    virtual void OnMouseLDClick(int iButton, int x, int y);
    virtual void OnMouseRDown(int iButton, int x, int y);
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnKeyDown(int iButton, int iKey);
    virtual void OnKeyUp(int iButton, int iKey);

    //��ȡ��ǰ״̬
    inline EStatus GetStatus() const { return m_eCurrentStatus; }

    //������Ϣ
    void LoginErrMsg(EErrMsg eMsg, const char* szRetStr = NULL, short sRetCode = 0);
    //ѡ����Ϸ���������
    void SelectGameWorld(int iIdx);
    // ��¼�˺�
    void Login(const std::string& szUsername, const std::string& szPassword);
    //���͵�¼����
    void OnNetLogin(int result, const char* szRetStr, short sRetCode, char cPointType, long lPoint);
    //��������״̬�ص�
    void OnNetConnect(bool bSucceeded);
    //����ǿ�Ƶ�¼
    void SetForceLogin(bool bForceLogin);
    //�������ӶϿ�
    void OnNetDisconnect();
    //���߳�
    void OnNetKick(int iCause);
    // ��ȡ�˻���Ϣ�Ļص�����
    void OnNetGetAccountInfo();
    // ѡ���ɫ���
    void OnNetSelectCharDone();
    // �ر��������ӵĺ���
    void NetClose();

    //�Ƿ񱣴��ʺ�
    inline void SetSaveAccount(bool bSave) { bSaveAccount = bSave; }

    inline bool GetSaveAccount() const { return bSaveAccount; }

    // �����ɫ���� ����
    void OnLeftRotation();
    void OnLeftRotationDown();
    void OnLeftRotationUp();
    // �����ɫ����
    void OnRightRotation();
    // ȷ��ѡ�Ǻ������Ϸ
    void OnSelectUserDone();
    void OnSelectUserWithPwd();
    //�ָ���ɫ ����
    void OnRestoreChar();
    void OnRestoreCharSuccess();
    //ɾ����ɫ
    void OnDeleteUser();
    //��ɫɾ���ص� ����ɾ������ͼ��
    void OnNetDeleteUser(long id, char hasDel);
    void OnRandomCreateUser();  //���������ɫ ����
    void OnCreateUser();        //���˻ص�
    //���ý�ɫ���� ����
    void OnSetCharPassword(const char* password);
    void OnChangeCharPassword(const char* old, const char* newpassword);
    void OnDelCharPassword(const char* password);
    void OnCharPasswordConfirm(const char* password);

    //�����ɫ���������Ϸ ����
    inline bool GetSelectUserWithPwd() const { return bSelectUserWithPwd; }

    inline void SetSelectUserWithPwd(bool bSel) { bSelectUserWithPwd = bSel; }

    //���õ�¼״̬
    void SetLoginState(EStatus eState);
    //ѡ���̻�����  lyy  2024.8.3
    void SetSelectShangOrZhou(int iSei);
    void UpdateCreateChar();  //���´��������ɫ
    void UpdateSelectChar();  //����ѡ�˽����ɫ
    void ClearSelectRoleOnce();

    virtual void OnConnect(bool bSucceeded);
    virtual void OnDisconnect();
    // �򵼵����磬����Ӧ����Ϊ��ͨ��Ա������Ϊ����ϵͳ��ʼ��֮ǰ��ȥȡ�������б��Ͱ�����Ϊ��̬��Ա
   public:
    // ��ʼ��ȡ��Ϸ����������ĺ���
    static void StartGetGameWorldServer();
    // ������ȡ��Ϸ����������ĺ���
    static void EndGetGameWorldServer();
    // ��������ָ�Ͻ��̵ĺ���
    static void OnGuideNetProcess(float fSecond);
    // ��ȡ��Ϸ����������б�ĺ�����������һ�λ�ȡ��ʱ��
    static float OnGuideGetGameWorldServerList(CG_CmdPacket* pPacket);
    // ��ȡָ�� IP �� ping ֵ�ĺ���
    static int GetPing(char* zIP);
    // ������Ϸ����������б�� UI �ĺ���
    void OnUIUpdateGameWorldServerList() const;

    void SaveAccountConfig();//�����ʺ�
    void LoadAccountConfig();

    // ��Ϸ����������б�ṹ��
    //lyymark ע�� ���ʹ��c++��׼���vector�����Ļ�
    // ��Ҫʹ��RtString���� ��Ϊvector���Զ������ڴ�
    struct SGWServerList {
        //RtString szName;       // ����������
        //RtString szIP;         // ������ IP ��ַ
        std::string szName;       // ����������
        std::string szIP;         // ������ IP ��ַ
        long        lPort;        // �������˿�
        long        lEvaluation;  // ����������ֵ
        long        ping;         // ������ ping ֵ
    };

    static CG_TCPSession*             ms_pGuideNet;             // �����򵼻Ự�ľ�ָ̬��
    static std::vector<SGWServerList> ms_pGameWorldServerList;  // ��Ϸ����������б�
    static std::string                ms_szGuideIP;             // ��IP ��ַ�ľ�̬�ַ���
    static long                       ms_lGuidePort;            // �򵼶˿ڵľ�̬����
    static std::string                m_szAccountUsername;      // �˺��û����ľ�̬�ַ���
    static std::string                m_szAccountPassword;      // �˺�����ľ�̬�ַ���
    // �����е�½�û������ַ���
    std::string m_hallName = "";
    // �����е�½ KEY ���ַ���
    std::string m_hallKey = "";
    // �Ƿ����������е�½
    bool m_ishallLogin = false;
    // �Ƿ�ǿ�Ƶ�½ (0---������¼, 1---ǿ�Ƶ�¼)
    bool m_bForceLogin = false;

   private:
    // ���ؽ���
    void EnterLoading();
    bool LeaveLoading();
    // ѡ��GameWorld����������
    void EnterSelectGameWorldServer();
    bool LeaveSelectGameWorldServer() const;
    //���ļ��ж�ȡ�˺�����
    void ReadAccountFromFile();
    // ��½����
    void EnterLogin();
    bool LeaveLogin() const;
    // ѡ�˽��棬����ɾ����ɫ
    void EnterSelectChar();
    bool LeaveSelectChar();
    // �����������
    void EnterCreateChar();
    bool LeaveCreateChar();
    void ChangeCharHead(int charCsvId, int headIndex);
    bool LoadModel(const std::string& modelName, CRT_ActorInstance** model,
                   std::string linkName = "", CRT_ActorInstance** parent = nullptr);

   public:  //ͼ�����
    void        Lyy_UpdateCameraPos();
    void        OnLoginInit();
    void        OnRenderMask(RTGRenderMask mask, float fSecond);
    void        UpdateGraphConfig(const char* szName);
    static bool DetectIntersection(const int& x, const int& y, CRT_ActorInstance* actor);
    // �ڵ�ǰm_mapActorѰ��ָ��name��actorʵ��
    CRT_ActorInstance* FindModel(const std::string& szName);
    virtual void       OnPoseBegin(SRT_Pose* pose);
    virtual void       OnPoseEnd(SRT_Pose* pose);
    virtual void       OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
    virtual void       OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);

   private:
    EStatus           m_eCurrentStatus     = GLS_NONE;  //��ǰ״̬
    bool              m_bLoading           = false;     //�ж��Ƿ��ʼ������login.ini�ļ�
    CGameClientFrame* m_pGameClientFrame   = nullptr;   //��Ϸ�ͻ���ָ��
    bool              m_bSelCharNetSucceed = false;     //�ж�ѡ�Ƿ�������Ӧ�Ƿ�ɹ�
    bool m_bCanInput = false;  //�Ƿ��������������� ���ﲻ���������UI�� ֻ�ǵ�ǰ��
    //��������Ч֮���
    bool         m_bLight2;
    RtgLightItem m_lightDirect;
    RtgLightItem m_lightDirect2;
    long         m_lSkyFog;
    //ini�ļ���ȡ
    RtIni m_ini;
    //�������
    std::string m_szGameWorldServerName;               // ��ǰѡ�����Ϸ���������
    std::string m_szGameWorldServerIP;                 //IP
    long        m_lGameWorldServerPort;                //�˿�
    std::vector<CRT_ActorInstance*> m_creatActorList;  // ���˽�������
    std::vector<short> m_selectUserCharIds;  // ��ѡ��������ﱣ�����ݱ����Ψһid

    //������ɫ���������װ��ģ��
    CRT_ActorInstance* m_pBody       = nullptr;
    CRT_ActorInstance* m_pZsDao      = nullptr;  //սʿ����1
    CRT_ActorInstance* m_pZsDao2     = nullptr;  //սʿ����2
    CRT_ActorInstance* m_pSsJian     = nullptr;  //��ʿ����
    CRT_ActorInstance* m_pSsJianPath = nullptr;  //��ʿ�����켣
    CRT_ActorInstance* m_pMmGong     = nullptr;  //MM����
    CRT_ActorInstance* m_pDsLun      = nullptr;  //��ʿ����
    CRT_ActorInstance* m_pDsLunPath  = nullptr;  //��ʿ����//iw0401huan_04  waiting_l1
    //login.ini ��������������ر���������
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapLogin      = {};  //����
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapSelectChar = {};  //ѡ��
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapCreateChar = {};  //����
    //���ڴ�������ĳ��� ���滻
    std::unordered_map<std::string, CRT_ActorInstance*> m_mapActor = {};

    std::string m_strCharPassword  = "";     //��ɫ���� ���õ�����
    long        m_iLastServer      = -1;     //��¼��һ��ѡ�еķ�����
    bool        bSelectUserWithPwd = false;  //���������Ϸ
    int         m_nDisconnectCause = 0;      //����Ͽ�ԭ��
    bool        bSaveAccount       = false;  //�Ƿ񱣴��˺�

   public:
    struct AccountConfig {
        bool        bSaveAccount = false;
        std::string szAccount    = "";
        std::string szPassword   = "";
    } m_accountConfig;

    //ֱ��public�� ԭ����Ҫ��װ ʵ������  add by lyy
    //-------------------��ɫѡ��--------------------
    std::unordered_map<uint32_t, int>      m_selRole_IDMapIndex   = {};
    std::unordered_map<uint32_t, GcActor*> m_selRole_IDMapGcActor = {};
    int                                    m_curSelRoleIndex      = -1;

    //-------------------��ɫ����---------------------
    enum Faction { None = FACTION_UNKNOWN, Shang = FACTION_SHANG, Zhou = FACTION_ZHOU };

    enum CreateChatIds { ZhanShi = 1, FengWu = 4, ShuShi = 7, DaoShi = 10 };

    struct CharSkinInfo {
        std::string act;
        std::string body;
        std::string hand;
        std::string leg;
    };

    //Ƥ������д���� ģ�� ���� ͷ �� ��
    static inline std::unordered_map<int, CharSkinInfo> s_mCharSkinInfo = {
        {ZhanShi, {"pf", "bpf011", "hpf011", "lpf011"}},
        {FengWu, {"ph", "bph001", "hph001", "lph001"}},
        {ShuShi, {"pn", "bpn011", "hpn011", "lpn011"}},
        {DaoShi, {"pt", "bpt008", "hpt008", "lpt008"}}};
    static inline int s_CreatActIds_FromCsv[4] = {ZhanShi, FengWu, ShuShi, DaoShi};

    int m_selectFaction = None;  //��Ӫ  None����ûѡ  �����ж϶��׶η���

    std::unordered_map<int, CRT_ActorInstance*> m_crtRole_csvIdMapActIns = {};  //ģ��
    std::unordered_map<int, vector<SHeadModel>> m_crtRole_csvIdMapHeads  = {};  //ͷ
    int                                         headIndex                = 0;   //ͷģ������

    int m_curCrtRoleCsvID = -1;  //ģ�ͱ�ID��ע�ⲻ��ģ��ID
    //���ﴴ��ҳ���������� �����Ҫ�� д�˺þò�д��
    std::vector<int> actSoltInTray = {3, 2, 1};

   public:
    //����ģ�͹��������� 4x3 ����RtgMatrix12��,����������� 4x4 ����RtgMatrix16������Ҫһ��ת������
    inline static void Lyy_AdjustCameraMatrix(RtgMatrix16* matOut, RtgMatrix12* matIn) {
        RtgVertex3 x = *(RtgVertex3*)&matIn->_00;
        RtgVertex3 y = *(RtgVertex3*)&matIn->_10;
        RtgVertex3 z = *(RtgVertex3*)&matIn->_20;
        x.Normalize();
        y.Normalize();
        z.Normalize();
        matOut->Identity();
        matOut->SetRow(0, x);
        matOut->SetRow(1, y);
        matOut->SetRow(2, z);
        matOut->SetRow(3, (float*)&matIn->_30);
    }
};

#ifndef _INC_GC_LOGIN_H_
#define _INC_GC_LOGIN_H_

#include "gc_login_session.h"
#include "../preLog.h"
//lyymark 1.Game.Init.LogInfo Ԥ��ģʽ����̨���
#ifdef _PREVIEW
#define P_LOGINFO(message) preLog::logInfo(message)
#define P_LOGWARN(message) preLog::logWarn(message)
#define P_LOGERROR(message) preLog::logError(message)
#else
#define P_LOGINFO(message)
#define P_LOGWARN(message)
#define P_LOGERROR(message)
#endif

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
        GLS_MAX                       //״̬�����ֵ��ͨ�����ڱ�ʾ״̬����������ѭ������
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

    // GcLogin��ʼ��һ��
    bool InitOnce();
    // ���һ��
    bool ClearOnce();
    // �߼���Ⱦ
    virtual void OnRun(float fSecond);
    // ͼ����Ⱦǰ
    virtual void OnBeginRender();
    // ͼ����Ⱦ
    virtual void OnRender(float fSecond);
    // ͼ����Ⱦ��
    virtual void OnEndRender();
    // ����ƶ�
    virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
    //�����������¼�
    virtual void OnMouseWheel(int iButton, long vDelta);
    //�������
    virtual void OnMouseLDown(int iButton, int x, int y);

    //��ť�ͷ�
    virtual void OnMouseUp(int iButton, int x, int y) {}

    //���˫��
    virtual void OnMouseLDClick(int iButton, int x, int y);
    //�Ҽ�����
    virtual void OnMouseRDown(int iButton, int x, int y);
    //�Ҽ��϶�
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    //����м��϶�
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    //���̰����¼�
    virtual void OnKeyDown(int iButton, int iKey);
    //�����ͷ��¼�
    virtual void OnKeyUp(int iButton, int iKey);

    inline int GetCurSelectChar() { return m_iCurSelectChar; }

    inline void SetCurSelectChar(int iSel) { m_iCurSelectChar = iSel; }

    inline EStatus GetStatus() { return m_eStatus; }

    void LoginErrMsg(EErrMsg eMsg, const char* szRetStr = NULL, short sRetCode = 0);
    void SelectGameWorld(int iIdx);

    /// <summary>
    /// ��¼�˻�����
    /// </summary>
    /// <param name="szUsername">�û���</param>
    /// <param name="szPassword">����</param>
    void Login(char* szUsername, char* szPassword);
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
    void SetSaveAccount(bool bSave) { bSaveAccount = bSave; }

    bool GetSaveAccount() { return bSaveAccount; }

    //�����ɫ����
    void OnLeftRotation();
    void OnLeftRotationDown();
    void OnLeftRotationUp();
    //�����ɫ����
    void OnRightRotation();
    //������Ϸ
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

    void SetSelectUser(int iSel);

    bool GetSelectUserWithPwd() { return bSelectUserWithPwd; }  //���������Ϸ

    void SetSelectUserWithPwd(bool bSel) { bSelectUserWithPwd = bSel; }

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
    void OnUIUpdateGameWorldServerList()const;

    // �������ѡ��Ľ�ɫ ID �ĺ���
    void SetLastSelectCharID(int iID);

    // ��Ϸ����������б�ṹ��
    struct SGWServerList {
        RtString szName;       // ����������
        RtString szIP;         // ������ IP ��ַ
        long     lPort;        // �������˿�
        long     lEvaluation;  // ����������ֵ
        long     ping;         // ������ ping ֵ
    };

    // �����򵼻Ự�ľ�ָ̬��
    static CG_TCPSession* ms_pGuideNet;

    // ��Ϸ����������б�ľ�ָ̬��
    static SGWServerList* ms_pGameWorldServerList;

    // ��Ϸ��������������ľ�̬����
    static long ms_lGameWorldServerCount;

    // ��IP ��ַ�ľ�̬�ַ���
    static std::string ms_szGuideIP;

    // �򵼶˿ڵľ�̬����
    static long ms_lGuidePort;

    // �˺��û����ľ�̬�ַ���
    static std::string m_szAccountUsername;

    // �˺�����ľ�̬�ַ���
    static std::string m_szAccountPassword;

    // ������½�û������ַ���
    std::string m_hallName;

    // ������½ KEY ���ַ���
    std::string m_hallKey;

    // �Ƿ��Ǵ�����½�Ĳ���ֵ
    bool m_ishallLogin;

    // �Ƿ�ǿ�Ƶ�½�Ĳ���ֵ (0---������¼, 1---ǿ�Ƶ�¼)
    bool m_bForceLogin;

    // ����̶���
    CSoftKeyboard m_Keyboard;

   private:
    // ������ؽ���
    void EnterLoading();
    bool LeaveLoading();

    // ѡ��GameWorld����������
    void EnterSelectGameWorldServer();
    bool LeaveSelectGameWorldServer();

    // ��½����
    void EnterLogin();
    bool LeaveLogin();

    // ѡ�˽��棬����ɾ������
    void EnterSelectChar();
    bool LeaveSelectChar();

    // �����������
    void EnterCreateChar();
    bool LeaveCreateChar();
    void OnEnterCreateChar();
    void OnLeaveCreateChar();

    int                GetCharIndexByActorID(short actorID);
    const char*        GetCameraPoseName(int iSel, bool bSelectChar);
    CRT_ActorInstance* GetSelectedActorByActorID(short actorID);

   public:
    void UpdateCameraPos();
    void SJDL_UpdateCameraPos();
    void OnLoading();
    void OnRenderMask(RTGRenderMask mask, float fSecond);
    void UpdateGraphConfig(const char* szName);

    // �ڵ�ǰm_mapActorѰ��ָ��name��actor
    CRT_ActorInstance* FindModel(const std::string& szName);

    virtual void OnPoseBegin(SRT_Pose* pose);
    virtual void OnPoseEnd(SRT_Pose* pose);
    virtual void OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
    virtual void OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);

    void SetCharSex(bool bSex);         //�����ɫ�Ա�---��Ů
    void ChangeCharAnimal(bool bNext);  //�ı������ɫ��Ф����
    void ChangeCharHair(bool bNext);    //�ı������ɫ��������
    void ChangeCharImage(bool bNext);   //�ı������ɫ��������

   private:
    EStatus           m_eStatus;
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

    RtString m_szGameWorldServerName;
    RtString m_szGameWorldServerIP;
    long     m_lGameWorldServerPort;

    std::vector<CRT_ActorInstance*> m_listSelActor;    // ѡ�˽����UI���������
    std::vector<GcActor*>           m_listSelGcActor;  // ѡ�˽����UI���������
    std::vector<short> m_listSelectChar;  // ��ѡ��������ﱣ��ID����������ݿ��Դ�g_TableUserActor�ж���

    char m_bSex;  //0��,1Ů

    bool m_bLoading;

    //��ǰActor����б�
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

    CRT_ActorInstance* m_pPetActor1;
    CRT_ActorInstance* m_pPetActor2;
    CRT_ActorInstance* m_pPetActor3;

    int         m_HeadModelIndex[4];
    int         m_HeadImageIndex[4];
    char        m_bCharFrozen[4];
    std::string m_strCharPassword;
    RtwRect     rectBtnSetChar;     //���ý�ɫ�������
    RtwRect     rectBtnChangeChar;  //�����ɫ�������
    int         iAnimalIndex;
    short       headModelID;
    short       headImageID;
    bool        bRandom;  //�������
    int         iRandomAnimalIndex;
    short       headRandomModelID;
    short       headRandomImageID;
    int         m_ePrevHeadID;
    int         m_eNextHeadID;
    long        m_iLastServer;       //��¼��һ��ѡ�еķ�����
    bool        bSelectUserWithPwd;  //���������Ϸ
    int         m_nDisconnectCause;  //����Ͽ�ԭ��
    bool        bSaveAccount;
};

#endif  // _INC_GC_LOGIN_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

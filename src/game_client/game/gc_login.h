#ifndef _INC_GC_LOGIN_H_
#define _INC_GC_LOGIN_H_

#include "gc_login_session.h"


// ���ﴦ���½��ѡ�˵ȷ���Ϸ���е�����
class GcLogin : public GcUserInput, public GcLoginSession, public CRT_PoseNotify
{
public:
	//״̬
	enum EStatus
	{
		GLS_NONE,
		GLS_LOADING,//��ʼ����
		GLS_SELECT_GAMEWORLD_SERVER,//ѡ����Ϸ���������
		GLS_LOGIN, //��¼
		GLS_SELECT_CHAR,  //ѡ��ɫ
		GLS_CREATE_CHAR,   //������ɫ
		GLS_ENTER_GAME_LOADING,     //������Ϸ
		GLS_MAX //TODO ��ʱδ֪����
	};

	enum EErrMsg
	{
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
		GLEM_LOGIN_FAIL_ACCOUNT_WRONG5,       //�������������ʺ��������Ϊ�����û���Ϣ��ȫ�������ʺ�10����
		GLEM_LOGIN_FAIL_NOT_FOUND,
		GLEM_GET_ACCOUNT_FAIL,
		GLEM_CREATE_RESULT_FAIL,
		GLEM_CREATE_RESULT_EXIST,
		GLEM_CREATE_RESULT_INVALID_NAME,
		GLEM_CREATE_BANNED_NAME,              //��ɫ���ƺ�Υ���ַ�
		GLEM_CREATE_RESULT_NAME_DISABLED,
		GLEM_CREATE_FACTION_ERROR,
		GLEM_RESTORE_RESULT_FAIL,             //�ָ�����ʧ��
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

	// ��ʼ�������
	bool InitOnce();
	bool ClearOnce();

	virtual void OnRun(float fSecond);          // �߼���Ⱦ
	virtual void OnBeginRender();				// ͼ����Ⱦǰ
	virtual void OnRender(float fSecond);       // ͼ����Ⱦ
	virtual void OnEndRender();					// ͼ����Ⱦ��

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

	void Login(char* szUsername, char* szPassword);
	void OnNetLogin(int result, const char* szRetStr, short sRetCode, char cPointType, long lPoint);  //��¼
	void OnNetConnect(bool bSucceeded);		//��������
	void SetForceLogin(bool bForceLogin);   //����ǿ�Ƶ�¼
	void OnNetDisconnect();					//�������ӶϿ�
	void OnNetKick(int iCause);
	void OnNetGetAccountInfo();
	void OnNetSelectCharDone();
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

	bool GetSelectUserWithPwd() { return bSelectUserWithPwd; }           //���������Ϸ
	void SetSelectUserWithPwd(bool bSel) { bSelectUserWithPwd = bSel; }

	virtual	void	OnConnect(bool bSucceeded);
	virtual void	OnDisconnect();


	// �򵼵����磬����Ӧ����Ϊ��ͨ��Ա������Ϊ����ϵͳ��ʼ��֮ǰ��ȥȡ�������б��Ͱ�����Ϊ��̬��Ա
public:
	static void StartGetGameWorldServer();
	static void EndGetGameWorldServer();
	static void OnGuideNetProcess(float fSecond);
	static float OnGuideGetGameWorldServerList(CG_CmdPacket* pPacket); // ������һ�λ�ȡ��ʱ��
	static int  GetPing(char* zIP);
	void OnUIUpdateGameWorldServerList();
	void SetLastSelectCharID(int iID);

	struct SGWServerList
	{
		RtString        szName;
		RtString        szIP;
		long            lPort;
		long            lEvaluation;
		long			ping;
	};
	static CG_TCPSession* ms_pGuideNet;
	static SGWServerList* ms_pGameWorldServerList;
	static long                     ms_lGameWorldServerCount;
	static std::string				ms_szGuideIP;
	static long                     ms_lGuidePort;
	static std::string				m_szAccountUsername;
	static std::string				m_szAccountPassword;

	std::string m_hallName;//������½�û���
	std::string m_hallKey;//������½KEY
	bool m_ishallLogin;//�Ƿ��Ǵ�����½
	bool m_bForceLogin;         //0---������¼,1---ǿ�Ƶ�¼
	CSoftKeyboard   m_Keyboard;
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

	int GetCharIndexByActorID(short actorID);
	const char* GetCameraPoseName(int iSel, bool bSelectChar);
	CRT_ActorInstance* GetSelectedActorByActorID(short actorID);

public:
	void UpdateCameraPos();
	void SJDL_UpdateCameraPos();
	void OnLoading();
	void OnRenderMask(RTGRenderMask mask, float fSecond);
	void UpdateGraphConfig(const char* szName);

	// �ڵ�ǰm_mapActorѰ��ָ��name��actor
	CRT_ActorInstance* FindModel(const char* szName);

	virtual void OnPoseBegin(SRT_Pose* pose);
	virtual void OnPoseEnd(SRT_Pose* pose);
	virtual void OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
	virtual void OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);

	void SetCharSex(bool bSex);         //�����ɫ�Ա�---��Ů

	void ChangeCharAnimal(bool bNext);          //�ı������ɫ��Ф����
	void ChangeCharHair(bool bNext);            //�ı������ɫ��������
	void ChangeCharImage(bool bNext);            //�ı������ɫ��������

private:
	EStatus             m_eStatus;
	EStatus             m_eNextStatus;
	EStatus             m_ePrevStatus;
	CGameClientFrame* m_pGameClientFrame;
	int                 m_iCurSelectChar;
	bool                m_bSelectCharDone;
	bool                m_bCanInput;
	bool                m_bLight2;
	RtgLightItem        m_lightDirect;
	RtgLightItem        m_lightDirect2;
	long                m_lSkyFog;
	RtIni               m_ini;

	RtString            m_szGameWorldServerName;
	RtString            m_szGameWorldServerIP;
	long                m_lGameWorldServerPort;

	std::vector<CRT_ActorInstance*> m_listSelActor; // ѡ�˽����UI���������
	std::vector<GcActor*>			m_listSelGcActor; // ѡ�˽����UI���������
	std::vector<short>  m_listSelectChar;       // ��ѡ��������ﱣ��ID����������ݿ��Դ�g_TableUserActor�ж���

	char                m_bSex;                 //0��,1Ů

	bool                m_bLoading;

	std::map<std::string, CRT_ActorInstance*>   m_mapActor;

	CRT_ActorInstance* m_pBody;
	CRT_ActorInstance* m_pCamera;//?
	CRT_ActorInstance* m_pWeaponWT;
	CRT_ActorInstance* m_pWeaponFL;
	CRT_ActorInstance* m_pWeaponFLWay;
	CRT_ActorInstance* m_pWeaponHL;
	CRT_ActorInstance* m_pWeaponMJ;
	std::map<std::string, CRT_ActorInstance*>   m_mapLogin;
	std::map<std::string, CRT_ActorInstance*>   m_mapSelectChar;
	std::map<std::string, CRT_ActorInstance*>   m_mapCreateChar;
	std::map<DWORD, GcActor*>					m_mapSelectActor;
	std::map<DWORD, CRT_ActorInstance*>         m_mapCreateActor;
	RtgMatrix12 mWeaponMatrix;

	CRT_ActorInstance* m_pPetActor1;
	CRT_ActorInstance* m_pPetActor2;
	CRT_ActorInstance* m_pPetActor3;

	int  m_HeadModelIndex[4];
	int  m_HeadImageIndex[4];
	char m_bCharFrozen[4];
	std::string		m_strCharPassword;
	RtwRect         rectBtnSetChar;                        //���ý�ɫ�������
	RtwRect         rectBtnChangeChar;                     //�����ɫ�������
	int             iAnimalIndex;
	short			headModelID;
	short			headImageID;
	float           fPs;
	bool            bRandom;                               //�������
	int             iRandomAnimalIndex;
	short           headRandomModelID;
	short           headRandomImageID;
	int             m_ePrevHeadID;
	int             m_eNextHeadID;
	long            m_iLastServer;                         //��¼��һ��ѡ�еķ�����
	bool            bSelectUserWithPwd;                    //���������Ϸ
	int             m_nDisconnectCause;                    //����Ͽ�ԭ��
	bool            bSaveAccount;
};





#endif // _INC_GC_LOGIN_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

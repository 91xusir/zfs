
#ifndef GC_WORLD_H
#define GC_WORLD_H

#include "game_states_machine.h"
#include "gc_missile_manager.h"
#include "net_session.h"
#include "net_synchro_position.h"
#include "gc_dnumber.h"
#include "gc_chat.h"
#include "gc_team_with_ui.h"
#include "gc_friend_with_ui.h"
#include "gc_mail_with_ui.h"
#include "gc_synchro_item.h"
#include "gc_bulletin.h"
#include "ui_project_shell.h"
#include "union_base.h"
#include "gc_master_list.h"

class CGameClientFrame;
class GcPlayer;
class GcScene;
extern int m_lastTouchCursor;
extern G_MEMDEF(s_strCheckProcessName, 200) 
struct LINEINFO
{
	DWORD dwUserID;
	DWORD dwGroupID;
	DWORD dwNowLineID;
};

struct SGcDungeon
{
	GcScene				  *scene;			// ����
	long                  lDungeonUID;		// ����ΨһID
	long                  lDungeonTID;		// ��������ID
	SFuBen                *info;			// ������Ϣ
	std::list<DWORD>      listUser;			// �û��б�
	time_t                tCreateTime;		// ����ʱ��
	time_t                tLimitTime;		// ����ʱ�䣬�������ʱ���ǿ���˳�
	time_t                tEndTime;			// ����ʱ�䣬�������ʱ����˳����������û��˳�������ʱ����������ʱ��
	long				  process;			// ��������ֵ
	std::vector<STrigger> trigger;			// ����

	inline STrigger* GetTrigger(DWORD dwID)
	{
		STrigger* pTrigger = NULL;
		std::vector<STrigger>::iterator it = trigger.begin();
		for (;it!=trigger.end();++it)
		{
			if ((*it).id == dwID)
			{
				pTrigger = &(*it);
				break;
			}
		}
		return pTrigger;
	}
};

class GcScene : public RtScene
{
public:
    GcScene(){m_pWorld=NULL;}
    virtual ~GcScene(){}
    void SetWorld(GcWorld* pWorld) { m_pWorld = pWorld; }

protected:
    virtual void OnAfterPreRender();
    virtual void OnLoadMap(const char* szMapFileName, bool bLoadAll);
    GcWorld* m_pWorld;
};

struct PlayEffectActorList
{
	PlayEffectActorList(DWORD userID = 0,string effect = "",bool link = true, 
		string linkSlot = "root",RtgVertex3 offset = RtgVertex3(0.f,0.f,0.f),
		string posename = "effect", bool loop = false,float speed = 1.f,DWORD waitTime = 5000)
	{
		dwUserID	= userID;
		szEffect	= effect;
		bLink		= link;
		szLinkSlot	= linkSlot;
		vctOffset	= offset;
		szPoseName	= posename;
		bLoop		= loop;
		fSpeed		= speed;
		dwStartTime = rtGetMilliseconds();
		dwWaitTime	= waitTime;
	}

	DWORD	dwUserID;
	string	szEffect;
	bool	bLink;
	string	szLinkSlot;
	RtgVertex3 vctOffset;
	string	szPoseName;
	bool	bLoop;
	float	fSpeed;
	DWORD	dwStartTime;
	DWORD	dwWaitTime;
};

/*----------------------------------------------------------------------------
-   GcWorld.
----------------------------------------------------------------------------*/


class CItemManager;
class GcWorld : public GcUserInput
{
public:
	enum EStates
	{
		NULL_STATE,				//��״̬
		LOGIN_STATE,			//��¼״̬
		CREATE_CHARACTER_STATE,	//������ɫ
		GAME_PLAYING_STATE,		//��Ϸ��״̬
	};

    enum ECameraMode
    {
        CAMERA_MODE_LOCK,
        CAMERA_MODE_TURN,
        CAMERA_MODE_FREE,
    };

	enum ENetDisconnectCause
	{
		NET_DISCONNECT_CAUSE_UNKNOWN,
		NET_DISCONNECT_CAUSE_SWITCH_REGION,		
		NET_DISCONNECT_CAUSE_MULTI_LOGIN,
		NET_DISCONNECT_CAUSE_POT_NOT_ENOUTH,
		NET_DISCONNECT_CAUSE_GM,
        NET_DISCONNECT_CAUSE_USERDEFINE,
	};

    // �º�����д��ǰ��һ���Ƿ�����ң�����һ����ÿ�ζ��ı亯���ı���λ�ã������ҲҪ���²���λ��
    void OnBagSkillUsed(void* pData);

    GcWorld (CGameClientFrame* pGameClientFrame);
    virtual ~GcWorld();

    bool InitOnce();                    // ֻ��ʼ��һ�εĳ�ʼ��
    bool ClearOnce();                   // ֻ���һ�ε����

    bool EnterGame();                   // ������Ϸ��ʱ����� (����Ӧ���д󲿷ֵĳ�ʼ��)
    bool LeaveGame();                   // �˳���Ϸ��ʱ����� (����Ӧ���д󲿷ֵ�����)

    bool EnterRegion();                 // ���볡��
    bool LeaveRegion();                 // �뿪����

    virtual void OnRun(float fSecond);          // �߼���Ⱦ
    virtual void OnBeginRender();				// ͼ����Ⱦǰ
    virtual void OnRender(float fSecond);       // ͼ����Ⱦ
    virtual void OnEndRender();					// ͼ����Ⱦ��

	virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY);
	virtual void OnMouseHoverMove(int iButton, int x, int y, int increaseX, int increaseY);
	virtual void OnMouseDragMove(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseWheel(int iButton, long vDelta);
	virtual void OnMouseLDown(int iButton, int x, int y);
	virtual void OnMouseUp(int iButton, int x, int y);
	virtual void OnMouseLDClick(int iButton,int x, int y);
	virtual void OnMouseRDown(int iButton,int x,int y);

    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
	virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);//  [8/13/2009 tim.yang] mouse middle button drag

	virtual void OnKeyDown(int iButton,int iKey);
    virtual void OnKeyUp(int iButton,int iKey);
	void OnKeyChar(int iChar);

	virtual	void OnKillFocus();
	virtual	void OnSetFocus();

	void OnPopupMenuOnlyByName(RtwPopupMenu* pPopupMenu,string& rActorName);
    void OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor);
    void OnHUDConfigChanged();

	bool OnMemberRClick(DWORD vDBID,const char* vpName);//�����һ����Ա��ѡ��
    // �����¼�
    void EnterRegionServer();
    void OnNetConnect(bool bSucceed);		//��������
    void OnNetDisconnect();					//�������ӶϿ�
    void OnNetCurrentProgress(float fProgress);         // �����ڶ���ʱ�䵽���һ��ʱ��֮��Ľ��� (�ٷֱ�)
	void OnNetKick(char cCause, char* szCause);

    // ���ص���������
    void OnNetDownloadCreateChar(CG_CmdPacket* vpPacket);//���ǵĳ�ʼ��
    void OnNetDownloadSnapshot(CG_CmdPacket *pPacket, DWORD dwServerTime, DWORD dwClientTime);
    void OnNetDownloadEvent(CG_CmdPacket *pPacket, DWORD dwClientTime);
    //lyymark ���ع�������
    void OnNetDownloadAttack(CG_CmdPacket *pPacket, DWORD dwServerTime, DWORD dwClientTime);
	void OnNetDownloadHelp(CG_CmdPacket *pPacket, DWORD dwServerTime, DWORD dwClientTime);
    void OnNetDownloadShowInfo(CG_CmdPacket *pPacket);
    void OnNetDownloadRidePet(CG_CmdPacket *pPacket);
    void OnNetDownloadDialogHTML(DWORD dwNpcID, const char* szString, const char* szName, const char* szValue, DWORD taskid, char cFinished);
	void OnNetDownloadDialogScript(short sType, char* sShowText, char* sCommandText);
    void OnNetDownloadDialogByType(DWORD dwNpcID, WORD wType);
    void OnNetDownloadSwitchRegion(CG_CmdPacket *pPacket);
	void OnNetDownloadShortcut(CG_CmdPacket* pPacket);
	void OnNetDownloadConfigData(CG_CmdPacket* pPacket);
	
	// �ϴ�����������
	void OnNetUploadBreakIntonate();
    void OnNetUploadPosition(DWORD dwClientTime);
	void OnNetUploadRidePet(char op);
    void OnNetUploadAttackPosition(WORD wSkillID, WORD wAttackID, const DWORD* pPos, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadAttackTarget(WORD wSkillID, WORD wAttackID, DWORD dwObjectID, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadAttackSelf(WORD wSkillID, WORD wAttackID, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadInteractiveNpc(DWORD dwNpcID, const char* szString);
	void OnNetUploadShortcut();
	void OnLocalSaveShortcut();   //add by yz : ������������ڱ����ļ���
	void OnNetUploadConfigData();

	

    // Hit Test, iStyle = 0xFFȫ��, 0x01�Լ�, 0x02 �Լ��ĳ���, 0xFC�������Լ��ͳ���
    GcActor* HitTest(int x, int y, int iStyle = 0xFC);
    GcActor* HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, int iStyle = 0xFC);
    bool CanTouch(GcActor* pActor);
    bool CanAttack(GcActor* pActor, bool bShowMsg=false);
    bool CanUseSkill(GcActor* pActor, SSkill* pSkill, bool bShowMsg=false);
	bool CanMove(bool isMiddleMap = false);
	/* gao 2009.12.22
	*/
	void UseSkill(SSkill* pAttr,int x,int y);
	void CheckSkill();
	RtwWidget* GetHintWidget(){guard;return (RtwWidget*)m_frmHint;unguard;}
	bool CanUseShortcut();
	void OnSetTabSelectTarget();
	GcActorSkill* OnGetBuffHintForSkill();
	/* end */
    SSkill* GetActiveSkill();
	
    void OnAfterScenePreRender();
    void OnPetActive(DWORD dwPetID,DWORD itemid);
    void OnInputCardID(long lNpcID, long lType, const char* szCardID, const char* szCardPwd);
	void SetDefaultCamerasetting();
    void SetDefaultCamera();
    void SetCameraMode(ECameraMode eMode);
    ECameraMode GetCameraMode()         { return m_eCameraMode; }
    bool IsInit()                       { return m_bInit; }

    inline RtScene* GetScene()          { return g_pScene; }
	inline RtsPathFind* GetPathFind()   { return &m_PathFind; }

    bool SetSceneFile(const char* szFilename);

	void SetTriggerBlock(int id);
	void RefreshTriggerBlockPos();
	void PlayActorEffect(DWORD userID,string effect,bool link = true, 
		string linkSlot = "root",RtgVertex3 offset = RtgVertex3(0.f,0.f,0.f),
		string posename = "effect", bool loop = false,float speed = 1.f,DWORD waitTime = 5000);

#if DO_CONSOLE
	void DebugBlockTerrain(bool vDisplay){mDebugBlockTerrain=vDisplay;}
#endif

    bool Postion2DTo3D(int v2DX,int v2DY,float& vX,float& vY,float& vZ);
    bool GetMouseVector(RtgVertex3& vOrg, RtgVertex3& vDir);
    bool TranslateTaskMacro(string& result, const char* text, void* data);
//--------------------add start by tony 05.06.10-----------------------------------//
	bool IsCastleWarTime(GcActor* p_actor);	//��ս���
	bool IsCastleWarTime(std::string feod_name);	
//--------------------add end   by tony 05.06.10-----------------------------------//
	DWORD GetActualNPCId(){ return m_dwActualNPCID; }
	DWORD GetInteractiveNpcId(){ return m_dwInteractiveNPCID; }
	DWORD GetUploadTaskId(){ return m_upTaskId; }
	void OnMouseRightDown(RtwWidget* vpSender, RtwEventDelegate* e);
	void SetCommandText(char* sCommandText){ strCommandText = sCommandText; }
	std::string GetCommandText(){ return strCommandText; }
	std::string GetSceneFileName(){ return m_szSceneFileName; }
	void OnNetDownloadCharGuide(short sType);
	void ResetHideTime();
	void HideGuideForm();
	void SetHideGuide();
	void ShowTaskGuide();             //��ʾ����׷�ٳ�����
	void ShowKeyboardGuide();         //��ʾ���̲���
	void ShowAttrPoint();             //��ʾ���Ե����
	void ShowAttrPointAllocate();     //��ʾ���Ե����
	void ShowSkillPage();             //����״δ򿪼���ҳ��
	void ShowSkillPoint();            //��Ҽ��ܼӵ�
	void ShowCallPet();               //����ٻ�����ͼ��
	void ShowPetInfo();               //��ҵ�ǰ�ٻ�������
	void ShowShopUI();                //��Ҵ��̵�ҳ��
	void ShowShopTrade();             //��ҵ����Ʒ���ֹ���ҳ��
	void ShowLifePage();              //��ҵ�������ҳ��
	void ShowViewMail();              //��Ҳ鿴�ʼ�
	void ShowExtraMail();             //��Ҳ鿴����
	void ShowStudySkill();            //���ѧϰ�����
	void ShowFriendUI();              //��һ�ú���
	void ShowMiddleMap();             //�����ʾ�е�ͼ
	void ShowMiddleMapAI();           //�����ʾ�е�ͼѰ·
	void ShowPlayerInScreen();        //��ʾ����ͬ�����

	void EnterDungon(DWORD UID,DWORD TID,DWORD process);
	void LeaveDungon();
	SGcDungeon *GetDungeon();

public:
	//GcScene*                m_pScene;               // ����
    bool                    m_bUsePlayerLight;      // ʹ�����ͷ�ϵƹ�
    bool                    m_bRun;                 // ��������״̬
    bool                    m_bSwitchRegion;        // �����л�����״̬
    bool                    m_bOnlyShowNPC;         // ֻ��ʾNPC
	char					m_cUserJhWg;
	ENetDisconnectCause		m_nDisconnectCause;		// ����Ͽ���ԭ�����Ϊ�쳣���л�Region�򱻷������Ͽ�
    CM_MEMDEF(m_szDisconnectCause, 512)
    RtgLight                m_lightPlayer;          // ����Լ��ĵƹ�
    GcPlayer*               m_pPlayer;              // ����Լ�
    DWORD                   m_dwPetID;
	GcActor*				m_pPet;					// ��
	GcPet*					m_pGcPet;				// �޹���
	GcSynchroActorManager   m_ActorManager;         // ��ɫ�������ﲻ��������Լ�
    GcSynchroItem           m_synchroItems;         // ͬ�����߹���
    RtsPathFind             m_PathFind;             // ·��������
    NetSession              m_Net;                  // ����
    NetSynchroPosition      m_NetSynachro;          // ����ͬ��λ��
	CMissileManager*		m_pMissileManager;		// ��������Ĺ�����
	DWORD					m_selectedActor;		// ��ǰѡ�е�����
    GcDynamicNumber         m_dynamicNumber;        // ͷ�ϵ�����������
	CChat					m_Chat;
	CTeamWithUI				m_Team;
	CFriendWithUI			m_Friend;
	CMailWithUI				m_Mail;
    CGameClientFrame*		m_pGameClientFrame;     // �ͻ���
	CBulletinManager		m_Bulletin;
	CProject				m_UIProject;
	CGCMasterList			m_MasterList;			//ʦ���б�
    int                     m_iCheckPlayerSeedSave;

	DWORD					m_nPetItemID;	

    bool                    m_bKeyAlt;
    bool                    m_bKeyShift;
    bool                    m_bKeyCtrl;
    bool                    m_bLogined;
    time_t                  m_tLogoutTime;

	CItemManager			*m_pItemManager;
	DWORD                   m_dNPCNameColor;
	DWORD                   m_dNPCID;
	DWORD                   m_dwActualNPCID;          //NPC���е�ID
	DWORD					m_dwInteractiveNPCID;
	DWORD                   m_upTaskId;               //�ϴ�����ID
	string                  strCommandText;           //�ű������ı�
	RtString				m_InteractiveNPCName;
	bool					bMainLine;
	std::string             m_szSceneFileName;        //�����ļ��� 
	//����ָ��
	bool                    bCurOperate;
//--------------------add start by tony 05.06.10-----------------------------------//
	bool					m_bCastleWarTime[FEOD_COUNT];	//��ս���
	/* 0: "��᪳�" 1: "�����" 2: "���ƹ�" */
//--------------------add end   by tony 05.06.10-----------------------------------//
	int                     m_iCheckPlayerSeed;//tim.yang
	int						m_iTeleport;			//ac.ma
	int						m_iDeadport;			//heten
	bool					m_bIsMovableCursor;		//
	RtwHtmlView*			m_frmHint;

	std::list<PlayEffectActorList>	m_listPlayEffectActor;
	SGcDungeon*				m_pDungeon;
protected:

    int                     m_iGameDate;            // ��Ϸ����
    int                     m_iGameMinute;          // ��Ϸʱ��(��0:0�ķ�����)
    float                   m_fNetCurrentProgress;  // �����ڶ���ʱ�䵽���һ��ʱ��֮��Ľ��� (�ٷֱ�)

    ECameraMode             m_eCameraMode             ; // �����ģʽ, 1����, 2������ת, 3��ȫ����
    float                   m_fCameraDefaultYaw       ;
    float                   m_fCameraDefaultDegree    ;
    float                   m_fCameraMinDistance      ;
    float                   m_fCameraMaxDistance      ;
    float                   m_fCameraDefaultDistance  ;
    float                   m_fCameraCurrentDistance  ;
	float                   m_fCurrentRollRadian;
    /*int                     m_iCheckPlayerSeed;*/
	RtwLabel				*m_pLbl_Fps;

	std::list<RtgVertex3>		m_listBlockAdd;
	std::list<RtgVertex3>		m_listBlockKill;
public:
	void					ShowDebugInfo();

private:
	void SetCamera();

    bool                    m_bInit;
    bool                    m_bStateChangeFrame;    // �Ƿ���״̬�ı�ĵ�һ֡
	bool					m_bLockCamera;			// �������

    HASH_STR(std::string, std::string)  m_mapTranslateMacro;

    //bool                                m_bImmeInput;   // ����ģʽ trueֱ�����룬 false��Ҫ�س�����
    //std::map<DWORD, RtwEventDispatcher> m_listShortcut; // �������

	RtgVertex3				m_Camera;				// ������ʱ�����λ��
    //RtwAliasButton*         m_pAliasButton; //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�

    CM_MEMDEF(m_szLastSceneFile, 128)
    CM_MEMDEF(m_szSceneFile, 128)

    int                     m_iTmpPacketSize;
    char*                   m_pTmpPacketData;

    bool                    m_bHackChecked;
    bool                    m_bCheckProcess;
    std::string             m_szCheckProcess;
    long                    m_dwCheckProcessID;

	int						m_drawTerrainBlock;//tim.yang  draw terrain block

	int						m_iKey1;
	int						m_iKey2;
	bool					m_bIsControlByKeyBoard;

	/* gao 2010.2.26
	��ҵİ����ƶ��¼�
	*/
	void UseKeyBoardMove();
	void OnMoveKeyDown(int direction);
	void OnMoveKeyUp(int direction);
public:
	bool                    m_bCurrentSelected;//����Ƿ�ѡ����Һ���
	char					m_cRideRetard;			// ���״̬�ӳٴ���
	bool					m_bKeyBoardMoving;
	bool					m_bFreeCamera;

#if DO_CONSOLE
	bool					mDebugBlockTerrain;		// �ر��赲�ĵ���
#endif
};

// ���Ĳ�������
enum ECursorTypeID
{
	cti_Normal					= 0,	// ����״̬
	cti_Decompose				= 1,	// �ֽ�װ��
	cti_Enchant					= 2,	// ��ħ
	cti_Split					= 3,		// ���װ��
	//cti_Repair					= 4     //����
};


// lua
CILua *Lua();
bool   LuaInit();
bool   LuaExit();

#endif//GC_WORLD_H
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

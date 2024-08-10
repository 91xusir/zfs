
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
	GcScene				  *scene;			// 场景
	long                  lDungeonUID;		// 副本唯一ID
	long                  lDungeonTID;		// 副本类型ID
	SFuBen                *info;			// 副本信息
	std::list<DWORD>      listUser;			// 用户列表
	time_t                tCreateTime;		// 创建时间
	time_t                tLimitTime;		// 极限时间，超过这个时间会强制退出
	time_t                tEndTime;			// 结束时间，超过这个时间会退出，在所有用户退出副本的时候会设置这个时间
	long				  process;			// 副本进度值
	std::vector<STrigger> trigger;			// 机关

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
		NULL_STATE,				//空状态
		LOGIN_STATE,			//登录状态
		CREATE_CHARACTER_STATE,	//创建角色
		GAME_PLAYING_STATE,		//游戏中状态
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

    // 新函数，写在前面一个是方便查找，另外一个是每次都改变函数的编译位置，让外挂也要重新查找位置
    void OnBagSkillUsed(void* pData);

    GcWorld (CGameClientFrame* pGameClientFrame);
    virtual ~GcWorld();

    bool InitOnce();                    // 只初始化一次的初始化
    bool ClearOnce();                   // 只清除一次的清除

    bool EnterGame();                   // 进入游戏的时候调用 (这里应该有大部分的初始化)
    bool LeaveGame();                   // 退出游戏的时候调用 (这里应该有大部分的销毁)

    bool EnterRegion();                 // 进入场景
    bool LeaveRegion();                 // 离开场景

    virtual void OnRun(float fSecond);          // 逻辑渲染
    virtual void OnBeginRender();				// 图形渲染前
    virtual void OnRender(float fSecond);       // 图形渲染
    virtual void OnEndRender();					// 图形渲染后

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

	bool OnMemberRClick(DWORD vDBID,const char* vpName);//队伍的一个成员被选择
    // 网络事件
    void EnterRegionServer();
    void OnNetConnect(bool bSucceed);		//网络连接
    void OnNetDisconnect();					//网络连接断开
    void OnNetCurrentProgress(float fProgress);         // 倒数第二次时间到最后一次时间之间的进度 (百分比)
	void OnNetKick(char cCause, char* szCause);

    // 下载的网络数据
    void OnNetDownloadCreateChar(CG_CmdPacket* vpPacket);//主角的初始化
    void OnNetDownloadSnapshot(CG_CmdPacket *pPacket, DWORD dwServerTime, DWORD dwClientTime);
    void OnNetDownloadEvent(CG_CmdPacket *pPacket, DWORD dwClientTime);
    //lyymark 下载攻击命令
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
	
	// 上传的网络数据
	void OnNetUploadBreakIntonate();
    void OnNetUploadPosition(DWORD dwClientTime);
	void OnNetUploadRidePet(char op);
    void OnNetUploadAttackPosition(WORD wSkillID, WORD wAttackID, const DWORD* pPos, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadAttackTarget(WORD wSkillID, WORD wAttackID, DWORD dwObjectID, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadAttackSelf(WORD wSkillID, WORD wAttackID, GcActor::SCmd* pCmd,int sum);
    void OnNetUploadInteractiveNpc(DWORD dwNpcID, const char* szString);
	void OnNetUploadShortcut();
	void OnLocalSaveShortcut();   //add by yz : 将快捷栏保存在本地文件中
	void OnNetUploadConfigData();

	

    // Hit Test, iStyle = 0xFF全部, 0x01自己, 0x02 自己的宠物, 0xFC不包含自己和宠物
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
	bool IsCastleWarTime(GcActor* p_actor);	//城战相关
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
	void ShowTaskGuide();             //显示任务追踪超链接
	void ShowKeyboardGuide();         //显示键盘操作
	void ShowAttrPoint();             //显示属性点添加
	void ShowAttrPointAllocate();     //显示属性点分配
	void ShowSkillPage();             //玩家首次打开技能页面
	void ShowSkillPoint();            //玩家技能加点
	void ShowCallPet();               //玩家召唤宠物图标
	void ShowPetInfo();               //玩家当前召唤出宠物
	void ShowShopUI();                //玩家打开商店页面
	void ShowShopTrade();             //玩家点击物品出现购买页面
	void ShowLifePage();              //玩家点击生活技能页面
	void ShowViewMail();              //玩家查看邮件
	void ShowExtraMail();             //玩家查看附件
	void ShowStudySkill();            //玩家学习生活技能
	void ShowFriendUI();              //玩家获得好友
	void ShowMiddleMap();             //玩家显示中地图
	void ShowMiddleMapAI();           //玩家显示中地图寻路
	void ShowPlayerInScreen();        //显示屏蔽同屏玩家

	void EnterDungon(DWORD UID,DWORD TID,DWORD process);
	void LeaveDungon();
	SGcDungeon *GetDungeon();

public:
	//GcScene*                m_pScene;               // 场景
    bool                    m_bUsePlayerLight;      // 使用玩家头上灯光
    bool                    m_bRun;                 // 正在运行状态
    bool                    m_bSwitchRegion;        // 正在切换区域状态
    bool                    m_bOnlyShowNPC;         // 只显示NPC
	char					m_cUserJhWg;
	ENetDisconnectCause		m_nDisconnectCause;		// 网络断开的原因可能为异常、切换Region或被服务器断开
    CM_MEMDEF(m_szDisconnectCause, 512)
    RtgLight                m_lightPlayer;          // 玩家自己的灯光
    GcPlayer*               m_pPlayer;              // 玩家自己
    DWORD                   m_dwPetID;
	GcActor*				m_pPet;					// 兽
	GcPet*					m_pGcPet;				// 兽管理
	GcSynchroActorManager   m_ActorManager;         // 角色管理，这里不包含玩家自己
    GcSynchroItem           m_synchroItems;         // 同步道具管理
    RtsPathFind             m_PathFind;             // 路径搜索类
    NetSession              m_Net;                  // 网络
    NetSynchroPosition      m_NetSynachro;          // 网络同步位置
	CMissileManager*		m_pMissileManager;		// 发射物体的管理者
	DWORD					m_selectedActor;		// 当前选中的物体
    GcDynamicNumber         m_dynamicNumber;        // 头上的跳动的数字
	CChat					m_Chat;
	CTeamWithUI				m_Team;
	CFriendWithUI			m_Friend;
	CMailWithUI				m_Mail;
    CGameClientFrame*		m_pGameClientFrame;     // 客户端
	CBulletinManager		m_Bulletin;
	CProject				m_UIProject;
	CGCMasterList			m_MasterList;			//师傅列表
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
	DWORD                   m_dwActualNPCID;          //NPC表中的ID
	DWORD					m_dwInteractiveNPCID;
	DWORD                   m_upTaskId;               //上传任务ID
	string                  strCommandText;           //脚本命令文本
	RtString				m_InteractiveNPCName;
	bool					bMainLine;
	std::string             m_szSceneFileName;        //场景文件名 
	//新手指引
	bool                    bCurOperate;
//--------------------add start by tony 05.06.10-----------------------------------//
	bool					m_bCastleWarTime[FEOD_COUNT];	//城战相关
	/* 0: "西岐城" 1: "朝歌城" 2: "穿云关" */
//--------------------add end   by tony 05.06.10-----------------------------------//
	int                     m_iCheckPlayerSeed;//tim.yang
	int						m_iTeleport;			//ac.ma
	int						m_iDeadport;			//heten
	bool					m_bIsMovableCursor;		//
	RtwHtmlView*			m_frmHint;

	std::list<PlayEffectActorList>	m_listPlayEffectActor;
	SGcDungeon*				m_pDungeon;
protected:

    int                     m_iGameDate;            // 游戏日期
    int                     m_iGameMinute;          // 游戏时间(离0:0的分钟数)
    float                   m_fNetCurrentProgress;  // 倒数第二次时间到最后一次时间之间的进度 (百分比)

    ECameraMode             m_eCameraMode             ; // 摄像机模式, 1锁定, 2可以旋转, 3完全自由
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
    bool                    m_bStateChangeFrame;    // 是否是状态改变的第一帧
	bool					m_bLockCamera;			// 锁定相机

    HASH_STR(std::string, std::string)  m_mapTranslateMacro;

    //bool                                m_bImmeInput;   // 输入模式 true直接输入， false需要回车输入
    //std::map<DWORD, RtwEventDispatcher> m_listShortcut; // 快捷输入

	RtgVertex3				m_Camera;				// 非锁定时的相机位置
    //RtwAliasButton*         m_pAliasButton; //gao 2009.12.18 去除原右键技能框

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
	玩家的按键移动事件
	*/
	void UseKeyBoardMove();
	void OnMoveKeyDown(int direction);
	void OnMoveKeyUp(int direction);
public:
	bool                    m_bCurrentSelected;//检测是否选中玩家和兽
	char					m_cRideRetard;			// 骑宠状态延迟处理
	bool					m_bKeyBoardMoving;
	bool					m_bFreeCamera;

#if DO_CONSOLE
	bool					mDebugBlockTerrain;		// 地表阻挡的调试
#endif
};

// 鼠标的操作种类
enum ECursorTypeID
{
	cti_Normal					= 0,	// 正常状态
	cti_Decompose				= 1,	// 分解装备
	cti_Enchant					= 2,	// 附魔
	cti_Split					= 3,		// 拆分装备
	//cti_Repair					= 4     //修理
};


// lua
CILua *Lua();
bool   LuaInit();
bool   LuaExit();

#endif//GC_WORLD_H
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

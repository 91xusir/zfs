
// С��ͼ
#ifndef _INC_UI_FORM_MINIMAP_H_
#define _INC_UI_FORM_MINIMAP_H_

class UIFormMiniMap:public GcTimerListener
{
public:
    enum EMinimapShowFlag
    {
        EMMSF_SHOW_MAP      = 0x0001,
        EMMSF_SHOW_PLAYER   = 0x0002,
        EMMSF_SHOW_TEAM     = 0x0004,
        EMMSF_SHOW_USER     = 0x0008,
        EMMSF_SHOW_MONSTER  = 0x0010,
        EMMSF_SHOW_NPC      = 0x0020,
        EMMSF_SHOW_FLAG     = 0x0040,
		//--------add start by tony 05.07.13--------------//
		EMMSF_SHOW_CWAR_ATTACKER     = 0x0080,
		EMMSF_SHOW_CWAR_DEFENDER     = 0x0100,
		//--------add end   by tony 05.07.13--------------//
		EMMSF_SHOW_NOFLAG	= 0xFFBF,
        EMMSF_SHOW_ALL      = 0xFFFF,
    };
	struct SLineInfo
	{
		DWORD dwLineID;
		DWORD dwUserCount;
		DWORD dwMaxUserCount;
	};
	struct SNpcPoint
	{
		int     x, y;
		DWORD   id;
	};
    UIFormMiniMap();
    ~UIFormMiniMap();

    void OnDrawMiniMap(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnMapMouseMove(int iMouseX, int iMouseY);
	
    bool MapIsShow();

    void SetScale(float fScale);
    float GetScale()                { return m_fScale; }

	void WarViewHide()	{ m_pBtnWarView->Hide(); }
	void WarViewShow()	{ m_pBtnWarView->Show(); }

    void SetSceneName(const char* szName);
	void SetSceneName();
    void SetPosition(float fX, float fY);
	void SetPKState(EPKAttr ePKAttr);

	void WarfieldShow(ui::RtwWidget* sender, RtwEventDelegate* e);
	void PlayerListUIChange(ui::RtwWidget* sender, RtwEventDelegate* e);//����б����仯
	void SysBattleSetting(ui::RtwWidget* sender, RtwEventDelegate* e);  //add by yz �����ر��ڹҽ���
	void OnTimerEvent(DWORD vID);										//ʱ��ص�����
	void PopMenu(ui::RtwWidget* sender, RtwEventDelegate* e);			//pop�˵����г�
	void PopMenuClose(ui::RtwWidget* sender, RtwEventDelegate* e);//pop�˵��ر�
	void PopMenuReset(ui::RtwWidget* sender, RtwEventDelegate* e);//pop�˵����ر�
	// gao 2010.1.28 ���������Ӧ�¼�����
	void OnSelectChildThread(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnClicked_MainTask(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnChildThreadButtonDown(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnReceiveLineCmd(CG_CmdPacket *pvPacket);
	void OnReceiveSwichLineRes(CG_CmdPacket *pvPacket);
	void OnReceiveLineInfo(CG_CmdPacket *pvPacket);
	void OnReceiveEnterLineNotify(CG_CmdPacket *pvPacket);
	void OnReceiveLeaveLineNotify(CG_CmdPacket *pvPacket);

	void DrawMiniMap(int iScreenX, int iScreenY, int iScreenWidth, int iScreenHeight, float fSceneWidth, WORD wShowFlag=EMMSF_SHOW_NOFLAG);
	void OnMapMouseMoveEvent(ui::RtwWidget* sender, RtwEventDelegate* e);
	void OnGetNpcHintText(ui::RtwWidget* sender, RtwEventDelegate* e);
	void OnAreamapMoveTo(ui::RtwWidget* sender,RtwEventDelegate* e);//max.zhou moveto Areamap
    void OnButtonShowWorldMap(ui::RtwWidget* sender, RtwEventDelegate* e); // ��ʾ�����ͼ

	// ���ڶ�̬�ƶ�����
	void RunAnimation();
	void DoFrame();
	void OnButtonLockCamera(ui::RtwWidget* sender, RtwEventDelegate* e);

	void OnRefreshCanAcceptTask();
protected:

    void OnButtonZoomInClick(ui::RtwWidget* sender, RtwEventDelegate* e);  // �Ŵ�
    void OnButtonZoomOutClick(ui::RtwWidget* sender, RtwEventDelegate* e); // ��С
    void OnButtonShowHideClick(ui::RtwWidget* sender, RtwEventDelegate* e); // �Ƿ���ʾС��ͼ
	void OnMoveTo(ui::RtwWidget* sender,RtwEventDelegate* e);//tim.yang hero moveto on minimap
	void UpdatePlayerListUI();//��������б����ʾ
	void GetMiniMapHint(ui::RtwWidget* sender, RtwEventDelegate* e);
	DWORD UIIndex2PlayerRegionID(int vUIIndex);//����ID�����RegionID
	
private:

    RtgTextItem*		m_texArrayTexture;
	RtgTextItem*		m_texMonster;
	RtgTextItem*		m_texNPC;
	RtgTextItem*		m_texPet;
	RtgTextItem*		m_texTeam;
	RtgTextItem*		m_texPlayer;
	RtgTextItem*		m_texDesFar;
	RtgTextItem*		m_texDesNear;
	RtgTextItem*		m_texCamera;
	RtgTextItem*		m_texMine;
	RtgTextItem*		m_texHerbs;
	RtgTextItem*		m_texSeekingpoint;

    DWORD       m_dwColorPlayer;
    DWORD       m_dwColorPet;
    DWORD       m_dwColorTeam;
    DWORD       m_dwColorUser;
    DWORD       m_dwColorMonster;
    DWORD       m_dwColorNpc;
	//--------add start by tony 05.07.13--------------//
	DWORD		m_dwColorAttacker;
	DWORD		m_dwColorDefender;
	//--------add end   by tony 05.07.13--------------//

	RtwButton*	m_pBtnWarView;
	RtwWidget*	m_pwdtMiniMap;
	RtwWidget*	m_pwdtMapTitle;
    RtwWidget*  m_pWidgetPosition;
    RtwWidget*  m_pWidgetSceneName;
	RtsSceneBlockMap* m_pblockmap;
	//RtwListBox*	m_pUIPlayerList;	//����б�
	RtwImage*	m_pImage;
	RtwButton*	m_pLockCamera;

    RtgShader   m_ShaderPoint;

    float       m_fScale;
    float       m_fPlayerX, m_fPlayerY;
	long m_iAreaX, m_iAreaY;
    //SNpcPoint   m_ptNpcs[40];
	vector<SNpcPoint>	m_vecNPCs;
    int         m_iNpcCount;
    SNpcPoint   m_lastNpcHint;
	vector<RtwLabel*> m_vectorNPCLable;
	vector<STaskDesc*>			m_vectorCanAccept;
	vector<RtwLabel*>			m_vectorTaskLabel;


	struct SUIDate
	{
	SUIDate():mUIIndex(-1),mpActor(NULL){};
	SUIDate(int vUIIndex,GcActor* vpActor):mUIIndex(vUIIndex),mpActor(vpActor){};
	
	int			mUIIndex;
	GcActor*	mpActor;
	DWORD		mPlayerID;
	string		mName;
	};

	map<int,DWORD>	mUI2PlayerID;
	list<DWORD>		mPlayerIDList;
	bool			mPopMenuOpen;
	/* gao 2010.2.1 ������ر��� */
	std::string		m_strMapName;
	std::list<SLineInfo>	m_lstLineInfo;
	RtwListBox*		m_plstLineContent;

	// �����ƶ�
	bool m_bIsRunning;
	bool m_bShow;
	float m_fStep;
};

#endif // _INC_UI_FORM_MINIMAP_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

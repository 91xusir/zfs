#ifndef _INC_UI_FORM_SKILL_H_
#define _INC_UI_FORM_SKILL_H_

const char* GenerateSkillHtmlHint(SSkill* pAttr, WORD wID, bool bStudied, bool bNextLev , int &iLine, const char* szAddinStr);
class UIFormSkill
{
    enum
    {
        SKILL_LIST_SIZE = 7,	//2010.1.7 原9个
        SKILL_GRID_MAT  = 4,
        SKILL_GRID_PAGE = 2,	//新技能只有两页
        SKILL_GRID_LINE = 8,
        SKILL_GRID_ROW  = 4,
		SKILL_COMMON_NUM = 15,	// 通用技能个数
		SKILL_EXPRESSION_NUM = 15,	// 表情技能个数
		SKILL_BOOK_NUM  = 5,
		SKILL_TRUMP_NUM = 5,
    };

    unsigned short  m_wMap[SKILL_GRID_MAT][SKILL_GRID_PAGE][SKILL_GRID_LINE][SKILL_GRID_ROW];
    std::string     m_szBackground[SKILL_GRID_MAT][SKILL_GRID_PAGE];
    RtwWidget*      m_pPanelMap[SKILL_GRID_PAGE][SKILL_GRID_LINE][SKILL_GRID_ROW];
    RtwButton*      m_pSkillMap[SKILL_GRID_PAGE][SKILL_GRID_LINE][SKILL_GRID_ROW];
    RtwWidget*      m_pLevelMap[SKILL_GRID_PAGE][SKILL_GRID_LINE][SKILL_GRID_ROW];
    RtwButton*      m_pAddButton[SKILL_GRID_PAGE][SKILL_GRID_LINE][SKILL_GRID_ROW];
    RtwWidget*      m_pPanelClass[SKILL_GRID_PAGE];
    RtwWidget*      m_pSP;
	RtwWidget*		m_pPP;
    int             m_iMetier;
    int             m_iLastClass;

public:
    void Show();
    void Hide();
    bool IsShowing();										// 是否已经显示
    void Refresh();
	void OnAddCanNotUseSkill(unsigned short subID);
	void OnDelCanNotUseSkill(unsigned short subID);
	void OnRefreshCanNotUseSkill();
    void UpdateSkillExp(short wID, short wExp, short wLevUpExp);
	bool CreateIconForGameClient(string imgStr,RtwImage** pImage);

private:
    int             m_iTopPosition;
    int             m_bUseScroll;
    RtwWidget*      m_pFrmThis;
	RtwButton*      m_btnClose;
    RtwScrollBar*   m_pScrollBar;
    RtwImage::Ptr   m_ptrImage;
    RtwWidget*      m_pWndSkillList[SKILL_LIST_SIZE];
    RtwButton*      m_pWndSkillListBtn[SKILL_LIST_SIZE];
    RtwWidget*      m_pWndSkillListName[SKILL_LIST_SIZE];
	RtwWidget*      m_pWndSkillListInfo1[SKILL_LIST_SIZE];
	RtwWidget*      m_pWndSkillListInfolb[SKILL_LIST_SIZE];
	RtwWidget*      m_pWndSkillListInfo2[SKILL_LIST_SIZE];
    RtwWidget*      m_pWndSkillListExp[SKILL_LIST_SIZE];
	RtwWidget*      m_pWndSkillListExpbpf[SKILL_LIST_SIZE];//ＥＸＰ　Label
	RtwWidget*      m_pWndSkillListExpTXT[SKILL_LIST_SIZE];//ＥＸＰ　Label
	RtwWidget*		m_pBtnForgetSkill[SKILL_LIST_SIZE];
    RtwProgressBar* m_pWndSkillListExpProgress[SKILL_LIST_SIZE];

	RtwLabel*		m_lbMeter1;
	RtwLabel*		m_lbMetertree1;
	RtwLabel*		m_lbMeter2;
	RtwLabel*		m_lbMetertree2;
	RtwImage*		m_imgImage1;
	RtwImage*		m_imgImage2;
	RtwImage*		m_imgImage3;
	RtwImage*		m_imgImage4;
	/* gao 2010.3.1
	技能面板的第三页相关数据
	*/
	RtwWidget*      m_pCommonSkill[SKILL_COMMON_NUM];
	RtwButton*      m_pCommonBtn[SKILL_COMMON_NUM];
	RtwWidget*      m_pCommonLev[SKILL_COMMON_NUM];

	RtwWidget*      m_pExpressionSkill[SKILL_EXPRESSION_NUM];
	RtwButton*      m_pExpressionBtn[SKILL_EXPRESSION_NUM];
	RtwWidget*      m_pExpressionLev[SKILL_EXPRESSION_NUM];

	RtwWidget*      m_pBookSkill[SKILL_BOOK_NUM];
	RtwButton*      m_pBookBtn[SKILL_BOOK_NUM];
	RtwWidget*      m_pBookLev[SKILL_BOOK_NUM];

	RtwWidget*      m_pTrumpSkill[SKILL_TRUMP_NUM];
	RtwButton*      m_pTrumpBtn[SKILL_TRUMP_NUM];
	RtwWidget*      m_pTrumpLev[SKILL_TRUMP_NUM];
	RtwWidget*      m_pTrumpAdd[SKILL_TRUMP_NUM];

	map<string,RtwImage*>		m_mapIcon;
	static RtwRect				m_imgRect[2][8];
	// 技能的禁用列表
	vector<unsigned short>		m_vecSkillSubID;

public:
#ifdef _DEBUG
	std::list<RtwButton*>   m_listCreateButton;
#endif

    void OnResize();
    void OnShow(void *,void *);
    void OnScroll(RtwWidget* sender,void *);
    void OnClass1Show(void *,void *);
    void OnClass2Show(void *,void *);
	void OnClass3Show(void *,void *);
	void OnClass4Show(void *,void *);

    int GetSkillItemIdx(const RtString& szStr); // -1 表示没有找到，否则返回相对的索引[0 base]
	bool CanStudySkill(SSkill *pSkill,GcActorSkill* pActorSkill);

	/* gao 2009.12.19 */
	void SetWidgetLabelName(int Metier);					// 将技能窗口名字初始化
	void OnRDownSkillItem(RtwWidget* sender, void*);
	void OnBtnForgetDown(RtwWidget* sender, void*);
	void OnBtnAddSkillDown(RtwWidget* sender, void*);
	void OnRefreshShortcut();
	/* end */
    void OnLClickSkillItem(RtwWidget* sender, void*);
    void OnLDClickSkillItem(RtwWidget* sender, void*);
    void OnGetHintTextSkillItem(RtwWidget* sender, void*);
    void OnDragSkillItem(RtwWidget* sender, void*);
    void OnDragEndSkillItem(RtwWidget* sender, void*);
    void OnSkillItemButtonClose(RtwWidget* sender, void*);

    void OnLClickCommonButton(RtwWidget* sender, void*);
    void OnLDClickCommonButton(RtwWidget* sender, void*);
    void OnGetHintTextCommonButton(RtwWidget* sender, void*);
	void OnMouseMove(RtwWidget* sender, void*);

    void OnLDClickTreeItem(RtwWidget* sender, void*);
    void OnGetTreeItemSkillHint(RtwWidget* sender, void*);
	void OnHideSkillPane(RtwWidget* sender, void*);

public:
    UIFormSkill();
    ~UIFormSkill();
};

#endif // _INC_UI_FORM_SKILL_H_

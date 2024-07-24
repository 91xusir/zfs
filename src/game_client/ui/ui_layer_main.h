#ifndef _INC_UI_LAYER_MAIN_H_
#define _INC_UI_LAYER_MAIN_H_

class UIFormSetting;
class UIFormPlayerItems;
class UIFormCharProp;
class UIFormPetProp;//tim.yang  ����ϵͳ
class UIFormPetSimple;
class UIFormCharSimple;
class UIFormTask;         //--����ϵͳ
class UIFormMainTask;     //--��������
class UIFormTrade;
class UIFormShop;
class UIFormPShop;
class UIFormLookPShop;
class UIFormBank;
class UIFormMsg;
class UIFormTextMsg;
class UIFormSkill;
class UIFormSkillStudy;
class UIFormMiniMap;
class UIFormUnion;
class UIFormHelp;
class UIFormUnionCreate;
class UIFormItemUpdate;
class UIFormFeodTax;
class UIFormFeodBank;
class UIFormFeodContent;
class UIFormHeroList;
class UIFormGm;
class UIFormMagicWeaponAddMp;
class UIFormMagicWeaponAddEle;
class UIFormRecoverPet;
class UIFormPkMode;
class UIFormClearSkill;
class UIFormRebirth;
//-------------------add start by tony 05.06.06------------------------------//
class UIFormCastleWar;
class UIFormCastleWarTimeSet;
class UIFormTimer;
class UIFormDupe;
//-------------------add end   by tony 05.06.06------------------------------//
class UIFormItemsInfo;
class UIFormCreInfo;
/**********************************gao 2009.11.5*************************************/
class UIFormMonInfo;//����Ѫ��UI
class CUI_Form_Chant;//����UI
class CUIForm_System;
class CUIForm_Speaker;
class CUIForm_Char;
class CUIForm_MiddleMap;
class CUIForm_Gem;
class CUIFormPlayerShop;
class CUIFormLookPlayerShop;
class CTimer_Hint;
/***************************************************************************************/
class UIFormSelectReward;
class UIFormCard;
class UIFormSellCard;
class UIFormAnalyseImage;
class UIFormBuyCard;
class UIFormBankPass;
class UIFormItemCompose;
class UIFormTimeBar;
class UIFormVendor;
class UIFormBid;
class UIFormSale;
class UIFormItemFuse;
class UIFormWarfield;
class UIFormPetEat;
class CUI_form_Team;
class CUIForm_Trump;
class CUIForm_Pickup;
class CUIForm_Collect;
class UIFormItemDecompound;
class UIFromPetMain;
class UIFromPetAptitude;
class UIFromPetCtrl;
class UIFormPetAbility;
class UIFormPetMerge;
class UIFormPetToBeads;
class UIFormPetMergeBeads;
class UIFormBankPet;
class UIFormShopCenter;

//��������ָ��
class CUIForm_NewPeopleHelp;
#define UI_HOT_KEY_COUNT		30
#define UI_BTN_REMIND_COUNT		3
#define UI_REMIND_MAX_SIZE		10

enum UIHotKeyType
{
	UIHotKey_Icon = 1,
	UIHotKey_Item = 2,
	UIHotKey_Skill = 3
};

enum UIRemindType
{
	UIRemind_NewMail = 1,
	UIRemind_LevelUp = 2,
	UIRemind_GmResponse = 3
};
enum HINT_WIDGET
{
	WIDGET_NULL = 0,
	WIDGET_SKILL_TREE = 1,
	WIDGET_SKILL_BUFF = 2,
	WIDGET_SKILL_SHORTCUT = 3,
	WIDGET_ITEAM_SHOW = 4
};
struct SETTING_ALL
{
	// setting
	bool		m_bShowSelfName;
	bool		m_bShowBlood;
	bool		m_bShowDegree;
	bool		m_bShowDamage;
	bool		m_bShowUnionName;
	bool		m_bShowEXP;
	bool		m_bShowOtherName;
	bool		m_bShowNPCName;
	bool		m_bShowOtherSpeak;
	bool		m_bShowTradeName;
	bool		m_bShowSelfSpeak;
	// refuse
	bool		m_bRefuseTeam;
	bool		m_bRefuseAction;
	bool		m_bRefuseMail;
	bool		m_bRefuseDuel;
	bool		m_bRefuseFriend;
	bool		m_bRefuseMonster;
	// skillshotcut3 show or not
	bool		m_bShowSkillShotcut;
	// music slience or not
	bool		m_bBackGroundMusicIsSlience;
	bool		m_bSoundIsSlience;
};
class UILayerMain
{
public:
	/**/
	void OnClicked_Union(void*, void*);

	void OnUpdateText_Chat(RtwWidget* sender, void*);

	void OnClick_HotKey(RtwWidget* sender, void*);

	//void OnAutoScroll_Feedback(RtwWidget* sender, void*);

	//void OnScroll_ManualScroll(RtwWidget* sender, void*);	

	void OnKeyChar(void*, void*);

	void OnClicked_System_System(void*, void*);

	void OnClicked_System_Char(void*, void*);

	void OnClicked_System_Map(void*, void*);
	void OnClicked_System_Battle(void*, void*);
	void OnLClicked_System_Battle(void*, void*);

	void OnClicked_SysSetting_SysSet(void*, void*);

	void OnClicked_SysSetting_Restart(void*, void*);

	void OnClicked_SysSetting_Exit(void*, void*);

	void OnClicked_SysSetting_GM(void*, void*);

	void OnClicked_SysSetting_Help(void*, void*);

	void OnClicked_SysSetting_Close(void*, void*);

	void OnClicked_ExitConfirm_Confirm(void*, void*);

	void OnClicked_ExitConfirm_Cancel(void*, void*);

	void OnItemClicked_HtmlHelpView(RtwWidget* sender, void*);

	void OnHtmlAction_HtmlHelpView(RtwWidget* sender, void*);

	//tim.yang
	void OnHtmlTask_HtmlHelpView(RtwWidget* sender, void*);
	void ClientMoveto(std::string szHref);
	//end

	void OnClicked_PlayerItems(void*, void*);

	void OnClicked_System_Skill(void*, void*);

	void OnClicked_HeroList(void*, void*);

	void OnClicked_ShopCenter(RtwWidget* sender, RtwEventDelegate* e);	//heten
	void OnClicked_AutoFight(RtwWidget* sender, RtwEventDelegate* e);

	//void OnClicked_RemindIcon(RtwWidget* sender, void*);

	void OnClick_FbTask(void*, void*);

	void OnClicked_PkMode(RtwWidget* sender, void*);
	void OnClicked_TeamApply(RtwWidget* sender, void*);
	void OnClicked_TeamOff(RtwWidget* sender, void*);
	void OnActivateFrmPkMode(RtwWidget* vpSender, RtwEventDelegate* e);

	//void OnClicked_Attribute_char(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_Attribute_union(RtwWidget* sender, RtwEventDelegate* e);
	//void OnClicked_Attribute_soc(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_Attribute_task(RtwWidget* sender, RtwEventDelegate* e);

	void OnMenuUser(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuUserByName(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuMonster(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuPlayer(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuPet(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuNpc(RtwWidget* sender, RtwEventDelegate* e);
	void OnMenuSystem(RtwWidget* sender, RtwEventDelegate* e);

	void OnFunAction(RtwWidget* sender, RtwEventDelegate* e);
	//void OnScroll_FocusWidget(RtwWidget* sender, void*);//max
	//void OnClick_Rfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	//void OnClick_Lfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	//void OnClick_Movefunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnClicked_AutoBattle(RtwWidget* sender, RtwEventDelegate* e);			//ac.ma
	void OnClicked_AutoPick(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoEatRed(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoEatBlue(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoHp30(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoHp60(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoHp90(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoSellWhite(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoSellGreen(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoSellBlue(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AutoSellGold(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AcceptTask(RtwWidget* sender, RtwEventDelegate* e);       //��������ť��Ӧ�¼�
	void OnClicked_CancelTask(RtwWidget* sender, RtwEventDelegate* e);       //ȡ������ť��Ӧ�¼�
	void OnClicked_FinishTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelFinishTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AcceptMainTask(RtwWidget* sender, RtwEventDelegate* e);       //������������ť��Ӧ�¼�
	void OnClicked_CancelMainTask(RtwWidget* sender, RtwEventDelegate* e);       //ȡ����������ť��Ӧ�¼�
	void OnClicked_FinishMainTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelFinishMainTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnHideHelp(RtwWidget* sender, RtwEventDelegate* e);                     //һ������Ĺرհ�ť
	void OnHideHelp2(RtwWidget* sender, RtwEventDelegate* e);
	void OnHideHelp3(RtwWidget* sender, RtwEventDelegate* e);
	//�����������б�
	void BuildMainTaskList();
	void OnClicked_ShowScript1(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelScript1(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_ShowScript2(RtwWidget* sender, RtwEventDelegate* e);

	//����������ʾ
	void HideGuideAllForm(void*, void*);

	void OnBtnAutoStateYes(void*, void*);
	void OnBtnAutoStateNo(void*, void*);

	//void SwitchHotKeyGroup();

	bool OnMemberChoice(DWORD vDBID, const char* vpName);

	bool OnSaveSkillShortcut(RtwButton* button, string& str);
	bool OnSaveSkillShortcut(RtwButton* button, int& iSkillID);
	//void OnMouseWheel(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	RtwButton* OnLoadSkillShortcut(const char* str);

	bool OnSaveItemShortcut(RtwButton* button, string& str);
	bool OnSaveItemShortcut(RtwButton* button, int& iItemType, int& iItemID);

	RtwButton* OnLoadItemShortcut(const char* str);

	void ClearShortcut();
	bool Createheadimage(GcPlayer* m_pPlayer);
	void LoadShortcut(CG_CmdPacket* pPacket = NULL);  //change by yz: ��Ӵӷ���˶�ȡ���������

	//add by yz: �ֽ��ַ���
	static void parseString(string& rStr, string& rSub)
	{
		size_t pos = rStr.find(';');
		if (pos != rStr.npos)
		{
			rSub = rStr.substr(0, pos);
			rStr = rStr.substr(pos + 1);
		}
	}
	//end

	void SaveShortcut(string& shortcut);
	void SaveShortcut();
	void SaveShortcutToNet(CG_CmdPacket* pPacket);  //add by yz : �ڷ���˱�����������

	void AddRemindIcon(UIRemindType type, const char* hint = "", const char* icon = "", const char* sound = "");

	//void RefreshRemindIcons();

	void UpdateBuyCardList(CG_CmdPacket* pPacket);
	void OnBuyCardResult(CG_CmdPacket* pPacket);
	/* gao 2009.12.22
	gcworld��û��UI_form_skill��ͷ�ļ��������ڴ���Ӵ˺���
	*/
	void InitWidgetLabel(int Metier);
	void OnClicked_TeamButton(RtwWidget* vpSender, RtwEventDelegate* e);
	void SetMiddleMapMouseState(bool bIsDown);
	void OnShowHelp(RtwWidget* vpSender, RtwEventDelegate* e);
	bool OnCloseWidget();
	void OnDragEndAliasButton(RtwWidget* vpSender, RtwEventDelegate* e);
	void OnMouseMoveOnSkillShotcut();
	void OnHotKeyCheck(int iButton, bool vAlt, bool vCtrl, bool vShift);
	void OnResHotKeyState(int iButton);
	void OnSetDefaultHotKeyState();
	void OnSetHotKeyStateFree();
	void OnButtonPetDown(RtwWidget* vpSender, RtwEventDelegate* e);
	void OnSwitchWeapon(void*, void*);

	// ���ڵĶ�������
	void DoFrame();
	void UpDateUIBlood();

	// ���Ը������������ڵĲ���
	void OnSetNewWidget(RtwWidget* pWidget);
	void OnDeleteWidget(RtwWidget* pWidget);
	RtwWidget* GetFirstWidget();
	RtwWidget* GetFocusParentWidget(RtwWidget* son);

	//added by ldr123 at 2010 04 16
	//������Ϣ��������ʵ��(������)
	//begin
// 	bool ItemHyperLinkSend(long itemId);
	//end
	/* end */

public:
	UILayerMain();

	~UILayerMain();

public:
	//RtwChatInputBox*	m_chatInput;
	//RtwWidget*		m_char;
	//RtwChatBox*		m_chatFeedback;
	//RtwChatBox*		m_worldchatFeedback; 
	//RtwChatBox*		m_systemFeedBack;
	//RtwVScrollBar*	m_chatScroll;
	//RtwButton*		m_hotKeyLeft;
	//RtwButton*		m_hotKeyRight;
	//RtwButton::Ptr	m_hotKeyGroup[UI_HOT_KEY_COUNT];
	RtwAliasButton* m_hotKeyAlias[UI_HOT_KEY_COUNT];
	DWORD			m_hotKeySet[UI_HOT_KEY_COUNT];
	bool			m_bHotKeyState[UI_HOT_KEY_COUNT];
	int				m_hotKeyGroupNumber;
	RtwAliasButton* m_hotKeyButton;
	bool			m_bCanUseSwitchWeapon;
	//RtwWidget*		m_hotKeyPage;
	//RtwPopupMenu*   m_popupMenu;
	RtwWidget* m_formHelp;
	RtwButton* m_btnClose;
	RtwWidget* m_formHelp2;        //����������Ķ�������
	RtwButton* m_btnClose2;
	RtwWidget* m_formHelp3;        //���������Ķ�������
	RtwButton* m_btnClose3;
	RtwHtmlView* m_viewHelp;
	RtwHtmlView* m_viewHelp2;
	RtwHtmlView* m_viewHelp3;
	RtwButton* m_btnAccept;        //��������
	RtwButton* m_btnCancel;        //ȡ������
	RtwButton* m_btnFinished;
	RtwButton* m_btnCancelFinished;
	RtwWidget* m_formMainLineTask;		//��������
	RtwTree* m_taskTree;
	RtwHtmlView* m_viewDesc;
	RtwWidget* m_formBtn1;
	RtwWidget* m_formBtn2;
	RtwButton* m_btnMainAccept;
	RtwButton* m_btnMainCancel;
	RtwButton* m_btnMainFinished;
	RtwButton* m_btnMainCancelFinished;
	RtwWidget* m_formGuide1;       //����ָ��
	RtwHtmlView* m_viewGuide1;
	RtwButton* m_btnGuideClose1;
	RtwWidget* m_formGuide2;
	RtwHtmlView* m_viewGuide2;
	RtwWidget* m_formGuide3;
	RtwHtmlView* m_viewGuide3;
	RtwWidget* m_formGuide4;
	RtwHtmlView* m_viewGuide4;
	RtwWidget* m_formGuide5;
	RtwHtmlView* m_viewGuide5;
	RtwWidget* m_formMessageBox1;  //�ű�ͨ�ý���
	RtwHtmlView* m_viewMessage1;
	RtwButton* m_btnMessageOK1;
	RtwButton* m_btnMessageCancel1;
	RtwWidget* m_formMessageBox2;
	RtwHtmlView* m_viewMessage2;
	RtwButton* m_btnMessageOK2;

	RtwLabel* m_LableTimer;       //����---��ʾ�����ʱ
	void            ShowActivityTime(float fSecond);
	void            ResetActivityTime();
	bool            bStartActivity;
	std::string     strActivityText;
	long            lActivityTime;
	RtwWidget* m_formSceneInfo;    //����---��ʾ������Ϣ
	RtwLabel* m_LableSceneInfo;
	RtwWidget* m_formSceneInfo1;
	RtwLabel* m_LableSceneInfo11;
	RtwLabel* m_LableSceneInfo12;
	RtwWidget* m_formSceneInfo2;
	RtwLabel* m_LableSceneInfo21;
	RtwLabel* m_LableSceneInfo22;
	RtwLabel* m_LableSceneInfo23;
	void            ShowSceneInfo(int num, int idx, std::string& info);
	std::string     strCommand;
	void            SendScheduleMsg();

	RtwWidget* m_playerForm;
	RtwWidget* m_playerLevel;
	RtwWidget* m_playerName;
	RtwWidget* m_playerHp;
	RtwProgressBar* m_playerHpBar;
	RtwWidget* m_playerMp;
	RtwProgressBar* m_playerMpBar;
	RtwWidget* m_playerExp;
	RtwProgressBar* m_playerExpBar;

	RtwWidget* m_petForm;
	RtwWidget* m_petLevel;
	RtwWidget* m_petName;
	RtwWidget* m_petHp;
	RtwProgressBar* m_petHpBar;
	RtwWidget* m_petMp;
	RtwProgressBar* m_petMpBar;
	RtwWidget* m_petExp;
	RtwProgressBar* m_petExpBar;

	RtwWidget* m_fmRage;
	//RtwButton*		m_btnRage; // gao 2010.1.5 ���õ�ʱ
	RtwProgressBar* m_proRage;
	RtwWidget* m_fmsysbattle;

	//���촰�����ӵ���������
	RtwWidget* m_fmhyperLinkInChat;
	RtwHtmlView* m_htmlviewLink;

	//��������ʱ
	//begin
	DWORD			m_nCountDownMillisecond;
	RtwWidget* m_fmCountDown;				//����ʱ
	RtwLabel* m_lblTitle;					//��������ʱtitle
	RtwLabel* m_lblCountDownConditionText;//��ȡ��������
	RtwLabel* m_lblCountDown;				//����ʱ
	RtwLabel* m_lblCountDownText;			//�ɱ�ɫ�ĵ���ʱ����

	bool			m_bCountDownStart;

	void CountDownTimer(DWORD dwMillisecond);
	void ShowCountDownForm();
	void SetCountDownFormVisable(bool bVisable = false);
	//end
	//2024.7.23  
	/*typedef struct strWidthLine
	{
		strWidthLine() :m_strMsg(""), m_nLine(0) {}

		string m_strMsg;
		int    m_nLine;
	};*/
	typedef struct strWidthLine
	{
		strWidthLine() :m_strMsg(""), m_nLine(0) {}

		string m_strMsg;
		int    m_nLine;
	}strWidthLine;
	map<string, strWidthLine> m_mapHtmlContainer;
	void			ShowHyerLinkFromChat(const char* pPacket);
	//end
	RtwChatBox::SItem* itemtext;
	std::string strtext;
	CUiCheckButton* m_btnAutoBattle;		//for  auto    ac.ma
	CUiCheckButton* m_btnAutoB;
	CUiCheckButton* m_btnAutoPick;
	CUiCheckButton* m_btnAutoEatRed;
	CUiCheckButton* m_btnAutoEatBlue;
	CUiCheckButton* m_btnAutoHp30;
	CUiCheckButton* m_btnAutoHp60;
	CUiCheckButton* m_btnAutoHp90;
	CUiCheckButton* m_btnAutoSellWhite;
	CUiCheckButton* m_btnAutoSellGreen;
	CUiCheckButton* m_btnAutoSellBlue;
	CUiCheckButton* m_btnAutoSellGold;

	RtwButton* m_btnAutoStateYes;				//ac.ma
	RtwButton* m_btnAutoStateNo;

	RtwWidget* m_formSysSetting;

	RtwButton* m_btnFbTask;
	struct SRemindIcon
	{
		UIRemindType type;
		string icon;
		string hint;
		string sound;
	};
	std::list<SRemindIcon>		m_lstRemindIcon;

	//RtwButton*		m_btnRemind[UI_BTN_REMIND_COUNT];
	DWORD           m_dwInteractiveNPCID;
	DWORD           m_upTaskId;
	// setting ��� ��������
	SETTING_ALL		m_eSetting;

public:
	UIFormCharProp* m_formCharProp;
	UIFormPetProp* m_formPetProp;//tim.yang   ����ϵͳ
	UIFormPetSimple* m_formPetSimple;
	UIFormRebirth* m_formRebirth;
	UIFormCharSimple* m_formCharSimple;
	UIFormTask* m_formTask;                              //---����ϵͳ
	UIFormMainTask* m_formMainTask;                          //---��������
	UIFormMagicWeaponAddMp* m_formMwAddMp;
	UIFormMagicWeaponAddEle* m_formMwAddEle;
	CUIForm_Trump* m_formTrump;
	UIFormRecoverPet* m_formRecoverPet;

	UIFormClearSkill* m_formClearSkill;       // ϴ���ܵ�
	UIFormHelp* m_formHelpSystem;
	UIFormSetting* m_formSetting;          // ����
	UIFormPlayerItems* m_fromPlayerItems;		// ������
	UIFormSkill* m_fromSkill;            // ����
	UIFormSkillStudy* m_fromSkillStudy;       // ѧϰ����
	UIFormMiniMap* m_fromMiniMap;          // С��ͼ
	UIFormTrade* m_formTrade;		    // ����ҽ���
	UIFormShop* m_formShop;			    // ϵͳ����
	UIFormBank* m_formBank;             // Ǯׯ
	UIFormUnion* m_formUnion;            // ����
	UIFormUnionCreate* m_formUnionCreate;      // �������
	UIFormItemUpdate* m_formItemUpdate;		// ����
	UIFormFeodBank* m_formFeodBank;			// ���ع���
	UIFormFeodTax* m_formFeodTax;			// ����˰��
	UIFormFeodContent* m_formFeodContent;		// ������֧��
	UIFormHeroList* m_formHeroList;			// ���а�
	UIFormGm* m_formGm;				// GM
	UIFormPkMode* m_formPkMode;			// PKģʽѡ��
	//-----------------add start by tony 05.06.06--------------------------------//
	UIFormCastleWar* m_formCastleWarReg;		//��սע��/�鿴���
	UIFormCastleWarTimeSet* m_formCastleWarTimeSet; //�޸�ʱ��
	UIFormTimer* m_formTimer;
	UIFormDupe* m_formDupe;
	//-----------------add end   by tony 05.06.06--------------------------------//
	UIFormItemsInfo* m_formItemsInfo;		// �쿴���װ��
	UIFormCreInfo* m_formCreInfo;			// boss��ͷ��Ѫ��
	/**********************gao 2009.11.05*************************/
	UIFormMonInfo* m_formMonInfo;			// �����Ѫ��
	CUI_Form_Chant* m_formChant;			// ����UI
	CUIForm_System* m_formHotKey;			// ���˵�
	CUIForm_Speaker* m_formSpeaker;			// �����������
	CUIForm_Char* m_formChar;				// �������Խ���
	CUIForm_MiddleMap* m_formMiddleMap;		// �е�ͼ
	CUIForm_Gem* m_formGem;				// ��ʯ�ϳ�
	CTimer_Hint* m_TimerHint;
	CUIFormPlayerShop* m_formPlayerShop;
	CUIFormLookPlayerShop* m_formLookPlayerShop;
	bool				m_bNeedSetTranForShotcut;
	/*****************************************************************/
	UIFormSelectReward* m_formSelectReward;		// ������ѡ��
	UIFormCard* m_formCard;
	UIFormSellCard* m_formSellCard;
	UIFormBuyCard* m_formBuyCard;
	UIFormAnalyseImage* m_formAnalyseImage;		// �����ʹ�õ�ͼƬʶ��
	UIFormBankPass* m_formBankPass;
	UIFormItemCompose* m_formItemCompose;		// ���ߺϳɣ���������ҩ������
	UIFormTimeBar* m_formTimeBar;			// ����ʹ�õĽ�����������wow�еģ�
	UIFormVendor* m_formVendor;
	UIFormBid* m_formBid;
	UIFormSale* m_formSale;
	UIFormItemFuse* m_formItemFuse;
	UIFormWarfield* m_formWarfield;
	UIFormPetEat* m_formPetEat;
	CUI_form_Team* m_formTeam;
	CUIForm_Pickup* m_formPickItem;
	CUIForm_Collect* m_formCollectBar;
	UIFormItemDecompound* m_formItemDecompound;
	//��������ָ��
	CUIForm_NewPeopleHelp* m_pNewPeopleHelp;

	UIFromPetMain* m_formPetMain;
	UIFromPetAptitude* m_formPetAptitude;
	UIFromPetCtrl* m_formPetCtrl;
	UIFormPetAbility* m_formPetAbility;
	UIFormPetMerge* m_formPetMerge;
	UIFormPetToBeads* m_formPetToBeads;
	UIFormPetMergeBeads* m_formPetMergeBeads;
	UIFormBankPet* m_formPetBank;
	UIFormShopCenter* m_formShopCenter;
public:
	ULONG m_shopNpcoid;
	vector<int>     taskChapterId;
	vector<MainStoryData> taskMainLine;
	long  mSelectRewardId;
	vector<RtwWidget*>	m_vecWidget;					// �����Ľ��㴰�ڣ�����ֻ���Ը����������Ĵ��ڲŻᱣ��������
};

#endif // _INC_UI_LAYER_MAIN_H_

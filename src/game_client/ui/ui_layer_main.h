#ifndef _INC_UI_LAYER_MAIN_H_
#define _INC_UI_LAYER_MAIN_H_
// 系统设置界面
class UIFormSetting;
// 玩家物品/背包界面
class UIFormPlayerItems;
// 角色属性界面
class UIFormCharProp;
// 宠物属性界面
class UIFormPetProp; //tim.yang 神兽系统
// 宠物简要信息界面
class UIFormPetSimple;
// 角色简要信息界面
class UIFormCharSimple;
// 任务系统界面
class UIFormTask;
// 主线任务界面
class UIFormMainTask;
// 交易界面
class UIFormTrade;
// NPC商店界面
class UIFormShop;
// 玩家商店界面
class UIFormPShop;
// 查看玩家商店界面
class UIFormLookPShop;
// 银行界面
class UIFormBank;
// 消息提示界面
class UIFormMsg;
// 文本消息界面
class UIFormTextMsg;
// 技能界面
class UIFormSkill;
// 技能学习界面
class UIFormSkillStudy;
// 小地图界面
class UIFormMiniMap;
// 公会界面
class UIFormUnion;
// 帮助界面
class UIFormHelp;
// 创建公会界面
class UIFormUnionCreate;
// 物品升级界面
class UIFormItemUpdate;
// 封地税收界面
class UIFormFeodTax;
// 封地银行界面
class UIFormFeodBank;
// 封地内容界面
class UIFormFeodContent;
// 英雄榜界面
class UIFormHeroList;
// GM工具界面
class UIFormGm;
// 法宝加魔界面
class UIFormMagicWeaponAddMp;
// 法宝加元素界面
class UIFormMagicWeaponAddEle;
// 宠物恢复界面
class UIFormRecoverPet;
// PK模式界面
class UIFormPkMode;
// 清除技能界面
class UIFormClearSkill;
// 转生界面
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
class UIFormMonInfo;    // 怪物血条界面
class CUI_Form_Chant;   // 技能吟唱界面
class CUIForm_System;   // 系统设置界面
class CUIForm_Speaker;  // 喇叭发言界面
class CUIForm_Char;     // 角色属性界面
class CUIForm_MiddleMap;// 中型地图界面
class CUIForm_Gem;      // 宝石界面
class CUIFormPlayerShop;// 玩家商店界面
class CUIFormLookPlayerShop; // 浏览玩家商店界面
class CTimer_Hint;      // 提示计时器
/***************************************************************************************/
class UIFormSelectReward;      // 选择奖励界面
class UIFormCard;              // 卡片界面
class UIFormSellCard;          // 出售卡片界面
class UIFormAnalyseImage;      // 图像分析界面
class UIFormBuyCard;           // 购买卡片界面
class UIFormBankPass;          // 银行密码界面
class UIFormItemCompose;       // 物品合成界面
class UIFormTimeBar;           // 时间条界面
class UIFormVendor;            // 摆摊界面
class UIFormBid;               // 竞拍界面
class UIFormSale;              // 出售界面
class UIFormItemFuse;          // 物品融合界面
class UIFormWarfield;          // 战场界面
class UIFormPetEat;            // 宠物喂养界面
class CUI_form_Team;           // 组队界面
class CUIForm_Trump;           // 法宝界面
class CUIForm_Pickup;          // 拾取界面
class CUIForm_Collect;         // 采集界面
class UIFormItemDecompound;    // 物品分解界面
class UIFromPetMain;           // 宠物主界面
class UIFromPetAptitude;       // 宠物资质界面
class UIFromPetCtrl;           // 宠物控制界面
class UIFormPetAbility;        // 宠物能力界面
class UIFormPetMerge;          // 宠物合成界面
class UIFormPetToBeads;        // 宠物化珠界面
class UIFormPetMergeBeads;     // 宠物珠子合成界面
class UIFormBankPet;           // 宠物仓库界面
class UIFormShopCenter;        // 商城中心界面

//新手升级指引
class CUIForm_NewPeopleHelp;   // 新手帮助界面
#define UI_HOT_KEY_COUNT		30  // 快捷键总数量
#define UI_BTN_REMIND_COUNT		3   // 提醒按钮数量
#define UI_REMIND_MAX_SIZE		10  // 提醒最大数量

// 快捷键类型枚举
enum UIHotKeyType
{
    UIHotKey_Icon = 1,  // 图标类型
    UIHotKey_Item = 2,  // 物品类型
    UIHotKey_Skill = 3  // 技能类型
};

// 提醒类型枚举
enum UIRemindType
{
    UIRemind_NewMail = 1,     // 新邮件提醒
    UIRemind_LevelUp = 2,     // 升级提醒
    UIRemind_GmResponse = 3   // GM回复提醒
};

// 界面部件类型枚举
enum HINT_WIDGET
{
    WIDGET_NULL = 0,           // 空部件
    WIDGET_SKILL_TREE = 1,     // 技能树部件
    WIDGET_SKILL_BUFF = 2,     // 技能BUFF部件
    WIDGET_SKILL_SHORTCUT = 3, // 技能快捷键部件
    WIDGET_ITEAM_SHOW = 4      // 物品展示部件
};
struct SETTING_ALL
{
	// 显示设置
	bool		m_bShowSelfName;			// 显示自己名字
	bool		m_bShowBlood;			// 显示血条
	bool		m_bShowDegree;			// 显示等级
	bool		m_bShowDamage;			// 显示伤害
	bool		m_bShowUnionName;		// 显示帮会名称
	bool		m_bShowEXP;				// 显示经验
	bool		m_bShowOtherName;		// 显示其他玩家名字
	bool		m_bShowNPCName;			// 显示NPC名字
	bool		m_bShowOtherSpeak;		// 显示其他人说话
	bool		m_bShowTradeName;		// 显示交易名字
	bool		m_bShowSelfSpeak;		// 显示自己说话

	// 拒绝设置
	bool		m_bRefuseTeam;			// 拒绝组队
	bool		m_bRefuseAction;			// 拒绝动作
	bool		m_bRefuseMail;			// 拒绝邮件
	bool		m_bRefuseDuel;			// 拒绝决斗
	bool		m_bRefuseFriend;			// 拒绝好友
	bool		m_bRefuseMonster;		// 拒绝怪物

	// 技能快捷栏设置
	bool		m_bShowSkillShotcut;		// 是否显示技能快捷栏

	// 音效设置
	bool		m_bBackGroundMusicIsSlience;	// 背景音乐是否静音
	bool		m_bSoundIsSlience;		// 音效是否静音
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
	void OnClicked_AcceptTask(RtwWidget* sender, RtwEventDelegate* e);       //接受任务按钮响应事件
	void OnClicked_CancelTask(RtwWidget* sender, RtwEventDelegate* e);       //取消任务按钮响应事件
	void OnClicked_FinishTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelFinishTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_AcceptMainTask(RtwWidget* sender, RtwEventDelegate* e);       //接受主线任务按钮响应事件
	void OnClicked_CancelMainTask(RtwWidget* sender, RtwEventDelegate* e);       //取消主线任务按钮响应事件
	void OnClicked_FinishMainTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelFinishMainTask(RtwWidget* sender, RtwEventDelegate* e);
	void OnHideHelp(RtwWidget* sender, RtwEventDelegate* e);                     //一级界面的关闭按钮
	void OnHideHelp2(RtwWidget* sender, RtwEventDelegate* e);
	void OnHideHelp3(RtwWidget* sender, RtwEventDelegate* e);
	//主线任务树列表
	void BuildMainTaskList();
	void OnClicked_ShowScript1(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_CancelScript1(RtwWidget* sender, RtwEventDelegate* e);
	void OnClicked_ShowScript2(RtwWidget* sender, RtwEventDelegate* e);

	//隐藏新手提示
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
	void LoadShortcut(CG_CmdPacket* pPacket = NULL);  //change by yz: 添加从服务端读取快捷栏配置

	//add by yz: 分解字符串
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
	void SaveShortcutToNet(CG_CmdPacket* pPacket);  //add by yz : 在服务端保存快捷栏配置

	void AddRemindIcon(UIRemindType type, const char* hint = "", const char* icon = "", const char* sound = "");

	//void RefreshRemindIcons();

	void UpdateBuyCardList(CG_CmdPacket* pPacket);
	void OnBuyCardResult(CG_CmdPacket* pPacket);
	/* gao 2009.12.22
	gcworld中没有UI_form_skill的头文件，所以在此添加此函数
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

	// 窗口的动画播放
	void DoFrame();
	void UpDateUIBlood();

	// 可以跟背包交互窗口的操作
	void OnSetNewWidget(RtwWidget* pWidget);
	void OnDeleteWidget(RtwWidget* pWidget);
	RtwWidget* GetFirstWidget();
	RtwWidget* GetFocusParentWidget(RtwWidget* son);

	//added by ldr123 at 2010 04 16
	//发送消息到服务器实现(超链接)
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
	RtwWidget* m_formHelp2;        //进接受任务的二级界面
	RtwButton* m_btnClose2;
	RtwWidget* m_formHelp3;        //进完成任务的二级界面
	RtwButton* m_btnClose3;
	RtwHtmlView* m_viewHelp;
	RtwHtmlView* m_viewHelp2;
	RtwHtmlView* m_viewHelp3;
	RtwButton* m_btnAccept;        //接受任务
	RtwButton* m_btnCancel;        //取消任务
	RtwButton* m_btnFinished;
	RtwButton* m_btnCancelFinished;
	RtwWidget* m_formMainLineTask;		//主线任务
	RtwTree* m_taskTree;
	RtwHtmlView* m_viewDesc;
	RtwWidget* m_formBtn1;
	RtwWidget* m_formBtn2;
	RtwButton* m_btnMainAccept;
	RtwButton* m_btnMainCancel;
	RtwButton* m_btnMainFinished;
	RtwButton* m_btnMainCancelFinished;
	RtwWidget* m_formGuide1;       //新手指引
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
	RtwWidget* m_formMessageBox1;  //脚本通用界面
	RtwHtmlView* m_viewMessage1;
	RtwButton* m_btnMessageOK1;
	RtwButton* m_btnMessageCancel1;
	RtwWidget* m_formMessageBox2;
	RtwHtmlView* m_viewMessage2;
	RtwButton* m_btnMessageOK2;

	RtwLabel* m_LableTimer;       //活动相关---显示活动倒计时
	void            ShowActivityTime(float fSecond);
	void            ResetActivityTime();
	bool            bStartActivity;
	std::string     strActivityText;
	long            lActivityTime;
	RtwWidget* m_formSceneInfo;    //活动相关---显示场景信息
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
	//RtwButton*		m_btnRage; // gao 2010.1.5 无用当时
	RtwProgressBar* m_proRage;
	RtwWidget* m_fmsysbattle;

	//聊天窗中连接的悬浮窗口
	RtwWidget* m_fmhyperLinkInChat;
	RtwHtmlView* m_htmlviewLink;

	//奖励倒计时
	//begin
	DWORD			m_nCountDownMillisecond;
	RtwWidget* m_fmCountDown;				//倒计时
	RtwLabel* m_lblTitle;					//奖励倒计时title
	RtwLabel* m_lblCountDownConditionText;//领取条件内容
	RtwLabel* m_lblCountDown;				//倒计时
	RtwLabel* m_lblCountDownText;			//可变色的倒计时文字

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
	// setting 相关 交互设置
	SETTING_ALL		m_eSetting;

public:
	UIFormCharProp* m_formCharProp;
	UIFormPetProp* m_formPetProp;//tim.yang   神兽系统
	UIFormPetSimple* m_formPetSimple;
	UIFormRebirth* m_formRebirth;
	UIFormCharSimple* m_formCharSimple;
	UIFormTask* m_formTask;                              //---任务系统
	UIFormMainTask* m_formMainTask;                          //---主线任务
	UIFormMagicWeaponAddMp* m_formMwAddMp;
	UIFormMagicWeaponAddEle* m_formMwAddEle;
	CUIForm_Trump* m_formTrump;
	UIFormRecoverPet* m_formRecoverPet;

	UIFormClearSkill* m_formClearSkill;       // 洗技能点
	UIFormHelp* m_formHelpSystem;
	UIFormSetting* m_formSetting;          // 设置
	UIFormPlayerItems* m_fromPlayerItems;		// 道具栏
	UIFormSkill* m_fromSkill;            // 技能
	UIFormSkillStudy* m_fromSkillStudy;       // 学习技能
	UIFormMiniMap* m_fromMiniMap;          // 小地图
	UIFormTrade* m_formTrade;		    // 与玩家交易
	UIFormShop* m_formShop;			    // 系统买卖
	UIFormBank* m_formBank;             // 钱庄
	UIFormUnion* m_formUnion;            // 帮派
	UIFormUnionCreate* m_formUnionCreate;      // 创建诸侯
	UIFormItemUpdate* m_formItemUpdate;		// 锻造
	UIFormFeodBank* m_formFeodBank;			// 采邑国库
	UIFormFeodTax* m_formFeodTax;			// 采邑税率
	UIFormFeodContent* m_formFeodContent;		// 采邑收支表
	UIFormHeroList* m_formHeroList;			// 排行榜
	UIFormGm* m_formGm;				// GM
	UIFormPkMode* m_formPkMode;			// PK模式选择
	//-----------------add start by tony 05.06.06--------------------------------//
	UIFormCastleWar* m_formCastleWarReg;		//城战注册/查看相关
	UIFormCastleWarTimeSet* m_formCastleWarTimeSet; //修改时间
	UIFormTimer* m_formTimer;
	UIFormDupe* m_formDupe;
	//-----------------add end   by tony 05.06.06--------------------------------//
	UIFormItemsInfo* m_formItemsInfo;		// 察看玩家装备
	UIFormCreInfo* m_formCreInfo;			// boss的头像血条
	/**********************gao 2009.11.05*************************/
	UIFormMonInfo* m_formMonInfo;			// 怪物的血条
	CUI_Form_Chant* m_formChant;			// 吟唱UI
	CUIForm_System* m_formHotKey;			// 主菜单
	CUIForm_Speaker* m_formSpeaker;			// 喇叭输入界面
	CUIForm_Char* m_formChar;				// 人物属性界面
	CUIForm_MiddleMap* m_formMiddleMap;		// 中地图
	CUIForm_Gem* m_formGem;				// 晶石合成
	CTimer_Hint* m_TimerHint;
	CUIFormPlayerShop* m_formPlayerShop;
	CUIFormLookPlayerShop* m_formLookPlayerShop;
	bool				m_bNeedSetTranForShotcut;//是否需要设置快捷键的透明度
	/*****************************************************************/
	UIFormSelectReward* m_formSelectReward;		// 任务奖励选择
	UIFormCard* m_formCard;
	UIFormSellCard* m_formSellCard;
	UIFormBuyCard* m_formBuyCard;
	UIFormAnalyseImage* m_formAnalyseImage;		// 防外挂使用的图片识别
	UIFormBankPass* m_formBankPass;
	UIFormItemCompose* m_formItemCompose;		// 道具合成（铁匠、制药、炼金）
	UIFormTimeBar* m_formTimeBar;			// 公共使用的进度条（类似wow中的）
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
	//新手升级指引
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
	vector<RtwWidget*>	m_vecWidget;					// 曾经的焦点窗口，这里只可以跟背包交互的窗口才会保存在里面
};

#endif // _INC_UI_LAYER_MAIN_H_

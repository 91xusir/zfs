#ifndef _INC_UI_FORM_PLAYERITEMS_H_
#define _INC_UI_FORM_PLAYERITEMS_H_

class GcActor;
#define MAX_LIMIT_ITEM 5

struct OPERATEITEMCD
{
	DWORD				m_dwLimitTime;
	DWORD				m_dwOperateTime;
	int					m_iPage;
	int					m_iI;
	int					m_iJ;
	RtwButton::Ptr		m_pBtnShortcut;
};
class UIFormPlayerItems
{
public:
	UIFormPlayerItems();
	~UIFormPlayerItems();

	RtwImage* LoadSequenceImage(const string &strFileName);

	void Run(unsigned long dwMillisecond);

	GcActor *m_pPlayer;

	void SetPlayer(GcActor* pActor) { m_pPlayer = pActor; }
	void Show();
	void Hide();
	bool IsShowing();										// 是否已经显示
	void Refresh(bool bRefreshRelated = false);			// 刷新界面（是否刷新与道具相关的界面）
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }
	bool IsInMissionPage() { return m_bInMissionPage; }		// 是在普通物品页还是在人物物品页
	void OnGetItemBaseNum(SItemID& item,int& Num,SItemID& itemBase);

	bool Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart);
	bool Is_Widget_Common(const string& id, int &outI, int &outJ);
	bool Is_Widget_Mission(const string& id, int &outI, int &outJ);
	void OnGetHintText_CommonItem(RtwWidget* sender, void*);
	void OnGetHintText_Parts(RtwWidget* sender, void*);

	void OnSetItemMask(int page, int i, int j, bool isEnable = false, float values = 0.0f, DWORD color = 0xb5000000, DWORD Alpha = 0xff, bool EnableAlpha = false);
	void OnDisableAllItemMask();
	void OnDisableItemMask(SItemID* Item);

	int OnGetCorrectPage()
	{
		if(m_iLastTabSelect== -1)
			return GetPlayer()->mItem.m_Bag.GetPageIndex();
		else
			return m_iLastTabSelect;
	}

	SItemID	OnGetUsingPetItem()
	{
		guard;
		if(ItemID_IsValid(m_pet))
			return m_pet;
		else
			return ItemID_CreateInvalid();
		unguard;
	}
	RtwRect OnGetFormRect()
	{
		guard;
		return m_pFrmThis->GetFrameRect();
		unguard;
	}

	//added by ldr123 at 10.04.16
	//begin
	//用于鼠标点击(ctrl键按下)道具时,发送id给服务端,以实现超链接的功能
	void ItemHyperLinkSend(DWORD itemTag, DWORD itemId, DWORD playerID, const std::string &strName, RtwPixel);
	//end
	void OnLClick_QuickStart(RtwWidget* sender, void*);
	void OnGetHintText_QuickStart(RtwWidget* sender, void*);
	void RemoveQuickStart(DWORD itemID);			// 如果某个道具已经不再属于player，而且快捷栏中有此道具，就要把快捷栏中的按钮删掉
	void GetQuickStartButton(DWORD itemID, vector<RtwButton*>& arrBtns);

	void DeleteQuickStartUserData(RtwWidget* sender, void*);

	void RefreshQuickStart();
	bool _RefreshQuickStart(DWORD ItemID, DWORD ItemType);

	bool GetQuickStartItemData(int pageIndex, int gridIndex, DWORD& ItemID, DWORD& ItemType);

	bool m_bLockOperate;
	DWORD m_dwLastLockTick;
	void LockOperate(bool bLock);
	bool OperateIsLocked();


	void OnPickItem(void*, void*);
	void OnEquipItem(void*, void*);
	void PlayTaskItem(int iTime);
	void PlayTaskItemEnd(void*, void*);
	void PlayActivityProgressBar(long lTime);
	void PlayActivityProgressBarEnd(void*, void*);

	void SetTabSelect(int i);

	// for UI MsgBox:
	DWORD m_pickItemOID;
	// 用来保存分离道具时所使用 数据
	byte  m_byteBuyInfo[6];

	DWORD m_equipItemOID;
	CItemContainerBase::EEquipParts m_equipPart;
	int m_equipPage;
	int m_equipGridI;
	int m_equipGridJ;

	SItemID m_pet;
	SItemID	m_LastPetItemID;

    //add by yz : 外部调用
	void UseBagItem(int page, int gridI, int gridJ, const SItemID* pItemID = NULL);
	//end yz

private:
	void OnPreDraw();
    void OnItemUpdate(RtwWidget* sender, void*);
    void DoUseCommonBagItem(int page, int gridI, int gridJ);
	void OnClose(RtwWidget* sender, void*);
	void OnMouseMove(RtwWidget* sender, void*);

	void OnLClick_CommonItem(RtwWidget* sender, void*);
	void OnLDClick_CommonItem(RtwWidget* sender, void*);
	void OnDrag_CommonItem(RtwWidget* sender, void*);
	void OnDragOut_CommonItem(RtwWidget* sender, void*);
	void OnDragEnd_CommonItem(RtwWidget* sender, void*);

	void OnGetHintText_MissionItem(RtwWidget* sender, void*);
	void OnRClick_MissionItem(RtwWidget* sender, void*);
	void OnDrag_MissionItem(RtwWidget* sender, void*);
    void OnDragEnd_MissionItem(RtwWidget* sender, void*);

	void OnDrag_Trash(RtwWidget* sender, void*);
    void OnDrag_Pass(RtwWidget* sender, void*);

    void cbItemUnlock(RtwWidget* sender, RtwEventDelegate* e);

	//void OnLClick_CommonButton(RtwWidget* sender, void*);
	//void OnLClick_MissionButton(RtwWidget* sender, void*);
	void OnMouseRDown_CommonButton(RtwWidget* sender, void*);

	void OnLDClick_Parts(RtwWidget* sender, void*);
	void OnDrag_Parts(RtwWidget* sender, void*);
	void OnDragEnd_Parts(RtwWidget* sender, void*);
	void OnLClick_Parts(RtwWidget *sender, void*);
	void OnEditNumOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);

	//void OnLClick_LeftArrow(void*, void*);
	//void OnLClick_RightArrow(void*, void*);

	void OnDeleteItem(void*, void*);
	void OnThrowItem(void*, void*);
	void OnDecomposeItem(void*, void*);
	void OnEnchantItem(void*, void*);

	void OnTabSelect(RtwWidget* sender, void*);

	void OnLClick_btnShop(RtwWidget* sender, void*);
	void OnLClick_btnSplit(RtwWidget* sender, void*);

	void OnSetCD(int type,SItemID& item);

	void OnThisHide(void*,void*);

	struct MaskItem
	{
		int ItemJ;
		int ItemI;
		int ItemPage;
	};
	RtwWidget			*m_pFrmThis;
	RtwTab				*m_pTabThis;

	//RtwButton *m_btnParts[CItemContainerBase::MAX_EQUIP_PARTS];
	RtwWidget			*m_frmGridsCommon[ITEM_PLAYER_INVENTORY_PAGE];
	RtwButton			*m_btnGridsCommon[ITEM_PLAYER_INVENTORY_PAGE][ITEM_PLAYER_INVENTORY_LINE][ITEM_PLAYER_INVENTORY_COLUMN];
	RtwLabel			*m_lbGridsCommon[ITEM_PLAYER_INVENTORY_PAGE][ITEM_PLAYER_INVENTORY_LINE][ITEM_PLAYER_INVENTORY_COLUMN];
	RtwWidget			*m_frmGridsMission;
	RtwButton			*m_btnGridsMission[ITEM_PLAYER_INVENTORY_LINE][ITEM_PLAYER_INVENTORY_COLUMN];
	RtwLabel			*m_lbGridsMission[ITEM_PLAYER_INVENTORY_LINE][ITEM_PLAYER_INVENTORY_COLUMN];

	RtwWidget			*m_lbMoney;
	//RtwWidget *m_lbPage;

	//RtwButton *m_btnCommonItem;
	//RtwButton *m_btnMissionItem;
	//RtwButton *m_btnPageLeft;
	//RtwButton *m_btnPageRight;
	RtwButton			*m_btnShop;
	RtwButton			*m_btnSplit;
	RtwButton			*m_btnTrash;
	RtwButton			*m_btnPass;
	RtwButton			*m_btnUpdate;
	RtwButton			*m_btnBank;


	RtwImage::Ptr		m_ptrHoverImage;

	//
	bool				m_bInMissionPage;

	// 以下变量在MessageBox的回调函数中使用
	bool				m_bBackupMission;
	int					m_pageBackup, m_iBackup, m_jBackup;
	int					m_IdBackup;
	bool				m_bBackupMission1;
	int					m_pageBackup1, m_iBackup1, m_jBackup1;
	int					m_IdBackup1;

	// 宠物的激活/收回时间
	const DWORD			m_PetLimitTime;
	DWORD				m_dwPetOperateTime;
	int					m_iPetActivePage, m_iPetActiveI, m_iPetActiveJ;
	RtwButton::Ptr		m_pBtnPetShortcut;

	void CallPet(void*, void*);

	//// 红瓶禁止时间
	//DWORD				m_HPLimitTime;
	//DWORD				m_dwHPOperateTime;
	//int					m_iHPPage, m_iHPI, m_iHPJ;
	//RtwButton::Ptr		m_pBtnHPShortcut;

	//// 蓝瓶禁止时间
	//DWORD				m_MPLimitTime;
	//DWORD				m_dwMPOperateTime;
	//int					m_iMPPage, m_iMPI, m_iMPJ;
	//RtwButton::Ptr		m_pBtnMPShortcut;
	// 通用禁止时间
	vector<vector<OPERATEITEMCD*>*>	m_vecOperateItemCD;

	bool				m_bPlayItem;
	bool				m_bIsSplit;

	int					m_iLastTabSelect;
	bool				m_bIsDrag;

	vector<MaskItem>    m_vMaskItem;

public:
	bool                bEnchant;

};

#endif // _INC_UI_FORM_PLAYERITEMS_H_

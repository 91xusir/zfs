#ifndef UI_FORM_SHOPCENTER_H
#define UI_FORM_SHOPCENTER_H

#define MAX_PAGE_COUNT    6
#define MAX_ITEM_COUNT    8

class UIFormShopCenter
{
public:
	UIFormShopCenter();
	virtual ~UIFormShopCenter();

	void ChangeSelectTab(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void ShowFirstPage(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void ShowUpPage(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void ShowDownPage(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void ShowEndPage(RtwWidget* pSender, RtwEventDelegate* pEvent);
	bool Is_Widget_Page(const string& id, int &outI);
	bool Is_Widget_Page_Button(const string& id, int &outI, int &outJ);

	void InitShopCenter();
	void Update();
	bool IsVisible();
	void Show();
	void Hide();

	//�Ƽ�����
	void ShowCommendItemInfo(std::vector<long> vecItem, int iCheckBtn, int iPage);
	void ShowCheckCommendItemAll(void*, void*);
	void ShowCheckCommendItemHot(void*, void*);
	void ShowCheckCommendItemNew(void*, void*);
	void ShowCheckCommendItemPresent(void*, void*);

	void RenderFactionActor();

private:
	RtwForm*        m_pFrmThis;
	RtwTab*         m_pTabPage;
	RtwWidget*      m_pFormItemType[MAX_PAGE_COUNT];
	RtwWidget*      m_pFormItem[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwButton*      m_pBtnPresentItem[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwButton*      m_pBtnBuyItem[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwLabel*       m_pItemIcon[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwLabel*       m_pLableItemName[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwLabel*       m_pLableItemPrice[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	RtwLabel*       m_pLableItemDownPrice[MAX_PAGE_COUNT][MAX_ITEM_COUNT];
	CUiCheckButton* m_ckBtnAll;
	CUiCheckButton* m_ckBtnHot;
	CUiCheckButton* m_ckBtnNew;
	CUiCheckButton* m_ckBtnPresent;
	RtwButton*      m_pFirstBtn[MAX_PAGE_COUNT];
	RtwButton*      m_pUpBtn[MAX_PAGE_COUNT];
	RtwButton*      m_pDownBtn[MAX_PAGE_COUNT];
	RtwButton*      m_pEndBtn[MAX_PAGE_COUNT];
	RtwWidget*      m_pFormCommend;            //---�Ƽ���Ʒ
	RtwWidget*      m_pFormCommendItem1;
	RtwWidget*      m_pBtnPresentItem1;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem1;            //---����ť
	RtwWidget*      m_pItemIcon1;
	RtwWidget*      m_pLableItemName1;
	RtwWidget*      m_pLableItemPrice1;
	RtwWidget*      m_pLableItemDownPrice1;
	RtwWidget*      m_pFormCommendItem2;
	RtwWidget*      m_pBtnPresentItem2;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem2;            //---����ť
	RtwWidget*      m_pItemIcon2;
	RtwWidget*      m_pLableItemName2;
	RtwWidget*      m_pLableItemPrice2;
	RtwWidget*      m_pLableItemDownPrice2;
	RtwWidget*      m_pFormCommendItem3;
	RtwWidget*      m_pBtnPresentItem3;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem3;            //---����ť
	RtwWidget*      m_pItemIcon3;
	RtwWidget*      m_pLableItemName3;
	RtwWidget*      m_pLableItemPrice3;
	RtwWidget*      m_pLableItemDownPrice3;
	RtwWidget*      m_pFormCommendItem4;
	RtwWidget*      m_pBtnPresentItem4;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem4;            //---����ť
	RtwWidget*      m_pItemIcon4;
	RtwWidget*      m_pLableItemName4;
	RtwWidget*      m_pLableItemPrice4;
	RtwWidget*      m_pLableItemDownPrice4;
	RtwWidget*      m_pFormCommendItem5;
	RtwWidget*      m_pBtnPresentItem5;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem5;            //---����ť
	RtwWidget*      m_pItemIcon5;
	RtwWidget*      m_pLableItemName5;
	RtwWidget*      m_pLableItemPrice5;
	RtwWidget*      m_pLableItemDownPrice5;
	RtwWidget*      m_pFormCommendItem6;
	RtwWidget*      m_pBtnPresentItem6;        //---���Ͱ�ť
	RtwWidget*      m_pBtnBuyItem6;            //---����ť
	RtwWidget*      m_pItemIcon6;
	RtwWidget*      m_pLableItemName6;
	RtwWidget*      m_pLableItemPrice6;
	RtwWidget*      m_pLableItemDownPrice6;

	Rtw3DView*      m_shopActorView;

	//�Ƽ���Ʒ����
	std::vector<long> vecCommendItemAll;
	std::vector<long> vecCommendItemHot;
	std::vector<long> vecCommendItemNew;
	std::vector<long> vecCommendItemPresent;
	//���ܵ�������
	std::vector<long> vecFuctionItemAll;
	//������������
	std::vector<long> vecSupplyItemAll;
	//�����������
	std::vector<long> vecMaterialItemAll;
	//������������
	std::vector<long> vecPetItemAll;
	//����ʱװ����
	std::vector<long> vecFashionItemAll;
	int m_curSelectPage[MAX_PAGE_COUNT];
	int m_curCommendPage[4];

	//��Ⱦʱװ���ﲿ��
	RtgCamera* m_pCamActor;
};

#endif //UI_FORM_SHOPCENTER_H
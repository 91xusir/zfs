#ifndef _INC_UI_FORM_COMPOSE_H_
#define _INC_UI_FORM_COMPOSE_H_

struct SUICompose
{
    short ID;
    short Level;
    std::string Name;
};

#define UI_ITEMCOMPOSE_MATERIAL_COUNT			5
class UIFormItemCompose
{
public:
	UIFormItemCompose();
	~UIFormItemCompose();

	void Show(EItemComposeType ComposeType);
	void Hide();
	bool IsVisible();
	void Refresh(bool bComposeList = true, bool bComposingDetail = true);
	void RefreshPlayerComposeList();

	void OnComposeTimeOver(RtwWidget* sender, void*);
	void OnRecvComposeBegin(short ComposeID, short ErrorCode);
	void OnRecvComposeEnd(short ComposeID, short ErrorCode);
	void ComposeBegin(short ComposeID);
	void ComposeEndSuccess(short ComposeID);
	void ComposeFail(short ComposeID);

	DWORD GetSelectComposeID() { return m_SelectComposeID; }
	EItemComposeType GetComposeType() { return m_ComposeType; }

private:
	RtwWidget *m_pFrmThis;
	RtwListBox* m_pComposeList;
	RtwWidget *m_pMaterialTitle;
	RtwButton *m_pProductIcon;
	RtwWidget *m_pProductInfo;
	RtwButton *m_pMaterialIcon[UI_ITEMCOMPOSE_MATERIAL_COUNT];
	RtwWidget *m_pMaterialInfo[UI_ITEMCOMPOSE_MATERIAL_COUNT];
	RtwImage  *m_pBackImage[ICT_Count];
	RtwButton *m_pMake;
	RtwButton *m_pMaxCont;
	RtwTextBox *m_pCount;
	RtwButton *m_pLeftArrow;
	RtwButton *m_pRightArrow;
	RtwScrollBar *m_pMaterialScroll;
	RtwWidget *m_pFrmImage;
	RtwProgressBar *m_pProgressBar;
	RtwLabel *m_pNeedMoney;
	RtwImage::Ptr		m_ptrHoverImage;

private:
	void OnPreDraw(bool bComposeList = true, bool bComposingDetail = true);
	void OnClicked_Make(RtwWidget* sender, void*);
	void OnClicked_MaxCont(RtwWidget* sender, void*);
	void OnClicked_LeftArrow(RtwWidget* sender, void*);
	void OnClicked_RightArrow(RtwWidget* sender, void*);
	void OnSelected_Composing(RtwWidget* sender, void*);
	void OnScrollMaterials(RtwWidget* sender,void*);
	void OnHide(RtwWidget* sender,void*);
	void OnGetHintText(RtwWidget* sender, void*);
	void OnTextBoxChange(RtwWidget* sender, void*);

	EItemComposeType m_ComposeType;
	DWORD m_SelectComposeID;
	int m_CanMakeCount;					// 可以制作的产品数量
	int m_MakeCount;					// 填入的制作产品数量

	std::vector<SUICompose> m_Composes;

	bool m_bMaking;
	bool m_bRecvComposeEndSuccess;
	bool m_bComposeTimeOver;

	bool m_bMaterialUseScroll;
	int m_ScrollLine;
};

#endif // _INC_UI_FORM_COMPOSE_H_

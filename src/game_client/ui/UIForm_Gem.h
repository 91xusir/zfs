#ifndef _INC_UI_FORM_GEM_H_
#define _INC_UI_FORM_GEM_H_
//#include "ui/rtw_ui.h"
//struct SItemID;
class CUIForm_Gem
{
public:

	CUIForm_Gem(void);
	~CUIForm_Gem(void);

	void Show();
	void Hide();
	void Refresh();
	void OnCommonItemDeleted(DWORD itemID);
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }
	bool IsVisible();

	bool OnBagToGem1(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToGem2(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToGem3(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToGem4(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToGem5(RtwWidget* sender, RtwEventDelegate* e);
	RtwRect OnGetFormRect()
	{
		guard;
		return m_pFrmThis->GetFrameRect();
		unguard;
	}

	RtwWidget* OnGetThisForm(){return m_pFrmThis;}
private:

	void OnBtnOKDown(RtwWidget* sender, void*);
	void OnDragToGem1(RtwWidget* sender, void*);
	void OnDragToGem2(RtwWidget* sender, void*);
	void OnDragToGem3(RtwWidget* sender, void*);
	void OnDragToGem4(RtwWidget* sender, void*);
	void OnDragToGem5(RtwWidget* sender, void*);
	void OnDragEndGem1(RtwWidget* sender, void*);
	void OnDragEndGem2(RtwWidget* sender, void*);
	void OnDragEndGem3(RtwWidget* sender, void*);
	void OnDragEndGem4(RtwWidget* sender, void*);
	void OnDragEndGem5(RtwWidget* sender, void*);
	void OnDragEndGem(RtwWidget* sender, void*);
	void OnMouseRDown(RtwWidget* sender, void*);
	void OnGetHintText(RtwWidget* sender, void*);
	void OnPreDraw();

	bool SeekGemPos1();
	bool SeekGemPos2();
	bool SeekGemPos3();
	bool SeekGemPos4();
	bool SeekGemPos5();

	void OnGetFocus(void*,void*){g_layerMain->OnSetNewWidget(m_pFrmThis);}
	void OnHide(void*,void*){Hide();g_layerMain->OnDeleteWidget(m_pFrmThis);}
private:

	RtwWidget*				m_pFrmThis;
	RtwWidget*				m_pBtnItem[5];
	RtwButton*				m_btnOK;
	RtwLabel*				m_plbMoney;
	RtwWidget*				m_pLbRate;
	RtwImage::Ptr			m_ptrHoverImage;

	SItemID                 m_gemItem1;
	SItemID                 m_gemItem2;
	SItemID                 m_gemItem3;
	SItemID                 m_gemItem4;
	SItemID                 m_gemItem5;
	SItemID					m_thisItem;		//记录当前放入并且已经存在的物品，用来控制提示次数

	int m_pageGem1, m_iGem1, m_jGem1;
	int m_pageGem2, m_iGem2, m_jGem2;
	int m_pageGem3, m_iGem3, m_jGem3;
	int m_pageGem4, m_iGem4, m_jGem4;
	int m_pageGem5, m_iGem5, m_jGem5;
	int moneyRequired;

};
#endif // _INC_UI_FORM_GEM_H_

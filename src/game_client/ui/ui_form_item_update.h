#ifndef _INC_UI_FORM_ITEM_UPDATE_H_
#define _INC_UI_FORM_ITEM_UPDATE_H_


#define ITEMUPDATE_UI_MAX_REQ			5	

class UIFormItemUpdate
{
public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }

	void OnCommonItemDeleted(DWORD itemID);
	bool OnBagToItem(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToScroll(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToLuckStone(RtwWidget* sender, RtwEventDelegate* e);
	RtwRect OnGetFormRect()
	{
		guard;
		return m_pFrmThis->GetFrameRect();
		unguard;
	}
	RtwWidget* OnGetThisForm(){return m_pFrmThis;}

private:
	RtwWidget *m_pFrmThis;		// 整个窗口

	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;

	RtwWidget *m_pBtnItem;
	RtwWidget *m_pBtnScroll;
	RtwWidget *m_pBtnLuckStone;
	RtwImage::Ptr		m_ptrHoverImage;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);

	void OnDragToItem(RtwWidget* sender, void*);
	void OnDragToScroll(RtwWidget* sender, void*);
	void OnDragToLuckStone(RtwWidget* sender, void*);
	void OnDragEndItem(RtwWidget* sender, void*);
	void OnDragEndScroll(RtwWidget* sender, void*);
	void OnDragEndLuckStone(RtwWidget* sender, void*);
	void OnMouseRDown(RtwWidget* sender, void*);
	void OnGetHintText(RtwWidget* sender, void*);

	SItemID m_UpdateItemID;
	int m_pageItem, m_iItem, m_jItem;
	int m_pageScroll, m_iScroll, m_jScroll;
	int m_pageLuckStone, m_iLuckStone, m_jLuckStone;
	SItemID m_ScrollID;
	SItemID m_LuckStoneID;

	// 把图标从道具兰拖过来，但是道具或卷轴的位置有改变时，更新位置
	bool SeekUpdateItemPos();
	bool SeekScrollPos();
	bool SeekLuckStonePos();

	void OnGetFocus(void*,void*);
	void OnHide(void*,void*);

    RtwWidget* m_ReqMoney;
private:
	void OnPreDraw();

public:
	UIFormItemUpdate();
	~UIFormItemUpdate();
};

#endif // _INC_UI_FORM_ITEM_UPDATE_H_

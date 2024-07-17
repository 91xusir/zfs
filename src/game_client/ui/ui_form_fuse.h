#ifndef _INC_UI_FORM_FUSE_H_
#define _INC_UI_FORM_FUSE_H_

class UIFormItemFuse
{
public:
	UIFormItemFuse();
	~UIFormItemFuse();

	void Show();
	void Hide();
	bool IsVisible();										// 是否已经显示
	void Refresh();
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }

	void OnCommonItemDeleted(DWORD itemID);
	bool OnBagToFuseItem1(RtwWidget* sender, RtwEventDelegate* e);
	bool OnBagToFuseItem2(RtwWidget* sender, RtwEventDelegate* e);

	RtwWidget* OnGetThisForm(){return m_pFrmThis;}

private:
	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pBtnItem1;
	RtwWidget *m_pBtnItem2;
	RtwWidget *m_pMoney;
	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;
	RtwImage::Ptr		m_ptrHoverImage;

	void OnClicked_OK(RtwWidget* sender, void*);
	void OnClicked_Cancel(RtwWidget* sender, void*);
	void OnDragToItem1(RtwWidget* sender, void*);
	void OnDragToItem2(RtwWidget* sender, void*);
	void OnDragEndItem1(RtwWidget* sender, void*);
	void OnDragEndItem2(RtwWidget* sender, void*);
	void OnMouseRDown(RtwWidget* sender, void*);
	void OnGetHintText(RtwWidget* sender, void*);
	void DoFuse(RtwWidget* sender, void*);

	// 把图标从道具兰拖过来，但是道具或卷轴的位置有改变时，更新位置
	bool SeekItem1();
	bool SeekItem2();

	void OnGetFocus(void*,void*){g_layerMain->OnSetNewWidget(m_pFrmThis);}
	void OnHide(void*,void*){g_layerMain->OnDeleteWidget(m_pFrmThis);}

	SItemID m_Item1;
	int m_pageItem1, m_iItem1, m_jItem1;
	int m_pageItem2, m_iItem2, m_jItem2;
	SItemID m_Item2;

private:
	void OnPreDraw();
};

#endif // _INC_UI_FORM_FUSE_H_

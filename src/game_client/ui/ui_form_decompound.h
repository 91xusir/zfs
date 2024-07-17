#ifndef _INC_UI_FORM_Decompound_H_
#define _INC_UI_FORM_Decompound_H_
#include "ui/rtw_ui.h"

class UIFormItemDecompound
{
private:
	RtwWidget *m_pFrmThis;
	RtwButton *m_pDecompoundIcon;
	RtwButton *m_pStartDecompound;
	RtwButton *m_pClose;
	RtwImage::Ptr		m_ptrHoverImage;

	SItemID m_ItemID;
	int				m_iISendItem;
	int				m_iJSendItem;
	int				m_iPage;

	void OnDecomposeItem(void*, void*);
	void RealDecomposeItem(void*, void*);

	void OnGetFocus(void*,void*){g_layerMain->OnSetNewWidget(m_pFrmThis);}
	void OnHide(void*,void*){g_layerMain->OnDeleteWidget(m_pFrmThis);}
public:
	UIFormItemDecompound(void);
	~UIFormItemDecompound(void);

	void Show();
	void Hide();
	bool IsVisible();
	void SetItemID( SItemID itemid );
	bool OnBagToDecompound(RtwWidget* sender, RtwEventDelegate* e);
	void OnDragEndDecompound(RtwWidget* sender, void*);
	void OnMouseRDown(RtwWidget* sender, RtwEventDelegate* e);
	void OnGetHintItemText(RtwWidget* sender, void*);

	void StartDecompound(RtwWidget* sender, void*);
	void Close(RtwWidget* sender, void*);

	RtwWidget* OnGetThisForm(){return m_pFrmThis;}
};

#endif
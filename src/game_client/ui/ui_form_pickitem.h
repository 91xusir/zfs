#ifndef _INC_UI_FORM_PICKITEM_H_
#define _INC_UI_FORM_PICKITEM_H_

class UIFormPickItem
{
public:
	UIFormPickItem();
	~UIFormPickItem();

	void Show();
	void Hide();
	bool IsVisible();
	void Refresh();
protected:

private:
	RtwWidget *m_pFrmThis;		// ��������
	RtwButton *m_pBtnItem[20];

	void OnClicked_ItemIcon(RtwWidget* sender, void*);
};
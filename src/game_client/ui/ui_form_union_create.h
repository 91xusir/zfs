//********************************************************************
//	created:	2010.05.31
//	filename: 	ui_form_union_create.h
//	author:		ldr123
//	purpose:	���ɴ�����
//  other:		
//********************************************************************
#ifndef _INC_UI_FORM_UNION_CREATE_H_
#define _INC_UI_FORM_UNION_CREATE_H_

class UIFormUnionCreate
{
public:
	void Refresh();
	void Show(DWORD npcID);
	void Hide();
	bool IsVisible();
private:
	//�����
	//ȷ����ť
	//ȡ��
	//npc id

	RtwWidget *m_pFrmThis;		// ��������

	RtwTextBox*	m_pUnionName;
	RtwButton*	m_pBtnOK;
	RtwButton*	m_pBtnCancel;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);

private:
	void OnPreDraw();
	DWORD m_npcID;

public:
	UIFormUnionCreate();
	~UIFormUnionCreate();
};

#endif // _INC_UI_FORM_UNION_CREATE_H_

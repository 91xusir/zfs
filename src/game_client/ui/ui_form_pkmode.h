#ifndef _INC_UI_FORM_PKMODE_H_
#define _INC_UI_FORM_PKMODE_H_

class UIFormPkMode
{
public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();

	bool CanInteract(GcActor* pActor); // ��û��ǿ�Ƽ�ʱ�ɲ��������ɫ����
	void OnChangePkModeImage(EPkMode mode);

private:
	RtwWidget*		m_pfrmThis;
	RtwListBox*		m_plstPKmode;
	DWORD			m_dwSwitchFreeTime;
private:
	void OnPreDraw();
	void OnChangePkMode(RtwWidget* sender, void*);

public:
	UIFormPkMode();
	~UIFormPkMode();
};

#endif // _INC_UI_FORM_PKMODE_H_

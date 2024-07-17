#ifndef _INC_UI_FORM_GM_H_
#define _INC_UI_FORM_GM_H_

class GcActor;
class UIFormMsg;

class UIFormGm
{
public:
	enum EState
	{
		sUnRequest,		// δ����
		sRequesting,	// ���ڷ�������
		sInQueue,		// �����Ŷ�
		sService,		// ���ڱ�����
	};

	void Show();
	void Hide();
	bool IsVisible();										// �Ƿ��Ѿ���ʾ
	void Refresh();

	void OnRecvResponse(DWORD GmID, char* GmName, short QuestionCount, short order);
	void OnRecvMsg(DWORD id, char* name, char* content);

	void OnUpdateText_Chat(RtwWidget* sender, void*);
	void OnClicked_Send(void*, void*);

private:
	RtwWidget *m_pFrmThis;		// ��������
	RtwButton *m_pBtnSend;
	RtwChatInputBox *m_pChatInput;
	RtwChatBox *m_pChatList;
	RtwWidget *m_pLbQuestionCount;
	RtwWidget *m_pLbOrder;
	RtwWidget *m_pLbGmName;
	RtwWidget *m_pLbState;
	RtwTextBox *m_pTbTitle;
	RtwScrollBar*	m_pContentScroll;

private:
	void OnAutoScroll_Content(RtwWidget* sender, void*);
	void OnScroll_Content(RtwWidget* sender, void*);	

	void OnPreDraw();

	EState m_state;

public:
	UIFormGm();
	~UIFormGm();
};

#endif // _INC_UI_FORM_GM_H_

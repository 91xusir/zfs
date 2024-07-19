
#ifndef GC_MAIL_WITH_UI_H 
#define GC_MAIL_WITH_UI_H 

#include "gc_mail.h"
#include "ui/rtw_ui.h"
#include <string>
#include <list>
#include <map>

using std::list;
using std::multimap;
using std::string;


class CMailWithUI:public CClientMail
{
public:

	CMailWithUI();
	virtual ~CMailWithUI();
	void Init();
	void Reset();
	void Clear();



	//-------�������-------
	void UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UserHint(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void UserClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void MailBookUIActive(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void MailBookUIHide(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void ChangeFaceUI(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void ActiveFaceForm(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UserInterfaceSelect(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void UserInterfacebtndown(ui::RtwWidget* vpSender,void*);
	//-----�ͻ������е�-----
	virtual void OnReceive(SMailData* vpMail);								//���յ��ʼ���Ĵ���
	virtual void OnDelete(SMailData* vpMail);								//�ʼ�ɾ����Ĵ���
	//-------���û�---------
	virtual void Message(const string& vrMessage);
	//-------����ӿ�--------
	bool OnMemberChoice(DWORD vDBID,const char* vpName);
	void UpdateInfoAtFriendList();
	void DeleteMail(void*,void*);
	void ExportMail(void*,void*);
	void SendMail(void* ,void*);
	void NewMail(void*,void*);
	void CCMail(void*,void*);
	void ReplayMail(void*,void*);
	void ClearMailBookUI();
	void OpenMailBookUI();
	void OpenMailUI(void * ,void *);
	void ReturnToMailBox(void *sender ,void *);
	void Initface();
	void OnDragToSendItem(RtwWidget* sender, void*);
	void OnDragEndSendItem(RtwWidget* sender, void*);
	void OnGetHintItemText(RtwWidget* sender, void*);
	void OnGetHintReceiveItem(RtwWidget* sender, void*);
	void OnHideSendMail(RtwWidget* sender, void*);
	void OnLDClickMailList(RtwWidget* sender, void*);
	void OnReceiveItemAndMoney(RtwWidget* sender, void*);
	void OnSendButtonMouseButtonDown(RtwWidget* sender, void*);
	void OnClearMailList();
	void OnSendMoneyChanged(RtwWidget* sender, void*);
	bool IsMailUIVisible() 
	{
		guard;
		return (mpMailUI && mpMailUI->GetFlags(wfVisible)); 
		unguard;
	}
	//
	void SetMailButtonNormal();
	void SetMailButtonAnimation();
private:
	void OnSelectMail(SMailData* vpMail);
	void OnDisplayMail(SMailData* vpMail);									//����ʾһ���ʼ�ʱ
	void UpdateReceiveListUI();

	void OnEnsureDeleteMail(void*,void*);
	
	multimap<string,SMailData*,greater<string> >		mTitleMap;
	multimap<string,SMailData*,greater<string> >		mAuthorMap;
	multimap<string,SMailData*,greater<string> >		mTimeMap;
	multimap<string,SMailData*,greater<string> >*		mpCurrentReceiveList;		
	SMailData*						mpSelectMail;


	
	ui::RtwTextBox* mpPreviewUI;
	ui::RtwLabel*	mpPreviewFormUI;
	ui::RtwLabel*   mpPreviewAuthorUI;
	ui::RtwLabel*   mpPreviewTitleUI;
	ui::RtwLabel*   mpPreviewTimeUI;
	SMailData*		mpDisplayMail;
	RtwWidget*		mpInfoAtFriendListUI;


	ui::RtwWidget*  mpLastFocus;		//���һ�ε����뽹�㴰��
	ui::RtwWidget*	mpMailUI;			//�ռ���
	ui::RtwListBox*	mpReceiveListUI;	//�ռ����ϵ��ʼ��б�

	ui::RtwWidget*	mpMailBookUI;		//���ʼ�����
	ui::RtwWidget*	mpSendToNameUI;
	ui::RtwWidget*	mpTitleUI;
	ui::RtwTextBox* mpContentUI;
	ui::RtwWidget*  mpChatFaceUI;
	ui::RtwButton*  mpbtnface[12][8];
	ui::RtwWidget*  m_pfrmDisplayMail;
	ui::RtwButton*	m_pSendButton;
	ui::RtwButton*	m_pReceiveButton;
	ui::RtwTextBox*	m_pSendMoney;
	ui::RtwLabel*	m_pReceiveMoney;
	RtwImage*		m_imgAlreadyOpen;
	RtwImage*		m_imgNotOpen;
	SItemID			m_SendItemID;
	int				m_iISendItem;
	int				m_iJSendItem;
	int				m_iPage;
	SItemID			m_ReceiveItemID;
	long			m_lSendMoney;
	long			m_lReceiveMoney;

	RtwImage*		m_imgMailNormal;
	RtwImage*		m_imgMailAnimation;
	bool			m_bShowAnimation;
};









#endif//GC_MAIL_WITH_UI_H 
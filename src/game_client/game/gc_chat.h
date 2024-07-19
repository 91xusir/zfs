

#ifndef GC_CHAT_H
#define GC_CHAT_H

#include <vector>

#include "ui/rtw_ui.h"
#include "gc_edit_chat.h"
#include "gc_chat_assistant.h"
#include "game_filter.h"
typedef unsigned char BYTE;

using std::vector;

/* gao 2010.1.7
�Ĳ��ýṹ�����洢��������
*/
struct CHATCONTENT
{
	CHATCONTENT(int cha,DWORD col,long id,const char* des,char* nam,char* con)
	{
		channel			= cha;
		Color			= col;
		FromRegionID	= id;
		int tmp = strlen(des);
		Description		= RT_NEW char[tmp+1];
		memcpy(Description,des,tmp);
		Description[tmp] = 0;

		tmp = strlen(nam);
		name		= RT_NEW char[tmp+1];
		memcpy(name,nam,tmp);
		name[tmp] = 0;

		tmp = strlen(con);
		content		= RT_NEW char[tmp+1];
		memcpy(content,con,tmp);
		content[tmp] = 0;
	}
	int		channel;
	DWORD	Color;
	long	FromRegionID;
	char*	Description;
	char*	name;
	char*	content;
};
/* end */
class CChat:public GcTimerListener
{
	
public:
	CChat();
	~CChat();
	EChannels GetNowChannel()
	{
		guard;
		return m_eNowChannel;
		unguard;
	}
	void SetNowChannel(EChannels channel)
	{
		guard;
		m_eNowChannel = channel;
		unguard;
	}
	void Init();
	void Clear();
	//�յ���Ϣ//��������ʾ
	void Receive(char vChannel,long vID,char vName[],const char vMessage[]);
	//����Ϣ //������������
	void Send(char vMessage[]);
	// ������Ϣ���û��������
	void InputSpeakerMessage(){}
	// ���͵�������Ϣ����
	void RollMessage(const std::string& vrMessage,bool vIgnorRepeated){}
	// ���͵�������Ϣ����
	void SpeakerMessage(const std::string& vrMessage,bool vIgnoreRepeated){}
	// ���˰�ť����Ӧ�¼�
	void OnMaskbtnDown(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	// ��ȡ����༭��
	CEditChat GetEditChat();
	// ��ȡ��ǰ����������
	int* GetChatMask();
	// ��ȡ��ǰ�Ĵ洢�������ݵ�����
	list<CHATCONTENT*>& GetChatContent();
	// ˢ�µ�ǰ�����촰��
	void RefreshChatWidget();
	void OnSystemBtnDown(int key);
	void SetChatBackGround(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void ClearChatBanckGround();
	/* end */
	void DoFrame();

	int GetCurrentHeight();
	bool CheckChatFrequency(EChannels channel);//true��ʾOK

	void AddBlackList(const string& vrName);
	void SetNeedChangeBackground(bool need);
	void SystemMessage(const std::string& vrMessage,bool vIgnoreRepeated);//[string& ��Ϣ,bool �Ƿ�����ظ�����]
	void BattleInfoMessage(const std::string& vrMessage);//[string& ��Ϣ,bool �Ƿ�����ظ�����]
	bool OnMemberChoice(DWORD vDBID,const char* vpName);
	CEditChat getmEdit(){return mEdit;}

	SChannelAtom* GetChannel(char vChannel);								//��Ƶ���Ƿ���ָ������ͨѶ��ʶ��Ƶ��
	SChannelAtom* GetChannel(EChannels vChannel);							//ͨ��Ƶ��ö�ٵõ�һ��Ƶ��
	void LoadChatSet();

	RtwChatInputBox*			mpChatInput;
	CFilter<CFilterEqual>		mBlackList;

	RtwTab*			m_tabChat;
	RtwChatBox*		m_chatFeedback[MAX_CHANNEL_CHATBOX];
	RtwVScrollBar*	m_chatScroll[MAX_CHANNEL_CHATBOX];
	RtwButton*		m_btnChatSelect[MAX_CHANNEL_CHATBOX];
	RtwChatBox*		m_worldchatFeedback; 
	RtwChatBox*		m_systemFeedBack;

private:
	void Initfacebtn();

	void OnAutoScroll_Feedback(RtwWidget* sender, void*);
	void OnClick_Rfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnClick_Lfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnClick_Movefunction(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnMouseWheel(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnScroll_FocusWidget(RtwWidget* sender, void*);
	void OnScroll_ManualScroll(RtwWidget* sender, void*);
	void OnMenuRUser(RtwWidget* sender, RtwEventDelegate* e);
	void OnRClick_btnChatSelect(RtwWidget* sender, void*);
	void OnListDIY_Select(RtwWidget* sender, void*);
	void OnChangeChatName(RtwWidget* sender, RtwEventDelegate*vpEvent);
	void OnLoseFocus(RtwWidget* sender, void*);
	void SaveChatSet();

	// �����˵�����Ӧ�¼�
	void OnChannelListSelect(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnChannelListLoseFocus(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	//�û��ӿ�
	void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void UserInterfaceEvent(ui::RtwWidget* sender, RtwEventDelegate* e);
	void UserInterfaceKeyEvent(ui::RtwWidget* sender, RtwEventDelegate* e);
	void UserInterfacebtndown(ui::RtwWidget* sender,void*);
	void OnTimerEvent(DWORD vID);//ʱ��ص�����
	void UpdateChatText(RtwWidget* sender, void*);
	void UserInterfaceSelect(ui::RtwWidget* vpSender,void*);
	void OnSetMaskInit(int& mask);
	void OnGetMaskValue(int& mask);

	RtwPopupMenu*   m_popupMenu;

	int				m_iChatMask[MAX_CHANNEL_CHATBOX];
	int				m_WheelValue;
	int				m_WheelMin;
	int				m_WheelMax;
	std::string		strtext;

	RtwButton*		m_pbtnChatMask;						// �������ְ�ť
	RtwButton*		m_pbtnChannelSelect;				// ��ǰ����ģʽѡ��ť
	std::list<CHATCONTENT*>	m_vocChatContent;			// ��������
	RtwListBox*		m_plisChannelList;					// ��ǰ����ģʽ�б�
	RtwListBox*		m_plstDIY;							// ���찴ť�ϵ��Ҽ������˵�
	CUiCheckButton* m_checkChat[MAX_CHANNEL_BUTTON];
	int				m_iMask[MAX_CHANNEL_BUTTON];
	EChannels		m_eNowChannel;						// ��ǰ��������Ƶ���ı��

	vector<SChannelAtom>		mChannel;
	RtwButton		*m_btnface[12][8];
	RtwImage		* faceImage[12][8];
	RtwWidget		*m_faceform;
	//EChanneLists	mActiveList;
	//vector<EChannels>			mList[MAX_LIST];

	CEditChat		mEdit;
	int				mBoxHeight;//�����ĸ߶�
	bool			m_bNeedChangeBackground;

	//����ˢ�����
	int MessageCount;
	int Time[TIME_BUFFER];

	//��ս����ˢ�����
	int CountyMessageCount;
	int CountyTime[COUNTY_TIME_BUFFER];

	//void AddReceiveChannel(EChanneLists vList,EChannels vChannel);
	//void DelReceiveChannel(EChanneLists vList,EChannels vChannel);

	//void Save();
	//void Load();
	
	//void ClearChannelUI();
};

#endif//GC_CHAT_H
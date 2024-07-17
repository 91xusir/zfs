

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
改采用结构体来存储聊天内容
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
	//收到消息//解析并显示
	void Receive(char vChannel,long vID,char vName[],const char vMessage[]);
	//发消息 //解析，并发送
	void Send(char vMessage[]);
	// 喇叭消息的用户输入界面
	void InputSpeakerMessage(){}
	// 发送到滚动消息窗口
	void RollMessage(const std::string& vrMessage,bool vIgnorRepeated){}
	// 发送到喇叭消息窗口
	void SpeakerMessage(const std::string& vrMessage,bool vIgnoreRepeated){}
	// 过滤按钮的响应事件
	void OnMaskbtnDown(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	// 获取聊天编辑框
	CEditChat GetEditChat();
	// 获取当前的聊天遮罩
	int* GetChatMask();
	// 获取当前的存储聊天内容的容器
	list<CHATCONTENT*>& GetChatContent();
	// 刷新当前的聊天窗口
	void RefreshChatWidget();
	void OnSystemBtnDown(int key);
	void SetChatBackGround(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void ClearChatBanckGround();
	/* end */
	void DoFrame();

	int GetCurrentHeight();
	bool CheckChatFrequency(EChannels channel);//true表示OK

	void AddBlackList(const string& vrName);
	void SetNeedChangeBackground(bool need);
	void SystemMessage(const std::string& vrMessage,bool vIgnoreRepeated);//[string& 信息,bool 是否忽略重复内容]
	void BattleInfoMessage(const std::string& vrMessage);//[string& 信息,bool 是否忽略重复内容]
	bool OnMemberChoice(DWORD vDBID,const char* vpName);
	CEditChat getmEdit(){return mEdit;}

	SChannelAtom* GetChannel(char vChannel);								//该频道是否是指定网络通讯标识的频道
	SChannelAtom* GetChannel(EChannels vChannel);							//通过频道枚举得到一个频道
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

	// 弹出菜单的响应事件
	void OnChannelListSelect(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnChannelListLoseFocus(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	//用户接口
	void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void UserInterfaceEvent(ui::RtwWidget* sender, RtwEventDelegate* e);
	void UserInterfaceKeyEvent(ui::RtwWidget* sender, RtwEventDelegate* e);
	void UserInterfacebtndown(ui::RtwWidget* sender,void*);
	void OnTimerEvent(DWORD vID);//时间回调函数
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

	RtwButton*		m_pbtnChatMask;						// 聊天遮罩按钮
	RtwButton*		m_pbtnChannelSelect;				// 当前聊天模式选择按钮
	std::list<CHATCONTENT*>	m_vocChatContent;			// 聊天内容
	RtwListBox*		m_plisChannelList;					// 当前聊天模式列表
	RtwListBox*		m_plstDIY;							// 聊天按钮上的右键弹出菜单
	CUiCheckButton* m_checkChat[MAX_CHANNEL_BUTTON];
	int				m_iMask[MAX_CHANNEL_BUTTON];
	EChannels		m_eNowChannel;						// 当前窗口输入频道的标记

	vector<SChannelAtom>		mChannel;
	RtwButton		*m_btnface[12][8];
	RtwImage		* faceImage[12][8];
	RtwWidget		*m_faceform;
	//EChanneLists	mActiveList;
	//vector<EChannels>			mList[MAX_LIST];

	CEditChat		mEdit;
	int				mBoxHeight;//聊天框的高度
	bool			m_bNeedChangeBackground;

	//聊天刷屏检查
	int MessageCount;
	int Time[TIME_BUFFER];

	//国战聊天刷屏检查
	int CountyMessageCount;
	int CountyTime[COUNTY_TIME_BUFFER];

	//void AddReceiveChannel(EChanneLists vList,EChannels vChannel);
	//void DelReceiveChannel(EChanneLists vList,EChannels vChannel);

	//void Save();
	//void Load();
	
	//void ClearChannelUI();
};

#endif//GC_CHAT_H
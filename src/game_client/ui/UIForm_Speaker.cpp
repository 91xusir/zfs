#include "gc_include.h"
#include "UIForm_Speaker.h"
#include "gc_edit_chat.h"
#include "gc_chat.h"

CUIForm_Speaker::CUIForm_Speaker(void):
m_pfrmSpeaker(NULL),
m_ptbTextBox(NULL),
m_pbtnOK(NULL),
m_pbtnCancel(NULL)
{
	guard;
	// 初始化变量
	m_pfrmSpeaker	= LOAD_UI("layworld.fmspeaker");
	m_ptbTextBox	= LOAD_UI_T(RtwTextBox,"layworld.fmspeaker.ebcontent");
	m_pbtnOK		= LOAD_UI_T(RtwButton,"layworld.fmspeaker.btntok");
	m_pbtnCancel	= LOAD_UI_T(RtwButton,"layworld.fmspeaker.btntcancel");
	// 事件注册
	m_pfrmSpeaker->EvHide	+= RTW_CALLBACK(this,CUIForm_Speaker,FormHide);
	m_pbtnOK->EvLClick		+= RTW_CALLBACK(this,CUIForm_Speaker,ButtonClick);
	m_pbtnCancel->EvLClick	+= RTW_CALLBACK(this,CUIForm_Speaker,ButtonClick);
	m_ptbTextBox->SetWordWrap(true);
	m_ptbTextBox->SetMultiLine(true);

	unguard;
}
void CUIForm_Speaker::Show()
{
	guard;
	m_pfrmSpeaker->Show();
	unguard;
}

void CUIForm_Speaker::Hide()
{
	guard;
	m_pfrmSpeaker->Hide();
	unguard;
}

CUIForm_Speaker::~CUIForm_Speaker(void)
{
	guard;
	unguard;
}
void CUIForm_Speaker::Send()
{
	guard;
	string sendText = m_ptbTextBox->getText();

	if (GetWorld()->m_pPlayer && !GetWorld()->m_pPlayer->State_CanSpeak())
	{
		PlayerFailOperate(R(MSG_FORBID_SPEAK), true);
		return;
	}

	SChannelAtom* pChannel= NULL;
	pChannel = GetWorld()->m_Chat.GetChannel(SPEAKER_CHANNEL);
	// 判断是否超出最高字数上限
	if(strlen(sendText.c_str())>CHAT_MESSAGE_MAX_COUNT)
	{
		GetWorld()->m_Chat.SystemMessage(R(CHAT_LIMIT_STRING),true);
		static string Temp;
		Temp=sendText;
		Temp=Temp.substr(0,CHAT_MESSAGE_MAX_COUNT);
		sendText=Temp;
	}

	if(!GetWorld()->m_Chat.CheckChatFrequency(pChannel->mChannel))
	{//速度太快，不发送
		return;
	}

	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_chat);
	pPacket->WriteByte(pChannel->mNetChannel);			// 聊天频道
	pPacket->WriteString((char*)(sendText.c_str()));				// 内容 // 服务器上有检查长度
	NetSend(pPacket);

	unguard;
}
void CUIForm_Speaker::ButtonClick(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(vpSender == m_pbtnOK)
	{
		Send();
		m_pfrmSpeaker->Hide();
	}else if(vpSender == m_pbtnCancel)
	{
		m_ptbTextBox->SetText("");
		m_pfrmSpeaker->Hide();
	}
	unguard;
}
void CUIForm_Speaker::FormHide(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	m_ptbTextBox->SetText("");
	m_pfrmSpeaker->Hide();
	unguard;
}

void CUIForm_Speaker::OnDLClickedSpeakerItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(!m_pfrmSpeaker->IsVisible())
		m_pfrmSpeaker->Show();
	unguard;
}

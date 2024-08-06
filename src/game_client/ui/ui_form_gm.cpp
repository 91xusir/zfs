#include "gc_include.h"
#include "ui_form_gm.h"
#include <mmsystem.h> 
#define MAX_MESSAGE_BYTES		128
#define MAX_TITLE_BYTES			64

UIFormGm::UIFormGm()
{
	guard;

	m_state = sUnRequest;

	m_pFrmThis = LOAD_UI("fmgm");
	m_pBtnSend = LOAD_UI_T(RtwButton, "fmgm.btnok");
	m_pChatInput = LOAD_UI_T(RtwChatInputBox, "fmgm.txtdialog");
	m_pChatList = LOAD_UI_T(RtwChatBox, "fmgm.txtFeedback");
	//m_pChatList->EvScroll += RTW_CALLBACK(this, UIFormGm, OnAutoScroll_Content);
	m_pLbQuestionCount = LOAD_UI_T(RtwWidget, "fmgm.lbseqnum");
	m_pLbOrder = LOAD_UI_T(RtwWidget, "fmgm.lbseqnum1");
	m_pLbGmName = LOAD_UI_T(RtwWidget, "fmgm.lbgmname");
	m_pLbState = LOAD_UI_T(RtwWidget, "fmgm.lbseqstatus");
	m_pTbTitle = LOAD_UI_T(RtwTextBox, "fmgm.txttitle");

	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_pChatInput->EvUpdateText += RTW_CALLBACK(this, UIFormGm, OnUpdateText_Chat);
	m_pBtnSend->EvLClick += RTW_CALLBACK(this, UIFormGm, OnClicked_Send);

	m_pContentScroll = LOAD_UI_T(RtwScrollBar, "fmgm.scrolldialg");
	m_pContentScroll->EvScroll += RTW_CALLBACK(this, UIFormGm, OnScroll_Content);

	m_pLbState->SetText("");
	m_pLbOrder->SetText("");
	m_pLbQuestionCount->SetText("");
	m_pLbGmName->SetText("");

	m_pFrmThis->Hide();

	unguard;
}

UIFormGm::~UIFormGm()
{
	guard;

	unguard;
}

void UIFormGm::OnRecvMsg(DWORD id, char* name, char* content)
{
	guard;

	int filter = 0xffffffff;
	m_pChatList->Add(filter, 0xffffff46, "", name, content); // 0xff73943f

	if (!IsVisible())
	{
		g_layerMain->AddRemindIcon(UIRemind_GmResponse);
	}

	unguard;
}

void UIFormGm::OnRecvResponse(DWORD GmID, char* GmName, short QuestionCount, short order)
{
	guard;

	if (order==0)
	{
		// 请求正在被服务
		m_state = sService;
		m_pLbState->SetText(R(MSG_GM_BE_IN_SERVICE));
		m_pLbGmName->SetText(GmName);
		m_pLbQuestionCount->SetText("0");
		m_pLbOrder->SetText("0");
	}
	else if (order>0)
	{
		// 正在排队
		m_state = sInQueue;
		m_pLbState->SetText(R(MSG_GM_ISINQUEUE));
		m_pLbGmName->SetText("");
		m_pLbQuestionCount->SetText(rtFormatNumber(QuestionCount));
		m_pLbOrder->SetText(rtFormatNumber(order));
	}
	else if (order==-1)
	{
		// 队列已满
		m_state = sUnRequest;
		m_pLbState->SetText(R(MSG_GM_QUEUEFULL));
		m_pLbGmName->SetText("");
		m_pLbQuestionCount->SetText("0");
		m_pLbOrder->SetText("0");
	}
	else if (order==-2)
	{
		// 没有在线的GM
		m_state = sUnRequest;
		m_pLbState->SetText(R(MSG_GM_NOGMONLINE));
		m_pLbGmName->SetText("");
		m_pLbQuestionCount->SetText("0");
		m_pLbOrder->SetText("0");
	}
	else if (order==-3)
	{
		// GM已经结束了与您的交谈
		m_state = sUnRequest;

		m_pChatList->Add(0xffffffff, 0xffffff46, R(SYSTEM_CHANNEL_DESCRIPTION), "", (char*)R(MSG_GM_END_SERVICE));

		m_pLbState->SetText(R(MSG_GM_END_SERVICE));
		m_pTbTitle->SetText("");
		m_pLbGmName->SetText("");
		m_pLbQuestionCount->SetText("0");
		m_pLbOrder->SetText("0");
	}
	else 
	{
		m_state = sUnRequest;
		m_pTbTitle->SetText("");
		m_pLbGmName->SetText("");
		m_pLbQuestionCount->SetText("0");
		m_pLbOrder->SetText("0");
	}

	Refresh();

	unguard;
}

void UIFormGm::OnPreDraw()
{
	guard;

	switch (m_state)
	{
	case sUnRequest:
		m_pTbTitle->Enable();
		m_pChatInput->Enable();
		m_pBtnSend->Enable();
		break;
	case sRequesting:
		m_pTbTitle->Disable();
		m_pChatInput->Disable();
		m_pBtnSend->Disable();
		break;
	case sInQueue:
		m_pTbTitle->Disable();
		m_pChatInput->Disable();
		m_pBtnSend->Disable();
		break;
	case sService:
		m_pTbTitle->Disable();
		m_pChatInput->Enable();
		m_pBtnSend->Enable();
		break;
	default:
		break;
	}

	unguard;
}

void UIFormGm::Show()
{
	guard;
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormGm::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormGm::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormGm::Refresh()
{
	guard;
	if ( IsVisible() )		Show();
	unguard;
}

void UIFormGm::OnUpdateText_Chat(RtwWidget* sender, void*)
{
	guard;

	if (m_pChatInput->GetText().empty())
		return;

	string titleForSend = "";

	switch (m_state)
	{
	case sUnRequest:
		titleForSend = m_pTbTitle->GetText();
		if (titleForSend.empty())
		{
			PlayerFailOperate(R(MSG_GM_TITLE_REQUIRED));
			return;
		}
		break;
	case sRequesting:
		return;
		break;
	case sInQueue:
		return;
		break;
	case sService:
		break;
	default:
		return;
		break;
	}

	static int MessageCount=0;
	static int Time[TIME_BUFFER];

	//防止刷屏的检查
	DWORD NowTime=timeGetTime();
	if(MessageCount<TIME_BUFFER)
	{
		Time[MessageCount]=NowTime;
		MessageCount++;
	}
	else
	{
		if(NowTime-Time[0] < CHECK_TIME)
		{//说话的速度太快
			m_pChatList->Add(0xffffffff, 0xffffff46, R(SYSTEM_CHANNEL_DESCRIPTION), "", (char*)R(LIMIT_MESSAGE));
			return;
		}

		for(int i=0;i<TIME_BUFFER-1;i++)
		{
			Time[i]=Time[i+1];
		}
		Time[TIME_BUFFER-1]=NowTime;
	}

	// 防止字符串溢出
	if(m_pChatInput->GetText().size() > MAX_MESSAGE_BYTES)
	{
		PlayerFailOperate(R(CHAT_LIMIT_STRING));
		return;
	}
	if(m_pTbTitle->GetText().size() > MAX_TITLE_BYTES)
	{
		PlayerFailOperate(R(CHAT_LIMIT_STRING));
		return;
	}



	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_chat);
	pPacket->WriteByte(CHAT_CHANNEL_GM);
	if (m_state==sService)
	{
		pPacket->WriteByte(0);
	}
	else 
	{
		pPacket->WriteByte(1);
		pPacket->WriteString((char*)titleForSend.c_str()); // 服务器上有检查长度
	}
	pPacket->WriteString((char*)m_pChatInput->GetText().c_str()); // 服务器上有检查长度
	NetSend(pPacket);

	m_pChatList->Add(0xffffffff, 0xffffffff, "", (char*)GetWorld()->m_pPlayer->GetName(), (char*)m_pChatInput->GetText().c_str());
	m_pChatInput->SetText("");
	m_pChatInput->SetFocus();
	
	unguard;
}

void UIFormGm::OnClicked_Send(void*, void*)
{
	guard;
	OnUpdateText_Chat(NULL, NULL);
	unguard;
}

void UIFormGm::OnAutoScroll_Content(RtwWidget* sender, void*)
{
	guard;

	m_pContentScroll->SetMaximum(m_pChatList->GetScrollRange());
	m_pContentScroll->SetValue(m_pChatList->GetScrollPosition());

	unguard;
}

void UIFormGm::OnScroll_Content(RtwWidget* sender, void*)
{
	guard;

	m_pChatList->SetScrollPosition(m_pContentScroll->GetValue());

	unguard;
}


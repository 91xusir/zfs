#include "gc_include.h"
#include "gc_chat.h"
#include "region_client.h"
#include "game_script_interface.h"
#include "gc_chat_const.h"
#include "game_string_assistant.h"
#include "filepack/ansi_file.h"
#include "filepack/memory_buffer.h"
#include "game_team_share_const.h"
#include "ui_form_textMsg.h"
#include <mmsystem.h> 
const char LOCAL_BLACKLIST_DIRECTORY[]="m_data";
const char LOCAL_BLACKLIST_PREFIX[]="bl_";
const int LOCATION_BLACKLIST_NO_CLOSE		=0;					//本地黑名单信息没有正常退出[正常应该非零]
const int LAST_LOCATION_BLACKLIST_VERSION   =100;				//最新地本地黑名单的版本
//const int MAX_BLACKLIST_COUNT=200;							//最大的黑名单数


CChat::CChat()
:mChannel(),
//mActiveList(),
mEdit(),
mBoxHeight(MAX_HEIGHT_STATE/2),//最开始的状态是最大的高度的一半
MessageCount(0),
/*ForfendTime(0),
LastOffend(0),
BeginForfendTime(0),*/
CountyMessageCount(0),
m_WheelValue(0),
m_WheelMin(0),
m_WheelMax(100)
{
    guard;
	for(int i=0;i<TIME_BUFFER;i++)
	{
		Time[i]=0;
	}

	for(int j=0;j<COUNTY_TIME_BUFFER;j++)
	{
		CountyTime[j]=0;
	}

	////设置各个频道列表的默认频道
	///* gao 2010.1.4
	//修正各频道列表显示的频道
	//*/
	//// 综合列表
	//AddReceiveChannel(ALL_LIST,PERSONAL_CHANNEL);
	//AddReceiveChannel(ALL_LIST,TEAM_CHANNEL);
	//AddReceiveChannel(ALL_LIST,UNION_CHANNEL);
	//AddReceiveChannel(ALL_LIST,BATTLE_CHANNEL);
	//AddReceiveChannel(ALL_LIST,NEAR_CHANNEL);
	//AddReceiveChannel(ALL_LIST,WORLD_CHANNEL);

	//// 附近列表
	//AddReceiveChannel(NEAR_LIST,NEAR_CHANNEL);

	//// 组队列表
	//AddReceiveChannel(TEAM_LIST,TEAM_CHANNEL);

	//// 帮派列表
	//AddReceiveChannel(UNION_LIST,UNION_CHANNEL);

	//// 战场列表
	//AddReceiveChannel(BATTLE_LIST,BATTLE_CHANNEL);

	//// 世界列表
	//AddReceiveChannel(WORLD_LIST,WORLD_CHANNEL);

	//// 密语列表
	//AddReceiveChannel(PERSONAL_LIST,PERSONAL_CHANNEL);

	m_iChatMask[0] = PERSONAL_CHANNEL_MASK | TEAM_CHANNEL_MASK | UNION_CHANNEL_MASK | BATTLE_CHANNEL_MASK | NEAR_CHANNEL_MASK | WORLD_CHANNEL_MASK | BATTLEINFO_CHANNEL_MASK;
	m_iChatMask[1] = PERSONAL_CHANNEL_MASK;
	m_iChatMask[2] = TEAM_CHANNEL_MASK;
	m_iChatMask[3] = UNION_CHANNEL_MASK;
	m_iChatMask[4] = BATTLE_CHANNEL_MASK;
	m_iChatMask[5] = WORLD_CHANNEL_MASK;
	m_iChatMask[6] = BATTLEINFO_CHANNEL_MASK;
	m_eNowChannel = NEAR_CHANNEL;
	/* end */
    unguard;
}
CChat::~CChat()
{
	guard;
	std::list<CHATCONTENT*>::iterator it;
	// 将最后的窗口内容清空，并释放资源
	for (it = m_vocChatContent.begin();it != m_vocChatContent.end();++it)
	{
		DEL_ONE((*it)->Description);
		DEL_ONE((*it)->name);
		DEL_ONE((*it)->content);
		DEL_ONE(*it);
	}
	m_vocChatContent.clear();
	unguard;
}
void CChat::Init()
{
	guard;
	m_tabChat		= LOAD_UI_T(RtwTab,"tbfeedback");
	char tmpStr[100];
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.txtFeedback",i);
		m_chatFeedback[i]	= LOAD_UI_T(RtwChatBox, tmpStr);
		// 	m_chatFeedback->ModifyFlags(0,wfMouseMove);
		m_chatFeedback[i]->EvScroll		+= RTW_CALLBACK(this, CChat, OnAutoScroll_Feedback);
		m_chatFeedback[i]->EvRClick		+= RTW_CALLBACK(this, CChat, OnClick_Rfunction);
		m_chatFeedback[i]->EvLClick		+= RTW_CALLBACK(this, CChat, OnClick_Lfunction);
		m_chatFeedback[i]->EvMouseMove	+= RTW_CALLBACK(this, CChat, OnClick_Movefunction);
		m_chatFeedback[i]->EvMouseWheel	+= RTW_CALLBACK(this, CChat, OnMouseWheel);
		//m_chatFeedback->SetDrawHeader(false);
		m_chatFeedback[i]->SetEnableInput(false);
		m_chatFeedback[i]->SetScrollBarV(true);

		rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.scrolldialg",i);
		m_chatScroll[i]						= LOAD_UI_T(RtwVScrollBar, tmpStr);
		m_chatScroll[i]->SetValue(100);
		m_chatScroll[i]->EvMouseMove		+= RTW_CALLBACK(this, CChat, OnScroll_FocusWidget);
		m_chatScroll[i]->EvScroll			+= RTW_CALLBACK(this, CChat, OnScroll_ManualScroll);

		//
		rt2_snprintf(tmpStr,100,"tbfeedback.btnchat%d",i);
		m_btnChatSelect[i]					= LOAD_UI_T(RtwButton,tmpStr);
		m_btnChatSelect[i]->EvRClick		+=RTW_CALLBACK(this,CChat,OnRClick_btnChatSelect);
	}

	//添加世界框
	m_worldchatFeedback = LOAD_UI_T(RtwChatBox,"worldfmfeedback.txtFeedback");
	m_worldchatFeedback->SetEnableInput(false);
	m_worldchatFeedback->EvLClick	+= RTW_CALLBACK(this, CChat, OnClick_Lfunction);

	m_systemFeedBack = LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID);
	m_systemFeedBack->EvRClick		+= RTW_CALLBACK(this, CChat, OnClick_Rfunction);
	m_systemFeedBack->EvLClick		+= RTW_CALLBACK(this, CChat, OnClick_Lfunction);
	// 	m_systemFeedBack->SetScrollBarV(true);
	// 	m_systemFeedBack->SetAutoScrollV(true);
	// 	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->EvScroll += RTW_CALLBACK(this, RtwTextBox, OnScroll_ScrollBarV);


	m_popupMenu = LOAD_UI_T(RtwPopupMenu, "RuserMenu");
	m_popupMenu->AddItem("密语");
	m_popupMenu->AddItem("复制名字");
	m_popupMenu->AddItem("添加好友");		//heten
	m_popupMenu->AddItem("添加黑名单");
	m_popupMenu->AddItem(R(POP_MENU_INVITE_STRING),NULL,(void*)5);//邀请
	m_popupMenu->AddItem(R(POP_MENU_APPLYTEAM),NULL,(void*)6);//邀请
	m_popupMenu->AddItem(R(POP_MENU_MASKPLAYER),NULL,(void*)7);//邀请
	m_popupMenu->AddItem(R(G_SEEITEMS), NULL, (void*)8);
	m_popupMenu->Hide();
	m_popupMenu->EvMenuSelect		+= RTW_CALLBACK(this, CChat, OnMenuRUser);

	//初始化频道的标记和网络上传的标记
	//私人频道
	mChannel.push_back(SChannelAtom(PERSONAL_CHANNEL	,CHAT_CHANNEL_PRIVATE	,PERSONAL_CHANNEL_MASK	,R(PERSONAL_CHANNEL_DESCRIPTION),PERSONAL_CHANNEL_COLOR));
	//队伍频道
	mChannel.push_back(SChannelAtom(TEAM_CHANNEL		,CHAT_CHANNEL_TEAM		,TEAM_CHANNEL_MASK		,R(TEAM_CHANNEL_DESCRIPTION)    ,TEAM_CHANNEL_COLOR));
	//诸侯频道
	mChannel.push_back(SChannelAtom(UNION_CHANNEL		,CHAT_CHANNEL_PARTY		,UNION_CHANNEL_MASK		,R(GUILD_CHANNEL_DESCRIPTION)   ,UNION_CHANNEL_COLOR));
	//一般频道
	mChannel.push_back(SChannelAtom(NEAR_CHANNEL		,CHAT_CHANNEL_NORMAL	,NEAR_CHANNEL_MASK		,R(NORMAL_CHANNEL_DESCRIPTION)  ,NEAR_CHANNEL_COLOR));
	//公开频道
	mChannel.push_back(SChannelAtom(WORLD_CHANNEL		,CHAT_CHANNEL_PUBLIC	,WORLD_CHANNEL_MASK		,R(PUBLIC_CHANNEL_DESCRIPTION)  ,WORLD_CHANNEL_COLOR));
	//世界频道
	mChannel.push_back(SChannelAtom(SPEAKER_CHANNEL		,CHAT_CHANNEL_WORLD		,SPEAKER_CHANNEL_MASK	,R(WORLD_CHANNEL_DESCRIPTION)   ,WORLD_CHANNEL_COLOR));
	//国家频道
	mChannel.push_back(SChannelAtom(BATTLE_CHANNEL		,CHAT_CHANNEL_COUNTRY	,BATTLE_CHANNEL_MASK	,R(COUNTRY_CHANNEL_DESCRIPTION) ,BATTLE_CHANNEL_COLOR));
	//战斗信息
	mChannel.push_back(SChannelAtom(BATTLEINFO_CHANNEL	,CHAT_CHANNEL_BATTLEINFO,BATTLEINFO_CHANNEL_MASK,R(BATTLEINFO_CHANNEL_DESCRIPTION),SYSTEM_CHANNEL_COLOR));
	//系统频道
	mChannel.push_back(SChannelAtom(SYSTEM_CHANNEL		,CHAT_CHANNEL_SYSTEM	,SYSTEM_CHANNEL_MASK	,R(SYSTEM_CHANNEL_DESCRIPTION)  ,SYSTEM_CHANNEL_COLOR));

	//设置系统信息的过滤器
	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->SetFilter(SYSTEM_CHANNEL_MASK);
	//LOAD_UI(UI_SYSTEM_FORM_ALL_WHOLE_ID)->ModifyFlags(0,wfGrabMouse);	//将系统信息框的所有的鼠标事件移除
	//LOAD_UI(UI_SYSTEM_FORM_ALL_WHOLE_ID)->ModifyFlags(0,wfPick);
	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->SetEnableInput(false);
	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->SetClearTime(SYSTEM_INFO_SCROLL_TIME);
	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->ClearBackGround();
	//将时间消息关联到该类上
	GetTimer()->AddTimer(this,SYSTEM_INFO_CLEAR_TIME);

	LOAD_UI(UI_CHANNEL_BUTTON_WHOLE_ID)->EvLClick	+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);

	//聊天频道的复选框
	LOAD_UI(UI_NORMAL_CHANNEL_WHOLE_ID)->EvLClick	+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI(UI_PUBLIC_CHANNEL_WHOLE_ID)->EvLClick	+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI(UI_PERSONAL_CHANNEL_WHOLE_ID)->EvLClick	+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI(UI_TEAM_CHANNEL_WHOLE_ID)->EvLClick		+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI(UI_GUILD_CHANNEL_WHOLE_ID)->EvLClick		+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI(UI_COUNTRY_CHANNEL_WHOLE_ID)->EvLClick	+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	LOAD_UI("fmchannel.ckbattleinfo")->EvLClick		+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);

	m_checkChat[0]			= LOAD_UI_T(CUiCheckButton,UI_NORMAL_CHANNEL_WHOLE_ID);
	m_iMask[0]				= NEAR_CHANNEL_MASK;
	m_checkChat[1]			= LOAD_UI_T(CUiCheckButton,UI_PUBLIC_CHANNEL_WHOLE_ID);
	m_iMask[1]				= WORLD_CHANNEL_MASK;
	m_checkChat[2]			= LOAD_UI_T(CUiCheckButton,UI_PERSONAL_CHANNEL_WHOLE_ID);
	m_iMask[2]				= PERSONAL_CHANNEL_MASK;
	m_checkChat[3]			= LOAD_UI_T(CUiCheckButton,UI_TEAM_CHANNEL_WHOLE_ID);
	m_iMask[3]				= TEAM_CHANNEL_MASK;
	m_checkChat[4]			= LOAD_UI_T(CUiCheckButton,UI_GUILD_CHANNEL_WHOLE_ID);
	m_iMask[4]				= UNION_CHANNEL_MASK;
	m_checkChat[5]			= LOAD_UI_T(CUiCheckButton,UI_COUNTRY_CHANNEL_WHOLE_ID);
	m_iMask[5]				= BATTLE_CHANNEL_MASK;
	m_checkChat[6]			= LOAD_UI_T(CUiCheckButton,"fmchannel.ckbattleinfo");
	m_iMask[6]				= BATTLEINFO_CHANNEL_MASK;

	LOAD_UI(UI_CHANNEL_WHOLE_ID)->EvActivate			+=RTW_CALLBACK(this,CChat,UserInterfaceEvent);


	//输入框的键盘事件
	mpChatInput=LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID);
	mpChatInput->EvKey			+= RTW_CALLBACK(this,CChat,UserInterfaceKeyEvent);
	mpChatInput->EvUpdateText	+= RTW_CALLBACK(this,CChat,UpdateChatText);

	//doskey的界面
	LOAD_UI_T(RtwListBox,UI_DOSKEY_LIST_WHOLE_ID)->EvSelect	+= RTW_CALLBACK(this,CChat,UserInterfaceSelect);

	//对话背景的开关
	//LOAD_UI("fmdialg.btnFeedbackclose")->ModifyFlags(wfDClick,0);//可以双击
	LOAD_UI(UI_FEEDBACK_SWITCH_WHOLE_ID)->EvLClick			+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);

	//对话用表情栏目的开关
	//LOAD_UI("fmdialg.btnchatface")->ModifyFlags(wfDClick,0);//可以双击
	LOAD_UI(UI_CHAT_FACE_SWITCH_WHOLE_ID)->EvLClick			+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);

	//表情框的选择事件tooth.shi
	// 	LOAD_UI_T(RtwIconList,UI_CHAT_FACE_ICON_LIST_WHOLE_ID)->EvSelect+=RTW_CALLBACK(this,CChat,UserInterfaceSelect);
	LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->EvActivate			+= RTW_CALLBACK(this,CChat,UserInterfaceEvent);
	/* gao 2010.1.6 */	
	//聊天遮罩按钮的初始化
	for (int i = 0;i < 6;++i)
		Time[i] = 0;

	m_pbtnChatMask = LOAD_UI_T(RtwButton,"layworld.fmdialg.channelall");
	m_pbtnChatMask->Disable();
	m_pbtnChatMask->EvLClick	+= RTW_CALLBACK(this, CChat, OnMaskbtnDown);

	m_pbtnChannelSelect = LOAD_UI_T(RtwButton,"fmdialg.btnchanel");
	//喇叭窗口的初始坐标计算
	RtwRect	Rect=m_tabChat->GetFrameRect();
	RtwRect rect2 = LOAD_UI("worldfmfeedback")->getFrameRect();
	int height = rect2.top - rect2.bottom;
	rect2.top = Rect.top + height;
	rect2.bottom = Rect.top;
	LOAD_UI("worldfmfeedback")->MoveResize(rect2,true);

	m_plisChannelList = LOAD_UI_T(RtwListBox, "listchannel");
	m_plisChannelList->AddItem("....");
	m_plisChannelList->AddItem("附近");
	m_plisChannelList->AddItem("队伍");
	m_plisChannelList->AddItem("帮派");
	m_plisChannelList->AddItem("战场");
	m_plisChannelList->AddItem("世界");
	m_plisChannelList->AddItem("密语");
	m_plisChannelList->EvSelect += RTW_CALLBACK(this, CChat, OnChannelListSelect);
	m_plisChannelList->EvUnFocus+= RTW_CALLBACK(this, CChat, OnChannelListLoseFocus);
	// 	LOAD_UI("fmfeedback")->EvMouseMove		+= RTW_CALLBACK(this,CChat,SetChatBackGround);

	m_plstDIY		= LOAD_UI_T(RtwListBox,"fmdiy.listdiy");
	m_plstDIY->AddItem("频道过滤设置");
	m_plstDIY->AddItem("频道名称设置");
	m_plstDIY->EvSelect								+= RTW_CALLBACK(this,CChat,OnListDIY_Select);
	//LOAD_UI("fmdiy")->EvUnFocus					+= RTW_CALLBACK(this,CChat,OnLoseFocus);
	//LOAD_UI(UI_CHANNEL_WHOLE_ID)->EvUnFocus		+= RTW_CALLBACK(this,CChat,OnLoseFocus);
	/* end */

	Initfacebtn();
	//LoadChatSet();
	unguard;
}

void CChat::Initfacebtn()
{
	guard;
#define BFID_SIZE_X		24
#define BFID_SIZE_Y		24
	char strId[32];
	m_faceform = LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID);

	for (int i=0; i<5; i++)
		for (int j=0; j<8; j++)
		{	
			rt2_sprintf(strId, "ui\\face\\%d.gif", i*8+j);
			faceImage[i][j] = g_workspace.getImageFactory()->createImage(strId);
			faceImage[i][j]->SetSourceRect(RtwRect(0, 0, 32, 32));
			rt2_sprintf(strId, "face%02d_%02d", i, j);
			faceImage[i][j]->m_Name=strId;
			RtwRect sRect;
			sRect.left = (BFID_SIZE_Y+3)*j;
			sRect.top = (BFID_SIZE_Y+3)*i;
			sRect.right = sRect.left + BFID_SIZE_Y;
			sRect.bottom = sRect.top + BFID_SIZE_Y;
			rt2_sprintf(strId, "btnface%02d_%02d", i, j);
			m_btnface[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnface[i][j]->SetName(strId);
			m_btnface[i][j]->SetButtonStateImages(faceImage[i][j],NULL,NULL,NULL);
			m_faceform->AddChild(m_btnface[i][j]);		
			m_btnface[i][j]->Move(sRect);
			m_btnface[i][j]->SetWidgetSize(SSize(24,24));
			m_btnface[i][j]->EvLClick+=RTW_CALLBACK(this,CChat,UserInterfacebtndown);
		}
		unguard;
}

void CChat::UserInterfacebtndown(ui::RtwWidget* vpSender,void*)
{
	guard;
	const string& Sender=vpSender->GetQualifiedID();
	if(LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->GetFlags(wfVisible))
	{
		LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Hide();
	}
	else
	{
		LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Show();
	}

	for (int i=0; i<5; i++)
		for (int j=0; j<8; j++)
		{	
			if (Sender==m_btnface[i][j]->getName())
			{		
				mpChatInput->InsertImage(m_btnface[i][j]->getNormalImage());
				g_workspace.SetFocusWidget(mpChatInput);
				return;
			}	
		}
	
	unguard;
}
void CChat::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{

	RtwChatBox::SItem* itemname =((RtwChatBox*)pWidget)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	if (itemname)
	{
		if (itemname->type==RtwChatBox::eit_Name)
		{
			if (itemname->text.substr(0,itemname->text.size()-2)==GetWorld()->m_pPlayer->GetName())
			{
				return;
			}
			string name;
			name=itemname->text.substr(0,itemname->text.size()-1);
			OnMemberChoice(0,name.c_str());
		}
	}
}

bool CChat::OnMemberChoice(DWORD vDBID,const char* vpName)
{
    guard;
	m_pbtnChannelSelect->SetText(m_plisChannelList->GetItemText((int)PERSONAL_CHANNEL+1,0));
	m_eNowChannel = PERSONAL_CHANNEL;
	mEdit.ReplacePrivateName(string(vpName));
	m_plisChannelList->SetSelect((int)PERSONAL_CHANNEL+1);
	g_workspace.SetFocusWidget(mpChatInput);//将该窗口激活
	return false;
    unguard;
}

void CChat::OnTimerEvent(DWORD vID)//时间回调函数
{
    guard;
	//int ranI = rand()%7;
	//SChannelAtom*  pChannel=GetChannel(SYSTEM_CHANNEL);
	//switch(ranI)
	//{
	//case 0:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_ZHIFUSHU));
	//	break;
	//case 1:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_RONGHESHU));
	//	break;
	//case 2:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_FABAOLIANHUA));
	//	break;
	//case 3:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_FABAOFENJIE));
	//	break;
	//case 4:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_BAITAN));
	//	break;
	//case 5:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_SHOUYI));
	//	break;
	//case 6:
	//	LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->Add(pChannel->mMask,0xFF90EE90,pChannel->mDescription.c_str(),"",(char*)R(MSG_TIPS_QISHOUSHU));
	//	break;
	//default:
	//	break;
	//}
	
    unguard;
}

int CChat::GetCurrentHeight()
{
    guard;
	RtwRect Rect=m_tabChat->GetFrameRect();
	return Rect.top;
    unguard;
}

void CChat::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
    guard;
	const string& Sender=vpSender->GetID();

	/* gao 2010.1.9
	原遮罩的响应事件注释
	*/
	if(Sender==UI_CHANNEL_BUTTON_ID)
	{
		if(m_plisChannelList->IsVisible())
			m_plisChannelList->Hide();
		else
		{
			m_plisChannelList->Show();
			g_workspace.SetFocusWidget(m_plisChannelList);
		}
	}
	else if(Sender==UI_FEEDBACK_SWITCH_ID )
	{//聊天背景开关按钮
		mBoxHeight++;
		mBoxHeight=mBoxHeight % MAX_HEIGHT_STATE;

		//如果Height为0，表示恢复回原来大小，则加上MAX_HEIGHT_STATE-2*EACH_HEIGHT高度//最小一档没有改变大小所以-2
		int SubtractHeight=(mBoxHeight==0 ? -((MAX_HEIGHT_STATE-2)*EACH_HEIGHT) :  EACH_HEIGHT);

		//如果最小的情况下，不显示
		if(mBoxHeight== (MAX_HEIGHT_STATE-1))
		{//不显示
			m_tabChat->Hide();
			/* gao 2010.1.9 让喇叭窗口跟着进行相就的更改*/
			LOAD_UI("worldfmfeedback")->Hide();
			//LOAD_UI("fmchannelbtn")->Hide();
			
		}
		else
		{//显示//并改变
			m_tabChat->Show();
			/* gao 2010.1.9 让喇叭窗口跟着进行相就的更改*/
			LOAD_UI("worldfmfeedback")->Show();
			
			RtwRect Rect;
			char tmpStr[100];
			
			int iSelect= m_tabChat->GetSelectedItem();
			//整个外观
			Rect=m_tabChat->GetFrameRect();
			Rect.top+= SubtractHeight;				//y向下偏移
			m_tabChat->MoveResize(Rect,true);
			for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
			{
				// 整体form
				rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d",i);
				Rect=LOAD_UI(tmpStr)->GetFrameRect();
				Rect.top+= SubtractHeight;				//y向下偏移
				//Rect.bottom-= SubtractHeight;				//y向下偏移
				LOAD_UI(tmpStr)->MoveResize(Rect,true);
				//上字部分
				Rect= m_chatFeedback[i]->GetFrameRect();
				Rect.top+= SubtractHeight;				//y向下偏移
				//Rect.bottom-=SubtractHeight;				//抵消外部窗口的y值变化，改变y值，y的绝对数值，不变
				m_chatFeedback[i]->MoveResize(Rect,true);
				//滚动条部分
				Rect= m_chatScroll[i]->GetFrameRect();
				//Rect.top+= SubtractHeight;				//y向下偏移
				Rect.bottom-= SubtractHeight;				//y向下偏移
				m_chatScroll[i]->MoveResize(Rect,true);
				//背景部分
				rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.back",i);
				Rect=LOAD_UI(tmpStr)->GetFrameRect();
				//Rect.top+= SubtractHeight;				//y向下偏移
				Rect.bottom-= SubtractHeight;				//y向下偏移
				LOAD_UI(tmpStr)->MoveResize(Rect,true);
				//聊天框的装饰物件
				rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.pandialgtop",i);
				Rect=LOAD_UI(tmpStr)->GetFrameRect();
				//Rect.top+= SubtractHeight;				//y向下偏移
				Rect.bottom-=SubtractHeight;				//抵消外部窗口的y值变化，改变y值，y的绝对数值，不变
				LOAD_UI(tmpStr)->MoveResize(Rect);
			}

			//heten  世界聊天框移动
			Rect=LOAD_UI("worldfmfeedback")->GetFrameRect();
			Rect.top+= SubtractHeight;				//y向下偏移
			Rect.bottom+= SubtractHeight;				//y向下偏移
			LOAD_UI("worldfmfeedback")->MoveResize(Rect,true);
			//end
		} 
	}
	else if(Sender==UI_CHAT_FACE_SWITCH_ID)
	{//表情框按钮
		if(LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->GetFlags(wfVisible))
		{
			LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Hide();
		}
		else
		{
			LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Show();
		}
	}

	//聊天频道复选框
	int iSelect= m_tabChat->GetSelectedItem();
	for(int i= 0;i< MAX_CHANNEL_BUTTON;i++)//去掉的三个为，喇叭，系统，GM
	{
		if(m_checkChat[i]== vpSender)
		{
			if(((CUiCheckButton*)vpSender)->GetChecked())
				m_iChatMask[iSelect] |= m_iMask[i];
			else 
				m_iChatMask[iSelect] &= (~(m_iMask[i]));
			return;
		}
	}

	//频道复选框失去焦点
	if(Sender==UI_CHANNEL_WHOLE_ID)
	{
		if(e->type == etUnFocus)
		{
			LOAD_UI(UI_CHANNEL_WHOLE_ID)->Hide();
		}
	}
	else if(Sender==UI_CHAT_FACE_FORM_ID)
	{//表情框失去焦点
		if(e->type == etUnFocus)
		{
			LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Hide();
		}
	}
    unguard;
}

//void CChat::ClearChannelUI()
//{
//    guard;
//	for(int i=0;i<MAX_CHANNEL;i++)
//	{
//		if(i!=SYSTEM_CHANNEL)
//		{
//			SChannelAtom* pChannel=GetChannel((EChannels)i);
//			if(pChannel!=NULL)
//			{
//				LOAD_UI_T(CUiCheckButton ,pChannel->mUIID.c_str())->SetChecked(false);
//			}
//		}
//	}
//    unguard;
//}

void CChat::UpdateChatText(RtwWidget* sender, void*)
{
	guard;
	Send((char*)(mpChatInput->GetText().c_str()));
	unguard;
}

void CChat::Clear()
{
    guard;
	//Save();
	GetTimer()->RemoveObject(this);//将时间消息取消关联
    unguard;
}

void CChat::OnMaskbtnDown(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	// 去调用综合过滤
	//if(LOAD_UI(UI_CHANNEL_WHOLE_ID)->GetFlags(wfVisible))
	//{
	//	LOAD_UI(UI_CHANNEL_WHOLE_ID)->Hide();
	//}
	//else
	//{
	//	LOAD_UI(UI_CHANNEL_WHOLE_ID)->Show();
	//	g_workspace.BringWidgetToBottom(LOAD_UI_T(RtwForm, "fmfeedback"));
	//	g_workspace.SetFocusWidget(LOAD_UI(UI_CHANNEL_WHOLE_ID));
	//}
	//// 跟据当前遮罩的复选框选择进行配置综合的遮罩
	//m_iChatMask = 0;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.cknormalcl")->GetChecked())
	//	m_iChatMask |= NEAR_CHANNEL_MASK;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckgroupcl")->GetChecked())
	//	m_iChatMask |= TEAM_CHANNEL_MASK;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckguildcl")->GetChecked())
	//	m_iChatMask |= UNION_CHANNEL_MASK;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckcountrycl")->GetChecked())
	//	m_iChatMask |= BATTLE_CHANNEL_MASK;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckallcl")->GetChecked())
	//	m_iChatMask |= WORLD_CHANNEL_MASK;
	//if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckpersoncl")->GetChecked())
	//	m_iChatMask |= PERSONAL_CHANNEL_MASK;
	//RefreshChatWidget();
	unguard;
}
CEditChat CChat::GetEditChat()
{
	guard;
	return mEdit;
	unguard;
}
int* CChat::GetChatMask()
{
	guard;
	return m_iChatMask;
	unguard;
}

list<CHATCONTENT*> &CChat::GetChatContent()
{
	guard;
	return m_vocChatContent;
	unguard;
}
void CChat::OnChannelListSelect(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	int iSelect = m_plisChannelList->GetSelectedItem();
	if (iSelect == 0)
	{
		return;
	}
	m_pbtnChannelSelect->SetText(m_plisChannelList->GetItemText(iSelect,0));
	m_eNowChannel = (EChannels)(iSelect-1);
	m_plisChannelList->Hide();
	g_workspace.SetFocusWidget(mpChatInput);
	unguard;
}
void CChat::OnChannelListLoseFocus(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	m_plisChannelList->Hide();
	unguard;
}
void CChat::OnSystemBtnDown(int key)
{
	guard;
	int iSelect = m_plisChannelList->GetSelectedItem();
	if(iSelect> m_plisChannelList->GetItemCount() - 1 || iSelect < 0)
		iSelect = 0;
	switch(key)
	{
	case VK_PRIOR:
		if(iSelect > 0)
			iSelect--;
		break;
	case VK_NEXT:
		if(iSelect < m_plisChannelList->GetItemCount() - 1)
			iSelect++;
		break;
	}
	m_plisChannelList->SetSelect(iSelect);
	OnChannelListSelect(NULL,NULL);
	unguard;
}
void CChat::SetChatBackGround(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
// 	LOAD_UI("fmfeedback.scrolldialg")->Show();
// 	m_bNeedChangeBackground = true;
	unguard;
}

void CChat::SetNeedChangeBackground(bool need)
{
	guard;

	m_bNeedChangeBackground = need;

	int iSelect= m_tabChat->GetSelectedItem();
	if (need/* && *//**/)
	{
		m_chatScroll[iSelect]->Show();
		LOAD_UI("fmdialg")->Show();
		char tmpStr[100];
		for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
		{
			m_btnChatSelect[i]->Show();
			rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.back",i);
			LOAD_UI(tmpStr)->Show();
		}
		///.show
	}
	unguard;
}

void CChat::DoFrame()
{
	guard;
	static DWORD color = 0x01295640;
	int iSelect= m_tabChat->GetSelectedItem();
	if(m_bNeedChangeBackground)
	{
		if(color < 0x7f295640)
			color += 0x02000000;
	}else
	{
		if(color > 0x01295640)
			color -= 0x02000000;
		else
		{
			char tmpStr[100];
			m_chatScroll[iSelect]->Hide();
			for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
			{
				m_btnChatSelect[i]->Hide();
				rt2_snprintf(tmpStr,100,"tbfeedback.fmchat%d.back",i);
				LOAD_UI(tmpStr)->Hide();
			}

			//焦点不是输入框并且输入框不为空
			if (g_workspace.getFocusWidget()!=mpChatInput && mpChatInput->getText().empty())
			{
				LOAD_UI("fmdialg")->Hide();
				/*if(m_faceform->IsVisible())
					m_faceform->Hide();*/
			}
			else
			{
				LOAD_UI("fmdialg")->Show();
			}

			/*if(LOAD_UI(UI_CHANNEL_WHOLE_ID)->GetFlags(wfVisible))
			{
				LOAD_UI(UI_CHANNEL_WHOLE_ID)->Hide();
			}*/
		}
	}

	m_tabChat->SetBackgroundColor(color);

	if(LOAD_UI("fmdiy")->IsVisible())
	{
		if(g_layerMain->GetFocusParentWidget(g_workspace.GetFocusWidget())!= LOAD_UI("fmdiy"))
			LOAD_UI("fmdiy")->Hide();
	}

	if(LOAD_UI(UI_CHANNEL_WHOLE_ID)->IsVisible())
	{
		if(g_layerMain->GetFocusParentWidget(g_workspace.GetFocusWidget())!= LOAD_UI(UI_CHANNEL_WHOLE_ID))
			LOAD_UI(UI_CHANNEL_WHOLE_ID)->Hide();
	}
	unguard;
}

void CChat::ClearChatBanckGround()
{
	guard;
	int iSelect= m_tabChat->GetSelectedItem();
	m_tabChat->SetBackgroundColor(0x01000000);
	m_chatScroll[iSelect]->Hide();
	unguard;
}

void CChat::RefreshChatWidget()
{
	guard;
	//g_layerMain->m_chatFeedback->RemoveAllItems();
	//list<CHATCONTENT*>::iterator it;
	//for(it = m_vocChatContent.begin();it != m_vocChatContent.end();++it)
	//{
	//	if((m_iChatMask & ((*it)->channel)) == (*it)->channel)
	//	{
	//		g_layerMain->m_chatFeedback->Add((*it)->channel,(*it)->Color,
	//			(*it)->Description,(*it)->name,(*it)->content,(*it)->FromRegionID);
	//	}
	//}
	unguard;
}

//void CChat::AddReceiveChannel(EChanneLists vList,EChannels vChannel)
//{
//    guard;
//	std::vector<EChannels>::iterator It;
//	for(It=mList[vList].begin();It!=mList[vList].end();It++)
//	{
//		if((*It)==vChannel)
//		{
//			break;
//		}
//	}
//
//	if(It==mList[vList].end())
//	{
//		mList[vList].push_back(vChannel);
//	}
//
//	return;
//    unguard;
//}

//void CChat::DelReceiveChannel(EChanneLists vList,EChannels vChannel)
//{
//    guard;
//	for(std::vector<EChannels>::iterator It=mList[vList].begin();It!=mList[vList].end();It++)
//	{
//		if((*It)==vChannel)
//		{
//			mList[vList].erase(It);
//			break;
//		}
//	}
//	return;
//    unguard;
//}

SChannelAtom* CChat::GetChannel(EChannels vChannel)
{
    guard;
	std::vector<SChannelAtom>::iterator It;
	for(It=mChannel.begin();It!=mChannel.end();++It)
	{
		if((*It).Is(vChannel))
		{
			break;
		}
	}

	if(It==mChannel.end())
	{//没有找到
		return NULL;
	}
	else
	{//找到
		return (&(*It));
	}
    unguard;
}

SChannelAtom* CChat::GetChannel(char vChannel)
{
    guard;
	std::vector<SChannelAtom>::iterator It;
	for(It=mChannel.begin();It!=mChannel.end();++It)
	{
		if((*It).Is(vChannel))
		{
			break;
		}
	}

	if(It==mChannel.end())
	{//没有找到
		return NULL;
	}
	else
	{//找到
		return (&(*It));
	}
    unguard;
}

void CChat::UserInterfaceKeyEvent(ui::RtwWidget* sender, RtwEventDelegate* e)
{	
    guard;
	switch(e->type)
	{
	case etKeyDown:
		mEdit.KeyDownEvent(e->key.code);
		break;

	case etKeyChar:
		mEdit.KeyCharDownEvent();
		break;
	}
    unguard;
}

bool CChat::CheckChatFrequency(EChannels channel)//true表示没有太快， false表示太快
{
	/* gao 2010.1.4
	原聊天检查更改
	去除原禁言惩罚，改为当时间小于最小发言间隔时，无法发言
	*/
	guard;
	if(channel == PERSONAL_CHANNEL || channel == TEAM_CHANNEL || channel == WORLD_CHANNEL)
	{
		return true;
	}else if(channel == NEAR_CHANNEL || channel == UNION_CHANNEL || channel == BATTLE_CHANNEL)
	{
		int i = (int)channel;
		//防止刷屏的检查
		DWORD NowTime=timeGetTime();
		// 用time【i】来记录对应的聊天频道的发言时间，以便用来计算是否可以再次发言
		if(NowTime-Time[i] < CHECK_TIME)
		{//说话的速度太快
			SystemMessage(R(LIMIT_MESSAGE),true);
			return false;
		}else
		{
			Time[i] = NowTime;
			return true;
		}
	}else if(channel == SPEAKER_CHANNEL)
	{
		int i = (int)channel;
		//防止刷屏的检查
		DWORD NowTime=timeGetTime();
		// 用time【i】来记录对应的聊天频道的发言时间，以便用来计算是否可以再次发言
		if(NowTime-Time[i] < SPEAKER_CHECK_TIME)
		{//说话的速度太快
			SystemMessage(R(LIMIT_MESSAGE),true);
			return false;
		}else
		{
			Time[i] = NowTime;
			return true;
		}
	}
	return true;
	/* end */
	unguard;
}


void CChat::Send(char vMessage[])												//发消息//解析，并发送
{
    guard;
	std::string smessage;
	smessage=vMessage;
	mEdit.Setlastindex(-1);
	if (smessage!="")
		mEdit.mTextlist.push_back(smessage);
	if (mEdit.mTextlist.size()>10)
	{
		mEdit.mTextlist.pop_front();
	}
	//编辑部分处理
	mEdit.Send(vMessage);
	return;
    unguard;
}



void CChat::AddBlackList(const string& vrName)
{
	/*list<string>& rList=mBlackList.GetList();
	if(rList.size()<MAX_BLACKLIST_COUNT)
	{*/
		/*mBlackList.Add(vrName);
		GetWorld()->m_Friend.GetDBID(GetWorld()->m_Friend.mConnection)
		GetWorld()->m_Friend.DataChange();*/
	GetWorld()->m_Friend.AddOtherToBlackList(std::string(vrName));
	//}
	//else
	//{//人数满，告诉用户不能邀请
	//	SystemMessage(R(BLACK_LIST_OVERFLOW),true);
	//}
};

//void CChat::Load()
//{
//	if(GetWorld()==NULL)
//	{
//		return;
//	}
//	if(GetWorld()->m_pPlayer==NULL)
//	{
//		return;
//	}
//
//	const char* pUserName=GetWorld()->m_pPlayer->GetName();
//	if(pUserName==NULL)
//	{
//		return;
//	}
//	if(pUserName[0]==0)
//	{
//		return;
//	}

// 注释 [4/14/2009 tooth.shi]
/*	RtMD5 NameMD5;
	string NameMD5String;
	NameMD5.EncodeString(const_cast<char*>(pUserName));
	NameMD5.GetMD5String(const_cast<char*>(NameMD5String.c_str()));
// 	NameMD5<<pUserName;
// 	NameMD5String<<NameMD5;	// change [3/16/2009 tooth.shi]
	string FullName=LOCAL_BLACKLIST_DIRECTORY;
	FullName=FullName+'\\'+LOCAL_BLACKLIST_PREFIX+NameMD5String;

	//保证文件的打开
	CAnsiFile LoadFile;
	if(!LoadFile.Open(FullName,"rb+"))
	{//无法打开
		if(LoadFile.Open(FullName,"rb"))
		{//只读原因
			LoadFile.Close();
			CAnsiFile::ChangeMode(FullName.c_str(),1);//READ_MODE
			if(!LoadFile.Open(FullName,"rb+"))
			{//无法打开文件
				return;
			}
		}
		else
		{//文件不存在
			return;
		}
	}

	//载入

	int Version=0;//当前版本
	

	if(LoadFile.GetBuffer(sizeof(Version),&Version)!=sizeof(Version))
	{
		goto load_blacklist_file_fail;
	}

	

	LoadFile.Seek(0,SEEK_SET);//到头
	LoadFile.PutBuffer(&LOCATION_BLACKLIST_NO_CLOSE,sizeof(LOCATION_BLACKLIST_NO_CLOSE));//表明文件开始读出

	switch(Version) 
	{
	case LOCATION_BLACKLIST_NO_CLOSE://上一次文件没有正常退出//将该文件删除
		goto load_blacklist_file_fail;
		break;
	case 100://版本
		{
			int Count=0;//一共有多少个名字
			if(LoadFile.GetBuffer(sizeof(Count),&Count)!=sizeof(Count))
			{
				goto load_blacklist_file_fail;
			}

			int Size=0;//一共有多少长
			if(LoadFile.GetBuffer(sizeof(Size),&Size)!=sizeof(Size))
			{//有错，//也要把该文件删除
				goto load_blacklist_file_fail;
			}

			if(Size>=1024*1024)
			{//数据太大，异常
				goto load_blacklist_file_fail;
			}

			CLinearityBuffer LoadBuffer;
			LoadBuffer.Resize(Size+1);

			if(LoadFile.GetBuffer(Size,LoadBuffer.Ptr())!=Size)
			{//有错
				goto load_blacklist_file_fail;
			}


			Decrypt(LoadBuffer.Ptr(),Size,NameMD5String);//解密
			LoadBuffer[Size]='\0';//字符串尾
			string AllString(LoadBuffer.Ptr());


			string Temp;
			for(int i=0;i!=Count;++i)
			{//一共有多少个名字
				Temp="";
				AllString>>Temp;
				mBlackList.Add(Temp);
			}
		}
		break;
	default:
		//有错
		goto load_blacklist_file_fail;
		break;
	}

	GetWorld()->m_Friend.DataChange();//界面更新

	LoadFile.Seek(0,SEEK_SET);//到头
	LoadFile.PutBuffer(&Version,sizeof(Version));//文件正常结束
	return;

load_blacklist_file_fail:
	LoadFile.Close();
	CAnsiFile::DeleteFile(FullName);*/
//	return;
//}

//void CChat::Save()
//{
//	RtMD5 NameMD5;
//	string NameMD5String;
//	char szNameMD5String[33];
//	NameMD5.EncodeString(const_cast<char*>(GetWorld()->m_pPlayer->GetName()));
//	NameMD5.GetMD5String(szNameMD5String);
//	szNameMD5String[32] = '\0';
//	NameMD5String = szNameMD5String;
//
//// 	NameMD5<<GetWorld()->m_pPlayer->GetName();
//// 	NameMD5String<<NameMD5;
//	string FullName=LOCAL_BLACKLIST_DIRECTORY;
//	FullName=FullName+'\\'+LOCAL_BLACKLIST_PREFIX+NameMD5String;
//
//
//	CAnsiFile BlackListFile;
//	if(BlackListFile.Open(FullName,"rb"))
//	{//有该文件
//		BlackListFile.Close();
//		if(BlackListFile.Open(FullName,"wb+"))
//		{//可以打开该文件
//		}
//		else
//		{//不能打开
//			CAnsiFile::ChangeMode(FullName.c_str(),1);//改为非只读READ_MODE
//			if(!BlackListFile.Open(FullName,"wb+"))
//			{//无法保存
//				LOG("文件无法保存！\n");
//				return;
//			}
//		}
//	}
//	else
//	{//没有该文件
//		if(!BlackListFile.Create(FullName))
//		{//创建失败
//			return;
//		}
//	}
//
//	//存盘
//
//	//版本号
//	BlackListFile.PutBuffer(&LOCATION_BLACKLIST_NO_CLOSE,sizeof(int));
//	list<string>& rList=mBlackList.GetList();
//	int Count=(int)rList.size();
//	BlackListFile.PutBuffer(&Count,sizeof(int));//有几个黑名单
//
//	string SaveData;
//	list<string>::iterator It;
//	for(It=rList.begin();It!=rList.end();++It)
//	{
//		SaveData<<*It;
//	}
//	int Size=SaveData.size();
//
//	CLinearityBuffer SaveBuffer;
//	SaveBuffer.Resize(Size);
//	memcpy(SaveBuffer.Ptr(),SaveData.c_str(),Size);
//	GSAssistantEncrypt(SaveBuffer.Ptr(),Size,NameMD5String);
//	
//	BlackListFile.PutBuffer(&Size,sizeof(Size));//数据的长度
//	BlackListFile.PutBuffer(SaveBuffer.Ptr(),Size);//数据存入
//
//	BlackListFile.Seek(0,SEEK_SET);//到文件头
//	BlackListFile.PutBuffer(&LAST_LOCATION_BLACKLIST_VERSION,sizeof(int));//版本号
//}

void CChat::SystemMessage(const std::string& vrMessage,bool vIgnoreRepeated)//[string& 信息,bool 是否忽略重复内容]
{
    guard;

	static SChannelAtom* pSystemChannel=GetChannel(SYSTEM_CHANNEL);

	const string& rLastMessage=LOAD_UI_T(RtwChatBox, UI_SYSTEM_FORM_BOX_WHOLE_ID)->GetLast(pSystemChannel->mMask);
	
	if(vIgnoreRepeated)
	{//要求忽略重复的信息
		if(rLastMessage==vrMessage)
		{//重复
			return;
		}
	}

	Receive(CHAT_CHANNEL_SYSTEM,0,"",vrMessage.c_str());
    unguard;
}

void CChat::BattleInfoMessage(const std::string& vrMessage)//[string& 信息,bool 是否忽略重复内容]
{
	Receive(CHAT_CHANNEL_BATTLEINFO,0,"",vrMessage.c_str());
}

void CChat::Receive(char vNetChannel,long vID,char vName[],const char vMessage[])		//收到消息//解析并显示
{
    guard;
	if(mBlackList.Filter(string(vName)))
	{//黑名单
		return;
	}

	SChannelAtom* pChannel=GetChannel(vNetChannel);
	if(pChannel!=NULL)
	{
		mEdit.Receive(pChannel,vID,vName,(char*)vMessage);
	}
    unguard;
}

void CChat::OnAutoScroll_Feedback(RtwWidget* sender, void*)
{
	guard;

	int iSelect= m_tabChat->GetSelectedItem();
	m_chatScroll[iSelect]->SetMaximum(m_chatFeedback[iSelect]->GetScrollRange());
	m_chatScroll[iSelect]->SetValue(m_chatFeedback[iSelect]->GetScrollPosition());

	unguard;
}

void CChat::OnClick_Rfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	RtwChatBox::SItem* itemname =((RtwChatBox*)pWidget)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	if(!itemname || !itemname->id)
	{
		m_popupMenu->Hide();
		return;
	}
	if (itemname)
	{
		if (itemname->type==RtwChatBox::eit_Name)
		{
			if (itemname->text.substr(0,(int)itemname->text.size()-2)==GetWorld()->m_pPlayer->GetName())
			{
				return;
			}
			if (m_popupMenu)
			{	
				m_popupMenu->SetText(itemname->text.substr(0,itemname->text.size()-2));
				m_popupMenu->SetID(itemname->id);		//heten
				m_popupMenu->Popup(pEvent->mouse.x,pEvent->mouse.y);
				m_popupMenu->Show();
				g_workspace.BringWidgetToTop(m_popupMenu);
				g_workspace.BringWidgetToBottom(m_tabChat);
				g_workspace.SetDefaultPopupMenu(m_popupMenu);//max
				g_workspace.SetFocusWidget(m_popupMenu);
			}
		}else
		{
			m_popupMenu->Hide();
		}
	}else
	{
		m_popupMenu->Hide();
	}
	unguard;

	//LOAD_UI("RuserMenu")->Hide();
}

void CChat::OnClick_Lfunction(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	RtwChatBox::SItem* itemname =((RtwChatBox*)pWidget)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));

	if (!itemname)
	{
		return;
	}

	//添加超级链接功能
	if (itemname->hyperLink.Text != "")
	{
		g_layerMain->OnHtmlAction_HtmlHelpView(pWidget, pEvent);
		return;
	}

	if (!itemname->id)
	{
		return ;
	}

	if (itemname)
	{
		if (itemname->type==RtwChatBox::eit_Name)
		{
			std::string strmyname;
			if (LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->getText()!="")
			{
				strmyname+=(itemname->text.substr(0,(int)itemname->text.size()-2))+" "+strtext;
				LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->SetText(strmyname);
			}else if(LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->getText()==""){
				strtext="";
				std::string strname;
				strname+=(itemname->text.substr(0,(int)itemname->text.size()-2))+" ";
				LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->SetText(strname);
			}
			GetWorld()->m_Chat.getmEdit().GetmDoskey().SetEditText(LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->getText());
			GetWorld()->m_Chat.OnMemberChoice(0,LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->getText().c_str());

			g_workspace.SetFocusWidget(LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID));
		}
	}
	unguard;
}

void CChat::OnClick_Movefunction(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	if (m_popupMenu->IsVisible())
	{
		g_workspace.SetFocusWidget(m_popupMenu);
	}else
	{
		g_workspace.SetFocusWidget(pWidget);
	}
	GetWorld()->m_Chat.SetNeedChangeBackground(true);

	// 	RtwChatBox::SItem* itemname =((RtwChatBox*)pWidget)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	// 	if (itemtext==itemname)
	// 	{
	// 		return;
	// 	}else if (itemtext)
	// 	{
	// 		itemtext->bDraw=false;
	// 	}
	// 	if (itemname&&itemname->rect.top>=m_chatFeedback->GetFrameRect().top)
	// 	{
	// 		itemname->bDraw=true;
	// 	}
	// 	if (itemname)
	// 	{
	// 		itemtext=itemname;
	// 	}
	unguard;
}

void CChat::OnMouseWheel(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	guard;
	int iSelect= m_tabChat->GetSelectedItem();
	if( pEvent )
	{
		m_WheelValue=m_chatScroll[iSelect]->GetValue();
		if( pEvent->mouseWheel.delta > 0 )
		{
			if (m_WheelValue > m_WheelMin)
				m_WheelValue -=5;
			if (m_WheelValue<0)
			{
				m_WheelValue=0;
			}
		}else
		{
			if (m_WheelValue < m_WheelMax)
				m_WheelValue +=5;
			if (m_WheelValue>100)
			{
				m_WheelValue=100;
			}
		}
	}
	m_chatScroll[iSelect]->SetValue(m_WheelValue);
	m_chatFeedback[iSelect]->SetScrollPosition(m_WheelValue);
	unguard;
}

void CChat::OnScroll_FocusWidget(RtwWidget* sender, void*)
{
	guard;
	g_workspace.SetFocusWidget(sender);
	unguard;
}

void CChat::OnScroll_ManualScroll(RtwWidget* sender, void*)
{
	guard;
	int iSelect= m_tabChat->GetSelectedItem();
	m_chatFeedback[iSelect]->SetScrollPosition(m_chatScroll[iSelect]->GetValue());
	unguard;
}

void CChat::OnMenuRUser(RtwWidget* sender, RtwEventDelegate* e)
{
	int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
	int iData = (int)((RtwPopupMenu*)sender)->GetItemData(iSelectItem);
	switch (iData)
	{
	case 1: // 密语, 把对方的名字复制到密语那里
		GetWorld()->m_Chat.OnMemberChoice(0,m_popupMenu->getText().c_str());
		break;
	case 2: // 复制玩家的名字
		g_workspace.getClipboard()->Copy(m_popupMenu->getText());
		break;
	case 3: //heten  添加好友
		{
			/*CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_subsystem_mail);
			pPacket->WriteByte(C2R_INVITE_OTHER_FRIEND_BYNAME);
			pPacket->WriteLong(0);
			pPacket->WriteString(m_popupMenu->getText().c_str());
			NetSend(pPacket);*/
			GetWorld()->m_Friend.InviteOther(m_popupMenu->getText());
		}
		break;
	case 4: // 添加黑名单
		GetWorld()->m_Friend.AddOtherToBlackList(m_popupMenu->getText());
		break;
	case 5:
		//CG_CmdPacket* pPacket = NetBeginWrite();
		//pPacket->WriteShort(c2r_subsystem_team);
		//pPacket->WriteByte(C2R_INVITE_OTHER_BY_NAME);
		//pPacket->WriteString(m_popupMenu->getText().c_str()); // 服务器上有检查长度
		//NetSend(pPacket);
		GetWorld()->m_Team.InviteOther(m_popupMenu->getText());
		break;
	}
}

void CChat::UserInterfaceSelect(ui::RtwWidget* vpSender,void*)
{
	guard;
	//由编辑部分处理
	const string& Sender=vpSender->GetQualifiedID();
	if(Sender==UI_DOSKEY_LIST_WHOLE_ID)
	{
		mEdit.DoskeySelect();
	}
	else if(Sender==UI_CHAT_FACE_ICON_LIST_WHOLE_ID)
	{
		const char* pKey;
		pKey=LOAD_UI_T(RtwIconList,UI_CHAT_FACE_ICON_LIST_WHOLE_ID)->GetSelectedKey();
		//LOAD_UI_T(RtwChatInputBox,UI_INPUT_WHOLE_ID)->InsertImage(pKey);
		LOAD_UI(UI_CHAT_FACE_FORM_WHOLE_ID)->Hide();//将窗口关闭
		mEdit.Change();
	}
	//else if(Sender==UI_CHAT_FORM_WHOLE_ID)
	//{
	//	RtwChatBox::Item* pItem=LOAD_UI_T(RtwChatBox,UI_CHAT_FORM_WHOLE_ID)->GetSelectItem();
	//	if(pItem!=NULL)
	//	{

	//		RtwChatBox::Piece*	pPiece=LOAD_UI_T(RtwChatBox,UI_CHAT_FORM_WHOLE_ID)->GetSelectPiece();
	//		if(pPiece!=NULL)
	//		{
	//			switch(pPiece->type)
	//			{

	//			case RtwChatBox::ptPrefix:
	//			case RtwChatBox::ptText:
	//			case RtwChatBox::ptImage:
	//			case RtwChatBox::ptSeparator:
	//			case RtwChatBox::ptBackslash:
	//				mEdit.AddText(pItem->text);		
	//				break;
	//			case RtwChatBox::ptName:
	//				{
	//					if(pItem->sender!=GetWorld()->m_pPlayer->GetName())
	//					{
	//						mEdit.ReplacePrivateName((std::string)pItem->sender);
	//					}
	//				}
	//				break;
	//			default:
	//				break;
	//			}
	//		}
	//	}
	//}
	unguard;
}

void CChat::OnRClick_btnChatSelect(RtwWidget* sender, void*)
{
	guard;
	//for(int i= 0;i< MAX_CHANNEL_BUTTON;i++)
	//{
	//	if(sender== m_btnChatSelect[i])
	//		m_tabChat->SetSelectedItem(i);
	//}
	LOAD_UI("fmdiy")->Move(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
	LOAD_UI("fmdiy")->Show();
	m_plstDIY->Show();
	g_workspace.SetFocusWidget(LOAD_UI("fmdiy"));
	unguard;
}

void CChat::OnListDIY_Select(RtwWidget* sender, void*)
{
	guard;
	int iSelect= m_plstDIY->GetSelectedItem();
	if(iSelect== 0)
	{
		LOAD_UI(UI_CHANNEL_WHOLE_ID)->Show();
		RtwRect rect= LOAD_UI("fmdiy")->GetFrameRect();
		LOAD_UI(UI_CHANNEL_WHOLE_ID)->Move(rect.left,rect.top);
		g_workspace.BringWidgetToBottom(m_tabChat);
		g_workspace.SetFocusWidget(LOAD_UI(UI_CHANNEL_WHOLE_ID));
		int jSelect= m_tabChat->GetSelectedItem();
		OnSetMaskInit(m_iChatMask[jSelect]);
	}else if(iSelect== 1)
	{
		UIFormTextMsg *pFrm;
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_CHAT_ENTERTHECHANNELNAME), R(MSG_CHAT_ENTERTHECHANNELNAME));
		pFrm->SetTxtLength(4);
		pFrm->EvOK = RTW_CALLBACK_1(this, CChat, OnChangeChatName, pFrm);
	}
	LOAD_UI("fmdiy")->Hide();
	unguard;
}

void CChat::OnSetMaskInit(int& mask)
{
	guard;
	if(mask& NEAR_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.cknormalcl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.cknormalcl")->SetChecked(false);

	if(mask& TEAM_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckgroupcl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckgroupcl")->SetChecked(false);

	if(mask& UNION_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckguildcl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckguildcl")->SetChecked(false);

	if(mask& BATTLE_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckcountrycl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckcountrycl")->SetChecked(false);

	if(mask& WORLD_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckallcl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckallcl")->SetChecked(false);

	if(mask& PERSONAL_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckpersoncl")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckpersoncl")->SetChecked(false);

	if(mask& BATTLEINFO_CHANNEL_MASK)
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckbattleinfo")->SetChecked(true);
	else
		LOAD_UI_T(CUiCheckButton,"fmchannel.ckbattleinfo")->SetChecked(false);
	SaveChatSet();
	unguard;
}

void CChat::OnGetMaskValue(int& mask)
{
	guard;
	mask= 0;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.cknormalcl")->GetChecked())
		mask |= NEAR_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckgroupcl")->GetChecked())
		mask |= TEAM_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckguildcl")->GetChecked())
		mask |= UNION_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckcountrycl")->GetChecked())
		mask |= BATTLE_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckallcl")->GetChecked())
		mask |= WORLD_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckpersoncl")->GetChecked())
		mask |= PERSONAL_CHANNEL_MASK;
	if(LOAD_UI_T(CUiCheckButton,"fmchannel.ckbattleinfo")->GetChecked())
		mask |= BATTLEINFO_CHANNEL_MASK;
	unguard;
}

void CChat::OnChangeChatName(RtwWidget* sender, RtwEventDelegate*vpEvent)
{
	guard;
	UIFormTextMsg *pFrm = (UIFormTextMsg *)vpEvent->GetUser1();
	RtString str = pFrm->GetString();
	int jSelect= m_tabChat->GetSelectedItem();
	m_btnChatSelect[jSelect]->SetText(str.c_str());
	SaveChatSet();
	unguard;
}

void CChat::SaveChatSet()
{
	guard;
	FILE* file;
	int mask;
	char name[100];
	rt2_snprintf(name,99,"usersetting\\%s.chatset",GetPlayer()->m_core.Name.c_str());
	file = fopen( name, "wb+" );

	for (int index = 0; index < MAX_CHANNEL_CHATBOX; index++)
	{
		mask= m_iChatMask[index];
		rt2_strncpy(name,m_btnChatSelect[index]->getText().c_str(),99);
		fwrite(&mask,1,sizeof(int),file);
		int num= strlen(name)+1;
		fwrite(&num,1,sizeof(int),file);
		fwrite(&name,1,num,file);
	}
	fclose(file);
	unguard;
}

void CChat::LoadChatSet()
{
	guard;
	FILE* file;
	int mask;
	char name[100];
	rt2_snprintf(name,99,"usersetting\\%s.chatset",GetPlayer()->m_core.Name.c_str());
	file = fopen( name, "rb+" );
	if(!file)
		return;

	for (int index = 0; index < MAX_CHANNEL_CHATBOX; index++)
	{
		fread(&mask,1,sizeof(int),file);
		m_iChatMask[index]= mask;
		int num= 0;
		fread(&num,1,sizeof(int),file);
		fread(&name,1,num,file);
		name[4]= 0;
		m_btnChatSelect[index]->SetText(name);
	}
	fclose(file);
	unguard;
}

void CChat::OnLoseFocus(RtwWidget* sender, void*)
{
	guard;
	sender->Hide();
	unguard;
}

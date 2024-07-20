#include "gc_include.h"
#include "gc_mail_const.h"
#include <sstream>
#include "gc_mail_with_ui.h"
#include "ui_form_playeritems.h"
#include "ui_form_msg.h"


const DWORD UI_ALREADY_OPEN_COLOR=0xff8f8f8f;

const char UI_MAIL_WHOLE_ID[]="fmmail2";
const char UI_MAIL_LIST_WHOLE_ID[]="fmmail2.fmcontent.lbxtext";
const char UI_MAIL_PREVIEW_WHOLE_ID[]="fmmail1.fmcontent.fmtext.ebtext";
const char UI_MAIL_PREVIEW_TITLE_WHOLE_ID[]="fmmail1.fmcontent.lbpeople2";

const char UI_MAILBOOK_WHOLE_ID[]="fmmail4";

const char UI_SEND_TO_NAME_WHOLE_ID[]="fmmail4.fmcontent.ebpeople1";
const char UI_MAIL_TITLE_WHOLE_ID[]="fmmail4.fmcontent.ebpeople2";
const char UI_MAIL_CONTENT_WHOLE_ID[]="fmmail4.fmcontent.fmtext.ebtext";
const char UI_RETURN_MAIL_BOX_WHOLE_ID[]="fmmail4.fmcontent.btnback";

//const char UI_MAIL_INFO_WHOLE_ID[]="fmfriendlist.lbinfobox";


const char UI_MAIL_FACE_SWITCH_WHOLE_ID[]="fmmail4.fmcontent.Expression";			//开聊天框的开关
const char UI_MAIL_FACE_SWITCH_ID[]="Expression";									//开聊天框的开关
const char UI_MAIL_FACE_FORM_WHOLE_ID[]="fmmail4.fmchatface";						//聊天表情框
const char UI_MAIL_FACE_FORM_ID[]="fmchatface";										//聊天表情框
const char UI_MAIL_FACE_ICON_LIST_WHOLE_ID[]="fmmail4.fmchatface.iconlist";			//聊天表情框中的图标列表框
const char UI_MAIL_FACE_ICON_LIST_ID[]="iconlist";									//聊天表情框中的图标列表框




const char *UI_FW_FLAG;//[]="Fw:";
const char *UI_RE_FLAG;//[]="Re:";

const char *UI_FRIEND_MAIL_INFO_FLAG1;//[]=收件箱(
const char *UI_FRIEND_MAIL_INFO_FLAG2;//[]=封)
const char *UI_FRIEND_MAIL_INFO_NULL;//[]=空)





CMailWithUI::CMailWithUI()
:mTitleMap(),
mAuthorMap(),
mTimeMap(),
mpCurrentReceiveList(NULL),		
mpSelectMail(NULL),

mpPreviewUI(NULL),
mpPreviewTitleUI(NULL),
mpDisplayMail(NULL),
mpInfoAtFriendListUI(NULL),
mpLastFocus(NULL),		//最后一次的输入焦点窗口
mpMailUI(NULL),			//收件箱
mpReceiveListUI(NULL),	//收件箱上的邮件列表

mpMailBookUI(NULL),		//发邮件界面
mpSendToNameUI(NULL),
mpTitleUI(NULL),
mpContentUI(NULL),
mpChatFaceUI(NULL)
{
}


CMailWithUI::~CMailWithUI()
{
	//Clear();
}

void CMailWithUI::Reset()
{
    guard;

	//程序的部分处理
	//界面的隐藏
	mpMailUI->Hide();
	mpMailBookUI->Hide();
	mpChatFaceUI->Hide();



	mpCurrentReceiveList=&mTimeMap;//当前排序列表[默认是按时间排序]
	mpSelectMail=NULL;
	UpdateInfoAtFriendList();
	//Load();
	//GetWorld()->m_Chat.Load();
    unguard;
}



void CMailWithUI::Init()
{
    guard;
	//常量初始化

	UI_FW_FLAG=R(UI_FW_FLAG);//[]="Fw:";
	UI_RE_FLAG=R(UI_RE_FLAG);//[]="Re:";

	UI_FRIEND_MAIL_INFO_FLAG1=R(UI_FRIEND_MAIL_INFO_FLAG1);//[]=收件箱(
	UI_FRIEND_MAIL_INFO_FLAG2=R(UI_FRIEND_MAIL_INFO_FLAG2);//[]=封)
	UI_FRIEND_MAIL_INFO_NULL=R(UI_FRIEND_MAIL_INFO_NULL);//[]=空)

	//事件的挂接


	
	//点击系统的收件箱
	RTW_WIDGET("fmsystem.btnsysmail")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, OpenMailUI);

	//[新建,回复,抄送]开收发邮件界面，删除邮件，导出邮件

	RTW_WIDGET("fmmail2.fmcontent.btnxmm1")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, NewMail);
	RTW_WIDGET("fmmail1.fmcontent.btnreplay")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, ReplayMail);
	RTW_WIDGET("fmmail1.fmcontent.btncc")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, CCMail);
	RTW_WIDGET("fmmail1.fmcontent.btnview")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, OnReceiveItemAndMoney);
	RTW_WIDGET("fmmail1.fmcontent.btnback")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, ReturnToMailBox);
	RTW_WIDGET("fmmail2.fmcontent.btnxmm5")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, OnEnsureDeleteMail);
	RTW_WIDGET("fmmail2.fmcontent.btnxmm2")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, ExportMail);

	
	//收发界面的发邮件按钮
	RTW_WIDGET("fmmail4.fmcontent.btncc")->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, SendMail);

	RTW_WIDGET(UI_RETURN_MAIL_BOX_WHOLE_ID)->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, ReturnToMailBox);

	//表情框的开关
	RTW_WIDGET(UI_MAIL_FACE_SWITCH_WHOLE_ID)->EvLClick += 
		RTW_CALLBACK(this, CMailWithUI, ChangeFaceUI);

	//表情框的激活事件
	RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->EvActivate+=
		RTW_CALLBACK(this,CMailWithUI,ActiveFaceForm);

	//表情框中列表的选择事件
// 	RTW_WIDGET_T(RtwIconList,UI_MAIL_FACE_ICON_LIST_WHOLE_ID)->EvSelect+=
// 		RTW_CALLBACK(this,CMailWithUI,UserInterfaceSelect); tooth.shi


	//得到指针
	mpMailUI=RTW_WIDGET(UI_MAIL_WHOLE_ID);
// 	mpMailUI->SetWidgetEffect(esTrans, esTrans);
	//mpMailUI->EvKey+=RTW_CALLBACK(mpMailUI,RtwWidget,DefaultKeyDown);

	mpReceiveListUI=RTW_WIDGET_T(RtwListBox,UI_MAIL_LIST_WHOLE_ID);
	mpPreviewUI=RTW_WIDGET_T(RtwTextBox,UI_MAIL_PREVIEW_WHOLE_ID);
	/* gao 2009.11.24
	此处，如果设置为不接受输入，那么整个程序将无法接受任何的输入，
	我们用下面的的SetEnableInput(false)来带代替此行代码。
	*/
	//mpPreviewUI->ModifyFlags(0,wfInput);
	/* gao 2009.11.23
	标题无法显示的问题*/
	mpPreviewFormUI		= RTW_WIDGET_T(RtwLabel,UI_MAIL_PREVIEW_TITLE_WHOLE_ID);
	mpPreviewAuthorUI	= RTW_WIDGET_T(RtwLabel,"fmmail1.fmcontent.lbpeople1");
	mpPreviewTitleUI	= RTW_WIDGET_T(RtwLabel,"fmmail1.fmcontent.lbpeople2");
	mpPreviewTimeUI		= RTW_WIDGET_T(RtwLabel,"fmmail1.fmcontent.lbpeople3");
	m_pSendButton		= RTW_WIDGET_T(RtwButton,"fmmail4.fmcontent.btnxmm");
	m_pReceiveButton	= RTW_WIDGET_T(RtwButton,"fmmail1.fmcontent.btnxmm");
	m_pReceiveButton->ModifyFlags(wfGrabMouse | wfMouseMove | wfHint, 0);
	m_pReceiveButton->EvHint		+= RTW_CALLBACK(this, CMailWithUI, OnGetHintReceiveItem);
	m_pReceiveMoney		= RTW_WIDGET_T(RtwLabel,"fmmail1.fmcontent.lbtext9");
	m_pSendButton->ModifyFlags(wfDragClient | wfDragServer|wfDragIn|wfDragOut, 0);
	m_pSendButton->EvDrag		+= RTW_CALLBACK(this, CMailWithUI, OnDragToSendItem);
	m_pSendButton->EvDragEnd	+= RTW_CALLBACK(this, CMailWithUI, OnDragEndSendItem);
	m_pSendButton->EvHint		+= RTW_CALLBACK(this, CMailWithUI, OnGetHintItemText);
	m_pSendButton->EvMouseRDown += RTW_CALLBACK(this, CMailWithUI, OnSendButtonMouseButtonDown);
	m_pReceiveButton->EvDrag	+= RTW_CALLBACK(this, CMailWithUI, OnDragToSendItem);
	m_pSendMoney				= RTW_WIDGET_T(RtwTextBox,"fmmail4.fmcontent.ebx1");
	m_pSendMoney->EvInputChar	+= RTW_CALLBACK(this, CMailWithUI, OnSendMoneyChanged);
	m_pSendMoney->SetNumberOnly(true);
	/* end */
	
	mpDisplayMail=NULL;

	mpMailBookUI=RTW_WIDGET(UI_MAILBOOK_WHOLE_ID);
	mpMailBookUI->EvHide		+= RTW_CALLBACK(this,CMailWithUI, OnHideSendMail);
	//mpMailBookUI->EvKey+=RTW_CALLBACK(mpMailBookUI,RtwWidget,DefaultKeyDown);

	mpSendToNameUI=RTW_WIDGET(UI_SEND_TO_NAME_WHOLE_ID);
	mpTitleUI=RTW_WIDGET(UI_MAIL_TITLE_WHOLE_ID);
	mpContentUI=RTW_WIDGET_T(RtwTextBox,UI_MAIL_CONTENT_WHOLE_ID);
	mpChatFaceUI=RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID);
	//mpInfoAtFriendListUI=RTW_WIDGET(UI_MAIL_INFO_WHOLE_ID);

	/* gao 2009.11.24
	此处功能是可以让用邮件显示多行，包括输入邮件，显示邮件
	*/
	mpContentUI->SetMultiLine(true);
	mpContentUI->SetWordWrap(true);
	mpContentUI->SetScrollBarV(true);
	mpContentUI->SetAutoScrollV(true);

	mpPreviewUI->SetWordWrap(true);
	mpPreviewUI->SetMultiLine(true);
	mpPreviewUI->SetScrollBarV(true);
	mpPreviewUI->SetEnableInput(false);
	mpPreviewUI->SetAutoScrollV(true);
	/* end */
	
	//单击
	mpReceiveListUI->GetColumnHeader(0)->EvLClick+=RTW_CALLBACK(this,CMailWithUI,UserClick);//列表的列被点击事件
	mpReceiveListUI->GetColumnHeader(1)->EvLClick+=RTW_CALLBACK(this,CMailWithUI,UserClick);//列表的列被点击事件
	mpReceiveListUI->GetColumnHeader(2)->EvLClick+=RTW_CALLBACK(this,CMailWithUI,UserClick);//列表的列被点击事件

	//打开收件列表的hint事件[因为，内容太长，只能用hint来显示所有的内容]
	mpReceiveListUI->ModifyFlags(wfHint,0);
	/* gao 2009.11.20
	说明，这里是替换掉了原来的EvHint事件，因为原EvHint只能接受非子控件消息
	*/
	mpReceiveListUI->SetDrawScrollBarV(true);
	mpReceiveListUI->EvHintItem		+=RTW_CALLBACK(this,CMailWithUI,UserHint);
	mpReceiveListUI->EvLDClick		+=RTW_CALLBACK(this,CMailWithUI,OnLDClickMailList);
	/* end */

	/* gao 2009.11.23
	此处，mpPreviewTitleUI 改为 mpPreviewFormUI*/
	//打开预览邮件标题的hint事件[因为，内容可能太长，要用hint来显示所有的内容]
	mpPreviewFormUI->ModifyFlags(wfHint|wfGrabMouse,0);
	mpPreviewFormUI->EvHint+=RTW_CALLBACK(this,CMailWithUI,UserHint);
	/* end */

	mpMailBookUI->EvActivate+=RTW_CALLBACK(this,CMailWithUI,MailBookUIActive);
	mpMailBookUI->EvHint+=RTW_CALLBACK(this,CMailWithUI,MailBookUIHide);
	/* gao 2010.2.2
	查看游戏邮件界面
	*/
	m_pfrmDisplayMail = RTW_WIDGET("fmmail1");
	m_pfrmDisplayMail->EvHint		+= RTW_CALLBACK(this,CMailWithUI,OnHideSendMail);
	RTW_WIDGET("fmmail2.fmcontent.btnxmm3")->EvLClick	+= RTW_CALLBACK(this, CMailWithUI, UserInterfaceEvent);
	RTW_WIDGET("fmmail1.fmcontent.btnback")->EvLClick	+= RTW_CALLBACK(this, CMailWithUI, UserInterfaceEvent);
	RTW_WIDGET("fmmail2.fmcontent.btnxmm4")->EvLClick	+= RTW_CALLBACK(this, CMailWithUI, UserInterfaceEvent);
	Initface();
	m_imgAlreadyOpen = g_workspace.getImageFactory()->createImage("ui_texture/ReadedMessage.tga");
	m_imgAlreadyOpen->SetBlend(true);
	m_imgNotOpen = g_workspace.getImageFactory()->createImage("ui_texture/NewMessage.tga");
	m_imgNotOpen->SetBlend(true);
	m_imgMailNormal = g_workspace.getImageFactory()->createImage("ui\\textures\\1.tga");
	m_imgMailNormal->SetSourceRect(RtwRect(164,399,206,441));
	m_imgMailNormal->SetBlend(true);
	m_imgMailAnimation = g_workspace.getImageFactory()->createImage("ui_texture/xinshou_12.gif");
	m_bShowAnimation = false;
	 unguard;
}

void CMailWithUI::Clear()
{
    guard;
	Save();
    unguard;
}


/*
*/

void CMailWithUI::OpenMailBookUI()
{
    guard;
	g_workspace.SetFocusWidget(mpSendToNameUI);
	mpMailBookUI->Show();
	mpMailBookUI->BringToTop();
	RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->Hide();
    unguard;
}
// 写邮件
void CMailWithUI::NewMail(void*,void*)
{
    guard;
	ClearMailBookUI();
	OpenMailBookUI();
	RTW_WIDGET("fmmail4.lbtitle")->SetText(R(MSG_CREATEMAIL));
    return;
    unguard;
}
void CMailWithUI::Initface()
{
	guard;
	#define BFID_SIZE_X		20
	#define BFID_SIZE_Y		20
	char strId[32];
	RtwImage* faceImage[10][8];
	RtwRect re;
	re.left=77;
	re.right=re.left+3;
	re.top=5;
	re.bottom=re.top+3;
	for (int i=0; i<5; i++)
		for (int j=0; j<8; j++)
		{	
			rt2_sprintf(strId, "face\\%d.gif", i*8+j);
			faceImage[i][j] = g_workspace.getImageFactory()->createImage(strId);
			rt2_sprintf(strId, "face%02d_%02d", i, j);
			faceImage[i][j]->m_Name=strId;
			RtwRect sRect;
			sRect.left = (BFID_SIZE_Y+3)*j;
			sRect.top = (BFID_SIZE_Y+3)*i;
			sRect.right = sRect.left + BFID_SIZE_Y;
			sRect.bottom = sRect.top + BFID_SIZE_Y;
			rt2_sprintf(strId, "btnface%02d_%02d", i, j);
			mpbtnface[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			mpbtnface[i][j]->SetName(strId);
			mpbtnface[i][j]->SetButtonStateImages(faceImage[i][j],NULL,NULL,NULL);
			mpChatFaceUI->AddChild(mpbtnface[i][j]);		
			mpbtnface[i][j]->Move(sRect);
			mpbtnface[i][j]->SetWidgetSize(SSize(15,15));
			mpbtnface[i][j]->EvLClick+=RTW_CALLBACK(this,CMailWithUI,UserInterfacebtndown);
		}
	unguard;
}

void CMailWithUI::ExportMail(void*,void*)
{
    guard;
	Export();
    unguard;
	return;
}

void CMailWithUI::OnEnsureDeleteMail(void*,void*)
{
	guard;
	int iSelect = mpReceiveListUI->GetSelectedItem();
	if(iSelect == -1)
	{
		Message(R(PLEASE_CHOICE_ONE_MAIL));//请先选择一封邮件！
		return;
	}

	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(R(MSG_MAIL_CONFIRMDELETEMAILWITHITEM), UIFormMsg::TYPE_OK_CANCEL,true,"EnsureDeleteMail");
	pConfirm->EvOK		= RTW_CALLBACK_1(this,CMailWithUI,DeleteMail,pConfirm);
	unguard;
}

void CMailWithUI::DeleteMail(void*,void*)
{
    guard;
	int iSelect = mpReceiveListUI->GetSelectedItem();
	if(iSelect == -1)
	{
		Message(R(PLEASE_CHOICE_ONE_MAIL));//请先选择一封邮件！
		return;
	}
	//显示选中的信的内容
	multimap<string,SMailData*,greater<string> >::iterator It;
	int Index=0;
	for(It=mpCurrentReceiveList->begin(),Index=0;It!=mpCurrentReceiveList->end();It++,Index++)
	{
		if(Index==iSelect)
		{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_subsystem_mail);
			pPacket->WriteByte(C2R_DELETE_MAIL);
			pPacket->WriteLong((*It).second->m_lMailID); // 服务器上有检查长度
			NetSend(pPacket);

			Delete(It->second);
			if(m_pfrmDisplayMail->IsVisible())
				m_pfrmDisplayMail->Hide();
			break;
		}
	}	

	return;
    unguard;
}

void CMailWithUI::CCMail(void*,void*)
{
    guard;
	ClearMailBookUI();
	if(mpDisplayMail==NULL)
	{
		return;
		mpContentUI->SetText("");
	}
	else
	{
		mpContentUI->SetText(mpDisplayMail->mContent.c_str());
		string Temp=UI_FW_FLAG;
		Temp+=mpDisplayMail->mTitle;
		mpTitleUI->SetText(Temp.c_str());
	}
	OpenMailBookUI();
	RTW_WIDGET("fmmail4.lbtitle")->SetText(R(MSG_MAIL_TRANSMITMAIL));
	return;

	
    unguard;
}
void CMailWithUI::OnLDClickMailList(RtwWidget* sender, void*)
{
	guard;
	//列表的点选
	int SelectIndex=mpReceiveListUI->GetSelectedItem();

	if(SelectIndex==-1)
	{
		//没有选中
		OnSelectMail(NULL);
		return;
	}

	mpReceiveListUI->SetItemColor(SelectIndex,UI_ALREADY_OPEN_COLOR);//改变选中的颜色
	// 		mpReceiveListUI->SetLineItemImage(SelectIndex,0,m_imgAlreadyOpen);

	//显示选中的信的内容
	multimap<string,SMailData*,greater<string> >::iterator It;
	int Index=0;
	for(It=mpCurrentReceiveList->begin(),Index=0;It!=mpCurrentReceiveList->end();It++,Index++)
	{
		if(Index==SelectIndex)
		{
			OnSelectMail(It->second);
			mpReceiveListUI->SetLineItemImage(Index,0,m_imgAlreadyOpen);
			break;
		}
	}	
	unguard;
}

void CMailWithUI::ReplayMail(void*,void*)
{
    guard;
	ClearMailBookUI();
	if(mpDisplayMail==NULL)
	{
		return;
	}
	else
	{
		mpSendToNameUI->SetText(mpDisplayMail->mFromName.c_str());
		string Temp=UI_RE_FLAG;
		Temp+=mpDisplayMail->mTitle;
		mpTitleUI->SetText(Temp.c_str());

	}
	OpenMailBookUI();
	RTW_WIDGET("fmmail4.lbtitle")->SetText(R(MSG_MAIL_REPLYMAIL));
	return;
    unguard;
}

void CMailWithUI::SendMail(void* ,void*)
{
    guard;
	if(mpSendToNameUI->GetText()=="")
	{
		Message(gMailStr[MSG_MAIL_SEND_TO_NAME_NULL]);
		return;
	}

	if(mpTitleUI->GetText()=="")
	{
		Message(gMailStr[MSG_MAIL_TITLE_NULL]);
		return;
	}

	if(mpContentUI->GetText()=="")
	{
		Message(gMailStr[MSG_MAIL_CONTENT_NULL]);
		return;
	}


	DWORD DBID=mpFriend->GetDBID(SConnection::FRIEND,string(mpSendToNameUI->GetText()));
	if(DBID==0)
	{
		Message(gMailStr[MSG_MAIL_RECEIVE_PLAYER_NOT_IN_FRIEND_LIST]);
		return;
	}

	DWORD itemID;
	long money;
	// 发送前判断
	if(!ItemID_IsValid(m_SendItemID))
		itemID = -1;
	else
		itemID = m_SendItemID.id;
	m_lSendMoney = atoi(m_pSendMoney->getText().c_str());
	if(m_lSendMoney <= 0)
		money = 0;
	else
		money = m_lSendMoney;
	Send(DBID, (char*)mpTitleUI->GetText().c_str(), (char*)mpContentUI->GetText().c_str(),itemID,money);
	if (mpTitleUI->GetText().size() > 100)
	{
		ShowSystemMessage(R(MSG_MAIL_YOURTITLEISSOLONG));
		return;
	}
	if (mpContentUI->GetText().size() > 1024)
	{
		ShowSystemMessage(R(MSG_MAIL_YOURMAILISSOLONG));
		return;
	}
	// 清空上次的附件
	m_SendItemID = ItemID_CreateInvalid();
	m_pSendButton->ModifyFlags(0, wfHint);
	m_pSendButton->SetBackgroundImage((RtwImage*)NULL);
	m_pSendMoney->SetText("");
	m_lSendMoney = 0;

	mpMailBookUI->Hide();
	/*ShowSystemMessage(R(MSG_MAIL_MAILHASSEND));*/

    unguard;
}

void CMailWithUI::Message(const std::string& vrMessage)
{
    guard;
	ShowSystemMessage(vrMessage.c_str(),true);
    unguard;
}

/*
void CMailWithUI::Active(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	if(mpLastFocus==NULL)
	{
		mpLastFocus=mpMailUI;
	}

	g_workspace.AddFocusWidget(mpLastFocus);
    unguard;
}
*/
void CMailWithUI::UserInterfacebtndown(ui::RtwWidget* vpSender,void*)
{
	guard;
	const string& Sender=vpSender->GetQualifiedID();
	if(mpChatFaceUI->GetFlags(wfVisible))
	{
		mpChatFaceUI->Hide();
	}
	else
	{
		mpChatFaceUI->Show();
	}
	for (int i=0; i<12; i++)
		for (int j=0; j<8; j++)
		{	
			if (i==0&&j==7||i==1&&j==7||i==3&&j==7)
			{

			}else
			{	
				if (Sender==mpbtnface[i][j]->getName())
				{		
					//mEdit.AddText((const std::string)faceImage[i][j]->m_Name);
					mpContentUI->InsertImage(mpbtnface[i][j]->getNormalImage());
					g_workspace.SetFocusWidget(mpContentUI);
					return;
				}	
			}
		}

		unguard;
}
  
void CMailWithUI::OpenMailUI(void* ,void*)
{
    guard;
	if (mpMailUI->GetFlags(wfVisible))
	{
		mpMailUI->Hide();
		return;
	}



	/* gao 2009.11.24
	注释说明：如果不注释，当有新的信件时，会把第一封自动设定为已读
	*/
	////如果没有选中信件，自动选第一封 
	//if(mpSelectMail==NULL && mpCurrentReceiveList->size() >= 1)
	//{
	//	OnSelectMail(mpCurrentReceiveList->begin()->second);
	//}

	//玩家查看邮件的新手提示
	GetWorld()->ShowViewMail();
	UpdateReceiveListUI();
	mpMailUI->Show();
	SetMailButtonNormal();
	
    unguard;
}

void CMailWithUI::ActiveFaceForm(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	//表情框//失效事件
	if (vpEvent->type == etUnFocus && 
		vpEvent->activate.widget != RTW_WIDGET(UI_MAIL_FACE_SWITCH_WHOLE_ID))
	{
		RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->Hide();
	}
    unguard;
}

void CMailWithUI::UserInterfaceSelect(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	//表情框被选择
	const char* pKey;
	pKey=RTW_WIDGET_T(RtwIconList,UI_MAIL_FACE_ICON_LIST_WHOLE_ID)->GetSelectedKey();
	//mpContentUI->InsertImage(pKey);
	RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->Hide();//将窗口关闭
    unguard;
}


void CMailWithUI::ChangeFaceUI(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	const string& Sender=vpSender->GetID();
	if(Sender=="btnchatface")
	{//表情框按钮
		if(RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->GetFlags(wfVisible))
		{
			RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->Hide();
		}
		else
		{
			RTW_WIDGET(UI_MAIL_FACE_FORM_WHOLE_ID)->Show();
			g_workspace.SetActiveWidget(mpChatFaceUI);
			g_workspace.BringWidgetToTop(mpChatFaceUI);
			//mpChatFaceUI->BringToTop();
		}
	}
    unguard;
}

void CMailWithUI::ReturnToMailBox(void *sender ,void *)
{
    guard;
	if(sender == RTW_WIDGET("fmmail1.fmcontent.btnback"))
	{
		RTW_WIDGET("fmmail1")->Hide();
		mpMailUI->Show();
	}else
	{
		mpMailBookUI->Hide();
		mpMailUI->Show();
	}
	return;
    unguard;
}

void CMailWithUI::ClearMailBookUI()
{
    guard;
	//清除界面上的东东
	mpSendToNameUI->SetText("");
	((RtwTextBox*)mpSendToNameUI)->SetCapacity(30);
	mpTitleUI->SetText("");
	((RtwTextBox*)mpTitleUI)->SetCapacity(50);
	mpContentUI->SetText("");
	mpContentUI->RemoveAllItems(false);
	((RtwTextBox*)mpContentUI)->SetCapacity(499);
	m_SendItemID= ItemID_CreateInvalid();
	m_ReceiveItemID= ItemID_CreateInvalid();
	m_lSendMoney= 0;
	m_lReceiveMoney= 0;
	m_pSendButton->SetBackgroundImage((RtwImage*)NULL);
	m_pSendMoney->SetText("");
	unguard;
}

void CMailWithUI::OnDelete(SMailData* vpMail)								//邮件删除后的处理
{
    guard;
	//邮件的收件列表
	multimap<string,SMailData*,greater<string> >::iterator It;
	multimap<string,SMailData*,greater<string> >*	pMap[3];
	
	pMap[0]=&mTitleMap;
	pMap[1]=&mAuthorMap;
	pMap[2]=&mTimeMap;
	for(int i=0;i<3;i++)
	{
		for(It=pMap[i]->begin();It!=pMap[i]->end();++It)
		{
			if(It->second==vpMail)
			{
				CG_CmdPacket* pPacket = NetBeginWrite();
				pPacket->WriteShort(c2r_subsystem_mail);
				pPacket->WriteByte(C2R_DELETE_MAIL);
				pPacket->WriteLong(vpMail->m_lMailID);
				NetSend(pPacket);

				pMap[i]->erase(It);
				break;
			}
		}
	}

	OnSelectMail(NULL);//不选任何一封信
	UpdateReceiveListUI();//更新接收列表

	return;


    unguard;
}

void CMailWithUI::OnReceive(SMailData* vpMail)	//接收到邮件后的处理
{
    guard;

	//邮件的收件列表
	mTitleMap.insert(multimap<string,SMailData*>::value_type(vpMail->mTitle,vpMail)) ;
	mAuthorMap.insert(multimap<string,SMailData*>::value_type(vpMail->mFromName ,vpMail));
	mTimeMap.insert(multimap<string,SMailData*>::value_type(vpMail->mTime,vpMail)) ;

	
	/*
	//如果是第一封信，自动选中
	if(mpCurrentReceiveList->size()==1)
	{
		OnSelectMail(vpMail);
	}
	*/
	
	UpdateInfoAtFriendList();
	UpdateReceiveListUI();

	
	if (g_layerMain && !vpMail->mAlreadyOpen)
		g_layerMain->AddRemindIcon(UIRemind_NewMail);

	return;
    unguard;
}


void CMailWithUI::UpdateInfoAtFriendList()
{
    guard;
	//好友列表上的收件箱的信息
	int Count=0;
	std::list<SMailData*>::iterator It;		
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{
		if(!(*It)->mAlreadyOpen)
		{
			Count++;
		}
	}

	stringstream Temp;
	if(Count==0)
	{
		Temp << UI_FRIEND_MAIL_INFO_FLAG1 << UI_FRIEND_MAIL_INFO_NULL;
	}
	else
	{
		Temp << UI_FRIEND_MAIL_INFO_FLAG1 << Count<<UI_FRIEND_MAIL_INFO_FLAG2;
	}

	//mpInfoAtFriendListUI->SetText(Temp.str().c_str());

    unguard;
}

bool CMailWithUI::OnMemberChoice(DWORD vDBID,const char* vpName)
{
    guard;
	RtwWidget *pWidget = g_workspace.GetFocusWidget();
	if (pWidget!=NULL)
	{
		RtwWidget *pParent = pWidget->GetParent();
		if(pParent && pParent == mpMailBookUI)
		{//父窗口是收发邮件的窗口
			mpSendToNameUI->SetText(vpName);//设置收件人的名字
		}
	}
	return false;
    unguard;
}


void CMailWithUI::OnSelectMail(SMailData* vpMail)
{
    guard;
	mpSelectMail=vpMail;
	OnDisplayMail(vpMail);
    unguard;
}

void CMailWithUI::OnDisplayMail(SMailData* vpMail)											//当显示一封邮件时
{
    guard;
	if(vpMail==NULL)
	{
		mpDisplayMail=NULL;
		mpPreviewUI->SetText("");
		mpPreviewAuthorUI->SetText("");
		mpPreviewTitleUI->SetText("");
		mpPreviewTitleUI->SetText("");
		return;
	}

	if(!vpMail->mAlreadyOpen)
	{
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_mail);
		pPacket->WriteByte(C2R_SETMAIL_READ);
		pPacket->WriteLong(vpMail->m_lMailID);
		NetSend(pPacket);
		vpMail->mAlreadyOpen=true;
	}
	mpDisplayMail=vpMail;
	mpPreviewUI->SetText(vpMail->mContent);
	mpPreviewAuthorUI->SetText(vpMail->mFromName);
	mpPreviewAuthorUI->SetTextColor(0xffffffff);
	mpPreviewTitleUI->SetText(vpMail->mTitle);
	mpPreviewTitleUI->SetTextColor(0xffffffff);
	mpPreviewTimeUI->SetText(vpMail->mTime);
	mpPreviewTimeUI->SetTextColor(0xffffffff);

	// 直接将道具刷新出来
	SItemBase *pItemClass = NULL;
	if (ItemID_IsValid(vpMail->m_Item))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(vpMail->m_Item.type);
		if(!pItemClass)			return;
		m_pReceiveButton->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
	}
	else 
		m_pReceiveButton->SetBackgroundImage((RtwImage*)NULL);
	char strmoney[32];
	rt2_sprintf(strmoney,"%d",vpMail->m_lMoney);
	m_pReceiveMoney->SetText(strmoney);

	if(!m_pfrmDisplayMail->IsVisible() && mpDisplayMail)
		m_pfrmDisplayMail->Show();
	UpdateInfoAtFriendList();
	g_workspace.SetFocusWidget(m_pfrmDisplayMail);
	//玩家查看邮件附件的新手提示
	GetWorld()->ShowExtraMail();
    unguard;
}


void CMailWithUI::UpdateReceiveListUI()
{
    guard;
	mpReceiveListUI->RemoveAllItems();
	
	/* gao 2009.11.20
	此处替换东西不少，就不细分出来了，这部分代码更改
	*/
	int Index = 0;
	multimap<string,SMailData*,greater<string> >::iterator It;
	for(It=mpCurrentReceiveList->begin();It!=mpCurrentReceiveList->end();++It)
	{
		mpReceiveListUI->AddItem();//添加一个空行
		mpReceiveListUI->SetItemText(Index,1,It->second->mFromName);//1列//作者
		mpReceiveListUI->SetItemText(Index,2,It->second->mTitle);//0列//标题
		mpReceiveListUI->SetItemText(Index,3,It->second->mTime);//2列//时间

		//已经打开的信的颜色
		if(It->second->mAlreadyOpen)
		{
			mpReceiveListUI->SetItemColor(Index,UI_ALREADY_OPEN_COLOR);
			mpReceiveListUI->SetLineItemImage(Index,0,m_imgAlreadyOpen);
		}else
		{
			mpReceiveListUI->SetLineItemImage(Index,0,m_imgNotOpen);
		}
		
		//当前选中的信
		if(It->second==mpSelectMail)
		{
			mpReceiveListUI->SetSelectedItem(Index);
		}
		Index++;
	}
	/* end */
    unguard;
}



void CMailWithUI::UserClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	if(vpSender==mpReceiveListUI->GetColumnHeader(0))
	{//第一列//标题
		mpCurrentReceiveList=&mTitleMap;
		
	}
	else if(vpSender==mpReceiveListUI->GetColumnHeader(1))
	{//第二列//作者
		mpCurrentReceiveList=&mAuthorMap;
	}
	else if(vpSender==mpReceiveListUI->GetColumnHeader(2))
	{//第三列//时间
		mpCurrentReceiveList=&mTimeMap;
	}
	UpdateReceiveListUI();
	return;
    unguard;
}

const char MAIL_ABSTRACT_COMPART[]="  ";
void CMailWithUI::UserHint(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	string Temp;
	/* gao 2009.11.23
	此处，mpPreviewFormUI 替换 mpPreviewTitleUI
	*/
	if(vpSender==mpPreviewFormUI)
	{
		if(mpDisplayMail==NULL)
		{
			Temp="";
		}
		else
		{
			Temp=mpDisplayMail->mTitle;
		}
		
	}
	else if(vpSender==mpReceiveListUI)
	{
		/* gao 2009.11.20 */
		int line = vpEvent->menuSelect.index;
		if(line == -1)
			return;
		Temp=mpReceiveListUI->GetItemText(line,1);
		Temp+=MAIL_ABSTRACT_COMPART;
		Temp+=mpReceiveListUI->GetItemText(line,2);
		Temp+=MAIL_ABSTRACT_COMPART;
		Temp+=mpReceiveListUI->GetItemText(line,3);
	}
	else 
	{
		return;
	}

	g_workspace.SetHintText(Temp);
	g_workspace.AutoHintSize();
	g_workspace.SetHintPosition(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());

	return;
    unguard;
}

void CMailWithUI::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
    guard;
	GetWorld()->HideGuideForm();

	if(vpSender == RTW_WIDGET("fmmail2.fmcontent.btnxmm3"))
	{
		//列表的点选
		int SelectIndex=mpReceiveListUI->GetSelectedItem();

		if(SelectIndex==-1)
		{
			//没有选中
			Message(R(PLEASE_CHOICE_ONE_MAIL));//请先选择一封邮件！
			return;
		}
		else
		{
			OnSelectMail(NULL);
		}

		mpReceiveListUI->SetItemColor(SelectIndex,UI_ALREADY_OPEN_COLOR);//改变选中的颜色
// 		mpReceiveListUI->SetLineItemImage(SelectIndex,0,m_imgAlreadyOpen);

		//显示选中的信的内容
		multimap<string,SMailData*,greater<string> >::iterator It;
		int Index=0;
		for(It=mpCurrentReceiveList->begin(),Index=0;It!=mpCurrentReceiveList->end();It++,Index++)
		{
			if(Index==SelectIndex)
			{
				OnSelectMail(It->second);
				mpReceiveListUI->SetLineItemImage(Index,0,m_imgAlreadyOpen);
				break;
			}
		}	
	}
	else if(vpSender == RTW_WIDGET("fmmail4.fmcontent.btnback"))
		m_pfrmDisplayMail->Hide();
	else if(vpSender == RTW_WIDGET("fmmail2.fmcontent.btnxmm4"))
		mpMailUI->Hide();
    unguard;
}

void CMailWithUI::MailBookUIHide(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{

}

void CMailWithUI::MailBookUIActive(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
    guard;
	unguard;
}
void CMailWithUI::OnDragToSendItem(RtwWidget* sender, void*)
{
	guard;
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if (ItemIsPet(item.type))
			{
				if(ItemID_IsValid(g_layerMain->m_fromPlayerItems->OnGetUsingPetItem()))
				{
					if(g_layerMain->m_fromPlayerItems->OnGetUsingPetItem().id == item.id)
					{
						ShowSystemMessage(R(MSG_PET_CANNOTSELLWHILECALL));
						return;
					}
				}
			}
			if (item.cBind!=IB_NotBind || !ItemCanDelete(item.type) || !ItemCanDrop(item.type) || !ItemCanTrade(item.type))
			{
				ShowSystemMessage(R(MSG_TRADE_BINDED));
				return;
			}
			m_pSendButton->ModifyFlags(wfHint|wfDragClient | wfDragServer|wfDragIn|wfDragOut, 0);
			m_SendItemID = item;
			m_iISendItem = i;
			m_iJSendItem = j;
			m_iPage = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();

			g_workspace.ResetHintText();

			// 直接将道具刷新出来
			SItemBase *pItemClass = NULL;
			if (ItemID_IsValid(m_SendItemID))
			{
				pItemClass = GetWorld()->m_pItemManager->GetItem(m_SendItemID.type);
				if(!pItemClass)			return;
				m_pSendButton->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
			}
			else 
				m_pSendButton->SetBackgroundImage((RtwImage*)NULL);
		}	
	}
	unguard;
}
void CMailWithUI::OnDragEndSendItem(RtwWidget* sender, void*)
{
	guard;
	if(!g_workspace.getMouseHover())
	{
		m_SendItemID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		sender->SetBackgroundImage((RtwImage*)NULL);
	}
	unguard;
}
void CMailWithUI::OnGetHintItemText(RtwWidget* sender, void*)
{
	guard;
	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, m_SendItemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), m_iPage, m_iISendItem, m_iJSendItem);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}
void CMailWithUI::OnHideSendMail(RtwWidget* sender, void*)
{
	guard;
	// 当发送界面关闭的时候，删除要发送的道具，以及money
	if(sender->GetParent() == m_pfrmDisplayMail)
	{
		m_ReceiveItemID = ItemID_CreateInvalid();
		m_pReceiveButton->ModifyFlags(0,wfHint);
		m_pReceiveButton->SetBackgroundImage((RtwImage*)NULL);
		m_pReceiveMoney->SetText("");
		m_lReceiveMoney = 0;
	}else
	{
		m_SendItemID = ItemID_CreateInvalid();
		m_pSendButton->ModifyFlags(0, wfHint);
		m_pSendButton->SetBackgroundImage((RtwImage*)NULL);
		m_pSendMoney->SetText("");
		m_lSendMoney = 0;
	}
	unguard;
}
void CMailWithUI::OnGetHintReceiveItem(RtwWidget* sender, void*)
{
	guard;
	if(ItemID_IsValid(mpDisplayMail->m_Item))
	{
		int nLines;
		RtString str = GenerateItemHtmlHint(nLines, mpDisplayMail->m_Item, GetWorld()->m_pPlayer);

		g_workspace.SetHintText((const char*)str, true);
		g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}else
	{
		sender->ModifyFlags(0, wfHint);
	}
	unguard;
}
void CMailWithUI::OnReceiveItemAndMoney(RtwWidget* sender, void*)
{
	guard;
	if (g_layerMain->m_formGuide3->IsVisible())
	{
		g_layerMain->m_formGuide3->Hide();
	}
	DWORD Itemid;
	long money;
	if(ItemID_IsValid(mpDisplayMail->m_Item))
		Itemid = mpDisplayMail->m_Item.id;
	else
		Itemid = -1;
	if(mpDisplayMail->m_lMoney > 0)
		money = mpDisplayMail->m_lMoney;
	else
		money = 0;
	if(GetPlayer()->mItem.m_Bag.FreeSpace() == 0 || GetPlayer()->mItem.m_Bag.GetMoney() > MAX_MONEY_COUNT - money)
	{
		ShowSystemMessage(R(MSG_ITEM_BAGFULL));
		return;
	}
	std::list<SMailData*>::iterator it;
	for(it = mReceiveBox.begin();it != mReceiveBox.end();it++)
	{
		if((*it)->m_lMailID == mpDisplayMail->m_lMailID)
		{
			(*it)->m_Item = ItemID_CreateInvalid();
			(*it)->m_lMoney = 0;
		}
	}
	m_pReceiveButton->SetBackgroundImage((RtwImage*)NULL);
	m_pReceiveMoney->SetText("");

	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_mail);
	pPacket->WriteByte(C2R_MOVE_ITEM);
	pPacket->WriteLong(mpDisplayMail->m_lMailID);
	NetSend(pPacket);
	unguard;
}
void CMailWithUI::OnClearMailList()
{
	guard;
	list<SMailData*>::iterator It;
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{
		DEL_ONE(*It);
	}
	mReceiveBox.clear();
	mTitleMap.clear();
	mAuthorMap.clear();
	mTimeMap.clear();
	Reset();
	unguard;
}
void CMailWithUI::OnSendButtonMouseButtonDown(RtwWidget* sender, void*)
{
	guard;
	m_SendItemID = ItemID_CreateInvalid();
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	unguard;
}
void CMailWithUI::SetMailButtonNormal()
{
	guard;
	if(m_bShowAnimation)
	{
		RTW_WIDGET_T(RtwButton,"fmsystem.btnsysmail")->SetNormalImage(m_imgMailNormal);
		m_bShowAnimation = false;
	}
	unguard;
}
void CMailWithUI::SetMailButtonAnimation()
{
	guard;
	if(!m_bShowAnimation && !mpMailUI->GetFlags(wfVisible))
	{
		RTW_WIDGET_T(RtwButton,"fmsystem.btnsysmail")->SetNormalImage(m_imgMailAnimation);
		m_bShowAnimation = true;
	}
	unguard;
}

void CMailWithUI::OnSendMoneyChanged(RtwWidget* sender, void*)
{
	guard;
	string moneyStr = m_pSendMoney->GetText();
	long money = atoi(moneyStr.c_str());
	if(money > GetPlayer()->mItem.m_Bag.GetMoney())
	{
		money = GetPlayer()->mItem.m_Bag.GetMoney();
	}
	char str[20];
	rt2_sprintf(str,"%d",money);
	m_pSendMoney->SetText(str);
	unguard;
}

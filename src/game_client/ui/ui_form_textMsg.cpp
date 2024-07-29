#include "gc_include.h"
#include "ui_form_textmsg.h"

unordered_map<string, RtwRefPtr<UIFormTextMsg> > UIFormTextMsg::s_forms;
unordered_map<string, RtwRefPtr<CUIFormInputBox> > CUIFormInputBox::input_forms;

UIFormTextMsg::UIFormTextMsg(FormType t)//modify by fox for bug
: m_autoDelete(false)
{
	guard;

	switch(t)
	{
	case TYPE_OK:
//		m_pFrmThis = g_workspace.LoadWidgetFromTheme("TextMessageBox");// change [3/18/2009 tooth.shi]
		m_pFrmThis = LOAD_UI("TextMessageBox");
		m_pFrmThis->Hide();

		(m_pFrmThis->GetChildWidget("btnok"))->EvLClick +=
			RTW_CALLBACK(this, UIFormTextMsg, OnLClick_OK);

		break;

	case TYPE_OK_CANCEL:
//		m_pFrmThis = g_workspace.LoadWidgetFromTheme("TextMessageBox");// change [3/18/2009 tooth.shi]
		m_pFrmThis = LOAD_UI("TextMessageBox");
		m_pFrmThis->Hide();

		RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
		pTB->EvUpdateText += RTW_CALLBACK(this, UIFormTextMsg, OnUpdateText);
		pTB->EvInputChar  += RTW_CALLBACK(this, UIFormTextMsg, OnInputChar);

		(m_pFrmThis->GetChildWidget("btnok"))->EvLClick +=
			RTW_CALLBACK(this, UIFormTextMsg, OnLClick_OK);

		(m_pFrmThis->GetChildWidget("btncancel"))->EvLClick +=
			RTW_CALLBACK(this, UIFormTextMsg, OnLClick_Cancel);

		// gao 2010.3.10
		/*(m_pFrmThis->GetChildWidget("$hide"))->EvLClick +=
			RTW_CALLBACK(this, UIFormTextMsg, OnLClick_Cancel);*/
		break;
	}

	//m_pFrmThis->EvClose += RTW_CALLBACK(this, UIFormTextMsg, OnClose);//rem by fox for bug
	m_maxNum= 0;
	m_iLength= 0;
	m_bNumOnly= false;
	unguard;
}

UIFormTextMsg::~UIFormTextMsg()
{
	guard;	

	//rem by fox for bug
	/*
	if (m_pFrmThis)
	{
		m_pFrmThis->Close();
	}
	*/

	unguard;
}

UIFormTextMsg* UIFormTextMsg::ShowStatic(const RtString& title, const RtString& content, FormType t, bool modal,const string& id)
{
	//一个补丁//当不同类型的UIFormTextMsg，的ID是不应该一样的//add by fox for bug
	string FixID;
	switch(t)
	{
	case TYPE_OK:
		FixID=id+"type_ok";
		break;
	case TYPE_OK_CANCEL:
		FixID=id+"type_ok_cancel";
		break;
	default:
		FixID=id+"unknow";
		break;
	}
	

	unordered_map<string, RtwRefPtr<UIFormTextMsg> >::iterator iter = s_forms.find(FixID);
	if(iter != s_forms.end())
	{
		//重新初始化
		//<add by fox for bug>
		iter->second->SetNumberOnly(false);
		iter->second->SetString(string(""));
		iter->second->SetTxtLength(0);
		iter->second->SetMaxNum(0);
		iter->second->m_string="";
		iter->second->EvOK.Clear();
		iter->second->EvCancel.Clear();
		//</add by fox for bug>
        iter->second->SetPassword(0);




		iter->second->Show(title,content,modal);
		return iter->second;
	}

	UIFormTextMsg* form = RT_NEW UIFormTextMsg(t);//modify by fox for bug

	form->m_id=FixID;

	s_forms[FixID]=form;

	form->Show(title, content, modal);

	return form;
}

void UIFormTextMsg::Show(const RtString& title, const RtString& content, bool modal)
{
	guard;

//	m_pFrmThis->GetChildWidget("lbTitle")->SetText((char*)title);
	m_pFrmThis->GetChildWidget("lbContent")->SetText((char*)content);
	m_pFrmThis->GetChildWidget("txtString")->SetFocus();

	if (modal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

	m_pFrmThis->BringToTop();
	g_workspace.SetFocusWidget(m_pFrmThis);
	
	unguard;
}

void UIFormTextMsg::SetString(string& str)
{
    m_pFrmThis->GetChildWidget("txtString")->SetText(str);
}
void UIFormTextMsg::SetTxtLength(int num)
{
	guard;
	((RtwTextBox*)(m_pFrmThis->GetChildWidget("txtString")))->SetCapacity(num);
	m_iLength= num;
	unguard;
}

void UIFormTextMsg::Hide()
{
	guard;

	m_pFrmThis->Hide();

	unguard;
}

bool UIFormTextMsg::IsVisible()
{
	guard;

	return m_pFrmThis != NULL && m_pFrmThis->IsVisible();

	unguard;
}

void UIFormTextMsg::SetNumberOnly(bool bNumberOnly)
{
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	pTB->SetNumberOnly(bNumberOnly);
	m_bNumOnly= bNumberOnly;
}

void UIFormTextMsg::SetMaxNum(int num)
{
	guard;
	m_maxNum= num;
	unguard;
}

void UIFormTextMsg::SetPassword(bool bPassword)
{

    RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
    if (bPassword)
        pTB->SetPassword('*');
    else 
        pTB->SetPassword(0);
}

void UIFormTextMsg::OnUpdateText(RtwWidget* sender, void*)
{
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();

	EvOK(sender, weUpdateText);

	//rem by fox for bug
	/*
	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
		*/
	m_pFrmThis->Hide();//add by fox for bug
}

void UIFormTextMsg::OnInputChar(RtwWidget* sender, void*)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();
	int num= atoi(m_string.c_str());
	if(!m_maxNum)
		return;
	if(num<= 0)
		pTB->SetText("0");
	else if(num> m_maxNum)
	{
		char length[100];
		rt2_snprintf(length,100,"%d",m_maxNum);
		pTB->SetText(length);
	}
	//EvOK(sender, weUpdateText);

	//m_pFrmThis->Hide();
	unguard;
}

void UIFormTextMsg::OnLClick_OK(RtwWidget* sender, void*)
{
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();

	EvOK(sender, weMouseClick);

	/*rem by fox for bug
	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
		*/
	m_pFrmThis->Hide();//add by fox for bug
}

void UIFormTextMsg::OnLClick_Cancel(RtwWidget* sender, void*)
{
	EvCancel(sender, weMouseClick);

	/*rem by fox for bug
	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
		*/
	m_pFrmThis->Hide();//add by fox for bug
}

/*rem by fox for bug
void UIFormTextMsg::OnClose(void*, void*)
{
	//if (m_autoDelete)
	//	delete this;
}
*/
// using for inputbox
CUIFormInputBox::CUIFormInputBox()//modify by fox for bug
: m_autoDelete(false)
{
	guard;
	m_pFrmThis = LOAD_UI("InputBox");
	m_pFrmThis->Hide();

	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	pTB->EvInputChar += RTW_CALLBACK(this, CUIFormInputBox, OnInputChar);
	pTB->EvUpdateText += RTW_CALLBACK(this, CUIFormInputBox, OnUpdateText);
	pTB->SetNumberOnly(true);
	pTB->SetText("0");
	SetTxtLength(2);
	m_string = "0";

	(m_pFrmThis->GetChildWidget("btnok"))->EvLClick				+=RTW_CALLBACK(this, CUIFormInputBox, OnLClick_OK);
	(m_pFrmThis->GetChildWidget("btncancel"))->EvLClick			+=RTW_CALLBACK(this, CUIFormInputBox, OnLClick_Cancel);
	(m_pFrmThis->GetChildWidget("upbutton"))->EvLClick			+=RTW_CALLBACK(this, CUIFormInputBox, OnAddNum);
	(m_pFrmThis->GetChildWidget("downbutton"))->EvLClick		+=RTW_CALLBACK(this, CUIFormInputBox, OnSubNum);
	(m_pFrmThis->GetChildWidget("btnmax"))->EvLClick			+=RTW_CALLBACK(this, CUIFormInputBox, OnSetMaxNum);

	m_maxNum = 99;
	m_iLength = 2;
	// gao 2010.3.10
	/*(m_pFrmThis->GetChildWidget("$hide"))->EvLClick +=
		RTW_CALLBACK(this, UIFormTextMsg, OnLClick_Cancel);*/
	//m_pFrmThis->EvClose += RTW_CALLBACK(this, UIFormTextMsg, OnClose);//rem by fox for bug

	unguard;
}

CUIFormInputBox::~CUIFormInputBox()
{
	guard;	
	unguard;
}

CUIFormInputBox* CUIFormInputBox::ShowStatic(const RtString& title, const RtString& content, bool modal,const string& id)
{
	guard;
	//一个补丁//当不同类型的CUIFormInputBox，的ID是不应该一样的//add by fox for bug
	string FixID;
		FixID=id+"input_box";	

	unordered_map<string, RtwRefPtr<CUIFormInputBox> >::iterator iter = input_forms.find(FixID);
	if(iter != input_forms.end())
	{
		//重新初始化
		//<add by fox for bug>
		iter->second->SetNumberOnly(true);
		iter->second->SetString(string("0"));
		iter->second->SetTxtLength(2);
		iter->second->m_string="0";
		iter->second->EvOK.Clear();
		iter->second->EvCancel.Clear();
		//</add by fox for bug>
        iter->second->SetPassword(0);
		iter->second->SetMaxNum(99);;

		iter->second->Show(title,content,modal);
		return iter->second;
	}else
	{
		CUIFormInputBox* form = RT_NEW CUIFormInputBox();//modify by fox for bug
		form->m_id=FixID;
		input_forms[FixID]=form;
		form->Show(title, content, modal);
		return form;
	}
	unguard;
}

void CUIFormInputBox::Show(const RtString& title, const RtString& content, bool modal)
{
	guard;

//	m_pFrmThis->GetChildWidget("lbTitle")->SetText((char*)title);
	m_pFrmThis->GetChildWidget("lbContent")->SetText((char*)content);
	m_pFrmThis->GetChildWidget("txtString")->SetFocus();

	if (modal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

	m_pFrmThis->BringToTop();
	
	unguard;
}

void CUIFormInputBox::SetString(string& str)
{
	guard;
    m_pFrmThis->GetChildWidget("txtString")->SetText(str);
	unguard;
}

int CUIFormInputBox::GetNumFromInputBox()
{
	guard;
	return atoi(m_string.c_str());
	unguard;
}
void CUIFormInputBox::SetTxtLength(int num)
{
	guard;
	((RtwTextBox*)(m_pFrmThis->GetChildWidget("txtString")))->SetCapacity(num);
	m_iLength = num;
	unguard;
}

void CUIFormInputBox::SetMaxNum(int num)
{
	guard;
	m_maxNum = num;
	unguard;
}

void CUIFormInputBox::Hide()
{
	guard;
	m_pFrmThis->Hide();
	unguard;
}

bool CUIFormInputBox::IsVisible()
{
	guard;
	return m_pFrmThis != NULL && m_pFrmThis->IsVisible();
	unguard;
}

void CUIFormInputBox::SetNumberOnly(bool bNumberOnly)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	pTB->SetNumberOnly(bNumberOnly);
	unguard;
}

void CUIFormInputBox::SetPassword(bool bPassword)
{
	guard;
    RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
    if (bPassword)
        pTB->SetPassword('*');
    else 
        pTB->SetPassword(0);
	unguard;
}

void CUIFormInputBox::OnInputChar(RtwWidget* sender, void*)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();
	int num= atoi(m_string.c_str());
	if(num< 0)
		pTB->SetText("0");
	else if(num> m_maxNum)
	{
		char* length= RT_NEW char[m_iLength+1];
		memset(length,0,m_iLength+1);
		rt2_sprintf(length,"%d",m_maxNum);
		pTB->SetText(length);
		DEL_ONE(length);
	}
	//EvOK(sender, weUpdateText);

	//m_pFrmThis->Hide();
	unguard;
}

void CUIFormInputBox::OnUpdateText(RtwWidget* sender, void*)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();

	EvOK(sender, weUpdateText);

	m_pFrmThis->Hide();
	unguard;
}

void CUIFormInputBox::OnLClick_OK(RtwWidget* sender, void*)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->GetText().c_str();

	EvOK(sender, weMouseClick);

	m_pFrmThis->Hide();
	unguard;
}

void CUIFormInputBox::OnLClick_Cancel(RtwWidget* sender, void*)
{
	guard;
	EvCancel(sender, weMouseClick);

	m_pFrmThis->Hide();
	unguard;
}

void CUIFormInputBox::OnAddNum(RtwWidget* sender, void*)
{
	guard;

	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->getText().c_str();
	int num= atoi(m_string.c_str());
	num++;
	if(num> m_maxNum)
	{
		num= 0;
	}
	char* length= RT_NEW char[m_iLength+1];
	memset(length,0,m_iLength+1);
	rt2_sprintf(length,"%d",num);
	pTB->SetText(length);
	DEL_ONE(length);

	unguard;
}

void CUIFormInputBox::OnSubNum(RtwWidget* sender, void*)
{
	guard;

	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	m_string = pTB->getText().c_str();
	int num= atoi(m_string.c_str());
	num--;
	if(num< 0)
	{
		num= m_maxNum;
	}
	char* length= RT_NEW char[m_iLength+1];
	memset(length,0,m_iLength+1);
	rt2_sprintf(length,"%d",num);
	pTB->SetText(length);
	DEL_ONE(length);

	unguard;
}

void CUIFormInputBox::OnSetMaxNum(RtwWidget* sender, void*)
{
	guard;
	RtwTextBox *pTB = (RtwTextBox *)m_pFrmThis->GetChildWidget("txtString");
	char* length= RT_NEW char[m_iLength+1];
	memset(length,0,m_iLength+1);
	rt2_sprintf(length,"%d",m_maxNum);
	pTB->SetText(length);
	DEL_ONE(length);
	unguard;
}

void CUIFormInputBox::OnMoveToXY(int x,int y)
{
	guard;
	m_pFrmThis->Move(x,y);
	unguard;
}

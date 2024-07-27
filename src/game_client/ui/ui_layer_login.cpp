#include "gc_include.h"
#include "ui_layer.h"
#include "ui_form_msg.h"
#include "gc_login.h"

UILayerLogin::UILayerLogin()
{
	guard;
	//lyymark 2.GcLogin.XML 加载用户登录UI
	g_workspace.Load("ui\\ui_layer_login.xml");

	//用户名输入框
	m_username = RTW_WIDGET_T(RtwTextBox, "fmlogin.fmusername.txtusername");
	m_username->EvLClick += RTW_CALLBACK(this, UILayerLogin, OnHideKeyboard);
	m_username->EvKeyChar+=RTW_CALLBACK(this, UILayerLogin, ontab);
	m_username->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));
	//设置用户输入框输入长度上限
	m_username->SetCapacity(20);
	m_username->SetFocus();

	//密码输入框
	m_password = RTW_WIDGET_T(RtwTextBox, "fmlogin.fmpassword.txtpassword");
	m_password->EvLClick += RTW_CALLBACK(this, UILayerLogin, OnShowKeyboard);
	m_password->EvKeyChar+=RTW_CALLBACK(this, UILayerLogin, ontab);
	m_password->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));
	//设置密码输入框输入长度上限
	m_password->SetCapacity(30);

	//单选框是否记住用户名
    m_pBtnSaveAccount = RTW_WIDGET_T(CUiCheckButton, "fmlogin.chkbutton");
	m_pBtnSaveAccount->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_SaveAccount));

	//软键盘
	LOAD_UI("fmlogin.btnkeyboard")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_Keyboard));

	//领取密保
	LOAD_UI("btngetpwd")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_GetPassword));
	//忘记密码
	LOAD_UI("btnforgetpwd")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_ForgetPassword));
	//申请帐号
	LOAD_UI("btncreate")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_CreateAccount));

	//退出按钮
	LOAD_UI("btnexit")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_Quit));
	//登录按钮
	LOAD_UI("fmlogin.btnlogin")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_Login));
	//强制登录
	LOAD_UI("ConfirmBox2.btnconfirm")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLogin));
	LOAD_UI("ConfirmBox2.btncancel")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLoginCancel));

	LOAD_UI("fmlogin.btnlogin")->EvKeyDown.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnKey));
	//返回按钮
	LOAD_UI("btnback")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_BackSelectServer));

	//lyymark 2.GcLogin.XML 加载服务器列表UI
	m_formServer = RT_NEW CUIForm_Server;
    m_iCurrentSelectServer = 1;
	//LOAD_UI("btnexit")->Show();

 	m_Keyboard.Init();

	unguard;
}

UILayerLogin::~UILayerLogin()
{
	guard;
	if(m_formServer)
	{
		DEL_ONE(m_formServer);
	}

	unguard;
}

void UILayerLogin::OnClicked_BackSelectServer(RtwWidget* sender, void*)
{
    guard;
    if (GetLogin()->GetStatus()==GcLogin::GLS_LOGIN)
    {
        GetLogin()->SetLoginState(GcLogin::GLS_SELECT_GAMEWORLD_SERVER);
    }
    unguard;
}

bool IsValidAccountName(const string& name)
{
    if (name.length() <= 0 || name.length() > 40)
    {
        return false;
    }
    const char* p = name.c_str();
    const char* stop = p + name.length(); 
    while(p < stop)
    {
        if (!((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z')))
        {
			if ((*p =='@' )||(*p =='-' )||(*p =='_' )||(*p =='.' ))
			{
				 
			}else
		    {
				return false;
			}
        }
        p++;
    }
    return true;
}

//游戏客户端登录按钮的响应事件
void UILayerLogin::OnClicked_Login(void*, void*)
{
	guard;

    if (GetLogin()->GetStatus()==GcLogin::GLS_LOGIN)
    {
        if (m_username->GetText().length()==0)
        {
            ShowMessage(R(MSG_USERNAME_CANNOT_NULL));
            return;
        }

		std::string str = m_username->GetText().substr(0, 1);
		const char *pFirst = str.c_str();
 		if (!((*pFirst >= 'A' && *pFirst <= 'Z') || (*pFirst >= 'a' && *pFirst <= 'z')))
		{
			ShowMessage(R(MSG_USERNAME_INVALID));
			return;
		}
		if (!strncmp(m_username->GetText().c_str(), "test", strlen("test")) == 0 && !strcmp(m_username->GetText().c_str(), "admin") == 0)
		{
			if (m_username->GetText().length()<4 || m_username->GetText().length()>20)
			{
				ShowMessage(R(MSG_USERNAME_INVALID_LONG));
				return;
			}
		}
        if (!IsValidAccountName(m_username->GetText()))
        {
            ShowMessage(R(MSG_USERNAME_INVALID));
            return;
        }
        if (m_password->GetText().length()==0)
        {
            ShowMessage(R(MSG_PASSWORD_CANNOT_NULL));
            return;
        }
		if(!strcmp(m_password->GetText().c_str(), "123456") == 0)
		{
			if (m_password->GetText().length()<7 || m_password->GetText().length()>30)
			{
				ShowMessage(R(MSG_PASSWORD_INVALID_LONG));
				return;
			}
		}

        GetLogin()->Login((char*)m_username->GetText().c_str(), (char*)m_password->GetText().c_str());

        // 保存帐号
        RtIni iniUser;
        if (!iniUser.OpenFile(R(INI_USER), true))
        {
            return;
        }

        if (m_pBtnSaveAccount->GetChecked())
        {
            if (!iniUser.FindSection("login")) iniUser.AddSection("login");
            if (!iniUser.SetEntry("login", "username", m_username->GetText().c_str()))
            {
                iniUser.AddEntry("login", "username", m_username->GetText().c_str());
            }
        }else
        {
            iniUser.DelEntry("login", "username");
        }

        iniUser.CloseFile();
    }

	unguard;
}

//强制登录按钮的响应事件
void UILayerLogin::OnClicked_ForceLogin(void*, void*)
{
	GetLogin()->SetForceLogin(true);
	GetLogin()->OnNetConnect(true);
}

void UILayerLogin::OnClicked_ForceLoginCancel(void*, void*)
{
	RTW_WIDGET("fmlogin.fmpassword.txtpassword")->SetText("");
	GetLogin()->SetForceLogin(false);
}
void UILayerLogin::OnClicked_Quit(void*, void*)
{
	guard;

	PostQuitMessage(0);

	unguard;
}

//领取密保
void UILayerLogin::OnClicked_GetPassword(void*, void*)
{
	guard;

	const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
	if (szHomepage==NULL) szHomepage = "http://www.wtgame.net";
	std::string szCmd = "explorer.exe ";
	szCmd += szHomepage;

	ShowWindow(GetWndHandle(), SW_MINIMIZE);
	WinExec(szCmd.c_str(), SW_SHOW);

	unguard;
}
//忘记密码
void UILayerLogin::OnClicked_ForgetPassword(void*, void*)
{
	guard;

	const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
	if (szHomepage==NULL) szHomepage = "http://www.wtgame.net";
	std::string szCmd = "explorer.exe ";
	szCmd += szHomepage;

	ShowWindow(GetWndHandle(), SW_MINIMIZE);
	WinExec(szCmd.c_str(), SW_SHOW);

	unguard;
}
//申请帐号
void UILayerLogin::OnClicked_CreateAccount(void*, void*)
{
    guard;

    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage==NULL) szHomepage = "http://www.wtgame.net";
    std::string szCmd = "explorer.exe ";
    szCmd += szHomepage;

    ShowWindow(GetWndHandle(), SW_MINIMIZE);
    WinExec(szCmd.c_str(), SW_SHOW);

    unguard;
}

void UILayerLogin::OnKey(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	if (e->type == etKeyChar && e->key.code == vkTab)
	{
		sender->NextTabStop();
	}
    unguard;
}

void UILayerLogin::OnHideKeyboard(void*, void*)
{
	guard;
	if (m_Keyboard.IsVisible())
	{
		m_Keyboard.HideSoftKeyboard();
	}
	unguard;
}

void UILayerLogin::OnShowKeyboard(void*, void*)
{
	guard;
	m_Keyboard.ShowSoftKeyboard();
	unguard;
}
void UILayerLogin::ontab(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(GetLogin()->GetStatus() == GcLogin::GLS_LOGIN)
	{
		if (e->key.code == vkTab || (e->key.code == vkTab && e->key.code == vkShift))
		{	
			if (m_username->GetQualifiedID()==sender->GetQualifiedID())
			{
				m_Keyboard.ShowSoftKeyboard();
				g_workspace.SetFocusWidget(m_password);
			}else
			{
				if (m_Keyboard.IsVisible())
				{
					m_Keyboard.HideSoftKeyboard();
				}
				g_workspace.SetFocusWidget(m_username);
			}

		}
	}
	unguard;
}

void UILayerLogin::OnUpdateText(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	OnClicked_Login(NULL,NULL);
    unguard;
}

void UILayerLogin::OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	/* gao 2010.1.14
	当前函数没有用，所以在此注释
	防止游戏中出现意外错误
	*/
    /*if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_GAMEWORLD_SERVER)
    {
        int i;
        for (i=0; i<MAX_BUTTON; i++)
        {
            if (m_pServerListButton[i]==sender)
            {
                m_iCurrentSelectServer = i+1;
                break;
            }
        }
        for (i=0; i<MAX_BUTTON; i++)
        {
            if (i!=(m_iCurrentSelectServer-1))
            {
                m_pServerListButton[i]->SetChecked(false);
            }
        }
    }*/
	/* end */
    unguard;
}

void UILayerLogin::OnClicked_EnterLogin(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	/* gao 2010.1.14
	此处，服务器列表由按钮改为列表
	原来的代码，暂时注释
	*/
    if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_GAMEWORLD_SERVER)
    {
        int i;
        for (i=0; i<MAX_BUTTON; i++)
        {
            if (m_pServerListButton[i]==sender)
            {
                m_iCurrentSelectServer = i+1;
                break;
            }
        }
        if (m_iCurrentSelectServer>=1 && m_iCurrentSelectServer<=MAX_BUTTON)
        {
            GetLogin()->SelectGameWorld(m_iCurrentSelectServer-1);
        }
    }
	// 新代码
	//m_iCurrentSelectServer = g_layerLogin->m_formServer->GetSelectItem() + 1;
	//if (m_iCurrentSelectServer>=1 && m_iCurrentSelectServer<=g_layerLogin->m_formServer->GetMaxLineNum())
	//{
	//	GetLogin()->SelectGameWorld(m_iCurrentSelectServer-1);
	//}
	//else
	//{
	//	ShowMessage(R(LMSG_SELECT_SERVERS));
	//}
	/* end */

    unguard;
}


void UILayerLogin::OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	if (m_pBtnSaveAccount->GetChecked())
	{
		GetLogin()->SetSaveAccount(true);
	} 
	else
	{
		GetLogin()->SetSaveAccount(false);
	}

	if (m_Keyboard.GetLastFocus())
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmusername.txtusername"));
	} 
	else
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmpassword.txtpassword"));
	}

	RtIni iniUser;
	if (!iniUser.OpenFile(R(INI_USER), true))
	{
		return;
	}
	long iSave = GetLogin()->GetSaveAccount();

	if (!iniUser.FindSection("account")) iniUser.AddSection("account");
	if (!iniUser.SetEntry("account", "save", iSave))
	{
		iniUser.AddEntry("account", "save", iSave);
	}

	iniUser.CloseFile();
    unguard;
}

void UILayerLogin::OnClicked_Keyboard(void*, void*)
{
	guard;
	if (m_Keyboard.GetLastFocus())
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmusername.txtusername"));
	} 
	else
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmpassword.txtpassword"));
	}

	if (m_Keyboard.IsVisible())
	{
		m_Keyboard.HideSoftKeyboard();
	} 
	else
	{
		m_Keyboard.ShowSoftKeyboard();
	}
	unguard;
}
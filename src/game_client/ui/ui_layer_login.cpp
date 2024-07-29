#include "gc_include.h"
#include "ui_layer.h"
//#include "ui_form_msg.h"
#include "gc_login.h"

UILayerLogin::UILayerLogin() {
    guard;
    //lyymark 2.GcLogin.XML 加载用户登录UI
    g_workspace.Load("ui\\ui_layer_login.xml");
    //用户登录form
    mp_loginForm = LOAD_UI_T(RtwForm, "loginForm");

    mp_selectServerName = LOAD_UI("loginForm.selectServerName");

    mp_txtAccout = LOAD_UI_T(RtwTextBox, "loginForm.txtAccout");

    mp_txtPwd = LOAD_UI_T(RtwTextBox, "loginForm.txtPwd");

    mp_ckSaveAcc = LOAD_UI_T(CUiCheckButton, "loginForm.ckSaveAcc.");

    mp_btnOk = LOAD_UI_T(RtwButton, "loginForm.btnOk");
    mp_btnBack = LOAD_UI_T(RtwButton, "loginForm.btnBack");

    //用户名输入框
    mp_txtAccout->EvKeyChar += RTW_CALLBACK(this, UILayerLogin, ontab);
    mp_txtAccout->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));

    //设置用户输入框输入长度上限
    mp_txtAccout->SetCapacity(20);
    mp_txtAccout->SetFocus();

    //密码输入框
    mp_txtPwd->EvKeyChar += RTW_CALLBACK(this, UILayerLogin, ontab);
    mp_txtPwd->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));
    //设置密码输入框输入长度上限
    mp_txtPwd->SetCapacity(30);

    //单选框是否记住用户名
    mp_ckSaveAcc->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_SaveAccount));

    //申请帐号
    LOAD_UI("btncreate")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_CreateAccount));

    //退出按钮
    LOAD_UI("btnexit")->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_Quit));

    //强制登录
    LOAD_UI("ConfirmBox2.btnconfirm")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLogin));
    LOAD_UI("ConfirmBox2.btncancel")
        ->EvLClick.ClearAndSetDelegate(
            RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLoginCancel));

    //返回按钮
    /*  LOAD_UI("btnback")->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_BackSelectServer));*/

    //lyymark 2.GcLogin.XML 加载服务器列表UI
    m_formServer = RT_NEW CUIForm_Server;
    //退出
    LOAD_UI("btnexit")->Show();
    //忘记密码
    // LOAD_UI("btnforgetpwd")->Show();
    //申请帐号
    LOAD_UI("btncreate")->Show();
    //隐藏 返回
    //LOAD_UI("btnback")->Hide();

    unguard;
}

UILayerLogin::~UILayerLogin() {
    guard;
    if (m_formServer) {
        DEL_ONE(m_formServer);
    }

    unguard;
}

void UILayerLogin::Show() {

    mp_loginForm->Show();
    mp_loginForm->SetShowCloseButton(false);
}

void UILayerLogin::Hide() {
    mp_loginForm->Hide();
}

void UILayerLogin::OnClicked_BackSelectServer(RtwWidget* sender, void*) {
    guard;
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        GetLogin()->SetLoginState(GcLogin::GLS_SELECT_GAMEWORLD_SERVER);
    }
    unguard;
}

bool IsValidAccountName(const string& name) {
    if (name.length() <= 0 || name.length() > 40) {
        return false;
    }
    const char* p = name.c_str();
    const char* stop = p + name.length();
    while (p < stop) {
        if (!((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) {
            if ((*p == '@') || (*p == '-') || (*p == '_') || (*p == '.')) {

            } else {
                return false;
            }
        }
        p++;
    }
    return true;
}

//游戏客户端登录按钮的响应事件
void UILayerLogin::OnClicked_Login(void*, void*) {
    guard;

    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        if (mp_txtAccout->GetText().length() == 0) {
            ShowMessage(R(MSG_USERNAME_CANNOT_NULL));
            return;
        }

        std::string str = mp_txtAccout->GetText().substr(0, 1);
        const char* pFirst = str.c_str();
        if (!((*pFirst >= 'A' && *pFirst <= 'Z') || (*pFirst >= 'a' && *pFirst <= 'z'))) {
            ShowMessage(R(MSG_USERNAME_INVALID));
            return;
        }
        if (!strncmp(mp_txtAccout->GetText().c_str(), "test", strlen("test")) == 0 &&
            !strcmp(mp_txtAccout->GetText().c_str(), "admin") == 0) {
            if (mp_txtAccout->GetText().length() < 4 || mp_txtAccout->GetText().length() > 20) {
                ShowMessage(R(MSG_USERNAME_INVALID_LONG));
                return;
            }
        }
        if (!IsValidAccountName(mp_txtAccout->GetText())) {
            ShowMessage(R(MSG_USERNAME_INVALID));
            return;
        }
        if (mp_txtPwd->GetText().length() == 0) {
            ShowMessage(R(MSG_PASSWORD_CANNOT_NULL));
            return;
        }
        if (!strcmp(mp_txtPwd->GetText().c_str(), "123456") == 0) {
            if (mp_txtPwd->GetText().length() < 7 || mp_txtPwd->GetText().length() > 30) {
                ShowMessage(R(MSG_PASSWORD_INVALID_LONG));
                return;
            }
        }

        GetLogin()->Login((char*)mp_txtAccout->GetText().c_str(),
                          (char*)mp_txtPwd->GetText().c_str());

        // 保存帐号
        RtIni iniUser;
        if (!iniUser.OpenFile(R(INI_USER), true)) {
            return;
        }

        if (mp_ckSaveAcc->GetChecked()) {
            if (!iniUser.FindSection("login"))
                iniUser.AddSection("login");
            if (!iniUser.SetEntry("login", "username", mp_txtAccout->GetText().c_str())) {
                iniUser.AddEntry("login", "username", mp_txtAccout->GetText().c_str());
            }
        } else {
            iniUser.DelEntry("login", "username");
        }

        iniUser.CloseFile();
    }

    unguard;
}

//强制登录按钮的响应事件
void UILayerLogin::OnClicked_ForceLogin(void*, void*) {
    GetLogin()->SetForceLogin(true);
    GetLogin()->OnNetConnect(true);
}

void UILayerLogin::OnClicked_ForceLoginCancel(void*, void*) {
    LOAD_UI("fmlogin.fmpassword.txtpassword")->SetText("");
    GetLogin()->SetForceLogin(false);
}

void UILayerLogin::OnClicked_Quit(void*, void*) {
    guard;

    PostQuitMessage(0);

    unguard;
}

//领取密保
void UILayerLogin::OnClicked_GetPassword(void*, void*) {
    guard;

    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    std::string szCmd = "explorer.exe ";
    szCmd += szHomepage;

    ShowWindow(GetWndHandle(), SW_MINIMIZE);
    WinExec(szCmd.c_str(), SW_SHOW);

    unguard;
}

//忘记密码
void UILayerLogin::OnClicked_ForgetPassword(void*, void*) {
    guard;

    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    std::string szCmd = "explorer.exe ";
    szCmd += szHomepage;

    ShowWindow(GetWndHandle(), SW_MINIMIZE);
    WinExec(szCmd.c_str(), SW_SHOW);

    unguard;
}

//申请帐号
void UILayerLogin::OnClicked_CreateAccount(void*, void*) {
    guard;

    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    std::string szCmd = "explorer.exe ";
    szCmd += szHomepage;

    ShowWindow(GetWndHandle(), SW_MINIMIZE);
    WinExec(szCmd.c_str(), SW_SHOW);

    unguard;
}

void UILayerLogin::OnKey(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (e->type == etKeyChar && e->key.code == vkTab) {
        sender->NextTabStop();
    }
    unguard;
}

void UILayerLogin::ontab(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        if (e->key.code == vkTab || (e->key.code == vkTab && e->key.code == vkShift)) {
            if (mp_txtAccout->GetQualifiedID() == sender->GetQualifiedID()) {
                g_workspace.SetFocusWidget(mp_txtPwd);
            } else {
                g_workspace.SetFocusWidget(mp_txtAccout);
            }
        }
    }
    unguard;
}

void UILayerLogin::OnUpdateText(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    OnClicked_Login(NULL, NULL);
    unguard;
}

//lyymark 2.GcLogin.UI.OnClicked_EnterUserLogin 进入用户登录页面
void UILayerLogin::OnClicked_EnterUserLogin(const int& currentSelectServerIndex) {
    guard;
    if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_GAMEWORLD_SERVER) {
        GetLogin()->SelectGameWorld(currentSelectServerIndex);
    }
    unguard;
}

void UILayerLogin::OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (mp_ckSaveAcc->GetChecked()) {
        GetLogin()->SetSaveAccount(true);
    } else {
        GetLogin()->SetSaveAccount(false);
    }

    /* if (m_Keyboard.GetLastFocus()) {
        g_workspace.SetFocusWidget(LOAD_UI("fmlogin.fmusername.txtusername"));
    } else {
        g_workspace.SetFocusWidget(LOAD_UI("fmlogin.fmpassword.txtpassword"));
    }*/

    RtIni iniUser;
    if (!iniUser.OpenFile(R(INI_USER), true)) {
        return;
    }
    long iSave = GetLogin()->GetSaveAccount();

    if (!iniUser.FindSection("account"))
        iniUser.AddSection("account");
    if (!iniUser.SetEntry("account", "save", iSave)) {
        iniUser.AddEntry("account", "save", iSave);
    }

    iniUser.CloseFile();
    unguard;
}

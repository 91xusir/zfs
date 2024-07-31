#include "gc_include.h"
#include "ui_layer.h"
#include "gc_login.h"
#include "ui_form_msg.h"

void global_closeApp() {

    UIFormMsg* pConfirm =
        UIFormMsg::ShowStatic("ȷ���˳���Ϸ��?", UIFormMsg::TYPE_OK_CANCEL, true, "game_exit");
    pConfirm->EvOK.ClearAndSetDelegate(RTW_CALLBACK(NULL, UILayerLogin, OnClicked_Quit));
    pConfirm->EvCancel.Clear();
}

static bool IsValidAccountName(const string& name) {
    if (name.empty() || name.length() > 40) {
        return false;
    }
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}

UILayerLogin::UILayerLogin() {
    //lyymark 2.GcLogin.XML �����û���¼UI
    g_workspace.Load("ui\\ui_layer_login.xml");
    //�û���¼form
    mp_loginForm = LOAD_UI_T(RtwForm, "loginForm");
    mp_selectServerName = LOAD_UI("loginForm.selectServerName");
    //�û��������
    mp_txtAccout = LOAD_UI_T(RtwTextBox, "loginForm.txtAccout");
    mp_txtAccout->EvKeyChar += RTW_CALLBACK(this, UILayerLogin, onTab);
    mp_txtAccout->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));
    mp_txtAccout->SetCapacity(20);  //�������볤������
    mp_txtAccout->SetFocus();

    mp_txtPwd = LOAD_UI_T(RtwTextBox, "loginForm.txtPwd");
    mp_txtPwd->EvKeyChar += RTW_CALLBACK(this, UILayerLogin, onTab);
    mp_txtPwd->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnUpdateText));
    mp_txtPwd->SetCapacity(30);

    //��ѡ���Ƿ��ס�û���
    mp_ckSaveAcc = LOAD_UI_T(CUiCheckButton, "loginForm.ckSaveAcc.");
    mp_ckSaveAcc->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_SaveAccount));

    //��¼
    LOAD_UI("loginForm.btnOk")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_Login));
    //����
    LOAD_UI("loginForm.btnBack")
        ->EvLClick.ClearAndSetDelegate(
            RTW_CALLBACK(this, UILayerLogin, OnClicked_BackSelectServer));

    //�̶���ʾ��ť
    //�����ʺ�
    LOAD_UI("btnCreate")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_CreateAccount));
    //����
    LOAD_UI("btnWeb")->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_CreateAccount));

    //�˳���ť
    LOAD_UI("btnExit")->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_Quit));

 /*   UIFormMsg* pConfirm = UIFormMsg::ShowStatic("���Ѿ��ڱ�ĵط�����,�Ƿ�ǿ�Ƶ�¼?",
                                                UIFormMsg::TYPE_OK_CANCEL, true, "force_login");
    pConfirm->EvOK.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLogin));
    pConfirm->EvCancel.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILayerLogin, OnClicked_ForceLoginCancel));*/

    //lyymark 2.GcLogin.XML ���ط������б�UI
    mp_layerServer = RT_NEW UILayerServer;
}

UILayerLogin::~UILayerLogin() {
    if (mp_layerServer) {
        DEL_ONE(mp_layerServer);
    }
}

void UILayerLogin::Show() {
    mp_loginForm->Show();
    mp_loginForm->SetShowCloseButton(false);
}

void UILayerLogin::Hide() {
    mp_loginForm->Hide();
}

void UILayerLogin::OnClicked_BackSelectServer(RtwWidget* sender, void*) {
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        GetLogin()->SetLoginState(GcLogin::GLS_SELECT_GAMEWORLD_SERVER);
    }
}

//��Ϸ�ͻ��˵�¼��ť����Ӧ�¼�
void UILayerLogin::OnClicked_Login(void*, void*) {

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

        // �����ʺ�
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
}

//ǿ�Ƶ�¼��ť����Ӧ�¼�
void UILayerLogin::OnClicked_ForceLogin(void*, void*) {
    GetLogin()->SetForceLogin(true);
    GetLogin()->OnNetConnect(true);
}

void UILayerLogin::OnClicked_ForceLoginCancel(void*, void*) {
    LOAD_UI("loginForm.txtPwd")->SetText("");
    GetLogin()->SetForceLogin(false);
}

void UILayerLogin::OnClicked_Quit(void*, void*) {
    PostQuitMessage(0);
}

//��ȡ�ܱ�
void UILayerLogin::OnClicked_GetPassword(void*, void*) {
    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    ShellExecute(0, 0, szHomepage, 0, 0, SW_SHOW);
}

//��������
void UILayerLogin::OnClicked_ForgetPassword(void*, void*) {
    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    ShellExecute(0, 0, szHomepage, 0, 0, SW_SHOW);
}

//�����ʺ�
void UILayerLogin::OnClicked_CreateAccount(void*, void*) {
    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "http://www.wtgame.net";
    ShellExecute(0, 0, szHomepage, 0, 0, SW_SHOW);
}

void UILayerLogin::onTab(RtwWidget* sender, RtwEventDelegate* e) {
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        if (e->key.code == vkTab) {
            if (mp_txtAccout->GetQualifiedID() == sender->GetQualifiedID()) {
                g_workspace.SetFocusWidget(mp_txtPwd);
            } else {
                g_workspace.SetFocusWidget(mp_txtAccout);
            }
        }
    }
}

void UILayerLogin::OnUpdateText(RtwWidget* sender, RtwEventDelegate* e) {
    OnClicked_Login(NULL, NULL);
}

//lyymark 2.GcLogin.UI.OnClicked_EnterUserLogin �����û���¼ҳ��
void UILayerLogin::OnClicked_EnterUserLogin(const int& currentSelectServerIndex) {
    if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_GAMEWORLD_SERVER) {
        GetLogin()->SelectGameWorld(currentSelectServerIndex);
    }
}

// �����ʺ�
void UILayerLogin::OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e) {
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
}

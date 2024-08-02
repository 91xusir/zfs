#include "gc_include.h"
#include "ui_layer.h"
#include "gc_login.h"
#include "ui_form_msg.h"
#include <algorithm>
#include <cctype>

void global_closeApp(void*, void*, void*) {

    UIFormMsg* pConfirm = UIFormMsg::ShowStatic("ȷ���˳���Ϸ��?", UIFormMsg::TYPE_OK_CANCEL);
    pConfirm->EvOK+=(UI_DELEGATE_F(UILayerLogin::OnClicked_Quit));
}

static bool IsValidAccountName(const string& name) {
    if (name.length() < 4 || name.length() > 20 || name == "admin" || !std::isalpha(name[0])) {
        return false;
    }
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}

UILayerLogin::UILayerLogin() {
    //lyymark 2.GcLogin.XML �����û���¼UI
    g_workspace.Load("ui/1.login.xml");
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
    LOAD_UI("btnExit")->EvLClick.ClearAndSetDelegate(UI_DELEGATE_F(global_closeApp));

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
    // ��ȡ��¼״̬
    if (GetLogin()->GetStatus() != GcLogin::GLS_LOGIN) {
        return;
    }
    const std::string username = mp_txtAccout->GetText();
    const std::string password = mp_txtPwd->GetText();

    // ��֤�˻�
    if (username.empty()) {
        ShowMessage(R(MSG_USERNAME_CANNOT_NULL));
        return;
    }
    if (!IsValidAccountName(username)) {
        ShowMessage(R(MSG_USERNAME_INVALID));
        return;
    }
    // ��֤����
    if (password.empty()) {
        ShowMessage(R(MSG_PASSWORD_CANNOT_NULL));
        return;
    }
    if (password.length() < 6 || password.length() > 20) {
        ShowMessage(R(MSG_PASSWORD_INVALID_LONG));
        return;
    }

    // ִ�е�¼
    GetLogin()->Login(username, password);

    // �����ʺ�
    RtIni iniUser;
    if (iniUser.OpenFile(R(INI_USER), true)) {
        if (mp_ckSaveAcc->GetChecked()) {
            if (!iniUser.FindSection("login")) {
                iniUser.AddSection("login");
            }
            iniUser.SetEntry("login", "username", username.c_str());
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

//�����ʺ�
void UILayerLogin::OnClicked_CreateAccount(void*, void*) {
    const char* szHomepage = GetGameIni()->GetEntry("net", "HomePage");
    if (szHomepage == NULL)
        szHomepage = "https://gitee.com/chuangxxt/zfs";
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

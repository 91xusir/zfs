#include "ui_login_account.h"
#include <uimain.h>
#include <string_mapping.h>



UILoginAccount::UILoginAccount() {
    //�û���¼form
    mp_loginForm = LOAD_UI_T(RtwForm, "loginForm");

    //��ǰѡ��ķ�����
    mp_selectServerName = LOAD_UI("loginForm.selectServerName");

    //�û��������
    mp_txtAccout = LOAD_UI_T(RtwTextBox, "loginForm.txtAccout");
    mp_txtAccout->EvKeyChar += UI_DELEGATE_M(this, UILoginAccount, onTab);
    mp_txtAccout->EvUpdateText.ClearAndSetDelegate(UI_DELEGATE_M(this, UILoginAccount, OnUpdateText));
    mp_txtAccout->SetCapacity(20);  //�����û���������볤������
    mp_txtAccout->SetFocus();

    //���������
    mp_txtPwd = LOAD_UI_T(RtwTextBox, "loginForm.txtPwd");
    mp_txtPwd->EvKeyChar += RTW_CALLBACK(this, UILoginAccount, onTab);
    mp_txtPwd->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILoginAccount, OnUpdateText));
    mp_txtPwd->SetCapacity(30);

     //��ѡ���Ƿ��ס�û���
    mp_ckSaveAcc = LOAD_UI_T(CUiCheckButton, "loginForm.ckSaveAcc.");
    mp_ckSaveAcc->EvLClick.ClearAndSetDelegate(
        RTW_CALLBACK(this, UILoginAccount, OnClicked_SaveAccount));

    //ȷ���ͷ��ذ�ť
    mp_btnOk = LOAD_UI_T(RtwButton, "loginForm.btnOk");

    mp_btnOk->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILoginAccount, OnClicked_Login));

    mp_btnBack = LOAD_UI_T(RtwButton, "loginForm.btnBack");

}

UILoginAccount::~UILoginAccount() {}

void UILoginAccount::Show() {

    mp_loginForm->Show();
    mp_loginForm->SetShowCloseButton(false);
}

void UILoginAccount::Hide() {
    mp_loginForm->Hide();
}


void UILoginAccount::OnClicked_BackSelectServer(RtwWidget* sender, void*) {
   
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        GetLogin()->SetLoginState(GcLogin::GLS_SELECT_GAMEWORLD_SERVER);
    }
    
}


static bool IsValidAccountName(const std::string& name) {
    if (name.empty() || name.length() > 40) {
        return false;
    }
    const char* p = name.c_str();
    const char* stop = p + name.length();
    const char* special_chars = "@-_.";
    while (p < stop) {
        if (!std::isalnum(*p) && !std::strchr(special_chars, *p)) {
            return false;
        }
        p++;
    }
    return true;
}

//��Ϸ�ͻ��˵�¼��ť����Ӧ�¼�
void UILoginAccount::OnClicked_Login(void*, void*) {
   

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
void UILoginAccount::OnClicked_ForceLogin(void*, void*) {
    GetLogin()->SetForceLogin(true);
    GetLogin()->OnNetConnect(true);
}

void UILoginAccount::OnClicked_ForceLoginCancel(void*, void*) {
    LOAD_UI("fmlogin.fmpassword.txtpassword")->SetText("");
    GetLogin()->SetForceLogin(false);
}




void UILoginAccount::OnKey(RtwWidget* sender, RtwEventDelegate* e) {
   
    if (e->type == etKeyChar && e->key.code == vkTab) {
        sender->NextTabStop();
    }
    
}

void UILoginAccount::onTab(RtwWidget* sender, RtwEventDelegate* e) {
   
    if (GetLogin()->GetStatus() == GcLogin::GLS_LOGIN) {
        if (e->key.code == vkTab || (e->key.code == vkTab && e->key.code == vkShift)) {
            if (mp_txtAccout->GetQualifiedID() == sender->GetQualifiedID()) {
                g_workspace.SetFocusWidget(mp_txtPwd);
            } else {
                g_workspace.SetFocusWidget(mp_txtAccout);
            }
        }
    }
    
}

void UILoginAccount::OnUpdateText(RtwWidget* sender, RtwEventDelegate* e) {
   
    OnClicked_Login(NULL, NULL);
    
}

//lyymark 2.GcLogin.UI.OnClicked_EnterUserLogin �����û���¼ҳ��
void UILoginAccount::OnClicked_EnterUserLogin(const int& currentSelectServerIndex) {
   
    if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_GAMEWORLD_SERVER) {
        GetLogin()->SelectGameWorld(currentSelectServerIndex);
    }
    
}

void UILoginAccount::OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e) {
   
    if (mp_ckSaveAcc->GetChecked()) {
        GetLogin()->SetSaveAccount(true);
    } else {
        GetLogin()->SetSaveAccount(false);
    }

    /* if (m_Keyboard.GetLastFocus()) {
        g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmusername.txtusername"));
    } else {
        g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmpassword.txtpassword"));
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

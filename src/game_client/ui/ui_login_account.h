#pragma once
#include "ui/rtw_ui.h"

/* fox by lyy 2024.7.28
	������ѡ�� �Լ� �û���¼���� �߼�����
*/

class UILoginAccount {
   public:
    UILoginAccount();
    ~UILoginAccount();
    void Show();
    void Hide();

    //ǿ�Ƶ�¼��ť����Ӧ�¼�
    void OnClicked_ForceLogin(void*, void*);

    void OnClicked_ForceLoginCancel(void*, void*);

    void OnClicked_EnterUserLogin(const int& serverId);

    //save account to file
    void OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e);

   private:
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

    void OnKey(RtwWidget* sender, RtwEventDelegate*);

    void onTab(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_BackSelectServer(RtwWidget* sender, void*);

    void OnClicked_Login(void*, void*);

   public:
    //----------------------------------fox by lyy 2024.7.28
    RtwForm* mp_loginForm;

    RtwWidget* mp_selectServerName;

    RtwTextBox* mp_txtAccout;

    RtwTextBox* mp_txtPwd;

    CUiCheckButton* mp_ckSaveAcc;

    RtwButton* mp_btnOk;
    RtwButton* mp_btnBack;
};

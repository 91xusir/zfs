#ifndef _INC_UI_LOGIN_LAYER_H_
#define _INC_UI_LOGIN_LAYER_H_

#include "ui_form_setting.h"
#include "soft_keyboard.h"

class CUIForm_Server;

/* fox by lyy 2024.7.28
	������ѡ�� �Լ� ��¼����
*/
class UILayerLogin {
   public:
    UILayerLogin();

    ~UILayerLogin();

   private:
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

    void OnKey(RtwWidget* sender, RtwEventDelegate*);

    void OnHideKeyboard(void*, void*);

    void OnShowKeyboard(void*, void*);

    void ontab(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_BackSelectServer(RtwWidget* sender, void*);

    void OnClicked_Login(void*, void*);

   public:
    //ǿ�Ƶ�¼��ť����Ӧ�¼�
    void OnClicked_ForceLogin(void*, void*);

    void OnClicked_ForceLoginCancel(void*, void*);

    void OnClicked_EnterUserLogin(const int& serverId);

   private:
    //�˳�
    void OnClicked_Quit(void*, void*);
    //��ȡ�ܱ�
    void OnClicked_GetPassword(void*, void*);
    //��������
    void OnClicked_ForgetPassword(void*, void*);
    //�����ʺ�
    void OnClicked_CreateAccount(void*, void*);
    //�����ʺ�
    void OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e);
    //�������
    void OnClicked_Keyboard(void*, void*);

   private:
    CSoftKeyboard m_Keyboard;

   public:
    constexpr static int MaxBtnsCounts = 8;//���÷����������ʾ����
    RtwTextBox*          m_username;
    RtwTextBox*          m_password;
    CUiCheckButton*      m_pBtnSaveAccount;
    CUIForm_Server* m_formServer;
};

class CUIForm_Server {
   public:
    CUIForm_Server(void);
    ~CUIForm_Server(void);
    void Show();
    void Hide();
    void setTips(const std::string& tips);
    void showTips();
    void OnInsertNewServer(const int index, const std::string serverName, int ping,
                           const std::string serverState);
    void OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e);

   private:
    //lyymark 2.GcLogin.UI.Server
    /*-----------------------------------------------add by lyy*/
    RtwWidget*                          mp_serverForm;
    RtwWidget*                          mp_serverTips;
    RtwWidget*                          mp_btnServersList[UILayerLogin::MaxBtnsCounts]{};
    std::unordered_map<RtwWidget*, int> mp_btnServersSelectedMap{};
    RtwWidget*                          mp_serverStatus[UILayerLogin::MaxBtnsCounts]{};

   public:
};

#endif  // _INC_UI_LOGIN_LAYER_H_

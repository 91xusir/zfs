#ifndef _INC_UI_LOGIN_LAYER_H_
#define _INC_UI_LOGIN_LAYER_H_

#include "ui_form_setting.h"
#include "soft_keyboard.h"

class CUIForm_Server;

/* fox by lyy 2024.7.28
	服务器选择 以及 登录界面
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
    //强制登录按钮的响应事件
    void OnClicked_ForceLogin(void*, void*);

    void OnClicked_ForceLoginCancel(void*, void*);

    void OnClicked_EnterUserLogin(const int& serverId);

   private:
    //退出
    void OnClicked_Quit(void*, void*);
    //领取密保
    void OnClicked_GetPassword(void*, void*);
    //忘记密码
    void OnClicked_ForgetPassword(void*, void*);
    //申请帐号
    void OnClicked_CreateAccount(void*, void*);
    //保存帐号
    void OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e);
    //打开软键盘
    void OnClicked_Keyboard(void*, void*);

   private:
    CSoftKeyboard m_Keyboard;

   public:
    constexpr static int MaxBtnsCounts = 8;//设置服务器最大显示个数
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

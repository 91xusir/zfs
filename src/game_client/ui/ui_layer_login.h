#ifndef _INC_UI_LOGIN_LAYER_H_
#define _INC_UI_LOGIN_LAYER_H_

#include "ui_form_setting.h"


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


  

   public:
    constexpr static int MaxBtnsCounts = 8;  //设置服务器最大显示个数
    //RtwTextBox*          mp_txtAccout;
    //RtwTextBox*          mp_txtPwd;
    //CUiCheckButton*      m_pBtnSaveAccount; 
    CUIForm_Server*      m_formServer;
    //----------------------------------fox by lyy 2024.7.28
    RtwForm* mp_loginForm;

    RtwWidget* mp_selectServerName;

    RtwTextBox* mp_txtAccout;

    RtwTextBox* mp_txtPwd;

    CUiCheckButton* mp_ckSaveAcc;

    RtwButton* mp_btnOk;
    RtwButton* mp_btnBack;
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
    RtwForm*                            mp_serverForm;
    RtwWidget*                          mp_serverTips;
    RtwButton*                          mp_btnServersList[UILayerLogin::MaxBtnsCounts]{};
    std::unordered_map<RtwButton*, int> mp_btnServersSelectedMap{};
    RtwWidget*                          mp_serverStatus[UILayerLogin::MaxBtnsCounts]{};

   public:
};

#endif  // _INC_UI_LOGIN_LAYER_H_

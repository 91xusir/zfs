#include <unordered_map>
#include <string>
#ifndef _INC_UI_LOGIN_LAYER_H_
#define _INC_UI_LOGIN_LAYER_H_

class UILayerServer;

class UILayerLogin {
   public:
    UILayerLogin();

    ~UILayerLogin();

    void Show();

    void Hide();

   private:
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

    void onTab(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_BackSelectServer(RtwWidget* sender, void*);

    void OnClicked_Login(void*, void*);

   public:
    //ǿ�Ƶ�¼��ť����Ӧ�¼�
    void OnClicked_ForceLogin(void*, void*);

    void OnClicked_ForceLoginCancel(void*, void*);

    void OnClicked_EnterUserLogin(const int& serverId);
    //�˳�
    void OnClicked_Quit(void*, void*);

   private:

    //�����ʺ�
    void OnClicked_CreateAccount(void*, void*);
    //�����ʺ�
    void OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e);

   public:
    //----------------------------------fix by lyy 2024.7.28
    constexpr static int MaxBtnsCounts = 8;  //���÷����������ʾ����

    UILayerServer*  mp_layerServer;
    RtwWidget*      mp_selectServerName;
    RtwForm*        mp_loginForm;
    RtwTextBox*     mp_txtAccout;
    RtwTextBox*     mp_txtPwd;
    CUiCheckButton* mp_ckSaveAcc;
};

class UILayerServer {
   public:
    UILayerServer();
    ~UILayerServer();
    void Show();
    void Hide();
    void setTips(const std::string& tips);
    void showTips();
    void OnInsertNewServer(const int& index, const std::string& serverName, const int& ping,
                           const std::string& serverState);
    void OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e);

   private:
    //lyymark 2.GcLogin.UI.Server
    /*-----------------------------------------------add by lyy*/
    struct ServerInfo {
        int        index;
        bool       isValid;
        RtwWidget* pStatus;
    };

    std::unordered_map<RtwButton*, ServerInfo> buttonToServerInfoMap{};
    RtwForm*                                   mp_serverForm;
    RtwWidget*                                 mp_serverTips;
};

#endif  // _INC_UI_LOGIN_LAYER_H_

#pragma once

class CUIForm_Server {
   public:
    CUIForm_Server(void);
    ~CUIForm_Server(void);
    void Show();
    void Hide();
    void Refresh();
    void IsVisible();
    void OnSelectServer(RtwWidget* sender, void* e);
    void OnInsertNewServer(const std::string serverName, int ping, const std::string serverState);
    void OnBtnOKDown(RtwWidget* sender, RtwEventDelegate* e);
    void SetSelectItemLast(int iLastSelect);
    RtwWidget* GetServerWidget();
    int        GetMaxLineNum();
    int        GetSelectItem();

   private:
    RtwWidget*  m_pfrmServer;
    RtwListBox* m_plstServerContent;
    RtwWidget*  m_plbSelectServer;
    RtwButton*  m_pbtnOK;
    int         m_iSelect;
    //lyymark 2.GcLogin.UI.Server
    /*-----------------------------------------------add by lyy*/
    RtwWidget* mp_serverForm;
    RtwWidget* mp_serverTips;
    RtwButton* mp_btnServers[8]{};
    RtwWidget* mp_serverStatus[8]{};
    int        activeBtnIndex;

   public:
};

#include "gc_include.h"
#include "ui_layer.h"
#include "UIForm_Server.h"

CUIForm_Server::CUIForm_Server(void) : m_iSelect(-1) {
    guard;
    m_pfrmServer = RTW_WIDGET("fmserver");
    m_plstServerContent = RTW_WIDGET_T(RtwListBox, "fmserver.fmxmm.lbxlogin");
    m_plbSelectServer = RTW_WIDGET("fmserver.fmxmm.lbtext");
    m_pbtnOK = RTW_WIDGET_T(RtwButton, "fmserver.fmxmm.btnok");
    m_plstServerContent->EvSelect += RTW_CALLBACK(this, CUIForm_Server, OnSelectServer);
    m_plstServerContent->EvLDClick += RTW_CALLBACK(this, CUIForm_Server, OnBtnOKDown);
    m_pbtnOK->EvLClick += RTW_CALLBACK(this, CUIForm_Server, OnBtnOKDown);

    /*-----------------------------------------------add by lyy*/
    mp_serverForm = LOAD_UI("serverForm");
    mp_serverTips = LOAD_UI("serverForm.serverTips");
    mp_btnServers[0] = LOAD_UI_T(RtwButton, "serverForm.btnServer1");
    mp_btnServers[1] = LOAD_UI_T(RtwButton, "serverForm.btnServer2");
    mp_btnServers[2] = LOAD_UI_T(RtwButton, "serverForm.btnServer3");
    mp_btnServers[3] = LOAD_UI_T(RtwButton, "serverForm.btnServer4");
    mp_btnServers[4] = LOAD_UI_T(RtwButton, "serverForm.btnServer5");
    mp_btnServers[5] = LOAD_UI_T(RtwButton, "serverForm.btnServer6");
    mp_btnServers[6] = LOAD_UI_T(RtwButton, "serverForm.btnServer7");
    mp_btnServers[7] = LOAD_UI_T(RtwButton, "serverForm.btnServer8");
    mp_serverStatus[0] = LOAD_UI("serverForm.serverStatus1");
    mp_serverStatus[1] = LOAD_UI("serverForm.serverStatus2");
    mp_serverStatus[2] = LOAD_UI("serverForm.serverStatus3");
    mp_serverStatus[3] = LOAD_UI("serverForm.serverStatus4");
    mp_serverStatus[4] = LOAD_UI("serverForm.serverStatus5");
    mp_serverStatus[5] = LOAD_UI("serverForm.serverStatus6");
    mp_serverStatus[6] = LOAD_UI("serverForm.serverStatus7");
    mp_serverStatus[7] = LOAD_UI("serverForm.serverStatus8");
    unguard;
}

CUIForm_Server::~CUIForm_Server(void) {
    guard;
    unguard;
}

void CUIForm_Server::Show() {
    guard;
    unguard;
}

void CUIForm_Server::Hide() {
    guard;
    m_pfrmServer->Hide();
    unguard;
}

void CUIForm_Server::Refresh() {
    guard;
    //m_plstServerContent->RemoveAllItems();
    for (auto btn : mp_btnServers) {
        btn->Hide();
    }
    unguard;
}

void CUIForm_Server::IsVisible() {
    guard;
    unguard;
}

void CUIForm_Server::OnSelectServer(RtwWidget* sender, void* e) {
    guard;
    int i = m_plstServerContent->GetSelectedItem();
    switch (i) {
        case -1:
            break;
        default:
            m_iSelect = i;
    }
    m_plbSelectServer->SetText(m_plstServerContent->GetItemText(i, 0));
    unguard;
}

void CUIForm_Server::OnInsertNewServer(const std::string serverName, int ping,
                                       const std::string serverState) {
    guard;
    // 确保按钮数组不为空
    if (mp_btnServers == nullptr) {
        return;  // 按钮数组未初始化
    }
    // 只处理当前按钮索引有效的情况
    if (activeBtnIndex >= 0 && activeBtnIndex < 8) {
        RtwButton* btn = mp_btnServers[activeBtnIndex];

        if (btn) {
            btn->SetText(serverName.c_str());  // 设置按钮文本
            btn->Show();                       // 显示按钮
        }
        // 更新索引到下一个按钮
        activeBtnIndex++;
    }
    unguard;
}

void CUIForm_Server::OnBtnOKDown(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    // 保存选中的服务器
    RtIni iniUser;
    if (!iniUser.OpenFile(R(INI_USER), true)) {
        return;
    }

    if (GetSelectItem() >= 0) {
        if (!iniUser.FindSection("server"))
            iniUser.AddSection("server");
        if (!iniUser.SetEntry("server", "select", (long)m_iSelect)) {
            iniUser.AddEntry("server", "select", (long)m_iSelect);
        }
    } else {
        iniUser.DelEntry("server", "select");
    }

    iniUser.CloseFile();

    g_layerLogin->OnClicked_EnterLogin(sender, e);
    unguard;
}

void CUIForm_Server::SetSelectItemLast(int iLastSelect) {
    guard;
    if (iLastSelect != -1 && iLastSelect <= m_plstServerContent->getLineCount())
        m_plstServerContent->SetSelect(iLastSelect);
    unguard;
}

RtwWidget* CUIForm_Server::GetServerWidget() {
    guard;
    return m_pfrmServer;
    unguard;
}

int CUIForm_Server::GetMaxLineNum() {
    // 	guard;
    return m_plstServerContent->getLineCount();
    // 	unguard;
}

int CUIForm_Server::GetSelectItem() {
    guard;
    return m_iSelect;
    unguard;
}

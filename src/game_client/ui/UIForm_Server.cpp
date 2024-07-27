#include "gc_include.h"
#include "ui_layer.h"

CUIForm_Server::CUIForm_Server(void) {
    guard;
    /*-----------------------------------------------add by lyy*/
    mp_serverForm = LOAD_UI("serverForm");
    mp_serverTips = LOAD_UI("serverForm.serverTips");
    setTips("服务器加载中...");
    showTips();
    mp_btnServersList[0] = LOAD_UI_T(RtwButton, "serverForm.btnServer0");
    mp_btnServersList[1] = LOAD_UI_T(RtwButton, "serverForm.btnServer1");
    mp_btnServersList[2] = LOAD_UI_T(RtwButton, "serverForm.btnServer2");
    mp_btnServersList[3] = LOAD_UI_T(RtwButton, "serverForm.btnServer3");
    mp_btnServersList[4] = LOAD_UI_T(RtwButton, "serverForm.btnServer4");
    mp_btnServersList[5] = LOAD_UI_T(RtwButton, "serverForm.btnServer5");
    mp_btnServersList[6] = LOAD_UI_T(RtwButton, "serverForm.btnServer6");
    mp_btnServersList[7] = LOAD_UI_T(RtwButton, "serverForm.btnServer7");
    // 填充按钮映射
    for (int i = 0; i < UILayerLogin::MaxBtnsCounts; ++i) {
        mp_btnServersSelectedMap[mp_btnServersList[i]] = i;
    }
    mp_serverStatus[0] = LOAD_UI("serverForm.serverStatus0");
    mp_serverStatus[1] = LOAD_UI("serverForm.serverStatus1");
    mp_serverStatus[2] = LOAD_UI("serverForm.serverStatus2");
    mp_serverStatus[3] = LOAD_UI("serverForm.serverStatus3");
    mp_serverStatus[4] = LOAD_UI("serverForm.serverStatus4");
    mp_serverStatus[5] = LOAD_UI("serverForm.serverStatus5");
    mp_serverStatus[6] = LOAD_UI("serverForm.serverStatus6");
    mp_serverStatus[7] = LOAD_UI("serverForm.serverStatus7");
    for (auto& btn : mp_btnServersList) {
        if (btn) {
            btn->EvLClick += UI_DELEGATE_M(this, CUIForm_Server, OnClicked_SelectServer);
        }
    }
    unguard;
}

CUIForm_Server::~CUIForm_Server(void) {
    guard;
    unguard;
}

void CUIForm_Server::Show() {
    guard;
    mp_serverForm->Show();
    unguard;
}

void CUIForm_Server::Hide() {
    guard;
    mp_serverForm->Hide();
    unguard;
}

void CUIForm_Server::setTips(const std::string& tips) {
    guard;
    if (mp_serverTips) {
        mp_serverTips->SetText(tips);
    }
    unguard;
}

void CUIForm_Server::showTips() {
    guard;
    if (mp_serverTips) {
        mp_serverTips->Show();
    }
    unguard;
}

void CUIForm_Server::OnInsertNewServer(const int index, const std::string serverName, int ping,
                                       const std::string serverState) {
    guard;
    setTips("请选择服务器:");

    // 只处理当前按钮索引有效的情况
    if (index >= 0 && index < UILayerLogin::MaxBtnsCounts) {

        auto& btn = mp_btnServersList[index];

        if (btn) {
            btn->SetText(serverName);  // 设置按钮文本
            btn->Show();               // 显示按钮
        }

        if (mp_serverStatus[index])
            mp_serverStatus[index]->SetText(serverState);
        mp_serverStatus[index]->Show();
    }
    unguard;
}

void CUIForm_Server::OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    auto& it = mp_btnServersSelectedMap.find(static_cast<RtwButton*>(sender));
    if (it != mp_btnServersSelectedMap.end()) {
        g_layerLogin->OnClicked_EnterUserLogin(it->second);
    }
    unguard;
}

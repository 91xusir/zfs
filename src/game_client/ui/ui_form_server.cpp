#include "gc_include.h"
#include "ui_layer.h"

UILayerServer::UILayerServer() {
    /*-----------------------------------------------add by lyy*/
    mp_serverForm = LOAD_UI_T(RtwForm, "serverForm");
    mp_serverTips = LOAD_UI("serverForm.serverTips");
    setTips("服务器加载中...");
    showTips();
    for (int index = 0; index < UILayerLogin::MaxBtnsCounts; index++) {
        std::string serverNameStr = "serverForm.btnServer" + std::to_string(index);
        std::string serverStatusStr = "serverForm.serverStatus" + std::to_string(index);
        const auto& btn = LOAD_UI_T(RtwButton, serverNameStr);
        const auto& status = LOAD_UI(serverStatusStr);
        buttonToServerInfoMap[btn] = {index, false, status};
    }
}

UILayerServer::~UILayerServer(void) {}

void UILayerServer::Show() {
    mp_serverForm->Show();
    mp_serverForm->SetShowCloseButton(false);
    for (const auto& [btn, info] : buttonToServerInfoMap) {
        if (!info.isValid) {
            btn->Hide();
            info.pStatus->Hide();
        }
    }
}

void UILayerServer::Hide() {
    mp_serverForm->Hide();
}

void UILayerServer::setTips(const std::string& tips) {
    if (mp_serverTips) {
        mp_serverTips->SetText(tips);
    }
}

void UILayerServer::showTips() {
    if (mp_serverTips) {
        mp_serverTips->Show();
    }
}

void UILayerServer::OnInsertNewServer(const int& index, const std::string& serverName,
                                      const int& ping, const std::string& serverState) {

    if (index < 0 || index >= UILayerLogin::MaxBtnsCounts)
        return;
    std::string serverNameStr = "serverForm.btnServer" + std::to_string(index);
    std::string serverStatusStr = "serverForm.serverStatus" + std::to_string(index);
    const auto& btn = LOAD_UI_T(RtwButton, serverNameStr);
    btn->EvLClick += UI_DELEGATE_M(this, UILayerServer, OnClicked_SelectServer);
    const auto& status = LOAD_UI(serverStatusStr);
    if (btn) {
        btn->SetText(serverName);
        btn->Show();
    }
    if (status) {
        status->SetText(serverState);
        status->SetTextColor(serverState == "流畅" ? 0xFF00FF00 : 0xFFFF0000);
        status->Show();
    }
    auto it = buttonToServerInfoMap.find(btn);
    if (it != buttonToServerInfoMap.end()) {
        it->second.isValid = true;
    }
    setTips("请选择服务器:");
}

void UILayerServer::OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e) {
    if (auto it = buttonToServerInfoMap.find(static_cast<RtwButton*>(sender));
        it != buttonToServerInfoMap.end()) {
        const auto& [btn, info] = *it;
        if (!info.isValid)
            return;
        g_layerLogin->OnClicked_EnterUserLogin(info.index);
    }
}

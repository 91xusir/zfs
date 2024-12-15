#include "gc_include.h"
#include "gc_login.h"
#include "ui_form_msg.h"
#include <nlohmann/json.hpp>
#include "ui_layer_select_char.h"
#include <ui/rtw_workspace.h>
#include "../HttpReq.h"
#include "../LyyUtils.h"
#include <vector>

/*
lyy 2024.8.6 重构
*/
UILayerSelectChar::UILayerSelectChar() {
    guard;
    g_workspace.Load("ui/2.selectChar.xml");
    //三个人物按钮
    for (std::size_t i = 0; i < MaxUserCharBtn; i++) {
        m_charBtnArray[i] = LOAD_UI_T(CUiCheckButton, "roleBtn.role" + std::to_string(i));
        m_charBtnArray[i]->EvLClick +=
            RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectChar, (void*)i);
    }
    //进入游戏
    LOAD_UI("enterBtn")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Enter));
    //返回上级 创建人物时有两阶段需要判断
    LOAD_UI("btnback")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
    //创建人物
    LOAD_UI("crtRoleBtn")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_EngterCreateChar);
    //删除人物
    LOAD_UI("delRoleBtn")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Delete);
    //创建人物角色界面
    LOAD_UI("fmShangZhou.btnshang")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)FACTION_SHANG);
    LOAD_UI("fmShangZhou.btnzhou")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)FACTION_ZHOU);
    m_usrRoleName = LOAD_UI_T(RtwTextBox, "fmCreatRole.txtRoleName");
    m_usrRoleName->SetCapacity(14);
    LOAD_UI("fmCreatRole.btnNameRand")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RandName);
    //人物角色发型
    LOAD_UI("fmCreatRole.btnHeadLeft")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_ChangeHide, (void*)0);
    LOAD_UI("fmCreatRole.btnHeadRight")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_ChangeHide, (void*)1);
    unguard;
}

UILayerSelectChar::~UILayerSelectChar() {}

//---------------选角界面----------------

//进入游戏
void UILayerSelectChar::OnClicked_Enter(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnSelectUserDone();
        }
    }
    unguard;
}

//返回按钮
void UILayerSelectChar::OnClicked_Back(void*, void*) {
    guard;
    const auto& gc_login = GetLogin();
    if (!gc_login)
        return;
    const auto& status = gc_login->GetStatus();
    if (status == GcLogin::GLS_CREATE_CHAR) {
        if (gc_login->m_selectFaction != GcLogin::None) {
            LOAD_UI("crtRoleBtn")->Disable();
            gc_login->SetSelectShangOrZhou(GcLogin::None);
        } else {
            gc_login->SetLoginState(GcLogin::GLS_SELECT_CHAR);
        }
    } else if (status == GcLogin::GLS_SELECT_CHAR) {
        gc_login->NetClose();
        gc_login->ClearSelectRoleOnce();
        gc_login->SetLoginState(GcLogin::GLS_LOGIN);
    }
    unguard;
}

//选择人物
void UILayerSelectChar::OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            int iSel = (int)e->param1;
            if (iSel == gc_login->m_curSelRoleIndex)
                return;
            gc_login->m_curSelRoleIndex = iSel;
            gc_login->UpdateSelectChar();
        }
    }
}

//创建人物
void UILayerSelectChar::OnClicked_EngterCreateChar(void*, void*) {
    const auto& gc_login = GetLogin();
    if (!gc_login)
        return;
    if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
        gc_login->SetLoginState(GcLogin::GLS_CREATE_CHAR);
    } else if (gc_login->GetStatus() == GcLogin::GLS_CREATE_CHAR &&
               gc_login->m_curCrtRoleCsvID > 0) {
        GetLogin()->OnCreateUser();
    }
}

//删除人物 begin
void UILayerSelectChar::OnClicked_Delete(void*, void*) {
    const auto& gc_login = GetLogin();
    if (!gc_login)
        return;
    if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {

        if (gc_login->m_curSelRoleIndex != -1) {
            gc_login->OnCharPasswordConfirm("");
        } else {
            UIFormMsg::ShowStatic(R(LMSG_PLS_CHOOSE_CHAR), UIFormMsg::TYPE_OK);
        }
    }
}

void UILayerSelectChar::OnReceivePasswordConfirm(char lRet) {
    const auto& gc_login = GetLogin();
    if (!gc_login)
        return;

    if (gc_login->GetStatus() != GcLogin::GLS_SELECT_CHAR)
        return;
    if (lRet != 1) {
        UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
        return;
    }
    if (gc_login->GetSelectUserWithPwd()) {
        gc_login->OnSelectUserWithPwd();
    } else {
        const GcAccountInfo& info = gc_login->GetAccountInfo();
        int                  iSel = gc_login->m_curSelRoleIndex;
        if (info.users[iSel].attributes.level >= 30) {
            //lyymark 等级大于30的角色要提示 这里先硬编码
            UIFormMsg* pFrm = UIFormMsg::ShowStatic("角色等级较高,确定删除吗?",
                                                    UIFormMsg::TYPE_OK_CANCEL);
            pFrm->EvOK = RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete, pFrm);
        } else {
            UIFormMsg* pFrm =
                UIFormMsg::ShowStatic(R(LMSG_CONFIRM_DELETE_CHAR), UIFormMsg::TYPE_OK_CANCEL);
            pFrm->EvOK = RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete, pFrm);
        }
    }
}

void UILayerSelectChar::OnConfirm_Delete(void*, void*) {
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnDeleteUser();
        }
    }
}

//删除人物 end

//人物左旋按钮
void UILayerSelectChar::OnClicked_LeftRotation(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnLeftRotation();
        }
    }
    unguard;
}

void UILayerSelectChar::OnClicked_LMouseDown(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnLeftRotationDown();
        }
    }
    unguard;
}

void UILayerSelectChar::OnClicked_LMouseUp(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnLeftRotationUp();
        }
    }
    unguard;
}

//人物右旋按钮
void UILayerSelectChar::OnClicked_RightRotation(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnRightRotation();
        }
    }
    unguard;
}

//---------------创建人物角色界面------------

//选择阵营
void UILayerSelectChar::OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            gc_login->SetSelectShangOrZhou((int)e->param1);
        }
    }
}

//随机昵称
void UILayerSelectChar::OnClicked_RandName(void*, void*) {
    HttpReq::getInstance()->asyncGet(
        "https://www.mxnzp.com/api/idiom/"
        "random?app_id=gclqdinyhywepkpo&app_secret=VUFtSDBXY3l6U2MremF3NDlxNUtzdz09",
        [this](const nlohmann::json& response) { this->OnSetRandName(response); });
}

void UILayerSelectChar::OnSetRandName(const nlohmann::json& json_response) {
    if (json_response.contains("error")) {
        m_usrRoleName->SetText("请不要点击过快");
        return;
    }
    std::string name = json_response["data"]["word"];
    m_usrRoleName->SetText(LyyUtils::Utf8ToGbk(name.c_str()));
}

template <typename T>
static inline void moveToLeft(std::vector<T>& vec) {
    if (!vec.empty()) {
        std::rotate(vec.begin(), vec.begin() + 1, vec.end());
    }
}

template <typename T>
static inline void moveToRight(std::vector<T>& vec) {
    if (!vec.empty()) {
        std::rotate(vec.rbegin(), vec.rbegin() + 1, vec.rend());
    }
}

//切换发型
void UILayerSelectChar::OnClicked_ChangeHide(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (!gc_login || gc_login->GetStatus() != GcLogin::GLS_CREATE_CHAR)
        return;
    const auto& curRoleCsvId = gc_login->m_curCrtRoleCsvID;
    auto&       headMap = gc_login->m_crtRole_csvIdMapHeads;
    if (!headMap[curRoleCsvId].size())
        return;
    const bool  left = (int)e->param1 == 0;
    const auto& ActIns = gc_login->m_crtRole_csvIdMapActIns[curRoleCsvId];
    const auto& head = headMap[curRoleCsvId][0];
    ActIns->UnloadSkin(head.skin.c_str());
    if (left)
        moveToLeft(gc_login->m_crtRole_csvIdMapHeads[gc_login->m_curCrtRoleCsvID]);
    else
        moveToRight(gc_login->m_crtRole_csvIdMapHeads[gc_login->m_curCrtRoleCsvID]);
    LOAD_UI("fmCreatRole.txtHead")->SetText(head.name);
    ActIns->LoadSkin(head.skin.c_str());
}

//----------------------废弃但保留--------------------
//设置密码
void UILayerSelectChar::OnReceiveSetPassword(char lRet) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            if (lRet == 0) {
                GetLogin()->GetAccountInfo().users[GetLogin()->m_curSelRoleIndex].hasCharPwd = true;
                GetLogin()->UpdateSelectChar();
                UIFormMsg::ShowStatic(R(MSG_CLIENT_SETPASSWORDSUCCESS), UIFormMsg::TYPE_OK);
            } else {
                UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
            }
        }
    }
    unguard;
}

//解除密码锁
void UILayerSelectChar::OnReceiveDelPassword(char lRet) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            if (lRet == 0) {
                GetLogin()->GetAccountInfo().users[GetLogin()->m_curSelRoleIndex].hasCharPwd =
                    false;
                GetLogin()->UpdateSelectChar();
                UIFormMsg::ShowStatic(R(MSG_CLIENT_REMOVEPASSWORDSUCCESS), UIFormMsg::TYPE_OK);
            } else {
                UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
            }
        }
    }
    unguard;
}

void UILayerSelectChar::OnUpdateText(RtwWidget* sender, RtwEventDelegate*) {}

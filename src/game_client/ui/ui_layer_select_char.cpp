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
lyy 2024.8.6 �ع�
*/
UILayerSelectChar::UILayerSelectChar() {
    guard;
    g_workspace.Load("ui/2.selectChar.xml");
    //�������ﰴť
    for (std::size_t i = 0; i < MaxUserCharBtn; i++) {
        m_charBtnArray[i] = LOAD_UI_T(CUiCheckButton, "roleBtn.role" + std::to_string(i));
        m_charBtnArray[i]->EvLClick +=
            RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectChar, (void*)i);
    }
    //������Ϸ
    LOAD_UI("enterBtn")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Enter));
    //�����ϼ� ��������ʱ�����׶���Ҫ�ж�
    LOAD_UI("btnback")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
    //��������
    LOAD_UI("crtRoleBtn")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_EngterCreateChar);
    //ɾ������
    LOAD_UI("delRoleBtn")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Delete);
    //���������ɫ����
    LOAD_UI("fmShangZhou.btnshang")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)FACTION_SHANG);
    LOAD_UI("fmShangZhou.btnzhou")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)FACTION_ZHOU);
    m_usrRoleName = LOAD_UI_T(RtwTextBox, "fmCreatRole.txtRoleName");
    m_usrRoleName->SetCapacity(14);
    LOAD_UI("fmCreatRole.btnNameRand")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RandName);
    //�����ɫ����
    LOAD_UI("fmCreatRole.btnHeadLeft")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_ChangeHide, (void*)0);
    LOAD_UI("fmCreatRole.btnHeadRight")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_ChangeHide, (void*)1);
    unguard;
}

UILayerSelectChar::~UILayerSelectChar() {}

//---------------ѡ�ǽ���----------------

//������Ϸ
void UILayerSelectChar::OnClicked_Enter(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnSelectUserDone();
        }
    }
    unguard;
}

//���ذ�ť
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

//ѡ������
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

//��������
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

//ɾ������ begin
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
            //lyymark �ȼ�����30�Ľ�ɫҪ��ʾ ������Ӳ����
            UIFormMsg* pFrm = UIFormMsg::ShowStatic("��ɫ�ȼ��ϸ�,ȷ��ɾ����?",
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

//ɾ������ end

//����������ť
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

//����������ť
void UILayerSelectChar::OnClicked_RightRotation(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnRightRotation();
        }
    }
    unguard;
}

//---------------���������ɫ����------------

//ѡ����Ӫ
void UILayerSelectChar::OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            gc_login->SetSelectShangOrZhou((int)e->param1);
        }
    }
}

//����ǳ�
void UILayerSelectChar::OnClicked_RandName(void*, void*) {
    HttpReq::getInstance()->asyncGet(
        "https://www.mxnzp.com/api/idiom/"
        "random?app_id=gclqdinyhywepkpo&app_secret=VUFtSDBXY3l6U2MremF3NDlxNUtzdz09",
        [this](const nlohmann::json& response) { this->OnSetRandName(response); });
}

void UILayerSelectChar::OnSetRandName(const nlohmann::json& json_response) {
    if (json_response.contains("error")) {
        m_usrRoleName->SetText("�벻Ҫ�������");
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

//�л�����
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

//----------------------����������--------------------
//��������
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

//���������
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

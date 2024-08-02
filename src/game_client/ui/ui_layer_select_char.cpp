#include "gc_include.h"
#include "gc_login.h"
#include "ui_form_msg.h"

UILayerSelectChar::UILayerSelectChar() {
    guard;
    g_workspace.Load("ui/2.selectChar.xml");
    //三个人物按钮
    for (std::size_t i = 0; i < MaxUserCharBtn; i++) {
        m_charBtnArray[i] = LOAD_UI_T(CUiCheckButton, "Pnbutton.btnuser" + std::to_string(i));
        m_charBtnArray[i]->EvLClick +=
            RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectChar, (void*)i);
    }
    //进入游戏
    LOAD_UI("btnenter")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Enter));
    //返回上级 创建人物时有两阶段需要判断
    LOAD_UI("btnback")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
    //创建人物
    LOAD_UI("btncreaterole")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_EngterCreateChar);
    //删除人物
    LOAD_UI("btndeletechar")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Delete);

    //人物角色性别
    LOAD_UI("btnmale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexMale);
    LOAD_UI("btnfemale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexFemale);
    //人物左旋按钮
    //LOAD_UI("btnuserleft")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LeftRotation);
    //LOAD_UI("btnuserleft")->EvMouseLDown += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseDown);
    //LOAD_UI("btnuserleft")->EvMouseLUp += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseUp);
    //人物右旋按钮
    //LOAD_UI("btnuserright")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RightRotation);

    //创建人物角色界面

    LOAD_UI("fmorder.btnshang")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)1);
    LOAD_UI("fmorder.btnzhou")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)2);

    m_charname = LOAD_UI_T(RtwTextBox, "fmcreatid1.fmname.txtname");
    m_charname->SetCapacity(14);
    //人物生肖选择框
    LOAD_UI("fmcreatid1.btnanimalleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevAnimal);
    LOAD_UI("fmcreatid1.btnanimalright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextAnimal);
    //人物角色发型
    LOAD_UI("fmcreatid2.btnhairleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHair);
    LOAD_UI("fmcreatid2.btnhairright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHair);
    //人物角色脸型
    LOAD_UI("fmcreatid2.btnfaceleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevFace);
    LOAD_UI("fmcreatid2.btnfaceright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextFace);
    //人物角色头像
    LOAD_UI("fmcreatid2.btnheadleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHead);
    LOAD_UI("fmcreatid2.btnheadright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHead);
    //随机创建
    LOAD_UI("fmcreatid3.btnrandom")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RandomCreate);
    //完成创建
    LOAD_UI("fmcreatid3.btncreate")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar);

    //四大门派---------蜀山派,花间门,苗疆,五台山
    LOAD_UI("fmshushan.btnshushan")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectShushan);
    LOAD_UI("fmhuajian.btnhuajian")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectHuajian);
    LOAD_UI("fmshengwu.btnshengwu")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectShenwu);
    LOAD_UI("fmmingwang.btnmingwang")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectMingwang);

    LOAD_UI("fmcreatid1")->Hide();
    LOAD_UI("fmcreatid2")->Hide();
    LOAD_UI("fmcreatid3")->Hide();
    unguard;
}

UILayerSelectChar::~UILayerSelectChar() {}

//进入游戏
void UILayerSelectChar::OnClicked_Enter(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnSelectUser();
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
        if (gc_login->m_selectFaction != -1) {
            gc_login->SetSelectShangOrZhou(-1);
        } else {
            gc_login->SetLoginState(GcLogin::GLS_SELECT_CHAR);
        }
    } else if (status == GcLogin::GLS_SELECT_CHAR) {
        gc_login->NetClose();
        gc_login->SetLoginState(GcLogin::GLS_LOGIN);
    }
    unguard;
}

//选择人物
void UILayerSelectChar::OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            gc_login->SetSelectUser((int)e->param1);
        }
    }
}

//创建人物
void UILayerSelectChar::OnClicked_EngterCreateChar(void*, void*) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            gc_login->SetLoginState(GcLogin::GLS_CREATE_CHAR);
        }
    }
}

//商 周
void UILayerSelectChar::OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            gc_login->SetSelectShangOrZhou((int)e->param1);
        }
    }
}

//删除人物
void UILayerSelectChar::OnClicked_Delete(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {

            if (GetLogin()->GetCurSelectChar() != -1) {

                GetLogin()->OnCharPasswordConfirm("");

            } else {
                UIFormMsg::ShowStatic(R(LMSG_PLS_CHOOSE_CHAR), UIFormMsg::TYPE_OK);
            }
        }
    }
    unguard;
}

//人物角色性别---男女
void UILayerSelectChar::OnClicked_SetSexMale(void*, void*) {
    LOAD_UI_T(CUiCheckButton, "btnmale")->SetChecked(true);
    GetLogin()->SetCharSex(false);
}

void UILayerSelectChar::OnClicked_SetSexFemale(void*, void*) {
    LOAD_UI_T(CUiCheckButton, "btnfemale")->SetChecked(true);
    GetLogin()->SetCharSex(true);
}

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

//创建人物角色界面
//人物生肖选择框
void UILayerSelectChar::OnClicked_PrevAnimal(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharAnimal(false);
        }
    }
    unguard;
}

void UILayerSelectChar::OnClicked_NextAnimal(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharAnimal(true);
        }
    }
    unguard;
}

//人物角色发型
//上一个发型名称
void UILayerSelectChar::OnClicked_PrevHair(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharHair(false);
        }
    }
    unguard;
}

//下一个发型名称
void UILayerSelectChar::OnClicked_NextHair(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharHair(true);
        }
    }
    unguard;
}

//人物角色脸型
void UILayerSelectChar::OnClicked_PrevFace(void*, void*) {
    guard;
    unguard;
}

void UILayerSelectChar::OnClicked_NextFace(void*, void*) {
    guard;
    unguard;
}

//人物角色头像
void UILayerSelectChar::OnClicked_PrevHead(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharImage(false);
        }
    }
    unguard;
}

void UILayerSelectChar::OnClicked_NextHead(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharImage(true);
        }
    }
    unguard;
}

//随机创建
void UILayerSelectChar::OnClicked_RandomCreate(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->OnRandomCreateUser();
        }
    }
    unguard;
}

//完成创建人物角色
void UILayerSelectChar::OnClicked_CreateChar(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->OnCreateUser();
        }
    }
    unguard;
}

//四大门派---------蜀山派,花间门,苗疆,五台山
//蜀山派
void UILayerSelectChar::OnClicked_SelectShushan(void*, void*) {
    guard;
    if (LOAD_UI("fmshushan.btnshushan")->getEnable()) {
        if (GetLogin()) {
            if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
                GetLogin()->SetSelectUser(2);
            }
        }
    }
    unguard;
}

//花间门
void UILayerSelectChar::OnClicked_SelectHuajian(void*, void*) {
    guard;
    if (LOAD_UI("fmhuajian.btnhuajian")->getEnable()) {
        if (GetLogin()) {
            if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
                GetLogin()->SetSelectUser(1);
            }
        }
    }
    unguard;
}

//苗疆
void UILayerSelectChar::OnClicked_SelectShenwu(void*, void*) {
    guard;
    if (LOAD_UI("fmshengwu.btnshengwu")->getEnable()) {
        if (GetLogin()) {
            if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
                GetLogin()->SetSelectUser(3);
            }
        }
    }
    unguard;
}

//五台山
void UILayerSelectChar::OnClicked_SelectMingwang(void*, void*) {
    guard;
    if (LOAD_UI("fmmingwang.btnmingwang")->getEnable()) {
        if (GetLogin()) {
            if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
                GetLogin()->SetSelectUser(0);
            }
        }
    }
    unguard;
}

//设置密码
void UILayerSelectChar::OnReceiveSetPassword(char lRet) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            if (lRet == 0) {
                GetLogin()->GetAccountInfo().users[GetLogin()->GetCurSelectChar()].hasCharPwd =
                    true;
                GetLogin()->UpdateSelectChar();
                UIFormMsg::ShowStatic(R(MSG_CLIENT_SETPASSWORDSUCCESS), UIFormMsg::TYPE_OK);
            } else {
                UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
            }
        }
    }
    unguard;
}

void UILayerSelectChar::OnReceivePasswordConfirm(char lRet) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            if (lRet == 1) {
                if (GetLogin()->GetSelectUserWithPwd()) {
                    GetLogin()->OnSelectUserWithPwd();
                } else {
                    const GcAccountInfo& info = GetLogin()->GetAccountInfo();
                    int                  iSel = GetLogin()->GetCurSelectChar();
                    if (info.users[iSel].attributes.level >= 30) {
                        UIFormMsg* pFrm = UIFormMsg::ShowStatic(R(LMSG_CONFIRM_DELETE_CHAR_THIRTY),
                                                                UIFormMsg::TYPE_OK_CANCEL);
                        pFrm->EvOK =
                            RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete, pFrm);
                    } else {
                        UIFormMsg* pFrm = UIFormMsg::ShowStatic(R(LMSG_CONFIRM_DELETE_CHAR),
                                                                UIFormMsg::TYPE_OK_CANCEL);
                        pFrm->EvOK =
                            RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete, pFrm);
                    }
                }
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
                GetLogin()->GetAccountInfo().users[GetLogin()->GetCurSelectChar()].hasCharPwd =
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

void UILayerSelectChar::OnConfirm_Delete(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnDeleteUser();
        }
    }
    unguard;
}

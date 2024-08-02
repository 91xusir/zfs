#include "gc_include.h"
#include "gc_login.h"
#include "ui_form_msg.h"

UILayerSelectChar::UILayerSelectChar() {
    guard;
    g_workspace.Load("ui/2.selectChar.xml");
    //�������ﰴť
    for (std::size_t i = 0; i < MaxUserCharBtn; i++) {
        m_charBtnArray[i] = LOAD_UI_T(CUiCheckButton, "Pnbutton.btnuser" + std::to_string(i));
        m_charBtnArray[i]->EvLClick +=
            RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectChar, (void*)i);
    }
    //������Ϸ
    LOAD_UI("btnenter")
        ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Enter));
    //�����ϼ� ��������ʱ�����׶���Ҫ�ж�
    LOAD_UI("btnback")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
    //��������
    LOAD_UI("btncreaterole")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_EngterCreateChar);
    //ɾ������
    LOAD_UI("btndeletechar")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Delete);

    //�����ɫ�Ա�
    LOAD_UI("btnmale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexMale);
    LOAD_UI("btnfemale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexFemale);
    //����������ť
    //LOAD_UI("btnuserleft")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LeftRotation);
    //LOAD_UI("btnuserleft")->EvMouseLDown += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseDown);
    //LOAD_UI("btnuserleft")->EvMouseLUp += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseUp);
    //����������ť
    //LOAD_UI("btnuserright")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RightRotation);

    //���������ɫ����

    LOAD_UI("fmorder.btnshang")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)1);
    LOAD_UI("fmorder.btnzhou")->EvLClick +=
        RTW_CALLBACK_1(this, UILayerSelectChar, OnClicked_SelectShangOrZhou, (void*)2);

    m_charname = LOAD_UI_T(RtwTextBox, "fmcreatid1.fmname.txtname");
    m_charname->SetCapacity(14);
    //������Фѡ���
    LOAD_UI("fmcreatid1.btnanimalleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevAnimal);
    LOAD_UI("fmcreatid1.btnanimalright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextAnimal);
    //�����ɫ����
    LOAD_UI("fmcreatid2.btnhairleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHair);
    LOAD_UI("fmcreatid2.btnhairright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHair);
    //�����ɫ����
    LOAD_UI("fmcreatid2.btnfaceleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevFace);
    LOAD_UI("fmcreatid2.btnfaceright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextFace);
    //�����ɫͷ��
    LOAD_UI("fmcreatid2.btnheadleft")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHead);
    LOAD_UI("fmcreatid2.btnheadright")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHead);
    //�������
    LOAD_UI("fmcreatid3.btnrandom")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RandomCreate);
    //��ɴ���
    LOAD_UI("fmcreatid3.btncreate")->EvLClick +=
        RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar);

    //�Ĵ�����---------��ɽ��,������,�置,��̨ɽ
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

//������Ϸ
void UILayerSelectChar::OnClicked_Enter(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            GetLogin()->OnSelectUser();
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

//ѡ������
void UILayerSelectChar::OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            gc_login->SetSelectUser((int)e->param1);
        }
    }
}

//��������
void UILayerSelectChar::OnClicked_EngterCreateChar(void*, void*) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_SELECT_CHAR) {
            gc_login->SetLoginState(GcLogin::GLS_CREATE_CHAR);
        }
    }
}

//�� ��
void UILayerSelectChar::OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e) {
    const auto& gc_login = GetLogin();
    if (gc_login) {
        if (gc_login->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            gc_login->SetSelectShangOrZhou((int)e->param1);
        }
    }
}

//ɾ������
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

//�����ɫ�Ա�---��Ů
void UILayerSelectChar::OnClicked_SetSexMale(void*, void*) {
    LOAD_UI_T(CUiCheckButton, "btnmale")->SetChecked(true);
    GetLogin()->SetCharSex(false);
}

void UILayerSelectChar::OnClicked_SetSexFemale(void*, void*) {
    LOAD_UI_T(CUiCheckButton, "btnfemale")->SetChecked(true);
    GetLogin()->SetCharSex(true);
}

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

//���������ɫ����
//������Фѡ���
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

//�����ɫ����
//��һ����������
void UILayerSelectChar::OnClicked_PrevHair(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharHair(false);
        }
    }
    unguard;
}

//��һ����������
void UILayerSelectChar::OnClicked_NextHair(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->ChangeCharHair(true);
        }
    }
    unguard;
}

//�����ɫ����
void UILayerSelectChar::OnClicked_PrevFace(void*, void*) {
    guard;
    unguard;
}

void UILayerSelectChar::OnClicked_NextFace(void*, void*) {
    guard;
    unguard;
}

//�����ɫͷ��
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

//�������
void UILayerSelectChar::OnClicked_RandomCreate(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->OnRandomCreateUser();
        }
    }
    unguard;
}

//��ɴ��������ɫ
void UILayerSelectChar::OnClicked_CreateChar(void*, void*) {
    guard;
    if (GetLogin()) {
        if (GetLogin()->GetStatus() == GcLogin::GLS_CREATE_CHAR) {
            GetLogin()->OnCreateUser();
        }
    }
    unguard;
}

//�Ĵ�����---------��ɽ��,������,�置,��̨ɽ
//��ɽ��
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

//������
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

//�置
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

//��̨ɽ
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

//��������
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

//���������
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

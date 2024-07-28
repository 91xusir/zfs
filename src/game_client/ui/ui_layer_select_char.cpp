#include "gc_include.h"
#include "gc_login.h"
#include "ui_form_msg.h"

UILayerSelectChar::UILayerSelectChar()
{
    guard;

    g_workspace.Load("ui/ui_layer_select_char.xml");

	//ѡ�˽���
	//�ĸ��½����ﰴť
	RTW_WIDGET("fmckusername.fmusername1.btnusername1")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar1);
	RTW_WIDGET("fmckusername.fmusername2.btnusername2")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar2);
	RTW_WIDGET("fmckusername.fmusername3.btnusername3")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar3);
	RTW_WIDGET("fmckusername.fmusername4.btnusername4")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar4);
	//�ָ�����
	RTW_WIDGET("btnrestore")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Restore);
	//ɾ������
	RTW_WIDGET("btndeletechar")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Delete);
	//��������
	RTW_WIDGET("btnsetcharpwd")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetCharPwd);
	//�������
	RTW_WIDGET("btnchangecharpwd")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_ChangeCharPwd);
	//�������
	RTW_WIDGET("btndelcharpwd")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_DelCharPwd);
	//�����ɫ�Ա�
	RTW_WIDGET("btnmale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexMale);
	RTW_WIDGET("btnfemale")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SetSexFemale);
	//����������ť
	//RTW_WIDGET("btnuserleft")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LeftRotation);
	//RTW_WIDGET("btnuserleft")->EvMouseLDown += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseDown);
	//RTW_WIDGET("btnuserleft")->EvMouseLUp += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_LMouseUp);
	//����������ť
	//RTW_WIDGET("btnuserright")->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RightRotation);
	//������Ϸ
    RTW_WIDGET("btnenter")->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Enter));
	//���ص�¼��ť
	RTW_WIDGET("btnbacklogin")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
	//����ѡ�˰�ť
	RTW_WIDGET("btnback")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Back);
	//�˳���Ϸ
	RTW_WIDGET("btnexit")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_Exit);
	//�����ɫ������
	m_numpwd = RTW_WIDGET_T(RtwTextBox, "fmpwd.fmtxtpwd.txtnumpwd");
	m_numpwd->SetCapacity(4);
	m_numpwd->EvUpdateText.ClearAndSetDelegate(RTW_CALLBACK(this, UILayerSelectChar, OnUpdateText));
	RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("�������������룬����Ϊ4λ����"));
	//�����رհ�ť
	m_closeBtn = RTW_WIDGET_T(RtwButton, "fmpwd.closeblt");
	m_closeBtn->EvLClick += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_ClosePwd);

	//���������ɫ����
	m_charname = RTW_WIDGET_T(RtwTextBox, "fmcreatid1.fmname.txtname");
	m_charname->SetCapacity(14);
	//������Фѡ���
	RTW_WIDGET("fmcreatid1.btnanimalleft")->EvLClick    += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevAnimal);
	RTW_WIDGET("fmcreatid1.btnanimalright")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextAnimal);
	//�����ɫ����
	RTW_WIDGET("fmcreatid2.btnhairleft")->EvLClick    += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHair);
	RTW_WIDGET("fmcreatid2.btnhairright")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHair);
	//�����ɫ����
	RTW_WIDGET("fmcreatid2.btnfaceleft")->EvLClick    += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevFace);
	RTW_WIDGET("fmcreatid2.btnfaceright")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextFace);
	//�����ɫͷ��
	RTW_WIDGET("fmcreatid2.btnheadleft")->EvLClick    += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_PrevHead);
	RTW_WIDGET("fmcreatid2.btnheadright")->EvLClick   += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_NextHead);
	//�������
	RTW_WIDGET("fmcreatid3.btnrandom")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_RandomCreate);
	//��ɴ���
	RTW_WIDGET("fmcreatid3.btncreate")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_CreateChar);

	//�Ĵ�����---------��ɽ��,������,�置,��̨ɽ
	RTW_WIDGET("fmshushan.btnshushan")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectShushan);
	RTW_WIDGET("fmhuajian.btnhuajian")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectHuajian);
	RTW_WIDGET("fmshengwu.btnshengwu")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectShenwu);
	RTW_WIDGET("fmmingwang.btnmingwang")->EvLClick  += RTW_CALLBACK(this, UILayerSelectChar, OnClicked_SelectMingwang);

	RTW_WIDGET("fmcreatid1")->Hide();
	RTW_WIDGET("fmcreatid2")->Hide();
	RTW_WIDGET("fmcreatid3")->Hide();


	m_iConfirmCnt = 0;
	m_CurBtnIdx = 0;
	m_OldPwd = "";
	m_NewPwd = "";
    unguard;
}

UILayerSelectChar::~UILayerSelectChar()
{
}
//ѡ�˽���
//�ĸ��½����ﰴť
void UILayerSelectChar::OnClicked_CreateChar1(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if (RTW_WIDGET("fmckusername.fmusername1.btnusername1.lbusername1")->GetText().empty())
			{
				GetLogin()->SetLoginState(GcLogin::GLS_CREATE_CHAR);
			}
			else
			{
				GetLogin()->SetSelectUser(0);
			}
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_CreateChar2(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if (RTW_WIDGET("fmckusername.fmusername2.btnusername2.lbusername2")->GetText().empty())
			{
				GetLogin()->SetLoginState(GcLogin::GLS_CREATE_CHAR);
			}
			else 
			{
				GetLogin()->SetSelectUser(1);
			}
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_CreateChar3(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if (RTW_WIDGET("fmckusername.fmusername3.btnusername3.lbusername3")->GetText().empty())
			{
				GetLogin()->SetLoginState(GcLogin::GLS_CREATE_CHAR);
			}
			else 
			{
				GetLogin()->SetSelectUser(2);
			}
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_CreateChar4(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if (RTW_WIDGET("fmckusername.fmusername4.btnusername4.lbusername4")->GetText().empty())
			{
				GetLogin()->SetLoginState(GcLogin::GLS_CREATE_CHAR);
			}
			else 
			{
				GetLogin()->SetSelectUser(3);
			}
		}
	}
	unguard;
}
//�ָ����ﰴť
void UILayerSelectChar::OnClicked_Restore(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnRestoreChar();
		}
	}
	unguard;
}
//ɾ������
void UILayerSelectChar::OnClicked_Delete(void*, void*)
{
	guard;
	m_CurBtnIdx = 1;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			//ȥ�����������������Ϸʱ������Ĳ���
			GetLogin()->SetSelectUserWithPwd(false);

			if (GetLogin()->GetCurSelectChar()!= -1)
			{
				if (GetLogin()->GetAccountInfo().users[GetLogin()->GetCurSelectChar()].hasCharPwd)
				{
					m_iConfirmCnt = 0;
					RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
					RTW_WIDGET("fmpwd")->Show();
					RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("����������������"));
				} 
				else
				{
					GetLogin()->OnCharPasswordConfirm("");
				}
			}
			else
			{
				UIFormMsg::ShowStatic(R(LMSG_PLS_CHOOSE_CHAR), UIFormMsg::TYPE_OK);
			}
		}
	}
	unguard;
}
//��������
void UILayerSelectChar::OnClicked_SetCharPwd(void*, void*)
{
	guard;
	m_CurBtnIdx = 2;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			//ȥ�����������������Ϸʱ������Ĳ���
			GetLogin()->SetSelectUserWithPwd(false);
			m_iConfirmCnt = 0;
			RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
			RTW_WIDGET("fmpwd")->Show();
		}
	}
	unguard;
}
//�������
void UILayerSelectChar::OnClicked_ChangeCharPwd(void*, void*)
{
	guard;
	m_CurBtnIdx = 3;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			//ȥ�����������������Ϸʱ������Ĳ���
			GetLogin()->SetSelectUserWithPwd(false);
			m_iConfirmCnt = 0;
			RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
			RTW_WIDGET("fmpwd")->Show();
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("����������ԭ������"));
		}
	}
	unguard;
}
//�������
void UILayerSelectChar::OnClicked_DelCharPwd(void*, void*)
{
	guard;
	m_CurBtnIdx = 4;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			//ȥ�����������������Ϸʱ������Ĳ���
			GetLogin()->SetSelectUserWithPwd(false);
			m_iConfirmCnt = 0;
			RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
			RTW_WIDGET("fmpwd")->Show();
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("����������ԭ������"));
		}
	}
	unguard;
}
//�����ɫ�Ա�---��Ů
void UILayerSelectChar::OnClicked_SetSexMale(void*, void*)
{
	RTW_WIDGET_T(CUiCheckButton, "btnmale")->SetChecked(true);
	GetLogin()->SetCharSex(false);
}
void UILayerSelectChar::OnClicked_SetSexFemale(void*, void*)
{
	RTW_WIDGET_T(CUiCheckButton, "btnfemale")->SetChecked(true);
	GetLogin()->SetCharSex(true);
}
//����������ť
void UILayerSelectChar::OnClicked_LeftRotation(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnLeftRotation();
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_LMouseDown(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnLeftRotationDown();
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_LMouseUp(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnLeftRotationUp();
		}
	}
	unguard;
}
//����������ť
void UILayerSelectChar::OnClicked_RightRotation(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnRightRotation();
		}
	}
	unguard;
}
//������Ϸ
void UILayerSelectChar::OnClicked_Enter(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnSelectUser();
		}
	}
	unguard;
}
//���ذ�ť
void UILayerSelectChar::OnClicked_Back(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			if (GetLogin()->GetAccountInfo().chatCount>0)
			{
				GetLogin()->SetLoginState(GcLogin::GLS_SELECT_CHAR);
			} 
			else
			{
				//û�д���ֱ�ӷ��ص�¼����
				GetLogin()->NetClose();
				GetLogin()->SetLoginState(GcLogin::GLS_LOGIN);
			}
		}else if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->NetClose();
			GetLogin()->SetLoginState(GcLogin::GLS_LOGIN);
		}
	}
	unguard;
}
//�˳���Ϸ
void UILayerSelectChar::OnClicked_Exit(void*, void*)
{
	guard;
	PostQuitMessage(0);
	unguard;
}

//����رտ�
void UILayerSelectChar::OnClicked_ClosePwd(void*, void*)
{

	RTW_WIDGET("fmpwd")->Hide();

}

//���������ɫ����
//������Фѡ���
void UILayerSelectChar::OnClicked_PrevAnimal(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharAnimal(false);
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_NextAnimal(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharAnimal(true);
		}
	}
	unguard;
}

//�����ɫ����
//��һ����������
void UILayerSelectChar::OnClicked_PrevHair(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharHair(false);
		}
	}
	unguard;
}
//��һ����������
void UILayerSelectChar::OnClicked_NextHair(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharHair(true);
		}
	}
	unguard;
}
//�����ɫ����
void UILayerSelectChar::OnClicked_PrevFace(void*, void*)
{
	guard;
	unguard;
}
void UILayerSelectChar::OnClicked_NextFace(void*, void*)
{
	guard;
	unguard;
}
//�����ɫͷ��
void UILayerSelectChar::OnClicked_PrevHead(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharImage(false);
		}
	}
	unguard;
}
void UILayerSelectChar::OnClicked_NextHead(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->ChangeCharImage(true);
		}
	}
	unguard;
}
//�������
void UILayerSelectChar::OnClicked_RandomCreate(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->OnRandomCreateUser();
		}
	}
	unguard;
}
//��ɴ��������ɫ
void UILayerSelectChar::OnClicked_CreateChar(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
		{
			GetLogin()->OnCreateUser();
		}
	}
	unguard;
}
//�Ĵ�����---------��ɽ��,������,�置,��̨ɽ
//��ɽ��
void UILayerSelectChar::OnClicked_SelectShushan(void*, void*)
{
	guard;
	if (RTW_WIDGET("fmshushan.btnshushan")->getEnable())
	{
		if (GetLogin())
		{
			if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
			{
				GetLogin()->SetSelectUser(2);
			}
		}
	}
	unguard;
}
//������
void UILayerSelectChar::OnClicked_SelectHuajian(void*, void*)
{
	guard;
	if (RTW_WIDGET("fmhuajian.btnhuajian")->getEnable())
	{
		if (GetLogin())
		{
			if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
			{
				GetLogin()->SetSelectUser(1);
			}
		}
	}
	unguard;
}
//�置
void UILayerSelectChar::OnClicked_SelectShenwu(void*, void*)
{
	guard;
	if (RTW_WIDGET("fmshengwu.btnshengwu")->getEnable())
	{
		if (GetLogin())
		{
			if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
			{
				GetLogin()->SetSelectUser(3);
			}
		}

	}
	unguard;
}
//��̨ɽ
void UILayerSelectChar::OnClicked_SelectMingwang(void*, void*)
{
	guard;
	if (RTW_WIDGET("fmmingwang.btnmingwang")->getEnable())
	{
		if (GetLogin())
		{
			if (GetLogin()->GetStatus()==GcLogin::GLS_CREATE_CHAR)
			{
				GetLogin()->SetSelectUser(0);
			}
		}
	}
	unguard;
}
//��������
void UILayerSelectChar::OnReceiveSetPassword(char lRet)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if(lRet == 0)
			{
				GetLogin()->GetAccountInfo().users[GetLogin()->GetCurSelectChar()].hasCharPwd=true;
				GetLogin()->UpdateSelectChar();
				UIFormMsg::ShowStatic(R(MSG_CLIENT_SETPASSWORDSUCCESS), UIFormMsg::TYPE_OK);
			}
			else
			{
				UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
			}
		}
	}
	unguard;
}

void UILayerSelectChar::OnReceivePasswordConfirm(char lRet)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if(lRet == 1)
			{			
				if (GetLogin()->GetSelectUserWithPwd())
				{
					GetLogin()->OnSelectUserWithPwd();
				} 
				else
				{
					const GcAccountInfo& info = GetLogin()->GetAccountInfo();
					int iSel = GetLogin()->GetCurSelectChar();
					if (info.users[iSel].attributes.level >= 30)
					{
						UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(LMSG_CONFIRM_DELETE_CHAR_THIRTY), UIFormMsg::TYPE_OK_CANCEL);
						pFrm->EvOK = RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete,pFrm);
					} 
					else
					{
						UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(LMSG_CONFIRM_DELETE_CHAR), UIFormMsg::TYPE_OK_CANCEL);
						pFrm->EvOK = RTW_CALLBACK_1(this, UILayerSelectChar, OnConfirm_Delete,pFrm);
					}
				}
			}else
			{
				UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
				RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
			}
		}
	}
	unguard;
}
//���������
void UILayerSelectChar::OnReceiveDelPassword(char lRet)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			if(lRet == 0)
			{		
				GetLogin()->GetAccountInfo().users[GetLogin()->GetCurSelectChar()].hasCharPwd=false;
				GetLogin()->UpdateSelectChar();
				UIFormMsg::ShowStatic(R(MSG_CLIENT_REMOVEPASSWORDSUCCESS), UIFormMsg::TYPE_OK);
			}else
			{
				UIFormMsg::ShowStatic(R(PASSWORD_ERR), UIFormMsg::TYPE_OK);
			}
		}
	}
	unguard;
}

void UILayerSelectChar::OnUpdateText(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if (RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText().empty())
	{
		ShowMessage(R(MSG_CHARACTER_CANNOT_NULL));
		RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
		return;
	}
	if (RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText().length()<4)
	{
		ShowMessage(R(MSG_CHARACTERPASSWORDISFOUR));
		RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
		return;
	}
	//�������ý�ɫ���������Ϸ
	if (GetLogin()->GetSelectUserWithPwd())
	{
		GetLogin()->OnCharPasswordConfirm(RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText().c_str());
	}
	m_iConfirmCnt++;
	if (m_iConfirmCnt == 1)           //-------------��һ�ε��ȷ��
	{
		if (m_CurBtnIdx == 1)         //ɾ�����ﰴť
		{
			GetLogin()->OnCharPasswordConfirm(RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText().c_str());
			if (RTW_WIDGET("fmpwd")->IsVisible())
			{
				RTW_WIDGET("fmpwd")->Hide();
			}
		}else if (m_CurBtnIdx == 2)  //�������밴ť
		{
			m_OldPwd = RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText();
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("�뽫�ղŵ�����������һ��"));
		}
		else if (m_CurBtnIdx == 3)  //�������
		{
			m_OldPwd = RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText();
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("������������"));
		}
		else if (m_CurBtnIdx == 4)  //�������
		{
			GetLogin()->OnDelCharPassword(RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText().c_str());
			if (RTW_WIDGET("fmpwd")->IsVisible())
			{
				RTW_WIDGET("fmpwd")->Hide();
			}
		}
		RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
	} 
	else if(m_iConfirmCnt == 2)     //-------------�ڶ��ε��ȷ��
	{
		if (m_CurBtnIdx == 2)
		{
			m_NewPwd = RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText();
			if (m_NewPwd != m_OldPwd)
			{
				ShowMessage(R(MSG_LOGIN_PASS_NOTSAME));
			} 
			else
			{
				GetLogin()->OnSetCharPassword(m_NewPwd.c_str());
				if (RTW_WIDGET("fmpwd")->IsVisible())
				{
					RTW_WIDGET("fmpwd")->Hide();
				}
			}
		}
		else if (m_CurBtnIdx == 3)
		{
			m_OldPwd = RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText();
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("��������һ��ȷ������"));
		}
	}
	else if (m_iConfirmCnt == 3)
	{
		//�����������⴦��
		if (m_CurBtnIdx == 3)
		{
			m_NewPwd = RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->GetText();
			if (m_NewPwd != m_OldPwd)
			{
				ShowMessage(R(MSG_LOGIN_PASS_NOTSAME));
			} 
			else
			{
				GetLogin()->OnChangeCharPassword(m_OldPwd.c_str(),m_NewPwd.c_str());
			}
			if (RTW_WIDGET("fmpwd")->IsVisible())
			{
				RTW_WIDGET("fmpwd")->Hide();
			}
		}
	}
	unguard;
}

void UILayerSelectChar::OnConfirm_Delete(void*, void*)
{
	guard;
	if (GetLogin())
	{
		if (GetLogin()->GetStatus()==GcLogin::GLS_SELECT_CHAR)
		{
			GetLogin()->OnDeleteUser();
		}
	}
	unguard;
}




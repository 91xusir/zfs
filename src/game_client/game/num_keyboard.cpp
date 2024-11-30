
#include "uimain.h"
#include "num_keyboard.h"
#include "string_mapping.h"


const int SUB_KEY_LENGTH=6;//����̽����У�ÿ��������ǰ׺�ĳ���

bool CNumKeyboard::Init()
{
	//�����¼��ҽӵ�����̵Ŀ���
	RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->EvFocus+=	RTW_CALLBACK(this,CNumKeyboard,Change);

	//��һ��
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum1")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum2")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum3")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum4")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum5")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum6")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum7")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum8")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum9")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnum0")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnumdelete")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmpwd.fmpwdlock.btnnumenter")->EvLClick+=
		RTW_CALLBACK(this,CNumKeyboard,UserInterfaceEvent);

	//</������¼�>
	return true;

};

void CNumKeyboard::Change(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	if(vpEvent->type== weFocusIn)
	{
		//�õ����뽹��
		RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetFocus();

		if(vpSender->GetID()=="txtnumpwd")
		{

		}
	}
	else if(vpEvent->type== etUnFocus)
	{
		//ʧȥ���뽹��
		if(vpSender->GetID()=="txtnumpwd")
		{

		}
	}
}


void CNumKeyboard::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
	const string& Sender=vpSender->GetID();

	g_workspace.SetFocusWidget(RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd"));

	char KeyboardChar=0;
	KeyboardChar=Sender[SUB_KEY_LENGTH];

	if(Sender=="btnnumdelete")
	{
		g_workspace.KeyDown(VK_BACK);
		g_workspace.KeyChar(VK_BACK);
		g_workspace.KeyUp(VK_BACK);
	}
	else if(Sender=="btnnumenter")
	{
		g_workspace.KeyDown(VK_RETURN);
		g_workspace.KeyChar(VK_RETURN);
		g_workspace.KeyUp(VK_RETURN);
	}
	else
	{
		if(KeyboardChar!=0)
		{
			g_workspace.KeyChar(KeyboardChar);
		}
	}
}


#include "uimain.h"
#include "soft_keyboard.h"
#include "string_mapping.h"

const int SUB_KEY_LENGTH=6;//软键盘界面中，每个按键的前缀的长度

bool CSoftKeyboard::Init()
{
	RTW_WIDGET("fmlogin.fmusername.txtusername")->EvFocus+=	RTW_CALLBACK(this,CSoftKeyboard,ChangeUserFocus);
	RTW_WIDGET("fmlogin.fmpassword.txtpassword")->EvFocus+=	RTW_CALLBACK(this,CSoftKeyboard,ChangePasswordFocus);

	//-------------------小写的软键盘----------------------//
	//第一行
	RTW_WIDGET("fmkeyboard.btnkey0")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey1")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey2")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey3")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey4")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey5")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey6")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey7")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey8")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey9")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey-")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeybackspace")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第二行
	RTW_WIDGET("fmkeyboard.btnkeyq")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyw")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeye")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyr")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyt")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyy")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyu")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyi")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyo")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyp")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey:")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyenter")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第三行
	RTW_WIDGET("fmkeyboard.btnkeya")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeys")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyd")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyf")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyg")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyh")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyj")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyk")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyl")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey[")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey]")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyLock")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第四行
	RTW_WIDGET("fmkeyboard.btnkeyz")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyx")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyc")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyv")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyb")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeyn")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkeym")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey<")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey>")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey(")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey)")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第五行
	RTW_WIDGET("fmkeyboard.btnkey!")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey@")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey#")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey$")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey%")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey^")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey*")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey~")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey,")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey?")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey|")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard.btnkey=")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//-------------------大写的软键盘----------------------//
	//第一行
	RTW_WIDGET("fmkeyboard1.btnkey0")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey1")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey2")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey3")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey4")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey5")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey6")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey7")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey8")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey9")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey-")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeybackspace")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第二行
	RTW_WIDGET("fmkeyboard1.btnkeyQ")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyW")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyE")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyR")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyT")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyY")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyU")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyI")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyO")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyP")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey:")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyenter")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第三行
	RTW_WIDGET("fmkeyboard1.btnkeyA")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyS")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyD")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyF")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyG")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyH")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyJ")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyK")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyL")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey[")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey]")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyLock")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第四行
	RTW_WIDGET("fmkeyboard1.btnkeyZ")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyX")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyC")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyV")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyB")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyN")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkeyM")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey<")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey>")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey(")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey)")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	//第五行
	RTW_WIDGET("fmkeyboard1.btnkey!")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey@")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey#")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey$")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey%")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey^")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey*")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey~")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey,")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey?")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey|")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);
	RTW_WIDGET("fmkeyboard1.btnkey=")->EvLClick+=
		RTW_CALLBACK(this,CSoftKeyboard,UserInterfaceEvent);

	return true;

};

void CSoftKeyboard::ShowSoftKeyboard()
{

	if(mCapLock)
	{
		if (RTW_WIDGET("fmkeyboard")->IsVisible())
		{
			RTW_WIDGET("fmkeyboard")->Hide();
		}
		RTW_WIDGET("fmkeyboard1")->Show();
	}
	else
	{
		if (RTW_WIDGET("fmkeyboard1")->IsVisible())
		{
			RTW_WIDGET("fmkeyboard1")->Hide();
		}
		RTW_WIDGET("fmkeyboard")->Show();
	}
}

void CSoftKeyboard::HideSoftKeyboard()
{
	if (RTW_WIDGET("fmkeyboard")->IsVisible())
	{
		RTW_WIDGET("fmkeyboard")->Hide();
	}
	if (RTW_WIDGET("fmkeyboard1")->IsVisible())
	{
		RTW_WIDGET("fmkeyboard1")->Hide();
	}
}

bool CSoftKeyboard::IsVisible()
{
	if (RTW_WIDGET("fmkeyboard")->IsVisible() || RTW_WIDGET("fmkeyboard1")->IsVisible())
	{
		return true;
	} 
	else
	{
		return false;
	}

}

void CSoftKeyboard::ChangeUserFocus(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	if(vpEvent->type== weFocusIn)
	{
		//得到输入焦点
		bUserFocus = true;

		if(vpSender->GetID()=="txtpassword")
		{
			ShowSoftKeyboard();
		}
	}
	else if(vpEvent->type== etUnFocus)
	{
		//失去输入焦点
		if(vpSender->GetID()=="txtpassword")
		{
			HideSoftKeyboard();
		}
	}
}

void CSoftKeyboard::ChangePasswordFocus(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	if(vpEvent->type== weFocusIn)
	{
		//得到输入焦点
		bUserFocus = false;

		if(vpSender->GetID()=="txtusername")
		{
			ShowSoftKeyboard();
		}
	}
	else if(vpEvent->type== etUnFocus)
	{
		//失去输入焦点
		if(vpSender->GetID()=="txtusername")
		{
			HideSoftKeyboard();
		}
	}
}

void CSoftKeyboard::UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* e)
{
	const string& Sender=vpSender->GetID();

	if (bUserFocus)
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmusername.txtusername"));
	} 
	else
	{
		g_workspace.SetFocusWidget(RTW_WIDGET("fmlogin.fmpassword.txtpassword"));
	}

	char KeyboardChar=0;
	KeyboardChar=Sender[SUB_KEY_LENGTH];

	if(Sender=="btnkeybackspace")
	{
		g_workspace.KeyDown(VK_BACK);
		g_workspace.KeyChar(VK_BACK);
		g_workspace.KeyUp(VK_BACK);
	}
	else if(Sender=="btnkeyenter")
	{
		g_workspace.KeyDown(VK_RETURN);
		g_workspace.KeyChar(VK_RETURN);
		g_workspace.KeyUp(VK_RETURN);
	}
	else if(Sender=="btnkeyLock")
	{
		mCapLock=!mCapLock;

		ShowSoftKeyboard();
	}
	else
	{
		if(KeyboardChar!=0)
		{
			g_workspace.KeyChar(KeyboardChar);
		}
	}
}

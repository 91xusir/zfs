
#ifndef SOFT_KEYBOARD_H
#define SOFT_KEYBOARD_H

#include "ui/rtw_ui.h"

class CSoftKeyboard
{
public:
	CSoftKeyboard():mCapLock(false), bUserFocus(false){};
	~CSoftKeyboard(){};

	bool Init();
	void ShowSoftKeyboard();
	void HideSoftKeyboard();
	bool IsVisible();

	void ChangeUserFocus(ui::RtwWidget* vpSender, RtwEventDelegate* e);
	void ChangePasswordFocus(ui::RtwWidget* vpSender, RtwEventDelegate* e);
	bool GetLastFocus() { return bUserFocus; }


private:
	void UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* e);

private:
	bool mCapLock;
	bool bUserFocus;
};



#endif//SOFT_KEYBOARD_H

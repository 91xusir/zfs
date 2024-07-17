#ifndef NUM_KEYBOARD_H
#define NUM_KEYBOARD_H

#include "ui/rtw_ui.h"

class CNumKeyboard
{
public:
	CNumKeyboard(){};
	~CNumKeyboard(){};

	bool Init();
	bool Clear();

	void Change(ui::RtwWidget* vpSender, RtwEventDelegate* e);

private:
	void UserInterfaceEvent(ui::RtwWidget* vpSender, RtwEventDelegate* e);
};

#endif//NUM_KEYBOARD_H

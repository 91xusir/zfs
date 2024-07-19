

#ifndef GC_DOSKEYWITHUI_H
#define GC_DOSKEYWITHUI_H

#include "gc_doskey.h"
#include "gc_chat_const.h"
#include "ui/rtw_ui.h"


class CDoskeyWithUI:public CDoskey
{
public:
	CDoskeyWithUI()
		:mpDoskeyList(NULL),mListSize(0),mDisplay(false),mSelectedItem(true)
	{};
	~CDoskeyWithUI(){};

	void UserInterfaceSelect();
private:
	virtual void		OnLock();
	virtual void		OnUnLock();

	virtual void		OnUp();
	virtual void		OnDown();
	virtual void		OnSetEditText();
	virtual void		UpdateToUI();

	ui::RtwListBox*				mpDoskeyList;
	int							mListSize;
	bool						mSelectedItem;
	bool						mDisplay;
};

#endif//GC_DOSKEYWITHUI_H
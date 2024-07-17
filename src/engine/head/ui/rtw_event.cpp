#include "ui/rtw_ui.h"


void RtwEventDispatcher::Dispatch(RtwWidget* sender, RtwEventDelegate* e)
{
	std::list<CUiDelegate> delList;
	foreach(std::list<CUiDelegate>, iter, m_delegates)
	{
		e->param1 = param.param1;
		CUiDelegate& delegate = *iter;
		delegate(sender, e);
		if (e->del_me)
			delList.push_back(delegate);
		if (e->cancle)
			return;
	}

	foreach(std::list<CUiDelegate>, iter, delList)
		(*this) -= *iter;
}
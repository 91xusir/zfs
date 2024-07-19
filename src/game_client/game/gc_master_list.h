#ifndef GC_MASTER_LIST_H 
#define GC_MASTER_LIST_H

#include "master_list_const.h"
#include "ui/rtw_ui.h"
class UIFormMsg;


class CGCMasterList
{
public:
	static const int MASTER_LIST_COLUMN_MAX_COUNT=4;

	CGCMasterList()
		:mList(),
		mpUIList(NULL),	
		mpUI(NULL),
		mpUILevel(NULL),
		mpUIPrentice(NULL),
		mSortMainIndex(0)
	{
		for(int i=0;i!=MASTER_LIST_COLUMN_MAX_COUNT;++i)
		{
			mMaxToMin[i]=true;
		}
	};

	void Init();
	void Reset(){};
	void Clear(){};


	void		ApplyOk(ui::RtwWidget* vpSender, RtwEventDelegate* );
	UIFormMsg*	SysMessage(const string& vrString);
	void		UpdateUI();
	void		ReceiveFromServer(CG_CmdPacket *pPacket);

	void		UIClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void		UIDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* );
private:

	CMasterList		mList;
	RtwListBox*		mpUIList;	
	RtwWidget*      mpUI;
	RtwWidget*      mpUILevel;
	RtwWidget*      mpUIPrentice;

	bool			mMaxToMin[MASTER_LIST_COLUMN_MAX_COUNT];
	int				mSortMainIndex;
};





#endif//GC_MASTER_LIST_H
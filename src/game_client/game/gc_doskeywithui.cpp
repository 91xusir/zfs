#include "gc_doskeywithui.h"
#include "rt1tort2.h"
#include "uimain.h"

const unsigned int DEFAULT_LINES_COUNTS=5;

void CDoskeyWithUI::OnLock()
{
	guard;
	if(mpDoskeyList==NULL)
	{
		mpDoskeyList=LOAD_UI_T(RtwListBox,UI_DOSKEY_LIST_WHOLE_ID);
		mpDoskeyList->SetAutoScrollH(false);
		mpDoskeyList->SetAutoScrollV(false);
	}

	mpDoskeyList->RemoveAllItems();

	list<string*> TempList;

	for(list<string>::iterator	It=mList.begin();It!=mList.end();++It)
	{
		TempList.push_front(&(*It));
	}

	for(list<string*>::iterator It=TempList.begin();It!=TempList.end();++It)
	{
		mpDoskeyList->AddItem((*It)->c_str());
	}
	
	mListSize=(unsigned int)mList.size();

	if(mListSize!=0)
	{
		if (mSelectedItem)
		{
			mpDoskeyList->SetSelectedItem(mListSize-1);
		}
		static unsigned int LineHeight=mpDoskeyList->GetItemHeight();
		static RtwRect DoskeyList=g_workspace.getViewportRect();

		unsigned int ChangeHeight=(mListSize-DEFAULT_LINES_COUNTS)*LineHeight;

		RtwRect NewRect;
		//NewRect=DoskeyList;
		mpDoskeyList->SetWidgetSize(SSize(mpDoskeyList->getWidgetSize().width,mpDoskeyList->getLineCount()*16+5));
		NewRect.top =(DoskeyList.bottom-110)-mpDoskeyList->getWidgetSize().height;
		NewRect.bottom=NewRect.top+mpDoskeyList->getWidgetSize().height;
		NewRect.left=50;
		NewRect.right=NewRect.left+mpDoskeyList->getWidgetSize().width;
		mpDoskeyList->MoveResize(NewRect);
		g_workspace.BringWidgetToTop(mpDoskeyList);
		mpDoskeyList->Show();
		mDisplay=true;
	}
	unguard;
	
}


void CDoskeyWithUI::OnUnLock()
{
	guard;
	if(mpDoskeyList!=NULL)
	{
		mpDoskeyList->Hide();
	}
	unguard;
}

void CDoskeyWithUI::OnUp()
{
	guard;
	UpdateToUI();
	unguard;
}

void CDoskeyWithUI::OnDown()
{
	guard;
	UpdateToUI();
	unguard;
}

void CDoskeyWithUI::OnSetEditText()
{
	guard;
	mpDoskeyList->Hide();
	mDisplay=false;
	unguard;
}

void CDoskeyWithUI::UserInterfaceSelect()
{
	guard;
	int Index=mpDoskeyList->GetSelectedItem();
	Index=(mListSize-1)-Index;
	if(mEditTextExist)
	{
		Index+=1;
	}
	SetIndex(Index);
	return ;
	unguard;
}

void CDoskeyWithUI::UpdateToUI()
{
	guard;
	if(mEditTextExist)
	{
		int Temp=(mListSize-1)-(mIndex-1);
		Temp= (Temp>=mListSize ? -1:Temp);

		mpDoskeyList->SetSelectedItem(Temp);
	}
	else
	{
		mSelectedItem=false;
		OnLock();
		mpDoskeyList->SetSelectedItem((mListSize-1)-mIndex);
		mSelectedItem=true;
	}

	if(!mDisplay)
	{
		mpDoskeyList->Show();
		mDisplay=true;
	}
	unguard;
}





#include "gc_doskey.h"
#include "gc_include.h"

const int MAX_DOSKEY_COUNT=10;


void CDoskey::UnLock()
{
	guard;
	if(!mLock)
	{
		return;
	}

	mLock=false;
	//将buffer中的Doskey加到List中
	for(std::list< std::string >::iterator It=mBufferList.begin();It!=mBufferList.end();It++)
	{
		Add(*It);
	}
	mBufferList.clear();
	OnUnLock();
	return;
	unguard;
}

void CDoskey::Reset()
{
	guard;
	if(mEditTextExist)
	{//将编辑的文字清除
		mList.pop_front();
		mEditTextExist=false;
	}
	mCurrent=mList.begin();
	if(Size()==0)
	{
		mIndex=-1;
	}
	else
	{
		mIndex=0;
	}
	unguard;
	
}

bool CDoskey::Lock()
{
	guard;
	if(mList.size()==0)
	{
		return false;
	}

	mLock=true;
	Reset();
	UpdateToUI();
	return true;
	unguard;
}


bool CDoskey::IsEditText()
{
	guard;
	if(!mLock)
	{
		return false;
	}

	if(mEditTextExist)
	{
		if(mCurrent==mList.begin())
		{
			return true;
		}
	}

	return false;
	unguard;
}

bool CDoskey::GetEditText(string& vrEditText)
{
	guard;
	if(!mLock)
	{
		return false;
	}


	if(mEditTextExist)
	{
		vrEditText=mList.front();
		return true;
	}
	else
	{
		return false;
	}
	unguard;
}

void CDoskey::SetEditText(string& vrEditText)
{
	guard;
	if(!mLock)
	{
		return;
	}

	if(mEditTextExist)
	{
		(*mList.begin())=vrEditText;
	}
	else
	{
		mList.push_front(vrEditText);
		mEditTextExist=true;
	}
	mCurrent=mList.begin();
	mIndex=0;
	OnSetEditText();
	unguard;
}

void CDoskey::Add(string& vrDoskey)
{
	guard;
	string EditText;
	std::list < std::string >*	pList;			

	if(mLock)
	{
		pList=&mBufferList;
	}
	else
	{
		pList=&mList;
		if(mEditTextExist)
		{
			EditText=pList->front();
			pList->pop_front();
		}
	}

	pList->remove(vrDoskey);
	if(pList->size()>=MAX_DOSKEY_COUNT)
	{
		pList->pop_back();
	}
	pList->push_front(vrDoskey);

	if(!mLock)
	{
		if(mEditTextExist)
		{
			pList->push_front(EditText);
		}
	}
	return;
	unguard;
}

string& CDoskey::Get()
{
	guard;
	static string NullString("");
	if(!mLock)
	{
		return NullString;
	}

	if(mCurrent==mList.end())
	{
		return NullString;
	}
	else
	{
		return (*mCurrent);
	}
	unguard;
}
void CDoskey::SetIndex(int vIndex)
{
	guard;
	if(!mLock)
	{
		return ;
	}

	if(vIndex<0 || vIndex>=mList.size())
	{
		return;
	}
	
	int Offset=vIndex-mIndex;
	if(Offset>0)
	{
		for(int i=0;i<Offset;i++)
		{
			Up();
		}
	}
	else
	{
		for(int i=0;i>Offset;i--)
		{
			Down();
		}
	}
	unguard;
}

bool CDoskey::Up()
{
	guard;
	if(!mLock)
	{
		return false;
	}

	//队列的下一个
	if(mCurrent==mList.end())
	{
		return false;
	}
	else
	{
		mCurrent++;
		mIndex++;

		if(mCurrent==mList.end())
		{
			mCurrent--;
			mIndex--;
			return false;
		}
	}
	OnUp();
	return true;
	unguard;
}

bool CDoskey::Down()
{
	guard;
	if(!mLock)
	{
		return false;
	}

	//队列的上一个
	if(mCurrent!=mList.begin())
	{
		mCurrent--;
		mIndex--;
	}

	OnDown();
	return true;
	unguard;
}


















#ifndef GC_DOSKEY_H
#define GC_DOSKEY_H

#include <string>
#include <list>
using std::string;
using std::list;

typedef unsigned long DWORD;

class CDoskey
{
public:
	struct Data
	{
		Data():mDBID(0),mName(){};
		DWORD	mDBID;
		string	mName;
	};

	CDoskey():mLock(false),mEditTextExist(false),mCurrent(),mIndex(-1),mList(),mBufferList()
	{
	};
	~CDoskey(){};

	
	
	bool		IsLock(){return mLock;};
	bool		Lock();
	void		UnLock();

	void		Add(string& vrDoskey);

	
	string&		Get();
	bool		Up();
	bool		Down();

	bool		IsEditText();
	void		SetEditText(string& vrEditText);
	bool		GetEditText(string& vrEditText);
	

	void		SetIndex(int vIndex);
	int			Index(){return mIndex;};
	int			Size(){return ((int)mList.size());};


protected:
	void		Reset();

	bool						mLock;
	bool						mEditTextExist;
	std::list<string>::iterator	mCurrent;
	int							mIndex;
	std::list<string>			mList;
	std::list<string>			mBufferList;
private:
	virtual void		OnLock(){};
	virtual void		OnUnLock(){};

	virtual void		OnUp(){};
	virtual void		OnDown(){};
	virtual void		OnSetEditText(){};
	virtual void		UpdateToUI()= 0;
};





#endif//GC_DOSKEY_H

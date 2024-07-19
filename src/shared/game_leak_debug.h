
#ifndef GAME_LEAK_DEBUG_H
#define GAME_LEAK_DEBUG_H

#include "game_debug_interface.h"

template<class T_KEY>
class CLeakInfo
{
public:
	friend CMessenger& operator <<(CMessenger& vrMessage,CLeakInfo& vrLeakInfo)
	{
		return vrLeakInfo.Print(vrMessage);
	};

	CLeakInfo(const T_KEY& vrKey):mKey(vrKey){};
	virtual ~CLeakInfo(){};
	T_KEY	Key() const {return mKey;};


	virtual CMessenger& Print(CMessenger& vrMessage)
	{
		vrMessage<<"[ " << mKey <<" ]"<<end;
		return vrMessage ;
	};
private:

	T_KEY	mKey;
};


template<class T_KEY, class T>
class CLeakDebug
{
public:

public:
	CLeakDebug(CMessenger& vrMessenger=CDebug("leak","leak.txt"))
		:mpMessage(vrMessenger.Clone()),mAddCount(0),mRemoveCount(0)

	{
	};

	~CLeakDebug()
	{
		if(mpMessage!=NULL)
		{
			*mpMessage<<"total add: "<< mAddCount <<"; total remove:" << mRemoveCount 
				<< " ;total leak:"<<mPool.size()<<end;
			
			map<T_KEY,T>::iterator It;
			for(It=mPool.begin();It!=mPool.end();++It)
			{
				(*mpMessage) << (It->second);
			}
			*mpMessage<<"-----------------------"<<end;
		}

		DEL_ONE(mpMessage)
	};

	void Add(const T& vrT)
	{
		
		map<T_KEY,T>::iterator It;
		if(mPool.find(vrT.Key())==mPool.end())
		{
			mPool.insert(map<T_KEY,T>::value_type(vrT.Key(),vrT));
			mAddCount++;
		}
		else
		{//已经存在
			*mpMessage<<"error: key is already exist [" << vrT.Key() <<"]"<<end;
		}
	};

	void Remove(T_KEY vKey)
	{
		
		map<T_KEY,T>::iterator It;
		if(mPool.find(vKey)==mPool.end())
		{//不存在
			*mpMessage<<"error: remove key is not exist [" << vKey <<"]"<<end;
		}
		else
		{
			mRemoveCount++;
			mPool.erase(vKey);
		}
	};
private:
	int				mAddCount;
	int				mRemoveCount;

	map<T_KEY,T>	mPool;
	CMessenger*		mpMessage;
};

#endif//GAME_LEAK_DEBUG_H
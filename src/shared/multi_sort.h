

#ifndef MULTI_SORT_H
#define MULTI_SORT_H

#include <vector>
#include <list>
#include "ref_pointer.h"


using std::vector;
using std::list;
//------------------比较部分-------------------

class CComp//比较基类
{
public:	
	//virtual ~CComp(){};//子类要能正常析构//只是一个接口//可以不调子类的析构//用默认即可
	virtual bool	Comp(void* vpLeft ,void* vpRight)=0;
	virtual bool	Equal(void* vpLeft ,void* vpRight)=0;
	virtual CComp*  Clone() const =0;
};


//将所有的Comp压缩到一个类层次
//因为Clone不是一个普遍操作，所以CRefPtr不支持//再套了个CCompPtr
class CCompPtr
{
public:
	CCompPtr(const CComp& vrComp)
	{
		mComp= vrComp.Clone();
	};

	bool	Comp(void* vpLeft ,void* vpRight)
	{
		mComp->Comp(vpLeft,vpRight);
	};

	bool	Equal(void* vpLeft ,void* vpRight)
	{
		mComp->Equal(vpLeft,vpRight);
	};

	CCompPtr& operator= (const CComp& vrComp)
	{
		mComp = vrComp.Clone();
		return *this;
	};

	CComp* operator ->()
	{
		return mComp;
	};

private:
	CRefPtr<CComp>  mComp;
};


//-------------------用户使用接口-------------------------
template<class T>
class CGreater:public CComp
{
public:
	virtual bool Comp(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) > (*((T*)vpRight));//这里强转不好//但不知道要怎么才能更好
	};

	virtual bool	Equal(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) == (*((T*)vpRight));//这里强转不好//但不知道要怎么才能更好
	}
	virtual CComp*  Clone() const
	{
		return RT_NEW CGreater<T>;
	}
};

template<class T>
class CLess:public CComp
{
public:	
	virtual bool Comp(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) < (*((T*)vpRight));//这里强转不好//但不知道要怎么才能更好
	};

	virtual bool	Equal(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) == (*((T*)vpRight));//这里强转不好//但不知道要怎么才能更好
	};

	virtual CComp*  Clone() const 
	{
		return RT_NEW CLess<T>;
	};
};

//-------------KEY部分--------------------
template<class USER_T>
class CKey//一行
{
public:
	CKey(USER_T* vpT)
		:mpT(vpT)
	{
	};

	~CKey(){};

	CKey& operator +=(void* vpKey)//有多少列
	{
		mKey.push_back(vpKey);
		return *this;
	};

	vector<void*>					mKey;
	USER_T*							mpT;
};


//----------------排序部分---------------------
class CMultiSort
{
public:
	CMultiSort(){};
	~CMultiSort(){};

	void SetMainIndex(int vMainIndex)//设置比较的优先次序//上次则退为次优先
	{

		list<int>::iterator It;
		for(It=mCompIndex.begin();It!=mCompIndex.end();++It)
		{
			if(*It==vMainIndex)
			{
				break;
			}
		}

		if(It==mCompIndex.end())
		{//没有这个索引
			return;
		}

		mCompIndex.erase(It);
		mCompIndex.push_front(vMainIndex);
		return;
	};

	CCompPtr& operator[](int vIndex)//得到某列的比较方法
	{
		return mComp[vIndex];
	}

	CMultiSort& operator +=( CComp& vrComp)
	{
		mComp.push_back(vrComp);
		mCompIndex.push_back(((int)mCompIndex.size()));
		return *this;
	}

	template<class USER_T>
		bool operator() ( CKey<USER_T>& vLeft, CKey<USER_T>& vRight) 
	{
		list<int>::iterator It=mCompIndex.begin();
		return  Comp(mComp,vLeft.mKey,vRight.mKey,It);
	}

	bool Comp(	vector< CCompPtr>&				vrComp,
		vector<void*>&					vrLeftKey,
		vector<void*>&					vrRightKey,
		list<int>::iterator				vIt)
	{
		if(vrComp[*vIt]->Comp((vrLeftKey[*vIt]),(vrRightKey[*vIt])))
		{
			return true;
		}
		else if(vrComp[*vIt]->Equal((vrLeftKey[*vIt]),(vrRightKey[*vIt])))
		{
			++vIt;
			if(vIt==mCompIndex.end())
			{//已经是最后一个元素//没有下一个元素
				return false;
			}

			//下一个元素
			return Comp(vrComp,vrLeftKey,vrRightKey,vIt);
		}
		else
		{
			return false;
		}
	};

	list<int>							mCompIndex;			//比较的索引//比较的优先次序
	vector< CCompPtr>					mComp;				//具体比较方法
};


#endif//MULTI_SORT_H

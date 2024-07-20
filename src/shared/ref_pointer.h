
#ifndef REF_POINTER_H
#define REF_POINTER_H

#include "use_count.h"

#define NULL    0

//引用计数指针[ok基本测试通过]
template<class T>
class CRefPtr
{
public:
	//构造函数//不希望隐式转换
	explicit CRefPtr(T* vpT=NULL)
		:mpT(vpT),mCount()
	{
	}

	//析构函数
	~CRefPtr()
	{
		if(mCount.Only())
		{
			DEL_ONE(mpT);
		}
	}

	//拷贝构造函数//就是默认
	//CRefPtr(const CRefPtr& vrPtr)


	//赋值函数
	CRefPtr& operator = ( const CRefPtr& vrPtr)
	{
		if(mCount.ReAttach(vrPtr.mCount))
		{
			DEL_ONE(mpT);
		}

		mpT=vrPtr.mpT;
		return *this;
	};


	//赋值函数//这个可以不写//会自动用operator = ( const CRefPtr& vrPtr)
	//但这个效率更高
	CRefPtr& operator = ( T* vpT)
	{
		if(mCount.Detach())
		{
			DEL_ONE(mpT);
		}

		mpT=vpT;
		return *this;
	}

	T* operator ->()
	{
		return mpT;
	}

	T& operator *()
	{
		return *mpT;
	}

	operator T* ()//将 CRefPtr 转换为 T*
	{
		return mpT;
	}

	bool Only()
	{
		return mCount.Only();
	}

	//专门用来提供写时复制的成员方法//要求T要有拷贝构造函数
	void MakeOnly()
	{
		if(mCount.MakeOnly())
		{
			mpT = RT_NEW T(*mpT);
		}
	}

private:

	T*			mpT;
	CCount		mCount;//引用计数
};







#endif//REF_POINTER_H

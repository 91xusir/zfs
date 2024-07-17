#ifndef SHARE_POINTER_H
#define SHARE_POINTER_H

#include "ref_pointer.h"

namespace fox
{
	template<class T>
	class CSharePtr
	{
	public:

		template<class T>
			friend class CWeakPtr ;

		//构造函数//不希望隐式转换
		explicit CSharePtr(T* vpT=NULL)
			:mPtrPtr( new T*(vpT) ),  mPtr(vpT)
		{/*LOG("");*/
		}

		//析构函数
		~CSharePtr()
		{/*LOG("");*/
			if(mPtr.Only())
			{
				*mPtrPtr = NULL;
			}
		}

		CSharePtr& operator = ( T* vpT)
		{/*LOG("");*/
			mPtrPtr= new T*( vpT );
			mPtr=vpT;
			return *this;
		}

		T* operator -> () const
		{/*LOG("");*/
			return mPtr.Get();
		}

		T& operator* ()
		{/*LOG("");*/
			return *mPtr;
		}

		bool operator == (const T* vpT) const
		{/*LOG("");*/
			return Get() == vpT;
		};

		bool operator == (const CSharePtr& vrPtr) const
		{/*LOG("");*/
			return Get() == vrPtr.Get();
		};

		bool operator !=(const T* vpT) const
		{/*LOG("");*/
			return Get() != vpT;
		};

		bool operator != (const CSharePtr& vrPtr) const
		{/*LOG("");*/
			return Get() != vrPtr.Get();
		};

		//operator T* () const//将 CRefPtr 转换为 T*//切不可如此转换//因为太隐式//很容易将已经被托管的指针在被其它地方删除或托管
		T* Get() const //将 CRefPtr 转换为 T*//仅仅有使用权//没有所有权！！
		{/*LOG("");*/
			return mPtr.Get();
		}
	private:
		CRefPtr<T*>		mPtrPtr;		//保存T的指针
		CRefPtr<T>		mPtr;			//保存T
	};
}
#endif//SHARE_POINTER_H
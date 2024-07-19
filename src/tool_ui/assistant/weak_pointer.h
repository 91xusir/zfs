#ifndef WEAK_POINTER_H 
#define WEAK_POINTER_H

#include "ref_pointer.h"
#include "share_pointer.h"

namespace fox
{
	template<class T>
	class CWeakPtr
	{
	public:
		//构造函数
		CWeakPtr():mPtrPtr(new T*(NULL)){/*LOG("");*/}

		//构造函数
		CWeakPtr(const CSharePtr<T>& vrPtr):mPtrPtr(vrPtr.mPtrPtr)
		{/*LOG("");*/}

		//赋值函数
		CWeakPtr& operator=(const CSharePtr<T>& vrPtr)
		{/*LOG("");*/
			mPtrPtr = vrPtr.mPtrPtr;
			return *this;
		}

		T* operator->() const
		{/*LOG("");*/
			return *mPtrPtr;
		}

		T& operator*()
		{/*LOG("");*/
			return *(*mPtrPtr);
		}

		operator T*()
		{/*LOG("");*/
			return *mPtrPtr;
		}
	private:
		CRefPtr<T*> mPtrPtr;		//保存T的指针
	};
}
#endif//WEAK_POINTER_H
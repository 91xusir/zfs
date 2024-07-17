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
		//���캯��
		CWeakPtr():mPtrPtr(new T*(NULL)){/*LOG("");*/}

		//���캯��
		CWeakPtr(const CSharePtr<T>& vrPtr):mPtrPtr(vrPtr.mPtrPtr)
		{/*LOG("");*/}

		//��ֵ����
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
		CRefPtr<T*> mPtrPtr;		//����T��ָ��
	};
}
#endif//WEAK_POINTER_H
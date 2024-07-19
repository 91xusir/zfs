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

		//���캯��//��ϣ����ʽת��
		explicit CSharePtr(T* vpT=NULL)
			:mPtrPtr( new T*(vpT) ),  mPtr(vpT)
		{/*LOG("");*/
		}

		//��������
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

		//operator T* () const//�� CRefPtr ת��Ϊ T*//�в������ת��//��Ϊ̫��ʽ//�����׽��Ѿ����йܵ�ָ���ڱ������ط�ɾ�����й�
		T* Get() const //�� CRefPtr ת��Ϊ T*//������ʹ��Ȩ//û������Ȩ����
		{/*LOG("");*/
			return mPtr.Get();
		}
	private:
		CRefPtr<T*>		mPtrPtr;		//����T��ָ��
		CRefPtr<T>		mPtr;			//����T
	};
}
#endif//SHARE_POINTER_H
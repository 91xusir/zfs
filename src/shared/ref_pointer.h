
#ifndef REF_POINTER_H
#define REF_POINTER_H

#include "use_count.h"

#define NULL    0

//���ü���ָ��[ok��������ͨ��]
template<class T>
class CRefPtr
{
public:
	//���캯��//��ϣ����ʽת��
	explicit CRefPtr(T* vpT=NULL)
		:mpT(vpT),mCount()
	{
	}

	//��������
	~CRefPtr()
	{
		if(mCount.Only())
		{
			DEL_ONE(mpT);
		}
	}

	//�������캯��//����Ĭ��
	//CRefPtr(const CRefPtr& vrPtr)


	//��ֵ����
	CRefPtr& operator = ( const CRefPtr& vrPtr)
	{
		if(mCount.ReAttach(vrPtr.mCount))
		{
			DEL_ONE(mpT);
		}

		mpT=vrPtr.mpT;
		return *this;
	};


	//��ֵ����//������Բ�д//���Զ���operator = ( const CRefPtr& vrPtr)
	//�����Ч�ʸ���
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

	operator T* ()//�� CRefPtr ת��Ϊ T*
	{
		return mpT;
	}

	bool Only()
	{
		return mCount.Only();
	}

	//ר�������ṩдʱ���Ƶĳ�Ա����//Ҫ��TҪ�п������캯��
	void MakeOnly()
	{
		if(mCount.MakeOnly())
		{
			mpT = RT_NEW T(*mpT);
		}
	}

private:

	T*			mpT;
	CCount		mCount;//���ü���
};







#endif//REF_POINTER_H

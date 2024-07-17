#ifndef REF_POINTER_H
#define REF_POINTER_H
#include "cmylog.h"

#define NULL    0

namespace fox
{
	//�������ָ��ϵ��ʹ�ã������û�ʹ��,������һ�������࣬�ӿڲ���̫����
	class CCount
	{
	public:
		CCount():mP(new int(1)){/*LOG("");*/};

		~CCount()
		{/*LOG("");*/
			if((--*mP)==0)
			{
				delete mP;
			}
		};

		CCount(const CCount& vrCount):mP(vrCount.mP)
		{/*LOG("");*/
			++*mP;//���ü�����һ
		};

		bool Only() const
		{/*LOG("");*/
			return *mP==1;
		}

		bool Detach()//���//true��ʾԭ������delete//false ��ʾû��delete
		{/*LOG("");*/
			if(--*mP == 0)
			{
				delete mP;
				mP = new int(1);
				return true;
			}
			mP = new int(1);
			return false;
		}

		bool ReAttach(const CCount& vrCount)//���°�//true��ʾԭ������delete//false��ʾû��delete
		{/*LOG("");*/
			++*vrCount.mP;
			if(--*mP == 0)
			{
				delete mP;
				mP = vrCount.mP;
				return true;
			}
			mP = vrCount.mP;
			return false;
		}

		bool MakeOnly()//����д//true��ʾ��Ҫ���ƶ�����//false��ʾ����Ҫ
		{/*LOG("");*/
			if(*mP == 1)
			{
				return false;
			}

			--*mP;
			mP = new int(1);
			return true;
		}

	private:
		CCount& operator=(const CCount& vrCount);//��ϣ������Ĭ�ϸ�ֵ����
		int* mP;
	};


	//���ü���ָ��[ok��������ͨ��]
	template<class T>
	class CRefPtr
	{
	public:
		//���캯��//��ϣ����ʽת��
		explicit CRefPtr(T* vpT=NULL):mpT(vpT),mCount(){/*LOG("");*/}

		//��������
		~CRefPtr()
		{/*LOG("");*/
			if(mCount.Only())
			{
				delete mpT;
			}
		}

		//��ֵ����
		CRefPtr& operator = ( const CRefPtr& vrPtr)
		{/*LOG("");*/
			if(mCount.ReAttach(vrPtr.mCount))
			{
				delete mpT;
			}

			mpT=vrPtr.mpT;
			return *this;
		};

		CRefPtr& operator = ( T* vpT)
		{/*LOG("");*/
			if(mCount.Detach())
			{
				if(mpT!=vpT)
				{//��ֹ�Ը���
					delete mpT;
				}
			}

			mpT=vpT;
			return *this;
		};

		bool operator == (const T* vpT) const
		{/*LOG("");*/
			return Get() == vpT;
		};

		bool operator == (const CRefPtr& vrPtr) const
		{/*LOG("");*/
			return Get() == vrPtr.Get();
		};

		bool operator !=(const T* vpT) const
		{/*LOG("");*/
			return Get() != vpT;
		};

		bool operator != (const CRefPtr& vrPtr) const
		{/*LOG("");*/
			return Get() != vrPtr.Get();
		};

		T* operator ->() const
		{/*LOG("");*/
			return mpT;
		}

		T& operator *() const
		{/*LOG("");*/
			return *mpT;
		}

		//operator T* () const//�� CRefPtr ת��Ϊ T*//�в������ת��//��Ϊ̫��ʽ//�����׽��Ѿ����йܵ�ָ���ڱ������ط�ɾ�����й�
		T* Get() const //�� CRefPtr ת��Ϊ T*//������ʹ��Ȩ//û������Ȩ����
		{/*LOG("");*/
			return mpT;
		}

		bool Only() const
		{/*LOG("");*/
			return mCount.Only();
		}

		//ר�������ṩдʱ���Ƶĳ�Ա����//Ҫ��TҪ�п������캯��
		void MakeOnly()
		{/*LOG("");*/
			if(mCount.MakeOnly())
			{
				mpT = new T(*mpT);
			}
		}
	private:
		T*			mpT;
		CCount		mCount;//���ü���
	};


	//�������ü���ָ��
	template<class T,size_t SIZE=sizeof(T)>
	class CRefArrayPtr
	{
	public:
		//���캯��//��ϣ����ʽת��
		explicit CRefArrayPtr(T* vpT=NULL)
			:mpT(vpT),mCount()
		{/*LOG("");*/
		}

		//��������
		~CRefArrayPtr()
		{/*LOG("");*/
			if(mCount.Only())
			{
				delete[] mpT;
			}
		}

		//��ֵ����
		CRefArrayPtr& operator = ( const CRefArrayPtr& vrPtr)
		{/*LOG("");*/
			if(mCount.ReAttach(vrPtr.mCount))
			{
				delete[] mpT;
			}

			mpT=vrPtr.mpT;
			return *this;
		};

		CRefArrayPtr& operator = ( T* vpT)
		{/*LOG("");*/
			if(mCount.Detach())
			{
				if(mpT!=vpT)
				{//��ֹ�Ը���
					delete[] mpT;
				}
			}
			mpT=vpT;
			return *this;
		}

		T& operator [] (int vIndex)
		{/*LOG("");*/
			return mpT[vIndex * SIZE];
		}

		operator T* () const//�� CRefArrayPtr ת��Ϊ T*
		{/*LOG("");*/
			return mpT;
		}

		bool Only()
		{/*LOG("");*/
			return mCount.Only();
		}

	private:
		T*			mpT;
		CCount		mCount;//���ü���
	};
}
#endif//REF_POINTER_H
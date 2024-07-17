#ifndef REF_POINTER_H
#define REF_POINTER_H
#include "cmylog.h"

#define NULL    0

namespace fox
{
	//该类仅供指针系列使用，不被用户使用,仅仅是一个辅助类，接口不必太教条
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
			++*mP;//引用计数加一
		};

		bool Only() const
		{/*LOG("");*/
			return *mP==1;
		}

		bool Detach()//解绑定//true表示原计数被delete//false 表示没有delete
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

		bool ReAttach(const CCount& vrCount)//重新绑定//true表示原计数被delete//false表示没有delete
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

		bool MakeOnly()//用于写//true表示需要复制对象本身//false表示不需要
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
		CCount& operator=(const CCount& vrCount);//不希望生成默认赋值函数
		int* mP;
	};


	//引用计数指针[ok基本测试通过]
	template<class T>
	class CRefPtr
	{
	public:
		//构造函数//不希望隐式转换
		explicit CRefPtr(T* vpT=NULL):mpT(vpT),mCount(){/*LOG("");*/}

		//析构函数
		~CRefPtr()
		{/*LOG("");*/
			if(mCount.Only())
			{
				delete mpT;
			}
		}

		//赋值函数
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
				{//防止自复制
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

		//operator T* () const//将 CRefPtr 转换为 T*//切不可如此转换//因为太隐式//很容易将已经被托管的指针在被其它地方删除或托管
		T* Get() const //将 CRefPtr 转换为 T*//仅仅有使用权//没有所有权！！
		{/*LOG("");*/
			return mpT;
		}

		bool Only() const
		{/*LOG("");*/
			return mCount.Only();
		}

		//专门用来提供写时复制的成员方法//要求T要有拷贝构造函数
		void MakeOnly()
		{/*LOG("");*/
			if(mCount.MakeOnly())
			{
				mpT = new T(*mpT);
			}
		}
	private:
		T*			mpT;
		CCount		mCount;//引用计数
	};


	//数组引用计数指针
	template<class T,size_t SIZE=sizeof(T)>
	class CRefArrayPtr
	{
	public:
		//构造函数//不希望隐式转换
		explicit CRefArrayPtr(T* vpT=NULL)
			:mpT(vpT),mCount()
		{/*LOG("");*/
		}

		//析构函数
		~CRefArrayPtr()
		{/*LOG("");*/
			if(mCount.Only())
			{
				delete[] mpT;
			}
		}

		//赋值函数
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
				{//防止自复制
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

		operator T* () const//将 CRefArrayPtr 转换为 T*
		{/*LOG("");*/
			return mpT;
		}

		bool Only()
		{/*LOG("");*/
			return mCount.Only();
		}

	private:
		T*			mpT;
		CCount		mCount;//引用计数
	};
}
#endif//REF_POINTER_H
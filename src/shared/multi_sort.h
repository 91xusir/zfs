

#ifndef MULTI_SORT_H
#define MULTI_SORT_H

#include <vector>
#include <list>
#include "ref_pointer.h"


using std::vector;
using std::list;
//------------------�Ƚϲ���-------------------

class CComp//�Ƚϻ���
{
public:	
	//virtual ~CComp(){};//����Ҫ����������//ֻ��һ���ӿ�//���Բ������������//��Ĭ�ϼ���
	virtual bool	Comp(void* vpLeft ,void* vpRight)=0;
	virtual bool	Equal(void* vpLeft ,void* vpRight)=0;
	virtual CComp*  Clone() const =0;
};


//�����е�Compѹ����һ������
//��ΪClone����һ���ձ����������CRefPtr��֧��//�����˸�CCompPtr
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


//-------------------�û�ʹ�ýӿ�-------------------------
template<class T>
class CGreater:public CComp
{
public:
	virtual bool Comp(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) > (*((T*)vpRight));//����ǿת����//����֪��Ҫ��ô���ܸ���
	};

	virtual bool	Equal(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) == (*((T*)vpRight));//����ǿת����//����֪��Ҫ��ô���ܸ���
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
		return (*((T*)vpLeft)) < (*((T*)vpRight));//����ǿת����//����֪��Ҫ��ô���ܸ���
	};

	virtual bool	Equal(void* vpLeft ,void* vpRight)
	{
		return (*((T*)vpLeft)) == (*((T*)vpRight));//����ǿת����//����֪��Ҫ��ô���ܸ���
	};

	virtual CComp*  Clone() const 
	{
		return RT_NEW CLess<T>;
	};
};

//-------------KEY����--------------------
template<class USER_T>
class CKey//һ��
{
public:
	CKey(USER_T* vpT)
		:mpT(vpT)
	{
	};

	~CKey(){};

	CKey& operator +=(void* vpKey)//�ж�����
	{
		mKey.push_back(vpKey);
		return *this;
	};

	vector<void*>					mKey;
	USER_T*							mpT;
};


//----------------���򲿷�---------------------
class CMultiSort
{
public:
	CMultiSort(){};
	~CMultiSort(){};

	void SetMainIndex(int vMainIndex)//���ñȽϵ����ȴ���//�ϴ�����Ϊ������
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
		{//û���������
			return;
		}

		mCompIndex.erase(It);
		mCompIndex.push_front(vMainIndex);
		return;
	};

	CCompPtr& operator[](int vIndex)//�õ�ĳ�еıȽϷ���
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
			{//�Ѿ������һ��Ԫ��//û����һ��Ԫ��
				return false;
			}

			//��һ��Ԫ��
			return Comp(vrComp,vrLeftKey,vrRightKey,vIt);
		}
		else
		{
			return false;
		}
	};

	list<int>							mCompIndex;			//�Ƚϵ�����//�Ƚϵ����ȴ���
	vector< CCompPtr>					mComp;				//����ȽϷ���
};


#endif//MULTI_SORT_H

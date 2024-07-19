#ifndef PACK_FACTORY_H 
#define PACK_FACTORY_H
#include <list>
//#include "game_pack.h"
#include "pack_error.h"


template<class TBaseProduct,class TType>
class CFactory
{
public:
	CFactory():mList(){};
	~CFactory()
	{
		typename std::list<TBaseProduct*>::iterator It;
		for(It=mList.begin();It!=mList.end();It++)
		{
			DEL_ONE((*It))
			(*It)=NULL;
		}
	}

	TBaseProduct* Get(const TType vType,const int vVersion)
	{
		typename std::list<TBaseProduct*>::iterator It;
		for(It=mList.begin();It!=mList.end();It++)
		{
			if((*It)->IsType(vType,vVersion))
			{
				return (*It)->Clone();
			}
		}

		//ERR("没有这种类型或版本的产品！)";
		return NULL;
	}

	TBaseProduct* GetReference(const TType vType,const int vVersion)
	{
		typename std::list<TBaseProduct*>::iterator It;
		for(It=mList.begin();It!=mList.end();++It)
		{
			if((*It)->IsType(vType,vVersion))
			{
				return (*It);
			}
		}
		//ERR("没有这种类型或版本的产品！)";
		return NULL;
	}
	template<class TDeriveProduct>
	bool RegisterType()
	{
		TBaseProduct* pProduct = RT_NEW TDeriveProduct;
		if(pProduct==NULL)
		{
			return false;
		}
		else
		{
			mList.push_back(pProduct);
			return true;
		}
	}

private:
	std::list<TBaseProduct*>	mList;

};


/*

template<class TDeriveProduct>
TBaseProduct* Get()
{
TBaseProduct* pProduct=RT_NEW TDeriveProduct;
if(pProduct==NULL)
{
//ERR("创建产品失败！");
}

return pProduct;
};
*/




#endif//PACK_FACTORY_H

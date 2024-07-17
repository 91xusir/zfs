
#ifndef USE_COUNT_H
#define USE_COUNT_H

//该类仅供指针系列使用，不被用户使用
//仅仅是一个辅助类，接口不必太教条

class CCount
{
public:
	CCount()
		:mP(RT_NEW int(1)){};

	~CCount()
	{
		if((--*mP)==0)
		{
			DEL_ONE(mP)
		}
	};

	CCount(const CCount& vrCount)
		:mP(vrCount.mP)
	{
		++*mP;//引用计数加一
	};



	bool Only()
	{
		return *mP==1;
	}

	bool Detach()//解绑定//true表示原计数被delete//false 表示没有delete
	{
		if(--*mP==0)
		{
			DEL_ONE(mP)
			mP=RT_NEW int(1);
			return true;
		}
		mP=RT_NEW int(1);
		return false;
	}

	bool ReAttach(const CCount& vrCount)//重新绑定//true表示原计数被delete//false表示没有delete
	{
		++*vrCount.mP;
		if(--*mP==0)
		{
			DEL_ONE(mP)
			mP=vrCount.mP;
			return true;
		}
		mP=vrCount.mP;
		return false;
	}

	bool MakeOnly()//用于写//true表示需要复制对象本身//false表示不需要
	{
		if(*mP==1)
		{
			return false;
		}

		--*mP;
		mP=RT_NEW int(1);
		return true;
	}

private:
	CCount& operator=(const CCount& vrCount);//不希望生成默认赋值函数
	int* mP;
};







#endif//USE_COUNT_H

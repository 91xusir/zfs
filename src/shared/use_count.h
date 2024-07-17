
#ifndef USE_COUNT_H
#define USE_COUNT_H

//�������ָ��ϵ��ʹ�ã������û�ʹ��
//������һ�������࣬�ӿڲ���̫����

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
		++*mP;//���ü�����һ
	};



	bool Only()
	{
		return *mP==1;
	}

	bool Detach()//���//true��ʾԭ������delete//false ��ʾû��delete
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

	bool ReAttach(const CCount& vrCount)//���°�//true��ʾԭ������delete//false��ʾû��delete
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

	bool MakeOnly()//����д//true��ʾ��Ҫ���ƶ�����//false��ʾ����Ҫ
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
	CCount& operator=(const CCount& vrCount);//��ϣ������Ĭ�ϸ�ֵ����
	int* mP;
};







#endif//USE_COUNT_H

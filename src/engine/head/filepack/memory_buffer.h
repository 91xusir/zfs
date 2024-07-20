#ifndef MEMORY_BUFFER_H 
#define MEMORY_BUFFER_H

#include<vector>
using std::vector;


class CMemoryBuffer
{
public:
	enum
	{
		DEFAULT_STEP_SIZE=15*1024,
	};

	CMemoryBuffer():mBufferList(),mUsedSize(0),mNewSize(0),mReadSize(0),mStepSize(DEFAULT_STEP_SIZE){};
	CMemoryBuffer(int vStepSize):mBufferList(),mUsedSize(0),mNewSize(0),mReadSize(0),mStepSize(vStepSize){};

	~CMemoryBuffer()
	{
		ReleaseBuffer();
	}

	bool Write(char* vpData,int vSize)
	{
		if(!NewBuffer(vSize))
		{
			return false;
		}

		int BeginBlockIndex=mUsedSize / mStepSize;
		int BeginBlockOffset=mUsedSize % mStepSize;

		int EndBlockIndex=(mUsedSize+vSize)/mStepSize;
		//int EndBlockOffset=(mUsedSize+vSize)%mStepSize;

		if(BeginBlockIndex==EndBlockIndex)
		{
			memcpy(mBufferList[BeginBlockIndex]+BeginBlockOffset,vpData,vSize);
		}
		else
		{
			//第一个
			int WriteSize;
			memcpy(mBufferList[BeginBlockIndex]+BeginBlockOffset,vpData,mStepSize-BeginBlockOffset);
			WriteSize=mStepSize-BeginBlockOffset;
			//中间的
			int i;
			for(i=BeginBlockIndex+1;i!=EndBlockIndex;i++)
			{
				memcpy(mBufferList[i],vpData+WriteSize,mStepSize);
				WriteSize+=mStepSize;
			}
			//最后的
			if (vSize - WriteSize > 0)
				memcpy(mBufferList[EndBlockIndex],vpData+WriteSize,vSize-WriteSize);
		}
		mUsedSize+=vSize;

		return true;
	};

	int Read(char*& vrpData)//读出数据//将数据存放的地址得到，返回值表示可以读多少字节
	{
		if(mReadSize==mUsedSize)
		{
			ClearData();//数据读完
			return 0;
		}

		int BeginBlockIndex=mReadSize/mStepSize;
		int BeginBlockOffset=mReadSize%mStepSize;

		vrpData=mBufferList[BeginBlockIndex]+BeginBlockOffset;
		
		int ReadSize=(mUsedSize-mReadSize) > (mStepSize-BeginBlockOffset) ? (mStepSize-BeginBlockOffset) : (mUsedSize-mReadSize) ;

		mReadSize+=ReadSize;

		return ReadSize;
	};


	void ClearData()	  //清空数据
	{
		mReadSize=0;
		mUsedSize=0;
	};

	void ReleaseBuffer()//释放内存
	{
		mUsedSize=0;
		mNewSize=0;
		mReadSize=0;
		vector<char*>::iterator It;
		for(It=mBufferList.begin();It!=mBufferList.end();++It)
		{
			DEL_ONE((*It));
		}
	};

	int DataSize()
	{
		return mUsedSize-mReadSize;
	}

private:
	bool NewBuffer(int vAddSize)
	{
		int MoreSize=vAddSize-(mNewSize-mUsedSize);
		if(MoreSize>0)
		{//需要再分配内存
			int Block=MoreSize/mStepSize+1;
			char* pBuffer; 
			int i;
			for(i=0;i<Block;++i)
			{
				pBuffer=RT_NEW char[mStepSize];
				if(pBuffer==NULL)
				{//内存耗尽
					return false;
				}
				mNewSize+=mStepSize;
				mBufferList.push_back(pBuffer);
			}
		}
		return true;
	}



	vector<char*>	mBufferList;
	int				mUsedSize;
	int				mNewSize;
	int				mReadSize;
	int				mStepSize;
};




class CLinearityBuffer
{
public:
	CLinearityBuffer():mpBuffer(NULL),mBufferSize(0){};
	~CLinearityBuffer(){DEL_ONE(mpBuffer);}

	CLinearityBuffer(  const CLinearityBuffer& vrBuffer)//拷贝构造
	{
		mpBuffer=vrBuffer.mpBuffer;
		mBufferSize=vrBuffer.mBufferSize;

		((CLinearityBuffer&)vrBuffer).mpBuffer=NULL;
		((CLinearityBuffer&)vrBuffer).mBufferSize=0;
	}

	CLinearityBuffer& operator=(CMemoryBuffer& vrMemoryBuffer)
	{
		Resize(vrMemoryBuffer.DataSize());

		//读出数据
		char* pData;
		int AlreadyWrite=0;
		int ReadSize=vrMemoryBuffer.Read(pData);
		for(;ReadSize!=0;)
		{
			//拷贝到线性缓存
			memcpy(Ptr()+AlreadyWrite,pData,ReadSize);
			AlreadyWrite+=ReadSize;
			ReadSize=vrMemoryBuffer.Read(pData);//继续读出数据
		}
		return *this;
	}

	CLinearityBuffer& operator=(const CLinearityBuffer& vrBuffer)//赋值函数
	{
		//防止自我复制
		if(&vrBuffer==this)
		{
			return *this;
		}

		//释放自己的内存
		Release();

		//将数据得到
		mpBuffer=vrBuffer.mpBuffer;
		mBufferSize=vrBuffer.mBufferSize;

		((CLinearityBuffer&)vrBuffer).mpBuffer=NULL;
		((CLinearityBuffer&)vrBuffer).mBufferSize=0;

		return *this;
	}

	bool ExpandSize(int vSize)//扩大内存//数据保留
	{
		if(vSize<=mBufferSize)
		{
			return true;
		}

		//新内存
		CLinearityBuffer Temp;
		if(!Temp.Resize(vSize))
		{
			return false;
		}

		//保留数据
		memcpy(Temp.Ptr(),Ptr(),mBufferSize);

		*this = Temp;
		return true;
	}

	//重新设置大小//数据不会保存
	bool Resize(int vSize)//比要求的空间大或等于
	{
		if(vSize<=mBufferSize)
		{
			return true;
		}

		if(mpBuffer!=NULL)
		{
			DEL_ONE(mpBuffer);
			mpBuffer=NULL;
			mBufferSize=0;
		}

		mpBuffer=RT_NEW char[vSize];
		if(mpBuffer==NULL)
		{
			return false;
		}
		mBufferSize=vSize;
		return true;
	}

	void Release()
	{
		//释放自己的内存
		if(mpBuffer!=NULL)
		{
			DEL_ONE(mpBuffer);
			mpBuffer=NULL;
			mBufferSize=0;
		}
	}
	int Size(){return mBufferSize;}

	//得到指针
	char* Ptr(){return mpBuffer;};

	char& operator [](const int vIndex)
	{
		return *(mpBuffer+vIndex);
	}

private:
	char*	mpBuffer;
	int		mBufferSize;
};










#endif//MEMORY_BUFFER_H

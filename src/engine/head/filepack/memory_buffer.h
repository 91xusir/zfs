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
			//��һ��
			int WriteSize;
			memcpy(mBufferList[BeginBlockIndex]+BeginBlockOffset,vpData,mStepSize-BeginBlockOffset);
			WriteSize=mStepSize-BeginBlockOffset;
			//�м��
			int i;
			for(i=BeginBlockIndex+1;i!=EndBlockIndex;i++)
			{
				memcpy(mBufferList[i],vpData+WriteSize,mStepSize);
				WriteSize+=mStepSize;
			}
			//����
			if (vSize - WriteSize > 0)
				memcpy(mBufferList[EndBlockIndex],vpData+WriteSize,vSize-WriteSize);
		}
		mUsedSize+=vSize;

		return true;
	};

	int Read(char*& vrpData)//��������//�����ݴ�ŵĵ�ַ�õ�������ֵ��ʾ���Զ������ֽ�
	{
		if(mReadSize==mUsedSize)
		{
			ClearData();//���ݶ���
			return 0;
		}

		int BeginBlockIndex=mReadSize/mStepSize;
		int BeginBlockOffset=mReadSize%mStepSize;

		vrpData=mBufferList[BeginBlockIndex]+BeginBlockOffset;
		
		int ReadSize=(mUsedSize-mReadSize) > (mStepSize-BeginBlockOffset) ? (mStepSize-BeginBlockOffset) : (mUsedSize-mReadSize) ;

		mReadSize+=ReadSize;

		return ReadSize;
	};


	void ClearData()	  //�������
	{
		mReadSize=0;
		mUsedSize=0;
	};

	void ReleaseBuffer()//�ͷ��ڴ�
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
		{//��Ҫ�ٷ����ڴ�
			int Block=MoreSize/mStepSize+1;
			char* pBuffer; 
			int i;
			for(i=0;i<Block;++i)
			{
				pBuffer=RT_NEW char[mStepSize];
				if(pBuffer==NULL)
				{//�ڴ�ľ�
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

	CLinearityBuffer(  const CLinearityBuffer& vrBuffer)//��������
	{
		mpBuffer=vrBuffer.mpBuffer;
		mBufferSize=vrBuffer.mBufferSize;

		((CLinearityBuffer&)vrBuffer).mpBuffer=NULL;
		((CLinearityBuffer&)vrBuffer).mBufferSize=0;
	}

	CLinearityBuffer& operator=(CMemoryBuffer& vrMemoryBuffer)
	{
		Resize(vrMemoryBuffer.DataSize());

		//��������
		char* pData;
		int AlreadyWrite=0;
		int ReadSize=vrMemoryBuffer.Read(pData);
		for(;ReadSize!=0;)
		{
			//���������Ի���
			memcpy(Ptr()+AlreadyWrite,pData,ReadSize);
			AlreadyWrite+=ReadSize;
			ReadSize=vrMemoryBuffer.Read(pData);//������������
		}
		return *this;
	}

	CLinearityBuffer& operator=(const CLinearityBuffer& vrBuffer)//��ֵ����
	{
		//��ֹ���Ҹ���
		if(&vrBuffer==this)
		{
			return *this;
		}

		//�ͷ��Լ����ڴ�
		Release();

		//�����ݵõ�
		mpBuffer=vrBuffer.mpBuffer;
		mBufferSize=vrBuffer.mBufferSize;

		((CLinearityBuffer&)vrBuffer).mpBuffer=NULL;
		((CLinearityBuffer&)vrBuffer).mBufferSize=0;

		return *this;
	}

	bool ExpandSize(int vSize)//�����ڴ�//���ݱ���
	{
		if(vSize<=mBufferSize)
		{
			return true;
		}

		//���ڴ�
		CLinearityBuffer Temp;
		if(!Temp.Resize(vSize))
		{
			return false;
		}

		//��������
		memcpy(Temp.Ptr(),Ptr(),mBufferSize);

		*this = Temp;
		return true;
	}

	//�������ô�С//���ݲ��ᱣ��
	bool Resize(int vSize)//��Ҫ��Ŀռ������
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
		//�ͷ��Լ����ڴ�
		if(mpBuffer!=NULL)
		{
			DEL_ONE(mpBuffer);
			mpBuffer=NULL;
			mBufferSize=0;
		}
	}
	int Size(){return mBufferSize;}

	//�õ�ָ��
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

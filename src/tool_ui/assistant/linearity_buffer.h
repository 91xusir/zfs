#ifndef LINEARITY_BUFFER_H
#define LINEARITY_BUFFER_H

#include "ref_pointer.h"

using fox::CRefArrayPtr;

//线性缓存
class CLinearityBuffer
{
public:
	CLinearityBuffer():mpBuffer(),mBufferSize(0){/*LOG("");*/};

	bool ExpandSize(int vSize)//扩大内存//数据保留
	{/*LOG("");*/
		if(vSize <= mBufferSize)
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
		memcpy(Temp, mpBuffer, mBufferSize);

		*this = Temp;
		return true;
	};

	//重新设置大小//数据不会保存
	bool Resize(int vSize)//比要求的空间大或等于
	{/*LOG("");*/
		if(vSize <= mBufferSize)
		{
			return true;
		}

		mpBuffer=NULL;
		mBufferSize=0;

		mpBuffer = new char[vSize];
		if(mpBuffer==NULL)
		{
			return false;
		}
		mBufferSize = vSize;
		return true;
	};

	void Release()
	{/*LOG("");*/
		//释放自己的内存
		mpBuffer = NULL;
		mBufferSize=0;
	};

	int Size(){return mBufferSize;}

	//得到指针
	operator char* () const
	{/*LOG("");*/
		return mpBuffer;
	};

// 	char& operator [](const int vIndex)使用引用，而非值传递
	char& operator [](const int& vIndex)
	{/*LOG("");*/
		return *(mpBuffer + vIndex);
	};
private:
	CRefArrayPtr<char>		mpBuffer;
	int						mBufferSize;
};

#endif
#ifndef LINEARITY_BUFFER_H
#define LINEARITY_BUFFER_H

#include "ref_pointer.h"

using fox::CRefArrayPtr;

//���Ի���
class CLinearityBuffer
{
public:
	CLinearityBuffer():mpBuffer(),mBufferSize(0){/*LOG("");*/};

	bool ExpandSize(int vSize)//�����ڴ�//���ݱ���
	{/*LOG("");*/
		if(vSize <= mBufferSize)
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
		memcpy(Temp, mpBuffer, mBufferSize);

		*this = Temp;
		return true;
	};

	//�������ô�С//���ݲ��ᱣ��
	bool Resize(int vSize)//��Ҫ��Ŀռ������
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
		//�ͷ��Լ����ڴ�
		mpBuffer = NULL;
		mBufferSize=0;
	};

	int Size(){return mBufferSize;}

	//�õ�ָ��
	operator char* () const
	{/*LOG("");*/
		return mpBuffer;
	};

// 	char& operator [](const int vIndex)ʹ�����ã�����ֵ����
	char& operator [](const int& vIndex)
	{/*LOG("");*/
		return *(mpBuffer + vIndex);
	};
private:
	CRefArrayPtr<char>		mpBuffer;
	int						mBufferSize;
};

#endif
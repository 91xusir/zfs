


#ifndef COMPRESS_MODE_H
#define COMPRESS_MODE_H

#include "filepack/game_pack.h"
#include "filepack/unpack_file.h"
#include "core/zlib.h"



//ѹ��ģʽ�Ľӿ�
class CCompress
{
public:
	CCompress(){};
	virtual ~CCompress(){};

	virtual CPack::ECompressMode GetMode()=0;//�õ�ѹ��ģʽ
	virtual int GetVersion()=0;//�õ�ѹ���汾


	virtual bool IsType(CPack::ECompressMode vType,int vVersion)=0;

	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)=0;
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)=0;
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize, CPack::SFileInfo* vpFileInfo)=0;


	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo)=0;
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)=0;					//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�



public:
	enum
	{
		DEFAULT_BUFFER_SIZE_4K=4*1024,
	};
};


//����Ҫѹ��//�Ƚ�����//��������
class CNULLCompress:public CCompress
{
public:
	enum
	{
		LAST_VERSION=0,//��ѹ�������°汾
	};

	CNULLCompress():CCompress(){};
	virtual ~CNULLCompress(){};

	virtual CPack::ECompressMode	GetMode(){return CPack::NULL_COMPRESS;};	//�õ�ѹ��ģʽ
	virtual int GetVersion(){return LAST_VERSION;};										//�õ�ѹ���汾
	virtual bool IsType(CPack::ECompressMode vType,int vVersion);

	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
		CPack::SFileInfo* vpFileInfo);

	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo);
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo);//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�
};


//�������ĸ����㷨
template <class T>
class CDeriveCompress:public CCompress
{
public:
	CDeriveCompress(){};
	virtual ~CDeriveCompress(){};

	virtual CPack::ECompressMode GetMode(){return mArithmetic.GetMode();};		//�õ�ѹ��ģʽ
	virtual int GetVersion(){return mArithmetic.GetVersion();};				//�õ�ѹ���汾

	virtual bool IsType(CPack::ECompressMode vType,int vVersion){return mArithmetic.IsType(vType,vVersion);};
	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
		CPack::SFileInfo* vpFileInfo);

	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo);
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo);//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�


private:
	CLinearityBuffer mOrigin;
	CLinearityBuffer mCompress;

	T   mArithmetic;
	int GetCompressSize(int vOriginSize){return mArithmetic.GetCompressSize(vOriginSize);};//�õ�ѹ����Ĵ�С��������ڵ�������ѹ����Ĵ�С
	bool CompressArithmetic(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)
	{
		return mArithmetic.Compress(vpIn,vInSize,vpOut,vrOutSize);
	};

	bool UnCompressArithmetic(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)
	{
		return mArithmetic.UnCompress(vpIn,vInSize,vpOut,vrOutSize);
	};
};


//-----------------------------------------------------------------------------------


//�㷨�ӿ�
class CArithmetic
{
public:
	CArithmetic(){};
	virtual ~CArithmetic(){};

	virtual CPack::ECompressMode GetMode()=0;	//�õ�ѹ��ģʽ
	virtual int GetVersion()=0;					//�õ�ѹ���汾

	virtual bool IsType(CPack::ECompressMode vType,int vVersion)=0;			//�Ƿ����������
	
	virtual int GetCompressSize(int vOriginSize)=0;									//�õ�ѹ����Ĵ�С��������ڵ�������ѹ����Ĵ�С
	virtual bool Compress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)=0;		//����ѹ�����С
	virtual bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)=0;	//���ؽ�ѹ���С
};



//lzw
class CLzwArithmetic:public CArithmetic
{
public:
	enum
	{
		LAST_VERSION=0,
	};
	CLzwArithmetic():CArithmetic(){};
	virtual ~CLzwArithmetic(){};

	virtual CPack::ECompressMode GetMode(){return CPack::LZW_COMPRESS;};//�õ�ѹ��ģʽ
	virtual int GetVersion(){return LAST_VERSION;};//�õ�ѹ���汾
	virtual bool IsType(CPack::ECompressMode vType,int vVersion)
	{
		return ((vType==CPack::LZW_COMPRESS) && (vVersion==LAST_VERSION)) ? true:false;
	}
	

	int GetCompressSize(int vOriginSize)//�õ�ѹ����Ĵ�С��������ڵ�������ѹ����Ĵ�С
	{
		return (vOriginSize*2+1024);	//ѹ����Ĵ�С//���ܻ��ԭ����
	};

	bool Compress(void* vpIn,int vInSize, void* vpOut,int& vrOutSize)//����ѹ�����С
	{
		vrOutSize = RtZCompress::Compress(vpIn,vInSize,vpOut);
		return true;
	};

	bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)//���ؽ�ѹ���С
	{
		vrOutSize=RtZCompress::Decompression(vpIn,vInSize,vpOut);
		return true;
	};
};


//zlib
class CZlibArithmetic:public CArithmetic
{
public:
	enum
	{
		LAST_VERSION=0,
	};

	CZlibArithmetic():CArithmetic(){};
	virtual ~CZlibArithmetic(){};

	virtual CPack::ECompressMode GetMode(){return CPack::ZLIB_COMPRESS;};	//�õ�ѹ��ģʽ
	virtual int GetVersion(){return LAST_VERSION;};				//�õ�ѹ���汾
	virtual bool IsType(CPack::ECompressMode vType,int vVersion)
	{
		return ((vType==CPack::ZLIB_COMPRESS) && (vVersion==LAST_VERSION)) ? true:false;
	}


	int GetCompressSize(int vOriginSize)						//�õ�ѹ����Ĵ�С��������ڵ�������ѹ����Ĵ�С
	{
		return vOriginSize*2+1024;
	}

	bool Compress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)			//����ѹ�����С
	{
		return (compress((Bytef*)vpOut,(uLongf*)&vrOutSize,(const Bytef*)vpIn,vInSize)==Z_OK);
	}

	bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)			//���ؽ�ѹ���С
	{
		return (uncompress((Bytef*)vpOut,(uLongf*)&vrOutSize,(const Bytef*)vpIn,vInSize)==Z_OK);
	}

};



//=========================================================================================

inline bool CNULLCompress::IsType(CPack::ECompressMode vType,int vVersion)
{
	if(vType==CPack::NULL_COMPRESS && vVersion==LAST_VERSION)
	{
		return true;
	}
	else
	{
		return false;
	}
};



inline bool CNULLCompress::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,CPack::SFileInfo* vpFileInfo)
{
	//static char Temp[DEFAULT_BUFFER_SIZE_4K];
	S_MEMDEF(Temp, DEFAULT_BUFFER_SIZE_4K)
	S_MEMPROTECTOR(Temp, DEFAULT_BUFFER_SIZE_4K, bMP)

	//��������//���õ�MD5ֵ//����Ҫѹ��
	RtMD5 Md5;
	//RtMD5Context context;

	if(!vrPack.BeginWrite((int)vrUnpackFile.OriginSize(),vpFileInfo))
	{
		return false;
	}

	size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//��������
	for(;ReadSize!=0;)
	{
		if(vrPack.Write(Temp,(int)ReadSize)!=(int)ReadSize)//������д��
		{
			return false;
		}

		Md5.Update((unsigned char*) Temp, (int)ReadSize);//����MD5
		ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//������������
	}

	vrPack.EndWrite();

	//�������ļ���Ϣ
	vpFileInfo->mOriginSize=(int)vrUnpackFile.OriginSize();
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	//vpFileInfo->mWriteTime=vrUnpackFile.WriteTime();
	//rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);
	Md5.Final();
	Md5.GetMD5((unsigned char*)vpFileInfo->mMD5);
	return true;
}


inline bool CNULLCompress::Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)
{
	int BufferSize=vrBuffer.DataSize();//Ҫ�ȱ��棬���������󣬻����
	//��������//���õ�MD5ֵ//��Ҫѹ��
	
	//RtMD5Context context;
	RtMD5 Md5;

	if(!vrPack.BeginWrite(vrBuffer.DataSize(),vpFileInfo))
	{
		return false;
	}

	char* pData;
	int ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		if(vrPack.Write(pData,ReadSize)!=ReadSize)
		{
			return false;
		}

		Md5.Update((unsigned char*) pData, (int)ReadSize);//����MD5
		ReadSize=vrBuffer.Read(pData);
	}

	vrPack.EndWrite();

	//�������ļ���Ϣ
	vpFileInfo->mOriginSize=BufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	/*
	//ȡ�õ�ǰʱ��
	time_t Now;
	time(&Now);
	vpFileInfo->mWriteTime=Now;
	*/
	Md5.Final();
	Md5.GetMD5((unsigned char*)vpFileInfo->mMD5);
	return true;
}

inline bool CNULLCompress::Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
					  CPack::SFileInfo* vpFileInfo)
{
	//��������//���õ�MD5ֵ//��Ҫѹ��
	RtMD5 Md5;
	//RtMD5Context context;

	if(!vrPack.OnceWrite(vpBuffer,vBufferSize,vpFileInfo))
	{
		return false;
	}

	Md5.Update ((unsigned char*) vpBuffer, vBufferSize);//����MD5
	//�������ļ���Ϣ
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	/*
	//ȡ�õ�ǰʱ��
	time_t Now;
	time(&Now);
	vpFileInfo->mWriteTime=Now;
	*/
	Md5.Final();
	Md5.GetMD5((unsigned char*)vpFileInfo->mMD5);
	return true;

}

inline bool CNULLCompress::UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo)
{
	return vrPack.OnceRead(vpBuffer,vpFileInfo);
}

inline CUnpackFile*	CNULLCompress::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�
{
	CUnpackFile* p=RT_NEW CMapUnpackFile(vrPack.mpPackFile,vpFileInfo);
	return p;
}


//----------------------------------------------------------------------------------
template<class T>
inline bool CDeriveCompress<T>::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=(int)vrUnpackFile.OriginSize();

	//���û���
	mOrigin.Resize(OriginSize);			//ѹ��ǰԭʼ��С

	//��������
	if((int)vrUnpackFile.GetBuffer(OriginSize,mOrigin.Ptr())!=OriginSize)
	{
		return false;
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
};

template<class T>
inline bool CDeriveCompress<T>::Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=vrBuffer.DataSize();

	//���û���
	mOrigin.Resize(OriginSize);			//ѹ��ǰԭʼ��С


	//��������
	char* pData;
	int AlreadyWrite=0;
	int ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		//���
		if(AlreadyWrite+ReadSize>OriginSize)
		{//�����ڴ泤��
			return false;
		}
		//���������Ի���
		memcpy(mOrigin.Ptr()+AlreadyWrite,pData,ReadSize);
		AlreadyWrite+=ReadSize;
		ReadSize=vrBuffer.Read(pData);//������������
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
};


template<class T>
inline bool CDeriveCompress<T>::Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
								   CPack::SFileInfo* vpFileInfo)
{
	//���û���
	int CompressSize=GetCompressSize(vBufferSize);
	mCompress.Resize(CompressSize);
	
	//��������//���õ�ԭʼMD5ֵ//Ҫѹ��
	RtMD5 Md5;


	Md5.Update((unsigned char*) vpBuffer, vBufferSize);//����MD5

	//ѹ��
	

	if(!CompressArithmetic(vpBuffer,vBufferSize,mCompress.Ptr(),CompressSize))
	{
		return false;
	}
//	int CompressSize=rtLzwComp(vpBuffer,vBufferSize,mCompress.Ptr());

	//д���
	if(!vrPack.OnceWrite(mCompress.Ptr(),CompressSize,vpFileInfo))
	{
		return false;
	}

	//�������ļ���Ϣ
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	//ȡ�õ�ǰʱ��
	//time_t Now;
	//time(&Now);
	//vpFileInfo->mWriteTime=Now;
	Md5.Final();
	Md5.GetMD5((unsigned char*)vpFileInfo->mMD5);
	return true;
};

template<class T>
inline bool CDeriveCompress<T>::UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo)
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//���û���
	mCompress.Resize(CompressSize);	//ѹ����Ĵ�С

	//��������
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return false;
	}

	//��ѹ��
	//if(rtLzwUncomp(mCompress.Ptr(),CompressSize,vpBuffer)!=vpFileInfo->mOriginSize)
	int OriginSize=vpFileInfo->mOriginSize;

	if(!UnCompressArithmetic(mCompress.Ptr(),CompressSize,vpBuffer,OriginSize))
	{
		return false;
	}
	if(OriginSize!=vpFileInfo->mOriginSize)
	{
		return false;
	}

	return true;
};

template<class T>
inline CUnpackFile*	CDeriveCompress<T>::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//���û���
	mCompress.Resize(CompressSize);	//ѹ����Ĵ�С

	//��������
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return NULL;
	}

	//��ѹ��
	CLinearityBuffer Temp;
	int UnCompressSize=vpFileInfo->mOriginSize;
	Temp.Resize(UnCompressSize);
	
	//if(rtLzwUncomp(mCompress.Ptr(),CompressSize,Temp.Ptr())!=vpFileInfo->mOriginSize)

	
	
	if(!UnCompressArithmetic(mCompress.Ptr(),CompressSize,Temp.Ptr(),UnCompressSize))
	{
		return NULL;
	}
	if(UnCompressSize!=vpFileInfo->mOriginSize)
	{
		return NULL;
	}


	//�õ�һ��CUnpackFile�ļ���
	CUnpackFile* p=RT_NEW CMemoryUnpackFile(Temp,vpFileInfo);

	return p;
};



































/*
inline bool CLzwCompress::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=(int)vrUnpackFile.OriginSize();

	//���û���
	mOrigin.Resize(OriginSize);			//ѹ��ǰԭʼ��С

	//��������
	if((int)vrUnpackFile.GetBuffer(OriginSize,mOrigin.Ptr())!=OriginSize)
	{
		return false;
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
}
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=vrBuffer.DataSize();

	//���û���
	mOrigin.Resize(OriginSize);			//ѹ��ǰԭʼ��С


	//��������
	char* pData;
	int AlreadyWrite=0;
	int ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		//���
		if(AlreadyWrite+ReadSize>OriginSize)
		{//�����ڴ泤��
			return false;
		}
		//���������Ի���
		memcpy(mOrigin.Ptr()+AlreadyWrite,pData,ReadSize);
		AlreadyWrite+=ReadSize;
		ReadSize=vrBuffer.Read(pData);//������������
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
}
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
					  CPack::SFileInfo* vpFileInfo)
{
	//���û���
	mCompress.Resize(vBufferSize*2+1024);	//ѹ����Ĵ�С//���ܻ��ԭ����

	//��������//���õ�MD5ֵ//Ҫѹ��
	RtMD5Context context;
	rtMD5Init (&context);

	//ѹ��
	int CompressSize=rtLzwComp(vpBuffer,vBufferSize,mCompress.Ptr());

	rtMD5Update (&context,(unsigned char*) mCompress.Ptr(), CompressSize);//����MD5

	//д���
	if(!vrPack.OnceWrite(mCompress.Ptr(),CompressSize,vpFileInfo))
	{
		return false;
	}

	//�������ļ���Ϣ
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	//ȡ�õ�ǰʱ��
	//time_t Now;
	//time(&Now);
	//vpFileInfo->mWriteTime=Now;
	rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);
	return true;
}
*/

/*
inline bool CLzwCompress::UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo)
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//���û���
	mCompress.Resize(CompressSize);	//ѹ����Ĵ�С

	//��������
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return false;
	}
	
	//��ѹ��
	if(rtLzwUncomp(mCompress.Ptr(),CompressSize,vpBuffer)!=vpFileInfo->mOriginSize)
	{
		return false;
	}

	return true;
}

*/
/*
inline CUnpackFile*	CLzwCompress::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//��ѹ�õ�һ���ļ�//Ҫɾ�����ļ�
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//���û���
	mCompress.Resize(CompressSize);	//ѹ����Ĵ�С

	//��������
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return NULL;
	}

	//��ѹ��
	CLinearityBuffer Temp;
	Temp.Resize(vpFileInfo->mOriginSize);
	if(rtLzwUncomp(mCompress.Ptr(),CompressSize,Temp.Ptr())!=vpFileInfo->mOriginSize)
	{
		return NULL;
	}

	//�õ�һ��CUnpackFile�ļ���
	CUnpackFile* p=RT_NEW CMemoryUnpackFile(Temp,vpFileInfo);

	return p;
}
*/











/*


virtual int Compress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)
{
memcpy(vpOutData,vpInData,vInDataLenth);
return vInDataLenth;
};

virtual int Uncompress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)
{
memcpy(vpOutData,vpInData,vInDataLenth);
return vInDataLenth;
};
*/


/*

//virtual CPack::ECompressMode Mode(){return CPack::LZW_COMPRESS;};
virtual int Compress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)
{return rtLzwComp(vpInData,vInDataLenth,vpOutData);};

virtual int Uncompress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)
{return rtLzwUncomp(vpInData,vInDataLenth,vpOutData);};
*/



//	virtual bool Uncompress();



/*
virtual int Compress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)=0;	//����������ݵĳ���
virtual int Uncompress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)=0;	//����������ݵĳ���

virtual bool Compress(CMemoryBuffer& vrInBuffer,CMemoryBuffer& vrOutBuffer)=0;		//���سɹ���ʧ��
virtual bool Uncompress(CMemoryBuffer& vrInBuffer,CMemoryBuffer& vrOutBuffer)=0;	//���سɹ���ʧ��

virtual bool Compress(void* vpInData,unsigned int vInDataLenth,CMemoryBuffer& vrOutBuffer)=0;	//����������ݵĳ���
virtual bool Uncompress(void* vpInData,unsigned int vInDataLenth,CMemoryBuffer& vrOutBuffer)=0;	//����������ݵĳ���
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)
{
//˵����ʱ����
static CLinearityBuffer LineBuffer;
static CMemoryBuffer Buffer;

//��������//���õ�MD5ֵ//Ҫѹ��
RtMD5Context context;
rtMD5Init (&context);

//��ջ���
Buffer.ClearData();

int CompressSize;//ѹ��������ݴ�С
char* pData;

//��������
int ReadSize=vrBuffer.Read(pData);
for(;ReadSize!=0;)
{
//�������Ի���
if(!LineBuffer.Resize(ReadSize*2))
{
return false;
}

//����ѹ��
CompressSize=rtLzwComp(pData,ReadSize,LineBuffer.Get());

//ѹ����Ĵ�Сд��
if(!Buffer.Write((char*)&CompressSize,sizeof(int)))
{
return false;
}

//ѹ���������д�뻺��
if(!Buffer.Write(LineBuffer.Get(),CompressSize))//����д��
{
return false;
}

rtMD5Update (&context,(unsigned char*) &CompressSize, sizeof(int));//����MD5
rtMD5Update (&context,(unsigned char*) LineBuffer.Get(),CompressSize);//����MD5
ReadSize=vrBuffer.Read(pData);//������������
}


//����ʱ����д���
vrPack.OnceWrite(Buffer,vpFileInfo);

//�������ļ���Ϣ
vpFileInfo->mOriginSize=(int)vrBuffer.DataSize();
vpFileInfo->mCompressMode=GetMode();
vpFileInfo->mCompressVersion=GetVersion();
rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);

return true;
}
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
//˵����ʱ����
static char Temp[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2];
static CMemoryBuffer Buffer;

//��������//���õ�MD5ֵ//Ҫѹ��
RtMD5Context context;
rtMD5Init (&context);

//��ջ���
Buffer.ClearData();

int CompressSize;//ѹ��������ݴ�С

//��������
size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);
for(;ReadSize!=0;)
{
//����ѹ��
CompressSize=rtLzwComp(Temp,(unsigned int)ReadSize,TempCompress);

//ѹ����Ĵ�Сд��
if(!Buffer.Write((char*)&CompressSize,sizeof(int)))
{
return false;
}

//ѹ���������д�뻺��
if(!Buffer.Write(TempCompress,CompressSize))//����д��
{
return false;
}

//ѹ�����MD5
rtMD5Update (&context,(unsigned char*) &CompressSize, sizeof(int));	//����MD5
rtMD5Update (&context,(unsigned char*) Temp, CompressSize);			//����MD5
ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);		//������������
}


//����ʱ����д���
vrPack.OnceWrite(Buffer,vpFileInfo);

//�������ļ���Ϣ
vpFileInfo->mOriginSize=(int)vrUnpackFile.Size();
vpFileInfo->mCompressMode=GetMode();
vpFileInfo->mCompressVersion=GetVersion();
rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);

return true;

}
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
int OriginSize=vrUnpackFile.OriginSize();

//���û���
mOrigin.Resize(OriginSize);			//ѹ��ǰԭʼ��С
mCompress.Resize(OriginSize*2);		//ѹ����Ĵ�С//���ܻ��ԭ����
mOrigin.ClearData();				//��ջ���
mCompress.ClearData();				//��ջ���

//��������//���õ�MD5ֵ//Ҫѹ��
RtMD5Context context;
rtMD5Init (&context);

//��������
if(vrUnpackFile.GetBuffer(OriginSize,Buffer.Get())!=OriginSize)
{
return false;
}

//����ѹ��
int CompressSize=rtLzwComp(mOrigin.Get(),OriginSize,mCompress.Get());

//ѹ�����MD5
rtMD5Update (&context,(unsigned char*) mCompress.Get(), CompressSize);	//����MD5

//����ʱ����д���
vrPack.OnceWrite(mCompress.Get(),CompressSize,vpFileInfo);

//�������ļ���Ϣ
vpFileInfo->mOriginSize=(int)vrUnpackFile.OriginSize();
vpFileInfo->mCompressMode=GetMode();
vpFileInfo->mCompressVersion=GetVersion();
rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);

return true;
}
*/





#endif//COMPRESS_MODE_H

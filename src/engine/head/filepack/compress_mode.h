


#ifndef COMPRESS_MODE_H
#define COMPRESS_MODE_H

#include "filepack/game_pack.h"
#include "filepack/unpack_file.h"
#include "core/zlib.h"



//压缩模式的接口
class CCompress
{
public:
	CCompress(){};
	virtual ~CCompress(){};

	virtual CPack::ECompressMode GetMode()=0;//得到压缩模式
	virtual int GetVersion()=0;//得到压缩版本


	virtual bool IsType(CPack::ECompressMode vType,int vVersion)=0;

	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)=0;
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)=0;
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize, CPack::SFileInfo* vpFileInfo)=0;


	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo)=0;
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)=0;					//解压得到一个文件//要删除该文件



public:
	enum
	{
		DEFAULT_BUFFER_SIZE_4K=4*1024,
	};
};


//不需要压缩//比较特殊//单独处理
class CNULLCompress:public CCompress
{
public:
	enum
	{
		LAST_VERSION=0,//无压缩的最新版本
	};

	CNULLCompress():CCompress(){};
	virtual ~CNULLCompress(){};

	virtual CPack::ECompressMode	GetMode(){return CPack::NULL_COMPRESS;};	//得到压缩模式
	virtual int GetVersion(){return LAST_VERSION;};										//得到压缩版本
	virtual bool IsType(CPack::ECompressMode vType,int vVersion);

	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
		CPack::SFileInfo* vpFileInfo);

	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo);
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo);//解压得到一个文件//要删除该文件
};


//派生出的各种算法
template <class T>
class CDeriveCompress:public CCompress
{
public:
	CDeriveCompress(){};
	virtual ~CDeriveCompress(){};

	virtual CPack::ECompressMode GetMode(){return mArithmetic.GetMode();};		//得到压缩模式
	virtual int GetVersion(){return mArithmetic.GetVersion();};				//得到压缩版本

	virtual bool IsType(CPack::ECompressMode vType,int vVersion){return mArithmetic.IsType(vType,vVersion);};
	virtual bool Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo);
	virtual bool Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
		CPack::SFileInfo* vpFileInfo);

	virtual bool			UnCompress(CPack& vrPack,void* vpBuffer,const CPack::SFileInfo* vpFileInfo);
	virtual CUnpackFile*	UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo);//解压得到一个文件//要删除该文件


private:
	CLinearityBuffer mOrigin;
	CLinearityBuffer mCompress;

	T   mArithmetic;
	int GetCompressSize(int vOriginSize){return mArithmetic.GetCompressSize(vOriginSize);};//得到压缩后的大小，必须大于等于真正压缩后的大小
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


//算法接口
class CArithmetic
{
public:
	CArithmetic(){};
	virtual ~CArithmetic(){};

	virtual CPack::ECompressMode GetMode()=0;	//得到压缩模式
	virtual int GetVersion()=0;					//得到压缩版本

	virtual bool IsType(CPack::ECompressMode vType,int vVersion)=0;			//是否是这个类型
	
	virtual int GetCompressSize(int vOriginSize)=0;									//得到压缩后的大小，必须大于等于真正压缩后的大小
	virtual bool Compress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)=0;		//返回压缩后大小
	virtual bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)=0;	//返回解压后大小
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

	virtual CPack::ECompressMode GetMode(){return CPack::LZW_COMPRESS;};//得到压缩模式
	virtual int GetVersion(){return LAST_VERSION;};//得到压缩版本
	virtual bool IsType(CPack::ECompressMode vType,int vVersion)
	{
		return ((vType==CPack::LZW_COMPRESS) && (vVersion==LAST_VERSION)) ? true:false;
	}
	

	int GetCompressSize(int vOriginSize)//得到压缩后的大小，必须大于等于真正压缩后的大小
	{
		return (vOriginSize*2+1024);	//压缩后的大小//可能会比原来大
	};

	bool Compress(void* vpIn,int vInSize, void* vpOut,int& vrOutSize)//返回压缩后大小
	{
		vrOutSize = RtZCompress::Compress(vpIn,vInSize,vpOut);
		return true;
	};

	bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)//返回解压后大小
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

	virtual CPack::ECompressMode GetMode(){return CPack::ZLIB_COMPRESS;};	//得到压缩模式
	virtual int GetVersion(){return LAST_VERSION;};				//得到压缩版本
	virtual bool IsType(CPack::ECompressMode vType,int vVersion)
	{
		return ((vType==CPack::ZLIB_COMPRESS) && (vVersion==LAST_VERSION)) ? true:false;
	}


	int GetCompressSize(int vOriginSize)						//得到压缩后的大小，必须大于等于真正压缩后的大小
	{
		return vOriginSize*2+1024;
	}

	bool Compress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)			//返回压缩后大小
	{
		return (compress((Bytef*)vpOut,(uLongf*)&vrOutSize,(const Bytef*)vpIn,vInSize)==Z_OK);
	}

	bool UnCompress(void* vpIn,int vInSize,void* vpOut,int& vrOutSize)			//返回解压后大小
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

	//读出数据//并得到MD5值//不需要压缩
	RtMD5 Md5;
	//RtMD5Context context;

	if(!vrPack.BeginWrite((int)vrUnpackFile.OriginSize(),vpFileInfo))
	{
		return false;
	}

	size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//读出数据
	for(;ReadSize!=0;)
	{
		if(vrPack.Write(Temp,(int)ReadSize)!=(int)ReadSize)//将数据写入
		{
			return false;
		}

		Md5.Update((unsigned char*) Temp, (int)ReadSize);//现算MD5
		ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//继续读出数据
	}

	vrPack.EndWrite();

	//填充相关文件信息
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
	int BufferSize=vrBuffer.DataSize();//要先保存，将来读过后，会减少
	//读出数据//并得到MD5值//不要压缩
	
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

		Md5.Update((unsigned char*) pData, (int)ReadSize);//现算MD5
		ReadSize=vrBuffer.Read(pData);
	}

	vrPack.EndWrite();

	//填充相关文件信息
	vpFileInfo->mOriginSize=BufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	/*
	//取得当前时间
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
	//读出数据//并得到MD5值//不要压缩
	RtMD5 Md5;
	//RtMD5Context context;

	if(!vrPack.OnceWrite(vpBuffer,vBufferSize,vpFileInfo))
	{
		return false;
	}

	Md5.Update ((unsigned char*) vpBuffer, vBufferSize);//现算MD5
	//填充相关文件信息
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	/*
	//取得当前时间
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

inline CUnpackFile*	CNULLCompress::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//解压得到一个文件//要删除该文件
{
	CUnpackFile* p=RT_NEW CMapUnpackFile(vrPack.mpPackFile,vpFileInfo);
	return p;
}


//----------------------------------------------------------------------------------
template<class T>
inline bool CDeriveCompress<T>::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=(int)vrUnpackFile.OriginSize();

	//设置缓存
	mOrigin.Resize(OriginSize);			//压缩前原始大小

	//读出数据
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

	//设置缓存
	mOrigin.Resize(OriginSize);			//压缩前原始大小


	//读出数据
	char* pData;
	int AlreadyWrite=0;
	int ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		//检查
		if(AlreadyWrite+ReadSize>OriginSize)
		{//超过内存长度
			return false;
		}
		//拷贝到线性缓存
		memcpy(mOrigin.Ptr()+AlreadyWrite,pData,ReadSize);
		AlreadyWrite+=ReadSize;
		ReadSize=vrBuffer.Read(pData);//继续读出数据
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
};


template<class T>
inline bool CDeriveCompress<T>::Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
								   CPack::SFileInfo* vpFileInfo)
{
	//设置缓存
	int CompressSize=GetCompressSize(vBufferSize);
	mCompress.Resize(CompressSize);
	
	//读出数据//并得到原始MD5值//要压缩
	RtMD5 Md5;


	Md5.Update((unsigned char*) vpBuffer, vBufferSize);//现算MD5

	//压缩
	

	if(!CompressArithmetic(vpBuffer,vBufferSize,mCompress.Ptr(),CompressSize))
	{
		return false;
	}
//	int CompressSize=rtLzwComp(vpBuffer,vBufferSize,mCompress.Ptr());

	//写入包
	if(!vrPack.OnceWrite(mCompress.Ptr(),CompressSize,vpFileInfo))
	{
		return false;
	}

	//填充相关文件信息
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	//取得当前时间
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

	//设置缓存
	mCompress.Resize(CompressSize);	//压缩后的大小

	//读出数据
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return false;
	}

	//解压缩
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
inline CUnpackFile*	CDeriveCompress<T>::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//解压得到一个文件//要删除该文件
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//设置缓存
	mCompress.Resize(CompressSize);	//压缩后的大小

	//读出数据
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return NULL;
	}

	//解压缩
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


	//得到一个CUnpackFile文件　
	CUnpackFile* p=RT_NEW CMemoryUnpackFile(Temp,vpFileInfo);

	return p;
};



































/*
inline bool CLzwCompress::Compress(CPack& vrPack,CUnpackFile& vrUnpackFile,	CPack::SFileInfo* vpFileInfo)
{
	int OriginSize=(int)vrUnpackFile.OriginSize();

	//设置缓存
	mOrigin.Resize(OriginSize);			//压缩前原始大小

	//读出数据
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

	//设置缓存
	mOrigin.Resize(OriginSize);			//压缩前原始大小


	//读出数据
	char* pData;
	int AlreadyWrite=0;
	int ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		//检查
		if(AlreadyWrite+ReadSize>OriginSize)
		{//超过内存长度
			return false;
		}
		//拷贝到线性缓存
		memcpy(mOrigin.Ptr()+AlreadyWrite,pData,ReadSize);
		AlreadyWrite+=ReadSize;
		ReadSize=vrBuffer.Read(pData);//继续读出数据
	}

	return Compress(vrPack,mOrigin.Ptr(),OriginSize,vpFileInfo);
}
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,	void* vpBuffer,	int vBufferSize,
					  CPack::SFileInfo* vpFileInfo)
{
	//设置缓存
	mCompress.Resize(vBufferSize*2+1024);	//压缩后的大小//可能会比原来大

	//读出数据//并得到MD5值//要压缩
	RtMD5Context context;
	rtMD5Init (&context);

	//压缩
	int CompressSize=rtLzwComp(vpBuffer,vBufferSize,mCompress.Ptr());

	rtMD5Update (&context,(unsigned char*) mCompress.Ptr(), CompressSize);//现算MD5

	//写入包
	if(!vrPack.OnceWrite(mCompress.Ptr(),CompressSize,vpFileInfo))
	{
		return false;
	}

	//填充相关文件信息
	vpFileInfo->mOriginSize=vBufferSize;
	vpFileInfo->mCompressMode=GetMode();
	vpFileInfo->mCompressVersion=GetVersion();
	//取得当前时间
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

	//设置缓存
	mCompress.Resize(CompressSize);	//压缩后的大小

	//读出数据
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return false;
	}
	
	//解压缩
	if(rtLzwUncomp(mCompress.Ptr(),CompressSize,vpBuffer)!=vpFileInfo->mOriginSize)
	{
		return false;
	}

	return true;
}

*/
/*
inline CUnpackFile*	CLzwCompress::UnCompress(CPack& vrPack,const CPack::SFileInfo* vpFileInfo)//解压得到一个文件//要删除该文件
{
	int CompressSize=vpFileInfo->mSizeInPack;

	//设置缓存
	mCompress.Resize(CompressSize);	//压缩后的大小

	//读出数据
	if(!vrPack.OnceRead(mCompress.Ptr(),vpFileInfo))
	{
		return NULL;
	}

	//解压缩
	CLinearityBuffer Temp;
	Temp.Resize(vpFileInfo->mOriginSize);
	if(rtLzwUncomp(mCompress.Ptr(),CompressSize,Temp.Ptr())!=vpFileInfo->mOriginSize)
	{
		return NULL;
	}

	//得到一个CUnpackFile文件　
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
virtual int Compress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)=0;	//返回输出数据的长度
virtual int Uncompress(void* vpInData,unsigned int vInDataLenth,void* vpOutData)=0;	//返回输出数据的长度

virtual bool Compress(CMemoryBuffer& vrInBuffer,CMemoryBuffer& vrOutBuffer)=0;		//返回成功或失败
virtual bool Uncompress(CMemoryBuffer& vrInBuffer,CMemoryBuffer& vrOutBuffer)=0;	//返回成功或失败

virtual bool Compress(void* vpInData,unsigned int vInDataLenth,CMemoryBuffer& vrOutBuffer)=0;	//返回输出数据的长度
virtual bool Uncompress(void* vpInData,unsigned int vInDataLenth,CMemoryBuffer& vrOutBuffer)=0;	//返回输出数据的长度
*/

/*
inline bool CLzwCompress::Compress(CPack& vrPack,CMemoryBuffer& vrBuffer,	CPack::SFileInfo* vpFileInfo)
{
//说明临时缓存
static CLinearityBuffer LineBuffer;
static CMemoryBuffer Buffer;

//读出数据//并得到MD5值//要压缩
RtMD5Context context;
rtMD5Init (&context);

//清空缓存
Buffer.ClearData();

int CompressSize;//压缩后的数据大小
char* pData;

//读出数据
int ReadSize=vrBuffer.Read(pData);
for(;ReadSize!=0;)
{
//扩大线性缓存
if(!LineBuffer.Resize(ReadSize*2))
{
return false;
}

//数据压缩
CompressSize=rtLzwComp(pData,ReadSize,LineBuffer.Get());

//压缩后的大小写入
if(!Buffer.Write((char*)&CompressSize,sizeof(int)))
{
return false;
}

//压缩后的数据写入缓存
if(!Buffer.Write(LineBuffer.Get(),CompressSize))//数据写入
{
return false;
}

rtMD5Update (&context,(unsigned char*) &CompressSize, sizeof(int));//现算MD5
rtMD5Update (&context,(unsigned char*) LineBuffer.Get(),CompressSize);//现算MD5
ReadSize=vrBuffer.Read(pData);//继续读出数据
}


//将临时缓存写入包
vrPack.OnceWrite(Buffer,vpFileInfo);

//填充相关文件信息
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
//说明临时缓存
static char Temp[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2];
static CMemoryBuffer Buffer;

//读出数据//并得到MD5值//要压缩
RtMD5Context context;
rtMD5Init (&context);

//清空缓存
Buffer.ClearData();

int CompressSize;//压缩后的数据大小

//读出数据
size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);
for(;ReadSize!=0;)
{
//数据压缩
CompressSize=rtLzwComp(Temp,(unsigned int)ReadSize,TempCompress);

//压缩后的大小写入
if(!Buffer.Write((char*)&CompressSize,sizeof(int)))
{
return false;
}

//压缩后的数据写入缓存
if(!Buffer.Write(TempCompress,CompressSize))//数据写入
{
return false;
}

//压缩后的MD5
rtMD5Update (&context,(unsigned char*) &CompressSize, sizeof(int));	//现算MD5
rtMD5Update (&context,(unsigned char*) Temp, CompressSize);			//现算MD5
ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);		//继续读出数据
}


//将临时缓存写入包
vrPack.OnceWrite(Buffer,vpFileInfo);

//填充相关文件信息
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

//设置缓存
mOrigin.Resize(OriginSize);			//压缩前原始大小
mCompress.Resize(OriginSize*2);		//压缩后的大小//可能会比原来大
mOrigin.ClearData();				//清空缓存
mCompress.ClearData();				//清空缓存

//读出数据//并得到MD5值//要压缩
RtMD5Context context;
rtMD5Init (&context);

//读出数据
if(vrUnpackFile.GetBuffer(OriginSize,Buffer.Get())!=OriginSize)
{
return false;
}

//数据压缩
int CompressSize=rtLzwComp(mOrigin.Get(),OriginSize,mCompress.Get());

//压缩后的MD5
rtMD5Update (&context,(unsigned char*) mCompress.Get(), CompressSize);	//现算MD5

//将临时缓存写入包
vrPack.OnceWrite(mCompress.Get(),CompressSize,vpFileInfo);

//填充相关文件信息
vpFileInfo->mOriginSize=(int)vrUnpackFile.OriginSize();
vpFileInfo->mCompressMode=GetMode();
vpFileInfo->mCompressVersion=GetVersion();
rtMD5Final ((unsigned char*)vpFileInfo->mMD5, &context);

return true;
}
*/





#endif//COMPRESS_MODE_H


#ifndef UNPACK_FILE_H
#define UNPACK_FILE_H

#include "pack_file.h"
#include "game_pack.h"



class CUnpackFile
{
public:


	CUnpackFile(){};
	virtual ~CUnpackFile(){};

	//virtual bool Create(CPackFile* vpPackFile, CPack::SFileInfo* vpFileInfo)=0;
	virtual bool Seek(long vOffset,int vOrigin=SEEK_CUR)=0;

	virtual const CPack::SFileInfo& GetInfo()=0;
	virtual const string& Name()=0;
	virtual const size_t SizeInPack()=0;//文件在包中的大小
	virtual const size_t OriginSize()=0;//文件原来的大小
	virtual const long   WriteTime()=0; //文件的修改时间

	virtual const size_t Tell()=0;
	virtual const size_t GetBuffer(const int vByteCount,void* vpBuffer)=0;

	virtual CLinearityBuffer GetBuffer()=0;//将文件内容复制到线性内存中//文件已经关闭
    virtual void GetBuffer(CLinearityBuffer& vBuffer)=0;//将文件内容复制到线性内存中//文件已经关闭


	template<class T>
		CUnpackFile& operator>> (T& vrT)
	{
		GetBuffer(sizeof(T),&vrT);
		return *this;
	};

	virtual CUnpackFile& operator >> ( string& vrString ) = 0;

private:
//	virtual	CUnpackFile& operator>>( CParameter* vpParameter ) = 0;
};






class CMapUnpackFile:public CUnpackFile
{
public:
	//CMapUnpackFile();
	CMapUnpackFile(CPackFile* vpPackFile, const CPack::SFileInfo* vpFileInfo);
	virtual ~CMapUnpackFile();

	//virtual void Create(CPackFile* vpPackFile, CPack::SFileInfo* vpFileInfo);
	virtual bool Seek(long vOffset,int vOrigin=SEEK_CUR);
	virtual const CPack::SFileInfo& GetInfo();
	virtual const string& Name();
	virtual const size_t SizeInPack();//文件在包中的大小
	virtual const size_t OriginSize();//文件原来的大小
	virtual const long   WriteTime(); //文件的修改时间

	virtual const size_t Tell();
	virtual const size_t GetBuffer(const int vByteCount,void* vpBuffer);
	virtual CLinearityBuffer GetBuffer();//将文件内容复制到线性内存中//文件同时关闭
    virtual void GetBuffer(CLinearityBuffer& vBuffer);//将文件内容复制到线性内存中//文件同时关闭

	virtual CUnpackFile& operator >> ( string& vrString ) ;

private:

	CPackFile*					mpPackFile;
	const CPack::SFileInfo*		mpFileInfo;
	size_t						mCurrent;
	
};



class CMemoryUnpackFile:public CUnpackFile
{
public:
	//CMemoryUnpackFile();
	CMemoryUnpackFile(CLinearityBuffer& vrBuffer,const CPack::SFileInfo* vpFileInfo);
	virtual ~CMemoryUnpackFile();

	virtual bool Seek(long vOffset,int vOrigin=SEEK_CUR);
	virtual const CPack::SFileInfo& GetInfo();
	virtual const string& Name();
	virtual const size_t SizeInPack();//文件在包中的大小
	virtual const size_t OriginSize();//文件原来的大小
	virtual const long   WriteTime(); //文件的修改时间

	virtual const size_t Tell();
	virtual const size_t GetBuffer(const int vByteCount,void* vpBuffer);
	virtual CLinearityBuffer GetBuffer();//将文件内容复制到线性内存中//文件同时关闭
    virtual void GetBuffer(CLinearityBuffer& vBuffer);//将文件内容复制到线性内存中//文件已经关闭

	virtual CUnpackFile& operator >> (	string& vrString ) ;
	
private:

	CLinearityBuffer			mBuffer;
	const CPack::SFileInfo*		mpFileInfo;
	size_t						mCurrent;
};


/////////////////////////////////////////////////////////////////////////////////////////
/*
inline CMemoryUnpackFile::CMemoryUnpackFile()
:CUnpackFile(),mBuffer(),mpFileInfo(NULL),mCurrent(0)
{
}
*/

inline CMemoryUnpackFile::CMemoryUnpackFile(CLinearityBuffer& vrBuffer,const CPack::SFileInfo* vpFileInfo)
:CUnpackFile(),mBuffer(vrBuffer),mpFileInfo(vpFileInfo),mCurrent(0)
{
}

inline  CMemoryUnpackFile::~CMemoryUnpackFile()
{
}


inline const CPack::SFileInfo& CMemoryUnpackFile::GetInfo()
{
	return *mpFileInfo;
}


inline const string& CMemoryUnpackFile::Name()
{
	static string NullString("");

	return mpFileInfo==NULL ? NullString : mpFileInfo->mName;
}

inline const size_t CMemoryUnpackFile::OriginSize()//文件原来的大小
{
	return mpFileInfo==NULL ? 0 : mpFileInfo->mOriginSize;
}

inline const size_t CMemoryUnpackFile::SizeInPack()//文件在包中的大小
{
	return mpFileInfo==NULL ? 0 : mpFileInfo->mSizeInPack;
}

inline const long   CMemoryUnpackFile::WriteTime() //文件的修改时间
{
	return mpFileInfo==NULL ? -1 : mpFileInfo->mWriteTime;
}

inline const size_t CMemoryUnpackFile::Tell()
{
	return mCurrent;
}

inline bool CMemoryUnpackFile::Seek(long vOffset,int vOrigin)
{
	if(mBuffer.Ptr()==NULL)
	{
		return false;
	}

	if(vOrigin==SEEK_SET)
	{
		mCurrent=vOffset;
	}
	else if(vOrigin==SEEK_CUR)
	{
		mCurrent+=vOffset;
	}
	else if(vOrigin==SEEK_END)
	{
		mCurrent=vOffset+mpFileInfo->mOriginSize;
	}
	else 
	{
		//err unknow origin
		return false;
	}

	if(mCurrent<0)
	{
		mCurrent=0;
		//err seek outside
		return false;
	}
	return true;
}

static CThreadCriticalSection ThreadLock1;
inline const size_t CMemoryUnpackFile::GetBuffer(const int vByteCount,void* vpBuffer)
{
	if(mpFileInfo==NULL)
	{
		return 0;
	}

	if(vByteCount<=0)
	{
		return 0;
	}

	ThreadLock1.Lock();
	size_t	Size	= mpFileInfo->mOriginSize;
	size_t	Begin	= ( mCurrent >= Size ) ? Size : mCurrent;							//得到开始[不超过该文件的size]
	size_t  End		= ( mCurrent + vByteCount >= Size ) ? Size : mCurrent + vByteCount; //得到结尾[不超过该文件的size]

	size_t  Result  =End-Begin;

	memcpy(vpBuffer,mBuffer.Ptr()+Begin,Result);
	mCurrent=End;
	ThreadLock1.UnLock();

	return Result;
}

inline CLinearityBuffer CMemoryUnpackFile::GetBuffer()//将文件内容复制到线性内存中//文件同时关闭
{
	return mBuffer;
}

inline void CMemoryUnpackFile::GetBuffer(CLinearityBuffer& vBuffer)//将文件内容复制到线性内存中//文件同时关闭
{
	vBuffer = mBuffer;
}

inline CUnpackFile& CMemoryUnpackFile::operator >> ( string& vrString ) 
{
	size_t StringLength;
	(CUnpackFile&)(*this) >> (StringLength);

	vrString="";//清空


	char* pData=mBuffer.Ptr()+mCurrent;
	char* pEnd=pData+StringLength;

	for(;pData!=pEnd;++pData)
	{
		vrString+=*pData;
	}
	mCurrent+=StringLength;
	return *this;
}

/*

inline CUnpackFile& CMemoryUnpackFile::operator >> (CParameter* vpParameter)
{
	GetBuffer((int)vpParameter->Size(),vpParameter->GetTPtr());
	return *this;
}
*/




//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
inline CMapUnpackFile::CMapUnpackFile()
:CUnpackFile(),mpPackFile(NULL),mpFileInfo(NULL),mCurrent(0)
{
}
*/



inline CMapUnpackFile::CMapUnpackFile(CPackFile* vpPackFile,const CPack::SFileInfo* vpFileInfo)
:CUnpackFile(),mpPackFile(vpPackFile),mpFileInfo(vpFileInfo),mCurrent(0)
{

}


inline CMapUnpackFile::~CMapUnpackFile()
{
}


inline const CPack::SFileInfo& CMapUnpackFile::GetInfo()
{
	return *mpFileInfo;
}

inline const string& CMapUnpackFile::Name()
{
	static string NullString("");

	return mpFileInfo==NULL ? NullString : mpFileInfo->mName;
}



inline const size_t CMapUnpackFile::OriginSize()
{
	return mpFileInfo==NULL ? 0 : mpFileInfo->mOriginSize;
}


inline const size_t CMapUnpackFile::SizeInPack()
{
	return mpFileInfo==NULL ? 0 : mpFileInfo->mSizeInPack;
}

inline const long CMapUnpackFile::WriteTime()
{
	return mpFileInfo==NULL ? -1 : mpFileInfo->mWriteTime;
}


inline const size_t CMapUnpackFile::Tell()
{
	return mCurrent;
}

inline bool CMapUnpackFile::Seek(long vOffset,int vOrigin)
{
	if(mpPackFile==NULL)
	{
		return false;
	}

	if(vOrigin==SEEK_SET)
	{
		mCurrent=vOffset;
	}
	else if(vOrigin==SEEK_CUR)
	{
		mCurrent+=vOffset;
	}
	else if(vOrigin==SEEK_END)
	{
		mCurrent=vOffset+mpFileInfo->mOriginSize;
	}
	else 
	{
		//err unknow origin
		return false;
	}

	if(mCurrent<0)
	{
		mCurrent=0;
		//err seek outside
		return false;
	}
	return true;
}


extern CThreadCriticalSection gRealReadFile;

inline const size_t CMapUnpackFile::GetBuffer(const int vByteCount,void* vpBuffer)
{
	if(mpFileInfo==NULL)
	{
		return 0;
	}

	if(vByteCount<=0)
	{
		return 0;
	}


	gRealReadFile.Lock();
	size_t	Size	= mpFileInfo->mOriginSize;
	size_t	Begin	= ( mCurrent >= Size ) ? Size : mCurrent;							//得到开始[不超过该文件的size]
	size_t  End		= ( mCurrent + vByteCount >= Size ) ? Size : mCurrent + vByteCount; //得到结尾[不超过该文件的size]

	size_t  Result  =End-Begin;


	size_t RealBegin=Begin+mpFileInfo->mOffset;
	mpPackFile->Seek((long)RealBegin,SEEK_SET);

	size_t RealResult = mpPackFile->GetBuffer((int)Result,vpBuffer);
	mCurrent=Begin+RealResult;
	gRealReadFile.UnLock();


	return RealResult ;
}


inline CLinearityBuffer CMapUnpackFile::GetBuffer()//将文件内容复制到线性内存中//文件同时关闭
{
	int Size=(int)OriginSize();
	CLinearityBuffer Buffer;
	Buffer.Resize(Size);
	Seek(0,SEEK_SET);
	GetBuffer((const int )OriginSize(),Buffer.Ptr());
	return Buffer;
}


inline void CMapUnpackFile::GetBuffer(CLinearityBuffer& vBuffer)//将文件内容复制到线性内存中//文件同时关闭
{
    int Size=(int)OriginSize();
    vBuffer.Resize(Size);
    Seek(0,SEEK_SET);
    GetBuffer((const int )OriginSize(),vBuffer.Ptr());
}


inline CUnpackFile& CMapUnpackFile::operator >> ( string& vrString ) 
{
	size_t  StringLength;
	(CUnpackFile&)(*this) >>  (StringLength);
	vrString=mpPackFile->GetString((int)StringLength);
	mCurrent+=StringLength;
	return *this;
}





#endif//UNPACK_FILE_H


#ifndef PACK_FILE_H 
#define PACK_FILE_H

#include "core/rt2_core.h"
#include "filepack/pack_error.h"
#include "filepack/ansi_file.h"
#include "filepack/memory_buffer.h"
#include "filepack/profile.h"


using std::string;


class CThreadCriticalSection
{
public:
	CThreadCriticalSection(){RT_INIT_CS(&mCriticalSection);};
	~CThreadCriticalSection(){RT_DELETE_CS(&mCriticalSection);};

	void Lock(){RT_LOCK_CS(&mCriticalSection);};
	void UnLock(){RT_UNLOCK_CS(&mCriticalSection);};
private:
	RT_TYPE_CS						mCriticalSection; //
};


class CMemoryBuffer;
class CPackFile
{
public:
	//操作系统相关static bool ChangeMode(const char vpFileName[],int vMode);//改变文件属性//SYS\STAT.H

	static bool ChangeMode(const char vpFileName[],DWORD vMode);//用rt_core的代码，同操作系统无关
	static bool DeleteFile(const string& vrFileName);
	static bool IsExist(const string vFileName);
	static const long WriteTime(const string& vrFileName);


	CPackFile();
	virtual ~CPackFile();

	bool Create(const string& vrFileName);
	bool Open(const string& vrFileName,const string vMode="rb+");
	void Close();

	virtual bool SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset)=0;
	virtual const string& Name()=0;
	
	virtual const long  Size()=0;
	virtual const long	Tell()=0;
	virtual int			Seek(long vOffset,int vOrigin=SEEK_CUR)=0;
	virtual void		Flush()=0;

	virtual const char*	GetString(int vCharCount)=0;
	virtual bool	PutString(const char* vpString,const size_t vStringLength=0)=0;
	virtual size_t	GetBuffer(const int vByteCount,void* vpBuffer)=0;
	virtual size_t	PutBuffer(const void* vpBuffer,const int vBufferSize)=0;

	
	
	
	//存储在PackFile中该类型的大小
	template<class T>
		size_t SizeInPackFile(const T& vrT);

	template<class T>
	CPackFile& operator <<   (const T& vrT);

	template<class T>
	CPackFile& operator>>   (T& vrT);

	//存储在PackFile中该类型的大小
	size_t SizeInPackFile(const string& vrString);

	CPackFile& operator<< (const string& vrString);
	CPackFile& operator>> (string& vrString);

	bool	PutBuffer( CMemoryBuffer& vrBuffer);//将内存写入

	const long WriteTime();

	void AddReference();
	int  DeleteReference();

protected:
	virtual void OnClose()=0;
private:
	virtual bool OnCreate(const string& vrFileName)=0;
	virtual bool OnOpen(const string& vrFileName,const string vMode="rb+")=0;
	

	int mReferenceCount;
};



class CAnsiPackFile:public CPackFile
{
public:
	CAnsiPackFile();
	virtual ~CAnsiPackFile();

	virtual bool SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset){return false;};
	virtual const string& Name();

	virtual const long  Size();
	virtual const long	Tell();
	virtual int			Seek(long vOffset,int vOrigin=SEEK_CUR);
	virtual void		Flush();

	virtual const char*	GetString(int vCharCount);
	virtual bool	PutString(const char* vpString,const size_t vStringLength=0);

	virtual size_t	GetBuffer(const int vByteCount,void* vpBuffer);
	virtual size_t	PutBuffer(const void* vpBuffer,const int vBufferSize);

protected:
	virtual void OnClose();
private:

	virtual bool OnCreate(const string& vrFileName);
	virtual bool OnOpen(const string& vrFileName,const string vMode="rb+");

	
protected:
	CAnsiFile mFile;
};

class CMemoryPackFile:public CPackFile
{
public:
	CMemoryPackFile();
	virtual ~CMemoryPackFile();

	virtual bool SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset){return false;};
	virtual const string& Name();

	virtual const long  Size();
	virtual const long	Tell();
	virtual int			Seek(long vOffset,int vOrigin=SEEK_CUR);
	virtual void		Flush();

	virtual const char*	GetString(int vCharCount);
	virtual bool	PutString(const char* vpString,const size_t vStringLength=0);

	virtual size_t	GetBuffer(const int vByteCount,void* vpBuffer);
	virtual size_t	PutBuffer(const void* vpBuffer,const int vBufferSize);

	bool Resize(int vSize){return mBuffer.Resize(vSize);};//要求多大的内存空间

protected:

	CLinearityBuffer mBuffer;
	int				 mSize;

	virtual void OnClose();
private:
	virtual bool OnCreate(const string& vrFileName);
	virtual bool OnOpen(const string& vrFileName,const string vMode="rb+");

	

	bool CheckMemory(int vSize);//如果写内存文件就必须要检查当前内存是否够用


	string			 mName;

	int				 mCurrent;
	bool			 mTextMode;
};



class CFlagPackFile:public CAnsiPackFile//带有标志的pack文件
{
public:
	CFlagPackFile()
	:CAnsiPackFile(),mFlagBegin(0),mSet(false),mOpenFlag(),mCloseFlag()
	{
	}

	virtual ~CFlagPackFile(){OnClose();};

	virtual bool SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset);
protected:
	virtual void OnClose();

private:
	bool	SetFlag(const string& vrFlag);

	long	mFlagBegin;
	bool	mSet;
	string	mOpenFlag;
	string	mCloseFlag;
};



class CFlagMemoryPackFile:public CMemoryPackFile//带有标志的pack文件
{
public:
	CFlagMemoryPackFile()
		:CMemoryPackFile(),mFlagBegin(0),mSet(false),mOpenFlag(),mCloseFlag()
	{
	}

	virtual ~CFlagMemoryPackFile(){OnClose();};

	virtual bool SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset);
protected:
	virtual void OnClose();

private:
	bool	SetFlag(const string& vrFlag);

	long	mFlagBegin;
	bool	mSet;
	string	mOpenFlag;
	string	mCloseFlag;
};

//当前CFlagMemoryPackFile的位置偏移vFlagOffset写标志，要特别小心使用//会回到当前位置
inline bool CFlagMemoryPackFile::SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset)
{
	mSet=true;
	mOpenFlag=vOpenFlag;
	mCloseFlag=vCloseFlag;
	mFlagBegin=Tell()+vFlagOffset;

	return SetFlag(mOpenFlag);//设置打开标记
}


inline bool CFlagMemoryPackFile::SetFlag(const string& vrFlag)
{
	if(!mSet)
	{//没有设过标志
		return false;
	}

	//保存现场
	long Now=Tell();

	bool Set=true;
	//设置标志
	Seek(mFlagBegin,SEEK_SET);//到写标志的位置
	//将标志写入
	if( PutBuffer(vrFlag.c_str(),(int ) (vrFlag.size()+1) )!=vrFlag.size()+1)
	{//写入出错
		Set=false;
	}


	//恢复现场
	Seek(Now,SEEK_SET);
	return Set;
}


inline void CFlagMemoryPackFile::OnClose()
{
	SetFlag(mCloseFlag);
	CMemoryPackFile::OnClose();
	mSet=false;
}







//当前CFlagPackFile的位置偏移vFlagOffset写标志，要特别小心使用//会回到当前位置
inline bool CFlagPackFile::SetFlag(const char vOpenFlag[],const char vCloseFlag[],const long vFlagOffset)
{
	mSet=true;
	mOpenFlag=vOpenFlag;
	mCloseFlag=vCloseFlag;
	mFlagBegin=mFile.Tell()+vFlagOffset;

	return SetFlag(mOpenFlag);//设置打开标记
}


inline bool CFlagPackFile::SetFlag(const string& vrFlag)
{
	if(!mSet)
	{//没有设过标志
		return false;
	}

	//保存现场
	long Now=mFile.Tell();
	string FileMode=mFile.OpenMode();
	string FileName=mFile.Name();

	mFile.Close();//原来的先关闭


	//可写打开
	bool Set=true;
	if(!mFile.Open(FileName,"rb+"))
	{
		//可能是只读的原因
		mFile.Close();
		CPackFile::ChangeMode(FileName.c_str(),RtFileManager::FP_WRITE);//改变文件属性
		if(!mFile.Open(FileName,"rb+"))
		{//无法打开
			Set=false;
		}
	}


	//设置标志
	if(Set)
	{
		mFile.Seek(mFlagBegin,SEEK_SET);//到写标志的位置
		//将标志写入
		if( mFile.PutBuffer(vrFlag.c_str(),(int ) (vrFlag.size()+1) )!=vrFlag.size()+1)
		{//写入出错
			mFile.Close();
			Set=false;
		}
	}

	mFile.Close();//关闭


	//恢复现场
	
	if(FileMode=="wb+")
	{
		FileMode="rb+";
	}

	mFile.Open(FileName,FileMode);
	mFile.Seek(Now,SEEK_SET);
	return Set;
}


inline void CFlagPackFile::OnClose()
{
	SetFlag(mCloseFlag);
	mFile.Close();
	mSet=false;
}
	



///////////////////////////////////////////////////////////////////////////////////////////////////
inline CPackFile::CPackFile()
:mReferenceCount(0)
{
}

inline CPackFile::~CPackFile()
{
}

inline bool CPackFile:: Create(const string& vrFileName)
{
	if(OnCreate(vrFileName))
	{
		mReferenceCount=1;
		return true;
	}
	return false;
}

inline bool CPackFile::Open(const string& vrFileName,const string vMode)
{
	if(OnOpen(vrFileName,vMode))
	{
		mReferenceCount=1;
		return true;
	}
	return false;
}

inline void CPackFile::Close()
{
	OnClose();
	mReferenceCount=0;
}


template<class T>
inline size_t CPackFile::SizeInPackFile(const T& vrT)
{
	return sizeof(T);
}

template<class T>
inline CPackFile& CPackFile::operator <<   (const T& vrT)
{
	PutBuffer(&vrT,sizeof(T));
	return *this;
}


template<class T>
inline CPackFile& CPackFile::operator>>   (T& vrT)
{
	GetBuffer(sizeof(T),&vrT);
	return *this;
}


inline size_t CPackFile::SizeInPackFile(const string& vrString)
{
	size_t Size=SizeInPackFile(vrString.size());
	Size+=vrString.size();
	return Size;
}

inline CPackFile& CPackFile::operator<< (const string& vrString)
{
	(*this)<<vrString.size();
	PutString(vrString.c_str(),vrString.size());
	return *this;
}

inline CPackFile& CPackFile::operator>> (string& vrString)
{
	size_t  StringLength=0;
	(*this) >> StringLength;
	vrString=GetString((int)StringLength);
	return *this;
}

inline bool CPackFile::PutBuffer( CMemoryBuffer& vrBuffer)//将内存写入
{
	char* pData;
	int ReadSize;
	ReadSize=vrBuffer.Read(pData);
	for(;ReadSize!=0;)
	{
		if(PutBuffer(pData,ReadSize)!=ReadSize)
		{
			return false;
		}
		ReadSize=vrBuffer.Read(pData);
	}
	return true;
}

inline const long CPackFile:: WriteTime()
{
	return WriteTime(Name());
}

inline void CPackFile::AddReference()
{
	mReferenceCount++;
}

inline int CPackFile::DeleteReference()
{
	mReferenceCount--;
	if(mReferenceCount<=0)
	{
		Close();
	}
	return mReferenceCount;
}


//---------------------------------------------------------------------------------------

inline CAnsiPackFile::CAnsiPackFile()
:CPackFile(),mFile()
{

}

inline CAnsiPackFile::~CAnsiPackFile()
{
	OnClose();
}

inline bool CAnsiPackFile::OnCreate(const string& vrFileName)
{
	return mFile.Create(vrFileName);
}

inline bool CAnsiPackFile::OnOpen(const string& vrFileName,const string vMode)
{
	return mFile.Open(vrFileName,vMode);
}

inline void CAnsiPackFile::OnClose()
{
	return mFile.Close();
}

inline const string& CAnsiPackFile::Name()
{
	return mFile.Name();
}


inline const long  CAnsiPackFile::Size()
{
	return mFile.Size();
}

inline const long  CAnsiPackFile::Tell()
{
	return mFile.Tell();
}

inline int	CAnsiPackFile::Seek(long vOffset,int vOrigin)
{
	return mFile.Seek(vOffset,vOrigin);
}

inline void	CAnsiPackFile::Flush()
{
	return mFile.Flush();
}

inline const char*	CAnsiPackFile::GetString(int vCharCount)
{

	return mFile.GetString(vCharCount);

}

inline bool	CAnsiPackFile::PutString(const char* vpString,const size_t vStringLength)
{
	return mFile.PutString(vpString,vStringLength);
}

inline size_t	CAnsiPackFile::GetBuffer(const int vByteCount,void* vpBuffer)
{

	return mFile.GetBuffer(vByteCount,vpBuffer);

}

inline 	size_t	CAnsiPackFile::PutBuffer(const void* vpBuffer,const int vBufferSize)
{
	return mFile.PutBuffer(vpBuffer,vBufferSize);
}


//----------------------------------------------------------------------------------------

inline CMemoryPackFile::CMemoryPackFile()
:CPackFile(),mBuffer(),mName(),mSize(0),mCurrent(0),mTextMode(false)
{
}

inline CMemoryPackFile::~CMemoryPackFile()
{
	OnClose();
}

inline bool CMemoryPackFile::OnCreate(const string& vrFileName)
{
	//mBuffer.Release();
	mSize=0;
	mName=vrFileName;
	mCurrent=0;
	mTextMode=false;
	return true;

	//return false;
}

inline bool CMemoryPackFile::OnOpen(const string& vrFileName,const string vMode)
{
	CAnsiFile Temp;
	if( Temp.Open(vrFileName,vMode))
	{
		mSize=Temp.Size();
		if(mBuffer.Resize(mSize))
		{
			Temp.GetBuffer(Temp.Size(),mBuffer.Ptr());
			mName=vrFileName;
			mCurrent=0;

			if(vMode.find("t")!=-1)
			{
				mTextMode=true;
			}
			else
			{
				mTextMode=false;
			}

			return true;
		}
		else
		{
			mCurrent=0;
			mSize=0;
			mName="";
			mTextMode=false;
			return false;
		}
	}
	else
	{
		return false;
	}
}

inline void CMemoryPackFile:: OnClose()
{
	mBuffer.Release();
	mSize=0;
	mName="";
	mCurrent=0;
	mTextMode=false;
	return ;
}

inline const string& CMemoryPackFile:: Name()
{
	return mName;
}

inline const long CMemoryPackFile:: Size()
{
	return mSize;
}

inline const long CMemoryPackFile::Tell()
{
	return mCurrent;
}

inline int CMemoryPackFile::Seek(long vOffset,int vOrigin)
{
	if(vOrigin==SEEK_SET)
	{
		if(!CheckMemory(vOffset))
		{
			return -1;
		};
		mCurrent=vOffset;
	}
	else if(vOrigin==SEEK_CUR)
	{
		if(!CheckMemory(mCurrent+vOffset))
		{
			return -1;
		};
		mCurrent+=vOffset;
	}
	else if(vOrigin==SEEK_END)
	{
		if(!CheckMemory(vOffset+mSize))
		{
			return -1;
		};
		mCurrent=vOffset+mSize;
	}
	else
	{
		return -1;
	}
	
	if(mCurrent<0)
	{
		mCurrent=0;
		//Err<<("MemoryFileReader:Seek Failed %i/%i: Tell()=%i \n",InPos,mDataSize,Tell());
	}

	if(mCurrent>mSize)
	{
		mSize=mCurrent;
	}

	
	return 0;
}

inline void CMemoryPackFile::Flush()
{
	return;
}

inline const char*	CMemoryPackFile::GetString(int vCharCount)
{
	static CLinearityBuffer Temp;
	Temp.Resize(vCharCount+1);


	const size_t Count=GetBuffer(vCharCount,Temp.Ptr());
	*(Temp.Ptr()+Count)='\0';//结尾零
	return Temp.Ptr();
}

inline bool	CMemoryPackFile::PutString(const char* vpString,const size_t vStringLength)
{
	if(vStringLength==0)
	{
		PutBuffer(vpString,(int)strlen(vpString));
	}
	else
	{
		PutBuffer(vpString,(int)vStringLength);
	}
	return true;
	//return false;
}

inline size_t	CMemoryPackFile::GetBuffer(const int vByteCount,void* vpBuffer)
{
	PROFILE("CMemoryPackFile_GetBuffer");
	if(vByteCount<=0)
	{
		return 0;
	}

	int Begin=(mCurrent >=mSize) ? mSize:mCurrent;
	int End=mCurrent+vByteCount >= mSize ? mSize: mCurrent+vByteCount;

	char* pBuffer=mBuffer.Ptr();

	if(mTextMode)
	{
		char* p=(char*)vpBuffer;
		memcpy(vpBuffer,mBuffer.Ptr()+Begin,End-Begin);
		for(int i=Begin;i!=End;++i)
		{
			if(pBuffer[i]==EOF)
			{
				break;
			}
			if(pBuffer[i]!=0x0d)
			{
				*(p++)=pBuffer[i];
			}
		}

		mCurrent=End;
		return p-(char* )vpBuffer;
	}
	else
	{
		long Result=End-Begin;

		memcpy(vpBuffer,mBuffer.Ptr()+Begin,Result);
		mCurrent=End;
		return Result;
	}
}

inline size_t	CMemoryPackFile::PutBuffer(const void* vpBuffer,const int vBufferSize)
{
	if(vBufferSize<=0)
	{
		return 0;
	}

	if(!CheckMemory(vBufferSize+mCurrent))
	{//内存不满足要求
		return 0;
	}


	//写入内容
	memcpy(mBuffer.Ptr()+mCurrent,vpBuffer,vBufferSize);
	mCurrent+=vBufferSize;
	if(mCurrent>mSize)
	{
		mSize=mCurrent;
	}
	return vBufferSize;
}


inline bool CMemoryPackFile::CheckMemory(int vSize)//如果写内存文件就必须要检查当前内存是否够用
{
	//保证线性内存大小够
	if(mBuffer.Size() < vSize)
	{//不够用
		if(!mBuffer.ExpandSize(vSize+50*1024*1024))//增加50M内存
		{//分配失败
			return false;
		}
	}

	return true;
}











/*
class CZipMemoryPackFile:public CMemoryPackFile
{
public:
CZipMemoryPackFile();
virtual ~CZipMemoryPackFile();
private:
virtual bool OnOpen(const string& vrFileName,const string vMode="rb+")
{
CLinearityBuffer Temp;
Temp.Resize(xxx);
Temp<< 压缩<<mBuffer;
mSize=Temp.Size();
}
};
*/




/*
bool CFlagPackFile::OnCreate(const string& vrFileName)
{
if(!mFile.Create(vrFileName))
{
return false;
}

//将标志写入
if( mFile.PutBuffer(mOpenFlag.c_str(),(int ) (mOpenFlag.size()+1) )!=mOpenFlag.size()+1)
{
mFile.Close();
return false;
}

return true;
}
*/
/*
bool CFlagPackFile::OnOpen(const string& vrFileName,const string vMode)
{
if(!mFile.Open(vrFileName,"rb+"))
{
//可能是只读的原因
mFile.Close();
CPackFile::ChangeMode(vrFileName.c_str(),rt_core::RTFP_WRITE);//改变文件属性
if(!mFile.Open(vrFileName,"rb+"))
{
return false;
}
}

if( mFile.PutBuffer(mOpenFlag.c_str(),(int) (mOpenFlag.size()+1) )!=mOpenFlag.size()+1)
{
mFile.Close();
return false;
}

mFile.Close();

return mFile.Open(vrFileName,vMode);
}*/
#endif//PACK_FILE_H


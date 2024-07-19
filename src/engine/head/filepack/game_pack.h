#ifndef GAME_PACK_H
#define GAME_PACK_H

#include <vector>
#include <string>

#include "pack_file.h"
#include "memory_buffer.h"
//#include "game_string_assistant.h"
//#include "compress_mode.h"
#include "pack_factory.h"



using std::string;

class CUnpackFile;
class CCompress;
class CPack;


//包的接口
class CPack
{
	friend class CPackManager;
	
	
	template<class TBaseProduct,class TType>
	friend class CFactory;

	//friend class CCompress;
	friend class CNULLCompress;
	
	template<class T>
	friend class CDeriveCompress;

public:
	enum ECompressMode
	{
		UNKNOWN_COMPRESS=0,
		NULL_COMPRESS=1,
		LZW_COMPRESS=2,
		ZLIB_COMPRESS=3,
	};



	struct SFileInfo
	{
	public:
		SFileInfo():mName(),mOffset(0),mSizeInPack(0),mOriginSize(0),
						mCompressMode(NULL_COMPRESS),mCompressVersion(0),mWriteTime(-1)
		{
			CM_MEMPROTECTOR(mMD5, 16)
		}
		SFileInfo(const SFileInfo &object)
		{
			CM_MEMPROTECTOR(mMD5, 16)
			mName = object.mName;
			mOffset = object.mOffset;
			mSizeInPack = object.mSizeInPack;
			mOriginSize = object.mOriginSize;
			mCompressMode = object.mCompressMode;
			mCompressVersion = object.mCompressVersion;
			mWriteTime = object.mWriteTime;
			rt2_strncpy(mMD5, object.mMD5, 16);
		}
		~SFileInfo(){CM_MEMUNPROTECTOR(mMD5)}

		string			mName;				//文件名
		int				mOffset;			//文件的偏移量
		int				mSizeInPack;		//文件在包中的大小
		int				mOriginSize;		//文件原来的大小
		ECompressMode	mCompressMode;		//压缩方式
		int				mCompressVersion;	//压缩方式的版本
		long			mWriteTime;			//文件的修改时间
		//char			mMD5[16];			//压缩前的，原始内容的，MD5值
		CM_MEMDEF(mMD5, 16)
		

	/*	friend CDebugInterface& operator << (CDebugInterface& vrDebug,SFileInfo& vrFileInfo)
		{
			vrDebug<<"["<<vrFileInfo.mName<<"]" << "[Offset:" << vrFileInfo.mOffset<<"]"
				<< "[SizeInPack:"<< vrFileInfo.mSizeInPack <<"] [OriginSize:"<< vrFileInfo.mOriginSize << "]"
				<< "[CompressMode:" << vrFileInfo.mCompressMode << "] [CompressVersion:"<< vrFileInfo.mCompressVersion << "] "
				<< "[WriteTime:" << fox::TimeConvert(vrFileInfo.mWriteTime) << "]";
			
			char Temp[16*2+1];
			rtMD5PrintString(Temp,(unsigned char*)vrFileInfo.mMD5);
			vrDebug<<"[MD5:]["<<Temp<<"]";
			return vrDebug;
		}*/


		size_t SizeInPackFile(CPackFile& vrPackFile)
		{
			size_t Size;
			Size=vrPackFile.SizeInPackFile(this->mName);
			Size+=vrPackFile.SizeInPackFile(this->mOffset);
			Size+=vrPackFile.SizeInPackFile(this->mSizeInPack);
			Size+=vrPackFile.SizeInPackFile(this->mOriginSize);
			Size+=vrPackFile.SizeInPackFile(this->mCompressMode);
			Size+=vrPackFile.SizeInPackFile(this->mCompressVersion);
			Size+=vrPackFile.SizeInPackFile(this->mWriteTime);
			Size+=16;//MD5_LENTH
			return Size;
		}

		friend CPackFile& operator<<( CPackFile& vrPackFile, SFileInfo& vrFileInfo)
		{
			vrPackFile<<vrFileInfo.mName;
			vrPackFile<<vrFileInfo.mOffset;
			vrPackFile<<vrFileInfo.mSizeInPack;
			vrPackFile<<vrFileInfo.mOriginSize;
			vrPackFile<<vrFileInfo.mCompressMode;
			vrPackFile<<vrFileInfo.mCompressVersion;
			vrPackFile<<vrFileInfo.mWriteTime;
			vrPackFile.PutBuffer(vrFileInfo.mMD5,16);
			return vrPackFile;
		} 

		friend CPackFile& operator>>( CPackFile& vrPackFile, SFileInfo& vrFileInfo)
		{
			vrPackFile>>vrFileInfo.mName;
			vrPackFile>>vrFileInfo.mOffset;
			vrPackFile>>vrFileInfo.mSizeInPack;
			vrPackFile>>vrFileInfo.mOriginSize;
			vrPackFile>>vrFileInfo.mCompressMode;
			vrPackFile>>vrFileInfo.mCompressVersion;
			vrPackFile>>vrFileInfo.mWriteTime;
			vrPackFile.GetBuffer(16,vrFileInfo.mMD5);
			return vrPackFile;
		} 
	};


public:
	virtual std::vector<SFileInfo*> GetAllFileInfo()=0;									//得到包中所有文件信息
	virtual const SFileInfo* GetFileInfo(const string& vrNameInPack)=0;					//得到文件信息

	virtual bool LoadFile(const string& vrNameInPack,			void* vpBuffer)=0;		//载入文件到内存
	virtual bool LoadFile(const CPack::SFileInfo* vpFileInPack,	void* vpBuffer)=0;		//载入文件到内存
	virtual bool AsyncLoadFile(const CPack::SFileInfo* vpFileInPack)	=0;				//异步载入一个文件
	
	
	virtual CUnpackFile* GetUnpackFile(const string& vrNameInPack)=0;					//得到一个不打包文件//要删除
	virtual CUnpackFile* GetUnpackFile(CPack::SFileInfo* vpFileInPack)=0;				//得到一个不打包文件//要删除

	virtual bool DelFile(const string& vrNameInPack)=0;									//删除包中文件
	
	//添加包中文件从一个包文件
	virtual bool AddFile(const string& vrNameInPack,	CUnpackFile& vrUnpackFile,
							CCompress* vpCompress,bool vReplace)=0;		
	
	//添加包中文件从文件
	virtual bool AddFile(const string& vrFileName,		const string& vrNameInPack,
							CCompress* vpCompress,bool vReplace)=0;
	
	//添加包中文件从内存缓存中//如果时间是-1则用当前时间
	virtual bool AddFile(const string& vrNameInPack,	CMemoryBuffer& vrBuffer,	
							CCompress* vpCompress,long vTime,bool vReplace)=0;
	
	//添加包中文件从整块内存中//如果时间是-1则用当前时间
	virtual bool AddFile(const string& vrNameInPack,	void* vpBuffer,				int vBufferSize,
							CCompress* vpCompress,long vTime,bool vReplace)=0;
	

	virtual void UpdateToCipherVersion()=0;				//ac.ma

	virtual bool Create()=0;							//创建一个包
	virtual bool Open()=0;								//打开一个已经存在的包
	virtual	bool Save()=0;								//将包中所有的信息存盘
	virtual bool Flush()=0;								//将包的信息写入


	virtual const string& Name();									//得到包的名称
	virtual void GetType(string& vrTypeName,int& vrVersion)=0;		//得到包的类型和版本

	virtual void GetContentVersion(string& vrMainVersion,string& vrSubVersion)=0;//读出包内容的主次版本信息
	virtual void SetContentVersion(string& vrMainVersion,string& vrSubVersion)=0;//设置包内容的主次版本信息

	virtual void GetTime(long& vrStartTime,long& vrEndTime)=0;		//得到包的起始和结束时间
	virtual void SetTime(long vStartTime,long vEndTime)=0;			//设置包的起始和结束时间

	virtual bool Repair()=0;//修复包//true表示有进行修复，false表示没有进行修复
	virtual bool CheckOverlap() { return true; }; // Check file and freeblock is overlaped or not.
	virtual void RemoveEmptyFile() {};

	CPackFile*	mpPackFile;

	CFactory<CCompress,CPack::ECompressMode>*		mpCompressFactory;			//压缩方式的工厂
protected:
	CPack():mpPackFile(NULL),mErr(){}
	virtual ~CPack();

	long PackSize()
	{
		if(mpPackFile==NULL)
		{
			return 0;
		}
		else
		{
			return mpPackFile->Size();
		}
	};
	bool Init(CPackFile* vpPackFile,CFactory<CCompress,CPack::ECompressMode>* vpCompressFactory);											//包的初始化

	virtual bool IsType(const string& vrTypeName,int vVersion)=0;				//是不是这个类型和版本
	virtual CPack* Clone()=0;													//克隆

	virtual void OnInit()=0;													//文件包初始化

	
	virtual bool BeginWrite(int vRequestSize,SFileInfo* vpFileInfo)=0;			//开始写
	virtual int  Write(const void* vpData,const int vDataSize)=0;				//写
	virtual void EndWrite()=0;													//结束写

	virtual bool OnceWrite(CMemoryBuffer& vrBuffer,SFileInfo* vpFileInfo)=0;	//一次写
	virtual bool OnceWrite(const void* vpData,const int vDataSize,
							SFileInfo* vpFileInfo)=0;							//一次写

	virtual bool OnceRead(void* vpData,const SFileInfo* vpFileInfo)=0;			//一次读出

	CPackErrStd	mErr;
	
	
};









/*
virtual bool AsyncLoadFile(const string vFileInPack)					//异步载入一个文件
{
SThreadParameter* p=RT_NEW  SThreadParameter;
p->mpPack=this;
p->mFileInPack=vFileInPack;

CREATE_THREAD(ThreadFunc,p);
}
*/

//转接口
//const SFileInfo* GetFileInfo(const char* vrNameInPack);					
//bool DelFile(const char* vpNameInPack);
//bool AddFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress);
//bool AddFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,int vRTReadSize);
//bool ReplaceFile(const char* vpNameInPack);
//bool ReplaceFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress);
//bool ReplaceFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,CCompress* vpCompress);

//置换包中文件从文件
//virtual bool ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
//						CCompress* vpCompress)=0;	

//置换包中文件从内存块//如果时间是-1则用当前时间
//virtual bool ReplaceFile(const string& vpNameInPack,	void* vpBuffer,				int vBufferSize,
//						CCompress* vpCompress,long vTime)=0;


#endif//GAME_PACK_H

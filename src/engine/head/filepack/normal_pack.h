#ifndef NORMAL_PACK_H
#define NORMAL_PACK_H
#include <map>
#include <list>

#include "game_pack.h"



using std::map;
using std::list;

const char	NORMAL_PACK_TYPE_NAME[]="NORMAL_TYPE";
const int	PACK_TYPE_LENGTH=32;
//const int	NORMAL_PACK_VERSION_100=100;
//const int	NORMAL_PACK_VERSION_101=101;
//const int	NORMAL_PACK_VERSION_102=102;
//const int   LAST_NORMAL_PACK_VERSION=103;//旧，不含包的时间
const int   LAST_NORMAL_PACK_VERSION=104;


extern CThreadCriticalSection gRealReadFile;

struct SNormalPackInfo
{
public:
	SNormalPackInfo()
	:mPackType(NORMAL_PACK_TYPE_NAME),
	mVersion(LAST_NORMAL_PACK_VERSION),				//包版本
	mBlockTableOffset(0),							//文件块表的偏移量[相对文件头]
	mBlockTableSize(0),								//文件块表的大小
	mFileCount(0),
	mBlockCount(0),									//文件块的总数
	mStartTime(0),									//文件包的起始时间[开区间]
	mEndTime(0)										//文件包的结束时间[闭区间]
	{
	};

	string	mPackType;						//包类型
	int		mVersion;						//包版本//结构表示版本

	int		mBlockTableOffset;				//文件块表的偏移量[相对文件头]
	int		mBlockTableSize;				//文件块表的大小

	size_t	mFileCount;						//文件总数
	size_t  mBlockCount;					//空余块总数
	
	//----------------------以下信息同文件表一起写入-----------------------------
	string  mMainContentVersion;	//内容的主版本标识
	string  mSubContentVersion;		//内容的次要版本的标识
	long    mStartTime;				//内容的起始时间
	long	mEndTime;				//内容的结束时间

	//</---------------------以下信息同文件表一起写入----------------------------->

	//包信息写入
	friend CPackFile& operator<<( CPackFile& vrPackFile, SNormalPackInfo& vrPackInfo)
	{
		vrPackFile<<vrPackInfo.mPackType;					//包类型
		vrPackFile<<LAST_NORMAL_PACK_VERSION;//vrPackInfo.mVersion;					//包版本//总是写最新的版本
		vrPackFile<<vrPackInfo.mBlockTableOffset;			//文件块表的偏移量[相对文件头]
		vrPackFile<<vrPackInfo.mBlockTableSize;				//文件块表的大小
		vrPackFile<<vrPackInfo.mFileCount;					//文件块表的大小
		vrPackFile<<vrPackInfo.mBlockCount;					//文件块的总数
		return vrPackFile;
	} 

	//包信息读出
	friend CPackFile& operator>> (CPackFile& vrPackFile, SNormalPackInfo& vrPackInfo)
	{
		vrPackFile>>vrPackInfo.mPackType;					//包类型
		vrPackFile>>vrPackInfo.mVersion;					//包版本
		vrPackFile>>vrPackInfo.mBlockTableOffset;			//文件块表的偏移量[相对文件头]
		vrPackFile>>vrPackInfo.mBlockTableSize;				//文件块表的大小
		vrPackFile>>vrPackInfo.mFileCount;					//文件块表的大小
		vrPackFile>>vrPackInfo.mBlockCount;					//文件块的总数
		return vrPackFile;
	}

};

struct SNormalFileInfo:public CPack::SFileInfo
{
	friend CPackFile& operator<<( CPackFile& vrPackFile, SNormalFileInfo& vrFileInfo)
	{
		return operator <<(vrPackFile,(SFileInfo&)vrFileInfo);
	} 

	friend CPackFile& operator>>( CPackFile& vrPackFile, SNormalFileInfo& vrFileInfo)
	{
		return operator >>(vrPackFile,(SFileInfo&)vrFileInfo);
	} 
	/*friend CDebugInterface& operator << (CDebugInterface& vrDebug,SNormalFileInfo& vrFileInfo)
	{
		return operator <<(vrDebug,(SFileInfo&)vrFileInfo);
	}*/
};


struct SChangingBlockInfo
{
public:
	SChangingBlockInfo():mOffset(0),mSize(0){};
	SChangingBlockInfo(int vOffset,int vSize):mOffset(vOffset),mSize(vSize){};

	int						mOffset;													//文件块的偏移量
	int						mSize;														//文件块的大小

	bool operator==(SChangingBlockInfo& vrInfo)
	{
		if(mOffset==vrInfo.mOffset && mSize==vrInfo.mSize)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	size_t SizeInPackFile(CPackFile& vrPackFile)
	{
		size_t Size=0;
		Size+=vrPackFile.SizeInPackFile(this->mOffset);
		Size+=vrPackFile.SizeInPackFile(this->mSize);
		return Size;
	}

	friend CPackFile& operator<<( CPackFile& vrPackFile, SChangingBlockInfo& vrBlockInfo)
	{
		vrPackFile<<vrBlockInfo.mOffset;
		vrPackFile<<vrBlockInfo.mSize;
		return vrPackFile;
	} 

	friend CPackFile& operator >> (CPackFile& vrPackFile,SChangingBlockInfo& vrBlockInfo)
	{
		vrPackFile>>vrBlockInfo.mOffset;
		vrPackFile>>vrBlockInfo.mSize;
		return vrPackFile;
	}

};




class CNormalPack:public CPack
{
	friend class CPackManager;

	template<class TBaseProduct,class TType>
	friend class CFactory;
public:

	struct SBufferData
	{
		string	mName;
		CLinearityBuffer mBuffer;
	};


	//父类的接口
    
	virtual std::vector<SFileInfo*> GetAllFileInfo();									//得到包中所有文件信息
	virtual const CPack::SFileInfo* GetFileInfo(const string& vrNameInPack);			//得到文件信息

	virtual bool LoadFile(const string& vrNameInPack,void* vpBuffer);					//载入文件到内存
	virtual bool LoadFile(const CPack::SFileInfo* vpFileInPack,void* vpBuffer);			//载入文件到内存

	virtual bool AsyncLoadFile(const CPack::SFileInfo* vpFileInPack);					//异步载入一个文件


	virtual CUnpackFile* GetUnpackFile(const string& vrNameInPack);					//得到一个不打包文件//要删除
	virtual CUnpackFile* GetUnpackFile(CPack::SFileInfo* vpFileInPack);				//得到一个不打包文件//要删除

	virtual bool DelFile(const string& vrNameInPack);									//删除包中文件
	virtual bool CheckOverlap(); // Check file and freeblock is overlaped or not.
	
	//添加包中文件从一个包文件
	virtual bool AddFile(const string& vrNameInPack,	CUnpackFile& vrUnpackFile,
		CCompress* vpCompress, bool vReplace);		

	//添加包中文件从文件
	virtual bool AddFile(const string& vrFileName,		const string& vrNameInPack,
		CCompress* vpCompress, bool vReplace);

	//添加包中文件从内存缓存中//如果时间是-1则用当前时间
	virtual bool AddFile(const string& vrNameInPack,	CMemoryBuffer& vrBuffer,
							CCompress* vpCompress, long vTime,bool vReplace);

	//添加包中文件从整块内存中//如果时间是-1则用当前时间
	virtual bool AddFile(const string& vrNameInPack,	void* vpBuffer,				int vBufferSize,
							CCompress* vpCompress, long vTime,bool vReplace);
	
	//virtual void SetDirty();						//文件包的脏标记

	virtual bool Create();			//创建一个包
	virtual bool Open();							//打开一个已经存在的包
	virtual	bool Save();				//将包中所有的信息存盘
	virtual bool Flush();							//将包的信息写入

	virtual void GetType(string& vrTypeName,int& vrVersion);		//得到包的类型和版本

	virtual void GetContentVersion(string& vrMainVersion,string& vrSubVersion);//读出包内容的主次版本信息
	virtual void SetContentVersion(string& vrMainVersion,string& vrSubVersion);//设置包内容的主次版本信息

	virtual void GetTime(long& vrStartTime,long& vrEndTime);		//得到包的起始和结束时间
	virtual void SetTime(long vStartTime,long vEndTime);			//设置包的起始和结束时间

	virtual bool Repair();//修复包//true表示有进行修复，false表示没有进行修复
	virtual void RemoveEmptyFile();

	virtual void UpdateToCipherVersion();			//ac.ma
	inline bool rtMD5DataString(char* szData, DWORD iLen, char* szMD5);	//ac.ma


	RT_TYPE_CS						mCriticalSection ;	//线程安全
	list<SBufferData>			mPool;				//缓冲池

	list<string>				mLoading;			//正在预载入的文件

protected:
	CNormalPack();
	virtual ~CNormalPack();

	virtual bool IsType(const string& vrTypeName,int vVersion);		//是不是这个类型和版本
	virtual CPack* Clone();											//克隆

	virtual void OnInit();											//文件包初始化

	virtual bool BeginWrite(int vRequestSize,SFileInfo* vpFileInfo);//开始写
	virtual int  Write(const void* vpData,const int vDataSize);		//写
	virtual void EndWrite();										//结束写
	
	virtual bool OnceWrite(CMemoryBuffer& vrBuffer,SFileInfo* vpFileInfo);	//一次写
	virtual bool OnceWrite(const void* vpData,const int vDataSize,
		SFileInfo* vpFileInfo);												//一次写

	virtual bool OnceRead(void* vpData,const SFileInfo* vpFileInfo);		//一次读出

private:
	void Dirty();									//设置文件包为脏
	
	const long GetEnd();							//得到包结尾
	void SetEnd(const long vEnd);					//设置包结尾

	SChangingBlockInfo GetBlock(int vBlockSize);	//得到空余块
	void FreeBlock(int vOffset,int vSize,bool vDirty=true);//释放空余块 vDirty指是否让文件表为dirty
	void FreeReserveBlock(int vOffset,int vSize);	//释放保留的空余块

	bool ReadFileTable();							//读文件表
	void WriteFileTable();							//写文件表

	bool								mDirty;		//脏标记
	long								mBegin;		//包的起始位置
	long								mEnd;		//包的结束位置
	bool								mAllowWrite;//允许写数据

	SNormalPackInfo						mInfo;		//当前的包信息
	map<string,SNormalFileInfo*>		mFileTable;	//当前的文件表
	list<SChangingBlockInfo>			mFreeBlock;	//当前包空的文件块
	list<SChangingBlockInfo>			mReserveFreeBlock;//保留的空文件块[不能被使用]

};



//virtual size_t GetFileCount();                           //得到包文件总数
//virtual bool GetAllFileInfo(SFileInfo* vpFileInfo[]);                             //得到包中所有文件信息

/*
//置换包中文件从文件
virtual bool ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
CCompress* vpCompress);	

//置换包中文件从内存块//如果时间是-1则用当前时间
virtual bool ReplaceFile(const string& vrNameInPack, void* vpBuffer,				int vBufferSize,
CCompress* vpCompress,long vTime);
*/




#endif//NORMAL_PACK_H


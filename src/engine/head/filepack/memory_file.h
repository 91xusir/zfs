#ifndef MEMORY_FILE_H 
#define MEMORY_FILE_H 

#include "game_pack.h"
#include "pack_manager.h"
#include "core/rt2_core.h"



const int DEFAULT_BUFFER_SIZE=1024*1024;		//1M

class CUnpackFileReader:public RtArchive
{
public:
	CUnpackFileReader(/*RtOutputDevice* vpError,*/CUnpackFile* vpUnpackFile,bool vTextMode)
		:mTextMode(vTextMode),
		mpcDiskBuffer( NULL ),
		mpUnpackFile(vpUnpackFile)/*,mpError(vpError)*/
	{
		m_nMode |=RtArchive::load;
	};

	virtual ~CUnpackFileReader()
	{
		DEL_ONE(mpUnpackFile);
		DEL_ARRAY( mpcDiskBuffer );
	}

	virtual void Seek( int InPos, SeekMode eSeek )
	{
		if(eSeek==SeekBegin)
		{
			mpUnpackFile->Seek(InPos,SEEK_SET);
		}
		else if(eSeek==SeekCurrent)
		{
			mpUnpackFile->Seek(InPos,SEEK_CUR);
		}
		else if(eSeek==SeekEnd)
		{
			mpUnpackFile->Seek(InPos,SEEK_END);
		}

		return;
	};

	virtual int Tell()
	{
		return (int)mpUnpackFile->Tell();
	};

	virtual int TotalSize()
	{
		return (int)mpUnpackFile->OriginSize();
	};

	virtual BOOL Close()
	{
		DEL_ONE(mpUnpackFile);
		mpUnpackFile=NULL;
		return !m_bIsError;
	};

	virtual void Flush()
	{
		return;
	};

	virtual long Serialize(void* V,int iLength)
	{
		if(iLength<=0)
		{
			return 0;
		}

		if(mTextMode)
		{
			char* p=(char*) V;
			char* pBuffer=(char*)V;

			int RealRead=(int)mpUnpackFile->GetBuffer(iLength,V);
			
			for(int i=0;i!=RealRead;++i)
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

			//LOG2("---[RT MODE][%s]Read[%d]\n",mpUnpackFile->Name().c_str(),p-(char*)V);

			return p-(char*)V;
		}
		else
		{
			if( mpcDiskBuffer )
			{
				if( ( mnCurrentPosition < mpUnpackFile->OriginSize() ) && ( (mnCurrentPosition + iLength) <= mpUnpackFile->OriginSize() )&& ( V != NULL ) )
				{
					unsigned char*  pkCurrentPos = mpcDiskBuffer + mnCurrentPosition;
					unsigned char*  pkDestBuffer = (unsigned char*)V;
					memcpy( pkDestBuffer , pkCurrentPos , iLength );
					mnCurrentPosition += iLength;
					return iLength;
				}
				return 0;
			}
			size_t Read=mpUnpackFile->GetBuffer(iLength,V);
			//LOG2("---[%s]Read[%d]\n",mpUnpackFile->Name().c_str(),Read);
			return (int)Read;
		}
	};
	void CacheDiskStore()
	{
		DEL_ARRAY( mpcDiskBuffer );
		mnCurrentPosition = 0;
        if( mpUnpackFile                     && 
			mpUnpackFile->OriginSize() > 0   &&
			mpUnpackFile->GetInfo().mCompressMode == CPack::NULL_COMPRESS )
		{
            mpcDiskBuffer = RT_NEW unsigned char[ mpUnpackFile->OriginSize() ];
			mpUnpackFile->GetBuffer( mpUnpackFile->OriginSize() , mpcDiskBuffer );
		}
	}

	
private:
	bool					mTextMode;
	CUnpackFile*			mpUnpackFile;

	unsigned long           mnCurrentPosition;
	unsigned char*          mpcDiskBuffer;

	/*RtOutputDevice*			mpError;*/
};


class MemoryPackFileWriter : public RtArchive
{
public:
	MemoryPackFileWriter( CPack* vpPack, CCompress* vpCompress,const string& vrFileName /*,RtOutputDevice* vpError*/)
		:mName(vrFileName),
		mpPack(vpPack),
		mpCompress(vpCompress),
		/*mpError(vpError),*/
		mCurrent(0),
		mpBuffer(NULL),
		mBufferSize(0),
		mDataSize(0)
	{
		m_nMode |= RtArchive::store;
	}

	virtual ~MemoryPackFileWriter()
	{
		Close();
	}

	virtual void Seek( int InPos, SeekMode eSeek )
	{

		if(eSeek==SeekBegin)
		{
			mCurrent=InPos;
		}
		else if(eSeek==SeekCurrent)
		{
			mCurrent+=InPos;
		}
		else if(eSeek==SeekEnd)
		{
			mCurrent=InPos+mDataSize;
		}
		else
		{
			return;
		}



		if(mCurrent<0)
		{
			mCurrent=0;
			m_bIsError=1;
			//mpError->Logf("MemoryFileReader:Seek Failed %i/%i: Tell()=%i \n",InPos,mDataSize,Tell());
		}
		return;
	}

	virtual int  Tell()
	{
		return mCurrent;
	}

	virtual BOOL Close()
	{

		if(!mpPack->AddFile(mName,mpBuffer,mDataSize,mpCompress,-1,true))
		{
			m_bIsError=1;
			//mpError->Logf("MemoryPackFileWriter: close failed, pack add file failed!");
		}

		DEL_ONE(mpBuffer);
		mpBuffer=NULL;

		return !m_bIsError;
	}

	virtual void Flush()
	{
		//mpPack->AddFile(mName,mpBuffer,mDataSize);
		mpPack->Flush();
		return;
	}

	virtual long Serialize( void* V, int iLength )
	{
		if (iLength<=0)
		{
			return 0;
		}


		int End=mCurrent+iLength;

		if(End>mBufferSize)
		{
			if(!ReSizeBuffer(End))
			{
				m_bIsError=1;
				//mpError->Logf("MemoryPackFileWriter: ReSizeBuffer[%i] failed!",End);
			}
		}


		if(mCurrent-mDataSize>0)
		{//当前指针已经再数据的后面，中间填充0
			memset(&(mpBuffer[mDataSize]),0,mCurrent-mDataSize);
		}

		//将数据区扩大
		if(End>mDataSize)
		{
			mDataSize=End;
		}

		//数据写入
		memcpy(&mpBuffer[mCurrent],V,iLength);
		//g_pFileManager->AddReadByte(Result);
		return iLength;
	}

protected:

	bool ReSizeBuffer(const int vSize)
	{
		const int MAX_BUFFER_SIZE= 64262;
		if(vSize<=mBufferSize)
		{
			return true;
		}
		if(vSize>MAX_BUFFER_SIZE)
		{
			return false;
		}

		int OldSize=mBufferSize;//旧的大小
		char* pOldBuffer=mpBuffer;//旧的指针

		if(mBufferSize==0)
		{
			mBufferSize=DEFAULT_BUFFER_SIZE;
		}

		for(;vSize>mBufferSize;)
		{
			mBufferSize=mBufferSize*2;
		}

		mpBuffer=RT_NEW char[mBufferSize];
		if(mpBuffer==NULL)
		{
			mBufferSize=OldSize;
			mpBuffer=pOldBuffer;
			return false;
		}
		else
		{
			memcpy(mpBuffer,pOldBuffer,OldSize);
			DEL_ONE(pOldBuffer);
		}

		return true;
	};



	string				mName;
	CPack*				mpPack;
	CCompress*			mpCompress;
	/*RtOutputDevice*		mpError;*/
	int					mCurrent;

	char*				mpBuffer;
	int					mBufferSize;
	int					mDataSize;
};



class CRtPackManager: public RtFileManager
{
public:
	CRtPackManager():mpPackManager(NULL){};
	~CRtPackManager()
	{
		DEL_ONE(mpPackManager);
		mpPackManager=NULL;
	};

	virtual void PreLoadFileRead(const char* vpFileName/*, RtOutputDevice* vpError*/) 
	{
		/*
		LOG("PreLoadFileRead:");
		LOG(vpFileName);
		LOG("\n");
*/
		
		string FileName=TransformFileName(vpFileName);


		vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

		const CPack::SFileInfo* pFileInfo=NULL;
		vector<CPack*>::iterator It;
		for(It=AllPacks.begin();It!=AllPacks.end();++It)
		{
			/*
			LOG("Get FileInfo:");
			LOG(FileName.c_str());
			LOG("\n");
*/
			pFileInfo=(*It)->GetFileInfo(FileName.c_str());
			if(pFileInfo!=NULL)
			{
				//LOG("AsyncLoad.\n");
				(*It)->AsyncLoadFile(pFileInfo);
				return;
			}
		}

		if(!AllPacks.empty())
		{
//			game_client::Debug<<"预先载入失败[" << FileName << "]\n";
		}
	}

	bool OpenPack(const char* pPackName,bool vSafeCheck)//一般不需要安全检查//客户端可能有需求
	{
		string PackName=TransformFileName(pPackName);

		if(vSafeCheck)
		{
			return PackManager()->OpenPack(PackName,CPackManager::READ_PACK,"rb");
		}
		else
		{
			return PackManager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb");
		}
	}


	int FileSize( const char* pFilename )
	{
		string FileName=TransformFileName(pFilename);
		const CPack::SFileInfo* pFileInfo= GetFileInfoFromAllPack(FileName.c_str());
		if(pFileInfo==NULL)
		{
			return 0;
		}
		else
		{
			return pFileInfo->mOriginSize;
		}

	}
	BOOL Copy( const char* pPackageName, const char* pDestFile, const char* pSrcFile, BOOL bReplaceExisting, BOOL bEvenIfReadOnly, BOOL bAttributes, void (*fnProgress)(FLOAT fFraction) )
	{
		return FALSE;
	}

	BOOL MakeDirectory( const char* pPath, BOOL bTree=0 )
	{
		return FALSE;
	}

	BOOL DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )
	{
		return FALSE;
	}

	BOOL Move( const char* pPackageName, const char* Dest, const char* Src, BOOL ReplaceExisting=1, BOOL EvenIfReadOnly=0, BOOL Attributes=0 )
	{
		return FALSE;
	}
    BOOL SetFilePermission( const char* pFilename, DWORD dwPermission )
    {
        return FALSE;
    }
    DWORD GetFilePermission( const char* pFilename )
    {
        return 0;
    }
	BOOL FileExist( const char* pFilename )
	{
		string FileName=TransformFileName(pFilename);
		if(GetFileInfoFromAllPack(FileName.c_str())==NULL)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	RtArchive* CreateFileReader(const char* pFilename,DWORD dwFlags/*,RtOutputDevice* pError*/)
	{
		RtString szFind(pFilename);
		RtArchive* p=CreateUnpackFileReader(pFilename,dwFlags/*,pError*/);
		return p;//CreateUnpackFileReader(pFilename,dwFlags,pError);
		
		//return CreateMemoryPackFileReader( pFilename, dwFlags, pError );
		//return CreateMapPackFileReader(pFilename,dwFlags,pError);//MapPackFile
	}

	CUnpackFileReader* CreateUnpackFileReader(const char* vpFilename,DWORD vFlags/*,RtOutputDevice* vpError*/)
	{
		if(stricmp(vpFilename,"stdin")==0)
		{
			return NULL;
		}


		string FileName=TransformFileName(vpFilename);
		vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

		//const CPack::SFileInfo* pFileInfo=NULL;
		vector<CPack*>::iterator It;

		CUnpackFile* pFile=NULL;
		for(It=AllPacks.begin();It!=AllPacks.end();++It)
		{
			pFile=(*It)->GetUnpackFile(FileName);
			if(pFile!=NULL)
			{
				break;
			}
		}
		

		if(pFile==NULL)
		{
			//LOG2("CRtPackManager:Can't find [%s] -->[%s]\n",vpFilename,FileName.c_str());
			return NULL;
		}

		//LOG("GetFileReader:");
		//LOG(FileName.c_str());
		//LOG("\n");

		CUnpackFileReader* p=RT_NEW CUnpackFileReader(/*vpError,*/pFile,false);
		return p;

	}



	MemoryPackFileWriter* CreateFileWriter( const char* pPackName, const char* pFilename, DWORD dwFlags/*, RtOutputDevice* pError*/ )
	{
		if(pPackName==NULL)
		{
			return NULL;
		}


		if (stricmp(pFilename, "stdout")==0)
		{
			return NULL;
		}

		string FileName=TransformFileName(pFilename);
		if( (dwFlags & FW_NoReplaceExisting) && FileSize(FileName.c_str())>=0 )
		{
			return NULL;
		}


		CPack* pPack;
		string PackName=TransformFileName(pPackName);
		pPack=PackManager()->GetPack(PackName);
		if(pPack==NULL)
		{
//			pError->Logf("CRtPackManager: not this pack: %s\n", PackName.c_str());
			return NULL;
		}

		CCompress* pCompress=PackManager()->GetCompress(CPack::NULL_COMPRESS,CNULLCompress::LAST_VERSION);
		if(pCompress==NULL)
		{
//			pError->Logf("CRtPackManager: Can't Get NULL_COMPRESS mode ,  [%s] create failed!\n", FileName.c_str());
			return NULL;
		}



		if(dwFlags&FW_Append)
		{
			CUnpackFile* pFile;
			pFile=pPack->GetUnpackFile(FileName);
			if(pFile==NULL)
			{
//				pError->Logf("CRtPackManager: create file failed: %s\n", FileName.c_str());
				return NULL;
			}

			pPack->DelFile(FileName);
			

			CLinearityBuffer Buffer;
            pFile->GetBuffer(Buffer);
			int Size=(int)pFile->OriginSize();
			DEL_ONE(pFile);
			MemoryPackFileWriter* pWrite=RT_NEW MemoryPackFileWriter(pPack,pCompress,FileName/*,pError*/);
			pWrite->Serialize(Buffer.Ptr(),Size);
			return pWrite;
		}
		else
		{
			string PackName=TransformFileName(pPackName);
			string FileName=TransformFileName(pFilename);

			return RT_NEW MemoryPackFileWriter(pPack,pCompress,FileName/*,pError*/);
		}
	}


	BOOL Delete( const char* pFilename, BOOL bRequireExists=0, BOOL bEvenReadOnly=0 )
	{
		string FileName=TransformFileName(pFilename);


		CPack* pPack=NULL;
		CPack::SFileInfo* pFileInfo=NULL;

		bool Ok=false;
		if(GetFileInfoFromAllPackEx(pPack,(const CPack::SFileInfo*&)pFileInfo,FileName.c_str()))
		{

			if(pPack->DelFile(FileName) )
			{
				Ok=true;
			}
		}

		if(bRequireExists)
		{
			return (BOOL)Ok;
		}
		else
		{
			return TRUE;
		}

	}

    SQWORD GetCreateTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetCreateTime( const char* pFilename )
    {
        return 0;
    }
    SQWORD GetAccessTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetAccessTime( const char* pFilename )
    {
        return 0;
    }
    SQWORD GetModifyTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetModifyTime( const char* pFilename )
    {
        return 0;
    }

	std::list<std::string> FindFiles( const char* pFilename, BOOL bFiles, BOOL bDirectories )
	{
		std::list<std::string> Result;
		return Result;
	}

	BOOL SetDefaultDirectory( const char* pFilename )
	{
		return FALSE;
	}

	RtString GetDefaultDirectory()
	{
		return "";
	}

private:



	string TransformFileName(const char* vpFileName)
	{
		string GetFileName;
		const char* p;
		size_t Size=strlen(vpFileName);

		if(Size>=2)
		{
			if(vpFileName[0]=='.' && (vpFileName[1]=='\\'|| vpFileName[1]=='/'))
			{
				p=vpFileName+2;
			}
			else
			{
				p=vpFileName;
			}
			GetFileName=p;
		}
		else
		{
			GetFileName=vpFileName;
		}

		for(size_t i=0;i!=GetFileName.size();++i)
		{
			if(GetFileName[i]=='/')
			{
				GetFileName[i]='\\';
			}
			else if(GetFileName[i]>='A' && GetFileName[i]<='Z')
			{
				GetFileName[i]+=('a'-'A');
			}
		}

		return GetFileName;
	}


	const CPack::SFileInfo* GetFileInfoFromAllPack(const char* vpFileName)
	{
		vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

		const CPack::SFileInfo* pFileInfo=NULL;
		vector<CPack*>::iterator It;
		for(It=AllPacks.begin();It!=AllPacks.end();++It)
		{
			pFileInfo=(*It)->GetFileInfo(vpFileName);
			if(pFileInfo!=NULL)
			{
				break;
			}
		}

		return pFileInfo;
	}

	bool GetFileInfoFromAllPackEx(CPack*& vprGetPack,const CPack::SFileInfo*& vprGetFileInfo,const char* vpFileName)
	{
		vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

		const CPack::SFileInfo* pFileInfo=NULL;
		vector<CPack*>::iterator It;
		for(It=AllPacks.begin();It!=AllPacks.end();++It)
		{
			pFileInfo=(*It)->GetFileInfo(vpFileName);
			if(pFileInfo!=NULL)
			{
				break;
			}
		}

		if(pFileInfo==NULL)
		{
			return false;
		}
		else
		{
			vprGetPack=*It;
			vprGetFileInfo=pFileInfo;
			return true;
		}
	}



	CPackManager* PackManager()
	{
		if(mpPackManager==NULL)
		{
			mpPackManager=RT_NEW CPackManager;
			mpPackManager->Init();
		}

		return mpPackManager;
	};

	CPackManager*  mpPackManager;
};



class CRtPackAndAnsiManager:public RtFileManager
{
public:
	CRtPackAndAnsiManager(CRtPackManager* vpPackManager,RtFileManager* vpAnsiManager)
		:mpPackManager(vpPackManager),mpAnsiManager(vpAnsiManager)
	{
	}
	~CRtPackAndAnsiManager()
	{
		DEL_ONE(mpPackManager);
		DEL_ONE(mpAnsiManager);
	};


	virtual void PreLoadFileRead(const char* vpFileName/*, RtOutputDevice* vpError=NULL*/) 
	{
		return mpPackManager->PreLoadFileRead(vpFileName/*,vpError*/);
	}



	int FileSize( const char* pFilename )
	{
		if(mpAnsiManager->FileExist(pFilename))
		{
			return mpAnsiManager->FileSize(pFilename);
		}
		else
		{
			if(mpPackManager->FileExist(pFilename))
			{
				return mpPackManager->FileSize(pFilename);
			}
		}

		return 0;
	}

	BOOL Copy( const char* pPackageName, const char* pDestFile, const char* pSrcFile, BOOL bReplaceExisting, BOOL bEvenIfReadOnly, BOOL bAttributes, void (*fnProgress)(FLOAT fFraction) )
	{
		return mpAnsiManager->Copy(pPackageName,pDestFile, pSrcFile, bReplaceExisting, bEvenIfReadOnly, bAttributes, fnProgress);
	}


	BOOL MakeDirectory( const char* pPath, BOOL bTree=0 )
	{
		return mpAnsiManager->MakeDirectory(pPath,bTree);
	}


	BOOL DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )
	{
		return mpAnsiManager->DeleteDirectory(pPath,bRequireExists,bTree);
	}

    BOOL SetFilePermission( const char* pFilename, DWORD dwPermission )
    {
        return mpAnsiManager->SetFilePermission(pFilename,dwPermission);
    }
    DWORD GetFilePermission( const char* pFilename )
    {
        return mpAnsiManager->GetFilePermission(pFilename);
    }

	BOOL Move( const char* pPackageName, const char* Dest, const char* Src, BOOL ReplaceExisting=1, BOOL EvenIfReadOnly=0, BOOL Attributes=0 )
	{
		return mpAnsiManager->Move( pPackageName, Dest, Src, ReplaceExisting, EvenIfReadOnly, Attributes);
	}

	BOOL FileExist( const char* pFilename )
	{
		if(mpAnsiManager->FileExist(pFilename))
		{
			return TRUE;
		}
		else
		{
			return mpPackManager->FileExist(pFilename);
		}
	}
	//tim.yang record time&filename
	//int m_time;
	//const char* m_fileName;
	//SYSTEMTIME m_sys;
	//end
	RtArchive* CreateFileReader( const char* pFilename, DWORD dwFlags/*, RtOutputDevice* pError*/ )
	{	
		//tim.yang record time&filename
		//FILE* fp = fopen("E:\\project\\ccxy\\trunk\\bin\\UseFileFrequency.txt","a");
		//if (fp == NULL) return false;
		//GetLocalTime( &m_sys );
		//m_fileName = pFilename;
		//char szIntroUser[512];
		//sprintf(szIntroUser,"%02d:%02d:%02d:%03d    ,%s\r\n",m_sys.wHour,m_sys.wMinute,m_sys.wSecond,m_sys.wMilliseconds,m_fileName);
		//int size = strlen(szIntroUser);
		//fwrite(szIntroUser, 1, size, fp);
		//fclose(fp);
		//end
		RtArchive* pArchive=NULL;

		if(mpAnsiManager->FileExist(pFilename))
		{
			pArchive=mpAnsiManager->CreateFileReader(pFilename,dwFlags/*,pError*/);
		}
		else
		{
			pArchive=mpPackManager->CreateFileReader(pFilename,dwFlags/*,pError*/);
		}
		
		return pArchive;
	}


	RtArchive* CreateFileWriter( const char* pPackageName, const char* pFilename, DWORD dwFlags/*, RtOutputDevice* pError*/ )
	{
		RtArchive* pArchive;
		pArchive=mpAnsiManager->CreateFileWriter(pPackageName,pFilename,dwFlags/*,pError*/);
		if(pArchive==NULL)
		{
			pArchive=mpPackManager->CreateFileWriter(pPackageName,pFilename,dwFlags/*,pError*/);
		}
		return pArchive;
	}

	BOOL Delete( const char* pFilename, BOOL bRequireExists=0, BOOL bEvenReadOnly=0 )
	{
		if(mpAnsiManager->Delete(pFilename,bRequireExists,bEvenReadOnly))
		{
			return TRUE;
		}
		else
		{
			return mpPackManager->Delete(pFilename,bRequireExists,bEvenReadOnly);
		}
	}


    SQWORD GetCreateTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetCreateTime( const char* pFilename )
    {
        return 0;
    }
    SQWORD GetAccessTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetAccessTime( const char* pFilename )
    {
        return 0;
    }
    SQWORD GetModifyTime( const char* pFilename )
    {
        return 0;
    }
    BOOL SetModifyTime( const char* pFilename )
    {
        return 0;
    }

	std::list<std::string> FindFiles( const char* pFilename, BOOL bFiles, BOOL bDirectories )
	{
		return mpAnsiManager->FindFiles(pFilename,bFiles,bDirectories);
	}


	BOOL SetDefaultDirectory( const char* pFilename )
	{
		return mpAnsiManager->SetDefaultDirectory(pFilename);
	}


	RtString GetDefaultDirectory()
	{
		return mpAnsiManager->GetDefaultDirectory();
	}

public:
	RtFileManager*		mpPackManager;
	RtFileManager*		mpAnsiManager;
};




/*
CMapPackFileReader* CreateMapPackFileReader(const char* pFilename,DWORD dwFlags,RtOutputDevice* pError)
{
if(stricmp(pFilename,"stdin")==0)
{
return NULL;
}
else
{
string FileName=TransformFileName(pFilename);
vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

const CPack::SFileInfo* pFileInfo=NULL;
CPack*					pPack=NULL;
vector<CPack*>::iterator It;

for(It=AllPacks.begin();It!=AllPacks.end();++It)
{
pFileInfo=(*It)->GetFileInfo(FileName);
pPack=(*It);
if(pFileInfo!=NULL)
{
break;
}
}

if(pFileInfo==NULL)
{
//pError->Logf("CRtPackManager: not this file: %s\n", FileName.c_str());
return NULL;
}
else
{
CMapPackFileReader* pFile;
pFile=new CMapPackFileReader(pError,pPack->mpPackFile,(CPack::SFileInfo*)pFileInfo,false);
return pFile;
}
}
}
*/

/*

MemoryPackFileReader* CreateMemoryPackFileReader( const char* pFilename, DWORD dwFlags, RtOutputDevice* pError )
{
if (stricmp(pFilename, "stdin")==0)
{
return NULL;
}
else
{
string FileName=TransformFileName(pFilename);
vector<CPack*> AllPacks=PackManager()->GetAllOpenPack();

const CPack::SFileInfo* pFileInfo=NULL;
vector<CPack*>::iterator It;
for(It=AllPacks.begin();It!=AllPacks.end();++It)
{
pFileInfo=(*It)->GetFileInfo(FileName);
if(pFileInfo!=NULL)
{
break;
}
}



if(pFileInfo==NULL)
{
pError->Logf("CRtPackManager: not this file: %s\n", FileName.c_str());
return NULL;
}
else
{
char* pBuffer =new char[pFileInfo->mSize];
(*It)->LoadFile(pFileInfo,pBuffer);
MemoryPackFileReader* pFile;
pFile=new MemoryPackFileReader(FileName,pError,pBuffer,pFileInfo->mSize,0,false);
return pFile;
}
}
}

*/




/*
MemoryPackFileReader* CreateFileReader( CPack* pPack, const char* pFilename, DWORD dwFlags, RtOutputDevice* pError )
{
if (stricmp(pFilename, "stdin")==0)
{
return NULL;
}
else
{
string FileName=TransformFileName(pFilename);
const CPack::SFileInfo* pFileInfo=pPack->GetFileInfo(FileName);
if(pFileInfo==NULL)
{
//pError->Logf("CRtPackManager: not this file: %s\n", FileName.c_str());
return NULL;
}
else
{
char* pBuffer =new char[pFileInfo->mSize];
pPack->LoadFile(pFileInfo,pBuffer);
MemoryPackFileReader* pFile;
pFile=new MemoryPackFileReader(FileName,pError,pBuffer,pFileInfo->mSize,0,false);
return pFile;
}
}
}
*/







/*
class CMapPackFileReader: public RtArchive
{
public:
CMapPackFileReader(RtOutputDevice* vpError,CPackFile* vpPackFile,CPack::SFileInfo* vpFileInfo,bool vTextMode)
:mTextMode(vTextMode),
mpPackFile(vpPackFile),
mpFileInfo(vpFileInfo),
mpError(vpError),
mCurrent(0)
{
m_nMode |= RtArchive::load;
};

virtual ~CMapPackFileReader()
{
Close();
};

virtual void Seek( int InPos, SeekMode eSeek )
{
if(eSeek==SeekBegin)
{
mCurrent=InPos;
}
else if(eSeek==SeekCurrent)
{
mCurrent+=InPos;
}
else if(eSeek==SeekEnd)
{
mCurrent=InPos+mpFileInfo->mSize;
}
else 
{
return;
}

if(mCurrent<0)
{
mCurrent=0;
m_bIsError=1;
mpError->Logf("CMapFileReader:Seek Failed %i/%i: Tell()=%i \n",InPos,mpFileInfo->mSize,Tell());
}
return;
};

virtual int Tell()
{
return mCurrent;
};

virtual int TotalSize()
{
return mpFileInfo->mSize;
};

virtual BOOL Close()
{
mpPackFile=NULL;
mpFileInfo=NULL;
mCurrent=0;
return !m_bIsError;
};

virtual void Flush()
{
return;
};



virtual long Serialize(void* V,int iLength)
{
//LOG2("Serialize(void* [%d],int [%d])",V,iLength);
if(iLength<=0)
{
return 0;
}

int Size=mpFileInfo->mSize;
//LOG1( "Size[%d]",Size);

int Begin=(mCurrent >= Size) ? Size : mCurrent;
//LOG1("Begin[%d]",Begin);
int End= ( mCurrent+iLength >= Size) ? Size:mCurrent+iLength;
//LOG1("End[%d]",End);

if(mTextMode)
{
char* p=(char*) V;
char* pBuffer=(char*)V;

long RealBegin=Begin+mpFileInfo->mOffset;
mpPackFile->Seek(RealBegin,SEEK_SET);
mpPackFile->GetBuffer(iLength,V);
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

return p-(char*)V;
}
else
{
//LOG("[rb mode]");

long Result=End-Begin;
//LOG1("Result[%d]",Result);

long RealBegin=Begin+mpFileInfo->mOffset;
//LOG1("RealBegin[%d]\n",RealBegin);
if(mpPackFile->Seek(RealBegin,SEEK_SET)!=0)
{
ERR1("mpPackFile->Seek(%d,SEEK_SET) ERR!\n",RealBegin);
}

size_t RealResult=mpPackFile->GetBuffer(Result,V);
//LOG1("RealResult[%d]",RealResult);
if(RealResult!=Result)
{
ERR3("[%d]=mpPackFile->GetBuffer([%d],[%d])",RealResult,Result,V);
}
mCurrent=Begin+RealResult;
//LOG1("mCurrent[%d]",mCurrent);
return RealResult;
}
}
private:
bool					mTextMode;
CPackFile*				mpPackFile;
CPack::SFileInfo*		mpFileInfo;
RtOutputDevice*			mpError;
int						mCurrent;
};

*/
/*
class MemoryPackFileReader : public RtArchive
{
public:
MemoryPackFileReader( const string& vrFileName , RtOutputDevice* vpError,void* vpBuffer,int vSize,int vRTSize,bool vTextMode) 
: mTextMode(vTextMode),
mName(vrFileName),
mpError(vpError),
mpBuffer((char*)vpBuffer),
mSize(vSize),
mCurrent(0),
mRTSize(vRTSize)
{
m_nMode |= RtArchive::load;
}
virtual ~MemoryPackFileReader()
{
Close();
}

virtual void Seek( int InPos, SeekMode eSeek )
{
if(eSeek==SeekBegin)
{
mCurrent=InPos;
}
else if(eSeek==SeekCurrent)
{
mCurrent+=InPos;
}
else if(eSeek==SeekEnd)
{
mCurrent=InPos+mSize;
}
else
{
return;
}

if(mCurrent<0)
{
mCurrent=0;
m_bIsError=1;
mpError->Logf("MemoryFileReader:Seek Failed %i/%i: Tell()=%i \n",InPos,mSize,Tell());
}
return;
}

virtual int  Tell()
{
return mCurrent;
}

virtual int  TotalSize()
{
return mSize;
}

virtual void* GetBuffer()
{
return mpBuffer;
}

virtual BOOL Close()
{
DEL_ONE(mpBuffer);
mpBuffer=NULL;
mCurrent=0;
return !m_bIsError;
}

virtual void Flush()
{
return;
}

virtual long Serialize( void* V, int iLength )
{
if (iLength<=0)
{
return 0;
}



int Begin=(mCurrent>=mSize) ? mSize:mCurrent;
int End=((mCurrent+iLength) >= mSize) ? mSize:mCurrent+iLength;

if(mTextMode)
{
char* p=(char*)V;

memcpy(V,mpBuffer+Begin,End-Begin);
for(int i=Begin;i!=End;++i)
{
if(mpBuffer[i]==EOF)
{
break;
}

if(mpBuffer[i]!=0x0d)
{
*(p++)=mpBuffer[i];
}
}
mCurrent=End;
return p-(char*)V;
}
else
{

long Result=End-Begin;

memcpy(V,mpBuffer+Begin,Result);
//g_pFileManager->AddReadByte(Result);
mCurrent=End;
return Result;
}

}

protected:
bool			mTextMode;
string			mName;
RtOutputDevice* mpError;
char*			mpBuffer;
int             mSize;
int				mCurrent;
int				mRTSize;//文件读写[rt]时的大小
};
*/




#endif//MEMORY_FILE_H 

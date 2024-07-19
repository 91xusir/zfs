

#include "filepack/pack_file.h"
#include "filepack/pack_manager.h"

//const int DEFAULT_BUFFER_SIZE=1024;



bool CPackFile::ChangeMode(const char vpFileName[],DWORD vMode)
{
	//RTFP_READ               = 0x01,
	//RTFP_WRITE              = 0x02,
	//RTFP_READONLY           = 0x01,
	//RTFP_READWRITE          = 0x03,
	if(RtCoreFile().SetFilePermission(vpFileName,vMode))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CPackFile::DeleteFile(const string& vrFileName)
{
	return RtCoreFile().Delete(vrFileName.c_str())==TRUE;
}


bool CPackFile::IsExist(const string vFileName)
{
	FILE* p=fopen(vFileName.c_str(),"rb");
	if(p==NULL)
	{
		return false;
	}
	else
	{
		fclose(p);
		return true;
	}
}


const long CPackFile::WriteTime(const string& vrFileName)//修改时间
{
	return RtCoreFile().GetModifyTime( vrFileName.c_str());
}















/*
const string& CPackFile::Name()
{
	return mName;
}



const long CPackFile::WriteTime()//修改时间
{
	if(mpFile==NULL)
	{
		return -1;
	}
	else
	{
		return g_pFileManager->GetModifyTime( Name().c_str());
	}
}
*/

/* 同操作系统相关
const long CPackFile::WriteTime()//修改时间
{
	if(mpFile==NULL)
	{
		return -1;
	}
	else
	{
        
		_finddata_t FileData;
		intptr_t  hFile=_findfirst(Name().c_str(),&FileData);
		if(hFile==-1)
		{
			return -1;
		}
		else
		{
			_findclose(hFile);
			return FileData.time_write;
		}
        
	}
}
*/
/*
bool CPackFile::Create(const string& vrFileName)
{
	if(Open(vrFileName,"wb+"))
	{
		return true;
	}
	else
	{
		//创建目录
		string Sub;

		size_t Begin=0;
		size_t End;
		End=vrFileName.find_first_of("\\");

		for(;End!=-1;)
		{
			Sub=vrFileName.substr(0,End);
			g_pFileManager->MakeDirectory(Sub.c_str());
			Begin=End+1;
			End=vrFileName.find_first_of("\\",Begin);
		}
		//创建目录


		if(Open(vrFileName,"wb+"))
		{
			return true;
		}
	}


	mErr<<"Can't create ansi file![" << vrFileName << "]\n";
	return false;
}
*/

/*同操作系统相关
bool CPackFile::Create(const string& vrFileName)
{
	if(Open(vrFileName,"wb+"))
	{
		return true;
	}
	else
	{
		//创建目录
		string Sub;

		size_t Begin=0;
		size_t End;
		End=vrFileName.find_first_of("\\");

		for(;End!=-1;)
		{
			Sub=vrFileName.substr(0,End);
			mkdir(Sub.c_str());
			Begin=End+1;
			End=vrFileName.find_first_of("\\",Begin);
		}
		//创建目录


		if(Open(vrFileName,"wb+"))
		{
			return true;
		}
	}


	mErr<<"Can't create ansi file![" << vrFileName << "]\n";
	return false;
}
*/
/*


bool CPackFile::Open(const string& vrFileName,const string vMode)
{

	mpFile=fopen(vrFileName.c_str(),vMode.c_str());
	if(mpFile==NULL)
	{
		//mErr<<"Can't open pack file:" << vrFileName << "[" << vMode << "]\n";
		return false;
	}
	mName=vrFileName;
	return true;
}
*/


/*
//同系统相关
bool CPackFile::ChangeMode(const char vpFileName[],int vMode)//改变文件属性//SYS\STAT.H
{
    
	if(_chmod(vpFileName,vMode)==0)
	{
		return true;
	}
	else
	{
		return false;
	} 
    
    return false;
}
*/

/*

void CPackFile::Close()
{
	if(mpFile!=NULL)
	{
		fclose(mpFile);
		mpFile=NULL;
	}

	DEL_ONE(mpBuffer)
	mpBuffer=NULL;

	return;
}


bool CPackFile::ReSizeBuffer(const int vSize)
{
	if(vSize<=mBufferSize)
	{
		return true;
	}


	if(mBufferSize==0)
	{
		mBufferSize=DEFAULT_BUFFER_SIZE;
	}

	for(;mBufferSize<vSize;)
	{
		mBufferSize+=DEFAULT_BUFFER_SIZE;
	}

	DEL_ONE(mpBuffer)
	mpBuffer=RT_NEW char[mBufferSize];
	if(mpBuffer==NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}


size_t	CPackFile::GetBuffer(const int vByteCount,void* vpBuffer)
{
	size_t Ret=fread(vpBuffer,sizeof(BYTE),vByteCount,mpFile);
	mCurrent+=(long)Ret;
	return Ret;
}

size_t  CPackFile::PutBuffer(const void* vpBuffer,const int vBufferSize)
{
	size_t Ret= fwrite(vpBuffer,sizeof(BYTE),vBufferSize,mpFile);
	mCurrent+=(long)Ret;
	return Ret;
}

bool CPackFile::PutBuffer( CMemoryBuffer& vrBuffer)
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
  

int	CPackFile::Seek(long vOffset,int vOrigin)
{
	if(vOrigin==SEEK_SET && mCurrent==vOffset)
	{//不用seek了，就是这个位置
		return 0;
	}


	int Return=fseek(mpFile,vOffset,vOrigin);


	if(Return==0)
	{
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
			mCurrent=ftell(mpFile);
		}
	}
	return Return;
}

const long  CPackFile::Size()
{
	long Now=Tell();
	Seek(0,SEEK_END);
	long FileSize=Tell();
	Seek(Now,SEEK_SET);
	return FileSize;
}


const long CPackFile::Tell()
{
	return ftell(mpFile);
}


const char*	CPackFile::GetString(int vCharCount)
{
	ReSizeBuffer(vCharCount+1);
	const size_t Count=fread(mpBuffer,sizeof(char),vCharCount,mpFile);
	mCurrent+=(long)Count;
	mpBuffer[Count]='\0';//结尾零
	return mpBuffer;
}


bool	CPackFile::PutString(const char* vpString,const size_t vStringLength)
{
	if(vStringLength==0)
	{
		mCurrent+=(long)fwrite(vpString,sizeof(char),strlen(vpString),mpFile);
	}
	else
	{
		mCurrent+=(long)fwrite(vpString,sizeof(char),vStringLength,mpFile);
	}
	return true;
}
*/




#include "filepack/ansi_file.h"
// #include "game_string_assistant.h"
#include "filepack/profile.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <minwindef.h>
#include <fileapi.h>
#include <core/rt2_core.h>

const int DEFAULT_BUFFER_SIZE=1024;

//using update_client::Err;
//using update_client::Log;
//using update_client::Debug;



const string& CAnsiFile::OpenMode()
{
	return mMode;
}

const string& CAnsiFile::Name()
{
	return mName;
}

const long CAnsiFile::WriteTime()//�޸�ʱ��
{
	if(mpFile==NULL)
	{
		return -1;
	}
	else
	{
		return RtCoreFile().GetModifyTime( Name().c_str());
	}
}

bool CAnsiFile::Create(const string& vrFileName)
{
	if(Open(vrFileName,"wb+"))
	{
		return true;
	}
	else
	{
		//����Ŀ¼
		string Sub;

		size_t Begin=0;
		size_t End;
		End=vrFileName.find_first_of("\\/");

		for(;End!=-1;)
		{
			Sub=vrFileName.substr(0,End);
			RtCoreFile().MakeDirectory(Sub.c_str());

			Begin=End+1;
			End=vrFileName.find_first_of("\\/",Begin);
		}
		//����Ŀ¼


		if(Open(vrFileName,"wb+"))
		{
			return true;
		}
	}

//	Err<<"Can't create ansi file![" << vrFileName << "]\n";
	return false;
}




/*����ϵͳ���
bool CAnsiFile::Create(const string& vrFileName)
{
	
	
	if(Open(vrFileName,"wb+"))
	{
		return true;
	}
	else
	{
		//����Ŀ¼
		string Sub;

		size_t Begin=0;
		size_t End;
		End=vrFileName.find_first_of("\\");
		
		for(;End!=-1;)
		{
			Sub=vrFileName.substr(0,End);
			mkdir(Sub.c_str());//����ϵͳ���
			
			Begin=End+1;
			End=vrFileName.find_first_of("\\",Begin);
		}
		//����Ŀ¼
		
		
		if(Open(vrFileName,"wb+"))
		{
			return true;
		}
	}


	Err<<"Can't create ansi file![" << vrFileName << "]\n";
	return false;
}
*/

static int All=0;
bool CAnsiFile::Open(const string& vrFileName,const string vMode)
{
	Close();

	//Debug<<"���ļ�[" << vrFileName <<"] mode["<< vMode << "]";

	mpFile=fopen(vrFileName.c_str(),vMode.c_str());
	if(mpFile==NULL)
	{
		return false;
		//mErr<<"Can't open file:" << vrFileName<<"\n";
	}

	All++;
//	Debug<<"һ�����ļ�"<<All<<"\n";

	mName=vrFileName;
	mMode=vMode;
	mCurrent=0;
	mLastMode=NULL_MODE;
	return true;
}


void CAnsiFile::Close()
{
	if(mpFile!=NULL)
	{
		All--;
		fclose(mpFile);
//		Debug<<"һ�����ļ�"<<All<<"\n";
		mpFile=NULL;
	}

	DEL_ONE(mpBuffer);
	mpBuffer=NULL;

	return;
}


bool CAnsiFile::DeleteFile(const string& vrFileName)
{
	return RtCoreFile().Delete(vrFileName.c_str())==TRUE;
}


bool CAnsiFile::ChangeMode(const char vpFileName[],DWORD vMode)//�ı��ļ�����//SYS\STAT.H
{
	/*
	RTFP_READ               = 0x01,
	RTFP_WRITE              = 0x02,
	RTFP_READONLY           = 0x01,
	RTFP_READWRITE          = 0x03,
	*/
	if(RtCoreFile().SetFilePermission(vpFileName,vMode))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CAnsiFile::ReSizeBuffer(const int vSize)
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

	DEL_ONE(mpBuffer);
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


size_t	CAnsiFile::GetBuffer(const int vByteCount,void* vpBuffer)
{
	PROFILE("CAnsiFile_GetBuffer");
	if(mLastMode!=READ_MODE)
	{//"rb+"ģʽ�£��任ģʽ����Ҫfseek����
		fseek(mpFile,0,SEEK_CUR);
		mLastMode=READ_MODE;
	}

	size_t Ret=fread(vpBuffer,sizeof(BYTE),vByteCount,mpFile);
	mCurrent+=(long)Ret;
	return Ret;
}

size_t  CAnsiFile::PutBuffer(const void* vpBuffer,const int vBufferSize)
{	
	if(mLastMode!=WRITE_MODE)
	{//"rb+"ģʽ�£��任ģʽ����Ҫfseek����
		fseek(mpFile,0,SEEK_CUR);
		mLastMode=WRITE_MODE;
	}

	size_t Ret= fwrite(vpBuffer,sizeof(BYTE),vBufferSize,mpFile);
	if(Ret!=vBufferSize)
	{
//		Err<<"PutBuffer Err![" <<this->mName << "]\n";
	}
	mCurrent+=(long)Ret;
	return Ret;
}


int	CAnsiFile::Seek(long vOffset,int vOrigin)
{
	if(vOrigin==SEEK_SET && mCurrent==vOffset)
	{//����seek�ˣ��������λ��
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

const long  CAnsiFile::Size()
{
	long Now=Tell();
	Seek(0,SEEK_END);
	long FileSize=Tell();
	Seek(Now,SEEK_SET);
	return FileSize;
}


const long CAnsiFile::Tell()
{
	return mCurrent;
	//return ftell(mpFile);
}


const char*	CAnsiFile::GetString(int vCharCount)
{
	if(mLastMode!=READ_MODE)
	{//"rb+"ģʽ�£��任ģʽ����Ҫfseek����
		fseek(mpFile,0,SEEK_CUR);
		mLastMode=READ_MODE;
	}

	ReSizeBuffer(vCharCount+1);
	const size_t Count=fread(mpBuffer,sizeof(char),vCharCount,mpFile);
	mCurrent+=(long)Count;
	mpBuffer[Count]='\0';//��β��
	return mpBuffer;
}


bool	CAnsiFile::PutString(const char* vpString,const size_t vStringLength)
{
	if(mLastMode!=WRITE_MODE)
	{//"rb+"ģʽ�£��任ģʽ����Ҫfseek����
		fseek(mpFile,0,SEEK_CUR);
		mLastMode=WRITE_MODE;
	}

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


/*����ϵͳ���
bool CAnsiFile::ChangeMode(const char vpFileName[],int vMode)//�ı��ļ�����//SYS\STAT.H
{
if(_chmod(vpFileName,vMode)==0)
{
return true;
}
else
{
return false;
}
}
*/



#include "filepack/game_pack.h"




const string& CPack::Name()									//得到包的名称
{
	if(mpPackFile==NULL)
	{
		static string NULL_STRING;
		return NULL_STRING;
	}
	else
	{
		return mpPackFile->Name();
	}
}

bool CPack::Init(CPackFile* vpPackFile,	
					CFactory<CCompress,CPack::ECompressMode>* vpCompressFactory)//包的初始化
{
	mpPackFile=vpPackFile;
	mpCompressFactory=vpCompressFactory;
	OnInit();
	return true;
}

CPack::~CPack()
{
	DEL_ONE(mpPackFile);
	mpPackFile=NULL;
}
/*
const CPack::SFileInfo* CPack::GetFileInfo(const char* vpNameInPack)
{
	const string NameInPack(vpNameInPack);
	return GetFileInfo(NameInPack);
}
*/
/*
bool CPack::DelFile(const char* vpNameInPack)
{
	const string NameInPack(vpNameInPack);
	return DelFile(NameInPack);
}
*/
/*
bool CPack::AddFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress)
{
	const string FileName(vpFileName);
	const string NameInPack(vpNameInPack);
	return AddFile(FileName,NameInPack,vpCompress);
}
*/
/*
bool CPack::ReplaceFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,CCompress* vpCompress,long vTime)
{
	const string NameInPack(vpNameInPack);
	return AddFile(NameInPack,vpBuffer,vBufferSize,vpCompress,vTime);
}
*/
/*
bool CPack::ReplaceFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress)
{
	const string FileName(vpFileName);
	const string NameInPack(vpNameInPack);
	return ReplaceFile(FileName,NameInPack,vpCompress);
}
*/
/*
bool CPack::ReplaceFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,CCompress* vpCompress)
{
	const string FileName(vpNameInPack);
	return ReplaceFile(FileName,vpBuffer,vBufferSize,vpCompress);
}
*/


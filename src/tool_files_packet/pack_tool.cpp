//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	pack_tool.cpp
//	author:		ldr123
//	purpose:	
//********************************************************************
#include <iostream>
#include <io.h>
#include "pack_tool.h"
#include "unpack_file.h"


using namespace std;

bool PackAllFiles(const string vFileName,const string vNameInPack,CPack* vpPack,CCompress*	vpCurrentCompressMode,bool vReplace);
string TransformFileName(const char* vpFileName);

CPackManager* CPackTool::Manager()
{
	if(mpManager==NULL)
	{
		mpManager=new CPackManager;
		mpManager->Init();
	}
	return mpManager;
}

bool CPackTool::IsPack(const string vPackName)//�Ƿ��ǰ��ļ�
{
	string PackName=TransformFileName(vPackName.c_str());
	return Manager()->IsPack(PackName);
}

bool CPackTool::SetContentVersion(string& vrMainVersion,string& vrSubVersion)
{
	if(mpLastPack==NULL)
	{
		return false;
	}

	mpLastPack->SetContentVersion(vrMainVersion,vrSubVersion);
	return true;
}

bool CPackTool::GetContentVersion(string& vrMainVersion,string& vrSubVersion)
{
	if(mpLastPack==NULL)
	{
		return false;
	}
	mpLastPack->GetContentVersion(vrMainVersion,vrSubVersion);
	return true;
}

bool CPackTool::GetContentVersion(const string vPackName,string& vrMainVersion,string& vrSubVersion)
{
	string PackName=TransformFileName(vPackName.c_str());
	if(!Manager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return false;
	}

	CPack* pPack=Manager()->GetPack(PackName);
	pPack->GetContentVersion(vrMainVersion,vrSubVersion);
	Manager()->ClosePack(PackName);
	return true;
}

bool CPackTool::GetPackTime(const string vPackName,long& vrStartTime,long& vrEndTime)//��ʱ��
{
	string PackName=TransformFileName(vPackName.c_str());
	if(!Manager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return false;
	}

	CPack* pPack=Manager()->GetPack(PackName);
	
	pPack->GetTime(vrStartTime,vrEndTime);
	Manager()->ClosePack(PackName);
	return true;
}

int	 CPackTool::PackVersion(const string vPackName)//���汾//-1��ʾ�汾�Ѿ���֧��
{
	string PackName=TransformFileName(vPackName.c_str());
	if(!Manager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return -1;
	}

	CPack* pPack=Manager()->GetPack(PackName);
	string	TypeName;
	int		Version=0;
	pPack->GetType(TypeName,Version);
	Manager()->ClosePack(vPackName);
	return Version;

}

bool CPackTool::Open(const string& vrPackName,string vMode)
{
	string PackName=TransformFileName(vrPackName.c_str());
	Manager()->OpenPack(PackName,CPackManager::NORMAL_PACK,vMode);//ͨ�����ߴ򿪵��ļ�����ϣ���ı���ı�־
	mpLastPack=Manager()->GetPack(PackName);
	return true;
}

bool CPackTool::Close(const string& vrPackName)
{
	string PackName=TransformFileName(vrPackName.c_str());
	Manager()->ClosePack(PackName);
	return true;
}

bool CPackTool::GetAllFilesInPack(vector<CPack::SFileInfo*>& vrFiles)
{
	if(mpLastPack==NULL)
	{
		return false;
	}

	vrFiles=mpLastPack->GetAllFileInfo();
	return true;
}

bool CPackTool::Create(const string& vrPackName)
{
	string PackName=TransformFileName(vrPackName.c_str());
	Manager()->CreatePack(PackName,false);
	mpLastPack=Manager()->GetPack(PackName);
	return true;
}

const int DEFAULT_BUFFER_SIZE_4K =4 *1024;
bool CPackTool::CreatePatchPack(const string& vrExeName,const string& vrPackName,const string& vrPatchPackName)
{
	CAnsiFile	ExeFile;
	CAnsiFile	PackFile;
	CAnsiFile   PatchFile;

	if(!CAnsiFile::DeleteFile(vrPatchPackName))
	{
		return false;
	}
	if(!ExeFile.Open(vrExeName))
	{
		return false;
	}

	if(!PackFile.Open(vrPackName))
	{
		return false;
	}
	if(!PatchFile.Create(vrPatchPackName))
	{
		return false;
	}


	static char Temp[DEFAULT_BUFFER_SIZE_4K];
	//exe������
	size_t ReadSize=ExeFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//��������
	for(;ReadSize!=0;)
	{
		if(PatchFile.PutBuffer(Temp,(int)ReadSize)!=(int)ReadSize)//������д��
		{
			return false;
		}
		ReadSize=ExeFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,Temp);//������������
	}
	

	//pack������//zlibѹ��
	CLinearityBuffer UnCompressBuffer;
	UnCompressBuffer.Resize(PackFile.Size());
	PackFile.GetBuffer(PackFile.Size(),UnCompressBuffer.Ptr());

	CLinearityBuffer CompressBuffer;
	CompressBuffer.Resize(UnCompressBuffer.Size()*2+1024);//����ѹ����Ĵ�С
	long CompressSize=CompressBuffer.Size();

	if(compress((Bytef*)CompressBuffer.Ptr(),(uLongf*)&CompressSize,(const Bytef*)(UnCompressBuffer.Ptr()),(UnCompressBuffer.Size()))==Z_OK)
	{//����
		PatchFile.PutBuffer(CompressBuffer.Ptr(),CompressSize);
	}
	else
	{//�쳣
		return false;
	}



	int Offset=ExeFile.Size();//�ļ�����ƫ��//�����ϸ��ļ��Ĵ�С
	int OriginSize=PackFile.Size();//�ļ����Ĵ�С


	//���ļ�����β������ƫ�ƣ�ԭʼ��С��ѹ�����С
	PatchFile.PutBuffer(&Offset,sizeof(Offset));
	PatchFile.PutBuffer(&OriginSize,sizeof(OriginSize));
	PatchFile.PutBuffer(&CompressSize,sizeof(CompressSize));


	ExeFile.Close();
	PackFile.Close();
	CAnsiFile::DeleteFile(vrExeName);
	return true;
}




bool CPackTool::Differ(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack)
{
	string MainPack=TransformFileName(vrMainPack.c_str());
	string AssistPack=TransformFileName(vrAssistPack.c_str());
	string DifferPack=TransformFileName(vrDifferPack.c_str());

	if(Manager()->DifferPack(MainPack,AssistPack,DifferPack))
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*

	Manager()->DeletePack(DifferPack);
	Open(MainPack);
	Open(AssistPack);
	Create(DifferPack);


	CPack* pMain = Manager()->GetPack(MainPack);
	CPack* pAssist = Manager()->GetPack(AssistPack);

	if(pMain==NULL || pAssist==NULL)
	{
		return false;
	}

	vector<CPack::SFileInfo*> MailFiles = pMain->GetAllFileInfo();
	vector<CPack::SFileInfo*> AssistFiles = pAssist->GetAllFileInfo();


*/
/*
bool CPackTool::UnitePatch(const string& vrMainPack,const string& vrAssistPack)//�ϲ��������������Ѳ�����ɾ��
{
	string MainPack=TransformFileName(vrMainPack.c_str());
	string AssistPack=TransformFileName(vrAssistPack.c_str());

	return Manager()->UnitePatch(MainPack,AssistPack);
}
*/


bool CPackTool::Unite(const string& vrMainPack,const string& vrAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress)
{
	string MainPack=TransformFileName(vrMainPack.c_str());
	string AssistPack=TransformFileName(vrAssistPack.c_str());

	if(Manager()->AddPack(MainPack,AssistPack,vReplace,vAllowOld,vNullCompress))
	{
		Manager()->DeletePack(AssistPack);
		return true;
	}
	else
	{
		return false;
	}
}



/*
bool CPackTool::Unite(const string& vrMainPack,const string& vrAssistPack)
{//��vrAssistPack��ӵ�vrMainPack��
string MainPack=TransformFileName(vrMainPack.c_str());
string AssistPack=TransformFileName(vrAssistPack.c_str());

if(!Manager()->OpenPack(AssistPack))
{
return false;
}

if(!Manager()->OpenPack(MainPack))
{
return false;
}

CPack* pMainPack=Manager()->GetPack(MainPack);
CPack* pAssistPack=Manager()->GetPack(AssistPack);

vector<CPack::SFileInfo*> AllFiles;//�õ����������ļ���Ϣ
vector<CPack::SFileInfo*>::iterator It;

AllFiles=pAssistPack->GetAllFileInfo();

CUnpackFile* pFile;
for(It=AllFiles.begin();It!=AllFiles.end();++It)
{
pFile=pAssistPack->GetUnpackFile(*It);
if(pFile!=NULL)
{
pMainPack->AddFile((*It)->mName,*pFile,mpCurrentCompressMode);
delete pFile;
}
}

return true;
}
*/
bool CPackTool::Unpack()
{
	if(mpLastPack==NULL)
	{
		return false;
	}

	return Manager()->UnpackAllFile(mpLastPack->Name());
}



bool CPackTool::Unpack(const string& vrPackName)
{
	string PackName=TransformFileName(vrPackName.c_str());
	return Manager()->UnpackAllFile(PackName);
}

bool CPackTool::UnpackFile(const string& vrPackName, const string& vrFileName)
{
	string FileName = TransformFileName(vrFileName.c_str());
	string PackName = TransformFileName(vrPackName.c_str());
	return Manager()->UnpackFile(PackName, FileName);
}

bool CPackTool::UnpackFloder(const string& vrPackName, const string& vrFloderInPack)
{
	string PackName = TransformFileName(vrPackName.c_str());
	string FloderInPack = TransformFileName(vrFloderInPack.c_str());
	return Manager()->UnpackAllFile(PackName, FloderInPack);
}

bool CPackTool::Pack(const string vFileName,const string vNameInPack,bool vReplace)
{
	if(mpLastPack==NULL)
	{
		return false;
	}

	PackAllFiles(vFileName,vNameInPack,mpLastPack,vReplace);
	return true;
}

bool CPackTool::Delete(const string& vrNameInPack)
{
	if(mpLastPack==NULL)
	{
		return false;
	}
	string NameInPack=TransformFileName(vrNameInPack.c_str());
	mpLastPack->DelFile(NameInPack);
	return true;
}

bool CPackTool::Add(const string vFileName,const string vNameInPack,bool vReplace)
{
	if(mpLastPack==NULL)
	{
		return false;
	}

	string FileName=TransformFileName(vFileName.c_str());
	string NameInPack=TransformFileName(vNameInPack.c_str());

	string Postfix=vFileName.substr(vFileName.find_last_of('.')+1);//�õ��ļ���׺
	if(mFilter.Filter(Postfix))
	{//����
		return false;
	}

	CCompress*	pCompress = GetCompressByOpertion(FileName);
	mpLastPack->AddFile(FileName,NameInPack,pCompress ,vReplace);
	return true;
}

void CPackTool::UpdateToCipherVersion()			//ac.ma
{
	mpLastPack->UpdateToCipherVersion();
}

bool CPackTool::Pack(const string vFileName,const string vNameInPack,const string& vrPackName,bool vReplace)
{
	string PackName=TransformFileName(vrPackName.c_str());
	mpLastPack=Manager()->GetPack(PackName);
	if(mpLastPack==NULL)
	{
		return false;
	}
	return Pack(vFileName,vNameInPack,vReplace);
}


bool CPackTool::Add(const string vFileName,const string vNameInPack,const string& vrPackName,bool vReplace)
{
	string PackName=TransformFileName(vrPackName.c_str());
	mpLastPack=Manager()->GetPack(PackName);
	if(mpLastPack==NULL)
	{
		return false;
	}
	return Add(vFileName,vNameInPack,vReplace);
}

bool CPackTool::SetOpertion(const CPack::ECompressMode vMode,const int vVersion)//����ѡ��
{
	mpCurrentCompressMode = Manager()->GetCompress(vMode,vVersion);
	return true;
}



bool CPackTool::PackAllFiles(const string vFileName,const string vNameInPack,CPack* vpPack,bool vReplace)
{
	_finddata_t FileData; 
	intptr_t hFile; 

	string FileName=TransformFileName(vFileName.c_str());
	string NameInPack=TransformFileName(vNameInPack.c_str());

	FileName =="." ? FileName="" : FileName+="\\" ;
	NameInPack =="." ? NameInPack="" : NameInPack+="\\" ;


	string FindName=FileName+"*.*";


	hFile=_findfirst(FindName.c_str(),&FileData);
	if(hFile==-1)
	{
		return false;
	}
	else
	{
		if(FileData.attrib&_A_SUBDIR)
		{

			string SubDir(FileData.name);
			if(SubDir!="." && SubDir!="..")
			{
				PackAllFiles(FileName+SubDir,NameInPack+SubDir,vpPack,vReplace);
			}
		}

		for(;_findnext(hFile,&FileData)==0;)
		{
			if(FileData.attrib&_A_SUBDIR)
			{
				string SubDir(FileData.name);
				if(SubDir!="." && SubDir!="..")
				{
					PackAllFiles(FileName+SubDir,NameInPack+SubDir,vpPack,vReplace);
				}
			}
			else
			{
				string SubName=TransformFileName(FileData.name);
				string FileFullName=FileName+SubName;

				string Postfix=FileFullName.substr(FileFullName.find_last_of('.')+1);//�õ��ļ���׺
				if(mFilter.Filter(Postfix))
				{//����
					cout<<FileFullName<<"[������]"<<endl;
				}
				else
				{
					cout<<FileFullName<<"["<<FileData.size <<"] MD5[";

					CCompress* pCompress=GetCompressByOpertion(FileFullName);
					vpPack->AddFile(FileFullName,NameInPack+SubName,pCompress,vReplace);
					cout<<"]......"<<endl;
				}
			}
		}
		_findclose(hFile);
	}
	return true;
}

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



/*
bool CPackTool::Replace(const string vFileName,const string vNameInPack)
{
if(mpLastPack==NULL)
{
return false;
}
string FileName=TransformFileName(vFileName.c_str());
string NameInPack=TransformFileName(vNameInPack.c_str());
mpLastPack->ReplaceFile(FileName,NameInPack,mpCurrentCompressMode);
return true;
}
*/
/*
bool CPackTool::Replace(const string vFileName,const string vNameInPack,const string& vrPackName)
{
string PackName=TransformFileName(vrPackName.c_str());
mpLastPack=Manager()->GetPack(PackName);
if(mpLastPack==NULL)
{
return false;
}
string FileName=TransformFileName(vFileName.c_str());
string NameInPack=TransformFileName(vNameInPack.c_str());
return Replace(vFileName,vNameInPack);
}
*/



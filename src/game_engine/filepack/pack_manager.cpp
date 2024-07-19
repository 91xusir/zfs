

#include "filepack/pack_manager.h"

#include <list>
#include <vector>
#include "filepack/normal_pack.h"
#include "filepack/unpack_file.h"

const int PACK_FLAG_LENGTH=9;//��������β'\0'

//˳��Ҫͬö�ٵ�˳��һ��//�ַ����ĳ��ȱ�����9
const char* PACK_FLAG[]={"GAME_PACK","WRITEPACK","READ_PACK","CHECKPACK"};

using std::list;
using std::vector;


CPackManager::CPackManager()
:mErr(),mPackFactory(),mPackList(),mCompressFactory()
{
}


CPackManager::~CPackManager()
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();It++)
	{
		DEL_ONE((*It))
	}
}

void CPackManager::Clear()
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();It++)
	{
		(*It)->Save();
	}
}




/*
bool CPackManager::WritePackFlag(CPackFile& vrFile,EPackFlag vPackFlag)
{
	if(NORMAL_PACK <= vPackFlag && vPackFlag<UNKNOW_PACK)
	{//�ܹ�ʶ��İ�
	}
	else
	{
		return false;
	}

	long Now=vrFile.Tell();
	vrFile.Seek(0,SEEK_SET);
	if(vrFile.PutBuffer( PACK_FLAG[(int)vPackFlag],(int)strlen(PACK_FLAG[(int)vPackFlag])+1 ) 
		!= (strlen( PACK_FLAG[(int)vPackFlag] ) +1) ) 
	{
		vrFile.Seek(Now,SEEK_SET);
		return false;
	}
	vrFile.Seek(Now,SEEK_SET);
	return true;
}
*/

CPackManager::EPackFlag CPackManager::CheckPackFlag(CPackFile& vrFile)
{
	char pFlag[PACK_FLAG_LENGTH+1];
	vrFile.GetBuffer(PACK_FLAG_LENGTH+1,pFlag);

	string Flag=pFlag;

	int i;
	for(i=0;i<(int)UNKNOW_PACK;i++)
	{
		if(Flag==PACK_FLAG[i])
		{
			break;
		}
	}

	return (EPackFlag)i;
}


bool CPackManager::Init()
{
	mPackFactory.RegisterType<CNormalPack>();
	mCompressFactory.RegisterType<CNULLCompress>();
	mCompressFactory.RegisterType<CDeriveCompress<CLzwArithmetic> >();
	mCompressFactory.RegisterType<CDeriveCompress<CZlibArithmetic> >();

	return true;
}

CCompress* CPackManager::GetCompress(const CPack::ECompressMode vMode,const int vVersion)
{
	return mCompressFactory.GetReference(vMode,vVersion);
}


CPackManager::EPackFlag CPackManager::CheckPack(const string& vrPackName)		//���һ����
{
	CAnsiPackFile PackFile;

	if(!PackFile.Open(vrPackName,"rb"))
	{
		return UNKNOW_PACK;
	}

	//���
	return CheckPackFlag(PackFile);
}


bool CPackManager::ExistPack(const string& vrPackName)
{
	if(PackIsOpen(vrPackName))
	{
		return true;
	}

	CAnsiPackFile PackFile;

	if(!PackFile.Open(vrPackName,"rb"))
	{
		return false;
	}

	//����ļ���־
	if(CheckPackFlag(PackFile)==UNKNOW_PACK)
	{
		return false;
	}

	return true;
}
bool CPackManager::IsPack(const string vPackName)			//�Ƿ��ǰ��ļ�
{
	if(PackIsOpen(vPackName))
	{//�Ѿ��򿪣���Ȼ��
		return true;
	}

	CAnsiPackFile File;
	if(File.IsExist(vPackName))
	{//�ļ�����
		if(File.Open(vPackName,"rb"))
		{
			//����ļ���־
			if(CheckPackFlag(File)!=UNKNOW_PACK)
			{
				return true;
			}
		}
	}
	return false;
}

/*
bool CPackManager::OpenPatchPack(const string& vrPackName)   //��һ��������
{
	//��һ��������
	if(PackIsOpen(vrPackName))
	{
		return true;
	}

	CPackFile* pFile=NULL;
	pFile=RT_NEW CFlagPackFile;

	if(pFile==NULL)
	{
		return false;
	}

	if(!pFile->Open(vrPackName,"rb"))
	{
		DEL_ONE(pFile)
		return false;
	}

	//������
	pFile->Seek( ((-(int)(sizeof(int))) * 2),SEEK_END);//�������������ƫ�ƺͱ���
	
	int Offset=0;
	*pFile>>Offset;
	pFile->Seek(Offset,SEEK_SET);
	
	if(!OpenPack(*pFile,READ_PACK))
	{
		DEL_ONE(pFile)
		return false;
	}

	return true;
}
*/

bool CPackManager::OpenPack(CPackFile& vrFile,const EPackFlag vOpenFlag)//��һ����
{
	//����ļ���־
	if(CheckPackFlag(vrFile)==UNKNOW_PACK)
	{
		return false;
	}

	if(vOpenFlag!=NORMAL_PACK)
	{
		vrFile.SetFlag(PACK_FLAG[(int)vOpenFlag],PACK_FLAG[NORMAL_PACK],-(PACK_FLAG_LENGTH+1));
	}
	

	long Begin=vrFile.Tell();

	string	TypeName;	//������
	int		Version;	//���汾

	vrFile>>TypeName;
	vrFile>>Version;

	vrFile.Seek(Begin-vrFile.Tell());

	CPack* pPack =mPackFactory.Get(TypeName,Version);
	if(pPack==NULL)
	{
		return false;
	}

	//���ĳ�ʼ��
	if(!pPack->Init(&vrFile,&mCompressFactory))
	{
		return false;
	}
	pPack->Open();
	mPackList.push_back(pPack);
	return true;
}


bool CPackManager::OpenPack(const string& vrPackName,const EPackFlag vOpenFlag ,const string vMode,bool vInMemory)//�򿪰�
{
	//���ļ�
	if(PackIsOpen(vrPackName))
	{
		return true;
	}

	CPackFile* pFile=NULL;
	if(vInMemory)
	{
		pFile=RT_NEW CMemoryPackFile;
	}
	else
	{
		pFile=RT_NEW CFlagPackFile;//CAnsiPackFile;
	}

	
	if(pFile==NULL)
	{
		return false;
	}


	if(!pFile->Open(vrPackName,vMode))
	{
		if(pFile->Open(vrPackName,"rb"))
		{//������ֻ����ԭ��
			pFile->Close();
			CPackFile::ChangeMode(vrPackName.c_str(),RtFileManager::FP_WRITE);//�ı��ļ�����
			if(!pFile->Open(vrPackName,vMode))
			{//���򲻿�
                mErr<<"Cannot open package ["<<vrPackName<<"]\n";
				DEL_ONE(pFile)
				return false;
			}
		}
		else
		{//�򲻿�
			mErr<<"Cannot open package ["<<vrPackName<<"]\n";
			DEL_ONE(pFile)
			return false;
		}
	}

	if(!OpenPack(*pFile,vOpenFlag))
	{
		DEL_ONE(pFile)
		return false;
	}

	return true;
}



bool CPackManager::PackIsOpen(const string& vrPackName)//���Ƿ����
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();It++)
	{
		if((*It)->Name()==vrPackName)
		{
			return true;
		}
	}
	return false;
}
/*
CPack* CPackManager::GetPack()
{
	if(mPackList.size()==0)
	{
		return NULL;
	}
	else
	{
		return (*(mPackList.begin()));
	}
}
*/

bool CPackManager::Repear(const string vPackName)			//�޸�ĳ����//true�޸�//falseû�޸�
{
	OpenPack(vPackName,CHECK_PACK,"rb+");
	CPack* pPack=GetPack(vPackName);
	if(pPack==NULL)
	{
		//Err<<"���ļ����ǰ��ļ����޷��޸���ɾ����";
		DeletePack(vPackName);//�޷��޸���ɾ��
		return false;
	}

	pPack->Repair();
	if(!ClosePack(pPack))
	{
		return false;
	}

	return true;
}

CPack* CPackManager::GetPack(const string& vrPackName)
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();++It)
	{
		if((*It)->Name()==vrPackName)
		{
			return (*It);
		}
	}
	return NULL;
}

bool CPackManager::ClosePack(CPack* vpPack)					//�ر�һ����
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();It++)
	{
		if((*It)==vpPack)
		{
			DEL_ONE((*It))
			mPackList.erase(It);
			return true;
		}
	}
	return false;
}

bool CPackManager::ClosePack(const string& vrPackName)//�ر�һ����
{
	list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();It++)
	{
		if((*It)->Name()==vrPackName)
		{
			DEL_ONE((*It))
			mPackList.erase(It);
			return true;
		}
	}
	return false;
}





bool CPackManager::UnpackAllFile(const string& vrPackName)
{
    if(PackIsOpen(vrPackName))
    {

    }
    else
    {
        if(!OpenPack(vrPackName,NORMAL_PACK,"rb"))//����д���
        {
            mErr<<vrPackName<<"not find!";
        }
    }


    CPack* pPack=NULL;
    pPack=GetPack(vrPackName);
    if(pPack==NULL)
    {
        return false;
    }

	vector<CPack::SFileInfo*> AllFiles=pPack->GetAllFileInfo();

	vector<CPack::SFileInfo*>::iterator It;
	for(It=AllFiles.begin();It!=AllFiles.end();++It)
	{
		UnpackFile(vrPackName,(*It)->mName);
	}

/*
    size_t Count=pPack->GetFileCount();
	if(Count==0)
	{
		return false;
	}

	CPack::SFileInfo** ppTemp=RT_NEW CPack::SFileInfo* [Count];
    pPack->GetAllFileInfo(ppTemp);


    for(size_t i=0;i<Count;i++)
    {
        UnpackFile(vrPackName,ppTemp[i]->mName);
    }

	DEL_ARRAY(ppTemp)
	*/
    return true;

}


bool CPackManager::UnpackFile(const string& vrFileName)						//�����д򿪵İ��н�ѹһ���ļ�
{
	std::list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();++It)
	{
		if(UnpackFile((*It),vrFileName))
		{
			return true;
		}
	}
	return false;
}

/*
size_t CPackManager::GetOpenPackCount()
{
	return mPackList.size();
}*/

vector<CPack*> CPackManager::GetAllOpenPack()				//�õ��Ѿ��򿪵����еİ�
{
	vector<CPack*> AllPack;

	std::list<CPack*>::iterator It;
	for(It=mPackList.begin();It!=mPackList.end();++It)
	{
		AllPack.push_back(*It);
	}
	return AllPack;
}
/*
bool CPackManager::GetAllOpenPack(CPack* vpPack[])                             //�õ����������ļ���Ϣ
{
	std::list<CPack*>::iterator It;
	int i=0;
	for(It=mPackList.begin();It!=mPackList.end();++It)
	{
		vpPack[i++]=*It;
	}
	return true;
}
*/

bool CPackManager::UnpackFile(CPack* vpPack,const string& vrFile)
{
	const CPack::SFileInfo* pFileInfo=NULL;
	pFileInfo=vpPack->GetFileInfo(vrFile);
	if(pFileInfo==NULL)
	{
		mErr<<vrFile<<" no find!";
		return false;
	}

	char* pTemp=RT_NEW char[pFileInfo->mOriginSize];

	vpPack->LoadFile(pFileInfo,pTemp);

	CAnsiPackFile Temp;
//afeng.wu 
	string unPackFile = pFileInfo->mName;
#ifdef WIN32

#else
	
	for (int i = 0; i < unPackFile.size(); i++)
	{
		if (unPackFile[i] == '\\')
			unPackFile[i] = '/';
	}
#endif
	if(Temp.Create(unPackFile))
	{
		Temp.PutBuffer(pTemp,pFileInfo->mOriginSize);
		Temp.Close();
	}
	DEL_ONE(pTemp)
	return true;
}



bool CPackManager::UnpackFile(const string& vrPackName,const string& vrFile)
{
    CPack* pPack=NULL;

    pPack=GetPack(vrPackName);
    if(pPack==NULL)
    {
        mErr<<"Not package is open!";
        return false;
    }

	return UnpackFile(pPack,vrFile);
}

//const int DEFAULT_MEMORY_PACK_SIZE=250*1024*1024;//250M
const int DEFAULT_MEMORY_PACK_SIZE=250*1024*1024;//250M
bool CPackManager::CreatePack(const string& vrPackName,bool vInMemory)//bool vInMemory�Ƿ�����ڴ���
{
	//�����ļ�
	if(PackIsOpen(vrPackName))
	{
		return false;
	}

	CPackFile* pFile=NULL;
	if(vInMemory)
	{//�ڴ��ļ�
		//pFile=RT_NEW CMemoryPackFile;
		CMemoryPackFile* pTemp=RT_NEW CFlagMemoryPackFile;
		pFile=pTemp;
		if(pTemp!=NULL)
		{
			if(!pTemp->Resize(DEFAULT_MEMORY_PACK_SIZE))
			{//����޷��õ�Ҫ���ڴ�
				DEL_ONE(pFile)
				return false;
			}
		}
	}
	else
	{//�����ļ�
		pFile=RT_NEW CFlagPackFile;
	}

	
	if(pFile==NULL)
	{
		return false;
	}
	if(!pFile->Create(vrPackName))
	{
		DEL_ONE(pFile)
		return false;
	}


	//д�ļ����
	pFile->SetFlag(PACK_FLAG[(int) (WRITE_PACK)],PACK_FLAG [(int) (NORMAL_PACK)],0);
	pFile->Seek(PACK_FLAG_LENGTH+1,SEEK_SET);//�ļ�seek����Ǻ�

	CPack* pPack =RT_NEW CNormalPack;//mPackFactory.GetPack<CNormalPack>();
	if(pPack==NULL)
	{
		DEL_ONE(pFile)
		return false;
	}

	//���ĳ�ʼ��
	if(!pPack->Init(pFile,&mCompressFactory))
	{
		DEL_ONE(pFile)
		return false;
	}
	pPack->Create();
	pPack->SetTime(0,-1);
	mPackList.push_back(pPack);
	return true;
}
/*
bool CPackManager::ReplacePack(const string& vrMainPack,const string& vrAssistPack)//���������Ķ����滻������
{
	if(!OpenPack(vrMainPack))
	{
		return false;
	}

	if(!OpenPack(vrAssistPack))
	{
		return false;
	}

	CPack* pMainPack=GetPack(vrMainPack);
	CPack* pAssistPack=GetPack(vrAssistPack);

	return ReplacePack(pMainPack,pAssistPack);
}
*/
/*
bool CPackManager::ReplacePack(CPack* vpMainPack,CPack* vpAssistPack)//���������Ķ����滻������
{
	vector<CPack::SFileInfo*> AllFiles;//�õ����������ļ���Ϣ
	vector<CPack::SFileInfo*>::iterator It;
	AllFiles=vpAssistPack->GetAllFileInfo();


	CCompress*			pCompress;
	CPack::SFileInfo*	pFile;//����������ļ���Ϣ

	CUnpackFile* pUnpackFile;
	for(It=AllFiles.begin();It!=AllFiles.end();++It)
	{
		pFile=*It;
		pCompress=mCompressFactory.GetReference(pFile->mCompressMode,pFile->mCompressVersion);

		pUnpackFile=vpAssistPack->GetUnpackFile(*It);
		if(pUnpackFile!=NULL)
		{
			vpMainPack->AddFile(pFile->mName,*pUnpackFile,pCompress);
			DEL_ONE(pUnpackFile)
		}
	}

	vpMainPack->Save();
	return true;
}
*/

//�������п۳����������Ѿ��е��ļ�
bool CPackManager::DifferPack(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack)
{
	if(!OpenPack(vrMainPack,NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return false;
	}

	if(!OpenPack(vrAssistPack,NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return false;
	}

	DeletePack(vrDifferPack);

	if(!CreatePack(vrDifferPack,false))
	{
		return false;
	}

	CPack* pMainPack=GetPack(vrMainPack);
	CPack* pAssistPack=GetPack(vrAssistPack);
	CPack* pDifferPack=GetPack(vrDifferPack);


	return DifferPack(pMainPack,pAssistPack,pDifferPack);
}
/*
bool CPackManager::UnitePatch(const string& vrMainPack,const string& vrAssistPack)
{
	if(!OpenPack(vrMainPack,WRITE_PACK,"rb+"))
	{
		return false;
	}

	if(!OpenPatchPack(vrAssistPack))
	{
		return false;
	}

	CPack* pMainPack=GetPack(vrMainPack);
	CPack* pAssistPack=GetPack(vrAssistPack);

	if(!AddPack(pMainPack,pAssistPack,true,true))
	{
		return false;
	}

	DeletePack(vrAssistPack);

	return true;


}
*/

bool CPackManager::AddPack(const string& vrMainPack,const string& vrAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress)
{//��vrAssistPack��ӵ�vrMainPack��
	if(!OpenPack(vrMainPack,WRITE_PACK,"rb+"))
	{
		return false;
	}

	if(!OpenPack(vrAssistPack,NORMAL_PACK,"rb"))//��ϣ���ı��־
	{
		return false;
	}

	CPack* pMainPack=GetPack(vrMainPack);
	CPack* pAssistPack=GetPack(vrAssistPack);

	return AddPack(pMainPack,pAssistPack,vReplace,vAllowOld,vNullCompress);
}

const int MD5_LENGTH=16;						//MD5�ĳ���
bool CPackManager::DifferPack(CPack* vpMainPack,CPack* vpAssistPack,CPack* vpDifferPack)
{
	//�������ݵ����汾��Ϣ�Ƿ���ͬ//���ݴΰ汾��Ϣ����ʱû��ʹ��
	string MainContentMainVersion;
	string MainContentSubVersion;
	string AssistContentMainVersion;
	string AssistContentSubVersion;

	vpMainPack->GetContentVersion(MainContentMainVersion,MainContentSubVersion);
	vpAssistPack->GetContentVersion(AssistContentMainVersion,AssistContentSubVersion);

	if(MainContentMainVersion!=AssistContentMainVersion)
	{//������Ҫ�汾��ͬ//����
		return false;
	}
	//�������ʱ��,��������ʱ��ο۳���������ʱ��//������������������һ����//��������һ�˱���ͬ����һ��
	long MainStartTime=0;
	long MainEndTime=0;
	long AssistStartTime=0;
	long AssistEndTime=0;

	long DifferStartTime=0;
	long DifferEndTime=0;

	vpMainPack->GetTime(MainStartTime,MainEndTime);
	vpAssistPack->GetTime(AssistStartTime,AssistEndTime);

	if(!(MainStartTime==AssistStartTime || MainEndTime==AssistEndTime))
	{//û��һ����һ����//����
		return false;
	}

	if(AssistStartTime<MainStartTime)
	{//�������Ѿ��в��ֲ����������У�����
		return false;
	}

	if(AssistEndTime>MainEndTime)
	{//�������Ѿ��в��ֲ����������У�����
		return false;
	}

	DifferStartTime = (MainStartTime==AssistStartTime) ? AssistEndTime : MainStartTime; 
	DifferEndTime =	(MainStartTime==AssistStartTime) ? MainEndTime : AssistStartTime;


	//��������������汾//����������ݴΰ汾//ͬ����һ��
	vpDifferPack->SetContentVersion(MainContentMainVersion,MainContentSubVersion);
	
	//�����������ʱ��汾
	vpDifferPack->SetTime(DifferStartTime,DifferEndTime);



	//�õ�һ���е��ļ����б�
	vector<CPack::SFileInfo*> MainFiles;
	MainFiles=vpMainPack->GetAllFileInfo();
	
	list<CPack::SFileInfo*> FilesList;

	{
		vector<CPack::SFileInfo*>::iterator It;
		for(It=MainFiles.begin();It!=MainFiles.end();++It)
		{
			FilesList.push_back(*It);
		}
	}


	//���ļ��б���//�������е��ļ�ɾ��
	const CPack::SFileInfo* pInfo=NULL;
	list<CPack::SFileInfo*>::iterator ListIt;
	for(ListIt=FilesList.begin();ListIt!=FilesList.end();)
	{
		pInfo=vpAssistPack->GetFileInfo((*ListIt)->mName);
		if(pInfo==NULL)
		{//û�и��ļ�//Ҫ����ļ�
			++ListIt;
		}
		else
		{//�и��ļ�//�ȶ�MD5ֵ

			int i;
			bool MD5Ok=true;
			for(i=0;i<MD5_LENGTH;i++)
			{
				if((*ListIt)->mMD5[i] != pInfo->mMD5[i])
				{
					MD5Ok=false;
					break;
				}
			}

			if(MD5Ok)
			{//md5��ͬ
				if (((*ListIt)->mCompressMode==pInfo->mCompressMode) 
					&& ((*ListIt)->mCompressVersion==pInfo->mCompressVersion))
				{//ѹ����ʽ�Ͱ汾����ͬ//��Ҫ����ļ�
					ListIt=FilesList.erase(ListIt);
				}
				else
				{//ѹ����ʽ�Ͱ汾�в�ͬ//Ҫ����ļ�
					++ListIt;
				}
			}
			else
			{//md5��ͬ//Ҫ����ļ�
				++ListIt;
			}
		}
	}

	//���ļ�д��differ��
	CCompress*			pCompress=NULL;
	CPack::SFileInfo*	pFile=NULL;

	CUnpackFile*		pUnpackFile=NULL;
	

	for(ListIt=FilesList.begin();ListIt!=FilesList.end();++ListIt)
	{
		pFile=*ListIt;
		pCompress=mCompressFactory.GetReference(pFile->mCompressMode,pFile->mCompressVersion);

		pUnpackFile=vpMainPack->GetUnpackFile(pFile);
		if(pUnpackFile!=NULL)
		{
			vpDifferPack->AddFile(pFile->mName,*pUnpackFile,pCompress,true);
			DEL_ONE(pUnpackFile)
		}
	}

	vpDifferPack->Save();
	return true;
}

bool CPackManager::AddPack(CPack* vpMainPack,CPack* vpAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress)
{
	//�������Ƿ��ܼ�������//���汾��Ϣ�Ƿ���ͬ//ʱ��汾�ļ��
	long AssistStartTime=0;
	long AssistEndTime=0;
	long MainStartTime=0;
	long MainEndTime=0;

	string MainContentMainVersion;
	string MainContentSubVersion;
	string AssistContentMainVersion;
	string AssistContentSubVersion;

	vpAssistPack->GetContentVersion(AssistContentMainVersion,AssistContentSubVersion);
	vpAssistPack->GetTime(AssistStartTime,AssistEndTime);
	
	vpMainPack->GetContentVersion(MainContentMainVersion,MainContentSubVersion);
	vpMainPack->GetTime(MainStartTime,MainEndTime);

	//�������汾���
	if(MainContentMainVersion!=AssistContentMainVersion)
	{//���汾����ͬ//����ȷ
	//	return false;
	}

	//ʱ��汾���
	if(MainEndTime < AssistStartTime || AssistEndTime < MainStartTime )
	{//����//����������û�н���

		return false;
	}

	if(!vAllowOld)
	{//��������Ӿɰ汾��Ϣ
		if(MainEndTime >= AssistEndTime)
		{//����ͬ������һ���£������//û�б�Ҫ���
			return false;
		}
	}


	//����ʱ��//��������ʱ��ı�
	MainStartTime = ( MainStartTime < AssistStartTime ) ? MainStartTime : AssistStartTime;
	MainEndTime= ( MainEndTime > AssistEndTime ) ? MainEndTime : AssistEndTime;

	vpMainPack->SetTime(MainStartTime,MainEndTime);


	vector<CPack::SFileInfo*> AllFiles;//�õ����������ļ���Ϣ
	vector<CPack::SFileInfo*>::iterator It;
	AllFiles=vpAssistPack->GetAllFileInfo();

	CCompress*			pCompress;
	CPack::SFileInfo*	pFile;//����������ļ���Ϣ

	CUnpackFile* pUnpackFile;
	for(It=AllFiles.begin();It!=AllFiles.end();++It)
	{
		pFile=*It;
		if(vNullCompress)
		{
			pCompress=mCompressFactory.GetReference(CPack::NULL_COMPRESS,CNULLCompress::LAST_VERSION);
		}
		else
		{
			pCompress=mCompressFactory.GetReference(pFile->mCompressMode,pFile->mCompressVersion);
		}

		pUnpackFile=vpAssistPack->GetUnpackFile(*It);
		if(pUnpackFile!=NULL)
		{
			vpMainPack->AddFile(pFile->mName,*pUnpackFile,pCompress,vReplace);
			DEL_ONE(pUnpackFile)
		}
	}


	//���������Ľ���ʱ������Ϊ�������Ľ���ʱ��


	vpMainPack->Save();
	
	return true;
}

bool CPackManager::DeletePack(CPack* vpPack)					//ɾ��һ����
{
	return DeletePack(vpPack->Name());
}

bool CPackManager::DeletePack(const string& vrPackName)		//ɾ��һ����
{
	string Name(vrPackName);
	ClosePack(vrPackName);
	return CPackFile::DeleteFile(Name);//���Ѿ��رգ�Ҫ�ȸ���һ�°�������
}

bool CPackManager::UnpackAllFile(const string& vrPackName, const string& sFliter)
{
	if(PackIsOpen(vrPackName))
	{

	}
	else
	{
		if(!OpenPack(vrPackName,NORMAL_PACK,"rb"))//����д���
		{
			mErr<<vrPackName<<"not find!";
		}
	}


	CPack* pPack=NULL;
	pPack=GetPack(vrPackName);
	if(pPack==NULL)
	{
		return false;
	}

	vector<CPack::SFileInfo*> AllFiles=pPack->GetAllFileInfo();

	vector<CPack::SFileInfo*>::iterator It;

	for(It=AllFiles.begin();It!=AllFiles.end();++It)
	{
		if (FileFliter(sFliter, (*It)->mName))
		{
			UnpackFile(vrPackName,(*It)->mName);
		}		
	}
	return true;
}

bool CPackManager::FileFliter(const string& sFliter, const string& sObjectFileName)
{	

	int fliterLength = sFliter.length();
	if (sFliter.compare(0,fliterLength,sObjectFileName, 0, fliterLength) != 0)
	{
		return false;
	}
	return true;
}


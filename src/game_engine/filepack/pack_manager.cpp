

#include "filepack/pack_manager.h"

#include <list>
#include <vector>
#include "filepack/normal_pack.h"
#include "filepack/unpack_file.h"

const int PACK_FLAG_LENGTH=9;//不包括结尾'\0'

//顺序要同枚举的顺序一样//字符串的长度必须是9
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
	{//能够识别的包
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


CPackManager::EPackFlag CPackManager::CheckPack(const string& vrPackName)		//检查一个包
{
	CAnsiPackFile PackFile;

	if(!PackFile.Open(vrPackName,"rb"))
	{
		return UNKNOW_PACK;
	}

	//检查
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

	//检查文件标志
	if(CheckPackFlag(PackFile)==UNKNOW_PACK)
	{
		return false;
	}

	return true;
}
bool CPackManager::IsPack(const string vPackName)			//是否是包文件
{
	if(PackIsOpen(vPackName))
	{//已经打开，当然是
		return true;
	}

	CAnsiPackFile File;
	if(File.IsExist(vPackName))
	{//文件存在
		if(File.Open(vPackName,"rb"))
		{
			//检查文件标志
			if(CheckPackFlag(File)!=UNKNOW_PACK)
			{
				return true;
			}
		}
	}
	return false;
}

/*
bool CPackManager::OpenPatchPack(const string& vrPackName)   //打开一个补丁包
{
	//打开一个补丁包
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

	//补丁包
	pFile->Seek( ((-(int)(sizeof(int))) * 2),SEEK_END);//最后两个数字是偏移和保留
	
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

bool CPackManager::OpenPack(CPackFile& vrFile,const EPackFlag vOpenFlag)//打开一个包
{
	//检查文件标志
	if(CheckPackFlag(vrFile)==UNKNOW_PACK)
	{
		return false;
	}

	if(vOpenFlag!=NORMAL_PACK)
	{
		vrFile.SetFlag(PACK_FLAG[(int)vOpenFlag],PACK_FLAG[NORMAL_PACK],-(PACK_FLAG_LENGTH+1));
	}
	

	long Begin=vrFile.Tell();

	string	TypeName;	//包类型
	int		Version;	//包版本

	vrFile>>TypeName;
	vrFile>>Version;

	vrFile.Seek(Begin-vrFile.Tell());

	CPack* pPack =mPackFactory.Get(TypeName,Version);
	if(pPack==NULL)
	{
		return false;
	}

	//包的初始化
	if(!pPack->Init(&vrFile,&mCompressFactory))
	{
		return false;
	}
	pPack->Open();
	mPackList.push_back(pPack);
	return true;
}


bool CPackManager::OpenPack(const string& vrPackName,const EPackFlag vOpenFlag ,const string vMode,bool vInMemory)//打开包
{
	//打开文件
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
		{//可能是只读的原因
			pFile->Close();
			CPackFile::ChangeMode(vrPackName.c_str(),RtFileManager::FP_WRITE);//改变文件属性
			if(!pFile->Open(vrPackName,vMode))
			{//还打不开
                mErr<<"Cannot open package ["<<vrPackName<<"]\n";
				DEL_ONE(pFile)
				return false;
			}
		}
		else
		{//打不开
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



bool CPackManager::PackIsOpen(const string& vrPackName)//包是否存在
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

bool CPackManager::Repear(const string vPackName)			//修复某个包//true修复//false没修复
{
	OpenPack(vPackName,CHECK_PACK,"rb+");
	CPack* pPack=GetPack(vPackName);
	if(pPack==NULL)
	{
		//Err<<"该文件不是包文件，无法修复，删除！";
		DeletePack(vPackName);//无法修复，删除
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

bool CPackManager::ClosePack(CPack* vpPack)					//关闭一个包
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

bool CPackManager::ClosePack(const string& vrPackName)//关闭一个包
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
        if(!OpenPack(vrPackName,NORMAL_PACK,"rb"))//不改写标记
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


bool CPackManager::UnpackFile(const string& vrFileName)						//从所有打开的包中解压一个文件
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

vector<CPack*> CPackManager::GetAllOpenPack()				//得到已经打开的所有的包
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
bool CPackManager::GetAllOpenPack(CPack* vpPack[])                             //得到包中所有文件信息
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
bool CPackManager::CreatePack(const string& vrPackName,bool vInMemory)//bool vInMemory是否放在内存中
{
	//创建文件
	if(PackIsOpen(vrPackName))
	{
		return false;
	}

	CPackFile* pFile=NULL;
	if(vInMemory)
	{//内存文件
		//pFile=RT_NEW CMemoryPackFile;
		CMemoryPackFile* pTemp=RT_NEW CFlagMemoryPackFile;
		pFile=pTemp;
		if(pTemp!=NULL)
		{
			if(!pTemp->Resize(DEFAULT_MEMORY_PACK_SIZE))
			{//如果无法得到要的内存
				DEL_ONE(pFile)
				return false;
			}
		}
	}
	else
	{//正常文件
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


	//写文件标记
	pFile->SetFlag(PACK_FLAG[(int) (WRITE_PACK)],PACK_FLAG [(int) (NORMAL_PACK)],0);
	pFile->Seek(PACK_FLAG_LENGTH+1,SEEK_SET);//文件seek到标记后

	CPack* pPack =RT_NEW CNormalPack;//mPackFactory.GetPack<CNormalPack>();
	if(pPack==NULL)
	{
		DEL_ONE(pFile)
		return false;
	}

	//包的初始化
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
bool CPackManager::ReplacePack(const string& vrMainPack,const string& vrAssistPack)//将辅助包的东西替换到主包
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
bool CPackManager::ReplacePack(CPack* vpMainPack,CPack* vpAssistPack)//将辅助包的东西替换到主包
{
	vector<CPack::SFileInfo*> AllFiles;//得到包中所有文件信息
	vector<CPack::SFileInfo*>::iterator It;
	AllFiles=vpAssistPack->GetAllFileInfo();


	CCompress*			pCompress;
	CPack::SFileInfo*	pFile;//辅助包里的文件信息

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

//从主包中扣除辅助包中已经有的文件
bool CPackManager::DifferPack(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack)
{
	if(!OpenPack(vrMainPack,NORMAL_PACK,"rb"))//不希望改变标志
	{
		return false;
	}

	if(!OpenPack(vrAssistPack,NORMAL_PACK,"rb"))//不希望改变标志
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
{//将vrAssistPack添加到vrMainPack中
	if(!OpenPack(vrMainPack,WRITE_PACK,"rb+"))
	{
		return false;
	}

	if(!OpenPack(vrAssistPack,NORMAL_PACK,"rb"))//不希望改变标志
	{
		return false;
	}

	CPack* pMainPack=GetPack(vrMainPack);
	CPack* pAssistPack=GetPack(vrAssistPack);

	return AddPack(pMainPack,pAssistPack,vReplace,vAllowOld,vNullCompress);
}

const int MD5_LENGTH=16;						//MD5的长度
bool CPackManager::DifferPack(CPack* vpMainPack,CPack* vpAssistPack,CPack* vpDifferPack)
{
	//检查包内容的主版本信息是否相同//内容次版本信息，暂时没有使用
	string MainContentMainVersion;
	string MainContentSubVersion;
	string AssistContentMainVersion;
	string AssistContentSubVersion;

	vpMainPack->GetContentVersion(MainContentMainVersion,MainContentSubVersion);
	vpAssistPack->GetContentVersion(AssistContentMainVersion,AssistContentSubVersion);

	if(MainContentMainVersion!=AssistContentMainVersion)
	{//内容主要版本不同//错误
		return false;
	}
	//差异包的时间,是主包的时间段扣除辅助包的时间//辅助包必须是主包的一部分//并且其中一端必须同主包一样
	long MainStartTime=0;
	long MainEndTime=0;
	long AssistStartTime=0;
	long AssistEndTime=0;

	long DifferStartTime=0;
	long DifferEndTime=0;

	vpMainPack->GetTime(MainStartTime,MainEndTime);
	vpAssistPack->GetTime(AssistStartTime,AssistEndTime);

	if(!(MainStartTime==AssistStartTime || MainEndTime==AssistEndTime))
	{//没有一端是一样的//错误
		return false;
	}

	if(AssistStartTime<MainStartTime)
	{//辅助包已经有部分不是在主包中，错误
		return false;
	}

	if(AssistEndTime>MainEndTime)
	{//辅助包已经有部分不是在主包中，错误
		return false;
	}

	DifferStartTime = (MainStartTime==AssistStartTime) ? AssistEndTime : MainStartTime; 
	DifferEndTime =	(MainStartTime==AssistStartTime) ? MainEndTime : AssistStartTime;


	//差异包的内容主版本//差异包的内容次版本//同主包一样
	vpDifferPack->SetContentVersion(MainContentMainVersion,MainContentSubVersion);
	
	//差异包的内容时间版本
	vpDifferPack->SetTime(DifferStartTime,DifferEndTime);



	//得到一共有的文件的列表
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


	//将文件列表中//辅助包有的文件删除
	const CPack::SFileInfo* pInfo=NULL;
	list<CPack::SFileInfo*>::iterator ListIt;
	for(ListIt=FilesList.begin();ListIt!=FilesList.end();)
	{
		pInfo=vpAssistPack->GetFileInfo((*ListIt)->mName);
		if(pInfo==NULL)
		{//没有该文件//要这个文件
			++ListIt;
		}
		else
		{//有该文件//比对MD5值

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
			{//md5相同
				if (((*ListIt)->mCompressMode==pInfo->mCompressMode) 
					&& ((*ListIt)->mCompressVersion==pInfo->mCompressVersion))
				{//压缩方式和版本都相同//不要这个文件
					ListIt=FilesList.erase(ListIt);
				}
				else
				{//压缩方式和版本有不同//要这个文件
					++ListIt;
				}
			}
			else
			{//md5不同//要这个文件
				++ListIt;
			}
		}
	}

	//将文件写入differ包
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
	//辅助包是否能加入主包//主版本信息是否相同//时间版本的检查
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

	//内容主版本检查
	if(MainContentMainVersion!=AssistContentMainVersion)
	{//主版本不相同//不正确
	//	return false;
	}

	//时间版本检查
	if(MainEndTime < AssistStartTime || AssistEndTime < MainStartTime )
	{//错误//表明两个包没有交集

		return false;
	}

	if(!vAllowOld)
	{//不允许添加旧版本信息
		if(MainEndTime >= AssistEndTime)
		{//主包同辅助包一样新，或更新//没有必要添加
			return false;
		}
	}


	//设置时间//将主包的时间改变
	MainStartTime = ( MainStartTime < AssistStartTime ) ? MainStartTime : AssistStartTime;
	MainEndTime= ( MainEndTime > AssistEndTime ) ? MainEndTime : AssistEndTime;

	vpMainPack->SetTime(MainStartTime,MainEndTime);


	vector<CPack::SFileInfo*> AllFiles;//得到包中所有文件信息
	vector<CPack::SFileInfo*>::iterator It;
	AllFiles=vpAssistPack->GetAllFileInfo();

	CCompress*			pCompress;
	CPack::SFileInfo*	pFile;//辅助包里的文件信息

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


	//将辅助包的结束时间设置为辅助包的结束时间


	vpMainPack->Save();
	
	return true;
}

bool CPackManager::DeletePack(CPack* vpPack)					//删除一个包
{
	return DeletePack(vpPack->Name());
}

bool CPackManager::DeletePack(const string& vrPackName)		//删除一个包
{
	string Name(vrPackName);
	ClosePack(vrPackName);
	return CPackFile::DeleteFile(Name);//包已经关闭，要先复制一下包的名字
}

bool CPackManager::UnpackAllFile(const string& vrPackName, const string& sFliter)
{
	if(PackIsOpen(vrPackName))
	{

	}
	else
	{
		if(!OpenPack(vrPackName,NORMAL_PACK,"rb"))//不改写标记
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


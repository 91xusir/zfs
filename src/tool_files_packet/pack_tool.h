//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	pack_tool.h
//	author:		ldr123
//	purpose:	
//********************************************************************
#ifndef PACK_TOOL_H 
#define PACK_TOOL_H
#include "pack_manager.h"
#include "game_filter.h"

class CPackTool
{
public:
	struct SAttachCompress
	{
		string					mPostfix;
		CPack::ECompressMode	mCompress;
		int						mVersion;
	};

	CPackTool():mpLastPack(NULL),mpManager(NULL),mpCurrentCompressMode(NULL),mFilter()
	{
		mpCurrentCompressMode=Manager()->GetCompress(CPack::NULL_COMPRESS,CNULLCompress::LAST_VERSION);
		//mpCurrentCompressMode=Manager()->GetCompress(CPack::LZW_COMPRESS,CLzwCompress::LAST_VERSION);
	};
	~CPackTool()
	{
		delete mpManager;
	}

	void Clear()
	{
		mpManager->Clear();
	}

	bool Create(const string& vrPackName);
	bool Open(const string& vrPackName,string vMode);

	bool IsPack(const string vPackName);//是否是包文件
	int	 PackVersion(const string vPackName);//包版本
	
	bool GetPackTime(const string vPackName,long& vrStartTime,long& vrEndTime);//得到包内容时间
	bool SetPackTime(const string vPackName,long vStartTime,long vEndTime);//设置包内容时间
	
	bool GetContentVersion(const string vPackName,string& vrMainVersion,string& vrSubVersion);
	bool GetContentVersion(string& vrMainVersion,string& vrSubVersion);
	bool SetContentVersion(string& vrMainVersion,string& vrSubVersion);

	bool Close(const string& vrPackName);
	bool GetAllFilesInPack(vector<CPack::SFileInfo*>& vrFiles);//得到包中所有文件的信息
	bool SetOpertion(const CPack::ECompressMode vMode,const int vVersion);//设置选项
	bool AttachOpertion(const string vFilePostfix,const CPack::ECompressMode vMode,const int vVersion)
	{
		SAttachCompress Temp;
		Temp.mPostfix=vFilePostfix;
		Temp.mCompress=vMode;
		Temp.mVersion=vVersion;
		mAttachCompressList.push_back(Temp);
		return true;
	};//设置某种文件后最对应的压缩格式



	bool CreatePatchPack(const string& vrExeName,const string& vrPackName,const string& vrPatchPackName);
	bool Differ(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack);
	bool Unite(const string& vrMainPack,const string& vrAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress);//合并，并把辅助包删除

	bool Unpack();
	bool Unpack(const string& vrPackName);
	bool UnpackFile(const string& vrPackName, const string& vrFileName);
	bool UnpackFloder(const string& vrPackName, const string& vrFloderInPack);

	bool Pack(const string vFileName,const string vNameInPack,const string& vrPackName,bool vReplace);
	bool Pack(const string vFileName,const string vNameInPack,bool vReplace);

	bool Add(const string vFileName,const string vNameInPack,const string& vrPackName,bool vReplace);
	bool Add(const string vFileName,const string vNameInPack,bool vReplace);

	void UpdateToCipherVersion();			//ac.ma

	CCompress* GetCompressByOpertion(const string vFileName)
	{
		string Postfix=vFileName.substr(vFileName.find_last_of('.')+1);
		list<SAttachCompress>::iterator It;
		for(It=mAttachCompressList.begin();It!=mAttachCompressList.end();++It)
		{
			if(Postfix == It->mPostfix)
			{
				break;
			}
		}

		if(It==mAttachCompressList.end())
		{//没有找到
			return mpCurrentCompressMode;
		}
		else
		{

			return Manager()->GetCompress(It->mCompress,It->mVersion);
		}
	};

	bool Delete(const string& vrNameInPack);

	CPackManager* GetPackManager(){return Manager();};

	void AddExceptFiles(const string vPostfix){mFilter.Add(vPostfix);};
	void DelExceptFiles(const string vPostfix){mFilter.Del(vPostfix);};

private:
	bool PackAllFiles(const string vFileName,const string vNameInPack,CPack* vpPack,bool vReplace);
	
	list<SAttachCompress>	mAttachCompressList;

	CPackManager* Manager();

	CPack*			mpLastPack;
	CPackManager*	mpManager;
	CCompress*		mpCurrentCompressMode;

	CFilter<CFilterEqual>	mFilter;
};
#endif//PACK_TOOL_H

#ifndef  PACK_MANAGER_H
#define PACK_MANAGER_H
#include <vector>
#include <list>

#include "pack_error.h"
#include "pack_factory.h"
#include "compress_mode.h"



class CPackManager
{
public:
	
	enum EPackFlag
	{
		NORMAL_PACK=0,		//正常的包
		WRITE_PACK,			//正在写的包
		READ_PACK,			//正在读的包
		CHECK_PACK,			//正在检查的包
		UNKNOW_PACK			//不认识的包//必须是这些枚举的最后一个
	};

	CPackManager();
	~CPackManager();

	//核心的接口
	bool Init();
	void Clear();
	
	CPack* GetPack(const string& vrPackName);		//得到一个包接口
	bool CreatePack(const string& vrPackName,bool vInMemory);		//创建包
	bool ExistPack(const string& vrPackName);		//是否存在某个包
	bool IsPack(const string vPackName);			//是否是包文件
	bool OpenPack(const string& vrPackName,const EPackFlag vOpenFlag ,const string vMode,bool vInMemory=false);		//打开包
	EPackFlag CheckPack(const string& vrPackName);	//检查一个包

	bool DeletePack(const string& vrPackName);		//删除一个包
	bool DeletePack(CPack* vpPack);					//删除一个包
	bool ClosePack(const string& vrPackName);		//关闭一个包
	bool ClosePack(CPack* vpPack);					//关闭一个包

	bool Repear(const string vPackName);			//修复某个包
	
	//将辅助包的东西添加到主包
	bool AddPack(const string& vrMainPack,const string& vrAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress);
	bool AddPack(CPack* vpMainPack,CPack* vpAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress);//将辅助包的东西添加到主包

	bool DifferPack(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack);//生成一个差异包
	bool DifferPack(CPack* vpMainPack,CPack* vpAssistPack,CPack* vpDifferPack);//生成一个差异包

	
	std::vector<CPack*> GetAllOpenPack();			//得到已经打开的所有的包

	CCompress* GetCompress(const CPack::ECompressMode vMode,const int vVersion);


	//一些不必要的方便性接口
	bool UnpackFile(const string& vrFileName);						//从所有打开的包中解压一个文件
	bool UnpackFile(const string& vrPackName,const string& vrFile);	//从某个包中解压文件
    bool UnpackAllFile(const string& vrPackName);					//解压整个包
	//andy
	bool UnpackAllFile(const string& vrPackName, const string& sFliter);   //解压包中符合过滤器下的所有文件
	bool FileFliter(const string& sFliter, const string& sObjectFileName);//判断文件是否在过滤目录下
private:
	bool OpenPack(CPackFile& vrFile,EPackFlag vOpenFlag);//打开一个包
	bool UnpackFile(CPack* vpPack,const string& vrFile);

	bool PackIsOpen(const string& vrPackName);					//包是否存在
	EPackFlag CheckPackFlag(CPackFile& vrFile);					//检查包的标记

	CPackErrStd									mErr;
	CFactory<CPack,string>						mPackFactory;
	std::list<CPack*>							mPackList;
	CFactory<CCompress,CPack::ECompressMode>	mCompressFactory;
};


//bool WritePackFlag(CPackFile& vrFile,EPackFlag vPackFlag);	//写包的标记
//bool OpenPatchPack(const string& vrPackName);   //打开一个补丁包
//bool UnitePatch(const string& vrMainPack,const string& vrAssistPack);
//bool ReplacePack(const string& vrMainPack,const string& vrAssistPack);//将辅助包的东西替换到主包
//bool ReplacePack(CPack* vpMainPack,CPack* vpAssistPack);//将辅助包的东西替换到主包






#endif//PACK_MANAGER_H

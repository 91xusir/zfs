
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
		NORMAL_PACK=0,		//�����İ�
		WRITE_PACK,			//����д�İ�
		READ_PACK,			//���ڶ��İ�
		CHECK_PACK,			//���ڼ��İ�
		UNKNOW_PACK			//����ʶ�İ�//��������Щö�ٵ����һ��
	};

	CPackManager();
	~CPackManager();

	//���ĵĽӿ�
	bool Init();
	void Clear();
	
	CPack* GetPack(const string& vrPackName);		//�õ�һ�����ӿ�
	bool CreatePack(const string& vrPackName,bool vInMemory);		//������
	bool ExistPack(const string& vrPackName);		//�Ƿ����ĳ����
	bool IsPack(const string vPackName);			//�Ƿ��ǰ��ļ�
	bool OpenPack(const string& vrPackName,const EPackFlag vOpenFlag ,const string vMode,bool vInMemory=false);		//�򿪰�
	EPackFlag CheckPack(const string& vrPackName);	//���һ����

	bool DeletePack(const string& vrPackName);		//ɾ��һ����
	bool DeletePack(CPack* vpPack);					//ɾ��һ����
	bool ClosePack(const string& vrPackName);		//�ر�һ����
	bool ClosePack(CPack* vpPack);					//�ر�һ����

	bool Repear(const string vPackName);			//�޸�ĳ����
	
	//���������Ķ�����ӵ�����
	bool AddPack(const string& vrMainPack,const string& vrAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress);
	bool AddPack(CPack* vpMainPack,CPack* vpAssistPack,bool vReplace,bool vAllowOld,bool vNullCompress);//���������Ķ�����ӵ�����

	bool DifferPack(const string& vrMainPack,const string& vrAssistPack,const string& vrDifferPack);//����һ�������
	bool DifferPack(CPack* vpMainPack,CPack* vpAssistPack,CPack* vpDifferPack);//����һ�������

	
	std::vector<CPack*> GetAllOpenPack();			//�õ��Ѿ��򿪵����еİ�

	CCompress* GetCompress(const CPack::ECompressMode vMode,const int vVersion);


	//һЩ����Ҫ�ķ����Խӿ�
	bool UnpackFile(const string& vrFileName);						//�����д򿪵İ��н�ѹһ���ļ�
	bool UnpackFile(const string& vrPackName,const string& vrFile);	//��ĳ�����н�ѹ�ļ�
    bool UnpackAllFile(const string& vrPackName);					//��ѹ������
	//andy
	bool UnpackAllFile(const string& vrPackName, const string& sFliter);   //��ѹ���з��Ϲ������µ������ļ�
	bool FileFliter(const string& sFliter, const string& sObjectFileName);//�ж��ļ��Ƿ��ڹ���Ŀ¼��
private:
	bool OpenPack(CPackFile& vrFile,EPackFlag vOpenFlag);//��һ����
	bool UnpackFile(CPack* vpPack,const string& vrFile);

	bool PackIsOpen(const string& vrPackName);					//���Ƿ����
	EPackFlag CheckPackFlag(CPackFile& vrFile);					//�����ı��

	CPackErrStd									mErr;
	CFactory<CPack,string>						mPackFactory;
	std::list<CPack*>							mPackList;
	CFactory<CCompress,CPack::ECompressMode>	mCompressFactory;
};


//bool WritePackFlag(CPackFile& vrFile,EPackFlag vPackFlag);	//д���ı��
//bool OpenPatchPack(const string& vrPackName);   //��һ��������
//bool UnitePatch(const string& vrMainPack,const string& vrAssistPack);
//bool ReplacePack(const string& vrMainPack,const string& vrAssistPack);//���������Ķ����滻������
//bool ReplacePack(CPack* vpMainPack,CPack* vpAssistPack);//���������Ķ����滻������






#endif//PACK_MANAGER_H

#ifndef NORMAL_PACK_H
#define NORMAL_PACK_H
#include <map>
#include <list>

#include "game_pack.h"



using std::map;
using std::list;

const char	NORMAL_PACK_TYPE_NAME[]="NORMAL_TYPE";
const int	PACK_TYPE_LENGTH=32;
//const int	NORMAL_PACK_VERSION_100=100;
//const int	NORMAL_PACK_VERSION_101=101;
//const int	NORMAL_PACK_VERSION_102=102;
//const int   LAST_NORMAL_PACK_VERSION=103;//�ɣ���������ʱ��
const int   LAST_NORMAL_PACK_VERSION=104;


extern CThreadCriticalSection gRealReadFile;

struct SNormalPackInfo
{
public:
	SNormalPackInfo()
	:mPackType(NORMAL_PACK_TYPE_NAME),
	mVersion(LAST_NORMAL_PACK_VERSION),				//���汾
	mBlockTableOffset(0),							//�ļ�����ƫ����[����ļ�ͷ]
	mBlockTableSize(0),								//�ļ����Ĵ�С
	mFileCount(0),
	mBlockCount(0),									//�ļ��������
	mStartTime(0),									//�ļ�������ʼʱ��[������]
	mEndTime(0)										//�ļ����Ľ���ʱ��[������]
	{
	};

	string	mPackType;						//������
	int		mVersion;						//���汾//�ṹ��ʾ�汾

	int		mBlockTableOffset;				//�ļ�����ƫ����[����ļ�ͷ]
	int		mBlockTableSize;				//�ļ����Ĵ�С

	size_t	mFileCount;						//�ļ�����
	size_t  mBlockCount;					//���������
	
	//----------------------������Ϣͬ�ļ���һ��д��-----------------------------
	string  mMainContentVersion;	//���ݵ����汾��ʶ
	string  mSubContentVersion;		//���ݵĴ�Ҫ�汾�ı�ʶ
	long    mStartTime;				//���ݵ���ʼʱ��
	long	mEndTime;				//���ݵĽ���ʱ��

	//</---------------------������Ϣͬ�ļ���һ��д��----------------------------->

	//����Ϣд��
	friend CPackFile& operator<<( CPackFile& vrPackFile, SNormalPackInfo& vrPackInfo)
	{
		vrPackFile<<vrPackInfo.mPackType;					//������
		vrPackFile<<LAST_NORMAL_PACK_VERSION;//vrPackInfo.mVersion;					//���汾//����д���µİ汾
		vrPackFile<<vrPackInfo.mBlockTableOffset;			//�ļ�����ƫ����[����ļ�ͷ]
		vrPackFile<<vrPackInfo.mBlockTableSize;				//�ļ����Ĵ�С
		vrPackFile<<vrPackInfo.mFileCount;					//�ļ����Ĵ�С
		vrPackFile<<vrPackInfo.mBlockCount;					//�ļ��������
		return vrPackFile;
	} 

	//����Ϣ����
	friend CPackFile& operator>> (CPackFile& vrPackFile, SNormalPackInfo& vrPackInfo)
	{
		vrPackFile>>vrPackInfo.mPackType;					//������
		vrPackFile>>vrPackInfo.mVersion;					//���汾
		vrPackFile>>vrPackInfo.mBlockTableOffset;			//�ļ�����ƫ����[����ļ�ͷ]
		vrPackFile>>vrPackInfo.mBlockTableSize;				//�ļ����Ĵ�С
		vrPackFile>>vrPackInfo.mFileCount;					//�ļ����Ĵ�С
		vrPackFile>>vrPackInfo.mBlockCount;					//�ļ��������
		return vrPackFile;
	}

};

struct SNormalFileInfo:public CPack::SFileInfo
{
	friend CPackFile& operator<<( CPackFile& vrPackFile, SNormalFileInfo& vrFileInfo)
	{
		return operator <<(vrPackFile,(SFileInfo&)vrFileInfo);
	} 

	friend CPackFile& operator>>( CPackFile& vrPackFile, SNormalFileInfo& vrFileInfo)
	{
		return operator >>(vrPackFile,(SFileInfo&)vrFileInfo);
	} 
	/*friend CDebugInterface& operator << (CDebugInterface& vrDebug,SNormalFileInfo& vrFileInfo)
	{
		return operator <<(vrDebug,(SFileInfo&)vrFileInfo);
	}*/
};


struct SChangingBlockInfo
{
public:
	SChangingBlockInfo():mOffset(0),mSize(0){};
	SChangingBlockInfo(int vOffset,int vSize):mOffset(vOffset),mSize(vSize){};

	int						mOffset;													//�ļ����ƫ����
	int						mSize;														//�ļ���Ĵ�С

	bool operator==(SChangingBlockInfo& vrInfo)
	{
		if(mOffset==vrInfo.mOffset && mSize==vrInfo.mSize)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	size_t SizeInPackFile(CPackFile& vrPackFile)
	{
		size_t Size=0;
		Size+=vrPackFile.SizeInPackFile(this->mOffset);
		Size+=vrPackFile.SizeInPackFile(this->mSize);
		return Size;
	}

	friend CPackFile& operator<<( CPackFile& vrPackFile, SChangingBlockInfo& vrBlockInfo)
	{
		vrPackFile<<vrBlockInfo.mOffset;
		vrPackFile<<vrBlockInfo.mSize;
		return vrPackFile;
	} 

	friend CPackFile& operator >> (CPackFile& vrPackFile,SChangingBlockInfo& vrBlockInfo)
	{
		vrPackFile>>vrBlockInfo.mOffset;
		vrPackFile>>vrBlockInfo.mSize;
		return vrPackFile;
	}

};




class CNormalPack:public CPack
{
	friend class CPackManager;

	template<class TBaseProduct,class TType>
	friend class CFactory;
public:

	struct SBufferData
	{
		string	mName;
		CLinearityBuffer mBuffer;
	};


	//����Ľӿ�
    
	virtual std::vector<SFileInfo*> GetAllFileInfo();									//�õ����������ļ���Ϣ
	virtual const CPack::SFileInfo* GetFileInfo(const string& vrNameInPack);			//�õ��ļ���Ϣ

	virtual bool LoadFile(const string& vrNameInPack,void* vpBuffer);					//�����ļ����ڴ�
	virtual bool LoadFile(const CPack::SFileInfo* vpFileInPack,void* vpBuffer);			//�����ļ����ڴ�

	virtual bool AsyncLoadFile(const CPack::SFileInfo* vpFileInPack);					//�첽����һ���ļ�


	virtual CUnpackFile* GetUnpackFile(const string& vrNameInPack);					//�õ�һ��������ļ�//Ҫɾ��
	virtual CUnpackFile* GetUnpackFile(CPack::SFileInfo* vpFileInPack);				//�õ�һ��������ļ�//Ҫɾ��

	virtual bool DelFile(const string& vrNameInPack);									//ɾ�������ļ�
	virtual bool CheckOverlap(); // Check file and freeblock is overlaped or not.
	
	//��Ӱ����ļ���һ�����ļ�
	virtual bool AddFile(const string& vrNameInPack,	CUnpackFile& vrUnpackFile,
		CCompress* vpCompress, bool vReplace);		

	//��Ӱ����ļ����ļ�
	virtual bool AddFile(const string& vrFileName,		const string& vrNameInPack,
		CCompress* vpCompress, bool vReplace);

	//��Ӱ����ļ����ڴ滺����//���ʱ����-1���õ�ǰʱ��
	virtual bool AddFile(const string& vrNameInPack,	CMemoryBuffer& vrBuffer,
							CCompress* vpCompress, long vTime,bool vReplace);

	//��Ӱ����ļ��������ڴ���//���ʱ����-1���õ�ǰʱ��
	virtual bool AddFile(const string& vrNameInPack,	void* vpBuffer,				int vBufferSize,
							CCompress* vpCompress, long vTime,bool vReplace);
	
	//virtual void SetDirty();						//�ļ���������

	virtual bool Create();			//����һ����
	virtual bool Open();							//��һ���Ѿ����ڵİ�
	virtual	bool Save();				//���������е���Ϣ����
	virtual bool Flush();							//��������Ϣд��

	virtual void GetType(string& vrTypeName,int& vrVersion);		//�õ��������ͺͰ汾

	virtual void GetContentVersion(string& vrMainVersion,string& vrSubVersion);//���������ݵ����ΰ汾��Ϣ
	virtual void SetContentVersion(string& vrMainVersion,string& vrSubVersion);//���ð����ݵ����ΰ汾��Ϣ

	virtual void GetTime(long& vrStartTime,long& vrEndTime);		//�õ�������ʼ�ͽ���ʱ��
	virtual void SetTime(long vStartTime,long vEndTime);			//���ð�����ʼ�ͽ���ʱ��

	virtual bool Repair();//�޸���//true��ʾ�н����޸���false��ʾû�н����޸�
	virtual void RemoveEmptyFile();

	virtual void UpdateToCipherVersion();			//ac.ma
	inline bool rtMD5DataString(char* szData, DWORD iLen, char* szMD5);	//ac.ma


	RT_TYPE_CS						mCriticalSection ;	//�̰߳�ȫ
	list<SBufferData>			mPool;				//�����

	list<string>				mLoading;			//����Ԥ������ļ�

protected:
	CNormalPack();
	virtual ~CNormalPack();

	virtual bool IsType(const string& vrTypeName,int vVersion);		//�ǲ���������ͺͰ汾
	virtual CPack* Clone();											//��¡

	virtual void OnInit();											//�ļ�����ʼ��

	virtual bool BeginWrite(int vRequestSize,SFileInfo* vpFileInfo);//��ʼд
	virtual int  Write(const void* vpData,const int vDataSize);		//д
	virtual void EndWrite();										//����д
	
	virtual bool OnceWrite(CMemoryBuffer& vrBuffer,SFileInfo* vpFileInfo);	//һ��д
	virtual bool OnceWrite(const void* vpData,const int vDataSize,
		SFileInfo* vpFileInfo);												//һ��д

	virtual bool OnceRead(void* vpData,const SFileInfo* vpFileInfo);		//һ�ζ���

private:
	void Dirty();									//�����ļ���Ϊ��
	
	const long GetEnd();							//�õ�����β
	void SetEnd(const long vEnd);					//���ð���β

	SChangingBlockInfo GetBlock(int vBlockSize);	//�õ������
	void FreeBlock(int vOffset,int vSize,bool vDirty=true);//�ͷſ���� vDirtyָ�Ƿ����ļ���Ϊdirty
	void FreeReserveBlock(int vOffset,int vSize);	//�ͷű����Ŀ����

	bool ReadFileTable();							//���ļ���
	void WriteFileTable();							//д�ļ���

	bool								mDirty;		//����
	long								mBegin;		//������ʼλ��
	long								mEnd;		//���Ľ���λ��
	bool								mAllowWrite;//����д����

	SNormalPackInfo						mInfo;		//��ǰ�İ���Ϣ
	map<string,SNormalFileInfo*>		mFileTable;	//��ǰ���ļ���
	list<SChangingBlockInfo>			mFreeBlock;	//��ǰ���յ��ļ���
	list<SChangingBlockInfo>			mReserveFreeBlock;//�����Ŀ��ļ���[���ܱ�ʹ��]

};



//virtual size_t GetFileCount();                           //�õ����ļ�����
//virtual bool GetAllFileInfo(SFileInfo* vpFileInfo[]);                             //�õ����������ļ���Ϣ

/*
//�û������ļ����ļ�
virtual bool ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
CCompress* vpCompress);	

//�û������ļ����ڴ��//���ʱ����-1���õ�ǰʱ��
virtual bool ReplaceFile(const string& vrNameInPack, void* vpBuffer,				int vBufferSize,
CCompress* vpCompress,long vTime);
*/




#endif//NORMAL_PACK_H


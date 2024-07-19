#ifndef GAME_PACK_H
#define GAME_PACK_H

#include <vector>
#include <string>

#include "pack_file.h"
#include "memory_buffer.h"
//#include "game_string_assistant.h"
//#include "compress_mode.h"
#include "pack_factory.h"



using std::string;

class CUnpackFile;
class CCompress;
class CPack;


//���Ľӿ�
class CPack
{
	friend class CPackManager;
	
	
	template<class TBaseProduct,class TType>
	friend class CFactory;

	//friend class CCompress;
	friend class CNULLCompress;
	
	template<class T>
	friend class CDeriveCompress;

public:
	enum ECompressMode
	{
		UNKNOWN_COMPRESS=0,
		NULL_COMPRESS=1,
		LZW_COMPRESS=2,
		ZLIB_COMPRESS=3,
	};



	struct SFileInfo
	{
	public:
		SFileInfo():mName(),mOffset(0),mSizeInPack(0),mOriginSize(0),
						mCompressMode(NULL_COMPRESS),mCompressVersion(0),mWriteTime(-1)
		{
			CM_MEMPROTECTOR(mMD5, 16)
		}
		SFileInfo(const SFileInfo &object)
		{
			CM_MEMPROTECTOR(mMD5, 16)
			mName = object.mName;
			mOffset = object.mOffset;
			mSizeInPack = object.mSizeInPack;
			mOriginSize = object.mOriginSize;
			mCompressMode = object.mCompressMode;
			mCompressVersion = object.mCompressVersion;
			mWriteTime = object.mWriteTime;
			rt2_strncpy(mMD5, object.mMD5, 16);
		}
		~SFileInfo(){CM_MEMUNPROTECTOR(mMD5)}

		string			mName;				//�ļ���
		int				mOffset;			//�ļ���ƫ����
		int				mSizeInPack;		//�ļ��ڰ��еĴ�С
		int				mOriginSize;		//�ļ�ԭ���Ĵ�С
		ECompressMode	mCompressMode;		//ѹ����ʽ
		int				mCompressVersion;	//ѹ����ʽ�İ汾
		long			mWriteTime;			//�ļ����޸�ʱ��
		//char			mMD5[16];			//ѹ��ǰ�ģ�ԭʼ���ݵģ�MD5ֵ
		CM_MEMDEF(mMD5, 16)
		

	/*	friend CDebugInterface& operator << (CDebugInterface& vrDebug,SFileInfo& vrFileInfo)
		{
			vrDebug<<"["<<vrFileInfo.mName<<"]" << "[Offset:" << vrFileInfo.mOffset<<"]"
				<< "[SizeInPack:"<< vrFileInfo.mSizeInPack <<"] [OriginSize:"<< vrFileInfo.mOriginSize << "]"
				<< "[CompressMode:" << vrFileInfo.mCompressMode << "] [CompressVersion:"<< vrFileInfo.mCompressVersion << "] "
				<< "[WriteTime:" << fox::TimeConvert(vrFileInfo.mWriteTime) << "]";
			
			char Temp[16*2+1];
			rtMD5PrintString(Temp,(unsigned char*)vrFileInfo.mMD5);
			vrDebug<<"[MD5:]["<<Temp<<"]";
			return vrDebug;
		}*/


		size_t SizeInPackFile(CPackFile& vrPackFile)
		{
			size_t Size;
			Size=vrPackFile.SizeInPackFile(this->mName);
			Size+=vrPackFile.SizeInPackFile(this->mOffset);
			Size+=vrPackFile.SizeInPackFile(this->mSizeInPack);
			Size+=vrPackFile.SizeInPackFile(this->mOriginSize);
			Size+=vrPackFile.SizeInPackFile(this->mCompressMode);
			Size+=vrPackFile.SizeInPackFile(this->mCompressVersion);
			Size+=vrPackFile.SizeInPackFile(this->mWriteTime);
			Size+=16;//MD5_LENTH
			return Size;
		}

		friend CPackFile& operator<<( CPackFile& vrPackFile, SFileInfo& vrFileInfo)
		{
			vrPackFile<<vrFileInfo.mName;
			vrPackFile<<vrFileInfo.mOffset;
			vrPackFile<<vrFileInfo.mSizeInPack;
			vrPackFile<<vrFileInfo.mOriginSize;
			vrPackFile<<vrFileInfo.mCompressMode;
			vrPackFile<<vrFileInfo.mCompressVersion;
			vrPackFile<<vrFileInfo.mWriteTime;
			vrPackFile.PutBuffer(vrFileInfo.mMD5,16);
			return vrPackFile;
		} 

		friend CPackFile& operator>>( CPackFile& vrPackFile, SFileInfo& vrFileInfo)
		{
			vrPackFile>>vrFileInfo.mName;
			vrPackFile>>vrFileInfo.mOffset;
			vrPackFile>>vrFileInfo.mSizeInPack;
			vrPackFile>>vrFileInfo.mOriginSize;
			vrPackFile>>vrFileInfo.mCompressMode;
			vrPackFile>>vrFileInfo.mCompressVersion;
			vrPackFile>>vrFileInfo.mWriteTime;
			vrPackFile.GetBuffer(16,vrFileInfo.mMD5);
			return vrPackFile;
		} 
	};


public:
	virtual std::vector<SFileInfo*> GetAllFileInfo()=0;									//�õ����������ļ���Ϣ
	virtual const SFileInfo* GetFileInfo(const string& vrNameInPack)=0;					//�õ��ļ���Ϣ

	virtual bool LoadFile(const string& vrNameInPack,			void* vpBuffer)=0;		//�����ļ����ڴ�
	virtual bool LoadFile(const CPack::SFileInfo* vpFileInPack,	void* vpBuffer)=0;		//�����ļ����ڴ�
	virtual bool AsyncLoadFile(const CPack::SFileInfo* vpFileInPack)	=0;				//�첽����һ���ļ�
	
	
	virtual CUnpackFile* GetUnpackFile(const string& vrNameInPack)=0;					//�õ�һ��������ļ�//Ҫɾ��
	virtual CUnpackFile* GetUnpackFile(CPack::SFileInfo* vpFileInPack)=0;				//�õ�һ��������ļ�//Ҫɾ��

	virtual bool DelFile(const string& vrNameInPack)=0;									//ɾ�������ļ�
	
	//��Ӱ����ļ���һ�����ļ�
	virtual bool AddFile(const string& vrNameInPack,	CUnpackFile& vrUnpackFile,
							CCompress* vpCompress,bool vReplace)=0;		
	
	//��Ӱ����ļ����ļ�
	virtual bool AddFile(const string& vrFileName,		const string& vrNameInPack,
							CCompress* vpCompress,bool vReplace)=0;
	
	//��Ӱ����ļ����ڴ滺����//���ʱ����-1���õ�ǰʱ��
	virtual bool AddFile(const string& vrNameInPack,	CMemoryBuffer& vrBuffer,	
							CCompress* vpCompress,long vTime,bool vReplace)=0;
	
	//��Ӱ����ļ��������ڴ���//���ʱ����-1���õ�ǰʱ��
	virtual bool AddFile(const string& vrNameInPack,	void* vpBuffer,				int vBufferSize,
							CCompress* vpCompress,long vTime,bool vReplace)=0;
	

	virtual void UpdateToCipherVersion()=0;				//ac.ma

	virtual bool Create()=0;							//����һ����
	virtual bool Open()=0;								//��һ���Ѿ����ڵİ�
	virtual	bool Save()=0;								//���������е���Ϣ����
	virtual bool Flush()=0;								//��������Ϣд��


	virtual const string& Name();									//�õ���������
	virtual void GetType(string& vrTypeName,int& vrVersion)=0;		//�õ��������ͺͰ汾

	virtual void GetContentVersion(string& vrMainVersion,string& vrSubVersion)=0;//���������ݵ����ΰ汾��Ϣ
	virtual void SetContentVersion(string& vrMainVersion,string& vrSubVersion)=0;//���ð����ݵ����ΰ汾��Ϣ

	virtual void GetTime(long& vrStartTime,long& vrEndTime)=0;		//�õ�������ʼ�ͽ���ʱ��
	virtual void SetTime(long vStartTime,long vEndTime)=0;			//���ð�����ʼ�ͽ���ʱ��

	virtual bool Repair()=0;//�޸���//true��ʾ�н����޸���false��ʾû�н����޸�
	virtual bool CheckOverlap() { return true; }; // Check file and freeblock is overlaped or not.
	virtual void RemoveEmptyFile() {};

	CPackFile*	mpPackFile;

	CFactory<CCompress,CPack::ECompressMode>*		mpCompressFactory;			//ѹ����ʽ�Ĺ���
protected:
	CPack():mpPackFile(NULL),mErr(){}
	virtual ~CPack();

	long PackSize()
	{
		if(mpPackFile==NULL)
		{
			return 0;
		}
		else
		{
			return mpPackFile->Size();
		}
	};
	bool Init(CPackFile* vpPackFile,CFactory<CCompress,CPack::ECompressMode>* vpCompressFactory);											//���ĳ�ʼ��

	virtual bool IsType(const string& vrTypeName,int vVersion)=0;				//�ǲ���������ͺͰ汾
	virtual CPack* Clone()=0;													//��¡

	virtual void OnInit()=0;													//�ļ�����ʼ��

	
	virtual bool BeginWrite(int vRequestSize,SFileInfo* vpFileInfo)=0;			//��ʼд
	virtual int  Write(const void* vpData,const int vDataSize)=0;				//д
	virtual void EndWrite()=0;													//����д

	virtual bool OnceWrite(CMemoryBuffer& vrBuffer,SFileInfo* vpFileInfo)=0;	//һ��д
	virtual bool OnceWrite(const void* vpData,const int vDataSize,
							SFileInfo* vpFileInfo)=0;							//һ��д

	virtual bool OnceRead(void* vpData,const SFileInfo* vpFileInfo)=0;			//һ�ζ���

	CPackErrStd	mErr;
	
	
};









/*
virtual bool AsyncLoadFile(const string vFileInPack)					//�첽����һ���ļ�
{
SThreadParameter* p=RT_NEW  SThreadParameter;
p->mpPack=this;
p->mFileInPack=vFileInPack;

CREATE_THREAD(ThreadFunc,p);
}
*/

//ת�ӿ�
//const SFileInfo* GetFileInfo(const char* vrNameInPack);					
//bool DelFile(const char* vpNameInPack);
//bool AddFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress);
//bool AddFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,int vRTReadSize);
//bool ReplaceFile(const char* vpNameInPack);
//bool ReplaceFile(const char* vpFileName,const char* vpNameInPack,CCompress* vpCompress);
//bool ReplaceFile(const char* vpNameInPack,void* vpBuffer,int vBufferSize,CCompress* vpCompress);

//�û������ļ����ļ�
//virtual bool ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
//						CCompress* vpCompress)=0;	

//�û������ļ����ڴ��//���ʱ����-1���õ�ǰʱ��
//virtual bool ReplaceFile(const string& vpNameInPack,	void* vpBuffer,				int vBufferSize,
//						CCompress* vpCompress,long vTime)=0;


#endif//GAME_PACK_H

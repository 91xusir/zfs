
#include "filepack/normal_pack.h"
#include "core/rt2_core.h"  //"md5.h"
//#include "game_string_assistant.h"
#include "filepack/unpack_file.h"
#include "filepack/compress_mode.h"

#include "filepack/pack_manager.h"
//using update_client::Debug;
//using fox::CMD5;
#include <time.h>

#ifdef WIN32
#define pthread_t int
#endif

CThreadCriticalSection gRealReadFile;  //��Ӵ��̶��ļ����̰߳�ȫ

struct SThreadParameter {
    CNormalPack*            mpPack;
    const CPack::SFileInfo* mpFileInfo;
};

const char DEFAULT_CONTENT_MAIN_VERSION[] = "ZFS[CN]";  //�ɰ汾��û���������汾��Ϣ�����ֵ
const char DEFAULT_CONTENT_SUB_VERSION[] = "";  //�ɰ汾��û�����ݴΰ汾��Ϣ���������ֵ

const int POOL_MAX_SIZE = 100 * 1024 * 1024;  //�������ص��������//100M
int       gDebugInfoThreadCount = 0;

//DWORD ThreadFunc(LPVOID vpParameter)
void* ThreadFunc(void* vpParameter) {
    gDebugInfoThreadCount++;
    //game_client::Debug<<"�߳̿�ʼ["<<rtMilliseconds()<<"][" << gDebugInfoThreadCount << "]\n";

    SThreadParameter* p = (SThreadParameter*)vpParameter;

    //game_client::Debug<<"�����ļ�[" << p->mpFileInfo->mName << "]["<<rtMilliseconds()<<"]\n";
    CCompress* pCompress = p->mpPack->mpCompressFactory->GetReference(
        p->mpFileInfo->mCompressMode, p->mpFileInfo->mCompressVersion);

    CNormalPack::SBufferData Temp;
    if (!Temp.mBuffer.Resize(p->mpFileInfo->mOriginSize)) {
        gDebugInfoThreadCount--;
        DEL_ONE(p);
        return 0;
    }

    //��ѹ��
    pCompress->UnCompress(*(p->mpPack), Temp.mBuffer.Ptr(), p->mpFileInfo);
    Temp.mName = p->mpFileInfo->mName;
    //game_client::Debug<<"�����ļ�����["<< p->mpFileInfo->mName << "]["<<rtMilliseconds()<<"]\n";

    //�����Ѿ�����ĳ���
    RT_LOCK_CS(&(p->mpPack->mCriticalSection));
    //����ش�С
    static int PoolSize = 0;
    PoolSize += Temp.mBuffer.Size();

    //�ͷŵ��ջ���С��POOL_MAX_SIZE
    for (; PoolSize >= POOL_MAX_SIZE && (p->mpPack->mPool.size() != 0);) {
        //�ŵ���ǰ���
        PoolSize -= p->mpPack->mPool.front().mBuffer.Size();
        p->mpPack->mPool.pop_front();
    }

    p->mpPack->mPool.push_back(Temp);

    //������Ԥ����Ķ������Ƴ�
    list<string>::iterator It;
    for (It = p->mpPack->mLoading.begin(); It != p->mpPack->mLoading.end(); ++It) {
        if ((*It) == p->mpFileInfo->mName) {
            p->mpPack->mLoading.erase(It);
            break;
        }
    }

    RT_UNLOCK_CS(&(p->mpPack->mCriticalSection));
    gDebugInfoThreadCount--;
    //	game_client::Debug<<"�߳̽���["<<rtMilliseconds()<<"][" << gDebugInfoThreadCount <<"]\n";

    DEL_ONE(p);
    return 0;
}

CNormalPack::CNormalPack()
    : mDirty(false), mBegin(0), mEnd(0), mAllowWrite(false), mInfo(), mFileTable(), mFreeBlock() {
    RT_INIT_CS(&mCriticalSection);
}

CNormalPack::~CNormalPack() {
    Save();

    //�ͷ��ļ����е��ļ���Ϣ
    map<string, SNormalFileInfo*>::iterator It;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        DEL_ONE((It->second));
    }

    RT_DELETE_CS(&mCriticalSection);
}

void CNormalPack::OnInit()  //�ļ�����ʼ��
{
    mBegin = mpPackFile->Tell();  //Ҫ��֤�ڳ�ʼ��ǰ���ļ�λ�������ڰ�ͷ
}

void CNormalPack::GetType(string& vrTypeName, int& vrVersion)  //�õ��������ͺͰ汾
{
    vrTypeName = mInfo.mPackType;  //������;
    vrVersion = mInfo.mVersion;    //���汾
    return;
}

void CNormalPack::GetContentVersion(string& vrMainVersion,
                                    string& vrSubVersion)  //���������ݵ����ΰ汾��Ϣ
{

    vrMainVersion = mInfo.mMainContentVersion;
    vrSubVersion = mInfo.mSubContentVersion;
    return;
}

void CNormalPack::SetContentVersion(string& vrMainVersion,
                                    string& vrSubVersion)  //���ð����ݵ����ΰ汾��Ϣ
{
    Dirty();  //�б䶯������Ϊ��
    mInfo.mMainContentVersion = vrMainVersion;
    mInfo.mSubContentVersion = vrSubVersion;
    //Save();
    return;
}

void CNormalPack::GetTime(long& vrStartTime, long& vrEndTime)  //�õ�������ʼ�ͽ���ʱ��
{
    vrStartTime = mInfo.mStartTime;
    vrEndTime = mInfo.mEndTime;
    return;
}

//-1��ʾ�õ�ǰʱ��
void CNormalPack::SetTime(long vStartTime, long vEndTime)  //���ð�����ʼ�ͽ���ʱ��
{
    Dirty();  //�б䶯������Ϊ��
    if (vStartTime == -1) {
        time_t Now;
        time(&Now);
        mInfo.mStartTime = Now;
    } else {
        mInfo.mStartTime = vStartTime;
    }

    if (vEndTime == -1) {
        time_t Now;
        time(&Now);
        mInfo.mEndTime = Now;
    } else {
        mInfo.mEndTime = vEndTime;
    }
    return;
}

bool CNormalPack::IsType(const string& vrTypeName, int vVersion) {
    if (vrTypeName == NORMAL_PACK_TYPE_NAME) {
        switch (vVersion) {
            case 103:
            case 104:
            case LAST_NORMAL_PACK_VERSION:
                return true;
                break;
            default:
                break;
        }
    }
    return false;
}

CPack* CNormalPack::Clone() {
    return (RT_NEW CNormalPack);
}

using std::vector;

vector<CPack::SFileInfo*> CNormalPack::GetAllFileInfo() {
    vector<SFileInfo*>                      AllFiles;
    map<string, SNormalFileInfo*>::iterator It;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        AllFiles.push_back(It->second);
    }

    return AllFiles;
}

const CPack::SFileInfo* CNormalPack::GetFileInfo(const string& vrNameInPack)  //�õ��ļ���Ϣ
{
    map<string, SNormalFileInfo*>::iterator It;
    It = mFileTable.find(vrNameInPack);

    if (It == mFileTable.end()) {
        return NULL;
    } else {
        return It->second;
    }
}

const int DEFAULT_BUFFER_SIZE_4K = 4 * 1024;
const int MD5_LENGTH = 16;

bool CNormalPack::Repair()  //�޸���//true��ʾ�б仯��false��ʾû�б仯
{
    bool                                    IsChange = false;
    char                                    Temp[DEFAULT_BUFFER_SIZE_4K];  //��ʱ�ڴ�
    map<string, SNormalFileInfo*>::iterator It;

    CUnpackFile* pFile = NULL;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        pFile = GetUnpackFile(It->second);

        //����MD5ֵ
        RtMD5 NewMD5;
        /*RtMD5 OldMD5;
		OldMD5.Update()
		OldMD5(( unsigned char  * const) (It->second->mMD5)  );*/

        size_t ReadSize = pFile->GetBuffer(DEFAULT_BUFFER_SIZE_4K, Temp);  //��������
        for (; ReadSize != 0;) {
            NewMD5.Update((unsigned char*)Temp, (unsigned int)ReadSize);  //����MD5
            ReadSize = pFile->GetBuffer(DEFAULT_BUFFER_SIZE_4K, Temp);    //������������
        }

        DEL_ONE(pFile);
        pFile = NULL;

        NewMD5.GetMD5((unsigned char*)(It->second->mMD5));
        //-		if(NewMD5.GetMD5(NULL) == (unsigned char * const)(It->second->mMD5))
        //		{//һ��
        //		}
        //		else
        //		{//��ͬ
        //			IsChange=true;
        //			Dirty();//�б䶯������Ϊ��
        //			char* const p=It->second->mMD5;
        //
        //			for(int i=0;i<16;++i)
        //			{
        //				p[i]=(NewMD5.GetMD5(NULL))[i];
        //			}
        //		}
    }

    return IsChange;
}

struct TEMP_BLOCK_INFO {
    TEMP_BLOCK_INFO(long o, long s, void* p, string name)
        : lOffset(o), lSize(s), vp(p), strName(name){};
    long   lOffset;
    long   lSize;
    void*  vp;
    string strName;
};

class TEMP_BLOCK_INFO_OP {
   public:
    bool operator()(const TEMP_BLOCK_INFO& t1, const TEMP_BLOCK_INFO& t2) {
        return t1.lOffset < t2.lOffset;
    }
};

void CNormalPack::RemoveEmptyFile() {
    std::list<std::string>                  collEmptyFile;
    map<string, SNormalFileInfo*>::iterator It;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        SNormalFileInfo* pFileInfo = It->second;
        if (pFileInfo->mSizeInPack <= 0) {
            collEmptyFile.push_back(pFileInfo->mName);
        }
    }

    std::list<std::string>::iterator emptyIt = collEmptyFile.begin();
    for (; emptyIt != collEmptyFile.end(); ++emptyIt) {
        this->DelFile(*emptyIt);
    }
}

bool CNormalPack::CheckOverlap() {
    std::vector<TEMP_BLOCK_INFO>            collBlocks;
    map<string, SNormalFileInfo*>::iterator It;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        SNormalFileInfo* pFileInfo = It->second;
        if (pFileInfo->mSizeInPack > 0)
            collBlocks.push_back(TEMP_BLOCK_INFO(pFileInfo->mOffset, pFileInfo->mSizeInPack,
                                                 (void*)pFileInfo, pFileInfo->mName));
    }

    list<SChangingBlockInfo>::iterator freeit;
    for (freeit = mFreeBlock.begin(); freeit != mFreeBlock.end(); ++freeit) {
        SChangingBlockInfo& blockInfo = *freeit;
        if (blockInfo.mSize > 0)
            collBlocks.push_back(
                TEMP_BLOCK_INFO(blockInfo.mOffset, blockInfo.mSize, (void*)&blockInfo, ""));
    }

    std::vector<TEMP_BLOCK_INFO>::iterator tpiit;
    std::vector<int>                       collOverlapped;
    std::sort(collBlocks.begin(), collBlocks.end(), TEMP_BLOCK_INFO_OP());
    long lLast = 0;
    int  iCount = 0;
    for (tpiit = collBlocks.begin(); tpiit != collBlocks.end(); ++tpiit) {
        TEMP_BLOCK_INFO& tpi = *tpiit;
        long             mylast = tpi.lOffset + tpi.lSize;
        if (tpi.lOffset < lLast) {
            // Overlapped!!!!
            tpi.lOffset = lLast;  // Change offset
            tpi.lSize = mylast - tpi.lOffset;
            //RtCoreLog().Error("Blcok overlapped, block name is: %s\n", tpi.strName.c_str());
            // Push current and last block to need fix list
            if (iCount > 0)
                collOverlapped.push_back(iCount - 1);
            collOverlapped.push_back(iCount);
        }
        if (mylast > lLast)
            lLast = mylast;
        ++iCount;
    }

    if (collOverlapped.size() > 0) {
        RtCoreLog().Error("\n\nTotal %d blocks overlapped.\n\n", collOverlapped.size());
        Dirty();
    }

    std::vector<int>::iterator overlappedIt;
    for (overlappedIt = collOverlapped.begin(); overlappedIt != collOverlapped.end();
         ++overlappedIt) {
        int              iCount = *overlappedIt;
        TEMP_BLOCK_INFO& tpi = collBlocks[iCount];
        RtCoreLog().Error("Blcok overlapped, block start at %d\t name is: %s\n", tpi.lOffset,
                          tpi.strName.c_str());
        if (tpi.strName.empty()) {
            // Handle free block
            for (freeit = mFreeBlock.begin(); freeit != mFreeBlock.end();) {
                SChangingBlockInfo& blockInfo = *freeit;
                if ((void*)(&blockInfo) == tpi.vp) {
                    if (tpi.lSize <= 0) {
                        freeit = mFreeBlock.erase(freeit);
                        continue;
                    } else {
                        blockInfo.mOffset = tpi.lOffset;
                        blockInfo.mSize = tpi.lSize;
                    }
                }
                ++freeit;
            }
        } else {
            // Handle file block
            It = mFileTable.find(tpi.strName);
            if (It != mFileTable.end()) {
                SNormalFileInfo* pFileInfo = It->second;
                if (tpi.lSize <= 0) {
                    DEL_ONE(pFileInfo);
                    mFileTable.erase(It);
                } else {
                    pFileInfo->mOffset = tpi.lOffset;
                    pFileInfo->mSizeInPack = tpi.lSize;
                    rt2_strcpy(pFileInfo->mMD5, "0000");
                    RtCoreLog().Error("New md5 is:%s\n", pFileInfo->mMD5);
                }
            }
        }
    }

    if (collOverlapped.size() > 0) {
        Save();
        return false;
    } else
        return true;
}

CUnpackFile* CNormalPack::GetUnpackFile(const string& vrNameInPack)  //�õ�һ��������ļ�//Ҫɾ��
{
    CPack::SFileInfo* pFileInfo = NULL;
    pFileInfo = (CPack::SFileInfo*)GetFileInfo(vrNameInPack);
    if (pFileInfo == NULL) {
        return false;
    }

    return GetUnpackFile(pFileInfo);
}

CUnpackFile*
CNormalPack::GetUnpackFile(CPack::SFileInfo* vpFileInPack)  //�õ�һ��������ļ�//Ҫɾ��
{
    //��黺������Ƿ��Ѿ��и��ļ�
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //û���ҵ�
        //�õ�ѹ����ʽ
        //		game_client::Debug<<"�Ӵ��̶����ļ�[" <<vpFileInPack->mName<<"]\n";
        RT_UNLOCK_CS(&mCriticalSection);
        CCompress* pCompress = mpCompressFactory->GetReference(vpFileInPack->mCompressMode,
                                                               vpFileInPack->mCompressVersion);

        //��ѹ��
        return pCompress->UnCompress(*this, vpFileInPack);
    } else {  //�ҵ�
        //game_client::Debug<<"��Ԥ�����ļ��еõ��ļ�[" << vpFileInPack->mName <<"]\n";
        CUnpackFile* p = RT_NEW CMemoryUnpackFile((*It).mBuffer, vpFileInPack);
        mPool.erase(It);
        RT_UNLOCK_CS(&mCriticalSection);
        return p;
    }
}

bool CNormalPack::LoadFile(const string& vrNameInPack, void* vpBuffer)  //�����ļ����ڴ�
{
    const CPack::SFileInfo* pFileInfo = NULL;
    pFileInfo = GetFileInfo(vrNameInPack);
    if (pFileInfo == NULL) {
        return false;
    }

    return LoadFile(pFileInfo, vpBuffer);
}

bool CNormalPack::LoadFile(const CPack::SFileInfo* vpFileInPack, void* vpBuffer)  //�����ļ����ڴ�
{

    //��黺������Ƿ��Ѿ��и��ļ�
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //û���ҵ�
        //�õ�ѹ����ʽ
        RT_UNLOCK_CS(&mCriticalSection);
        CCompress* pCompress = mpCompressFactory->GetReference(vpFileInPack->mCompressMode,
                                                               vpFileInPack->mCompressVersion);

        //��ѹ��
        pCompress->UnCompress(*this, vpBuffer, vpFileInPack);

    } else {  //�ҵ�
        memcpy(vpBuffer, (*It).mBuffer.Ptr(), vpFileInPack->mOriginSize);
        RT_UNLOCK_CS(&mCriticalSection);
    }

    return true;
}

bool CNormalPack::AsyncLoadFile(const CPack::SFileInfo* vpFileInPack)  //�첽����һ���ļ�
{

    //game_client::Log<<"�첽�����ļ�["<<rtMilliseconds()<<"]\n";
    //���Ѿ������list
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //û�ҵ�

        //��Ԥ�����list
        list<string>::iterator It1;
        for (It1 = mLoading.begin(); It1 != mLoading.end(); ++It1) {
            if ((*It1) == vpFileInPack->mName) {
                break;
            }
        }

        if (It1 == mLoading.end()) {                  //û���ҵ�
            mLoading.push_back(vpFileInPack->mName);  //����Ԥ����Ķ���
            RT_UNLOCK_CS(&mCriticalSection);
        } else {  //�ҵ�
            RT_UNLOCK_CS(&mCriticalSection);
            return true;
        }

        //��һ���߳�����
        SThreadParameter* p = RT_NEW SThreadParameter;
        p->mpFileInfo = vpFileInPack;
        p->mpPack = this;
        std::cout << "Ԥ�������ļ�[" << vpFileInPack->mName << "][" << rtMilliseconds() << "]\n";
        pthread_t _pthreadid = 0;
        RT_CREATE_THREAD(&ThreadFunc, p);
        std::cout << "�첽�����ļ�����[" << rtMilliseconds() << "]" << std::endl;
        return true;
    } else {  //�ҵ�
        RT_UNLOCK_CS(&mCriticalSection);
        std::cout << "�첽�����ļ�����[" << rtMilliseconds() << "]\n";
        return true;
    }
}

bool CNormalPack::DelFile(const string& vrNameInPack)  //ɾ�������ļ�
{
    map<string, SNormalFileInfo*>::iterator It;
    It = mFileTable.find(vrNameInPack);

    if (It == mFileTable.end()) {
        return false;
    }

    SNormalFileInfo* p = It->second;
    //�ͷŸ��ļ����ļ��ռ�
    FreeBlock(p->mOffset, p->mSizeInPack);
    //�ͷŸ��ļ���Ϣ�ڴ�
    DEL_ONE(p);
    //���ļ������Ƴ�
    mFileTable.erase(It);
    return true;
}

//const int DEFAULT_BUFFER_SIZE_4K=4*1024;
bool CNormalPack::AddFile(const string& vrNameInPack, CUnpackFile& vrUnpackFile,
                          CCompress* vpCompress, bool vReplace)  //��Ӱ����ļ���һ�����ļ�
{
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //�Ѿ�����
            return false;
        }
    }

    //�����µ��ļ���Ϣ
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;                   //�������
    pFileInfo->mWriteTime = vrUnpackFile.WriteTime();  //���ʱ��

    if (!vpCompress->Compress(*this, vrUnpackFile, pFileInfo))  //������ѹ���ã��������Ӧ��Ϣ
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //���ļ���Ϣ��ӵ��ļ�����
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

bool CNormalPack::AddFile(const string& vrFileName, const string& vrNameInPack,
                          CCompress* vpCompress, bool vReplace)  //��Ӱ����ļ����ļ�
{
    string Temp = ".\\";
    Temp = Temp + Name();
    if (vrFileName == Name() || vrFileName == Temp) {
        mErr << "Can't add self!"
             << "\n";
        return false;
    }

    CAnsiPackFile TempFile;

    if (!TempFile.Open(vrFileName, "rb")) {
        return false;
    }

    long  FileSize = TempFile.Size();
    char* pTempBuffer = RT_NEW char[FileSize];
    if (pTempBuffer == NULL) {
        return false;
    }

    if (FileSize != (long)TempFile.GetBuffer(FileSize, pTempBuffer)) {
        mErr << "read file err!\n";
    }

    AddFile(vrNameInPack, pTempBuffer, FileSize, vpCompress, TempFile.WriteTime(), vReplace);
    DEL_ONE(pTempBuffer);
    return true;
}

//��Ӱ����ļ����ڴ滺����
bool CNormalPack::AddFile(const string& vrNameInPack, CMemoryBuffer& vrBuffer,
                          CCompress* vpCompress, long vTime, bool vReplace) {
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //�Ѿ�����
            return false;
        }
    }

    //�����µ��ļ���Ϣ
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;  //�������
    //���ʱ��//vTime=-1�õ�ǰʱ��
    if (vTime == -1) {
        time_t Now;
        time(&Now);
        pFileInfo->mWriteTime = Now;

    } else {
        pFileInfo->mWriteTime = vTime;
    }

    if (!vpCompress->Compress(*this, vrBuffer, pFileInfo))  //������ѹ���ã��������Ӧ��Ϣ
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //���ļ���Ϣ��ӵ��ļ�����
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

//��Ӱ����ļ��������ڴ���
bool CNormalPack::AddFile(const string& vrNameInPack, void* vpBuffer, int vBufferSize,
                          CCompress* vpCompress, long vTime, bool vReplace) {
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //�Ѿ�����
            return false;
        }
    }

    //�����µ��ļ���Ϣ
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;  //�������
    //���ʱ��//vTime=-1�õ�ǰʱ��
    if (vTime == -1) {
        time_t Now;
        time(&Now);
        pFileInfo->mWriteTime = Now;
    } else {
        pFileInfo->mWriteTime = vTime;
    }

    if (!vpCompress->Compress(*this, vpBuffer, vBufferSize,
                              pFileInfo))  //������ѹ���ã��������Ӧ��Ϣ
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //���ļ���Ϣ��ӵ��ļ�����
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

bool CNormalPack::Create()  //����һ����
{
    mDirty = true;  //����

    //д����Ϣ
    (*mpPackFile) << mInfo;

    SetEnd(mpPackFile->Tell());
    return true;
}

bool CNormalPack::Open()  //��һ���Ѿ����ڵİ�
{
    //��������Ϣ
    (*mpPackFile) >> mInfo;
    SetEnd(mpPackFile->Size());  //�ļ�β
    return ReadFileTable();
}

bool CNormalPack::Flush()  //��������Ϣд��
{
    mpPackFile->Flush();
    return true;
}

bool CNormalPack::Save()  //���������е���Ϣ����
{
    if (!mDirty) {
        return true;
    }

    //���������ͷ�
    list<SChangingBlockInfo>::iterator It;
    for (It = mReserveFreeBlock.begin(); It != mReserveFreeBlock.end(); ++It) {
        FreeBlock((*It).mOffset, (*It).mSize);
    }

    //д�ļ���Ϳ����
    //	Debug<<"д���ļ���ʼ\n";
    WriteFileTable();
    //	Debug<<"д���ļ������\n";
    mDirty = false;

    //д����Ϣ
    mpPackFile->Seek(mBegin, SEEK_SET);
    (*mpPackFile) << mInfo;
    Flush();

    return true;
}

void CNormalPack::Dirty() {
    if (!mDirty) {
        //������������
        mDirty = true;
        //�ͷž��ļ���//���µ��ļ���д��ǰ�������������ͷ�
        FreeReserveBlock(mInfo.mBlockTableOffset, mInfo.mBlockTableSize);
    }
    return;
}

const long CNormalPack::GetEnd()  //�õ�����β
{
    return mEnd;
}

void CNormalPack::SetEnd(const long vEnd) {
    mEnd = vEnd;
    return;
}

SChangingBlockInfo CNormalPack::GetBlock(int vBlockSize)  //�õ������
{
    Dirty();  //�б䶯������Ϊ��

    //ȡ����С�ķ��ϵĿ��п�
    list<SChangingBlockInfo>::iterator It;
    //list<SChangingBlockInfo>::iterator It1;

    SChangingBlockInfo Block;
    bool               bDoatEnd = true;
    for (It = mFreeBlock.begin(); It != mFreeBlock.end(); It++) {
        if ((*It).mSize >= vBlockSize) {
            Block = (*It);
            mFreeBlock.erase(It);
            bDoatEnd = false;
            break;
        }
    }

    if (bDoatEnd) {  //���û�з��ϵĿ��п飬�ʹӰ�β�õ�
        Block.mOffset = GetEnd();
        Block.mSize = vBlockSize;
        SetEnd(GetEnd() + vBlockSize);  //�ı����β
    } else {                            //�еõ����п�
        //��Ҫ�Ѷ���Ŀռ��ͷŵ�
        if (Block.mSize - vBlockSize > 0) {  //�ռ��ж�
            FreeBlock(Block.mOffset + vBlockSize, Block.mSize - vBlockSize);
            Block.mSize = vBlockSize;
        }
    }

    //	Debug<<"�õ��ռ� Offset[" <<Block.mOffset << "]     Size["<< Block.mSize << "]\n";
    return Block;
}

void CNormalPack::FreeReserveBlock(int vOffset, int vSize)  //�ͷű����Ŀ����
{
    if (vSize == 0) {
        return;
    }
    Dirty();  //�б䶯������Ϊ��
    SChangingBlockInfo Block(vOffset, vSize);
    mReserveFreeBlock.push_back(Block);  //�����б�
    return;
}

void CNormalPack::FreeBlock(int vOffset, int vSize, bool vDirty)  //�ͷſ����
{
    if (vSize == 0) {
        return;
    }

    if (vDirty) {
        Dirty();  //�б䶯������Ϊ��
    }

    int Offset = vOffset;
    int Size = vSize;

    //���ÿ�ͬ�Ѿ��еĿ����ϲ�
    list<SChangingBlockInfo>::iterator It;

    for (It = mFreeBlock.begin(); It != mFreeBlock.end();) {
        if (Offset + Size == It->mOffset) {
            Size += It->mSize;  //���ÿ�ĺ�һ����õ�
            It = mFreeBlock.erase(It);
        } else if (Offset == It->mOffset + It->mSize) {
            Offset = It->mOffset;
            Size += It->mSize;  //���ÿ��ǰһ����õ�
            It = mFreeBlock.erase(It);
        } else {
            It++;
        }

        if (Offset != vOffset && (Offset + Size) != vOffset + vSize) {
            break;
        }
    }

    SChangingBlockInfo Block(Offset, Size);

    //Ŀǰ���������С
    //if(Offset+Size==GetEnd())
    //{//�ǰ�β���ı��β
    //	SetEnd(Offset);
    //}
    //else
    //��������б�
    for (It = mFreeBlock.begin(); It != mFreeBlock.end(); ++It) {
        if (It->mSize >= Size) {
            break;
        }
    }
    mFreeBlock.insert(It, Block);

    return;
}

void CNormalPack::UpdateToCipherVersion()  //ac.ma
{
    if (mInfo.mVersion >= LAST_NORMAL_PACK_VERSION) {
        //MessageBox(0,"���ļ���Ϊ���ܰ汾������ʧ��","��Ϣ��ʾ",MB_OK);
        return;
    } else
        mInfo.mVersion = LAST_NORMAL_PACK_VERSION;
    Dirty();
    Save();
}

bool CNormalPack::rtMD5DataString(char* szData, DWORD iLen, char* szMD5)  //ac.ma
{
    if (iLen < 16)
        return false;
    RtMD5 rtMD5;
    rtMD5.EncodeData(szData, iLen);
    char buff[33];
    memset(buff, 0, 33);
    rtMD5.GetMD5String(buff);
    memcpy(szMD5, buff, 16);
    return true;
}

void CNormalPack::WriteFileTable()  //д�ļ���
{
    //�����ļ����С

    //�ļ�����Ϣ��С
    size_t Size = 0;

    //104�汾��һЩ����Ϣ����������//����һ�ֲ���
    Size += mpPackFile->SizeInPackFile(mInfo.mMainContentVersion);  //���ݵ���Ҫ�汾��Ϣ�Ĵ�С
    Size += mpPackFile->SizeInPackFile(mInfo.mSubContentVersion);  //���ݵĴ�Ҫ�汾��Ϣ�Ĵ�С
    Size += mpPackFile->SizeInPackFile(mInfo.mStartTime);  //������ʼʱ����Ϣ�Ĵ�С
    Size += mpPackFile->SizeInPackFile(mInfo.mEndTime);    //���ݽ���ʱ����Ϣ�Ĵ�С

    map<string, SNormalFileInfo*>::iterator MapIt;
    for (MapIt = mFileTable.begin(); MapIt != mFileTable.end(); ++MapIt) {
        Size += MapIt->second->SizeInPackFile(*mpPackFile);
    }

    //�������Ϣ��С
    list<SChangingBlockInfo>::iterator ListIt;  //��ǰ���յ��ļ���
    for (ListIt = mFreeBlock.begin(); ListIt != mFreeBlock.end(); ++ListIt) {
        Size += ListIt->SizeInPackFile(*mpPackFile);
    }

    //�õ���ǰ���ļ����С
    //[����Ǵ��ļ���β�õĿռ䣬���б�Ĵ�С����]
    //[����Ǵ��б����õĿռ䣬����ʣ�࣬���б�ռ䲻��]
    //[����Ǵ��б����õĿռ䣬��С�պã����б�ռ��С]
    //���ԣ�ֻ��С
    SChangingBlockInfo Block = GetBlock((int)Size);

    //���ļ�д��
    mpPackFile->Seek(Block.mOffset, SEEK_SET);

    //�õ�����Ϣ
    mInfo.mBlockTableOffset = Block.mOffset;  //�ļ�����ƫ����[����ļ�ͷ]
    mInfo.mBlockTableSize = Block.mSize;      //�ļ����Ĵ�С

    mInfo.mFileCount = mFileTable.size();   //�ļ�����
    mInfo.mBlockCount = mFreeBlock.size();  //���������

    //д�ļ�����Ϣ
    // Debug<<"д�ļ�����Ϣ��ʼ\n";

    (*mpPackFile) << mInfo.mMainContentVersion;  //���ݵ���Ҫ�汾��Ϣ
    (*mpPackFile) << mInfo.mSubContentVersion;   //���ݵĴ�Ҫ�汾��Ϣ
    (*mpPackFile) << mInfo.mStartTime;           //������ʼʱ����Ϣ
    (*mpPackFile) << mInfo.mEndTime;             //���ݽ���ʱ����Ϣ

    for (MapIt = mFileTable.begin(); MapIt != mFileTable.end(); ++MapIt) {
        (*mpPackFile) << (*(MapIt->second));
        //Debug<< "�����ļ���Ϣд��[" << (*(MapIt->second)) << "]\n";
    }
    //Debug<<"д�ļ�����Ϣ����\n";

    //д�������Ϣ
    //Debug<<"д�������Ϣд�뿪ʼ\n";
    for (ListIt = mFreeBlock.begin(); ListIt != mFreeBlock.end(); ++ListIt) {
        (*mpPackFile) << (*ListIt);
        //Debug<<"�������Ϣ Offset [ " <<ListIt->mOffset <<  " ]    Size ["  << ListIt->mSize <<"]\n";
    }
    //Debug<<"д�������Ϣд�����\n";

    return;
}

bool CNormalPack::ReadFileTable()  //���ļ���
{
    //��λ
    mpPackFile->Seek(mInfo.mBlockTableOffset, SEEK_SET);

    //�����ļ�����Ϣ
    size_t           i;
    SNormalFileInfo* pFileInfo = NULL;

    //���ְ���Ϣ//����ʱ����Ϣ
    switch (mInfo.mVersion) {
        case 103:
            mInfo.mMainContentVersion = DEFAULT_CONTENT_MAIN_VERSION;
            mInfo.mSubContentVersion = DEFAULT_CONTENT_SUB_VERSION;
            mInfo.mStartTime = 0;
            mInfo.mEndTime = 0;
            break;
        case 104:
            (*mpPackFile) >> mInfo.mMainContentVersion;
            (*mpPackFile) >> mInfo.mSubContentVersion;
            (*mpPackFile) >> mInfo.mStartTime;
            (*mpPackFile) >> mInfo.mEndTime;
            break;
        case LAST_NORMAL_PACK_VERSION:
            (*mpPackFile) >> mInfo.mMainContentVersion;
            (*mpPackFile) >> mInfo.mSubContentVersion;
            (*mpPackFile) >> mInfo.mStartTime;
            (*mpPackFile) >> mInfo.mEndTime;
            mpPackFile->Seek(8, SEEK_CUR);
            break;
        default:
            break;
    }

    //Debug<<"�����ļ�����Ϣ��ʼ\n";
    for (i = 0; i < mInfo.mFileCount; i++) {
        pFileInfo = RT_NEW SNormalFileInfo;
        if (pFileInfo == NULL) {
            return false;
        }

        (*mpPackFile) >> (*pFileInfo);

        //Debug<< "[" << (*pFileInfo) << "]\n";

        //���ļ���Ϣ��ӵ��ļ�����
        mFileTable.insert(map<string, SNormalFileInfo*>::value_type(pFileInfo->mName, pFileInfo));
    }
    //	Debug<<"�����ļ�����Ϣ����\n";

    //�����������Ϣ

    //	Debug<<"�����������Ϣ��ʼ\n";
    SChangingBlockInfo BlockInfo;
    for (i = 0; i < mInfo.mBlockCount; i++) {
        (*mpPackFile) >> BlockInfo;
        //���������Ϣ��ӵ��б���
        FreeBlock(BlockInfo.mOffset, BlockInfo.mSize, false);

        //		Debug<<"Offset[ " << BlockInfo.mOffset << "]   Size["  << BlockInfo.mSize << "]\n";
    }
    //	Debug<<"�����������Ϣ����\n";

    return true;
}

bool CNormalPack::BeginWrite(int vRequestSize, SFileInfo* vpFileInfo)  //��ʼд
{
    //�õ�Ҫ���ļ��ռ�
    SChangingBlockInfo Block = GetBlock(vRequestSize);

    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //��ָ��λ��
    mAllowWrite = true;
    return true;

    /*
	//���ڴ滺��д��
	if(!mpPackFile->PutBuffer(Temp))
	{
		return false;
	}
	*/
}

int CNormalPack::Write(const void* vpData, const int vDataSize)  //д
{
    if (!mAllowWrite) {
        return 0;
    }

    return (int)mpPackFile->PutBuffer(vpData, vDataSize);
}

void CNormalPack::EndWrite()  //����д
{
    mAllowWrite = false;
    return;
}

bool CNormalPack::OnceWrite(CMemoryBuffer& vrBuffer, SFileInfo* vpFileInfo) {
    //�õ�Ҫ���ļ��ռ�
    SChangingBlockInfo Block = GetBlock(vrBuffer.DataSize());

    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //��ָ��λ��
    return mpPackFile->PutBuffer(vrBuffer);
}

bool CNormalPack::OnceWrite(const void* vpData, const int vDataSize,
                            SFileInfo* vpFileInfo)  //һ��д
{
    //�õ�Ҫ���ļ��ռ�
    SChangingBlockInfo Block = GetBlock(vDataSize);
    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //��ָ��λ��

    if ((const int)mpPackFile->PutBuffer(vpData, vDataSize) == vDataSize) {
        return true;
    } else {
        return false;
    }
}

bool CNormalPack::OnceRead(void* vpData, const SFileInfo* vpFileInfo)  //һ�ζ���
{
    gRealReadFile.Lock();
    mpPackFile->Seek(vpFileInfo->mOffset, SEEK_SET);  //��ָ��λ��

    if (mpPackFile->GetBuffer(vpFileInfo->mSizeInPack, vpData) == vpFileInfo->mSizeInPack) {
        gRealReadFile.UnLock();
        return true;
    } else {
        gRealReadFile.UnLock();
        return false;
    }
}

/*
bool CNormalPack::AddFile(const string& vrNameInPack,CUnpackFile& vrUnpackFile,
CCompress* vpCompress)			//��Ӱ����ļ���һ�����ļ�
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//�Ѿ�����
return false;
}

//�ļ�ѹ��
static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static CMemoryBuffer Temp;
Temp.ClearData();//�������

//��������//���õ�MD5ֵ//�õ�
RtMD5Context context;
rtMD5Init (&context);

size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,TempBuffer);//��������
for(;ReadSize!=0;)
{
if(vpCompress->Compress(TempBuffer,ReadSize))
{
//������Ϣ
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, (int)ReadSize);//����MD5
ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,TempBuffer);//������������
}


//�õ�Ҫ���ļ��ռ�
SChangingBlockInfo Block=GetBlock(Temp.DataSize());

//���ڴ滺��д��
mpPackFile->Seek(Block.mOffset,SEEK_SET);
if(!mpPackFile->PutBuffer(Temp))
{
return false;
}


//�����µ��ļ���Ϣ
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;

pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSizeInPack=Block.mSize;
pFileInfo->mOriginSize=(int)vrUnpackFile.Size();
pFileInfo->mCompressMode=vpCompress->GetMode();
pFileInfo->mCompressVersion=vpCompress->GetVersion();
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);


//���ļ���Ϣ��ӵ��ļ�����
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));
return true;
}
*/

/*
void CNormalPack::WriteFileTable()				//д�ļ���
{
mpPackFile->Seek(GetEnd(),SEEK_SET);		//�õ���β

//�õ�����Ϣ
mInfo.mBlockTableOffset=GetEnd();			//�ļ�����ƫ����[����ļ�ͷ]


mInfo.mFileCount=mFileTable.size();			//�ļ�����
mInfo.mBlockCount=mFreeBlock.size();		//���������


//д�ļ�����Ϣ
map<string,SNormalFileInfo*>::iterator MapIt;		
for(MapIt=mFileTable.begin();MapIt!=mFileTable.end();++MapIt)
{
(*mpPackFile)<<(*(MapIt->second));
Debug<< "�����ļ���Ϣд��[" << (*(MapIt->second)) << "]\n";
}

//д�������Ϣ
list<SChangingBlockInfo>::iterator ListIt;	//��ǰ���յ��ļ���
for(ListIt=mFreeBlock.begin();ListIt!=mFreeBlock.end();++ListIt)
{
(*mpPackFile)<<(*ListIt);
}


//
SetEnd(mpPackFile->Tell());
mInfo.mBlockTableSize=mEnd-mInfo.mBlockTableOffset;			//�ļ����Ĵ�С


return ;
}
*/

/*
bool CNormalPack::AddFile(const string& vrNameInPack,void* vpBuffer,int vBufferSize,int vRTReadSize)	//��Ӱ����ļ����ڴ�
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//�Ѿ�����
return false;
}

//�ļ�ѹ��
static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2+4];//4����Ϊ������һ���ֽ�˵�����ݵĳ���
int					CompressSize;
static CMemoryBuffer Temp;
Temp.ClearData();//�������



//��������//���õ�MD5ֵ
RtMD5Context context;
rtMD5Init (&context);


int ReadSize=vrBuffer.Read(pData);//��������
for(;ReadSize!=0;)
{
CompressSize=vpCompress->Compress(TempBuffer,(unsigned int)ReadSize,TempCompress);//ѹ��

//д����ʱ�ڴ�
if(!Temp.Write(TempCompress,CompressSize))
{
//������Ϣ
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, ReadSize);//����MD5
ReadSize=vrBuffer.Read(pData);
}


//�õ�Ҫ���ļ��ռ�
SChangingBlockInfo Block=GetBlock(Temp->DataSize());


//���ڴ滺��д��
mpPackFile->Seek(Block.mOffset,SEEK_SET);

if(!mpPackFile->PutBuffer(Temp))
{
return false;
}










//�õ�Ҫ���ļ��ռ�
SChangingBlockInfo Block=GetBlock(vBufferSize);
//���ļ�д��
mpPackFile->Seek(Block.mOffset,SEEK_SET);
if(mpPackFile->PutBuffer(vpBuffer,vBufferSize)!=vBufferSize)
{
return false;
}






//�����µ��ļ���Ϣ
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;
pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSize=Block.mSize;
pFileInfo->mRTSize=vRTReadSize;
pFileInfo->
pFileInfo->

//�õ�MD5ֵ
RtMD5Context context;
rtMD5Init (&context);
rtMD5Update (&context,(unsigned char*) vpBuffer, vBufferSize);
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);
//MDPrint((unsigned char*)pFileInfo->mMD5);



//���ļ���Ϣ��ӵ��ļ�����
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));

return true;
}
*/

/*
//�û������ļ����ļ�
bool CNormalPack::ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
CCompress* vpCompress)
{
DelFile(vrNameInPack);
return AddFile(vrFileName,vrNameInPack,vpCompress);
}
*/

/*
//�û������ļ����ڴ��
bool CNormalPack::ReplaceFile(const string& vrNameInPack,	void* vpBuffer,		int vBufferSize,
CCompress* vpCompress,long vTime)
{
DelFile(vrNameInPack);
return AddFile(vrNameInPack,vpBuffer,vBufferSize,vpCompress,vTime);  // (vrNameInPack,vpBuffer,vBufferSize,vRTReadSize);
}
*/

/*
//��Ӱ����ļ����ڴ滺����
bool CNormalPack::AddFile(const string& vrNameInPack,CMemoryBuffer& vrBuffer,CCompress* vpCompress)
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//�Ѿ�����
return false;
}

//�ļ�ѹ��



static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2+4];//4����Ϊ������һ���ֽ�˵�����ݵĳ���
int					CompressSize;
static CMemoryBuffer Temp;
Temp.ClearData();//�������



//��������//���õ�MD5ֵ
RtMD5Context context;
rtMD5Init (&context);


int ReadSize=vrBuffer.Read(pData);//��������
for(;ReadSize!=0;)
{
CompressSize=vpCompress->Compress(TempBuffer,(unsigned int)ReadSize,TempCompress);//ѹ��

//д����ʱ�ڴ�
if(!Temp.Write(TempCompress,CompressSize))
{
//������Ϣ
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, ReadSize);//����MD5
ReadSize=vrBuffer.Read(pData);
}


//�õ�Ҫ���ļ��ռ�
SChangingBlockInfo Block=GetBlock(Temp->DataSize());


//���ڴ滺��д��
mpPackFile->Seek(Block.mOffset,SEEK_SET);

if(!mpPackFile->PutBuffer(Temp))
{
return false;
}

//�����µ��ļ���Ϣ
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;

pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSizeInPack=Block.mSize;
pFileInfo->mOriginSize=vrBuffer.DataSize();
pFileInfo->mCompressMode=vpCompress->GetMode();
pFileInfo->mCompressVersion=vpCompress->GetVersion();
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);


//���ļ���Ϣ��ӵ��ļ�����
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));

return true;
}
*/

/*
size_t  CNormalPack::GetFileCount()//�õ����ļ�����
{
return mFileTable.size();
}

bool CNormalPack::GetAllFileInfo(SFileInfo* vpFileInfo[])                             //�õ����������ļ���Ϣ
{
map<string,SNormalFileInfo*>::iterator  It;
int i=0;
for(It=mFileTable.begin();It!=mFileTable.end();++It)
{
vpFileInfo[i++]=It->second;
}

return true;
}
*/

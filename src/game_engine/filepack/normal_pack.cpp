
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

CThreadCriticalSection gRealReadFile;  //真从磁盘读文件的线程安全

struct SThreadParameter {
    CNormalPack*            mpPack;
    const CPack::SFileInfo* mpFileInfo;
};

const char DEFAULT_CONTENT_MAIN_VERSION[] = "ZFS[CN]";  //旧版本中没有内容主版本信息用这个值
const char DEFAULT_CONTENT_SUB_VERSION[] = "";  //旧版本中没有内容次版本信息，就用这个值

const int POOL_MAX_SIZE = 100 * 1024 * 1024;  //最多允许池的最大容量//100M
int       gDebugInfoThreadCount = 0;

//DWORD ThreadFunc(LPVOID vpParameter)
void* ThreadFunc(void* vpParameter) {
    gDebugInfoThreadCount++;
    //game_client::Debug<<"线程开始["<<rtMilliseconds()<<"][" << gDebugInfoThreadCount << "]\n";

    SThreadParameter* p = (SThreadParameter*)vpParameter;

    //game_client::Debug<<"读入文件[" << p->mpFileInfo->mName << "]["<<rtMilliseconds()<<"]\n";
    CCompress* pCompress = p->mpPack->mpCompressFactory->GetReference(
        p->mpFileInfo->mCompressMode, p->mpFileInfo->mCompressVersion);

    CNormalPack::SBufferData Temp;
    if (!Temp.mBuffer.Resize(p->mpFileInfo->mOriginSize)) {
        gDebugInfoThreadCount--;
        DEL_ONE(p);
        return 0;
    }

    //解压缩
    pCompress->UnCompress(*(p->mpPack), Temp.mBuffer.Ptr(), p->mpFileInfo);
    Temp.mName = p->mpFileInfo->mName;
    //game_client::Debug<<"结束文件读入["<< p->mpFileInfo->mName << "]["<<rtMilliseconds()<<"]\n";

    //加入已经载入的池中
    RT_LOCK_CS(&(p->mpPack->mCriticalSection));
    //计算池大小
    static int PoolSize = 0;
    PoolSize += Temp.mBuffer.Size();

    //释放到空或者小于POOL_MAX_SIZE
    for (; PoolSize >= POOL_MAX_SIZE && (p->mpPack->mPool.size() != 0);) {
        //放掉最前面的
        PoolSize -= p->mpPack->mPool.front().mBuffer.Size();
        p->mpPack->mPool.pop_front();
    }

    p->mpPack->mPool.push_back(Temp);

    //将它从预载入的队列中移除
    list<string>::iterator It;
    for (It = p->mpPack->mLoading.begin(); It != p->mpPack->mLoading.end(); ++It) {
        if ((*It) == p->mpFileInfo->mName) {
            p->mpPack->mLoading.erase(It);
            break;
        }
    }

    RT_UNLOCK_CS(&(p->mpPack->mCriticalSection));
    gDebugInfoThreadCount--;
    //	game_client::Debug<<"线程结束["<<rtMilliseconds()<<"][" << gDebugInfoThreadCount <<"]\n";

    DEL_ONE(p);
    return 0;
}

CNormalPack::CNormalPack()
    : mDirty(false), mBegin(0), mEnd(0), mAllowWrite(false), mInfo(), mFileTable(), mFreeBlock() {
    RT_INIT_CS(&mCriticalSection);
}

CNormalPack::~CNormalPack() {
    Save();

    //释放文件表中的文件信息
    map<string, SNormalFileInfo*>::iterator It;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        DEL_ONE((It->second));
    }

    RT_DELETE_CS(&mCriticalSection);
}

void CNormalPack::OnInit()  //文件包初始化
{
    mBegin = mpPackFile->Tell();  //要保证在初始化前，文件位置正好在包头
}

void CNormalPack::GetType(string& vrTypeName, int& vrVersion)  //得到包的类型和版本
{
    vrTypeName = mInfo.mPackType;  //包类型;
    vrVersion = mInfo.mVersion;    //包版本
    return;
}

void CNormalPack::GetContentVersion(string& vrMainVersion,
                                    string& vrSubVersion)  //读出包内容的主次版本信息
{

    vrMainVersion = mInfo.mMainContentVersion;
    vrSubVersion = mInfo.mSubContentVersion;
    return;
}

void CNormalPack::SetContentVersion(string& vrMainVersion,
                                    string& vrSubVersion)  //设置包内容的主次版本信息
{
    Dirty();  //有变动，设置为脏
    mInfo.mMainContentVersion = vrMainVersion;
    mInfo.mSubContentVersion = vrSubVersion;
    //Save();
    return;
}

void CNormalPack::GetTime(long& vrStartTime, long& vrEndTime)  //得到包的起始和结束时间
{
    vrStartTime = mInfo.mStartTime;
    vrEndTime = mInfo.mEndTime;
    return;
}

//-1表示用当前时间
void CNormalPack::SetTime(long vStartTime, long vEndTime)  //设置包的起始和结束时间
{
    Dirty();  //有变动，设置为脏
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

const CPack::SFileInfo* CNormalPack::GetFileInfo(const string& vrNameInPack)  //得到文件信息
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

bool CNormalPack::Repair()  //修复包//true表示有变化，false表示没有变化
{
    bool                                    IsChange = false;
    char                                    Temp[DEFAULT_BUFFER_SIZE_4K];  //临时内存
    map<string, SNormalFileInfo*>::iterator It;

    CUnpackFile* pFile = NULL;
    for (It = mFileTable.begin(); It != mFileTable.end(); ++It) {
        pFile = GetUnpackFile(It->second);

        //计算MD5值
        RtMD5 NewMD5;
        /*RtMD5 OldMD5;
		OldMD5.Update()
		OldMD5(( unsigned char  * const) (It->second->mMD5)  );*/

        size_t ReadSize = pFile->GetBuffer(DEFAULT_BUFFER_SIZE_4K, Temp);  //读出数据
        for (; ReadSize != 0;) {
            NewMD5.Update((unsigned char*)Temp, (unsigned int)ReadSize);  //计算MD5
            ReadSize = pFile->GetBuffer(DEFAULT_BUFFER_SIZE_4K, Temp);    //继续读出数据
        }

        DEL_ONE(pFile);
        pFile = NULL;

        NewMD5.GetMD5((unsigned char*)(It->second->mMD5));
        //-		if(NewMD5.GetMD5(NULL) == (unsigned char * const)(It->second->mMD5))
        //		{//一样
        //		}
        //		else
        //		{//不同
        //			IsChange=true;
        //			Dirty();//有变动，设置为脏
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

CUnpackFile* CNormalPack::GetUnpackFile(const string& vrNameInPack)  //得到一个不打包文件//要删除
{
    CPack::SFileInfo* pFileInfo = NULL;
    pFileInfo = (CPack::SFileInfo*)GetFileInfo(vrNameInPack);
    if (pFileInfo == NULL) {
        return false;
    }

    return GetUnpackFile(pFileInfo);
}

CUnpackFile*
CNormalPack::GetUnpackFile(CPack::SFileInfo* vpFileInPack)  //得到一个不打包文件//要删除
{
    //检查缓冲池中是否已经有该文件
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //没有找到
        //得到压缩方式
        //		game_client::Debug<<"从磁盘读入文件[" <<vpFileInPack->mName<<"]\n";
        RT_UNLOCK_CS(&mCriticalSection);
        CCompress* pCompress = mpCompressFactory->GetReference(vpFileInPack->mCompressMode,
                                                               vpFileInPack->mCompressVersion);

        //解压缩
        return pCompress->UnCompress(*this, vpFileInPack);
    } else {  //找到
        //game_client::Debug<<"从预载入文件中得到文件[" << vpFileInPack->mName <<"]\n";
        CUnpackFile* p = RT_NEW CMemoryUnpackFile((*It).mBuffer, vpFileInPack);
        mPool.erase(It);
        RT_UNLOCK_CS(&mCriticalSection);
        return p;
    }
}

bool CNormalPack::LoadFile(const string& vrNameInPack, void* vpBuffer)  //载入文件到内存
{
    const CPack::SFileInfo* pFileInfo = NULL;
    pFileInfo = GetFileInfo(vrNameInPack);
    if (pFileInfo == NULL) {
        return false;
    }

    return LoadFile(pFileInfo, vpBuffer);
}

bool CNormalPack::LoadFile(const CPack::SFileInfo* vpFileInPack, void* vpBuffer)  //载入文件到内存
{

    //检查缓冲池中是否已经有该文件
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //没有找到
        //得到压缩方式
        RT_UNLOCK_CS(&mCriticalSection);
        CCompress* pCompress = mpCompressFactory->GetReference(vpFileInPack->mCompressMode,
                                                               vpFileInPack->mCompressVersion);

        //解压缩
        pCompress->UnCompress(*this, vpBuffer, vpFileInPack);

    } else {  //找到
        memcpy(vpBuffer, (*It).mBuffer.Ptr(), vpFileInPack->mOriginSize);
        RT_UNLOCK_CS(&mCriticalSection);
    }

    return true;
}

bool CNormalPack::AsyncLoadFile(const CPack::SFileInfo* vpFileInPack)  //异步载入一个文件
{

    //game_client::Log<<"异步载入文件["<<rtMilliseconds()<<"]\n";
    //找已经载入的list
    RT_LOCK_CS(&mCriticalSection);
    list<SBufferData>::iterator It;
    for (It = mPool.begin(); It != mPool.end(); ++It) {
        if ((*It).mName == vpFileInPack->mName) {
            break;
        }
    }

    if (It == mPool.end()) {  //没找到

        //找预载入的list
        list<string>::iterator It1;
        for (It1 = mLoading.begin(); It1 != mLoading.end(); ++It1) {
            if ((*It1) == vpFileInPack->mName) {
                break;
            }
        }

        if (It1 == mLoading.end()) {                  //没有找到
            mLoading.push_back(vpFileInPack->mName);  //加入预载入的队列
            RT_UNLOCK_CS(&mCriticalSection);
        } else {  //找到
            RT_UNLOCK_CS(&mCriticalSection);
            return true;
        }

        //开一个线程载入
        SThreadParameter* p = RT_NEW SThreadParameter;
        p->mpFileInfo = vpFileInPack;
        p->mpPack = this;
        std::cout << "预先载入文件[" << vpFileInPack->mName << "][" << rtMilliseconds() << "]\n";
        pthread_t _pthreadid = 0;
        RT_CREATE_THREAD(&ThreadFunc, p);
        std::cout << "异步载入文件结束[" << rtMilliseconds() << "]" << std::endl;
        return true;
    } else {  //找到
        RT_UNLOCK_CS(&mCriticalSection);
        std::cout << "异步载入文件结束[" << rtMilliseconds() << "]\n";
        return true;
    }
}

bool CNormalPack::DelFile(const string& vrNameInPack)  //删除包中文件
{
    map<string, SNormalFileInfo*>::iterator It;
    It = mFileTable.find(vrNameInPack);

    if (It == mFileTable.end()) {
        return false;
    }

    SNormalFileInfo* p = It->second;
    //释放该文件的文件空间
    FreeBlock(p->mOffset, p->mSizeInPack);
    //释放该文件信息内存
    DEL_ONE(p);
    //从文件表中移除
    mFileTable.erase(It);
    return true;
}

//const int DEFAULT_BUFFER_SIZE_4K=4*1024;
bool CNormalPack::AddFile(const string& vrNameInPack, CUnpackFile& vrUnpackFile,
                          CCompress* vpCompress, bool vReplace)  //添加包中文件从一个包文件
{
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //已经存在
            return false;
        }
    }

    //创建新的文件信息
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;                   //填好名字
    pFileInfo->mWriteTime = vrUnpackFile.WriteTime();  //填好时间

    if (!vpCompress->Compress(*this, vrUnpackFile, pFileInfo))  //将内容压缩好，并填充相应信息
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //将文件信息添加到文件表中
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

bool CNormalPack::AddFile(const string& vrFileName, const string& vrNameInPack,
                          CCompress* vpCompress, bool vReplace)  //添加包中文件从文件
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

//添加包中文件从内存缓存中
bool CNormalPack::AddFile(const string& vrNameInPack, CMemoryBuffer& vrBuffer,
                          CCompress* vpCompress, long vTime, bool vReplace) {
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //已经存在
            return false;
        }
    }

    //创建新的文件信息
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;  //填好名字
    //填好时间//vTime=-1用当前时间
    if (vTime == -1) {
        time_t Now;
        time(&Now);
        pFileInfo->mWriteTime = Now;

    } else {
        pFileInfo->mWriteTime = vTime;
    }

    if (!vpCompress->Compress(*this, vrBuffer, pFileInfo))  //将内容压缩好，并填充相应信息
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //将文件信息添加到文件表中
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

//添加包中文件从整块内存中
bool CNormalPack::AddFile(const string& vrNameInPack, void* vpBuffer, int vBufferSize,
                          CCompress* vpCompress, long vTime, bool vReplace) {
    if (vReplace) {
        DelFile(vrNameInPack);
    } else {
        if (GetFileInfo(vrNameInPack) != NULL) {  //已经存在
            return false;
        }
    }

    //创建新的文件信息
    SNormalFileInfo* pFileInfo = RT_NEW SNormalFileInfo;

    pFileInfo->mName = vrNameInPack;  //填好名字
    //填好时间//vTime=-1用当前时间
    if (vTime == -1) {
        time_t Now;
        time(&Now);
        pFileInfo->mWriteTime = Now;
    } else {
        pFileInfo->mWriteTime = vTime;
    }

    if (!vpCompress->Compress(*this, vpBuffer, vBufferSize,
                              pFileInfo))  //将内容压缩好，并填充相应信息
    {
        DEL_ONE(pFileInfo);
        return false;
    }

    //将文件信息添加到文件表中
    mFileTable.insert(map<string, SNormalFileInfo*>::value_type(vrNameInPack, pFileInfo));
    return true;
}

bool CNormalPack::Create()  //创建一个包
{
    mDirty = true;  //脏标记

    //写包信息
    (*mpPackFile) << mInfo;

    SetEnd(mpPackFile->Tell());
    return true;
}

bool CNormalPack::Open()  //打开一个已经存在的包
{
    //读出包信息
    (*mpPackFile) >> mInfo;
    SetEnd(mpPackFile->Size());  //文件尾
    return ReadFileTable();
}

bool CNormalPack::Flush()  //将包的信息写入
{
    mpPackFile->Flush();
    return true;
}

bool CNormalPack::Save()  //将包中所有的信息存盘
{
    if (!mDirty) {
        return true;
    }

    //将保留块释放
    list<SChangingBlockInfo>::iterator It;
    for (It = mReserveFreeBlock.begin(); It != mReserveFreeBlock.end(); ++It) {
        FreeBlock((*It).mOffset, (*It).mSize);
    }

    //写文件表和空余块
    //	Debug<<"写入文件表开始\n";
    WriteFileTable();
    //	Debug<<"写入文件表结束\n";
    mDirty = false;

    //写包信息
    mpPackFile->Seek(mBegin, SEEK_SET);
    (*mpPackFile) << mInfo;
    Flush();

    return true;
}

void CNormalPack::Dirty() {
    if (!mDirty) {
        //首先设置脏标记
        mDirty = true;
        //释放旧文件表//在新的文件表被写入前，并不真正被释放
        FreeReserveBlock(mInfo.mBlockTableOffset, mInfo.mBlockTableSize);
    }
    return;
}

const long CNormalPack::GetEnd()  //得到包结尾
{
    return mEnd;
}

void CNormalPack::SetEnd(const long vEnd) {
    mEnd = vEnd;
    return;
}

SChangingBlockInfo CNormalPack::GetBlock(int vBlockSize)  //得到空余块
{
    Dirty();  //有变动，设置为脏

    //取得最小的符合的空闲块
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

    if (bDoatEnd) {  //如果没有符合的空闲块，就从包尾得到
        Block.mOffset = GetEnd();
        Block.mSize = vBlockSize;
        SetEnd(GetEnd() + vBlockSize);  //改变包结尾
    } else {                            //有得到空闲块
        //还要把多余的空间释放掉
        if (Block.mSize - vBlockSize > 0) {  //空间有多
            FreeBlock(Block.mOffset + vBlockSize, Block.mSize - vBlockSize);
            Block.mSize = vBlockSize;
        }
    }

    //	Debug<<"得到空间 Offset[" <<Block.mOffset << "]     Size["<< Block.mSize << "]\n";
    return Block;
}

void CNormalPack::FreeReserveBlock(int vOffset, int vSize)  //释放保留的空余块
{
    if (vSize == 0) {
        return;
    }
    Dirty();  //有变动，设置为脏
    SChangingBlockInfo Block(vOffset, vSize);
    mReserveFreeBlock.push_back(Block);  //保留列表
    return;
}

void CNormalPack::FreeBlock(int vOffset, int vSize, bool vDirty)  //释放空余块
{
    if (vSize == 0) {
        return;
    }

    if (vDirty) {
        Dirty();  //有变动，设置为脏
    }

    int Offset = vOffset;
    int Size = vSize;

    //将该块同已经有的空余块合并
    list<SChangingBlockInfo>::iterator It;

    for (It = mFreeBlock.begin(); It != mFreeBlock.end();) {
        if (Offset + Size == It->mOffset) {
            Size += It->mSize;  //将该块的后一个块得到
            It = mFreeBlock.erase(It);
        } else if (Offset == It->mOffset + It->mSize) {
            Offset = It->mOffset;
            Size += It->mSize;  //将该块的前一个块得到
            It = mFreeBlock.erase(It);
        } else {
            It++;
        }

        if (Offset != vOffset && (Offset + Size) != vOffset + vSize) {
            break;
        }
    }

    SChangingBlockInfo Block(Offset, Size);

    //目前不处理包变小
    //if(Offset+Size==GetEnd())
    //{//是包尾，改变包尾
    //	SetEnd(Offset);
    //}
    //else
    //插入空余列表
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
        //MessageBox(0,"该文件已为加密版本，升级失败","信息提示",MB_OK);
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

void CNormalPack::WriteFileTable()  //写文件表
{
    //计算文件表大小

    //文件表信息大小
    size_t Size = 0;

    //104版本后一些包信息放在了这里//算是一种补丁
    Size += mpPackFile->SizeInPackFile(mInfo.mMainContentVersion);  //内容的主要版本信息的大小
    Size += mpPackFile->SizeInPackFile(mInfo.mSubContentVersion);  //内容的次要版本信息的大小
    Size += mpPackFile->SizeInPackFile(mInfo.mStartTime);  //内容起始时间信息的大小
    Size += mpPackFile->SizeInPackFile(mInfo.mEndTime);    //内容结束时间信息的大小

    map<string, SNormalFileInfo*>::iterator MapIt;
    for (MapIt = mFileTable.begin(); MapIt != mFileTable.end(); ++MapIt) {
        Size += MapIt->second->SizeInPackFile(*mpPackFile);
    }

    //空余块信息大小
    list<SChangingBlockInfo>::iterator ListIt;  //当前包空的文件块
    for (ListIt = mFreeBlock.begin(); ListIt != mFreeBlock.end(); ++ListIt) {
        Size += ListIt->SizeInPackFile(*mpPackFile);
    }

    //得到当前的文件表大小
    //[如果是从文件结尾拿的空间，块列表的大小不变]
    //[如果是从列表中拿的空间，并有剩余，块列表空间不变]
    //[如果是从列表中拿的空间，大小刚好，块列表空间变小]
    //所以，只大不小
    SChangingBlockInfo Block = GetBlock((int)Size);

    //将文件写入
    mpPackFile->Seek(Block.mOffset, SEEK_SET);

    //得到包信息
    mInfo.mBlockTableOffset = Block.mOffset;  //文件块表的偏移量[相对文件头]
    mInfo.mBlockTableSize = Block.mSize;      //文件块表的大小

    mInfo.mFileCount = mFileTable.size();   //文件总数
    mInfo.mBlockCount = mFreeBlock.size();  //空余块总数

    //写文件表信息
    // Debug<<"写文件表信息开始\n";

    (*mpPackFile) << mInfo.mMainContentVersion;  //内容的主要版本信息
    (*mpPackFile) << mInfo.mSubContentVersion;   //内容的次要版本信息
    (*mpPackFile) << mInfo.mStartTime;           //内容起始时间信息
    (*mpPackFile) << mInfo.mEndTime;             //内容结束时间信息

    for (MapIt = mFileTable.begin(); MapIt != mFileTable.end(); ++MapIt) {
        (*mpPackFile) << (*(MapIt->second));
        //Debug<< "包中文件信息写入[" << (*(MapIt->second)) << "]\n";
    }
    //Debug<<"写文件表信息结束\n";

    //写空余块信息
    //Debug<<"写空余块信息写入开始\n";
    for (ListIt = mFreeBlock.begin(); ListIt != mFreeBlock.end(); ++ListIt) {
        (*mpPackFile) << (*ListIt);
        //Debug<<"空余块信息 Offset [ " <<ListIt->mOffset <<  " ]    Size ["  << ListIt->mSize <<"]\n";
    }
    //Debug<<"写空余块信息写入结束\n";

    return;
}

bool CNormalPack::ReadFileTable()  //读文件表
{
    //定位
    mpPackFile->Seek(mInfo.mBlockTableOffset, SEEK_SET);

    //读出文件表信息
    size_t           i;
    SNormalFileInfo* pFileInfo = NULL;

    //部分包信息//包的时间信息
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

    //Debug<<"读出文件表信息开始\n";
    for (i = 0; i < mInfo.mFileCount; i++) {
        pFileInfo = RT_NEW SNormalFileInfo;
        if (pFileInfo == NULL) {
            return false;
        }

        (*mpPackFile) >> (*pFileInfo);

        //Debug<< "[" << (*pFileInfo) << "]\n";

        //将文件信息添加到文件表中
        mFileTable.insert(map<string, SNormalFileInfo*>::value_type(pFileInfo->mName, pFileInfo));
    }
    //	Debug<<"读出文件表信息结束\n";

    //读出空余块信息

    //	Debug<<"读出空余块信息开始\n";
    SChangingBlockInfo BlockInfo;
    for (i = 0; i < mInfo.mBlockCount; i++) {
        (*mpPackFile) >> BlockInfo;
        //将空余块信息添加到列表中
        FreeBlock(BlockInfo.mOffset, BlockInfo.mSize, false);

        //		Debug<<"Offset[ " << BlockInfo.mOffset << "]   Size["  << BlockInfo.mSize << "]\n";
    }
    //	Debug<<"读出空余块信息结束\n";

    return true;
}

bool CNormalPack::BeginWrite(int vRequestSize, SFileInfo* vpFileInfo)  //开始写
{
    //得到要的文件空间
    SChangingBlockInfo Block = GetBlock(vRequestSize);

    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //到指定位置
    mAllowWrite = true;
    return true;

    /*
	//将内存缓存写入
	if(!mpPackFile->PutBuffer(Temp))
	{
		return false;
	}
	*/
}

int CNormalPack::Write(const void* vpData, const int vDataSize)  //写
{
    if (!mAllowWrite) {
        return 0;
    }

    return (int)mpPackFile->PutBuffer(vpData, vDataSize);
}

void CNormalPack::EndWrite()  //结束写
{
    mAllowWrite = false;
    return;
}

bool CNormalPack::OnceWrite(CMemoryBuffer& vrBuffer, SFileInfo* vpFileInfo) {
    //得到要的文件空间
    SChangingBlockInfo Block = GetBlock(vrBuffer.DataSize());

    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //到指定位置
    return mpPackFile->PutBuffer(vrBuffer);
}

bool CNormalPack::OnceWrite(const void* vpData, const int vDataSize,
                            SFileInfo* vpFileInfo)  //一次写
{
    //得到要的文件空间
    SChangingBlockInfo Block = GetBlock(vDataSize);
    vpFileInfo->mOffset = Block.mOffset;
    vpFileInfo->mSizeInPack = Block.mSize;
    mpPackFile->Seek(Block.mOffset, SEEK_SET);  //到指定位置

    if ((const int)mpPackFile->PutBuffer(vpData, vDataSize) == vDataSize) {
        return true;
    } else {
        return false;
    }
}

bool CNormalPack::OnceRead(void* vpData, const SFileInfo* vpFileInfo)  //一次读出
{
    gRealReadFile.Lock();
    mpPackFile->Seek(vpFileInfo->mOffset, SEEK_SET);  //到指定位置

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
CCompress* vpCompress)			//添加包中文件从一个包文件
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//已经存在
return false;
}

//文件压缩
static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static CMemoryBuffer Temp;
Temp.ClearData();//清空数据

//读出数据//并得到MD5值//得到
RtMD5Context context;
rtMD5Init (&context);

size_t ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,TempBuffer);//读出数据
for(;ReadSize!=0;)
{
if(vpCompress->Compress(TempBuffer,ReadSize))
{
//出错信息
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, (int)ReadSize);//现算MD5
ReadSize=vrUnpackFile.GetBuffer(DEFAULT_BUFFER_SIZE_4K,TempBuffer);//继续读出数据
}


//得到要的文件空间
SChangingBlockInfo Block=GetBlock(Temp.DataSize());

//将内存缓存写入
mpPackFile->Seek(Block.mOffset,SEEK_SET);
if(!mpPackFile->PutBuffer(Temp))
{
return false;
}


//创建新的文件信息
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;

pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSizeInPack=Block.mSize;
pFileInfo->mOriginSize=(int)vrUnpackFile.Size();
pFileInfo->mCompressMode=vpCompress->GetMode();
pFileInfo->mCompressVersion=vpCompress->GetVersion();
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);


//将文件信息添加到文件表中
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));
return true;
}
*/

/*
void CNormalPack::WriteFileTable()				//写文件表
{
mpPackFile->Seek(GetEnd(),SEEK_SET);		//得到包尾

//得到包信息
mInfo.mBlockTableOffset=GetEnd();			//文件块表的偏移量[相对文件头]


mInfo.mFileCount=mFileTable.size();			//文件总数
mInfo.mBlockCount=mFreeBlock.size();		//空余块总数


//写文件表信息
map<string,SNormalFileInfo*>::iterator MapIt;		
for(MapIt=mFileTable.begin();MapIt!=mFileTable.end();++MapIt)
{
(*mpPackFile)<<(*(MapIt->second));
Debug<< "包中文件信息写入[" << (*(MapIt->second)) << "]\n";
}

//写空余块信息
list<SChangingBlockInfo>::iterator ListIt;	//当前包空的文件块
for(ListIt=mFreeBlock.begin();ListIt!=mFreeBlock.end();++ListIt)
{
(*mpPackFile)<<(*ListIt);
}


//
SetEnd(mpPackFile->Tell());
mInfo.mBlockTableSize=mEnd-mInfo.mBlockTableOffset;			//文件块表的大小


return ;
}
*/

/*
bool CNormalPack::AddFile(const string& vrNameInPack,void* vpBuffer,int vBufferSize,int vRTReadSize)	//添加包中文件从内存
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//已经存在
return false;
}

//文件压缩
static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2+4];//4是因为其中有一个字节说明数据的长度
int					CompressSize;
static CMemoryBuffer Temp;
Temp.ClearData();//清空数据



//读出数据//并得到MD5值
RtMD5Context context;
rtMD5Init (&context);


int ReadSize=vrBuffer.Read(pData);//读出数据
for(;ReadSize!=0;)
{
CompressSize=vpCompress->Compress(TempBuffer,(unsigned int)ReadSize,TempCompress);//压缩

//写入临时内存
if(!Temp.Write(TempCompress,CompressSize))
{
//出错信息
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, ReadSize);//现算MD5
ReadSize=vrBuffer.Read(pData);
}


//得到要的文件空间
SChangingBlockInfo Block=GetBlock(Temp->DataSize());


//将内存缓存写入
mpPackFile->Seek(Block.mOffset,SEEK_SET);

if(!mpPackFile->PutBuffer(Temp))
{
return false;
}










//得到要的文件空间
SChangingBlockInfo Block=GetBlock(vBufferSize);
//将文件写入
mpPackFile->Seek(Block.mOffset,SEEK_SET);
if(mpPackFile->PutBuffer(vpBuffer,vBufferSize)!=vBufferSize)
{
return false;
}






//创建新的文件信息
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;
pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSize=Block.mSize;
pFileInfo->mRTSize=vRTReadSize;
pFileInfo->
pFileInfo->

//得到MD5值
RtMD5Context context;
rtMD5Init (&context);
rtMD5Update (&context,(unsigned char*) vpBuffer, vBufferSize);
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);
//MDPrint((unsigned char*)pFileInfo->mMD5);



//将文件信息添加到文件表中
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));

return true;
}
*/

/*
//置换包中文件从文件
bool CNormalPack::ReplaceFile(const string& vrFileName,	const string& vrNameInPack,
CCompress* vpCompress)
{
DelFile(vrNameInPack);
return AddFile(vrFileName,vrNameInPack,vpCompress);
}
*/

/*
//置换包中文件从内存块
bool CNormalPack::ReplaceFile(const string& vrNameInPack,	void* vpBuffer,		int vBufferSize,
CCompress* vpCompress,long vTime)
{
DelFile(vrNameInPack);
return AddFile(vrNameInPack,vpBuffer,vBufferSize,vpCompress,vTime);  // (vrNameInPack,vpBuffer,vBufferSize,vRTReadSize);
}
*/

/*
//添加包中文件从内存缓存中
bool CNormalPack::AddFile(const string& vrNameInPack,CMemoryBuffer& vrBuffer,CCompress* vpCompress)
{
if(GetFileInfo(vrNameInPack)!=NULL)
{//已经存在
return false;
}

//文件压缩



static char TempBuffer[DEFAULT_BUFFER_SIZE_4K];
static char TempCompress[DEFAULT_BUFFER_SIZE_4K*2+4];//4是因为其中有一个字节说明数据的长度
int					CompressSize;
static CMemoryBuffer Temp;
Temp.ClearData();//清空数据



//读出数据//并得到MD5值
RtMD5Context context;
rtMD5Init (&context);


int ReadSize=vrBuffer.Read(pData);//读出数据
for(;ReadSize!=0;)
{
CompressSize=vpCompress->Compress(TempBuffer,(unsigned int)ReadSize,TempCompress);//压缩

//写入临时内存
if(!Temp.Write(TempCompress,CompressSize))
{
//出错信息
return false;
}
rtMD5Update (&context,(unsigned char*) TempBuffer, ReadSize);//现算MD5
ReadSize=vrBuffer.Read(pData);
}


//得到要的文件空间
SChangingBlockInfo Block=GetBlock(Temp->DataSize());


//将内存缓存写入
mpPackFile->Seek(Block.mOffset,SEEK_SET);

if(!mpPackFile->PutBuffer(Temp))
{
return false;
}

//创建新的文件信息
SNormalFileInfo* pFileInfo=RT_NEW SNormalFileInfo;

pFileInfo->mName=vrNameInPack;
pFileInfo->mOffset=Block.mOffset;
pFileInfo->mSizeInPack=Block.mSize;
pFileInfo->mOriginSize=vrBuffer.DataSize();
pFileInfo->mCompressMode=vpCompress->GetMode();
pFileInfo->mCompressVersion=vpCompress->GetVersion();
rtMD5Final ((unsigned char*)pFileInfo->mMD5, &context);


//将文件信息添加到文件表中
mFileTable.insert(map<string,SNormalFileInfo*>::value_type(vrNameInPack,pFileInfo));

return true;
}
*/

/*
size_t  CNormalPack::GetFileCount()//得到包文件总数
{
return mFileTable.size();
}

bool CNormalPack::GetAllFileInfo(SFileInfo* vpFileInfo[])                             //得到包中所有文件信息
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

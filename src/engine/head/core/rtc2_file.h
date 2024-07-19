/**
* @file rtc2_file.h 负责文件系统和流式文件操作.
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     RtArchive
-----------------------------------------------------------------------------*/

/** 流式文件读取基础类.
* 并不包含实际的读取，需要由其他类继承
*/
class RT2_CORE_API RtArchive
{
public:
    enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2 };
    enum SeekMode { SeekBegin, SeekCurrent, SeekEnd };
    RtArchive()
    {
        m_iFileVersion = RTE_FILE_VERSION;
        m_nMode = 0;
        m_bIsError = 0;
		memset( m_sPacktFileName , NULL , _MAX_PATH );
		memset( m_sPathFileName ,  NULL , _MAX_PATH );
    }
    virtual ~RtArchive() {}
    virtual void SerializeBits( void* V, int iLengthBits )
    {
        Serialize( V, (iLengthBits+7)/8 );
        if( IsLoading() )
            ((BYTE*)V)[iLengthBits/8] &= ((1<<(iLengthBits&7))-1);
    }
    virtual long Serialize( void* V, int iLength ) = 0;                                     ///< 读 / 写
    virtual void Seek( int InPos, SeekMode eSeek = SeekCurrent )    {}                      ///< 移动当前指针
    virtual void Flush()                                            {}                      ///< 提交
    virtual BOOL Close()                                            { return !m_bIsError;}  ///< 关闭
    virtual BOOL GetError()                                         { return m_bIsError;}   ///< 取得错误
    virtual int  Tell()                                             { return INDEX_NONE;}   ///< 取得当前位置
    virtual int  TotalSize()                                        { return INDEX_NONE;}   ///< 取得大小
    virtual BOOL AtEnd()
    {
        int Pos = Tell();
        return Pos!=INDEX_NONE && Pos>=TotalSize();
    }
    virtual RtArchive& operator<<( class RtObject*& Res );

    RtObject* NewObject();
    RtObject* ReadObject(const RtRuntimeClass* pClass);
    bool ReadObject(RtObject* _obj, const RtRuntimeClass* pClass);

    bool WriteObject(RtObject* pObj);
    RtRuntimeClass* ReadClass(UINT* pSchema);
    void WriteClass(const RtRuntimeClass* pClassRef);

    // Status accessors.
    int  Version() const        { return m_iFileVersion;}
    BOOL IsError() const        { return m_bIsError;}
	//    BOOL IsLoading() const      { return ((m_nMode & RtArchive::load) != 0); }
	inline BOOL IsLoading() const	
	{
		int nTest = m_nMode & RtArchive::load;
		if (nTest != 0)
			return true;
		else
			return false;
	}
	//    BOOL IsStoring() const      { return ((m_nMode & RtArchive::load) == 0); 
	inline BOOL IsStoring() const      
	{
		int nTest = m_nMode & RtArchive::load;
		if (nTest == 0)
			return true;
		else
			return false;
	}
	BOOL IsBufferEmpty()        { return TotalSize()==0; }

    // Friend archivers.
    friend RtArchive& operator<<( RtArchive& Ar, BYTE& B )  { Ar.Serialize( &B, 1 );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, SBYTE& B ) { Ar.Serialize( &B, 1 );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, WORD& W )  { Ar.ByteOrderSerialize( &W, sizeof(W) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, SWORD& S ) { Ar.ByteOrderSerialize( &S, sizeof(S) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, DWORD& D ) { Ar.ByteOrderSerialize( &D, sizeof(D) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, size_t& D ) { Ar.ByteOrderSerialize( &D, sizeof(D) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, int& I )   { Ar.ByteOrderSerialize( &I, sizeof(I) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, FLOAT& F ) { Ar.ByteOrderSerialize( &F, sizeof(F) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, QWORD& Q ) { Ar.ByteOrderSerialize( &Q, sizeof(Q) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, SQWORD& S) { Ar.ByteOrderSerialize( &S, sizeof(S) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, RtTime& F) { Ar.ByteOrderSerialize( &F, sizeof(F) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, long &L)   { Ar.ByteOrderSerialize( &L, sizeof(L) );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, char &C )  { Ar.Serialize( &C, 1 );return Ar;}
    friend RtArchive& operator<<( RtArchive& Ar, bool &B )  { Ar.Serialize( &B, 1 );return Ar;}

    friend RtArchive& operator<< (RtArchive& Ar, float3& f3)
    {
        Ar<<f3.x<<f3.y<<f3.z;
        return Ar;
    }

    friend RtArchive& operator<<( RtArchive& Ar, const char* str) 
    { 
        int iLen = 0;
        if(Ar.IsLoading())
        {
            CHECKEX("Cannot write const char*");
        }else
        {
            iLen = (int)strlen(str);
            Ar << iLen;
            Ar.Serialize((void*)str, iLen);
        }
        return Ar;
    }
    friend RtArchive& operator<<( RtArchive& Ar, std::string& str) 
    { 
        const size_t _buffer_size = 1024;
        char _buffer[_buffer_size];
        int iLen = 0;

        if(Ar.IsLoading())
        {
            Ar << iLen;
            if(iLen < _buffer_size)
            {
                Ar.Serialize(_buffer, iLen);
                _buffer[iLen] = 0;
                str = _buffer;
            }else
            {
                char *pTmp = (char*)RtMemoryAllocator::Instance().Alloc(iLen+1, __FILE__, __LINE__);
                Ar.Serialize(pTmp,iLen);
                pTmp[iLen] = 0;
                str = pTmp;
                RtMemoryAllocator::Instance().Free(pTmp);
            }
        }else
        {
            iLen = (int)str.size();
            Ar << iLen;
            Ar.Serialize((void*)str.c_str(), iLen);
        }
        return Ar;
    }
    friend RtArchive& operator<<( RtArchive &Ar, RtString &str) 
    {
        const size_t _buffer_size = 1024;
        char _buffer[_buffer_size];
        int iLen;
        if (Ar.IsLoading())
        {
            Ar << iLen;
            if(iLen < _buffer_size)
            {
                Ar.Serialize(_buffer, iLen);
                _buffer[iLen] = 0;
                str = _buffer;
            }else
            {
                char *pTmp = (char*)RtMemoryAllocator::Instance().Alloc(iLen+1, __FILE__, __LINE__);
                Ar.Serialize(pTmp, iLen);
                pTmp[iLen] = 0;
                str = pTmp;
                RtMemoryAllocator::Instance().Free(pTmp);
            }
        }else
        {
            iLen = str.GetLength();
            Ar << iLen;
            Ar.Serialize((char*)str, iLen);
        }
        return Ar;
    }

    // Hardcoded datatype routines that may not be overridden.
    RtArchive& ByteOrderSerialize( void* V, int Length )
    {
        Serialize( V, Length );
        return *this;
    }
public:
	char    m_sPacktFileName[ _MAX_PATH ];
	char    m_sPathFileName [ _MAX_PATH ];
protected:
    // Status variables.
    int  m_nMode;
    int  m_iFileVersion;
    BOOL m_bIsError;
};

/*-----------------------------------------------------------------------------
-     RtMemoryArchive
-----------------------------------------------------------------------------*/
class RtMemoryArchive : public RtArchive
{
public:
	RtMemoryArchive(char* pBuf=NULL, int iSize=0);
    virtual ~RtMemoryArchive();

    virtual void Seek(int InPos, SeekMode eSeek);
    virtual int  Tell();
    virtual int  TotalSize();
    virtual BOOL Close();
    virtual void Flush();
    virtual long Serialize(void* V, int iLength);

	bool InitReader(int size);
	bool InitWriter(int size);

	char *GetBuffer();
	int GetBufferSize();
	int GetDataSize();

protected:
	char *AllocBuffer(int size);
	bool  RellocBuffer(int min);
	bool  IsOverflow(int offset);

    char*   m_pAllocMem;
    char*   m_buf;
	char*   m_pos;
    int     m_dataSize;		// valid data size
	int     m_bufSize;			// buffer total size
};


/*-----------------------------------------------------------------------------
-     RtFileManager
-         
-----------------------------------------------------------------------------*/

enum ERTFileReadState
{
    FRS_FAIL,             ///< 读取失败
    FRS_CONNECT,          ///< 连接中
    FRS_DOWNLOADING,      ///< 下载中
    FRS_COMPLETE          ///< 完成
};

typedef void (*TCBAsyncFileRender)(RtArchive* pArch, const char* pFilename, int iParam1, int iParam2);
typedef void (*TCBAsyncFileRenderState)(const char* pFilename, ERTFileReadState eState, int iParam1, int iParam2);

/** 文件管理.
* 包含异步文件读取等功能
*/
class RT2_CORE_API RtFileManager
{
public:
    enum ERTFileTimes
    {
        FT_Create      = 0,
        FT_LastAccess  = 1,
        FT_LastWrite   = 2,
    };

    enum ERTFileWrite
    {
        FW_NoFail            = 0x01,
        FW_NoReplaceExisting = 0x02,
        FW_EvenIfReadOnly    = 0x04,
        FW_Unbuffered        = 0x08,
        FW_Append            = 0x10,
        FW_AllowRead         = 0x20,
        FW_Text              = 0x80,
    };

    enum ERTFileRead
    {
        FR_NoFail            = 0x01,
    };


    enum ERTFilePermission
    {
        FP_READ               = 0x01,
        FP_WRITE              = 0x02,
        FP_READONLY           = 0x01,
        FP_READWRITE          = 0x03,
    };
public:
    RtFileManager()
    {
        m_dwTotalReadByte = 0;
        m_dwTotalWriteByte = 0;
    }
    virtual ~RtFileManager(){}

    /// 异步文件读取
    bool CreateAsyncFileRender(
        const char* pFilename,              ///< 文件名
        TCBAsyncFileRender pCBRead,         ///< 读取Callback, 如果读取失败, pArch返回NULL
        TCBAsyncFileRenderState pCBState=0, ///< 状态Callback
        int iParam1=0,                      ///< 参数1
        int iParam2=0,                      ///< 参数2
        DWORD dwReadFlags=0                 ///< 标志
        );

    /** 预载入指定文件.
    * @param pFilename 需要预载入的文件
    * @return 无返回参数
    */
    virtual void PreLoadFileRead(const char* pFilename) {}

    /** 创建读取文件.
    * @param pFilename 文件名
    * @param dwReadFlags 读取参数，由ERTFileRead按位组合而成
    * @return 返回文件流指针
    */
    virtual RtArchive* CreateFileReader( const char* pFilename, DWORD dwReadFlags=0)=0;

    /** 创建写入文件.
    * @param pFilename 文件名
    * @param dwReadFlags 读取参数，由ERTFileWrite按位组合而成
    * @return 返回文件流指针
    */
    virtual RtArchive* CreateFileWriter( const char* pPackageName, const char* pFilename, DWORD dwWriteFlags=0)=0;

    /** 关闭文件.
    * @param pFileArchive 文件流指针
    * @return 无返回参数
    */
    virtual void   CloseFile( RtArchive* pFileArchive );

    /** 取得文件大小.
    * @param pFilename 文件名
    * @return 返回文件大小，只有一个32文int值，所以文件最大为2G
    */
    virtual int    FileSize( const char* pFilename )=0;

    /** 设置文件属性.
    * @param pFilename 文件名
    * @param dwPermission 由ERTFilePermission按位组合而成
    * @return 返回是否成功
    */
    virtual BOOL   SetFilePermission( const char* pFilename, DWORD dwPermission )=0;

    /** 取得文件属性.
    * @param pFilename 文件名
    * @return 返回属性，具体的值对应ERTFilePermission的成员
    */
    virtual DWORD  GetFilePermission( const char* pFilename )=0;

    /** 检查文件是否存在.
    * @param pFilename 文件名
    * @return 返回是否存在
    */
    virtual BOOL   FileExist( const char* pFilename ) { return FALSE;}

    /** 删除文件.
    * @param pFilename 文件名
    * @param bRequireExists 这个参数如果为TRUE，那么文件必须存在，否则返回FALSE
    * @param bEvenReadOnly 是否删除只读文件
    * @return 返回删除是否成功
    */
    virtual BOOL   Delete( const char* pFilename, BOOL bRequireExists=0, BOOL bEvenReadOnly=0 )=0;

    /** 复制文件.
    * @param pPackageName 包名，用于打包文件中，如果普通文件这个参数传NULL
    * @param pDest 目标文件名
    * @param pSrc 源文件名
    * @param bReplace 是否覆盖
    * @param bEvenIfReadOnly 当目标文件只读的时候是否覆盖
    * @param bAttributes 是否复制属性
    * @param fnProgress 复制百分比回调函数，默认为NULL
    * @return 返回复制是否成功
    */
    virtual BOOL   Copy( const char* pPackageName, const char* pDest, const char* pSrc, BOOL bReplace=1, BOOL bEvenIfReadOnly=0, BOOL bAttributes=0, void (*fnProgress)(FLOAT fFraction)=NULL )=0;

    /** 移动文件.
    * @param pPackageName 包名，用于打包文件中，如果普通文件这个参数传NULL
    * @param pDest 目标文件名
    * @param pSrc 源文件名
    * @param bReplace 是否覆盖
    * @param bEvenIfReadOnly 当目标文件只读的时候是否覆盖
    * @param bAttributes 是否复制属性
    * @return 返回复制是否成功
    */
    virtual BOOL   Move( const char* pPackageName, const char* pDest, const char* pSrc, BOOL bReplace=1, BOOL bEvenIfReadOnly=0, BOOL bAttributes=0 )=0;

    /** 取得文件创建时间.
    * @param pFilename 文件名
    * @return 返回时间
    */
    virtual SQWORD GetCreateTime( const char* pFilename )=0;

    /** 设置文件创建时间.
    * @param pFilename 文件名
    * @return 返回设置是否成功
    */
    virtual BOOL   SetCreateTime( const char* pFilename )=0;

    /** 取得文件最后存取时间.
    * @param pFilename 文件名
    * @return 返回时间
    */
    virtual SQWORD GetAccessTime( const char* pFilename )=0;

    /** 设置文件最后存取时间.
    * @param pFilename 文件名
    * @return 返回设置是否成功
    */
    virtual BOOL   SetAccessTime( const char* pFilename )=0;

    /** 取得文件修改时间.
    * @param pFilename 文件名
    * @return 返回时间
    */
    virtual SQWORD GetModifyTime( const char* pFilename )=0;

    /** 设置文件修改时间.
    * @param pFilename 文件名
    * @return 返回设置是否成功
    */
    virtual BOOL   SetModifyTime( const char* pFilename )=0;

    /** 创建目录.
    * @param pPath 目录名
    * @param bTree 如果目录名中包含多级目录没有创建的时候，是否创建整个目录树
    * @return 返回创建是否成功
    */
    virtual BOOL   MakeDirectory( const char* pPath, BOOL bTree=0 )=0;

    /** 删除目录.
    * @param pPath 目录名
    * @param bRequireExists 如果这个参数为TRUE，那么目录必须存在，否则返回FALSE
    * @param bTree 如果目录名中包含多级目录可以删除的时候，是否删除整个目录树
    * @return 返回删除是否成功
    */
    virtual BOOL   DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )=0;

    /** 查找文件.
    * @param pPathname 文件名，支持 * 和 ?，比如d:\abc\*.xml
    * @param bFiles 是否列出文件
    * @param bDirectories 是否列出目录
    * @return 返回找到的所有文件/目录
    */
    virtual std::list<std::string> FindFiles( const char* pPathname, BOOL bFiles, BOOL bDirectories )=0;

    /** 设置默认目录.
    * @param pPathname 目录名
    * @return 返回设置是否成功
    */
    virtual BOOL   SetDefaultDirectory( const char* pPathname )=0;

    /** 取得默认目录.
    * @return 返回目录名
    */
    virtual RtString GetDefaultDirectory()=0;

    /** 初始化.
    */
    virtual void Init(BOOL bStartup) {}

    /// 取得总共的读取文件字节数
    virtual DWORD TotalReadByte()               { return m_dwTotalReadByte; }
    /// 取得总共的写入文件字节数
    virtual DWORD TotalWriteByte()              { return m_dwTotalWriteByte; }
    /// 添加总的读取文件字节数
    virtual void AddReadByte(DWORD dwSize)      { m_dwTotalReadByte += dwSize; }
    /// 添加总的写入文件字节数
    virtual void AddWriteByte(DWORD dwSize)     { m_dwTotalWriteByte += dwSize; }

private:
    DWORD   m_dwTotalReadByte;
    DWORD   m_dwTotalWriteByte;
};

/*-----------------------------------------------------------------------------
-   文件函数
-----------------------------------------------------------------------------*/

/** 根据文件名取得扩展名.
* @param pPathname 文件名（包含扩展名）
* @return 返回扩展名
*/
RT2_CORE_API const char* rtGetFileExt( const char* szFilename );

/** 切分完整文件名.
* @param path  [in] 完整文件名
* @param drive [out] 驱动器名字
* @param dir   [out] 目录
* @param fname [out] 文件名(不包含扩展名)
* @param ext   [out] 文件扩展名
* @return 无返回值
*/
RT2_CORE_API void rtfSplitpath(const char *path, char *drive, char *dir, char *fname, char *ext);

/** 组合完整文件名.
* @param path  [out] 完整文件名
* @param drive [in] 驱动器名字
* @param dir   [in] 目录
* @param fname [in] 文件名(不包含扩展名)
* @param ext   [in] 文件扩展名
* @return 无返回值
*/
RT2_CORE_API void rtfMakepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext);

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/

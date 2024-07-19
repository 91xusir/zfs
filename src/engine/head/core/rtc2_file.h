/**
* @file rtc2_file.h �����ļ�ϵͳ����ʽ�ļ�����.
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     RtArchive
-----------------------------------------------------------------------------*/

/** ��ʽ�ļ���ȡ������.
* ��������ʵ�ʵĶ�ȡ����Ҫ��������̳�
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
    virtual long Serialize( void* V, int iLength ) = 0;                                     ///< �� / д
    virtual void Seek( int InPos, SeekMode eSeek = SeekCurrent )    {}                      ///< �ƶ���ǰָ��
    virtual void Flush()                                            {}                      ///< �ύ
    virtual BOOL Close()                                            { return !m_bIsError;}  ///< �ر�
    virtual BOOL GetError()                                         { return m_bIsError;}   ///< ȡ�ô���
    virtual int  Tell()                                             { return INDEX_NONE;}   ///< ȡ�õ�ǰλ��
    virtual int  TotalSize()                                        { return INDEX_NONE;}   ///< ȡ�ô�С
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
    FRS_FAIL,             ///< ��ȡʧ��
    FRS_CONNECT,          ///< ������
    FRS_DOWNLOADING,      ///< ������
    FRS_COMPLETE          ///< ���
};

typedef void (*TCBAsyncFileRender)(RtArchive* pArch, const char* pFilename, int iParam1, int iParam2);
typedef void (*TCBAsyncFileRenderState)(const char* pFilename, ERTFileReadState eState, int iParam1, int iParam2);

/** �ļ�����.
* �����첽�ļ���ȡ�ȹ���
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

    /// �첽�ļ���ȡ
    bool CreateAsyncFileRender(
        const char* pFilename,              ///< �ļ���
        TCBAsyncFileRender pCBRead,         ///< ��ȡCallback, �����ȡʧ��, pArch����NULL
        TCBAsyncFileRenderState pCBState=0, ///< ״̬Callback
        int iParam1=0,                      ///< ����1
        int iParam2=0,                      ///< ����2
        DWORD dwReadFlags=0                 ///< ��־
        );

    /** Ԥ����ָ���ļ�.
    * @param pFilename ��ҪԤ������ļ�
    * @return �޷��ز���
    */
    virtual void PreLoadFileRead(const char* pFilename) {}

    /** ������ȡ�ļ�.
    * @param pFilename �ļ���
    * @param dwReadFlags ��ȡ��������ERTFileRead��λ��϶���
    * @return �����ļ���ָ��
    */
    virtual RtArchive* CreateFileReader( const char* pFilename, DWORD dwReadFlags=0)=0;

    /** ����д���ļ�.
    * @param pFilename �ļ���
    * @param dwReadFlags ��ȡ��������ERTFileWrite��λ��϶���
    * @return �����ļ���ָ��
    */
    virtual RtArchive* CreateFileWriter( const char* pPackageName, const char* pFilename, DWORD dwWriteFlags=0)=0;

    /** �ر��ļ�.
    * @param pFileArchive �ļ���ָ��
    * @return �޷��ز���
    */
    virtual void   CloseFile( RtArchive* pFileArchive );

    /** ȡ���ļ���С.
    * @param pFilename �ļ���
    * @return �����ļ���С��ֻ��һ��32��intֵ�������ļ����Ϊ2G
    */
    virtual int    FileSize( const char* pFilename )=0;

    /** �����ļ�����.
    * @param pFilename �ļ���
    * @param dwPermission ��ERTFilePermission��λ��϶���
    * @return �����Ƿ�ɹ�
    */
    virtual BOOL   SetFilePermission( const char* pFilename, DWORD dwPermission )=0;

    /** ȡ���ļ�����.
    * @param pFilename �ļ���
    * @return �������ԣ������ֵ��ӦERTFilePermission�ĳ�Ա
    */
    virtual DWORD  GetFilePermission( const char* pFilename )=0;

    /** ����ļ��Ƿ����.
    * @param pFilename �ļ���
    * @return �����Ƿ����
    */
    virtual BOOL   FileExist( const char* pFilename ) { return FALSE;}

    /** ɾ���ļ�.
    * @param pFilename �ļ���
    * @param bRequireExists ����������ΪTRUE����ô�ļ�������ڣ����򷵻�FALSE
    * @param bEvenReadOnly �Ƿ�ɾ��ֻ���ļ�
    * @return ����ɾ���Ƿ�ɹ�
    */
    virtual BOOL   Delete( const char* pFilename, BOOL bRequireExists=0, BOOL bEvenReadOnly=0 )=0;

    /** �����ļ�.
    * @param pPackageName ���������ڴ���ļ��У������ͨ�ļ����������NULL
    * @param pDest Ŀ���ļ���
    * @param pSrc Դ�ļ���
    * @param bReplace �Ƿ񸲸�
    * @param bEvenIfReadOnly ��Ŀ���ļ�ֻ����ʱ���Ƿ񸲸�
    * @param bAttributes �Ƿ�������
    * @param fnProgress ���ưٷֱȻص�������Ĭ��ΪNULL
    * @return ���ظ����Ƿ�ɹ�
    */
    virtual BOOL   Copy( const char* pPackageName, const char* pDest, const char* pSrc, BOOL bReplace=1, BOOL bEvenIfReadOnly=0, BOOL bAttributes=0, void (*fnProgress)(FLOAT fFraction)=NULL )=0;

    /** �ƶ��ļ�.
    * @param pPackageName ���������ڴ���ļ��У������ͨ�ļ����������NULL
    * @param pDest Ŀ���ļ���
    * @param pSrc Դ�ļ���
    * @param bReplace �Ƿ񸲸�
    * @param bEvenIfReadOnly ��Ŀ���ļ�ֻ����ʱ���Ƿ񸲸�
    * @param bAttributes �Ƿ�������
    * @return ���ظ����Ƿ�ɹ�
    */
    virtual BOOL   Move( const char* pPackageName, const char* pDest, const char* pSrc, BOOL bReplace=1, BOOL bEvenIfReadOnly=0, BOOL bAttributes=0 )=0;

    /** ȡ���ļ�����ʱ��.
    * @param pFilename �ļ���
    * @return ����ʱ��
    */
    virtual SQWORD GetCreateTime( const char* pFilename )=0;

    /** �����ļ�����ʱ��.
    * @param pFilename �ļ���
    * @return ���������Ƿ�ɹ�
    */
    virtual BOOL   SetCreateTime( const char* pFilename )=0;

    /** ȡ���ļ�����ȡʱ��.
    * @param pFilename �ļ���
    * @return ����ʱ��
    */
    virtual SQWORD GetAccessTime( const char* pFilename )=0;

    /** �����ļ�����ȡʱ��.
    * @param pFilename �ļ���
    * @return ���������Ƿ�ɹ�
    */
    virtual BOOL   SetAccessTime( const char* pFilename )=0;

    /** ȡ���ļ��޸�ʱ��.
    * @param pFilename �ļ���
    * @return ����ʱ��
    */
    virtual SQWORD GetModifyTime( const char* pFilename )=0;

    /** �����ļ��޸�ʱ��.
    * @param pFilename �ļ���
    * @return ���������Ƿ�ɹ�
    */
    virtual BOOL   SetModifyTime( const char* pFilename )=0;

    /** ����Ŀ¼.
    * @param pPath Ŀ¼��
    * @param bTree ���Ŀ¼���а����༶Ŀ¼û�д�����ʱ���Ƿ񴴽�����Ŀ¼��
    * @return ���ش����Ƿ�ɹ�
    */
    virtual BOOL   MakeDirectory( const char* pPath, BOOL bTree=0 )=0;

    /** ɾ��Ŀ¼.
    * @param pPath Ŀ¼��
    * @param bRequireExists ����������ΪTRUE����ôĿ¼������ڣ����򷵻�FALSE
    * @param bTree ���Ŀ¼���а����༶Ŀ¼����ɾ����ʱ���Ƿ�ɾ������Ŀ¼��
    * @return ����ɾ���Ƿ�ɹ�
    */
    virtual BOOL   DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )=0;

    /** �����ļ�.
    * @param pPathname �ļ�����֧�� * �� ?������d:\abc\*.xml
    * @param bFiles �Ƿ��г��ļ�
    * @param bDirectories �Ƿ��г�Ŀ¼
    * @return �����ҵ��������ļ�/Ŀ¼
    */
    virtual std::list<std::string> FindFiles( const char* pPathname, BOOL bFiles, BOOL bDirectories )=0;

    /** ����Ĭ��Ŀ¼.
    * @param pPathname Ŀ¼��
    * @return ���������Ƿ�ɹ�
    */
    virtual BOOL   SetDefaultDirectory( const char* pPathname )=0;

    /** ȡ��Ĭ��Ŀ¼.
    * @return ����Ŀ¼��
    */
    virtual RtString GetDefaultDirectory()=0;

    /** ��ʼ��.
    */
    virtual void Init(BOOL bStartup) {}

    /// ȡ���ܹ��Ķ�ȡ�ļ��ֽ���
    virtual DWORD TotalReadByte()               { return m_dwTotalReadByte; }
    /// ȡ���ܹ���д���ļ��ֽ���
    virtual DWORD TotalWriteByte()              { return m_dwTotalWriteByte; }
    /// ����ܵĶ�ȡ�ļ��ֽ���
    virtual void AddReadByte(DWORD dwSize)      { m_dwTotalReadByte += dwSize; }
    /// ����ܵ�д���ļ��ֽ���
    virtual void AddWriteByte(DWORD dwSize)     { m_dwTotalWriteByte += dwSize; }

private:
    DWORD   m_dwTotalReadByte;
    DWORD   m_dwTotalWriteByte;
};

/*-----------------------------------------------------------------------------
-   �ļ�����
-----------------------------------------------------------------------------*/

/** �����ļ���ȡ����չ��.
* @param pPathname �ļ�����������չ����
* @return ������չ��
*/
RT2_CORE_API const char* rtGetFileExt( const char* szFilename );

/** �з������ļ���.
* @param path  [in] �����ļ���
* @param drive [out] ����������
* @param dir   [out] Ŀ¼
* @param fname [out] �ļ���(��������չ��)
* @param ext   [out] �ļ���չ��
* @return �޷���ֵ
*/
RT2_CORE_API void rtfSplitpath(const char *path, char *drive, char *dir, char *fname, char *ext);

/** ��������ļ���.
* @param path  [out] �����ļ���
* @param drive [in] ����������
* @param dir   [in] Ŀ¼
* @param fname [in] �ļ���(��������չ��)
* @param ext   [in] �ļ���չ��
* @return �޷���ֵ
*/
RT2_CORE_API void rtfMakepath(char *path, const char *drive, const char *dir, const char *fname, const char *ext);

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/

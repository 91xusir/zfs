
namespace rt_graph {

enum { MAX_TEX_FILE_NAME = 64 };
struct _TexItem
{
    RtgDevice*      pDevice;
    unsigned int    dwResID;
    unsigned int    dwIdx;
    //char            szFileName[MAX_TEX_FILE_NAME];  // �ļ���
	CM_MEMDEF(szFileName, MAX_TEX_FILE_NAME)
    int             iWidth, iHeight;            // ͼƬ��С
    int             iRefCount;                  // ʹ�ü���
    short           iPathIdx;                   // ·������ ����, -1 ��̬, -2 RenderTarget, -3 DepthStencil
    bool            bUseOffScreen;
    BYTE            cReserve1;                  // 
    BYTE            cUse;                       // λ�� 0 ��, 1 û��ʹ��, 2 ����ʹ��
    BYTE            cFormat;                    // 0 Ĭ��, 1 A8R8G8B8, 2 A4R4G4B4, 3 A1R5G5B5 4 X8R8G8B8
    bool            bLastRender;                // ���һ���Ƿ���Ⱦ
    bool            bQualityManage;             // �Ƿ�ʹ����ͼ����
    int             iUseless;                   // δʹ�õļ�����û��ʹ�õ�ʱ��ÿ֡��һ
    short           iMipMapCount;               // MipMap����
    BYTE            cTexBitType;                // 0 Alpha, 1 Color, 2 ColorKey
    BYTE            cR;
    void *          pTexture;                   // DirectX ����ͼָ�� (Video)
    void *          pOffScreen;                 // 
    void *          pSrcRGBData;                // ͼ��RGB����ָ��, ���Ϊ�վͱ�ʾû������
    int             iSrcRGBDataSize;
    //void *          pSrcAlphaData;              // ͼ��Alpha����ָ��, ���Ϊ�վͱ�ʾû������

	_TexItem();
	_TexItem(const _TexItem &object);
    ~_TexItem();
    void Reset();
    int  GetDataSize();
#ifndef _DEBUG
	inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
};
struct _TexIFL
{
    RtgDevice*      pDevice;
    unsigned int    dwResID;
    unsigned int    dwIdx;
    //char            szFileName[MAX_TEX_FILE_NAME];
	CM_MEMDEF(szFileName, MAX_TEX_FILE_NAME)
    int             iPathIdx;                   // ·������
    int             iRefCount;
    int             iIflNumTextures;
    unsigned int *  pTextureID;
    int             iCurrentTexture;
    float           fTimeAccum, fTimeToChange;

	_TexIFL();
	_TexIFL(const _TexIFL &object);
    ~_TexIFL();
    void AddRef();
    void Release();
#ifndef _DEBUG
    inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
};

class RtgTexture
{
public:
    enum GET_ENTRY
    {
        ALREADY_LOAD,
        ALREADY_DELETE,
        LOAD_NEW,
        FULL
    };
    struct TexLock
    {
        unsigned int    dwTexID;
        int             iWidth;
        int             iHeight;
        int             iPitch;
        void*           pBits;
    };

public:
    // Init & Clear
    RtgTexture(RtgDevice* pDevice);
    ~RtgTexture();
    void InitTexture(void);
    void DeleteAllTextures(void);

    void OnBeginRender();
    void OnEndRender();
    bool OnRestoreDevice();
    bool OnInvalidateDevice();

    // Setting
    BOOL SelectTextureFormat(int iBits); // ������ͼ����ɫ���(�磺16, 32)
    void SetTextureFilter(RTGTextureFilter eMipmap, RTGTextureFilter eMag, RTGTextureFilter eMin);
    void SetTextureQuality(int iQuality);    // ������ͼ����
	int  GetTextureQuality();
    int  GetTextureFilter(void);

    /// ����Ϊ�Ƿ��Զ�����DXT��ʽ����ͼ.
    //  Ŀǰ���Ƽ�ʹ�ã���Ϊ���ɵ�����������
    void SetGenerateDXT(bool bGenerate);
    bool GetGenerateDXT()                   { return m_bGenerateDXT; }

    bool Save2File(int texId, const char* fileName);

    /// ������ͼ.
    // @param szFilename      ��ͼ���ļ���
    // @param eTexBitType     ��ͼ����ɫ���ݸ�ʽ
    //      - RTGTBT_AUTO       �Զ�ѡ��
    //      - RTGTBT_COLOR      ֻ����ɫ����(������BMP)
    //      - RTGTBT_ALPHA      ��Alpha����(������TGA)
    //      - RTGTBT_COLORKEY   Alpha�������ʾ�Ƿ�Ҫʹ��ǰ�����ɫ(������TGA)
    //      - RTGTBT_DXT_AUTO   DXT��ʽ
    // @param bCreateNow      �Ƿ����Ͼʹ�����ͼ���������Ϊfalse���ͻ���ʹ�õ����ID��ʱ����д��������������������ͽ��д���
    // @param iMipmap         �м����Mipmap
    // @param cFormat         ��ͼ���ڴ��еĸ�ʽ 0 Ĭ��, 1 A8R8G8B8, 2 A4R4G4B4, 3 A1R5G5B5
    // @param bQualityManage  �Ƿ������������������ܾ��п��ܱ��������������е�UIӦ�ö��������Ա�֤UI��������
    // @return ���ش�������ͼ��ID��������ܴ������ͷ���0
    unsigned int CreateTexture(const char *szFilename, RTGTexBitType eTexBitType, bool bCreateNow=false, int iMipmap=1, BYTE cFormat=0, bool bQualityManage=true);
    unsigned int CreateTextureFromMemory(void *pMemoryTextureFile, int iLength, RTGTexBitType eTexBitType, int iMipmap=1, BYTE cFormat=0);
	unsigned int CreateTextureFromArchive(RtArchive *pArc,int iLength, RTGTexBitType eTexBitType,int iMipmap,BYTE cFormat,bool bflip=false);
    unsigned int CreateTexture(int iWidth, int iHeight, DWORD fmt = RTG_TEXTURE_A8R8G8B8, bool bCreateNow=false); 
    unsigned int CreateTextureDynamic(int iWidth, int iHeight, DWORD fmt = RTG_TEXTURE_A8R8G8B8, bool bCreateNow=false);
    unsigned int CreateTextureRenderTarget(int iWidth, int iHeight, bool bUseOffScreen=false , int nSurfaceFormat = 1);
    unsigned int CreateTextureDepthStencil(int width, int height);
    unsigned int CreateIFLTexture(const char *szFilename, const char *szPathname, int iPathIdx, RTGTexBitType eTexBitType, bool bCreateNow=false, int iMipmap=1, bool bQualityManage=true);
    
    //
    // add by zxy
    // desc : create texture by raw argb data,
    //        the argb data mush 4-byte alignment
    //
    unsigned int CreateTexture(int iWidth, int iHeight, void* argb); 


    /// ɾ����ͼ.
    // @param dwResourceID  ��ͼ��ID
    // @return �����Ƿ�ɹ�
    BOOL DeleteTexture(unsigned int dwResourceID);

    int  GetTextureSize(unsigned int dwResourceID);
    BOOL UseTexture(int iTextureStage, unsigned int ResourceID);
    BOOL UpdateTexture(int dwTextureID, void *pBuffer);
    BOOL UpdateTextureDynamic(int dwTextureID, void *pBuffer);
    _TexItem* GetTextureItem(unsigned int dwResourceID);
    BOOL LockTexture(unsigned int dwResourceID, TexLock& texLock);
    void UnlockTexture(TexLock& texLock);

    // Animation
    void TextureAnimation(float fTimeDiff);

    // Path & Name
    char* GetTextureName(unsigned int dwResourceID);
    BOOL  SetTexturePath(const char *szPath, int iIndex=0);
    char* GetTexturePath(int iIndex=0);

    void SetTotalTextureMem(unsigned int dwTotalMem); // for debug, һ�����ʼ�ĵط����ò���Ч

    // ͳ��
    int TotalUsedTextureSize();
    unsigned int TotalVideoTextureMem();
    unsigned int UsedVideoTextureMem();
    unsigned int FreeVideoTextureMem();

    int GetUsedTextureNum()                 { return (int)m_listUsedTexture.size(); }
    int GetUnuseTextureNum()                { return (int)m_listUnusedTexture.size(); }
    int GetEmptyTextureNum()                { return (int)m_listEmptyTexture.size(); }

    void DumpUsedTexture(bool bDumpToFile=true);
	void StopLoadTexture(){ m_bLoad = false;}
	void StartLoadTexture(){ m_bLoad = true;}

private:
    _TexItem* GetTextureEntry(const char *szFilename, int iPathIdx); // ȡ����ͼ�Ŀ�λ
    _TexItem* GetTextureEntryEx(const char *szFilename, int iPathIdx); // ȡ����ͼ�Ŀ�λ, û�п�λ���ͷ�һЩ��ͼ
    _TexIFL* GetIFLTextureEntry(const char *szFilename, int iPathIdx); // ȡ��IFL��ͼ�Ŀ�λ
    char* GetTextureFilePath(const char* szFilename, int &iPathIdx, int &iExtIdx);
    BOOL CreateTextureEntity(_TexItem* pItem);
    BOOL UnloadAllTexture();    // �����д�������ͼ����գ���Ҫ������ͼ�����仯��ȫ�ֱ仯��ʱ��
    bool PhysicsDeleteMinimumCostTexture();
    bool ReleaseVideoMemMinimumCostTexture();

    void ActiveTexture(_TexItem* pItem);
    void AddTextureRef(_TexItem* pItem);
    void ReleaseTextureRef(_TexItem* pItem);
    void ReleaseTextureVideoMem(_TexItem* pItem);
    void CreateTextureVideoMem(_TexItem* pItem);

    unsigned int AllocID(unsigned int dwIdx);
    bool ReleaseID(unsigned int dwRes);
    unsigned int FindIdx(unsigned int dwRes)
    {
        EXT_SPACE::unordered_map<unsigned int, unsigned int>::iterator it = m_mapRes.find(dwRes);
        if (it!=m_mapRes.end())
        {
            return (*it).second;
        }
        return 0;
    }

public:
    static int  m_iMaxMultiTextures;

#ifdef MULTITHREAD
	RtProcessLock  m_lock;
#endif

private:
    enum { MAX_TEXTURE_PATHS        = 8     };
    enum { MAX_TEXTURES             = 1024  };
    enum { MAX_IFL_TEXTURES         = 128   };
    enum { IFL_TAG                  = 2000  };
    enum { MAX_FILENAME_EXTENSION   = 6     };

    //char            m_szTexturePathList[MAX_TEXTURE_PATHS][128];
    //char            m_szFilenameExtension[MAX_FILENAME_EXTENSION][12];
	CM_MEMDEF2(m_szTexturePathList, MAX_TEXTURE_PATHS, 128)
	CM_MEMDEF2(m_szFilenameExtension, MAX_FILENAME_EXTENSION, 12)
    RTGTexBitType   m_eFilenameExtensionBitType[MAX_FILENAME_EXTENSION];

public:
    _TexItem*        m_TextureList;

private:
    RtgDevice*      m_pDevice;
	_TexIFL*         m_IFLTextureList;

    bool            m_bUseSrcDataCompress;  // �Ƿ���ڴ��е���ͼ���ݽ���ѹ��
    bool            m_bUseCustomTexManager; // �Ƿ�ʹ���Լ�����ͼ����
    bool            m_bGenerateDXT;         // �Ƿ�����DXT��ʽ
	bool            m_bLoad;                // �Ƿ���ļ���������ͼ

    int             bTexture[8];
    int             m_iNumTextureBits;
    unsigned int    m_dwCurrentTexture;
    unsigned int    m_dwCurSysTexID;

    unsigned int    m_dwTotalVideoTexSize;  // �ܹ����Դ��С
    unsigned int    m_dwUsedVideoTexSize;   // �ܹ�ʹ�õ��Դ��С

    std::list<_TexItem*>  m_listUsedTexture;      // ��ǰʹ�õ���ͼ
    std::list<_TexItem*>  m_listUnusedTexture;    // �Ѿ�û��ʹ�õ���ͼ������û�д�����ɾ��
    std::list<_TexItem*>  m_listEmptyTexture;     // �յĵ���ͼ

    std::list<_TexIFL*>   m_listUsedIFL;          // ��ǰʹ�õ���ͼ
    //std::list<_TexIFL*>   m_listUnusedIFL;        // �Ѿ�û��ʹ�õ���ͼ������û�д�����ɾ��
    std::list<_TexIFL*>   m_listEmptyIFL;         // �յĵ���ͼ

    DWORD           m_dwIncreaseRes;
    EXT_SPACE::unordered_map<unsigned int, unsigned int>    m_mapRes;

    friend RtgDevice;
    friend _TexItem;
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

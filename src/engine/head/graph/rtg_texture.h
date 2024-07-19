
namespace rt_graph {

enum { MAX_TEX_FILE_NAME = 64 };
struct _TexItem
{
    RtgDevice*      pDevice;
    unsigned int    dwResID;
    unsigned int    dwIdx;
    //char            szFileName[MAX_TEX_FILE_NAME];  // 文件名
	CM_MEMDEF(szFileName, MAX_TEX_FILE_NAME)
    int             iWidth, iHeight;            // 图片大小
    int             iRefCount;                  // 使用计数
    short           iPathIdx;                   // 路径索引 类型, -1 动态, -2 RenderTarget, -3 DepthStencil
    bool            bUseOffScreen;
    BYTE            cReserve1;                  // 
    BYTE            cUse;                       // 位置 0 空, 1 没有使用, 2 正在使用
    BYTE            cFormat;                    // 0 默认, 1 A8R8G8B8, 2 A4R4G4B4, 3 A1R5G5B5 4 X8R8G8B8
    bool            bLastRender;                // 最后一次是否渲染
    bool            bQualityManage;             // 是否使用贴图管理
    int             iUseless;                   // 未使用的计数，没有使用的时候每帧加一
    short           iMipMapCount;               // MipMap层数
    BYTE            cTexBitType;                // 0 Alpha, 1 Color, 2 ColorKey
    BYTE            cR;
    void *          pTexture;                   // DirectX 的贴图指针 (Video)
    void *          pOffScreen;                 // 
    void *          pSrcRGBData;                // 图像RGB数据指针, 如果为空就表示没有数据
    int             iSrcRGBDataSize;
    //void *          pSrcAlphaData;              // 图像Alpha数据指针, 如果为空就表示没有数据

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
    int             iPathIdx;                   // 路径索引
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
    BOOL SelectTextureFormat(int iBits); // 设置贴图的颜色深度(如：16, 32)
    void SetTextureFilter(RTGTextureFilter eMipmap, RTGTextureFilter eMag, RTGTextureFilter eMin);
    void SetTextureQuality(int iQuality);    // 设置贴图质量
	int  GetTextureQuality();
    int  GetTextureFilter(void);

    /// 设置为是否自动生成DXT格式的贴图.
    //  目前不推荐使用，因为生成的质量不够好
    void SetGenerateDXT(bool bGenerate);
    bool GetGenerateDXT()                   { return m_bGenerateDXT; }

    bool Save2File(int texId, const char* fileName);

    /// 创建贴图.
    // @param szFilename      贴图的文件名
    // @param eTexBitType     贴图的颜色数据格式
    //      - RTGTBT_AUTO       自动选择
    //      - RTGTBT_COLOR      只有颜色数据(适用于BMP)
    //      - RTGTBT_ALPHA      有Alpha数据(适用于TGA)
    //      - RTGTBT_COLORKEY   Alpha层仅仅表示是否要使用前面的颜色(适用于TGA)
    //      - RTGTBT_DXT_AUTO   DXT格式
    // @param bCreateNow      是否马上就创建贴图，如果设置为false，就会在使用到这个ID的时候进行创建，否则调用这个函数就进行创建
    // @param iMipmap         有几层的Mipmap
    // @param cFormat         贴图在内存中的格式 0 默认, 1 A8R8G8B8, 2 A4R4G4B4, 3 A1R5G5B5
    // @param bQualityManage  是否接受质量管理，如果接受就有可能被调低质量，所有的UI应该都不接受以保证UI的清晰度
    // @return 返回创建好贴图的ID，如果不能创建，就返回0
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


    /// 删除贴图.
    // @param dwResourceID  贴图的ID
    // @return 返回是否成功
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

    void SetTotalTextureMem(unsigned int dwTotalMem); // for debug, 一定在最开始的地方调用才有效

    // 统计
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
    _TexItem* GetTextureEntry(const char *szFilename, int iPathIdx); // 取得贴图的空位
    _TexItem* GetTextureEntryEx(const char *szFilename, int iPathIdx); // 取得贴图的空位, 没有空位会释放一些贴图
    _TexIFL* GetIFLTextureEntry(const char *szFilename, int iPathIdx); // 取得IFL贴图的空位
    char* GetTextureFilePath(const char* szFilename, int &iPathIdx, int &iExtIdx);
    BOOL CreateTextureEntity(_TexItem* pItem);
    BOOL UnloadAllTexture();    // 把所有创建的贴图都清空，主要用在贴图质量变化等全局变化的时候
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

    bool            m_bUseSrcDataCompress;  // 是否对内存中的贴图数据进行压缩
    bool            m_bUseCustomTexManager; // 是否使用自己的贴图管理
    bool            m_bGenerateDXT;         // 是否生成DXT格式
	bool            m_bLoad;                // 是否从文件中载入贴图

    int             bTexture[8];
    int             m_iNumTextureBits;
    unsigned int    m_dwCurrentTexture;
    unsigned int    m_dwCurSysTexID;

    unsigned int    m_dwTotalVideoTexSize;  // 总共的显存大小
    unsigned int    m_dwUsedVideoTexSize;   // 总共使用的显存大小

    std::list<_TexItem*>  m_listUsedTexture;      // 当前使用的贴图
    std::list<_TexItem*>  m_listUnusedTexture;    // 已经没有使用的贴图，但是没有从物理删除
    std::list<_TexItem*>  m_listEmptyTexture;     // 空的的贴图

    std::list<_TexIFL*>   m_listUsedIFL;          // 当前使用的贴图
    //std::list<_TexIFL*>   m_listUnusedIFL;        // 已经没有使用的贴图，但是没有从物理删除
    std::list<_TexIFL*>   m_listEmptyIFL;         // 空的的贴图

    DWORD           m_dwIncreaseRes;
    EXT_SPACE::unordered_map<unsigned int, unsigned int>    m_mapRes;

    friend RtgDevice;
    friend _TexItem;
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/


//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_texture_manager.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtgTextureManager_H_
#define _RtgTextureManager_H_

#include "core/rt2_core.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "rtg_thread_lock.h"

namespace rt_graph {

    /*
      texture fill function prototype
     */
    typedef void (*RtgTextFill)(int, int, void*, int, void*, int);

    /*
      utility description
     */
    enum
    {
        D3DX_UI_MIPLEVEL = 1,
        Text_File_Path_Size = 200
    };

    /* texture lock flag */
    enum
    {
        Text_Lock_Read  = 1 << 0,
        Text_Lock_Write = 1 << 1,
        Text_Lock_All = 0xffffffff
    };

    /*
      description of texture source from
     */
    typedef enum _RtgTextSource 
    {
        Text_Src_File = 0,
        Text_Src_File_Memery,
        Text_Src_ARGB_Data,
        Text_Src_Dynamic,
        Text_Src_Sequence

    } RtgTextSource;

    /*
      texture
     */
    typedef struct _RtgTextItem
    {
        RtgTextSource textSrc;
        //char          fileName[MAX_PATH];
		CM_MEMDEF(fileName, MAX_PATH)
        UINT          width;
        UINT          height;
        UINT          mipLevels;
        D3DFORMAT     format;
        D3DPOOL       poolType;
        DWORD         Usage;
        int           Ref;  
        UINT          dataSize;
        void*         pData;
        bool          bLock;
        RtObjState    state;
        IDirect3DTexture9* pText;

		_RtgTextItem()
		{
			CM_MEMPROTECTOR(fileName, MAX_PATH)
		}
		_RtgTextItem(const _RtgTextItem &object)
		{
			memcpy(this, &object, sizeof(_RtgTextItem));
			CM_MEMPROTECTOR(fileName, MAX_PATH)
		}
		~_RtgTextItem()
		{
			CM_MEMUNPROTECTOR(fileName)
		}
    } RtgTextItem;

    /*
      sequence texture
     */
    typedef struct _RtgSeqTextItem
    {
        enum 
        {
            SetText_Max_Text_Num = 64
        };

        RtgTextSource textSrc;
        //char          fileName[MAX_PATH];
		CM_MEMDEF(fileName, MAX_PATH)
        UINT          width;
        UINT          height;
        UINT          mipLevels;
        D3DFORMAT     format;
        D3DPOOL       poolType;
        DWORD         Usage;
        int           Ref;  
        UINT          dataSize;
        void*         pData;
        bool          bLock;
        RtObjState    state;
        IDirect3DTexture9* pText;

        RtgTextItem* apText[SetText_Max_Text_Num];
        float   interFrame[SetText_Max_Text_Num]; 
        float   lastUpdateTime;
        int     frameNum;
        int     curFrame;

		_RtgSeqTextItem()
		{
			CM_MEMPROTECTOR(fileName, MAX_PATH)
		}
		_RtgSeqTextItem(const _RtgSeqTextItem &object)
		{
			memcpy(this, &object, sizeof(_RtgSeqTextItem));
			CM_MEMPROTECTOR(fileName, MAX_PATH)
		}
		~_RtgSeqTextItem()
		{
			CM_MEMUNPROTECTOR(fileName)
		}
	} RtgSeqTextItem;

    /*
      description of texture lock
     */
    typedef struct _RtgTextLock
    {
        RtgTextItem* texItem;
        IDirect3DTexture9* texLock;
        UINT   width;
        UINT   height;
        UINT   pitch;
        DWORD  flag;
        void* data;
    } RtgTextLock;

    /* 
      texture manager, allocate, fill, release the texture
     */
    class RtgTextureManager
    {
    public :

        enum 
        {
            Text_Catch_Size = 32,
            Text_Load_PreFrame = 1
        };

        bool Init();
        bool FrameMove();
        
        IDirect3DTexture9* GetD3dTexture(RtgTextItem* texItem);
        IDirect3DSurface9* GetFrameTargt(RtgTextItem* texItem);
        IDirect3DSurface9* GetDepthTargt(RtgTextItem* texItem);

        RtgTextItem* CreateTextureFromFile_Direct(
            const char filePath[],
            UINT       width  = D3DX_DEFAULT,
            UINT       height = D3DX_DEFAULT,
            UINT       mipLevels = D3DX_DEFAULT,
            D3DFORMAT  format = D3DFMT_FROM_FILE,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage  = 0,
            BOOL       bForceLoad = false
            );

        RtgTextItem* CreateTextureFromFile(
            const char fileName[], 
            UINT       width  = D3DX_DEFAULT, 
            UINT       height = D3DX_DEFAULT, 
            UINT       mipLevels = D3DX_DEFAULT,
            D3DFORMAT  format = D3DFMT_FROM_FILE,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage = 0,
            BOOL       bForceLoad = false
            );

        RtgTextItem* CreateTexture(
            UINT       width, 
            UINT       height, 
            UINT       mipLevels = 1,
            D3DFORMAT  format = D3DFMT_A8R8G8B8,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage = 0
            );

        RtgTextItem* CreateTextureFromFileMemery(
            PVOID      pData,
            UINT       dataSize,
            UINT       width = D3DX_DEFAULT, 
            UINT       height = D3DX_DEFAULT, 
            UINT       mipLevels = D3DX_DEFAULT,
            D3DFORMAT  format = D3DFMT_FROM_FILE,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage = 0
            );

        RtgTextItem* CreateTextureFromArchive(
            RtArchive* pArchive,
            UINT       width = D3DX_DEFAULT, 
            UINT       height = D3DX_DEFAULT, 
            UINT       mipLevels = D3DX_DEFAULT,
            D3DFORMAT  format = D3DFMT_FROM_FILE,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage = 0
            );

        RtgTextItem* CreateTextureFromARGBData(
            PVOID      pData,
            UINT       dataSize,
            UINT       width, 
            UINT       height, 
            UINT       mipLevels = 1,
            D3DFORMAT  format = D3DFMT_A8R8G8B8,
            D3DPOOL    poolType = D3DPOOL_MANAGED,
            DWORD      Usage = 0
            );

        RtgTextItem* CreateDynamicTexture(
            UINT       width, 
            UINT       height,
            D3DFORMAT  format = D3DFMT_A8R8G8B8
            );

        RtgTextItem* CreateFrameBufferTexture(
            UINT       width, 
            UINT       height,
            D3DFORMAT  format = D3DFMT_A8R8G8B8
            );

        RtgTextItem* CreateDepthBufferTexture(
            UINT       width, 
            UINT       height,
            D3DFORMAT  format = D3DFMT_D24S8
            );

        RtgTextItem* CreateUiTexture(
            const char fileName[], 
            UINT       width  = D3DX_DEFAULT, 
            UINT       height = D3DX_DEFAULT
            );

        void ReleaseTexture(RtgTextItem* textItem);
        
        void AddTextSearchPath(const char szPath[]);
        bool GetTextFilePath(char textPath[], const char textName[]);  
        
        UINT QueryFreeTextMemory();
        bool IsDynamicTexture(RtgTextItem* texItem);
        bool NativeLockable(RtgTextItem* texItem);
        void DumpTextInfo(const char fileName[]);
        bool Save(RtgTextItem* texItem, const char* fileName);

        bool Lock(RtgTextLock* texLock, RtgTextItem* texItem);
        bool Unlock(RtgTextLock* texLock);

        bool UpdateTexture(RtgTextItem* texItem, void* data);

        void OnLostDevice();
        void OnResetDevice();

        bool SetTextureQuality(int _Quality);
        int  GetTextureQuality();

        D3DXIMAGE_FILEFORMAT GetTextFileFmt(const char fileName[]);
        RtgTextFill GetTextFill(RtgTextItem* texItem);
        RtgTextItem* GetChessboardText();

        RtgTextureManager(IDirect3DDevice9* pd3dDevice);
        virtual ~RtgTextureManager();

        size_t GetLoadQueueSize() const { return m_loadQueue.size(); }
        size_t GetCratQueueSize() const { return m_cratQueue.size(); }

        void EnableMutiThreadLoad(bool bEnable = true)
        {
            m_bMutiThreadLoad = bEnable;
        }

    protected :
        bool CreateTexture(RtgTextItem* textItem);
        bool IsSequenceTextFile(const char fileName[]);
        void DeleteTexture(RtgTextItem* texItem);
        static void Load_Thread(void* _ptMgr);

		// Added by Wayne Wong 2011-01-04
		void CheckMipLevels( RtgTextItem* texItem );

    protected :
        IDirect3DDevice9*           m_pDevice;
        map<string, RtgTextItem*>   m_mapText;
        set<RtgTextItem*>           m_setText;
        vector<RtgTextItem*>        m_vecResetText;
        list<RtgTextItem*>          m_listCatch;
        vector<string>              m_vecPath;
        RtgTextItem *               m_texChessboard;
        bool                        m_bExit;
        bool                        m_bExited;
        list<RtgTextItem*>          m_loadQueue;
        list<RtgTextItem*>          m_cratQueue;
        thread_lock                 m_loadLock;
        thread_lock                 m_cratLock;
        bool                        m_bMutiThreadLoad;

    };

}

#endif
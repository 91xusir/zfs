
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

typedef unsigned int textId;
#define ErrorTextId (-1)

typedef enum _TextMemType
{
    TextMem_VM = 0,
    TextMem_MM,
    TextMem_AM,
    TextMem_SM,
    TextMem_Type_Count
} TextMemType;

typedef enum _PixelFormat
{
    Pixel_ARGB = 0,
    Pixel_RGB,
    Pixel_DXT,
    Pixel_X8,
    PixelFormat_Count
} PixelFormat;

enum 
{
    Default_Text_Size = 0
};

class RtgTextureManager
{
public :
    bool Init(IDirect3DDevice9* pDevice);

    textId CreateTexture(const char szFileName[], int width, int height, bool bMip = true);
    textId CreateTexture(int width, int height, TextMemType memType, PixelFormat pFmt);
    void   DeleteTexture(textId tId);

    void*  Lock(textId _tId);
    void   Unlock(textId _tId);
    bool   SetTextureData(void* data, int width, int height, PixelFormat srcFmt);

    RtgTextureManager();
    ~RtgTextureManager();

protected :
    void Resize(TextMemType tmt);

protected :
    IDirect3DDevice9*          m_pDevice;
    vector<IDirect3DTexture9*> m_vecText[TextMem_Type_Count];
    vector<WORD>               m_FreeIdx[TextMem_Type_Count];
    map<string, textId>        m_mapText;
};

#endif
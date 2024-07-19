
#include "rtg_graph_inter.h"
#include "rtg_image_bmp.h"
namespace rt_graph {

int RtgTexture::m_iMaxMultiTextures = 0;

enum
{
	///!��ͼ����ܹ�ʹ�õ��Դ�����,���ǵ���ͼ�ƺ��ܶ࣬���Ѿ��Թ�64M,�����е���ͼ����
	MAX_TEXTURE_VIDEO_MEMORY = 120 * 1024 * 1024
};

typedef std::list<_TexItem*>   _TexItemList;

enum
{
    ///!��ʹ�õ���ͼת��δʹ�õ���ͼ
	TEXTURE_USED_TO_UNUSED  = 0,
	///!��δʹ�õ���ͼת��ʹ�õ���ͼ
	TEXTURE_UNUSED_TO_USED  = 1,
	///!�ӿյ���ͼת��ʹ�õ���ͼ
	TEXTURE_EMPTY_TO_USED   = 2,
	//!��ʹ�õ���ͼת��յ���ͼ
	TEXTURE_USED_TO_EMPTY   = 3,
	///!��δʹ�õ���ͼת��յ���ͼ
	TEXTURE_UNUSED_TO_EMPTY = 4,

};

bool  IsInTextureList( _TexItemList& paTextureList , _TexItem* pkTextureItem )
{
	std::list<_TexItem*>::iterator it;
	for (it=paTextureList.begin(); it!=paTextureList.end(); it++)
	{
		_TexItem*  pkCurrent = *it;
		if( pkCurrent == pkTextureItem )
		{
			return true;			
		}
	}
	return false;
}


int  CheckTextureUsedVedioMemory( _TexItemList& paUsedTexture   ,_TexItemList& paUnUsedTexture )						           
{
	int  nVideoMemory = 0;
	std::list<_TexItem*>::iterator it;
	for (it=paUsedTexture.begin(); it!=paUsedTexture.end(); it++)
	{
		_TexItem*  pkCurrent = *it;
		if( pkCurrent->pTexture )
		{
			nVideoMemory += pkCurrent->GetDataSize();
		}
	}
	for (it=paUnUsedTexture.begin(); it!=paUnUsedTexture.end(); it++)
	{
		_TexItem*  pkCurrent = *it;
		if( pkCurrent->pTexture )
		{
            nVideoMemory += pkCurrent->GetDataSize();
		}		
	}
	return nVideoMemory;
}


void  TextureListTransform( _TexItemList& paEmptyTexture  ,
						    _TexItemList& paUsedTexture   ,
							_TexItemList& paUnUsedTexture ,
							_TexItem*     pkTextureItem   ,
							short         nTransformType )
{
    if( NULL == pkTextureItem ) return;
#ifdef _DEBUG

	bool bIsInUsed   = IsInTextureList( paUsedTexture   , pkTextureItem );
	bool bIsInUnUsed = IsInTextureList( paUnUsedTexture , pkTextureItem );
	bool bIsInEmpty  = IsInTextureList( paEmptyTexture  , pkTextureItem );
	int nTotalReal   = CheckTextureUsedVedioMemory( paUsedTexture , paUnUsedTexture );	
	switch( nTransformType )
	{
	case TEXTURE_USED_TO_UNUSED:
		{
            CHECK( true  == bIsInUsed   );
			CHECK( false == bIsInUnUsed );
			CHECK( false == bIsInEmpty  );
		}
		break;
	case TEXTURE_UNUSED_TO_USED:
		{
			CHECK( false  == bIsInUsed   );
			CHECK( true   == bIsInUnUsed );
			CHECK( false  == bIsInEmpty  );			
		}
		break;
	case TEXTURE_EMPTY_TO_USED:
		{
			CHECK( false  == bIsInUsed   );
			CHECK( false  == bIsInUnUsed );
			CHECK( true   == bIsInEmpty  );			
		}
		break;
	case TEXTURE_USED_TO_EMPTY:
		{
			CHECK( true   == bIsInUsed   );
			CHECK( false  == bIsInUnUsed );
			CHECK( false  == bIsInEmpty  );	
		}
		break;
	default:
		{
            return;
		}
		break;
	}
#endif
	switch( nTransformType )
	{
	case TEXTURE_USED_TO_UNUSED:
		{
            paUsedTexture.remove( pkTextureItem );
			paUnUsedTexture.push_back( pkTextureItem );
		}
		break;
	case TEXTURE_UNUSED_TO_USED:
		{
			paUnUsedTexture.remove( pkTextureItem );
			paUsedTexture.push_back( pkTextureItem );
		}
		break;
	case TEXTURE_EMPTY_TO_USED :
		{
			paEmptyTexture.remove( pkTextureItem );
			paUsedTexture.push_back( pkTextureItem );
		}
		break;
	case TEXTURE_USED_TO_EMPTY:
		{
			paUsedTexture.remove( pkTextureItem );
			paEmptyTexture.push_back( pkTextureItem );
		}
		break;
	case TEXTURE_UNUSED_TO_EMPTY:
		{
			paUnUsedTexture.remove( pkTextureItem );
			paEmptyTexture.push_back( pkTextureItem );
		}
		break;
	}
}

_TexItem::_TexItem()
{
	CM_MEMPROTECTOR(szFileName, MAX_TEX_FILE_NAME)

    pDevice = NULL;
    pTexture = NULL;
    pSrcRGBData = NULL;
    Reset();
}

_TexItem::_TexItem(const _TexItem &object)
{
	memcpy(this, &object, sizeof(_TexItem));
	CM_MEMPROTECTOR(szFileName, MAX_TEX_FILE_NAME)
}

_TexItem::~_TexItem()
{
	CM_MEMUNPROTECTOR(szFileName)

    Reset();
}

void _TexItem::Reset()
{
    // ���������� "Pure virtual function call" ����
    //     ��˵���е���ͼû���ڳ��������ʱ�������
    //     �Ӷ�������ϵͳ����Ժ�����������
    //     ����ķ���������ϵͳ����֮ǰ���������Դ
    if (pTexture)
    {
        CHECK(pDevice!=NULL);
        RTASSERT(false);
        //   pDevice->m_Texture.ReleaseTextureVideoMem(this);
    }
    if (pSrcRGBData)
    {
        RtCoreMem().Free(pSrcRGBData);
        pSrcRGBData = NULL;
    }
    dwResID = 0;
    iPathIdx = 0;
    szFileName[0] = '\0';
    iRefCount = 0;
    bUseOffScreen = false;
    cUse = 0;
    cFormat = 0;
    bLastRender = false;
    bQualityManage = true;
    iUseless = 0;
    pTexture = NULL;
    pOffScreen = NULL;
    pSrcRGBData = NULL;
    iSrcRGBDataSize = 0;
    iWidth = iHeight = 0;
    iMipMapCount = 0;
}

int _TexItem::GetDataSize()
{
    int iSize = 0;
    int iAccum = iWidth * iHeight * 4;
    for (int i=0; i<iMipMapCount ; i++ )
    {
        iSize += iAccum;
        iAccum /= 4;
    }	
    return iSize;
}

_TexIFL::_TexIFL()
{
	CM_MEMPROTECTOR(szFileName, MAX_TEX_FILE_NAME)

    pDevice = NULL;
    iPathIdx = 0;
    szFileName[0] = '\0';
    iRefCount = 0;
    iIflNumTextures = 0;
    pTextureID = 0;
    iCurrentTexture = 0;
    fTimeAccum = 0.0f;
    fTimeToChange = 0.033f;
}

_TexIFL::_TexIFL(const _TexIFL &object)
{
	memcpy(this, &object, sizeof(_TexIFL));
	CM_MEMPROTECTOR(szFileName, MAX_TEX_FILE_NAME)
}

_TexIFL::~_TexIFL()
{
	CM_MEMUNPROTECTOR(szFileName)

    iRefCount = 1;
    Release();
}

void _TexIFL::AddRef()
{
    iRefCount++;
}

void _TexIFL::Release()
{
    CHECK(pDevice!=NULL);
    iRefCount--;
    if ( iRefCount<=0 )
    {
        for ( int i=0; i<iIflNumTextures; i++ )
        {
            RTASSERT(false);
           // pDevice->m_Texture.DeleteTexture(pTextureID[i]);
        }
        RtCoreMem().Free(pTextureID);
        iPathIdx = 0;
        szFileName[0] = '\0';
        iRefCount = 0;
        iIflNumTextures = 0;
        pTextureID = 0;
        iCurrentTexture = 0;
        fTimeAccum = 0.0f;
        fTimeToChange = 0.033f;
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//void RtgTexStage::SetTexture(RtgDevice& inDevice, const char *szFilename, RTGTexBitType eTexBitType, bool bCreateNow, int iMipmap, BYTE cFormat, bool bQualityManage)
//{
//    if (dwTextureID)
//    {
//        inDevice.m_Texture.DeleteTexture(dwTextureID);
//        dwTextureID = 0;
//    }
//    SetTexture(inDevice.m_Texture.CreateTexture(szFilename, eTexBitType, bCreateNow, iMipmap, cFormat, bQualityManage));
//}

//void RtgTexStage::SetTexture(unsigned int dwTexID)
//{
//    dwTextureID = dwTexID;
//#ifdef _DEBUG
//    /*
//    if (dwTextureID)
//    {
//        pTex = pDevice->m_Texture.GetTextureItem(dwTextureID);
//        CHECK(pTex!=NULL);
//    }else
//    {
//        pTex = NULL;
//    }
//    */
//#endif
//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

RtgTexture::RtgTexture(RtgDevice* pDevice)
{
	CM_MEMPROTECTOR2(m_szTexturePathList, MAX_TEXTURE_PATHS, 128)
	CM_MEMPROTECTOR2(m_szFilenameExtension, MAX_FILENAME_EXTENSION, 12)

    int i;
    m_pDevice = pDevice;
    m_TextureList    = RT_NEW _TexItem[MAX_TEXTURES];
    m_IFLTextureList = RT_NEW _TexIFL[MAX_IFL_TEXTURES];
    for (i=0; i<MAX_TEXTURES; i++)
    {
        m_TextureList[i].pDevice = pDevice;
    }
    for (i=0; i<MAX_IFL_TEXTURES; i++)
    {
        m_IFLTextureList[i].pDevice = pDevice;
    }

    for (i=0; i<MAX_TEXTURE_PATHS; i++)
    {
        m_szTexturePathList[i][0] = '\0';
    }
    rt2_strcpy(m_szTexturePathList[0], ".\\");

    for (i=0; i<MAX_FILENAME_EXTENSION; i++)
    {
        m_szFilenameExtension[i][0] = '\0';
    }
	rt2_strcpy(m_szFilenameExtension[0], ".rle"); m_eFilenameExtensionBitType[0] = RTGTBT_COLOR;
	rt2_strcpy(m_szFilenameExtension[1], ".jpg"); m_eFilenameExtensionBitType[1] = RTGTBT_COLOR;
	rt2_strcpy(m_szFilenameExtension[2], ".ifl"); m_eFilenameExtensionBitType[2] = RTGTBT_AUTO;
    rt2_strcpy(m_szFilenameExtension[3], ".bmp"); m_eFilenameExtensionBitType[3] = RTGTBT_COLOR;
    rt2_strcpy(m_szFilenameExtension[4], ".tga"); m_eFilenameExtensionBitType[4] = RTGTBT_ALPHA;
    rt2_strcpy(m_szFilenameExtension[5], ".dds"); m_eFilenameExtensionBitType[5] = RTGTBT_DXT_AUTO;    
	
    m_dwIncreaseRes = 1;
    m_iNumTextureBits = 16; // ϵͳ����ɫ���
    m_dwCurrentTexture = 0;
    m_dwCurSysTexID = 0;
    m_dwTotalVideoTexSize = 0;
    m_dwUsedVideoTexSize = 0;
    m_bUseSrcDataCompress = false;
    m_bUseCustomTexManager = false;
    m_bGenerateDXT = false;
    m_bLoad = true;

    for (i=0; i<MAX_TEXTURES; i++)
    {
        m_TextureList[i].dwIdx = i+1;
        m_listEmptyTexture.push_back(m_TextureList+i);
        m_TextureList[i].cUse = 0;
    }

    for (i=0; i<MAX_IFL_TEXTURES; i++)
    {
        m_IFLTextureList[i].dwIdx = i+IFL_TAG+1;
        m_listEmptyIFL.push_back(m_IFLTextureList+i);
    }
}

RtgTexture::~RtgTexture()
{
	CM_MEMUNPROTECTOR2(m_szTexturePathList, MAX_TEXTURE_PATHS, 128)
	CM_MEMUNPROTECTOR2(m_szFilenameExtension, MAX_FILENAME_EXTENSION, 12)

	DEL_ARRAY(m_IFLTextureList);
    DEL_ARRAY(m_TextureList);
}

void RtgTexture::SetTotalTextureMem(unsigned int dwTotalMem)
{
    m_dwTotalVideoTexSize =dwTotalMem;
}

void RtgTexture::InitTexture(void)
{
    m_dwCurrentTexture = 0;
    if (m_dwTotalVideoTexSize==0)
    {
        m_dwTotalVideoTexSize = m_pDevice->GetAvailableTextureMem()/2 - 3*1024*1024;
		
		//m_dwTotalVideoTexSize = MAX_TEXTURE_VIDEO_MEMORY;

        unsigned int dwMinSize = 12*1024*1024;
        if (m_dwTotalVideoTexSize < dwMinSize)
        {
            if (dwMinSize<(unsigned int)m_pDevice->GetAvailableTextureMem())
            {
                m_dwTotalVideoTexSize = dwMinSize;
            }else
            {
                m_dwTotalVideoTexSize = m_pDevice->GetAvailableTextureMem();
            }
        }
    }
    if (m_dwTotalVideoTexSize>(unsigned int)m_pDevice->GetAvailableTextureMem())
    {
        m_dwTotalVideoTexSize = m_pDevice->GetAvailableTextureMem();
    }
}

int RtgTexture::GetTextureSize(unsigned int dwResID)
{

    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::GetTextureSize �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return 0;
    }

    if (dwIdx<1 || dwIdx>=MAX_TEXTURES)
    {
        return 0;
    }

    if (dwIdx < IFL_TAG)
    {
        _TexItem *pItem = &m_TextureList[dwIdx-1];
        return pItem->GetDataSize();
    }else
    {
        int iSize = 0;
        _TexIFL *pItem = &m_IFLTextureList[dwIdx-IFL_TAG-1];
        for ( int i=0; i<pItem->iIflNumTextures; i++ )
        {
            iSize += GetTextureSize( pItem->pTextureID[i] );
        }
        return iSize;
    }
}

int RtgTexture::TotalUsedTextureSize()
{
    int iSize = 0;
    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        iSize += (*it)->GetDataSize();
    }
    return iSize;
}

BOOL RtgTexture::SelectTextureFormat(int iBits)
{
    return m_pDevice->SelectTextureFormat(iBits);
}

char* RtgTexture::GetTexturePath(int iIndex)
{
    if (iIndex<0 || iIndex>=MAX_TEXTURE_PATHS)
        return NULL;
    return m_szTexturePathList[iIndex];
}

BOOL RtgTexture::SetTexturePath(const char *szPath, int iIndex)
{
    if ( iIndex<0 && iIndex>=MAX_TEXTURE_PATHS )
        return FALSE;
    int iLen = (int)strlen(szPath);
    char *p = m_szTexturePathList[iIndex];
    rt2_strcpy(p, szPath);
    strlwr(p);
    if ( p[iLen-1]!='\\' )
    {
        p[iLen] = '\\';
        p[iLen+1] = 0;
    }
    return TRUE;
}

void RtgTexture::TextureAnimation(float fTimeDiff)
{
    _TexIFL *pItem;
    std::list<_TexIFL*>::iterator it;
    for (it=m_listUsedIFL.begin(); it!=m_listUsedIFL.end(); it++)
    {
        pItem = *it;
        pItem->fTimeAccum += fTimeDiff;
        if ( pItem->fTimeAccum > pItem->fTimeToChange )
        {
            pItem->fTimeAccum = 0.0f;
            pItem->iCurrentTexture++;
            if ( pItem->iCurrentTexture >= pItem->iIflNumTextures )
            {
                pItem->iCurrentTexture = 0;
            }
        }
    }
}

void RtgTexture::SetTextureFilter(RTGTextureFilter eMipmap, RTGTextureFilter eMin, RTGTextureFilter eMag)
{
    m_pDevice->SetTextureFilterMip(eMipmap);
    m_pDevice->SetTextureFilterMin(eMin);
    m_pDevice->SetTextureFilterMag(eMag);
}

void RtgTexture::SetTextureQuality(int iQuality)
{
    RtgImage::m_iTextureQuality = iQuality;
    UnloadAllTexture();
}

int RtgTexture::GetTextureQuality()
{
	return RtgImage::m_iTextureQuality;
}

_TexItem* RtgTexture::GetTextureEntry(const char *szFilename, int iPathIdx)
{
    
    _TexItem *pItem;
    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        if (!strcmp(pItem->szFileName, szFilename) && pItem->iPathIdx==iPathIdx)
        {
            AddTextureRef(pItem);
            return pItem;
        }
    }

    for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
    {
        pItem = *it;
        if (!strcmp(pItem->szFileName, szFilename) && pItem->iPathIdx==iPathIdx)
        {
            AddTextureRef(pItem);
           /* m_listUsedTexture.push_back(pItem); 
            m_listUnusedTexture.remove(pItem);*/
			TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pItem,TEXTURE_UNUSED_TO_USED);
			pItem->cUse = 2;
            return pItem;
        }
    }

    if (m_listEmptyTexture.size())
    {
        pItem = *(m_listEmptyTexture.begin());        
        pItem->Reset();
        rt2_strncpy(pItem->szFileName, szFilename, MAX_TEX_FILE_NAME);
        pItem->szFileName[MAX_TEX_FILE_NAME-1] = 0;
        pItem->iPathIdx = iPathIdx;

		//m_listEmptyTexture.pop_front();
        //m_listUsedTexture.push_back(pItem);
		TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pItem,TEXTURE_EMPTY_TO_USED );
		pItem->cUse = 2;
        pItem->dwResID = AllocID(pItem->dwIdx);
        return pItem;
    }

    RtCoreLog().Error("����: ��ͼ����̫��(���ֵ: %d).\n", MAX_TEXTURES);
    return NULL; // �Ѿ�����
}

_TexIFL* RtgTexture::GetIFLTextureEntry(const char *szFilename, int iPathIdx)
{

    _TexIFL *pItem;
    std::list<_TexIFL*>::iterator it;
    for (it=m_listUsedIFL.begin(); it!=m_listUsedIFL.end(); it++)
    {
        pItem = *it;
        if (!strcmp(pItem->szFileName, szFilename) && pItem->iPathIdx==iPathIdx)
        {
            pItem->AddRef();
            return pItem;
        }
    }

    if (m_listEmptyIFL.size())
    {
        pItem = *(m_listEmptyIFL.begin());
        rt2_strncpy(pItem->szFileName, szFilename, MAX_TEX_FILE_NAME);
        pItem->szFileName[MAX_TEX_FILE_NAME-1] = 0;
        pItem->iPathIdx = iPathIdx;
        m_listEmptyIFL.pop_front();
        m_listUsedIFL.push_back(pItem);
        pItem->dwResID = AllocID(pItem->dwIdx);
        return pItem;
    }

    RtCoreLog().Error("����: IFL��ͼ����̫��(���ֵ: %d).\n", MAX_IFL_TEXTURES);
    return NULL; // �Ѿ�����
}

unsigned int RtgTexture::CreateIFLTexture(const char *szFilename, const char *szPathname, int iPathIdx, RTGTexBitType eTexBitType, bool bCreateNow, int iMipmap, bool bQualityManage)
{
	funguard;
    _TexIFL *pIFLTexture;
    pIFLTexture = GetIFLTextureEntry(szFilename, iPathIdx);
    if (pIFLTexture==NULL)
    {
        // �Ѿ�����
        return 0;
    }
    if (pIFLTexture->iRefCount)
    {
        return pIFLTexture->dwResID;
    }
    pIFLTexture->AddRef();

    std::vector< std::string > FileList;


    // �����ļ�
    RtArchive* pFile = RtCoreFile().CreateFileReader(szPathname, 0);
	//delete szPathname;
	szPathname = NULL;
    if (pFile==NULL) return 0;
    int iSize = pFile->TotalSize();
    char* pBuffer = (char*)RtCoreMem().Alloc(iSize+1, __FILE__, __LINE__);
    if (pBuffer==NULL)
    {
        pFile->Close();
        return 0;
    }
    pFile->Serialize(pBuffer, iSize);
    pBuffer[iSize] = 0;
    pFile->Close();

    char step[] = "\n\r\0";
    char *str = strtok(pBuffer, step);
    while(str)
    {
        FileList.push_back(str);
        str=strtok(NULL, step);
    }

    char szTexFilePath[512];
    char szFullname[512];
    rtfSplitpath(szFilename, NULL, szTexFilePath, NULL, NULL);

    pIFLTexture->iIflNumTextures = (int)FileList.size();
    pIFLTexture->pTextureID = (unsigned int*)RtCoreMem().Alloc(sizeof(unsigned int)*(pIFLTexture->iIflNumTextures), __FILE__, __LINE__);
    for ( int i=0; i<pIFLTexture->iIflNumTextures; i++ )
    {
        const char *szFile = FileList[i].c_str();
        rt2_strcpy(szFullname, szTexFilePath);
        rt2_strcat(szFullname, szFile);
        pIFLTexture->pTextureID[i] = CreateTexture(szFullname, eTexBitType, bCreateNow, iMipmap, 0, bQualityManage);
    }
    RtCoreMem().Free(pBuffer);
    pIFLTexture->AddRef();
    return pIFLTexture->dwResID;
	fununguard;
}

char* RtgTexture::GetTextureFilePath(const char* szFilename, int &iPathIdx, int &iExtIdx)
{
    //char * szBuffer = RT_NEW char[256];
	//static char  szBuffer[ MAX_PATH ];
	S_MEMDEF(szBuffer, MAX_PATH)
	S_MEMPROTECTOR(szBuffer, MAX_PATH, bMP)
	memset( szBuffer , NULL , MAX_PATH );
    for ( int i=0; i<MAX_TEXTURE_PATHS; i++ )
    {
        if ( m_szTexturePathList[i][0] )
        {
#ifdef _DEBUG
            if (strlen(szFilename)>=MAX_TEX_FILE_NAME-1)
            {
                RtCoreLog().Error("����: ��ͼ�ļ���̫�� [%s] %d\n", szFilename, strlen(szFilename));
            }
#endif
            iPathIdx = i;
            for (int j=0; j<MAX_FILENAME_EXTENSION; j++)
            {
                if (m_szFilenameExtension[j][0])
                {
                    rt2_strcpy( szBuffer, m_szTexturePathList[i]);
                    rt2_strcat( szBuffer, szFilename);
                    rt2_strcat( szBuffer, m_szFilenameExtension[j]);
                    if (RtCoreFile().FileExist(szBuffer))
                    {
                        iExtIdx = j;
                        return szBuffer;
                    }
                }
            }
        }
    }
    return NULL;
}

// ȡ����ͼ�Ŀ�λ, û�п�λ���ͷ�һЩ��ͼ
_TexItem* RtgTexture::GetTextureEntryEx(const char *szFilename, int iPathIdx)
{
//#ifdef MULTITHREAD
	//m_lock.Lock();
//#endif
    // ��ͼ�������࣬�������Ҫ����
	///!�������ڴ���һֱ����������ͼ���ܾ�û��ʹ�õ���ͼ���ɾ��
	int  nUsedTextureNumber   = m_listUsedTexture.size();
	int  nUnUsedTextureNumber = m_listUnusedTexture.size();
    while ( ( nUsedTextureNumber + nUnUsedTextureNumber ) >=MAX_TEXTURES  )
    {
		nUsedTextureNumber   = m_listUsedTexture.size();
		nUnUsedTextureNumber = m_listUnusedTexture.size();
        if (!PhysicsDeleteMinimumCostTexture())
        {
            RtCoreLog().Error("����: ͬʱʹ����ͼ����̫��(%d), ���޸ĳ������߳����Ա����������.\n", MAX_TEXTURES);
            break;
        }
    }

    // ȡ����ͼ�Ŀ�λ�������ͼ�Ѿ����ھ�ֱ�ӷ���ID
    //     ע�⣬��ͬĿ¼��ͬ�ļ�������ͼ�����������ͬ����ͼ
    
     _TexItem *pItem = GetTextureEntry(szFilename, iPathIdx);
	 
//#ifdef MULTITHREAD 
	 //m_lock.Unlock();	 
//#endif	
	 return pItem;
}

// ����ɾ����С���۵���ͼ
//    ��ɾ�� Unused ��ɾ�� Used
bool RtgTexture::PhysicsDeleteMinimumCostTexture()
{
    _TexItem *pItem, *pSelect;
    std::list<_TexItem*>::iterator it;
    int iUseless, iRef;

    // ɾ��Unused
    pSelect = NULL;
    for (iRef=0; iRef<10; iRef++) // �������ô�����ɾ������ɾ�����ô���С��
    {
        iUseless = -1;
        for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
        {
            pItem = *it;
            if (pItem->iRefCount==iRef && pItem->iUseless > iUseless && pItem->iPathIdx>=0)
            {
                iUseless = pItem->iUseless;
                pSelect = pItem;
            }
        }
        if (pSelect)
        {
            break;
        }
    }

    if (pSelect)
    {
		if(pSelect->iRefCount >0)
			RtCoreLog().Error("ɾ����������Ϊ%d����ͼ/n",pSelect->iRefCount);
        /*m_listUnusedTexture.remove(pSelect);
        m_listEmptyTexture.push_back(pSelect);*/
		TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pSelect,TEXTURE_UNUSED_TO_EMPTY );

        pSelect->cUse = 0;
        ReleaseID(pSelect->dwResID);
        pSelect->Reset();
        return true;
    }

    // ɾ������һ��Unused
    for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
    {
        pItem = *it;
        if (pItem->iPathIdx>=0)
        {
            pSelect = pItem;
            /*m_listUnusedTexture.remove(pSelect);
            m_listEmptyTexture.push_back(pSelect);*/
			TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pSelect,TEXTURE_USED_TO_EMPTY );
            pSelect->cUse = 0;
            ReleaseID(pSelect->dwResID);
            pSelect->Reset();
            return true;
        }
    }

    // ɾ������һ��Unused
    if (m_listUnusedTexture.size())
    {
        pSelect = *(m_listUnusedTexture.begin());

       /* m_listUnusedTexture.pop_front();
        m_listEmptyTexture.push_back(pSelect);*/

		TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pSelect,TEXTURE_USED_TO_EMPTY );

        pSelect->cUse = 0;
        ReleaseID(pSelect->dwResID);
        pSelect->Reset();
        return true;
    }

    // ɾ��Used
    pSelect = NULL;
    iUseless = -1;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        if (pItem->iUseless > iUseless && pItem->iPathIdx>=0)
        {
            iUseless = pItem->iUseless;
            pSelect = pItem;
        }
    }

    if (pSelect)
    {
       /* m_listUsedTexture.remove(pSelect);
        m_listEmptyTexture.push_back(pSelect);*/

		TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pSelect,TEXTURE_USED_TO_EMPTY );

        pSelect->cUse = 0;
        ReleaseID(pSelect->dwResID);
        pSelect->Reset();
        return true;
    }

    return false;
}

// �ͷŴ�����С���Դ�
bool RtgTexture::ReleaseVideoMemMinimumCostTexture()
{
	_TexItem *pItem, *pSelect;
	int iUseless, iRef;
	std::list<_TexItem*>::iterator it;

	// ���ͷ�Unused
	pSelect = NULL;
	for (iRef=0; iRef<10; iRef++) // �������ô�����ɾ������ɾ�����ô���С��
	{
		iUseless = -1;
		for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
		{
			pItem = *it;
			if (pItem->pTexture && pItem->iRefCount==iRef && pItem->iUseless>iUseless && pItem->iPathIdx>=0)
			{
				iUseless = pItem->iUseless;
				pSelect = pItem;
			}
		}
		if (pSelect) break;
	}
	if (pSelect)
	{
		ReleaseTextureVideoMem(pSelect);
		return true;
	}

	// �ͷ�����һ��Unused
	for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
	{
		pItem = *it;
		if (pItem->pTexture && pItem->iPathIdx>=0)
		{
			ReleaseTextureVideoMem(pItem);
			return true;
		}
	}

	// �ͷ�Used
	pSelect = NULL;
	iUseless = -1;
	for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
	{
		pItem = *it;
		if (pItem->pTexture && pItem->iUseless>iUseless && pItem->iPathIdx>=0)
		{
			iUseless = pItem->iUseless;
			pSelect = pItem;
		}
	}

	if (pSelect)
	{
		ReleaseTextureVideoMem(pSelect);
		return true;
	}

	return false;
}

unsigned int RtgTexture::AllocID(unsigned int dwIdx)
{
    unsigned int dwRes = m_dwIncreaseRes;
    m_mapRes[dwRes] = dwIdx;
    m_dwIncreaseRes ++;
    return dwRes;
}

bool RtgTexture::ReleaseID(unsigned int dwRes)
{
    EXT_SPACE::unordered_map<unsigned int, unsigned int>::iterator it = m_mapRes.find(dwRes);
    if (it!=m_mapRes.end())
    {
        m_mapRes.erase(it);
        return true;
    }
    return false;
}

//#pragma pack(push)
//#pragma pack(1)
//struct SRtgBitmapFileHeader
//{ 
//	WORD    bfType; 
//	DWORD   bfSize; 
//	WORD    bfReserved1; 
//	WORD    bfReserved2; 
//	DWORD   bfOffBits; 
//};
//
//struct SRtgBitmapInfoHeader
//{
//	DWORD  biSize; 
//	LONG   biWidth; 
//	LONG   biHeight; 
//	WORD   biPlanes; 
//	WORD   biBitCount; 
//	DWORD  biCompression; 
//	DWORD  biSizeImage; 
//	LONG   biXPelsPerMeter; 
//	LONG   biYPelsPerMeter; 
//	DWORD  biClrUsed; 
//	DWORD  biClrImportant; 
//};
//#pragma pack(pop)

unsigned int RtgTexture::CreateTextureFromArchive(RtArchive* pArc,int iLength, RTGTexBitType eTexBitType,int iMipmap,BYTE cFormat,bool bflip)
{
	char szName[40];
	_TexItem *pItem;
	RtgImage img;
	if(!rtgiLoadImageBMP(pArc,&img,bflip))
	{
		return 0;
	}

	rt2_sprintf(szName, "|SysTexMemory %X", m_dwCurSysTexID++);
	pItem = GetTextureEntryEx(szName, 0); // Ĭ��Ϊ�����ڵ�һ��Ŀ¼����
	if (pItem==NULL) return 0;
	if (pItem->iRefCount)
	{
		return pItem->dwResID;
	}
	pItem->bQualityManage = false;
	pItem->cTexBitType = (BYTE)eTexBitType;
	pItem->iWidth = img.info.iWidth;
	pItem->iHeight = img.info.iHeight;
	pItem->iMipMapCount = img.GetMipMapCount();
	pItem->cFormat = cFormat;
	AddTextureRef(pItem);
    
	unsigned int dwRequiredSize = pItem->GetDataSize();
	int iCost = 0;
	while (m_dwTotalVideoTexSize < m_dwUsedVideoTexSize+dwRequiredSize)
	{
		if (!ReleaseVideoMemMinimumCostTexture())
		{
			RtCoreLog().Error("����: ʹ�õ��Դ����[%d/%d], ����������[%d]���Դ�.\n", m_dwUsedVideoTexSize, m_dwTotalVideoTexSize, dwRequiredSize);
			break;
		}
	}

	// ������ͼ
	if (!m_pDevice->CreateTextureByImage(pItem, img))
	{
		RtCoreLog().Error("����: DX ���ܴ�����ͼ [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
		return FALSE;
	}
    
	return pItem->dwResID;
}


unsigned int RtgTexture::CreateTextureFromMemory(void *pMemoryTextureFile, int iLength, RTGTexBitType eTexBitType, int iMipmap, BYTE cFormat)
{
    char szName[40];
    _TexItem *pItem;
    int iWidth, iHeight;
	
    if (!RtgImage::Check24bBmpFormat(pMemoryTextureFile, iLength, iWidth, iHeight))
    {
        return 0; // ����һ����ȷ��24λ,bmpͼ��
    }
    rt2_sprintf(szName, "|SysTexMemory %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, 0); // Ĭ��Ϊ�����ڵ�һ��Ŀ¼����
    if (pItem==NULL) return 0;
    if (pItem->iRefCount)
    {
        return pItem->dwResID;
    }
    pItem->bQualityManage = false;
    pItem->cTexBitType = (BYTE)eTexBitType;
    pItem->iWidth = iWidth;
    pItem->iHeight = iHeight;
    pItem->iMipMapCount = (short)iMipmap;
    pItem->cFormat = cFormat;
    pItem->iSrcRGBDataSize = iLength;
    pItem->pSrcRGBData = (char*)RtCoreMem().Alloc(pItem->iSrcRGBDataSize, __FILE__, __LINE__);
    memcpy(pItem->pSrcRGBData, pMemoryTextureFile, pItem->iSrcRGBDataSize);
    AddTextureRef(pItem);
    CreateTextureVideoMem(pItem);
    return pItem->dwResID;
}

bool RtgTexture::Save2File(int texId, const char* fileName)
{
    RTASSERT(fileName);

    _TexItem* tItem = GetTextureItem(texId);

    if (!tItem)
        return false;

    if (m_pDevice->SaveTexture(tItem->pTexture, fileName))
    {
        char temp[MAX_PATH];
        rt2_strncpy(temp, fileName, sizeof(temp));
        char* t = strrchr(temp, '\\');
        if (t)
            *++t = 0;
        for (int i = 0; i < MAX_TEXTURE_PATHS; ++i)
        {
            if (stricmp(temp, m_szTexturePathList[i]) == 0)
            {
                tItem->iPathIdx = i;
                const char* t = strrchr(fileName, '\\');
                if (t)
                    ++t;
                else
                    t = fileName;
                rt2_strncpy(tItem->szFileName, t, sizeof(tItem->szFileName));
                break;
            }
        }

        return true;
    }
    return false;
}

unsigned int RtgTexture::CreateTexture(const char *szFilename, RTGTexBitType eTexBitType, bool bCreateNow, int iMipmap, BYTE cFormat, bool bQualityManage)
{

#ifdef MULTITHREAD
	m_lock.Lock();
#endif   
    int iPathIdx, iExtIdx;
    const char* szPathname;

    char ext[8];
    char szTexFileName[64];
    char szTexFilePath[128];
    char szTexPathName[256];
    _TexItem *pItem;

    _splitpath(szFilename, NULL, szTexFilePath, szTexFileName, ext);
    rt2_strcpy(szTexPathName, szTexFilePath);
    rt2_strcat(szTexPathName, szTexFileName);
    strlwr(ext);

    // ����ļ��Ƿ���ڣ���������ھ��˳�
    szPathname = GetTextureFilePath(szTexPathName, iPathIdx, iExtIdx);
    if (szPathname==NULL)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("����: ���ܴ���ͼ [%s] ��Ŀ¼ [%s].\n", szFilename, RtCoreFile().GetDefaultDirectory());
#endif
#ifdef MULTITHREAD
	    m_lock.Unlock();
#endif		
        return 0;
    }

    // �����IFL�����ļ��ʹ���IFL��ͼ
    if (!strcmp(ext, ".ifl"))
    {      
		unsigned int i = CreateIFLTexture(szFilename, szPathname, iPathIdx, eTexBitType, bCreateNow, iMipmap, bQualityManage);
#ifdef MULTITHREAD
		m_lock.Unlock();
#endif
//		
		return i;
    }

    // ȡ����ͼ�Ŀ�λ�������ͼ�Ѿ����ھ�ֱ�ӷ���ID
    //     ע�⣬��ͬĿ¼��ͬ�ļ�������ͼ�����������ͬ����ͼ
    if (eTexBitType==RTGTBT_AUTO)
    {
        eTexBitType = m_eFilenameExtensionBitType[iExtIdx];
    }
    rt2_strcat(szTexPathName, m_szFilenameExtension[iExtIdx]);
    pItem = GetTextureEntryEx(szTexPathName, iPathIdx);
    if (pItem==NULL)
	{
#ifdef MULTITHREAD
		m_lock.Unlock();
#endif		
		return 0;
	}
    if (pItem->iRefCount)
    {
#ifdef MULTITHREAD
		m_lock.Unlock();
#endif		
        return pItem->dwResID;
    }
    pItem->bQualityManage = bQualityManage;
    pItem->iMipMapCount = (short)iMipmap;
    pItem->cFormat = cFormat;
    pItem->cTexBitType = (BYTE)eTexBitType;
    /*
    pItem->bUseColorKey = ( szTexFileName[0]=='k' || szTexFileName[0]=='K' );
    */
    AddTextureRef(pItem);
    if  (bCreateNow)
    {
        CreateTextureVideoMem(pItem);
    }
#ifdef MULTITHREAD
	m_lock.Unlock(); 
#endif	

    return pItem->dwResID;
	
}

BOOL RtgTexture::CreateTextureEntity(_TexItem* pItem)
{
	/*m_lock.Lock();*/

    RtgImage img;

    if (!pItem->pSrcRGBData)
    {
        char szFullname[512];
        rt2_strcpy( szFullname, m_szTexturePathList[pItem->iPathIdx] );
        rt2_strcat( szFullname, pItem->szFileName);

		if (!m_bLoad)
		{
			return FALSE;
		}

        // ��ȡ�ļ�������ļ���ȡʧ�ܾ�ֱ�ӷ���
        if (!img.Load(szFullname))
        {
#ifdef _DEBUG
            RtCoreLog().Warn("����: ����������ͼ�ļ�[%s], ��Ŀ¼[%s].\n", szFullname, RtCoreFile().GetDefaultDirectory());
#endif
            return FALSE;
        }
    }else
    {
        if (!img.LoadFromMemory(pItem->pSrcRGBData, pItem->iSrcRGBDataSize))
        {
#ifdef _DEBUG
            RtCoreLog().Warn("����: ���������ڴ���ͼ�ļ�.\n");
#endif
            return FALSE;
        }
    }

    /*
    if (pItem->bUseColorKey)
    {
        img.ColorKeyInImage();
    }
    */
    img.FixTextureQuality(pItem->bQualityManage);

    pItem->iWidth       = img.info.iWidth;
    pItem->iHeight      = img.info.iHeight;
    if (pItem->iMipMapCount==0) pItem->iMipMapCount = img.GetMipMapCount();
    else if (pItem->iMipMapCount>img.GetMipMapCount()) pItem->iMipMapCount = img.GetMipMapCount();

    // �����ǰʣ��Ŀռ䲻�����������ͼ���ͼ����ͷŴ�����С����ͼ
    unsigned int dwRequiredSize = pItem->GetDataSize();
    int iCost = 0;
    while (m_dwTotalVideoTexSize < m_dwUsedVideoTexSize+dwRequiredSize)
    {
        if (!ReleaseVideoMemMinimumCostTexture())
        {
            RtCoreLog().Error("����: ʹ�õ��Դ����[%d/%d], ����������[%d]���Դ�.\n", m_dwUsedVideoTexSize, m_dwTotalVideoTexSize, dwRequiredSize);
            break;
        }
    }

    // ������ͼ
    if (!m_pDevice->CreateTextureByImage(pItem, img))
    {
        RtCoreLog().Error("����: DX ���ܴ�����ͼ [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
        return FALSE;
    }

    if (m_bUseCustomTexManager)
    {
        // ���ﴴ��������ڴ�

        if (m_bUseSrcDataCompress)
        {
            // ѹ���ڴ�����
        }
    }

	/*m_lock.Unlock();*/

    return TRUE;
}

unsigned int RtgTexture::CreateTexture(int iWidth, int iHeight, DWORD fmt, bool bCreateNow)
{
    char szName[40];
    _TexItem *pItem;
    rt2_sprintf(szName, "|SysTexManager %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, -5);
    if (pItem==NULL) return 0;
    if (pItem->iRefCount)
    {
        return pItem->dwResID;
    }
    pItem->bQualityManage = false;
    pItem->iWidth = iWidth;
    pItem->iHeight = iHeight;
    pItem->iMipMapCount = 1;
    pItem->cFormat = fmt;
    AddTextureRef(pItem);
    if  (bCreateNow)
    {
        CreateTextureVideoMem(pItem);
    }
    return pItem->dwResID;
}


unsigned int RtgTexture::CreateTexture(int iWidth, int iHeight, void* argb)
{
    char szName[40];
    _TexItem *pItem;
    
    rt2_sprintf(szName, "|SysTexManager %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, -5);
    
    if (pItem==NULL) 
        return 0;
    
    if (pItem->iRefCount)
        return pItem->dwResID;

    pItem->bQualityManage = false;
    pItem->iWidth = iWidth;
    pItem->iHeight = iHeight;
    pItem->iMipMapCount = 1;
    pItem->cFormat = RTG_TEXTURE_A8R8G8B8;
    
    AddTextureRef(pItem);
    CreateTextureVideoMem(pItem);

    TexLock texLock;
    
    if (LockTexture(pItem->dwResID, texLock))
    {
        memcpy(texLock.pBits, argb, texLock.iPitch * texLock.iHeight);
        UnlockTexture(texLock);
    }

    return pItem->dwResID;
}

unsigned int RtgTexture::CreateTextureDynamic(int iWidth, int iHeight, DWORD fmt, bool bCreateNow)
{
    char szName[40];
    _TexItem *pItem;
    rt2_sprintf(szName, "|SysTexDynamic %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, -1);
    if (pItem==NULL) return 0;
    if (pItem->iRefCount)
    {
        return pItem->dwResID;
    }
    pItem->bQualityManage = false;
    pItem->iWidth = iWidth;
    pItem->iHeight = iHeight;
    pItem->iMipMapCount = 1;
    pItem->cFormat = fmt;
    AddTextureRef(pItem);
    if  (bCreateNow)
    {
        CreateTextureVideoMem(pItem);
    }
    return pItem->dwResID;
}

unsigned int RtgTexture::CreateTextureRenderTarget(int iWidth, int iHeight, bool bUseOffScreen /*= false*/ , int nSurfaceFormat /*= 1*/ )
{
    char szName[40];
    _TexItem *pItem;
    rt2_sprintf(szName, "|SysTexRenderTarget %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, -2);
    if (pItem==NULL) return 0;
    if (pItem->iRefCount)
    {
        return pItem->dwResID;
    }
    pItem->bQualityManage = false;
    pItem->iWidth = iWidth;
    pItem->iHeight = iHeight;
    pItem->iMipMapCount = 1;
    pItem->bUseOffScreen = bUseOffScreen;
	pItem->cFormat       = nSurfaceFormat;
    AddTextureRef(pItem);
    CreateTextureVideoMem(pItem);
    return pItem->dwResID;
}

unsigned int RtgTexture::CreateTextureDepthStencil(int width ,int height)
{
    //CHECKEX("Ŀǰ��֧�ֺ��� CreateTextureDepthStencil");
    char szName[40];
    _TexItem *pItem;
    rt2_sprintf(szName, "|SysTexDepthStencil %X", m_dwCurSysTexID++);
    pItem = GetTextureEntryEx(szName, -3);
    if (pItem==NULL) return 0;
    if (pItem->iRefCount)
    {
        return pItem->dwResID;
    }
    pItem->bQualityManage = false;
    pItem->iWidth = width;
    pItem->iHeight = height;
    pItem->iMipMapCount = 1;
    pItem->cFormat = RTG_TEXTURE_D24;
    AddTextureRef(pItem);
    CreateTextureVideoMem(pItem);

    return pItem->dwResID;
}

char* RtgTexture::GetTextureName(unsigned int dwResID)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::GetTextureName �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return NULL;
    }
    if (dwIdx<1 || dwIdx>MAX_TEXTURES)
        return NULL;
    _TexItem *pItem = &m_TextureList[dwIdx-1];
    return pItem->szFileName;
}

_TexItem* RtgTexture::GetTextureItem(unsigned int dwResID)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::GetTextureItem �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return 0;
    }
    if (dwIdx < IFL_TAG)
    {
        if (dwIdx<1 || dwIdx>MAX_TEXTURES)
            return NULL;
        return &m_TextureList[dwIdx-1];
    }else
    {
        if ( dwIdx > IFL_TAG + MAX_IFL_TEXTURES )
            return NULL;
        _TexIFL *pIflItem = &m_IFLTextureList[dwIdx-IFL_TAG-1];
        unsigned int iTexID = pIflItem->pTextureID[pIflItem->iCurrentTexture];
        dwIdx = FindIdx(iTexID);
        if (dwIdx==0)
        {
#ifdef _DEBUG
            RtCoreLog().Warn("RtgTexture::GetTextureItem �Ҳ���IDΪ%d����ͼ.\n", dwResID);
#endif
            return NULL;
        }
        return &m_TextureList[dwIdx-1];
    }
}

BOOL RtgTexture::DeleteTexture(unsigned int dwResID)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::DeleteTexture �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return FALSE;
    }
    if (dwIdx < IFL_TAG)
    {
        if (dwIdx<1 || dwIdx>MAX_TEXTURES)
            return FALSE;

        _TexItem *pItem = &m_TextureList[dwIdx-1];
        CHECK(pItem->cUse!=0);
        ReleaseTextureRef(pItem);
        if (pItem->cUse==2)
        {
            if (pItem->iRefCount<=0)
            {
                /*m_listUsedTexture.remove(pItem);
                m_listUnusedTexture.push_back(pItem); */
				TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pItem,TEXTURE_USED_TO_UNUSED );
				pItem->cUse = 1;
                pItem->iPathIdx = 0;
            }
        }
        return TRUE;
    }else
    {
        if ( dwIdx > IFL_TAG + MAX_IFL_TEXTURES )
            return FALSE;

        _TexIFL *pItem = &m_IFLTextureList[dwIdx-IFL_TAG-1];
        if (pItem->iRefCount)
        {
            pItem->Release();
            return TRUE;
        }else
        {
            return FALSE;
        }
    }
    return FALSE;
}

void RtgTexture::ActiveTexture(_TexItem* pItem)
{
#ifdef MULTITHREAD
	m_lock.Lock();
#endif
    pItem->bLastRender = true;
    CHECK(pItem->cUse!=0);
    if (pItem->cUse==1)
    {
        /*m_listUnusedTexture.remove(pItem);
        m_listUsedTexture.push_back(pItem); */
		TextureListTransform( m_listEmptyTexture,m_listUsedTexture,m_listUnusedTexture,pItem,TEXTURE_UNUSED_TO_USED );
		pItem->cUse = 2;
    }
    if (pItem->pTexture==NULL)
    {
        CreateTextureVideoMem(pItem);
    }
#ifdef MULTITHREAD
	m_lock.Unlock();
#endif
	
}

void RtgTexture::AddTextureRef(_TexItem* pItem)
{
    CHECK(pItem!=NULL);
    pItem->iRefCount++;
}

void RtgTexture::ReleaseTextureRef(_TexItem* pItem)
{
    CHECK(pItem!=NULL);
    if (pItem->iRefCount>0)
    {
        pItem->iRefCount--;
    }
}

void RtgTexture::ReleaseTextureVideoMem(_TexItem* pItem)
{
    CHECK(pItem!=NULL);
    if (pItem->pTexture==0) return;
    if (m_pDevice->DeleteTexture(pItem))
    {		
        m_dwUsedVideoTexSize -= pItem->GetDataSize();
    }
}

void RtgTexture::CreateTextureVideoMem(_TexItem* pItem)
{
    CHECK(pItem!=NULL);
    if (pItem->pTexture!=0) return;
    BOOL bResult = FALSE;
    switch (pItem->iPathIdx)
    {
    case -1:
        bResult = m_pDevice->CreateTextureDynamic(pItem);
        if (!bResult)
        {
            RtCoreLog().Error("����: DX ���ܴ�����̬��ͼ [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
        }
        break;
    case -2:
        bResult = m_pDevice->CreateTextureRenderTarget(pItem);
        if (!bResult)
        {
            RtCoreLog().Error("����: DX ���ܴ��� Render Target [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
        }
        break;
    case -3:
        bResult = m_pDevice->CreateTextureDepthStencil(pItem);
        if (!bResult)
        {
            RtCoreLog().Error("����: DX ���ܴ��� Depth Stencil [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
        }
        break;
    case -5 :
        bResult = m_pDevice->CreateTexture(pItem, true);
        if (!bResult)
        {
            RtCoreLog().Error("����: DX ���ܴ�����̬��ͼ [��С: (%d,%d), MipMap: %d].\n", pItem->iWidth, pItem->iHeight, pItem->iMipMapCount);
        }
        break;
    default:
        bResult = CreateTextureEntity(pItem);
        break;
    }
    if (bResult)
    {
        m_dwUsedVideoTexSize += pItem->GetDataSize();
    }else
    {
       // RtCoreLog().Error("�������´�����ͼ����.\n");
    }
}

BOOL RtgTexture::UseTexture(int iTextureStage, unsigned int dwResID)
{
    RTASSERT(false);
    /*
    _TexItem* pItem;

    if (dwResID==0)
    {
        m_pDevice->UseTexture(iTextureStage, NULL);
        return TRUE;
    }

    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Info("RtgTexture::UseTexture �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return FALSE;
    }

    m_dwCurrentTexture = dwIdx;

    if (dwIdx < IFL_TAG)
    {
        if ( dwIdx<=0 || dwIdx>MAX_TEXTURES || m_pDevice->m_bTexture[iTextureStage]==0 )
        {
            m_pDevice->UseTexture(iTextureStage, NULL);
            if (dwIdx!=0)
            {
                RtCoreLog().Error("����: ��ͼID���󣬲����ҵ���Ӧ����ͼ.\n");
            }
            return FALSE;
        }else
        {
            pItem = &(m_TextureList[dwIdx-1]);
            ActiveTexture(pItem);
            if (pItem->pTexture)
            {
                if (!m_pDevice->UseTexture(iTextureStage, pItem))
                {
                    RtCoreLog().Error("����: UseTexture����.\n");
                }
            }
        }
    }else
    {
        if ( dwIdx<=0 || dwIdx>IFL_TAG + MAX_IFL_TEXTURES || m_pDevice->m_bTexture[iTextureStage]==0 )
        {
            m_pDevice->UseTexture(iTextureStage, NULL);
            if (dwIdx!=0)
            {
                RtCoreLog().Error("����: IFL��ͼID���󣬲����ҵ���Ӧ����ͼ.\n");
            }
            return FALSE;
        }
        _TexIFL *texture = &m_IFLTextureList[dwIdx - IFL_TAG - 1];
			 unsigned int iTexID = texture->pTextureID[ texture->iCurrentTexture ];
			dwIdx = FindIdx(iTexID);
			if (dwIdx<=0 || dwIdx>MAX_TEXTURES || iTextureStage>m_iMaxMultiTextures)
			{
				m_pDevice->UseTexture(iTextureStage, NULL);
				if (dwIdx!=0)
				{
					RtCoreLog().Error("����: ��ͼID���󣬲����ҵ���Ӧ����ͼ.\n");
				}
				return FALSE;
			}else
			{
				pItem = &(m_TextureList[dwIdx-1]);
				ActiveTexture(pItem);
				if (pItem->pTexture)
				{
					if (!m_pDevice->UseTexture(iTextureStage, pItem))
					{
						RtCoreLog().Error("����: UseTexture����.\n");
					}
				}
			}
       
    }

    */
    return TRUE;
}

void RtgTexture::DeleteAllTextures(void)
{
    /*
    int i;
    for ( i=0; i<MAX_IFL_TEXTURES; i++ )
    {
        if ( m_IFLTextureList[i].iRefCount )
        {
            m_IFLTextureList[i].iRefCount = 1;
            DeleteTexture(IFL_TAG+i+1);
        }
    }

    for ( i=0; i<MAX_TEXTURES; i++ )
    {
        if ( m_TextureList[i].iRefCount )
        {
            m_TextureList[i].iRefCount = 1;
            DeleteTexture(i+1);
        }
    }
    */
}

BOOL RtgTexture::LockTexture(unsigned int dwResID, RtgTexture::TexLock& texLock)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::LockTexture �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return FALSE;
    }
    if (dwIdx<=0 || dwIdx>IFL_TAG || dwIdx>MAX_TEXTURES)
    {
        CHECKEX("��ͼID����");
        return FALSE;
    }
    _TexItem *pItem = &m_TextureList[dwIdx-1];
    if (pItem)
    {
        ActiveTexture(pItem);
        return m_pDevice->TextureLock(pItem, texLock);
    }else
    {
        return FALSE;
    }
}

void RtgTexture::UnlockTexture(RtgTexture::TexLock& texLock)
{
    if (texLock.dwTexID<=0 || texLock.dwTexID>IFL_TAG || texLock.dwTexID>MAX_TEXTURES)
    {
        CHECKEX("��ͼID����");
        return;
    }
    _TexItem *pItem = &m_TextureList[texLock.dwTexID-1];
    if (pItem)
    {
        m_pDevice->TextureUnlock(pItem, texLock);
    }
}

BOOL RtgTexture::UpdateTexture(int dwResID, void *pBuffer)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::UpdateTexture �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return FALSE;
    }
    if (dwIdx<=0 || dwIdx>IFL_TAG || dwIdx>MAX_TEXTURES)
    {
        CHECKEX("��ͼID����");
        return FALSE;
    }
    _TexItem *pItem = &m_TextureList[dwIdx-1];
    ActiveTexture(pItem);
    return m_pDevice->UpdateTexture(pItem, pBuffer);
}

BOOL RtgTexture::UpdateTextureDynamic(int dwResID, void *pBuffer)
{
    unsigned int dwIdx = FindIdx(dwResID);
    if (dwIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgTexture::UpdateTextureDynamic �Ҳ���IDΪ%d����ͼ\n", dwResID);
#endif
        return FALSE;
    }
    if (dwIdx<=0 || dwIdx>IFL_TAG || dwIdx>MAX_TEXTURES)
    {
        CHECKEX("��ͼID����");
        return FALSE;
    }
    _TexItem *pItem = &m_TextureList[dwIdx-1];
    ActiveTexture(pItem);
    return m_pDevice->UpdateTextureDynamic(pItem, pBuffer);
}

unsigned int RtgTexture::TotalVideoTextureMem()
{
    return m_dwTotalVideoTexSize;
}

unsigned int RtgTexture::UsedVideoTextureMem()
{
    return m_dwUsedVideoTexSize;
}

unsigned int RtgTexture::FreeVideoTextureMem()
{
    return m_dwTotalVideoTexSize - m_dwUsedVideoTexSize;
}

// �����д�������ͼ����գ���Ҫ������ͼ�����仯��ȫ�ֱ仯��ʱ��
BOOL RtgTexture::UnloadAllTexture()
{
    _TexItem *pItem;

    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        if (pItem->bQualityManage)
        {
            ReleaseTextureVideoMem(pItem);
        }
    }

    for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
    {
        pItem = *it;
        if (pItem->bQualityManage)
        {
            ReleaseTextureVideoMem(pItem);
        }
    }
    return TRUE;
}

void RtgTexture::OnBeginRender()
{
    _TexItem* pItem;
    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        if (pItem->bLastRender)
        {
            pItem->iUseless = 0;
        }else
        {
            pItem->iUseless ++;
        }
        pItem->bLastRender = false;
    }

    for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
    {
        pItem = *it;
        pItem->iUseless ++;
    }
}

void RtgTexture::OnEndRender()
{
    _TexItem *pItem;
    std::list<_TexItem*>::iterator it;
	///!����ط���Ϊÿһ֡���������������ԾͲ�����TextureListTransform,���Ч��
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end();)
    {
        pItem = *it;
        if (pItem->iUseless>15)
        {
            it = m_listUsedTexture.erase(it);
            m_listUnusedTexture.push_back(pItem); pItem->cUse = 1;
			continue;
        }
        it++;
    }	
}
bool RtgTexture::OnRestoreDevice()
{
    /*
    _TexItem *pItem;
    pItem = m_pUsedTexture;
    while (pItem)
    {
        ReleaseTextureVideoMem(pItem);
        pItem = pItem->pNext;
    }
    pItem = m_pUnusedTexture;
    while (pItem)
    {
        ReleaseTextureVideoMem(pItem);
        pItem = pItem->pNext;
    }
    */
    return true;
}

bool RtgTexture::OnInvalidateDevice()
{
    _TexItem *pItem;
    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        ReleaseTextureVideoMem(pItem);
    }
    for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
    {
        pItem = *it;
        ReleaseTextureVideoMem(pItem);
    }
    return true;
}

void RtgTexture::DumpUsedTexture(bool bDumpToFile)
{
    RtArchive* pAr;
    char* pPathName[] = {"Dynamic", "RenderTarget", "DepthStencil"};
    _TexItem *pItem;
    char szOutBuf[512];
    if (bDumpToFile)
    {
        pAr = RtCoreFile().CreateFileWriter(NULL, "DumpTexture.csv");
        if (pAr)
        {
            rt2_sprintf(szOutBuf, "\"Path\",\"File\",\"Width\",\"Height\",\"Size\",\"MinMap\",\"QualityManage\"\n");
            pAr->Serialize(szOutBuf, (int)strlen(szOutBuf));
        }
    }else
    {
        RtCoreLog().Info("DumpUsedTexture %d, MemUsed:%d[%d]\n", m_listUsedTexture.size(), TotalUsedTextureSize(), UsedVideoTextureMem());
    }
    std::list<_TexItem*>::iterator it;
    for (it=m_listUsedTexture.begin(); it!=m_listUsedTexture.end(); it++)
    {
        pItem = *it;
        if (bDumpToFile)
        {
            if (pAr)
            {
				memset( szOutBuf , NULL , 512 );
				int nSize = 0;
				if( pItem->pTexture )
				{
                    nSize = pItem->GetDataSize();
				}
                rt2_sprintf(szOutBuf, "\"%s\",\"%s\",\"%d\",\"%d\",\"%d\",\"%d\",\"%s\"\n",
                    pItem->iPathIdx<0?pPathName[-pItem->iPathIdx-1]:m_szTexturePathList[pItem->iPathIdx],
                    pItem->szFileName,
                    pItem->iWidth,
                    pItem->iHeight,
                    nSize,
                    pItem->iMipMapCount,
                    pItem->bQualityManage?"Enable":"Disable"
                    );
                pAr->Serialize(szOutBuf, (int)strlen(szOutBuf));
            }
        }else
        {
			int nSize = 0;
			if( pItem->pTexture )
			{
				nSize = pItem->GetDataSize();
			}
            RtCoreLog().Info("  [%s]%s (%dx%d Mip:%d Q:%s Size:%d)\n",
                pItem->iPathIdx<0?pPathName[-pItem->iPathIdx-1]:m_szTexturePathList[pItem->iPathIdx],
                pItem->szFileName,
                pItem->iWidth,
                pItem->iHeight,
                pItem->iMipMapCount,
                pItem->bQualityManage?"Enable":"Disable",
                nSize );
        }
    }
	if (bDumpToFile)
	{
		if (pAr)
		{
            memset( szOutBuf , NULL , 512 );
			rt2_sprintf(szOutBuf, "%s", "======================Unuse Textures ===================\r\n");
			pAr->Serialize(szOutBuf, (int)strlen(szOutBuf));
		}
	}	
	for (it=m_listUnusedTexture.begin(); it!=m_listUnusedTexture.end(); it++)
	{
		pItem = *it;
		if (bDumpToFile)
		{
			if (pAr)
			{
				int nSize = 0;
				if( pItem->pTexture )
				{
					nSize = pItem->GetDataSize();
				}
				memset( szOutBuf , NULL , 512 );
				rt2_sprintf(szOutBuf, "\"%s\",\"%s\",\"%d\",\"%d\",\"%d\",\"%d\",\"%s\"\n",
					pItem->iPathIdx<0?pPathName[-pItem->iPathIdx-1]:m_szTexturePathList[pItem->iPathIdx],
					pItem->szFileName,
					pItem->iWidth,
					pItem->iHeight,
					nSize,
					pItem->iMipMapCount,
					pItem->bQualityManage?"Enable":"Disable"
					);
				pAr->Serialize(szOutBuf, (int)strlen(szOutBuf));
			}
		}else
		{
			int nSize = 0;
			if( pItem->pTexture )
			{
				nSize = pItem->GetDataSize();
			}
			RtCoreLog().Info("  [%s]%s (%dx%d Mip:%d Q:%s Size:%d)\n",
				pItem->iPathIdx<0?pPathName[-pItem->iPathIdx-1]:m_szTexturePathList[pItem->iPathIdx],
				pItem->szFileName,
				pItem->iWidth,
				pItem->iHeight,
				pItem->iMipMapCount,
				pItem->bQualityManage?"Enable":"Disable",
				nSize );
		}
	}

    if (bDumpToFile)
    {
        if (pAr)
        {
            RtCoreFile().CloseFile(pAr);
        }
    }
}

void RtgTexture::SetGenerateDXT(bool bGenerate)
{
    m_bGenerateDXT = bGenerate;
}

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

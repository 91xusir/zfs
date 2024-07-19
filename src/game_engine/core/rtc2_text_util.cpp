#include "core/rt2_core.h"

namespace rt2_core {

RtTextConfig::RtTextConfig()
{
    m_iCount = 0;
    m_pItem = NULL;
    m_pConfigText = NULL;
}

RtTextConfig::RtTextConfig(const char* szText)
{
    m_iCount = 0;
    m_pItem = NULL;
    m_pConfigText = NULL;
    SetConfig(szText);
}

RtTextConfig::~RtTextConfig()
{
    Clear();
}

void RtTextConfig::Clear()
{
    RtCoreMem().Free(m_pItem);
    RtCoreMem().Free(m_pConfigText);
    m_iCount = 0;
}

bool RtTextConfig::SetConfig(const char* szText)
{
    if (szText==NULL) return false;

    int i;
    Clear();
    i = (int)strlen(szText);
    m_pConfigText = (char*)RtCoreMem().Alloc(i+2, __FILE__, __LINE__);
    strcpy(m_pConfigText, szText);
    m_pConfigText[i+1] = 0;

    m_iCount = 0;
    GetInit();
    while (GetNextItem(false))
    {
        GetNextValue(false);
        m_iCount ++;
    }

    m_pItem = (SItem*)RtCoreMem().Alloc(sizeof(SItem)*m_iCount, __FILE__, __LINE__);
    GetInit();
    for (i=0; i<m_iCount; i++)
    {
        m_pItem[i].pItem  = GetNextItem(true);
        m_pItem[i].pValue = GetNextValue(true);
    }

    return true;
}

bool RtTextConfig::GetValue(const char* szItem, const char** ppValue)
{
    int i;
    for (i=0; i<m_iCount; i++)
    {
        if (stricmp(szItem, m_pItem[i].pItem)==0)
        {
            if (ppValue) *ppValue = m_pItem[i].pValue;
            return true;
        }
    }
    return false;
}

bool RtTextConfig::GetItem(int iIdx, const char** ppItem, const char** ppValue)
{
    if (iIdx>=0 && iIdx<m_iCount)
    {
        *ppItem  = m_pItem[iIdx].pItem;
        if (ppValue) *ppValue = m_pItem[iIdx].pValue;
        return true;
    }else
    {
        return false;
    }
}

void RtTextConfig::GetInit()
{
    m_bError = false;
    m_pCurPos = m_pConfigText;
}

const char* RtTextConfig::GetNextItem(bool bFlag)
{
    if (m_bError) return NULL;
    SkipSpace();
    char* pItem = m_pCurPos;
    while (*m_pCurPos && IsIDChar(*m_pCurPos))
    {
        m_pCurPos ++;
    }
    if (m_pCurPos==pItem)
    {
        m_bError = true;
        return NULL;
    }
    char* pEndItem = m_pCurPos;
    SkipSpace();
    if (*m_pCurPos==';' || *m_pCurPos==0)
    {
        m_bNullValue = true;
        if (*m_pCurPos==';')
        {
            m_pCurPos ++;
            SkipSpace();
        }
    }else
    {
        if (*m_pCurPos=='=')
        {
            m_pCurPos ++;
            SkipSpace();
        }else
        {
            m_bError = true;
            return NULL;
        }
        m_bNullValue = false;
    }
    if (bFlag)
    {
        *pEndItem = 0;
    }
    return pItem;
}

const char* RtTextConfig::GetNextValue(bool bFlag)
{
    const char* pNull = "";
    if (m_bNullValue) return pNull;
    bool bInQuot = (*m_pCurPos=='"');
    char* pValue;
    if (bInQuot)
    {
        m_pCurPos++;
        pValue = m_pCurPos;
        bool bHaveQuots = false;
        while (*m_pCurPos)
        {
            if (*m_pCurPos=='"')
            {
                if (m_pCurPos[1]=='"')
                {
                    bHaveQuots = true;
                    m_pCurPos ++;
                }else
                {
                    break;
                }
            }
            m_pCurPos ++;
        }
        if (*m_pCurPos!='"')
        {
            m_bError = true;
            return pNull;
        }else
        {
            char* pEndValue = m_pCurPos;
            m_pCurPos ++;
            SkipSpace();
            if (*m_pCurPos==';')
            {
                m_pCurPos ++;
                SkipSpace();
            }else
            {
                if (*m_pCurPos!=0)
                {
                    m_bError = true;
                    return pNull;
                }
            }
            if (pEndValue<=pValue) return pNull;
            if (bFlag)
            {
                if (bHaveQuots)
                {
                    char *pS, *pD;
                    pS = pD = pValue;
                    while (pS<pEndValue)
                    {
                        if (*pS=='"')
                        {
                            if (pS[1]=='"')
                            {
                                pS ++;
                            }
                        }
                        if (pD != pS)
                        {
                            *pD = *pS;
                        }
                        pD ++;
                        pS ++;
                    }
                    pEndValue = pD;
                }
                *pEndValue = 0;
            }
        }
    }else
    {
        pValue = m_pCurPos;
        while (*m_pCurPos && *m_pCurPos!=';')
        {
            m_pCurPos ++;
        }
        char* pEndValue = m_pCurPos;
        if (*m_pCurPos==';')
        {
            m_pCurPos ++;
            SkipSpace();
        }
        if (IsSpace(pEndValue[-1]))
        {
            pEndValue --;
            while (IsSpace(pEndValue[-1]) && pEndValue>pValue)
            {
                pEndValue --;
            }
        }
        if (pEndValue<=pValue) return pNull;
        if (bFlag)
        {
            *pEndValue = 0;
        }
    }
    if (*m_pCurPos==';') m_pCurPos ++;
    return pValue;
}

static char s_szHexMapTable[] = "0123456789ABCDEFX";

#define SAVE_HEX_STR(i, o)  szStr[i] = s_szHexMapTable[(v&(0x0F<<o))>>o]
#define LOAD_HEX_STR(i, o)  v |= ((((szStr[i]>'9')?(szStr[i]-'A'+10):(szStr[i]-'0'))&0x0F)<<o)

int SaveToString(char* szStr, char v)
{
    SAVE_HEX_STR(0, 4);
    SAVE_HEX_STR(1, 0);
    return sizeof (char)*2;
}

int SaveToString(char* szStr, short v)
{
    SAVE_HEX_STR(0, 12);
    SAVE_HEX_STR(1, 8);
    SAVE_HEX_STR(2, 4);
    SAVE_HEX_STR(3, 0);
    return sizeof (short)*2;
}

int SaveToString(char* szStr, int v)
{
    SAVE_HEX_STR(0, 28);
    SAVE_HEX_STR(1, 24);
    SAVE_HEX_STR(2, 20);
    SAVE_HEX_STR(3, 16);
    SAVE_HEX_STR(4, 12);
    SAVE_HEX_STR(5, 8);
    SAVE_HEX_STR(6, 4);
    SAVE_HEX_STR(7, 0);
    return sizeof (int)*2;
}

int SaveToString(char* szStr, long v)
{
    SAVE_HEX_STR(0, 28);
    SAVE_HEX_STR(1, 24);
    SAVE_HEX_STR(2, 20);
    SAVE_HEX_STR(3, 16);
    SAVE_HEX_STR(4, 12);
    SAVE_HEX_STR(5, 8);
    SAVE_HEX_STR(6, 4);
    SAVE_HEX_STR(7, 0);
    return sizeof (long)*2;
}

int SaveToString(char* szStr, BYTE  v)
{
    SAVE_HEX_STR(0, 4);
    SAVE_HEX_STR(1, 0);
    return sizeof (BYTE)*2;
}

int SaveToString(char* szStr, WORD  v)
{
    SAVE_HEX_STR(0, 12);
    SAVE_HEX_STR(1, 8);
    SAVE_HEX_STR(2, 4);
    SAVE_HEX_STR(3, 0);
    return sizeof (WORD)*2;
}

int SaveToString(char* szStr, UINT v)
{
    SAVE_HEX_STR(0, 28);
    SAVE_HEX_STR(1, 24);
    SAVE_HEX_STR(2, 20);
    SAVE_HEX_STR(3, 16);
    SAVE_HEX_STR(4, 12);
    SAVE_HEX_STR(5, 8);
    SAVE_HEX_STR(6, 4);
    SAVE_HEX_STR(7, 0);
    return sizeof (UINT)*2;
}

int SaveToString(char* szStr, DWORD v)
{
    SAVE_HEX_STR(0, 28);
    SAVE_HEX_STR(1, 24);
    SAVE_HEX_STR(2, 20);
    SAVE_HEX_STR(3, 16);
    SAVE_HEX_STR(4, 12);
    SAVE_HEX_STR(5, 8);
    SAVE_HEX_STR(6, 4);
    SAVE_HEX_STR(7, 0);
    return sizeof (DWORD)*2;
}

int SaveToString(char* szStr, float v)
{
    return SaveToString(szStr, (*((DWORD*)&v)));
}

int SaveToString(char* szStr, const char* v)
{
    int i = (int)strlen(v);
    CHECK(i<60000);
    SaveToString(szStr, (WORD)i);
    memcpy(szStr+sizeof(WORD)*2, v, i);
    return i+sizeof(WORD)*2;
}

int LoadFromString(char* szStr, char &v)
{
    v = 0;
    LOAD_HEX_STR(0, 4);
    LOAD_HEX_STR(1, 0);
    return sizeof (char)*2;
}

int LoadFromString(char* szStr, short &v)
{
    v = 0;
    LOAD_HEX_STR(0, 12);
    LOAD_HEX_STR(1, 8);
    LOAD_HEX_STR(2, 4);
    LOAD_HEX_STR(3, 0);
    return sizeof (short)*2;
}

int LoadFromString(char* szStr, int &v)
{
    v = 0;
    LOAD_HEX_STR(0, 28);
    LOAD_HEX_STR(1, 24);
    LOAD_HEX_STR(2, 20);
    LOAD_HEX_STR(3, 16);
    LOAD_HEX_STR(4, 12);
    LOAD_HEX_STR(5, 8);
    LOAD_HEX_STR(6, 4);
    LOAD_HEX_STR(7, 0);
    return sizeof (int)*2;
}

int LoadFromString(char* szStr, long &v)
{
    v = 0;
    LOAD_HEX_STR(0, 28);
    LOAD_HEX_STR(1, 24);
    LOAD_HEX_STR(2, 20);
    LOAD_HEX_STR(3, 16);
    LOAD_HEX_STR(4, 12);
    LOAD_HEX_STR(5, 8);
    LOAD_HEX_STR(6, 4);
    LOAD_HEX_STR(7, 0);
    return sizeof (long)*2;
}

int LoadFromString(char* szStr, BYTE &v)
{
    v = 0;
    LOAD_HEX_STR(0, 4);
    LOAD_HEX_STR(1, 0);
    return sizeof (BYTE)*2;
}

int LoadFromString(char* szStr, WORD &v)
{
    v = 0;
    LOAD_HEX_STR(0, 12);
    LOAD_HEX_STR(1, 8);
    LOAD_HEX_STR(2, 4);
    LOAD_HEX_STR(3, 0);
    return sizeof (WORD)*2;
}

int LoadFromString(char* szStr, UINT &v)
{
    v = 0;
    LOAD_HEX_STR(0, 28);
    LOAD_HEX_STR(1, 24);
    LOAD_HEX_STR(2, 20);
    LOAD_HEX_STR(3, 16);
    LOAD_HEX_STR(4, 12);
    LOAD_HEX_STR(5, 8);
    LOAD_HEX_STR(6, 4);
    LOAD_HEX_STR(7, 0);
    return sizeof (UINT)*2;
}

int LoadFromString(char* szStr, DWORD &v)
{
    v = 0;
    LOAD_HEX_STR(0, 28);
    LOAD_HEX_STR(1, 24);
    LOAD_HEX_STR(2, 20);
    LOAD_HEX_STR(3, 16);
    LOAD_HEX_STR(4, 12);
    LOAD_HEX_STR(5, 8);
    LOAD_HEX_STR(6, 4);
    LOAD_HEX_STR(7, 0);
    return sizeof (DWORD)*2;
}

int LoadFromString(char* szStr, float &v)
{
    return LoadFromString(szStr, (*((DWORD*)&v)));
}

int LoadFromString(char* szStr, char* v)
{
    WORD i;
    LoadFromString(szStr, i);
    memcpy(v, szStr+sizeof(WORD)*2, i);
    return i+sizeof(WORD)*2;
}

std::string LoadFromString(char* szStr)
{
    WORD i;
    LoadFromString(szStr, i);
    return std::string(szStr+sizeof(WORD)*2, i);
}

} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

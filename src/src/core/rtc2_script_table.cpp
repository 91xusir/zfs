#include "core/rt2_core.h"

namespace rt2_core {

/*------------------------------------------------------------------------
-   class RtTable
------------------------------------------------------------------------*/

RtTable::RtTable(char cSeparator)
{
    m_cSeparator = cSeparator;
    m_pContent = NULL;
    m_pItems = NULL;
    m_pLines = NULL;
    m_InvalidLine.SetLine(this, -1);
}

RtTable::~RtTable()
{
    Clear();
}

// 载入一个文本表格文件 szFilename
bool RtTable::LoadTableFile(const char* szFilename)
{
    CHECK(szFilename!=NULL);
    if (!RtCoreFile().FileExist(szFilename))
    {
        return false;
    }

    Clear();

    int iFileSize = RtCoreFile().FileSize(szFilename);
    m_szFilename = szFilename;
    m_pContent = (char*)RtCoreMem().Alloc(iFileSize+1, __FILE__, __LINE__);
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    CHECK (pArc!=NULL);
    pArc->Serialize(m_pContent, iFileSize);
    m_pContent[iFileSize] = 0;
    RtCoreFile().CloseFile(pArc);

    if (!ParseTextTable())
    {
        Clear();
        return false;
    }

    m_pItems = (char**)RtCoreMem().Alloc(sizeof(char*)*m_iLineCount*m_iMaxRow, __FILE__, __LINE__);
    memset(m_pItems, 0, sizeof(char*)*m_iLineCount*m_iMaxRow);

    m_pLines = RT_NEW TableLine[m_iLineCount];
    for (int i=0; i<m_iLineCount; i++)
    {
        m_pLines[i].SetLine(this, i);
    }

    ParseTextTable();
    return true;
}

// 清除所有信息
void RtTable::Clear()
{
    m_iTitleLine = 0;
    m_iTitleRow = 0;
    m_iMaxRow = 0;
    m_iLineCount = 0;
    m_szFilename = "";
    if (m_pContent)
    {
        RtCoreMem().Free(m_pContent);
        m_pContent = NULL;
        RtCoreMem().Free(m_pItems);
        DEL_ARRAY(m_pLines);
    }
}

bool RtTable::ParseTextTable()
{
    CHECK(m_pContent!=NULL);
    int iLine, iRow;
    int iState; // 0 普通, 1 字符串内
    char *pWord, *pCur, *pd;

    iState = 0;
    iLine = iRow = 0;
    pd = pWord = pCur = m_pContent;
    while (*pCur)
    {
        if (iState==0)
        {
            if (*pCur=='"')
            {
                iState = 1;
            }else if (*pCur==m_cSeparator)
            {
                if (m_pItems)
                {
                    *pd = 0;
                    CHECK(iLine<m_iLineCount && iRow<m_iMaxRow);
                    m_pItems[iLine*m_iMaxRow + iRow] = pWord;
                }
                iRow ++;
                pd = pWord = pCur + 1;
                if (m_pItems==0)
                {
                    if (m_iMaxRow<iRow) m_iMaxRow=iRow;
                }
            }else if (*pCur==0x0A || *pCur==0x0D)
            {
                if (pCur[1]==0x0A || pCur[1]==0x0D)
                {
                    pCur ++;
                }
                if (m_pItems)
                {
                    *pd = 0;
                    CHECK(iLine<m_iLineCount && iRow<m_iMaxRow);
                    m_pItems[iLine*m_iMaxRow + iRow] = pWord;
                }
                iRow ++;
                if (m_pItems==0)
                {
                    if (m_iMaxRow<iRow) m_iMaxRow=iRow;
                }
                iLine ++;
                iRow = 0;
                pd = pWord = pCur + 1;
            }else
            {
                if (m_pItems)
                {
                    if (pCur!=pd) *pd = *pCur;
                    pd ++;
                }
            }
        }else if (iState==1)
        {
            if (*pCur=='"')
            {
                if (pCur[1]=='"')
                {
                    // 还是双引号
                    pCur ++;
                    if (m_pItems)
                    {
                        if (pCur!=pd) *pd = *pCur;
                        pd ++;
                    }
                }else
                {
                    // 结束
                    iState = 0;
                }
            }else
            {
                if (m_pItems)
                {
                    if (pCur!=pd) *pd = *pCur;
                    pd ++;
                }
            }
        }
        pCur ++;
    }
    if (pWord!=pCur)
    {
        if (m_pItems)
        {
            *pd = 0;
            CHECK(iLine<m_iLineCount && iRow<m_iMaxRow);
            m_pItems[iLine*m_iMaxRow + iRow] = pWord;
        }
        iRow ++;
        if (m_pItems==0)
        {
            if (m_iMaxRow<iRow) m_iMaxRow=iRow;
        }
        iLine ++;
    }

    m_iLineCount = iLine;
    return true;
}

bool RtTable::FindPosByString(const char* szString, int& iLine, int& iRow)
{
    char* p;
    int i, j;
    for (i=0; i<m_iLineCount; i++)
    {
        for (j=0; j<m_iMaxRow; j++)
        {
            p = m_pItems[i*m_iMaxRow+j];
            if (p)
            {
                if (strcmp(p, szString)==0)
                {
                    iLine = i;
                    iRow = j;
                    return true;
                }
            }
        }
    }
    return false;
}

int RtTable::FindLineByString(const char* szString)
{
    int iLine, iRow;
    if (FindPosByString(szString, iLine, iRow))
    {
        return iLine;
    }
    return -1;
}

int RtTable::FindRowByString(const char* szString)
{
    int iLine, iRow;
    if (FindPosByString(szString, iLine, iRow))
    {
        return iRow;
    }
    return -1;
}

const RtTable::TableLine& RtTable::operator[](const char* szIdx) const
{
    for (int i=0; i<m_iLineCount; i++)
    {
        if (strcmp(m_pItems[i*m_iMaxRow+m_iTitleRow], szIdx)==0)
        {
            return m_pLines[i];
        }
    }
    return m_InvalidLine;
}

const char* RtTable::GetString(int iLine, const char* szRowIdx) const
{
    char* pStr;
    static char szNull[] = "";
    if (iLine<0) return szNull;
    for (int i=0; i<m_iMaxRow; i++)
    {
        if (strcmp(m_pItems[m_iTitleLine*m_iMaxRow+i], szRowIdx)==0)
        {
            pStr = m_pItems[iLine*m_iMaxRow+i];
            if (pStr==NULL) pStr = szNull;
            return pStr;
        }
    }
    return szNull;
}

const char* RtTable::GetString(const char* szLineIdx, const char* szRowIdx) const
{
    return (*this)[szLineIdx][szRowIdx];
}

// 取得指定行和列的字符串
const char* RtTable::GetString(int iLine, int iRow) const
{
    if (iLine<0 || iLine>=m_iLineCount || iRow<0 || iRow>=m_iMaxRow) return NULL;
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return "";
    return (const char*)m_pItems[iLine*m_iMaxRow+iRow];
}

bool RtTable::Char (int iLine, int iRow, char & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (char)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Byte (int iLine, int iRow, unsigned char & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (unsigned char)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Short(int iLine, int iRow, short & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (short)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Word (int iLine, int iRow, unsigned short& vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (unsigned short)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Int  (int iLine, int iRow, int & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (int)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::UInt (int iLine, int iRow, unsigned int & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (unsigned int)atoi(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Long (int iLine, int iRow, long & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (long)atol(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::DWord(int iLine, int iRow, unsigned long & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (unsigned long)atol(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Float (int iLine, int iRow, float & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = (float)atof(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

bool RtTable::Double(int iLine, int iRow, double & vValue) const
{
    if (m_pItems[iLine*m_iMaxRow+iRow]==NULL) return false;
    vValue = atof(m_pItems[iLine*m_iMaxRow+iRow]);
    return true;
}

/*------------------------------------------------------------------------
-   class RtTableLoader
------------------------------------------------------------------------*/
RtTableLoader::RtTableLoader()
{
}

RtTableLoader::~RtTableLoader()
{
    Clear();
}

bool RtTableLoader::Load(RtString& szLoadFilename)
{
	int iLine, iMaxLine, iRow, iMaxRow, iTitleLine, iIdx;
    ETableReadState eResult;
    TableObject* pNewObj;
	RtCsv csv;

    m_szTableFilename = szLoadFilename;
	if(!csv.LoadTableFile(m_szTableFilename))
		return false;

    if (!OnSetTitleLine(csv))
    {
        return false;
    }

    if (!OnReadStart())
    {
        return false;
    }

    iTitleLine = csv.GetTitleLine();
    iMaxRow    = csv.GetMaxRow();
	iMaxLine   = csv.GetLineCount();
    iLine = iTitleLine;
	for (iLine++; iLine<iMaxLine; iLine++)
	{
        eResult = OnReadLineStart(iLine, csv[iLine]);
        if (eResult==TRS_EXIT)
        {
            return false;
        }else if (eResult==TRS_SKIP)
        {
            continue;
        }

        pNewObj = OnCreateLineObject();
        if (pNewObj==NULL)
        {
            return false;
        }

        iIdx = -1;
        for (iRow=0; iRow<iMaxRow; iRow++)
        {
            if (iRow==csv.GetTitleRow())
            {
                iIdx = csv[iLine][iRow].Int();
            }
            eResult = OnReadLineItem(iLine, csv[iTitleLine][iRow].Str(), csv[iLine][iRow], pNewObj);
            if (eResult==TRS_EXIT)
            {
                OnDestroyLineObject(pNewObj);
                return false;
            }else if (eResult==TRS_SKIP)
            {
                break;
            }
        }
        if (iRow!=iMaxRow)
        {
            OnDestroyLineObject(pNewObj);
            continue;
        }

        eResult = OnReadLineFinish(iLine, pNewObj);
        if (eResult==TRS_EXIT)
        {
            return false;
        }else if (eResult==TRS_SKIP)
        {
            OnDestroyLineObject(pNewObj);
            continue;
        }

        if (iIdx==-1)
        {
            OnDestroyLineObject(pNewObj);
        }else
        {
            if (m_map.find(iIdx)==m_map.end())
            {
                m_table.push_back(pNewObj);
                m_map[iIdx] = pNewObj;
            }else
            {
                *(m_map[iIdx]) = *pNewObj;
                OnDestroyLineObject(pNewObj);
            }
        }
    }

    if (!OnReadFinish())
    {
        return false;
    }

    return true;
}

void RtTableLoader::Clear()
{
    int i, iCnt = (int)m_table.size();
    for (i=0; i<iCnt; i++)
    {
        OnDestroyLineObject(m_table[i]);
    }
    m_table.clear();
}

bool RtTableLoader::Reload()
{
    return ReloadByFilename(m_szTableFilename);
}

bool RtTableLoader::ReloadByFilename(RtString& szLoadFilename)
{
    return Load(szLoadFilename);
}

RtTableLoader::TableObject* RtTableLoader::Find(int iIdx)
{
    std::map<int, TableObject*>::iterator it = m_map.find(iIdx);
    if (it!=m_map.end())
        return it->second;
    return NULL;
}

RtTableLoader::TableObject* RtTableLoader::At(int iIdx)
{
    if (iIdx>=0 && iIdx<(int)m_table.size())
        return m_table[iIdx];
    return NULL;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/

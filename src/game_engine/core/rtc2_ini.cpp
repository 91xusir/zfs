#include <stdio.h>
#include "core/rt2_core.h"
#include "rtc2_ini_i.h"
using namespace std;

namespace rt2_core {

RtIni::SItem RtIni::s_nullItem;

///////////////////////////////////////////////////////////////////////////

#define AZOTH_MAX_LONG_BUFLEN   20
#define AZOTH_MAX_DOUBLE_BUFLEN 20

///////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#   define DumpValue _DumpValue
void _DumpValue(azoth_value *arg)
{
    switch (arg->type)
    {
    case IS_LONG:
        printf("Long: %d", arg->value.lval);
        break;
    case IS_DOUBLE:
        printf("Double: %f", arg->value.dval);
        break;
    case IS_STRING:
        printf("String: %s", arg->value.str.val);
        break;
    case IS_BOOL:
        printf("Bool: %d", arg->value.lval);
        break;
    case IS_NULL:
    case IS_ARRAY:
    case IS_OBJECT:
    case IS_RESOURCE:
    case IS_CONSTANT:
    case IS_CONSTANT_ARRAY:
        printf(" NULL");
        break;
    }
}
#else
#   define DumpValue (void*)(0)
#endif

static void AzothIniParserCallback(azoth_value *arg1, azoth_value *arg2, int type, void *arg)
{
    RtIni*  This;
    if (arg==0 || arg1->value.str.val[0]==0)
    {
        if (arg1)   azoth_freestr(arg1->value.str.val);
        if (arg2)
        {
            if (arg2->type==IS_STRING)
            {
                azoth_freestr(arg2->value.str.val);
            }
        }
        return;
    }
    This = (RtIni*)arg;

    switch (type)
    {
    case AZOTH_INI_SECTION:
        This->AddSection(arg1->value.str.val);
        break;
    case AZOTH_INI_ENTRY:
        if (arg2)
        {
            //printf("Entry: [");DumpValue(arg1);printf("] -> [");DumpValue(arg2);printf("]\n");
            switch (arg2->type)
            {
            case IS_BOOL:
            case IS_LONG:
                This->AddEntry(0, arg1->value.str.val, arg2->value.lval);
                break;
            case IS_DOUBLE:
                This->AddEntry(0, arg1->value.str.val, arg2->value.dval);
                break;
            case IS_STRING:
                This->AddEntry(0, arg1->value.str.val, arg2->value.str.val);
                break;
            case IS_NULL:
                break;
            case IS_ARRAY:
                break;
            case IS_OBJECT:
                break;
            case IS_RESOURCE:
                break;
            case IS_CONSTANT:
                break;
            case IS_CONSTANT_ARRAY:
                break;
            }
        }else
        {
            This->AddEntry(0, arg1->value.str.val, "");
        }
        break;
    }
    if (arg1)   azoth_freestr(arg1->value.str.val);
    if (arg2)
    {
        if (arg2->type==IS_STRING)
        {
            azoth_freestr(arg2->value.str.val);
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void AzothIniErrorCallback(const char* szFilename, const char* szErrMsg, long lLineNo, void *arg)
{
    ((RtIni*)arg)->OnError(szFilename, szErrMsg, lLineNo);
}

///////////////////////////////////////////////////////////////////////////

static int AzothIniOutputToFile(int iItemType, const char* szArg1, const char* szArg2, int iValueType, void *arg)
{
    if (szArg1[0]==0)
        return 0;
    RtArchive* pArc = (RtArchive*)arg;
    char szBuf[8000];
    switch (iItemType)
    {
    case RtIni::IT_SECTION:
        sprintf(szBuf, "\n[%s]\n", szArg1);
        pArc->Serialize(szBuf, (int)strlen(szBuf));
        break;

    case RtIni::IT_ENTRY:
        if (iValueType==RtIni::VT_STRING)
        {
            sprintf(szBuf, "%s = \"%s\"\n", szArg1, szArg2);
        }else
        {
            sprintf(szBuf, "%s = %s\n", szArg1, szArg2);
        }
        pArc->Serialize(szBuf, (int)strlen(szBuf));
        break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////

long RtIni::SItem::ConvertToLong() const
{
    switch (eType)
    {
    case VT_LONG:    return atol(szValue.c_str());
    case VT_DOUBLE:  return (long)atof(szValue.c_str());
    case VT_STRING:  return atol(szValue.c_str());
    default:                return 0;
    }
}

double RtIni::SItem::ConvertToFloat() const
{
    switch (eType)
    {
    case VT_LONG:    return (double)atol(szValue.c_str());
    case VT_DOUBLE:  return atof(szValue.c_str());
    case VT_STRING:  return atof(szValue.c_str());
    default:                return 0.0;
    }
}

void RtIni::SItem::SetLong(long v)
{
    char szBuf[AZOTH_MAX_LONG_BUFLEN];
    sprintf(szBuf, "%d", v);
    eType = VT_LONG;
    szValue = szBuf;
}

void RtIni::SItem::SetFloat(double v)
{
    char szBuf[AZOTH_MAX_DOUBLE_BUFLEN];
    rt2_snprintf(szBuf, AZOTH_MAX_DOUBLE_BUFLEN, "%f", v);
    eType = VT_DOUBLE;
    szValue = szBuf;
}

const RtIni::SItem& RtIni::SItem::operator = (const char* v)
{
    eType = VT_STRING;
    szValue = v;
    return *this;
}

///////////////////////////////////////////////////////////////////////////

RtIni::SSection::SSection(RtIni* p)
{
    pIni = p;
    pFirstItem = NULL;
    pLastItem = NULL;
    pNext = NULL;
    pPrev = NULL;
}

RtIni::SSection::~SSection()
{
    SItem *pItem, *pTmpItem;
    pItem = pFirstItem;
    while (pItem)
    {
        pTmpItem = pItem->pNext;
        delete pItem;
        pItem = pTmpItem;
    }
}

int RtIni::SSection::GetCount()
{
    int iCur = 0;
    SItem* p = pFirstItem;
    while (p)
    {
        iCur ++;
        p = p->pNext;
    }
    return iCur;
}

RtIni::SItem& RtIni::SSection::operator[](int nIndex)
{
    int iCur = 0;
    SItem* pCurrentItem = pFirstItem;
    while (pCurrentItem)
    {
        if (iCur==nIndex)
        {
            break;
        }
        iCur ++;
        pCurrentItem = pCurrentItem->pNext;
    }
    if (pCurrentItem)
    {
        return *pCurrentItem;
    }
    CHECKEX("ini array overbound");
    return s_nullItem;
}

RtIni::SItem& RtIni::SSection::operator[](const char* szIdx)
{
    SItem* pCurrentItem = pFirstItem;
    while (pCurrentItem)
    {
        if (stricmp(pCurrentItem->szName.c_str(), szIdx)==0)
        {
            break;
        }
        pCurrentItem = pCurrentItem->pNext;
    }
    if (pCurrentItem)
    {
        return *pCurrentItem;
    }
    if (pIni->m_bCanWrite)
    {
        pIni->AddEntry(szName.c_str(), szIdx, (long)0);
        return *(pIni->m_pCurrentItem);
    }else
    {
        RtCoreLog().Warn("INI: Cannot found item [%s / %s] in [%s]\n", szName.c_str(), szIdx, pIni->m_szFilename.c_str());
        //CHECKEX("Cannot found item");
    }
    return s_nullItem;
}

///////////////////////////////////////////////////////////////////////////

RtIni::RtIni(bool bSameEntry) : m_sDefaultSection(this)
{
    m_pCurrentItem = NULL;
    m_bInit = false;
    m_bCanWrite = false;
    m_bSameEntry = bSameEntry;

    m_sDefaultSection.szName = "";
    m_sDefaultSection.pNext = NULL;
    m_sDefaultSection.pPrev = NULL;
    m_pFirstSection = &m_sDefaultSection;
    m_pLastSection = &m_sDefaultSection;
    m_pCurrentSection = m_pFirstSection;
}

RtIni::~RtIni()
{
    ClearData();
}

void RtIni::ClearData()
{
    SSection *pSection, *pTmpSection;
    pSection = m_pFirstSection->pNext;
    while (pSection)
    {
        pTmpSection = pSection->pNext;
        delete pSection;
        pSection = pTmpSection;
    }

    m_pCurrentItem = NULL;
    m_bInit = false;
    m_bCanWrite = false;

    m_sDefaultSection.szName = "";
    m_sDefaultSection.pNext = NULL;
    m_sDefaultSection.pPrev = NULL;
    m_pFirstSection = &m_sDefaultSection;
    m_pLastSection = &m_sDefaultSection;
    m_pCurrentSection = m_pFirstSection;
}

bool RtIni::OpenFile(const char* szFilename, bool bWrite)
{
    ClearData();
    int iResult = AzothParseIniFile(szFilename, AzothIniErrorCallback, AzothIniParserCallback, this);
    if (!(bWrite && iResult==-1))
    {
        if (iResult)
        {
            switch (iResult)
            {
            case -1:    // cannot open file
                break;
            default:    // parse error
                break;
            }
            return false;
        }
    }
	releaseAllBuffer();
    m_szFilename = szFilename;
    m_bCanWrite = bWrite;
    m_bInit = true;
    m_pCurrentSection = NULL;
    m_pCurrentItem = NULL;
    return true;
}

bool RtIni::CloseFile(bool bWrite)
{
    if (bWrite)
    {
        if (m_bCanWrite)
        {
            return SaveAs(m_szFilename.c_str());
        }
    }
    return true;
}

bool RtIni::SaveFile()
{
    if (m_bCanWrite)
    {
        return SaveAs(m_szFilename.c_str());
    }
    return true;
}

bool RtIni::SaveAs(const char* szFilename)
{
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFilename, RtFileManager::FW_Text);
    if (pArc==NULL)
        return false;

    this->ScanningAll(AzothIniOutputToFile, pArc);

    pArc->Close();
    delete pArc;
    return true;
}

bool RtIni::FindSection(const char* szSection)
{
    if (szSection==NULL) return m_pCurrentSection!=NULL;

    if (m_pCurrentSection && stricmp(m_pCurrentSection->szName.c_str(), szSection)==0)
    {
        return true;
    }

    m_pCurrentSection = m_pFirstSection;
    while (m_pCurrentSection)
    {
        if (stricmp(m_pCurrentSection->szName.c_str(), szSection)==0)
        {
            break;
        }
        m_pCurrentSection = m_pCurrentSection->pNext;
    }
    if (m_pCurrentSection)
    {
        return true;
    }
    return false;
}

bool RtIni::FindEntry(const char* szSection, const char* szEntry)
{
    if (szSection!=0)
    {
        if (!FindSection(szSection))
        {
            return false;
        }
    }
    if (szEntry==0)
    {
        return false;
    }
    m_pCurrentItem = m_pCurrentSection->pFirstItem;
    while (m_pCurrentItem)
    {
        if (stricmp(m_pCurrentItem->szName.c_str(), szEntry)==0)
        {
            break;
        }
        m_pCurrentItem = m_pCurrentItem->pNext;
    }
    if (m_pCurrentItem)
    {
        return true;
    }
    return false;
}

bool RtIni::AddSection(const char* szSection)
{
    if (!FindSection(szSection))
    {
        m_pCurrentSection = RT_NEW SSection(this);
        m_pCurrentSection->szName = szSection;
        m_pCurrentSection->pNext = NULL;
        m_pCurrentSection->pPrev = m_pLastSection;
        if (m_pLastSection) m_pLastSection->pNext = m_pCurrentSection;
        m_pLastSection = m_pCurrentSection;
        return true;
    }else
    {
        return false; // section already exist
    }
}

bool RtIni::AddEntry(const char* szSection, const char* szEntry, const char* szValue)
{
    if (!FindSection(szSection))
    {
        return false;  // section inexistent
    }

    if (m_bSameEntry || !FindEntry(0, szEntry))
    {
        m_pCurrentItem = RT_NEW SItem;
        m_pCurrentItem->szName = szEntry;
        m_pCurrentItem->szValue = szValue;
        m_pCurrentItem->eType = VT_STRING;
        m_pCurrentItem->pNext = NULL;
        m_pCurrentItem->pPrev = m_pCurrentSection->pFirstItem;
        if (m_pCurrentSection->pFirstItem==NULL) m_pCurrentSection->pFirstItem = m_pCurrentItem;
        if (m_pCurrentSection->pLastItem) m_pCurrentSection->pLastItem->pNext = m_pCurrentItem;
        m_pCurrentSection->pLastItem = m_pCurrentItem;
        return true;
    }else
    {
        return false;  // entry already exist
    }
}

bool RtIni::AddEntry(const char* szSection, const char* szEntry, long lNumber)
{
    if (!FindSection(szSection))
    {
        return false;  // section inexistent
    }
    if (m_bSameEntry || !FindEntry(0, szEntry))
    {
        m_pCurrentItem = RT_NEW SItem;
        m_pCurrentItem->szName = szEntry;
        char szBuf[AZOTH_MAX_LONG_BUFLEN];
        sprintf(szBuf, "%d", lNumber);
        m_pCurrentItem->szValue = szBuf;
        m_pCurrentItem->eType = VT_LONG;
        m_pCurrentItem->pNext = NULL;
        m_pCurrentItem->pPrev = m_pCurrentSection->pFirstItem;
        if (m_pCurrentSection->pFirstItem==NULL) m_pCurrentSection->pFirstItem = m_pCurrentItem;
        if (m_pCurrentSection->pLastItem) m_pCurrentSection->pLastItem->pNext = m_pCurrentItem;
        m_pCurrentSection->pLastItem = m_pCurrentItem;
        return true;
    }else
    {
        return false;  // entry already exist
    }
}

bool RtIni::AddEntry(const char* szSection, const char* szEntry, DWORD dwNumber)
{
    return AddEntry(szSection, szEntry, static_cast<long>(dwNumber));
}

bool RtIni::AddEntry(const char* szSection, const char* szEntry, double dNumber)
{
    if (!FindSection(szSection))
    {
        return false;  // section inexistent
    }
    if (m_bSameEntry || !FindEntry(0, szEntry))
    {
        m_pCurrentItem = RT_NEW SItem;
        m_pCurrentItem->szName = szEntry;
        char szBuf[AZOTH_MAX_DOUBLE_BUFLEN];
        sprintf(szBuf, "%f", dNumber);
        m_pCurrentItem->szValue = szBuf;
        m_pCurrentItem->eType = VT_DOUBLE;
        m_pCurrentItem->pNext = NULL;
        m_pCurrentItem->pPrev = m_pCurrentSection->pFirstItem;
        if (m_pCurrentSection->pFirstItem==NULL) m_pCurrentSection->pFirstItem = m_pCurrentItem;
        if (m_pCurrentSection->pLastItem) m_pCurrentSection->pLastItem->pNext = m_pCurrentItem;
        m_pCurrentSection->pLastItem = m_pCurrentItem;
        return true;
    }else
    {
        return false;  // entry already exist
    }
}

bool RtIni::DelSection(const char* szSection)
{
    if (FindSection(szSection))
    {
        if (m_pCurrentSection->pNext) m_pCurrentSection->pNext->pPrev = m_pCurrentSection->pPrev;
        if (m_pCurrentSection->pPrev) m_pCurrentSection->pPrev->pNext = m_pCurrentSection->pNext;
        if (m_pCurrentSection->pNext==NULL) m_pLastSection = m_pCurrentSection->pPrev;
        if (m_pCurrentSection->pPrev==NULL) m_pFirstSection = m_pCurrentSection->pNext;
        delete m_pCurrentSection;
        m_pCurrentSection = NULL;
        return true;
    }
    return false;
}

bool RtIni::DelEntry(const char* szSection, const char* szEntry, bool bAllSameEntry)
{
    if (bAllSameEntry)
    {
        while (FindEntry(szSection, szEntry))
        {
            if (m_pCurrentItem->pNext) m_pCurrentItem->pNext->pPrev = m_pCurrentItem->pPrev;
            if (m_pCurrentItem->pPrev) m_pCurrentItem->pPrev->pNext = m_pCurrentItem->pNext;
            if (m_pCurrentItem->pNext==NULL) m_pCurrentSection->pLastItem = m_pCurrentItem->pPrev;
            if (m_pCurrentItem->pPrev==NULL) m_pCurrentSection->pFirstItem = m_pCurrentItem->pNext;
            delete m_pCurrentItem;
            m_pCurrentItem = NULL;
        }
        return true;
    }else
    {
        if (!FindEntry(szSection, szEntry))
            return false;  // entry inexistent
        if (m_pCurrentItem->pNext) m_pCurrentItem->pNext->pPrev = m_pCurrentItem->pPrev;
        if (m_pCurrentItem->pPrev) m_pCurrentItem->pPrev->pNext = m_pCurrentItem->pNext;
        if (m_pCurrentItem->pNext==NULL) m_pCurrentSection->pLastItem = m_pCurrentItem->pPrev;
        if (m_pCurrentItem->pPrev==NULL) m_pCurrentSection->pFirstItem = m_pCurrentItem->pNext;
        delete m_pCurrentItem;
        m_pCurrentItem = NULL;
        return true;
    }
}

bool RtIni::SetEntry(const char* szSection, const char* szEntry, const char* szValue)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    m_pCurrentItem->eType = VT_STRING;
    m_pCurrentItem->szValue = szValue;
    return true;
}

bool RtIni::SetEntry(const char* szSection, const char* szEntry, long lNumber)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    char szBuf[AZOTH_MAX_LONG_BUFLEN];
    sprintf(szBuf, "%d", lNumber);
    m_pCurrentItem->eType = VT_LONG;
    m_pCurrentItem->szValue = szBuf;
    return true;
}

bool RtIni::SetEntry(const char* szSection, const char* szEntry, double dNumber)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    char szBuf[AZOTH_MAX_DOUBLE_BUFLEN];
    sprintf(szBuf, "%f", dNumber);
    m_pCurrentItem->eType = VT_DOUBLE;
    m_pCurrentItem->szValue = szBuf;
    return true;
}

bool RtIni::SetEntry(const char* szSection, const char* szEntry, DWORD dwNumber)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    char szBuf[AZOTH_MAX_LONG_BUFLEN];
    sprintf(szBuf, "%u", dwNumber);
    m_pCurrentItem->eType = VT_LONG;
    m_pCurrentItem->szValue = szBuf;
    return true;
}

RtIni::ValueType RtIni::GetEntryType(const char* szSection, const char* szEntry)
{
    if (!FindEntry(szSection, szEntry))
        return VT_NONE;  // entry inexistent
    return m_pCurrentItem->eType;
}

const char* RtIni::GetEntry(const char* szSection, const char* szEntry)
{
    if (!FindEntry(szSection, szEntry))
        return NULL;  // entry inexistent
    return m_pCurrentItem->szValue.c_str();
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, long* pNumber)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    switch (m_pCurrentItem->eType)
    {
    case VT_LONG:
        *pNumber = atol(m_pCurrentItem->szValue.c_str());
        break;
    case VT_DOUBLE:
        *pNumber = (long)atof(m_pCurrentItem->szValue.c_str());
        break;
    case VT_STRING:
        *pNumber = atol(m_pCurrentItem->szValue.c_str());
        break;
    default:
        return false;
    }
    return true;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, double* pNumber)
{
    if (!FindEntry(szSection, szEntry))
        return false;  // entry inexistent
    switch (m_pCurrentItem->eType)
    {
    case VT_LONG:
        *pNumber = (double)atol(m_pCurrentItem->szValue.c_str());
        break;
    case VT_DOUBLE:
        *pNumber = atof(m_pCurrentItem->szValue.c_str());
        break;
    case VT_STRING:
        *pNumber = atof(m_pCurrentItem->szValue.c_str());
        break;
    default:
        return false;
    }
    return true;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, char* pStr, int iMaxLen)
{
    const char* pResult = GetEntry(szSection, szEntry);
    if (pResult==NULL)
        return false;
    //strncpy(pStr, pResult, iMaxLen);
    //pStr[iMaxLen-1] = 0;
	rt2_strncpy(pStr, pResult, iMaxLen);
    return true;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, bool* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = (lNum!=0);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, char* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<char>(lNum);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, BYTE* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<BYTE>(lNum);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, short* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<short>(lNum);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, WORD* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<WORD>(lNum);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, int* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<int>(lNum);
        return true;
    }
    return false;
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, DWORD* pNumber)
{
    long lNum;
    if (GetEntry(szSection, szEntry, &lNum))
    {
        *pNumber = static_cast<DWORD>(lNum);
        return true;
    }
    return false;
}

const char* RtIni::GetFilename()
{
    return m_szFilename.c_str();
}

bool RtIni::FirstSection(RtString* pName)
{
    m_pCurrentSection = m_pFirstSection;
    if (pName && m_pCurrentSection)
    {
        *pName = m_pCurrentSection->szName;
    }
    return m_pCurrentSection!=NULL;
}

bool RtIni::NextSection(RtString* pName)
{
    if (m_pCurrentSection)
    {
        m_pCurrentSection = m_pCurrentSection->pNext;
    }
    if (pName && m_pCurrentSection)
    {
        *pName = m_pCurrentSection->szName;
    }
    return m_pCurrentSection!=NULL;
}

bool RtIni::FirstEntry(RtString* pName, RtString* pValue, RtIni::ValueType* pType)
{
    if (m_pCurrentSection)
    {
        m_pCurrentItem = m_pCurrentSection->pFirstItem;
    }
    if (m_pCurrentItem)
    {
        if (pName)
        {
            *pName = m_pCurrentItem->szName;
        }
        if (pValue)
        {
            *pValue = m_pCurrentItem->szValue ;
        }
        if (pType)
        {
            *pType = m_pCurrentItem->eType;
        }
        return true;
    }
    return false;
}

bool RtIni::NextEntry(RtString* pName, RtString* pValue, RtIni::ValueType* pType)
{
    if (m_pCurrentItem)
    {
        m_pCurrentItem = m_pCurrentItem->pNext;
    }
    if (m_pCurrentItem)
    {
        if (pName)
        {
            *pName = m_pCurrentItem->szName;
        }
        if (pValue)
        {
            *pValue = m_pCurrentItem->szValue ;
        }
        if (pType)
        {
            *pType = m_pCurrentItem->eType;
        }
        return true;
    }
    return false;
}

int RtIni::ScanningAll(TCallbackINIVisit pCallbackVisit, void *arg)
{
#define AZOTH_INI_VISIT_ITEM(result, a1, a2, a3, a4, a5)    \
    if (pCallbackVisit) result = pCallbackVisit(a1, a2, a3, a4, a5); \
    else                result = this->OnVisit(a1, a2, a3, a4, a5);

    int iResult = 0;
    if (!FirstSection())
        return iResult;
    do
    {
        AZOTH_INI_VISIT_ITEM(iResult, AZOTH_INI_SECTION, m_pCurrentSection->szName.c_str(), 0, 0, arg);
        if (iResult)
            return iResult;
        if (FirstEntry())
        {
            do
            {
                AZOTH_INI_VISIT_ITEM(iResult, AZOTH_INI_ENTRY, m_pCurrentItem->szName.c_str(), m_pCurrentItem->szValue.c_str(), m_pCurrentItem->eType, arg);
                if (iResult)
                    return iResult;
            }while (NextEntry());
        }
    }while (NextSection());
    return 0;
#undef  AZOTH_INI_VISIT_ITEM
}

void RtIni::OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
{
}

int RtIni::OnVisit(int iItemType, const char* szArg1, const char* szArg2, int iValueType, void *arg)
{
    return 0;
}

bool RtIni::AddEntry(const char* szSection, const char* szEntry, float fNumber)
{
    double num=fNumber;
    return AddEntry(szSection, szEntry, num);
}

bool RtIni::SetEntry(const char* szSection, const char* szEntry, float fNumber)
{
    double num=fNumber;
    return SetEntry(szSection, szEntry, num);
}

bool RtIni::GetEntry(const char* szSection, const char* szEntry, float* pNumber)
{
    double num;
    bool bResult = GetEntry(szSection, szEntry, &num);
    if (bResult) *pNumber=(float)num;
    return bResult;
}

int RtIni::GetCount()
{
    int iCur = 0;
    SSection* p = m_pFirstSection;
    while (p)
    {
        iCur ++;
        p = p->pNext;
    }
    return iCur;
}

RtIni::SSection& RtIni::operator[](int nIndex)
{
    int iCur = 0;
    SSection* pCurrent = m_pFirstSection;
    while (pCurrent)
    {
        if (iCur==nIndex)
        {
            break;
        }
        iCur ++;
        pCurrent = pCurrent->pNext;
    }
    if (pCurrent)
    {
        return *pCurrent;
    }
    CHECKEX("ini array overbound");
    static SSection nullSection(this);
    return nullSection;
}

RtIni::SSection& RtIni::operator[](const char* szIdx)
{
    if (szIdx==NULL) return *m_pCurrentSection;

    if (m_pCurrentSection && stricmp(m_pCurrentSection->szName.c_str(), szIdx)==0)
    {
        return *m_pCurrentSection;
    }

    m_pCurrentSection = m_pFirstSection;
    while (m_pCurrentSection)
    {
        if (stricmp(m_pCurrentSection->szName.c_str(), szIdx)==0)
        {
            break;
        }
        m_pCurrentSection = m_pCurrentSection->pNext;
    }
    if (m_pCurrentSection)
    {
        return *m_pCurrentSection;
    }
    if (m_bCanWrite)
    {
        AddSection(szIdx);
        return *m_pCurrentSection;
    }else
    {
        RtCoreLog().Warn("INI: Cannot found section [%s] in [%s]\n", szIdx, m_szFilename.c_str());
        //CHECKEX("Cannot found section");
    }
    static SSection nullSection(this);
    return nullSection;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/

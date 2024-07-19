#include <stdio.h>
#include "core/rt2_core.h"
#include "rtc2_xml_i.h"
#include <stack>

namespace rt2_core {

static std::string s_strAzXmlItem;

BEGIN_EXTERN_C()

void callback_AzothError(const char* szFilename, const char* szErrMsg, long lLineNo, void *arg)
{
    ((RtScriptXML*)arg)->OnError(szFilename, szErrMsg, lLineNo);
}

void callback_AzothXmlStart(const char *pTag, void *arg)
{
    ((RtScriptXML*)arg)->OnXmlStart(pTag);
}

static int g_bAzXmlIsCloseTagEnd = 0;
static int s_bAzXmlHaveItems = FALSE;

void AzothXmlHaveItems(void *arg)
{
    if (s_bAzXmlHaveItems==TRUE)
    {
        if (!s_strAzXmlItem.empty())
        {
            ((RtScriptXML*)arg)->OnItem(s_strAzXmlItem.c_str());
        }
        s_strAzXmlItem = "";
        s_bAzXmlHaveItems = FALSE;
    }
}

void callback_AzothXmlTagStart(const char *pTag, void *arg)
{
    AzothXmlHaveItems(arg);
    g_bAzXmlIsCloseTagEnd = 0;
    ((RtScriptXML*)arg)->OnTagStart(pTag);
}

void callback_AzothXmlTagEnd(void *arg)
{
    AzothXmlHaveItems(arg);
    if (g_bAzXmlIsCloseTagEnd)
    {
        ((RtScriptXML*)arg)->OnCloseTagEnd();
    }else
    {
        ((RtScriptXML*)arg)->OnTagEnd();
    }
}

void callback_AzothXmlTagEndImme(void *arg)
{
    AzothXmlHaveItems(arg);
    ((RtScriptXML*)arg)->OnTagEndImme();
}

void callback_AzothXmlCloseTagStart(const char *pTag, void *arg)
{
    AzothXmlHaveItems(arg);
    g_bAzXmlIsCloseTagEnd = 1;
    ((RtScriptXML*)arg)->OnCloseTagStart(pTag);
}

void callback_AzothXmlTagAttrib(const char *pAttribute, const char *pValue, void *arg)
{
    ((RtScriptXML*)arg)->OnAttribute(pAttribute, pValue);
}

void callback_AzothXmlItem(char bSpace, const char *pItem, void *arg)
{
    static char bHaveSpace = 0;
    s_bAzXmlHaveItems = TRUE;
    if (bSpace==0)
    {
        if (bHaveSpace && !s_strAzXmlItem.empty())  s_strAzXmlItem += " ";
        s_strAzXmlItem += pItem;
        bHaveSpace = 0;
    }else
    {
        bHaveSpace = 1;
    }
    // ((RtScriptXML*)arg)->OnItem(pItem);
}

END_EXTERN_C()

///////////////////////////////////////////////////////////////////////

RtScriptXML::RtScriptXML()
{
}

RtScriptXML::~RtScriptXML()
{
}

void RtScriptXML::OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
{
}

const char* RtScriptXML::GetFilename()
{
    return m_strFilename.c_str();
}

int RtScriptXML::ParserFile(const char* szFilename, int bHtmlCompatible)
{
    int iResult = AzothParseXml(1, bHtmlCompatible, szFilename,
        callback_AzothError,
        callback_AzothXmlStart,
        callback_AzothXmlTagStart,
        callback_AzothXmlTagEnd,
        callback_AzothXmlTagEndImme,
        callback_AzothXmlCloseTagStart,
        callback_AzothXmlTagAttrib,
        callback_AzothXmlItem,
        this);
	releaseAllBuffer();

    if (iResult)
    {
        switch (iResult)
        {
        case -1:    // cannot open file
            break;
        default:    // parse error
            break;
        }
        return iResult;
    }
    m_strFilename = szFilename;
    return 0;
}

int RtScriptXML::ParserString(const char* szString, int bHtmlCompatible)
{
    int iResult = AzothParseXml(0, bHtmlCompatible, szString,
        callback_AzothError,
        callback_AzothXmlStart,
        callback_AzothXmlTagStart,
        callback_AzothXmlTagEnd,
        callback_AzothXmlTagEndImme,
        callback_AzothXmlCloseTagStart,
        callback_AzothXmlTagAttrib,
        callback_AzothXmlItem,
        this);
	releaseAllBuffer();

    if (iResult)
    {
        switch (iResult)
        {
        case -1:    // cannot open file
            break;
        default:    // parse error
            break;
        }
        return iResult;
    }
    m_strFilename = "String";
    return 0;
}

///////////////////////////////////////////////////////////////////////
// RtsXmlDoc
///////////////////////////////////////////////////////////////////////

const char* RtsXmlDoc::NodePtr::GetName()
{
    return strName.c_str();
}

bool RtsXmlDoc::NodePtr::FirstProp(const char** ppName, const char** ppValue)
{
    itAttr = mapAttribute.begin();
    if (itAttr!=mapAttribute.end())
    {
        *ppName  = (*itAttr).first.c_str();
        *ppValue = (*itAttr).second.c_str();
        itAttr ++;
        return true;
    }
    return false;
}

bool RtsXmlDoc::NodePtr::NextProp(const char** ppName, const char** ppValue)
{
    if (itAttr!=mapAttribute.end())
    {
        *ppName  = (*itAttr).first.c_str();
        *ppValue = (*itAttr).second.c_str();
        itAttr ++;
        return true;
    }
    return false;
}

const char* RtsXmlDoc::NodePtr::GetProp(const char* pPropName) const
{
    std::map<std::string, std::string>::const_iterator it = mapAttribute.find(std::string(pPropName));
    if (it!=mapAttribute.end())
    {
        return (*it).second.c_str();
    }
    return NULL;
}

const char* RtsXmlDoc::NodePtr::GetProp_s(const char* pPropName) const
{
    std::map<std::string, std::string>::const_iterator it = mapAttribute.find(std::string(pPropName));
    if (it!=mapAttribute.end())
    {
        return (*it).second.c_str();
    }
    return "";
}

int RtsXmlDoc::NodePtr::GetPropInt(const char* pPropName) const
{
    const char* pStr = GetProp(pPropName);
    if (pStr)
    {
        if (pStr[0]=='0' && (pStr[1]=='x'||pStr[1]=='X'))
        {
            return GetPropHex(pPropName);
        }
        return atoi(pStr);
    }
    return 0;
}

unsigned long RtsXmlDoc::NodePtr::GetPropDword(const char* pPropName) const
{
    const char* pStr = GetProp(pPropName);
    if (pStr)
    {
        if (pStr[0]=='0' && (pStr[1]=='x'||pStr[1]=='X'))
        {
            return (unsigned long)GetPropHex(pPropName);
        }
        return (unsigned long)atoi(pStr);
    }
    return 0;
}

int RtsXmlDoc::NodePtr::GetPropHex(const char* pPropName) const
{
    const char* pStr = GetProp(pPropName);
    if (pStr && pStr[0]!=0 && pStr[1]!=0)
    {
        if (pStr[0]=='0' && (pStr[1]=='x'||pStr[1]=='X'))
        {
            int iValue = 0;
            pStr += 2;
            while (*pStr)
            {
                iValue <<= 4;
                if (*pStr>='0' && *pStr<='9')
                {
                    iValue |= (*pStr-'0');
                }else if (*pStr>='a' && *pStr<='f')
                {
                    iValue |= (*pStr-'a')+10;
                }else if (*pStr>='A' && *pStr<='F')
                {
                    iValue |= (*pStr-'A')+10;
                }else
                {
                    return 0;
                }
                pStr ++;
            }
            return iValue;
        }
    }
    return 0;
}

float RtsXmlDoc::NodePtr::GetPropFloat(const char* pPropName) const
{
    const char* pStr = GetProp(pPropName);
    if (pStr)
    {
        return atof(pStr);
    }
    return 0;
}

bool RtsXmlDoc::NodePtr::GetPropBool(const char* pPropName) const
{
    const char* pStr = GetProp(pPropName);
    if (pStr)
    {
        if (stricmp(pStr, "true")==0)
            return true;
        else if (stricmp(pStr, "false")==0)
            return false;
        return atoi(pStr)!=0;
    }
    return false;
}

bool RtsXmlDoc::NodePtr::AddProp(const char* szPropName, const char* szPropValue)
{
    mapAttribute[std::string(szPropName)] = std::string(szPropValue);
    return true;
}

bool RtsXmlDoc::NodePtr::AddProp(const char* szPropName, unsigned long dwValue, bool bHex)
{
    char szBuf[50];
    if (bHex)
        sprintf(szBuf, "%u", dwValue);
    else
        sprintf(szBuf, "0x%X", dwValue);
    return AddProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::AddProp(const char* szPropName, int iValue, bool bHex)
{
    char szBuf[50];
    if (bHex)
        sprintf(szBuf, "%d", iValue);
    else
        sprintf(szBuf, "0x%X", iValue);
    return AddProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::AddProp(const char* szPropName, float fValue)
{
    char szBuf[50];
    sprintf(szBuf, "%.3f", fValue);
    return AddProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::AddProp(const char* szPropName, bool bValue)
{
    return AddProp(szPropName, (bValue?"true":"false"));
}

bool RtsXmlDoc::NodePtr::SetProp(const char* szPropName, const char* szPropValue)
{
    mapAttribute[std::string(szPropName)] = std::string(szPropValue);
    return true;
}

bool RtsXmlDoc::NodePtr::SetProp(const char* szPropName, unsigned long dwValue, bool bHex)
{
    char szBuf[50];
    if (bHex)
        sprintf(szBuf, "0x%X", dwValue);
    else
        sprintf(szBuf, "%u", dwValue);
    return SetProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::SetProp(const char* szPropName, int iValue, bool bHex)
{
    char szBuf[50];
    if (bHex)
        sprintf(szBuf, "0x%X", iValue);
    else
        sprintf(szBuf, "%d", iValue);
    return SetProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::SetProp(const char* szPropName, float fValue)
{
    char szBuf[50];
    sprintf(szBuf, "%.3f", fValue);
    return SetProp(szPropName, szBuf);
}

bool RtsXmlDoc::NodePtr::SetProp(const char* szPropName, bool bValue)
{
    return SetProp(szPropName, (bValue?"true":"false"));
}

bool RtsXmlDoc::NodePtr::DelProp(const char* szPropName)
{
    std::map<std::string, std::string>::iterator it = mapAttribute.find(std::string(szPropName));
    if (it!=mapAttribute.end())
    {
        mapAttribute.erase(it);
        return true;
    }
    return false;
}

RtsXmlDoc::NodePtr* RtsXmlDoc::NodePtr::AddItem(const char* szItemContent)
{
    CHECK(szItemContent);
    NodePtr* pNew = RT_NEW NodePtr;
    pNew->eType = XML_ITEM;
    pNew->pParent = this;
    pNew->pNext = NULL;
    pNew->pChildren = NULL;
    pNew->strName = szItemContent;

    NodePtr* pNode;
    if (pChildren)
    {
        pNode = pChildren;
        while (pNode->pNext)
        {
            pNode = pNode->pNext;
        }
        pNode->pNext = pNew;
    }else
    {
        pChildren = pNew;
    }
    return pNew;
}

bool RtsXmlDoc::NodePtr::DelItem(const char* szItemContent)
{
    NodePtr *pNode, *pDel;
    if (pChildren)
    {
        if (pChildren->eType==XML_ITEM)
        {
            if (strcmp(((NodePtr*)pChildren)->GetName(), szItemContent)==0)
            {
                pDel = pChildren;
                pChildren = pChildren->pNext;
                delete pDel;
                return true;
            }
        }
        pNode = pChildren;
        while (pNode->pNext)
        {
            if (pNode->pNext->eType==XML_ITEM)
            {
                if (strcmp(((NodePtr*)pNode->pNext)->GetName(), szItemContent)==0)
                {
                    pDel = pNode->pNext;
                    pNode->pNext = pNode->pNext->pNext;
                    delete pDel;
                    return true;
                }
            }
            pNode = pNode->pNext;
        }
    }
    return false;
}

RtsXmlDoc::NodePtr* RtsXmlDoc::NodePtr::AddTag(const char* szTagName)
{
    CHECK(szTagName);
    NodePtr* pNew = RT_NEW NodePtr;
    pNew->eType = XML_TAG;
    pNew->pParent = this;
    pNew->pNext = NULL;
    pNew->pChildren = NULL;
    pNew->strName = szTagName;

    NodePtr* pNode;
    if (pChildren)
    {
        pNode = pChildren;
        while (pNode->pNext)
        {
            pNode = pNode->pNext;
        }
        pNode->pNext = pNew;
    }else
    {
        pChildren = pNew;
    }
    return pNew;
}

bool RtsXmlDoc::NodePtr::DelTag(const char* szTagName, bool bAll)
{
    NodePtr *pNode, *pDel;
    if (pChildren)
    {
        if (pChildren->eType==XML_TAG)
        {
            if (strcmp(((NodePtr*)pChildren)->strName.c_str(), szTagName)==0)
            {
                pDel = pChildren;
                pChildren = pChildren->pNext;
                delete pDel;
                return true;
            }
        }
        pNode = pChildren;
        while (pNode->pNext)
        {
            if (pNode->pNext->eType==XML_TAG)
            {
                if (strcmp(((NodePtr*)pNode->pNext)->strName.c_str(), szTagName)==0)
                {
                    pDel = pNode->pNext;
                    pNode->pNext = pNode->pNext->pNext;
                    delete pDel;
                    if (!bAll)
                    {
                        return true;
                    }
                    continue;
                }
            }
            pNode = pNode->pNext;
        }
    }
    return bAll;
}

void RtsXmlWriteSpace(RtArchive& arc, bool bFormated, int iLevel)
{
    if (bFormated)
    {
        const char* szSpace = "   ";
        while (iLevel>0)
        {
            arc.Serialize((void*)szSpace, 3);
            iLevel --;
        }
    }
}

bool RtsXmlDoc::NodePtr::WriteToArchive(RtArchive& arc, bool bFormated, int iLevel)
{
    char szBuf[512];
    if (eType==XML_TAG)
    {
        RtsXmlWriteSpace(arc, bFormated, iLevel);
        sprintf(szBuf, "<%s", strName.c_str());
        arc.Serialize(szBuf, (int)strlen(szBuf));

        const char *pName, *pValue;
        if (FirstProp(&pName, &pValue))
        {
            do
            {
                sprintf(szBuf, " %s=\"%s\"", pName, pValue);
                arc.Serialize(szBuf, (int)strlen(szBuf));
            }while(NextProp(&pName, &pValue));
        }
        if (pChildren)
        {
            if (bFormated)  sprintf(szBuf, ">\n");
            else            sprintf(szBuf, ">");
            arc.Serialize(szBuf, (int)strlen(szBuf));

            NodePtr* pNode = pChildren;
            while (pNode)
            {
                pNode->WriteToArchive(arc, bFormated, iLevel+1);
                pNode = pNode->pNext;
            }

            RtsXmlWriteSpace(arc, bFormated, iLevel);
            if (bFormated)  sprintf(szBuf, "</%s>\n", strName.c_str());
            else            sprintf(szBuf, "</%s>", strName.c_str());
            arc.Serialize(szBuf, (int)strlen(szBuf));
        }else
        {
            if (bFormated)  sprintf(szBuf, "/>\n");
            else            sprintf(szBuf, "/>");
            arc.Serialize(szBuf, (int)strlen(szBuf));
        }
    }else
    {
        RtsXmlWriteSpace(arc, bFormated, iLevel);
        if (bFormated)  sprintf(szBuf, "%s\n", strName.c_str());
        else            sprintf(szBuf, "%s", strName.c_str());
        arc.Serialize(szBuf, (int)strlen(szBuf));
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

RtsXmlDoc::RtsXmlDoc()
{
    m_pRoot = NULL;
    m_pCurrent = NULL;
    m_strDocDesc = " version=\"1.0\" encoding=\"gb2312\"";
}

RtsXmlDoc::~RtsXmlDoc()
{
    Cleanup();
}

void RtsXmlDoc::Cleanup()
{
    if (m_pRoot==NULL) return;
    NodePtr *pNode, *pDelete;
    std::stack<NodePtr*> stackNode;
    
    pNode = m_pRoot;
    while (pNode || stackNode.size())
    {
        if (pNode)
        {
            stackNode.push(pNode);
            pNode = pNode->pChildren;
        }else
        {
            pDelete = stackNode.top();
            pNode = pDelete->pNext;
            delete pDelete;
            stackNode.pop();
        }
    }
    m_pRoot = NULL;
    m_pCurrent = NULL;
}

void RtsXmlDoc::OnXmlStart(const char *pTag)
{
    m_strDocDesc = pTag;
}

void RtsXmlDoc::OnTagStart(const char *pTag)
{
    CHECK(pTag);
    if (m_pRoot==NULL)
    {
        m_pRoot = RT_NEW NodePtr;
        m_pRoot->eType = XML_TAG;
        m_pRoot->pParent = NULL;
        m_pRoot->pNext = NULL;
        m_pRoot->pChildren = NULL;
        m_pRoot->strName = pTag;
        m_pCurrent = m_pRoot;
    }else
    {
        m_pCurrent = m_pCurrent->AddTag(pTag);
    }
}

void RtsXmlDoc::OnTagEnd()
{
}

void RtsXmlDoc::OnTagEndImme()
{
    m_pCurrent = m_pCurrent->pParent;
}

void RtsXmlDoc::OnCloseTagStart(const char *pTag)
{
    m_pCurrent = m_pCurrent->pParent;
}

void RtsXmlDoc::OnCloseTagEnd()
{
}

void RtsXmlDoc::OnAttribute(const char *pAttribute, const char *pValue)
{
    m_pCurrent->mapAttribute[std::string(pAttribute)] = std::string(pValue);
}

void RtsXmlDoc::OnItem(const char *pItem)
{
    CHECK(pItem);
    m_pCurrent->AddItem(pItem);
}

RtsXmlDoc::NodePtr* RtsXmlDoc::NewRoot(const char* szRootname)
{
    if (m_pRoot)
        return NULL;
    m_pRoot = RT_NEW NodePtr;
    m_pRoot->eType = XML_TAG;
    m_pRoot->pParent = NULL;
    m_pRoot->pNext = NULL;
    m_pRoot->pChildren = NULL;
	m_pRoot->strName = szRootname;
	m_pCurrent = m_pRoot;
    m_pCurrent = m_pRoot;
    return m_pCurrent;
}

bool RtsXmlDoc::WriteToFile(const char* szFilename)
{
    RtArchive* pArchive = RtCoreFile().CreateFileWriter(NULL, szFilename, RtFileManager::FW_Text);
    if (pArchive==NULL)
        return false;
    bool bResult = WriteToArchive(*pArchive, true);
    RtCoreFile().CloseFile(pArchive);
    return bResult;
}

bool RtsXmlDoc::WriteToArchive(RtArchive& arc, bool bFormated)
{
    if (!m_strDocDesc.empty())
    {
        char szBuf[256];
        if (bFormated)  sprintf(szBuf, "<?xml %s?>\n", m_strDocDesc.c_str());
        else            sprintf(szBuf, "<?xml %s?>\n", m_strDocDesc.c_str());
        arc.Serialize(szBuf, (int)strlen(szBuf));
    };
    return m_pRoot->WriteToArchive(arc, bFormated, 0);
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/

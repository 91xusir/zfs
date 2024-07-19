
#include "net/rt_net.h"
#include "business.h"

CCardTradeList::STradeRecord::STradeRecord()
{
	CM_MEMPROTECTOR(szSalesmanAccountName, 24)
	CM_MEMPROTECTOR(szSalesmanUsername, 24)
	CM_MEMPROTECTOR(szCardID, 36)
	CM_MEMPROTECTOR(szCardPwdMD5, 36)
	CM_MEMPROTECTOR(szCardInfo, 100)

    lConsignationID = 0;
    dwSalesmanAccountID = 0;
    szSalesmanAccountName[0] = 0;
    dwSalesmanUserID = 0;
    szSalesmanUsername[0] = 0;
    cCardType = 0;
    szCardID[0] = 0;
    szCardPwdMD5[0] = 0;
    szCardInfo[0] = 0;
    lPrice = 0;
    lStartTime = 0;
    lEndTime = 0;
}

CCardTradeList::STradeRecord::~STradeRecord()
{
	CM_MEMUNPROTECTOR(szSalesmanAccountName)
	CM_MEMUNPROTECTOR(szSalesmanUsername)
	CM_MEMUNPROTECTOR(szCardID)
	CM_MEMUNPROTECTOR(szCardPwdMD5)
	CM_MEMUNPROTECTOR(szCardInfo)
}

CCardTradeList::CCardTradeList()
{
    m_sTotal = 0;
    m_sStart = 0;
    m_sCount = 0;
    m_bModified = false;
}

CCardTradeList::~CCardTradeList()
{
    std::list<STradeRecord*>::iterator it;
    for (it=m_listFree.begin(); it!=m_listFree.end(); it++)
    {
        DEL_ONE(*it);
    }
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        DEL_ONE(*it);
    }
}

void CCardTradeList::Add(long lConsignationID,
        long dwSalesmanAccountID, const char* szSalesmanAccountName,
        long dwSalesmanUserID, const char* szSalesmanUsername,
        char cCardType, const char* szCardID, const char* szCardPwdMD5, long lCardGroupType,
        const char* szCardInfo, long lPrice, long lStartTime, long lEndTime)
{
    // ≈–∂œ «∑Ò÷ÿ∏¥
    if (Find(lConsignationID)) return;
    if (szCardID)
    {
        if (Find(szCardID)) return;
    }

    int i;
    if (m_listFree.size()==0)
    {
        for (i=0; i<50; i++)
        {
            m_listFree.push_back(RT_NEW STradeRecord);
        }
    }

    STradeRecord* pRecord = m_listFree.front();
    m_listFree.pop_front();
    m_listRecord.push_back(pRecord);

    pRecord->lConsignationID        = lConsignationID;
    pRecord->dwSalesmanAccountID    = dwSalesmanAccountID;
    pRecord->dwSalesmanUserID       = dwSalesmanUserID;
    pRecord->cCardType              = cCardType;
    pRecord->lPrice                 = lPrice;
    pRecord->lStartTime             = lStartTime;
    pRecord->lEndTime               = lEndTime;
    pRecord->lCardGroupType         = lCardGroupType;

    pRecord->szSalesmanAccountName[0] = 0;
    if (szSalesmanAccountName)
    {
        rt2_strncpy(pRecord->szSalesmanAccountName, szSalesmanAccountName, 24);
        //pRecord->szSalesmanAccountName[23]   = 0;
    }

    pRecord->szSalesmanUsername[0] = 0;
    if (szSalesmanUsername)
    {
        rt2_strncpy(pRecord->szSalesmanUsername, szSalesmanUsername, 24);
        //pRecord->szSalesmanUsername[23]   = 0;
    }

    pRecord->szCardID[0] = 0;
    if (szCardID)
    {
        rt2_strncpy(pRecord->szCardID, szCardID, 36);
        //pRecord->szCardID[35] = 0;
    }

    pRecord->szCardPwdMD5[0] = 0;
    if (szCardPwdMD5)
    {
        rt2_strncpy(pRecord->szCardPwdMD5, szCardPwdMD5, 36);
        //pRecord->szCardPwdMD5[35] = 0;
    }

    pRecord->szCardInfo[0] = 0;
    if (szCardInfo)
    {
        rt2_strncpy(pRecord->szCardInfo, szCardInfo, 100);
        //pRecord->szCardInfo[99] = 0;
    }

    m_bModified = true;
}

void CCardTradeList::Remove(long lConsignationID)
{
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        if ((*it)->lConsignationID==lConsignationID)
        {
            m_listFree.push_back(*it);
            m_listRecord.erase(it);
            break;
        }
    }
    m_bModified  = true;
}

void CCardTradeList::RemoveAll()
{
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        m_listFree.push_back(*it);
    }
    m_listRecord.clear();
    m_bModified  = true;
}

int CCardTradeList::GetCount()
{
    return (int)m_listRecord.size();
}

int CCardTradeList::GetCount(const char* szUsername)
{
    int iCount = 0;
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        if (strcmp((*it)->szSalesmanUsername,szUsername)==0)
        {
            iCount ++;
        }
    }
    return iCount;
}

int CCardTradeList::GetCount(long lUserID)
{
    int iCount = 0;
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        if ((*it)->dwSalesmanUserID==lUserID)
        {
            iCount ++;
        }
    }
    return iCount;
}

CCardTradeList::STradeRecord* CCardTradeList::Find(long lConsignationID)
{
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        if ((*it)->lConsignationID==lConsignationID)
        {
            return (*it);
        }
    }
    return NULL;
}

CCardTradeList::STradeRecord* CCardTradeList::Find(const char* szCardID)
{
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        if (strcmp((*it)->szCardID, szCardID)==0)
        {
            return (*it);
        }
    }
    return NULL;
}

bool CCardTradeList::WriteTo(CG_CmdPacket *pPacket)
{
    pPacket->WriteByte(1); // Version
    pPacket->WriteShort((short)m_listRecord.size());

    STradeRecord* pRecord;
    std::list<STradeRecord*>::iterator it;
    for (it=m_listRecord.begin(); it!=m_listRecord.end(); it++)
    {
        pRecord = *it;

        pPacket->WriteLong(pRecord->lConsignationID);
        pPacket->WriteByte(pRecord->cCardType);
        pPacket->WriteLong(pRecord->lCardGroupType);
        pPacket->WriteLong(pRecord->dwSalesmanUserID);
        pPacket->WriteString(pRecord->szSalesmanUsername);
        pPacket->WriteString(pRecord->szCardInfo);
        pPacket->WriteLong(pRecord->lPrice);
        pPacket->WriteLong(pRecord->lStartTime);
        pPacket->WriteLong(pRecord->lEndTime);
    }
    return true;
}

bool CCardTradeList::ReadFrom(CG_CmdPacket *pPacket)
{
    char cVersion, cCardType;
    short i, sCount;
    long lConsignationID, lPrice, lStartTime, lEndTime, lCardGroupType, dwSalesmanUserID;
    char *szSalesmanUsername, *szCardInfo;

    pPacket->ReadByte(&cVersion);

    if (cVersion!=1) return false;
    RemoveAll();

    pPacket->ReadShort(&sCount);

    for (i=0; i<sCount; i++)
    {
        pPacket->ReadLong(&lConsignationID);
        pPacket->ReadByte(&cCardType);
        pPacket->ReadLong(&lCardGroupType);
        pPacket->ReadLong(&dwSalesmanUserID);
        pPacket->ReadString(&szSalesmanUsername);
        pPacket->ReadString(&szCardInfo);
        pPacket->ReadLong(&lPrice);
        pPacket->ReadLong(&lStartTime);
        pPacket->ReadLong(&lEndTime);

        Add(lConsignationID, 0, NULL, dwSalesmanUserID, szSalesmanUsername, cCardType,
            NULL, NULL, lCardGroupType, szCardInfo, lPrice, lStartTime, lEndTime);
    }
    return true;
}

bool CCardTradeList::WriteToBlock(short sStart, short sCount, CG_CmdPacket *pPacket)
{
    int iCnt = GetCount();
    if (sCount<0 || sCount>24) sCount = 24;
    if (sStart>iCnt)
    {
        sStart = (iCnt/sCount)*sCount;
        if (iCnt==sStart)
        {
            sStart = iCnt-sCount;
        }
    }
    if (sStart<0)
    {
        sStart = 0;
    }
    if ((iCnt-sStart)<sCount) sCount = iCnt-sStart;

    pPacket->WriteByte(1); // Version

    pPacket->WriteShort((short)iCnt);
    pPacket->WriteShort((short)sStart);
    pPacket->WriteShort((short)sCount);

    STradeRecord* pRecord;
    std::list<STradeRecord*>::iterator it;

    it = m_listRecord.begin();
    for (iCnt=0; iCnt<sStart; iCnt++)
    {
        it ++;
    }

    for (iCnt=0; it!=m_listRecord.end() && iCnt<sCount; it++, iCnt++)
    {
        pRecord = *it;

        pPacket->WriteLong(pRecord->lConsignationID);
        pPacket->WriteLong(pRecord->lCardGroupType);
        pPacket->WriteLong(pRecord->dwSalesmanUserID);
        pPacket->WriteString(pRecord->szSalesmanUsername);
        pPacket->WriteLong(pRecord->lPrice);
        pPacket->WriteLong(pRecord->lStartTime);
        pPacket->WriteLong(pRecord->lEndTime);
    }
    return true;
}

bool CCardTradeList::ReadFromBlock(CG_CmdPacket *pPacket)
{
    char cVersion, cCardType = 0;
    short i;
    long lConsignationID, lSalesmanUserID, lPrice, lStartTime, lEndTime, lCardGroupType;
    char* szSalesmanUsername = 0;
	char const* szCardInfo = "";

    pPacket->ReadByte(&cVersion);
    if (cVersion!=1) return false;
    RemoveAll();

    pPacket->ReadShort(&m_sTotal);
    pPacket->ReadShort(&m_sStart);
    pPacket->ReadShort(&m_sCount);

    for (i=0; i<m_sCount; i++)
    {
        pPacket->ReadLong(&lConsignationID);
        pPacket->ReadLong(&lCardGroupType);
        pPacket->ReadLong(&lSalesmanUserID);
        pPacket->ReadString(&szSalesmanUsername);
        pPacket->ReadLong(&lPrice);
        pPacket->ReadLong(&lStartTime);
        pPacket->ReadLong(&lEndTime);

        Add(lConsignationID, 0, NULL, lSalesmanUserID, szSalesmanUsername, cCardType,
            NULL, NULL, lCardGroupType, szCardInfo, lPrice, lStartTime, lEndTime);
    }
    return true;
}

CCardTradeList::STradeRecord* CCardTradeList::FirstRecord()
{
    m_itRecord = m_listRecord.begin();
    if (m_itRecord==m_listRecord.end())
    {
        return NULL;
    }

    STradeRecord* pResult = *m_itRecord;
    m_itRecord ++;
    return pResult;
}

CCardTradeList::STradeRecord* CCardTradeList::NextRecord()
{
    if (m_itRecord==m_listRecord.end())
    {
        return NULL;
    }

    STradeRecord* pResult = *m_itRecord;
    m_itRecord ++;
    return pResult;
}

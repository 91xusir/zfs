#ifndef _BUSINESS_H_
#define _BUSINESS_H_

class CCardTradeList
{
public:
    struct STradeRecord
    {
        long    lConsignationID;            // 寄卖ID, 这个是数据库上的唯一ID
        long    dwSalesmanAccountID;
        //char    szSalesmanAccountName[24];  // 卖的人的用户名
		CM_MEMDEF(szSalesmanAccountName, 24)
        long    dwSalesmanUserID;
        //char    szSalesmanUsername[24];     // 卖的人的角色名
		CM_MEMDEF(szSalesmanUsername, 24)
        char    cCardType;                  // 卡片类型
        //char    szCardID[36];               // 卡号
		CM_MEMDEF(szCardID, 36)
        //char    szCardPwdMD5[36];           // 密码
		CM_MEMDEF(szCardPwdMD5, 36)
        long    lCardGroupType;             // 卡片组类型
        //char    szCardInfo[100];            // 卡片信息(不能超过49个汉字)
		CM_MEMDEF(szCardInfo, 100)
        long    lPrice;                     // 价格
        long    lStartTime;                 // 寄售时间
        long    lEndTime;                   // 超时时间
		STradeRecord();
		~STradeRecord();
    };
    enum
    {
        e_lTax               =    200000, //    20*10000
        e_lMinPrice          =   200000, //   20*10000
        e_lMaxPrice          = 100000000, // 10000*10000
        e_lMaxRecords        =      100,
        e_lMaxRecordsPerUser =        5,
        e_lDefaultTradeTime  =    21600, // 6*60*60
    };
public:
	CCardTradeList();
	~CCardTradeList();

public:
    void Add(long lConsignationID,
        long dwSalesmanAccountID, const char* szSalesmanAccountName,
        long dwSalesmanUserID, const char* szSalesmanUsername,
        char cCardType, const char* szCardID, const char* szCardPwdMD5, long lCardGroupType,
        const char* szCardInfo, long lPrice, long lStartTime, long lEndTime);
    void Remove(long lConsignationID);
    void RemoveAll();

    int GetCount();
    int GetCount(const char* szUsername);
    int GetCount(long lUserID);
    STradeRecord* Find(long lConsignationID);
    STradeRecord* Find(const char* szCardID);

public:
    bool WriteTo(CG_CmdPacket *pPacket);
    bool ReadFrom(CG_CmdPacket *pPacket);

    bool WriteToBlock(short sStart, short sCount, CG_CmdPacket *pPacket);
    bool ReadFromBlock(CG_CmdPacket *pPacket);

    STradeRecord* FirstRecord();
    STradeRecord* NextRecord();

public:
    short   m_sTotal, m_sStart, m_sCount;       // for client

    bool    m_bModified;

private:
    std::list<STradeRecord*>            m_listFree;
    std::list<STradeRecord*>            m_listRecord;
    std::list<STradeRecord*>::iterator  m_itRecord;
};


#endif//_BUSINESS_H_

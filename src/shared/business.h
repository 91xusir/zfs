#ifndef _BUSINESS_H_
#define _BUSINESS_H_

class CCardTradeList
{
public:
    struct STradeRecord
    {
        long    lConsignationID;            // ����ID, ��������ݿ��ϵ�ΨһID
        long    dwSalesmanAccountID;
        //char    szSalesmanAccountName[24];  // �����˵��û���
		CM_MEMDEF(szSalesmanAccountName, 24)
        long    dwSalesmanUserID;
        //char    szSalesmanUsername[24];     // �����˵Ľ�ɫ��
		CM_MEMDEF(szSalesmanUsername, 24)
        char    cCardType;                  // ��Ƭ����
        //char    szCardID[36];               // ����
		CM_MEMDEF(szCardID, 36)
        //char    szCardPwdMD5[36];           // ����
		CM_MEMDEF(szCardPwdMD5, 36)
        long    lCardGroupType;             // ��Ƭ������
        //char    szCardInfo[100];            // ��Ƭ��Ϣ(���ܳ���49������)
		CM_MEMDEF(szCardInfo, 100)
        long    lPrice;                     // �۸�
        long    lStartTime;                 // ����ʱ��
        long    lEndTime;                   // ��ʱʱ��
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

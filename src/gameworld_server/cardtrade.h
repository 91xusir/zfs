
#ifndef _GW_CARD_TRADE_H_
#define	_GW_CARD_TRADE_H_

#include "business.h"

/*** ����˵�� *************************************************************************
    
    һ, ����
        1, ����ڼ���NPC�����뿨�š�������ۼ� (Region�ж��ۼ۷�Χ)
        2, Region�Ѹ���ҵ�ID�����š�������ۼ۴���Gameworld
        3, Gameworld�Ѹü����������һ�������������, ͬʱ��Login����Ƿ�������
        4, ���Login����������
           5, ��Ӽ�����Ϣ�����ݿ���
           6, ��Ӽ�����Ϣ���б���
           7, �������˵�����ɹ�
        8, ���Login����ʧ�ܻ򷵻س�ʱ������ӵ����ݿ���̳���
           9, �������˵����ʧ��

        * Ҫ��ֹ����������

    ��, ����
        1, ����ڹ���NPC��ѡ��
        2, Region�����ҵĽ�Ǯ��Ϣ����ʱ��ȡ��ҽ�Ǯ����֪ͨGameworld
        3, Gameworld��Login�����ֵ
        4, �����ֵ�ɹ�
           5, ��Gameworld�����������ʺ�������ӽ�Ǯ����Ϣ
           6, ɾ���������ݿ��¼
           7, ɾ�����ۼ�¼
           8, Ȼ��֪ͨRegion��Ǯ
        9, �����ֵʧ�ܣ���ֱ��֪ͨRegion�ѿ۵�Ǯ�黹

        * Ҫ��ֹ����ͬʱ����
        * Ҫ��ֹһ����ι���

    ��, ȡ������

***************************************************************************************/

class GwCardTrade
{
private:
    // ����ID
    struct SRequisition
    {
        long    lRequisitionID;
        long    dwSalesmanAccountID;
        //char    szSalesmanAccountName[24];  // �����˵��û���
		CM_MEMDEF(szSalesmanAccountName, 24)
        long    dwSalesmanUserID;
        //char    szSalesmanUsername[24];     // �����˵Ľ�ɫ��
		CM_MEMDEF(szSalesmanUsername, 24)
		char    cCardType;
        int     iRequisitionType;           // 1 �Ƿ�������, 2 ��ֵ
        //char    szCardID[36];
		CM_MEMDEF(szCardID, 36)
        //char    szCardPwdMD5[36];
		CM_MEMDEF(szCardPwdMD5, 36)
        long    lPrice;
        long    lRequisitionTime;
        long    lBuyerUserID;
        long    lBuyConsignationID;

		SRequisition()
		{
			CM_MEMPROTECTOR(szSalesmanAccountName, 24)
			CM_MEMPROTECTOR(szSalesmanUsername, 24)
			CM_MEMPROTECTOR(szCardID, 36)
			CM_MEMPROTECTOR(szCardPwdMD5, 36)
		}
		SRequisition(const SRequisition &object)
		{
			CM_MEMPROTECTOR(szSalesmanAccountName, 24)
			CM_MEMPROTECTOR(szSalesmanUsername, 24)
			CM_MEMPROTECTOR(szCardID, 36)
			CM_MEMPROTECTOR(szCardPwdMD5, 36)
		}
		~SRequisition()
		{
			CM_MEMUNPROTECTOR(szSalesmanAccountName)
			CM_MEMUNPROTECTOR(szSalesmanUsername)
			CM_MEMUNPROTECTOR(szCardID)
			CM_MEMUNPROTECTOR(szCardPwdMD5)
		}
    };
public:
	GwCardTrade();
	~GwCardTrade();

    bool Startup();

    void Run();

    void RegionModify();

public:
    // �����㿨
    bool Consign(long lUserID, char cCardType, const char* szCardID, const char* szCardPwd, long lPrice);

    // ����㿨
    bool BuyCard(long lUserID, long lPriceSecond, long lConsignationID);

    // ȡ������
    bool CancelConsign(long lUserID, long lConsignationID);
    bool CancelConsign(long lUserID, const char* szCardID);

    void OnCheckCardCanTrade(long lRequisitionID, long lUserID, const char* szCardID, long lCardGroupType, char cCan, const char* szMsg);
    void OnChargeResult(long dwUserParam, long lUserID, const char* szCardID, char cSucceed, const char* szMsg);

    void UpdateCardTradeList();

private:
    void OnConsignSucceeded(SRequisition* pRequisition, long lCardGroupType, const char* szCardInfo);
    void OnConsignFailed(SRequisition* pRequisition, long lCardGroupType, const char* szCardInfo);
    void OnConsignFailed(long lUserID, const char* szCardID, long lCardGroupType, const char* szCardInfo);

    void OnBuyCardSucceeded(SRequisition* pRequisition, const char* szCardInfo);
    void OnBuyCardFailed(SRequisition* pRequisition, const char* szCardInfo);
    void OnBuyCardFailed(long lUserID, long lConsignationID, long lPrice, const char* szCardID, const char* szCardInfo);

    enum ECardOP
    {
        ECOP_CONSIGN            = 1,
        ECOP_BUY_SUCCEEDED      = 2,
        ECOP_BUY_FAILED         = 3,
        ECOP_ADD_GIFT_FAILED    = 4,
        ECOP_BUY                = 5,
    };
    void Log(ECardOP eCOP, const char* szCardID, long lUserID, long lPrice);

private:
    long AddDBRecord(char cCardType, const char* szCardID, const char* szCardPwdMD5,
        long lGroupType, const char* szCardInfo,
        long lAccountID, const char* szAccountName,
        long lUserID, const char* szUsername, long lPrice, long lStartTime, long lEndTime);
    void RemoveDBRecord(long lConsignationID);

private:
    bool                            m_bNeedUpdateToRegion;
    // ���׵��б�
    CCardTradeList                  m_list;
    long                            m_lRequisitionID;
    std::map<long, SRequisition>    m_mapRequisition;

};

#endif//_GW_CARD_TRADE_H_

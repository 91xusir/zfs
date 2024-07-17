
#ifndef _GW_AUCTION_H_
#define	_GW_AUCTION_H_

#include "auction_common.h"

/*
 * ע�⣺��gameworld���޷������ĳЩ���ݽ��в������������ͨ������ָ�region����
 */


class GwAuction
{
public:
	GwAuction();
	~GwAuction();

    bool Startup();
    void Shutdown();

    void Run();

    void SaveToDatabase();

    void RegionModify();

public:
    // ����
    bool Auction(long lUserID, SItemID id, long dwDuration, long lBasePrice, long lUniquePrice);

    // Ͷ��
    bool Bid(long lUserID, long lBidPrice, long lAuctionId);

    // ȡ������
    bool CancelAuction(long lUserID, long lAuctionId);

    // ȡ������
    bool CancelBid(long lUserID, long lAuctionId);

    // ȡ��һ����������
    bool GetItems(long lUserID, long lFreeSpace);

    // �����б�Region
    void UpdateAuctionListToRegion();

    // ���µ��ͻ���
    void UpdateToClient(int iCnt, long lUserID1, long lUserID2=0, long lUserID3=0);

    void OnGetItemRet(long lUserID, char cMoney, long lMoney, char cItem, SItemID& itemID);

    SAuctionItem* FindAuction(long lAuctionId, long lUserID);
    int GetUserAuctionCnt(long lUserID);
    int GetAccountAuctionCnt(long lAccountID);

    bool UpdateDibToDatabase(SAuctionItem* pAuctionItem);

private:
    void OnAuctionFailed(SItemID id, long lUserID, short sType);
    void OnAuctionSucceeded(SAuctionItem* pAuctionItem); // �����¼��Log�ͷ��͵�Region
    void OnAuctionCanceled(SAuctionItem* pAuctionItem);  // �����¼��Log�ͷ��͵�Region
    void OnAuctionFinished(SAuctionItem* pAuctionItem);  // �����¼��Log�ͷ��͵�Region
    void RemoveAuction(SAuctionItem* pAuctionItem);

    void OnBidFailed(long lUserID, long lBidPrice, short sType);
    void OnBidSucceeded(SAuctionItem* pAuctionItem);
    void OnBidOvertake(SAuctionItem* pAuctionItem);
    void OnBidCanceled(SAuctionItem* pAuctionItem);
    void OnBidSubmission(SAuctionItem* pAuctionItem);

private:
    bool ConvertOldVersion();

    bool AddDBRecord(SAuctionItem* pAuctionItem);
    bool RemoveDBRecord(SAuctionItem* pAuctionItem);
    void AddMoney(long lUserID, long lMoney);
    void AddItem(long lUserID, SItemID item);
    void Log(EAuctionOP eOP, SAuctionItem* pAuctionItem);
    void SendToAllRegion(EAuctionOP eOP, SAuctionItem* pAuctionItem);

private:
    long                        m_lLastRunTime;
    long                        m_lLastUpdateTime;
    bool                        m_bNeedUpdateToRegion;
    std::list<SAuctionItem>     m_listAuction;
};

#endif//_GW_AUCTION_H_

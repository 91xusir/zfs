
#ifndef _GW_AUCTION_H_
#define	_GW_AUCTION_H_

#include "auction_common.h"

/*
 * 注意：在gameworld上无法对玩家某些数据进行操作，此种情况通过发送指令到region进行
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
    // 拍卖
    bool Auction(long lUserID, SItemID id, long dwDuration, long lBasePrice, long lUniquePrice);

    // 投标
    bool Bid(long lUserID, long lBidPrice, long lAuctionId);

    // 取消寄卖
    bool CancelAuction(long lUserID, long lAuctionId);

    // 取消寄卖
    bool CancelBid(long lUserID, long lAuctionId);

    // 取得一个拍卖所得
    bool GetItems(long lUserID, long lFreeSpace);

    // 更新列表到Region
    void UpdateAuctionListToRegion();

    // 更新到客户端
    void UpdateToClient(int iCnt, long lUserID1, long lUserID2=0, long lUserID3=0);

    void OnGetItemRet(long lUserID, char cMoney, long lMoney, char cItem, SItemID& itemID);

    SAuctionItem* FindAuction(long lAuctionId, long lUserID);
    int GetUserAuctionCnt(long lUserID);
    int GetAccountAuctionCnt(long lAccountID);

    bool UpdateDibToDatabase(SAuctionItem* pAuctionItem);

private:
    void OnAuctionFailed(SItemID id, long lUserID, short sType);
    void OnAuctionSucceeded(SAuctionItem* pAuctionItem); // 负责记录到Log和发送到Region
    void OnAuctionCanceled(SAuctionItem* pAuctionItem);  // 负责记录到Log和发送到Region
    void OnAuctionFinished(SAuctionItem* pAuctionItem);  // 负责记录到Log和发送到Region
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

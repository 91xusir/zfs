
#ifndef _RS_AUCTION_H_
#define	_RS_AUCTION_H_

#include "auction_common.h"

class RsAuction
{
public:
    // 拍卖
    bool Auction(CRegionCreature* CmdGiver, SItemID id, long dwDuration, long lBasePrice, long lUniquePrice);

    // 投标
    bool Bid(CRegionCreature* CmdGiver, long lBidPrice, long lAuctionId);

    // 取消寄卖
    bool CancelAuction(CRegionCreature* CmdGiver, long lAuctionId);

    // 取消竞拍
    bool CancelBid(CRegionCreature* CmdGiver, long lAuctionId);

    // 取得一个拍卖所得
    bool GetItems(CRegionCreature* CmdGiver);

    SAuctionItem* FindAuction(long lAuctionId, long lUserID);
    SAuctionItem* FindAuctionByItem(SItemID& id, long lUserID);
    int GetUserAuctionCnt(long lUserID);

public:
    void OnUpdateAuction(CG_CmdPacket *cmd);
    void OnEvent(EAuctionOP eOP, SAuctionItem* pAuctionItem);
    void OnUpdateToClient(CG_CmdPacket *cmd);

    void OnAuctionFailed(SItemID id, long lUserID, short sType);
    void OnBidFailed(long lUserID, long lBidPrice, short sType);

    void OnGetItems(long lUserID, char cMoney, long lMoney, char cItem, SItemID itemID);

    void RemoveAuction(long lAuctionId);
    void UpdateAuction(SAuctionItem* pItem);

public:
    std::list<SAuctionItem> m_listAuction;
};


#endif//_RS_AUCTION_H_

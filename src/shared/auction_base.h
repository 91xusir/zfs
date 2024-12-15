#ifndef AUCTION_BASE_H
#define AUCTION_BASE_H

#include "region_client.h"
#include "item_base.h"

#define MAX_ITEM_PER_ACC 3
#define MAX_ITEM_TOTAL 20

#define AUCTION_ITEM_DEALING 0
#define AUCTION_ITEM_DEAL_SUCCESS 1
#define AUCTION_ITEM_DEAL_FAILED 2

#define AUCTION_METIER_ALL 0      //无职业
#define AUCTION_METIER_WARRIOR 1  //战士
#define AUCTION_METIER_HUNTER 2   //凤舞
#define AUCTION_METIER_WIZARD 3   //术士
#define AUCTION_METIER_TAOIST 4   //道士

#define AUCTION_CHARGE_BASE 1000

#define AUCTION_SALER 0
#define AUCTION_BUYER 1

class CAuctionItem  // : public SItem
{
   public:
    CAuctionItem();
    CAuctionItem(const CAuctionItem& rhs);
    ~CAuctionItem();

   public:
    bool Serialize(CG_CmdPacket* cmd);

    bool operator==(CAuctionItem& rhs) {
        return (m_lAuctionId == rhs.m_lAuctionId) && (m_id.id == rhs.m_id.id) &&
               (m_dwBargainorAcc == rhs.m_dwBargainorAcc) &&
               (m_dwBargainorChr == rhs.m_dwBargainorChr) &&
               (m_dwPurchaserAcc == rhs.m_dwPurchaserAcc) &&
               (m_dwPurchaserChr == rhs.m_dwPurchaserChr) && (m_dwBasePrice == rhs.m_dwBasePrice) &&
               (m_dwBidPrice == rhs.m_dwBidPrice) && (m_dwUniquePrice == rhs.m_dwUniquePrice);
    }

    bool operator!=(CAuctionItem& rhs) { return m_id != rhs.m_id; }

    //CAuctionItem& operator = (const CAuctionItem& rhs)
    //{
    //
    //}

   public:
    long AuctionId() { return m_lAuctionId; }

    SItemID ItemID() { return m_id; }

    //DWORD ItemID()			{ return id.id; }
    DWORD Bargainor() { return m_dwBargainorAcc; }

    DWORD BargainorChrID() { return m_dwBargainorChr; }

    std::string BargainorChr() { return m_strBargainorChr; }

    DWORD Purchaser() { return m_dwPurchaserAcc; }

    DWORD PurchaserChrID() { return m_dwPurchaserChr; }

    std::string PurchaserChr() { return m_strPurchaserChr; }

    DWORD PassTime() { return m_dwPassTime; }

    DWORD Duration() { return m_dwDuration; }

    DWORD HandlingCharge() { return m_dwHandlingCharge; }

    DWORD BasePrice() { return m_dwBasePrice; }

    DWORD BidPrice() { return m_dwBidPrice; }

    DWORD UniquePrice() { return m_dwUniquePrice; }

    //void ItemID(DWORD dw)			{ id.id = dw; }
    void AuctionId(long id) { m_lAuctionId = id; }

    void ItemID(SItemID sid) { m_id = sid; }

    void Bargainor(DWORD dw) { m_dwBargainorAcc = dw; }

    void Purchaser(DWORD dw) { m_dwPurchaserAcc = dw; }

    void BargainorChrID(DWORD dw) { m_dwBargainorChr = dw; }

    void BargainorChr(char const* str) { m_strBargainorChr = str; }

    void PurchaserChrID(DWORD dw) { m_dwPurchaserChr = dw; }

    void PurchaserChr(char const* str) { m_strPurchaserChr = str; }

    void PassTime(DWORD dw) { m_dwPassTime = dw; }

    void Duration(DWORD dw) { m_dwDuration = dw; }

    void HandlingCharge(DWORD dw) { m_dwHandlingCharge = dw; }

    void BasePrice(DWORD dw) { m_dwBasePrice = dw; }

    //void BidPrice(DWORD dw)			{ m_dwBidPrice = dw; }
    void UniquePrice(DWORD dw) { m_dwUniquePrice = dw; }

    void BidPrice(DWORD dw) {
        m_dwBidPrice = dw;

        HandlingCharge((long)dw * 0.05);
    }

   private:
    long        m_lAuctionId;
    SItemID     m_id;
    DWORD       m_dwBargainorAcc;
    DWORD       m_dwBargainorChr;
    std::string m_strBargainorChr;
    DWORD       m_dwPurchaserAcc;
    DWORD       m_dwPurchaserChr;
    std::string m_strPurchaserChr;
    DWORD       m_dwPassTime;
    DWORD       m_dwDuration;
    DWORD       m_dwHandlingCharge;
    DWORD       m_dwBasePrice;
    DWORD       m_dwBidPrice;
    DWORD       m_dwUniquePrice;
};

class CSalesroom {
   public:
    CSalesroom() {
        m_DealingItemList.clear();
        m_DealSuccessItemList.clear();
        m_DealFailedItemList.clear();
    }

    ~CSalesroom() {}

   public:
    //bool	DealingItem(CAuctionItem item);

    bool AddItem(CAuctionItem item, int type, int who = -1);
    bool DelItem(CAuctionItem item, int type);
    bool UpdateItem(CAuctionItem item, int type);
    bool UpdateSuccessItem(CAuctionItem item, int who);

    CAuctionItem*            FindDealingItemById(SItemID item_id);
    CAuctionItem*            FindDealSucessItemById(SItemID item_id);
    CAuctionItem*            FindDealFailedItemById(SItemID item_id);
    std::list<CAuctionItem>* FindDealSuccessItemBySalerAccId(DWORD acc_id);
    std::list<CAuctionItem>* FindDealSuccessItemByBuyerAccId(DWORD acc_id);
    std::list<CAuctionItem>* FindDealFailedItemBySalerAccId(DWORD acc_id);
    std::list<CAuctionItem>* FindDealFailedItemByBuyerAccId(DWORD acc_id);

    int GetSaleItemNumByAcc(DWORD acc_id);
    int GetBidItemNumByAcc(DWORD acc_id);
    //int		GetItemNumByClass(CAuctionItem* item);

    bool Serialize(CG_CmdPacket* cmd);
    bool SerializeDealing(CG_CmdPacket* cmd);
    void Clear();

    std::list<CAuctionItem>* GetDealing() { return &m_DealingItemList; }

    std::list<CAuctionItem>* GetDealSuccess() { return &m_DealSuccessItemList; }

    std::list<CAuctionItem>* GetDealFailed() { return &m_DealFailedItemList; }

   public:
    std::list<CAuctionItem> m_DealingItemList;
    std::list<CAuctionItem> m_DealSuccessItemList;
    std::list<CAuctionItem> m_DealFailedItemList;
};

#endif  //AUCTION_BASE_H

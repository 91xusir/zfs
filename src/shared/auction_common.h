
#ifndef _AUCTION_COMMON_H_
#define	_AUCTION_COMMON_H_

enum EAuctionOP
{
    EAOP_AUCTION            = 1,
    EAOP_AUCTION_FAILED,
    EAOP_AUCTION_SUCCEEDED,
    EAOP_AUCTION_CANCELED,
    EAOP_AUCTION_FINISHED,
    EAOP_BID_FAILED,
    EAOP_BID_SUCCEEDED,
    EAOP_BID_OVERTAKE,
    EAOP_BID_CANCELED,
    EAOP_BID_SUBMISSION,            // 提交上列表
    EAOP_UPDATE_TO_CLIENT,
    EAOP_GET_ITEM,
};

struct SAuctionItem
{
    enum EAuctionConfig
    {
        e_lMaxRecords = 180,
        e_lUserMaxRecords = 3,
    };
public:
    int         iIdx;
	SItemID     itemID;              // 道具
    long        lBargainorAcc;
	long	    lBargainorChr;       // 拍卖者
    std::string strBargainorChr;
    long        lPurchaserAcc;
	long	    lPurchaserChr;       // 购买者
	std::string strPurchaserChr;
	long	    lPassTime;           // 拍卖过去的时间(秒)
	long	    lDuration;           // 期限(小时)
	long	    lHandlingCharge;     // 
	long	    lBasePrice;          // 低价
	long	    lBidPrice;           // 当前价
	long	    lUniquePrice;        // 一口价

	bool Serialize(CG_CmdPacket *cmd)
    {
	    *cmd << iIdx;
	    itemID.Serialize(cmd);
	    *cmd << lBargainorAcc;
	    *cmd << lBargainorChr;
	    *cmd << strBargainorChr;
	    *cmd << lPurchaserAcc;
	    *cmd << lPurchaserChr;
	    *cmd << strPurchaserChr;
	    *cmd << lPassTime;
	    *cmd << lDuration;
	    *cmd << lHandlingCharge;
	    *cmd << lBasePrice;
	    *cmd << lBidPrice;
	    *cmd << lUniquePrice;
	    return (!cmd->IsError());
    }
};

#endif//_AUCTION_COMMON_H_

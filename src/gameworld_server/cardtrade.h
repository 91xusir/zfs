
#ifndef _GW_CARD_TRADE_H_
#define	_GW_CARD_TRADE_H_

#include "business.h"

/*** 流程说明 *************************************************************************
    
    一, 寄卖
        1, 玩家在寄卖NPC处输入卡号、密码和售价 (Region判断售价范围)
        2, Region把该玩家的ID、卡号、密码和售价传给Gameworld
        3, Gameworld把该寄卖请求加入一个寄卖请求队列, 同时向Login检查是否允许交易
        4, 如果Login返回允许交易
           5, 添加寄卖信息到数据库中
           6, 添加寄卖信息到列表中
           7, 返回玩家说寄卖成功
        8, 如果Login返回失败或返回超时，或添加到数据库过程出错
           9, 返回玩家说寄卖失败

        * 要防止多次申请寄卖

    二, 购买
        1, 玩家在购买NPC处选择卡
        2, Region检查玩家的金钱信息，临时扣取玩家金钱，并通知Gameworld
        3, Gameworld向Login申请充值
        4, 如果充值成功
           5, 在Gameworld上向寄售玩家帐号里面添加金钱和信息
           6, 删除寄售数据库记录
           7, 删除寄售记录
           8, 然后通知Region扣钱
        9, 如果充值失败，就直接通知Region把扣的钱归还

        * 要防止多人同时购买
        * 要防止一个多次购买

    三, 取消寄卖

***************************************************************************************/

class GwCardTrade
{
private:
    // 请求ID
    struct SRequisition
    {
        long    lRequisitionID;
        long    dwSalesmanAccountID;
        //char    szSalesmanAccountName[24];  // 卖的人的用户名
		CM_MEMDEF(szSalesmanAccountName, 24)
        long    dwSalesmanUserID;
        //char    szSalesmanUsername[24];     // 卖的人的角色名
		CM_MEMDEF(szSalesmanUsername, 24)
		char    cCardType;
        int     iRequisitionType;           // 1 是否允许交易, 2 充值
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
    // 寄卖点卡
    bool Consign(long lUserID, char cCardType, const char* szCardID, const char* szCardPwd, long lPrice);

    // 购买点卡
    bool BuyCard(long lUserID, long lPriceSecond, long lConsignationID);

    // 取消寄卖
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
    // 交易的列表
    CCardTradeList                  m_list;
    long                            m_lRequisitionID;
    std::map<long, SRequisition>    m_mapRequisition;

};

#endif//_GW_CARD_TRADE_H_

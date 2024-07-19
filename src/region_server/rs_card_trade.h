
#ifndef _RS_CARD_TRADE_H_
#define	_RS_CARD_TRADE_H_

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

class RsCardTrade
{
public:
	RsCardTrade();
	~RsCardTrade();

public:
    // 寄卖点卡
    bool Consign(CRegionCreature* pUser, const char* szCardID, long lVerify, long lPrice);

    // 购买点卡
    bool BuyCard(CRegionCreature* pUser, long lConsignationID);

    // 取消寄卖
    bool CancelConsign(CRegionCreature* pUser, long lConsignationID);

    // 向Gameworld请求
    void RequestUpdate();

    // 客户端请求
    void ClientRequestUpdate(CRegionCreature* pUser, short sStart, short sCount);

    void OnConsignResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szCardID, const char* szMsg);
    void OnBuyCardResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szMsg);
    void OnUpdateTradeList(CG_CmdPacket* pCmd);

    void StartTrade();
    void StopTrade();

private:
    // 交易的列表
    CCardTradeList                  m_list;
    bool                            m_bCanTrade;

};

#endif//_RS_CARD_TRADE_H_

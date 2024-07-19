
#include "region.h"
#include "rs_card_trade.h"

RsCardTrade::RsCardTrade()
{
    m_bCanTrade = true;
}

RsCardTrade::~RsCardTrade()
{
}

void RsCardTrade::StartTrade()
{
    m_bCanTrade = true;
}

void RsCardTrade::StopTrade()
{
    m_bCanTrade = false;
}

bool RsCardTrade::Consign(CRegionCreature* pUser, const char* szCardID, long lVerify, long lPrice)
{
    if (pUser->m_userInfo==NULL)
    {
        return false;
    }

    if (!m_bCanTrade)
    {
		rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_TRADE_END2));
	    pUser->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // 检查个人的数量
    if (m_list.GetCount(pUser->m_core.Name.c_str())>CCardTradeList::e_lMaxRecordsPerUser)
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_USER_RECORD_TOO_MUCH), CCardTradeList::e_lMaxRecordsPerUser);
	    pUser->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // 检查总数
    if (m_list.GetCount()>=CCardTradeList::e_lMaxRecords)
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_RECORD_TOO_MUCH), CCardTradeList::e_lMaxRecords);
	    pUser->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // 检查是否重复
    if (m_list.Find(szCardID))
    {
	    pUser->SendSystemMessage(R(MSG_CARD_ALREADY_TRADE));
        return false;
    }

	// 检查验证码
	if (lVerify != 553011)
	{
		pUser->SendSystemMessage(R(MSG_CARD_FAILVERI));
		return false;
	}

    // 检查价格
	// 价格乘1万 [3/19/2009 tooth.shi]
	//lPrice *= 10000;
    if (lPrice<CCardTradeList::e_lMinPrice || lPrice>CCardTradeList::e_lMaxPrice)
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_PRICE_ERR), CCardTradeList::e_lMinPrice, CCardTradeList::e_lMaxPrice);
	    pUser->SendSystemMessage(g_strStaticBuffer);
        return false;
    }
// 寄卖点卡不扣钱 [3/19/2009 tooth.shi]
     // 检查自己的钱是否足够
     int iMyMoney = pUser->m_pItemContainer->GetMoney();
     if (iMyMoney<CCardTradeList::e_lTax)
     {
         //rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_MONEY_NOT_ENOUGH), CCardTradeList::e_lTax);
		rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_TRADE_MONEY_MUST), CCardTradeList::e_lTax);
         pUser->SendSystemMessage(g_strStaticBuffer);
         return false;
     }
 
     // 这里预扣金钱, 如果失败一定要返回给玩家
     pUser->RemoveGold(CCardTradeList::e_lTax);

    // 发送命令
    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_card_consign);
    cmd.WriteLong(pUser->m_userInfo->m_userId);
    cmd.WriteByte(2); // 1 道具卡, 2 点卡
    cmd.WriteString((char*)szCardID);
    cmd.WriteString((char*)szCardID);
    cmd.WriteLong(lPrice);
    g_region->m_gws.EndSend();
    return true;
}

void RsCardTrade::OnConsignResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szCardID, const char* szMsg)
{
    CRegionUser *pUser = g_region->FindUser(lUserID);
    if (pUser==NULL || pUser->m_dummy==NULL)
    {
        return;
    }

    if (cSucceed==0) // 如果失败，就还钱
    {
// 寄卖点卡不还钱 [3/19/2009 tooth.shi]        
		pUser->m_dummy->AddGold(CCardTradeList::e_lTax);
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_FAILED), szMsg); // "寄卖点卡失败: %s"
	    pUser->m_dummy->SendSystemMessage(g_strStaticBuffer);
    }else
    {
	    pUser->m_dummy->SendSystemMessage(R(MSG_CARD_CONSIGN_SUCCEEDED));
    }
}

bool RsCardTrade::BuyCard(CRegionCreature* pUser, long lConsignationID)
{
    if (pUser->m_userInfo==NULL)
    {
        return false;
    }

    if (!m_bCanTrade)
    {
		rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_TRADE_END2));
	    pUser->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // 检查是否存在
    CCardTradeList::STradeRecord* pTradeInfo = m_list.Find(lConsignationID);
    if (pTradeInfo==NULL)
    {
	    pUser->SendSystemMessage(R(MSG_CARD_INEXISTENCE));
        return false;
    }

    // 检查是否是自己的寄卖品
    if (pTradeInfo->dwSalesmanUserID==pUser->m_userInfo->m_userId)
    {
	    pUser->SendSystemMessage(R(MSG_CARD_SELF_CONSIGN));
        return false;
    }

    // 检查自己的钱是否足够
    int iMyMoney = pUser->m_pItemContainer->GetMoney();
    if (iMyMoney<pTradeInfo->lPrice)
    {
	    pUser->SendSystemMessage(R(MSG_CARD_MONEY_NOT_ENOUGH));
        return false;
    }

    // 这里预扣金钱, 如果失败一定要返回给玩家
    pUser->RemoveGold(pTradeInfo->lPrice);

    // 发送命令
    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_card_buy);
    cmd.WriteLong(pUser->m_userInfo->m_userId);
    cmd.WriteLong(lConsignationID);
    cmd.WriteLong(pTradeInfo->lPrice);
    g_region->m_gws.EndSend();
    return true;
}

void RsCardTrade::OnBuyCardResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szMsg)
{
    CRegionUser *pUser = g_region->FindUser(lUserID);
    if (pUser==NULL || pUser->m_dummy==NULL)
    {
        return;
    }

    if (cSucceed==0) // 如果失败，就还钱
    {
        pUser->m_dummy->AddGold(lPrice);

        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_BUY_FAILED), szMsg); // "购买点卡失败: %s"
	    pUser->m_dummy->SendSystemMessage(g_strStaticBuffer);
    }

    if (pUser->Session())
    {
	    g_sendCmd->BeginWrite();
	    g_sendCmd->WriteShort(r2c_buy_card_ret);
        g_sendCmd->WriteLong(lConsignationID);
        g_sendCmd->WriteByte(cSucceed);
		pUser->RecvCmd(g_sendCmd);
    }
}

bool RsCardTrade::CancelConsign(CRegionCreature* pUser, long lConsignationID)
{
    return false;
}

void RsCardTrade::RequestUpdate()
{
    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_card_request_update);
    g_region->m_gws.EndSend();
}

void RsCardTrade::ClientRequestUpdate(CRegionCreature* pUser, short sStart, short sCount)
{
    if (pUser->m_userInfo && pUser->m_userInfo->Session())
    {
	    g_sendCmd->BeginWrite();
	    g_sendCmd->WriteShort(r2c_update_card_list);
        m_list.WriteToBlock(sStart, sCount, g_sendCmd);
		pUser->m_userInfo->RecvCmd(g_sendCmd);
    }
}

void RsCardTrade::OnUpdateTradeList(CG_CmdPacket* pCmd)
{
    m_list.ReadFrom(pCmd);
}

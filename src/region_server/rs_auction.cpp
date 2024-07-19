
#include "region.h"
#include "item_base.h"
#include "auction_common.h"
#include "rs_auction.h"
#include "cmd_auction.h"

//////////////////////////////////////////////////////////////////////////////

bool RsAuction::Auction(CRegionCreature* CmdGiver, SItemID id, long dwDuration, long lBasePrice, long lUniquePrice)
{
    if (dwDuration<2 || dwDuration>24)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_DURATION_ERR));
        return false;
    }

    if (lBasePrice<10 || lBasePrice>MAX_MONEY_COUNT)
    {
	    CmdGiver->SendSystemMessage(R(MSG_AUCTION_PRICE_ERR));
        return false;
    }

    if (lUniquePrice<0 || (lUniquePrice>0 && lUniquePrice<lBasePrice))
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_UNIQUE_PRICE_ERR));
        return false;
    }

    // �����˵�����
    if (GetUserAuctionCnt(CmdGiver->m_userInfo->m_userId)>SAuctionItem::e_lUserMaxRecords)
    {
		rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_LIMIT_COUNT), SAuctionItem::e_lUserMaxRecords);
	    CmdGiver->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // �������
    if ((int)m_listAuction.size()>=SAuctionItem::e_lMaxRecords)
    {
		rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_MAX), SAuctionItem::e_lMaxRecords);
	    CmdGiver->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

    // ����Ƿ��е���
    if (!CmdGiver->m_pItemContainer->m_Bag.Find(id))
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_ITEM_NOT_EXIST));
        return false;
    }

    int iOutPage, iOutI, iOutJ;
    if (!CmdGiver->m_pItemContainer->m_Bag.Find(id.id, &iOutPage, &iOutI, &iOutJ))
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_ITEM_NOT_IN_BAG));
        return false;
    }

    id = CmdGiver->m_pItemContainer->m_Bag.GetItem(iOutPage, iOutI, iOutJ);

    // ����Ƿ��ظ�
    if (FindAuctionByItem(id, 0))
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_ITEM_IN_LIST));
        return false;
    }

    // ����Լ���Ǯ�Ƿ��㹻
    int iMyMoney = CmdGiver->m_pItemContainer->GetMoney();
    int iStorageCharge = dwDuration * 1000;
    if (iMyMoney<iStorageCharge)
    {
		rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_MONEY_MUST), iStorageCharge);
        CmdGiver->SendSystemMessage(g_strStaticBuffer);
        return false;
    }

	if(!ItemCanTrade(id))
	{
		CmdGiver->SendSystemMessage(R(MSG_AUCTI0N_CANNOT_AUCTION1));
		return false;
	}

//     if (id.cBind==IB_Binded)
//     {
// 		CmdGiver->SendSystemMessage(R(MSG_AUCTI0N_CANNOT_AUCTION2));
// 		return false;
//     }

    // ����Ԥ�۱��ܷѺ͵���, ���ʧ��һ��Ҫ���ظ����
	CmdGiver->RemoveGold(iStorageCharge);
    CmdGiver->m_pItemContainer->m_Bag.RemoveItem(iOutPage, iOutI, iOutJ);
    CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, iOutPage, iOutI, iOutJ, ITEM_REASON_NULL);
    CmdGiver->RecvCmd(g_sendCmd);

    // ��������
    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_auction_auction);
    cmd.WriteLong(CmdGiver->m_userInfo->m_userId);
    id.Serialize(&cmd);
    cmd.WriteLong(dwDuration);
    cmd.WriteLong(lBasePrice);
    cmd.WriteLong(lUniquePrice);
    g_region->m_gws.EndSend();
    return true;
}

bool RsAuction::Bid(CRegionCreature* CmdGiver, long lBidPrice, long lAuctionId)
{
    if (lBidPrice<0)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_PRICE_ERR1));
        return false;
    }

    SAuctionItem* pAuctionItem = FindAuction(lAuctionId, 0);
    if (pAuctionItem==NULL)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_ITEM_NOT_IN_LIST));
        return false;
    }

    if (lBidPrice<pAuctionItem->lBidPrice)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_PRICE_ERR2));
        return false;
    }

    if (lBidPrice<pAuctionItem->lBasePrice)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_PRICE_ERR3));
        return false;
    }

    if (pAuctionItem->lBargainorChr==CmdGiver->m_userInfo->m_userId)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_PRICE_ERR4));
        return false;
    }

    // ��߼�Ϊһ�ڼ�
    if (lBidPrice>=pAuctionItem->lUniquePrice)
    {
        lBidPrice = pAuctionItem->lUniquePrice;
    }

    // ����Լ���Ǯ�Ƿ��㹻
//     int iMyMoney = CmdGiver->m_pItemContainer->GetMoney();
//     int iStorageCharge = lBidPrice * 0.05f;
//     if (iMyMoney<(lBidPrice+iStorageCharge))
//     {
// 		rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_BID_MONEY_MUST), lBidPrice+iStorageCharge, lBidPrice,iStorageCharge);
//         CmdGiver->SendSystemMessage(g_strStaticBuffer);
//         return false;
//     }

    // ����Ԥ�������Ѻ�������, ���ʧ��һ��Ҫ�������ѷ��ظ����
// 	CmdGiver->RemoveGold(iStorageCharge+lBidPrice);
// 
//     CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
//     cmd.WriteShort(r2g_auction_bid);
//     cmd.WriteLong(CmdGiver->m_userInfo->m_userId);
//     cmd.WriteLong(lBidPrice);
//     cmd.WriteLong(lAuctionId);
//     g_region->m_gws.EndSend();
	CmdGiver->m_userInfo->Prededuct(lBidPrice,1,lAuctionId);
    return true;
}

bool RsAuction::CancelAuction(CRegionCreature* CmdGiver, long lAuctionId)
{
    if (FindAuction(lAuctionId, CmdGiver->m_userInfo->m_userId)==0)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_ITEM_NOT_EXIST));
        return false;
    }

    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_auction_cancel_auction);
    cmd.WriteLong(CmdGiver->m_userInfo->m_userId);
    cmd.WriteLong(lAuctionId);
    g_region->m_gws.EndSend();
    return true;
}

bool RsAuction::CancelBid(CRegionCreature* CmdGiver, long lAuctionId)
{
    SAuctionItem* pAuctionItem = FindAuction(lAuctionId, 0);
    if (pAuctionItem==0)
    {
	    CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_ITEM_NOT_EXIST));
        return false;
    }

    if (pAuctionItem->lPurchaserChr!=CmdGiver->m_userInfo->m_userId)
    {
		CmdGiver->SendSystemMessage(R(MSG_AUCTION_BID_NOT_YOU));
        return false;
    }

    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_auction_cancel_bid);
    cmd.WriteLong(CmdGiver->m_userInfo->m_userId);
    cmd.WriteLong(lAuctionId);
    g_region->m_gws.EndSend();
    return true;
}

bool RsAuction::GetItems(CRegionCreature* CmdGiver)
{
    CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_auction_get_items);
    cmd.WriteLong(CmdGiver->m_userInfo->m_userId);
    cmd.WriteLong(CmdGiver->m_pItemContainer->m_Bag.FreeSpace());
    g_region->m_gws.EndSend();
    return true;
}

SAuctionItem* RsAuction::FindAuction(long lAuctionId, long lUserID)
{
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (lAuctionId==pAuctionItem->iIdx && (lUserID==0 || lUserID==pAuctionItem->lBargainorChr))
        {
            return pAuctionItem;
        }
    }
    return NULL;
}

SAuctionItem* RsAuction::FindAuctionByItem(SItemID& id, long lUserID)
{
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (id.id==pAuctionItem->itemID.id && (lUserID==0 || lUserID==pAuctionItem->lBargainorChr))
        {
            return pAuctionItem;
        }
    }
    return NULL;
}

int RsAuction::GetUserAuctionCnt(long lUserID)
{
    int iCnt = 0;
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (lUserID==pAuctionItem->lBargainorChr)
        {
            iCnt ++;
        }
    }
    return iCnt;
}

void RsAuction::OnUpdateAuction(CG_CmdPacket *cmd)
{
#ifdef WIN32
    SAuctionItem* pItem;
    MSG("------ ������Ϣ Gameworld -> Region ------------------------------------\n");
#endif
    short i,sCnt;
    m_listAuction.clear();
    cmd->ReadShort(&sCnt);
    for (i=0; i<sCnt; i++)
    {
        m_listAuction.push_back(SAuctionItem());
        m_listAuction.back().Serialize(cmd);
#ifdef WIN32
        pItem = &(m_listAuction.back());
        SItemBase* pItemBase = g_region->m_pItemManager->GetItem(pItem->itemID.type);
        char* szItemName = pItemBase?pItemBase->name:"δ֪����";
        MSG3("    ����: %s, ������: %s, ������: %s, �۸�: %d/%d ����: %d\n",
            szItemName, pItem->strBargainorChr.c_str(), pItem->strPurchaserChr.c_str(), pItem->lBasePrice, pItem->lUniquePrice, pItem->lBidPrice);
#endif
    }
}

void RsAuction::OnUpdateToClient(CG_CmdPacket *cmd)
{
    char cCnt;
    long lUser1, lUser2, lUser3;
    CRegionUser *pUser;

    cmd->ReadByte(&cCnt);
    if (cCnt>0)
    {
        cmd->ReadLong(&lUser1);
        pUser = g_region->FindUser(lUser1);
        if (pUser)
        {
	        if(pUser->m_dummy)
				CAuctionCmdBuilder_R2C::Build_r2c_auction_update(pUser->m_dummy);
            //pUser->RecvCmd(g_sendCmd);
        }
    }
    if (cCnt>1)
    {
        cmd->ReadLong(&lUser2);
        pUser = g_region->FindUser(lUser2);
        if (pUser)
        {
	        if(pUser->m_dummy)
				CAuctionCmdBuilder_R2C::Build_r2c_auction_update(pUser->m_dummy);
            //pUser->RecvCmd(g_sendCmd);
        }
    }
    if (cCnt>2)
    {
        cmd->ReadLong(&lUser3);
        pUser = g_region->FindUser(lUser3);
        if (pUser)
        {
	        if(pUser->m_dummy)
				CAuctionCmdBuilder_R2C::Build_r2c_auction_update(pUser->m_dummy);
            //pUser->RecvCmd(g_sendCmd);
        }
    }
}

void RsAuction::OnAuctionFailed(SItemID id, long lUserID, short sType)
{
#ifdef WIN32
    MSG1("%d ����ʧ��\n", lUserID);
#endif
    CRegionUser *pUser = g_region->FindUser(lUserID);
    if (pUser)
    {
        // ���ھ���ӵ��ߺͽ�Ǯ
        pUser->m_dummy->AddGold(CCardTradeList::e_lTax);
        pUser->m_dummy->AddItem(id);
        switch (sType)
        {
        case 2:
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_LIMIT_COUNT2));
            break;
        case 4:
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_SERVER_BUSY));
            break;
        default:
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_FAIL));
            break;
        }
    }else
    {
        // �����ھ�ֻ��Log��
    }
}

void RsAuction::OnBidFailed(long lUserID, long lBidPrice, short sType)
{
#ifdef WIN32
    MSG1("%d ����ʧ��\n", lUserID);
#endif
    CRegionUser *pUser = g_region->FindUser(lUserID);
    if (pUser)
    {
        // ���ھ���ӽ�Ǯ
        //pUser->m_dummy->AddGold(lBidPrice*1.05f);
		//pUser->m_dummy->AddPoint(lBidPrice);��worldserver������
        switch (sType)
        {
        case 3:
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_SERVER_BUSY));
            break;
        default:
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_FAIL));
            break;
        }
    }else
    {
        // �����ھ�ֻ��Log��
    }
}

void RsAuction::OnGetItems(long lUserID, char cMoney, long lMoney, char cItem, SItemID itemID)
{
    CRegionUser *pUser = g_region->FindUser(lUserID);
    if (pUser)
    {
        g_strStaticBuffer[0] = 0;
        if (cMoney==1)
        {
			pUser->m_dummy->AddPoint(lMoney);
			rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_GET_POINT));
			//rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_GET_MONEY), lMoney);
        }
        if (cItem==1)
        {
            pUser->m_dummy->AddItem(itemID);
			rt2_sprintf(g_strStaticBuffer+strlen(g_strStaticBuffer), R(MSG_AUCTION_GET_ITEM), "");
        }
        if (g_strStaticBuffer[0]==0 && cItem!=2 && cMoney != 1)
        {
			rt2_strcpy(g_strStaticBuffer, R(MSG_AUCTION_NOT_YOUR_ITEM));
        }
        pUser->m_dummy->SendSystemMessage(g_strStaticBuffer);
#ifdef WIN32
        MSG1("%s\n", g_strStaticBuffer);
#endif

		if(pUser->m_dummy)
			CAuctionCmdBuilder_R2C::Build_r2c_auction_update(pUser->m_dummy);
		 
        // ���ͻ�Gameworld˵ȡ���ĵ���
        if (cMoney==1 || cItem==1)
        {
            CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
            cmd.WriteShort(r2g_auction_get_items_ret);
            cmd.WriteLong(lUserID);
            cmd.WriteByte(cMoney);
            if (cMoney) cmd.WriteLong(lMoney);
            cmd.WriteByte(cItem);
            if (cItem) itemID.Serialize(&cmd);
            g_region->m_gws.EndSend();
        }
    }
}

void RsAuction::OnEvent(EAuctionOP eOP, SAuctionItem* pAuctionItem)
{
    CRegionUser *pUser=NULL;
    switch (eOP)
    {
    case EAOP_AUCTION: // ����
        m_listAuction.push_back(*pAuctionItem);
        pUser = g_region->FindUser(pAuctionItem->lBargainorChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ��Ʒ�ѳɹ����������н�������\n", pAuctionItem->strBargainorChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_SUCCEED));
        }
        break;
    case EAOP_AUCTION_FAILED: // ����֮ǰ�ļ��ʧ��
        break;
    case EAOP_AUCTION_SUCCEEDED: // �����ɹ�(����)
        RemoveAuction(pAuctionItem->iIdx);
        pUser = g_region->FindUser(pAuctionItem->lBargainorChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ������Ʒ�ɹ����ѱ����˹���\n", pAuctionItem->strBargainorChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_ITEM_BE_SALE));
        }
        break;
    case EAOP_AUCTION_CANCELED: // ������ȡ��(Ŀǰû����)
        RemoveAuction(pAuctionItem->iIdx);
        pUser = g_region->FindUser(pAuctionItem->lBargainorChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ������Ʒȡ��\n", pAuctionItem->strBargainorChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_ITEM_CANCEL));
        }
        break;
    case EAOP_AUCTION_FINISHED: // ��������(ʱ�䵽������û������)
        RemoveAuction(pAuctionItem->iIdx);
        pUser = g_region->FindUser(pAuctionItem->lBargainorChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ������Ʒ���������˹���\n", pAuctionItem->strBargainorChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_TIME_OVER));
        }
        break;
    case EAOP_BID_FAILED: // ����֮ǰ�ļ��ʧ��
        break;
    case EAOP_BID_SUCCEEDED: // ���ĳɹ�(��õ���)
        pUser = g_region->FindUser(pAuctionItem->lPurchaserChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ���۳ɹ����ĵ���Ʒ\n", pAuctionItem->strPurchaserChr.c_str());
#endif
            if (pAuctionItem->lBidPrice>=pAuctionItem->lUniquePrice)
            {
				pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_UNIQUE_SUCCEED));
            }else
            {
				pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_SUCCEED_GET_ITEM));
            }
        }
        break;
    case EAOP_BID_OVERTAKE: // ���ı�����
        pUser = g_region->FindUser(pAuctionItem->lPurchaserChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ���۱�����\n", pAuctionItem->strPurchaserChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_FAIL2));
        }
        break;
    case EAOP_BID_CANCELED: // ����ȡ��(����������ȡ��)
        {
            SAuctionItem* pItem = FindAuction(pAuctionItem->iIdx, pAuctionItem->lBargainorChr);
            if (pItem)
            {
                pItem->lPurchaserAcc = 0;
                pItem->lPurchaserChr = 0;
                pItem->lBidPrice = pItem->lBasePrice;
            }
        }
        pUser = g_region->FindUser(pAuctionItem->lPurchaserChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s ���۱�ȡ��\n", pAuctionItem->strPurchaserChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_CANCEL));
        }
        break;
    case EAOP_BID_SUBMISSION: // �����ύ(���ļ�Ǯ������ǰ�ļ�Ǯ�����µ������б���)
        UpdateAuction(pAuctionItem);
        pUser = g_region->FindUser(pAuctionItem->lPurchaserChr);
        if (pUser)
        {
#ifdef WIN32
            MSG1("%s �ɹ��ύ����\n", pAuctionItem->strPurchaserChr.c_str());
#endif
			pUser->m_dummy->SendSystemMessage(R(MSG_AUCTION_BID_SUCCEED));
        }
        break;
    }
}

void RsAuction::RemoveAuction(long lAuctionId)
{
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (pAuctionItem->iIdx==lAuctionId)
        {
            m_listAuction.erase(it);
            break;
        }
    }
}

void RsAuction::UpdateAuction(SAuctionItem* pItem)
{
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (pAuctionItem->iIdx==pItem->iIdx && pAuctionItem->lBargainorChr==pItem->lBargainorChr)
        {
            *pAuctionItem = *pItem;
            break;
        }
    }
}


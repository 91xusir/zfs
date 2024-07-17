
#include "gw_server.h"
#include "item_base.h"
#include "auction.h"

GwAuction::GwAuction()
{
    m_bNeedUpdateToRegion = false;
}

GwAuction::~GwAuction()
{
}
 
bool GwAuction::Startup()
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    int i = 0;

    /*
    // 把道具存盘都修改为最新版本
    rt2_sprintf(cmd.command, "SELECT ItemID,ItemVer FROM T_auction_dealing WHERE ItemVer!=%d AND ItemVer!=0 LIMIT 5000", GetItemSerializeVersion());
    if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult))
    {
        return false;
    }
    int count = mysql_num_rows(pDBResult);
    for (i=0; i<count; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        std::string strOrigItem = row[0];
        int nOrigItemVer = atol(row[1]);
        SItemID item;
        item.LoadFromString(nOrigItemVer, strOrigItem.c_str(), &g_ItemMgr);
        item.cLock = 0;
        std::string strNewItem = item.SaveToString(&g_ItemMgr);
        
        MYSQL_RES *resTmp;
        rt2_ sprintf(cmd.command, "UPDATE T_auction_dealing SET ItemID='%s',ItemVer=%d WHERE ItemID='%s' AND ItemVer=%d", strNewItem.c_str(), GetItemSerializeVersion(), strOrigItem.c_str(), nOrigItemVer);
        if (!g_server.m_database.ExecCommand(cmd.command, &resTmp))
        {
            // 可能有重复的道具
            rt2_ sprintf(cmd.command, "DELETE FROM T_auction_dealing WHERE ItemID='%s' AND ItemVer=%d", strOrigItem.c_str(), nOrigItemVer);
            if (!g_server.m_database.ExecCommand(cmd.command, &resTmp))
            {
                return false;
            }
        }
        mysql_free_result(resTmp);
    }
    mysql_free_result(pDBResult);

    rt2_ sprintf(cmd.command, "SELECT `Index`,ItemID,ItemVer FROM T_auction_result WHERE `Type`=1 AND ItemVer!=%d AND ItemVer!=0 LIMIT 5000", GetItemSerializeVersion());
    if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult))
    {
        return false;
    }
    count = mysql_num_rows(pDBResult);
    for (i=0; i<count; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        int Idx = atoi(row[0]);
        std::string strOrigItem = row[1];
        int nOrigItemVer = atol(row[2]);
        SItemID item;
        item.LoadFromString(nOrigItemVer, strOrigItem.c_str(), &g_ItemMgr);
        item.cLock = 0;
        std::string strNewItem = item.SaveToString(&g_ItemMgr);

        MYSQL_RES *resTmp;
        rt2_ sprintf(cmd.command, "UPDATE T_auction_result SET ItemID='%s',ItemVer=%d WHERE `Index`=%d", strNewItem.c_str(), GetItemSerializeVersion(), Idx);
        if (!g_server.m_database.ExecCommand(cmd.command, &resTmp))
        {
            // 可能有重复的道具
            rt2_ sprintf(cmd.command, "DELETE FROM T_auction_result WHERE `Index`=%d", Idx);
            if (!g_server.m_database.ExecCommand(cmd.command, &resTmp))
            {
                return false;
            }
        }
        mysql_free_result(resTmp);
    }
    mysql_free_result(pDBResult);
    */


    if (!ConvertOldVersion())
    {
        return false;
    }

    //
    rt2_sprintf(cmd.command, "SELECT `ItemIDStr`,`ItemVer`,`BargainorAcc`,`BargainorChrID`,`BargainorChr`,"
        "`PurchaserAcc`, `PurchaserChrID`,`PurchaserChr`,`PassTime`,`Duration`,`HandlingCharge`,"
        "`BasePrice`,`BidPrice`,`UniquePrice`,`Index`,`ItemUID` FROM `T_auction` LIMIT %d",
        SAuctionItem::e_lMaxRecords);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        const char* pErrMsg = "T_auction 数据库出错";
        ERR1("%s\n", pErrMsg);
        CHECKEX(pErrMsg);
        return false;
    }else
    {
        MYSQL_ROW row;
        SAuctionItem item;
		int nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);

            item.itemID.LoadFromString(atol(row[1]), row[0], &g_ItemMgr);
            item.itemID.cLock = 0;
            item.lBargainorAcc      =       atol(row[2]);
            item.lBargainorChr      =       atol(row[3]);
            item.strBargainorChr    =            row[4];
            item.lPurchaserAcc      =       atol(row[5]);
            item.lPurchaserChr      =       atol(row[6]);
            item.strPurchaserChr    =            row[7];
            item.lPassTime          =       atol(row[8]);
            item.lDuration          =       atol(row[9]);
            item.lHandlingCharge    =       atol(row[10]);
            item.lBasePrice         =       atol(row[11]);
            item.lBidPrice          =       atol(row[12]);
            item.lUniquePrice       =       atol(row[13]);
            item.iIdx               =       atoi(row[14]);

            if (item.itemID.id==(DWORD)atol(row[15]))
            {
            }

            m_listAuction.push_back(item);
		}
		mysql_free_result(pDBResult);
    }
    m_lLastRunTime = (long)time(NULL);
    m_lLastUpdateTime = m_lLastRunTime;
    m_bNeedUpdateToRegion = true;
    return true;
}

bool GwAuction::ConvertOldVersion()
{
    MYSQL_RES *pDBSelResult;
    MYSQL_RES *pDBInsResult;

    rt2_sprintf(g_strStaticBuffer, "SELECT `ItemID`,`ItemVer`,`BargainorAcc`,`BargainorChrID`,`BargainorChr`,"
        "`PurchaserAcc`, `PurchaserChrID`,`PurchaserChr`,`PassTime`,`Duration`,`HandlingCharge`,"
        "`BasePrice`,`BidPrice`,`UniquePrice` FROM `T_auction_dealing` LIMIT %d", SAuctionItem::e_lMaxRecords);
    if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBSelResult) )
    {
        const char* pErrMsg = "T_auction_dealing 数据库出错";
        ERR1("%s\n", pErrMsg);
        CHECKEX(pErrMsg);
        return false;
    }else
    {
        MYSQL_ROW row;
        SItemID item;
        int i;
		int nRowCount = mysql_num_rows(pDBSelResult);
        if (nRowCount!=0)
        {
		    for (i=0; i<nRowCount; i++)
		    {
			    row = mysql_fetch_row(pDBSelResult);

                item.LoadFromString(atol(row[1]), row[0], &g_ItemMgr);

                rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_auction`("
                    "`ItemUID`,"
                    "`ItemIDStr`,"
                    "`ItemVer`,"
                    "`BargainorAcc`,"
                    "`BargainorChrID`,"
                    "`BargainorChr`,"
                    "`PurchaserAcc`,"
                    "`PurchaserChrID`,"
                    "`PurchaserChr`,"
                    "`PassTime`,"
                    "`Duration`,"
                    "`HandlingCharge`,"
                    "`BasePrice`,"
                    "`BidPrice`,"
                    "`UniquePrice`"
                    ") VALUES(%d,'%s',%s,%s,%s,'%s',%s,%s,'%s',%s,%s,%s,%s,%s,%s)",
                    item.id, row[0], row[1], row[2], row[3], row[4], row[5], row[6],
                    row[7], row[8], row[9], row[10], row[11], row[12], row[13]);
                if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBInsResult) )
                {
                    break;
                }else
                {
		            mysql_free_result(pDBInsResult);
                }
		    }
		    mysql_free_result(pDBSelResult);

            if (i==nRowCount)
            {
	            rt2_sprintf(g_strStaticBuffer, "DELETE FROM `T_auction_dealing` WHERE 1");
                if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBSelResult) )
                {
                    i = 0;
                }else
                {
		            mysql_free_result(pDBSelResult);
                }
            }

            if (i!=nRowCount)
            {
                // 删除
                rt2_sprintf(g_strStaticBuffer, "SELECT `Index` FROM `T_auction` LIMIT %d, 1", nRowCount-1);
                if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBSelResult) )
                {
                    return false;
                }
			    row = mysql_fetch_row(pDBSelResult);
                i = atoi(row[0]);
		        mysql_free_result(pDBSelResult);

	            rt2_sprintf(g_strStaticBuffer, "DELETE FROM `T_auction` WHERE `Index`>%d", i);
                if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBSelResult) )
                {
                    return false;
                }
		        mysql_free_result(pDBSelResult);
                return false;
            }
        }else
        {
		    mysql_free_result(pDBSelResult);
        }
    }
    return true;
}

void GwAuction::Run()
{
    long tNow = (long)time(NULL);
    long tDif = tNow-m_lLastRunTime;
    m_lLastRunTime = tNow;
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator itNext, it = m_listAuction.begin();
    for (;it!=m_listAuction.end(); it=itNext)
    {
        itNext = it;
        itNext ++;
        pAuctionItem = &(*it);
        pAuctionItem->lPassTime += tDif;
        if (   (pAuctionItem->lPassTime > pAuctionItem->lDuration*60*60)
            || ((pAuctionItem->lBidPrice>=pAuctionItem->lUniquePrice)&&(pAuctionItem->lPurchaserChr!=0))  )
        {
            // 必须从数据库中删除才能从列表中删除，否则会造成拍卖道具复制
            if (RemoveDBRecord(pAuctionItem))
            {
                if (pAuctionItem->lPurchaserChr)
                {
                    OnBidSucceeded(pAuctionItem);
                    OnAuctionSucceeded(pAuctionItem);
                }else
                {
                    OnAuctionFinished(pAuctionItem);
                }
                RemoveAuction(pAuctionItem);
            }
        }
    }
    if ((tNow-m_lLastUpdateTime)>10*60) // 10分钟更新一次数据库
    {
        rt2_sprintf(g_strStaticBuffer, "UPDATE `T_auction` SET `PassTime`=`PassTime`+%d LIMIT %d", tNow-m_lLastUpdateTime, SAuctionItem::e_lMaxRecords+10);
        m_lLastUpdateTime = tNow;
	    MYSQL_RES *pDBResult;
        if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
        {
            // 失败
        }else
        {
		    mysql_free_result(pDBResult);
        }
    }
    if (m_bNeedUpdateToRegion)
    {
        this->UpdateAuctionListToRegion();
    }
}

void GwAuction::Shutdown()
{
    SaveToDatabase();
}

void GwAuction::SaveToDatabase()
{
    long tNow = (long)time(NULL);
    long tDif = tNow-m_lLastRunTime;
    m_lLastRunTime = tNow;
    rt2_sprintf(g_strStaticBuffer, "UPDATE `T_auction` SET `PassTime`=`PassTime`+%d LIMIT %d", tDif, SAuctionItem::e_lMaxRecords+10);
	MYSQL_RES *pDBResult;
    if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        // 失败
    }else
    {
		mysql_free_result(pDBResult);
        return;
    }
}

void GwAuction::RegionModify()
{
    m_bNeedUpdateToRegion = true;
}

bool GwAuction::Auction(long lUserID, SItemID id, long dwDuration, long lBasePrice, long lUniquePrice)
{
    id.cLock = 0;
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser==NULL)
    {
        OnAuctionFailed(id, lUserID, 0);
        return true;
    }

    // 检查总数量
    if ((int)m_listAuction.size()>=SAuctionItem::e_lMaxRecords)
    {
        OnAuctionFailed(id, lUserID, 1);
        return true;
    }

    // 检查单个帐号数量
    if ((int)GetAccountAuctionCnt(lUserID)>=SAuctionItem::e_lUserMaxRecords)
    {
        OnAuctionFailed(id, lUserID, 2);
        return true;
    }

    // 先检查是否重复
    if (FindAuction(id.id, lUserID))
    {
        OnAuctionFailed(id, lUserID, 3);
        return true;
    }

    SAuctionItem item;
    item.iIdx               = 0;
    item.itemID             = id;
    item.lBargainorAcc      = pUser->accountId;
    item.lBargainorChr      = lUserID;
    item.strBargainorChr	= pUser->name; 
	item.lPurchaserAcc		= 0; 
    item.lPurchaserChr      = 0; 
    item.lPassTime          = 0;
    item.lDuration          = dwDuration;
    item.lHandlingCharge    = 0;
    item.lBasePrice         = lBasePrice;
    item.lBidPrice          = lBasePrice;
    item.lUniquePrice       = lUniquePrice;


    // 添加寄卖信息到数据库中，只有正确的添加到数据库中才算成功
    if (AddDBRecord(&item))
    {
        // Log
        m_listAuction.push_back(item);
        Log(EAOP_AUCTION, &(m_listAuction.back()));
        SendToAllRegion(EAOP_AUCTION, &(m_listAuction.back()));

        g_server.LogUserEvent(PLT_Auction, lUserID, "", 1, id.SaveToString(&g_ItemMgr), 0, "", 0, "");
    }else
    {
        OnAuctionFailed(id, lUserID, 4);
    }

    // 发送系统公告, 格式为： xxx 寄卖了 xxx 价格为 xxx
    //     如： 财神 寄卖了 300点点卡 价格为 500000
    //HideName(pAuctionItem->strPurchaserChr, g_strStaticBuffer+512);
    //rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_BROADCAST), g_strStaticBuffer+512, szCardInfo, pRequisition->lPrice);
    //g_server.BroadcastBulletin(g_strStaticBuffer, false);

    UpdateToClient(1, lUserID);

    return true;
}

bool GwAuction::Bid(long lUserID, long lBidPrice, long lAuctionId)
{
    long lUser2 = 0;
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser==NULL)
    {
        OnBidFailed(lUserID, lBidPrice, 0);
        return true;
    }

    SAuctionItem* pAuctionItem = FindAuction(lAuctionId, 0);
    if (pAuctionItem==NULL)
    {
        OnBidFailed(lUserID, lBidPrice, 1);
        return true;
    }

    if (lBidPrice<pAuctionItem->lBasePrice)
    {
        OnBidFailed(lUserID, lBidPrice, 4);
        return true;
    }

    if (pAuctionItem->lPurchaserChr!=0)
    {
        if (lBidPrice<=pAuctionItem->lBidPrice)
        {
            OnBidFailed(lUserID, lBidPrice, 2);
            return true;
        }
        OnBidOvertake(pAuctionItem);
        lUser2 = pAuctionItem->lPurchaserChr;
    }
    long lOldAcc, lOldChr, lOldBidPrice;
    std::string strOldChr;

    if (lBidPrice>=pAuctionItem->lUniquePrice) lBidPrice = pAuctionItem->lUniquePrice;

    lOldAcc = pAuctionItem->lPurchaserAcc;
    lOldChr = pAuctionItem->lPurchaserChr;
    lOldBidPrice = pAuctionItem->lBidPrice;
    strOldChr = pAuctionItem->strPurchaserChr;

    pAuctionItem->lPurchaserAcc = pUser->accountId;
    pAuctionItem->lPurchaserChr = lUserID;
    pAuctionItem->strPurchaserChr = pUser->name;
    pAuctionItem->lBidPrice = lBidPrice;

    if (lBidPrice>=pAuctionItem->lUniquePrice)
    {
        // 必须从数据库中删除才能从列表中删除，否则会造成拍卖道具复制
        if (RemoveDBRecord(pAuctionItem))
        {
            OnBidSucceeded(pAuctionItem);
            OnAuctionSucceeded(pAuctionItem);
            RemoveAuction(pAuctionItem);
        }
    }else
    {
        // 返回玩家说竞价成功，只有正确的更新数据库才算成功
        if (UpdateDibToDatabase(pAuctionItem))
        {
            OnBidSubmission(pAuctionItem);
        }else
        {
            pAuctionItem->lPurchaserAcc = lOldAcc;
            pAuctionItem->lPurchaserChr = lOldChr;
            pAuctionItem->lBidPrice = lOldBidPrice;
            pAuctionItem->strPurchaserChr = strOldChr;
            OnBidFailed(lUserID, lBidPrice, 3);
            return true;
        }
    }

    if (lUser2) UpdateToClient(2, pAuctionItem->lBargainorChr, pAuctionItem->lPurchaserChr);
    else        UpdateToClient(3, pAuctionItem->lBargainorChr, lUser2, pAuctionItem->lPurchaserChr);
    return true;
}

bool GwAuction::UpdateDibToDatabase(SAuctionItem* pAuctionItem)
{
	MYSQL_RES *pDBResult;
    rt2_sprintf(g_strStaticBuffer,
        "UPDATE `T_auction` "
        "SET `PurchaserAcc`=%d,`PurchaserChrID`=%d,`PurchaserChr`='%s',`BidPrice`=%d WHERE `ItemUID`='%d'",
        pAuctionItem->lPurchaserAcc, pAuctionItem->lPurchaserChr, pAuctionItem->strPurchaserChr.c_str(), pAuctionItem->lBidPrice,
        pAuctionItem->itemID.id);
    if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        return false;
    }else
    {
		mysql_free_result(pDBResult);
        return true;
    }
}

bool GwAuction::CancelAuction(long lUserID, long lAuctionId)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser==NULL)
    {
        return true;
    }
    SAuctionItem* pAuctionItem = FindAuction(lAuctionId, lUserID);
    if (pAuctionItem==NULL)
    {
        return true;
    }
    // 必须从数据库中删除才能从列表中删除，否则会造成拍卖道具复制
    if (RemoveDBRecord(pAuctionItem))
    {
        if (pAuctionItem->lPurchaserChr)
        {
            OnBidCanceled(pAuctionItem);
        }
        OnAuctionCanceled(pAuctionItem);
        RemoveAuction(pAuctionItem);

        UpdateToClient(1, lUserID);
    }
    return true;
}

bool GwAuction::CancelBid(long lUserID, long lAuctionId)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser==NULL)
    {
        return true;
    }
    SAuctionItem* pAuctionItem = FindAuction(lAuctionId, 0);
    if (pAuctionItem==NULL)
    {
        return true;
    }
    if (pAuctionItem->lPurchaserChr==0)
    {
        return true;
    }
    if (pAuctionItem->lPurchaserChr!=lUserID)
    {
        return true;
    }
    OnBidCanceled(pAuctionItem);
    pAuctionItem->lBidPrice = pAuctionItem->lBasePrice;
    pAuctionItem->lPurchaserChr = 0;
    pAuctionItem->lPurchaserAcc = 0;
    pAuctionItem->strPurchaserChr.clear();
    UpdateDibToDatabase(pAuctionItem);

    UpdateToClient(2, pAuctionItem->lBargainorChr, lUserID);
    return true;
}

bool GwAuction::GetItems(long lUserID, long lFreeSpace)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser)
    {
	    GWDBCommand cmd;
	    MYSQL_RES* res;
        MYSQL_ROW row;
        long lMoney = 0;
        int iVersion = 0;
        int iAffectedRows;
        bool bBagFull = false;
        SItemID item;
	    rt2_sprintf(cmd.command, "SELECT sum(`Money`) FROM `T_auction_result` WHERE `ChrID`=%d and `Type`=0", lUserID);
	    if (!g_server.m_database.ExecCommand(cmd.command, &res))
        {
		    ERR("[GwAuction::GetItems] Can NOT access Auction DB!\n");
            lMoney = 0;
            return false;
        }
		row = mysql_fetch_row(res);
        if (row)
        {
            lMoney = (row[0]==0)?0:atol(row[0]);
        }
	    mysql_free_result(res);

        if (lMoney)
        {
	        rt2_sprintf(cmd.command, "DELETE FROM `T_auction_result` WHERE `ChrID`=%d and `Type`=0", lUserID);
	        if (!g_server.m_database.ExecCommandRow(cmd.command, &res, &iAffectedRows))
            {
		        ERR("[GwAuction::GetItems] Can NOT access Auction DB!\n");
                lMoney = 0;
                return false;
            }
	        mysql_free_result(res);
            if (iAffectedRows==0)
            {
                lMoney = 0;
            }
        }

        do
        {
            int iResultIdx;
	        rt2_sprintf(cmd.command, "SELECT `ItemVer`,`ItemID`,`Index` FROM `T_auction_result` WHERE `ChrID`=%d and `Type`=1 LIMIT 1", lUserID);
	        if (!g_server.m_database.ExecCommand(cmd.command, &res))
            {
		        ERR("[GwAuction::GetItems] Can NOT access Auction DB!\n");
                iVersion = 0;
                return false;
            }
		    row = mysql_fetch_row(res);
            if (row)
            {
                iVersion = (row[0]==0)?0:atol(row[0]);
                item.LoadFromString(iVersion, row[1], &g_ItemMgr);
                item.cLock = 0;
                iResultIdx = atoi(row[2]);
            }
	        mysql_free_result(res);

            if (iVersion)
            {
                if (lFreeSpace==0)
                {
					g_server.SendSystemMessageToUser((DWORD)lUserID, R(MSG_AUCTION_BAG_FULL));
                    iVersion = 0;
                    bBagFull = true;
                }
            }

            if (iVersion)
            {
	            rt2_sprintf(cmd.command, "DELETE FROM `T_auction_result` WHERE `Index`=%d", iResultIdx);
	            if (!g_server.m_database.ExecCommandRow(cmd.command, &res, &iAffectedRows))
                {
		            ERR("[GwAuction::GetItems] Can NOT access Auction DB!\n");
                    iVersion = 0;
                    return false;
                }
	            mysql_free_result(res);
                if (iAffectedRows==0)
                {
                    iVersion = 0;
                }
            }
        }while (false);

	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_get_result);
            cmd.WriteLong(lUserID);
            cmd.WriteByte(lMoney?1:0);
            if (lMoney) cmd.WriteLong(lMoney);
            cmd.WriteByte(iVersion?1:(bBagFull?2:0));
            if (iVersion) item.Serialize(&cmd);
	        g_server.m_region.SendToPeer(iSe);
        }

        return true;

    }
    return true;
}

void GwAuction::UpdateAuctionListToRegion()
{
    m_bNeedUpdateToRegion = false;
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_update);
    cmd.WriteShort((short)m_listAuction.size());
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        pAuctionItem->Serialize(&cmd);
    }
	g_server.m_region.BroadcastCmd();
}

SAuctionItem* GwAuction::FindAuction(long lAuctionId, long lUserID)
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

int GwAuction::GetUserAuctionCnt(long lUserID)
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

int GwAuction::GetAccountAuctionCnt(long lAccountID)
{
    int iCnt = 0;
    SAuctionItem* pAuctionItem;
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (; it!=m_listAuction.end(); it++)
    {
        pAuctionItem = &(*it);
        if (lAccountID==pAuctionItem->lBargainorAcc)
        {
            iCnt ++;
        }
    }
    return iCnt;
}

void GwAuction::OnAuctionFailed(SItemID id, long lUserID, short sType)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
	if (pUser)
    {
	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_failed);
            cmd.WriteLong(lUserID);
            id.Serialize(&cmd);
	        cmd.WriteShort(sType);
	        g_server.m_region.SendToPeer(iSe);
        }
    }
}

void GwAuction::OnAuctionSucceeded(SAuctionItem* pAuctionItem)
{
    // 如果拍卖成功，就把拍卖得的钱给卖家
    Log(EAOP_AUCTION_SUCCEEDED, pAuctionItem);
    AddMoney(pAuctionItem->lBargainorChr, pAuctionItem->lBidPrice * 0.9);
    SendToAllRegion(EAOP_AUCTION_SUCCEEDED, pAuctionItem);

    g_server.LogUserEvent(PLT_Auction, pAuctionItem->lBargainorChr, "", 3, pAuctionItem->itemID.SaveToString(&g_ItemMgr), pAuctionItem->lBidPrice, pAuctionItem->strPurchaserChr.c_str(), 0, "");
}

void GwAuction::OnAuctionCanceled(SAuctionItem* pAuctionItem)
{
    // 如果拍卖取消，就把道具还给卖家
    Log(EAOP_AUCTION_CANCELED, pAuctionItem);
    AddItem(pAuctionItem->lBargainorChr, pAuctionItem->itemID);
    SendToAllRegion(EAOP_AUCTION_CANCELED, pAuctionItem);

    g_server.LogUserEvent(PLT_Auction, pAuctionItem->lBargainorChr, "", 2, pAuctionItem->itemID.SaveToString(&g_ItemMgr), 0, "", 0, "");
}

void GwAuction::OnAuctionFinished(SAuctionItem* pAuctionItem)
{
    // 如果拍卖结束，就把道具还给卖家
    Log(EAOP_AUCTION_FINISHED, pAuctionItem);
    AddItem(pAuctionItem->lBargainorChr, pAuctionItem->itemID);
    SendToAllRegion(EAOP_AUCTION_FINISHED, pAuctionItem);

    g_server.LogUserEvent(PLT_Auction, pAuctionItem->lBargainorChr, "", 2, pAuctionItem->itemID.SaveToString(&g_ItemMgr), 0, "", 0, "");
}

void GwAuction::RemoveAuction(SAuctionItem* pAuctionItem)
{
    std::list<SAuctionItem>::iterator it = m_listAuction.begin();
    for (;it!=m_listAuction.end(); it++)
    {
        if (pAuctionItem == &(*it))
        {
            m_listAuction.erase(it);
            break;
        }
    }
}

void GwAuction::OnBidFailed(long lUserID, long lBidPrice, short sType)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
	if (pUser)
    {
	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_bid_failed);
            cmd.WriteLong(lUserID);
            cmd.WriteLong(lBidPrice);
	        cmd.WriteShort(sType);
	        g_server.m_region.SendToPeer(iSe);

			CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_add_point);
			packet.WriteLong(pUser->accountId);
			packet.WriteLong(lUserID);
			packet.WriteShort(lBidPrice);
			g_server.m_login.SendCmd();
        }
    }
}

void GwAuction::OnBidSucceeded(SAuctionItem* pAuctionItem)
{
    // 如果竞标成功，就把道具给买家
    Log(EAOP_BID_SUCCEEDED, pAuctionItem);
    AddItem(pAuctionItem->lPurchaserChr, pAuctionItem->itemID);
    SendToAllRegion(EAOP_BID_SUCCEEDED, pAuctionItem);

    g_server.LogUserEvent(PLT_Auction, pAuctionItem->lPurchaserChr, "", 4, pAuctionItem->itemID.SaveToString(&g_ItemMgr), pAuctionItem->lBidPrice, "", 0, pAuctionItem->strBargainorChr.c_str());
}

void GwAuction::OnBidOvertake(SAuctionItem* pAuctionItem)
{
    // 如果竞标被超过，就把金钱还给买家
    Log(EAOP_BID_OVERTAKE, pAuctionItem);
    AddMoney(pAuctionItem->lPurchaserChr, pAuctionItem->lBidPrice);
    SendToAllRegion(EAOP_BID_OVERTAKE, pAuctionItem);
}

void GwAuction::OnBidCanceled(SAuctionItem* pAuctionItem)
{
    // 如果竞标取消，就把金钱还给买家
    Log(EAOP_BID_CANCELED, pAuctionItem);
    AddMoney(pAuctionItem->lPurchaserChr, pAuctionItem->lBidPrice*0.9f);
    SendToAllRegion(EAOP_BID_CANCELED, pAuctionItem);
}

void GwAuction::OnBidSubmission(SAuctionItem* pAuctionItem)
{
    // 如果竞标提交了
    Log(EAOP_BID_SUBMISSION, pAuctionItem);
    SendToAllRegion(EAOP_BID_SUBMISSION, pAuctionItem);
}

bool GwAuction::AddDBRecord(SAuctionItem* pAuctionItem)
{
    int iCount = 0;
	MYSQL_RES *pDBResult;

    rt2_sprintf(g_strStaticBuffer, "SELECT count(*) FROM `T_auction`");
    if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        return false;
    }else
    {
        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        iCount = atoi(row[0]);
		mysql_free_result(pDBResult);
    }

    rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_auction`("
        "`ItemUID`,"
        "`ItemIDStr`,"
        "`ItemVer`,"
        "`BargainorAcc`,"
        "`BargainorChrID`,"
        "`BargainorChr`,"
        "`PurchaserAcc`,"
        "`PurchaserChrID`,"
        "`PurchaserChr`,"
        "`PassTime`,"
        "`Duration`,"
        "`HandlingCharge`,"
        "`BasePrice`,"
        "`BidPrice`,"
        "`UniquePrice`"
        ") VALUES(%d, '%s', %d, %d, %d,'%s',%d, %d,'%s',%d,%d,%d,%d,%d,%d)",
        pAuctionItem->itemID.id,
        pAuctionItem->itemID.SaveToString(&g_ItemMgr),
        ::GetItemSerializeVersion(),
        pAuctionItem->lBargainorAcc,
        pAuctionItem->lBargainorChr,
        pAuctionItem->strBargainorChr.c_str(),
        pAuctionItem->lPurchaserAcc,
        pAuctionItem->lPurchaserChr,
        pAuctionItem->strPurchaserChr.c_str(),
        pAuctionItem->lPassTime,
        pAuctionItem->lDuration,
        pAuctionItem->lHandlingCharge,
        pAuctionItem->lBasePrice,
        pAuctionItem->lBidPrice,
        pAuctionItem->lUniquePrice);
    if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult) )
    {
        return false;
    }else
    {
		mysql_free_result(pDBResult);
    }

    rt2_sprintf(g_strStaticBuffer, "SELECT `Index` FROM `T_auction` LIMIT %d, 1", iCount);
    if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        return false;
    }else
    {
        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        pAuctionItem->iIdx = atoi(row[0]);
		mysql_free_result(pDBResult);
    }
    return true;
}

bool GwAuction::RemoveDBRecord(SAuctionItem* pAuctionItem)
{
    // 从数据库中删除
    int iRow;
    MYSQL_RES *pDBResult;
    rt2_sprintf(g_strStaticBuffer, "DELETE FROM `T_auction` WHERE `Index`='%d'", pAuctionItem->iIdx);
	if (!g_server.m_database.ExecCommandRow(g_strStaticBuffer, &pDBResult, &iRow))
    {
		return false;
    }else
    {
		mysql_free_result(pDBResult);
        return (iRow!=0);
    }
}

void GwAuction::AddMoney(long lUserID, long lMoney)
{
    rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_auction_result`("
        "`Index`,`Type`,`ItemID`,`ItemVer`,`Money`,`ChrID`,`Time`"
        ") VALUES('', '0', '', '0', '%d','%d','%d')", lMoney, lUserID, time(NULL));

	MYSQL_RES *pDBResult;
	if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        ERR2("GwAuction::AddMoney ERROR, User=%d, Money=%d\n", lUserID, lMoney);
		return;
    }else
    {
		mysql_free_result(pDBResult);
    }
}

void GwAuction::AddItem(long lUserID, SItemID item)
{
    item.cLock = 0;
    rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_auction_result`("
        "`Index`,`Type`,`ItemID`,`ItemVer`,`Money`,`ChrID`,`Time`"
        ") VALUES('', '1', '%s', '%d', '0','%d','%d')",
        item.SaveToString(&g_ItemMgr),
        ::GetItemSerializeVersion(),
        lUserID,
        time(NULL));

	MYSQL_RES *pDBResult;
	if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
        ERR2("GwAuction::AddItem ERROR, User=%d, Item=%s\n", lUserID, item.SaveToString(&g_ItemMgr));
		return;
    }else
    {
		mysql_free_result(pDBResult);
    }
}

void GwAuction::Log(EAuctionOP eOP, SAuctionItem* pAuctionItem)
{
    rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_auction_log`("
        "`Index`,"
        "`Type`,"
        "`ItemID`,"
        "`ItemVer`,"
        "`BargainorAcc`,"
        "`BargainorChrID`,"
        "`BargainorChr`,"
        "`PurchaserAcc`,"
        "`PurchaserChrID`,"
        "`PurchaserChr`,"
        "`PassTime`,"
        "`Duration`,"
        "`HandlingCharge`,"
        "`BasePrice`,"
        "`BidPrice`,"
        "`UniquePrice`,"
        "`Time`"
        ") VALUES('', '%d', '%s', '%d', '%d', '%d', '%s', '%d', '%d','%s','%d', %d, '%d', %d, '%d', %d, %d)",
        eOP,
        pAuctionItem->itemID.SaveToString(&g_ItemMgr),
        ::GetItemSerializeVersion(),
        pAuctionItem->lBargainorAcc,
        pAuctionItem->lBargainorChr,
        pAuctionItem->strBargainorChr.c_str(),
        pAuctionItem->lPurchaserAcc,
        pAuctionItem->lPurchaserChr,
        pAuctionItem->strPurchaserChr.c_str(),
        pAuctionItem->lPassTime,
        pAuctionItem->lDuration,
        pAuctionItem->lHandlingCharge,
        pAuctionItem->lBasePrice,
        pAuctionItem->lBidPrice,
        pAuctionItem->lUniquePrice,
        time(NULL));

    MYSQL_RES *pDBResult;
	if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
		return;
    }else
    {
		mysql_free_result(pDBResult);
    }
}

void GwAuction::SendToAllRegion(EAuctionOP eOP, SAuctionItem* pAuctionItem)
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_event);
    cmd.WriteShort(eOP);
    pAuctionItem->Serialize(&cmd);
	g_server.m_region.BroadcastCmd();

    //UpdateAuctionListToRegion();
}

void GwAuction::UpdateToClient(int iCnt, long lUserID1, long lUserID2, long lUserID3)
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_auction_event);
    cmd.WriteShort(EAOP_UPDATE_TO_CLIENT);
    cmd.WriteByte((char)iCnt);
    if (iCnt>0) cmd.WriteLong(lUserID1);
    if (iCnt>1) cmd.WriteLong(lUserID2);
    if (iCnt>2) cmd.WriteLong(lUserID3);
	g_server.m_region.BroadcastCmd();
    //UpdateAuctionListToRegion();
}

void GwAuction::OnGetItemRet(long lUserID, char cMoney, long lMoney, char cItem, SItemID& itemID)
{
    SAuctionItem item;

    item.iIdx = 0;
    item.lBargainorAcc = 0;
    if (cMoney)
    {
        if (cItem)
        {
			item.strBargainorChr = R(MSG_AUCTION_BARGAINOR1);
        }else
        {
            item.strBargainorChr = R(MSG_AUCTION_BARGAINOR2);
        }
    }else
    {
        if (cItem)
        {
            item.strBargainorChr = R(MSG_AUCTION_BARGAINOR3);
        }else
        {
            item.strBargainorChr = R(MSG_AUCTION_BARGAINOR4);
        }
    }
    item.lPurchaserAcc = 0;
	item.lPurchaserChr = 0;
    item.strPurchaserChr.clear();
	item.lPassTime = 0;
	item.lDuration = 0;
	item.lHandlingCharge = 0;
	item.lBidPrice = 0;
	item.lUniquePrice = 0;

    item.itemID = itemID;
    item.lBargainorChr = lUserID;
    item.lBasePrice = lMoney;

    Log(EAOP_GET_ITEM, &item);
}

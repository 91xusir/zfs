
#include "gw_server.h"
#include "cardtrade.h"


void HideName(const char* szName, char* szHideName)
{
    int iLen = (int)strlen(szName);
    if (iLen<=2)
    {
        // 全部显示
        rt2_strcpy(szHideName, szName);
        return;
    }else if (iLen<=4)
    {
        // 显示头两个字母
        rt2_strcpy(szHideName, szName);
        if (szHideName[0]&0x80)
        {
            // 第一个字是汉字，就保留第一个汉字
            memset(szHideName+2, '*', iLen-2);
        }else if (szHideName[1]&0x80)
        {
            // 第二个字才是汉字，保留前3个字符(1个字母, 2个汉字)
            memset(szHideName+3, '*', iLen-3);
        }else
        {
            // 前两个字母都不是汉字，就保留前两个字母
            memset(szHideName+2, '*', iLen-2);
        }
    }else
    {
        int i, j;
        char cType[20];
        j = 0;
        for (i=0; i<iLen && j<20; i++)
        {
            if (szName[i]&0x80)
            {
                cType[j] = 2;
                i ++;
                if (i>=iLen)
                {
                    break;
                }
            }else
            {
                cType[j] = 1;
            }
            j ++;
        }

        // 显示头两个字母 和 后两个字母
        int k;
        k = 0;
        j /= 2;
        for (i=0; i<j; i++)
        {
            k += cType[i];
        }
        memcpy(szHideName, szName, k);
        rt2_strcpy(szHideName+k, "***");
    }
}

GwCardTrade::GwCardTrade()
{
    m_lRequisitionID = 1;
    m_bNeedUpdateToRegion = false;
}

GwCardTrade::~GwCardTrade()
{
}

bool GwCardTrade::Startup()
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    rt2_sprintf(cmd.command, "SELECT `ID`,`CardType`,`GroupType`,`CardID`,`CardPwd`,`CardInfo`,"
        "`UserID`,`UserName`,`AccountID`,`AccountName`,`Price`,"
        "UNIX_TIMESTAMP(`StartTime`),UNIX_TIMESTAMP(`EndTime`) FROM `T_CardTrade` LIMIT %d",
        CCardTradeList::e_lMaxRecords);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        const char* pErrMsg = "T_CardTrade数据库出错";
        ERR1("%s\n", pErrMsg);
        CHECKEX(pErrMsg);
        return false;
    }else
    {
        MYSQL_ROW row;
        long lConsignationID, lUserID, lAccountID, lPrice, lStartTime, lEndTime, lCardGroupType;
        char cCardType;
        char *szCardID, *szCardPwd, *szCardInfo, *szUsername, *szAccountName;
		int nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);

            lConsignationID =       atol(row[0]);
            cCardType       = (char)atoi(row[1]);
            lCardGroupType  =       atol(row[2]);
            szCardID        =            row[3];
            szCardPwd       =            row[4];
            szCardInfo      =            row[5];
            lUserID         =       atol(row[6]);
            szUsername      =            row[7];
            lAccountID      =       atol(row[8]);
            szAccountName   =            row[9];
            lPrice          =       atol(row[10]);
            lStartTime      =       atol(row[11]);
            lEndTime        =       atol(row[12]);

            m_list.Add(lConsignationID, lAccountID, szAccountName, lUserID, szUsername,
                cCardType, szCardID, szCardPwd, lCardGroupType,
                szCardInfo, lPrice, lStartTime, lEndTime);
		}
		mysql_free_result(pDBResult);
    }
    return true;
}

void GwCardTrade::Run()
{
    long tNow = (long)time(NULL);
    std::list<long> delList;
    CCardTradeList::STradeRecord* pTradeRecord = m_list.FirstRecord();
    while (pTradeRecord)
    {
        if (tNow > pTradeRecord->lEndTime)
        {
            delList.push_back(pTradeRecord->lConsignationID);
        }
        pTradeRecord = m_list.NextRecord();
    }
    std::list<long>::iterator it;
    for (it=delList.begin(); it!=delList.end(); it++)
    {
        CancelConsign(0, *it);
    }

    if (m_list.m_bModified)
    {
        m_list.m_bModified = false;
        m_bNeedUpdateToRegion = true;
    }

    if (m_bNeedUpdateToRegion)
    {
        UpdateCardTradeList();
        m_bNeedUpdateToRegion = false;
    }
}

void GwCardTrade::RegionModify()
{
    m_bNeedUpdateToRegion = true;
}

bool GwCardTrade::Consign(long lUserID, char cCardType, const char* szCardID, const char* szCardPwd, long lPrice)
{
    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser==NULL)
    {
        OnConsignFailed(lUserID, szCardID, 0, "Unknown");
        return true;
    }

    // 先检查是否重复
    if (m_list.Find(szCardID))
    {
        OnConsignFailed(lUserID, szCardID, 0, R(MSG_CARD_ALREADY_TRADE));
        return true;
    }

    // 检查数量
    if (m_list.GetCount()>=CCardTradeList::e_lMaxRecords)
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_RECORD_TOO_MUCH), CCardTradeList::e_lMaxRecords);
        OnConsignFailed(lUserID, szCardID, 0, g_strStaticBuffer);
        return true;
    }

    // 向Login申请检查是否可以交易
    m_mapRequisition[m_lRequisitionID] = SRequisition();
    SRequisition* pRequisition = &(m_mapRequisition[m_lRequisitionID]);

    pRequisition->lRequisitionID = m_lRequisitionID;
    pRequisition->dwSalesmanAccountID = pUser->accountId;
    rt2_strncpy(pRequisition->szSalesmanAccountName, pUser->accountName.c_str(), 23); //pRequisition->szSalesmanAccountName[23] = 0;
    pRequisition->dwSalesmanUserID = lUserID;
    rt2_strncpy(pRequisition->szSalesmanUsername, pUser->name.c_str(), 23); //pRequisition->szSalesmanUsername[23] = 0;
    pRequisition->cCardType = cCardType;
    pRequisition->iRequisitionType = 1;
    rt2_strncpy(pRequisition->szCardID, szCardID, 35); //pRequisition->szCardID[35] = 0;
    rt2_strncpy(pRequisition->szCardPwdMD5, szCardPwd, 35); //pRequisition->szCardPwdMD5[35] = 0;
    pRequisition->lPrice = lPrice;
    pRequisition->lRequisitionTime = time(NULL);

    g_server.CheckCardCanTrade(pRequisition->dwSalesmanAccountID, pRequisition->szSalesmanAccountName, lUserID,
        cCardType, szCardID, szCardPwd, m_lRequisitionID, CCardTradeList::e_lDefaultTradeTime/60+1);

    m_lRequisitionID ++;

    return true;
}

void GwCardTrade::OnCheckCardCanTrade(long lRequisitionID, long lUserID, const char* szCardID, long lCardGroupType, char cCan, const char* szMsg)
{
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(lRequisitionID);
    if (it==m_mapRequisition.end())
    {
        return;
    }
    SRequisition& requisition = (*it).second;
    if (requisition.lRequisitionID!=lRequisitionID) return;
    if (strcmp(requisition.szCardID,szCardID)!=0) return;
    if (requisition.iRequisitionType!=1) return;
    if (requisition.dwSalesmanUserID!=lUserID) return;

    if (cCan)
    {
        OnConsignSucceeded(&requisition, lCardGroupType, szMsg);
    }else
    {
        OnConsignFailed(&requisition, lCardGroupType, szMsg);
    }
}

void GwCardTrade::OnConsignSucceeded(SRequisition* pRequisition, long lCardGroupType, const char* szCardInfo)
{
    long lNow, lStopTime;
    lNow = (long)time(NULL);
    lStopTime = lNow+CCardTradeList::e_lDefaultTradeTime;

    // 先检查是否重复
    if (m_list.Find(pRequisition->szCardID))
    {
        OnConsignFailed(pRequisition, lCardGroupType, R(MSG_CARD_ALREADY_TRADE));
        return;
    }

    // 检查数量
    if (m_list.GetCount()>=CCardTradeList::e_lMaxRecords)
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_RECORD_TOO_MUCH), CCardTradeList::e_lMaxRecords);
        OnConsignFailed(pRequisition, lCardGroupType, g_strStaticBuffer);
        return;
    }

    // Log
    Log(ECOP_CONSIGN, pRequisition->szCardID, pRequisition->dwSalesmanUserID, pRequisition->lPrice);

    // 5, 添加寄卖信息到数据库中
    long lConsignationID = AddDBRecord(pRequisition->cCardType, pRequisition->szCardID,
        pRequisition->szCardPwdMD5, lCardGroupType, szCardInfo,
        pRequisition->dwSalesmanAccountID, pRequisition->szSalesmanAccountName,
        pRequisition->dwSalesmanUserID, pRequisition->szSalesmanUsername,
        pRequisition->lPrice, lNow, lStopTime);
    if (lConsignationID==0)
    {
        OnConsignFailed(pRequisition, lCardGroupType, szCardInfo);
        return;
    }

    // 6, 添加寄卖信息到列表中
    m_list.Add(lConsignationID,
        pRequisition->dwSalesmanAccountID, pRequisition->szSalesmanAccountName, 
        pRequisition->dwSalesmanUserID, pRequisition->szSalesmanUsername,
        pRequisition->cCardType, pRequisition->szCardID, pRequisition->szCardPwdMD5,
        lCardGroupType, szCardInfo, pRequisition->lPrice, lNow, lStopTime);

    // 发送系统公告, 格式为： xxx 寄卖了 xxx 价格为 xxx
    //     如： 财神 寄卖了 300点点卡 价格为 500000
    HideName(pRequisition->szSalesmanUsername, g_strStaticBuffer+512);
    rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_BROADCAST), g_strStaticBuffer+512, szCardInfo, pRequisition->lPrice);
    g_server.BroadcastBulletin(g_strStaticBuffer, false);

    UpdateCardTradeList();

    // 7, 返回玩家说寄卖成功
    // g_server.SendSystemMessageToUser((DWORD)pRequisition->lUserID, R(MSG_CARD_CONSIGN_SUCCEEDED));
    GWUser* pUser = g_server.FindActorById(pRequisition->dwSalesmanUserID);
	if (pUser)
    {
	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_consign_ret);
            cmd.WriteLong(pRequisition->dwSalesmanUserID);
	        cmd.WriteByte(1);
	        cmd.WriteLong(lConsignationID);
            cmd.WriteLong(pRequisition->lPrice);
	        g_server.m_region.SendToPeer(iSe);
        }
    }

    // 删除申请列表
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(pRequisition->lRequisitionID);
    if (it!=m_mapRequisition.end())
    {
        m_mapRequisition.erase(it);
    }
}

void GwCardTrade::OnConsignFailed(SRequisition* pRequisition, long lCardGroupType, const char* szCardInfo)
{
    OnConsignFailed(pRequisition->dwSalesmanUserID, pRequisition->szCardID, lCardGroupType, szCardInfo);

    // 删除申请列表
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(pRequisition->lRequisitionID);
    if (it!=m_mapRequisition.end())
    {
        m_mapRequisition.erase(it);
    }
}

void GwCardTrade::OnConsignFailed(long lUserID, const char* szCardID, long lCardGroupType, const char* szCardInfo)
{
    // 9, 返回玩家说寄卖失败
    //rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_FAILED), szCardInfo);
    //g_server.SendSystemMessageToUser((DWORD)lUserID, g_strStaticBuffer);
    GWUser* pUser = g_server.FindActorById(lUserID);
	if (pUser)
    {
	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_consign_ret);
            cmd.WriteLong(lUserID);
	        cmd.WriteByte(0);
            cmd.WriteString((char*)szCardID);
            cmd.WriteString((char*)szCardInfo);
	        g_server.m_region.SendToPeer(iSe);
            return;
        }
    }
    rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_FAILED_MSG), CCardTradeList::e_lTax);
	string StrItemID = "";
	g_server.m_mail.AddMail(lUserID, 0, R(MSG_CARD_CONSIGN_FAILED_MAIL_MSG), g_strStaticBuffer, true,StrItemID,0);
    if (!g_server.AddMoneyToGift(lUserID, CCardTradeList::e_lTax, R(MSG_CARD_CONSIGN_FAILED_GIFT_MSG)))
    {
        Log(ECOP_ADD_GIFT_FAILED, szCardID, lUserID, CCardTradeList::e_lTax);
    }
}

bool GwCardTrade::BuyCard(long lBuyerUserID, long lPriceSecond, long lConsignationID)
{
    CCardTradeList::STradeRecord* pInfo = m_list.Find(lConsignationID);
    if (pInfo==NULL)
    {
        OnBuyCardFailed(lBuyerUserID, 0, lPriceSecond, "", R(MSG_CARD_INEXISTENCE));
        return true;
    }

    // 检查买的人是否存在
    GWUser* pUser = g_server.FindActorById(lBuyerUserID);
    if (pUser==NULL)
    {
        OnBuyCardFailed(lBuyerUserID, 0, lPriceSecond, "", R(MSG_CARD_BUYER_ERR));
        return true;
    }

    // 向Login申请检查充值
    m_mapRequisition[m_lRequisitionID] = SRequisition();
    SRequisition* pRequisition = &(m_mapRequisition[m_lRequisitionID]);

    pRequisition->lRequisitionID = m_lRequisitionID;
    pRequisition->lBuyerUserID = lBuyerUserID;
    pRequisition->cCardType = pInfo->cCardType;
    pRequisition->iRequisitionType = 2; // 充值
    rt2_strncpy(pRequisition->szSalesmanUsername, pInfo->szSalesmanUsername, 23); //pRequisition->szSalesmanUsername[23] = 0;
    rt2_strncpy(pRequisition->szCardID, pInfo->szCardID, 35); //pRequisition->szCardID[35] = 0;
    rt2_strncpy(pRequisition->szCardPwdMD5 , pInfo->szCardPwdMD5, 35); //pRequisition->szCardPwdMD5[35] = 0;
    pRequisition->lPrice = pInfo->lPrice;
    pRequisition->lRequisitionTime = time(NULL);
    pRequisition->lBuyConsignationID = lConsignationID;
    pRequisition->dwSalesmanUserID = pInfo->dwSalesmanUserID;

    g_server.ChargeCard(m_lRequisitionID, lBuyerUserID,
        pInfo->dwSalesmanAccountID, pInfo->szSalesmanAccountName,
        pInfo->cCardType, pInfo->szCardID, pInfo->szCardPwdMD5);

    Log(ECOP_BUY, pInfo->szCardID, lBuyerUserID, pInfo->lPrice);

    m_lRequisitionID ++;
    return true;
}

void GwCardTrade::OnChargeResult(long lRequisitionID, long lUserID, const char* szCardID, char cSucceed, const char* szMsg)
{
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(lRequisitionID);
    if (it==m_mapRequisition.end())
    {
        // 出错了
        return;
    }
    SRequisition& requisition = (*it).second;
    if (requisition.lRequisitionID!=lRequisitionID) return;
    if (strcmp(requisition.szCardID, szCardID)!=0) return;
    if (requisition.iRequisitionType!=2) return;
    if (requisition.lBuyerUserID!=lUserID) return;

    if (cSucceed)
    {
        Log(ECOP_BUY_SUCCEEDED, requisition.szCardID, requisition.lBuyerUserID, requisition.lPrice);
        OnBuyCardSucceeded(&requisition, szMsg);

		// Log the event
		GWUser* pUserBuy = g_server.FindActorById(requisition.lBuyerUserID);
		if (pUserBuy)
		{
			if (!g_server.LogUserEvent(PLT_CardTrade, pUserBuy->userID, "", ECOP_BUY_SUCCEEDED, "", requisition.lPrice, requisition.szCardID, requisition.dwSalesmanUserID, requisition.szSalesmanUsername))
			{
				ERR("LogUserEvent Error!\n");
			}
		}
    }else
    {
        Log(ECOP_BUY_FAILED, requisition.szCardID, requisition.lBuyerUserID, requisition.lPrice);
        OnBuyCardFailed(&requisition, szMsg);
    }
}

// 注意：由于服务器的卡已经充值掉了，所以这里不允许返回出错
void GwCardTrade::OnBuyCardSucceeded(SRequisition* pRequisition, const char* szCardInfo)
{
    long lNow, lStopTime;
    lNow = (long)time(NULL);
    lStopTime = lNow+CCardTradeList::e_lDefaultTradeTime;

    // 先检查是否存在
    CCardTradeList::STradeRecord* pTradeInfo = m_list.Find(pRequisition->lBuyConsignationID);
    if (pTradeInfo==NULL)
    {
        // 这里已经找不到寄卖信息了，这是不应该出现的情况
    }else
    {
        // 5, 在Gameworld上向寄售玩家帐号里面添加金钱和信息
        if (!g_server.AddMoneyToGift(pRequisition->dwSalesmanUserID, pTradeInfo->lPrice, R(MSG_CARD_TRADE_OFF_GIFT_MSG)))
        {
            Log(ECOP_ADD_GIFT_FAILED, pRequisition->szCardID, pRequisition->dwSalesmanUserID, pTradeInfo->lPrice);
        }
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_TRADE_OFF_MSG), pTradeInfo->lPrice);
        g_server.SendSystemMessageToUser(pRequisition->dwSalesmanUserID, g_strStaticBuffer);
		string StrItemID = "";
	    g_server.m_mail.AddMail(pRequisition->dwSalesmanUserID, 0, R(MSG_CARD_TRADE_OFF_MAIL_MSG), g_strStaticBuffer, true,StrItemID,0);
    }

    // 通知购买玩家
    //g_server.SendSystemMessageToUser((DWORD)pRequisition->lBuyerUserID, "买卡成功。");
    if (pTradeInfo)
    {
        GWUser* pUser = g_server.FindActorById(pRequisition->lBuyerUserID);
	    if (pUser)
        {
	        int iSe = g_server.GetRegionSession(pUser->regionID);
	        if (iSe!=-1)
            {
	            CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_buy_ret);
                cmd.WriteLong(pRequisition->lBuyerUserID);
	            cmd.WriteByte(1);
	            cmd.WriteLong(pRequisition->lBuyConsignationID);
                cmd.WriteLong(pTradeInfo->lPrice);
                cmd.WriteString((char*)R(MSG_CARD_BUY_BACK_MSG));
	            g_server.m_region.SendToPeer(iSe);
            }

            // 发送系统公告, 格式为： xxx 花费 xxx 购买了 xxx 的 xxx
            //     如： 苦力 花费 500000 购买了 财神 的 300点点卡
            HideName(pRequisition->szSalesmanUsername, g_strStaticBuffer+512);
            rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_BUY_BROADCAST),
                pUser->name.c_str(), pTradeInfo->lPrice, g_strStaticBuffer+512, szCardInfo);
            g_server.BroadcastBulletin(g_strStaticBuffer, false);
        }
    }

    // 6, 删除寄售数据库记录
    RemoveDBRecord(pRequisition->lBuyConsignationID);

    // 7, 删除寄售记录
    m_list.Remove(pRequisition->lBuyConsignationID);

    UpdateCardTradeList();

    // 删除申请列表
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(pRequisition->lRequisitionID);
    if (it!=m_mapRequisition.end())
    {
        m_mapRequisition.erase(it);
    }
}

void GwCardTrade::OnBuyCardFailed(SRequisition* pRequisition, const char* szCardInfo)
{
    OnBuyCardFailed(pRequisition->lBuyerUserID, pRequisition->lBuyConsignationID, pRequisition->lPrice, pRequisition->szCardID, szCardInfo);

    // 删除申请列表
    std::map<long, SRequisition>::iterator it = m_mapRequisition.find(pRequisition->lRequisitionID);
    if (it!=m_mapRequisition.end())
    {
        m_mapRequisition.erase(it);
    }
}

void GwCardTrade::OnBuyCardFailed(long lUserID, long lConsignationID, long lPrice, const char* szCardID, const char* szCardInfo)
{
    // 通知Region还钱
    GWUser* pUser = g_server.FindActorById(lUserID);
	if (pUser)
    {
	    int iSe = g_server.GetRegionSession(pUser->regionID);
	    if (iSe!=-1)
        {
	        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_buy_ret);
	        cmd.WriteLong(lUserID);
	        cmd.WriteByte(0);
	        cmd.WriteLong(lConsignationID);
            cmd.WriteLong(lPrice);
            cmd.WriteString((char*)szCardInfo);
	        g_server.m_region.SendToPeer(iSe);
            return;
        }
    }
    rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_BUY_FAILED_MSG), lPrice);
	string StrItemID = "";
	g_server.m_mail.AddMail(lUserID, 0, R(MSG_CARD_BUG_FAILED_MAIL_MSG), g_strStaticBuffer, true,StrItemID,0);
    if (!g_server.AddMoneyToGift(lUserID, lPrice, R(MSG_CARD_BUG_FAILED_GIFT_MSG)))
    {
        Log(ECOP_ADD_GIFT_FAILED, szCardID, lUserID, lPrice);
    }
}

bool GwCardTrade::CancelConsign(long lUserID, long lConsignationID)
{
    long lNow, lStopTime;
    lNow = (long)time(NULL);
    lStopTime = lNow+CCardTradeList::e_lDefaultTradeTime;

    // 先检查是否存在
    CCardTradeList::STradeRecord* pTradeInfo = m_list.Find(lConsignationID);
    if (pTradeInfo==NULL)
    {
        // 这里已经找不到寄卖信息了，这是不应该出现的情况
    }else
    {
		CG_CmdPacket& cmd = g_server.m_login.BeginCmd(gws2lgs_card_failed);
		cmd.WriteString(pTradeInfo->szCardID);
		g_server.m_login.SendCmd();

        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONSIGN_CANCEL));
        g_server.SendSystemMessageToUser(pTradeInfo->dwSalesmanUserID, g_strStaticBuffer);
		string StrItemID = "";
	    g_server.m_mail.AddMail(pTradeInfo->dwSalesmanUserID, 0, R(MSG_CARD_CONSIGN_CANCEL_MAIL), g_strStaticBuffer, true,StrItemID,0);
    }

    // 6, 删除寄售数据库记录
    RemoveDBRecord(lConsignationID);

    // 7, 删除寄售记录
    m_list.Remove(lConsignationID);

    UpdateCardTradeList();
    return true;
}

bool GwCardTrade::CancelConsign(long lUserID, const char* szCardID)
{
    CCardTradeList::STradeRecord* pTradeInfo = m_list.Find(szCardID);
    if (pTradeInfo)
    {
        return CancelConsign(lUserID, pTradeInfo->lConsignationID);
    }
    return false;
}

long GwCardTrade::AddDBRecord(char cCardType, const char* szCardID, const char* szCardPwdMD5, long lGroupType,
    const char* szCardInfo, long lAccountID, const char* szAccountName,
    long lUserID, const char* szUsername, long lPrice, long lStartTime, long lEndTime)
{
	MYSQL_RES *pDBResult;
    rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTrade`"
        "(`ID`,`CardType`,`GroupType`,`CardID`,`CardPwd`,`CardInfo`,`UserID`,`UserName`,"
        "`AccountID`,`AccountName`,`Price`,`StartTime`,`EndTime`) "
        "VALUES('', %d, %d, '%s','%s','%s', %d, '%s', %d, '%s', %d, FROM_UNIXTIME(%u), FROM_UNIXTIME(%u))",
        cCardType, lGroupType, szCardID, szCardPwdMD5, szCardInfo, lUserID, szUsername,
        lAccountID, szAccountName, lPrice, lStartTime, lEndTime);
    if ( !g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult) )
    {
        return 0;
    }else
    {
		mysql_free_result(pDBResult);
    }

    MYSQL_RES* pMySqlResult;
    rt2_sprintf(g_strStaticBuffer, "SELECT `ID` FROM `T_CardTrade` WHERE `CardID` = '%s' ", szCardID);
    if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pMySqlResult, true))
    {
        return 0;
    }

    if (mysql_num_rows(pMySqlResult)!=1)
    {
        mysql_free_result(pMySqlResult);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(pMySqlResult);
    long lConsignationID = atol(row[0]);
    mysql_free_result(pMySqlResult);
    return lConsignationID;
}

void GwCardTrade::RemoveDBRecord(long lConsignationID)
{
    // 从数据库中删除
	MYSQL_RES *pDBResult;
	rt2_sprintf(g_strStaticBuffer, "DELETE FROM `T_CardTrade` WHERE `ID`=%d", lConsignationID);
	if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
		return;
    }else
    {
		mysql_free_result(pDBResult);
    }
}

void GwCardTrade::UpdateCardTradeList()
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_update);
    m_list.WriteTo(&cmd);
	g_server.m_region.BroadcastCmd();
}

void GwCardTrade::Log(ECardOP eCOP, const char* szCardID, long lUserID, long lPrice)
{
    switch (eCOP)
    {
    case ECOP_CONSIGN:
        rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTradeLog`(`ID`,`OpType`,`CardID`,`UserID`,`Price`,`opTime`) "
            "VALUES('',%d,'%s',%d,%d,FROM_UNIXTIME(%u))", 1, szCardID, lUserID, lPrice, time(NULL));
        break;
    case ECOP_BUY_SUCCEEDED:
        rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTradeLog`(`ID`,`OpType`,`CardID`,`UserID`,`Price`,`opTime`) "
            "VALUES('',%d,'%s',%d,%d,FROM_UNIXTIME(%u))", 2, szCardID, lUserID, lPrice, time(NULL));
        break;
    case ECOP_BUY_FAILED:
        rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTradeLog`(`ID`,`OpType`,`CardID`,`UserID`,`Price`,`opTime`) "
            "VALUES('',%d,'%s',%d,%d,FROM_UNIXTIME(%u))", 3, szCardID, lUserID, lPrice, time(NULL));
        break;
    case ECOP_ADD_GIFT_FAILED:
        rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTradeLog`(`ID`,`OpType`,`CardID`,`UserID`,`Price`,`opTime`) "
            "VALUES('',%d,'%s',%d,%d,FROM_UNIXTIME(%u))", 4, szCardID, lUserID, lPrice, time(NULL));
        break;
    case ECOP_BUY:
        rt2_sprintf(g_strStaticBuffer, "INSERT INTO `T_CardTradeLog`(`ID`,`OpType`,`CardID`,`UserID`,`Price`,`opTime`) "
            "VALUES('',%d,'%s',%d,%d,FROM_UNIXTIME(%u))", 5, szCardID, lUserID, lPrice, time(NULL));
        break;
    default:
        return;
    }
	MYSQL_RES *pDBResult;
	if (!g_server.m_database.ExecCommand(g_strStaticBuffer, &pDBResult))
    {
		return;
    }else
    {
		mysql_free_result(pDBResult);
    }
}

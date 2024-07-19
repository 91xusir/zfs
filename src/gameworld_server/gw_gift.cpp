#include "gw_server.h"

#define MAX_GET_GIFTS_COUNT		1

CGiftMgr g_GiftMgr;

CGiftMgr* GetGiftMgr()
{
	return &g_GiftMgr;
}

bool CGiftMgr::IsGiftWithdrawed(GWUser* pUser, DWORD index)
{
	CHECK(pUser);

	for (int i=0; i<(int)pUser->giftsWithdrawed.size(); i++)
	{
		if (pUser->giftsWithdrawed[i].index == index)
			return true;
	}

	return false;
}

void CGiftMgr::GetAllGifts(DWORD userID, std::vector<SGift>& gifts)
{
	gifts.clear();

	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)		return;

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT idx, level, type, attribute, UNIX_TIMESTAMP(addTime), UNIX_TIMESTAMP(limitTime), msg FROM T_gift WHERE charID=%d ORDER BY idx DESC", userID);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CGiftMgr::GetAllGifts] Can NOT access Gift DB!\n");
		return;
	}

	int count = mysql_num_rows(res);
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		DWORD index = atol(row[0]);
		if (!IsGiftWithdrawed(pUser, index))
		{
			SGift gift;
			gift.index = index;
			gift.userID = userID;
			gift.level = atol(row[1]);
			gift.type = atol(row[2]);
			if (row[3])
                gift.attribs = row[3];
			gift.addTime = atoi(row[4]);
			gift.limitTime = atoi(row[5]);
			if (row[6])
				gift.message = row[6];
			gifts.push_back(gift);
		}
	}
	mysql_free_result(res);

	return;
}

void CGiftMgr::GetGifts(DWORD userID, int level, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw)
{
	giftsCanWithdraw.clear();
	giftsCanNotWithdraw.clear();

	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)		return;

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT idx, level, type, attribute, UNIX_TIMESTAMP(addTime), UNIX_TIMESTAMP(limitTime), msg FROM T_gift WHERE charID=%d ORDER BY idx DESC", userID);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CGiftMgr::GetGifts] Can NOT access Gift DB!\n");
		return;
	}

	int count = mysql_num_rows(res);
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		DWORD index = atol(row[0]);
		if (!IsGiftWithdrawed(pUser, index))
		{
			SGift gift;
			gift.index = index;
			gift.userID = userID;
			gift.level = atol(row[1]);
			gift.type = atol(row[2]);
			if (row[3])
                gift.attribs = row[3];
			gift.addTime = atoi(row[4]);
			gift.limitTime = atoi(row[5]);
			if (row[6])
				gift.message = row[6];

			if (gift.level <= level)
			{
				if (giftsCanWithdraw.size() < MAX_GET_GIFTS_COUNT)
					giftsCanWithdraw.push_back(gift);
			}
			else 
			{
				if (giftsCanNotWithdraw.size() < MAX_GET_GIFTS_COUNT)
					giftsCanNotWithdraw.push_back(gift);
			}
		}
	}
	mysql_free_result(res);

	return;
}

bool CGiftMgr::OnRecvWithdrawSuccess(DWORD userID, SGift& gift)
{
	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)		return false;

	pUser->giftsWithdrawed.push_back(gift);

	// 为了避免存盘高峰，如果礼物超过一定数量，就尽量存盘
	if (pUser->giftsWithdrawed.size() >= 20 && g_server.m_database.m_commandQueue.GetFreeSize() > 200)
	{
		g_server.m_database.SaveUserData(pUser, false, 0, NULL);
		GWLoginClient *lc = g_server.FindLoginClientByAccountId(pUser->accountId);
		if (lc)
		{
			lc->m_lastSaveTime = time(NULL);
		}
	}

	return true;
}

bool CGiftMgr::RemoveInDB(DWORD index)
{
	GWDBCommand cmd;
	//MYSQL_RES* res;
	rt2_sprintf(cmd.command, "DELETE FROM T_gift WHERE idx=%d", index);
	if (!g_server.m_database.PushCommand(cmd))
	{
		ERR("[CGiftMgr::RemoveInDB] Can NOT Push to Gift DB!\n");
		return false;
	}

	return true;
}

void CGiftMgr::NetSend_g2r_query_gifts_response(DWORD userID, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw)
{
	short i = 0;
	short countCanWithdraw = (short)giftsCanWithdraw.size();
	short countCanNotWithdraw = (short)giftsCanNotWithdraw.size();
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_query_gifts_response);
	cmd.WriteLong(userID);
	cmd.WriteShort(countCanWithdraw);
	for (i=0; i<countCanWithdraw; i++)
	{
		giftsCanWithdraw[i].Serialize(&cmd);
	}
	cmd.WriteShort(countCanNotWithdraw);
	for (i=0; i<countCanNotWithdraw; i++)
	{
		giftsCanNotWithdraw[i].Serialize(&cmd);
	}
	g_server.m_region.SendCmd();
}

bool CGiftMgr::AddGift(DWORD userID, int level, int type, const char* attrib, DWORD addTime/*=0*/, DWORD limitTime/*=0*/, const char* msg/*=""*/)
{
	if (addTime==0)
		addTime = time(NULL);
    if (limitTime==0)
        limitTime = 0x7fffffff;

	if (!GiftType_IsValid(type))
		return false;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "INSERT INTO T_gift(idx, charID, level, type, attribute, addTime, limitTime, msg) VALUES("
		"'',"
		"%d,"
		"%d,"
		"%d,"
		"'%s',"
		"FROM_UNIXTIME(%u),"
		"FROM_UNIXTIME(%u),"
		"'%s')",
		userID,
		level,
		type,
		attrib,
		addTime,
		limitTime,
		msg);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return false;
	}

	mysql_free_result(pDBResult);
	return true;
}

bool CGiftMgr::ClearGift(DWORD userID)
{
	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "DELETE FROM T_gift WHERE charID = %d", userID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return false;
	}

	mysql_free_result(pDBResult);
	return true;
}

bool GiftType_IsValid(int giftType)
{
	return (giftType>0 && giftType<GT_Count);
}

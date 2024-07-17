#include "gw_server.h"


CGWItemIDMgr g_GWItemIDMgr;


bool CGWItemIDMgr::GetIDData(int idx, DWORD &beginID, DWORD &currentID, DWORD &maxID)
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    rt2_sprintf(cmd.command, "SELECT ItemID,MaxID,BeginID FROM T_ItemID WHERE RegionID=%d LIMIT 1", idx);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        const char* pErrMsg = "T_ItemID数据库出错";
        ERR1("%s\n", pErrMsg);
        return false;
    }
    else
    {
        int count = mysql_num_rows(pDBResult);
        if (count==0)
        {
            const char* pErrMsg = "T_ItemID数据库未配置";
            ERR1("%s\n", pErrMsg);
            CHECKEX(pErrMsg);
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        currentID = atol(row[0]);
        maxID = atol(row[1]);
        beginID = atol(row[2]);
        mysql_free_result(pDBResult);

        return true;
    }

    return true;
}

bool CGWItemIDMgr::SetIDData(int idx, DWORD beginID, DWORD currentID, DWORD maxID)
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    rt2_sprintf(cmd.command, "UPDATE T_ItemID SET ItemID=%d,BeginID=%d,MaxID=%d WHERE RegionID=%d", currentID, beginID, maxID, idx);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        return false;
    }
    else
    {
        mysql_free_result(pDBResult);
        return true;
    }
}

void CGWItemIDMgr::SendIDData(int regionID)
{
    DWORD nowID, maxID, beginID;
    if (!GetIDData(regionID, beginID, nowID, maxID))
    {
        ERR1("[CGWItemIDMgr::SendIDData] GetIDData Error (regionID=%d)\n", regionID);
        return;
    }

	if (regionID==GMSever_RegionID)
	{
		CG_CmdPacket& cmd = g_server.m_gm.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(gw2gm_itemid);
		cmd.WriteLong(nowID);
		cmd.WriteLong(maxID);
		g_server.m_gm.SendCmd();
	}
	else if (regionID==GWServer_RegionID)
	{
	}
	else
	{
		GWRegion& rg = g_server.m_regionTable[regionID];
		if (rg.sessionIndex == -1)		return;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_itemid);
		cmd.WriteLong(nowID);
		cmd.WriteLong(maxID);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

bool CGWItemIDMgr::ChangeNewIDIndex(int regionID)
{
    DWORD currentID, beginID, maxID;
    if (!GetIDData(regionID, beginID, currentID, maxID))
    {
        ERR("[CGWItemIDMgr::ChangeNewIDIndex] GetIDData Error!\n");
        return false;
    }

    float rate = 1.0f * (currentID-beginID) / (maxID - beginID);
    if (currentID == beginID)
    {
        ERR("[CGWItemIDMgr::ChangeNewIDIndex] currentID is equal to beginID\n");
        return false;
    }
    if (rate < 0.8f)
    {
        ERR2("[CGWItemIDMgr::ChangeNewIDIndex] idx=%d, rate=%f, are you sure to drop this id resource?!\n", regionID, rate);
        return false;
    }

    // Find a new resource (从idx=29开始)
    int newIdx = 29;
    for (; newIdx>5; newIdx--)
    {
        DWORD curr1, beg1, max1;
        if (!GetIDData(newIdx, beg1, curr1, max1))
            continue;
        if (curr1 == beg1) // 资源未使用过
        {
            // 交换这两个资源
            if (!SetIDData(regionID, beg1, curr1, max1))
                return false;
            if (!SetIDData(newIdx, beginID, currentID, maxID))
                return false;
            SendIDData(regionID);
            return true;
        }
    }
    return false;
}

void CGWItemIDMgr::OnRegionRegister(int regionID)
{
    DWORD currentID, beginID, maxID;
    if (!GetIDData(regionID, beginID, currentID, maxID))
    {
        ERR("[CGWItemIDMgr::OnRegionRegister] GetIDData Error!\n");
        return;
    }

    float rate = 1.0f * (currentID-beginID) / (maxID-beginID);
    if (rate >= 0.94)
    {
        if (!ChangeNewIDIndex(regionID))
        {
            ERR("[CGWItemIDMgr::OnRegionRegister] ChangeNewIDIndex Error!\n");
        }
        LOG1("regionID %d change T_ItemID resource\n", regionID);
    }

	SendIDData(regionID);
}




CGWItemIDFactory g_GWItemIDFactory;
DWORD GetNextItemID()
{
	return g_GWItemIDFactory.GetNextItemID();
}

CGWItemIDFactory::CGWItemIDFactory()
{
	m_dwNextID = 0;
	m_dwIDInDB = 0;
	m_dwIDMaxInDB = 0;
}

CGWItemIDFactory::~CGWItemIDFactory()
{
}

bool CGWItemIDFactory::Init()
{
	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	DWORD nowID, maxID, beginID;
	rt2_sprintf(cmd.command, "SELECT ItemID,MaxID,BeginID FROM T_ItemID WHERE RegionID=%d LIMIT 1", GWServer_RegionID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		const char* pErrMsg = "T_ItemID数据库出错";
		ERR1("%s\n", pErrMsg);
		CHECKEX(pErrMsg);
		return false;
	}
	else
	{
		int count = mysql_num_rows(pDBResult);
		if (count==0)
		{
            const char* pErrMsg = "T_ItemID数据库未配置";
            ERR1("%s\n", pErrMsg);
			CHECKEX(pErrMsg);
			return false;
		}
		MYSQL_ROW row = mysql_fetch_row(pDBResult);
		nowID = atol(row[0]);
		maxID = atol(row[1]);
		beginID = atol(row[2]);
		mysql_free_result(pDBResult);

		// 计算ID资源的使用率，是否需要发出警告
		static bool bFirstRun = true;
		if (bFirstRun)
		{
			bFirstRun = false;
			int delta1 = nowID - beginID;
			int delta2 = maxID - nowID;
			float rate = delta1 / delta2;
			if (rate>5.0f)
			{
//				ERR("道具ID已经使用了80%");
			}
		}
	}

	m_dwNextID = nowID+1;
	m_dwIDInDB = nowID;

	UpdateDB();
	return true;
}

DWORD CGWItemIDFactory::GetNextItemID()
{
	//	CHECK(m_dwNextID!=0);
	if (m_dwNextID==0)
		return 0xffffffff;

	if (m_dwNextID+ItemID_UpdateDB_Circle >= m_dwIDInDB)
	{
		UpdateDB();
	}
#ifdef _DEBUG
	LOG1("CreateItem ITEMID=%d\n", m_dwNextID);
#endif
	return m_dwNextID++;
}

void CGWItemIDFactory::UpdateDB()
{
	m_dwIDInDB += ItemID_UpdateDB_Circle;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "UPDATE T_ItemID SET ItemID=%d WHERE RegionID=%d", m_dwIDInDB, GWServer_RegionID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		ERR("Can NOT update Database(Table:T_ItemID)\n");
		return;
	}
	else
	{
		mysql_free_result(pDBResult);
		return;
	}
}

bool CBankPass::UpdatePass(DWORD userID, short operateID, const char* pass)
{
	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)
		return false;
	if (!pass)
		return false;
	if ( (strlen(pass)!=16) && (strlen(pass)!=0) )
		return false;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "UPDATE T_bank SET Password='%s' WHERE UserID=%d", pass, pUser->userID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		ERR("[CBankPass::AddPass] DB error!\n");
		Send_g2r_bankpass_result(userID, operateID, 1, pass);
		return false;
	}

	mysql_free_result(pDBResult);

	Send_g2r_bankpass_result(userID, operateID, 0, pass);

	return true;
}

bool CBankPass::Send_g2r_bankpass_result(DWORD userID, short operateID, short errorCode, const char* newPass)
{
	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)			return false;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_bankpass_result);
	cmd.WriteLong(userID);
	cmd.WriteShort(operateID);
	cmd.WriteShort(errorCode);
	cmd.WriteString((char*)newPass);
	g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
	return true;
}


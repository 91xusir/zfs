#include "GW_ActivityManager.h"
#include "gw_server.h"

extern CRS_Activity g_TableActivity;
extern GWServer g_server;

const char* GW_ActivityManager::c_sUserInfoTable = "user_activity";
const char* GW_ActivityManager::c_sUserEnterTable = "enter_activity";
//////////////////////////////////////////////////////////////////////////

GW_ActivityManager::GW_ActivityManager(void) :
c_CycleTime(59 * 1000),
m_lLastUpdataTime(rtMilliseconds())
{
}

GW_ActivityManager::~GW_ActivityManager(void)
{
	Shutdown();
}

bool	GW_ActivityManager::StartUp(void)
{
	ActivityRes *pRes = NULL;
	int iIndex = g_TableActivity.GetFirstActivity(&pRes);
	while(pRes)
	{
		GW_ActivityBase *pActivity = CreateActivity(iIndex, pRes);
		if(pActivity)
			pActivity->GetDatabaseActivity();
		iIndex = g_TableActivity.GetNextActivity(&pRes);
	}

	return true;
}

void	GW_ActivityManager::Shutdown(void)
{
	for(GW_ACTIVITYS::iterator itAct = mapActivitys.begin();
		itAct != mapActivitys.end();
		itAct++)
	{
		itAct->second->SetDatabaseActivity();
		DEL_ONE(itAct->second);
	}
	mapActivitys.clear();
}

void	GW_ActivityManager::UpdataActivity(int iID, const char *sActivity)
{
	GW_ActivityBase *pActivity = GetActivity(iID);
	if(pActivity)
		pActivity->UpdataActivity(sActivity);
}

void	GW_ActivityManager::GetUserEnterActivity(int iSession, long lUserID)
{
	if(lUserID <= 0)
		return;

	GWDBCommand cmd;
	cmd.succeeded = GW_ActivityManager::ReturnUserEnterActivity;
	//cmd.failed = GW_ActivityManager::ReturnUserEnterActivity;
	GWUser* pUser = g_server.FindActorById(lUserID);
	if(!pUser)
		return;
	long *pResultParam = RT_NEW long[2];
	pResultParam[0] = iSession;
	pResultParam[1] = lUserID;
	cmd.data = pResultParam;
	cmd.type = dbCmdQuery;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"SELECT * FROM `%s` WHERE `UserName` = '%s'",
		c_sUserEnterTable, pUser->name.c_str());
	g_server.m_ActDB.PushCommand(cmd);
}

void	GW_ActivityManager::DeleteAllInfoByUser(std::string &sUserName)
{
	if(sUserName.empty())
		return;

	GWDBCommand cmd;
	cmd.type = dbCmdQuery;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"DELETE FROM `%s` WHERE `UserName` = '%s'",
		c_sUserEnterTable, sUserName.c_str());
	g_server.m_ActDB.PushCommand(cmd);

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"DELETE FROM `%s` WHERE `UserName` = '%s'",
		c_sUserInfoTable, sUserName.c_str());
	g_server.m_ActDB.PushCommand(cmd);
}

void	GW_ActivityManager::ReturnUserInfo(GWDBResult* pDBResult)
{
	if(!pDBResult || !pDBResult->data || !pDBResult->res)
	{
		DEL_ARRAY(pDBResult->data);
		return;
	}

	long iSession = *(long*)(pDBResult->data);
	long lActID = *((long*)(pDBResult->data) + 1);
	long lUserID = *((long*)(pDBResult->data) + 2);
	long lState = *((long*)(pDBResult->data) + 3);
	long lParam[5] = {0};
	for(int ii=0; ii<5; ii++)
		lParam[ii] = *((long*)(pDBResult->data) + 4 + ii);
	DEL_ARRAY(pDBResult->data);
	if(mysql_num_rows(pDBResult->res) <= 0)
	{
		GWUser* pUser = g_server.FindActorById(lUserID);
		if(pUser)
		{
			GWDBCommand cmd;
			cmd.type = dbCmdQuery;
			rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO `user_activity` VALUES (%d, '%s', '')", lActID, pUser->name.c_str());
			g_server.m_ActDB.PushCommand(cmd);
			GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, "", lParam);
		}
		return;
	}
	MYSQL_ROW row = mysql_fetch_row(pDBResult->res);
	GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, std::string(row[0]), lParam);
}

void	GW_ActivityManager::ReturnActivityInfo(GWDBResult* pDBResult)
{
	if(!pDBResult || !pDBResult->data || !pDBResult->res)
		return;

	GW_ActivityBase *pActivity = (GW_ActivityBase*)pDBResult->data;
	std::string sBuffer;
	pActivity->GetSerializeString(sBuffer);
	if(mysql_num_rows(pDBResult->res) <= 0)
	{
		GWDBCommand cmd;
		cmd.type = dbCmdQuery;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
			"INSERT INTO `%s` VALUES (%d, '%s', '%s')",
			pActivity->GetTableName(), pActivity->GetID(), pActivity->GetActName(), sBuffer.c_str());
		g_server.m_ActDB.PushCommand(cmd);
		return;
	}
	MYSQL_ROW row = mysql_fetch_row(pDBResult->res);
	pActivity->UpdataActivity(row[0]);
}

void	GW_ActivityManager::ReturnUserEnterActivity(GWDBResult* pDBResult)
{
	//GW_ActivityManager::GetUserEnterActivity唯一对应
	if(!pDBResult || !pDBResult->data || !pDBResult->res)
	{
		DEL_ARRAY(pDBResult->data);
		return;
	}

	long iSession = *(long*)(pDBResult->data);
	long lUserID = *((long*)(pDBResult->data) + 1);
	DEL_ARRAY(pDBResult->data);
	GWUser* pUser = g_server.FindActorById(lUserID);
	if(!pUser)
		return;

	std::string	sBuffer;
	unsigned int iCount = mysql_num_fields(pDBResult->res);
	MYSQL_ROW pRow = mysql_fetch_row(pDBResult->res);
	unsigned long *plStringLen = mysql_fetch_lengths(pDBResult->res);
	MYSQL_FIELD *pField = mysql_fetch_field(pDBResult->res);
	if(mysql_num_rows(pDBResult->res) <= 0)
	{
		std::string sSQL("INSERT INTO `");
		sSQL += c_sUserEnterTable;
		sSQL += "` VALUES (";
		char sTemp[20] = {0};
		rt2_snprintf(sTemp, 20, "%d", pUser->userID);
		sSQL += sTemp;
		sSQL += ", '";
		sSQL += pUser->name;
		sSQL += "'";
		for(unsigned int ii=2; ii<iCount; ii++)
		{
			sSQL += ", '0'";
		}
		sSQL += ")";

		GWDBCommand cmd;
		cmd.type = dbCmdQuery;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "%s", sSQL.c_str());
		g_server.m_ActDB.PushCommand(cmd);

		for(unsigned int ii=1; ii<iCount; ii++)
		{
			sBuffer += pField[ii].name;
			sBuffer += ".0";
			if(ii+1 < iCount)
				sBuffer += ".";
		}
	}
	else
	{
		char *pCurrent = pRow[0];
		pCurrent += plStringLen[0] + 1;
		for(unsigned int ii=1; ii<iCount; ii++)
		{
			sBuffer += pField[ii].name;
			sBuffer += ".";
			sBuffer += pCurrent;
			pCurrent += plStringLen[ii] + 1;
			if(ii+1 < iCount)
				sBuffer += ".";
		}
	}

	CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
	command.WriteShort(ACTMSG_UPDATAUSERJOIN);
	command.WriteString(pUser->name.c_str());
	command.WriteString(sBuffer.c_str());
	g_server.m_region.SendToPeer(iSession);
}

GW_ActivityBase*	GW_ActivityManager::CreateActivity(int iID, ActivityRes *pRes)
{
	if(iID <= 0 || !pRes)
		return NULL;

	GW_ActivityBase *pActivity = GetActivity(pRes->eActivityType);
	if(pActivity)
	{
		return NULL;
	}
	else
	{
		switch(pRes->eActivityType)
		{
		case ACTIVITY_BASE:
			break;
		case ACTIVITY_TELPHER:
			pActivity = RT_NEW GW_ActivityTelpher(iID, pRes);
			break;
		case ACTIVITY_TREASURE:
			pActivity = RT_NEW GW_ActivityTreasure(iID, pRes);
			break;
		case ACTIVITY_CACHOT:
			pActivity = RT_NEW GW_ActivityCachot(iID, pRes);
			break;
		}

		if(pActivity)
		{
			mapActivitys[pRes->eActivityType] = pActivity;
			return pActivity;
		}
	}

	return NULL;
}

bool	GW_ActivityManager::RemoveActivity(int iID)
{
	GW_ACTIVITYS::iterator itAct = mapActivitys.find(iID);
	if(itAct != mapActivitys.end())
	{
		DEL_ONE(itAct->second);
		mapActivitys.erase(itAct);
		return true;
	}

	return false;
}

void	GW_ActivityManager::OnEvent(void)
{
	long lCurTime = rtMilliseconds();
	if(m_lLastUpdataTime + c_CycleTime > lCurTime)
		return;

	for(GW_ACTIVITYS::iterator itAct = mapActivitys.begin();
		itAct != mapActivitys.end();
		itAct++)
		itAct->second->OnEvent(lCurTime);
	m_lLastUpdataTime = lCurTime;
}

void	GW_ActivityManager::UpdataActivityToRegion(int iRegionID)
{
	if(iRegionID < 0 )
		return;

	for(GW_ACTIVITYS::iterator itAct = mapActivitys.begin();
		itAct != mapActivitys.end();
		itAct++)
	{
		CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
		command.WriteShort(ACTMSG_CREATE);
		command.WriteLong(itAct->first);
		command.WriteShort(itAct->second->GetType());
		std::string sBuffer;
		itAct->second->GetSerializeString(sBuffer);
		command.WriteString(sBuffer.c_str());
		g_server.m_region.SendToPeer(iRegionID);
	}
}

void	GW_ActivityManager::Dispose(long lRegionID, CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	ACTIVITY_MESSAGE eMsg = ACT_MESSAGE_OVER;
	pCommand->ReadShort((short*)&eMsg);

	long lActType = 0;
	GW_ActivityBase *pActivity = NULL;
	switch(eMsg)
	{
	case ACTMSG_GETUSERINFO:
		pCommand->ReadLong(&lActType);
		pActivity = GetActivity(lActType);
		if(pActivity && lRegionID >= 0)
			pActivity->GetUserInfo(lRegionID, pCommand);
		break;
	case ACTMSG_SAVEUSERINFO:
		SaveUserInfo(pCommand);
		break;
	}
}

void	GW_ActivityManager::SetUserInfo(int iSession, long lActID, long lUserID, long eState, std::string sActivity, long *plParam)
{
	if(iSession < 0 || lUserID <= 0 || lActID <= 0)
		return;

	CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
	command.WriteShort(ACTMSG_GETUSERINFO);
	command.WriteLong(lActID);
	command.WriteLong(lUserID);
	command.WriteLong(eState);
	command.WriteString(sActivity.c_str());
	if(plParam)
	{
		for(int ii=0; ii<5; ii++)
			command.WriteLong(plParam[ii]);
	}
	g_server.m_region.SendToPeer(iSession);
	//g_server.m_region.BroadcastCmd();
}

void	GW_ActivityManager::SaveUserInfo(CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	long lActID = 0;
	pCommand->ReadLong(&lActID);
	char *pTemp = NULL;
	pCommand->ReadString(&pTemp);
	std::string sUserName(pTemp ? pTemp : "");
	short iEnterActive = 0;
	pCommand->ReadShort(&iEnterActive);
	pCommand->ReadString(&pTemp);
	std::string sBuffer(pTemp ? pTemp : "");

	GW_ActivityBase *pActivity = GetActivity(lActID);
	if(pActivity)
		pActivity->SaveUserInfo(sUserName, iEnterActive, sBuffer);
}

GW_ActivityBase*	GW_ActivityManager::GetActivity(int iID)
{
	GW_ACTIVITYS::iterator itAct = mapActivitys.find(iID);
	if(itAct != mapActivitys.end())
		return itAct->second;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

GW_ActivityBase::GW_ActivityBase(int iID, ActivityRes *pRes) :
m_iID(iID),
m_eType(pRes ? pRes->eActivityType : ACTIVITY_BASE),
m_sActName(pRes ? pRes->sActName : ""),
m_iEventCycle(pRes ? pRes->iEventCycle : 0),
m_pActivityInfo(0),
m_lLastSpring(rtMilliseconds())
{
	if(pRes)
	{
		for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
		{
			for(int jj=0; jj<ActivityRes::iMaxEventCount; jj++)
			{
				m_bCanFore[ii][jj] = true;
				for(int kk=0; kk<5; kk++)
					m_iForeTime[ii][jj][kk] = pRes->iForeTime[ii][jj][kk];
			}
		}
		for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
		{
			for(int jj=0; jj<5; jj++)
			{
				m_iStartTime[ii][jj] = pRes->iStartTime[ii][jj];
				m_iOverTime[ii][jj] = pRes->iOverTime[ii][jj];
			}
		}
		for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
		{
			m_sStartInfo[ii] = pRes->sStartInfo[ii];
			m_sOverInfo[ii] = pRes->sOverInfo[ii];
			m_sCycleInfo[ii] = pRes->sCycleInfo[ii];
			m_sForeInfo[ii] = pRes->sForeInfo[ii];
		}
	}
	else
	{
		for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
		{
			for(int jj=0; jj<ActivityRes::iMaxEventCount; jj++)
			{
				m_bCanFore[ii][jj] = true;
				for(int kk=0; kk<5; kk++)
					m_iForeTime[ii][jj][kk] = -1;
			}
		}
		for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
		{
			for(int jj=0; jj<5; jj++)
			{
				m_iStartTime[ii][jj] = -1;
				m_iOverTime[ii][jj] = -1;
			}
		}
	}
}

GW_ActivityBase::~GW_ActivityBase(void)
{
	DEL_ONE(m_pActivityInfo);
}

void	GW_ActivityBase::UpdataActivity(const char *sActivity)
{
	if(!sActivity || sActivity[0] == 0 || !m_pActivityInfo)
		return;
	m_pActivityInfo->SerializeToThis(&std::string(sActivity));
}

void	GW_ActivityBase::OnEvent(long lCurTime)
{
	if(!m_pActivityInfo)
		return;

	int iTemp = 0;
	int iCurDate[5] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);

	for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
	{
		if(m_iStartTime[ii][0] < 0)
			break;
		bool bStartup = true;
		for(int jj=0; jj<5; jj++)
		{
			if(iCurDate[jj] != m_iStartTime[ii][jj] && m_iStartTime[ii][jj] != 0)
			{
				bStartup = false;
				break;
			}
		}
		if(bStartup)
			Startup(ii);
	}

	for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
	{
		if(m_iOverTime[ii][0] < 0)
			break;
		bool bShutdown = true;
		for(int jj=0; jj<5; jj++)
		{
			if(iCurDate[jj] != m_iOverTime[ii][jj] && m_iOverTime[ii][jj] != 0)
			{
				bShutdown = false;
				break;
			}
		}
		if(bShutdown)
			Shutdown(ii);
	}

	for(int ii=0; ii<ActivityRes::iMaxEventCount; ii++)
	{
		for(int jj=0; jj<ActivityRes::iMaxEventCount; jj++)
		{
			if(m_iForeTime[ii][jj][0] < 0)
				continue;
			bool bForeTime = true;
			for(int kk=0; kk<5; kk++)
			{
				if(iCurDate[kk] != m_iForeTime[ii][jj][kk] && m_iForeTime[ii][jj][kk] != 0)
				{
					bForeTime = false;
					break;
				}
			}
			if(bForeTime && m_pActivityInfo->eState != AS_START)
				ForeTime(ii, jj);
		}
	}

	if(m_iEventCycle > 0 && m_lLastSpring + m_iEventCycle <= lCurTime && m_pActivityInfo->eState == AS_START)
	{
		CycleTime(rand() % ActivityRes::iMaxEventCount);
		m_lLastSpring = lCurTime;
	}
}

//void	GW_ActivityBase::GetUserInfo(int iRegionID, CG_CmdPacket *pCommand)
//{
//	if(!pCommand)
//		return;
//
//	ACTIVITY_TYPE eType = ACTIVITY_TYPE_OVER;
//	pCommand->ReadShort((short*)&eType);
//	if(eType != m_eType)
//		return;
//	ACTIVE_STATE eState = AS_STATE_OVER;
//	pCommand->ReadShort((short*)&eState);
//	if(eState >= AS_STATE_OVER || eState <= AS_NONE)
//		return;
//	char *pName = NULL;
//	pCommand->ReadString(&pName);
//	if(!pName || pName[0] == 0)
//		return;
//	long lUserID = 0;
//	pCommand->ReadLong(&lUserID);
//	long lParam[5] = {0};
//	for(int ii=0; ii<5; ii++)
//		pCommand->ReadLong(&lParam[ii]);
//
//	//向数据库查询指定用户的活动Info信息,数据库返回时调用GW_ActivityManager::SetUserInfo(...)
//	//要查询的数据库表名==m_DBTableName
//	GWDBCommand cmd;
//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
//		"SELECT `ActivityState` FROM `user_activity` WHERE `ActivityID` = %d AND `UserName` = '%s'",
//		m_iID, pName);
//	long *pResultParam = RT_NEW long[4+5];
//	pResultParam[0] = iRegionID;
//	pResultParam[1] = m_iID;
//	pResultParam[2] = lUserID;
//	pResultParam[3] = eState;
//	for(int ii=0; ii<5; ii++)
//		pResultParam[4+ii] = lParam[ii];
//	cmd.data = pResultParam;
//	cmd.succeeded = GW_ActivityManager::ReturnUserInfo;
//	cmd.failed = GW_ActivityManager::ReturnUserInfo;
//	cmd.type = dbCmdQuery;
//	g_server.m_ActDB.PushCommand(cmd);
//}

void	GW_ActivityBase::GetDatabaseActivity(void)
{
	//if(m_DBTableName.empty() || m_sActName.empty())
	//	return;

	//GWDBCommand cmd;
	//rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
	//	"SELECT `ActivityState` FROM `%s` WHERE `ActivityID` = '%d' AND `ActName` = '%s'",
	//	m_DBTableName.c_str(), m_iID, m_sActName.c_str());
	//cmd.data = this;
	//cmd.succeeded = GW_ActivityManager::ReturnActivityInfo;
	//cmd.type = dbCmdQuery;
	//g_server.m_ActDB.PushCommand(cmd);
}

void	GW_ActivityBase::SetDatabaseActivity(void)
{
	//std::string sBuffer;
	//if(!GetSerializeString(sBuffer))
	//	return;

	//MYSQL_RES *pRes = 0;
	//GWDBCommand cmd;
	//rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
	//	"DELETE FROM `%s` WHERE `ActivityID` = '%d'",// AND `ActName` = '%s'",
	//	m_DBTableName.c_str(), m_iID, m_sActName.c_str());
	//g_server.m_ActDB.ExecCommand(cmd.command, &pRes);
	////g_server.m_ActDB.PushCommand(cmd);

	//rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
	//	"INSERT INTO `%s` VALUES ('%d', '%s', '%s')",
	//	m_DBTableName.c_str(), m_iID, m_sActName.c_str(), sBuffer.c_str());
	//g_server.m_ActDB.ExecCommand(cmd.command, &pRes);
	////g_server.m_ActDB.PushCommand(cmd);
}

//void	GW_ActivityBase::SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer)
//{
//	//向数据库保存指定用户指定活动Info信息
//	//要查询的数据库表名=="User" + m_DBTableName
//	//根据m_iID值和sUserName在指定表中查找唯一记录，先删除后新建
//	//更新内容为sBuffer
//
//	//用户属性数据库里，一新表UserActivity，保存每个用户的每个活动的iEnterActive值
//	GWDBCommand cmd;
//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
//		"DELETE FROM `%s` WHERE `ActivityID` = '%d' AND `UserName` = '%s'",
//		GW_ActivityManager::c_sUserInfoTable, m_iID, sUserName.c_str());
//	g_server.m_ActDB.PushCommand(cmd);
//
//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
//		"INSERT INTO `%s` VALUES ('%d', '%s', '%s')",
//		GW_ActivityManager::c_sUserInfoTable, m_iID, sUserName.c_str(), sBuffer.c_str());
//	g_server.m_ActDB.PushCommand(cmd);
//
//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
//		"UPDATE `%s` SET `%d` = '%d' WHERE `UserName` = '%s'",
//		GW_ActivityManager::c_sUserEnterTable, m_iID, iEnterActive ? 1 : 0, sUserName.c_str());
//	g_server.m_ActDB.PushCommand(cmd);
//}

bool	GW_ActivityBase::GetSerializeString(std::string &sBuffer)
{
	if(!m_pActivityInfo)
		return false;
	m_pActivityInfo->SerializeToString(&sBuffer);
	if(sBuffer.empty())
		return false;
	return true;
}

//void	GW_ActivityBase::Dispose(CG_CmdPacket *pCommand)
//{
//	if(!pCommand)
//		return;
//
//	ACTIVITY_TYPE eType = ACTIVITY_TYPE_OVER;
//	pCommand->ReadShort(&eType);
//	switch(eType)
//	{
//	case ACTMSG_GETUSERINFO:
//		GetUserInfo(pCommand);
//		break;
//	case ACTMSG_SAVEUSERINFO:
//		SaveUserInfo(pCommand);
//		break;
//	}
//}

void	GW_ActivityBase::Startup(int iStartIndex)
{
	if(!m_pActivityInfo || iStartIndex < 0 || iStartIndex >= ActivityRes::iMaxEventCount)
		return;
	if(m_pActivityInfo->eState == AS_START)
		return;

	int iTemp = 0;
	m_pActivityInfo->eState = AS_START;
	int iCurDate[Activity::ciActTimeCount] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
	{
		m_bCanFore[iStartIndex][ii] = false;
		m_pActivityInfo->iUpdataTime[ii] = iCurDate[ii];
	}

	CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
	command.WriteShort(ACTMSG_START);
	command.WriteLong(m_iID);
	command.WriteLong(iStartIndex);
	g_server.m_region.BroadcastCmd();

	if(!m_sStartInfo[iStartIndex].empty())
	{
		CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
		command.WriteShort(ACTMSG_BROADCAST);
		command.WriteString(m_sStartInfo[iStartIndex].c_str());
		g_server.m_region.BroadcastCmd();
	}
}

void	GW_ActivityBase::Shutdown(int iOverIndex)
{
	if(iOverIndex < 0 || iOverIndex >= ActivityRes::iMaxEventCount)
		return;
	if(m_pActivityInfo->eState == AS_OVER)
		return;

	int iTemp = 0;
	m_pActivityInfo->eState = AS_OVER;
	int iCurDate[Activity::ciActTimeCount] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
	{
		m_bCanFore[iOverIndex][ii] = true;
		m_pActivityInfo->iUpdataTime[ii] = iCurDate[ii];
	}

	CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
	command.WriteShort(ACTMSG_OVER);
	command.WriteLong(m_iID);
	command.WriteLong(iOverIndex);
	g_server.m_region.BroadcastCmd();

	if(!m_sOverInfo[iOverIndex].empty())
	{
		CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
		command.WriteShort(ACTMSG_BROADCAST);
		command.WriteString(m_sOverInfo[iOverIndex].c_str());
		g_server.m_region.BroadcastCmd();
	}

	SetDatabaseActivity();
}

void	GW_ActivityBase::CycleTime(int iCycleIndex)
{
	if(iCycleIndex < 0 || iCycleIndex >= ActivityRes::iMaxEventCount)
		return;

	CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
	command.WriteShort(ACTMSG_CYCLE);
	command.WriteLong(m_iID);
	g_server.m_region.BroadcastCmd();

	if(!m_sCycleInfo[iCycleIndex].empty())
	{
		CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
		command.WriteShort(ACTMSG_BROADCAST);
		command.WriteString(m_sCycleInfo[iCycleIndex].c_str());
		g_server.m_region.BroadcastCmd();
	}
}

void	GW_ActivityBase::ForeTime(int iActIndex, int iForeIndex)
{
	if( iActIndex < 0 || iActIndex >= ActivityRes::iMaxEventCount ||
		iForeIndex < 0 || iForeIndex >= ActivityRes::iMaxEventCount ||
		m_sForeInfo[iActIndex].empty() )
		return;

	if(m_bCanFore[iActIndex][iForeIndex])
	{
		m_bCanFore[iActIndex][iForeIndex] = false;
		CG_CmdPacket& command = g_server.m_region.BeginCmd(g2r_activity_system);
		command.WriteShort(ACTMSG_BROADCAST);
		command.WriteString(m_sForeInfo[iActIndex].c_str());
		g_server.m_region.BroadcastCmd();
	}
}

//////////////////////////////////////////////////////////////////////////

GW_ActivityTelpher::GW_ActivityTelpher(int iID, ActivityRes *pRes) :
GW_ActivityBase(iID, pRes)
{
	m_DBTableName = "telpher";
	//GWDBCommand cmd;
	//rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
	//	"CREATE TABLE `%s` (ActivityID integer NOT NULL, ActName Varchar(255) NOT NULL, ActivityState Varchar(512))",
	//	m_DBTableName.c_str());
	//g_server.m_ActDB.PushCommand(cmd);

	m_eType = ACTIVITY_TELPHER;
	DEL_ONE(m_pActivityInfo);
	m_pActivityInfo = RT_NEW ActivityInfo(pRes ? pRes->iLevel : 1);
	m_pActivityInfo->eActivityType = m_eType;
	m_pActivityInfo->eState = AS_OVER;
	int iTemp = 0;
	int iCurDate[Activity::ciActTimeCount] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
		m_pActivityInfo->iUpdataTime[ii] = iCurDate[ii];
}

GW_ActivityTelpher::~GW_ActivityTelpher(void)
{
	//GW_ActivityBase::~GW_ActivityBase();
}

void	GW_ActivityTelpher::GetUserInfo(int iRegionID, CG_CmdPacket *pCommand)
{

}

void	GW_ActivityTelpher::SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer)
{

}

//////////////////////////////////////////////////////////////////////////

GW_ActivityTreasure::GW_ActivityTreasure(int iID, ActivityRes *pRes) :
GW_ActivityBase(iID, pRes),
m_UserStateTable("user_hold_treasure")
{
	m_DBTableName = "";
	m_eType = ACTIVITY_TREASURE;
	DEL_ONE(m_pActivityInfo);
	m_pActivityInfo = RT_NEW ActivityInfo(pRes ? pRes->iLevel : 1);
	m_pActivityInfo->eActivityType = m_eType;
	m_pActivityInfo->eState = AS_OVER;
	int iTemp = 0;
	int iCurDate[Activity::ciActTimeCount] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
		m_pActivityInfo->iUpdataTime[ii] = iCurDate[ii];
}

GW_ActivityTreasure::~GW_ActivityTreasure(void)
{

}

void	GW_ActivityTreasure::GetUserInfo(int iRegionID, CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	ACTIVITY_TYPE eType = ACTIVITY_TYPE_OVER;
	pCommand->ReadShort((short*)&eType);
	if(eType != m_eType)
		return;
	ACTIVE_STATE eState = AS_STATE_OVER;
	pCommand->ReadShort((short*)&eState);
	if(eState >= AS_STATE_OVER || eState <= AS_NONE)
		return;
	char *pName = NULL;
	pCommand->ReadString(&pName);
	if(!pName || pName[0] == 0)
		return;
	long lUserID = 0;
	pCommand->ReadLong(&lUserID);
	long lParam[5] = {0};
	for(int ii=0; ii<5; ii++)
		pCommand->ReadLong(&lParam[ii]);

	//向数据库查询指定用户的活动Info信息,数据库返回时调用GW_ActivityManager::SetUserInfo(...)
	//要查询的数据库表名==m_DBTableName
	GWDBCommand cmd;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"SELECT `UserName`, `ActivityType`, `JoinState`, `StateTime` FROM `%s` WHERE `UserID` = %d",
		m_UserStateTable.c_str(), lUserID);
	long *pResultParam = RT_NEW long[4+5];
	pResultParam[0] = iRegionID;
	pResultParam[1] = m_iID;
	pResultParam[2] = lUserID;
	pResultParam[3] = eState;
	for(int ii=0; ii<5; ii++)
		pResultParam[4+ii] = lParam[ii];
	cmd.data = pResultParam;
	cmd.succeeded = GW_ActivityTreasure::ReturnUserInfo;
	cmd.failed = GW_ActivityTreasure::ReturnUserInfo;
	cmd.type = dbCmdQuery;
	g_server.m_ActDB.PushCommand(cmd);
}

void	GW_ActivityTreasure::ReturnUserInfo(GWDBResult* pDBResult)
{
	if(!pDBResult || !pDBResult->data || !pDBResult->res)
	{
		DEL_ARRAY(pDBResult->data);
		return;
	}

	long iSession = *(long*)(pDBResult->data);
	long lActID = *((long*)(pDBResult->data) + 1);
	long lUserID = *((long*)(pDBResult->data) + 2);
	long lState = *((long*)(pDBResult->data) + 3);
	long lParam[5] = {0};
	for(int ii=0; ii<5; ii++)
		lParam[ii] = *((long*)(pDBResult->data) + 4 + ii);
	DEL_ARRAY(pDBResult->data);
	long lRowCount = mysql_num_rows(pDBResult->res);
	unsigned int iFieldCount = mysql_num_fields(pDBResult->res);
	if(lRowCount <= 0)
	{
		GWUser* pUser = g_server.FindActorById(lUserID);
		if(pUser)
		{
			GWDBCommand cmd;
			cmd.type = dbCmdQuery;
			rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
				"INSERT INTO `user_hold_treasure`(UserID, UserName, ActivityType, JoinState, StateTime) VALUES (%d, '%s', %d, %d, '0.0.0.0.0')",
				pUser->userID, pUser->name.c_str(), ACTIVITY_TREASURE, AS_OVER);
			g_server.m_ActDB.PushCommand(cmd);
			GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, "", lParam);
		}
		return;
	}
	//else if(lRowCount < 4)
	//	return;

	MYSQL_FIELD *field = mysql_fetch_fields(pDBResult->res);
	MYSQL_ROW row = mysql_fetch_row(pDBResult->res);
	UserActivityInfo info;
	info.eActivityType = (ACTIVITY_TYPE)atoi(row[1]);
	info.eState = (ACTIVE_STATE)atoi(row[2]);
	char *sTime = strtok(row[3], ".");
	for(int ii=0; ii<Activity::ciActTimeCount && sTime && sTime[0] != 0; ii++)
	{
		info.iUpdataTime[ii] = atoi(sTime);
		sTime = strtok(NULL, ".");
	}
	std::string sBuffer;
	info.SerializeToString(&sBuffer);
	GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, sBuffer, lParam);
}

void	GW_ActivityTreasure::SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer)
{
	GWUser* pUser = g_server.FindActorByName(sUserName.c_str());
	if(!pUser)
		return;

	UserActivityInfo userInfo;
	userInfo.SerializeToThis(&sBuffer);
	std::string sTime;
	char ss[10] = {0};
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
	{
		rt2_snprintf(ss, 10, "%d", userInfo.iUpdataTime[ii]);
		sTime += ss;
		if(ii < Activity::ciActTimeCount - 1)
			sTime += ".";
	}
	//不确定数据库中是否已经有了此活动用户记录，所以先删除1次
	//先查询判断的话，异步逻辑比较困难
	GWDBCommand cmd;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"DELETE FROM `user_hold_treasure` WHERE `UserID` = '%d'", pUser->userID);
	g_server.m_ActDB.PushCommand(cmd);

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"INSERT INTO `user_hold_treasure` (UserID, UserName, ActivityType, JoinState, StateTime) VALUES (%d, '%s', %d, %d, '%s')",
		pUser->userID, sUserName.c_str(), userInfo.eActivityType, userInfo.eState, sTime.c_str());
	g_server.m_ActDB.PushCommand(cmd);

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"UPDATE `%s` SET `%d` = '%d' WHERE `UserID` = '%d'",
		GW_ActivityManager::c_sUserEnterTable, m_eType, iEnterActive ? 1 : 0, pUser->userID);
	g_server.m_ActDB.PushCommand(cmd);
}

//////////////////////////////////////////////////////////////////////////

GW_ActivityCachot::GW_ActivityCachot(int iID, ActivityRes *pRes) :
GW_ActivityBase(iID, pRes),
m_UserStateTable("user_cachot")
{
	m_DBTableName = "";
	m_eType = ACTIVITY_CACHOT;
	DEL_ONE(m_pActivityInfo);
	m_pActivityInfo = RT_NEW ActivityCachot(pRes ? pRes->iLevel : 1, pRes ? pRes->iDegree : 0);
	m_pActivityInfo->eActivityType = m_eType;
	m_pActivityInfo->eState = AS_OVER;
	int iTemp = 0;
	int iCurDate[Activity::ciActTimeCount] = {0};
	rtSystemTime(iTemp, iCurDate[0], iCurDate[1], iCurDate[2], iCurDate[3], iCurDate[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
		m_pActivityInfo->iUpdataTime[ii] = iCurDate[ii];
}

GW_ActivityCachot::~GW_ActivityCachot(void)
{

}

void	GW_ActivityCachot::GetUserInfo(int iRegionID, CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	ACTIVITY_TYPE eType = ACTIVITY_TYPE_OVER;
	pCommand->ReadShort((short*)&eType);
	if(eType != m_eType)
		return;
	ACTIVE_STATE eState = AS_STATE_OVER;
	pCommand->ReadShort((short*)&eState);
	if(eState >= AS_STATE_OVER || eState <= AS_NONE)
		return;
	char *pName = NULL;
	pCommand->ReadString(&pName);
	if(!pName || pName[0] == 0)
		return;
	long lUserID = 0;
	pCommand->ReadLong(&lUserID);
	long lParam[5] = {0};
	for(int ii=0; ii<5; ii++)
		pCommand->ReadLong(&lParam[ii]);

	//向数据库查询指定用户的活动Info信息,数据库返回时调用GW_ActivityManager::SetUserInfo(...)
	//要查询的数据库表名==m_DBTableName
	GWDBCommand cmd;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"SELECT `UserName`, `ActivityType`, `JoinState`, `RunDegree`, `StateTime` FROM `%s` WHERE `UserID` = %d",
		m_UserStateTable.c_str(), lUserID);
	long *pResultParam = RT_NEW long[4+5];
	pResultParam[0] = iRegionID;
	pResultParam[1] = m_iID;
	pResultParam[2] = lUserID;
	pResultParam[3] = eState;
	for(int ii=0; ii<5; ii++)
		pResultParam[4+ii] = lParam[ii];
	cmd.data = pResultParam;
	cmd.succeeded = GW_ActivityCachot::ReturnUserInfo;
	cmd.failed = GW_ActivityCachot::ReturnUserInfo;
	cmd.type = dbCmdQuery;
	g_server.m_ActDB.PushCommand(cmd);
}

void	GW_ActivityCachot::ReturnUserInfo(GWDBResult* pDBResult)
{
	if(!pDBResult || !pDBResult->data || !pDBResult->res)
	{
		DEL_ARRAY(pDBResult->data);
		return;
	}

	long iSession = *(long*)(pDBResult->data);
	long lActID = *((long*)(pDBResult->data) + 1);
	long lUserID = *((long*)(pDBResult->data) + 2);
	long lState = *((long*)(pDBResult->data) + 3);
	long lParam[5] = {0};
	for(int ii=0; ii<5; ii++)
		lParam[ii] = *((long*)(pDBResult->data) + 4 + ii);
	DEL_ARRAY(pDBResult->data);
	long lRowCount = mysql_num_rows(pDBResult->res);
	unsigned int iFieldCount = mysql_num_fields(pDBResult->res);
	if(lRowCount <= 0)
	{
		GWUser* pUser = g_server.FindActorById(lUserID);
		if(pUser)
		{
			GWDBCommand cmd;
			cmd.type = dbCmdQuery;
			rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
				"INSERT INTO `user_cachot`(UserID, UserName, ActivityType, JoinState, RunDegree, StateTime) VALUES (%d, '%s', %d, %d, %d, '0.0.0.0.0')",
				pUser->userID, pUser->name.c_str(), ACTIVITY_CACHOT, AS_OVER, 0);
			g_server.m_ActDB.PushCommand(cmd);
			GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, "", lParam);
		}
		return;
	}
	//else if(lRowCount < 5)
	//	return;

	MYSQL_ROW row = mysql_fetch_row(pDBResult->res);
	UserCachotInfo info;
	info.eActivityType = (ACTIVITY_TYPE)atoi(row[1]);
	info.eState = (ACTIVE_STATE)atoi(row[2]);
	info.iDegree = atoi(row[3]);
	char *sTime = strtok(row[4], ".");
	for(int ii=0; ii<Activity::ciActTimeCount && sTime && sTime[0] != 0; ii++)
	{
		info.iUpdataTime[ii] = atoi(sTime);
		sTime = strtok(NULL, ".");
	}
	std::string sBuffer;
	info.SerializeToString(&sBuffer);
	GW_ActivityManager::GetSingle()->SetUserInfo(iSession, lActID, lUserID, lState, sBuffer, lParam);
}

void	GW_ActivityCachot::SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer)
{
	GWUser* pUser = g_server.FindActorByName(sUserName.c_str());
	if(!pUser)
		return;

	UserCachotInfo userInfo;
	userInfo.SerializeToThis(&sBuffer);
	std::string sTime;
	char ss[10] = {0};
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
	{
		rt2_snprintf(ss, 10, "%d", userInfo.iUpdataTime[ii]);
		sTime += ss;
		if(ii < Activity::ciActTimeCount - 1)
			sTime += ".";
	}
	//不确定数据库中是否已经有了此活动用户记录，所以先删除1次
	//先查询判断的话，异步逻辑比较困难
	GWDBCommand cmd;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"DELETE FROM `%s` WHERE `UserID` = '%d'", m_UserStateTable.c_str(), pUser->userID);
	g_server.m_ActDB.PushCommand(cmd);

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"INSERT INTO `%s` (UserID, UserName, ActivityType, JoinState, RunDegree, StateTime) VALUES (%d, '%s', %d, %d, %d, '%s')",
		m_UserStateTable.c_str(), pUser->userID, sUserName.c_str(), userInfo.eActivityType, userInfo.eState, userInfo.iDegree, sTime.c_str());
	g_server.m_ActDB.PushCommand(cmd);

	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH,
		"UPDATE `%s` SET `%d` = '%d' WHERE `UserID` = '%d'",
		GW_ActivityManager::c_sUserEnterTable, m_eType, iEnterActive ? 1 : 0, pUser->userID);
	g_server.m_ActDB.PushCommand(cmd);
}

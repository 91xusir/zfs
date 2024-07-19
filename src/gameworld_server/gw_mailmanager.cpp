#include "gw_server.h"


bool GWMailManager::AddMail(int receiverID,
						 int senderID,
						 const string& title,
						 const string& content, 
						 bool notify,
						 const string& StrItemID,
						 long Money)
{
	// senderID为0时是系统邮件

    // 系统不接收任何邮件
    if (receiverID==0)
    {
        return true;
    }
	
	long IsOpen = 0;

	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "SELECT COUNT(*) FROM mail WHERE ReceiverID=%d", receiverID);

	MYSQL_RES* res;

	if (!g_server.m_database.ExecCommand(cmd.command, &res))
		return false;

	MYSQL_ROW row = mysql_fetch_row(res);	

	int count = atol(row[0]);

	mysql_free_result(res);	

	if (count >= GW_MAX_MAIL && senderID!=0)
	{
		if (notify)
			return AddMail(senderID, 0, R(MSG_MAIL_SERVER_FULL), content, false,StrItemID,Money);
		else
			return false;
	}

	cmd.type = dbCmdInsert;
	if (notify)
	{
		cmd.succeeded = (GWDBCallback)OnMailSent;
		cmd.failed = (GWDBCallback)OnMailSent;
	}
	else
	{
		cmd.succeeded = NULL;
		cmd.failed = NULL;
	}
	cmd.data = (void*)receiverID;

	string ttitle, tcontent;
	g_server.m_database.EscapeString(ttitle, title);
	g_server.m_database.EscapeString(tcontent, content);

	if (senderID!=0)
	{
		// 普通邮件
		rt2_sprintf(cmd.command, "INSERT INTO mail SELECT "
			"'',"
			"%d,"
			"%d,"
			"'%s',"
			"%d,"
			"%d,"
			"NOW(),"
			"'%s',"
			"'%s',"
			"t1.Name,"
			"t2.Name "
			"FROM user as t1, user as t2 "
			"WHERE t1.ID=%d AND t2.ID=%d",
			receiverID,
			senderID,
			StrItemID.c_str(),
			Money,
			IsOpen,
			ttitle.c_str(),
			tcontent.c_str(),
			receiverID,
			senderID);
	}
	else 
	{
		// 系统邮件
		rt2_sprintf(cmd.command, "INSERT INTO mail SELECT "
			"'',"
			"%d,"
			"%d,"
			"'%s',"
			"%d,"
			"%d,"
			"NOW(),"
			"'%s',"
			"'%s',"
			"t1.Name,"
			"'%s' "
			"FROM user as t1 "
			"WHERE t1.ID=%d",
			receiverID,
			senderID,
			StrItemID.c_str(),
			Money,
			IsOpen,
			ttitle.c_str(),
			tcontent.c_str(),
			R(MAIL_ADMIN),
			receiverID);

		/*Tianh 系统给玩家发送邮件后统计到LOG*/
		if (!g_server.LogUserEvent(PLT_SystemMessages, receiverID, "", 0, ttitle.c_str(), 0, tcontent.c_str(), 0, ""))
		{
			return eError;
		}
	}

	g_server.m_database.PushCommand(cmd);

	return true;
}

bool  GWMailManager::Update(short IsOpen,long MailID)
{
	if (MailID<0)
	{
		return false;
	}

	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "UPDATE mail SET IsOpen=%d WHERE ID=%d",
		IsOpen,
		MailID);

	cmd.data = NULL;
	cmd.failed = NULL;
	cmd.succeeded = NULL;
	cmd.type = dbCmdUpdate;

	g_server.m_database.PushCommand(cmd);
}

bool GWMailManager::MoveItem(long MailID)
{
	if (MailID<0)
	{
		return false;
	}
	
	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "SELECT "
		"ReceiverID,"	
		"StrItemID,"
		"Money "
		"FROM mail WHERE ID=%d",
		MailID);

	cmd.data = NULL;
	cmd.failed = NULL;
	cmd.succeeded = OnMailItem;
	cmd.type = dbCmdQuery;

	g_server.m_database.PushCommand(cmd);

	UpDateItemInfo(MailID);

}

bool GWMailManager::UpDateItemInfo(long MailID)
{
	if (MailID<0)
	{
		return false;
	}

	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "UPDATE mail SET StrItemID='%s',Money=%d WHERE ID=%d","",0,
		MailID);

	cmd.data = NULL;
	cmd.failed = NULL;
	cmd.succeeded = NULL;
	cmd.type = dbCmdUpdate;

	g_server.m_database.PushCommand(cmd);
}

void GWMailManager::OnMailItem(GWDBResult* result)
{
	int numRows = mysql_num_rows(result->res);

	if (numRows == 0)
		return;

	MYSQL_ROW row = mysql_fetch_row(result->res);
	
	long ReceiverID = atol(row[0]);

	GWUser* user = g_server.FindActorById(ReceiverID);
	if (user == NULL)
		return;

	int rgIdx = g_server.m_regionTable[user->regionID].sessionIndex;

	if (rgIdx == -1)
		return;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_Item_Mail);

	cmd.WriteLong(atol(row[0]));    
	cmd.WriteString(row[1]);	
	cmd.WriteLong(atol(row[2]));

	g_server.m_region.SendToPeer(rgIdx);
}



void GWMailManager::OnMailSent(GWDBResult* result)
{
	if (result->okay)
	{
		if (result->affectRows > 0)
		{
			g_server.m_mail.CheckMail((int)result->data);
		}
		else
		{
		}
	}
	else
	{
        MSG("Add Mail error.\n");
	}
}

void GWMailManager::RemoveMail(long MailID)
{
	GWDBCommand cmd;

	rt2_sprintf(cmd.command, "DELETE FROM mail WHERE ID=%d",MailID);

	cmd.data = NULL;
	cmd.failed = NULL;
	cmd.succeeded = NULL;
	cmd.type = dbCmdDelete;

	g_server.m_database.PushCommand(cmd);
}


void GWMailManager::CheckMail(int userID)
{
	GWUser *user = g_server.FindActorById(userID);
	if(!user) return;

	GWLoginClient *lc = g_server.FindLoginClientByActorId(userID);
	if (!lc || lc->GetState()==LCS_SELECT_CHAR)
		return;

	GWDBCommand cmd;
	if (user->lastMailDate.empty())
	{
		rt2_sprintf(cmd.command, "SELECT "
			"ID,"
			"SenderID,"	
			"SenderName,"
			"StrItemID,"
			"Money,"
			"ReceivedDate,"
			"Title,"
			"Content,"
			"IsOpen "
			"FROM mail WHERE ReceiverID=%d ORDER BY ReceivedDate DESC",
			userID);
	}
	else
	{
		rt2_sprintf(cmd.command, "SELECT "
			"ID,"
			"SenderID,"	
			"SenderName,"
			"StrItemID,"
			"Money,"
			"ReceivedDate,"
			"Title,"
			"Content,"
			"IsOpen "
			"FROM mail WHERE ReceiverID=%d AND ReceivedDate>'%s' ORDER BY ReceivedDate DESC",
			userID,
			user->lastMailDate.c_str());
	}

	cmd.type = dbCmdQuery;
	cmd.failed = NULL;
	cmd.succeeded = OnMailChecked;
	cmd.data = (void*)userID;

	g_server.m_database.PushCommand(cmd);
}


void GWMailManager::OnMailChecked(GWDBResult* result)
{
	int numRows = mysql_num_rows(result->res);

	if (numRows == 0)
		return;

	int userID = (int)result->data;

	GWUser* user = g_server.FindActorById(userID);
	if (user == NULL)
		return;

	//if (user->state != usBusy)
	//	return;

	int rgIdx = g_server.m_regionTable[user->regionID].sessionIndex;

	if (rgIdx == -1)
		return;

	MYSQL_ROW row = mysql_fetch_row(result->res);

	user->lastMailDate = row[4];

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_mail);
	cmd.WriteLong(userID);
	cmd.WriteString(row[4]);
	cmd.WriteLong(numRows);

	for (int n = 0; n < numRows; n++)
	{
		cmd.WriteLong(atol(row[0]));    // ID
		cmd.WriteLong(atol(row[1]));	// SenderID
		cmd.WriteString(row[2]);		// SenderName
		cmd.WriteString(row[3]);		// Iten info
		cmd.WriteLong(atol(row[4]));    // Money
		cmd.WriteString(row[5]);		// ReceivedDate
		cmd.WriteString(row[6]);		// Title
		cmd.WriteString(row[7]);		// Content 
		cmd.WriteLong(atol(row[8]));		    // IsOpen

		row = mysql_fetch_row(result->res);
	}

	g_server.m_region.SendToPeer(rgIdx);
}


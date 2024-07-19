//-----------------------------------------------------------------------------
//created by rendh 2010.01.19
//-----------------------------------------------------------------------------
#include "region.h"
#include "linesys_net_define.h"
#include "rs_cmd_LineSys.h"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define CHECK_READ(p)		if(!(p)) return CMD_ERROR_READ_DATA;
#define CHECK_ASSERT(p)		if(!(p)) return CMD_ERROR_UNKNOWN;
#define CHECK_NULL(p)		if(!(p)) return CMD_ERROR_UNKNOWN;

//----------------------------
//客户端消息与对应处理函数表 
//----------------------------
const STLINESYS LineSysCmdProcTbl[] = 
{
	{c2r_linesys_SwitchLineReq,		  cmd_c2r_linesys_SwitchLineReq},

	{c2r_linesys_LineInfoReq,		  cmd_c2r_linesys_LineInfoReq}

};

//-----------------------------------
//GW消息与对应处理函数表
//一般情况下GW收到消息执行存档后响应
//-----------------------------------
const STLINESYS_GW LineSysCmdProcTbl_GW[] = 
{
	{g2r_linesys_CreateLineReq,		cmd_g2r_linesys_CreateLineReq},

	{g2r_linesys_EnterLineReq,		cmd_g2r_linesys_EnterLineReq},

	{g2r_linesys_EnterLineRes,		cmd_g2r_linesys_EnterLineRes},

	{g2r_linesys_SwitchLineReq,		cmd_g2r_linesys_SwitchLineReq},
	
	{g2r_linesys_SwitchLineRes,		cmd_g2r_linesys_SwitchLineRes},

	{g2r_linesys_LeaveLineReq,		cmd_g2r_linesys_LeaveLineReq},

	{g2r_linesys_LineInfoRes,		cmd_g2r_linesys_LineInfoRes}

};

//---------------------------------------
//客户端消息匹配函数
//---------------------------------------
int LineSysCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//参数检查
	short cmd_type;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(LineSysCmdProcTbl); i++)
	{
		if(cmd_type == LineSysCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(LineSysCmdProcTbl))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return LineSysCmdProcTbl[i].cmd_func(CmdGiver, cmd);
}

//---------------------------------------
//GW消息匹配函数
//---------------------------------------
int LineSysCmdProcess_FromGW(CG_CmdPacket *cmd)
{	
	//参数检查
	short cmd_type;

	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(LineSysCmdProcTbl_GW); i++)
	{
		if(cmd_type == LineSysCmdProcTbl_GW[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(LineSysCmdProcTbl_GW))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return LineSysCmdProcTbl_GW[i].cmd_func(cmd);
}

//------------------------------------------------------------------------------------------
//以下处理来自客户端消息
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
#define CHECK_IS_USER(cre)		if(!cre->IsUser())	return CMD_ERROR_UNKNOWN;

//------------------------------------------------------------------------------------------

int	cmd_c2r_linesys_SwitchLineReq(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{

	
	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	//规则检查
	CHECK_IS_USER(CmdGiver)

	long dwGroupID, dwNewLineID, dwOldLineID;
	dwGroupID = dwNewLineID = dwOldLineID = 0;
	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwNewLineID))
	CHECK_READ(cmd->ReadLong(&dwOldLineID))
	
	CLineSysCmdSend_R2G::Send_r2g_linesys_SwitchLineReq(dwGroupID, dwNewLineID, dwOldLineID, CmdGiver->m_userInfo->m_userId);


	return CMD_ERROR_NONE;
}

int	cmd_c2r_linesys_LineInfoReq(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	//规则检查
	CHECK_IS_USER(CmdGiver)
	
	long dwGroupID, dwLineID;
	dwGroupID = dwLineID = 0;

	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwLineID))

	CLineSysCmdSend_R2G::Send_r2g_linesys_LineInfoReq(dwGroupID, dwLineID, CmdGiver->m_userInfo->m_userId);

	return CMD_ERROR_NONE;
}

//------------------------------------------------------------------------
//以下处理来自GW消息或产生通知型消息
//------------------------------------------------------------------------
int cmd_g2r_linesys_CreateLineReq(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwRegionCount = 0;

	CHECK_READ(cmd->ReadLong(&dwRegionCount))

	if (!g_region->m_rsLineMan.CreateLine(dwRegionCount))
	{
		CLineSysCmdSend_R2G::Send_r2g_linesys_CreateLineRes(-1);

		return CMD_ERROR_UNKNOWN;
	}

	CLineSysCmdSend_R2G::Send_r2g_linesys_CreateLineRes(g_cfgRs.lRegionID);
	
	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_EnterLineReq(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwGroupID, dwLineID, dwUserID, iRegionID, pos[3];
	dwGroupID = dwLineID = dwUserID = pos[0] = pos[1] = pos[2] = 0;
	iRegionID = -1;
	
	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwLineID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadLong(&iRegionID))
	CHECK_READ(cmd->ReadData(pos, sizeof(pos)))

	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (!g_region->m_rsLineMan.EnterLine(dwGroupID, dwLineID, iRegionID, pUser, pos))
	{
		return CMD_ERROR_UNKNOWN;
	}

	CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineRes(dwGroupID, dwLineID, dwUserID, pUser);

	CLineSysCmdBuilder_R2C::Build_r2c_linesys_EnterLineNotify(dwGroupID, dwLineID, dwUserID, ERR_NULL);
	pUser->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_EnterLineRes(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwGroupID, dwUserID, dwError;
	dwGroupID = dwUserID = dwError = 0;

	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadLong(&dwError))


	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (pUser)
	{
		CLineSysCmdBuilder_R2C::Build_r2c_linesys_EnterLineNotify(dwGroupID, 0, dwUserID, dwError);
		pUser->RecvCmd(g_sendCmd);
	}else
	{
		return CMD_ERROR_UNKNOWN;
	}

	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_SwitchLineReq(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwGroupID, dwOldLineID, dwNewLineID, dwUserID, iRegionID;
	dwGroupID = dwNewLineID = dwOldLineID = dwUserID = 0;
	iRegionID = -1;

	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwOldLineID))
	CHECK_READ(cmd->ReadLong(&dwNewLineID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadLong(&iRegionID))

	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (!g_region->m_rsLineMan.SwitchLine(dwGroupID, dwOldLineID, dwNewLineID, iRegionID, pUser))
	{
		return CMD_ERROR_UNKNOWN;
	}

	CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineRes(dwGroupID, dwNewLineID, dwUserID, pUser);

	CLineSysCmdBuilder_R2C::Build_r2c_linesys_EnterLineNotify(dwGroupID, dwNewLineID, dwUserID, ERR_NULL);
	pUser->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_SwitchLineRes(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwGroupID, dwLineID, dwUserID, dwError;
	dwGroupID = dwLineID = dwUserID = dwError = 0;

	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwLineID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadLong(&dwError))


	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (pUser)
	{
		CLineSysCmdBuilder_R2C::Build_r2c_linesys_SwitchLineRes(dwGroupID, dwLineID, dwUserID, dwError);
		pUser->RecvCmd(g_sendCmd);
	}else
	{
		return CMD_ERROR_UNKNOWN;
	}
	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_LeaveLineReq(CG_CmdPacket *cmd)
{	
	CHECK_NULL(cmd)

	long dwGroupID, dwLineID, dwUserID, pos[3];
	dwGroupID = dwLineID = dwUserID = 0;
	memset(pos, 0, sizeof(pos));
	char* pstrmap = NULL;


	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwLineID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadString(&pstrmap))
	CHECK_READ(cmd->ReadData(pos, sizeof(pos)))

	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (!g_region->m_rsLineMan.LeaveLine(dwGroupID, dwLineID, pUser, pstrmap, pos))
	{
		return CMD_ERROR_UNKNOWN;
	}

	CLineSysCmdBuilder_R2C::Build_r2c_linesys_LeaveLineNotify(dwGroupID, dwLineID, dwUserID, ERR_NULL);
	pUser->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int cmd_g2r_linesys_LineInfoRes(CG_CmdPacket *cmd)
{
	CHECK_NULL(cmd)

	long dwGroupID, dwLineID, dwUserID, dwLineInfoCount;
	dwGroupID = dwLineID = dwUserID = dwLineInfoCount = 0;

	CHECK_READ(cmd->ReadLong(&dwGroupID))
	CHECK_READ(cmd->ReadLong(&dwLineID))
	CHECK_READ(cmd->ReadLong(&dwUserID))
	CHECK_READ(cmd->ReadLong(&dwLineInfoCount))

	list<SLineInfo> listLineInfo;
	SLineInfo stLineInfo;

	for (int i = 0; i <  dwLineInfoCount; i++)
	{
		CHECK_READ(cmd->ReadData(&stLineInfo, sizeof(SLineInfo)))
		listLineInfo.push_back(stLineInfo);
	}

	CRegionUser* pUser = g_region->FindUser(dwUserID);

	if (pUser)
	{
		CLineSysCmdBuilder_R2C::Build_r2c_linesys_LineInfoRes(dwGroupID, dwLineID, dwUserID, &listLineInfo);
		pUser->RecvCmd(g_sendCmd);
	}else
	{
		return CMD_ERROR_UNKNOWN;
	}

	return CMD_ERROR_NONE;
}



//------------------------------------------
//以下是R2C消息建构
//------------------------------------------

void CLineSysCmdBuilder_R2C::Build_R2C_Cmd_Begin()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_linesys);
}

void CLineSysCmdBuilder_R2C::Build_r2c_linesys_SwitchLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError)
{
	CLineSysCmdBuilder_R2C::Build_R2C_Cmd_Begin();
	g_sendCmd->WriteShort(r2c_linesys_SwitchLineRes);
	g_sendCmd->WriteLong(dwGroupID);
	g_sendCmd->WriteLong(dwLineID);
	g_sendCmd->WriteLong(dwUserID);
	g_sendCmd->WriteLong(dwError);
}

void CLineSysCmdBuilder_R2C::Build_r2c_linesys_LineInfoRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, list<SLineInfo>* plistLineInfo)
{
	CLineSysCmdBuilder_R2C::Build_R2C_Cmd_Begin();
	g_sendCmd->WriteShort(r2c_linesys_LineInfoRes);
	g_sendCmd->WriteLong(dwGroupID);
	g_sendCmd->WriteLong(dwLineID);
	g_sendCmd->WriteLong(dwUserID);
	g_sendCmd->WriteLong(plistLineInfo->size());

	std::list<SLineInfo>::iterator it;

	for (it = plistLineInfo->begin(); it != plistLineInfo->end(); it++)
	{
		g_sendCmd->WriteData(&(*it), sizeof(SLineInfo));
	}
}




void CLineSysCmdBuilder_R2C::Build_r2c_linesys_EnterLineNotify(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError)
{
	CLineSysCmdBuilder_R2C::Build_R2C_Cmd_Begin();
	g_sendCmd->WriteShort(r2c_linesys_EnterLineNotify);
	g_sendCmd->WriteLong(dwGroupID);
	g_sendCmd->WriteLong(dwLineID);
	g_sendCmd->WriteLong(dwUserID);
	g_sendCmd->WriteLong(dwError);
}

void CLineSysCmdBuilder_R2C::Build_r2c_linesys_LeaveLineNotify(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError)
{
	CLineSysCmdBuilder_R2C::Build_R2C_Cmd_Begin();
	g_sendCmd->WriteShort(r2c_linesys_LeaveLineNotify);
	g_sendCmd->WriteLong(dwGroupID);
	g_sendCmd->WriteLong(dwLineID);
	g_sendCmd->WriteLong(dwUserID);
	g_sendCmd->WriteLong(dwError);
}

//------------------------------------------
//以下是R2G消息建构发送
//------------------------------------------
void CLineSysCmdSend_R2G::Send_r2g_linesys_CreateLineRes(int iRegionID)
{

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_CreateLineRes);
	packet.WriteLong(iRegionID);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(DWORD dwGroupID, DWORD dwUserID, long* ppos)
{	
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_EnterLineReq);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwUserID);
	packet.WriteData(ppos, 12);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, CRegionUser* pUser)
{	
	if (!pUser)
	{
		return;
	}
	
	pUser->m_dummy->m_dwGroupID = dwGroupID;
	pUser->m_dummy->m_dwLineID = dwLineID;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_EnterLineRes);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwLineID);
	packet.WriteLong(dwUserID);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_SwitchLineReq(DWORD dwGroupID, DWORD dwNewLineID, DWORD dwOldLineID, DWORD dwUserID)
{
	
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_SwitchLineReq);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwOldLineID);
	packet.WriteLong(dwNewLineID);
	packet.WriteLong(dwUserID);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, const char* pstrmap, long* ppos)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_LeaveLineReq);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwLineID);
	packet.WriteLong(dwUserID);
	packet.WriteString(pstrmap);
	packet.WriteData(ppos, 12);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, CRegionUser* pUser)
{
	if (!pUser)
	{
		return;
	}

	pUser->m_dummy->m_dwGroupID = 0;
	pUser->m_dummy->m_dwLineID = 0;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_LeaveLineRes);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwLineID);
	packet.WriteLong(dwUserID);
	g_region->m_gws.EndSend();
}

void CLineSysCmdSend_R2G::Send_r2g_linesys_LineInfoReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_linesys);
	packet.WriteShort(r2g_linesys_LineInfoReq);
	packet.WriteLong(dwGroupID);
	packet.WriteLong(dwLineID);
	packet.WriteLong(dwUserID);
	g_region->m_gws.EndSend();
}


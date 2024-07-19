//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------

#include "gw_server.h"
#include "gw_cmd_LineSys.h"
#include "linesys_net_define.h"
//----------------------------
//region消息与对应处理函数表
//----------------------------


const STLINESYS GW_LineSysCmdProcTbl[] = 
{

	{r2g_linesys_CreateLineRes,		cmd_r2g_LineSys_CreateLineRes},

	{r2g_linesys_EnterLineReq,		cmd_r2g_LineSys_EnterLineReq},
	
	{r2g_linesys_EnterLineRes,		cmd_r2g_LineSys_EnterLineRes},
	
	{r2g_linesys_SwitchLineReq,		cmd_r2g_LineSys_SwitchLineReq},
	
	{r2g_linesys_LeaveLineReq,		cmd_r2g_LineSys_LeaveLineReq},
	
	{r2g_linesys_LeaveLineRes,		cmd_r2g_LineSys_LeaveLineRes},

	{r2g_linesys_LineInfoReq,		cmd_r2g_LineSys_LineInfoReq}
};

//---------------------------------------
//REGION消息匹配函数
//---------------------------------------


int GW_LineSysCmdProcess(CG_CmdPacket *cmd)
{
	short cmd_type;
	
	SAFE_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(i = 0; i < NUM(GW_LineSysCmdProcTbl); i++)
	{
		if(cmd_type == GW_LineSysCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(GW_LineSysCmdProcTbl))
	{
		return eError;
	}

	return GW_LineSysCmdProcTbl[i].cmd_func(cmd);
}

//----------------------------
//以下处理来自REGION消息
//----------------------------

int	cmd_r2g_LineSys_CreateLineRes			(CG_CmdPacket *cmd)
{
	guard;

	SAFE_ASSERT(cmd)

	long iRegionID = -1;

	SAFE_READ(cmd->ReadLong(&iRegionID))

	if (iRegionID == -1)
	{
		return eError;
	}
	
	g_server.m_LineMan.OnCreateLineRes(iRegionID);

	return eOkay;

	unguard;
}

int	cmd_r2g_LineSys_EnterLineReq(CG_CmdPacket *cmd)
{
	guard;
	
	SAFE_ASSERT(cmd)

	long dwGroupID = 0;
	long dwUID = 0;
	DWORD dwLineID = 0;
	int iRegionID = -1;
	long pos[3];
	memset(pos, 0, sizeof(pos));

	int len = sizeof(pos);
	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwUID))
	SAFE_READ(cmd->ReadData(pos, len))

	if(!g_server.m_LineMan.EnterLineReq(dwGroupID, dwUID, dwLineID, iRegionID))
	{
		CLineSysCmdHelper_GW::Send_g2r_LineSys_EnterLineRes(dwGroupID, dwLineID, ERR_FULL);
		return eError;
	}

	CLineSysCmdHelper_GW::Send_g2r_LineSys_EnterLineReq(dwGroupID, dwLineID, dwUID, iRegionID, pos);
	
	return eOkay;
	
	unguard;
}

int	cmd_r2g_LineSys_EnterLineRes(CG_CmdPacket *cmd)
{
	guard;

	SAFE_ASSERT(cmd)

	long dwGroupID = 0;
	long dwLineID = 0;
	long dwUID = 0;
	
	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwLineID))
	SAFE_READ(cmd->ReadLong(&dwUID))

	if (!g_server.m_LineMan.OnEnterLineRes(dwGroupID, dwUID))
	{
		return eError;
	}

	GWUser *user = g_server.FindActorById(dwUID);

	if(user)
	{
		user->LineSYSGroupID = dwGroupID;
		user->LineSYSLineID = dwLineID;
	}else
	{
		ERR1("EnterLineRes error: Cannot found user ID %d\n", dwUID);
	}

	return eOkay;

	unguard;
}

int	cmd_r2g_LineSys_SwitchLineReq(CG_CmdPacket *cmd)
{
	guard;

	SAFE_ASSERT(cmd)

	long dwGroupID = 0;
	long dwUID = 0;
	long dwOldLineID = 0;
	long dwNewLineID = 0;
	int iRegionID = -1;

	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwOldLineID))
	SAFE_READ(cmd->ReadLong(&dwNewLineID))
	SAFE_READ(cmd->ReadLong(&dwUID))

	if (!g_server.m_LineMan.SwitchLineReq(dwGroupID, dwOldLineID, dwNewLineID, dwUID, iRegionID))
	{
		CLineSysCmdHelper_GW::Send_g2r_LineSys_SwitchLineRes(dwGroupID, dwOldLineID, dwUID, ERR_FULL);
		return eError;
	}

	CLineSysCmdHelper_GW::Send_g2r_LineSys_SwitchLineReq(dwGroupID, dwOldLineID, dwNewLineID, dwUID, iRegionID);
	return eOkay;

	unguard;
}

int	cmd_r2g_LineSys_LeaveLineReq(CG_CmdPacket *cmd)
{
	guard;	
	
	SAFE_ASSERT(cmd)

	long dwGroupID = 0;
	long dwUID = 0;
	long dwLineID = 0;
	char* pstrmap = NULL;
	long pos[3];
	memset(pos, 0, sizeof(pos));

	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwLineID))
	SAFE_READ(cmd->ReadLong(&dwUID))
	SAFE_READ(cmd->ReadString(&pstrmap))
	SAFE_READ(cmd->ReadData(pos, sizeof(pos)))

	if (!g_server.m_LineMan.LeaveLineReq(dwGroupID, dwLineID, dwUID))
	{
		return eError;
	}

	CLineSysCmdHelper_GW::Send_g2r_LineSys_LeaveLineReq(dwGroupID, dwLineID, dwUID, pstrmap, pos);
	return eOkay;

		
	unguard;
}

int	cmd_r2g_LineSys_LeaveLineRes(CG_CmdPacket *cmd)
{
	guard;	
	
	SAFE_ASSERT(cmd)

	long dwGroupID = 0;
	long dwUID = 0;
	long dwLineID = 0;

	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwLineID))
	SAFE_READ(cmd->ReadLong(&dwUID))

	if (!g_server.m_LineMan.OnLeaveLineRes(dwGroupID, dwLineID, dwUID))
	{
		return eError;
	}

	return eOkay;
		
	unguard;
}

int	cmd_r2g_LineSys_LineInfoReq(CG_CmdPacket *cmd)
{
	guard;	
	
	SAFE_ASSERT(cmd)

	long dwGroupID, dwUID, dwLineID;
	dwGroupID = dwUID = dwLineID = 0;

	std::list<gw_LineGroup::SLineInfo> listLineInfo;

	SAFE_READ(cmd->ReadLong(&dwGroupID))
	SAFE_READ(cmd->ReadLong(&dwLineID))
	SAFE_READ(cmd->ReadLong(&dwUID))

	if (!g_server.m_LineMan.LineInfoReq(dwGroupID, listLineInfo))
	{
		return eError;
	}

	CLineSysCmdHelper_GW::Send_g2r_LineSys_LineInfoRes(dwGroupID, dwLineID, dwUID, &listLineInfo);
	return eOkay;
		
	unguard;
}



void CLineSysCmdHelper_GW::Send_g2r_LineSys_CreateLineReq(DWORD dwRegionCount)
{
	guard;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_CreateLineReq);
	cmd.WriteLong(dwRegionCount);

	g_server.m_region.SendCmd();

	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_EnterLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, int iRegionID, long* ppos)
{
	guard;
			
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_EnterLineReq);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwLineID);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(iRegionID);
	cmd.WriteData(ppos, 12);

	g_server.m_region.SendCmd();
	
	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_EnterLineRes(DWORD dwGroupID, DWORD dwUserID, DWORD dwError)
{
	guard;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_EnterLineRes);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(dwError);

	g_server.m_region.SendCmd();

	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_SwitchLineReq(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int iRegionID)
{
	guard;
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_SwitchLineReq);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwOldLineID);
	cmd.WriteLong(dwNewLineID);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(iRegionID);

	g_server.m_region.SendCmd();
	
	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_SwitchLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError)
{	
	guard;
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_SwitchLineRes);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwLineID);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(dwError);

	g_server.m_region.SendCmd();
	
	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, char* pstrmap, long* ppos)
{	
	guard;
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_LeaveLineReq);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwLineID);
	cmd.WriteLong(dwUserID);
	cmd.WriteString(pstrmap);
	cmd.WriteData(ppos, 12);

	g_server.m_region.SendCmd();
		
	unguard;
}

void CLineSysCmdHelper_GW::Send_g2r_LineSys_LineInfoRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, std::list<gw_LineGroup::SLineInfo>* plistLineInfo)
{	
	guard;
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_linesys);
	cmd.WriteShort(g2r_linesys_LineInfoRes);
	cmd.WriteLong(dwGroupID);
	cmd.WriteLong(dwLineID);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong((long)plistLineInfo->size());
	
	std::list<gw_LineGroup::SLineInfo>::iterator it;

	for (it = plistLineInfo->begin(); it != plistLineInfo->end(); it++)
	{
		cmd.WriteData(&(*it), sizeof(gw_LineGroup::SLineInfo));
	}

	g_server.m_region.SendCmd();
		
	unguard;
}


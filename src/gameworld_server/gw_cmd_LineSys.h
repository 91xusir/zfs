//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#pragma once


typedef int (*GW_LINESYS_CMD_PFUNC)(CG_CmdPacket *cmd);
typedef struct {
	short					cmd_type;
	GW_LINESYS_CMD_PFUNC	cmd_func;
}STLINESYS;

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define SAFE_READ(p)	{ if(!(p)) return eInvalidData; }
#define SAFE_ASSERT(p)  { if(!(p)) return eError; }

//------------------------------
//region消息匹配函数
//------------------------------
int GW_LineSysCmdProcess(CG_CmdPacket *cmd);

//-------------------------------
//region消息处理函数
//-------------------------------
int	cmd_r2g_LineSys_CreateLineRes			(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_EnterLineReq		(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_EnterLineRes		(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_SwitchLineReq			(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_LeaveLineReq			(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_LeaveLineRes			(CG_CmdPacket *cmd);
int	cmd_r2g_LineSys_LineInfoReq			(CG_CmdPacket *cmd);

class CLineSysCmdHelper_GW
{
public:
	static void Send_g2r_LineSys_CreateLineReq(DWORD dwRegionCount);
	static void Send_g2r_LineSys_EnterLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, int iRegionID, long* ppos);
	static void Send_g2r_LineSys_EnterLineRes(DWORD dwGroupID, DWORD dwUserID, DWORD dwError);
	static void Send_g2r_LineSys_SwitchLineReq(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int iRegionID);
	static void Send_g2r_LineSys_SwitchLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError);
	static void Send_g2r_LineSys_LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, char* pstrmap, long* ppos);
	static void Send_g2r_LineSys_LineInfoRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, std::list<gw_LineGroup::SLineInfo>* plistLineInfo);	
};



//-----------------------------------------------------------------------------
//created by rendh 2010.01.19
//-----------------------------------------------------------------------------
#pragma once


typedef int (*LINESYS_CMD_PFUNC)(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
typedef int (*LINESYS_CMD_PFUNC_GW)(CG_CmdPacket *cmd);
typedef struct{
	short						cmd_type;
	LINESYS_CMD_PFUNC			cmd_func;
}STLINESYS;
typedef struct{
	short						cmd_type;
	LINESYS_CMD_PFUNC_GW		cmd_func;
}STLINESYS_GW;

struct SLineInfo
{
	DWORD dwLineID;
	DWORD dwUserCount;
	DWORD dwMaxUserCount;
};

//------------------------
//匹配处理函数
//------------------------
int LineSysCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
int LineSysCmdProcess_FromGW(CG_CmdPacket *cmd);

//--------------------------------------
//处理客户端消息函数声明
//消息定义文件：linesys_net_define.h
//--------------------------------------
//----------------------------------------------------------------------------------------
int	cmd_c2r_linesys_SwitchLineReq		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_linesys_LineInfoReq			  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
//----------------------------------------------------------------------------------------

//--------------------------------------
//处理GW消息函数声明/生成反馈消息
//消息定义文件：linesys_net_define.h
//--------------------------------------
//------------------------------------------------------------------------------
int cmd_g2r_linesys_CreateLineReq			(CG_CmdPacket *cmd);
int cmd_g2r_linesys_EnterLineReq			(CG_CmdPacket *cmd);
int cmd_g2r_linesys_EnterLineRes			(CG_CmdPacket *cmd);
int cmd_g2r_linesys_SwitchLineReq			(CG_CmdPacket *cmd);	
int cmd_g2r_linesys_SwitchLineRes			(CG_CmdPacket *cmd);	
int cmd_g2r_linesys_LeaveLineReq			(CG_CmdPacket *cmd);	
int cmd_g2r_linesys_LineInfoRes				(CG_CmdPacket *cmd);
//------------------------------------------------------------------------------


class CLineSysCmdBuilder_R2C
{
public:
	static void Build_r2c_linesys_SwitchLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError);
	static void Build_r2c_linesys_LineInfoRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, list<SLineInfo>* plistLineInfo);
	static void Build_r2c_linesys_EnterLineNotify(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError);
	static void Build_r2c_linesys_LeaveLineNotify(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, DWORD dwError);
private:
	static void Build_R2C_Cmd_Begin();
};

class CLineSysCmdSend_R2G
{
public:
	static void Send_r2g_linesys_CreateLineRes(int iRegionID);
	static void Send_r2g_linesys_EnterLineReq(DWORD dwGroupID, DWORD dwUserID, long* ppos);
	static void Send_r2g_linesys_EnterLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, CRegionUser* pUser);
	static void Send_r2g_linesys_SwitchLineReq(DWORD dwGroupID, DWORD dwNewLineID, DWORD dwOldLineID, DWORD dwUserID);
	static void Send_r2g_linesys_LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, const char* pstrmap, long* ppos);
	static void Send_r2g_linesys_LeaveLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, CRegionUser* pUser);
	static void Send_r2g_linesys_LineInfoReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID);
};



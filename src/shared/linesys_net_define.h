//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#pragma once

const short c2r_linesys_cmd_base = 10000;
const short r2c_linesys_cmd_base = 15000;
const short r2g_linesys_cmd_base = 20000;
const short g2r_linesys_cmd_base = 25000;

//--------------------
//gc 2 region消息
//--------------------
const short c2r_linesys_SwitchLineReq		= c2r_linesys_cmd_base + 111;
const short c2r_linesys_LineInfoReq	= c2r_linesys_cmd_base + 112;

//--------------------
//region 2 gc消息
//--------------------
const short r2c_linesys_SwitchLineRes			= r2c_linesys_cmd_base + 110;
const short r2c_linesys_LineInfoRes				= r2c_linesys_cmd_base + 111;
const short r2c_linesys_EnterLineNotify			= r2c_linesys_cmd_base + 112;
const short r2c_linesys_LeaveLineNotify			= r2c_linesys_cmd_base + 113;
//--------------------
//region 2 gw消息
//--------------------
const short r2g_linesys_CreateLineRes			= r2g_linesys_cmd_base + 110;
const short r2g_linesys_EnterLineReq			= r2g_linesys_cmd_base + 111;
const short r2g_linesys_EnterLineRes			= r2g_linesys_cmd_base + 112;
const short r2g_linesys_SwitchLineReq				= r2g_linesys_cmd_base + 113;
const short r2g_linesys_LeaveLineReq				= r2g_linesys_cmd_base + 114;
const short r2g_linesys_LeaveLineRes			= r2g_linesys_cmd_base + 115;
const short r2g_linesys_LineInfoReq				= r2g_linesys_cmd_base + 116;

//--------------------
//gw 2 region消息
//--------------------

const short g2r_linesys_CreateLineReq			= g2r_linesys_cmd_base + 110;
const short g2r_linesys_EnterLineReq				= g2r_linesys_cmd_base + 111;
const short g2r_linesys_EnterLineRes			= g2r_linesys_cmd_base + 112;
const short g2r_linesys_SwitchLineReq		= g2r_linesys_cmd_base + 113;
const short g2r_linesys_SwitchLineRes			= g2r_linesys_cmd_base + 114;
const short g2r_linesys_LeaveLineReq			= g2r_linesys_cmd_base + 115;
const short g2r_linesys_LineInfoRes			= g2r_linesys_cmd_base + 116;


//---------------------------------------------------------
//Error Code(BYTE)
//---------------------------------------------------------

//------------
#define ERR_NULL				0	//无错	
#define ERR_FULL			1	//人满
#define ERR_GENERAL			2	//一般性错误

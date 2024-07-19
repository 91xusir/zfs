//
// s_cmdchannel.h: interface for the CS_CmdChannel class.
//
#ifndef __CS_CMD_CHANNEL_H__
#define __CS_CMD_CHANNEL_H__

struct SCommand
{
	TCmdFunction  Func;
	int			  CmdId;
	string		  CmdName;
	string		  Desc;
};

const int CMD_ERROR_NONE	   = 0;
const int CMD_ERROR_READ_DATA  = 1;
const int CMD_ERROR_WRITE_DATA = 2;
const int CMD_ERROR_KICK_USER  = 3;
const int CMD_ERROR_UNKNOWN    = -1;
	
class CCmdChannel
{
public:
	CCmdChannel();
	virtual ~CCmdChannel();
	int   GetCmdCount();
	bool  RecvCmd(short cmdId,CG_CmdPacket *cmd);
	bool  AddCmd(SCommand *cmd);
	bool  AddCmd(TCmdFunction func,int id,const char *name,const char *desc);
	SCommand *GetCmdById(int id);
	SCommand *GetCmdByName(const char *name);

	string m_channelName;
	vector<SCommand>  m_cmdList;
};

#endif

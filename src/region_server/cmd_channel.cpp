//
// s_cmdchannel.cpp: implementation of the CCmdChannel class.
//
#include "region.h"


CCmdChannel::CCmdChannel()
{

}

CCmdChannel::~CCmdChannel()
{

}

bool CCmdChannel::AddCmd(SCommand *cmd)
{
	m_cmdList.push_back(*cmd);
	return true;
}

bool CCmdChannel::AddCmd(TCmdFunction func,int id,const char *name,const char *desc)
{
	SCommand cmd;
	cmd.Func    = func;
	cmd.CmdId   = id;
	cmd.CmdName = name;
	cmd.Desc	= desc;
	
	m_cmdList.push_back(cmd);
	return true;
}

SCommand *CCmdChannel::GetCmdByName(const char *name)
{
	for(unsigned int i=0;i<m_cmdList.size();i++)
	{
		if(m_cmdList[i].CmdName==name) return &m_cmdList[i]; 
	}
	return NULL;
}

SCommand *CCmdChannel::GetCmdById(int id)
{
	for(unsigned int i=0;i<m_cmdList.size();i++)
	{
		if (m_cmdList[i].CmdId==id) return &m_cmdList[i];
	}
	return NULL;
}

int CCmdChannel::GetCmdCount()
{
	return (int)m_cmdList.size();
}

bool CCmdChannel::RecvCmd(short cmdId,CG_CmdPacket *cmd)
{
	int ret;
	SCommand *src;
	
	src = GetCmdById(cmdId);
	if(!src)
	{
		LOG2("CCmdChannel: channel %s : unknow cmd = %d\n",m_channelName.c_str(),cmdId);
		return false;
	}
	ret = src->Func(cmd);
	
	// check result
	if(ret != CMD_ERROR_NONE)
	{
		LOG3("CCmdChannel: channel %s : cmd id = %d,ret = %d\n",m_channelName.c_str(),cmdId,ret);
        if (ret==CMD_ERROR_KICK_USER)
        {
            return false;
        }
		return true;
	}
	return true;
}

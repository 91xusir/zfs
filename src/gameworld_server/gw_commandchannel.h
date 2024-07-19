#ifndef _INC_GW_COMMAND_CHANNEL_H_
#define _INC_GW_COMMAND_CHANNEL_H_

#ifdef _DEBUG

#define GW_BEGIN_CMD(func) \
int func(CG_CmdPacket* packet) \
{ \
	guard; \
	// LOG1("Execute net command [ %s ]\n", #func); \

#else

#define GW_BEGIN_CMD(func) \
    int func(CG_CmdPacket* packet) \
{ \
    guard; \

#endif

#define GW_END_CMD \
    return eOkay; \
    unguard; \
}



enum GWCommandResult
{
	eOkay,
	eError,
	eNetwork,
	eInvalidCmd,
	eInvalidData
};

class GWCommandChannel
{
public:
	typedef int (*CommandFunction)(CG_CmdPacket* pkt);

private:
	struct Command
	{
		int cmdID;
		CommandFunction func;
		string name;
	};

public:
	void RegisterCommand(int cmdID, CommandFunction func, const char* name)
	{
		m_commands.push_back(Command());
		m_commands.back().cmdID = cmdID;
		m_commands.back().func = func;
		m_commands.back().name = name;
	}

	int Invoke(int cmdID, CG_CmdPacket* pkt)
	{
		stlforeach(list<Command>, iter, m_commands)
		{
			if (iter->cmdID == cmdID)
			{
				return iter->func(pkt);
			}
		}
		
		return eInvalidCmd;
	}

private:
	list<Command> m_commands;
};

#endif // _INC_GW_COMMAND_CHANNEL_H_

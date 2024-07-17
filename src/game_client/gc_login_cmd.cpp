#include "gc_include.h"
#include "region_client.h"
#include "gc_login_session.h"

using namespace gc_net;

#define BEGIN_CMD_FUNC(name) bool GcLoginSession::name(CG_CmdPacket* packet) { guard;
#define END_CMD_FUNC ;return true; unguard; }

BEGIN_CMD_FUNC(cmd_g2c_login_ret)
{
	long result;

	if (!packet->ReadLong(&result))
		return false;

	GetWorld()->OnNetLogin(result == 0);
}
END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_create_char_ret)
{
	long result;
	long chatID;
	char* name;

	if (!packet->ReadLong(&result))
		return false;

	// result = 0 succeeded
	// result = 1 duplate name
	// result = 2 general errro

	if (!packet->ReadLong(&chatID))
		return false;

	if (!packet->ReadString(&name))
		return false;
}
END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_delete_char_ret)
{
	long result;

	if (!packet->ReadLong(&result))
		return false;

	// result = 0 succeeded
	// result = 1 failed
}
END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_query_account_ret)
{
	GcAccountInfo m_account;

	long chatCount;

	if (!packet->ReadLong(&chatCount))
		return false;

	if (chatCount > 8)
		return false;

	m_account.chatCount	 = chatCount;

	for (int n = 0; n < chatCount; n++)
	{
		char* name;
		char* delDate;

		if (!packet->ReadLong(&m_account.users[n].id))
			return false;

		if (!packet->ReadByte(&m_account.users[n].metier))
			return false;

		if (!packet->ReadString(&name))
			return false;

		if (!packet->ReadString(&delDate))
			return false;

		m_account.users[n].name = name;
		m_account.users[n].deleteDate = delDate;
	}
}
END_CMD_FUNC;


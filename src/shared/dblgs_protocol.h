#ifndef _INC_LOGIN_DBAGENT_PROTOCOL_H_
#define _INC_LOGIN_DBAGENT_PROTOCOL_H_

enum
{
	lgs2agent_base              = 100,
	lgs2agent_query_user		= 101,

	agent2lgs_base				= 200,
	agent2lgs_query_user_ret    = 201,

};

#endif // _INC_LOGIN_DBAGENT_PROTOCOL_H_

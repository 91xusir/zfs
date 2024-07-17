//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "core/rt2_core.h"
#include "net/rt_net.h"
#include "rt1tort2.h"
#include "castlewar_base.h"

const STWarInfo& STWarInfo::operator =(const STWarInfo& rhs)
{
	guard;
	
	feod_name			= rhs.feod_name;
	master_union_id		= rhs.master_union_id;
	master_union_logo	= rhs.master_union_logo;
	master_union_name	= rhs.master_union_name;
	vassal_name			= rhs.vassal_name;
	flag_id				= rhs.flag_id;
	state				= rhs.state;
	start_time			= rhs.start_time;
	end_time			= rhs.end_time;
	last_end_time		= rhs.last_end_time;
	guard_animal_is_dead	= rhs.guard_animal_is_dead;

	v_attacker_union_info = rhs.v_attacker_union_info;
	v_defender_union_info = rhs.v_defender_union_info;

	return *this;

	unguard;
}

bool STAttackerInfo::Serialize(CG_CmdPacket *cmd)
{
	guard;

	*cmd << union_id;
	*cmd << union_name;
	*cmd << vassal_name;
	*cmd << flag_id;

	if (cmd->IsError())
		return false;
	else 
		return true;

	unguard;
}
bool STDefenderInfo::Serialize(CG_CmdPacket *cmd)
{
	guard;

	*cmd << union_id;
	*cmd << union_name;
	*cmd << vassal_name;
	*cmd << is_allowed;
	*cmd << flag_id;

	if (cmd->IsError())
		return false;
	else 
		return true;

	unguard;
}

bool STWarInfo::Serialize(CG_CmdPacket *cmd)
{
	guard;

	*cmd << feod_name;
	*cmd << master_union_id;
	*cmd << master_union_name;
	*cmd << master_union_logo;
	*cmd << vassal_name;
	*cmd << flag_id;
	*cmd << (int&)state;
	*cmd << start_time;
	*cmd << end_time;
	*cmd << last_end_time;
	*cmd << guard_animal_is_dead;

	if(cmd->IsReading())
	{
		char size;

		v_attacker_union_info.clear();
		v_defender_union_info.clear();

		*cmd << size;

		if((int)size < 0)
		{
			return false;
		}

		for(int i = 0; i < (int)size; i++)
		{
			STAttackerInfo tmp;

			tmp.Serialize(cmd);

			v_attacker_union_info.push_back(tmp);
		}
		
		*cmd << size;

		if((int)size < 0)
		{
			return false;
		}

		for(int i = 0; i < (int)size; i++)
		{
			STDefenderInfo tmp;

			tmp.Serialize(cmd);

			v_defender_union_info.push_back(tmp);
		}

	}else
	{
		char size = (char)v_attacker_union_info.size();

		*cmd << size;

		for(int i = 0; i < (int)size; i++)
		{
			v_attacker_union_info[i].Serialize(cmd);
		}

		size = (char)v_defender_union_info.size();

		*cmd << size;

		for(int i = 0; i < (int)size; i++)
		{
			v_defender_union_info[i].Serialize(cmd);
		}
	}

	if (cmd->IsError())
		return false;
	else 
		return true;

	unguard;
}


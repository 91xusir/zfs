#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "net/rt_net.h"
#include "herolist_base.h"

SHeroList::SHeroList(int HeroCount, int UnionCount)
{
	m_version = 0;
	m_nHeroCount = HeroCount;
	m_nUnionCount = UnionCount;
}

SHeroList::~SHeroList()
{
}

bool SHeroList::Serialize(CG_CmdPacket *cmd)
{
	guard;

	if ( cmd->IsReading() )
	{
		m_warriorList.clear();
		m_hunterList.clear();
		m_taolistList.clear();
		m_wizardList.clear();
		m_unionList.clear();

		SHeroListItem hlItem;
		SUnionListItem ulItem;
		short size;

		*cmd << size;
		for (int i=0; i<size; i++)
		{
			*cmd << hlItem.name;
			*cmd << hlItem.level;
			*cmd << hlItem.score;
			m_warriorList.push_back(hlItem);
		}
		*cmd << size;
		for (int i=0; i<size; i++)
		{
			*cmd << hlItem.name;
			*cmd << hlItem.level;
			*cmd << hlItem.score;
			m_hunterList.push_back(hlItem);
		}
		*cmd << size;
		for (int i=0; i<size; i++)
		{
			*cmd << hlItem.name;
			*cmd << hlItem.level;
			*cmd << hlItem.score;
			m_taolistList.push_back(hlItem);
		}
		*cmd << size;
		for (int i=0; i<size; i++)
		{
			*cmd << hlItem.name;
			*cmd << hlItem.level;
			*cmd << hlItem.score;
			m_wizardList.push_back(hlItem);
		}
		*cmd << size;
		for (int i=0; i<size; i++)
		{
			*cmd << ulItem.name;
			*cmd << ulItem.level;
			*cmd << ulItem.score;
			m_unionList.push_back(ulItem);
		}
	}
	else
	{
		short size = (short)m_warriorList.size();
		cmd->WriteShort(size);
		for (int i=0; i<size; i++)
		{
			cmd->WriteString((char*)m_warriorList[i].name.c_str()); // 只从服务器下载到客户端
			cmd->WriteShort(m_warriorList[i].level);
			cmd->WriteShort(m_warriorList[i].score);
		}
		size = (short)m_hunterList.size();
		cmd->WriteShort(size);
		for (int i=0; i<size; i++)
		{
			cmd->WriteString((char*)m_hunterList[i].name.c_str()); // 只从服务器下载到客户端
			cmd->WriteShort(m_hunterList[i].level);
			cmd->WriteShort(m_hunterList[i].score);
		}
		size = (short)m_taolistList.size();
		cmd->WriteShort(size);
		for (int i=0; i<size; i++)
		{
			cmd->WriteString((char*)m_taolistList[i].name.c_str()); // 只从服务器下载到客户端
			cmd->WriteShort(m_taolistList[i].level);
			cmd->WriteShort(m_taolistList[i].score);
		}
		size = (short)m_wizardList.size();
		cmd->WriteShort(size);
		for (int i=0; i<size; i++)
		{
			cmd->WriteString((char*)m_wizardList[i].name.c_str()); // 只从服务器下载到客户端
			cmd->WriteShort(m_wizardList[i].level);
			cmd->WriteShort(m_wizardList[i].score);
		}
		size = (short)m_unionList.size();
		cmd->WriteShort(size);
		for (int i=0; i<size; i++)
		{
			cmd->WriteString((char*)m_unionList[i].name.c_str()); // 只从服务器下载到客户端
			cmd->WriteShort(m_unionList[i].level);
			cmd->WriteShort(m_unionList[i].score);
		}
	}

	return true;

	unguard;
}

#ifndef _HEROLIST_BASE_H
#define _HEROLIST_BASE_H

#include <string>
#include <vector>
using namespace std;
class CG_CmdPacket;

#define HERO_LIST_COUNT			30
#define UNION_LIST_COUNT		20

struct SHeroListItem
{
	string	name;
	short	level;
	short	score;
	SHeroListItem() { name = ""; level = 0; score = 0; }
};

struct SUnionListItem
{
	string	name;
	short	level;
	short	score;
	SUnionListItem() { name = ""; level = 0; score = 0; }
};

class SHeroList
{
public:
	SHeroList(int HeroCount, int UnionCount);
	~SHeroList();

	int m_version;
	int m_nHeroCount;
	int m_nUnionCount;
	vector<SHeroListItem> m_warriorList;	//սʿ
	vector<SHeroListItem> m_hunterList;		//������
	vector<SHeroListItem> m_wizardList;		//��ʿ
	vector<SHeroListItem> m_taolistList;	//��ʿ

	vector<SUnionListItem> m_unionList;

	bool Serialize(CG_CmdPacket *cmd);
};

#endif//_HEROLIST_BASE_H


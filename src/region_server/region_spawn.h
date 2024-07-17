#ifndef _INC_REGION_SPAWN_H_
#define _INC_REGION_SPAWN_H_

#include <list>

class CRegionSpawn
{
	friend class CRegionSpawnMgr;

	struct SpawnLair
	{
		long				begin[2];
		long				end[2];
		CRegionCreature*	npc;
	};		
	
private:
	void			Init(short id, short npcID, int maxNpcCount, int spawnInterval, float rate, long areaBegin[], long areaEnd[], int timeBegin = 0, int timeEnd = 24);
	void			Spawn(int timeStamp,CRegionScene *scene);
	void			Clear();
	void			RealSpawn(bool cheat, CRegionScene *scene);
	CRegionCreature* CreateNpc(long* pos, char dir, bool cheat, CRegionScene *scene);	

	void			_check_spawn();

public:
	void			KillNpc(CRegionCreature* npc);
	CRegionSpawn*	GetUnderling() { return m_underling; }
	CRegionSpawn*	GetLeader() { return m_leader; }
	void			SetLeader(CRegionSpawn* leader);

public:
	CRegionSpawn();

public:
	CRegionSpawn*	m_leader;
	CRegionSpawn*	m_underling;

	int				m_factionId;

	char			m_unitedPoint;
	char			m_loyalPoint;
	char			m_bravePoint;

	short			m_angryPointLevel1;
	short			m_angryPointLevel2;

	float			m_rate;
	short			m_id;
	short			m_npcID;
	int				m_maxNpcCount;
	int				m_curNpcCount;
	int				m_lastSpawnTimeStamp;
	int				m_spawnInterval;
	int				m_timeBegin;
	int				m_timeEnd;	
	long			m_areaBegin[3];
	long			m_areaEnd[3];
	DWORD			m_ai_level;
	DWORD			m_proTarget;
	std::list<SPathNode> m_PatrolPath;	//巡逻路径
	vector<SpawnLair> m_npc;
};

class CRegionSpawnMgr
{
public:
	bool			LoadSpawnTable(const char* filename);
	void			Spawn();
	void			SpawnCheat();
	CRegionSpawn*	GetSpawnById(short id);
	void            SetScene(CRegionScene *scene) { m_scene = scene; }
	void            SetLineSysSceneID(int nLineSysSceneID) { m_nLineSysSceneID = nLineSysSceneID; }

public:
	CRegionSpawnMgr();

public:
	list<CRegionSpawn> m_spawns;

	float			m_respawnIntervalFactor;
	float			m_respawnCountFactor;
	CRegionScene   *m_scene;

	list<CRegionSpawn> m_cheatSpawns;
	list<CRegionSpawn>::iterator m_cheatIt;
	int	m_nCheatIndex;
	int m_nLineSysSceneID;//分线系统加载的Scene文件ID, 对应spawn表中的所在场景ID
};

extern CRegionSpawnMgr g_spawnMgr;

#endif // _INC_REGION_SPAWN_H_

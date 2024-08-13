#include "region.h"
#include "region_spawn.h"
#include "region_ai.h"
#include "region_state_snapshot.h"

CRegionSpawnMgr g_spawnMgr;

#define TAB_COL_ID "ID"
#define TAB_COL_SCENE_ID "所在场景ID"
#define TAB_COL_AREA "重生地点"
#define TAB_COL_INTERVAL "重生周期(秒)"
#define TAB_COL_MONSTER_ID "怪物ID"
#define TAB_COL_COUNT "怪物数量"
#define TAB_COL_RATE "重生概率"
#define TAB_COL_BEGIN_TIME "重生开始时间"
#define TAB_COL_END_TIME "重生结束时间"
#define TAB_COL_LEADER "首领"
#define TAB_COL_BRAVE "胆量"
#define TAB_COL_UNITED "团结"
#define TAB_COL_LOYAL "忠诚"
#define TAB_COL_FACTION_ID "阵营ID"
#define TAB_COL_ANGRY_LEVEL_1 "愤怒1"
#define TAB_COL_ANGRY_LEVEL_2 "愤怒2"
#define TAB_COL_AI_LEVEL "AI级别"
#define TAB_COL_TARGET_ID "保护目标ID"

CRegionSpawnMgr::CRegionSpawnMgr()
    : m_respawnIntervalFactor(1), m_respawnCountFactor(1), m_nCheatIndex(0), m_nLineSysSceneID(1) {}

CRegionSpawn* CRegionSpawnMgr::GetSpawnById(short id) {
    stlforeach(std::list<CRegionSpawn>, spawn, m_spawns) if (spawn->m_id == id) return &*spawn;

    return NULL;
}

BOOL VertexIntersectRectangle(const float v1, const float v2, const float rect[4]) {
    if (v1 < rect[0])
        return FALSE;
    if (v1 > rect[2])
        return FALSE;
    if (v2 < rect[1])
        return FALSE;
    if (v2 > rect[3])
        return FALSE;
    return TRUE;
}

int GetTriClockDir(const float* pt1, const float* pt2, const float* pt3) {
    float test;
    test = (((pt2[0] - pt1[0]) * (pt3[1] - pt1[1])) - ((pt3[0] - pt1[0]) * (pt2[1] - pt1[1])));
    if (test > 0)
        return 1;
    else if (test < 0)
        return -1;
    else
        return 0;
}

BOOL LineIntersectLine(const float* v0, const float* v1, const float* u0, const float* u1) {
    int test1_a, test1_b, test2_a, test2_b;

    test1_a = GetTriClockDir(v0, v1, u0);
    test1_b = GetTriClockDir(v0, v1, u1);
    if (test1_a != test1_b) {
        test2_a = GetTriClockDir(u0, u1, v0);
        test2_b = GetTriClockDir(u0, u1, v1);
        if (test2_a != test2_b) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL RectangleIntersectRectangle(const float r1[4], const float r2[4]) {
    if (VertexIntersectRectangle(r1[0], r1[1], r2))
        return TRUE;
    if (VertexIntersectRectangle(r1[0], r1[3], r2))
        return TRUE;
    if (VertexIntersectRectangle(r1[2], r1[1], r2))
        return TRUE;
    if (VertexIntersectRectangle(r1[2], r1[3], r2))
        return TRUE;

    if (VertexIntersectRectangle(r2[0], r2[1], r1))
        return TRUE;
    if (VertexIntersectRectangle(r2[0], r2[3], r1))
        return TRUE;
    if (VertexIntersectRectangle(r2[2], r2[1], r1))
        return TRUE;
    if (VertexIntersectRectangle(r2[2], r2[3], r1))
        return TRUE;

    if (LineIntersectLine(r1, r1 + 2, r2, r2 + 2))
        return TRUE;
    return FALSE;
}

bool CRegionSpawnMgr::LoadSpawnTable(const char* filename) {
    m_spawns.clear();
    m_cheatSpawns.clear();

    RtCsv file;

    if (!file.LoadTableFile(filename)) {
        ERR1("Can not open spawn table file %s\n", filename);

        return false;
    }

    int titleLine = file.FindLineByString("ID");

    if (titleLine < 0) {
        ERR("Invalid spawn table format, missing ID column\n");
        return false;
    }

    int numLines = file.GetLineCount();

    if (numLines <= 1) {
        ERR1("Invalid spawn table format (lines=%d)\n", numLines);
        return false;
    }

    for (int lineId = titleLine + 1; lineId < numLines; lineId++) {
        int nLineSysSceneID = file[lineId][TAB_COL_SCENE_ID].Int();

        if (nLineSysSceneID != m_nLineSysSceneID) {
            continue;
        }

        int npc = file[lineId][TAB_COL_MONSTER_ID].Int();

        SNpc* snpc = g_TableNpc.FindNpc(npc);

        if (snpc == NULL) {
            LOG2("Can not find npc %d\n", npc, lineId);

            continue;
        }

        const char* areaName = (char*)file[lineId][TAB_COL_AREA].Str();

        RtsAreaHelper* area = m_scene->m_pArea->FindArea(areaName);

        if (area == NULL)
            continue;

        // 检查这个区域是否被当前Region包含，如果不包含就不分配怪物
        float r1[4], r2[4];
        r1[0] = area->lMinX;
        r1[1] = area->lMinY;
        r1[2] = area->lMaxX;
        r1[3] = area->lMaxY;

        bool                                            bIntersect = false;
        RtsSceneBlockMap*                               pMap;
        EXT_SPACE::unordered_map<ULONG, long>::iterator it;
        for (it = g_region->m_regionMap.begin(); it != g_region->m_regionMap.end(); it++) {
            if ((*it).second == g_region->m_lCurRegionID) {
                pMap  = (RtsSceneBlockMap*)((*it).first);
                r2[0] = (pMap->m_pBlock->m_iBlockPosX) * g_fSceneBlockWidth;
                r2[1] = (pMap->m_pBlock->m_iBlockPosY) * g_fSceneBlockHeight;
                r2[2] = r2[0] + g_fSceneBlockWidth;
                r2[3] = r2[1] + g_fSceneBlockHeight;

                if (!(r1[0] >= r2[2] || r1[1] >= r2[3] || r1[2] < r2[0] || r1[3] < r2[1])) {
                    bIntersect = true;
                    break;
                }
            }
        }
        if (!bIntersect && m_scene->m_sceneName != "gz" && m_nLineSysSceneID == 1) {
            continue;
        }

        // 怪物数量
        float count = file[lineId][TAB_COL_COUNT].Int();

        if (count <= 0)
            continue;

        float interval = file[lineId][TAB_COL_INTERVAL].Int();

        const char* t;

        t             = file[lineId][TAB_COL_BEGIN_TIME];
        int timeBegin = t && *t ? atol(t) : 0;

        t           = file[lineId][TAB_COL_END_TIME];
        int timeEnd = t && *t ? atol(t) : 24;

        m_spawns.push_back(CRegionSpawn());
        m_cheatSpawns.push_back(CRegionSpawn());

        CRegionSpawn* spawn      = &m_spawns.back();
        CRegionSpawn* cheatSpawn = &m_cheatSpawns.back();

        interval *= 1000;
        interval *= m_respawnIntervalFactor;
        count *= m_respawnCountFactor;
        count += 0.00001f;

        float rate = file[lineId][TAB_COL_RATE].Float();
        int   id   = file[lineId][TAB_COL_ID].Int();

        if (count < 1)
            count = 1;

        spawn->Init(id, npc, count, (int)interval, rate, &area->lMinX, &area->lMaxX, timeBegin,
                    timeEnd);

        int cheatCount;
        if (count == 1) {
            cheatCount = 0;
        } else if (count < 10) {
            cheatCount = count + 1;
        } else {
            cheatCount = count * (1 + 0.1f);
        }
        if (cheatCount > 0)
            cheatSpawn->Init(id + 10000, npc, cheatCount, (int)interval, rate, &area->lMinX,
                             &area->lMaxX, timeBegin, timeEnd);

        int leader = file[lineId][TAB_COL_LEADER].Int();
        if (leader != 0)
            spawn->SetLeader(GetSpawnById(leader));

        spawn->m_bravePoint       = (char)(100 * file[lineId][TAB_COL_BRAVE].Float());
        spawn->m_unitedPoint      = (char)(100 * file[lineId][TAB_COL_UNITED].Float());
        spawn->m_loyalPoint       = (char)(100 * file[lineId][TAB_COL_LOYAL].Float());
        spawn->m_factionId        = (char)(100 * file[lineId][TAB_COL_FACTION_ID].Int());
        spawn->m_angryPointLevel1 = file[lineId][TAB_COL_ANGRY_LEVEL_1].Int();
        spawn->m_angryPointLevel2 = file[lineId][TAB_COL_ANGRY_LEVEL_2].Int();
        //spawn->m_proTarget = file[lineId][TAB_COL_TARGET_ID].DWord();

        //DWORD ai_level = file[lineId][TAB_COL_AI_LEVEL].DWord();
        //spawn->m_ai_level = 0x0000;

        //switch(ai_level)
        //{
        //case 0:
        //	spawn->m_ai_level = AI_FIXED;
        //	break;
        //case 1:
        //	spawn->m_ai_level = AI_FIXED | AI_CONJURE;
        //	break;
        //case 2:
        //	spawn->m_ai_level = AI_IDLE;
        //	break;
        //case 3:
        //	spawn->m_ai_level = AI_IDLE | AI_PROTECT;
        //	break;
        //case 4:
        //	spawn->m_ai_level = AI_IDLE | AI_CONJURE;
        //	break;
        //case 5:
        //	spawn->m_ai_level = AI_IDLE | AI_PROTECT | AI_CONJURE;
        //	break;
        //case 6:
        //	spawn->m_ai_level = AI_PATROL;
        //	break;
        //case 7:
        //	spawn->m_ai_level = AI_PATROL | AI_PROTECT;
        //	break;
        //case 8:
        //	spawn->m_ai_level = AI_PATROL | AI_CONJURE;
        //	break;
        //case 9:
        //	spawn->m_ai_level = AI_PATROL | AI_PROTECT | AI_CONJURE;
        //	break;
        //default:
        //	spawn->m_ai_level = AI_FIXED;
        //	break;
        //}

        ////读取巡逻路径，其格式为1,1;2,2
        //char delim[] = ";";								//分隔符

        //SPathNode node = {0, 0};						//路径节点

        //char* pStrTmp = (char *)file[lineId][TAB_COL_PATROL_PATH].Str();	//提取字符串

        //char* k;

        //if(pStrTmp)
        //{
        //	k = strtok(pStrTmp, delim);					//取得第一个坐标

        //	if( (k != NULL) && (strlen(k) > 0) )
        //	{
        //		//循环提取所有坐标
        //		while(k != NULL)
        //		{
        //			//坐标读取错误
        //			if(sscanf(k, "%d,%d", &node.x, &node.y) != 2)
        //			{
        //				ERR("文件npc.csv坐标格式出错。\n");
        //				return false;
        //			}

        //			spawn->m_PatrolPath.push_back(node);

        //			k = strtok(NULL, delim);			//取得后续坐标
        //		}//end of while

        //	}//end of if

        //}//end of if
        //
    }

    m_cheatIt = m_cheatSpawns.begin();
    return true;
}

CRegionSpawn::CRegionSpawn()
    : m_leader(NULL), m_underling(NULL), m_rate(1), m_factionId(0), m_unitedPoint(100),
      m_loyalPoint(100), m_bravePoint(100), m_angryPointLevel1(0), m_angryPointLevel2(0),
      m_ai_level(0), m_lastSpawnTimeStamp(0) {}

void CRegionSpawnMgr::Spawn() {
    unsigned hours     = g_region->m_gwt.GetTrueTime();
    int      timeStamp = rtGetMilliseconds();

    stlforeach(std::list<CRegionSpawn>, iter, m_spawns) {
        if (hours >= (unsigned)iter->m_timeBegin && hours <= (unsigned)iter->m_timeEnd)
            iter->Spawn(timeStamp, m_scene);
        else
            iter->Clear();
    }
}

void CRegionSpawnMgr::SpawnCheat() {
    int cnt = 0;
    while (cnt++ < 100) {
        if (m_cheatIt != m_cheatSpawns.end()) {
            ++m_cheatIt;
            ++m_nCheatIndex;
            if (m_nCheatIndex >= m_cheatSpawns.size()) {
                ERR("[Check spawn] cheat IT spawn error over the size.\nSize:%d, Index:%d\n",
                    m_cheatSpawns.size(), m_nCheatIndex);
                m_cheatIt = m_cheatSpawns.end();
                return;
            }
        } else {
            m_cheatIt     = m_cheatSpawns.begin();
            m_nCheatIndex = 0;
        }
        CRegionSpawn* spawn = &(*m_cheatIt);
        if (spawn->m_curNpcCount >= spawn->m_maxNpcCount)
            continue;
        spawn->RealSpawn(true, m_scene);
        break;
    }
    if (g_region->IsCheatId(g_factory->PeekNextObjectId()))
        g_factory->GetNextObjectId();
}

void CRegionSpawn::Spawn(int now, CRegionScene* scene) {
    if (m_curNpcCount >= m_maxNpcCount) {
        m_lastSpawnTimeStamp = now;
        return;
    }

    int spawnTime = m_lastSpawnTimeStamp + m_spawnInterval;

    if (m_lastSpawnTimeStamp != 0 && now < spawnTime)
        return;

    m_lastSpawnTimeStamp = now;

    float rate = rand() % 100 / 100.0f;

    if (rate > m_rate)
        return;

    RealSpawn(false, scene);
}

void CRegionSpawn::Init(short id, short npcID, int maxNpcCount, int spawnInterval, float rate,
                        long areaBegin[2], long areaEnd[2], int timeBegin, int timeEnd) {
    m_id                 = id;             // ID
    m_npcID              = npcID;          // 怪物ID
    m_maxNpcCount        = maxNpcCount;    // 怪物数量
    m_curNpcCount        = 0;              // 当前 怪物数量
    m_spawnInterval      = spawnInterval;  // 重生周期(秒)
    m_rate               = rate;           // 重生概率
    m_lastSpawnTimeStamp = 0;              // 最后一次重生时间
    memcpy(m_areaBegin, areaBegin, sizeof(long) * 2);
    memcpy(m_areaEnd, areaEnd, sizeof(long) * 2);
    m_timeBegin = timeBegin;  // 重生开始时间
    m_timeEnd   = timeEnd;    // 重生结束时间

    int rangeu = areaEnd[0] - areaBegin[0];  // 范围 x
    int rangev = areaEnd[1] - areaBegin[1];  // 范围 y

    float asp   = fabs((float)rangeu / rangev);  // 范围宽高比
    float vsize = sqrtf(maxNpcCount / asp);      // x 方向上有多少个怪

    int cv = (int)vsize;          // x 方向上有多少个怪
    int cu = (int)(vsize * asp);  // y 方向上有多少个怪

    if (cu == 0)  // 限制范围
        cu = 1;
    else if (cu > maxNpcCount)
        cu = maxNpcCount;

    if (cv == 0)  // 限制范围
        cv = 1;
    else if (cv > maxNpcCount)
        cv = maxNpcCount;

    int num = cu * cv;            // 当前计算的总数是多少个怪
    int fur = maxNpcCount - num;  // 离需要设置的有多少偏差

    cv += fur / cu;  // x 方向上添加多少行

    fur %= cu;  // 还剩下多少偏差

    m_npc.resize(maxNpcCount);  // 分配 npc

    int exp = fur / cv;  //

    if (fur % cv)  // 额外的 y 方向上的列
        exp++;

    int tx1 = rangeu / cu;          // x 方向上的距离(大)
    int tx2 = rangeu / (cu + exp);  // x 方向上的距离(小)
    int ty  = rangev / cv;          // y 方向上的距离

    float fZx = (cu > 5) ? 1.4f : 1.f;  // 在原来的基础上扩大1.4倍的区域选择范围
    float fZy = (cv > 5) ? 1.4f : 1.f;

    std::vector<SpawnLair>::iterator lair = m_npc.begin();

    for (int n = 0; n < cv; n++) {
        int t;
        int cm;

        if (fur > 0) {
            t = tx2 * fZx;

            if (fur >= exp) {
                fur -= exp;
                cm = cu + exp;
            } else {
                cm  = cu + fur;
                fur = 0;
            }
        } else {
            t  = tx1 * fZx;
            cm = cu;
        }

        for (int m = 0; m < cm; m++) {
            lair->begin[0] = areaBegin[0] + m * t;
            lair->begin[1] = areaBegin[1] + n * ty * fZy;
            lair->end[0]   = areaBegin[0] + (m + 1) * t;
            lair->end[1]   = areaBegin[1] + (n + 1) * ty * fZy;

            lair->npc = NULL;
            lair++;
        }
    }

    CHECK(lair == m_npc.end());
}

void CRegionSpawn::KillNpc(CRegionCreature* npc) {
    std::cout << "CRegionSpawn::KillNpc:" << npc->m_npcId << std::endl;
    std::cout << "Cm_curNpcCount:" << m_curNpcCount << std::endl;
    CHECK(m_curNpcCount > 0);
    stlforeach(std::vector<SpawnLair>, lair, m_npc) {
        if (lair->npc == npc) {
            lair->npc = NULL;
            m_curNpcCount--;
            break;
        }
    }
}

void CRegionSpawn::_check_spawn() {
    if (m_npcID < 0 || m_npcID > 2000) {
        CRegionSpawn& theSpawn = *(g_spawnMgr.m_cheatIt);
        if (theSpawn.m_npcID > 2000 || theSpawn.m_npcID < 0)
            ERR("[Check spawn] cheat IT spawn error npcID %d spawnID %d\n", theSpawn.m_npcID,
                theSpawn.m_id);

        list<CRegionSpawn>::iterator it      = g_spawnMgr.m_cheatSpawns.begin();
        int                          counter = 0;
        for (; it != g_spawnMgr.m_cheatSpawns.end(); ++it) {
            CRegionSpawn& spawn = *it;
            if (it == g_spawnMgr.m_cheatIt)
                ERR("[Check spawn] cheat IT point to list at %d\n", counter);

            if (spawn.m_npcID > 2000 || spawn.m_npcID < 0)
                ERR("[Check spawn] list spawn error at %d npcID %d spawnID %d\n", counter,
                    spawn.m_npcID, spawn.m_id);

            ++counter;
        }
    }
}

void CRegionSpawn::RealSpawn(bool cheat, CRegionScene* scene) {
    long pos[3];
    char dir;

    stlforeach(std::vector<SpawnLair>, lair, m_npc) {
        int nCun = 3;
        while (!lair->npc && nCun > 0) {
            dir = rand() % 255;
            //PZH
            /*
			int a = lair->begin[0];
			int b = lair->begin[1];

			long rangex = (lair->end[0] - lair->begin[0]) / 20;
			long rangey = (lair->end[1] - lair->begin[1]) / 20;		
			long rx = (rand() % rangex) * 20; if (rangex < 0) rx = -rx;
			long ry = (rand() % rangey) * 20; if (rangey < 0) ry = -ry;

			int tx, ty;
			scene->m_pTerrain->GetTerrainByPosFast(rx, ry, tx, ty);*/

            long rangex = lair->end[0] - lair->begin[0];
            long rangey = lair->end[1] - lair->begin[1];

            rangex /= 20;
            rangey /= 20;
            if (0 == rangex || 0 == rangey) {
                break;
            }
            if (rangex < 0) {
                rangex = -rangex;
            }
            if (rangey < 0) {
                rangey = -rangey;
            }
            long rx = (rand() % rangex) * 20;
            long ry = (rand() % rangey) * 20;
            //
            pos[0] = lair->begin[0] + rx;
            pos[1] = lair->begin[1] + ry;
            pos[2] = 0;

#ifdef KICK_CHEAT
            if (!cheat && g_region->IsCheatId(g_factory->PeekNextObjectId())) {
                nCun--;
                g_spawnMgr.SpawnCheat();
                continue;
            }
#endif
            CRegionCreature* npc = CreateNpc(pos, dir, cheat, scene);
            //if(!npc) return;
            if (NULL == npc) {
                nCun--;
                continue;
            }
            lair->npc    = npc;
            npc->m_spawn = this;
            m_curNpcCount++;
            if (cheat)
                return;
        }
    }
    // m_curNpcCount = m_maxNpcCount;
}

CRegionCreature* CRegionSpawn::CreateNpc(long* pos, char dir, bool cheat, CRegionScene* scene) {
    // spawn monster
    CRegionCreature* cre = g_region->CreateNpc(m_npcID, cheat);
    if (!cre) {
        LOG1("Can't create NPC (ID=%d)\n", m_npcID);
        return NULL;
    }

    // cre->m_ai = g_aiMgr.CreateAI("monster");
    cre->m_faction          = m_factionId;
    cre->m_unitedPoint      = m_unitedPoint;
    cre->m_loyalPoint       = m_loyalPoint;
    cre->m_bravePoint       = m_bravePoint;
    cre->m_angryPointLevel1 = m_angryPointLevel1;
    cre->m_angryPointLevel2 = m_angryPointLevel2;
    // cre->m_proTarget = m_proTarget;
    // cre->m_ai_level = m_ai_level;
    // cre->m_PatrolPath = m_PatrolPath;

    if (scene->AddCreature(cre, pos, dir)) {
        // cre->m_ai->OnBorn(cre);
        if (cre->IsScriptLoaded())
            cre->CallScript("OnCreate", false);
    } else {
        g_factory->DestroyObject(cre);
        cre = NULL;
    }
    return cre;
}

void CRegionSpawn::Clear() {
    if (m_curNpcCount == 0)
        return;

    stlforeach(std::vector<SpawnLair>, lair, m_npc) {
        if (lair->npc)
            lair->npc->Die(NULL);
    }

    m_curNpcCount = 0;
}

void CRegionSpawn::SetLeader(CRegionSpawn* leader) {
    if (m_leader == leader)
        return;

    if (m_leader)
        m_leader->m_underling->SetLeader(NULL);

    m_leader = leader;

    if (m_leader)
        m_leader->m_underling = this;
}

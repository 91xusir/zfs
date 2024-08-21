#include "region.h"
#include "rs_national_war.h"

//PZH
#include "PlayerLogic.h"
//

// ------------------------------------------------------------------------
// region item
// ------------------------------------------------------------------------
#define RAGE_LEVELUP_EXP 300

CRegionItem::CRegionItem()
{
	// SItemID m_item;
	// long    m_pos[3];
	// char    m_dir;
	m_masterId		 = 0;
    m_masterKeepTime = 0;
	m_sceneLiveTime  = 0;
	m_dropStyle = ItemDrop_ParabolaNear;
	m_hideTime = 0;
    m_pathNode = NULL;
	m_dropUserID = 0;
}

CRegionItem::~CRegionItem()
{
		
}

void CRegionItem::WriteSnapshot(CG_CmdPacket *cmd,CRegionCreature *target)
{
	short dx = m_pos[0] - target->m_pos[0];
	short dy = m_pos[1] - target->m_pos[1];
	cmd->WriteLong(m_oId);
	cmd->WriteByte(m_type);
	cmd->WriteLong(m_item.type);
	cmd->WriteByte(m_item.color);
	cmd->WriteByte(m_item.cBind);
	cmd->SerializeShortEx(dx);
	cmd->SerializeShortEx(dy);
	// cmd->WriteShort(m_pos[0] - target->m_pos[0]);
	// cmd->WriteShort(m_pos[1] - target->m_pos[1]);
	cmd->WriteByte(m_dir);
	cmd->WriteByte(m_dropStyle);
}

bool CRegionItem::GetNextDropPos(long* posCenter, long* pos, int& counter)
{
	// @@@@
	// 未考虑阻挡
	// 未检测地图边缘

	// offset :
	// 0 -- 不变
	// 1 -- 右
	// 2 -- 下
	// 3 -- 左
	// 4 -- 上
	static int offset[MAX_NPC_DROP_ITEM] = { 0, 1, 2, 3, 3, 4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4 }; //, 4, 1, 1, 1, 1, 1, };
	static long OFFSET_SIZE = 20;

	if (counter>=MAX_NPC_DROP_ITEM)
		return false;

	switch (offset[counter])
	{
	case 0:
		break;
	case 1:
		pos[0] += OFFSET_SIZE;
		break;
	case 2:
		pos[1] -= OFFSET_SIZE;
		break;
	case 3:
		pos[0] -= OFFSET_SIZE;
		break;
	case 4:
		pos[1] += OFFSET_SIZE;
		break;
	default:
		break;
	}

	counter ++;
	return true;
}

// ------------------------------------------------------------------------
// region scene
// ---------------------------------------------------------------------


EXT_SPACE::unordered_map< int, CRegionScene::NPCLIST > CRegionScene::ms_npcLists;

bool CRegionScene::LoadNpcList(const char* file)
{
	SSceneNpc tmp;

	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		tmp.lID			= csv[j]["ID"].Long();

		if (tmp.lID <= 0)
			continue;

		tmp.sSceneID	= csv[j]["所属场景ID"].Short();
		tmp.sNpcId		= csv[j]["npcID"].Short();
		tmp.sDif		= csv[j]["难度"].Short();
		tmp.cDir		= csv[j]["朝向"].Byte();
		if (csv[j]["启用"].Short() == 1)
			tmp.bEnable = true;
		else
			tmp.bEnable = false;

		if(sscanf(csv[j]["坐标"].Str(),"%d,%d,%d",&tmp.pos[0],&tmp.pos[1], &tmp.pos[2]) != 3)
		{
			ERR("Scene NPC list::Load %d parse 坐标 error.\n", tmp.lID);
			return false;
		}
		
		EXT_SPACE::unordered_map< int, NPCLIST >::iterator it = ms_npcLists.find(tmp.sSceneID);
		if (it == ms_npcLists.end())
			ms_npcLists[tmp.sSceneID] = NPCLIST();
		NPCLIST& npcList = ms_npcLists[tmp.sSceneID];

		npcList.push_back(tmp);
	}

	return true;
}

CRegionScene::CRegionScene()
{	
	m_pPathFind = RT_NEW RtsPathFind;
	m_pPathFind->InitOnce();	
	m_pPathFind->SetSearchType(1);
	m_defPos[0] = 1800;
	m_defPos[1] = 855;
	m_pArea = NULL;
	m_gridObj = NULL;
    m_cmdTmp.SetSize(16*1024);
	m_nSceneID = 0;
}

CRegionScene::~CRegionScene()
{
}

bool CRegionScene::Init()
{
	return true;
}

bool CRegionScene::Exit()
{
	OnDestroy();

	DEL_ARRAY(m_gridObj);
	m_pPathFind->ClearOnce();
	DEL_ONE(m_pPathFind);
	DEL_ONE(m_pArea);
	DEL_ONE(m_pTerrain);

	return true;
}

void CRegionScene::CreateAllNpc()
{
	EXT_SPACE::unordered_map< int, NPCLIST >::iterator allit = ms_npcLists.find(m_nSceneID);
	if (allit == ms_npcLists.end())
	{
		ERR("Create all npc on scene error!!! No npc in table for %d scene\n", m_nSceneID);
		return;
	}

	NPCLIST& npcList = ms_npcLists[m_nSceneID];
	for (NPCLIST::iterator it = npcList.begin(); it != npcList.end(); ++it)
	{
		SSceneNpc& sNpc = *it;
		if (sNpc.bEnable)
			AddNpc(sNpc);
	}
}

bool CRegionScene::TestAddCreature(long x,long y,long z)
{
	// C [12/9/2008 Louis.Huang]
/*
	int tx,ty;
    if(!m_pTerrain->GetTerrainTile(x, y, tx, ty))
        return false;
	 
	if(m_pPathFind->CanAdd(tx,ty))
		return true;
	else
*/
		return false;
}

bool CRegionScene::MoveCreature(CRegionCreature *cre,long x,long y,long z)
{
	int tx,ty;
	if(!m_pTerrain->GetTerrainTile(x, y, tx, ty))
		return false;

	////--------debug-----------//	
	//if((cre->m_userInfo != NULL) && (cre->m_userInfo->m_username == "1134"))
	//{
	//	LOG("-----------------------------------\n");
	//	LOG1("-玩家名字: [%s]\n", (char*)cre->m_userInfo->m_username.c_str());
	//	LOG2("-玩家移动目的地网格坐标: ([%d], [%d])\n", tx, ty);
	//	LOG1("-玩家路径总数: ([%d])\n", cre->m_pathNode->PathCount());
	//	LOG("-----------------------------------\n");
	//}	
	////--------debug-----------//

	if(m_pPathFind->MoveTo(cre->m_pathNode,tx,ty, 1))
    {
		// ...
		int newx,newy;
		long pos[3];
		pos[0] = x;
		pos[1] = y;
		if(GetGridByPos(pos,newx,newy))
		{
            if(cre->m_gridX != newx || cre->m_gridY != newy)
			{
				RemoveObject(cre,cre->m_pos);
				AddObject(cre,pos);
			}
		}
		return true;
    }
	else
    {
        if (cre->m_iUserMoveFailedCnt==0)
        {
			if (cre->m_userInfo)
				LOG1("User [%s] MoveFailed！\n", (char*)cre->m_userInfo->m_username.c_str());
        }
        cre->m_iUserMoveFailedCnt ++;
		return false;
    }
}

//
// creature operate
//
bool CRegionScene::AddCreature(CRegionCreature *cre,long *pos,char dir)
{
	if(cre->IsUser())
	{
		if(!OnTryAddUser(cre->m_userInfo))
			return false;
		cre->SetProtectTime(REBIRTH_PROTECT_MILLSECONDS);
	}

	if(!pos) pos = m_defPos;

	int x,y;
	int rx,ry;
    int bx,by;

	// check pos valid
    if (!m_pTerrain->GetTerrainTile(pos[0], pos[1], x, y))
        return false;

	if ( cre->m_npcInfo && cre->m_npcInfo->Type == 6 )
	{
		if(!m_pPathFind->FindSpace(cre->GetObjectId(), (short)cre->m_type, x,y,rx,ry,false))
		{
			ERR("CRegionScene::AddCreature: find space error\n");
			return false;
		}
	}
	// get a free space
	else if(!m_pPathFind->FindSpace(cre->GetObjectId(), (short)cre->m_type, x,y,rx,ry,true))
	{
		if(!m_pPathFind->FindSpace(cre->GetObjectId(), (short)cre->m_type, x,y,rx,ry,false))
		{
			ERR("CRegionScene::AddCreature: find space error\n");
			return false;
		}
	}
		
	// link creature and scene
	cre->m_scene = this;
	m_objectHash[cre->m_oId] = cre;
	// R [12/9/2008 Louis.Huang]
	//cre->m_pathNode = (RtsPathPath*)m_pPathFind->AddActor(cre->GetObjectId(), cre->m_type,rx,ry,true,false);
	TBlockType blockType = PB_BLOCK;
	if (cre->IsUser())
	{
		blockType = PB_UNBLOCK;
	}else if (cre->IsMonster() || cre->IsGuarder())
	{
		blockType = PB_UNBLOCK;
	}else if (OB_TYPE(cre) != OBJECT_TYPE_NPC_SERVICE)
	{
		if (cre->m_npcInfo && cre->m_npcInfo->Type == 8)
		{
			blockType = PB_UNBLOCK;
		}else
			blockType = PB_BLOCK;
	}else if (cre->IsUserPet() || cre->IsCallNpc())
	{
		blockType = PB_UNBLOCK;
	}
	cre->m_pathNode = (RtsPathPath*)m_pPathFind->AddActor(cre->GetObjectId(), cre->m_type,rx,ry,true,blockType);

	if (!cre->m_pathNode)
	{
		return false;
	}

	float px,py;
	m_pTerrain->GetTerrainPos(rx,ry,px,py);
	cre->m_pos[0] = px;
	cre->m_pos[1] = py;
	cre->m_dir = dir;

	cre->LeaveCombat();
	
	// if have a pet,add pet,if pet have a pet ...
	if(cre->m_pet || cre->m_dwPetSave > 0)
	{
		if (cre->m_pet)
		{
			if(!AddCreature(cre->m_pet,pos,dir)) 
			{
				ERR("CRegionScene::AddCreature: user add pet to scene error");
				return false;
			}
			//g_sendCmd->BeginWrite();
			//g_sendCmd->WriteShort(r2c_active_pet);
			//g_sendCmd->WriteLong(cre->m_pet->m_oId);
			//g_sendCmd->WriteLong(cre->m_dwPetSave);
			//cre->RecvCmd(g_sendCmd);
		}
		else if (cre->IsUser() && cre->m_pItemContainer)
		{
			SItemID pet = cre->m_pItemContainer->m_PetBag.GetItemFromID(cre->m_dwPetSave);
			if (ItemIsPet(pet))
				cre->ReleasePet(pet);
		}
	}
	if(cre->m_hero)
	{
		if(!AddCreature(cre->m_hero,pos,dir)) 
		{
			ERR("CRegionScene::AddCreature: user add hero to scene error");
			return false;
		}
	}

	m_pTerrain->GetBlockByPos((float)pos[0], (float)pos[1], bx, by);
	if (bx>=0 && bx<m_pTerrain->GetBlockCntX() && by>=0 && by<m_pTerrain->GetBlockCntY())
	{
		cre->m_iCurrentBlockX = bx;
		cre->m_iCurrentBlockY = by;
        cre->OnBlockChanged(m_pTerrain->GetBlockMap(bx-m_pTerrain->GetCenterX(), by-m_pTerrain->GetCenterY()), NULL);
        cre->OnPositionChanged((float)pos[0], (float)pos[1]);
	}

	if (cre->m_ai)
		cre->m_ai->OnBorn(cre);

	// ...
	AddObject(cre,cre->m_pos);

	if(cre->IsUser())
	{
		OnUserEnter(cre->m_userInfo);
		g_AntiRobot.OnAfterAddUser(cre->m_userInfo);
	}

	return true;
}

CRegionCreature *CRegionScene::RemoveCreature(ULONG oId)
{
	TObjectHash::iterator it;
	it = m_objectHash.find(oId);
	if(it == m_objectHash.end())
		return NULL;

	CRegionObject *object = (*it).second;
    if( OB_TYPE(object) != OBJECT_TYPE_CREATURE &&
        OB_TYPE(object) != OBJECT_TYPE_NPC_SERVICE &&
		OB_TYPE(object) != OBJECT_TYPE_NPC_COMBATIVE &&
		OB_TYPE(object) != OBJECT_TYPE_SPIRIT
		)
		return NULL;

	CRegionCreature *cre = (CRegionCreature*)object;
    cre->OnRemoveCreature();
    if (cre->m_pathNode)
    {
	    m_pPathFind->RemoveActor(cre->m_pathNode);
    }
	cre->m_scene = NULL;
	m_objectHash.erase(it);

	if(cre->m_pet)
	{
		cre->m_dwPetSave = cre->m_pet->m_dwPetItemOID;
		RemoveCreature(cre->m_pet->m_oId);
		//g_sendCmd->BeginWrite();
		//g_sendCmd->WriteShort(r2c_active_pet);
		//g_sendCmd->WriteLong(0);
		//cre->RecvCmd(g_sendCmd);
	}

	if(cre->m_hero)
	{
		RemoveCreature(cre->m_hero->m_oId);
	}

	// ...
	RemoveObject(cre,cre->m_pos);

	if(cre->IsUser())
	{
		OnUserLeave(cre->m_userInfo,0);
		g_AntiRobot.OnBeforeRemoveUser(cre->m_userInfo);
	}

	return cre;	
}

CRegionCreature *CRegionScene::FindUserByName(const char* username)
{
	TObjectHash::iterator it;
	for(it=m_objectHash.begin(); it!=m_objectHash.end(); it++)
	{
		CRegionObject *object = (*it).second;
		if (OB_TYPE(object) != OBJECT_TYPE_CREATURE)
		{
			continue;
		}
		CRegionCreature *tmp = (CRegionCreature*)object;
		if (tmp->m_userInfo && tmp->m_userInfo->m_username.c_str())
		{
			if (strcmp(tmp->m_userInfo->m_username.c_str(),username) == 0)
			{
				return (CRegionCreature*)object;
			}
		}
	}
	return NULL;
}

CRegionCreature *CRegionScene::FindCreature(ULONG oId)
{
	TObjectHash::iterator it;
	it = m_objectHash.find(oId);
	if(it == m_objectHash.end())
		return NULL;

	CRegionObject *object = (*it).second;
	if(OB_TYPE(object) != OBJECT_TYPE_CREATURE &&
	   OB_TYPE(object) != OBJECT_TYPE_NPC_SERVICE &&
	   OB_TYPE(object) != OBJECT_TYPE_NPC_COMBATIVE &&
	   OB_TYPE(object) != OBJECT_TYPE_SPIRIT
	   )
	   return NULL;

	CRegionCreature *tmp = (CRegionCreature*)object;

	if(tmp->m_userInfo)
	{
		if(!tmp->m_userInfo->m_active) return NULL;
	}
	return (CRegionCreature*)object;
}

void CRegionScene::FindNpcByType(ULONG npcType, std::vector<CRegionCreature*>& foundNpcs)
{
	for(TObjectHash::iterator it = m_objectHash.begin(); it != m_objectHash.end(); ++ it)
	{
		CRegionObject *object = (*it).second;
		if(OB_TYPE(object) != OBJECT_TYPE_CREATURE && OB_TYPE(object) != OBJECT_TYPE_NPC_SERVICE && OB_TYPE(object) != OBJECT_TYPE_NPC_COMBATIVE)
			continue;

		CRegionCreature *tmp = (CRegionCreature*)object;

		if(!tmp->m_npcInfo)
			continue;

		if(tmp->m_npcInfo->Id == npcType)
			foundNpcs.push_back(tmp);
	}
}

//
// item operate
//
bool CRegionScene::AddItem(CRegionItem *item,long *pos,char dir)
{
	item->m_dir = dir;
	item->m_pos[0] = pos[0];
	item->m_pos[1] = pos[1];
	item->m_pos[2] = pos[2];
    int rx, ry;
    m_pTerrain->GetTerrainTile(pos[0], pos[1], rx, ry);
	// R [12/9/2008 Louis.Huang]
    //item->m_pathNode = m_pPathFind->AddActor(item, rx, ry, false, false);
	item->m_pathNode = m_pPathFind->AddActor(item->GetObjectId(), item->m_type, rx, ry, false, PB_UNBLOCK);

	if (!item->m_pathNode)
	{
		return false;
	}

	item->m_sceneLiveTime = rtGetMilliseconds() + g_cfg.game.scene_item_live_time*1000;

	m_objectHash[item->m_oId] = item;

	// ...
	AddObject(item,pos);
	return true;
}

CRegionItem *CRegionScene::RemoveItem(ULONG oId)
{
	TObjectHash::iterator it;
	it = m_objectHash.find(oId);
	if(it == m_objectHash.end())
		return NULL;

	CRegionObject *object = (*it).second;
	if(OB_TYPE(object) != OBJECT_TYPE_ITEM)
		return NULL;

    m_pPathFind->RemoveActor(((CRegionItem*)object)->m_pathNode);
	m_objectHash.erase(it);

	// ...
	RemoveObject(object,((CRegionItem*)object)->m_pos);
	return (CRegionItem*)object;
}

CRegionItem *CRegionScene::FindItem(ULONG oId)
{
	TObjectHash::iterator it;
	it = m_objectHash.find(oId);
	if(it == m_objectHash.end())
		return NULL;

	CRegionObject *object = (*it).second;
	if(OB_TYPE(object) != OBJECT_TYPE_ITEM)
		return NULL;

	return (CRegionItem*)object;
}

#define COMPRESS_SNAPSHOT

#ifdef COMPRESS_SNAPSHOT
    //static char s_cmdCompBuf[32*1024];
	extern G_MEMDEF(s_cmdCompBuf, 32*1024)
#endif

const int MAX_SNAPSHOT_OBJECT_SIZE = 50;
bool CRegionScene::BuildSnapshot(CG_CmdPacket *pPacket, CRegionCreature *cre)
{
#ifdef COMPRESS_SNAPSHOT
    m_cmdTmp.BeginWrite();
    CG_CmdPacket *cmd = &m_cmdTmp;
#else
    CG_CmdPacket *cmd = pPacket;
#endif

	cmd->WriteLong(cre->m_pos[0]);
	cmd->WriteLong(cre->m_pos[1]);

	CRegionObject *ob;
	CRegionCreature *tmp;
	CRegionItem *item;
	int ObjectNum = 0;

	long timenow = rtGetMilliseconds();

	// performance test
	// 10000 = 500(ms)
	// 1000  = 50(ms)
	// for(int i=0; i<10000; i++)
	//	ObjectNum = GetAroundCreatureByTile(cre->m_pos, DEFAULT_RANGE, NULL, PT_ALL, false, MAX_SNAPSHOT_OBJECT_SIZE);
	
	// performance test
	// 10000	= 0(ms)
	// 100000   = 50(ms)
	// for(int i=0; i<100000; i++)
    CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
	ObjectNum = GetAroundObjectByGrid(pFindCreature, cre->m_pos, DEFAULT_RANGE ,MAX_SNAPSHOT_OBJECT_SIZE);
	for(int i=0; i<ObjectNum; i++)
    {
        if(cmd->IsError()) 
            return false;
        ob = pFindCreature[i];
        if( OB_TYPE(ob) == OBJECT_TYPE_CREATURE  || 
            OB_TYPE(ob) == OBJECT_TYPE_NPC_SERVICE  ||
			OB_TYPE(ob) == OBJECT_TYPE_NPC_COMBATIVE ||
			OB_TYPE(ob) == OBJECT_TYPE_SPIRIT
			)
        {
            tmp = (CRegionCreature*)ob;

            // if(tmp == cre || tmp == cre->m_pet) continue;
            // if(tmp == cre) continue;

			bool sender = true;

			if (tmp->m_IsHiding == 1)
			{
				sender = false;
				if( cre->m_pTeam->mData.Size() && tmp->m_pTeam->mData.Size() )
				{
					if (cre->m_pTeam->mData.GetCaptain() == tmp->m_pTeam->mData.GetCaptain())
						sender = true;
				}
				if (cre == tmp)
				{
					sender = true;
				}
			}

			if ( !sender ) continue;

			if(tmp->Distance(cre->m_pos) > DEFAULT_RANGE) continue;

			// if is a pet,continue
			if(tmp->m_master && tmp->m_master->m_pet == tmp)
				continue;

			// if it is not VISIBLE, and "cre" is not VISIBLE, then do not send
			if (cre->State_IsVisible())
			{
				if (!tmp->State_IsVisible())
				{
					continue;
				}
			}
			// if has pet,write pet info first
			if(tmp->m_pet && tmp->m_pet->State_IsVisible() )
			{
				tmp->m_pet->WriteSnapshot(cmd,cre);
			}
			else if (tmp->m_pet && !tmp->m_pet->State_IsVisible() && tmp == cre)
			{
				tmp->m_pet->WriteSnapshot(cmd,cre);
			}
			// if has hero,write pet info first
			if(tmp->m_hero)
			{
				tmp->m_hero->WriteSnapshot(cmd,cre);
			}
			tmp->WriteSnapshot(cmd,cre);
		}
        else if(OB_TYPE(ob) == OBJECT_TYPE_ITEM)
        {
            item = (CRegionItem*)ob;
            if(timenow >= item->m_hideTime)
            {
                item->WriteSnapshot(cmd,cre);
            }
        }
    }

    // 克隆和压缩
#ifdef COMPRESS_SNAPSHOT
    if(!pPacket->CloneBitFrom(&m_cmdTmp))
		ERR("clone packet bit error\n");

    char* pPData = m_cmdTmp.GetByteData();
    int k, j=m_cmdTmp.GetByteDataSize()/4;
    for (k=0; k<j; k++)
    {
        ((long*)pPData)[k] ^= 0x968B78D6;
    }
	int iOutLen = RtZCompress::Compress(pPData, m_cmdTmp.GetByteDataSize(), s_cmdCompBuf);

    pPacket->WriteLong(m_cmdTmp.GetByteDataSize());
    pPacket->WriteBinary(s_cmdCompBuf, iOutLen);
#endif

	return !pPacket->IsError();
}

//
// only use can recv cmd
//
void CRegionScene::BroadcastCmd(long *pos,long range,CG_CmdPacket *cmd,CRegionCreature *exclude)
{
	CRegionObject *ob;
	CRegionCreature *tmp;

    CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
	// int ObjectNum = GetAroundCreatureByTile(pos, range, exclude, PT_PATH, false, MAX_SNAPSHOT_OBJECT_SIZE);
	int ObjectNum = GetAroundObjectByGrid(pFindCreature, pos, range, MAX_SNAPSHOT_OBJECT_SIZE);
	for (int i=0; i<ObjectNum; i++)
	{
		ob = pFindCreature[i];
		if(ob == exclude) continue;
		if( (OB_TYPE(ob) == OBJECT_TYPE_CREATURE) ||
//-----------------add start by tony 05.05.19--------------------------------//
			(OB_TYPE(ob) == OBJECT_TYPE_NPC_COMBATIVE)
//-----------------add start by tony 05.05.19--------------------------------//
			)
		{
			tmp = (CRegionCreature*)ob;
			if(!tmp->IsUser()) continue;
			if(tmp->Distance(pos) > DEFAULT_RANGE) continue;
			tmp->RecvCmd(cmd);
		}
	}
}

//
// all creature can recv msg
//
void CRegionScene::BroadcastMsg(long *pos,long range,const char *msg,
								const char *param,CRegionCreature *exclude)
{
	CRegionObject *ob;
	CRegionCreature *tmp;

	// int ObjectNum = GetAroundCreatureByTile(pos, range, exclude, PT_PATH, false, MAX_SNAPSHOT_OBJECT_SIZE);
    CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
	int ObjectNum = GetAroundObjectByGrid(pFindCreature, pos, range, MAX_SNAPSHOT_OBJECT_SIZE);

	for (int i=0; i<ObjectNum; i++)
	{
		ob = pFindCreature[i];
		if( (OB_TYPE(ob) == OBJECT_TYPE_CREATURE) ||
//---------------add start by tony 05.05.19----------------------------------//
			(OB_TYPE(ob) == OBJECT_TYPE_NPC_COMBATIVE)
//---------------add end   by tony 05.05.19----------------------------------//
			)
		{
			tmp = (CRegionCreature*)ob;
			if(tmp->Distance(pos) > DEFAULT_RANGE) continue;
			tmp->RecvMsg(msg,param);
		}
	}
}

bool CRegionScene::LoadSceneFile(const char *file)
{
	char tmp[128];
	rt2_sprintf(tmp,"scene/scene01/%s.rts",file);
    char* ext = strrchr(tmp, '.');

	m_pTerrain = RT_NEW RtsSceneDoc;
    if (!m_pTerrain->LoadMap(tmp, true))
    {
        ERR1("CRegionScene: load scene file [%s] failed.\n", tmp);
    }

	string areaPkInfo(tmp, ext-tmp);
    areaPkInfo += ".rsr";
    m_pTerrain->AddAreaInfo(areaPkInfo.c_str());
	m_pPathFind->SetBindTerrain(m_pTerrain);

	string areaFileName(tmp, ext-tmp);
	areaFileName += "_monster.rsr";
	m_pArea = RT_NEW RtsSceneArea;
	if (!m_pArea->LoadFromFile(areaFileName.c_str()))
	{
		// ERR1("CRegionScene: load scene monster file [%s] failed.\n", areaFileName.c_str());
	}
	
	BuildGrid();

	m_sceneName = file;

	// Don't create npc here, user script instead. [8/27/2009 Louis.Huang]
	//CreateAllNpc();

	// trigger event oncreate
	// OnCreate();
	return true;
}

void CRegionScene::GetAroundCreatureByGrid(long *pos,long range, vector<CRegionCreature*>& creList,CRegionCreature *exclude)
{
	CRegionObject *ob;
	CRegionCreature *tmp;

    CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
	int ObjectNum = GetAroundObjectByGrid(pFindCreature, pos, range, MAX_SNAPSHOT_OBJECT_SIZE);
	for (int i=0; i<ObjectNum; i++)
	{
		ob = pFindCreature[i];
		if(ob == exclude) continue;
		if( (OB_TYPE(ob) == OBJECT_TYPE_CREATURE) ||
			//-----------------add start by tony 05.05.19--------------------------------//
			(OB_TYPE(ob) == OBJECT_TYPE_NPC_COMBATIVE)
			//-----------------add start by tony 05.05.19--------------------------------//
			)
		{
			tmp = (CRegionCreature*)ob;
			if(tmp->Distance(pos) > DEFAULT_RANGE) continue;
			if(pFindCreature[i])
				creList.push_back((CRegionCreature*)pFindCreature[i]);
		}
    }
}

int CRegionScene::GetPathContaniner(RtSceneBlockTerrain::STileAttr* pAttr,
									CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT],
									RtsPathPos* pPathNode,
									CRegionCreature *exclude, 
									char cType,
									bool bBlock,
									int iCnt,
									int iMaxFind)
{
	if (pAttr && pAttr->bWalk==0 && pAttr->pContainer) 
	{ 
		pPathNode = pAttr->pContainer->pNodes; 
		while (pPathNode) 
		{ 
			if (bBlock) 
			{ 
				if (!pPathNode->cBlock) 
				{ 
					pPathNode = pPathNode->pNext; 
					continue; 
				}
			}

			if (pPathNode->cType & cType) 
			{ 
				// R [12/9/2008 Louis.Huang]
/*
				if (exclude!=((CRegionCreature*)(pPathNode->pData))) 
				{
					if( ((CRegionObject*)(pPathNode->pData))->m_type == OBJECT_TYPE_CREATURE || 
						((CRegionObject*)(pPathNode->pData))->m_type == OBJECT_TYPE_NPC_COMBATIVE) 
					{ 
						pFindCreature[iCnt++] = ((CRegionObject*)(pPathNode->pData)); 
						if (iCnt>=iMaxFind) return iCnt; 
					} 
				} 
*/
				if (exclude->GetObjectId()!=pPathNode->dwActorId)
				{
					if( (pPathNode->cType == OBJECT_TYPE_CREATURE) || 
						(pPathNode->cType == OBJECT_TYPE_NPC_COMBATIVE))
					{ 
						pFindCreature[iCnt++] = FindCreature(pPathNode->dwActorId); 
						if (iCnt>=iMaxFind) return iCnt; 
					} 
				} 

			} 
			pPathNode = pPathNode->pNext; 
		} 
	}
}

#define GET_PATH_CONTAINER  \
    if (pAttr && pAttr->bWalk==0 && pAttr->pContainer) \
    { \
        pPathNode = pAttr->pContainer->pNodes; \
        while (pPathNode) \
        { \
            if (bBlock) \
            { \
                if (!pPathNode->cBlock) \
                { \
                    pPathNode = pPathNode->pNext; \
                    continue; \
                } \
            } \
            if (pPathNode->cType & cType) \
            { \
				DWORD dwExcludeId = 0; \
				if (exclude) dwExcludeId = exclude->GetObjectId(); \
                if (dwExcludeId!=pPathNode->dwActorId) \
                { \
					if( (pPathNode->cType == OBJECT_TYPE_CREATURE) || \
						(pPathNode->cType == OBJECT_TYPE_NPC_COMBATIVE)) \
						{ \
							CRegionCreature* pCreature = FindCreature(pPathNode->dwActorId); \
							if (pCreature != NULL) \
								pFindCreature[iCnt++] =  pCreature; \
							if (iCnt>=iMaxFind) return iCnt; \
						} \
                } \
            } \
            pPathNode = pPathNode->pNext; \
        } \
    }

int CRegionScene::GetAroundCreatureByTile(CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], long *pos, long range, CRegionCreature *exclude, char cType, bool bBlock, int iMaxFind)
{
    int iCnt=0;
    int x, y, j;
    int iTerrainX, iTerrainY;
    RtSceneBlockTerrain::STileAttr* pAttr;
    RtsPathPos* pPathNode = NULL;
    int iFindRange = range/g_fSceneTerrainTileWidth;
    m_pTerrain->GetTerrainByPosFast(pos[0], pos[1], iTerrainX, iTerrainY);

    if (iMaxFind>SCENE_MAX_FIND_OBJECT)
    {
        iMaxFind = SCENE_MAX_FIND_OBJECT;
    }

    // 当前这点
    pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX,iTerrainY);
	// R [12/8/2008 Louis.Huang]
    GET_PATH_CONTAINER
	//GetPathContaniner(pAttr,pFindCreature,pPathNode,exclude, cType,bBlock,iCnt,iMaxFind);//tim.yang
	if (iCnt>=iMaxFind) return iCnt; 
    // 周围
    for (j=1; j<iFindRange; j++)
    {
        y = -j;
        for(x=-j;x<=j;x++)
        {
            pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX+x,iTerrainY+y);
			// R [12/8/2008 Louis.Huang]
			GET_PATH_CONTAINER
			//GetPathContaniner(pAttr,pFindCreature,pPathNode,exclude, cType,bBlock,iCnt,iMaxFind);//tim.yang
			if (iCnt>=iMaxFind) return iCnt; 
        }
        y = j;
        for(x=-j;x<=j;x++)
        {
            pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX+x,iTerrainY+y);
			// R [12/8/2008 Louis.Huang]
			GET_PATH_CONTAINER
			//GetPathContaniner(pAttr,pFindCreature,pPathNode,exclude, cType,bBlock,iCnt,iMaxFind);//tim.yang
			if (iCnt>=iMaxFind) return iCnt; 
        }
        x = -j;
        for(y=-j+1;y<j;y++)
        {
            pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX+x,iTerrainY+y);
			// R [12/8/2008 Louis.Huang]
			GET_PATH_CONTAINER
			//GetPathContaniner(pAttr,pFindCreature,pPathNode,exclude, cType,bBlock,iCnt,iMaxFind);//tim.yang
			if (iCnt>=iMaxFind) return iCnt; 
        }
        x = j;
        for(y=-j+1;y<j;y++)
        {
            pAttr = m_pTerrain->GetTerrainPathAttr(iTerrainX+x,iTerrainY+y);
			// R [12/8/2008 Louis.Huang]
			GET_PATH_CONTAINER
			//GetPathContaniner(pAttr,pFindCreature,pPathNode,exclude, cType,bBlock,iCnt,iMaxFind);//tim.yang
			if (iCnt>=iMaxFind) return iCnt; 
        }
    }
    return iCnt;
}

#if 0
bool CRegionScene::AddMonsterLair(DWORD spawnInterval,short npcId,long pos[3],char dir)
{
	SNpc *npc = g_TableNpc.FindNpc(npcId);
	if(!npc)
	{
		LOG1("CRegionScene::AddMonsterLair: can't find npc = %d.\n",npcId);
		return false;
	}

	SMonsterLair *lair = RT_NEW SMonsterLair;
	lair->LastSpawnTime = 0;
	lair->SpawnInterval = spawnInterval;
	lair->NpcId = npcId;
	lair->Position[0] = pos[0];
	lair->Position[1] = pos[1];
	lair->Position[2] = pos[2];
	lair->Dir = dir;
	lair->Monster = NULL;
	m_lairList.push_back(lair);
	return true;
}
#endif

void CRegionScene::OnTimer(ULONG id, DWORD dwParam[])
{
	CRegionObject::OnTimer(id, dwParam);
	switch(id)
	{
		case SCENE_TIMER_SPAWN_MONSTER:
			g_spawnMgr.Spawn();
			break;
		case SCENE_TIMER_ITEM:
			ItemProcess();
			break;
//---------------------add start by tony 05.05.23----------------------------//
		case SCENE_TIMER_GUARDER:
			g_GuarderMgr.CreateGuarder();
			break;
		case SCENE_TIMER_CASTLEWAR:
			g_RegionCastleWarMgr.OnTimer();
			break;
		case SCENE_TIMER_WARFIELD:
			g_WarfieldMgr.Run();
			break;
		case SCENE_TIMER_WARFIELD_SPAWN:
			if(g_WarfieldMgr.m_pSpawnMgr)
			{
				g_WarfieldMgr.m_pSpawnMgr->Spawn();
			}
			break;
//---------------------add end   by tony 05.05.23----------------------------//
	}
}

CRegionCreature* CRegionScene::AddNpc(SSceneNpc& npcInfo, long lLevel, float fAtt[])
{
	CRegionCreature *cre = g_region->CreateNpc(npcInfo.sNpcId);
	if(!cre)
		return 0;

	if(lLevel <= 0)
		lLevel = 1;
	cre->m_core.Lev = lLevel;
	SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(lLevel));
	if(base)
	{
		cre->m_core.Attack.Base = base->FAttack;
		cre->m_core.DamageMin.Base = 0.9f * base->FDamage;
		cre->m_core.DamageMax.Base = 1.1f * base->FDamage;
		cre->m_core.Dodge.Base = base->FDodge;
		cre->m_core.Armor.Base = base->FArmor;
		cre->m_core.MaxHp.Base = base->FHp;
		cre->m_core.MaxMp.Base = base->FMp;
	}
	if(fAtt && fAtt[0] > 0)
	{
		if(fAtt[1] > 0)
			cre->m_core.Attack.Base = fAtt[1];
		if(fAtt[2] > 0)
		{
			cre->m_core.DamageMin.Base = 0.9f * fAtt[2];
			cre->m_core.DamageMax.Base = 1.1f * fAtt[2];
		}
		if(fAtt[3] > 0)
			cre->m_core.Dodge.Base = fAtt[3];
		if(fAtt[4] > 0)
			cre->m_core.Armor.Base = fAtt[4];
		if(fAtt[5] > 0)
			cre->m_core.MaxHp.Base = fAtt[5];
		if(fAtt[6] > 0)
			cre->m_core.MaxMp.Base = fAtt[6];
		if(fAtt[7] > 0)
		{
			cre->m_core.Exp = fAtt[7];
			cre->m_killReward.Exp = fAtt[7];
		}
	}
	cre->m_core.SetHp(cre->m_core.MaxHp.Base);
	cre->m_core.SetMp(cre->m_core.MaxMp.Base);

	// Record point shop npc object id here. [8/27/2009 Tim.Yang]
	if (1314 == npcInfo.sNpcId)
	{
		g_region->m_shopnpcoid = cre->GetObjectId();
	}

	if(!AddCreature(cre,npcInfo.pos,npcInfo.cDir))
	{
		g_factory->DestroyObject(cre);
		return 0;
	}
	else
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	return cre;
}

// ----------------------------------------------------------------------------
// scene event 
// ----------------------------------------------------------------------------
void CRegionScene::OnCreate()
{
	if(IsScriptLoaded())
		CallScript("OnCreate", false);
}

void CRegionScene::OnRun()
{
	if(IsScriptLoaded())
		CallScript("OnRun", true);
}

void CRegionScene::OnDestroy()
{
	if(IsScriptLoaded())
		CallScript("OnDestroy", false);

	//if (g_region->m_eState == CRegionServer::ssShutdown)
	//{
	//	//程序已经退出，这里再走下去就会遭遇大量已经delete的指针
	//	return;
	//}

	// Remove and destroy all object in this scene.
	TObjectList creList,itemList;
	TObjectList::iterator obIt;
	
	TObjectHash::iterator iter = m_objectHash.begin();
	for (; iter!=m_objectHash.end(); iter++)
	{
		CRegionObject *ob = iter->second;
		if(OB_TYPE(ob)==OBJECT_TYPE_CREATURE ||
		   OB_TYPE(ob)==OBJECT_TYPE_NPC_SERVICE ||
		   OB_TYPE(ob)==OBJECT_TYPE_NPC_COMBATIVE)
		{
			creList.push_back(ob);
			((CRegionCreature*)ob)->DeleteAI();
		}
		else if(OB_TYPE(ob)==OBJECT_TYPE_ITEM)
		{
			itemList.push_back(ob);
		}
	}
	
	for(obIt=creList.begin(); obIt!=creList.end(); obIt++)
	{
		RemoveCreature((*obIt)->m_oId);
		g_factory->DestroyObject(*obIt);
	}
	for(obIt=itemList.begin(); obIt!=itemList.end(); obIt++)
	{
		RemoveItem((*obIt)->m_oId);
		g_factory->DestroyObject(*obIt);
	}
}

void CRegionScene::OnUserEnter(CRegionUser *user)
{
	m_userHash[user->m_userId] = user->m_dummy;
}

void CRegionScene::OnUserLeave(CRegionUser *user,int reason)
{
	m_userHash.erase(user->m_userId); 
}

bool CRegionScene::OnTryAddUser(CRegionUser *user)
{
	return true;
}

void CRegionScene::OnCreatureDead(CRegionCreature *user,CRegionCreature *killer)
{
	//send to reborn user rige info
	if(killer->IsUser())
	{
// 		if(killer->m_iRigelevel > 2)
// 		{
// 			killer->SendSystemMessage(R(MSG_RAGE_FULL));
// 			return;
// 		}
// 		++killer->m_iKillMonsterNum;
// 		if(killer->m_iKillMonsterNum >= RAGE_LEVELUP_EXP)
// 		{
// 			++killer->m_iRigelevel;
// 			killer->m_iKillMonsterNum = 0;
// 		}

	// 	CG_CmdPacket* packet = RT_NEW CG_CmdPacket;
 		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_killmonster_addrige);
		g_sendCmd->WriteLong(killer->m_iRigelevel);
		g_sendCmd->WriteLong(killer->m_iKillMonsterNum);
		killer->RecvCmd(g_sendCmd);
		//end rige info
	}
}

void CRegionScene::ItemProcess()
{
	// 清除超时的道具
	// 道具的主人是否清0

	int currTick = rtGetMilliseconds();
	vector<DWORD> arrDeadItem;

	TObjectHash::iterator iter = m_objectHash.begin();
	for (; iter!=m_objectHash.end(); iter++)
	{
		CRegionObject *object = (*iter).second;
		if(OB_TYPE(object) == OBJECT_TYPE_ITEM)
		{
			CRegionItem *pRItem = (CRegionItem*)object;
			if (pRItem->m_sceneLiveTime < currTick)
			{
				arrDeadItem.push_back(pRItem->m_oId);
			}
		}
	}

	for (int i=0; i<(int)arrDeadItem.size(); i++)
	{
		CRegionItem *rgItem = RemoveItem(arrDeadItem[i]);
		if(rgItem)
			g_factory->DestroyObject(rgItem);
	}
}


RtsSceneBlockRtb* CRegionScene::FindBlockByFileName(const char* filename)
{
    if (filename==NULL || filename[0]==0) return NULL;

	int cx = m_pTerrain->GetBlockCntX();
	int cy = m_pTerrain->GetBlockCntY();

	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			RtsSceneBlockRtb* bmap = m_pTerrain->GetBlockMap(x-m_pTerrain->GetCenterX(), y-m_pTerrain->GetCenterY());

            if (bmap && (strcmp(bmap->m_szFileName, filename) == 0))
				return bmap;
		}
	}

	return NULL;
}

RtsSceneBlockRtb* CRegionScene::FindBlockByPos(const long* pos)
{
	int ibx, iby;

	m_pTerrain->GetBlockByPos(pos[0] ,
		pos[1],
		ibx, iby);

	ibx -= m_pTerrain->GetCenterX();
	iby -= m_pTerrain->GetCenterY();

	return m_pTerrain->GetBlockMap(ibx, iby);
}

const int GRID_SIZE = 150;
void CRegionScene::BuildGrid()
{
	m_maxX = (m_pTerrain->GetBlockCntX()-m_pTerrain->GetCenterX()+1) * g_fSceneBlockWidth;
	m_minX = (-m_pTerrain->GetCenterX()-1) * g_fSceneBlockWidth;

	m_maxY = (m_pTerrain->GetBlockCntY()-m_pTerrain->GetCenterY()+1) * g_fSceneBlockWidth;
	m_minY = (-m_pTerrain->GetCenterY()-1) * g_fSceneBlockWidth;

	m_gridxNum = (m_maxX-m_minX)/GRID_SIZE+1;
	m_gridyNum = (m_maxY-m_minY)/GRID_SIZE+1;
	LOG1("grid num = %d\n",m_gridxNum*m_gridyNum);

    /*
	m_gridObj = RT_NEW CRegionObject*[m_gridxNum*m_gridyNum];
	for(int i=0; i<m_gridxNum*m_gridyNum; i++)
	{
		m_gridObj[i] = NULL;
	}
    */
	m_gridObj = RT_NEW TGridObj[m_gridxNum*m_gridyNum];
}

bool CRegionScene::GetGridByPos(long *pos,int &x,int &y)
{
	if(pos[0]<m_minX || pos[0]>m_maxX) return false;
	if(pos[1]<m_minY || pos[1]>m_maxY) return false;

	x = (pos[0]-m_minX)/GRID_SIZE;
	y = (pos[1]-m_minY)/GRID_SIZE;
	
	return true;
}

bool CRegionScene::AddObject(CRegionObject *obj,long *pos)
{
	int x,y;
	if(!GetGridByPos(pos,x,y)) return false;

	obj->m_gridX = x;
	obj->m_gridY = y;
    /*
	obj->m_prevObj = NULL;
	obj->m_nextObj = m_gridObj[x+y*m_gridxNum];
	if(m_gridObj[x+y*m_gridxNum])
		m_gridObj[x+y*m_gridxNum]->m_prevObj = obj;
	m_gridObj[x+y*m_gridxNum] = obj;
    */
    m_gridObj[x+y*m_gridxNum].push_back(obj);

	return true;
}

bool CRegionScene::RemoveObject(CRegionObject *obj,long *pos)
{
	// int x,y;
	// if(!GetGridByPos(pos,x,y)) return false;
	if(obj->m_gridX<0 || obj->m_gridY<0) return false;

    /*
	if(obj->m_nextObj)
		obj->m_nextObj->m_prevObj = obj->m_prevObj;

	if(obj->m_prevObj)
		obj->m_prevObj->m_nextObj = obj->m_nextObj;
	else
		m_gridObj[obj->m_gridX+obj->m_gridY*m_gridxNum] = obj->m_nextObj;
    */
#ifdef _DEBUG
    TGridObj* pGridObj;
    pGridObj = m_gridObj + obj->m_gridX+obj->m_gridY*m_gridxNum;
    pGridObj->remove(obj);
#else
    m_gridObj[obj->m_gridX+obj->m_gridY*m_gridxNum].remove(obj);
#endif

	return true;
}

/*
#define GET_GRID_OBJECT 	int tmpx = x + basex;\
							int tmpy = y + basey;\
							if(tmpx<0 || tmpx>=m_gridxNum || tmpy<0 || tmpy>=m_gridyNum) break;\
							baseObj = m_gridObj[tmpx+tmpy*m_gridxNum];\
							while(baseObj&&iCnt<maxFind)\
							{\
								pFindCreature[iCnt++] = baseObj;\
								baseObj = baseObj->m_nextObj;\
							}

int CRegionScene::GetAroundObjectByGrid(CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], long *pos, long range, int maxFind)
{
	int j,x,y,iFindRange,iCnt = 0,basex,basey;
	iFindRange = range / GRID_SIZE + 1;
	
	CRegionObject *baseObj;
	if(!GetGridByPos(pos,basex,basey)) return 0;

	x = 0;
	y = 0;
	do
	{
		GET_GRID_OBJECT
	}while(0);
	
	for (j=1; j<iFindRange; j++)
	{
		y = -j;
		for(x=-j;x<=j;x++)
		{
			GET_GRID_OBJECT
		}
		y = j;
		for(x=-j;x<=j;x++)
		{
			GET_GRID_OBJECT
		}
		x = -j;
		for(y=-j+1;y<j;y++)
		{
			GET_GRID_OBJECT
		}
		x = j;
		for(y=-j+1;y<j;y++)
		{
			GET_GRID_OBJECT
		}
	}
	return iCnt;
}
*/

/*
inline int RegionSceneGetGridObject(int x, int y, int basex, int basey, CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], int& iCnt)
{
    tmpx = x + basex;
    tmpy = y + basey;
    if(tmpx<0 || tmpx>=m_gridxNum || tmpy<0 || tmpy>=m_gridyNum)
        break;
    TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
    for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
    {
        pFindCreature[iCnt++] = baseObj;
    }
}
*/

int CRegionScene::GetAroundObjectByGrid(CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT], long *pos, long range, int maxFind)
{
	int j,x,y,iFindRange,iCnt = 0,basex,basey,tmpx,tmpy;
	iFindRange = range / GRID_SIZE + 1;

    TGridObj::iterator baseObjIt;
	if(!GetGridByPos(pos,basex,basey)) return 0;

	x = 0;
	y = 0;
	do
	{
        tmpx = x + basex;
        tmpy = y + basey;
        if(tmpx<0 || tmpx>=m_gridxNum || tmpy<0 || tmpy>=m_gridyNum)
            break;
        TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
        for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
        {
            pFindCreature[iCnt++] = *baseObjIt;
        }
	}while(0);

	for (j=1; j<iFindRange && iCnt<maxFind; j++)
	{
		y = -j;
        tmpy = y + basey;
        if(tmpy>=0 && tmpy<m_gridyNum)
        {
		    for(x=-j;x<=j && iCnt<maxFind;x++)
		    {
                tmpx = x + basex;
                if(tmpx>0 && tmpx<m_gridxNum)
                {
                    TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
                    for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
                    {
                        pFindCreature[iCnt++] = *baseObjIt;
                    }
                }
		    }
        }
		y = j;
        tmpy = y + basey;
        if(tmpy>=0 && tmpy<m_gridyNum)
        {
		    for(x=-j;x<=j && iCnt<maxFind;x++)
		    {
                tmpx = x + basex;
                if(tmpx>0 && tmpx<m_gridxNum)
                {
                    TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
                    for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
                    {
                        pFindCreature[iCnt++] = *baseObjIt;
                    }
                }
		    }
        }
		x = -j;
        tmpx = x + basex;
        if(tmpx>0 && tmpx<m_gridxNum)
        {
		    for(y=-j+1;y<j && iCnt<maxFind;y++)
		    {
                tmpy = y + basey;
                if(tmpy>=0 && tmpy<m_gridyNum)
                {
                    TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
                    for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
                    {
                        pFindCreature[iCnt++] = *baseObjIt;
                    }
                }
		    }
        }
		x = j;
        tmpx = x + basex;
        if(tmpx>0 && tmpx<m_gridxNum)
        {
		    for(y=-j+1;y<j && iCnt<maxFind;y++)
		    {
                tmpy = y + basey;
                if(tmpy>=0 && tmpy<m_gridyNum)
                {
                    TGridObj& curGridObj = m_gridObj[tmpx+tmpy*m_gridxNum];
                    for (baseObjIt=curGridObj.begin(); baseObjIt!=curGridObj.end() && iCnt<maxFind; baseObjIt++)
                    {
                        pFindCreature[iCnt++] = *baseObjIt;
                    }
                }
		    }
        }
	}
	return iCnt;
}

void CRegionScene::AddTeam(int key, int num)
{
	return;
}

void CRegionScene::RemoveTeam(int key, int num)
{
	return;
}

int CRegionScene::GetTeamNum(int key)
{
	return 1;
}

// ----------------------------------------------------------------------------
// fb scene
// ----------------------------------------------------------------------------
CRegionSceneFB::CRegionSceneFB()
{

}

CRegionSceneFB::~CRegionSceneFB()
{

}

//CRegionCreature* CRegionSceneFB::AddNpc(SSceneNpc& npcInfo, long lLevel, float fAtt[])
//{
//
//	if (m_curDif < npcInfo.sDif)
//		return 0;
//
//	CRegionCreature *cre = g_region->CreateNpc(npcInfo.sNpcId);
//	if(!cre)
//		return 0;
//	
//	float fbRate = 1 + (m_curDif-1)*0.25;
//	float fbHpRate = 1 + (m_curDif-1)*0.25;
//
//
//	float npcRate = 1 + (cre->m_npcInfo->Dif/2.0f);
//	float npcDmRate = 1 + (cre->m_npcInfo->Dif*0.25);
//	float hpRate = 6*1.5;
//
//
//	cre->m_core.Lev = m_curLev + cre->m_npcInfo->Dif*3;
//	if(cre->m_npcInfo->Boss)
//		cre->m_core.Lev = m_curLev + 10;
//
//	SBaseValue *base = g_TableBaseValue.Find(lLevel > 0 ? lLevel : GetRealLevel(cre->m_core.Lev));
//
//	// reset npc level
//	cre->m_core.Lev = m_curLev;
//	if(base)
//	{
//		if(fAtt)
//		{
//			if(fAtt[0] > 0)
//				base->FAttack = fAtt[0];
//			if(fAtt[1] > 0)
//				base->FDamage = fAtt[1];
//			if(fAtt[2] > 0)
//				base->FDodge = fAtt[2];
//			if(fAtt[3] > 0)
//				base->FArmor = fAtt[3];
//			if(fAtt[4] > 0)
//				base->FHp = fAtt[4];
//			if(fAtt[5] > 0)
//				base->FMp = fAtt[5];
//		}
//
//		cre->m_core.DamageMin.Base = 0.9f * base->FDamage * 1.5 / (cre->m_core.AttSpeed.GetValue()/100.0f) * fbRate;
//		cre->m_core.DamageMax.Base = 1.1f * base->FDamage * 1.5 / (cre->m_core.AttSpeed.GetValue()/100.0f) * fbRate;
//		cre->m_core.Attack.Base    = base->FAttack;
//		cre->m_core.Armor.Base     = base->FArmor; 
//		cre->m_core.Dodge.Base     = base->FDodge;
//		cre->m_killReward.Exp      = (20+5*(cre->m_core.Lev-1)*(1+0.05*(cre->m_core.Lev-1)))* fbRate;
//		cre->m_core.CriticalHit.Base = 20;
//
//
//		if(cre->m_npcInfo->Boss)
//		{
//			cre->m_core.DamageMin.Base *= npcDmRate;
//			cre->m_core.DamageMax.Base *= npcDmRate;
//			cre->m_core.MaxHp.Base      = cre->m_npcInfo->Hp* fbRate*npcRate;
//			cre->m_core.MaxMp.Base      = cre->m_npcInfo->Mp* fbRate;
//			cre->m_killReward.Exp      *= npcRate;
//		}
//		else
//		{
//			cre->m_core.MaxHp.Base     = cre->m_npcInfo->Hp *fbRate*npcRate;
//			cre->m_core.MaxMp.Base     = cre->m_npcInfo->Mp*fbRate;
//		}
//
//		if(cre->m_core.Lev > 10)
//		{
//			cre->m_core.ElementArmor[ELEMENT_WATER].Base  = (cre->m_core.Lev - 10) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_WATER];
//			cre->m_core.ElementArmor[ELEMENT_FIRE].Base   = (cre->m_core.Lev - 10) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_FIRE];
//			cre->m_core.ElementArmor[ELEMENT_POISON].Base = (cre->m_core.Lev - 10) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_POISON];
//		}		
//
//		cre->m_core.SetHp(cre->m_core.GetMaxHp());
//		cre->m_core.SetMp(cre->m_core.GetMaxMp());
//	}
//
//	if(!AddCreature(cre,npcInfo.pos,npcInfo.cDir))
//	{
//		g_factory->DestroyObject(cre);
//		return 0;
//	}
//	else
//	{
//		if(cre->IsScriptLoaded())
//			cre->CallScript("OnCreate", false);
//	}
//	return cre;
//}


CRegionCreature* CRegionSceneFB::AddNpc(SSceneNpc& npcInfo, long lLevel, float fAtt[])
{
	if (m_curDif < npcInfo.sDif)
		return 0;

	CRegionCreature *cre = g_region->CreateNpc(npcInfo.sNpcId);
	if(!cre)
		return 0;

	// 副本当前的全部用户平均等级为m_curUserLV，数值计算使用此变量
	if(lLevel <= 0)
		lLevel=1;
	float fbRate = 1 + lLevel*0.005;
	float fbHpRate = (1+((lLevel-1)+10)*5/1000)*(1+lLevel/20)*(1+0.5)*1.5*(lLevel-1+10);

	float npcRate = 1 + (cre->m_npcInfo->Dif*0.5);
	float npcDmRate = 1 + (cre->m_npcInfo->Dif*0.25);
	float hpRate = 6*1.5;

	cre->m_core.Lev = lLevel;
	SBaseValue *base = g_TableBaseValue.Find(lLevel > 0 ? GetRealLevel(lLevel) : GetRealLevel(cre->m_core.Lev));
	if(base)
	{
		if(fAtt[0] > 0)
		{
			if(fAtt[1] > 0)
				base->FAttack = fAtt[1];
			if(fAtt[2] > 0)
				base->FDamage = fAtt[2];
			if(fAtt[3] > 0)
				base->FDodge = fAtt[3];
			if(fAtt[4] > 0)
				base->FArmor = fAtt[4];
			if(fAtt[5] > 0)
				base->FHp = fAtt[5];
			if(fAtt[6] > 0)
				base->FMp = fAtt[6];
			if(fAtt[7] > 0)
				cre->m_core.Exp = fAtt[7];
			cre->m_core.DamageMin.Base = 0.9f * base->FDamage;
			cre->m_core.DamageMax.Base = 1.1f * base->FDamage;
			cre->m_core.Attack.Base    = base->FAttack;
			cre->m_core.Armor.Base     = base->FArmor; 
			cre->m_core.Dodge.Base     = base->FDodge;
			cre->m_core.MaxHp.Base     = base->FHp;
			cre->m_core.MaxMp.Base     = base->FMp;
			cre->m_killReward.Exp      = fAtt[7] > 0 ? (long)fAtt[7] : cre->m_npcInfo->Exp;
			cre->m_core.CriticalHit.Base = 20;
		}
		else
		{
			cre->m_core.DamageMin.Base = 0.9f * base->FDamage * 3.5/ (cre->m_core.AttSpeed.GetValue()/100.0f) * fbRate;
			cre->m_core.DamageMax.Base = 1.1f * base->FDamage * 3.5/ (cre->m_core.AttSpeed.GetValue()/100.0f) * fbRate;
			
			cre->m_core.ElementDamage[ELEMENT_WATER].Base = (base->FDamage/(cre->m_core.AttSpeed.GetValue()/100.0f))*(cre->m_npcInfo->ElementDamage[ELEMENT_WATER])* fbRate;
			cre->m_core.ElementDamage[ELEMENT_FIRE].Base = (base->FDamage/(cre->m_core.AttSpeed.GetValue()/100.0f))*(cre->m_npcInfo->ElementDamage[ELEMENT_FIRE])* fbRate;
			cre->m_core.ElementDamage[ELEMENT_POISON].Base = (base->FDamage/(cre->m_core.AttSpeed.GetValue()/100.0f))*(cre->m_npcInfo->ElementDamage[ELEMENT_POISON])* fbRate;

			
			cre->m_core.Attack.Base    = base->FAttack;
			cre->m_core.Armor.Base     = base->FArmor; 
			cre->m_core.Dodge.Base     = base->FDodge;
			cre->m_killReward.Exp      = (20+5*(cre->m_core.Lev-1)*(1+0.05*(cre->m_core.Lev-1)))* fbRate*2;
			cre->m_core.CriticalHit.Base = 8;

			if(cre->m_npcInfo->Boss)
			{
				cre->m_core.DamageMin.Base *= npcDmRate;
				cre->m_core.DamageMax.Base *= npcDmRate;

				cre->m_core.ElementDamage[ELEMENT_WATER].Base *=npcDmRate;
				cre->m_core.ElementDamage[ELEMENT_FIRE].Base *=npcDmRate;
				cre->m_core.ElementDamage[ELEMENT_POISON].Base *=npcDmRate;

				cre->m_core.MaxHp.Base      = cre->m_npcInfo->Hp*fbHpRate*npcRate;
				cre->m_core.MaxMp.Base      = cre->m_npcInfo->Mp*fbRate;
				cre->m_killReward.Exp      *= npcRate;
			}
			else
			{
				cre->m_core.MaxHp.Base     = cre->m_npcInfo->Hp*fbHpRate*npcRate;
				cre->m_core.MaxMp.Base     = cre->m_npcInfo->Mp*fbRate;
			}
		}

		if(cre->m_core.Lev > 10)
		{
			cre->m_core.ElementArmor[ELEMENT_WATER].Base  = (cre->m_core.Lev ) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_WATER];
			cre->m_core.ElementArmor[ELEMENT_FIRE].Base   = (cre->m_core.Lev ) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_FIRE];
			cre->m_core.ElementArmor[ELEMENT_POISON].Base = (cre->m_core.Lev ) * 0.1 * ELE_ARMOR_SCALE*2*cre->m_npcInfo->ElementArmor[ELEMENT_POISON];
		}		

		cre->m_core.SetHp(cre->m_core.GetMaxHp());
		cre->m_core.SetMp(cre->m_core.GetMaxMp());
	}

	if(!AddCreature(cre,npcInfo.pos,npcInfo.cDir))
	{
		g_factory->DestroyObject(cre);
		return 0;
	}
	else
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	return cre;
}

void CRegionSceneFB::OnTimer(ULONG id, DWORD dwParam[])
{
	CRegionScene::OnTimer(id, dwParam);
}

void CRegionSceneFB::OnCreate()
{
	CRegionScene::OnCreate();
}

void CRegionSceneFB::OnRun()
{
    CRegionScene::OnRun();
}

void CRegionSceneFB::OnDestroy()
{
	CRegionScene::OnDestroy();
	
}

void CRegionSceneFB::OnUserEnter(CRegionUser *user)
{
	CRegionScene::OnUserEnter(user);
}

void CRegionSceneFB::OnUserLeave(CRegionUser *user,int reason)
{
	CRegionScene::OnUserLeave(user,reason);
}

bool CRegionSceneFB::AddCreature(CRegionCreature *cre,long *pos,char dir)
{
	bool bRet = CRegionScene::AddCreature(cre,pos,dir);
	cre->InvokeEventCommand("on_bornFB");
	return bRet;
}


bool CRegionSceneFB::OnTryAddUser(CRegionUser *user)
{
	if(!CRegionScene::OnTryAddUser(user))
		return false;

	return true;
}

EItemSelectionLevel GetSelLevel(int lev)
{
	if(lev<=9) return ISL_1_9;
	if(lev<=19) return ISL_10_19;
	if(lev<=29) return ISL_20_29;
	if(lev<=39) return ISL_30_39;
	if(lev<=49) return ISL_40_49;
	if(lev<=59) return ISL_50_59;
	if(lev<=69) return ISL_60_69;
	if(lev<=79) return ISL_70_79;
	if(lev<=89) return ISL_80_89;
	if(lev<=99) return ISL_90_99;
	

	return ISL_90_99;
}

void CRegionSceneFB::OnCreatureDead(CRegionCreature *cre,CRegionCreature *killer)
{
	cre->InvokeEventCommand("on_deadFB");

	TObjectHash::iterator it;
	
	if(cre->IsUser())
	{
		cre->m_core.SetHp(1);
		cre->m_core.SetMp(1);
	}
//	return;
//	static int blueNum=0,greenNum=0,dropNum=0,goldNum=0;
//	if(!cre->IsMonster())
//		return;
//
//	cre->GetPlayerLogic()->GetMonsterLogicExt()->DropItemS(killer, this);//PZH 改成道具落在尸体上
//	SNpc *info = cre->m_npcInfo; 
//	if(!info) return;
////
////	bool bFirstDropItem = true;
////	SItemID item;
////	for(int i=0; i<info->RandItemNum; i++)
////	{
////		// if(!GetProb(info->RandItemDropRate[i] * g_cfg.game.item_drop_rate)) continue;
////		dropNum++;
////		if(!GetProb(info->RandItemDropRate[i])) continue;
////		
////		EItemColor color = ItemColor_Green;
////		float blueRate = 0.0f;
////		float goldRate = 0.0f;
////		switch(m_curDif)
////		{
////			case 2: blueRate = 0.05; goldRate = 0; break;
////			case 3: blueRate = 0.05; goldRate = 0; break;
////			case 4: blueRate = 0.05; goldRate = 0; break;
////		}
////		
////		if(info->Boss)
////		{
////			blueRate *= 5;
////			goldRate = 0.1;
////		}
////
////		if(m_curDif == 4)
////		{
////			goldRate = 0.02;
////		}
////
////		//增加一种Boss类型，2，下周添加打最终Boss的活动
////		//if(info->Boss == 2)
////		//{
////		//	switch(m_curDif)
////		//	{
////		//	case 0: killer->AddItem();break;
////		//	case 1: killer->AddItem();break;
////		//	case 2: killer->AddItem();break;
////		//	case 3: killer->AddItem();break;
////		//	case 4: killer->AddItem();break;
////		//	}
////		//}
////
////		//添加金装掉落的等级衰减
////		goldRate = goldRate * (m_curLev + 20) / killer->m_core.Lev;
////		if(goldRate > 0.1) goldRate = 0.1;
////		if(goldRate < 0) goldRate = 0;
////		
////		if(GetProb(blueRate))
////			color = ItemColor_Blue;
////
//////		if(m_curDif == 4 && color == ItemColor_Blue && GetProb(0.1))
////		if(color == ItemColor_Blue && GetProb(goldRate))
////			color = ItemColor_Golden;
////
////		int dropItemLev = m_curLev;
////		if(color == ItemColor_Blue)
////		{
////			dropItemLev = m_info->reqLev[m_curDif-1];
////			blueNum++;
////		}
////		else if(color == ItemColor_Golden)
////		{
////			dropItemLev = m_info->reqLev[m_curDif-1];
////			goldNum++;
////		}
////		else
////		{
////			dropItemLev = m_info->reqLev[m_curDif];
////			greenNum++;
////		}
////
////		SItemSelector sel;
////		sel.level = GetSelLevel(dropItemLev);
////		sel.type  = (EItemSelectionType)info->RandItem[i];
////		vector<DWORD> id;
////		g_region->m_pItemManager->m_pSelectionTable->FindSelection(sel,id);
////		if(id.size() == 0) continue;
////		long idx = GetRand((int)id.size()-1,0);
////						
////		item = g_region->m_pItemManager->CreateItem(id[idx],color,0,IB_BindWhenEquip);
////		
////		if(ItemID_IsValid(item))
////		{
////			if (ItemIsBlue(item))
////			{
////				std::string itemString;
////				item.SaveToReadableString(g_region->m_pItemManager, itemString);
////				ERR1("FB drop a BLUE item: item=%s\n", itemString.c_str());
////				ERR4("FB item drop=%d,gold=%d,blue=%d,green=%d\n",dropNum,goldNum,blueNum,greenNum);
////			}
////			else if(ItemIsGolden(item))
////			{
////				std::string itemString;
////				item.SaveToReadableString(g_region->m_pItemManager, itemString);
////				ERR1("FB drop a GOLD item: item=%s\n", itemString.c_str());
////				ERR4("FB item drop=%d,gold=%d,blue=%d,green=%d\n",dropNum,goldNum,blueNum,greenNum);
////			}
////
////			CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
////			dummy->m_item = item;
////			dummy->m_hideTime = rtGetMilliseconds() + 1000;
////			dummy->m_masterId = 0;
////			dummy->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
////			dummy->m_dropStyle = ItemDrop_Descend;
////
////			static long pos[3];
////			static int dropCounter = 0;
////			if (bFirstDropItem)
////			{
////				pos[0] = cre->m_pos[0];	pos[1] = cre->m_pos[1];  pos[2] = cre->m_pos[2];
////				dropCounter = 0;
////				bFirstDropItem = false;
////			}
////
////			if(!CRegionItem::GetNextDropPos(cre->m_pos,pos,dropCounter))
////			{
////				pos[0] = cre->m_pos[0];	pos[1] = cre->m_pos[1];  pos[2] = cre->m_pos[2];
////			}
////
////			if(!AddItem(dummy,pos,0))
////				g_factory->DestroyObject(dummy);
////		}
////	}
////
////	// 普通道具掉落
////	bFirstDropItem = true;
////	for(int i=0; i<info->ItemNum; i++)
////	{
////		if(!GetProb(info->ItemDropRate[i] * g_cfg.game.item_drop_rate)) continue;
////		EItemColor color = ItemColor_White;
////		if(GetProb(info->ItemGreenRate*g_cfg.game.item_green_rate))
////		{
////			color = ItemColor_Green;
////			if(GetProb(info->ItemBlueRate*g_cfg.game.item_blue_rate))	
////			{
////				color = ItemColor_Blue;
////				// golden item
////				if(GetProb(info->ItemGoldRate))	
////				{
////					color = ItemColor_Golden;
////				}
////			}
////		}
////		item = g_region->m_pItemManager->CreateItem(info->Item[i],color);
////
////// 		if(ItemIsArmor(item.type) || ItemIsWeapon(item.type) || ItemIsShipin(item.type))
////// 			if(!ItemCanNpcTrade(item.type)) continue;
////
////		if(ItemID_IsValid(item))
////		{
////			CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
////			dummy->m_item = item;
////			dummy->m_hideTime = rtGetMilliseconds() + 1000;
////			dummy->m_masterId = 0;
////			if(killer)
////                dummy->m_masterId = killer->m_oId;
////			dummy->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
////			dummy->m_dropStyle = ItemDrop_Descend;
////
////			static long pos[3];
////			static int dropCounter = 0;
////			if (bFirstDropItem)
////			{
////				pos[0] = cre->m_pos[0];	pos[1] = cre->m_pos[1];  pos[2] = cre->m_pos[2];
////				dropCounter = 0;
////				bFirstDropItem = false;
////			}
////
////			if (!CRegionItem::GetNextDropPos(cre->m_pos, pos, dropCounter) )
////			{
////				pos[0] = cre->m_pos[0];	pos[1] = cre->m_pos[1];  pos[2] = cre->m_pos[2];
////			}
////
////			if(!AddItem(dummy,pos,0))
////				g_factory->DestroyObject(dummy);
////		}
////	}
//
//	float exp = g_cfg.game.exp_rate*GetRand(cre->m_killReward.Exp*1.1+1.0f,cre->m_killReward.Exp*0.9)/m_userHash.size();
//	for(it=m_userHash.begin(); it!=m_userHash.end(); it++)
//	{
//		CRegionCreature *tmp = (CRegionCreature*)(it->second);
//		CRegionUser* user = tmp->m_userInfo; 
//		if(!user->m_active) continue;
//
//		// 打卡
//		tmp->AddExp(exp*tmp->GetExpRate(),0);
//		if(info->Credit > 0)
//			tmp->AddCredit(info->Credit);
//		
//		int kk = 0;
//		for(kk = 0; kk < info->nTaskNum; kk++)
//		{
//			if(!GetProb(info->TaskItemDropRate[kk]))
//			{
//				continue;
//			}
//			int nRet = killer->GetPlayerLogic()->AllMyTeamAddTaskItem(cre, kk);
//			if(0 != nRet && killer->m_task.IsTaskAccept(cre->m_npcInfo->TaskId[kk]))
//			{
//				if(/*GetProb(m_npcInfo->TaskItemDropRate) && */
//					killer->GetItemNum(cre->m_npcInfo->TaskItemId[kk], ItemColor_White) < cre->m_npcInfo->TaskItemLimit[kk])
//				{
//					killer->AddItem(cre->m_npcInfo->TaskItemId[kk],ItemColor_White);
//					STaskInfo* info = killer->m_task.FindTask(cre->m_npcInfo->TaskId[kk]);
//					if (NULL != info)
//					{
//						if (NULL != info->Desc)
//						{
//							killer->CanFinishTask(info->Desc, 0, NULL);
//						}
//					}
//					killer->UpdateToClient(NET_CRE_UPDATE_TASK);
//				}
//			}
//			//if (0 != nRet)
//			//{
//			//	if(/*info->FbTask &&*/ info->TaskId[kk] && tmp->m_task.IsTaskAccept(info->TaskId[kk])/*tmp->m_task.IsFbTaskAccept(info->TaskId[kk], m_curDif)*/)
//			//	{
//			//		if(tmp->GetDungeonMissionItemNum(info->TaskItemId[kk], m_curDif) < info->TaskItemLimit[kk])
//			//		{
//			//			tmp->AddDungeonMissionItem(info->TaskItemId[kk], m_curDif);
//			//		}
//			//	}
//			//}
//		}
//	}

}


// ----------------------------------------------------------------------------
// warfield scene
// ----------------------------------------------------------------------------
CRsSceneWarfield::CRsSceneWarfield()
{

}

CRsSceneWarfield::~CRsSceneWarfield()
{

}

void CRsSceneWarfield::OnTimer(ULONG id, DWORD dwParam[])
{
	CRegionScene::OnTimer(id, dwParam);
}

void CRsSceneWarfield::OnCreate()
{
	CRegionScene::OnCreate();
}

void CRsSceneWarfield::OnRun()
{
	CRegionScene::OnRun();
}

void CRsSceneWarfield::OnDestroy()
{
	CRegionScene::OnDestroy();
}

void CRsSceneWarfield::OnUserEnter(CRegionUser *user)
{
	CRegionScene::OnUserEnter(user);
}

void CRsSceneWarfield::OnUserLeave(CRegionUser *user,int reason)
{
	CRegionScene::OnUserLeave(user,reason);
}

bool CRsSceneWarfield::OnTryAddUser(CRegionUser *user)
{
	if(!CRegionScene::OnTryAddUser(user))
		return false;

	return true;
}

void CRsSceneWarfield::OnCreatureDead(CRegionCreature *cre,CRegionCreature *killer)
{
	if(cre->IsUser())
	{
		cre->m_core.SetHp(1);
		cre->m_core.SetMp(1);
	}
}

void CRegionSceneFB::AddTeam(int key, int num)
{
	if(m_hash_team.find(key)!=m_hash_team.end())
	{
		m_hash_team[key] += num;
	}else
	{
		m_hash_team[key] = num;
	}
}

void CRegionSceneFB::RemoveTeam(int key, int num)
{
	if(m_hash_team.find(key)!=m_hash_team.end())
	{
		m_hash_team[key] -= num;
	}else
	{
		m_hash_team[key] = 0;
	}
	
	if(m_hash_team[key]<0) m_hash_team[key] = 0;
}

int CRegionSceneFB::GetTeamNum(int key)
{
	return m_hash_team[key];
}

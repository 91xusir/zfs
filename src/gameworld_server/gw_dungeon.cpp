#include "gw_server.h"
#include "gw_dungeon.h"

//static const char* s_szDNoneError       = "没有错误";
//static const char* s_szDungeonTooMuch   = "副本数量已经到最大值";
//static const char* s_szDNoServer        = "当前没有副本服务器";
//static const char* s_szDWrongType       = "错误的副本类型";
//static const char* s_szDOutOfService    = "副本未开放";
//static const char* s_szDErrorUnknown    = "未知错误";

#define s_szDNoneError        R(MSG_DUNGEON_NONE_ERR)
#define s_szDungeonTooMuch    R(MSG_DUNGEON_SERVER_OVERFLOW)
#define s_szDNoServer         R(MSG_DUNGEON_NO_SERVER)
#define s_szDWrongType        R(MSG_DUNGEON_WRONG_TYPE)
#define s_szDOutOfService     R(MSG_DUNGEON_OUT_OF_SERVICE)
#define s_szDErrorUnknown     R(MSG_DUNGEON_UNKNOWN_ERROR)

CGwDungeonMgr::CGwDungeonMgr()
{
    m_lTotalDungeon = 0;
    m_lDungeonIdx = 1;
	// Change 300 to 30 Louis.Huang @ Wed 25 Feb 2009
    m_iMaxDungeon = 60;
    m_szErrorInfo = NULL;
}

CGwDungeonMgr::~CGwDungeonMgr()
{
}
//tim.yang  设置副本最大数量
int CGwDungeonMgr::GetListDungeonSize()
{
	int listDungeonSize = m_listDungeon.size();
	return listDungeonSize;
}
void CGwDungeonMgr::SetMaxDungeon(long maxdungeon)
{
	m_iMaxDungeon = maxdungeon;
}

bool CGwDungeonMgr::Startup()
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    rt2_sprintf(cmd.command, "SELECT LastID FROM T_DungeonID LIMIT 1", GWServer_RegionID);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        const char* pErrMsg = "T_DungeonID数据库出错";
        ERR1("%s\n", pErrMsg);
        CHECKEX(pErrMsg);
        return false;
    }else
    {
        int count = mysql_num_rows(pDBResult);
        if (count==0)
        {
            const char* pErrMsg = "T_DungeonID数据库未配置";
            ERR1("%s\n", pErrMsg);
            CHECKEX(pErrMsg);
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(pDBResult);
        m_lDungeonIdx = atol(row[0]);
        if (m_lDungeonIdx==0) m_lDungeonIdx = 1;
        mysql_free_result(pDBResult);

        if (m_lDungeonIdx>0x2FFFFFFF)
        {
            ERR("WARNING: Dungeon ID close to full.\n");
        }
    }
    return true;
}

bool CGwDungeonMgr::Shutdown()
{
    UpdateDatabaseID();
    return true;
}

void CGwDungeonMgr::UpdateDatabaseID()
{
    GWDBCommand cmd;
    MYSQL_RES *pDBResult;
    rt2_sprintf(cmd.command, "UPDATE T_DungeonID SET LastID=%d", m_lDungeonIdx);
    if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
    {
        ERR("Can NOT update Database(Table:T_DungeonID)\n");
        return;
    }else
    {
        mysql_free_result(pDBResult);
        return;
    }
}

void CGwDungeonMgr::Run()
{
    // 这里也要检查副本的极限时间，如果极限时间到了就要去Region上面检查是否到极限时间
    //     如果Region上的副本不存在就销毁副本
    //     如果存在就按照Region上处理
}

CGwDungeonMgr::SGwDungeon* CGwDungeonMgr::EnterDungeon(DWORD dwUserID, long lDungeonType, long lDungeonLev, long lDungeonUserLV, long lExitX, long lExitY)
{
    int i, j;
    DWORD dwID;
    GWUser* pLeader;
    SGwDungeon* pDungeon;

    // 如果找不到玩家，说明这个玩家要创建一个新的副本
    EXT_SPACE::unordered_map<DWORD, SGwDungeon*>::iterator itUser = m_mapUser.find(dwUserID);
    if (itUser==m_mapUser.end())
	{
		// 玩家自身条件的判断
		pLeader = g_server.FindActorById(dwUserID);
		if (pLeader==NULL)
		{
			m_szErrorInfo = s_szDErrorUnknown;
			return NULL;
		}
		if (pLeader->team)
		{
			for (i=pLeader->team->Begin(); i!=pLeader->team->End(); i++)
			{
				dwID = (*pLeader->team)[i].mDBID;
				itUser = m_mapUser.find(dwID);
				if (itUser!=m_mapUser.end())
				{
					pDungeon = (*itUser).second;

					pDungeon->listUser.push_back(SUser());
					pDungeon->listUser.back().dwUserID = dwUserID;
					pDungeon->listUser.back().bInDungeon = false;
					m_mapUser[dwUserID] = pDungeon;
					pDungeon->listApplicant.push_back(dwUserID);

					g_server.m_chatTransmittal.Chat(
						g_server.m_region.GetSession().index, dwUserID, R(MSG_DUNGEON),
						CHAT_CHANNEL_SYSTEM, R(MSG_DUNGEON_SENDING));
					// 传送玩家到副本中
					g_server.MoveUserToDungeon(dwUserID, pDungeon->lDungeonIdx, pDungeon->pInfo->bornPos[0], pDungeon->pInfo->bornPos[1], true);
					return pDungeon;
				}
			}
		}

        // 如果副本的总个数已经到达最大个数就创建失败
        if ((int)m_listDungeon.size()>=m_iMaxDungeon)
        {
            m_szErrorInfo = s_szDungeonTooMuch;
            return NULL;
        }

        // 如果没有副本服务器
        if ((int)m_mapDungeonServer.size()==0)
        {
            m_szErrorInfo = s_szDNoServer;
            return NULL;
        }

        // 副本类型错误
        SFuBen* pInfo = g_TableFB.Find(lDungeonType);
        if (pInfo==NULL)
        {
            m_szErrorInfo = s_szDWrongType;
            return NULL;
        }

        // 副本时间判断
        if (!pInfo->cyc.NowIsTime())
        {
            m_szErrorInfo = s_szDOutOfService;
            return NULL;
        }

        // 创建副本(1)，查找可以在什么地方创建副本
        i = 100000;
        j = -1;
        std::map<int, int>::iterator itServer;
        for (itServer=m_mapDungeonServer.begin(); itServer!=m_mapDungeonServer.end(); itServer++)
        {
            if ((*itServer).second<i)
            {
                i = (*itServer).second;
                j = (*itServer).first;
            }
        }
        if (j==-1)
        {
            m_szErrorInfo = s_szDErrorUnknown;
            return NULL;
        }

        // 创建副本(2)，创建
        m_listDungeon.push_back(SGwDungeon());
        pDungeon = &(m_listDungeon.back());
        pDungeon->lDungeonType = lDungeonType;
		pDungeon->lDungeonLev = lDungeonLev;
		pDungeon->lDungeonUserLV = lDungeonUserLV;
        pDungeon->pInfo = pInfo;
        pDungeon->iRegionIdx = j;
        pDungeon->lDungeonIdx = m_lDungeonIdx++;
        pDungeon->eDungeonState = EDS_UNCREATE; // 还未创建
        pDungeon->tCreateTime = time(NULL);
        pDungeon->tCreateLimitTime = pDungeon->tCreateTime + 10;
        pDungeon->tLimitTime = pDungeon->tCreateTime + pInfo->timeLimitSecond;
        pDungeon->tEndTime = 0;
        m_mapDungeon[pDungeon->lDungeonIdx] = pDungeon;

        // 更新数据库的ID
        UpdateDatabaseID();

        // 添加准备进入副本的人
        pDungeon->listApplicant.push_back(dwUserID);

        // 添加进入这个副本的所有玩家的信息
        //if (pLeader->team)
        //{
        //    // 这里要查找这个人是否已经在别的队伍里面，如果已经在，就不能进入这个副本
        //    for (i=pLeader->team->Begin(); i!=pLeader->team->End(); i++)
        //    {
        //        dwID = (*pLeader->team)[i].mDBID;
        //        itUser = m_mapUser.find(dwID);
        //        if (itUser==m_mapUser.end())
        //        {
        //            pDungeon->listUser.push_back(SUser());
        //            pDungeon->listUser.back().dwUserID = dwID;
        //            pDungeon->listUser.back().bInDungeon = false;
        //            m_mapUser[dwID] = pDungeon;

        //            // 把所有的团队成员都传送进副本
        //            if (dwID!=dwUserID)
        //            {
        //                pDungeon->listApplicant.push_back(dwID);
        //            }
        //        }
        //    }
        //}else
        {
            pDungeon->listUser.push_back(SUser());
            pDungeon->listUser.back().dwUserID = dwUserID;
            pDungeon->listUser.back().bInDungeon = false;
            m_mapUser[dwUserID] = pDungeon;
        }

        // 发送命令给Region，通知他创建副本
        //     Region收到命令后就创建副本，并且返回成功或者失败
        //     如果失败，就需要解散这个副本信息
        //     如果成功，就根据副本信息进行玩家的传送
        //   ==> 后面的过程请参考 OnCreateDungeonRet 的处理

	    int se = g_server.GetRegionSession(pDungeon->iRegionIdx);
	    if (se != -1)
        {
#ifdef _DEBUG
            LOG("Dungeon: Create\n");
#endif
            CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_create_dungeon);
            cmd.WriteLong(pDungeon->lDungeonIdx);
            cmd.WriteLong(pDungeon->lDungeonType);
			cmd.WriteLong(pDungeon->lDungeonLev);
			cmd.WriteLong(pDungeon->lDungeonUserLV);
			cmd.WriteLong(lExitX);
			cmd.WriteLong(lExitY);
            g_server.m_region.SendToPeer(g_server.GetRegionSession(pDungeon->iRegionIdx), &cmd);
            return pDungeon;
        }else
        {
            // 创建失败，因为Region连接不存在
            m_szErrorInfo = s_szDErrorUnknown;
            return NULL;
        }
    }else
    {
        // 如果能找到这个玩家，说明玩家在副本之中
        pDungeon = (*itUser).second;

        // 如果副本还没有启动，就添加到准备进入副本的列表中
        if (pDungeon->eDungeonState==EDS_UNCREATE)
        {
            if (time(NULL)>pDungeon->tCreateLimitTime)
            {
                _DestroyDungeon(pDungeon);
     //           g_server.m_chatTransmittal.Chat(
					//g_server.m_region.GetSession().index, dwUserID, R(MSG_DUNGEON),
					//CHAT_CHANNEL_SYSTEM, R(MSG_DUNGEON_CREATE_OVERTIME));
				m_szErrorInfo = R(MSG_DUNGEON_CREATE_OVERTIME);
				return NULL;
            }else
            {
                // 如果已经添加进来了就不要再添加
                std::list<DWORD>::iterator itApp = pDungeon->listApplicant.begin();
                for (; itApp!=pDungeon->listApplicant.end(); itApp++)
                {
                    if (*itApp==dwUserID)
                    {
                        break;
                    }
                }
                if (itApp==pDungeon->listApplicant.end())
                {
                    pDungeon->listApplicant.push_back(dwUserID);
                }
                g_server.m_chatTransmittal.Chat(
					g_server.m_region.GetSession().index, dwUserID, R(MSG_DUNGEON),
					CHAT_CHANNEL_SYSTEM, R(MSG_DUNGEON_ADD_IN_LIST));
                return pDungeon;
            }
        }else
        {
            g_server.m_chatTransmittal.Chat(
				g_server.m_region.GetSession().index, dwUserID, R(MSG_DUNGEON),
				CHAT_CHANNEL_SYSTEM, R(MSG_DUNGEON_SENDING));
            // 传送玩家到副本中
            g_server.MoveUserToDungeon(dwUserID, pDungeon->lDungeonIdx, pDungeon->pInfo->bornPos[0], pDungeon->pInfo->bornPos[1], true);
            return pDungeon;
        }
    }
    return NULL;
}

bool CGwDungeonMgr::LeaveDungeon(DWORD dwUserID)
{
    SGwDungeon* pDungeon;
    EXT_SPACE::unordered_map<DWORD, SGwDungeon*>::iterator itUser = m_mapUser.find(dwUserID);
    if (itUser!=m_mapUser.end())
    {
        // 如果能找到这个玩家，说明玩家在副本之中
        pDungeon = (*itUser).second;
        // 如果副本还没有启动
        pDungeon->listApplicant.remove(dwUserID);
        std::list<SUser>::iterator it;
        for (it=pDungeon->listUser.begin(); it!=pDungeon->listUser.end(); it++)
        {
            if ((*it).dwUserID==dwUserID)
            {
                pDungeon->listUser.erase(it);
                break;
            }
        }
        m_mapUser.erase(dwUserID);

        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_leave_dungeon_ret);
        cmd.WriteLong(dwUserID);
        g_server.m_region.SendToPeer(g_server.GetRegionSession(pDungeon->iRegionIdx), &cmd);
    }else
    {
        MSG1("LeaveDungeon: %d 不在副本中\n", dwUserID);
        return false;
    }
    return true;
}

bool CGwDungeonMgr::DestroyDungeon(DWORD dwDungeonUID, long dwExitSecond)
{
    SGwDungeon* pDungeon = FindGwDungeon(dwDungeonUID);
    if (pDungeon==NULL)
    {
        // 如果副本找不到，就可能是由于某种原因导致他已经销毁了
        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_destroy_dungeon_ret);
        cmd.WriteLong(dwDungeonUID);
        cmd.WriteLong(dwExitSecond);
        g_server.m_region.SendCmd();
        return false;
    }

    bool bResult = true;
	int se = g_server.GetRegionSession(pDungeon->iRegionIdx);
	if (se != -1)
    {
        // 通知Region销毁副本
        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_destroy_dungeon_ret);
        cmd.WriteLong(dwDungeonUID);
        cmd.WriteLong(dwExitSecond);
        g_server.m_region.SendCmd();
    }else
    {
        // 创建失败，因为Region连接不存在
        m_szErrorInfo = s_szDErrorUnknown;
        bResult = false;
    }

    // 销毁副本
    if (dwExitSecond==0)
    {
#ifdef _DEBUG
        LOG("Dungeon: Destroy\n");
#endif
        _DestroyDungeon(pDungeon);
    }

    return bResult;
}

// Region收到命令后就创建副本，并且返回成功或者失败
// 如果失败，就需要解散这个副本信息
// 如果成功，就根据副本信息进行玩家的传送
void CGwDungeonMgr::OnCreateDungeonRet(long lDungeonIdx, bool bResult)
{
    if (m_mapDungeon.find(lDungeonIdx)==m_mapDungeon.end())
    {
        return;
    }

#ifdef _DEBUG
    LOG("Dungeon: OnCreateDungeonRet\n");
#endif

    SGwDungeon* pDungeon = m_mapDungeon[lDungeonIdx];

    if (bResult)
    {
        pDungeon->eDungeonState = EDS_CREATED;

        // 创建成功
        m_mapDungeonServer[pDungeon->iRegionIdx] = m_mapDungeonServer[pDungeon->iRegionIdx]+1;

        // 传送玩家
        std::list<DWORD>::iterator itAppUser = pDungeon->listApplicant.begin();
        for (; itAppUser!=pDungeon->listApplicant.end(); itAppUser++)
        {
            g_server.MoveUserToDungeon((*itAppUser), pDungeon->lDungeonIdx, pDungeon->pInfo->bornPos[0], pDungeon->pInfo->bornPos[1], true);
        }
    }else
    {
        // 创建失败
        _DestroyDungeon(pDungeon);
    }
}

void CGwDungeonMgr::_DestroyDungeon(SGwDungeon* pDungeon)
{
    std::list<SUser>::iterator itUser;
    EXT_SPACE::unordered_map<long, SGwDungeon*>::iterator itmapDungeon;
    std::list<SGwDungeon>::iterator itDungeon;

    for (itUser=pDungeon->listUser.begin(); itUser!=pDungeon->listUser.end(); itUser++)
    {
        m_mapUser.erase((*itUser).dwUserID);
    }
    m_mapDungeon.erase(pDungeon->lDungeonIdx);

    for (itDungeon=m_listDungeon.begin(); itDungeon!=m_listDungeon.end(); )
    {
        if (&(*itDungeon)==pDungeon)
        {
            itDungeon = m_listDungeon.erase(itDungeon);
        }else
        {
            itDungeon++;
        }
    }

}

CGwDungeonMgr::SGwDungeon* CGwDungeonMgr::FindGwDungeon(long lDungeonIdx)
{
    EXT_SPACE::unordered_map<long, SGwDungeon*>::iterator it = m_mapDungeon.find(lDungeonIdx);
    if (it!=m_mapDungeon.end())
    {
        return it->second;
    }
    return NULL;
}

void CGwDungeonMgr::AddDungeonRegion(int iRegionID)
{
    if (m_mapDungeonServer.find(iRegionID)==m_mapDungeonServer.end())
    {
        m_mapDungeonServer[iRegionID] = 0;
    }
}

void CGwDungeonMgr::OnRegionDisconnected(int iRegionID)
{
    SGwDungeon* pDungeon;
    std::list<long> listRegionDungeon;
    EXT_SPACE::unordered_map<long, SGwDungeon*>::iterator itM;
    for (itM=m_mapDungeon.begin(); itM!=m_mapDungeon.end(); itM++)
    {
        pDungeon = itM->second;
        if (pDungeon->iRegionIdx==iRegionID)
        {
            listRegionDungeon.push_back(pDungeon->lDungeonIdx);
        }
    }

    std::list<long>::iterator it;
    for (it=listRegionDungeon.begin(); it!=listRegionDungeon.end(); it++)
    {
        DestroyDungeon(*it, 0);
    }
}

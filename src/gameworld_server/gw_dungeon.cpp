#include "gw_server.h"
#include "gw_dungeon.h"

//static const char* s_szDNoneError       = "û�д���";
//static const char* s_szDungeonTooMuch   = "���������Ѿ������ֵ";
//static const char* s_szDNoServer        = "��ǰû�и���������";
//static const char* s_szDWrongType       = "����ĸ�������";
//static const char* s_szDOutOfService    = "����δ����";
//static const char* s_szDErrorUnknown    = "δ֪����";

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
//tim.yang  ���ø����������
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
        const char* pErrMsg = "T_DungeonID���ݿ����";
        ERR1("%s\n", pErrMsg);
        CHECKEX(pErrMsg);
        return false;
    }else
    {
        int count = mysql_num_rows(pDBResult);
        if (count==0)
        {
            const char* pErrMsg = "T_DungeonID���ݿ�δ����";
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
    // ����ҲҪ��鸱���ļ���ʱ�䣬�������ʱ�䵽�˾�ҪȥRegion�������Ƿ񵽼���ʱ��
    //     ���Region�ϵĸ��������ھ����ٸ���
    //     ������ھͰ���Region�ϴ���
}

CGwDungeonMgr::SGwDungeon* CGwDungeonMgr::EnterDungeon(DWORD dwUserID, long lDungeonType, long lDungeonLev, long lDungeonUserLV, long lExitX, long lExitY)
{
    int i, j;
    DWORD dwID;
    GWUser* pLeader;
    SGwDungeon* pDungeon;

    // ����Ҳ�����ң�˵��������Ҫ����һ���µĸ���
    EXT_SPACE::unordered_map<DWORD, SGwDungeon*>::iterator itUser = m_mapUser.find(dwUserID);
    if (itUser==m_mapUser.end())
	{
		// ��������������ж�
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
					// ������ҵ�������
					g_server.MoveUserToDungeon(dwUserID, pDungeon->lDungeonIdx, pDungeon->pInfo->bornPos[0], pDungeon->pInfo->bornPos[1], true);
					return pDungeon;
				}
			}
		}

        // ����������ܸ����Ѿ������������ʹ���ʧ��
        if ((int)m_listDungeon.size()>=m_iMaxDungeon)
        {
            m_szErrorInfo = s_szDungeonTooMuch;
            return NULL;
        }

        // ���û�и���������
        if ((int)m_mapDungeonServer.size()==0)
        {
            m_szErrorInfo = s_szDNoServer;
            return NULL;
        }

        // �������ʹ���
        SFuBen* pInfo = g_TableFB.Find(lDungeonType);
        if (pInfo==NULL)
        {
            m_szErrorInfo = s_szDWrongType;
            return NULL;
        }

        // ����ʱ���ж�
        if (!pInfo->cyc.NowIsTime())
        {
            m_szErrorInfo = s_szDOutOfService;
            return NULL;
        }

        // ��������(1)�����ҿ�����ʲô�ط���������
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

        // ��������(2)������
        m_listDungeon.push_back(SGwDungeon());
        pDungeon = &(m_listDungeon.back());
        pDungeon->lDungeonType = lDungeonType;
		pDungeon->lDungeonLev = lDungeonLev;
		pDungeon->lDungeonUserLV = lDungeonUserLV;
        pDungeon->pInfo = pInfo;
        pDungeon->iRegionIdx = j;
        pDungeon->lDungeonIdx = m_lDungeonIdx++;
        pDungeon->eDungeonState = EDS_UNCREATE; // ��δ����
        pDungeon->tCreateTime = time(NULL);
        pDungeon->tCreateLimitTime = pDungeon->tCreateTime + 10;
        pDungeon->tLimitTime = pDungeon->tCreateTime + pInfo->timeLimitSecond;
        pDungeon->tEndTime = 0;
        m_mapDungeon[pDungeon->lDungeonIdx] = pDungeon;

        // �������ݿ��ID
        UpdateDatabaseID();

        // ���׼�����븱������
        pDungeon->listApplicant.push_back(dwUserID);

        // ��ӽ������������������ҵ���Ϣ
        //if (pLeader->team)
        //{
        //    // ����Ҫ����������Ƿ��Ѿ��ڱ�Ķ������棬����Ѿ��ڣ��Ͳ��ܽ����������
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

        //            // �����е��Ŷӳ�Ա�����ͽ�����
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

        // ���������Region��֪ͨ����������
        //     Region�յ������ʹ������������ҷ��سɹ�����ʧ��
        //     ���ʧ�ܣ�����Ҫ��ɢ���������Ϣ
        //     ����ɹ����͸��ݸ�����Ϣ������ҵĴ���
        //   ==> ����Ĺ�����ο� OnCreateDungeonRet �Ĵ���

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
            // ����ʧ�ܣ���ΪRegion���Ӳ�����
            m_szErrorInfo = s_szDErrorUnknown;
            return NULL;
        }
    }else
    {
        // ������ҵ������ң�˵������ڸ���֮��
        pDungeon = (*itUser).second;

        // ���������û������������ӵ�׼�����븱�����б���
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
                // ����Ѿ���ӽ����˾Ͳ�Ҫ�����
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
            // ������ҵ�������
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
        // ������ҵ������ң�˵������ڸ���֮��
        pDungeon = (*itUser).second;
        // ���������û������
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
        MSG1("LeaveDungeon: %d ���ڸ�����\n", dwUserID);
        return false;
    }
    return true;
}

bool CGwDungeonMgr::DestroyDungeon(DWORD dwDungeonUID, long dwExitSecond)
{
    SGwDungeon* pDungeon = FindGwDungeon(dwDungeonUID);
    if (pDungeon==NULL)
    {
        // ��������Ҳ������Ϳ���������ĳ��ԭ�������Ѿ�������
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
        // ֪ͨRegion���ٸ���
        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_destroy_dungeon_ret);
        cmd.WriteLong(dwDungeonUID);
        cmd.WriteLong(dwExitSecond);
        g_server.m_region.SendCmd();
    }else
    {
        // ����ʧ�ܣ���ΪRegion���Ӳ�����
        m_szErrorInfo = s_szDErrorUnknown;
        bResult = false;
    }

    // ���ٸ���
    if (dwExitSecond==0)
    {
#ifdef _DEBUG
        LOG("Dungeon: Destroy\n");
#endif
        _DestroyDungeon(pDungeon);
    }

    return bResult;
}

// Region�յ������ʹ������������ҷ��سɹ�����ʧ��
// ���ʧ�ܣ�����Ҫ��ɢ���������Ϣ
// ����ɹ����͸��ݸ�����Ϣ������ҵĴ���
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

        // �����ɹ�
        m_mapDungeonServer[pDungeon->iRegionIdx] = m_mapDungeonServer[pDungeon->iRegionIdx]+1;

        // �������
        std::list<DWORD>::iterator itAppUser = pDungeon->listApplicant.begin();
        for (; itAppUser!=pDungeon->listApplicant.end(); itAppUser++)
        {
            g_server.MoveUserToDungeon((*itAppUser), pDungeon->lDungeonIdx, pDungeon->pInfo->bornPos[0], pDungeon->pInfo->bornPos[1], true);
        }
    }else
    {
        // ����ʧ��
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

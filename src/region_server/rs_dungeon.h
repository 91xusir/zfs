#ifndef _INC_RS_DUNGEON_H_
#define _INC_RS_DUNGEON_H_

// 副本系统
/*
region-玩家请求进入某一难度的副本
region-通知gameworld
gameworld-整个team条件检查，等级，是否已经完成上一难度副本任务
gameworld-选择副本region
副本region-创建副本
gameworld-分配玩家进入副本region
副本region-接受玩家，再次做条件检查
*/

struct RunTrigger;

class CRsDungeonMgr : public CRegionObject
{
public:
    struct SRsDungeon
    {
		CRegionSceneFB     *scene;          // 场景
        long                lDungeonUID;    // 副本唯一ID
        long                lDungeonTID;    // 副本类型ID
		SFuBen             *info;
        std::list<DWORD>    listUser;       // 用户列表
        time_t              tCreateTime;    // 创建时间
        time_t              tLimitTime;     // 极限时间，超过这个时间会强制退出
		time_t              tEndTime;       // 结束时间，超过这个时间会退出，在所有用户退出副本的时候会设置这个时间
		long				process;		// 副本进度值
		std::vector<STrigger> trigger;		// 机关

		inline STrigger* GetTrigger(DWORD dwID)
		{
			STrigger* pTrigger = NULL;
			std::vector<STrigger>::iterator it = trigger.begin();
			for (;it!=trigger.end();++it)
			{
				if ((*it).id == dwID)
				{
					pTrigger = &(*it);
					break;
				}
			}
			return pTrigger;
		}
    };

public:
    CRsDungeonMgr();
    virtual ~CRsDungeonMgr();

	bool Init();
	bool Exit();
    void Run();

    // 创建副本，收到GWS的创建命令后就要根据指定的信息创建一个副本，需要返回创建是否成功
    bool CreateDungeon(int tableId,int uId,int fbDif,int fbUserLV, int iExitPosX, int iExitPosY);

    // 销毁副本，收到GWS的销毁命令后就要根据指定的信息销毁一个副本，需要返回销毁是否成功
    // 移除所有玩家并销毁NPC
    void DestroyDungeon(int uId, int iExitSecond);

	/*
    // 准备进入，收到准备进入的命令后就加入准备进入的队列，并返回允许准备进入
    //     等客户端连接过来才能算真的连接进入副本
    void PreenterDungeon(DWORD dwUserID, DWORD dwSeed);
	*/

    // 添加玩家
	bool AddUserToDungeon(int uId, DWORD userId, char cBorn, long x, long y);
	bool _AddUserToDungeon(int uId, CRegionUser *user, char cBorn, long x, long y);

    // 移除玩家
    CRegionUser *RemoveUser(CRegionUser *user);

	// 请求进入
	const char* RequestEnter(CRegionUser *user,int fbId,int fbDif,int iUserLV,int iExitX, int iExitY);

	// 请求离开
	bool RequestLeave(CRegionUser *user);

	// 根据唯一ID查找副本
	SRsDungeon *FindDungeon(int uId);

	//卷轴离开副本
	void DungeonLeaveByItem(CRegionUser *user);

    //玩家掉线离开副本
    bool OnDungeonUserLeave(CRegionUser *user);

    void BroadcastSystemMessage(SRsDungeon *pDungeon, const char* szMessage);

	//LeiJun
	long GetDungeonIDByUserID(DWORD dwID);
	long GetDungeonIDBySceneID(long lSceneID);

	// 启动机关
	void StartTrigger(SRsDungeon *fb, DWORD triggerId, bool open = true);
	void StartTrigger(DWORD fbId, DWORD triggerId, bool open = true);
	void StartTriggerRun();
	void SetTriggerBlock(SRsDungeon *fb, STrigger trigger);

public:
    static int s_iMaxLastQuitTime;

private:
    list<SRsDungeon*>                   m_listDungeon;      // 副本的列表
    int                                 m_iMaxDungeon;      // 最大的副本数量 
    //char                                m_szMsg[256];
	CM_MEMDEF(m_szMsg, 256)
	std::list<RunTrigger>					m_listRunTrigger;	// 准备改变状态的机关
	// EXT_SPACE::hash_map<DWORD, SRsDungeon*> m_mapUser;   // 用户索引副本<玩家ID, 所在副本>
};

struct RunTrigger
{
	RunTrigger(CRsDungeonMgr::SRsDungeon *dungeon,DWORD trigger,DWORD time)
	{
		pDungeon = dungeon;
		dwTrigger = trigger;
		dwTime = rtGetMilliseconds() + time;
	}
	CRsDungeonMgr::SRsDungeon *pDungeon;
	DWORD		dwTrigger;
	DWORD		dwTime;
};

#endif // _INC_RS_DUNGEON_H_

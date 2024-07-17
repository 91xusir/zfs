#ifndef _INC_GW_DUNGEON_H_
#define _INC_GW_DUNGEON_H_

// 副本系统
class CGwDungeonMgr
{
public:
    enum EDungeonState
    {
        EDS_UNCREATE,
        EDS_CREATED,
    };
    struct SUser
    {
        DWORD dwUserID;                     // 用户的ID
        bool  bInDungeon;                   // 是否在副本内
    };
    struct SGwDungeon
    {
        long                lDungeonType;   // 副本类型
        SFuBen*             pInfo;          // 副本类型
		long                lDungeonLev;    // 副本难度
		long                lDungeonUserLV;    // 副本用户平均等级
        int                 iRegionIdx;     // 副本所在服务器
        long                lDungeonIdx;    // 副本的编号
        EDungeonState       eDungeonState;  // 副本的状态(0 未创建, 1 已创建)
        std::list<DWORD>    listApplicant;  // 请求进入副本的人，在创建副本的时候这个人的周围的人就会被传送
        std::list<SUser>    listUser;       // 用户列表
        time_t              tCreateTime;    // 创建时间
        time_t              tCreateLimitTime;//创建极限时间，这个时间还没有创建出来就会强制把准备穿送列表删除
        time_t              tLimitTime;     // 极限时间，超过这个时间会强制退出
        time_t              tEndTime;       // 结束时间，超过这个时间会退出，在所有用户退出副本的时候会设置这个时间
    };

public:
    CGwDungeonMgr();
    ~CGwDungeonMgr();

    bool Startup();
    bool Shutdown();
    void UpdateDatabaseID();

    // 副本的，这里会做一些时间相关的判断
    void Run();

    // 进入副本 (如果条件满足)
    //     如果这个人第一次进副本就会创建一个新的副本，并把这个人周围一定范围内同一队伍的人传送进去
    //         注：如果队伍中有任意一个人不满足进入副本的条件，就会进入失败
    //     如果这个人曾经进入副本，就会被传送到指定的副本中
    SGwDungeon* EnterDungeon(DWORD dwUserID, long lDungeonType, long lDungeonLev, long lDungeonUserLV, long lExitX, long lExitY);

    // 离开副本
    //     当副本没有人的时候这个副本退出
    bool LeaveDungeon(DWORD dwUserID);

    // 销毁副本
    bool DestroyDungeon(DWORD dwDungeonUID, long dwExitSecond);

    // Region返回成功或者失败的信息
    void OnCreateDungeonRet(long lDungeonIdx, bool bResult);

    // 查找服务器
    SGwDungeon* FindGwDungeon(long lDungeonIdx);

    // 添加副本服务器
    void AddDungeonRegion(int iRegionID);

    void OnRegionDisconnected(int iRegionID);

	int GetListDungeonSize();//tim.yang  设置副本最大数量
	void SetMaxDungeon(long maxdungeon);//tim.yang  设置副本最大数量
private:
    void _DestroyDungeon(SGwDungeon* pDungeon);

private:
    std::list<SGwDungeon>                   m_listDungeon;      // 副本的列表
    EXT_SPACE::hash_map<long, SGwDungeon*>  m_mapDungeon;       // 副本索引<副本ID, 副本指针>
    EXT_SPACE::hash_map<DWORD, SGwDungeon*> m_mapUser;          // 用户索引副本<玩家ID, 所在副本>
    std::map<int, int>                      m_mapDungeonServer; // 副本服务器<索引, 副本数量>
    long                                    m_lDungeonIdx;      // 当前的索引
    int                                     m_iMaxDungeon;      // 最大的副本数量

private:
    long        m_lTotalDungeon;    // 历史的副本个数，用来做统计用

public:
    const char* m_szErrorInfo;
};

#endif // _INC_GW_DUNGEON_H_

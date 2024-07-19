#ifndef _INC_GW_DUNGEON_H_
#define _INC_GW_DUNGEON_H_

// ����ϵͳ
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
        DWORD dwUserID;                     // �û���ID
        bool  bInDungeon;                   // �Ƿ��ڸ�����
    };
    struct SGwDungeon
    {
        long                lDungeonType;   // ��������
        SFuBen*             pInfo;          // ��������
		long                lDungeonLev;    // �����Ѷ�
		long                lDungeonUserLV;    // �����û�ƽ���ȼ�
        int                 iRegionIdx;     // �������ڷ�����
        long                lDungeonIdx;    // �����ı��
        EDungeonState       eDungeonState;  // ������״̬(0 δ����, 1 �Ѵ���)
        std::list<DWORD>    listApplicant;  // ������븱�����ˣ��ڴ���������ʱ������˵���Χ���˾ͻᱻ����
        std::list<SUser>    listUser;       // �û��б�
        time_t              tCreateTime;    // ����ʱ��
        time_t              tCreateLimitTime;//��������ʱ�䣬���ʱ�仹û�д��������ͻ�ǿ�ư�׼�������б�ɾ��
        time_t              tLimitTime;     // ����ʱ�䣬�������ʱ���ǿ���˳�
        time_t              tEndTime;       // ����ʱ�䣬�������ʱ����˳����������û��˳�������ʱ����������ʱ��
    };

public:
    CGwDungeonMgr();
    ~CGwDungeonMgr();

    bool Startup();
    bool Shutdown();
    void UpdateDatabaseID();

    // �����ģ��������һЩʱ����ص��ж�
    void Run();

    // ���븱�� (�����������)
    //     �������˵�һ�ν������ͻᴴ��һ���µĸ����������������Χһ����Χ��ͬһ������˴��ͽ�ȥ
    //         ע�����������������һ���˲�������븱�����������ͻ����ʧ��
    //     ���������������븱�����ͻᱻ���͵�ָ���ĸ�����
    SGwDungeon* EnterDungeon(DWORD dwUserID, long lDungeonType, long lDungeonLev, long lDungeonUserLV, long lExitX, long lExitY);

    // �뿪����
    //     ������û���˵�ʱ����������˳�
    bool LeaveDungeon(DWORD dwUserID);

    // ���ٸ���
    bool DestroyDungeon(DWORD dwDungeonUID, long dwExitSecond);

    // Region���سɹ�����ʧ�ܵ���Ϣ
    void OnCreateDungeonRet(long lDungeonIdx, bool bResult);

    // ���ҷ�����
    SGwDungeon* FindGwDungeon(long lDungeonIdx);

    // ��Ӹ���������
    void AddDungeonRegion(int iRegionID);

    void OnRegionDisconnected(int iRegionID);

	int GetListDungeonSize();//tim.yang  ���ø����������
	void SetMaxDungeon(long maxdungeon);//tim.yang  ���ø����������
private:
    void _DestroyDungeon(SGwDungeon* pDungeon);

private:
    std::list<SGwDungeon>                   m_listDungeon;      // �������б�
    EXT_SPACE::hash_map<long, SGwDungeon*>  m_mapDungeon;       // ��������<����ID, ����ָ��>
    EXT_SPACE::hash_map<DWORD, SGwDungeon*> m_mapUser;          // �û���������<���ID, ���ڸ���>
    std::map<int, int>                      m_mapDungeonServer; // ����������<����, ��������>
    long                                    m_lDungeonIdx;      // ��ǰ������
    int                                     m_iMaxDungeon;      // ���ĸ�������

private:
    long        m_lTotalDungeon;    // ��ʷ�ĸ���������������ͳ����

public:
    const char* m_szErrorInfo;
};

#endif // _INC_GW_DUNGEON_H_

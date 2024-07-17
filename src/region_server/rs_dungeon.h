#ifndef _INC_RS_DUNGEON_H_
#define _INC_RS_DUNGEON_H_

// ����ϵͳ
/*
region-����������ĳһ�Ѷȵĸ���
region-֪ͨgameworld
gameworld-����team������飬�ȼ����Ƿ��Ѿ������һ�Ѷȸ�������
gameworld-ѡ�񸱱�region
����region-��������
gameworld-������ҽ��븱��region
����region-������ң��ٴ����������
*/

struct RunTrigger;

class CRsDungeonMgr : public CRegionObject
{
public:
    struct SRsDungeon
    {
		CRegionSceneFB     *scene;          // ����
        long                lDungeonUID;    // ����ΨһID
        long                lDungeonTID;    // ��������ID
		SFuBen             *info;
        std::list<DWORD>    listUser;       // �û��б�
        time_t              tCreateTime;    // ����ʱ��
        time_t              tLimitTime;     // ����ʱ�䣬�������ʱ���ǿ���˳�
		time_t              tEndTime;       // ����ʱ�䣬�������ʱ����˳����������û��˳�������ʱ����������ʱ��
		long				process;		// ��������ֵ
		std::vector<STrigger> trigger;		// ����

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

    // �����������յ�GWS�Ĵ���������Ҫ����ָ������Ϣ����һ����������Ҫ���ش����Ƿ�ɹ�
    bool CreateDungeon(int tableId,int uId,int fbDif,int fbUserLV, int iExitPosX, int iExitPosY);

    // ���ٸ������յ�GWS������������Ҫ����ָ������Ϣ����һ����������Ҫ���������Ƿ�ɹ�
    // �Ƴ�������Ҳ�����NPC
    void DestroyDungeon(int uId, int iExitSecond);

	/*
    // ׼�����룬�յ�׼������������ͼ���׼������Ķ��У�����������׼������
    //     �ȿͻ������ӹ���������������ӽ��븱��
    void PreenterDungeon(DWORD dwUserID, DWORD dwSeed);
	*/

    // ������
	bool AddUserToDungeon(int uId, DWORD userId, char cBorn, long x, long y);
	bool _AddUserToDungeon(int uId, CRegionUser *user, char cBorn, long x, long y);

    // �Ƴ����
    CRegionUser *RemoveUser(CRegionUser *user);

	// �������
	const char* RequestEnter(CRegionUser *user,int fbId,int fbDif,int iUserLV,int iExitX, int iExitY);

	// �����뿪
	bool RequestLeave(CRegionUser *user);

	// ����ΨһID���Ҹ���
	SRsDungeon *FindDungeon(int uId);

	//�����뿪����
	void DungeonLeaveByItem(CRegionUser *user);

    //��ҵ����뿪����
    bool OnDungeonUserLeave(CRegionUser *user);

    void BroadcastSystemMessage(SRsDungeon *pDungeon, const char* szMessage);

	//LeiJun
	long GetDungeonIDByUserID(DWORD dwID);
	long GetDungeonIDBySceneID(long lSceneID);

	// ��������
	void StartTrigger(SRsDungeon *fb, DWORD triggerId, bool open = true);
	void StartTrigger(DWORD fbId, DWORD triggerId, bool open = true);
	void StartTriggerRun();
	void SetTriggerBlock(SRsDungeon *fb, STrigger trigger);

public:
    static int s_iMaxLastQuitTime;

private:
    list<SRsDungeon*>                   m_listDungeon;      // �������б�
    int                                 m_iMaxDungeon;      // ���ĸ������� 
    //char                                m_szMsg[256];
	CM_MEMDEF(m_szMsg, 256)
	std::list<RunTrigger>					m_listRunTrigger;	// ׼���ı�״̬�Ļ���
	// EXT_SPACE::hash_map<DWORD, SRsDungeon*> m_mapUser;   // �û���������<���ID, ���ڸ���>
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

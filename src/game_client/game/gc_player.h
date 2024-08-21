
/*----------------------------------------------------------------------------
-   ��ң���������������Լ�����Ϣ�������Լ��Ķ������Ϣ
----------------------------------------------------------------------------*/
#ifndef GC_PLAYER_H
#define GC_PLAYER_H
#include "gc_actor.h"

class GcPlayer : public GcActor
{
public:
    GcPlayer();
    virtual ~GcPlayer();

	virtual void OnInit();
	virtual void OnNetUploadAttack();
    virtual void OnPlayFootfall();

	DWORD DBID()				{return mDBID;}
	void SetDBID(DWORD vID)		{mDBID=vID;}
	void SetTeamOff(long teamoff);

//    bool CanAttack(GcActor* pTargetActor);
//    bool AttackKeyHitActor(GcActor* pTargetActor);
//    bool SkillKeyHitActor(int iX, int iY);

    void OnSetTarget(DWORD vID);
	DWORD OnGetTarget()			{return m_dwLatestTarget;}
    virtual void OnPositionChanged(float fX, float fY);
    virtual void OnRemoveCreature();
    bool ActorInSkillTarget(GcActor* pActor, SSkill* pSkill);
    void ReadMatchFromNet(CG_CmdPacket *cmd);
	/* gao 2010.1.29 ����ϵͳ���к������ */
	DWORD GetGroupID(){guard;return m_dwGroupID;unguard;}
	DWORD GetLineID(){guard;return m_dwLineID;unguard;}
	void SetGroupID(DWORD id){guard;m_dwGroupID = id;unguard;}
	void SetLineID(DWORD id){guard;m_dwLineID = id;unguard;}
	/* end */
	bool OnGetPlayerIsMoving();

	virtual void AddCommandMoveTo(float fX, float fY, float fZ, float fTime, float fDistance);

	virtual void Run(float fSecond);

	int GetMoveCmdNum();

	virtual bool CheckCanStartTrigger( DWORD triggerID, bool checkRemove = false );
	virtual bool ProcessKey(STaskKey &key);
	virtual bool SetModel(short Id, short vHeadModelID);

	char                m_posFlag;							// ����У��λ��
    DWORD               m_dwLatestTarget;                   // ���һ�ε�Ŀ��
	std::list<short>        m_matchList;					// ѧ����䷽�б�

protected:
    virtual void OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock);
    virtual void OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock); // ������ĳ������
    virtual void OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock);  // ���뿪ĳ������
	virtual void UpdateAreaPKAttr();
	virtual void DoCommand(SCmd* pCmd);

private:
	virtual void OnCreateGraph();

	DWORD				    mDBID;								// ���ݿ�ΨһID
	/* gao 2010.1.29 ����ϵͳ�������� */
	DWORD					m_dwGroupID;							// ��ǰ�û����ڷ�������ID
	DWORD					m_dwLineID;							// ��ǰ�û�������ID
	/* end */

	//copy from player.h   ac.ma
	//////////////////////////////////////////////////////////////////////////
public:
	
	DWORD		m_dwRedBtlCoolDown;
	DWORD		m_dwBlueBtlCoolDown;
	DWORD		m_dwMoveCoolDown;
	DWORD		m_dwPickItemCoolDown;
	DWORD		m_dwBattleCoolDown;

	DWORD		m_dwHPOperateTime;
	DWORD		m_dwMPOperateTime;

	//add by yz 2010-6-30:��¼���ҩ��ʹ��ʱ��
	DWORD       m_dwHPPetOperateTime;
	DWORD       m_dwMPPetOperateTime;
	//end yz

	int			m_iStickyX;
	int			m_iStickyY;

	bool		m_bShowID;

// 	float		m_iLastX;
// 	float		m_iLastY;

	std::list<SPkTempInfo> m_listTempPkInfo;
	DWORD		m_dwGetDropTime;

	SItemID		ChooseRedInBag(int lackHp,bool bLowThenThirty);

	void		_auto_eat_red();
	void		_auto_eat_blue();
	void		_auto_battle();

	void		_auto_pick_item();
	void		_auto_sell_item();

public:
	void SetAutobattle(bool autobattle)			//judge m_bAutoState		ac.ma
	{
		m_bAutoBattle = autobattle;

		if (m_bAutoBattle)	//heten
		{
			m_bAutoState = true;
			SetStickyPosition();
		}else
			m_bAutoState = false;	

		m_pLastTargetID = 0;
	}
	void SetStickyPosition();		//ac.ma

	void SetAutoEatRed(bool autoeatred){m_bAutoEatRed = autoeatred;}
	void SetAutoEatBlue(bool autoeatblue);
	
	//change by yz 2010-7-1: �ı��Ѫ����
	void SetAutoHp30(bool autohp30)
	{
		m_bChooseAutoHp30 = autohp30; 
		if(autohp30)
		{
			m_iAutoHp = 30;
			m_iAutoMp = 30;
		}
	}
	void SetAutoHp60(bool autohp60)
	{
		m_bChooseAutoHp60 = autohp60;
		if(autohp60)
		{
			m_iAutoHp = 60;
			m_iAutoMp = 60;
		}
	}
	void SetAutoHp90(bool autohp90)
	{
		m_bChooseAutoHp90 = autohp90;
		if(autohp90)
		{
			m_iAutoHp = 90;
			m_iAutoMp = 90;
		}
	}
	//end yz
	void SetAutoPick(bool autopick)
	{
		m_bAutoPick = autopick;

		//if (m_bAutoPick || m_bAutoBattle)
		//	m_bAutoState = true;
		//else
		//	m_bAutoState = false;
		//ע�� by heten
	}
	void SetAutoState(bool autostate){m_bAutoState = autostate;}
	bool GetAutobattle(){return m_bAutoBattle;}
	bool GetAutoState(){return m_bAutoState;}

	void AddPkTempUser(DWORD id,DWORD time = 0);

private:
	bool m_bAutoState;		//����Ƿ����Զ�״̬		ac.ma

	bool m_bAutoPick;		//�Ƿ��Զ�ʰȡ
	bool m_bAutoBattle;		//�Ƿ��Զ�ս��
	//bool m_bQuickBattle;	//˲�ƴ��

	bool m_bAutoEatRed;	    //�Ƿ��Զ���Ѫ
	bool m_bAutoEatBlue;	//�Ƿ��Զ�����
	bool m_bChooseAutoHp30;
	bool m_bChooseAutoHp60;
	bool m_bChooseAutoHp90;
	int  m_iAutoMp;			//��Ѫ����
	int  m_iAutoHp;			//��������

	bool m_isGoBack;//�Ƿ����ڷ��عһ���
	int m_iLastX;
	int m_iLastY;
	int m_iAddSettleTime;//��¼��һ��λ��ͣ����ʱ��
	GcSynchroActor *m_curMonster;//������¼����ұ���סʱ��ǰ���ڹ����Ĺ���

	//for auto sell				//ac.ma
public:
	int m_Point;		//heten
	void SetAutoSellWhite(bool autowhite){m_bWhiteAutoSell = autowhite;}
	void SetAutoSellGreen(bool autogreen){m_bGreenAutoSell = autogreen;}
	void SetAutoSellBlue(bool autoblue){m_bBlueAutoSell = autoblue;}
	void SetAutoSellGold(bool autogold){m_bGoldAutoSell = autogold;}
	bool GetAutoSellWhite(){return m_bWhiteAutoSell;}
	bool GetAutoSellGreen(){return m_bGreenAutoSell;}
	bool GetAutoSellBlue(){return m_bBlueAutoSell;}
	bool GetAutoSellGold(){return m_bGoldAutoSell;}	
private:
	bool		m_bWhiteAutoSell;
	bool		m_bGreenAutoSell;
	bool		m_bBlueAutoSell;
	bool		m_bGoldAutoSell;

private:

	float   _distant(GcActor* sMonster);
	float	_distant(long x, long y);

	//change by yz
	// Return Item ID
	SItemID FindSmallRedbottleInBag()
	{
		std::vector<DWORD> typeArray;
		//typeArray.push_back(401);
		//typeArray.push_back(404);
		//typeArray.push_back(411);
		//typeArray.push_back(412);
		//typeArray.push_back(441);
		//typeArray.push_back(444);
		//typeArray.push_back(651);
		//typeArray.push_back(10015);		//50Ѫ  ���Ͳ�
		typeArray.push_back(6001);     //��Ʒ������
		typeArray.push_back(6018);     //��Ʒ������
		return _find_item_in_bag_by_types(typeArray);
	}

	SItemID FindMiddleRedbottleInBag()
	{
		std::vector<DWORD> typeArray;
		//typeArray.push_back(402);
		//typeArray.push_back(405);
		//typeArray.push_back(442);
		//typeArray.push_back(445);
		//typeArray.push_back(652);
		//typeArray.push_back(669);
		//typeArray.push_back(674);
		//typeArray.push_back(812);
		//typeArray.push_back(1134);
		////����� ���ٷֱȼ�Ѫ��ҩƿ
		//typeArray.push_back(655);
		//typeArray.push_back(9999);
		//typeArray.push_back(1052);
		typeArray.push_back(6003);    //��Ʒ������
		typeArray.push_back(6020);    //��Ʒ������
		return _find_item_in_bag_by_types(typeArray);
	}

	SItemID FindLargeRedbottleInBag()
	{
		std::vector<DWORD> typeArray;
		//typeArray.push_back(403);
		//typeArray.push_back(443);
		//typeArray.push_back(672);
		//typeArray.push_back(816);
		//typeArray.push_back(1402);
		//typeArray.push_back(1404);
		//typeArray.push_back(4402);
		//typeArray.push_back(4404);
		//typeArray.push_back(655);
		//typeArray.push_back(9999);
		//typeArray.push_back(1052);
		//typeArray.push_back(4504);	//500 Ѫƿ
		//typeArray.push_back(4629);	//500 Ѫƿ
		typeArray.push_back(6005);	//һƷ������
		typeArray.push_back(6022);	//һƷ������
		return _find_item_in_bag_by_types(typeArray);
	}

	SItemID FindSmallBluebottleInBag()
	{
		std::vector<DWORD> typeArray;
		/*typeArray.push_back(406);
		typeArray.push_back(409);
		typeArray.push_back(413);
		typeArray.push_back(446);
		typeArray.push_back(449);
		typeArray.push_back(653);*/
		typeArray.push_back(6002);    //��Ʒ������
		typeArray.push_back(6019);    //��Ʒѩ����
		return _find_item_in_bag_by_types(typeArray);
	}

	SItemID FindMiddleBluebottleInBag()
	{
		std::vector<DWORD> typeArray;
		/*typeArray.push_back(407);
		typeArray.push_back(410);
		typeArray.push_back(447);
		typeArray.push_back(450);
		typeArray.push_back(654);
		typeArray.push_back(670);
		typeArray.push_back(813);
		typeArray.push_back(1135);*/
		typeArray.push_back(6004);    //��Ʒ������
		typeArray.push_back(6021);    //��Ʒѩ����
		return _find_item_in_bag_by_types(typeArray);
	}

	SItemID FindLargeBluebottleInBag()
	{
		std::vector<DWORD> typeArray;
		//typeArray.push_back(408);
		//typeArray.push_back(448);
		//typeArray.push_back(671);
		//typeArray.push_back(817);
		//typeArray.push_back(1403);
		//typeArray.push_back(1405);
		//typeArray.push_back(4403);
		//typeArray.push_back(4405);
		//typeArray.push_back(4505);		//500��ƿ
		//typeArray.push_back(4630);		//500��ƿ
		typeArray.push_back(6006);    //һƷ������
		typeArray.push_back(6023);    //һƷѩ����
		return _find_item_in_bag_by_types(typeArray);
	}
	//end
	//add by yz
	bool _ableUseItem(SItemID& rItemID);    //�ж��ܷ�ʹ��item
	void _commonAttack(DWORD dwTargetID);   //��ͨ���� @Ŀ��ID
	void _skillAttack(GcSynchroActor *pTarget, const SSkill* pSkill);  //���ܹ��� @1Ŀ��ID��@2����ID
	//end

private:
	SItemID _find_item_in_bag_by_types(std::vector<DWORD>& typeArray);
	//{
	//	int nCount = 0, nPage = 0, nI = 0, nJ = 0;
	//	for (std::vector<DWORD>::iterator it = typeArray.begin();
	//		it != typeArray.end(); ++it)
	//	{
	//		if (mItem.m_Bag.FindType(*it, ItemColor_All, &nCount, &nPage, &nI, &nJ))
	//			return mItem.m_Bag.GetItem(nPage, nI, nJ).id;
	//	}

	//	return 0;
	//}
public:
	void UseItem(SItemID lItem, int iBagLine = 0, int iBagColumn = 0, int iPage = 0);
	//{
	//	if (lItemID <= 0) return;

	//	if (iBagLine == 0 || iBagColumn == 0)
	//	{
	//		if (mItem.m_Bag.Find(lItemID, &iPage, &iBagLine, &iBagColumn))
	//			CItemNetHelper::NetSend_c2r_player_use_item(lItemID, ITEM_CONTAINER_PLAYER_COMMON_BAG, iPage, iBagLine, iBagColumn);
	//	}
	//}

	//////////////////////////////////////////////////////////////////////////
	//copy end   ac.ma
};



#endif//GC_PLAYER_H

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

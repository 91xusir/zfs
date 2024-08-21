
/*----------------------------------------------------------------------------
-   玩家，这里描述了玩家自己的信息，包括自己的队伍等信息
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
	/* gao 2010.1.29 分线系统所有函数相关 */
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

	char                m_posFlag;							// 用于校正位置
    DWORD               m_dwLatestTarget;                   // 最后一次的目标
	std::list<short>        m_matchList;					// 学会的配方列表

protected:
    virtual void OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock);
    virtual void OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock); // 当进入某个区域
    virtual void OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock);  // 当离开某个区域
	virtual void UpdateAreaPKAttr();
	virtual void DoCommand(SCmd* pCmd);

private:
	virtual void OnCreateGraph();

	DWORD				    mDBID;								// 数据库唯一ID
	/* gao 2010.1.29 分线系统所用数据 */
	DWORD					m_dwGroupID;							// 当前用户所在服务器组ID
	DWORD					m_dwLineID;							// 当前用户所在线ID
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

	//add by yz 2010-6-30:记录骑宠药的使用时间
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
	
	//change by yz 2010-7-1: 改变加血底线
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
		//注释 by heten
	}
	void SetAutoState(bool autostate){m_bAutoState = autostate;}
	bool GetAutobattle(){return m_bAutoBattle;}
	bool GetAutoState(){return m_bAutoState;}

	void AddPkTempUser(DWORD id,DWORD time = 0);

private:
	bool m_bAutoState;		//玩家是否处于自动状态		ac.ma

	bool m_bAutoPick;		//是否自动拾取
	bool m_bAutoBattle;		//是否自动战斗
	//bool m_bQuickBattle;	//瞬移打怪

	bool m_bAutoEatRed;	    //是否自动补血
	bool m_bAutoEatBlue;	//是否自动补蓝
	bool m_bChooseAutoHp30;
	bool m_bChooseAutoHp60;
	bool m_bChooseAutoHp90;
	int  m_iAutoMp;			//补血底线
	int  m_iAutoHp;			//补蓝底线

	bool m_isGoBack;//是否正在返回挂机点
	int m_iLastX;
	int m_iLastY;
	int m_iAddSettleTime;//记录在一个位置停留的时间
	GcSynchroActor *m_curMonster;//用来记录被玩家被卡住时当前正在攻击的怪物

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
		//typeArray.push_back(10015);		//50血  酥油茶
		typeArray.push_back(6001);     //三品续命丹
		typeArray.push_back(6018);     //三品参王膏
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
		////新添加 按百分比加血的药瓶
		//typeArray.push_back(655);
		//typeArray.push_back(9999);
		//typeArray.push_back(1052);
		typeArray.push_back(6003);    //二品续命丹
		typeArray.push_back(6020);    //二品参王膏
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
		//typeArray.push_back(4504);	//500 血瓶
		//typeArray.push_back(4629);	//500 血瓶
		typeArray.push_back(6005);	//一品续命丹
		typeArray.push_back(6022);	//一品参王膏
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
		typeArray.push_back(6002);    //三品续精丹
		typeArray.push_back(6019);    //三品雪莲汤
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
		typeArray.push_back(6004);    //二品续精丹
		typeArray.push_back(6021);    //二品雪莲汤
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
		//typeArray.push_back(4505);		//500蓝瓶
		//typeArray.push_back(4630);		//500蓝瓶
		typeArray.push_back(6006);    //一品续精丹
		typeArray.push_back(6023);    //一品雪莲汤
		return _find_item_in_bag_by_types(typeArray);
	}
	//end
	//add by yz
	bool _ableUseItem(SItemID& rItemID);    //判断能否使用item
	void _commonAttack(DWORD dwTargetID);   //普通攻击 @目标ID
	void _skillAttack(GcSynchroActor *pTarget, const SSkill* pSkill);  //技能攻击 @1目标ID，@2技能ID
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

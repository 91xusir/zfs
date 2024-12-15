//
// Region creature cpp file
//
#include "region.h"
//--------------------add start by tony 05.05.23-----------------------------//
#include "region_guarder.h"
//--------------------add end   by tony 05.05.23-----------------------------//
#include "region_prentice.h"
#include "rs_national_war.h"
#include "jhwg_session.h"

//PZH
#include "PlayerLogic.h"
//

//LeiJun
#include "RegionActManager.h"

#define TOTAL_OFFLINE_TIME 18000  //18000		ac.ma
#define EVERY_PER_TIME 900        //900		ac.ma

extern CRS_Problem g_TableProblem;

bool SAttackParam::Serialize(CG_CmdPacket& Cmd) {
    if (Cmd.IsReading()) {
        if (!Cmd.ReadShort(&Seed))
            return false;
        if (!Cmd.ReadShort((short*)(&crcCheck)))
            return false;
        if (!Cmd.ReadShort(&Skill))
            return false;
        if (!Cmd.ReadByte(&TargetType))
            return false;
        if (!Cmd.ReadLong(&LuaCheck))
            return false;

        if (TargetType == SKILL_TARGET_TYPE_POSITION) {
            if (!Cmd.ReadLong(&Pos[0]))
                return false;
            if (!Cmd.ReadLong(&Pos[1]))
                return false;
            if (!Cmd.ReadLong(&Pos[2]))
                return false;
        } else if (TargetType == SKILL_TARGET_TYPE_ACTOR) {
            if (!Cmd.ReadLong((long*)&EnemyId))
                return false;
        } else if (TargetType == SKILL_TARGET_TYPE_NONE) {
        } /*PZH*/ else if (SKILL_TARGET_TYPE_BREAK == TargetType) {
        }  //
        else {
            return false;
        }
    } else {
        if (!Cmd.WriteShort(Seed))
            return false;
        if (!Cmd.WriteShort(crcCheck))
            return false;
        if (!Cmd.WriteShort(Skill))
            return false;
        if (!Cmd.WriteByte(TargetType))
            return false;
        if (!Cmd.WriteLong(LuaCheck))
            return false;

        if (TargetType == SKILL_TARGET_TYPE_POSITION) {
            if (!Cmd.WriteLong(Pos[0]))
                return false;
            if (!Cmd.WriteLong(Pos[1]))
                return false;
            if (!Cmd.WriteLong(Pos[2]))
                return false;
        } else if (TargetType == SKILL_TARGET_TYPE_ACTOR) {
            if (!Cmd.WriteLong((long)EnemyId))
                return false;
        } else if (TargetType == SKILL_TARGET_TYPE_NONE) {
        } /*PZH*/ else if (SKILL_TARGET_TYPE_BREAK == TargetType) {
        }  //
        else {
            return false;
        }
    }
    return true;
}

/*
CG_CmdPacket& operator<<(CG_CmdPacket& Cmd, SAttackParam &p)
{
	Cmd << p.crcCheck << p.Seed << p.Skill << p.TargetType;
	if(p.TargetType == SKILL_TARGET_TYPE_POSITION)
	{
		Cmd << p.Pos[0] << p.Pos[1] << p.Pos[2];
	}
	else if(p.TargetType == SKILL_TARGET_TYPE_ACTOR)
	{
		Cmd << p.EnemyId;
	}

	//Cmd << p.Damage;
	//for(int i=0; i<MAX_ELEMENT_ATTR; i++)
	//	Cmd << p.EleDmg[i];
	return Cmd;
}
*/

// ----------------------------------------------------------------------------------
// Region creature
// ----------------------------------------------------------------------------------

//-----------------add start by tony 05.07.15---------------------//
#define NUM(array) (sizeof(array) / sizeof((array[0])))
const std::string SceneNameZhou[] = {"scene01_002.rtb", "scene01_003.rtb", "scene01_004.rtb",
                                     "scene01_005.rtb", "scene01_008.rtb", "scene01_009.rtb",
                                     "scene01_010.rtb", "scene01_011.rtb", "scene01_012.rtb",
                                     "scene01_013.rtb", "scene01_045.rtb", "scene01_046.rtb"};

const std::string SceneNameShang[] = {"scene01_025.rtb", "scene01_026.rtb", "scene01_027.rtb",
                                      "scene01_028.rtb", "scene01_029.rtb", "scene01_030.rtb",
                                      "scene01_035.rtb", "scene01_036.rtb", "scene01_037.rtb",
                                      "scene01_038.rtb", "scene01_039.rtb", "scene01_040.rtb"};

//-----------------add end   by tony 05.07.15---------------------//

CRegionCreature::CRegionCreature() : /*PZH*/ m_pPlayerLogic(NULL) {
    CM_MEMPROTECTOR(m_skillAttackRate, NPC_MAX_SKILL)

    m_core = SCreature();

    time_t tmpTime = time(NULL);
    tm     now     = *(localtime(&tmpTime));
    localTime      = now.tm_mday;

    m_WorldChatTime  = 0;
    m_UseItemEggTime = 0;
    m_IsDead         = 0;
    tempItemID       = 0;
    m_IsInvincible   = 0;
    m_IsFirstLogin   = 0;
    m_modelRef       = 1;
    //m_bStarted = false;
    m_iGuardID = 0;
    m_WardNPC  = NULL;
    m_master   = NULL;
    m_pet      = NULL;
    m_hero     = NULL;
    m_bRidePet = false;
    m_userInfo = NULL;
    m_dir      = 0;
    m_scene    = NULL;
    m_pos[0] = m_pos[1] = m_pos[2] = 0;
    m_ai                           = NULL;
    m_bStatic                      = false;
    m_lair                         = NULL;
    m_actorInfo                    = NULL;
    m_enemy                        = 0;
    m_spawn                        = NULL;

    m_pTeam   = NULL;  //队伍
    m_pMail   = NULL;  //邮件
    m_pFriend = NULL;  //好友列表

    m_assistRange = 0;
    m_faction     = 0;

    m_iCurrentBlockX = m_iCurrentBlockY = -1;

    m_pItemContainer = NULL;
    m_pBank          = NULL;
    m_pTrade         = NULL;
    m_pPShop         = NULL;
    m_bPShop         = 0;

    m_combatRule       = COMBAT_RULE_AGGRESSIVE;
    m_bravePoint       = 100;
    m_unitedPoint      = 100;
    m_loyalPoint       = 100;
    m_angryPointLevel1 = 0;
    m_angryPointLevel2 = 0;
    m_patrolRate       = 10;
    m_ai_level         = 0;

    m_NameColor = -1;
    m_vResideObjectID.clear();
    vPopularity.clear();

    m_traceRange = 200;
    m_npcInfo    = NULL;

    m_hpRemain          = 0;
    m_mpRemain          = 0;
    m_EnpRemain         = 0;
    m_hpRecover         = 0;
    m_mpRecover         = 0;
    m_EnpRecover        = 0;
    m_hpAutoRecover     = 0;
    m_mpAutoRecover     = 0;
    m_hpAutoRecoverRate = 0;
    m_mpAutoRecoverRate = 0;
    m_SkillhpRecover    = 0;
    m_SkillmpRecover    = 0;
    m_npcId             = 0;
    m_combatBusy        = 0;
    m_CcombatBusy       = 0;
    m_usePetBusy        = 0;
    m_pathNode          = NULL;

    m_popularity = "";

    m_unionID       = 0;
    m_unionName     = "";
    m_unionNickName = "";
    m_unionIcon     = UNION_DEFAULT_ICON_NAME;
    //-------------add start by tony 05.06.13--------------------------------//
    m_feod_name   = "";
    m_is_attacker = (BYTE)0;
    //-------------add end   by tony 05.06.13--------------------------------//

    m_pSellBag = NULL;

    // m_iPKStateChangeTime = 0;
    m_eCurPosPKAttr   = PKA_NONE;
    m_eBlockPosPKAttr = PKA_NONE;

    m_Skill.Init(this);
    m_lockTarget = false;

    m_dwPetItemOID = 0;

    m_configData = "";
    m_posFlag    = 0;

    m_cuffDamageMin = 0;
    m_cuffDamageMax = 0;
    m_cuffAttRange  = 0;
    m_cuffAttSpeed  = 0;

    //m_bDead = false;//PZH

    m_nGetMoneyFromMonster = 0;
    m_nGetMoneyFromTask    = 0;
    m_nGetItemCount        = 0;
    m_nGetExp              = 0;
    m_nMonsterCount        = 0;
    m_MonitorLastTime      = time(NULL);
    m_lastEmoteTime        = 0;

    m_petExpRate       = 20;
    m_petCounterAttack = true;

    m_dwState  = 0xffffffff;
    m_bFunKick = false;

    m_dungeonUID         = 0;
    m_dungeonTID         = 0;
    m_dungeonLastQuit    = 0;
    m_crcCheckFlag       = 0;
    m_wgChecked          = 0;
    m_wgCheckedTime      = 0;
    m_wgMaybeChecked     = 0;
    m_wgMaybeCheckedTime = 0;
    m_tNextCheckProcess  = 0;
    m_lAbsClientTime     = 0;
    m_lPosCmdCnt         = 0;
    m_lFirstPosCmdTime   = 0;
    m_dwLastPickItemTime = 0;

    m_iUserMoveFailedCnt    = 0;
    m_lLastCheckProcessTime = 0;

    m_cheatKick  = 0;
    m_cheatDummy = false;

    m_petAbsorbMp = 0;
    m_petAbsorbHp = 0;

    CulAwardCount = 0;

    localTime = 0;  //保存当前的日子

    ExtractHpR   = 0;  //百分比抽取目标血量，但最多不能多于一个固定值
    ExtractMaxHp = 0;  //抽取目标血量最大固定值

    m_ChangeModelID = 0;  //保存变身后的模型ID

    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        TemporaryElementArmor[i]  = 0.00f;
        TemporaryElementArmor1[i] = 0.00f;
        ElementFactor[i]          = 0.00f;
        ElementFactor1[i]         = 0.00f;
    }

    m_mtFinish[0] = 0;
    m_mtFinish[1] = 0;
    m_mtFinish[2] = 0;

    m_mtClearDay = 0;
    m_mtBusy     = 0;

    m_wenTaskAccept = 0;
    m_wenTaskFinish = 0;
    m_wenClearDay   = 0;

    m_gongTaskAccept = 0;
    m_gongTaskFinish = 0;
    m_gongClearDay   = 0;

    m_UnionTask     = 0;
    m_UnionClearDay = 0;

    m_MouTaskFinish = 0;
    m_MouTaskAccept = 0;
    m_MouClearDay   = 0;

    m_cardTime          = 0;
    m_usingCard         = 0;
    m_cardLastTime      = 0;
    m_LastDieTime       = 0;
    m_bIsInWarfield     = false;
    m_lWarfieldLastQuit = 0;

    m_bNpcSellRareItemStart       = false;
    m_bScapegoatRemovedDuringDead = false;
    m_valid                       = true;

    m_tmpcurrCircle     = 0;
    m_tmpcurrMonsterNum = 0;
    m_tmpfinditemID     = 0;
    m_tmpfindnpcID      = 0;
    m_tmpkillmonsterID  = 0;
    m_lastEquipItemTime = 0;

    //m_pHuiYuanHoptoad = 0;//tim.yang   会员蟾蜍精
    //m_HuiYuanHoptoadCount = 0;
    m_curHuiYuanHoptoadOid = 0;
    m_huiyuanTime          = 0;
    //m_isHuiYuan = false;

    //神灯
    memset(m_GodLightMonster, 0, sizeof(m_GodLightMonster));
    m_IsAddMonster = 0;
    m_monsterCount = 0;
    m_bIsDead      = 0;
    m_bIsOutOfLine = 0;
    m_bIsFBReborn  = 0;
    m_FBID         = 0;

    //rendh 2010.01.21
    m_dwGroupID = 0;
    m_dwLineID  = 0;

    //tim.yang  MC
    m_mc          = CMonCastle::GetMCInstance();
    m_isAddMCTask = false;

    //rige
    m_iKillMonsterNum = 0;
    m_iRigelevel      = 0;

    m_isVip = 0;

    //ac.ma	for防沉迷
    m_playerLastLogoutTime = 0;
    m_playerNowLoginTime   = 0;
    m_iIncomeState         = 100;
    m_lTotalOnline         = 0;
    m_lTotalOffline        = 0;
    m_bforGetTime          = false;
    m_ltempOnline          = 0;
    m_lLastTotalOnline     = 0;

    OriginalLevel = 0;
    m_bIsRining   = false;  // 是否在修炼法宝
    m_lIsCombat   = 0;
    m_IsHiding    = 0;

    m_lTriggerTime = 0;
    m_nFBprocess   = 0;
    triggerID      = 0;
    triggerMain    = 0;
    triggerOpen    = 0;
    triggerIsClose = 0;

    m_bTime     = true;
    m_dwPetSave = 0;
    m_vPkGetItemList.clear();
    m_dwPkGetItemTime = 0;
    m_dwProtectTime   = 0;

    m_ePkMode        = PK_Monster;
    m_dwLastModelID  = 0;
    m_bBroadcastTele = false;
    m_dwCatchMe      = 0;
    m_dwCatchPet     = 0;
    m_dwLinkUpPet    = 0;

    m_ResistSwim      = 0;  //抵抗眩晕
    m_ResistLethargy  = 0;  //抵抗昏睡
    m_ResistBleed     = 0;  //抵抗流血
    m_ResistBluntness = 0;  //抵抗迟钝

    m_ReceiveDamageSkillR = 0;
    m_TeamShareDamage     = 0;

    m_ScopeoDinaryInjury = 0;
    m_EffectProbability  = 0;  //普攻触发特效的概率
    m_PTEffectID         = 0;  //触发特效的ID

    m_SkillSeries    = 0;  //要增加伤害的技能系列
    m_SkilldamageR   = 0;  //要增加伤害
    m_UltimateChange = 0;
    ReboundHurtR     = 0;
    ReboundHurtRType = 0;
    IsDoTeleport     = false;

    m_bRefuseTeam    = 0;
    m_bRefusetrade   = 0;
    m_bRefuseMail    = 0;
    m_bRefuseDuel    = 0;
    m_bRefuseFriend  = 0;
    m_bRefuseMonster = 0;

    // [8/20/2009 tim.yang] 双倍经验瓶使用限制
    InitCardMinBottleParam();  //初始化使用次数和上次使用时间
}

CRegionCreature::~CRegionCreature() {
    CM_MEMUNPROTECTOR(m_skillAttackRate)

    if (m_pathNode) {
        if (m_scene && m_scene->m_pPathFind) {
            m_scene->m_pPathFind->RemoveActor(m_pathNode);
            LOG("CRegionCreature::~CRegionCreature 在退出的时候才进行移除路径点操作\n");
        }
    }
    if (m_ai)
        g_aiMgr.CloseAI(m_ai);
    if (m_pItemContainer)
        DEL_ONE(m_pItemContainer);
    if (m_pSellBag)
        DEL_ONE(m_pSellBag);
    if (m_pPShop)
        DEL_ONE(m_pPShop);
    if (m_pBank)
        DEL_ONE(m_pBank);
    if (IsUser() && m_IsAddMonster)  //tim.yang
    {
        m_bIsOutOfLine = true;
        GodLightCreateMonster_DelAllMonster();  //tim.yang 神灯   玩家死亡清除掉神灯召唤出来的monster
    }
    if (IsMonster() && this->m_npcId == 1960) {
        this->m_npcId = 0;
        this->m_scene = 0;
    }
    //tim.yang  会员功能
    //if(IsUser() && m_pHuiYuanHoptoad)
    //{
    //	RemoveHuiYuanHoptoad();
    //}
    //end
    if (IsMonster())  //tim.yang  MC
    {
        m_mc->CalMonsterNum();
    }
    if (IsUser() && m_isAddMCTask) {
        m_isAddMCTask = false;
    }
    if (IsMonster() && m_mc) {
        m_mc->EraseMonsterFromList(this);
    }
    //PZH
    DEL_ONE(m_pPlayerLogic);
    //
}

//PZH
CPlayerLogic* CRegionCreature::GetPlayerLogic() {
    if (NULL == m_pPlayerLogic) {
        m_pPlayerLogic = RT_NEW CPlayerLogic(*this);
    }
    return m_pPlayerLogic;
}

//
// [8/20/2009 tim.yang] 判断是否是在同一天 nowtime:当前时间  lasttime：上次使用时间 true:同一天   false:不在同一天
bool CRegionCreature::isSameDay(long nowtime, long lastTime) {
    RtLocateTime time1(nowtime);
    RtLocateTime time2(lastTime);

    if ((time1.iYear != time2.iYear) ||
        (time1.iYear == time2.iYear && time1.iMonth != time2.iMonth) ||
        (time1.iYear == time2.iYear && time1.iMonth == time2.iMonth && time1.iDay != time2.iDay)) {
        return false;
    } else {
        return true;
    }
}

////add by Tianh  分线moveto
//void CRegionCreature::LineMoveto(int _lineGroup,int _fRebirthX,int _fRebirthY)
//{
//
//	int   lineGroup = _lineGroup;
//	float fRebirthX = _fRebirthX;
//	float fRebirthY = _fRebirthY;
//
//	bool switchScene = true;
//	int iRx, iRy;
//	iRx = fRebirthX;
//	iRy = fRebirthY;
//
//	m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);
//
//	long pos[2];
//	pos[0] = fRebirthX;
//	pos[1] = fRebirthY;
//
//	if (lineGroup == 0)
//	{
//		MSG3("玩家[%s], 大地图回程[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX, fRebirthY);
//		if ( m_dwGroupID && m_dwLineID )
//		{
//			CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(m_dwGroupID, m_dwLineID, m_userInfo->m_userId, "DefMap", pos);
//		}else
//			switchScene = false;
//	}
//	else if (lineGroup > 0)
//	{
//		if ( m_dwGroupID && m_dwLineID )
//		{
//			switchScene = false;
//		}else
//			CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(lineGroup, m_userInfo->m_userId, pos);
//	}
//
//	if ( !switchScene )
//	{
//		CG_CmdPacket& packet = g_region->m_gws.BeginSend();
//		packet.WriteShort(r2g_move_to);
//		packet.WriteLong(m_userInfo->m_userId);
//		packet.WriteLong((long)iRx);
//		packet.WriteLong((long)iRy);
//		g_region->m_gws.EndSend();
//	}
//}
//end
// [8/20/2009 tim.yang] 双倍经验瓶使用限制
//true：还在同一天    false：不是同一天
bool CRegionCreature::IsUseCardMinBottleOverday() {
    // 计算时间
    long tmpTime = time(NULL);
    //tm now = *(localtime(&tmpTime));

    if (0 == m_lastuseCardMinuteBottleTime) {
        m_lastuseCardMinuteBottleTime = time(NULL);
    }
    long lastTime = m_lastuseCardMinuteBottleTime;
    //RtLocateTime last(lastTime);

    if (isSameDay(tmpTime, lastTime)) {
        return false;
    } else {
        return true;
    }
}

bool CRegionCreature::CanUseCardMinBottle() {
    if (!g_region)
        return false;
    if (m_useCardMinuteBottleNum < g_region->GetUseCardMinBottleMaxNum()) {
        return true;
    } else {
        rt2_sprintf(g_strStaticBuffer, R(MSG_USE_CARDMINBOTTLE_IS_MAX_NUM),
                    g_region->GetUseCardMinBottleMaxNum());
        SendSystemMessage(g_strStaticBuffer);
        return false;
    }
}

void CRegionCreature::AddUseCardMinBottleNum() {
    ++m_useCardMinuteBottleNum;
    m_lastuseCardMinuteBottleTime = time(NULL);
}

void CRegionCreature::InitCardMinBottleParam() {
    m_useCardMinuteBottleNum      = 0;
    m_lastuseCardMinuteBottleTime = 0;
}

//end

void CRegionCreature::AddHp(int add, int IsFullHpShow, bool Hit) {
    m_core.AddHp(add);

    if (add > 0) {
        if (IsFullHpShow) {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_display);
            g_sendCmd->WriteShort(ZHILIAO);
            g_sendCmd->WriteLong(GetObjectId());
            g_sendCmd->WriteLong(add);
            g_sendCmd->WriteByte(Hit);
            if (m_scene && 1 == IsFullHpShow) {
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
            } else {
                RecvCmd(g_sendCmd);
            }
        }
    }
}

void CRegionCreature::BackHome() {
    if (GetPlayerLogic()->IsInPShop()) {
        return;
    }
    // 副本内回城
    if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_FB) {
        rt2_sprintf(g_strStaticBuffer, R("副本内不能使用回程"));
        SendSystemMessage(g_strStaticBuffer);
        return;
    }

    //战场内回城
    //if(OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_WARFIELD)
    //{
    //	if (!m_userInfo) return;
    //	g_WarfieldMgr.RequestLeave(m_userInfo);
    //}

    float fRebirthX, fRebirthY;
    int   lineGroup = 0;
    switch (m_core.RelivePostion) {
        case 1: {
            lineGroup = 1;
            fRebirthX = RELIVEPOSTIONX1;
            fRebirthY = RELIVEPOSTIONY1;
        } break;
        case 2: {
            fRebirthX = RELIVEPOSTIONX2;
            fRebirthY = RELIVEPOSTIONY2;
        } break;
        case 3: {
            fRebirthX = RELIVEPOSTIONX3;
            fRebirthY = RELIVEPOSTIONY3;
        } break;
        case 4: {
            fRebirthX = RELIVEPOSTIONX4;
            fRebirthY = RELIVEPOSTIONY4;
        } break;
        case 5: {
            lineGroup = 1;
            fRebirthX = RELIVEPOSTIONX5;
            fRebirthY = RELIVEPOSTIONY5;
        } break;
        default: {
            m_core.RelivePostion = 1;
            lineGroup            = 1;
            fRebirthX            = RELIVEPOSTIONXOTHER;
            fRebirthY            = RELIVEPOSTIONYOTHER;
        } break;
    }

    bool switchScene = true;
    int  iRx, iRy;
    iRx = fRebirthX;
    iRy = fRebirthY;
    m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);
    iRx += rand() % 10 - 5;
    iRy += rand() % 10 - 5;

    long pos[2];
    pos[0] = fRebirthX + rand() % 200 - 100;
    pos[1] = fRebirthY + rand() % 200 - 100;

    if (lineGroup) {
        MSG3("玩家[%s], 分线复活[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX,
             fRebirthY);
        if (m_dwGroupID && m_dwLineID) {
            switchScene = false;
        } else
            CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(lineGroup, m_userInfo->m_userId,
                                                               pos);
    } else {
        MSG3("玩家[%s], 大地图复活[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX,
             fRebirthY);
        if (m_dwGroupID && m_dwLineID) {
            CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(m_dwGroupID, m_dwLineID,
                                                               m_userInfo->m_userId, "DefMap", pos);
        } else
            switchScene = false;
    }

    if (!switchScene) {
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_move_to);
        packet.WriteLong(m_userInfo->m_userId);
        packet.WriteLong((long)iRx);
        packet.WriteLong((long)iRy);
        g_region->m_gws.EndSend();
    }
}

void CRegionCreature::AddPlayerOnlineTime(long add)  //Tianh   用来计算在线累积时间 5小时清除一次
{
    int RewardCul = 0;

    //取得当前时间：年月日分秒
    time_t tmpTime    = time(NULL);
    tm     now        = *(localtime(&tmpTime));
    int    ilocalTome = now.tm_mday;

    m_core.PlayerOnlineTime += add;

    //每小时奖励一次修为，每天最多奖励5次
    if (m_core.PlayerOnlineTime >= 60 && CulAwardCount <= 5 && ilocalTome == localTime) {
        //每小时奖励的修为值   18/6
        RewardCul =
            6 * (20 + 5 * (m_core.GradeLev - 1) * (1 + 0.05 * (m_core.GradeLev - 1))) * 6 * 0.4;
        AddCulExp(RewardCul);

        ////发送系统消息
        //if (0 != RewardCul)
        //{
        //	rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_TIME_CUL), RewardCul);
        //	SendSystemMessage(g_strStaticBuffer);
        //}
        m_core.PlayerOnlineTime = 0;

        CulAwardCount++;
    } else if (m_core.PlayerOnlineTime < 0) {
        m_core.PlayerOnlineTime = 0;
    }

    if (CulAwardCount >= 5 || ilocalTome != localTime) {
        CulAwardCount           = 0;
        m_core.PlayerOnlineTime = 0;
        localTime               = now.tm_mday;
    }
}

void CRegionCreature::RecvCmd(CG_CmdPacket* cmd) {
    if (m_userInfo)
        return m_userInfo->RecvCmd(cmd);
}

// update data to client
void CRegionCreature::UpdateToClient(char type) {
    if (m_userInfo || m_master) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_update_creature);
        g_sendCmd->WriteLong(m_oId);
        g_sendCmd->WriteByte(type);
        Serialize(type, g_sendCmd);

        // LOG1("update full,size = %d\n",g_sendCmd->GetByteDataSize() +
        //	BitToByte(g_sendCmd->GetBitDataSize()));
        if (m_master) {
            m_master->RecvCmd(g_sendCmd);
        } else {
            RecvCmd(g_sendCmd);
        }
    }
}

// update delta data to client
void CRegionCreature::UpdateToClientDelta() {
    if (m_userInfo /* || m_master*/) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_update_cre_delta);
        g_sendCmd->WriteLong(m_oId);
        m_core.SerializeDelta(*g_sendCmd, m_oldCore);
        // LOG2("update_delta: %d,%d\n",g_sendCmd->GetByteDataSize(),g_sendCmd->GetBitDataSize());

        // suppose client was updated imm
        m_oldCore = m_core;
        /*
		if(m_master)
		{
			long temp;
			memcpy(&temp,g_sendCmd->GetBitData(),sizeof(long));
			LOG3("update delta,size = %d,bit = %d,data = %d\n",g_sendCmd->GetByteDataSize() +
				BitToByte(g_sendCmd->GetBitDataSize()),g_sendCmd->GetBitDataSize(),temp);
		}
		*/

        if (m_master)
            m_master->RecvCmd(g_sendCmd);
        else
            RecvCmd(g_sendCmd);
    }
}

void CRegionCreature::SendProblemToClient(long lProblemID) {
    if (lProblemID <= 0 || (!IsUser()))
        return;
    ProblemData* pProblem = g_TableProblem.FindProblemByID(lProblemID);
    if (pProblem) {
        std::string sHtmlText("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; "
                              "charset=gb2312\"/></head><body>");
        sHtmlText += pProblem->sTitle;
        sHtmlText += "<p>&nbsp;</p>";
        sHtmlText += pProblem->sDescription;
        sHtmlText += "<p>&nbsp;</p>";
        sHtmlText += "<p>&nbsp;</p>";
        for (int ii = 0; ii < ProblemData::OPTION_COUNT; ii++) {
            if (pProblem->sOption[ii].empty())
                break;
            sHtmlText += pProblem->sOption[ii];
        }
        sHtmlText += "<p>&nbsp;</p>";
        sHtmlText += "</body></html>";

        g_region->ShowHtmlDialog(this, this, sHtmlText.c_str());
    }
}

void CRegionCreature::Serialize(char type, CG_CmdPacket* cmd) {
    m_core.Serialize(type, cmd);
    switch (type) {
        case NET_UPDATE_ALL:
            if (m_pItemContainer)
                m_pItemContainer->Serialize(type, cmd);
            // m_task.SerializeTask(cmd);
            // m_task.SerializeFbTask(cmd);
            break;

        case NET_CRE_UPDATE_MODEL:
            break;

        case NET_CRE_UPDATE_ATTR_SIMPLE:
            break;

        case NET_CRE_UPDATE_ATTR_BASE:
            break;

        case NET_CRE_UPDATE_ATTR_ALL:
            break;

        case NET_CRE_UPDATE_TASK:
            m_task.SerializeTask(cmd);
            break;

        case NET_CRE_UPDATE_FB_TASK:
            m_task.SerializeFbTask(cmd);
            break;
    }
}

bool CRegionCreature::WriteSnapshot(CG_CmdPacket* cmd, CRegionCreature* target) {
    long flag = 0, newFlag = 0;
    cmd->WriteLong(m_oId);
    cmd->WriteByte(m_type);

    long IsFlagNpc = 0;
    if (m_npcInfo && g_RegionCastleWarMgr.IsFlagNpc(m_npcInfo->Id)) {
        flag |= SYNC_FLAG_WAR;
    }

    if (m_npcInfo && m_npcInfo->Boss != 0) {
        flag |= SYNC_FLAG_BOSS;
    }

    if (m_npcInfo && (m_npcInfo->Type == 6 || m_npcInfo->Type == 7 || m_npcInfo->Type == 8)) {
        flag |= SYNC_FLAG_TRIGGER;
    }
    /*
	// kick cheat
	if(target->m_cheatKick > rtGetMilliseconds())
	{
		if(GetProb(0.5))
			cmd->WriteLong(GetRand(100000,0));
	}
	*/

    BYTE cNameColor = 0;

    if (IsUser()) {
        flag |= SYNC_FLAG_USER;
        if (m_core.Metier == METIER_TAOIST)
            flag |= SYNC_FLAG_METIER_TAOIST;
    } else if (IsUserPet()) {
        flag |= SYNC_FLAG_PET;
    } else if (IsCallNpc()) {
        flag |= SYNC_FLAG_CALL;
    } else if (IsSpirit()) {
        flag |= SYNC_FLAG_SPIRIT;
    }

    flag |= SYNC_FLAG_POSITION;
    flag |= SYNC_FLAG_DIRECTION;

    if (m_core.ComplexModel)
        flag |= SYNC_FLAG_COMPLEX_MODEL;

    // write info base on flag
    cmd->WriteLong(flag);

    // PZH
    long cDead = this->GetPlayerLogic()->GetLiveType();

    //if (m_bDead)
    //{
    //	//cmd->WriteByte(1);
    //	cDead = 1;
    //}else{
    //	//cmd->WriteByte(0);
    //	cDead = 0;
    //}

    ////<add by Tianh> 这里处理超级变身

    //cmd->WriteByte(m_UltimateChange);

    cmd->SerializeBit(cDead, 2);
    long cMakeTemp = 0;
    if (NULL != m_pTeam) {
        if (!m_pTeam->mData.Empty()) {
            if (m_pTeam->mData.GetCaptain() == m_oId) {
                cMakeTemp = 2;
            } else {
                cMakeTemp = 1;
            }
        }
    }
    cmd->SerializeBit(cMakeTemp, 2);
    cmd->SerializeBit(m_lIsCombat, 1);

    if (flag & SYNC_FLAG_TRIGGER) {
        cmd->SerializeBit(triggerIsClose, 1);
        cmd->WriteLong(triggerID);
    }

    //
    //
    //-----------add start by tony 05.06.29------------------//
    if (flag & SYNC_FLAG_WAR) {
        cmd->WriteString((char*)m_unionName.c_str());
    }
    //-----------add end   by tony 05.06.29------------------//

    if (flag & SYNC_FLAG_BOSS) {
        cmd->WriteLong(m_core.GetMaxHp());
        cmd->WriteLong(m_core.GetHp());
        cmd->WriteLong(m_core.Lev);
    } else {
        char cHPRate      = (((float)m_core.GetHp()) / m_core.GetMaxHp()) * 100;
        char cMPRate      = (((float)m_core.GetMp()) / m_core.GetMaxMp()) * 100;
        char cActorID     = m_core.actorID;
        long lLev         = m_core.Lev;
        long lHeadImageID = m_core.HeadImageID;

        if (m_core.Metier == METIER_TAOIST) {
            if (m_core.GetMp() <= 0) {
                m_core.SetShieldHp(0);
                m_Skill.ClearEffByClass(18);  //去掉魔法盾效果(圣巫))
            }
        }

        if (cHPRate < 0)
            cHPRate = 0;
        else if (cHPRate > 100)
            cHPRate = 100;

        if (cMPRate < 0)
            cMPRate = 0;
        else if (cMPRate > 100)
            cMPRate = 100;

        cmd->WriteByte(cHPRate);
        cmd->WriteByte(cMPRate);
        cmd->WriteByte(cActorID);
        cmd->WriteLong(lLev);
        cmd->WriteLong(lHeadImageID);
    }

    if (flag & SYNC_FLAG_POSITION) {
        short dx = m_pos[0] - target->m_pos[0];
        short dy = m_pos[1] - target->m_pos[1];
        cmd->SerializeShortEx(dx);
        cmd->SerializeShortEx(dy);
        if (m_ai) {
            cmd->WriteByte(m_ai->m_bTrace ? 1 : 0);
        } else {
            cmd->WriteByte(0);
        }
    }
    if (flag & SYNC_FLAG_DIRECTION) {
        cmd->WriteByte(m_dir);
    }
    if (flag & SYNC_FLAG_COMPLEX_MODEL) {
        cmd->WriteByte(m_modelRef);
    } else {
        cmd->WriteShort(m_core.ModelId);
    }
    if (!(flag & SYNC_FLAG_USER)) {
        cmd->WriteShort(m_npcId);
    } else if (flag & SYNC_FLAG_METIER_TAOIST) {
        long lShapeshift = m_ChangeModelID == 0 ? 0 : 1;
        cmd->SerializeBit(lShapeshift, 1);
    }

    /*Tianh 09.12.25*/
    char cFaction = -1;
    if (m_core.Faction == FACTION_ZHOU)
        cFaction = 0;
    else if (m_core.Faction == FACTION_SHANG)
        cFaction = 1;
    else if (m_core.Faction == FACTION_NEUTRAL)
        cFaction = 3;
    cmd->WriteByte(cFaction);

    switch (GetPKState()) {
        case PK_STATE_YELLOW_NAME:
            cNameColor |= PK_STATE_YELLOW_NAME;
            break;
        case PK_STATE_RED_NAME:
            cNameColor |= PK_STATE_RED_NAME;
            break;
    }
    if (m_bStatic) {
        cmd->WriteByte(MOVE_STATE_STATIC | cNameColor);
    } else if (IsRidePet()) {
        cmd->WriteByte(MOVE_STATE_WALK | MOVE_STATE_RIDE | cNameColor);
        cmd->WriteLong(m_pet->m_oId);
    }

    else {
        cmd->WriteByte(MOVE_STATE_WALK | cNameColor);
    }

    this->m_Skill.NetWriteEffect(cmd);
    return true;
}

void CRegionCreature::WgChecked(int iBaseTime, int iRandTime, char* szCheckInfo) {
    if (m_master) {
        m_master->WgChecked(iBaseTime, iRandTime);
        return;
    }
    if (m_wgChecked == 0) {
        ERR("WgChecked:,UserName: %s, IP: %s, CheckInfo: %s\n", m_userInfo->m_username.c_str(),
            m_userInfo->m_userIpAddress.c_str(), szCheckInfo);

        m_wgChecked = 1;
        if (g_region)
            g_region->m_iWgCheckedTotal++;
        m_wgCheckedTime = (long)time(NULL) + iBaseTime +
                          rand() % iRandTime;  // 检查到外挂，加上120到600秒的延时退出
#ifdef WIN32
        LOG1("CRegionCreature::WgChecked %d\n", m_wgCheckedTime - (long)time(NULL));
#endif
    }
}

void CRegionCreature::PossiblyUsingWG(int iCnt, int iBaseTime, int iRandTime) {
    if (m_master) {
        m_master->PossiblyUsingWG(iCnt, iBaseTime, iRandTime);
        return;
    }
    long lNow = (long)time(NULL);
    // 客户端回答问题是50秒锁定，这边设置成70秒是最少20秒没有问题的时间
    //     如果 m_wgMaybeChecked 为0，就是还没有开始检查可能的外挂，这时候一定要到上次检查后的70秒才能开始检查
    //     否则，就可以检查
    if (m_wgMaybeChecked != 0 || (lNow > m_wgMaybeCheckedTime + 70 - iBaseTime)) {
        if (m_wgMaybeChecked < 5) {
            m_wgMaybeChecked += iCnt;
            m_wgMaybeCheckedTime = lNow + iBaseTime + rand() % iRandTime;
#ifdef WIN32
            LOG2("CRegionCreature::PossiblyUsingWG %d %d\n", m_wgMaybeChecked,
                 m_wgMaybeCheckedTime - lNow);
#endif
        } else {
        }
    }

    //if (m_userInfo)
    //{
    //	NOTICE("PossiblyUsingWG user name:%s user ip:%s\n",
    //		m_userInfo->m_username.c_str(), m_userInfo->m_userIpAddress.c_str());
    //}
    //else if (m_npcInfo)
    //{
    //	NOTICE("PossiblyUsingWG npc name:%s \n",
    //		m_npcInfo->Name.c_str());
    //}
    //else
    //{
    //	NOTICE("PossiblyUsingWG unknow creature regionID:%d \n",GetObjectId());
    //}
}

void CRegionCreature::SendWgCheckedToClient() {
    if (m_master) {
        m_master->SendWgCheckedToClient();
        return;
    }
    //if (m_userInfo)
    //	NOTICE("find WG kick user %s \n",m_userInfo->m_username);
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_wg_checked);
    RecvCmd(g_sendCmd);
}

bool CRegionCreature::CanMove() {
    if (!m_core.IsAbiMove())
        return false;
    if (IsMonster()) {
        if (m_combatBusy && rtGetMilliseconds() < m_combatBusy)
            return false;
    }
    int x = m_stateList.size();
    if (/*PZH*/ /*m_bDead*/ !this->GetPlayerLogic()->IsLive())
        return false;
    if (!m_stateList.empty()) {
        if (FindStateCannotMove())
            return false;
    }

    if (!State_CanMove())
        return false;

    if (m_master) {
        if (!m_master->m_core.IsAbiMove())
            return false;
        if (/*PZH*/ /*m_master->m_bDead*/ !m_master->GetPlayerLogic()->IsLive())
            return false;
        if (!m_master->State_CanMove())
            return false;
    }
    return true;
}

long CRegionCreature::GetAttackBusy() {
    if (m_core.AttSpeed.GetValue() == 0)
        return 1000;

    return (1.0f / m_core.AttSpeed.GetValue()) * 1000;
}

void CRegionCreature::AddInvincibility() {
    m_core.AddMove();
    m_core.AddUseSkill();
    m_core.AddNormalAttack();
    //m_core.AddAbiBeAttack(1); // 因为我们不希望锁定的时候无敌，就暂时去掉这个函数
}

void CRegionCreature::RemoveInvincibility() {
    m_core.SubMove();
    m_core.SubUseSkill();
    m_core.SubNormalAttack();
    //m_core.RemoveAbiBeAttack(1); // 因为我们不希望锁定的时候无敌，就暂时去掉这个函数
}

bool CRegionCreature::HaveEffectByID(unsigned short wID) {
    CActorSkill* pActorSkill = m_Skill.FindActorSkill(this->m_oId);
    if (!pActorSkill)
        return false;

    if (pActorSkill->FindEffect(wID))
        return true;
    return false;
}

bool CRegionCreature::HaveEffectByClass(unsigned char cID) {
    CActorSkill* pActorSkill = m_Skill.FindActorSkill(this->m_oId);
    if (!pActorSkill)
        return false;

    if (pActorSkill->FindEffectByClassID(cID))
        return true;
    return false;
}

CRegionCreature* CRegionCreature::GetEntity() {
    if (IsRidePet())
        return m_pet;
    return this;
}

CRegionCreature* CRegionCreature::GetControl() {
    if (IsRided())
        return m_master;
    return this;
}

bool CRegionCreature::IsInSafeArea() {
    // 根据时间和地点判断是否在安全区
    return false;
    // return m_eCurPosPKAttr == PKA_NONE;
}

bool CRegionCreature::CheckUseExpItem() {
    size_t listsize = 10;
    switch (m_core.Metier) {
        case METIER_WARRIOR:
            if (g_region->m_HeroList.m_warriorList.size() <= listsize) {
                listsize = g_region->m_HeroList.m_warriorList.size();
            }
            for (size_t i = 0; i < listsize; ++i) {
                if (m_core.Name == g_region->m_HeroList.m_warriorList[i].name) {
                    return false;
                }
            }
            break;
        case METIER_HUNTER:
            if (g_region->m_HeroList.m_warriorList.size() <= listsize) {
                listsize = g_region->m_HeroList.m_warriorList.size();
            }
            for (size_t i = 0; i < listsize; ++i) {
                if (m_core.Name == g_region->m_HeroList.m_hunterList[i].name) {
                    return false;
                }
            }
            break;
        case METIER_WIZARD:
            if (g_region->m_HeroList.m_warriorList.size() <= listsize) {
                listsize = g_region->m_HeroList.m_warriorList.size();
            }
            for (size_t i = 0; i < listsize; ++i) {
                if (m_core.Name == g_region->m_HeroList.m_wizardList[i].name) {
                    return false;
                }
            }
            break;
        case METIER_TAOIST:
            if (g_region->m_HeroList.m_warriorList.size() <= listsize) {
                listsize = g_region->m_HeroList.m_warriorList.size();
            }
            for (size_t i = 0; i < listsize; ++i) {
                if (m_core.Name == g_region->m_HeroList.m_taolistList[i].name) {
                    return false;
                }
            }
            break;
    }
    return true;
}

bool CRegionCreature::CheckAttackSpeed(long interval, bool test) {
    if (m_combatBusy && rtGetMilliseconds() < m_combatBusy) {
        //CRegionCreature* pRecv = this;
        //if(m_master)
        //{
        //	pRecv = m_master;
        //}

        //DWORD inter = m_combatBusy - rtGetMilliseconds();
        //pRecv->SendSystemMessage(rtFormatNumber(inter));
        //LOG4("@@@@@@ %d %d %d %d\n", m_combatBusy, rtGetMilliseconds(), m_combatBusy - rtGetMilliseconds(), interval);
        if (!m_atChk.Valid(m_combatBusy - rtGetMilliseconds())) {
            // LOG1("[%s] attack speed error,ignore\n",m_core.Name.c_str());
            // LOG4("###### %d %d %d %d\n", m_combatBusy, rtGetMilliseconds(), m_combatBusy - rtGetMilliseconds(), interval);
            this->PossiblyUsingWG();
            return false;

            //RtCoreLog().Debug("攻击速度出错 interval = %d m_combatBusy=%d",interval,m_combatBusy);
        }
    }

    if (!test)
        m_combatBusy = rtGetMilliseconds() + interval;  //Tianh 临时修改
    return true;
}

bool CRegionCreature::CheckCommonAttackSpeed(long interval, bool test) {
    DWORD dwNowTime = rtGetMilliseconds();
    if (m_CcombatBusy && dwNowTime < m_CcombatBusy) {
        //NOTICE("AttackSpeed check error: interval %d ms\n",
        //	m_CcombatBusy - dwNowTime);
        //NOTICE("m_atChk.cur:%d, m_atChk.cur:%d \n",
        //	m_atChk.cur,m_atChk.max);

        if (!m_atChk.Valid(m_CcombatBusy - dwNowTime)) {
            this->PossiblyUsingWG();
            return false;
        }
    }

    //if(!test)
    m_CcombatBusy = dwNowTime + interval;  //Tianh 临时修改

    return true;
}

// check if A can attack B
bool CRegionCreature::TryNormalAttack(CRegionCreature* target, int attRange) {
    if (!m_core.IsAbiNormalAttack())
        return false;

    // 自己相关属性判断，攻击速度等

    //Tianh 这里技能时间防外挂检查  暂时去掉
    if (!CheckCommonAttackSpeed(m_core.GetAttackInterval(), false)) {
        //LOG("normal attack speed error\n");
        return false;
    }
    //----------add start by tony 06.05.16---------------------------------//
    //-换装备1秒内不能攻击
    //-这是为了改正骑兽攻击时换装备攻击速度过快问题，导致问题的真正原因
    //-不在这里
    DWORD last = 0;
    if (IsUserPet() || IsCallNpc()) {
        last = m_master->m_lastEquipItemTime;
    } else if (IsUser()) {
        last = m_lastEquipItemTime;
    }
    DWORD now = time(NULL);
    if (now - last < 1) {
        return false;
    }
    //----------add end   by tony 06.05.16------------------//
    //if (IsChengBody()) //Tianh  在变身状态下距离是60
    //{
    //	if ((60 + target->m_model->ModelRadius + 60) < Distance(target->m_pos))
    //	{
    //		// too far away
    //		return false;
    //	}
    //}
    if ((m_core.AttackRange.GetValue() + target->m_model->ModelRadius + 60) <
        Distance(target->m_pos)) {
        // too far away
        return false;
    }

    // 对方属性判断
    return true;
}

bool CRegionCreature::TryStateHit(int param, CRegionCreature* target, SSkill* pSkillAttr) {
    float rate;
    int   delta = target->m_core.Lev - m_core.Lev;
    rate        = param + m_core.GetAbiStateHit() - target->m_core.GetAbiResist();
    //if((pSkillAttr->wID >= 373&&pSkillAttr->wID <= 377)||(pSkillAttr->wID >= 388&&pSkillAttr->wID <= 392))
    //{
    //	rate -= target->m_core.ReFaint.GetValue()/100; //抗晕击修正
    //}
    // 	if(delta > 5)
    // 	{
    // 		return false;
    // 	}
    // 	else
    // 	{
    // 		while(delta-- > 0)
    // 			rate *= 0.75;
    // 	}

    //2009.4.19 by Cheney, 降低等级致晕的衰减。记得要把Boss等级统一调整为150级。

    //if(IsMonster() && (target->IsUser() || target->IsUserPet()) && delta > 5)
    //{
    //	return false;
    //}

    //if(delta > 30)
    //{
    //	return false;
    //}
    //else
    //{
    //	while(delta-- >0)
    //		rate *= 0.97;
    //}

    if (rate < 0)
        return false;

    return GetProb(rate / 100.0f);
}

bool CRegionCreature::IsSkillCritical(int param, CRegionCreature* target) {
    // if(!IsUser()) return false;
    float rate =
        m_core.GetSkillCriRate(target->m_core.Lev, target->m_core.ReSkillCritical.GetValue()) +
        param / 100.0;
    if ((IsUser() || IsUserPet() || IsCallNpc()) &&
        (target->IsUser() || target->IsUserPet() || target->IsCallNpc()))
        rate /= 2;

    return GetProb(rate);
}

void CRegionCreature::GetSkillCriDamage(int& phyDmg, int eleDmg[MAX_ELEMENT_ATTR],
                                        CRegionCreature* target) {
    phyDmg = 0;
    for (int i = 0; i < MAX_ELEMENT_ATTR; i++)
        eleDmg[i] = 0;

    if (IsUser()) {
        SItemID item = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
        if (ItemID_IsValid(item)) {
            SItemBase* pClass = g_region->m_pItemManager->GetItem(item.type);
            if (!pClass)
                return;

            //if(ItemID_IsValid(item) && ItemIsTrump(item.type))
            //{
            //	MW_GetEleValue(item,eleDmg);
            //	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
            //	{
            //		eleDmg[i] /= 2;
            //		eleDmg[i] *= (1+m_core.SkillCriDamageRate/100.0f);
            //	}
            //}
            //else
            //{
            // 				float damage,tmp;
            // 				ItemUpdate_GetExtraAddtion(item,g_region->m_pItemManager, damage,tmp,tmp,tmp,tmp);
            // 				//phyDmg  = ((pClass->nDamageMax+pClass->nDamageMin)/2+damage)*2;
            // 				phyDmg = ((pClass->nDamageMax+pClass->nDamageMin)/2+damage)*2;
            // 				phyDmg *= (1+m_core.SkillCriDamageRate/100.0f);

            //2009.4.20 Cheney, 提高属性影响的暴击
            phyDmg = (m_core.GetDamageMax() + m_core.GetDamageMin()) / 2;
            phyDmg *= (1 + m_core.SkillCriDamageRate / 100.f);

            if (IsUser()) {
                for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                    eleDmg[i] = m_core.ElementDamage[i].GetValue();
                }
            } else {
                if (m_npcInfo) {
                    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                        eleDmg[i] =
                            m_npcInfo->ElementDamage[i] + m_core.ElementDamage[i].GetValue();
                    }
                } else {
                    LOG("m_npcInfo is Null , GetSkillCriDamage function");
                }
            }

            /*Tianh  元素攻击的浮动值*/
            for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                if (eleDmg[i] >= 5)
                    eleDmg[i] = GetRand(eleDmg[i] * 1.1, eleDmg[i] * 0.9);
            }

            for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                eleDmg[i] *= (1 + m_core.SkillCriDamageRate / 100.f);
            }

            //}
        }
    } else {
        GetDamage(phyDmg, eleDmg);
        phyDmg *= 2;
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++)
            eleDmg[i] *= 2;
    }
    target->ApplyDamage(this, phyDmg, eleDmg, true);
}

// check if A can hit B
bool CRegionCreature::TryHit(CRegionCreature* target) {
    // pk self def check
    // check pk state,check user pet also...
    //-----------------------chg start by tony 05.05.20--------------------------//
    if ((target->IsUser() || target->IsUserPet() || target->IsCallNpc()) &&
        (IsUser() || IsUserPet() || IsCallNpc()) &&
        (!g_RegionCastleWarMgr.IsCastleWarTime(target) ||
         !g_RegionCastleWarMgr.IsCastleWarTime(this)))
    //-----------------------chg end   by tony 05.05.20--------------------------//
    {
        CRegionCreature *src, *tar;
        src = this;
        if (IsUserPet() || IsCallNpc())
            src = m_master;
        tar = target;
        if (target->IsUserPet() || target->IsCallNpc())
            tar = target->m_master;
        if (tar->GetPKState() == PK_STATE_NORMAL && (!src->IsSelfDef(tar)))
            tar->AddSelfDef(src);
    }

    if (!target->m_core.IsAbiDodge())
        return true;

    int myAtt = 0;
    if (IsRided()) {
        myAtt = m_core.GetAttack() * 2;
    } else {
        myAtt = m_core.GetAttack();
    }
    int tarDog = target->m_core.GetDodge();
    /*int attachAtt = m_core.GetAbiStateHit();*/
    float rate = 0.f;
    /*Tianh 战斗公式  普通攻击命中*/
    if (IsUser() && target->IsMonster()) {
        if (target->IsUserPet() || target->IsCallNpc()) {
            rate = (((float)myAtt + 500) / (tarDog + 1200));
        } else if (target->IsUser()) {
            rate = (((float)myAtt + 500) / (tarDog + 1200));
        } else {
            rate = (((float)myAtt + 500) / (tarDog + 1200)) + 0.50;
        }
    } else if (IsUserPet() && target->IsMonster()) {
        if (target->IsUserPet() || target->IsCallNpc()) {
            rate = (((float)myAtt + 500) / (tarDog + 1200));
        } else if (target->IsUser()) {
            rate = (((float)myAtt + 500) / (tarDog + 1200));
        } else {
            rate = (((float)myAtt + 500) / (tarDog + 1200)) + 0.50;
        }
    } else  //怪打任何都+0.5
    {
        rate = (((float)myAtt + 500) / (tarDog + 1200)) + 0.50;
    }

    if (0 == rate) {
        return true;
    }

    if (rate >= 1)
        rate = 0.9999;
    if (rate <= 0.01)
        rate = 0.0001;

    return GetProb(rate);
}

// 判断是否可以攻击敌人
//     enemy为NULL，表示检查是否可以攻击地面
//
//  如果处在某种状态就不能攻击，如注法宝等
//  如果敌人是自己的宠物，就不能攻击
//  服务NPC不能攻击
//  不能攻击自己阵营的守卫(城战期间守城npc除外)
//  如果是玩家对玩家
//      不能PK新手
//      新手不能PK
//  如果自己再交易或者摆摊就不能攻击
//  人可以攻击任何怪，怪也可以攻击任何人 [这里还没有进行安全区判断，所以即使在安全区也可以攻击，如幻境]
//  如果自己在绝对安全区，就不允许攻击
//  如果对方在绝对安全区，就不允许攻击
//
bool CRegionCreature::CanAttack(CRegionCreature* enemy, SSkill* pSkill) {

    // 如果处在某种状态就不能攻击，如注法宝等
    if (!m_stateList.empty())
        return false;

    if (IsUser() && IsInProtect() && enemy && enemy->IsUser()) {
        SendSystemMessage(R(MSG_SELF_PROTECTED));
        return false;
    } else if (enemy && enemy->IsInProtect() && IsUser()) {
        SendSystemMessage(R(MSG_ENEMY_PROTECTED));
        return false;
    }

    bool             bSelfIsUserOrPet = (IsUser() || IsUserPet() || IsCallNpc());
    CRegionCreature* pSelf            = IsUser() ? this : m_master;

    EPKAttr ePKAttr;
    if (enemy) {
        //LeiJun 如果敌人的所属用户是自己就不能攻击
        if (enemy->m_master == this)
            return false;

        // 如果是检查是否可以攻击敌人
        bool bEnemyIsUserOrPet  = (enemy->IsUser() || enemy->IsUserPet() || enemy->IsCallNpc());
        CRegionCreature* pEnemy = enemy->IsUser() ? enemy : enemy->m_master;

        // 如果是自己的宠物，就不能攻击
        if (m_pet == enemy) {
            return false;
        }

        // 服务NPC不能攻击
        if (OB_TYPE(enemy) == OBJECT_TYPE_NPC_SERVICE) {
            return false;
        }

        // 如果PK为0 并且 处于不能攻击状态的话 就不能攻击
        if (bEnemyIsUserOrPet && pEnemy->m_core.Pk == 0 && pEnemy->m_core.IsAbiBeAttack() != 0) {
            return false;
        }

        //-----------------------add start by tony 05.06.07--------------------------//
        if (enemy->m_npcInfo != NULL && m_mc &&
            !m_mc->m_bstartTask)  //tim.yang  MC  在没有进行怪物攻城的时候才允许城战
        {
            // 如果是城战相关的NPC，就让城战系统进行判断
            if (g_RegionCastleWarMgr.IsFlagNpc(enemy->m_npcInfo->Id) ||
                g_RegionCastleWarMgr.IsGuardAnimal(enemy->m_npcInfo->Id)) {
                return g_RegionCastleWarMgr.CanAttack(enemy->m_npcInfo->Id, this, enemy->m_oId);
            }
        }
        //-----------------------add end   by tony 05.06.07--------------------------//

        //-----------------------add start by tony 05.05.20--------------------------//
        //不能攻击自己阵营的守卫(城战期间守城npc除外)
        if (enemy->IsGuarder()) {
            if (bSelfIsUserOrPet && (pSelf->m_core.Faction == enemy->m_core.Faction) &&
                (enemy->m_npcInfo != NULL) &&
                !g_RegionCastleWarMgr.IsCastleWarTime(enemy->m_npcInfo->Home))
                return false;
        }
        //-----------------------add end   by tony 05.05.20--------------------------//

        // 如果自己再交易或者摆摊就不能攻击
        if (m_pTrade || m_pPShop) {
            // MSG("CRegionCreature::CanAttack 自己在交易或者摆摊，不能攻击\n");
            return false;
        }

        if (IsUser() && enemy->IsUser()) {
            if (pSkill) {
                if (pSkill->IsAddHpSkill) {
                    return true;
                }
            }
            if (!pSkill || pSkill->bABad != 0)
                if (m_pTeam->mData.Size() && enemy->m_pTeam->mData.Size()) {
                    if (m_pTeam->mData.GetCaptain() == enemy->m_pTeam->mData.GetCaptain())
                        return false;
                }
        }

        //tim.yang MC
        if (IsUser() && 0 == m_core.Faction && 723 == enemy->m_npcId) {
            return false;
        }
        if (IsUser() && 1 == m_core.Faction && 722 == enemy->m_npcId) {
            return false;
        }
        if (IsMonster() && 0 == m_core.Faction && 723 == enemy->m_npcId) {
            return true;
        }
        if (IsMonster() && 1 == m_core.Faction && 722 == enemy->m_npcId) {
            return true;
        }
        //end
        // 人可以攻击任何怪，怪也可以攻击任何人
        if (bSelfIsUserOrPet != bEnemyIsUserOrPet) {
            if (!bEnemyIsUserOrPet && enemy->m_npcInfo)  // 如果敌人是怪
            {
                if (enemy->m_npcInfo->Type == 4 || enemy->m_npcInfo->Type == 5)  // 药材或者矿物
                {
                    if (pSkill == NULL) {
                        return false;  // 药材或者矿物是不能普攻的
                    } else {
                        if ((pSkill->dwRTarget & SKILL_TARGET_HERB) &&
                            (enemy->m_npcInfo->Type == 4))  // 药材
                        {
                            if (pSkill->iLevel < enemy->m_npcInfo->Level) {
                                return false;  // 等级不够
                            }
                        } else if ((pSkill->dwRTarget & SKILL_TARGET_MINE) &&
                                   (enemy->m_npcInfo->Type == 5))  // 矿物
                        {
                            if (pSkill->iLevel < enemy->m_npcInfo->Level) {
                                return false;  // 等级不够
                            }
                        } else {
                            return false;  // 其他技能不能攻击 药材 和 矿物
                        }
                    }
                }
                if (enemy->m_ai && CRegionAI::STATE_RETURN == enemy->m_ai->m_state) {
                    return false;
                }
                //玩家不得攻击旅行NPC
                if (enemy->m_ai && enemy->m_ai->GetType() == CRegionAI::TYPE_TOUR)
                    return false;
            }
            return true;
        }

        // 下面的判断说明或者都是怪，或者都是人

        //如果都是怪物
        if (!bEnemyIsUserOrPet && !bSelfIsUserOrPet) {
            if (m_ai && enemy->m_ai) {
                CRegionAI::Type eSelfAIType  = m_ai->GetType();
                CRegionAI::Type eEnemyAIType = enemy->m_ai->GetType();
                if (eSelfAIType == CRegionAI::TYPE_MONSTER && eEnemyAIType == CRegionAI::TYPE_TOUR)
                    return true;
                if (eSelfAIType == CRegionAI::TYPE_TOUR && eEnemyAIType == CRegionAI::TYPE_MONSTER)
                    return true;
            }
        }

        // 如果是玩家对玩家
        if (bEnemyIsUserOrPet && bSelfIsUserOrPet) {
            bool prevent = true;
            if (pSelf->m_core.Faction == FACTION_ZHOU) {
                if (g_RegionCastleWarMgr.IsInTimeZone("XiQi") &&
                    g_RegionCastleWarMgr.InCastleWarArea(this))
                    prevent = false;
            } else if (pSelf->m_core.Faction == FACTION_SHANG) {
                if (g_RegionCastleWarMgr.IsInTimeZone("ZhaoGe") &&
                    g_RegionCastleWarMgr.InCastleWarArea(this))
                    prevent = false;
            }

            if (prevent) {
                // 不能PK新手
                //if(bEnemyIsUserOrPet && pEnemy->m_core.Lev <= 10)
                //            {
                //	SendSystemMessage(R(MSG_CANT_PK_NEWBIE));
                //	return false;
                //}
                //            // 新手不能PK
                //if(pSelf->m_core.Lev <= 10)
                //            {
                //	SendSystemMessage(R(MSG_NEWBIE_CANT_PK));
                //	return false;
                //}
            }
        }

        // 如果自己在绝对安全区，就不允许攻击
        ePKAttr = m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //------------------chg start by tony 05.06.09-----------------------------------------//
            if (g_RegionCastleWarMgr.IsCastleWarTime(this))  // 城战
            //------------------chg end   by tony 05.06.09-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            // MSG("CRegionCreature::CanAttack 自己在安全区，不能攻击\n");
            return false;
        }

        // 如果对方在绝对安全区，就不允许攻击
        ePKAttr = enemy->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //------------------chg start by tony 05.06.09-----------------------------------------//
            if (g_RegionCastleWarMgr.IsCastleWarTime(this))  // 城战
            //------------------chg end   by tony 05.06.09-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            // MSG("CRegionCreature::CanAttack 对方在安全区，不能攻击\n");
            return false;
        }
    } else if (pSkill->dwRTarget & SKILL_TARGET_POSITION) {
        // 如果自己再交易或者摆摊就不能攻击
        if (m_pTrade || m_pPShop) {
            // MSG("CRegionCreature::CanAttack 自己在交易或者摆摊，不能攻击\n");
            return false;
        }
        return true;
    } else  // if (enemy) else ...
    {
        // 自己在安全区，不能攻击
        ePKAttr = m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //------------------chg start by tony 05.06.09-----------------------------------------//
            if (g_RegionCastleWarMgr.IsCastleWarTime(this))  // 城战
            //------------------chg end   by tony 05.06.09-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST) {
            // MSG("CRegionCreature::CanAttack 自己在安全区，不能攻击\n");
            return false;
        }
    }  // if (enemy) else endif
    return true;
}

void CRegionCreature::DoAttack(long enemyId, long seed) {
    int phyDmg = 0;
    int eleDmg[MAX_ELEMENT_ATTR];

    CRegionCreature* enemy = m_scene->FindCreature(enemyId);
    if (!enemy)
        return;

    if (!CanAttack(enemy, NULL))
        return;

    DoAttack(enemy, seed);
}

void CRegionCreature::DoAttack(CRegionCreature* enemy, long seed) {
    int  phyDmg                   = 0;
    int  eleDmg[MAX_ELEMENT_ATTR] = {0, 0, 0};
    char eleSpec                  = 0;
    if (!enemy)
        return;

    // let pet know event
    if (m_pet && m_pet->m_ai) {
        CRegionPetAI* petAI = (CRegionPetAI*)m_pet->m_ai;
        if (petAI &&
            (petAI->m_eAS == CRegionPetAI::AS_ATTACK || petAI->m_eAS == CRegionPetAI::AS_SYNERGY)) {
            petAI->OnAttack(enemy);
        }
    }

    //Tianh   召唤的作战NPC  这里只有USER才可以有
    if (CallNpc.size() != 0) {
        std::vector<CRegionCreature*>::iterator vit = CallNpc.begin();
        for (; vit != CallNpc.end(); vit++) {
            if ((*vit)->m_ai) {
                CRegionCallNpc* CallNpcAI = NULL;
                CallNpcAI                 = (CRegionCallNpc*)(*vit)->m_ai;
                if (CallNpcAI && (CallNpcAI->m_eAS == CRegionCallNpc::AS_ATTACK ||
                                  CallNpcAI->m_eAS == CRegionCallNpc::AS_SYNERGY)) {
                    CallNpcAI->OnAttack(enemy);
                }
            }
        }
    }

    if (m_EffectProbability) {
        if (GetProb(m_EffectProbability / 100)) {
            if (&(enemy->m_Skill)) {
                (&(enemy->m_Skill))->SetbRun(true);
                (&(enemy->m_Skill))->AddEffect(m_PTEffectID, 0, true);
                /*(&(enemy->m_Skill))->SetbRun(false);*/
            }
        }
    }

    if (m_ScopeoDinaryInjury)  //如果是普攻群体伤害
    {
        int  iCnt = 0;
        long pPos[3];
        pPos[0] = enemy->m_pos[0];
        pPos[1] = enemy->m_pos[1];
        pPos[2] = enemy->m_pos[2];

        CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
        bool bCondition = (!((enemy->IsUser()) || (enemy->IsUserPet()) || (enemy->IsCallNpc())));
        iCnt =
            enemy->m_scene->GetAroundCreatureByTile(pFindCreature, pPos, 100, NULL, PT_PATH, false);
        CRegionCreature* pTargetCreature;

        for (int i = 0; i < iCnt; i++) {
            if (pFindCreature[i]->m_type == OBJECT_TYPE_CREATURE ||
                pFindCreature[i]->m_type == OBJECT_TYPE_NPC_COMBATIVE) {
                pTargetCreature = (CRegionCreature*)(pFindCreature[i]);
                if (pTargetCreature == this) {
                    continue;
                }
                if (pTargetCreature->GetPlayerLogic()->GetLiveType() ==
                    CPlayerLogic::eDead)  //死亡的
                {}
                float ScopeoDinaryInjury = 0.f;
                if (pTargetCreature == enemy) {
                    ScopeoDinaryInjury = 1.f;
                } else {
                    ScopeoDinaryInjury = ((float)m_ScopeoDinaryInjury) / 100;
                }

                if ((enemy->IsUser() || enemy->IsUserPet() || enemy->IsCallNpc()) &&
                    (pTargetCreature->IsUser() || pTargetCreature->IsUserPet() ||
                     pTargetCreature->IsCallNpc())) {
                    CRegionCreature* pSource = NULL;
                    CRegionCreature* pTarget = NULL;
                    if (enemy->IsUser()) {
                        pSource = enemy;
                    } else if (enemy->IsUserPet() || enemy->IsCallNpc()) {
                        pSource = enemy->m_master;
                    }

                    if (pTargetCreature->IsUser()) {
                        pTarget = pTargetCreature;
                    } else if (pTargetCreature->IsUserPet() || pTargetCreature->IsCallNpc()) {
                        pTarget = pTargetCreature->m_master;
                    }

                    if (pSource) {
                        bool useDefault = false;
                        switch (pSource->m_eCurPosPKAttr) {
                            case PKA_SAFEST:
                                useDefault = true;
                                continue;
                                break;
                            case PKA_NORMAL:
                            case PKA_UNSAFE:
                                if (pSource->IsSelfDef(pTarget)) {
                                    GetEntity()->RealAttack(pTargetCreature->GetEntity(), phyDmg,
                                                            eleDmg, seed, ScopeoDinaryInjury);
                                    break;
                                }
                                if (pSource->m_ePkMode == PK_Monster) {
                                    continue;
                                } else if (pSource->m_ePkMode == PK_Goodevil) {
                                    if (pTargetCreature->GetPKState() == PK_STATE_RED_NAME) {
                                        GetEntity()->RealAttack(pTargetCreature->GetEntity(),
                                                                phyDmg, eleDmg, seed,
                                                                ScopeoDinaryInjury);
                                    }
                                    break;
                                } else if (pSource->m_ePkMode == PK_Free) {
                                    GetEntity()->RealAttack(pTargetCreature->GetEntity(), phyDmg,
                                                            eleDmg, seed, ScopeoDinaryInjury);
                                    break;
                                }
                                break;
                            default:
                                useDefault = true;
                                continue;
                                break;
                        }
                    }
                } else if (!(pTargetCreature->IsUser() || pTargetCreature->IsUserPet() ||
                             pTargetCreature->IsCallNpc())) {
                    if (pTargetCreature->m_cheatDummy)
                        continue;
                    GetEntity()->RealAttack(pTargetCreature->GetEntity(), phyDmg, eleDmg, seed,
                                            ScopeoDinaryInjury);
                }
            }
        }
    } else {
        GetEntity()->RealAttack(enemy->GetEntity(), phyDmg, eleDmg, seed);
    }
}

void CRegionCreature::GetDamage(int& phyDmg, int eleDmg[MAX_ELEMENT_ATTR],
                                float ScopeoDinaryInjuryR) {
    /*RecomputeAttr();*/

    // compute damage

    long IsRidedMax, IsRidedMin = 0;
    long IsRidedElementDamage[MAX_ELEMENT_ATTR];
    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        IsRidedElementDamage[i] = 0;
    }

    //if(IsRided())
    //{
    //	IsRidedMax = (m_core.DamageMin.Base)*2.5;
    //	IsRidedMin = (m_core.DamageMax.Base)*2.5;
    //	for(int i = 0; i <MAX_ELEMENT_ATTR ;i++)
    //	{
    //		IsRidedElementArmor[i] = m_core.ElementArmor[i].Base *2.5;
    //		IsRidedElementDamage[i] = m_core.ElementDamage[i].Base *2.5;
    //	}
    //}
    int delta = 0;
    if (IsRided()) {
        IsRidedMax = (m_core.DamageMin.Base) * 2.5;
        IsRidedMin = (m_core.DamageMax.Base) * 2.5;
        delta      = IsRidedMax - IsRidedMin;
        if (delta)
            phyDmg = IsRidedMin;
        else
            phyDmg = IsRidedMin + GetRand(delta);
    } else {
        delta = (m_core.GetDamageMax()) - (m_core.GetDamageMin());
        if (delta == 0)
            phyDmg = m_core.GetDamageMin();
        else
            phyDmg = m_core.GetDamageMin() + GetRand(delta);
    }

    if (IsUser()) {
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            eleDmg[i] = m_core.ElementDamage[i].GetValue();
        }
    }
    if (IsRided()) {
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            IsRidedElementDamage[i] = m_core.ElementDamage[i].GetValue() * 2.5;
        }
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            eleDmg[i] = m_npcInfo->ElementDamage[i] + IsRidedElementDamage[i];
        }
    } else {
        /*	if(IsRided())
			{
				for(int i=0; i<MAX_ELEMENT_ATTR; i++)
				{
					eleDmg[i] = m_npcInfo->ElementDamage[i] + m_core.ElementDamage[i].GetValue();
				}
			}*/
        if (IsUserPet()) {
            for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                eleDmg[i] = m_npcInfo->ElementDamage[i] + m_core.ElementDamage[i].GetValue();
            }
        }
        if (IsMonster()) {
            for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                eleDmg[i] = m_npcInfo->ElementDamage[i] + m_core.ElementDamage[i].GetValue();
            }
        } else {
            LOG("m_npcInfo is Null , GetDamage function");
        }
    }

    /*Tianh  元素攻击的浮动值*/
    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        if (eleDmg[i] >= 5)
            eleDmg[i] = GetRand(eleDmg[i] * 1.1, eleDmg[i] * 0.9);
    }

    phyDmg = phyDmg * ScopeoDinaryInjuryR;
    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        eleDmg[i] = eleDmg[i] * ScopeoDinaryInjuryR;
    }

    //if(m_core.Metier == METIER_TAOIST || m_core.Metier == METIER_WIZARD)
    //{
    //       if (m_pItemContainer)
    //       {
    //		SItemID item = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
    //           if(ItemID_IsValid(item) && ItemIsWeapon_MagicWeapon(item.type))
    //           {
    //               int ele[MAX_ELEMENT_ATTR];
    //               MW_GetEleValue(item,ele);
    //               MW_ComputeAttenuation(ele,eleDmg);
    //           }else if(ItemID_IsValid(item))
    //		{
    //			eleDmg[ELEMENT_WATER] *= 0.5;
    //			eleDmg[ELEMENT_FIRE] *= 0.5;
    //			eleDmg[ELEMENT_POISON] *= 0.5;
    //		}
    //       }
    //}
}

void CRegionCreature::RealAttack(CRegionCreature* enemy, int phyDmg, int eleDmg[MAX_ELEMENT_ATTR],
                                 long seed, float ScopeoDinaryInjuryR) {
    int ret = 0;
    EnterCombat();
    if (!TryNormalAttack(enemy, m_core.AttackRange.GetValue())) {
        // can't do attack
        ret = USE_SKILL_FAILED;
    }

    if (!TryHit(enemy)) {
        // miss

        ret = USE_SKILL_MISS;
    }

    // compute damage
    GetDamage(phyDmg, eleDmg, ScopeoDinaryInjuryR);

    /*ret =  USE_SKILL_OK;*/

    if (enemy->m_TeamShareDamage) {
        std::vector<CRegionCreature*> RegionCreature;  //存储符合条件队友的指针容器

        //判断队的人是否还有1个以上的队友还有相同的BUF状态，否则不分担伤害
        //生命锁链只对队伍里有相同BUF的人有效果
        short        ID        = 0;
        CActorSkill* pReleaser = FindActorSkill(enemy, enemy->m_oId);
        if (pReleaser) {
            ID = pReleaser->FindEffectbyClass(LIFE_CHAINS);
        }

        if (enemy->m_pTeam) {
            for (int i = 0; i < MAX_TEAM_MENBER_COUNT; i++) {

                CRegionUser* pRegionUser =
                    g_region->FindUser(enemy->m_pTeam->mData.mMember[i].mDBID);
                if (pRegionUser) {
                    if (pRegionUser->m_dummy) {
                        if (pRegionUser->m_dummy->m_oId == enemy->m_oId) {
                            //如果是自己
                            continue;
                        }

                        CActorSkill* pActorSkill =
                            FindActorSkill(pRegionUser->m_dummy, pRegionUser->m_dummy->m_oId);
                        if (pActorSkill) {
                            if (pActorSkill->FindEffectbyId(ID)) {
                                RegionCreature.push_back(pRegionUser->m_dummy);
                            }
                        }
                    }
                }
            }
        }

        if (RegionCreature.size() < 1) {
            //没有队友有BUF状态   不分担
            RealAttackcalculating(enemy, phyDmg, eleDmg, ret, seed);
        } else {
            int viDmgElement[MAX_ELEMENT_ATTR] = {0, 0, 0};
            int kiDmgElement[MAX_ELEMENT_ATTR] = {0, 0, 0};
            //开始分担伤害
            int viDmgPhy                 = phyDmg * enemy->m_TeamShareDamage / 100;
            viDmgElement[ELEMENT_WATER]  = eleDmg[ELEMENT_WATER] * enemy->m_TeamShareDamage / 100;
            viDmgElement[ELEMENT_FIRE]   = eleDmg[ELEMENT_FIRE] * enemy->m_TeamShareDamage / 100;
            viDmgElement[ELEMENT_POISON] = eleDmg[ELEMENT_POISON] * enemy->m_TeamShareDamage / 100;
            //有k-1个队友来分担伤害
            int conut                    = RegionCreature.size();
            int kiDmgPhy                 = viDmgPhy / conut;
            kiDmgElement[ELEMENT_WATER]  = viDmgElement[ELEMENT_WATER] / conut;
            kiDmgElement[ELEMENT_FIRE]   = viDmgElement[ELEMENT_FIRE] / conut;
            kiDmgElement[ELEMENT_POISON] = viDmgElement[ELEMENT_POISON] / conut;

            std::vector<CRegionCreature*>::iterator itArea = RegionCreature.begin();
            for (; itArea != RegionCreature.end(); itArea++) {
                RealAttackcalculating(*itArea, kiDmgPhy, kiDmgElement, ret, seed);
            }

            //还属于自己本身的伤害
            float phyDmgR          = 1.0f - ((enemy->m_TeamShareDamage) / 100.0f);
            phyDmg                 = phyDmg * phyDmgR;
            eleDmg[ELEMENT_WATER]  = eleDmg[ELEMENT_WATER] * phyDmgR;
            eleDmg[ELEMENT_FIRE]   = eleDmg[ELEMENT_FIRE] * phyDmgR;
            eleDmg[ELEMENT_POISON] = eleDmg[ELEMENT_POISON] * phyDmgR;

            RealAttackcalculating(enemy, phyDmg, eleDmg, ret, seed);
        }
    } else {
        RealAttackcalculating(enemy, phyDmg, eleDmg, ret, seed);
    }
}

void CRegionCreature::RealAttackcalculating(CRegionCreature* enemy, int phyDmg,
                                            int eleDmg[MAX_ELEMENT_ATTR], int ret, long seed) {
    char eleSpec  = -1;
    bool critical = false;
    long enemyId  = enemy->m_oId;
    enemy->ApplyDamage(this, phyDmg, eleDmg, false);
    if ((m_core.CriticalHit.GetValue() - enemy->m_core.ReCriticalHit.GetValue()) >
        0)  //增加抗必杀属性差数运算
    {
        if (phyDmg > 0 &&
            GetProb((m_core.CriticalHit.GetValue() - enemy->m_core.ReCriticalHit.GetValue()) /
                    100.0f)) {
            //Tian 增加爆击伤害
            phyDmg   = phyDmg * (2 + m_core.CriticalDamageR / 100.0) + m_core.CriticalDamage;
            critical = true;
        } else {
            critical = false;
        }
    } else {
        critical = false;
    }

    //格挡参加运算
    if (enemy->m_core.ReParry.GetValue() > 0) {
        if (phyDmg > 0 && GetProb(enemy->m_core.ReParry.GetValue() / 100.0f)) {
            phyDmg = 0;

            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_display);
            g_sendCmd->WriteShort(GEDANG);
            g_sendCmd->WriteLong(GetObjectId());
            if (m_scene)  //tim.yang
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        }
    }

    ////Tianh   策划测试用
    //if (IsUser())
    //{
    //	if (enemy->IsUser())
    //	{
    //		RtCoreLog().Debug("您的普通【物理】攻击对敌人[玩家]造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("您的普通【物理 元素 (水)】攻击对敌人[玩家]造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (火)】攻击对敌人[玩家]造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (冰)】攻击对敌人[玩家]造成伤害是: %d\n",eleDmg[2]);
    //	}
    //	else if (enemy->IsMonster())
    //	{
    //		RtCoreLog().Debug("您的普通【物理】攻击对敌人[怪物]造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("您的普通【物理 元素 (水)】攻击对敌人[怪物]造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (火)】攻击对敌人[怪物]造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (冰)】攻击对敌人[怪物]造成伤害是: %d\n",eleDmg[2]);
    //	}
    //	else if (enemy->IsUserPet())
    //	{
    //		RtCoreLog().Debug("您的普通【物理】攻击对敌人[宠物]造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("您的普通【物理 元素 (水)】攻击对敌人[宠物]造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (火)】攻击对敌人[宠物]造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("您的普通【物理 元素 (冰)】攻击对敌人[宠物]造成伤害是: %d\n",eleDmg[2]);
    //	}
    //

    //}
    //else if (IsMonster())
    //{
    //	if (enemy->IsUserPet())
    //	{
    //		RtCoreLog().Debug("怪物的普通【物理】攻击对您的[宠物]造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (水)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (火)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (冰)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[2]);
    //	}
    //	else
    //	{
    //		RtCoreLog().Debug("怪物的普通【物理】攻击对您造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (水)】攻击对您造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (火)】攻击对您造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("怪物普通【物理 元素 (冰)】攻击对您造成伤害是: %d\n",eleDmg[2]);
    //	}
    //}
    //else if (IsUserPet())
    //{
    //	if (enemy->IsUser())
    //	{
    //		RtCoreLog().Debug("对方宠物的普通【物理】攻击对您造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (水)】攻击对您造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (火)】攻击对您造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (冰)】攻击对您造成伤害是: %d\n",eleDmg[2]);
    //	}
    //	else if (enemy->IsUserPet())
    //	{
    //		RtCoreLog().Debug("对方宠物的普通【物理】攻击对您的[宠物]造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (水)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (火)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("对方宠物的普通【物理 元素 (冰)】攻击对您的[宠物]造成伤害是: %d\n",eleDmg[2]);
    //	}
    //	else if (enemy->IsMonster())
    //	{
    //		RtCoreLog().Debug("您宠物的普通【物理】攻击对怪物造成伤害是: %d\n",phyDmg);
    //		RtCoreLog().Debug("您宠物的普通【物理 元素 (水)】攻击对怪物造成伤害是: %d\n",eleDmg[0]);
    //		RtCoreLog().Debug("您宠物的普通【物理 元素 (火)】攻击对怪物造成伤害是: %d\n",eleDmg[1]);
    //		RtCoreLog().Debug("您宠物的普通【物理 元素 (冰)】攻击对怪物造成伤害是: %d\n",eleDmg[2]);
    //	}
    //}

    if (enemy->m_IsInvincible) {
        phyDmg    = 0;
        eleDmg[0] = 0;
        eleDmg[1] = 0;
        eleDmg[2] = 0;
    }

    if (ret != USE_SKILL_OK && ret != USE_SKILL_MISS)
        return;

    long dead = enemy->RecvDamage(GetControl(), phyDmg, eleDmg, false);

    long criHit = 0;

    if (critical)
        criHit = 1;
    else
        criHit = 0;

    //AddBattleState(enemy);
    // eleDmg[0] = rtRandom(100,0);
    // eleDmg[1] = rtRandom(100,0);
    // eleDmg[2] = rtRandom(100,0);

    // build event to client
    // ...
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_event);
    g_sendCmd->WriteLong(g_region->m_time);
    g_sendCmd->WriteShort(EVENT_ATTACK);
    g_sendCmd->WriteLong(m_oId);

    g_sendCmd->WriteShort(seed);
    g_sendCmd->WriteShort(0);  // normal attack
    /*
	g_sendCmd->SerializeBit(criHit,1);				// critical hit
	long tmp = eleSpec;
	g_sendCmd->SerializeBit(tmp,4);
	*/
    g_sendCmd->WriteByte(SKILL_TARGET_TYPE_ACTOR);
    //PZH
    g_sendCmd->WriteByte(0);  //是否是吟唱(真打为0 吟唱为1)
    //
    g_sendCmd->WriteLong(enemyId);

    g_sendCmd->WriteLong(enemyId);

    // attack ret
    g_sendCmd->WriteByte(ret);
    //PZH
    g_sendCmd->WriteByte(0);  //普通攻击永远发0
    //
    if (ret == USE_SKILL_OK) {
        g_sendCmd->SerializeBit(dead, 3);
        g_sendCmd->SerializeBit(criHit, 1);
        g_sendCmd->WriteShort(phyDmg);
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++)
            g_sendCmd->WriteShort(eleDmg[i]);
    }
    if (m_scene)  //tim.yang
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);

    ClearHidingAndTopo();  //取消隐身或者遁地状态
                           // LOG1("attack packet size = %d",g_sendCmd->GetByteDataSize());
}

/*
void CRegionCreature::DoAttack(SAttackParam &param)
{
	// m_combatBusy += attack interval
	// fix later
	// ...
	if(m_combatBusy && rtGetMilliseconds() < m_combatBusy) return;

	CRegionCreature *t=NULL;
	if (param.Skill==0 || (param.Skill!=0 && param.TargetType==SKILL_TARGET_TYPE_ACTOR))
	{
		t = m_scene->FindCreature(param.EnemyId);
		if (!t) return;
	}
	if(IsUser())
	{
		m_combatBusy = rtGetMilliseconds() + 500;

		// 查看是否允许攻击(PK)
		if (t && t->IsUser())
		{
			EPKAttr ePKAttr = m_eCurPosPKAttr;
			if (0) // 国战的时候安全区会变成普通区
			{
				if (ePKAttr==PKA_SAFE)
				{
					ePKAttr = PKA_NORMAL;
				}
			}
			switch (ePKAttr)
			{
			case PKA_NORMAL:     // 普通区域(有红名惩罚)
				// 任何时候都可以攻击(相同阵营之间会被认为是强制攻击)
				break;
			case PKA_UNSAFE:     // 不安全区域(无红名惩罚)
				// 可以攻击
				break;
			case PKA_SAFE:       // 安全区(除了国战状态都是不允许PK的)
			case PKA_SAFEST:     // 绝对安全区(新手区)
				// 安全区不允许攻击
				return;
			}
		}
	}else
	{
		m_combatBusy = rtGetMilliseconds() + 3000;
	}

	// if ride on pet,pet do attack (必须是普通攻击)
	if(IsRidePet() && param.Skill==0)
	{
		m_pet->DoAttack(param);
		return;
	}

	short damage = 0;
	float att = m_core.Attack.GetValue();
	int delta = m_core.GetDamageMax() - m_core.GetDamageMin();
	if(delta == 0)
		damage = m_core.GetDamageMin();
	else
		damage = m_core.GetDamageMin() + GetRand(delta);
	if(param.Skill == 0)
	{
		// don't attack pet
		if(t == m_pet) return;
		if(t && t->IsRidePet())
		{
			t = t->m_pet;
		}
	}else
	{
		SSkill* pSkill = Skill()->FindSkill(param.Skill);
		if (pSkill==NULL) return;

		if (param.TargetType==SKILL_TARGET_TYPE_ACTOR)
		{
			// don't attack pet
			if(t==NULL || t == m_pet) return;
			if(t->IsRidePet())
			{
				t = t->m_pet;
			}

			UseSkill(pSkill, param);
			short sLastDamage = t->m_core.GetLastDamage();
			if (sLastDamage)
			{
				damage += sLastDamage;
			}else
			{
				damage = 0; // 如果伤害为0就认为是一个没有伤害的技能
			}
			if (damage==0) return;
		}
		else
		{
			UseSkill(pSkill, param);
			return;
		}
	}

	// set pet ai target
	if(m_enemy==0 || m_enemy!=param.EnemyId)
		m_enemy = param.EnemyId;

	if(m_pet && m_pet->m_ai)
		m_pet->m_ai->OnAttack(t);

	// check attack speed and range
	// ...
	ApplyDamage(damage, att, t);
	// RecvDamage(damage,)

	// build event
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_event);
	g_sendCmd->WriteLong(g_region->m_time);
	g_sendCmd->WriteShort(EVENT_ATTACK);
	g_sendCmd->WriteLong(m_oId);
	*g_sendCmd << param;
	if (t==NULL)
	{
		g_sendCmd->WriteByte(0);
	}else
	{
		g_sendCmd->WriteByte(1);				// only one target
		g_sendCmd->WriteLong(t->m_oId);
		g_sendCmd->WriteShort(damage);
	}
	m_scene->BroadcastCmd(m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
}
*/

/*
void CRegionCreature::ApplyDamage(short& sDamage, float fAtt, CRegionCreature* pTarget)
{
	// careful,object may be destory in recv damage
	// sDamage -= pTarget->m_core.Armor.GetValue();
	sDamage /= (1+pTarget->m_core.Armor.GetValue()/65.0);
	if(sDamage < 0) sDamage = 1;

	// check if miss
	// float dog = pTarget->m_core.Dodge.GetValue();
	// float rate = 2*fAtt/(fAtt+dog);
	// if(rate > 1) rate = 1;
	// if(rate < 0.05) rate = 0.05;
	// if(!GetProb(rate)) sDamage = 0;

	pTarget->OnBeAttacked(this, sDamage);
	pTarget->RecvDamage(this,sDamage);
}
*/

bool CRegionCreature::CanUseSkill(short sSkillID) {
    return m_Skill.CanUseSkill((unsigned short)sSkillID);
}

bool CRegionCreature::UseSkill(SAttackParam& param) {
    //PZH
    bool bRet = false;
    do {
        bool    bRealUse  = false;
        char    cIntonate = 0;
        SSkill* pSkill    = Skill()->FindSkill(param.Skill);
        if (NULL == pSkill) {
            break;
        }

        if (pSkill->wSubID == 64)  //遁地
        {
            m_Skill.TopoClearUseful();
        }

        if (pSkill->wClass == 13 && IsChengBody())  // 变身状态下不能使用法宝技能
        {
            break;
        }

        bool bIsIntonatint = this->GetPlayerLogic()->IsIntonating();
        if (0 == pSkill->iIsIntonate) {
            if (bIsIntonatint) {
                GetPlayerLogic()->StopIntonate();
                break;
            }
            bRealUse = true;
        } else {
            if (!bIsIntonatint) {
                if (0 != GetPlayerLogic()->StartIntonate(param)) {
                    break;
                }
            } else {
                if (GetPlayerLogic()->GetSAttackParam().Skill != param.Skill) {
                    GetPlayerLogic()->StopIntonate();
                    break;
                }
                unsigned long dwT1 = rtGetMilliseconds();
                unsigned long dwT2 = GetPlayerLogic()->GetFireTime();
                if (dwT1 < dwT2) {
                    GetPlayerLogic()->SetMiss(true);
                    //GetPlayerLogic()->StopIntonate();
                    //break;
                }
                bRealUse = true;
            }
        }
        bool bSend = true;
        if (bRealUse) {
            if (bIsIntonatint) {
                GetPlayerLogic()->StopIntonate();
            }
            m_Skill.m_bSkillWriteDamage = TRUE;
            m_Skill.m_listSkillDamage.clear();
            if (!UseSkill(pSkill, param)) {
                bSend = false;
            } else {
                m_Skill.m_bSkillWriteDamage = FALSE;
                if (m_Skill.m_listSkillDamage.size() == 0 && param.TargetType != 2) {
                    bSend = false;
                }
                bRet = true;
            }
        } else {
            cIntonate = 1;
        }
        if (!bSend) {
            break;
        }
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_event);
        g_sendCmd->WriteLong(g_region->m_time);
        g_sendCmd->WriteShort(EVENT_ATTACK);
        g_sendCmd->WriteLong(m_oId);

        g_sendCmd->WriteShort(param.Seed);
        g_sendCmd->WriteShort(param.Skill);  // skill attack
        g_sendCmd->WriteByte(param.TargetType);
        g_sendCmd->WriteByte(cIntonate);  //是否是吟唱(真打为0 吟唱为1)
        switch (param.TargetType) {
            case SKILL_TARGET_TYPE_NONE:
                break;
            case SKILL_TARGET_TYPE_ACTOR:
                g_sendCmd->WriteLong(param.EnemyId);
                break;
            case SKILL_TARGET_TYPE_POSITION:
                g_sendCmd->WriteLong(param.Pos[0]);
                g_sendCmd->WriteLong(param.Pos[1]);
                g_sendCmd->WriteLong(param.Pos[2]);
                break;
        }
        if (0 == cIntonate) {
            GetPlayerLogic()->SetCurUseSkill(pSkill);
            m_Skill.SkillWriteDamageBuffer(g_sendCmd);
            GetPlayerLogic()->SetCurUseSkill(NULL);
        }

        if (!m_scene) {
            //MSG4("m_scene为空!\n");
            break;
        }
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        bRet = bSend;
    } while (false);
    return bRet;
    //   SSkill* pSkill = Skill()->FindSkill(param.Skill);
    //   if (pSkill)
    //   {
    //       m_Skill.m_bSkillWriteDamage = TRUE;
    //       m_Skill.m_listSkillDamage.clear();
    //       if (UseSkill(pSkill, param))
    //       {
    //           m_Skill.m_bSkillWriteDamage = FALSE;
    //           if (m_Skill.m_listSkillDamage.size()>0)
    //           {
    //               g_sendCmd->BeginWrite();
    //               g_sendCmd->WriteShort(r2c_event);
    //               g_sendCmd->WriteLong(g_region->m_time);
    //               g_sendCmd->WriteShort(EVENT_ATTACK);
    //               g_sendCmd->WriteLong(m_oId);

    //               g_sendCmd->WriteShort(param.Seed);
    //               g_sendCmd->WriteShort(param.Skill); // skill attack
    //               g_sendCmd->WriteByte(param.TargetType);
    //               switch (param.TargetType)
    //               {
    //               case SKILL_TARGET_TYPE_NONE:
    //                   break;
    //               case SKILL_TARGET_TYPE_ACTOR:
    //                   g_sendCmd->WriteLong(param.EnemyId);
    //                   break;
    //               case SKILL_TARGET_TYPE_POSITION:
    //                   g_sendCmd->WriteLong(param.Pos[0]);
    //                   g_sendCmd->WriteLong(param.Pos[1]);
    //                   g_sendCmd->WriteLong(param.Pos[2]);
    //                   break;
    //               }

    //               m_Skill.SkillWriteDamageBuffer(g_sendCmd);

    //			if (!m_scene)
    //			{
    //				//MSG4("m_scene为空!\n");
    //				return false;
    //			}
    //               m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    //           }
    //           return true;
    //       }
    //       m_Skill.m_bSkillWriteDamage = FALSE;
    //   }
    ////MSG4("没有发现ID为：%d 的技能\n", param.Skill);
    //   return false;
    //
}

//Tianh 删除套装技能效果
void CRegionCreature::DeleteSkill(unsigned short wID) {
    m_Skill.DeleteSkill(wID, true);
}

void CRegionCreature::UseSkill(unsigned short wID) {
    m_Skill.AddSkill(wID, 100, true);
}

bool CRegionCreature::UseSkill(SSkill* pSkill, SAttackParam& param) {
    bool bUseFinish = false;

    //if(m_pet && pSkill->dwRTarget == 8)
    //{
    //	param.TargetType = SKILL_TARGET_PET;
    //	param.EnemyId = m_pet->m_oId;
    //}
    if (pSkill->dwRTarget == 0)  // 对自己使用技能
    {
        bUseFinish = m_Skill.UseSkill(param.Skill);
        return bUseFinish;
    } else if (pSkill->dwRTarget & SKILL_TARGET_POSITION)  // 对地使用的技能
    {
        if (pSkill->bABad) {
            if (!CanAttack(NULL, pSkill))
                return false;
        }
        float fPos[3];
        fPos[0]    = param.Pos[0];
        fPos[1]    = param.Pos[1];
        fPos[2]    = param.Pos[2];
        bUseFinish = m_Skill.UseSkill(param.Skill, fPos);
        //if(bUseFinish && pSkill->bABad)
        //	AddBattleState(m_scene->FindCreature(param.EnemyId));
        return bUseFinish;
    } else  // 对敌人使用的技能
    {
        CRegionCreature* pActor = m_scene->FindCreature(param.EnemyId);
        if (pActor == NULL)
            return false;
        CRegionCreature* pEnemy = pActor;
        if (pSkill->wID != PET_SKILL_RIDE) {
            pActor = pActor->GetEntity();
        }
        if (pSkill->bABad) {
            if (!CanAttack(pActor, pSkill))
                return false;

            if (!pSkill->IsAddHpSkill) {
                // 如果是有害技能，就通知兽我在打他
                if (m_pet && m_pet->m_ai) {
                    CRegionPetAI* petAI = (CRegionPetAI*)m_pet->m_ai;
                    if (petAI && (petAI->m_eAS == CRegionPetAI::AS_ATTACK ||
                                  petAI->m_eAS == CRegionPetAI::AS_SYNERGY)) {
                        petAI->OnAttack(pActor);
                    }
                }

                //Tianh   召唤的作战NPC  这里只有USER才可以有
                if (CallNpc.size() != 0) {
                    std::vector<CRegionCreature*>::iterator vit = CallNpc.begin();
                    for (; vit != CallNpc.end(); vit++) {
                        if ((*vit)->m_ai) {
                            CRegionCallNpc* CallNpcAI = NULL;
                            CallNpcAI                 = (CRegionCallNpc*)(*vit)->m_ai;
                            if (CallNpcAI && (CallNpcAI->m_eAS == CRegionCallNpc::AS_ATTACK ||
                                              CallNpcAI->m_eAS == CRegionCallNpc::AS_SYNERGY)) {
                                CallNpcAI->OnAttack(pActor);
                            }
                        }
                    }
                }
            }
        }
        bUseFinish = m_Skill.UseSkill(param.Skill, pActor->m_oId);
        if (bUseFinish) {
            //是有害技能时，进入战斗
            //非有害技能时，目标是战斗状态时也进入战斗
            //if(pSkill->bABad)
            //	AddBattleState(pEnemy);
            //else if(pEnemy && pEnemy->HaveBattleState())
            //	AddBattleState(pEnemy);
        }
        return bUseFinish;
    }
}

void CRegionCreature::ApplyDamage(CRegionCreature* enemy, int& phyDmg, int eleDmg[MAX_ELEMENT_ATTR],
                                  bool isSkill, SSkill* pSkillAttr) {
    /*Tianh 计算物理吸收*/
    //Min(0.90,(防御)/((防御)+3000))
    float PhysicalAbsorption, PhysicalAbsorption1 = 0.0f;

    float Armor         = m_core.GetArmor();
    PhysicalAbsorption1 = (Armor / (Armor + 3000));

    PhysicalAbsorption = Min(0.9f, PhysicalAbsorption1);

    /*Tianh 伤害计算*/
    //物理伤害(1)*Min(1,Max(0.2,(1+(我方等级-对方等级)/20)))*(1-物理伤害吸收)
    float factor  = 0.0f;
    float factor1 = 0.0f;
    float factor2 = 0.0f;

    factor2 = 1 + (enemy->m_core.Lev - m_core.Lev) / 30;

    factor1 = Max(0.2f, factor2);
    factor  = Min(2.0f, factor1);

    phyDmg *= factor * (1 - PhysicalAbsorption);

    //if(IsUser())
    //{
    //	//if(enemy->m_core.Lev > m_core.Lev + 5)
    //	//{
    //	//	phyDmg *= (1+(enemy->m_core.Lev - m_core.Lev)*0.2);
    //	//}
    //	//else if(enemy->m_core.Lev > m_core.Lev)
    //	//{
    //	//	phyDmg *= (1+(enemy->m_core.Lev - m_core.Lev)*0.1);
    //	//}
    //	/*phyDmg *= factor*(1-PhysicalAbsorption);*/

    //}
    //else if(IsUserPet())
    //{
    //	//if(enemy->m_core.Lev > m_core.Lev + 10)
    //	//{
    //	//	phyDmg *= (1+(enemy->m_core.Lev - m_core.Lev)*0.2);
    //	//}
    //	//else if(enemy->m_core.Lev > m_core.Lev + 5)
    //	//{
    //	//	phyDmg *= (1+(enemy->m_core.Lev - m_core.Lev)*0.1);
    //	//}
    //	/*phyDmg *= factor*(1-PhysicalAbsorption);*/
    //}

    /*phyDmg /= (1+(m_core.GetArmor()/35.0f));*/
    /*phyDmg -= m_core.DamageAbsorb.GetValue();*/

    //物理伤害(2)*(1-附加物理伤害吸收值)
    phyDmg *= 1 - m_core.PhyDmgAbsorbRate / 100;

    if (phyDmg < 0)
        phyDmg = 1;

    //if( (enemy->IsUser()||enemy->IsUserPet()) && IsMonster() )
    //{
    //	if(m_core.Lev < 10)
    //	{

    //	}
    //	else if(m_core.Lev < 30)
    //	{
    //		phyDmg *= 1.25;
    //	}
    //	else if(m_core.Lev < 45)
    //	{
    //		phyDmg *= 1.15;
    //	}
    //	else
    //	{
    //		phyDmg *= 1.1;
    //	}
    //}

    //if (IsUser())
    //{

    /*Tianh 元素的伤害吸收*/
    //Min(0.75,火防御/(火防御+3000))
    if (IsRided()) {
        long IsRidedElementArmor[MAX_ELEMENT_ATTR];
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            IsRidedElementArmor[i] = m_core.ElementArmor[i].GetValue() * 2.5;
        }
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            TemporaryElementArmor1[i] =
                (float)(IsRidedElementArmor[i]) / (IsRidedElementArmor[i] + 3000);

            if (TemporaryElementArmor1[i] >= 0.75) {
                TemporaryElementArmor[i] = 0.75;
            } else {
                TemporaryElementArmor[i] = TemporaryElementArmor1[i];
            }
        }
    } else {
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            TemporaryElementArmor1[i] = (float)(m_core.ElementArmor[i].GetValue()) /
                                        (m_core.ElementArmor[i].GetValue() + 3000);

            if (TemporaryElementArmor1[i] >= 0.75) {
                TemporaryElementArmor[i] = 0.75;
            } else {
                TemporaryElementArmor[i] = TemporaryElementArmor1[i];
            }
        }
    }

    //}
    //else
    //{
    //if (m_npcInfo)
    //{
    //	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
    //	{
    //		TemporaryElementArmor1[i] = (m_npcInfo->ElementArmor[i]+m_core.ElementArmor[i].GetValue())/(m_npcInfo->ElementArmor[i]+3000);

    //		if (TemporaryElementArmor1[i] >= 0.75)
    //		{
    //			TemporaryElementArmor[i] = 0.75;
    //		}
    //		else
    //		{
    //			TemporaryElementArmor[i] = TemporaryElementArmor1[i];
    //		}
    //	}
    //}
    //else
    //{
    //	LOG("m_npcInfo is Null , ApplyDamage function");
    //}

    /*}*/

    if ((1 + (enemy->m_core.Lev - m_core.Lev) / 30) > 0.2) {
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            ElementFactor1[i] = (float)(1 + (enemy->m_core.Lev - m_core.Lev) / 50);
        }
    } else {
        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            ElementFactor1[i] = 0.2;
        }
    }

    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        if (ElementFactor1[i] < 2.0) {
            ElementFactor[i] = ElementFactor1[i];
        } else if (ElementFactor1[i] >= 2.0) {
            ElementFactor[i] = 2.0;
        }
    }

    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        eleDmg[i] *= ElementFactor[i] * (1 - TemporaryElementArmor[i]);
        eleDmg[i] *= (1 - m_core.ElementAbsorb[i].GetValue());

        //<add by Tianh> 如果是普通攻击   则元素伤害*1/4
        if (!isSkill) {
            eleDmg[i] *= 0.25;
        }
    }

    //for(int i=0; i<MAX_ELEMENT_ATTR; i++)
    //{
    //	eleDmg[i] /= (1 + m_core.ElementArmor[i].GetValue()/ELE_ARMOR_SCALE);
    //	eleDmg[i] -= m_core.ElementAbsorb[i].GetValue();
    //	if(eleDmg[i] < 0) eleDmg[i] = 0;
    //}

    // pvp的伤害衰减
    if (enemy) {
        if ((IsUser() || IsUserPet() || IsCallNpc()) &&
            (enemy->IsUser() || enemy->IsUserPet() || enemy->IsCallNpc())) {
            phyDmg *= 0.25;
            for (int i = 0; i < MAX_ELEMENT_ATTR; i++)
                //eleDmg[i] *= 0.5;
                eleDmg[i] *= 0.25;  //降低元素属性的PvP衰减
        } else {
            phyDmg *= (100.0f - m_core.PhyDmgAbsorbRate) / 100.0f;
        }
    }

    ////战士所受物理伤害降低15%
    //if(m_core.Metier == METIER_WARRIOR)
    //{
    //	if(enemy && enemy->IsMonster()) //另战士只有在PvE的时候才享受衰减
    //		phyDmg *= 0.85;
    //}

    // 神仙对普通npc的伤害加成
    //	if(enemy && !IsUserPet())
    //	{
    //		if(enemy->m_npcInfo && m_npcInfo)
    //		{
    //			if(enemy->m_npcInfo->Id>=1254 && enemy->m_npcInfo->Id<=1259)
    //				phyDmg *= 1;//Tianh 这里减少对神仙的伤害加成
    //		}
    //	}
}

long CRegionCreature::RecvDamage(CRegionCreature* enemy, int& phyDmg, int eleDmg[MAX_ELEMENT_ATTR],
                                 int AttackType) {
    if (enemy) {
        if (enemy == this)  //自己不能把自己杀死
        {
            return (CPlayerLogic::eFlop);
        }
    }

    //if (enemy)
    //{
    //	if (enemy->ExtractHpR != 0) //按百分比抽取对方血量  最大是ExtractMaxHp
    //	{
    //		int ihp = m_core.GetHp() * (ExtractHpR/100);
    //		if (ihp > ExtractMaxHp)
    //		{
    //			ihp = ExtractMaxHp;
    //		}
    //		AddHp(-ihp);
    //	}
    //}

    EnterCombat();
    if (m_dwCatchPet)
        StopCatchSpirit(PET_CATCH_ERROR_BEATTACKED);

    long nRet = CPlayerLogic::eLive;

    if (AttackType)  //如果接收的是范围攻击
    {
        ClearHidingAndTopo();  //取消隐身或者遁地状态
    }
    if (m_flag & OBJECT_FLAG_DESTROY) {
        if (m_scene) {
            m_scene->RemoveCreature(m_oId);
        }
        return CPlayerLogic::eDead;
    }
    if (/*PZH*/ /*m_bDead*/ !this->GetPlayerLogic()->IsLive()) {
        return GetPlayerLogic()->GetLiveType();
        //return CPlayerLogic::eDead;
    }

    //------------add start by tony 05.07.26-------------------------//
    if (m_npcInfo != NULL) {
        if (g_RegionCastleWarMgr.IsGuardAnimal(m_npcInfo->Id) == true && m_mc &&
            !m_mc->m_bstartTask)  //tim.yang  MC  在没有进行怪物攻城的时候才允许城战
        {
            if (enemy != NULL) {
                if (g_RegionCastleWarMgr.CanAttack(m_npcInfo->Id, enemy, m_oId) == false) {
                    return 0;
                }
            }
        }
    }
    //------------add end   by tony 05.07.26-------------------------//

    //long dead = 0;
    int finalDmg;
    finalDmg = phyDmg;

    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        finalDmg += eleDmg[i];
    }

    int realDamage = finalDmg;

    if (enemy) {
        if (enemy->IsUser()) {
            if (enemy->m_pItemContainer->LoseEquip(CItemContainerBase::WEAPON_1, 1))
                enemy->RefreshEquipWear();
        }
    }

    if (IsUser()) {
        vector<CItemContainerBase::EEquipParts> part;
        for (int i = CItemContainerBase::NECKLACE; i <= CItemContainerBase::KITS; i++) {
            SItemID item = m_pItemContainer->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
            if (ItemID_IsValid(item)) {
                part.push_back((CItemContainerBase::EEquipParts)i);
            }
        }
        if (part.size()) {
            if (m_pItemContainer->LoseEquip(part[GetRand(part.size() - 1)], 1))
                RefreshEquipWear();
        }
    }

    //Tian 这里增加了血盾的影响(俗称魔法盾) 10.01.29
    if (0 == m_core.GetShieldHp() && 0 == m_core.GetShieldMpR()) {
        if (m_core.GetHp() < realDamage)
            realDamage = m_core.GetHp();

        AddHp(-finalDmg);

        //用伤害率把受到的伤害转化成HP
        if (m_core.AbsorptionHpR) {
            AddHp(finalDmg * (m_core.AbsorptionHpR / 100.0));
        }

        if (enemy) {
            //Tianh 反弹伤害
            if (ReboundHurtR != 0) {
                int ReboundHurtDamage      = 0;
                int vele[MAX_ELEMENT_ATTR] = {0, 0, 0};
                if (ReboundHurtRType == 1) {
                    ReboundHurtDamage = phyDmg * (ReboundHurtR / 100.0);
                    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                        vele[i] += eleDmg[i] * (ReboundHurtR / 100.0);
                    }
                } else if (ReboundHurtRType == 2) {
                    ReboundHurtDamage = phyDmg * (ReboundHurtR / 100.0);
                } else if (ReboundHurtRType == 3) {
                    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
                        vele[i] += eleDmg[i] * (ReboundHurtR / 100.0);
                    }
                }
                long dead = enemy->RecvDamage(this, ReboundHurtDamage, vele);
                /*m_Skill.SkillWriteDamage(enemy->m_oId, 0, ReboundHurtDamage, vele,dead,0);	*/

                long enemyId = enemy->m_oId;
                long criHit  = 0;
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(r2c_event);
                g_sendCmd->WriteLong(g_region->m_time);
                g_sendCmd->WriteShort(EVENT_ATTACK);
                g_sendCmd->WriteLong(m_oId);
                g_sendCmd->WriteShort(0);
                g_sendCmd->WriteShort(0);  // normal attack
                g_sendCmd->WriteByte(SKILL_TARGET_TYPE_ACTOR);
                g_sendCmd->WriteByte(0);  //是否是吟唱(真打为0 吟唱为1)
                g_sendCmd->WriteLong(enemyId);
                g_sendCmd->WriteLong(enemyId);
                g_sendCmd->WriteByte(USE_SKILL_OK);
                g_sendCmd->WriteByte(0);

                g_sendCmd->SerializeBit(dead, 3);
                g_sendCmd->SerializeBit(criHit, 1);
                g_sendCmd->WriteShort(ReboundHurtDamage);
                for (int i = 0; i < MAX_ELEMENT_ATTR; i++)
                    g_sendCmd->WriteShort(vele[i]);

                if (m_scene)
                    m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
            }

            if (enemy->m_core.AbsorbHp.GetValue()) {
                enemy->AddHp((enemy->m_core.AbsorbHp.GetValue() / 100.0f) * realDamage, 1);
            }

            if (enemy->m_core.AbsorbHpByRate.GetValue() != 0) {
                if (GetProb(0.05))
                    enemy->m_core.AddHp(enemy->m_core.AbsorbHpByRate.GetValue() / 100.0f *
                                        realDamage);
            }

            if (enemy->m_core.AbsorbMp.GetValue()) {
                float absorbMpR = enemy->m_core.AbsorbMp.GetValue() / 100.f;
                float Damage    = realDamage * absorbMpR;
                if (m_core.GetMp() > 0)  //有蓝吸蓝
                {
                    if (m_core.GetMp() < Damage) {
                        Damage = m_core.GetMp();
                    }
                    m_core.AddMp(-Damage);
                    enemy->m_core.AddMp(Damage);
                }
                //else  //没蓝吸血
                //{
                //	if (m_core.GetHp() < Damage)
                //	{
                //		Damage = m_core.GetHp();
                //	}
                //	m_core.AddHp(-Damage);
                //	enemy->m_core.AddMp(Damage);
                //}
            }

            int absorbMpRate = enemy->m_core.AbsorbMpRate.GetValue();
            if (absorbMpRate > 0) {
                float mpRate = absorbMpRate / 100.0f;
                enemy->m_core.AddMp(mpRate * realDamage);
            }

            if (enemy->IsUserPet() || enemy->IsCallNpc()) {
                if (GetProb(enemy->m_petAbsorbMp / 100.0f)) {
                    enemy->m_core.AddMp(1);
                }
                if (enemy->m_petAbsorbHp > 0) {
                    enemy->m_master->AddHp(realDamage * enemy->m_petAbsorbHp / 100.0);
                }
            }
        }

        if (m_core.GetHp() <= 0) {
            m_core.SetHp(0);
            //dead = 1;
        }

        OnBeAttacked(enemy, realDamage);
        if (m_core.GetHp() == 0) {
            if (IsUserPet() || IsCallNpc()) {
                /* m_pMaster->CallbackPet();*/
                // m_core.SetHp(m_core.MaxHp.GetValue());
            } else {
                //m_core.SetHp(m_core.MaxHp.GetValue()/2);
            }
            nRet = Die(enemy);
        }
    } else if (m_core.GetShieldMpR() && m_core.GetShieldHp()) {
        //Tianh  圣巫的盾是掉蓝的  蓝掉光了  则盾消失  时间到了也会消失
        if (m_core.Metier == METIER_TAOIST)  //圣巫
        {
            if (m_core.GetMp() < realDamage)
                realDamage = m_core.GetMp();

            long Dmg = finalDmg * m_core.GetShieldMpR() / 100.0;

            m_core.AddMp(-Dmg);

            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_display);
            g_sendCmd->WriteShort(XISHOU);
            g_sendCmd->WriteLong(GetObjectId());
            g_sendCmd->WriteLong(finalDmg);
            if (m_scene) {
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
            }

            OnBeAttacked(enemy, realDamage);

            if (m_core.GetMp() == 0) {
                m_core.SetShieldHp(0);
                m_core.SetShieldMpR(0);
                m_Skill.ClearEffByClass(18);  //去掉魔法盾效果(圣巫)
                m_core.IsShieldMpR = false;
            }
        }
    } else {
        if (m_core.GetShieldHp() < realDamage)
            realDamage = m_core.GetShieldHp();

        m_core.AddShieldHp(-finalDmg);

        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_display);
        g_sendCmd->WriteShort(XISHOU);
        g_sendCmd->WriteLong(GetObjectId());
        g_sendCmd->WriteLong(finalDmg);
        if (m_scene) {
            m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        }
        OnBeAttacked(enemy, realDamage);

        if (m_core.GetShieldHp() == 0) {
            m_Skill.ClearEffByClass(12);  //Tianh 这里要清除掉魔法盾特效
        }
    }

    return nRet;
}

char CRegionCreature::GetPKState() {
    if (m_core.Pk == 0)
        return PK_STATE_NORMAL;

    if (m_core.Pk >= g_pkConfig->iStateChangeTimeRedName * 60 * 1000)
        return PK_STATE_RED_NAME;
    else if (m_core.Pk >= g_pkConfig->iStateChangeTimeYellowName * 60 * 1000)
        return PK_STATE_YELLOW_NAME;

    return PK_STATE_NORMAL;
}

//判断是否在参数指定名字的区域里
bool CRegionCreature::AtAreaByName(const char* sAreaName) {
    if (!sAreaName)
        return false;

    std::list<RtsSceneBlockRtb::SArea*>::iterator itArea = m_listArea.begin();
    for (; itArea != m_listArea.end(); itArea++) {
        if (strcmp((*itArea)->szName, sAreaName) == 0)
            return true;
    }

    return false;
}

// 已经转换为主人
void CRegionCreature::OnPkOther(CRegionCreature* other) {
    //------------------chg start by tony 05.06.09-----------------------------------------//
    //if (g_RegionCastleWarMgr.IsCastleWarTime(this)) // 城战
    if (((m_core.Faction == FACTION_ZHOU) && (g_RegionCastleWarMgr.IsInTimeZone("XiQi"))) ||
        ((m_core.Faction == FACTION_SHANG) && (g_RegionCastleWarMgr.IsInTimeZone("ZhaoGe")))) {
        if (g_RegionCastleWarMgr.InCastleWarArea(this)) {
            return;
        }
    }
    //------------------chg end   by tony 05.06.09-----------------------------------------//

    if (g_WarfieldMgr.IsInWarfield(this))
        return;

    char ePKState = GetPKState();
    if (other->GetPKState() != PK_STATE_RED_NAME) {
        EPKAttr ePKAttr = m_eCurPosPKAttr;
        //------------------chg start by tony 05.06.09-----------------------------------------//
        if (g_RegionCastleWarMgr.IsCastleWarTime(this))  // 城战
        //------------------chg end   by tony 05.06.09-----------------------------------------//
        {
            if (ePKAttr == PKA_SAFE) {
                ePKAttr = PKA_NORMAL;
            }
        }

        switch (ePKAttr) {
            case PKA_NORMAL:  // 普通区域(有红名惩罚)
            {
                int pkFact = 1;
                //级别差大于30级，PK值翻倍
                if (m_core.Lev > (other->m_core.Lev + 30)) {
                    pkFact *= 2;
                }
                //相同阵营，PK值翻倍
                if (m_core.Faction == other->m_core.Faction) {
                    pkFact *= 2;
                }
                // 检查正当防卫，防止数值溢出
                if (!IsSelfDef(other) && m_core.Pk <= 200000000) {
                    m_core.Pk += g_pkConfig->iStateChangeTimeIncrease * 30 * 1000 * pkFact;
                }
                UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);

                break;
            }
                // 			case PKA_UNSAFE:     // 不安全区域(无红名惩罚)
                // 			case PKA_SAFE:       // 安全区(除了国战状态都是不允许PK的)
                // 			case PKA_SAFEST:     // 绝对安全区(新手区)
                //             case PKA_PKUNABLE:
                // 			// 没有红名计算
                // 			return;
        }
        //杀入天牢.. 这里还需要判断是否已经坐牢.. 待补充
    }

    //战功
    if (g_region->m_pWarZhulu->IsInZhuluMap(this->m_userInfo) &&
        (g_region->m_pWarZhulu->IsZhuluActive(true) ||
         g_region->m_pWarZhulu->IsZhuluActive(false))) {
        if (m_core.Faction != other->m_core.Faction) {
            AddExploit(1);
            g_region->m_pWarZhulu->OnZhuluPKOther(this->m_userInfo);
        }
        if (m_core.Faction == other->m_core.Faction)
            m_core.Pk += g_pkConfig->iStateChangeTimeIncrease * 60 * 1000;
    } else if (other->m_core.Lev > 30 && m_core.Faction != other->m_core.Faction &&
               g_pkConfig->exploitManager.iOn_Off != 0) {
        time_t tmpTime = time(NULL);
        tm     now     = *(localtime(&tmpTime));
        tm*    last    = localtime((time_t*)(&(other->m_LastDieTime)));
        if (other->m_LastDieTime == 0 ||
            last->tm_yday - now.tm_yday >= g_pkConfig->exploitManager.iDie_Interval) {
            if (now.tm_hour <= g_pkConfig->exploitManager.iEndTime &&
                now.tm_hour >= g_pkConfig->exploitManager.iBeginTime) {
                other->m_LastDieTime = tmpTime;
                CalcExploit(other);
            }
        } else {
            rt2_sprintf(g_strStaticBuffer, R(MSG_NOT_GET_ROLE_APPARATUS),
                        other->m_core.Name.c_str());
            SendSystemMessage(g_strStaticBuffer);
        }
    }

    //if(m_core.Pk >= 600 * 60 * 1000)
    //{
    //	int iRx, iRy;
    //	float fRebirthX, fRebirthY;

    //	fRebirthX = 49840;
    //	fRebirthY = -26460;

    //	SendSystemMessage(R(MSG_TRANSFER_TO_TIANLAO));

    //	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    //	packet.WriteShort(r2g_move_to);
    //	packet.WriteLong(m_userInfo->m_userId);
    //	m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);

    //	iRx += rand() % 40 - 20;
    //	iRy += rand() % 40 - 20;

    //	packet.WriteLong((long)iRx);
    //	packet.WriteLong((long)iRy);
    //	g_region->m_gws.EndSend();
    //}
}

// 已经转换为主人
void CRegionCreature::OnBePked(CRegionCreature* killer) {
    // ...

    //if (g_RegionCastleWarMgr.IsCastleWarTime(this)) // 城战
    if (((m_core.Faction == FACTION_ZHOU) && (g_RegionCastleWarMgr.IsInTimeZone("XiQi"))) ||
        ((m_core.Faction == FACTION_SHANG) && (g_RegionCastleWarMgr.IsInTimeZone("ZhaoGe")))) {
        if (g_RegionCastleWarMgr.InCastleWarArea(this)) {
            return;
        }
    }

    if (killer->IsUser()) {
        if (g_WarfieldMgr.IsInWarfield(this))
            return;
        /*m_pFriend->AddEnmity(killer->m_userInfo->m_userId,killer->m_core.Name,1);*/
        // LOG2("add black list: %s[%d]\n",killer->m_core.Name.c_str(),killer->m_userInfo->m_userId);

        char  ePKState = GetPKState();
        float expLost  = 0;

        bool pkPunish = true;
        if (((m_core.Faction == FACTION_ZHOU) && (g_RegionCastleWarMgr.IsInTimeZone("XiQi"))) ||
            ((m_core.Faction == FACTION_SHANG) && (g_RegionCastleWarMgr.IsInTimeZone("ZhaoGe")))) {
            if (g_RegionCastleWarMgr.InCastleWarArea(this))
                pkPunish = false;
        }

        if (pkPunish) {
            SPKLost* pLostRule;
            if (ePKState == PK_STATE_RED_NAME) {
                pLostRule = &(g_pkConfig->lostRed);
                //Cheney EXP全部删掉,3/30
                //static float RED_NAME_DIE_LOST_EXP=0.0f;
                //static CIniVariable Temp(GetGameRule(),"GameExpRule","RedNameDieLostExp",RED_NAME_DIE_LOST_EXP);
                //expLost = RED_NAME_DIE_LOST_EXP;
            } else if (ePKState == PK_STATE_YELLOW_NAME) {
                pLostRule = &(g_pkConfig->lostYellow);
                //static float YELLOW_NAME_DIE_LOST_EXP=0.0f;
                //static CIniVariable Temp(GetGameRule(),"GameExpRule","YellowNameDieLostExp",YELLOW_NAME_DIE_LOST_EXP);
                //expLost = YELLOW_NAME_DIE_LOST_EXP;
            } else {
                pLostRule = &(g_pkConfig->lostWhite);
            }

            int             dropMoney = 0;
            vector<SItemID> dropEquipItems;
            vector<SItemID> dropBagItems;

            int iRate;
            // money
            if (rtRandom(100, 0) < pLostRule->iMoney) {
                iRate = rtRandom(pLostRule->iMoneyMaxRate, pLostRule->iMoneyMinRate);
                // 掉iRate%的钱
                if (ePKState !=
                    PK_STATE_NORMAL)  // White name not drop money Louis @ Sat 28 Feb 2009
                    if (m_pItemContainer) {
                        dropMoney = m_pItemContainer->DropMoney(this, iRate / 100.f);
                        killer->AddGold(dropMoney);
                    }
            }

            //大幅改变PK掉落规则，Cheney 3/30

            float fPK = m_core.Pk / 1000 / 60;
            int   iBagDropRate, iEquipDropRate;
            iEquipDropRate = ((fPK / 1020) * (fPK / 1020) * 4) * 100;
            iBagDropRate   = (0.05 + (fPK * 0.987 / 1020) * 4) * 100;

            char eKillerPKState = killer->GetPKState();
            if (eKillerPKState == PK_STATE_RED_NAME) {
                iEquipDropRate += 10;
                iBagDropRate += 10;
            } else if (eKillerPKState == PK_STATE_YELLOW_NAME) {
                iEquipDropRate += 1;
                iBagDropRate += 5;
            }

            if (iEquipDropRate >= 400)
                iEquipDropRate = 400;
            if (iBagDropRate >= 400)
                iBagDropRate = 400;

            //处理包裹
            iRate = 0;
            while (iBagDropRate >= 100) {
                iRate += 1;
                iBagDropRate -= 100;
            }
            if (rtRandom(100, 0) < iBagDropRate)
                iRate += 1;
            if (m_pItemContainer)
                m_pItemContainer->DropBagItem(this, iRate, dropBagItems);

            // 处理装备
            iRate = 0;
            while (iEquipDropRate >= 100) {
                iRate += 1;
                iEquipDropRate -= 100;
            }
            if (rtRandom(100, 0) < iEquipDropRate)
                iRate += 1;
            if (m_pItemContainer)
                m_pItemContainer->DropEquipItem(this, iRate, dropEquipItems);

            char dropcount = dropBagItems.size() + dropEquipItems.size();

            if (dropcount) {
                vector<SItemID> listDropItem;

                if (dropBagItems.size()) {
                    listDropItem = dropBagItems;
                }
                if (dropEquipItems.size()) {
                    vector<SItemID>::iterator it = dropEquipItems.begin();
                    for (; it != dropEquipItems.end(); ++it) {
                        listDropItem.push_back((*it));
                    }
                }
                killer->SendPkGetItemList(listDropItem);
            }

            //Log记录
            string strBag;
            strBag += rtFormatNumber(GetItemSerializeVersion());
            strBag += ",";
            strBag += rtFormatNumber((int)(dropEquipItems.size() + dropBagItems.size()));
            strBag += ",";
            for (int i = 0; i < (int)dropEquipItems.size(); i++) {
                strBag += dropEquipItems[i].SaveToString(g_region->m_pItemManager);
            }
            for (int j = 0; j < (int)dropBagItems.size(); j++) {
                strBag += dropBagItems[j].SaveToString(g_region->m_pItemManager);
            }
            strBag += rtFormatNumber(dropMoney);

            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_log_event);
            packet.WriteByte(PLT_PKDrop);
            packet.WriteLong(m_userInfo->m_userId);
            packet.WriteString((char*)m_userInfo->m_userIpAddress.c_str());  // userIP
            packet.WriteString((char*)strBag.c_str());
            g_region->m_gws.EndSend();

            // 更新道具信息到客户端
            UpdateToClient(NET_UPDATE_ALL);
        }

        std::string       ignoreMapName = "scene01_019.rtb";
        std::string       map;
        RtsSceneBlockRtb* bmap = g_region->m_defScene->FindBlockByPos(m_pos);
        if (bmap) {
            map = bmap->m_szFileName;
        } else {
            map = "scene01_001.rtb";
        }

        //if(map != ignoreMapName)
        //{
        //	char cTmp512[512];
        //	rt2_sprintf(cTmp512, "玩家“%s”在坐标(%d,%d)被玩家“%s”杀死了，有仇报仇，有怨报怨!!!。",m_core.Name.c_str(), m_pos[0]/20,m_pos[1]/20,killer->m_core.Name.c_str());
        //	g_region->m_gws.BroadcastBulletin(cTmp512);
        //}
    }
}

void CRegionCreature::Rebirth() {
    //PZH
    //if(!m_bDead) return;
    if (GetPlayerLogic()->IsLive())
        return;

    //m_bDead = false;
    GetPlayerLogic()->ReLive();
    //
    m_hpRecover = 0;
    m_hpRemain  = 0;

    // 副本内死亡
    if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_FB) {
        m_core.SetHp(1);
        m_core.SetMp(1);
        // ForceSetPosition(m_scene->m_defPos[0],m_scene->m_defPos[1],m_scene->m_defPos[2]);
        int           tx, ty;
        float         fx = m_scene->m_defPos[0], fy = m_scene->m_defPos[1];
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_move_to);
        packet.WriteLong(m_userInfo->m_userId);
        m_scene->m_pTerrain->GetTerrainByPosFast(fx, fy, tx, ty);
        packet.WriteLong((long)tx + rand() % 10 - 5);  // 副本随机5的位置
        packet.WriteLong((long)ty + rand() % 10 - 5);
        g_region->m_gws.EndSend();
        return;
    }

    // rebirth 重生
    char              ePKState = GetPKState();
    float             fRebirthX, fRebirthY;
    RtsSceneBlockRtb* pBlockMap = (m_iCurrentBlockX < 0)
                                      ? (NULL)
                                      : (m_scene->m_pTerrain->GetBlockMap(
                                            m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                            m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY()));
    // 如果在地图中，就丢到指定的位置
    if (pBlockMap) {
        std::string szBlockName(pBlockMap->m_szFileName);
        SSceneInfo* pSceneInfo = g_TableScene.FindScene(szBlockName);
        if (pSceneInfo) {
            // 在自己的地盘或者公共的地盘死亡的
            if (pSceneInfo->sceneFaction == m_core.Faction ||
                (pSceneInfo->sceneFaction != FACTION_ZHOU &&
                 pSceneInfo->sceneFaction != FACTION_SHANG)) {
                if (ePKState == PK_STATE_RED_NAME) {
                    switch (rtRandom(5, 0)) {
                        case 0:
                            fRebirthX = pSceneInfo->fRebirthRedX1;
                            fRebirthY = pSceneInfo->fRebirthRedY1;
                            break;
                        case 1:
                            fRebirthX = pSceneInfo->fRebirthRedX2;
                            fRebirthY = pSceneInfo->fRebirthRedY2;
                            break;
                        case 2:
                            fRebirthX = pSceneInfo->fRebirthRedX3;
                            fRebirthY = pSceneInfo->fRebirthRedY3;
                            break;
                        case 3:
                            fRebirthX = pSceneInfo->fRebirthRedX4;
                            fRebirthY = pSceneInfo->fRebirthRedY4;
                            break;
                        default:
                        case 4:
                            fRebirthX = pSceneInfo->fRebirthRedX5;
                            fRebirthY = pSceneInfo->fRebirthRedY5;
                            break;
                    }
                } else {
                    switch (rtRandom(3, 0)) {
                        case 0:
                            fRebirthX = pSceneInfo->fRebirth1X;
                            fRebirthY = pSceneInfo->fRebirth1Y;
                            break;
                        case 1:
                            fRebirthX = pSceneInfo->fRebirth2X;
                            fRebirthY = pSceneInfo->fRebirth2Y;
                            break;
                        default:
                        case 2:
                            fRebirthX = pSceneInfo->fRebirth3X;
                            fRebirthY = pSceneInfo->fRebirth3Y;
                            break;
                    }
                }
            } else  // 在对方的地盘死亡的
            {
                if (ePKState == PK_STATE_RED_NAME) {
                    if (pSceneInfo->sceneFaction == FACTION_SHANG) {
                        fRebirthX = g_pkConfig->fSafeRebirthShangOtherX;
                        fRebirthY = g_pkConfig->fSafeRebirthShangOtherY;
                    } else  // pSceneInfo->sceneFaction==FACTION_ZHOU
                    {
                        fRebirthX = g_pkConfig->fSafeRebirthZhouOtherX;
                        fRebirthY = g_pkConfig->fSafeRebirthZhouOtherY;
                    }
                } else {
                    switch (rtRandom(3, 0)) {
                        case 0:
                            fRebirthX = pSceneInfo->fX_Oppose1;
                            fRebirthY = pSceneInfo->fY_Oppose1;
                            break;
                        case 1:
                            fRebirthX = pSceneInfo->fX_Oppose2;
                            fRebirthY = pSceneInfo->fY_Oppose2;
                            break;
                        default:
                        case 2:
                            fRebirthX = pSceneInfo->fX_Oppose3;
                            fRebirthY = pSceneInfo->fY_Oppose3;
                            break;
                    }
                }
            }
        } else {
            pBlockMap = NULL;
        }
    }
    // 否则丢到出生位置
    if (pBlockMap == NULL) {
        if (m_core.Faction == FACTION_ZHOU)  // 周
        {
            fRebirthX = g_pkConfig->fSafeRebirthZouX;
            fRebirthY = g_pkConfig->fSafeRebirthZouY;
        } else  // if (m_core.Faction==FACTION_SHANG) // 商
        {
            fRebirthX = g_pkConfig->fSafeRebirthShangX;
            fRebirthY = g_pkConfig->fSafeRebirthShangY;
        }
    }

    // 已经取出地点了，现在丢过去
    MSG3("玩家[%s], 重生到点[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX, fRebirthY);
    int           iRx, iRy;
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_move_to);
    packet.WriteLong(m_userInfo->m_userId);
    m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);

    iRx += rand() % 40 - 20;
    iRy += rand() % 40 - 20;

    packet.WriteLong((long)iRx);
    packet.WriteLong((long)iRy);
    g_region->m_gws.EndSend();
}

//
// add Popularity to user
// Tianh   增加声望
//
bool CRegionCreature::AddPopularity(long sV, int Popularity) {
    bool LevUp = false;
    bool isAdd = true;
    if (sV > 0) {
        if (Popularity == XIANYU) {
            if (m_core.XianyuLev >= 6) {
                return false;
            }
            m_core.Xianyu += sV;
            while (m_core.Xianyu >= g_TablePopularity.GetNextXainyuExp(m_core.XianyuLev + 1)) {
                if (m_core.XianyuLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Xianyu -= g_TablePopularity.GetNextXainyuExp(m_core.XianyuLev + 1);
                m_core.XianyuLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }
            ToPopularity(XIANYU);
        }
        if (Popularity == GUANFU) {
            if (m_core.GuanfuLev >= 6) {
                return false;
            }
            m_core.Guanfu += sV;
            while (m_core.Guanfu >= g_TablePopularity.GetNextGuanfuExp(m_core.GuanfuLev + 1)) {
                if (m_core.GuanfuLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Guanfu -= g_TablePopularity.GetNextGuanfuExp(m_core.GuanfuLev + 1);
                m_core.GuanfuLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }
            ToPopularity(GUANFU);
        }
        if (Popularity == MANZU) {
            if (m_core.ManzuLev >= 6) {
                return false;
            }
            m_core.Manzu += sV;
            while (m_core.Manzu >= g_TablePopularity.GetNextManzuExp(m_core.ManzuLev + 1)) {
                if (m_core.ManzuLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Manzu -= g_TablePopularity.GetNextManzuExp(m_core.ManzuLev + 1);
                m_core.ManzuLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }
            ToPopularity(MANZU);
        }
        if (Popularity == YISHI) {
            if (m_core.YishiLev >= 6) {
                return false;
            }
            m_core.Yishi += sV;
            while (m_core.Yishi >= g_TablePopularity.GetNextYiShiExp(m_core.YishiLev + 1)) {
                if (m_core.YishiLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Yishi -= g_TablePopularity.GetNextYiShiExp(m_core.YishiLev + 1);
                /*玩家升级前等级*/
                m_core.YishiLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }

            ToPopularity(YISHI);
        }
        if (Popularity == YIJUN) {
            if (m_core.YijunLev >= 6) {
                return false;
            }
            m_core.Yijun += sV;
            while (m_core.Yijun >= g_TablePopularity.GetNextYijunExp(m_core.YijunLev + 1)) {
                if (m_core.YijunLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Yijun -= g_TablePopularity.GetNextYijunExp(m_core.YijunLev + 1);
                /*玩家升级前等级*/
                m_core.YijunLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }
            ToPopularity(YIJUN);
        }
        if (Popularity == LIANMENG) {
            if (m_core.Lianmeng >= 6) {
                return false;
            }
            m_core.Lianmeng += sV;
            while (m_core.Lianmeng >=
                   g_TablePopularity.GetNextLianmengExp(m_core.LianmengLev + 1)) {
                if (m_core.LianmengLev >= 6)
                    break;
                // Cullevel up
                LevUp = true;
                m_core.Lianmeng -= g_TablePopularity.GetNextLianmengExp(m_core.LianmengLev + 1);
                /*玩家升级前等级*/
                m_core.LianmengLev++;
            }
            if (LevUp) {
                /*g_region->BuildEventCulLevelUp(g_sendCmd,this);*/
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
            }
            ToPopularity(LIANMENG);
        }
    }

    return true;
}

//把声望转化成字符串
bool CRegionCreature::ToPopularity(int Popularity) {
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_display);
        g_sendCmd->WriteShort(SHENGWANG);
        g_sendCmd->WriteLong(GetObjectId());
        g_sendCmd->WriteLong(Popularity);
        RecvCmd(g_sendCmd);
    }

    string str = "";
    long   a, b, c, d, e, f       = 0;
    long   a1, b1, c1, d1, e1, f1 = 0;
    if (Popularity == XIANYU) {
        a  = m_core.Xianyu;
        a1 = m_core.XianyuLev;
    }
    if (Popularity == XIANYU) {
        b  = m_core.Guanfu;
        b1 = m_core.GuanfuLev;
    }
    if (Popularity == XIANYU) {
        c  = m_core.Manzu;
        c1 = m_core.ManzuLev;
    }
    if (Popularity == XIANYU) {
        d  = m_core.Yishi;
        d1 = m_core.YishiLev;
    }
    if (Popularity == XIANYU) {
        e  = m_core.Yijun;
        e1 = m_core.YijunLev;
    }
    if (Popularity == XIANYU) {
        f  = m_core.Lianmeng;
        f1 = m_core.LianmengLev;
    }

    char Cstr[1024];
    rt2_sprintf(Cstr, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", a, a1, b, b1, c, c1, d, d1, e, e1, f,
                f1);
    m_popularity = Cstr;

    return true;
}

//解析声望字符串
bool CRegionCreature::LoadToPopularity(const char* szString) {
    string Str = szString;

    static int Pos  = 0;
    size_t     nPos = Str.find(",");
    if (nPos == string::npos && atoi(Str.c_str()) != 0) {
        vPopularity.push_back(atoi(Str.c_str()));
    } else {
        size_t nStart = 0;
        Pos           = nPos;
        while (nPos != string::npos) {

            // size_t newPos = tempStr.find(";", nPos+1);
            string newTemp = Str.substr(nStart, Pos);

            vPopularity.push_back(atoi(newTemp.c_str()));

            nStart = nPos + 1;
            nPos   = Str.find(",", nStart);

            if (nPos > Str.length()) {
                string newTemp = Str.substr(nStart, Str.length() - Pos);
                vPopularity.push_back(atoi(newTemp.c_str()));
            } else {
                Pos = nPos - Pos + 1;
            }
        }
    }
    if (!vPopularity.empty()) {
        m_core.Xianyu      = vPopularity[0];
        m_core.XianyuLev   = vPopularity[1];
        m_core.Guanfu      = vPopularity[2];
        m_core.GuanfuLev   = vPopularity[3];
        m_core.Manzu       = vPopularity[4];
        m_core.ManzuLev    = vPopularity[5];
        m_core.Yishi       = vPopularity[6];
        m_core.YishiLev    = vPopularity[7];
        m_core.Yijun       = vPopularity[8];
        m_core.YijunLev    = vPopularity[9];
        m_core.Lianmeng    = vPopularity[10];
        m_core.LianmengLev = vPopularity[11];
    } else {
        m_core.Xianyu      = 0;
        m_core.XianyuLev   = 0;
        m_core.Guanfu      = 0;
        m_core.GuanfuLev   = 0;
        m_core.Manzu       = 0;
        m_core.ManzuLev    = 0;
        m_core.Yishi       = 0;
        m_core.YishiLev    = 0;
        m_core.Yijun       = 0;
        m_core.YijunLev    = 0;
        m_core.Lianmeng    = 0;
        m_core.LianmengLev = 0;
    }

    vPopularity.clear();

    return true;
}

// add by zzh
void CRegionCreature::Rebirth(char type, int iHPrate, int iMPrate) {
    //PZH
    //if(!m_bDead) return;
    if (GetPlayerLogic()->IsLive())
        return;

    m_hpRecover = 0;
    m_hpRemain  = 0;

    // 副本内死亡
    if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_FB) {
        m_core.SetHp(1);
        m_core.SetMp(1);
        // ForceSetPosition(m_scene->m_defPos[0],m_scene->m_defPos[1],m_scene->m_defPos[2]);
        int           tx, ty;
        float         fx = m_scene->m_defPos[0], fy = m_scene->m_defPos[1];
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_move_to);
        packet.WriteLong(m_userInfo->m_userId);
        m_scene->m_pTerrain->GetTerrainByPosFast(fx, fy, tx, ty);
        packet.WriteLong((long)tx + rand() % 10 - 5);  // 副本随机5的位置
        packet.WriteLong((long)ty + rand() % 10 - 5);
        g_region->m_gws.EndSend();
        GetPlayerLogic()->ReLive();
        SetProtectTime(REBIRTH_PROTECT_MILLSECONDS);
        LeaveCombat();
        return;
    }

    // rebirth 重生
    char              ePKState = GetPKState();
    float             fRebirthX, fRebirthY;
    RtsSceneBlockRtb* pBlockMap = (m_iCurrentBlockX < 0)
                                      ? (NULL)
                                      : (m_scene->m_pTerrain->GetBlockMap(
                                            m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                            m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY()));

    switch (type) {
        case 1: {
            if (!GetItemNum(PERFECT_REBIRTH_ITEM) > 0 || !RemoveItem(PERFECT_REBIRTH_ITEM, 1))
                return;
            m_core.SetHp(m_core.GetMaxHp());
            m_core.SetMp(m_core.GetMaxMp());
        } break;
        case 2: {
            static float        DIE_BY_MONSTER_ADDED_LOST_EXP = 0.01f;
            static CIniVariable Temp(GetGameRule(), "GameExpRule", "DieByMonsterAddkedLostExp",
                                     DIE_BY_MONSTER_ADDED_LOST_EXP);
            ULONG iLostExp = g_TableExp.GetNextLevExp(m_core.Lev) * DIE_BY_MONSTER_ADDED_LOST_EXP;
            if (m_core.Exp > iLostExp)
                m_core.Exp -= iLostExp;
            else
                m_core.Exp = 0;

            ULONG iLostMoney = m_pItemContainer->GetMoney() / 10;

            if (!RemoveGold(iLostMoney))
                return;

            m_core.SetHp(m_core.GetMaxHp() / 10);
            m_core.SetMp(m_core.GetMaxMp() / 10);
        } break;
        case 3: {
            int lineGroup = 0;
            switch (m_core.RelivePostion) {
                case 1: {
                    lineGroup = 1;
                    fRebirthX = RELIVEPOSTIONX1;
                    fRebirthY = RELIVEPOSTIONY1;
                } break;
                case 2: {
                    fRebirthX = RELIVEPOSTIONX2;
                    fRebirthY = RELIVEPOSTIONY2;
                } break;
                case 3: {
                    fRebirthX = RELIVEPOSTIONX3;
                    fRebirthY = RELIVEPOSTIONY3;
                } break;
                case 4: {
                    fRebirthX = RELIVEPOSTIONX4;
                    fRebirthY = RELIVEPOSTIONY4;
                } break;
                case 5: {
                    lineGroup = 1;
                    fRebirthX = RELIVEPOSTIONX5;
                    fRebirthY = RELIVEPOSTIONY5;
                } break;
                default: {
                    m_core.RelivePostion = 1;
                    lineGroup            = 1;
                    fRebirthX            = RELIVEPOSTIONXOTHER;
                    fRebirthY            = RELIVEPOSTIONYOTHER;
                } break;
            }

            bool switchScene = true;
            int  iRx, iRy;
            iRx = fRebirthX;
            iRy = fRebirthY;
            m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);
            iRx += rand() % 10 - 5;
            iRy += rand() % 10 - 5;

            long pos[2];
            pos[0] = fRebirthX + rand() % 200 - 100;
            pos[1] = fRebirthY + rand() % 200 - 100;

            if (lineGroup) {
                MSG3("玩家[%s], 分线复活[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX,
                     fRebirthY);
                if (m_dwGroupID && m_dwLineID) {
                    switchScene = false;
                } else
                    CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(lineGroup,
                                                                       m_userInfo->m_userId, pos);
            } else {
                MSG3("玩家[%s], 大地图复活[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX,
                     fRebirthY);
                if (m_dwGroupID && m_dwLineID) {
                    CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(
                        m_dwGroupID, m_dwLineID, m_userInfo->m_userId, "DefMap", pos);
                } else
                    switchScene = false;
            }

            if (!switchScene) {
                CG_CmdPacket& packet = g_region->m_gws.BeginSend();
                packet.WriteShort(r2g_move_to);
                packet.WriteLong(m_userInfo->m_userId);
                packet.WriteLong((long)iRx);
                packet.WriteLong((long)iRy);
                g_region->m_gws.EndSend();
            }
            m_core.SetHp(m_core.GetMaxHp() / 10);
            m_core.SetMp(m_core.GetMaxMp() / 10);
        } break;
        case 4: {
            m_core.SetHp(m_core.GetMaxHp() * iHPrate / 100);
            m_core.SetMp(m_core.GetMaxMp() * iMPrate / 100);
        } break;
        default:
            return;
            break;
    }

    //m_bDead = false;
    GetPlayerLogic()->ReLive();

    SetProtectTime(REBIRTH_PROTECT_MILLSECONDS);
    LeaveCombat();

    if (type != 3) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_user_rebirth);
        g_sendCmd->WriteLong(this->GetObjectId());
        g_sendCmd->WriteByte(type);
        if (NULL != m_scene) {
            m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        }
    }
}

void CRegionCreature::UltimateChange(int sV) {
    if (sV > 0) {
        (&m_Skill)->ImmunoHarmful = true;
        m_UltimateChange          = 1;
    } else {
        (&m_Skill)->ImmunoHarmful = false;
        m_UltimateChange          = 0;
    }
    //增加攻击力
    m_core.DamageMin.Temp += sV;
    m_core.DamageMax.Temp += sV;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_Ultimate_Change);
    g_sendCmd->WriteLong(this->GetObjectId());
    g_sendCmd->SerializeBit(m_UltimateChange, 1);

    if (NULL != m_scene) {
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    }
}

//Tianh 隐身 10.02.25
void CRegionCreature::Hiding(int sV) {
    if (sV > 0) {
        m_IsHiding = 1;
        LeaveCombat();  //脱离战斗状态
    } else {
        m_IsHiding = 0;
    }

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_Hiding);
    g_sendCmd->WriteLong(this->GetObjectId());
    g_sendCmd->SerializeBit(m_IsHiding, 1);

    if (NULL != m_scene) {
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    }
}

//Tian 遁地
void CRegionCreature::PlayerLurk(int sV) {
    if (sV > 0) {
        m_IsHiding = 2;
        LeaveCombat();  //脱离战斗状态
    } else {
        m_IsHiding = 0;
    }

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_Lurk);
    g_sendCmd->WriteLong(this->GetObjectId());
    g_sendCmd->SerializeBit(m_IsHiding, 1);

    if (NULL != m_scene) {
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    }
}

//
//Tianh  10.03.18 增加灵力值   和杀死怪物的等级 以及玩家等级 RMB道具有关系
//
void CRegionCreature::AddSoul(int MonsterLev) {
    //灵力值的最大存储上限
    float MaxSoul = m_core.GetSoulValueMax();
    if (m_core.Soul >= MaxSoul) {
        m_core.Soul = MaxSoul;
    } else {
        //无衰减
        float NoCollapseSoul = 1 + 0.25 * (MonsterLev - 1) * (1 + 0.05 * (MonsterLev - 1));
        //有衰减
        float CollapseSoul = (1 - Abs(m_core.Lev - MonsterLev) * 0.1) * NoCollapseSoul;
        //最终灵力获得
        float WinSoul = Max(0.0f, (CollapseSoul) * (1 + (m_core.GradeLev / 20 + 1) / 3));

        ////
        //添加RMB道具的作用  未实现
        ////
        long vSoul = 0;

        if (0 != (long)(WinSoul + 0.50))  //这里加0.50 实现一个4舍5入的效果
        {
            //发送系统消息
            rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_SOUL), (long)WinSoul);
            SendBattleInfoMessage(g_strStaticBuffer);

            vSoul = (long)(WinSoul + 0.50);

            if ((m_core.Soul + vSoul) > MaxSoul) {
                vSoul = MaxSoul - m_core.Soul;
            }

            {
                if (vSoul > 0) {
                    m_core.Soul += vSoul;

                    g_sendCmd->BeginWrite();
                    g_sendCmd->WriteShort(r2c_display);
                    g_sendCmd->WriteShort(LINGLI);
                    g_sendCmd->WriteLong(GetObjectId());
                    g_sendCmd->WriteLong(vSoul);
                    RecvCmd(g_sendCmd);
                }
            }
        }
    }
}

//
// 死亡流程
// Dead -> OnDead(killer不为空） -> OnPkOther -> OnBePked -> Kill monster reward
//
// 物体死亡，只调用自己的死亡，killer可以是空
//
long CRegionCreature::Die(CRegionCreature* killer) {
    long nRet;
    if (IsMonster()) {
        if (killer && killer->IsUserPet()) {
            //杀死怪物获得灵力值
            if (killer->m_master) {
                killer->m_master->AddSoul(m_core.Lev);
            }
        }
        //如果杀死怪物的是玩家
        if (killer && killer->IsUser()) {
            //杀死怪物获得灵力值
            killer->AddSoul(m_core.Lev);

            //-----------发送系统新闻 ------------------  Tianh
            if (true == m_npcInfo->isPrompt) {
                RtsSceneBlockRtb* pMap   = m_scene->FindBlockByPos(m_pos);
                string            strone = pMap->m_szBlockName;
                string            str    = m_npcInfo->Name;
                string            str1   = killer->m_core.Name.c_str();
                SItemID           ItemID;

                const char* playername = str1.c_str();
                const char* Name       = "";
                const char* BossName   = str.c_str();
                const char* SceneName  = strone.c_str();
                const char* ItemName   = "";
                this->Systembroadcast(playername, Name, BossName, SceneName, BRATBOSS, ItemName);
            }
        }

        nRet = CPlayerLogic::eDead;

        //不是旅行者时，有1/3几率说出遗言，如果有定义遗言
        if (m_ai && m_ai->GetType() != CRegionAI::TYPE_TOUR && rand() % 5 < 1)
            m_ai->TalkOver();
    } else {
        nRet = CPlayerLogic::eHaveItem;
    }
    do {
        if (m_flag & OBJECT_FLAG_DESTROY) {
            break;
        }
        //PZH
        //if(m_bDead) return;

        //m_bDead = true;
        if (!GetPlayerLogic()->IsLive()) {
            break;
        }
        GetPlayerLogic()->Die();
        //

        m_bScapegoatRemovedDuringDead = false;

        if (killer) {
            m_scene->OnCreatureDead(this, killer);
            nRet = OnDead(killer);
        }
        if (IsUser()) {
            //
            //-----------发送系统新闻 ------------------  Tianh
            //
            //得到玩家场景
            if (killer && killer->IsUser()) {
                RtsSceneBlockRtb* pMap   = m_scene->FindBlockByPos(m_pos);
                string            strone = pMap->m_szBlockName;

                string str  = killer->m_core.Name.c_str();
                string str1 = m_core.Name.c_str();

                const char* playername = killer->m_core.Name.c_str();
                const char* Name       = str1.c_str();
                const char* BossName   = "";
                const char* SceneName  = strone.c_str();
                SItemID     ItemID;
                const char* ItemName = "";
                this->Systembroadcast(playername, Name, BossName, SceneName, FIELDKILLPERSON,
                                      ItemName);
            }
            //-----------发送系统新闻 ------------------  Tianh

            int mt = m_task.IsDoingMoneyTask();
            if (mt) {
                CancelTask(mt, false);
            }

            // exp lost //Cheney,这里全部删掉。
            //    if (!m_bScapegoatRemovedDuringDead)
            //    {
            //        if (GetItemNum(SCAPEGOAT_ITEM) > 0)
            //        {
            //            RemoveItem(SCAPEGOAT_ITEM, 1);
            //            m_bScapegoatRemovedDuringDead = true;
            //        }
            //        else
            //        {
            //static float DIE_BY_MONSTER_ADDED_LOST_EXP=0.0f;
            //static CIniVariable Temp(GetGameRule(),"GameExpRule","DieByMonsterAddedLostExp",DIE_BY_MONSTER_ADDED_LOST_EXP);
            //ULONG iLostExp = g_TableExp.GetNextLevExp(m_core.Lev)*DIE_BY_MONSTER_ADDED_LOST_EXP;
            //if (m_core.Exp>iLostExp)    m_core.Exp -= iLostExp;
            //else                        m_core.Exp = 0;
            //        }
            //    }

            // 告诉玩家说他已经死了
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_you_are_dead);
            if (killer == 0) {
                g_sendCmd->WriteByte(0);
            } else {
                if (killer->IsUser())
                    g_sendCmd->WriteByte(1);
                else
                    g_sendCmd->WriteByte(2);
                g_sendCmd->WriteString((char*)killer->m_core.Name.c_str());
            }
            RecvCmd(g_sendCmd);

            /*
			CCreatureState *state = RT_NEW CCreState_Dead;
			SCreStateParam param;
			if(!AddState(state,&param))
			{
			delete state;
			}
			*/
            //-------------------------------add start by tony 05.06.09------------------//
            if ((g_RegionCastleWarMgr.IsCastleWarTime("XiQi") &&
                 (m_core.Faction == FACTION_ZHOU)) ||
                (g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe") &&
                 (m_core.Faction == FACTION_SHANG))) {
                //if(g_RegionCastleWarMgr.FindByUnionID(m_unionID) != NULL)
                if (g_RegionCastleWarMgr.InCastleWarArea(this)) {
                    g_RegionCastleWarMgr.Rebirth(this);
                    break;
                }
            }
            //-------------------------------add end   by tony 05.06.09------------------//
            //战场内回城
            if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_WARFIELD) {
                g_WarfieldMgr.OnUserDead(this);
                break;
            }

            //Rebirth();
        } else if (IsUserPet()) {
            // 在主人定时器中处理，不要在这里处理

        } else if (IsMonster()) {
            //城战相关
            if (m_npcInfo != NULL) {
                if (g_RegionCastleWarMgr.IsGuardAnimal(m_npcInfo->Id)) {
                    g_RegionCastleWarMgr.KillAnimalNpc(killer, this);
                    break;
                }

                if (g_RegionCastleWarMgr.IsFlagNpc(m_npcInfo->Id)) {
                    g_RegionCastleWarMgr.KillFlagNpc(this, true);
                    break;
                }
            }
            CallSpirit();

            // 怪物或npc
            if (m_master) {
                m_master->SetWarder(NULL);
                if (m_master->IsUser() && m_master->IsScriptLoaded() && m_npcInfo) {
                    char sFuncName[20] = {0};
                    rt2_snprintf(sFuncName, 20, "WardNPCDie%d", m_npcInfo->Id);
                    m_master->CallScript(sFuncName, false);
                    m_master = NULL;
                }
                //---------add start by tony 05.08.09 ai modify---------------//
                if (m_master->m_ai) {
                    m_master->m_ai->OnUnderlingDead(this);
                }
                //---------add end   by tony 05.08.09 ai modify---------------//

                m_master->m_pet      = NULL;
                m_master->m_bRidePet = false;
                //m_scene->RemoveCreature(m_oId); //PZH 怪物死后是变成尸体状，而不是马上消失
                //g_factory->DestroyObject(this);

                //怪物死亡增加灵力
            } else {
                if (m_pet) {
                    m_scene->RemoveCreature(m_pet->m_oId);
                    m_dwPetSave = 0;
                }
                //m_scene->RemoveCreature(m_oId); //PZH 怪物死后是变成尸体状，而不是马上消失

                // notify monster lair
                if (m_lair) {
                    m_lair->LastSpawnTime = rtGetMilliseconds();
                    m_lair->Monster       = NULL;
                    m_lair                = NULL;
                }

                // destroy
                //g_factory->DestroyObject(this); //PZH 怪物死后是变成尸体状，而不是马上消失
                if (m_pet) {
                    g_factory->DestroyObject(m_pet);
                    m_pet = NULL;
                }
            }
        }
        //-------------------------------add start by tony 05.05.18------------------//
        else if (IsGuarder()) {
            if (m_npcInfo && g_WarfieldMgr.IsRebirthNpc(m_npcInfo->Id)) {
                g_WarfieldMgr.OnRebirthPosKilled(this, killer);
                break;
            }

            if (m_npcInfo && g_WarfieldMgr.IsFlagNpc(m_npcInfo->Id)) {
                g_WarfieldMgr.OnFlagIsDestroy(m_npcInfo->Id);
                break;
            }
            //modify later
            //...
#ifdef _DEBUG
            LOG1("The guarder [%s] is dead.\n", m_core.Name.c_str());
#endif  //_DEBUG                                                                                   \
    // destroy
            m_scene->RemoveCreature(m_oId);
            g_factory->DestroyObject(this);
        }
        if (IsUser() && m_IsAddMonster)  //tim.yang  GD
            m_bIsDead = true;
        //-------------------------------add end   by tony 05.05.18------------------//
    } while (false);

    if (killer && killer->m_targets.empty() && killer->m_enemyList.empty())
        killer->LeaveCombat();
    return nRet;
}

// C [12/9/2008 Louis.Huang]
//
// bool CRegionCreature::TestSetPosition(long x,long y,long z)
// {
// 	return m_scene->TestAddCreature(x,y,z);
// }

void CRegionCreature::MeDie(CRegionCreature* killer) {
    if (killer && killer != this && m_pPlayerLogic) {
        SetTarget(killer);
        m_core.SetHp(0);
        m_pPlayerLogic->SetLiveType((CPlayerLogic::LiveType)Die(killer));
        g_region->BuildEventDead(g_sendCmd, this);
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    }
}

int CRegionCreature::CheckCanMove(int page, int gridI, int gridJ, SItemID item) {
    // 催粮任务期间无法使用回城
    if (IsDoingMoneyTask()) {
        SendSystemMessage(R(MSG_TASK_CANNOT_USE_SCROLL));
        return 1;
    }

    //天牢中无法使用传送
    int bx, by;
    m_scene->m_pTerrain->GetBlockByPos((float)m_pos[0], (float)m_pos[1], bx, by);
    RtsSceneBlockRtb* pBlockMap = 0;
    if (bx >= 0 && bx < m_scene->m_pTerrain->GetBlockCntX() && by >= 0 &&
        by < m_scene->m_pTerrain->GetBlockCntY()) {
        pBlockMap = m_scene->m_pTerrain->GetBlockMap(bx - m_scene->m_pTerrain->GetCenterX(),
                                                     by - m_scene->m_pTerrain->GetCenterY());
    }
    if (pBlockMap && !strcmp(pBlockMap->m_szFileName, "scene01_117.rtb")) {
        SendSystemMessage(R(MSG_INTIANLAO_CANNOT_USE_TELEPORT));
        return 1;
    }

    // 删除道具
    if (ItemCanPile(item.type)) {
        if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
            return 1;
        CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
        RecvCmd(g_sendCmd);
    } else {
        m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
        CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI,
                                                      gridJ, ITEM_REASON_DELETE);
        RecvCmd(g_sendCmd);
    }

    // 副本内回城
    if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_FB) {
        if (!m_userInfo)
            return 1;
        g_region->m_fbMgr->DungeonLeaveByItem(m_userInfo);
        //g_region->m_fbMgr->RequestLeave(m_userInfo);
        return 2;
    }

    //战场内回城
    if (OB_TYPE(m_scene) == OBJECT_TYPE_SCENE_WARFIELD) {
        if (!m_userInfo)
            return 1;
        g_WarfieldMgr.RequestLeave(m_userInfo);
        return 2;
    }
    return 0;
}

bool CRegionCreature::SetPosition(long x, long y, long z) {
    // 先看看tile有没有变化
    int tx, ty;
    int txCurr, tyCurr;

    if (!m_scene || !m_scene->m_pTerrain)
        return false;

    m_scene->m_pTerrain->GetTerrainTile(x, y, tx, ty);
    m_scene->m_pTerrain->GetTerrainTile(m_pos[0], m_pos[1], txCurr, tyCurr);
    if (tx != txCurr || ty != tyCurr) {
        //tim.yang  会员功能
        //if(m_pHuiYuanHoptoad)
        //{
        //	RemoveHuiYuanHoptoad();
        //}
        //end
        if (!m_scene->MoveCreature(this, x, y, z))
            return false;
        //PZH
        //if(GetPlayerLogic()->IsGetingSkin())//如果正在剥皮，移动了就是取消剥皮
        //{
        //	GetPlayerLogic()->SendGetSkinError(8);
        //	GetPlayerLogic()->CaneclGetingSkin();
        //}
        //
    }

    m_pos[0] = x;
    m_pos[1] = y;
    m_pos[2] = z;

    if (IsRidePet()) {
        m_scene->MoveCreature(m_pet, x, y, z);
        m_pet->m_pos[0] = x;
        m_pet->m_pos[1] = y;
        m_pet->m_pos[2] = z;
    }

    int bx, by;

    m_scene->m_pTerrain->GetBlockByPos((float)x, (float)y, bx, by);
    if (bx >= 0 && bx < m_scene->m_pTerrain->GetBlockCntX() && by >= 0 &&
        by < m_scene->m_pTerrain->GetBlockCntY()) {
        if (bx != m_iCurrentBlockX || by != m_iCurrentBlockY) {
            RtsSceneBlockRtb* pBlockMap = m_scene->m_pTerrain->GetBlockMap(
                bx - m_scene->m_pTerrain->GetCenterX(), by - m_scene->m_pTerrain->GetCenterY());
            RtsSceneBlockRtb* pOldBlockMap =
                (m_iCurrentBlockX < 0) ? (NULL)
                                       : (m_scene->m_pTerrain->GetBlockMap(
                                             m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                             m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY()));
            m_iCurrentBlockX = bx;
            m_iCurrentBlockY = by;
            OnBlockChanged(pBlockMap, pOldBlockMap);
        }
        OnPositionChanged((float)x, (float)y);
    }

    // set titl
    // ...
    return true;
}

bool CRegionCreature::ForceSetPosition(long x, long y, long z, bool bBroadcast /* = false*/) {
    /*
	long finX,finY,finZ;
	if(bFindSpace)
	{
		int ix,iy,gx,gy;
		float fx,fy;
		m_scene->m_pTerrain->GetTerrainTile(x, y, ix, iy);
		m_scene->m_pPathFind->FindSpace(ix,iy,gx,gy);
		if(gx!=ix || gy!=iy)
		{
			m_scene->m_pTerrain->GetTerrainPos(gx,gy,)
		}
	}
	*/

    if (SetPosition(x, y, z)) {
        long lBroadcast = m_bBroadcastTele;  // 用来控制客户端动作触发瞬移
        m_posFlag++;
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_force_position);
        g_sendCmd->WriteLong(m_oId);
        g_sendCmd->WriteByte(m_posFlag);
        // for fb system
        // ...
        g_sendCmd->WriteString((char*)m_scene->m_sceneName.c_str());
        g_sendCmd->WriteLong(x);
        g_sendCmd->WriteLong(y);
        g_sendCmd->SerializeBit(lBroadcast, 1);

        if (bBroadcast) {
            if (m_master) {
                if (m_master->m_scene) {
                    m_master->m_scene->BroadcastCmd(m_master->m_pos, DEFAULT_RANGE, g_sendCmd,
                                                    NULL);
                } else {
                    m_master->RecvCmd(g_sendCmd);
                }
            } else {
                if (m_scene) {
                    m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
                } else {
                    RecvCmd(g_sendCmd);
                }
            }
        } else {
            if (m_master)
                m_master->RecvCmd(g_sendCmd);
            else
                RecvCmd(g_sendCmd);
        }

        //--------debug-----------//
        int tx, ty;
        m_scene->m_pTerrain->GetTerrainTile(m_pos[0], m_pos[1], tx, ty);
        if ((m_userInfo != NULL)) {
            float fX2, fY2;
            fX2      = m_pos[0] - x;
            fY2      = m_pos[1] - y;
            float fD = sqrt(fX2 * fX2 + fY2 * fY2);

            if ((long)fD > 100) {
                /*
				LOG1("-UserName: [%s]\n", (char*)m_userInfo->m_username.c_str());
				LOG2("-Orginal pos: ([%d], [%d])\n", ix, iy);
				LOG2("-Final pos: ([%d], [%d])\n", tx, ty);
				LOG("-ForceSetPosition Failed, but return true\n\n");
				*/
            }
        }
        //--------debug-----------//

        return true;
    }
    return false;
}

void CRegionCreature::OnPositionChanged(float fX, float fY) {
    // 区域进入和离开事件
    int                             i, iCnt;
    static RtsSceneBlockRtb::SArea* s_pArea[10];  // 一次最多10个块
    RtsSceneBlockRtb::SArea*        pArea = m_scene->m_pTerrain->FindFirstArea(fX, fY);
    for (i = 0; i < 9 && pArea; i++) {
        s_pArea[i] = pArea;
        pArea      = m_scene->m_pTerrain->FindNextArea();
    }
    std::list<RtsSceneBlockRtb::SArea*>::iterator it, itNext;
    iCnt = i;
    for (it = m_listArea.begin(); it != m_listArea.end(); it = itNext) {
        itNext = it;
        itNext++;
        for (i = 0; i < iCnt; i++) {
            if (s_pArea[i] && (*it) == s_pArea[i]) {
                s_pArea[i] = NULL;
                break;
            }
        }
        if (i == iCnt) {
            OnAreaExit((*it), NULL);
            m_listArea.erase(it);
        }
    }
    for (i = 0; i < iCnt; i++) {
        if (s_pArea[i]) {
            m_listArea.push_back(s_pArea[i]);
            OnAreaEnter(s_pArea[i], NULL);
        }
    }
    UpdateAreaPKAttr();
}

void CRegionCreature::OnRemoveCreature() {
    if (m_pathNode) {
        if (m_scene && m_scene->m_pPathFind) {
            m_scene->m_pPathFind->RemoveActor(m_pathNode);
        }
    }

    RtsSceneBlockRtb* pBlockMap =
        m_scene->m_pTerrain->GetBlockMap(m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                         m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY());
    OnBlockChanged(NULL, pBlockMap);

    m_iCurrentBlockX = -1;
    m_iCurrentBlockY = -1;

    std::list<RtsSceneBlockRtb::SArea*>::iterator it, itNext;
    for (it = m_listArea.begin(); it != m_listArea.end(); it = itNext) {
        itNext = it;
        itNext++;
        OnAreaExit((*it), NULL);
        m_listArea.erase(it);
    }
    m_listArea.clear();

    if (IsUser() && time(NULL) - m_MonitorLastTime > 60) {
        // 记录到数据库
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_log_event);
        packet.WriteByte(PLT_Monitor);
        char cTmp1024[1024];
        rt2_snprintf(cTmp1024, 1023, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                     m_nGetExp, m_nGetItemCount, m_nGetMoneyFromMonster, m_nGetMoneyFromTask,
                     m_nMonsterCount, m_core.AttSpeed.GetValue(), m_core.AttackRange.GetValue(),
                     m_core.DamageMin.GetValue(), m_core.DamageMax.GetValue(),
                     m_core.Attack.GetValue(), m_core.Dodge.GetValue(), m_core.Armor.GetValue(),
                     m_pItemContainer->GetMoney(), m_pBank->GetMoney(), m_core.Lev, m_core.Exp,
                     m_core.Credit);
        cTmp1024[1023] = 0;
        packet.WriteLong(m_userInfo->m_userId);
        packet.WriteString((char*)m_userInfo->m_userIpAddress.c_str());  // userIP
        packet.WriteString(cTmp1024);                                    // money
        packet.WriteLong(time(NULL) - m_MonitorLastTime);                // usetime
        g_region->m_gws.EndSend();

        m_nGetMoneyFromTask    = 0;
        m_nGetMoneyFromMonster = 0;
        m_nGetExp              = 0;
        m_nGetItemCount        = 0;
        m_nMonsterCount        = 0;
        m_MonitorLastTime      = time(NULL);
    }
}

bool CRegionCreature::SetModel(short Id) {
    SCreModel* model = g_TableCreModel.FindCreModel(Id);
    if (!model)
        return NULL;

    m_model = model;
    if (!IsMonster()) {
        m_moveSpeed   = model->MoveSpeed;
        m_attackRange = model->AttackRange;
    }
    m_core.ModelId      = model->Id;
    m_core.ComplexModel = model->bComplex;
    return true;
}

bool CRegionCreature::AddWeapon(short Id) {
    //	SItem *item = RS_FindItem(Id);
    //	if(!item) return false;

    //	if(item->Type != ITEM_TYPE_WEAPON)
    //		return false;

    //	m_core.Weapon = Id;

    if (m_pItemContainer) {
        SItemID item = ItemID_CreateInvalid();
        m_pItemContainer->Equip(item, CItemContainerBase::WEAPON_1);
    }
    return true;
}

bool CRegionCreature::WriteCreateInfo(CG_CmdPacket* cmd) {
    cmd->WriteLong(m_oId);
    cmd->WriteShort(m_core.ModelId);

    // for fb system
    // ...
    // cmd->WriteString("scene01");
    cmd->WriteString((char*)m_scene->m_sceneName.c_str());
    cmd->WriteLong(m_pos[0]);
    cmd->WriteLong(m_pos[1]);
    cmd->WriteLong(m_pos[2]);

    // write item

    // write union
    cmd->WriteLong(m_unionID);
    cmd->WriteString((char*)m_unionName.c_str());
    cmd->WriteString((char*)m_unionNickName.c_str());
    cmd->WriteString((char*)m_unionIcon.c_str());

    // write skill info
    m_Skill.NetWriteSkill(g_sendCmd);

    cmd->WriteByte(m_core.Faction);
    //PZH
    long ln;
    if (GetPlayerLogic()->IsTeamFree()) {
        ln = 1;
    } else {
        ln = 0;
    }
    cmd->SerializeBit(ln, 1);
    //
    if (m_pet) {
        cmd->WriteLong(m_pet->m_oId);
        cmd->WriteShort(m_pet->m_core.ModelId);
        cmd->WriteLong(m_pet->m_pos[0]);
        cmd->WriteLong(m_pet->m_pos[1]);
        cmd->WriteLong(m_pet->m_pos[2]);
        // write skill info
        m_pet->m_Skill.NetWriteSkill(g_sendCmd);
    } else {
        cmd->WriteLong(0);
        cmd->WriteShort(0);
        cmd->WriteLong(0);
        cmd->WriteLong(0);
        cmd->WriteLong(0);
        // write skill info
        cmd->WriteByte(0);
    }
    return true;
}

bool CRegionCreature::WriteModelInfo(CG_CmdPacket* cmd) {
    SItemID item;

    cmd->WriteLong(m_oId);
    cmd->WriteString((char*)m_core.Name.c_str());
    cmd->WriteShort(m_core.ModelId);
    cmd->WriteShort(m_core.HeadModelId);
    cmd->WriteByte(m_modelRef);
    long lShapeshift = m_ChangeModelID == 0 ? 0 : 1;
    cmd->SerializeBit(lShapeshift, 1);
    if (m_pItemContainer)
        m_pItemContainer->Serialize(NET_UPDATE_ITEM_SHOW, cmd);
    cmd->WriteByte(m_core.Metier);
    cmd->WriteByte(m_core.Sex);
    cmd->WriteByte(m_bPShop);
    if (m_pPShop)
        cmd->WriteString((char*)m_pPShop->GetName().c_str());
    else
        cmd->WriteString("");
    cmd->WriteLong(m_unionID);
    cmd->WriteString((char*)m_unionName.c_str());
    cmd->WriteString((char*)m_unionNickName.c_str());
    cmd->WriteString((char*)m_unionIcon.c_str());
    cmd->WriteString((char*)m_feod_name.c_str());
    cmd->WriteByte(m_is_attacker);
    cmd->WriteLong(m_dwState);

    return true;
}

void CRegionCreature::ResetCheaterCheck() {
    //LOG1("reset cheater check,speed = %d\n",m_core.MovSpeed.GetValue());

    //<add by fox>
    static int          PLAYER_CHECK_INTERVAL    = 10;   //检查的时间间隔[单位：秒]
    static float        PLAYER_SPEED_CHECK_RANGE = 1.4;  //速度检查的容错范围
    static CIniVariable SpeedCheckInterval(GetGameRule(), "GameCheck", "PlayerCheckInterval",
                                           PLAYER_CHECK_INTERVAL);
    static CIniVariable PlayerSpeedCheckRange(GetGameRule(), "GameCheck", "PlayerSpeedCheckRange",
                                              PLAYER_SPEED_CHECK_RANGE);
    //</add by fox>

    m_mvChk.Init(m_core.MovSpeed.GetValue() * PLAYER_CHECK_INTERVAL * PLAYER_SPEED_CHECK_RANGE,
                 m_core.MovSpeed.GetValue());
    m_atChk.Init(PLAYER_CHECK_INTERVAL * 0.15);
}

void CRegionCreature::OnTimer(DWORD id, DWORD dwParam[]) {
    /*
	if(IsUser())
		LOG1("user timer called id = %d",id);
	*/
    CRegionObject::OnTimer(id, dwParam);
    long RestoreHp  = m_core.RestoreHp.Base;
    long RestoreMp  = m_core.RestoreMp.Base;
    long RestoreEnp = m_core.RestoreEnP.Base;

    switch (id) {
        case CRE_TIMER_MONITOR:
            if (IsUser() && m_userInfo->m_active && m_userInfo->Session()) {
                // 记录到数据库
                CG_CmdPacket& packet = g_region->m_gws.BeginSend();
                packet.WriteShort(r2g_log_event);
                packet.WriteByte(PLT_Monitor);
                char cTmp1024[1024];
                rt2_snprintf(cTmp1024, 1023, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                             m_nGetExp, m_nGetItemCount, m_nGetMoneyFromMonster,
                             m_nGetMoneyFromTask, m_nMonsterCount, m_core.AttSpeed.GetValue(),
                             m_core.AttackRange.GetValue(), m_core.DamageMin.GetValue(),
                             m_core.DamageMax.GetValue(), m_core.Attack.GetValue(),
                             m_core.Dodge.GetValue(), m_core.Armor.GetValue(),
                             m_pItemContainer->GetMoney(), m_pBank->GetMoney(), m_core.Lev,
                             m_core.Exp, m_core.Credit);
                cTmp1024[1023] = 0;
                packet.WriteLong(m_userInfo->m_userId);
                packet.WriteString((char*)m_userInfo->m_userIpAddress.c_str());  // userIP
                packet.WriteString(cTmp1024);                                    // money
                packet.WriteLong(PLAYER_MONITOR_CYCLE);                          // usetime
                g_region->m_gws.EndSend();

                m_nGetMoneyFromTask    = 0;
                m_nGetMoneyFromMonster = 0;
                m_nGetExp              = 0;
                m_nGetItemCount        = 0;
                m_nMonsterCount        = 0;
                m_MonitorLastTime      = time(NULL);
            }
            break;
        case CRE_TIMER_HPANDMP:
            if (IsUser() && /*PZH*/ /*!m_bDead*/ GetPlayerLogic()->IsLive()) {
                AddHp(RestoreHp);
                m_core.AddMp(RestoreMp);
                if (m_pet) {
                    int RestorPetHP = int(m_pet->m_core.Lev / 10 + 1) * 5;
                    int RestorPetMP = 3;
                    m_pet->AddHp(RestorPetHP);
                    m_pet->m_core.AddMp(RestorPetMP);
                }
            }
            break;
        case CRE_TIMER_TIME: {
            if (IsUser()) {
                AddPlayerOnlineTime(5);  //分钟
            }
        } break;
        case CRE_TIMER_ENP:
            if (IsUser() && GetPlayerLogic()->IsLive()) {
                m_core.AddEnp(RestoreEnp);
            }
            break;
        case CRE_TIMER_TIME2: {
            m_core.nLoginNowTime += 1;     //1分钟
            if (m_core.nLoginNowTime > 1)  //Tianh 上线保护为1分钟
            {
                m_bTime              = false;
                m_core.nLoginNowTime = 0;
            }
        } break;
        case CRE_TIMER_CHECK_CARD:
            if (IsUser()) {
                if (m_usingCard) {
                    m_cardTime -= (CHECK_CARD_INTERVAL / 1000);
                    if (m_cardTime <= 0) {
                        m_cardTime = 0;
                        EndUseCard();
                    }
                } else {
                    /*
				m_cardTime += CHECK_CARD_INTERVAL/1000/24;
				if(m_cardTime < 0) m_cardTime = 0;
				if(m_cardTime > 7*3600) m_cardTime = 7*3600;
				*/
                }
            }
            break;

        case CRE_TIMER_PRENTICE:
            if (IsUser()) {
                g_PrenticeMgr.QueryPrenticesOnlineTime(m_userInfo);
            }
            break;

        case CRE_TIMER_CHEATER_CHECK:
            if (IsUser()) {
                //static int i=0;
                //rt2_sprintf(g_strStaticBuffer, "第%d次检查", i++);
                //this->SendSystemMessage(g_strStaticBuffer);

                // 避免骑兽时使用加速软件
                if (m_pet)
                    m_pet->ResetCheaterCheck();

                ResetCheaterCheck();
                TickTask(0);
                // 				if(!m_bFunKick && !g_region->IsFunActive(true) && !g_region->IsFunActive(false)
                // 					&& g_region->IsInFunMap(m_userInfo))
                // 				{
                // 					g_region->KickFunUser(m_userInfo);
                // 					m_bFunKick = true;
                // 				}
                if (ShouldKickCheat())
                    g_region->KickUser(m_userInfo->m_userId, 0, true, true,
                                       WG_INFO_SHOULD_KICK_CHEATER);
            }
            RemoveTimeoutSelfDef();
            // LOG2("user att speed = %d, mov speed = %d",m_core.AttSpeed.GetValue(),m_core.MovSpeed.GetValue());
            break;

        case CRE_TIMER_KILL_SELF:
            if (m_npcInfo) {
                if (IsSpirit()) {
                    if (m_dwCatchMe) {
                        CRegionCreature* cre = m_scene->FindCreature(m_dwCatchMe);
                        if (cre)
                            cre->StopCatchSpirit(PET_CATCH_ERROR_LIFETIME);
                    }
                    GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
                    g_region->BuildEventDead(g_sendCmd, this);
                }
                DeleteAI();
                m_scene->RemoveCreature(m_oId);
                g_factory->DestroyObject(this);
            }
            StopTimer(CRE_TIMER_KILL_SELF);
            break;

        case CRE_TIMER_DIE_SELF:
            if (m_npcInfo && dwParam)
                MeDie(m_scene->FindCreature(dwParam[0]));
            StopTimer(CRE_TIMER_DIE_SELF);
            break;

        case CRE_TIMER_CHECK_ITEM_VALID:
            if (m_pItemContainer) {
                m_pItemContainer->OnCreatureAttribChanged();
            }
            break;
        case CRE_TIMER_STGTTIME: {
            if (m_SkillhpRecover != 0) {
                AddHp(m_SkillhpRecover);
            }
            if (m_SkillmpRecover != 0) {
                m_core.AddMp(m_SkillmpRecover);
            }
        } break;
        case CRE_TIMER_HEARTBEAT:
            if (m_hpRemain > 0 && /*PZH*/ /*!m_bDead*/ GetPlayerLogic()->IsLive()) {
                AddHp(m_hpRecover);
                m_hpRemain--;
            }
            if (m_mpRemain > 0) {
                m_core.AddMp(m_mpRecover);
                m_mpRemain--;
            }
            if (m_EnpRemain > 0) {
                m_core.AddEnp(m_EnpRecover);
                m_EnpRemain--;
            }
            if (IsScriptLoaded())
                CallScript("OnHeartbeat", true);

            m_hpAutoRecover += m_hpAutoRecoverRate * HEARTBEAT_INTERVAL / 1000;
            m_mpAutoRecover += m_mpAutoRecoverRate * HEARTBEAT_INTERVAL / 1000;

            if (/*PZH*/ /*!m_bDead*/ GetPlayerLogic()->IsLive()) {
                int add;
                add = m_hpAutoRecover;
                m_hpAutoRecover -= add;
                AddHp(add);

                add = m_mpAutoRecover;
                m_mpAutoRecover -= add;
                m_core.AddMp(add);
            }

            //用户、NPC都需要更新状态
            TickState(HEARTBEAT_INTERVAL);
            if (IsUser()) {
                // UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
                UpdateToClientDelta();
                if (m_pTeam != NULL) {
                    m_pTeam->DateChangeAffectTeam();
                }
                // 收回宠物，不要在宠物死亡函数里调用回收代码，因为整个调用流程比较复杂
                if (m_pet) {
                    if (m_pet->m_core.GetHp() <= 0 ||
                        /*PZH*/ /*m_pet->m_bDead*/ !m_pet->GetPlayerLogic()->IsLive()) {
                        m_pet->m_core.SetHp(1);
                        if (IsRidePet())
                            RideOffPet();
                        CallbackPet(false);

                        //发送系统消息
                        rt2_sprintf(g_strStaticBuffer, R(MSG_PET_DIE));
                        SendSystemMessage(g_strStaticBuffer);
                    }
                }
                if (CallNpc.size() != 0) {
                    std::vector<CRegionCreature*>::iterator it = CallNpc.begin();
                    for (; it != CallNpc.end();) {
                        if ((*it)->m_core.GetHp() <= 0 || !(*it)->GetPlayerLogic()->IsLive()) {
                            (*it)->DeleteAI();
                            m_scene->RemoveCreature((*it)->m_oId);
                            g_factory->DestroyObject((*it));
                            (*it) = NULL;
                            it    = CallNpc.erase(it);

                            SendMagCallNpc();
                        } else {
                            ++it;
                        }
                    }
                }
            };

            if (IsUserPet()) {
                UpdateToClient(NET_CRE_UPDATE_PET_INFO);
            } else if (IsCallNpc()) {
                UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);
            }

            {
                unsigned long dwNow = rtGetMilliseconds();
                unsigned long dwDiff;
                if (m_Skill.m_dwLastRunSkill == 0) {
                    dwDiff = 0;
                } else {
                    dwDiff = dwNow - m_Skill.m_dwLastRunSkill;
                }
                m_Skill.m_dwLastRunSkill    = dwNow;
                m_Skill.m_bSkillWriteDamage = TRUE;
                m_Skill.m_listSkillDamage.clear();
                //if (m_core.GetHp() <= 0)  //死亡状态下BUF不更新
                //{
                //	m_Skill.SetbRun(false);
                //}
                this->m_Skill.Run(dwDiff);  // Run Skill
                m_Skill.m_bSkillWriteDamage = FALSE;
                if (m_Skill.m_listSkillDamage.size() > 0 && m_scene) {
                    g_sendCmd->BeginWrite();
                    g_sendCmd->WriteShort(r2c_event);
                    g_sendCmd->WriteLong(g_region->m_time);
                    g_sendCmd->WriteShort(EVENT_ATTACK);
                    g_sendCmd->WriteLong(m_oId);
                    g_sendCmd->WriteShort(0);
                    g_sendCmd->WriteShort(0);  // skill attack
                    g_sendCmd->WriteByte(SKILL_TARGET_TYPE_NONE);
                    g_sendCmd->WriteByte(0);  //是否是吟唱(真打为0 吟唱为1)
                    m_Skill.SkillWriteDamageBuffer(g_sendCmd);
                    m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
                }
            }

            if (m_core.Pk > 0) {
                char cOldPKState = GetPKState();
                m_core.Pk -= HEARTBEAT_INTERVAL;
                if (m_core.Pk < 0)
                    m_core.Pk = 0;
                if (cOldPKState != GetPKState()) {
                    // 发送到客户端, 告诉自己名字状态改变
                    g_sendCmd->BeginWrite();
                    g_sendCmd->WriteShort(r2c_name_state);
                    g_sendCmd->WriteByte(GetPKState());
                    g_sendCmd->WriteLong(m_core.Pk);
                    RecvCmd(g_sendCmd);
                }
            } else {
                m_core.Pk = 0;
            }

            // npc贩卖道具
            if (m_bNpcSellRareItemStart && !m_NpcSellRareItem.empty()) {
                time_t nowTime = time(NULL);
                stlforeach(std::vector<SNpcSellRareItem>, iterSell, m_NpcSellRareItem) {
                    SNpcSellRareItem& sri = *iterSell;
                    if (sri.nextSpawnTime == 0) {
                        if (sri.count != sri.maxCount)
                            sri.nextSpawnTime = nowTime + sri.interval;
                    } else if (nowTime >= sri.nextSpawnTime) {
                        sri.count++;
                        if (sri.count < sri.maxCount) {
                            sri.nextSpawnTime = nowTime + sri.interval;
                        } else {
                            sri.nextSpawnTime = 0;
                        }
                    } else {
                    }
                }
            }
            break;
        default:
            break;
    }
    GodLightCreateMonster_DelAllMonster();  //tim.yang 神灯   玩家死亡清除掉神灯召唤出来的monster
    if (IsUser() &&
        m_IsAddMonster)  //玩家神灯召唤出来的monster被打死，将该玩家身上的所有相关属性清除
    {
        for (int i = 0; i < GODLIGHT_MONSTER_COUNT; ++i) {
            if (m_GodLightMonster[i] && m_GodLightMonster[i]->m_npcId != 1960) {
                m_GodLightMonster[i] = 0;
                m_monsterCount--;
            }
        }
    }
    if (m_dwPkGetItemTime && IsUser() && m_vPkGetItemList.size() &&
        m_dwPkGetItemTime < rtGetMilliseconds()) {
        m_dwPkGetItemTime = 0;
        m_vPkGetItemList.clear();
    }
    if (IsInProtect() && m_dwProtectTime < rtGetMilliseconds()) {
        m_dwProtectTime = 0;
    }
    if (IsInCombat() && m_dwCombatEndTime < rtGetMilliseconds()) {
        m_lIsCombat = 0;
        LeaveCombat();
    }
    //if(!m_core.bAdult)
    //	SetIncomeState(m_playerNowLoginTime);		//ac.ma
}

void CRegionCreature::RecvMsg(const char* msg, const char* param) {
    if (IsScriptLoaded())
        CallScript("OnRecvMsg", false, msg, param);
}

long CRegionCreature::Distance(long* pos) {
    float fX2, fY2;
    fX2      = m_pos[0] - pos[0];
    fY2      = m_pos[1] - pos[1];
    float fD = sqrt(fX2 * fX2 + fY2 * fY2);
    return (long)fD;
}

void CRegionCreature::ActiveAI() {
    if (m_ai && !m_ai->IsValid()) {
        m_ai->OnBorn(this);
    }
}

void CRegionCreature::DeleteAI() {
    if (m_ai) {
        m_ai->OnDead();
        g_aiMgr.CloseAI(m_ai);
        m_ai = NULL;
    }
    // StopTimer(CRE_TIMER_AI);
}

//////////////////////////////////////////////////////////////////////////
//Tianh  系统广播接口
//////////////////////////////////////////////////////////////////////////
//玩家名，被杀玩家名，BOSS名字，地点，装备ID
void CRegionCreature::Systembroadcast(const char* Playername, const char* Name,
                                      const char* BossName, const char* SceneName, long type,
                                      const char* ItemName) {
    /*string smsg = "";*/

    //char* message = RT_NEW char[strlen(msg) + 1024];
    //switch (type)
    //{
    //case SMITHINGWEAPONS:     //铸兵
    //	strcpy(message,"[打造兵器]");
    //	break;
    //case SMITHINGARMOR:       //铸甲
    //	strcpy(message,"[打造护甲]");
    //	break;
    //case STRENGTHENEQUIP:     //装备强化
    //	strcpy(message,"[装备强化]");
    //	break;
    //case BRATBOSS:            //击败BOSS
    //	strcpy(message,"[击败BOSS]");
    //	break;
    //case RENOVATEBOSS:        //BOSS刷新
    //	strcpy(message,"[BOSS刷新]");
    //	break;
    //case BRATFINALBOSSl:      //打败终极BOSS
    //	strcpy(message,"[终极BOSS]");
    //	break;
    //case FIELDKILLPERSON:     //野外杀人
    //	strcpy(message,"[武林战事]");
    //	break;
    //default:
    //	strcpy(message,"[无]");
    //}

    /*long iItemID = ItemID.id;*/
    /*rt2_strcat(message,msg);*/
    g_region->m_gws.SendSystemNews(Playername, Name, BossName, SceneName, type, CHAT_CHANNEL_NEWS,
                                   ItemName);  //heten
    /*delete message;*/
    return;
}

void CRegionCreature::SetAI(const char* ai) {
    if (m_ai) {
        g_aiMgr.CloseAI(m_ai);
        m_ai = NULL;
    }

    //----------add start by tony 05.08.09 ai modify--------------------//
    switch (m_npcInfo->AILevel) {
        case 0:
            m_ai_level = AI_FIXED;
            break;
        case 1:
            m_ai_level = AI_FIXED | AI_CONJURE;
            break;
        case 2:
            m_ai_level = AI_IDLE;
            break;
        case 3:
            m_ai_level = AI_IDLE | AI_PROTECT;
            break;
        case 4:
            m_ai_level = AI_IDLE | AI_CONJURE;
            break;
        case 5:
            m_ai_level = AI_IDLE | AI_PROTECT | AI_CONJURE;
            break;
        case 6:
            m_ai_level = AI_PATROL;
            break;
        case 7:
            m_ai_level = AI_PATROL | AI_PROTECT;
            break;
        case 8:
            m_ai_level = AI_PATROL | AI_CONJURE;
            break;
        case 9:
            m_ai_level = AI_PATROL | AI_PROTECT | AI_CONJURE;
            break;
        default:
            m_ai_level = AI_FIXED;
            break;
    }
    //----------add end   by tony 05.08.09 ai modify--------------------//

    if (strcmp(ai, "pet") == 0)
        m_ai = g_aiMgr.CreateAI("pet", m_ai_level);
    else if (strcmp(ai, "callnpc") == 0)
        m_ai = g_aiMgr.CreateAI("callnpc", m_ai_level);
    else if (strcmp(ai, "guarder") == 0)
        m_ai = g_aiMgr.CreateAI("guarder", m_ai_level);
    else if (strcmp(ai, "s_guarder") == 0)
        m_ai = g_aiMgr.CreateAI("s_guarder", m_ai_level);
    else if (strcmp(ai, "monster") == 0)
        m_ai = g_aiMgr.CreateAI("monster", m_ai_level);
    else if (strcmp(ai, "hero") == 0)
        m_ai = g_aiMgr.CreateAI("hero", m_ai_level);
    else if (strcmp(ai, "g_monster") == 0)
        m_ai = g_aiMgr.CreateAI("g_monster", m_ai_level);  //tim.yang
    else if (strcmp(ai, "mc_monster") == 0) {
        m_ai = g_aiMgr.CreateAI("mc_monster", m_ai_level);
    } else if (strcmp(ai, "tour") == 0)
        m_ai = g_aiMgr.CreateAI("tour", m_ai_level);
    else
        m_ai = NULL;
}

void CRegionCreature::AddSelfDef(CRegionCreature* enemy) {
    if (g_WarfieldMgr.IsInWarfield(this))
        return;

    list<SSelfDef>::iterator it;
    for (it = m_selfDef.begin(); it != m_selfDef.end(); it++) {
        if (it->enemyId == enemy->m_oId) {
            it->beginTime = rtGetMilliseconds();
            return;
        }
    }

    SSelfDef def;
    def.beginTime = rtGetMilliseconds();
    def.enemyId   = enemy->m_oId;
    def.enemyName = enemy->m_core.Name;
    m_selfDef.push_back(def);

    rt2_sprintf(g_strStaticBuffer, R(MSG_SELF_DEF_BEGIN), enemy->m_core.Name.c_str());
    SendSystemMessage(g_strStaticBuffer);
    return;
}

bool CRegionCreature::IsSelfDef(CRegionCreature* enemy) {
    list<SSelfDef>::iterator it;
    for (it = m_selfDef.begin(); it != m_selfDef.end(); it++) {
        if (enemy) {
            if (it->enemyId == enemy->m_oId) {
                return true;
            }
        }
    }
    return false;
}

///add by Tianh
void CRegionCreature::DeleteLifeSkill(unsigned short wID) {
    CActorSkill* pActorSkill = FindActorSkill(this, m_oId);
    int          wSID        = 0;
    if (pActorSkill) {
        wSID = pActorSkill->GetSkillwSubID(wID);
    }
    //遗忘配方
    if (wSID == ITEMCOMPOSE_WEAPON_SKILL)  //铸兵术
    {
        std::list<short>::iterator iterCompose = m_listMatchID.begin();
        for (; iterCompose != m_listMatchID.end();) {
            SItemCompose* pComposeClass =
                g_region->m_pItemManager->m_pComposeMgr->FindComposing(*iterCompose);
            if (pComposeClass) {
                if (pComposeClass->ComposeType == 1) {
                    /*m_listMatchID.remove(*iterCompose);*/
                    iterCompose = m_listMatchID.erase(iterCompose);
                    continue;
                }
            }

            ++iterCompose;
        }
        pActorSkill->DeleteSkill(wID, true);
    } else if (wSID == ITEMCOMPOSE_EQUIP_SKILL)  // 铸甲术
    {
        std::list<short>::iterator iterCompose = m_listMatchID.begin();
        for (; iterCompose != m_listMatchID.end();) {
            SItemCompose* pComposeClass =
                g_region->m_pItemManager->m_pComposeMgr->FindComposing(*iterCompose);
            if (pComposeClass) {
                if (pComposeClass->ComposeType == 4) {
                    /*m_listMatchID.remove(*iterCompose);*/
                    iterCompose = m_listMatchID.erase(iterCompose);
                    continue;
                }
            }
            ++iterCompose;
        }
        pActorSkill->DeleteSkill(wID, true);
    } else if (wSID == ITEMCOMPOSE_YAOPING_SKILL)  // 炼丹术
    {
        std::list<short>::iterator iterCompose = m_listMatchID.begin();
        for (; iterCompose != m_listMatchID.end();) {
            SItemCompose* pComposeClass =
                g_region->m_pItemManager->m_pComposeMgr->FindComposing(*iterCompose);
            if (pComposeClass) {
                if (pComposeClass->ComposeType == 2) {
                    iterCompose = m_listMatchID.erase(iterCompose);
                    continue;
                }
            }
            ++iterCompose;
        }
        pActorSkill->DeleteSkill(wID, true);
    } else if (wSID == ITEMCOMPOSE_ENCHANT_SKILL)  // 制符术
    {
        std::list<short>::iterator iterCompose = m_listMatchID.begin();
        for (; iterCompose != m_listMatchID.end();) {
            SItemCompose* pComposeClass =
                g_region->m_pItemManager->m_pComposeMgr->FindComposing(*iterCompose);
            if (pComposeClass) {
                if (pComposeClass->ComposeType == 3) {
                    iterCompose = m_listMatchID.erase(iterCompose);
                    continue;
                }
            }
            ++iterCompose;
        }
        pActorSkill->DeleteSkill(wID, true);
    } else {
        if (pActorSkill) {
            pActorSkill->DeleteSkill(wID, true);
        }
    }
    UpdateMatchToClient();
    GetPlayerLogic()->OnSkillNumChange();
}

void CRegionCreature::RemoveTimeoutSelfDef() {
    if (m_selfDef.empty())
        return;

    list<SSelfDef>::iterator it;
    for (it = m_selfDef.begin(); it != m_selfDef.end();) {
        if (rtGetMilliseconds() - it->beginTime > 60 * 1000) {
            rt2_sprintf(g_strStaticBuffer, R(MSG_SELF_DEF_END), it->enemyName.c_str());
            SendSystemMessage(g_strStaticBuffer);
            it = m_selfDef.erase(it);
            continue;
        }
        it++;
        continue;
    }
}

void CRegionCreature::Interactive(CRegionCreature* target, CG_CmdPacket* cmd) {
    char* str;
    if (!cmd->ReadString(&str))
        return;

    //目前只有答题返回时才会调用这里
    if (strncmp(str, "Script_", 7) == 0 && IsScriptLoaded()) {
        const char* sScriptStr = str + 7;
        if (sScriptStr[0] != '\0')
            CallScript(sScriptStr, false);
        return;
    }
}

//
// OnBeAttacked 中不能再调用攻击、技能、场景的findcreature函数，否则会引起递归调用
//
void CRegionCreature::OnBeAttacked(CRegionCreature* enemy, int damage) {
    // if(enemy == m_master) return;
    if (IsScriptLoaded() && !IsUser() && !IsUserPet() && !IsCallNpc()) {
        if (enemy) {
            CallScript("OnBeAttacked", false, enemy);
        }
    }

    /*
	// 避免宠物主动攻击导致红名
	if( IsUserPet() && (enemy->IsUser()||enemy->IsUserPet()) )
		return;
	*/

    if (enemy) {
        int hate = enemy->m_core.Hate + (enemy->m_core.HateRate / 100.0f) * damage;
        AddEnemy(enemy, damage);
        if (!m_lockTarget)
            AddTarget(enemy, hate, damage);

        if (m_ai)
            m_ai->OnBeAttacked(enemy, damage);

        if ((IsUserPet() && m_ai) || (m_pet && m_pet->m_ai)) {
            CRegionPetAI* petAI = NULL;
            if (IsUserPet())
                petAI = (CRegionPetAI*)m_ai;
            else
                petAI = (CRegionPetAI*)m_pet->m_ai;
            if (petAI && (petAI->m_eAS == CRegionPetAI::AS_SYNERGY ||
                          petAI->m_eAS == CRegionPetAI::AS_PASSIVE)) {
                petAI->OnAttack(enemy);
            }
        }

        //Tianh   召唤的作战NPC  这里只有USER才可以有
        if (CallNpc.size() != 0) {
            std::vector<CRegionCreature*>::iterator vit = CallNpc.begin();
            for (; vit != CallNpc.end(); vit++) {
                if ((*vit)->m_ai) {
                    CRegionCallNpc* CallNpcAI = NULL;
                    CallNpcAI                 = (CRegionCallNpc*)(*vit)->m_ai;
                    if (CallNpcAI && (CallNpcAI->m_eAS == CRegionCallNpc::AS_SYNERGY ||
                                      CallNpcAI->m_eAS == CRegionCallNpc::AS_PASSIVE)) {
                        CallNpcAI->OnAttack(enemy);
                    }
                }
            }
        }

        if (m_npcInfo && g_WarfieldMgr.IsRebirthNpc(m_npcInfo->Id)) {
            g_WarfieldMgr.OnRebirthPosBeAttacked(this);
        }

        if (m_npcInfo && g_WarfieldMgr.IsFlagNpc(m_npcInfo->Id)) {
            g_WarfieldMgr.OnFlagBeAttacked(m_npcInfo->Id, this);
        }
    }
}

void CRegionCreature::AddProtector() {
    if (m_npcInfo != NULL) {
        if (m_npcInfo->ProtecterID == 0)
            return;
    }

    m_protectorList.clear();

    std::vector<CRegionCreature*> creList;

    m_scene->GetAroundCreatureByGrid(m_pos,
                                     /*m_viewRange*/ m_assistRange, creList, this);

    if (creList.empty()) {
        return;
    }

    std::vector<CRegionCreature*>::iterator iter = creList.begin();

    for (; iter != creList.end(); iter++) {
        if ((*iter)->m_npcInfo && m_npcInfo) {
            if ((*iter)->m_npcInfo->Id != m_npcInfo->ProtecterID)
                continue;
            if (*iter == this)
                continue;

            m_protectorList.push_back((*iter));
        }
    }
}

bool CRegionCreature::CreateAllUnderlingNpc(short npc_id, char Type, int num, long* pos) {
    short id = npc_id;  //npc_id unused

    if (num <= 0) {
        return false;
    }

    //召唤新的NPC时把原来的清楚
    std::vector<CRegionCreature*>::iterator it = CallNpc.begin();
    for (; it != CallNpc.end(); it++) {
        if (*it) {
            (*it)->DeleteAI();
            m_scene->RemoveCreature((*it)->m_oId);
            g_factory->DestroyObject((*it));
            (*it) = NULL;
        }
    }
    CallNpc.clear();

    //以玩家为中心画圆 得到圆上平均距离的8个点
    //一个圆可以有8个点 每个点相差360/8度
    int    Radius = 25;  //半径为10
    double vPov   = 45;
    for (int i = 0; i < 8; i++) {
        float vpos[3];

        //过了180度就变了  有时间改正
        double Pov = 45 * i;
        float  x   = Radius / sin(Pov);
        float  y   = Radius / cos(Pov);
        vpos[0]    = m_pos[0] + x;
        vpos[1]    = m_pos[1] + y;
        vpos[2]    = m_pos[2];

        if (i > num) {
            break;
        }
        if (Type == PLAYER_CALL)  //玩家召唤
        {
            CreateUnderlingNpc(id, vpos);
        } else if (Type == BOSS_CALL)  //BOSS或者NPC召唤
        {
            CreateUnderlingNpcByBoss(id, vpos);
        }
    }

    SendMagCallNpc();

    return true;
}

bool CRegionCreature::CreateUnderlingNpc(short npc_id, float pos[3]) {
    CRegionCreature* p_cre = g_region->CreateNpc(npc_id);
    if (!p_cre) {
        return false;
    }

    long vpos[3];
    vpos[0] = (long)pos[0];
    vpos[1] = (long)pos[1];
    vpos[2] = (long)pos[2];
    if (!m_scene->AddCreature(p_cre, vpos, 0)) {
        g_factory->DestroyObject(p_cre);
        return false;
    }

    CallNpc.push_back(p_cre);

    p_cre->m_master = this;
    p_cre->SetAI("callnpc");
    p_cre->ActiveAI();
    p_cre->m_ai->OnFollow(this);
    p_cre->SetTimer(CRE_TIMER_HEARTBEAT, HEARTBEAT_INTERVAL);
    p_cre->ResetCheaterCheck();

    p_cre->m_core.Lev = m_core.Lev;

    return true;
}

bool CRegionCreature::CreateUnderlingNpcByBoss(short npc_id, float pos[3]) {
    CRegionCreature* p_cre = g_region->CreateNpc(npc_id);
    if (!p_cre) {
        return false;
    }

    long vpos[3];
    vpos[0] = (long)pos[0];
    vpos[1] = (long)pos[1];
    vpos[2] = (long)pos[2];

    if (!m_scene->AddCreature(p_cre, vpos, 0)) {
        g_factory->DestroyObject(p_cre);
        return false;
    }

    p_cre->SetAI("monster");
    p_cre->ActiveAI();

    p_cre->m_core.Lev = m_core.Lev;

    return true;
}

void CRegionCreature::DeleteAddUnderlingNpc() {
    if (CallNpc.size() == 0) {
        return;
    }

    std::vector<CRegionCreature*>::iterator it = CallNpc.begin();
    for (; it != CallNpc.end(); it++) {
        if (*it) {
            (*it)->DeleteAI();
            m_scene->RemoveCreature((*it)->m_oId);
            g_factory->DestroyObject((*it));
            (*it) = NULL;
        }
    }

    CallNpc.clear();

    SendMagCallNpc();
}

void CRegionCreature::DeleteCallNpc(int sV) {
    if (sV < 0) {
        return;
    }
    if (CallNpc.size()) {
        for (int i = 0; i < sV; i++) {
            std::vector<CRegionCreature*>::iterator it = CallNpc.begin();

            if (*it) {
                (*it)->DeleteAI();
                m_scene->RemoveCreature((*it)->m_oId);
                g_factory->DestroyObject((*it));
                (*it) = NULL;

                it = CallNpc.erase(it);

                /*it++;*/
            }

            SendMagCallNpc();

            if (!CallNpc.size()) {
                return;
            }
        }
    }
}

void CRegionCreature::SendMagCallNpc() {
    // 发送到客户端, 告诉召唤NPC数量
    long Count = CallNpc.size();
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_CallNpc_Count);
    g_sendCmd->WriteLong(GetObjectId());
    g_sendCmd->WriteLong(Count);
    RecvCmd(g_sendCmd);
}

void CRegionCreature::CalcExploit(CRegionCreature* other) {
    int addExploit = 0;
    int minItemId  = 0;
    int maxItemId  = 0;
    if (other->m_core.Lev >= 30 && other->m_core.Lev <= 59) {
        addExploit = 1;
        minItemId  = 5027;
        maxItemId  = 5030;
    } else if (other->m_core.Lev >= 60 && other->m_core.Lev <= 69) {
        addExploit = 2;
        minItemId  = 5031;
        maxItemId  = 5034;
    } else if (other->m_core.Lev >= 70 && other->m_core.Lev <= 79) {
        addExploit = 4;
        minItemId  = 5035;
        maxItemId  = 5038;
    } else if (other->m_core.Lev >= 80 && other->m_core.Lev <= 89) {
        addExploit = 8;
        minItemId  = 5039;
        maxItemId  = 5042;
    } else if (other->m_core.Lev >= 90 && other->m_core.Lev <= 99) {
        addExploit = 16;
        minItemId  = 5043;
        maxItemId  = 5046;
    } else if (other->m_core.Lev >= 100 && other->m_core.Lev <= 109) {
        addExploit = 32;
        minItemId  = 5047;
        maxItemId  = 5050;
    } else if (other->m_core.Lev >= 110 && other->m_core.Lev <= 119) {
        addExploit = 64;
        minItemId  = 5051;
        maxItemId  = 5054;
    } else if (other->m_core.Lev >= 120) {
        addExploit = 128;
        minItemId  = 5055;
        maxItemId  = 5058;
    }
    if (m_core.Lev > other->m_core.Lev) {
        switch (m_core.Lev - other->m_core.Lev) {
            case 1:
                addExploit = addExploit * 0.8;
                break;
            case 2:
                addExploit = addExploit * 0.6;
                break;
            case 3:
                addExploit = addExploit * 0.4;
                break;
            case 4:
                addExploit = addExploit * 0.2;
            default:
                addExploit = 0;
        }
    }

    AddExploit(addExploit);
    int iRateRt = rtRandom(g_pkConfig->exploitManager.iRandom + 1, 1);  //1 is 狂战士
    if (iRateRt == 1) {
        int iRate = rtRandom(maxItemId + 1, minItemId);  //1 is 狂战士
        CreateDropItem(iRate, ItemDrop_Descend);
        rt2_sprintf(g_strStaticBuffer, R(MSG_GET_ROLE_APPARATUS), other->m_core.Name.c_str());
        SendSystemMessage(g_strStaticBuffer);
    }
}

void CRegionCreature::CreateDropItem(const DWORD itemId, EItemDropStyle dropStyle) {
    //SItemID item = g_region->m_pItemManager->CreateItem(itemId);
    //if(ItemID_IsValid(item))
    //{
    //	CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
    //	dummy->m_item = item;
    //	dummy->m_dropStyle = dropStyle;
    //	if(!m_scene->AddItem(dummy,m_pos,0))
    //		g_factory->DestroyObject(dummy);
    //}
}

//
// killer 不可以是空
//
long CRegionCreature::OnDead(CRegionCreature* killer) {
    long nRet = CPlayerLogic::eHaveItem;

    rt2_sprintf(g_strStaticBuffer, R(MSG_BE_KILLED), killer->m_core.Name.c_str());
    SendBattleInfoMessage(g_strStaticBuffer);

    stlforeach(STargetList, iter, m_targets) {
        CRegionCreature* c = m_scene->FindCreature((*iter).Id);
        if (c)
            c->RemoveTargetByCreature(this);
    }

    if ((IsUser() /*||IsUserPet()*/) &&
        (killer->IsUser() || killer->IsUserPet() || killer->IsCallNpc())) {
        // 全部转换成玩家计算
        CRegionCreature *me, *enemy;
        me = this;
        if (IsUserPet())
            me = m_master;
        enemy = killer;
        if (killer->IsUserPet() || killer->IsCallNpc())
            enemy = killer->m_master;
        enemy->OnPkOther(me);
        me->OnBePked(enemy);
    }

    if (m_pPShop)
        StopPShop();
    if (m_pTrade)
        StopPlayerTrade();

    if (IsScriptLoaded())
        CallScript("OnDead", false, killer);

    // 把不是由被动技能添加的效果都删除
    if (&m_Skill) {
        m_Skill.DeleteUIDEffect(0, true);
    }

    //-------------------------------add start by tony 05.05.18------------------//
    if (IsGuarder()) {
        //modify later
        //...
        g_GuarderMgr.KillGuarder(m_npcInfo->Id);
    }
    //-------------------------------add end   by tony 05.05.18------------------//

    CRegionCreature* maxDamageCre = NULL;
    if ((IsGuarder() || IsMonster()) /*&& OB_TYPE(m_scene) != OBJECT_TYPE_SCENE_FB */ &&
        !m_cheatDummy) {
        if (g_region->m_pWarZhulu->IsZhuluActive(true)) {
            if (m_npcInfo->Id == 1997)
                g_region->m_pWarZhulu->OnZhuluWin(FACTION_ZHOU);
            if (m_npcInfo->Id == 2000)
                g_region->m_pWarZhulu->OnZhuluWin(FACTION_SHANG);
        }

        float totalExp =
            g_cfg.game.exp_rate *
            m_killReward
                .Exp;  //g_cfg.game.exp_rate * GetRand( rtFloor(m_killReward.Exp*1.1 + 1.0f), m_killReward.Exp*0.9 );
        //totalExp *= g_region->GetExpRate();

        int maxDamage = 0;

        CRegionCreature* maxDamageCreSingle = NULL;
        int              maxDamageSingle    = -1;

        // 分配经验,按造成伤害的百分比分配
        map<CRegionCreature*, int>           kpMap;
        map<CRegionCreature*, int>::iterator kpIt;

        STargetList::iterator it;
        for (it = m_enemyList.begin(); it != m_enemyList.end(); it++) {
            CRegionCreature* cur = m_scene->FindCreature(it->Id);
            if (!cur)
                continue;
            if (!cur->IsUser() && !cur->IsUserPet())
                continue;

            CPlayerLogic::LiveType type = cur->GetPlayerLogic()->GetLiveType();
            if (CPlayerLogic::eLive != type) {
                continue;
            }

            int exp = 0;
            exp     = totalExp * ComputeExpRate(cur->m_core.Lev, m_core.Lev) * it->Damage /
                  m_core.MaxHp.GetValue();
            if (exp == 0)
                exp++;

            if (it->Damage > maxDamageSingle) {
                maxDamageCreSingle = cur;
                maxDamageSingle    = it->Damage;
            }

            // 组队经验累加给队长
            if (!cur->m_pTeam->mData.Empty()) {
                CRegionUser* user = g_region->FindUser(cur->m_pTeam->mData.GetCaptain());
                if (user)
                    cur = user->m_dummy;
            }
            if (!cur)
                continue;

            kpIt = kpMap.find(cur);
            if (kpIt == kpMap.end())
                kpMap[cur] = exp;
            else
                kpMap[cur] += exp;

            int tmp = kpMap[cur];
            if (tmp > maxDamage) {
                maxDamageCre = cur;
                maxDamage    = tmp;
            }
        }

        for (kpIt = kpMap.begin(); kpIt != kpMap.end(); kpIt++) {
            CRegionCreature* cur = kpIt->first;
            cur->LeaveCombat();  // 清空战斗状态
            if (cur->m_pTeam->mData.Empty()) {
                cur->m_nMonsterCount++;
                // 打卡
                cur->AddExp(kpIt->second * cur->GetExpRate(), 0);
                if (cur == maxDamageCre &&
                    GetProb(m_killReward.GoldRate * ComputeGoldRate(cur->m_core.Lev, m_core.Lev))) {
                    int goldNum =
                        g_cfg.game.gold_rate *
                        GetRand(rtFloor(m_killReward.Gold * 1.1 + 1.0f), m_killReward.Gold * 0.9);
                    cur->AddGold(goldNum);
                    cur->m_nGetMoneyFromMonster += goldNum;
                }
            } else {
                // team player
                // get total lev,should modify later...
                int totalLev = 0;
                int validNum = 0;
                int num      = cur->m_pTeam->mData.Size();
                for (int i = 0; i < num; i++) {
                    CRegionUser* user = g_region->FindUser(cur->m_pTeam->mData[i].mDBID);
                    if (!user)
                        continue;
                    CPlayerLogic::LiveType type = user->m_dummy->GetPlayerLogic()->GetLiveType();
                    if (CPlayerLogic::eLive != type) {
                        continue;
                    }

                    // 距离限制
                    if (user->m_dummy->Distance(m_pos) <= 800) {
                        totalLev += user->m_dummy->m_core.Lev;
                        validNum++;
                    }
                }

                int addExp = 0;
                /*if(validNum == 1)
				addExp = kpIt->second;
				else
				addExp = kpIt->second * (1.0+(validNum)*0.1);*/
                addExp = kpIt->second;

                if (addExp < 0)
                    addExp = 0;
                // LOG2("team get exp,src = %d,final = %d\n",m_killReward.Exp, totalExp);

                int addGold = 0;
                if (cur == maxDamageCre &&
                    GetProb(m_killReward.GoldRate *
                            ComputeGoldRate(killer->m_core.Lev, m_core.Lev))) {
                    addGold =
                        g_cfg.game.gold_rate *
                        GetRand(rtFloor(m_killReward.Gold * 1.1 + 1.0f), m_killReward.Gold * 0.9);
                }

                for (int i = 0; i < num; i++) {
                    CRegionUser* user = g_region->FindUser(cur->m_pTeam->mData[i].mDBID);
                    if (!user)
                        continue;
                    CPlayerLogic::LiveType type = user->m_dummy->GetPlayerLogic()->GetLiveType();
                    if (CPlayerLogic::eLive != type) {
                        continue;
                    }
                    if (user->m_dummy->Distance(m_pos) > 800 || 0 == validNum) {
                        continue;
                    }
                    // 距离限制必须与上面的代码一样
                    //if (user->m_dummy->Distance(m_pos) <= 800 && validNum != 0)
                    //{

                    int exp = 0;
                    /*
					if(user->m_dummy == killer)
					{
					exp = totalExp / (validNum + 1) * 2;
					}
					else
					*/
                    {
                        //exp = addExp / (validNum);
                        //(怪物经验*(1+(0.2+0.02*队伍人数)*队伍人数))*人物等级/队伍等级总合
                        if (0 != totalLev) {
                            exp = addExp * (1 + (0.2 + 0.02 * validNum) * validNum) *
                                  user->m_dummy->m_core.Lev / totalLev;
                        }
                        // 打卡
                        exp *= user->m_dummy->GetExpRate();
                        /*if(m_core.Lev > user->m_dummy->m_core.Lev + 8 )
						exp = exp * GetRealLevel(user->m_dummy->m_core.Lev) / GetRealLevel(m_core.Lev);*/
                    }

                    user->m_dummy->AddExp(exp, 0);
                    // LOG2("	team member get exp %d, sp %d\n",exp, sp);
                    if (addGold > 0) {
                        // user->m_dummy->AddGold( addGold * user->m_dummy->m_core.Lev/totalLev );
                        user->m_dummy->AddGold(addGold / validNum);
                        user->m_dummy->m_nGetMoneyFromMonster += addGold / validNum;
                    }
                    //}
                }
            }
        }

        // 改成道具直接掉在尸体上
        // 金钱道具归最大伤害者,金钱上面已经处理过了，接下来处理道具
        //lyytodo 改回掉落到地上
        //if(maxDamageCre && maxDamageCre->IsUser() && m_npcInfo)
        //{
        //	bool bFirstDropItem = true;
        //	SItemID item;
        //	for(int i=0; i<MAX_NPC_DROP_ITEM; i++)
        //	{
        //              if(m_npcInfo->ItemDropRate[i] < 1 &&
        //				!GetProb(m_npcInfo->ItemDropRate[i] * g_cfg.game.item_drop_rate * ComputeGoldRate(killer->m_core.Lev,m_core.Lev)))
        //			continue;
        //		EItemColor color = ItemColor_White;
        //		if(GetProb(m_npcInfo->ItemGreenRate*g_cfg.game.item_green_rate))
        //		{
        //			color = ItemColor_Green;
        //			if(GetProb(m_npcInfo->ItemBlueRate*g_cfg.game.item_blue_rate))
        //			{
        //				color = ItemColor_Blue;
        //				// golden item
        //				if(GetProb(m_npcInfo->ItemGoldRate))
        //				{
        //					color = ItemColor_Golden;
        //				}
        //			}
        //		}

        //		EItemBind bindType = IB_NotBind;
        //		switch(m_npcInfo->ItemBindType)
        //		{
        //			case 0: bindType = IB_NotBind;break;
        //			case 1: bindType = IB_BindWhenEquip;break;
        //			case 2: bindType = IB_BindWhenPick;break;
        //			case 3: //高级模式：金装拾取绑定，蓝装20%概率装备绑定、80概率拾取绑定，其他不绑定
        //				{
        //					if(color == ItemColor_Golden)
        //					{
        //						bindType = IB_BindWhenPick;
        //					}
        //					else if(color == ItemColor_Blue)
        //					{
        //						if(GetProb(0.2)) bindType = IB_NotBind;
        //						else bindType = IB_BindWhenPick;
        //					}
        //					break;
        //				}
        //			default: bindType = IB_NotBind;
        //		}

        //		item = g_region->m_pItemManager->CreateItem(m_npcInfo->Item[i],color,0,bindType);
        //		//if(!ItemCanNpcTrade(item.type)) continue;
        //		if(ItemID_IsValid(item))
        //		{
        //			CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
        //			dummy->m_item = item;
        //			dummy->m_hideTime = rtGetMilliseconds() + 1000;
        //			dummy->m_masterId = maxDamageCre->m_oId;
        //			dummy->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
        //			dummy->m_dropStyle = ItemDrop_Descend;

        //			static long pos[3];
        //			static int dropCounter = 0;
        //			if (bFirstDropItem)
        //			{
        //				pos[0] = m_pos[0];	pos[1] = m_pos[1];  pos[2] = m_pos[2];
        //				dropCounter = 0;
        //				bFirstDropItem = false;
        //			}
        //
        //			if ( !CRegionItem::GetNextDropPos(m_pos, pos, dropCounter) )
        //			{
        //				pos[0] = m_pos[0];	pos[1] = m_pos[1];  pos[2] = m_pos[2];
        //			}

        //			if(!m_scene->AddItem(dummy,pos,0))
        //				g_factory->DestroyObject(dummy);
        //		}
        //	}
        //}

        if (maxDamageCreSingle &&
            (maxDamageCreSingle->IsUser() || maxDamageCreSingle->IsUserPet()) && m_npcInfo) {
            //PZH
            if (maxDamageCreSingle->IsUserPet()) {
                maxDamageCreSingle = maxDamageCreSingle->m_master;
            }
            int kk = 0;
            for (kk = 0; kk < m_npcInfo->nTaskNum; kk++) {
                if (!GetProb(m_npcInfo->TaskItemDropRate[kk])) {
                    continue;
                }
                int nRet = maxDamageCreSingle->GetPlayerLogic()->AllMyTeamAddTaskItem(this, kk);
                //
                // check task item
                if (/*PZH*/ 0 != nRet && /**/ m_npcInfo->FbTask == 0 && m_npcInfo->TaskId[kk] &&
                    maxDamageCreSingle->m_task.IsTaskAccept(m_npcInfo->TaskId[kk])) {
                    if (/*GetProb(m_npcInfo->TaskItemDropRate) && */
                        maxDamageCreSingle->GetItemNum(m_npcInfo->TaskItemId[kk], ItemColor_White) <
                        m_npcInfo->TaskItemLimit[kk]) {
                        maxDamageCreSingle->AddItem(m_npcInfo->TaskItemId[kk], ItemColor_White);
                        STaskInfo* info =
                            maxDamageCreSingle->m_task.FindTask(m_npcInfo->TaskId[kk]);
                        if (NULL != info) {
                            if (NULL != info->Desc) {
                                maxDamageCreSingle->CanFinishTask(info->Desc, 0, NULL);
                            }
                        }
                        //maxDamageCreSingle->UpdateToClient(NET_UPDATE_ALL);
                        maxDamageCreSingle->UpdateToClient(NET_CRE_UPDATE_TASK);
                    }
                    //挖宝任务随机事件
                    if (m_npcInfo->TaskId[kk] >= 1628 && m_npcInfo->TaskId[kk] <= 1641) {
                        OnTreasureDiscover(killer);
                    }

                    //---------add start by tony 06.05.10---------------------------------------//
                    //-reason: 跑环任务杀怪
                    STaskInfo* info = maxDamageCreSingle->m_task.FindTask(m_npcInfo->TaskId[kk]);
                    if (info && info->Desc && info->Desc->circleTask == TASK_CIRCLE_BATTLE) {
                        SNpc* pMonster = g_TableNpc.FindNpc(info->m_killmonsterID);
                        if (m_npcInfo && pMonster && info->m_killmonsterID == m_npcInfo->Id) {
                            if (info->m_currMonsterNum <
                                atol(info->Desc->taskReq[0].param[1].c_str())) {
                                info->m_currMonsterNum += 1;
                                std::string monster =
                                    "已杀死" + pMonster->Name + ":" +
                                    std::string(rtFormatNumber(info->m_currMonsterNum));
                                maxDamageCreSingle->SendSystemMessage((char*)monster.c_str());
                                maxDamageCreSingle->UpdateToClient(NET_CRE_UPDATE_TASK);
                                //NetSend_r2c_task_event(2);
                            }
                        }
                        maxDamageCreSingle->CanFinishTask(info->Desc, 0, NULL);
                    }

                    //---------add end   by tony 06.05.10---------------------------------------//
                }
            }
        }

        if (IsGuarder() || IsMonster()) {
            //PZH
            nRet = GetPlayerLogic()->GetMonsterLogicExt()->DropItemS(
                maxDamageCreSingle ? maxDamageCreSingle : killer);
        }
    }

    if (!IsGuarder() && !IsMonster()) {
        if (m_spawn)
            m_spawn->KillNpc(this);
    }

    if (!IsUserPet())
        DeleteAI();
    return nRet;
}

void CRegionCreature::OnTreasureDiscover(CRegionCreature* discover) {
    char   cTmp512[512];
    double boss_rate = 1 / 500;
    if (GetProb(boss_rate))  //刷Boss
    {
        //刷新Boss
        std::string      npc_name                 = "";
        static int       Default_Faction          = 0;
        static int       Default_unitedPoint      = 1;
        static int       Default_loyalPoint       = 1;
        static int       Default_bravePoint       = 1;
        static int       Default_angryPointLevel1 = 30;
        static int       Default_angryPointLevel2 = 70;
        CRegionCreature* cre;

        //if(GetProb(0.3))
        //{
        //	cre = g_region->CreateNpc(1003);
        //	npc_name = "神鬼";
        //}
        //else if(GetProb(0.3))
        //{
        //	cre = g_region->CreateNpc(1004);
        //	npc_name = "妖后";
        //}
        //else
        //{
        //	cre = g_region->CreateNpc(1005);
        //	npc_name = "魔帝";
        //}
        cre->m_faction          = Default_Faction;
        cre->m_unitedPoint      = Default_unitedPoint;
        cre->m_loyalPoint       = Default_loyalPoint;
        cre->m_bravePoint       = Default_bravePoint;
        cre->m_angryPointLevel1 = Default_angryPointLevel1;
        cre->m_angryPointLevel2 = Default_angryPointLevel2;

        if (g_region->m_defScene->AddCreature(cre, m_pos, m_dir)) {
            if (cre->IsScriptLoaded())
                cre->CallScript("OnCreate", false);

            //discover->SendSystemMessage(R(MSG_TREASURE_BOSS));

            //rt2_sprintf(cTmp512, "玩家%s挖宝惊动了“%s”，坐标在(%d,%d)!!! 掉落法宝炼化符及各类宝石，极低概率掉落金色黑麟。",discover->m_core.Name.c_str(), npc_name.c_str(), m_pos[0]/20,m_pos[1]/20);
            //g_region->m_gws.BroadcastBulletin(cTmp512);

        } else {
            g_factory->DestroyObject(cre);
            cre = NULL;
        }

        //全服公告

        if (true == m_npcInfo->isPrompt) {
            RtsSceneBlockRtb* pMap   = m_scene->FindBlockByPos(m_pos);
            string            strone = "";
            if (pMap) {
                strone = pMap->m_szBlockName;
            }

            string str1 = m_npcInfo->Name;

            string      str        = cre->m_core.Name;
            const char* playername = str.c_str();
            const char* Name       = "";
            const char* BossName   = str1.c_str();
            const char* SceneName  = strone.c_str();
            const char* ItemName   = "";
            cre->Systembroadcast(playername, Name, BossName, SceneName, RENOVATEBOSS, ItemName);

            /*在[地点]刮起一阵妖风，风吹沙走，昏天暗地，一个邪恶的身影从地底冒出，[BOSS]又回到人间了。*/
        }
    }
    //else if(GetProb(0.3)) //刷恶鬼
    //{
    //	//刷新小怪

    //	std::string npc_name = "";
    //	static int Default_Faction = 0;
    //	static int Default_unitedPoint = 1;
    //	static int Default_loyalPoint = 1;
    //	static int Default_bravePoint = 1;
    //	static int Default_angryPointLevel1 = 30;
    //	static int Default_angryPointLevel2 = 70;

    //	CRegionCreature *cre = g_region->CreateNpc(1960);
    //	cre->m_faction = Default_Faction;
    //	cre->m_unitedPoint = Default_unitedPoint;
    //	cre->m_loyalPoint = Default_loyalPoint;
    //	cre->m_bravePoint = Default_bravePoint;
    //	cre->m_angryPointLevel1 = Default_angryPointLevel1;
    //	cre->m_angryPointLevel2 = Default_angryPointLevel2;

    //	if(m_scene->AddCreature(cre, m_pos, m_dir))
    //	{
    //		if(cre->IsScriptLoaded())
    //			cre->CallScript("OnCreate", false);

    //		cre->m_ai->OnBeAttacked(discover, 10000);
    //		discover->SendSystemMessage(R(MSG_TREASURE_GHOST));

    //		if(GetProb(0.5)) //50%的概率发出全服公告
    //		{
    //			/*rt2_sprintf(cTmp512, "玩家%s挖宝惊动了恶鬼缠身，坐标在(%d,%d)，击败恶鬼有可能获得五光宝石和刑天宝石!!!",discover->m_core.Name.c_str(), m_pos[0]/20,m_pos[1]/20);
    //			g_region->m_gws.BroadcastBulletin(cTmp512);*/
    //			RtsSceneBlockRtb* pMap = m_scene->FindBlockByPos(m_pos);
    //			string strone = pMap->m_szFileName;

    //			string str = "玩家[";
    //			str += discover->m_core.Name.c_str();
    //			str += "]挖宝惊动了恶鬼缠身，在[";
    //			str += strone;
    //			str += "]，击败恶鬼有可能获得五光宝石和刑天宝石!!!";
    //			const char* msg = str.c_str();
    //			SItemID ItemID;
    //			this->Systembroadcast(msg,RENOVATEBOSS,ItemID,discover->m_core.Name.c_str());
    //		}
    //	}
    //	else
    //	{
    //		g_factory->DestroyObject(cre);
    //		cre = NULL;
    //	}
    //}
}

void CRegionCreature::Quit() {
    DeleteAddUnderlingNpc();

    if (m_pTrade) {
        DWORD            dwTradeWith = m_pTrade->GetOtherPlayer(m_oId);
        CRegionCreature* pPlayerTo   = m_scene->FindCreature(dwTradeWith);
        if (pPlayerTo) {
            CItemCmdBuilder_Svr::Build_r2c_trade_cancel(m_oId);
            pPlayerTo->RecvCmd(g_sendCmd);
            pPlayerTo->m_pTrade = NULL;
        }
        DEL_ONE(m_pTrade);
    }

    if (m_scene)
        m_scene->RemoveCreature(m_oId);

    g_factory->DestroyObject(this);
    if (m_ai)  //tim.yang
        DeleteAI();
    if (m_pet) {
        m_pet->DeleteAI();
        g_factory->DestroyObject(m_pet);
    }

    //one region//only!
    if (m_pTeam != NULL) {
        //m_pTeam->OnOffline();//下线
        //m_pTeam->SelfLeave();
        DEL_ONE(m_pTeam);
        m_pTeam = NULL;
    }

    if (m_pMail != NULL) {
        DEL_ONE(m_pMail);
        m_pMail = NULL;
    }

    if (m_pFriend != NULL) {
        m_pFriend->OnOffline();  //下线
        DEL_ONE(m_pFriend);
        m_pFriend = NULL;
    }
}

void CRegionCreature::PetSetInfo(int rate, bool counterAttack) {
    // check is valid rate 20,35,50
    if (rate != 20 && rate != 35 && rate != 50) {
        return;
    }

    m_petCounterAttack = counterAttack;
    m_petExpRate       = rate;
}

bool CRegionCreature::RidePet() {
    if (IsRidePet())
        return false;
    if (!m_pet || !m_pet->m_ai)
        return false;

    if (GetPlayerLogic()->IsInPShop()) {
        return false;  //摆摊状态不能骑
    }
    //<add by Tian>
    SPet* info = g_TablePet.FindPet(m_pet->m_npcId);
    if (!info) {
        g_factory->DestroyObject(m_pet);
        return false;
    }
    if (m_pet->m_core.Lev < 35)
        return false;
    if (!info->isride)
        return false;

    if (m_flag & OBJECT_FLAG_DESTROY)
        return false;
    if (/*PZH*/ /*m_bDead*/ !GetPlayerLogic()->IsLive())
        return false;

    if (m_pet->m_flag & OBJECT_FLAG_DESTROY)
        return false;
    if (/*PZH*/ /*m_pet->m_bDead*/ !m_pet->GetPlayerLogic()->IsLive())
        return false;

    m_bRidePet = true;

    m_pet->ForceSetPosition(m_pos[0], m_pos[1], m_pos[2]);

    // 骑乘技能的属性加成
    // ...
    SSkill* skill;
    skill = m_Skill.FindSkillBySubID(17);
    if (skill) {
        // 必须和rideoff中同步
        // m_pet->m_core.MovSpeed.Temp += (skill->iLevel-1) * 10;
        m_pet->m_core.DamageMin.Rate += skill->iLevel * 0.25;
        m_pet->m_core.DamageMax.Rate += skill->iLevel * 0.25;
        m_pet->m_core.Armor.Rate += skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_WATER].Rate += skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_FIRE].Rate += skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_POISON].Rate += skill->iLevel * 0.15;
    }

    //人物移动速度=宠物的移动速度
    m_core.DamageMin.Rate *= 1.2f;
    m_core.DamageMax.Rate *= 1.2f;
    m_core.Armor.Rate *= 1.5f;
    m_core.MaxHp.Rate *= 1.2f;
    m_core.MaxMp.Rate *= 1.2f;
    m_core.Attack.Rate *= 1.2f;
    m_core.Dodge.Rate *= 1.2f;
    m_core.ElementArmor[ELEMENT_WATER].Rate *= 1.5f;
    m_core.ElementArmor[ELEMENT_FIRE].Rate *= 1.5f;
    m_core.ElementArmor[ELEMENT_POISON].Rate *= 1.5f;
    m_core.ElementDamage[ELEMENT_WATER].Rate *= 1.5f;
    m_core.ElementDamage[ELEMENT_FIRE].Rate *= 1.5f;
    m_core.ElementDamage[ELEMENT_POISON].Rate *= 1.5f;
    RecomputeAttr();

    int     page, i, j;
    SItemID pet = ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet;
    if (!pet.params[PET_PARAM_STATE]) {
        CBag* pPetBag = &m_pItemContainer->m_PetBag;
        if (pPetBag->Find(pet, &page, &i, &j)) {
            pet.params[PET_PARAM_STATE] = 1;
            if (pPetBag->SetItem(page, i, j, pet)) {
                ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet = pet;
                CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                                   page, i, j, pet);
                RecvCmd(g_sendCmd);
            }
        }
    }
    //m_pet->DeleteAI();
    m_pet->m_ai->SetActive(false);

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_ride_ret);
    g_sendCmd->WriteByte(RIDE_ON);
    g_sendCmd->WriteLong(m_pet->m_oId);
    RecvCmd(g_sendCmd);

    //const int AFTER_RIDE_PET_IGNORE_SPEED_CHECK_TIME = 2;
    //m_mvChk.Ignore(AFTER_RIDE_PET_IGNORE_SPEED_CHECK_TIME);

    return true;
}

bool CRegionCreature::RideOffPet(bool bResetState /* = true*/) {
    if (!IsRidePet())
        return false;
    if (!m_pet)
        return false;

    m_bRidePet = false;
    //m_pet->SetAI("pet");
    //m_pet->ActiveAI();
    if (m_pet->m_ai) {
        m_pet->m_ai->SetActive(true);
        m_pet->m_ai->OnFollow(this);
    }

    int     page, i, j;
    SItemID pet = ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet;
    if (pet.params[PET_PARAM_STATE]) {
        CBag* pPetBag = &m_pItemContainer->m_PetBag;
        if (pPetBag->Find(pet, &page, &i, &j)) {
            if (bResetState)
                pet.params[PET_PARAM_STATE] = 0;
            if (pPetBag->SetItem(page, i, j, pet)) {
                ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet = pet;
                CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                                   page, i, j, pet);
                RecvCmd(g_sendCmd);
            }
        }
    }

    SSkill* skill;
    skill = m_Skill.FindSkillBySubID(17);
    if (skill) {
        // 必须和rideon中同步
        // m_pet->m_core.MovSpeed.Temp -= (skill->iLevel-1) * 10;
        m_pet->m_core.DamageMin.Rate -= skill->iLevel * 0.25;
        m_pet->m_core.DamageMax.Rate -= skill->iLevel * 0.25;
        m_pet->m_core.Armor.Rate -= skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_WATER].Rate -= skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_FIRE].Rate -= skill->iLevel * 0.15;
        m_pet->m_core.ElementArmor[ELEMENT_POISON].Rate -= skill->iLevel * 0.15;
    }
    //人物移动速度=宠物的移动速度
    m_core.DamageMin.Rate /= 1.2f;
    m_core.DamageMax.Rate /= 1.2f;
    m_core.Armor.Rate /= 1.5f;
    m_core.MaxHp.Rate /= 1.2f;
    m_core.MaxMp.Rate /= 1.2f;
    m_core.Attack.Rate /= 1.2f;
    m_core.Dodge.Rate /= 1.2f;
    m_core.ElementArmor[ELEMENT_WATER].Rate /= 1.5f;
    m_core.ElementArmor[ELEMENT_FIRE].Rate /= 1.5f;
    m_core.ElementArmor[ELEMENT_POISON].Rate /= 1.5f;
    m_core.ElementDamage[ELEMENT_WATER].Rate /= 1.5f;
    m_core.ElementDamage[ELEMENT_FIRE].Rate /= 1.5f;
    m_core.ElementDamage[ELEMENT_POISON].Rate /= 1.5f;
    RecomputeAttr();

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_ride_ret);
    g_sendCmd->WriteByte(RIDE_OFF);
    g_sendCmd->WriteLong(m_pet->m_oId);
    RecvCmd(g_sendCmd);

    return true;
}

//
// Tianh   增加势力声望
//
//g_TablePopularity

//
// add Cul to user
// Tianh   增加修为
//
void CRegionCreature::AddCulExp(int Cul) {
    static int PLAYER_CULMAX_LEVEL = 0;

    if (m_core.MetierLev == 0) {
        if (m_core.GradeLev >= 39) {
            return;
        }
    } else if (m_core.MetierLev == 1) {
        if (m_core.GradeLev >= 79) {
            return;
        }
    }

    if (Cul == 0)
        return;
    if (m_core.CulExp < 0)
        m_core.CulExp = 0;

    bool CulLevUp = false, CulLevDown = false;

    //在这里没有给GM 做保护 只是玩家正常状态
    if (m_core.Cul >= 4163724330) {
        Cul = 0;
    }
    if (Cul > 0) {
        m_core.CulExp += Cul;
        m_core.Cul += Cul;
        OriginalCulLevel = m_core.GradeLev;
        while (m_core.CulExp >= g_TableExp.GetNextLevCulExp(m_core.GradeLev - 1)) {
            if (m_core.MetierLev == 0) {
                if (m_core.GradeLev >= 39) {
                    return;
                }
            } else if (m_core.MetierLev == 1) {
                if (m_core.GradeLev >= 79) {
                    return;
                }
            }

            if (m_core.GradeLev > m_core.GetGradeMaxLev())
                break;
            // Cullevel up
            CulLevUp = true;
            m_core.CulExp -= g_TableExp.GetNextLevCulExp(m_core.GradeLev - 1);
            /*玩家升级前等级*/
            m_core.GradeLev++;
            ReComputePetBagSize();
        }
        if (CulLevUp) {
            if (IsScriptLoaded())
                CallScript("CulLevelUp", false, m_core.GradeLev - OriginalCulLevel);
            g_region->BuildEventCulLevelUp(g_sendCmd, this);
            m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  //广播
        }

        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_display);
            g_sendCmd->WriteShort(XIUWEI);
            g_sendCmd->WriteLong(GetObjectId());
            g_sendCmd->WriteLong(Cul);
            RecvCmd(g_sendCmd);
        }

    }
    //4.8 LeiJun 减修为实现
    else if (Cul < 0) {
        OriginalCulLevel = m_core.GradeLev;
        m_core.GradeLev  = 1;
        ReComputePetBagSize();
        m_core.CulExp += Cul;
        if (m_core.CulExp < 0)
            m_core.CulExp = 0;
        m_core.Cul += Cul;
        if (m_core.Cul < 0)
            m_core.Cul = 0;
        while (m_core.CulExp >= g_TableExp.GetNextLevCulExp(m_core.GradeLev + 1)) {
            if (m_core.GradeLev > m_core.GetGradeMaxLev())
                break;
            m_core.CulExp -= g_TableExp.GetNextLevCulExp(m_core.GradeLev + 1);
            ++m_core.GradeLev;
            ReComputePetBagSize();
        }
        //没有Cul下降事件，所以……
        //if(m_core.GradeLev < OriginalCulLevel)
        //{
        //	g_region->BuildEventCulLevelUp(g_sendCmd,this);
        //	m_scene->BroadcastCmd(m_pos,DEFAULT_RANGE,g_sendCmd,NULL); //广播
        //}
    }
    AttChangeMessageToClient(Cul > 0 ? "MSG_ADD_CUL" : "MSG_DEL_CUL", Cul);
}

//
// add exp to user,then add to pet and magic weapon
//
//lmk 增加经验
void CRegionCreature::AddExp(int Exp, int SP) {
    //if(m_core.Lev >= 90) return;//rem by fox

    ////测试版本等级上现为35级
    //if (IsUser())
    //{
    //	if(m_core.Lev >= 99)
    //		return;
    //}

    //<add by fox>
    static int          PLAYER_MAX_LEVEL = 0;
    static int          PET_MAX_LEVEL    = 0;
    static CIniVariable PlayerMaxLevel(GetGameRule(), "GameLevelRule", "PlayerMaxLevel",
                                       PLAYER_MAX_LEVEL);
    static CIniVariable PetMaxLevel(GetGameRule(), "GameLevelRule", "PetMaxLevel", PET_MAX_LEVEL);
    //</add by fox>

    SP = 0;  // 重置技能点数为 0
    // 如果经验和技能点数都为 0，则不做任何操作，直接返回
    if (Exp == 0 && SP == 0) {
        return;
    }

    //发送系统消息
    if (Exp > 0) {
        // 经验增加，发送增加经验的系统消息
        rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_EXP), Exp);
        SendBattleInfoMessage(g_strStaticBuffer);
    } else if (Exp < 0) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_REMOVE_EXP), -Exp);
        SendBattleInfoMessage(g_strStaticBuffer);
    }

    if (m_pItemContainer)  //当玩家装备经验瓶时，只给经验瓶加经验，其他不加
    {
        // 获取装备的饰品项
        SItemID  neckItem = m_pItemContainer->GetEquipItem(CItemContainerBase::CHARM, true);
        SShiPin* pShi     = (SShiPin*)g_region->m_pItemManager->GetItem(neckItem.type);
        int      neckId   = g_region->m_pItemManager->GetIndexFromType(neckItem.type);
        // 判断饰品是否为经验瓶
        if (pShi && (neckId == 4306 || neckId == 4515)) {
            // 给经验瓶增加经验
            neckItem.params[MW_PARAM_EXP] += Exp;
            int  createid, maxExp;
            char createname[20];
            switch (neckId) {
                case 4306:
                    createid = 4307;
                    maxExp   = BOTTLE_EXP_MAX;
                    rt2_strcpy(createname, "满经验瓶");
                    break;
                case 4515:
                    createid = 4516;
                    maxExp   = TIANYANCHUXING;
                    rt2_strcpy(createname, "充满能量的天眼雏形");
                    break;
            }
            // 如果经验瓶的经验已满
            if (neckItem.params[MW_PARAM_EXP] >= maxExp) {
                Exp = neckItem.params[MW_PARAM_EXP] - maxExp;
                int page, i, j;

                SItemID bottle = g_region->m_pItemManager->CreateItem(createid);
                // 创建满经验瓶物品并添加到背包中
                if (ItemID_IsValid(bottle)) {
                    if (AddItem(bottle)) {
                        // 发送获得满经验瓶的系统消息
                        rt2_sprintf(g_strStaticBuffer, R(MSG_AUCTION_GET_ITEM), createname);
                        SendBattleInfoMessage(g_strStaticBuffer);
                    } else {
                        // 背包满了，发送背包满的系统消息
                        rt2_sprintf(g_strStaticBuffer, R(IMSG_BAG_FULL));
                        SendBattleInfoMessage(g_strStaticBuffer);
                        return;
                    }
                }
                // 解除装备的经验瓶
                if (!m_pItemContainer->UnEquip(CItemContainerBase::CHARM, false)) {
                    ERR1("unequip bottle exp error~!!~!!!");
                    return;
                }
                m_pItemContainer->UpdateItemToClient(CItemContainerBase::CHARM);
                m_pItemContainer->OnCreatureAttribChanged(CItemContainerBase::CHARM);
            } else {
                // 更新经验瓶的装备信息
                if (!m_pItemContainer->SetEquipItem(CItemContainerBase::CHARM, neckItem)) {
                    ERR1("set equip error~!!~!!!");
                    return;
                }
                // 发送经验瓶增加经验的系统消息
                rt2_sprintf(g_strStaticBuffer, R(MSG_BOTTLE_ADD_EXP), Exp,
                            maxExp - neckItem.params[MW_PARAM_EXP]);
                SendBattleInfoMessage(g_strStaticBuffer);
                return;
            }
        }
    }

    // 如果宠物存在且等级未达到最大值，则宠物获得经验
    //(0 - PET_MAX_LEVEL)开区间能获得经验
    if (m_pet && m_pet->m_ai && m_pet->m_core.Lev < PET_MAX_LEVEL)  //change by fox
    {
        SItemID& petItem = ((CRegionPetAI*)(m_pet->m_ai))->m_pItemPet;
        if ((ItemIsPetSpectre(petItem) && petItem.params[PET_PARAM_STATE])) {
            // 幽鬼附体状态不加经验
        } else {
            float rate =
                ((float)GetRealLevel(m_pet->m_core.Lev)) / ((float)GetRealLevel(m_core.Lev));
            // 限制比例最大为1
            if (rate * 100 > 100) {
                rate = 100 / 100.f;
            }
            // 计算宠物实际获得的经验
            int add = GetRand(rate * Exp * 1.1, rate * Exp * 0.9);
            m_pet->AddExp(add, SP);
            // 如果获得的经验不为 0，则发送宠物获得经验的系统消息
            if (add != 0) {
                rt2_sprintf(g_strStaticBuffer, R(MSG_PET_GETEXPERIENCE), add);
                SendBattleInfoMessage(g_strStaticBuffer);
            }

            m_pet->SavePetDataToItem(this);
        }
    }

    //(0 - PLAYER_MAX_LEVEL)开区间能获得经验
    // 如果玩家等级已达到最大值且经验为正，则不增加经验
    if (m_core.Lev >= PLAYER_MAX_LEVEL && Exp >= 0)
        return;  //add by fox

    m_nGetExp += Exp;  // 增加玩家的获取经验总量

    // magic weapon
    // add here
    //if (m_pItemContainer)
    //{
    // SItemID item = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
    // SWeapon* pWea = (SWeapon*)g_region->m_pItemManager->GetItem(item.type);
    // if(ItemIsWeapon_MagicWeapon(item.type) && pWea)
    // {
    //  float rate = ((float)GetRealLevel(item.params[MW_PARAM_LEV])) / ((float)GetRealLevel(m_core.Lev));
    //  if(rate > 1) rate = 1;
    //  if(rate < 0) rate = 0;

    //  item.params[MW_PARAM_EXP] += rate * Exp;
    //  bool LevUp = false;
    //  while(item.params[MW_PARAM_EXP] >= g_TableExp.GetNextLevExp(item.params[MW_PARAM_LEV]))
    //  {
    //   if(item.params[MW_PARAM_LEV] >= pWea->nMwMaxLevel) break;
    //   // level up
    //   LevUp = true;
    //   item.params[MW_PARAM_EXP] -= g_TableExp.GetNextLevExp(item.params[MW_PARAM_LEV]);
    //   item.params[MW_PARAM_LEV]++;
    //  }
    //
    //  SWeapon *pItemClass = (SWeapon *)g_region->m_pItemManager->GetItem(item.type);
    //  int begin = pItemClass->requirement[ItemRequirement_Level];
    //  int end = pWea->nMwMaxLevel;
    //  if(end - begin != 0)
    //  {
    //            item.level = ((float)item.params[MW_PARAM_LEV] - begin)/(end - begin)*10.0f;
    //   if(item.level < 0 )
    //    item.level = 0;
    //   if(item.level > ITEM_MAX_ITEM_LEVEL)
    //    item.level = ITEM_MAX_ITEM_LEVEL;
    //  }
    //  else
    //  {
    //   ERR1("magic weapon [%d] begin == end", item.type);
    //  }
    //  if(LevUp)
    //  {
    //   // notify client magic weapon levelup
    //   // g_region->BuildEventLevelUp(g_sendCmd,this);
    //   // m_scene->BroadcastCmd(m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
    //  }
    //  UpdateMagicWeapon(item);
    // }
    //}

    // send client msg
    if (Exp != 0) {   // 如果经验不为 0，则调整防沉迷状态下的经验
        if (Exp > 0)  //ac.ma  for防沉迷
            Exp *= m_iIncomeState * 0.01;
        //rt2_sprintf(g_strStaticBuffer, R(MSG_ADD_EXP), Exp);
        //SendSystemMessage(g_strStaticBuffer);
    }
    // 确保玩家经验值不为负
    if (m_core.Exp < 0)
        m_core.Exp = 0;

    bool LevUp           = false;  // 玩家是否升级
    bool LevDown         = false;  // 玩家是否降级
    int  nLevelupCount   = 0;      // 升级次数
    int  nLevelDownCount = 0;      // 降级次数
                                   // 如果经验为正，处理升级逻辑
    if (Exp > 0) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_display);
        g_sendCmd->WriteShort(JINGYAN);
        g_sendCmd->WriteLong(GetObjectId());
        g_sendCmd->WriteLong(Exp);
        RecvCmd(g_sendCmd);

        m_core.Exp += Exp;             // 增加玩家当前经验值
        OriginalLevel   = m_core.Lev;  // 记录玩家原本的等级
        long NeedExpLev = 0;
        // 根据是否为宠物计算下一级所需的经验值
        if (IsUserPet()) {
            NeedExpLev = g_TableExp.GetNextLevExp(m_core.Lev) / 3;
        } else {
            NeedExpLev = g_TableExp.GetNextLevExp(m_core.Lev);
        }
        // 如果当前经验值超过了下一级所需经验值，则处理升级
        while (m_core.Exp >= NeedExpLev) {
            if (m_core.Lev > MAX_LEVEL)
                break;
            // level up
            LevUp = true;              // 标记为升级
            m_core.Exp -= NeedExpLev;  // 减去升级所需的经验值

            /*玩家升级前等级*/
            m_core.Lev++;     // 提升等级
            nLevelupCount++;  // 记录升级次数

            if (IsUser()) {  // 如果是玩家，增加修为并发送升级消息
                AddCulExp(g_TableExp.GetLevCulTable(m_core.Lev - 1));

                rt2_sprintf(g_strStaticBuffer, R(MSG_CHARACTER_LEVELUP), m_core.Lev);
                SendBattleInfoMessage(g_strStaticBuffer);
            }

            if (IsUserPet()) {  // 如果是宠物，重新计算升级所需经验值并发送宠物升级消息
                NeedExpLev = g_TableExp.GetNextLevExp(m_core.Lev) / 3;

                rt2_sprintf(g_strStaticBuffer, R(MSG_PET_LEVELUP), m_core.Lev);
                SendBattleInfoMessage(g_strStaticBuffer);
            } else {
                NeedExpLev = g_TableExp.GetNextLevExp(m_core.Lev);
            }
        }
        if (LevUp) {                 // 如果发生了升级
            Levelup(nLevelupCount);  // 执行升级操作，并传递升级次数
            if (IsUser()) {
                m_pFriend->SelfDataChangeAffectOther();  // 如果是用户角色，通知好友数据发生了变化
            }
        } else {
            // UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);// 如果没有发生升级，可以选择更新客户端的属性信息
        }
    }
    if (Exp < 0) {          // 如果经验值为负，表示经验减少
        m_core.Exp -= Exp;  // 从当前经验值中减去负的经验值
        // 当当前经验值大于等于当前等级前一个等级所需经验值时
        while (m_core.Exp >= g_TableExp.GetNextLevExp(m_core.Lev - 1)) {
            if (m_core.Lev < 1)  // 如果等级小于 1，则退出循环
                break;
            LevDown    = true;  // 标记发生了降级
            m_core.Exp = 0;     // 将当前经验值清零
            m_core.Lev--;       // 降低等级
            nLevelDownCount++;  // 增加降级计数
        }
        if (LevDown) {                   // 如果发生了降级
            LevelDown(nLevelDownCount);  // 执行降级操作，并传递降级次数
        }
    }
}

void CRegionCreature::Levelup(int iUpCount) {
    if (iUpCount <= 0)  // 如果升级次数小于等于0，直接返回
        return;

    // 如果是用户角色或宠物角色，执行以下操作
    if (IsUser() || IsUserPet()) {
        for (int ii = 0; ii < iUpCount; ii++)  // 针对每次升级调用 OnLevelUp 方法
            OnLevelUp();
        if (IsScriptLoaded())                          // 如果脚本已经加载
            CallScript("OnLevelup", false, iUpCount);  // 调用脚本的 OnLevelup 方法
    }

    // 构建并广播角色升级事件
    g_region->BuildEventLevelUp(g_sendCmd, this);                  // 构建升级事件的命令
    m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);  // 广播事件到场景中

    g_PrenticeMgr.OnAfterUserLevelup(this, iUpCount);  // 调用师徒管理器处理用户升级后的操作

    // 通知用户所属的国家成员
    if (m_userInfo && m_unionID != 0) {
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();  // 开始构建网络包
        packet.WriteShort(r2g_union);                        // 写入协议标识
        packet.WriteShort(c2r_union_player_levelup);         // 写入操作类型（玩家升级）
        packet.WriteLong(m_userInfo->m_userId);              // 写入用户ID
        packet.WriteShort(m_core.Lev);                       // 写入当前等级
        g_region->m_gws.EndSend();                           // 结束构建并发送网络包
    }

    long NowLevel = m_core.Lev;  // 获取当前等级
    if (m_userInfo && m_unionID != 0) {
        /* 升级后记录到日志 */
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();  // 开始构建网络包
        packet.WriteShort(r2g_log_event);                    // 写入协议标识
        packet.WriteByte(PLT_LevelChanges);                  // 写入日志类型（等级变化）
        packet.WriteLong(m_userInfo->m_userId);              // 写入用户ID
        packet.WriteString(m_userInfo->m_userIpAddress.c_str());  // 写入用户IP地址
        packet.WriteLong(OriginalLevel);                          // 写入原始等级
        packet.WriteLong(NowLevel);                               // 写入当前等级
        packet.WriteLong(iUpCount);                               // 写入升级次数
        g_region->m_gws.EndSend();                                // 结束构建并发送网络包
    }

    GetPlayerLogic()->OnLevelUp();  // 调用玩家逻辑层的 OnLevelUp 方法
}

void CRegionCreature::LevelDown(int iDownCount) {
    if (iDownCount >= 0)
        return;

    if (IsUser() || IsUserPet()) {
        for (int ii = 0; ii < iDownCount; ii++)
            OnLevelDown();
        if (IsScriptLoaded())
            CallScript("OnLevelDown", false, iDownCount);
    }

    g_region->BuildEventLevelUp(g_sendCmd, this);
    m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);

    g_PrenticeMgr.OnAfterUserLevelDown(this, iDownCount);

    // 通知诸侯国内成员
    if (m_userInfo && m_unionID != 0) {
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_union);
        packet.WriteShort(c2r_union_player_levelup);
        packet.WriteLong(m_userInfo->m_userId);
        packet.WriteShort(m_core.Lev);
        g_region->m_gws.EndSend();
    }
}

void CRegionCreature::AddGold(int Gold) {
    //增加金币获得率
    Gold = Gold + Gold * (m_core.RateGold.GetValue() / 100);
    if (Gold == 0) {
        return;
    }
    if (Gold > 0) {
        Gold *= m_iIncomeState * 0.01;
        if (m_pItemContainer && m_pItemContainer->AddMoney(Gold)) {
            CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(Gold, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                        ITEM_REASON_PICK);
            RecvCmd(g_sendCmd);
        }
    } else {
        if (m_pItemContainer && m_pItemContainer->RemoveMoney(-Gold)) {
            CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(Gold, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                        ITEM_REASON_PICK);
            RecvCmd(g_sendCmd);
        }
    }
}

long CRegionCreature::GetGold() {
    if (!m_pItemContainer)
        return 0;
    return m_pItemContainer->GetMoney();
}

bool CRegionCreature::RemoveGold(int remove) {
    if (!m_pItemContainer)
        return false;

    if (m_pItemContainer->GetMoney() < remove)
        return false;

    if (m_pItemContainer->RemoveMoney(remove)) {
        CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(-remove, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                    ITEM_REASON_PICK);
        RecvCmd(g_sendCmd);
        return true;
    } else
        return false;
}

void CRegionCreature::AddCredit(int add) {
    m_core.Credit += add;
    if (m_core.Credit < 0)
        m_core.Credit = 0;

    AttChangeMessageToClient(add > 0 ? "MSG_ADD_CREDIT" : "MSG_REMOVE_CREDIT", add);
}

void CRegionCreature::AddExploit(int add) {
    m_core.Exploit += add;
    if (m_core.Exploit < 0)
        m_core.Exploit = 0;

    if (add > 0) {
        rt2_sprintf(g_strStaticBuffer, R(您获得 % d战功), add);
        SendBattleInfoMessage(g_strStaticBuffer);
    }
}

//tim.yang  神灯
bool CRegionCreature::GodLightCreateMonster_AddMonster(CRegionCreature* target, byte monsterCount) {
    if (monsterCount > GODLIGHT_MONSTER_COUNT)
        monsterCount = GODLIGHT_MONSTER_COUNT;
    int count = 0;
    for (int i = 0; i < monsterCount; ++i) {
        m_GodLightMonster[i] = g_region->CreateNpc(1960);
        if (!m_GodLightMonster[i]) {
            return false;
        }
        char dir = rand() % 255;
        long pos[3];
        memcpy(pos, m_pos, sizeof(pos));
        if (!g_region->m_defScene->AddCreature(m_GodLightMonster[i], pos, dir)) {
            g_factory->DestroyObject(m_GodLightMonster[i]);
            m_GodLightMonster[i] = 0;
            return false;
        }
        m_GodLightMonster[i]->m_ai->OnBeAttacked(target, 10000);
        count++;
    }
    if (count == monsterCount)
        m_monsterCount = monsterCount;
    else
        m_monsterCount = count;
    m_IsAddMonster = 1;
    SendSystemMessage(R(MSG_GODLIGHT_ADDMONSTER));
    return true;
}

void CRegionCreature::GodLightCreateMonster_DelAllMonster() {
    if (m_bIsDead || m_bIsOutOfLine) {
        for (int i = 0; i < GODLIGHT_MONSTER_COUNT; ++i) {
            if (m_GodLightMonster[i] && m_GodLightMonster[i]->m_scene) {
                m_GodLightMonster[i]->Quit();
                m_GodLightMonster[i] = 0;
                if (m_monsterCount)
                    m_monsterCount--;
            }
        }
        if (m_monsterCount == 0) {
            m_IsAddMonster = 0;
            m_bIsDead      = 0;
            m_bIsOutOfLine = 0;
            m_FBID         = 0;
            m_bIsFBReborn  = 0;
        }
    }
}

void CRegionCreature::GodLightCreateMonster(CRegionCreature* target) {
    if (!GodLightCreateMonster_AddMonster(target, 2))
        return;
}

void CRegionCreature::GodLightDropItem() {
    //SItemID item = g_region->m_pItemManager->CreateItem(5000);//灯油
    //if(ItemID_IsValid(item))
    //{
    //	CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
    //	dummy->m_item = item;
    //	dummy->m_dropStyle = ItemDrop_ParabolaNear;
    //	if(!m_scene->AddItem(dummy,m_pos,0))
    //		g_factory->DestroyObject(dummy);
    //}
    //SendSystemMessage(R(MSG_GODLIGHT_DROPITEM));
}

void CRegionCreature::GodLightCreateLight() {
    //SItemID item = g_region->m_pItemManager->CreateItem(1113);//指路明灯
    //if(ItemID_IsValid(item))
    //{
    //	CRegionItem *dummy = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
    //	dummy->m_item = item;
    //	dummy->m_dropStyle = ItemDrop_ParabolaNear;
    //	dummy->m_item.TimeLimit = time(NULL) + 5 * 60;//时效5分钟
    //	if(!m_scene->AddItem(dummy,m_pos,0))
    //		g_factory->DestroyObject(dummy);
    //}
    //SendSystemMessage(R(MSG_GODLIGHT_DROPLIGHT));
}

//void CRegionCreature::RemoveHuiYuanHoptoad()
//{
//	g_region->m_defScene->RemoveCreature(((CRegionObject*)m_pHuiYuanHoptoad)->m_oId);
//	g_factory->DestroyObject(m_pHuiYuanHoptoad);
//	if (m_pHuiYuanHoptoad)
//	{
//		m_pHuiYuanHoptoad->Quit();
//	}
//	m_pHuiYuanHoptoad = 0;
//	m_HuiYuanHoptoadCount = 0;
//	m_curHuiYuanHoptoadOid = 0;
//}

void CRegionCreature::FinishMCTask() {
    int hortationItemId = 0;
    if (m_core.Metier == METIER_WARRIOR) {
        hortationItemId = 4406;
    } else if (m_core.Metier == METIER_HUNTER) {
        hortationItemId = 4407;
    } else if (m_core.Metier == METIER_TAOIST) {
        hortationItemId = 4408;
    } else if (m_core.Metier == METIER_WIZARD) {
        hortationItemId = 4409;
    }
    //int hortationItemId = rtRandom(4410,4406);
    SItemID item = g_region->m_pItemManager->CreateItem(hortationItemId);  //MC任务奖励
    if (ItemID_IsValid(item)) {
        AddItem(item);
    }
}

SItemID CRegionCreature::CheckEquipment(DWORD typeId, EItemColor color,
                                        CItemContainerBase::EEquipParts* part) {
    for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
        SItemID ret = m_pItemContainer->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
        if (ItemID_IsValid(ret) && CItemManager::GetIndexFromType(ret.type) == typeId &&
            (color == ret.color || color == ItemColor_All)) {
            if (part)
                *part = (CItemContainerBase::EEquipParts)i;

            return ret;
        }
    }

    return SItemID();
}

SItemID CRegionCreature::CheckEquipment(DWORD Id, CItemContainerBase::EEquipParts* part) {
    for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
        SItemID ret = m_pItemContainer->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
        if (ItemID_IsValid(ret) && ret.id == Id) {
            if (part)
                *part = (CItemContainerBase::EEquipParts)i;

            return ret;
        }
    }

    return SItemID();
}

bool CRegionCreature::UseItem(
    int page, int gridI, int gridJ,
    CItemContainerBase::EEquipParts part /* = CItemContainerBase::EQUIP_DEFAULT_PART*/,
    bool                            bBagItem /* = true*/) {
    if (!m_pItemContainer)
        return false;

    SItemID item;
    if (bBagItem)
        item = m_pItemContainer->m_Bag.GetItem(page, gridI, gridJ);
    else
        item = m_pItemContainer->GetEquipItem(part, true);

    if (m_pTrade || m_pPShop)  // 交易和摆摊状态下不能使用道具
        return false;

    if (GetPlayerLogic()->GetLiveType() != CPlayerLogic::eLive)
        return false;

    if (0 == this->GetPlayerLogic()->UseTaskItem(page, gridI, gridJ)) {
        return true;
    }

    // 检查道具的使用需求是否满足
    if (!(ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item) /*PZH*/ ||
          ItemIsTrump(item) || ItemIsFashion(item) || ItemIsGem(item) /**/)) {
        if (!m_pItemContainer->CheckUse(item, m_core))
            return false;
    }

    //tim.yang  会员功能
    //SItemBase* pItem = g_region->m_pItemManager->GetItem(item.type);
    int index = g_region->m_pItemManager->GetIndexFromType(item.type);
    //if(4401 == index && 1!=m_HuiYuanHoptoadCount)
    //{
    //	m_HuiYuanHoptoadCount = 1;
    //	m_pHuiYuanHoptoad = g_region->CreateNpc(1958);
    //	if(!m_pHuiYuanHoptoad)
    //	{
    //		return false;
    //	}
    //	m_curHuiYuanHoptoadOid = m_pHuiYuanHoptoad->GetObjectId();
    //	char dir = rand() % 255;
    //	long pos[3];
    //	memcpy(pos,m_pos,sizeof(pos));
    //	if(!m_scene->AddCreature(m_pHuiYuanHoptoad, pos, dir))
    //	{
    //		g_factory->DestroyObject(m_pHuiYuanHoptoad);
    //		m_pHuiYuanHoptoad = NULL;
    //	}
    //	return true;
    //}
    //end
    //if (735 == index || 4627 == index || 4628 == index)
    //{
    //	WriteAccountTime(index);
    //}
    //else
    // 可装备道具(不可重叠摆放道具)
    if (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item) /*PZH*/ ||
        ItemIsTrump(item) || ItemIsFashion(item) || ItemIsGem(item) /**/) {
        if (bBagItem)  // Equip
        {
            // CHECK UP
            // 符合装备道具的条件
            // 道具栏有足够的空格放脱下来的道具（有可能会脱掉2个道具）

            DWORD dwTime = rtGetMilliseconds();

            // 特殊情况：
            if (part == CItemContainerBase::WEAPON_2) {
                SItemID item1 = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
                if (ItemID_IsValid(item1)) {
                    SWeapon* pWeaClass1 = (SWeapon*)g_region->m_pItemManager->GetItem(item1.type);
                    if (pWeaClass1 && pWeaClass1->bBothHands)
                        part = CItemContainerBase::WEAPON_1;
                } else {
                    part = CItemContainerBase::WEAPON_1;
                }
            }

            if (part == CItemContainerBase::EQUIP_DEFAULT_PART)
                part = m_pItemContainer->GetDefaultEquipPart(item);

            if (!m_pItemContainer->CheckEquip(part, item, m_core))
                return false;

            int nUnEquipCount = m_pItemContainer->GetUnEquipItemCount(item, part);
            if (nUnEquipCount == 2) {
                if (m_pItemContainer->m_Bag.IsFull())
                    return false;
            }

            if (item.cBind == IB_BindWhenPick || item.cBind == IB_BindWhenEquip)
                item.cBind = IB_Binded;

            SItemID                         itemUnEquip1, itemUnEquip2;
            CItemContainerBase::EEquipParts partUnEquip1, partUnEquip2;
            if (!m_pItemContainer->Equip(item, part, false, &itemUnEquip1, &partUnEquip1,
                                         &itemUnEquip2, &partUnEquip2)) {
                //SItemID itemTmp = m_pItemContainer->GetEquipItem(part, true);
                //bool bValidTmp = m_pItemContainer->GetEquipValid(part);
                //ERR6("EquipFail, Dump Status:\n\tEquip=(id=%d,index=%d,part=%d)\n\tPartItem=(id=%d,index=%d,valid=%d)\n",
                //	item.id, CItemManager::GetIndexFromType(item.type), part, itemTmp.id, CItemManager::GetIndexFromType(itemTmp.type), bValidTmp?1:0);
                //ERR("装备失败\n");
                return false;
            }
            if (!m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ)) {
                ERR("[cmd_c2r_player_equipitem] error 1\n");
                return false;
            }
            int pageUnEquip1, iUnEquip1, jUnEquip1;
            if (ItemID_IsValid(itemUnEquip1) &&
                !m_pItemContainer->m_Bag.AddItem(itemUnEquip1, &pageUnEquip1, &iUnEquip1,
                                                 &jUnEquip1)) {
                ERR("[cmd_c2r_player_equipitem] error 2\n");
                return false;
            }
            int pageUnEquip2, iUnEquip2, jUnEquip2;
            if (ItemID_IsValid(itemUnEquip2) &&
                !m_pItemContainer->m_Bag.AddItem(itemUnEquip2, &pageUnEquip2, &iUnEquip2,
                                                 &jUnEquip2)) {
                ERR("[cmd_c2r_player_equipitem] error 3\n");
                return false;
            }

            m_core.RecomputeAttr();
            if (m_pItemContainer->ItemCanChangeAttrib(item)) {
                m_pItemContainer->OnCreatureAttribChanged();
            }
            m_Skill.OnAttributeChanged();

            if (ItemID_IsValid(itemUnEquip1) && ItemIsTrump(itemUnEquip1)) {
                for (int i = TRUMP_SKILL_1; i < TRUMP_SKILL_MAX; i++) {
                    SSkill* pSkill = Skill()->FindSkill(itemUnEquip1.params[i]);
                    if (pSkill) {
                        m_Skill.DeleteSkill(itemUnEquip1.params[i], true);
                    }
                }
            }

            if (ItemIsTrump(item)) {  // 加入法宝技能
                for (int i = TRUMP_SKILL_1; i < TRUMP_SKILL_MAX; i++) {
                    SSkill* pSkill = Skill()->FindSkill(item.params[i]);
                    if (pSkill) {
                        m_Skill.AddSkill(item.params[i], 0, true);
                    }
                }
            }

            // 通知Player，去掉此物品
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                          gridI, gridJ, ITEM_REASON_EQUIP);
            RecvCmd(g_sendCmd);

            if (ItemID_IsValid(itemUnEquip1)) {
                // 通知Player，增加换掉的装备
                CItemCmdBuilder_Svr::Build_r2c_bag_additem(
                    itemUnEquip1, ITEM_CONTAINER_PLAYER_COMMON_BAG, pageUnEquip1, iUnEquip1,
                    jUnEquip1, ITEM_REASON_UNEQUIP);
                RecvCmd(g_sendCmd);
            }
            if (ItemID_IsValid(itemUnEquip2)) {
                // 通知Player，增加换掉的装备
                CItemCmdBuilder_Svr::Build_r2c_bag_additem(
                    itemUnEquip2, ITEM_CONTAINER_PLAYER_COMMON_BAG, pageUnEquip2, iUnEquip2,
                    jUnEquip2, ITEM_REASON_UNEQUIP);
                RecvCmd(g_sendCmd);
            }

            //			LOG1("Equip use time: %d\n", rtGetMilliseconds()-dwTime);
        } else  // UnEquip
        {
            if (m_pItemContainer->m_Bag.FreeSpace() == 0)
                return false;

            if (m_pItemContainer->UnEquip(part)) {
                int p, i, j;
                if (!m_pItemContainer->m_Bag.AddItem(item, &p, &i, &j)) {
                    ERR("UnEquip success, but can NOT put the item into bag\n");
                    m_pItemContainer->Equip(item, part);
                    return false;
                }

                m_core.RecomputeAttr();
                if (m_pItemContainer->ItemCanChangeAttrib(item)) {
                    m_pItemContainer->OnCreatureAttribChanged();
                }
                m_Skill.OnAttributeChanged();

                if (ItemIsTrump(item)) {
                    for (int i = TRUMP_SKILL_1; i < TRUMP_SKILL_MAX; i++) {
                        SSkill* pSkill = Skill()->FindSkill(item.params[i]);
                        if (pSkill) {
                            m_Skill.DeleteSkill(item.params[i], true);
                        }
                    }
                }

                CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                           p, i, j, ITEM_REASON_UNEQUIP);
                RecvCmd(g_sendCmd);
            }
        }  // UnEquip

    }
    // 药瓶    Tianh 10.06.19  重写
    else if (ItemIsYaoPing(item)) {
        CRegionCreature* entity = GetEntity();
        if (ItemIsHPBottle(item) && entity->m_core.GetHp() == entity->m_core.GetMaxHp())
            return false;
        if (ItemIsMPBottle(item) &&
            (m_mpRemain > 0 || m_core.GetMp() == m_core.GetMaxMp() || IsRidePet()))
            return false;
        if (ItemIsPetHpBottle(item)) {
            if (!m_pet) {
                return false;
            } else {
                if (m_pet->m_core.GetHp() == m_pet->m_core.GetMaxHp()) {
                    return false;
                }
            }
        }
        if (ItemIsPetMpBottle(item)) {
            if (!m_pet) {
                return false;
            } else {
                if (m_pet->m_core.GetMp() == m_pet->m_core.GetMaxMp()) {
                    return false;
                }
            }
        }
        if (ItemIsEnergyBottle(item) && entity->m_core.GetEnP() == entity->m_core.GetMaxEnP())
            return false;

        SItem* pItem = g_region->m_pItemManager->GetItem(item);
        if (!pItem)
            return false;

        unsigned long dwTime = rtGetMilliseconds();

        if (ItemIsHPBottle(item))  //血瓶
        {
            if (dwTime < GetPlayerLogic()->GetCanUseYaoPingHpTime()) {
                return false;
            }
        }
        if (ItemIsMPBottle(item))  //蓝瓶
        {
            if (dwTime < GetPlayerLogic()->GetCanUseYaoPingMpTime()) {
                return false;
            }
        }
        if (ItemIsPetHpBottle(item))  //宠物药
        {
            if (dwTime < GetPlayerLogic()->GetCanUseYaoPingPetHpTime()) {
                return false;
            }
        }
        if (ItemIsPetMpBottle(item))  //宠物药
        {
            if (dwTime < GetPlayerLogic()->GetCanUseYaoPingPetMpTime()) {
                return false;
            }
        }
        if (ItemIsEnergyBottle(item))  //能量瓶
        {
            if (dwTime < GetPlayerLogic()->GetCanUseYaoPingEnergyTime()) {
                return false;
            }
        }

        if (ItemIsCardMinuteBottle(item)) {
            if (IsUseCardMinBottleOverday()) {
                InitCardMinBottleParam();
            }
            if (CanUseCardMinBottle()) {
                if (!AddCardMinute(((SYaoPing*)pItem->pItemBase)->nCardMinute)) {
                    return false;
                } else {
                    AddUseCardMinBottleNum();
                }
            } else {
                return false;
            }
        }

        if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
            return false;

        if (ItemIsHPBottle(item)) {
            if (IsRidePet()) {
                if (pItem->pYaoPing->nHPRate != 0) {
                    entity->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    entity->m_hpRecover = pItem->pYaoPing->nHPRate / 3.0f / 100.0f *
                                          entity->m_core.GetMaxHp() / entity->m_hpRemain;
                } else {
                    entity->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    entity->m_hpRecover = pItem->pYaoPing->nHP * 1.5 / entity->m_hpRemain;
                }
            } else {
                if (pItem->pYaoPing->nHPRate != 0) {
                    entity->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    entity->m_hpRecover = pItem->pYaoPing->nHPRate / 100.0f *
                                          entity->m_core.GetMaxHp() / entity->m_hpRemain;
                } else {
                    entity->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    entity->m_hpRecover = pItem->pYaoPing->nHP / entity->m_hpRemain;
                }
            }
            GetPlayerLogic()->SetCanUseYaoPingHpTime(rtGetMilliseconds() + 10000);
        } else if (ItemIsMPBottle(item)) {
            if (!IsRidePet()) {
                if (pItem->pYaoPing->nMPRate != 0) {
                    m_mpRemain = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    m_mpRecover =
                        pItem->pYaoPing->nMPRate / 100.0f * m_core.GetMaxMp() / m_mpRemain;
                    m_mpRemain--;
                    m_core.AddMp(m_mpRecover);
                } else {
                    m_mpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                    m_mpRecover = pItem->pYaoPing->nMP / m_mpRemain;
                    m_mpRemain--;
                    m_core.AddMp(m_mpRecover);
                }
            }
            GetPlayerLogic()->SetCanUseYaoPingMpTime(rtGetMilliseconds() + 10000);
        } else if (ItemIsEXPBottle(item)) {
            AddExp(((SYaoPing*)pItem->pItemBase)->nExp, 0);
        } else if (ItemIsPetHpBottle(item) && m_pet) {

            if (pItem->pYaoPing->nHPRate != 0) {
                m_pet->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                m_pet->m_hpRecover = pItem->pYaoPing->nHPRate / 3.0f / 100.0f *
                                     m_pet->m_core.GetMaxHp() / m_pet->m_hpRemain;
            } else {
                m_pet->m_hpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                m_pet->m_hpRecover = pItem->pYaoPing->nHP * 1.5 / m_pet->m_hpRemain;
            }
            GetPlayerLogic()->SetCanUseYaoPingPetHpTime(rtGetMilliseconds() + 10000);
        } else if (ItemIsPetMpBottle(item) && m_pet) {

            if (pItem->pYaoPing->nMPRate != 0) {
                m_pet->m_mpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                m_pet->m_mpRecover = pItem->pYaoPing->nMPRate / 3.0f / 100.0f *
                                     m_pet->m_core.GetMaxMp() / m_pet->m_mpRemain;
            } else {
                m_pet->m_mpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                m_pet->m_mpRecover = pItem->pYaoPing->nMP * 1.5 / m_pet->m_mpRemain;
            }
            GetPlayerLogic()->SetCanUseYaoPingPetMpTime(rtGetMilliseconds() + 10000);
        } else if (ItemIsEnergyBottle(item)) {
            if (pItem->pYaoPing->nEnpRate != 0) {
                this->m_EnpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                this->m_EnpRecover = pItem->pYaoPing->nEnpRate / 3.0f / 100.0f *
                                     this->m_core.GetMaxEnP() / this->m_EnpRemain;
            } else {
                this->m_EnpRemain  = pItem->pYaoPing->nTime / HEARTBEAT_INTERVAL;
                this->m_EnpRecover = pItem->pYaoPing->nEnp * 1.5 / this->m_EnpRemain;
            }
            GetPlayerLogic()->SetCanUseYaoPingEnergyTime(rtGetMilliseconds() + 10000);
        }

        CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
        RecvCmd(g_sendCmd);
    }
    // 宠物(不可重叠摆放道具)
    else if (ItemIsPet(item)) {
        // if(m_hero) return false;

        if (m_pet) {
            bool releasePet = false;
            if (m_pet->m_dwPetItemOID != item.id) {
                releasePet = true;
                ;
            }
            if (releasePet) {
                m_usePetBusy = 0;
                ReleasePet(item);
            }
        } else
            ReleasePet(item);

        CItemCmdBuilder_Svr::Build_r2c_player_use_pet(m_oId, item, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                      page, gridI, gridJ);
        RecvCmd(g_sendCmd);
    }
    // 神仙(不可重叠摆放道具)
    else if (ItemIsOther_Hero(item.type)) {
        DWORD tTime = time(NULL);
        if (item.params[ITEMHERO_PARAM_LIFE] != 0 && item.params[ITEMHERO_PARAM_LIFE] < tTime) {
            return false;
        }

        // if(m_pet)
        // {
        // SendSystemMessage(R(MSG_HAVE_PET));
        // return false;
        // }

        if (m_hero) {
            CallbackHero();
        } else {
            ReleaseHero(item);
        }
    }
    // 技能令牌
    else if (ItemIsSkill(item)) {
        SItemSkill* pItemSkill = g_region->m_pItemManager->GetItem(item)->pSkill;
        if (!pItemSkill)
            return false;
        DWORD skillID = pItemSkill->skillID;

        SSkill* pSkill = Skill()->FindSkill(skillID);
        if (!pSkill)
            return false;

        //<add by Tianh>
        CActorSkill* pActorSkill = m_Skill.FindActorSkill(this->m_oId);
        if (!pActorSkill) {
            return false;
        }
        std::list<CActorSkill::SActorEffect>::iterator itEffect;

        for (itEffect = pActorSkill->m_listEffect.begin();
             itEffect != pActorSkill->m_listEffect.end(); itEffect++) {
            if ((&(*itEffect))->pAttr->wID ==
                pSkill->effectID)  //如果还有此技能的持续效果  则不能再使用
            {
                rt2_sprintf(g_strStaticBuffer, R(MSG_ITEM_USING));
                SendSystemMessage(g_strStaticBuffer);
                return false;
            }
        }
        //<add by Tianh>

        bool bIsAdded = false;
        if (!m_Skill.FindSkillBySubID(pSkill->wSubID)) {
            bIsAdded = true;
            if (!m_Skill.AddSkill(skillID, 0, false))
                return false;
        }

        SAttackParam param;
        long         ID = m_oId;

        if (pSkill->dwRTarget == SKILL_TARGET_PET)  //对宠物使用
        {
            if (!m_pet) {
                return false;
            }
            if (m_pet->m_core.GetHp() == m_pet->m_core.GetMaxHp()) {
                rt2_sprintf(g_strStaticBuffer, R(MSG_PET_NONEEDTORECOVER));
                SendSystemMessage(g_strStaticBuffer);
                return false;
            }
            ID = m_pet->m_oId;
        }
        CRegionCreature* target = m_scene->FindCreature(ID);
        param.TargetType        = GetTargetParty(target);
        param.Skill             = skillID;
        param.EnemyId           = ID;
        UseSkill(pSkill, param);

        if (bIsAdded) {
            m_Skill.DeleteSkill(skillID, false);
        }

        // 删除道具
        if (ItemCanPile(item.type)) {
            if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                return false;
            CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
            RecvCmd(g_sendCmd);
        } else {
            m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                          gridI, gridJ, ITEM_REASON_DELETE);
            RecvCmd(g_sendCmd);
        }
    }
    // 回城卷轴
    else if (ItemIsScrollHome(item.type)) {
        int iCanMove = CheckCanMove(page, gridI, gridJ, item);
        if (iCanMove > 0)
            return false;
        char     ePKState = GetPKState();
        float    fRebirthX, fRebirthY;
        SScroll* pItem = g_region->m_pItemManager->GetItem(item)->pScroll;
        if (pItem->MovePosX != 0 && pItem->MovePosY != 0) {
            LOG3("玩家[%s], 传送到点[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), pItem->MovePosX,
                 pItem->MovePosY);
            int           iRx, iRy;
            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_move_to);
            packet.WriteLong(m_userInfo->m_userId);
            // 			m_scene->m_pTerrain->GetTerrainByPosFast(pItem->MovePosX, pItem->MovePosY, iRx, iRy);
            //
            // 			iRx += rand() % 40 - 20;
            // 			iRy += rand() % 40 - 20;

            packet.WriteLong((long)pItem->MovePosX);
            packet.WriteLong((long)pItem->MovePosY);
            g_region->m_gws.EndSend();
        } else {
            // 参考Dead()函数中的代码：
            RtsSceneBlockRtb* pBlockMap =
                (m_iCurrentBlockX < 0) ? (NULL)
                                       : (m_scene->m_pTerrain->GetBlockMap(
                                             m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                             m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY()));
            // 如果在地图中，就丢到指定的位置
            if (pBlockMap) {
                std::string szBlockName(pBlockMap->m_szFileName);
                SSceneInfo* pSceneInfo = g_TableScene.FindScene(szBlockName);
                if (pSceneInfo) {
                    // 在自己的地盘或者公共的地盘死亡的
                    if (pSceneInfo->sceneFaction == m_core.Faction ||
                        (pSceneInfo->sceneFaction != FACTION_ZHOU &&
                         pSceneInfo->sceneFaction != FACTION_SHANG)) {
                        if (ePKState == PK_STATE_RED_NAME) {
                            switch (rtRandom(5, 0)) {
                                case 0:
                                    fRebirthX = pSceneInfo->fRebirthRedX1;
                                    fRebirthY = pSceneInfo->fRebirthRedY1;
                                    break;
                                case 1:
                                    fRebirthX = pSceneInfo->fRebirthRedX2;
                                    fRebirthY = pSceneInfo->fRebirthRedY2;
                                    break;
                                case 2:
                                    fRebirthX = pSceneInfo->fRebirthRedX3;
                                    fRebirthY = pSceneInfo->fRebirthRedY3;
                                    break;
                                case 3:
                                    fRebirthX = pSceneInfo->fRebirthRedX4;
                                    fRebirthY = pSceneInfo->fRebirthRedY4;
                                    break;
                                default:
                                case 4:
                                    fRebirthX = pSceneInfo->fRebirthRedX5;
                                    fRebirthY = pSceneInfo->fRebirthRedY5;
                                    break;
                            }
                        } else {
                            switch (rtRandom(3, 0)) {
                                case 0:
                                    fRebirthX = pSceneInfo->fRebirth1X;
                                    fRebirthY = pSceneInfo->fRebirth1Y;
                                    break;
                                case 1:
                                    fRebirthX = pSceneInfo->fRebirth2X;
                                    fRebirthY = pSceneInfo->fRebirth2Y;
                                    break;
                                default:
                                case 2:
                                    fRebirthX = pSceneInfo->fRebirth3X;
                                    fRebirthY = pSceneInfo->fRebirth3Y;
                                    break;
                            }
                            if (g_RegionCastleWarMgr.IsCastleWarTime("XiQi") &&
                                (m_core.Faction == FACTION_ZHOU))
                                return false;
                            if (g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe") &&
                                (m_core.Faction == FACTION_SHANG))
                                return false;
                            //------------add end   by tony 05.07.08-------------//
                        }
                    } else  // 在对方的地盘死亡的
                    {
                        if (ePKState == PK_STATE_RED_NAME) {
                            if (pSceneInfo->sceneFaction == FACTION_SHANG) {
                                fRebirthX = g_pkConfig->fSafeRebirthShangOtherX;
                                fRebirthY = g_pkConfig->fSafeRebirthShangOtherY;
                            } else  // pSceneInfo->sceneFaction==FACTION_ZHOU
                            {
                                fRebirthX = g_pkConfig->fSafeRebirthZhouOtherX;
                                fRebirthY = g_pkConfig->fSafeRebirthZhouOtherY;
                            }
                        } else {
                            switch (rtRandom(3, 0)) {
                                case 0:
                                    fRebirthX = pSceneInfo->fX_Oppose1;
                                    fRebirthY = pSceneInfo->fY_Oppose1;
                                    break;
                                case 1:
                                    fRebirthX = pSceneInfo->fX_Oppose2;
                                    fRebirthY = pSceneInfo->fY_Oppose2;
                                    break;
                                default:
                                case 2:
                                    fRebirthX = pSceneInfo->fX_Oppose3;
                                    fRebirthY = pSceneInfo->fY_Oppose3;
                                    break;
                            }
                        }
                    }
                } else {
                    pBlockMap = NULL;
                }
            }
            // 否则丢到出生位置
            if (pBlockMap == NULL) {
                if (m_core.Faction == FACTION_ZHOU)  // 周
                {
                    fRebirthX = g_pkConfig->fSafeRebirthZouX;
                    fRebirthY = g_pkConfig->fSafeRebirthZouY;
                } else  // if (m_core.Faction==FACTION_SHANG) // 商
                {
                    fRebirthX = g_pkConfig->fSafeRebirthShangX;
                    fRebirthY = g_pkConfig->fSafeRebirthShangY;
                }
            }

            // 已经取出地点了，现在丢过去
            LOG3("玩家[%s], 传送到点[%.0f,%.0f]\n", m_userInfo->m_username.c_str(), fRebirthX,
                 fRebirthY);
            int           iRx, iRy;
            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_move_to);
            packet.WriteLong(m_userInfo->m_userId);
            m_scene->m_pTerrain->GetTerrainByPosFast(fRebirthX, fRebirthY, iRx, iRy);

            iRx += rand() % 40 - 20;
            iRy += rand() % 40 - 20;

            packet.WriteLong((long)iRx);
            packet.WriteLong((long)iRy);
            g_region->m_gws.EndSend();
        }
    } else if (ItemIsOther(item.type)) {
        if ((m_UseItemEggTime + 5 * 1000) <= rtGetMilliseconds()) {
            m_UseItemEggTime = rtGetMilliseconds();
        } else {
            long Time = ((m_UseItemEggTime + 5 * 1000) - rtGetMilliseconds()) / 1000;
            rt2_sprintf(g_strStaticBuffer, R(MSG_ITEM_AFTERSECONDUSE), Time);
            SendSystemMessage(g_strStaticBuffer);

            return false;
        }

        SOtherItem* pItem = g_region->m_pItemManager->GetItem(item)->pOtherItem;
        if (!pItem)
            return false;

        if (ItemIsOther_Magic(item.type)) {
            SItemID          newItem;
            int              nMoney   = 0;
            int              nExp     = 0;
            int              nSoul    = 0;
            EMagicItemResult miResult = MI_Null;

            // 随机选择出奖励
            SMagicItemReward* pReward = NULL;
            if ((pReward = g_region->m_pItemManager->m_pMagicEggRewardMgr->FindReward(
                     atoi(pItem->param1.c_str()))) == NULL) {
                return false;
            }

            double RandHit      = (rand() % 10000) / 100.0;
            int    nSelectIndex = 0;

            for (nSelectIndex = 0; nSelectIndex < (int)pReward->HitNum.size(); nSelectIndex++) {
                if (RandHit < pReward->HitNum[nSelectIndex])
                    break;
            }
            if (nSelectIndex >= (int)pReward->HitNum.size()) {
                nSelectIndex = 0;
            }

            // 给出奖励  Tianh 09.03.09
            if (pReward->RewardType[nSelectIndex] == MI_ALL) {
                int FreeBag   = m_pItemContainer->m_Bag.FreeSpace();
                int ItemCount = pReward->vMaxCount[nSelectIndex];
                if (ItemCount > FreeBag) {
                    rt2_sprintf(g_strStaticBuffer, R(MSG_CLEAR_BAG_ITEM));
                    SendSystemMessage(g_strStaticBuffer);
                    return false;
                }
                for (nSelectIndex = 0; nSelectIndex < (int)pReward->HitNum.size(); nSelectIndex++) {
                    if (pReward->HitNum[nSelectIndex] < 0 ||
                        !GetProb(pReward->HitNum[nSelectIndex])) {
                        if (pReward->HitNum[nSelectIndex] != 100) {
                            continue;
                        }
                    }
                    miResult                   = MI_ALL;
                    DWORD      RewardItemID    = pReward->RewardParam[nSelectIndex];
                    EItemColor RewardItemColor = ItemColor_Green;
                    if (GetProb(pReward->BlueRate[nSelectIndex])) {
                        RewardItemColor = ItemColor_Blue;

                        if (GetProb(pReward->GoldenRate[nSelectIndex])) {
                            RewardItemColor = ItemColor_Golden;
                        }
                    }

                    //增加开出奖励为绑定  Tianh IB_NotBind
                    newItem = g_region->m_pItemManager->CreateItem(RewardItemID, RewardItemColor, 0,
                                                                   IB_Binded);
                    if (ItemID_IsValid(newItem)) {
                        if (!AddItem(newItem)) {
                            return false;
                        }
                    }
                }
                if (nSelectIndex >= (int)pReward->HitNum.size()) {
                    nSelectIndex = 0;
                }
            } else if (nSelectIndex >= 0 && nSelectIndex < (int)pReward->HitNum.size()) {
                if (pReward->RewardType[nSelectIndex] == MI_Money) {
                    miResult = MI_Money;
                    nMoney   = pReward->RewardParam[nSelectIndex];

                    m_pItemContainer->AddMoney(nMoney);
                    CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(
                        nMoney, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
                    RecvCmd(g_sendCmd);
                } else if (pReward->RewardType[nSelectIndex] == MI_Exp) {
                    miResult = MI_Exp;
                    nExp     = pReward->RewardParam[nSelectIndex];

                    AddExp(nExp, 0);
                } else if (pReward->RewardType[nSelectIndex] == MI_SOUL) {
                    miResult = MI_SOUL;
                    nSoul    = pReward->RewardParam[nSelectIndex];

                    m_core.AddSoul(nSoul);
                } else if (pReward->RewardType[nSelectIndex] == MI_Item) {
                    miResult      = MI_Item;
                    int FreeBag   = m_pItemContainer->m_Bag.FreeSpace();
                    int ItemCount = pReward->vMaxCount[nSelectIndex];
                    if (ItemCount > FreeBag) {
                        rt2_sprintf(g_strStaticBuffer, R(MSG_CLEAR_BAG_ITEM));
                        SendSystemMessage(g_strStaticBuffer);
                        return false;
                    }
                    DWORD      RewardItemID    = pReward->RewardParam[nSelectIndex];
                    EItemColor RewardItemColor = ItemColor_White;
                    if (GetProb(pReward->GreenRate[nSelectIndex]))
                        RewardItemColor = ItemColor_Green;
                    if (GetProb(pReward->BlueRate[nSelectIndex]))
                        RewardItemColor = ItemColor_Blue;
                    if (GetProb(pReward->GoldenRate[nSelectIndex]))
                        RewardItemColor = ItemColor_Golden;

                    //增加开出奖励为绑定
                    newItem = g_region->m_pItemManager->CreateItem(RewardItemID, RewardItemColor, 0,
                                                                   IB_Binded);
                    if (ItemID_IsValid(newItem)) {
                        if (!AddItem(newItem)) {
                            return false;
                        }
                    }
                } else {
                    ERR1("Magic Item Reward Null(User=%s)!\n", m_userInfo->m_username.c_str());
                }
            }

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }

            // log到数据库
            std::string strObject, strEgg;
            strEgg = rtFormatNumber(GetItemSerializeVersion());
            strEgg += ",";
            strEgg += item.SaveToString(g_region->m_pItemManager);
            if (miResult == MI_Money) {
                strObject = rtFormatNumber(nMoney);
            } else if (miResult == MI_Item) {
                strObject = rtFormatNumber(GetItemSerializeVersion());
                strObject += ",";
                strObject += newItem.SaveToString(g_region->m_pItemManager);
            } else if (miResult == MI_Exp) {
                strObject = rtFormatNumber(nExp);
            } else {
            }
            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_log_event);
            packet.WriteByte(PLT_MagicEgg);
            packet.WriteLong(m_userInfo->m_userId);
            packet.WriteString((char*)m_userInfo->m_userIpAddress.c_str());  // userIP
            packet.WriteLong(miResult);
            packet.WriteString((char*)strObject.c_str());
            packet.WriteString((char*)strEgg.c_str());
            g_region->m_gws.EndSend();
        } else if (ItemIsOther_Task(item.type)) {
            if (CanAcceptTask(pItem->taskID)) {
                AcceptTask(pItem->taskID);
                // 删除道具
                if (ItemCanPile(item.type)) {
                    if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                        return false;
                    CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                    RecvCmd(g_sendCmd);
                } else {
                    m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                    CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                        ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                    RecvCmd(g_sendCmd);
                }
            }
        } else if (ItemIsOther_Flag(item.type)) {
            //--------------------add start by tony 05.06.07--------------------------------------------//
            // 插令旗
            // 判断
            //是否可插令旗
            if (!g_RegionCastleWarMgr.CanInsertFlag(this)) {
                return false;
            }

            // 生成NPC
            if (!g_RegionCastleWarMgr.CreateFlagNpc(this)) {
                return false;
            }

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
            //--------------------add start by tony 05.06.07--------------------------------------------//
        } else if (ItemIsOther_Effect(item.type)) {
            string EffectName;
            char   Style;
            DWORD  ActorID;
            // long PosX, PosY;
            if (pItem->effect[0] != '\0') {
                EffectName = pItem->effect[0];
                Style      = 1;
                ActorID    = m_oId;

                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(r2c_play_effect);
                g_sendCmd->WriteByte(Style);
                g_sendCmd->WriteString(pItem->effect);
                g_sendCmd->WriteLong(ActorID);
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
            }

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearStr(item.type)) {
            SUserActor* actor = g_TableUserActor.FindUserActor(m_core.Metier, 0);
            if (!actor)
                return false;
            if (m_core.Str.Base <= actor->BaseStr) {
                SendSystemMessage(R(MSG_CLEARPOINT_MIN));
                return false;
            }
            m_core.Str.Base--;
            m_core.AttrPoint++;
            m_pItemContainer->OnCreatureAttribChanged();
            RecomputeAttr();

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearDex(item.type)) {
            SUserActor* actor = g_TableUserActor.FindUserActor(m_core.Metier, 0);
            if (!actor)
                return false;
            if (m_core.Dex.Base <= actor->BaseDex) {
                SendSystemMessage(R(MSG_CLEARPOINT_MIN));
                return false;
            }
            m_core.Dex.Base--;
            m_core.AttrPoint++;
            m_pItemContainer->OnCreatureAttribChanged();
            RecomputeAttr();

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearHit(item.type)) {
            SUserActor* actor = g_TableUserActor.FindUserActor(m_core.Metier, 0);
            if (!actor)
                return false;
            if (m_core.Hit.Base <= actor->BaseHit) {
                SendSystemMessage(R(MSG_CLEARPOINT_MIN));
                return false;
            }
            m_core.Hit.Base--;
            m_core.AttrPoint++;
            m_pItemContainer->OnCreatureAttribChanged();
            RecomputeAttr();

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearCon(item.type)) {
            SUserActor* actor = g_TableUserActor.FindUserActor(m_core.Metier, 0);
            if (!actor)
                return false;
            if (m_core.Con.Base <= actor->BaseCon) {
                SendSystemMessage(R(MSG_CLEARPOINT_MIN));
                return false;
            }
            m_core.Con.Base--;
            m_core.AttrPoint++;
            m_pItemContainer->OnCreatureAttribChanged();
            RecomputeAttr();

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearInt(item.type)) {
            SUserActor* actor = g_TableUserActor.FindUserActor(m_core.Metier, 0);
            if (!actor)
                return false;
            if (m_core.Int.Base <= actor->BaseInt) {
                SendSystemMessage(R(MSG_CLEARPOINT_MIN));
                return false;
            }
            m_core.Int.Base--;
            m_core.AttrPoint++;
            m_pItemContainer->OnCreatureAttribChanged();
            RecomputeAttr();

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_ClearSkill(item.type)) {
            int nMaxUseLevel = atoi(pItem->param1.c_str());
            if (m_core.Lev > nMaxUseLevel)
                return false;

            if (IsChengBody()) {
                rt2_sprintf(g_strStaticBuffer, R(MSG_CANNOT_USEITEMWHILETRANSFORM));
                SendSystemMessage(g_strStaticBuffer);
                return false;
            }
            m_Skill.ClearFightSkill(true);

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else if (ItemIsOther_Spawn(item.type))  // 用来召唤生物的令牌
        {
            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }

            // 添加NPC
            DWORD       npc_id                   = atoi(pItem->param1.c_str());
            std::string npc_name                 = "";
            static int  Default_Faction          = 0;
            static int  Default_unitedPoint      = 1;
            static int  Default_loyalPoint       = 1;
            static int  Default_bravePoint       = 1;
            static int  Default_angryPointLevel1 = 30;
            static int  Default_angryPointLevel2 = 70;

            CRegionCreature* cre    = g_region->CreateNpc(npc_id);
            cre->m_faction          = Default_Faction;
            cre->m_unitedPoint      = Default_unitedPoint;
            cre->m_loyalPoint       = Default_loyalPoint;
            cre->m_bravePoint       = Default_bravePoint;
            cre->m_angryPointLevel1 = Default_angryPointLevel1;
            cre->m_angryPointLevel2 = Default_angryPointLevel2;

            if (m_scene->AddCreature(cre, m_pos, m_dir)) {
                if (cre->IsScriptLoaded())
                    cre->CallScript("OnCreate", false);
            } else {
                g_factory->DestroyObject(cre);
                cre = NULL;
            }

            EArmsKind eType = g_WarfieldMgr.IsArmsNpc(npc_id);
            if (eType != eNone) {
                //CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
                //cmd.WriteShort(r2g_warfield_create_arms);
                //cmd.WriteLong(m_userInfo->m_userId);
                //cmd.WriteLong(m_core.Lev);
                //cmd.WriteLong((long)eType);
                //g_region->m_gws.EndSend();

                int iExploit[3] = {2, 5, 30};
                g_WarfieldMgr.AddPerTmpExploit(m_userInfo->m_userId, iExploit[(long)eType]);
            }
        } else if (ItemIsOther_NwTeleport(item.type))  // （国战）随机传送
        {
            // 计算位置
            const char* NwTeleportPos_Zhou[4]  = {"zhou_sys_1_1", "zhou_sys_1_2", "zhou_sys_1_3",
                                                  "zhou_sys_1_4"};
            const char* NwTeleportPos_Shang[4] = {"shang_sys_1_1", "shang_sys_1_2", "shang_sys_1_3",
                                                  "shang_sys_1_4"};
            int         PosIndex               = rand() % 4;
            RtsAreaHelper* pAreaHelper         = NULL;
            if (m_core.Faction == FACTION_ZHOU)
                pAreaHelper = m_scene->m_pArea->FindArea(NwTeleportPos_Shang[PosIndex]);
            else if (m_core.Faction == FACTION_SHANG)
                pAreaHelper = m_scene->m_pArea->FindArea(NwTeleportPos_Zhou[PosIndex]);
            else
                return false;

            if (!pAreaHelper)
                return false;
            int TelX = (pAreaHelper->lMaxX + pAreaHelper->lMinX) / 2;
            int TelY = (pAreaHelper->lMaxY + pAreaHelper->lMinY) / 2;

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }

            // 传送
            if (m_pTeam && m_pTeam->mData.Size() > 0) {
                std::vector<CRegionCreature*> Creatures;
                m_scene->GetAroundCreatureByGrid(m_pos, 200, Creatures, this);
                std::vector<CRegionCreature*>::iterator iterCre = Creatures.begin();
                for (; iterCre != Creatures.end(); iterCre++) {
                    CRegionCreature* pCreTemp = *iterCre;
                    //if (pCreTemp == this)   continue;
                    if (!pCreTemp->m_userInfo)
                        continue;
                    if (pCreTemp->m_core.Faction != m_core.Faction)
                        continue;

                    if (m_pTeam->mData.Find(pCreTemp->m_userInfo->m_userId)) {
                        pCreTemp->ForceSetPosition(TelX, TelY, 0);
                    }
                }
            } else {
                ForceSetPosition(TelX, TelY, 0);
            }
        } else if (ItemIsOther_NwSkill(item.type))  // （国战）加速卷轴
        {
            if (IsRidePet()) {
                SendSystemMessage(R(骑宠物状态下不能使用));
                return false;
            }

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }

            // 全队提速50%，持续60秒，不能和加速药水一起使用，不能在骑兽时使用
            const WORD SkillID = atoi(pItem->param1.c_str());

            // 释放技能到玩家身上
            SSkill* pSkill = Skill()->FindSkill(SkillID);
            if (!pSkill)
                return false;

            SAttackParam param;
            param.TargetType = SKILL_TARGET_TYPE_ACTOR;
            param.Skill      = SkillID;

            std::vector<CRegionCreature*> creatures;
            if (m_pTeam && m_pTeam->mData.Size() > 0) {
                int teamIdx = m_pTeam->mData.Begin();
                for (; teamIdx < m_pTeam->mData.End(); teamIdx++) {
                    CRegionUser* pUserTeam = g_region->FindUser(m_pTeam->mData[teamIdx].mDBID);
                    if (pUserTeam && pUserTeam->m_dummy)
                        creatures.push_back(pUserTeam->m_dummy);
                }
            } else {
                creatures.push_back(this);
            }

            stlforeach(std::vector<CRegionCreature*>, iterCre, creatures) {
                CRegionCreature* pCre = *iterCre;
                if (pCre->IsRidePet())
                    continue;
                if (Distance(pCre->m_pos) > 200)
                    continue;

                bool bIsAdded = false;
                if (!pCre->m_Skill.FindSkillBySubID(pSkill->wSubID)) {
                    bIsAdded = true;
                    if (!pCre->m_Skill.AddSkill(SkillID, 0, false))
                        return false;
                }

                param.EnemyId = pCre->m_oId;
                pCre->UseSkill(pSkill, param);

                if (bIsAdded) {
                    pCre->m_Skill.DeleteSkill(SkillID, false);
                }
            }
        } else if (ItemIsOther_NwCallTeam(item.type))  // （国战）将组队成员传送至使用者身边
        {
            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }

            // 将组队成员传送至使用者身边
            if (m_pTeam && m_pTeam->mData.Size() > 0) {
                int teamIdx = m_pTeam->mData.Begin();
                for (; teamIdx < m_pTeam->mData.End(); teamIdx++) {
                    if (m_pTeam->mData[teamIdx].mDBID == m_userInfo->m_userId)
                        continue;

                    CRegionUser* pUserTeam = g_region->FindUser(m_pTeam->mData[teamIdx].mDBID);
                    if (m_core.Faction != pUserTeam->m_dummy->m_core.Faction)
                        continue;

                    if (pUserTeam && pUserTeam->m_dummy) {
                        pUserTeam->m_dummy->ForceSetPosition(m_pos[0], m_pos[1], m_pos[2]);
                    }
                }
            }
        } else if (ItemIsOther_BankExtend(item.type)) {
            if (!m_pBank)
                return false;
            if (m_pBank->GetLineCount() >= g_region->m_pItemManager->GetBankMaxLineCount()) {
                SendSystemMessage(R(操作失败，仓库已经达到最大容量));
                return false;
            }

            int line = m_pBank->GetLineCount() + 1;
            if (!m_pBank->Resize(m_pBank->GetPageCount(), line, m_pBank->GetColCount())) {
                SendSystemMessage(R(仓库扩容操作失败));
                return false;
            } else {
                SendSystemMessage(R(仓库扩容操作成功));
            }

            // 删除道具
            if (ItemCanPile(item.type)) {
                if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                    return false;
                CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
                RecvCmd(g_sendCmd);
            } else {
                m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
                CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(
                    ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
                RecvCmd(g_sendCmd);
            }
        } else {
            //ERR1("Invalid Item Operate! type=0x%x\n", item.type);
        }
    } else if (ItemIsScrollCompose(item.type)) {
        SItemBase* pItemClass = g_region->m_pItemManager->GetItem(item.type);
        if (!pItemClass)
            return false;
        // 取得配方资料
        SItemCompose* pComposeClass =
            g_region->m_pItemManager->m_pComposeMgr->FindComposing(pItemClass->composeID);
        if (!pComposeClass)
            return false;
        // 判断是否满足学习配方的条件
        bool    bCanStudy    = false;
        SSkill* pSkill       = NULL;
        WORD    CheckSkillID = 0;
        switch (pComposeClass->ComposeType) {
            case ICT_Casting:
                CheckSkillID = ITEMCOMPOSE_EQUIP_SKILL;
                break;
            case ICT_Equipment:
                CheckSkillID = ITEMCOMPOSE_WEAPON_SKILL;
                break;
            case ICT_Yaoping:
                CheckSkillID = ITEMCOMPOSE_YAOPING_SKILL;
                break;
            case ICT_EnchantScroll:
                CheckSkillID = ITEMCOMPOSE_ENCHANT_SKILL;
                break;
            default:
                break;
        }
        pSkill = m_Skill.FindSkillBySubID(CheckSkillID);
        if (!pSkill || (pSkill->iLevel < pComposeClass->SkillLevel))
            bCanStudy = false;
        else
            bCanStudy = true;
        if (!bCanStudy) {
            CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_unknown_error);
            RecvCmd(g_sendCmd);
            return false;
        }
        // 检查玩家是否已经学会了该配方
        bool                       bStudied    = false;
        std::list<short>::iterator iterCompose = m_listMatchID.begin();
        for (; iterCompose != m_listMatchID.end(); iterCompose++) {
            if (pComposeClass->Idx == (*iterCompose)) {
                bStudied = true;
                break;
            }
        }
        if (bStudied) {
            CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_compose_learned);
            RecvCmd(g_sendCmd);
            return false;
        }

        // 学习配方
        AddMatch(pComposeClass->Idx);
        char cTmp256[256];
        rt2_sprintf(cTmp256, R(MSG_LEARN_PRESCRIPTION), pComposeClass->Name);
        SendSystemMessage(cTmp256);

        // 删除道具
        if (ItemCanPile(item.type)) {
            if (!m_pItemContainer->UsePileItem(page, gridI, gridJ))
                return false;
            CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(item.id, page, gridI, gridJ);
            RecvCmd(g_sendCmd);
        } else {
            m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                          gridI, gridJ, ITEM_REASON_DELETE);
            RecvCmd(g_sendCmd);
        }
    } else {
    }

    //4.2 LeiJun
    int iItemID = g_region->m_pItemManager->GetIndexFromType(item.type);
    if (IsScriptLoaded() && iItemID > 0) {
        char sFuncName[40] = {0};
        rt2_snprintf(sFuncName, 40, "OnUseItem%d", iItemID);
        CallScript(sFuncName, false, item.id);
    }
    return true;
}

bool CRegionCreature::UseItem(DWORD Id) {
    if (!m_pItemContainer)
        return false;

    int page, i, j;
    if (m_pItemContainer->m_Bag.Find(Id, &page, &i, &j)) {
        return UseItem(page, i, j);
    } else {
        // @@@@
        ERR1("Can NOT find the item in bag [id=%d]\n", Id);
        return false;
    }
}

bool CRegionCreature::UseItem(DWORD typeId, EItemColor color) {
    if (!m_pItemContainer)
        return false;

    int page, i, j, count;
    if (m_pItemContainer->m_Bag.FindType(typeId, color, &count, &page, &i, &j)) {
        return UseItem(page, i, j);
    } else {
        // @@@@
        ERR1("Can NOT find the item in bag [type=%d]\n", typeId);
        return false;
    }
}

bool CRegionCreature::AddItem(SItemID item) {
    if (!m_pItemContainer)
        return false;

    bool ret;
    int  page, i, j;
    if (ItemIsMissionItem(item.type))
        ret = m_pItemContainer->m_MissionBag.AddItem(item, &page, &i, &j);
    else if (ItemIsPet(item.type))
        return AddPetByItem(item);
    else
        ret = m_pItemContainer->m_Bag.AddItem(item, &page, &i, &j);

    if (ret) {
        if (ItemIsMissionItem(item))
            CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_MISSION_BAG,
                                                       page, i, j, ITEM_REASON_PICK);
        else
            CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                       i, j, ITEM_REASON_PICK);
        RecvCmd(g_sendCmd);
        return true;
    } else  // lyymark 丢在地上
    {
        CRegionItem *rgItem = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
        if (rgItem)
        {
        	rgItem->m_item = item;
        	rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
        	rgItem->m_masterId = m_oId;
        	if(!m_scene->AddItem(rgItem,m_pos,0))
        	{
        		ERR("道具丢到地上出错\n");
        		g_factory->DestroyObject(rgItem);
        		return false;
        	}
        	return true;
        }
        else
        {
        	return false;
        }
        return false;
    }
}

void CRegionCreature::AddPoint(short sPoint) {
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_add_point);
    packet.WriteLong(m_userInfo->m_userId);
    packet.WriteShort(sPoint);
    g_region->m_gws.EndSend();
}

bool CRegionCreature::AddCardMinute(int minute) {
    int maxCardTime = 200 * 3600;
    if (m_cardTime + minute * 60 > maxCardTime) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_ECARD_TIME_WILL_ENOUGH), m_cardTime / 3600,
                    (m_cardTime % 3600) / 60);
        SendSystemMessage(g_strStaticBuffer);
        return false;
    }
    m_cardTime += minute * 60;
    rt2_sprintf(g_strStaticBuffer, R(MSG_ECARD_ADD_TIME), minute, m_cardTime / 3600,
                (m_cardTime % 3600) / 60);
    SendSystemMessage(g_strStaticBuffer);
    return true;
}

bool CRegionCreature::AddItem(DWORD typeId, EItemColor color) {
    if (!m_pItemContainer)
        return false;

    SItemID item;
    item = g_region->m_pItemManager->CreateItem(typeId, color);
    if (!ItemID_IsValid(item)) {
        g_region->m_pItemManager->DeleteItem(item);
        return false;
    }

    if (!AddItem(item)) {
        g_region->m_pItemManager->DeleteItem(item);
        return false;
    }

    return true;
}

int CRegionCreature::GetItemNum(DWORD typeId, EItemColor color /* = ItemColor_All*/) {
    if (!m_pItemContainer)
        return 0;

    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return 0;

    int count = 0;
    if (ItemIsMissionItem(pItemClass->type)) {
        if (m_pItemContainer->m_MissionBag.FindType(typeId, color, &count))
            return count;
    } else if (ItemIsPet(pItemClass->type)) {
        if (m_pItemContainer->m_PetBag.FindType(typeId, color, &count))
            return count;
    } else {
        if (m_pItemContainer->m_Bag.FindType(typeId, color, &count))
            return count;
    }
    return 0;
}

bool CRegionCreature::RemoveItem(DWORD typeId, int num, EItemColor color /* = ItemColor_All*/) {
    if (!m_pItemContainer)
        return false;

    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return 0;

    bool ret;
    if (ItemIsMissionItem(pItemClass->type))
        ret = m_pItemContainer->m_MissionBag.RemoveItem(typeId, color, num);
    else if (ItemIsPet(pItemClass->type))
        ret = m_pItemContainer->m_PetBag.RemoveItem(typeId, color, num);
    else
        ret = m_pItemContainer->m_Bag.RemoveItem(typeId, color, num);

    if (ret) {
        if (ItemIsMissionItem(pItemClass->type))
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_MISSION_BAG,
                                                       &(m_pItemContainer->m_MissionBag));
        else if (ItemIsPet(pItemClass->type))
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                       &(m_pItemContainer->m_PetBag));
        else
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       &(m_pItemContainer->m_Bag));
        RecvCmd(g_sendCmd);
    }
    return ret;
}

bool CRegionCreature::RemoveItem(DWORD Id, char bMissionBag /* = 0*/, int reason /* = 0*/) {
    if (!m_pItemContainer)
        return false;

    CBag*          pBag = NULL;
    EItemContainer ECon = ITEM_CONTAINER_PLAYER_COMMON_BAG;
    int            page, gridI, gridJ;
    if (!bMissionBag) {
        ECon = ITEM_CONTAINER_PLAYER_COMMON_BAG;
        pBag = &(m_pItemContainer->m_Bag);
    } else if (bMissionBag == 1) {
        ECon = ITEM_CONTAINER_PLAYER_MISSION_BAG;
        pBag = &(m_pItemContainer->m_MissionBag);
    } else if (bMissionBag == 2) {
        ECon = ITEM_CONTAINER_PLAYER_PET_BAG;
        pBag = &(m_pItemContainer->m_PetBag);
    }

    if (!pBag->Find(Id, &page, &gridI, &gridJ)) {
        return false;
    }
    if (!pBag->RemoveItem(page, gridI, gridJ)) {
        return false;
    }
    CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ECon, page, gridI, gridJ, reason);
    RecvCmd(g_sendCmd);
    return true;
}

int CRegionCreature::GetDungeonMissionItemNum(DWORD typeId, int difficulty) {
    if (!m_pItemContainer)
        return 0;

    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return 0;

    int count = 0;
    if (!ItemIsOther_Dungeon(pItemClass->type)) {
        ERR1("[CRegionCreature::GetDungeonMissionItemNum] Item is NOT a dungeon mission "
             "item(index=%d)\n",
             typeId);
        return 0;
    }
    vector<DWORD> paramIndices;
    vector<DWORD> paramParams;
    paramIndices.push_back(0);
    paramParams.push_back(difficulty);
    // MAKE SURE (paramIndices.size()==paramParams.size())
    if (ItemIsMissionItem(pItemClass->type)) {
        if (m_pItemContainer->m_MissionBag.FindTypeWithParams(typeId, paramIndices, paramParams,
                                                              ItemColor_All, &count))
            return count;
    } else if (ItemIsPet(pItemClass->type)) {
        if (m_pItemContainer->m_PetBag.FindTypeWithParams(typeId, paramIndices, paramParams,
                                                          ItemColor_All, &count))
            return count;
    } else {
        if (m_pItemContainer->m_Bag.FindTypeWithParams(typeId, paramIndices, paramParams,
                                                       ItemColor_All, &count))
            return count;
    }
    return 0;
}

bool CRegionCreature::AddDungeonMissionItem(DWORD typeId, int difficulty) {
    SItemID item;
    item = g_region->m_pItemManager->CreateItem(typeId, ItemColor_White);
    if (!ItemID_IsValid(item)) {
        g_region->m_pItemManager->DeleteItem(item);
        return false;
    }

    item.params[0]  = difficulty;
    item.paramCount = 1;

    if (!AddItem(item)) {
        g_region->m_pItemManager->DeleteItem(item);
        return false;
    }

    return true;
}

bool CRegionCreature::RemoveDungeonMissionItem(DWORD typeId, int num, int difficulty) {
    if (!m_pItemContainer)
        return false;

    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return false;
    if (!ItemIsOther_Dungeon(pItemClass->type)) {
        ERR1("[CRegionCreature::RemoveDungeonMissionItem] Item is NOT a dungeon mission "
             "item(index=%d)\n",
             typeId);
        return false;
    }

    vector<DWORD> arrParamIndices;
    vector<DWORD> arrParams;
    arrParamIndices.push_back(0);
    arrParams.push_back(difficulty);
    // MAKE SURE (paramIndices.size()==paramParams.size())

    bool ret;
    if (ItemIsMissionItem(pItemClass->type))
        ret = m_pItemContainer->m_MissionBag.RemoveItemWithParams(typeId, arrParamIndices,
                                                                  arrParams, ItemColor_All, num);
    else if (ItemIsPet(pItemClass->type))
        ret = m_pItemContainer->m_PetBag.RemoveItemWithParams(typeId, arrParamIndices, arrParams,
                                                              ItemColor_All, num);
    else
        ret = m_pItemContainer->m_Bag.RemoveItemWithParams(typeId, arrParamIndices, arrParams,
                                                           ItemColor_All, num);

    if (ret) {
        if (ItemIsMissionItem(pItemClass->type))
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_MISSION_BAG,
                                                       &(m_pItemContainer->m_MissionBag));
        else if (ItemIsMissionItem(pItemClass->type))
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                       &(m_pItemContainer->m_PetBag));
        else
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       &(m_pItemContainer->m_Bag));
        RecvCmd(g_sendCmd);
    }
    return ret;
}

bool CRegionCreature::StopPShop() {
    BOOL_ENSURE(m_pPShop && m_pPShop->Stop());
    DEL_ONE(m_pPShop);
    m_bPShop = 0;
    m_modelRef++;
    CItemCmdBuilder_Svr::Build_r2c_pshop_stop_response(1);
    RecvCmd(g_sendCmd);
    return true;
}

bool CRegionCreature::StopPlayerTrade() {
    if (!m_pTrade)
        return true;

    DWORD            dwTradeWith = m_pTrade->GetOtherPlayer(m_oId);
    CRegionCreature* pTradeWith  = m_scene->FindCreature(dwTradeWith);

    if (!pTradeWith)  // 如果交易对象已经断线或离开了
    {
        CItemCmdBuilder_Svr::Build_r2c_trade_cancel(dwTradeWith);
        RecvCmd(g_sendCmd);

        DEL_ONE(m_pTrade);
        return true;
    } else {
        DWORD dwForCheck = m_pTrade->GetOtherPlayer(pTradeWith->m_oId);
        if (dwForCheck != m_oId) {
            ERR("[CRegionCreature::StopPlayerTrade] Error in CTrade!\n");
            return false;
        }

        CItemCmdBuilder_Svr::Build_r2c_trade_cancel(m_oId);
        pTradeWith->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_trade_cancel(pTradeWith->m_oId);
        RecvCmd(g_sendCmd);

        DEL_ONE(m_pTrade);
        pTradeWith->m_pTrade = NULL;

        return true;
    }
}

void CRegionCreature::QueryAccountPoing() {
    if (m_userInfo) {
        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_query_poing);
        packet.WriteLong(m_userInfo->m_userId);
        packet.WriteByte(1);  // 需要显示网页
        g_region->m_gws.EndSend();

        g_region->ShowHtmlDialog(NULL, this, R(QUERY_POINT_HTML), "POINT",
                                 R(MSG_QUERY_ACCOUNT_POINT));
    }
}

PetType CRegionCreature::GetPetType() {
    if ((IsUserPet() && m_ai) || (m_pet && m_pet->m_ai)) {
        CRegionPetAI* petAI = NULL;
        if (IsUserPet())
            petAI = (CRegionPetAI*)m_ai;
        else
            petAI = (CRegionPetAI*)m_pet->m_ai;
        if (petAI) {
            return petAI->GetPetType();
        }
    }
    return PET_TYPE_NULL;
}

bool CRegionCreature::ReleasePet(SItemID& item) {
    if (m_usePetBusy && rtGetMilliseconds() < m_usePetBusy)
        return false;
    m_usePetBusy = rtGetMilliseconds() + USE_PET_BUSY;

    if (m_pet)
        CallbackPet();
    if (m_hero)
        return false;

    SItemPet* pPetClass = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
    if (!pPetClass)
        return false;

    //<add by Tianh> 释放宠物增加修为等级限制
    if (m_core.GradeLev < pPetClass->NeedCulLev) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_PET_CALLFITLEVEL), pPetClass->NeedCulLev);
        SendSystemMessage(g_strStaticBuffer);

        return false;
    }

    // if pet is die
    if (item.params[PET_PARAM_HP] <= 0) {
        item.params[PET_PARAM_HP] = 1;
    }

    if (item.params[PET_PARAM_LEV] > DWORD(m_core.Lev + 5)) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_PET_CALLFAILCAUSELEVEL));
        SendSystemMessage(g_strStaticBuffer);
        return false;
    }
    CRegionCreature* cre = g_region->CreateNpc(pPetClass->petID);
    if (!cre)
        return false;

    // set pet info
    SPet* info = g_TablePet.FindPet(cre->m_npcId);
    if (!info) {
        g_factory->DestroyObject(cre);
        return false;
    }

    if (!m_scene->AddCreature(cre, m_pos, 0)) {
        g_factory->DestroyObject(cre);
        return false;
    }
    AddPet(cre);

    cre->m_core.Name    = item.name;
    cre->m_dwPetItemOID = item.id;
    cre->SetAI("pet");
    cre->ActiveAI();
    cre->m_ai->OnFollow(this);
    cre->SetTimer(CRE_TIMER_HEARTBEAT, HEARTBEAT_INTERVAL);
    cre->ResetCheaterCheck();
    UpdatePetDataByItem(item);

    if (item.params[PET_PARAM_STATE]) {
        if (ItemIsPetSpectre(item)) {
            LinkUpWithPet(item.id);
        } else if (ItemIsPetFairy(item)) {
            RidePet();
        }
    }

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_active_pet);
    g_sendCmd->WriteLong(cre->m_oId);
    g_sendCmd->WriteLong(item.id);
    RecvCmd(g_sendCmd);

    GetPlayerLogic()->OnPetChange();
    return true;
}

bool CRegionCreature::CallbackPet(bool bSaveCallPet /* = false*/, bool bResetState /* = true*/) {
    if (IsRidePet()) {
        RideOffPet(bResetState);
    } else if (IsLinkUpWithPet()) {
        LinkUpWithPet(0, bResetState);
    }

    // if(m_usePetBusy && rtGetMilliseconds() < m_usePetBusy)
    //	return false;
    m_usePetBusy = rtGetMilliseconds() + 1000;

    if (!m_pet)
        return false;

    if (!bSaveCallPet) {
        m_dwPetSave = 0;
    } else {
        m_dwPetSave = m_pet->m_dwPetItemOID;
    }
    m_pet->SavePetDataToItem(this);
    m_pet->DeleteAI();
    m_scene->RemoveCreature(m_pet->m_oId);
    g_factory->DestroyObject(m_pet);
    m_pet = NULL;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_active_pet);
    g_sendCmd->WriteLong(0);
    RecvCmd(g_sendCmd);

    GetPlayerLogic()->OnPetChange();
    return true;
}

bool CRegionCreature::ReleaseHero(SItemID& item) {
    if (m_usePetBusy && rtGetMilliseconds() < m_usePetBusy)
        return false;
    m_usePetBusy = rtGetMilliseconds() + USE_PET_BUSY;

    // if(m_pet) return false; //删除宠物释放后无法释放神仙。(需要研究)
    if (m_hero)
        return false;

    SOtherItem* pItem = g_region->m_pItemManager->GetItem(item)->pOtherItem;
    if (!pItem)
        return false;

    DWORD            npc_id = atoi(pItem->param1.c_str());
    CRegionCreature* cre    = g_region->CreateNpc(npc_id);
    if (!cre)
        return false;

    AddHero(cre);
    m_curHuiYuanHoptoadOid = cre->GetObjectId();
    if (m_scene->AddCreature(cre, m_pos, m_dir)) {
        if (cre->IsScriptLoaded())
            cre->CallScript("OnCreate", false);
    } else {
        g_factory->DestroyObject(cre);
        cre = NULL;
        return false;
    }

    cre->SetAI("hero");
    cre->ActiveAI();
    cre->m_ai->OnFollow(this);
    cre->SetTimer(CRE_TIMER_HEARTBEAT, HEARTBEAT_INTERVAL);

    return true;
}

bool CRegionCreature::CallbackHero() {
    // Comment blow by louis
    // We can have hero even we are riding.
    // if(IsRidePet()) return false;

    m_usePetBusy = rtGetMilliseconds() + 1000;

    if (!m_hero)
        return false;
    m_hero->DeleteAI();
    m_scene->RemoveCreature(m_hero->m_oId);
    g_factory->DestroyObject(m_hero);
    m_hero                 = NULL;
    m_curHuiYuanHoptoadOid = 0;
    return true;
}

void CRegionCreature::RecomputeAttr() {
    if (m_master) {
        SPet* info = g_TablePet.FindPet(m_npcId);
        if (!info)
            return;

        SBaseValue* base = g_TableBaseValue.Find(GetRealLevel(m_core.Lev));
        if (!base)
            return;

        CRegionPetAI* petAI = (CRegionPetAI*)m_ai;
        if (!petAI || !petAI->m_pItemPet.id)
            return;

        SItemID item = petAI->m_pItemPet;

        if (!ItemIsPet(item))
            return;

        float FHp     = 0.f;
        float FMp     = 0.f;
        float FAttack = 0.f;
        float FDamage = 0.f;
        float FDodge  = 0.f;
        float FArmor  = 0.f;
        float ElementArmorRate[3];
        float ElementAttackRate[3];

        if (!GetPetInfoFromItem(item, FHp, FMp, FAttack, FDamage, FDodge, FArmor, ElementArmorRate,
                                ElementAttackRate))
            return;

        m_core.DamageMin.Base =
            (base->FDamage * FDamage + info->AddedDamage) * (1 - info->DamageFloat);
        m_core.DamageMax.Base =
            (base->FDamage * FDamage + info->AddedDamage) * (1 + info->DamageFloat);
        m_core.Attack.Base   = base->FAttack * FAttack + info->AddedAttack;
        m_core.Armor.Base    = base->FArmor * FArmor + info->AddedArmor;
        m_core.Dodge.Base    = base->FDodge * FDodge + info->AddedDodge;
        m_core.MaxHp.Base    = base->FHp * FHp + info->AddedHp;
        m_core.MaxMp.Base    = base->FMp * FMp + info->AddedMp;
        m_core.AttSpeed.Base = info->AttackSpeed * ATT_SPEED_SCALE;

        for (int i = 0; i < MAX_PET_MODEL_CHANGE; i++) {
            if (m_core.Lev <= info->ModelLevel[i]) {
                m_core.ModelId          = info->ModelChange[i];
                m_attackRange           = info->AttackRangeChange[i];
                m_core.AttackRange.Base = info->AttackRangeChange[i];
                break;
            }
        }

        for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
            m_core.ElementArmor[i].Base  = ((m_core.Lev - 1) * 1 + 10) * 2 * (ElementArmorRate[i]);
            m_core.ElementDamage[i].Base = (m_core.Lev - 1 + 10) * 2 * (ElementAttackRate[i]);
        }
    } else {
        m_core.RecomputeAttr();
    }
}

bool CRegionCreature::SetLev(int lev) {
    if (lev < 1 || lev > MAX_LEVEL)
        return false;

    ULONG setexp = 0;
    if (lev >= m_core.Lev) {
        lev = lev - m_core.Lev;
        for (int i = 0; i < lev; ++i) {
            setexp = g_TableExp.GetNextLevExp(m_core.Lev + 1);
            while (setexp > 1312200000) {
                AddExp(1312200000, 0);
                setexp -= 1312200000;
            }
            AddExp(setexp, 0);
        }
    } else {
        lev = m_core.Lev - lev;
        for (int i = 0; i < lev; ++i) {
            setexp = g_TableExp.GetNextLevExp(m_core.Lev);
            while (setexp > 1312200000) {
                AddExp(-1312200000, 0);
                setexp -= 1312200000;
            }
            AddExp(-setexp, 0);
        }
    }

    return true;
}

void CRegionCreature::OnLevelUp() {
    if (IsUserPet() && m_ai) {
        ((CRegionPetAI*)m_ai)->PetLevUp();
    }
    RecomputeAttr();
    if (IsUser()) {
        /*if(m_core.Lev >= 5)*/  //Tianh 去掉5级后开始加技能SP的限制
        m_core.SkillPoint++;
        /*Tianh**09.12.14--每升一级AP+5各属性+1*/
        m_core.AttrPoint += 5;

        m_core.Str.Base++;
        m_core.Con.Base++;
        m_core.Dex.Base++;
        m_core.Hit.Base++;
        m_core.Int.Base++;

        m_core.RecomputeAttr(IsChengBody());

        FbCheckTask(true);
    }
    m_core.SetHp(m_core.GetMaxHp());
    m_core.SetMp(m_core.GetMaxMp());
}

void CRegionCreature::OnLevelDown() {
    RecomputeAttr();
    if (IsUser()) {
        //Tianh 降级不需要处理SP和AP
        // 	if(m_core.Lev >= 5)
        // 		m_core.SkillPoint -= 1;
        // 	m_core.AttrPoint -= 5;
        FbCheckTask(false);
    }
    m_core.SetHp(m_core.GetMaxHp());
    m_core.SetMp(m_core.GetMaxMp());
}

void CRegionCreature::FbCheckTask(bool notifyClient) {
    vector<long> fblist;
    g_TableFB.GetByLevel(m_core.Lev, m_core.Faction, fblist);
    if (fblist.size() > 0) {
        for (int i = 0; i < (int)fblist.size(); i++)
            m_task.AddFb(fblist[i]);
        if (notifyClient)
            UpdateToClient(NET_CRE_UPDATE_FB_TASK);
    }
}

void CRegionCreature::FbCheckAllTask(bool notifyClient) {
    vector<long> fblist;
    g_TableFB.GetByLevelBelow(m_core.Lev, m_core.Faction, fblist);
    if (fblist.size() > 0) {
        for (int i = 0; i < (int)fblist.size(); i++)
            m_task.AddFb(fblist[i]);
    }

    m_task.CheckAllFbTask();
    if (notifyClient)
        UpdateToClient(NET_CRE_UPDATE_FB_TASK);
}

void CRegionCreature::UpdatePetDataByItem(SItemID& item) {
    if (!ItemIsPet(item))
        return;
    if (!m_pet)
        return;

    /*
	BYTE level = -1;
	DWORD exp = 0;
	BYTE aiType = 0;
	ItemID_GetPetData(item, level, exp, aiType);
	*/
    m_pet->m_core.Lev = item.params[PET_PARAM_LEV];
    if (m_pet->m_core.Lev < 1)
        m_pet->m_core.Lev = 1;
    m_pet->m_core.Exp = item.params[PET_PARAM_EXP];

    // get model and attr
    m_pet->RecomputeAttr();

    //Tianh
    CActorSkill* petActorSkill = FindActorSkill(m_pet, m_pet->m_oId);

    if (petActorSkill && petActorSkill->FindPassive(SUPER_TREATMAEN_HP)) {
        if (item.params[PET_PARAM_HP] == 1) {
            int Hp = m_pet->m_core.GetMaxHp() * 0.2;
            m_pet->m_core.SetHp(Hp);
        }
    } else {
        m_pet->m_core.SetHp(item.params[PET_PARAM_HP]);
    }

    if (petActorSkill && petActorSkill->FindPassive(SUPER_TREATMAEN_MP)) {
        if (item.params[PET_PARAM_HP] == 1) {
            int Mp = m_pet->m_core.GetMaxMp() * 0.2;
            m_pet->m_core.SetMp(Mp);
        }
    } else {
        m_pet->m_core.SetMp(item.params[PET_PARAM_MP]);
    }

    // AI type
    // item.params[PET_PARAM_AI]
}

void CRegionCreature::SavePetDataToItem(CRegionCreature* pMaster) {
    int page, i, j;
    if (m_dwPetItemOID == 0) {
        ERR("[SavePetDataToItem]: m_dwPetItemOID==0\n");
        return;
    }
    if (!pMaster)
        return;
    CRegionItemContainer* pCon = pMaster->m_pItemContainer;
    if (!pCon)
        return;
    if (!pCon->m_PetBag.Find(m_dwPetItemOID, &page, &i, &j)) {
        ERR("[SavePetDataToItem]: Can NOT find the pet item\n");
        return;
    }

    SItemID item = pCon->m_PetBag.GetItem(page, i, j);
    if (!ItemID_IsValid(item))
        return;

    // ItemID_SetPetData(item, m_core.Lev, m_core.Exp, 0);
    // item.paramCount = 5;
    item.params[PET_PARAM_LEV] = m_core.Lev;
    item.params[PET_PARAM_EXP] = m_core.Exp;
    item.params[PET_PARAM_HP]  = m_core.GetHp();
    if (item.params[PET_PARAM_HP] <= 0) {
        item.params[PET_PARAM_HP] = 1;
    }
    item.params[PET_PARAM_MP] = m_core.GetMp();
    // item.params[PET_PARAM_AI] = m_core.Lev;

    if (m_ai)
        ((CRegionPetAI*)m_ai)->m_pItemPet = item;

    pCon->m_PetBag.SetItem(page, i, j, item);
    pCon->UpdateItemToClient(ITEM_CONTAINER_PLAYER_PET_BAG, page, i, j);
}

void CRegionCreature::UpdateEquipTrump(SItemID& newItem) {
    if (!m_pItemContainer)
        return;

    SItemID item = m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);
    if (!ItemIsTrump(item.type)) {
        ERR("[CRegionCreature::UpdateEquipTrump] It Is NOT MagicWeapon, But Try To Update It!\n");
        return;
    }
    if (!m_pItemContainer->SetEquipItem(CItemContainerBase::TRUMP, newItem)) {
        return;
    }
    m_pItemContainer->UpdateItemToClient(CItemContainerBase::TRUMP);

    m_pItemContainer->OnCreatureAttribChanged(CItemContainerBase::TRUMP);
}

bool CRegionCreature::SetWarder(CRegionCreature* pWarder) {
    if (IsUser() || IsUserPet() || IsCallNpc())
        return false;
    if (pWarder && !pWarder->IsUser())
        return false;

    if (pWarder)
        pWarder->m_WardNPC = this;
    if (m_master)
        m_master->m_WardNPC = NULL;
    m_master = pWarder;
    return true;
}

bool CRegionCreature::RemoveWardNPC(void) {
    if (!m_WardNPC)
        return false;
    m_WardNPC->DeleteAI();
    m_scene->RemoveCreature(m_WardNPC->m_oId);
    g_factory->DestroyObject(m_WardNPC);
    m_WardNPC = NULL;
    return true;
}

void CRegionCreature::Chat(char channel, const char* msg, const int Range, const char faction) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_chat);
    g_sendCmd->WriteByte(channel);
    g_sendCmd->WriteLong(IsUser() ? m_oId : 0);
    g_sendCmd->WriteString((char*)m_core.Name.c_str());
    g_sendCmd->WriteString((char*)msg);
    if (faction == FACTION_UNKNOWN)
        m_scene->BroadcastCmd(m_pos, Range, g_sendCmd, NULL);
    else
        g_region->BroadcastCmd(g_sendCmd, NULL, faction);
}

//单对单聊天
// gao 2010.3.19
// 添加一个新的参数，目的是判断是否是发给自己的
// 当是发给自己的时候，这里要改ID，这样在客户端在聊天框添加可以通过ID添加好友，不会判断出错
void CRegionCreature::Chat(char channel, const char* msg, CRegionCreature* receive_cre,
                           CRegionCreature* from_cre, bool self) {
    //Tianh 如果接受者的黑名单里有发送者   则不接收
    if (receive_cre && from_cre) {
        if (receive_cre->m_pFriend && from_cre->m_userInfo) {
            if (receive_cre->m_pFriend->mConnection.Exist(SConnection::BLACKLIST,
                                                          from_cre->m_userInfo->m_userId)) {
                return;
            }
        }
    }

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_chat);
    g_sendCmd->WriteByte(channel);
    if (from_cre == NULL) {
        g_sendCmd->WriteLong(0);
        g_sendCmd->WriteString((char*)"");
    } else {
        g_sendCmd->WriteLong(from_cre->m_oId);
        if (self)
            g_sendCmd->WriteString((char*)receive_cre->m_core.Name.c_str());
        else
            g_sendCmd->WriteString((char*)from_cre->m_core.Name.c_str());
    }

    g_sendCmd->WriteString((char*)msg);
    receive_cre->RecvCmd(g_sendCmd);
}

void CRegionCreature::ChatNewS(const char* fromName, const char* Name, const char* BossName,
                               const char* SceneName, long type, const char* ItemName) {

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_chat_NewS);
    /*g_sendCmd->WriteByte(channel);*/
    /*g_sendCmd->WriteLong(m_oId);*/
    /*g_sendCmd->WriteString((char*)m_core.Name.c_str());*/
    g_sendCmd->WriteString((char*)fromName);
    g_sendCmd->WriteString((char*)Name);
    g_sendCmd->WriteString((char*)BossName);
    g_sendCmd->WriteString((char*)SceneName);
    g_sendCmd->WriteLong(type);
    g_sendCmd->WriteString((char*)ItemName);
    g_region->BroadcastCmd(g_sendCmd, NULL, FACTION_ALL);

    return;
}

void CRegionCreature::Chat(char channel, const char* msg, CRegionCreature* receive_cre,
                           ULONG from_region_id, const char* from_name) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_chat);
    g_sendCmd->WriteByte(channel);
    g_sendCmd->WriteLong(from_region_id);
    g_sendCmd->WriteString((char*)from_name);
    g_sendCmd->WriteString((char*)msg);
    receive_cre->RecvCmd(g_sendCmd);

    return;
}

////返回玩家信息
//void CRegionCreature::GetItemInfo(long PlayerID,long ID,int ChatType)
//{
//	SItemID rItem;
//	long lcolor;
//	//<add by Tianh>  解析发来的物品
//	CRegionCreature *pCreature = m_scene->FindCreature(PlayerID);
//	rItem = pCreature->m_pItemContainer->m_Bag.GetItemFromID(ID);
//	lcolor = SItemID.color;
//
//	g_sendCmd->BeginWrite();
//	g_sendCmd->WriteShort(r2c_hyperLink_item);
//	g_sendCmd->WriteLong(lcolor);
//
//	if (ChatType == CHAT_CHANNEL_TEAM)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_PARTY)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_COUNTRY)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_PUBLIC)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_PRIVATE)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_SYSTEM)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_GM)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_MAX)
//	{
//	}
//	else if (ChatType == CHAT_CHANNEL_NEWS)
//	{
//	}
//}

//Tian 10.04.18 解析聊天中的超连接
std::string gChatString;

bool CRegionCreature::ResolvingLinkChat(const char* msg) {
    gChatString           = "";
    const string strBegin = "item[";
    const string strEnd   = "]meti";

    const string test = msg;

    size_t  nPos     = 0;
    int     nCurPos  = 0;
    int     nEnd     = 0;
    string  AllStr   = "";
    string  Link     = "";
    string  newTemp  = "";
    string  newTemp1 = "";
    string  newTemp2 = "";
    long    ItemID   = 0;
    long    PlayerID = 0;
    long    Type     = 0;
    SItemID rItem;
    long    lcolor   = 0;
    string  ItemName = "";
    string  StrItem;

    while (nPos != string::npos) {
        nPos = test.find(strBegin, nCurPos);
        if (nPos != string::npos) {
            string str = test.substr(nCurPos, nPos - nCurPos);
            /*cout<<"普通文本:\t"<<str<<endl;*/
            AllStr += str;

            nCurPos = nPos + 5;
            nPos    = test.find(strEnd, nCurPos);
            if (nPos != string::npos) {
                /*cout<<"超链接:\t"<<test.substr(nCurPos, nPos-nCurPos)<<endl;*/

                Link = test.substr(nCurPos, nPos - nCurPos);

                size_t nPos = Link.find(",");
                newTemp     = Link.substr(0, nPos);
                Type        = atol(newTemp.c_str());

                size_t nPos1 = nPos + 1;
                nPos         = Link.find(",", nPos1);
                newTemp1     = Link.substr(nPos1, nPos);
                ItemID       = atol(newTemp1.c_str());

                size_t nPos2 = nPos + 1;
                nPos         = Link.find(",", nPos2);
                newTemp2     = Link.substr(nPos2, Link.length() - nPos);
                PlayerID     = atol(newTemp2.c_str());

                if (PlayerID < 0 || ItemID <= 0 || Type < 0) {
                    break;
                }
                if (PlayerID > 0) {
                    CRegionCreature* pCreature = m_scene->FindCreature(PlayerID);
                    ItemName                   = "";

                    if (pCreature) {
                        if (!(pCreature->IsUser())) {
                            break;
                        }
                        if (Type == 0)  //背包物品
                        {
                            if (pCreature->m_pItemContainer) {
                                rItem  = pCreature->m_pItemContainer->m_Bag.GetItemFromID(ItemID);
                                lcolor = rItem.color;

                                CItemManager* m_pItemManager =
                                    pCreature->m_pItemContainer->GetItemManager();
                                if (m_pItemManager) {
                                    StrItem              = rItem.SaveToString(m_pItemManager);
                                    SItemBase* pItemBase = m_pItemManager->GetItem(rItem.type);
                                    if (pItemBase) {
                                        ItemName = pItemBase->name;
                                    }
                                }
                            }
                        } else if (Type == 1)  //身上物品
                        {
                            for (DWORD i = CItemContainerBase::WEAPON_1;
                                 i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
                                if (m_pItemContainer) {
                                    SItemID ret = pCreature->m_pItemContainer->GetEquipItem(
                                        (CItemContainerBase::EEquipParts)i, true);
                                    if (ret.id == ItemID) {
                                        CItemManager* m_pItemManager =
                                            pCreature->m_pItemContainer->GetItemManager();
                                        lcolor               = ret.color;
                                        StrItem              = ret.SaveToString(m_pItemManager);
                                        SItemBase* pItemBase = m_pItemManager->GetItem(ret.type);
                                        if (pItemBase) {
                                            ItemName = pItemBase->name;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (PlayerID == 0) {
                }

                //item[color, 名字,StringItem]item
                char Cstr[10240];
                AllStr += "item[";
                rt2_sprintf(Cstr, "%d^)!%s^)!%s", lcolor, ItemName.c_str(), StrItem.c_str());

                AllStr += Cstr;
                nPos += 5;
            } else {
                if (nPos != test.size()) {
                    cout << "非法:\t" << test.substr(nCurPos, test.size() - nCurPos) << endl;
                    break;
                }
            }
            nCurPos = nPos;
        } else if (nCurPos != test.size()) {
            //没有找到开始token,则正常语句
            AllStr += test.substr(nCurPos, test.size() - nCurPos);
            break;
        }
    }

    gChatString = AllStr;

    if (gChatString.empty() || gChatString == "") {
        return false;
    }

    return true;
}

void CRegionCreature::SendSystemMessage(const char* szSysMsg, CRegionCreature* pFromCre) {
    if (szSysMsg == NULL || szSysMsg[0] == 0)
        return;
    Chat(CHAT_CHANNEL_SYSTEM, szSysMsg, this, pFromCre);
}

void CRegionCreature::SendBattleInfoMessage(const char* szSysMsg, CRegionCreature* pFromCre) {
    if (szSysMsg == NULL)
        return;
    Chat(CHAT_CHANNEL_BATTLEINFO, szSysMsg, this, pFromCre);
}

void CRegionCreature::CommonChat(char channel, char const* msg, char const* target,
                                 char const* title) {
    if (!ResolvingLinkChat(msg)) {
        return;
    }

    const char* Cmag = gChatString.c_str();
    // 	const char* Cmag = ResolvingLinkChat(msg);
    if (channel == CHAT_CHANNEL_PRIVATE) {  //私人频道
        PrivateChat(target, Cmag);
    } else if (channel == CHAT_CHANNEL_TEAM) {  //队伍频道
        TeamChat(Cmag);
    } else if (channel == CHAT_CHANNEL_PARTY) {  //同盟频道
        PartyChat(Cmag);
    } else if (channel == CHAT_CHANNEL_COUNTRY) {  //国家频道
        CountryChat(Cmag);
    } else if (channel == CHAT_CHANNEL_GM) {  //GM频道
        if (title && *title)
            GmChat(Cmag, title);
        else
            GmChat(Cmag);
    } else if (channel == CHAT_CHANNEL_PUBLIC) {  //公共频道
        //公共聊天[!]
        // Chagne public chat to world chat bu louis
        //CmdGiver->Chat(channel,msg,DEFAULT_CHAT_RANGE*3);
        PublicChat(Cmag);
    } else {  //一般频道
        Chat(channel, Cmag);
    }
}

const int MAX_COUNTRY_CHAT_LEVEL            = 80;
const int MAX_LEUD_COUNTRY_CHAT_UNION_LEVEL = 60;

void CRegionCreature::ClearResideForNameColor(void) {
    for (std::list<ULONG>::iterator itReside = m_vResideObjectID.begin();
         itReside != m_vResideObjectID.end(); ++itReside) {
        CRegionCreature* pCreature = m_scene->FindCreature(*itReside);
        if (pCreature && pCreature->IsUser()) {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_updata_npc_color);
            g_sendCmd->WriteLong(GetObjectId());  //NPC的ID
            g_sendCmd->WriteLong(0);              //32位颜色值
            pCreature->RecvCmd(g_sendCmd);
        }
    }

    m_vResideObjectID.clear();
    m_NameColor = -1;
}

void CRegionCreature::DeleteResideForNameColor(ULONG lObjectID) {
    if (lObjectID <= 0 || IsUser() || IsUserPet() || IsCallNpc())
        return;

    for (std::list<ULONG>::iterator itReside = m_vResideObjectID.begin();
         itReside != m_vResideObjectID.end(); ++itReside) {
        if ((*itReside) == lObjectID) {
            CRegionCreature* pCreature = m_scene->FindCreature(lObjectID);
            if (pCreature && pCreature->IsUser()) {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(r2c_updata_npc_color);
                g_sendCmd->WriteLong(GetObjectId());  //NPC的ID
                g_sendCmd->WriteLong(0);              //32位颜色值
                pCreature->RecvCmd(g_sendCmd);
            }
            m_vResideObjectID.erase(itReside);
            break;
        }
    }
}

void CRegionCreature::AddResideForNameColor(ULONG lObjectID) {
    if (lObjectID <= 0 || IsUser() || IsUserPet() || IsCallNpc())
        return;

    for (std::list<ULONG>::iterator itReside = m_vResideObjectID.begin();
         itReside != m_vResideObjectID.end(); ++itReside) {
        if ((*itReside) == lObjectID)
            return;
    }

    if (m_scene->FindCreature(lObjectID))
        m_vResideObjectID.push_back(lObjectID);
}

void CRegionCreature::SetColorToNPCName(DWORD dwColor) {
    if (IsUser() || IsUserPet() || IsCallNpc())
        return;
    m_NameColor = dwColor;
    if (!m_vResideObjectID.empty())
        UpdataNPCNameColorToClient();
}

void CRegionCreature::UpdataNPCNameColorToClient(void) {
    if (IsUser() || IsUserPet() || m_NameColor == -1 || IsCallNpc()) {
        ClearResideForNameColor();
        m_NameColor = -1;
        return;
    }

    for (std::list<ULONG>::iterator itReside = m_vResideObjectID.begin();
         itReside != m_vResideObjectID.end(); itReside++) {
        CRegionCreature* pCreature = m_scene->FindCreature(*itReside);
        if (!pCreature || !pCreature->IsUser()) {
            itReside = m_vResideObjectID.erase(itReside);
            if (itReside == m_vResideObjectID.end())
                break;
            continue;
        }

        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_updata_npc_color);
        g_sendCmd->WriteLong(GetObjectId());  //NPC的ID
        g_sendCmd->WriteLong(m_NameColor);    //32位颜色值
        pCreature->RecvCmd(g_sendCmd);
    }
}

void CRegionCreature::UpdataNPCNameColorToClient(ULONG lObjectID) {
    if (IsUser() || IsUserPet() || m_NameColor == -1 || IsCallNpc()) {
        ClearResideForNameColor();
        m_NameColor = -1;
        return;
    }

    for (std::list<ULONG>::iterator itReside = m_vResideObjectID.begin();
         itReside != m_vResideObjectID.end(); itReside++) {
        if (lObjectID != (*itReside))
            continue;

        CRegionCreature* pCreature = m_scene->FindCreature(lObjectID);
        if (!pCreature || !pCreature->IsUser()) {
            itReside = m_vResideObjectID.erase(itReside);
            return;
        }

        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_updata_npc_color);
        g_sendCmd->WriteLong(GetObjectId());  //NPC的ID
        g_sendCmd->WriteLong(m_NameColor);    //32位颜色值
        pCreature->RecvCmd(g_sendCmd);
        return;
    }
}

void CRegionCreature::UpdataAnimationToClient(const char* sAnimName) {
    if (IsUser() || !sAnimName || sAnimName[0] == 0 || !m_scene)
        return;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_update_npc_animation);
    g_sendCmd->WriteLong(GetObjectId());  //NPC的ID
    g_sendCmd->WriteString(sAnimName);

    CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT] = {0};
    int            ObjectNum =
        m_scene->GetAroundObjectByGrid(pFindCreature, m_pos, DEFAULT_RANGE, SCENE_MAX_FIND_OBJECT);
    for (int i = 0; i < ObjectNum; i++) {
        CRegionCreature* pUser = dynamic_cast<CRegionCreature*>(pFindCreature[i]);
        if (pUser && pUser->IsUser())
            pUser->RecvCmd(g_sendCmd);
    }
}

void CRegionCreature::CountryChat(const char* msg)  //国家聊天
{
    char Temp[100];

    //你的等级不够
    if (m_core.Lev <= MAX_COUNTRY_CHAT_LEVEL && !IsUnionCaption()) {
        rt2_sprintf(Temp, R(MSG_CHAT_LEVEL_LOW), MAX_COUNTRY_CHAT_LEVEL);
        SendSystemMessage(Temp);
        return;
    }

    //是诸侯，但等级不够
    if (m_core.Lev <= MAX_LEUD_COUNTRY_CHAT_UNION_LEVEL && IsUnionCaption()) {
        rt2_sprintf(Temp, R(MSG_CHAT_IS_UNION_BUT_LEVEL_LOW), MAX_LEUD_COUNTRY_CHAT_UNION_LEVEL);
        SendSystemMessage(Temp);
        return;
    }

    //不在国战场景中
    if (!g_WarfieldMgr.IsInWarfield(this)) {
        SendSystemMessage(R(MSG_CHAT_NOT_IN_NATIONAL_WAR));
        return;
    }

    //给所有同阵营的玩家发消息
    g_WarfieldMgr.BroadcastMessage((char*)msg, CHAT_CHANNEL_COUNTRY, m_oId, m_core.Name.c_str(),
                                   m_core.Faction);
}

void CRegionCreature::PublicChat(const char* msg) {
    //不到10级  不可以聊天
    if (m_core.Lev < 10) {
        return;
    }

    if ((m_WorldChatTime + 20 * 1000) <= rtGetMilliseconds()) {
        g_region->m_gws.SendChat(FACTION_ALL, m_core.Name.c_str(), CHAT_CHANNEL_PUBLIC, msg, "",
                                 m_oId);
        m_WorldChatTime = rtGetMilliseconds();
    } else {
        long Time = ((m_WorldChatTime + 20 * 1000) - rtGetMilliseconds()) / 1000;
        rt2_sprintf(g_strStaticBuffer, R(MSG_CHAT_NEEDDELAY), Time);
        SendSystemMessage(g_strStaticBuffer);
    }

    return;
    //string smsg = msg;
    //string strSendMessage = "";
    //if (msg[0] == '!' || smsg.find("！") == 1)
    //{
    //if(m_pItemContainer->m_Bag.FindType(6220))
    //{
    //	RemoveItem(6220,1);
    // 			char* message = RT_NEW char[strlen(msg) + 6];
    //			switch (m_core.Faction)
    //			{
    //			case FACTION_SHANG:
    //				strSendMessage = "[商]";
    //// 				strcpy(message,"[商]");
    //				break;
    //			case FACTION_ZHOU:
    //				strSendMessage = "[周]";
    //// 				strcpy(message,"[周]");
    //				break;
    //			case FACTION_NEUTRAL://中立为世界消息  无阵营
    //				strSendMessage = "[世界]";
    //// 				strcpy(message,"[世界]");
    //				break;
    //			default:
    //				strSendMessage = "[无]";
    //// 				strcpy(message,"[无]");
    //			}
    //			if(msg[0] == '!') msg++;
    //
    //			strSendMessage += msg;
    // 			else msg = msg + 2;
    // 			rt2_strcat(message,msg);
    //小喇叭
    //g_region->m_gws.SendChat(FACTION_ALL,m_core.Name.c_str(),CHAT_CHANNEL_WORLD,strSendMessage.c_str());		//heten
    // 			delete message;
    //}
    /*else
			SendSystemMessage(R(MSG_CHAT_PUBLIC_NOT_LABA));*/
    //return;
    //}
    //else
    //{
    //	if (IsHuiYuan())

    //	else if (m_pItemContainer->m_Bag.GetMoney() >= 1000)
    //	{
    //		AddGold(-1000);
    //		g_region->m_gws.SendChat(m_core.Faction,m_core.Name.c_str(),CHAT_CHANNEL_PUBLIC,msg);
    //		SendSystemMessage(R(MSG_CHAT_PUBLIC_USE_MONEY));
    //	}
    //	else
    //		SendSystemMessage(R(MSG_CHAT_PUBLIC_NOT_MEMBER));
    //}
}

void CRegionCreature::PartyChat(const char* msg) {
    if (m_unionID == 0) {
        SendSystemMessage(R(MSG_CHAT_NOT_UNION));  //系统消息
        return;
    }
    g_region->m_gws.SendChat(m_unionID, m_core.Name.c_str(), CHAT_CHANNEL_PARTY, msg);
    return;
}

void CRegionCreature::TeamChat(const char* msg) {
    if (m_pTeam->mData.Size() == 0) {
        SendSystemMessage(R(MSG_CHAT_NOT_TEAM));  //系统消息
        return;
    }

    CRegionUser*     user;
    CRegionCreature* cre;
    for (int i = m_pTeam->mData.Begin(); i != m_pTeam->mData.End(); i++) {
        user = g_region->FindUser(m_pTeam->mData[i].mDBID);
        if (user == NULL) {  //该玩家在region上找不到，向上发
            g_region->m_gws.SendChat(m_pTeam->mData[i].mDBID, m_core.Name.c_str(),
                                     CHAT_CHANNEL_TEAM, msg);
        } else {
            cre = user->m_dummy;
            Chat(CHAT_CHANNEL_TEAM, msg, cre, this);  //发给队友和自己
        }
    }
}

const int CHAT_NAME_LIMIT_COUNT = 32;

void CRegionCreature::PrivateChat(const char* target, const char* msg) {
    string TargetName(target);

    if (TargetName.size() > CHAT_NAME_LIMIT_COUNT) {
        TargetName = TargetName.substr(0, CHAT_NAME_LIMIT_COUNT);
    }

    CRegionUser* user = g_region->FindUser(TargetName.c_str());

    if (user == NULL) {  //该玩家在region上找不到，向上发
        g_region->m_gws.SendChat(TargetName.c_str(), m_core.Name.c_str(), CHAT_CHANNEL_PRIVATE,
                                 msg);
    } else {
        CRegionCreature* cre = user->m_dummy;

        if (cre != this) {
            Chat(CHAT_CHANNEL_PRIVATE, msg, cre, this);  //发给密聊者
        }
        Chat(CHAT_CHANNEL_PRIVATE, msg, this, cre, true);  //发给自己
    }
    return;
}

void CRegionCreature::GmChat(const char* msg, const char* title /*=""*/) {
    guard;

    if (!m_userInfo)
        return;

    g_region->m_gws.SendChat(m_userInfo->m_userId, m_core.Name.c_str(), CHAT_CHANNEL_GM, msg,
                             title);
    return;

    unguard;
}

void CRegionCreature::OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock) {
    if (!IsUser())
        return;

    if (m_userInfo == NULL)
        return;

    if (pNewBlock == NULL || pNewBlock->m_pBlock == NULL)
        return;

    // pk attribure.
    std::string szBlockName(pNewBlock->m_szFileName);
    SSceneInfo* pSceneInfo = g_TableScene.FindScene(szBlockName);
    if (pSceneInfo) {
        switch (pSceneInfo->iPKAttr) {
            case 0:
                m_eBlockPosPKAttr = PKA_UNSAFE;
                break;
            default:
            case 1:
                m_eBlockPosPKAttr = PKA_NORMAL;
                break;
            case 2:
                m_eBlockPosPKAttr = PKA_SAFE;
                break;
            case 3:
                m_eBlockPosPKAttr = PKA_SAFEST;
                break;
            case 4:
                m_eBlockPosPKAttr = PKA_PKUNABLE;
                break;
        }
    } else {
        m_eBlockPosPKAttr = PKA_NORMAL;
    }
    //Tianh
    m_pFriend->SelfDataChangeAffectOther();

    //-------------add start by tony 06.05.30-----------------------------//
    //- 玩家切换场景块处理
    CG_CmdPacket& cmd = g_region->m_gws.BeginSend();
    cmd.WriteShort(r2g_user_change_block);
    cmd.WriteLong(m_userInfo->m_userId);
    if (pOldBlock && pOldBlock->m_pBlock)
        cmd.WriteString((char*)pOldBlock->GetFileName());
    else
        cmd.WriteString("");
    cmd.WriteString((char*)pNewBlock->GetFileName());
    g_region->m_gws.EndSend();
    //-------------add end   by tony 06.05.30-----------------------------//

    // switch region

    EXT_SPACE::unordered_map<ULONG, long>::iterator iter =
        g_region->m_regionMap.find((ULONG)pNewBlock);

    if (iter == g_region->m_regionMap.end())  // 没有找到新的块
        return;

    if ((*iter).second == (long)g_cfgRs.lRegionID)  // 和当前是相同的块
        return;

    m_userInfo->m_regionId = (ULONG)((*iter).second);  // 设置新的块ID
    g_region->m_gws.BeginSwitchRegion(m_userInfo);
}

const char* s_szPKArea = "PK:";

void CRegionCreature::UpdateAreaPKAttr() {
    int                                           iLen;
    char *                                        pStr, *pValue;
    RtsSceneBlockRtb::SArea*                      pArea;
    std::list<RtsSceneBlockRtb::SArea*>::iterator it;
    m_eCurPosPKAttr = m_eBlockPosPKAttr;
    for (it = m_listArea.begin(); it != m_listArea.end(); it++) {
        pArea = (*it);
        pStr  = strstr((char*)pArea->szData, (char*)s_szPKArea);
        if (pStr) {
            pStr += strlen(s_szPKArea);
            pValue = strchr(pStr, ';');
            if (pValue == NULL)
                pValue = pStr + strlen(pStr);
            iLen = pValue - pStr;
            if (pValue && iLen > 0) {
                if (strncmp(pStr, "Unsafe", strlen("Unsafe")) == 0) {
                    m_eCurPosPKAttr = PKA_UNSAFE;
                    return;
                } else if (strncmp(pStr, "Safest", strlen("Safest")) == 0) {
                    m_eCurPosPKAttr = PKA_SAFEST;
                    return;
                } else if (strncmp(pStr, "Safe", strlen("Safe")) == 0) {
                    m_eCurPosPKAttr = PKA_SAFE;
                    return;
                } else if (strncmp(pStr, "pkunable", strlen("pkunable")) == 0) {
                    m_eCurPosPKAttr = PKA_PKUNABLE;
                    return;
                }
            }
        }
    }
    if (IsRidePet()) {
        m_pet->m_eCurPosPKAttr = m_eCurPosPKAttr;
    }
}

void CRegionCreature::OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock) {
    if (!IsUser()) {
        return;
    }
    GetPlayerLogic()->OnAreaEnter(pArea->szName);
    char functionName[1024] = {0};
    rt2_snprintf(functionName, sizeof(functionName) / sizeof(functionName[0]) - 1, "OnEnterArea_%s",
                 pArea->szName);
    m_scene->CallScript(functionName, true, this);
}

void CRegionCreature::OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock) {
    if (!IsUser()) {
        return;
    }

    char functionName[1024] = {0};
    rt2_snprintf(functionName, sizeof(functionName) / sizeof(functionName[0]) - 1, "OnExitArea_%s",
                 pArea->szName);
    m_scene->CallScript(functionName, true, this);
}

bool CRegionCreature::CanAcceptTask(int TaskId, CRegionCreature* npc) {
    STaskDesc* desc = g_TableTask.FindTask(TaskId);
    if (!desc)
        return false;

    return CanAcceptTask(desc, false, npc);
}

bool CRegionCreature::CanAcceptTask(STaskDesc* task, bool showErr, CRegionCreature* npc) {
    if (task->moneyTask) {
        if (m_task.IsDoingMoneyTask())
            return false;
        if (m_mtFinish[task->moneyTask - 1] >= 50 * 2) {
            // if(showErr) SendSystemMessage(R(MSG_TASK_CANCEL_BUSY));
            return false;
        }
        if (m_mtBusy > time(NULL)) {
            // if(showErr) SendSystemMessage(R(MSG_TASK_MAX_LIMIT));
            return false;
        }
    }

    //PZH
    if (task->IsWenTask() /*wenTask*/) {
        if (m_task.IsDoingWenTask()) {
            //SendSystemMessage("你正在做某个门派修炼任务，不能再接其他门派修炼任务");
            return false;
        }
        if (m_wenTaskAccept >= 20) {
            //SendSystemMessage("每天最多只能接20次门派修炼任务");
            return false;
        }
        //if (rtGetMilliseconds() < GetPlayerLogic()->GetNextWenTaskTime())
        //{
        //	//SendSystemMessage("放弃派修炼任务后，10分钟后才能接门派修炼任务");
        //	return false;
        //}
    }
    if (task->IsUnionTask()) {
        if (m_UnionTask >= 20 || 0 == m_unionID) {
            return false;
        }
    }
    if (task->IsMouTask()) {
        if (m_MouTaskAccept >= 20) {
            return false;
        }
    }
    //
    if (m_task.GetTaskAcceptNum() >= MAX_TASK_SELECT_NUM)
        return false;

    if (task->access == false)
        return false;

    if (m_core.Lev < task->reqMinLev || m_core.Lev > task->reqMaxLev)
        return false;

    if (task->m_TaskGroupID < 40) {
        MainStoryData* pData = CMainStoryMgr::GetMgr().GetMainStoryByTaskID(task->id);
        if (NULL == pData) {
            return false;
        }
        int nMainID = GetPlayerLogic()->GetCurMainStoryID();
        if (nMainID != pData->nDataID) {
            return false;
        }
    }

    if (task->countryLimit != FACTION_ALL) {
        if (task->countryLimit != m_core.Faction)
            return false;
    }
    if (task->metierLimit != METIER_ALL) {
        if (m_core.Metier != task->metierLimit)
            return false;
    }

    // 分组任务
    if (task->group != 0 && task->group != -1)  //tim.yang  日常任务
    {
        if (m_task.IsTaskGroupAccept(task->group))
            return false;
    }

    STaskInfo* info = m_task.FindTask(task->id);
    if (info) {
        if (/*PZH*/ /*info->IsDoing*/ info->FIsDoing())
            return false;
        if (info->LastFinishTime + task->getTimeLimit > (DWORD)time(NULL))
            return false;
    }

    if (task->repeat == 0 /*&& info*/) {
        if (m_task.IsDoneTask(task->id)) {
            return false;
        }
        //if(info->FinishNum > 0) return false;
    }

    if (task->reqUnion == 1 && !IsUnionCaption()) {
        return false;
    }

    //----------add start by tony 06.05.09---------------------------//
    int               npcID = 0;
    std::vector<long> param;
    if (npc && npc->m_npcInfo) {
        npcID = npc->m_npcInfo->Id;
        param.push_back(npcID);
    }
    //----------add end   by tony 06.05.09---------------------------//
    //---------chg start by tony 06.05.09-------------------------------------------//
#if 0
	@	if (!ProcessTaskKeyList(task->acceptReq, TYPE_CHECK, NULL))
		@		return false;
#else
    if (!ProcessTaskKeyList(task->acceptReq, TYPE_CHECK, &param))
        return false;
#endif
    //---------chg end   by tony 06.05.09-------------------------------------------//

    return true;
}

//---------chg start by tony 06.05.09-------------------------------------------//
#if 0
@ bool CRegionCreature::CanFinishTask(STaskDesc* task, int idx)
#else
bool CRegionCreature::CanFinishTask(STaskDesc* task, int idx, CRegionCreature* npc)
#endif
//---------chg end  by tony 06.05.09--------------------------------------------//
{
    int        nRet = 1;
    STaskInfo* info = m_task.FindTask(task->id);
    do {
        if (NULL == info) {
            break;
        }
        //---------add start by tony 06.05.09-------------------------------------------//
        int               npcID = 0;
        std::vector<long> param;
        if (npc && npc->m_npcInfo) {
            npcID = npc->m_npcInfo->Id;
            param.push_back(npcID);
        }
        bool bB = false;
        if (info->Desc && info->Desc->circleTask) {
            switch (info->Desc->circleTask) {
                case TASK_CIRCLE_FINDNPC:
                    if (info->m_findnpcID != npcID) {
                        bB = true;
                    }
                    break;
                case TASK_CIRCLE_ITEM:
                case TASK_CIRCLE_BATTLE:
                    if (info->m_srcNpcID != npcID) {
                        bB = true;
                    }
                    break;
                default:
                    break;
            }
            if (bB) {
                break;
            }
        }
        //---------chg end   by tony 06.05.09-------------------------------------------//

        if (/*PZH*/ /*!info->IsDoing*/ !info->FIsDoing()) {
            break;
        }
        if (task->finishType == TASK_FINISH_TYPE_SELECT) {
            if (!ProcessTaskKeyList(task->specReq[idx], TYPE_CHECK, NULL)) {
                nRet = 2;
                break;
            }
        } else {
            //--------------------------chg start by tony 06.05.09-------------------------------------------//
#if 0
			@ 		if (!ProcessTaskKeyList(task->taskReq, TYPE_CHECK, 0))
				@			return false;
#else
            if (!ProcessTaskKeyList(task->taskReq, TYPE_CHECK, &param, task)) {
                nRet = 2;
                break;
            }
#endif
            //--------------------------chg end   by tony 06.05.09-------------------------------------------//
        }
        nRet = 0;

    } while (false);
    bool bRet = false;
    switch (nRet) {
        case 0:
            if (!info->FIsCanFinish()) {
                info->FSetCanFinish();
                UpdateToClient(NET_CRE_UPDATE_TASK);
            }
            bRet = true;
            break;
        case 2:
            if (info->FIsCanFinish()) {
                info->FSetDoing();
                UpdateToClient(NET_CRE_UPDATE_TASK);
            }
            break;
        default:
            break;
    }
    return bRet;
}

//--------------------------chg start by tony 06.05.09-------------------------------------------//
#if 0
@bool CRegionCreature::IsDoingTask(STaskDesc* task)
@{
	@	STaskInfo* info = m_task.FindTask(task->id);
	@	if (!info) return false;
	@
		@	return info->IsDoing != 0;
	@}
#else
bool CRegionCreature::IsDoingTask(STaskDesc* task, CRegionCreature* npc) {
    STaskInfo* info = m_task.FindTask(task->id);
    if (!info)
        return false;

    if (npc && npc->m_npcInfo && info->Desc && info->Desc->circleTask) {
        switch (info->Desc->circleTask) {
            case TASK_CIRCLE_FINDNPC: {
                if (info->m_findnpcID != npc->m_npcInfo->Id)
                    return false;
            } break;
            case TASK_CIRCLE_ITEM:
            case TASK_CIRCLE_BATTLE: {
                if (info->m_srcNpcID != npc->m_npcInfo->Id)
                    return false;
            } break;
            default:
                return false;
                break;
        }
    }

    return info->FIsDoing();  //info->IsDoing != 0;
}
#endif
//--------------------------chg end   by tony 06.05.09-------------------------------------------//

//----------chg start by tony 06.05.09------------------------------------------------//
#if 0
@ bool CRegionCreature::AcceptTask(int TaskId)
#else
bool CRegionCreature::AcceptTask(int TaskId, CRegionCreature* npc)
#endif
//----------chg end   by tony 06.05.09------------------------------------------------//
{
    STaskDesc* desc = g_TableTask.FindTask(TaskId);
    if (desc) {
        //----------chg start by tony 06.05.09------------------------------------------------//
#if 0
		@		return AcceptTask(desc);
#else
        return AcceptTask(desc, npc);
#endif
        //----------chg end   by tony 06.05.09------------------------------------------------//
    } else {
        return false;
    }
}

//----------chg start by tony 06.05.09------------------------------------------------//
#if 0
@ bool CRegionCreature::AcceptTask(STaskDesc* task)
#else
bool CRegionCreature::AcceptTask(STaskDesc* task, CRegionCreature* npc)
#endif
//----------chg end   by tony 06.05.09------------------------------------------------//
{
    //-----add start by tony 06.05.09------------------------//
    if (task->circleTask != 0 && npc && npc->m_npcInfo) {
        //STaskInfo* info = m_task.FindTask(task->id);
        if (m_task.m_currCircle >= g_tcTotalCircle) {
            m_task.m_currCircle = 0;
            m_task.m_isActive   = 1;
            //info->m_currMonsterNum = 0;
            //info->m_finditemID = 0;
            //info->m_findnpcID = 0;
            //info->m_killmonsterID = 0;
            m_tmpcurrCircle     = 0;
            m_tmpcurrMonsterNum = 0;
            m_tmpfinditemID     = 0;
            m_tmpfindnpcID      = 0;
            m_tmpkillmonsterID  = 0;
            return true;
        }
    }
    //-----add end   by tony 06.05.09------------------------//

    if (m_task.AcceptTask(task->id)) {
        if (task->IsWenTask()) {
            m_wenTaskAccept++;
        } else if (task->IsUnionTask()) {
            m_UnionTask++;
        } else if (task->IsMouTask()) {
            m_MouTaskAccept++;
        }
        ProcessTaskKeyList(task->flagItem, TYPE_ADD, 0);
        //-----add start by tony 06.05.09------------------------//
        if (task->circleTask != 0 && npc && npc->m_npcInfo) {
            STaskInfo* info = m_task.FindTask(task->id);
            if (info) {
                m_task.m_currCircle++;
                info->m_srcNpcID = npc->m_npcInfo->Id;
                switch (task->circleTask) {
                    case TASK_CIRCLE_ITEM:
                        info->m_finditemID = m_tmpfinditemID;
                        break;
                    case TASK_CIRCLE_FINDNPC:
                        info->m_findnpcID = m_tmpfindnpcID;
                        break;
                    case TASK_CIRCLE_BATTLE:
                        info->m_killmonsterID = m_tmpkillmonsterID;
                        break;
                    default:
                        break;
                }
            }
        }
        std::string msg = R(MSG_TASK_ACCEPTNEW) + task->name;
        SendSystemMessage(msg.c_str());
        //-----add end   by tony 06.05.09------------------------//
        UpdateToClient(NET_CRE_UPDATE_TASK);
        NetSend_r2c_task_event(2);

        if (IsScriptLoaded()) {
            char sFuncName[50] = {0};
            rt2_snprintf(sFuncName, 50, "OnAcceptTask%d", task->id);
            if (npc)
                CallScript(sFuncName, false, npc);
            else
                CallScript(sFuncName, false, 0);
        }
        GetPlayerLogic()->TrackTask(task->id);
        CheckTaskFinishState(task, npc, false);
        return true;
    }
    if (m_task.m_bisDayTimeTaskFinish) {
        SendSystemMessage(R(MSG_DAYTIMETASK));  //tim.yang  提示玩家日常任务一天只能做一次
        m_task.m_bisDayTimeTaskFinish = false;
    }
    return false;
}

void CRegionCreature::TickTask(unsigned long delta) {
    int id = m_task.FindTimeoutTask();
    if (id) {
        CancelTask(id, true);
    }
}

void CRegionCreature::RemoveMCTask()  //tim.yang  MC
{
    m_task.RemoveMCTask();
    UpdateToClient(NET_CRE_UPDATE_TASK);
    NetSend_r2c_task_event(0);
}

bool CRegionCreature::CancelTask(DWORD TaskId, bool busy) {
    if (m_task.CancelTask(TaskId)) {
        STaskDesc* desc = g_TableTask.FindTask(TaskId);
        if (desc) {
            RemoveTaskItem(desc);
            // ProcessTaskKeyList(desc->flagItem,TYPE_REMOVE);
            if (desc->moneyTask && busy) {
                m_mtBusy = time(NULL) + (desc->moneyTask + 2) * 2 * 60;
            }
            if (desc->IsWenTask() /*wenTask*/) {
                m_wenTaskFinish = 0;
            }
            //----------add start by tony 06.05.11-----------------//
            if (desc->circleTask) {
                m_task.m_currCircle = 0;
                m_tmpcurrCircle     = 0;
                m_tmpcurrMonsterNum = 0;
                m_tmpfinditemID     = 0;
                m_tmpfindnpcID      = 0;
                m_tmpkillmonsterID  = 0;
            }
            //----------add end   by tony 06.05.11-----------------//
            vector<STaskKey>* req = NULL;

            if (desc->finishType == TASK_FINISH_TYPE_NORMAL) {
                req = &desc->taskReq;
            }
            if (NULL != req) {
                vector<long> vl;
                vl.push_back(1);
                ProcessTaskKeyList(*req, TYPE_REMOVE, &vl);
            }
            if (desc->IsGongTask()) {
                GetPlayerLogic()->SetNextGongTaskTime(time(NULL) + 600);
            }
        }

        if (IsScriptLoaded()) {
            char sFuncName[50] = {0};
            rt2_snprintf(sFuncName, 50, "OnCancelTask%d", TaskId);
            CallScript(sFuncName, false);
        }

        UpdateToClient(NET_CRE_UPDATE_TASK);
        std::string msg = R(MSG_TASK_CANCELTASK) + desc->name;
        SendSystemMessage(msg.c_str());
        NetSend_r2c_task_event(0);
    }
    return false;
}

bool CRegionCreature::AddReward(int rewardId) {
    SFBReward* rw = g_TableFBReword.Find(rewardId);
    if (!rw)
        return false;

    if (rw->exp != 0) {
        AddExp(rw->exp, 0);
        return true;
    } else if (rw->gold != 0) {
        AddGold(rw->gold);
        return true;
    } else if (rw->itemId != 0) {
        EItemColor color = ItemColor_White;
        if (rw->color == "green")
            color = ItemColor_Green;
        else if (rw->color == "blue")
            color = ItemColor_Blue;

        EItemBind tmpBind = IB_NotBind;
        if (rw->bindType == 1) {
            tmpBind = IB_Binded;
        } else if (rw->bindType == 2) {
            tmpBind = IB_BindWhenEquip;
        }

        SItemID item =
            g_region->m_pItemManager->CreateItem(rw->itemId, color, 0, tmpBind, rw->colorParam1,
                                                 rw->colorRate, rw->colorParam2, rw->colorRate);

        //if(!ItemCanNpcTrade(item.type)) return false;
        if (!ItemID_IsValid(item))
            return false;
        return AddItem(item);
    }
    return false;
}

bool CRegionCreature::ProcessTaskKeyList(vector<STaskKey>& table, int op, vector<long>* param,
                                         STaskDesc* pDesc) {
    bool failed = false;
    for (size_t i = 0; i < table.size(); i++) {
        if (!ProcessTaskKey(table[i], op, param, pDesc))
            failed = true;
        if (failed && op == TYPE_CHECK)
            return false;
    }
    return !failed;
}

bool CRegionCreature::ProcessTaskKey(STaskKey& key, int op, vector<long>* param, STaskDesc* pDesc) {
    /*Tianh  增加GM命令增加灵力值*/
    //if (key.key == "add soul")
    //{
    //	int teamId = atoi(key.param[0].c_str());
    //	int monsterNum = atoi(key.param[1].c_str());
    //
    //	return true;
    //}
    if (key.key == "fbprocess") {
        int process = atoi(key.param[0].c_str());
        if (process > m_nFBprocess)
            return false;
        return true;
    }

    if (key.key == "addteam") {
        int teamId     = atoi(key.param[0].c_str());
        int monsterNum = atoi(key.param[1].c_str());
        m_scene->AddTeam(teamId, monsterNum);
        return true;
    }

    if (key.key == "removeteam") {
        int teamId     = atoi(key.param[0].c_str());
        int monsterNum = atoi(key.param[1].c_str());
        m_scene->RemoveTeam(teamId, monsterNum);
        return true;
    }

    if (key.key == "item") {
        long itemId;
        itemId = atol(key.param[0].c_str());
        if (itemId == 0)
            return true;

        long itemNum;
        itemNum = atol(key.param[1].c_str());
        if (itemNum == 0)
            return true;

        EItemColor color = ItemColor_White;
        if (key.param[2] == "all")
            color = ItemColor_All;
        else if (key.param[2] == "white")
            color = ItemColor_White;
        else if (key.param[2] == "green")
            color = ItemColor_Green;
        else if (key.param[2] == "blue")
            color = ItemColor_Blue;

        switch (op) {
            case TYPE_CHECK:
                if (GetItemNum(itemId, color) < itemNum)
                    return false;
                else
                    return true;
                break;

            case TYPE_ADD:
                for (int i = 0; i < itemNum; i++)
                    AddItem(itemId, color);
                return true;
                break;

            case TYPE_REMOVE:
                if (NULL != param) {
                    if (param->size() > 0) {
                        if (1 == (*param)[0]) {
                            SItemBase* pItemClass =
                                g_region->m_pItemManager->GetItemFromIndex(itemId);
                            if (NULL != pItemClass) {
                                //if (ItemIsMissionItem(pItemClass->type))
                                { return RemoveItem(itemId, -itemNum, color); }
                            }
                        }
                    }
                }
                SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(itemId);
                if (NULL != pItemClass) {
                    //if (ItemIsMissionItem(pItemClass->type))
                    { return RemoveItem(itemId, itemNum, color); }
                }
                break;
        }
    }
    if (key.key == "pet") {
        long petId;
        petId = atol(key.param[0].c_str());
        if (petId == 0)
            return true;

        long petNum;
        petNum = atol(key.param[1].c_str());
        if (petNum == 0)
            return true;

        switch (op) {
            case TYPE_CHECK:
                if (GetPetNum(petId) < petNum)
                    return false;
                else
                    return true;
                break;

            case TYPE_ADD:
                for (int i = 0; i < petNum; i++)
                    AddPetToBag(petId);
                return true;
                break;

            case TYPE_REMOVE:
                if (NULL != param) {
                    if (param->size() > 0) {
                        if (1 == (*param)[0]) {
                            return RemovePetFromBag(petId, -petNum);
                        }
                    }
                }
                return RemovePetFromBag(petId, petNum);
                break;
        }
    }
    if (key.key == "sreward") {
        if (!param || param->size() < 2)
            return false;
        long sreward = (*param)[1];
        bool valid   = false;

        for (int k = 0; k < MAX_TASK_PARAM_SIZE; k++) {
            long rId;
            rId = atol(key.param[k].c_str());
            if (rId == sreward && sreward > 0)
                valid = true;
        }
        switch (op) {
            case TYPE_CHECK:
                return valid;
                break;

            case TYPE_ADD:
                if (!valid)
                    return false;
                return AddReward(sreward);
                break;

            case TYPE_REMOVE:
                return valid;
                break;
        }
    }
    /*else if(key.key == "credit")
	{
		int num;
		num = atol(key.param[0].c_str());
		switch(op)
		{
		case TYPE_CHECK:
			if(GetCredit() < num)
				return false;
			else
				return true;
			break;

		case TYPE_ADD:
			AddCredit(num);
			return true;
			break;

		case TYPE_REMOVE:
			RemoveCredit(num);
			return true;
			break;
		}
	}*/
    else if (key.key == "beginmove") {
        m_ai->SetLvFlag(AI_PATROL);
    } else if (key.key == "killself") {
        long lTime = atol(key.param[0].c_str());
        SetTimer(CRE_TIMER_KILL_SELF, lTime > 0 ? lTime : 1000);
        m_valid = false;
        return true;
    } else if (key.key == "allcredit") {
        int num;
        num = atol(key.param[0].c_str());
        TObjectHash::iterator it;
        for (it = m_scene->m_userHash.begin(); it != m_scene->m_userHash.end(); it++) {
            CRegionCreature* cre = (CRegionCreature*)(it->second);
            cre->AddCredit(num);
        }
        return true;
    } else if (key.key == "summon") {
        if (!m_valid)
            return false;
        int npcId = atol(key.param[0].c_str());
        int cnt   = atol(key.param[1].c_str());
        for (int i = 0; i < cnt; i++) {
            CRegionCreature* cre = g_region->CreateNpc(npcId);
            if (cre) {
                long pos[3];
                pos[0] = atol(key.param[2].c_str());
                pos[1] = atol(key.param[3].c_str());
                if (!m_scene->AddCreature(cre, pos, 0)) {
                    g_factory->DestroyObject(cre);
                } else {
                    if (cre->IsScriptLoaded())
                        cre->CallScript("OnCreate", false);

                    cre->m_ai->SetLvFlag(AI_PATROL);
                }
            }
        }
    } else if (key.key == "gold") {
        int num;
        num = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_CHECK:
                if (GetGold() < num)
                    return false;
                else
                    return true;
                break;

            case TYPE_ADD:
                AddGold(num);
                //GetPlayerLogic()->AddGold(num);
                m_nGetMoneyFromTask += num;
                return true;
                break;

            case TYPE_REMOVE:
                RemoveGold(num);
                return true;
                break;
        }
    } else if (key.key == "idle") {
        if (m_ai) {
            m_ai->SetLvFlag(AI_IDLE);
            m_ai->SetState(CRegionAI::STATE_MOVE);
            m_ai->SetStartNode(m_pos);
        }
    } else if (key.key == "team") {
        if (m_pTeam->mData.Empty()) {
            return false;
        } else {
            return true;
        }
        /*
		switch(op)
		{
		case TYPE_CHECK:
			if(GetGold() < num)
				return false;
			else
				return true;
			break;

		case TYPE_ADD:
			AddGold(num);
			m_nGetMoneyFromTask += num;
			return true;
			break;

		case TYPE_REMOVE:
			RemoveGold(num);
			return true;
			break;
		}
		*/
    } else if (key.key == "setlev") {
        int lev;
        lev = atol(key.param[0].c_str());
        SetLev(lev);
    } else if (key.key == "exp") {
        int num;
        num = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_ADD:
                AddExp(num, 0);
                return true;
                break;
        }
    } else if (key.key == "task") {
        int num;
        num = atol(key.param[0].c_str());

        //Cheney.2009.8.11 增加职业区分赠送任务
        int AwardClass = 0;
        if (!key.param[1].empty())
            AwardClass = atol(key.param[1].c_str());

        switch (op) {
            case TYPE_CHECK:
                return m_task.IsTaskAccept(num);

            case TYPE_ADD:
                if (AwardClass != 0 && AwardClass != m_core.Metier)
                    return false;
                return AcceptTask(num);

                break;
        }
    } else if (key.key == "lvlexp") {
        int lev;
        lev = atol(key.param[0].c_str());
        if (lev == 0)
            lev = m_core.Lev;

        int   add;
        DWORD next = g_TableExp.GetNextLevExp(lev);
        switch (op) {
            case TYPE_ADD:
                add = next * atof(key.param[1].c_str());
                // if(add > next - m_core.Exp)
                //	add = next - m_core.Exp;
                AddExp(add, 0);
                return true;
        }
    } else if (key.key == "lvlexpsm") {
        int lev;
        lev = atol(key.param[0].c_str());
        if (lev == 0)
            lev = m_core.Lev;

        int   add = 0, tmp = 0, cnt = 0;
        DWORD next = g_TableExp.GetNextLevExp(lev);
        add        = next * atof(key.param[1].c_str());
        tmp        = add;
        while (tmp >= 10) {
            tmp /= 10;
            cnt++;
        }
        tmp = 1;
        cnt--;
        while (cnt-- > 0) {
            tmp *= 10;
        }
        add = (add / tmp) * tmp;
        switch (op) {
            case TYPE_ADD:
                AddExp(add, 0);
                return true;
        }
    } else if (key.key == "ritem") {
        int id[3];
        for (int i = 0; i < 3; i++)
            id[i] = atol(key.param[2 + i].c_str());

        int num;
        num = atol(key.param[0].c_str());

        EItemColor color = ItemColor_All;
        if (key.param[1] == "all")
            color = ItemColor_All;
        else if (key.param[1] == "white")
            color = ItemColor_White;
        else if (key.param[1] == "green")
            color = ItemColor_Green;
        else if (key.param[1] == "blue")
            color = ItemColor_Blue;

        switch (op) {
            case TYPE_CHECK:
                for (int i = 0; i < 3; i++) {
                    if (GetItemNum(id[i], color) >= num)
                        return true;
                }
                return false;
                break;

            case TYPE_REMOVE:
                for (int i = 0; i < 3; i++) {
                    if (RemoveItem(id[i], num, color))
                        return true;
                }
                return false;
                break;

            case TYPE_ADD:
                int ret;
                ret = GetRand(2, 0);
                while (num--)
                    AddItem(id[ret], color);
                return true;
                break;
        }
    } else if (key.key == "mlev") {
        int lev;
        lev = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_CHECK:
                if (lev == -1)
                    return true;
                if (m_core.MetierLev == lev)
                    return true;
                return false;
                break;

            case TYPE_REMOVE:
                break;

            case TYPE_ADD:
                if (lev < 0 || lev > 4)
                    return false;
                m_core.MetierLev = lev;

                SUserActor* actor;
                actor = g_TableUserActor.FindUserActor(m_core.Metier, m_core.MetierLev);
                if (actor) {
                    SCreModel* model = g_TableCreModel.FindCreModel(actor->ModelId);
                    if (!model)
                        return NULL;

                    m_model             = model;
                    m_core.ModelId      = model->Id;
                    m_core.ComplexModel = model->bComplex;
                    g_sendCmd->BeginWrite();
                    g_sendCmd->WriteShort(r2c_change_model);
                    g_sendCmd->WriteShort(m_core.ModelId);
                    RecvCmd(g_sendCmd);
                }
                return true;
                break;
        }
    } else if (key.key == "unionscore")  // 诸侯国积分
    {
        int num = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_CHECK: {
                if (m_unionID == 0)
                    return false;
                EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion =
                    g_region->m_unionMap.find(m_unionID);
                if (iterUnion == g_region->m_unionMap.end())
                    return false;
                //是否修改成帮派资金
                // 				SUnionData& unionData = iterUnion->second;
                // 				if (unionData.score >= num)
                // 					return true;
                // 				else
                // 					return false;
            } break;
            case TYPE_ADD: {
                if (m_unionID == 0)
                    return false;
                EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion =
                    g_region->m_unionMap.find(m_unionID);
                if (iterUnion == g_region->m_unionMap.end())
                    return false;

                CG_CmdPacket& packet = g_region->m_gws.BeginSend();
                packet.WriteShort(r2g_union);
                packet.WriteShort(r2g_union_add_score);
                packet.WriteLong(m_userInfo->m_userId);
                packet.WriteLong(m_unionID);
                packet.WriteLong(num);
                g_region->m_gws.EndSend();

                return true;
            }
            default:
                break;
        }
    } else if (key.key == "unionlev")  // 诸侯国等级
    {
        // 		int num = atol(key.param[0].c_str());
        // 		switch(op)
        // 		{
        // 		case TYPE_CHECK:
        // 			{
        // 				if (m_unionID==0)
        // 					return false;
        // 				if (IsUnionOperateLocked(m_unionID))
        // 					return false;
        // 				EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion = g_region->m_unionMap.find(m_unionID);
        // 				if (iterUnion==g_region->m_unionMap.end())
        // 					return false;
        // 				SUnionData& unionData = iterUnion->second;
        // 				if (unionData.level == num)
        // 					return true;
        // 				else
        // 					return false;
        // 			}
        // 			break;
        // 		case TYPE_ADD:
        // 			{
        // 				if (m_unionID==0)
        // 					return false;
        // 				EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion = g_region->m_unionMap.find(m_unionID);
        // 				if (iterUnion==g_region->m_unionMap.end())
        // 					return false;
        //
        // 				CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        // 				packet.WriteShort(r2g_union);
        // 				packet.WriteShort(c2r_union_update_level);
        // 				packet.WriteLong(m_userInfo->m_userId);
        // 				g_region->m_gws.EndSend();
        //
        // 				LockUnionOperate(m_unionID, true);
        //
        // 				return true;
        // 			}
        // 		default:
        // 			break;
        // 		}
    } else if (key.key == "ifree") {
        long num;
        num = atol(key.param[0].c_str());
        if (num == 0)
            return true;

        if (!m_pItemContainer)
            return 0;
        return (m_pItemContainer->m_Bag.FreeSpace() >= num);
    } else if (key.key == "fbitem") {
        long itemId;
        itemId = atol(key.param[0].c_str());
        if (itemId == 0)
            return true;

        long itemNum;
        itemNum = atol(key.param[1].c_str());
        if (itemNum == 0)
            return true;

        if (!param || param->size() < 1)
            return false;
        int dif = (*param)[0];
        switch (op) {
            case TYPE_CHECK:
                if (GetDungeonMissionItemNum(itemId, dif) < itemNum)
                    return false;
                else
                    return true;
                break;

            case TYPE_ADD:
                for (int i = 0; i < itemNum; i++)
                    AddDungeonMissionItem(itemId, dif);
                return true;
                break;

            case TYPE_REMOVE:
                return RemoveDungeonMissionItem(itemId, itemNum, dif);
                break;
        }
    } else if (key.key == "itemex") {
        long itemId;
        itemId = atol(key.param[0].c_str());
        if (itemId == 0)
            return true;

        long itemNum;
        itemNum = atol(key.param[1].c_str());
        if (itemNum == 0)
            return true;

        EItemColor color = ItemColor_White;
        if (key.param[2] == "white")
            color = ItemColor_White;
        else if (key.param[2] == "green")
            color = ItemColor_Green;
        else if (key.param[2] == "blue")
            color = ItemColor_Blue;
        else if (key.param[2] == "golden")
            color = ItemColor_Golden;

        long nMinites = atol(key.param[3].c_str());
        // if(nMinites == 0) return true;

        short Att1 = 0;
        if (!key.param[4].empty())
            Att1 = atol(key.param[4].c_str());

        short Att2 = 0;
        if (!key.param[5].empty())
            Att2 = atol(key.param[5].c_str());

        unsigned char AttRate = 0;
        if (!key.param[6].empty())
            AttRate = atol(key.param[6].c_str());

        int EquipLevel = 0;
        if (!key.param[7].empty())
            EquipLevel = atol(key.param[7].c_str());
        if (EquipLevel < 0 || EquipLevel > 10)
            EquipLevel = 0;

        //Cheney.2009.8.11 增加职业区分赠送装备
        int AwardClass = 0;
        if (!key.param[8].empty())
            AwardClass = atol(key.param[8].c_str());

        switch (op) {
            case TYPE_CHECK:
                break;

            case TYPE_ADD: {
                //Cheney.2009.8.11 增加职业区分赠送装备
                if (AwardClass != 0 && AwardClass != m_core.Metier)
                    return false;

                DWORD limitTime = 0;
                if (nMinites)
                    limitTime = time(NULL) + nMinites * 60;
                for (int i = 0; i < itemNum; i++) {
                    SItemID item = g_region->m_pItemManager->CreateItem(itemId, color, EquipLevel,
                                                                        IB_NotBind, Att1, AttRate,
                                                                        Att2, AttRate, limitTime);
                    if (!ItemID_IsValid(item))
                        return false;
                    AddItem(item);
                }
                return true;
            } break;

            case TYPE_REMOVE:
                break;
        }
    } else if (key.key == "skill") {
        long skillID;
        skillID = atol(key.param[0].c_str());
        if (skillID == 0)
            return false;
        switch (op) {
            case TYPE_CHECK:
                return (Skill()->FindSkill(skillID) != NULL);

            case TYPE_ADD: {
                SSkill* pS = Skill()->FindSkill(skillID);
                if (NULL != pS) {
                    if (m_Skill.AddSkill(skillID, 0, true)) {
                        //std::string skillMsg = "";
                        //skillMsg = R(MSG_SKILL_LEARNNEWSKILL) + std::string(pS->szName);

                        const int c_nBuffSize = 128;
                        char      szBuff[c_nBuffSize];
                        try {
                            rt2_sprintf(szBuff, R(MSG_SKILL_LEARNNEWSKILL), pS->szName);
                            SendSystemMessage(szBuff);
                        } catch (...) {
                            return false;
                        }

                        return true;
                    }
                }
                return false;
            }

            case TYPE_REMOVE:
                if (Skill()->FindSkill(skillID)) {
                    if (m_Skill.DeleteSkill(skillID, true)) {
                        return true;
                    }
                }
                break;
        }
    } else if (key.key == "effect") {
        long effectID;
        effectID = atol(key.param[0].c_str());
        if (effectID == 0)
            return false;
        switch (op) {
            case TYPE_CHECK:
                return (Skill()->FindEffect(effectID) != NULL);

            case TYPE_ADD:
                if (Skill()->FindEffect(effectID)) {
                    if (m_Skill.AddEffect(effectID, 0, true)) {
                        return true;
                    }
                }
                return false;

            case TYPE_REMOVE:
                if (Skill()->FindEffect(effectID)) {
                    if (m_Skill.DeleteEffect(effectID, true)) {
                        return true;
                    }
                }
                break;
        }
    }
    /*
	else if(key.key == "wenaccept")
	{
		switch(op)
		{
			case TYPE_CHECK: return true;
			case TYPE_REMOVE: m_wenTaskAccept++; return true;
		}
	}
	*/
    else if (key.key == "wenreward") {
        //int exp = 0;
        int vc = m_wenTaskFinish % 10;

        float fR    = 1.1f + (vc - 6) / 10.0f;
        int   nBase = 0;
        if (m_core.Lev < 20) {
            nBase = 1184;
        } else if (m_core.Lev < 25) {
            nBase = 1697;
        } else if (m_core.Lev < 30) {
            nBase = 2300;
        } else {
            nBase = 2993;
        }

        int nCul = (int)(nBase * fR);
        if (vc == 9) {
            nCul = nCul + nBase / 2;
            //fR = fR * 1.5f;
        }
        AddCulExp(nCul);
        if (19 == m_wenTaskFinish) {
            AddItem(6229, ItemColor_White);
        }
        /*int reward[5][10] = {
			{4594,	5011,	5429,	5847,	6264,	6682,	7100,	7517,	7935,	8353},
			{7744,	8448,	9152,	9856,	10560,	11264,	11968,	12672,	13376,	14080},
			{16542,	18046,	19550,	21054,	22558,	24062,	25566,	27070,	28574,	30078},
			{24012,	26195,	28378,	30561,	32744,	34927,	37110,	39293,	41476,	43659},
			{31775,	34664,	37553,	40441,	43330,	46219,	49107,	51996,	54885,	57774},
		};
		if(m_core.Lev < 30)
		{
			exp = reward[0][vc];
		}
		else if(m_core.Lev < 40)
		{
			exp = reward[1][vc];
		}
		else if(m_core.Lev < 50)
		{
			exp = reward[2][vc];
		}
		else if(m_core.Lev < 60)
		{
			exp = reward[3][vc];
		}
		else
		{
			exp = reward[4][vc];
		}*/

        /*
		int reward[5][10] = {
			{6891,	7518,	8114,	8771,	9397,	10024,	10650,	11277,	11903,	12530},
			{11616,	12672,	13728,	14784,	15840,	16896,	17952,	19008,	20064,	21120},
			{24814,	27070,	29326,	31582,	33838,	36094,	38350,	40605,	42861,	45117},
			{36019,	39293,	42568,	45842,	49116,	52391,	55665,	58940,	62214,	65489},
			{47664,	51997,	56330,	60663,	64996,	69329,	73662,	77995,	82328,	86661},
		};

		if(m_core.Lev < 20)
		{
			exp = reward[0][vc];
			AddItem(767,ItemColor_White);
			AddItem(772,ItemColor_White);
			if(vc == 9)
			{
				if(GetProb(1)) AddItem(1085,ItemColor_White);
				if(GetProb(0.001)) AddItem(1101,ItemColor_Blue);
			}
		}
		else if(m_core.Lev < 25)
		{
			exp = reward[1][vc];
			AddItem(767,ItemColor_White);
			AddItem(772,ItemColor_White);
			if(vc == 9)
			{
				if(GetProb(1)) AddItem(1085,ItemColor_White);
				if(GetProb(0.001)) AddItem(1101,ItemColor_Blue);
			}
		}
		else if(m_core.Lev < 30)
		{
			exp = reward[2][vc];
			AddItem(768,ItemColor_White);
			AddItem(773,ItemColor_White);
			if(vc == 9)
			{
				if(GetProb(1)) AddItem(1086,ItemColor_White);
				if(GetProb(0.001)) AddItem(1101,ItemColor_Blue);
			}
		}
		else if(m_core.Lev < 35)
		{
			exp = reward[3][vc];
			AddItem(768,ItemColor_White);
			AddItem(773,ItemColor_White);
			if(vc == 9)
			{
				if(GetProb(1)) AddItem(1086,ItemColor_White);
				if(GetProb(0.001)) AddItem(1101,ItemColor_Blue);
			}
		}
		else
		{
			exp = reward[4][vc];
			AddItem(769,ItemColor_White);
			AddItem(774,ItemColor_White);
			if(vc == 9)
			{
				if(GetProb(1)) AddItem(1087,ItemColor_White);
				if(GetProb(0.001)) AddItem(1101,ItemColor_Blue);
			}
		}
		*/

        //		exp *= 30;
        //		if(exp < 0) exp = 0;
        //		if(exp > 1000000) exp = 1000000;
        //		AddExp(exp,0);
        //
        //        if (vc == 9)
        //        {
        //            if(GetProb(0.15))
        //            {
        //                AddItem(430,ItemColor_White);
        //            }
        //			else if(GetProb(0.15))
        //			{
        //				AddItem(431,ItemColor_White);
        //			}
        ////             if(GetProb(0.5))
        ////             {
        ////                 SItemID item = g_region->m_pItemManager->CreateItem(1112, ItemColor_Blue, 0, IB_NotBind, 8047, 100);
        ////                 if(ItemID_IsValid(item))
        ////                 {
        ////                     DWORD now = time(NULL);
        ////                     DWORD tLife = now + 2*3600;
        ////                     item.paramCount = ITEMSHIPIN_PARAM_SIZE;
        ////                     item.params[ITEMSHIPIN_PARAM_LIFE] = tLife;
        ////                     AddItem(item);
        ////                 }
        ////             }
        //        }
        // AddGold(exp/10);
        m_wenTaskFinish++;
        return true;
    } else if (key.key == "gzaddzg") {
        //int num;
        //num = atol(key.param[0].c_str());
        //switch(op)
        //{
        //	case TYPE_ADD:
        //		g_WarfieldMgr.AddExploit(m_unionID,m_core.Faction,num);
        //		return true;
        //}
    } else if (key.key == "gzaddjf") {
        int num;
        num = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_ADD:
                g_WarfieldMgr.AddUserScore(m_userInfo->m_userId, num);
                return true;
        }
    } else if (key.key == "gzaddgod") {
        int num, idx;
        idx = atol(key.param[0].c_str());
        num = atol(key.param[1].c_str());
        switch (op) {
            case TYPE_CHECK:
                return g_WarfieldMgr.CanConjureHero(m_core.Faction, idx);

            case TYPE_ADD:
                //g_WarfieldMgr.ConjureHero(m_core.Faction,num,idx);
                g_WarfieldMgr.ConjureHero((long)m_userInfo->m_userId, num, idx);
                return true;
        }
    }
    //---------add start by tony 06.05.09-------------------------------//
    //-reason:新添加跑环任务，所以新加了关键字circle_item\circle_hold\circle_monster\circle\circle_exp\circle_task
    //- circle_item: 仅用于完成任务时 寻物：只需找到道具即可完成
    //- circle_hold: 仅用于完成任务时 找人：只需拥有这个人物即可完成
    //- circle_monster: 仅用于完成任务时   战斗：只需杀死特定数量的怪物即可完成
    //- circle: 仅用于检查是否能接受任务
    //- circle_exp: 仅用于跑环任务奖励
    //- circle_task: 仅用于跑环任务奖励
    else if (key.key == "circle_item") {
        int        id   = atol(key.param[0].c_str());
        int        num  = atol(key.param[1].c_str());
        STaskInfo* info = m_task.FindTask(id);
        switch (op) {
            case TYPE_CHECK: {
                if (info && info->Desc && info->Desc->circleTask != 0) {
                    if (GetItemNum(info->m_finditemID, ItemColor_White) < num)
                        return false;

                    m_tmpfinditemID = info->m_finditemID;

                    /* return true; */  // 改动：添加挂起功能
                    return (m_task.m_isActive == 1);
                } else {
                    LOG1("Task Error: Cannot find task [%d]\n", id);
                    return false;
                }
            }

            case TYPE_ADD:
                LOG("Task Error: unexpected keyword: [circle_item]\n");
                //CHECK(0);
                return true;
            case TYPE_REMOVE:
                //return RemoveItem(info->m_finditemID,num,ItemColor_White);
                if (RemoveItem(m_tmpfinditemID, num, ItemColor_White)) {
                    LOG1("RemoveItem id[%d]\n", m_tmpfinditemID);
                    return true;
                } else {
                    LOG2("RemoveItem Error para[%d, %d]\n", m_tmpfinditemID, num);
                    return false;
                }
                break;
        }
    } else if (key.key == "circle_hold") {
        int        id   = atol(key.param[0].c_str());
        STaskInfo* info = m_task.FindTask(id);
        if (!info)
            return false;
        switch (op) {
            case TYPE_CHECK: {
                if (param && !param->empty()) {
                    // 改动：添加挂起功能
                    if ((m_task.IsTaskAccept(id) && info->m_findnpcID == param->front())) {
                        return (m_task.m_isActive == 1);
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            case TYPE_ADD:
                LOG("Task Error: unexpected keyword: [circle_hold]\n");
                CHECK(0);
                return true;
        }
    } else if (key.key == "circle_monster") {
        int id = 0, num = 0;
        id  = atol(key.param[0].c_str());
        num = atol(key.param[1].c_str());
        switch (op) {
            case TYPE_CHECK: {
                STaskInfo* info = m_task.FindTask(id);
                if (info && info->Desc && info->Desc->circleTask != 0) {
                    // 改动：添加挂起功能
                    if ((info->m_currMonsterNum >= num)) {
                        return (m_task.m_isActive == 1);
                    } else {
                        return false;
                    }
                } else {
                    LOG1("Task Error: Cannot find task [%d]\n", id);
                    return false;
                }
            }

            case TYPE_ADD:
                LOG("Task Error: unexpected keyword: [circle_monster]\n");
                CHECK(0);
                return true;
        }
    } else if (key.key == "circle") {
        int id = 0;
        id     = atol(key.param[0].c_str());
        switch (op) {
            case TYPE_CHECK: {
                return (m_task.m_currCircle == 0);
            }

            case TYPE_ADD:
                LOG("Task Error: unexpected keyword: [circle]\n");
                return true;
        }
    } else if (key.key == "circle_exp") {
        //const int expRate[50] = {
        //	1, 2, 3, 4, 5, 6, 8, 8, 9, 10,
        //	6, 7, 8, 9, 10,11,12,13,14,15,
        //	11,12,13,14,15,16,17,18,19,20,
        //	16,17,18,19,20,21,22,23,24,25,
        //	21,22,23,24,25,26,27,28,29,30
        //};

        int   id   = atol(key.param[0].c_str());
        DWORD next = g_TableExp.GetNextLevExp(m_core.Lev);
        switch (op) {
            case TYPE_CHECK:
                LOG("Task Error: unexpected task keyword: [circle_exp]\n");
                return true;

            case TYPE_ADD: {
                //STaskInfo* info = m_task.FindTask(id);
                //if(info && info->Desc)
                //{
                if (m_task.m_currCircle > 0 && m_task.m_currCircle <= g_tcTotalCircle) {
                    if (m_task.m_currCircle <= (int)g_tcexpRate.size()) {
                        int addexp = next * ((float)g_tcexpRate[m_task.m_currCircle - 1] / 10000);
                        if (m_core.Lev >= 80 && m_core.Lev < 84) {
                            addexp /= 2;
                        } else if (m_core.Lev >= 85 && m_core.Lev <= 90) {
                            addexp /= 4;
                        }
                        AddExp(addexp, 0);
                    } else {
                        LOG("Task Error: expRate error\n");
                    }
                    return true;
                } else {
                    LOG1("Task Error: invalid currCircle [%d]\n", m_task.m_currCircle);
                    return true;
                }
                //}else
                //{
                //	LOG1("Task Error: Cannot find task [%d]\n", id);
                //	return false;
                //}
                return true;
            }
        }
    } else if (key.key == "circle_task") {
        //const int itemID[] = {71, 72, 13, 14, 46, 47};
        //const int npcID[] = {1287,1288,1289,1290,1291};
        //const int monsterID[]={1,2,3};
        int id     = atol(key.param[0].c_str());
        int npcoID = 0;
        if (param && !param->empty()) {
            npcoID = param->front();
        }
        CRegionCreature* pNpc = g_region->m_defScene->FindCreature(npcoID);
        if (!pNpc) {
            LOG("Finish Task Error: cannot find srcnpc\n");
            return false;
        }
        switch (op) {
            case TYPE_CHECK:
                return m_task.IsTaskAccept(id);

            case TYPE_ADD: {
                if (m_task.m_currCircle >= g_tcTotalCircle) {
                    m_task.m_currCircle = 0;
                    m_task.m_isActive   = 1;
                    m_tmpcurrCircle     = 0;
                    m_tmpcurrMonsterNum = 0;
                    m_tmpfinditemID     = 0;
                    m_tmpfindnpcID      = 0;
                    m_tmpkillmonsterID  = 0;
                    return true;
                }

                long ret = TASK_STATE_CAN_ACCEPT;

                int rnum = rand() % 100;
                int rnd  = 0;

                if (rnum < 50) {
                    rnd = TASK_CIRCLE_ITEM - 1;
                } else if (rnum >= 50 && rnum < 75) {
                    rnd = TASK_CIRCLE_FINDNPC - 1;
                } else {
                    rnd = TASK_CIRCLE_BATTLE - 1;
                }

                STaskDesc* desc = g_TableTask.FindTask(pNpc->m_acceptTask[rnd]->id);
                if (NULL == desc) {
                    return false;
                }
                if (m_core.Lev < desc->reqMinLev || m_core.Lev > desc->reqMaxLev) {
                    //CancelTask(pNpc->m_acceptTask[rnd-1]->id,true);
                    m_task.m_currCircle = 0;
                    m_tmpcurrCircle     = 0;
                    m_tmpcurrMonsterNum = 0;
                    m_tmpfinditemID     = 0;
                    m_tmpfindnpcID      = 0;
                    m_tmpkillmonsterID  = 0;
                    return false;
                }

                switch (rnd + 1) {
                    case TASK_CIRCLE_ITEM: {
                        int size = (int)g_tcrndItemID.size();
                        int r    = rand() % size;

                        m_tmpfinditemID = g_tcrndItemID[r];
                        SItemBase* pBase =
                            g_region->m_pItemManager->GetItemFromIndex(m_tmpfinditemID);
                        if (NULL == pBase) {
                            return false;
                        }
                        g_region->ShowHtmlDialog(pNpc, this,
                                                 pNpc->m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(),
                                                 "FIND_ITEM", (char*)pBase->name);
                    } break;
                    case TASK_CIRCLE_FINDNPC: {
                        int size = (int)g_tcrndNpcID.size();
                        int r    = rand() % size;

                        while (g_tcrndNpcID[r] == pNpc->m_npcInfo->Id)
                            r = rand() % size;
                        m_tmpfindnpcID = g_tcrndNpcID[r];

                        SNpc* pTaskNpc = g_TableNpc.FindNpc(m_tmpfindnpcID);
                        if (NULL == pTaskNpc) {
                            return false;
                        }
                        g_region->ShowHtmlDialog(pNpc, this,
                                                 pNpc->m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(),
                                                 "FIND_NPC", (char*)pTaskNpc->Name.c_str());
                    } break;
                    case TASK_CIRCLE_BATTLE: {
                        int size = (int)g_tcrndMonsterID.size();
                        int r    = rand() % size;

                        m_tmpkillmonsterID = g_tcrndMonsterID[r];
                        SNpc* pMonster     = g_TableNpc.FindNpc(m_tmpkillmonsterID);
                        g_region->ShowHtmlDialog(pNpc, this,
                                                 pNpc->m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(),
                                                 "KILL_MONSTER", (char*)pMonster->Name.c_str());
                    } break;
                }

                return AcceptTask(pNpc->m_acceptTask[rnd]->id, pNpc);
            }
        }
    } else if (key.key == "on_arrive") {
        switch (m_ai->GetType()) {
            case CRegionAI::TYPE_TOUR:
                if (IsScriptLoaded() && !IsUser() && !IsUserPet()) {
                    CallScript("OnArrive", false);
                }
                break;
        }
    }

    //---------add end   by tony 06.05.09-------------------------------//
    //PZH
    int nT = GetPlayerLogic()->ProcessTaskKey(key, op, param, pDesc);
    switch (nT) {
        case 0:
            return true;
        case 1:
        case 2:
            return false;
        default:
            break;
    }
    //

    ERR1("unknow key %s", key.key.c_str());

    return false;
}

//---------chg start by tony 06.05.10--------------------------------------------//
#if 0
@ bool CRegionCreature::FinishTask(STaskDesc* task, int idx)
#else
bool CRegionCreature::FinishTask(STaskDesc* task, int idx, CRegionCreature* npc)
#endif
//---------chg end   by tony 06.05.10--------------------------------------------//
{
    vector<STaskKey>*req = NULL, *addReward = NULL;

    if (task->finishType == TASK_FINISH_TYPE_NORMAL) {
        req = &task->taskReq;
    } else if (task->finishType == TASK_FINISH_TYPE_SELECT) {
        req       = &task->specReq[idx];
        addReward = &task->specReward[idx];
    } else {
        return false;
    }

    if (m_task.FinishTask(task)) {
        if (IsScriptLoaded()) {
            char sFuncName[50] = {0};
            rt2_snprintf(sFuncName, 50, "OnFinishTask%d", task->id);
            if (npc)
                CallScript(sFuncName, false, npc);
            else
                CallScript(sFuncName, false, 0);
        }

        // remove task request item
        ProcessTaskKeyList(*req, TYPE_REMOVE, 0);

        // remove task flag item
        RemoveTaskItem(task);
        // ProcessTaskKeyList(task->flagItem,TYPE_REMOVE);

        // give reward
        //---------chg start by tony 06.05.10--------------------------------------------//
#if 0
		@		ProcessTaskKeyList(task->fixReward, TYPE_ADD, 0);
#else
        std::vector<long> param;
        if (npc && npc->m_npcInfo) {
            param.push_back(npc->m_oId);
        }
        std::string msg = R(MSG_TASK_FINISHTASK) + task->name;
        SendSystemMessage(msg.c_str());

        ProcessTaskKeyList(task->fixReward, TYPE_ADD, &param);

        if (0 < idx && idx <= task->selectReward.size()) {
            ProcessTaskKey(task->selectReward[idx - 1], TYPE_ADD, NULL, task);
        }
        int nJobSie = task->jobReward.size();
        int nIndex  = m_core.Metier - 1;
        if (0 <= nIndex && nIndex < nJobSie) {
            ProcessTaskKey(task->jobReward[nIndex], TYPE_ADD, NULL, task);
        }
#endif
        //---------chg end   by tony 06.05.10--------------------------------------------//

        if (GetProb(task->randomRewardRate) && !task->randomReward.empty()) {
            ProcessTaskKey(task->randomReward[GetRand((int)task->randomReward.size() - 1, 0)],
                           TYPE_ADD, 0);
        }
        if (addReward && GetProb(task->specRewardRate))
            ProcessTaskKeyList(*addReward, TYPE_ADD, 0);

        if (task->moneyTask) {
            m_mtFinish[task->moneyTask - 1]++;
        }
        GetPlayerLogic()->AfterFinishTask(task);
        UpdateToClient(NET_CRE_UPDATE_TASK);
        NetSend_r2c_task_event(1);
        return true;
    }

    return false;
}

//-----------chg start by tony 06.05.09--------------------------------------------------------------//
#if 0
@ bool CRegionCreature::TaskProcess(STaskDesc* task, const char* str, char** link)
#else
bool CRegionCreature::TaskProcess(STaskDesc* task, const char* str, char** link,
                                  CRegionCreature* npc)
#endif
//-----------chg end   by tony 06.05.09--------------------------------------------------------------//
{
    if (task->acceptFlag == str) {
        //-----------chg start by tony 06.05.09--------------------------------------------------------------//
#if 0
		@		if (CanAcceptTask(task, true))
			@		{
			@			AcceptTask(task);
			@* link = (char*)task->npc1AfterAcceptTalk.c_str();
			@			return true;
			@		}
#else
        if (CanAcceptTask(task, true, npc)) {
            AcceptTask(task, npc);
            *link = (char*)task->npc1AfterAcceptTalk.c_str();
            return true;
        }
#endif
        //-----------chg end   by tony 06.05.09--------------------------------------------------------------//
    }

    if (task->finishType == TASK_FINISH_TYPE_NORMAL) {

    } else if (task->finishType == TASK_FINISH_TYPE_SELECT) {
        for (int i = 0; i < MAX_TASK_SELECT_NUM; i++) {
            if (task->specFlag[i] != str)
                continue;
                //-----------chg start by tony 06.05.09--------------------------------------------------------------//
#if 0
			@			if (CanFinishTask(task, i))
				@			{
				@				FinishTask(task, i);
				@* link = (char*)task->npc2AfterFinishTalk.c_str();
				@				return true;
				@			}
#else
            if (CanFinishTask(task, i, npc)) {
                FinishTask(task, i, npc);
                *link = (char*)task->npc2AfterFinishTalk.c_str();
                return true;
            }
#endif
            //-----------chg end   by tony 06.05.09--------------------------------------------------------------//
            else {
                *link = (char*)task->npc2SelectFailedTalk.c_str();
                return true;
            }
        }
    }
    return false;
}

void CRegionCreature::RemoveTaskItem(STaskDesc* desc) {
    ProcessTaskKeyList(desc->flagItem, TYPE_REMOVE, 0);
}

//---------chg start by tony 06.05.09---------------------------------------//
#if 0
@ long CRegionCreature::CheckTaskAcceptState(STaskDesc* task)
@{
	@	if (IsDoingTask(task))
		@		return TASK_STATE_DOING;
	@
		@	// check if can accept task
		@	if (CanAcceptTask(task, true, npc))
		@		return TASK_STATE_CAN_ACCEPT;
	@
		@	return TASK_STATE_NONE;
	@}
#else
long CRegionCreature::CheckTaskAcceptState(STaskDesc* task, CRegionCreature* npc) {
    if (IsDoingTask(task, npc))
        return TASK_STATE_DOING;

    // check if can accept task
    if (CanAcceptTask(task, true, npc))
        return TASK_STATE_CAN_ACCEPT;

    return TASK_STATE_NONE;
}
#endif
//---------chg end  by tony 06.05.09---------------------------------------//

//---------chg start by tony 06.05.09---------------------------------------//
#if 0
@ long CRegionCreature::CheckTaskFinishState(STaskDesc* task)
@{
	@	// check if doing task
		@	if (IsDoingTask(task))
		@	{
		@		// check if can auto finish task
			@		if (task->finishType == TASK_FINISH_TYPE_NORMAL)
			@		{
			@			if (CanFinishTask(task, 0))
				@			{
				@				FinishTask(task, 0);
				@				return TASK_STATE_CAN_FINISH;
				@			}
			@		}
		@		return TASK_STATE_DOING;
		@	}
	@
		@	return TASK_STATE_NONE;
	@}
#else
long CRegionCreature::CheckTaskFinishState(STaskDesc* task, CRegionCreature* npc, bool bFinish,
                                           long nIndex) {
    // check if doing task
    if (IsDoingTask(task, npc)) {
        // check if can auto finish task
        if (task->finishType == TASK_FINISH_TYPE_NORMAL) {
            if (CanFinishTask(task, 0, npc)) {
                if (bFinish) {
                    FinishTask(task, nIndex, npc);
                }
                return TASK_STATE_CAN_FINISH;
            }
        }
        return TASK_STATE_DOING;
    }

    return TASK_STATE_NONE;
}
#endif
//---------chg end  by tony 06.05.09---------------------------------------//

void CRegionCreature::NetSend_r2c_task_event(char result) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_task_event);
    g_sendCmd->WriteByte(result);
    return RecvCmd(g_sendCmd);
}

ETaskOverHead CRegionCreature::GetTaskOverHead(DWORD npcOID) {
    CRegionCreature* pNPC = m_scene->FindCreature(npcOID);
    if (!pNPC || !pNPC->m_npcInfo)
        return TOH_Unknown;
    CRegionNpcTask* pReNPC = (CRegionNpcTask*)(pNPC);

    ETaskOverHead   ret     = TOH_Unknown;
    int             nFinish = 0, nDoing = 0, nAccept = 0;
    int             nFinishMain = 0, nDoingMain = 0, nAcceptMain = 0;
    int             nT = 0;
    std::ostrstream os;

    CPlayerLogic::ForTaskType enType = pReNPC->GetPlayerLogic()->GetForTaskType();
    switch (enType) {
        case CPlayerLogic::eRand:
            nT = pReNPC->GetCNpcTaskLogicExt()->DefTalkRand(os, this, nFinish, nDoing, nAccept,
                                                            false);
            break;
        case CPlayerLogic::eList:
            nT = pReNPC->GetCNpcTaskLogicExt()->GetTaskList(os, this, true, nFinishMain, nDoingMain,
                                                            nAcceptMain, false);
            nT = pReNPC->GetCNpcTaskLogicExt()->GetTaskList(os, this, false, nFinish, nDoing,
                                                            nAccept, false);
            break;
        case CPlayerLogic::eCircle:
            break;
        default:
            break;
    }
    if (0 != nFinishMain || 0 != nFinish) {
        ret = TOH_AcceptAndFinish;
    } else if (0 != nAcceptMain || 0 != nAccept) {
        ret = TOH_CanAccept;
    } else if (0 != nDoingMain || 0 != nDoing) {
        ret = TOH_AcceptNoFinish;
    } else {
        ret = TOH_CanNotAccept;
    }
    //ETaskOverHead nTemp = TOH_CanNotAccept;
    //	bool bAcceptNoFinish  = false;
    //
    //	if(!pNPC->m_npcInfo->FinishTaskId.empty())
    //	{
    //		for(int i=0; i<(int)pNPC->m_npcInfo->FinishTaskId.size(); i++)
    //		{
    //			DWORD taskID = pNPC->m_npcInfo->FinishTaskId[i];
    //			int nPlayerLevel = m_core.Lev;
    //			STaskDesc *pTaskDesc = g_TableTask.FindTask(taskID);
    //			if(!pTaskDesc)
    //				continue;
    //
    //			//-------add start by tony 06.05.09------------------------//
    //			//-如果是跑环任务
    //			if(pTaskDesc->circleTask!=0)
    //			{
    //				STaskInfo* info = m_task.FindTask(pTaskDesc->id);
    //				if(!info) continue;
    //				switch(pTaskDesc->circleTask) {
    //				case TASK_CIRCLE_FINDNPC:
    //					if(info->m_findnpcID != pNPC->m_npcInfo->Id)
    //					{
    //						continue;
    //					}
    //					break;
    //				case TASK_CIRCLE_ITEM:
    //				case TASK_CIRCLE_BATTLE:
    //					if(info->m_srcNpcID != pNPC->m_npcInfo->Id)
    //					{
    //						continue;
    //					}
    //					break;
    //				default:
    //					break;
    //				}
    //			}
    //			//-------add end   by tony 06.05.09------------------------//
    //
    //			//修改任务完成状态--LxJian
    //			if(m_task.IsTaskAccept(taskID))
    //			{
    //				if (CanFinishTask(pTaskDesc,0, pNPC))
    //				{
    //					return TOH_AcceptAndFinish;
    //				}
    //				else
    //				{
    //					//return TOH_AcceptNoFinish;
    //					bAcceptNoFinish = true;
    //					//nTemp = TOH_AcceptNoFinish;
    //				}
    //			}
    //		}
    //	}
    //
    //	if(!pNPC->m_npcInfo->GetTaskId.empty())
    //	{
    //		for(int i=0; i<(int)pNPC->m_npcInfo->GetTaskId.size(); i++)
    //		{
    //			DWORD taskID = pNPC->m_npcInfo->GetTaskId[i];
    //			int nPlayerLevel = m_core.Lev;
    //			STaskDesc *pTaskDesc = g_TableTask.FindTask(taskID);
    //			if(!pTaskDesc)
    //				continue;
    //
    //			//tim.yang 日常任务  当天做完清除NPC头上提示
    //			if (!m_task.m_taskList.empty())
    //			{
    //				STaskInfo *task = m_task.FindTask(taskID);
    //				if(task && pTaskDesc->group == -1 && task->DayTimeTaskFinishTime!=0)
    //					continue;
    //			}
    //			//end
    //
    //			// check if can accept,do some extra check to save cpu
    //			if((nPlayerLevel >= pTaskDesc->reqMinLev) && (nPlayerLevel <= pTaskDesc->reqMaxLev))
    //			{
    //				//------------chg start by tony 06.05.09-----------------------------------------------//
    //#if 0
    //				@				if(CanAcceptTask(pTaskDesc,false))
    //#else
    //				if(CanAcceptTask(pTaskDesc,false, pNPC))
    //#endif
    //					//------------chg end   by tony 06.05.09-----------------------------------------------//
    //					return TOH_CanAccept;
    //				else
    //				{
    //					//ret = TOH_CanNotAccept;
    //					STaskInfo *info = m_task.FindTask(pTaskDesc->id);
    //					if(NULL != info)
    //					{
    //						if (info->FIsDoing())
    //						{
    //							//ret =TOH_AcceptNoFinish;
    //							bAcceptNoFinish = true;
    //						}
    //					}
    //				}
    //			}
    //		}
    //	}
    //
    //	if (bAcceptNoFinish)
    //	{
    //		ret = TOH_AcceptNoFinish;
    //	}
    //	else
    //	{
    //		ret = TOH_CanNotAccept;
    //	}
    return ret;
}

// ---------------------------------------------------------------------------
// fb task system
// ---------------------------------------------------------------------------
bool CRegionCreature::FbCanFinishTask(int taskId, int sreward) {
    SFbTask* task = g_TableTask.FindFbTask(taskId);
    if (!task)
        return false;

    SFbTaskInfo* info = m_task.GetFbTaskInfo(task->fbId);
    if (!info)
        return false;

    if (task->visDif > info->curDif)
        return false;

    if (info->IsTaskFinished(taskId))
        return false;

    vector<long> param;
    param.push_back(info->curDif);
    param.push_back(sreward);

    // process key
    if (!ProcessTaskKeyList(task->taskFinReq[info->curDif], TYPE_CHECK, &param)) {
        return false;
    }

    return true;
}

bool CRegionCreature::FbFinishTask(int taskId, int sreward) {
    SFbTask* task = g_TableTask.FindFbTask(taskId);
    if (!task)
        return false;

    SFbTaskInfo* info = m_task.GetFbTaskInfo(task->fbId);
    if (!info)
        return false;

    return FbFinishTask(task, info, sreward);
}

bool CRegionCreature::FbFinishTask(SFbTask* task, SFbTaskInfo* info, int sreward) {
    vector<long> param;
    param.push_back(info->curDif);
    param.push_back(sreward);

    // process key
    //if (info->curDif>=(int)task->taskFinReq.size())
    //{
    //    LOG1("FbFinishTask process key error 1\n");
    //    return false;
    //}
    if (!ProcessTaskKeyList(task->taskFinReq[info->curDif], TYPE_REMOVE, &param)) {
        // return false;
    }

    //if (info->curDif>=(int)task->taskReward.size())
    //{
    //    LOG1("FbFinishTask process key error 2\n");
    //    return false;
    //}
    if (!ProcessTaskKeyList(task->taskReward[info->curDif], TYPE_ADD, &param)) {}

    SFbTaskInfo* pTaskInfo = m_task.GetFbTaskInfo(task->fbId);
    if (!pTaskInfo)
        return false;
    pTaskInfo->FinishTask(task->id);
    //info->FinishTask(task->id);
    UpdateToClient(NET_CRE_UPDATE_FB_TASK);

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_finish_fb_task);
    g_sendCmd->WriteLong(task->id);
    RecvCmd(g_sendCmd);
    return true;
}

void CRegionCreature::AddEnemy(CRegionCreature* enemy, int damage) {
    STargetList::iterator it;
    if (!enemy->IsUser() && !enemy->IsUserPet() && !enemy->IsCallNpc())
        return;

    if (enemy->IsUserPet() || enemy->IsCallNpc())
        enemy = enemy->m_master;

    for (it = m_enemyList.begin(); it != m_enemyList.end(); it++) {
        if (it->Id == enemy->m_oId) {
            it->Damage += damage;
            return;
        }
    }

    STarget tar;
    tar.Id     = enemy->m_oId;
    tar.Damage = damage;
    m_enemyList.push_back(tar);
}

void CRegionCreature::SetTarget(CRegionCreature* target) {
    if (/*PZH*/ /*target->m_bDead == true*/ !target->GetPlayerLogic()->IsLive()) {
        // ERR("CRegionCreature::AddTarget Can't add self\n");
        return;
    }

    //LeiJun 是所属用户则不+为目标
    if (m_master && m_master == target && target->m_WardNPC == this)
        return;

    m_targets.clear();
    m_targets.push_front(STarget());
    m_targets.front().Id   = target->GetObjectId();
    m_targets.front().Hate = 0;
}

// Tian 添加仇恨的计算·····
void CRegionCreature::AddTarget(CRegionCreature* target, int hate, int damage) {
    if (target == this && /*PZH*/ /*target->m_bDead == true*/ !target->GetPlayerLogic()->IsLive()) {
        // ERR("CRegionCreature::AddTarget Can't add self\n");
        return;
    }

    //LeiJun 是所属用户则不+为目标
    if (m_master && m_master == target && target->m_WardNPC == this)
        return;

    if (m_targets.empty()) {
        m_targets.push_front(STarget());
        m_targets.front().Id     = target->GetObjectId();
        m_targets.front().Hate   = hate;
        m_targets.front().Damage = damage;
    } else if (m_targets.front().Id == target->GetObjectId()) {
        m_targets.front().Hate += hate;
        m_targets.front().Damage += damage;
    } else {
        stlforeach(STargetList, h, m_targets) {
            if (h->Id == target->GetObjectId())
                break;
        }
        if (h == m_targets.end()) {
            m_targets.push_back(STarget());
            m_targets.back().Id     = target->GetObjectId();
            m_targets.back().Hate   = hate;
            m_targets.back().Damage = damage;
        } else {
            h->Hate += hate;
            h->Damage += damage;

            if (h->Hate > m_targets.front().Hate) {
                m_targets.push_front(STarget());
                m_targets.front().Id     = h->Id;
                m_targets.front().Hate   = h->Hate;
                m_targets.front().Damage = h->Damage;
                m_targets.erase(h);
            }
        }
    }
}

CRegionCreature* CRegionCreature::GetCurrentTarget() {
    if (m_targets.empty())
        return NULL;

    CRegionCreature* target = m_scene->FindCreature(m_targets.front().Id);

    if (target && (m_traceRange != 0) && IsOutOfRange(target->m_pos, m_traceRange)) {
        target->LeaveCombat();
        target = NULL;
    }
    /**
		如果target为NULL，说明目标丢失，从列表中寻找一个仇恨值最高的目标，
		目标列表中的对象可能已经不存在，重复执行此函数直到列表寻找到目标
		或者清空列表。如果lockTarget为真，说明这是宠物AI，目标丢失后，
		执行跟随主人的命令。
	*/
    if (target == NULL) {
        m_targets.pop_front();

        STargetList::iterator hate = m_targets.begin();

        stlforeach(STargetList, iter, m_targets) {
            if (iter->Hate > hate->Hate)
                hate = iter;
        }

        if (hate != m_targets.begin()) {
            m_targets.push_front(*hate);
            m_targets.erase(hate);
        }

        target = GetCurrentTarget();
    }

    return target;
}

/*
STargetList::iterator CRegionCreature::FindTargetByCreature(CRegionCreature* cre)
{
	stlforeach(STargetList, i, m_targets)
		if (i->Id == cre->GetObjectId())
			return i;

	return m_targets.end();
}
*/

void CRegionCreature::RemoveTargetByCreature(CRegionCreature* cre) {
    if (!cre) {
        ERR("RemoveTargetByCreature NULL param\n");
        return;
    }

    STargetList::iterator it;
    for (it = m_targets.begin(); it != m_targets.end(); it++) {
        if ((*it).Id == cre->GetObjectId()) {
            if (cre->IsInCombat()) {
                cre->LeaveCombat();
            }
            m_targets.erase(it);
            return;
        }
    }
}

bool CRegionCreature::IsOutOfRange(const long* pos, int range) {
    long x = pos[0] - m_pos[0];
    long y = pos[1] - m_pos[1];

    return x * x + y * y > range * range;
}

void CRegionCreature::EquipCuff() {
    int realLev = GetRealLevel(m_core.Lev);
    switch (m_core.Metier) {
        case METIER_WARRIOR:  //明王
            m_cuffDamageMin = 0;
            m_cuffDamageMax = 0;
            m_cuffAttSpeed  = 0.8 * ATT_SPEED_SCALE;
            m_cuffAttRange  = 40;
            break;

        case METIER_HUNTER:  //花间
            m_cuffDamageMin = 0;
            m_cuffDamageMax = 0;
            m_cuffAttSpeed  = 0.8 * ATT_SPEED_SCALE;
            m_cuffAttRange  = 40;
            break;

        case METIER_WIZARD:  //术士
            m_cuffDamageMin = 0;
            m_cuffDamageMax = 0;
            m_cuffAttSpeed  = 0.8 * ATT_SPEED_SCALE;
            m_cuffAttRange  = 40;
            break;

        case METIER_TAOIST:  //道士
            m_cuffDamageMin = 0;
            m_cuffDamageMax = 0;
            m_cuffAttSpeed  = 0.8 * ATT_SPEED_SCALE;
            m_cuffAttRange  = 40;
            break;
    }
    m_core.DamageMin.Temp += m_cuffDamageMin;
    m_core.DamageMax.Temp += m_cuffDamageMax;
    m_core.AttSpeed.Temp += m_cuffAttSpeed;
    m_core.AttackRange.Temp += m_cuffAttRange;
}

void CRegionCreature::UnEquipCuff() {
    m_core.DamageMin.Temp -= m_cuffDamageMin;
    m_core.DamageMax.Temp -= m_cuffDamageMax;
    m_core.AttSpeed.Temp -= m_cuffAttSpeed;
    m_core.AttackRange.Temp -= m_cuffAttRange;
}

//Tianh  //是否在变身状态
bool CRegionCreature::IsChengBody() {
    if (IsUser()) {
        if (m_ChangeModelID) {
            return true;
        } else {
            return false;
        }
    } else if (IsUserPet()) {
        if (m_master) {
            if (m_master->m_ChangeModelID) {
                return true;
            } else {
                return false;
            }
        }
    }
}

//Tianh //变身
void CRegionCreature::ChangeBoby(int ModelID) {
    SCreModel* model = g_TableCreModel.FindCreModel(ModelID);
    if (!model)
        return;
    m_ChangeModelID = ModelID;  //保存变身后的模型ID+
    m_model         = model;
    if (m_dwLastModelID == ModelID) {
        m_ChangeModelID = 0;
    }
    //m_dwLastModelID = m_core.ModelId;
    m_core.ModelId      = model->Id;
    m_core.ComplexModel = model->bComplex;
    SendChangeBosyToC(m_core.ModelId);
}

void CRegionCreature::SendChangeBosyToC(int ChangeModelID) {
    m_core.RecomputeAttr(IsChengBody());

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_Chang_Body);
    g_sendCmd->WriteLong(GetObjectId());
    g_sendCmd->WriteLong((long)ChangeModelID);
    g_sendCmd->WriteByte(m_core.HeadModelId);
    long lShapeshift = m_ChangeModelID;
    g_sendCmd->SerializeBit(lShapeshift, 1);
    if (NULL != m_scene) {
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
    }
}

//Tianh 切换武器
bool CRegionCreature::ChangeWeapon() {
    if (m_pTrade || m_pPShop)
        return false;

    SItemID item1 = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
    SItemID item3 = m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_3, true);
    m_pItemContainer->UnEquip(CItemContainerBase::WEAPON_1);
    m_pItemContainer->UnEquip(CItemContainerBase::WEAPON_3);
    m_pItemContainer->Equip(item3, CItemContainerBase::WEAPON_1);
    m_pItemContainer->Equip(item1, CItemContainerBase::WEAPON_3);

    m_pItemContainer->UpdateItemToClient(CItemContainerBase::WEAPON_1);
    m_pItemContainer->UpdateItemToClient(CItemContainerBase::WEAPON_3);

    /*m_Skill.SpecialClearUseful(); */  //add by Tianh 换武器清除所有有益BUF 除了双倍等RMB道具

    return true;
}

void CRegionCreature::CMoveParent(float* npos) {

    float vpos[3];
    vpos[0] = npos[0];
    vpos[1] = npos[1];
    vpos[2] = npos[2];

    int x = (int)vpos[0];
    int y = (int)vpos[1];

    //iRx += rand() % 10 - 2;
    //iRy += rand() % 10 - 2;
    ForceSetPosition(x, y, 0, true);
    /*ForceSetPosition(x / 20,y / 20,0,true);*/
}

//
//add by Tianh
//
void CRegionCreature::ClearHidingAndTopo() {
    if (m_core.Metier == METIER_HUNTER && m_IsHiding != 0) {
        if (m_IsHiding == 1) {
            m_Skill.ClearEffByClass(14);  //隐身
            m_Skill.ClearEffByClass(20);

            /*Hiding(0);*/
        } else if (m_IsHiding == 2) {
            m_Skill.ClearEffByClass(11);  //遁地
                                          /*PlayerLurk(0);*/
        }
    }

    UpdateToClientDelta();
}

//
// magic weapon
//
void CRegionCreature::MagicWeaponBeginAddMp() {
    if (MagicWeaponEndAddMp())
        return;

    CCreatureState* state = RT_NEW CCreState_MagWeaAddMp;
    if (!AddState(state, NULL)) {
        DEL_ONE(state);
        return;
    }
}

bool CRegionCreature::MagicWeaponEndAddMp() {
    CCreatureState* state = FindStateByType(CRE_STATE_MAGIC_WEAPON_ADD_MP);
    if (!state)
        return false;

    state->End();
    return true;
}

void CRegionCreature::MagicWeaponBeginAddEle(int type) {
    // if(MagicWeaponEndAddEle()) return;
    CCreatureState* state = FindStateByType(CRE_STATE_MAGIC_WEAPON_ADD_ELE);
    if (state)
        return;

    state = RT_NEW CCreState_MagWeaAddEle;
    SCreStateParam param;
    param.p1 = type;
    if (!AddState(state, &param)) {
        DEL_ONE(state);
        return;
    }
}

bool CRegionCreature::MagicWeaponEndAddEle() {
    ItemID_SetInvalid(m_RifineTrump);
    m_bIsRining           = false;
    CCreatureState* state = FindStateByType(CRE_STATE_MAGIC_WEAPON_ADD_ELE);
    if (!state)
        return false;

    state->End();
    return true;
}

void CRegionCreature::TickState(unsigned long delta) {
    if (m_stateList.empty())
        return;

    bool                            ret;
    list<CCreatureState*>::iterator it;
    CCreatureState*                 state;
    for (it = m_stateList.begin(); it != m_stateList.end();) {
        state = *it;
        ret   = state->Tick(delta);
        if (!ret) {
            it = m_stateList.erase(it);
            DEL_ONE(state);
            continue;
        }
        it++;
    }
}

bool CRegionCreature::AddState(CCreatureState* state, SCreStateParam* param) {
    if (state->GetType() == CRE_STATE_PROCESS_BATTLE) {
        CCreatureState* pOldState = FindStateByType(CRE_STATE_PROCESS_BATTLE);
        if (pOldState) {
            DEL_ONE(state);
            pOldState->Begin(this, param);
            return true;
        } else
            BeginBattleState();
    }
    if (!state->Begin(this, param))
        return false;

    m_stateList.push_back(state);
    return true;
}

bool CRegionCreature::FindStateCannotMove() {
    list<CCreatureState*>::iterator it;
    for (it = m_stateList.begin(); it != m_stateList.end(); it++) {
        if ((*it)->GetCanMove()) {
            return false;
        }
    }
    return true;
}

CCreatureState* CRegionCreature::FindStateByType(ECreatureStateType type) {
    list<CCreatureState*>::iterator it;
    for (it = m_stateList.begin(); it != m_stateList.end(); it++) {
        if ((*it)->GetType() == type) {
            return *it;
        }
    }
    return NULL;
}

void CRegionCreature::AddBattleState(CRegionCreature* pEnemy) {
    if (!pEnemy)
        return;

    SCreStateParam params;
    params.p1              = pEnemy->GetObjectId();
    CCreatureState* pState = RT_NEW CCreState_Battle;
    if (!AddState(pState, &params)) {
        DEL_ONE(pState);
        return;
    }
    CRegionCreature* pPet = GetEntity();
    if (pPet && pPet != this) {
        pState = RT_NEW CCreState_Battle;
        if (!pPet->AddState(pState, &params)) {
            DEL_ONE(pState);
            return;
        }
    }

    params.p1 = GetObjectId();
    pState    = RT_NEW CCreState_Battle;
    if (!pEnemy->AddState(pState, &params)) {
        DEL_ONE(pState);
        return;
    }
    CRegionCreature* pEnemyPet = pEnemy->GetEntity();
    if (pEnemyPet && pEnemyPet != pEnemy) {
        pState = RT_NEW CCreState_Battle;
        if (!pEnemyPet->AddState(pState, &params)) {
            DEL_ONE(pState);
            return;
        }
    }
}

bool CRegionCreature::OnlyBattleState(void) {
    if (m_stateList.empty())
        return true;

    list<CCreatureState*>::iterator it;
    for (it = m_stateList.begin(); it != m_stateList.end(); it++) {
        if ((*it)->GetType() == CRE_STATE_PROCESS_BATTLE)
            return false;
    }

    return true;
}

bool CRegionCreature::HaveBattleState(void) {
    if (FindStateByType(CRE_STATE_PROCESS_BATTLE))
        return true;
    return false;
}

void CRegionCreature::BeginBattleState(void) {
    //通知客户端进入战斗状态
    //printf("Begin battle state %d\n", GetObjectId());
}

void CRegionCreature::EndBattleState(void) {
    //通知客户端退出战斗状态
    //printf("End battle state %d\n", GetObjectId());
}

void CRegionCreature::AttChangeMessageToClient(const char* sMessageType, int iAttValue) {
    if (!sMessageType || iAttValue == 0)
        return;
    if (iAttValue < 0)
        iAttValue *= -1;

    //通知客户端属性值增加
    const char* sTargetMsg = Rs(sMessageType);
    if (sTargetMsg != sMessageType) {
        rt2_sprintf(g_strStaticBuffer, sTargetMsg, iAttValue);
        SendBattleInfoMessage(g_strStaticBuffer);
    }
}

DWORD CRegionCreature::GetTargetParty(
    CRegionCreature* pTarget)  // 取得敌人的类型，可能是友军，敌军，宏在skill_base.h中
{
    /*
	if(pTarget == NULL)
	{
		return SKILL_TARGET_TYPE_NONE;
	}
	if (IsMonster())
	{
		// 自己是怪物
		if (pTarget->IsMonster())
		{
			return SKILL_TARGET_FRIEND_ACTOR; // 怪物对怪物都是友军
		}else if (pTarget->IsUserPet())
		{
			return SKILL_TARGET_ENEMY_PET;
		}else
		{
			return SKILL_TARGET_ENEMY_ACTOR;
		}
	}else if(IsUser() || IsUserPet())// 自己是人
	{
		// 自己是人
		CRegionCreature* pMaster, *pTMaster;
		if (pTarget->IsMonster())
		{
			// 对方是怪物，而自己是人
			return SKILL_TARGET_ENEMY_ACTOR; // 对怪物天生敌对
		}else if(pTarget->IsUser() || pTarget->IsUserPet())
		{
			// 如果对方也是人或者兽
			if (this==pTarget)
			{
				// 如果目标就是自己
				if (IsUserPet())
				{
					return SKILL_TARGET_PET;
				}else
				{
					return SKILL_TARGET_PLAYER;
				}
			}else
			{
				if (IsUserPet())
				{
					if (m_master==pTarget)
					{
						return SKILL_TARGET_PLAYER;
					}else
					{
						pMaster = m_master;
					}
				}else
				{
					if (m_pet==pTarget)
					{
						return SKILL_TARGET_PET;
					}else
					{
						pMaster = this;
					}
				}
			}

			if (pTarget->IsUserPet())
			{
				pTMaster = pTarget->m_master;
			}else
			{
				pTMaster = pTarget;
			}

			if (m_pTeam)
			{
				if (pMaster->m_pTeam->mData.Exist(pTMaster->m_userInfo->m_userId))
				{
					if (pTMaster==pTarget)
					{
						return SKILL_TARGET_FRIEND_ACTOR | SKILL_TARGET_TEAM_MEMBER;
					}else
					{
						return SKILL_TARGET_FRIEND_PET;
					}
				}
			}

			if (pTMaster==pTarget)
			{
				return SKILL_TARGET_NEUTRAL_ACTOR;
			}else
			{
				return SKILL_TARGET_NEUTRAL_PET;
			}
		}
//--------------------add start by tony 05.05.19-----------------------------//
		else if(pTarget->IsGuarder())
		{
			if (IsUserPet())
			{
				pMaster = m_master;
			}else
			{
				pMaster = this;
			}

			if(pMaster->m_core.Faction == pTarget->m_npcInfo->Faction)
			{
				if(g_RegionCastleWarMgr.IsCastleWarTime(pTarget->m_npcInfo->Home))
				{
					return SKILL_TARGET_ENEMY_ACTOR;
				}else
				{
					return SKILL_TARGET_FRIEND_ACTOR;
				}
			}else if(pTarget->m_npcInfo->Faction == FACTION_UNKNOWN)
			{
				return SKILL_TARGET_NEUTRAL_ACTOR;
			}else
			{
				return SKILL_TARGET_ENEMY_ACTOR;
			}
		}
		else
		{
			return SKILL_TARGET_NEUTRAL_ACTOR;
		}
	}
//--------------------add start by tony 05.05.19-----------------------------//
	else if(IsGuarder())
	{
		if (pTarget->IsGuarder())
		{
			return SKILL_TARGET_FRIEND_ACTOR; // 怪物对怪物都是友军
		}else if (pTarget->IsUserPet())
		{
			return SKILL_TARGET_ENEMY_PET;
		}else
		{
			return SKILL_TARGET_ENEMY_ACTOR;
		}
	}else //处理宋异人等的情况
	{
		if (pTarget->IsMonster())
		{
			return SKILL_TARGET_FRIEND_ACTOR; // 怪物对怪物都是友军
		}else if (pTarget->IsUserPet())
		{
			return SKILL_TARGET_ENEMY_PET;
		}else
		{
			return SKILL_TARGET_ENEMY_ACTOR;
		}
	}
//--------------------add start by tony 05.05.19-----------------------------//
	*/
    bool bMe, bTarget;
    if (pTarget == NULL) {
        return SKILL_TARGET_TYPE_NONE;
    }

    if (IsUserPet() && pTarget == m_master) {
        return SKILL_TARGET_MASTER;
    }

    if (pTarget->GetPlayerLogic()->GetLiveType() == CPlayerLogic::eHaveItem) {
        return SKILL_TARGET_CORPSE;
    }

    if (pTarget->IsUserPet() && pTarget == this) {
        return SKILL_TARGET_PETSELF;
    }

    //if(IsUserPet() && (pTarget == m_master || pTarget == this))
    //{
    //	return SKILL_TARGET_MASTERANDPETSELF
    //}

    // 如果双方有一方是怪物
    bMe     = IsMonster();
    bTarget = pTarget->IsMonster();
    if (bMe || bTarget) {
        if (bMe && bTarget) {
            return SKILL_TARGET_FRIEND_ACTOR;  // 怪物对怪物都是友军
        }
        if (pTarget->IsUserPet()) {
            return SKILL_TARGET_ENEMY_PET;
        } else {
            return SKILL_TARGET_ENEMY_ACTOR;
        }
    }

    // 如果双方有一个是 战斗NPC
    bMe     = IsGuarder();
    bTarget = pTarget->IsGuarder();
    if (bMe || bTarget) {
        CRegionCreature *pGuarder, *pGuarder2;
        if (bMe) {
            pGuarder  = this;
            pGuarder2 = pTarget;
        } else {
            pGuarder  = pTarget;
            pGuarder2 = this;
        }

        if (pGuarder2->IsGuarder())  // 如果双方都是战斗NPC，那不同阵营间就是敌人，否则就是朋友
        {
            if (pGuarder->m_npcInfo->Faction == FACTION_UNKNOWN ||
                pGuarder2->m_npcInfo->Faction ==
                    FACTION_UNKNOWN) {  // 如果其中一个阵营不明确，那就是中立关系
                return SKILL_TARGET_NEUTRAL_ACTOR;
            } else if (pGuarder->m_npcInfo->Faction ==
                       pGuarder2->m_npcInfo->Faction) {  // 如果阵营相同就是朋友，否则就是敌人
                return SKILL_TARGET_FACTION | SKILL_TARGET_FRIEND_ACTOR;
            } else {
                return SKILL_TARGET_FACTION_x | SKILL_TARGET_ENEMY_ACTOR;
            }
        } else if (pGuarder2->IsUser())  // 其中一个是玩家
        {
            if (pGuarder->m_npcInfo->Faction == FACTION_UNKNOWN) {
                return SKILL_TARGET_NEUTRAL_ACTOR;
            } else if (pGuarder->m_npcInfo->Faction == pGuarder2->m_core.Faction) {
                return SKILL_TARGET_FACTION | SKILL_TARGET_FRIEND_ACTOR;
            } else {
                return SKILL_TARGET_FACTION_x | SKILL_TARGET_ENEMY_ACTOR;
            }
        } else if (pGuarder2->IsUserPet() || pGuarder2->IsCallNpc())  // 其中一个是玩家的兽
        {
            if (pGuarder->m_npcInfo->Faction == FACTION_UNKNOWN) {
                return SKILL_TARGET_NEUTRAL_ACTOR;
            } else if (pGuarder->m_npcInfo->Faction == pGuarder2->m_master->m_core.Faction) {
                return SKILL_TARGET_FACTION | SKILL_TARGET_FRIEND_ACTOR;
            } else {
                return SKILL_TARGET_FACTION_x | SKILL_TARGET_ENEMY_ACTOR;
            }
        } else {
            return SKILL_TARGET_NEUTRAL_ACTOR;
        }
    }

    bMe     = (IsUser() || IsUserPet() || IsCallNpc());
    bTarget = (pTarget->IsUser() || pTarget->IsUserPet() || pTarget->IsCallNpc());
    if (bMe && bTarget) {
        CRegionCreature *pMaster, *pTMaster;
        DWORD            dwParty = 0;
        if (IsUserPet() || IsCallNpc()) {
            pMaster = m_master;
        } else {
            pMaster = this;
        }
        if (pTarget->IsUserPet() || pTarget->IsCallNpc()) {
            pTMaster = pTarget->m_master;
        } else {
            pTMaster = pTarget;
        }

        if (pMaster == pTMaster)  // 如果主人就是自己的主人
        {
            if (pTMaster != pTarget)  // 如果对方是兽
            {
                return SKILL_TARGET_PET | SKILL_TARGET_TEAM_MEMBER | SKILL_TARGET_FACTION |
                       SKILL_TARGET_UNION;
            } else  // 如果对方是人
            {
                return SKILL_TARGET_PLAYER | SKILL_TARGET_TEAM_MEMBER | SKILL_TARGET_FACTION |
                       SKILL_TARGET_UNION;
            }
        }

        if (pMaster->m_unionID != 0) {
            if (pMaster->m_unionID == pTMaster->m_unionID) {
                dwParty |= SKILL_TARGET_UNION;
            } else {
                dwParty |= SKILL_TARGET_UNION_x;
            }
        }

        if (pMaster->m_core.Faction == pTMaster->m_core.Faction) {
            dwParty |= SKILL_TARGET_FACTION;
        } else {
            dwParty |= SKILL_TARGET_FACTION_x;
        }

        if (pMaster->m_pTeam->mData.Exist(pTMaster->m_userInfo->m_userId)) {
            dwParty |= SKILL_TARGET_TEAM_MEMBER;
            if (pTMaster == pTarget) {
                dwParty |= SKILL_TARGET_FRIEND_ACTOR;
            } else {
                dwParty |= SKILL_TARGET_FRIEND_PET;
            }
        } else {
            if (pTMaster == pTarget) {
                dwParty |= SKILL_TARGET_ENEMY_ACTOR;
                //dwParty |= SKILL_TARGET_NEUTRAL_ACTOR;
            } else {
                dwParty |= SKILL_TARGET_ENEMY_PET;
                //dwParty |= SKILL_TARGET_NEUTRAL_PET;
            }
        }
        return dwParty;
    }
    return SKILL_TARGET_NEUTRAL_ACTOR;
}

void CRegionCreature::ToCreatureInfo(SCreatureInfo* pInfo) {
    if (!pInfo)
        return;

    pInfo->name      = m_core.Name;
    pInfo->DbID      = (m_userInfo ? m_userInfo->m_userId : 0);
    pInfo->metier    = m_core.Metier;
    pInfo->metierLev = m_core.MetierLev;
    pInfo->faction   = m_core.Faction;
    pInfo->level     = m_core.Lev;
    pInfo->exp       = m_core.Exp;
    pInfo->hp        = m_core.GetHp();
    pInfo->mp        = m_core.GetMp();
    pInfo->EnP       = m_core.GetEnP();
    pInfo->exploit   = m_core.Exploit;
    pInfo->pkValue   = m_core.Pk;
    pInfo->credit    = m_core.Credit;
    /**************Tianh**************/
    pInfo->cul         = m_core.Cul;
    pInfo->culExp      = m_core.CulExp;
    pInfo->soul        = m_core.Soul;
    pInfo->contribute  = m_core.Contribute;
    pInfo->point       = m_core.Point;
    pInfo->record      = m_core.Record;
    pInfo->unionRecord = m_core.UnionRecord;
    pInfo->jobRecord   = m_core.JobRecord;
    pInfo->bornTag     = m_core.BornTag;
    pInfo->sex         = m_core.Sex;
    pInfo->gradeLev    = m_core.GradeLev;
    /*pInfo->areaHonors = m_core.AreaHonors;
	pInfo->relivePostion = m_core.RelivePostion; */
    pInfo->vigour      = m_core.Vigour;
    pInfo->vigourPoint = m_core.VigourPoint;
    /*********************************/
    pInfo->money     = m_pItemContainer ? m_pItemContainer->GetMoney() : 0;
    pInfo->bankMoney = m_pBank->GetMoney();
    pInfo->DamMin    = m_core.GetDamageMin();
    pInfo->DamMax    = m_core.GetDamageMax();
    for (int i = 0; i < MAX_ELEMENT_ATTR; i++) {
        pInfo->EleDamage[i] = m_core.ElementDamage[i].GetValue();
        pInfo->EleArmor[i]  = m_core.ElementArmor[i].GetValue();
    }
    pInfo->unionName = m_unionName;
    pInfo->petLevel  = (m_pet ? m_pet->m_core.Lev : 0);
    pInfo->petHP     = (m_pet ? m_pet->m_core.GetHp() : 0);

    RtsSceneBlockRtb* pBlockMap = (m_iCurrentBlockX < 0)
                                      ? (NULL)
                                      : (m_scene->m_pTerrain->GetBlockMap(
                                            m_iCurrentBlockX - m_scene->m_pTerrain->GetCenterX(),
                                            m_iCurrentBlockY - m_scene->m_pTerrain->GetCenterY()));
    if (pBlockMap)
        pInfo->scene = pBlockMap->GetBlockName();
    else
        pInfo->scene = "";

    return;
}

bool CRegionCreature::IsUnionCaption() {
    if (!m_userInfo)
        return false;
    if (m_unionID == 0)
        return false;

    EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union =
        g_region->m_unionMap.find(m_unionID);
    if (iter_union == g_region->m_unionMap.end()) {
        return false;
    } else {
        if (iter_union->second.m_nCaptainID != m_userInfo->m_userId)
            return false;
        else
            return true;
    }
}

void CRegionCreature::NetSend_State() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_user_state);
    g_sendCmd->WriteLong(m_dwState);
    RecvCmd(g_sendCmd);
}

void CRegionCreature::State_SetVisible(bool bVisible) {
    if (bVisible)
        m_dwState |= CreState_Visible;
    else
        m_dwState &= ~CreState_Visible;
    NetSend_State();
    m_modelRef++;
}

void CRegionCreature::State_SetMovable(bool bCanMove) {
    if (bCanMove)
        m_dwState |= CreState_CanMove;
    else
        m_dwState &= ~CreState_CanMove;
    NetSend_State();
    m_modelRef++;
}

void CRegionCreature::State_SetSpeakable(bool bSpeakable) {
    if (bSpeakable)
        m_dwState |= CreState_CanSpeak;
    else
        m_dwState &= ~CreState_CanSpeak;
    NetSend_State();
    m_modelRef++;
}

void CRegionCreature::State_SetPShopable(bool bPShopable) {
    if (bPShopable)
        m_dwState |= CreState_CanPShop;
    else
        m_dwState &= ~CreState_CanPShop;
    NetSend_State();
    m_modelRef++;
}

bool CRegionCreature::IsDoingMoneyTask() {
    return m_task.IsDoingMoneyTask() != 0;
}

void CRegionCreature::SetIncomeState(long lplayerNowLoginTime)  //ac.ma
{
    if (m_core.bAdult || lplayerNowLoginTime == 0)
        return;
    unsigned long lTimeOnline = time(NULL) - m_playerNowLoginTime + m_lLastTotalOnline;
    if (m_ltempOnline < lTimeOnline)
        m_bforGetTime = true;
    m_ltempOnline = lTimeOnline;

    if (lTimeOnline / (EVERY_PER_TIME * 4) >= 5) {
        if (lTimeOnline % EVERY_PER_TIME == 0 && m_bforGetTime) {
            SendSystemMessage(R(MSG_INCOME_STATE_FIVE_HOUR));
            m_bforGetTime = false;
        }
        m_iIncomeState = 0;
    } else
        switch (lTimeOnline / (EVERY_PER_TIME * 4)) {
            case 1:
                if (lTimeOnline % (EVERY_PER_TIME * 4) == 0 && m_bforGetTime) {
                    SendSystemMessage(R(MSG_INCOME_STATE_ONE_HOUR));
                    m_bforGetTime = false;
                }
                m_iIncomeState = 100;
                break;
            case 2:
                if (lTimeOnline % (EVERY_PER_TIME * 4) == 0 && m_bforGetTime) {
                    SendSystemMessage(R(MSG_INCOME_STATE_TWO_HOUR));
                    m_bforGetTime = false;
                }
                m_iIncomeState = 100;
                break;
            case 3:
                if (lTimeOnline % (EVERY_PER_TIME * 2) == 0 && m_bforGetTime) {
                    SendSystemMessage(R(MSG_INCOME_STATE_THREE_HOUR));
                    SendSystemMessage(R(MSG_INCOME_STATE_FOUR_HOUR));
                    m_bforGetTime = false;
                }
                m_iIncomeState = 50;
                break;
            case 4:
                if (lTimeOnline % (EVERY_PER_TIME * 2) == 0 && m_bforGetTime) {
                    SendSystemMessage(R(MSG_INCOME_STATE_FOUR_HOUR));
                    m_bforGetTime = false;
                }
                m_iIncomeState = 50;
                break;
        }
}

bool CRegionCreature::SaveExtraData(RtMemoryBlock& block) {
    //static char saveBuffer[MAX_CMD_PACKET_SIZE*2];
    S_MEMDEF(saveBuffer, MAX_CMD_PACKET_SIZE * 2)
    S_MEMPROTECTOR(saveBuffer, MAX_CMD_PACKET_SIZE * 2, bMP)

    int size = MAX_CMD_PACKET_SIZE * 2;
    g_sendCmd->BeginWrite();

    // 催钱任务相关数据
    {
        unsigned long dwTime = this->GetPlayerLogic()->GetNextGongTaskTime();
        g_sendCmd->WriteShort(5);  // 版本
        *g_sendCmd << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        *g_sendCmd << m_mtClearDay << m_wenClearDay << m_wenTaskFinish << m_wenTaskAccept
                   << m_gongClearDay << m_gongTaskFinish << m_gongTaskAccept << dwTime;
        *g_sendCmd << m_UnionTask << m_UnionClearDay << m_MouTaskFinish << m_MouTaskAccept
                   << m_MouClearDay;
    }

    // 打卡相关数据
    {
        g_sendCmd->WriteShort(2);  // 版本
        *g_sendCmd << m_cardTime << m_usingCard << m_cardLastTime << m_LastDieTime;
    }

    // 配方相关数据
    {
        g_sendCmd->WriteShort(1);  // 版本
        g_sendCmd->WriteShort((short)m_listMatchID.size());
        std::list<short>::iterator it;
        for (it = m_listMatchID.begin(); it != m_listMatchID.end(); it++) {
            g_sendCmd->WriteShort((short)*it);
        }
    }

    // 战功
    g_sendCmd->WriteLong(m_core.Exploit);

    // 神灯相关数据  tim.yang
    {
        g_sendCmd->WriteShort(3);  // 版本
        if (0 == m_monsterCount) {
            m_IsAddMonster = 0;
        }
        *g_sendCmd << m_IsAddMonster << m_monsterCount;
    }

    {  //ac.ma
        g_sendCmd->WriteShort(6);
        m_playerLastLogoutTime = time(NULL);  //记录本次下线时间
        m_lTotalOnline         = m_playerLastLogoutTime - m_playerNowLoginTime + m_lLastTotalOnline;
        if (m_core.bAdult) {
            m_playerLastLogoutTime = 0;
            m_lTotalOnline         = 0;
            m_lTotalOffline        = 0;
        }
        *g_sendCmd << m_playerLastLogoutTime << m_lTotalOnline << m_lTotalOffline;

        *g_sendCmd << m_useCardMinuteBottleNum << m_lastuseCardMinuteBottleTime;
    }

    // 准备发送数据
    if (!g_sendCmd->SaveToBuffer(saveBuffer, size)) {
        ERR1("[%s] save data to buffer error\n", m_core.Name.c_str());
    }
    block.SetData(saveBuffer, size);
    return true;
}

#define EndVer                                                                                     \
    else {                                                                                         \
        ERR1("[%s] load data version error\n", m_core.Name.c_str());                               \
        return false;                                                                              \
    }

bool CRegionCreature::LoadExtraData(RtMemoryBlock& block) {
    m_playerNowLoginTime = time(NULL);  //记录本次登陆时间
    if (block.size == 0)
        return true;
    CG_CmdPacket tmp;
    if (!tmp.LoadFromBuffer((char*)block.data, block.size)) {
        ERR1("[%s] load data from buffer error\n", m_core.Name.c_str());
        return false;
    }
    short ver;

    // 催钱任务相关数据
    tmp << ver;
    if (ver == 1) {
        tmp << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        tmp << m_mtClearDay;
    } else if (ver == 2) {
        tmp << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        tmp << m_mtClearDay << m_wenClearDay << m_wenTaskFinish << m_wenTaskAccept;
    } else if (ver == 3) {
        tmp << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        tmp << m_mtClearDay << m_wenClearDay << m_wenTaskFinish << m_wenTaskAccept << m_gongClearDay
            << m_gongTaskFinish << m_gongTaskAccept;
    } else if (ver == 4) {
        unsigned long dwTime;
        tmp << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        tmp << m_mtClearDay << m_wenClearDay << m_wenTaskFinish << m_wenTaskAccept << m_gongClearDay
            << m_gongTaskFinish << m_gongTaskAccept << dwTime;
        this->GetPlayerLogic()->SetNextGongTaskTime(dwTime);
    } else if (ver == 5) {
        unsigned long dwTime;
        tmp << m_mtBusy << m_mtFinish[0] << m_mtFinish[1] << m_mtFinish[2];
        tmp << m_mtClearDay << m_wenClearDay << m_wenTaskFinish << m_wenTaskAccept << m_gongClearDay
            << m_gongTaskFinish << m_gongTaskAccept << dwTime;
        tmp << m_UnionTask << m_UnionClearDay << m_MouTaskFinish << m_MouTaskAccept
            << m_MouClearDay;
        this->GetPlayerLogic()->SetNextGongTaskTime(dwTime);
    }
    EndVer;
    if (m_mtClearDay != g_region->m_yDay) {
        m_mtFinish[0] = 0;
        m_mtFinish[1] = 0;
        m_mtFinish[2] = 0;
        m_mtClearDay  = g_region->m_yDay;
    }
    if (m_wenClearDay != g_region->m_yDay) {
        m_wenClearDay   = g_region->m_yDay;
        m_wenTaskFinish = 0;
        m_wenTaskAccept = 0;
    }
    if (m_gongClearDay != g_region->m_yDay) {
        m_gongClearDay   = g_region->m_yDay;
        m_gongTaskFinish = 0;
        m_gongTaskAccept = 0;
    }

    if (m_UnionClearDay != g_region->m_yDay) {
        m_UnionClearDay = g_region->m_yDay;
        m_UnionTask     = 0;
    }

    if (m_MouClearDay != g_region->m_yDay) {
        m_MouClearDay   = g_region->m_yDay;
        m_MouTaskAccept = 0;
        m_MouTaskFinish = 0;
    }

    // 打卡相关数据
    tmp << ver;
    if (ver == 1) {
        tmp << m_cardTime << m_usingCard << m_cardLastTime;
    } else if (ver == 2) {
        tmp << m_cardTime << m_usingCard << m_cardLastTime << m_LastDieTime;
    }
    EndVer;

    // 配方相关数据
    tmp << ver;
    if (ver == 1) {
        short i, sID, sCnt;
        tmp << sCnt;
        m_listMatchID.clear();
        for (i = 0; i < sCnt; i++) {
            tmp << sID;
            m_listMatchID.push_back(sID);
        }
    }
    EndVer;

    // 战功
    tmp << m_core.Exploit;

    tmp << ver;
    if (ver == 3)  //tim.yang 神灯
    {
        tmp << m_IsAddMonster << m_monsterCount;
        if (IsUser() && m_IsAddMonster && !m_FBID)
            GodLightCreateMonster_AddMonster(this, m_monsterCount);
        if (IsUser() && m_IsAddMonster && m_FBID)
            m_bIsFBReborn = true;  //如果是在副本中重生
    }

    tmp << ver;  //ac.ma
    if (ver == 4) {
        tmp << m_playerLastLogoutTime;      //记录上次下线时间
        m_playerNowLoginTime = time(NULL);  //记录本次登陆时间
        m_lTotalOnline       = 0;
        m_lTotalOffline      = 0;
    } else if (ver == 5) {
        tmp << m_playerLastLogoutTime << m_lTotalOnline << m_lTotalOffline;  //记录上次下线时间
        m_playerNowLoginTime   = time(NULL);  //记录本次登陆时间
        long ltempTotalOffline = m_playerNowLoginTime - m_playerLastLogoutTime + m_lTotalOffline;
        if (ltempTotalOffline >= TOTAL_OFFLINE_TIME || ltempTotalOffline < 0) {
            m_lTotalOffline = 0;
            m_lTotalOnline  = 0;
        } else
            m_lTotalOffline = ltempTotalOffline;
        m_lLastTotalOnline = m_lTotalOnline;

        if (m_core.bAdult) {
            m_playerLastLogoutTime = 0;
            m_lTotalOnline         = 0;
            m_lTotalOffline        = 0;
            m_playerNowLoginTime   = 0;
            ltempTotalOffline      = 0;
        }
    } else if (6 == ver) {
        tmp << m_playerLastLogoutTime << m_lTotalOnline << m_lTotalOffline;  //记录上次下线时间
        m_playerNowLoginTime   = time(NULL);  //记录本次登陆时间
        long ltempTotalOffline = m_playerNowLoginTime - m_playerLastLogoutTime + m_lTotalOffline;
        if (ltempTotalOffline >= TOTAL_OFFLINE_TIME || ltempTotalOffline < 0) {
            m_lTotalOffline = 0;
            m_lTotalOnline  = 0;
        } else
            m_lTotalOffline = ltempTotalOffline;
        m_lLastTotalOnline = m_lTotalOnline;

        if (m_core.bAdult) {
            m_playerLastLogoutTime = 0;
            m_lTotalOnline         = 0;
            m_lTotalOffline        = 0;
            m_playerNowLoginTime   = 0;
            ltempTotalOffline      = 0;
        }
        tmp << m_useCardMinuteBottleNum << m_lastuseCardMinuteBottleTime;
    }
    return true;
}

bool CRegionCreature::Save(SUserData& data) {
    if (!m_userInfo->m_bSaveData)
        return false;

    RtsSceneBlockRtb* bmap = g_region->m_defScene->FindBlockByPos(m_pos);
    if (bmap) {
        data.scene = bmap->m_szFileName;
    } else {
        data.scene = "scene01_001.rtb";
        LOG2("Cannot find block by Pos (%d,%d)[SaveUserData]\n", m_pos[0], m_pos[1]);
    }

    memset(&data.attr, 0, sizeof(data.attr));
    data.attr.actorID      = m_actorInfo->Id;
    data.attr.hp           = m_core.GetHp();
    data.attr.mp           = m_core.GetMp();
    data.attr.EnP          = m_core.GetEnP();
    data.attr.metier       = m_core.Metier;
    data.attr.metierLevel  = m_core.MetierLev;
    data.attr.faction      = m_core.Faction;
    data.attr.exp          = m_core.Exp;
    data.attr.ap           = m_core.AttrPoint;
    data.attr.sp           = m_core.SkillPoint;
    data.attr.level        = m_core.Lev;
    data.attr.x            = m_pos[0];
    data.attr.y            = m_pos[1];
    data.attr.strength     = m_core.Str.Base;
    data.attr.dexterity    = m_core.Dex.Base;
    data.attr.constitution = m_core.Con.Base;
    data.attr.intellect    = m_core.Int.Base;
    data.attr.hit          = m_core.Hit.Base;
    data.attr.credit       = m_core.Credit;
    data.attr.pk           = m_core.Pk;
    data.attr.spClearCnt   = m_core.SkillClearCnt;

    /****Tianh**09.12.13**/
    data.attr.Soul             = m_core.Soul;
    data.attr.Vigour           = m_core.Vigour;
    data.attr.VigourPoint      = m_core.VigourPoint;
    data.attr.Sex              = m_core.Sex;
    data.attr.Contribute       = m_core.Contribute;
    data.attr.Point            = m_core.Point;
    data.attr.Record           = m_core.Record;
    data.attr.UnionRecord      = m_core.UnionRecord;
    data.attr.JobRecord        = m_core.JobRecord;
    data.attr.BornTag          = m_core.BornTag;
    data.attr.Cul              = m_core.Cul;
    data.attr.CulExp           = m_core.CulExp;
    data.attr.RelivePostion    = m_core.RelivePostion;
    data.attr.PlayerOnlineTime = m_core.PlayerOnlineTime;
    data.attr.GradeLev         = m_core.GradeLev;
    data.attr.exploit          = m_core.Exploit;
    data.attr.HeadImageID      = m_core.HeadImageID;

    data.userID = m_userInfo->m_userId;
    m_pFriend->Save(data.friends);
    m_task.SaveToString(data.task);
    m_task.SaveFbTask(data.fbTask);
    data.bank       = m_pBank->SaveToString(g_region->m_pItemManager);
    data.bankPet    = m_pBankPet->SaveToString(g_region->m_pItemManager);
    data.strEffects = m_Skill.SaveEffectToString();  // skills effect
    data.strSkills  = m_Skill.SaveToString();        // skills

    data.items             = m_pItemContainer->SaveToString();        // items;
    data.pet               = m_pItemContainer->SavePetBagToString();  // pets;
    data.money             = m_pItemContainer->GetMoney();
    data.shortcut          = m_shortcut;
    data.configData        = m_configData;
    data.bankMoney         = m_pBank->GetMoney();
    data.dwState           = m_dwState;
    data.dungeonTID        = m_dungeonTID;       // 副本唯一ID
    data.dungeonUID        = m_dungeonUID;       // 副本类型ID
    data.dungeonLastQuit   = m_dungeonLastQuit;  // 副本类型ID
    data.LineSYSGroupID    = m_dwGroupID;        //分线系统组ID
    data.LineSYSLineID     = m_dwLineID;         //分线系统线ID
    data.popularity        = m_popularity;
    data.IsInWarfield      = m_bIsInWarfield;
    data.lWarfieldLastQuit = m_lWarfieldLastQuit;
    data.petItemID         = m_dwPetSave;

    data.isFirstLogin = m_IsFirstLogin;  //是否第一次登陆

    if (&m_Skill)  //是否有变狼技能 --如果没有此技能   则不保存变身ID  第2次上线回人形
    {
        SSkill* pSSkill = m_Skill.FindSkillBySubID(SUIDSKILLLANG);
        if (!pSSkill) {
            m_ChangeModelID = 0;
        }
    }

    data.ChangeModelID = m_ChangeModelID;
    data.isDead        = m_IsDead;  //是否死亡
    //data.UpdateSCDTime      = (long)time(NULL);

    //脚本功能状态值
    for (int ii = 0; ii < SCRIPT_FUNC_STATE_COUNT; ii++)
        data.lFunctionState[ii] = m_userInfo->m_lFunctionState[ii];

    SaveExtraData(data.extraData);
    return true;
}

bool CRegionCreature::Load(SUserData& data) {
    // set actor Info
    // 根据职业和等级取出人物ID
    SUserActor* actor = g_TableUserActor.FindUserActor(
        data.attr.actorID /*data.attr.metier,data.attr.metierLevel*/);
    if (!actor) {
        ERR2("Can't find actor by %d %d\n", data.attr.metier, data.attr.metierLevel);
        return false;
    }

    // region 不需要判断，在gameworld上判断
    /*
	if(!actor->bZCreate && !actor->bSCreate)
		return false;
	*/
    m_actorInfo = actor;
    SetModel(actor->ModelId);

    m_dwLastModelID = actor->ModelId;

    m_userInfo->m_username = data.name;
    m_userInfo->privileges = data.privileges;
    m_userInfo->m_bankPass = data.bankPass;
    m_core.Name            = data.name;

    //脚本功能状态值
    for (int ii = 0; ii < SCRIPT_FUNC_STATE_COUNT; ii++)
        m_userInfo->m_lFunctionState[ii] = data.lFunctionState[ii];

    // 得到队伍，好友，邮件
    if (m_pTeam) {
        DEL_ONE(m_pTeam);
        m_pTeam = NULL;
    }

    if (m_pMail) {
        DEL_ONE(m_pMail);
        m_pMail = NULL;
    }
    if (m_pFriend) {
        DEL_ONE(m_pFriend);
        m_pFriend = NULL;
    }

    m_pTeam   = RT_NEW   CServerTeam(this, m_oId, m_userInfo->m_userId, m_userInfo->m_username,
                                     data.attr.HeadImageID);
    m_pFriend = RT_NEW CServerFriend(this, m_oId, m_userInfo->m_userId, m_userInfo->m_username);
    m_pMail   = RT_NEW   CServerMail(this);
    m_pMail->SetFriendList(m_pFriend);

    // fill user data
    m_unionID               = data.unionID;
    m_unionName             = data.unionName;
    m_unionNickName         = data.unionNickName;
    m_unionIcon             = data.unionIcon;
    m_pos[0]                = data.attr.x;
    m_pos[1]                = data.attr.y;
    m_pos[2]                = 0;
    m_core.Metier           = data.attr.metier;
    m_core.MetierLev        = data.attr.metierLevel;
    m_core.Faction          = data.attr.faction;
    m_core.Lev              = data.attr.level;
    m_core.Str.Base         = data.attr.strength;
    m_core.Dex.Base         = data.attr.dexterity;
    m_core.Hit.Base         = data.attr.hit;
    m_core.Con.Base         = data.attr.constitution;
    m_core.Int.Base         = data.attr.intellect;
    m_core.SkillPoint       = data.attr.sp;
    m_core.AttrPoint        = data.attr.ap;
    m_core.Exp              = data.attr.exp;
    m_core.SkillClearCnt    = data.attr.spClearCnt;
    m_core.Soul             = data.attr.Soul;
    m_core.Vigour           = data.attr.Vigour;
    m_core.VigourPoint      = data.attr.VigourPoint;
    m_core.Sex              = data.attr.Sex;
    m_core.Contribute       = data.attr.Contribute;
    m_core.Point            = data.attr.Point;
    m_core.Record           = data.attr.Record;
    m_core.UnionRecord      = data.attr.UnionRecord;
    m_core.JobRecord        = data.attr.JobRecord;
    m_core.BornTag          = data.attr.BornTag;
    m_core.RelivePostion    = data.attr.RelivePostion;
    m_core.Cul              = data.attr.Cul;
    m_core.Exploit          = data.attr.exploit;  // 战功
    m_core.CulExp           = data.attr.CulExp;
    m_core.PlayerOnlineTime = data.attr.PlayerOnlineTime;
    m_core.GradeLev         = data.attr.GradeLev;
    m_core.HeadImageID      = data.attr.HeadImageID;
    m_core.actorID          = data.attr.actorID;
    m_core.EnP              = data.attr.EnP;
    m_core.RecomputeAttr();
    EquipCuff();
    ReComputePetBagSize();

    // move speed default set;
    m_core.MovSpeed.Base = BASE_MOVE_SPEED;
    ResetCheaterCheck();

    //<add by fox>
    static int          PLAYER_CHECK_INTERVAL = 10;  //检查的时间间隔[单位：秒]
    static CIniVariable SpeedCheckInterval(GetGameRule(), "GameCheck", "PlayerCheckInterval",
                                           PLAYER_CHECK_INTERVAL);
    //</add by fox>
    SetTimer(CRE_TIMER_CHEATER_CHECK, PLAYER_CHECK_INTERVAL * 1000);

    m_hpAutoRecoverRate = 0.1;
    m_mpAutoRecoverRate = 0.1;
    m_shortcut          = data.shortcut;
    m_configData        = data.configData;

    m_core.Pk     = data.attr.pk;
    m_core.Credit = data.attr.credit;

    m_pFriend->Load(data.friends);
    /*
	if(!(data.cSwitchRegion))
	{
		//是新创建的
		m_pFriend->OnOnline();
	}
	*/

    m_dwState = data.dwState;

    //解析势力声望
    m_popularity = data.popularity;
    LoadToPopularity(m_popularity.c_str());

    m_Skill.LoadFromString(data.strSkills.c_str(), data.lastUpdateTime);
    //为每个人添加怒气爆发技能
    // 	m_Skill.StudySkill(1601, true);
    // 	m_Skill.StudySkill(1602, true);
    // 	m_Skill.StudySkill(1603, true);

    m_task.LoadFromString(data.task);
    m_task.LoadFbTask(data.fbTask);
    GetPlayerLogic()->SetCurMainStoryID(m_task.m_nMainTaskData);
    m_core.HeadModelId = data.headModelID;

    m_pItemContainer = RT_NEW CRegionItemContainer(g_region->m_pItemManager, this);
    int                       iMaxLineCount = g_region->m_pItemManager->GetBankMaxLineCount();
    m_pBank = RT_NEW CBag(iMaxLineCount, BANK_GRID_COLUMN_COUNT, 1, g_region->m_pItemManager);
    m_pBankPet =
        RT_NEW CBag(BANK_PET_LINE_COUNT, BANK_PET_COLUMN_COUNT, 1, g_region->m_pItemManager);

    if (m_pBank->LoadFromString(data.bank.c_str(), g_region->m_pItemManager) <= 0) {
        ERR1("[CRegionServer::CreateUser] Bank LoadFromString Method Failed!(user=%s)\n",
             data.name.c_str());
        return false;
    }
    m_pBank->SetMoney(data.bankMoney);

    if (m_pBankPet->LoadFromString(data.bankPet.c_str(), g_region->m_pItemManager) <= 0) {
        ERR1("[CRegionServer::CreateUser] BankPet LoadFromString Method Failed!(user=%s)\n",
             data.name.c_str());
        return false;
    }

    if (m_pItemContainer->LoadFromString((char*)data.items.c_str()) <= 0) {
        ERR1("[CRegionServer::CreateUser] ItemContainer LoadFromString Method Failed!(user=%s)\n",
             data.name.c_str());
        return false;
    }
    if (m_pItemContainer->LoadPetBagFromString((char*)data.pet.c_str()) <= 0) {
        ERR1("[CRegionServer::CreateUser] ItemContainer LoadPetBagFromString Method "
             "Failed!(user=%s)\n",
             data.name.c_str());
        return false;
    }
    m_pItemContainer->SetMoney(data.money);
    RecomputeAttr();

    m_core.bAdult = data.bAdult;  //读取是否成人	ac.ma

    m_IsFirstLogin  = data.isFirstLogin;  //是否第一次登陆
    m_IsDead        = data.isDead;        //是否死亡
    m_ChangeModelID = data.ChangeModelID;

    m_Skill.StartupSkill();
    m_Skill.LoadEffectFromString(data.strEffects.c_str());
    m_pItemContainer->OnCreatureAttribChanged();
    m_Skill.OnAttributeChanged();

    m_core.SetHp(data.attr.hp);
    m_core.SetMp(data.attr.mp);

    m_FBID = data.dungeonUID;  // 副本唯一ID
    //rendh 2010.01.21
    m_dwGroupID = data.LineSYSGroupID;
    m_dwLineID  = data.LineSYSLineID;
    //rendh 2010.01.21
    LoadExtraData(data.extraData);
    ComputeCardTime(m_cardLastTime, data.onlineTime == 0);

    JudgeDayTimeTaskIsAnotherDay();  //tim.yang 日常任务

    m_dwPetSave        = data.petItemID;
    m_core.huiyuantime = data.huiyuanTime;  //tim.yang 会员时间
    SetVip(data.isVip);
    //if (!m_core.huiyuantime.empty())
    //{
    //	if (!IsHuiYuanTimeOver(m_core.huiyuantime))
    //	{
    //		SItemID item = g_region->m_pItemManager->CreateItem(4401);
    //		item.TimeLimit = m_huiyuanTime;
    //		if (item.TimeLimit<=0)
    //			item.TimeLimit = 0;
    //		if(ItemID_IsValid(item))
    //		{
    //			AddItem(item);
    //			//m_isHuiYuan = true;
    //			SendSystemMessage(R(MSG_HUIYUAN_ADDITEM));
    //		}
    //	}
    //	else
    //	{
    //		SendSystemMessage(R(MSG_HUIYUAN_DELITEM));
    //	}
    //}
    /*
	if(m_usingCard != 0)
	{
		SendSystemMessage(R(MSG_ECARD_USING));
		ShowCardInfo();
	}
	*/

#ifdef ENABLE_OPERATOR_NEW
    // Check memory overlap here
    // RtCoreMem().CheckAll();
#endif

    return true;
}

bool CRegionCreature::IsHuiYuan() {
    guard;
    if (m_pItemContainer && m_pItemContainer->m_Bag.CheckIsHuiyuan(10)) {
        return true;
    }
    return false;
    unguard;
}

//tim.yang  根据会员时间判断是否添加会员时间
bool CRegionCreature::IsHuiYuanTimeOver(string huiyuantime) {
    int year = 0, month = 0, day = 0, hour = 0;
    int nstart = 0;
    int nops   = huiyuantime.find("-", nstart);
    if (nops)
        year = atoi(huiyuantime.substr(nstart, nops - nstart).c_str());

    nstart = nops + 1;
    nops   = huiyuantime.find("-", nstart);
    if (nops)
        month = atoi(huiyuantime.substr(nstart, nops - nstart).c_str());

    nstart = nops + 1;
    nops   = huiyuantime.find(" ", nstart);
    if (nops)
        day = atoi(huiyuantime.substr(nstart, nops - nstart).c_str());

    // 计算时间
    time_t nowTime = time(NULL);
    tm     now     = *(localtime(&nowTime));

    //判断会员时间是否到期
    if ((now.tm_year + 1900 > year) || (now.tm_year + 1900 == year && now.tm_mon + 1 > month) ||
        (now.tm_year + 1900 == year && now.tm_mon + 1 == month && now.tm_mday > day)) {
        return true;
    } else {
        CBag* pBag = NULL;
        pBag       = &(m_pItemContainer->m_Bag);
        if (pBag->IsFull()) {
            SendSystemMessage(R(MSG_HUIYUAN_BAGISFULL));
            return true;
        }
        //显示会员到期时间
        tm temp;
        temp.tm_year    = year - 1900;
        temp.tm_mon     = month - 1;
        temp.tm_mday    = day;
        temp.tm_hour    = 0;
        temp.tm_min     = 0;
        temp.tm_sec     = 1;
        temp.tm_isdst   = -1;
        time_t tmp      = mktime(&temp);
        tm     overtime = *(localtime(&tmp));
        int    disday   = (overtime.tm_year - now.tm_year) * 365 + (overtime.tm_yday - now.tm_yday);
        if (disday >= 0) {
            disday += 1;
            m_huiyuanTime = nowTime + disday * 24 * 3600;
        } else {
            m_huiyuanTime = 0;
        }
        //end
        return false;
    }
}

//end

bool CRegionCreature::isteleport(int npcId) {
    SNpc* pNpc = g_TableNpc.FindNpc(npcId);
    if (!pNpc)
        return false;
    vector<long>::iterator it = pNpc->Function.begin();
    for (; it != pNpc->Function.end(); ++it) {
        if ((*it) == 7) {
            return true;
        }
    }
    return false;
}

//tim.yang 日常任务
void CRegionCreature::JudgeDayTimeTaskIsAnotherDay() {
    list<STaskInfo>::iterator it = m_task.m_taskList.begin();
    for (; it != m_task.m_taskList.end(); ++it) {
        STaskDesc* desc = g_TableTask.FindTask(it->Id);
        if (!desc)
            continue;
        if (desc->group == -1 && it->DayTimeTaskFinishTime) {
            // 计算时间
            time_t tmpTime = time(NULL);
            tm     now     = *(localtime(&tmpTime));

            long lastTime = it->DayTimeTaskFinishTime;
            tm*  last     = localtime((time_t*)&lastTime);
            if (!last)
                continue;

            int day = 0;
            day     = (now.tm_year - last->tm_year) * 365 + (now.tm_yday - last->tm_yday);

            if (day != 0) {
                it->DayTimeTaskFinishTime = 0;
            }
        }
    }
}

//end

void CRegionCreature::ComputeCardTime(long lastTime, bool newBie) {
    // 计算打卡时间
    time_t tmpTime = time(NULL);

    if (lastTime == 0) {
        if (!newBie)
            m_cardTime = 7 * 3600;
        m_cardLastTime = tmpTime;
        return;
    }

    tm     now       = *(localtime(&tmpTime));
    time_t lastTime2 = lastTime;
    tm*    last      = localtime(&lastTime2);
    if (!last)
        return;

    int day = 0;
    day     = (now.tm_year - last->tm_year) * 365 + (now.tm_yday - last->tm_yday);
    if (day < 0)
        day = 0;
    if (day > 7)
        day = 7;

    m_cardLastTime = tmpTime;
    if (m_cardTime < 7 * 3600) {
        m_cardTime += day * 3600;
        if (m_cardTime > 7 * 3600)
            m_cardTime = 7 * 3600;
    }
    if (m_cardTime < 0)
        m_cardTime = 0;
}

float CRegionCreature::GetExpRate() {
    if (IsUserPet() || IsCallNpc())
        return m_master->GetExpRate();

    float rate = m_core.RateExp.GetValue() / 100.0f;
    if (IsUser()) {
        if (m_usingCard)
            rate *= 2;
    }
    return rate;
}

void CRegionCreature::StartUseCard() {
    time_t now = time(NULL);
    tm*    tt  = localtime(&now);
    //	if(tt->tm_wday < 1 || tt->tm_wday > 4)
    //	{
    //		SendSystemMessage(R(MSG_ECARD_FAILED_DAY_ERROR));
    //		return;
    //	}
    if (m_cardTime <= 30) {
        SendSystemMessage(R(MSG_ECARD_FAILED_TIME_NOT_ENOUGH));
        return;
    }

    SendSystemMessage(R(MSG_ECARD_BEGIN));
    m_usingCard = true;
    ShowCardInfo();
}

void CRegionCreature::ShowCardInfo() {
    if (m_cardTime >= 3600) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_ECARD_TIME_H), m_cardTime / 3600,
                    (m_cardTime % 3600) / 60);
    } else {
        rt2_sprintf(g_strStaticBuffer, R(MSG_ECARD_TIME), m_cardTime / 60);
    }
    SendSystemMessage(g_strStaticBuffer);
}

void CRegionCreature::EndUseCard() {
    SendSystemMessage(R(MSG_ECARD_END));
    m_usingCard = false;
}

void CRegionCreature::InvokeEventCommand(const char* event) {
    if (!m_npcInfo)
        return;

    // find command by event
    int i;
    for (i = 0; i < (int)m_npcInfo->EventCmd.size(); i++) {
        if (m_npcInfo->EventCmd[i].event == event) {
            // exe command
            ProcessTaskKey(m_npcInfo->EventCmd[i].command, TYPE_ADD, NULL);
        }
    }
}

int CRegionUser::Prededuct(
    short sPoint, long event,
    long lAuctionId /*= 0*/)  // 预扣点数, 用来做用点数向NPC买道具, 不能为负数
{
    int lDeductOrderID = 0;

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_prededuct);
    packet.WriteLong(m_userId);
    packet.WriteLong(lDeductOrderID);
    packet.WriteLong(event);
    packet.WriteShort(sPoint);
    packet.WriteLong(lAuctionId);
    g_region->m_gws.EndSend();

    return lDeductOrderID;
}

void CRegionUser::Deduct(short sPoint, long orderid, DeductType type,
                         void* v)  // 扣点数, 用来做用点数向NPC买道具, 不能为负数
{
    //int lDeductOrderID = 0;

    CDeduct::GetDeductInstance()->AddDeductData(orderid, type, v);
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_deduct);
    packet.WriteLong(m_userId);
    packet.WriteLong(orderid);
    packet.WriteShort(sPoint);
    g_region->m_gws.EndSend();

    // return lDeductOrderID;
}

void CRegionUser::OnPrededuct(long lDeductOrderID, char cResult, short sPoint) {}

void CRegionUser::OnDeduct(long lDeductOrderID, char cResult, short sPoint) {}

void CRegionUser::LockAccount(short sLockFlag, long lLockSecond) {
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_lock_account);
    packet.WriteLong(m_userId);
    packet.WriteShort(sLockFlag);
    packet.WriteLong(lLockSecond);
    g_region->m_gws.EndSend();
}

// ----------------------------------------------------------------------------------
// Region task npc
// ----------------------------------------------------------------------------------
CRegionNpcTask::CRegionNpcTask() {
    m_pCNpcTaskLogicExt = NULL;  //PZH
    m_func              = NULL;
}

CRegionNpcTask::~CRegionNpcTask() {
    DEL_ONE(m_pCNpcTaskLogicExt);  //PZH
}

void CRegionNpcTask::Interactive(CRegionCreature* target, CG_CmdPacket* cmd) {
    char* str;
    if (!cmd->ReadString(&str))
        return;

    //LeiJun 如果NPC有所属，则只对其所属进行交互
    if (m_master && m_master != target)
        return;

    //tim.yang    神灯
    if (strcmp(str, "GodLight") == 0) {
        int num = rtRandom(3, 0);
        switch (num) {
            case 0:
                target->GodLightCreateMonster(target);
                break;
            case 1:
                target->GodLightDropItem();
                break;
            case 2:
                target->GodLightCreateLight();
                break;
        }
        g_region->m_defScene->RemoveCreature(this->m_oId);
        g_factory->DestroyObject(this);
    }
    // if(!IsScriptLoaded()) return;
    else if (strcmp(str, "default") == 0) {
        // check if can finish task
        if (m_npcInfo && m_npcInfo->DefaultTalk.size() > 0) {
            GetCNpcTaskLogicExt()->DefTalk(target);
            //g_region->ShowHtmlDialog(this,target,m_npcInfo->DefaultTalk.c_str());
        }

        if (IsScriptLoaded())
            CallScript("OnInteractive", false, target);
    } else if (strcmp(str, "QueryPoint") == 0) {
        target->QueryAccountPoing();
    } else if (strncmp(str, "Rebirth", strlen("Rebirth")) == 0) {
        if (!str[strlen("Rebirth")] && !str[strlen("Rebirth") + 1]) {
            return;
        }
        strtok(str, "_");
        char* k   = strtok(NULL, "_");
        int   num = atoi(k);
        ERR2("\n%d\n", num);
        if (num > 0 && num < 6) {
            target->m_core.RelivePostion = num;
            target->SendSystemMessage(R(RES_REBIRTH_LINKOK_SYSTEM));
        }
        g_region->ShowHtmlDialog(this, target, R(RES_REBIRTH_LINKOK));
        return;
    }
    //------------add start by tony 06.05.22------------------------//
    else if (strcmp(str, "BuyCircleTaskItem") == 0) {
        target->OnBuyTaskItem(0);
    }
    //------------add end   by tony 06.05.22------------------------//

    //3.31 LeiJun添加UI特殊功能脚本调用
    else if (strncmp(str, "Script_", 7) == 0 && target && target->IsScriptLoaded()) {
        const char* sScriptStr = str + 7;
        if (sScriptStr[0] != '\0')
            target->CallScript(sScriptStr, false, this);
        return;
    }

    //PZH
    //else
    DisposeTaskInterface(target, str);
}

void CRegionNpcTask::DisposeTaskInterface(CRegionCreature* pTarget, char* pInfo) {
    if (!pTarget || !pInfo || pInfo[0] == '\0')
        return;

    if (0 != GetCNpcTaskLogicExt()->ProcessTask(pTarget, pInfo)) {
        // script enter function must be "Ent" prefix
        if (strlen(pInfo) <= 3)
            return;
        if (pInfo[0] != 'E' || pInfo[1] != 'n' || pInfo[2] != 't')
            return;

        if (strcmp(pInfo, "EntTask") == 0) {
            // check if can accept RT_NEW task or finish task or select reward
            //PZH
            GetCNpcTaskLogicExt()->ShowTaskList(pTarget);
            //EntTask(target);
            //
        } else {
            // flag,such as accept,select
            ProcessTask(pTarget, pInfo);
        }

        InvokeEventCommand(pInfo);
    }
}

bool CRegionNpcTask::ProcessFinishTaskList(CRegionCreature* target) {
    long ret;

    // finish task list
    for (size_t i = 0; i < m_finishTask.size(); i++) {
        // 可以完成的任务优先处理
//-----------------------chg start by tony 06.05.09-------------------------------------------//
#if 0
		@		ret = target->CheckTaskFinishState(m_finishTask[i]);
		@		if (ret == TASK_STATE_CAN_FINISH)
			@		{
			@			ShowHtmlDialog(target, m_finishTask[i]->npc2AfterFinishTalk.c_str());
			@			return true;
			@		}
#else
        ret = target->CheckTaskFinishState(m_finishTask[i], this);
        if (ret == TASK_STATE_CAN_FINISH) {
            if (!IsCircleTaskNpc()) {
                ShowHtmlDialog(target, m_finishTask[i]->npc2AfterFinishTalk.c_str());
            }
            return true;
        }
#endif
        //-----------------------chg end   by tony 06.05.09-------------------------------------------//
    }
    for (size_t i = 0; i < m_finishTask.size(); i++) {
        //-----------------------chg start by tony 06.05.09-------------------------------------------//
#if 0
		@		ret = target->CheckTaskFinishState(m_finishTask[i]);
#else
        ret = target->CheckTaskFinishState(m_finishTask[i], this);
#endif
        //-----------------------chg end   by tony 06.05.09-------------------------------------------//

        if (ret == TASK_STATE_DOING) {
            if (m_finishTask[i]->circleTask == 0) {
                ShowHtmlDialog(target, m_finishTask[i]->npc2InTaskTalk.c_str());
            }
            //-----------------------add start by tony 06.05.09-------------------------------------------//
            else {
                STaskInfo* info = target->m_task.FindTask(m_finishTask[i]->id);
                if (!info)
                    continue;

                //ShowHtmlDialog(target,m_acceptTask[i]->npc1InTaskTalk.c_str());
                switch (m_finishTask[i]->circleTask) {
                    case TASK_CIRCLE_ITEM: {
                        SItemBase* pBase =
                            g_region->m_pItemManager->GetItemFromIndex(info->m_finditemID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_finishTask[i]->npc1InTaskTalk.c_str(),
                                                 "FIND_ITEM", (char*)pBase->name);
                    } break;
                    case TASK_CIRCLE_FINDNPC: {
                        SNpc* pNpc = g_TableNpc.FindNpc(info->m_findnpcID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_finishTask[i]->npc1InTaskTalk.c_str(),
                                                 "FIND_NPC", (char*)pNpc->Name.c_str());
                    } break;
                    case TASK_CIRCLE_BATTLE: {
                        SNpc* pNpc = g_TableNpc.FindNpc(info->m_killmonsterID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_finishTask[i]->npc1InTaskTalk.c_str(),
                                                 "KILL_MONSTER", (char*)pNpc->Name.c_str());
                    } break;
                    default:
                        break;
                }
            }
            //-----------------------add end   by tony 06.05.09-------------------------------------------//
            return true;
        }
    }

    /*
	// finish task list
	for(size_t i=0; i<m_finishTask.size(); i++)
	{
		ret = target->CheckTaskFinishState(m_finishTask[i]);
		switch(ret)
		{
		case TASK_STATE_DOING:
			ShowHtmlDialog(target,m_finishTask[i]->npc2InTaskTalk.c_str());
			return true;

		case TASK_STATE_CAN_FINISH:
			ShowHtmlDialog(target,m_finishTask[i]->npc2AfterFinishTalk.c_str());
			return true;
		}
	}
	*/
    return false;
}

bool CRegionNpcTask::IsCircleTaskNpc() {
    for (size_t i = 0; i < m_acceptTask.size(); i++) {
        if (m_acceptTask[i]->circleTask != 0) {
            return true;
        }
    }

    return false;
}

bool CRegionNpcTask::ProcessAcceptTaskList(CRegionCreature* target) {
    long ret;

    //-----------add start by tony 06.05.09-------------------------------------//
    //-reason:新添加跑环任务,与以前任务系统有很大差别,所以在npc上加以区别
    //const int itemID[] = {71, 72, 13, 14, 46, 47};
    //const int npcID[] = {1287,1288,1289,1290,1291};
    //const int monsterID[]={1,2,3};
    if (IsCircleTaskNpc()) {
        for (size_t i = 0; i < m_acceptTask.size(); i++) {
            STaskInfo* info = target->m_task.FindTask(m_acceptTask[i]->id);
            if (!info)
                continue;
            ret = target->CheckTaskAcceptState(m_acceptTask[i], this);
            if (ret == TASK_STATE_DOING) {
                //ShowHtmlDialog(target,m_acceptTask[i]->npc1InTaskTalk.c_str());
                switch (m_acceptTask[i]->circleTask) {
                    case TASK_CIRCLE_ITEM: {
                        SItemBase* pBase =
                            g_region->m_pItemManager->GetItemFromIndex(info->m_finditemID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_acceptTask[i]->npc1InTaskTalk.c_str(),
                                                 "FIND_ITEM", (char*)pBase->name);
                    } break;
                    case TASK_CIRCLE_FINDNPC: {
                        SNpc* pNpc = g_TableNpc.FindNpc(info->m_findnpcID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_acceptTask[i]->npc1InTaskTalk.c_str(),
                                                 "FIND_NPC", (char*)pNpc->Name.c_str());
                    } break;
                    case TASK_CIRCLE_BATTLE: {
                        SNpc* pNpc = g_TableNpc.FindNpc(info->m_killmonsterID);
                        g_region->ShowHtmlDialog(this, target,
                                                 m_acceptTask[i]->npc1InTaskTalk.c_str(),
                                                 "KILL_MONSTER", (char*)pNpc->Name.c_str());
                    } break;
                }

                return true;
            }
        }

        //int rnd = rand()%3;
        //ret = target->CheckTaskAcceptState(m_acceptTask[rnd], this);
        //switch(ret)
        //{
        //case TASK_STATE_DOING:
        //	ShowHtmlDialog(target,m_acceptTask[rnd]->npc1InTaskTalk.c_str());
        //	return true;

        //case TASK_STATE_CAN_ACCEPT:
        //	{
        //		switch(rnd+1)
        //		{
        //		case TASK_CIRCLE_ITEM:
        //			{
        //				int size = sizeof(itemID)/sizeof(itemID[0]);
        //				int r = rand()%size;

        //				target->m_tmpfinditemID = itemID[r];
        //				SItemBase* pBase= g_region->m_pItemManager->GetItemFromIndex(target->m_tmpfinditemID);

        //				g_region->ShowHtmlDialog(this,target,m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(), "FIND_ITEM", (char*)pBase->name);
        //			}
        //			break;
        //		case TASK_CIRCLE_FINDNPC:
        //			{
        //				int size = sizeof(npcID)/sizeof(npcID[0]);
        //				int r = rand()%size;

        //				while(npcID[r]==m_npcInfo->Id)
        //				{
        //					r = rand()%size;
        //				}
        //				target->m_tmpfindnpcID = npcID[r];
        //				SNpc* pNpc = g_TableNpc.FindNpc(target->m_tmpfindnpcID);

        //				g_region->ShowHtmlDialog(this,target,m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(), "FIND_NPC", (char*)pNpc->Name.c_str());
        //			}
        //			break;
        //		case TASK_CIRCLE_BATTLE:
        //			{
        //				int size = sizeof(monsterID)/sizeof(monsterID[0]);
        //				int r = rand()%size;

        //				target->m_tmpkillmonsterID = monsterID[r];
        //				SNpc* pNpc = g_TableNpc.FindNpc(target->m_tmpkillmonsterID);

        //				g_region->ShowHtmlDialog(this,target,m_acceptTask[rnd]->npc1PreAcceptTalk.c_str(), "KILL_MONSTER", (char*)pNpc->Name.c_str());
        //			}
        //			break;
        //		}
        //		//ShowHtmlDialog(target,m_acceptTask[rnd]->npc1PreAcceptTalk.c_str());
        //	}
        //	return true;
        //}
    }
    //else
    //{
    //-----------add end   by tony 06.05.09-------------------------------------//
    // accept task list
    for (size_t i = 0; i < m_acceptTask.size(); i++) {
        //---------chg start by tony 06.05.09---------------------------------------//
#if 0
		@			ret = target->CheckTaskAcceptState(m_acceptTask[i]);
#else
        ret = target->CheckTaskAcceptState(m_acceptTask[i], this);
#endif
        switch (ret) {
            case TASK_STATE_DOING:
                ShowHtmlDialog(target, m_acceptTask[i]->npc1InTaskTalk.c_str());
                return true;

            case TASK_STATE_CAN_ACCEPT:
                ShowHtmlDialog(target, m_acceptTask[i]->npc1PreAcceptTalk.c_str());
                return true;
        }
    }
    //}
    return false;
}

bool CRegionNpcTask::ProcessTask(CRegionCreature* target, char* str) {
    char* link = NULL;

    // finish task list
    for (size_t i = 0; i < m_finishTask.size(); i++) {
        //----------chg start by tony 06.05.11--------------------------//
#if 0
		@		if (target->TaskProcess(m_finishTask[i], str, &link))
#else
        if (target->TaskProcess(m_finishTask[i], str, &link, this))
#endif
        //----------chg end   by tony 06.05.11--------------------------//
        {
            ShowHtmlDialog(target, link);
            return true;
        }
    }

    // accept random task
    /*if(m_npcInfo->RandomTask)//PZH 取消NPC随机任务
	{
		int rate = 0;
		int rd = GetRand(100);
		size_t i = 0;
		for(i=0; i<m_npcInfo->RandomTaskRate.size(); i++)
		{
			rate += m_npcInfo->RandomTaskRate[i];
			if(rd < rate) break;
		}
		if(i < m_acceptTask.size())
		{
			if(target->TaskProcess(m_acceptTask[i],m_acceptTask[i]->acceptFlag.c_str(),&link))
			{
				ShowHtmlDialog(target,link);
				return true;
			}
		}
	}
	else*/
    {
        size_t i = 0;
        for (i = 0; i < m_acceptTask.size(); i++) {
            if (target->TaskProcess(m_acceptTask[i], str, &link)) {
                //ShowHtmlDialog(target,link);
                return true;
            }
        }
    }
    g_region->ShowHtmlDialog(this, target, R(RES_DEFAULT_TALK));
    return false;
}

bool CRegionNpcTask::GetNpcFunction(long func) {
    return (m_func & func) != 0;
}

void CRegionNpcTask::SetNpcFunction(long func) {
    m_func |= func;
}

void CRegionNpcTask::EntTask(CRegionCreature* target) {
    if (ProcessFinishTaskList(target))
        return;

    if (ProcessAcceptTaskList(target))
        return;

    g_region->ShowHtmlDialog(this, target, R(RES_DEFAULT_TALK));
    return;
}

//PZH
CNpcTaskLogicExt* CRegionNpcTask::GetCNpcTaskLogicExt() {

    if (NULL == m_pCNpcTaskLogicExt) {
        m_pCNpcTaskLogicExt = RT_NEW CNpcTaskLogicExt(*this);
    }
    return m_pCNpcTaskLogicExt;
}

//
void CRegionNpcTask::ShowHtmlDialog(CRegionCreature* target, const char* link) {
    if (link && strlen(link) != 0) {
        g_region->ShowHtmlDialog(this, target, link);
    } else {
        g_region->ShowHtmlDialog(this, target, m_npcInfo->DefaultTalk.c_str());
    }
}

void CRegionCreature::UpdateMatchToClient() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_match_list);
    WriteMatchToNet(g_sendCmd);
    RecvCmd(g_sendCmd);
}

void CRegionCreature::WriteMatchToNet(CG_CmdPacket* cmd) {
    cmd->WriteShort((short)m_listMatchID.size());
    std::list<short>::iterator it;
    for (it = m_listMatchID.begin(); it != m_listMatchID.end(); it++) {
        cmd->WriteShort((short)*it);
    }
}

void CRegionCreature::AddMatch(short sMatchID) {
    std::list<short>::iterator it;
    for (it = m_listMatchID.begin(); it != m_listMatchID.end(); it++) {
        if (*it == sMatchID)
            break;
    }
    if (it == m_listMatchID.end()) {
        m_listMatchID.push_back(sMatchID);
        UpdateMatchToClient();
    }
}

void CRegionCreature::DelMatch(short sMatchID) {
    std::list<short>::iterator it;
    for (it = m_listMatchID.begin(); it != m_listMatchID.end(); it++) {
        if (*it == sMatchID) {
            m_listMatchID.erase(it);
            UpdateMatchToClient();
            break;
        }
    }
}

bool CRegionUser::CheckBankPass(const std::string& strPass) {
    if (m_bankPass.empty()) {
        if (strPass.empty())
            return true;
        else
            return false;
    }

    char strMD5Password[36];
    rtMD5StringString((char*)strPass.c_str(), strMD5Password);
    std::string strMD5Check;
    for (int i = 0; i < 16; i++) {
        strMD5Check.push_back(strMD5Password[i]);
    }

    if (m_bankPass == strMD5Check)
        return true;
    else
        return false;
}

int CRegionCreature::FindInSellRareItem(DWORD itemId) {
    int idx = 0;
    stlforeach(std::vector<SNpcSellRareItem>, iter, m_NpcSellRareItem) {
        SNpcSellRareItem& sri = *iter;
        if (sri.item.id == itemId)
            return idx;
        idx++;
    }
    return -1;
}

bool CRegionCreature::StartRareItemSell() {
    m_bNpcSellRareItemStart = true;
    return true;
}

bool CRegionCreature::StopRareItemSell() {
    m_bNpcSellRareItemStart = false;
    return true;
}

void CRegionCreature::OnBuyTaskItem(int taskType) {
    switch (taskType) {
        case 0: {
            STaskInfo* task = m_task.FindCircleTask();
            if (task) {
                STaskDesc* desc = g_TableTask.FindTask(task->Id);
                if (desc && desc->circleTask == TASK_CIRCLE_ITEM) {
                    //- 检查玩家金钱是否足够
                    if (GetGold() < 1000000) {
                        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_BUY_MONEY_NOT_ENOUGH), 1000000,
                                    GetGold());
                        SendSystemMessage(g_strStaticBuffer);
                        return;
                    }

                    //- 创建道具并给予玩家
                    SItemID item = g_region->m_pItemManager->CreateItem(task->m_finditemID);
                    if (ItemID_IsValid(item)) {
                        if (AddItem(item)) {
                            LOG1("AddItem id[%d]\n", task->m_finditemID);
                            //- 扣除金钱
                            RemoveGold(1000000);
                            UpdateToClient(NET_CRE_UPDATE_TASK);
                        }
                    }
                } else {
                    SendSystemMessage("你没有这样的任务");
                    LOG("TaskError: circle task is not finding item\n");
                }
            } else {
                SendSystemMessage("你没有这样的任务");
            }
        } break;
        default:
            break;
    }
}

bool CRegionUser::ForbidUser(bool bBan, ECreatureState eType, long lTimeSecond) {
    //CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    //packet.WriteShort(r2g_forbid_user);
    //   if (bBan)
    //   {
    //    packet.WriteByte(1);
    //   }else
    //   {
    //    packet.WriteByte(0);
    //   }
    //   packet.WriteString((char*)this->m_username.c_str());
    //packet.WriteLong(eType);
    //packet.WriteLong(lTimeSecond*1000);
    //g_region->m_gws.EndSend();
    return true;
}

void CRegionUser::SaveAllActivityInfo(bool bDelete) {
    for (ACTINFOS::iterator itInfo = m_ActInfos.begin(); itInfo != m_ActInfos.end(); ++itInfo) {
        CRegionActManager::GetSingle()->SaveUserInfo(
            itInfo->first, this, GetActivityJoin(itInfo->first), itInfo->second);
        if (bDelete)
            DEL_ONE(itInfo->second);
    }
    if (bDelete) {
        m_ActInfos.clear();
    }
}

bool CRegionUser::wgCheckServer(unsigned char* buf, int& iDataSize, int iBufSize) {
    int           iCheckData = *(int*)buf;
    short         cmdID      = ((short*)buf)[3];
    unsigned long dwNow      = (unsigned long)time(NULL);
    if (cmdID == c2r_attack || cmdID == c2r_update_position) {
        // 计算可能的Seed
        m_lClientLUASeed = ((m_lClientLUASeed * 205839 + 2835234) / 17) % 65536;
        MSG2("%d, %d\n", iCheckData - m_lClientLUASeed, m_lClientLUASeed);
#ifdef _WIN32
        if (((dwNow - m_lClientLastCheckedTime) > 3) || (cmdID == c2r_attack))
#else
        if (m_lClientLastCheckedTime == 0) {
            m_lClientLastCheckedTime = dwNow;
        }
        if (((dwNow - m_lClientLastCheckedTime) > 5 * 60) ||
            (cmdID == c2r_attack))  // 每5分钟检查一次
#endif
        {
            if (cmdID == c2r_update_position) {
                m_lClientLastCheckedTime = dwNow + rand() % 60;  // 随机1分钟
            }
            if (!g_region->JhWgCheck(iCheckData, m_lClientLUASeed, cmdID))  // 返回false表示找到外挂
            {
                m_iJhWgCheckedCnt++;
                if (g_region->m_bJhWgServer) {
                    if (g_region->m_pJhWgServer) {
                        g_region->m_pJhWgServer->CheckedUser(m_accountName.c_str(),
                                                             m_username.c_str(), this->m_userId,
                                                             m_iJhWgCheckedCnt);
                    }
                } else {
                    if (m_iJhWgCheckedCnt > 57) {
                        LOG1("玩家 [%s] 使用外挂\n", m_username.c_str());
                        m_dummy->SendSystemMessage("为了创造公平的游戏环境，请勿使用外挂。使用外挂"
                                                   "者每五分钟被禁止移动一分钟");
                        ForbidUser(true, CreState_CanMove, 60);
                    }
                }
            }
        }
    }
    iDataSize -= 4;
    memcpy(buf, buf + 4, iDataSize);
    return true;
}

void CRegionUser::RecvCmd(CG_CmdPacket* cmd) {
    if (!m_session)
        return;

    if (g_bUnWg) {
        short cmdID;
        bool  bEncrypt     = true;
        bool  bIsBroadcast = true;

        char* pCmdBuf = cmd->GetByteData();
        memcpy(&cmdID, pCmdBuf, 2);

        //if(cmdID != r2c_init)
        //{
        //	bEncrypt = true;
        //}else
        //{
        //	bEncrypt = false;
        //}

        if (cmdID != r2c_init) {
            if (cmdID == r2c_update_show_info || cmdID == r2c_remove_item ||
                cmdID == r2c_learn_skill || cmdID == r2c_subsystem_mail ||
                cmdID == r2c_you_are_dead || cmdID == r2c_subsystem_castlewar ||
                cmdID == r2c_killmonster_addrige) {
                bIsBroadcast = false;
            }

            unsigned char buf[1024 * 20];
            int           size = 1024 * 20;
            memset((void*)buf, 0, sizeof(buf));
            cmd->SaveToBuffer((char*)buf, size);

            size = sizeof(buf);
            if (NES_Encrypt_S(m_userId, buf, size, true, bIsBroadcast)) {
                LOG1("NES_Encrypt_S Failed(size[%d])\n", size);
                return;
            }

            if (size > MAX_CMD_PACKET_SIZE) {
                LOG1("After NES_Encrypt_S, size is [%d], overflow\n", size);
                return;
            }

            g_sendCmdWG->BeginWrite();
            g_sendCmdWG->WriteBinary((char*)buf, size);
            if (!m_session->SendPacket(g_sendCmdWG))
                MSG4("UserInfo: user recv cmd error,cmd id is %d\n", cmdID);
            return;
        }
    }

    if (!m_session->SendPacket(cmd))
        MSG("UserInfo: user recv cmd error\n");
}

void CRegionCreature::OnAddAccountTime(long lUserID, char* pAccountTime, char cSucceed) {
    if (cSucceed == 0)  // 如果失败
    {
        SItemID item = g_region->m_pItemManager->CreateItem(tempItemID);
        if (ItemID_IsValid(item)) {
            AddItem(item);
        }
        rt2_sprintf(g_strStaticBuffer, "充值失败");  // "购买点卡失败: %s"
        SendSystemMessage(g_strStaticBuffer);
    } else {
        rt2_sprintf(g_strStaticBuffer,
                    "充值成功；目前您的会员到期时间为 %s,重新登陆后会员时间会被刷新！！",
                    pAccountTime);
        SendSystemMessage(g_strStaticBuffer);
    }
}

void CRegionCreature::WriteAccountTime(int index) {
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_add_account_time);
    packet.WriteLong(m_userInfo->m_userId);
    switch (index) {
        case 735:
            packet.WriteLong(31);
            break;
        case 4627:
            packet.WriteLong(93);
            break;
        case 4628:
            packet.WriteLong(186);
            break;
    }
    tempItemID = index;
    RemoveItem(index, 1);
    g_region->m_gws.EndSend();
    WriteUseitemLog(index);
}

void CRegionCreature::WriteUseitemLog(int index) {
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_UseItem);
    packet.WriteLong(m_userInfo->m_userId);
    packet.WriteString((char*)m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(index);                                         // strParam1
    g_region->m_gws.EndSend();
}

// add by zzh
void CRegionCreature::UpdateTrigger() {
    if (m_dungeonUID == 0) {
        return;
    }

    CRsDungeonMgr::SRsDungeon* fb = g_region->m_fbMgr->FindDungeon(m_dungeonUID);

    if (!fb)
        return;

    std::vector<STrigger>::iterator iter;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_trigger_close);
    g_sendCmd->WriteLong(m_dungeonUID);
    g_sendCmd->WriteLong(m_dungeonTID);
    g_sendCmd->WriteLong(fb->process);
    for (iter = fb->trigger.begin(); iter != fb->trigger.end(); ++iter) {
        if ((*iter).id != 0) {
            long close = (*iter).close;
            g_sendCmd->SerializeBit(close, 1);
        }
    }
    RecvCmd(g_sendCmd);
}

// add by zzh
void CRegionCreature::StartTrigger(CRegionCreature* npc, bool AotoOpen /* = false*/) {
    if (!npc)
        return;

    long start    = 0;
    int  id       = npc->triggerID;
    long delBlock = 0;

    CRsDungeonMgr::SRsDungeon* fb       = g_region->m_fbMgr->FindDungeon(m_dungeonUID);
    STrigger*                  pTrigger = fb->GetTrigger(id);

    if (!pTrigger)
        return;

    if (!ProcessTaskKeyList(pTrigger->startKey, TYPE_CHECK, NULL) && !AotoOpen) {
        return;
    }

    if (pTrigger->maxUseNum && pTrigger->curUseNum >= pTrigger->maxUseNum && !AotoOpen)
        return;

    if (pTrigger->type == 1 || pTrigger->type == 2 || pTrigger->type == 3) {
        if (pTrigger->close) {
            if (npc->triggerOpen == 0 && npc->m_lTriggerTime <= rtMilliseconds()) {
                pTrigger->triggerCurNum++;
                if (pTrigger->triggerCurNum >= pTrigger->triggerNum) {
                    if (!ProcessTaskKeyList(pTrigger->startKeyRemove, TYPE_REMOVE, NULL) &&
                        !AotoOpen) {
                        pTrigger->triggerCurNum--;

                        start = 2;
                        g_sendCmd->BeginWrite();
                        g_sendCmd->WriteShort(r2c_start_trigger);
                        g_sendCmd->WriteShort(id);
                        g_sendCmd->SerializeBit(delBlock, 1);
                        g_sendCmd->WriteByte(pTrigger->close);
                        g_sendCmd->WriteLong(npc->GetObjectId());
                        g_sendCmd->SerializeBit(start, 2);
                        RecvCmd(g_sendCmd);

                        return;
                    }
                    for (int i = 0; i < pTrigger->triggerNum; i++) {
                        CRegionCreature* cre = m_scene->FindCreature(pTrigger->triggerUID[i]);
                        if (cre) {
                            cre->m_lTriggerTime = rtMilliseconds() + pTrigger->UseTime;
                        }
                    }
                    pTrigger->triggerCurNum = pTrigger->triggerNum;
                    pTrigger->close         = 0;
                    start                   = 1;
                    if (!AotoOpen)
                        ProcessTaskKeyList(pTrigger->startKeyAdd, TYPE_ADD, NULL);
                }
                npc->triggerOpen    = 1;
                npc->triggerIsClose = 0;
            } else
                return;
        } else {
            if (npc->triggerOpen == 1 && npc->m_lTriggerTime <= rtMilliseconds()) {
                pTrigger->triggerCurNum--;
                if (pTrigger->triggerCurNum <= 0) {
                    if (!ProcessTaskKeyList(pTrigger->startKeyRemove, TYPE_REMOVE, NULL) &&
                        !AotoOpen) {
                        pTrigger->triggerCurNum++;

                        start = 2;
                        g_sendCmd->BeginWrite();
                        g_sendCmd->WriteShort(r2c_start_trigger);
                        g_sendCmd->WriteShort(id);
                        g_sendCmd->SerializeBit(delBlock, 1);
                        g_sendCmd->WriteByte(pTrigger->close);
                        g_sendCmd->WriteLong(npc->GetObjectId());
                        g_sendCmd->SerializeBit(start, 2);
                        RecvCmd(g_sendCmd);

                        return;
                    }
                    for (int i = 0; i < pTrigger->triggerNum; i++) {
                        CRegionCreature* cre = m_scene->FindCreature(pTrigger->triggerUID[i]);
                        if (cre) {
                            cre->m_lTriggerTime = rtMilliseconds() + pTrigger->UseTime;
                        }
                    }
                    pTrigger->triggerCurNum = 0;
                    pTrigger->close         = 1;
                    start                   = 1;
                    if (!AotoOpen)
                        ProcessTaskKeyList(pTrigger->startKeyAdd, TYPE_ADD, NULL);
                }
                npc->triggerOpen    = 0;
                npc->triggerIsClose = 1;
            } else
                return;
        }
    }
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_start_trigger);
    // 机关ID
    g_sendCmd->WriteShort(id);
    // 是否删除阻挡
    g_sendCmd->SerializeBit(delBlock, 1);
    // 开启还是关闭
    g_sendCmd->WriteByte(pTrigger->close);
    // 触发物件ID
    g_sendCmd->WriteLong(npc->GetObjectId());
    // 是否启动
    g_sendCmd->SerializeBit(start, 2);

    if (start == 1) {
        // 机关被触发的次数
        if (!AotoOpen)
            pTrigger->curUseNum++;
        // 表现物件ID
        for (int i = 0; i < pTrigger->opendoorNum; i++) {
            CRegionCreature* cre = m_scene->FindCreature(pTrigger->opendoorUID[i]);
            if (cre) {
                cre->triggerIsClose = pTrigger->close;
            }
            g_sendCmd->WriteLong(pTrigger->opendoorUID[i]);
        }
        // 主要表现物件ID
        //g_sendCmd->WriteLong(pTrigger->opendoorMainUID);

        if (pTrigger->type == 3) {
            // 阻挡物件ID
            for (int i = 0; i < pTrigger->blockCount; i++) {
                g_sendCmd->WriteLong(pTrigger->blockUID[i]);
                if (pTrigger->close) {
                    // 添加阻挡物件
                    long pos[2];
                    int  dir;
                    pos[0] = pTrigger->blockPos[i].x;
                    pos[1] = pTrigger->blockPos[i].y;
                    dir    = pTrigger->blockDir[i];
                    if (pTrigger->block[i]) {
                        CRegionCreature* cre = g_region->CreateNpc(pTrigger->block[i]);
                        if (cre) {
                            pTrigger->blockUID[i] = cre->GetObjectId();
                            cre->triggerID        = pTrigger->id;
                            m_scene->AddCreature(cre, pos, dir);
                        }
                    }
                } else {
                    CRegionCreature* cre = m_scene->FindCreature(pTrigger->blockUID[i]);
                    if (cre) {
                        m_scene->RemoveCreature(pTrigger->blockUID[i]);
                        g_factory->DestroyObject(cre);
                    }
                }
            }
        } else {
            g_region->m_fbMgr->StartTrigger(fb, pTrigger->id, !pTrigger->close);
        }

        if (fb->process < pTrigger->process) {
            fb->process = pTrigger->process;
        }
    }

    CRegionUser*               user;
    std::list<DWORD>::iterator it = fb->listUser.begin();
    for (; it != fb->listUser.end(); it++) {
        user = g_region->FindUser(*it);
        if (user && user->m_dummy) {
            user->m_dummy->RecvCmd(g_sendCmd);
        }
    }

    if (start == 1) {
        // 处理强制带动机关
        for (int i = 0; i < pTrigger->closeTriggerNum; i++) {
            if (pTrigger->closeTrigger[i]) {
                STrigger* pForced = fb->GetTrigger(pTrigger->closeTrigger[i]);
                if (!pForced)
                    continue;
                if (pForced->fbID != fb->info->id)
                    continue;
                CRegionCreature* cre;
                for (int j = 0; j < pForced->triggerNum; j++) {
                    cre = m_scene->FindCreature(pForced->triggerUID[j]);
                    if (!cre)
                        continue;
                    cre->triggerOpen = pForced->close;
                }
                if (!cre)
                    continue;
                pForced->triggerCurNum = pForced->triggerNum;
                if (cre->triggerOpen)
                    pForced->curUseNum = pForced->maxUseNum;
                else
                    pForced->curUseNum = 0;
                StartTrigger(cre, true);
            }
        }
    }
}

// add by zzh
void CRegionCreature::EnterDungeon() {
    if (m_dungeonUID == 0) {
        return;
    }

    CRsDungeonMgr::SRsDungeon* fb = g_region->m_fbMgr->FindDungeon(m_dungeonUID);

    if (!fb)
        return;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_enter_dungeon);
    g_sendCmd->WriteLong(m_dungeonUID);
    g_sendCmd->WriteLong(m_dungeonTID);
    g_sendCmd->WriteLong(fb->process);
    RecvCmd(g_sendCmd);
}

// add by zzh
void CRegionCreature::LeaveDungeon() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_enter_dungeon);
    g_sendCmd->WriteLong(0);
    g_sendCmd->WriteLong(0);
    g_sendCmd->WriteLong(0);
    RecvCmd(g_sendCmd);
}

// add by zzh
void CRegionCreature::RefreshEquipWear() {
    char count = 0;
    char part[16];
    long wear[16];
    for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
        SItemID ret = m_pItemContainer->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
        if (ItemID_IsValid(ret) && (ItemIsWeapon(ret) || ItemIsArmor(ret) || ItemIsShipin(ret))) {
            if (ret.curWear <= 0) {
                //m_pItemContainer->UnEquip((CItemContainerBase::EEquipParts)i,true);
                //m_pItemContainer->UpdateItemToClient((CItemContainerBase::EEquipParts)i);
                //UseItem(-1, -1, -1, (CItemContainerBase::EEquipParts)i, false);
                ret.curWear = 0;
                if (ret.Valid) {
                    m_pItemContainer->OnCreatureAttribChanged((CItemContainerBase::EEquipParts)i);
                    m_pItemContainer->SetEquipValid((CItemContainerBase::EEquipParts)i, 0);
                }
                continue;
            }
            part[count] = i;
            wear[count] = ret.curWear;
            count++;
        }
    }
    if (count > 0) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_refresh_equip_wear);
        g_sendCmd->WriteByte(count);
        for (int i = 0; i < count; i++) {
            g_sendCmd->WriteByte(part[i]);
            g_sendCmd->WriteLong(wear[i]);
        }
        RecvCmd(g_sendCmd);
    }
}

void CRegionCreature::ClientDoCommand(char command, long finished, bool sendself /* = false*/) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_do_command);
    g_sendCmd->WriteLong(GetObjectId());
    g_sendCmd->SerializeBit(finished, 1);
    g_sendCmd->WriteByte(command);
    if (m_scene) {
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, sendself ? NULL : this);
    }
}

// add by zzh
void CRegionCreature::SendPkGetItemList(std::vector<SItemID> itemList) {
    m_vPkGetItemList = itemList;

    char dropcount = m_vPkGetItemList.size();

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_pk_drop_item);
    g_sendCmd->WriteByte(dropcount);
    vector<SItemID>::iterator it;
    if (m_vPkGetItemList.size()) {
        it = m_vPkGetItemList.begin();
        for (; it != m_vPkGetItemList.end(); ++it) {
            (*it).Serialize(g_sendCmd);
        }
    }
    RecvCmd(g_sendCmd);
    m_dwPkGetItemTime = rtGetMilliseconds() + PK_DROP_TIME_MILLSECOND;
}

// add by zzh
bool CRegionCreature::IsInProtect() {
    return m_dwProtectTime != 0;
}

// add by zzh
void CRegionCreature::SetProtectTime(DWORD millseconds, bool add /* = false*/) {
    if (add) {
        m_dwProtectTime += rtGetMilliseconds() + millseconds;
    } else {
        m_dwProtectTime = rtGetMilliseconds() + millseconds;
    }

    if (!(&m_Skill)) {
        return;
    }
    //是否已经有这个效果  如果有则将这个效果删除  重新赋值新的效果
    if ((&m_Skill)->FindEffectbyId(PKPROTECT)) {
        (&m_Skill)->DeleteEffect(PKPROTECT, true);
    }

    CActorSkill::SActorEffect* pEffect;
    (&m_Skill)->m_listEffect.push_back(CActorSkill::SActorEffect());
    SSkillEffect* pEffectAttr = Skill()->FindEffect(PKPROTECT);

    if (pEffectAttr) {
        pEffect              = &((&m_Skill)->m_listEffect.back());
        pEffect->bNeedDelete = false;
        pEffect->dwParentUID = 0;
        pEffect->iRunTimes   = 0;
        pEffect->dwRunTime   = 0;
        pEffect->pAttr       = pEffectAttr;
        if (pEffect->pAttr) {
            pEffect->pAttr->iCycle = m_dwProtectTime - rtGetMilliseconds();
        }
        /*(&m_Skill)->OnEffectOpen(pEffect);*/
        (&m_Skill)->SetbRun(true);
        (&m_Skill)->OnEffectModify();
    }

    if (m_pet) {
        m_pet->SetProtectTime(millseconds, add);
    }
}

void CRegionCreature::Setrefusesetting() {
    ////发送消息到GameWorld
    //CG_CmdPacket* pPacket= GameWorldPacket();
    //pPacket->WriteByte(R2GW_INVITE_OTHER);
    //pPacket->WriteLong(OtherDBID);
    //SendToGameWorld();
}

void CRegionCreature::AddSkillSeries() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_Skill_Series);
    g_sendCmd->WriteLong(GetObjectId());
    g_sendCmd->WriteLong((long)m_SkillSeries);
    RecvCmd(g_sendCmd);
}

// add by zzh
void CRegionCreature::EnterCombat() {
    m_lIsCombat       = 1;
    m_dwCombatEndTime = rtGetMilliseconds() + COMBAT_KEEP_MILLSECONDS;
    if (m_master) {
        m_master->EnterCombat();
    }
}

// add by zzh
void CRegionCreature::LeaveCombat() {
    //m_lIsCombat = 0;
    m_dwCombatEndTime = rtGetMilliseconds() + LEAVE_COMBAT_MILLSECONDS;
}

// add by zzh
bool CRegionCreature::IsInCombat() {
    return m_lIsCombat != 0;
}

// add by zzh
void CRegionCreature::RemoveAllTargets() {
    if (m_targets.empty())
        return;
    if (IsMonster()) {
        stlforeach(STargetList, iter, m_targets) {
            CRegionCreature* target = m_scene->FindCreature(iter->Id);
            if (target) {
                target->LeaveCombat();
            }
        }
    }
    m_targets.clear();
}

// add by zzh
bool CRegionCreature::DoTeleportSkill(DWORD dwDistance) {
    float fDir[3];

    BYTE cDir = m_dir;

    float tmp[2];
    tmp[0] = ((float)(cDir & 0x3F)) / 0x3F;
    if (cDir & 0x80)
        tmp[0] = -tmp[0];
    tmp[1] = sqrt(1 - tmp[0] * tmp[0]);
    if (cDir & 0x40)
        tmp[1] = -tmp[1];
    fDir[0] = -tmp[1];
    fDir[1] = tmp[0];
    fDir[2] = 0.f;

    float l;
    l = fsqrt(fDir[0] * fDir[0] + fDir[1] * fDir[1] + fDir[2] * fDir[2]);
    if (l > DELTA) {
        fDir[0] /= l;
        fDir[1] /= l;
        fDir[2] /= l;
    } else {
        fDir[1] = 1.f;
        fDir[0] = fDir[2] = 0.f;
    }

    //RtCoreLog().Debug("teleport dir: %f,%f,%d\n",fDir[0],fDir[1],(int)cDir);

    long X = m_pos[0];
    long Y = m_pos[1];

    if (m_scene && m_scene->m_pTerrain) {
        int count = dwDistance / 20;
        for (int i = 0; i < count; i++) {
            float fX = m_pos[0] - fDir[0] * 20.f * (i + 1);
            float fY = m_pos[1] - fDir[1] * 20.f * (i + 1);

            const RtSceneBlockTerrain::STileAttr* pAttr =
                m_scene->m_pTerrain->GetTerrainAttr(fX / 20, fY / 20);
            if (!pAttr || (pAttr && pAttr->bWalk)) {
                break;
            } else {
                X = fX;
                Y = fY;
            }
        }
    }
    if (m_userInfo && (X != m_pos[0] || Y != m_pos[1])) {
        m_bBroadcastTele = true;
        IsDoTeleport     = true;
        g_region->m_gws.MoveUserToPosition(m_userInfo->m_userId, 0, X / 20, Y / 20);
    }
    return true;
}

// add by zzh
bool CRegionCreature::CallSpirit(long dwSpiritID) {
    if (dwSpiritID) {
        CRegionCreature* cre = g_region->CreateSpirit(dwSpiritID);
        if (!cre)
            return false;

        if (!m_scene->AddCreature(cre, m_pos, 0)) {
            g_factory->DestroyObject(cre);
            return false;
        }
    } else if (IsMonster() && m_npcInfo && m_npcInfo->CallSpiritNum) {
        for (int i = 0; i < m_npcInfo->CallSpiritNum; i++) {
            int   SpiritID = m_npcInfo->CallSpiritID[i];
            float fRate    = m_npcInfo->CallSpiritRate[i];

            if (GetProb(fRate)) {
                CRegionCreature* cre = g_region->CreateSpirit(SpiritID);
                if (!cre)
                    return false;

                if (!m_scene->AddCreature(cre, m_pos, 0)) {
                    g_factory->DestroyObject(cre);
                    return false;
                }
                cre->SetTimer(CRE_TIMER_KILL_SELF, 90000);
            }
        }
    }
    return true;
}

// add by zzh
bool CRegionCreature::StopCatchSpirit(char cInfo, bool broadcast /* = true*/) {
    if (!IsUser())
        return false;

    CRegionCreature* cre = m_scene->FindCreature(m_dwCatchPet);
    if (cre)
        cre->m_dwCatchMe = 0;

    m_dwCatchPet = 0;

    if (cInfo) {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_catch_pet);
        g_sendCmd->WriteByte(cInfo);
        g_sendCmd->WriteLong(m_oId);
        if (broadcast && m_scene)
            m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        else
            RecvCmd(g_sendCmd);
    }
    return true;
}

// add by zzh
bool CRegionCreature::AddCatchSpiritState(CRegionCreature* cre) {
    if (!IsUser())
        return false;
    CCreatureState* state = FindStateByType(CRE_STATE_CATCH_PET);
    if (state || !cre || !cre->IsSpirit()) {
        state->End();
        return false;
    }

    state = RT_NEW CCreState_CatchPet;
    SCreStateParam param;
    if (!AddState(state, &param)) {
        DEL_ONE(state);
        return false;
    } else {
        if (!((CCreState_CatchPet*)state)->SetSpirit(cre)) {
            DEL_ONE(state);
            return false;
        } else {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_catch_pet);
            g_sendCmd->WriteByte(PET_CATCH_ERROR_START);
            g_sendCmd->WriteLong(m_oId);
            g_sendCmd->WriteLong(cre->m_oId);
            if (m_scene)
                m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
        }
    }
    return true;
}

// add by zzh
bool CRegionCreature::AddPetToBagByItem(long itemID) {
    CBag* pPetBag = &m_pItemContainer->m_PetBag;
    if (!pPetBag)
        return false;

    //携带数量上限=3+int（玩家修为等级/20
    if (pPetBag->GetBagItemNum() >= 3 + m_core.GradeLev / 20) {
        SendSystemMessage(R(MSG_PET_CANNOT_ADDPET_GRADE));
        return false;
    }

    SItemID pet = g_region->m_pItemManager->CreateItem(itemID);
    if (!ItemIsPet(pet))
        return false;

    int page, i, j;
    if (pPetBag->AddItem(pet, &page, &i, &j)) {
        CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page, i,
                                                           j, pet);
        RecvCmd(g_sendCmd);
        return true;
    }
    return false;
}

// add by zzh
bool CRegionCreature::AddPetToBag(long dwPetID) {
    if (!IsUser())
        return false;
    CBag* pPetBag = &m_pItemContainer->m_PetBag;
    if (!pPetBag)
        return false;

    //携带数量上限=3+int（玩家修为等级/20
    if (pPetBag->GetBagItemNum() >= 3 + m_core.GradeLev / 20) {
        SendSystemMessage(R(MSG_PET_CANNOT_ADDPET_GRADE));
        return false;
    }

    SPet* pPet = g_TablePet.FindPet(dwPetID);
    if (!pPet)
        return false;

    SItemID pet = g_region->m_pItemManager->CreateItem(pPet->dwItemID);
    if (!ItemIsPet(pet))
        return false;

    int page, i, j;
    if (pPetBag->AddItem(pet, &page, &i, &j) && IsUser()) {
        CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page, i,
                                                           j, pet);
        RecvCmd(g_sendCmd);
        return true;
    }
    return false;
}

// add by zzh
bool CRegionCreature::DropUnRide(float fRate /* = 1.f*/) {
    if (!IsRidePet())
        return false;

    if (GetProb(fRate)) {
        RideOffPet();
        return true;
    }
    return false;
}

// add by zzh
bool CRegionCreature::LinkUpWithPet(long petItemID /* = 0*/, bool bResetState /* = true*/) {
    if (!IsUser())
        return false;

    if (petItemID) {
        if (m_dwLinkUpPet) {
            if (LinkUpWithPet(0) && LinkUpWithPet(petItemID)) {
                return true;
            }
            return false;
        }

        CBag* pPetBag = &m_pItemContainer->m_PetBag;

        int page, i, j;
        if (!pPetBag->Find(petItemID, &page, &i, &j))
            return false;

        SItemID pet = pPetBag->GetItem(page, i, j);

        if (!ItemIsPetSpectre(pet))
            return false;

        pet.params[PET_PARAM_STATE] = 1;
        if (pPetBag->SetItem(page, i, j, pet)) {
            if (m_pet && m_pet->m_ai) {
                ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet = pet;
                m_pet->State_SetVisible(false);
                m_pet->m_ai->m_bHaveNextNodePos = false;
            } else
                return false;

            CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page,
                                                               i, j, pet);
            RecvCmd(g_sendCmd);
        } else
            return false;

        DWORD LinkUpLev = pet.params[PET_PARAM_LINKUP_LEV];

        m_core.DamageMin.Rate *= (1 + (LinkUpLev * 0.03f));
        m_core.DamageMax.Rate *= (1 + (LinkUpLev * 0.03f));
        m_core.Armor.Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.MaxHp.Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.MaxMp.Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.Attack.Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.Dodge.Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.RestoreHp.Rate *= (1 + (LinkUpLev * 0.05f));
        m_core.ElementArmor[ELEMENT_WATER].Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.ElementArmor[ELEMENT_FIRE].Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.ElementArmor[ELEMENT_POISON].Rate *= (1 + (LinkUpLev * 0.02f));
        m_core.ElementDamage[ELEMENT_WATER].Rate *= (1 + (LinkUpLev * 0.03f));
        m_core.ElementDamage[ELEMENT_FIRE].Rate *= (1 + (LinkUpLev * 0.03f));
        m_core.ElementDamage[ELEMENT_POISON].Rate *= (1 + (LinkUpLev * 0.03f));
        RecomputeAttr();

        m_dwLinkUpPet = pet.id;

        {  //合体特效
            //是否已经有这个效果  如果有则将这个效果删除  重新赋值新的效果
            if (!(&m_Skill)->FindEffectbyId(APPOSEBODY)) {
                CActorSkill::SActorEffect* pEffect;
                (&m_Skill)->m_listEffect.push_back(CActorSkill::SActorEffect());
                SSkillEffect* pEffectAttr = Skill()->FindEffect(APPOSEBODY);

                if (pEffectAttr) {
                    pEffect              = &((&m_Skill)->m_listEffect.back());
                    pEffect->bNeedDelete = false;
                    pEffect->dwParentUID = 0;
                    pEffect->iRunTimes   = 0;
                    pEffect->dwRunTime   = 0;
                    pEffect->pAttr       = pEffectAttr;
                    /*(&m_Skill)->OnEffectOpen(pEffect);*/
                    /*(&m_Skill)->SetbRun(true);*/
                    (&m_Skill)->OnEffectModify();
                }
            }
        }

        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_subsystem_item);
        g_sendCmd->WriteShort(r2c_pet_linkup);
        g_sendCmd->WriteLong(m_dwLinkUpPet);
        RecvCmd(g_sendCmd);

        return true;
    } else if (m_dwLinkUpPet) {
        CBag* pPetBag = &m_pItemContainer->m_PetBag;

        int page, i, j;
        if (!pPetBag->Find(m_dwLinkUpPet, &page, &i, &j))
            return false;

        SItemID pet = pPetBag->GetItem(page, i, j);

        if (!ItemIsPetSpectre(pet))
            return false;

        if (bResetState)
            pet.params[PET_PARAM_STATE] = 0;

        if (pPetBag->SetItem(page, i, j, pet)) {
            if (m_pet && m_pet->m_ai) {
                ((CRegionPetAI*)m_pet->m_ai)->m_pItemPet = pet;
                m_pet->State_SetVisible(true);
                m_pet->m_ai->m_bHaveNextNodePos = false;
            } else
                return false;

            CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page,
                                                               i, j, pet);
            RecvCmd(g_sendCmd);
        } else
            return false;

        DWORD LinkUpLev = pet.params[PET_PARAM_LINKUP_LEV];

        m_core.DamageMin.Rate /= (1 + (LinkUpLev * 0.03f));
        m_core.DamageMax.Rate /= (1 + (LinkUpLev * 0.03f));
        m_core.Armor.Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.MaxHp.Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.MaxMp.Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.Attack.Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.Dodge.Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.RestoreHp.Rate /= (1 + (LinkUpLev * 0.05f));
        m_core.ElementArmor[ELEMENT_WATER].Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.ElementArmor[ELEMENT_FIRE].Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.ElementArmor[ELEMENT_POISON].Rate /= (1 + (LinkUpLev * 0.02f));
        m_core.ElementDamage[ELEMENT_WATER].Rate /= (1 + (LinkUpLev * 0.03f));
        m_core.ElementDamage[ELEMENT_FIRE].Rate /= (1 + (LinkUpLev * 0.03f));
        m_core.ElementDamage[ELEMENT_POISON].Rate /= (1 + (LinkUpLev * 0.03f));
        RecomputeAttr();

        m_dwLinkUpPet = 0;

        {  //宠物附体技能消失
            if ((&m_Skill)->FindEffectbyId(APPOSEBODY)) {
                (&m_Skill)->DeleteEffect(APPOSEBODY, true);
            }
        }

        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_subsystem_item);
        g_sendCmd->WriteShort(r2c_pet_linkup);
        g_sendCmd->WriteLong(m_dwLinkUpPet);
        RecvCmd(g_sendCmd);

        return true;
    }

    return false;
}

// add by zzh
bool CRegionCreature::IsLinkUpWithPet() {
    return m_dwLinkUpPet != 0;
}

// add by zzh
bool CRegionCreature::IsTeamFriend(CRegionCreature* cre) {
    if (m_pTeam && cre->m_userInfo) {
        for (int i = 0; i < MAX_TEAM_MENBER_COUNT; i++) {
            if (m_pTeam->mData.mMember[i].mDBID == cre->m_userInfo->m_userId) {
                return true;
            }
        }
    }
    return false;
}

// add by zzh
int CRegionCreature::GetPetNum(DWORD petId) {
    if (!m_pItemContainer)
        return 0;

    CBag* pPetBag = &m_pItemContainer->m_PetBag;
    if (!pPetBag)
        return 0;

    SPet* pPet = g_TablePet.FindPet(petId);
    if (!pPet)
        return 0;

    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(pPet->dwItemID);
    if (!pItemClass)
        return 0;

    int count = 0;
    if (m_pItemContainer->m_PetBag.FindType(pPet->dwItemID, ItemColor_All, &count))
        return count;
    return 0;
}

// add by zzh
bool CRegionCreature::RemovePetFromBag(DWORD petId, int num /* = 1*/) {

    if (!m_pItemContainer)
        return false;

    CBag* pPetBag = &m_pItemContainer->m_PetBag;
    if (!pPetBag)
        return false;

    SPet* pPet = g_TablePet.FindPet(petId);
    if (!pPet)
        return false;

    DWORD      typeId     = pPet->dwItemID;
    SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return false;

    bool ret = m_pItemContainer->m_PetBag.RemoveItem(typeId, ItemColor_All, num);

    if (ret) {
        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                   &(m_pItemContainer->m_PetBag));
        RecvCmd(g_sendCmd);
    }
    return ret;
}

// add by zzh
bool CRegionCreature::AddPetByItem(SItemID item) {
    if (!m_pItemContainer)
        return false;

    if (m_pItemContainer->m_PetBag.GetBagItemNum() >= 3 + m_core.GradeLev / 20) {
        SendSystemMessage(R(MSG_PET_CANNOT_ADDPET_GRADE));
        return false;
    }

    int  page, i, j;
    bool ret = false;
    if (ItemIsPet(item.type))
        ret = m_pItemContainer->m_PetBag.AddItem(item, &page, &i, &j);

    if (ret) {
        if (ItemIsPet(item.type))
            CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, ITEM_CONTAINER_PLAYER_PET_BAG, page, i,
                                                       j, ITEM_REASON_PICK);
        RecvCmd(g_sendCmd);
        return true;
    } else {
        return false;
    }
}

// add by zzh
void CRegionCreature::SendPetInfo() {
    if (!m_pet || !m_pet->m_ai) {
        return;
    }

    CRegionPetAI* petAI = (CRegionPetAI*)m_pet->m_ai;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_model_info);
    g_sendCmd->WriteLong(m_pet->m_oId);
    g_sendCmd->WriteString(m_pet->m_core.Name.c_str());
    g_sendCmd->WriteByte((char)petAI->GetPetType());
    g_sendCmd->WriteLong(petAI->m_pItemPet.params[PET_PARAM_FUSE_LEV]);
    g_sendCmd->WriteLong(petAI->m_pItemPet.params[PET_PARAM_ABILITY_LEV]);
    g_sendCmd->WriteLong(m_oId);
    if (m_scene)
        m_scene->BroadcastCmd(m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
}

// add by zzh
void CRegionCreature::ReComputePetBagSize() {
    if (!IsUser() || !m_pItemContainer)
        return;

    m_pItemContainer->m_PetBag.Resize(ITEM_PET_BAG_PAGE, ITEM_PET_BAG_LINE,
                                      3 + m_core.GradeLev / 20);
}

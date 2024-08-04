

#include "core/rt2_core.h"
#include "rt1tort2.h"
//#include "gc_include.h"
#include "skill_manage.h"
//#include "g_cmdpacket.h"
#include "net/rt_net.h"
#include "string_mapping.h"
#include <string>
#include <time.h>

// A [11/9/2008 Louis.Huang]
extern char* g_strStaticBuffer;
//#define SLOG    LOG
//#define SLOG1   LOG1

/*----------------------------------------------------------------------------
- ????.
----------------------------------------------------------------------------*/
static CSkillManage* s_pSkillManage = NULL;

bool SkillInit(const char* szSkillFile, const char* szPassiveFile, const char* szEffectFile, const char* szMutexFile)
{
    CHECK (s_pSkillManage==NULL);
    s_pSkillManage = RT_NEW CSkillManage;
    return s_pSkillManage->Init(szSkillFile, szPassiveFile, szEffectFile, szMutexFile);
}

bool SkillClean()
{
    CHECK(s_pSkillManage!=NULL);
    s_pSkillManage->Clean();
    DEL_ONE(s_pSkillManage);
    return true;
}

DWORD GetSkillTargetValue(DWORD dwTableValue)
{
    switch (dwTableValue)
    {
    case 0:     return SKILL_TARGET_NONE         ; // 无
    case 1:     return SKILL_TARGET_ENEMY_ACTOR  ; // 敌人
    case 2:     return SKILL_TARGET_ENEMY_PET    ; // 敌人的兽
    case 3:     return SKILL_TARGET_PLAYER       ; // 玩家
    case 4:     return SKILL_TARGET_PET          ; // 玩家的兽
    case 5:     return SKILL_TARGET_FRIEND_ACTOR ; // 友军
    case 6:     return SKILL_TARGET_FRIEND_PET   ; // 友军的兽
    case 7:     return SKILL_TARGET_NEUTRAL_ACTOR; // 中立
    case 8:     return SKILL_TARGET_NEUTRAL_PET  ; // 中立的兽
    case 9:     return SKILL_TARGET_CORPSE       ; // 尸体
    case 10:    return SKILL_TARGET_POSITION     ; // 位置
    case 11:    return SKILL_TARGET_ENEMY        ; // 敌人(人和兽)
    case 12:    return SKILL_TARGET_FRIEND       ; // 友军(人和兽)
    case 13:    return SKILL_TARGET_NEUTRAL      ; // 中立(人和兽)
    case 14:    return SKILL_TARGET_ALL_ACTOR    ; // 所有的人
    case 15:    return SKILL_TARGET_ALL_PET      ; // 所有的兽
    case 16:    return SKILL_TARGET_PLAYER|SKILL_TARGET_FRIEND_ACTOR; // 自己和所有的友军
    case 17:    return SKILL_TARGET_TEAM_MEMBER  ; // 队伍成员
    case 18:    return SKILL_TARGET_TEAM_MEMBER|SKILL_TARGET_PLAYER; // 自己和队伍成员
    case 19:    return SKILL_TARGET_HERB; // 药草
    case 20:    return SKILL_TARGET_MINE; // 矿
    case 21:    return SKILL_TARGET_ITEM_BAG; // 道具袋中的道具
    case 22:    return SKILL_TARGET_UNION; // 同一诸侯
    case 23:    return SKILL_TARGET_FACTION; // 同一阵营
    case 24:    return SKILL_TARGET_UNION_x; // 其他诸侯
    case 25:    return SKILL_TARGET_FACTION_x; // 其他阵营
	case 26:    return SKILL_TARGET_MASTER; //对自己的主人
	case 27:    return SKILL_TARGET_ALL;    //对所有人和怪物
	case 28:    return SKILL_TARGET_PETSELF; //宠物自己
	case 29:    return SKILL_TARGET_PETSELF|SKILL_TARGET_MASTER;
    }
    CHECKEX("GetSkillTargetValue 错误的技能目标类型");
    return 0;
}

/*----------------------------------------------------------------------------
- CSkillManage.
----------------------------------------------------------------------------*/

CSkillManage* Skill()
{
    CHECK(s_pSkillManage!=NULL);
    return s_pSkillManage;
}

CSkillManage::CSkillManage()
{
    m_iTotalSkill = 0;
    m_pPoolSkill = NULL;
    m_iTotalEffect = 0;
    m_pPoolEffect = NULL;
}

CSkillManage::~CSkillManage()
{
    Clean();
}


bool CSkillManage::Init(const char* szSkillFile, const char* szPassiveFile, const char* szEffectFile, const char* szMutexFile)
{
    int i, j;
    SSkill* pSkill;
    SSkillEffect* pEffect;
    int iTotalSkill, iTotalPassive, iIdxSkill, iIdxPassive, iIdxEffect, iCntEffect;
#ifdef _DEBUG
    SSkill* pLastLoadSkill = NULL;
#endif

    RtCoreLog().Info("Loading Skill Table [%s].\n", szSkillFile);

    m_mapSkill.clear();
    if (!m_csvSkill.LoadTableFile(szSkillFile))
    {
        RtCoreLog().Info("        Failed [Can't load file].\n");
        return false;
    }

    RtCoreLog().Info("Loading Passive Skill Table [%s].\n", szPassiveFile);
    if (!m_csvPassive.LoadTableFile(szPassiveFile))
    {
        RtCoreLog().Info("        Failed [Can't load file].\n");
        return false;
    }

    RtCoreLog().Info("Loading Skill Effect Table [%s].\n", szEffectFile);
    if (!m_csvEffect.LoadTableFile(szEffectFile))
    {
        RtCoreLog().Info("        Failed [Can't load file].\n");
        return false;
    }

    iIdxSkill = m_csvSkill.FindLineByString("wIdx");
    if (iIdxSkill<0)
    {
        RtCoreLog().Info("        Failed [Can't find 'wIdx' field].\n");
        return false;
    }
    m_csvSkill.SetTitleLine(iIdxSkill);
    iTotalSkill = m_csvSkill.GetLineCount()-iIdxSkill-1;

    iIdxPassive = m_csvPassive.FindLineByString("wIdx");
    if (iIdxPassive<0)
    {
        RtCoreLog().Info("        Failed [Can't find 'wIdx' field].\n");
        return false;
    }
    m_csvPassive.SetTitleLine(iIdxPassive);
    iTotalPassive = m_csvPassive.GetLineCount()-iIdxPassive-1;

    iIdxEffect = m_csvEffect.FindLineByString("wIdx");
    if (iIdxEffect<0)
    {
        RtCoreLog().Info("        Failed [Can't find 'wIdx' field].\n");
        return false;
    }
    m_csvEffect.SetTitleLine(iIdxEffect);
    m_iTotalEffect = m_csvEffect.GetLineCount()-iIdxEffect-1;

    m_iTotalSkill = iTotalSkill + iTotalPassive;

    m_pPoolSkill = RT_NEW SSkill[m_iTotalSkill];
    memset(m_pPoolSkill, 0, sizeof(SSkill)*m_iTotalSkill);
    m_pPoolEffect = RT_NEW SSkillEffect[m_iTotalEffect];
    memset(m_pPoolEffect, 0, sizeof(SSkillEffect)*m_iTotalEffect);

    j = iIdxSkill;
    i = j+iTotalSkill;
    pSkill = m_pPoolSkill;
	string tempStr = "";
    for (j++,iTotalSkill=0; j<=i; j++,pSkill++,iTotalSkill++)
    {
        if (!m_csvSkill[j][0].IsValid())
        {
            break;
        }
        pSkill->wID              = m_csvSkill[j]["wIdx"].Word();                 // 索引
        pSkill->szName           = m_csvSkill[j]["szName"].Str();                // 名字

        pSkill->wSubID           = m_csvSkill[j]["wSubID"].Word();               // 技能类型
        pSkill->dwType           = m_csvSkill[j]["dwType"].Word();               // 技能类型
        pSkill->wClass           = m_csvSkill[j]["wClass"].Word();               // 技能系列
        pSkill->wStrSkillSub     = m_csvSkill[j]["wStrSkillSub"].Word();         // 加强技能的类型ID
		tempStr                  = m_csvSkill[j]["wManyStrSkillSub"].Str();      // 多加强技能的类型ID
        pSkill->iLevel           = m_csvSkill[j]["iLevel"].Int();                // 技能等级
        pSkill->iMaxLevel        = m_csvSkill[j]["iMaxLev"].Int();               // 最高等级
        pSkill->iLevelUpExp      = m_csvSkill[j]["iLevelUpExp"].Int();           // 升级经验
        pSkill->szSimpleDesc     = m_csvSkill[j]["szSimpleDesc"].Str();          // 简单说明
        pSkill->szDescription    = m_csvSkill[j]["szDescription"].Str();         // 详细说明
        pSkill->szIcon           = m_csvSkill[j]["szIcon"].Str();                // 图标

        pSkill->iMP                 = m_csvSkill[j]["iMP"].Int();                   // 消耗MP
		pSkill->iSP                 = m_csvSkill[j]["iSP"].Int();                   // 消耗SkillPoint
		pSkill->iSoul				= m_csvSkill[j]["iSoul"].Int();				// 消耗灵力
        pSkill->filterSee.pNodes    = (SSkillNodeFilter*)m_csvSkill[j]["filterSee"].Str();    // 开始能学习的玩家等级[>=这个等级的玩家就可以看到这个技能是否可以学习]
        pSkill->filterUse.pNodes    = (SSkillNodeFilter*)m_csvSkill[j]["filterUse"].Str();    // 使用条件
        pSkill->filterStudy.pNodes  = (SSkillNodeFilter*)m_csvSkill[j]["filterStudy"].Str();  // 学习条件

        pSkill->dwRTarget        = m_csvSkill[j]["dwRTarget"].DWord();           // 施放目标
        pSkill->iRMinDest        = m_csvSkill[j]["iRMinDest"].Int();             // 施放最小距离
        pSkill->iRMaxDest        = m_csvSkill[j]["iRMaxDest"].Int();             // 施放最大距离
        pSkill->iRTime           = m_csvSkill[j]["iRTime"].Int();                // 施放时间
        pSkill->iRRTime          = m_csvSkill[j]["iRRTime"].Int();               // 施放恢复时间
        pSkill->szRAction1       = m_csvSkill[j]["szRAction1"].Str();            // 施放动作
		pSkill->szRAction2       = m_csvSkill[j]["szRAction2"].Str();            // 施放动作
        pSkill->szREvent         = m_csvSkill[j]["szREvent"].Str();              // 施放触发事件
        pSkill->szREffect1       = m_csvSkill[j]["szREffect1"].Str();            // 施放特效
		pSkill->szREffect2       = m_csvSkill[j]["szREffect2"].Str();            // 施放特效
        pSkill->szREfLink        = m_csvSkill[j]["szREfLink"].Str();             // 施放特效Link
        pSkill->szRSound         = m_csvSkill[j]["szRSound"].Str();              // 施放音效
		pSkill->szRSoundLoop     = m_csvSkill[j]["szRSoundLoop"].Str();          // 施放的循环音效
		pSkill->szWayModel       = m_csvSkill[j]["szWayModel"].Str();            // 武器路径动画
		pSkill->szWayName		 = m_csvSkill[j]["szWayName"].Str();			 // 武器动画名
		pSkill->iAddModel		 = m_csvSkill[j]["iAddModel"].Int();			 // 需增加显示几个武器模型

        pSkill->bFly             = m_csvSkill[j]["bFly"].Byte();                 // 是否飞行
        pSkill->cFlyType         = m_csvSkill[j]["cFlyType"].Byte();             // 飞行方式
        pSkill->wFlySpeed        = m_csvSkill[j]["wFlySpeed"].Word();            // 飞行速度
        pSkill->szFlyEffect      = m_csvSkill[j]["szFlyEffect"].Str();           // 飞行特效

        pSkill->dwATarget        = m_csvSkill[j]["dwATarget"].DWord();           // 攻击目标
        pSkill->bAType           = m_csvSkill[j]["bAType"].Byte();               // 是否范围攻击
        pSkill->cAMaxObj         = m_csvSkill[j]["cAMaxObj"].Byte();             // 最大目标个数
        pSkill->wARange          = m_csvSkill[j]["wARange"].Word();              // 最大作用范围
        pSkill->bABad            = m_csvSkill[j]["bABad"].Word();                // 是否有害
        pSkill->bADamage         = m_csvSkill[j]["bADamage"].Word();             // 伤害计算
		pSkill->DamageMode       = m_csvSkill[j]["DamageMode"].Word();           // 伤害方式
        pSkill->bAlwaysUsable    = m_csvSkill[j]["AlwaysUsable"].Word();         // 
        pSkill->dwAHitRate       = m_csvSkill[j]["dwAHitRate"].DWord();          // 
        pSkill->wHits            = m_csvSkill[j]["iHits"].Word();                // 工作动作的Hit点数

        pSkill->szUEffect        = m_csvSkill[j]["szUEffect"].Str();             // 受击特效
        pSkill->szUEfLink        = m_csvSkill[j]["szUEfLink"].Str();             // 受击特效Link

        //pSkill->pCollision[5]    = m_csvSkill[j]["pCollision"].Str();            // 技能冲突
        pSkill->actSelfTemp.pNodes  = (SSkillActionNode*)m_csvSkill[j]["actSelfTemp"].Str();
        pSkill->actTargetTemp.pNodes= (SSkillActionNode*)m_csvSkill[j]["actTargetTemp"].Str();
        pSkill->actSelfUse.pNodes   = (SSkillActionNode*)m_csvSkill[j]["actSelfUse"].Str(); // 作用效果
        pSkill->actTargetUse.pNodes = (SSkillActionNode*)m_csvSkill[j]["actTargetUse"].Str(); // 作用效果

        pSkill->iStateRate            = m_csvSkill[j]["iStateRate"].Int();
        pSkill->iStateType            = m_csvSkill[j]["iStateType"].Int();
        pSkill->actSelfState.pNodes   = (SSkillActionNode*)m_csvSkill[j]["actSelfState"].Str();
        pSkill->actTargetState.pNodes = (SSkillActionNode*)m_csvSkill[j]["actTargetState"].Str();

        pSkill->dwRTarget = GetSkillTargetValue(pSkill->dwRTarget);
        pSkill->dwATarget = GetSkillTargetValue(pSkill->dwATarget);

		pSkill->iIsIntonate         = m_csvSkill[j]["iIsIntonate"].Int();         // 是否需要吟唱
        pSkill->iIntonateTime       = m_csvSkill[j]["iIntonateTime"].Int();       //吟唱时间
		pSkill->iBreakOdds          = m_csvSkill[j]["iBreakOdds"].Int();          // 打断几率
        pSkill->iBeBreakOdds        = m_csvSkill[j]["iBeBreakOdds"].Int();        //被打断几率

		pSkill->isFlashMove         = m_csvSkill[j]["isFlashMove"].Int();         //是否瞬间移动
		pSkill->szChangModel        = m_csvSkill[j]["szChangeModel"].Str();		  // 人物切换模型(特效)
		pSkill->ConsumeAnger        = m_csvSkill[j]["ConsumeAnger"].Int();		  // 消耗怒气

		pSkill->effectID            = m_csvSkill[j]["effectID"].Int();

		/*pSkill->SkillImpose         = m_csvSkill[j]["SkillImpose"].Int();*/

		pSkill->IsAddHpSkill        = m_csvSkill[j]["IsAddHPSkill"].Int();

		pSkill->ResistState         = m_csvSkill[j]["ResistState"].Int();

		pSkill->iStrStateRate       = m_csvSkill[j]["iStrStateRate"].Int();

        if (pSkill->wID<=0)
		{
			continue;
            RtCoreLog().Info("出现ID小于等于0的技能, ID=%d, Name=%d\n", pSkill->wID, pSkill->szName);
            CHECKEX ("技能ID小于等于0");
        }
        if (m_mapSkill.find(pSkill->wID)!=m_mapSkill.end())
		{
			continue;
            RtCoreLog().Info("技能的ID重复了 ID=%d [%s]和[%s]\n", pSkill->wID, pSkill->szName, m_mapSkill.find(pSkill->wID)->second->szName);
            CHECKEX ("技能ID重复");
        }
        m_mapSkill[pSkill->wID]  = pSkill;
 
 
		//提取加强型技能ID 
		static int Pos = 0;
		size_t nPos = tempStr.find(";");
		if (nPos == string::npos && atoi(tempStr.c_str()) != 0)
		{
			pSkill->wManyStrSkillSub.push_back(atoi(tempStr.c_str()));
		}
		else
		{
			size_t nStart = 0;
			Pos = nPos;
			while (nPos != string::npos)
			{

				// size_t newPos = tempStr.find(";", nPos+1);
				string newTemp = tempStr.substr(nStart, Pos);

				pSkill->wManyStrSkillSub.push_back(atoi(newTemp.c_str()));


				nStart = nPos+1;
				nPos = tempStr.find(";", nStart);

				if (nPos > tempStr.length())
				{
					string newTemp = tempStr.substr(nStart,tempStr.length() - Pos);
					pSkill->wManyStrSkillSub.push_back(atoi(newTemp.c_str()));
				}
				else
				{
					Pos = nPos - Pos +1;
				}	
			}
		}

#ifdef _DEBUG
        pLastLoadSkill = pSkill;
#endif
    }

    j = iIdxPassive;
    i = j+iTotalPassive;
    pSkill = m_pPoolSkill + iTotalSkill;
    for (j++,iTotalPassive=0; j<=i; j++,pSkill++,iTotalPassive++)
    {
        if (!m_csvPassive[j][0].IsValid())
        {
            break;
        }
        pSkill->wID              = m_csvPassive[j]["wIdx"].Word();              // 索引
        pSkill->szName           = m_csvPassive[j]["szName"].Str();             // 名字

        pSkill->wSubID           = m_csvPassive[j]["wSubID"].Word();            // 技能类型
        pSkill->dwType           = SKILL_TYPE_PASSIVE;                          // 技能类型
        pSkill->wClass           = m_csvPassive[j]["wClass"].Word();            // 技能系列
        pSkill->iLevel           = m_csvPassive[j]["iLevel"].Int();             // 技能等级
        pSkill->iMaxLevel        = m_csvPassive[j]["iMaxLev"].Int();            // 最高等级
        pSkill->iLevelUpExp      = 0;//m_csvPassive[j]["iLevelUpExp"].Int();        // 升级经验
        pSkill->szSimpleDesc     = m_csvPassive[j]["szSimpleDesc"].Str();       // 简单说明
        pSkill->szDescription    = m_csvPassive[j]["szDescription"].Str();      // 详细说明
        pSkill->szIcon           = m_csvPassive[j]["szIcon"].Str();             // 图标

        pSkill->iMP              = m_csvPassive[j]["iMP"].Int();                // 消耗MP
		pSkill->iSP              = m_csvPassive[j]["iSP"].Int();                // 消耗SkillPoint
		pSkill->iSoul          = m_csvPassive[j]["iSoul"].Int();            // 消耗灵力
        pSkill->filterSee.pNodes    = (SSkillNodeFilter*)m_csvPassive[j]["filterSee"].Str();    // 开始能学习的玩家等级[>=这个等级的玩家就可以看到这个技能是否可以学习]
        pSkill->filterUse.pNodes    = (SSkillNodeFilter*)m_csvPassive[j]["filterUse"].Str();    // 使用条件
        pSkill->filterStudy.pNodes  = (SSkillNodeFilter*)m_csvPassive[j]["filterStudy"].Str();  // 学习条件

        pSkill->actSelfTemp.pNodes  = (SSkillActionNode*)m_csvPassive[j]["actSelfTemp"].Str();
        pSkill->actTargetTemp.pNodes= (SSkillActionNode*)m_csvPassive[j]["actTargetTemp"].Str();
        pSkill->actTargetUse.pNodes = (SSkillActionNode*)m_csvPassive[j]["actUse"].Str();

        pSkill->actTemp.pNodes      = (SSkillActionNode*)m_csvPassive[j]["actTemp"].Str();
        pSkill->actOpen.pNodes      = (SSkillActionNode*)m_csvPassive[j]["actOpen"].Str();
        pSkill->actClose.pNodes     = (SSkillActionNode*)m_csvPassive[j]["actClose"].Str();

		pSkill->iIsIntonate        =  0;//m_csvSkill[j]["iIsIntonate"].Int();             // 被动技能不需要吟唱

        if (pSkill->wID<=0)
		{
			continue;
            RtCoreLog().Info("出现ID小于等于0的技能, ID=%d, Name=%d\n", pSkill->wID, pSkill->szName);
            CHECKEX ("技能ID小于等于0");
        }
        if (m_mapSkill.find(pSkill->wID)!=m_mapSkill.end())
		{
			continue;
            RtCoreLog().Info("技能的ID重复了 ID=%d [%s]和[%s]\n", pSkill->wID, pSkill->szName, m_mapSkill.find(pSkill->wID)->second->szName);
            CHECKEX ("技能ID重复");
        }
        m_mapSkill[pSkill->wID]  = pSkill;
#ifdef _DEBUG
        pLastLoadSkill = pSkill;
#endif
    }

    i = m_iTotalSkill - (iTotalSkill + iTotalPassive);
    if (i)
    {
        RtCoreLog().Info("        Detected %d skill blank line\n", i);
        m_iTotalSkill = iTotalSkill + iTotalPassive;
    }

    j = iIdxEffect;
    i = j+m_iTotalEffect;
    pEffect = m_pPoolEffect;
    for (j++,iCntEffect=0; j<=i; j++,pEffect++,iCntEffect++)
    {
        if (!m_csvEffect[j][0].IsValid())
        {
            break;
        }
        pEffect->wID             = m_csvEffect[j]["wIdx"].Word();
        pEffect->szName          = m_csvEffect[j]["szName"].Str();
        pEffect->cLevel          = m_csvEffect[j]["cLevel"].Byte();
        pEffect->cClass          = m_csvEffect[j]["cClass"].Int();
        pEffect->szDescription   = m_csvEffect[j]["szDescription"].Str();
        pEffect->szIcon          = m_csvEffect[j]["szIcon"].Str();
        pEffect->filter.pNodes   = (SSkillNodeFilter*)m_csvEffect[j]["filter"].Str();
        pEffect->szEffect        = m_csvEffect[j]["szEffect"].Str();
        pEffect->szEffLink       = m_csvEffect[j]["szEffLink"].Str();
        pEffect->cTarget         = m_csvEffect[j]["cTarget"].Byte();
        pEffect->cMaxObj         = m_csvEffect[j]["cMaxObj"].Byte();
        pEffect->wRange          = m_csvEffect[j]["wRange"].Word();
        pEffect->dwATarget       = m_csvEffect[j]["dwATarget"].DWord();
        pEffect->cAType          = m_csvEffect[j]["cAType"].Byte();
        pEffect->cOverlay        = m_csvEffect[j]["cOverlay"].Byte();
        pEffect->actTemp.pNodes  = (SSkillActionNode*)m_csvEffect[j]["actTemp"].Str();
        pEffect->actOpen.pNodes  = (SSkillActionNode*)m_csvEffect[j]["actOpen"].Str();
        pEffect->actCycle.pNodes = (SSkillActionNode*)m_csvEffect[j]["actCycle"].Str();
        pEffect->actClose.pNodes = (SSkillActionNode*)m_csvEffect[j]["actClose"].Str();
        pEffect->iDTimes         = m_csvEffect[j]["iDTimes"].Int();
        pEffect->iCycle          = m_csvEffect[j]["iCycle"].Int();
		pEffect->isCleared       = m_csvEffect[j]["isCleared"].Int();

        pEffect->dwATarget = GetSkillTargetValue(pEffect->dwATarget);

        if (pEffect->wID<=0)
        {
            RtCoreLog().Info("出现ID小于等于0的效果, ID=%d, Name=%d\n", pEffect->wID, pEffect->szName);
            CHECKEX ("效果ID小于等于0");
        }
        if (m_mapEffect.find(pEffect->wID)!=m_mapEffect.end())
        {
            RtCoreLog().Info("效果的ID重复了 ID=%d [%s]和[%s]\n", pEffect->wID, pEffect->szName, m_mapEffect.find(pEffect->wID)->second->szName);
            CHECKEX ("效果ID重复");
        }
        m_mapEffect[pEffect->wID] = pEffect;
    }

	VOID *p = (void*)&m_mapEffect;
    i = m_iTotalEffect - iCntEffect;
    if (i)
    {
        RtCoreLog().Info("        Detected %d effect blank line\n", i);
        m_iTotalEffect = iCntEffect;
    }

    int iFilterCnt=0, iEffectCnt=0;
    pSkill = m_pPoolSkill;
	
    for (i=0; i<iTotalSkill; i++)
    {
        iFilterCnt += pSkill->filterSee.Analyse((const char*)pSkill->filterSee.pNodes, NULL, 0);
        iFilterCnt += pSkill->filterUse.Analyse((const char*)pSkill->filterUse.pNodes, NULL, 0);
        iFilterCnt += pSkill->filterStudy.Analyse((const char*)pSkill->filterStudy.pNodes, NULL, 0);

        iEffectCnt += pSkill->actSelfTemp.Analyse((const char*)pSkill->actSelfTemp.pNodes, NULL, 0);
        iEffectCnt += pSkill->actTargetTemp.Analyse((const char*)pSkill->actTargetTemp.pNodes, NULL, 0);
        iEffectCnt += pSkill->actSelfUse.Analyse((const char*)pSkill->actSelfUse.pNodes, NULL, 0);
        iEffectCnt += pSkill->actTargetUse.Analyse((const char*)pSkill->actTargetUse.pNodes, NULL, 0);
        iEffectCnt += pSkill->actSelfState.Analyse((const char*)pSkill->actSelfState.pNodes, NULL, 0);
        iEffectCnt += pSkill->actTargetState.Analyse((const char*)pSkill->actTargetState.pNodes, NULL, 0);
        pSkill ++;
    }

    pSkill = m_pPoolSkill + iTotalSkill;
    for (i=0; i<iTotalPassive; i++)
    {
        iFilterCnt += pSkill->filterSee.Analyse((const char*)pSkill->filterSee.pNodes, NULL, 0);
        iFilterCnt += pSkill->filterUse.Analyse((const char*)pSkill->filterUse.pNodes, NULL, 0);
        iFilterCnt += pSkill->filterStudy.Analyse((const char*)pSkill->filterStudy.pNodes, NULL, 0);

        iEffectCnt += pSkill->actSelfTemp.Analyse((const char*)pSkill->actSelfTemp.pNodes, NULL, 0);
        iEffectCnt += pSkill->actTargetTemp.Analyse((const char*)pSkill->actTargetTemp.pNodes, NULL, 0);
        iEffectCnt += pSkill->actTargetUse.Analyse((const char*)pSkill->actTargetUse.pNodes, NULL, 0);

        iEffectCnt += pSkill->actTemp.Analyse((const char*)pSkill->actTemp.pNodes, NULL, 0);
        iEffectCnt += pSkill->actOpen.Analyse((const char*)pSkill->actOpen.pNodes, NULL, 0);
        iEffectCnt += pSkill->actClose.Analyse((const char*)pSkill->actClose.pNodes, NULL, 0);
        pSkill ++;
    }

    pEffect = m_pPoolEffect;
    for (i=0; i<m_iTotalEffect; i++)
    {
        iFilterCnt += pEffect->filter.Analyse((const char*)pEffect->filter.pNodes, NULL, 0);

        iEffectCnt += pEffect->actTemp.Analyse((const char*)pEffect->actTemp.pNodes, NULL, 0);
        iEffectCnt += pEffect->actOpen.Analyse((const char*)pEffect->actOpen.pNodes, NULL, 0);
        iEffectCnt += pEffect->actCycle.Analyse((const char*)pEffect->actCycle.pNodes, NULL, 0);
        iEffectCnt += pEffect->actClose.Analyse((const char*)pEffect->actClose.pNodes, NULL, 0);
        pEffect ++;
    }

    m_pSkillFilterNode = RT_NEW SSkillNodeFilter[iFilterCnt];
    m_pSkillEffectNode = RT_NEW SSkillActionNode[iEffectCnt];

    iFilterCnt=0, iEffectCnt=0;
    pSkill = m_pPoolSkill;
    for (i=0; i<iTotalSkill; i++)
    {
        iFilterCnt += pSkill->filterSee.Analyse((const char*)pSkill->filterSee.pNodes, this, iFilterCnt);             // 使用条件
        iFilterCnt += pSkill->filterUse.Analyse((const char*)pSkill->filterUse.pNodes, this, iFilterCnt);             // 使用条件
        iFilterCnt += pSkill->filterStudy.Analyse((const char*)pSkill->filterStudy.pNodes, this, iFilterCnt);         // 学习条件

        iEffectCnt += pSkill->actSelfTemp.Analyse((const char*)pSkill->actSelfTemp.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actTargetTemp.Analyse((const char*)pSkill->actTargetTemp.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actSelfUse.Analyse((const char*)pSkill->actSelfUse.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actTargetUse.Analyse((const char*)pSkill->actTargetUse.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actSelfState.Analyse((const char*)pSkill->actSelfState.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actTargetState.Analyse((const char*)pSkill->actTargetState.pNodes, this, iEffectCnt);
        pSkill ++;
    }

    pSkill = m_pPoolSkill + iTotalSkill;
    for (i=0; i<iTotalPassive; i++)
    {
        iFilterCnt += pSkill->filterSee.Analyse((const char*)pSkill->filterSee.pNodes, this, iFilterCnt);             // 使用条件
        iFilterCnt += pSkill->filterUse.Analyse((const char*)pSkill->filterUse.pNodes, this, iFilterCnt);             // 使用条件
        iFilterCnt += pSkill->filterStudy.Analyse((const char*)pSkill->filterStudy.pNodes, this, iFilterCnt);         // 学习条件

        iEffectCnt += pSkill->actSelfTemp.Analyse((const char*)pSkill->actSelfTemp.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actTargetTemp.Analyse((const char*)pSkill->actTargetTemp.pNodes, this, iEffectCnt);
        iEffectCnt += pSkill->actTargetUse.Analyse((const char*)pSkill->actTargetUse.pNodes, this, iEffectCnt);

        iEffectCnt += pSkill->actTemp.Analyse((const char*)pSkill->actTemp.pNodes, this, iEffectCnt);                 // 作用效果
        iEffectCnt += pSkill->actOpen.Analyse((const char*)pSkill->actOpen.pNodes, this, iEffectCnt);                 // 作用效果
        iEffectCnt += pSkill->actClose.Analyse((const char*)pSkill->actClose.pNodes, this, iEffectCnt);               // 周期效果

        pSkill ++;
    }

    pEffect = m_pPoolEffect;
    for (i=0; i<m_iTotalEffect; i++)
    {
        iFilterCnt += pEffect->filter.Analyse((const char*)pEffect->filter.pNodes, this, iFilterCnt);

        iEffectCnt += pEffect->actTemp.Analyse((const char*)pEffect->actTemp.pNodes, this, iEffectCnt);
        iEffectCnt += pEffect->actOpen.Analyse((const char*)pEffect->actOpen.pNodes, this, iEffectCnt);
        iEffectCnt += pEffect->actCycle.Analyse((const char*)pEffect->actCycle.pNodes, this, iEffectCnt);
        iEffectCnt += pEffect->actClose.Analyse((const char*)pEffect->actClose.pNodes, this, iEffectCnt);
        pEffect ++;
    }

    RtCoreLog().Info("    Active Skill : %d\n    Passive Skill: %d\n    Skill Effect : %d\n" , iTotalSkill, iTotalPassive, m_iTotalEffect);

    if (!LoadMutex(szMutexFile))
    {
        return false;
    }
    return true;
}

bool CSkillManage::LoadMutex(const char* szMutexFile)
{
    // 互斥表
    int iTotalMutex, iIdxMutex, i, j, k, iCnt;
    RtCsv csvMutex;
    if (!csvMutex.LoadTableFile(szMutexFile))
    {
        RtCoreLog().Info("        Failed [Can't load file %s].\n", szMutexFile);
        return false;
    }

    iIdxMutex = csvMutex.FindLineByString("冲突");
    if (iIdxMutex<0)
    {
        RtCoreLog().Info("        Failed [Can't find '冲突' field].\n");
        return false;
    }
    csvMutex.SetTitleLine(iIdxMutex);
    iTotalMutex = csvMutex.GetLineCount()-iIdxMutex-1;

    unsigned short sType, sID1, sID2;
    unsigned short *pBuf, *pNext, *pCntBuf;
    pCntBuf = RT_NEW unsigned short[m_iTotalEffect*3];
    memset(pCntBuf, 0, sizeof(unsigned short)*m_iTotalEffect*3);

    pBuf = pCntBuf;
    for (i=0; i<m_iTotalEffect; i++)
    {
        m_pPoolEffect[i].mutex.listMutex = pBuf++;
        m_pPoolEffect[i].mutex.listImmune = pBuf++;
        m_pPoolEffect[i].mutex.listDecline = pBuf++;
    }

    iCnt = 0;
    j = iIdxMutex;
    i = j+iTotalMutex;
    for (j++; j<=i; j++)
    {
        if (!csvMutex[j][0].IsValid())
        {
            break;
        }
        sType = csvMutex[j][0].Word();
        sID1  = csvMutex[j][1].Word();
        sID2  = csvMutex[j][2].Word();
        if (sType==1) // 互斥
        {
            for (k=0; k<m_iTotalEffect; k++)
            {
                if (m_pPoolEffect[k].cClass==sID1)
                {
                    m_pPoolEffect[k].mutex.listMutex[0] ++;
                    iCnt ++;
                }
                if (m_pPoolEffect[k].cClass==sID2)
                {
                    m_pPoolEffect[k].mutex.listMutex[0] ++;
                    iCnt ++;
                }
            }
        }else if (sType==2) // 免疫
        {
            for (k=0; k<m_iTotalEffect; k++)
            {
                if (m_pPoolEffect[k].cClass==sID1)
                {
                    m_pPoolEffect[k].mutex.listImmune[0] ++;
                    iCnt ++;
                }
                if (m_pPoolEffect[k].cClass==sID2)
                {
                    m_pPoolEffect[k].mutex.listDecline[0] ++;
                    iCnt ++;
                }
            }
        }else
        {
            CHECKEX(szMutexFile);
        }
    }

    int iTotalPool = iCnt+m_iTotalEffect*3;
    m_pPoolMutex = RT_NEW unsigned short[iTotalPool];
    memset(m_pPoolMutex, 0, sizeof(unsigned short)*(iTotalPool));
    pBuf = m_pPoolMutex;
    for (i=0; i<m_iTotalEffect; i++)
    {
        pNext = pBuf + m_pPoolEffect[i].mutex.listMutex[0]+1;
        CHECK(pNext<=m_pPoolMutex+iTotalPool);
        m_pPoolEffect[i].mutex.listMutex = pBuf;
        pBuf = pNext;

        pNext = pBuf + m_pPoolEffect[i].mutex.listImmune[0]+1;
        CHECK(pNext<=m_pPoolMutex+iTotalPool);
        m_pPoolEffect[i].mutex.listImmune = pBuf;
        pBuf = pNext;

        pNext = pBuf + m_pPoolEffect[i].mutex.listDecline[0]+1;
        CHECK(pNext<=m_pPoolMutex+iTotalPool);
        m_pPoolEffect[i].mutex.listDecline = pBuf;
        pBuf = pNext;
    }
    DEL_ARRAY(pCntBuf);

    j = iIdxMutex;
    i = j+iTotalMutex;
    for (j++; j<=i; j++)
    {
        if (!csvMutex[j][0].IsValid())
        {
            break;
        }
        sType = csvMutex[j][0].Word();
        sID1  = csvMutex[j][1].Word();
        sID2  = csvMutex[j][2].Word();
        if (sType==1) // 互斥
        {
            for (k=0; k<m_iTotalEffect; k++)
            {
                if (m_pPoolEffect[k].cClass==sID1)
                {
                    pBuf = m_pPoolEffect[k].mutex.listMutex;
                    while (*pBuf) pBuf ++;
                    *pBuf = sID2;
                }
                if (m_pPoolEffect[k].cClass==sID2)
                {
                    pBuf = m_pPoolEffect[k].mutex.listMutex;
                    while (*pBuf) pBuf ++;
                    *pBuf = sID1;
                }
            }
        }else if (sType==2) // 免疫
        {
            for (k=0; k<m_iTotalEffect; k++)
            {
                if (m_pPoolEffect[k].cClass==sID1)
                {
                    pBuf = m_pPoolEffect[k].mutex.listImmune;
                    while (*pBuf) pBuf ++;
                    *pBuf = sID2;
                }
                if (m_pPoolEffect[k].cClass==sID2)
                {
                    pBuf = m_pPoolEffect[k].mutex.listDecline;
                    while (*pBuf) pBuf ++;
                    *pBuf = sID1;
                }
            }
        }
    }
    return true;
}

bool CSkillManage::Clean()
{
    if (m_pPoolSkill)
    {
        DEL_ARRAY(m_pPoolMutex);
        DEL_ARRAY(m_pPoolSkill);
        DEL_ARRAY(m_pPoolEffect);
        DEL_ARRAY(m_pSkillFilterNode);
        DEL_ARRAY(m_pSkillEffectNode);
    }
    m_iTotalSkill = 0;
    return true;
}

SSkill* CSkillManage::FindSkill(unsigned short wID)
{
    EXT_SPACE::unordered_map<unsigned short, SSkill*>::iterator it = m_mapSkill.find(wID);
    if (it!=m_mapSkill.end())
    {
        return (*it).second;
    }
    return NULL;
}

SSkill* CSkillManage::FindLastLevelSkill(unsigned short wSkillID)
{
	SSkill* pSkill = FindSkill(wSkillID);
	if (pSkill==NULL) return NULL;
	if (pSkill->iLevel>pSkill->iMaxLevel || pSkill->iLevel <= 1) return NULL;

	EXT_SPACE::unordered_map<unsigned short, SSkill*>::iterator it;
	for (it=m_mapSkill.begin(); it!=m_mapSkill.end(); it++)
	{
		if (it->second->wSubID==pSkill->wSubID)
		{
			if (it->second->iLevel==pSkill->iLevel-1)
			{
				return it->second;
			}
		}
	}
	return NULL;
}

SSkill* CSkillManage::FindNextLevelSkill(unsigned short wSkillID)
{
    SSkill* pSkill = FindSkill(wSkillID);
    if (pSkill==NULL) return NULL;
    if (pSkill->iLevel>=pSkill->iMaxLevel) return NULL;

    EXT_SPACE::unordered_map<unsigned short, SSkill*>::iterator it;
    for (it=m_mapSkill.begin(); it!=m_mapSkill.end(); it++)
    {
        if (it->second->wSubID==pSkill->wSubID)
        {
            if (it->second->iLevel==pSkill->iLevel+1)
            {
                return it->second;
            }
        }
    }
    return NULL;
}

const char* CSkillManage::FindSkillClass(unsigned short wClassID)
{
    switch (wClassID)
    {
    case 1:     return R(SMSG_CLASS_NAME_1);
    case 2:     return R(SMSG_CLASS_NAME_2);
    case 3:     return R(SMSG_CLASS_NAME_3);
    case 4:     return R(SMSG_CLASS_NAME_4);
    case 5:     return R(SMSG_CLASS_NAME_5);
    case 6:     return R(SMSG_CLASS_NAME_6);
    case 7:     return R(SMSG_CLASS_NAME_7);
    case 8:     return R(SMSG_CLASS_NAME_8);
    case 9:     return R(SMSG_CLASS_NAME_9);
    case 10:    return R(SMSG_CLASS_NAME_10);
    case 11:    return R(SMSG_CLASS_NAME_11);
    case 12:    return R(SMSG_CLASS_NAME_12);
    case 13:    return R(SMSG_CLASS_NAME_13);
    default:    return "";
    }
}

SSkillEffect* CSkillManage::FindEffect(unsigned short wID)
{
    EXT_SPACE::unordered_map<unsigned short, SSkillEffect*>::iterator it = m_mapEffect.find(wID);
    if (it!=m_mapEffect.end())
    {
        return (*it).second;
    }
    return NULL;
}

SSkill* CSkillManage::FirstSkill()
{
    m_itSkill = m_mapSkill.begin();
    if (m_itSkill==m_mapSkill.end())
    {
        return NULL;
    }
    return (*m_itSkill).second;
}

SSkill* CSkillManage::NextSkill()
{
    m_itSkill ++;
    if (m_itSkill==m_mapSkill.end())
    {
        return NULL;
    }
    return (*m_itSkill).second;
}

SSkillEffect* CSkillManage::FirstEffect()
{
    m_itEffect = m_mapEffect.begin();
    if (m_itEffect==m_mapEffect.end())
    {
        return NULL;
    }
    return (*m_itEffect).second;
}

SSkillEffect* CSkillManage::NextEffect()
{
    m_itEffect ++;
    if (m_itEffect==m_mapEffect.end())
    {
        return NULL;
    }
    return (*m_itEffect).second;
}

/*----------------------------------------------------------------------------
- CActorSkill.
----------------------------------------------------------------------------*/

static char const* s_pCannotFindSkillMsg = "CActorSkill: %s, 技能[ID=%d]不在人物的技能列表中.\n";

CActorSkill::CActorSkill()
{
    m_sActiveSkillID = 0;
    m_pLastErrorInfo = NULL;
    m_sLastDamage = 0;
    m_sLastDamageOffset = 0;
    m_dwPassiveUID = 1;
    int i;
    for (i=0; i<SKILL_MAX_ELEMENT_ATTR; i++)
    {
        m_sLastElmDamage[i] = 0;
    }
    m_bRun = false;

	m_SchoolPublicCDTime = 0;
	m_WapPublicCDTime = 0;
	m_PropPublicCDTime = 0;
	m_beastPublicCDTime = 0;

	ImmunoUseful = false;  //是否免疫有益BUF
	ImmunoHarmful = false; //是否免疫有害BUF
	ImmunoAll = false;    //是否免疫多有BUF

}

CActorSkill::~CActorSkill()
{
    ClearSkill(false);
    ClearEffect(false);
}

// 技能存盘字符串的格式
// 变量说明
//     v          版本号
//     a          当前激活的技能ID
//     sn         技能的个数
//     pn         被动技能的个数
//     en         效果的个数
//     state      技能的状态
//     runtime    当前技能已经运行了多少时间
//     resumetime 技能恢复时间，还差多少时间恢复，0表示已经恢复
// 版本1 (v==1)
//     v;a;sn;{id:state runtime resumetime;}en;{id:state runtime resumetime;}-1
//     例子(1个技能没有效果) 1;1;1;7:0 0 0;0;-1
// 版本3 (v==3)
//     v a sn {id resumetime} pn {id state uid}
// 版本4 (v==4)
//     v a sn {id resumetime exp} pn {id state uid}

const char* CActorSkill::SaveToString()
{
    BYTE* pData = (BYTE*)g_strStaticBuffer;
    BYTE cSize;
    BYTE iVersion = 4;
    WORD wResumeTime;
    //WORD wExp;

    memcpy(pData, &iVersion, sizeof(BYTE)); pData += sizeof(BYTE);              // Version
    memcpy(pData, &m_sActiveSkillID, sizeof(short)); pData += sizeof(short);    // Active Skill

    // 普通技能
    cSize = (BYTE)m_listSkill.size();
    memcpy(pData, &cSize, sizeof(BYTE)); pData += sizeof(BYTE); // Skill Count
    stlforeach(std::list<CSkillInstance*>, itSkill, m_listSkill)
    {
        CSkillInstance* pSkill = (*itSkill);
        memcpy(pData, &(pSkill->m_pAttr->wID), sizeof(WORD)); pData += sizeof(WORD);
        wResumeTime = (pSkill->m_dwResumeTime)/1000;
		if (wResumeTime > 1800)
		{
			wResumeTime =0 ;
		} 
		/*memcpy(pData, &(pSkill->m_dwResumeTime), sizeof(unsigned long)); pData += sizeof(unsigned long);*/
        memcpy(pData, &wResumeTime, sizeof(unsigned short)); pData += sizeof(WORD);
        memcpy(pData, &(pSkill->m_wExp), sizeof(unsigned short)); pData += sizeof(WORD);
    }

    // 被动技能
    cSize = (BYTE)m_listPassive.size();
    memcpy(pData, &cSize, sizeof(BYTE)); pData += sizeof(BYTE); // Passive Count
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        SActorPassive* pPassive = &(*itPassive);
        memcpy(pData, &(pPassive->pAttr->wID), sizeof(WORD)); pData += sizeof(WORD);
        memcpy(pData, &(pPassive->bActive), sizeof(bool)); pData += sizeof(bool);
        memcpy(pData, &(pPassive->dwUID), sizeof(DWORD)); pData += sizeof(DWORD);
    }

    // 转成16进制数据
    int i, iLen = pData-(BYTE*)g_strStaticBuffer;
    for (i=0; i<iLen; i++)
    {
        cSize = g_strStaticBuffer[i]&0x0F;
        pData[i*2+0] = cSize<10 ? ('0'+cSize) : ('A'+cSize-10);
        cSize = (g_strStaticBuffer[i]>>4)&0x0F;
        pData[i*2+1] = cSize<10 ? ('0'+cSize) : ('A'+cSize-10);
    }
    pData[i*2] = 0;

    // Result
    return g_strStaticBuffer+iLen;
}

void CActorSkill::LoadFromString(const char* szString,long UpdateTime)
{
    BYTE* pData = (BYTE*)g_strStaticBuffer;

    int j, iLen = (int)strlen(szString)/2;

    for (j=0; j<iLen; j++)
    {
        *pData = ((szString[j*2]>='A')?(szString[j*2]+10-'A'):(szString[j*2]-'0'))
            | (((szString[j*2+1]>='A')?(szString[j*2+1]+10-'A'):(szString[j*2+1]-'0'))<<4);
        pData ++;
    }
    pData = (BYTE*)g_strStaticBuffer;

    WORD wID;
    BYTE i, cSize;
    BYTE iVersion;
    WORD wResumeTime;
    WORD wExp;
    SSkill *pAttr;
    memcpy(&iVersion, pData, sizeof(BYTE)); pData += sizeof(BYTE);              // Version
    if (iVersion!=3 && iVersion!=4) return;

    ClearSkill(false);
    ClearEffect(false);

    memcpy(&m_sActiveSkillID, pData, sizeof(short)); pData += sizeof(short);    // Active Skill

    memcpy(&cSize, pData, sizeof(BYTE)); pData += sizeof(BYTE);                 // Skill Count

	long vTime = (long)time(NULL);
    for (i=0; i<cSize; i++)
    {
        memcpy(&wID, pData, sizeof(WORD)); pData += sizeof(WORD);
        pAttr = Skill()->FindSkill(wID);
        if (pAttr && pAttr->dwType!=SKILL_TYPE_PASSIVE)
        {
            AddSkill(wID, 0, false);
            CSkillInstance* pSkill = FindActiveSkill(wID);
            if (pSkill==NULL)
            {
                RtCoreLog().Info("CActorSkill::LoadFromString 读取技能错误.\n");
                return;
            }
            if (iVersion==3)
            {
                memcpy(&pSkill->m_dwResumeTime, pData, sizeof(unsigned long)); pData += sizeof(unsigned long);
            }else if (iVersion==4)
            {
                memcpy(&wResumeTime, pData, sizeof(unsigned short)); pData += sizeof(WORD);
                memcpy(&wExp, pData, sizeof(unsigned short)); pData += sizeof(WORD);
				long vResumeTime = 0;

				if ((wResumeTime * 1000) < ((vTime - UpdateTime) * 1000))
					vResumeTime = 0;
				else if ((vTime - UpdateTime) < 0) //保护措施
					vResumeTime = 0;
				else if ((vTime - UpdateTime) > 1800)
					vResumeTime = 0;
				else
					vResumeTime = (wResumeTime * 1000) - ((vTime - UpdateTime) * 1000);

                pSkill->m_dwResumeTime = vResumeTime;
                pSkill->m_wExp = wExp;
            }

			//OnResumeTimeModify(wID,wResumeTime);
        }else
        {
            RtCoreLog().Info("CActorSkill::LoadFromString 没有找到ID=%d的技能，添加失败.\n", wID);
            if (iVersion==3)
            {
                pData += sizeof(unsigned long);
            }else if (iVersion==4)
            {
                pData += sizeof(WORD);
                pData += sizeof(WORD);
            }
        }
    }

    memcpy(&cSize, pData, sizeof(BYTE)); pData += sizeof(BYTE);                 // Passive Count
    for (i=0; i<cSize; i++)
    {
        memcpy(&wID, pData, sizeof(WORD)); pData += sizeof(WORD);
        pAttr = Skill()->FindSkill(wID);
        if (pAttr && pAttr->dwType==SKILL_TYPE_PASSIVE)
        {
            AddSkill(wID, 0, false);
            SActorPassive* pPassive = FindPassiveSkill(wID);
            if (pPassive==NULL)
            {
                RtCoreLog().Info("CActorSkill::LoadFromString 读取技能错误.\n");
                return;
            }
            memcpy(&(pPassive->bActive), pData, sizeof(bool)); pData += sizeof(bool);
            memcpy(&(pPassive->dwUID), pData, sizeof(DWORD)); pData += sizeof(DWORD);
            pPassive->bActive = false;
            pPassive->dwUID = -1;
        }else
        {
            RtCoreLog().Info("CActorSkill::LoadFromString 没有找到ID=%d的技能，添加失败.\n", wID);
            pData += sizeof(bool);
            pData += sizeof(DWORD);
        }
    }

    OnSkillModify();
    OnEffectModify();
}

//Tianh   保存BUF
const char* CActorSkill::SaveEffectToString()
{
    BYTE* pData = (BYTE*)g_strStaticBuffer;

    BYTE iVersion = 3;
    memcpy(pData, &iVersion, sizeof(BYTE)); pData += sizeof(BYTE);              // Version

    BYTE cSize = 0;
    SActorEffect* pEffect;
    std::list<SActorEffect>::iterator itEffect;
    for (itEffect=m_listEffect.begin(); itEffect!=m_listEffect.end(); itEffect++)
    {
		
        pEffect = &(*itEffect);
        if (pEffect->dwParentUID==0)
        {
			//if (!(pEffect->pAttr->isCleared))
			//{
				cSize ++;
			//}
            
        }
    }
	memcpy(pData, &cSize, sizeof(BYTE)); pData += sizeof(BYTE);                 // Effect Count
	for (itEffect=m_listEffect.begin(); itEffect!=m_listEffect.end(); itEffect++)
	{
		pEffect = &(*itEffect);
		if (pEffect->dwParentUID==0)
		{
			//if (!(pEffect->pAttr->isCleared))
			//{
				memcpy(pData, &(pEffect->pAttr->wID), sizeof(WORD)); pData += sizeof(WORD);
				memcpy(pData, &(pEffect->dwRunTime), sizeof(DWORD)); pData += sizeof(DWORD);
				memcpy(pData, &(pEffect->dwParentUID), sizeof(DWORD)); pData += sizeof(DWORD);
			//}
		}
	}

    int i, iLen = pData-(BYTE*)g_strStaticBuffer;

    for (i=0; i<iLen; i++)
    {
        cSize = g_strStaticBuffer[i]&0x0F;
        pData[i*2+0] = cSize<10 ? ('0'+cSize) : ('A'+cSize-10);
        cSize = (g_strStaticBuffer[i]>>4)&0x0F;
        pData[i*2+1] = cSize<10 ? ('0'+cSize) : ('A'+cSize-10);
    }
    pData[i*2] = 0;

    /*RtCoreLog().Info("Save Skill Data Size=%d [%s]\n", iLen, g_strStaticBuffer+iLen);*/

    return g_strStaticBuffer+iLen;
}

void CActorSkill::LoadEffectFromString(const char* szString)
{
    BYTE* pData = (BYTE*)g_strStaticBuffer;

    int j, iLen = (int)strlen(szString)/2;

    for (j=0; j<iLen; j++)
    {
        *pData = ((szString[j*2]>='A')?(szString[j*2]+10-'A'):(szString[j*2]-'0'))
            | (((szString[j*2+1]>='A')?(szString[j*2+1]+10-'A'):(szString[j*2+1]-'0'))<<4);
        pData ++;
    }
    pData = (BYTE*)g_strStaticBuffer;

    WORD wID;
    BYTE i, cSize;
    BYTE iVersion;
    memcpy(&iVersion, pData, sizeof(BYTE)); pData += sizeof(BYTE);              // Version
    if (iVersion!=3) return;

    memcpy(&cSize, pData, sizeof(BYTE)); pData += sizeof(BYTE);                 // Effect Count
    for (i=0; i<cSize; i++)
    {
        memcpy(&wID, pData, sizeof(WORD)); pData += sizeof(WORD);
        SActorEffect* pEffect = AddEffect(wID, 0, false);
        if (pEffect)
        {
            memcpy(&(pEffect->dwRunTime), pData, sizeof(DWORD)); pData += sizeof(DWORD);
            memcpy(&(pEffect->dwParentUID), pData, sizeof(DWORD)); pData += sizeof(DWORD);
        }else
        {
            RtCoreLog().Info("CActorSkill::LoadFromString 没有找到ID=%d的效果，添加失败.\n", wID);
            pData += sizeof(DWORD);
            pData += sizeof(DWORD);
        }
    }

    OnEffectModify();
}

bool CActorSkill::IsTargetSkill(unsigned short wID)
{
    SSkill* pSkill = Skill()->FindSkill(wID);
    if (pSkill)
    {
        return ((pSkill->dwRTarget&(~SKILL_TARGET_POSITION))!=0);
    }
    return false;
}

bool CActorSkill::AddSkill(unsigned short wID, unsigned short wExp, bool bRefresh)
{
    if (FindAllSkill(wID))
    {
        RtCoreLog().Info("CActorSkill: 添加技能错误, 技能[%d]已经存在\n", wID);
        return false;
    }
    SSkill* pAttr = Skill()->FindSkill(wID);
    if (pAttr==NULL)
    {
        RtCoreLog().Info("CActorSkill: 生成技能错误, 技能[ID=%d]不存在\n", wID);
        return false;
    }
    if (pAttr->dwType==SKILL_TYPE_PASSIVE)
    {
        if ((int)m_listPassive.size()>=g_iActorMaxPassive)
        {
            RtCoreLog().Info("CActorSkill: 添加技能错误, 被动技能已经达到最大值[%d]\n", g_iActorMaxPassive);
            return false;
        }
        m_listPassive.push_back(SActorPassive());
        SActorPassive* pPassive = &(m_listPassive.back());
		//PlayerFailOperate(R(MSG_SKILL_LEARNNEWSKILL));
        pPassive->dwUID = m_dwPassiveUID ++;
        pPassive->bActive = false;
        pPassive->pAttr = pAttr;
        if (m_bRun)
        {
            if ((!pPassive->bActive) && pPassive->pAttr->filterUse.IsSuccess(this)==NULL)
            {
                OnPassiveOpen(pPassive);
                pPassive->bActive = true;
            }
        }
    }else
    {
        if ((int)m_listSkill.size()>=g_iActorMaxSkill)
        {
            RtCoreLog().Info("CActorSkill: 添加技能错误, 主动技能已经达到最大值[%d]\n", g_iActorMaxSkill);
            return false;
        }
        CSkillInstance* pSkill = CSkillInstance::GenerateOne(pAttr);
        m_listSkill.push_back(pSkill);
		//PlayerFailOperate(R(MSG_SKILL_LEARNNEWSKILL));
        pSkill->m_wExp = wExp;
	}

	std::list<SSkillContinueMask>::iterator it;
	for (it=m_listMask.begin();it!=m_listMask.end();++it)
	{
		if ((*it).wSkillClass == pAttr->wClass)
		{
			CSkillInstance *pSkill = FindActiveSkill(pAttr->wID);
			pSkill->ContinueMask((*it).resumeTime,(*it).useTime);
			it = m_listMask.erase(it);
			break;
		}
	}

    TidyUp();//整理技能

    if (bRefresh)
    {
        OnSkillModify();//更新到客户端
    }
    return true;
}

int CActorSkill::GetSkillwSubID(int ID)
{
	unsigned short wSubID = 0;
	stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
	{
		if ((*it)->m_pAttr->wID==ID)
		{
			wSubID = ((*it)->m_pAttr->wSubID);
			return wSubID;
		}
	}
	return 0;
}

//Tianh 修改删除技能  根据ID找到wSubID  删除
//因为技能升级后ID会变
bool CActorSkill::DeleteSkill(unsigned short wID, bool bRefresh)
{
    // 主动技能
	unsigned short wSubID = 0;
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if ((*it)->m_pAttr->wID==wID)
        {
			wSubID = ((*it)->m_pAttr->wSubID);
        }
    }

	stlforeach(std::list<CSkillInstance*>, itt, m_listSkill)
	{
		if ((*itt)->m_pAttr->wSubID == wSubID)
		{
			SSkillContinueMask scm;
			scm.wSkillClass = (*itt)->m_pAttr->wClass;
			scm.resumeTime = (*itt)->m_dwResumeTime;
			scm.useTime = (*itt)->m_dwUserTime;

			if (!((*itt)->m_pAttr->wClass  == 0))
			{
				m_listMask.push_back(scm);
			}	

			DEL_ONE(*itt);
			m_listSkill.erase(itt);
			if (bRefresh) OnSkillModify();

			//<add by Tianh>
	
			return true; 
		}
	}

	unsigned short itPassivewSubID = 0;
    // 被动技能
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if ((*itPassive).pAttr->wID==wID)
        {
			itPassivewSubID = (*itPassive).pAttr->wSubID;
        }
    }

	stlforeach(std::list<SActorPassive>, ittPassive, m_listPassive)
	{
		if ((*ittPassive).pAttr->wSubID==itPassivewSubID)
		{
			if ((*ittPassive).bActive)
			{
				OnPassiveClose(&(*ittPassive));
				(*ittPassive).bActive = false;
			}
			DeleteUIDEffect((*ittPassive).dwUID, bRefresh);
			m_listPassive.erase(ittPassive);
			if (bRefresh) OnSkillModify();
			return true;
		}
	}
    RtCoreLog().Info(s_pCannotFindSkillMsg, "删除技能错误", wID);
    return false;
}

void CActorSkill::ClearSkill(bool bRefresh)
{
    int iRemoveSP = 0;
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        iRemoveSP += (*it)->m_pAttr->iSP * (*it)->m_pAttr->iLevel;
        DEL_ONE(*it);
    }
    m_listSkill.clear();

    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        iRemoveSP += (*itPassive).pAttr->iSP * (*itPassive).pAttr->iLevel;
        if ((*itPassive).bActive)
        {
            OnPassiveClose(&(*itPassive));
            (*itPassive).bActive = false;
        }
        DeleteUIDEffect((*itPassive).dwUID, bRefresh);
    }
    m_listPassive.clear();

    if (bRefresh)
    {
        OnSkillModify();
    }
}

void CActorSkill::ClearFightSkill(bool bRefresh) // 洗战斗点
{
	int iRemoveSP = 0;
	std::list<CSkillInstance*>::iterator itSkill;
	for (itSkill=m_listSkill.begin(); itSkill!=m_listSkill.end();)
	{
		unsigned short wClass = (*itSkill)->m_pAttr->wClass;
		if (wClass > 0 && 13 != wClass && 15 != wClass && 17 != wClass && 18 != wClass && 19 != wClass)
		{
			iRemoveSP += (*itSkill)->m_pAttr->iSP * (*itSkill)->m_pAttr->iLevel;
			DEL_ONE(*itSkill);
			itSkill = m_listSkill.erase(itSkill);
		}else
		{
			itSkill ++;
		}
	}

	std::list<SActorPassive>::iterator itPassive;
	for (itPassive=m_listPassive.begin(); itPassive!=m_listPassive.end();)
	{
		if ((*itPassive).pAttr->wClass>0)
		{
			iRemoveSP += (*itPassive).pAttr->iSP * (*itPassive).pAttr->iLevel;
			if ((*itPassive).bActive)
			{
				OnPassiveClose(&(*itPassive));
				(*itPassive).bActive = false;
			}
			DeleteUIDEffect((*itPassive).dwUID, bRefresh);
			itPassive = m_listPassive.erase(itPassive);
		}else
		{
			itPassive ++;
		}
	}

	OnClearSkill(true, iRemoveSP);

	if (bRefresh)
	{
		OnSkillModify();
		OnEffectModify();
	}
}

void CActorSkill::ClearLifeSkill(bool bRefresh) // 洗生活点
{
    int iRemoveSP = 0;
    std::list<CSkillInstance*>::iterator itSkill;
    for (itSkill=m_listSkill.begin(); itSkill!=m_listSkill.end();)
    {
        if ((*itSkill)->m_pAttr->wClass<=0)
        {
            iRemoveSP += (*itSkill)->m_pAttr->iSP * (*itSkill)->m_pAttr->iLevel;
            DEL_ONE(*itSkill);
            itSkill = m_listSkill.erase(itSkill);
        }else
        {
            itSkill ++;
        }
    }

    std::list<SActorPassive>::iterator itPassive;
    for (itPassive=m_listPassive.begin(); itPassive!=m_listPassive.end();)
    {
        if ((*itPassive).pAttr->wClass<=0)
        {
            iRemoveSP += (*itPassive).pAttr->iSP * (*itPassive).pAttr->iLevel;
            if ((*itPassive).bActive)
            {
                OnPassiveClose(&(*itPassive));
                (*itPassive).bActive = false;
            }
            DeleteUIDEffect((*itPassive).dwUID, bRefresh);
            itPassive = m_listPassive.erase(itPassive);
        }else
        {
            itPassive ++;
        }
    }

    // OnClearSkill(false, iRemoveSP);

    if (bRefresh)
    {
        OnSkillModify();
        OnEffectModify();
    }
}

CSkillInstance* CActorSkill::FindActiveSkill(unsigned short wID)
{
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if ((*it)->m_pAttr->wID==wID)
        {
            return *it;
        }
    }
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::FindPassiveSkill(unsigned short wID)
{
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if ((*itPassive).pAttr->wID==wID)
        {
            return &(*itPassive);
        }
    }
    return NULL;
}

bool CActorSkill::FindPassive(unsigned short wID)
{
	stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
	{
		if ((*itPassive).pAttr->wID==wID)
		{
			return true;
		}
	}
	return false;
}

short CActorSkill::FindEffectbyClass(int cClass)
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->cClass==cClass)
		{
            return  ((*it).pAttr->wID);
		}
		it ++;
	}
	return 0;
}

bool CActorSkill::FindEffectbyId(int wID)
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->wID==wID)
		{
			return  true;
		}
		it ++;
	}
	return false;
}

bool CActorSkill::FindAllSkill(unsigned short wID)
{
    if (FindActiveSkill(wID))
    {
        return true;
    }
    return FindPassiveSkill(wID)!=NULL;
}

SSkill* CActorSkill::FindSkillByName(const char* szSkillName)
{
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if (strcmp((*it)->m_pAttr->szName, szSkillName)==0)
        {
            return (*it)->m_pAttr;
        }
    }
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if (strcmp((*itPassive).pAttr->szName, szSkillName)==0)
        {
            return (*itPassive).pAttr;
        }
    }
    return NULL;
}

SSkill* CActorSkill::FindSkillBySubID(WORD wSubID)
{
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if ((*it)->m_pAttr->wSubID==wSubID)
        {
            return (*it)->m_pAttr;
        }
    }
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if ((*itPassive).pAttr->wSubID==wSubID)
        {
            return (*itPassive).pAttr;
        }
    }
    return NULL;
}

CSkillInstance* CActorSkill::FindActiveSkillByName(const char* szSkillName)
{
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if (strcmp((*it)->m_pAttr->szName, szSkillName)==0)
        {
            return (*it);
        }
    }
    return NULL;
}

short CActorSkill::HaveCollectSkill()
{
	stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
	{
		if ((*it)->m_pAttr->wSubID==SKILL_DIG_ORZ_SUBID  ||
			(*it)->m_pAttr->wSubID==SKILL_DIG_DRUG_SUBID ||
			(*it)->m_pAttr->wSubID==SKILL_DIG_SKIN_SUBID)
		{
			return (*it)->m_pAttr->wSubID;
		}
	}
	return 0;
}

short CActorSkill::HaveProtectSkill()
{
	stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
	{
		if ((*it)->m_pAttr->wSubID==SKILL_MAKE_WEAPON_SUBID  ||
			(*it)->m_pAttr->wSubID==SKILL_MAKE_EQUIP_SUBID	 ||
			(*it)->m_pAttr->wSubID==SKILL_MAKE_LEECHDOM_SUBID)
		{
			return (*it)->m_pAttr->wSubID;
		}
	}
	return 0;
}

CSkillInstance* CActorSkill::FindActiveSkillBySubID(WORD wSubID)
{
    stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
    {
        if ((*it)->m_pAttr->wSubID==wSubID)
        {
            return (*it);
        }
    }
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::FindPassiveSkillByName(const char* szSkillName)
{
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if (strcmp((*itPassive).pAttr->szName, szSkillName)==0)
        {
            return &(*itPassive);
        }
    }
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::FindPassiveSkillBySubID(WORD wSubID)
{
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if ((*itPassive).pAttr->wSubID==wSubID)
        {
            return &(*itPassive);
        }
    }
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::FindStrengthenSkill(WORD wSkillID)
{
    CSkillInstance* pActiveSkill = FindActiveSkill(wSkillID);
    if (pActiveSkill && pActiveSkill->m_pAttr->wStrSkillSub)
    {
        return FindPassiveSkillBySubID(pActiveSkill->m_pAttr->wStrSkillSub);
    }
    return NULL;
}

CActorSkill::SActorEffect* CActorSkill::AddEffect(unsigned short wID, DWORD dwUID, bool bRefresh)
{
    if ((int)m_listEffect.size()>=g_iActorMaxEffect)
    {
        RtCoreLog().Info("CActorSkill: 添加效果错误, 效果已经达到最大值[%d]\n", g_iActorMaxEffect);
        return NULL;
    }
    SActorEffect* pEffect;
    SSkillEffect* pEffectAttr = Skill()->FindEffect(wID);

	if (!pEffectAttr)
	{
		RtCoreLog().Info("添加效果错误，未找到此效果: 不存在的效果ID:[%d]\n", wID);
		return NULL;
	}

    if (pEffectAttr)
    {
		if (pEffectAttr->cAType == 2)  //免疫有害
		{
			if(ImmunoAll || ImmunoHarmful)
			{
				return NULL;
			}
		}
		else if (pEffectAttr->cAType == 1)  //免疫有益
		{
			if(ImmunoAll || ImmunoUseful)
			{
				return NULL;
			}
		}

        if (pEffectAttr->filter.IsSuccess(this)==NULL) // 不能打开就不要添加
        {
            // 互斥检查

            // 如果有相同类型，但是不同等级的效果，就根据等级覆盖
            pEffect = FindEffectByClassID(pEffectAttr->cClass);
            if (pEffect)
            {
                if (pEffect->pAttr->cLevel>pEffectAttr->cLevel)
                {
                    return NULL; // 当前效果等级已经比较高，不需要再添加效果
                }else if (pEffect->pAttr->cLevel==pEffectAttr->cLevel)
                {
                    if (pEffectAttr->cOverlay)
                    {
                        DeleteEffect(pEffect->pAttr->wID, false); // 删除原来效果
                    }else
                    {
                        return NULL; // 已经有相同等级的效果，并且设置为不覆盖，不需要再添加效果
                    }
                }else
                {
                    //pEffect->bNeedDelete = true;
                    DeleteEffect(pEffect->pAttr->wID, false); // 删除原来效果
                }
            }else
            {
                int i;

                // 如果这个效果被免疫，就不添加
                if (pEffectAttr->mutex.listImmune)
                {
                    for (i=0; pEffectAttr->mutex.listImmune[i]; i++)
                    {
                        if (FindEffectByClassID(pEffectAttr->mutex.listImmune[i]))
                        {
                            break;
                        }
                    }
                    if (pEffectAttr->mutex.listImmune[i])
                    {
                        //RtCoreLog().Info("效果[%s]被免疫\n", pEffectAttr->szName);
                        return NULL; // 如果这个效果被免疫，就不添加
                    }
                }

                // 如果这个效果免疫某些效果，就删除这些效果
                if (pEffectAttr->mutex.listDecline)
                {
                    for (i=0; pEffectAttr->mutex.listDecline[i]; i++)
                    {
                        pEffect = FindEffectByClassID(pEffectAttr->mutex.listDecline[i]);
                        if (pEffect)
                        {
                            //RtCoreLog().Info("效果[%s]免疫了[%s]\n", pEffectAttr->szName, pEffect->pAttr->szName);
                            pEffect->bNeedDelete = true;
                            // DeleteEffect(pEffect->pAttr->wID, false); // 删除免疫效果
                        }
                    }
                }

                // 如果这个效果与某些效果互斥，就删除这些效果
                if (pEffectAttr->mutex.listMutex)
                {
                    for (i=0; pEffectAttr->mutex.listMutex[i]; i++)
                    {
                        pEffect = FindEffectByClassID(pEffectAttr->mutex.listMutex[i]);
                        if (pEffect)
                        {
                            //RtCoreLog().Info("效果[%s]互斥了[%s]\n", pEffectAttr->szName, pEffect->pAttr->szName);
                            pEffect->bNeedDelete = true;
                            // DeleteEffect(pEffect->pAttr->wID, false); // 删除免疫效果
                        }
                    }
                }
            }

            // 添加效果
            m_listEffect.push_back(SActorEffect());
            pEffect = &(m_listEffect.back());
            pEffect->bNeedDelete = false;
            pEffect->dwParentUID = dwUID;
            pEffect->iRunTimes = 0;
            pEffect->dwRunTime = 0;
            pEffect->pAttr = pEffectAttr;

			//<add by  Tianh>//技能为1的时候不对自己作用技能效果 只对周围
			if (pEffect->pAttr->cTarget != 1)
			{
				OnEffectOpen(pEffect);
			} 
            //OnEffectOpen(pEffect);
            if (bRefresh)
            {
                OnEffectModify();
            }
            return pEffect;
        }
    }
    return NULL;
}

bool CActorSkill::DeleteEffect(unsigned short wID, bool bRefresh)
{
    std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->wID==wID)
		{
			OnEffectClose(&(*it)); // 关闭后就一定要删除
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	if (bRefresh)
	{
        OnEffectModify();
    }
    return true;
}

void CActorSkill::ClearEffect(bool bRefresh)
{
    stlforeach(std::list<SActorEffect>, it, m_listEffect)
    {
        OnEffectClose(&(*it)); // 关闭后就一定要删除
    }
    m_listEffect.clear();
    if (bRefresh)
    {
        OnEffectModify();
    }
}

void CActorSkill::DeleteUIDEffect(DWORD dwUID, bool bRefresh)
{
    std::list<SActorEffect>::iterator it;
    for (it=m_listEffect.begin(); it!=m_listEffect.end();)
    {
        if ((*it).dwParentUID==dwUID)
        {
			if ((*it).pAttr->isCleared)
			{
				OnEffectClose(&(*it)); // 关闭后就一定要删除
				it = m_listEffect.erase(it);
				continue;
			}
        }
        it ++;
    }
    if (bRefresh)
    {
        OnEffectModify();
    }
}

CActorSkill::SActorEffect* CActorSkill::FindEffect(unsigned short wID)
{
    stlforeach(std::list<SActorEffect>, it, m_listEffect)
    {
        if ((*it).pAttr->wID==wID)
        {
            return &(*it);
        }
    }
    return NULL;
}

CActorSkill::SActorEffect* CActorSkill::FindEffectByClassID(int cID)
{
    stlforeach(std::list<SActorEffect>, it, m_listEffect)
    {
        if ((*it).pAttr->cClass==cID)
        {
            return &(*it);
        }
    }
    return NULL;
}

void CActorSkill::OnAttributeChanged(bool bRefresh)
{
    const char* szResult;
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        SActorPassive* pPassive = &(*itPassive);
        if (pPassive->bActive)
        {
            // 检查条件是否还满足，不满足就关闭
            if ((szResult=pPassive->pAttr->filterUse.IsSuccess(this))!=NULL)
            {
                OnPassiveClose(pPassive);
                pPassive->bActive = false;
                DeleteUIDEffect(pPassive->dwUID, bRefresh);
            }
        }else
        {
            // 检查条件是否满足，满足就打开
            if ((szResult=pPassive->pAttr->filterUse.IsSuccess(this))==NULL)
            {
                OnPassiveOpen(pPassive);
                pPassive->bActive = true;
            }
        }
    }

    bool bChange = false;
    std::list<SActorEffect>::iterator itEffect;
    for (itEffect=m_listEffect.begin(); itEffect!=m_listEffect.end(); )
    {
        SActorEffect* pEffect =  &(*itEffect);
        if (pEffect->pAttr->filter.IsSuccess(this)!=NULL)
        {
            bChange = true;
            OnEffectClose(pEffect); // 关闭后就一定要删除
            itEffect = m_listEffect.erase(itEffect);
        }
        itEffect ++;
    }
    // 再处理一次，用来处理依赖效果的效果，但是这种处理只有1层。
    for (itEffect=m_listEffect.begin(); itEffect!=m_listEffect.end(); )
    {
        SActorEffect* pEffect =  &(*itEffect);
        if (pEffect->pAttr->filter.IsSuccess(this)!=NULL)
        {
            bChange = true;
            OnEffectClose(pEffect); // 关闭后就一定要删除
            itEffect = m_listEffect.erase(itEffect);
        }
        itEffect ++;
    }

    if (bChange && bRefresh)
    {
        OnEffectModify();
    }
}

void CActorSkill::StartupSkill()
{
    m_bRun = true;
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        if ((!(*itPassive).bActive) && (*itPassive).pAttr->filterUse.IsSuccess(this)==NULL)
        {
            OnPassiveOpen(&(*itPassive));
            (*itPassive).bActive = true;
        }
    }
}

float CActorSkill::GetPublicCDRate(SSkill* pAttr)
{
	if(pAttr->wClass == 13)
	{
		return (Wap_PublicCDMaxTime)?((float)m_WapPublicCDTime / Wap_PublicCDMaxTime):0.f;
	}else if(pAttr->wClass == 21)
	{
		return (beast_PublicCDMaxTime)?((float)m_beastPublicCDTime / beast_PublicCDMaxTime):0.f;
	}else if(pAttr->wClass >= 1 && pAttr->wClass <= 12 )
	{
		return (School_PublicCDMaxTime)?((float)m_SchoolPublicCDTime / School_PublicCDMaxTime):0.f;
	}
    //warning C4715: “CActorSkill::GetPublicCDRate”: 不是所有的控件路径都返回值
    return 0.f;
}

void CActorSkill::ShutdownSkill()
{
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        SActorPassive* pPassive = &(*itPassive);
        if (pPassive->bActive)
        {
            OnPassiveClose(pPassive);
            pPassive->bActive = false;
            DeleteUIDEffect(pPassive->dwUID, false);
        }
    }
    /*ClearEffect(false);*/
    m_bRun = false;
}

CSkillInstance* CActorSkill::GetSkillByIndex(int iIndex)
{
	if(iIndex < 0 || iIndex >= (int)m_listSkill.size())
		return 0;

	std::list<CSkillInstance*>::iterator it = m_listSkill.begin();
	while(iIndex > 0)
	{
		--iIndex;
		++it;
	}
	return *it;
}

CSkillInstance* CActorSkill::GetRandomSkill()
{
    int i, num = (int)m_listSkill.size();
    if(num == 0) return 0;
    num = rand()%num;

    std::list<CSkillInstance*>::iterator it = m_listSkill.begin();
    for (i=0; i<num; i++)
    {
        it ++;
    }
    return *it;
}

void CActorSkill::NetWriteSkill(CG_CmdPacket* pPacket)
{
    pPacket->WriteByte((char)m_listSkill.size());
    stlforeach(std::list<CSkillInstance*>, itSkill, m_listSkill)
    {
        CSkillInstance* pSkill = (*itSkill);
        pPacket->WriteShort(pSkill->m_pAttr->wID);
        pPacket->WriteShort(pSkill->m_wExp);
		pPacket->WriteLong(pSkill->m_dwResumeTime);
		pPacket->WriteLong(pSkill->m_dwUserTime);
    }

    pPacket->WriteByte((char)m_listPassive.size());
    stlforeach(std::list<SActorPassive>, itPassive, m_listPassive)
    {
        SActorPassive* pPassive = &(*itPassive);
        pPacket->WriteShort(pPassive->pAttr->wID);
    }
}

void CActorSkill::NetWriteEffect(CG_CmdPacket* pPacket)
{
    int cCnt = (int)m_listEffect.size();
   /* if (cCnt>10) cCnt = 0;*/
    pPacket->WriteByte(cCnt);
    std::list<SActorEffect>::iterator itEffect = m_listEffect.begin();
    for (int c=0; c<cCnt; c++)
    {
		long leftTime = -1;
		SActorEffect& theEffect = *itEffect;
        pPacket->WriteShort(theEffect.pAttr->wID);
		
		if (theEffect.pAttr->iCycle>0 && theEffect.pAttr->iDTimes > 0)
			leftTime = theEffect.pAttr->iDTimes * theEffect.pAttr->iCycle - theEffect.dwRunTime;
			
		pPacket->WriteLong(leftTime);
        itEffect ++;
    }
}

void CActorSkill::SetActiveSkill(unsigned short wID)
{
    if (FindActiveSkill(wID))
    {
        m_sActiveSkillID = wID;
    }else
    {
        //RtCoreLog().Info(s_pCannotFindSkillMsg, "设置激活错误", wID);
    }
}

unsigned short CActorSkill::GetActiveSkill()
{
    return m_sActiveSkillID;
}

bool CActorSkill::CanUseSkill(unsigned short wID)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        CHECK(pInstance->m_pAttr->dwRTarget==SKILL_TARGET_NONE);
        if (pInstance->CanUse(this, SKILL_TARGET_TYPE_NONE, NULL, NULL))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
    }
    return false;
}

bool CActorSkill::CanUseSkill(unsigned short wID, CActorSkill* pTarget)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        if (pInstance->CanUse(this, SKILL_TARGET_TYPE_ACTOR, pTarget, NULL))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
    }
    return false;
}

bool CActorSkill::CanUseSkill(unsigned short wID, float* pPos)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        CHECK(pInstance->m_pAttr->dwRTarget&SKILL_TARGET_POSITION);
        if (pInstance->CanUse(this, SKILL_TARGET_TYPE_POSITION, NULL, pPos))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
    }
    return false;
}

bool CActorSkill::UseSkill(unsigned short wID)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        CHECK(pInstance->m_pAttr->dwRTarget==SKILL_TARGET_NONE);
        if (pInstance->DoUse(this, SKILL_TARGET_TYPE_NONE, NULL, NULL))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
		MSG4("使用技能失败 %s\n", m_pLastErrorInfo);
    }
	MSG4("没有发现ID为：%d 的技能\n", wID);
    return false;
}

bool CActorSkill::UseSkill(unsigned short wID, DWORD dwTargetID)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        CHECK(pInstance->m_pAttr->dwRTarget&(~SKILL_TARGET_POSITION));
        if (pInstance->DoUse(this, SKILL_TARGET_TYPE_ACTOR, dwTargetID, NULL))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
    }
    return false;
}

bool CActorSkill::UseSkill(unsigned short wID, float* pPos)
{
    CSkillInstance* pInstance = FindActiveSkill(wID);
    if (pInstance)
    {
        CHECK(pInstance->m_pAttr->dwRTarget&SKILL_TARGET_POSITION);
        if (pInstance->DoUse(this, SKILL_TARGET_TYPE_POSITION, NULL, pPos))
        {
            return true;
        }
        m_pLastErrorInfo = pInstance->GetErrInfo();
    }
    return false;
}

// 学习技能
bool CActorSkill::StudySkill(unsigned short wID, bool bRefresh)
{
    SSkill* pAttr = Skill()->FindSkill(wID);
    if (pAttr && pAttr->wClass != 14)
    {
        SSkill* pSkillIns = FindSkillBySubID(pAttr->wSubID);
        if (pSkillIns)
        {
            if (pSkillIns->iLevel>=pAttr->iLevel)
            {
                m_pLastErrorInfo = R(SMSG_ALREADY_STUDY);
                return false;
            }
        }

        if (GetSkillPoint()<pAttr->iSP)
        {
            m_pLastErrorInfo = R(SMSG_SP_NOT_ENOUTH);
            return false;
		}

		if (GetSkillSoul()<pAttr->iSoul)
		{
			m_pLastErrorInfo = R(SMSG_VIGOUR_NOT_ENOUTH);
			return false;
		}

        const char* pFailMsg = pAttr->filterStudy.IsSuccess(this);
        if (pFailMsg==NULL)
        {
			OnStudySkill(pAttr, bRefresh);
            return true;
        }
        m_pLastErrorInfo = pFailMsg;
        return false;
    }
    m_pLastErrorInfo = R(SMSG_SKILL_INEXISTENT);
    return false;
}

const char* CActorSkill::GetLastErrInfo()
{
    return m_pLastErrorInfo?m_pLastErrorInfo:"";
}

//Tianh
//该函数中会遍历该角色所拥有的每一个技能
//刷新每一个技能的冷却时间（还要过多长时间才能使用该技能）
//刷新每一个技能特效的持续时间等
void CActorSkill::Run(unsigned long dwMillisecond)
{
    if (!m_bRun) return;
    stlforeach(std::list<CSkillInstance*>, itSkill, m_listSkill)
    {
        /*(*itSkill)->OnRun(dwMillisecond);*/
		(*itSkill)->OnRun(dwMillisecond,this);
    }

	//Tianh
	if (m_SchoolPublicCDTime>dwMillisecond)
	{
		m_SchoolPublicCDTime -=dwMillisecond;
	} 
	else
	{
		m_SchoolPublicCDTime = 0;
	}
	if (m_WapPublicCDTime>dwMillisecond)
	{
		m_WapPublicCDTime -= dwMillisecond;
	} 
	else
	{
		m_WapPublicCDTime =0;
	}
	if (m_PropPublicCDTime>dwMillisecond)
	{
		m_PropPublicCDTime -= dwMillisecond;
	}else
	{
		m_PropPublicCDTime = 0;
	}
	if (m_beastPublicCDTime>dwMillisecond)
	{
		m_beastPublicCDTime -= dwMillisecond;
	}else
	{
		m_beastPublicCDTime = 0;
	}

    bool bRemove;
    bool bModify = false;
    SActorEffect* pEffect;
    std::list<SActorEffect>::iterator itEffect;
    for (itEffect=m_listEffect.begin(); itEffect!=m_listEffect.end(); )
    {
        pEffect = &(*itEffect);
        pEffect->dwRunTime += dwMillisecond;
        if (pEffect->pAttr->iCycle>0) // 如果存在周期
        {
            bRemove = false;
            if (pEffect->bNeedDelete)
            {
                bRemove = true;
            }else
            {
                while (pEffect->dwRunTime > (DWORD)(pEffect->iRunTimes*pEffect->pAttr->iCycle))
                {
                    if (pEffect->pAttr->iDTimes>0 && pEffect->iRunTimes>=pEffect->pAttr->iDTimes)
                    {
                        bRemove = true;
                        break;
                    }
                    if (OnEffectCycle(pEffect))//Tianh 这里注意返回true 说明死亡 死亡里会删除特效
                    {
						OnEffectModify();
						return;
                    }
                    pEffect->iRunTimes ++;
                }
            }
            if (bRemove)
            {
                bModify = true;
                OnEffectClose(pEffect); // 关闭后就一定要删除
                itEffect = m_listEffect.erase(itEffect);
                continue;
            }
        }
        itEffect++;
    }

    if (bModify)
    {
        OnEffectModify();
    }
}

short CActorSkill::GetClassSP(unsigned short wClassID)
{
    short iCntSp = 0;

    CSkillInstance* pSkill;
    std::list<CSkillInstance*>::iterator itSkill;
    for (itSkill=m_listSkill.begin(); itSkill!=m_listSkill.end(); itSkill++)
    {
        pSkill = *itSkill;
        if (pSkill->m_pAttr->wClass==wClassID)
        {
            iCntSp += pSkill->m_pAttr->iLevel * pSkill->m_pAttr->iSP;
        }
    }

    SActorPassive* pPassive;
    std::list<SActorPassive>::iterator itPassive;
    for (itPassive=m_listPassive.begin(); itPassive!=m_listPassive.end(); itPassive++)
    {
        pPassive = &(*itPassive);
        if (pPassive->pAttr->wClass==wClassID)
        {
            iCntSp += pPassive->pAttr->iLevel * pPassive->pAttr->iSP;
        }
    }
    return iCntSp;
}

void CActorSkill::TidyUp()
{
    // 主动技能
    {
        std::list<CSkillInstance*>::iterator it1, it2;
        do
        {
            for (it1=m_listSkill.begin(); it1!=m_listSkill.end(); it1++)
            {
                it2 = it1;
                it2 ++;
                for (; it2!=m_listSkill.end(); it2++)
                {
                    if ((*it1)->m_pAttr->wSubID==(*it2)->m_pAttr->wSubID)
                    {
                        if ((*it1)->m_pAttr->iLevel>=(*it2)->m_pAttr->iLevel)
                        {
                            it2 = m_listSkill.erase(it2);
                            break;
                        }else
                        {
                            it1 = m_listSkill.erase(it1);
                            break;
                        }
                    }
                }
                if (it2!=m_listSkill.end())
                {
                    break;
                }
            }
        } while(it1!=m_listSkill.end());
    }
    // 被动技能
    {
        std::list<SActorPassive>::iterator it1, it2;
        do
        {
            for (it1=m_listPassive.begin(); it1!=m_listPassive.end(); it1++)
            {
                it2 = it1;
                it2 ++;
                for (; it2!=m_listPassive.end(); it2++)
                {
                    if ((*it1).pAttr->wSubID==(*it2).pAttr->wSubID)
                    {
                        if ((*it1).pAttr->iLevel>=(*it2).pAttr->iLevel)
                        {
                            if (!DeleteSkill((*it2).pAttr->wID, false))
                            {
                                return;
                            }
                            break;
                        }else
                        {
                            if (!DeleteSkill((*it1).pAttr->wID, false))
                            {
                                return;
                            }
                            break;
                        }
                    }
                }
                if (it2!=m_listPassive.end())
                {
                    break;
                }
            }
        } while(it1!=m_listPassive.end());
    }
}

SSkill* CActorSkill::FirstSkill()
{
    m_bFindSkill = true;
    CSkillInstance* pSkill = FirstActiveSkill();
    if (pSkill)
    {
        return pSkill->m_pAttr;
    }
    return NULL;
}

SSkill* CActorSkill::NextSkill()
{
    SActorPassive* pPassive;
    if (m_bFindSkill)
    {
        CSkillInstance* pSkill = NextActiveSkill();
        if (pSkill)
        {
            return pSkill->m_pAttr;
        }
        m_bFindSkill = false;
        pPassive = FirstPassiveSkill();
        if (pPassive)
        {
            return pPassive->pAttr;
        }
    }else
    {
        pPassive = NextPassiveSkill();
        if (pPassive)
        {
            return pPassive->pAttr;
        }
    }
    return NULL;
}

CSkillInstance* CActorSkill::FirstActiveSkill()
{
    m_itListSkill = m_listSkill.begin();
    if (m_itListSkill!=m_listSkill.end())
        return (*m_itListSkill);
    return NULL;
}

CSkillInstance* CActorSkill::NextActiveSkill()
{
    m_itListSkill ++;
    if (m_itListSkill!=m_listSkill.end())
        return (*m_itListSkill);
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::FirstPassiveSkill()
{
    m_itListPassive = m_listPassive.begin();
    if (m_itListPassive!=m_listPassive.end())
        return &(*m_itListPassive);
    return NULL;
}

CActorSkill::SActorPassive* CActorSkill::NextPassiveSkill()
{
    m_itListPassive ++;
    if (m_itListPassive!=m_listPassive.end())
        return &(*m_itListPassive);
    return NULL;
}

CActorSkill::SActorEffect* CActorSkill::FirstEffect()
{
    m_itListEffect = m_listEffect.begin();
    if (m_itListEffect!=m_listEffect.end())
        return &(*m_itListEffect);
    return NULL;
}

CActorSkill::SActorEffect* CActorSkill::NextEffect()
{
    m_itListEffect ++;
    if (m_itListEffect!=m_listEffect.end())
        return &(*m_itListEffect);
    return NULL;
}

int CActorSkill::SkillCount()
{
    return (int)(m_listSkill.size() + m_listPassive.size());
}

int CActorSkill::EffectCount()
{
    return (int)m_listEffect.size();
}

int CActorSkill::GetAllActiveSkillID(std::vector<int> &vIDList)
{
	vIDList.clear();
	stlforeach(std::list<CSkillInstance*>, it, m_listSkill)
	{
		vIDList.push_back( (int)(*it)->m_pAttr->wID );
	}

	return m_listSkill.size();
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

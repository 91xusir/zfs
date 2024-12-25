#include "gc_include.h"
#include "region_client.h"
#include "gc_actor.h"
#include "gc_actor_script_interface.h"
#include "game_assistant.h"
#include "gc_complex_missile.h"
#include "gc_bomb_missile.h"
#include "gc_missile_listener.h"
#include "gc_track_bomb_missile.h"
#include "gc_track_missile.h"
#include "trump_show.h"
#include "weapon_show.h"
#include "ui_form_playeritems.h"
#include "ui_form_char.h"
#include "ui_form_task.h"
#include "ui_form_item_update.h"
#include "ui_form_trade.h"
#include "ui_form_skill.h"
#include "ui_form_dupe.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "ui_form_playershop.h"
#include "ui_form_cre_info.h"
#include "UIForm_Char.h"
#include "gc_cursor.h"
#include "ui_form_trump.h"
#include "UIForm_Pickup.h"
#include "ui_form_minimap.h"
#include "UIForm_MiddleMap.h"
#include "ui_form_decompound.h"
#include "UIForm_Gem.h"
#include "ui_form_timebar.h"
#include "ui_form_pet.h"

#if DO_CONSOLE
bool GcActor::mDebug = false;
#endif  //DO_CONSOLE
#include <preConsole.h>

// Removed Unused Code by Wayne Wong 2010-12-14
//std::list<GcActor::SAttack*> m_attackUnused;
//std::list<GcActor::SAttack*> m_attackAlloc;
EXT_SPACE::unordered_map<GcActor*, std::string> GcActor::s_strActorNameMap;

GcActor::SAttack* NewAttack() {
    GcActor::SAttack* pNew = RT_NEW GcActor::SAttack;
    pNew->Init();
    return pNew;

    //GcActor::SAttack* pNew;

    //if (m_attackUnused.size()==0)
    //{
    //	pNew = RT_NEW GcActor::SAttack[20];都
    //	m_attackAlloc.push_back(pNew);
    //	for (int i=0; i<20; i++)
    //	{
    //		m_attackUnused.push_back(pNew+i);
    //	}
    //}
    //pNew = m_attackUnused.front();
    //m_attackUnused.pop_front();
    //pNew->Init();
    //return pNew;
}

void DelAttack(GcActor::SAttack* pAttack) {
    DEL_ONE(pAttack);
    //m_attackUnused.push_front(pAttack);
}

//////////////////////////////////////////////////////////////////////////
// CActorMissileListener
//////////////////////////////////////////////////////////////////////////

class CActorMissileListener : public CMissileListener {
   public:
    CActorMissileListener() {}

    virtual ~CActorMissileListener() {}

    // 时间到时刻
    virtual void OnTimeOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3) {
        guard;
        GcActor* pActor = FindActor(dwParam1);
        if (pActor)
            pActor->OnAttackArrive((WORD)dwParam2, false);
        unguard;
    }

    // 超出距离时刻
    virtual void OnSideOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3) {
        guard;
        GcActor* pActor = FindActor(dwParam1);
        if (pActor)
            pActor->OnAttackArrive((WORD)dwParam2, false);
        unguard;
    }

    // 到达目标时刻
    virtual void OnArrive(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3) {
        guard;
        GcActor* pActor = FindActor(dwParam1);
        if (pActor)
            pActor->OnAttackArrive((WORD)dwParam2, true);
        unguard;
    }
};

//////////////////////////////////////////////////////////////////////////
// GcActor
//////////////////////////////////////////////////////////////////////////
bool GcActor::m_bLoadPRI = false;
int  GcActor::m_PRIPlayer[GcActor::ACMD_MAX][GcActor::ACMD_MAX];
int  GcActor::m_PRINetuser[GcActor::ACMD_MAX][GcActor::ACMD_MAX];

GcActor::GcActor()
    : mBaseActor(this),
      mItem(((CGameClientFrame*)GetApp())->m_pItemManager, &(this->m_core), this) {
    guard;
    if (!m_bLoadPRI)
        LoadPRI();
    m_wAttackID             = 1;
    mpTrump                 = NULL;
    m_pRidePet              = NULL;
    m_dwID                  = 0;
    m_pCurCmd               = NULL;
    m_bIsIdle               = true;
    m_eNpcType              = ENT_UNKNOWN;
    m_eRideState            = ERS_NONE;
    mItemChangeCount        = 0;
    m_eBlockPosPKAttr       = PKA_NONE;
    m_bDeleteSelf           = false;
    m_iCurrentBlockY        = -1;
    s_strActorNameMap[this] = R(G_NONAME);
    m_pModel                = NULL;
    m_Skill.Init(this);
    m_bIsAttack       = false;
    m_pHUD            = NULL;
    m_unionID         = 0;
    m_bPShop          = 0;
    m_iCheckSeed      = 0;
    m_eCurPosPKAttr   = PKA_NONE;
    m_iCurrentBlockX  = -1;
    m_bHideHUD        = false;
    m_bWantStopAttack = false;
    m_pNpc            = NULL;
    m_bShowHUD        = true;

    m_CallNpcCount = 0;

    m_PkMode = PK_Monster;
    //-------------add start by tony 05.06.13--------------------------------//
    m_feod_name   = "";
    m_is_attacker = (BYTE)0;
    //-------------add end   by tony 05.06.13--------------------------------//

    m_dwState       = 0xffffffff;
    m_lBossMaxHp    = 0;
    m_lBossHp       = 0;
    m_bRenderSelect = true;
    m_cHpRate       = 100 / 3;
    m_HeadImageID   = 0;
    m_cMpRate       = 100 / 3;  // 剩余MP的比率(值为0到100)
    m_actorID       = 0;        // 人物ID
    m_lLev          = 0;        // 等级

    m_pLastTargetID     = 0;
    m_bTrace            = false;
    m_dwIntonateTime    = 0;
    m_iNeedIntonateTime = 0;
    m_bIntonate         = false;
    m_bIsDead           = 0;
    m_iRebirthTime      = 0;
    m_cRebirthType      = 0;
    m_cInTeam           = 0;
    m_bTeamOnOff        = true;
    m_lPickActorID      = 0xffffffff;
    m_bIsCombat         = false;
    m_bIsCollect        = false;
    bActivity           = false;
    m_lPlayIdleTime     = 0;
    m_cPickType         = 0;
    m_lLastDeadval      = 0;
    m_pChangeEffect     = NULL;

    CM_MEMPROTECTOR(m_pCollectTool, 20)
    m_pCollectTool[0]  = 0;
    m_bTriggerOver     = false;
    m_bTriggerStart    = false;
    triggerID          = 0;
    triggerMain        = 0;
    triggerOpen        = 0;
    m_bTriggerClose    = 1;
    m_lTriggerCloseNet = 2;
    m_bInDungeon       = false;
    m_bInCallPet       = false;
    m_dungeonUID       = 0;
    m_dungeonTID       = 0;
    m_cIsDeadNet       = 0;
    bTaskCollect       = false;
    m_fVisible         = 1.f;
    m_eIdlePose        = GcBaseActor::POSE_IDLE;
    m_mapSetEffect.clear();
    m_bDoMoveCmd          = false;
    m_cShapeshift         = 0;
    m_dwShapeshiftModelID = 0;
    m_cShapeshiftNet      = 0;
    m_SkillSeries         = 0;

    CM_MEMPROTECTOR(m_linkName, 20)

    m_dwCatchPet      = 0;
    m_dwCatchMe       = 0;
    m_bTeleEvent      = false;
    m_bDisplay        = true;
    m_linkName[0]     = 0;
    m_ePetType        = PET_TYPE_NULL;
    m_dwPetMasterID   = 0;
    m_dwPetFuseLev    = 0;
    m_dwPetAbilityLev = 0;
    m_dwDeadTime      = 0;
    unguard;
}

GcActor::~GcActor() {
    guard;

    CM_MEMUNPROTECTOR(m_pCollectTool)
    CM_MEMUNPROTECTOR(m_linkName)

    mBaseActor.RemoveSelf();
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        DEL_ONE((*it));
    }
    m_listAttack.clear();

    this->DestroyGraphData();
    unguard;
}

void GcActor::LoadPRI() {
    guard;
    m_bLoadPRI = true;
    int   i, j;
    RtCsv csv;
    if (csv.LoadTableFile(R(RES_TABLE_PRI_PLAYER))) {
        for (j = 0; j < ACMD_MAX; j++) {
            for (i = 0; i < ACMD_MAX; i++) {
                if (csv[GetCmdName((ECommands)j)][GetCmdName((ECommands)i)].IsValid()) {
                    m_PRIPlayer[j][i] =
                        csv[GetCmdName((ECommands)j)][GetCmdName((ECommands)i)].Int();
                } else {
                    m_PRIPlayer[j][i] = -1;
                }
            }
        }
    } else {
        memset(m_PRIPlayer, 0, sizeof(int) * ACMD_MAX * ACMD_MAX);
    }
    RtCsv csv1;
    if (csv1.LoadTableFile(R(RES_TABLE_PRI_NETUSER))) {
        for (j = 0; j < ACMD_MAX; j++) {
            for (i = 0; i < ACMD_MAX; i++) {
                if (csv1[GetCmdName((ECommands)j)][GetCmdName((ECommands)i)].IsValid()) {
                    m_PRINetuser[j][i] =
                        csv1[GetCmdName((ECommands)j)][GetCmdName((ECommands)i)].Int();
                } else {
                    m_PRINetuser[j][i] = -1;
                }
            }
        }
    } else {
        memset(m_PRINetuser, 0, sizeof(int) * ACMD_MAX * ACMD_MAX);
    }
    unguard;
}

void GcActor::Destroy() {
    guard;

    g_pScene->RemoveObjectByLastRenderList(mBaseActor.GetRender());

    if (GetWorld() && GetWorld()->m_pPlayer && GetWorld()->m_pPlayer->m_dwLatestTarget == ID())
        GetWorld()->m_pPlayer->m_dwLatestTarget = 0;

    unguard;
}

DWORD GcActor::ID() {
    return ((((m_dwID & 0xFFFF0000) >> 16) & 0x0000FFFF) |
            (((m_dwID & 0x0000FFFF) << 16) & 0xFFFF0000));
}

void GcActor::SetID(DWORD vID) {
    m_dwDummyID2 = m_dwDummyID = vID;
    m_dwID =
        ((((vID & 0xFFFF0000) >> 16) & 0x0000FFFF) | (((vID & 0x0000FFFF) << 16) & 0xFFFF0000));
}

void GcActor::CreateTrump() {
    guard;
    if (mpTrump != NULL) {
        mpTrump->Delete();  //销毁
    }

    mpTrump = RT_NEW CComplexMissile;  //法宝
    if (mpTrump == NULL) {
        ShowErr(R(AMSG_CREATE_TRUMP_FAIL));
    } else {
        mpTrump->SetMaster(&mBaseActor);
    }
    unguard;
}

//创建图形
bool GcActor::CreateGraphData(SCreModel* vpModel, short HeadModelID) {
    guard;
    m_pModel = vpModel;
    //m_cModelFlag = cFlag;
    //mBaseActor.SetMoveSpeed(vpModel->MoveSpeed);
    //mpAI->SetAttackRange(vpModel->AttackRange);

    //设置道具的默认值
    if (vpModel->bComplex && !m_cShapeshiftNet) {
        // mItem.EquipDefaultItem(CItemContainer::HEAD,vpModel->HeadSkin);
        mItem.EquipDefaultItem(CItemContainer::GLOVE, vpModel->HandSkin.c_str());
        mItem.EquipDefaultItem(CItemContainer::BODY, vpModel->BodySkin.c_str());
        mItem.EquipDefaultItem(CItemContainer::FOOT, vpModel->FootSkin.c_str());
        mItem.EquipDefaultItem(CItemContainer::CAESTUS, vpModel->CaestusSkin.c_str());
    } else {
        // mItem.EquipDefaultItem(CItemContainer::HEAD,NULL);
        mItem.EquipDefaultItem(CItemContainer::GLOVE, NULL);
        mItem.EquipDefaultItem(CItemContainer::BODY, NULL);
        mItem.EquipDefaultItem(CItemContainer::FOOT, NULL);
        mItem.EquipDefaultItem(CItemContainer::CAESTUS, NULL);
    }

    OnCreateGraph();

    ResetHudName();

    //    return mBaseActor.CreateGraphData(vpModel);
    bool res = mBaseActor.CreateGraphData(vpModel, HeadModelID);
    if (res) {
        //mBaseActor.PlayPose(GcBaseActor::POSE_STAND, true);
        if (g_pScene) {
            g_pScene->m_camMatPrev.Unit();
        }
    }
    return res;

    unguard;
}

bool GcActor::CreateGraphData(short vModelID, short HeadModelID) {
    guard;
    if (vModelID == 0) {
        return false;
    }
    m_core.ModelId = vModelID;

    SCreModel* pModel = g_TableCreModel.FindCreModel(vModelID);
    if (pModel != NULL) {
        return CreateGraphData(pModel, HeadModelID);
    }
    return false;
    unguard;
}

bool GcActor::DestroyGraphData()  //销毁图形
{
    guard;

    if (!m_pHUD.IsEmpty()) {
        //m_pHUD->Close();//rem by fox
        g_workspace.RemoveWidget(m_pHUD->getId());
        m_pHUD->Hide();
        m_pHUD->ShowHPBar(false);
        m_pHUD->ShowStall(false);  //add by max
        m_pHUD->ShowLogo(false);
        m_pHUD->ShowSpeak(false);
        m_pHUD->drop();
        m_pHUD = NULL;
    }

    if (mpTrump != NULL) {
        mpTrump->Delete();
    }

    return mBaseActor.DestroyGraphData();
    unguard;
}

// 更新角色头顶的HUD(血条、名字等信息)显示
void GcActor::UpdateHUD() {
    guard;
    // HUD
    if (!m_pHUD.IsEmpty()) {
        // 重置HUD上显示的名字
        ResetHudName();
        //if (GetWorld()->m_pPlayer == this)
        //	m_pHUD->ShowHPBar(false);
        //else if (GetWorld()->m_pPet == this)
        //	m_pHUD->ShowHPBar(true);
        //2010.3.28 gao

        // 获取HUD在屏幕上的坐标
        int iX, iY;
        GetDevice()->GetScreenPos(GetHUDPos(), iX, iY);
        // 设置HUD的位置
        m_pHUD->SetPosition(RtgVertex3(iX, iY, 0));

        // add by fancui for set bloodbar value @ 2005-04-21
        // 设置血条的值,m_cHpRate为当前血量百分比
        m_pHUD->SetBloodValue((float)m_cHpRate /*/3*/);  //除以3 是为了让血条的最大长度为33
        // added by fancui for set bloodbar value @ 2005-04-21
    }
    unguard;
}

//lmk GcActor::Run 逻辑循环
void GcActor::Run(float fSecond)  //逻辑循环
{  // 如果角色正在吟唱且不是当前玩家，则播放吟唱动画
    if (m_bIntonate && GetPlayer() != this)
        mBaseActor.PlayPose(GcBaseActor::POSE_CHANT, true);

    m_Skill.Run(fSecond * 1000);  // 运行技能系统，fSecond 转换为毫秒

    mItem.Run(fSecond * 1000);  // 运行物品系统，fSecond 转换为毫秒

    RunCommand(fSecond);  // 执行命令

    mBaseActor.Run(fSecond);  // 运行角色的基础动画

    if (mpTrump)  // 如果有法宝，则运行法宝的逻辑
        mpTrump->Run(fSecond);

    // 获取角色的位置并触发位置变化事件
    const float* fPos = GetMatrix()->Position();
    OnPositionChanged(fPos[0], fPos[1]);

    //if (m_pHUD)
    //	m_pHUD->SetSpeakText(rtFormatNumber(m_aiStateForDebug));

    if (!m_pNpc && mBaseActor.GetGraph())  // 如果角色是 NPC 并且有图形组件，则设置图形的可见性

        GetGraph()->SetVisible(m_fVisible);

    if (m_bIsDead != m_cIsDeadNet) {      // 处理角色死亡状态
        m_dwDeadTime += fSecond * 1000;   // 增加死亡时间
        if (m_dwDeadTime > 3000) {        // 如果死亡时间超过 3000 毫秒
            m_dwDeadTime = 0;             // 重置死亡时间
            m_bIsDead    = m_cIsDeadNet;  // 更新死亡状态
        }
    } else if (m_dwDeadTime != 0) {
        m_dwDeadTime = 0;  // 如果死亡时间不为零，重置死亡时间
    }
}

bool GcActor::RunCommand(float fSecond) {
    guard;
    const float g_fClientDieTime       = 5.f;
    const float g_fClientDieFlyHeight  = 60.f;
    const float g_fClientDieSinkHeight = -100.f;

    if (m_pCurCmd) {
        CRT_ActorInstance* pGraph;
        GcActor*           pTarget;
        const RtgMatrix16* pM16   = NULL;
        float              fSpeed = -1.f;

        m_pCurCmd->fRunTime += fSecond;
        switch (m_pCurCmd->eCmd) {
            case ACMD_NONE:
                m_pCurCmd = NULL;
                LOG("GcActor::RunCommand: 某个地方把命令设置成NONE的时候没有修改命令运行状态.\n");
                break;
            case ACMD_MOVETO:
                if (!IsAbiMove()) {
                    m_bDoMoveCmd = false;
                    CommandFinished();
                    break;
                }
                if (!mBaseActor.MoveToPosition(m_pCurCmd->f[0], m_pCurCmd->f[1],
                                               m_pCurCmd->fDistance, m_pCurCmd->fTime, fSecond,
                                               false)) {
                    m_bDoMoveCmd = false;
                    CommandFinished();
                }
                break;
            case ACMD_FOLLOW:
                if (!IsAbiMove()) {
                    CommandFinished();
                    break;
                }
                pTarget = FindAllActor(m_pCurCmd->dw[0]);
                if (pTarget) {
                    pM16 = pTarget->GetMatrix();
                    if (!mBaseActor.MoveToPosition(pM16->_30, pM16->_31, m_pCurCmd->fDistance,
                                                   m_pCurCmd->fTime, fSecond, true)) {
                        CommandFinished();
                    }
                } else {
                    CommandFinished();
                }
                break;
            case ACMD_DEATH:
                if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
                    m_bIsDead = m_cIsDeadNet;
                }
                break;
            case ACMD_PLAY_POSE:
                if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
                    if (m_pCurCmd->dw[0] == GcBaseActor::POSE_DEAD && GetPlayer() == this) {
                        CommandFinished();
                        ClearCommand();
                        g_layerMain->m_formRebirth->Show();
                    }
                }
                break;
            case ACMD_DEATH_ACT:
                if (m_pCurCmd->fRunTime < g_fClientDieTime) {
                    if (m_eNpcType == ENT_MONSTER || m_eNpcType == ENT_NPC_COMBATIVE)
                        m_bHideHUD = true;

                    if (mBaseActor.m_vPoseID != GcBaseActor::POSE_BODY &&
                        mBaseActor.m_vPoseID != GcBaseActor::POSE_DEAD && NpcType() != ENT_SPIRIT) {
                        mBaseActor.PlayPose(GcBaseActor::POSE_BODY, true);
                    }

                    pGraph = mBaseActor.GetGraph()->p();
                    if (pGraph && m_bIsDead > 0 && m_eNpcType != ENT_USER) {
                        if (m_pCurCmd->dw[0])  // dw[0]半透明
                        {
                            mBaseActor.GetGraph()->SetVisible(
                                (g_fClientDieTime - m_pCurCmd->fRunTime) / g_fClientDieTime);
                            //pGraph->SetVisible((g_fClientDieTime-m_pCurCmd->fRunTime)/g_fClientDieTime);
                        }
                        if (m_pCurCmd->dw[1] && this != GetPlayer())  // dw[1]飞
                        {
                            const RtgVertex3* _v3 = pGraph->GetMatrix()->Row(3);
                            pGraph->SetMatrix_Row(
                                3, RtgVertex3(_v3->x, _v3->y,
                                              _v3->z + g_fClientDieFlyHeight *
                                                           (fSecond / g_fClientDieTime)));
                        }
                        if (m_pCurCmd->dw[2])  // dw[2]入地
                        {
                            const RtgVertex3* _v3 = pGraph->GetMatrix()->Row(3);
                            pGraph->SetMatrix_Row(
                                3, RtgVertex3(_v3->x, _v3->y,
                                              _v3->z + g_fClientDieSinkHeight *
                                                           (fSecond / g_fClientDieTime)));
                        }
                    }
                } else {
                    CommandFinished();
                }
                break;
            case ACMD_COLLECT:
                if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
                    mBaseActor.PlayPose(mBaseActor.m_vPoseID, false);
                    //CommandFinished();
                }
                break;
            case ACMD_PLAYTASKITEM:
                if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
                    CommandFinished();
                }
                break;
            case ACMD_START_TRIGGER: {
                if (!GetWorld()->GetDungeon()) {
                    ERR("客户端副本信息不存在\n");
                    CommandFinished();
                    break;
                }

                if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
                    STrigger* pTrigger = GetWorld()->GetDungeon()->GetTrigger(triggerID);

                    if (!pTrigger) {
                        ERR("机关信息不存在\n");
                        CommandFinished();
                        break;
                    }

                    if (8 == m_pNpc->Type) {
                        pTrigger->opendoorCurNum++;
                    }

                    if (8 == m_pNpc->Type && m_bTriggerOver && pTrigger->type == 2) {
                        for (int i = 0; i < pTrigger->opendoorNum; i++) {
                            GcActor* pActor = FindAllActor(pTrigger->opendoorUID[i]);
                            if (pActor) {
                                if (pActor->m_bTriggerOver)
                                    continue;
                                pActor->m_bTriggerOver = true;
                                pActor->StartTriggerAdd(pTrigger->close, true);
                                break;
                            } else {
                                pTrigger->opendoorCurNum++;
                            }
                        }
                    }

                    if (8 == m_pNpc->Type && triggerMain && pTrigger->type != 2) {
                        CommandFinished();
                        pTrigger->state = pTrigger->close;
                        if (pTrigger->type == 2) {
                            for (int i = 0; i < pTrigger->opendoorNum; i++) {
                                GcActor* pActor = FindAllActor(pTrigger->opendoorUID[i]);
                                if (pActor) {
                                    pActor->m_bTriggerOver = false;
                                }
                            }
                        }
                        return true;
                    }

                    else if (7 == m_pNpc->Type && m_bTriggerStart) {
                        if (pTrigger->opendoorNum == 0) {
                        } else {
                            for (int i = 0; i < pTrigger->opendoorNum; i++) {
                                GcActor* pActor = FindAllActor(pTrigger->opendoorUID[i]);
                                if (pActor) {
                                    pActor->StartTriggerAdd(pTrigger->close, true);
                                    if (pTrigger->type == 2) {
                                        pActor->m_bTriggerOver = true;
                                        break;
                                    }
                                } else {
                                    pTrigger->opendoorCurNum++;
                                }
                            }
                        }
                        m_bTriggerStart = false;
                    }
                    CommandFinished();
                }
            } break;
        }
        return true;
    } else {
        if (m_listCmd.size()) {
            m_pCurCmd = &(m_listCmd.front());
            if (m_pCurCmd->eCmd != ACMD_TELEPORT) {
                m_bIsIdle = false;
            }

            while (m_pCurCmd->cSubType == 2) {
                SCmd cmd = *m_pCurCmd;
                m_listCmd.pop_front();
                if (cmd.bLoop &&
                    m_listCmd.size() == 0)  // 如果是一个循环命令，并且后面没有命令，就再添加一次
                {
                    if (m_bWantStopAttack)
                        m_bWantStopAttack = false;
                    else {
                        AddCommand(cmd);
                    }
                }

                if (m_listCmd.size()) {
                    m_pCurCmd = &(m_listCmd.front());
                } else {
                    m_pCurCmd = NULL;
                    break;
                }
            }
            if (m_pCurCmd) {
                DoCommand(m_pCurCmd);
                if (m_pCurCmd) {
                    if (m_pCurCmd->eCmd == ACMD_MOVETO || m_pCurCmd->eCmd == ACMD_FOLLOW) {
                        RunCommand(fSecond);
                    }
                }
                return true;
            }
        } else if (GetGraph()->p() && GetGraph()->p()->ResourceReady()) {
            if (m_bIsDead > 0) {
                RunBody(fSecond);
            } else
                RunIdle(fSecond);
        }
    }
    return false;
    unguard;
}

void GcActor::RunBody(float fSecond) {
    if (m_pNpc && (m_pNpc->Type == 4 || m_pNpc->Type == 5 || m_pNpc->Type == 9))
        return;
    if (NpcType() == ENT_SPIRIT)
        return;

    if (mBaseActor.GetGraph()->p() && !mBaseActor.GetGraph()->p()->IsPlayingPose()) {
        if (m_cIsDeadNet == 0 && NpcType() == ENT_MONSTER)
            m_cIsDeadNet = 1;
        m_bIsDead = m_cIsDeadNet;
        mBaseActor.PlayPose(GcBaseActor::POSE_BODY, false);
        if (m_bIsDead == 1 && (m_eNpcType == ENT_MONSTER || m_eNpcType == ENT_NPC_COMBATIVE)) {
            if (!mBaseActor.m_pItemEffect) {
                const char* effect = "linghun";
                const char* link   = "root";
                ////#ifndef _DEBUG
                //				if (m_pNpc && m_pNpc->szDeadEffect.c_str() && strlen(m_pNpc->szDeadEffect.c_str()) < 32)
                //				{
                //					effect = m_pNpc->szDeadEffect.c_str();
                //				}
                //				if (m_pNpc && m_pNpc->szDeadLink.c_str() && strlen(m_pNpc->szDeadLink.c_str()) < 32)
                //				{
                //					link = m_pNpc->szDeadLink.c_str();
                //				}
                //// #endif
                if (effect && link) {
                    mBaseActor.PlayDropItemEffect(effect, link);
                }
            }
        } else {
            mBaseActor.StopDropItemEffect();
        }
    } else if (mBaseActor.m_vPoseID != GcBaseActor::POSE_DEAD &&
               mBaseActor.m_vPoseID != GcBaseActor::POSE_BODY) {
        mBaseActor.PlayPose(GcBaseActor::POSE_BODY, false);
    }

    if (this == GetPlayer()) {
        time_t iNowTime;
        // 		static DWORD timeBack = 0;
        if (!g_layerMain->m_formRebirth->IsVisible()) {
            // 			timeBack = 0;
            g_layerMain->m_formRebirth->Show();
        }
        // 		else
        // 		{
        // 			timeBack += fSecond*100+3;
        //
        // 			if (timeBack < 180)
        // 			{
        // 				g_layerMain->m_formRebirth->SetRebirthColor((DWORD)((timeBack)<<24));
        // 			}
        // 		}
        time(&iNowTime);
        int surplusTime = (m_iRebirthTime - iNowTime);

        if (surplusTime >= 0) {
            g_layerMain->m_formRebirth->SetRebirthTime(surplusTime);
        } else {
            g_layerMain->m_formRebirth->SendRebirthType(3);
            g_layerMain->m_formRebirth->Hide();
        }

        if (this == GetPlayer() && g_layerMain->m_formChar->IsVisible() &&
            g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP) {
            g_layerMain->m_formChar->ReCreate3DViewGraph();
        }
    }
}

void GcActor::RunIdle(float fSecond) {
    guard;
    if (m_bIsDead)
        return;

    if (m_pNpc && (m_pNpc->Type == 7 || m_pNpc->Type == 8)) {  // 机关待机
        if (!mBaseActor.GetGraph()->p()->IsPlayingPose()) {
            if (m_bTriggerClose)
                mBaseActor.PlayPose("wait02", false);
            else
                mBaseActor.PlayPose("wait01", false);
        }
        return;
    }
    //mBaseActor.PlayDropItemEffect("beiji","root");
    GcBaseActor::EPoses vPose = GcBaseActor::POSE_STAND;
    if (!m_bIsIdle) {
        m_bIsIdle = true;
        //mBaseActor.PlayPose(GcBaseActor::POSE_STAND, true);
        if (!m_bIsCombat) {
            if (rtGetMilliseconds() > m_lPlayIdleTime) {
                vPose           = GcBaseActor::POSE_IDLE;
                m_lPlayIdleTime = rtGetMilliseconds() + 10 * 1000;
            } else {
                vPose = GcBaseActor::POSE_STAND;
            }
        } else {
            vPose = GcBaseActor::POSE_GUARD;
        }
        if (m_eIdlePose != GcBaseActor::POSE_IDLE) {
            vPose = m_eIdlePose;
        }
        mBaseActor.PlayPose(vPose, false);
    } else {
        CRT_ActorInstance* pG = mBaseActor.GetGraph()->p();
        if (pG) {
            if (!pG->IsPlayingPose()) {
                if (!m_bIsCombat) {
                    if (rtGetMilliseconds() > m_lPlayIdleTime) {
                        vPose           = GcBaseActor::POSE_IDLE;
                        m_lPlayIdleTime = rtGetMilliseconds() + 10 * 1000;
                    } else {
                        vPose = GcBaseActor::POSE_STAND;
                    }
                } else {
                    vPose = GcBaseActor::POSE_GUARD;
                }
                if (m_eIdlePose != GcBaseActor::POSE_IDLE) {
                    vPose = m_eIdlePose;
                }
                mBaseActor.PlayPose(vPose, false);
            }
        }
    }
    unguard;
}

void GcActor::DoAllCommandNow() {
    guard;
    int iCnt = 0;
    while (RunCommand(0.f)) {
        iCnt++;
    }
    unguard;
}

#if DO_CONSOLE

const int LINE_HEIGHT = 20;

void GcActor::DebugInfo() {
    if (mDebug) {
        string strInfo = "Command list: ";
        if (m_listCmd.size()) {
            SCmd*                     pCmd;
            std::list<SCmd>::iterator it;
            for (it = m_listCmd.begin(); it != m_listCmd.end(); it++) {
                pCmd    = &(*it);
                strInfo = strInfo + GetCmdName(pCmd->eCmd) + "\n  ";
            }
        } else {
            strInfo = strInfo + "NULL";
        }
        GetDevice()->DrawString(10, 25, 0xffffffff, strInfo.c_str());

        //string DebugString;

        ////人物AI部分的状态
        //DebugString="";

        //if(mpAI!=NULL)
        //{
        //    switch(mpAI->GetAttackState())
        //    {
        //    case CAIProcessor::NULL_STATE:
        //        DebugString="NULL_STATE";
        //        break;
        //    case CAIProcessor::MOVE_STATE:
        //        DebugString="MOVE_STATE";
        //        break;
        //    case CAIProcessor::SEARCH_STATE:
        //        DebugString="SEARCH_STATE";
        //        break;
        //    case CAIProcessor::READY_ATTACK_STATE:
        //        DebugString="READY_ATTACK_STATE";
        //        break;
        //    case CAIProcessor::ATTACK_STATE:
        //        DebugString="ATTACK_STATE";
        //        break;
        //    default:
        //        DebugString="UNKNOW_STATE";
        //        break;
        //    }
        //    DebugString="AI:["+DebugString+"]";
        //    GetDevice()->DrawString(100,Y,0xffffffff,DebugString.c_str());
        //    Y+=LINE_HEIGHT;
        //    DebugString="ID:[";
        //    DebugString=DebugString+ID();
        //    DebugString=DebugString+"]";
        //    GetDevice()->DrawString(100,Y,0xffffffff,DebugString.c_str());
        //    Y+=LINE_HEIGHT;
        //}

        ////人物基本部分
        //mBaseActor.DebugInfo(100,Y);
        //
        //if(mpTrump!=NULL)//法宝
        //{
        //    mpTrump->DebugInfo(300,200);
        //}
    }
}
#endif  //DO_CONSOLE

void GcActor::Render(RtgDevice& inDevice, RTGRenderMask eMask)  //渲染
{
    guard;
    mBaseActor.Render(inDevice, eMask);

    if (mpTrump) {
        mpTrump->Render(inDevice, eMask);
    }
    unguard;
}

void GcActor::Point2Tile(float vX, float vY, WORD& vrTileX, WORD& vrTileY) {
    int ix, iy;
    g_pScene->GetTerrainByPosFast(vX, vY, ix, iy);
    vrTileX = ix;
    vrTileY = iy;
}

bool GcActor::SetModel(short Id, short vHeadModelID) {
    guard;
    if (Id == 0)
        return false;
    if (m_core.ModelId == Id)
        return false;

    SCreModel* model = g_TableCreModel.FindCreModel(Id);
    if (!model)
        return false;
    m_core.ModelId = Id;

    // mItem.UnEquip(CItemContainer::WEAPON_1);
    // mItem.UnEquip(CItemContainer::WEAPON_2);
    // mItem.Reset();
    mItem.OnResetGraph();
    CreateGraphData(model, vHeadModelID);
    // mItem.UpdateGraph();

    //add by yz 2010-6-25:摆摊时隐藏武器和法宝
    if (m_bPShop) {
        if (NULL != mBaseActor.m_pTrump)  //隐藏法宝
            mBaseActor.m_pTrump->m_bRender = false;
        if (NULL != mBaseActor.m_pWeapon)  //隐藏武器
            mBaseActor.m_pWeapon->m_bRender = false;
    } else if (!GetRidePet()) {
        if (NULL != mBaseActor.m_pTrump)  //隐藏法宝
            mBaseActor.m_pTrump->m_bRender = true;
        if (NULL != mBaseActor.m_pWeapon)  //隐藏武器
            mBaseActor.m_pWeapon->m_bRender = true;
    }
    //end yz

    // 播放换模型特效
    // mBaseActor.PlayEffect("eborn.act",true,NULL,RtgVertex3(0,0,0));
    m_cShapeshift = m_cShapeshiftNet;
    CommandFinished();
    return true;
    unguard;
}

void GcActor::DisplayActor(bool bDisplay) {
    guard;

    if (!m_bDisplay)
        bDisplay = m_bDisplay;

    if (!m_pHUD.IsEmpty()) {
        if (bDisplay && !m_bHideHUD) {
            if (m_eNpcType == ENT_NPC_SERVICE) {
                m_pHUD->SetNameColor(0xFF63B8FF);
            } else if (m_eNpcType == ENT_MONSTER) {
                if (m_pNpc && m_pNpc->ViewRange > 0) {
                    m_pHUD->SetNameColor(0xFFEE4000);
                } else {
                    m_pHUD->SetNameColor(0xFF00CD00);
                }

            } else if (m_eNpcType == ENT_PET) {
                m_pHUD->SetNameColor(0xFFD09E00);
            } else if (m_eNpcType == ENT_SPIRIT) {
                m_pHUD->SetNameColor(0xFFD09E00);
            }
            if (m_pNpc && ID() == GetWorld()->m_dNPCID) {
                if (GetWorld()->m_dNPCNameColor) {
                    m_pHUD->SetNameColor(GetWorld()->m_dNPCNameColor);
                } else {
                    m_pHUD->SetNameColor(0xFFFFFFFF);
                }
            }

            if (g_layerMain) {
                bool showHUDName = false;
                if (this == GetPlayer() && g_layerMain->m_eSetting.m_bShowSelfName) {
                    m_pHUD->Show(true);
                    m_pHUD->ShowNameSeparate(true);
                    showHUDName = true;
                }
                if (g_layerMain->m_eSetting.m_bShowOtherName &&
                    (this->m_eNpcType == ENT_USER || this->m_eNpcType == ENT_PET)) {
                    m_pHUD->Show(true);
                    m_pHUD->ShowNameSeparate(true);
                    showHUDName = true;
                }
                if (g_layerMain->m_eSetting.m_bShowNPCName &&
                    (this->NpcType() == ENT_NPC_SERVICE || this->NpcType() == ENT_MONSTER ||
                     this->NpcType() == ENT_NPC_COMBATIVE || this->NpcType() == ENT_SPIRIT)) {
                    m_pHUD->Show(true);
                    m_pHUD->ShowNameSeparate(true);
                    showHUDName = true;
                }
                if (!showHUDName) {
                    m_pHUD->Show(true);
                    m_pHUD->ShowNameSeparate(false);
                }
                if (this->IsTrigger())
                    m_pHUD->Hide();
            }
        } else
            m_pHUD->Hide();
    }
    GetGraph()->DisplayActor(bDisplay);
    unguard;
}

void GcActor::SetPKState(char ePKState) {
    if (m_ePKState != ePKState) {
        m_ePKState = ePKState;
        if (!m_pHUD.IsEmpty()) {
            switch (ePKState) {
                case PK_STATE_NORMAL:
                    m_pHUD->SetNameColor(0xFFFFFFFF);
                    break;
                case PK_STATE_YELLOW_NAME:
                    m_pHUD->SetNameColor(0xFFFFFF00);
                    break;
                case PK_STATE_RED_NAME:
                    m_pHUD->SetNameColor(0xFFFF0000);
                    break;
            }
        }
        m_dwDummyID2 = rtRandom();
    }
}

RtgVertex3 GcActor::GetHUDPos() {
    guard;
    RtgVertex3         vPos(mBaseActor.GetMatrix()->Position());
    CRT_ActorInstance* pActor = mBaseActor.GetGraph()->p();
    if (pActor) {
        vPos           = pActor->GetWorldMatrix()->Position();
        RtgAABB* pAABB = pActor->GetStaticBoundingBox();
        if (pActor->m_Name == "mon062.act") {
            RtgAABB* pAABB1 = pActor->GetBoundingBox();
            pAABB->vPos.z   = pAABB1->vPos.z;
            pAABB->vExt.z   = pAABB1->vExt.z;
        }
        if (NpcType() == ENT_NPC_SERVICE) {
            if (pAABB->vPos.z > 14.0f && pAABB->vPos.z < 15.0f) {
                pAABB->vPos.z = 21.30f;
            }
            if (pAABB->vExt.z > 14.0f && pAABB->vExt.z < 15.0f) {
                pAABB->vExt.z = 21.30f;
            }
        }
        vPos.z += pAABB->vPos.z + pAABB->vExt.z;
    }
    return vPos;
    unguard;
}

//自动寻路到某地
void GcActor::GotoThere(int x, int y) {
    // 将地图设置为x, y 坐标
    g_layerMain->m_formMiddleMap->SetOnMoveto(x, y);

    // 定义两个浮点数 X 和 Y，用于存储转换后的坐标
    float X = 0, Y = 0;

    // 根据地形的 XY 坐标获取实际的场景坐标，并存储在 X 和 Y 中
    g_pScene->GetPosByTerrainXY(x, y, X, Y);

    // 将当前函数的标识符压入调用堆栈中，以便跟踪调用链
    PushCallStack(csn_GcActor_AddCommandMoveTo);

    // 更新玩家的随机种子，通过一个数学表达式生成新的种子值
    (((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >>
      16) &
     0x7fff);

    // 将移动命令添加到玩家的指令列表中，移动到 X, Y 坐标，其他参数用作方向和速度
    GetWorld()->m_pPlayer->AddCommandMoveTo(X, Y, 0, -1.0f, 0);

    // 保存当前的随机种子值，以备将来使用
    GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;

    // 从调用堆栈中弹出当前函数的标识符，表示此函数执行完毕
    PopCallStack();
}

void GcActor::GotoNpcFront(SNpc* pNpc) {
    RtgMatrix16 npcMatrix;
    npcMatrix.Unit();
    rtgDecodeUprightDirection(npcMatrix, pNpc->Pos[2]);
    float X = pNpc->Pos[0] - npcMatrix._10 * 30 + 0.5f;
    float Y = pNpc->Pos[1] - npcMatrix._11 * 30 + 0.5f;

    //GetWorld()->m_pPlayer->AddCommandMoveTo(X+.5f,Y+.5f,Z,-1.f,0.f);

    if (g_layerMain->m_formMiddleMap->IsVisible()) {
        g_layerMain->m_formMiddleMap->SetOnMoveto(X / 20, Y / 20);
    }

    PushCallStack(csn_GcActor_AddCommandMoveTo);
    (((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >>
      16) &
     0x7fff);
    GetWorld()->m_pPlayer->AddCommandMoveTo(X, Y, 0, -1.0f, 0);
    GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
    PopCallStack();
}

void GcActor::AddCommandMoveTo(float fX, float fY, float fZ, float fTime, float fDistance) {

    guard;
    // 检查是否有当前命令，如果是 "MoveTo" 类型，则记录当前命令的时间
    int extraTime = 0;
    if (m_pCurCmd) {
        if (m_pCurCmd->eCmd == ACMD_MOVETO) {
            extraTime = m_pCurCmd->fTime;
        }
    }

    // 如果当前命令是 "MoveTo" 类型，则将其标记为完成
    if (m_pCurCmd && m_pCurCmd->eCmd == ACMD_MOVETO)
        CommandFinished();

    // 遍历命令列表，删除所有 "MoveTo" 类型的命令
    std::list<SCmd>::iterator it;
    for (it = m_listCmd.begin(); it != m_listCmd.end();) {
        if ((*it).eCmd == ACMD_MOVETO) {
            it = m_listCmd.erase(it);
        } else {
            it++;
        }
    }

    // 创建一个新的 "MoveTo" 命令并设置其参数
    SCmd cmd;
    cmd.eCmd      = ACMD_MOVETO;        // 设置命令类型为 "MoveTo"
    cmd.f[0]      = fX;                 // 设置目标 X 坐标
    cmd.f[1]      = fY;                 // 设置目标 Y 坐标
    cmd.f[2]      = fZ;                 // 设置目标 Z 坐标
    cmd.fDistance = fDistance;          // 设置移动距离
    cmd.fTime     = fTime + extraTime;  // 设置移动时间，考虑到之前命令的额外时间

    // 将新命令添加到命令列表中
    AddCommand(cmd);

    // 更新检查种子值
    m_iCheckSeed = m_iCheckSeed * 203014L + 3521051L;
    // 重置目标 ID
    m_pLastTargetID = 0;

    unguard;
}

void GcActor::AddCommandTeleport(int iX, int iY, float fDistance) {
    guard;
    SCmd cmd;
    cmd.eCmd      = ACMD_TELEPORT;
    cmd.i[0]      = iX;
    cmd.i[1]      = iY;
    cmd.fDistance = fDistance;
    AddCommand(cmd);
    unguard;
}

void GcActor::AddCommand(ECommands eCommand) {
    guard;
    SCmd cmd;
    cmd.eCmd = eCommand;
    AddCommand(cmd);
    unguard;
}

void GcActor::ClearCommand() {
    guard;
    m_pCurCmd = NULL;
    m_listCmd.clear();
    //#ifdef _DEBUG
    //    if (m_pHUD)
    //    {
    //        m_pHUD->SetGuildText(std::string("ClearCommand"));
    //    }
    //#endif
    unguard;
}

void GcActor::RemoveCurCommand() {
    guard;
    if (m_pCurCmd == NULL)
        return;
    if (m_pCurCmd->cSubType == 1) {
        SCmd* pCmd;
        while (m_listCmd.size()) {
            pCmd = &(m_listCmd.front());
            if (pCmd->cSubType == 1) {
                m_listCmd.pop_front();
            } else if (pCmd->cSubType == 2) {
                m_listCmd.pop_front();
                break;
            } else {
                LOG("RemoveCurCommand: "
                    "命令出错了，在读取子命令[cSubType==1]的时候读到了非法的命令.\n");
                break;
            }
        }
        m_pCurCmd = NULL;
    } else {
        CommandFinished();
    }
    //#ifdef _DEBUG
    //    std::list<SCmd>::iterator itCmd = m_listCmd.begin();
    //    std::string szCmdName;
    //    while (itCmd!=m_listCmd.end())
    //    {
    //        szCmdName += GetCmdName((*itCmd).eCmd);
    //        szCmdName += " ";
    //        itCmd ++;
    //    }
    //    if (m_pHUD)
    //    {
    //        m_pHUD->SetGuildText(szCmdName);
    //        if (!szCmdName.empty()) m_pHUD->SetSpeakText(szCmdName);
    //    }
    //#endif
    unguard;
}

void GcActor::AddCommand(SCmd& vCmd) {
    guard;
    if (GetRideState() == GcActor::ERS_PET)
        return;

    bool  bInsert = false, bHavePRI = false;
    int   iPRI;
    SCmd* pLast;
    bool  bCtrl = (this == GetPlayer());
    guard_name(CmdProcess);
    if (vCmd.eCmd == ACMD_DEATH) {
        goto deadbug;
    }
    while (m_listCmd.size()) {
        if (m_pCurCmd) {
            pLast = m_pCurCmd;
        } else {
            pLast = &(m_listCmd.back());
        }
        if (!bHavePRI) {
            if (bCtrl)
                iPRI = m_PRIPlayer[vCmd.eCmd][pLast->eCmd];
            else
                iPRI = m_PRINetuser[vCmd.eCmd][pLast->eCmd];
        } else {
            bHavePRI = false;
        }
        if (iPRI == -1)  // 直接忽略
        {
            return;
        } else if (iPRI == 1)  // 挤掉
        {
            if (pLast->cSubType == 2) {
                do {
                    m_listCmd.pop_back();
                    if (m_listCmd.size() == 0)
                        break;
                    pLast = &(m_listCmd.back());
                } while (pLast->cSubType == 1);
            } else {
                if (m_listCmd.size()) {
                    m_listCmd.pop_back();
                }
            }
            continue;
        } else if (iPRI == 0)  // 排队
        {
            break;
        } else if (iPRI == 2)  // 插入到第一个位置
        {
            //CommandFinished();
            m_pCurCmd = NULL;
            bInsert   = true;
            break;
        } else if (
            iPRI ==
            3)  // 根据子命令进行排挤，如果这个命令是当前命令就根据当前子命令进行判断，否则直接挤掉
        {
            CHECK(pLast->cSubType == 2);  // 必须是组合命令
            std::list<SCmd>::iterator it      = --m_listCmd.end();
            bool                      bCurCmd = false;

            if (it == m_listCmd.begin()) {
                return;
            }
            it--;

            while ((*it).cSubType == 1) {
                if (it == m_listCmd.begin()) {
                    bCurCmd = true;
                    break;
                }
                it--;
            }
            if (bCurCmd)  // 是当前命令
            {
                pLast = &(*it);
                if (bCtrl)
                    iPRI = m_PRIPlayer[vCmd.eCmd][pLast->eCmd];
                else
                    iPRI = m_PRINetuser[vCmd.eCmd][pLast->eCmd];
                bHavePRI = true;
            } else {
                m_listCmd.erase(++it, m_listCmd.end());
            }
        } else {
            CHECKEX("GcActor::AddCommand 优先级错误");
            break;
        }
    }
    unguard;

deadbug:
    if (m_listCmd.size() > 6) {
#ifdef _DEBUG
        LOG1("GcActor::AddCommand 积累的命令已经太多了[%d个], 前6个命令为:\n", m_listCmd.size());
        int                       iCmd  = 0;
        std::list<SCmd>::iterator itCmd = m_listCmd.begin();
        while (itCmd != m_listCmd.end()) {
            LOG3("    %d: %s [%d]\n", iCmd + 1, GetCmdName((*itCmd).eCmd), (*itCmd).cSubType);
            iCmd++;
            itCmd++;
        }
#endif
        if (this == GetPlayer()) {
            int  movecount = 0;
            SCmd cmd       = m_listCmd.front();
            m_listCmd.clear();
            m_listCmd.push_back(cmd);
            m_pCurCmd = &m_listCmd.front();
        } else {
            m_listCmd.clear();
        }
        AddDoCommandPose();
    }

    if (m_listCmd.size() == 0) {
        m_pCurCmd = NULL;
    }

    std::list<SCmd> bakList;
    if (bInsert) {
        bakList = m_listCmd;
        m_listCmd.clear();
    }

    SCmd cmd = vCmd;
    switch (vCmd.eCmd) {
        case ACMD_TALKNPC:
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_FOLLOW;
            m_listCmd.push_back(cmd);
            cmd.bLoop = false;
            cmd.eCmd  = ACMD_TALKNPC;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_PICKITEM:
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_FOLLOW;
            m_listCmd.push_back(cmd);
            cmd.bLoop = false;
            cmd.eCmd  = ACMD_PICKITEM;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_CATCHPET:
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_FOLLOW;
            m_listCmd.push_back(cmd);
            cmd.bLoop = false;
            cmd.eCmd  = ACMD_CATCHPET;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_COLLECT:
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_COLLECT;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_PLAYTASKITEM:
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_PLAYTASKITEM;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_ATTACK_SOMEBODY: {
            cmd.wSkillID = 0;
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_FOLLOW;
            m_listCmd.push_back(cmd);
            cmd.eCmd = ACMD_ATTACK;
            m_listCmd.push_back(cmd);
            cmd.eCmd     = ACMD_ATTACK_SOMEBODY;
            cmd.cSubType = 2;
            m_listCmd.push_back(cmd);
        } break;
        case ACMD_SKILL_SOMEWHERE:
            cmd.cSubType = 1;
            cmd.bLoop    = false;
            cmd.eCmd     = ACMD_MOVETO;
            m_listCmd.push_back(cmd);
            cmd.bLoop = false;
            cmd.eCmd  = ACMD_USESKILL;
            m_listCmd.push_back(cmd);
            cmd.eCmd     = ACMD_SKILL_SOMEWHERE;
            cmd.cSubType = 2;
            m_listCmd.push_back(cmd);
            break;
        case ACMD_SKILL_SOMEBODY: {
            int actorid  = cmd.dw[0];
            cmd.cSubType = 1;
            cmd.bLoop    = false;
            cmd.eCmd     = ACMD_FOLLOW;
            m_listCmd.push_back(cmd);
            cmd.bLoop = false;
            cmd.eCmd  = ACMD_USESKILL;
            m_listCmd.push_back(cmd);
            SSkill* skill = Skill()->FindSkill(cmd.wSkillID);
            if (skill && skill->bABad && !(skill->IsAddHpSkill)) {
                cmd.bLoop    = true;
                cmd.eCmd     = ACMD_ATTACK_SOMEBODY;
                cmd.cSubType = 2;
#ifdef USE_LUA_CHECK
                int iRand     = rtRandom() & 0x7fff;
                int iValue    = (((iRand * 8561849L + 586468622L) >> 16) & 0x7fff);
                cmd.iNetCheck = (((iRand ^ actorid) << 16) & 0xFFFF0000) | iValue;
#endif
                m_listCmd.push_back(cmd);
            }
        } break;
        case ACMD_DEATH:
            ClearCommand();
            if (m_eNpcType == ENT_USER) {
                cmd.eCmd  = ACMD_PLAY_POSE;
                cmd.dw[0] = GcBaseActor::POSE_DEAD;
                m_listCmd.push_back(cmd);
                break;
            }
            if (m_pModel) {
                if (m_bIsDead == 4) {
                    if (m_pNpc) {
                        if (m_lLastDeadval == 0 && NpcType() != ENT_SPIRIT) {
                            cmd.cSubType = 1;
                            cmd.eCmd     = ACMD_PLAY_POSE;
                            cmd.dw[0]    = GcBaseActor::POSE_DEAD;
                            m_listCmd.push_back(cmd);
                        }
                    }
                    cmd.eCmd           = ACMD_DEATH_ACT;  // dw[0]半透明, dw[1]飞, dw[2]入地
                    m_pModel->DeadType = GetRand(MODEL_DEAD_TYPE_SINK, MODEL_DEAD_TYPE_TRANS_FLY);
                    switch (m_pModel->DeadType) {
                        default:
                        case MODEL_DEAD_TYPE_TRANS_FLY:
                            cmd.dw[0] = 1;
                            cmd.dw[1] = 1;
                            cmd.dw[2] = 0;
                            break;
                        case MODEL_DEAD_TYPE_TRANS_SINK:
                            cmd.dw[0] = 1;
                            cmd.dw[1] = 0;
                            cmd.dw[2] = 1;
                            break;
                        case MODEL_DEAD_TYPE_TRANS:
                            cmd.dw[0] = 1;
                            cmd.dw[1] = 0;
                            cmd.dw[2] = 0;
                            break;
                        case MODEL_DEAD_TYPE_SINK:
                            cmd.dw[0] = 0;
                            cmd.dw[1] = 0;
                            cmd.dw[2] = 1;
                            break;
                    }
                    m_listCmd.push_back(cmd);
                    cmd.eCmd = ACMD_DELETE_SELF;
                    m_listCmd.push_back(cmd);
                } else if (NpcType() != ENT_SPIRIT) {
                    cmd.cSubType = 1;
                    cmd.eCmd     = ACMD_PLAY_POSE;
                    cmd.dw[0]    = GcBaseActor::POSE_DEAD;
                    m_listCmd.push_back(cmd);
                }
                cmd.eCmd     = ACMD_DEATH;
                cmd.cSubType = 2;
                m_listCmd.push_back(cmd);
                m_lLastDeadval = m_cIsDeadNet;
            }
            break;
        case ACMD_BORN:
            if (m_pModel) {
                cmd.cSubType = 1;
                cmd.eCmd     = ACMD_PLAY_POSE;
                cmd.dw[0]    = GcBaseActor::POSE_BORN;
                m_listCmd.push_back(cmd);
                if (strcmp(m_pModel->Act.c_str(), m_pModel->BornAct.c_str())) {
                    cmd.eCmd    = ACMD_CHANGE_ACTOR;
                    cmd.pStr[0] = m_pModel->Act.c_str();
                    m_listCmd.push_back(cmd);
                }
                cmd.eCmd     = ACMD_BORN;
                cmd.cSubType = 2;
                m_listCmd.push_back(cmd);
            }
            break;
        case ACMD_MOVETO: {                               // 处理 'ACMD_MOVETO' 类型的命令
            const float* pPos = GetMatrix()->Position();  // 获取当前位置的坐标
            // 检查当前位置是否接近目标位置
            // fabs 用于计算浮点数的绝对值
            // 比较当前位置和目标位置的 X 和 Y 坐标
            // 如果当前位置接近目标位置（误差小于 10），则跳过该命令

            if (fabs(pPos[0] - cmd.f[0]) < 10.f && fabs(pPos[1] - cmd.f[1]) < 10.f) {
                break;
            }

            m_listCmd.push_back(vCmd);
        } break;
        case ACMD_INTONATE: {
            m_listCmd.push_back(vCmd);
        } break;
        case ACMD_START_TRIGGER: {
            m_listCmd.push_back(vCmd);
        } break;
        case ACMD_SHAPESHIFT: {
            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_PLAY_POSE;
            cmd.pStr[0]  = "skill_q3";
            m_listCmd.push_back(cmd);

            cmd.cSubType = 1;
            cmd.eCmd     = ACMD_SHAPESHIFT;
            m_listCmd.push_back(cmd);
            m_cShapeshift = 1;
        } break;
        default:
            m_listCmd.push_back(vCmd);
            break;
    }
    if (bInsert) {
        m_listCmd.insert(m_listCmd.end(), bakList.begin(), bakList.end());
    }
    //#ifdef _DEBUG
    //    std::list<SCmd>::iterator itCmd = m_listCmd.begin();
    //    std::string szCmdName;
    //    while (itCmd!=m_listCmd.end())
    //    {
    //        szCmdName += GetCmdName((*itCmd).eCmd);
    //        szCmdName += " ";
    //        itCmd ++;
    //    }
    //    if (m_pHUD)
    //    {
    //        m_pHUD->SetGuildText(szCmdName);
    //        if (!szCmdName.empty()) m_pHUD->SetSpeakText(szCmdName);
    //    }
    //#endif
    unguard;
}

void GcActor::AttackSomebody(WORD wAttackID, DWORD dwTargetID, int iNetCheck, float fTime) {
    guard;
    if (m_listCmd.size()) {
        SCmd* pLast = &(m_listCmd.back());
        if (pLast->eCmd == ACMD_ATTACK_SOMEBODY && pLast->dw[0] == dwTargetID) {
            return;  // 上一个命令是相同的命令，就直接返回
        }
    }

    GcActor* pCtrl = this;
    if (GetRideState() == ERS_ACTOR) {
        pCtrl = this->m_pRidePet;
    }

    SCmd     cmd;
    SItemID  item1 = pCtrl->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
    SWeapon* pWea1 = NULL;
    if (ItemID_IsValid(item1))
        pWea1 = (SWeapon*)GetWorld()->m_pItemManager->GetItem(item1.type);

    CheckCallStack(csn_GcActor_AttackSomebody, GetWorld()->m_pItemManager, 400);

    if (GetWorld()->m_pPlayer == this) {
        if (pCtrl == this) {
            cmd.fDistance = m_core.AttackRange.GetValue();
        } else {
            if (m_pNpc) {
                cmd.fDistance = m_pNpc->AttackRange + 40.f;
            } else {
                cmd.fDistance = 80.f;
            }
        }
    }
    //else if(GetWorld()->m_pPet == this && m_pNpc)
    //{
    //	cmd.fDistance = m_pNpc->AttackRange;
    //}
    //else
    //{
    //	// 网络回传命令，直接攻击
    //	cmd.fDistance = 1000;
    //}
    else if (pWea1) {
        cmd.fDistance = pWea1->nAttRange + 40.f;
    } else if (m_pNpc) {
        cmd.fDistance = m_pNpc->AttackRange + 40.f;
    } else {
        cmd.fDistance = 40.f + 40.f;
    }

    //if(GetWorld()->m_pPlayer != this)
    //	SubmitCurMoveCommand();

    cmd.dw[0]       = dwTargetID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.wAttackID   = wAttackID;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = fTime;
    cmd.eCmd      = ACMD_ATTACK_SOMEBODY;
    cmd.bLoop     = true;
    cmd.iNetCheck = iNetCheck;
    //   if(GetWorld()->m_pPlayer == this)
    //       cmd.bLoop = true;
    //else
    //	cmd.bLoop = false;
    AddCommand(cmd);
    unguard;
}

void GcActor::UseIntonate(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ,
                          int iNetCheck, float fTime) {
    guard;
    SCmd    cmd;
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill == NULL)
        return;

    int                         iMyRMaxDest;
    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
    //if (pSkill->wStrSkillSub)
    //{
    //	pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
    //}

    //if (pStrengthenSkill)
    //{
    //	if (pStrengthenSkill->pAttr->iRMaxDest)
    //	{
    //		iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
    //	}else
    //	{
    //		iMyRMaxDest = pSkill->iRMaxDest;
    //	}
    //}else
    //{
    //	iMyRMaxDest = pSkill->iRMaxDest;
    //}

    iMyRMaxDest = pSkill->iRMaxDest;
    ////Tianh 修改加强型技能
    if (!(pSkill->wManyStrSkillSub.empty())) {
        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
             it != pSkill->wManyStrSkillSub.end(); it++) {
            pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(*(it));

            if (pStrengthenSkill) {
                //if (pStrengthenSkill->pAttr->iRMinDest)
                //{
                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                //}
                if (pStrengthenSkill->pAttr->iRMaxDest) {
                    iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
                }
            }
        }
    }

    cmd.fDistance = iMyRMaxDest;  // 距离
    if (cmd.fDistance < 0.f)
        cmd.fDistance = 0.f;
    cmd.f[0]        = fX;
    cmd.f[1]        = fY;
    cmd.f[2]        = fZ;
    cmd.cTargetType = SKILL_TARGET_TYPE_POSITION;
    cmd.wSkillID    = wSkillID;
    cmd.wAttackID   = wAttackID;
    cmd.fTime       = fTime;
    cmd.eCmd        = ACMD_INTONATE;
    cmd.bLoop       = false;
    cmd.iNetCheck   = iNetCheck;
    AddCommand(cmd);
    unguard;
}

// 对人攻击技能吟唱
void GcActor::UseIntonate(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck,
                          float fTime) {
    guard;

    if (m_listCmd.size()) {
        SCmd* pLast = &(m_listCmd.back());
        if (pLast->eCmd == ACMD_INTONATE && pLast->dw[0] == dwTargetID &&
            pLast->wSkillID == wSkillID) {
            return;  // 上一个命令是相同的命令，就直接返回
        }
    }
    SCmd    cmd;
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill == NULL)
        return;

    int                         iMyRMaxDest;
    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
    //if (pSkill->wStrSkillSub)
    //{
    //	pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
    //}
    ////CheckCallStack(csn_GcActor_UseTargetSkill, g_layerMain->m_playerExp, 200);

    //if (pStrengthenSkill)
    //{
    //	if (pStrengthenSkill->pAttr->iRMaxDest)
    //	{
    //		iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
    //	}else
    //	{
    //		iMyRMaxDest = pSkill->iRMaxDest;
    //	}
    //}else
    //{
    //	iMyRMaxDest = pSkill->iRMaxDest;
    //}

    iMyRMaxDest = pSkill->iRMaxDest;

    ////Tianh 修改加强型技能
    if (!(pSkill->wManyStrSkillSub.empty())) {
        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
             it != pSkill->wManyStrSkillSub.end(); it++) {
            pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(*(it));

            if (pStrengthenSkill) {
                //if (pStrengthenSkill->pAttr->iRMinDest)
                //{
                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                //}
                if (pStrengthenSkill->pAttr->iRMaxDest) {
                    iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
                }
            }
        }
    }

    cmd.fDistance = iMyRMaxDest;  // 距离
    if (cmd.fDistance < 0.f)
        cmd.fDistance = 0.f;
    cmd.dw[0]       = dwTargetID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.wSkillID    = wSkillID;
    cmd.wAttackID   = wAttackID;
    cmd.fTime       = fTime;
    cmd.eCmd        = ACMD_INTONATE;
    cmd.bLoop       = false;
    cmd.iNetCheck   = iNetCheck;
    AddCommand(cmd);
    unguard;
}

// 对定点施放技能
void GcActor::UsePositionSkill(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ,
                               int iNetCheck, float fTime) {
    guard;
    SCmd    cmd;
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill == NULL)
        return;

    int                         iMyRMaxDest;
    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
    //if (pSkill->wStrSkillSub)
    //{
    //	pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
    //}

    //if (pStrengthenSkill)
    //{
    //	if (pStrengthenSkill->pAttr->iRMaxDest)
    //	{
    //		iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
    //	}else
    //	{
    //		iMyRMaxDest = pSkill->iRMaxDest;
    //	}
    //}else
    //{
    //	iMyRMaxDest = pSkill->iRMaxDest;
    //}

    iMyRMaxDest = pSkill->iRMaxDest;
    ////Tianh 修改加强型技能
    if (!(pSkill->wManyStrSkillSub.empty())) {
        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
             it != pSkill->wManyStrSkillSub.end(); it++) {
            pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(*(it));

            if (pStrengthenSkill) {
                //if (pStrengthenSkill->pAttr->iRMinDest)
                //{
                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                //}
                if (pStrengthenSkill->pAttr->iRMaxDest) {
                    iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
                }
            }
        }
    }

    cmd.fDistance = iMyRMaxDest;  // 距离
    if (cmd.fDistance < 0.f)
        cmd.fDistance = 0.f;
    cmd.f[0]        = fX;
    cmd.f[1]        = fY;
    cmd.f[2]        = fZ;
    cmd.cTargetType = SKILL_TARGET_TYPE_POSITION;
    cmd.wSkillID    = wSkillID;
    cmd.wAttackID   = wAttackID;
    cmd.fTime       = fTime;
    cmd.eCmd        = ACMD_SKILL_SOMEWHERE;
    cmd.bLoop       = false;
    cmd.iNetCheck   = iNetCheck;

    if (pSkill->wClass == 13)
        DoTrumpSkill(cmd);
    else
        AddCommand(cmd);
    unguard;
}

// 对某人施放技能
void GcActor::UseTargetSkill(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck,
                             float fTime) {
    guard;

    if (m_listCmd.size()) {
        SCmd* pLast = &(m_listCmd.back());
        if (pLast->eCmd == ACMD_SKILL_SOMEBODY && pLast->dw[0] == dwTargetID &&
            pLast->wSkillID == wSkillID) {
            return;  // 上一个命令是相同的命令，就直接返回
        }
    }
    if (wSkillID == SKILL_NOLMAL_ATTACK) {
        int iSum = 0;
#ifdef USE_LUA_CHECK
        int iRand  = rtRandom() & 0x7fff;
        int iValue = (((iRand * 8561849L + 586468622L) >> 16) & 0x7fff);
        iSum       = (((iRand ^ dwTargetID) << 16) & 0xFFFF0000) | iValue;
#endif
        PushCallStack(csn_GcActor_AttackSomebody);
        AttackSomebody(0, dwTargetID, iSum, fTime);
        PopCallStack();
        return;
    }
    SCmd    cmd;
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill == NULL)
        return;

    int                         iMyRMaxDest;
    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
    //if (pSkill->wStrSkillSub)
    //{
    //	pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
    //}
    CheckCallStack(csn_GcActor_UseTargetSkill, g_layerMain->m_playerExp, 200);

    //if (pStrengthenSkill)
    //{
    //	if (pStrengthenSkill->pAttr->iRMaxDest)
    //	{
    //		iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
    //	}else
    //	{
    //		iMyRMaxDest = pSkill->iRMaxDest;
    //	}
    //}else
    //{
    //	iMyRMaxDest = pSkill->iRMaxDest;
    //}

    iMyRMaxDest = pSkill->iRMaxDest;
    ////Tianh 修改加强型技能
    if (!(pSkill->wManyStrSkillSub.empty())) {
        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
             it != pSkill->wManyStrSkillSub.end(); it++) {
            pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(*(it));

            CheckCallStack(csn_GcActor_UseTargetSkill, g_layerMain->m_playerExp, 200);

            if (pStrengthenSkill) {
                //if (pStrengthenSkill->pAttr->iRMinDest)
                //{
                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                //}
                if (pStrengthenSkill->pAttr->iRMaxDest) {
                    iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
                }
            }
        }
    }

    cmd.fDistance = iMyRMaxDest;  // 距离
    if (cmd.fDistance < 0.f)
        cmd.fDistance = 0.f;
    cmd.dw[0]       = dwTargetID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.wSkillID    = wSkillID;
    cmd.wAttackID   = wAttackID;
    cmd.fTime       = fTime;
    cmd.eCmd        = ACMD_SKILL_SOMEBODY;
    cmd.bLoop       = false;
    cmd.iNetCheck   = iNetCheck;

    if (pSkill->wClass == 13)
        DoTrumpSkill(cmd);
    else
        AddCommand(cmd);
    unguard;
}

void GcActor::UseSelfSkill(WORD wAttackID, WORD wSkillID, int iNetCheck, float fTime) {
    guard;
    SCmd    cmd;
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill == NULL)
        return;

    int                         iMyRMaxDest;
    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
    //if (pSkill->wStrSkillSub)
    //{
    //	pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
    //}

    //if (pStrengthenSkill)
    //{
    //	if (pStrengthenSkill->pAttr->iRMaxDest)
    //	{
    //		iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
    //	}else
    //	{
    //		iMyRMaxDest = pSkill->iRMaxDest;
    //	}
    //}else
    //{
    //	iMyRMaxDest = pSkill->iRMaxDest;
    //}

    iMyRMaxDest = pSkill->iRMaxDest;
    ////Tianh 修改加强型技能
    if (!(pSkill->wManyStrSkillSub.empty())) {
        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
             it != pSkill->wManyStrSkillSub.end(); it++) {
            pStrengthenSkill = m_Skill.FindPassiveSkillBySubID(*(it));

            if (pStrengthenSkill) {
                //if (pStrengthenSkill->pAttr->iRMinDest)
                //{
                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                //}
                if (pStrengthenSkill->pAttr->iRMaxDest) {
                    iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
                }
            }
        }
    }

    cmd.fDistance = iMyRMaxDest;  // 距离
    if (cmd.fDistance < 0.f)
        cmd.fDistance = 0.f;
    cmd.dw[0]       = this->ID();
    cmd.cTargetType = SKILL_TARGET_TYPE_NONE;
    cmd.wSkillID    = pSkill->wID;
    cmd.wAttackID   = wAttackID;
    cmd.fTime       = fTime;
    cmd.bLoop       = false;
    cmd.eCmd        = ACMD_USESKILL;
    cmd.iNetCheck   = iNetCheck;

    if (pSkill->wClass == 13)
        DoTrumpSkill(cmd);
    else
        AddCommand(cmd);
    unguard;
}

static unsigned short crctableWorld[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
    0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
    0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
    0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
    0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
    0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
    0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
    0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

#define CRC16_INIT_VALUE 0xffff
#define CRC16_XOR_VALUE 0x0000

void GcActor::DoCommand(SCmd* pCmd) {
    guard;
    if (m_pCurCmd == NULL)
        return;
    GcActor* pActor = NULL;
    if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
        pActor = FindAllActor(pCmd->dw[0]);
        if (pActor == NULL || pActor->m_bIsDead != 0) {
            if (pActor && pActor->m_bIsDead != 4 &&
                    (pCmd->eCmd == ACMD_COLLECT && pActor->m_bIsDead == 2) ||
                ((pCmd->eCmd == ACMD_PICKITEM || pCmd->eCmd == ACMD_FOLLOW) && pActor &&
                 (pActor->m_bIsDead == 1 || pActor->m_bIsDead == 2)) ||
                pCmd->eCmd == ACMD_PLAY_POSE || pCmd->eCmd == ACMD_DEATH_ACT ||
                pCmd->eCmd == ACMD_PLAYTASKITEM) {
            } else if (!pActor || pActor->NpcType() != ENT_USER) {
                RemoveCurCommand();
                return;
            }
        }
    }
    pCmd->fRunTime          = 0.f;
    const RtgMatrix16* pM16 = NULL;
    SSkill*            pSkill;
    const char*        pPoseName = NULL;
    float              fD, fSpeed;
    switch (pCmd->eCmd) {
        case ACMD_SHAPESHIFT: {
            m_cShapeshift = m_cShapeshiftNet;
            if (m_dwShapeshiftModelID) {
                SetModel(m_dwShapeshiftModelID, m_core.HeadModelId);
                mItem.UpdateGraph();
                m_dwShapeshiftModelID = 0;
            }
            ClearCommand();
        } break;
        case ACMD_TALKNPC: {
            if (m_bIsDead) {
                RemoveCurCommand();
                return;
            }
            fD = 0;
            if (pActor) {
                const RtgMatrix16 *pm1, *pm2;
                pm1       = GetMatrix();
                pm2       = pActor->GetMatrix();
                float dis = ((pm1->_30 - pm2->_30) * (pm1->_30 - pm2->_30) +
                             (pm1->_31 - pm2->_31) * (pm1->_31 - pm2->_31));
                fD        = sqrt(dis);
            }
            if (fD <= m_pCurCmd->fDistance + 10) {
                if (this == GetPlayer()) {
                    if (!pActor->m_pNpc || pActor->m_pNpc->Type == 8)
                        return;

                    //pActor->triggerID = triggerID;

                    if (pActor->m_pNpc->Type == 2) {
                        if (pActor->mBaseActor.IsLastPose("wait") ||
                            pActor->mBaseActor.IsLastPose("waiting01") ||
                            pActor->mBaseActor.IsLastPose("waiting02")) {
                            pActor->mBaseActor.PlayPose("talk");
                        }
                    } else if (pActor->m_pNpc->Type == 7 && pActor->triggerID) {
                        if (!CheckCanStartTrigger(pActor->triggerID)) {
                            PlayerFailOperate(R(MSG_CONDITIONSHASNOTREACHED));
                            RemoveCurCommand();
                            return;
                        }
                    }

                    char npcFaction = pActor->m_pNpc->Faction;
                    // 如果是NPC就[触发]
                    ClearCommand();
                    GetWorld()->m_dwActualNPCID      = pActor->m_pNpc->Id;
                    GetWorld()->m_dwInteractiveNPCID = pActor->ID();
                    GetWorld()->m_InteractiveNPCName = pActor->GetName();
                    GetWorld()->OnNetUploadInteractiveNpc(pActor->ID(), "default");  // first
                    RemoveCurCommand();
                }
            } else {
                PlayerFailOperate(R(MSG_DISTANCEISSOFAR));
                RemoveCurCommand();
            }
        } break;
        case ACMD_START_TRIGGER: {
            if (m_bIsDead) {
                RemoveCurCommand();
                return;
            }
            char* pose      = "scene01";
            m_bTriggerClose = 0;
            if (pCmd->dw[0]) {  // 关闭
                pose            = "scene02";
                m_bTriggerClose = 1;
            }
            mBaseActor.PlayPose(pose, false);
            //m_bTriggerClose = !m_lTriggerCloseNet;
        } break;
        case ACMD_CATCHPET: {
            if (this == GetPlayer()) {
                SPet* pPet = g_TablePet.FindPet(pActor->m_pNpc->Id);
                if (!pPet) {
                    RemoveCurCommand();
                    return;
                }
                if (m_core.GradeLev < pPet->NeedCulLev) {
                    PlayerFailOperate(R(MSG_PET_CATCH_ERROR_CULLEV));
                    RemoveCurCommand();
                    return;
                }
                if (Distance(pActor) > 120) {
                    PlayerFailOperate(R(MSG_PET_CATCH_ERROR_DISTANCE));
                    RemoveCurCommand();
                    return;
                }
                if (mItem.m_PetBag.GetBagItemNum() >= 3 + m_core.GradeLev / 20) {
                    PlayerFailOperate(R(MSG_PET_CATCH_ERROR_BAGFULL));
                    RemoveCurCommand();
                    return;
                }
                if (!m_Skill.FindActiveSkill(CATCH_PET_SKILL)) {
                    PlayerFailOperate(R(MSG_SKILL_NEEDCATCHPET));
                    RemoveCurCommand();
                    return;
                }
                CPetNetHelper::NetSend_c2r_catch_pet_start(pActor->ID());
            }

            m_dwCatchPet        = pActor->ID();
            pActor->m_dwCatchMe = ID();
            mBaseActor.PlayPose(GcBaseActor::POSE_MAKE, true);
        } break;
        case ACMD_PICKITEM: {
            if (m_bIsDead) {
                RemoveCurCommand();
                return;
            }
            m_bInCallPet = false;
            fD           = m_pCurCmd->fDistance;
            if (Distance2(pActor) < (fD * fD) &&
                (pActor->m_bIsDead == 1 || pActor->m_bIsDead == 2 || pActor->m_bIsDead == 3)) {
                mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                if (m_pCurCmd == NULL) {
                    break;
                }
                if (this == GetPlayer()) {
                    if (m_cPickType == 0) {
                        SendPickItemType(pActor);
                    } else {
                        GetPlayer()->SendPickItemId(ItemID_CreateInvalid(), 1);
                    }
                    RemoveCurCommand();
                } else {
                }
            } else {
                RemoveCurCommand();
            }
        } break;
        case ACMD_COLLECT: {
            m_bInCallPet = false;
            fD           = m_pCurCmd->fDistance + 10.f;
            if (pActor && Distance2(pActor) < (fD * fD) && pActor->m_bIsDead == 2 && m_bIsCollect) {
                if (m_pCurCmd == NULL) {
                    break;
                }
                mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                GcBaseActor::EPoses pose = GcBaseActor::POSE_DIG;
                if (pActor->m_pNpc) {
                    if (pActor->m_pNpc->Type == 4)
                        pose = GcBaseActor::POSE_GATHER;
                    else if (pActor->m_pNpc->Type == 5)
                        pose = GcBaseActor::POSE_DIG;
                    else
                        pose = GcBaseActor::POSE_SCALP;
                }
                pPoseName = mBaseActor.PlayPose(pose, true);
            } else {
                //CommandFinished();
                RemoveCurCommand();
            }
        } break;
        case ACMD_PLAYTASKITEM: {
            if (bActivity) {
                break;
            } else {
                fD = m_pCurCmd->fDistance + 10.f;
                if (pActor && Distance2(pActor) < (fD * fD) && pActor->m_bIsDead == 2 &&
                    m_bIsCollect) {
                    if (m_pCurCmd == NULL) {
                        break;
                    }
                    mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                    if (pActor->m_pNpc) {
                        if (pActor->m_pNpc->Type == 9) {
                            GcBaseActor::EPoses pose = GcBaseActor::POSE_MAKE;
                            mBaseActor.PlayPose(pose, true);
                        }
                    }
                } else {
                    RemoveCurCommand();
                }
            }
        } break;
        case ACMD_INTONATE: {
            m_bInCallPet = false;
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                m_SCmd      = *pCmd;
                m_SCmd.eCmd = ACMD_SKILL_SOMEWHERE;
            } else {
                m_SCmd      = *pCmd;
                m_SCmd.eCmd = ACMD_SKILL_SOMEBODY;
            }

            pSkill = Skill()->FindSkill(pCmd->wSkillID);
            if (pSkill) {
                if (pSkill->bAlwaysUsable == 0 && !IsAbiUseSkill()) {
                    CommandFinished();
                    break;
                }
                WORD wCurAttackID = 0;
                bool bCanUseSkill = true;
                bool bIsLocalCmd  = (pCmd->fTime < 0.f);

                m_iNeedIntonateTime = pSkill->iIntonateTime;
                m_dwIntonateTime    = rtGetMilliseconds();
                //m_dwIntonateTime = pSkill->iIntonateTime;

                if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                    mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                    mBaseActor.FaceTo(m_pCurCmd->f[0], m_pCurCmd->f[1]);
                }

                if (m_pCurCmd == NULL) {
                    break;
                }

                SItemID  Item1, Item2;
                SWeapon *pWeaClass1, *pWeaClass2;
                Item1 = mItem.GetEquipItem(CItemContainer::WEAPON_1, true);
                Item2 = mItem.GetEquipItem(CItemContainer::WEAPON_2, true);
                pWeaClass1 =
                    (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));
                pWeaClass2 =
                    (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item2.type));

                if (this == GetPlayer()) {
                    bCanUseSkill = false;
                    if (pCmd->cTargetType == SKILL_TARGET_TYPE_NONE) {
                        if (m_Skill.UseSkill(pSkill->wID))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        if (m_Skill.UseSkill(pSkill->wID, pActor->ID()))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        if (m_Skill.UseSkill(pSkill->wID, pCmd->f))
                            bCanUseSkill = true;
                    }

                    if (!bCanUseSkill) {
                        LOG1("使用技能失败，原因：%s\n", m_Skill.GetLastErrInfo());
                        RemoveCurCommand();
                        break;
                    }
                    m_bIntonate = true;

                    SAttack* pAttack = NewAttack();

                    pAttack->cAnswer = 1;
                    pAttack->pSkill  = pSkill;
                    pAttack->cTarget = pCmd->cTargetType;
                    if (pAttack->cTarget == SKILL_TARGET_TYPE_ACTOR) {
                        pAttack->dwTargetID = pCmd->dw[0];
                    } else if (pAttack->cTarget == SKILL_TARGET_TYPE_POSITION) {
                        pAttack->f[0] = pCmd->f[0];
                        pAttack->f[1] = pCmd->f[1];
                        pAttack->f[2] = pCmd->f[2];
                    }
                    wCurAttackID       = m_wAttackID++;
                    pAttack->wAttackID = wCurAttackID;
                    if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->dw[0] & 0x000000FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x0000FF00) >> 8)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x00FF0000) >> 16)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0xFF000000) >> 24)];
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        DWORD dw1, dw2, dw3;
                        dw1                     = pCmd->f[0];
                        dw2                     = pCmd->f[1];
                        dw3                     = pCmd->f[2];
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw1 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0xFF000000) >> 24)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw2 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0xFF000000) >> 24)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw3 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0xFF000000) >> 24)];
                        pCmd->dw[0]  = dw1;
                        pCmd->dw[1]  = dw2;
                        pCmd->dw[2]  = dw3;
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    } else {
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    }
                    if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        GetWorld()->OnNetUploadAttackTarget(pCmd->wSkillID, pAttack->wAttackID,
                                                            pCmd->dw[0], pCmd, pCmd->iNetCheck);
                    } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        GetWorld()->OnNetUploadAttackPosition(pCmd->wSkillID, pAttack->wAttackID,
                                                              pCmd->dw, pCmd, 1);
                    } else {
                        GetWorld()->OnNetUploadAttackSelf(pCmd->wSkillID, pAttack->wAttackID, pCmd,
                                                          2);
                    }
                } else if (this == GetPlayerPet()->GetPetActor()) {
                    bCanUseSkill = false;
                    if (pCmd->cTargetType == SKILL_TARGET_TYPE_NONE) {
                        if (m_Skill.UseSkill(pSkill->wID))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        if (m_Skill.UseSkill(pSkill->wID, pActor->ID()))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        if (m_Skill.UseSkill(pSkill->wID, pCmd->f))
                            bCanUseSkill = true;
                    }

                    if (!bCanUseSkill) {
                        LOG1("使用技能失败，原因：%s\n", m_Skill.GetLastErrInfo());
                        RemoveCurCommand();
                        break;
                    }
                    wCurAttackID = m_pCurCmd->wAttackID;
                    if (!AttackAnswer(wCurAttackID, pPoseName)) {
                        LOG("DoCommand:ACMD_ATTACK "
                            "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
                    }
                    m_curAttackId = m_pCurCmd->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                } else {
                    // 如果非玩家控制角色就说明这是一个网络攻击命令，
                    //     这里要查找出这个命令，并且响应他，在PoseBreak或者PoseEnd中删除这个命令
                    //     如果这里找不到这个命令，就说明这个命令由于某种原因被删除了
                    wCurAttackID = m_pCurCmd->wAttackID;
                    if (!AttackAnswer(wCurAttackID, pPoseName)) {
                        LOG("DoCommand:ACMD_ATTACK "
                            "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
                    }
                    m_curAttackId = m_pCurCmd->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                    //mBaseActor.PlayPose(GcBaseActor::POSE_WALK, true);
                }
            } else {
                LOG1("DoCommand: 使用了一个找不到的技能[ID=%d].\n", pCmd->wSkillID);
            }
        } break;
        case ACMD_TELEPORT:
            //if ( m_bIsDead > 0 )
            //{
            //	ClearCommand();
            //	return;
            //}
            m_bInCallPet = false;
            mBaseActor.MoveToTerrainNow(pCmd->i[0], pCmd->i[1], true);
            m_dwDummyID = rtRandom();
            if (GetPlayer() == this && g_pScene)
                g_pScene->m_camMatPrev.Unit();
            CommandFinished();  // 当场完成

            break;
        case ACMD_MOVETO:
            if (m_bIsDead > 0) {
                m_bDoMoveCmd = false;
                ClearCommand();
                return;
            }
            if (!IsAbiMove()) {
                m_bDoMoveCmd = false;
                CommandFinished();
                break;
            }
            if (bActivity) {
                g_layerMain->m_formTimeBar->End(false);
            }
            if (m_bIsCollect && this != GetPlayer()) {
                m_bIsCollect = false;
                SetIsTaskCollect(false);
                ChangeCollectTool(false);
            }
            pM16 = mBaseActor.GetMatrix();
            fD   = fsqrt((m_pCurCmd->f[0] - pM16->_30) * (m_pCurCmd->f[0] - pM16->_30) +
                         (m_pCurCmd->f[1] - pM16->_31) * (m_pCurCmd->f[1] - pM16->_31));
            if (fD > m_pCurCmd->fDistance) {
                m_bDoMoveCmd = true;
                fSpeed       = 1.f;
                if (m_pModel) {
                    if (m_pCurCmd->fTime < 0) {
                        if (m_eRideState == GcActor::ERS_ACTOR) {
                            // fSpeed = 80.f/(m_pRidePet->m_pModel?m_pRidePet->m_pModel->MoveSpeed:80.f);
                            fSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue() /
                                     GetWorld()->m_pPet->m_pModel->MoveSpeed;
                        } else {
                            // fSpeed = 80.f/m_pModel->MoveSpeed;
                            fSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue() /
                                     GetWorld()->m_pPlayer->m_pModel->MoveSpeed;
                        }
                    } else {
                        if ((fD - m_pCurCmd->fDistance) / m_pCurCmd->fTime > 80.f * 2.f) {
                            fSpeed = (80.f * 2.f) / m_pModel->MoveSpeed;
                        } else {
                            fSpeed = ((fD - m_pCurCmd->fDistance) / m_pCurCmd->fTime) /
                                     m_pModel->MoveSpeed;
                        }
                    }
                    mBaseActor.StartMove();
                }
                if (GetGraph()->p()->ResourceReady()) {
                    if (m_bTrace || m_eRideState == GcActor::ERS_ACTOR || NpcType() == ENT_PET ||
                        NpcType() == ENT_CALLNPC) {
                        mBaseActor.PlayPose(GcBaseActor::POSE_RUN, true, NULL, fSpeed);
                    } else
                        mBaseActor.PlayPose(GcBaseActor::POSE_WALK, true, NULL, fSpeed);
                }
            } else {
                m_bDoMoveCmd = false;
                CommandFinished();  // 当场完成
            }
            break;
        case ACMD_FOLLOW:
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            if (!IsAbiMove()) {
                CommandFinished();
                break;
            }
            m_bInCallPet = false;
            // fD = Distance(pActor);
            fD = 0;
            if (pActor) {
                const RtgMatrix16 *pm1, *pm2;
                pm1       = GetMatrix();
                pm2       = pActor->GetMatrix();
                float dis = ((pm1->_30 - pm2->_30) * (pm1->_30 - pm2->_30) +
                             (pm1->_31 - pm2->_31) * (pm1->_31 - pm2->_31));
                fD        = sqrt(dis);
            }
            if (fD > m_pCurCmd->fDistance) {
                fSpeed = 1.f;
                if (m_pModel) {
                    if (m_pCurCmd->fTime < 0) {
                        // fSpeed = 80.f/m_pModel->MoveSpeed;
                        if (m_eRideState == GcActor::ERS_ACTOR) {
                            // fSpeed = 80.f/(m_pRidePet->m_pModel?m_pRidePet->m_pModel->MoveSpeed:80.f);
                            fSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue() /
                                     GetWorld()->m_pPet->m_pModel->MoveSpeed;
                        } else {
                            // fSpeed = 80.f/m_pModel->MoveSpeed;
                            fSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue() /
                                     GetWorld()->m_pPlayer->m_pModel->MoveSpeed;
                        }
                    } else {
                        if ((fD - m_pCurCmd->fDistance) / m_pCurCmd->fTime > 80.f * 2.f) {
                            fSpeed = (80.f * 2.f) / m_pModel->MoveSpeed;
                        } else {
                            fSpeed = ((fD - m_pCurCmd->fDistance) / m_pCurCmd->fTime) /
                                     m_pModel->MoveSpeed;
                        }
                    }
                }
                if (m_eRideState == GcActor::ERS_ACTOR || NpcType() == ENT_PET ||
                    NpcType() == ENT_CALLNPC) {
                    mBaseActor.PlayPose(GcBaseActor::POSE_RUN, true, NULL, fSpeed);
                } else
                    mBaseActor.PlayPose(GcBaseActor::POSE_WALK, true, NULL, fSpeed);
            } else {
                CommandFinished();  // 当场完成
            }
            break;
        case ACMD_ATTACK:
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            if (!IsAbiNormalAttack()) {
                CommandFinished();  // 当场完成
                break;
            }
            m_bInCallPet = false;
            fD           = m_pCurCmd->fDistance + 10.f;
            if (Distance2(pActor) < (fD * fD) && pActor->m_bIsDead == 0) {
                mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                if (GetProb(0.01f))
                    pPoseName = mBaseActor.PlayPose(GcBaseActor::POSE_BASH, false);
                else
                    pPoseName = mBaseActor.PlayPose(GcBaseActor::POSE_ATTACK, false);
                if (m_pCurCmd == NULL) {
                    break;
                }
                if (this == GetPlayer()) {
                    // 如果是玩家控制的角色，就添加一个攻击命令，这个命令会再PoseBreak或者PoseEnd中删除
                    SAttack* pAttack   = NewAttack();  //new SAttack;
                    pAttack->cAnswer   = 1;
                    pAttack->pSkill    = 0;
                    pAttack->pPoseName = pPoseName;
                    pAttack->cTarget   = pCmd->cTargetType;
                    if (pAttack->cTarget == SKILL_TARGET_TYPE_ACTOR) {
                        pAttack->dwTargetID = pCmd->dw[0];
                    } else if (pAttack->cTarget == SKILL_TARGET_TYPE_POSITION) {
                        pAttack->f[0] = pCmd->f[0];
                        pAttack->f[1] = pCmd->f[1];
                        pAttack->f[2] = pCmd->f[2];
                    }
                    pAttack->wAttackID = m_wAttackID++;
                    if (m_wAttackID == 0)
                        m_wAttackID = 1;

                    unsigned short crcvalue = CRC16_INIT_VALUE;
                    crcvalue                = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                    crcvalue =
                        (crcvalue << 8) ^
                        crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ (pCmd->dw[0] & 0x000000FF)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x0000FF00) >> 8)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x00FF0000) >> 16)];
                    crcvalue = (crcvalue << 8) ^
                               crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0xFF000000) >> 24)];
                    pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;

                    m_listAttack.push_back(pAttack);
                    if (m_listAttack.size() > 10) {
                        LOG1("GcActor::DoCommand "
                             "攻击缓冲区中的缓冲数据太多了[%d个].将会直接清除一部分\n",
                             m_listAttack.size());
                    }
                    m_bIsAttack       = true;
                    m_bWantStopAttack = false;

                    // update position first
                    GetWorld()->m_NetSynachro.OnUploadPosition(0);

                    // spec checksum
#ifdef USE_LUA_CHECK
                    //int sum;
                    //Lua()->CallLFunc("nk","i:i",pAttack->dwTargetID, &sum);
#endif
                    GetWorld()->OnNetUploadAttackTarget(pCmd->wSkillID, pAttack->wAttackID,
                                                        pCmd->dw[0], pCmd,
                                                        pCmd->iNetCheck);  // 普通攻击

                    // 记录攻击参数，本地累加关键帧
                    m_curAttackId = pAttack->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                } else {
                    // 如果非玩家控制角色就说明这是一个网络攻击命令，
                    //     这里要查找出这个命令，并且响应他，在PoseBreak或者PoseEnd中删除这个命令
                    //     如果这里找不到这个命令，就说明这个命令由于某种原因被删除了
                    if (!AttackAnswer(m_pCurCmd->wAttackID, pPoseName)) {
                        LOG("DoCommand:ACMD_ATTACK "
                            "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
                    }
                    m_curAttackId = m_pCurCmd->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                }
            } else {
                CommandFinished();
                // RemoveCurCommand();
            }
            break;
        case ACMD_USESKILL:
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            m_bInCallPet = false;
            if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                //if (Distance2(pActor) > (fD*fD))
                //{
                //    CommandFinished();
                //    // RemoveCurCommand();
                //    break;
                //}
            } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                fD   = m_pCurCmd->fDistance;
                pM16 = mBaseActor.GetMatrix();
                if (((m_pCurCmd->f[0] - pM16->_30) * (m_pCurCmd->f[0] - pM16->_30) +
                     (m_pCurCmd->f[1] - pM16->_31) * (m_pCurCmd->f[1] - pM16->_31)) > (fD * fD)) {
                    CommandFinished();
                    // RemoveCurCommand();
                    break;
                }
            }
            pSkill = Skill()->FindSkill(pCmd->wSkillID);
            if (pSkill) {
                if (pSkill->bAlwaysUsable == 0 && !IsAbiUseSkill()) {
                    CommandFinished();
                    break;
                }
                WORD wCurAttackID = 0;
                bool bCanUseSkill = true;
                bool bIsLocalCmd  = (pCmd->fTime < 0.f);

                //// 自己MP不够时自动喝蓝瓶
                //if (this == GetWorld()->m_pPlayer)
                //{
                //	GcSkill* pS = (GcSkill*)m_Skill.FindSkill(pSkill->wID);
                //	if (pS)
                //		pS->AutoEatMPBottle();
                //}

                if (bIsLocalCmd)  // 如果时间为-1就是本地发出来的命令，本地发出的命令就需要上传到服务器
                {
                    bCanUseSkill = false;
                    if (pCmd->cTargetType == SKILL_TARGET_TYPE_NONE) {
                        if (m_Skill.UseSkill(pSkill->wID))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        if (m_Skill.UseSkill(pSkill->wID, pActor->ID()))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        if (m_Skill.UseSkill(pSkill->wID, pCmd->f))
                            bCanUseSkill = true;
                    }
                }
                if (!bCanUseSkill) {
                    LOG1("使用技能失败，原因：%s\n", m_Skill.GetLastErrInfo());
                    RemoveCurCommand();
                    if (pSkill->iIsIntonate) {
                        GetWorld()->OnNetUploadBreakIntonate();
                    }
                    break;
                }

                if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                    mBaseActor.FaceTo(pActor->GetMatrix()->_30, pActor->GetMatrix()->_31);
                } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                    mBaseActor.FaceTo(m_pCurCmd->f[0], m_pCurCmd->f[1]);
                }
                if (pSkill->wClass != 13)
                    pPoseName = mBaseActor.PlayPose(GcBaseActor::POSE_ATTACK, false, pSkill);

                if (m_pCurCmd == NULL) {
                    break;
                }

                SItemID  Item1, Item2;
                SWeapon *pWeaClass1, *pWeaClass2;
                Item1 = mItem.GetEquipItem(CItemContainer::WEAPON_1, true);
                Item2 = mItem.GetEquipItem(CItemContainer::WEAPON_2, true);
                pWeaClass1 =
                    (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));
                pWeaClass2 =
                    (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item2.type));

                if (pSkill->wClass == 13) {
                    // 法宝技能
                    if (pWeaClass1 && mBaseActor.m_pTrump) {
                        if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                            mBaseActor.m_pTrump->DoSkillAttack(true, 0, pCmd->f, pSkill);
                        } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                            mBaseActor.m_pTrump->DoSkillAttack(false, pActor->ID(), NULL, pSkill);
                        } else {
                            pM16 = mBaseActor.GetMatrix();
                            mBaseActor.m_pTrump->DoSkillAttack(true, 0, &(pM16->_30), pSkill);
                        }
                    } else {
                    }
                } else {
                    if (mBaseActor.m_pWeapon) {
                        if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                            mBaseActor.m_pWeapon->DoSkillAttack(true, 0, pCmd->f, pSkill);
                        } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                            mBaseActor.m_pWeapon->DoSkillAttack(false, pActor->ID(), NULL, pSkill);
                        } else {
                            pM16 = mBaseActor.GetMatrix();
                            mBaseActor.m_pWeapon->DoSkillAttack(true, 0, &(pM16->_30), pSkill);
                        }
                    }

                    const char* szREffect = NULL;  // 普通技能

                    if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                        (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                        szREffect = pSkill->szREffect2;
                    else
                        szREffect = pSkill->szREffect1;

                    if (szREffect) {
                        const char* pPlayEffectLink =
                            pSkill->szREfLink[0] ? pSkill->szREfLink : NULL;
                        mBaseActor.PlayEffect(szREffect, true, pPlayEffectLink,
                                              RtgVertex3(0.f, 0.f, 0.f), false,
                                              mBaseActor.m_fLastPoseSpeed);
                    }
                }
                if (pSkill->wSubID == 184) {
                    std::list<SCmd>::iterator it = m_listCmd.begin();
                    for (; it != m_listCmd.end();) {
                        if (&(*it) != m_pCurCmd) {
                            it = m_listCmd.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    m_cShapeshift = 1;
                }

                m_vTelePoint.x = GetMatrix()->_30;
                m_vTelePoint.y = GetMatrix()->_31;
                m_bTeleEvent   = true;

                if (bIsLocalCmd)  // 如果时间为-1就是本地发出来的命令，本地发出的命令就需要上传到服务器
                {
                    // 如果是玩家控制的角色，就添加一个攻击命令，这个命令会再PoseBreak或者PoseEnd中删除

                    // 特殊技能使用
                    //if(pSkill->wSubID == 24 || pSkill->wSubID == 39)
                    //{
                    //	MagicWeaponBeginAddMp();
                    //	CommandFinished(); // 当场完成
                    //	break;
                    //}
                    if (pSkill->wSubID == 1054 /* || pSkill->wSubID == 55*/) {
                        MagicWeaponBeginAddEle();
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 583)  // 铸兵术
                    {
                        LivingSkillProduce(SKILL_PRODUCE_WEAPON);
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 588)  // 铸甲术
                    {
                        LivingSkillProduce(SKILL_PRODUCE_EQUIP);
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 584)  // 炼丹术
                    {
                        LivingSkillProduce(SKILL_PRODUCE_LEECHDOM);
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 585)  // 炼金术
                    {
                        LivingSkillProduce(SKILL_PRODUCE_CHARM);
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 586)  // 融合术
                    {
                        LivingSkillSyncretize();
                        CommandFinished();  // 当场完成
                        break;
                    } else if (pSkill->wSubID == 587)  // 分解术
                    {
                        //LivingSkillItemDecompound();
                        LivingSkillProduce(SKILL_DECOMPOUND_ITEM);
                        CommandFinished();  // 当场完成
                        break;
                    } /*else if(pSkill->wSubID == 590) // 摆摊
				{
					g_layerMain->m_formPlayerShop->OnStartInitShop();
					CommandFinished(); // 当场完成
					break;
				}*/

                    SAttack* pAttack = NewAttack();

                    pAttack->cAnswer   = 1;
                    pAttack->pSkill    = pSkill;
                    pAttack->pPoseName = pPoseName;
                    pAttack->cTarget   = pCmd->cTargetType;
                    if (pAttack->cTarget == SKILL_TARGET_TYPE_ACTOR) {
#ifdef USE_LUA_CHECK
                        //Lua()->CallLFunc("scc","i",pAttack);
#endif
                        pAttack->dwTargetID = pCmd->dw[0];
                    } else if (pAttack->cTarget == SKILL_TARGET_TYPE_POSITION) {
                        pAttack->f[0] = pCmd->f[0];
                        pAttack->f[1] = pCmd->f[1];
                        pAttack->f[2] = pCmd->f[2];
                    }
                    wCurAttackID       = m_wAttackID++;
                    pAttack->wAttackID = wCurAttackID;
                    if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->dw[0] & 0x000000FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x0000FF00) >> 8)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0x00FF0000) >> 16)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->dw[0] & 0xFF000000) >> 24)];
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        DWORD dw1, dw2, dw3;
                        dw1                     = pCmd->f[0];
                        dw2                     = pCmd->f[1];
                        dw3                     = pCmd->f[2];
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw1 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0xFF000000) >> 24)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw2 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0xFF000000) >> 24)];
                        crcvalue =
                            (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw3 & 0x000000FF)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x0000FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x00FF0000) >> 16)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0xFF000000) >> 24)];
                        pCmd->dw[0]  = dw1;
                        pCmd->dw[1]  = dw2;
                        pCmd->dw[2]  = dw3;
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    } else {
                        unsigned short crcvalue = CRC16_INIT_VALUE;
                        crcvalue                = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pCmd->wSkillID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pCmd->wSkillID & 0x0FF00) >> 8)];
                        crcvalue = (crcvalue << 8) ^
                                   crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                        crcvalue =
                            (crcvalue << 8) ^
                            crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                        pCmd->wCheck = crcvalue ^ CRC16_XOR_VALUE;
                    }
                    if (m_wAttackID == 0)
                        m_wAttackID = 1;
                    m_listAttack.push_back(pAttack);

                    if (m_listAttack.size() > 10) {
                        LOG1("GcActor::DoCommand "
                             "攻击缓冲区中的缓冲数据太多了[%d个].将会直接清除一部分\n",
                             m_listAttack.size());
                    }
                    if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        // spec checksum
#ifdef USE_LUA_CHECK
                        //int sum;
                        //Lua()->CallLFunc("sk","i:i",pActor, &sum);
#endif
                        GetWorld()->OnNetUploadAttackTarget(pCmd->wSkillID, pAttack->wAttackID,
                                                            pCmd->dw[0], pCmd, pCmd->iNetCheck);
                    } else if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        GetWorld()->OnNetUploadAttackPosition(pCmd->wSkillID, pAttack->wAttackID,
                                                              pCmd->dw, pCmd, 1);
                    } else {
                        GetWorld()->OnNetUploadAttackSelf(pCmd->wSkillID, pAttack->wAttackID, pCmd,
                                                          2);
                    }

                    // 记录攻击参数，本地累加关键帧
                    m_curAttackId = pAttack->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                } else if (this == GetPlayerPet()->GetPetActor()) {
                    bCanUseSkill = false;
                    if (pCmd->cTargetType == SKILL_TARGET_TYPE_NONE) {
                        if (m_Skill.UseSkill(pSkill->wID))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                        if (m_Skill.UseSkill(pSkill->wID, pActor->ID()))
                            bCanUseSkill = true;
                    } else if (pCmd->cTargetType == SKILL_TARGET_TYPE_POSITION) {
                        if (m_Skill.UseSkill(pSkill->wID, pCmd->f))
                            bCanUseSkill = true;
                    }

                    if (!bCanUseSkill) {
                        LOG1("使用技能失败，原因：%s\n", m_Skill.GetLastErrInfo());
                        RemoveCurCommand();
                        break;
                    }
                    wCurAttackID = m_pCurCmd->wAttackID;
                    if (!AttackAnswer(wCurAttackID, pPoseName)) {
                        LOG("DoCommand:ACMD_ATTACK "
                            "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
                    }
                    m_curAttackId = m_pCurCmd->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                } else {
                    // 如果非玩家控制角色就说明这是一个网络攻击命令，
                    //     这里要查找出这个命令，并且响应他，在PoseBreak或者PoseEnd中删除这个命令
                    //     如果这里找不到这个命令，就说明这个命令由于某种原因被删除了
                    wCurAttackID = m_pCurCmd->wAttackID;
                    if (!AttackAnswer(wCurAttackID, pPoseName)) {
                        LOG("DoCommand:ACMD_ATTACK "
                            "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
                    }
                    m_curAttackId = m_pCurCmd->wAttackID;
                    if (pPoseName)
                        m_curAttackPose = pPoseName;
                    m_curAttackEvent = 0;
                }
                if (pPoseName == NULL && wCurAttackID != 0) {
                    OnAttackEvent(wCurAttackID);
                    AttackEnd(wCurAttackID);
                }
            } else {
                LOG1("DoCommand: 使用了一个找不到的技能[ID=%d].\n", pCmd->wSkillID);
            }
            break;
        case ACMD_UNDER_ATTACK: {
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            m_bInCallPet = false;
            if (mBaseActor.PlayPose(GcBaseActor::POSE_HURT, false) == NULL) {
                CommandFinished();  // 当场完成
            }
        } break;
        case ACMD_PLAY_POSE:
            if (m_bIsDead > 0) {
                if ((GcBaseActor::EPoses)pCmd->dw[0] != GcBaseActor::POSE_DEAD &&
                    (GcBaseActor::EPoses)pCmd->dw[0] != GcBaseActor::POSE_RELIVE) {
                    ClearCommand();
                    return;
                }
            }
            if (NpcType() == ENT_USER)
                m_eIdlePose = GcBaseActor::POSE_IDLE;
            if (pCmd->dw[0]) {
                if (mBaseActor.PlayPose((GcBaseActor::EPoses)pCmd->dw[0], false) == NULL) {
                    CommandFinished();  // 当场完成
                }
            } else if (pCmd->pStr[0]) {
                if (!mBaseActor.PlayPose((char*)pCmd->pStr[0], false)) {
                    CommandFinished();
                }
            }
            break;
        case ACMD_CHANGE_ACTOR:
            if (pCmd->pStr[0]) {
                if (pCmd->pStr[0][0]) {
                    mBaseActor.SetGraph(0, pCmd->pStr[0]);
                }
            }
            CommandFinished();  // 当场完成
            break;
        case ACMD_UNEQUIP_ALL:
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            CommandFinished();  // 当场完成
            break;
        case ACMD_EQUIP_ALL:
            if (m_bIsDead > 0) {
                ClearCommand();
                return;
            }
            CommandFinished();  // 当场完成
            break;
        case ACMD_DEATH_ACT:
            m_bInCallPet = false;
            if (NpcType() == ENT_USER) {
                m_eIdlePose = GcBaseActor::POSE_IDLE;
            }
            if (!mBaseActor.GetGraph()->p()->IsPlayingPose() && NpcType() != ENT_SPIRIT) {
                mBaseActor.PlayPose(GcBaseActor::POSE_BODY, false);
            }
            mBaseActor.GetGraph()->EnableShadow(false);
            break;
        case ACMD_DELETE_SELF:
            m_bDeleteSelf = true;
            m_bInCallPet  = false;
            CommandFinished();  // 当场完成
            break;
        default:
            RemoveCurCommand();
            break;
    }
    unguard;
}

void GcActor::SubmitCurMoveCommand() {
    if (m_pCurCmd == NULL)
        return;
    GcActor* pActor = NULL;
    if (m_pCurCmd->eCmd != ACMD_MOVETO && m_pCurCmd->eCmd != ACMD_FOLLOW) {
        return;
    }
    RtgVertex3 vSource, vTarget, vDif;
    if (m_pCurCmd->cTargetType == SKILL_TARGET_TYPE_ACTOR) {
        pActor = FindActor(m_pCurCmd->dw[0]);
        if (pActor == NULL) {
            RemoveCurCommand();
            return;
        }
        vTarget = pActor->GetMatrix()->Position();
    } else {
        vTarget = m_pCurCmd->f;
    }
    vSource = GetMatrix()->Position();
    vDif    = vTarget - vSource;
    // vTarget = vSource + vDif * (m_pCurCmd->fDistance / vDif.Length());
    mBaseActor.MoveToTerrainNow(vTarget.x, vTarget.y, true);
    CommandFinished();
}

float GcActor::Distance(float x, float y) {
    float              fD  = 0.f;
    const RtgMatrix16* pm1 = GetMatrix();
    float              dis = ((pm1->_30 - x) * (pm1->_30 - x) + (pm1->_31 - y) * (pm1->_31 - y));
    fD                     = sqrt(dis);
    return fD;
}

float GcActor::Distance(GcActor* pActor) {
    guard;
    const RtgMatrix16 *pm1, *pm2;
    CHECK(pActor != NULL);
    pm1 = GetMatrix();
    pm2 = pActor->GetMatrix();

    float dis = ((pm1->_30 - pm2->_30) * (pm1->_30 - pm2->_30) +
                 (pm1->_31 - pm2->_31) * (pm1->_31 - pm2->_31));
    dis       = sqrt(dis) - pActor->m_pModel->ModelRadius;
    if (dis < 0)
        dis = 0;
    return dis;
    unguard;
}

float GcActor::Distance2(GcActor* pActor) {
    guard;
    const RtgMatrix16 *pm1, *pm2;
    CHECK(pActor != NULL);
    pm1       = GetMatrix();
    pm2       = pActor->GetMatrix();
    float dis = ((pm1->_30 - pm2->_30) * (pm1->_30 - pm2->_30) +
                 (pm1->_31 - pm2->_31) * (pm1->_31 - pm2->_31));
    if (dis < 0 || pActor->m_pModel == NULL)
        return 0.f;
    dis = sqrt(dis) - pActor->m_pModel->ModelRadius;
    if (dis < 0)
        return 0.f;
    return dis * dis;
    unguard;
}

void GcActor::CommandFinished() {
    guard;
    if (m_pCurCmd == NULL)
        return;
    CHECK(m_listCmd.size() != 0);
    m_pCurCmd = NULL;
    m_listCmd.pop_front();

    //#ifdef _DEBUG
    //    std::list<SCmd>::iterator itCmd = m_listCmd.begin();
    //    std::string szCmdName;
    //    while (itCmd!=m_listCmd.end())
    //    {
    //        szCmdName += GetCmdName((*itCmd).eCmd);
    //        szCmdName += " ";
    //        itCmd ++;
    //    }
    //    if (m_pHUD)
    //    {
    //        m_pHUD->SetGuildText(szCmdName);
    //        if (!szCmdName.empty()) m_pHUD->SetSpeakText(szCmdName);
    //    }
    //#endif
    unguard;
}

void GcActor::MoveToNow(float fX, float fY) {
    guard;
    int iTileX, iTileY;
    g_pScene->GetTerrainByPosFast(fX, fY, iTileX, iTileY);
    MoveToNow(iTileX, iTileY);
    unguard;
}

void GcActor::MoveToNow(int iTileX, int iTileY) {
    guard;
    ClearCommand();
    AddCommandTeleport(iTileX, iTileY, 0.f);
    DoAllCommandNow();
    unguard;
}

void GcActor::OnPoseBegin(SRT_Pose* pPose) {
    guard;
    if (GetRideState() == ERS_PET) {
        m_pMaster->OnPoseBegin(pPose);
        return;
    }
    unguard;
}

void GcActor::OnPoseEnd(SRT_Pose* pPose) {
    guard;
    if (GetRideState() == ERS_PET) {
        m_pMaster->OnPoseEnd(pPose);
        return;
    }
    if (m_listAttack.size()) {
        RtgVertex3                    vPos;
        SAttack*                      pAttack;
        std::list<SAttack*>::iterator it, itNext;
        for (it = m_listAttack.begin(); it != m_listAttack.end(); it = itNext) {
            pAttack = *it;
            itNext  = it;
            itNext++;
            if (pAttack->pPoseName && strcmp(pPose->Name.c_str(), pAttack->pPoseName) == 0) {
                m_bIsAttack = false;

                CHECK(pAttack->cAnswer != 0);
                switch (pAttack->cAnswer) {
                    case 2:
                        AttackEnd(pAttack->wAttackID);
                        break;
                    case 1:
                        LOG("GcActor::AttackBreak 攻击事件被中断\n");
                    default:
                        AttackBreak(pAttack->wAttackID);
                        break;
                }
            }
        }
    }

    if (m_pCurCmd) {
        switch (m_pCurCmd->eCmd) {
            case ACMD_MOVETO:
            case ACMD_FOLLOW:
                break;
            case ACMD_ATTACK:
            case ACMD_USESKILL:
            case ACMD_PLAY_POSE:
            case ACMD_UNDER_ATTACK:
                CommandFinished();
                break;
        }
    }
    unguard;
}

void GcActor::OnPoseBreak(SRT_Pose* pOldPose, SRT_Pose* pNewPose) {
    guard;
    if (GetRideState() == ERS_PET) {
        m_pMaster->OnPoseBreak(pOldPose, pNewPose);
        return;
    }
    if (m_listAttack.size()) {
        RtgVertex3                    vPos;
        SAttack*                      pAttack;
        std::list<SAttack*>::iterator it, itNext;
        for (it = m_listAttack.begin(); it != m_listAttack.end(); it = itNext) {
            pAttack = *it;
            itNext  = it;
            itNext++;
            if (pAttack->pPoseName && strcmp(pOldPose->Name.c_str(), pAttack->pPoseName) == 0) {
                m_bIsAttack = false;

                CHECK(pAttack->cAnswer != 0);
                switch (pAttack->cAnswer) {
                    case 2:
                        AttackEnd(pAttack->wAttackID);
                        break;
                    default:
                    case 1:
                        AttackBreak(pAttack->wAttackID);
                        break;
                }
            }
        }
    }
    unguard;
}

CActorMissileListener s_ActorMissileListener;

void GcActor::OnPoseEvent(SRT_Pose* pPose, SRT_PoseEvent* pEvent) {
    guard;
    // 如果角色处于骑乘状态并骑乘的是宠物，则由主角色处理姿态事件
    if (GetRideState() == ERS_PET) {
        m_pMaster->OnPoseEvent(pPose, pEvent);
        return;
    }
    // 提前检查是否有待处理的攻击
    if (m_listAttack.empty()) {
        // 没有攻击需要处理，退出
        return;
    }

    for (auto it = m_listAttack.begin(); it != m_listAttack.end();) {
        SAttack* pAttack = *it;
        if (pAttack->pPoseName &&
            pPose->Name == pAttack->pPoseName) {  //lyymark 判断动作映射名称和动作事件名称是否匹配
            if (pAttack->pSkill)  // 技能攻击
            {
                //CHECK(pPose->Name==pAttack->pSkill->szRAction);
                if (pEvent->Action == pAttack->pSkill->szREvent) {
                    OnAttackEvent(pAttack);  // 技能使用事件

                } else if (strcmp(pAttack->pSkill->szREvent, "shapeshift") == 0) {
                    DelAttack(pAttack);
                    it = m_listAttack.erase(it);
                    SCmd cmd;
                    cmd.cSubType = 1;
                    cmd.eCmd     = ACMD_SHAPESHIFT;
                    m_listCmd.push_back(cmd);
                    return;
                }
                //else if (strcmp(pAttack->pSkill->szREvent,"teleport") == 0)
                //{
                //	DelAttack(pAttack);
                //	it = m_listAttack.erase(it);
                //	mBaseActor.MoveToTerrainNow(m_vTelePoint.x, m_vTelePoint.y, true);
                //	m_dwDummyID = rtRandom();
                //	m_bTeleEvent = false;
                //	return;
                //}
            } else if (pEvent->Action == "hit") {
                OnAttackEvent(pAttack);  // 普通攻击
            }
        }
        it++;
    }

    unguard;
}

void GcActor::OnAttackEvent(WORD wAttackID) {
    guard;
    SAttack*                      pAttack;
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        pAttack = *it;
        if (pAttack->wAttackID == wAttackID) {
            OnAttackEvent(pAttack);
            break;
        }
    }
    unguard;
}

void GcActor::OnAttackEvent(SAttack* pAttack) {
    guard;                  // 进入临界区，保护代码块中的数据
    GcActor* pCtrl = this;  // 当前对象指针
    // 如果角色骑乘了宠物，将控制指针指向宠物
    if (GetRideState() == ERS_ACTOR) {
        pCtrl = this->m_pRidePet;
    }

    // 如果 NPC 类型是宠物，记录攻击 ID 和当前攻击 ID 的日志
    if (NpcType() == ENT_PET) {
        RtCoreLog().Debug("pet pAttack->wAttackID:%d, m_curAttackId: %d", pAttack->wAttackID,
                          m_curAttackId);
    }

    // 测试：如果攻击 ID 与当前攻击 ID 匹配，增加攻击事件计数器
    if (pAttack->wAttackID == m_curAttackId) {
        m_curAttackEvent++;
    }

    CRT_ActorInstance *pInstS, *pInstT;
    GcActor*           pTarget;
    // RtgMatrix12* pMat; // 未使用的变量
    RtgVertex3       vPos, TargetVertex3;  // 攻击位置和目标位置
    char             cTmp128[128];         // 临时字符串用于存储子弹模型名称
    CBeelineMissile* pMissile;             // 子弹对象

    // 如果攻击有技能
    if (pAttack->pSkill) {
        if (NpcType() == ENT_PET) {
            RtCoreLog().Debug("pet pSkill:%s", pAttack->pSkill->szName);
        }
        guard_name(GcActor::OnAttackEvent::skill);  // 进入技能处理的临界区
        // 技能使用事件处理
        CHECK(pAttack->cAnswer != 0);  // 确保 cAnswer 不为 0
        pAttack->cAnswer = 2;          // 设置攻击的回答标志

        // 如果技能需要飞行
        if (pAttack->pSkill->bFly) {  // mBaseActor.GetGraph() 是不可能为空的
            pInstT = pCtrl->mBaseActor.GetGraph()->p();  // 获取基础演员图形
            if (pInstT) {
                RtgMatrix12 _mat;
                // 获取“左手”的骨骼矩阵位置
                if (pInstT->GetBoneMat(&_mat, "Bip01 L Hand"))
                    vPos = _mat.Position();
                else
                    vPos.Set(0.f);
            } else {
                vPos.Set(0.f);
            }
            vPos = vPos * *(pCtrl->GetMatrix());  // 计算攻击位置

            pInstT  = NULL;
            pTarget = FindActor(pAttack->dwTargetID);  // 查找目标角色
            if (pTarget) {
                pInstT = pTarget->mBaseActor.GetGraph()->p();
                if (pInstT) {
                    // 获取目标位置
                    TargetVertex3 = pInstT->GetBoundingBox()->vPos;
                    TargetVertex3.x += pInstT->GetWorldMatrix()->_30;
                    TargetVertex3.y += pInstT->GetWorldMatrix()->_31;
                    TargetVertex3.z += pInstT->GetWorldMatrix()->_32;
                    // 创建并设置子弹
                    pMissile = RT_NEW CBeelineMissile(
                        vPos, TargetVertex3, (char*)pAttack->pSkill->szFlyEffect,
                        &s_ActorMissileListener, pAttack->pSkill->wFlySpeed,
                        pAttack->pSkill->cFlyType == 2);
                    pMissile->SetParameter(this->ID(), pAttack->wAttackID);
                }
            }
            if (pInstT == NULL) {
                OnAttackArrive(pAttack->wAttackID, false);  // 无目标，攻击失败
            } else {
                OnAttackArrive(pAttack->wAttackID, true);  // 有目标，攻击成功
            }
        } else {
            OnAttackArrive(pAttack->wAttackID, false);  // 不需要飞行的技能攻击，攻击失败
        }
        unguard;                                     // 离开技能处理的临界区
    } else {                                         // 普通攻击
        guard_name(GcActor::OnAttackEvent::normal);  // 进入普通攻击处理的临界区
        CHECK(pAttack->cAnswer != 0);                // 确保 cAnswer 不为 0
        SItemID item = pCtrl->mItem.GetEquipItem(CItemContainer::WEAPON_1, true);  // 获取装备的物品

        // 检查装备是否是弓
        if (ItemIsWeapon_Riband(item)) {
            OnAttackArrive(pAttack->wAttackID, true);  // 弓直接攻击，攻击成功
        }
        // 其他攻击处理（代码被注释掉了）
        //else if (ItemIsWeapon_Ball(item))
        //{	// 术士用法宝
        //	SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
        //	if (pItemClass)
        //	{
        //		pInstS = pCtrl->mBaseActor.GetGraph()->p();
        //		if (pInstS)
        //		{
        //			vPos = pInstS->GetBoneMat("Bip01 L Hand")->Position();
        //		}else
        //		{
        //			vPos.Set(0.f);
        //		}
        //		vPos = vPos * *(pCtrl->GetMatrix());

        //		pInstT = NULL;
        //		pTarget = FindActor(pAttack->dwTargetID);
        //		if (pTarget)
        //		{
        //			pInstT = pTarget->mBaseActor.GetGraph()->p();
        //			if (pInstT)
        //			{
        //				TargetVertex3 = pInstT->GetBoundingBox()->vPos;
        //				TargetVertex3.x += pInstT->GetWorldMatrix()->_30;
        //				TargetVertex3.y += pInstT->GetWorldMatrix()->_31;
        //				TargetVertex3.z += pInstT->GetWorldMatrix()->_32;
        //				rt2_strncpy(cTmp128, ((SWeapon*)pItemClass)->Models1[item.level], 128);
        //				rt2_strcat(cTmp128, "bullet_mj");
        //				pMissile = RT_NEW CBeelineMissile(vPos, TargetVertex3, cTmp128, &s_ActorMissileListener, 300);
        //				pMissile->SetParameter(this->ID(), pAttack->wAttackID);
        //			}
        //		}
        //		if (pInstT==NULL)
        //		{
        //			OnAttackArrive(pAttack->wAttackID, false);
        //		}
        //	}
        //}
        else if (ItemIsWeapon_Wand(item.type) && m_cShapeshiftNet == 0) {  // 法杖攻击
            SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
            if (pItemClass) {
                pInstS = pCtrl->mBaseActor.GetGraph()->p();

                RtgMatrix12 _mat;
                // 获取“左手”的骨骼矩阵位置
                if (pInstS && pInstS->GetBoneMat(&_mat, "Bip01 L Hand"))
                    vPos = _mat.Position();
                else
                    vPos.Set(0.f);
                vPos = vPos * *(pCtrl->GetMatrix());  // 计算攻击位置

                pInstT  = NULL;
                pTarget = FindActor(pAttack->dwTargetID);  // 查找目标角色
                if (pTarget) {
                    pInstT = pTarget->mBaseActor.GetGraph()->p();
                    if (pInstT) {
                        // 获取目标位置
                        TargetVertex3 = pInstT->GetBoundingBox()->vPos;
                        TargetVertex3.x += pInstT->GetWorldMatrix()->_30;
                        TargetVertex3.y += pInstT->GetWorldMatrix()->_31;
                        TargetVertex3.z += pInstT->GetWorldMatrix()->_32;
                        // 创建并设置子弹
                        rt2_strncpy(cTmp128, "bullet_mj", 128);
                        pMissile = RT_NEW CBeelineMissile(vPos, TargetVertex3, cTmp128,
                                                          &s_ActorMissileListener, 300);
                        pMissile->SetParameter(this->ID(), pAttack->wAttackID);
                    }
                }
                if (pInstT == NULL) {
                    OnAttackArrive(pAttack->wAttackID, false);  // 无目标，攻击失败
                }
            }
        } else {                                       // 其他普通攻击
            OnAttackArrive(pAttack->wAttackID, true);  // 直接攻击，攻击成功
        }
        unguard;  // 离开普通攻击处理的临界区
    }
    unguard;  // 离开整个方法的临界区
}

bool GcActor::OnAttackArrive(WORD wAttackID, bool bSucceed) {
    guard;  // 进入临界区，保护代码块中的数据

    // 如果攻击列表为空，记录日志并返回 false
    if (m_listAttack.size() == 0) {
        LOG("GcActor::OnAttackArrive 当前缓冲区没有任何攻击数据，却收到攻击结束消息.\n");
        return false;
    }

    bool                          bResult = false;  // 初始化结果为 false
    RtgVertex3                    vPos;             // 攻击位置（虽然未使用）
    SAttack*                      pAttack;          // 当前攻击
    std::list<SAttack*>::iterator it, itNext;       // 攻击列表的迭代器

    // 遍历攻击列表
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it = itNext) {
        pAttack = *it;  // 获取当前攻击
        itNext  = it;   // 保存当前迭代器的位置
        itNext++;       // 移动到下一个迭代器位置

        // 如果攻击 ID 匹配
        if (pAttack->wAttackID == wAttackID) {
            OnAttackArrive(pAttack, bSucceed);  // 调用处理函数
            bResult = true;                     // 设置结果为 true
        }
    }

    return bResult;  // 返回结果
    unguard;         // 离开临界区，解除对数据的保护
}

// 处理攻击到达
void GcActor::OnAttackArrive(SAttack* pAttack, bool bSucceed) {
    guard;  // 进入保护代码块，确保在异常情况下能正确处理

    GcActor* pActor;  // 用于保存受击的角色指针

    // 获取角色装备的主武器和副武器信息
    SItemID item  = mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
    SItemID item2 = mItem.GetEquipItem(CItemContainerBase::WEAPON_2, true);

    // 遍历攻击造成的所有伤害记录
    std::vector<SAttack::SDamage>::iterator itDamage = pAttack->listDamage.begin();
    for (; itDamage != pAttack->listDamage.end(); itDamage++) {
        // 查找受击角色
        pActor = FindActor((*itDamage).dwActorID, true);
        if (pActor) {  // 如果找到角色
            // 如果攻击技能有效且不是“坏”技能
            if (!pAttack->pSkill || pAttack->pSkill->bABad) {
                if (itDamage->ret == USE_SKILL_OK && GetProb(0.03f)) {
                    // 如果角色有当前命令且该命令不是使用技能命令
                    if (pActor->m_pCurCmd) {
                        if (pActor->m_pCurCmd->eCmd != ACMD_USESKILL) {
                            pActor->AddCommand(ACMD_UNDER_ATTACK);  // 添加受击命令
                        }
                    } else {
                        pActor->AddCommand(ACMD_UNDER_ATTACK);  // 添加受击命令
                        pActor->mBaseActor.FaceTo(GetMatrix()->_30,
                                                  GetMatrix()->_31);  // 角色面朝攻击来源
                    }
                }
            }

            // 设置受击特效和链接
            const char* szHitEffect = NULL;  // 默认受击特效
            const char* szHitLink   = NULL;  // 默认特效链接
            if (itDamage->criHit)            // 如果是暴击
                szHitEffect = "bisha";       // 设置暴击特效
            else
                szHitEffect = "beiji";  // 设置普通受击特效

            // 如果攻击回合数小于3，进行特效播放
            if (pAttack->cAnswer < 3) {
                pAttack->cAnswer = 3;  // 更新攻击回合数为3
                // 播放技能特效
                if (pAttack->pSkill) {  // 如果是技能攻击
                    if (pAttack->pSkill->szUEffect[0]) {
                        szHitEffect = pAttack->pSkill->szUEffect;  // 使用技能特效
                    }
                    if (pAttack->pSkill->szUEfLink[0]) {
                        szHitLink = pAttack->pSkill->szUEfLink;  // 使用技能特效链接
                    }
                } else {
                    // 非技能攻击的特效处理
                    if (itDamage->criHit)
                        szHitEffect = "bisha";  // 暴击特效
                    else
                        szHitEffect = "beiji";  // 普通受击特效
                }

                // 播放特效
                if (szHitLink && (*itDamage).ret != USE_SKILL_MISS) {
                    pActor->mBaseActor.PlayEffect(szHitEffect, true, szHitLink,
                                                  RtgVertex3(0, 0, 0));  // 播放特效链接
                } else {
                    if (pActor->IsValid() && (*itDamage).ret != USE_SKILL_MISS)
                        pActor->mBaseActor.PlayEffect(
                            szHitEffect, false, NULL,
                            RtgVertex3(
                                0, 0,
                                pActor->GetGraph()->p()->GetBoundingBox()->vPos.z));  // 播放特效
                }
                g_layerMain->UpDateUIBlood();  // 更新血条显示
            }

            // 显示伤害数字
            if (g_layerMain->m_eSetting.m_bShowDamage) {
                bool lasthit = false;
                // 处理非NPC角色的伤害显示
                if (pActor->m_pNpc == NULL ||
                    (pActor->m_pNpc->Type != 4 && pActor->m_pNpc->Type != 5)) {
                    if (!m_pCurCmd) {
                        lasthit = true;

                        // 显示伤害数字
                        pActor->ShowDamageNumber((*itDamage).ret, (*itDamage).sDamage,
                                                 (*itDamage).eleDmg, (*itDamage).criHit,
                                                 itDamage->eleSpe);
                    } else {
                        if (m_pCurCmd->eCmd == ACMD_ATTACK) {
                            if (ItemIsWeapon_Hoop(item.type)) {  // 双环武器处理
                                //if(itDamage->hits == 1)
                                //{
                                //	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
                                //		itDamage->eleDmg[i] = 0;
                                //}
                                if (itDamage->criHit) {
                                    if (itDamage->sDamage) {
                                        pActor->ShowHeadNotify(CHAR_BAOJIPUTONG,
                                                               itDamage->sDamage /
                                                                   2);  // 显示暴击伤害
                                    }
                                } else {
                                    pActor->ShowDamageNumber(itDamage->ret, itDamage->sDamage / 2,
                                                             itDamage->eleDmg, itDamage->criHit,
                                                             itDamage->eleSpe);  // 显示普通伤害
                                }
                            } else {
                                lasthit = true;  // 标记这是最后一次处理的伤害

                                if (itDamage->criHit) {       // 如果这是一次暴击
                                    if (itDamage->sDamage) {  // 如果伤害值存在
                                        // 调用 pActor 的 ShowHeadNotify 方法显示暴击伤害
                                        pActor->ShowHeadNotify(
                                            CHAR_BAOJIPUTONG,  // 调用 pActor 的 ShowHeadNotify 方法显示暴击伤害
                                            itDamage->sDamage);  // 显示暴击伤害
                                    }
                                } else {
                                    pActor->ShowDamageNumber(itDamage->ret, itDamage->sDamage,
                                                             itDamage->eleDmg, itDamage->criHit,
                                                             itDamage->eleSpe);  // 显示普通伤害
                                }
                            }
                        } else {
                            // 处理技能攻击的伤害信息
                            SSkill* skill =
                                pAttack->pSkill /*Skill()->FindSkill(m_pCurCmd->wSkillID)*/;

                            if (skill) {
                                int total = 0;
                                for (int i = 0; i <= skill->wHits; i++)
                                    total += i;  // 计算总命中次数
                                if (total == 0)
                                    total = 1;

                                int safe = 0;
                                while (itDamage->hits < m_curAttackEvent && safe++ < 10) {
                                    short eleDmg[MAX_ELEMENT_ATTR];
                                    for (int i = ELEMENT_WATER; i < MAX_ELEMENT_ATTR; i++) {
                                        eleDmg[i] = itDamage->eleDmg[i] * (itDamage->hits + 1) /
                                                    total;  // 计算元素伤害
                                    }

                                    if (itDamage->criHit) {
                                        if (itDamage->sDamage) {
                                            pActor->ShowHeadNotify(CHAR_BAOJIPUTONG,
                                                                   itDamage->sDamage *
                                                                       (itDamage->hits + 1) /
                                                                       total);  // 显示暴击伤害
                                        }
                                        if (itDamage->eleDmg[ELEMENT_WATER]) {
                                            pActor->ShowHeadNotify(
                                                CHAR_BAOJISHUI,
                                                eleDmg[ELEMENT_WATER]);  // 显示水元素伤害
                                        }
                                        if (itDamage->eleDmg[ELEMENT_FIRE]) {
                                            pActor->ShowHeadNotify(
                                                CHAR_BAOJIHUO,
                                                eleDmg[ELEMENT_FIRE]);  // 显示火元素伤害
                                        }
                                        if (itDamage->eleDmg[ELEMENT_POISON]) {
                                            pActor->ShowHeadNotify(
                                                CHAR_BAOJIDU,
                                                eleDmg[ELEMENT_POISON]);  // 显示毒元素伤害
                                        }
                                    } else {
                                        pActor->ShowDamageNumber(itDamage->ret,
                                                                 itDamage->sDamage *
                                                                     (itDamage->hits + 1) / total,
                                                                 eleDmg, itDamage->criHit,
                                                                 itDamage->eleSpe);  // 显示普通伤害
                                    }

                                    itDamage->hits++;  // 增加击中次数
                                                       // LOG1("hits = %d\n",itDamage->hits);
                                }
                                if (itDamage->hits == skill->wHits)
                                    lasthit = true;  // 标记最后一次击中
                            }
                        }
                    }
                }
            }

            // 播放必杀音效
            RtgVertex3 pos;  // 音效播放位置
            if (itDamage->criHit) {
                g_pSoundMgr->PlayOnce("cri_hit_2.wav", false, 0, pos);  // 播放暴击音效
            }

            // 处理角色死亡
            if (itDamage->dead)  // && lasthit)
            {
                LOG("actor die\n");
                GetWorld()->m_ActorManager.ActorDie(itDamage->dwActorID,
                                                    itDamage->dead);        // 角色死亡处理
                GetWorld()->m_ActorManager.HideHudHp(itDamage->dwActorID);  // 隐藏角色血条
            }
        }
    }

    // 注释掉的代码用于释放攻击特效资源
    //if ( PoolEffect )
    //{
    //	ActorUncache(PoolEffect);
    //}
    //pAttack->listDamage.clear(); // 清空伤害列表（注释掉的代码）

    unguard;  // 退出保护代码块
}

void GcActor::PickItemAdd(long pActorID) {
    if (m_bIsDead) {
        return;
    }
    SCmd cmd;
    cmd.fDistance   = 34.f;
    cmd.dw[0]       = pActorID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.eCmd        = ACMD_PICKITEM;
    cmd.bLoop       = true;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;
    AddCommand(cmd);
}

void GcActor::StopCatchPet(char cError) {
    if (m_dwCatchPet) {
        GcActor* pPet = FindAllActor(m_dwCatchPet);
        if (pPet) {
            pPet->m_dwCatchMe = 0;
        }
        m_dwCatchPet = 0;
    }
    if (this == GetPlayer()) {
        switch (cError) {
            case PET_CATCH_ERROR_START:
                break;
            case PET_CATCH_ERROR_LIFETIME: {
                g_layerMain->m_formTimeBar->Hide(true);
                PlayerFailOperate(R(MSG_PET_CATCH_ERROR_LIFETIME));
            } break;
            case PET_CATCH_ERROR_BECATCHED: {
                g_layerMain->m_formTimeBar->Hide(true);
                PlayerFailOperate(R(MSG_PET_CATCH_ERROR_BECATCHED));
            } break;
            case PET_CATCH_ERROR_BEATTACKED: {
                g_layerMain->m_formTimeBar->Hide(true);
                PlayerFailOperate(R(MSG_PET_CATCH_ERROR_BEATTACKED));
            } break;
            case PET_CATCH_ERROR_DIE: {
                g_layerMain->m_formTimeBar->Hide(true);
                PlayerFailOperate(R(MSG_PET_CATCH_ERROR_DIE));
            } break;
            case PET_CATCH_ERROR_SUCCEED: {
                g_layerMain->m_formTimeBar->Hide(true);
                PlayerFailOperate(R(MSG_PET_CATCH_ERROR_SUCCEED));
            } break;
            default:
                g_layerMain->m_formTimeBar->Hide(true);
                break;
        }
    }
    RemoveCurCommand();
}

void GcActor::CatchPetAdd(GcActor* pActor) {
    if (m_bIsDead || !pActor)
        return;

    SCmd cmd;
    cmd.fDistance   = 34.f;
    cmd.dw[0]       = pActor->ID();
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.eCmd        = ACMD_CATCHPET;
    cmd.bLoop       = false;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;

    AddCommand(cmd);
}

void GcActor::ShapeshiftAdd(long modelID) {
    AddCommand(ACMD_SHAPESHIFT);
}

// 添加开始触发器命令
// @param close - 关闭标志
// @param final - 最终标志
void GcActor::StartTriggerAdd(char close, bool final) {
    // 如果角色已死亡则返回
    if (m_bIsDead) {
        return;
    }
    ClearCommand();  //清除命令
    //RemoveCurCommand();

    // 创建命令结构
    SCmd cmd;
    // 根据final参数设置dw[0]值
    if (final)
        cmd.dw[0] = close;
    else
        cmd.dw[0] = !close;

    // 设置命令类型为开始触发器
    cmd.eCmd = ACMD_START_TRIGGER;

    // 设置命令执行时间
    // 如果是玩家则设为-1,否则为0
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;

    // 添加命令到命令队列
    AddCommand(cmd);
}

void GcActor::TalkNpcAdd(long pActorID) {
    if (m_bIsDead) {
        return;
    }
    SCmd     cmd;
    GcActor* pActor = FindAllActor(pActorID);
    if (pActor->m_pNpc) {
        if (pActor->m_pNpc->Type == 2) {
            cmd.fDistance = 70.f;
        } else
            cmd.fDistance = 20.f;
    }
    cmd.dw[0]       = pActorID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.eCmd        = ACMD_TALKNPC;
    cmd.bLoop       = true;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;
    AddCommand(cmd);
}

void GcActor::CollectAdd(long pActorID) {
    guard;
    if (m_bIsCollect || m_bIsDead) {
        return;
    }
    SCmd cmd;
    cmd.fDistance   = 40.f + 300.f;
    cmd.dw[0]       = pActorID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.eCmd        = ACMD_COLLECT;
    cmd.bLoop       = true;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;

    AddCommand(cmd);

    m_bIsCollect = true;
    //m_lPickActorID = pActorID;

    if (this == GetPlayer()) {
        GcActor* pActor = FindAllActor(m_lPickActorID);
        m_lCollectTime  = pActor->m_pNpc->dwGetSkinTime * 1000;  // 毫秒计时
    }

    unguard;
}

void GcActor::PlayCollectTask(long pActorID) {
    if (m_bIsCollect || m_bIsDead) {
        return;
    }
    SCmd cmd;
    cmd.fDistance   = 40.f + 300.f;
    cmd.dw[0]       = pActorID;
    cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
    cmd.eCmd        = ACMD_PLAYTASKITEM;
    cmd.bLoop       = true;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;

    AddCommand(cmd);

    m_bIsCollect   = true;
    m_lCollectTime = 5000;
}

void GcActor::PlayActivityProgressBar(long lActorID, long lTime) {
    bActivity = true;
    g_layerMain->m_fromPlayerItems->PlayActivityProgressBar(lTime);

    SCmd cmd;
    cmd.fDistance   = 40.f + 300.f;
    cmd.dw[0]       = lActorID;
    cmd.cTargetType = SKILL_TARGET_TYPE_NONE;
    cmd.eCmd        = ACMD_PLAYTASKITEM;
    cmd.bLoop       = true;
    if (this == GetPlayer())
        cmd.fTime = -1.f;
    else
        cmd.fTime = 0;

    AddCommand(cmd);
}

// 要添加攻击命令
void GcActor::AttackAdd(SAttack* pAttack) {
    guard;
    SCmd        cmd;
    SSkill*     pSkill;
    const float fDistanceOffset = 300.f;

    //// 如果是网络回传的命令（其他人），需要加速上一命令
    //if(GetWorld()->m_pPlayer != this && m_pCurCmd && m_pCurCmd->eCmd == ACMD_MOVETO)
    //{
    //	AddCommandTeleport(m_pCurCmd->f[0],m_pCurCmd->f[1],0);
    //}

    //if(GetWorld()->m_pPlayer != this)
    //	SubmitCurMoveCommand();

    // 添加一个攻击操作
    if (pAttack->pSkill) {  // 技能攻击
        if (pAttack->pSkill->dwRTarget & SKILL_TARGET_POSITION) {
            //UsePositionSkill(pAttack->wAttackID, pAttack->pSkill->wID, pAttack->f[0], pAttack->f[1], pAttack->f[2], pAttack->fTime);
            pSkill = Skill()->FindSkill(pAttack->pSkill->wID);
            if (pSkill == NULL)
                return;
            cmd.fDistance   = pSkill->iRMaxDest + fDistanceOffset;  // 距离
            cmd.f[0]        = pAttack->f[0];
            cmd.f[1]        = pAttack->f[1];
            cmd.f[2]        = pAttack->f[2];
            cmd.cTargetType = SKILL_TARGET_TYPE_POSITION;
            cmd.wSkillID    = pAttack->pSkill->wID;
            cmd.wAttackID   = pAttack->wAttackID;
            cmd.fTime       = pAttack->fTime;
            cmd.eCmd        = ACMD_SKILL_SOMEWHERE;
            cmd.bLoop       = false;
            AddCommand(cmd);
        } else {
            //UseTargetSkill(pAttack->wAttackID, pAttack->pSkill->wID, pAttack->dwTargetID, pAttack->fTime);
            if (m_listCmd.size()) {
                SCmd* pLast = &(m_listCmd.back());
                if (pLast->eCmd == ACMD_USESKILL && pLast->dw[0] == pAttack->dwTargetID) {
                    return;  // 上一个命令是相同的命令，就直接返回
                }
            }
            pSkill = Skill()->FindSkill(pAttack->pSkill->wID);
            if (pSkill == NULL)
                return;
            cmd.fDistance   = pSkill->iRMaxDest + fDistanceOffset;  // 距离
            cmd.dw[0]       = pAttack->dwTargetID;
            cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
            cmd.wSkillID    = pAttack->pSkill->wID;
            cmd.wAttackID   = pAttack->wAttackID;
            cmd.fTime       = pAttack->fTime;
            cmd.eCmd        = ACMD_USESKILL;
            cmd.bLoop       = false;
            AddCommand(cmd);
        }
    } else {  // 普通攻击
        //AttackSomebody(pAttack->wAttackID, pAttack->dwTargetID, pAttack->fTime);
        if (m_listCmd.size()) {
            SCmd* pLast = &(m_listCmd.back());
            if (pLast->eCmd == ACMD_ATTACK && pLast->dw[0] == pAttack->dwTargetID) {
                return;  // 上一个命令是相同的命令，就直接返回
            }
        }

        SWeapon* pWeapon  = NULL;
        SItemID  itemWea1 = mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
        if (ItemID_IsValid(itemWea1))
            pWeapon = (SWeapon*)(GetItemMgr()->GetItem(itemWea1.type));

        if (pWeapon)
            cmd.fDistance = pWeapon->nAttRange + fDistanceOffset;
        else
            cmd.fDistance = 40.f + fDistanceOffset;
        cmd.dw[0]       = pAttack->dwTargetID;
        cmd.cTargetType = SKILL_TARGET_TYPE_ACTOR;
        cmd.wAttackID   = pAttack->wAttackID;
        cmd.fTime       = pAttack->fTime;
        cmd.eCmd        = ACMD_ATTACK;
        cmd.bLoop       = true;
        AddCommand(cmd);
    }

    // 添加一个攻击效果数据
    m_listAttack.push_back(pAttack);
    unguard;
}

GcActor::SAttack* GcActor::FindAttackByAttackID(unsigned short wAttackID) {
    guard;
    SAttack*                      pAttack;
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        pAttack = *it;
        if (pAttack->wAttackID == wAttackID) {
            return pAttack;
        }
    }
    return NULL;
    unguard;
}

bool GcActor::AttackAnswer(unsigned short wAttackID, const char* pPoseName) {
    guard;
    if (wAttackID == 0)
        return false;
    SAttack*                      pAttack;
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        pAttack = *it;
        if (pAttack->wAttackID == wAttackID) {
            if (pAttack->cAnswer == 0)
                pAttack->cAnswer = 1;
            pAttack->pPoseName = pPoseName;
            return true;
        }
    }
    return false;
    unguard;
}

bool GcActor::AttackBreak(unsigned short wAttackID) {
    guard;
    if (wAttackID == 0)
        return false;
    SAttack*                      pAttack;
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        pAttack = *it;
        if (pAttack->wAttackID == wAttackID) {
            CHECK(pAttack->cAnswer != 0);
            CHECK(pAttack->pPoseName != NULL);
            if (pAttack->cAnswer != 3) {
                OnAttackArrive(pAttack, false);
            }
            DelAttack(pAttack);
            m_listAttack.erase(it);
            if (m_pCurCmd) {
                switch (m_pCurCmd->eCmd) {
                    case ACMD_MOVETO:
                    case ACMD_FOLLOW:
                        break;
                    case ACMD_ATTACK:
                    case ACMD_USESKILL:
                    case ACMD_PLAY_POSE:
                    case ACMD_UNDER_ATTACK:
                        CommandFinished();
                        break;
                }
            }
            return true;
        }
    }
    CHECKEX("GcActor::AttackBreak Can't find AttackID");
    return false;
    unguard;
}

void GcActor::AttackEnd(unsigned short wAttackID) {
    guard;
    if (wAttackID == 0)
        return;
    SAttack*                      pAttack;
    std::list<SAttack*>::iterator it;
    for (it = m_listAttack.begin(); it != m_listAttack.end(); it++) {
        pAttack = *it;
        if (pAttack->wAttackID == wAttackID) {
            if (pAttack->cAnswer < 2) {
                OnAttackArrive(pAttack, true);
            }
            DelAttack(pAttack);
            m_listAttack.erase(it);
            if (m_pCurCmd) {
                switch (m_pCurCmd->eCmd) {
                    case ACMD_MOVETO:
                    case ACMD_FOLLOW:
                        break;
                    case ACMD_ATTACK:
                    case ACMD_USESKILL:
                    case ACMD_PLAY_POSE:
                    case ACMD_UNDER_ATTACK:
                        CommandFinished();
                        break;
                }
            }
            return;
        }
    }
    CHECKEX("GcActor::AttackBreak Can't find AttackID");
    unguard;
}

void GcActor::ShowHeadNotify(const char* type, short sDamage, bool spec /* = false*/) {
    char cDir = 0;
    char szStr[20];
    if (this == GetPlayer())
        cDir = DIR_RIGHT;
    else
        cDir = DIR_LEFT;
    if (sDamage >= 0)
        rt2_sprintf(szStr, "-%d", sDamage);
    else
        rt2_sprintf(szStr, "+%d", -sDamage);
    GetWorld()->m_dynamicNumber.AddString(type, szStr, GetHUDPos(), 1.f, true, cDir, PIC_CHAR);
}

//void GcActor::ShowHeadNotify(char *type,bool spec/* = false*/)
//{
//	char cDir = 0;
//	if(this == GetPlayer()) cDir = DIR_RIGHTUP;
//	else cDir = DIR_LEFTUP;
//	GetWorld()->m_dynamicNumber.AddString(type, GetHUDPos(), 2.f,true,cDir,PIC_CHAR);
//}
void GcActor::ShowDamageNumber(int ret, short sDamage, short eleDmg[MAX_ELEMENT_ATTR], bool criHit,
                               char eleSpe) {
    guard;

    /* P_LOGINFO(std::string("调用 ShowDamageNumber 函数: ") + "\n返回值: " + std::to_string(ret) +
              "\n伤害值: " + std::to_string(sDamage) + "\n元素伤害: (" + std::to_string(eleDmg[0]) +
              ", " + std::to_string(eleDmg[1]) + ", " + std::to_string(eleDmg[2]) + ")" +
              "\n暴击命中: " + std::to_string(criHit) + "\n元素特性: " + std::to_string(eleSpe));*/

    char szStr[20];  // 存储要显示的伤害文本
    char cDir = 0;   // 伤害文本的显示方向

    // 如果技能未命中
    if (ret == USE_SKILL_MISS) {
        // 根据是否为玩家决定显示方向
        if (this == GetPlayer())
            cDir = DIR_RIGHTUP;
        else
            cDir = DIR_LEFTUP;
        // 显示未命中的提示
        GetWorld()->m_dynamicNumber.AddString(CHAR_SHANBI, GetHUDPos(), 2.f, true, cDir, PIC_CHAR);
        return;  // 返回，结束函数
    }
    // 如果技能命中
    else if (ret == USE_SKILL_OK) {
        // 如果伤害值为0，则不显示任何伤害
        if (sDamage == 0) {
            // 这里可以添加代码来处理伤害为0的情况（如果有需要的话）
        }
        // 如果伤害值为正数
        else if (sDamage >= 0) {
            // 根据是否为玩家决定显示方向
            if (this == GetPlayer())
                cDir = DIR_LEFTUP;
            else
                cDir = DIR_RIGHTUP;

            // 格式化伤害值为字符串
            rt2_sprintf(szStr, "-%d", sDamage);

            // 如果是暴击，显示暴击效果
            if (criHit) {
                GetWorld()->m_dynamicNumber.AddString(CHAR_BAOJIPUTONG, GetHUDPos(), 2.f, true,
                                                      cDir, PIC_CHAR);
            }
            // 显示实际的伤害值
            GetWorld()->m_dynamicNumber.AddString(szStr, GetHUDPos(), 0xFF00FF00, 1.f, true,
                                                  PIC_NORMAL);
        }
        // 如果伤害值为负数（通常表示恢复）
        else {
            // 格式化伤害值为字符串（正数形式）
            rt2_sprintf(szStr, "+%d", -sDamage);
            // 显示实际的恢复值
            GetWorld()->m_dynamicNumber.AddString(szStr, GetHUDPos(), 0xFF00FF00, 1.f, true,
                                                  PIC_NOBAD);
        }

        // 显示元素伤害特效
        RtgVertex3 vPos(mBaseActor.GetMatrix()->Position());

        // 根据元素特效类型播放相应的效果（注释掉的代码段可以根据需求启用）
        /*
        string elePfm;
        switch(eleSpe)
        {
        case ELEMENT_AUTOPFM_WATER:
            mBaseActor.PlayEffect("eff_water_at",false,NULL,RtgVertex3(0,0,0));
            break;
        case ELEMENT_AUTOPFM_FIRE:
            mBaseActor.PlayEffect("eff_bomb_at",false,NULL,RtgVertex3(0,0,0));
            break;
        case ELEMENT_AUTOPFM_ICE:
            mBaseActor.PlayEffect("eff_ice_at",false,NULL,RtgVertex3(0,0,0));
            break;
        case ELEMENT_AUTOPFM_POISON:
            mBaseActor.PlayEffect("eff_posion_dot",false,NULL,RtgVertex3(0,0,0));
            break;
        }
        */

        // 显示元素伤害数字
        if (this == GetPlayer())
            cDir = DIR_LEFTUP;
        else
            cDir = DIR_RIGHTUP;
        float offset = 15;  // 元素伤害显示的起始偏移

        // 如果水元素伤害大于0
        if (eleDmg[ELEMENT_WATER] > 0) {
            // 如果是暴击，显示暴击效果
            if (criHit) {
                GetWorld()->m_dynamicNumber.AddString(
                    CHAR_BAOJISHUI,
                    RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset), 2.f, true,
                    cDir, PIC_CHAR);
            }
            // 显示实际的水元素伤害值
            rt2_sprintf(szStr, "-%d", eleDmg[ELEMENT_WATER]);
            GetWorld()->m_dynamicNumber.AddString(
                szStr, RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset), 0xFF00FFFF,
                1, true, PIC_WATER);
            offset += 15;  // 更新偏移
        }

        // 如果火元素伤害大于0
        if (eleDmg[ELEMENT_FIRE] > 0) {
            // 如果是暴击，显示暴击效果
            if (criHit) {
                GetWorld()->m_dynamicNumber.AddString(
                    CHAR_BAOJIHUO, RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset),
                    2.f, true, cDir, PIC_CHAR);
            }
            // 显示实际的火元素伤害值
            rt2_sprintf(szStr, "-%d", eleDmg[ELEMENT_FIRE]);
            GetWorld()->m_dynamicNumber.AddString(
                szStr, RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset), 0xFFFF00FF,
                1, true, PIC_FIRE);
            offset += 15;  // 更新偏移
        }

        // 如果毒元素伤害大于0
        if (eleDmg[ELEMENT_POISON] > 0) {
            // 如果是暴击，显示暴击效果
            if (criHit) {
                GetWorld()->m_dynamicNumber.AddString(
                    CHAR_BAOJIDU, RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset),
                    2.f, true, cDir, PIC_CHAR);
            }
            // 显示实际的毒元素伤害值
            rt2_sprintf(szStr, "-%d", eleDmg[ELEMENT_POISON]);
            GetWorld()->m_dynamicNumber.AddString(
                szStr, RtgVertex3(GetHUDPos().x, GetHUDPos().y, GetHUDPos().z + offset), 0xFF00FF00,
                1, true, PIC_POISON);
            offset += 15;  // 更新偏移
        }
    }
    unguard;
}

void GcActor::UpdateFromServer(CG_CmdPacket* cmd) {
    guard;
    char type;
    if (!cmd->ReadByte(&type))
        return;
    Serialize(type, cmd);
    unguard;
}

void GcActor::UpdateFromServerDelta(CG_CmdPacket* cmd) {
    guard;  // 进入保护区，可能是线程同步或其他保护机制
    // LOG2("update_delta: %d,%d\n",cmd->GetByteDataSize(),cmd->GetBitDataSize());
    // 记录更新前的状态
    SCreature old     = m_core;             // 保存角色的旧状态
    char      faction = m_core.Faction;     // 保存角色的阵营信息
    long      lSP     = m_core.SkillPoint;  // 保存角色的技能点数
    // LOG1("old lev = %d\n",old.Lev);
    // 使用增量序列化来更新角色数据
    m_core.SerializeDelta(*cmd, old);
    // LOG1("new lev = %d\n",m_core.Lev);

    // 更新血量比率
    if (GetWorld()->m_pPlayer == this && GetRideState() != GcActor::ERS_ACTOR) {
        m_cHpRate = (((float)m_core.GetHp()) / m_core.GetMaxHp()) * 100;
        if (m_cHpRate < 0)
            m_cHpRate = 0;  // 确保血量比率不低于 0
        else if (m_cHpRate > 100)
            m_cHpRate = 100;  // 确保血量比率不超过 100
    }

    // 如果角色是玩家或宠物，更新相关界面
    if (GetWorld()->m_pPlayer == this || (GetWorld()->m_pPet == this)) {
        if (g_layerMain) {
            if (g_layerMain->m_formChar)
                g_layerMain->m_formChar->Refresh();  // 刷新角色界面
            if (g_layerMain->m_formPetProp)
                g_layerMain->m_formPetProp->Update();  //tim.yang  神兽系统
            if (g_layerMain->m_formCharSimple) {
                g_layerMain->m_formCharSimple->Update();               // 更新简化角色界面
                g_layerMain->m_formCharSimple->UpdateTaskTrackList();  // 更新任务追踪列表
            }
            if (g_layerMain->m_formPetSimple)
                g_layerMain->m_formPetSimple->Update();  // 更新简化宠物界面
            if (g_layerMain->m_formTrump)
                g_layerMain->m_formTrump->Refresh();  // 刷新界面
            if (m_core.SkillPoint != lSP && g_layerMain->m_fromSkill) {
                if (g_layerMain->m_fromSkill->IsShowing()) {
                    g_layerMain->m_fromSkill->Refresh();  // 刷新技能界面
                }
            }
        }
    }
    /*GetWorld()->m_pPlayer->_auto_eat_red();
	GetWorld()->m_pPlayer->_auto_eat_blue();*/
    // 如果角色处于自动状态，自动吃药
    GcPlayer* pTmpPlayer = GetWorld()->m_pPlayer;
    if (pTmpPlayer->GetAutoState()) {
        pTmpPlayer->_auto_eat_red();   // 自动使用红药水
        pTmpPlayer->_auto_eat_blue();  // 自动使用蓝药水
    }

    unguard;  // 退出保护区
}

const char* GcActor::GetCmdName(GcActor::ECommands eCmd) {
    switch (eCmd) {
        case ACMD_NONE:
            return "NONE";
        case ACMD_BORN:
            return "BORN";
        case ACMD_DIE:
            return "DIE";
        case ACMD_TELEPORT:
            return "TELEPORT";
        case ACMD_MOVETO:
            return "MOVETO";
        case ACMD_FOLLOW:
            return "FOLLOW";
        case ACMD_ATTACK:
            return "ATTACK";
        case ACMD_USESKILL:
            return "USESKILL";
        case ACMD_PICKITEM:
            return "PICKITEM";
        case ACMD_INTONATE:
            return "INTONATE";
        case ACMD_COLLECT:
            return "COLLECT";
        case ACMD_PLAYTASKITEM:
            return "PLAYTASKITEM";
        case ACMD_CATCHPET:
            return "CATCHPET";
        case ACMD_UNDER_ATTACK:
            return "UNDER_ATTACK";
        case ACMD_PLAY_POSE:
            return "PLAY_POSE";
        case ACMD_CHANGE_ACTOR:
            return "CHANGE_ACTOR";
        case ACMD_UNEQUIP_ALL:
            return "UNEQUIP_ALL";
        case ACMD_EQUIP_ALL:
            return "EQUIP_ALL";
        case ACMD_DEATH_ACT:
            return "DEATH_ACT";
        case ACMD_DELETE_SELF:
            return "DELETE_SELF";
        case ACMD_ATTACK_SOMEBODY:
            return "ATTACK_SOMEBODY";
        case ACMD_SKILL_SOMEWHERE:
            return "SKILL_SOMEWHERE";
        case ACMD_SKILL_SOMEBODY:
            return "SKILL_SOMEBODY";
        case ACMD_DEATH:
            return "DEATH";
        case ACMD_START_TRIGGER:
            return "START_TRIGGER";
        case ACMD_TALKNPC:
            return "TALKNPC";
        case ACMD_SHAPESHIFT:
            return "SHAPESHIFT";
    }
    return "Unknowe";
}

void GcActor::Serialize(char type, CG_CmdPacket* cmd) {
    guard;
    m_core.Serialize(type, cmd);

    switch (type) {
        case NET_UPDATE_ALL:
            mItem.Serialize(type, cmd);
            mItem.UpdateGraph();
            if (g_layerMain && g_layerMain->m_fromPlayerItems) {
                g_layerMain->m_fromPlayerItems->Refresh(true);
            }

            /*
		m_task.SerializeTask(cmd);
		if(GetWorld()->m_pPlayer==this)
		if (g_layerMain && g_layerMain->m_formTaskInfo)
		g_layerMain->m_formTaskInfo->Update();

		m_task.SerializeFbTask(cmd);
		*/
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
            if (GetWorld()->m_pPlayer == this) {
                if (g_layerMain && g_layerMain->m_formTask)
                    g_layerMain->m_formTask->Update();
                if (g_layerMain && g_layerMain->m_formCharSimple)
                    g_layerMain->m_formCharSimple->UpdateTaskTrackList();
                if (g_layerMain && g_layerMain->m_fromMiniMap)
                    g_layerMain->m_fromMiniMap->OnRefreshCanAcceptTask();
                if (g_layerMain && g_layerMain->m_formMiddleMap)
                    g_layerMain->m_formMiddleMap->Refresh();
            }
            break;

        case NET_CRE_UPDATE_FB_TASK:
            m_task.SerializeFbTask(cmd);
            g_layerMain->m_formDupe->Refresh();
            /*
		if(GetWorld()->m_pPlayer==this)
		{
		if (g_layerMain && g_layerMain->m_formTaskInfo)
		{
		g_layerMain->m_formTaskInfo->Update();
		}
		}
		*/
            break;
    }

    if (GetWorld()->m_pPlayer == this || (GetWorld()->m_pPet == this)) {
        if (g_layerMain) {
            if (g_layerMain->m_formCharProp)
                g_layerMain->m_formCharProp->Update();
            if (g_layerMain->m_formPetProp)
                g_layerMain->m_formPetProp->Update();  //tim.yang  神兽系统
            if (g_layerMain->m_formCharSimple)
                g_layerMain->m_formCharSimple->Update();
            if (g_layerMain->m_formPetSimple)
                g_layerMain->m_formPetSimple->Update();
            if (g_layerMain->m_formTrump)
                g_layerMain->m_formTrump->Refresh();
        }
        if (GetPlayerPet()->GetPetActor() == this && g_layerMain->m_formPetMain->IsVisible() &&
            g_layerMain->m_formPetMain->IsSelectCalledPet()) {
            g_layerMain->m_formPetMain->Refresh();
        }
    }
    unguard;
}

// 参数说明：
// Active - 布尔值，表示是否要激活（装备）武器。如果为 true，则装备武器；如果为 false，则卸下武器。
// Model - 字符串，表示武器的模型名称，用于在图形中显示武器。
// weapon - 指向 SWeapon 结构的指针，包含了武器的详细信息（如模型路径等）。
// item - 传入的 SItemID 结构，包含了武器的 ID 和其他相关信息，用于识别和处理武器。
// lmk 武器装备link点
void GcActor::EquipWeapon(bool Active, const char* Model, SWeapon* weapon, SItemID& item) {
    guard;

    if (!Active) {
        if (mBaseActor.m_pWeapon) {
            // 选择武器Link点
            if (ItemIsWeapon_NeedLink(mBaseActor.m_pWeapon->m_item)) {
                if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
                    GetGraph()->UnLink("Box03");
                } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
                    GetGraph()->UnLink("Bip01 R Hand");
                    GetGraph()->UnLink("Bip01 L Hand");
                } else if (ItemIsWeapon_Pestle(item)) {
                    GetGraph()->UnLink("Box02");  //战士右手单刀
                } else {
                    GetGraph()->UnLink("Bip01 R Hand");

                    // GetGraph()->UnLink("Box01");
                }
            }

            DEL_ONE(mBaseActor.m_pWeapon);
            mBaseActor.m_pWeapon = NULL;
        }
        return;
    } else {
        // 选择武器Link点
        if (ItemIsWeapon_NeedLink(item)) {
            if (ItemIsWeapon_Riband(item)) {
                GetGraph()->Link("Box03", Model);
            } else if (ItemIsWeapon_Hoop(item)) {
                GetGraph()->Link("Bip01 R Hand", Model);
                GetGraph()->Link("Bip01 L Hand", Model);
            } else if (ItemIsWeapon_Pestle(item)) {
                GetGraph()->Link("Box02", Model);
            } else {
                GetGraph()->Link("Bip01 R Hand", Model);
                //GetGraph()->Link("Box01", Model);
            }
        }

        if (mBaseActor.m_pWeapon) {
            DEL_ONE(mBaseActor.m_pWeapon);
            mBaseActor.m_pWeapon = NULL;
        }
        mBaseActor.m_pWeapon = RT_NEW CWeaponShow;
        mBaseActor.m_pWeapon->Init(&mBaseActor, Model, weapon, item);
    }
    unguard;
}

void GcActor::EquipTrump(bool Active, const char* Model, SItemID& item, STrump* pTrump) {
    guard;

    if (!Active) {
        if (mBaseActor.m_pTrump) {
            DEL_ONE(mBaseActor.m_pTrump);
            mBaseActor.m_pTrump = NULL;
        }
        return;
    } else {
        if (mBaseActor.m_pTrump) {
            DEL_ONE(mBaseActor.m_pTrump);
            mBaseActor.m_pTrump = NULL;
        }
        mBaseActor.m_pTrump = RT_NEW CTrumpShow;
        mBaseActor.m_pTrump->Init(&mBaseActor, Model, item, pTrump);
    }
    unguard;
}

// 重置角色头顶显示的名字
void GcActor::ResetHudName() {
    // 获取角色名字
    const char* pName = GetName();
    if (pName && pName[0]) {
        // 如果HUD为空且需要显示HUD,则创建新的HUD
        if (m_pHUD.IsEmpty() && m_bShowHUD) {
            //           m_pHUD = (RtwHUD*)g_workspace.getWidgetFactory()->CreateWidget(g_workspace.GetLayer(0), "", "HUD", RtwWidget::m_DefaultFlag);// 注释 [3/16/2009 tooth.shi]
            // 创建HUD控件
            m_pHUD = (RtwHUD*)g_workspace.getWidgetFactory()->createWidget(wtHud);
            // 添加到工作区
            g_workspace.AddWidget(m_pHUD);
        }

        // 如果HUD存在,设置显示内容
        if (!m_pHUD.IsEmpty()) {
            // 如果是玩家、宠物或召唤NPC
            if (m_eNpcType == ENT_USER || m_eNpcType == ENT_PET || NpcType() == ENT_CALLNPC) {
                /****lxJian*09.12.03****/
                // 如果是中立阵营,只显示名字
                if (m_core.Faction == FACTION_NEUTRAL) {
                    rt2_sprintf(g_strStaticBuffer, "%s", pName);
                    //sprintf(g_strStaticBuffer, "%s", pName);
                }
                // 如果是周或商阵营,显示名字和阵营
                else {
                    rt2_sprintf(g_strStaticBuffer, "%s[%s]", pName,
                                (m_core.Faction == FACTION_ZHOU ? R(G_ZHOU) : R(G_SHANG)));
                    //sprintf(g_strStaticBuffer, "%s", pName);
                }

            }
            // 其他类型NPC只显示名字
            else {
                rt2_sprintf(g_strStaticBuffer, "%s", pName);
            }
            // 设置HUD显示的文本
            m_pHUD->SetNameText((char*)g_strStaticBuffer);
        }
    }
}

void GcActor::SetName(const char* szName) {
    guard;
    if (szName && szName[0]) {
        s_strActorNameMap[this] = szName;
        ResetHudName();
        if (NpcType() == ENT_PET) {
            SetPetHUD();
        }
    }
    unguard;
}

const char* GcActor::GetName() {
    return s_strActorNameMap[this].c_str();
}

void GcActor::Ride(GcActor* pPet, bool useUserPos /* = false*/) {
    guard;
    if (pPet == NULL)
        return;
    if (IsValid() == NULL || pPet->IsValid() == NULL)
        return;

    UnRide();  // 先下来

    SetWeaponTrumpVisible(false, true);
    ClearCommand();
    pPet->ClearCommand();
    pPet->mBaseActor.RemoveSelf();
    if (!pPet->m_pHUD.IsEmpty())
        pPet->m_pHUD->Hide();
    pPet->m_bHideHUD = true;

    // 测试骑乘动作播放
    // mBaseActor.PlayPose(GcBaseActor::POSE_RIDE,true); //骑兽动作
    string ridePose = "wait" + pPet->m_pModel->RidePoseFix;
    mBaseActor.PlayPose((char*)ridePose.c_str(), true);

    if (!useUserPos) {
        RtgMatrix16 matrix  = mBaseActor.m_Matrix;
        mBaseActor.m_Matrix = *(pPet->mBaseActor.GetMatrix());
        mBaseActor.Update();
        if (!mBaseActor.AddSelfFind(mBaseActor.m_Matrix._30, mBaseActor.m_Matrix._31)) {
            mBaseActor.m_Matrix = matrix;
            mBaseActor.Update();
            mBaseActor.AddSelfFind(mBaseActor.m_Matrix._30, mBaseActor.m_Matrix._31);
        }
    } else {
        pPet->mBaseActor.m_Matrix       = mBaseActor.m_Matrix;
        pPet->GetGraph()->p()->m_matrix = mBaseActor.m_Matrix;
        pPet->mBaseActor.Update();
        pPet->mBaseActor.AddSelfFind(mBaseActor.m_Matrix._30, mBaseActor.m_Matrix._31);
    }
    pPet->mBaseActor.PlayPose(GcBaseActor::POSE_IDLE, false);
    GetGraph()->p()->LinkParent(pPet->GetGraph()->p(), "Box");  //图象link到野兽上
    GetGraph()->p()->SetMatrix(RtgMatrix12::matIdentity);

    m_eRideState             = ERS_ACTOR;
    m_pRidePet               = pPet;
    m_pRidePet->m_eRideState = ERS_PET;
    m_pRidePet->m_pMaster    = this;

    if (this == GetPlayer() && g_layerMain->m_formChar->IsVisible() &&
        g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP) {
        g_layerMain->m_formChar->ReCreate3DViewGraph();
    }

    unguard;
}

void GcActor::UnRide() {
    guard;
    if (m_pRidePet == NULL)
        return;
    if (GetGraph()->p() == NULL)
        return;

    ClearCommand();
    m_pRidePet->ClearCommand();

    GcActor* pPet = m_pRidePet;

    m_pRidePet->m_eRideState = ERS_NONE;
    m_pRidePet->m_pMaster    = NULL;
    m_eRideState             = ERS_NONE;
    m_pRidePet               = NULL;

    GetGraph()->p()->UnlinkParent();
    mBaseActor.m_Matrix = *(pPet->mBaseActor.GetMatrix());
    mBaseActor.Update();
    pPet->mBaseActor.AddSelfFind(mBaseActor.m_Matrix._30, mBaseActor.m_Matrix._31);

    mBaseActor.PlayPose(GcBaseActor::POSE_IDLE, true);
    pPet->mBaseActor.PlayPose(GcBaseActor::POSE_IDLE, true);
    if (!pPet->m_pHUD.IsEmpty())
        pPet->m_pHUD->Show(true);
    pPet->m_bHideHUD = false;

    SetWeaponTrumpVisible(true, true);

    if (this == GetPlayer() && g_layerMain->m_formChar->IsVisible() &&
        g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP) {
        g_layerMain->m_formChar->ReCreate3DViewGraph();
    }

    return;
    unguard;
}

void GcActor::DelHUD() {
    if (!m_pHUD.IsEmpty()) {
        //m_pHUD->Close();//rem by fox
        //g_workspace.ClearWidget(g_workspace.GetLayer(0),m_pHUD);// 注释 [3/16/2009 tooth.shi]
        g_workspace.RemoveWidget(m_pHUD->getId());
        m_pHUD->ShowHPBar(false);
        m_pHUD->Hide();
        m_pHUD->drop();
        m_pHUD = NULL;
    }
    m_bShowHUD = false;
}

void GcActor::OnPositionChanged(float fX, float fY) {
    guard;
    int bx, by;
    g_pScene->RtsSceneDoc::GetBlockByPos(fX, fY, bx, by);
    if (!(bx >= 0 && bx < g_pScene->GetBlockCntX() && by >= 0 && by < g_pScene->GetBlockCntY()))
        return;

    if (bx != m_iCurrentBlockX || by != m_iCurrentBlockY) {
        RtsSceneBlockRtb* pBlockMap =
            g_pScene->GetBlockMap(bx - g_pScene->GetCenterX(), by - g_pScene->GetCenterY());
        RtsSceneBlockRtb* pOldBlockMap =
            (m_iCurrentBlockX < 0)
                ? (NULL)
                : (g_pScene->GetBlockMap(m_iCurrentBlockX - g_pScene->GetCenterX(),
                                         m_iCurrentBlockY - g_pScene->GetCenterY()));
        m_iCurrentBlockX = bx;
        m_iCurrentBlockY = by;
        OnBlockChanged(pBlockMap, pOldBlockMap);
    }

    // 区域进入和离开事件
    int                             i, iCnt;
    static RtsSceneBlockRtb::SArea* s_pArea[10];  // 一次最多10个块
    RtsSceneBlockRtb::SArea*        pArea = g_pScene->FindFirstArea(fX, fY);
    for (i = 0; i < 9 && pArea; i++) {
        s_pArea[i] = pArea;
        pArea      = g_pScene->FindNextArea();
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
            pArea = *it;
            m_listArea.erase(it);
            OnAreaExit(pArea, NULL);
        }
    }
    for (i = 0; i < iCnt; i++) {
        if (s_pArea[i]) {
            m_listArea.push_back(s_pArea[i]);
            OnAreaEnter(s_pArea[i], NULL);
        }
    }
    unguard;
}

void GcActor::OnMapChanged() {
    guard;
    std::list<RtsSceneBlockRtb::SArea*>::iterator it;
    for (it = m_listArea.begin(); it != m_listArea.end(); it++) {
        OnAreaExit(*it, NULL);
    }
    m_listArea.clear();
    unguard;
}

void GcActor::OnRemoveCreature() {
    guard;
    RtsSceneBlockRtb* pBlockMap =
        (m_iCurrentBlockX < 0) ? (NULL)
                               : (g_pScene->GetBlockMap(m_iCurrentBlockX - g_pScene->GetCenterX(),
                                                        m_iCurrentBlockY - g_pScene->GetCenterY()));
    OnBlockChanged(NULL, pBlockMap);

    m_iCurrentBlockX = -1;
    m_iCurrentBlockY = -1;

    RtsSceneBlockRtb::SArea*                      pArea;
    std::list<RtsSceneBlockRtb::SArea*>::iterator it, itNext;
    for (it = m_listArea.begin(); it != m_listArea.end(); it = itNext) {
        itNext = it;
        itNext++;
        pArea = *it;
        m_listArea.erase(it);
        OnAreaExit(pArea, NULL);
    }
    m_listArea.clear();
    unguard;
}

void GcActor::OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock) {
    guard;
    if (pNewBlock == NULL || pNewBlock->m_pBlock == NULL)
        return;

    // pk attribure
    SSceneInfo* pSceneInfo = g_TableScene.FindScene(std::string(pNewBlock->m_szFileName));
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
    UpdateAreaPKAttr();
    unguard;
}

const char* s_szPKArea = "PK:";

void GcActor::UpdateAreaPKAttr() {
    guard;
    int                                           iLen;
    char *                                        pStr, *pValue;
    RtsSceneBlockRtb::SArea*                      pArea;
    std::list<RtsSceneBlockRtb::SArea*>::iterator it;
    m_eCurPosPKAttr = m_eBlockPosPKAttr;
    for (it = m_listArea.begin(); it != m_listArea.end(); it++) {
        pArea = (*it);
        pStr  = strstr(const_cast<char*>(pArea->szData), const_cast<char*>(s_szPKArea));
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
    m_eCurPosPKAttr = PKA_NORMAL;
    unguard;
}

void GcActor::OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock) {
    guard;
    UpdateAreaPKAttr();
    unguard;
}

void GcActor::OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock) {
    guard;
    UpdateAreaPKAttr();
    unguard;
}

void GcActor::MagicWeaponBeginAddMp() {
    //SItemID item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
    //SItemID item = g_layerMain->m_formTrump->GetItemID();
    //if(!ItemID_IsValid(item)) return;
    //if(!ItemIsWeapon_MagicWeapon(item.type)) return;
    //if(!ItemIsTrump(item.type)) return;

    //if(g_layerMain->m_formMwAddMp->GetVisible() || g_layerMain->m_formMwAddEle->GetVisible())
    //	return;
    if (g_layerMain->m_formTrump->IsVisiable()) {
        return;
    }

    //g_layerMain->m_formMwAddMp->SetVisible(true);
    //g_layerMain->m_formMwAddMp->Update();
    g_layerMain->m_formTrump->Show();
    g_layerMain->m_formTrump->Refresh();
}

void GcActor::SendTrumpRifine(char rifine, bool stop /* = false */) {
    //if (!ItemID_IsValid(m_RifineTrump) && rifine != PNEUMA_TRUMP )
    //{
    //	return;
    //}

    CG_CmdPacket* cmd = NetBeginWrite();
    cmd->WriteShort(c2r_mw_add_ele);
    if (stop) {
        cmd->WriteByte(1);
    } else {
        cmd->WriteByte(0);
        cmd->WriteByte(rifine);
    }
    NetSend(cmd);
}

void GcActor::SendPickItemType(GcActor* pActor) {
    if (g_layerMain->m_formPickItem->IsVisible())
        return;

    char  str[200];
    float fD = 34.f;
    if (Distance2(pActor) < (fD * fD)) {
        m_lPickActorID = pActor->ID();
        char type      = pActor->m_bIsDead;
        // 1,为捡物品,2,为扒皮
        if (type != 1 && type != 2 && type != 3) {
            return;
        }

        if (!pActor->m_pNpc) {
            return;
        }

        if (type == 2) {
            if (pActor->m_pNpc->Type == 4) {
                if (!m_Skill.FindActiveSkillBySubID(SKILL_DIG_DRUG_SUBID)) {
                    PlayerFailOperate(R(MSG_SKILL_NEEDBAICAO));
                    return;
                } else {
                    SSkill* pSkill = m_Skill.FindSkillBySubID(SKILL_DIG_DRUG_SUBID);
                    if (pSkill->iLevel < pActor->m_pNpc->Level) {
                        rt2_snprintf(str, 199, R(MSG_SKILL_BAICAOLOW), pActor->m_pNpc->Name.c_str(),
                                     pActor->m_pNpc->Level, pSkill->szName);
                        PlayerFailOperate(str);
                        return;
                    }
                }
            } else if (pActor->m_pNpc->Type == 5) {
                if (!m_Skill.FindActiveSkillBySubID(SKILL_DIG_ORZ_SUBID)) {
                    PlayerFailOperate(R(MSG_SKILL_NEEDYEJIN));
                    return;
                } else {
                    SSkill* pSkill = m_Skill.FindSkillBySubID(SKILL_DIG_ORZ_SUBID);
                    if (pSkill->iLevel < pActor->m_pNpc->Level) {
                        rt2_snprintf(str, 199, R(MSG_SKILL_YEJINLOW), pActor->m_pNpc->Name.c_str(),
                                     pActor->m_pNpc->Level, pSkill->szName);
                        PlayerFailOperate(str);
                        return;
                    }
                }
            }
            //采集类任务通用类型
            else if (pActor->m_pNpc->Type == 9) {
                //if(bTaskCollect) return;

                if (!GetWorld()->m_pPlayer)
                    return;
                CTaskInfo* task = &GetWorld()->m_pPlayer->m_task;

                SNpc*                          pNpc = g_TableNpc.FindNpc(pActor->m_pNpc->Id);
                std::list<STaskInfo>::iterator it;
                for (long i = 0; i < pNpc->nTaskNum; i++) {
                    for (it = task->m_taskList.begin(); it != task->m_taskList.end(); it++) {
                        if (it->IsDoing == 0)
                            continue;
                        if (it->IsDoing == 2)
                            continue;
                        STaskDesc* desc = g_TableTask.FindTask((*it).Id);
                        if (!desc)
                            continue;

                        if (desc->id == pNpc->TaskId[i]) {
                            bTaskCollect = true;
                            break;
                        }
                    }
                }
                if (!bTaskCollect) {
                    PlayerFailOperate(R(MSG_TASK_NOCAIJITASK));
                    return;
                }
            } else if (pActor->m_pNpc->Type == 1) {
                if (!m_Skill.FindActiveSkillBySubID(SKILL_DIG_SKIN_SUBID)) {
                    PlayerFailOperate(R(MSG_SKILL_NEEDBOPI));
                    return;
                } else {
                    SSkill* pSkill = m_Skill.FindSkillBySubID(SKILL_DIG_SKIN_SUBID);
                    int     level  = pActor->m_pNpc->Level / 10 + 1;
                    if (pSkill->iLevel < level) {
                        rt2_snprintf(str, 199, R(MSG_SKILL_BOPILOW), pActor->m_pNpc->Name.c_str(),
                                     level, pSkill->szName);
                        PlayerFailOperate(str);
                        return;
                    }
                }
            } else
                return;
        }
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_pickitem_type);
        cmd->WriteLong(m_lPickActorID);
        cmd->WriteByte(type);
        NetSend(cmd);
    } else {
        PlayerFailOperate(R(MSG_DISTANCEISTOOFAR));
    }
}

void GcActor::SendPickItemId(SItemID id, long isMouseRD /* = 0*/) {
    if (!ItemID_IsValid(id) && !isMouseRD) {
        return;
    }
    GcActor* pActor = FindAllActor(m_lPickActorID);
    if (!pActor)
        return;
    float fD = 34.f;
    if (Distance2(pActor) > (fD * fD)) {
        PlayerFailOperate(R(MSG_DISTANCEISTOOFAR));
        if (g_layerMain->m_formPickItem->IsVisible()) {
            g_layerMain->m_formPickItem->Hide();
        }
        return;
    }
    CG_CmdPacket* cmd = NetBeginWrite();
    cmd->WriteShort(c2r_pickitem_id);
    cmd->WriteLong(m_lPickActorID);
    cmd->SerializeBit(isMouseRD, 1);
    if (isMouseRD == 0) {
        cmd->WriteLong(id.id);
    }
    NetSend(cmd);
}

void GcActor::MagicWeaponEndAddMp() {}

void GcActor::MagicWeaponBeginAddEle() {
    //SItemID item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
    //if(!ItemID_IsValid(item)) return;
    //if(!ItemIsWeapon_MagicWeapon(item.type)) return;

    //if(g_layerMain->m_formMwAddMp->GetVisible() || g_layerMain->m_formMwAddEle->GetVisible())
    //	return;
    //g_layerMain->m_formMwAddEle->SetVisible(true);
    //g_layerMain->m_formMwAddEle->Update();
    if (g_layerMain->m_formTrump->IsVisiable()) {
        return;
    }
    g_layerMain->m_formTrump->Show();
    g_layerMain->m_formTrump->Refresh();
}

void GcActor::MagicWeaponBeginAddEle(int type) {}

void GcActor::MagicWeaponEndAddEle() {}

void GcActor::LivingSkillProduce(short v) {
    // 打开 铁匠/制药/符咒制作 的界面
    switch (v) {
        case SKILL_PRODUCE_WEAPON:  // 铁匠
            g_layerMain->m_formItemCompose->Show(ICT_Equipment);
            break;
        case SKILL_PRODUCE_LEECHDOM:  // 制药
            g_layerMain->m_formItemCompose->Show(ICT_Yaoping);
            break;
        case SKILL_PRODUCE_CHARM:  // 符咒制作
            g_layerMain->m_formItemCompose->Show(ICT_EnchantScroll);
            break;
        case SKILL_PRODUCE_EQUIP:
            g_layerMain->m_formItemCompose->Show(ICT_Casting);
            break;
        case SKILL_DECOMPOUND_ITEM:
            g_layerMain->m_formItemDecompound->Show();
            g_workspace.SetFocusWidget(g_layerMain->m_formItemDecompound->OnGetThisForm());
            break;
        default:
            break;
    }
}

void GcActor::LivingSkillSyncretize() {
    // 打开 融合 的界面
    g_layerMain->m_formItemFuse->SetPos(180, 240);
    g_layerMain->m_formItemFuse->Show();
    g_workspace.SetFocusWidget(g_layerMain->m_formItemFuse->OnGetThisForm());
    g_layerMain->m_fromPlayerItems->SetPos(460, 140);
    g_layerMain->m_fromPlayerItems->Show();
}

void GcActor::LivingSkillItemDecompound() {
    // 设置成 分解 的状态
    ((CGameClientFrame*)GetApp())
        ->SetMouseCapture(cti_Decompose, GAME_CURSOR_DEITEM, NULL, R(请选择要分解的装备));
}

bool GcActor::AutoEatMPBottle() {
    CItemContainer* pCon = &(GetWorld()->m_pPlayer->mItem);

    DWORD itemID = 0, itemType = 0;
    if (g_layerMain) {
        for (int p = 0; p <= 1; p++) {
            for (int i = 0; i < UI_HOT_KEY_COUNT; i++) {
                if (g_layerMain->m_fromPlayerItems->GetQuickStartItemData(p, i, itemID, itemType)) {
                    if (ItemIsMPBottle(itemType))
                        break;
                    else
                        itemID = 0;
                }
            }
            if (itemID != 0)
                break;
        }
    }

    SItemID item;
    int     page, i, j;
    if (itemID != 0) {
        if (pCon->m_Bag.FindID(itemID, &page, &i, &j)) {
            item = pCon->m_Bag.GetItem(page, i, j);
        }
    }

    if (ItemID_IsValid(item)) {
        if (g_layerMain && g_layerMain->m_formItemUpdate->IsVisible()) {
            // m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
            return false;
        } else if (g_layerMain && g_layerMain->m_formGem->IsVisible()) {
            return false;
        } else if (g_layerMain && g_layerMain->m_formTrade->IsShowing()) {
            // m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
            return false;
        } else {
            if (!pCon->OperateItem_CheckUIContent(item, IO_Eat))
                return false;
            else if (!pCon->CheckUse(item, GetWorld()->m_pPlayer->m_core))
                return false;
            else {
                CItemNetHelper::NetSend_c2r_player_use_item(
                    item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j);
                return true;
            }
        }
    } else {
        // m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
        return false;
    }
}

bool GcActor::InCastleWarArea() {

#define NUM(array) (sizeof(array) / sizeof((array[0])))

    const std::string SceneNameZhou[] = {"scene01_002.rtb", "scene01_003.rtb", "scene01_004.rtb",
                                         "scene01_005.rtb"};

    const std::string SceneNameShang[] = {"scene01_029.rtb", "scene01_030.rtb", "scene01_035.rtb",
                                          "scene01_036.rtb"};

    RtsSceneBlockRtb* pBlockMap =
        (m_iCurrentBlockX < 0) ? (NULL)
                               : (g_pScene->GetBlockMap(m_iCurrentBlockX - g_pScene->GetCenterX(),
                                                        m_iCurrentBlockY - g_pScene->GetCenterY()));

    if (pBlockMap == NULL) {
        return false;
    }

    std::string map = pBlockMap->m_szFileName;

    for (int i = 0; i < NUM(SceneNameZhou); i++) {
        if (strncmp((char*)map.c_str(), (char*)SceneNameZhou[i].c_str(),
                    strlen((char*)SceneNameZhou[i].c_str())) == 0) {
            return true;
        }
    }

    for (int i = 0; i < NUM(SceneNameShang); i++) {
        if (strncmp((char*)map.c_str(), (char*)SceneNameShang[i].c_str(),
                    strlen((char*)SceneNameShang[i].c_str())) == 0) {
            return true;
        }
    }

    return false;
}

bool GcActor::ProcessTaskKeyList(std::vector<STaskKey>& table, int op, long param) {
    bool failed = false;
    for (size_t i = 0; i < table.size(); i++) {
        if (!ProcessTaskKey(table[i], op, param))
            failed = true;
        if (failed && op == TYPE_CHECK)
            return false;
    }
    return !failed;
}

bool GcActor::ProcessTaskKey(STaskKey& key, int op, long param) {
    if (key.key == "fbprocess") {
        int process = atoi(key.param[0].c_str());
        if (process > m_nFBprocess)
            return false;
        return true;
    }

    if (key.key == "fbitem") {
        long itemId;
        itemId = atol(key.param[0].c_str());
        if (itemId == 0)
            return true;

        long itemNum;
        itemNum = atol(key.param[1].c_str());
        if (itemNum == 0)
            return true;

        int dif = param;
        switch (op) {
            case TYPE_CHECK:
                if (GetDungeonMissionItemNum(itemId, dif) < itemNum)
                    return false;
                else
                    return true;
                break;
            default:
                break;
        }
    } else if (key.key == "ifree") {
        long num;
        num = atol(key.param[0].c_str());
        if (num == 0)
            return true;

        return (mItem.m_Bag.FreeSpace() >= num);
    } else if (key.key == "sreward") {
        return true;
    } else {
        ERR1("unknow key %s", key.key.c_str());
        return false;
    }
    return false;
}

int GcActor::GetDungeonMissionItemNum(DWORD typeId, int difficulty) {
    SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(typeId);
    if (!pItemClass)
        return 0;

    int count = 0;
    if (!ItemIsOther_Dungeon(pItemClass->type)) {
        ERR1("[GcActor::GetDungeonMissionItemNum] Item is NOT a dungeon mission "
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
        if (mItem.m_MissionBag.FindTypeWithParams(typeId, paramIndices, paramParams, ItemColor_All,
                                                  &count))
            return count;
    } else if (ItemIsPet(pItemClass->type)) {
        if (mItem.m_PetBag.FindTypeWithParams(typeId, paramIndices, paramParams, ItemColor_All,
                                              &count))
            return count;
    } else {
        if (mItem.m_Bag.FindTypeWithParams(typeId, paramIndices, paramParams, ItemColor_All,
                                           &count))
            return count;
    }
    return 0;
}

void GcActor::ProcessFixedTaskReward(STaskKey& key, int& goldRew, int& expRew, int& lev,
                                     float& percent, int& itemId, int& itemNum, int& itemColor,
                                     int& skillID) {
    if (key.key == "gold") {
        goldRew   = atol(key.param[0].c_str());
        expRew    = 0;
        lev       = 0;
        percent   = 0;
        itemId    = 0;
        itemNum   = 0;
        itemColor = 0;
        skillID   = 0;
    } else if (key.key == "exp") {
        expRew    = atol(key.param[0].c_str());
        lev       = 0;
        percent   = 0;
        goldRew   = 0;
        itemId    = 0;
        itemNum   = 0;
        itemColor = 0;
        skillID   = 0;
    } else if (key.key == "lvlexpsm") {
        lev = atol(key.param[0].c_str());
        if (lev == 0)
            lev = m_core.Lev;
        percent   = (float)100 * atof(key.param[1].c_str());
        goldRew   = 0;
        expRew    = 0;
        itemId    = 0;
        itemNum   = 0;
        itemColor = 0;
        skillID   = 0;
    } else if (key.key == "item") {
        itemId    = atol(key.param[0].c_str());
        itemNum   = atol(key.param[1].c_str());
        itemColor = atol(key.param[2].c_str());
        goldRew   = 0;
        expRew    = 0;
        lev       = 0;
        percent   = 0;
        skillID   = 0;
    } else if (key.key == "skill") {
        goldRew   = 0;
        expRew    = 0;
        lev       = 0;
        percent   = 0;
        itemId    = 0;
        itemNum   = 0;
        itemColor = 0;
        skillID   = atol(key.param[0].c_str());
    }
}

void GcActor::ProcessRandomTaskReward(STaskKey& key, int* id, int len) {
    if ((key.key == "sreward") && id) {
        for (int i = 0; i < len; i++) {
            id[i] = atol(key.param[i].c_str());
        }
    }
}

//void GcActor::GetFixedReward(int taskId, int& goldRew, int& expRew, int& lev, int& percent)
//{
//	SFbTask *task = g_TableTask.FindFbTask(taskId);
//	if(!task)
//	{
//		goldRew = expRew = lev = percent = 0;
//		return;
//	}
//
//	SFbTaskInfo *info = m_task.GetFbTaskInfo(task->fbId);
//	if(!info)
//	{
//		goldRew = expRew = lev = percent = 0;
//		return;
//	}
//
//	for(size_t i = 0; i < table.size(); i++)
//	{
//		ProcessFixedTaskReward(table[i], goldRew, expRew, lev, percent);
//	}
//}

void GcActor::FbTaskState(int taskId, bool& bFinished, bool& bCanFinishNow) {
    bFinished     = false;
    bCanFinishNow = false;

    SFbTask* task = g_TableTask.FindFbTask(taskId);
    if (!task)
        return;

    SFbTaskInfo* info = m_task.GetFbTaskInfo(task->fbId);
    if (!info)
        return;

    if (task->visDif > info->curDif)
        return;

    if (info->IsTaskFinished(taskId)) {
        bFinished     = true;
        bCanFinishNow = false;
    } else {
        bFinished = false;
        if (ProcessTaskKeyList(task->taskFinReq[info->curDif], TYPE_CHECK, info->curDif))
            bCanFinishNow = true;
        else
            bCanFinishNow = false;
    }
}

bool GcActor::IsAbiMove() {
    GcActor* pPet = GetRidePet();
    if (pPet) {
        return pPet->m_core.IsAbiMove();
    } else {
        return m_core.IsAbiMove();
    }
}

bool GcActor::IsAbiDodge() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.IsAbiDodge();
    else
        return m_core.IsAbiDodge();
}

bool GcActor::IsAbiUseSkill() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.IsAbiUseSkill();
    else
        return m_core.IsAbiUseSkill();
}

bool GcActor::IsAbiNormalAttack() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.IsAbiNormalAttack();
    else
        return m_core.IsAbiNormalAttack();
}

int GcActor::GetAbiResist() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.GetAbiResist();
    else
        return m_core.GetAbiResist();
}

int GcActor::GetAbiStateHit() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.GetAbiStateHit();
    else
        return m_core.GetAbiStateHit();
}

int GcActor::IsAbiBeAttack() {
    GcActor* pPet = GetRidePet();
    if (pPet)
        return pPet->m_core.IsAbiBeAttack();
    else
        return m_core.IsAbiBeAttack();
}

void GcActor::SkillHideModel(char hide) {
    if (hide == 1) {
        // 隐身
        if (this == GetPlayer()) {
            //mBaseActor.GetGraph()->SetVisible(0.3f);
            m_fVisible = 0.3f;
        } else {
            if (m_cInTeam && GetWorld()->m_Team.mData.Exist(GetName()))
                //mBaseActor.GetGraph()->SetVisible(0.3f);
                m_fVisible = 0.3f;
            else {
                //mBaseActor.GetGraph()->SetVisible(0.f);
                //m_fVisible = 0.f;

                //add by yz 2010-6-23:在隐身的同时隐藏hud
                m_fVisible = 0.f;
                if (!m_pHUD.IsEmpty())
                    m_pHUD->Hide();               //隐藏HUD
                if (NULL != mBaseActor.m_pTrump)  //隐藏法宝
                    mBaseActor.m_pTrump->m_bRender = false;
                //end yz
            }
        }
        mBaseActor.GetGraph()->EnableShadow(false);
    } else if (hide == 2) {
        //add by yz 2010-6-23:在隐身的同时隐藏hud
        if (!m_pHUD.IsEmpty())
            m_pHUD->Hide();
        if (NULL != mBaseActor.m_pTrump)
            mBaseActor.m_pTrump->m_bRender = false;
        //end yz

        m_fVisible = 0.f;
        mBaseActor.GetGraph()->EnableShadow(false);
    } else {
        // 显身
        //mBaseActor.GetGraph()->SetVisible(1.f);
        if ((int)m_fVisible == 1)
            return;
        m_fVisible = 1.f;

        //add by yz 2010-6-23:在显身的同时显示其它
        if (!m_pHUD.IsEmpty())
            m_pHUD->Show(true);
        if (NULL != mBaseActor.m_pTrump)
            mBaseActor.m_pTrump->m_bRender = true;
            //end yz

#ifdef KICK_CHEAT
        if (ID() && ID() % 21 == 0) {
            mBaseActor.GetGraph()->EnableShadow(false);
        } else
            mBaseActor.GetGraph()->EnableShadow(true);
#else
        mBaseActor.GetGraph()->EnableShadow(true);
#endif
    }
}

void GcActor::ChangeActorToEffect(const char* effect) {
    //mBaseActor.GetGraph()->SetVisible(0.f);
    m_fVisible = 0.f;
    mBaseActor.GetGraph()->EnableShadow(false);
    if (!effect && m_pChangeEffect) {
        return;
    }
    //mBaseActor.PlayEffect(effect,true,"root",RtgVertex3(0,0,0),true);
    m_pChangeEffect = effect;
}

void GcActor::ChangeEffectToActor() {
    //mBaseActor.GetGraph()->SetVisible(1.f);
    m_fVisible = 1.f;
#ifdef KICK_CHEAT
    if (ID() && ID() % 21 == 0) {
        mBaseActor.GetGraph()->EnableShadow(false);
    }
#else
    mBaseActor.GetGraph()->EnableShadow(true);
#endif
    //if (m_pChangeEffect)
    //{
    //	mBaseActor.StopEffect(m_pChangeEffect);
    //}
    m_pChangeEffect = NULL;
}

void GcActor::SetWeaponTrumpVisible(bool weapon, bool trump)  // 设置武器和法宝是否显示
{
    if (mBaseActor.m_pWeapon) {
        if (ItemIsWeapon_NeedLink(mBaseActor.m_pWeapon->m_item)) {
            GcActorGraph* pGraph = NULL;
            if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
                pGraph = GetGraph()->GetLinkActor("Box03");
            } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
                pGraph = GetGraph()->GetLinkActor("Bip01 R Hand");
                pGraph = GetGraph()->GetLinkActor("Bip01 L Hand");
            } else {
                pGraph = GetGraph()->GetLinkActor("Box01");
            }
            if (pGraph)
                pGraph->DisplayActor(weapon);
        } else {
            mBaseActor.m_pWeapon->m_bRender = weapon;
        }
    }

    if (mBaseActor.m_pTrump) {
        mBaseActor.m_pTrump->m_bRender = trump;
    }
}

void GcActor::ChangeCollectTool(bool totool) {
    if (mBaseActor.m_pTrump) {
        mBaseActor.m_pTrump->m_bRender = !totool;
    }
    if (!mBaseActor.m_pWeapon || GetRideState() == ERS_ACTOR) {
        return;
    }
    if (totool) {
        if (m_pCollectTool[0]) {
            return;
        }
        // 选择武器Link点
        if (ItemIsWeapon_NeedLink(mBaseActor.m_pWeapon->m_item)) {
            if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
                //m_pCollectTool = GetGraph()->GetLinkActorName("Box03");
                rt2_strncpy(m_pCollectTool, GetGraph()->GetLinkActorName("Box03"), 20);
                GetGraph()->UnLink("Box03");
            } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
                //m_pCollectTool = GetGraph()->GetLinkActorName("Bip01 R Hand");
                rt2_strncpy(m_pCollectTool, GetGraph()->GetLinkActorName("Bip01 R Hand"), 20);
                GetGraph()->UnLink("Bip01 R Hand");
                GetGraph()->UnLink("Bip01 L Hand");
            } else {
                //m_pCollectTool = GetGraph()->GetLinkActorName("Box01");
                rt2_strncpy(m_pCollectTool, GetGraph()->GetLinkActorName("Box01"), 20);
                GetGraph()->UnLink("Box01");
            }
        } else
            mBaseActor.m_pWeapon->m_bRender = false;
    } else {
        // 选择武器Link点
        if (ItemIsWeapon_NeedLink(mBaseActor.m_pWeapon->m_item)) {
            if (m_pCollectTool[0]) {
                if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
                    GetGraph()->Link("Box03", m_pCollectTool);
                } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
                    GetGraph()->Link("Bip01 R Hand", m_pCollectTool);
                    GetGraph()->Link("Bip01 L Hand", m_pCollectTool);
                } else {
                    GetGraph()->Link("Box01", m_pCollectTool);
                }
                mBaseActor.m_pWeapon->RevertLinkModel();
                //m_pCollectTool = NULL;
                ZeroMemory(m_pCollectTool, strlen(m_pCollectTool));
            }
        } else
            mBaseActor.m_pWeapon->m_bRender = true;
    }
}

//lyymark 头像设置
RtwImage* GcActor::GetHeadImage(char index) {
    //if ( index < 0 || index > 8)
    //	index = 0;

    char filename[32];
    rt2_sprintf(filename, "%d", (int)index);
    return g_workspace.getImageFactory()->createImage("ui\\x_textures\\pic_role_1");
}

void GcActor::DoTrumpSkill(SCmd cmd) {
    guard;

    if (m_bIsDead > 0)
        return;

    SSkill*            pSkill = NULL;
    float              fD     = 0.f;
    const RtgMatrix16* pM16   = NULL;
    GcActor*           pActor = FindAllActor(cmd.dw[0]);

    if (cmd.cTargetType == SKILL_TARGET_TYPE_ACTOR) {
        if (!pActor || pActor->m_bIsDead > 0)
            return;
    } else if (cmd.cTargetType == SKILL_TARGET_TYPE_POSITION) {
        fD   = cmd.fDistance;
        pM16 = mBaseActor.GetMatrix();
        if (((cmd.f[0] - pM16->_30) * (cmd.f[0] - pM16->_30) +
             (cmd.f[1] - pM16->_31) * (cmd.f[1] - pM16->_31)) > (fD * fD)) {
            return;
        }
    }
    pSkill = Skill()->FindSkill(cmd.wSkillID);
    if (pSkill) {
        if (pSkill->bAlwaysUsable == 0 && !IsAbiUseSkill()) {
            return;
        }
        WORD wCurAttackID = 0;
        bool bCanUseSkill = true;
        bool bIsLocalCmd  = (cmd.fTime < 0.f);

        if (bIsLocalCmd)  // 如果时间为-1就是本地发出来的命令，本地发出的命令就需要上传到服务器
        {
            bCanUseSkill = false;
            if (cmd.cTargetType == SKILL_TARGET_TYPE_NONE) {
                if (m_Skill.UseSkill(pSkill->wID))
                    bCanUseSkill = true;
            } else if (cmd.cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                if (m_Skill.UseSkill(pSkill->wID, pActor->ID()))
                    bCanUseSkill = true;
            } else if (cmd.cTargetType == SKILL_TARGET_TYPE_POSITION) {
                if (m_Skill.UseSkill(pSkill->wID, cmd.f))
                    bCanUseSkill = true;
            }
        }
        if (!bCanUseSkill) {
            LOG1("使用法宝技能失败，原因：%s\n", m_Skill.GetLastErrInfo());
            return;
        }

        //if (cmd.cTargetType==SKILL_TARGET_TYPE_ACTOR)
        //{
        //	mBaseActor.FaceTo(pActor->Matrix()->_30, pActor->Matrix()->_31);
        //}else if (cmd.cTargetType==SKILL_TARGET_TYPE_POSITION)
        //{
        //	mBaseActor.FaceTo(cmd.f[0], cmd.f[1]);
        //}
        if (pSkill->wClass != 13)
            return;

        SItemID  Item1;
        SWeapon* pWeaClass1;
        Item1      = mItem.GetEquipItem(CItemContainer::TRUMP, true);
        pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));

        if (pWeaClass1 && mBaseActor.m_pTrump) {
            if (cmd.cTargetType == SKILL_TARGET_TYPE_POSITION) {
                mBaseActor.m_pTrump->DoSkillAttack(true, 0, cmd.f, pSkill);
            } else if (cmd.cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                mBaseActor.m_pTrump->DoSkillAttack(false, pActor->ID(), NULL, pSkill);
            } else {
                pM16 = mBaseActor.GetMatrix();
                mBaseActor.m_pTrump->DoSkillAttack(true, 0, &(pM16->_30), pSkill);
            }
        }

        if (bIsLocalCmd)  // 如果时间为-1就是本地发出来的命令，本地发出的命令就需要上传到服务器
        {
            SAttack* pAttack = NewAttack();

            pAttack->cAnswer   = 1;
            pAttack->pSkill    = pSkill;
            pAttack->pPoseName = pSkill->szWayName;
            pAttack->cTarget   = cmd.cTargetType;
            if (pAttack->cTarget == SKILL_TARGET_TYPE_ACTOR) {
                pAttack->dwTargetID = cmd.dw[0];
            } else if (pAttack->cTarget == SKILL_TARGET_TYPE_POSITION) {
                pAttack->f[0] = cmd.f[0];
                pAttack->f[1] = cmd.f[1];
                pAttack->f[2] = cmd.f[2];
            }
            wCurAttackID       = m_wAttackID++;
            pAttack->wAttackID = wCurAttackID;
            if (cmd.cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                unsigned short crcvalue = CRC16_INIT_VALUE;
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (cmd.wSkillID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.wSkillID & 0x0FF00) >> 8)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (cmd.dw[0] & 0x000000FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.dw[0] & 0x0000FF00) >> 8)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.dw[0] & 0x00FF0000) >> 16)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.dw[0] & 0xFF000000) >> 24)];
                cmd.wCheck = crcvalue ^ CRC16_XOR_VALUE;
            } else if (cmd.cTargetType == SKILL_TARGET_TYPE_POSITION) {
                DWORD dw1, dw2, dw3;
                dw1                     = cmd.f[0];
                dw2                     = cmd.f[1];
                dw3                     = cmd.f[2];
                unsigned short crcvalue = CRC16_INIT_VALUE;
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (cmd.wSkillID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.wSkillID & 0x0FF00) >> 8)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                crcvalue = (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw1 & 0x000000FF)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x0000FF00) >> 8)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0x00FF0000) >> 16)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw1 & 0xFF000000) >> 24)];
                crcvalue = (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw2 & 0x000000FF)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x0000FF00) >> 8)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0x00FF0000) >> 16)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw2 & 0xFF000000) >> 24)];
                crcvalue = (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (dw3 & 0x000000FF)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x0000FF00) >> 8)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0x00FF0000) >> 16)];
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ ((dw3 & 0xFF000000) >> 24)];
                cmd.dw[0]  = dw1;
                cmd.dw[1]  = dw2;
                cmd.dw[2]  = dw3;
                cmd.wCheck = crcvalue ^ CRC16_XOR_VALUE;
            } else {
                unsigned short crcvalue = CRC16_INIT_VALUE;
                crcvalue =
                    (crcvalue << 8) ^ crctableWorld[(crcvalue >> 8) ^ (cmd.wSkillID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((cmd.wSkillID & 0x0FF00) >> 8)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ (pAttack->wAttackID & 0x00FF)];
                crcvalue = (crcvalue << 8) ^
                           crctableWorld[(crcvalue >> 8) ^ ((pAttack->wAttackID & 0x0FF00) >> 8)];
                cmd.wCheck = crcvalue ^ CRC16_XOR_VALUE;
            }
            if (m_wAttackID == 0)
                m_wAttackID = 1;
            m_listAttack.push_back(pAttack);

            if (m_listAttack.size() > 10) {
                LOG1("GcActor::DoCommand "
                     "攻击缓冲区中的缓冲数据太多了[%d个].将会直接清除一部分\n",
                     m_listAttack.size());
            }
            if (cmd.cTargetType == SKILL_TARGET_TYPE_ACTOR) {
                GetWorld()->OnNetUploadAttackTarget(cmd.wSkillID, pAttack->wAttackID, cmd.dw[0],
                                                    &cmd, cmd.iNetCheck);
            } else if (cmd.cTargetType == SKILL_TARGET_TYPE_POSITION) {
                GetWorld()->OnNetUploadAttackPosition(cmd.wSkillID, pAttack->wAttackID, cmd.dw,
                                                      &cmd, 1);
            } else {
                GetWorld()->OnNetUploadAttackSelf(cmd.wSkillID, pAttack->wAttackID, &cmd, 2);
            }

            // 记录攻击参数，本地累加关键帧
            m_curAttackId    = pAttack->wAttackID;
            m_curAttackEvent = 0;
        } else {
            // 如果非玩家控制角色就说明这是一个网络攻击命令，
            //     这里要查找出这个命令，并且响应他，在PoseBreak或者PoseEnd中删除这个命令
            //     如果这里找不到这个命令，就说明这个命令由于某种原因被删除了
            wCurAttackID = cmd.wAttackID;
            if (!AttackAnswer(wCurAttackID, "")) {
                LOG("DoCommand:ACMD_ATTACK "
                    "响应攻击事件的时候没有找到该事件，该事件可能已经被删除.\n");
            }
            m_curAttackId    = cmd.wAttackID;
            m_curAttackEvent = 0;
        }
        if (!pSkill->szWayName && wCurAttackID != 0) {
            OnAttackEvent(wCurAttackID);
            AttackEnd(wCurAttackID);
        }
    } else {
        LOG1("DoCommand: 使用了一个找不到的技能[ID=%d].\n", cmd.wSkillID);
    }
    unguard;
}

void GcActor::AddDoCommandPose(char command /* = 0 */) {
    switch (command) {
        case DO_COMMAND_GETSKIN:
            m_eIdlePose = GcBaseActor::POSE_SCALP;
            break;
        case DO_COMMAND_GETMINE:
            m_eIdlePose = GcBaseActor::POSE_DIG;
            break;
        case DO_COMMAND_GETMEDICINE:
            m_eIdlePose = GcBaseActor::POSE_GATHER;
            break;
        case DO_COMMAND_RIFINE:
            if (mBaseActor.m_pTrump) {
                mBaseActor.m_pTrump->Refine();
            }
            m_eIdlePose = GcBaseActor::POSE_REFINE;
            break;
        case DO_COMMAND_TASKCOLECT:
            m_eIdlePose = GcBaseActor::POSE_MAKE;
            break;
        default:
            m_eIdlePose = GcBaseActor::POSE_IDLE;
            break;
    }
    if (!m_pCurCmd && !m_listCmd.size()) {
        mBaseActor.PlayPose(m_eIdlePose, false);
    }
}

void GcActor::vUltimateChange(long sV) {
    if (sV == 1) {
        mBaseActor.GetGraph()->SetVisible(0.5f);
    } else {
        mBaseActor.GetGraph()->SetVisible(1.f);
    }
}

bool GcActor::IsShapeshifting() {
    if (GetGraph() && GetGraph()->p() && GetGraph()->p()->IsPlayingPose() &&
        GetGraph()->p()->GetCurrentPose().Name == "skill_q3")
        return true;
    return false;
}

bool GcActor::IsShapeshifted() {
    return m_cShapeshift != 0;
}

bool GcActor::DoTeleportSkill() {
    return false;
}

bool GcActor::ProcessKey(STaskKey& key) {
    return true;
}

bool GcActor::CheckCanStartTrigger(DWORD triggerID, bool checkRemove /* = false */) {
    return true;
}

void GcActor::OnSetWeaponShow(bool bNeed) {
    guard;  // 开始保护块，用于调试或错误处理

    // 调用基类或其他模块的武器显示处理函数
    mBaseActor.OnSetWeaponShow(bNeed);

    // 如果不需要显示武器（bNeed 为 false）
    if (!bNeed) {
        // 如果没有武器，则返回
        if (!mBaseActor.m_pWeapon)
            return;

        // 如果武器的 ID 有效
        if (ItemID_IsValid(mBaseActor.m_pWeapon->m_item)) {
            char       Sex        = m_core.Sex;  // 获取角色的性别
            SItemBase* pItemClass = NULL;        // 用于存储物品类指针
            SWeapon*   pWeapon    = NULL;        // 用于存储武器指针

            // 根据武器的类型获取物品类
            pItemClass = GetWorld()->m_pItemManager->GetItem(mBaseActor.m_pWeapon->m_item.type);
            if (!pItemClass)
                return;

            // 将物品类转换为武器类
            pWeapon = (SWeapon*)pItemClass;

            // 根据性别选择对应的武器模型路径
            if (Sex) {
                // 女模型
                // m_pMaster->EquipWeapon(true, pWeapon->Models1[mBaseActor.m_pWeapon->m_item.level], pWeapon, item);
                rt2_strncpy(m_linkName, pWeapon->Models1[mBaseActor.m_pWeapon->m_item.level], 20);
            } else {
                // 男模型
                // m_pMaster->EquipWeapon(true, pWeapon->Models2[mBaseActor.m_pWeapon->m_item.level], pWeapon, item);
                rt2_strncpy(m_linkName, pWeapon->Models2[mBaseActor.m_pWeapon->m_item.level], 20);
            }
        }

        // 根据武器类型解除与角色模型的绑定
        if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
            // 解除绑定 "Box03"
            // rt2_strncpy(linkName, GetGraph()->GetLinkActorName("Box03"), 20);
            GetGraph()->UnLink("Box03");
        } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
            // 解除绑定 "Bip01 R Hand" 和 "Bip01 L Hand"
            // rt2_strncpy(linkName, GetGraph()->GetLinkActorName("Bip01 R Hand"), 20);
            GetGraph()->UnLink("Bip01 R Hand");
            GetGraph()->UnLink("Bip01 L Hand");
        } else {
            // 解除绑定 "Box01"
            // rt2_strncpy(linkName, GetGraph()->GetLinkActorName("Box01"), 20);
            GetGraph()->UnLink("Box01");
        }
    } else {
        // 如果需要显示武器（bNeed 为 true）
        if (m_linkName[0] && mBaseActor.m_pWeapon) {
            // 根据武器类型进行绑定
            if (ItemIsWeapon_Riband(mBaseActor.m_pWeapon->m_item)) {
                GetGraph()->Link("Box03", m_linkName);
            } else if (ItemIsWeapon_Hoop(mBaseActor.m_pWeapon->m_item)) {
                GetGraph()->Link("Bip01 R Hand", m_linkName);
                GetGraph()->Link("Bip01 L Hand", m_linkName);
            } else {
                //lyymark 2024.8.11 修改武器 斧头 link点
                GetGraph()->Link("Box01", m_linkName);
                //GetGraph()->Link("Bip01 R Hand", m_linkName);
            }

            // 调用武器的恢复绑定模型方法
            mBaseActor.m_pWeapon->RevertLinkModel();

            // 清空 m_linkName
            ZeroMemory(m_linkName, strlen(m_linkName));
        }
    }

    unguard;  // 结束保护块
}

void GcActor::SetPetHUD() {
    if (!m_pHUD)
        return;

    string szPetAbility;
    string szPetFuseLev;
    string szPetMaster;
    DWORD  dwPetFuseLevColor = 0xFFFFFFFF;
    DWORD  dwPetAbilityColor = 0xFF99FF33;
    DWORD  dwPetMasterColor  = 0xFF63B8FF;

    switch (m_ePetType) {
        case PET_TYPE_NORMAL: {
            dwPetFuseLevColor = 0xFF25A988;
            if (m_dwPetFuseLev == 0) {
                szPetFuseLev = "初阶融合宠";
            } else if (m_dwPetFuseLev == 1) {
                szPetFuseLev = "一阶融合宠";
            } else if (m_dwPetFuseLev == 2) {
                szPetFuseLev = "二阶融合宠";
            } else if (m_dwPetFuseLev == 3) {
                szPetFuseLev = "三阶融合宠";
            } else  //if (m_dwPetFuseLev == 5)
            {
                szPetFuseLev = "四阶融合宠";
            }
        } break;
        case PET_TYPE_FAIRY: {
            dwPetFuseLevColor = 0xFF7E36B4;
            if (m_dwPetFuseLev == 0) {
                szPetFuseLev = "初阶仙兽";
            } else if (m_dwPetFuseLev == 1) {
                szPetFuseLev = "一阶仙兽";
            } else if (m_dwPetFuseLev == 2) {
                szPetFuseLev = "二阶仙兽";
            } else  //if (m_dwPetFuseLev == 4)
            {
                szPetFuseLev = "三阶仙兽";
            }
        } break;
        case PET_TYPE_SPECTRE: {
            dwPetFuseLevColor = 0xFF6F3D35;
            if (m_dwPetFuseLev < 1) {
                szPetFuseLev = "初阶幽灵";
            } else if (m_dwPetFuseLev < 6) {
                szPetFuseLev = "一阶幽灵";
            } else if (m_dwPetFuseLev < 11) {
                szPetFuseLev = "二阶幽灵";
            } else if (m_dwPetFuseLev < 16) {
                szPetFuseLev = "三阶幽灵";
            } else  //if (m_dwPetFuseLev < 1)
            {
                szPetFuseLev = "四阶幽灵";
            }
        } break;
    }

    if (m_dwPetAbilityLev < 1) {
        szPetAbility = "野兽";
    } else if (m_dwPetAbilityLev < 4) {
        szPetAbility = "灵兽";
    } else if (m_dwPetAbilityLev < 7) {
        szPetAbility = "圣兽";
    } else if (m_dwPetAbilityLev < 10) {
        szPetAbility = "神兽";
    } else  //if (m_dwPetFuseLev < 1)
    {
        szPetAbility = "吞天兽";
    }

    GcActor* pMaster = FindAllActor(m_dwPetMasterID);
    if (pMaster) {
        szPetMaster = pMaster->GetName();
        szPetMaster += " 驯养";
    }

    m_pHUD->SetPetFlag(true, szPetAbility, dwPetAbilityColor, szPetFuseLev, dwPetFuseLevColor,
                       szPetMaster, dwPetMasterColor);
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

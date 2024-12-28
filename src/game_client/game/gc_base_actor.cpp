#include "gc_include.h"
#include "gc_base_actor.h"
#include "game_assistant.h"
#include "gc_camera.h"

#include "gc_beeline_missile.h"
#include "gc_track_missile.h"
#include "gc_whirl_missile.h"

#include "gc_circle_missile.h"
#include "gc_track_2_missile.h"
#include "trump_show.h"
#include "weapon_show.h"

#ifndef PUBLISH
bool GcBaseActor::m_bDrawBoundBox = false;
#endif

//
//	class GcRenderActorGraph
//

RT_IMPLEMENT_DYNAMIC(GcRenderActorBase, RtsRenderAgentShadow, 0, "gc")

int GcRenderActorBase::s_iShadowQuality = 1;

GcRenderActorBase::~GcRenderActorBase() {}

static long idrender = 0;

void GcRenderActorBase::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask) {
    m_pGraph->Render(inDevice, eMask);
}

void GcRenderActorBase::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                        RtgVertex3& scale) {
    m_pGraph->RenderHighLight(inCamera, inDevice, color, scale);
}

void GcRenderActorBase::AgentRenderShader() {
    if (m_pGraph->m_pMaster->GetRideState() == GcActor::ERS_ACTOR) {
        return;
    }

    RtgVertex3 color(0, 0.5, 0);
    switch (s_iShadowQuality) {
        case 0:  // 无阴影
            break;
        default:
        case 1:  // 全部都是圈
            m_pGraph->GetGraph()->RenderShadow(*GetDevice(), FALSE,
                                               m_pGraph->m_pMaster->GetModel()->ModelRadius);
            break;
        case 2:  // 自己和兽动态其他圈
            if (m_pGraph->m_pMaster == GetPlayer() || m_pGraph->m_pMaster == GetWorld()->m_pPet) {
                m_pGraph->GetGraph()->RenderShadow(*GetDevice(), TRUE,
                                                   m_pGraph->m_pMaster->GetModel()->ModelRadius);
            } else {
                m_pGraph->GetGraph()->RenderShadow(*GetDevice(), FALSE,
                                                   m_pGraph->m_pMaster->GetModel()->ModelRadius);
            }
            break;
        case 3:  // 全部动态
            if (!m_pGraph->m_pMaster->IsTrigger()) {
                m_pGraph->GetGraph()->RenderShadow(*GetDevice(), TRUE,
                                                   m_pGraph->m_pMaster->GetModel()->ModelRadius);
            }
            break;
    }
    if (m_pGraph->m_pMaster->ID() == GetWorld()->m_selectedActor) {
        if (m_pGraph->m_pMaster->IsNPC())  // sys npc
        {
            color.Set(0, 1, 1);
        } else if (m_pGraph->m_pMaster->NpcType() == GcActor::ENT_USER ||
                   m_pGraph->m_pMaster->NpcType() == GcActor::ENT_PET ||
                   m_pGraph->m_pMaster->NpcType() == GcActor::ENT_CALLNPC)  // user and pet ?
        {
            color.Set(1, 1, 1);
        } else  // monster
        {
            int userLev = GetWorld()->m_pPlayer->m_core.Lev;
            int tarLev  = m_pGraph->m_pMaster->GetMonsterLevel();
            if (tarLev > userLev + 5) {
                color.Set(1, 0, 0);
            } else if (tarLev > userLev) {
                color.Set(1, 1, 0);
            } else if (tarLev > userLev - 5) {
                color.Set(0, 1, 0);
            } else {
                color.Set(0, 0, 1);
            }
        }
        if (m_pGraph->m_pMaster->m_bRenderSelect)
            m_pGraph->GetGraph()->RenderSelect(*GetDevice(), color,
                                               m_pGraph->m_pMaster->GetModel()->ModelRadius);
    }
}

CRT_ActorInstance* GcRenderActorBase::GetActor() {
    return m_pGraph->GetGraph()->p();
}

RT_IMPLEMENT_DYNAMIC(GcStateOverHead, RtsRenderAgent, 0, "gc")

GcStateOverHead::GcStateOverHead() {
    m_pTaskGraph1     = NULL;
    m_pTaskGraph2     = NULL;
    m_pTaskGraph3     = NULL;
    m_pActor          = NULL;
    m_pRender         = NULL;
    m_dwLastQueryTime = 0;
}

GcStateOverHead::~GcStateOverHead() {}

void GcStateOverHead::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask) {
    if (m_pRender) {
        RtgVertex3 v = m_pActor->GetHUDPos();

        RtgMatrix16 m16;
        m16.Unit();
        m16._30 = v.x;
        m16._31 = v.y;
        m16._32 = v.z + 28;
        m_pRender->SetMatrix(m16);
        m_pRender->Render(&inDevice, eMask);
    }
}

void GcStateOverHead::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                      RtgVertex3& scale) {
    if (m_pRender) {
        RtgVertex3 v = m_pActor->GetHUDPos();

        RtgMatrix16 m16;
        m16.Unit();
        m16._30 = v.x;
        m16._31 = v.y;
        m16._32 = v.z + 48;
        m_pRender->SetMatrix(m16);

        m_pRender->RenderHighLight(&inDevice, color, scale);
    }
}

void GcStateOverHead::OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene) {
    guard;
    if (m_pRender)
        pScene->AddRenderObject(this);
    unguard;
}

void GcStateOverHead::NetSend_c2r_query_task_overhead(DWORD npcOID) {
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_query_task_overhead);
    pPacket->WriteLong(npcOID);
    NetSend(pPacket);
}

void GcStateOverHead::Run(float fSecond) {
    // Update Graphics

    // Query New State
    // fox by 跑环任务//头顶提示
    DWORD tick = rtGetMilliseconds();

    if (m_pRender)
        m_pRender->Tick(fSecond * 1000.f);

    if (tick - m_dwLastQueryTime >= TASK_OVERHEAD_QUERY_PERIOD) {
        m_dwLastQueryTime = tick;
        long nTemp        = m_pActor->ID();
        NetSend_c2r_query_task_overhead(nTemp);
        // if( m_pActor && m_pActor->m_pNpc &&
        // (!m_pActor->m_pNpc->FinishTaskId.empty() ||
        // !m_pActor->m_pNpc->GetTaskId.empty()) )
        //{
        //	//在这个NPC处可完成的任务
        //          for(int i=0; i<m_pActor->m_pNpc->FinishTaskId.size(); i++)
        //	{
        //		if(GetWorld()->m_pPlayer->m_task.IsTaskAccept(m_pActor->m_pNpc->FinishTaskId[i]))
        //		{
        //			//RefreshGraph(TOH_HaveAccept);//by fox for run task
        //			NetSend_c2r_query_task_overhead(m_pActor->ID());//add
        // fox for run task 			return;
        //		}
        //	}
        //	//在这个NPC处可接受的任务
        //	for(int i=0; i<m_pActor->m_pNpc->GetTaskId.size(); i++)
        //	{
        //		DWORD taskID = m_pActor->m_pNpc->GetTaskId[i];
        //		int nPlayerLevel = GetWorld()->m_pPlayer->m_core.Lev;
        //		STaskDesc *pTaskDesc = g_TableTask.FindTask(taskID);
        //		if(!pTaskDesc)
        //			continue;

        //		// check if can accept,do some extra check to save cpu
        //		if((nPlayerLevel >= pTaskDesc->reqMinLev) && (nPlayerLevel <=
        // pTaskDesc->reqMaxLev))
        //		{
        //			NetSend_c2r_query_task_overhead(m_pActor->ID());
        //			return;
        //		}
        //	}
    }
    // RefreshGraph(TOH_Unknown);
    //}
}

void GcStateOverHead::RefreshGraph(ETaskOverHead state) {
    guard;

    static std::string strAccept = "pic_f_icon11.act";  // 可接受的
    static std::string strDoing  = "pic_f_icon12.act";  // 正在进行中
    static std::string strFinish = "pic_f_icon13.act";  // 可完成的

    if (state == TOH_AcceptAndFinish) {
        if (!m_pTaskGraph2) {
            m_pTaskGraph2 = RtcGetActorManager()->CreateActor(strFinish.c_str());
            if (m_pTaskGraph2 == NULL) {
                ERR1("[GcActor::Render] 无法载入任务指示模型，name=%s.\n", strFinish.c_str());
                return;
            }
        }
        m_pRender = m_pTaskGraph2;
    } else if (state == TOH_CanAccept) {
        if (!m_pTaskGraph1) {
            m_pTaskGraph1 = RtcGetActorManager()->CreateActor(strAccept.c_str());
            if (m_pTaskGraph1 == NULL) {
                ERR1("[GcActor::Render] 无法载入任务指示模型，name=%s.\n", strAccept.c_str());
                return;
            }
        }
        m_pRender = m_pTaskGraph1;
    } else if (state == TOH_AcceptNoFinish) {
        if (!m_pTaskGraph3) {
            m_pTaskGraph3 = RtcGetActorManager()->CreateActor(strDoing.c_str());
            if (m_pTaskGraph3 == NULL) {
                ERR1("[GcActor::Render] 无法载入任务指示模型，name=%s.\n", strDoing.c_str());
                return;
            }
        }
        m_pRender = m_pTaskGraph3;
    } else if (state == TOH_CanNotAccept) {
        m_pRender = NULL;
    } else {
        m_pRender = NULL;
    }

    unguard;
}

void GcStateOverHead::Clear() {
    g_pScene->RemoveObjectByLastRenderList(this);

    if (m_pTaskGraph1)
        RtcGetActorManager()->ReleaseActor(m_pTaskGraph1);
    if (m_pTaskGraph2)
        RtcGetActorManager()->ReleaseActor(m_pTaskGraph2);
    if (m_pTaskGraph3)
        RtcGetActorManager()->ReleaseActor(m_pTaskGraph3);

    m_pRender = NULL;
}

// 添加NPC头顶动态提示
RT_IMPLEMENT_DYNAMIC(GcActorOverHead, RtsRenderAgent, 0, "gc")

GcActorOverHead::GcActorOverHead() {
    m_pActorGraph = NULL;
    m_pActor      = NULL;
    m_pRender     = NULL;
}

GcActorOverHead::~GcActorOverHead() {}

void GcActorOverHead::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask) {
    if (m_pRender) {
        RtgVertex3 v = m_pActor->GetHUDPos();

        RtgMatrix16 m16;
        m16.Unit();
        m16._30 = v.x;
        m16._31 = v.y;
        m16._32 = v.z + 18;
        m_pRender->SetMatrix(m16);

        m_pRender->Render(&inDevice, eMask);
    }
}

void GcActorOverHead::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                      RtgVertex3& scale) {
    if (m_pRender) {
        RtgVertex3 v = m_pActor->GetHUDPos();

        RtgMatrix16 m16;
        m16.Unit();
        m16._30 = v.x;
        m16._31 = v.y;
        m16._32 = v.z + 28;
        m_pRender->SetMatrix(m16);

        m_pRender->RenderHighLight(&inDevice, color, scale);
    }
}

void GcActorOverHead::OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene) {
    guard;

    if (m_pRender) {
        RtgVertex3 vPos = m_pActor->GetMatrix()->Position();
        pScene->AddRenderObject(this);
    }

    unguard;
}

void GcActorOverHead::Run(float fSecond) {
    if (m_pRender)
        m_pRender->Tick(fSecond * 1000.f);
}

void GcActorOverHead::NewActorOverHead(std::string strHint) {
    guard;

    if (strHint.empty())
        return;

    if (!strHint.empty()) {
        strHint += ".act";
        if (!m_pActorGraph) {
            m_pActorGraph = RtcGetActorManager()->CreateActor(strHint.c_str());
            if (m_pActorGraph == NULL) {
                ERR1("[GcActor::Render] 无法载入人物头顶指示模型，name=%s.\n", strHint.c_str());
                return;
            }
            // if (!m_pActorGraph->Create())
            //{
            //	ERR1("[GcActor::Render] 无法创建人物头顶指示模型，name=%s.\n",
            // strHint.c_str()); 	UnloadActorInstance(m_pActorGraph);
            //	DEL_ONE(m_pActorGraph);
            //	return;
            // }
        }
        m_pRender = m_pActorGraph;
    }

    unguard;
}

void GcActorOverHead::Clear() {
    g_pScene->RemoveObjectByLastRenderList(this);

    if (m_pActorGraph)
        RtcGetActorManager()->ReleaseActor(m_pActorGraph);

    m_pRender = NULL;
}

const int DEFAULT_WALK_SPEED = 80.0f;

GcBaseActor::GcBaseActor(GcActor* vpMaster)
    : m_Matrix(), mGraph(), mpModel(NULL), m_Time(0.0), m_LastTileX(0), m_LastTileY(0),
      m_CurrentState(NULL_STATE), m_RideState(NO_RIDE_STATE), m_pMaster(vpMaster), m_eftList(),
      mAttackTargetID(0) {
    CM_MEMPROTECTOR(m_poseName, 50)
    CM_MEMPROTECTOR(m_szLastPoseName, 40)
    m_fMoveSpeed             = DEFAULT_WALK_SPEED;
    m_bMoveToPositionNewMove = TRUE;
    m_fMoveToPositionOldX    = 0.f;
    m_fMoveToPositionOldY    = 0.f;
    m_fMoveToPositionTime    = 0.f;
    m_fMoveToPositionSpeed   = 0.f;
    m_fMoveToPositionSpeedX  = 0.f;
    m_fMoveToPositionSpeedY  = 0.f;
    m_bPutOnPathFinder       = 0;
    m_Render.SetGraph(this);
    m_Matrix.Unit();
    m_pPathNode         = NULL;
    m_pTrump            = NULL;
    m_pWeapon           = NULL;
    m_bLastLoop         = false;
    m_szLastPoseName[0] = 0;
    m_pStateOverHead    = NULL;
    m_pActorOverHead    = NULL;
    m_fLastDistance     = 1000000.f;
    m_isDrawPathLine    = 0;
    RtIni iniUser;  // tim.yang  读取是否绘制路线的配置文件变量
    if (iniUser.OpenFile(R(INI_GAME))) {
        iniUser.GetEntry("pathline", "pathline", &m_isDrawPathLine);
    }
    // LOG("==>创建GcBaseActor\n");
    m_pPathFind      = 0;
    m_vPoseID        = POSE_IDLE;
    m_pItemEffect    = NULL;
    m_bRender        = true;
    m_blockType      = PB_UNBLOCK;
    m_fLastMoveSpeed = 0.f;
}

GcBaseActor::~GcBaseActor() {
    guard;

    CM_MEMUNPROTECTOR(m_poseName)
    CM_MEMUNPROTECTOR(m_szLastPoseName)

    CHECK(m_bPutOnPathFinder == 0);
    if (m_pPathNode) {
        ERR("GcBaseActor::~GcBaseActor 没有从PathFind中清除干净\n");
    }
    // LOG("==>销毁GcBaseActor\n");
    unguard;
}

bool GcBaseActor::CreateGraphData(SCreModel* vpModel, short HeadModelID) {
    guard;

    if (m_pMaster && m_pMaster->m_pNpc &&
        (!m_pMaster->m_pNpc->GetTaskId.empty() || !m_pMaster->m_pNpc->FinishTaskId.empty())) {
        if (!m_pStateOverHead) {
            m_pStateOverHead           = RT_NEW GcStateOverHead();
            m_pStateOverHead->m_pActor = m_pMaster;
        }
    }

    if (!m_pActorOverHead) {
        m_pActorOverHead           = RT_NEW GcActorOverHead();
        m_pActorOverHead->m_pActor = m_pMaster;
    }
    m_bLastLoop         = false;
    m_szLastPoseName[0] = 0;

    // LOG1("==>CreateGraphData %s\n",
    // (vpModel==NULL)?"==================异常==============":"OK");
    if (mpModel && mpModel != vpModel)
        UnLoadAllSkin();
    mpModel = vpModel;
    return SetGraph(vpModel, HeadModelID, vpModel->Act.c_str());
    unguard;
}

/**
 * @brief 设置图形
 *
 * @param HeadModelID 头模型ID
 * @param vpActorName 模型文件名称
 * @return true
 * @return false
 */
bool GcBaseActor::SetGraph(short HeadModelID, const char* vpActorName) {
    guard;
    return SetGraph(mpModel, HeadModelID, vpActorName);
    unguard;
}

/**
 * @brief 设置图形
 *
 * @param vpModel 与后端通用的模型数据
 * @param HeadModelID 头模型ID
 * @param vpActorName 模型文件名称
 * @return true
 * @return false
 */
bool GcBaseActor::SetGraph(SCreModel* vpModel, short HeadModelID, const char* vpActorName) {
    mGraph.ReplaceActor(vpActorName);

    if (vpModel->bComplex && !m_pMaster->m_cShapeshiftNet) {
        // Modified by Wayne Wong 2010-12-29 for temp demo
        // mGraph.UnloadAllSkin();
        if (mGraph.p()) {
            {
                list<void*>&          listCommand = mGraph.p()->m_listCommand;
                list<void*>::iterator _it         = listCommand.begin();

                while (_it != listCommand.end()) {
                    Rac_CmdType* _cmd = (Rac_CmdType*)(*_it);

                    if (GetActorCmdtype(_cmd) == Ractype_LoadSub &&
                        ((Rac_LoadSub*)_cmd)->stype == Rac_LoadSub::_sub_skin &&
                        ((Rac_LoadSub*)_cmd)->otype == Rac_LoadSub::_load) {
                        if (((Rac_LoadSub*)_cmd)->sname != "")
                            _it = listCommand.erase(_it);
                        else
                            ++_it;
                    }
                }
            }

            {
                vector<CRT_SkinInstance*>&          skinList = mGraph.p()->m_skinList;
                vector<CRT_SkinInstance*>::iterator _it      = skinList.begin();
                while (_it != skinList.end()) {
                    CRT_SkinInstance* skinInstance = *_it;
                    if (skinInstance->m_Name != "") {
                        _it = skinList.erase(_it);
                        RtcGetActorManager()->ReleaseSkin(skinInstance);
                    } else {
                        ++_it;
                    }
                }
            }
        }

        string headModelStr;

        if (HeadModelID) {
            SHeadModel headModel;

            if (g_TableHeadModel.GetHeadModelByID(HeadModelID, headModel))
                headModelStr = headModel.skin;
            else
                headModelStr = vpModel->HeadSkin;
        } else {
            headModelStr = vpModel->HeadSkin;
        }

        if (m_strComplexHead != headModelStr) {
            if (!m_strComplexHead.empty())
                mGraph.UnloadSkin(m_strComplexHead.c_str());
            m_strComplexHead = headModelStr;
            mGraph.LoadSkin(m_strComplexHead.c_str(), true);
        }

        if (m_strComplexBody != vpModel->BodySkin) {
            if (!m_strComplexBody.empty())
                mGraph.UnloadSkin(m_strComplexBody.c_str());
            m_strComplexBody = vpModel->BodySkin;
            mGraph.LoadSkin(m_strComplexBody.c_str(), false);
        }

        if (m_strComplexHand != vpModel->HandSkin) {
            if (!m_strComplexHand.empty())
                mGraph.UnloadSkin(m_strComplexHand.c_str());
            m_strComplexHand = vpModel->HandSkin;
            mGraph.LoadSkin(m_strComplexHand.c_str(), false);
        }

        if (m_strComplexFoot != vpModel->FootSkin) {
            if (!m_strComplexFoot.empty())
                mGraph.UnloadSkin(m_strComplexFoot.c_str());
            m_strComplexFoot = vpModel->FootSkin;
            mGraph.LoadSkin(m_strComplexFoot.c_str(), false);
        }
    } else {
        if (!vpModel->AddedSkin.empty())
            mGraph.LoadSkin(vpModel->AddedSkin.c_str(), false);

        if (mGraph.p())
            if (!vpModel->ReplaceMtl.empty())
                mGraph.p()->ChangeMaterial(0, vpModel->ReplaceMtl.c_str());
    }

    if (mGraph.p()) {
        RtgVertex3 vScale(vpModel->ModelScale);
        mGraph.p()->SetDrawScale(vScale);

#ifdef KICK_CHEAT
        if (m_pMaster->ID() && m_pMaster->ID() % 21 == 0) {
            GetGraph()->p()->m_bRenderEffect = false;
            GetGraph()->p()->SetOffset(RtgVertex3(0, 0, -10000));
            GetGraph()->p()->m_bEnableShadow = false;
            m_pMaster->DelHUD();
            m_pMaster->m_bRenderSelect = false;
        } else {
            if (vpModel->LinkEffect.length() > 0)
                PlayEffect(vpModel->LinkEffect.c_str(), true, vpModel->LinkPoint.c_str(),
                           RtgVertex3(0, 0, 0), true);
        }
#else
        if (vpModel->LinkEffect.length() > 0)
            PlayEffect(vpModel->LinkEffect.c_str(), true, vpModel->LinkPoint.c_str(),
                       RtgVertex3(0, 0, 0), true);
#endif
    }

    mGraph.RegisterActorNotify(this);  // 注册通知
    Update();

    return true;
}

void GcBaseActor::UnLoadAllSkin() {
    guard;

    if (!m_strComplexHead.empty()) {
        mGraph.UnloadSkin(m_strComplexHead.c_str());
        m_strComplexHead.clear();
    }
    if (!m_strComplexBody.empty()) {
        mGraph.UnloadSkin(m_strComplexBody.c_str());
        m_strComplexBody.clear();
    }
    if (!m_strComplexHand.empty()) {
        mGraph.UnloadSkin(m_strComplexHand.c_str());
        m_strComplexHand.clear();
    }
    if (!m_strComplexFoot.empty()) {
        mGraph.UnloadSkin(m_strComplexFoot.c_str());
        m_strComplexFoot.clear();
    }
    unguard;
}

bool GcBaseActor::DestroyGraphData() {
    guard;
    mGraph.RegisterActorNotify(NULL);
    mGraph.ClearActor();

    ClearEffect();

    if (m_pTrump) {
        DEL_ONE(m_pTrump);
        m_pTrump = NULL;
    }
    if (m_pWeapon) {
        DEL_ONE(m_pWeapon);
        m_pWeapon = NULL;
    }
    if (m_pStateOverHead) {
        m_pStateOverHead->Clear();
        DEL_ONE(m_pStateOverHead);
        m_pStateOverHead = NULL;
    }
    if (m_pActorOverHead) {
        m_pActorOverHead->Clear();
        DEL_ONE(m_pActorOverHead);
        m_pActorOverHead = NULL;
    }

    RtcGetActorManager()->ReleaseActor(m_pItemEffect);
    m_pItemEffect = NULL;
    return true;
    unguard;
}

bool GcBaseActor::MoveInTile(float vX, float vY) {
    guard;
    CHECK(m_bPutOnPathFinder != 0);
    RtsPathFind* pPathFind = GetWorld()->GetPathFind();  // 获取当前世界的路径查找器
    int          TileX, TileY;

    Point2Tile(vX, vY, TileX, TileY);  // 将世界坐标 vX, vY 转换为逻辑格子坐标 TileX, TileY
    // 检查角色是否移动到了新的格子

    if (TileX != m_LastTileX || TileY != m_LastTileY) {
        char cBlock = '0';
        // if (m_pMaster && m_pMaster == GetPlayer())
        cBlock = (char)m_blockType;  // 将阻挡类型设置为角色的阻挡类型
        // 使用路径查找器尝试将角色移动到新格子

        if (pPathFind->MoveTo(m_pPathNode, TileX, TileY, cBlock)) {
            m_LastTileX = TileX;  // 如果移动成功，更新最后所在的逻辑格子坐标

            m_LastTileY = TileY;
        } else {
            return false;
        }
    }
    return true;
    unguard;
}

void GcBaseActor::Update()  // 将位置和方向更新到图形部分
{
    mGraph.SetPosition(m_Matrix._30, m_Matrix._31, m_Matrix._32);
    mGraph.SetRotation(m_Matrix);
}

void GcBaseActor::FaceTo(float vX, float vY) {
    guard;
    RtgVertex3 Pos(vX, vY, m_Matrix._32);
    m_Matrix.FaceToVertex(Pos);
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        m_pMaster->m_pRidePet->mBaseActor.m_Matrix = m_Matrix;
        m_pMaster->m_pRidePet->mBaseActor.mGraph.SetRotation(m_Matrix);
    } else {
        mGraph.SetRotation(m_Matrix);
    }
    unguard;
}

bool GcBaseActor::SetPosition(float vX, float vY) {
    guard;
    // 调用 MoveInTile 函数将角色移动到指定的逻辑格子位置
    if (!MoveInTile(vX, vY))  // 逻辑格子的位置
    {
        m_bMoveToPositionNewMove = TRUE;
        return false;
    }
    // 更新角色矩阵的平移分量，即角色在世界中的 X 和 Y 坐标
    m_Matrix._30 = vX;
    m_Matrix._31 = vY;
    // 获取并更新角色在当前 X, Y 位置上的高度 (Z 坐标)
    g_pScene->GetTerrainHeight(m_Matrix._30, m_Matrix._31, m_Matrix._32);
    // 如果角色正在骑乘状态，将宠物的矩阵与角色矩阵同步
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        m_pMaster->m_pRidePet->mBaseActor.m_Matrix = m_Matrix;
        m_pMaster->m_pRidePet->mBaseActor.mGraph.SetPosition(m_Matrix._30, m_Matrix._31,
                                                             m_Matrix._32);
    } else {
        mGraph.SetPosition(m_Matrix._30, m_Matrix._31, m_Matrix._32);
    }
    return true;
    unguard;
}

bool GcBaseActor::SetPosition(int vX, int vY) {
    float x, y;
    g_pScene->GetPosByTerrainXY(vX, vY, x, y);
    if (SetPosition(x, y)) {
        return true;
    }
    return false;
}

void GcBaseActor::SetMoveSpeed(float fSpeed) {
    m_fMoveSpeed = fSpeed;
}

void GcBaseActor::UpdateDirection() {
    mGraph.SetRotation(m_Matrix);
}

bool GcBaseActor::HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, float& fOutMinDist,
                          bool bHitTestTouch) {
    guard;
    if (m_CurrentState == READY_BORN_STATE || m_CurrentState == BORN_STATE) {
        // LOG("该物体正在出生中或者还未出生的状态\n");
        return false;
    }

    if (bHitTestTouch)  // 检查该物体是否能被选中
    {
        if (m_pMaster) {
            if (!GetWorld()->CanTouch(m_pMaster)) {
                return false;
            }
        }
    }

    RtgVertex3 v0, v1, vMin, vMax;
    float      r1, r2;

    v0 = vOrg;
    v1 = v0 + vDir * 1000.f;  // 只判断1000距离远的物体

    bool        bGetBound  = false;
    RtgMatrix16 mInvMat    = m_Matrix;
    const float fMinSelect = g_fSceneTerrainTileWidth * 0.5f;
    const float fMaxSelect = g_fSceneTerrainTileWidth * 0.75f;
    if (mGraph.p()) {
        RtgAABB* pAABB = mGraph.p()->GetStaticBoundingBox();
        // RtgAABB* pAABB = mGraph.p()->GetBoundingBox();
        if (mGraph.p()->m_Name == "mon062.act") {
            RtgAABB* pAABB1 = mGraph.p()->GetBoundingBox();
            pAABB           = pAABB1;
        }
        if (pAABB) {
            vMax = pAABB->vExt;
            if (mpModel) {
                vMax.x = mpModel->ModelRadius / 2.f;
                vMax.y = mpModel->ModelRadius / 2.f;
            } else {
                if (vMax.x < fMinSelect)
                    vMax.x = fMinSelect;
                else if (vMax.x > fMaxSelect)
                    vMax.x = fMaxSelect;
                if (vMax.y < fMinSelect)
                    vMax.y = fMinSelect;
                else if (vMax.y > fMaxSelect)
                    vMax.y = fMaxSelect;
            }
            // mInvMat._30 += pAABB->vPos.x;
            // mInvMat._31 += pAABB->vPos.y;
            // mInvMat._32 += pAABB->vPos.z;
            mInvMat.Invert();
            vMin.x = -vMax.x;
            vMin.y = -vMax.y;
            vMax.z = pAABB->vPos.z + pAABB->vExt.z;
            vMin.z = 0;  // pAABB->vPos.z-pAABB->vExt.z;
            // if (vMax.z<20.f) vMax.z = 20.f;
            // else if (vMax.z>50.f) vMax.z=50.f;
            v0        = v0 * mInvMat;
            v1        = v1 * mInvMat;
            bGetBound = true;
        }
    }
    if (!bGetBound) {
        float fX, fY;
        int   iTx, iTy;
        g_pScene->GetTerrainByPosFast(m_Matrix._30, m_Matrix._31, iTx, iTy);
        g_pScene->GetPosByTerrainXY(iTx, iTy, fX, fY);
        mInvMat._30 = fX;
        mInvMat._31 = fY;
        mInvMat._32 += 20.f;
        mInvMat.Invert();
        if (mpModel) {
            vMax.x = mpModel->ModelRadius;
            vMax.y = mpModel->ModelRadius;
        } else {
            vMax.x = fMinSelect;
            vMax.y = fMinSelect;
        }
        vMax.z = 20.f;
        vMin   = vMax;
        vMin.x = -vMin.x;
        vMin.y = -vMin.y;
        vMin.z = -vMin.z;
        v0     = v0 * mInvMat;
        v1     = v1 * mInvMat;
    }

    if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2)) {
        if (r1 >= 0.f) {
            fOutMinDist = r1;
        } else {
            fOutMinDist = 0.f;
        }
        return true;
    }

    /*
  RtgVertex3 v0, v1, vMin, vMax;
  float r1, r2;
  float fX, fY;
  int iTx, iTy;

  g_pScene->GetTerrainByPosFast(m_Matrix._30, m_Matrix._31, iTx, iTy);
  g_pScene->GetPosByTerrainXY(iTx, iTy, fX, fY);

  vMin.x = fX - g_fSceneTerrainTileWidth*0.5f;
  vMin.y = fY - g_fSceneTerrainTileWidth*0.5f;
  vMin.z = m_Matrix._32;
  vMax.x = vMin.x + g_fSceneTerrainTileWidth;
  vMax.y = vMin.y + g_fSceneTerrainTileWidth;
  vMax.z = vMin.z + 40.f;

  v0 = vOrg;
  v1 = v0 + vDir * 1000.f; // 只判断1000距离远的物体

  if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2))
  {
      if (r1>=0.f)
      {
          fOutMinDist = r1;
      }else
      {
          fOutMinDist = 0.f;
      }
      return true;
  }
  */
    return false;
    unguard;
}

void GcBaseActor::Run(float fSecond) {
    for (list<CRT_ActorInstance*>::iterator it = m_eftList.begin(); it != m_eftList.end();) {
        CRT_ActorInstance* ef = *it;

        if (!ef) {
            it = m_eftList.erase(it);
            continue;
        }

        if (!ef->m_parent)
            ef->Tick(fSecond * 1000.f);

        if (ef->ResourceReady() && !ef->IsPlayingPose()) {
            RtcGetActorManager()->ReleaseActor(ef);
            it = m_eftList.erase(it);
            continue;
        }

        ++it;
    }

    mGraph.FrameMove(fSecond * 1000);

    if (m_pTrump)
        m_pTrump->Tick(fSecond * 1000);

    if (m_pWeapon)
        m_pWeapon->Tick(fSecond * 1000);

    if (m_pStateOverHead)
        m_pStateOverHead->Run(fSecond);

    if (m_pActorOverHead)
        m_pActorOverHead->Run(fSecond);

    return;
}

void GcBaseActor::Render(RtgDevice& inDevice, RTGRenderMask eMask) {
    guard;
    if (!m_bRender)
        return;
    RtgVertex3 vColor(10, 10, 10);
    bool       bDrawFrame = false;

    if (GetWorld()) {
        if (bDrawFrame) {
            g_pScene->GetTerrainColor(m_Matrix._30, m_Matrix._31, vColor);
            if (vColor.x < 0.3)
                vColor.x = 0.3;
            if (vColor.y < 0.3)
                vColor.y = 0.3;
            if (vColor.z < 0.3)
                vColor.z = 0.3;
            mGraph.SetBaseColor(vColor);
        } else {
            if (m_pMaster->ID() == GetWorld()->m_selectedActor) {
                // if(eMask == RTGRM_BLENDOBJ_ONLY)
                //	mGraph.RenderSelect(inDevice);
                mGraph.SetBaseColor(vColor);
            } else {
                g_pScene->GetTerrainColor(m_Matrix._30, m_Matrix._31, vColor);
                if (vColor.x < 0.3)
                    vColor.x = 0.3;
                if (vColor.y < 0.3)
                    vColor.y = 0.3;
                if (vColor.z < 0.3)
                    vColor.z = 0.3;
                mGraph.SetBaseColor(vColor);
            }
        }
    }

    mGraph.Render(inDevice, eMask, true);

    if (GetWorld()) {
        if (bDrawFrame) {
            if (m_pMaster->ID() == GetWorld()->m_selectedActor) {
                if (eMask == RTGRM_NOBLENDOBJ_ONLY)
                    mGraph.p()->RenderFrame(&inDevice, RtgVertex3(1, 0.5, 0),
                                            RtgVertex3(1.03, 1.03, 1.01));
            }
        }
    }

#ifndef PUBLISH
    if (m_bDrawBoundBox && g_pScene) {
        RtgVertex3  vMin, vMax;
        int         iTx, iTy;
        DWORD       dwColor;
        RtgAABB*    pAABB;
        RtgMatrix16 m16;
        RtgShader   nullShader;
        nullShader.iNumStage = 0;
        DWORD dwLighting     = GetDevice()->GetRenderState(RTGRS_LIGHTING);
        GetDevice()->SetRenderState(RTGRS_LIGHTING, FALSE);
        GetDevice()->SetShader(&nullShader);
        m16.Unit();
        GetDevice()->SetMatrix(RTGTS_WORLD, &m16);
        if (g_pScene->GetTerrainByPos(m_Matrix._30, m_Matrix._31, iTx, iTy)) {
            bool        bGetBound  = false;
            RtgMatrix16 mInvMat    = m_Matrix;
            const float fMinSelect = g_fSceneTerrainTileWidth * 0.5f;
            const float fMaxSelect = g_fSceneTerrainTileWidth * 0.75f;
            if (mGraph.p()) {
                RtgAABB* pAABB = mGraph.p()->GetStaticBoundingBox();
                if (pAABB) {
                    vMax = pAABB->vExt;
                    if (mpModel) {
                        vMax.x = mpModel->ModelRadius / 2.f;
                        vMax.y = mpModel->ModelRadius / 2.f;
                    } else {
                        if (vMax.x < fMinSelect)
                            vMax.x = fMinSelect;
                        else if (vMax.x > fMaxSelect)
                            vMax.x = fMaxSelect;
                        if (vMax.y < fMinSelect)
                            vMax.y = fMinSelect;
                        else if (vMax.y > fMaxSelect)
                            vMax.y = fMaxSelect;
                    }
                    // mInvMat._30 += pAABB->vPos.x;
                    // mInvMat._31 += pAABB->vPos.y;
                    // mInvMat._32 += pAABB->vPos.z;
                    vMin.x = -vMax.x;
                    vMin.y = -vMax.y;
                    vMax.z = pAABB->vPos.z + pAABB->vExt.z;
                    vMin.z = 0;  // pAABB->vPos.z-pAABB->vExt.z;
                    if (vMax.z < 20.f)
                        vMax.z = 20.f;
                    else if (vMax.z > 50.f)
                        vMax.z = 50.f;
                    bGetBound = true;
                }
            }
            if (!bGetBound) {
                float fX, fY;
                int   iTx, iTy;
                g_pScene->GetTerrainByPosFast(m_Matrix._30, m_Matrix._31, iTx, iTy);
                g_pScene->GetPosByTerrainXY(iTx, iTy, fX, fY);
                mInvMat._30 = fX;
                mInvMat._31 = fY;
                mInvMat._32 += 20.f;
                vMax.x = fMinSelect;
                vMax.y = fMinSelect;
                vMax.z = 20.f;
                vMin   = vMax;
                vMin.x = -vMin.x;
                vMin.y = -vMin.y;
                vMin.z = -vMin.z;
            }
            dwColor = 0xFFFF0000;
            GetDevice()->DrawBox(vMax, mInvMat, &dwColor);
        }
        pAABB = mGraph.GetBoundingBox();
        if (pAABB) {
            m16 = m_Matrix;
            // m16._30 += pAABB->vPos.x;
            // m16._31 += pAABB->vPos.y;
            m16._32 += pAABB->vPos.z;
            dwColor = 0xFF0000FF;
            GetDevice()->DrawBox(pAABB->vExt, m16, &dwColor);
            if (m_pMaster->NpcType() == GcActor::ENT_MONSTER) {
                // static char szOutputString[1024];
                S_MEMDEF(szOutputString, 1024)
                S_MEMPROTECTOR(szOutputString, 1024, bDefined)

                rt2_sprintf(szOutputString,
                            "------------------------\n"
                            "包围盒位置X:     %2.3f(ms)\n"
                            "包围盒位置Y:     %2.3f(ms)\n"
                            "包围盒位置Z:     %2.3f(ms)\n"
                            "包围盒外框X:     %2.3f(ms)\n"
                            "包围盒外框Y:     %2.3f(ms)\n"
                            "包围盒外框Z:     %2.3f(ms)\n",
                            pAABB->vPos.x, pAABB->vPos.y, pAABB->vPos.z, pAABB->vExt.x,
                            pAABB->vExt.y, pAABB->vExt.z);
                GetDevice()->DrawString(5, 155, 0xFF00FF00, szOutputString);
            }
        }
        pAABB = mGraph.p()->GetStaticBoundingBox();
        if (pAABB) {
            m16 = m_Matrix;
            // m16._30 += pAABB->vPos.x;
            // m16._31 += pAABB->vPos.y;
            m16._32 += pAABB->vPos.z;
            dwColor = 0xFF00FF00;
            GetDevice()->DrawBox(pAABB->vExt, m16, &dwColor);
        }
        GetDevice()->SetRenderState(RTGRS_LIGHTING, dwLighting);
    }
#endif

    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end();) {
        if (!(*it)) {
            it = m_eftList.erase(it);
        } else {
            (*it)->Render(&inDevice, eMask);
            it++;
        }
    }
    if (m_pItemEffect) {
        m_pItemEffect->Render(&inDevice, eMask);
    }
    if (m_pTrump) {
        m_pTrump->Render(&inDevice, eMask);
    }
    if (m_pWeapon) {
        m_pWeapon->Render(&inDevice, eMask);
    }

    // Render path find  //tim.yang   根据配置文件变量来绘制路线
    if (m_isDrawPathLine && m_pMaster == GetPlayer() && m_pPathNode) {
        int iPathCnt = m_pPathNode->PathCount();
        // if (iPathCnt)
        {
            RtgVertex3 vPos, vT1(0.f, -10.f, 5.f), vT2(7.f, 3.f, 5.f), vT3(-7.f, 3.f, 5.f), v1, v2,
                v3, v4, v5;
            RtgVertex4         vColor(1.f, 0.f, 0.f, 0.f);
            RtgMatrix16        m16;
            RtsPathPath::PPos* pPos;
            DWORD              dwColor = 0xFF00FF00;
            RtgShader          nullShader;
            nullShader.iNumStage = 0;
            m16.Unit();
            inDevice.SetMatrix(RTGTS_WORLD, &m16);
            GetDevice()->SetDefaultMaterial();
            GetDevice()->SetShader(&nullShader);
            DWORD dwLighting = GetDevice()->GetRenderState(RTGRS_LIGHTING);
            GetDevice()->SetRenderState(RTGRS_LIGHTING, TRUE);
            RtgVertex3 vPoint(0, 0, 0);
            v4 = vPoint * m_drawLineStartMatrix;
            v1 = vT1 * m_drawLineStartMatrix;
            v2 = vT2 * m_drawLineStartMatrix;
            v3 = vT3 * m_drawLineStartMatrix;
            GetDevice()->DrawTriangle(v1, v2, v3, &dwColor);
            dwColor = 0xFFFF0000;
            for (int i = 0;; i++) {
                pPos = m_pPathNode->GetStep(i);
                if (0 == pPos) {
                    v5 = vPoint * m_Matrix;
                    GetDevice()->DrawLine(v4, v5, &dwColor);
                    break;
                }
                g_pScene->GetPosByTerrain(pPos->MinX, pPos->MinY, vPos);
                m16._30 = vPos.x;
                m16._31 = vPos.y;
                m16._32 = vPos.z;
                v1      = vT1 * m16;
                v2      = vT2 * m16;
                v3      = vT3 * m16;
                v5      = vPoint * m16;
                GetDevice()->DrawTriangle(v1, v2, v3, &dwColor);
                GetDevice()->DrawLine(v4, v5, &dwColor);
                v4 = vPoint * m16;
            }
            v1 = vT1 * m_Matrix;
            v2 = vT2 * m_Matrix;
            v3 = vT3 * m_Matrix;
            GetDevice()->DrawTriangle(v1, v2, v3, &dwColor);
            GetDevice()->RestoreShader(&nullShader);
            GetDevice()->SetRenderState(RTGRS_LIGHTING, dwLighting);
        }
    }
    // end

    unguard;
}

void GcBaseActor::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                  RtgVertex3& scale) {
    guard;
    if (!m_bRender)
        return;
    RtgVertex3 vColor(10.f, 10.f, 10.f);
    bool       bDrawFrame = false;
    if (GetWorld()) {
        if (bDrawFrame) {
            g_pScene->GetTerrainColor(m_Matrix._30, m_Matrix._31, vColor);
            if (vColor.x < 0.3)
                vColor.x = 0.3;
            if (vColor.y < 0.3)
                vColor.y = 0.3;
            if (vColor.z < 0.3)
                vColor.z = 0.3;
            mGraph.SetBaseColor(vColor);
        } else {
            if (m_pMaster->ID() == GetWorld()->m_selectedActor) {
                mGraph.SetBaseColor(vColor);
            } else {
                g_pScene->GetTerrainColor(m_Matrix._30, m_Matrix._31, vColor);
                if (vColor.x < 0.3)
                    vColor.x = 0.3;
                if (vColor.y < 0.3)
                    vColor.y = 0.3;
                if (vColor.z < 0.3)
                    vColor.z = 0.3;
                mGraph.SetBaseColor(vColor);
            }
        }
    }
    mGraph.RenderHighLight(inCamera, inDevice, color, scale);
    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end(); it++) {
        (*it)->RenderHighLight(&inDevice, color, scale);
    }
    if (m_pItemEffect) {
        m_pItemEffect->RenderHighLight(&inDevice, color, scale);
    }
    if (m_pTrump) {
        m_pTrump->RenderHighLight(&inDevice, color, scale);
    }
    if (m_pWeapon) {
        m_pWeapon->RenderHighLight(&inDevice, color, scale);
    }
    unguard;
}

bool GcBaseActor::MoveToTerrainNow(int iTileX, int iTileY, bool bFindSpace) {
    guard;
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        m_pMaster->m_pRidePet->mBaseActor.UpdateDirection();
    } else {
        UpdateDirection();
    }

    if (m_pMaster) {
        float fX, fY;
        g_pScene->GetPosByTerrainXY(iTileX, iTileY, fX, fY);
        m_pMaster->OnPositionChanged(fX, fY);
        // g_pScene->OnMoveToTerrain(iTileX,iTileY);
    }
    if (bFindSpace) {
        if (AddSelfFind(iTileX, iTileY)) {
            return true;
        }
    } else {
        if (AddSelf(iTileX, iTileY)) {
            return true;
        }
    }
    ERR2("GcBaseActor::MoveToTerrainNow 移动角色到(%d,%d)失败\n", iTileX, iTileY);
    return false;
    unguard;
}

bool GcBaseActor::MoveToTerrainNow(float fX, float fY, bool bFindSpace) {
    guard;
    int iTileX, iTileY;
    g_pScene->GetTerrainByPosFast(fX, fY, iTileX, iTileY);
    return MoveToTerrainNow(iTileX, iTileY, bFindSpace);
    unguard;
}

// 返回true表示还可以走，false表示已经走到了，不能再走了
// fNeedTime 表示走到目的点需要的时间，如果小于0，就使用玩家的速度
bool GcBaseActor::MoveToPosition(float fX, float fY, float fDistance, float fNeedTime,
                                 float fDiffTime, bool bTarget) {
    guard;
    // 如果路径节点为空，调用添加自我查找的方法并返回
    if (m_pPathNode == NULL) {
        return AddSelfFind(fX, fY);
    }

    m_pPathFind = GetWorld()->GetPathFind();  // 获取路径查找器
    if (!m_pPathFind) {
        return false;  // 如果没有路径查找器，返回 false
    }
    //  如果是玩家，角色类型为1
    int actortype = 0;
    if (GcActor::ENT_USER == m_pMaster->NpcType()) {
        actortype = 1;
    }

    int iTileX, iTileY;                                               // 当前瓦片坐标
    int iFinalTileX, iFinalTileY;                                     // 目标瓦片坐标
    g_pScene->GetTerrainByPosFast(fX, fY, iFinalTileX, iFinalTileY);  // 获取目标位置的瓦片坐标
        // 计算目标点与当前位置的距离
    float fD =
        sqrt((fX - m_Matrix._30) * (fX - m_Matrix._30) + (fY - m_Matrix._31) * (fY - m_Matrix._31));
    if (fD <= fDistance) {
        // 如果距离小于等于允许误差距离，认为已经到达目标点
        m_bMoveToPositionNewMove = TRUE;
        return false;
    } else if (1 == actortype && fD > 10000.f && fNeedTime >= 0.f) {
        P_LOGINFO("距离过远:" + std::to_string(fD));
        SetPosition(fX, fY);
        m_bMoveToPositionNewMove = TRUE;
        return false;
    } else if (1 != actortype && fD > 500.f && fNeedTime >= 0.f) {
        P_LOGINFO("距离过远:" + std::to_string(fD));
        SetPosition(fX, fY);
        m_bMoveToPositionNewMove = TRUE;
        return false;
    }

    bool IgonreActor = /* m_pMaster == GetPlayer() ? false : */ false;

    // 如果目标位置或时间与之前的记录不同，或是新的移动请求
    if (fX != m_fMoveToPositionOldX || fY != m_fMoveToPositionOldY ||
        m_fMoveToPositionTime != fNeedTime || m_bMoveToPositionNewMove) {
        // if(actortype == 1)
        //	RtCoreLog().Info("moveto %f,%f -- old %f,%f, needtime %f,%f, bNewmove
        //%d\n", fX, fY, m_fMoveToPositionOldX, m_fMoveToPositionOldY,
        // m_fMoveToPositionTime, fNeedTime, m_bMoveToPositionNewMove);
        //
        // 记录新的目标位置和时间
        m_fMoveToPositionOldX    = fX;
        m_fMoveToPositionOldY    = fY;
        m_fMoveToPositionTime    = fNeedTime;
        m_bMoveToPositionNewMove = false;
        // 获取当前位置的瓦片坐标
        g_pScene->GetTerrainByPosFast(fX, fY, iTileX, iTileY);
        // 查找路径，如果找到了路径
        if (FindPath(iTileX, iTileY, IgonreActor, bTarget))  // 目的地的路径已经找到
        {
            if (fNeedTime <= 0.f) {  // 如果需要时间小于等于 0，设定速度为默认速度或宠物速度
                if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
                    // m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED;
                    m_fMoveToPositionSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue();
                } else {
                    // m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED;
                    m_fMoveToPositionSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue();
                }
            } else {
                // 计算总距离，然后计算出平均速度
                m_fMoveToPositionSpeed = fD / fNeedTime;
                if (m_fMoveToPositionSpeed > DEFAULT_WALK_SPEED * 2.f) {
                    m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED * 2.f;
                    LOG("GcBaseActor::MoveToPosition "
                        "移动速度太快，被调整成普通速度的2倍.\n");
                } else if (m_fMoveToPositionSpeed < 0.f) {
                    m_fMoveToPositionSpeed = 0.f;
                }
            }
            m_fLastDistance = 1000000.f;  // 初始化最后距离为很大值
            if (m_pPathNode->Next(iTileX, iTileY, m_pPathFind, IgonreActor, bTarget,
                                  actortype))  // 如果路径还有下一段，更新目标位置
            {
                if (iTileX == iFinalTileX && iTileY == iFinalTileY) {
                    m_vMoveToPosition.x = fX;
                    m_vMoveToPosition.y = fY;
                } else {
                    g_pScene->GetPosByTerrain(iTileX, iTileY, m_vMoveToPosition);
                }
            } else {
                // 如果没有下一段，直接设定目标位置为最终位置
                m_vMoveToPosition.x = fX;
                m_vMoveToPosition.y = fY;
            }
            // 计算从当前位置到目标点的距离
            fD = sqrt((m_vMoveToPosition.x - m_Matrix._30) * (m_vMoveToPosition.x - m_Matrix._30) +
                      (m_vMoveToPosition.y - m_Matrix._31) * (m_vMoveToPosition.y - m_Matrix._31));
            // 计算X和Y方向上的速度分量
            m_fMoveToPositionSpeedX =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.x - m_Matrix._30) / fD;
            m_fMoveToPositionSpeedY =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.y - m_Matrix._31) / fD;
            FaceTo(m_vMoveToPosition.x, m_vMoveToPosition.y);
        } else {
            m_bMoveToPositionNewMove = TRUE;
            return false;  // 没找到目的地，可能是被阻挡，走不过去
        }
    }

    float fDX, fDY;

    // 计算离最近一个目标点的距离
    fD = sqrt((m_vMoveToPosition.x - m_Matrix._30) * (m_vMoveToPosition.x - m_Matrix._30) +
              (m_vMoveToPosition.y - m_Matrix._31) * (m_vMoveToPosition.y - m_Matrix._31));

    // 如果是越走越近就对了，如果越走越远，就停止走路
    if (fD < m_fLastDistance) {
        m_fLastDistance = fD;
    } else {
        if (1 != actortype)  // tim.yang  automove  玩家寻路不做此判断
        {
            // 如果越走越远，就停止走路
            LOG2("越走越远了 %.2f , %.2f\n", m_fLastDistance, fD);
            m_bMoveToPositionNewMove = TRUE;
            return false;  // 路径已经结束
        } else {
            // RtCoreLog().Info("越走越远了 %.2f , %.2f\n", m_fLastDistance, fD);
            m_fMoveToPositionSpeedX =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.x - m_Matrix._30) / fD;
            m_fMoveToPositionSpeedY =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.y - m_Matrix._31) / fD;
            FaceTo(m_vMoveToPosition.x, m_vMoveToPosition.y);
        }
    }

    // 计算下一个走路的位置
    if (fD > (fDiffTime * m_fMoveToPositionSpeed)) {
        fDX = m_Matrix._30 + fDiffTime * m_fMoveToPositionSpeedX;
        fDY = m_Matrix._31 + fDiffTime * m_fMoveToPositionSpeedY;
    } else {
        fDX = m_vMoveToPosition.x;
        fDY = m_vMoveToPosition.y;
        if (m_pPathNode->Next(iTileX, iTileY, m_pPathFind, IgonreActor, bTarget,
                              actortype))  // 还有下一段
        {

            // RtCoreLog().Info("nextnode %d,%d\n", iTileX, iTileY);

            m_fLastDistance = 1000000.f;
            if (iTileX == iFinalTileX && iTileY == iFinalTileY) {
                m_vMoveToPosition.x = fX;
                m_vMoveToPosition.y = fY;
            } else {
                g_pScene->GetPosByTerrain(iTileX, iTileY, m_vMoveToPosition);
            }
            fD = sqrt((m_vMoveToPosition.x - m_Matrix._30) * (m_vMoveToPosition.x - m_Matrix._30) +
                      (m_vMoveToPosition.y - m_Matrix._31) * (m_vMoveToPosition.y - m_Matrix._31));
            m_fMoveToPositionSpeedX =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.x - m_Matrix._30) / fD;
            m_fMoveToPositionSpeedY =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.y - m_Matrix._31) / fD;
            FaceTo(m_vMoveToPosition.x, m_vMoveToPosition.y);
        } else {
            m_bMoveToPositionNewMove = TRUE;
            return false;  // 路径已经结束
        }
    }

    if (m_fLastMoveSpeed != m_fMoveToPositionSpeed && m_pMaster->m_pCurCmd) {
        m_fLastMoveSpeed = m_fMoveToPositionSpeed;
        float fSpeed     = 1.f;
        if (m_pMaster->m_pModel) {
            if (m_pMaster == GetPlayer()) {
                if (m_pMaster->GetRideState() == GcActor::ERS_ACTOR) {
                    fSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue() /
                             GetWorld()->m_pPet->m_pModel->MoveSpeed;
                } else {
                    fSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue() /
                             GetWorld()->m_pPlayer->m_pModel->MoveSpeed;
                }
            } else {
                if ((fD - m_pMaster->m_pCurCmd->fDistance) / m_pMaster->m_pCurCmd->fTime >
                    80.f * 2.f) {
                    fSpeed = (80.f * 2.f) / m_pMaster->m_pModel->MoveSpeed;
                } else {
                    fSpeed =
                        ((fD - m_pMaster->m_pCurCmd->fDistance) / m_pMaster->m_pCurCmd->fTime) /
                        m_pMaster->m_pModel->MoveSpeed;
                }
            }
        }
        if (m_pMaster->GetRideState() == GcActor::ERS_ACTOR ||
            m_pMaster->NpcType() == GcActor::ENT_PET)
            PlayPose(GcBaseActor::POSE_RUN, true, NULL, fSpeed);
        else if (m_pMaster->NpcType() == GcActor::ENT_CALLNPC)
            PlayPose(GcBaseActor::POSE_RUN, true, NULL, fSpeed);
        else
            PlayPose(GcBaseActor::POSE_WALK, true, NULL, fSpeed);
    }

    // 设置位置
    SetPosition(fDX, fDY);
    return true;
    unguard;
}

bool GcBaseActor::FindPath(int vX, int vY, bool vIgnoreActor, bool vIgnoreTarget) {
    guard;
    if (m_pPathNode == NULL)
        return false;
    m_pPathFind = GetWorld()->GetPathFind();
    if (!m_pPathFind) {
        return false;
    }
    // tim.yang
    int actorType         = 0;
    m_drawLineStartMatrix = m_Matrix;  // tim.yang  记录角色移动前的位置
    if (GcActor::ENT_USER == m_pMaster->NpcType()) {
        actorType = 1;
    }

    if (m_pPathFind->FindPathEx(m_pPathNode, vX, vY, vIgnoreActor, vIgnoreTarget, actorType)) {
        m_pPathFind->SmoothPath(m_pPathNode, vIgnoreActor);
        return true;
    }

    return false;
    // if (GcActor::ENT_USER == m_pMaster->NpcType())
    //{
    //	actorType = 1;//player  如果是玩家
    //	m_pPathFind->InitAStar();  //初始化参数
    //	m_pPathNode->ClearPathEx(); //每次寻路前清除路点链表
    //	int startX = m_pPathNode->curPos.MinX;
    //	int startY = m_pPathNode->curPos.MinY;
    //	if (m_pPathFind->Distance2(startX,startY,vX,vY))
    //	{
    //		if(m_pPathFind->FindPathEx(m_pPathNode,vX,vY,vIgnoreActor,vIgnoreTarget,actorType))
    //		{
    //			m_pPathFind->SmoothPath(m_pPathNode,vIgnoreActor);
    //			return true;
    //		}
    //		return false;
    //	}
    //	else
    //	{
    //		if
    //(m_pPathFind->FindPathFromWP(m_pPathNode,startX,startY,vX,vY,vIgnoreActor,vIgnoreTarget,actorType))
    //		{
    //			m_pPathFind->SmoothPath(m_pPathNode,vIgnoreActor);
    //			return true;
    //		}
    //		return false;
    //	}
    // }
    // else
    //{
    //	if(m_pPathFind->FindPathEx(m_pPathNode,vX,vY,vIgnoreActor,vIgnoreTarget,actorType))
    //	{
    //		m_pPathFind->SmoothPath(m_pPathNode,vIgnoreActor);
    //		return true;
    //	}
    //	return false;
    // }
    // return false;
    unguard;
}

bool GcBaseActor::AddSelfFind(int vX, int vY) {
    guard;
    if (m_bPutOnPathFinder) {
        if (!RemoveSelf()) {
            return false;
        }
    }

    CHECK(m_bPutOnPathFinder == 0);
    //	 if (GetWorld()->GetPathFind()->FindSpace(vX, vY, vX, vY))// 注释
    //[3/16/2009 tooth.shi]
    int actorType = 1;
    if (m_pMaster->NpcType() == GcActor::ENT_USER) {
        m_blockType = PB_UNBLOCK;
    } else if (m_pMaster->NpcType() == GcActor::ENT_MONSTER ||
               m_pMaster->NpcType() == GcActor::ENT_NPC_COMBATIVE) {
        m_blockType = PB_UNBLOCK;
    } else if (m_pMaster->NpcType() == GcActor::ENT_NPC_SERVICE) {
        if (m_pMaster->m_pNpc &&
            (m_pMaster->m_pNpc->Type == 8 || m_pMaster->m_pNpc->AIType == "s_guarder")) {
            m_blockType = PB_UNBLOCK;
        } else
            m_blockType = PB_BLOCK;
    } else if (m_pMaster->NpcType() == GcActor::ENT_PET) {
        m_blockType = PB_UNBLOCK;
    }
    if (m_pMaster->NpcType() == GcActor::ENT_CALLNPC) {
        m_blockType = PB_UNBLOCK;
    }
    if (GetWorld()->GetPathFind()->CanAdd(this->m_pMaster->ID(), actorType, vX, vY, m_blockType)) {
        //        if
        //        (m_pPathNode=(RtsPathPath*)GetWorld()->GetPathFind()->AddActor(this,vX,vY,true,true))//
        //        注释 [3/16/2009 tooth.shi]
        if (m_pPathNode = (RtsPathPath*)GetWorld()->GetPathFind()->AddActor(
                this->m_pMaster->ID(), actorType, vX, vY, vX, vY, true, m_blockType)) {
            m_bPutOnPathFinder++;
            m_LastTileX = vX;
            m_LastTileY = vY;
            SetPosition(vX, vY);
            return true;
        }
    }
    ERR2("GcBaseActor::AddSelf (ID=%d, Name=%s)加入场景出错\n", m_pMaster ? m_pMaster->ID() : 0,
         m_pMaster ? m_pMaster->GetName() : "Null");
    return false;
    unguard;
}

bool GcBaseActor::AddSelf(int vX, int vY) {
    guard;
    if (m_bPutOnPathFinder) {
        if (!RemoveSelf()) {
            return false;
        }
    }

    CHECK(m_bPutOnPathFinder == 0);
    int actorType = 1;
    if (m_pMaster->NpcType() == GcActor::ENT_USER) {
        m_blockType = PB_UNBLOCK;
    } else if (m_pMaster->NpcType() == GcActor::ENT_MONSTER ||
               m_pMaster->NpcType() == GcActor::ENT_NPC_COMBATIVE) {
        m_blockType = PB_NONE;
    } else if (m_pMaster->NpcType() == GcActor::ENT_NPC_SERVICE) {
        if (m_pMaster->m_pNpc &&
            (m_pMaster->m_pNpc->Type == 8 || m_pMaster->m_pNpc->AIType == "s_guarder")) {
            m_blockType = PB_UNBLOCK;
        } else
            m_blockType = PB_BLOCK;
    } else if (m_pMaster->NpcType() == GcActor::ENT_PET) {
        m_blockType = PB_UNBLOCK;
    }
    if (m_pMaster->NpcType() == GcActor::ENT_CALLNPC) {
        m_blockType = PB_UNBLOCK;
    }
    //	m_pPathNode=(RtsPathPath*)GetWorld()->GetPathFind()->AddActor(this,vX,vY,true,true);//
    // 注释 [3/16/2009 tooth.shi]
    m_pPathNode = (RtsPathPath*)GetWorld()->GetPathFind()->AddActor(
        this->m_pMaster->ID(), actorType, vX, vY, vX, vY, true, m_blockType);
    if (m_pPathNode) {
        m_bPutOnPathFinder++;
        m_LastTileX = vX;
        m_LastTileY = vY;

        SetPosition(vX, vY);
        return true;
    } else {
        ERR("GcBaseActor::AddSelf 加入场景出错\n");
    }
    return false;
    unguard;
}

bool GcBaseActor::RemoveSelf() {
    guard;
    if (m_bPutOnPathFinder == 0)
        return false;
    this->m_pMaster->ClearCommand();
    if (GetWorld()->GetPathFind()->RemoveActor(m_pPathNode)) {
    } else {
        ERR("GcBaseActor::RemoveSelf 从场景移除出错\n");
    }
    if (m_bPutOnPathFinder > 0) {
        m_bPutOnPathFinder--;
    }
    return true;
    unguard;
}

bool GcBaseActor::AddSelf(float vX, float vY) {
    guard;
    int X, Y;
    Point2Tile(vX, vY, X, Y);
    return AddSelf(X, Y);
    unguard;
}

bool GcBaseActor::AddSelfFind(float vX, float vY) {
    guard;
    int X, Y;
    Point2Tile(vX, vY, X, Y);
    return AddSelfFind(X, Y);
    unguard;
}

void GcBaseActor::Point2Tile(float vX, float vY, int& vrTileX, int& vrTileY) {
    g_pScene->GetTerrainByPos(vX, vY, vrTileX, vrTileY);
}

void GcBaseActor::Tile2Point(int vTileX, int vTileY, float& vrX, float& vrY, float& vrZ) {
    RtgVertex3 v3;
    g_pScene->GetPosByTerrain(vTileX, vTileY, v3);
    vrX = v3.x;
    vrY = v3.y;
    vrZ = v3.z;
}

bool GcBaseActor::IsPosition(float vX, float vY) {
    if (m_Matrix._30 == vX && m_Matrix._31 == vY) {
        return true;
    }
    return false;
}

bool GcBaseActor::IsPosition(int vX, int vY) {
    int X, Y;
    Point2Tile(m_Matrix._30, m_Matrix._31, X, Y);

    if (vX == X && vY == Y) {
        return true;
    }
    return false;
}

// const int STRING_LENGTH=1024;

bool GcBaseActor::PlayPose(char vpPoseName[], bool vLoop) {
    guard;

    if (vpPoseName[0] == '\0')
        return false;

    bool bResult;
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        bResult = m_pMaster->m_pRidePet->mBaseActor.mGraph.PlayPose(vpPoseName, vLoop);
        rt2_strncpy(m_pMaster->m_pRidePet->mBaseActor.m_szLastPoseName, vpPoseName, 40);
    } else {
        bResult = mGraph.PlayPose(vpPoseName, vLoop);
        rt2_strncpy(m_szLastPoseName, vpPoseName, 40);
    }
    // if (!bResult)
    //{
    //	ERR1("GcBaseActor::PlayPose 播放动作失败[%s]\n",vpPoseName);
    // }
    return bResult;

    unguard;
}

bool GcBaseActor::IsPlayer() {
    return (&(GetWorld()->m_pPlayer->mBaseActor) == this);
}

// 弓
static const char* BowPoseName[] = {
    "wait_bow",    // 0 站立
    "attack_bow",  // 1 攻击
    "walk_bow",    // 2 移动
    "die",         // 3 死亡
    "hurt_bow",    // 4 受击
    "skill",       // 5 技能
    "waiting_bow"  // 6 耍酷
};
// 弩
static const char* CrossBowPoseName[] = {
    "wait_crossbow",    // 0 站立
    "attack_crossbow",  // 1 攻击
    "walk_crossbow",    // 2 移动
    "die",              // 3 死亡
    "hurt_crossbow",    // 4 受击
    "skill",            // 5 技能
    "waiting_crossbow"  // 6 耍酷
};
// 斧头
static const char* AxePoseName[] = {
    "wait_t",            // 0 站立
    "attack_t",          // 1 攻击
    "walk_t",            // 2 移动
    "die",               // 3 死亡
    "hurt_t",            // 4 受击
    "skill",             // 5 技能
    "waiting_t",         // 6 耍酷
    "attack_t_critical"  // 7 暴击
};

// 刀双手
static const char* DaoDoublePoseName[] = {
    "wait_t_sin",             // 0 站立
    "attack_t_sin",           // 1 攻击
    "walk_t_sin",             // 2 移动
    "die",                    // 3 死亡
    "hurt_t_sin",             // 4 受击
    "skill",                  // 5 技能
    "waiting_t_sin",          // 6 耍酷
    "attack_t_sin_critical",  // 7 暴击
};

// 刀单手（无盾牌）
static const char* DaoPoseWithoutShield[] = {
    "wait_sin",             // 0 站立
    "attack_sin",           // 1 攻击
    "walk_sin_r",           // 2 移动
    "die",                  // 3 死亡
    "hurt_sin",             // 4 受击
    "skill",                // 5 技能
    "waiting_sin",          // 6 耍酷
    "attack_sin_critical",  // 7 暴击
};

// 刀单手（有盾牌）
static const char* DaoPoseWithShield[] = {
    "wait_sin",             // 0 站立
    "attack_sin",           // 1 攻击
    "walk_sin",             // 2 移动
    "die",                  // 3 死亡
    "hurt_sin",             // 4 受击
    "skill",                // 5 技能
    "waiting_sin",          // 6 耍酷
    "attack_sin_critical",  // 7 暴击
};

// 只有盾牌
static const char* PoseShieldOnly[] = {
    "wait_non",     // 0 站立
    "attack_non",   // 1 攻击
    "walk_shield",  // 2 移动
    "die",          // 3 死亡
    "hurt_non",     // 4 受击
    "skill",        // 5 技能
    "waiting_non",  // 6 耍酷
    "attack_non",   // 7 暴击
};

// 人物软鞭动作
static const char* WhipPoseName[] = {
    "wait_whip",             // 0 等待
    "attack_whip",           // 1 攻击
    "walk_whip",             // 2 移动
    "die",                   // 3 死亡
    "hurt_whip",             // 4 受击
    "skill",                 // 5 技能
    "waiting_whip",          // 6 耍酷
    "attack_whip_critical",  // 7 暴击
};

// 软鞭武器动作
static const char* WhipWeaponPoseName[] = {"wait", "attack",  "walk",
                                           "wait",  // 3 默认死亡动作
                                           "hurt", "skill11", "waiting", "attack_critical"};

// 人物法宝动作
static const char* FPoseName[] = {
    "wait_non",          // 0 站立
    "attack_non_magic",  // 1 攻击
    "walk_non",          // 2 移动
    "die",               // 3 死亡
    "hurt_non",          // 4 受击
    "skill",             // 5 技能
    "waiting_non"        // 6 耍酷
};

// 杖
static const char* WandPoseName[] = {
    "wait_wand",    // 0 持仗等待动作
    "attack_wand",  // 1 持仗攻击动作
    "walk_wand",    // 2 持仗移动动作
    "die",          // 3 默认死亡动作
    "hurt_wand",    // 4 持仗受击动作
    "skill",        // 5 默认技能动作
    "waiting_wand"  // 6 持仗耍酷动作
};

// 默认动作
static const char* DefaultPoseName[] = {
    "wait_non",          // 0 默认站立动作
    "attack_non_magic",  // 1 默认攻击动作
    "walk_non",          // 2 默认移动动作
    "die",               // 3 默认死亡动作
    "hurt_non",          // 4 默认受击动作
    "skill",             // 5 默认技能动作
    "waiting_non"        // 6
};
// 简单NPC默认动作
static const char* OldSimpleNpcDefaulePoseName[] = {
    "null_",    // 0 空
    "wait",     // 1 待机
    "wait",     // 2 警戒
    "wait",     // 3 插播待机		2种
    "talk",     // 4 对话
    "walk",     // 5 跑步
    "walk",     // 6 追击
    "attack",   // 7 攻击动作
    "attack",   // 8 持械攻击暴击
    "hurt",     // 9 受击动作
    "wait",     // 10 吟唱
    "wait",     // 11 炼制法宝
    "wait",     // 12 制作物品
    "wait",     // 13 打坐
    "wait",     // 14 挖掘
    "wait",     // 15 采药
    "wait",     // 16 扒皮
    "wait",     // 17 抽出武器
    "wait",     // 18 收回武器
    "die",      // 19 死亡动作
    "",         // 20 死亡尸体动作
    "wait",     // 21 复活动作
    "skill01",  // 22 默认技能动作
};
// 旧主角默认动作
static const char* OldUserDefaulePoseName[] = {
    "wait_non",     // 0无姿势，默认状态
    "wait_non",     // 1站立姿势
    "wait_non",     // 2战斗待机动作
    "waiting_non",  // 3空闲状态
    "wait_non",     // 4对话姿势
    "walk_non",     // 5行走
    "walk_non",     // 6奔跑，追击
    "attack_non",   // 7攻击
    "attack_non",   // 8暴击动作
    "hurt_non",     // 9受伤状态
    "chant_",       // 10吟唱动作
    "get_mine",     // 11修炼法宝
    "make_item",    // 12制作物品
    "siting_",      // 13打坐
    "no",           // 14挖掘动作
    "no",           // 15采药动作
    "no",           // 16扒皮动作
    "no",           // 17抽出武器
    "no",           // 18收回武器
    "die",          // 19死亡
    "",             // 20死亡尸体
    "waiting_non",  // 21复活
    "wait_30ride",  // 22骑乘
};
static const char* ChuTouPoseName[] = {
    "wait_mine",         // 0 等待动作
    "attack_t",          // 1 攻击动作
    "walk_mine",         // 2 移动动作
    "die",               // 3 默认死亡动作
    "hurt_t",            // 4 受击动作
    "skill",             // 5 默认技能动作
    "waiting_t",         // 6 耍酷动作
    "attack_t_critical"  // 7 致命攻击动作
};

// 十字镐
static const char* ShiZiGaoPoseName[] = {
    "wait_mine",         // 0 等待动作
    "attack_t",          // 1 攻击动作
    "walk_mine",         // 2 移动动作
    "die",               // 3 默认死亡动作
    "hurt_t",            // 4 受击动作
    "skill",             // 5 默认技能动作
    "waiting_t",         // 6 耍酷动作
    "attack_t_critical"  // 7 致命攻击动作
};

//tooth0708---------------------------------------------------------------
static const char* pDefaultPoseName[] = {
    "waiting_non",  // POSE_NONE
    "attack_non",   // POSE_ATTACK      默认攻击动作
    "walk_non",     // POSE_WALK        默认移动动作
    "wait_non",     // POSE_STAND       默认站立动作
    "hurt_non",     // POSE_HURT        默认受击动作
    "wait_non",     // POSE_IDEL
    "die",          // POSE_DEAD        默认死亡动作
    "c_wait",       // POSE_RIDE        默认死亡动作
    "waiting_non",  // POSE_READY_BORN  默认技能动作
    "waiting_non",  // POSE_BORN
    ""              // POSE_MAX
};

static const char* pDefaultPoseNameSimple[] = {
    "null_",    // 0 空
    "wait",     // 1 待机
    "wait",     // 2 警戒
    "waiting",  // 3 待机
    "talk",     // 4 对话
    "walk",     // 5 跑步
    "run",      // 6 追击
    "attack",   // 7 攻击动作
    "attack",   // 8 持械攻击暴击
    "hurt",     // 9 受击动作
    "wait",     // 10 吟唱
    "wait",     // 11 炼制法宝
    "wait",     // 12 制作物品
    "wait",     // 13 打坐
    "wait",     // 14 挖掘
    "wait",     // 15 采药
    "wait",     // 16 扒皮
    "wait",     // 17 抽出武器
    "wait",     // 18 收回武器
    "die",      // 19 死亡动作
    "body",     // 20 死亡尸体动作
    "wait",     // 21 复活动作
    "skill01",  // 22 默认技能动作
};
// 变身状态的动作
static const char* pShapeshiftPoseName[] = {
    "null_",     // 0 空
    "wait",      // 1 待机
    "wait",      // 2 警戒
    "waiting",   // 3 插播待机	2种
    "wait",      // 4 对话
    "run",       // 5 跑步
    "run",       // 6 追击
    "attack",    // 7 攻击动作
    "critical",  // 8 持械攻击暴击
    "hurt",      // 9 受击动作
    "chant",     // 10 吟唱
    "wait",      // 11 炼制法宝
    "make",      // 12 制作物品
    "wait",      // 13 打坐
    "dig",       // 14 挖掘
    "gather",    // 15 采药
    "scalp",     // 16 扒皮
    "wait",      // 17 抽出武器
    "wait",      // 18 收回武器
    "die",       // 19 死亡动作
    "body",      // 20 死亡尸体动作
    "relive",    // 21 复活动作
    "skill01",   // 22 默认技能动作
};
//表情动作
static const char* pFunActionPoseName[] = {
    "well",   //
    "smile",  //
    "nod",    //
    "no",     //
    "cry",    //
    "cheer",  //
    "wave",   //
};
//tooth0708人物动作基础名
static const char* BasicPoseName[] = {
    "null_",      // 0 空
    "wait_",      // 1 待机
    "wait_a_",    // 2 战斗待机
    "waiting_",   // 3 待机		2种
    "waiting_",   // 4 对话
    "run_",       // 5 跑步
    "run_",       // 6 追击
    "attack_",    // 7 攻击动作			3种
    "critical_",  // 8 持械攻击暴击
    "hurt_",      // 9 受击动作
    "chant_",     // 10 吟唱				2种
    "magic_",     // 11 炼制法宝
    "make_",      // 12 制作物品
    "siting_",    // 13 打坐
    "dig_",       // 14 挖掘
    "gather_",    // 15 采药
    "scalp_",     // 16 扒皮
    "pull_",      // 17 抽出武器
    "down_",      // 18 收回武器
    "die_",       // 19 死亡动作
    "body_",      // 20 死亡尸体动作
    "relive_",    // 21 复活动作
    "skill_",     // 22 默认技能动作
};
//tooth0708人物动作武器连接名
static const char* WeaponPoseName[] = {
    "n",  // 0 空手
    "c",  // 1 杵
    "f",  // 2 斧
    "s",  // 3 丝带
    "h",  // 4 双环
    "l",  // 5 飞轮
    "j",  // 6 剑
    "z",  // 7 短杖
    "q",  // 8 法球
};

// 新 飞剑和飞轮
char* GcBaseActor::GetNewWeaponPose(EPoses Pose, SItemID& item) {
    guard;
    // 动作基础名
    const char* basic = NULL;
    basic             = BasicPoseName[Pose];
    if (!basic || Pose <= POSE_NONE)
        return NULL;
    // 武器名
    const char* weapon = NULL;
    if (ItemID_IsValid(item)) {
        if (ItemIsWeapon_Wheel(item))  //飞轮
            weapon = WeaponPoseName[5];
        else if (ItemIsWeapon_Sword(item))  //剑
            weapon = WeaponPoseName[6];
    } else {
        return NULL;
    }
    if (!weapon)
        return NULL;
    // 动作标号
    int num = 0;
    if (Pose == POSE_IDLE || Pose == POSE_CHANT) {
        if (GetProb(0.5f)) {
            num = 0;
        } else {
            num = 1;
        }
    } else if (Pose == POSE_ATTACK) {
        if (GetProb(0.333333f)) {
            num = 0;
        } else if (GetProb(0.5f)) {
            num = 1;
        } else {
            num = 2;
        }
    }
    rt2_sprintf(m_poseName, "%s%s%d", basic, weapon, num);
    return m_poseName;
    unguard;
}

// tooth0708 根据NPC获取姿势名称
char* GcBaseActor::GetPoseByNPC(EPoses Pose) {
    guard;

    // 获取动作基础名
    const char* basic = NULL;
    basic             = pDefaultPoseNameSimple[Pose];
    // 检查基础名是否有效
    if (!basic || Pose <= POSE_NONE)
        return NULL;

    // 初始化动作标号
    int num = 0;
    // 如果是闲置姿势，随机选择动作标号
    if (Pose == POSE_IDLE) {
        if (GetProb(0.5f)) {
            num = 1;
        } else {
            num = 2;
        }
        // 根据基础名和标号生成完整的姿势名称
        rt2_sprintf(m_poseName, "%s0%d", basic, num);
        return m_poseName;
    }

    // 如果不是闲置姿势，直接使用基础名作为姿势名称
    rt2_sprintf(m_poseName, "%s", basic);
    return m_poseName;

    unguard;
}

//tooth0708
char* GcBaseActor::GetPoseByWeapon(EPoses Pose, SItemID& item) {
    guard;
    // 动作基础名
    const char* basic = NULL;
    basic             = BasicPoseName[Pose];
    if (!basic || Pose <= POSE_NONE)
        return NULL;
    // 武器名
    const char* weapon = NULL;
    if (ItemID_IsValid(item)) {
        if (ItemIsWeapon_Pestle(item))
            weapon = WeaponPoseName[1];
        else if (ItemIsWeapon_Axe(item))
            weapon = WeaponPoseName[2];
        else if (ItemIsWeapon_Riband(item))
            weapon = WeaponPoseName[3];
        else if (ItemIsWeapon_Hoop(item))
            weapon = WeaponPoseName[4];
        else if (ItemIsWeapon_Wheel(item))
            weapon = WeaponPoseName[5];
        else if (ItemIsWeapon_Sword(item))
            weapon = WeaponPoseName[6];
        else if (ItemIsWeapon_Wand(item))
            weapon = WeaponPoseName[7];
        else if (ItemIsWeapon_Ball(item))
            weapon = WeaponPoseName[8];
        else
            return NULL;
    } else {
        weapon = WeaponPoseName[0];
    }
    if (!weapon)
        return NULL;

    // 动作标号
    int num = 0;
    if (Pose == POSE_IDLE || Pose == POSE_CHANT) {
        if (GetProb(0.5f)) {
            num = 0;
        } else {
            num = 1;
        }
    } else if (Pose == POSE_ATTACK) {
        if (GetProb(0.333333f)) {
            num = 0;
        } else if (GetProb(0.5f)) {
            num = 1;
        } else {
            num = 2;
        }
    }

    rt2_sprintf(m_poseName, "%s%s%d", basic, weapon, num);
    return m_poseName;
    unguard;
}

const char* GcBaseActor::GetPoseByWeapon(EPoses PoseId, SItemID& item1, SItemID& item2) {
    guard;
    const char** pose = NULL;  // 初始化姿势数组指针为NULL

    SWeapon *pWeaClass1, *pWeaClass2;  // 定义两个武器类指针
    pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())
                                ->m_pItemManager->GetItem(item1.type));  // 获取第一个物品的武器类
    pWeaClass2 = (SWeapon*)(((CGameClientFrame*)GetApp())
                                ->m_pItemManager->GetItem(item2.type));  // 获取第二个物品的武器类
    if (pWeaClass1 && (ItemIsWeapon_Pestle(item1))) {  // 如果第一个物品双刀
        if (pWeaClass1->bBothHands ||
            (pWeaClass2 && !ItemIsShield(item2.type))) {  // 如果是双手持用或第二个物品不是盾牌
            pose = DaoDoublePoseName;                     //
        } else if (pWeaClass2 && ItemIsShield(item2.type)) {  // 如果第二个物品是盾牌
            pose = DaoPoseWithShield;
        } else {
            pose = DaoPoseWithoutShield;
        }
    } else if (ItemIsWeapon_Riband(item1.type))  // 弓
        pose = BowPoseName;
    else if (ItemIsWeapon_Axe(item1.type))  // 斧头
        pose = AxePoseName;
    else if (ItemIsWeapon_Wheel(item1.type) || ItemIsWeapon_Sword(item1.type) ||
             ItemIsWeapon_Ball(item1.type))  // 轮 剑 球
        pose = FPoseName;
    else if (ItemIsWeapon_Wand(item1.type))  // 杖
        pose = WandPoseName;
    else if (ItemIsWeapon_Hoop(item1.type))  // 鞭
        pose = WhipPoseName;                 //
    else if (ItemIsMineEquip(item1.type))    // 矿
        pose = ShiZiGaoPoseName;
    else if (ItemIsHerbalEquip(item1.type))  // 草药
        pose = ChuTouPoseName;
    else
        pose = DefaultPoseName;  // 默认

    if (!pose)
        return NULL;         // 如果pose为NULL，则返回NULL
    const char* ret = NULL;  // 初始化返回值为NULL
    switch (PoseId) {
        case POSE_ATTACK:
            if (ItemIsWeapon_Pestle(item1.type) || ItemIsWeapon_Axe(item1.type) ||
                ItemIsWeapon_Hoop(item1.type)) {
                if (rand() % 100 > 30)
                    ret = pose[1];
                else
                    ret = pose[7];
            } else {
                ret = pose[1];
            }
            break;
        case POSE_WALK:
            ret = pose[2];
            break;
        case POSE_STAND:
            ret = pose[0];
            break;
        case POSE_GUARD:
            ret = pose[0];
            break;
        case POSE_IDLE:
            ret = pose[6];
            break;
        case POSE_DEAD:
            ret = pose[3];
            break;
        case POSE_HURT:
            ret = pose[4];
            break;
    }
    return ret;
    unguard;
}

//lyymark 播放动作
// @param vPoseID - 动作ID
// @param vLoop - 是否循环播放
// @param pSkill - 技能信息
// @param fSpeed - 播放速度
// @return 返回动作名称,失败返回NULL
const char* GcBaseActor::PlayPose(EPoses vPoseID, bool vLoop, SSkill* pSkill, float fSpeed) {
    guard;
    if (mpModel == NULL) {
        LOG("GcBaseActor::PlayPose 试图播放一个模型信息还没有创建的人物动作.\n");
        return NULL;
    }

    m_vPoseID = vPoseID;
    if (vPoseID < POSE_NONE || vPoseID >= POSE_MAX) {
        return "";
    }
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        return m_pMaster->m_pRidePet->mBaseActor.PlayPose(vPoseID, vLoop, pSkill, fSpeed);
    }

    // add by lyy 2024.12.16  旧动作播放
    CRT_Actor* core = GetGraph()->GetActorInstance()->GetCore();
    if (!core) {
        return nullptr;
    }
    const char* m_coreVersion = core->m_szVersion;
    bool        isOldAct      = std::strcmp(m_coreVersion, "tooth0708") != 0;
    if (isOldAct)
        return OldPlayPose(vPoseID, vLoop, pSkill, fSpeed);
    // add end

    const char* pPoseName = NULL;
    if (mpModel->bComplex)
        pPoseName = pDefaultPoseName[vPoseID];
    else
        pPoseName = pDefaultPoseNameSimple[vPoseID];

    SItemID  Item1, Item2;
    SWeapon *pWeaClass1, *pWeaClass2 = NULL;
    Item1 = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_1, true);
    //Item2 = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_2, true);
    pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));
    //pWeaClass2 = (SWeapon*)( ((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item2.type) );

    int iSkillTime = 0;
    if (!mpModel->bComplex) {
        if (vPoseID == POSE_IDLE) {
            pPoseName = GetPoseByNPC(vPoseID);
        } else if (vPoseID == POSE_ATTACK && pSkill) {
            if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                pPoseName = pSkill->szRAction2;
            else
                pPoseName = pSkill->szRAction1;

            iSkillTime                                   = pSkill->iRTime;
            CActorSkill::SActorPassive* pStrengthenSkill = NULL;
            ////Tianh 修改加强型技能
            if (!(pSkill->wManyStrSkillSub.empty())) {
                for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
                     it != pSkill->wManyStrSkillSub.end(); it++) {
                    pStrengthenSkill = m_pMaster->m_Skill.FindPassiveSkillBySubID(*(it));

                    if (pStrengthenSkill) {
                        //if (pStrengthenSkill->pAttr->iRMinDest)
                        //{
                        //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                        //}
                        if (pStrengthenSkill->pAttr->iRMaxDest) {
                            iSkillTime += pStrengthenSkill->pAttr->iRTime;
                        }
                    }
                }
            }
        }
    } else if (m_pMaster->m_cShapeshift == 2) {
        if (vPoseID == POSE_IDLE) {
            pPoseName = GetPoseByNPC(vPoseID);
        } else if (vPoseID == POSE_WALK) {
            vPoseID   = POSE_RUN;
            pPoseName = pShapeshiftPoseName[vPoseID];
        } else if (vPoseID == POSE_ATTACK && pSkill) {
            pPoseName = pSkill->szRAction1;
        } else {
            pPoseName = pShapeshiftPoseName[vPoseID];
        }
    } else if (pWeaClass1)  // 有武器
    {
        switch (vPoseID) {
            case POSE_ATTACK:
                if (pSkill) {
                    if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                        (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                        pPoseName = pSkill->szRAction2;
                    else
                        pPoseName = pSkill->szRAction1;
                    iSkillTime                                   = pSkill->iRTime;
                    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
                    //if(pSkill->wStrSkillSub)
                    //{
                    //	pStrengthenSkill = m_pMaster->m_Skill.FindPassiveSkillBySubID(pSkill->wStrSkillSub);
                    //}
                    //if (pStrengthenSkill)
                    //{
                    //	if (pStrengthenSkill->pAttr->iRTime)
                    //	{
                    //		iSkillTime = pStrengthenSkill->pAttr->iRTime;
                    //	}
                    //}
                    /*iMyRMaxDest = pSkill->iRMaxDest*/;
                    ////Tianh 修改加强型技能
                    if (!(pSkill->wManyStrSkillSub.empty())) {
                        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
                             it != pSkill->wManyStrSkillSub.end(); it++) {
                            pStrengthenSkill = m_pMaster->m_Skill.FindPassiveSkillBySubID(*(it));

                            if (pStrengthenSkill) {
                                //if (pStrengthenSkill->pAttr->iRMinDest)
                                //{
                                //	iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
                                //}
                                if (pStrengthenSkill->pAttr->iRMaxDest) {
                                    iSkillTime += pStrengthenSkill->pAttr->iRTime;
                                }
                            }
                        }
                    }
                } else {
                    //pPoseName = GetPoseByWeapon(vPoseID, Item1, Item2);
                    pPoseName = GetPoseByWeapon(vPoseID, Item1);
                }
                break;

            case POSE_WALK:
            case POSE_STAND:
            case POSE_IDLE:
            case POSE_DEAD:
            case POSE_HURT:
            case POSE_GUARD:
            default:
                //pPoseName = GetPoseByWeapon(vPoseID, Item1, Item2);
                pPoseName = GetPoseByWeapon(vPoseID, Item1);
                break;
        }
    }
    //else if (!pWeaClass1 && pWeaClass2 && ItemIsShield(Item2.type))
    //{
    //	switch(vPoseID)
    //	{
    //	case POSE_ATTACK: pPoseName = PoseShieldOnly[1]; break;
    //	case POSE_WALK:   pPoseName = PoseShieldOnly[2]; break;
    //	case POSE_STAND:  pPoseName = PoseShieldOnly[0]; break;
    //	case POSE_IDLE:   pPoseName = PoseShieldOnly[6]; break;
    //	case POSE_DEAD:   pPoseName = PoseShieldOnly[3]; break;
    //	case POSE_HURT:   pPoseName = PoseShieldOnly[4]; break;
    //	default:
    //		break;
    //	}
    //}
    else  // 没有武器
    {
        // if (vPoseID==POSE_ATTACK && pSkill) pPoseName = pSkill->szRAction;

        pPoseName = GetPoseByWeapon(vPoseID, Item1);
        if (vPoseID == POSE_ATTACK && pSkill) {
            pPoseName = pSkill->szRAction1;
        }
    }

    if (vPoseID == POSE_FUNACTION) {
        DWORD dwIdx = (DWORD)pSkill;
        char  cDir;
        cDir  = dwIdx & 0x0FF;
        dwIdx = (dwIdx >> 16) & 0x0FFFF;
        if (dwIdx < 1 || dwIdx > 7) {
            dwIdx = 1;
        }
        pPoseName = pFunActionPoseName[dwIdx - 1];
        vLoop     = false;
    }

    if (pPoseName == 0 || pPoseName[0] == 0) {
        //ERR("播放动作失败,动作名字为空\n");
        return NULL;
    }

    if (m_bLastLoop == vLoop && m_bLastLoop == true && strcmp(m_szLastPoseName, pPoseName) == 0) {
        if (!mGraph.p()->IsPlayingPose())
            ERR("Last is a loop cmd, but GetCurrentPose()==NULL!\n");
        return m_szLastPoseName;
    }

    m_bLastLoop = vLoop;
    rt2_strncpy(m_szLastPoseName, pPoseName, 40);
    //m_szLastPoseName[39] = 0;

    SRT_Pose* tpose = mGraph.p()->GetPose(m_szLastPoseName);

    // check normal attack speed
    if (!pSkill && GetWorld() && GetWorld()->m_pPlayer == m_pMaster &&
        (vPoseID == POSE_ATTACK || vPoseID == POSE_BASH)) {
        if (tpose) {
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) / fSpeed) /
                     (m_pMaster->m_core.GetAttackInterval() * 30 / 1000);
        }
    }

    if (iSkillTime) {
        if (tpose)
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) * 1000.f / 30.f) / (float)iSkillTime;
    }

    m_fLastPoseSpeed = fSpeed;
    if (!mGraph.PlayPose(m_szLastPoseName, vLoop, fSpeed)) {
        // ERR2("播放动作失败,模型文件[%s] 动作[%s] \n", mGraph.FileName(), m_szLastPoseName);
        const SRT_Pose* pActorPose = &mGraph.p()->GetCurrentPose();
        if (pActorPose && pActorPose->IsVaild()) {
            rt2_strncpy(m_szLastPoseName, pActorPose->Name.c_str(), 40);
            //m_szLastPoseName[39] = 0;
        }
        return NULL;
    }
    if (pWeaClass1 && !m_pMaster->m_cShapeshiftNet) {
        if (vPoseID == POSE_ATTACK && !pSkill) {
            GcActor* p = FindActor(m_pMaster->m_pCurCmd->dw[0], true);
            if (!p)
                return false;
            if (m_pWeapon) {
                m_pWeapon->DoAttack(p->ID(), m_szLastPoseName, vLoop, fSpeed);
            }
        } else if (m_pWeapon) {
            if (!pSkill) {
                m_pWeapon->SetWeaponState(CWeaponShow::STATE_IDLE);
                if (ItemIsWeapon_NeedLink(Item1)) {
                    if (ItemIsWeapon_Riband(Item1)) {
                        GcActorGraph* pWeaponGraph;
                        pWeaponGraph = m_pMaster->GetGraph()->GetLinkActor("Box03");
                        if (pWeaponGraph) {
                            pWeaponGraph->PlayPose(m_szLastPoseName, vLoop, fSpeed);
                        }
                    }
                } else {
                    m_pWeapon->PlayPose(m_szLastPoseName, vLoop, fSpeed);
                }

                Safe_ReleaseActor(m_pWeapon->m_skillEffect);

            } else {
                //m_way->PlayPoseInTime(skill->szWayName,skill->iRTime,false);
                if (!m_pWeapon->PlayPose(pSkill->szWayName, vLoop, fSpeed))
                    m_pWeapon->PlayPose(m_szLastPoseName, vLoop, fSpeed);
            }
        }
    }
    return pPoseName;
    unguard;
}

const char* GcBaseActor::OldPlayPose(EPoses vPoseID, bool vLoop, SSkill* pSkill, float fSpeed) {

    const char* pPoseName = NULL;

    if (mpModel->bComplex)
        pPoseName = pDefaultPoseName[vPoseID];
    else
        pPoseName = OldSimpleNpcDefaulePoseName[vPoseID];

    SItemID  Item1, Item2;
    SWeapon *pWeaClass1, *pWeaClass2 = NULL;
    int      iSkillTime = 0;

    // 获取主武器信息
    Item1      = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_1, true);
    Item2      = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_2, true);
    pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));

    if (!mpModel->bComplex) {
        if (vPoseID == POSE_ATTACK && pSkill) {
            // 根据武器类型选择攻击动作
            if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                pPoseName = pSkill->szRAction2;  // 选择第二个攻击动作
            else
                pPoseName = pSkill->szRAction1;  // 选择第一个攻击动作
            iSkillTime                                   = pSkill->iRTime;  // 设置技能时间
            CActorSkill::SActorPassive* pStrengthenSkill = NULL;
            ////Tianh 修改加强型技能
            // 处理技能加强效果
            if (!(pSkill->wManyStrSkillSub.empty())) {
                for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
                     it != pSkill->wManyStrSkillSub.end(); it++) {
                    pStrengthenSkill =
                        m_pMaster->m_Skill.FindPassiveSkillBySubID(*(it));  // 根据子ID查找被动技能
                    if (pStrengthenSkill) {
                        if (pStrengthenSkill->pAttr->iRMaxDest) {
                            iSkillTime += pStrengthenSkill->pAttr->iRTime;  // 累加技能时间
                        }
                    }
                }
            }
        }
    } else if (pWeaClass1)  // 有武器
    {
        switch (vPoseID) {
            case POSE_ATTACK:
                if (pSkill) {
                    // 根据武器类型选择攻击动作
                    if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                        (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                        pPoseName = pSkill->szRAction2;
                    else
                        pPoseName = pSkill->szRAction1;
                    iSkillTime                                   = pSkill->iRTime;
                    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
                    ////Tianh 修改加强型技能
                    // 处理多个加强技能
                    if (!(pSkill->wManyStrSkillSub.empty())) {
                        for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
                             it != pSkill->wManyStrSkillSub.end(); it++) {
                            pStrengthenSkill = m_pMaster->m_Skill.FindPassiveSkillBySubID(*(it));

                            if (pStrengthenSkill) {
                                if (pStrengthenSkill->pAttr->iRMaxDest) {
                                    iSkillTime += pStrengthenSkill->pAttr->iRTime;
                                }
                            }
                        }
                    }
                } else {
                    // 获取武器动作
                    pPoseName = GetPoseByWeapon(vPoseID, Item1, Item2);
                }
                break;
            case POSE_WALK:
            case POSE_STAND:
            case POSE_IDLE:
            case POSE_DEAD:
            case POSE_HURT:
            case POSE_GUARD:
            default:
                pPoseName = GetPoseByWeapon(vPoseID, Item1, Item2);
                break;
        }
    } else  // 没有武器
    {
        pPoseName = OldUserDefaulePoseName[vPoseID];  // 获取默认动作
        if (vPoseID == POSE_ATTACK && pSkill) {
            pPoseName = pSkill->szRAction1;
        }
    }

    // 表情动作
    if (vPoseID == POSE_FUNACTION) {
        DWORD dwIdx = (DWORD)pSkill;
        char  cDir;
        cDir  = dwIdx & 0x0FF;
        dwIdx = (dwIdx >> 16) & 0x0FFFF;
        if (dwIdx < 1 || dwIdx > 7) {
            dwIdx = 1;
        }
        pPoseName = pFunActionPoseName[dwIdx - 1];
        vLoop     = false;
    }

    // 检查动作名称是否有效
    if (pPoseName == 0 || pPoseName[0] == 0) {
        //ERR("播放动作失败,动作名字为空\n");
        return NULL;
    }

    // 检查是否重复播放相同的循环动作
    if (m_bLastLoop == vLoop && m_bLastLoop == true && strcmp(m_szLastPoseName, pPoseName) == 0) {
        if (!mGraph.p()->IsPlayingPose())
            P_LOGINFO("Last is a loop cmd, but GetCurrentPose()==NULL!\n");
        return m_szLastPoseName;
    }

    // 保存当前动作信息
    m_bLastLoop = vLoop;
    rt2_strncpy(m_szLastPoseName, pPoseName, 40);
    SRT_Pose* tpose = mGraph.p()->GetPose(m_szLastPoseName);

    // 调整普通攻击速度
    if (!pSkill && GetWorld() && GetWorld()->m_pPlayer == m_pMaster &&
        (vPoseID == POSE_ATTACK || vPoseID == POSE_BASH)) {
        if (tpose) {
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) / fSpeed) /
                     (m_pMaster->m_core.GetAttackInterval() * 30 / 1000);
        }
    }

    // 根据技能时间调整播放速度
    if (iSkillTime) {
        if (tpose)
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) * 1000.f / 30.f) / (float)iSkillTime;
    }

    // 保存动作速度并播放动作
    m_fLastPoseSpeed = fSpeed;
    if (!mGraph.PlayPose(m_szLastPoseName, vLoop, fSpeed)) {
        P_LOGINFO("播放动作失败" + std::string(mGraph.FileName()) + "动作" +
                  std::string(m_szLastPoseName));
        const SRT_Pose* pActorPose = &mGraph.p()->GetCurrentPose();
        if (pActorPose && pActorPose->IsVaild()) {
            rt2_strncpy(m_szLastPoseName, pActorPose->Name.c_str(), 40);
        }
        return NULL;
    }

    // 武器动作
    if (pWeaClass1 && !m_pMaster->m_cShapeshiftNet &&
        (ItemIsWeapon_Wheel(Item1.type) || ItemIsWeapon_Sword(Item1.type) ||
         ItemIsWeapon_Ball(Item1.type))) {
        if (vPoseID == POSE_ATTACK && !pSkill) {                        // 如果是普通攻击
            GcActor* p = FindActor(m_pMaster->m_pCurCmd->dw[0], true);  // 找攻击目标
            if (!p)
                return false;
            if (m_pWeapon) {
                const char* n = GetNewWeaponPose(vPoseID, Item1);
                if (n) {
                    const std::string a = n;
                    P_LOGINFO(a);
                    m_pWeapon->DoAttack(p->ID(), n, vLoop, fSpeed);
                }
            }
        } else if (m_pWeapon) {
            if (!pSkill) {
                m_pWeapon->SetWeaponState(CWeaponShow::STATE_IDLE);  // 设置武器状态为闲置
                if (ItemIsWeapon_NeedLink(Item1)) {
                    if (ItemIsWeapon_Riband(Item1)) {
                        GcActorGraph* pWeaponGraph;
                        pWeaponGraph = m_pMaster->GetGraph()->GetLinkActor("Box03");
                        if (pWeaponGraph)
                            pWeaponGraph->PlayPose(m_szLastPoseName, vLoop, fSpeed);
                    }
                } else {  // 如果不是带绳索的武器，则直接播放武器动作
                    const char* poseName = GetNewWeaponPose(vPoseID, Item1);
                    if (poseName != nullptr) {
                        m_pWeapon->m_poseName = poseName;  // 正常赋值
                    } else {
                        m_pWeapon->m_poseName = "";  // 处理空指针，赋值为空字符串
                    }
                    if (m_pWeapon->m_poseName != m_pWeapon->m_szLastPoseName)
                        m_pWeapon->PlayPose(m_pWeapon->m_poseName.c_str(), true, fSpeed);
                    m_pWeapon->m_szLastPoseName = m_pWeapon->m_poseName;
                }
                // 释放武器技能效果
                Safe_ReleaseActor(m_pWeapon->m_skillEffect);
            } else {
                if (!m_pWeapon->PlayPose(pSkill->szWayName, vLoop, fSpeed))
                    m_pWeapon->PlayPose(m_szLastPoseName, vLoop, fSpeed);
            }
        }
    }
    return pPoseName;
}

bool GcBaseActor::IsLastPose(const char* posename) {
    if (strcmp(m_szLastPoseName, posename) == 0)
        return true;
    return false;
}

void GcBaseActor::StartMove() {
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        if (GetWorld()->m_pPet) {
            m_fMoveSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue();
        }
    } else {
        m_fMoveSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue();
    }
}

void GcBaseActor::OnPoseBegin(SRT_Pose* vpPose) {
    m_pMaster->OnPoseBegin(vpPose);
}

void GcBaseActor::OnPoseEnd(SRT_Pose* vpPose) {
    m_pMaster->OnPoseEnd(vpPose);
}

void GcBaseActor::OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose) {
    m_pMaster->OnPoseBreak(oldPose, newPose);
}

#if DO_CONSOLE
void GcBaseActor::DebugInfo(int vX, int vY) {}
#endif

void GcBaseActor::StopDropItemEffect() {
    if (m_pItemEffect) {
        RtcGetActorManager()->ReleaseActor(m_pItemEffect);
        m_pItemEffect = NULL;
    }
}

void GcBaseActor::PlayDropItemEffect(const char* file /* = "skill_hit1"*/,
                                     const char* link /* = "root" */) {
    guard;
    if (m_pItemEffect) {
        return;
    }
    m_pItemEffect = RtcGetActorManager()->CreateActor(file);
    m_pItemEffect->Disable(AIF_Light_Enable);
    m_pItemEffect->m_bEnableShadow = false;

    m_pItemEffect->SetMatrix(RtgMatrix12::matIdentity);
    if (m_pItemEffect) {
        // m_pItemEffect->Create();
        m_pItemEffect->LinkParent(GetGraph()->p(), link);
        m_pItemEffect->PlayPose("effect", true);
        // m_pItemEffect->SetDrawScale(RtgVertex3(GetGraph()->GetBoundingBox()->vExt.x
        // / m_pItemEffect->GetBoundingBox()->vExt.x,
        //	GetGraph()->GetBoundingBox()->vExt.y /
        // m_pItemEffect->GetBoundingBox()->vExt.y,
        //	GetGraph()->GetBoundingBox()->vExt.z /
        // m_pItemEffect->GetBoundingBox()->vExt.z));
    }
    unguard;
}

void GcBaseActor::PlayEffect(const char* file, bool link, const char* slot, RtgVertex3 offset,
                             bool loop, float speed, const char* szPosename) {
    CRT_ActorInstance* pActor = GetGraph()->p();

    if (pActor == NULL)
        return;

    CRT_ActorInstance* ef = NULL;

    extern const char* GetActorFileName(const char* name);
    const char*        pFileFull = GetActorFileName(file);

    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end();) {
        CRT_ActorInstance* effect = *it;
        if (!effect) {
            it = m_eftList.erase(it);
            continue;
        }
        if (effect->m_Name == pFileFull) {
            if (link && slot) {
                if (effect->m_parentSlot == slot) {
                    ef = effect;
                    break;
                }
            } else {
                RtgMatrix12 matrix = RtgMatrix12::matIdentity;
                matrix.SetRow(3, RtgVertex3(pActor->GetMatrix()->_30 + offset.x,
                                            pActor->GetMatrix()->_31 + offset.y,
                                            effect->m_localMat._32));
                if (matrix == effect->m_localMat) {
                    ef = effect;
                    break;
                }
            }
        }
        it++;
    }

    if (!ef) {
        ef = RtcGetActorManager()->CreateActor(file);
        m_eftList.push_back(ef);
    }

    if (!ef)
        return;

    ef->Disable(AIF_Light_Enable);
    ef->m_bEnableShadow = false;

    if (szPosename == NULL)
        szPosename = "effect";

    if (!ef->IsPlayingPose())
        ef->PlayPose(szPosename, loop, speed);

    ef->SetVisible(1.f);
    ef->SetMatrix(RtgMatrix12::matIdentity);

    if (link && slot) {
        ef->LinkParent(pActor, slot);
    } else {
        ef->SetMatrix_Row(3, RtgVertex3(pActor->GetMatrix()->_30 + offset.x,
                                        pActor->GetMatrix()->_31 + offset.y,
                                        pActor->GetMatrix()->_32 + offset.z));
    }
}

void GcBaseActor::StopEffect(const char* file) {
    extern const char* GetActorFileName(const char* name);
    const char*        pFileFull = GetActorFileName(file);

    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end(); it++) {
        CRT_ActorInstance* ef = *it;
        if (NULL != ef->GetCore()) {
            if (strcmp(ef->GetCore()->m_poTag.c_str(), pFileFull) == 0) {
                RtcGetActorManager()->ReleaseActor(ef);
                m_eftList.erase(it);
                break;
            }
        }
    }
}

bool GcBaseActor::FindEffect(const char* file) {
    if (!file || strlen(file) < 1)
        return false;

    extern const char* GetActorFileName(const char* name);
    const char*        pFileFull = GetActorFileName(file);

    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end(); it++) {
        CRT_ActorInstance* ef = *it;

        if (strcmp(ef->m_Name.c_str(), pFileFull) == 0) {

            return true;
        }
        // end
    }

    return false;
}

void GcBaseActor::ClearEffect() {
    list<CRT_ActorInstance*>::iterator it;
    for (it = m_eftList.begin(); it != m_eftList.end(); it++) {
        RtcGetActorManager()->ReleaseActor(*it);
    }
    m_eftList.clear();
}

void GcBaseActor::OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event) {
    guard;
    m_pMaster->OnPoseEvent(pose, event);
    CRT_ActorInstance* pActor = GetGraph()->p();
    if (pActor == NULL)
        return;
    if (event == NULL)
        return;

    char buf[10][32];

    if (event->Action == "spawn") {
        int link;
        if (sscanf(event->Param.c_str(), "%s %d", buf[0], &link) != 2) {
            LOG("PoseEvent: spawn effect param error\n");
            return;
        }
        PlayEffect(buf[0], true, "root", RtgVertex3(0, 0, 0), false);
    } else if (event->Action == "footprint") {
        RtgMatrix12 _mat;
        bool        bmat = false;

        if (event->Param == "left") {
            bmat = pActor->GetBoneMat(&_mat, "Bip01 L Foot");
        } else {
            bmat = pActor->GetBoneMat(&_mat, "Bip01 R Foot");
        }
        if (bmat) {
            RtgMatrix16 m16;
            RtgMatrix12 mSelfWorld = _mat * pActor->m_matrix;
            m16.Set4X3Matrix(mSelfWorld);
            g_pScene->m_FootPrint.Add(m16, 0);
        }
        return;
    } else if (event->Action == "throw") {
        // 发射物体

        // Param
        // link effect file name(may be "null")
        // //buf[0] missile target act file name(may be "null")
        // //buf[1]
        if (sscanf(event->Param.c_str(), "%s %s", buf[0], buf[1]) != 2) {
            ShowErr("throw parameter error");
            return;
        }

        // SItem*	pItem=NULL;
        // byte	Level=0;
        // m_pMaster->mItem.GetItem(CItemContainer::WEAPON_1,pItem);
        // if(pItem==NULL)
        //{//没有武器
        //	return;
        // }
        // Level = pItem->id.level;

        // 源的ID
        DWORD SourceID = m_pMaster->ID();

        // new　一个抛掷型的Missile
        {
            char*          pMissileName = "";  // pItem->pWea->Act[Level];
            CWhirlMissile* pMissile = RT_NEW CWhirlMissile(SourceID, mAttackTargetID, pMissileName);
            if (strcmp(buf[1], "null") != 0) {  // 目标特效名称
                pMissile->SetTargetEffect(buf[1]);
            }
        }

    } else if (event->Action == "missile") {
        // 发射物体

        // Param
        BOOL       UseParentRotation = FALSE;
        RtgVertex3 OffsetPosition;
        RtgVertex3 OffsetRotation;
        float      MissileSpeed;
        OffsetPosition.Zero();
        OffsetRotation.Zero();

        // parent bone(may be "null")					//buf[0]
        // use parent rotation(true,false) //UseParentRotation offset
        // position(x,y,z)						//OffsetPosition
        // offset rotation(x,y,z)
        // //OffsetRotation missile act file name
        // //buf[1] missile fly speed
        // //MissileSpeed track/beeline
        // //buf[2] missile target act file name(may be "null")	//buf[3]

        if (sscanf(event->Param.c_str(), "%s %d %f %f %f %f %f %f %s %f %s %s", buf[0],
                   &UseParentRotation, &OffsetPosition.x, &OffsetPosition.y, &OffsetPosition.z,
                   &OffsetRotation.x, &OffsetRotation.y, &OffsetRotation.z, buf[1], &MissileSpeed,
                   buf[2], buf[3]) != 12) {
            ShowErr("missile parameter error");
            return;
        }

        // 源的矩阵
        const RtgMatrix12* pSourceMatrix;
        const RtgMatrix12* pBaseMatrix = pActor->GetWorldMatrix();
        if ((strcmp(buf[0], "null") == 0)) {
            pSourceMatrix = pBaseMatrix;
        } else {
            RtgMatrix12 _mat;

            if (!pActor->GetBoneMat(&_mat, buf[0]))
                pSourceMatrix = pBaseMatrix;
            else {
                RtgMatrix12 Matrix = (*pBaseMatrix) * _mat;
                pSourceMatrix      = &Matrix;
            }
        }

        RtgVertex3 Source;
        Source.x = pSourceMatrix->_30;
        Source.y = pSourceMatrix->_31;
        Source.z = pSourceMatrix->_32;

        /*还未处理这个参数
    if(UseParentRotation)
    {//要使用link点的旋转
            //还未处理好
    }
    else
    {//只使用位置信息
    }
    */

        // offset position(x,y,z)
        // //OffsetPosition
        Source += OffsetPosition;

        // offset rotation(x,y,z)
        // //OffsetRotation 该参数还未处理

        // track/beeline
        // //buf[2]
        if (strcmp(buf[2], "track") == 0) {
            // 跟踪类型
            //  missile act file name						//buf[1]
            // missile fly speed
            // //MissileSpeed
            CTrackMissile* pMissile =
                RT_NEW     CTrackMissile(Source, mAttackTargetID, buf[1], NULL, MissileSpeed);

            // missile target act file name(may be "null")	//buf[3]
            if (strcmp(buf[3], "null") != 0) {
                pMissile->SetTargetEffect(buf[2]);
            }

        } else if (strcmp(buf[2], "beeline") == 0) {
            // 直线型
            // 目标
            GcActor* pA = FindActor(mAttackTargetID);
            if (pA == NULL) {
                return;
            }
            GcActorGraph* p = pA->mBaseActor.GetGraph();
            if (p == NULL) {
                return;
            }

            RtgVertex3 Target = p->p()->GetBoundingBox()->vPos;

            Target.x += p->p()->m_matrix._30;
            Target.y += p->p()->m_matrix._31;
            Target.z += p->p()->m_matrix._32;

            // missile act file name						//buf[1]
            // missile fly speed
            // //MissileSpeed
            CBeelineMissile* pMissile =
                RT_NEW       CBeelineMissile(Source, Target, buf[1], NULL, MissileSpeed);

            // missile target act file name(may be "null")	//buf[3]
            if (strcmp(buf[3], "null") != 0) {
                pMissile->SetTargetEffect(buf[3]);
            }
        }
    } else if (event->Action == "footstep") {
        // 脚印
        RtgMatrix12 _mat;
        if (pActor->GetBoneMat(&_mat, event->Param.c_str())) {
            RtgMatrix12 mSelfWorld = _mat * pActor->m_matrix;
            RtgMatrix16 m16;
            m16.Set4X3Matrix(mSelfWorld);
            g_pScene->m_FootPrint.Add(m16, 0);
        }

        // lyymark 脚步声音
        m_pMaster->OnPlayFootfall();
    } else if (event->Action == "pose_end") {
        if (m_pMaster->IsNPC() && m_pMaster->IsIdel()) {
            if (GetProb(0.6)) {
                //				GetGraph()->PlayPose("wait", true);
                PlayPose("wait", true);
            } else {
                if (GetProb(0.5)) {
                    //					GetGraph()->PlayPose("waiting01",
                    // true);
                    PlayPose("waiting01", true);
                } else {
                    //					GetGraph()->PlayPose("waiting02",
                    // true);
                    PlayPose("waiting02", true);
                }
            }
        }
    }

    unguard;
}

void GcBaseActor::OnSetWeaponShow(bool bNeed) {
    if (m_pWeapon)
        m_pWeapon->m_bRender = bNeed;
}

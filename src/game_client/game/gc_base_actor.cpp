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
        case 0:  // ����Ӱ
            break;
        default:
        case 1:  // ȫ������Ȧ
            m_pGraph->GetGraph()->RenderShadow(*GetDevice(), FALSE,
                                               m_pGraph->m_pMaster->GetModel()->ModelRadius);
            break;
        case 2:  // �Լ����޶�̬����Ȧ
            if (m_pGraph->m_pMaster == GetPlayer() || m_pGraph->m_pMaster == GetWorld()->m_pPet) {
                m_pGraph->GetGraph()->RenderShadow(*GetDevice(), TRUE,
                                                   m_pGraph->m_pMaster->GetModel()->ModelRadius);
            } else {
                m_pGraph->GetGraph()->RenderShadow(*GetDevice(), FALSE,
                                                   m_pGraph->m_pMaster->GetModel()->ModelRadius);
            }
            break;
        case 3:  // ȫ����̬
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
    // fox by �ܻ�����//ͷ����ʾ
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
        //	//�����NPC������ɵ�����
        //          for(int i=0; i<m_pActor->m_pNpc->FinishTaskId.size(); i++)
        //	{
        //		if(GetWorld()->m_pPlayer->m_task.IsTaskAccept(m_pActor->m_pNpc->FinishTaskId[i]))
        //		{
        //			//RefreshGraph(TOH_HaveAccept);//by fox for run task
        //			NetSend_c2r_query_task_overhead(m_pActor->ID());//add
        // fox for run task 			return;
        //		}
        //	}
        //	//�����NPC���ɽ��ܵ�����
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

    static std::string strAccept = "pic_f_icon11.act";  // �ɽ��ܵ�
    static std::string strDoing  = "pic_f_icon12.act";  // ���ڽ�����
    static std::string strFinish = "pic_f_icon13.act";  // ����ɵ�

    if (state == TOH_AcceptAndFinish) {
        if (!m_pTaskGraph2) {
            m_pTaskGraph2 = RtcGetActorManager()->CreateActor(strFinish.c_str());
            if (m_pTaskGraph2 == NULL) {
                ERR1("[GcActor::Render] �޷���������ָʾģ�ͣ�name=%s.\n", strFinish.c_str());
                return;
            }
        }
        m_pRender = m_pTaskGraph2;
    } else if (state == TOH_CanAccept) {
        if (!m_pTaskGraph1) {
            m_pTaskGraph1 = RtcGetActorManager()->CreateActor(strAccept.c_str());
            if (m_pTaskGraph1 == NULL) {
                ERR1("[GcActor::Render] �޷���������ָʾģ�ͣ�name=%s.\n", strAccept.c_str());
                return;
            }
        }
        m_pRender = m_pTaskGraph1;
    } else if (state == TOH_AcceptNoFinish) {
        if (!m_pTaskGraph3) {
            m_pTaskGraph3 = RtcGetActorManager()->CreateActor(strDoing.c_str());
            if (m_pTaskGraph3 == NULL) {
                ERR1("[GcActor::Render] �޷���������ָʾģ�ͣ�name=%s.\n", strDoing.c_str());
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

// ���NPCͷ����̬��ʾ
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
                ERR1("[GcActor::Render] �޷���������ͷ��ָʾģ�ͣ�name=%s.\n", strHint.c_str());
                return;
            }
            // if (!m_pActorGraph->Create())
            //{
            //	ERR1("[GcActor::Render] �޷���������ͷ��ָʾģ�ͣ�name=%s.\n",
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
    RtIni iniUser;  // tim.yang  ��ȡ�Ƿ����·�ߵ������ļ�����
    if (iniUser.OpenFile(R(INI_GAME))) {
        iniUser.GetEntry("pathline", "pathline", &m_isDrawPathLine);
    }
    // LOG("==>����GcBaseActor\n");
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
        ERR("GcBaseActor::~GcBaseActor û�д�PathFind������ɾ�\n");
    }
    // LOG("==>����GcBaseActor\n");
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
    // (vpModel==NULL)?"==================�쳣==============":"OK");
    if (mpModel && mpModel != vpModel)
        UnLoadAllSkin();
    mpModel = vpModel;
    return SetGraph(vpModel, HeadModelID, vpModel->Act.c_str());
    unguard;
}

/**
 * @brief ����ͼ��
 *
 * @param HeadModelID ͷģ��ID
 * @param vpActorName ģ���ļ�����
 * @return true
 * @return false
 */
bool GcBaseActor::SetGraph(short HeadModelID, const char* vpActorName) {
    guard;
    return SetGraph(mpModel, HeadModelID, vpActorName);
    unguard;
}

/**
 * @brief ����ͼ��
 *
 * @param vpModel ����ͨ�õ�ģ������
 * @param HeadModelID ͷģ��ID
 * @param vpActorName ģ���ļ�����
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

    mGraph.RegisterActorNotify(this);  // ע��֪ͨ
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
    RtsPathFind* pPathFind = GetWorld()->GetPathFind();  // ��ȡ��ǰ�����·��������
    int          TileX, TileY;

    Point2Tile(vX, vY, TileX, TileY);  // ���������� vX, vY ת��Ϊ�߼��������� TileX, TileY
    // ����ɫ�Ƿ��ƶ������µĸ���

    if (TileX != m_LastTileX || TileY != m_LastTileY) {
        char cBlock = '0';
        // if (m_pMaster && m_pMaster == GetPlayer())
        cBlock = (char)m_blockType;  // ���赲��������Ϊ��ɫ���赲����
        // ʹ��·�����������Խ���ɫ�ƶ����¸���

        if (pPathFind->MoveTo(m_pPathNode, TileX, TileY, cBlock)) {
            m_LastTileX = TileX;  // ����ƶ��ɹ�������������ڵ��߼���������

            m_LastTileY = TileY;
        } else {
            return false;
        }
    }
    return true;
    unguard;
}

void GcBaseActor::Update()  // ��λ�úͷ�����µ�ͼ�β���
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
    // ���� MoveInTile ��������ɫ�ƶ���ָ�����߼�����λ��
    if (!MoveInTile(vX, vY))  // �߼����ӵ�λ��
    {
        m_bMoveToPositionNewMove = TRUE;
        return false;
    }
    // ���½�ɫ�����ƽ�Ʒ���������ɫ�������е� X �� Y ����
    m_Matrix._30 = vX;
    m_Matrix._31 = vY;
    // ��ȡ�����½�ɫ�ڵ�ǰ X, Y λ���ϵĸ߶� (Z ����)
    g_pScene->GetTerrainHeight(m_Matrix._30, m_Matrix._31, m_Matrix._32);
    // �����ɫ�������״̬��������ľ������ɫ����ͬ��
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
        // LOG("���������ڳ����л��߻�δ������״̬\n");
        return false;
    }

    if (bHitTestTouch)  // ���������Ƿ��ܱ�ѡ��
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
    v1 = v0 + vDir * 1000.f;  // ֻ�ж�1000����Զ������

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
  v1 = v0 + vDir * 1000.f; // ֻ�ж�1000����Զ������

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
                            "��Χ��λ��X:     %2.3f(ms)\n"
                            "��Χ��λ��Y:     %2.3f(ms)\n"
                            "��Χ��λ��Z:     %2.3f(ms)\n"
                            "��Χ�����X:     %2.3f(ms)\n"
                            "��Χ�����Y:     %2.3f(ms)\n"
                            "��Χ�����Z:     %2.3f(ms)\n",
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

    // Render path find  //tim.yang   ���������ļ�����������·��
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
    ERR2("GcBaseActor::MoveToTerrainNow �ƶ���ɫ��(%d,%d)ʧ��\n", iTileX, iTileY);
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

// ����true��ʾ�������ߣ�false��ʾ�Ѿ��ߵ��ˣ�����������
// fNeedTime ��ʾ�ߵ�Ŀ�ĵ���Ҫ��ʱ�䣬���С��0����ʹ����ҵ��ٶ�
bool GcBaseActor::MoveToPosition(float fX, float fY, float fDistance, float fNeedTime,
                                 float fDiffTime, bool bTarget) {
    guard;
    // ���·���ڵ�Ϊ�գ�����������Ҳ��ҵķ���������
    if (m_pPathNode == NULL) {
        return AddSelfFind(fX, fY);
    }

    m_pPathFind = GetWorld()->GetPathFind();  // ��ȡ·��������
    if (!m_pPathFind) {
        return false;  // ���û��·�������������� false
    }
    //  �������ң���ɫ����Ϊ1
    int actortype = 0;
    if (GcActor::ENT_USER == m_pMaster->NpcType()) {
        actortype = 1;
    }

    int iTileX, iTileY;                                               // ��ǰ��Ƭ����
    int iFinalTileX, iFinalTileY;                                     // Ŀ����Ƭ����
    g_pScene->GetTerrainByPosFast(fX, fY, iFinalTileX, iFinalTileY);  // ��ȡĿ��λ�õ���Ƭ����
        // ����Ŀ����뵱ǰλ�õľ���
    float fD =
        sqrt((fX - m_Matrix._30) * (fX - m_Matrix._30) + (fY - m_Matrix._31) * (fY - m_Matrix._31));
    if (fD <= fDistance) {
        // �������С�ڵ������������룬��Ϊ�Ѿ�����Ŀ���
        m_bMoveToPositionNewMove = TRUE;
        return false;
    } else if (1 == actortype && fD > 10000.f && fNeedTime >= 0.f) {
        P_LOGINFO("�����Զ:" + std::to_string(fD));
        SetPosition(fX, fY);
        m_bMoveToPositionNewMove = TRUE;
        return false;
    } else if (1 != actortype && fD > 500.f && fNeedTime >= 0.f) {
        P_LOGINFO("�����Զ:" + std::to_string(fD));
        SetPosition(fX, fY);
        m_bMoveToPositionNewMove = TRUE;
        return false;
    }

    bool IgonreActor = /* m_pMaster == GetPlayer() ? false : */ false;

    // ���Ŀ��λ�û�ʱ����֮ǰ�ļ�¼��ͬ�������µ��ƶ�����
    if (fX != m_fMoveToPositionOldX || fY != m_fMoveToPositionOldY ||
        m_fMoveToPositionTime != fNeedTime || m_bMoveToPositionNewMove) {
        // if(actortype == 1)
        //	RtCoreLog().Info("moveto %f,%f -- old %f,%f, needtime %f,%f, bNewmove
        //%d\n", fX, fY, m_fMoveToPositionOldX, m_fMoveToPositionOldY,
        // m_fMoveToPositionTime, fNeedTime, m_bMoveToPositionNewMove);
        //
        // ��¼�µ�Ŀ��λ�ú�ʱ��
        m_fMoveToPositionOldX    = fX;
        m_fMoveToPositionOldY    = fY;
        m_fMoveToPositionTime    = fNeedTime;
        m_bMoveToPositionNewMove = false;
        // ��ȡ��ǰλ�õ���Ƭ����
        g_pScene->GetTerrainByPosFast(fX, fY, iTileX, iTileY);
        // ����·��������ҵ���·��
        if (FindPath(iTileX, iTileY, IgonreActor, bTarget))  // Ŀ�ĵص�·���Ѿ��ҵ�
        {
            if (fNeedTime <= 0.f) {  // �����Ҫʱ��С�ڵ��� 0���趨�ٶ�ΪĬ���ٶȻ�����ٶ�
                if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
                    // m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED;
                    m_fMoveToPositionSpeed = GetWorld()->m_pPet->m_core.MovSpeed.GetValue();
                } else {
                    // m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED;
                    m_fMoveToPositionSpeed = GetWorld()->m_pPlayer->m_core.MovSpeed.GetValue();
                }
            } else {
                // �����ܾ��룬Ȼ������ƽ���ٶ�
                m_fMoveToPositionSpeed = fD / fNeedTime;
                if (m_fMoveToPositionSpeed > DEFAULT_WALK_SPEED * 2.f) {
                    m_fMoveToPositionSpeed = DEFAULT_WALK_SPEED * 2.f;
                    LOG("GcBaseActor::MoveToPosition "
                        "�ƶ��ٶ�̫�죬����������ͨ�ٶȵ�2��.\n");
                } else if (m_fMoveToPositionSpeed < 0.f) {
                    m_fMoveToPositionSpeed = 0.f;
                }
            }
            m_fLastDistance = 1000000.f;  // ��ʼ��������Ϊ�ܴ�ֵ
            if (m_pPathNode->Next(iTileX, iTileY, m_pPathFind, IgonreActor, bTarget,
                                  actortype))  // ���·��������һ�Σ�����Ŀ��λ��
            {
                if (iTileX == iFinalTileX && iTileY == iFinalTileY) {
                    m_vMoveToPosition.x = fX;
                    m_vMoveToPosition.y = fY;
                } else {
                    g_pScene->GetPosByTerrain(iTileX, iTileY, m_vMoveToPosition);
                }
            } else {
                // ���û����һ�Σ�ֱ���趨Ŀ��λ��Ϊ����λ��
                m_vMoveToPosition.x = fX;
                m_vMoveToPosition.y = fY;
            }
            // ����ӵ�ǰλ�õ�Ŀ���ľ���
            fD = sqrt((m_vMoveToPosition.x - m_Matrix._30) * (m_vMoveToPosition.x - m_Matrix._30) +
                      (m_vMoveToPosition.y - m_Matrix._31) * (m_vMoveToPosition.y - m_Matrix._31));
            // ����X��Y�����ϵ��ٶȷ���
            m_fMoveToPositionSpeedX =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.x - m_Matrix._30) / fD;
            m_fMoveToPositionSpeedY =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.y - m_Matrix._31) / fD;
            FaceTo(m_vMoveToPosition.x, m_vMoveToPosition.y);
        } else {
            m_bMoveToPositionNewMove = TRUE;
            return false;  // û�ҵ�Ŀ�ĵأ������Ǳ��赲���߲���ȥ
        }
    }

    float fDX, fDY;

    // ���������һ��Ŀ���ľ���
    fD = sqrt((m_vMoveToPosition.x - m_Matrix._30) * (m_vMoveToPosition.x - m_Matrix._30) +
              (m_vMoveToPosition.y - m_Matrix._31) * (m_vMoveToPosition.y - m_Matrix._31));

    // �����Խ��Խ���Ͷ��ˣ����Խ��ԽԶ����ֹͣ��·
    if (fD < m_fLastDistance) {
        m_fLastDistance = fD;
    } else {
        if (1 != actortype)  // tim.yang  automove  ���Ѱ·�������ж�
        {
            // ���Խ��ԽԶ����ֹͣ��·
            LOG2("Խ��ԽԶ�� %.2f , %.2f\n", m_fLastDistance, fD);
            m_bMoveToPositionNewMove = TRUE;
            return false;  // ·���Ѿ�����
        } else {
            // RtCoreLog().Info("Խ��ԽԶ�� %.2f , %.2f\n", m_fLastDistance, fD);
            m_fMoveToPositionSpeedX =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.x - m_Matrix._30) / fD;
            m_fMoveToPositionSpeedY =
                m_fMoveToPositionSpeed * (m_vMoveToPosition.y - m_Matrix._31) / fD;
            FaceTo(m_vMoveToPosition.x, m_vMoveToPosition.y);
        }
    }

    // ������һ����·��λ��
    if (fD > (fDiffTime * m_fMoveToPositionSpeed)) {
        fDX = m_Matrix._30 + fDiffTime * m_fMoveToPositionSpeedX;
        fDY = m_Matrix._31 + fDiffTime * m_fMoveToPositionSpeedY;
    } else {
        fDX = m_vMoveToPosition.x;
        fDY = m_vMoveToPosition.y;
        if (m_pPathNode->Next(iTileX, iTileY, m_pPathFind, IgonreActor, bTarget,
                              actortype))  // ������һ��
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
            return false;  // ·���Ѿ�����
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

    // ����λ��
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
    m_drawLineStartMatrix = m_Matrix;  // tim.yang  ��¼��ɫ�ƶ�ǰ��λ��
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
    //	actorType = 1;//player  ��������
    //	m_pPathFind->InitAStar();  //��ʼ������
    //	m_pPathNode->ClearPathEx(); //ÿ��Ѱ·ǰ���·������
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
    //	 if (GetWorld()->GetPathFind()->FindSpace(vX, vY, vX, vY))// ע��
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
        //        ע�� [3/16/2009 tooth.shi]
        if (m_pPathNode = (RtsPathPath*)GetWorld()->GetPathFind()->AddActor(
                this->m_pMaster->ID(), actorType, vX, vY, vX, vY, true, m_blockType)) {
            m_bPutOnPathFinder++;
            m_LastTileX = vX;
            m_LastTileY = vY;
            SetPosition(vX, vY);
            return true;
        }
    }
    ERR2("GcBaseActor::AddSelf (ID=%d, Name=%s)���볡������\n", m_pMaster ? m_pMaster->ID() : 0,
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
    // ע�� [3/16/2009 tooth.shi]
    m_pPathNode = (RtsPathPath*)GetWorld()->GetPathFind()->AddActor(
        this->m_pMaster->ID(), actorType, vX, vY, vX, vY, true, m_blockType);
    if (m_pPathNode) {
        m_bPutOnPathFinder++;
        m_LastTileX = vX;
        m_LastTileY = vY;

        SetPosition(vX, vY);
        return true;
    } else {
        ERR("GcBaseActor::AddSelf ���볡������\n");
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
        ERR("GcBaseActor::RemoveSelf �ӳ����Ƴ�����\n");
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
    //	ERR1("GcBaseActor::PlayPose ���Ŷ���ʧ��[%s]\n",vpPoseName);
    // }
    return bResult;

    unguard;
}

bool GcBaseActor::IsPlayer() {
    return (&(GetWorld()->m_pPlayer->mBaseActor) == this);
}

// ��
static const char* BowPoseName[] = {
    "wait_bow",    // 0 վ��
    "attack_bow",  // 1 ����
    "walk_bow",    // 2 �ƶ�
    "die",         // 3 ����
    "hurt_bow",    // 4 �ܻ�
    "skill",       // 5 ����
    "waiting_bow"  // 6 ˣ��
};
// ��
static const char* CrossBowPoseName[] = {
    "wait_crossbow",    // 0 վ��
    "attack_crossbow",  // 1 ����
    "walk_crossbow",    // 2 �ƶ�
    "die",              // 3 ����
    "hurt_crossbow",    // 4 �ܻ�
    "skill",            // 5 ����
    "waiting_crossbow"  // 6 ˣ��
};
// ��ͷ
static const char* AxePoseName[] = {
    "wait_t",            // 0 վ��
    "attack_t",          // 1 ����
    "walk_t",            // 2 �ƶ�
    "die",               // 3 ����
    "hurt_t",            // 4 �ܻ�
    "skill",             // 5 ����
    "waiting_t",         // 6 ˣ��
    "attack_t_critical"  // 7 ����
};

// ��˫��
static const char* DaoDoublePoseName[] = {
    "wait_t_sin",             // 0 վ��
    "attack_t_sin",           // 1 ����
    "walk_t_sin",             // 2 �ƶ�
    "die",                    // 3 ����
    "hurt_t_sin",             // 4 �ܻ�
    "skill",                  // 5 ����
    "waiting_t_sin",          // 6 ˣ��
    "attack_t_sin_critical",  // 7 ����
};

// �����֣��޶��ƣ�
static const char* DaoPoseWithoutShield[] = {
    "wait_sin",             // 0 վ��
    "attack_sin",           // 1 ����
    "walk_sin_r",           // 2 �ƶ�
    "die",                  // 3 ����
    "hurt_sin",             // 4 �ܻ�
    "skill",                // 5 ����
    "waiting_sin",          // 6 ˣ��
    "attack_sin_critical",  // 7 ����
};

// �����֣��ж��ƣ�
static const char* DaoPoseWithShield[] = {
    "wait_sin",             // 0 վ��
    "attack_sin",           // 1 ����
    "walk_sin",             // 2 �ƶ�
    "die",                  // 3 ����
    "hurt_sin",             // 4 �ܻ�
    "skill",                // 5 ����
    "waiting_sin",          // 6 ˣ��
    "attack_sin_critical",  // 7 ����
};

// ֻ�ж���
static const char* PoseShieldOnly[] = {
    "wait_non",     // 0 վ��
    "attack_non",   // 1 ����
    "walk_shield",  // 2 �ƶ�
    "die",          // 3 ����
    "hurt_non",     // 4 �ܻ�
    "skill",        // 5 ����
    "waiting_non",  // 6 ˣ��
    "attack_non",   // 7 ����
};

// ������޶���
static const char* WhipPoseName[] = {
    "wait_whip",             // 0 �ȴ�
    "attack_whip",           // 1 ����
    "walk_whip",             // 2 �ƶ�
    "die",                   // 3 ����
    "hurt_whip",             // 4 �ܻ�
    "skill",                 // 5 ����
    "waiting_whip",          // 6 ˣ��
    "attack_whip_critical",  // 7 ����
};

// �����������
static const char* WhipWeaponPoseName[] = {"wait", "attack",  "walk",
                                           "wait",  // 3 Ĭ����������
                                           "hurt", "skill11", "waiting", "attack_critical"};

// ���﷨������
static const char* FPoseName[] = {
    "wait_non",          // 0 վ��
    "attack_non_magic",  // 1 ����
    "walk_non",          // 2 �ƶ�
    "die",               // 3 ����
    "hurt_non",          // 4 �ܻ�
    "skill",             // 5 ����
    "waiting_non"        // 6 ˣ��
};

// ��
static const char* WandPoseName[] = {
    "wait_wand",    // 0 ���̵ȴ�����
    "attack_wand",  // 1 ���̹�������
    "walk_wand",    // 2 �����ƶ�����
    "die",          // 3 Ĭ����������
    "hurt_wand",    // 4 �����ܻ�����
    "skill",        // 5 Ĭ�ϼ��ܶ���
    "waiting_wand"  // 6 ����ˣ�ᶯ��
};

// Ĭ�϶���
static const char* DefaultPoseName[] = {
    "wait_non",          // 0 Ĭ��վ������
    "attack_non_magic",  // 1 Ĭ�Ϲ�������
    "walk_non",          // 2 Ĭ���ƶ�����
    "die",               // 3 Ĭ����������
    "hurt_non",          // 4 Ĭ���ܻ�����
    "skill",             // 5 Ĭ�ϼ��ܶ���
    "waiting_non"        // 6
};
// ��NPCĬ�϶���
static const char* OldSimpleNpcDefaulePoseName[] = {
    "null_",    // 0 ��
    "wait",     // 1 ����
    "wait",     // 2 ����
    "wait",     // 3 �岥����		2��
    "talk",     // 4 �Ի�
    "walk",     // 5 �ܲ�
    "walk",     // 6 ׷��
    "attack",   // 7 ��������
    "attack",   // 8 ��е��������
    "hurt",     // 9 �ܻ�����
    "wait",     // 10 ����
    "wait",     // 11 ���Ʒ���
    "wait",     // 12 ������Ʒ
    "wait",     // 13 ����
    "wait",     // 14 �ھ�
    "wait",     // 15 ��ҩ
    "wait",     // 16 ��Ƥ
    "wait",     // 17 �������
    "wait",     // 18 �ջ�����
    "die",      // 19 ��������
    "",         // 20 ����ʬ�嶯��
    "wait",     // 21 �����
    "skill01",  // 22 Ĭ�ϼ��ܶ���
};
// ������Ĭ�϶���
static const char* OldUserDefaulePoseName[] = {
    "wait_non",     // 0�����ƣ�Ĭ��״̬
    "wait_non",     // 1վ������
    "wait_non",     // 2ս����������
    "waiting_non",  // 3����״̬
    "wait_non",     // 4�Ի�����
    "walk_non",     // 5����
    "walk_non",     // 6���ܣ�׷��
    "attack_non",   // 7����
    "attack_non",   // 8��������
    "hurt_non",     // 9����״̬
    "chant_",       // 10��������
    "get_mine",     // 11��������
    "make_item",    // 12������Ʒ
    "siting_",      // 13����
    "no",           // 14�ھ���
    "no",           // 15��ҩ����
    "no",           // 16��Ƥ����
    "no",           // 17�������
    "no",           // 18�ջ�����
    "die",          // 19����
    "",             // 20����ʬ��
    "waiting_non",  // 21����
    "wait_30ride",  // 22���
};
static const char* ChuTouPoseName[] = {
    "wait_mine",         // 0 �ȴ�����
    "attack_t",          // 1 ��������
    "walk_mine",         // 2 �ƶ�����
    "die",               // 3 Ĭ����������
    "hurt_t",            // 4 �ܻ�����
    "skill",             // 5 Ĭ�ϼ��ܶ���
    "waiting_t",         // 6 ˣ�ᶯ��
    "attack_t_critical"  // 7 ������������
};

// ʮ�ָ�
static const char* ShiZiGaoPoseName[] = {
    "wait_mine",         // 0 �ȴ�����
    "attack_t",          // 1 ��������
    "walk_mine",         // 2 �ƶ�����
    "die",               // 3 Ĭ����������
    "hurt_t",            // 4 �ܻ�����
    "skill",             // 5 Ĭ�ϼ��ܶ���
    "waiting_t",         // 6 ˣ�ᶯ��
    "attack_t_critical"  // 7 ������������
};

//tooth0708---------------------------------------------------------------
static const char* pDefaultPoseName[] = {
    "waiting_non",  // POSE_NONE
    "attack_non",   // POSE_ATTACK      Ĭ�Ϲ�������
    "walk_non",     // POSE_WALK        Ĭ���ƶ�����
    "wait_non",     // POSE_STAND       Ĭ��վ������
    "hurt_non",     // POSE_HURT        Ĭ���ܻ�����
    "wait_non",     // POSE_IDEL
    "die",          // POSE_DEAD        Ĭ����������
    "c_wait",       // POSE_RIDE        Ĭ����������
    "waiting_non",  // POSE_READY_BORN  Ĭ�ϼ��ܶ���
    "waiting_non",  // POSE_BORN
    ""              // POSE_MAX
};

static const char* pDefaultPoseNameSimple[] = {
    "null_",    // 0 ��
    "wait",     // 1 ����
    "wait",     // 2 ����
    "waiting",  // 3 ����
    "talk",     // 4 �Ի�
    "walk",     // 5 �ܲ�
    "run",      // 6 ׷��
    "attack",   // 7 ��������
    "attack",   // 8 ��е��������
    "hurt",     // 9 �ܻ�����
    "wait",     // 10 ����
    "wait",     // 11 ���Ʒ���
    "wait",     // 12 ������Ʒ
    "wait",     // 13 ����
    "wait",     // 14 �ھ�
    "wait",     // 15 ��ҩ
    "wait",     // 16 ��Ƥ
    "wait",     // 17 �������
    "wait",     // 18 �ջ�����
    "die",      // 19 ��������
    "body",     // 20 ����ʬ�嶯��
    "wait",     // 21 �����
    "skill01",  // 22 Ĭ�ϼ��ܶ���
};
// ����״̬�Ķ���
static const char* pShapeshiftPoseName[] = {
    "null_",     // 0 ��
    "wait",      // 1 ����
    "wait",      // 2 ����
    "waiting",   // 3 �岥����	2��
    "wait",      // 4 �Ի�
    "run",       // 5 �ܲ�
    "run",       // 6 ׷��
    "attack",    // 7 ��������
    "critical",  // 8 ��е��������
    "hurt",      // 9 �ܻ�����
    "chant",     // 10 ����
    "wait",      // 11 ���Ʒ���
    "make",      // 12 ������Ʒ
    "wait",      // 13 ����
    "dig",       // 14 �ھ�
    "gather",    // 15 ��ҩ
    "scalp",     // 16 ��Ƥ
    "wait",      // 17 �������
    "wait",      // 18 �ջ�����
    "die",       // 19 ��������
    "body",      // 20 ����ʬ�嶯��
    "relive",    // 21 �����
    "skill01",   // 22 Ĭ�ϼ��ܶ���
};
//���鶯��
static const char* pFunActionPoseName[] = {
    "well",   //
    "smile",  //
    "nod",    //
    "no",     //
    "cry",    //
    "cheer",  //
    "wave",   //
};
//tooth0708���ﶯ��������
static const char* BasicPoseName[] = {
    "null_",      // 0 ��
    "wait_",      // 1 ����
    "wait_a_",    // 2 ս������
    "waiting_",   // 3 ����		2��
    "waiting_",   // 4 �Ի�
    "run_",       // 5 �ܲ�
    "run_",       // 6 ׷��
    "attack_",    // 7 ��������			3��
    "critical_",  // 8 ��е��������
    "hurt_",      // 9 �ܻ�����
    "chant_",     // 10 ����				2��
    "magic_",     // 11 ���Ʒ���
    "make_",      // 12 ������Ʒ
    "siting_",    // 13 ����
    "dig_",       // 14 �ھ�
    "gather_",    // 15 ��ҩ
    "scalp_",     // 16 ��Ƥ
    "pull_",      // 17 �������
    "down_",      // 18 �ջ�����
    "die_",       // 19 ��������
    "body_",      // 20 ����ʬ�嶯��
    "relive_",    // 21 �����
    "skill_",     // 22 Ĭ�ϼ��ܶ���
};
//tooth0708���ﶯ������������
static const char* WeaponPoseName[] = {
    "n",  // 0 ����
    "c",  // 1 ��
    "f",  // 2 ��
    "s",  // 3 ˿��
    "h",  // 4 ˫��
    "l",  // 5 ����
    "j",  // 6 ��
    "z",  // 7 ����
    "q",  // 8 ����
};

// �� �ɽ��ͷ���
char* GcBaseActor::GetNewWeaponPose(EPoses Pose, SItemID& item) {
    guard;
    // ����������
    const char* basic = NULL;
    basic             = BasicPoseName[Pose];
    if (!basic || Pose <= POSE_NONE)
        return NULL;
    // ������
    const char* weapon = NULL;
    if (ItemID_IsValid(item)) {
        if (ItemIsWeapon_Wheel(item))  //����
            weapon = WeaponPoseName[5];
        else if (ItemIsWeapon_Sword(item))  //��
            weapon = WeaponPoseName[6];
    } else {
        return NULL;
    }
    if (!weapon)
        return NULL;
    // �������
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

// tooth0708 ����NPC��ȡ��������
char* GcBaseActor::GetPoseByNPC(EPoses Pose) {
    guard;

    // ��ȡ����������
    const char* basic = NULL;
    basic             = pDefaultPoseNameSimple[Pose];
    // ���������Ƿ���Ч
    if (!basic || Pose <= POSE_NONE)
        return NULL;

    // ��ʼ���������
    int num = 0;
    // ������������ƣ����ѡ�������
    if (Pose == POSE_IDLE) {
        if (GetProb(0.5f)) {
            num = 1;
        } else {
            num = 2;
        }
        // ���ݻ������ͱ��������������������
        rt2_sprintf(m_poseName, "%s0%d", basic, num);
        return m_poseName;
    }

    // ��������������ƣ�ֱ��ʹ�û�������Ϊ��������
    rt2_sprintf(m_poseName, "%s", basic);
    return m_poseName;

    unguard;
}

//tooth0708
char* GcBaseActor::GetPoseByWeapon(EPoses Pose, SItemID& item) {
    guard;
    // ����������
    const char* basic = NULL;
    basic             = BasicPoseName[Pose];
    if (!basic || Pose <= POSE_NONE)
        return NULL;
    // ������
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

    // �������
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
    const char** pose = NULL;  // ��ʼ����������ָ��ΪNULL

    SWeapon *pWeaClass1, *pWeaClass2;  // ��������������ָ��
    pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())
                                ->m_pItemManager->GetItem(item1.type));  // ��ȡ��һ����Ʒ��������
    pWeaClass2 = (SWeapon*)(((CGameClientFrame*)GetApp())
                                ->m_pItemManager->GetItem(item2.type));  // ��ȡ�ڶ�����Ʒ��������
    if (pWeaClass1 && (ItemIsWeapon_Pestle(item1))) {  // �����һ����Ʒ˫��
        if (pWeaClass1->bBothHands ||
            (pWeaClass2 && !ItemIsShield(item2.type))) {  // �����˫�ֳ��û�ڶ�����Ʒ���Ƕ���
            pose = DaoDoublePoseName;                     //
        } else if (pWeaClass2 && ItemIsShield(item2.type)) {  // ����ڶ�����Ʒ�Ƕ���
            pose = DaoPoseWithShield;
        } else {
            pose = DaoPoseWithoutShield;
        }
    } else if (ItemIsWeapon_Riband(item1.type))  // ��
        pose = BowPoseName;
    else if (ItemIsWeapon_Axe(item1.type))  // ��ͷ
        pose = AxePoseName;
    else if (ItemIsWeapon_Wheel(item1.type) || ItemIsWeapon_Sword(item1.type) ||
             ItemIsWeapon_Ball(item1.type))  // �� �� ��
        pose = FPoseName;
    else if (ItemIsWeapon_Wand(item1.type))  // ��
        pose = WandPoseName;
    else if (ItemIsWeapon_Hoop(item1.type))  // ��
        pose = WhipPoseName;                 //
    else if (ItemIsMineEquip(item1.type))    // ��
        pose = ShiZiGaoPoseName;
    else if (ItemIsHerbalEquip(item1.type))  // ��ҩ
        pose = ChuTouPoseName;
    else
        pose = DefaultPoseName;  // Ĭ��

    if (!pose)
        return NULL;         // ���poseΪNULL���򷵻�NULL
    const char* ret = NULL;  // ��ʼ������ֵΪNULL
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

//lyymark ���Ŷ���
// @param vPoseID - ����ID
// @param vLoop - �Ƿ�ѭ������
// @param pSkill - ������Ϣ
// @param fSpeed - �����ٶ�
// @return ���ض�������,ʧ�ܷ���NULL
const char* GcBaseActor::PlayPose(EPoses vPoseID, bool vLoop, SSkill* pSkill, float fSpeed) {
    guard;
    if (mpModel == NULL) {
        LOG("GcBaseActor::PlayPose ��ͼ����һ��ģ����Ϣ��û�д��������ﶯ��.\n");
        return NULL;
    }

    m_vPoseID = vPoseID;
    if (vPoseID < POSE_NONE || vPoseID >= POSE_MAX) {
        return "";
    }
    if (m_pMaster->m_eRideState == GcActor::ERS_ACTOR) {
        return m_pMaster->m_pRidePet->mBaseActor.PlayPose(vPoseID, vLoop, pSkill, fSpeed);
    }

    // add by lyy 2024.12.16  �ɶ�������
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
            ////Tianh �޸ļ�ǿ�ͼ���
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
    } else if (pWeaClass1)  // ������
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
                    ////Tianh �޸ļ�ǿ�ͼ���
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
    else  // û������
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
        //ERR("���Ŷ���ʧ��,��������Ϊ��\n");
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
        // ERR2("���Ŷ���ʧ��,ģ���ļ�[%s] ����[%s] \n", mGraph.FileName(), m_szLastPoseName);
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

    // ��ȡ��������Ϣ
    Item1      = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_1, true);
    Item2      = m_pMaster->mItem.GetEquipItem(CItemContainer::WEAPON_2, true);
    pWeaClass1 = (SWeapon*)(((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(Item1.type));

    if (!mpModel->bComplex) {
        if (vPoseID == POSE_ATTACK && pSkill) {
            // ������������ѡ�񹥻�����
            if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                pPoseName = pSkill->szRAction2;  // ѡ��ڶ�����������
            else
                pPoseName = pSkill->szRAction1;  // ѡ���һ����������
            iSkillTime                                   = pSkill->iRTime;  // ���ü���ʱ��
            CActorSkill::SActorPassive* pStrengthenSkill = NULL;
            ////Tianh �޸ļ�ǿ�ͼ���
            // �����ܼ�ǿЧ��
            if (!(pSkill->wManyStrSkillSub.empty())) {
                for (vector<int>::iterator it = pSkill->wManyStrSkillSub.begin();
                     it != pSkill->wManyStrSkillSub.end(); it++) {
                    pStrengthenSkill =
                        m_pMaster->m_Skill.FindPassiveSkillBySubID(*(it));  // ������ID���ұ�������
                    if (pStrengthenSkill) {
                        if (pStrengthenSkill->pAttr->iRMaxDest) {
                            iSkillTime += pStrengthenSkill->pAttr->iRTime;  // �ۼӼ���ʱ��
                        }
                    }
                }
            }
        }
    } else if (pWeaClass1)  // ������
    {
        switch (vPoseID) {
            case POSE_ATTACK:
                if (pSkill) {
                    // ������������ѡ�񹥻�����
                    if ((pWeaClass1 && pWeaClass1->bBothHands) ||
                        (pWeaClass1 && pWeaClass2 && !ItemIsShield(Item2) && !ItemIsShield(Item1)))
                        pPoseName = pSkill->szRAction2;
                    else
                        pPoseName = pSkill->szRAction1;
                    iSkillTime                                   = pSkill->iRTime;
                    CActorSkill::SActorPassive* pStrengthenSkill = NULL;
                    ////Tianh �޸ļ�ǿ�ͼ���
                    // ��������ǿ����
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
                    // ��ȡ��������
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
    } else  // û������
    {
        pPoseName = OldUserDefaulePoseName[vPoseID];  // ��ȡĬ�϶���
        if (vPoseID == POSE_ATTACK && pSkill) {
            pPoseName = pSkill->szRAction1;
        }
    }

    // ���鶯��
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

    // ��鶯�������Ƿ���Ч
    if (pPoseName == 0 || pPoseName[0] == 0) {
        //ERR("���Ŷ���ʧ��,��������Ϊ��\n");
        return NULL;
    }

    // ����Ƿ��ظ�������ͬ��ѭ������
    if (m_bLastLoop == vLoop && m_bLastLoop == true && strcmp(m_szLastPoseName, pPoseName) == 0) {
        if (!mGraph.p()->IsPlayingPose())
            P_LOGINFO("Last is a loop cmd, but GetCurrentPose()==NULL!\n");
        return m_szLastPoseName;
    }

    // ���浱ǰ������Ϣ
    m_bLastLoop = vLoop;
    rt2_strncpy(m_szLastPoseName, pPoseName, 40);
    SRT_Pose* tpose = mGraph.p()->GetPose(m_szLastPoseName);

    // ������ͨ�����ٶ�
    if (!pSkill && GetWorld() && GetWorld()->m_pPlayer == m_pMaster &&
        (vPoseID == POSE_ATTACK || vPoseID == POSE_BASH)) {
        if (tpose) {
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) / fSpeed) /
                     (m_pMaster->m_core.GetAttackInterval() * 30 / 1000);
        }
    }

    // ���ݼ���ʱ����������ٶ�
    if (iSkillTime) {
        if (tpose)
            fSpeed = ((tpose->EndFrm - tpose->StartFrm) * 1000.f / 30.f) / (float)iSkillTime;
    }

    // ���涯���ٶȲ����Ŷ���
    m_fLastPoseSpeed = fSpeed;
    if (!mGraph.PlayPose(m_szLastPoseName, vLoop, fSpeed)) {
        P_LOGINFO("���Ŷ���ʧ��" + std::string(mGraph.FileName()) + "����" +
                  std::string(m_szLastPoseName));
        const SRT_Pose* pActorPose = &mGraph.p()->GetCurrentPose();
        if (pActorPose && pActorPose->IsVaild()) {
            rt2_strncpy(m_szLastPoseName, pActorPose->Name.c_str(), 40);
        }
        return NULL;
    }

    // ��������
    if (pWeaClass1 && !m_pMaster->m_cShapeshiftNet &&
        (ItemIsWeapon_Wheel(Item1.type) || ItemIsWeapon_Sword(Item1.type) ||
         ItemIsWeapon_Ball(Item1.type))) {
        if (vPoseID == POSE_ATTACK && !pSkill) {                        // �������ͨ����
            GcActor* p = FindActor(m_pMaster->m_pCurCmd->dw[0], true);  // �ҹ���Ŀ��
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
                m_pWeapon->SetWeaponState(CWeaponShow::STATE_IDLE);  // ��������״̬Ϊ����
                if (ItemIsWeapon_NeedLink(Item1)) {
                    if (ItemIsWeapon_Riband(Item1)) {
                        GcActorGraph* pWeaponGraph;
                        pWeaponGraph = m_pMaster->GetGraph()->GetLinkActor("Box03");
                        if (pWeaponGraph)
                            pWeaponGraph->PlayPose(m_szLastPoseName, vLoop, fSpeed);
                    }
                } else {  // ������Ǵ���������������ֱ�Ӳ�����������
                    const char* poseName = GetNewWeaponPose(vPoseID, Item1);
                    if (poseName != nullptr) {
                        m_pWeapon->m_poseName = poseName;  // ������ֵ
                    } else {
                        m_pWeapon->m_poseName = "";  // �����ָ�룬��ֵΪ���ַ���
                    }
                    if (m_pWeapon->m_poseName != m_pWeapon->m_szLastPoseName)
                        m_pWeapon->PlayPose(m_pWeapon->m_poseName.c_str(), true, fSpeed);
                    m_pWeapon->m_szLastPoseName = m_pWeapon->m_poseName;
                }
                // �ͷ���������Ч��
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
        // ��������

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
        //{//û������
        //	return;
        // }
        // Level = pItem->id.level;

        // Դ��ID
        DWORD SourceID = m_pMaster->ID();

        // new��һ�������͵�Missile
        {
            char*          pMissileName = "";  // pItem->pWea->Act[Level];
            CWhirlMissile* pMissile = RT_NEW CWhirlMissile(SourceID, mAttackTargetID, pMissileName);
            if (strcmp(buf[1], "null") != 0) {  // Ŀ����Ч����
                pMissile->SetTargetEffect(buf[1]);
            }
        }

    } else if (event->Action == "missile") {
        // ��������

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

        // Դ�ľ���
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

        /*��δ�����������
    if(UseParentRotation)
    {//Ҫʹ��link�����ת
            //��δ�����
    }
    else
    {//ֻʹ��λ����Ϣ
    }
    */

        // offset position(x,y,z)
        // //OffsetPosition
        Source += OffsetPosition;

        // offset rotation(x,y,z)
        // //OffsetRotation �ò�����δ����

        // track/beeline
        // //buf[2]
        if (strcmp(buf[2], "track") == 0) {
            // ��������
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
            // ֱ����
            // Ŀ��
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
        // ��ӡ
        RtgMatrix12 _mat;
        if (pActor->GetBoneMat(&_mat, event->Param.c_str())) {
            RtgMatrix12 mSelfWorld = _mat * pActor->m_matrix;
            RtgMatrix16 m16;
            m16.Set4X3Matrix(mSelfWorld);
            g_pScene->m_FootPrint.Add(m16, 0);
        }

        // lyymark �Ų�����
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

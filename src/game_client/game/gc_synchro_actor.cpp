
#include "gc_include.h"

/*----------------------------------------------------------------------------
-   GcSynchroActorManager 角色管理
----------------------------------------------------------------------------*/

GcSynchroActorManager::GcSynchroActorManager()
{
    m_fDownloadTime = 0.f;
    m_sMaxDisplayChar = 50;	
}

GcSynchroActorManager::~GcSynchroActorManager()
{
}

bool GcSynchroActorManager::Init()
{
	return true;
}

bool GcSynchroActorManager::Clear()
{
    guard;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        (*it).second->Destroy();
        DEL_ONE((*it).second);
    }
    m_ActorMap.clear();
    return true;
    unguard;
}

// 开始网络刷新位置等信息
void GcSynchroActorManager::OnBeginDownloadSnapshot()
{
    m_fDownloadTime = GetDevice()->GetAppTime();
}

// 结束网络刷新位置等信息，如果有的物件没有被刷新就会被Disable
void GcSynchroActorManager::OnEndDownloadSnapshot()
{
    guard;
    GcSynchroActor* pActor;
    std::list<GcSynchroActor*> delList;

    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        if (pActor->m_eActiveState==AAS_ACTIVE || pActor->m_eActiveState==AAS_DEATH)
        {
            if (pActor->m_fDownloadTime < m_fDownloadTime - abs(pActor->m_fDownloadWaitTime - 2))
			{
				if (GetPlayer()->m_dungeonTID && pActor->m_pNpc && pActor->triggerID != 0 )
				{
					if (pActor->m_pNpc->Type == 7 || pActor->m_pNpc->Type == 8 )
					{
						continue;
					}
				}
                pActor->DisplayActor(false);

				if (pActor->GetGraph())
				{
					pActor->GetGraph()->EnableShadow(false);
				}

                pActor->m_eActiveState = AAS_INACTIVE;
                LOG("信息: 一个人物因为没有网络数据被隐藏了.\n");
            }
        }else if (pActor->m_eActiveState==AAS_INACTIVE)
        {
            if (m_fDownloadTime - pActor->m_fDownloadTime > pActor->m_fDownloadWaitTime)
			{
				if (GetPlayer()->m_dungeonTID && pActor->m_pNpc && pActor->triggerID != 0 )
				{
					if (pActor->m_pNpc->Type == 7 || pActor->m_pNpc->Type == 8 )
					{
						continue;
					}
				}
                delList.push_back(pActor);
                LOG("信息: 一个人物因为长时间没有网络数据被删除了.\n");
            }
        }
    }

    std::list<GcSynchroActor*>::iterator itList;
    for (itList=delList.begin(); itList!=delList.end(); itList++)
    {
        DeleteActor(*itList);
    }

    unguard;
}

void GcSynchroActorManager::OnDownloadActorSnapshot(GcSynchroActor* pActor, bool bBeforBorn)
{
    guard;
    const RtgMatrix16* pM16 = pActor->GetMatrix();
    // LOG4("Actor: %d, Time %.3f, [%.2f, %.2f]\n", pActor->ID(), m_fDownloadTime, pM16->_30, pM16->_31);
    pActor->m_fDownloadTime = m_fDownloadTime;
    if (pActor->m_eActiveState==AAS_ACTIVE)
    {
    }else if (pActor->m_eActiveState==AAS_INACTIVE)
    {
        pActor->DisplayActor(true);
        pActor->m_eActiveState = AAS_ACTIVE;
        //LOG("信息: 一个不显示但还在缓冲区中的人物被重新显示.\n");
    }else if (pActor->m_eActiveState==AAS_DEATH)
    {
        LOG("警告: 一个已经死去的人物还收到了网络同步信息.\n");
    }
    unguard;
}

void GcSynchroActorManager::UpdateHUD()
{
	guard;
	GcSynchroActor* pActor;
	EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
	for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
	{
		pActor = (*it).second;
		pActor->UpdateHUD();
	}
	unguard
}

void GcSynchroActorManager::Run(float fSecond)
{
    guard;
    GcSynchroActor* pActor;
    std::list<GcSynchroActor*> delList;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        pActor->Run(fSecond);
        if (pActor->m_bDeleteSelf)
        {
            delList.push_back(pActor);
        }
        /*
        if (pActor->m_eActiveState==AAS_ACTIVE)
        {   // 活动的人物的处理
            pActor->Run(fSecond);
        }else if (pActor->m_eActiveState==AAS_DEATH)
        {
            if (pActor->Die(fSecond))
            {
                delList.push_back(pActor);
                LOG("信息: 一个人已经彻底死了.\n");
            }else
            {
                pActor->mBaseActor.Run(fSecond);
            }
        }
        */
    }

    std::list<GcSynchroActor*>::iterator itList;
    for (itList=delList.begin(); itList!=delList.end(); itList++)
    {
        DeleteActor(*itList);
    }
    unguard;
}

void GcSynchroActorManager::OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene)
{
    guard;
    short sDisplayCount = 0;
	short sDisplayPlayer = 0;
    CRT_ActorInstance* p;
    GcSynchroActor* pActor;
    bool bOnlyShowNPC = GetWorld()->m_bOnlyShowNPC;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        if (bOnlyShowNPC)
        {
            if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
            {
				sDisplayPlayer++;
				if (sDisplayPlayer > 10)
				{
					GetWorld()->ShowPlayerInScreen();
				}
                continue;
            }
        }
        if (pActor->m_eActiveState==AAS_INACTIVE)
            continue;
        p = pActor->GetGraph()->p();
        if(p!=NULL)
        {
            if (inCamera.CheckVisable(p->Get_WorldBoundBox()) && sDisplayCount<m_sMaxDisplayChar)
            //if(inCamera.ViewCheckBound(*(p->GetBoundingBox()),*(pActor->GetMatrix())) && sDisplayCount<m_sMaxDisplayChar)
            {

				if (!pActor->m_pHUD.IsEmpty())
				{
					//pActor->m_pHUD->SetBloodValue(pActor->m_cHpRate/3);
					//pActor->m_pHUD->ShowHPBar(false);
				}
				pActor->DisplayActor(pActor->m_bDisplay);

                sDisplayCount ++;
                pScene->AddRenderObject(pActor->mBaseActor.GetRender());
				if (pActor->mBaseActor.m_pStateOverHead)
                {
                    pActor->mBaseActor.m_pStateOverHead->OnAfterScenePreRender(inCamera, pScene);
                }
				//添加NPC头顶动态提示
				pActor->mBaseActor.m_pActorOverHead->OnAfterScenePreRender(inCamera, pScene);
            }
            else
            {
                pActor->DisplayActor(false);
            }
        }
    }
    unguard;
}

void GcSynchroActorManager::DeleteActor(GcSynchroActor* pActor)
{
    guard;
    if (pActor->ID()==GetWorld()->m_dwPetID)
    {
        //GetPlayerPet()->SetPetActor(NULL);
		//GetPlayerPet()->GetPetActor() = NULL;
		GetWorld()->m_pPet = NULL;
    }
    if (pActor->m_eActiveState!=AAS_DEATH)
    {
        pActor->mBaseActor.RemoveSelf();
    }
    pActor->OnRemoveCreature();
    pActor->DisplayActor(false);
    m_ActorMap.erase(pActor->ID());
#ifdef USE_LUA_CHECK
	Lua()->CallLFunc("da","i",pActor);
#endif
	pActor->Destroy();
    DEL_ONE(pActor);
	
    unguard;
}
void GcSynchroActorManager::ActorRebirth(DWORD vID)
{
	guard;
	GcSynchroActor* pActor = this->FindAll(vID);
	if (pActor)
	{
		pActor->m_eActiveState = AAS_ACTIVE;
	}
	unguard;
}

void GcSynchroActorManager::ActorDie(DWORD vID,char dead)
{
    guard;
    GcSynchroActor* pActor = this->FindAll(vID);
    if (pActor)
    {
        /*
#ifdef _DEBUG
        if (pActor->m_eActiveState==AAS_DEATH)
        {
            CHECKEX("人家已经死了，不用再死一次吧");
        }
#endif
        */
		pActor->m_lLastDeadval = pActor->m_bIsDead;
		if (pActor->m_bIsDead == 4)
		{
			return;
		}
		if (dead)
		{
			pActor->m_bIsDead = dead;
		}
        if (pActor->m_eActiveState!=AAS_DEATH || dead == 4)
        {
            ActorDie(pActor);
        }
    }
    unguard;
}

void GcSynchroActorManager::ActorDie(GcSynchroActor* pActor)
{
    guard;
    if (!pActor->m_pHUD.IsEmpty() && pActor->m_eNpcType!=GcActor::ENT_USER)
    {
        pActor->m_pHUD->SetShowFlag(0); // hide all hud
    }
	if (pActor->m_eNpcType!=GcActor::ENT_USER)
	{
		pActor->m_eActiveState = AAS_DEATH;
		if (pActor->m_bIsDead == 4)
		{
			pActor->mBaseActor.RemoveSelf();
		}
	}
    // LOG("信息: 有个家伙死了.\n");
    pActor->OnDie();
    unguard;
}

//处理血条延时消失的bug   tim.yang  
void GcSynchroActorManager::HideHudHp(DWORD vID)
{
	guard;
	GcSynchroActor* pActor = this->FindAll(vID);
	if (pActor && pActor->m_eActiveState==AAS_DEATH)
	{
		if (!pActor->m_pHUD.IsEmpty())
		{
			pActor->m_pHUD->SetShowFlag(0);
			pActor->m_pHUD->ShowHPBar(false);
		}
	}
	unguard;
}

void GcSynchroActorManager::ActorBorn(DWORD vID)
{
    guard;
    GcSynchroActor* pActor = this->FindAll(vID);
    if (pActor)
    {
        LOG("信息: 有个家伙出生了.\n");
        pActor->OnBorn();
    }
    unguard;
}

GcSynchroActor* GcSynchroActorManager::FindActive(DWORD vID, bool bDead)
{
    guard;
	GcSynchroActor* pActor=NULL;
	EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator  It = m_ActorMap.find(vID);
	if(It!=m_ActorMap.end())
	{
		pActor=(*It).second;
        if ((pActor->m_eActiveState!=AAS_ACTIVE && !bDead) || pActor->ID()==GetWorld()->m_dwPetID)
		{
			pActor = NULL;
		}
	}
	return pActor;
    unguard;
}

GcSynchroActor* GcSynchroActorManager::FindAll(DWORD vID)
{
    guard;
    GcSynchroActor* pActor=NULL;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator  It = m_ActorMap.find(vID);
    if(It!=m_ActorMap.end())
    {
        pActor=(*It).second;
    }
    return pActor;
    unguard;
}

GcSynchroActor* GcSynchroActorManager::NewActor(DWORD vID, short vModelID, short vHeadModelID, bool bBeforBorn, short wNpcID)
{
    guard;
    GcSynchroActor* pNew = this->FindAll(vID);
    if (pNew)
    {
        CHECKEX("该人物已经存在");
        return pNew;
    }
    pNew = RT_NEW GcSynchroActor();

	if (wNpcID!=0)
	{
		SNpc* pNpc = g_TableNpc.FindNpc(wNpcID);
		if(pNpc)
		{
			pNew->m_pNpc = pNpc;
		}
	}

    if (!pNew->BindID(vID, vModelID, vHeadModelID, bBeforBorn))
    {
        LOG1("GcSynchroActorManager::NewActor: 模型[%d]不存在.\n", vModelID);
        DEL_ONE(pNew);
        return NULL;
    }
    pNew->m_eActiveState = AAS_ACTIVE;
    m_ActorMap.insert(EXT_SPACE::unordered_map<DWORD,GcSynchroActor*>::value_type(vID, pNew));
    //LOG("信息: 创建一个新的人物.\n");
#ifdef USE_LUA_CHECK
	Lua()->CallLFunc("ca","ii",pNew,vID);
#endif
	int iid;
	Lua()->CallLFunc("fa","i:i",pNew,&iid);
	if(iid != vID)
	{
		int t;
		t = 232;
	}
	return pNew;
    unguard;
}

GcActor* GcSynchroActorManager::HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, float fOutMinDist, bool bHitTestTouch)
{
    guard;
    float fDist;
    GcSynchroActor* pActor;
    GcActor* pNearestActor = NULL;
    int eLastTouchCursor = 0;
    bool bOnlyShowNPC = GetWorld()->m_bOnlyShowNPC;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        if (bOnlyShowNPC)
        {
            if (pActor->NpcType()==GcActor::ENT_USER || pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType() == GcActor::ENT_CALLNPC)
            {
                continue;
            }
        }
        if (/*pActor->m_eActiveState==AAS_ACTIVE && */pActor->m_eLifeState==GcSynchroActor::ELife && pActor->ID()!=GetWorld()->m_dwPetID)
        {   // 活动的人物的处理
            if (pActor->mBaseActor.HitTest(vOrg, vDir, fDist, bHitTestTouch)/* && !pActor->m_pHUD.IsEmpty()*/)
            {
                if (pNearestActor==NULL || fDist<fOutMinDist)
                {
                    pNearestActor = pActor;
                    fOutMinDist = fDist;
                    eLastTouchCursor = m_lastTouchCursor;
                }
            }
        }
    }
	if (GetPlayerPet()->GetPetActor() && pNearestActor == GetPlayerPet()->GetPetActor() && GetPlayerPet()->IsLinkUpPet())
	{
		return NULL;
	}
    m_lastTouchCursor = eLastTouchCursor;
    return pNearestActor;
    unguard;
}

void GcSynchroActorManager::SetMaxDisplayChar(short sMax)
{
    m_sMaxDisplayChar = sMax;
}

void GcSynchroActorManager::OnHUDConfigChanged()
{
    GcSynchroActor* pActor;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        if (pActor->m_eActiveState==AAS_ACTIVE && pActor->m_eLifeState==GcSynchroActor::ELife && pActor->ID()!=GetWorld()->m_dwPetID)
        {
            if (!pActor->m_pHUD.IsEmpty())
            {
                pActor->m_pHUD->ApplyFlag();
            }
        }
    }
}

void GcSynchroActorManager::OnMapChanged()
{
    GcSynchroActor* pActor;
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
    for (it=m_ActorMap.begin(); it!=m_ActorMap.end(); it++)
    {
        pActor = (*it).second;
        pActor->OnMapChanged();
    }
}

/*----------------------------------------------------------------------------
-   GcSynchroActor 角色
----------------------------------------------------------------------------*/

const float g_fClientDieTime = 5.f;
const float g_fClientDieFlyHeight = 60.f;
const float g_fClientDieSinkHeight = -100.f;

GcSynchroActor::GcSynchroActor()
{
    m_fDownloadTime = 0.f;
    m_eActiveState = AAS_INACTIVE;
	m_fCurDieTime = g_fClientDieTime;
	m_eLifeState = ELife;
	m_fDownloadWaitTime = 5.f;
}

GcSynchroActor::~GcSynchroActor()
{
}

bool GcSynchroActor::BindID(DWORD vID, short vModelID, short vHeadModelID, bool bBeforBorn)
{
    guard;
    this->SetID(vID);
    this->CreateGraphData(vModelID, vHeadModelID);
    if (bBeforBorn)
    {
        m_eLifeState = EPrenatal;
        mBaseActor.GetGraph()->PlayPose("static");
    }
    return true;
    unguard;
}

bool GcSynchroActor::Die(float fSecond)
{
    return false;
}

void GcSynchroActor::OnDie()
{
    guard;
    AddCommand(ACMD_DEATH);
    unguard;
}

void GcSynchroActor::OnBorn()
{
    guard;
    AddCommand(ACMD_BORN);
    m_eLifeState = ELife;
    unguard;
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

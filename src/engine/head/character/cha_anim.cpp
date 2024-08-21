
//////////////////////////////////////////////////////////////////////////
//
//   FileName : cha_anim.cpp
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc : refactor
//
//////////////////////////////////////////////////////////////////////////

#include "character/actor.h"

RT_IMPLEMENT_DYNCREATE(CRT_ActorInstance, RtObject, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_SkinInstance, RtObject, NULL, "")

const int BOUND_MIN = 10;
EActorQuality QuaActor = QUALITY_HIGH;
EActorQuality QuaSkin  = QUALITY_HIGH;
EActorQuality QuaMtl   = QUALITY_HIGH;
EActorQuality QuaEft   = QUALITY_HIGH;
//char fileNameBuf[512];
//G_MEMDEF(fileNameBuf, 512)
//void	RegAnimMemory(void)
//{
//	G_MEMPROTECTOR(fileNameBuf, 512)
//}

void ActorSetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect)
{
	QuaActor = actor;
	QuaSkin  = skin;
	QuaMtl   = mtl;
	QuaEft   = effect;
}

void ActorGetQuality(EActorQuality &actor,EActorQuality &skin,EActorQuality &mtl,
					 EActorQuality &effect)
{
	actor   = QuaActor; 
	skin	= QuaSkin;   
	mtl		= QuaMtl;    
	effect	= QuaEft; 
}

const char* GetActorFileName(const char* name)
{
	S_MEMDEF(fileNameBuf, MAX_PATH)
	S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
	size_t len = strlen(name);
	if (name[len - 4] == '.' && 
        name[len - 3] == 'a' && 
        name[len - 2] == 'c' && 
        name[len - 1] == 't')
	{
		return name;
	}
	else
	{
		rt2_sprintf(fileNameBuf, "%s.act", name);
		return fileNameBuf;
	}
}

const char* GetSkinFileName(const char* name)
{
	S_MEMDEF(fileNameBuf, MAX_PATH)
	S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
	size_t len = strlen(name);
	
    if (name[len - 4] == '.' && 
        name[len - 3] == 's' && 
        name[len - 2] == 'k' && 
        name[len - 1] == 'i')
	{
		return name;
	}
	else
	{
		rt2_sprintf(fileNameBuf, "%s.ski", name);
		return fileNameBuf;
	}
}

const char* GetEffectFileName(const char* name)
{
	S_MEMDEF(fileNameBuf, MAX_PATH)
	S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
	size_t len = strlen(name);
	
    if (name[len - 4] == '.' && 
        name[len - 3] == 'e' && 
        name[len - 2] == 'f' && 
        name[len - 1] == 't')
	{
		return name;
	}
	else
	{
		rt2_sprintf(fileNameBuf, "%s.eft", name);
		return fileNameBuf;
	}
}

const char* GetMtlLibFileName(const char* name)
{
	S_MEMDEF(fileNameBuf, MAX_PATH)
	S_MEMPROTECTOR(fileNameBuf, MAX_PATH, bMP)
	size_t len = strlen(name);
	
    if (name[len - 4] == '.' && 
        name[len - 3] == 'm' && 
        name[len - 2] == 't' && 
        name[len - 1] == 'l')
	{
		return name;
	}
	else
	{
		rt2_sprintf(fileNameBuf, "%s.mtl", name);
		return fileNameBuf;
	}
}

void TagPoseNotify::OnPoseBegin(SRT_Pose *pose) 
{
    if (!pose || !pose->pParentAct)
        return;
    pose->pParentAct->Enable(true, true);
}

 void TagPoseNotify::OnPoseEnd(SRT_Pose *pose) 
 {
    if (!pose || !pose->pParentAct)
        return;
    pose->pParentAct->Enable(false, false);
}

bool CRT_ActorInstance::m_bGlobalRenderActor  = true;
bool CRT_ActorInstance::m_bGlobalRenderShadow = true;

CRT_ActorInstance::CRT_ActorInstance()
    : m_notify(NULL),
      m_curFrame(0),
      m_oldFrame(0),
      m_bLoop(false),
      m_lastFrame(-1),
      m_core(NULL),
      m_parent(NULL),
      m_bValid(false),
      m_bEnableShadow(true),
      m_animSpeed(1.f),
      m_visible(1.f),
      m_bRenderEffect(true),
      m_bRenderByParent(false),
      m_bDisplay(true),
      m_bUpdate(true),
      m_linkCount(0),
      m_bUseLight(true),
      m_bUseVC(false),
      m_bZTest(true),
      m_Flags(0),
      m_lastTickFrame(-1),
#ifdef CHAR_VIEWER
      m_bDrawScale(true)
#else
      m_bDrawScale(false)
#endif
{
    m_curPose.Invalid();
    m_prePose.Invalid();
    m_aabboxWorld.Zero();
	m_boundingBox.Zero();
	m_matrix.Unit();
	m_localMat.Unit();
	m_drawScale.Set(1.f, 1.f, 1.f);
	m_offset.Set(0.f, 0.f, 0.f);
    SetState(Object_State_New);
}

bool CRT_ActorInstance::CoreLinkParent(CRT_ActorInstance* parent, const char* slot)
{   // 如果父实例为空，返回false
    if (!parent)
        return false;
    // 如果父实例的状态是就绪状态，直接调用具体的链接实现
    if (parent->GetState() == Object_State_Ready)
        return CoreLinkParentImpl(parent, slot);

    // 创建一个用于链接的命令并添加到命令列表
    Rac_ActorLink* _cmd = CreateActorCommand<Rac_ActorLink>();
    _cmd->ltype = Rac_ActorLink::_link;
    _cmd->_parent = parent;
    if (slot)
        _cmd->_slot = slot;
    m_listCommand.push_back(_cmd);	
    ++m_linkCount;

    return true;
}

void CRT_ActorInstance::CoreUnlinkParent()
{
    // 如果链接计数小于等于0，则直接调用具体的解除实现
    if (m_linkCount <= 0)
        return CoreUnlinkParentImpl();
    // 创建一个用于解除链接的命令并添加到命令列表
    Rac_ActorLink* _cmd = CreateActorCommand<Rac_ActorLink>();
    _cmd->ltype = Rac_ActorLink::_unlink;
    m_listCommand.push_back(_cmd);	
}

// 具体实现将当前实例链接到父实例，并指定一个槽位
bool CRT_ActorInstance::CoreLinkParentImpl(CRT_ActorInstance* parent, const char* slot)
{// 如果父实例为空或当前实例已有父实例，则返回false
    if (!parent || m_parent) 
        return false;
    // 设置当前实例的父实例
    m_parent = parent;
    m_parentSlot.clear();
    // 如果提供了槽位信息，则保存该槽位信息
    if (slot)
        m_parentSlot = slot;
    // 将当前实例添加到父实例的子列表中
    parent->AddChild(this);
    // 更新矩阵
    UpdateMatrix();

    return true;
}

void CRT_ActorInstance::CoreUnlinkParentImpl()
{
    if (!m_parent) 
        return;

    m_parent->DelChild(this);
    m_parent = NULL;
    m_parentSlot.clear();
    UpdateMatrix();

}
// 将当前实例链接到父实例，并指定一个槽位
bool CRT_ActorInstance::LinkParent(CRT_ActorInstance* parent, const char* slot)
{
    // 首先解除当前的父子链接关系
	CoreUnlinkParent();
    // 然后链接到新的父实例
    return CoreLinkParent(parent, slot);
}

void CRT_ActorInstance::UnlinkParent()
{
	CoreUnlinkParent();
}

CRT_ActorInstance::~CRT_ActorInstance()
{
    Destroy();
}

bool CRT_ActorInstance::PlayPose(const char* name, bool loop, float speed)
{
    if (GetState() == Object_State_Ready)
    {
        SRT_Pose* pose = GetPose(name);

        if (!pose)
            return false;

        if (pose->ResourceReady())
            return PlayPoseImpl(pose, loop, speed, Rac_PlayPose::_speed);
    }

    Rac_PlayPose* _cmd = CreateActorCommand<Rac_PlayPose>();
    if (name)
        _cmd->pname = name;
    _cmd->pmode = Rac_PlayPose::_speed;
    _cmd->speed = speed;
    _cmd->bloop = loop;
    m_listCommand.push_back(_cmd);	

	return true;
}

bool CRT_ActorInstance::PlayPoseInTime(const char* name, int mills, bool loop)
{
    if (GetState() == Object_State_Ready)
    {
        SRT_Pose* pose = GetPose(name);

        if (!pose)
            return false;

        if (pose->ResourceReady())
            return PlayPoseImpl(pose, loop, (float)mills / 1000.f, Rac_PlayPose::_speed);
    }

    Rac_PlayPose* _cmd = CreateActorCommand<Rac_PlayPose>();
    if (name)
        _cmd->pname = name;
    _cmd->pmode = Rac_PlayPose::_time;
    _cmd->speed = (float)mills;
    _cmd->bloop = loop;
    m_listCommand.push_back(_cmd);	

    return true;
}

bool CRT_ActorInstance::PlayPoseImpl(const char name[], bool loop, float speed, DWORD _mode)
{
    return PlayPoseImpl(GetPose(name), loop, speed, _mode);
}

bool CRT_ActorInstance::PlayPoseImpl(SRT_Pose* pose, bool loop, float speed, DWORD _mode)
{
    if (!pose) 
        return false;

    bool bNeedActive = (pose->Name != m_curPose.Name);

    if (m_curPose.IsVaild() && bNeedActive)
        m_curPose.UnActive();

    if (_mode == Rac_PlayPose::_time)
        speed = (pose->EndFrm - pose->StartFrm) / 30.0f * 1000.0f / (int)speed;

    m_animSpeed = speed;
    m_bLoop     = loop;
    m_curFrame  = pose->StartFrm;
    m_oldFrame  = pose->StartFrm;
    m_lastFrame = -1;
    m_curPose.StartFrm = pose->StartFrm;
    m_curPose.EndFrm = pose->EndFrm;

    if (bNeedActive)
    {
        m_curPose.InitFrom(pose);
        m_curPose.Active(this);
    }

    UseFrame(m_curFrame);

    if (m_notify)
        m_notify->OnPoseBegin(&m_curPose);

    return true;
}

bool CRT_ActorInstance::SetVCImpl(vcArray_t* aryVc)
{
    if (!IsEnableVC())
        return false;

    size_t vcNum = 0;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        vcNum += m_skinList[i]->Get_VertexArray()->size();
    }

    if (vcNum != aryVc->size())
    {
        RtCoreLog().Error("%s set vc fail\n", m_Name.c_str());
        return false;
    }

    int offset = 0;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        m_skinList[i]->SetVC(*aryVc, offset);
        offset += (int)m_skinList[i]->Get_VertexArray()->size();
    }

    return true;
}

void CRT_ActorInstance::ProcessComnandList()
{
    if (!m_listCommand.empty())
    {
        Rac_CmdType* _cmd = (Rac_CmdType*)m_listCommand.front();
        bool bprocessed = false; 

        switch (GetActorCmdtype(_cmd))
        {
        case Ractype_PlayPose :
            {
                Rac_PlayPose* _cmdp = (Rac_PlayPose*)_cmd;
                SRT_Pose* pose = GetPose(_cmdp->pname.c_str());

                if (pose)
                {
                    if (pose->ResourceReady())
                    {
                        PlayPoseImpl(pose, _cmdp->bloop, _cmdp->speed, _cmdp->pmode);
                        bprocessed = true;
                    }
                    else
                    {
                        bprocessed = false;
                    }
                }
                else
                {
                    bprocessed = true;
                }

            }
            break;
        case Ractype_Setvc :
            {
                Rac_Setvc* _cmdp = (Rac_Setvc*)_cmd;
                SetVCImpl(_cmdp->_vc);
                bprocessed = true;
            }
            break;
        case Ractype_Setbasecolor :
            {
                Rac_Setbasecolor* _cmdp = (Rac_Setbasecolor*)_cmd;
                SetBaseColorImpl(_cmdp->color, _cmdp->bchild);
                bprocessed = true;
            }
            break;
        case Ractype_Acotrlink :
            {
                Rac_ActorLink* _cmdp = (Rac_ActorLink*)_cmd;
                if (_cmdp->ltype == Rac_ActorLink::_link)
                {
                    if (_cmdp->_parent->GetState() == Object_State_Ready)
                    {
                        CoreLinkParentImpl(_cmdp->_parent, _cmdp->_slot.c_str());
                        --m_linkCount;
                        bprocessed = true;
                    }
                    else
                        bprocessed = false;
                }
                else
                {
                    CoreUnlinkParentImpl();
                    bprocessed = true;
                }
            }
            break;
        case Ractype_LoadSub :
            {
                Rac_LoadSub* _cmdp = (Rac_LoadSub*)_cmd;
                switch (_cmdp->stype)
                {
                case Rac_LoadSub::_sub_skin :
                    if (_cmdp->otype == Rac_LoadSub::_load)
                        LoadSkinImpl(_cmdp->sname.c_str(), _cmdp->bdisableMip);
                    else
                        UnloadSkinImpl(_cmdp->sname.c_str());
                    break;
                case Rac_LoadSub::_sub_mtl :
                    if (_cmdp->otype == Rac_LoadSub::_load)
                        ;
                    else
                        ;
                    break;
                case Rac_LoadSub::_sub_efft :
                    if (_cmdp->otype == Rac_LoadSub::_load)
                        ApplyEffectImpl(_cmdp->sname.c_str());
                    else
                        RemoveEffect(_cmdp->sname.c_str());
                    break;
                default :
                    break;
                }
                bprocessed = true;
            }
            break;
        case Ractype_Setscale :
            {
                Rac_Setscale* _cmdp = (Rac_Setscale*)_cmd;
                SetDrawScaleImpl(_cmdp->scale);
                bprocessed = true;
            }
            break;
        case Ractype_Setmtl :
            {
                Rac_Setmtl* _cmdp = (Rac_Setmtl*)_cmd;
                ChangeMaterialImpl(_cmdp->_skinIndex, _cmdp->mtlname.c_str());
                bprocessed = true;
            }
            break;
        case Ractype_Setvisible :
            {
                Rac_Setvisible* _cmdp = (Rac_Setvisible*)_cmd;
                SetVisibleImpl(_cmdp->_visible);
                bprocessed = true;
            }
            break;
        default :
            RtCoreLog().Warn("no actor comnand process(type : %u)\n", GetActorCmdtype(_cmd));
            bprocessed = false;
            break;
        }

        if (bprocessed)
        {
            DestroyActorCommand(_cmd);
            m_listCommand.pop_front();
        }
    }
}

void CRT_ActorInstance::ClearComnandList()
{
    m_listCommand.clear();

    for (size_t i = 0; i < m_skinList.size(); ++i)
        if (m_skinList[i]) 
            m_skinList[i]->ClearComnandList();
}

// lyymark CRT_ActorInstance 主帧动画处理
void CRT_ActorInstance::Tick(float deltaMill, bool bUpdateChild /* = true */)
{
    DWORD tkTick = rtMilliseconds();

    if (!m_bUpdate || GetState() != Object_State_Ready)
        return;

    if (m_lastTickFrame == RtGetRender()->GetRenderFrame())
        return;
    m_lastTickFrame = RtGetRender()->GetRenderFrame();

    ProcessComnandList();

    float OldFrame = m_oldFrame;

    m_curFrame += (deltaMill / 1000.f * 30.f * m_animSpeed);
    m_oldFrame = m_curFrame;

	if (m_curPose.IsVaild() && m_curPose.ResourceReady())
	{
		if (m_curFrame >= m_curPose.EndFrm)
		{
            ProcessPoseEvent(OldFrame, m_curPose.EndFrm + 1, &m_curPose);

			if (m_bLoop)
			{
                m_curFrame = m_curPose.StartFrm;
                m_oldFrame = m_curFrame;
			}
			else
			{
                m_prePose = m_curPose;
				m_curFrame = m_curPose.EndFrm;
				m_oldFrame = m_curFrame;
                StopPose();

				if (m_notify) 
                    m_notify->OnPoseEnd(&m_prePose);

			}
		}
		else
		{
			ProcessPoseEvent(OldFrame, m_curFrame, &m_curPose);
		}
	}
    //可能是骨骼动画
	RealUseFrame(m_curFrame);

    DWORD ruSkin = rtMilliseconds();
	for (size_t i = 0; i < m_skinList.size(); ++i)
		if (m_skinList[i]) 
            //lyymark 这里是CRT_SkinInstance
            m_skinList[i]->Tick(deltaMill);
    RtGetPref()->skinUpdate += rtMilliseconds() - ruSkin;

    DWORD ruEfft = rtMilliseconds();
	for (size_t i = 0; i < m_effectList.size(); ++i)
		if (m_effectList[i]->RequestTick()) 
            //lyymark 这里是粒子发射器的Tick CRT_EffectEmitter类
            m_effectList[i]->Tick(deltaMill);
    RtGetPref()->efftUpdate += rtMilliseconds() - ruEfft;

    RtGetPref()->ActrUpdate += rtMilliseconds() - tkTick;

    if (bUpdateChild)
    {
        for (size_t i = 0; i < m_childs.size(); ++i)
            m_childs[i]->Tick(deltaMill, bUpdateChild);
    }

}

void CRT_ActorInstance::ProcessPoseEvent(float oldFrame, float curFrame, SRT_Pose *curPose)
{
#ifdef MAX_PLUGIN_EXPORTS
    return;
#endif

    size_t evnNum = curPose->EventList.size();

    if (evnNum <= 0)
        return;

    for (size_t i = 0; i < evnNum; ++i)
    {
        SRT_PoseEvent* event = &curPose->EventList[i];
        long key = event->KeyFrame;

        if (oldFrame <= key && curFrame > key)
        {
            char buf[32];
            float rate;

            if (event->Action == "animation")
            {
                if (event->nTagActIndex >= 0 && event->nTagActIndex < (int)curPose->vecTagActs.size())
                {
                    CRT_ActorInstance* act = curPose->vecTagActs[event->nTagActIndex];

                    if (act)
                    {
                        act->PlayPose(event->Param1.c_str(), false);
                    }
                }
            }
            else if(event->Action == "sound")
            {
                if (sscanf(event->Param.c_str(),"%s %f",buf,&rate) != 2)
                {
                    RtCoreLog().Info("DefaultPoseEvent: sound param error\n");
                    return;
                }

                int ran = rand() << 16;

                if (ran % 1000000 < rate * 1000000)
                {
                    int len = strlen(buf);
                    buf[len++] = '.';
                    buf[len++] = 'w';
                    buf[len++] = 'a';
                    buf[len++] = 'v';
                    buf[len++] = '\0';
                    g_pSoundMgr->PlayOnce(buf,false,0,RtgVertex3(m_matrix._30,m_matrix._31,m_matrix._32));
                }
            }
            else if(event->Action == "camera light")
            {
                RtgVertex3 color;

                if (sscanf(event->Param.c_str(), "%f %f %f", &color.m_v[0], &color.m_v[1], &color.m_v[2]) != 3)
                {
                    RtCoreLog().Info("DefaultPoseEvent: camera light param error\n");
                    return;
                }

                RtGetRender()->m_pCamera->SetColoredGlass(RtgCamera::COLOR_ADD, RtgVectorToColor(color,1));
            }
            else if (event->Action == "camera quake")
            {
                float fScope;
                float fCycle;
                float fTime;

                if (sscanf(event->Param.c_str(), "%f %f %f", &fScope, &fCycle, &fTime) != 3)
                {
                    RtCoreLog().Info("DefaultPoseEvent: camera quake param error\n");
                    return;
                }

                RtGetRender()->m_pCamera->Tremble(fScope, fCycle, fTime);
            }
            else if (m_notify)
            {
                m_notify->OnPoseEvent(curPose,&curPose->EventList[i]);
            }
        }
    }
}

bool CRT_ActorInstance::PlayPose(bool loop /* = false */, float speed /* = 1.0f */)
{
    return PlayPose("first", loop, speed);
}

void CRT_ActorInstance::LoadSkin(const char* name, bool bdisableMip /* = false */)
{
    if (!name)
        return;

    if (GetState() == Object_State_Ready)
        return LoadSkinImpl(name, bdisableMip);

    Rac_LoadSub* _cmd = CreateActorCommand<Rac_LoadSub>();
    _cmd->stype = Rac_LoadSub::_sub_skin;
    _cmd->otype = Rac_LoadSub::_load;
    _cmd->sname = name;
    _cmd->bdisableMip = bdisableMip;
    m_listCommand.push_back(_cmd);	
}

void CRT_ActorInstance::UnloadSkin(const char* name)
{
    if (!name)
        return;

    if (m_listCommand.empty())
        return UnloadSkinImpl(name);

    Rac_LoadSub* _cmd = CreateActorCommand<Rac_LoadSub>();
    _cmd->stype = Rac_LoadSub::_sub_skin;
    _cmd->otype = Rac_LoadSub::_unload;
    _cmd->sname = name;
    m_listCommand.push_back(_cmd);	

}

void CRT_ActorInstance::UnloadAllSkin()
{
    list<void*>::iterator _it = m_listCommand.begin();

    while (_it != m_listCommand.end())
    {
        Rac_CmdType* _cmd = (Rac_CmdType*)(*_it);

        if (GetActorCmdtype(_cmd) == Ractype_LoadSub              &&
            ((Rac_LoadSub*)_cmd)->stype == Rac_LoadSub::_sub_skin &&
            ((Rac_LoadSub*)_cmd)->otype == Rac_LoadSub::_load)
        {
            _it = m_listCommand.erase(_it);
        }
    }

    for (size_t i = 0; i < m_skinList.size(); ++i)
        RtcGetActorManager()->ReleaseSkin(m_skinList[i]);
    m_skinList.clear();

}

void CRT_ActorInstance::LoadSkinImpl(const char* name, bool bdisableMip)
{
    CRT_SkinInstance* _skin = RtcGetActorManager()->CreateSkin(name, this);
    if (_skin)
    {
        _skin->EnableMip(!bdisableMip);
        m_skinList.push_back(_skin);
    }
}

void CRT_ActorInstance::UnloadSkinImpl(const char* name)
{
    const char* real = GetSkinFileName(name);

    for (vector<CRT_SkinInstance*>::iterator i = m_skinList.begin(); 
        i != m_skinList.end(); ++i)
    {
        CRT_SkinInstance* _skin = (*i);

        if (!_skin) 
            continue;

        if (_skin->m_Name == real)
        {
            m_skinList.erase(i);
            RtcGetActorManager()->ReleaseSkin(_skin);
            return;
        }
    }
}

bool CRT_ActorInstance::ImportActor(const char* name)
{
    RtArchive *ar = RtCoreFile().CreateFileReader(name);

    if (!ar)
        return false;

    RtObject* o = ar->ReadObject(NULL);
    DEL_ONE(ar);

    if (!o || !o->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor)))
    {
        DEL_ONE(o);
        return false;
    }

    CRT_Actor* act = (CRT_Actor*)o;
    m_core->Merge2(act);

    for (size_t i = 0; i < act->m_eftList.size(); ++i)
    {
        CRT_Effect* p = act->m_eftList[i];

        ar = RtCoreFile().CreateFileWriter(NULL, "__temp.eft");
        if (!ar) 
            continue;
        ar->WriteObject(p);
        ar->Close();
        DEL_ONE(ar);
        ApplyEffect("__temp");
    }

    for (size_t i = 0; i < act->m_skinList.size(); ++i)
    {
        CRT_Skin* p = act->m_skinList[i];

        ar = RtCoreFile().CreateFileWriter(NULL, "__temp.ski");
       
        if (!ar) 
            continue;

        ar->WriteObject(p);
        ar->Close();
        DEL_ONE(ar);

        CRT_SkinInstance *skin = RtcGetActorManager()->CreateSkin("__temp", this, true);
        if (skin) 
            m_skinList.push_back(skin);
    }

    return true;
}

bool CRT_ActorInstance::ApplyEffect(const char* name)
{
    if (!name)
        return false;

    if (GetState() == Object_State_Ready)
        return ApplyEffectImpl(name);

    Rac_LoadSub* _cmd = CreateActorCommand<Rac_LoadSub>();
    _cmd->stype = Rac_LoadSub::_sub_efft;
    _cmd->otype = Rac_LoadSub::_load;
    _cmd->sname = name;
    m_listCommand.push_back(_cmd);	

    return true;
}

bool CRT_ActorInstance::RemoveEffect(const char* name)
{
    if (!name)
        return false;

    if (m_listCommand.empty())
        return RemoveEffectImpl(name);

    Rac_LoadSub* _cmd = CreateActorCommand<Rac_LoadSub>();
    _cmd->stype = Rac_LoadSub::_sub_efft;
    _cmd->otype = Rac_LoadSub::_unload;
    _cmd->sname = name;
    m_listCommand.push_back(_cmd);	

    return true;
}


bool CRT_ActorInstance::ApplyEffectImpl(const char* name)
{
    CRT_Effect* effect = RtcGetActorManager()->CreateEffect(name, this);

    if (effect)
    {
        m_effectList.push_back(effect);
        return true;
    }

    return false;
}

bool CRT_ActorInstance::RemoveEffectImpl(const char* name)
{
    const char *real = GetEffectFileName(name);

    for (vector<CRT_Effect*>::iterator i = m_effectList.begin(); 
        i != m_effectList.end(); ++i)
    {
        CRT_Effect* ef = (*i);

        if (!ef) 
            continue;

        if (ef->m_poTag == real)
        {
            m_effectList.erase(i);
            RtcGetActorManager()->ReleaseEffect(ef);
            return true;
        }
    }

    return false;
}

void CRT_ActorInstance::OnLoadFinish()
{
    RTASSERT(GetState() < Object_State_Creating);
    RTASSERT(m_skinList.empty());
    RTASSERT(m_core);

    m_bones = m_core->m_bones;
    m_bones[0].UseFrame(0.f, this);

    for (size_t i = 0; i < m_core->m_skinList.size(); ++i)
    {
        CRT_SkinInstance* skin = RT_NEW CRT_SkinInstance;
        skin->AttachActor(this);
        skin->SetCoreObject(m_core->m_skinList[i]);
        skin->OnLoadFinish();
        m_skinList.push_back(skin);
    }

    for (size_t i = 0; i < m_core->m_eftList.size(); ++i)
    {
        CRT_Effect* efft = (CRT_Effect*)m_core->m_eftList[i]->NewObject();
        efft->LinkActor(this);
        efft->OnLoadFinish();
        m_effectList.push_back(efft);
    }

    m_Notify.Notify(this, Actor_Load_Finish);
}

//lyymark CRT_ActorInstance 动画重置 
void CRT_ActorInstance::Reset()
{
    CoreUnlinkParentImpl();
    RegisterNotify(NULL);
    Unregister_ActorNotify();
    ClearComnandList();

    m_visible = 1.f;
    m_oldFrame = 0;
    m_animSpeed = 1.f;
    m_linkCount = 0;
    m_bDrawScale = false;
    m_lastFrame = -1;
    m_lastTickFrame = -1;
    m_bDisplay = true;
    m_bUpdate = true;
    m_bLoop = false;
    m_curFrame = 0;
    m_oldFrame = 0;

    m_curPose.Invalid();
    m_drawScale.Set(1.f, 1.f, 1.f);
    m_offset.Set(0.f, 0.f, 0.f);

    for (size_t i = 0; i < m_skinList.size(); ++i)
        if (m_skinList[i])
            m_skinList[i]->Reset();

    for (size_t i = 0; i < m_effectList.size(); ++i)
        if (m_effectList[i])
            m_effectList[i]->Reset();

    for (size_t i = 0; i < m_childs.size(); ++i)
        if (m_childs[i])
            m_childs[i]->Reset();

}

bool CRT_ActorInstance::Create()
{
    begin_log_tick(0);

    if (!m_core) 
        return false;

	if (!m_core->RequestCreate())
        return false;

	for (size_t i = 0; i < m_skinList.size(); ++i)
        m_skinList[i]->Create();

    for (size_t i = 0; i < m_effectList.size(); ++i)
        m_effectList[i]->Create();

   	UpdateBoundingBox();
    Update_WorldBoundBox();

    m_Notify.Notify(this, Actor_Create_Finish);

    end_log_tick2(0, "create atcor instance", m_Name.c_str());

    return true;
}

void CRT_ActorInstance::Destroy()
{
    RtcAcotrManager* _cMgr = RtcGetActorManager();

    CoreUnlinkParentImpl();

    for (size_t i = 0; i < m_skinList.size(); i++)
        _cMgr->ReleaseSkin(m_skinList[i]);
    m_skinList.clear();

    for (size_t i = 0; i < m_effectList.size(); i++)
        _cMgr->ReleaseEffect(m_effectList[i]);
    m_effectList.clear();

    for (size_t i = 0; i < m_childs.size(); ++i)
    {
        CRT_ActorInstance* _child = m_childs[i];
        _child->m_parent = NULL;
        _child->m_parentSlot.clear();
    }
    m_childs.clear();
}

void CRT_ActorInstance::SetDrawScale(RtgVertex3 scale)
{
    if (GetState() == Object_State_Ready)
        return SetDrawScaleImpl(scale);

    Rac_Setscale* _cmd = CreateActorCommand<Rac_Setscale>();
    _cmd->scale = scale;
    m_listCommand.push_back(_cmd);	

    return;
}

void CRT_ActorInstance::SetDrawScaleImpl(RtgVertex3 scale)
{
    m_bDrawScale = true;
    m_drawScale = scale;

    RtgMatrix16 mat;
    mat.Unit();
    mat._00 = scale.x;
    mat._11 = scale.y;
    mat._22 = scale.z;
    m_staticBoundingBox.vExt = m_core->m_boundBoxList[0].vExt * mat;
    m_staticBoundingBox.vPos = m_core->m_boundBoxList[0].vPos * mat;
}

void CRT_ActorInstance::Render(RtgDevice* dev, RTGRenderMask mask, bool bEffect, bool bRenderchild)
{
    DWORD trAcotr = rtMilliseconds();

    if (!m_bDisplay || !m_bGlobalRenderActor || GetState() != Object_State_Ready)
        return;

	RtgMatrix16 world;
	world.Set4X3Matrix(m_matrix);

	if (m_bDrawScale)
	{
		RtgMatrix16 scale;
		scale.Unit();
		scale._00 = m_drawScale.x;
		scale._11 = m_drawScale.y;
		scale._22 = m_drawScale.z;
		world = scale * world;
	}

	world._30 += m_offset.x;
	world._31 += m_offset.y;
	world._32 += m_offset.z;

	dev->SetMatrix(RTGTS_WORLD, &world);
	
    if (IsEnableLight())
		dev->SetRenderState(RTGRS_LIGHTING, TRUE);
	else
		dev->SetRenderState(RTGRS_LIGHTING, FALSE);

	if (!IsEnableZText())
		dev->SetRenderState(RTGRS_Z_TEST, FALSE);

    DWORD trSkin = rtMilliseconds();

	for (size_t i = 0; i < m_skinList.size(); ++i)
	{
		if (!m_skinList[i]) 
            continue;
        m_skinList[i]->Render(dev, mask);
	}

    RtGetPref()->skinRender += rtMilliseconds() - trSkin;

#ifdef CHAR_VIEWER
	RenderBone(dev, mask);
#endif

	if (!m_core->m_bZTest)
		dev->SetRenderState(RTGRS_Z_TEST, TRUE);

    DWORD trEfft = rtMilliseconds();

	if (mask != RTGRM_NOBLENDOBJ_ONLY)
	{
		if (m_bRenderEffect && bEffect)
		{
			for (size_t i = 0; i < m_effectList.size(); ++i)
			{
				if (!m_effectList[i]) 
                    continue;
				m_effectList[i]->Render(dev, mask);
			}
		}
	}

    RtGetPref()->efftRender += rtMilliseconds() - trEfft;
    RtGetPref()->ActrRender += rtMilliseconds() - trAcotr;

    for (size_t i = 0; i < m_childs.size(); ++i)
    {
        if (m_childs[i]->m_bRenderByParent || bRenderchild)
            m_childs[i]->Render(dev, mask, bEffect, bRenderchild);
    }

    RtgRenderOption* rp = RtgGetRenderOption();

    if (rp->bObjectNormal)
        RenderNormal();
}
  
void CRT_ActorInstance::RenderBone(RtgDevice* dev, RTGRenderMask mask)
{
    DWORD dwColor = 0x0;

    for(int i=0;i<m_bones.size();i++)
    {
        std::string sName = m_bones[i].Name;
        if (m_bones[i].isPaint || sName == m_strHighLightBone)
        {		
            if (sName == m_strHighLightBone)
                dwColor = 0xff0000ff;
            else
                dwColor = 0xffAAAAAA;
            RtgMatrix16 world;
            world.Set4X3Matrix(m_bones[i].skinMat);
            dev->SetMatrix(RTGTS_WORLD,&world);
            RtgVertex3 vMin = RtgVertex3(-2,-2,-2);
            RtgVertex3 vMax = RtgVertex3(2,2,2);
            RtGetRender()->DrawBox(vMin, vMax, &dwColor);
			dwColor = 0xffAA0000;
			RtGetRender()->DrawLine(RtgVertex3(0, 0, 0), RtgVertex3(10, 0, 0), &dwColor);
			dwColor = 0xff00AA00;
			RtGetRender()->DrawLine(RtgVertex3(0, 0, 0), RtgVertex3(0, 10, 0), &dwColor);
			dwColor = 0xff0000AA;
			RtGetRender()->DrawLine(RtgVertex3(0, 0, 0), RtgVertex3(0, 0, 10), &dwColor);
        }
    }
}

void CRT_ActorInstance::RenderNormal()
{
    for (size_t i = 0; i < m_skinList.size(); ++i)
        m_skinList[i]->RenderNormal();
}

void CRT_ActorInstance::RenderShadow(RtgDevice *dev , bool bRenderchild /* = true */, float fShadowFadeFactor /* = 0.5f */)
{
	if (GetState() != Object_State_Ready || 
        !m_bGlobalRenderShadow           || 
        !m_core->m_bDynamicShadow        || 
        !m_bEnableShadow) 
        return;

	RtgMatrix16 world;
	world.Set4X3Matrix(m_matrix);

	if (m_bDrawScale)
	{
		RtgMatrix16 scale;
		scale.Unit();
		scale._00 = m_drawScale.x;
		scale._11 = m_drawScale.y;
		scale._22 = m_drawScale.z;
		world = scale * world;
	}
	dev->SetMatrix(RTGTS_WORLD, &world);

	for (size_t i = 0; i < m_skinList.size(); ++i)
	{
		if (m_skinList[i]->IsEnableDynShadow())
			m_skinList[i]->RenderShadow();
	}

    if (bRenderchild)
        for (size_t i = 0; i < m_childs.size(); ++i)
            m_childs[i]->RenderShadow(dev, bRenderchild, fShadowFadeFactor);
}

void CRT_ActorInstance::RenderFrame(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale)
{
    if (GetState() != Object_State_Ready)
        return;

	RtgMatrix16 world;
	world.Set4X3Matrix(m_matrix);

    if (m_bDrawScale)
	{
		RtgMatrix16 scale1;
		scale1.Unit();
		scale1._00 = m_drawScale.x;
		scale1._11 = m_drawScale.y;
		scale1._22 = m_drawScale.z;
		world = scale1 * world;
	}

	RtgMatrix16 t;
	t.Unit();
	t.Translate(-GetBoundingBox()->vPos.x,-GetBoundingBox()->vPos.y,-GetBoundingBox()->vPos.z);
	t.Scale(scale.x,scale.y,scale.z);
	t.Translate(GetBoundingBox()->vPos.x,GetBoundingBox()->vPos.y,GetBoundingBox()->vPos.z);
	world = t * world;
	dev->SetMatrix(RTGTS_WORLD,&world);
	
	for (size_t i = 0; i < m_skinList.size(); ++i)
		m_skinList[i]->RenderFrame(color);

}

void CRT_ActorInstance::RenderHighLight(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale)
{
    if (GetState() != Object_State_Ready)
        return;

	RtgMatrix16 world;
	world.Set4X3Matrix(m_matrix);

	if (m_bDrawScale)
	{
		RtgMatrix16 scale1;
		scale1.Unit();
		scale1._00 = m_drawScale.x;
		scale1._11 = m_drawScale.y;
		scale1._22 = m_drawScale.z;
		world = scale1 * world;
	}

	RtgMatrix16 t;
	t.Unit();

	t._30 -= GetBoundingBox()->vPos.x;
	t._31 -= GetBoundingBox()->vPos.y;
	t._32 -= GetBoundingBox()->vPos.z;
	t.Scale(scale.x,scale.y,scale.z);
	t._30 += GetBoundingBox()->vPos.x;
	t._31 += GetBoundingBox()->vPos.y;
	t._32 += GetBoundingBox()->vPos.z;

	world = t * world;
	dev->SetMatrix(RTGTS_WORLD,&world);

	for (size_t i = 0; i < m_skinList.size(); ++i)
		m_skinList[i]->RenderHLight();
}

bool CRT_ActorInstance::ComputeVC(RtgMatrix16& world, vcArray_t& aryVc, RtgVertex3 vSkyLight, RtgLightItem* pLights)
{
    if (!IsEnableVC())
        return false;

    int vcNum = 0;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        vcNum += m_skinList[i]->Get_VertexArray()->size();
    }

    aryVc.resize(vcNum);

    int offset = 0;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        m_skinList[i]->ComputeVC(world, aryVc, offset, vSkyLight, pLights);
        offset += (int)m_skinList[i]->Get_VertexArray()->size();
    }

	return true;
}

bool CRT_ActorInstance::SetVC(vcArray_t& aryVc)
{
    if (GetState() == Object_State_Ready)
        return SetVCImpl(&aryVc);

    Rac_Setvc* _cmd = CreateActorCommand<Rac_Setvc>();
    _cmd->_vc = &aryVc;
    m_listCommand.push_back(_cmd);	

	return true;
}

void CRT_ActorInstance::SetBaseColor(RtgVertex3 &color, bool bSetchild /* = false */)
{
    if (GetState() == Object_State_Ready)
        return SetBaseColorImpl(color, bSetchild);

    Rac_Setbasecolor* _cmd = CreateActorCommand<Rac_Setbasecolor>();
    _cmd->color = color;
    _cmd->bchild = bSetchild;
    m_listCommand.push_back(_cmd);	

    return;
}

bool CRT_ActorInstance::UpdateBone(float frame)
{
    if (!m_core)
        return false;

    if (m_core->m_frameNum == 1 && frame == 0)
        return false;

    if (m_bones.empty())
        return false;

    if (frame < 0 || frame >= m_core->m_frameNum)
        return false;

    m_bones[0].UseFrame(frame, this);

    return true;

}

//lyymark RealUseFrame
void CRT_ActorInstance::RealUseFrame(float frame)
{
    long _nframe = (long)frame;

    if (_nframe == m_lastFrame)
        return;
    m_lastFrame = _nframe;

	if (UpdateBone(_nframe)) 
    {
	    if (m_bDrawScale)
	    {
		    RtgMatrix16 mat;
		    mat.Unit();
		    mat._00 = m_drawScale.x;
		    mat._11 = m_drawScale.y;
		    mat._22 = m_drawScale.z;
		    m_boundingBox.vExt = m_core->m_boundBoxList[frame].vExt * mat;
		    m_boundingBox.vPos = m_core->m_boundBoxList[frame].vPos * mat;
		    if(m_boundingBox.vExt.x < BOUND_MIN)
			    m_boundingBox.vExt.x = BOUND_MIN;
		    if(m_boundingBox.vExt.y < BOUND_MIN)
			    m_boundingBox.vExt.y = BOUND_MIN;
		    if(m_boundingBox.vExt.z < BOUND_MIN)
			    m_boundingBox.vExt.z = BOUND_MIN;
	    }
	    else
	    {
		    m_boundingBox = m_core->m_boundBoxList[frame];
	    }
        
	    m_curFrame = frame;
        DWORD ruSkin = rtMilliseconds();
	    for (int i = 0; i < m_skinList.size(); i++)
            //更新网格 网格就是皮肤mesh
		    m_skinList[i]->UseFrame(frame);

        for (size_t i = 0; i < m_childs.size(); ++i)
        {
            if (m_childs[i]->m_parent)
                m_childs[i]->UpdateMatrix();
        }

        RtGetPref()->skinUpdate += rtMilliseconds() - ruSkin;
    }
    //lyymark 疑似标准材质渲染重复了 推测是类型识别不正确，无法判断子类类型，后续出问题再修改
    /*for (int i = 0; i < m_skinList.size(); i++)
        if (m_skinList[i]->ResourceReady())
            m_skinList[i]->GetMaterialLib()->UseFrame(frame);*/

    DWORD ruEfft = rtMilliseconds();
	for (int i = 0; i < m_effectList.size(); i++)
		if(m_effectList[i]->RequestUseFrame()) 
            m_effectList[i]->UseFrame(frame);
    RtGetPref()->efftUpdate += rtMilliseconds() - ruEfft;

}

void CRT_ActorInstance::SetVisible(float visible)
{
    if (GetState() == Object_State_Ready)
        return SetVisibleImpl(visible);

    Rac_Setvisible* _cmd = CreateActorCommand<Rac_Setvisible>();
    _cmd->_visible = visible;
    m_listCommand.push_back(_cmd);	
}

void CRT_ActorInstance::SetVisibleImpl(float visible)
{
    m_visible = visible;
    SetSkinVisible(visible);
}

void CRT_ActorInstance::SetSkinVisible(float visible)
{
    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        m_skinList[i]->SetVisible(visible);
    }
}

void CRT_ActorInstance::SetBaseColorImpl(RtgVertex3 &color, bool bSetchild)
{
    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        if (!m_skinList[i]) 
            continue;
        m_skinList[i]->SetBaseColor(color);
    }

    if (bSetchild)
    {
        for (size_t i = 0; i < m_childs.size(); ++i)
            m_childs[i]->SetBaseColor(color, bSetchild);
    }
}

void CRT_ActorInstance::ChangeMaterial(size_t _skinIndex, const char _mtlName[])
{
    if (!_mtlName || !(*_mtlName))
        return;

    if (GetState() == Object_State_Ready)
        return ChangeMaterialImpl(_skinIndex, _mtlName);

    Rac_Setmtl* _cmd = CreateActorCommand<Rac_Setmtl>();
    _cmd->_skinIndex = _skinIndex;
    _cmd->mtlname = _mtlName;
    m_listCommand.push_back(_cmd);	
}

void CRT_ActorInstance::ResetRibbonEffect()
{
    for (size_t i = 0; i < m_effectList.size(); ++i)
        m_effectList[i]->ResetRibbonDynamic();
}

void CRT_ActorInstance::ChangeMaterialImpl(size_t _skinIndex, const char _mtlName[])
{
    if (_skinIndex >= m_skinList.size())
        return;
    m_skinList[_skinIndex]->ChangeMaterial(_mtlName);
}

RtgAABB *CRT_ActorInstance::UpdateBoundingBox()
{
	RtgRangeAABB tmp;
	float min[3];
    float max[3];
	
    min[0] = -10;
	min[1] = -10;
	min[2] = 0;
	max[0] = 10;
	max[1] = 10;
	max[2] = 40;

	tmp.vMin.Set(min);
	tmp.vMax.Set(max);

#ifdef CHAR_VIEWER
	tmp.vMin.Set(-10.f);
	tmp.vMax.Set(10.f);
#endif

	m_boundingBox = m_core->m_boundBoxList[0];

	if(m_boundingBox.vExt.x < BOUND_MIN)
		m_boundingBox.vExt.x = BOUND_MIN;
	if(m_boundingBox.vExt.y < BOUND_MIN)
		m_boundingBox.vExt.y = BOUND_MIN;
	if(m_boundingBox.vExt.z < BOUND_MIN)
		m_boundingBox.vExt.z = BOUND_MIN;

	m_staticBoundingBox = m_core->m_boundBoxList[0];

	if(m_staticBoundingBox.vExt.x < BOUND_MIN)
		m_staticBoundingBox.vExt.x = BOUND_MIN;
	if(m_staticBoundingBox.vExt.y < BOUND_MIN)
		m_staticBoundingBox.vExt.y = BOUND_MIN;
	if(m_staticBoundingBox.vExt.z < BOUND_MIN)
		m_staticBoundingBox.vExt.z = BOUND_MIN;

	return &m_boundingBox;
}

bool CRT_ActorInstance::Save(RtArchive* ar)
{
    bool ret = true;

    vector<CRT_Skin*> tmpSkinList = m_core->m_skinList;
    vector<CRT_Effect*> tmpEftList  = m_core->m_eftList;
    vector<CRT_MaterialLib*> tmpMtlLib; 

    m_core->m_skinList.clear();
    m_core->m_eftList = m_effectList;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {
        m_core->m_skinList.push_back(m_skinList[i]->GetCore());
        tmpMtlLib.push_back(GetCore()->m_skinList[i]->m_mtlLib);
        m_core->m_skinList[i]->m_mtlLib = m_skinList[i]->GetMaterialLib();
    }

    RtArchive* local = ar;
    
    if (!local)
        local = RtCoreFile().CreateFileWriter(NULL, m_core->m_diskFile.c_str());
    
    if (!local) 
        return false;

    ret = local->WriteObject(m_core);

    if (!ar)
        DEL_ONE(local);

    for (size_t i = 0; i < m_skinList.size(); ++i)
        m_core->m_skinList[i]->m_mtlLib = tmpMtlLib[i];
    
    m_core->m_skinList = tmpSkinList;
    m_core->m_eftList  = tmpEftList;
    
    return ret;
}

bool CRT_ActorInstance::IsRegIntersect(const RtgVertex3& reg0, const RtgVertex3& reg1)
{
    if (GetState() != Object_State_Ready)
        return false;

    for (size_t i = 0; i < m_skinList.size(); ++i)
    {        
        CRT_SkinInstance* sk = m_skinList[i];
        verArray_t& vv = *sk->Get_VertexArray();

        for (size_t i = 0; i < sk->GetCore()->m_rsList.size(); ++i)
        {
            RtcSkinSubset* rs = &sk->GetCore()->m_rsList[i];
            RtgVertex3 vi;

            for (size_t t = 0; t < rs->faceList.size(); t += 3)
            {
                if (rtgRegTriIntersect(reg0, reg1, 
                    vv[rs->faceList[t]].pos,
                    vv[rs->faceList[t + 1]].pos,
                    vv[rs->faceList[t + 2]].pos,
                    &vi))
                    return true;
            }
        }
    }

    return false;
}

#ifdef CHAR_VIEWER

void CRT_ActorInstance::AttachPoseGrid(void *p)
{
    if(!GetPoseMap()) return;

    CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
    CXTPPropertyGridItem *pItem;

    CString tmp;
    TPoseMap::iterator it;
    pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("Add Pose"));
    pItem->SetFlags(xtpGridItemHasEdit|xtpGridItemHasExpandButton);

    struct _poseinfo
    {
        SRT_Pose& pose;

        _poseinfo(SRT_Pose& p)
            : pose(p)
        {
        }

        bool operator <(const _poseinfo& pi) const
        {
            return pose.Name < pi.pose.Name;
        }

    };

    set<_poseinfo> set_pose;
    for(it=GetPoseMap()->begin(); it!=GetPoseMap()->end(); it++)
    {
        SRT_Pose *pose = &(*it).second;
        set_pose.insert(*pose);
    }

    for (set<_poseinfo>::iterator i = set_pose.begin(); i != set_pose.end(); ++i)
    {
        const  _poseinfo& pose = *i;

        pItem = pRoot->AddChildItem(new CCustomItemString("Pose",&pose.pose.Name));
        pItem->SetFlags(xtpGridItemHasExpandButton);
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("Start",pose.pose.StartFrm,&pose.pose.StartFrm));
        pItem->AddChildItem(new CXTPPropertyGridItemNumber("End",pose.pose.EndFrm,&pose.pose.EndFrm));
        tmp.Format("%d",pose.pose.EventList.size());
        pItem = pItem->AddChildItem(new CXTPPropertyGridItem("Event",tmp));
        pItem->SetFlags(xtpGridItemHasExpandButton);
        pItem->Expand();
    }
    pRoot->Expand();
}

void CRT_ActorInstance::AttachLinkBoxGrid(void *p)
{
	if (!m_core)
		return;

	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem;

	CString tmp;
	std::vector<SRT_Bone>::iterator it;
	for(it=m_core->m_bones.begin(); it!=m_core->m_bones.end(); it++)
	{
		std::string name = it->Name;
		pItem = pRoot->AddChildItem(new CCustomItemString("LinkPoint",&name));
		pItem->SetFlags(xtpGridItemHasExpandButton);
		pItem->Expand();
	}
	pRoot->Expand();
}

bool CRT_ActorInstance::OnPoseGridNotify(WPARAM wParam, LPARAM lParam)
{
	if(!GetPoseMap()) return false;

	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	CString tmp;
	switch(wParam)
	{
		case XTP_PGN_ITEMVALUE_CHANGED:
			break;

		case XTP_PGN_INPLACEBUTTONDOWN:
			if(pItem->GetCaption() == "Add Pose")
			{
				tmp = pItem->GetValue();
				if(tmp.GetLength() == 0) return false;
				SRT_Pose pose;
				pose.StartFrm = 0;
				pose.EndFrm = 0;
				pose.Name = tmp;
				// check if pose exist
				if(GetPose(tmp)) return false;
				(*GetPoseMap())[pose.Name] = pose;
				return true;	
			}
			else if(pItem->GetCaption() == "Pose")
			{
				tmp = pItem->GetValue();
				if(tmp.GetLength() == 0) return false;
				if(MessageBox(NULL,"确实要删除吗？","提示",MB_OKCANCEL)!=IDOK)
					return false;
				GetPoseMap()->erase((const char*)tmp);
				return true;
			}
			else if(pItem->GetCaption() == "Event")
			{
				if(!pItem->GetParentItem()) return false;
				SRT_Pose *pose = GetPose(pItem->GetParentItem()->GetValue());
                if(!pose) return false;
				CDlgPose dlg;
				dlg.m_pose = pose; 
                dlg.m_act = this;
				dlg.DoModal();
				CString tmp;
				tmp.Format("%d",pose->EventList.size());
				pItem->SetValue(tmp);

                if (m_curPose.IsVaild())
                {
                    m_curPose.UnActive();
                    m_curPose.Active(this);
                }

				return false;
			}
			break;

		case XTP_PGN_SELECTION_CHANGED:
			// pItem->GetCaption() == "Pose";
			break;
	}
	return false;
}

bool CRT_ActorInstance::OnLinkBoxGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	CString tmp;
	switch(wParam)
	{
	case XTP_PGN_DBLCLICK:
		if(pItem->GetCaption() == "LinkPoint")
		{
			tmp = pItem->GetValue();
			if(tmp.GetLength() == 0) return false;

            char* t = tmp.GetBuffer();
            if (m_strHighLightBone == string(t))
                m_strHighLightBone = "";
            else
                m_strHighLightBone = t;
            tmp.ReleaseBuffer();

			std::vector<SRT_Bone>::iterator it;
			for(it=m_core->m_bones.begin(); it!=m_core->m_bones.end(); it++)
			{
				std::string name = it->Name;
				if (!strcmp((const char*)tmp,name.c_str()))
				{
					it->isPaint = 1;
					break;
				}
			}
		}
		break;
	case XTP_PGN_RCLICK:
		if(pItem->GetCaption() == "LinkPoint")
		{
			tmp = pItem->GetValue();
			if(tmp.GetLength() == 0) return false;
			std::vector<SRT_Bone>::iterator it;
			for(it=m_core->m_bones.begin(); it!=m_core->m_bones.end(); it++)
			{
				std::string name = it->Name;
				if (!strcmp((const char*)tmp,name.c_str()))
				{
					it->isPaint = 0;
					break;
				}
			}
		}
		break;
	}
	return false;
}

string CRT_ActorInstance::GetSelectedPose(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;
	if(pItem->GetCaption() == "Pose")
		return (const char*)pItem->GetValue();

	return "";
}

CRT_Effect *CRT_ActorInstance::GetSelectedEffect(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;

	if(pItem->GetCaption() == "Effect Index")
	{
		// get effect idx
		int idx = atol(pItem->GetValue());
		if(idx < 0 || idx >= m_effectList.size()) return NULL;
		return m_effectList[idx];
	}
	return NULL;
}

// attach to actor info grid
void CRT_ActorInstance::AttachGrid(void *p)
{	
	CXTPPropertyGridItem *pRoot = (CXTPPropertyGridItem*)p;
	CXTPPropertyGridItem *pItem,*pItemSkin,*pItemList,*pSubItem;
	// CXTPPropertyGridItemConstraints  *pList;

	int i,j;
	CString tmp;
	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("基本属性"));
	if (m_core)
	{
		if(m_core->m_animType==ACTOR_ANIM_VA)
			pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("动画类型","顶点动画"));
		else if(m_core->m_animType==ACTOR_ANIM_STATIC)
			pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("动画类型","静态"));
		else if(m_core->m_animType==ACTOR_ANIM_SKEL)
			pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("动画类型","骨骼动画"));
		else 
			pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("动画类型","错误"));
	}
	else
	{
		pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("动画类型","静态无动画"));
	}
	pSubItem->SetReadOnly(TRUE);

	if (m_core)
	{
		pSubItem = pItem->AddChildItem(new CCustomItemBool("Enable Lighting",&m_core->m_bUseLight));
		pSubItem->SetDescription("是否使用光照。");
		pSubItem = pItem->AddChildItem(new CCustomItemBool("Enable VC",&m_core->m_bUseVC));
		pSubItem->SetDescription("是否使用顶点色。");
		pSubItem = pItem->AddChildItem(new CCustomItemBool("Enable ZTest",&m_core->m_bZTest));
		pSubItem->SetDescription("是否不做深度测试(false=始终在最前)。");
		pSubItem = pItem->AddChildItem(new CCustomItemBool("Enable Dynamic Shadow",&m_core->m_bDynamicShadow));
		pSubItem->SetDescription("是否使用动态阴影。");
	}
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemV3("缩放(不存盘!)",&m_drawScale));
	pSubItem = pItem->AddChildItem(RT_NEW CCustomItemV3("偏移(不存盘!)",&m_offset));
	pItem->Expand();

	pItem = pRoot->AddChildItem(new CXTPPropertyGridItem("操作"));
	pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("Import Skin"));
	pSubItem->SetDescription("从已有文件中导入一个Skin。");
	pSubItem->SetFlags(xtpGridItemHasExpandButton);

	pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("Import Effect"));
	pSubItem->SetDescription("从已有文件中导入一个Effect。");
	pSubItem->SetFlags(xtpGridItemHasExpandButton);

    pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("Import Actor"));
    pSubItem->SetDescription("从已有文件中导入一个Actor。");
    pSubItem->SetFlags(xtpGridItemHasExpandButton);

	pSubItem = pItem->AddChildItem(new CXTPPropertyGridItem("New Effect"));
	pSubItem->SetDescription("新建一个Effect");
	pSubItem->SetFlags(xtpGridItemHasExpandButton);
	pItem->Expand();

	for(i=0; i<m_skinList.size(); i++)
	{
		CRT_SkinInstance *skin;
		skin = m_skinList[i];

		tmp.Format("%d",i);
		pItemSkin = pRoot->AddChildItem(new CXTPPropertyGridItem("Skin Index",tmp));
		pItemSkin->SetFlags(xtpGridItemHasExpandButton);    
		pItem = pItemSkin->AddChildItem(new CCustomItemString("Skin Name",&skin->GetCore()->m_name));
		pItem->SetReadOnly(TRUE);
		pItemSkin->AddChildItem(new CCustomItemBool("阴影",&skin->GetCore()->m_bDynShadow));

		pItem = pItemSkin->AddChildItem(new CXTPPropertyGridItem("Change Mtl Lib"));
        pItem->SetFlags(xtpGridItemHasExpandButton);
		// pItem = pItemSkin->AddChildItem(new CCustomItemBool("Board",&skin->m_core->m_bBoard));
        for(j=0; j<skin->GetCore()->m_rsList.size(); j++)
		{
			// rs list
			RtcSkinSubset *rs = &skin->GetCore()->m_rsList[j];
			tmp.Format("%d",j);
			// pItem = pItemList->AddChildItem(new CXTPPropertyGridItem("Render Surface Index",tmp));
			// pItem->SetReadOnly(TRUE);
			// pItem = pItemList->AddChildItem(new CCustomItemString("Base Texture",&rs->Texture));
			pItem = pItemSkin->AddChildItem(new CCustomItemString("Material",&rs->Material));
            if(!skin->GetMaterialLib()->GetMaterial(rs->Material.c_str()))
            {
				pItem = pItemSkin->AddChildItem(new CXTPPropertyGridItem("state","Not Found"));
				pItem->SetReadOnly(TRUE);
			}
			else 
            {
				pItem = pItemSkin->AddChildItem(new CXTPPropertyGridItem("state","Active"));
				pItem->SetReadOnly(TRUE);
			}
		}
		// pItemSkin->Expand();
	}

	// effect list
	CXTPPropertyGridItem *pItemEffect;
	for(i=0; i<m_effectList.size(); i++)
	{
		CRT_Effect *effect;
		effect = m_effectList[i];

		tmp.Format("%d",i);
		pItemEffect = pRoot->AddChildItem(new CXTPPropertyGridItem("Effect Index",tmp));
		pItemEffect->SetFlags(xtpGridItemHasExpandButton);
		tmp.Format("%s",effect->GetName());
		pItemEffect->AddChildItem(new CXTPPropertyGridItem("Name",tmp));
	}
	pRoot->Expand();
}

bool CRT_ActorInstance::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
    std::cout << std::to_string(wParam) << std::endl;
	switch(wParam)
	{
		case 2:
			if(pItem->GetCaption()=="Material")
			{
				for(int i=0; i<m_skinList.size(); i++)
				{
					m_skinList[i]->OnMaterialChange();
				}
				// return true;
			}
			else if(pItem->GetCaption() == "Name")
			{
				CRT_Effect *eft = m_effectList[atol(pItem->GetParentItem()->GetValue())];
				std::string tmp = pItem->GetValue();
				if(eft) eft->SetName(tmp.c_str());
			}
			break;

		case 8:
			if(pItem->GetCaption() == "Import Skin")
			{
				char filter[] = "Skin Files (*.ski)|*.ski||";
				char bak[255];

				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();

				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;
				
				// LoadSkin(file);
				CRT_SkinInstance *skin = RtcGetActorManager()->CreateSkin(file, this, true);
				if(!skin) 
                    return false;
				m_skinList.push_back(skin);
				// CRT_Skin *tmp = skin->m_core->NewObject();
				// m_core->m_skinList.push_back(tmp);
				return true;
			}
			else if(pItem->GetCaption() == "Change Mtl Lib")
			{
				char filter[] = "MtlLib Files (*.mtl)|*.mtl||";
				char bak[255];

				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();

				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;

				// replace old lib
				m_skinList[atol(pItem->GetParentItem()->GetValue())]->ChangeMaterial(file);
				return true;
			}
			else if(pItem->GetCaption() == "New Effect")
			{
				CDlgCreateEffect dlg;
				dlg.DoModal();
				return true;
			}
			else if(pItem->GetCaption() == "Import Effect")
			{
				char filter[] = "Effect Files (*.eft)|*.eft||";
				char bak[255];
				
				// save path
				GetCurrentDirectory(255,bak); 
				CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
				if(dlg.DoModal() != IDOK)
				{
					// restore path
					SetCurrentDirectory(bak);
					return false;
				}	
				CString file = dlg.GetFileName();

				// restore path
				SetCurrentDirectory(bak);
				if(file.GetLength() <=0 ) return false;
				ApplyEffect(file);
				return true;
			}
            else if(pItem->GetCaption() == "Import Actor")
            {
                char filter[] = "Actor Files (*.act)|*.act||";
                char bak[255];

                // save path
                GetCurrentDirectory(255,bak); 
                CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
                if(dlg.DoModal() != IDOK)
                {
                    // restore path
                    SetCurrentDirectory(bak);
                    return false;
                }	
                CString file = dlg.GetPathName();

                // restore path
                SetCurrentDirectory(bak);
                if(file.GetLength() <=0 ) return false;
                ImportActor(file);
                return true;
            }
			else if(pItem->GetCaption() == "Skin Index")
			{
				if(MessageBox(NULL,"确实要删除吗？","提示",MB_OKCANCEL)!=IDOK)
					return false;

				// del skin
				int skin = atol(pItem->GetValue());
				if(skin < 0 || skin >= m_skinList.size()) return false;

				// delete it
				vector<CRT_SkinInstance*>::iterator it1 = m_skinList.begin();
				vector<CRT_Skin*>::iterator it2 = m_core->m_skinList.begin();
				for(int i=0;i!=skin;i++)
				{
					it1++;
					//it2++;
				}
				m_skinList.erase(it1);
				// m_core->m_skinList.erase(it2);
				return true;
			}
			else if(pItem->GetCaption() == "Effect Index")
			{
				if(MessageBox(NULL,"确实要删除吗？","提示",MB_OKCANCEL)!=IDOK)
					return false;

				// del effect
				int eft = atol(pItem->GetValue());
				if(eft < 0 || eft >= m_effectList.size()) return false;

				// delete it
				vector<CRT_Effect*>::iterator it1 = m_effectList.begin();
				// vector<CRT_Effect*>::iterator it2 = m_core->m_eftList.begin();
				for(int i=0;i!=eft;i++)
				{
					it1++;
					// it2++;
				}
				m_effectList.erase(it1);
				// m_core->m_eftList.erase(it2);
				return true;
			}
			break;
	}
	return false;
}

CRT_Material *CRT_ActorInstance::GetSelectedMaterial(void *p)
{
    return NULL;
}

CRT_Skin *CRT_ActorInstance::GetSelectedSkin(void *p)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)p;

	if(pItem->GetCaption() == "Skin Index")
	{
		// get skin idx
		int skin = atol(pItem->GetValue());
		if(skin < 0 || skin >= m_skinList.size()) return NULL;
		return m_skinList[skin]->GetCore();
	}
	return NULL;
}

bool CRT_ActorInstance::UpdateSkin(RtgDevice *dev)
{
	return true;
}

bool CRT_ActorInstance::UpdateEffect(RtgDevice *dev)
{
	return true;
}

#endif

CRT_SkinInstance::CRT_SkinInstance()
    : m_core(NULL),
      m_actor(NULL),
      m_mtlLib(NULL),
      m_bHide(false),
      m_aryVc(NULL),
      m_vcOffset(0),
      m_visible(1.f),
      m_fadeOut(1.f),
      m_frameVisible(1.f),
      m_oldFrameVisible(1.f),
      m_updateNor(false),
      m_bEnableMip(true),
      m_pvc(NULL)
{
}

CRT_SkinInstance::~CRT_SkinInstance()
{
    RtcGetActorManager()->ReleaseMtl(m_mtlLib);
    RtgReleaseBuffer(m_pvc);
}

void CRT_SkinInstance::ChangeMaterial(const char* name)
{
    if (GetState() == Object_State_Ready)
        return ChangeMaterialImpl(name);

    Rac_Setmtl* _cmd = CreateActorCommand<Rac_Setmtl>();
    if (name)
        _cmd->mtlname = name;
    m_listCommand.push_back(_cmd);	
}

void CRT_SkinInstance::ChangeMaterialImpl(const char* name)
{
    CRT_MaterialLib* _mtl = RtcGetActorManager()->CreateMtl(name, this);

    if (!_mtl)
        return;

    RtcGetActorManager()->ReleaseMtl(m_mtlLib);
    m_mtlLib = _mtl;
}

void CRT_SkinInstance::OnMaterialChange()
{
    m_updateNor = GetMaterialLib()->RequestUpdateNormal();
}

void CRT_SkinInstance::SetCoreObject(CRT_Skin* core)
{
    if (m_core == core)
        return;

    RtcGetActorManager()->ReleasePoolObject(m_core);
    RtcGetActorManager()->ReleaseMtl(m_mtlLib);

    if (!(m_core = core))
        return;
    ++m_core->m_poRef;
}

void CRT_SkinInstance::OnLoadFinish()
{
    RTASSERT(m_core);
    m_core->OnCreateInstance(this);
}

bool CRT_SkinInstance::Create()
{
    m_mtlLib = (CRT_MaterialLib*)m_core->m_mtlLib->NewObject();
    OnMaterialChange();
    m_fadeOut = 0.f;

    return true;
}

void CRT_SkinInstance::Reset()
{
    m_visible = 1.f;
    m_frameVisible = 1.f;
    m_oldFrameVisible = 1.f;
    m_fadeOut = 1.f;

    if (m_mtlLib)
        m_mtlLib->Reset();
}

void CRT_SkinInstance::Render(RtgDevice* _Dev, RTGRenderMask _Mask)
{
    if (GetState() != Object_State_Ready || !m_mtlLib || m_mtlLib->GetState() != Object_State_Ready)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RtgMatrix16 oldMat;

    RTASSERT(dev);
    RTASSERT(m_core);
    RTASSERT(m_core->m_mtlLib);

    RTGTextureFilter mipf = dev->GetTextureFilterMip();

    if (!m_bEnableMip)
        dev->SetTextureFilterMip(RTGTEXF_NONE);

    DWORD _Offset = 0;
    DWORD _Size = 0;

    RtgBufferItem* vb = m_core->RequestVB(this, &_Offset, &_Size);
    RtgBufferItem* ib = NULL;

    if (!vb)
        return;

    if (m_core->m_bBoard)
    {
        RtgMatrix16 boardMat;
        RtgVertex3 pos;

        dev->GetMatrix(RTGTS_WORLD, &oldMat);
        boardMat.Unit();
        boardMat = dev->m_pCamera->m_matBillboard;
        m_core->GetBoardPos(pos);

        pos = pos * oldMat;
        boardMat._30 = pos.x;
        boardMat._31 = pos.y;
        boardMat._32 = pos.z;

        dev->SetMatrix(RTGTS_WORLD, &boardMat);
    }

    dev->SetStream_vb(0, vb, m_core->vertexSize(), _Offset, m_core->vertexNum());

    if (m_pvc)
    {
        dev->SetStream_vb(1, m_pvc, sizeof(DWORD), 0, m_core->vertexNum());
        dev->SetVertexDecl_vnct();
        dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_COLOR1);
    }
    else
    {
        dev->SetVertexDecl_vnt();
        dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    }

    for (size_t i = 0; i < m_core->m_rsList.size(); ++i)
    {
        CRT_Material* mtl = m_mtlLib->GetMaterial(m_core->m_rsList[i]._Id);

        if (!mtl || !mtl->ResourceReady())
            continue;

        if (_Mask == RTGRM_ALL || (mtl->IsTransparent() ^ (_Mask == RTGRM_NOBLENDOBJ_ONLY)))
        {
            ib = m_core->m_rsList[i].RequestIB();

            if (!ib)
                continue;

            if (mtl->Apply(dev))
            {
                dev->SetStream_ib(ib);
                dev->DrawIndexedPrimitive(
                    D3DPT_TRIANGLELIST,
                    0,
                    0,
                    m_core->vertexNum(),
                    0,
                    m_core->m_rsList[i].faceList.size() / 3
                    );
                mtl->Restore(dev);
            }
        }
    }

    if (m_pvc)
        dev->DisableStreamSource(1);

    if (m_core->m_bBoard)
        dev->SetMatrix(RTGTS_WORLD, &oldMat);

    dev->SetTextureFilterMip(mipf);
}

void CRT_SkinInstance::RenderShadow()
{
    if (GetState() != Object_State_Ready)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RtgMatrix16 oldMat;

    RTASSERT(dev);
    RTASSERT(m_core);
    RTASSERT(m_core->m_mtlLib);

    DWORD _Offset = 0;
    DWORD _Size = 0;

    RtgBufferItem* vb = m_core->RequestVB(this, &_Offset, &_Size);
    RtgBufferItem* ib = NULL;

    if (!vb)
        return;

    size_t mtlNum = min(m_mtlLib->GetMtlNum(), m_core->m_rsList.size());

    dev->SetStream_vb(0, vb, m_core->vertexSize(), _Offset, m_core->vertexNum());
    dev->SetVertexDecl_vnt();
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);

    for (size_t i = 0; i < mtlNum; ++i)
    {
        CRT_Material* mtl = m_mtlLib->GetMaterial(i); 
        RTASSERT(mtl);

        if (!mtl->ResourceReady() || mtl->IsTransparent())
            continue;

        ib = m_core->m_rsList[i].RequestIB();

        if (!ib)
            continue;

        RtgShader _shader;

        switch (mtl->GetAlphaMdoe())
        {
        case ALPHA_NULL :
            _shader.iNumStage = 1;
            _shader.dwTextureFactor = RtgVectorToColor(RtgVertex3(0.5f, 0.5f, 0.5f), 1.f);
            _shader.Shaders[0].SetTexture(0);
            _shader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
            _shader.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
            _shader.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
        case ALPHA_COLOR_KEY :
            _shader.Shaders[0].SetTexture(mtl->GetBaseText());
            _shader.bColorKey = true;
            break;

        case ALPHA_SUB :
        case ALPHA_ADD :
            continue;
            /*
            _shader.iNumStage = 1;
            _shader.dwTextureFactor = RtgVectorToColor(RtgVertex3(0.5f, 0.5f, 0.5f), 1.f);
            _shader.Shaders[0].SetTexture(mtl->GetBaseText());
            _shader.Shaders[0].eColorOp   = RTGTOP_MODULATE;
            _shader.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
            _shader.Shaders[0].eColorArg2 = RTGTA_TEXTURE;
            _shader.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
            _shader.bEnableBlendFunc = true;
            _shader.eBlendSrcFunc = RTGBLEND_ONE;
            _shader.eBlendDstFunc = RTGBLEND_ONE;
            */
            break;

        case ALPHA_BLEND :
            _shader.iNumStage = 1;
            _shader.dwTextureFactor = RtgVectorToColor(RtgVertex3(0.5f, 0.5f, 0.5f), 1.f);
            _shader.Shaders[0].SetTexture(mtl->GetBaseText());
            _shader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
            _shader.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
            _shader.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
            _shader.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
            _shader.bEnableBlendFunc = true;
            _shader.eBlendSrcFunc = RTGBLEND_SRCALPHA;
            _shader.eBlendDstFunc = RTGBLEND_ONE;
            break;
        }

        dev->SetShader(&_shader);
        dev->SetStream_ib(ib);
        dev->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            0,
            0,
            m_core->vertexNum(),
            0,
            m_core->m_rsList[i].faceList.size() / 3
            );
        dev->RestoreShader(&_shader);
    }
}

#define COLOR_EXCHANGE(x) (((x&0x000000ff)<<16)|(x&0x0000ff00)|((x&0x00ff0000)>>16))

void CRT_SkinInstance::RenderHLight()
{
    if (GetState() != Object_State_Ready)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RtgMatrix16 oldMat;

    RTASSERT(dev);
    RTASSERT(m_core);
    RTASSERT(m_core->m_mtlLib);

    DWORD _Offset = 0;
    DWORD _Size = 0;

    RtgBufferItem* vb = m_core->RequestVB(this, &_Offset, &_Size);
    RtgBufferItem* ib = NULL;   

    if (!vb)
        return;

    size_t mtlNum = min(m_mtlLib->GetMtlNum(), m_core->m_rsList.size());

    RtgVertex3 dif3 = RtgColorToVector(COLOR_EXCHANGE(dev->m_dwPostProcessObjectHLColorDif));
    RtgVertex3 spe3 = RtgColorToVector(COLOR_EXCHANGE(dev->m_dwPostProcessObjectHLColorSpe));
    RtgVertex3 emi3 = RtgColorToVector(COLOR_EXCHANGE(dev->m_dwPostProcessObjectHLColorEmi));
    RtgVertex4 dif4 = RtgVertex4(dif3.x, dif3.y, dif3.z, 1.f);
    RtgVertex4 spe4 = RtgVertex4(spe3.x, spe3.y, spe3.z, 1.f);
    RtgVertex4 emi4 = RtgVertex4(emi3.x, emi3.y, emi3.z, 1.f);

    dev->SetStream_vb(0, vb, m_core->vertexSize(), _Offset, m_core->vertexNum());
    dev->SetStream_ib(ib);
    dev->SetVertexDecl_vnt();
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetRenderState(RTGRS_HEIGHTLIGHT, TRUE);
    dev->SetMaterial(dif4, 
        RtgVertex4(0.f, 0.f, 0.f, 0.f), 
        spe4, 
        emi4, 
        dev->m_fPostProcessObjectHLColorSpePower);
    dev->SetRenderState(RTGRS_LIGHTING, TRUE);

    for (size_t i = 0; i < mtlNum; ++i)
    {
        CRT_Material* mtl = m_mtlLib->GetMaterial(i);
        RTASSERT(mtl);

        if (!mtl->ResourceReady())
            continue;

        ib = m_core->m_rsList[i].RequestIB();

        if (!ib)
            continue;

        RtgShader _shader;
        _shader.dwTextureFactor = 0xffffffff;

        switch (mtl->GetAlphaMdoe())
        {
        case ALPHA_NULL :
            _shader.iNumStage = 1;
            _shader.Shaders[0].SetTexture(0);
            _shader.Shaders[0].eColorOp   = RTGTOP_MODULATE;
            _shader.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
            _shader.Shaders[0].eColorArg2 = RTGTA_DIFFUSE; 
            _shader.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
            _shader.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
            _shader.bEnableBlendFunc = true;
            _shader.eBlendSrcFunc = RTGBLEND_ONE;
            _shader.eBlendDstFunc = RTGBLEND_ONE;
        case ALPHA_COLOR_KEY :
            _shader.Shaders[0].SetTexture(mtl->GetBaseText());
            _shader.bColorKey = true;
            break;

        case ALPHA_SUB :
        case ALPHA_ADD :
            continue;
            break;

        case ALPHA_BLEND :
            continue;
            break;
        }

        dev->SetShader(&_shader);
        dev->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            0,
            0,
            m_core->vertexNum(),
            0,
            m_core->m_rsList[i].faceList.size() / 3
            );
        dev->RestoreShader(&_shader);
    }

    dev->SetRenderState(RTGRS_HEIGHTLIGHT, FALSE);
}

void CRT_SkinInstance::RenderFrame(const RtgVertex3& _color)
{
    if (GetState() != Object_State_Ready)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RtgMatrix16 oldMat;

    RTASSERT(dev);
    RTASSERT(m_core);
    RTASSERT(m_core->m_mtlLib);

    DWORD _Offset = 0;
    DWORD _Size = 0;

    RtgBufferItem* vb = m_core->RequestVB(this, &_Offset, &_Size);
    RtgBufferItem* ib = NULL; 

    if (!vb)
        return;

    size_t mtlNum = min(m_mtlLib->GetMtlNum(), m_core->m_rsList.size());

    DWORD dwCull = dev->GetRenderState(RTGRS_CULLMODE);
    dev->SetStream_vb(0, vb, m_core->vertexSize(), _Offset, m_core->vertexNum());
    dev->SetVertexDecl_vnt();
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetRenderState(RTGRS_CULLMODE, RTGCULL_CON);

    for (size_t i = 0; i < mtlNum; ++i)
    {
        CRT_Material* mtl = m_mtlLib->GetMaterial(i);
        RTASSERT(mtl);

        if (!mtl->ResourceReady())
            continue;

        ib = m_core->m_rsList[i].RequestIB();

        if (!ib)
            continue;

        RtgShader _shader;

        switch (mtl->GetAlphaMdoe())
        {
        case ALPHA_NULL :
            _shader.iNumStage = 1;
            _shader.dwTextureFactor = RtgVectorToColor(_color, 1.f);
            _shader.Shaders[0].SetTexture(0);
            _shader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
            _shader.Shaders[0].eColorArg1 = RTGTA_TFACTOR; 
            _shader.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
            _shader.bEnableBlendFunc = true;
            _shader.eBlendSrcFunc = RTGBLEND_ONE;
            _shader.eBlendDstFunc = RTGBLEND_ONE;
            break;

        case ALPHA_SUB :
        case ALPHA_ADD :
            continue;
            break;

        case ALPHA_BLEND :
            continue;
            break;
        }

        dev->SetShader(&_shader);
        dev->SetStream_ib(ib);
        dev->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            0,
            0,
            m_core->vertexNum(),
            0,
            m_core->m_rsList[i].faceList.size() / 3
            );
        dev->RestoreShader(&_shader);
    }

    dev->SetRenderState(RTGRS_CULLMODE, dwCull);
}

void CRT_SkinInstance::RenderNormal()
{
    RtgDeviceD3D9* dev = RtGetRender();
    verArray_t* va = Get_VertexArray();

    RTASSERT(dev);

    if (!va)
        return;

    RtgShareBufferItem vbItem;
    RtgVertex3* vb = (RtgVertex3*)dev->GetBuffMgr()->Lock_Shared_vb(va->size() * 2 * sizeof(RtgVertex3), &vbItem);

    for (size_t i = 0; i < va->size(); ++i)
    {
        (vb++)->Set((*va)[i].pos);
        (vb++)->Set((*va)[i].pos + (*va)[i].nor * 10.f);
    }

    dev->GetBuffMgr()->Unlock(&vbItem);

    DWORD blight = dev->GetRenderState(RTGRS_LIGHTING);

    dev->SetRenderState(RTGRS_LIGHTING, FALSE);
    dev->SetDefaultMaterial(1.f, 1.f, 1.f, 1.f);
    dev->SetShader(&dev->m_nullShader);
    dev->SetStream_vb(0, &vbItem, sizeof(RtgVertex3), D3DFVF_XYZ);
    dev->DrawPrimitive(
        D3DPT_LINELIST,
        0,
        va->size());
    dev->SetRenderState(RTGRS_LIGHTING, blight);


}

void CRT_SkinInstance::SetVisible(float visible)
{
    if (GetState() == Object_State_Ready)
        return SetVisibleImpl(visible);

    Rac_Setvisible* _cmd = CreateActorCommand<Rac_Setvisible>();
    _cmd->_visible = visible;
    m_listCommand.push_back(_cmd);	
}

void CRT_SkinInstance::SetVisibleImpl(float visible)
{
    float ret = m_frameVisible * visible;

    if (Abs(m_visible - visible) > FLT_EPSILON || Abs(m_frameVisible - m_oldFrameVisible) > FLT_EPSILON)
    {
        m_visible = visible;
        m_oldFrameVisible = m_frameVisible;

        for (int i = 0; i < GetMaterialLib()->m_mtlList.size(); ++i)
        {
            GetMaterialLib()->m_mtlList[i]->SetVisibleGlobal(ret);
        }
    }
}

void CRT_SkinInstance::UseFrame(float frame)
{
    if (!ResourceReady())
        return;

    float visible = m_frameVisible;

    if (m_core->UseFrame(this, frame, &visible, m_updateNor))
    {
        m_frameVisible = visible;
        SetVisible(m_visible);
    }

#ifdef CHAR_VIEWER
	m_updateNor = GetMaterialLib()->RequestUpdateNormal();
#endif

}

void CRT_SkinInstance::ProcessComnandList()
{
    if (!m_listCommand.empty())
    {
        Rac_CmdType* _cmd = (Rac_CmdType*)m_listCommand.front();
        bool bprocessed = false; 

        switch (GetActorCmdtype(_cmd))
        {
        case Ractype_Setbasecolor :
            {
                CRT_MaterialLib* _mtllib = GetMaterialLib();
                RTASSERT(_mtllib);

                if (_mtllib->GetState() == Object_State_Ready)
                {
                    Rac_Setbasecolor* _cmdp = (Rac_Setbasecolor*)_cmd;
                    SetBaseColorImpl(_cmdp->color);
                    bprocessed = true;
                }
             
            }
            break;
        case Ractype_Setvisible :
            {
                Rac_Setvisible* _cmdp = (Rac_Setvisible*)_cmd;
                SetVisibleImpl(_cmdp->_visible);
                bprocessed = true;
            }
            break;
        case Ractype_Setmtl :
            {
                Rac_Setmtl* _cmdp = (Rac_Setmtl*)_cmd;
                ChangeMaterialImpl(_cmdp->mtlname.c_str());
                bprocessed = true;
            }
            break;
        default :
            RtCoreLog().Warn("no actor comnand process(type : %u)\n", GetActorCmdtype(_cmd));
            bprocessed = false;
            break;
        }

        if (bprocessed)
        {
            DestroyActorCommand(_cmd);
            m_listCommand.pop_front();
        }
    }
}

void CRT_SkinInstance::ClearComnandList()
{
    m_listCommand.clear();
}

void CRT_SkinInstance::Tick(float deltaMill)
{
    if (!ResourceReady())
        return;

    CRT_MaterialLib* _mtllib = GetMaterialLib();

    ProcessComnandList();
    //lyymark 材质淡入淡出的效果
  /*  if (m_fadeOut < m_visible)
    {
        m_fadeOut += deltaMill / 2000.f;

        if (m_fadeOut > m_visible)
            m_fadeOut = m_visible;
        
        for (int i = 0; i < GetMaterialLib()->m_mtlList.size(); ++i)
            GetMaterialLib()->m_mtlList[i]->SetVisibleGlobal(m_fadeOut);
    }
    */
    if (_mtllib)
        _mtllib->Tick(deltaMill);
}

void CRT_SkinInstance::Output()
{
}

void CRT_SkinInstance::SetBaseColor(RtgVertex3 &color)
{	
    if (GetState() == Object_State_Ready)
    {
        if (!GetMaterialLib())
            return;

        if (GetMaterialLib()->GetState() == Object_State_Ready)
            return SetBaseColorImpl(color);
    }

    Rac_Setbasecolor* _cmd = CreateActorCommand<Rac_Setbasecolor>();
    _cmd->color = color;
    m_listCommand.push_back(_cmd);	
}

void CRT_SkinInstance::SetBaseColorImpl(RtgVertex3 &color)
{	
    CRT_MaterialLib* _mtllib = GetMaterialLib();
    RTASSERT(_mtllib);
    for(int i = 0; i < _mtllib->m_mtlList.size(); i++)
        _mtllib->m_mtlList[i]->SetBaseColor(color);
}

bool CRT_SkinInstance::ComputeVC(RtgMatrix16 &world, vcArray_t& aryVc, int offset,RtgVertex3 vSkyLight, RtgLightItem* pLights)
{
    DWORD vc = 0;
    verArray_t& aryVertex = *Get_VertexArray();
    RtgMatrix16 nor = world;
    RtgVertex3 v;
    RtgVertex3 n;
    RtgVertex4 amb_v4;
    RtgVertex4 dif_v4;
    RtgVertex4 emi_v4;
    RtgVertex4 spe_v4;
    RtgVertex3 amb_v3;
    RtgVertex3 dif_v3;
    RtgVertex3 emi_v3;
    RtgVertex3 spe_v3;

	nor._30 = nor._31 = nor._32 = 0.0f;

    for (size_t i = 0; i < aryVertex.size(); ++i)
    {
        v = aryVertex[i].pos * world;
        n = aryVertex[i].nor * nor;

        amb_v4.Set(1.f, 1.f, 1.f, 1.f);
        dif_v4.Set(1.f, 1.f, 1.f, 1.f);
        emi_v4.Set(0.f, 0.f, 0.f, 0.f);

        if (GetMaterialLib()->m_mtlList[0]->IsKindOf(RT_RUNTIME_CLASS(CRT_MtlStandard)))
            ((CRT_MtlStandard*)GetMaterialLib()->m_mtlList[0])->GetBaseInfo(amb_v4, dif_v4, emi_v4);
        
        amb_v3.Set(amb_v4.x, amb_v4.y, amb_v4.z);
        dif_v3.Set(dif_v4.x, dif_v4.y, dif_v4.z);
        emi_v3.Set(emi_v4.x, emi_v4.y, emi_v4.z);

        vc = RtgComputeVertexColor(v, n, amb_v3, dif_v3, emi_v3, vSkyLight, pLights);
        aryVc[i + offset] = vc;
    }

	return true;
}

bool CRT_SkinInstance::SetVC(vcArray_t& aryVc,int offset)
{
	m_aryVc = &aryVc;
	m_vcOffset = offset;

    RtgReleaseBuffer(m_pvc);
    
    UINT sizBuf = Get_VertexArray()->size() * sizeof(DWORD);

    RtgSetCreateBufferDebugInfo();
    m_pvc = RtGetRender()->GetBuffMgr()->CreateBuffer(
        sizBuf,
        D3DFVF_DIFFUSE,
        0,
        D3DPOOL_MANAGED,
        m_aryVc->pointer() + offset
        );

    if (!m_pvc)
        return false;
	return true;
}

bool CRT_SkinInstance::Save(RtArchive* ar)
{
    bool ret = true;
    RtArchive* local = ar;

    if (!local)
        local = RtCoreFile().CreateFileWriter(NULL, m_core->m_diskFile.c_str());

    if (!local) 
        return false;

    swap(m_core->m_mtlLib, m_mtlLib);
    ret = local->WriteObject(m_core);
    swap(m_core->m_mtlLib, m_mtlLib);

    if (!ar)
        DEL_ONE(local);
    return ret;
}

bool CRT_SkinInstance::SaveAll()
{
	return true;
}

RtcAcotrManager* RtcGetActorManager()
{
    static RtcAcotrManager _actMgr;
    return &_actMgr;
}

bool ActorInit()
{
	RT_STATIC_REGISTRATION_CLASS(CRT_PoolObject);
	RT_STATIC_REGISTRATION_CLASS(CRT_Skin);
	RT_STATIC_REGISTRATION_CLASS(CRT_VaSkin);
	RT_STATIC_REGISTRATION_CLASS(CRT_SkelSkin);
	RT_STATIC_REGISTRATION_CLASS(CRT_Actor);
	RT_STATIC_REGISTRATION_CLASS(CRT_MaterialLib);
	RT_STATIC_REGISTRATION_CLASS(CRT_Material);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlStandard);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlMu);
	RT_STATIC_REGISTRATION_CLASS(CRT_MtlBlend);
	RT_STATIC_REGISTRATION_CLASS(CRT_ActorInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_SkinInstance);
	RT_STATIC_REGISTRATION_CLASS(CRT_Effect);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectBase);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectEmitter);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectRibbon);
	RT_STATIC_REGISTRATION_CLASS(CRT_EffectRibbon2);
    RT_STATIC_REGISTRATION_CLASS(CRT_Effect_Dynamic_Ribbon);

    RtcGetActorManager()->Init();

	return true;
}

bool ActorExit()
{
    RtcGetActorManager()->Exit();
    return true;
}

RtsSceneUtil* baseScene = NULL;

RtsSceneUtil* ActorGetScene()
{
    return baseScene;
}

void ActorSetScene(RtsSceneUtil *scene)
{
    baseScene = scene;
}


#include "scene/rt_scene.h"

/*------------------------------------------------------------------------
-   ���������е��˶��ص�����
------------------------------------------------------------------------*/

// ��ֹ������
void cbInnervationMovementActionless(RtgVertex3& vLocalPos, float& fSpeed, float& fParamL1, float& fParamL2)
{
}

/*------------------------------------------------------------------------
-   ��̬ע�ắ�����������ע�����˶��ĺ���
------------------------------------------------------------------------*/

bool RtsInnervation::InitInnervation()
{
    pMovementFunction = NULL;
    RegisterMovement("��ֹ", cbInnervationMovementActionless);
    return true;
}

/*------------------------------------------------------------------------
-   RtsInnervation. ��̬���࣬���ڶ��嶯̬�Ĳ���
------------------------------------------------------------------------*/


RT_IMPLEMENT_DYNCREATE(RtsInnervation, RtsObject, 0, "scene")

RtsInnervation::SMovement* RtsInnervation::pMovementFunction = NULL;

RtsInnervation::RtsInnervation()
{
    m_pParticles = NULL;
    m_pUseParticles = NULL;
    m_pUnuseParticles = NULL;
    m_fLastCreateTime = 0.f;
    m_bPlaySecond = false;
    m_bValid = false;
    m_fRenderTime = 0.f;
#if RTS_DRAW_HELPER
    SetShowType(RtGetRender(),SO_MODEL);
#endif
}

RtsInnervation::~RtsInnervation()
{
    ReleaseResource();
}

void RtsInnervation::ReleaseResource()
{

    if (m_pParticles)
    {
        for (int i=0; i<m_iParticleNum; i++)
        {
            if (m_pParticles[i].pChar)
            {
                RtcGetActorManager()->ReleaseActor(m_pParticles[i].pChar);
                m_pParticles[i].pChar = NULL;
            }
        }
        DEL_ARRAY(m_pParticles);
        m_pParticles = NULL;
        m_pUseParticles = NULL;
        m_pUnuseParticles = NULL;
        m_fLastCreateTime = 0.f;
        m_bValid = false;
        m_fRenderTime = 0.f;
    }

     SetState(Object_State_New);

}

//void RtsInnervation::UnloadOldInnervation(RtgDevice* pDevice)
//{
//	funguard;
//    RtsObject* pObj = GetBeginObject();
//    while (pObj)
//    {
//        //if (pObj->IsKindOf(RT_RUNTIME_CLASS(RtsInnervation)) && pObj!=this)
//        if (pObj->IsKindOf(RT_RUNTIME_CLASS(RtsInnervation)))
//        {
//            if (((RtsInnervation*)pObj)->m_fRenderTime!=0.f && ((pDevice->GetAppTime() - ((RtsInnervation*)pObj)->m_fRenderTime)>5.f))
//            {
//                //RtCoreLog().Info("UnloadOldInnervation\n");
//                ((RtsInnervation*)pObj)->Unload();
//                break;
//            }
//        }
//        pObj = GetNextObject();
//    }
//	funguard_end;
//}

void RtsInnervation::SParticle::OnPoseEnd(SRT_Pose *pose)
{
    if (pParent->m_cPlaySecondRate>0)
    {
        if (rtRandom(pParent->m_cPlaySecondRate, 0)==0)
        {
            if (pChar->PlayPose("scene2"))
            {
                return;
            }
        }
    }
    pChar->PlayPose("scene");
}

bool RtsInnervation::CreateResource(const char fileName[], RtsSGrid* _grid, bool bForceLoad /* = false */)
{
    RtIni ini;
    const char* pFilename;
    m_fileName = fileName;

    if (!ini.OpenFile(m_fileName.c_str()))
    {
        RtCoreLog().Error("����: ���ܴ򿪷�����������ļ�[%s].\n", m_fileName.c_str());
        return false;
    }

    pFilename = ini.GetEntry("", "file");
    if (!pFilename) RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "file");
    else m_szCharFileName = pFilename;

    long lParticleNum;
    if (!ini.GetEntry("", "count", &lParticleNum))          RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "count");
    if (!ini.GetEntry("", "X_Ext", &(m_vRangeHalf.x)))      RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "X_Ext");
    if (!ini.GetEntry("", "Y_Ext", &(m_vRangeHalf.y)))      RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "Y_Ext");
    if (!ini.GetEntry("", "Z_Ext", &(m_vRangeHalf.z)))      RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "Z_Ext");
    if (!ini.GetEntry("", "GlobalSpeed", &m_fGlobalSpeed))  RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "GlobalSpeed");
    if (!ini.GetEntry("", "LocalSpeed", &m_fLocalSpeed))    RtCoreLog().Warn("����: ���ļ�[%s]���Ҳ�����[%s]\n", m_fileName.c_str(), "LocalSpeed");
    m_iParticleNum = (unsigned char)lParticleNum;

    long lSecondRate;
    if (!ini.GetEntry("", "SecondRate", &lSecondRate))
    {
        lSecondRate = 0;
    }
    m_cPlaySecondRate = (char)lSecondRate;

    ini.CloseFile();

    if (!Init(m_iParticleNum, m_vRangeHalf, m_fGlobalSpeed, m_fLocalSpeed))
    {
        RtCoreLog().Error("RtsInnervation::Reload: ��ʼ��ʧ��.");
        return false;
    }

    if (!OnReload(RtGetRender()))
    {
        return false;
    }
 //   SetLoad(TRUE);
    m_fRenderTime = RtGetRender()->GetAppTime();

    SetState(Object_State_Ready);

    return true;
}

bool RtsInnervation::OnReload(RtgDevice* pDevice)
{
    return true;
}

bool RtsInnervation::Init(int iParticleNum, RtgVertex3 vRangeHalf, float fGlobalSpeed, float fLocalSpeed)
{
    m_iParticleNum = iParticleNum;
    m_vRangeHalf = vRangeHalf;
    m_fGlobalSpeed = fGlobalSpeed;
    m_fLocalSpeed = fLocalSpeed;

    m_pParticles = RT_NEW SParticle[m_iParticleNum];
    for (int i=0; i<m_iParticleNum-1; i++)
    {
        m_pParticles[i].pNext = m_pParticles+i+1;
        m_pParticles[i].pChar = NULL;
        m_pParticles[i].pParent = this;
    }
    m_pParticles[m_iParticleNum-1].pNext = NULL;
    m_pParticles[m_iParticleNum-1].pChar = NULL;
    m_pParticles[m_iParticleNum-1].pParent = this;

    m_pUnuseParticles = m_pParticles;
    m_pUseParticles = NULL;

    this->m_AABB.vExt = vRangeHalf;
    this->m_AABB.vPos.Set(0.f);
    Update_WorldBoundBox();

    OnInitParticle();
    return true;
}

void RtsInnervation::Move(float fOffsetSecond)
{
    SParticle *pCur, *pPre;
    if (m_pUnuseParticles)
    {
        if (m_fLastCreateTime<=0.f)
        {
            pCur = m_pUnuseParticles;
            m_pUnuseParticles = m_pUnuseParticles->pNext;
            pCur->pNext = m_pUseParticles;
            m_pUseParticles = pCur;

            OnCreateParticle(pCur);
            m_fLastCreateTime = 3.f; // ÿ3�봴��һ������
        }else
        {
            m_fLastCreateTime -= fOffsetSecond;
        }
    }

    RtgVertex3 vDir;
    float fLen;
    bool bDelete = false;
    pPre = NULL;
    pCur = m_pUseParticles;
    while (pCur)
    {
        // �����ƶ�
        pCur->pCall(pCur->vLocalPos, pCur->fLocalSpeed, pCur->fParamL1, pCur->fParamL2);

        // ȫ���ƶ�
        fLen = (fOffsetSecond*pCur->fGlobalSpeed);
        vDir = pCur->vDestinationPos - pCur->vGlobalPos;
        if (fLen>vDir.Length())
        {
            pCur->vGlobalPos = pCur->vDestinationPos;
            bDelete = OnGlobalParticleDone(pCur);
        }else
        {
            vDir.Normalize();
            pCur->vGlobalPos += fLen*vDir;
        }

        if (bDelete)
        {
            if (pPre)
            {
                pPre->pNext = pCur->pNext;
            }else
            {
                m_pUseParticles = NULL;
            }
            pCur->pNext = m_pUnuseParticles;
            m_pUnuseParticles = pCur->pNext;
            if (pPre) pCur = pPre->pNext;
            else      pCur = NULL;
        }else
        {
            // ��һ������
            pCur = pCur->pNext;
        }
    }
}

void RtsInnervation::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
    m_fRenderTime = inDevice.GetAppTime();
    RtsObject::Render(inCamera, inDevice, eMask);
}

long RtsInnervation::Serialize(RtArchive& ar)
{
    if (!RtsObject::Serialize(ar)) return FALSE;
    ar << m_fileName;
    return TRUE;
}

bool RtsInnervation::ClearInnervation()
{
    SMovement* pDelete;
    while (pMovementFunction)
    {
        pDelete = pMovementFunction;
        pMovementFunction = pMovementFunction->pNext;
        DEL_ONE(pDelete);
    }
    return true;
}

bool RtsInnervation::RegisterMovement(const char* szName, TInnervationMovementCB pCallBack)
{
    SMovement* pMovement = RT_NEW SMovement;
    pMovement->szName = szName;
    pMovement->pCallBack = pCallBack;
    pMovement->pNext = pMovementFunction;
    pMovementFunction = pMovement;
    return true;
}

/*------------------------------------------------------------------------
The End.
------------------------------------------------------------------------*/

RT_IMPLEMENT_DYNCREATE(RtsInnerveFireFly, RtsInnervation, 0, "scene")

RtsInnerveFireFly::RtsInnerveFireFly()
{
}

RtsInnerveFireFly::~RtsInnerveFireFly()
{
}

bool RtsInnerveFireFly::OnReload(RtgDevice* pDevice)
{
    if (!m_fileName.empty())
    {
        if (m_pParticles[0].pChar==NULL)
        {
            int i;
//			m_pParticles[0].pChar = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT,m_szCharFileName);
			m_pParticles[0].pChar = RtcGetActorManager()->CreateActor(m_szCharFileName.c_str());
            if (m_pParticles[0].pChar==NULL)
            {
                RtCoreLog().Error("RtsInnerveFireFly::OnReload: �����������ļ�[%s]ʧ��\n", m_szCharFileName);
                return false;
            }
            for (i=1; i<m_iParticleNum; i++)
            {
//				m_pParticles[i].pChar =(CRT_ActorInstance*)ACTOR()->CreateInstance( ACTOR_RES_TYPE_ACT,m_szCharFileName);
				m_pParticles[i].pChar = RtcGetActorManager()->CreateActor(m_szCharFileName.c_str());//LoadActorInstance(m_szCharFileName);
                m_pParticles[i].pChar->PlayPose("scene", m_cPlaySecondRate==0);
                m_pParticles[i].pChar->RegisterNotify(m_pParticles+i);
            }

            //for (i=0; i<m_iParticleNum; i++)
            //{
            //    //m_pParticles[i].pChar->Create();
            //    m_pParticles[i].pChar->PlayPose("scene", m_cPlaySecondRate==0);
            //    m_pParticles[i].pChar->RegisterNotify(m_pParticles+i);
            //    m_pParticles[i].pChar->SetValid(m_bValid);
            //}
        }
        return true;
    }else
    {
        return RtsInnervation::OnReload(pDevice);
    }
}

void RtsInnerveFireFly::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
    RtsInnervation::Render(inCamera, inDevice, eMask);
    SParticle*  pParticle = m_pUseParticles;
    RtgMatrix16 m16 = this->GetMatrix();

    while (pParticle)
    {
        if (pParticle->pChar)
        {
            m16.SetRow(3, (pParticle->vGlobalPos));
            if (pParticle->vGlobalPos!=pParticle->vDestinationPos)
            {
                m16.FaceToVertex(pParticle->vDestinationPos, RtgVertex3(0.f, 0.f, 1.f));
            }

            pParticle->pChar->SetWorldMatrix(m16);
            //m16.Get4X3Matrix(&(pParticle->pChar->GetWorldMatrix()->_00));
            pParticle->pChar->Render(&inDevice, eMask);
        }

        pParticle = pParticle->pNext;
    }
}

void RtsInnerveFireFly::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    RtsInnervation::RenderHighLight(inCamera, inDevice, color, scale);
    SParticle*  pParticle = m_pUseParticles;
    RtgMatrix16 m16 = this->GetMatrix();

    while (pParticle)
    {
        if (pParticle->pChar)
        {
            m16.SetRow(3, (pParticle->vGlobalPos));
            if (pParticle->vGlobalPos!=pParticle->vDestinationPos)
            {
                m16.FaceToVertex(pParticle->vDestinationPos, RtgVertex3(0.f, 0.f, 1.f));
            }
            pParticle->pChar->SetWorldMatrix(m16);
            //m16.Get4X3Matrix(&(pParticle->pChar->GetWorldMatrix()->_00));
            pParticle->pChar->RenderHighLight(&inDevice, color, scale);
        }

        pParticle = pParticle->pNext;
    }
}

long RtsInnerveFireFly::Serialize(RtArchive& ar)
{
    if (!RtsInnervation::Serialize(ar)) return FALSE;
    return TRUE;
}

void RtsInnerveFireFly::OnInitParticle()
{
    //Move(rtSeconds().GetFloat());
}

void RtsInnerveFireFly::OnCreateParticle(SParticle* pParticle)
{
    CHECK(pParticle!=NULL);
    RtgVertex3 vPos = this->GetMatrix().Position();

    pParticle->vGlobalPos    = vPos;
    pParticle->vGlobalPos.x += m_vRangeHalf.x * (((rand()%2000)-1000)/1000.f);
    pParticle->vGlobalPos.y += m_vRangeHalf.y * (((rand()%2000)-1000)/1000.f);
    pParticle->vGlobalPos.z += m_vRangeHalf.z * ((rand()%1000)/1000.f);

    pParticle->vDestinationPos    = vPos;
    pParticle->vDestinationPos.x += m_vRangeHalf.x * (((rand()%2000)-1000)/1000.f);
    pParticle->vDestinationPos.y += m_vRangeHalf.y * (((rand()%2000)-1000)/1000.f);
    pParticle->vDestinationPos.z += m_vRangeHalf.z * ((rand()%1000)/1000.f);

    pParticle->vLocalPos.Set(0.f);

    pParticle->fParamG1 = 0.f;
    pParticle->fParamG2 = 0.f;
    pParticle->fParamL1 = 0.f;
    pParticle->fParamL2 = 0.f;

    pParticle->fGlobalSpeed = m_fGlobalSpeed;
    pParticle->fLocalSpeed  = m_fLocalSpeed;

    pParticle->pCall = pMovementFunction->pCallBack;
}

bool RtsInnerveFireFly::OnGlobalParticleDone(SParticle* pParticle)
{
    pParticle->vDestinationPos    = this->GetMatrix().Position();
    pParticle->vDestinationPos.x += m_vRangeHalf.x * (((rand()%2000)-1000)/1000.f);
    pParticle->vDestinationPos.y += m_vRangeHalf.y * (((rand()%2000)-1000)/1000.f);
    pParticle->vDestinationPos.z += m_vRangeHalf.z * ((rand()%1000)/1000.f);
    return false; // ��Ҫɾ��
}

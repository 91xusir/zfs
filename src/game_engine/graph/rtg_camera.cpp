
#include "rtg_graph_inter.h"

namespace rt_graph {

RT_IMPLEMENT_DYNCREATE(RtgCamera, RtObject, 0, "RtGraph")

RtgCamera::RtgCamera()
{

    m_bTremble = false;
    m_bColoredGlass = false;
    m_bOrtho = 0;
    ReUpdate();
}

RtgCamera::~RtgCamera()
{
}

void RtgCamera::Tremble(float fScope, float fCycle, float fTime)
{
    m_fTrembleCycle = fCycle;
    m_fTrembleScopeCur = m_fTrembleScope = fScope;
    m_fTrembleTimeCur = m_fTrembleTime = fTime;
    m_bTremble = true;
    m_bUpdate  = TRUE;
}

void RtgCamera::SetColoredGlass(EGlassType eType, DWORD dwColor, const char* szFilename, float fTimeIn, float fTimeOut, float fAlphaMax, float fAlphaMin)
{
    RtgDeviceD3D9* dev = RtGetRender();

    if (!dev)
        return;

    m_eGlassType = eType;
    m_fGlassTimeIn = fTimeIn;
    m_fGlassTimeOut = fTimeOut;
    m_fGlassTimeCur = 0.f;
    m_dwGlassColor = dwColor & 0x00FFFFFF;
    m_fAlphaMin = fAlphaMin;
    m_fAlphaMax = fAlphaMax;

    if (szFilename)
    {
        if (m_szFilename!=szFilename)
        {
            dev->DeleteShader(&m_GlassShader);
            m_GlassShader.iNumStage = 1;
            m_GlassShader.Shaders[0].SetTexture(dev->GetTextMgr()->CreateTextureFromFile(szFilename, 0, 0, 1));
            // m_GlassShader.Shaders[0].SetTexture(m_pDevice->m_Texture.CreateTexture(szFilename, RTGTBT_AUTO, true, 1));
        }

        m_svct[0] = RtgNVertexVCT(                    0.f,                     0.f, dwColor, 0.f, 1.f);
        m_svct[1] = RtgNVertexVCT(                    0.f, dev->m_iWndHeight, dwColor, 0.f, 0.f);
        m_svct[2] = RtgNVertexVCT( dev->m_iWndWidth, dev->m_iWndHeight, dwColor, 1.f, 0.f);
        m_svct[3] = RtgNVertexVCT( dev->m_iWndWidth,                     0.f, dwColor, 1.f, 1.f);
        m_eVF = RTG_TRANSFORMED_VCT;
    }else
    {
        dev->DeleteShader(&m_GlassShader);
        m_GlassShader.iNumStage = 0;
        m_szFilename = "";

        m_svc[0] = RtgNVertexVC(                    0.f,                     0.f, dwColor);
        m_svc[1] = RtgNVertexVC(                    0.f, dev->m_iWndHeight, dwColor);
        m_svc[2] = RtgNVertexVC( dev->m_iWndWidth, dev->m_iWndHeight, dwColor);
        m_svc[3] = RtgNVertexVC( dev->m_iWndWidth,                     0.f, dwColor);
        m_eVF = RTG_TRANSFORMED_VC;
    }

    switch (eType)
    {
    case COLOR_ADD:
        m_GlassShader.bEnableBlendFunc = true;
        m_GlassShader.eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_GlassShader.eBlendDstFunc = RTGBLEND_ONE;
        break;
    case COLOR_MODULATE:
        m_GlassShader.bEnableBlendFunc = true;
        m_GlassShader.eBlendSrcFunc = RTGBLEND_ZERO;
        m_GlassShader.eBlendDstFunc = RTGBLEND_SRCCOLOR;
        break;
    }

    m_szFilename = szFilename;
    m_bColoredGlass = true;
}

void RtgCamera::Render(float fDifSecond)
{
    RtgDeviceD3D9* dev = RtGetRender();

    if (!dev)
        return;

    if (m_bTremble)
    {
        m_fTrembleScopeCur = m_fTrembleScope * m_fTrembleTimeCur / m_fTrembleTime;
        m_fTrembleTimeCur -= fDifSecond;
        if (m_fTrembleTimeCur<0.f)
        {
            m_bTremble = false;
            m_fTrembleTimeCur = 0.f;
        }
    }
    if (m_bColoredGlass)
    {
        m_fGlassTimeCur += fDifSecond;
        if (m_fGlassTimeCur>m_fGlassTimeIn+m_fGlassTimeOut)
        {
            m_bColoredGlass = false;
        }
    }
    if (m_bColoredGlass)
    {
        BYTE cAlpha;
        if (m_fGlassTimeCur<m_fGlassTimeIn)
        {
            cAlpha = ((m_fGlassTimeCur/m_fGlassTimeIn)*(m_fAlphaMax-m_fAlphaMin)+m_fAlphaMin)*255;
        }else
        {
            cAlpha = (((m_fGlassTimeIn+m_fGlassTimeOut-m_fGlassTimeCur)/m_fGlassTimeOut)*(m_fAlphaMax-m_fAlphaMin)+m_fAlphaMin)*255;
        }
        DWORD dwColor = m_dwGlassColor | (cAlpha<<24);

        if (m_szFilename.IsEmpty())
        {
            m_svc[0].c = m_svc[1].c = m_svc[2].c = m_svc[3].c = dwColor;
        }else
        {
            m_svct[0].c = m_svct[1].c = m_svct[2].c = m_svct[3].c = dwColor;
        }

        BOOL bBakFog = dev->GetFogEnable();
        dev->SetFogEnable(FALSE);
        dev->SetShader(&m_GlassShader);
        dev->SetVertexFormat(m_eVF);
        if (m_eVF==RTG_TRANSFORMED_VCT)
        {
            dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, m_svct, sizeof(RtgNVertexVCT), 2);
        }else
        {
            dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, m_svc, sizeof(RtgNVertexVC), 2);

        }
        dev->RestoreShader(&m_GlassShader);
        dev->SetFogEnable(bBakFog);
    }
}

void RtgCamera::ReUpdate()
{
    m_bUpdate = TRUE;
}

void RtgCamera::SetViewParams(float fx, float fy, float fz, float cx, float cy, float cz, float ux, float uy, float uz)
{
	RtgVertex3 from(fx, fy, fz);
	RtgVertex3 center(cx, cy, cz);
	RtgVertex3 up(ux, uy, uz);
	SetViewParams( from, center, up );
}

void RtgCamera::SetViewParams(RtgVertex3 &vEyePt, RtgVertex3 &vLookatPt, RtgVertex3 &vUpVec )
{
    m_vEyePt    = vEyePt;
    m_vLookatPt = vLookatPt;

    m_vAxisZ = m_vEyePt - vLookatPt;        m_vAxisZ.Normalize();
    m_vAxisX = vUpVec.Cross(m_vAxisZ);      m_vAxisX.Normalize();
    m_vAxisY = m_vAxisZ.Cross(m_vAxisX);

    this->OnSetViewParams();

    ReUpdate();
}

void RtgCamera::OnSetViewParams()
{
    m_matView.Unit();

    m_matView.SetAxis(0, m_vAxisX);
    m_matView.SetAxis(1, m_vAxisY);
    m_matView.SetAxis(2, m_vAxisZ);

    m_matView._30 = - m_vAxisX.Dot(m_vEyePt);
    m_matView._31 = - m_vAxisY.Dot(m_vEyePt);
    m_matView._32 = - m_vAxisZ.Dot(m_vEyePt);

    // billboard matrix
    m_matBillboard = m_matView.Inverse();
    m_matBillboard._30 = 0.0f;
    m_matBillboard._31 = 0.0f;
    m_matBillboard._32 = 0.0f;
}

void RtgCamera::ToggleProjectType()
{
    m_bOrtho = !m_bOrtho;
    RestoreDevice();
}

void RtgCamera::SetProjectType(BOOL bPerspective)
{
    m_bOrtho = !bPerspective;
    RestoreDevice();
}

void RtgCamera::RestoreDevice()
{
    if (m_bOrtho)
    {
        SetOrthoParams(m_fLeft, m_fTop, m_fRight, m_fBottom, m_fNearPlane, m_fFarPlane);
    }else
    {
        SetProjParams(m_fFOV, m_fAspect, m_fNearPlane, m_fFarPlane);
    }
}

void RtgCamera::SetProjParams( float fFOV, float fAspect, float fNearPlane, float fFarPlane )
{
    m_bOrtho      = FALSE;
    m_fFOV        = fFOV;
    m_fAspect     = fAspect;
    m_fNearPlane  = fNearPlane;
    m_fFarPlane   = fFarPlane;
    m_matProj.Unit();
    m_matProj.Perspective(fFOV, fAspect, fNearPlane, fFarPlane);
    this->OnSetProjParams();
  //  UpdateFrustrumClipPlanes();
    ComputeFrustrum();
}

void RtgCamera::SetOrthoParams(float fLeft, float fTop, float fRight, float fBottom, float fNearPlane, float fFarPlane)
{
    m_bOrtho     = TRUE;
    m_fLeft      = fLeft;
    m_fRight     = fRight;
    m_fBottom    = fBottom;
    m_fTop       = fTop;
    m_fNearPlane = fNearPlane;
    m_fFarPlane  = fFarPlane;
    m_matProj.Unit();
#ifdef RTG_LEFT_HANDED
    m_matProj.Ortho(fLeft, fRight, fBottom, fTop, fNearPlane, fFarPlane, false);
#else
    m_matProj.Ortho(fLeft, fRight, fBottom, fTop, fNearPlane, fFarPlane, true);
#endif
    this->OnSetProjParams();
    //UpdateFrustrumClipPlanes();
    ComputeFrustrum();
}

void RtgCamera::OnSetProjParams()
{
}

void RtgCamera::Identity(void)
{
    m_matView.Identity();
    ReUpdate();
}

void RtgCamera::UpdateMatrix(void)
{
    if (m_bUpdate==TRUE)
    {       
		m_bUpdate = FALSE; // update flag
        OnUpdateMatrix();
        ComputeFrustrum();
      //  UpdateFrustrumClipPlanes();		
    }
}

void RtgCamera::GetEyeInfo(RtgVertex3& eyePos, RtgVertex3& eyeDir)
{
#if 1
    RtgMatrix16 matProj = m_matViewProj;

    RtgVertex3 v;
    v.x = 0.f / matProj._00;
    v.y = 0.f / matProj._11;
#ifdef RTG_LEFT_HANDED
    v.z =  1.0f;
#else
    v.z =  -1.0f;
#endif

    RtgMatrix16 m = m_matView.Inverse();

    eyeDir.x  = v.x*m._00 + v.y*m._10 + v.z*m._20;
    eyeDir.y  = v.x*m._01 + v.y*m._11 + v.z*m._21;
    eyeDir.z  = v.x*m._02 + v.y*m._12 + v.z*m._22;
    eyeDir.Normalize();

    eyePos.x = m._30;
    eyePos.y = m._31;
    eyePos.z = m._32;
#else
    RtgVertex3 vOut, v;
    RtgMatrix16 matInvProj;
    matInvProj = m_matViewProj.Inverse();

    v.x = 0.f;
    v.y = 0.f;
    v.z = 0.f;

    vOut = v * matInvProj;
    eyePos.Set(vOut.x/vOut.w,vOut.y/vOut.w,vOut.z/vOut.w);

    v.z = 1.0f;
    vOut = v * matInvProj;
    eyeDir.Set(vOut.x/vOut.w,vOut.y/vOut.w,vOut.z/vOut.w);

    // get dir
    eyeDir = eyeDir-eyePos;
    eyeDir.Normalize();
#endif
}

void RtgCamera::OnUpdateMatrix()
{ 
}

void RtgCamera::AddPitch(float fAmount)
{
    m_bUpdate |= OnAddPitch(fAmount);
}

void RtgCamera::AddYaw(float fAmount)
{
    m_bUpdate |= OnAddYaw(fAmount);
}

void RtgCamera::AddRoll(float fAmount)
{
    m_bUpdate |= OnAddRoll(fAmount);
}

void RtgCamera::MoveForward(float fAmount)
{
    m_bUpdate |= OnMoveForward(fAmount);
}

void RtgCamera::MoveRight(float fAmount)
{
    m_bUpdate |= OnMoveRight(fAmount);
}

void RtgCamera::MoveUp(float fAmount)
{
    m_bUpdate |= OnMoveUp(fAmount);
}

void RtgCamera::SetPitch(float fValue)
{
    m_bUpdate |= OnSetPitch(fValue);
}

void RtgCamera::SetYaw(float fValue)
{
    m_bUpdate |= OnSetYaw(fValue);
}

void RtgCamera::SetRoll(float fValue)
{
    m_bUpdate |= OnSetRoll(fValue);
}

void RtgCamera::SetPosition(RtgVertex3 &vPos)
{
    m_bUpdate |= OnSetPosition(vPos);
}

void RtgCamera::SetForward(float fValue)
{
    m_bUpdate |= OnSetForward(fValue);
}

void RtgCamera::SetRight(float fValue)
{
    m_bUpdate |= OnSetRight(fValue);
}

void RtgCamera::SetUp(float fValue)
{
    m_bUpdate |= OnSetUp(fValue);
}

void RtgCamera::ComputeFrustrum()
{
    m_matViewProj = m_matView * m_matProj;

    // right
    m_Planes[0].x = (m_matViewProj._03 - m_matViewProj._00);
    m_Planes[0].y = (m_matViewProj._13 - m_matViewProj._10);
    m_Planes[0].z = (m_matViewProj._23 - m_matViewProj._20);
    m_Planes[0].w = (m_matViewProj._33 - m_matViewProj._30);
    m_Planes[0].PlaneNormalize();

    // left
    m_Planes[1].x = (m_matViewProj._03 + m_matViewProj._00);
    m_Planes[1].y = (m_matViewProj._13 + m_matViewProj._10);
    m_Planes[1].z = (m_matViewProj._23 + m_matViewProj._20);
    m_Planes[1].w = (m_matViewProj._33 + m_matViewProj._30);
    m_Planes[1].PlaneNormalize();

    // buttom
    m_Planes[2].x = (m_matViewProj._03 + m_matViewProj._01);
    m_Planes[2].y = (m_matViewProj._13 + m_matViewProj._11);
    m_Planes[2].z = (m_matViewProj._23 + m_matViewProj._21);
    m_Planes[2].w = (m_matViewProj._33 + m_matViewProj._31);
    m_Planes[2].PlaneNormalize();

    // top
    m_Planes[3].x = (m_matViewProj._03 - m_matViewProj._01);
    m_Planes[3].y = (m_matViewProj._13 - m_matViewProj._11);
    m_Planes[3].z = (m_matViewProj._23 - m_matViewProj._21);
    m_Planes[3].w = (m_matViewProj._33 - m_matViewProj._31);
    m_Planes[3].PlaneNormalize();

    // near
    m_Planes[4].x = (m_matViewProj._03 + m_matViewProj._02);
    m_Planes[4].y = (m_matViewProj._13 + m_matViewProj._12);
    m_Planes[4].z = (m_matViewProj._23 + m_matViewProj._22);
    m_Planes[4].w = (m_matViewProj._33 + m_matViewProj._32);
    m_Planes[4].PlaneNormalize();

    // far
    m_Planes[5].x = (m_matViewProj._03 - m_matViewProj._02);
    m_Planes[5].y = (m_matViewProj._13 - m_matViewProj._12);
    m_Planes[5].z = (m_matViewProj._23 - m_matViewProj._22);
    m_Planes[5].w = (m_matViewProj._33 - m_matViewProj._32);
    m_Planes[5].PlaneNormalize();
}

//bool RtgCamera::ViewCheckPoint(float x, float y, float z)
//{
//    RtgVertex4 *pPlane = m_Planes;
//    for (int i=0; i<6; i++, pPlane++)
//    {
//        if (pPlane->x * x + pPlane->y * y + pPlane->z * z + pPlane->w > 0)
//            return false;
//    }
//    return true;
//}
//
//bool RtgCamera::ViewCheckSphere(float x, float y, float z, float fRadius)
//{
//    RtgVertex4 *pPlane = m_Planes;
//    for (int i=0; i<6; i++, pPlane++)
//    {
//        if (pPlane->x * x + pPlane->y * y + pPlane->z * z + pPlane->w > fRadius)
//            return false;
//    }
//    return true;
//}
//
//bool RtgCamera::ViewCheckBound(const RtgAABB &aabb, const RtgMatrix16 &m16)
//{
//	RtgVertex3 v=aabb.vExt*m16;
//    float fRadius = v.Length();//((RtgAABB)aabb).vExt.Length();
//    return ViewCheckSphere(m16._30+aabb.vPos.x, m16._31+aabb.vPos.y, m16._32+aabb.vPos.z, fRadius);
//}

void RtgCamera::SetMatrix(RtgMatrix16& ma)
{
	// camera transform matrix
    ma.GetRow(0, m_vAxisX);
    ma.GetRow(1, m_vAxisY);
    ma.GetRow(2, m_vAxisZ);
	ma.GetRow(3, m_vEyePt);
	// billboard matrix
	m_matBillboard = ma;
    m_matBillboard._30 = 0.0f;
    m_matBillboard._31 = 0.0f;
    m_matBillboard._32 = 0.0f;
	// invert of camera matrix
	m_matView = ma.Inverse();

    if (m_bTremble)
    {
        m_matView._32 += m_fTrembleScopeCur * fsin(((m_fTrembleTime-m_fTrembleTimeCur)/m_fTrembleCycle)*PIx2);
    }

}

// 需要注意的是，如果同时更改投影矩阵和显示矩阵就一定要在调用本函数之前更新投影矩阵
void RtgCamera::SetViewMatrix(RtgMatrix16& m16)
{
    // view matrix
    m_matView = m16;

    // billboard matrix
    m_matBillboard = m16.Inverse();
    m_matBillboard._30 = 0.0f;
    m_matBillboard._31 = 0.0f;
    m_matBillboard._32 = 0.0f;

    // eye position
    m_vEyePt.Set((float*)m_matView.Position());

    ComputeFrustrum();
    // Update planes
    //UpdateFrustrumClipPlanes();

    // no update
    m_bUpdate = FALSE;
}

/*----------------------------------------------------------------------------
    class RtgCameraFPS.
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgCameraFPS, RtgCamera, 0, "RtGraph")

RtgCameraFPS::RtgCameraFPS() : RtgCamera()
{
}

RtgCameraFPS::~RtgCameraFPS()
{
}

void RtgCameraFPS::OnSetViewParams()
{
    m_RefMat.Identity();
    m_fRY = m_fRZ = 0.f;

#ifdef RTG_LEFT_HANDED
    m_fRX = -3.14f;
#else
    m_fRX = 0.f;
#endif
    m_RefMat.Identity();

    m_RefMat.SetAxis(0, m_vAxisX);
    m_RefMat.SetAxis(1, m_vAxisY);
    m_RefMat.SetAxis(2, m_vAxisZ);

    m_RefMat._30 = 0.f;
    m_RefMat._31 = 0.f;
    m_RefMat._32 = 0.f;
}

void RtgCameraFPS::OnUpdateMatrix()
{
    RtgVertex3 v;
    v.Zero();
    //v = m_pSporter->GetMatrix()->Position(); // 取得位置
    v.z += 50.f;

	RtgMatrix16 ma;
	ma.Identity();
	ma.RotateLY(m_fRY);
	ma.RotateLZ(-m_fRZ);
	ma.RotateLX(m_fRX);
	ma.SetRow(3, v);
    //ma.SetRow(3, m_vEyePt);

    SetMatrix(m_RefMat*ma);
}

BOOL RtgCameraFPS::OnAddPitch(float fAmount)
{
    m_fRX -= fAmount;
    return TRUE;
}

BOOL RtgCameraFPS::OnAddYaw(float fAmount)
{
    m_fRY -= fAmount;
    return TRUE;
}

BOOL RtgCameraFPS::OnAddRoll(float fAmount)
{
    m_fRZ += fAmount;
    return TRUE;
}

BOOL RtgCameraFPS::OnMoveForward(float fAmount)
{
    return FALSE;
}

BOOL RtgCameraFPS::OnMoveRight(float fAmount)
{
    return FALSE;
}

BOOL RtgCameraFPS::OnMoveUp(float fAmount)
{
    return FALSE;
}

/*----------------------------------------------------------------------------
    class RtgCameraEditor.
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgCameraEditor, RtgCamera, 0, "RtGraph")

RtgCameraEditor::RtgCameraEditor() : RtgCamera()
{
	m_vPos.Zero();
	m_fPitch = m_fYaw = m_fRoll = 0.f;
}

RtgCameraEditor::~RtgCameraEditor()
{
}

int RtgCameraEditor::SaveToBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(szBuffer+iSize, &(m_vPos.x), sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(szBuffer+iSize, &m_fPitch,   sizeof(float)*3   ); iSize += sizeof(float)*3;
    return iSize;
}

int RtgCameraEditor::LoadFromBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(&(m_vPos.x), szBuffer+iSize, sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(&m_fPitch,   szBuffer+iSize, sizeof(float)*3   ); iSize += sizeof(float)*3;
    return iSize;
}

void RtgCameraEditor::OnSetViewParams()
{
    // 根据当前的值来设定我们需要的值
    m_vPos = m_vLookatPt;
    m_fYaw = m_fPitch = 0.f;
    m_fRoll = DegreeToRadian(60.f);
}

void RtgCameraEditor::OnUpdateMatrix()
{
	RtgMatrix16 ma;
	ma.Identity();
	ma.SetRow(3, m_vPos);
	ma.RotateLY(m_fPitch);
	ma.RotateLZ(-m_fYaw);
	ma.RotateLX(m_fRoll);
    SetMatrix(ma);
}

BOOL RtgCameraEditor::OnAddPitch(float fAmount)
{
    m_fPitch += fAmount;
    m_fPitch = 0.f;
    return TRUE;
}

BOOL RtgCameraEditor::OnAddYaw(float fAmount)
{
    m_fYaw += fAmount;
    return TRUE;
}

BOOL RtgCameraEditor::OnAddRoll(float fAmount)
{
    m_fRoll += fAmount;
    return TRUE;
}

BOOL RtgCameraEditor::OnMoveForward(float fAmount)
{
    RtgVertex3 v = m_vPos;
    RtgVertex3 up(0.f, 0.f, 1.f);
    RtgVertex3 dir = m_vAxisX.Cross(up);
    v += dir * fAmount;
    m_vPos.x = v.x;
    m_vPos.y = v.y;
	m_vPos.z = v.z;
    return TRUE;
}

BOOL RtgCameraEditor::OnMoveRight(float fAmount)
{
    RtgVertex3 v = m_vPos;
    v += m_vAxisX * fAmount;
    m_vPos.x = v.x;
    m_vPos.y = v.y;
	m_vPos.z = v.z;
    return TRUE;
}

BOOL RtgCameraEditor::OnMoveUp(float fAmount)
{
    m_vPos.z += fAmount;
    return TRUE;
}

BOOL RtgCameraEditor::OnSetPitch(float fValue)
{
    m_fPitch = fValue;
    return TRUE;
}

BOOL RtgCameraEditor::OnSetYaw(float fValue)
{
    m_fYaw = fValue;
    return TRUE;
}

BOOL RtgCameraEditor::OnSetRoll(float fValue)
{
    m_fRoll = fValue;
    return TRUE;
}

BOOL RtgCameraEditor::OnSetUp(float fValue)
{
    m_vPos.z = fValue;
    return TRUE;
}

BOOL RtgCameraEditor::OnSetPosition(RtgVertex3 &vPos)
{
    m_vPos = vPos;
    return TRUE;
}

/*----------------------------------------------------------------------------
class RtgCamera3DMax.
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgCamera3DMax, RtgCamera, 0, "RtGraph")

RtgCamera3DMax::RtgCamera3DMax() : RtgCamera()
{
	m_vPos.Zero();
	m_fPitch = m_fYaw = m_fRoll = 0.f;
}

RtgCamera3DMax::~RtgCamera3DMax()
{
}

int RtgCamera3DMax::SaveToBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(szBuffer+iSize, &(m_vPos.x), sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(szBuffer+iSize, &m_fPitch,   sizeof(float)*3   ); iSize += sizeof(float)*3;
    return iSize;
}

int RtgCamera3DMax::LoadFromBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(&(m_vPos.x), szBuffer+iSize, sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(&m_fPitch,   szBuffer+iSize, sizeof(float)*3   ); iSize += sizeof(float)*3;
    return iSize;
}

void RtgCamera3DMax::OnSetViewParams()
{
	// 根据当前的值来设定我们需要的值
	m_vPos = m_vLookatPt;
	m_fYaw = m_fPitch = 0.f;
	m_fRoll = DegreeToRadian(60.f);
}

void RtgCamera3DMax::OnUpdateMatrix()
{
	RtgMatrix16 ma;
	ma.Identity();
	ma.SetRow(3, m_vPos);
	ma.RotateLY(m_fPitch);
	ma.RotateLZ(-m_fYaw);
	ma.RotateLX(m_fRoll);
	
	SetMatrix(ma);
}

BOOL RtgCamera3DMax::OnAddPitch(float fAmount)
{
	m_fPitch += fAmount;
	m_fPitch = 0.f;
	return TRUE;
}

BOOL RtgCamera3DMax::OnAddYaw(float fAmount)
{
	m_fYaw += fAmount;
	return TRUE;
}

BOOL RtgCamera3DMax::OnAddRoll(float fAmount)
{
	m_fRoll += fAmount;
	return TRUE;
}

BOOL RtgCamera3DMax::OnMoveForward(float fAmount)
{
	m_vPos -= m_vAxisZ * fAmount;
	return TRUE;
}

BOOL RtgCamera3DMax::OnMoveRight(float fAmount)
{
	m_vPos += m_vAxisX * fAmount;
	return TRUE;
}

BOOL RtgCamera3DMax::OnMoveUp(float fAmount)
{
	m_vPos.z += fAmount;
	return TRUE;
}

BOOL RtgCamera3DMax::OnSetPitch(float fValue)
{
	m_fPitch = fValue;
	return TRUE;
}

BOOL RtgCamera3DMax::OnSetYaw(float fValue)
{
	m_fYaw = fValue;
	return TRUE;
}

BOOL RtgCamera3DMax::OnSetRoll(float fValue)
{
	m_fRoll = fValue;
	return TRUE;
}

BOOL RtgCamera3DMax::OnSetUp(float fValue)
{
	m_vPos.z = fValue;
	return TRUE;
}

BOOL RtgCamera3DMax::OnSetPosition(RtgVertex3 &vPos)
{
	m_vPos = vPos;
	return TRUE;
}

/*----------------------------------------------------------------------------
-   class RtgCameraGame.
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgCameraGame, RtgCamera, 0, "RtGraph")

RtgCameraGame::RtgCameraGame() : RtgCamera()
{
    m_vPos.Zero();
    m_fPitch = m_fYaw = m_fRoll = m_fDistance = 0.f;
    m_fUpOffset = 30.f;
}

RtgCameraGame::~RtgCameraGame()
{
}

int RtgCameraGame::SaveToBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(szBuffer+iSize, &(m_vPos.x), sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(szBuffer+iSize, &m_fPitch,   sizeof(float)*5   ); iSize += sizeof(float)*5;
    return iSize;
}

int RtgCameraGame::LoadFromBuffer(char* szBuffer)
{
    int iSize = 0;
    if (szBuffer) memcpy(&(m_vPos.x), szBuffer+iSize, sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
    if (szBuffer) memcpy(&m_fPitch,   szBuffer+iSize, sizeof(float)*5   ); iSize += sizeof(float)*5;
    return iSize;
}

void RtgCameraGame::OnSetViewParams()
{
    // 根据当前的值来设定我们需要的值
    m_vPos = m_vLookatPt;
    m_fYaw = m_fPitch = 0.f;
    m_fRoll = DegreeToRadian(60.f);
    m_fDistance = 200.f;
}

void RtgCameraGame::OnUpdateMatrix()
{
    RtgMatrix16 ma;
    ma.Identity();
    ma.SetRow(3, m_vPos);
    ma._32 += m_fUpOffset;
    ma.RotateLY(m_fPitch);
    ma.RotateLZ(-m_fYaw);
    ma.RotateLX(m_fRoll);
    ma.TranslateZ(m_fDistance);
    SetMatrix(ma);
}

BOOL RtgCameraGame::OnAddPitch(float fAmount)
{
    m_fPitch += fAmount;
    m_fPitch = 0.f;
    return FALSE;
}

BOOL RtgCameraGame::OnAddYaw(float fAmount)
{
    m_fYaw += fAmount;	
    return  ( fAmount != 0.0f );
}

BOOL RtgCameraGame::OnAddRoll(float fAmount)
{
    m_fRoll += fAmount;
    return ( fAmount != 0.0f );
}

BOOL RtgCameraGame::OnMoveForward(float fAmount)
{
    m_fDistance += fAmount;
    return ( fAmount != 0.0f );
}

BOOL RtgCameraGame::OnSetForward(float fValue)
{
	BOOL bResult = ( m_fDistance != fValue );
    m_fDistance  = fValue;
    return bResult;
}

BOOL RtgCameraGame::OnSetPitch(float fValue)
{
	BOOL bResult = ( m_fPitch != fValue );
    m_fPitch = fValue;
    return bResult;
}

BOOL RtgCameraGame::OnSetYaw(float fValue)
{
	BOOL bResult = ( m_fYaw != fValue );
    m_fYaw = fValue;
    return bResult;
}

BOOL RtgCameraGame::OnSetRoll(float fValue)
{
	BOOL bResult = ( m_fRoll != fValue );
    m_fRoll = fValue;
    return bResult;
}

BOOL RtgCameraGame::OnSetPosition(RtgVertex3 &vPos)
{
	BOOL bResult = ( m_vPos != vPos );
    m_vPos = vPos;
    return bResult;
}

/*----------------------------------------------------------------------------
-   class RtgCameraD3DView.
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgCameraD3DView, RtgCamera, 0, "RtGraph")

RtgCameraD3DView::RtgCameraD3DView() : RtgCamera()
{
	m_vPos.Zero();
	m_fPitch = m_fYaw = m_fRoll = m_fDistance = m_fTranslationX = m_fTranslationY = 0.f;
	m_fUpOffset = 30.f;
}

RtgCameraD3DView::~RtgCameraD3DView()
{
}

int RtgCameraD3DView::SaveToBuffer(char* szBuffer)
{
	int iSize = 0;
	if (szBuffer) memcpy(szBuffer+iSize, &(m_vPos.x), sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
	if (szBuffer) memcpy(szBuffer+iSize, &m_fPitch,   sizeof(float)*5   ); iSize += sizeof(float)*5;
	return iSize;
}

int RtgCameraD3DView::LoadFromBuffer(char* szBuffer)
{
	int iSize = 0;
	if (szBuffer) memcpy(&(m_vPos.x), szBuffer+iSize, sizeof(RtgVertex3)); iSize += sizeof(RtgVertex3);
	if (szBuffer) memcpy(&m_fPitch,   szBuffer+iSize, sizeof(float)*5   ); iSize += sizeof(float)*5;
	return iSize;
}

void RtgCameraD3DView::OnSetViewParams()
{
	// 根据当前的值来设定我们需要的值
	m_vPos = m_vLookatPt;
	m_fYaw = m_fPitch = 0.f;
	m_fRoll = DegreeToRadian(60.f);
	m_fDistance = 0.f;
}

void RtgCameraD3DView::OnUpdateMatrix()
{
	RtgMatrix16 ma;
	ma.Identity();
	ma.SetRow(3, m_vPos);
	ma._32 += m_fUpOffset;
	ma.RotateLY(m_fPitch);
	ma.RotateLZ(-m_fYaw);
	ma.RotateLX(m_fRoll);
	ma.TranslateZ(m_fDistance);
	ma.TranslateX(m_fTranslationX);
	ma.TranslateY(m_fTranslationY);
	SetMatrix(ma);
}

BOOL RtgCameraD3DView::OnAddPitch(float fAmount)
{
	m_fPitch += fAmount;
	m_fPitch = 0.f;
	return FALSE;
}

BOOL RtgCameraD3DView::OnAddYaw(float fAmount)
{
	m_fYaw += fAmount;	
	return  ( fAmount != 0.0f );
}

BOOL RtgCameraD3DView::OnAddRoll(float fAmount)
{
	m_fRoll += fAmount;
	return ( fAmount != 0.0f );
}

BOOL RtgCameraD3DView::OnMoveForward(float fAmount)
{
	m_fDistance += fAmount;
	return ( fAmount != 0.0f );
}

BOOL RtgCameraD3DView::OnMoveRight(float fAmount)
{
	m_fTranslationX += fAmount;
	return ( fAmount != 0.0f );
}

BOOL RtgCameraD3DView::OnMoveUp(float fAmount)
{
	m_fTranslationY += fAmount;
	return ( fAmount != 0.0f );
}

BOOL RtgCameraD3DView::OnSetRight(float fValue)
{
	BOOL bResult = ( m_fTranslationX != fValue );
	m_fTranslationX  = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetUp(float fValue)
{
	BOOL bResult = ( m_fTranslationY != fValue );
	m_fTranslationY  = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetForward(float fValue)
{
	BOOL bResult = ( m_fDistance != fValue );
	m_fDistance  = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetPitch(float fValue)
{
	BOOL bResult = ( m_fPitch != fValue );
	m_fPitch = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetYaw(float fValue)
{
	BOOL bResult = ( m_fYaw != fValue );
	m_fYaw = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetRoll(float fValue)
{
	BOOL bResult = ( m_fRoll != fValue );
	m_fRoll = fValue;
	return bResult;
}

BOOL RtgCameraD3DView::OnSetPosition(RtgVertex3 &vPos)
{
	BOOL bResult = ( m_vPos != vPos );
	m_vPos = vPos;
	return bResult;
}
} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

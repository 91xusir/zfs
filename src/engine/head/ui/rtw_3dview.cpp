#include "ui/rtw_ui.h"
#include "character/actor.h"

RT_IMPLEMENT_DYNAMIC(Rtw3DView, RtwWidget, 0, "ui")

Rtw3DView::Rtw3DView()
{
	m_pCameraActor = NULL;

	RtwRect rcLocation;
	rcLocation.left = 100;
	rcLocation.top = 100;
	rcLocation.right = 600;
	rcLocation.bottom = 600;

	Move(rcLocation);
	SetWidgetSize(SSize(rcLocation.right-rcLocation.left,rcLocation.bottom-rcLocation.top));

	m_fMouseWheelSpeed = 4.f;
	m_fMouseMoveSpeed = 8.f;
	m_fMaxRoll = 180;
	m_fMinRoll = -180;

	m_fCameraMinDistance      = 0.f;
	m_fCameraMaxDistance      = 500.f;
	m_fCameraDefaultDistance  = 0.f;

	m_bCanForward = true;
	m_bCanYaw = true;
	m_bCanRoll = true;

	m_bRefreshCam = true;

	m_fDefYawDegree = 38.f;
	m_fDefRollDegree = 18.f;
	m_fUpOffset = 0.f;

	_ResetSelf();
}

Rtw3DView::~Rtw3DView()
{

}

void Rtw3DView::OnClick_Close(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (m_pBtn_Close->getFlag(wfVisible,false))
	{
		Hide();
	}
}

void Rtw3DView::OnRemoveChild(RtwWidget* pWidget)
{
	if (pWidget == m_pBtn_Close)
		DROP_RTUI_OBJECT(m_pBtn_Close);
}

void Rtw3DView::Show()
{
	RtwWidget::Show();
}	

void Rtw3DView::Hide()
{
	RtwWidget::Hide();
	_ResetSelf();
}


void Rtw3DView::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void Rtw3DView::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool Rtw3DView::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	m_pBtn_Close = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$Close");
	UI_ENSURE_B(m_pBtn_Close);
	AddChild(m_pBtn_Close);
	m_pBtn_Close->Show();
	m_pBtn_Close->SetJoinParentAutoScroll(false);
	m_pBtn_Close->EvLClick += RTW_CALLBACK(this, Rtw3DView, OnClick_Close);

	return true;
}

void Rtw3DView::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pBtn_Close);

	RtwWidget::OnBeforeDestroy();
}

void Rtw3DView::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void Rtw3DView::SetShowCloseButton(bool bShow)
{
	if (!m_pBtn_Close || m_pBtn_Close->getVisible() == bShow)
		return;

	if (bShow)
		m_pBtn_Close->Show();
	else 
		m_pBtn_Close->Hide();
}

void Rtw3DView::SetDefYawDegree(float fAmount)
{
	m_fIncreaseX = DegreeToRadian(fAmount);
	m_bRefreshCam = true;
}

void Rtw3DView::SetDefRollDegree(float fAmount)
{
	m_fIncreaseY = DegreeToRadian(fAmount);
	m_bRefreshCam = true;
}

void Rtw3DView::SetDefForward(float fAmount)
{
	m_fCameraDefaultDistance = fAmount;
	m_fCameraCurrentDistance  = m_fCameraDefaultDistance;
	m_bRefreshCam = true;
}

void Rtw3DView::_ResetSelf()
{
	ReleaseAllActor();

	m_WidgetType = wt3dview;
	m_fCameraCurrentDistance  = m_fCameraDefaultDistance;
	m_fNowRollY = 10.f;

	m_fIncreaseX = DegreeToRadian(m_fDefYawDegree);
	m_fIncreaseY = DegreeToRadian(m_fDefRollDegree);
	m_fAmount = 0.f;

	m_bRefreshCam = true;
}

int Rtw3DView::AddCreateActor(char *filename,RtgMatrix12* matrix/* = NULL*/)
{
	CRT_ActorInstance *pActor = RtcGetActorManager()->CreateActor(filename);
	if (!pActor) return -1;

	RtgMatrix12 mat;
	if (matrix) mat = *matrix;
	else mat.Unit();

	pActor->SetWorldMatrix(mat);
	mat.Unit();
	pActor->SetMatrix(mat);
	pActor->Enable(true,true);

	m_vCreateActor.push_back(pActor);

	return AddRenderActor(pActor);
}

int Rtw3DView::AddRenderActor(CRT_ActorInstance *m_pActor,RtgMatrix12* matrix/* = NULL*/)
{
	CRT_ActorInstance *pActor = m_pActor;
	if (!pActor) 
		return -1;

	if (matrix)
		pActor->SetWorldMatrix(*matrix);

	m_vRenderActor.push_back(pActor);

	std::vector<RtgMatrix12*> vViewMatrix;
	vViewMatrix.push_back((RtgMatrix12*)pActor->GetWorldMatrix());
	m_vViewMatrixOther.push_back(vViewMatrix);

	m_bRefreshCam = true;

	if(!m_pCameraActor)
		m_pCameraActor = m_vRenderActor[0];

	return m_vRenderActor.size() - 1;
}

void Rtw3DView::SetActorMatrix(int cont,RtgMatrix12* matrix)
{
	if (cont < m_vRenderActor.size() && matrix)
	{
		m_vRenderActor[cont]->SetWorldMatrix(*matrix);
	}
}

void Rtw3DView::SetActorMatrix(CRT_ActorInstance *m_pActor,RtgMatrix12* matrix)
{
	if (!m_pActor || !matrix) return;
	size_t i = 0;
	for (i=0;i<m_vCreateActor.size();i++)
	{
		if (m_vCreateActor[i] == m_pActor)
		{
			m_vRenderActor[i]->SetWorldMatrix(*matrix);
		}
	}
}

void Rtw3DView::Run(DWORD dwDelta)
{
	RtwWidget::Run(dwDelta);

	size_t i = 0;
	for (i=0;i<m_vRenderActor.size();i++)
	{
		if (m_vRenderActor[i])
		{
			m_vRenderActor[i]->Tick(dwDelta);
		}
	}
}

void Rtw3DView::Render( const RtwRect* pClipRect/* = NULL*/, bool bAlwaysRender/* = false*/)
{
	RtwWidget::Render(pClipRect,bAlwaysRender);

	RtwRect rcRender = m_rcFrame;
	if (pClipRect)
		rcRender.ClipBy(*pClipRect); 

	if (m_reLastFrame.left != m_rcFrame.left || m_reLastFrame.top != m_rcFrame.top || 
		m_reLastFrame.right	!= m_rcFrame.right - m_rcFrame.left || m_reLastFrame.bottom != m_rcFrame.bottom - m_reLastFrame.top)
	{
		m_reLastFrame.left  = m_rcFrame.left;
		m_reLastFrame.top  = m_rcFrame.top;
		m_reLastFrame.right	= m_rcFrame.right - m_rcFrame.left;
		m_reLastFrame.bottom	= m_rcFrame.bottom - m_rcFrame.top;
		m_bRefreshCam = true;
	}

	
	if (m_pCameraActor && m_LastActorMatrix != *m_pCameraActor->GetMatrix())
	{
		m_bRefreshCam = true;
		m_LastActorMatrix = *m_pCameraActor->GetMatrix();
	}

	if (m_pCameraActor && m_bRefreshCam )
	{
		RtwRect rcDevicerView = g_workspace.getRenderDevice()->getViewportRect();
		float fTempX = 0.f,fTempY = 0.f;
		if (m_reLastFrame.left < rcDevicerView.left)
		{
			fTempX = rcDevicerView.left - m_reLastFrame.left;
			m_reLastFrame.left = rcDevicerView.left;
			m_reLastFrame.right -= fTempX;
			fTempX /= 7.f;
		}
		if (m_reLastFrame.top < rcDevicerView.top)
		{
			fTempY = rcDevicerView.top - m_reLastFrame.top;
			m_reLastFrame.top = rcDevicerView.top;
			m_reLastFrame.bottom -= fTempY;
			fTempY /= 7.f;
		}
		if (m_reLastFrame.left + m_reLastFrame.right > rcDevicerView.right)
		{
			fTempX = rcDevicerView.right - (m_reLastFrame.left + m_reLastFrame.right);
			m_reLastFrame.right = m_reLastFrame.right + fTempX;
			//m_reLastFrame.left -= fTempX;
			fTempX /= 7.f;
		}
		if (m_reLastFrame.top + m_reLastFrame.bottom > rcDevicerView.bottom)
		{
			fTempY = rcDevicerView.bottom - (m_reLastFrame.bottom + m_reLastFrame.top);
			m_reLastFrame.bottom = m_reLastFrame.bottom + fTempY;
			//m_reLastFrame.top -= fTempY;
			fTempY /= 7.f;
		}
		m_ViewCam.SetProjParams(1.f, (float)m_reLastFrame.right / (float)m_reLastFrame.bottom, 1, 1000);
		m_ViewCam.SetUpOffset(m_fUpOffset);

		RtgVertex3 Vertex3 = m_pCameraActor->GetWorldMatrix()->Position();
		RtgAABB bound = *m_pCameraActor->GetStaticBoundingBox();
		Vertex3.z += bound.vExt.z;

		float fForwardTemp = max(max(bound.vExt.z,bound.vExt.y),bound.vExt.x);
		if (fabs(fForwardTemp - bound.vExt.y) < 0.000001f)
		{
			fForwardTemp = fabs(fForwardTemp * fcos(m_fIncreaseX)) / 0.47f;
		}
		else
		{
			fForwardTemp = fForwardTemp / 0.42f;
		}
		//if ((fabs(bound.vExt.y * fcos(m_fIncreaseX)) / bound.vExt.z) > (min(m_reLastFrame.right,m_reLastFrame.bottom) / max(m_reLastFrame.right,m_reLastFrame.bottom)))
		//{
		//	fForwardTemp *= max((float)m_reLastFrame.right,(float)m_reLastFrame.bottom) / min((float)m_reLastFrame.right,(float)m_reLastFrame.bottom);
		//}

		RtgMatrix12 _mat = *m_pCameraActor->GetMatrix();
		float ca = facos(_mat._00);
		float sa = fasin(_mat._01);

		if (sa < 0.f)
		{
			ca = -ca;
		}

		float fYawTemp = ca ;

		m_ViewCam.SetViewParams(Vertex3,Vertex3,RtgVertex3(0.f,0.f,1.f));

		m_ViewCam.AddYaw(m_fIncreaseX - fYawTemp);
		m_ViewCam.AddRoll(m_fIncreaseY);
		m_ViewCam.MoveForward(m_fCameraCurrentDistance + fForwardTemp);
		m_ViewCam.SetRight(fTempX);
		m_ViewCam.SetUp(-fTempY);

		m_ViewCam.UpdateMatrix();
		m_bRefreshCam = m_pCameraActor->ResourceReady() ? false : true;
	}
	else if (!m_pCameraActor && !m_vRenderActor.empty())
	{
		m_pCameraActor = m_vRenderActor[0];
	}

	g_workspace.getRenderDevice()->BeginRenderViewPort(&m_ViewCam,m_reLastFrame.left,m_reLastFrame.top,m_reLastFrame.right,m_reLastFrame.bottom,0.f,1.f);

	size_t i = 0;
	for (i=0;i<m_vRenderActor.size();i++)
	{
		if (m_vRenderActor[i])
		{
			int renderOtherMatrix = 0;
			do
			{
				if (renderOtherMatrix && m_vViewMatrixOther[i][renderOtherMatrix])
					m_vRenderActor[i]->SetWorldMatrix(*m_vViewMatrixOther[i][renderOtherMatrix]);
				renderOtherMatrix ++;

				m_vRenderActor[i]->SetBaseColor(RtgVertex3(1.f,1.f,1.f));
				g_workspace.getRenderDevice()->RenderActorInstance(m_vRenderActor[i],RTGRM_NOBLENDOBJ_ONLY);
				g_workspace.getRenderDevice()->RenderActorInstance(m_vRenderActor[i],RTGRM_BLENDOBJ_ONLY);
			}
			while (renderOtherMatrix < m_vViewMatrixOther[i].size());
		}
	}

	g_workspace.getRenderDevice()->EndRenderViewPort();
}

void Rtw3DView::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);
}

bool Rtw3DView::CloneFrom(const RtwWidget* pWidget)
{
	return true;
}

void Rtw3DView::SetYaw(float fIncreaseX)
{
	if (!m_bCanYaw) return;
	m_fIncreaseX += DegreeToRadian(fIncreaseX) / m_fMouseMoveSpeed;
	m_bRefreshCam = true;
}

void Rtw3DView::SetRoll(float fIncreaseY)
{
	if (!m_bCanRoll) return;
	float tmpRoll = (-fIncreaseY)/m_fMouseMoveSpeed;
	if(m_fNowRollY + tmpRoll > m_fMaxRoll)
	{
		m_fIncreaseY += DegreeToRadian(m_fMaxRoll - m_fNowRollY);
		m_fNowRollY = m_fMaxRoll;
	}
	else if(m_fNowRollY + tmpRoll < m_fMinRoll)
	{
		m_fIncreaseY += DegreeToRadian(m_fMinRoll - m_fNowRollY);
		m_fNowRollY = m_fMinRoll;
	}
	else
	{
		m_fIncreaseY += DegreeToRadian(tmpRoll);
		m_fNowRollY += tmpRoll;
	}
	m_bRefreshCam = true;
}

void Rtw3DView::SetForward(float fAmount)
{
	if (!m_bCanForward) return;
	m_fAmount = -fAmount*m_fMouseWheelSpeed;
	m_fCameraCurrentDistance += m_fAmount;
	if (m_fCameraCurrentDistance < m_fCameraMinDistance || m_fCameraCurrentDistance > m_fCameraMaxDistance)
	{
		m_fCameraCurrentDistance -= m_fAmount;
	}
	m_bRefreshCam = true;
}

void Rtw3DView::SetCameraForwardMinMax(float fMin,float fMax)
{
	m_fCameraMinDistance = fMin;
	m_fCameraMaxDistance = fMax;
}

void Rtw3DView::SetCanForward(bool bForward)
{
	m_bCanForward = bForward;
}

void Rtw3DView::SetCanYaw(bool bYaw)
{
	m_bCanYaw = bYaw;
}

void Rtw3DView::SetCanRoll(bool bRoll)
{
	m_bCanRoll = bRoll;
}

bool Rtw3DView::SetActorPlayPose(int cont,char *name,bool bLoop,float fSpeed)
{
	if (cont < m_vRenderActor.size() && name)
	{
		return m_vRenderActor[cont]->PlayPose(name,bLoop,fSpeed);
	}
	return false;
}

CRT_ActorInstance* Rtw3DView::GetActorByCont(int cont)
{
	if (cont < m_vRenderActor.size())
	{
		return m_vRenderActor[cont];
	}
	return NULL;
}

void Rtw3DView::SetCamera(RtgCameraD3DView ViewCam)
{
	m_ViewCam = ViewCam;
}

RtgCameraD3DView* Rtw3DView::GetCamera()
{
	return &m_ViewCam;
}

void Rtw3DView::ReleaseActor(int cont)
{
	if (cont < m_vRenderActor.size())
	{
		if (m_pCameraActor == m_vRenderActor[cont])
			m_pCameraActor = NULL;
		Safe_ReleaseActor(m_vRenderActor[cont]);
		m_vRenderActor[cont] = NULL;
		m_vViewMatrixOther[cont].clear();
	}
}
void Rtw3DView::ReleaseAllActor()
{
	size_t i = 0;
	for (i=0;i<m_vCreateActor.size();i++)
	{
		if (m_vCreateActor[i])
		{
			Safe_ReleaseActor(m_vCreateActor[i]);
			m_vCreateActor[i] = NULL;
		}
	}
	m_vCreateActor.clear();
	m_vRenderActor.clear();
	m_vViewMatrixOther.clear();
	m_pCameraActor = NULL;
}
void Rtw3DView::SetUpOffset(float fUpOffset)
{
	m_fUpOffset = fUpOffset;
	m_bRefreshCam = true;
}

void Rtw3DView::RefreshView()
{
	m_bRefreshCam = true;
}

void Rtw3DView::ResetView()
{
	ReleaseAllActor();

	m_fCameraCurrentDistance  = m_fCameraDefaultDistance;
	m_fNowRollY = 10.f;

	m_fIncreaseX = DegreeToRadian(m_fDefYawDegree);
	m_fIncreaseY = DegreeToRadian(m_fDefRollDegree);
	m_fAmount = 0.f;

	m_bRefreshCam = true;
}

void Rtw3DView::AddRenderActorMatrix(int cont,RtgMatrix12* matrix)
{
	if (cont < m_vViewMatrixOther.size())
	{
		m_vViewMatrixOther[cont].push_back(matrix);
	}
}
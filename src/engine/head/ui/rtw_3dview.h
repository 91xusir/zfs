#ifndef _RTUI_3DVIEW_H
#define _RTUI_3DVIEW_H

class CRT_ActorInstance;
namespace ui 
{
	class Rtw3DView : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(Rtw3DView, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

		struct ViewMatrixVector 
		{
			std::vector<RtgMatrix12 *> vpMatrix;
		};

	public:
		Rtw3DView();
		virtual ~Rtw3DView();

		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);

		virtual void Show();
		virtual void Hide();

		virtual void Run(DWORD dwDelta);
		virtual void Render( const RtwRect* pClipRect = NULL, bool bAlwaysRender = false);

	public:
		void SetShowCloseButton(bool bShow);

		int AddCreateActor(char *filename,RtgMatrix12* matrix = NULL);
		int AddRenderActor(CRT_ActorInstance *m_pActor,RtgMatrix12* matrix = NULL);
		void SetActorMatrix(int cont,RtgMatrix12* matrix);
		void SetActorMatrix(CRT_ActorInstance *m_pActor,RtgMatrix12* matrix);

		CRT_ActorInstance* GetActorByCont(int cont);

		void AddRenderActorMatrix(int cont,RtgMatrix12* matrix);

		void ReleaseActor(int cont);
		void ReleaseAllActor();

		bool SetActorPlayPose(int cont,char *name,bool bLoop = false ,float fSpeed = 1.f);

		void SetCamera(RtgCameraD3DView ViewCam);
		RtgCameraD3DView* GetCamera() ;

		void SetYaw(float fIncreaseX);
		void SetRoll(float fIncreaseY);
		void SetForward(float fAmount);

		void SetCameraForwardMinMax(float fMin,float fMax);
		void SetCanForward(bool bForward);
		void SetCanYaw(bool bYaw);
		void SetCanRoll(bool bRoll);

		void SetDefYawDegree(float fAmount);
		void SetDefRollDegree(float fAmount);
		void SetDefForward(float fAmount);
		void SetUpOffset(float fUpOffset);

		void RefreshView();
		void ResetView();

	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove | wfDClick;

	protected:
		RtwButton* m_pBtn_Close;
		std::vector<CRT_ActorInstance*>	m_vCreateActor;
		std::vector<CRT_ActorInstance*>	m_vRenderActor;
		RtgCameraD3DView m_ViewCam;
		CRT_ActorInstance *m_pCameraActor;

		float m_fIncreaseX;
		float m_fIncreaseY;
		float m_fAmount;

		float m_fMaxRoll;
		float m_fMinRoll;

		float m_fNowRollY;

		float m_fMouseMoveSpeed;
		float m_fMouseWheelSpeed;

		float m_fCameraMinDistance      ;
		float m_fCameraMaxDistance      ;
		float m_fCameraDefaultDistance  ;
		float m_fCameraCurrentDistance  ;
		float m_fUpOffset;

		float m_fDefYawDegree;
		float m_fDefRollDegree;

		bool m_bCanForward;
		bool m_bCanYaw;
		bool m_bCanRoll;

		bool m_bRefreshCam;

		RtwRect m_reLastFrame;
		RtgMatrix12	m_LastActorMatrix;
		std::vector<std::vector<RtgMatrix12*>> m_vViewMatrixOther;

	private:
		void _ResetSelf();
		void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseWheel_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_Close(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	};
} // namespace ui
#endif // _RTUI_3DVIEW_H
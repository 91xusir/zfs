#include "gc_include.h"
#include "UIForm_System.h"

#define m_fSTEP 6.f

CUIForm_System::CUIForm_System(void):
m_pfrmSystem(NULL),
m_pbtnShow_CloseButton(NULL),
m_bIsRunning(false),
m_fStep(0.f),
m_bShow(false)
{
	guard;
	m_pfrmSystem = 	RTW_WIDGET("layworld.fmsystem");
	m_pbtnShow_CloseButton = RTW_WIDGET_T(RtwButton,"layworld.fmsystem.btnsyschar");
	RTW_WIDGET("fmclose.btnclose")->EvLClick += RTW_CALLBACK(this, CUIForm_System, Show_Close);
	RTW_WIDGET("fmclose.btnopen")->EvLClick += RTW_CALLBACK(this, CUIForm_System, Show_Close);
	unguard;
}

CUIForm_System::~CUIForm_System(void)
{
	guard;
	unguard;
}

void CUIForm_System::Show()
{
	guard;
	m_pfrmSystem->Show();
	unguard;
}
void CUIForm_System::Hide()
{
	guard;
	m_pfrmSystem->Hide();
	unguard;
}

void CUIForm_System::Show_Close(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(m_bShow)
	{
		m_bShow = false;
		m_bIsRunning = true;
		m_fStep = -m_fSTEP;
		RTW_WIDGET("fmclose.btnclose")->Show();
		RTW_WIDGET("fmclose.btnopen")->Hide();
	}else
	{
		m_bShow = true;
		m_bIsRunning = true;
		m_fStep = m_fSTEP;
		RTW_WIDGET("fmclose.btnclose")->Hide();
		RTW_WIDGET("fmclose.btnopen")->Show();
	}
	unguard;
}

void CUIForm_System::RunAnimation()
{
	guard;
	RECT winRect;
	::GetClientRect((HWND)GetDevice()->GetWndHandle(),&winRect);
	static RtwRect rectBaseRect = m_pfrmSystem->getFrameRect();
	static float width = rectBaseRect.getWidth();
	rectBaseRect.right = winRect.right;
	rectBaseRect.left  = winRect.right - width;
	if(m_bIsRunning)
	{
		if(m_fStep == m_fSTEP)// 向右移动
		{
			RtwRect rectNowRect = m_pfrmSystem->getFrameRectRef();
			if(rectNowRect.left < rectBaseRect.right - 1)
			{
				rectNowRect.left += m_fStep;
				rectNowRect.right += m_fStep;
				m_pfrmSystem->Move(SPoint(rectNowRect.left,rectNowRect.top));
			}else
			{
				m_bIsRunning = false;
				m_fStep = 0;
			}
		}else if(m_fStep = -m_fSTEP)// 向左移动
		{
			RtwRect rectNowRect = m_pfrmSystem->getFrameRectRef();
			if(rectNowRect.left > rectBaseRect.left + 1)
			{
				rectNowRect.left += m_fStep;
				rectNowRect.right += m_fStep;
				m_pfrmSystem->Move(SPoint(rectNowRect.left,rectNowRect.top));
			}else
			{
				m_bIsRunning = false;
				m_fStep = 0;
			}
		}
	}
	unguard;
}
void CUIForm_System::DoFrame()
{
	guard;
 	RunAnimation();
	unguard;
}

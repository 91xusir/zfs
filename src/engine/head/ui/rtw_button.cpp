#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_button.cpp.
//	author:		ldr123
//	purpose:	按钮
//  other:		局部重构添加注释
//********************************************************************

//普通按钮
RT_IMPLEMENT_DYNAMIC(RtwButton, RtwWidget, 0, "ui")

//选择按钮
RT_IMPLEMENT_DYNAMIC(CUiCheckButton, RtwButton, 0, "ui")

RT_IMPLEMENT_DYNAMIC(RtwAliasButton, RtwButton, 0, "ui")

//用来存储多选按钮队列,用以实现radiobutton的功能
std::map< std::string, std::vector<int> > g_mapStrVecCheckButton;

RtwButton::RtwButton()
:m_bNeedSelect(false),m_bSelected(false),m_bBlink(false),m_sBlinkInterval(500),m_bPushed(false),
m_pImgNormal(NULL), m_pImgHighlight(NULL), m_pImgPushed(NULL), m_pImgDisabled(NULL),
m_pImgMask(NULL), m_nForceDrawBackImage(NO_FORCE)
{
	m_bIsUseSystemFont    = true;
}

RtwButton::~RtwButton(){}

void RtwButton::OnBeforeDestroy()
{
	_ResetSelf();
	RtwWidget::OnBeforeDestroy();
}

void RtwButton::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwButton::_ResetSelf()
{
	m_WidgetType	= wtButton;
	m_Flag			= m_DefaultFlag;
	m_rcClient		= RtwRect(0, 0, 30, 10);
	m_rcFrame		= RtwRect(0, 0, 30, 10);
	m_TextAlign		= SUiTextAlignment(alignCenter, alignCenter);//默认居中

	DROP_TRUI_IMAGE(m_pImgNormal);
	DROP_TRUI_IMAGE(m_pImgHighlight);
	DROP_TRUI_IMAGE(m_pImgPushed);
	DROP_TRUI_IMAGE(m_pImgDisabled);
	DROP_TRUI_IMAGE(m_pImgMask);
}

bool RtwButton::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();
	return true;
}

//设置按钮各状态下的图片
void RtwButton::SetButtonStateImages(RtwImage* pNormal, RtwImage* pHighlight, RtwImage* pPushed, RtwImage* pDisabled)
{
	SetNormalImage(pNormal);
	SetHoverImage(pHighlight);
	SetPushedImage(pPushed);
	SetDisabledImage(pDisabled);
}

//正常
void RtwButton::SetNormalImage(RtwImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgNormal);

	if (pImg)
	{
		m_pImgNormal = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//高亮(鼠标在上面覆盖的情况)
void RtwButton::SetHighlightImage(RtwImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgHighlight);

	if (pImg)
	{
		m_pImgHighlight = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//遮罩
void RtwButton::SetMaskImage( RtwImage* pImg )
{
	DROP_TRUI_IMAGE(m_pImgMask);
	if (pImg)
	{
		m_pImgMask = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//按下
void RtwButton::SetPushedImage(RtwImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgPushed);

	if (pImg)
	{
		m_pImgPushed = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//禁用
void RtwButton::SetDisabledImage(RtwImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgDisabled);

	if (pImg)
	{
		m_pImgDisabled = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//选中
void RtwButton::SetSelected(bool bSelected)
{
	//如果不可选择,则强制改为可选择
	if(!m_bNeedSelect)
		m_bNeedSelect=true;

	m_bSelected=bSelected;
}

void RtwButton::SetBlink(bool bBlink)
{
	m_bBlink=bBlink;
}

void RtwButton::SetBlinkInterval(short sBlinkInterval)
{
	m_sBlinkInterval=sBlinkInterval;
}

//强制按下
void RtwButton::SetPushed(bool bPushed)
{
	m_bPushed = bPushed;
}

//与SetHighlightImage功能一样
void RtwButton::SetHoverImage(RtwImage* pImg)
{
	SetHighlightImage(pImg);
}

void RtwButton::SetHoverImage(RtwComplexImage cimage){}

//图片的动态切换(用于gif等动态图片)
void RtwButton::Run(DWORD dwDelta)
{
	RtwWidget::Run(dwDelta);
	IMAGETICK(m_pImgNormal,		dwDelta);
	IMAGETICK(m_pImgHighlight,	dwDelta);
	IMAGETICK(m_pImgPushed,		dwDelta);
	IMAGETICK(m_pImgDisabled,	dwDelta);
	IMAGETICK(m_pImgMask,		dwDelta);
}

void RtwButton::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	// 前景图
	if (m_pImgNormal)
	{
		if(m_bNeedSelect)
		{
			double dTransparency;
			dTransparency=0.5; //可选状态,但未选择的时候,透明度为50%,用以和其他按钮区分
			if(m_bSelected)
			{
				dTransparency=1;	
			}
			//设置透明度
			m_pImgNormal->getRenderImage()->SetTransparency(dTransparency);
		}
		m_pImgNormal->getRenderImage()->SetCtrlTransparent(m_Transparency);
		g_workspace.getRenderDevice()->DrawImage(
			m_pImgNormal->getRenderImage(), m_rcFrame, 
			SUiTextAlignment(alignNear, alignNear), pClipRect);
	}

	RtwImage *pDrawImage = NULL;

	if (getEnable() /*&& NULL == pDrawImage*/ )//状态可用
	{
		if (g_workspace.getMouseCapture() == this)//
		{
			if ( /*NULL == pDrawImage &&*/ m_pImgPushed && m_pImgPushed->getRenderImage())
			{
				pDrawImage = m_pImgPushed;
			}
		}
		else if (g_workspace.getMouseHover() == this)
		{
			if ( /*NULL == pDrawImage &&*/ m_pImgHighlight && m_pImgHighlight->getRenderImage())
			{
				pDrawImage = m_pImgHighlight;			
			}
		}
	}
	else 
	{
		if ( /*NULL == pDrawImage &&*/ m_pImgDisabled && m_pImgDisabled->getRenderImage())//禁用
		{
			pDrawImage = m_pImgDisabled;
		}
	}

	//在有强制显示某种状态的图片情况下,根据状态判断显示的结果无效
	if( m_nForceDrawBackImage == FORCE_DRAW_HIGHLIGHTIMAGE ) pDrawImage = m_pImgHighlight;
	if( m_nForceDrawBackImage == FORCE_DRAW_PUSHEDIMAGE    ) pDrawImage = m_pImgPushed;
	if( m_nForceDrawBackImage == FORCE_DRAW_DISABLEIMAGE   ) pDrawImage = m_pImgDisabled;

	RtwWidget::DrawForeground(pClipRect);

	if (pDrawImage)
	{//绘制按钮图片
		RtwImage *pImage = pDrawImage->getRenderImage();
		if (pImage)
		{
			pImage->SetCtrlTransparent(m_Transparency);
		}
		g_workspace.getRenderDevice()->DrawImage(pImage, m_rcClient,SUiTextAlignment(alignNear, alignNear), pClipRect);
	}

	if( m_bIsUseSystemFont && m_FontType.empty() )
	{//是否使用系统字体
		g_workspace.getFontManager()->DrawString(m_Text, m_rcFrame, SETCOLORALPHA(m_TextColor, m_Transparency), 
			g_workspace.getFontManager()->getDefaultFont() ,false, m_TextAlign, false, pClipRect );
	}else
	{
		// 按钮文字
		RtwPixel textColor = SETCOLORALPHA(m_TextColor, m_Transparency);
		g_workspace.getFontManager()->DrawString(m_Text, m_rcFrame,textColor , false, m_TextAlign,
			false, pClipRect,m_FontType,m_FontSize);
	}

	//有边框不?
	if (m_BorderSize)
	{
		if (m_BorderColor != 0x00000000)
		{
			RtwPixel BorderColor = SETCOLORALPHA(m_BorderColor, m_Transparency);
			g_workspace.getRenderDevice()->DrawRectangleBorder(m_rcFrame, BorderColor, pClipRect);
		}
	}

	//绘制遮罩
	if( m_pImgMask )
	{
		m_pImgMask->getRenderImage()->SetCtrlTransparent(m_Transparency);
		g_workspace.getRenderDevice()->DrawImage( m_pImgMask->getRenderImage(), m_rcClient, 
			SUiTextAlignment(alignNear, alignNear),	pClipRect);
	}
}

//绘制背景
void RtwButton::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool RtwButton::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwButton::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwButton* pOther = (RtwButton*)pWidget;

	DROP_TRUI_IMAGE(m_pImgNormal);
	m_pImgNormal = pOther->m_pImgNormal;
	if (m_pImgNormal)
		GRAB_TRUI_IMAGE(m_pImgNormal);

	DROP_TRUI_IMAGE(m_pImgHighlight);
	m_pImgHighlight = pOther->m_pImgHighlight;
	if (m_pImgHighlight)
		GRAB_TRUI_IMAGE(m_pImgHighlight);

	DROP_TRUI_IMAGE(m_pImgPushed);
	m_pImgPushed = pOther->m_pImgPushed;
	if (m_pImgPushed)
		GRAB_TRUI_IMAGE(m_pImgPushed);

	DROP_TRUI_IMAGE(m_pImgDisabled);
	m_pImgDisabled = pOther->m_pImgDisabled;
	if (m_pImgDisabled)
		GRAB_TRUI_IMAGE(m_pImgDisabled);

	return true;
}

void RtwButton::CalcClient()
{
	m_rcClient = m_rcFrame;
}

void RtwButton::OnRemoveChild(RtwWidget* pWidget){}

//按钮是否是混合渲染模式
void RtwButton::EnableAlphaEffect(bool bAlpha)
{
	if (m_pBackGround && m_pBackGround->getRenderImage())
	{
		m_pBackGround->SetBlend(bAlpha);
	}
}

void RtwButton::LoadAttributesFromTemplate(std::string namge){}


//多选按钮(通过radioid可以实现单选钮的功能)
CUiCheckButton::CUiCheckButton():
m_pImgChecked(NULL), m_pImgCheckHighlight(NULL), m_bChecked(false), m_strRadioID("-1"),
m_NormalImage(NULL), m_HightImage(NULL), m_PushImage(NULL)
{
	m_bIsUseSystemFont				= true;
}

CUiCheckButton::~CUiCheckButton(){}

void CUiCheckButton::OnBeforeDestroy()
{
	_ResetSelf();
	RtwButton::OnBeforeDestroy();
}

void CUiCheckButton::Reset()
{
	RtwButton::Reset();
	_ResetSelf();
}

void CUiCheckButton::_ResetSelf()
{
	m_WidgetType		= wtCheckButton;
	m_Flag				= m_DefaultFlag;
	m_rcClient			= RtwRect(0, 0, 30, 10);
	m_rcFrame			= RtwRect(0, 0, 30, 10);
	m_TextAlign			= SUiTextAlignment(alignCenter, alignCenter);
	m_bCheckStyleLayout = false;
	m_strRadioID		= "-1";
	SetCheckStyleLayout(false);
}

//位移
void CUiCheckButton::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	CalcClient();
}

bool CUiCheckButton::Create(DWORD Id)
{
	if (!RtwButton::Create(Id))
		return false;

	_ResetSelf();

	return true;
}

//图片的动态切换(用于gif等动态图片)
void CUiCheckButton::Run(DWORD dwDelta)
{
	RtwButton::Run(dwDelta);
	IMAGETICK(m_pImgChecked,	dwDelta);
	IMAGETICK(m_HightImage,		dwDelta);
	IMAGETICK(m_PushImage,		dwDelta);
	IMAGETICK(m_NormalImage,	dwDelta);
	IMAGETICK(m_pImgCheckHighlight,	dwDelta);
}

void CUiCheckButton::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	if (m_bChecked && m_pImgChecked && getEnable())
	{	
		RtwRect rect	=	m_rcCheckImage;
		rect.top		+=	m_pImgChecked->borderSize.top;
		rect.left		+=	m_pImgChecked->borderSize.left;
		rect.bottom		-=	m_pImgChecked->borderSize.bottom;
		rect.right		-=	m_pImgChecked->borderSize.right;

		if (m_pImgChecked->getRenderImage())
		{
			m_pImgChecked->getRenderImage()->SetCtrlTransparent(m_Transparency);
			g_workspace.getRenderDevice()->DrawImage(m_pImgChecked->getRenderImage(), 
				rect, SUiTextAlignment(alignNear, alignNear), pClipRect);
		}

		if (m_pImgChecked->border)
		{
			m_pImgChecked->border->SetCtrlTransparent(m_Transparency);
			g_workspace.getRenderDevice()->DrawBorderImage(m_pImgChecked->border,m_rcCheckImage,
				m_pImgChecked->borderSize,SUiTextAlignment(alignNear, alignNear), pClipRect);
		}
	}
	else 
	{
		/*
			修正checkbutton各态显示问题
		*/
		RtwImage *pDrawImage = NULL;
		if (getEnable())
		{
			if (g_workspace.getMouseCapture() == this)	//鼠标点击
			{
				if(m_pImgPushed)
				{
					pDrawImage = m_pImgPushed;
				}
				else
				{
					pDrawImage = m_PushImage;
				}
			}
			else if (g_workspace.getMouseHover() == this)	//鼠标覆盖(高亮,mousemove等事件)
			{
				if(m_pImgCheckHighlight)
				{
					if (m_pImgCheckHighlight->getRenderImage())
					{
						m_pImgCheckHighlight->getRenderImage()->SetCtrlTransparent(m_Transparency);
						g_workspace.getRenderDevice()->DrawImage(m_pImgCheckHighlight->getRenderImage(), 
							m_rcCheckImage, SUiTextAlignment(alignNear, alignNear), pClipRect);
					}

					if (m_pImgCheckHighlight->border)
					{
						m_pImgCheckHighlight->border->SetCtrlTransparent(m_Transparency);
						g_workspace.getRenderDevice()->DrawBorderImage(m_pImgCheckHighlight->border,
							m_rcCheckImage,m_pImgCheckHighlight->borderSize,
							SUiTextAlignment(alignNear, alignNear), pClipRect);
					}
				}
				else
				{
					pDrawImage = m_HightImage;
				}
			}
			else 
			{
				if(m_pImgNormal)
				{
					pDrawImage = m_pImgNormal;
				}
				else
				{
					pDrawImage=m_NormalImage;
				}
			}
		}
		else 
		{
			if (m_pImgDisabled)
				pDrawImage = m_pImgDisabled;
		}

		if (pDrawImage)
		{
			pDrawImage->getRenderImage()->SetCtrlTransparent(m_Transparency);
			g_workspace.getRenderDevice()->DrawImage(pDrawImage->getRenderImage(), m_rcCheckImage, 
				SUiTextAlignment(alignNear, alignNear), pClipRect);
		}
	}
	
	//通过指定字体显示文字
	if( m_bIsUseSystemFont && m_FontType.empty() )
	{
		g_workspace.getFontManager()->DrawString(m_Text, m_rcText, SETCOLORALPHA(m_TextColor, m_Transparency), 
			g_workspace.getFontManager()->getDefaultFont() ,
			false, m_TextAlign, false, pClipRect );
	}else
	{
		// 按钮文字
		g_workspace.getFontManager()->DrawString(m_Text, m_rcText, SETCOLORALPHA(m_TextColor, m_Transparency), 
			false, m_TextAlign, false, pClipRect, m_FontType, m_FontSize);
	}

	RtwWidget::DrawForeground(pClipRect);
}

void CUiCheckButton::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwButton::DrawBackground(pClipRect);
}

bool CUiCheckButton::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(CUiCheckButton::GetRuntimeClass()));

	UI_ENSURE_B(RtwButton::CloneFrom(pWidget));
	
	CUiCheckButton* pOther = (CUiCheckButton*)pWidget;
	m_bCheckStyleLayout = pOther->m_bCheckStyleLayout;
	m_rcText = pOther->m_rcText;

	return true;
}

//选中
void CUiCheckButton::SetCheckedImage(RtwComplexImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgChecked);

	if (pImg)
	{
		m_pImgChecked = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//高亮
void CUiCheckButton::SetHighlightImage(RtwComplexImage* pImg)
{
	DROP_TRUI_IMAGE(m_pImgCheckHighlight);

	if (pImg)
	{
		m_pImgCheckHighlight = pImg;
		GRAB_TRUI_IMAGE(pImg);
	}
}

//选中
void CUiCheckButton::SetChecked(bool bChecked)
{
	m_bChecked = bChecked;
}

void CUiCheckButton::SetCheckStyleLayout(bool bCheckStyle)
{
	m_bCheckStyleLayout = bCheckStyle; //文字在选择框右侧
	CalcClient();
}

void CUiCheckButton::CalcClient()
{
	m_rcClient = m_rcFrame;

	//文字在选择框右侧
	if (m_bCheckStyleLayout)
	{
		m_rcCheckImage.left		= m_rcClient.left;
		m_rcCheckImage.right	= m_rcCheckImage.left + m_rcClient.getHeight();
		m_rcCheckImage.top		= m_rcClient.top;
		m_rcCheckImage.bottom	= m_rcClient.bottom;
		m_rcText.left			= m_rcCheckImage.right;
		m_rcText.right			= m_rcClient.right;
		m_rcText.top			= m_rcClient.top;
		m_rcText.bottom			= m_rcClient.bottom;
	}
	else 
	{
		m_rcCheckImage	= m_rcClient;
		m_rcText		= m_rcClient;
	}
}

//设置单选钮组的id
void CUiCheckButton::setRadioID(string strRadioID)
{
	if (strRadioID.empty())
	{
		return;
	}

	if (strRadioID == "-1")
	{
		return;
	}

	if (strRadioID == m_strRadioID)
	{
		return;
	}

	//搜索指定id
	iterMapStrVec iterMap = g_mapStrVecCheckButton.find(m_strRadioID);

	if (iterMap != g_mapStrVecCheckButton.end())
	{
		vector<int>::iterator vci = iterMap->second.begin();
		for (; vci!=iterMap->second.end(); ++vci)
		{
			if ((*vci) == m_Id) //删除已经存在的id
			{
				vci = iterMap->second.erase(vci);
			}
		}
	}
	g_mapStrVecCheckButton[strRadioID].push_back(m_Id);
	m_strRadioID = strRadioID;
	UpdateRadioButton();
}

void CUiCheckButton::UpdateRadioButton()
{
	iterMapStrVec itermap = g_mapStrVecCheckButton.find(m_strRadioID);
	if (itermap != g_mapStrVecCheckButton.end())
	{
		CUiCheckButton *pCheckButton;
		for (vector<int>::iterator vci=itermap->second.begin(); vci!=itermap->second.end(); ++vci)
		{
			if (
				g_workspace.getWidgetFactory()->FindWidget(*vci,(RtwWidget**)&pCheckButton) //根据控件id找到控件
				&& (pCheckButton->m_Id != m_Id)	//控件id不是自己id
				&& pCheckButton->getEnable()		//控件好用不?
				)
			{
				pCheckButton->SetChecked(false);
			}
			else
			{
				SetChecked(true);
			}
		}
	}
}

//鼠标单击
void CUiCheckButton::OnMouseClick(const RtwEventDelegate& e)
{
	if (getEnable())
	{
		if (e.mouse.button == buttonLeft)	//左键
		{
			if (m_strRadioID != "-1") //默认分组时-1表示不属于radiobutton,否则就要find看有没有同组的button
			{
				UpdateRadioButton();
			}
			else
			{
				//不是radiobutton,设置状态为当前状态的逆状态
				SetChecked(!GetChecked());
			}
		}else if (e.mouse.button == buttonRight)
		{
			EvRClick(this, e);
		}
		EvLClick(this, e);
	}
}

void CUiCheckButton::OnRemoveChild(RtwWidget* pWidget){}

RtwAliasButton::RtwAliasButton():m_pRealBtn(NULL){}

RtwAliasButton::~RtwAliasButton()
{
	m_pRealBtn = NULL;
}

void RtwAliasButton::OnBeforeDestory()
{
	m_pRealBtn = NULL;
	RtwButton::OnBeforeDestroy();
}

void RtwAliasButton::CommandButton()
{
	if (m_pRealBtn)
	{
		RtwEventDelegate e;
		e.type			= weMouseClick;
		e.mouse.button	= buttonRight;
		m_pRealBtn->OnMouseClick(e);
	}
}

void RtwAliasButton::Reset()
{
	RtwButton::Reset();
	_ResetSelf();
}

bool RtwAliasButton::Create(DWORD Id)
{
	if (!RtwButton::Create(Id))
		return false;

	_ResetSelf();
	ModifyFlags(wfDragIn | wfDragOut | wfHint | wfVisible | wfDClick,0);
	this->EvDrag += RTW_CALLBACK(this, RtwAliasButton, OnDragIn_Aliasbtn);

	return true;
}

void RtwAliasButton::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	CalcClient();
}

void RtwAliasButton::DrawBackground(const RtwRect* pClipRect /* = NULL */){}

void RtwAliasButton::DrawForeground(const RtwRect* pClipRect /* = NULL */){}

bool RtwAliasButton::CloneFrom(const RtwWidget* pWidget){return false;}
void RtwAliasButton::OnRemoveChild(RtwWidget* pWidget){}

void RtwAliasButton::SetButton(const std::string strBtnName){}

void RtwAliasButton::SetButton(RtwButton* btn)
{
	if(m_pRealBtn)
	{
		RemoveChild(m_pRealBtn->getId());
		m_pRealBtn = NULL;
	}
	AddChild(btn);
	m_pRealBtn = btn;
	if(m_pRealBtn)
	{
		m_pRealBtn->Move(RtwRect(1,1,32,32));
		m_pRealBtn->EvDragEnd.ClearAndSetDelegate(RTW_CALLBACK(this, RtwAliasButton, OnDragOut_Aliasbtn));
	}
}

void RtwAliasButton::OnDragIn_Aliasbtn(RtwWidget* sender, RtwEventDelegate* e)
{
	RtwWidget* pDragSvr = g_workspace.GetMouseDragWidget();
	if (!pDragSvr) return;
	if (pDragSvr->getWidgetType() != wtButton)
		return;	// Current we only support button
	SetNormalImage(((RtwButton *)pDragSvr)->getNormalImage());
	SetButton((RtwButton *)pDragSvr);
	g_workspace.SetMouseDragWidget(NULL);
}

void RtwAliasButton::OnDragOut_Aliasbtn(RtwWidget* sender, RtwEventDelegate* e)
{
	if(g_workspace.getMouseHover() && 
		g_workspace.getMouseHover()->getWidgetType() == wtAliasButton &&
		m_pRealBtn == sender)
	{
		RemoveChild(sender->getId());
		if(((RtwAliasButton*)g_workspace.getMouseHover())->GetButton())
		{
			RtwButton* pButton = (RtwButton*)((RtwAliasButton*)g_workspace.getMouseHover())->GetButton();
			((RtwAliasButton*)g_workspace.getMouseHover())->SetButton(m_pRealBtn);
			m_pRealBtn = NULL;
			SetButton(pButton);
		}else
		{
			((RtwAliasButton*)g_workspace.getMouseHover())->SetButton(m_pRealBtn);
			m_pRealBtn->EvDragEnd -= RTW_CALLBACK(this,RtwAliasButton,OnDragOut_Aliasbtn);
			m_pRealBtn = NULL;
		}
		e->cancle = true;
		g_workspace.SetMouseDragWidget(NULL);
	}
	else
	{
		if(m_pRealBtn)
		{
			if (g_workspace.getMouseHover()&&g_workspace.getMouseHover()->GetParent())
			{
				if (g_workspace.getMouseHover()->GetParent()->getWidgetType()==wtAliasButton
					&&g_workspace.getMouseHover()!=g_workspace.getMouseCapture())
				{
					RemoveChild(sender->getId());
					e->del_me = true;
					if(((RtwAliasButton*)g_workspace.getMouseHover()->GetParent())->GetButton())
					{
						RtwButton* pButton = (RtwButton*)((RtwAliasButton*)g_workspace.getMouseHover()->GetParent())->GetButton();
						((RtwAliasButton*)g_workspace.getMouseHover()->GetParent())->SetButton(m_pRealBtn);
						m_pRealBtn = NULL;
						SetButton(pButton);
					}else
					{
						((RtwAliasButton*)g_workspace.getMouseHover()->GetParent())->SetButton(m_pRealBtn);
						m_pRealBtn->EvDragEnd -= RTW_CALLBACK(this,RtwAliasButton,OnDragOut_Aliasbtn);
						m_pRealBtn = NULL;
					}
					e->cancle = true;
					g_workspace.SetMouseDragWidget(NULL);
				}
			}else if(g_workspace.getMouseHover()==NULL)
			{
				RemoveChild(sender->getId());
				e->del_me = true;
				m_pRealBtn = NULL;
				e->cancle = true;
				g_workspace.SetMouseDragWidget(NULL);
			}
		}
	}
}

void RtwAliasButton::_ResetSelf()
{
	m_WidgetType	= wtAliasButton;
	m_Flag			= m_DefaultFlag;
	m_rcClient		= RtwRect(0, 0, 34, 34);
	m_rcFrame		= RtwRect(0, 0, 34, 34);
	m_TextAlign		= SUiTextAlignment(alignCenter, alignCenter);

	DROP_TRUI_IMAGE(m_pImgNormal);
	DROP_TRUI_IMAGE(m_pImgHighlight);
	DROP_TRUI_IMAGE(m_pImgPushed);
	DROP_TRUI_IMAGE(m_pImgDisabled);	
	DROP_TRUI_IMAGE(m_pImgMask);
}

#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.05.28 16:00
//	filename: 	rtw_hud.cpp
//	author:		ldr123
//	purpose:	hud
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwHUD, RtwWidget, 0, "ui")

//-----------------------------------------------------------------------------
//-				说话
//-				名字
//-				帮派
//-				血条
//-----------------------------------------------------------------------------
 long RtwHUD::s_cShowItem =	(long)RtwHUD::siSpeak|(long)RtwHUD::esiFaction|
(long)RtwHUD::esiHPBar|(long)RtwHUD::siMsg|
(long)RtwHUD::siName |(long)RtwHUD::siNick|
(long)RtwHUD::siGuild|(long)RtwHUD::esiStall|(long)RtwHUD::esiLogo/* |
(long)RtwHUD::siPetAbility|(long)RtwHUD::siPetFuseLev|(long)RtwHUD::siPetMaster*/;

bool RtwHUD::ename=1;
bool RtwHUD::espeak=1;

RtwHUD::RtwHUD(): 
					m_speakColor(0xffffffff), m_speakDuration(3), m_speakTimeStamp(0), m_bloodValue(50),
					m_cShowItem(0), m_bshow(true),m_pstallshow(false)
{
	m_WidgetType = wtHud;
	Init();
}

RtwHUD::~RtwHUD()
{
	if (m_pSpeakUI)		g_workspace.RemoveWidget(m_pSpeakUI->getId());
	if (m_pBloodBar)	g_workspace.RemoveWidget(m_pBloodBar->getId());
	if (m_pStall)		g_workspace.RemoveWidget(m_pStall->getId());
	if (m_partylogo)	g_workspace.RemoveWidget(m_partylogo->getId());

	DROP_RTUI_OBJECT(m_pSpeakUI);
	DROP_RTUI_OBJECT(m_pBloodBar);
	DROP_RTUI_OBJECT(m_pStall);
	DROP_RTUI_OBJECT(m_partylogo);
}

bool RtwHUD::Init()
{

	m_pSpeakUI	= (RtwTextBox*)g_workspace.getWidgetFactory()->createWidget(wtEditBox);
	m_pBloodBar = (RtwProgressBar*)g_workspace.getWidgetFactory()->createWidget(wtProgressBar);
	m_pStall	= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	m_partylogo	=(RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);

	g_workspace.AddWidget(m_pSpeakUI);
	g_workspace.AddWidget(m_pBloodBar);
	g_workspace.AddWidget(m_pStall);
	g_workspace.AddWidget(m_partylogo);

	m_pSpeakUI->Hide();
	m_pBloodBar->Hide();
	m_pStall->Hide();
	m_partylogo->Hide();

	RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui/x_textures/1.tga");
	if(pImage)
	{
		pImage->SetSourceRect(RtwRect(1, 216, 147,226));
		pImage->SetBlend(true);
	}

	m_pBloodBar->SetBarImage(pImage);
	DROP_RTUI_OBJECT(pImage);

	m_pSpeakUI->SetMultiLine(true);
	m_pSpeakUI->SetWordWrap(true);
	m_pSpeakUI->SetAutoScrollV(false);
	m_pSpeakUI->SetEnableInput(false);
	m_pSpeakUI->SetAutoSizeV(true);

	m_pSpeakUI->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	m_pSpeakUI->Move(RtwRect(0,0,200,32));
	m_pSpeakUI->Disable();

	m_pBloodBar->Move(RtwRect(0,0,30,3));
	m_pBloodBar->SetValue(1);

	m_pStall->Move(RtwRect(0,0,140,15));
	m_pStall->SetText("");
	m_pStall->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	m_pStall->SetTextColor(RtwPixel(0xff00ff00));
	m_szStall = "";
	m_partylogo->SetWidgetSize(SSize(12,12));
	m_partylogo->Move(RtwRect(0,0,12,12));
	m_partylogo->ModifyFlag(0,wfPick);

	ShowHPBar(false);
	ShowParty(true);	// 工会名称
	ShowName(true);		// 角色姓名
	ShowStall(false);

	m_name.SetSize(130,15);
	m_title.SetSize(160,15);
	m_title.color=0xff88aadd;
	m_pPartyUi.SetSize(130,15);
	m_pPartyUi.color=0xff22bb88;

	m_EditBoxPetAbility.SetSize(130,15);
	m_EditBoxPetFuseLev.SetSize(130,15);
	m_EditBoxPetMaster.SetSize(130,15);
	m_EditBoxPetMaster.color=0xFF63B8FF;

	return true;
}

void RtwHUD::Show()
{
	if(IsShowHPBar() && m_bshow)
	{
		m_pBloodBar->Show();
	}else
	{
		m_pBloodBar->Hide();
	}

	if(m_pstallshow)
	{
		m_pStall->Show();
	}
	else
	{
		m_pStall->Hide();
	}
}

void RtwHUD::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
	if(!m_bshow)
		return;

	if(IsShowPetMaster())
	{
		m_EditBoxPetMaster.Render(inDevice);
	}

	if(IsShowName())
	{
		m_name.Render(inDevice);
	}

	if(IsShowPetAbility())
	{
		m_EditBoxPetAbility.Render(inDevice);
	}

	if(IsShowPetFuseLev())
	{
		m_EditBoxPetFuseLev.Render(inDevice);
	}

	if(IsShowTitle())
	{
		m_title.Render(inDevice);
	}

	if (IsShowParty())
	{
		m_pPartyUi.Render(inDevice);
	}
}

void RtwHUD::OnHudFlagChanged(long newFlag)
{
	RtwHUD::s_cShowItem = newFlag;
}

void RtwHUD::SetPosition(RtgVertex3 pos)
{
	RtwRect rc;
	int x1=0, y1=0;
	int x2=0, y2=0;
	int x3=0, y3=0;
	int x4=0, y4=0;
	int x5=0, y5=0;
	int iOffset = 0;

	int x6=0, y6=0;
	int x7=0, y7=0;
	int x = 0,y = 0;

	if(IsShowHPBar())
	{
		rc = m_pBloodBar->GetFrameRect();
		x1 = pos.x - (rc.getWidth()>>1);
		y1 = pos.y - rc.getHeight();
		y1 -= iOffset;
		m_pBloodBar->Move(SPoint(x1, y1));

		iOffset += 3;
	}

	if(IsShowPetMaster() && !m_EditBoxPetMaster.text.empty())
	{
		x3 = pos.x;
		y3 = pos.y;
		y3 -= iOffset;
		m_EditBoxPetMaster.SetPos(RtgVertex3(x3,y3,pos.z));
		iOffset += 16;
	}

	if(IsShowName() && !m_name.text.empty())
	{
		x3 = pos.x;
		y3 = pos.y;
		y3 -= iOffset;
		m_name.SetPos(RtgVertex3(x3,y3,pos.z));

		iOffset += 16;
	}

	if(IsShowPetAbility() && !m_EditBoxPetAbility.text.empty())
	{
		x = pos.x;
		y = pos.y;
		y -= iOffset;
		m_EditBoxPetAbility.SetPos(RtgVertex3(x,y,pos.z));
		iOffset += 16;
	}

	if(IsShowPetFuseLev() && !m_EditBoxPetFuseLev.text.empty())
	{
		x = pos.x;
		y = pos.y;
		y -= iOffset;
		m_EditBoxPetFuseLev.SetPos(RtgVertex3(x,y,pos.z));
		iOffset += 16;
	}

	if(IsShowParty())
	{
		x2 = pos.x;
		y2 = pos.y;
		y2 -= iOffset;
		m_pPartyUi.SetPos(RtgVertex3(x2,y2,pos.z));

// 		if(IsShowTitle())
		{
			x2 = pos.x-((int)(m_title.text.size()*6+6)>>1);
			y2 = pos.y;
			y2 -= iOffset;
			int		strWidth = g_workspace.getFontManager()->GetStringWidth(m_pPartyUi.text);
			m_pPartyUi.SetSize(strWidth+2,16);
			m_pPartyUi.SetPos(RtgVertex3(x2,y2,pos.z));

			strWidth = g_workspace.getFontManager()->GetStringWidth(m_title.text);
			m_title.SetSize(strWidth+2,15);
			x6=m_pPartyUi.rect.s.right+12+(m_title.width>>1);
			y6 = pos.y;
			y6 -= iOffset;
			m_title.SetSize(strWidth+2,16);
			m_title.SetPos(RtgVertex3(x6,y6,pos.z));
		}

		x7=m_pPartyUi.rect.s.left-12;
		y7=m_pPartyUi.rect.s.top;
		m_partylogo->Move(SPoint(x7, y7));
		iOffset += 16;
	}

	if(m_pstallshow)
	{
		iOffset+=10;
		int		strWidth = g_workspace.getFontManager()->GetStringWidth(m_pStall->getText());
		m_pStall->SetWidgetSize(SSize(strWidth+2,20));
		rc = m_pStall->GetFrameRect();
		x5 = pos.x - (rc.getWidth()>>1);
		y5 = pos.y - rc.getHeight();
		y5 -= iOffset;
		m_pStall->Move(SPoint(x5, y5));
		iOffset += 16;
	}

	if(IsShowSpeak())
	{
		iOffset += 10;
		rc = m_pSpeakUI->GetFrameRect();
		x4 = pos.x - (rc.getWidth()>>1);
		y4 = pos.y - (rc.getHeight());
		y4-=iOffset;
		m_pSpeakUI->Move(SPoint(x4, y4));

		iOffset += 16;
	}

	m_pos = pos;
}

void RtwHUD::SetStallText(const string& text)
{
	m_szStall = text;
	m_pStall->SetText(text);	
}

void RtwHUD::SetSpeakText(const string& text)
{
	m_cShowItem |= siSpeak;
	int nLineNum = 0;
	m_pSpeakUI->SetTextnew(text);
	m_speakTimeStamp = rtSecond();
}

void RtwHUD::SetSpeakTime()
{
	m_speakTimeStamp = rtSecond();
}

void RtwHUD::SetSpeakColor(DWORD color){}

void RtwHUD::SetBloodValue(float value)
{
	m_pBloodBar->SetValue(value/100.0);
}

void RtwHUD::SetMessageColor(DWORD color){}

void RtwHUD::SetMessageText(const string& text)
{
	m_pStall->SetText(text);
}

void RtwHUD::SetNameColor(DWORD color)
{
	m_name.color = color;

}

void RtwHUD::SetNameText(const string& text)
{
	m_name.SetText(text);
}

void RtwHUD::SetPartyColor(DWORD color){}

void RtwHUD::SetPartyLogo(RtwImage* image)
{
	m_partylogo->SetBackgroundImage(image);
}

void RtwHUD::SetGuildText(const string& text)
{
	m_pPartyUi.SetText(text);
}


void RtwHUD::SetTitleText(const string& text)
{
	m_title.SetText(text);
}
void RtwHUD::SetTitleColor(DWORD color)
{
	m_title.color = color;
}

void RtwHUD::ShowHPBar(bool flag)
{
	if (flag)
	{
		m_cShowItem|=esiHPBar;
		s_cShowItem|=esiHPBar;
		if(m_bshow) m_pBloodBar->Show();
	}
	else
	{
		m_cShowItem&=(~esiHPBar);
		s_cShowItem&=(~esiHPBar);
		m_pBloodBar->Hide();
	}
}

void RtwHUD::ShowStall(bool flag)
{
	if (flag)
	{
		m_cShowItem|=esiStall;
		s_cShowItem|=esiStall;
		if(m_bshow) 
		{
			m_pStall->Show();
			m_pstallshow=true;
		}
	}
	else
	{
		m_cShowItem&=(~esiStall);
		s_cShowItem&=(~esiStall);
		m_pStall->Hide();
		m_pstallshow=false;
	}

}
void RtwHUD::SetTitleLogo(RtwImage* image){}

void RtwHUD::EditBox3D::Render()
{
	g_workspace.getFontManager()->DrawString(text, rect.s, color, true, SUiTextAlignment(alignCenter,alignCenter));
}

void RtwHUD::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
// 	DWORD tmp = rtMilliseconds();
	RtwWidget::DrawForeground(pClipRect);
	if(!m_bshow)
		return;

	if(IsShowPetMaster())
	{
		m_EditBoxPetMaster.Render();
	}

	if(IsShowName() && m_bShowName)
	{
		m_name.Render();
	}

	if(IsShowPetAbility())
	{
		m_EditBoxPetAbility.Render();
	}

	if(IsShowPetFuseLev())
	{
		m_EditBoxPetFuseLev.Render();
	}

	if(IsShowTitle())
	{
		m_title.Render();
	}

	if (IsShowParty())
	{
		m_pPartyUi.Render();
		m_partylogo->Show();
	} 
	else
	{
		m_partylogo->Hide();
	}

	if (IsShowSpeak())
	{
		if (m_pSpeakUI->getLine(0))
		{
			if (m_speakTimeStamp+m_speakDuration > (int)rtSecond())
			{
				m_pSpeakUI->Show();
			}
			else
			{
				m_pSpeakUI->RemoveAllItems();
				m_pSpeakUI->Hide();
			}
		}
	}
// 
// 	RtGetPref()->RenderHud += (rtMilliseconds()-tmp);
}

void RtwHUD::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}
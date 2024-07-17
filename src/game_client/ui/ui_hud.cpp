 
#include "gc_include.h"
#include "rtui.h"
#include "ui_hud.h"

//-----------------------------------------------------------------------------
//-				说话
//-				名字
//-				帮派
//-				血条
//-----------------------------------------------------------------------------
BYTE CHud::s_cShowItem =	CHud::esiSpeak|CHud::esiFaction|
                            CHud::esiHPBar|CHud::esiMsg|
							CHud::esiName |CHud::esiTitle|
							CHud::esiParty|CHud::esiStall;
							

//CDynamicInfo CHud::s_dynInfo;

bool CHud::ename=1;
bool CHud::espeak=1;
CHud::CHud()
: m_speakColor(0xffffffff)
, m_speakDuration(3)
, m_speakTimeStamp(0)
, m_bloodValue(50)
, m_cShowItem(CHud::s_cShowItem)
, m_bshow(true)
{
 // BYTE b = s_cShowItem;
}

CHud::~CHud()
{
    g_ui.RemoveWidget(m_pSpeakUI->getId());
    //g_ui.RemoveWidget(m_pNameUI->getId());
    g_ui.RemoveWidget(m_pBloodBar->getId());
   // g_ui.RemoveWidget(m_pPartyUI->getId());
	g_ui.RemoveWidget(m_pStall->getId());
	DROP_RTUI_OBJECT(m_pSpeakUI);
	//DROP_RTUI_OBJECT(m_pNameUI);
	DROP_RTUI_OBJECT(m_pBloodBar);
	//DROP_RTUI_OBJECT(m_pPartyUI);
	DROP_RTUI_OBJECT(m_pStall);
}

bool CHud::Init()
{

	m_pSpeakUI = (CUiEditBox*)g_ui.getWidgetFactory()->createWidget(wtEditBox);
	/*m_pNameUI = (CUiEditBox*)g_ui.getWidgetFactory()->createWidget(wtEditBox);*/
	m_pBloodBar = (CUiProgressBar*)g_ui.getWidgetFactory()->createWidget(wtProgressBar);
	/*m_pPartyUI = (CUiEditBox*)g_ui.getWidgetFactory()->createWidget(wtEditBox);*/
	m_pStall = (CUiButton*)g_ui.getWidgetFactory()->createWidget(wtButton);
	g_ui.AddWidget(m_pSpeakUI);
	/*g_ui.AddWidget(m_pNameUI);*/
	/*g_ui.AddWidget(m_pPartyUI);*/
	g_ui.AddWidget(m_pBloodBar);
	g_ui.AddWidget(m_pStall);

	m_pSpeakUI->Hide();
	/*m_pNameUI->Hide();*/
	m_pBloodBar->Hide();
	/*m_pPartyUI->Hide();*/
	m_pStall->Hide();

	IUiImage* pImage = g_ui.getImageFactory()->createImage("ui\\textures\\MainInterface_001.tga");
	pImage->SetSourceRect(SRect(10,200,115,204));
	m_pBloodBar->SetBarImage(pImage);
	DROP_RTUI_OBJECT(pImage);

	m_pSpeakUI->SetMultiLine(true);
	m_pSpeakUI->SetWordWrap(true);
	m_pSpeakUI->SetAutoScrollV(false);
    m_pSpeakUI->SetEnableInput(false);
    m_pSpeakUI->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	m_pSpeakUI->Move(SRect(0,0,200,15));
	m_pSpeakUI->Disable();
	m_pSpeakUI->SetBackgroundColor(SColor(0x0000000f));

	//m_pNameUI->SetMultiLine(false);
	//m_pNameUI->SetAutoScrollV(false);
 //   m_pNameUI->SetEnableInput(false);
	//m_pNameUI->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	//m_pNameUI->Move(SRect(0,0,200,15));
	//m_pNameUI->Disable();
	//m_pNameUI->SetBackgroundColor(SColor(0x00ffffff));

	/*m_pPartyUI->SetMultiLine(false);
	m_pPartyUI->SetAutoScrollV(false);
    m_pPartyUI->SetEnableInput(false);
	m_pPartyUI->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	m_pPartyUI->Move(SRect(0,0,100,15));
	m_pPartyUI->Disable();
	m_pPartyUI->SetBackgroundColor(SColor(0x00ffffff));*/

	m_pBloodBar->Move(SRect(0,0,60,6));
	m_pBloodBar->SetBackgroundColor(SColor(0xff000000));
	m_pBloodBar->SetValue(1);

	m_pStall->Move(SRect(0,0,140,15));
	m_pStall->SetBackgroundColor(SColor(0x99ff00ff));
	m_pStall->SetText("");
	m_pStall->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	m_pStall->SetBorderSize(1);
	m_pStall->SetBorderColor(0xFFC28D3B);
	m_szStall = "";

    ShowHPBar(false);
    ShowParty(false);	// 工会名称
    ShowName(true);		// 角色姓名
	ShowStall(false);

	m_name.SetSize(130,15);
	m_title.SetSize(260,15);
	m_pPartyUi.SetSize(130,15);
	//s_dynInfo.InitOnce();
	
	return true;
}
//-----------------------------------------------------------------------------

void CHud::Show()
{
	if(IsShowHPBar())
	{
		m_pBloodBar->Show();
	}else
	{
		m_pBloodBar->Hide();
	}

	//if(IsShowParty())
	//{
	//	m_pPartyUI->Show();
	//}
	//else
	//{
	//	m_pPartyUI->Hide();
	//}

	//if(IsShowName())
	//{
	//	//m_pNameUI->SetText("***");
	//	m_pNameUI->Show();
	//}
	//else
	//{
	//	m_pNameUI->Hide();
	//}

	//Render3D();

	//if(IsShowSpeak())
	//{
	//	//if(m_pSpeakUI->getText() != "")
	//	if(m_pSpeakUI->getLine(0))
	//	{
	//		unsigned long c = m_pSpeakUI->getLineCount();
	//		unsigned long height = 0;
	//		for(int i=0; i<c; i++)
	//		{
	//			ui::CUiEditBox::SLine* pLine = m_pSpeakUI->getLine(i);
	//			height += pLine->rect.getHeight();
	//		}

	//		if(height!=0)
	//			m_pSpeakUI->Move(SRect(0, 0, 200, height+c*2));
	//		
	//		if(m_speakTimeStamp+m_speakDuration>GetClient()->GetTime())
	//		{
	//			m_pSpeakUI->Show();
	//		}else
	//		{
	//			m_pSpeakUI->RemoveAllLines();
	//			m_pSpeakUI->Hide();
	//		}
	//	}
	//}

	if(IsShowStall())
	{
		m_pStall->Show();
	}
	else
	{
		m_pStall->Hide();
	}
}
void CHud::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
	if(!m_bshow)
		return;
	if(IsShowHPBar())
	{

	}

	if(IsShowName())
	{
		m_name.Render(inDevice);
	}
    if(IsShowTitle())
	{
		m_title.Render(inDevice);
	}
	if (IsShowParty())
	{
		m_pPartyUi.Render(inDevice);
	}
	if(IsShowSpeak())
	{
		/*if(m_pSpeakUI->getLine(0))
		{
			if(m_speakTimeStamp+m_speakDuration>GetClient()->GetTime())
			{
				m_pSpeakUI->Show();
			}else
			{
				m_pSpeakUI->RemoveAllLines();
				m_pSpeakUI->Hide();
			}
		}*/
	}

    if(IsShowStall())
	{

	}

	//if(IsShowDynInfo())
	//{
	//	CHud::s_dynInfo.Render(inCamera, inDevice, eMask);
	//}
}

void CHud::OnHudFlagChanged(BYTE newFlag)
{
	CHud::s_cShowItem = newFlag;
	//ApplyFlag();
}
void CHud::SetPosition(RtgVertex3 pos)
{
	SRect rc;
	int x1=0, y1=0;
	int x2=0, y2=0;
	int x3=0, y3=0;
	int x4=0, y4=0;
	int x5=0, y5=0;
	int iOffset = 0;

	//BYTE A = m_cShowItem;
	//BYTE B = s_cShowItem;

	if(IsShowHPBar())
	{
		rc = m_pBloodBar->getFrameRect();
		x1 = pos.x - rc.getWidth() / 2;
		y1 = pos.y - rc.getHeight();
		y1 -= iOffset;
		m_pBloodBar->Move(SPoint(x1, y1));

		iOffset += 10;
	}

	if(IsShowParty())
	{
		//rc = m_pPartyUI->getFrameRect();
		//x2 = pos.x - rc.getWidth() / 2;
		//y2 = pos.y - rc.getHeight();
		//y2 -= iOffset;
		//m_pPartyUI->Move(SPoint(x2, y2));

		x2 = pos.x;
		y2 = pos.y;
		y2 -= iOffset;
		m_pPartyUi.SetPos(RtgVertex3(x2,y2,pos.z));

		iOffset += 15;
	}

	if(IsShowTitle())
	{
		x2 = pos.x;
		y2 = pos.y;
		y2 -= iOffset;
		m_title.SetPos(RtgVertex3(x2,y2,pos.z));

		iOffset += 15;
	}
	if(IsShowName())
	{
		x3 = pos.x;
		y3 = pos.y;
		y3 -= iOffset;
		m_name.SetPos(RtgVertex3(x3,y3,pos.z));

		iOffset += 15;
	}
	

	/*if(IsShowSpeak())
	{
		rc = m_pSpeakUI->getFrameRect();
		x4 = x - rc.getWidth() / 2;
		y4 = y - rc.getHeight();
		y4-=iOffset;
		m_pSpeakUI->Move(SPoint(x4, y4));

		iOffset += 15;
	}*/

	if(IsShowStall())
	{
		iOffset += 10;
		rc = m_pStall->getFrameRect();
		x5 = pos.x - rc.getWidth() / 2;
		y5 = pos.y - rc.getHeight();
		y5 -= iOffset;
		m_pStall->Move(SPoint(x5, y5));
	}
	
}

void CHud::SetStallText(const string& text)
{
	m_szStall = text;
	m_pStall->SetText(text);
}

void CHud::SetSpeakText(const string& text)
{
	//m_pSpeakUI->SetText(text);
	m_speakTimeStamp = rtSecond();
}

void CHud::SetSpeakTime()
{
	m_speakTimeStamp = rtSecond();
}

void CHud::SetSpeakColor(DWORD color)
{
}

void CHud::SetBloodValue(float value)
{
	m_pBloodBar->SetValue(value);
	
}

void CHud::SetMessageColor(DWORD color)
{
}

void CHud::SetMessageText(const string& text)
{
}

void CHud::SetNameColor(DWORD color)
{
	m_name.color = color;
	
}

void CHud::SetNameText(const string& text)
{
	//m_pNameUI->SetText(text);
	//m_sname = text;
	m_name.SetText(text);
}

void CHud::SetPartyColor(DWORD color)
{
}

void CHud::SetPartyLogo(IUiImage* image)
{
}

void CHud::SetPartyText(const string& text)
{
	//m_pPartyUI->SetText(text);
	m_pPartyUi.SetText(text);
}


void CHud::SetTitleText(const string& text)
{
	//m_pPartyUI->SetText(text);
	m_title.SetText(text);
}
void CHud::SetTitleColor(DWORD color)
{
	m_title.color = color;
}

void CHud::SetTitleLogo(IUiImage* image)
{
}

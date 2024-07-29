#include "gc_include.h"
#include "region_client.h"
#include "gc_bulletin.h"


const int Bulletin_yPos = 170;
const int Bulletin_speed = 60;
const int Bulletin_baseTime = 1400/Bulletin_speed*1000;
const int Bulletin_showCount = 2;
const int Bulletin_UrgentCount = 5;
const int Bulletin_sleep = 900000;//等待15分钟再播放下一条

RtgVertex3 pos;
bool g_bNotify = true;

CBulletinManager::CBulletinManager()
{
	g_workspace.Load("ui/ui_layer_main.xml");
	m_pFormBulletin = LOAD_UI_T(RtwForm, "fmBulletin");
	m_pChatBulletin = LOAD_UI_T(RtwTextBox, "fmBulletin.txtFeedback");

	m_pChatBulletin->SetEnableInput(false);
	m_pChatBulletin->SetMultiLine(false);
	m_pChatBulletin->SetWordWrap(false);
	m_pChatBulletin->ModifyFlag(wfPenetrate, 0);
	m_pFormBulletin->ModifyFlag(wfPenetrate, 0);
	//m_pFormBulletin->SetBackgroundColor(0x50000000);

	m_pChatBulletin->SetFontSize(20);
	Clear();
}

CBulletinManager::~CBulletinManager(){}

void CBulletinManager::Run(DWORD delta)
{
	guard;

	static string strTmp = "";

	//紧急公告
	if (!m_UrgentBulletins.empty())
	{
		if (g_bNotify)
		{
			g_pSoundMgr->PlayOnce("ui_notify.wav", false, 0, pos);
			if (m_NormalBulletins.size() > 0)
			{
				SBulletin& bullNormal = m_NormalBulletins.front();
				bullNormal.showCount = 0;
			}
			g_bNotify = false;
		}

		SBulletin& bull = m_UrgentBulletins.front();

		int strLength = bull.content.size();

		if (bull.content != strTmp)
		{
			strTmp = bull.content;
			m_pChatBulletin->SetTextColor(bull.color);
			m_pChatBulletin->SetText(bull.content);
		}

		if (!m_pFormBulletin->IsVisible())
		{
			m_pFormBulletin->Show();
			g_workspace.AddTopRenderWidget(m_pFormBulletin);
		}

		((RtwChatBox*)m_pChatBulletin)->WDScroll(delta, bull.showCount, bull.showCount);

		if (bull.showCount == Bulletin_UrgentCount)
		{
			m_UrgentBulletins.pop_front();
			g_bNotify = true;
			m_pFormBulletin->Hide();
		}

		return;
	}

	if (m_NormalBulletins.empty())
		return;

	if (m_nTimeCounter > 0)
	{
		m_nTimeCounter -= delta;
		return;
	}

	SBulletin& bull = m_NormalBulletins.front();
	int strLength = bull.content.size();

	if (g_bNotify)
	{
		g_pSoundMgr->PlayOnce("ui_notify.wav", false, 0, pos);
		g_bNotify = false;
	}

	if (bull.content != strTmp)
	{
		strTmp = bull.content;
		m_pChatBulletin->SetTextColor(bull.color);
		m_pChatBulletin->SetText(bull.content);
	}

	if (!m_pFormBulletin->IsVisible())
	{
		m_pFormBulletin->Show();
		g_workspace.AddTopRenderWidget(m_pFormBulletin);
	}

	((RtwChatBox*)m_pChatBulletin)->WDScroll(delta, bull.showCount, bull.showCount);

	if (bull.showCount == Bulletin_showCount)
	{
		m_NormalBulletins.pop_front();
		g_bNotify = true;
		m_nTimeCounter = Bulletin_sleep;
		m_pFormBulletin->Hide();
	}

	//end
	unguard;
}

void CBulletinManager::OnRecvNewBulletin(const char* content, DWORD gmID, DWORD color, char style)
{
	guard;

	SBulletin bull;
	bull.content = content;
	bull.gmID = gmID;
	bull.color = color;
	bull.style = style;
	bull.showCount = 0;

	if (color == 0xEE7600)
	{
		//普通gm公告
		m_NormalBulletins.push_back(bull);
	}
	else if (color == 0x97FFFF)
	{
		//紧急gm公告
		m_UrgentBulletins.push_back(bull);
	}
	else
	{
		m_UrgentBulletins.push_back(bull);
	}

	unguard;
}


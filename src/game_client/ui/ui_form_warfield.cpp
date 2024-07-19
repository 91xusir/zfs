
#include "gc_include.h"
#include "game_assistant.h"
#include "ui_form_warfield.h"

#define WARFIELD_NUM_LIMIT	200

UIFormWarfield::UIFormWarfield()
{
	m_pFrmThis		= RTW_WIDGET("fmwarlist");
	m_pLabelTime	= RTW_WIDGET_T(RtwLabel, "fmwarlist.lbwartime");
	m_pLabelTitle	= RTW_WIDGET_T(RtwLabel, "fmwarlist.lblist");
	m_pTab			= RTW_WIDGET_T(RtwTab, "fmwarlist.tablist");
	m_pListZhou		= RTW_WIDGET_T(RtwListBox, "fmwarlist.tablist.fmguildlist.listzhou");
	m_pListShang	= RTW_WIDGET_T(RtwListBox, "fmwarlist.tablist.fmguildlist.listshang");
	m_pBtnInfo		= RTW_WIDGET_T(RtwButton, "fmwarlist.tablist.btnguildlist");
	m_pBtnSort		= RTW_WIDGET_T(RtwButton, "fmwarlist.tablist.btnguildinfo");
	m_pLabelNumZhou		= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhounum");
	m_pLabelNumShang	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshangnum");
	m_pLabelScoreZhou	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhouscore");
	m_pLabelScoreShang	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshangscore");
	m_pLabelRateZhou	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhouchange");
	m_pLabelRateShang	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshangchange");
	m_pProgressZhou1	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barzhou1");
	m_pProgressZhou2	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barzhou2");
	m_pProgressZhou3	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barzhou3");
	m_pProgressShang1	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barshang1");
	m_pProgressShang2	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barshang2");
	m_pProgressShang3	= RTW_WIDGET_T(RtwProgressBar, "fmwarlist.tablist.fminfolist.barshang3");

	m_pLabelZhou1Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhou1count");
	m_pLabelZhou2Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhou2count");
	m_pLabelZhou3Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbzhou3count");
	m_pLabelShang1Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshang1count");
	m_pLabelShang2Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshang2count");
	m_pLabelShang3Count	= RTW_WIDGET_T(RtwLabel, "fmwarlist.tablist.fminfolist.lbshang3count");

	m_pBtnInfo->EvLClick += RTW_CALLBACK(this, UIFormWarfield, OnBtnInfo);
	m_pBtnSort->EvLClick += RTW_CALLBACK(this, UIFormWarfield, OnBtnList);

	m_pTab->SetSelectedItem(0);
	m_pLabelTime->SetText("");
	m_pLabelNumZhou->SetText("");
	m_pLabelNumShang->SetText("");
	m_pLabelZhou1Count->SetText("0/1000");
	m_pLabelZhou2Count->SetText("0/2000");
	m_pLabelZhou3Count->SetText("0/3000");

	m_pLabelShang1Count->SetText("0/1000");
	m_pLabelShang2Count->SetText("0/2000");
	m_pLabelShang3Count->SetText("0/3000");
	m_pLabelRateZhou->SetText("1.0");
	m_pLabelRateShang->SetText("1.0");

	Hide();

	m_bReport = false;
	m_winFaction = FACTION_UNKNOWN;
}

bool UIFormWarfield::IsInInfo()
{
	return m_pTab->GetSelectedItem() == 0;
}

void UIFormWarfield::OnPreDraw()
{
	char faction = GetWorld()->m_pPlayer->m_core.Faction;
	if(FACTION_UNKNOWN == faction) return;

	long lHour = (long)m_stWarInfo.tRemainTime / (60 * 60);
	long lMin = (long)m_stWarInfo.tRemainTime / 60 - lHour * 60;
	std::string strHour = rtFormatNumber(lHour);
	std::string strMin = rtFormatNumber(lMin);
	//std::string strTime = strHour + "小时" + strMin + "分";//by fox for string
	std::string strTime = strHour + R(MSG_WARFIELD_HOUR) + strMin + R(MSG_WARFIELD_MINUTE);

	m_pLabelTime->SetText(strTime);

	if(IsInInfo())
	{
		std::string strNumZhou;
		std::string strNumShang;

		if((float)m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.1)
		{
			strNumZhou = R(MSG_WARFIELD_VERY_LITTLE);
		}else if((float)m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.2)
		{
			strNumZhou = R(MSG_WARFIELD_LITTLE);
		}else if((float)m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.3)
		{
			strNumZhou = R(MSG_WARFIELD_INTER);
		}else if((float)m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.5)
		{
			strNumZhou = R(MSG_WARFIELD_MANY);
		}else
		{
			strNumZhou = R(MSG_WARFIELD_VERY_MANY);
		}

		if((float)m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.1)
		{
			strNumShang = R(MSG_WARFIELD_VERY_LITTLE);
		}else if((float)m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.2)
		{
			strNumShang = R(MSG_WARFIELD_LITTLE);
		}else if((float)m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.3)
		{
			strNumShang = R(MSG_WARFIELD_INTER);
		}else if((float)m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.5)
		{
			strNumShang = R(MSG_WARFIELD_MANY);
		}else
		{
			strNumShang = R(MSG_WARFIELD_VERY_MANY);
		}

		m_pLabelNumZhou->SetText(strNumZhou);
		m_pLabelNumShang->SetText(strNumShang);

		char buf[10];
		rt2_sprintf(buf, "%.1f", m_stWarInfo.lScoreZhou);
		m_pLabelScoreZhou->SetText((char*)buf);
		rt2_sprintf(buf, "%.1f", m_stWarInfo.lScoreShang);
		m_pLabelScoreShang->SetText((char*)buf);
		//sprintf(buf, "%.1f", m_stWarInfo.fRZhou);
		//m_pLabelRateZhou->SetText((char*)buf);
		//sprintf(buf, "%.1f", m_stWarInfo.fRShang);
		//m_pLabelRateShang->SetText((char*)buf);

		if(abs(m_stWarInfo.fRZhou - 1.0) < 0.3)
		{
			m_pLabelRateZhou->SetText(R(STR_INTER));
		}else if((m_stWarInfo.fRZhou - 1.0) >= 0.3)
		{
			m_pLabelRateZhou->SetText(R(STR_HIGH));
		}else
		{
			m_pLabelRateZhou->SetText(R(STR_LOW));
		}

		if(abs(m_stWarInfo.fRShang - 1.0) < 0.3)
		{
			m_pLabelRateShang->SetText(R(STR_INTER));
		}else if((m_stWarInfo.fRShang - 1.0) >= 0.3)
		{
			m_pLabelRateShang->SetText(R(STR_HIGH));
		}else
		{
			m_pLabelRateShang->SetText(R(STR_LOW));
		}

		//m_pLabelScoreZhou->SetText(rtFormatNumber((long)m_stWarInfo.lScoreZhou));
		//m_pLabelScoreShang->SetText(rtFormatNumber((long)m_stWarInfo.lScoreShang));
		//m_pLabelRateZhou->SetText(rtFormatNumber((long)m_stWarInfo.fRZhou));
		//m_pLabelRateZhou->SetText(rtFormatNumber((long)m_stWarInfo.fRShang));
		m_pProgressZhou1->SetValue((float)m_stWarInfo.lPriHero[0]/m_stWarInfo.lPriHero[2]);
		m_pProgressZhou2->SetValue((float)m_stWarInfo.lInterHero[0]/m_stWarInfo.lInterHero[2]);
		m_pProgressZhou3->SetValue((float)m_stWarInfo.lSeniorHero[0]/m_stWarInfo.lSeniorHero[2]);
		m_pProgressShang1->SetValue((float)m_stWarInfo.lPriHero[1]/m_stWarInfo.lPriHero[2]);
		m_pProgressShang2->SetValue((float)m_stWarInfo.lInterHero[1]/m_stWarInfo.lInterHero[2]);
		m_pProgressShang3->SetValue((float)m_stWarInfo.lSeniorHero[1]/m_stWarInfo.lSeniorHero[2]);

		std::string strZhou1 = std::string(rtFormatNumber(m_stWarInfo.lPriHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lPriHero[2]));
		std::string strZhou2 = std::string(rtFormatNumber(m_stWarInfo.lInterHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lInterHero[2]));
		std::string strZhou3 = std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[2]));
		std::string strShang1 = std::string(rtFormatNumber(m_stWarInfo.lPriHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lPriHero[2]));
		std::string strShang2 = std::string(rtFormatNumber(m_stWarInfo.lInterHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lInterHero[2]));
		std::string strShang3 = std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[2]));
		m_pLabelZhou1Count->SetText(strZhou1);
		m_pLabelZhou2Count->SetText(strZhou2);
		m_pLabelZhou3Count->SetText(strZhou3);
		m_pLabelShang1Count->SetText(strShang1);
		m_pLabelShang2Count->SetText(strShang2);
		m_pLabelShang3Count->SetText(strShang3);
	}else
	{
		m_pListZhou->RemoveAllItems();
		m_pListShang->RemoveAllItems();

		std::list<SExploit> listZhou;
		std::list<SExploit> listShang;

		std::list<SExploit>::iterator it = m_stWarInfo.listExploit.begin();
		for(; it != m_stWarInfo.listExploit.end(); ++it)
		{
			if(FACTION_ZHOU == it->faction)
			{
				listZhou.push_back((*it));
			}else if(FACTION_SHANG == it->faction)
			{
				listShang.push_back((*it));
			}
		}

		listZhou.sort(std::greater<SExploit>());
		listShang.sort(std::greater<SExploit>());

		it = listZhou.begin();
		for(int i = 1; it != listZhou.end(); ++it, ++i)
		{
			int idx = m_pListZhou->AddItem("");
			m_pListZhou->SetItemText(idx, 0, rtFormatNumber(i));
			m_pListZhou->SetItemText(idx, 1, it->unionName);
			m_pListZhou->SetItemText(idx, 2, rtFormatNumber(it->lExploit));
		}

		it = listShang.begin();
		for(int j = 1; it != listShang.end(); ++it, ++j)
		{
			int idx = m_pListShang->AddItem("");
			m_pListShang->SetItemText(idx, 0, rtFormatNumber(j));
			m_pListShang->SetItemText(idx, 1, it->unionName);
			m_pListShang->SetItemText(idx, 2, rtFormatNumber(it->lExploit));
		}
	}
}

void UIFormWarfield::OnBtnInfo(RtwWidget* sender, void*)
{
	guard;

	if(!m_bReport)
	{
		Refresh();
	}else
	{
		Report();
	}

	unguard;
}

void UIFormWarfield::OnBtnList(RtwWidget* sender, void*)
{
	guard;

	if(!m_bReport)
	{
		Refresh();
	}else
	{
		Report();
	}

	unguard;
}

void UIFormWarfield::Hide()
{
	m_pFrmThis->Hide();
}

void UIFormWarfield::Show()
{
	guard;
	if (m_pFrmThis)
	{
		if (!IsVisible())
		{
			OnPreDraw();
			m_pFrmThis->Show();
		}
	}
	unguard;
}

bool UIFormWarfield::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormWarfield::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}

	unguard;
}

void UIFormWarfield::OnRecvUpdateData(long lRemain, long numZhou, float lScoreZhou, long numShang, float lScoreSHang, 
									  float* lPriHero, float* lInterHero, float* lSeniorHero, 
									  std::list<SExploit>* pExploit, float fRZhou, float fRShang)
{
	m_stWarInfo.tRemainTime = lRemain;
	m_stWarInfo.lNumZhou	= numZhou;
	m_stWarInfo.lNumShang	= numShang;
	m_stWarInfo.lScoreZhou	= lScoreZhou;
	m_stWarInfo.lScoreShang = lScoreSHang;
	m_stWarInfo.fRZhou		= fRZhou;
	m_stWarInfo.fRShang		= fRShang;

	memcpy((void*)m_stWarInfo.lPriHero, (void*)lPriHero, sizeof(float) * 3);
	memcpy((void*)m_stWarInfo.lInterHero, (void*)lInterHero, sizeof(float) * 3);
	memcpy((void*)m_stWarInfo.lSeniorHero, (void*)lSeniorHero, sizeof(float) * 3);

	m_stWarInfo.listExploit.clear();
	m_stWarInfo.listExploit = *pExploit;

	Refresh();
}

void UIFormWarfield::OnReport(long lReason, char faction, long numZhou, float lScoreZhou, long numShang, float lScoreSHang,
							float* lPriHero, float* lInterHero, float* lSeniorHero,
							std::list<SExploit>* pExploit, float fRZhou, float fRShang)
{
	Hide();
	m_bReport = true;

	m_winFaction			= faction;
	m_stWarInfo.lNumZhou	= numZhou;
	m_stWarInfo.lNumShang	= numShang;
	m_stWarInfo.lScoreZhou	= lScoreZhou;
	m_stWarInfo.lScoreShang = lScoreSHang;
	m_stWarInfo.fRZhou		= fRZhou;
	m_stWarInfo.fRShang		= fRShang;

	memcpy((void*)m_stWarInfo.lPriHero, (void*)lPriHero, sizeof(long) * 3);
	memcpy((void*)m_stWarInfo.lInterHero, (void*)lInterHero, sizeof(long) * 3);
	memcpy((void*)m_stWarInfo.lSeniorHero, (void*)lSeniorHero, sizeof(long) * 3);

	m_stWarInfo.listExploit.clear();
	m_stWarInfo.listExploit = *pExploit;

	Report();
}

void UIFormWarfield::Clear()
{
	//Hide();

	//m_pTab->SetSelectedItem(0);
	//m_pLabelTime->SetText("");
	//m_pLabelNumZhou->SetText("");
	//m_pLabelNumShang->SetText("");
	//m_pLabelZhou1Count->SetText("0/1000");
	//m_pLabelZhou2Count->SetText("0/2000");
	//m_pLabelZhou3Count->SetText("0/3000");

	//m_pLabelShang1Count->SetText("0/1000");
	//m_pLabelShang2Count->SetText("0/2000");
	//m_pLabelShang3Count->SetText("0/3000");

	//m_bReport = false;
	//m_winFaction = FACTION_UNKNOWN;
}

void UIFormWarfield::Report()
{
	//m_pLabelTitle->SetText("国战结束");//by fox for string
	m_pLabelTitle->SetText(R(MSG_WARFIELD_FINISH));

	if(FACTION_UNKNOWN == m_winFaction)
	{
		//m_pLabelTime->SetText("平局");//by fox for string
		m_pLabelTime->SetText(R(MSG_WARFIELD_DRAW));

	}else if(FACTION_ZHOU == m_winFaction)
	{
		//m_pLabelTime->SetText("周朝获胜");//by fox for string
		m_pLabelTime->SetText(R(MSG_WARFIELD_ZHOU_WIN));
	}else
	{
		//m_pLabelTime->SetText("商朝获胜");//by fox for string
		m_pLabelTime->SetText(R(MSG_WARFIELD_SHANG_WIN));
	}

	if(IsInInfo())
	{
		std::string strNumZhou;
		std::string strNumShang;

		if(m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.1)
		{
			strNumZhou = R(MSG_WARFIELD_VERY_LITTLE);
		}else if(m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.2)
		{
			strNumZhou = R(MSG_WARFIELD_LITTLE);
		}else if(m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.3)
		{
			strNumZhou = R(MSG_WARFIELD_INTER);
		}else if(m_stWarInfo.lNumZhou / WARFIELD_NUM_LIMIT < 0.5)
		{
			strNumZhou = R(MSG_WARFIELD_MANY);
		}else
		{
			strNumZhou = R(MSG_WARFIELD_VERY_MANY);
		}

		if(m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.1)
		{
			strNumShang = R(MSG_WARFIELD_VERY_LITTLE);
		}else if(m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.2)
		{
			strNumShang = R(MSG_WARFIELD_LITTLE);
		}else if(m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.3)
		{
			strNumShang = R(MSG_WARFIELD_INTER);
		}else if(m_stWarInfo.lNumShang / WARFIELD_NUM_LIMIT < 0.5)
		{
			strNumShang = R(MSG_WARFIELD_MANY);
		}else
		{
			strNumShang = R(MSG_WARFIELD_VERY_MANY);
		}

		m_pLabelNumZhou->SetText(strNumZhou);
		m_pLabelNumShang->SetText(strNumShang);
		char buf[10];
		rt2_sprintf(buf, "%.1f", m_stWarInfo.lScoreZhou);
		m_pLabelScoreZhou->SetText((char*)buf);
		rt2_sprintf(buf, "%.1f", m_stWarInfo.lScoreShang);
		m_pLabelScoreShang->SetText((char*)buf);

		if(abs(m_stWarInfo.fRZhou - 1.0) < 0.3)
		{
			m_pLabelRateZhou->SetText(R(STR_INTER));
		}else if((m_stWarInfo.fRZhou - 1.0) >= 0.3)
		{
			m_pLabelRateZhou->SetText(R(STR_HIGH));
		}else
		{
			m_pLabelRateZhou->SetText(R(STR_LOW));
		}

		if(abs(m_stWarInfo.fRShang - 1.0) < 0.3)
		{
			m_pLabelRateShang->SetText(R(STR_INTER));
		}else if((m_stWarInfo.fRShang - 1.0) >= 0.3)
		{
			m_pLabelRateShang->SetText(R(STR_HIGH));
		}else
		{
			m_pLabelRateShang->SetText(R(STR_LOW));
		}
		
		//sprintf(buf, "%.1f", m_stWarInfo.fRShang);
		//m_pLabelRateShang->SetText((char*)buf);

		//m_pLabelScoreZhou->SetText(rtFormatNumber(m_stWarInfo.lScoreZhou));
		//m_pLabelScoreShang->SetText(rtFormatNumber(m_stWarInfo.lScoreShang));
		//m_pLabelRateZhou->SetText(rtFormatNumber((long)m_stWarInfo.fRShang));
		//m_pLabelRateZhou->SetText(rtFormatNumber((long)m_stWarInfo.fRZhou));
		m_pProgressZhou1->SetValue((float)m_stWarInfo.lPriHero[0]/m_stWarInfo.lPriHero[2]);
		m_pProgressZhou2->SetValue((float)m_stWarInfo.lInterHero[0]/m_stWarInfo.lInterHero[2]);
		m_pProgressZhou3->SetValue((float)m_stWarInfo.lSeniorHero[0]/m_stWarInfo.lSeniorHero[2]);
		m_pProgressShang1->SetValue((float)m_stWarInfo.lPriHero[1]/m_stWarInfo.lPriHero[2]);
		m_pProgressShang2->SetValue((float)m_stWarInfo.lInterHero[1]/m_stWarInfo.lInterHero[2]);
		m_pProgressShang3->SetValue((float)m_stWarInfo.lSeniorHero[1]/m_stWarInfo.lSeniorHero[2]);

		std::string strZhou1 = std::string(rtFormatNumber(m_stWarInfo.lPriHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lPriHero[2]));
		std::string strZhou2 = std::string(rtFormatNumber(m_stWarInfo.lInterHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lInterHero[2]));
		std::string strZhou3 = std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[0])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[2]));
		std::string strShang1 = std::string(rtFormatNumber(m_stWarInfo.lPriHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lPriHero[2]));
		std::string strShang2 = std::string(rtFormatNumber(m_stWarInfo.lInterHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lInterHero[2]));
		std::string strShang3 = std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[1])) + "/" + std::string(rtFormatNumber(m_stWarInfo.lSeniorHero[2]));
		m_pLabelZhou1Count->SetText(strZhou1);
		m_pLabelZhou2Count->SetText(strZhou2);
		m_pLabelZhou3Count->SetText(strZhou3);
		m_pLabelShang1Count->SetText(strShang1);
		m_pLabelShang2Count->SetText(strShang2);
		m_pLabelShang3Count->SetText(strShang3);
	}else
	{
		m_pListZhou->RemoveAllItems();
		m_pListShang->RemoveAllItems();

		std::list<SExploit> listZhou;
		std::list<SExploit> listShang;

		std::list<SExploit>::iterator it = m_stWarInfo.listExploit.begin();
		for(; it != m_stWarInfo.listExploit.end(); ++it)
		{
			if(FACTION_ZHOU == it->faction)
			{
				listZhou.push_back((*it));
			}else if(FACTION_SHANG == it->faction)
			{
				listShang.push_back((*it));
			}
		}

		listZhou.sort(std::greater<SExploit>());
		listShang.sort(std::greater<SExploit>());

		it = listZhou.begin();
		for(int i = 1; it != listZhou.end(); ++it, ++i)
		{
			int idx = m_pListZhou->AddItem("");
			m_pListZhou->SetItemText(idx, 0, rtFormatNumber(i));
			m_pListZhou->SetItemText(idx, 1, it->unionName);
			m_pListZhou->SetItemText(idx, 2, rtFormatNumber(it->lExploit));
		}

		it = listShang.begin();
		for(int j = 1; it != listShang.end(); ++it, ++j)
		{
			int idx = m_pListShang->AddItem("");
			m_pListShang->SetItemText(idx, 0, rtFormatNumber(j));
			m_pListShang->SetItemText(idx, 1, it->unionName);
			m_pListShang->SetItemText(idx, 2, rtFormatNumber(it->lExploit));
		}
	}

	m_pFrmThis->Show();
}

bool CWarfieldNetHelper::NetSend_c2r_warfield_view()
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_warfield_view);
	return NetSend(cmd);
}
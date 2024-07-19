#include "gc_include.h"
#include "ui_form_herolist.h"

// Sort
static enum ESort
{
	Sort_Name,
	Sort_Level,
	Sort_Score
};

static ESort heroSortPriority[3] = { Sort_Score, Sort_Level, Sort_Name };
static bool SortFunc_Hero(const SHeroListItem& h1, const SHeroListItem& h2);

static ESort unionSortPriority[3] = { Sort_Score, Sort_Level, Sort_Name };
static bool SortFunc_Union(const SUnionListItem& u1, const SUnionListItem& u2);


bool SortFunc_Hero(const SHeroListItem& h1, const SHeroListItem& h2)
{
	guard;

	int depth = 0;
	while (depth<3)
	{
		if (heroSortPriority[depth]==Sort_Score)
		{
			if (h1.score != h2.score)				return (h1.score > h2.score);
		}
		else if (heroSortPriority[depth]==Sort_Level)
		{
			if (h1.level != h2.level)				return (h1.level > h2.level);
		}
		else if (heroSortPriority[depth]==Sort_Name)
		{
			if (h1.name != h2.name)					return (h1.name > h2.name);
		}
		depth++;
	}

	CHECK(0);		// Name是不可能相同的
	return true;

	unguard;
}

bool SortFunc_Union(const SUnionListItem& u1, const SUnionListItem& u2)
{
	guard;

	int depth = 0;
	while (depth<3)
	{
		if (unionSortPriority[depth]==Sort_Score)
		{
			if (u1.score != u2.score)				return (u1.score > u2.score);
		}
		else if (unionSortPriority[depth]==Sort_Level)
		{
			if (u1.level != u2.level)				return (u1.level > u2.level);
		}
		else if (unionSortPriority[depth]==Sort_Name)
		{
			if (u1.name != u2.name)					return (u1.name > u2.name);
		}
		depth++;
	}

	CHECK(0);		// Name是不可能相同的
	return true;

	unguard;
}


UIFormHeroList::UIFormHeroList()
:m_HeroList(HERO_LIST_COUNT, UNION_LIST_COUNT)
{
	guard;

	m_pFrmThis						= RTW_WIDGET("fmnewlist");
	m_pTabHeroList					= RTW_WIDGET_T(RtwTab, "fmnewlist.fmlist.tablist");
	m_listHeroAoShi					= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist1.listaoshi");
	m_listHeroXianChong				= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist2.listaoshi");
	m_listHeroShuiDi				= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist3.listaoshi");
	m_listHeroFuJia					= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist4.listaoshi");
	m_listHeroYuanBao				= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist5.listaoshi");
	m_listHeroLongHu				= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist6.listaoshi");
	m_listHeroDiFu					= RTW_WIDGET_T(RtwListBox, "fmnewlist.fmlist.tablist.fmpersonlist7.listaoshi");

	// 注册本窗口中的事件
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist1")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist2")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist3")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist4")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist5")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	RTW_WIDGET("fmnewlist.fmlist.tablist.btnguildlist6")->EvLClick += RTW_CALLBACK(this, UIFormHeroList, OnNeedRefresh);
	m_listHeroAoShi->GetColumnHeader(0)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroAoShi->GetColumnHeader(1)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroAoShi->GetColumnHeader(2)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroXianChong->GetColumnHeader(0)->EvLClick		+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroXianChong->GetColumnHeader(1)->EvLClick		+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroXianChong->GetColumnHeader(2)->EvLClick		+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroShuiDi->GetColumnHeader(0)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroShuiDi->GetColumnHeader(1)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroShuiDi->GetColumnHeader(2)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroFuJia->GetColumnHeader(0)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroFuJia->GetColumnHeader(1)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroFuJia->GetColumnHeader(2)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroYuanBao->GetColumnHeader(0)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroYuanBao->GetColumnHeader(1)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);
	m_listHeroYuanBao->GetColumnHeader(2)->EvLClick			+= RTW_CALLBACK(this, UIFormHeroList, OnHeroSortHeaderClick);

	m_pTabHeroList->SetSelectedItem(0);

	m_pFrmThis->Hide();

	unguard;
}

UIFormHeroList::~UIFormHeroList()
{
	guard;
	unguard;
}

bool UIFormHeroList::IsInHeroList()
{
	guard;
	return (m_pTabHeroList->GetSelectedItem()==0);
	unguard;
}

char UIFormHeroList::InWhickMetierList()
{
	guard;

	int iMetier = m_pTabHeroList->GetSelectedItem();
	switch(iMetier)
	{
	case 0:		return AoShiQunXiong;
	case 1:		return XianChongZhiWang;
	case 2:		return ShuiDiShiChuan;
	case 3:		return FuJiaYiFang;
	case 4:		return YuanBaoDaHeng;
	case 5:		return LongPanHuYue;
	case 6:		return DiFuChangKe;
	default:	return ChuCuolie;
	}

	unguard;
}

void UIFormHeroList::OnPreDraw()
{
	guard;

	char cTmp32[32];
	char cMetier = InWhickMetierList();
	if (cMetier==AoShiQunXiong)
	{
		m_listHeroAoShi->RemoveAllItems();
		int nListCount = m_HeroList.m_warriorList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroAoShi->AddItem("");
			m_listHeroAoShi->SetItemText(i, 1, m_HeroList.m_warriorList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_warriorList[i].level);
            //cTmp32[31] = 0;
			m_listHeroAoShi->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_warriorList[i].score);
            //cTmp32[31] = 0;
			m_listHeroAoShi->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==XianChongZhiWang)
	{
		m_listHeroXianChong->RemoveAllItems();
		int nListCount = m_HeroList.m_hunterList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroXianChong->AddItem("");
			m_listHeroXianChong->SetItemText(i, 1, m_HeroList.m_hunterList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_hunterList[i].level);
            //cTmp32[31] = 0;
			m_listHeroXianChong->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_hunterList[i].score);
            //cTmp32[31] = 0;
			m_listHeroXianChong->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==ShuiDiShiChuan)
	{
		m_listHeroShuiDi->RemoveAllItems();
		int nListCount = m_HeroList.m_taolistList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroShuiDi->AddItem("");
			m_listHeroShuiDi->SetItemText(i, 1, m_HeroList.m_taolistList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_taolistList[i].level);
            //cTmp32[31] = 0;
			m_listHeroShuiDi->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_taolistList[i].score);
            //cTmp32[31] = 0;
			m_listHeroShuiDi->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==FuJiaYiFang)
	{
		m_listHeroFuJia->RemoveAllItems();
		int nListCount = m_HeroList.m_wizardList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroFuJia->AddItem("");
			m_listHeroFuJia->SetItemText(i, 1, m_HeroList.m_wizardList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_wizardList[i].level);
            //cTmp32[31] = 0;
			m_listHeroFuJia->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_wizardList[i].score);
            //cTmp32[31] = 0;
			m_listHeroFuJia->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==YuanBaoDaHeng)
	{
		m_listHeroYuanBao->RemoveAllItems();
		int nListCount = m_HeroList.m_unionList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroYuanBao->AddItem("");
			m_listHeroYuanBao->SetItemText(i, 1, m_HeroList.m_unionList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].level);
            //cTmp32[31] = 0;
			m_listHeroYuanBao->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].score);
            //cTmp32[31] = 0;
			m_listHeroYuanBao->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==LongPanHuYue)
	{
		m_listHeroLongHu->RemoveAllItems();
		int nListCount = m_HeroList.m_unionList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroLongHu->AddItem("");
			m_listHeroLongHu->SetItemText(i, 1, m_HeroList.m_unionList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].level);
			//cTmp32[31] = 0;
			m_listHeroLongHu->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].score);
			//cTmp32[31] = 0;
			m_listHeroLongHu->SetItemText(i, 3, cTmp32);
		}
	}
	else if (cMetier==DiFuChangKe)
	{
		m_listHeroDiFu->RemoveAllItems();
		int nListCount = m_HeroList.m_unionList.size();
		for (int i=0; i<nListCount; i++)
		{
			m_listHeroDiFu->AddItem("");
			m_listHeroDiFu->SetItemText(i, 1, m_HeroList.m_unionList[i].name );
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].level);
			//cTmp32[31] = 0;
			m_listHeroDiFu->SetItemText(i, 2, cTmp32);
			rt2_snprintf(cTmp32, 32, "%d", m_HeroList.m_unionList[i].score);
			//cTmp32[31] = 0;
			m_listHeroDiFu->SetItemText(i, 3, cTmp32);
		}
	}
	unguard;
}

void UIFormHeroList::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormHeroList::Show()
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_herolist_query);
	pPacket->WriteLong(m_HeroList.m_version);
	NetSend(pPacket);

	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormHeroList::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormHeroList::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormHeroList::OnHeroSortHeaderClick(RtwWidget* sender, void*)
{
	guard;
	unguard;
}

void UIFormHeroList::OnNeedRefresh(RtwWidget* sender, void*)
{
	guard;
	Refresh();
	unguard;
}

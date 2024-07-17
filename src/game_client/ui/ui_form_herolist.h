#ifndef _INC_UI_FORM_HEROLIST_H_
#define _INC_UI_FORM_HEROLIST_H_

#include "herolist_base.h"

class UIFormHeroList
{
public:
	enum CHECKBUTTONTYPE
	{
		AoShiQunXiong,
		XianChongZhiWang,
		ShuiDiShiChuan,
		FuJiaYiFang,
		YuanBaoDaHeng,
		LongPanHuYue,
		DiFuChangKe,
		ChuCuolie
	};
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();
	bool IsInHeroList();		// 是在英雄榜还是在诸侯榜中
	char InWhickMetierList();	// 在哪个职业的列表中

	SHeroList m_HeroList; 

private:
	RtwWidget *m_pFrmThis;

	RtwTab* m_pTabHeroList;

	RtwListBox*	m_listHeroAoShi;
	RtwListBox*	m_listHeroXianChong;
	RtwListBox*	m_listHeroFuJia;
	RtwListBox*	m_listHeroShuiDi;
	RtwListBox*	m_listHeroYuanBao;
	RtwListBox*	m_listHeroLongHu;
	RtwListBox*	m_listHeroDiFu;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);

private:
	void OnPreDraw();
	void OnHeroSortHeaderClick(RtwWidget* sender, void*);
	void OnUnionSortHeaderClick(RtwWidget* sender, void*);
	void OnNeedRefresh(RtwWidget* sender, void*);

public:
	UIFormHeroList();
	~UIFormHeroList();
};

#endif // _INC_UI_FORM_HEROLIST_H_

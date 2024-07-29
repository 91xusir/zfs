#ifndef GC_BULLETIN_H
#define GC_BULLETIN_H

#include "core/rt2_core.h"
#include "graph/rt_graph.h"
//using namespace std;


class CBulletinManager
{
public:
	struct SBulletin
	{
		string	content;
		DWORD	gmID;
		DWORD	color;
		char	style;

		int		showCount;	// ×ßÁË¼¸±é
		int		timeLeft;	// 
	};
public:
	CBulletinManager();
	~CBulletinManager();
	void Run(DWORD delta);
	void Clear()
	{
		m_pChatBulletin->Hide();
		m_pFormBulletin->Hide();
		m_NormalBulletins.clear();
		m_UrgentBulletins.clear();
		m_nTimeCounter = 0;
	}

	void OnRecvNewBulletin(const char* content, DWORD gmID, DWORD color, char style);
	
private:
	list<SBulletin>		m_NormalBulletins;
	list<SBulletin>		m_UrgentBulletins;

	RtwTextBox *		m_pChatBulletin;
	RtwForm*			m_pFormBulletin;

	int					m_nTimeCounter;
};

#endif

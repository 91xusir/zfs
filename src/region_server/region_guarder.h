//---------------------------------------------------------------------------//
//created by tony 05.05.20
//---------------------------------------------------------------------------//

#ifndef _REGION_GUARDER_H
#define _REGION_GUARDER_H

typedef struct 
{
	short	id;
	int		last_time_stamp;
	bool	isdead;

	SNpc	guarder_info;
}SGInfo;

typedef struct
{
	DWORD	o_id;
	long	pos[3];
	char	dir;
	bool	isdead;
	SNpc	guarder_info;
}STCWarGuarderInfo;

//--------------------add start by tony 05.06.27---------------------------//
class CGuarderLair
{
public:
	CGuarderLair();
	~CGuarderLair(){};

public:
	void Init(short id, short max_cnt, long* area_begin, long* area_end, char dir, char* home);

public:
	short	m_npc_id;
	short	m_max_count;
	short	m_cur_count;
	long	m_area_begin[3];
	long	m_area_end[3];
	char	m_dir;
	std::string m_home;
};
//--------------------add end   by tony 05.06.27---------------------------//

class CRegionGuarderMgr
{
public:	
	void	Init();
	
	void	CreateGuarder();

	void	KillGuarder(short id);

	SNpc*	GetGuarderById(short id);

//--------------add start by tony 05.06.14----------------------------//
	//城战npc相关
	void	CreateAllCastleWarGuarder(std::string feod_name);

	void	KillAllCastleWarGuarder(std::string feod_name);

	SNpc*	GetCastleWarGuarderById(short id);

	bool	LoadCastleWarGuarderTbl(const char* file_name);

	void	CreateGuarder(CGuarderLair* p_lair);
//--------------add end   by tony 05.06.14----------------------------//

public:
	CRegionGuarderMgr();

public:
	std::vector<SGInfo>		m_guarder_info;

	float	m_interval_factor;
	int		m_max_counter;
	int		m_curr_counter;	

//--------------add start by tony 05.06.14----------------------------//
	//城战npc相关
	std::vector<STCWarGuarderInfo>		m_castlewar_guarder;
//--------------add end   by tony 05.06.14----------------------------//
};

extern CRegionGuarderMgr g_GuarderMgr;

#endif // _REGION_GUARDER_H

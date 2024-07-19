#ifndef REGION_UNION_H
#define REGION_UNION_H


class CUnionIconMgr
{
public:
	struct SUnionIconData
	{
		string fileName;
		char* data;
		int dataLen;
	};
	
	vector<SUnionIconData> m_arrIcon;
	CUnionIconMgr() {}
	~CUnionIconMgr()
	{
		for (int i=0; i<(int)m_arrIcon.size(); i++)
			delete[] m_arrIcon[i].data;
	}
	SUnionIconData *Find(string fileName);
};

class CRegionFeodMgr
{
public:
	vector<SFeod> m_arrFeods;
	SFeod *Find(string name);

public:
	CRegionFeodMgr();
	~CRegionFeodMgr();
	void Clear();
	bool AddFeodInfo(SFeod& feod);
	void GetFeodInfo(string feodName, DWORD &dwCurrentUnion, int &rate);
	bool AddIncome(string name, int income);
};

extern CUnionIconMgr	g_UnionIconMgr;
extern CRegionFeodMgr	g_RegionFeodMgr;

#endif//REGION_UNION_H


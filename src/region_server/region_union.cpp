#include "region.h"
#include "region_union.h"

CUnionIconMgr	g_UnionIconMgr;
CRegionFeodMgr	g_RegionFeodMgr;

CUnionIconMgr::SUnionIconData *CUnionIconMgr::Find(string fileName)
{
	for (int i=0; i<(int)m_arrIcon.size(); i++)
	{
		if (m_arrIcon[i].fileName==fileName)
			return &(m_arrIcon[i]);
	}
	return NULL;
}


CRegionFeodMgr::CRegionFeodMgr()
{
	Clear();
}

CRegionFeodMgr::~CRegionFeodMgr()
{
}

void CRegionFeodMgr::Clear()
{
	m_arrFeods.clear();
}

SFeod* CRegionFeodMgr::Find(string name)
{
	for (int i=0; i<(int)m_arrFeods.size(); i++)
	{
		if (m_arrFeods[i].name==name)
			return &(m_arrFeods[i]);
	}
	return NULL;
}

void CRegionFeodMgr::GetFeodInfo(string feodName, DWORD &dwCurrentUnion, int &rate)
{
	dwCurrentUnion = 0;
	rate = 0;
	SFeod* pFeod = Find(feodName);
	if (!pFeod)
	{
		ERR1("未知的采邑地点：%s\n", feodName.c_str());
		return;
	}
	dwCurrentUnion = pFeod->unionID;
	rate = pFeod->taxRate;
	if (dwCurrentUnion!=0)
	{
		CHECK(rate>=UNION_TAXRATE_MIN && rate<=UNION_TAXRATE_MAX);
	}
}

bool CRegionFeodMgr::AddIncome(string name, int income)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_feod);
	packet.WriteShort(r2g_feod_new_income);
	packet.WriteString((char*)name.c_str());
	packet.WriteLong(income);
	g_region->m_gws.EndSend();

	return true;
}

bool CRegionFeodMgr::AddFeodInfo(SFeod& feod)
{
	m_arrFeods.push_back(feod);
	return true;
}


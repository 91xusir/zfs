#ifndef _GW_FEOD_H
#define _GW_FEOD_H

class CFeodMgr;

class CFeod
{
	friend class CFeodMgr;
public:
	SFeod	m_data;
	int		m_lastIncomeDay;

public:
	CFeod()
	{
		m_lastIncomeDay = -1;
	}
	~CFeod() {}

	bool ChangUnion(DWORD id);
	bool SetTaxRate(int rate);
	bool WithdrawBank(int withdraw);
	bool AddIncome(int income);
	bool SaveToDB();

	void OnCurrentUnionChanged(DWORD oldUnion, DWORD newUnion);

	int GetBank() { return m_data.bank; }
	int GetIncomeToday() { return m_data.incomeToday; }
	DWORD GetUnionID() { return m_data.unionID; }
	int GetTaxRate() { return m_data.taxRate; }
	std::string GetName() { return m_data.name; }
};

class CFeodMgr
{
public:
	CFeodMgr();
	~CFeodMgr();
	bool Init();
	void Tick();

	CFeod* FindFeod(string name);
	void OnRegionRegister(int regionID);
	void OnUnionDeleted(DWORD unionID);

public:
	std::vector<CFeod> m_feods;
};

extern "C" CFeodMgr g_FeodMgr;

CFeodMgr* GetFeodMgr();


#endif//_GW_FEOD_H


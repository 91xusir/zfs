#ifndef ITEM_TRADE_H
#define ITEM_TRADE_H

class CTrade   //交易系统
{
public:
	CTrade(CItemContainerBase* pCon1, CItemContainerBase* pCon2, DWORD Player1, DWORD Player2, CItemManager* pItemMgr);
   ~CTrade();
	void Reset(CItemContainerBase *pCon1, CItemContainerBase* pCon2, DWORD Player1, DWORD Player2);
	bool IsValid() { return m_pCon1&&m_pCon2; }

	void Lock(CItemContainerBase *pCon);
	void UnLock();
	bool IsLocked(CItemContainerBase *pCon);
	void Agree(CItemContainerBase *pCon);
	bool IsAgreed(CItemContainerBase *pCon);

	bool PutIn(CItemContainerBase *pCon, int pageFrom, int iFrom, int jFrom, int iTo, int jTo, char bMissionBag);
	bool PutIn(CItemContainerBase *pCon, int money);
	bool TakeOut(CItemContainerBase *pCon, int iFrom, int jFrom, int pageTo, int iTo, int jTo, bool bPet = false);
	bool TakeOut(CItemContainerBase *pCon, int money);

	bool Deal();

	DWORD GetOtherPlayer(DWORD playerID)
	{
		if (playerID==m_dwPlayer1)
			return m_dwPlayer2;
		else 
			return m_dwPlayer1;
		CHECK(0);
		return 0;
	}

	CItemContainerBase* GetOtherContainer(CItemContainerBase *pCon)
	{
		if (pCon==m_pCon1)
			return m_pCon2;
		else if (pCon==m_pCon2)
			return m_pCon1;
		CHECK(0);
		return NULL;
	}

	CBag* GetTradeBag(CItemContainerBase *pCon)
	{
		if (pCon==m_pCon1)
			return &m_bag1;
		else if (pCon==m_pCon2)
			return &m_bag2;
		CHECK(0);
		return NULL;
	}

	CBag* GetTradeOtherBag(CItemContainerBase *pCon)
	{
		if (pCon==m_pCon1)
			return &m_bag2;
		else if (pCon==m_pCon2)
			return &m_bag1;
		CHECK(0);
		return NULL;
	}

	CBag* GetTradePetBag(CItemContainerBase *pCon)
	{
		if (pCon==m_pCon1)
			return &m_Petbag1;
		else if (pCon==m_pCon2)
			return &m_Petbag2;
		CHECK(0);
		return NULL;
	}

	CBag* GetTradeOtherPetBag(CItemContainerBase *pCon)
	{
		if (pCon==m_pCon1)
			return &m_Petbag2;
		else if (pCon==m_pCon2)
			return &m_Petbag1;
		CHECK(0);
		return NULL;
	}

	void SetItemCount(CItemContainerBase *pCon, int CommonCount, int MissionCount, int PetCount)
	{
		if (pCon==m_pCon1)
		{
			m_nCommonItemCount1 = CommonCount;
			m_nMissionItemCount1 = MissionCount;
			m_nPetItemCount1 = PetCount;
		}
		else if (pCon==m_pCon2)
		{
			m_nCommonItemCount2 = CommonCount;
			m_nMissionItemCount2 = MissionCount;
			m_nPetItemCount2 = PetCount;
		}
		else 
			CHECK(0);
	}

	CBag& GetPetbag1() {return m_Petbag1;}
	CBag& GetPetbag2() {return m_Petbag2;}
private:
	int m_nCommonItemCount1;
	int m_nMissionItemCount1;
	int m_nPetItemCount1;
	int m_nCommonItemCount2;
	int m_nMissionItemCount2;
	int m_nPetItemCount2;

	CBag m_bag1, m_bag2;
	CBag m_Petbag1, m_Petbag2;
	bool m_bLock1, m_bLock2;
	bool m_bAgreeDeal1, m_bAgreeDeal2;
	CItemContainerBase *m_pCon1;
	CItemContainerBase *m_pCon2;

	DWORD m_dwPlayer1;
	DWORD m_dwPlayer2;
};


#endif//ITEM_TRADE_H

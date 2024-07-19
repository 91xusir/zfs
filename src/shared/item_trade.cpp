#include "region_client.h"
#include "item_trade.h"

CTrade::CTrade(CItemContainerBase* pCon1, CItemContainerBase* pCon2, DWORD Player1, DWORD Player2, CItemManager* pItemMgr)
	:m_bag1(TRADE_GRID_LINE_COUNT, TRADE_GRID_COLUMN_COUNT, 1, pItemMgr), m_bag2(TRADE_GRID_LINE_COUNT, TRADE_GRID_COLUMN_COUNT, 1, pItemMgr), 
	 m_Petbag1(ITEM_PET_BAG_LINE, ITEM_PET_BAG_COLUMN, 1, pItemMgr), m_Petbag2(ITEM_PET_BAG_LINE, ITEM_PET_BAG_COLUMN, 1, pItemMgr)
{
	Reset(pCon1, pCon2, Player1, Player2);
}

CTrade::~CTrade()
{
}

void CTrade::Reset(CItemContainerBase *pCon1, CItemContainerBase* pCon2, DWORD Player1, DWORD Player2)
{
	guard;

	m_pCon1 = pCon1;
	m_pCon2 = pCon2;
	m_dwPlayer1 = Player1;
	m_dwPlayer2 = Player2;
	if ( IsValid() )
	{
		m_bLock1 = false;
		m_bLock2 = false;
		m_bAgreeDeal1 = false;
		m_bAgreeDeal2 = false;
		m_bag1.Clear();
		m_bag2.Clear();
		m_Petbag1.Clear();
		m_Petbag2.Clear();
		m_nCommonItemCount1 = 0;
		m_nMissionItemCount1 = 0;
		m_nPetItemCount1 = 0;
		m_nCommonItemCount2 = 0;
		m_nMissionItemCount2 = 0;
		m_nPetItemCount2 = 0;
	}

	unguard;
}

bool CTrade::PutIn(CItemContainerBase *pCon, int pageFrom, int iFrom, int jFrom, int iTo, int jTo, char bMissionBag)
{
	guard;

	SItemID itemFrom;
	if (bMissionBag == 1)
		itemFrom = pCon->m_MissionBag.GetItem(pageFrom, iFrom, jFrom);
	else if (bMissionBag == 2)
		itemFrom = pCon->m_PetBag.GetItem(pageFrom, iFrom, jFrom);
	else
		itemFrom = pCon->m_Bag.GetItem(pageFrom, iFrom, jFrom);

	BOOL_ENSURE( ItemCanTrade(itemFrom) )

	if ( !IsValid() )	return false;
	if (pCon==m_pCon1)
	{
		int tmpPage, tmpI, tmpJ;
		if (m_bLock1)	return false;
		if (ItemIsPet(itemFrom))
			return m_Petbag1.AddItem(itemFrom, &tmpPage, &tmpI, &tmpJ, false);
		else
			return m_bag1.AddItem(itemFrom, &tmpPage, &tmpI, &tmpJ, false);
	}
	else if (pCon==m_pCon2)
	{
		int tmpPage, tmpI, tmpJ;
		if (m_bLock2)	return false;
		if (ItemIsPet(itemFrom))
			return m_Petbag2.AddItem(itemFrom, &tmpPage, &tmpI, &tmpJ, false);
		else
			return m_bag2.AddItem(itemFrom, &tmpPage, &tmpI, &tmpJ, false);
	}
	else
		return false;

	unguard;
}

bool CTrade::PutIn(CItemContainerBase *pCon, int money)
{
	guard;

	if ( !IsValid() )			return false;
	if ( !MoneyIsValid(money))	return false;
	if (pCon==m_pCon1)
	{
		if (m_bLock1)	return false;
		m_bag1.SetMoney(money);
		return true;
	}
	else if (pCon==m_pCon2)
	{
		if (m_bLock2)	return false;
		m_bag2.SetMoney(money);
		return true;
	}
	else
		return false;

	unguard;
}

bool CTrade::TakeOut(CItemContainerBase *pCon, int iFrom, int jFrom, int pageTo, int iTo, int jTo, bool bPet/* = false*/)
{
	guard;

	if ( !IsValid() )	return false;
	SItemID item;
	if (pCon==m_pCon1)
	{
		if (m_bLock1)	return false;
		if (bPet)
			return m_Petbag1.RemoveItem(0, iFrom, jFrom);
		else
			return m_bag1.RemoveItem(0, iFrom, jFrom);
	}
	else if (pCon==m_pCon2)
	{
		if (m_bLock2)	return false;
		if (bPet)
			return m_Petbag2.RemoveItem(0, iFrom, jFrom);
		else
			return m_bag2.RemoveItem(0, iFrom, jFrom);
	}
	else
		return false;

	unguard;
}

bool CTrade::TakeOut(CItemContainerBase *pCon, int money)
{
	guard;

	if ( !IsValid() )	        return false;
    if ( !MoneyIsValid(money))	return false;
	if (pCon==m_pCon1)
	{
		if (m_bLock1)	return false;
        BOOL_ENSURE(m_bag1.GetMoney() >= money)
		pCon->AddMoney(money);
		m_bag1.RemoveMoney(money);
		return true;
	}
	else if (pCon==m_pCon2)
	{
		if (m_bLock2)	return false;
        BOOL_ENSURE(m_bag2.GetMoney() >= money)
		pCon->AddMoney(money);
		m_bag2.RemoveMoney(money);
		return true;
	}
	else
		return false;

	unguard;
}

void CTrade::Lock(CItemContainerBase *pCon)
{
	if (pCon==m_pCon1)			m_bLock1 = true;
	else if (pCon==m_pCon2)		m_bLock2 = true;
}

void CTrade::UnLock()
{
	m_bLock1 = false;
	m_bLock2 = false;
	m_bAgreeDeal1 = m_bAgreeDeal2 = false;
}

bool CTrade::IsLocked(CItemContainerBase *pCon)
{
	if (pCon==m_pCon1)			return m_bLock1;
	else if (pCon==m_pCon2)		return m_bLock2;
	else						return false;
}

void CTrade::Agree(CItemContainerBase *pCon)
{
	if (pCon==m_pCon1)			m_bAgreeDeal1 = true;
	else if (pCon==m_pCon2)		m_bAgreeDeal2 = true;
}

bool CTrade::IsAgreed(CItemContainerBase *pCon)
{
	if (pCon==m_pCon1)			return m_bAgreeDeal1;
	else if (pCon==m_pCon2)		return m_bAgreeDeal2;
	else						return false;
}

bool CTrade::Deal()
{
	guard;

	BOOL_ENSURE(m_bLock1 && m_bLock2)
	int nTrade1 = m_bag1.Count();
	int nTrade2 = m_bag2.Count();
	int nTradePet1 = m_Petbag1.Count();
	int nTradePet2 = m_Petbag2.Count();
	BOOL_ENSURE(nTrade1==m_nCommonItemCount1+m_nMissionItemCount1)
	BOOL_ENSURE(nTrade2==m_nCommonItemCount2+m_nMissionItemCount2)
	BOOL_ENSURE(nTradePet1==m_nPetItemCount1)
	BOOL_ENSURE(nTradePet2==m_nPetItemCount2)
	int nFreeCommon1 = m_pCon1->m_Bag.FreeSpace();
	int nFreeMission1 = m_pCon1->m_MissionBag.FreeSpace();
	int nFreePet1 = m_pCon1->m_PetBag.FreeSpace();
	int nFreeCommon2 = m_pCon2->m_Bag.FreeSpace();
	int nFreeMission2 = m_pCon2->m_MissionBag.FreeSpace();
	int nFreePet2 = m_pCon2->m_PetBag.FreeSpace();
	int nTmpCommon = m_nCommonItemCount2 - m_nCommonItemCount1;
	int nTmpMission = m_nMissionItemCount2 - m_nMissionItemCount1;
	int nTmpPet = m_nPetItemCount2 - m_nPetItemCount1;

    if (m_bag2.GetMoney()>m_pCon2->GetMoney())
    {
        return false;
    }

    if (m_bag1.GetMoney()>m_pCon1->GetMoney())
    {
        return false;
    }

	if ( nTmpPet <= nFreePet1 && -nTmpPet <= nFreePet2 && nTmpCommon<=nFreeCommon1 && nTmpMission<=nFreeMission1 && -nTmpCommon<=nFreeCommon2 && -nTmpMission<=nFreeMission2)
	{
        vector<SItemID> items1;
		vector<SItemID> items2;
		vector<SItemID> itemsPet1;
		vector<SItemID> itemsPet2;
        m_bag1.ToItemsArray(items1);
		m_bag2.ToItemsArray(items2);
		m_Petbag1.ToItemsArray(itemsPet1);
		m_Petbag2.ToItemsArray(itemsPet2);

		// 检查是否有已绑定道具
		int i;
		for (i=0; i<(int)items1.size(); i++)
		{
			if (items1[i].cBind==IB_Binded)
				return false;
		}
		for (i=0; i<(int)items2.size(); i++)
		{
			if (items2[i].cBind==IB_Binded)
				return false;
		}
		for (i=0; i<(int)itemsPet1.size(); i++)
		{
			if (itemsPet1[i].cBind==IB_Binded)
				return false;
		}
		for (i=0; i<(int)itemsPet2.size(); i++)
		{
			if (itemsPet2[i].cBind==IB_Binded)
				return false;
		}

		BOOL_ENSURE( CItemContainerBase::ExchangeItems(m_pCon1, items1, m_pCon2, items2) )
		BOOL_ENSURE( CItemContainerBase::ExchangeItems(m_pCon1, itemsPet1, m_pCon2, itemsPet2) )
        m_pCon2->AddMoney(m_bag1.GetMoney());
        m_pCon2->RemoveMoney(m_bag2.GetMoney());
        m_pCon1->AddMoney(m_bag2.GetMoney());
        m_pCon1->RemoveMoney(m_bag1.GetMoney());

        return true;
    }
    else
    {
        return false;
    }

	unguard;
}

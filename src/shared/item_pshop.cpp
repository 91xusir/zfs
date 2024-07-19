#include "region_client.h"
#include "item_pshop.h"

CPrivateShop::CPrivateShop(CItemManager* pItemMgr)
:m_sellItemBag(PSHOP_LINE_MAX, PSHOP_GRID_COLUMN_COUNT, 1, pItemMgr),
m_buyItemBag(PSHOP_LINE_MAX_BUY,PSHOP_GRID_COLUMN_COUNT_BUY,1,pItemMgr)
{
	Reset();
}

CPrivateShop::~CPrivateShop()
{
}

void CPrivateShop::Reset()
{
	guard;

	m_playerID = 0;
	m_pItemCon = NULL;
	m_name = "";
	m_price.clear();
	m_sellItemBag.Clear();
	m_buyItemBag.Clear();
	m_bStart = false;
	//m_moneyTotal = 0;

	unguard;
}

void CPrivateShop::SetPlayer(DWORD playerID, CItemContainerBase *pCon)
{
	guard;

	CHECK(playerID!=0 && pCon);

	m_playerID = playerID;
	m_pItemCon = pCon;

	unguard;
}

bool CPrivateShop::Init(bool bSell, DWORD playerID, CItemContainerBase *pCon,string name)
{
	guard;

	BOOL_ENSURE(playerID!=0 && pCon)
	//Reset();

	//m_bSell = bSell;
	m_playerID = playerID;
	m_pItemCon = pCon;
	if(bSell)
	{
		if(m_name== "" && name!= "")
		{
			char tmpStr[100];
			rt2_snprintf(tmpStr,100,"%s的小货摊",name.c_str());
			m_name= tmpStr;
		}
	}

	return true;

	unguard;
}

bool CPrivateShop::AddSellItem(SItemID item, UINT price)
{
	guard;

	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)
	BOOL_ENSURE( ItemID_IsValid(item) )
//	BOOL_ENSURE( MoneyIsValid(price) )	// @@@@

	int tmpPage, tmpI, tmpJ;
	if ( !m_sellItemBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false) )
		return false;

	m_price[item.id] = price;
	//m_moneyTotal += price;

	return true;

	unguard;
}

bool CPrivateShop::AddSellItem(SItemID item, int i, int j, UINT price)
{
	guard;

	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)
	BOOL_ENSURE( ItemID_IsValid(item) )
	//	BOOL_ENSURE( MoneyIsValid(price) )	// @@@@

	int tmpPage, tmpI, tmpJ;
	if ( !m_sellItemBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false) )
		return false;

	m_price[item.id] = price;
	//m_moneyTotal += price;

	SItemID itemTo = m_sellItemBag.GetItem(0,i,j);
	if(!ItemID_IsValid(itemTo))
	{
		m_sellItemBag.MoveItem(tmpI,tmpJ,i,j);
	}
	return true;

	unguard;
}
bool CPrivateShop::AddBuyItem(SItemID item, UINT price)
{
	guard;
	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)
	BOOL_ENSURE( ItemID_IsValid(item) )
	//	BOOL_ENSURE( MoneyIsValid(price) )	// @@@@

	int tmpPage, tmpI, tmpJ;
	if ( !m_buyItemBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false) )
		return false;

	m_price[item.id] = price;

	return true;
	unguard;
}

bool CPrivateShop::AddBuyItem(SItemID item, int i, int j, UINT price)
{
	guard;
	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)
	BOOL_ENSURE( ItemID_IsValid(item) )
	//	BOOL_ENSURE( MoneyIsValid(price) )	// @@@@

	int tmpPage, tmpI, tmpJ;
	if ( !m_buyItemBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false) )
		return false;

	m_price[item.id] = price;

	SItemID itemTo = m_buyItemBag.GetItem(0,i,j);
	if(!ItemID_IsValid(itemTo))
	{
		m_buyItemBag.MoveItem(tmpI,tmpJ,i,j);
	}
	return true;
	unguard;
}

bool CPrivateShop::RemoveSellItem(int i, int j)
{
	guard;

	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)

	SItemID item = m_sellItemBag.GetItem(0, i, j);
	BOOL_ENSURE ( ItemID_IsValid(item) )

	if ( !m_sellItemBag.RemoveItem(0, i, j) )
		return false;

	//m_moneyTotal -= m_price[item.id];
	m_price.erase(item.id);

	return true;

	unguard;
}

bool CPrivateShop::RemoveSellItem(DWORD itemOID)
{
	guard;

	int page, i, j;
	if ( m_sellItemBag.Find(itemOID, &page, &i, &j) )
	{
		return RemoveSellItem(i, j);
	}
	else		return false;

	unguard;
}

bool CPrivateShop::RemoveBuyItem(int i, int j)
{
	guard;
	BOOL_ENSURE(m_playerID!=0 && m_pItemCon)

	SItemID item = m_buyItemBag.GetItem(0, i, j);
	BOOL_ENSURE ( ItemID_IsValid(item) )

	if ( !m_buyItemBag.RemoveItem(0, i, j) )
		return false;

	m_price.erase(item.id);

	return true;
	unguard;
}

bool CPrivateShop::RemoveBuyItem(DWORD itemOID)
{
	guard;
	int page, i, j;
	if ( m_buyItemBag.Find(itemOID, &page, &i, &j) )
	{
		return RemoveBuyItem(i, j);
	}
	else		return false;
	unguard;
}

bool CPrivateShop::Serialize(CG_CmdPacket *cmd)
{
	guard;

	if ( cmd->IsWritting() )
	{
		// Shop Name
		(*cmd) << m_name;
		// Player ID
		(*cmd) << m_playerID;
		// moneyTotal
		//(*cmd) << m_moneyTotal;
		// bSell
		//(*cmd) << m_bSell;
		// Bag
		m_sellItemBag.Serialize(0, cmd);
		m_buyItemBag.Serialize(0,cmd);
		// Price
		DWORD itemID;
		long price;
		EXT_SPACE::unordered_map<DWORD, UINT>::iterator iter = m_price.begin();
		for (; iter!=m_price.end(); iter++)
		{
			itemID = (*iter).first;
			price = (*iter).second;
			(*cmd) << itemID;
			(*cmd) << price;
		}
		itemID = 0xffffffff;
		(*cmd) << itemID;				// 结束符
	}
	else
	{
		// Shop Name
		(*cmd) << m_name;
		// Player ID
		(*cmd) << m_playerID;
		// moneyTotal
		//(*cmd) << m_moneyTotal;
		// bSell
		//(*cmd) << m_bSell;
		// Bag
		m_sellItemBag.Serialize(0, cmd);
		m_buyItemBag.Serialize(0,cmd);
		// Price
		m_price.clear();
		DWORD itemID;
		long price;
		(*cmd) << itemID;

		int counterForSafe = 0;
		while (itemID != 0xffffffff && counterForSafe++<200)
		{
			(*cmd) << price;

			if (!MoneyIsValid(price) && price != -99)
				return false;

			m_price[itemID] = price;

			(*cmd) << itemID;
		}

		if (counterForSafe >= 200)
			return false;

	}

	return true;

	unguard;
}

bool CPrivateShop::Start()
{
	guard;

	BOOL_ENSURE( IsValid() )
	m_bStart = true;
	return true;

	unguard;
}

bool CPrivateShop::Stop()
{
	guard;

	BOOL_ENSURE( IsValid() )
	m_bStart = false;
	return true;

	unguard;
}

UINT CPrivateShop::GetPrice(DWORD itemOID)
{
	guard;

	EXT_SPACE::unordered_map<DWORD, UINT>::iterator iter = m_price.find(itemOID);
	if ( iter != m_price.end() )
	{
		return (*iter).second;
	}
	return 0xffffffff;

	unguard;
}

bool CPrivateShop::SetPrice(UINT price,DWORD itemOID)
{
	guard;
	EXT_SPACE::unordered_map<DWORD, UINT>::iterator iter = m_price.find(itemOID);
	if ( iter != m_price.end() )
	{
		(*iter).second = price;
		return true;
	}
	return false;
	unguard;
}

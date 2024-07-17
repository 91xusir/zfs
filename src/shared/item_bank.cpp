//#include "item_bank.h"
//
//CItemBank::CItemBank(DWORD playerID, CItemManager *pItemMgr)
//:m_bankBag(BANK_LINE_MAX, BANK_GRID_COLUMN_COUNT, 1)
//{
//	Reset();
//	if (playerID > 0)
//	{
//		m_playerID = playerID;
//	}
//	m_pItemMgr = pItemMgr;
//}
//
//CItemBank::~CItemBank()
//{
//}
//
//bool CItemBank::GetBankItems(DWORD playerID, CBag &outBagBank)
//{
//	if ( outBagBank.GetLineCount()!=BANK_LINE_MAX  ||  outBagBank.GetColCount()!=BANK_GRID_COLUMN_COUNT  ||  outBagBank.GetPageCount()!=1 )
//		return false;
//
//	//SItemID item;
//	//item.type = 0x11000274;
//	//for (int i=1; i<21; i++)
//	//{
//	//	item.oid = i*1000;
//	//	BOOL_ENSURE( outBagBank.AddItem(item) )
//	//}
//	//item.type = 0x41019178;
//	//for (int i=1; i<6; i++)
//	//{
//	//	item.oid = i*10000;
//	//	BOOL_ENSURE( outBagBank.AddItem(item) )
//	//}
//	//outBagBank.AddMoney(200);
//
//	return true;
//}
//
//bool CItemBank::StoreItems(DWORD playerID, CBag &inAddBag)
//{
//	if ( inAddBag.IsEmpty() )		return true;
//	// @@@@
//
//	return true;
//}
//
//bool CItemBank::WithdrawItems(DWORD playerID, CBag &inRemoveBag)
//{
//	if ( inRemoveBag.IsEmpty() )		return true;
//	// @@@@
//
//	return true;
//}
//
//void CItemBank::Reset()
//{
//	m_playerID = 0;
//	m_bankBag.Clear();
//}
//
//bool CItemBank::AddItem(SItemID item)
//{
//	if ( !m_bankBag.AddItem(item) )
//		return false;
//
//	return true;
//}
//
//bool CItemBank::RemoveItem(SItemID item)
//{
//	if ( !m_bankBag.RemoveItem(item) )
//		return false;
//
//	return true;
//}
//
//bool CItemBank::AddMoney(int money)
//{
//	m_bankBag.AddMoney(money);
//	return true;
//}


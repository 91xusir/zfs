#ifndef ITEM_BANK_H
#define ITEM_BANK_H

//#define BANK_GRID_LINE_COUNT		3
//#define BANK_GRID_COLUMN_COUNT		6
//
//#define BANK_LINE_MAX				8
//
//class CItemBank
//{
//public:
//	CItemBank(DWORD playerID, CItemManager *pItemMgr);
//   ~CItemBank();
//   void Reset();
//
//   bool AddItem(SItemID item);
//   bool RemoveItem(SItemID item);
//   bool AddMoney(int money);
//
//   // 以下3个函数只做与DB相关的操作，并不操作玩家道具
//   static bool GetBankItems(DWORD playerID, CBag &outBagBank);
//   static bool StoreItems(DWORD playerID, CBag &inAddBag);
//   static bool WithdrawItems(DWORD playerID, CBag &inRemoveBag);
//
//private:
//	DWORD m_playerID;
//	CBag m_bankBag;
//	CItemManager *m_pItemMgr;
//};

#endif//ITEM_BANK_H

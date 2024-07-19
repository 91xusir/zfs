#ifndef ITEM_PSHOP_H
#define ITEM_PSHOP_H

// 说明，此处收购物品ID是DWROD的最大值逆向取值，所有可收购物品都是这样处理
// 特此说明，避免出现假ID跟真正物品ID相同的情况
class CPrivateShop
{
public:
	CPrivateShop(CItemManager* pItemMgr);
   ~CPrivateShop();
	void Reset();
	bool IsValid() { return m_playerID!=0; }
	bool IsStart() { return m_bStart; }
	//bool IsSell() { return m_bSell; }
	string GetName() { return m_name; }
	void SetName(string& name) { m_name = name; }
	void SetName(char* name) { m_name = name; }
	DWORD GetPlayerID() { return m_playerID; }
	CBag* GetSellItemBag() { return &m_sellItemBag; }
	CBag* GetBuyItemBag() { return &m_buyItemBag; }
	UINT GetPrice(DWORD itemOID);
	bool SetPrice(UINT price,DWORD itemOID);
	//UINT GetMoneyTotal() { return m_moneyTotal; }

	bool Init(bool bSell, DWORD playerID, CItemContainerBase *pCon,string name= "");
	bool Serialize(CG_CmdPacket *cmd);
	void SetPlayer(DWORD playerID, CItemContainerBase *pCon);

	bool AddSellItem(SItemID item, UINT price);
	bool AddSellItem(SItemID item, int i, int j, UINT price);
	bool AddBuyItem(SItemID item, UINT price);
	bool AddBuyItem(SItemID item, int i, int j, UINT price);
	bool RemoveSellItem(int i, int j);
	bool RemoveSellItem(DWORD itemOID);
	bool RemoveBuyItem(int i, int j);
	bool RemoveBuyItem(DWORD itemOID);
	void RemoveAllPrice(){m_price.clear();}
	bool Start();
	bool Stop();

private:
	//bool m_bSell;									// 卖东西还是买东西
	string m_name;									// 个人商店名称
	DWORD m_playerID;								// 玩家ID
	CItemContainerBase *m_pItemCon;					// 玩家道具栏
	EXT_SPACE::unordered_map<DWORD,UINT> m_price;		// 商品价格
	CBag m_sellItemBag;								// 商品包
	CBag m_buyItemBag;
	bool m_bStart;									// 是否已开始摆摊
	//long m_moneyTotal;								// 总价格
};

#endif//ITEM_PSHOP_H

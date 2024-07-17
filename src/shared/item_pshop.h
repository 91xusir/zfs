#ifndef ITEM_PSHOP_H
#define ITEM_PSHOP_H

// ˵�����˴��չ���ƷID��DWROD�����ֵ����ȡֵ�����п��չ���Ʒ������������
// �ش�˵����������ּ�ID��������ƷID��ͬ�����
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
	//bool m_bSell;									// ��������������
	string m_name;									// �����̵�����
	DWORD m_playerID;								// ���ID
	CItemContainerBase *m_pItemCon;					// ��ҵ�����
	EXT_SPACE::unordered_map<DWORD,UINT> m_price;		// ��Ʒ�۸�
	CBag m_sellItemBag;								// ��Ʒ��
	CBag m_buyItemBag;
	bool m_bStart;									// �Ƿ��ѿ�ʼ��̯
	//long m_moneyTotal;								// �ܼ۸�
};

#endif//ITEM_PSHOP_H

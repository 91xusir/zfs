
#include "auction_base.h"

CAuctionItem::CAuctionItem()
:   m_lAuctionId(-1),
    m_dwBargainorAcc(0),
	m_dwBargainorChr(0),
	m_strBargainorChr(""),
	m_dwPurchaserAcc(0),
	m_dwPurchaserChr(0),
	m_strPurchaserChr(""),
	m_dwPassTime(0),
	m_dwDuration(0),
	m_dwHandlingCharge(0),
	m_dwBasePrice(0),
	m_dwBidPrice(0),
	m_dwUniquePrice(0)
{
	//

}

CAuctionItem::CAuctionItem(const CAuctionItem& rhs)
{
    m_lAuctionId     = rhs.m_lAuctionId;
	m_id			 = rhs.m_id;
	m_dwBargainorAcc = rhs.m_dwBargainorAcc;
	m_dwBargainorChr = rhs.m_dwBargainorChr;
	m_strBargainorChr= rhs.m_strBargainorChr;
	m_dwPurchaserAcc = rhs.m_dwPurchaserAcc;
	m_dwPurchaserChr = rhs.m_dwPurchaserChr;
	m_strPurchaserChr= rhs.m_strPurchaserChr;
	m_dwPassTime	 = rhs.m_dwPassTime;
	m_dwDuration	 = rhs.m_dwDuration;
	m_dwHandlingCharge	= rhs.m_dwHandlingCharge;
	m_dwBasePrice		= rhs.m_dwBasePrice;
	m_dwBidPrice		= rhs.m_dwBidPrice;
	m_dwUniquePrice		= rhs.m_dwUniquePrice;
}

CAuctionItem::~CAuctionItem()
{
	//
}

bool CAuctionItem::Serialize(CG_CmdPacket *cmd)
{
	guard;

	*cmd << m_lAuctionId;
	m_id.Serialize(cmd);
	*cmd << m_dwBargainorAcc;
	*cmd << m_dwBargainorChr;
	*cmd << m_strBargainorChr;
	*cmd << m_dwPurchaserAcc;
	*cmd << m_dwPurchaserChr;
	*cmd << m_strPurchaserChr;
	*cmd << m_dwPassTime;
	*cmd << m_dwDuration;
	*cmd << m_dwHandlingCharge;
	*cmd << m_dwBasePrice;
	*cmd << m_dwBidPrice;
 	*cmd << m_dwUniquePrice;

	if (cmd->IsError())
		return false;
	else 
		return true;

	unguard;
}

bool CSalesroom::AddItem(CAuctionItem item, int type, int who)
{
	switch(type)
	{
	case AUCTION_ITEM_DEALING:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
			{
				if((*it).ItemID().id == item.ItemID().id)
				{
					RtCoreLog().Info("Salesroom [DealingItem] have had this item [%d]\n", item.ItemID().id);
					return false;
				}
			}

			m_DealingItemList.push_back(item);
		}
		break;
	case AUCTION_ITEM_DEAL_SUCCESS:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
			{
				if((*it).ItemID().id == item.ItemID().id)
				{
					//RtCoreLog().Info("Salesroom [DealSuccessItem] have had this item [%d]\n", item.ItemID().id);
					if(who == AUCTION_BUYER)
					{
						(*it).Purchaser(item.Purchaser());
						(*it).PurchaserChr((char*)item.PurchaserChr().c_str());
						(*it).PurchaserChrID(item.PurchaserChrID());
					}else
					{
						(*it).Bargainor(item.Bargainor());
						(*it).BargainorChr((char*)item.BargainorChr().c_str());
						(*it).BargainorChrID(item.BargainorChrID());
					}
					return true;
				}
			}

			m_DealSuccessItemList.push_back(item);
		}
		break;
	case AUCTION_ITEM_DEAL_FAILED:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
			{
				if((*it) == item)
				{
					RtCoreLog().Info("Salesroom [DealFailedItem] have had this item [%d]\n", item.ItemID().id);
					return false;
				}
			}

			m_DealFailedItemList.push_back(item);
		}
		break;
	default:
		RtCoreLog().Info("Unknown item type [%d]\n", type);
		return false;
		break;
	}
	
	return true;
}

bool CSalesroom::DelItem(CAuctionItem item, int type)
{
	switch(type)
	{
	case AUCTION_ITEM_DEALING:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
			{
				if((*it).ItemID() == item.ItemID())
				{
					m_DealingItemList.erase(it);
					return true;
				}
			}
		}
		break;
	case AUCTION_ITEM_DEAL_SUCCESS:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
			{
				if((*it).ItemID() == item.ItemID())
				{
					m_DealSuccessItemList.erase(it);
					return true;
				}
			}
		}
		break;
	case AUCTION_ITEM_DEAL_FAILED:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
			{
				if((*it) == item)
				{
					m_DealFailedItemList.erase(it);
					return true;
				}
			}
		}
		break;
	default:
		RtCoreLog().Info("Unknown item type [%d]\n", type);
		return false;
		break;
	}

	return false;
}

bool CSalesroom::UpdateItem(CAuctionItem item, int type)
{
	switch(type)
	{
	case AUCTION_ITEM_DEALING:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
			{
				if((*it).ItemID().id == item.ItemID().id)
				{
					(*it) = item;
					return true;
				}
			}
		}
		break;
	case AUCTION_ITEM_DEAL_SUCCESS:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
			{
				if((*it).ItemID().id == item.ItemID().id)
				{
					(*it) = item;
					return true;
				}
			}
		}
		break;
	case AUCTION_ITEM_DEAL_FAILED:
		{
			stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
			{
				if((*it).ItemID().id == item.ItemID().id)
				{
					(*it) = item;
					return true;
				}
			}
		}
		break;
	default:
		RtCoreLog().Info("Unknown item type [%d]\n", type);
		return false;
		break;
	}

	RtCoreLog().Info("Update Item Failed [%d]\n", item.ItemID().id);
	return false;
}

int CSalesroom::GetSaleItemNumByAcc(DWORD acc_id)
{
	int cnt = 0;

	stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	{
		if((*it).Bargainor() == acc_id)
		{
			cnt++;
		}
	}

	return cnt;
}

int CSalesroom::GetBidItemNumByAcc(DWORD acc_id)
{
	int cnt = 0;

	stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	{
		if((*it).Purchaser() == acc_id)
		{
			cnt++;
		}
	}

	return cnt;
}

//int CSalesroom::GetItemNumByClass(CAuctionItem* item)
//{
	//int cnt		= 0;
	//int klass	= 0;

	//SItemBase* pItemClass = g_ItemMgr.GetItem(item->ItemID().type);
	//if (!pItemClass)
	//{
	//	return 0;
	//}

	//BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//if ( cTmp==0x0F)
	//{
	//	klass = AUCTION_METIER_ALL;
	//}
	//if ( (cTmp & 0x01) != 0x0) //"战士和";
	//{
	//	klass = AUCTION_METIER_WARRIOR;
	//}
	//if ( (cTmp & 0x02) != 0x0) //"女猎手和";
	//{
	//	klass = AUCTION_METIER_HUNTER;
	//}
	//if ( (cTmp & 0x04) != 0x0) //"道士和";
	//{
	//	klass = AUCTION_METIER_TAOIST;
	//}
	//if ( (cTmp & 0x08) != 0x0) //"术士和";
	//{
	//	klass = AUCTION_METIER_WIZARD;
	//}

	//switch(klass)
	//{
	//case AUCTION_METIER_ALL:
	//	{
	//		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	//		{
	//			SItemID item = (*it).ItemID();
	//			SItemBase* pItemClass = g_ItemMgr.GetItem(item.type);
	//			if (!pItemClass) return 0;
	//			BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//			if (cTmp==0x0F)
	//			{
	//				cnt++;
	//			}
	//		}
	//	}
	//	break;
	//case AUCTION_METIER_WARRIOR:
	//	{
	//		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	//		{
	//			SItemID item = (*it).ItemID();
	//			SItemBase* pItemClass = g_ItemMgr.GetItem(item.type);
	//			if (!pItemClass) return 0;
	//			BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//			if ( (cTmp & 0x01) != 0x0)
	//			{
	//				cnt++;
	//			}
	//		}
	//	}
	//	break;
	//case AUCTION_METIER_HUNTER:
	//	{
	//		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	//		{
	//			SItemID item = (*it).ItemID();
	//			SItemBase* pItemClass = g_ItemMgr.GetItem(item.type);
	//			if (!pItemClass) return 0;
	//			BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//			if ( (cTmp & 0x02) != 0x0)
	//			{
	//				cnt++;
	//			}
	//		}
	//	}
	//	break;
	//case AUCTION_METIER_TAOIST:
	//	{
	//		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	//		{
	//			SItemID item = (*it).ItemID();
	//			SItemBase* pItemClass = g_ItemMgr.GetItem(item.type);
	//			if (!pItemClass) return 0;
	//			BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//			if ( (cTmp & 0x04) != 0x0)
	//			{
	//				cnt++;
	//			}
	//		}
	//	}
	//	break;
	//case AUCTION_METIER_WIZARD:
	//	{
	//		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	//		{
	//			SItemID item = (*it).ItemID();
	//			SItemBase* pItemClass = g_ItemMgr.GetItem(item.type);
	//			if (!pItemClass) return 0;
	//			BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	//			if ( (cTmp & 0x08) != 0x0)
	//			{
	//				cnt++;
	//			}
	//		}
	//	}
	//	break;
	//default:
	//	RtCoreLog().Info("Error: item invalid\n");
	//	break;
	//}

	//return cnt;
	//return 0;
//}

bool CSalesroom::Serialize(CG_CmdPacket *cmd)
{
	if(cmd->IsReading())
	{
		char size;

		m_DealingItemList.clear();
		m_DealSuccessItemList.clear();
		m_DealFailedItemList.clear();

		*cmd << size;

		if((int)size < 0)
		{
			return false;
		}

		for(int i = 0; i < (int)size; i++)
		{
			CAuctionItem tmp;

			tmp.Serialize(cmd);

			m_DealingItemList.push_back(tmp);
		}

		*cmd << size;

		if((int)size < 0)
		{
			return false;
		}

		for(int i = 0; i < (int)size; i++)
		{
			CAuctionItem tmp;

			tmp.Serialize(cmd);

			m_DealSuccessItemList.push_back(tmp);
		}

		*cmd << size;

		if((int)size < 0)
		{
			return false;
		}

		for(int i = 0; i < (int)size; i++)
		{
			CAuctionItem tmp;

			tmp.Serialize(cmd);

			m_DealFailedItemList.push_back(tmp);
		}

	}else
	{
		char size = (char)m_DealingItemList.size();

		*cmd << size;

		stlforeach(std::list<CAuctionItem>, it1, m_DealingItemList)
		{
			CAuctionItem* pItem = (CAuctionItem*)&(*it1);
			pItem->Serialize(cmd);
		}
		//for(int i = 0; i < (int)size; i++)
		//{
		//	m_DealingItemList[i].Serialize(cmd);
		//}

		size = (char)m_DealSuccessItemList.size();

		*cmd << size;

		stlforeach(std::list<CAuctionItem>, it2, m_DealSuccessItemList)
		{
			CAuctionItem* pItem = (CAuctionItem*)&(*it2);
			pItem->Serialize(cmd);
		}

		//for(int i = 0; i < (int)size; i++)
		//{
		//	m_DealSuccessItemList[i].Serialize(cmd);
		//}

		size = (char)m_DealFailedItemList.size();

		*cmd << size;

		stlforeach(std::list<CAuctionItem>, it3, m_DealFailedItemList)
		{
			CAuctionItem* pItem = (CAuctionItem*)&(*it3);
			pItem->Serialize(cmd);
		}

		//for(int i = 0; i < (int)size; i++)
		//{
		//	m_DealFailedItemList[i].Serialize(cmd);
		//}
	}

	if (cmd->IsError())
		return false;
	else 
		return true;
}

bool CSalesroom::SerializeDealing(CG_CmdPacket *cmd)
{
	if(cmd->IsReading())
	{
		char size;

		m_DealingItemList.clear();

		*cmd << size;

		for(unsigned char i = 0; i < (unsigned char)size; i++)
		{
			CAuctionItem tmp;

			if (!tmp.Serialize(cmd))
				break;

			m_DealingItemList.push_back(tmp);
		}
	}else
	{
		char size = (char)m_DealingItemList.size();

		*cmd << size;

		stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
		{
			CAuctionItem* pItem = (CAuctionItem*)&(*it);
			pItem->Serialize(cmd);
		}

		//for(int i = 0; i < (int)size; i++)
		//{
		//	m_DealingItemList[i].Serialize(cmd);
		//}
	}

	if (cmd->IsError())
		return false;
	else 
		return true;
}

bool CSalesroom::UpdateSuccessItem(CAuctionItem item, int who)
{
	stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
	{
		if(who == AUCTION_SALER)
		{
			if((*it).ItemID().id == item.ItemID().id)
			{
				(*it).Bargainor(0);
				(*it).BargainorChr("");
				(*it).BargainorChrID(0);
				return true;
			}
		}else
		{
			if((*it).ItemID().id == item.ItemID().id)
			{
				(*it).Purchaser(0);
				(*it).PurchaserChr("");
				(*it).PurchaserChrID(0);
				return true;
			}
		}
	}
    return false;
}

std::list<CAuctionItem>* CSalesroom::FindDealSuccessItemBySalerAccId(DWORD acc_id)
{
	static std::list<CAuctionItem>	sta_success_sale;

	sta_success_sale.clear();

	stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
	{
		if(((*it).Bargainor() == acc_id) && ((*it).Purchaser() != 0))
		{
			sta_success_sale.push_back(*it);
		}
	}

	return &sta_success_sale;
}

std::list<CAuctionItem>* CSalesroom::FindDealFailedItemBySalerAccId(DWORD acc_id)
{
	static std::list<CAuctionItem>	sta_failed_sale;

	sta_failed_sale.clear();

	stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
	{
		if(((*it).Bargainor() == acc_id) && ((*it).Purchaser() == 0))
		{
			sta_failed_sale.push_back(*it);
		}
	}

	return &sta_failed_sale;
}

std::list<CAuctionItem>* CSalesroom::FindDealFailedItemByBuyerAccId(DWORD acc_id)
{
	static std::list<CAuctionItem>	sta_failed_buy;

	sta_failed_buy.clear();

	stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
	{
		if((*it).Purchaser() == acc_id)
		{
			sta_failed_buy.push_back(*it);
		}
	}

	return &sta_failed_buy;
}

std::list<CAuctionItem>* CSalesroom::FindDealSuccessItemByBuyerAccId(DWORD acc_id)
{
	static std::list<CAuctionItem>	sta_success_buy;

	sta_success_buy.clear();

	stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
	{
		if((*it).Purchaser() == acc_id)
		{
			sta_success_buy.push_back(*it);
		}
	}

	return &sta_success_buy;
}

CAuctionItem* CSalesroom::FindDealingItemById(SItemID item_id)
{
	stlforeach(std::list<CAuctionItem>, it, m_DealingItemList)
	{
		if((*it).ItemID().id == item_id.id)
		{			
			return &(*it);
		}
	}

	return NULL;
}

CAuctionItem* CSalesroom::FindDealSucessItemById(SItemID item_id)
{
	stlforeach(std::list<CAuctionItem>, it, m_DealSuccessItemList)
	{
		if((*it).ItemID().id == item_id.id)
		{
			return &(*it);
		}
	}

	return NULL;
}

CAuctionItem* CSalesroom::FindDealFailedItemById(SItemID item_id)
{
	stlforeach(std::list<CAuctionItem>, it, m_DealFailedItemList)
	{
		if((*it).ItemID().id == item_id.id)
		{
			return &(*it);
		}
	}

	return NULL;
}

void CSalesroom::Clear()
{
	m_DealingItemList.clear();
	m_DealSuccessItemList.clear();
	m_DealFailedItemList.clear();
}

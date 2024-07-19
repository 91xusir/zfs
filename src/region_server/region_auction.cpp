
#include "region_auction.h"

CSalesroom	g_SalesroomBackUp;

bool RemoveCharge(CAuctionItem* item)
{
	if(!item) return false;

	CRegionUser* p_user = g_region->FindUser((char*)item->PurchaserChr().c_str());

	if((p_user == NULL) || p_user->m_dummy == NULL)
	{
		return false;
	}
	CRegionCreature* p_cre = p_user->m_dummy;

	if(!p_cre->RemoveGold(item->BidPrice()))
	{
		return false;
	}

	return true;
}

bool RemoveStorageCharge(CAuctionItem* item)
{
	if(!item) return false;

	CRegionUser* p_user = g_region->FindUser((char*)item->BargainorChr().c_str());

	if((p_user == NULL) || p_user->m_dummy == NULL)
	{
		return false;
	}
	CRegionCreature* p_cre = p_user->m_dummy;

	if(!p_cre->RemoveGold(item->Duration() * AUCTION_CHARGE_BASE))
	{
		return false;
	}

	return true;
}

bool RemoveItemFromBag(CAuctionItem* item)
{
	if(!item) return false;

	CRegionUser* p_user = g_region->FindUser((char*)item->BargainorChr().c_str());

	if((p_user == NULL) || p_user->m_dummy == NULL)
	{
		return false;
	}
	CRegionCreature* p_cre = p_user->m_dummy;
	
	int page, gridI, gridJ;
	if (!p_cre->m_pItemContainer->m_Bag.Find(item->ItemID().id, &page, &gridI, &gridJ))
	{
		//...........
		return false;
	}
	if(!p_cre->m_pItemContainer->RemoveItem(item->ItemID()))
	{
		//.............
		return false;
	}
	CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_NULL);
	p_cre->RecvCmd(g_sendCmd);

	return true;
}


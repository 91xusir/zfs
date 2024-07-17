#include "region.h"


bool CRegionGiftMgr::WithdrawGift(SGift& gift, char& errorCode)
{
	errorCode = GIFT_ERROR_UNKNOWN;

	CRegionUser *pUser = g_region->FindUser(gift.userID);
	if (!pUser || !pUser->m_dummy)
		return false;

	switch (gift.type)
	{
	case GIFT_TYPE_ITEMS:
		{
			vector<SCreateItem> arrCreateItems;
			if (!g_region->m_pItemManager->AnalyseCreateString(gift.attribs, arrCreateItems))
			{
				ERR1("[CRegionGiftMgr::WithdrawGift] AnalyseCreateString error!(gift.index=%d)\n", gift.index);
				errorCode = GIFT_ERROR_UNKNOWN;
				return false;
			}
			int count = (int)arrCreateItems.size();
			if (pUser->m_dummy->m_pItemContainer->m_Bag.FreeSpace() < count)
			{
				errorCode = GIFT_BAG_NOT_ENOUGH;
				return false;
			}

			for (int i=0; i<count; i++)
			{
				SItemID item;
				item = g_region->m_pItemManager->CreateItemByStruct(arrCreateItems[i]);
				if (ItemID_IsValid(item))
				{
					if (ItemIsOther_Hero(item.type))
					{
						item.params[ITEMHERO_PARAM_LIFE] = gift.limitTime;
						item.paramCount = ITEMHERO_PARAM_SIZE;
					}
                    if (arrCreateItems[i].index == 1110)
                    {
                        DWORD now = time(NULL);
                        DWORD tLife = now + 60*24*3600;
                        item.paramCount = ITEMSHIPIN_PARAM_SIZE;
                        item.params[ITEMSHIPIN_PARAM_LIFE] = tLife;
                    }
                    if (arrCreateItems[i].index == 1112)
                    {
                        DWORD now = time(NULL);
                        DWORD tLife = now + 0.25*3600; // 2*3600
                        item.paramCount = ITEMSHIPIN_PARAM_SIZE;
                        item.params[ITEMSHIPIN_PARAM_LIFE] = tLife;
                    }

					if (!pUser->m_dummy->AddItem(item))
					{
						ERR2("[CRegionGiftMgr::WithdrawGift] Can NOT add item to player(player=%s,itemtype=0x%x)\n", pUser->m_dummy->m_core.Name.c_str(), item.type);
					}
				}
				else 
				{
					ERR3("[CRegionGiftMgr::WithdrawGift] Can NOT create item (id=%s,color=%d,level=%d)\n", arrCreateItems[i].index, arrCreateItems[i].color, arrCreateItems[i].level);
				}
			}
		}
		break;
	case GIFT_TYPE_MONEY:
		{
			int money = atoi(gift.attribs.c_str());
			pUser->m_dummy->AddGold(money);
		}
		break;
	case GIFT_TYPE_EXP:
		{
			int exp = atoi(gift.attribs.c_str());
			pUser->m_dummy->AddExp(exp, 0);
		}
		break;
    case GIFT_TYPE_EXPLOIT:
        {
            int exploit = atoi(gift.attribs.c_str());
            pUser->m_dummy->AddExploit(exploit);
        }
        break;
	default:
		ERR3("[CRegionGiftMgr::WithdrawGift]Invalid gift type(index=%d, type=%d, userID=%d)\n", gift.index, gift.type, gift.userID);
		break;
	}

	errorCode = GIFT_SUCCESS;
	return true;
}

bool CRegionGiftMgr::Build_r2c_get_gift_response(char errorCode, const char* giftMsg)
{
	CG_CmdPacket *pPacket = g_sendCmd;
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_get_gift_response);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteString((char*)giftMsg);
	return true;
}

bool CRegionGiftMgr::NetSend_r2g_query_gifts(DWORD userID, int level)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_query_gifts);
	packet.WriteLong(userID);
	packet.WriteLong(level);
	return g_region->m_gws.EndSend();
}

bool CRegionGiftMgr::NetSend_r2g_withdraw_gift_success(DWORD userID, SGift& gift)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_withdraw_gift_success);
	packet.WriteLong(userID);
	gift.Serialize(&packet);
	return g_region->m_gws.EndSend();
}

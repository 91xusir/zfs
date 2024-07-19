#include "region_deduct.h"
CDeduct* CDeduct::m_deductInstance = 0;
CDeduct::CDeduct()
{
	m_iDeductId = 0;
}
CDeduct::~CDeduct()
{
	m_iDeductId = 0;
}

void CDeduct::AddDeductData(int deductid,DeductType type,void* v)
{
	SDeductData* newData = RT_NEW SDeductData;
	newData->deductid = deductid;
	newData->type	  = type;
	newData->vdata	  = v;
	//newData->vItemID  = point_arrItems;
	CDeduct::GetDeductInstance()->m_listDeductData.push_back(newData);
}

void CDeduct::DeleteData(int deductid)
{
	std::list<SDeductData*>::iterator iter = m_listDeductData.begin();
	for (; iter != m_listDeductData.end();)
	{
		if((*iter)->deductid == deductid)
		{
			DEL_ONE(*iter);
			iter = m_listDeductData.erase(iter);
			return;
		}
		else
			++iter;
	}
}
SDeductData* CDeduct::GetDeductDataById(int deductId)
{
	std::list<SDeductData*>::iterator iter = m_listDeductData.begin();
	for (; iter != m_listDeductData.end(); ++iter)
	{
		if((*iter)->deductid == deductId)
			return *iter;
	}
	return NULL;
}

// void CDeduct::ChangeDeductId(int i)
// {
//		m_iDeductId = m_iDeductId + i;
// }

int CDeduct::GetDeductId()
{
	return m_iDeductId++;
}

bool CDeduct::UseDeductData(long userid,int deductid,char* cResult,short sPoint,short sCurPoint)
{
	CRegionUser* pUser = g_region->FindUser(userid);
	bool bRet = false;
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
		SDeductData* deductdata = GetDeductDataById(deductid);
		if (deductdata == 0) return false;
		// 调用对应函数
		else
		{
			if (deductdata->type == DEDUCT_TYPE_BUYITEM)
			{
				vector<SItemID>* itemid = static_cast<vector<SItemID>*>(deductdata->vdata);
				if(strcmp(cResult,"") != 0)
				{
					pUser->m_dummy->SendSystemMessage(cResult);
					bRet = false;
				}
				else
				{
					if(!pUser->m_dummy->m_pItemContainer->AddItem(*itemid)) 
					{
						ERR("从NPC处买点数道具时出错：[AddItem]");
						CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
						bRet = false;
					}
					else
					{
						char message[100];
						rt2_sprintf(message,"您使用了%d的点数,剩余%d点。",sPoint,sCurPoint);
						pUser->m_dummy->SendSystemMessage(message);
						string strItem;
						strItem += rtFormatNumber(GetItemSerializeVersion());
						strItem += ",";
						strItem += rtFormatNumber((int)(*itemid).size());
						strItem += ",";
						for (int i=0; i<(int)(*itemid).size(); ++i)
						{
							strItem += (*itemid)[i].SaveToString(g_region->m_pItemManager);
						}
						CG_CmdPacket& packet = g_region->m_gws.BeginSend();
						packet.WriteShort(r2g_log_event);
						packet.WriteByte(PLT_PointCast);
						packet.WriteLong(userid);
						packet.WriteString((char*)pUser->m_userIpAddress.c_str()); // userIP
						packet.WriteLong((long)sPoint);
						packet.WriteString((char*)strItem.c_str());
						packet.WriteLong((long)sCurPoint);
						packet.WriteString("购买点数道具");
						g_region->m_gws.EndSend();

						CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(pUser->m_dummy->m_pItemContainer->m_Bag));
						pUser->m_dummy->RecvCmd(g_sendCmd);
						CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(1);
						pUser->m_dummy->RecvCmd(g_sendCmd);
						CItemCmdBuilder_Svr::Build_r2c_point_result(-sPoint);	//heten
						pUser->m_dummy->RecvCmd(g_sendCmd);	//heten
						bRet = true;
					}
				}
				DEL_ONE(itemid);
			}
			else if(deductdata->type == DEDUCT_TYPE_WORLD_MESSAGE)
			{
				//发送世界喊话
				bRet = true;
			}
			DeleteData(deductid);
		}
	}
	return bRet;
}


#include "gc_include.h"
#include "ui_form_shopcenter.h"

UIFormShopCenter::UIFormShopCenter()
{
	m_pFrmThis  = RTW_WIDGET_T(RtwForm,"fmshoppingmail");
	m_pFrmThis->SetMovable(true);

	m_pTabPage = RTW_WIDGET_T(RtwTab,"fmshoppingmail.tabitem");
	m_pTabPage->EvChangeSelected += RTW_CALLBACK(this, UIFormShopCenter, ChangeSelectTab);

	//推荐商品
	char tmp[256];
	for (size_t i = 1; i <= MAX_PAGE_COUNT; i++)
	{
		rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d", i);
		m_pFormItemType[i] = RTW_WIDGET_T(RtwForm, tmp);
		rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.btfirstpage", i);
		m_pFirstBtn[i] = RTW_WIDGET_T(RtwButton, tmp);
		m_pFirstBtn[i]->EvLClick += RTW_CALLBACK(this, UIFormShopCenter, ShowFirstPage);
		rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.btuppage", i);
		m_pUpBtn[i] = RTW_WIDGET_T(RtwButton, tmp);
		rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.btdownpage", i);
		m_pDownBtn[i] = RTW_WIDGET_T(RtwButton, tmp);
		rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.btendpage", i);
		m_pEndBtn[i] = RTW_WIDGET_T(RtwButton, tmp);
		for (size_t j = 1; j <= MAX_ITEM_COUNT; j++)
		{
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d", i, j);
			m_pFormItem[i][j] = RTW_WIDGET_T(RtwForm, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.btbuyitem", i, j);
			m_pBtnBuyItem[i][j] = RTW_WIDGET_T(RtwButton, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.btgiveitem", i, j);
			m_pBtnPresentItem[i][j] = RTW_WIDGET_T(RtwButton, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.lblitemicon", i, j);
			m_pItemIcon[i][j] = RTW_WIDGET_T(RtwLabel, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.lblitemname", i, j);
			m_pLableItemName[i][j] = RTW_WIDGET_T(RtwLabel, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.lblprice", i, j);
			m_pLableItemPrice[i][j] = RTW_WIDGET_T(RtwLabel, tmp);
			rt2_snprintf(tmp, 256, "fmshoppingmail.tabitem.fmtab%d.fmitem.fmiteminfo%d.lbloffsale", i, j);
			m_pLableItemDownPrice[i][j] = RTW_WIDGET_T(RtwLabel, tmp);
		}
	}

	m_ckBtnAll = RTW_WIDGET_T(CUiCheckButton, "fmshoppingmail.tabitem.fmtab1.bttype1");
	m_ckBtnAll->EvLClick += RTW_CALLBACK(this, UIFormShopCenter, ShowCheckCommendItemAll);
	m_ckBtnHot = RTW_WIDGET_T(CUiCheckButton, "fmshoppingmail.tabitem.fmtab1.bttype2");
	m_ckBtnHot->EvLClick += RTW_CALLBACK(this, UIFormShopCenter, ShowCheckCommendItemHot);
	m_ckBtnNew = RTW_WIDGET_T(CUiCheckButton, "fmshoppingmail.tabitem.fmtab1.bttype3");
	m_ckBtnNew->EvLClick += RTW_CALLBACK(this, UIFormShopCenter, ShowCheckCommendItemNew);
	m_ckBtnPresent = RTW_WIDGET_T(CUiCheckButton, "fmshoppingmail.tabitem.fmtab1.bttype4");
	m_ckBtnPresent->EvLClick += RTW_CALLBACK(this, UIFormShopCenter, ShowCheckCommendItemPresent);

	m_shopActorView = RTW_WIDGET_T(Rtw3DView, "fmshoppingmail.fmitemact.lbitemact");

	for (size_t i = 0; i < MAX_PAGE_COUNT; i++)
	{
		m_curSelectPage[i] = 0;
	}
	for (size_t i = 0; i < 4; i++)
	{
		m_curCommendPage[i] = 1;
	}

	//初始化商城数据
	InitShopCenter();
}

UIFormShopCenter::~UIFormShopCenter()
{

}

void UIFormShopCenter::ChangeSelectTab(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	int i =pEvent->type;
	switch(i)
	{
	case 0:
		ShowCommendItemInfo(vecCommendItemAll, 1, 1);
		break;
	case 1:
		ShowCommendItemInfo(vecFuctionItemAll, 2, 1);
		break;
	case 2:
		ShowCommendItemInfo(vecSupplyItemAll, 3, 1);
		break;
	case 3:
		ShowCommendItemInfo(vecMaterialItemAll, 4, 1);
		break;
	case 4:
		ShowCommendItemInfo(vecPetItemAll, 5, 1);
		break;
	case 5:
		ShowCommendItemInfo(vecFashionItemAll, 6, 1);
		break;
	default:
		break;
	}
}

void UIFormShopCenter::ShowFirstPage(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	const string& WidgetID = pSender->getName();
	int i;
	Is_Widget_Page(WidgetID, i);

	if (i == 1)
	{
		if (m_ckBtnAll->GetChecked())
		{
			ShowCommendItemInfo(vecCommendItemAll, 1, 1);
		} 
		else if(m_ckBtnHot->GetChecked())
		{
			ShowCommendItemInfo(vecCommendItemHot, 1, 1);
		}
		else if (m_ckBtnNew->GetChecked())
		{
			ShowCommendItemInfo(vecCommendItemNew, 1, 1);
		} 
		else if(m_ckBtnPresent->GetChecked())
		{
			ShowCommendItemInfo(vecCommendItemPresent, 1, 1);
		}
	} 
	else if(i == 2)
	{
	}
}

void UIFormShopCenter::ShowUpPage(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	const string& WidgetID = pSender->getName();
	int i;
	Is_Widget_Page(WidgetID, i);

	if (i == 1)
	{
		if (m_ckBtnAll->GetChecked())
		{
			int iPage = vecCommendItemAll.size()/8 + 1;
			iPage--;
			m_curCommendPage[1] = iPage;
			ShowCommendItemInfo(vecCommendItemAll, 1, m_curCommendPage[1]);
		} 
		else if(m_ckBtnHot->GetChecked())
		{
			int iPage = vecCommendItemHot.size()/8 + 1;
			iPage--;
			m_curCommendPage[2] = iPage;
			ShowCommendItemInfo(vecCommendItemHot, 1, m_curCommendPage[2]);
		}
		else if (m_ckBtnNew->GetChecked())
		{
			int iPage = vecCommendItemNew.size()/8 + 1;
			iPage--;
			m_curCommendPage[3] = iPage;
			ShowCommendItemInfo(vecCommendItemNew, 1, m_curCommendPage[3]);
		} 
		else if(m_ckBtnPresent->GetChecked())
		{
			int iPage = vecCommendItemAll.size()/8 + 1;
			iPage--;
			m_curCommendPage[4] = iPage;
			ShowCommendItemInfo(vecCommendItemPresent, 1, m_curCommendPage[4]);
		}
	} 
	else if(i == 2)
	{
	}
}

void UIFormShopCenter::ShowDownPage(RtwWidget* pSender, RtwEventDelegate* pEvent)
{

}

void UIFormShopCenter::ShowEndPage(RtwWidget* pSender, RtwEventDelegate* pEvent)
{

}

bool UIFormShopCenter::Is_Widget_Page(const string& id, int &outI)
{
	if (strncmp(id.c_str(), "layworld.fmshoppingmail.tabitem.fmtab", 36) == 0)
	{
		string strI = string(&id[37], 1);
		outI = atoi(strI.c_str());
		return true;
	}
	else 
	{
		return false;
	}
}

bool UIFormShopCenter::Is_Widget_Page_Button(const string& id, int &outI, int &outJ)
{
	if (strncmp(id.c_str(), "CommItem", 8) == 0)
	{
		string strI = string(&id[9], 2);
		string strJ = string(&id[12], 2);
		outI = atoi(strI.c_str());
		outJ = atoi(strJ.c_str());
		return true;
	}
	else 
	{
		return false;
	}
}

void UIFormShopCenter::InitShopCenter()
{
	vecCommendItemAll.clear();
	vecCommendItemHot.clear();
	vecCommendItemNew.clear();
	vecCommendItemPresent.clear();
	vecFuctionItemAll.clear();
	vecSupplyItemAll.clear();
	vecMaterialItemAll.clear();
	vecPetItemAll.clear();
	vecFashionItemAll.clear();

	std::vector<SShopCenter> m_ShopCenterTable = g_TableShopCenter.GetShopCenterV();
	for (long i = 0; i < m_ShopCenterTable.size(); i++)
	{
		if (m_ShopCenterTable[i].bHotSale)
		{
			vecCommendItemAll.push_back(m_ShopCenterTable[i].id);

			if (m_ShopCenterTable[i].commendType == 1)
			{
				vecCommendItemHot.push_back(m_ShopCenterTable[i].id);
			} 
			else if(m_ShopCenterTable[i].commendType == 2)
			{
				vecCommendItemNew.push_back(m_ShopCenterTable[i].id);
			}
			else if (m_ShopCenterTable[i].commendType == 3)
			{
				vecCommendItemPresent.push_back(m_ShopCenterTable[i].id);
			}
		}

		if (m_ShopCenterTable[i].type == 1)
		{
			vecFuctionItemAll.push_back(m_ShopCenterTable[i].id);
		} 
		else if(m_ShopCenterTable[i].type == 2)
		{
			vecSupplyItemAll.push_back(m_ShopCenterTable[i].id);
		}
		else if (m_ShopCenterTable[i].type == 3)
		{
			vecMaterialItemAll.push_back(m_ShopCenterTable[i].id);
		}
		else if (m_ShopCenterTable[i].type == 4)
		{
			vecPetItemAll.push_back(m_ShopCenterTable[i].id);
		}
		else if (m_ShopCenterTable[i].type == 5)
		{
			vecFashionItemAll.push_back(m_ShopCenterTable[i].id);
		}
	}

	for (size_t i = 1; i <= MAX_PAGE_COUNT; i++)
	{
		m_pFirstBtn[i]->Disable();
		m_pUpBtn[i]->Disable();
		m_pDownBtn[i]->Disable();
		m_pEndBtn[i]->Disable();
	}
	//默认显示第一页
	ShowCommendItemInfo(vecCommendItemAll, 1, 1);

}

void UIFormShopCenter::Update()
{

}

bool UIFormShopCenter::IsVisible()
{
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
}

void UIFormShopCenter::Show()
{
	//GcActorGraph *pActorGraph = GetPlayer()->GetGraph();
	//if (pActorGraph)
	//{
	//	m_shopActorView->AddRenderActor(pActorGraph->p());
	//}

	m_pFrmThis->Show();
}

void UIFormShopCenter::Hide()
{
	if (IsVisible())
	{
		m_pFrmThis->Hide();
	}
}

void UIFormShopCenter::ShowCommendItemInfo(std::vector<long> vecItem, int iCheckBtn, int iPage)
{
	if (vecItem.size() > iPage*8)
	{
		for (size_t i = (iPage-1)*8+1; i <= (iPage-1)*8+8; i++)
		{
			SShopCenter* shopCenter = g_TableShopCenter.FindShopCenter(vecItem[i-1]);
			if(!shopCenter) continue;

			SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(shopCenter->itemId);
			if(pItemClass)
			{
				std::string itemIcon = pItemClass->icon;
				itemIcon += "_b";
				RtwImage* pImage = g_workspace.getImageFactory()->createImage(itemIcon);
				if(pImage)
				{
					pImage->SetBlend(true);
					m_pItemIcon[iCheckBtn][i]->SetBackgroundImage(pImage);
				}
				else
				{
					RtCoreLog().Info("找不到文件：%s", itemIcon.c_str());
				}
			}

			m_pLableItemName[iCheckBtn][i]->SetText(shopCenter->itemName);
			m_pLableItemName[iCheckBtn][i]->Show();

			char tmp[256];
			rt2_snprintf(tmp, 256, "售价:%d元宝", shopCenter->goldPrice);
			m_pLableItemPrice[iCheckBtn][i]->SetText(tmp);
			m_pLableItemPrice[iCheckBtn][i]->Show();

			if (shopCenter->fSubPriceRate > 0.f)
			{
				rt2_snprintf(tmp, 256, "折后价:%d元宝", shopCenter->goldPrice * shopCenter->fSubPriceRate);
				m_pLableItemDownPrice[iCheckBtn][i]->SetText(tmp);
				m_pLableItemDownPrice[iCheckBtn][i]->Show();
			}
			else
			{
				m_pLableItemDownPrice[iCheckBtn][i]->Hide();
			}
		}
	} 
	else
	{
		for (size_t i = (iPage-1)*8+1; i <= vecItem.size()-(iPage-1)*8; i++)
		{
			SShopCenter* shopCenter = g_TableShopCenter.FindShopCenter(vecItem[i-1]);
			if(!shopCenter) continue;

			SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(shopCenter->itemId);
			if(pItemClass)
			{
				std::string itemIcon = pItemClass->icon;
				itemIcon += "_b";
				RtwImage* pImage = g_workspace.getImageFactory()->createImage(itemIcon);
				if(pImage)
				{
					pImage->SetBlend(true);
					m_pItemIcon[iCheckBtn][i]->SetBackgroundImage(pImage);
				}
			}

			m_pLableItemName[iCheckBtn][i]->SetText(shopCenter->itemName);
			m_pLableItemName[iCheckBtn][i]->Show();

			char tmp[256];
			rt2_snprintf(tmp, 256, "售价:%d元宝", shopCenter->goldPrice);
			m_pLableItemPrice[iCheckBtn][i]->SetText(tmp);
			m_pLableItemPrice[iCheckBtn][i]->Show();

			if (shopCenter->fSubPriceRate > 0.f)
			{
				rt2_snprintf(tmp, 256, "折后价:%d元宝", shopCenter->goldPrice * shopCenter->fSubPriceRate);
				m_pLableItemDownPrice[iCheckBtn][i]->SetText(tmp);
				m_pLableItemDownPrice[iCheckBtn][i]->Show();
			}
			else
			{
				m_pLableItemDownPrice[iCheckBtn][i]->Hide();
			}
		}
		for (size_t i = vecItem.size()+1; i <= iPage*8; i++)
		{
			m_pItemIcon[iCheckBtn][i]->SetBackgroundImage(NULL);
			m_pLableItemName[iCheckBtn][i]->Hide();
			m_pLableItemPrice[iCheckBtn][i]->Hide();
			m_pLableItemDownPrice[iCheckBtn][i]->Hide();
		}
	}
}

void UIFormShopCenter::ShowCheckCommendItemAll(void*, void*)
{
	ShowCommendItemInfo(vecCommendItemAll, 1, 1);
}

void UIFormShopCenter::ShowCheckCommendItemHot(void*, void*)
{
	ShowCommendItemInfo(vecCommendItemHot, 1, 1);
}

void UIFormShopCenter::ShowCheckCommendItemNew(void*, void*)
{
	ShowCommendItemInfo(vecCommendItemNew, 1, 1);
}

void UIFormShopCenter::ShowCheckCommendItemPresent(void*, void*)
{
	ShowCommendItemInfo(vecCommendItemPresent, 1, 1);
}

void UIFormShopCenter::RenderFactionActor()
{
	//m_pCamActor = RT_NEW RtgCamera();
	//m_pCamActor->SetProjParams(DegreeToRadian(60.0f), 4.f/3.f, 10.0f, 1000.0f);
	//m_pCamActor->SetViewParams(RtgVertex3(100.0f, 10.0f, 10.0f), RtgVertex3(0.0f, 0.0f, 0.0f), RtgVertex3(0.0f, 0.0f, 1.0f));

	//RtwRect rc = RtwRect(0, 0, 100, 100);
	//g_workspace.getRenderDevice()->BeginRenderViewPort(&m_pCamActor,rc.left,rc.top,rc.right,rc.bottom,0.f,1.f);

	//GcActorGraph *pActorGraph = GetWorld()->m_pPlayer->GetGraph();
	//if (pActorGraph)
	//{
	//	g_workspace.getRenderDevice()->RenderActorInstance(pActorGraph,RTGRM_NOBLENDOBJ_ONLY);
	//	g_workspace.getRenderDevice()->RenderActorInstance(pActorGraph,RTGRM_BLENDOBJ_ONLY);
	//}

	//g_workspace.getRenderDevice()->EndRenderViewPort();
}
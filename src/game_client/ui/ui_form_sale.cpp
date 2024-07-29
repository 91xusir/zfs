
#include "gc_include.h"
#include "ui_form_sale.h"
#include "ui_form_msg.h"
#include "ui_form_vendor.h"
#include "ui_form_playeritems.h"
#include "game_assistant.h"
#include "net_command_auction.h"

UIFormSale::UIFormSale()
{
	m_pFrmThis  = LOAD_UI("fmAuction");
	m_pFrmThis->ModifyFlags(wfDragClient, 0);
	m_pBtnItem	= LOAD_UI_T(RtwButton, "fmAuction.panOrder.pansource01.btnskill");
	m_pLabelName	= LOAD_UI_T(RtwLabel, "fmAuction.panOrder.pansource01.lbskillinfo");
	m_pBasePrice	= LOAD_UI_T(RtwTextBox, "fmAuction.panOrder.lbPricenum");
	m_pUniquePrice	= LOAD_UI_T(RtwTextBox, "fmAuction.panOrder.lbPricenum1");
	m_pTimeLimit	= LOAD_UI_T(RtwComboBox, "fmAuction.panOrder.cbtime");
	m_pHandlingCharge	= LOAD_UI_T(RtwLabel, "fmAuction.panOrder.lbPricenum2");
	m_pBtnSell			= LOAD_UI_T(RtwButton, "fmAuction.panOrder.btnBuy");
	m_pList				= LOAD_UI_T(RtwListBox, "fmAuction.fmAuctionRight");
	m_pBtnBack			= LOAD_UI_T(RtwButton, "fmAuction.btnBack");
	m_pBtnRefresh		= LOAD_UI_T(RtwButton, "fmAuction.btnRefresh");

	//m_pList->ModifyFlags(wfHint, 0);//rem by fox
//	m_pList->SetColumnFormat(RtwStringFormat(alignCenter,alignCenter));//add by fox// 注释 [3/17/2009 tooth.shi]


	m_pBtnItem->ModifyFlags(wfDragClient | wfDragServer, 0);
	m_pBtnItem->EvDrag += RTW_CALLBACK(this, UIFormSale, OnDragItem);
	m_pBtnItem->EvDragEnd += RTW_CALLBACK(this, UIFormSale, OnDragEndItem);
	
	m_pBtnBack->EvLClick += RTW_CALLBACK(this, UIFormSale, OnBtnBackClicked);
	m_pBtnSell->EvLClick += RTW_CALLBACK(this, UIFormSale, OnBtnSellClicked);
	m_pBtnRefresh->EvLClick	+= RTW_CALLBACK(this, UIFormSale, OnBtnRefreshClicked);;
		
	
	m_pBtnItem->EvHint	 += RTW_CALLBACK(this, UIFormSale, OnGetBtnHintText);

	m_pTimeLimit->getDropList()->EvSelect += RTW_CALLBACK(this, UIFormSale, OnSelectTime);
	//m_pList->EvSelect	   += RTW_CALLBACK(this, UIFormSale, OnListItemSelect);//rem by fox
	//m_pList->EvMouseMove	+= RTW_CALLBACK(this, UIFormSale, OnMouseMove);
	m_pList->EvMouseWheel	+= RTW_CALLBACK(this, UIFormSale, OnMouseMove);//add by fox
	m_pList->EvSelect	    += RTW_CALLBACK(this, UIFormSale, OnMouseMove);//add by fox
	m_pFrmThis->EvMouseMove += RTW_CALLBACK(this, UIFormSale, OnMouseMove);
	//m_pList->EvHint		 += RTW_CALLBACK(this, UIFormSale, OnGetListHintItemText);
	m_pFrmThis->EvHint		 += RTW_CALLBACK(this, UIFormSale, OnGetListHintItemText);
	m_pList->ModifyFlags(0,wfMouseMove);
	m_itemID.Clear();
	m_pLabelName->SetText("");
	m_pHandlingCharge->SetText("");
	m_pBasePrice->SetText("");
	m_pUniquePrice->SetText("");

	m_pBasePrice->SetNumberOnly(true);
	m_pUniquePrice->SetNumberOnly(true);

	m_index = -1;

	Hide();

}

void UIFormSale::Hide()
{
	m_itemID.Clear();
	
	m_pFrmThis->Hide();
}

void UIFormSale::OnBtnRefreshClicked(RtwWidget* sender, void*)
{
	CAuctionNetHelper::NetSend_c2r_auction_update();
}

void UIFormSale::OnListItemSelect(RtwWidget* sender, void*)
{
	//m_pList->ModifyFlags(wfHint, 0);//rem by fox
}

void UIFormSale::OnSelectTime(RtwWidget* sender, void*)
{
	if(m_pTimeLimit->GetSelectedItem() == 0)
	{
		m_pHandlingCharge->SetText(rtFormatNumber(2 * 1000));
	}else if(m_pTimeLimit->GetSelectedItem() == 1)
	{
		m_pHandlingCharge->SetText(rtFormatNumber(4 * 1000));
	}else if(m_pTimeLimit->GetSelectedItem() == 2)
	{
		m_pHandlingCharge->SetText(rtFormatNumber(8 * 1000));
	}else if(m_pTimeLimit->GetSelectedItem() == 3)
	{
		m_pHandlingCharge->SetText(rtFormatNumber(12 * 1000));
	}else if(m_pTimeLimit->GetSelectedItem() == 4)
	{
		m_pHandlingCharge->SetText(rtFormatNumber(24 * 1000));
	}
}

void UIFormSale::OnDragItem(RtwWidget* sender, void*)
{
	//RtwWidget* pDragSvr = g_workspace.GetDragServer();
	//if (!pDragSvr) return;
	//const string& WidgetID = pDragSvr->GetID();

	//// 是不是道具栏的格子
	//int i,j;
	//if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	//{
	//	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	//	SItemID item = pCon->m_Bag.GetItem(i, j);
	//	if (ItemID_IsValid(item))
	//	{
	//		if(ItemCanUpdate(item.type) && !ItemIsWeapon_MagicWeapon(item.type))
	//		{
	//			m_pBtnItem->ModifyFlags(wfHint, 0);
	//			m_UpdateItemID = item;
	//			m_pageItem = pCon->m_Bag.GetPageIndex();
	//			m_iItem = i;
	//			m_jItem = j;

	//			g_workspace.ResetHintText();
	//			Refresh();
	//		}
	//		else 
	//		{
	//			PlayerFailOperate(R(MSG_ITEM_CAN_NOT_ITEMUPDATE));
	//		}
	//	}	
	//}
}

void UIFormSale::OnDragEndItem(RtwWidget* sender, void*)
{
	m_pBtnItem->ModifyFlags(wfHint, 0);
	
	RtwWidget* pClient = g_workspace.GetDragClient();

	CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
	const string& widgetID = sender->GetID();
	int i, j;
	g_layerMain->m_fromPlayerItems->Is_Widget_Common(widgetID, i, j);

	if(pClient)
	{
		const RtwRect rcClient = pClient->GetFrameRect();
		const RtwRect rcAuction = m_pBtnItem->GetFrameRect();
		if ( rcAuction.IsContained(rcClient.left, rcClient.top) )
		{
			SItemID itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(i, j);
			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			if(!pItemBase)	return;

			SetItemID(itemID);
			Refresh();
		}
	}
}

void UIFormSale::Show()
{
	guard;
	if (m_pFrmThis)
	{
		if (!IsVisible())
		{
			OnPreDraw();
			m_pFrmThis->Show();
		}
	}
	unguard;
}

bool UIFormSale::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormSale::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}

	unguard;
}

void UIFormSale::OnBtnBackClicked(RtwWidget* sender, void*)
{
	Hide();

	g_layerMain->m_formVendor->Show();
	g_layerMain->m_formVendor->Refresh();
}

void UIFormSale::OnBtnSellClicked(RtwWidget* sender, void*)
{
	//int idx = m_pList->GetSelectedItem();

	//if(idx == -1) return;

	//CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(idx);

	//if(!pItem) return;

	DWORD duration = 0;
	if(m_pTimeLimit->GetSelectedItem() == 0)
	{
		duration = 2;
	}else if(m_pTimeLimit->GetSelectedItem() == 1)
	{
		duration = 4;
	}else if(m_pTimeLimit->GetSelectedItem() == 2)
	{
		duration = 8;
	}else if(m_pTimeLimit->GetSelectedItem() == 3)
	{
		duration = 12;
	}else if(m_pTimeLimit->GetSelectedItem() == 4)
	{
		duration = 24;
	}else
	{
		//UIFormMsg::ShowStatic("请选择时间", UIFormMsg::TYPE_OK);//by fox for string
		UIFormMsg::ShowStatic(R(MSG_BID_PLEASE_CHOICE_TIME), UIFormMsg::TYPE_OK);
		return;
	}

	long base_price = atol((char*)m_pBasePrice->GetText().c_str());
	long unique_price = atol((char*)m_pUniquePrice->GetText().c_str());

	if(base_price > unique_price)
	{
//		UIFormMsg::ShowStatic("一口价必须比底价更高", UIFormMsg::TYPE_OK);//by fox for string
		UIFormMsg::ShowStatic(R(MSG_BID_PRICE_MUST_HIGHER), UIFormMsg::TYPE_OK);
		
		return;
	}
	
	CAuctionNetHelper::NetSend_c2r_auction_sale(m_itemID, 
												base_price,
												unique_price,
												duration);

	m_itemID.Clear();
	m_pBtnItem->ModifyFlags(wfHint ,0);

}

void UIFormSale::ClearView()
{
	m_itemID.Clear();
	//Refresh();
}

void UIFormSale::OnGetListHintItemText(RtwWidget* sender, void*)
{
	/*//rem by fox
	int idx = m_index;

	if(idx == -1)
	{
		//m_pList->ModifyFlags(0, wfHint);//rem by fox
		return;
	}
	*/

	CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(m_index);

	if(!pItem) return;

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, pItem->ItemID(), GetWorld()->m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
}


void UIFormSale::OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent)
{
	
	RtwRect rect=sender->GetClientRect();
	m_index = m_pList->IndexFromPoint(pEvent->mouse.x, pEvent->mouse.y-rect.top-50,false);//modify by fox

	
	if(m_index==-1)
	{
		m_pFrmThis->ModifyFlags(0,wfHint);
		g_workspace.HideHint(0);
	}
	else
	{
		m_pFrmThis->ModifyFlags(wfHint,0);
		g_workspace.ShowHint(m_pFrmThis,0);
	}

	
}

void UIFormSale::OnGetBtnHintText(RtwWidget* sender, void*)
{
	guard;

	if(ItemID_IsValid(m_itemID))
	{
		int nLines;
		RtString str = GenerateItemHtmlHint(nLines, m_itemID, GetWorld()->m_pPlayer);

		g_workspace.SetHintText((const char*)str, true);
		//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
		g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}else
	{
		m_pBtnItem->ModifyFlags(0, wfHint);
	}

	unguard;
}

void UIFormSale::OnPreDraw()
{
	//if(!ItemID_IsValid(m_itemID))
	//{
	//	m_itemID.Clear();	
	//	m_pLabelName->SetText("");
	//	m_pBasePrice->SetText("");
	//	m_pUniquePrice->SetText("");
	//	m_pHandlingCharge->SetText("");
	//}

	SItemBase *pItemClass = NULL;
	if (ItemID_IsValid(m_itemID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_itemID.type);
		if(!pItemClass)			return;
//		m_pBtnItem->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnItem->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );

		if (ItemCanPile(m_itemID))
		{
			char cTmp128[128];
			rt2_sprintf(cTmp128, "%d", m_itemID.count);
			m_pBtnItem->SetText(cTmp128);
			//m_pBtnItem->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]
			std::string name = GetWorld()->m_pItemManager->GetItem(m_itemID.type)->name;
			m_pLabelName->SetText(name + "(" + rtFormatNumber((long)m_itemID.count) + ")");
		}else
		{
			m_pBtnItem->SetText("");
			m_pLabelName->SetText(GetWorld()->m_pItemManager->GetItem(m_itemID.type)->name);
		}
		//m_pBasePrice->SetText("");
		//m_pUniquePrice->SetText("");
	}else
	{
		m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);
		m_pBtnItem->SetText("");
		m_pLabelName->SetText("");
		m_pBasePrice->SetText("");
		m_pUniquePrice->SetText("");
	}
		
	m_pList->RemoveAllItems();
	int id=0;
	stlforeach(std::list<CAuctionItem>, it, *(g_SalesroomGC.GetDealing()))
	{
		if((*it).BargainorChr() == GetWorld()->m_pPlayer->GetName())
		{
			 m_pList->AddItem("");
			//m_pList->SetItemIcon(id, );
			RtwImage* itemicon=g_workspace.getImageFactory()->createImage(GetWorld()->m_pItemManager->GetItem((*it).ItemID().type)->icon);
			m_pList->SetLineItemImage(id,0,itemicon);
			m_pList->SetItemText(id, 1, GetWorld()->m_pItemManager->GetItem((*it).ItemID().type)->name);
			m_pList->SetItemText(id, 2, rtFormatNumber(GetWorld()->m_pItemManager->GetItem((*it).ItemID().type)->requirement[ItemRequirement_Level]));
			if(((*it).Duration() * 3600 - (*it).PassTime()) / 3600 == 0)
			{
				//m_pList->SetItemText(id, 3, "小于1小时");//by fox for string
				m_pList->SetItemText(id, 3, R(MSG_BID_TIME_LESS_ONE_HOUR));
			}else
			{
				std::string time = rtFormatNumber(((*it).Duration() * 3600 - (*it).PassTime()) / 3600);
				//time = time + "小时";//by fox for string
				time = time + R(MSG_BID_HOUR);
				m_pList->SetItemText(id, 3, time.c_str());
			}
			m_pList->SetItemText(id, 4, (char*)(*it).PurchaserChr().c_str());
			m_pList->SetItemText(id, 5, rtFormatNumber((*it).BidPrice()));
			m_pList->SetItemText(id, 6, rtFormatNumber((*it).UniquePrice()));
		
		

			m_pList->SetItemData(id, (void*)&(*it));
			id++;
			// 注释 [3/17/2009 tooth.shi]
// 			SItemBase *pItemClass = NULL;
// 			if (ItemID_IsValid((*it).ItemID()))
// 			{
// 				pItemClass = GetWorld()->m_pItemManager->GetItem((*it).ItemID().type);
// 				if(!pItemClass)			return;
// 				m_pList->SetItemIcon(id, new RtwImage(pItemClass->icon));
// 			}
		}
	}
}

//void UIFormSale::OnDragItem(RtwWidget* sender, void*)
//{
	//const string& widgetID = sender->GetID();
	//int i, j;
	//g_layerMain->m_fromPlayerItems->Is_Widget_Common(widgetID, i, j);

	//SItemID itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(i, j);
	//SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
	//if(!pItemBase) return;

	//if (GetWorld()->m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
	//	return;
	//
	//if(!ItemCanTrade(itemID))
	//{
	//	UIFormMsg::ShowStatic(R(MSG_AUCTION_CANNOT_TRADE), UIFormMsg::TYPE_OK);
	//}

	//	
	//string strID = "QS_" + sender->GetID();
	//RtwButton* pButton = NULL;
	//pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strID));
	//if (pButton==NULL)
	//{
	//	pButton = (RtwButton*)sender->Clone(NULL, strID);
	//	pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);
	//	pButton->LoadAttributesFromTemplate("IconBorder");

	//	DWORD *pDword = new DWORD[3];
	//	pDword[0] = UIHotKey_Item;
	//	int itemCount = 0;
	//	if (ItemIsYaoPing(itemID))
	//	{
	//		int p1st, i1st, j1st;
	//		GetWorld()->m_pPlayer->mItem.m_Bag.FindType( GetWorld()->m_pItemManager->GetIndexFromType(itemID.type), ItemColor_All, &itemCount, &p1st, &i1st, &j1st);
	//		SItemID item1st = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(p1st, i1st, j1st);
	//		pDword[1] = item1st.id;
	//		pDword[2] = item1st.type;
	//		pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
	//		pButton->SetText(rtFormatNumber(itemCount));
	//	}
	//	else 
	//	{
	//		pDword[1] = itemID.id;
	//		pDword[2] = itemID.type;
	//	}
	//	pButton->SetUserData(pDword);
	//	pButton->EvHint		+= RTW_CALLBACK(this, UIFormSale, OnGetHintText_QuickStart);
	//}
	//g_workspace.SetDragServer(pButton);
//}

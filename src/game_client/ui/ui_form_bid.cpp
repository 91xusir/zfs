
#include "gc_include.h"
#include "ui_form_bid.h"
#include "ui_form_vendor.h"
#include "net_command_auction.h"
#include "ui_form_msg.h"

UIFormBid::UIFormBid()
{
	m_pFrmThis  = LOAD_UI("fmOrder");
	m_pBtnItem	= LOAD_UI_T(RtwButton, "fmOrder.panOrder.pansource01.btnskill");
	m_pLabelName	= LOAD_UI_T(RtwLabel, "fmOrder.panOrder.pansource01.lbskillinfo");
	m_pBidPrice	= LOAD_UI_T(RtwTextBox, "fmOrder.panOrder.lbPricenum");
	m_pUniquePrice	= LOAD_UI_T(RtwLabel, "fmOrder.panOrder.lbPricenum1");
	m_pHandlingCharge	= LOAD_UI_T(RtwLabel, "fmOrder.panOrder.lbPricenum2");
	m_pTotal			= LOAD_UI_T(RtwLabel, "fmOrder.panOrder.lbPricenum3");
	m_pBtnBid			= LOAD_UI_T(RtwButton, "fmOrder.panOrder.btnBuy");
	m_pBtnCancelBid		= LOAD_UI_T(RtwButton, "fmOrder.btnBuy");
	m_pList				= LOAD_UI_T(RtwListBox, "fmOrder.fmAuctionRight");
	m_pBtnBack			= LOAD_UI_T(RtwButton, "fmOrder.btnBack");
	m_pBtnRefresh		= LOAD_UI_T(RtwButton, "fmOrder.btnRefresh");

	//m_pList->ModifyFlags(wfHint, 0);//rem by fox
	//m_pList->SetColumnFormat(RtwStringFormat(alignCenter,alignCenter));//add by fox // 注释 [3/17/2009 tooth.shi]

	m_pBidPrice->EvKey += RTW_CALLBACK(this, UIFormBid, OnBidPriceChanged);
	m_pBtnBack->EvLClick += RTW_CALLBACK(this, UIFormBid, OnBtnBackClicked);
	m_pBtnBid->EvLClick += RTW_CALLBACK(this, UIFormBid, OnBtnBidClicked);
	m_pBtnCancelBid->EvLClick += RTW_CALLBACK(this, UIFormBid, OnBtnCancelBidClicked);
	//m_pList->EvHint		 += RTW_CALLBACK(this, UIFormBid, OnGetListHintItemText);
	m_pBtnItem->EvHint	 += RTW_CALLBACK(this, UIFormBid, OnGetBtnHintText);
	//m_pList->EvSelect	 += RTW_CALLBACK(this, UIFormBid, OnListItemSelect);//rem by fox
	m_pBtnRefresh->EvLClick	+= RTW_CALLBACK(this, UIFormBid, OnBtnRefreshClicked);;
	//m_pList->EvMouseMove	+= RTW_CALLBACK(this, UIFormBid, OnMouseMove);
	m_pList->EvMouseWheel	+= RTW_CALLBACK(this, UIFormBid, OnMouseMove);//add by fox
	m_pList->EvSelect	    += RTW_CALLBACK(this, UIFormBid, OnMouseMove);//add by fox
	m_pList->ModifyFlags(0,wfMouseMove);
	m_pFrmThis->EvMouseMove	+= RTW_CALLBACK(this, UIFormBid, OnMouseMove);
	m_pFrmThis->EvHint		 += RTW_CALLBACK(this, UIFormBid, OnGetListHintItemText);
    m_lLastAuctionId = -1;
	m_LastItemID.Clear();
	m_pLabelName->SetText("");
	m_pHandlingCharge->SetText("");
	m_pBidPrice->SetText("");
	m_pUniquePrice->SetText("");
	m_pTotal->SetText("");

	m_pBidPrice->SetNumberOnly(true);

	m_index = -1;

	Hide();

}

void UIFormBid::OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent)
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

void UIFormBid::OnBidPriceChanged(RtwWidget* sender, void*)
{
	
	//m_pBidPrice->SetText(rtFormatNumber((long)(pItem->BidPrice() * 1.05 * 1.05)));

	//m_pUniquePrice->SetText(rtFormatNumber(pItem->UniquePrice()));

	long bid_price = atol(m_pBidPrice->GetText().c_str());
	long handling_charge = bid_price * 0.05;
	long total = bid_price + handling_charge;
	
	m_pHandlingCharge->SetText(rtFormatNumber(handling_charge));

	m_pTotal->SetText(rtFormatNumber(total));
}

void UIFormBid::OnBtnRefreshClicked(RtwWidget* sender, void*)
{
	CAuctionNetHelper::NetSend_c2r_auction_update();
}

void UIFormBid::Hide()
{
    m_lLastAuctionId = -1;
	m_LastItemID.Clear();
	m_pFrmThis->Hide();
}

void UIFormBid::OnListItemSelect(RtwWidget* sender, void*)
{
	//m_pList->ModifyFlags(wfHint, 0);//rem by fox
}

void UIFormBid::ShowDetail(CAuctionItem* pItem)
{
	if(!pItem)
	{
        m_lLastAuctionId = -1;
		m_LastItemID.Clear();
		
		m_pBtnItem->SetBackgroundImage((RtwImage*)NULL);

		m_pBtnItem->SetText("");
		
		m_pBidPrice->SetText("");

		m_pLabelName->SetText("");

		m_pUniquePrice->SetText("");

		m_pHandlingCharge->SetText("");

		m_pTotal->SetText("");

		return;
	}

	m_pBtnItem->ModifyFlags(wfHint, 0);

    m_lLastAuctionId = pItem->AuctionId();
	m_LastItemID = pItem->ItemID();

	m_item = *pItem;

	SItemBase *pItemClass = NULL;
	
	if(ItemID_IsValid(m_LastItemID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_LastItemID.type);
		if(!pItemClass)			return;
//		m_pBtnItem->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
		m_pBtnItem->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );

		if (ItemCanPile(m_LastItemID))
		{
			char cTmp128[128];
			rt2_sprintf(cTmp128, "%d", m_LastItemID.count);
			m_pBtnItem->SetText(cTmp128);
			//m_pBtnItem->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]
			std::string name = GetWorld()->m_pItemManager->GetItem(m_LastItemID.type)->name;
			m_pLabelName->SetText(name + "(" + rtFormatNumber((long)m_LastItemID.count) + ")");
		}else
		{
			m_pBtnItem->SetText("");
			m_pLabelName->SetText(GetWorld()->m_pItemManager->GetItem(m_LastItemID.type)->name);
		}

		//m_pLabelName->SetText(GetWorld()->m_pItemManager->GetItem(m_LastItemID.type)->name);
		
		long bid_price = (long)(pItem->BidPrice() * 1.05);
		long handling_charge = bid_price * 0.05;
		long total = bid_price + handling_charge;

		bid_price = bid_price > pItem->UniquePrice() ? pItem->UniquePrice() : bid_price;
		handling_charge = bid_price * 0.05;
		total = bid_price + handling_charge;

		if(total > pItem->UniquePrice())
		{
			total = pItem->UniquePrice();
		}

		m_pBidPrice->SetText(rtFormatNumber(bid_price));
		
		m_pUniquePrice->SetText(rtFormatNumber(pItem->UniquePrice()));

		m_pHandlingCharge->SetText(rtFormatNumber(handling_charge));

		m_pTotal->SetText(rtFormatNumber(total));
	}else
	{
		m_pBtnItem->SetText("");

		m_pBidPrice->SetText("");

		m_pLabelName->SetText("");

		m_pUniquePrice->SetText("");

		m_pHandlingCharge->SetText("");

		m_pTotal->SetText("");

	}
}

void UIFormBid::ClearView()
{
	Refresh();
}

void UIFormBid::Show(CAuctionItem* pItem)
{
	guard;
	if (m_pFrmThis)
	{
		if (!IsVisible())
		{			
			OnPreDraw();
			ShowDetail(pItem);
			m_pFrmThis->Show();			
		}
	}
	unguard;
}

bool UIFormBid::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormBid::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		OnPreDraw();
		ShowDetail(NULL);
		m_pFrmThis->Show();
	}

	unguard;
}

void UIFormBid::OnGetListHintItemText(RtwWidget* sender, void*)
{
	/*//rem by fox
	int idx = m_index;

	if(idx == -1) 
	{
		m_pList->ModifyFlags(0, wfHint);
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

void UIFormBid::OnGetBtnHintText(RtwWidget* sender, void*)
{
	guard;

	if(ItemID_IsValid(m_LastItemID))
	{
		int nLines;
		RtString str = GenerateItemHtmlHint(nLines, m_LastItemID, GetWorld()->m_pPlayer);

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

void UIFormBid::OnBtnBidClicked(RtwWidget* sender, void*)
{
	if(!ItemID_IsValid(m_LastItemID))
	{
		return;
	}
	
	if(m_item.BidPrice() >= atol((char*)m_pBidPrice->GetText().c_str()))
	{
		//UIFormMsg::ShowStatic("该物品现在价格比你出价更高", UIFormMsg::TYPE_OK);//by fox for string
		UIFormMsg::ShowStatic(R(MSG_ITEM_PRICE_HIGHER), UIFormMsg::TYPE_OK);
		
		long bid_price = (long)(m_item.BidPrice() * 1.05);

		bid_price = bid_price > m_item.UniquePrice() ? m_item.UniquePrice() : bid_price;

		m_pBidPrice->SetText(rtFormatNumber(bid_price));

		return;
	}

	CAuctionNetHelper::NetSend_c2r_auction_buy(m_lLastAuctionId, atol((char*)m_pBidPrice->GetText().c_str()));

    m_lLastAuctionId = -1;
	m_LastItemID.Clear();
}

void UIFormBid::OnBtnBackClicked(RtwWidget* sender, void*)
{
	Hide();

	g_layerMain->m_formVendor->Show();
	g_layerMain->m_formVendor->Refresh();
}

void UIFormBid::OnBtnCancelBidClicked(RtwWidget* sender, void*)
{
	int idx = m_pList->GetSelectedItem();

	if(idx == -1) return;

	CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(idx);

	if(!pItem) return;

    CAuctionNetHelper::NetSend_c2r_auction_cancel_buy(pItem->AuctionId());

}

void UIFormBid::OnPreDraw()
{
	m_pList->RemoveAllItems();
	int id =0;
	stlforeach(std::list<CAuctionItem>, it, *(g_SalesroomGC.GetDealing()))
	{
		if((*it).PurchaserChr() == GetWorld()->m_pPlayer->GetName())
		{
		    m_pList->AddItem("");
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

			m_pList->SetItemText(id, 4, (char*)(*it).BargainorChr().c_str());
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

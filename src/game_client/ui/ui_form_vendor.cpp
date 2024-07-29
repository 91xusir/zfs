
#include "gc_include.h"
#include "auction_base.h"
#include "item_base.h"
#include "ui_form_vendor.h"
#include "ui_form_sale.h"
#include "ui_form_bid.h"
#include "game_assistant.h"
#include "net_command_auction.h"

CSalesroom	g_SalesroomGC;
CSalesroom	g_SalesroomGCPart1;
CSalesroom	g_SalesroomGCPart2;

int	GetItemClass(CAuctionItem* item)
{
	int klass	= AUCTION_METIER_ALL;
	
	SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item->ItemID().type);
	if (!pItemClass)
	{
		return 0;
	}

	BYTE cTmp = pItemClass->requirement[ItemRequirement_Career];
	if ( cTmp==0x0F)
	{
		klass = AUCTION_METIER_ALL;
	}else if ( (cTmp & 0x01) != 0x0) //"战士和";
	{
		klass = AUCTION_METIER_WARRIOR;
	}else if ( (cTmp & 0x02) != 0x0) //"女猎手和";
	{
		klass = AUCTION_METIER_HUNTER;
	}else if ( (cTmp & 0x04) != 0x0) //"道士和";
	{
		klass = AUCTION_METIER_TAOIST;
	}else if ( (cTmp & 0x08) != 0x0) //"术士和";
	{
		klass = AUCTION_METIER_WIZARD;
	}

	return klass;
}

UIFormVendor::UIFormVendor()
{
	m_pFrmThis  = LOAD_UI("fmVendor");
	m_pTree		= LOAD_UI_T(RtwTree, "fmVendor.panItem");
	m_pList		= LOAD_UI_T(RtwListBox, "fmVendor.fmAuctionRight");
	
	m_pBtnSell		= LOAD_UI_T(RtwButton, "fmVendor.btnSell");
	m_pBtnBuy		= LOAD_UI_T(RtwButton, "fmVendor.btnBuy");
	m_pBtnRefresh	= LOAD_UI_T(RtwButton, "fmVendor.btnRefresh");
	m_btn1			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn1");
	m_btn2			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn2");
	m_btn3			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn3");
	m_btn4			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn4");
	m_btn5			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn5");
	m_btn6			=LOAD_UI_T(RtwButton, "fmVendor.panItem.btn6");
//	m_pList->SetColumnFormat(RtwStringFormat(alignCenter,alignCenter));//add by fox
	m_pList->ModifyFlags(wfHint,0);
	//m_pList->ModifyFlags(wfHint, 0);
	//						LOAD_UI_T(RtwButton, "fmVendor.panItem.btn1");
	m_pBtnSell->EvLClick	+= RTW_CALLBACK(this, UIFormVendor, OnBtnSellClicked);
	m_pBtnBuy->EvLClick		+= RTW_CALLBACK(this, UIFormVendor, OnBtnBuyClicked);
	//m_pList->EvHint			+= RTW_CALLBACK(this, UIFormVendor, OnGetListHintItemText);
	m_pTree->EvSelectNode	+= RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	//m_pList->EvSelect		+= RTW_CALLBACK(this, UIFormVendor, OnListItemSelect);//rem by fox
	m_pBtnRefresh->EvLClick	+= RTW_CALLBACK(this, UIFormVendor, OnBtnRefreshClicked);
	//m_pList->EvMouseMove	+= RTW_CALLBACK(this, UIFormVendor, OnMouseMove);
	m_pList->SetHoverSelect(true);
	//m_pList->EvMouseWheel	+= RTW_CALLBACK(this, UIFormVendor, OnMouseMove);//add by fox
	//m_pList->EvSelect		+= RTW_CALLBACK(this, UIFormVendor, OnMouseMove);//add by fox
	//m_pList->ModifyFlags(0,wfMouseMove);
	//m_pFrmThis->EvMouseMove	+= RTW_CALLBACK(this, UIFormVendor, OnMouseMove);
	m_pList->EvMoveItem  +=RTW_CALLBACK(this, UIFormVendor, OnMouseMove);
	m_pList->EvHintItem		+= RTW_CALLBACK(this, UIFormVendor, OnGetListHintItemText);
	m_btn1->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	m_btn2->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	m_btn3->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	m_btn4->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	m_btn5->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	m_btn6->EvLClick        += RTW_CALLBACK(this, UIFormVendor, OnTreeNodeSelect);
	//m_pList->SetSelectedMode(RtwListBox::MOUSE_MOVE_MODE);

	m_index = -1;

	Hide();
}

void UIFormVendor::OnPreDraw()
{
	//ShowItemList(AUCTION_METIER_ALL);
}

void UIFormVendor::OnRefresh()
{
	Refresh();
}

void UIFormVendor::OnMouseMove(RtwWidget* sender, RtwEventDelegate* pEvent)
{	
// 	m_pList->ModifyFlags(wfHint,0);
// 	g_workspace.ShowHint(m_pList,0);
// 	CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(m_pList->GetSelectedItem());
// 
// 	if(!pItem) return;
// 
// 	if( !ItemID_IsValid(pItem->ItemID()) )
// 	{
// 		return;
// 	}
// 
// 	int nLines;
// 	RtString str = GenerateItemHtmlHint(nLines, pItem->ItemID(), GetWorld()->m_pPlayer);
// 
// 	g_workspace.SetHintText((const char*)str, true);
// 	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
// 	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
// 	g_workspace.AutoHintSize();
// 	g_workspace.AutoHintPosition();
}

void UIFormVendor::OnBtnRefreshClicked(RtwWidget* sender, void*)
{
	CAuctionNetHelper::NetSend_c2r_auction_update();
}

void UIFormVendor::OnListItemSelect(RtwWidget* sender, void*)
{
	//m_pList->ModifyFlags(wfHint, 0);
}

void UIFormVendor::OnGetListHintItemText(RtwWidget* sender, void*)
{


	//<rem by fox>

// 	if(m_index == -1)
// 	{
// 		m_pList->ModifyFlags(0, wfHint);	
// 		return;
// 	}

	//</rem by fox>

	CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(m_pList->GetSelectedItem());

	if(!pItem) return;

	if( !ItemID_IsValid(pItem->ItemID()) )
	{
		return;
	}
	
	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, pItem->ItemID(), GetWorld()->m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 280);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
}

void UIFormVendor::ShowItemList(int klass)
{
	m_pList->RemoveAllItems();

	int i=0;
	int id =0;
	stlforeach(std::list<CAuctionItem>, it, *(g_SalesroomGC.GetDealing()))
	{
		i++;
		if( (klass == 0xff)|| ((klass != 0xff) && (GetItemClass((CAuctionItem*)&(*it)) == klass)) )
		{
			CAuctionItem* pItem = (CAuctionItem*)&(*it);
            SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem((*it).ItemID().type);
			 m_pList->AddItem("");
            if (pItemClass)
            {
				RtwImage* itemicon=g_workspace.getImageFactory()->createImage(pItemClass->icon);
				m_pList->SetLineItemImage(id,0,itemicon);
			    m_pList->SetItemText(id, 1, pItemClass->name);
			    m_pList->SetItemText(id, 2, rtFormatNumber(pItemClass->requirement[ItemRequirement_Level]));
            }else
            {
                //m_pList->SetItemText(id, 1, "未知的道具");//by fox for string
				m_pList->SetItemText(id, 1, R(MSG_UNKNOW_ITEM));
            }
			
			if(((*it).Duration() * 3600 - (*it).PassTime()) / 3600 == 0)
			{
				//m_pList->SetItemText(id, 3, "小于1小时");//by fox for string
				m_pList->SetItemText(id, 3, R(MSG_BID_TIME_LESS_ONE_HOUR));
			}else
			{
				DWORD t1 = (*it).Duration() * 3600;
				DWORD t2 = (*it).PassTime();

				std::string time = rtFormatNumber((t1 - t2) / 3600);
				//time = time + "小时";//by fox for string
				time = time + R(MSG_BID_HOUR);
				m_pList->SetItemText(id, 3, time.c_str());
			}
			
			m_pList->SetItemText(id, 4, (char*)(*it).BargainorChr().c_str());
			m_pList->SetItemText(id, 5, rtFormatNumber((*it).BidPrice()));
			m_pList->SetItemText(id, 6, rtFormatNumber((*it).UniquePrice()));

			m_pList->SetItemData(id, (void*)&(*it));

			CAuctionItem* item = (CAuctionItem*)&(*it);

			if(pItemClass)
            {
			    if (ItemID_IsValid((*it).ItemID()))
			    {
				    //m_pList->SetItemIcon(id, new RtwImage(pItemClass->icon));// change [3/17/2009 tooth.shi]
					m_pList->SetItemIcon(id,g_workspace.getImageFactory()->createImage(pItemClass->icon));
			    }
            }
			id++;
		}
	}
	
	//switch(klass)
	//{
	//case AUCTION_METIER_ALL:
	//	{
	//		
	//	}
	//	break;
	//case AUCTION_METIER_WARRIOR:
	//	break;
	//case AUCTION_METIER_HUNTER:
	//	break;
	//case AUCTION_METIER_TAOIST:
	//	break;
	//case AUCTION_METIER_WIZARD:
	//	break;
	//default:
	//	break;
	//}
}

void UIFormVendor::OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	RtwButton* pLastItem =(RtwButton*)pSender;
	
			
		if(pLastItem->getName() == "layworld.fmVendor.panItem.btn1")
		{
			ShowItemList(AUCTION_METIER_WARRIOR);
		}else if(pLastItem->getName() =="layworld.fmVendor.panItem.btn2")
		{
			ShowItemList(AUCTION_METIER_HUNTER);
		}else if(pLastItem->getName() == "layworld.fmVendor.panItem.btn3")
		{
			ShowItemList(AUCTION_METIER_TAOIST);
		}else if(pLastItem->getName() == "layworld.fmVendor.panItem.btn4")
		{
			ShowItemList(AUCTION_METIER_WIZARD);
		}else if(pLastItem->getName() == "layworld.fmVendor.panItem.btn5")
		{
			ShowItemList(AUCTION_METIER_ALL);
		}else if(pLastItem->getName() =="layworld.fmVendor.panItem.btn6")
		{
			ShowItemList(0xff);
		}
	
	m_pFrmThis->Show();
	//Refresh();
	unguard;
}

void UIFormVendor::Hide()
{
	m_index = -1;
	m_pFrmThis->Hide();
}

void UIFormVendor::Show()
{
	guard;
	if (m_pFrmThis)
	{
		if (!IsVisible())
		{
			ShowItemList(0xff);
			m_pFrmThis->Show();
		}
	}
	unguard;
}

bool UIFormVendor::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormVendor::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		RtwTree::Item* pLastItem = m_pTree->GetLastSelect();
		
 		if (!pLastItem)
 		{
 			ShowItemList(0xff);
 		}else
 		{
 			if(pLastItem->text == R(MSG_AUCTION_METIER_WARRIOR))
 			{
 				ShowItemList(AUCTION_METIER_WARRIOR);
 			}else if(pLastItem->text == R(MSG_AUCTION_METIER_HUNTER))
 			{
 				ShowItemList(AUCTION_METIER_HUNTER);
 			}else if(pLastItem->text == R(MSG_AUCTION_METIER_TAOIST))
 			{
 				ShowItemList(AUCTION_METIER_TAOIST);
 			}else if(pLastItem->text == R(MSG_AUCTION_METIER_WIZARD))
 			{
 				ShowItemList(AUCTION_METIER_WIZARD);
 			}else if(pLastItem->text == R(MSG_OTHER_ITEM))
 			{
 				ShowItemList(AUCTION_METIER_ALL);
 			}else if(pLastItem->text == R(MSG_ALL_ITEM))
 			{
 				ShowItemList(0xff);
 			}
 		}
		m_pFrmThis->Show();
	}

	unguard;
}

void UIFormVendor::OnBtnBuyClicked(RtwWidget* sender, void*)
{
	int idx = m_pList->GetSelectedItem();

	if(idx == -1) 
	{
		Hide();
		g_layerMain->m_formBid->Show(NULL);
		return;
	}

	CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(idx);

	if(!pItem) return;

	Hide();

	g_layerMain->m_formBid->Show(pItem);
}

void UIFormVendor::OnBtnSellClicked(RtwWidget* sender, void*)
{
	//int idx = m_pList->GetSelectedItem();

	//if(idx == -1) return;

	//CAuctionItem*	pItem = (CAuctionItem*)m_pList->GetItemData(idx);

	Hide();

	g_layerMain->m_formSale->Show();
}

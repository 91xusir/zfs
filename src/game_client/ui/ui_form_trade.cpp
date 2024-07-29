#include "gc_include.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_shop.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_decompound.h"
#include "ui_form_union_create.h"
#include "ui_form_msg.h"
#include "ui_form_textMsg.h"
#include "ui_form_char.h"
#include "ui_form_pet.h"

UIFormTrade::UIFormTrade()
:m_trade(NULL, NULL, 0, 0, GetWorld()->m_pItemManager)
{
	guard;

	m_pFrmThis = LOAD_UI("fmtrade");
	m_pFrmGridsUp = LOAD_UI("fmtrade.fmotheritem");
	m_pFrmGridsDown = LOAD_UI("fmtrade.fmselfitem");
	m_lbNameUp = LOAD_UI("fmtrade.fmlbother.lbother");
	m_lbNameDown = LOAD_UI("fmtrade.fmlbself.lbself");
	m_lbMoneyUp = LOAD_UI("fmtrade.lbOtherMoney");
	m_lbMoneyDown = LOAD_UI("fmtrade.lbSelfMoney");
	m_btnMoney = LOAD_UI_T(RtwButton, "fmtrade.btngold");
	m_btnOK = LOAD_UI_T(RtwButton, "fmtrade.btnok");
	m_btnOK->EvLClick += RTW_CALLBACK(this, UIFormTrade, OnOK);
	m_btnDeal = LOAD_UI_T(RtwButton, "fmtrade.btncancel");
	m_btnDeal->EvLClick += RTW_CALLBACK(this, UIFormTrade, OnDeal);
	m_btnMoney->EvLClick += RTW_CALLBACK(this, UIFormTrade, OnMoney);
	//	LOAD_UI("fmtrade.$hide")->EvLClick += RTW_CALLBACK(this, UIFormTrade, OnClose);
	m_pFrmThis->EvHide += RTW_CALLBACK(this, UIFormTrade, OnClose);

	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui\\textures\\highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);
	// 注册其他窗口中的事件
	LOAD_UI("fmcharall.tabcharall.fmsoc.btntrade")->EvLClick += RTW_CALLBACK(this, UIFormTrade, OnBtnInvite);

	RtwRect rc = m_pFrmGridsUp->GetFrameRect();
	int left = rc.Left() +1;
	int top = rc.Top() +3;
	// 创建Inventory的格子
#define GRID_SIZE_X		38
#define GRID_SIZE_Y		38
	RtwButton* widget = NULL;
	char strId[32];
	for (int i=0; i<TRADE_GRID_LINE_COUNT; i++)
		for (int j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
		{
			rt2_sprintf(strId, "Trade_Up_%d_%d", i, j);

			m_btnGridsUp[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			//m_btnGridsUp[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnGridsUp[i][j]->SetName(strId);
			m_pFrmGridsUp->AddChild(m_btnGridsUp[i][j]);
			m_btnGridsUp[i][j]->SetBorderColor(0x60c0c0aa);
			m_btnGridsUp[i][j]->SetBorderSize(1);
			m_btnGridsUp[i][j]->SetHighlightImage(m_ptrHoverImage);

			RtwRect sRect;
			//sRect.left = 2 + (GRID_SIZE_X+1)*j;
			//sRect.top = 2 + (GRID_SIZE_Y+1)*i;
			//sRect.right = sRect.left + GRID_SIZE_X;
			//sRect.bottom = sRect.top + GRID_SIZE_Y;
			sRect.left = (GRID_SIZE_Y+3)*j+3;
			sRect.top = (GRID_SIZE_X+5)*i+6;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_btnGridsUp[i][j]->Move(sRect);
			m_btnGridsUp[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
			//			m_btnGridsUp[i][j]->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
			//			m_btnGridsUp[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]

			m_btnGridsUp[i][j]->EvHint += RTW_CALLBACK(this, UIFormTrade, OnGetHintText_Up);
		}
		rc = m_pFrmGridsDown->GetFrameRect();
		left = rc.Left() +1;
		top = rc.Top() +3;
		for (int i=0; i<TRADE_GRID_LINE_COUNT; i++)
			for (int j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
			{
				char strId[32];

				rt2_sprintf(strId, "Trade_Down_%d_%d", i, j);

				m_btnGridsDown[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
				m_btnGridsDown[i][j]->SetName(strId);
				m_pFrmGridsDown->AddChild(m_btnGridsDown[i][j]);
				m_btnGridsDown[i][j]->SetBorderColor(0x60c0c0aa);
				m_btnGridsDown[i][j]->SetBorderSize(1);
				m_btnGridsDown[i][j]->SetHighlightImage(m_ptrHoverImage);

				RtwRect sRect;
				//sRect.left = 2 + (GRID_SIZE_X+1)*j;
				//sRect.top = 2 + (GRID_SIZE_Y+1)*i;
				//sRect.right = sRect.left + GRID_SIZE_X;
				//sRect.bottom = sRect.top + GRID_SIZE_Y;
				sRect.left = (GRID_SIZE_Y+3)*j+3;
				sRect.top = (GRID_SIZE_X+5)*i+6;
				sRect.right = sRect.left + GRID_SIZE_X;
				sRect.bottom = sRect.top + GRID_SIZE_Y;
				m_btnGridsDown[i][j]->Move(sRect);
				m_btnGridsDown[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
				//m_btnGridsDown[i][j]->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
				//			m_btnGridsDown[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]
				m_btnGridsDown[i][j]->EvHint += RTW_CALLBACK(this, UIFormTrade, OnGetHintText_Down);
				m_btnGridsDown[i][j]->EvDrag += RTW_CALLBACK(this, UIFormTrade, OnDrag_Down);
			}

	m_pFrmOtherInvite = NULL;
	m_timeLastInvite = 0;
	m_actorInviteU = 0;
	m_bPressDeal = false;
	m_pFrmThis->Hide();
	m_bRefuseInvite = false;

	m_pFmPetTrade	= LOAD_UI_T(RtwForm, "fmtrade2");
	m_pBtnToTrade	= LOAD_UI_T(RtwButton, "fmtrade2.petsellblt");
	m_pBtnToMine	= LOAD_UI_T(RtwButton, "fmtrade2.petreturnblt");
	m_pListPetMine	= LOAD_UI_T(RtwListBox, "fmtrade2.conjureform02.mywithlist");
	m_pListPetTrade = LOAD_UI_T(RtwListBox, "fmtrade2.conjureform03.myselllist");
	m_pListPetOther = LOAD_UI_T(RtwListBox, "fmtrade2.conjureform01.obpetlist");
	m_pBtnTradePet	= LOAD_UI_T(RtwButton, "fmtrade.pettradeblt");
	m_pBtnLookPet	= LOAD_UI_T(RtwButton, "fmtrade2.btcheckchar");
	m_p3DView		= LOAD_UI_T(Rtw3DView, "fmtrade2.conjureform04");

	m_pFmPetTrade->EvHide		+= RTW_CALLBACK(this, UIFormTrade, OnHidePetTrade);
	m_pBtnToTrade->EvLClick		+= RTW_CALLBACK(this, UIFormTrade, OnLClickPetToTrade);
	m_pBtnToMine->EvLClick		+= RTW_CALLBACK(this, UIFormTrade, OnLClickPetToMine);
	m_pBtnTradePet->EvLClick	+= RTW_CALLBACK(this, UIFormTrade, OnLClickTradePet);
	m_pListPetMine->EvLDClick	+= RTW_CALLBACK(this, UIFormTrade, OnLDClickListPet);
	m_pListPetOther->EvLDClick	+= RTW_CALLBACK(this, UIFormTrade, OnLDClickListPet);
	m_pListPetTrade->EvLDClick	+= RTW_CALLBACK(this, UIFormTrade, OnLDClickListPet);
	m_pBtnLookPet->EvLClick		+= RTW_CALLBACK(this, UIFormTrade, OnLClickLookPet);
	m_pListPetOther->EvSelect	+= RTW_CALLBACK(this, UIFormTrade, OnSelectPet);
	
	ResetPetTrade();
	unguard;
}

void UIFormTrade::OnLDClickListPet(RtwListBox *sender,void *)
{
	RtwListBox::SLine *pLine =  sender->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			if (sender == m_pListPetOther)
			{
				g_layerMain->m_formPetMain->Show(*pItem);
			}
			else
			{
				g_layerMain->m_formPetMain->Show(*pItem);
			}
		}
	}
}

void UIFormTrade::HidePetTrade()
{
	m_pFmPetTrade->Hide();
	ResetPetTrade();
}

void UIFormTrade::OnSelectPet(RtwButton *sender,void *)
{
	RtwListBox::SLine *pLine =  m_pListPetOther->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem)
		{
			m_p3DView->ReleaseAllActor();
			SItemID pet = *pItem;
			if (ItemIsPet(pet))
			{
				SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(pet.type);
				SNpc* pNpc = g_TableNpc.FindNpc(pItemPet->petID);
				if(pNpc)
				{
					SCreModel *model = g_TableCreModel.FindCreModel(pNpc->ModelId);
					if (model)
					{
						int cont = m_p3DView->AddCreateActor((char*)model->Act.c_str());
						CRT_ActorInstance *p = m_p3DView->GetActorByCont(cont);
						p->PlayPose("wait",true);
					}
				}
			}
		}
	}
}

void UIFormTrade::OnLClickLookPet(RtwButton *sender,void *)
{
	RtwListBox::SLine *pLine =  m_pListPetOther->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			g_layerMain->m_formPetMain->Show(*pItem);
		}
	}
}

void UIFormTrade::ShowPetTrade()
{
	if (!m_pFmPetTrade->IsVisible())
	{
		m_pFmPetTrade->Show();
		OnPreDrawPetTrade();
	}
	else
	{
		RefreshPetTrade();
	}
}

void UIFormTrade::OnHidePetTrade(RtwWidget *sender,void *)
{
	if (m_listPetTrade.empty() && m_listPetOther.empty())
		return;
	
	m_trade.GetTradePetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
	m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
	if (m_trade.IsLocked( &(GetPlayer()->mItem) ) )
	{
		m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		m_trade.UnLock();
		CItemNetHelper::NetSend_c2r_trade_unlock();
	}
	m_p3DView->ReleaseAllActor();
	ResetPetTrade();
}

void UIFormTrade::RefreshPetTrade()
{
	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	SItemID item;
	int id = 0;
	SItemPet *pItemPet = NULL;
	SPet *pPet = NULL;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);

	CBag *pMyTradePetBag = m_trade.GetTradePetBag(pMyContainer);
	CBag *pOtherTradePetBag = m_trade.GetTradeOtherPetBag(pMyContainer);

	ResetPetTrade();

	if (!m_pFmPetTrade->IsVisible() && (!pMyTradePetBag->IsEmpty() || !pOtherTradePetBag->IsEmpty()))
		m_pFmPetTrade->Show();

	SItemID pet;
	int idMine = 0;
	int idOther = 0;
	for (int i=0; i<ITEM_PET_BAG_LINE; i++)
	{
		for (int j=0; j<ITEM_PET_BAG_COLUMN; j++)
		{
			pet = pMyTradePetBag->GetItem(i, j);
			if (ItemIsPet(pet))
			{
				m_listPetTrade.push_back(pet);
				m_pListPetTrade->AddItem(pet.name);
				m_pListPetTrade->SetLineParam(idMine,&m_listPetTrade.back());
				idMine++;
			}

			pet = pOtherTradePetBag->GetItem(i, j);
			if (ItemIsPet(pet))
			{
				m_listPetOther.push_back(pet);
				m_pListPetOther->AddItem(pet.name);
				m_pListPetOther->SetLineParam(idOther,&m_listPetOther.back());
				idOther++;
			}
		}
	}

	id = 0;
	for (int i=0; i<ITEM_PET_BAG_COLUMN; i++)
	{
		item = pBag->GetItem(0, i);
		if (ItemIsPet(item))
		{
			list<SItemID>::iterator iter = m_listPetTrade.begin();
			for (;iter!=m_listPetTrade.end();++iter)
			{
				SItemID pet = *iter;
				if (ItemIsPet(pet) && pet.id == item.id)
				{
					break;
				}
			}
			if (iter == m_listPetTrade.end())
			{
				pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(item.type);
				if (pItemPet)
				{
					pPet = g_TablePet.FindPet(pItemPet->petID);
					if (pPet)
					{
						m_listPetMine.push_back(item);
						m_pListPetMine->AddItem(item.name);
						m_pListPetMine->SetLineParam(id,&m_listPetMine.back());
						id++;
					}
				}
			}
		}
	}
}

void UIFormTrade::ResetPetTrade()
{
	m_listPetMine.clear();
	m_listPetTrade.clear();
	m_listPetOther.clear();
	m_pListPetMine->RemoveAllItems();
	m_pListPetTrade->RemoveAllItems();
	m_pListPetOther->RemoveAllItems();
}

void UIFormTrade::OnPreDrawPetTrade()
{
	ResetPetTrade();

	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	SItemID item;
	SItemPet *pItemPet = NULL;
	SPet *pPet = NULL;
	int id = 0;

	if (m_listPetMine.empty())
	{
		for (int i=0; i<ITEM_PET_BAG_COLUMN; i++)
		{
			item = pBag->GetItem(0, i);
			if (ItemIsPet(item))
			{
				pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(item.type);
				if (pItemPet)
				{
					pPet = g_TablePet.FindPet(pItemPet->petID);
					if (pPet)
					{
						m_listPetMine.push_back(item);
						m_pListPetMine->AddItem(item.name);
						m_pListPetMine->SetLineParam(id,&m_listPetMine.back());
						id++;
					}
				}
			}
		}
	}
}

void UIFormTrade::OnLClickTradePet(RtwButton *sender,void *)
{
	if (m_trade.IsLocked( &(GetPlayer()->mItem) ) )
	{
		m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		m_trade.UnLock();
		CItemNetHelper::NetSend_c2r_trade_unlock();
	}
	if (!m_pFmPetTrade->IsVisible())
	{
		ShowPetTrade();
	}
	else
	{
		HidePetTrade();
	}
}

void UIFormTrade::OnLClickPetToTrade(RtwButton *sender,void *)
{

	CBag *pBag = &GetPlayer()->mItem.m_PetBag;
	if (!pBag) return;

	RtwListBox::SLine *pLine =  m_pListPetMine->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			if (GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetItem().id == pItem->id)
			{
				PlayerFailOperate(R(MSG_CANNOT_TRADE_PET_CALLOUT));
				return;
			}

			GcPlayer *pPlayer = GetWorld()->m_pPlayer;
			char bMissionBag;

			CHECK(pPlayer!=NULL);
			bMissionBag = 0;

			SItemID itemFrom = *pItem;

			if (itemFrom.cBind==IB_Binded)
			{
				PlayerFailOperate(R(MSG_TRADE_BINDED));
				return;
			}

			if (ItemIsPet(itemFrom))
			{
				bMissionBag = 2;
			}
			int page,i,j;
			if (!pBag->Find(itemFrom,&page,&i,&j))
				return;

			ETradableResult tradalbe = pPlayer->mItem.CanItemTrade(2, pPlayer->mItem.m_PetBag.GetPageIndex(), i, j);
			if ( tradalbe==TR_Can )
			{
				if (m_trade.IsLocked( &(pPlayer->mItem) ) )
				{
					m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
					m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
					m_trade.UnLock();
					CItemNetHelper::NetSend_c2r_trade_unlock();
				}
				if ( m_trade.PutIn( &(pPlayer->mItem), pBag->GetPageIndex(), i, j, 0, 0, bMissionBag) )
				{
					int id = m_pListPetMine->getSelectLine();
					if (m_pListPetMine->RemoveLine(id))
					{
						if (m_pListPetTrade->AddItem(pItem->name))
						{
							m_listPetTrade.push_back(*pItem);
							id = m_pListPetTrade->getLineCount() - 1;
							m_pListPetTrade->SetLineParam(id,pItem);
						}
					}
				}
				g_workspace.ResetHintText();
			}
			else 
			{
				if (tradalbe==TR_CanNot_Essence)
				{
					PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
					return;
				}
				else if (tradalbe==TR_CanNot_Pet)
				{
					PlayerFailOperate(R(MSG_STORE_PET_FIRST));
					return;
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
					return;
				}
			}
		}
	}
}

void UIFormTrade::OnLClickPetToMine(RtwButton *sender,void *)
{
	RtwListBox::SLine *pLine =  m_pListPetTrade->getSelectLineItem();
	if (pLine)
	{
		SItemID *pItem = (SItemID*)pLine->param;
		if (pItem && ItemIsPet(*pItem))
		{
			CBag* pBag = m_trade.GetTradePetBag(&(GetPlayer()->mItem));

			if (m_trade.IsLocked( &(GetPlayer()->mItem) ) )
			{
				m_trade.UnLock();
				m_trade.GetTradeOtherBag( &(GetPlayer()->mItem) )->Clear();
				m_trade.GetTradeOtherPetBag( &(GetPlayer()->mItem) )->Clear();
				CItemNetHelper::NetSend_c2r_trade_unlock();
			}

			int page,i,j;
			if (!pBag->Find(pItem->id,&page,&i,&j))
				return;

			if ( m_trade.TakeOut( &(GetPlayer()->mItem), i, j, pBag->GetPageIndex(), 0, 0,true) )
			{
				int id = m_pListPetTrade->getSelectLine();
				if (m_pListPetTrade->RemoveLine(id))
				{
					list<SItemID>::iterator iter = m_listPetTrade.begin();
					for (;iter!=m_listPetTrade.end();++iter)
					{
						if ((*iter).id == pItem->id)
						{
							if (m_pListPetMine->AddItem(pItem->name))
							{
								id = m_pListPetMine->getLineCount() - 1;
								m_pListPetMine->SetLineParam(id,pItem);
							}
							iter = m_listPetTrade.erase(iter);
							break;
						}
					}
				}
			}
			g_workspace.ResetHintText();
		}
	}
}

UIFormTrade::~UIFormTrade()
{
	guard;

	unguard;
}

void UIFormTrade::OnPreDraw()
{
	guard;

	if ( !m_trade.IsValid() )
		return;

	SItemID item;
	SItemBase *pItemClass = NULL;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);
	CBag *pMyTradeBag = m_trade.GetTradeBag(pMyContainer);
	CBag *pOtherTradeBag = m_trade.GetTradeOtherBag(pMyContainer);

	CBag *pMyTradePetBag = m_trade.GetTradePetBag(pMyContainer);
	CBag *pOtherTradePetBag = m_trade.GetTradeOtherPetBag(pMyContainer);

	if ( !(pMyContainer && m_pPlayerUp && m_pPlayerDown && pMyTradeBag && pOtherTradeBag) )
		return;

	pMyTradeBag->Pinch();
	pOtherTradeBag->Pinch();

	char szTmp[64];

	RtwImage* pImagegrids = g_workspace.getImageFactory()->createImage("ui/textures/gridsitem.tga");
	// 自己的道具

	for (int i=0; i<TRADE_GRID_LINE_COUNT; i++)
	{
		for (int j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
		{
			item = pMyTradeBag->GetItem(i, j);
			char *szImage;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsDown[i][j]->ModifyFlags(wfDragServer|wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				CHECK(pItemClass!=NULL);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					//m_btnGridsDown[i][j]->SetBackgroundImage(new RtwImage(szImage));// change [3/17/2009 tooth.shi]
					m_btnGridsDown[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown[i][j]->SetText(szTmp);
					}
					else 
					{
						m_btnGridsDown[i][j]->SetText("");
					}
				}
				else 
					ERR("没有找到道具的icon\n");

				m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
				m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
				m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ptrHoverImage->getRenderImage()->SetBlend(true);
				m_btnGridsDown[i][j]->SetHoverImage(m_ptrHoverImage);
			}
			else
			{
				m_btnGridsDown[i][j]->ModifyFlags(0, wfDragServer|wfHint);
				m_btnGridsDown[i][j]->SetBackgroundImage(pImagegrids);
				m_btnGridsDown[i][j]->SetText("");

				RtwComplexImage hoverImg;
				m_btnGridsDown[i][j]->SetHoverImage(hoverImg);
			}
		}
	}

		// 对方的道具
	for (int i=0; i<TRADE_GRID_LINE_COUNT; i++)
	{
		for (int j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
		{
			item = pOtherTradeBag->GetItem(i, j);
			char *szImage;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsUp[i][j]->ModifyFlags(wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				CHECK(pItemClass);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					//m_btnGridsUp[i][j]->SetBackgroundImage(new RtwImage(szImage));// change [3/17/2009 tooth.shi]
					m_btnGridsUp[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsUp[i][j]->SetText(szTmp);
					}
					else 
					{
						m_btnGridsUp[i][j]->SetText("");
					}
				}
				else 
					ERR("没有找到道具的icon\n");

				m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
				m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
				m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ptrHoverImage->getRenderImage()->SetBlend(true);
				m_btnGridsUp[i][j]->SetHoverImage(m_ptrHoverImage);
			}
			else
			{
				m_btnGridsUp[i][j]->ModifyFlags(0, wfHint);
				m_btnGridsUp[i][j]->SetBackgroundImage(pImagegrids);
				m_btnGridsUp[i][j]->SetText("");

				RtwComplexImage hoverImg;
				m_btnGridsUp[i][j]->SetHoverImage(hoverImg);
			}
		}
	}

	// 自己是否已经点了"交易"
	bool bLockMine, bLockOther;
	bLockMine = m_trade.IsLocked( pMyContainer );
	bLockOther = m_trade.IsLocked( m_trade.GetOtherContainer(pMyContainer) );
	if ( bLockMine && bLockOther )
	{
		m_btnOK->Disable();
		if (m_bPressDeal)
			m_btnDeal->Disable();
		else 
			m_btnDeal->Enable();
	}
	else if (bLockMine)
	{
		m_bPressDeal = false;
		m_btnOK->Disable();
		m_btnDeal->Disable();
	}
	else if (bLockOther)
	{
		m_bPressDeal = false;
		m_btnOK->Enable();
		m_btnDeal->Disable();
	}
	else
	{
		m_bPressDeal = false;
		m_btnOK->Enable();
		m_btnDeal->Disable();
	}

	// 人物名字
	m_lbNameUp->SetText( m_pPlayerUp->GetName() );
	m_lbNameDown->SetText( m_pPlayerDown->GetName() );

	// 金钱
	int moneyDown = pMyTradeBag->GetMoney();
	int moneyUp = pOtherTradeBag->GetMoney();
	rt2_snprintf(szTmp, 64, "%d", moneyDown);
	//szTmp[63] = 0;
	m_lbMoneyUp->SetText(szTmp);
	rt2_snprintf(szTmp, 64, "%d", moneyUp);
	//szTmp[63] = 0;
	m_lbMoneyDown->SetText(szTmp);

	if (!pMyTradePetBag->IsEmpty() || !pOtherTradePetBag->IsEmpty() || m_pFmPetTrade->IsVisible())
	{
		RefreshPetTrade();
	}

	unguard;
}

void UIFormTrade::Show()
{
	guard;

	if (IsShowing())			return;

	if (g_layerMain)
	{
		//if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
		//	g_layerMain->m_fromPlayerItems->Hide();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石合成
			g_layerMain->m_formGem->Hide();
		if (g_layerMain->m_formShop->IsVisible())			// NPC商店
			g_layerMain->m_formShop->Hide();
		//if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
		//	g_layerMain->m_formPShop->Hide();
		if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
			g_layerMain->m_formLookPlayerShop->Hide();
		//if (g_layerMain->m_formTrade->IsShowing())			// 交易
		//	g_layerMain->m_formTrade->Hide();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())	// 道具分解
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}

	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormTrade::IsShowing()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormTrade::Hide()
{
	guard;
	if (m_pFrmThis)
	{
		m_pFrmThis->Hide();
		HidePetTrade();
	}
	unguard;
}

void UIFormTrade::Refresh()
{
	guard;
	if (IsShowing())
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormTrade::OnBtnInvite(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;

	ShowSystemMessage(R(MSG_ITEM_SELECT_TRADE_PLAYER));

	//RtwButton* pButton = (RtwButton*)vpSender;
	RtwButton* pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	pButton->CloneFrom((RtwButton*)vpSender);
	RtwAliasButton* pActive = (RtwAliasButton*)g_workspace.LookupWidget("fmhotkey.lbhotkeyenable");
	if(!pButton || !pActive) return;
	pActive->SetButton(pButton);

	unguard;
}

void UIFormTrade::InviteOther(DWORD actorID)
{
	guard;
	CItemNetHelper::NetSend_c2r_trade_invite(actorID);
	ShowSystemMessage(R(MSG_ITEM_WAIT_FOR_MESSAGE));
	unguard;
}

void UIFormTrade::ResponseOther_Agree(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;
	m_pFrmOtherInvite = NULL;
	CItemNetHelper::NetSend_c2r_trade_invite_response( m_actorInviteU, 1);
	m_actorInviteU = 0;
	unguard;
}

void UIFormTrade::ResponseOther_Disagree(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;
	m_pFrmOtherInvite = NULL;
	CItemNetHelper::NetSend_c2r_trade_invite_response( m_actorInviteU, 0);
	m_actorInviteU = 0;
	unguard;
}

// 是不是下面的格子
bool UIFormTrade::Is_Widget_Down(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<TRADE_GRID_LINE_COUNT; i++)
		for (j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
		{
			const string& idDown = m_btnGridsDown[i][j]->GetID();
			if ( idDown==id )
				goto Next;
		}
Next:
		if (i<TRADE_GRID_LINE_COUNT && j<TRADE_GRID_COLUMN_COUNT)
		{
			outI = i;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

bool UIFormTrade::Is_Widget_Up(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<TRADE_GRID_LINE_COUNT; i++)
		for (j=0; j<TRADE_GRID_COLUMN_COUNT; j++)
		{
			const string& idUp = m_btnGridsUp[i][j]->GetID();
			if ( idUp==id )
				goto Next;
		}
Next:
		if (i<TRADE_GRID_LINE_COUNT && j<TRADE_GRID_COLUMN_COUNT)
		{
			outI = i;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

void UIFormTrade::OnGetHintText_Down(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pMyTradeBag = m_trade.GetTradeBag( &(GetWorld()->m_pPlayer->mItem) );
	CBag *pOtherTradeBag = NULL;
	int i, j;
	Is_Widget_Down(widgetID, i, j);
	SItemID itemID = pMyTradeBag->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, pMyTradeBag, 0, i, j);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormTrade::OnDrag_Down(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	GcPlayer *pPlayer = GetWorld()->m_pPlayer;

	const string& widgetID_c = sender->GetID();
	const string& widgetID_s = pDragSvr->GetID();

	int iTo, jTo;
	Is_Widget_Down(widgetID_c, iTo, jTo);

	int i,j;
	CBag *pBagFrom, *pBagTo;
	bool bMissionBag;
	if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(widgetID_s, i, j) )
	{
		CHECK(pPlayer!=NULL);
		pBagFrom = &(pPlayer->mItem.m_Bag);
		pBagTo = m_trade.GetTradeBag( &(pPlayer->mItem) );
		bMissionBag = false;

		SItemID itemFrom = pBagFrom->GetItem(i, j);
		SItemID itemTo = pBagTo->GetItem(iTo, jTo);

		if (itemFrom.cBind==IB_Binded)
		{
			PlayerFailOperate(R(MSG_TRADE_BINDED));
			return;
		}

		ETradableResult tradalbe = pPlayer->mItem.CanItemTrade(1, pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
		if ( tradalbe==TR_Can )
		{
			if (m_trade.IsLocked( &(pPlayer->mItem) ) )
			{
				m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
				m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
				m_trade.UnLock();
				CItemNetHelper::NetSend_c2r_trade_unlock();
			}
			if ( m_trade.PutIn( &(pPlayer->mItem), pBagFrom->GetPageIndex(), i, j, iTo, jTo, bMissionBag) )
			{
				pBagFrom->SetVisible( pBagFrom->GetPageIndex(), i, j, false);
			}
			g_workspace.ResetHintText();
			Refresh();
			g_layerMain->m_fromPlayerItems->Refresh();
			if(g_workspace.GetMouseDragWidget() != NULL)
				g_workspace.SetMouseDragWidget(NULL);
		}
		else 
		{
			if (tradalbe==TR_CanNot_Essence)
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
				return;
			}
			else if (tradalbe==TR_CanNot_Pet)
			{
				PlayerFailOperate(R(MSG_STORE_PET_FIRST));
				return;
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
				return;
			}
		}
	}
	else if (g_layerMain->m_fromPlayerItems->Is_Widget_Mission(widgetID_s, i, j))
	{
		CHECK(pPlayer!=NULL);
		pBagFrom = &(pPlayer->mItem.m_MissionBag);
		pBagTo = m_trade.GetTradeBag( &(pPlayer->mItem) );
		bMissionBag = true;

		SItemID itemFrom = pBagFrom->GetItem(i, j);
		SItemID itemTo = pBagTo->GetItem(iTo, jTo);

		if (itemFrom.cBind==IB_Binded)
		{
			PlayerFailOperate(R(MSG_TRADE_BINDED));
			return;
		}

		ETradableResult tradalbe = pPlayer->mItem.CanItemTrade(1, pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
		if ( tradalbe==TR_Can )
		{
			if (m_trade.IsLocked( &(pPlayer->mItem) ) )
			{
				m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
				m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
				m_trade.UnLock();
				CItemNetHelper::NetSend_c2r_trade_unlock();
			}
			if ( m_trade.PutIn( &(pPlayer->mItem), pBagFrom->GetPageIndex(), i, j, iTo, jTo, bMissionBag) )
			{
				pBagFrom->SetVisible( pBagFrom->GetPageIndex(), i, j, false);
			}
			g_workspace.ResetHintText();
			Refresh();
			g_layerMain->m_fromPlayerItems->Refresh();
			if(g_workspace.GetMouseDragWidget() != NULL)
				g_workspace.SetMouseDragWidget(NULL);
		}
		else 
		{
			if (tradalbe==TR_CanNot_Essence)
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
				return;
			}
			else if (tradalbe==TR_CanNot_Pet)
			{
				PlayerFailOperate(R(MSG_STORE_PET_FIRST));
				return;
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CAN_NOT_TRADE));
				return;
			}
		}
	}

	return;

	unguard;
}

void UIFormTrade::OnGetHintText_Up(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pMyTradeBag = NULL;
	CBag *pOtherTradeBag = m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) );
	int i, j;
	Is_Widget_Up(widgetID, i, j);
	SItemID itemID = pOtherTradeBag->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, pOtherTradeBag, 0, i, j);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormTrade::OnMoney(RtwWidget* sender, void*)
{
	guard;
	UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_MONEY_COUNT), R(MSG_ITEM_TRADEMONEY_COUNT));
	pFrm->SetNumberOnly(true);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormTrade, OnTradeMoney, pFrm);
	unguard;
}

void UIFormTrade::OnOK(RtwWidget* sender, void*)
{
	guard;

	bool bNeedBankPass = false;
	CBag *pBag = m_trade.GetTradeBag( &(GetWorld()->m_pPlayer->mItem) );
	std::vector<SItemID> items;
	pBag->ToItemsArray(items);
	for (int i=0; i<(int)items.size(); i++)
	{
		SItemID& item = items[i];
		if (item.cLock!=0)
		{
			bNeedBankPass = true;
			break;
		}
	}

	if (!bNeedBankPass)
	{
		CBag *pPetBag = m_trade.GetTradePetBag( &(GetWorld()->m_pPlayer->mItem) );
		std::vector<SItemID> items;
		pPetBag->ToItemsArray(items);
		for (int i=0; i<(int)items.size(); i++)
		{
			SItemID& item = items[i];
			if (item.cLock!=0)
			{
				bNeedBankPass = true;
				break;
			}
		}
	}

	if (bNeedBankPass)
	{
		UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_PROTECT), R(MSG_NEED_BANKPASS));
		pFrm->SetPassword(true);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormTrade, cbLock, pFrm);
	}
	else
	{
		DoLock("");
	}

	unguard;
}

void UIFormTrade::OnDeal(RtwWidget* sender, void*)
{
	guard;
	CItemNetHelper::NetSend_c2r_trade_deal();
	m_bPressDeal = true;
	Refresh();
	unguard;
}

void UIFormTrade::OnClose(RtwWidget* sender, void*)
{
	guard;
	CItemNetHelper::NetSend_c2r_trade_cancel();
	unguard;
}

void UIFormTrade::OnTradeMoney(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if (!IsShowing())
		return;

	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	RtString str = pFrm->GetString();
	long money = str.ToLong();
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	if (!MoneyIsValid(money))
	{
		PlayerFailOperate(R(MSG_ITEM_INVALID_NUMBER));
		return;
	}
	if ( money > pCon->m_Bag.GetMoney() )
	{
		PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_MONEY));
		return;		// @@@@
	}
	if ( m_trade.IsLocked( &(GetWorld()->m_pPlayer->mItem) ) )
	{
		m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		m_trade.UnLock();
		CItemNetHelper::NetSend_c2r_trade_unlock();
	}
	//	m_trade.GetTradeBag(pCon)->SetMoney(money);
	m_trade.PutIn( &(GetWorld()->m_pPlayer->mItem), money);
	Refresh();

	unguard;
}

// 10秒钟之后如果玩家没有相应,就当作拒绝交易
void UIFormTrade::OnTimerEvent(DWORD dwID)
{
	guard;

	if (m_pFrmOtherInvite)
	{
		m_pFrmOtherInvite->Hide();//modify by fox
		m_pFrmOtherInvite = NULL;
		//		CItemNetHelper::NetSend_c2r_trade_invite_response(m_actorInviteU, 0);
		m_actorInviteU = 0;
	}
	GetTimer()->RemoveTimer(dwID);

	unguard;
}

void UIFormTrade::OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData)
{
	guard;

	if (pTargetActor==NULL) return;

	if (g_layerMain->m_formTrade->IsShowing() )
	{
		return;
	}
	if (   pTargetActor->NpcType() != GcActor::ENT_NPC_SERVICE
		&& pTargetActor->NpcType() != GcActor::ENT_NPC_COMBATIVE
		&& pTargetActor->NpcType() != GcActor::ENT_MONSTER)
	{
		pPopupMenu->AddItem(R(G_TRADE), NULL, (void*)iData);
	}

	unguard;
}

bool UIFormTrade::OnCommand(GcActor* pTargetActor)
{
	guard;
	if (pTargetActor==NULL) return false;

	if (g_layerMain->m_formTrade->IsShowing() )
	{
		LOG("正在交易\n");
		return true;
	}

	if (pTargetActor->NpcType()==GcActor::ENT_NPC_SERVICE ||
		pTargetActor->NpcType()==GcActor::ENT_NPC_COMBATIVE ||
		pTargetActor->NpcType()==GcActor::ENT_MONSTER)
	{
		PlayerFailOperate(R(THAT_OBJECT_NOT_PLAYER));
		return false;
	}

	int currTime = rtGetMilliseconds();

	if (pTargetActor->NpcType()==GcActor::ENT_NPC_SERVICE)
	{
		PlayerFailOperate(R(MSG_ITEM_NOT_PLAYER));
	}
	else
	{
		if ( abs(currTime - m_timeLastInvite) <= 5000)
		{
			PlayerFailOperate(R(MSG_ITEM_WAITFOR_NEXTINVITE));
		}
		else
		{
			m_timeLastInvite = currTime;
			InviteOther( pTargetActor->ID() );
		}
	}
	return true;
	unguard;
}

bool UIFormTrade::MouseRClick(int x, int y)
{
	guard;
	return OnCommand(GetWorld()->HitTest(x, y, 0xFC));
	unguard;
}

void UIFormTrade::cbLock(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	RtString str = pFrm->GetString();
	DoLock(str);

	unguard;
}

void UIFormTrade::DoLock(const char* strBankPass)
{
	guard;

	//m_trade.Lock( &(GetWorld()->m_pPlayer->mItem) );
	Refresh();

	CBag *pBag = m_trade.GetTradeBag( &(GetWorld()->m_pPlayer->mItem) );
	SDWORDArray idArray;
	pBag->ToItemIDArray(idArray.array);

	CBag *pPetBag = m_trade.GetTradePetBag( &(GetWorld()->m_pPlayer->mItem) );
	SDWORDArray idArrayPet;
	pPetBag->ToItemIDArray(idArrayPet.array);

	CItemNetHelper::NetSend_c2r_trade_lock(idArray, idArrayPet, pBag->GetMoney(), strBankPass);

	unguard;
}



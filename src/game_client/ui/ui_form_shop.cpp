#include "gc_include.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_shop.h"
#include "ui_form_msg.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_decompound.h"
#include "ui_form_union_create.h"
#include "ui_form_textMsg.h"
#include "ui_form_char.h"
#include "gc_cursor.h"

UIFormShop::UIFormShop()
:m_bagUp(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager)
,m_bagDown(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager)
,m_bagDownSell(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager)
{
	guard;

	m_pFrmThis			=	LOAD_UI("fmnewshop");
	m_pFrmGridsUp		=	LOAD_UI("fmnewshop.fmshop.fmsellitem");
	m_pFrmGridsDown		=	LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent1.fmselfitem");
	m_pFrmGridsDown2	=	LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent2.fmsellitem");
	m_pCheckTab			=	LOAD_UI_T(RtwTab,"fmnewshop.fmshop.tbctrading");
	m_lbMoneyName1		=	LOAD_UI_T(RtwLabel,"fmnewshop.fmshop.tbctrading.fmcontent1.lbtbc2");
	m_lbWindowsTitle	=	LOAD_UI_T(RtwLabel,"fmnewshop.lbtitle");
	m_lbPaidType		=	LOAD_UI_T(RtwLabel,"fmnewshop.fmshop.tbctrading.fmcontent1.lbmoney");
	m_lbMoneyShopBuy	=	LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent1.lbbuygoldnum");
	m_lbMoneyShopSell	=	LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent2.lbbuygoldnum");
	m_btnOK				=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent1.btnok");
	m_btnSellOK			=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent2.btnok");
	m_btnCancel			=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent1.btncancel");
	m_btnSellCancel		=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent2.btncancel");
	m_btnRepairAll		=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent1.btnall");
	m_btnBuyRepairOne	=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent1.btnrepair");
	m_btnSellRepairOne	=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent2.btnrepair");
	m_btnSellRepairAll	=	LOAD_UI_T(RtwButton, "fmnewshop.fmshop.tbctrading.fmcontent2.btnall");
	m_pScrollUp			=	LOAD_UI_T(RtwVScrollBar, "fmnewshop.fmshop.fmsellitem.scrollUp");
	m_pScrollDown1		=	LOAD_UI_T(RtwVScrollBar, "fmnewshop.fmshop.tbctrading.fmcontent1.fmselfitem.scrollUp");
	m_pScrollDown2		=	LOAD_UI_T(RtwVScrollBar, "fmnewshop.fmshop.tbctrading.fmcontent2.fmsellitem.scrollUp");
	m_ptrHoverImage		=	g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");

	m_pFrmThis->EvRClick							+=	RTW_CALLBACK(this, UIFormShop, OnRClickRepair);
	m_btnOK->EvLClick								+=	RTW_CALLBACK(this, UIFormShop, OnOK);
	m_pFrmThis->EvHide								+=	RTW_CALLBACK(this, UIFormShop, OnHideForm);
	m_pFrmThis->EvFocus								+=	RTW_CALLBACK(this, UIFormShop, OnGetFocus);
	m_btnSellOK->EvLClick							+=	RTW_CALLBACK(this, UIFormShop, OnOK);
	m_btnCancel->EvLClick							+=	RTW_CALLBACK(this, UIFormShop, OnCancel);
	m_btnSellCancel->EvLClick						+=	RTW_CALLBACK(this, UIFormShop, OnCancel);
	m_btnBuyRepairOne->EvLClick						+=	RTW_CALLBACK(this, UIFormShop, OnOneRepairOK);
	m_btnSellRepairOne->EvLClick					+=	RTW_CALLBACK(this, UIFormShop, OnOneRepairOK);
	m_btnRepairAll->EvLClick						+=	RTW_CALLBACK(this, UIFormShop, OnAllRepairOK);
	m_btnSellRepairAll->EvLClick					+=	RTW_CALLBACK(this, UIFormShop, OnAllRepairOK);
	m_pScrollUp->EvScroll							+=	RTW_CALLBACK(this, UIFormShop, OnScrollUp);
	m_pScrollDown1->EvScroll						+=	RTW_CALLBACK(this, UIFormShop, OnScrollDown1);
	m_pScrollDown2->EvScroll						+=	RTW_CALLBACK(this, UIFormShop, OnScrollDown2);
	LOAD_UI("fmnewshop.btnclose")->EvLClick		+=	RTW_CALLBACK(this, UIFormShop, OnCancel);

	RtwRect rc = m_pFrmGridsUp->GetFrameRect();
	int left = rc.Left() +1;
	int top = rc.Top() +1;
	// 创建Inventory的格子
#define GRID_SIZE_X		34
#define GRID_SIZE_Y		34
	RtwButton* widget = NULL;
	for (int i=0; i<SHOP_GRID_LINE_UP_COUNT; i++)
		for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
		{
			char strId[32];
			rt2_sprintf(strId, "Shop_Up_%d_%d", i, j);

			m_btnGridsUp[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			m_pFrmGridsUp->AddChild(m_btnGridsUp[i][j]);
			m_btnGridsUp[i][j]->SetBorderColor(0x60c0c0aa);
			m_btnGridsUp[i][j]->SetBorderSize(1);
			RtwRect sRect;
			sRect.left = 2 + (GRID_SIZE_X+1)*j;
			sRect.top = 2 + (GRID_SIZE_Y+1)*i;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_btnGridsUp[i][j]->Move(sRect);
			//m_btnGridsUp[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormShop, OnLDClick_Up);
			m_btnGridsUp[i][j]->EvMouseRDown += RTW_CALLBACK(this, UIFormShop, OnLDClick_Up);
			m_btnGridsUp[i][j]->EvHint += RTW_CALLBACK(this, UIFormShop, OnGetHintText_Up);
			m_btnGridsUp[i][j]->EvDrag += RTW_CALLBACK(this, UIFormShop, OnDrag_Up);
			m_btnGridsUp[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormShop, OnDragEnd_Up);
			m_btnGridsUp[i][j]->SetName(strId);
			m_btnGridsUp[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
			//m_btnGridsUp[i][j]->EvLClick += RTW_CALLBACK(this, UIFormShop, OnLClickRepair);
			m_btnGridsUp[i][j]->EvRClick += RTW_CALLBACK(this,UIFormShop,OnRClickRepair);
		}
		rc = m_pFrmGridsDown->GetFrameRect();
		left = rc.Left() +1;
		top = rc.Top() +1;
		for (int i=0; i<SHOP_GRID_LINE_DOWN_COUNT; i++)
			for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
			{		
				char strId[32];
				rt2_sprintf(strId, "Shop_Down_%d_%d", i, j);

				m_btnGridsDown[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
				m_pFrmGridsDown->AddChild(m_btnGridsDown[i][j]);
				m_btnGridsDown[i][j]->SetBorderColor(0x60c0c0aa);
				m_btnGridsDown[i][j]->SetBorderSize(1);
				RtwRect sRect;
				sRect.left = 2 + (GRID_SIZE_X+1)*j;
				sRect.top = 2 + (GRID_SIZE_Y+1)*i;
				sRect.right = sRect.left + GRID_SIZE_X;
				sRect.bottom = sRect.top + GRID_SIZE_Y;
				m_btnGridsDown[i][j]->Move(sRect);
				//m_btnGridsDown[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormShop, OnLDClick_Down);
				m_btnGridsDown[i][j]->EvMouseRDown += RTW_CALLBACK(this, UIFormShop, OnLDClick_Down);
				m_btnGridsDown[i][j]->EvHint += RTW_CALLBACK(this, UIFormShop, OnGetHintText_Down);
				m_btnGridsDown[i][j]->EvDrag += RTW_CALLBACK(this, UIFormShop, OnDrag_Down);
				m_btnGridsDown[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormShop, OnDragEnd_Down);
				m_btnGridsDown[i][j]->SetName(strId);
				m_btnGridsDown[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
				m_btnGridsDown[i][j]->EvRClick += RTW_CALLBACK(this,UIFormShop,OnRClickRepair);
				//m_btnGridsDown[i][j]->EvDrag += RTW_CALLBACK(this,UIFormShop,OnDragFromBagToDown);

			}
			for (int i=0; i<SHOP_GRID_LINE_DOWN_COUNT; i++)
				for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
				{		
					char strId[32];
					rt2_sprintf(strId, "Shop_Down_%d_%d", i, j);

					m_btnGridsDown2[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
					m_pFrmGridsDown2->AddChild(m_btnGridsDown2[i][j]);
					m_btnGridsDown2[i][j]->SetBorderColor(0x60c0c0aa);
					m_btnGridsDown2[i][j]->SetBorderSize(1);
					RtwRect sRect;
					sRect.left = 2 + (GRID_SIZE_X+1)*j;
					sRect.top = 2 + (GRID_SIZE_Y+1)*i;
					sRect.right = sRect.left + GRID_SIZE_X;
					sRect.bottom = sRect.top + GRID_SIZE_Y;
					m_btnGridsDown2[i][j]->Move(sRect);
					//m_btnGridsDown2[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormShop, OnLDClick_Down);
					m_btnGridsDown2[i][j]->EvMouseRDown += RTW_CALLBACK(this, UIFormShop, OnLDClick_Down);
					m_btnGridsDown2[i][j]->EvHint += RTW_CALLBACK(this, UIFormShop, OnGetHintText_Down);
					//m_btnGridsDown2[i][j]->EvDrag += RTW_CALLBACK(this, UIFormShop, OnDrag_Down);
					m_btnGridsDown2[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormShop, OnDragEnd_Down);
					m_btnGridsDown2[i][j]->SetName(strId);
					m_btnGridsDown2[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
					m_btnGridsDown2[i][j]->EvDrag += RTW_CALLBACK(this,UIFormShop,OnDragFromBagToDown);
				}

				m_pCheckTab->EvChangeSelected		+= RTW_CALLBACK(this,UIFormShop,OnCheckButtonDown);
				m_fPriceRate = 0.0f;
				m_bLockOperate = false;
				m_dwLastRecvData = 0;
				m_TimeDelta = 0;
				m_dwnpcID = 0;
				m_bIsRepair = false;
				m_iSellType = 0;
				m_iPay = 0;
				m_iBagI = 0;m_iBagJ = 0;m_iBagPage = 0;
				m_pFrmThis->Hide();

	unguard;
}

UIFormShop::~UIFormShop()
{
	guard;

	unguard;
}

void UIFormShop::OnPreDraw()
{
	guard;

	SItemID item;
	SItemBase *pItemClass = NULL;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);
	CBag *pBagDown, *pBagUp, *pBagDownSell;
	pBagUp = &m_bagUp;
	pBagDown = &m_bagDown;
	pBagDownSell = &m_bagDownSell;
	char szTmp[64];

	DWORD NowSeconds = (DWORD)time(NULL) + m_TimeDelta/1000;

	pBagUp->Pinch();
	pBagDown->Pinch();
	pBagDownSell->Pinch();

	int lineStart;
	// 下面的物品栏
	if(m_pCheckTab->GetSelectedItem() == 0)
	{
		//玩家点击物品出现购买页面的新手提示
		if (!pBagDown->IsEmpty())
		{
			GetWorld()->HideGuideForm();
			GetWorld()->ShowShopTrade();
		}

		if (m_bUseScrollDown1)		lineStart = m_iScrollDown1;
		else						lineStart = 0;
		for (int i=lineStart, ii=0; ii<SHOP_GRID_LINE_DOWN_COUNT; i++,ii++)
			for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
			{
				int delta = pBagDown->Count() - (i*pBagDown->GetColCount()+j);

				item = pBagDown->GetItem(i, j);
				char *szImage;
				if ( ItemID_IsValid(item) )
				{
					m_btnGridsDown[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
					pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (strlen(pItemClass->icon) != 0)
					{
						szImage = pItemClass->icon;
						m_btnGridsDown[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
					}

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown[ii][j]->SetText(szTmp);
						m_btnGridsDown[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsDown[ii][j]->SetText("");
					}

					m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
					m_ptrHoverImage->getRenderImage()->GetImage()->eBlendDstFunc = RTGBLEND_ONE;
					m_ptrHoverImage->getRenderImage()->GetImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
					m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsDown[ii][j]->SetHoverImage(m_ptrHoverImage);
				}
				else if ((delta <= 0) && (-delta < m_RareItemsToBuy.size()))
				{
					int iRare = -delta;
					item = m_RareItemsToBuy[iRare];

					m_btnGridsDown[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
					pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (strlen(pItemClass->icon) != 0)
					{
						szImage = pItemClass->icon;
						m_btnGridsDown[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
					}

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown[ii][j]->SetText(szTmp);
						m_btnGridsDown[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsDown[ii][j]->SetText("");
					}

					m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
					m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
					m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
					m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsDown[ii][j]->SetHoverImage(m_ptrHoverImage);
				}
				else
				{
					m_btnGridsDown[ii][j]->ModifyFlags(0, wfDragServer|wfHint|wfDragOut);
					m_btnGridsDown[ii][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsDown[ii][j]->SetText("");

					RtwComplexImage hoverImg;
					m_btnGridsDown[ii][j]->SetHoverImage(hoverImg);
				}
			}

	}else
	{
		int lineStart;
		if (m_bUseScrollDown2)		lineStart = m_iScrollDown2;
		else						lineStart = 0;
		for (int i=lineStart, ii=0; ii<SHOP_GRID_LINE_DOWN_COUNT; i++,ii++)
			for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
			{
				int delta = pBagDownSell->Count() - (i*pBagDownSell->GetColCount()+j);

				item = pBagDownSell->GetItem(i, j);
				char *szImage;
				if ( ItemID_IsValid(item) )
				{
					m_btnGridsDown2[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
					pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (strlen(pItemClass->icon) != 0)
					{
						szImage = pItemClass->icon;
						m_btnGridsDown2[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
					}

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown2[ii][j]->SetText(szTmp);
						m_btnGridsDown2[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsDown2[ii][j]->SetText("");
					}

					m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
					m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
					m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
					m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsDown2[ii][j]->SetHoverImage(m_ptrHoverImage);
				}
				else if ((delta <= 0) && (-delta < m_RareItemsToBuy.size()))
				{
					int iRare = -delta;
					item = m_RareItemsToBuy[iRare];

					m_btnGridsDown2[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
					pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (strlen(pItemClass->icon) != 0)
					{
						szImage = pItemClass->icon;
						m_btnGridsDown2[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
					}

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown2[ii][j]->SetText(szTmp);
						m_btnGridsDown2[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsDown2[ii][j]->SetText("");
					}

					m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
					m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
					m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
					m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsDown2[ii][j]->SetHoverImage(m_ptrHoverImage);
				}
				else
				{
					m_btnGridsDown2[ii][j]->ModifyFlags(0, wfDragServer|wfHint|wfDragOut);
					m_btnGridsDown2[ii][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsDown2[ii][j]->SetText("");

					RtwComplexImage hoverImg;
					m_btnGridsDown2[ii][j]->SetHoverImage(hoverImg);
				}
			}

	}

	// 上面的物品栏
	if (m_bUseScrollUp)			lineStart = m_iScrollUp;
	else						lineStart = 0;
	for (int i=lineStart, ii=0; ii<SHOP_GRID_LINE_UP_COUNT; i++, ii++)
		for (int j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
		{
			int delta = pBagUp->Count() - (i*pBagUp->GetColCount()+j);

			item = pBagUp->GetItem(i, j);
			char *szImage = NULL;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsUp[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					m_btnGridsUp[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
				}

				if (ItemCanPile(item))
				{
					rt2_sprintf(szTmp, "%d", item.count);
					m_btnGridsUp[ii][j]->SetText(szTmp);
					m_btnGridsUp[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
				}
				else 
				{
					m_btnGridsUp[ii][j]->SetText("");
				}

				m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
				m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
				m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ptrHoverImage->getRenderImage()->SetBlend(true);
				m_btnGridsUp[ii][j]->SetHoverImage(m_ptrHoverImage);
				m_btnGridsUp[ii][j]->EnableAlphaEffect(false);
				m_btnGridsUp[ii][j]->SetAlphaRef(0);
			}
			else if ((delta <= 0) && (-delta < m_RareItems.size()))
			{
				int iRare = -delta;
				SNpcSellRareItem& sri = m_RareItems[iRare];
				item = sri.item;

				m_btnGridsUp[ii][j]->ModifyFlags(wfDragServer|wfHint|wfDragOut, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					m_btnGridsUp[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));
				}

				rt2_sprintf(szTmp, "%d", m_RareItems[iRare].count);
				m_btnGridsUp[ii][j]->SetText(szTmp);
				m_btnGridsUp[ii][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));

				m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
				m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
				m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ptrHoverImage->getRenderImage()->SetBlend(true);
				m_btnGridsUp[ii][j]->SetHoverImage(m_ptrHoverImage);

				if (sri.count == sri.maxCount || sri.nextSpawnTime == 0)
				{
					m_btnGridsUp[ii][j]->EnableAlphaEffect(false);
					m_btnGridsUp[ii][j]->SetAlphaRef(0);
				}
				else 
				{
					m_btnGridsUp[ii][j]->EnableAlphaEffect(true);
					m_btnGridsUp[ii][j]->SetAlphaRef(255 - 255*(NowSeconds-(sri.nextSpawnTime-sri.interval))/sri.interval);
				}
			}
			else
			{
				m_btnGridsUp[ii][j]->ModifyFlags(0, wfDragServer|wfHint|wfDragOut);
				m_btnGridsUp[ii][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsUp[ii][j]->SetText("");

				RtwComplexImage hoverImg;
				m_btnGridsUp[ii][j]->SetHoverImage(hoverImg);

				m_btnGridsUp[ii][j]->EnableAlphaEffect(false);
				m_btnGridsUp[ii][j]->SetAlphaRef(0);
			}
		}

		// 金钱

		int moneyVar = pBagDown->GetMoney();
		int	moneySell = pBagDownSell->GetMoney();
		if (m_iSellType == 1)
		{
			rt2_snprintf(szTmp, 64, "%d", moneyVar);
			m_lbMoneyShopBuy->SetText(szTmp);
			rt2_snprintf(szTmp, 64, "%d", moneySell);
			m_lbMoneyShopSell->SetText(szTmp);
		} 
		else
		{
			rt2_snprintf(szTmp, 64, "%d", m_iPay);
			m_lbMoneyShopBuy->SetText(szTmp);
			rt2_snprintf(szTmp, 64, "%d", moneySell);
			m_lbMoneyShopSell->SetText(szTmp);
		}

		m_btnOK->Enable();
		m_btnSellOK->Enable();
		unguard;
}

void UIFormShop::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		m_bBuyFromNpc = !m_pCheckTab->GetSelectedItem();
		if (m_bBuyFromNpc)
		{
			m_fPriceRate = itemshop_buy_pricerate;
			LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent1")->Show();
			LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent2")->Hide();
		}
		else
		{
			m_fPriceRate = itemshop_sell_pricerate;
			LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent1")->Hide();
			LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent2")->Show();
		}
		m_pFrmThis->Show();
		OnPreDraw();
	}

	unguard;
}

/* gao 2010.2.4
说明，这里的bool说明是不是有接口传过来的消息，如果是，
就说明这里是第一次打，我们就tabctrl设置为0即第一个
*/
void UIFormShop::Show(bool bBuyFromNpc, DWORD npcID)
{
	guard;
	g_workspace.SetFocusWidget(m_pFrmThis);
	//玩家打开商店页面的新手提示
	GetWorld()->ShowShopUI();
	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(npcID);
	SNpc* npc = NULL;npc = pNpc->m_pNpc;
	RtwImage *BackImage = NULL;
	if(!npc)
		return;
	vector<long>::iterator it;
	for(it = npc->Function.begin();it != npc->Function.end();++it)
	{
		switch(*it)
		{
		case 1:
			m_lbMoneyName1->SetText("支出金钱");
			m_lbWindowsTitle->SetText("商   店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_jinqian2.dds");
			m_iSellType = 1;
			break;
		case 8:
			m_lbMoneyName1->SetText("支出帮贡");
			m_lbWindowsTitle->SetText("帮贡商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_banggong.dds");
			m_iSellType = 8;
			break;
		case 9:
			m_lbMoneyName1->SetText("支出门贡");
			m_lbWindowsTitle->SetText("门贡商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_mengong.dds");
			m_iSellType = 9;
			break;
		case 10:
			m_lbMoneyName1->SetText("支出仙誉");
			m_lbWindowsTitle->SetText("仙誉商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_xianyu.dds");
			m_iSellType = 10;
			break;
		case 11:
			m_lbMoneyName1->SetText("支出军功");
			m_lbWindowsTitle->SetText("军功商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_jungong.dds");
			m_iSellType = 11;
			break;
		case 12:
			m_lbMoneyName1->SetText("支出俗名");
			m_lbWindowsTitle->SetText("俗名商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_suming.dds");
			m_iSellType = 12;
			break;
		case 13:
			m_lbMoneyName1->SetText("支出积分");
			m_lbWindowsTitle->SetText("积分商店");
			m_lbWindowsTitle->SetTextFont("隶书");
			m_lbWindowsTitle->SetFontSize(18);
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_jifen.dds");
			m_iSellType = 13;
			break;
		/*default:
			m_lbMoneyName1->SetText("支出金钱");
			BackImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_t_jinqian1.dds");
			m_iSellType = 1;
			break;*/
		}
	}
	if(BackImage)
	{
		BackImage->SetBlend(true);
		m_lbPaidType->SetBackgroundImage(BackImage);
	}
	m_dwnpcID = npcID;
	if(bBuyFromNpc == false)
		m_pCheckTab->SetSelectedItem(1);
	else
		m_pCheckTab->SetSelectedItem(0);
	GcActor* pPlayer = GetWorld()->m_pPlayer;
	UnlockOperate();

	if (g_layerMain)
	{
		if (!g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
			g_layerMain->m_fromPlayerItems->Show();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石合成
			g_layerMain->m_formGem->Hide();
		if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
			g_layerMain->m_formLookPlayerShop->Hide();
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Hide();
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}
	/*if(!m_pFrmThis->IsVisible())
	{
		m_bagUp.Clear();
		m_bagDown.Clear();
		m_RareItems.clear();
		m_RareItemsToBuy.clear();
	}*/


	//m_bBuyFromNpc = !m_pCheckTab->GetSelectedItem();
	m_npcID = npcID;
	CItemNetHelper::NetSend_c2r_shop_query(npcID);
	SetScrollBar();
	//SetScrollBar();
	//m_iPay = 0;
	//if (m_bBuyFromNpc)
	//{
	//	//g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	//	m_fPriceRate = itemshop_buy_pricerate;
	//	m_pScrollUp->SetValue(0);
	//	m_pScrollDown1->SetValue(0);
	//	
	//	m_bIsRepair = false;
	//}
	//else
	//{
	//	m_fPriceRate = itemshop_sell_pricerate;
	//	m_pScrollUp->SetValue(0);
	//	m_pScrollDown2->SetValue(0);
	//	//m_pFrmThis->Show();
	//}
	//
	//OnPreDraw();
	m_pFrmThis->Show();
	Refresh();
	//SetScrollBar();
	unguard;
}

void UIFormShop::OnCheckButtonDown(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(m_dwnpcID)
	{
		Refresh();
	}
	unguard;
}

bool UIFormShop::IsVisible()
{
	guard;

	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;

	unguard;
}

void UIFormShop::Hide()
{
	guard;
	if (m_pFrmThis)
	{
		m_bagUp.Clear();
		m_bagDown.Clear();
		m_bagDownSell.Clear();
		m_RareItems.clear();
		m_RareItemsToBuy.clear();
		g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
		m_bIsRepair = false;
		m_bLockOperate = false;
		m_iPay = 0;
		m_pFrmThis->Hide();
	}
	unguard;
}

// 是不是下面的格子
bool UIFormShop::Is_Widget_Down(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<SHOP_GRID_LINE_DOWN_COUNT; i++)
		for (j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
		{
			if (m_bBuyFromNpc)
			{
				const string& idDown = m_btnGridsDown[i][j]->GetID();
				if ( idDown==id )
					goto Next;
			} 
			else
			{
				const string& idDowns = m_btnGridsDown2[i][j]->GetID();
				if ( idDowns==id )
					goto NextSell;
			}
		}
Next:
		if (i<SHOP_GRID_LINE_DOWN_COUNT && j<SHOP_GRID_COLUMN_COUNT)
		{
			outI = i + m_iScrollDown1;
			outJ = j;
			return true;
		}
		else	return false;
NextSell:
		if (i<SHOP_GRID_LINE_DOWN_COUNT && j<SHOP_GRID_COLUMN_COUNT)
		{
			outI = i + m_iScrollDown2;
			outJ = j;
			return true;
		}
		else	return false;
		unguard;
}

bool UIFormShop::Is_Widget_Up(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<SHOP_GRID_LINE_UP_COUNT; i++)
		for (j=0; j<SHOP_GRID_COLUMN_COUNT; j++)
		{
			const string& idUp = m_btnGridsUp[i][j]->GetID();
			if ( idUp==id )
				goto Next;
		}
Next:
		if (i<SHOP_GRID_LINE_UP_COUNT && j<SHOP_GRID_COLUMN_COUNT)
		{
			outI = i + m_iScrollUp;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

void UIFormShop::OnGetHintText_Down(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pBag = &m_bagDown;
	CBag *pBagSell = &m_bagDownSell;

	int i, j;
	Is_Widget_Down(widgetID, i, j);

	SItemID item;
	int nLines;
	RtString str;
	if (m_bBuyFromNpc)
	{
		int delta = pBag->Count() - (i*pBag->GetColCount()+j);
		if (delta>0)
		{
			item = pBag->GetItem(0, i, j);
		}
		else if (-delta < m_RareItemsToBuy.size())
		{
			item = m_RareItemsToBuy[-delta];
		}
		if (!ItemID_IsValid(item))		return;

		if (m_iSellType != 1)
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,m_iPay);
		if (abs(m_fPriceRate-itemshop_buy_pricerate) > 0.0001)
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate);
		else 
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate);
	}
	else 
	{
		item = pBagSell->GetItem(0, i, j);
		if (!ItemID_IsValid(item))		return;

		str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate);
	}

	g_workspace.SetHintText((const char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormShop::OnDrag_Down(RtwWidget* sender, void*)
{
	guard;
	if(!m_bBuyFromNpc)
		return;
	if (!m_bIsRepair)
	{
		RtwWidget* pDragSvr = g_workspace.GetDragServer();
		if (!pDragSvr)	return;

		if (m_bLockOperate)
			return;

		const string& widgetID_s = pDragSvr->GetID();

		int i,j;
		if ( Is_Widget_Up(widgetID_s, i, j) )
		{
			Do_UpToDown(i, j);
		}
	}
	else
		return;


	unguard;
}

void UIFormShop::OnDrag_Up(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr)	return;

	if (m_bLockOperate)
		return;

	const string& widgetID_s = pDragSvr->GetID();
	int i,j;
	if ( Is_Widget_Down(widgetID_s, i, j) )
	{
		Do_DownToUp(i, j);
	}

	unguard;
}

void UIFormShop::OnLDClick_Down(RtwWidget* sender, void*)
{
	guard;

	int i,j;
	Is_Widget_Down(sender->GetID(), i, j);

	if (m_bLockOperate)
		return;

	Do_DownToUp(i, j);

	unguard;
}

void UIFormShop::OnLDClick_Up(RtwWidget* sender, void*)
{
	guard;

	if (!m_bIsRepair)
	{
		int i,j;
		Is_Widget_Up(sender->GetID(), i, j);

		if (m_bLockOperate)
			return;

		Do_UpToDown(i, j);
	}
	else
		return;

	unguard;
}

void UIFormShop::OnGetHintText_Up(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pBag = &m_bagUp;

	int i, j;
	Is_Widget_Up(widgetID, i, j);

	SItemID item;
	int delta = pBag->Count() - (i*pBag->GetColCount()+j);
	if (delta>0)
	{
		item = pBag->GetItem(0, i, j);
	}
	else if (-delta < m_RareItems.size())
	{
		item = m_RareItems[-delta].item;
	}
	if (!ItemID_IsValid(item))
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}
	
	int nLines;
	RtString str;
	int money = 0;
	SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
		
	if (pItemBase)
	{
		switch(m_iSellType)
		{
		case 1:
			if (abs(m_fPriceRate-itemshop_buy_pricerate) > 0.0001)
				str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate);
			else 
				str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate);
			break;
		case 8:
			money = (int)(pItemBase->itemUnionRecord * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		case 9:
			money = (int)(pItemBase->itemJobRecord * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		case 10:
			money = (int)(pItemBase->itemCredit * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		case 11:
			money = (int)(pItemBase->itemRecord * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		case 12:
			money = (int)(pItemBase->itemContribute * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		case 13:
			money = (int)(pItemBase->itemPoint * m_fPriceRate * item.count);
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate,0,money);
			break;
		}
	}
	else
	{
		if (abs(m_fPriceRate-itemshop_buy_pricerate) > 0.0001)
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate, 0, false, m_fPriceRate-itemshop_buy_pricerate);
		else 
			str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer, NULL, 0, i, j, m_fPriceRate);
	}
	
		
	g_workspace.SetHintText((const char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	if (m_bIsRepair)
	{
		SItemBase* pItemBase = NULL;
		CItemManager* pItemManager = NULL;
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		pItemManager = pCon->GetItemManager();
		if (ItemIsArmor(item.type)||ItemIsGem(item.type)||ItemIsShipin(item.type))
		{
			if (pItemManager)
			{
				pItemBase = pItemManager->GetItem(item.type);
			}
			if (pItemBase)
			{
				m_irepairMoney = ( (pItemBase->maxWear/100)-(item.curWear/100) );
				m_lbMoneyShopSell->SetText(rtFormatNumber(m_irepairMoney));
			}

		}
	}


	unguard;
}

void UIFormShop::OnOK(RtwWidget* sender, void*)
{
	guard;
	if (g_layerMain->m_formGuide3->IsVisible())
	{
		g_layerMain->m_formGuide3->Hide();
	}
	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CItemContainer* pCon = &(pPlayer->mItem);
	if (m_bagDown.IsEmpty() && m_RareItemsToBuy.empty() && m_bagDownSell.IsEmpty() && m_RareItems.empty())
	{
		Hide();
		return;
	}

	if (m_bLockOperate)
		return;

	if (m_bBuyFromNpc)
	{
		DoBuy();
	}
	else
	{
		bool bNeedBankPass = false;
		std::vector<SItemID> itemsToSell;
		m_bagDownSell.ToItemsArray(itemsToSell);
		for (int i=0; i<(int)itemsToSell.size(); i++)
		{
			SItemID& item = itemsToSell[i];
			if (item.cLock!=0)
			{
				bNeedBankPass = true;
				break;
			}
		}

		if (bNeedBankPass)
		{
			UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_PROTECT), R(MSG_NEED_BANKPASS));
			pFrm->SetPassword(true);
			pFrm->EvOK = RTW_CALLBACK_1(this, UIFormShop, cbSell, pFrm);
		}
		else
		{
			DoSell("");
		}
		g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	}

	unguard;
}

void UIFormShop::OnLClickRepair(SItemID &item)
{
	guard;
	if (!m_bIsRepair)
	{
		return;
	}
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemBase* pItemBase = NULL;
	CItemManager* pItemManager = NULL;
	RtgVertex3 pos;

	g_pSoundMgr->PlayOnce("ui_repair.wav", false, 0, pos);
	if (ItemID_IsValid(item) )
	{
		if(ItemIsArmor(item.type)||ItemIsWeapon(item.type)||ItemIsShipin(item.type))
		{	
			pItemManager = pCon->GetItemManager();
			if (pItemManager)
			{
				pItemBase = pItemManager->GetItem(item.type);
			}
			if (pItemBase)
			{
				int maxs = pItemBase->maxWear/100;
				int wear = item.curWear/100;
				if ( wear>= 0 && wear <  maxs )
				{
					m_irepairMoney = (maxs-wear);
					if (pCon->m_Bag.GetMoney() >= m_irepairMoney)
					{
						CItemNetHelper::NetSend_c2r_item_repair(0,item.id);
					}
					else
					{
						PlayerFailOperate(R(MSG_AUCTION_BID_NO_MONEY));
						return;
					}

				}
				else
				{
					PlayerFailOperate(R(MSG_ITEM_NOTNEEDREPAIR));
					return;
				}
			}
		}
		else 
		{
			PlayerFailOperate(R(MSG_ITEM_CANNOTREPAIR));
			return;
		}
	}
	else
	{
		PlayerFailOperate(R(OBJECT_NOT_FIND));
		return;
	}
	unguard;
};
void UIFormShop::OnOneRepairOK(RtwWidget* sender,void*)
{
	guard;

	if (!m_bIsRepair)
	{
		m_bIsRepair = true;
	}
	else
	{
		m_bIsRepair = false;
		return;
	}
	unguard;
};

void UIFormShop::OnAllRepairOK(RtwWidget* sender,void*)
{
	guard;
	CItemNetHelper::NetSend_c2r_item_repair(1,NULL);
	unguard;
};

void UIFormShop::OnRClickRepair(RtwWidget* sender,void*)
{
	guard;
	if (m_bIsRepair)
	{
		GameSetCursor(GAME_CURSOR_NORMAL);
		m_bIsRepair = false;
	} 
	else
	{
		return;
	}
	unguard;
};
void UIFormShop::OnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	m_bIsRepair = false;
	unguard;
}

void UIFormShop::OnScrollUp(RtwWidget* sender,void*)
{
	guard;

	if (m_bUseScrollUp)
	{
		m_iScrollUp = m_pScrollUp->GetValue();
		Refresh();
	}

	unguard;
}

void UIFormShop::OnScrollDown1(RtwWidget* sender,void*)
{
	guard;

	if (m_bUseScrollDown1)
	{
		m_iScrollDown1 = m_pScrollDown1->GetValue();
		Refresh();
	}

	unguard;
}

void UIFormShop::OnScrollDown2(RtwWidget* sender,void*)
{
	guard;
	if (m_bUseScrollDown2)
	{
		m_iScrollDown2 = m_pScrollDown2->GetValue();
		Refresh();
	}
	unguard;
}

void UIFormShop::cbSell(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	UIFormTextMsg *pFrm = (UIFormTextMsg *)sender->GetUser1();
	RtString str = pFrm->GetString();
	DoSell(str);

	unguard;
}

void UIFormShop::DoSell(const char* strBankPass)
{
	guard;

	SDWORDArray idArray;
	m_bagDownSell.ToItemIDArray(idArray.array);
	for (int i=0; i<(int)idArray.array.size(); i++)
	{
		SItemID item;
		int page=0, gridI=0, gridJ=0;
		if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(idArray.array[i], &page, &gridI, &gridJ) )
			continue;
		else
		{
			PlayerFailOperate(R(MSG_ITEM_EXIST));
			return;
		}
	}
	CItemNetHelper::NetSend_c2r_shop_sell(m_npcID, idArray, m_bagDownSell.GetMoney(), strBankPass);
	m_btnSellOK->Disable();
	LockOperate();

	unguard;
}

void UIFormShop::DoBuy()
{
	guard;

	SItemBase *pItemClass = NULL;
	CItemContainer* pCon = &(GetWorld()->m_pPlayer->mItem);
	std::vector<SItemID> arrItemsDeal;
	m_bagDown.ToItemsArray(arrItemsDeal);
	stlforeach(std::vector<SItemID>, iterRare1, m_RareItemsToBuy)
	{
		arrItemsDeal.push_back(*iterRare1);
	}

	int nCommonItemCount = 0;
	int nMissionItemCount = 0;
	for (int i=0; i<(int)arrItemsDeal.size(); i++)
	{
		SItemID item = arrItemsDeal[i];
		if ( ItemIsMissionItem(item.type) )
			nMissionItemCount ++;
		else 
			nCommonItemCount ++;
		pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
		CHECK(pItemClass);
	}
	if (pCon->m_Bag.FreeSpace()<nCommonItemCount || pCon->m_MissionBag.FreeSpace()<nMissionItemCount)
	{
		PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
		return ;
	}
	if (m_iSellType == 1)
	{  
		if ( pCon->GetMoney() < m_bagDown.GetMoney() )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_MONEY));
			return ;
		}
	}
	else if (m_iSellType == 8)
	{
		if ( GetPlayer()->m_core.UnionRecord < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_UNIONRECORD));
			return ;
		}
	}
	else if (m_iSellType == 9)
	{
		if ( GetPlayer()->m_core.JobRecord < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_JOBRECORD));
			return ;
		}
	}
	else if (m_iSellType == 10)
	{
		if ( GetPlayer()->m_core.Credit < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_CREDIT));
			return ;
		}
	}
	else if (m_iSellType == 11)
	{
		if ( GetPlayer()->m_core.Record < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_RECORD));
			return ;
		}
	}
	else if (m_iSellType == 12)
	{
		if ( GetPlayer()->m_core.Contribute < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_CONTRIBUTE));
			return ;
		}
	}
	else if (m_iSellType == 13)
	{
		if ( GetPlayer()->m_core.Point < m_iPay )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_POINT));
			return ;
		}
	}
	else
	{
		if ( pCon->GetMoney() < m_bagDown.GetMoney() )
		{
			PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_MONEY));
			return ;
		}
	}

	SDWORDArray idArray;
	m_bagDown.ToItemIDArray(idArray.array);
	stlforeach(std::vector<SItemID>, iterRare2, m_RareItemsToBuy)
	{
		idArray.array.push_back(iterRare2->id);
	}
	if (m_iSellType == 1)
		CItemNetHelper::NetSend_c2r_shop_buy(m_npcID, idArray, m_bagDown.GetMoney());
	else
		CItemNetHelper::NetSend_c2r_shop_buy(m_npcID, idArray, m_iPay);
	m_btnOK->Disable();
	LockOperate();

	unguard;
}

void UIFormShop::SetScrollBar()
{
	guard;

	m_pScrollDown2->SetValue(m_pScrollDown2->GetValue());

	m_iLineCountDown2 = m_iLineCountDown1 =SHOP_GRID_LINE_DOWN_COUNT; m_iLineCountUp = SHOP_GRID_LINE_UP_COUNT;
	m_bUseScrollUp = m_bUseScrollDown1 = m_bUseScrollDown2 = false;

	int nLineMaxUp = m_bagUp.GetVisibleMaxLine();
	if (nLineMaxUp <= SHOP_GRID_LINE_UP_COUNT)
	{
		m_bUseScrollUp = true;
		m_pScrollUp->SetValue(0);
		m_pScrollUp->Disable();
		m_iLineCountUp = SHOP_GRID_LINE_UP_COUNT;
		m_bUseScrollUp = false;
	} 
	else
	{
		m_pScrollUp->SetValue(m_pScrollUp->GetValue());
		m_pScrollUp->Enable();
		m_iLineCountUp = nLineMaxUp;
		m_bUseScrollUp = true;
		m_pScrollUp->SetRange(0, nLineMaxUp-SHOP_GRID_LINE_UP_COUNT);
	}
	if (m_bBuyFromNpc)
	{
		int nLineMaxDown1 = m_bagDown.GetVisibleMaxLine();
		if (nLineMaxDown1 <= SHOP_GRID_LINE_DOWN_COUNT)
		{
			m_bUseScrollDown1 = true;
			m_pScrollDown1->SetValue(0);
			m_pScrollDown1->Disable();
			m_iLineCountDown1 = SHOP_GRID_LINE_DOWN_COUNT;
			m_bUseScrollDown1 = false;
		} 
		else
		{
			m_pScrollDown1->SetValue(m_pScrollDown1->GetValue());
			m_pScrollDown1->Enable();
			m_iLineCountDown1 = nLineMaxDown1;
			m_bUseScrollDown1 = true;
			m_pScrollDown1->SetRange(0, nLineMaxDown1-SHOP_GRID_LINE_DOWN_COUNT);
		}
	}
	else
	{
		int nLineMaxDown2 = m_bagDown.GetVisibleMaxLine();
		if (nLineMaxDown2 <= SHOP_GRID_LINE_DOWN_COUNT)
		{
			m_bUseScrollDown2 = true;
			m_pScrollDown2->SetValue(0);
			m_pScrollDown2->Disable();
			m_iLineCountDown2 = SHOP_GRID_LINE_DOWN_COUNT;
			m_bUseScrollDown2 = false;
		} 
		else
		{
			m_pScrollDown2->SetValue(m_pScrollDown1->GetValue());
			m_pScrollDown2->Enable();
			m_iLineCountDown2 = nLineMaxDown2;
			m_bUseScrollDown2 = true;
			m_pScrollDown2->SetRange(0, nLineMaxDown2-SHOP_GRID_LINE_DOWN_COUNT);
		}
	}

	unguard;
}

void UIFormShop::Run()
{
	guard;

	if (!IsVisible())
		return;
	SetScrollBar();
	static DWORD dwLastRunTick = 0;
	DWORD dwNow = rtGetMilliseconds();
	if (dwNow - dwLastRunTick > 1000)
	{
		dwLastRunTick = dwNow;

		if (!m_RareItems.empty())
		{
			DWORD tt = (DWORD)time(NULL) + m_TimeDelta/1000;

			if (dwNow - m_dwLastRecvData > 10000)
			{
				for (int i=0; i<(int)m_RareItems.size(); i++)
				{
					SNpcSellRareItem& sri = m_RareItems[i];
					if ( (sri.nextSpawnTime!=0) && (tt > sri.nextSpawnTime + 1) )
					{
						CItemNetHelper::NetSend_c2r_shop_query(m_npcID);
						break;
					}
				}
			}

			//Refresh();
		}
	}

	unguard;
}

void UIFormShop::OnDragEnd_Up(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pClient = g_workspace.GetDragClient();
	if (pClient)
	{
		const string& widgetID_c = pClient->GetID();
		int i, j;
		if ( !Is_Widget_Down(widgetID_c, i, j) )
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}

void UIFormShop::OnDragEnd_Down(RtwWidget* sender, void*)
{
	guard;

	/*RtwWidget* pClient = g_workspace.GetDragClient();
	if (pClient)
	{
	const string& widgetID_c = pClient->GetID();
	int i, j;
	}*/

	unguard;
}

void UIFormShop::Do_DownToUp(int i, int j)
{
	guard;
	SItemID item;
	int delta;
	if (m_bBuyFromNpc)
	{
		delta = m_bagDown.Count() - (i*m_bagDown.GetColCount()+j);
		if (delta>0)
		{
			item = m_bagDown.GetItem(0, i, j);
		}
		else if (-delta < m_RareItemsToBuy.size())
		{
			item = m_RareItemsToBuy[-delta];
		}
		if (!ItemID_IsValid(item))		return;
	}
	
	if (m_bBuyFromNpc)
	{
		if (delta>0)
		{
			m_bagDown.RemoveItem(0, i, j);
		}
		else if (-delta < m_RareItemsToBuy.size())
		{
			std::vector<SItemID>::iterator iter1 = m_RareItemsToBuy.begin();
			for (int tmpIdx=0; tmpIdx<(-delta); tmpIdx++)
				iter1++;
			m_RareItemsToBuy.erase(iter1);
			stlforeach(std::vector<SNpcSellRareItem>, iter2, m_RareItems)
			{
				SNpcSellRareItem& sri = *iter2;
				if (sri.item.id == item.id)
				{
					sri.count ++;
					break;
				}
			}
		}
		else 
		{
			CHECK(0);
		}
	}
	else
	{
		SItemID items = m_bagDownSell.GetItem(0,i,j);
		if(!ItemID_IsValid(items))
			return;
		m_bagDownSell.RemoveItem(0, i, j);
		SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(items.type);
		CHECK(pItemBase);
		int priceBase = GetItemMgr()->GetItemValue(items);
		int price = rtRound(priceBase * m_fPriceRate * items.count);
		m_bagDownSell.RemoveMoney(price);
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&items);
		Refresh();
		return;
	}

	SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
	CHECK(pItemBase);

	int priceBase = GetItemMgr()->GetItemValue(item);
	int price = rtRound(priceBase * m_fPriceRate * item.count);

	switch(m_iSellType)
	{
	case 1:
		m_bagDown.RemoveMoney(price);
		break;
	case 8:
		m_iPay -= (int)(pItemBase->itemUnionRecord * m_fPriceRate * item.count);
		break;
	case 9:
		m_iPay -= (int)(pItemBase->itemJobRecord * m_fPriceRate * item.count);
		break;
	case 10:
		m_iPay -= (int)(pItemBase->itemCredit * m_fPriceRate * item.count);
		break;
	case 11:
		m_iPay -= (int)(pItemBase->itemRecord * m_fPriceRate * item.count);
		break;
	case 12:
		m_iPay -= (int)(pItemBase->itemContribute * m_fPriceRate * item.count);
		break;
	case 13:
		m_iPay -= (int)(pItemBase->itemPoint * m_fPriceRate * item.count);
		break;
	default:
		m_bagDown.RemoveMoney(price);
		break;
	}
	if(m_iPay <= 0)
		m_iPay = 0;
	g_workspace.ResetHintText();
	Refresh();
	unguard;
}

void UIFormShop::Do_UpToDown(int i, int j)
{
	guard;
	if(!m_bBuyFromNpc)
	{
		m_pCheckTab->SetSelectedItem(0);
		Refresh();
	}
	SItemID item;
	int delta = m_bagUp.Count() - (i*m_bagUp.GetColCount()+j);
	if (delta>0)
	{
		item = m_bagUp.GetItem(0, i, j);
	}
	else if (-delta < m_RareItems.size())
	{
		if (m_RareItems[-delta].count <= 0)
			return;
		item = m_RareItems[-delta].item;
	}
	if (!ItemID_IsValid(item))		return;

	int tmpPage, tmpI, tmpJ;
	if (m_bBuyFromNpc)
	{
		if (delta>0)
		{
			if (!m_bagDown.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
				return;
		}
		else if (-delta < m_RareItems.size())
		{
			m_RareItemsToBuy.push_back(item);

			stlforeach(std::vector<SNpcSellRareItem>, iter2, m_RareItems)
			{
				SNpcSellRareItem& sri = *iter2;
				if (sri.item.id == item.id)
				{
					sri.count --;
					break;
				}
			}
		}
		else 
		{
			CHECK(0);
		}
	}
	SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
	CHECK(pItemBase);

	int priceBase = GetItemMgr()->GetItemValue(item);
	int price = rtRound(priceBase * m_fPriceRate * item.count);
	if (GetItemMgr()->GetItemPoint(item) == 0)
	{
		switch(m_iSellType)
		{
		case 1:
			m_bagDown.AddMoney(price);
			break;
		case 8:
			m_iPay += (int)(pItemBase->itemUnionRecord * m_fPriceRate * item.count);
			break;
		case 9:
			m_iPay += (int)(pItemBase->itemJobRecord * m_fPriceRate * item.count);
			break;
		case 10:
			m_iPay += (int)(pItemBase->itemCredit * m_fPriceRate * item.count);
			break;
		case 11:
			m_iPay += (int)(pItemBase->itemRecord * m_fPriceRate * item.count);
			break;
		case 12:
			m_iPay += (int)(pItemBase->itemContribute * m_fPriceRate * item.count);
			break;
		case 13:
			m_iPay += (int)(pItemBase->itemPoint * m_fPriceRate * item.count);
			break;
		default:
			m_bagDown.AddMoney(price);
			break;
		}
	}
	g_workspace.ResetHintText();
	Refresh();


	unguard;
}

void UIFormShop::OnDragFromBagToDown(ui::RtwWidget *sender, ui::RtwEventDelegate *e)
{
	guard;
	if(m_bBuyFromNpc)
	{
		m_pCheckTab->SetSelectedItem(1);
		Refresh();
		//return;
	}
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();
	// 是不是道具栏的格子
	int i,j,page,ii,jj;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		m_iBagI = i;m_iBagJ = j;
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if(m_bagDownSell.FindID(item.id))
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
				return;
			}
			if ((item.type&ITEM_MASK_CAN_NPCTRADE) != 0x0)
			{
				//sender->ModifyFlags(wfHint, 0);
				page = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();m_iBagPage = page;
				m_bagDownSell.AddItem(item, &page, &ii, &jj, false);
				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_iBagPage,m_iBagI,m_iBagJ,true,1.0f,0xb5000000,128,true);
			}
		}
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
			return;
		}
		SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
		CHECK(pItemBase);

		int priceBase = GetItemMgr()->GetItemValue(item);
		int price = rtRound(priceBase * m_fPriceRate * item.count);
		if (GetItemMgr()->GetItemPoint(item) == 0)
		{
			m_bagDownSell.AddMoney(price);
		}
		g_workspace.ResetHintText();
		Refresh();
	}
	unguard;
}

bool UIFormShop::OnBagToShop(ui::RtwWidget *sender, ui::RtwEventDelegate *e)
{
	guard;
	if (m_bBuyFromNpc)
	{
		m_pCheckTab->SetSelectedItem(1);
		Refresh();
		//return false;
	}
	int i,j,page,ii,jj;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item))
	{
		m_iBagI = i;m_iBagJ = j;
		if(m_bagDownSell.FindID(item.id))
		{
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}
		if((item.type&ITEM_MASK_CAN_NPCTRADE) != 0x0)
		{	
			//sender->ModifyFlags(wfHint, 0);
			page = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();m_iBagPage = page;
			m_bagDownSell.AddItem(item, &page, &ii, &jj, false);
			g_layerMain->m_fromPlayerItems->OnSetItemMask(m_iBagPage,m_iBagI,m_iBagJ,true,1.0f,0xb5000000,128,true);




			SItemBase *pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
			CHECK(pItemBase);

			int priceBase = GetItemMgr()->GetItemValue(item);
			int price = rtRound(priceBase * m_fPriceRate * item.count);
			if (GetItemMgr()->GetItemPoint(item) == 0)
			{
				m_bagDownSell.AddMoney(price);
			}
			g_workspace.ResetHintText();
			Refresh();
			return true;
		}
		else 
		{
			PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
			return false;
		}
	}else
		return false;
	unguard;
}

void UIFormShop::OnGetFocus(void*,void*)
{
	guard;
	g_layerMain->OnSetNewWidget(m_pFrmThis);
	unguard;
}

void UIFormShop::OnHideForm(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(g_layerMain)
		g_layerMain->OnDeleteWidget(m_pFrmThis);
	unguard;
}

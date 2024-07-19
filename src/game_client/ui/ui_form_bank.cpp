#include "gc_include.h"
#include "item_bank.h"
#include "ui_form_textmsg.h"
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
#include "ui_form_char.h"

UIFormBank::UIFormBank()
:m_bagUp(ITEM_PLAYER_INVENTORY_LINE*ITEM_PLAYER_INVENTORY_PAGE, BANK_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager)
,m_bagDown(GetWorld()->m_pItemManager->GetBankMaxLineCount(), BANK_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager)
{
	guard;

	m_pFrmThis = RTW_WIDGET("fmnewbank");
	m_pFrmGridsUp = RTW_WIDGET("fmnewbank.fmbank.fmbankitem");
	m_pFrmGridsDown = RTW_WIDGET("fmnewbank.fmbank.fmselfitem");
	m_lbNameUp = RTW_WIDGET("fmnewbank.fmbank.fmlbbank.lbbank");
	m_lbNameDown = RTW_WIDGET("fmnewbank.fmbank.fmlbsell.lbsell");
	m_lbMoneyUp = RTW_WIDGET("fmnewbank.fmbank.lbOtherMoney");
	m_lbMoneyDown = RTW_WIDGET("fmnewbank.fmbank.lbSelfMoney");
	m_lbMoneyNameUp = RTW_WIDGET("fmnewbank.fmbank.lbbuygold");
	m_lbMoneyNameDown = RTW_WIDGET("fmnewbank.fmbank.lbsellgold");
	m_btnMoney = RTW_WIDGET_T(RtwButton, "fmnewbank.fmbank.btngold");
	m_btnMoney->EvLClick += RTW_CALLBACK(this, UIFormBank, OnBtnMoney);
	m_btnOK = RTW_WIDGET_T(RtwButton, "fmnewbank.fmbank.btnok");
	m_btnOK->EvLClick += RTW_CALLBACK(this, UIFormBank, OnOK);
	m_btnCancel = RTW_WIDGET_T(RtwButton, "fmnewbank.fmbank.btncancel");
	m_btnCancel->EvLClick += RTW_CALLBACK(this, UIFormBank, OnCancel);
//	RTW_WIDGET("fmnewbank.fmbank.$hide")->EvLClick += RTW_CALLBACK(this, UIFormBank, OnCancel);
	m_pScrollUp = RTW_WIDGET_T(RtwVScrollBar, "fmnewbank.fmbank.scrollUp");
	m_pScrollDown = RTW_WIDGET_T(RtwVScrollBar, "fmnewbank.fmbank.scrollDown");
	m_pScrollUp->EvScroll += RTW_CALLBACK(this, UIFormBank, OnScrollUp);
	m_pScrollDown->EvScroll += RTW_CALLBACK(this, UIFormBank, OnScrollDown);
    m_lbSize = RTW_WIDGET("fmnewbank.fmbank.lbbankleft");
    m_lbSizeInfo = RTW_WIDGET("fmnewbank.fmbank.lbbankleftinfo");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

//    m_ptrHoverImage = new RtwImage("highlight_button.tga");// change [3/17/2009 tooth.shi]
	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");

	RtwRect rc = m_pFrmGridsUp->GetFrameRect();
	int left = rc.Left() +1;
	int top = rc.Top() +1;
	// 创建Inventory的格子
#define GRID_SIZE_X		34
#define GRID_SIZE_Y		34
	RtwButton* widget = NULL;
	for (int i=0; i<BANK_GRID_LINE_COUNT; i++)
		for (int j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			char strId[32];
			rt2_sprintf(strId, "Bank_Up_%d_%d", i, j);
			//m_btnGridsUp[i][j] = (RtwButton*)m_pFrmGridsUp->CreateWidget(strId, "Button", RtwButton::wfDefault | wfDragClient | wfDClick);// change [3/17/2009 tooth.shi]
			//widget = m_btnGridsUp[i][j];						
			//m_btnGridsUp[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]
			//widget->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
			m_btnGridsUp[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			m_pFrmGridsUp->AddChild(m_btnGridsUp[i][j]);
			m_btnGridsUp[i][j]->SetBorderColor(0x60c0c0aa);
			m_btnGridsUp[i][j]->SetBorderSize(1);
			m_btnGridsUp[i][j]->Move(RtwRect(2 + (GRID_SIZE_X+1)*j,2 + (GRID_SIZE_Y+1)*i,2 + (GRID_SIZE_X+1)*j +GRID_SIZE_X,2 + (GRID_SIZE_Y+1)*i + GRID_SIZE_Y));
			m_btnGridsUp[i][j]->SetName(strId);
			m_btnGridsUp[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);
			//m_btnGridsUp[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormBank, OnLDClick_Up);
			m_btnGridsUp[i][j]->EvMouseRDown += RTW_CALLBACK(this, UIFormBank, OnLDClick_Up);
			m_btnGridsUp[i][j]->EvHint += RTW_CALLBACK(this, UIFormBank, OnGetHintText_Up);
			m_btnGridsUp[i][j]->EvDrag += RTW_CALLBACK(this, UIFormBank, OnDrag_Up);
			m_btnGridsUp[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormBank, OnDragEnd_Up);
		}
	rc = m_pFrmGridsDown->GetFrameRect();
	left = rc.Left() +1;
	top = rc.Top() +1;
	for (int i=0; i<BANK_GRID_LINE_COUNT; i++)
		for (int j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			char strId[32];
			rt2_sprintf(strId, "Bank_Down_%d_%d", i, j);
// 			m_btnGridsDown[i][j] = (RtwButton*)m_pFrmGridsDown->CreateWidget(strId, "Button", RtwButton::wfDefault | wfDragClient|wfDClick);
// 			widget = m_btnGridsDown[i][j];// change [3/17/2009 tooth.shi]
			//widget->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
//			m_btnGridsDown[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// 注释 [3/17/2009 tooth.shi]
			m_btnGridsDown[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			m_pFrmGridsDown->AddChild(m_btnGridsDown[i][j]);
			m_btnGridsDown[i][j]->SetBorderColor(0x60c0c0aa);
			m_btnGridsDown[i][j]->SetBorderSize(1);
			m_btnGridsDown[i][j]->Move(RtwRect(2 + (GRID_SIZE_X+1)*j,2 + (GRID_SIZE_Y+1)*i,2 + (GRID_SIZE_X+1)*j +GRID_SIZE_X,2 + (GRID_SIZE_Y+1)*i + GRID_SIZE_Y));
			m_btnGridsDown[i][j]->SetName(strId);
			m_btnGridsDown[i][j]->ModifyFlag(wfDragOut|wfDragIn|wfHint|wfDClick,0);			
			//m_btnGridsDown[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormBank, OnLDClick_Down);
			m_btnGridsDown[i][j]->EvMouseRDown += RTW_CALLBACK(this, UIFormBank, OnLDClick_Down);
			m_btnGridsDown[i][j]->EvHint += RTW_CALLBACK(this, UIFormBank, OnGetHintText_Down);
			m_btnGridsDown[i][j]->EvDrag += RTW_CALLBACK(this, UIFormBank, OnDrag_Down);
			m_btnGridsDown[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormBank, OnDragEnd_Down);
		}

	m_bLockOperate = false;
	m_npcID = 0;
    m_BankSize = 0;
    m_BankItemCount = 0;

	m_pFrmThis->Hide();

	unguard;
}

UIFormBank::~UIFormBank()
{
	guard;
	unguard;
}

void UIFormBank::OnPreDraw()
{
	guard;

	SItemID item;
	SItemBase *pItemClass = NULL;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);
	CBag *pBagDown, *pBagUp;
	pBagUp = &m_bagUp;
	pBagDown = &m_bagDown;
	char szTmp[64];
	
	pBagUp->Pinch();
	pBagDown->Pinch();

	// 下面的物品栏
	int lineStart;
	if (m_bUseScrollDown)		lineStart = m_iScrollDown;
	else						lineStart = 0;
	for (int i=lineStart, ii=0; ii<BANK_GRID_LINE_COUNT; i++,ii++)
		for (int j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			item = pBagDown->GetItem(i, j);
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsDown[ii][j]->ModifyFlags(wfDragOut|wfDragServer | wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if ( pItemClass->icon[0] != 0)
				{
					//m_btnGridsDown[ii][j]->SetBackgroundImage(new RtwImage(pItemClass->icon));// change [3/17/2009 tooth.shi]
					m_btnGridsDown[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
				}

				if (ItemCanPile(item))
				{
					rt2_sprintf(szTmp, "%d", item.count);
					m_btnGridsDown[ii][j]->SetText(szTmp);
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
				m_btnGridsDown[ii][j]->ModifyFlags(0, wfDragOut|wfDragServer|wfHint);
				m_btnGridsDown[ii][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsDown[ii][j]->SetText("");
			
                RtwComplexImage hoverImg;
                m_btnGridsDown[ii][j]->SetHoverImage(hoverImg);
			}
		}

	// 上面的物品栏
	if (m_bUseScrollUp)			lineStart = m_iScrollUp;
	else						lineStart = 0;
	for (int i=lineStart, ii=0; ii<BANK_GRID_LINE_COUNT; i++, ii++)
		for (int j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			item = pBagUp->GetItem(i, j);
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsUp[ii][j]->ModifyFlags(wfDragOut|wfDragServer|wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					//m_btnGridsUp[ii][j]->SetBackgroundImage(new RtwImage(pItemClass->icon));// change [3/17/2009 tooth.shi]
					m_btnGridsUp[ii][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
				}

				if (ItemCanPile(item))
				{
					rt2_sprintf(szTmp, "%d", item.count);
					m_btnGridsUp[ii][j]->SetText(szTmp);
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
			}
			else
			{
				m_btnGridsUp[ii][j]->ModifyFlags(0, wfDragOut|wfHint | wfDragServer);
				m_btnGridsUp[ii][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsUp[ii][j]->SetText("");

                RtwComplexImage hoverImg;
                m_btnGridsUp[ii][j]->SetHoverImage(hoverImg);
			}
		}

	// 金钱
	rt2_snprintf(szTmp, 64, "%d", m_bagUp.GetMoney());
    //szTmp[63] = 0;
	m_lbMoneyUp->SetText(szTmp);
	rt2_snprintf(szTmp, 64, "%d", m_bagDown.GetMoney());
    //szTmp[63] = 0;
	m_lbMoneyDown->SetText(szTmp);

    // 个数
    if (m_bStore)
    {
		m_lbSize->SetText("");
		m_lbSizeInfo->Hide();
        m_lbSize->Hide();
    }
    else 
    {
		m_BankItemCount = m_bagUp.Count();
		m_BankSize = m_bagUp.GetPageCount() * m_bagUp.GetLineCount() * m_bagUp.GetColCount();
        m_lbSize->Show();
        m_lbSizeInfo->Show();
        rt2_snprintf(szTmp, 64, "%d/%d", m_BankItemCount, m_BankSize);
        //szTmp[63] = 0;
        m_lbSize->SetText(szTmp);
    }

	m_btnOK->Enable();

	unguard;
}

void UIFormBank::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormBank::Show(bool bStore, DWORD npcID)
{
	guard;

	GcActor* pPlayer = GetWorld()->m_pPlayer;
	if ( IsVisible() )		return;
	UnlockOperate();

	if (g_layerMain)
	{
		if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
			g_layerMain->m_fromPlayerItems->Hide();
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
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		//if (g_layerMain->m_formBank->IsVisible())			// 仓库
		//	g_layerMain->m_formBank->Hide();
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

	m_bStore = bStore;
	m_npcID = npcID;

	m_bagDown.Clear();
	m_bagUp.Clear();

	if (bStore)
	{
        m_bagUp.Resize(1, ITEM_PLAYER_INVENTORY_LINE*ITEM_PLAYER_INVENTORY_PAGE, BANK_GRID_COLUMN_COUNT);
		pPlayer->mItem.m_Bag.AddToBag(&m_bagUp, ITEM_MASK_CAN_TRADE);
//		pPlayer->mItem.m_MissionBag.AddToBag(&m_bagUp);
		m_bagUp.SetMoney(pPlayer->mItem.GetMoney());
	}
	else
	{
		CItemNetHelper::NetSend_c2r_bank_query(m_npcID, "");
	}

	if (bStore)
	{
		m_lbNameUp->SetText(R(MSG_ITEM_CAN_STORE));
		m_lbNameDown->SetText(R(MSG_ITEM_TO_STORE));
		m_btnOK->SetText(R(MSG_ITEM_CUNCHU));
		m_btnMoney->SetText(R(MSG_ITEM_CUNQIAN));
		m_lbMoneyNameUp->SetText(R(MSG_ITEM_XIANJIN));
		m_lbMoneyNameDown->SetText(R(MSG_ITEM_CUNJIN));
	}
	else
	{
		m_lbNameUp->SetText(R(MSG_ITEM_BANKITEM));
		m_lbNameDown->SetText(R(MSG_ITEM_TO_WITHDRAW));
		m_btnOK->SetText(R(MSG_ITEM_QUHUI));
		m_btnMoney->SetText(R(MSG_ITEM_QUQIAN));
		m_lbMoneyNameUp->SetText(R(MSG_ITEM_BANK));
		m_lbMoneyNameDown->SetText(R(MSG_ITEM_QUJIN));
	}

	// 滚动条
	SetScrollBar();

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormBank::IsVisible()
{
	guard;
	if ( m_pFrmThis && (m_pFrmThis->GetFlags(wfVisible) ) )	return true;
	else	return false;
	unguard;
}

void UIFormBank::Hide()
{
	guard;
	if (m_pFrmThis)
	{
		m_bLockOperate = false;
		m_pFrmThis->Hide();
	}
	unguard;
}

// 是不是下面的格子
bool UIFormBank::Is_Widget_Down(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<BANK_GRID_LINE_COUNT; i++)
		for (j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			string idDown = m_btnGridsDown[i][j]->GetID();
			if ( idDown==id )
				goto Next;
		}
Next:
		if (i<BANK_GRID_LINE_COUNT && j<BANK_GRID_COLUMN_COUNT)
		{
			outI = i + m_iScrollDown;
			outJ = j;
			return true;
		}
		else	return false;

	unguard;
}

bool UIFormBank::Is_Widget_Up(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<BANK_GRID_LINE_COUNT; i++)
		for (j=0; j<BANK_GRID_COLUMN_COUNT; j++)
		{
			const string& idUp = m_btnGridsUp[i][j]->GetID();
			if ( idUp==id )
				goto Next;
		}
Next:
		if (i<BANK_GRID_LINE_COUNT && j<BANK_GRID_COLUMN_COUNT)
		{
			outI = i + m_iScrollUp;
			outJ = j;
			return true;
		}
		else	return false;

	unguard;
}

void UIFormBank::OnGetHintText_Up(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pBag = &m_bagUp;

	int i, j;
	Is_Widget_Up(widgetID, i, j);
	SItemID itemID = pBag->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, pBag, 0, i, j);
	g_workspace.SetHintText((const char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormBank::OnGetHintText_Down(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CBag *pBag = &m_bagDown;

	int i, j;
	Is_Widget_Down(widgetID, i, j);
	SItemID itemID = pBag->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, pBag, 0, i, j);

	g_workspace.SetHintText((char*)str, true);
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormBank::OnDrag_Down(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr)	return;

	const string& widgetID_s = pDragSvr->GetID();

	if (m_bagDown.IsFull())
		return;

	int i,j;
	int tmpPage, tmpI, tmpJ;
	if ( Is_Widget_Up(widgetID_s, i, j) )
	{
		SItemID item = m_bagUp.GetItem(0, i, j);
		m_bagUp.RemoveItem(0, i, j);
		m_bagDown.AddItem(item, &tmpPage, &tmpI, &tmpJ, false);

		g_workspace.ResetHintText();
		Refresh();
	}

	unguard;
}

void UIFormBank::OnDrag_Up(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr)	return;

	const string widgetID_s = pDragSvr->GetID();

	int i,j;
	int tmpPage, tmpI, tmpJ;
	if ( Is_Widget_Down(widgetID_s, i, j) )
	{
		SItemID item = m_bagDown.GetItem(0, i, j);
		m_bagDown.RemoveItem(0, i, j);
		m_bagUp.AddItem(item, &tmpPage, &tmpI, &tmpJ, false);

		g_workspace.ResetHintText();
		Refresh();
	}

	unguard;
}

void UIFormBank::OnLDClick_Up(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	if (m_bagDown.IsFull())
		return;

	int i,j;
	int tmpPage, tmpI, tmpJ;
	Is_Widget_Up(sender->GetID(), i, j);
	SItemID item = m_bagUp.GetItem(0, i, j);
	m_bagUp.RemoveItem(0, i, j);
	m_bagDown.AddItem(item, &tmpPage, &tmpI, &tmpJ, false);

	g_workspace.ResetHintText();
	Refresh();

	unguard;
}

void UIFormBank::OnLDClick_Down(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	int i,j;
	int tmpPage, tmpI, tmpJ;
	Is_Widget_Down(sender->GetID(), i, j);
	SItemID item = m_bagDown.GetItem(0, i, j);
	m_bagDown.RemoveItem(0, i, j);
	m_bagUp.AddItem(item, &tmpPage, &tmpI, &tmpJ, false);

	g_workspace.ResetHintText();
	Refresh();

	unguard;
}

void UIFormBank::OnOK(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	if ( m_bagDown.IsEmpty() && m_bagDown.GetMoney()==0)
	{
		PlayerFailOperate(R(MSG_ITEM_SELECT_NONE));
		return;
	}

	vector<DWORD> itemIDs;
	m_bagDown.ToItemIDArray(itemIDs);
	for (int i=0; i<(int)itemIDs.size(); i++)
	{
		if (GetWorld()->m_pPlayer->mItem.CanItemTrade(itemIDs[i])==TR_CanNot_Pet)
		{
			PlayerFailOperate(R(MSG_STORE_PET_FIRST));
			return;
		}
	}

	if (m_bStore)
	{
		SDWORDArray idArray;
		m_bagDown.ToItemIDArray(idArray.array);
		CItemNetHelper::NetSend_c2r_bank_store(idArray, m_bagDown.GetMoney(), m_npcID);
		m_btnOK->Disable();
		LockOperate();
	}
	else
	{
		if (GetWorld()->m_pPlayer->mItem.m_Bag.FreeSpace()<m_bagDown.Count())
		{
			PlayerFailOperate(R(MSG_ITEM_WITHDRAW_FAIL_NOSPACE));
		}
		else
		{
			SDWORDArray idArray;
			m_bagDown.ToItemIDArray(idArray.array);
			CItemNetHelper::NetSend_c2r_bank_withdraw(idArray, m_bagDown.GetMoney(), m_npcID);
			m_btnOK->Disable();
			LockOperate();
		}
	}

	unguard;
}

void UIFormBank::OnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormBank::OnBtnMoney(RtwWidget* sender, void*)
{
	guard;

	if (m_bLockOperate)
		return;

	UIFormTextMsg *pFrm;
	if (m_bStore)
        pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_MONEY_COUNT), R(MSG_ITEM_STOREMONEY_COUNT));
	else
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_MONEY_COUNT), R(MSG_ITEM_WITHDRAWMONEY_COUNT));
		
	pFrm->SetNumberOnly(true);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormBank, OnMoney, pFrm);

	unguard;
}

void UIFormBank::OnMoney(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if (m_bLockOperate)
		return;

	if (!IsVisible())
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
	if ( money > m_bagUp.GetMoney() )
	{
		PlayerFailOperate(R(MSG_ITEM_NOT_ENOUGH_MONEY));
		return;
	}
	m_bagDown.SetMoney(money);
	Refresh();

	unguard;
}

void UIFormBank::OnScrollUp(RtwWidget* sender,void*)
{
	guard;
	if (m_bUseScrollUp)
	{
		m_iScrollUp = m_pScrollUp->GetValue();
		g_workspace.ResetHintText();
		Refresh();
	}
	unguard;
}

void UIFormBank::OnScrollDown(RtwWidget* sender,void*)
{
	guard;
	if (m_bUseScrollDown)
	{
		m_iScrollDown = m_pScrollDown->GetValue();
		g_workspace.ResetHintText();
		Refresh();
	}
	unguard;
}

void UIFormBank::OnDragEnd_Up(RtwWidget* sender, void*)
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

void UIFormBank::OnDragEnd_Down(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pClient = g_workspace.GetDragClient();
	if (pClient)
	{
		const string& widgetID_c = pClient->GetID();
		int i, j;
		if ( !Is_Widget_Up(widgetID_c, i, j) )
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}

void UIFormBank::SetScrollBar()
{
	guard;

	m_iScrollUp = m_iScrollDown = 0;
	m_pScrollUp->SetValue(0);
	m_pScrollDown->SetValue(0);

	int nLineMax = m_bagUp.GetVisibleMaxLine();
	if (nLineMax <= BANK_GRID_LINE_COUNT)
	{
		//		m_pScrollUp->Disable();
		m_iLineCountUp = m_iLineCountDown = BANK_GRID_LINE_COUNT;
		m_bUseScrollUp = m_bUseScrollDown = false;
		m_pScrollDown->Disable();
	}
	else
	{
		m_pScrollUp->Enable();
		m_iLineCountUp = m_iLineCountDown = nLineMax;
		m_bUseScrollUp = m_bUseScrollDown = true;
		m_pScrollUp->SetRange(0, nLineMax-BANK_GRID_LINE_COUNT);
		//m_pScrollDown->Enable();
		//m_pScrollDown->SetRange(0, nLineMax-BANK_GRID_LINE_COUNT);
	}

	m_pScrollDown->Enable();
	m_bUseScrollDown = true;
	m_iScrollDown = 0;
	m_iLineCountDown = GetWorld()->m_pItemManager->GetBankMaxLineCount();
	m_pScrollDown->SetRange(0, GetWorld()->m_pItemManager->GetBankMaxLineCount() - BANK_GRID_LINE_COUNT);

	unguard;
}

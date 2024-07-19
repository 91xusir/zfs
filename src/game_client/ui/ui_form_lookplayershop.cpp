#include "gc_include.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_playeritems.h"
#include "ui_form_textMsg.h"
#include "ui_form_msg.h"
#include "item_net_define.h"

CUIFormLookPlayerShop::CUIFormLookPlayerShop(void)
:m_PShop(GetWorld()->m_pItemManager)
{
	guard;
	m_pFrmThis				= RTW_WIDGET("fmbuyershop");
	m_pFrmGridsUp			= RTW_WIDGET("fmbuyershop.frmsell.tabsell.frmitem");
	m_pFrmGridsDown			= RTW_WIDGET("fmbuyershop.frmbuy.frmcontent");
	m_lbSellerName			= RTW_WIDGET_T(RtwLabel,"fmbuyershop.lbinput.lbpalyername2.lbplayername");
	m_tbxShopName			= RTW_WIDGET_T(RtwTextBox,"fmbuyershop.lbinput.lbshopname2.tbxshopname");

	m_tbxShopName->SetEnableInput(false);
	RTW_WIDGET("fmbuyershop.frmsell.tabsell.btnpet")->Disable();
	RTW_WIDGET("fmbuyershop.btnshopinfo")->Disable();

	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);

	m_ptrMaskImage = g_workspace.getImageFactory()->createImage("ui_texture/passive_mask.dds");
	m_ptrMaskImage->SetBlend(true);
	m_ptrMaskImage->getRenderImage()->GetImage()->m_Transparency = 100;
	m_ptrMaskImage->getRenderImage()->GetImage()->m_BackTransparency = 155;
	m_ptrMaskImage->getRenderImage()->GetImage()->eBlendDstFunc = RTGBLEND_SRCCOLOR;
	m_ptrMaskImage->getRenderImage()->GetImage()->eBlendSrcFunc = RTGBLEND_ZERO;

	char strId[32];
#define GRID_SIZE_X		36
#define GRID_SIZE_Y		36
#define GRID_ITEMSIZE_X		31
#define GRID_ITEMSIZE_Y		31
	RtwButton* widget = NULL;
	RtwImage* pImagegrids = g_workspace.getImageFactory()->createImage("ui/textures/gridsitem.tga");
	for (int i=0; i<PSHOP_GRID_LINE_COUNT; i++)
	{
		for (int j=0; j<PSHOP_GRID_COLUMN_COUNT; j++)
		{

			rt2_sprintf(strId, "CommuplbItem_%02d_%02d", i, j);
			m_lbGridsUp[i][j] = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
			m_lbGridsUp[i][j]->SetName(strId);
			m_pFrmGridsUp->AddChild(m_lbGridsUp[i][j]);
			m_lbGridsUp[i][j]->SetBorderColor(0x60c0c0aa);
			m_lbGridsUp[i][j]->SetBorderSize(1);
			m_lbGridsUp[i][j]->SetBackgroundImage(pImagegrids);
			RtwRect sRect;
			sRect.left = (GRID_SIZE_X+2)*j+2;
			sRect.top = (GRID_SIZE_Y+2)*i+2;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_lbGridsUp[i][j]->Move(sRect);

			rt2_sprintf(strId, "CommupItem_%02d_%02d", i, j);
			m_btnGridsUp[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnGridsUp[i][j]->SetName(strId);
			m_pFrmGridsUp->AddChild(m_btnGridsUp[i][j]);
			m_btnGridsUp[i][j]->SetHoverImage(m_ptrHoverImage);

			sRect.left = (GRID_SIZE_Y+2)*j+4;
			sRect.top = (GRID_SIZE_X+2)*i+4;
			sRect.right = sRect.left + GRID_ITEMSIZE_X;
			sRect.bottom = sRect.top + GRID_ITEMSIZE_Y;
			m_btnGridsUp[i][j]->Move(sRect);
			m_btnGridsUp[i][j]->ModifyFlag(wfDragIn|wfHint|wfDClick,0);

			m_btnGridsUp[i][j]->EvLClick			+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnLClick_SellItem);
			m_btnGridsUp[i][j]->EvLDClick			+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnLDClick_SellItem);
			m_btnGridsUp[i][j]->EvRClick			+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnRClick_SellButton);
			m_btnGridsUp[i][j]->EvHint				+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnGetHintText_SellItem);
			m_btnGridsUp[i][j]->EvDrag				+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnDrag_SellItem);
			m_btnGridsUp[i][j]->EvDragEnd			+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnDragEnd_SellItem);
		}
	}
	for (int i=0; i<PSHOP_GRID_LINE_COUNT_BUY; i++)
	{
		for (int j=0; j<PSHOP_GRID_COLUMN_COUNT_BUY; j++)
		{
			rt2_sprintf(strId, "CommdownlbItem_%02d_%02d", i, j);
			m_lbGridsDown[i][j] = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
			m_lbGridsDown[i][j]->SetName(strId);
			m_pFrmGridsDown->AddChild(m_lbGridsDown[i][j]);
			m_lbGridsDown[i][j]->SetBorderColor(0x60c0c0aa);
			m_lbGridsDown[i][j]->SetBorderSize(1);
			m_lbGridsDown[i][j]->SetBackgroundImage(pImagegrids);
			RtwRect sRect;
			sRect.left = (GRID_SIZE_X+2)*j+2;
			sRect.top = (GRID_SIZE_Y+2)*i+2;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_lbGridsDown[i][j]->Move(sRect);

			rt2_sprintf(strId, "CommdownItem_%02d_%02d", i, j);
			m_btnGridsDown[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnGridsDown[i][j]->SetName(strId);
			m_pFrmGridsDown->AddChild(m_btnGridsDown[i][j]);
			m_btnGridsDown[i][j]->SetHoverImage(m_ptrHoverImage);

			sRect.left = (GRID_SIZE_Y+2)*j+4;
			sRect.top = (GRID_SIZE_X+2)*i+4;
			sRect.right = sRect.left + GRID_ITEMSIZE_X;
			sRect.bottom = sRect.top + GRID_ITEMSIZE_Y;
			m_btnGridsDown[i][j]->Move(sRect);
			m_btnGridsDown[i][j]->ModifyFlag(wfDragIn|wfHint|wfDClick,0);

			m_btnGridsDown[i][j]->EvLClick		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnLClick_BuyItem);
			m_btnGridsDown[i][j]->EvLDClick		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnLDClick_BuyItem);
			m_btnGridsDown[i][j]->EvRClick		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnRClick_BuyButton);
			m_btnGridsDown[i][j]->EvHint		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnGetHintText_BuyItem);
			m_btnGridsDown[i][j]->EvDrag		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnDrag_BuyItem);
			m_btnGridsDown[i][j]->EvDragEnd		+= RTW_CALLBACK(this, CUIFormLookPlayerShop, OnDragEnd_BuyItem);
		}
	}
	m_nMaxDistance = 400;
	m_pFrmThis->Hide();
	m_iSelectI	= -1;
	m_iSelectJ	= -1;
	m_operateItem = ItemID_CreateInvalid();
	unguard;
}

CUIFormLookPlayerShop::~CUIFormLookPlayerShop(void)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::Show()
{
	guard;
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

void CUIFormLookPlayerShop::Hide()
{
	guard;
	m_pFrmThis->Hide();
	unguard;
}

void CUIFormLookPlayerShop::Refresh()
{
	guard;
	Show();
	unguard;
}

bool CUIFormLookPlayerShop::IsVisible()
{
	guard;
	return m_pFrmThis->IsVisible();
	unguard;
}

void CUIFormLookPlayerShop::Init()
{
	guard;
	m_PShop.Init(false, GetPlayer()->ID(), &(GetPlayer()->mItem) );
	unguard;
}

void CUIFormLookPlayerShop::SetShopSeller(string name)
{
	guard;
	m_lbSellerName->SetText(name);
	unguard;
}

void CUIFormLookPlayerShop::OnPreDraw()
{
	guard;
	m_tbxShopName->SetText(m_PShop.GetName());
	SItemID item;
	SItemBase *pItemClass = NULL;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);
	CBag *pBagDown, *pBagUp;
	pBagUp = m_PShop.GetSellItemBag();
	pBagDown = m_PShop.GetBuyItemBag();
	char szTmp[64];

	// 上面的物品栏
	for (int i=0; i<PSHOP_GRID_LINE_COUNT; i++)
		for (int j=0; j<PSHOP_GRID_COLUMN_COUNT; j++)
		{
			item = pBagUp->GetItem(i, j);
			char *szImage=NULL;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsUp[i][j]->ModifyFlags(wfDragServer|wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					//m_btnGridsUp[i][j]->SetBackgroundImage(new RtwImage(szImage));// change [3/17/2009 tooth.shi]
					m_btnGridsUp[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsUp[i][j]->SetText(szTmp);
						m_btnGridsUp[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsUp[i][j]->SetText("");
					}
				}
				m_btnGridsUp[i][j]->SetHoverImage(m_ptrHoverImage);
				m_btnGridsUp[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
				if(m_PShop.GetPrice(item.id) == -99)
				{
					m_btnGridsUp[i][j]->SetMaskColor(0xa0ff0000);
					m_btnGridsUp[i][j]->SetMaskValue(1.f);
				}else
				{
					m_btnGridsUp[i][j]->SetMaskColor(0x01000000);
					m_btnGridsUp[i][j]->SetMaskValue(0.f);
				}
			}
			else
			{
				m_btnGridsUp[i][j]->ModifyFlags(0, wfHint | wfDragServer);
				m_btnGridsUp[i][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsUp[i][j]->SetText("");
			}
		}

	// 下面的物品栏
	for (int i=0; i<PSHOP_GRID_LINE_COUNT_BUY; i++)
		for (int j=0; j<PSHOP_GRID_COLUMN_COUNT_BUY; j++)
		{
			item = pBagDown->GetItem(i, j);
			char *szImage;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsDown[i][j]->ModifyFlags(wfDragServer | wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					m_btnGridsDown[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsDown[i][j]->SetText(szTmp);
						m_btnGridsDown[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsDown[i][j]->SetText("");
					}
				}
			}
			else
			{
				m_btnGridsDown[i][j]->ModifyFlags(0, wfDragServer|wfHint);
				m_btnGridsDown[i][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsDown[i][j]->SetText("");
			}
		}
	unguard;
}
bool CUIFormLookPlayerShop::Is_Widget_Up(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<PSHOP_GRID_LINE_COUNT; i++)
		for (j=0; j<PSHOP_GRID_COLUMN_COUNT; j++)
		{
			const string& idUp = m_btnGridsUp[i][j]->GetID();
			if ( idUp==id )
				goto Next;
		}
Next:
		if (i<PSHOP_GRID_LINE_COUNT && j<PSHOP_GRID_COLUMN_COUNT)
		{
			outI = i;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

// 是不是下面的格子
bool CUIFormLookPlayerShop::Is_Widget_Down(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<PSHOP_GRID_LINE_COUNT_BUY; i++)
		for (j=0; j<PSHOP_GRID_COLUMN_COUNT_BUY; j++)
		{
			const string& idDown = m_btnGridsDown[i][j]->GetID();
			if ( idDown==id )
				goto Next;
		}
Next:
		if (i<PSHOP_GRID_LINE_COUNT_BUY && j<PSHOP_GRID_COLUMN_COUNT_BUY)
		{
			outI = i;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

void CUIFormLookPlayerShop::OnSetNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	CUIFormInputBox* pInputFrm=(CUIFormInputBox* )vpEvent->GetUser1();
	int num = pInputFrm->GetNumFromInputBox();
	if(num== 0)
		return;
	m_operateItem.count = num;

	UIFormTextMsg *pFrm;
	pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
	pFrm->SetNumberOnly(true);
	pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormLookPlayerShop, OnEnsurePrice, pFrm);
	pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormLookPlayerShop,OnCancelOperate,pFrm);
	unguard;
}

void CUIFormLookPlayerShop::OnEnsurePrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	int num = 1;
	if(vpEvent)
	{
		CUIFormInputBox* pInputFrm=(CUIFormInputBox* )vpEvent->GetUser1();
		num = pInputFrm->GetNumFromInputBox();
		m_operateItem.count = num;
	}

	char content[1024];
	SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(m_operateItem.type);
	if(!m_bOperateSell)
	{
		rt2_sprintf(content,R(MSG_TRADE_CONFIRMBUY),m_PShop.GetPrice(m_operateItem.id)*num,num,pItemClass->name);
	}else
	{
		rt2_sprintf(content,R(MSG_TRADE_CONFIRMSELL),m_PShop.GetPrice(m_operateItem.id)*num,num,pItemClass->name);
	}
	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(content, UIFormMsg::TYPE_OK_CANCEL,true,"EnsureBuy");
	pConfirm->EvOK		= RTW_CALLBACK_1(this,CUIFormLookPlayerShop,OnEnsureDoAction,pConfirm);
 	pConfirm->EvCancel	= RTW_CALLBACK_1(this,CUIFormLookPlayerShop,OnCancelOperate,pConfirm);
	unguard;
}

void CUIFormLookPlayerShop::OnEnsureDoAction(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(!m_bOperateSell)
	{
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_item);
		pPacket->WriteShort(c2r_pshop_buy);
		pPacket->WriteLong(m_PShop.GetPlayerID());	
		pPacket->WriteLong(m_operateItem.id);
		pPacket->WriteLong(m_PShop.GetPrice(m_operateItem.id));
		if(ItemCanPile(m_operateItem))
			pPacket->WriteShort((short)(m_operateItem.count));
		else
			pPacket->WriteShort(0);
		NetSend(pPacket);
	}else
	{
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_item);
		pPacket->WriteShort(c2r_pshop_sell);
		pPacket->WriteLong(m_PShop.GetPlayerID());
		pPacket->WriteLong(m_selfItem.id);
		pPacket->WriteLong(m_operateItem.id);
		pPacket->WriteLong(m_PShop.GetPrice(m_operateItem.id));
		if(ItemCanPile(m_operateItem))
			pPacket->WriteShort((short)(m_operateItem.count));
		else
			pPacket->WriteShort(0);
		NetSend(pPacket);
	}
	unguard;
}

void CUIFormLookPlayerShop::OnCancelOperate(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	m_operateItem = ItemID_CreateInvalid();
	m_selfItem = ItemID_CreateInvalid();
	unguard;
}

void CUIFormLookPlayerShop::OnLClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnLDClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnRClick_SellButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	const string& widgetID = vpSender->GetID();
	int i,j;
	Is_Widget_Up(widgetID, i, j);
	m_operateItem = m_PShop.GetSellItemBag()->GetItem(i, j);
	if ( !ItemID_IsValid(m_operateItem) )
		return;
	if ((int)(m_PShop.GetPrice(m_operateItem.id))== -99)
	{
		ShowSystemMessage(R(MSG_PSHOP_NOTSELL));
		return;
	}
	if(GetPlayer()->mItem.m_Bag.GetMoney()< m_PShop.GetPrice(m_operateItem.id))
	{
		ShowSystemMessage(R(MSG_BAITAN_HAVENOTENOUGHMONEY));
		return;
	}
	if(GetPlayer()->mItem.m_Bag.IsFull())
	{
		ShowSystemMessage(R(MSG_BAITAN_HAVENOSPACE));
		return;
	}
	m_bOperateSell = false;
	OnMouseRClickEvent(m_operateItem);
	unguard;
}

void CUIFormLookPlayerShop::OnGetHintText_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	const string& widgetID = vpSender->GetID();

	int i, j;
	Is_Widget_Up(widgetID, i, j);
	SItemID itemID = m_PShop.GetSellItemBag()->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		vpSender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	UINT price = m_PShop.GetPrice(itemID.id);
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, m_PShop.GetSellItemBag(), 0, i, j, -1.0f, price);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

void CUIFormLookPlayerShop::OnDrag_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnDragEnd_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	RtwWidget* pClient = g_workspace.getMouseHover();
	if(!pClient)
		return;
	const string& widgetID = pClient->GetID();
	int i, j;
	if(g_layerMain->m_fromPlayerItems->Is_Widget_Common(widgetID, i, j))
	{
		OnRClick_SellButton(vpSender,NULL);
	}
	unguard;
}

void CUIFormLookPlayerShop::OnLClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnLDClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnRClick_BuyButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	const string& widgetID = vpSender->GetID();
	int i,j;
	Is_Widget_Down(widgetID, i, j);
	m_operateItem = m_PShop.GetBuyItemBag()->GetItem(i, j);
	if ( !ItemID_IsValid(m_operateItem) )
		return;
	m_bOperateSell = true;
	OnMouseRClickEvent(m_operateItem);
	unguard;
}

void CUIFormLookPlayerShop::OnGetHintText_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	const string& widgetID = vpSender->GetID();

	int i, j;
	Is_Widget_Down(widgetID, i, j);
	SItemID itemID = m_PShop.GetBuyItemBag()->GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		vpSender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	UINT price = m_PShop.GetPrice(itemID.id);
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, m_PShop.GetSellItemBag(), 0, i, j, -1.0f, price);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

void CUIFormLookPlayerShop::OnDrag_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormLookPlayerShop::OnDragEnd_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}
void CUIFormLookPlayerShop::OnMouseRClickEvent(SItemID item)
{
	guard;
	if (!ItemID_IsValid(item))
		return;

	m_operateItem = item;
	if(m_bOperateSell)
	{
		int num = 0;
		g_layerMain->m_fromPlayerItems->OnGetItemBaseNum(m_selfItem,num,m_operateItem);
		if(num== 0)
		{
			ShowSystemMessage(R(MSG_ITEM_YOUDONTHAVETHIS));
			return;
		}
		if(num < item.count)
			m_operateItem.count = num;
	}
	if(ItemCanPile(m_operateItem))
	{
		string title;
		if(m_bOperateSell)
			title= R(MSG_TRADE_ENTERNUMBER);
		else
			title= R(MSG_TRADE_ENTERBUYNUMBER);
		CUIFormInputBox* pInputFrm;
		pInputFrm = CUIFormInputBox::ShowStatic("",title.c_str());	
		pInputFrm->SetMaxNum(m_operateItem.count);
		pInputFrm->OnMoveToXY(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		pInputFrm->EvOK = RTW_CALLBACK_1(this,CUIFormLookPlayerShop,OnEnsurePrice,pInputFrm);
		pInputFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormLookPlayerShop,OnCancelOperate,pInputFrm);
	}else
	{
		OnEnsurePrice(NULL,NULL);
	}
	unguard;
}

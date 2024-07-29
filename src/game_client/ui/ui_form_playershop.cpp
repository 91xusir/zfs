#include "gc_include.h"
#include "ui_form_playershop.h"
#include "ui_form_playeritems.h"
#include "ui_form_textMsg.h"
#include "UIForm_Char.h"


CUIFormPlayerShop::CUIFormPlayerShop()
:m_PShop(GetWorld()->m_pItemManager),
m_itemListBag(BUY_ITEM_LINE_COUNT,BUY_ITEM_COLUMN_COUNT,1,GetWorld()->m_pItemManager)
{
	guard;

	m_pFrmThis				= LOAD_UI("fmplayershop");
	m_pFrmGridsUp			= LOAD_UI("fmplayershop.frmsell.tabsell.frmitem");
	m_pFrmGridsDown			= LOAD_UI("fmplayershop.frmbuy.frmcontent");
	m_btnStopSellOne		= LOAD_UI_T(RtwButton,"fmplayershop.fmbtn3.btnunshowitem");
	m_lbSellerName			= LOAD_UI_T(RtwLabel,"fmplayershop.lbinput.lbpalyername2.lbplayername");
	m_tbxShopName			= LOAD_UI_T(RtwTextBox,"fmplayershop.lbinput.tbxshopname");
	m_tbxTotalMoney			= LOAD_UI_T(RtwTextBox,"fmplayershop.fmtotalprice.lbprice2.tbxshopname");
	m_btnChangeName			= LOAD_UI_T(RtwButton,"fmplayershop.lbinput.btnStart1");
	m_btnStart				= LOAD_UI_T(RtwButton,"fmplayershop.fmbtn3.btnstart");
	m_btnStop				= LOAD_UI_T(RtwButton,"fmplayershop.fmbtn3.btncloseshop");
	m_btnShopChat			= LOAD_UI_T(RtwButton,"fmplayershop.btnshopinfo");

	m_pFrmBuyItemList		= LOAD_UI("fmpshoplist");
	m_pFrmGridsBuyItem		= LOAD_UI("fmpshoplist.fmitem");
	m_cbxComboBoxBuyItem	= LOAD_UI_T(RtwComboBox,"fmpshoplist.fmlist.cbxlist");
	m_btnOK					= LOAD_UI_T(RtwButton,"fmpshoplist.btnok");
	m_btnCancel				= LOAD_UI_T(RtwButton,"fmpshoplist.btnback");

	m_btnShopChat->Disable();
	LOAD_UI("fmplayershop.frmsell.tabsell.btnpet")->Disable();
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

	m_pFrmThis->EvHide				+= RTW_CALLBACK(this,CUIFormPlayerShop,OnHideForm);
	m_pFrmThis->EvFocus				+= RTW_CALLBACK(this,CUIFormPlayerShop,OnGetFocus);
	m_btnStopSellOne->EvLClick		+= RTW_CALLBACK(this,CUIFormPlayerShop,OnLClickBtnStopSellOneLClick);
	m_btnChangeName->EvLClick		+= RTW_CALLBACK(this,CUIFormPlayerShop,OnLClickBtnChangeName);
	m_tbxShopName->EvInputChar		+= RTW_CALLBACK(this,CUIFormPlayerShop,OnShopNameChange);
	m_cbxComboBoxBuyItem->EvUpdateText	+= RTW_CALLBACK(this,CUIFormPlayerShop,OnSelectComboBox);
	m_btnOK->EvLClick				+= RTW_CALLBACK(this,CUIFormPlayerShop,OnLClickBtnOK);
	m_btnCancel->EvLClick			+= RTW_CALLBACK(this,CUIFormPlayerShop,OnLClickBtnCancelItemList);
	m_btnStart->EvLClick			+= RTW_CALLBACK(this,CUIFormPlayerShop,OnStartShop);
	m_btnStop->EvLClick				+= RTW_CALLBACK(this,CUIFormPlayerShop,OnStopShop);

	m_tbxShopName->SetCapacity(20);
	m_tbxTotalMoney->SetEnableInput(false);
	char strId[32];
#define GRID_SIZE_X		36
#define GRID_SIZE_Y		36
#define GRID_ITEMSIZE_X		31
#define GRID_ITEMSIZE_Y		31
	RtwButton* widget = NULL;
	RtwImage* pImagegrids = g_workspace.getImageFactory()->createImage("ui/textures/gridsitem.tga");
	for (int i=0; i<PSHOP_GRID_LINE_COUNT; i++)
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

			m_btnGridsUp[i][j]->EvLClick			+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLClick_SellItem);
			m_btnGridsUp[i][j]->EvLDClick			+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLDClick_SellItem);
			m_btnGridsUp[i][j]->EvRClick			+= RTW_CALLBACK(this, CUIFormPlayerShop, OnRClick_SellButton);
			m_btnGridsUp[i][j]->EvHint				+= RTW_CALLBACK(this, CUIFormPlayerShop, OnGetHintText_SellItem);
			m_btnGridsUp[i][j]->EvDrag				+= RTW_CALLBACK(this, CUIFormPlayerShop, OnDrag_SellItem);
			m_btnGridsUp[i][j]->EvDragEnd			+= RTW_CALLBACK(this, CUIFormPlayerShop, OnDragEnd_SellItem);
		}
	for (int i=0; i<PSHOP_GRID_LINE_COUNT_BUY; i++)
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

			m_btnGridsDown[i][j]->EvLClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLClick_BuyItem);
			m_btnGridsDown[i][j]->EvLDClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLDClick_BuyItem);
			m_btnGridsDown[i][j]->EvRClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnRClick_BuyButton);
			m_btnGridsDown[i][j]->EvHint		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnGetHintText_BuyItem);
			m_btnGridsDown[i][j]->EvDrag		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnDrag_BuyItem);
			m_btnGridsDown[i][j]->EvDragEnd		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnDragEnd_BuyItem);
		}

	// 收购物品材料
	for (int i=0; i<BUY_ITEM_LINE_COUNT; i++)
		for (int j=0; j<BUY_ITEM_COLUMN_COUNT; j++)
		{
			rt2_sprintf(strId, "BuylbItem_%02d_%02d", i, j);
			m_lbGridsBuy[i][j] = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
			m_lbGridsBuy[i][j]->SetName(strId);
			m_pFrmGridsBuyItem->AddChild(m_lbGridsBuy[i][j]);
			m_lbGridsBuy[i][j]->SetBorderColor(0x60c0c0aa);
			m_lbGridsBuy[i][j]->SetBorderSize(1);
			m_lbGridsBuy[i][j]->SetBackgroundImage(pImagegrids);
			RtwRect sRect;
			sRect.left = (GRID_SIZE_X+2)*j+2;
			sRect.top = (GRID_SIZE_Y+2)*i+2;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_lbGridsBuy[i][j]->Move(sRect);

			rt2_sprintf(strId, "BuyItem_%02d_%02d", i, j);
			m_btnGridsBuy[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnGridsBuy[i][j]->SetName(strId);
			m_pFrmGridsBuyItem->AddChild(m_btnGridsBuy[i][j]);
			m_btnGridsBuy[i][j]->SetHoverImage(m_ptrHoverImage);

			sRect.left = (GRID_SIZE_Y+2)*j+4;
			sRect.top = (GRID_SIZE_X+2)*i+4;
			sRect.right = sRect.left + GRID_ITEMSIZE_X;
			sRect.bottom = sRect.top + GRID_ITEMSIZE_Y;
			m_btnGridsBuy[i][j]->Move(sRect);
			m_btnGridsBuy[i][j]->ModifyFlag(wfDragIn|wfHint|wfDClick,0);

			m_btnGridsBuy[i][j]->EvLClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLClick_BuyListItem);
			m_btnGridsBuy[i][j]->EvLDClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnLDClick_BuyListItem);
			m_btnGridsBuy[i][j]->EvRClick		+= RTW_CALLBACK(this, CUIFormPlayerShop, OnRClick_BuyListButton);
			m_btnGridsBuy[i][j]->EvHint			+= RTW_CALLBACK(this, CUIFormPlayerShop, OnGetHintText_BuyListItem);
		}
	InitFromCSV();
	m_bLockOperate = false;

	m_pFrmThis->Hide();
	m_iModelID = -1;
	m_iHeadModelID = -1;
	m_iSelectI	= -1;
	m_iSelectJ	= -1;
	m_iSelectAtItemListI = -1;
	m_iSelectAtItemListJ = -1;
	m_operateItem = ItemID_CreateInvalid();
	m_bAddSellItemFormDrag = false;
	m_iAllBuyItemPrice= 0;

	unguard;
}

void CUIFormPlayerShop::InitFromCSV()
{
	guard;
	// 加载表格中可以收购的物品
	RtCsv csv;
	if ( !csv.LoadTableFile(R(RES_TABLE_ITEMCANBUY)) )
	{
		ShowSystemMessage(R(MSG_TABLE_LOADSHOPPINGLISTFAIL));
		return;
	}
	int nLineCount = csv.GetLineCount();

	int i = csv.FindLineByString("ID");
	if(i<0) return;
	csv.SetTitleLine(i);

	string type;
	string content;
	bool canInsert;
	int j= 0;
	m_cbxComboBoxBuyItem->RemoveAllItems();
	for (i=1; i<nLineCount; i++)
	{
		type = csv[i]["类型"].Str();
		content = csv[i]["itemID"].Str();
		canInsert = (bool)(csv[i]["是否启用"].Int());
		if(canInsert)
		{
			m_cbxComboBoxBuyItem->AddItem(type);
			m_vectorSItemBase.push_back(RT_NEW vector<SItemID>);
			j++;
			if (!content.empty())
			{
				char *pStrItems = RT_NEW char[content.size() +1];
				rt2_strncpy(pStrItems, content.c_str(), content.size() +1);

				SItemID item;
				const char sep[] = ",;";	// 分隔符
				char *k = strtok(pStrItems, sep);
				while (k)
				{
					int n = atoi(k);
					item = GetWorld()->m_pItemManager->CreateItem(n);
					m_vectorSItemBase[j-1]->push_back(item);

					k = strtok(NULL, sep);
				}
				DEL_ARRAY(pStrItems);
			}
		}
	}
	unguard;
}

CUIFormPlayerShop::~CUIFormPlayerShop()
{
	guard;
	for (int i = 0; i < m_vectorSItemBase.size(); i++)
		DEL_ONE(m_vectorSItemBase[i]);
	unguard;
}

void CUIFormPlayerShop::Show()
{
	guard;
	char str[100];
	rt2_snprintf(str,100,"%d",m_iAllBuyItemPrice);
	m_tbxTotalMoney->SetText(str);
	if(m_iAllBuyItemPrice> GetPlayer()->mItem.m_Bag.GetMoney())
		m_tbxTotalMoney->SetTextColor(0xff0000);
	else
		m_tbxTotalMoney->SetTextColor(0xffffff);
	m_pFrmThis->Show();
	g_workspace.SetFocusWidget(m_pFrmThis);
	unguard;
}

void CUIFormPlayerShop::Init()
{
	guard;
	if ( !m_PShop.Init(true, GetPlayer()->ID(), &(GetPlayer()->mItem), GetPlayer()->GetName() ) )
		return;
	m_lbSellerName->SetText(GetPlayer()->GetName());
	m_tbxShopName->SetText(m_PShop.GetName());
	if(GetPlayer()->m_bPShop || m_bLockOperate)
	{
		m_btnStart->Disable();
		m_btnStop->Enable();
	}
	else
	{
		m_btnStart->Enable();
		m_btnStop->Disable();
	}
	unguard;
}

void CUIFormPlayerShop::OnStartInitShop()
{
	guard;
	RECT winRect;
	::GetClientRect((HWND)GetDevice()->GetWndHandle(),&winRect);
	RtwRect rect1 = g_layerMain->m_formPlayerShop->OnGetFormRect();
	RtwRect rect2 = g_layerMain->m_fromPlayerItems->OnGetFormRect();

	int x = (winRect.right - winRect.left - rect1.getWidth()-rect2.getWidth())*0.5 +winRect.left;
	int y = (winRect.bottom - winRect.top - rect1.getHeight())*0.5 +winRect.top;
	g_layerMain->m_formPlayerShop->SetPos(x, y);
	g_layerMain->m_fromPlayerItems->SetPos(x+rect1.getWidth(), y);
	g_layerMain->m_fromPlayerItems->Show();

	Init();
	OnPreDraw();
	Show();
	unguard;
}

bool CUIFormPlayerShop::IsVisible()
{
	guard;
	return m_pFrmThis->IsVisible();
	unguard;
}

void CUIFormPlayerShop::Refresh()
{
	guard;
	if(m_pFrmThis->IsVisible())
	{
		OnPreDraw();
		Show();
	}
	unguard;
}

void CUIFormPlayerShop::OnPreDraw()
{
	guard;
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

void CUIFormPlayerShop::OnLClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& WidgetID = vpSender->GetID();
	int i,j;
	Is_Widget_Up(WidgetID, i, j);
	CBag *pBagUp;
	pBagUp = m_PShop.GetSellItemBag();
	m_operateItem = pBagUp->GetItem(0,i,j);
	/*if (!ItemID_IsValid(m_operateItem))
		return;*/
	if(vpSender != m_btnGridsUp[m_iSelectI][m_iSelectJ])
		((RtwButton*)vpSender)->SetMaskImage(m_ptrMaskImage);
	else
	{
		((RtwButton*)vpSender)->SetMaskImage((RtwImage*)NULL);
		m_iSelectI = -1;
		m_iSelectJ = -1;
		return;
	}

	if(m_iSelectI != -1 && m_iSelectJ!= -1 && vpSender != m_btnGridsUp[m_iSelectI][m_iSelectJ])
	{
		m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskImage((RtwImage*)NULL);
	}
	m_iSelectI = i;
	m_iSelectJ = j;
	if((int)(m_PShop.GetPrice(m_operateItem.id))== -99)
		m_btnStopSellOne->SetText(R(MSG_TRADE_UPSHELF));
	else
		m_btnStopSellOne->SetText(R(MSG_TRADE_OFFSHELF));
	unguard;
}

void CUIFormPlayerShop::OnLDClick_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& WidgetID = vpSender->GetID();
	int i,j;
	Is_Widget_Up(WidgetID, i, j);
	CBag *pBagDown;
	pBagDown = m_PShop.GetSellItemBag();
	m_operateItem = pBagDown->GetItem(0,i,j);
	if (!ItemID_IsValid(m_operateItem))
		return;
	
	m_iSelectI= i;
	m_iSelectJ= j;
	m_bOperateSell = true;
	UIFormTextMsg *pFrm;
	if(ItemCanPile(m_operateItem))
	{
		CUIFormInputBox* pInputFrm;
		pInputFrm = CUIFormInputBox::ShowStatic("",R(MSG_TRADE_ENTERNUMBER));
		pInputFrm->SetMaxNum(m_operateItem.count);
		pInputFrm->OnMoveToXY(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		pInputFrm->EvOK = RTW_CALLBACK_1(this,CUIFormPlayerShop,OnChangeNum,pInputFrm);
		pInputFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pInputFrm);
	}else
	{
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
		pFrm->SetMaxNum(100000000);
		pFrm->SetNumberOnly(true);
		pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnChangePrice, pFrm);
		pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	}
	unguard;
}

void CUIFormPlayerShop::OnRClick_SellButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& WidgetID = vpSender->GetID();
	int i,j;
	SItemID item;
	Is_Widget_Up(WidgetID, i, j);
	CBag *pBagUp;
	pBagUp = m_PShop.GetSellItemBag();
	pBagUp->RemoveItem(0, i, j);
	Refresh();
	if(vpSender == m_btnGridsUp[m_iSelectI][m_iSelectJ])
	{
		((RtwButton*)vpSender)->SetMaskImage((RtwImage*)NULL);
		((RtwButton*)vpSender)->SetMaskColor(0x01000000);
		((RtwButton*)vpSender)->SetMaskValue(1.f);
		m_iSelectI = -1;
		m_iSelectJ = -1;
	}
	unguard;
}

void CUIFormPlayerShop::OnGetHintText_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
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

void CUIFormPlayerShop::OnDrag_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	RtwWidget* pDragSvr = g_workspace.GetDragServer();

	if (!pDragSvr)	return;
	//const string& widgetID_c = vpSender->GetID();
	const string& widgetID_s = pDragSvr->GetID();
	int i, j/*,iTo,jTo*/;
	CBag* pBagFrom/*,pBagTo*/;
	/*Is_Widget_Up(widgetID_c, iTo, jTo);*/
	g_layerMain->m_fromPlayerItems->Is_Widget_Common(widgetID_s, i, j);
	pBagFrom = &(GetPlayer()->mItem.m_Bag);
	/*pBagTo = m_PShop.GetSellItemBag();*/

	OnLClick_SellItem(vpSender,NULL);
	m_bAddSellItemFormDrag = true;
	m_operateItem = pBagFrom->GetItem(g_layerMain->m_fromPlayerItems->OnGetCorrectPage(),i,j);
	OnMouseRClickEvent(m_operateItem);
	unguard;
}

void CUIFormPlayerShop::OnDragEnd_SellItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormPlayerShop::OnLClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	if(m_pFrmBuyItemList->IsVisible())
		return;
	else
	{
		const string& widgetID = vpSender->GetID();
		int i, j;
		Is_Widget_Down(widgetID, i, j);
		SItemID itemID = m_PShop.GetBuyItemBag()->GetItem(i, j);
		if ( !ItemID_IsValid(itemID) )
		{
			OnShowItemList();
			m_iSelectI = i;
			m_iSelectJ = j;
			return;
		}
	}
	unguard;
}

void CUIFormPlayerShop::OnLDClick_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& WidgetID = vpSender->GetID();
	int i,j;
	Is_Widget_Down(WidgetID, i, j);
	CBag *pBagDown;
	pBagDown = m_PShop.GetBuyItemBag();
	m_operateItem = pBagDown->GetItem(0,i,j);
	if (!ItemID_IsValid(m_operateItem))
		return;

	m_bOperateSell = false;
	m_iSelectI= i;
	m_iSelectJ= j;
	UIFormTextMsg *pFrm;
	if(ItemCanPile(m_operateItem))
	{
		CUIFormInputBox* pInputFrm;
		pInputFrm = CUIFormInputBox::ShowStatic("",R(MSG_TRADE_ENTERNUMBER));
		pInputFrm->SetMaxNum(m_operateItem.count);
		pInputFrm->OnMoveToXY(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		pInputFrm->EvOK = RTW_CALLBACK_1(this,CUIFormPlayerShop,OnChangeNum,pInputFrm);
		pInputFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pInputFrm);
	}else
	{
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
		pFrm->SetNumberOnly(true);
		pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnChangePrice, pFrm);
		pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	}
	unguard;
}

void CUIFormPlayerShop::OnRClick_BuyButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	if(m_pFrmBuyItemList->IsVisible())
		return;
	else
	{
		const string& widgetID = vpSender->GetID();
		int i, j;
		Is_Widget_Down(widgetID, i, j);
		SItemID itemID = m_PShop.GetBuyItemBag()->GetItem(i, j);
		if ( !ItemID_IsValid(itemID) )
		{
			OnShowItemList();
			m_iSelectI = i;
			m_iSelectJ = j;
			return;
		}else
		{
			SItemID item = m_PShop.GetBuyItemBag()->GetItem(0,i,j);
			m_iAllBuyItemPrice-= m_PShop.GetPrice(item.id)*item.count;
			m_PShop.GetBuyItemBag()->RemoveItem(0, i, j);
			Refresh();
		}
	}
	unguard;
}

void CUIFormPlayerShop::OnGetHintText_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
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

void CUIFormPlayerShop::OnDrag_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormPlayerShop::OnDragEnd_BuyItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormPlayerShop::OnLClick_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& widgetID = vpSender->GetID();
	int i, j;
	Is_Widget_ItemList(widgetID, i, j);
	SItemID itemID = m_itemListBag.GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		return;
	}

	if(vpSender != m_btnGridsBuy[m_iSelectAtItemListI][m_iSelectAtItemListJ])
		((RtwButton*)vpSender)->SetMaskImage(m_ptrMaskImage);

	if(m_iSelectAtItemListI != -1 && m_iSelectAtItemListJ!= -1 && vpSender != m_btnGridsBuy[m_iSelectAtItemListI][m_iSelectAtItemListJ])
	{
		m_btnGridsBuy[m_iSelectAtItemListI][m_iSelectAtItemListJ]->SetMaskImage((RtwImage*)NULL);
	}
	m_iSelectAtItemListI = i;
	m_iSelectAtItemListJ = j;
	unguard;
}

void CUIFormPlayerShop::OnLDClick_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	unguard;
}

void CUIFormPlayerShop::OnRClick_BuyListButton(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	const string& widgetID = vpSender->GetID();
	int i, j;
	Is_Widget_ItemList(widgetID, i, j);
	SItemID itemID = m_itemListBag.GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		return;
	}
	OnAddBuyItem(itemID);
	unguard;
}

void CUIFormPlayerShop::OnGetHintText_BuyListItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	const string& widgetID = vpSender->GetID();

	int i, j;
	Is_Widget_ItemList(widgetID, i, j);
	SItemID itemID = m_itemListBag.GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		vpSender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, &m_itemListBag, 0, i, j);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

void CUIFormPlayerShop::OnHideForm(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(g_layerMain && !GetPlayer()->m_bPShop)
	{
		CBag *pBagUp;
		pBagUp = m_PShop.GetSellItemBag();
		pBagUp->Clear();
	}
	g_layerMain->OnDeleteWidget(m_pFrmThis);
	unguard;
}

bool CUIFormPlayerShop::Is_Widget_Up(const string& id, int &outI, int &outJ)
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
bool CUIFormPlayerShop::Is_Widget_Down(const string& id, int &outI, int &outJ)
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

bool CUIFormPlayerShop::Is_Widget_ItemList(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<BUY_ITEM_LINE_COUNT; i++)
		for (j=0; j<BUY_ITEM_COLUMN_COUNT; j++)
		{
			const string& idDown = m_btnGridsBuy[i][j]->GetID();
			if ( idDown==id )
				goto Next;
		}
Next:
		if (i<BUY_ITEM_LINE_COUNT && j<BUY_ITEM_COLUMN_COUNT)
		{
			outI = i;
			outJ = j;
			return true;
		}
		else	return false;

		unguard;
}

void CUIFormPlayerShop::OnSetNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	CUIFormInputBox* pInputFrm=(CUIFormInputBox* )vpEvent->GetUser1();
	int num = pInputFrm->GetNumFromInputBox();
	if(num== 0)
	{
		OnCancelOperate(NULL,NULL);
		return;
	}
	m_operateItem.count = num;

	UIFormTextMsg *pFrm;
	pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
	pFrm->SetNumberOnly(true);
	pFrm->SetMaxNum(100000000);
	pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnSetPrice, pFrm);
	pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	unguard;
}

void CUIFormPlayerShop::OnSetPrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	UIFormTextMsg *pFrm = (UIFormTextMsg *)vpEvent->GetUser1();
	RtString str = pFrm->GetString();
	long money = str.ToLong();
	if (money <= 0)
	{
		OnCancelOperate(NULL,NULL);
		return;
	}

	if(m_bOperateSell)
	{
		if(m_bAddSellItemFormDrag)
		{
			if ( !m_PShop.AddSellItem(m_operateItem,m_iSelectI,m_iSelectJ,money) )
			{
				PlayerFailOperate(R(MSG_ITEM_ADDITEM_FAIL));
				m_iSelectI = -1;
				m_iSelectJ = -1;
				m_operateItem = ItemID_CreateInvalid();
				return;
			}
			m_bAddSellItemFormDrag = false;
		}else
		{
			if ( !m_PShop.AddSellItem(m_operateItem, money) )
			{
				PlayerFailOperate(R(MSG_ITEM_ADDITEM_FAIL));
				m_iSelectI = -1;
				m_iSelectJ = -1;
				m_operateItem = ItemID_CreateInvalid();
				return;
			}
		}
	}else
	{
		if ( !m_PShop.AddBuyItem(m_operateItem,m_iSelectI,m_iSelectJ,money) )
		{
			PlayerFailOperate(R(MSG_ITEM_ADDITEM_FAIL));
			m_iSelectI = -1;
			m_iSelectJ = -1;
			m_operateItem = ItemID_CreateInvalid();
			return;
		}
		if(	ItemCanPile(m_operateItem))
			m_iAllBuyItemPrice+= money*m_operateItem.count;
		else
			m_iAllBuyItemPrice+= money;
	}

	g_workspace.ResetHintText();
	Refresh();
	//m_iSelectI = -1;
	//m_iSelectJ = -1;
	m_operateItem = ItemID_CreateInvalid();
	unguard;
}

void CUIFormPlayerShop::OnChangeNum(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	CUIFormInputBox* pInputFrm=(CUIFormInputBox* )vpEvent->GetUser1();
	int num = pInputFrm->GetNumFromInputBox();
	m_operateItem.count = num;

	UIFormTextMsg *pFrm;
	pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
	pFrm->SetNumberOnly(true);
	pFrm->SetMaxNum(100000000);
	pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnChangePrice, pFrm);
	pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	unguard;
}

void CUIFormPlayerShop::OnChangePrice(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	UIFormTextMsg *pFrm = (UIFormTextMsg *)vpEvent->GetUser1();
	RtString str = pFrm->GetString();
	long money = str.ToLong();
	if (money <= 0)
	{
		m_operateItem = ItemID_CreateInvalid();
		return;
	}


	if(m_bOperateSell)
	{
		if ( !m_PShop.GetSellItemBag()->SetItem(m_iSelectI,m_iSelectJ,m_operateItem) )
		{
			PlayerFailOperate(R(MSG_ITEM_ADDITEM_FAIL));
			m_iSelectI = -1;
			m_iSelectJ = -1;
			m_operateItem = ItemID_CreateInvalid();
			return;
		}
	}else
	{
		SItemID item = m_PShop.GetBuyItemBag()->GetItem(0,m_iSelectI,m_iSelectJ);
		m_iAllBuyItemPrice-= m_PShop.GetPrice(item.id)*item.count;
		if( !m_PShop.GetBuyItemBag()->SetItem(m_iSelectI,m_iSelectJ,m_operateItem))
		{
			PlayerFailOperate(R(MSG_ITEM_ADDITEM_FAIL));
			m_iSelectI = -1;
			m_iSelectJ = -1;
			m_operateItem = ItemID_CreateInvalid();
			return;
		}
		if(	ItemCanPile(m_operateItem))
			m_iAllBuyItemPrice+= money*m_operateItem.count;
		else
			m_iAllBuyItemPrice+= money;
	}
	if ( !m_PShop.SetPrice(money, m_operateItem.id) )
	{
		PlayerFailOperate(R(MSG_TRADE_CHANGEPRICEFAIL));
		m_operateItem = ItemID_CreateInvalid();
		return;
	}
	m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskColor(0x01000000);
	m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskValue(0.f);

	OnChangeBtnStopSellOneText();

	g_workspace.ResetHintText();
	m_operateItem = ItemID_CreateInvalid();
	Refresh();
	unguard;
}

void CUIFormPlayerShop::OnMouseRClickEvent(SItemID item)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	if (!ItemID_IsValid(item))
		return;
	if(m_PShop.GetSellItemBag()->Find(item.id))
		return;
	if (item.cBind!=IB_NotBind || !ItemCanDelete(item.type) || !ItemCanDrop(item.type) || !ItemCanTrade(item.type))
	{
		ShowSystemMessage(R(MSG_TRADE_BINDED));
		return;
	}
	if (ItemIsPet(item.type))
	{
		if(ItemID_IsValid(g_layerMain->m_fromPlayerItems->OnGetUsingPetItem()))
		{
			if(g_layerMain->m_fromPlayerItems->OnGetUsingPetItem().id == item.id)
			{
				ShowSystemMessage(R(MSG_PET_CANNOTSELLWHILECALL));
				return;
			}
		}
	}

	m_operateItem = item;
	m_bOperateSell = true;
	if(ItemCanPile(item))
	{
		CUIFormInputBox* pInputFrm;
		pInputFrm = CUIFormInputBox::ShowStatic("",R(MSG_TRADE_ENTERNUMBER));
		pInputFrm->SetMaxNum(item.count);
		pInputFrm->OnMoveToXY(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		pInputFrm->EvOK = RTW_CALLBACK_1(this,CUIFormPlayerShop,OnSetNum,pInputFrm);
		pInputFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pInputFrm);
	}else
	{
		UIFormTextMsg *pFrm;
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
		pFrm->SetMaxNum(100000000);
		pFrm->SetNumberOnly(true);
		pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnSetPrice, pFrm);
		pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	}
	unguard;
}

void CUIFormPlayerShop::OnCancelOperate(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	m_operateItem = ItemID_CreateInvalid();
	if(m_iSelectI != -1 && m_iSelectJ != -1)
	{
		m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskImage((RtwImage*)NULL);
		m_iSelectI = -1;
		m_iSelectJ = -1;
	}
	unguard;
}

void CUIFormPlayerShop::OnLClickBtnStopSellOneLClick(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	if(vpSender->getText()== R(MSG_TRADE_OFFSHELF))
	{
		if(m_iSelectI != -1 && m_iSelectJ != -1)
		{
			SItemID item = m_PShop.GetSellItemBag()->GetItem(m_iSelectI, m_iSelectJ);
			if ( !ItemID_IsValid(item) )
			{
				ShowSystemMessage(R(MSG_ITEM_NOUSEITEMSELECT));
				return;
			}
			m_PShop.SetPrice(-99,item.id);
			m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskColor(0xa0ff0000);
			m_btnGridsUp[m_iSelectI][m_iSelectJ]->SetMaskValue(1.f);
		}else
		{
			ShowSystemMessage(R(MSG_TRADE_SELECTONEITEM));
		}
		vpSender->SetText(R(MSG_TRADE_UPSHELF));
	}else
	{
		OnLDClick_SellItem(m_btnGridsUp[m_iSelectI][m_iSelectJ],NULL);
	}
	unguard;
}

void CUIFormPlayerShop::OnLClickBtnChangeName(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	string shopName = m_tbxShopName->getText();
	m_PShop.SetName(shopName);
	m_btnChangeName->Disable();
	unguard;
}

void CUIFormPlayerShop::OnShopNameChange(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(GetPlayer()->m_bPShop || m_bLockOperate)
		return;
	string shopName = m_tbxShopName->getText();
	if(shopName != m_PShop.GetName() || m_PShop.GetName() == "")
		m_btnChangeName->Enable();
	else
		m_btnChangeName->Disable();
	unguard;
}

void CUIFormPlayerShop::OnLClickBtnCancelItemList(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	m_pFrmBuyItemList->Hide();
	m_iSelectAtItemListI = -1;
	m_iSelectAtItemListJ = -1;
	unguard;
}

void CUIFormPlayerShop::OnShowItemList()
{
	guard;
	m_pFrmBuyItemList->Show();
	g_workspace.SetFocusWidget(m_pFrmBuyItemList);
	m_cbxComboBoxBuyItem->SetSelectedItem(0);
	OnSelectComboBox(NULL,NULL);
	unguard;
}

void CUIFormPlayerShop::OnPreDrawItemList()
{
	guard;
	SItemID item;
	SItemBase *pItemClass = NULL;
	char szTmp[64];
	for (int i=0; i<BUY_ITEM_LINE_COUNT; i++)
		for (int j=0; j<BUY_ITEM_COLUMN_COUNT; j++)
		{
			item = m_itemListBag.GetItem(i, j);
			char *szImage=NULL;
			if ( ItemID_IsValid(item) )
			{
				m_btnGridsBuy[i][j]->ModifyFlags(wfDragServer|wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (strlen(pItemClass->icon) != 0)
				{
					szImage = pItemClass->icon;
					//m_btnGridsUp[i][j]->SetBackgroundImage(new RtwImage(szImage));// change [3/17/2009 tooth.shi]
					m_btnGridsBuy[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

					if (ItemCanPile(item))
					{
						rt2_sprintf(szTmp, "%d", item.count);
						m_btnGridsBuy[i][j]->SetText(szTmp);
						m_btnGridsBuy[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsBuy[i][j]->SetText("");
					}
				}
				m_btnGridsBuy[i][j]->SetHoverImage(m_ptrHoverImage);
				m_btnGridsBuy[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
			}
			else
			{
				m_btnGridsBuy[i][j]->ModifyFlags(0, wfHint | wfDragServer);
				m_btnGridsBuy[i][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGridsBuy[i][j]->SetText("");
			}
		}
	unguard;
}

void CUIFormPlayerShop::OnSelectComboBox(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	int i = m_cbxComboBoxBuyItem->GetSelectedItem();
	if(i == -1)
	{
		ShowSystemMessage(R(MSG_ITEM_SELECTAITEMFIRST));
		return;
	}else
	{
		if(i< m_vectorSItemBase.size())
		{
			m_itemListBag.Clear();
			vector<SItemID>::iterator it;
			for(it = m_vectorSItemBase[i]->begin();it != m_vectorSItemBase[i]->end();it++)
			{
				m_itemListBag.AddItem((*it));
			}
		}
	}

	if(m_iSelectAtItemListI != -1 && m_iSelectAtItemListJ!= -1 )
	{
		m_btnGridsBuy[m_iSelectAtItemListI][m_iSelectAtItemListJ]->SetMaskImage((RtwImage*)NULL);
	}
	m_iSelectAtItemListI = -1;
	m_iSelectAtItemListJ = -1;

	OnPreDrawItemList();
	unguard;
}

void CUIFormPlayerShop::OnLClickBtnOK(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	SItemID item = m_itemListBag.GetItem(0,m_iSelectAtItemListI,m_iSelectAtItemListJ);
	OnAddBuyItem(item);
	unguard;
}

void CUIFormPlayerShop::OnAddBuyItem(SItemID item)
{
	guard;
	m_pFrmBuyItemList->Hide();
	item.id = 0-2-m_iSelectI*m_iSelectJ-m_iSelectJ;
	m_operateItem = item;
	m_bOperateSell = false;
	if(ItemCanPile(item))
	{
		CUIFormInputBox* pInputFrm;
		pInputFrm = CUIFormInputBox::ShowStatic("",R(MSG_TRADE_ENTERNUMBER));
		pInputFrm->OnMoveToXY(g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		pInputFrm->EvOK = RTW_CALLBACK_1(this,CUIFormPlayerShop,OnSetNum,pInputFrm);
		pInputFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pInputFrm);
	}else
	{
		UIFormTextMsg *pFrm;
		pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_SAY_PRICE), R(MSG_ITEM_INPUT_ITEM_PRICE));
		pFrm->SetNumberOnly(true);
		pFrm->EvOK = RTW_CALLBACK_1(this, CUIFormPlayerShop, OnSetPrice, pFrm);
		pFrm->EvCancel	= RTW_CALLBACK_1(this,CUIFormPlayerShop,OnCancelOperate,pFrm);
	}
	unguard;
}

void CUIFormPlayerShop::OnStartShop(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if (!GetWorld()->m_pPlayer->State_CanPShop())
	{
		PlayerFailOperate(R(MSG_FORBID_PSHOP), true);
		return;
	}

	if ( GetPlayer()->m_bPShop )
	{
		PlayerFailOperate(R(MSG_ITEM_PSHOP_START_SUCCESS));
		return;
	}

	if (m_PShop.GetName().empty())
	{
		PlayerFailOperate(R(MSG_ITEM_PSHOP_INPUT_NAME), true);
		return;
	}

	if(m_PShop.GetSellItemBag()->IsEmpty()&& m_PShop.GetBuyItemBag()->IsEmpty())
	{
		ShowSystemMessage(R(MSG_BAITAN_SHOPISEMPTY));
		return;
	}
	if(m_iAllBuyItemPrice> GetPlayer()->mItem.m_Bag.GetMoney())
	{
		PlayerFailOperate(R(MSG_TRADE_NOTENOUGHMONEYTOPURCHASE), true);
		return;
	}

	CItemNetHelper::NetSend_c2r_pshop_start(&m_PShop, NULL);
	LockOperate();
	unguard;
}

void CUIFormPlayerShop::OnStopShop(RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	if(!GetPlayer()->m_bPShop && !m_bLockOperate)
		return;
	CItemNetHelper::NetSend_c2r_pshop_stop();
	UnlockOperate();
	unguard;
}

void CUIFormPlayerShop::OnRecvActorPShop(GcActor *pActor)
{
	guard;

	CHECK(pActor);
	if (pActor->m_bPShop)
	{
		//string txt = R(MSG_ITEM_PSHOP);
		//if (pActor->m_PShopName!="")
		//	txt += ": " + pActor->m_PShopName;
		if (pActor->m_pHUD)
		{
			pActor->m_pHUD->SetStallText((char*)pActor->m_PShopName);
			pActor->m_pHUD->ShowStall(true);
		}
	}
	else 
	{
		if (pActor->m_pHUD)
		{
			pActor->m_pHUD->SetStallText("");
			pActor->m_pHUD->ShowStall(false);
		}
	}

	unguard;
}

void CUIFormPlayerShop::OnRecvStart()
{
	guard;

	GetPlayer()->ChangeCollectTool(true);

	g_layerMain->m_fromPlayerItems->LockOperate(true);
	ShowSystemMessage(R(MSG_BAITAN_BAGLOCKED));
	UnlockOperate();

	ShowSystemMessage(R(MSG_ITEM_PSHOP_START_SUCCESS));
	m_PShop.Start();
	m_tbxShopName->SetEnableInput(false);
	m_btnChangeName->Disable();
	m_btnStart->Disable();
	m_btnStop->Enable();

	GetWorld()->m_pPlayer->m_bPShop = true;
	GetWorld()->m_pPlayer->m_PShopName = m_PShop.GetName().c_str();
	OnRecvActorPShop(GetWorld()->m_pPlayer);

	m_iModelID = GetPlayer()->m_core.ModelId;
	m_iHeadModelID= GetPlayer()->m_core.HeadModelId;
	GetPlayer()->SetModel(5501,0);
	GetPlayer()->mBaseActor.PlayEffect("elevup",true,NULL,RtgVertex3(0,0,0));
	//GetPlayer()->ChangeCollectTool(false);
	//GetPlayer()->OnSetWeaponShow(false);


	unguard;
}

void CUIFormPlayerShop::OnRecvStop()
{
	guard;

	GetPlayer()->ChangeCollectTool(false);

	g_layerMain->m_fromPlayerItems->LockOperate(false);
	ShowSystemMessage(R(MSG_BAITAN_BAGUNLOCKED));
	UnlockOperate();

	ShowSystemMessage(R(MSG_ITEM_PSHOP_STOP_SUCCESS));
	m_PShop.Stop();
	m_tbxShopName->SetEnableInput(true);
	OnShopNameChange(NULL,NULL);
	m_btnStart->Enable();
	m_btnStop->Disable();

	GetWorld()->m_pPlayer->m_bPShop = false;
	OnRecvActorPShop(GetWorld()->m_pPlayer);

	if(m_iModelID != -1 && m_iHeadModelID != -1)
	{
		GetPlayer()->SetModel(m_iModelID,m_iHeadModelID);
		//GetPlayer()->OnSetWeaponShow(true);
		GetPlayer()->mItem.UpdateGraph();
		GetPlayer()->mBaseActor.PlayEffect("elevup",true,NULL,RtgVertex3(0,0,0));
		m_iModelID = -1;
		m_iHeadModelID = -1;
	}

	unguard;
}

void CUIFormPlayerShop::OnCommonItemDeleted(DWORD itemID)
{
	guard;

	// 如果摆摊已经开始，就停止（不应该出现的情况）
	if (m_PShop.IsStart())
		CItemNetHelper::NetSend_c2r_pshop_stop();

	// 在上面的bag就直接删掉
	// 在下面的bag，删掉并且处理钱
	int page, i, j;
	if (m_PShop.GetSellItemBag()->Find(itemID, &page, &i, &j))
	{
		m_PShop.RemoveSellItem(i, j);
		m_PShop.GetSellItemBag()->Pinch();
		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_PShop.GetBuyItemBag()->Find(itemID, &page, &i, &j))
	{
		m_PShop.RemoveBuyItem(i, j);
		m_PShop.GetBuyItemBag()->Pinch();
		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else 
	{
		return;
	}

	unguard;
}

void CUIFormPlayerShop::OnGetFocus(void*,void*)
{
	guard;
	g_layerMain->OnSetNewWidget(m_pFrmThis);
	unguard;
}
void CUIFormPlayerShop::OnChangeBtnStopSellOneText()
{
	guard;
	if((int)(m_PShop.GetPrice(m_operateItem.id))== -99)
		m_btnStopSellOne->SetText(R(MSG_TRADE_UPSHELF));
	else
		m_btnStopSellOne->SetText(R(MSG_TRADE_OFFSHELF));
	unguard;
}

void CUIFormPlayerShop::OnDeleteItem(SItemID& item)
{
	guard;
	CBag *pBagUp;
	pBagUp = m_PShop.GetSellItemBag();
	int i,j,page;
	if(pBagUp->Find(item,&page,&i,&j))
		pBagUp->RemoveItem(0, i, j);
	Refresh();
	unguard;
}

void CUIFormPlayerShop::OnDeleteAllItem()
{
	guard;
	m_PShop.GetBuyItemBag()->Clear();
	m_PShop.GetSellItemBag()->Clear();
	m_PShop.RemoveAllPrice();
	Refresh();
	unguard;
}
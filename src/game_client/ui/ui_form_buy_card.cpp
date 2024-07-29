#include "gc_include.h"
#include "business.h"
#include "ui_form_msg.h"
#include "ui_form_buy_card.h"

CCardTradeList* UIFormBuyCard::m_pList = NULL;

UIFormBuyCard::UIFormBuyCard()
{
	guard;

    m_pList = RT_NEW CCardTradeList;

	m_pFrmThis = LOAD_UI("fmbuycard");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_pFrmGrids = LOAD_UI("fmbuycard.fmsellcard");

	m_lbMoneyTotal = LOAD_UI("fmbuycard.lbgoldnum");
	m_lbPageInfo = LOAD_UI("fmbuycard.lbpage");

	m_btnBuy = LOAD_UI_T(RtwButton, "fmbuycard.btnok");
	m_btnCancel = LOAD_UI_T(RtwButton, "fmbuycard.btncancel");
	m_btnPageLeft = LOAD_UI_T(RtwButton, "fmbuycard.btnitemleft");
    m_btnPageRight = LOAD_UI_T(RtwButton, "fmbuycard.btnitemright");

    //m_btnBuy->SetText("刷新列表");//by fox for string
	m_btnBuy->SetText(R(MSG_REFURBISH_LIST));


	m_btnBuy->EvLClick += RTW_CALLBACK(this, UIFormBuyCard, OnBuy);
	m_btnCancel->EvLClick += RTW_CALLBACK(this, UIFormBuyCard, OnCancel);
	m_btnPageLeft->EvLClick += RTW_CALLBACK(this, UIFormBuyCard, OnPageLeft);
	m_btnPageRight->EvLClick += RTW_CALLBACK(this, UIFormBuyCard, OnPageRight);
	m_pFrmThis->EvHide += RTW_CALLBACK(this, UIFormBuyCard, OnCancel);

    //m_ptrHoverImage = new RtwImage("highlight_button.tga");// change [3/17/2009 tooth.shi]
	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
    m_lLastRefreshTime = 0;

#define GRID_SIZE_X		34
#define GRID_SIZE_Y		34
	RtwRect rc = m_pFrmGrids->GetFrameRect();
	int left = rc.Left() +1;
	int top = rc.Top() +1;
	char strId[32];
	RtwButton* widget = NULL;
	for (int i=0; i<CARD_SHOP_GRID_LINE_COUNT; i++)
    {
		for (int j=0; j<CARD_SHOP_GRID_COLUMN_COUNT; j++)
		{
            m_pItems[i][j] = NULL;
			rt2_sprintf(strId, "CardShop_%d_%d", i, j);
// 			m_btnGrids[i][j] = (RtwButton*)m_pFrmGrids->CreateWidget(strId, "Button", RtwButton::wfDefault | wfDragClient | wfDClick);
// 			widget = m_btnGrids[i][j];// change [3/17/2009 tooth.shi]
			m_btnGrids[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			m_pFrmGrids->AddChild(m_btnGrids[i][j]);
			m_btnGrids[i][j]->SetBorderColor(0xeef59f04);
			m_btnGrids[i][j]->SetBorderSize(1);
			RtwRect sRect;
			sRect.left = (GRID_SIZE_Y+2)*j+2;
			sRect.top = (GRID_SIZE_X+2)*i+2;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_btnGrids[i][j]->Move(sRect);
			//widget->SetTextFormat(RtwStringFormat(alignFar, alignFar));
            m_btnGrids[i][j]->EvLClick += RTW_CALLBACK(this, UIFormBuyCard, OnGridLClick);
			m_btnGrids[i][j]->EvLDClick += RTW_CALLBACK(this, UIFormBuyCard, OnGridLDClick);
			m_btnGrids[i][j]->EvHint += RTW_CALLBACK(this, UIFormBuyCard, OnGridGetHintText);
		}
    }

    m_bWaitingForUser = false;

	m_pFrmThis->Hide();

	unguard;
}

UIFormBuyCard::~UIFormBuyCard()
{
	guard;
    DEL_ONE(m_pList);
	unguard;
}

void UIFormBuyCard::SetUserData(long lNpcID, char cCardType)
{
    m_lNpcID = lNpcID;
    m_cCardType = cCardType;
}

void UIFormBuyCard::CopyPageData()
{
    m_sTotal = m_pList->m_sTotal;
    m_sStart = m_pList->m_sStart;
    m_sCount = m_pList->m_sCount;
    int iPageTotal = CARD_SHOP_GRID_LINE_COUNT*CARD_SHOP_GRID_COLUMN_COUNT;
    if (m_sCount==0 || m_sTotal==0)
    {
        m_sPage = m_sPageCount = 0;
    }else
    {
        m_sPage = (m_sStart / iPageTotal) + 1;
        m_sPageCount = ((m_sTotal-1) / iPageTotal) + 1;
    }

    int x=0, y=0;
    DWORD dwItemType;
    CItemManager* pItemManager = GetWorld()->m_pItemManager;
    CCardTradeList::STradeRecord* pTradeInfo = m_pList->FirstRecord();
    while (pTradeInfo)
    {
        switch (pTradeInfo->lCardGroupType)
        {
        case 21: // 米果一卡通50点
            dwItemType = 734;
            break;
        case 22: // 米果一卡通100点
            dwItemType = 735;
            break;
        case 19: // 米果一卡通150点
            dwItemType = 736;
            break;
        case 23: // 米果一卡通200点
            dwItemType = 737;
            break;
        case 20: // 米果一卡通300点
            dwItemType = 738;
            break;
        case 24: // 米果一卡通500点
            dwItemType = 739;
            break;
        default: // 点卡
            dwItemType = 740;
            break;
        }

        m_pItems[y][x] = GetWorld()->m_pItemManager->GetItemFromIndex(dwItemType);
        m_lConsignationID[y][x] = pTradeInfo->lConsignationID;

        x ++;
        if (x>=CARD_SHOP_GRID_COLUMN_COUNT)
        {
            y ++;
            x = 0;
        }
        if (y>=CARD_SHOP_GRID_LINE_COUNT)
        {
            break;
        }

        pTradeInfo = m_pList->NextRecord();
    }
    for (; y<CARD_SHOP_GRID_LINE_COUNT; y++)
    {
        for (; x<CARD_SHOP_GRID_COLUMN_COUNT; x++)
        {
            m_pItems[y][x] = NULL;
            m_lConsignationID[y][x] = 0;
        }
        x = 0;
    }
}

void UIFormBuyCard::OnPreDraw()
{
	guard;

	char *szImage;
	char szTmp[64];
    int i, j;
	CItemContainer *pMyContainer = &(GetWorld()->m_pPlayer->mItem);

    CopyPageData();

	for (i=0; i<CARD_SHOP_GRID_LINE_COUNT; i++)
    {
		for (j=0; j<CARD_SHOP_GRID_COLUMN_COUNT; j++)
		{
			if (m_pItems[i][j])
			{
				m_btnGrids[i][j]->ModifyFlags(wfHint, 0);
				if (strlen(m_pItems[i][j]->icon) != 0)
				{
					szImage = m_pItems[i][j]->icon;
					//m_btnGrids[i][j]->SetBackgroundImage(new RtwImage(szImage));// change [3/17/2009 tooth.shi]
					m_btnGrids[i][j]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(szImage));

                    /*
					if (ItemCanPile(item))
					{
						sprintf(szTmp, "%d", item.count);
						m_btnGrids[i][j]->SetText(szTmp);
					}else
					{
						m_btnGrids[i][j]->SetText("");
					}
                    */
				}

                m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
                m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
                m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
                m_ptrHoverImage->getRenderImage()->SetBlend(true);
                m_btnGrids[i][j]->SetHoverImage(m_ptrHoverImage);
			}else
			{
				m_btnGrids[i][j]->ModifyFlags(0, wfHint);
				m_btnGrids[i][j]->SetBackgroundImage((RtwImage*)NULL);
				m_btnGrids[i][j]->SetText("");

                RtwComplexImage hoverImg;
                m_btnGrids[i][j]->SetHoverImage(hoverImg);
			}
		}
    }

	// 金钱
	int moneyTotal = pMyContainer->m_Bag.GetMoney();
	rt2_snprintf(szTmp, 64, "%d", moneyTotal);
    //szTmp[63] = 0;
	m_lbMoneyTotal->SetText(szTmp);

    // 翻页
	rt2_snprintf(szTmp, 64, "%d/%d", m_sPage, m_sPageCount);
    //szTmp[63] = 0;
	m_lbPageInfo->SetText(szTmp);

	unguard;
}

void UIFormBuyCard::OnNetDataUpdate(CG_CmdPacket* pPacket)
{
	guard;
    if (m_pList)
    {
        m_pList->ReadFromBlock(pPacket);
        Refresh();
    }
	unguard;
}

void UIFormBuyCard::OnBuyCardResult(CG_CmdPacket* pPacket)
{
	guard;
    RequestPage(m_sPage);
	unguard;
}

void UIFormBuyCard::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormBuyCard::Show()
{
	guard;
    m_sTotal = m_sStart = m_sCount = 0;
	OnPreDraw();
	m_pFrmThis->Show();
    RequestPage(0);
    m_bWaitingForUser = false;
	unguard;
}

bool UIFormBuyCard::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) ) return true;
	else return false;
	unguard;
}

void UIFormBuyCard::Hide()
{
	guard;
	if (m_pFrmThis)
	{
		m_pFrmThis->Hide();
	}
	unguard;
}

bool UIFormBuyCard::GetSelectGridPos(const string& id, int &outI, int &outJ)
{
	guard;

	int i,j;
	for (i=0; i<PSHOP_GRID_LINE_COUNT; i++)
    {
		for (j=0; j<PSHOP_GRID_COLUMN_COUNT; j++)
		{
			const string& idStr = m_btnGrids[i][j]->GetID();
			if ( idStr==id )
				goto Next;
		}
    }
Next:
	if (i<CARD_SHOP_GRID_LINE_COUNT && j<CARD_SHOP_GRID_COLUMN_COUNT)
	{
		outI = j;
		outJ = i;
		return true;
	}else
    {
        return false;
    }
	unguard;
}

void UIFormBuyCard::OnGridLClick(RtwWidget* sender, void*)
{
	guard;
	int x, y;
	if (!GetSelectGridPos(sender->GetID(), x, y))
    {
        return;
    }
	m_iLastSelX = x;
	m_iLastSelY = y;
	unguard;
}

void UIFormBuyCard::OnGridLDClick(RtwWidget* sender, void*)
{
	guard;
	int x, y;
	if (!GetSelectGridPos(sender->GetID(), x, y))
    {
        return;
    }
	m_iLastSelX = x;
	m_iLastSelY = y;

    Buy();
	unguard;
}

void UIFormBuyCard::OnBuy(RtwWidget* sender, void*)
{
	guard;
    if (time(NULL)-m_lLastRefreshTime<5) // 5秒才能刷新一次
    {
        return;
    }
    this->RequestPage(m_sPage);
	unguard;
}

void UIFormBuyCard::Buy()
{
	guard;
    if (m_pItems[m_iLastSelY][m_iLastSelX]==NULL)
    {
        return;
    }

    // 检查金钱
    CCardTradeList::STradeRecord* pTradeInfo = m_pList->Find(m_lConsignationID[m_iLastSelY][m_iLastSelX]);
    if (pTradeInfo)
    {
        int iCurMoney = GetWorld()->m_pPlayer->mItem.GetMoney();
        if (iCurMoney<pTradeInfo->lPrice)
        {
            rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_BUY_MONEY_NOT_ENOUGH), pTradeInfo->lPrice, iCurMoney);
            UIFormMsg::ShowStatic(g_strStaticBuffer, UIFormMsg::TYPE_OK);
            return;
        }
    }

    if (m_bWaitingForUser) return;
    m_bWaitingForUser = true;
    m_iLastCmdX = m_iLastSelX;
    m_iLastCmdY = m_iLastSelY;
    //"您所购的点卡会立即充入您当前角色所属的帐号中，您确定吗？"
    rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_CONFIRM_BUY), m_pItems[m_iLastCmdY][m_iLastCmdX]->name, pTradeInfo->lPrice);
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(g_strStaticBuffer, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormBuyCard, cbOnBuy, pFrm);
	pFrm->EvCancel = RTW_CALLBACK_1(this, UIFormBuyCard, cbOnCancelBuy,pFrm);
	unguard;
}

void UIFormBuyCard::cbOnBuy(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    m_bWaitingForUser = false;
    if (!IsVisible())
		return;
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_buy_card);
    pPacket->WriteLong(m_lNpcID);
    pPacket->WriteLong(m_lConsignationID[m_iLastCmdY][m_iLastCmdX]);
    NetSend(pPacket);
    m_pList->Remove(m_lConsignationID[m_iLastCmdY][m_iLastCmdX]);
    this->Refresh();
	unguard;
}

void UIFormBuyCard::cbOnCancelBuy(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    m_bWaitingForUser = false;
	unguard;
}

void UIFormBuyCard::OnGridGetHintText(RtwWidget* sender, void*)
{
	guard;

    int x, y;
	if (!GetSelectGridPos(sender->GetID(), x, y))
    {
        return;
    }

    if (m_pItems[y][x]==NULL)
    {
        return;
    }

    CCardTradeList::STradeRecord* pTradeRecord = m_pList->Find(m_lConsignationID[y][x]);
    if (pTradeRecord==NULL)
    {
        return;
    }

	int nLines;
    int iPrice = 0;
    int iTimeLimit = 0;
    CItemManager* pItemManager = GetWorld()->m_pItemManager;
    SItemID itemID = pItemManager->CreateItem(pItemManager->GetIndexFromType(m_pItems[y][x]->type));
    if (!ItemID_IsValid(itemID))
    {
        return;
    }

    iPrice = pTradeRecord->lPrice;
    long tNow = time(NULL);
    if (tNow>pTradeRecord->lEndTime)
    {
        iTimeLimit = 0;
    }else
    {
        iTimeLimit = pTradeRecord->lEndTime-tNow;
    }

	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, NULL, -1, -1, -1, -1.f, iPrice, false, -1.f, iTimeLimit);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormBuyCard::OnCancel(RtwWidget* sender, void*)
{
	guard;
    Hide();
	unguard;
}

void UIFormBuyCard::OnPageLeft(RtwWidget* sender,void*)
{
	guard;
	if (m_sPageCount)
	{
        if (m_sPage>0)
        {
            RequestPage(m_sPage-1);
        }
	}
	unguard;
}

void UIFormBuyCard::OnPageRight(RtwWidget* sender,void*)
{
	guard;
	if (m_sPageCount)
	{
        if (m_sPage<m_sPageCount)
        {
            RequestPage(m_sPage+1);
        }
	}
	unguard;
}

void UIFormBuyCard::RequestPage(short sPage)
{
	guard;
    if (sPage>=m_sPageCount) sPage = m_sPageCount-1;
    if (sPage<0) sPage = 0;
    short sPageTotal = CARD_SHOP_GRID_LINE_COUNT*CARD_SHOP_GRID_COLUMN_COUNT;

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_request_card_list);
    pPacket->WriteLong(m_lNpcID);
    pPacket->WriteShort(sPage*sPageTotal);
    pPacket->WriteShort(sPageTotal);
    NetSend(pPacket);
	unguard;
}

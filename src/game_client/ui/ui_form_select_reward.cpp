#include "gc_include.h"
#include "ui_form_select_reward.h"

const char* strMoneyImage = "sit90.bmp";
const char* strExpImage = "sit89.bmp";

UIFormSelectReward::UIFormSelectReward()
{
	guard;

	m_pFrmThis = RTW_WIDGET("fmtaskitem");

	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);
	m_pRewards[0] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb1");
	m_pRewards[1] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb2");
	m_pRewards[2] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb3");
	m_pRewards[3] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb4");
	m_pRewards[4] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb5");
	m_pRewards[5] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb6");
	m_pRewards[6] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb7");
	m_pRewards[7] = RTW_WIDGET_T(CUiCheckButton, "fmtaskitem.aliasComb8");
	m_pOK = RTW_WIDGET_T(RtwButton, "fmtaskitem.btnok");
	m_pCancel = RTW_WIDGET_T(RtwButton, "fmtaskitem.btncancel");
	m_pTitle = RTW_WIDGET("fmtaskitem.lbinfo");

	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	for (int i=0; i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		m_ptrHoverImage->getRenderImage()->SetTransparency(0.4);
		m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
		m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_ptrHoverImage->getRenderImage()->SetBlend(true);
		m_pRewards[i]->SetCheckedImage((RtwComplexImage*)m_ptrHoverImage);

		m_pRewards[i]->SetText("");

		m_pRewards[i]->EvLClick += RTW_CALLBACK(this, UIFormSelectReward, OnLClickReward);
		m_pRewards[i]->EvHint += RTW_CALLBACK(this, UIFormSelectReward, OnGetHintText);
	}
	m_pOK->EvLClick += RTW_CALLBACK(this, UIFormSelectReward, OnOK);
	m_pCancel->EvLClick += RTW_CALLBACK(this, UIFormSelectReward, OnCancel);

	m_bShowOnly = true;
	m_SelectIndex = -1;
	m_TaskID = 0;

	m_pFrmThis->Hide();

	unguard;
}

UIFormSelectReward::~UIFormSelectReward()
{
	guard;

	unguard;
}

void UIFormSelectReward::OnPreDraw()
{
	guard;

	for (size_t i=0; i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		m_TmpItems[i].Clear();
		m_pRewards[i]->ModifyFlags(0, wfHint);
		m_pRewards[i]->SetChecked(false);
	}

	if (m_bShowOnly)
	{
		m_pTitle->SetText(R(MSG_ITEMREWARD_PREVIEW));
	}
	else 
	{
		m_pTitle->SetText(R(MSG_ITEMREWARD_SELECT));
	}
	size_t i;
	for (i=0; i<m_RewardIDs.size() && i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		SFBReward* pReward = g_TableFBReword.Find(m_RewardIDs[i]);
		if (!pReward)
			continue;

		m_pRewards[i]->ModifyFlags(wfHint, 0);

		// 奖励只会有一个
		if (pReward->itemId != 0)
		{
			SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(pReward->itemId);
			if (!pItemClass)
				continue;

			EItemColor itemColor = ItemColor_White;
			if (pReward->color=="white")
				itemColor = ItemColor_White;
			else if (pReward->color=="green")
				itemColor = ItemColor_Green;
			else if (pReward->color=="blue")
				itemColor = ItemColor_Blue;
			else if (pReward->color=="golden")
				itemColor = ItemColor_Golden;
			else 
				itemColor = ItemColor_White;

			int itemLevel = 0;
			EItemBind itemBind = IB_NotBind;
			if (pReward->bindType==0)
				itemBind = IB_NotBind;
			else if (pReward->bindType==1)
				itemBind = IB_BindWhenPick;
			else if (pReward->bindType==2)
				itemBind = IB_BindWhenEquip;
			else 
				itemBind = IB_NotBind;

			WORD ColorAttribID1 = pReward->colorParam1;
			BYTE ColorRate1 = pReward->colorRate;
			WORD ColorAttribID2 = pReward->colorParam2;
			BYTE ColorRate2 = pReward->colorRate;

			SItemID item = GetWorld()->m_pItemManager->CreateItem(pReward->itemId, itemColor, itemLevel, itemBind, ColorAttribID1, ColorRate1, ColorAttribID2, ColorRate2);
			if (ItemID_IsValid(item))
			{
				m_TmpItems[i] = item;
				//m_pRewards[i]->SetBackgroundImage( new RtwImage(pItemClass->icon) );// change [3/17/2009 tooth.shi]
				m_pRewards[i]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));
			}
		}
		else if (pReward->exp != 0)
		{
			//m_pRewards[i]->SetBackgroundImage( new RtwImage(strExpImage) );// change [3/17/2009 tooth.shi]
			m_pRewards[i]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(strExpImage));
		}
		else if (pReward->gold != 0)
		{
			//m_pRewards[i]->SetBackgroundImage( new RtwImage(strMoneyImage) );// change [3/17/2009 tooth.shi]
			m_pRewards[i]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(strMoneyImage));
		}
	}
	for (; i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		m_pRewards[i]->SetBackgroundImage((RtwImage*)NULL);
	}

	if (m_SelectIndex>=0 && m_SelectIndex<(int)m_RewardIDs.size())
	{
		m_pRewards[m_SelectIndex]->SetChecked(true);
		m_pOK->Enable();
	}
	else 
	{
		m_pOK->Disable();
	}

	unguard;
}

void UIFormSelectReward::Show()
{
	guard;
	OnPreDraw();
	m_pFrmThis->ShowAndFocus();
	unguard;
}

bool UIFormSelectReward::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormSelectReward::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormSelectReward::Refresh()
{
	guard;
	if ( IsVisible() )		Show();
	unguard;
}

void UIFormSelectReward::OnOK(RtwWidget* sender, void*)
{
	guard;

	if (m_bShowOnly)
	{
		Hide();
		return;
	}

	if (m_TaskID!=0 && (m_SelectIndex>=0 && m_SelectIndex<(int)m_RewardIDs.size()) )
	{
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_finish_fb_task);
		pPacket->WriteLong(m_TaskID);
		pPacket->WriteLong(GetSelectRewardID());
		NetSend(pPacket);
	}

	Hide();

	unguard;
}

void UIFormSelectReward::OnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormSelectReward::OnGetHintText(RtwWidget* sender, void*)
{
	guard;
	size_t i;
	for (i=0; i<m_RewardIDs.size() && i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		if (sender == m_pRewards[i])
			break;
	}
	if (i>=m_RewardIDs.size() || i>=UI_FORM_SELECT_REWARD_MAX_COUNT)
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	SFBReward* pReward = g_TableFBReword.Find(m_RewardIDs[i]);
	if (!pReward)
		return;

	if (pReward->itemId != 0)
	{
		int nLines;
		RtString str = GenerateItemHtmlHint(nLines, m_TmpItems[i], GetWorld()->m_pPlayer);
		g_workspace.SetHintText((char*)str, true);
		//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
		g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}
	else if (pReward->exp != 0)
	{
		char cTmp256[256];
		rt2_sprintf(cTmp256, "%s: %d", R(Exp), pReward->exp);
		g_workspace.SetHintText(cTmp256);
		g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}
	else if (pReward->gold != 0)
	{
		char cTmp256[256];
		rt2_sprintf(cTmp256, "%s: %d", R(G_Money), pReward->gold);
		g_workspace.SetHintText(cTmp256);
		g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}

	unguard;
}

void UIFormSelectReward::OnLClickReward(RtwWidget* sender, void*)
{
	guard;

	if (m_bShowOnly)
		return;
	size_t i;
	for (i=0; i<m_RewardIDs.size() && i<UI_FORM_SELECT_REWARD_MAX_COUNT; i++)
	{
		if (sender == m_pRewards[i])
			break;
	}
	if (i>=m_RewardIDs.size() || i>=UI_FORM_SELECT_REWARD_MAX_COUNT)
	{
		return;
	}

	m_SelectIndex = i;

	Refresh();

	unguard;
}

#include "gc_include.h"
#include "item_net_define.h"
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
#include "ui_form_timebar.h"
#include "ui_form_char.h"

static bool ComposingSortFunc(SUICompose& c1, SUICompose& c2);

bool ComposingSortFunc(SUICompose& c1, SUICompose& c2)
{
    guard;

    if (c1.Level >= c2.Level)
        return true;
    else 
        return false;

    unguard;
}

UIFormItemCompose::UIFormItemCompose()
{
	guard;

	m_ComposeType = (EItemComposeType)0;
	m_SelectComposeID = 0;
	m_MakeCount = -1;
	m_CanMakeCount = -1;
	m_bRecvComposeEndSuccess = false;
	m_bComposeTimeOver = false;
	m_bMaterialUseScroll = false;
	m_ScrollLine = 0;
	m_bMaking = false;

	m_pFrmThis = RTW_WIDGET("fmmineskill");
	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);
	m_pProductInfo = RTW_WIDGET("fmmineskill.lbtext1");
	m_pMaterialTitle = RTW_WIDGET("fmmineskill.lbborder1.lbxmm");
	m_pComposeList = RTW_WIDGET_T(RtwListBox, "fmmineskill.fmtext.listfriend");
	m_pProductIcon = RTW_WIDGET_T(RtwButton, "fmmineskill.btnmineitem");
	((RtwButton*)m_pProductIcon)->SetHoverImage(m_ptrHoverImage);
	m_pMaterialIcon[0] = RTW_WIDGET_T(RtwButton, "fmmineskill.pansource.btnmineitem");
	((RtwButton*)m_pMaterialIcon[0])->SetHoverImage(m_ptrHoverImage);
	m_pMaterialIcon[1] = RTW_WIDGET_T(RtwButton, "fmmineskill.pansource.btnmineitem1");
	((RtwButton*)m_pMaterialIcon[1])->SetHoverImage(m_ptrHoverImage);
	m_pMaterialIcon[2] = RTW_WIDGET_T(RtwButton, "fmmineskill.pansource.btnmineitem2");
	((RtwButton*)m_pMaterialIcon[2])->SetHoverImage(m_ptrHoverImage);
	m_pMaterialIcon[3] = RTW_WIDGET_T(RtwButton, "fmmineskill.pansource.btnmineitem3");
	((RtwButton*)m_pMaterialIcon[3])->SetHoverImage(m_ptrHoverImage);
	m_pMaterialIcon[4] = RTW_WIDGET_T(RtwButton, "fmmineskill.pansource.btnmineitem4");
	((RtwButton*)m_pMaterialIcon[4])->SetHoverImage(m_ptrHoverImage);
	m_pMaterialInfo[0] = RTW_WIDGET("fmmineskill.pansource.lbtbc3");
	m_pMaterialInfo[1] = RTW_WIDGET("fmmineskill.pansource.lbtbc4");
	m_pMaterialInfo[2] = RTW_WIDGET("fmmineskill.pansource.lbtbc2");
	m_pMaterialInfo[3] = RTW_WIDGET("fmmineskill.pansource.lbtbc5");
	m_pMaterialInfo[4] = RTW_WIDGET("fmmineskill.pansource.lbmineinfo4");
	m_pMake = RTW_WIDGET_T(RtwButton, "fmmineskill.btnok");
	m_pCount = RTW_WIDGET_T(RtwTextBox, "fmmineskill.lbtext2");
	m_pLeftArrow = RTW_WIDGET_T(RtwButton, "fmmineskill.btnbaby3");
	m_pRightArrow = RTW_WIDGET_T(RtwButton, "fmmineskill.btnbaby4");
	//m_pMaterialScroll = RTW_WIDGET_T(RtwScrollBar, "fmmineskill.fmsource.scrolldialg");
	m_pFrmImage = RTW_WIDGET("fmmineskill.pansource");
	m_pMaxCont = RTW_WIDGET_T(RtwButton, "fmmineskill.btnbaby2");
	m_pNeedMoney = RTW_WIDGET_T(RtwLabel, "fmmineskill.lbmoney");

	char tmp[128];
	for (int i=1;i<ICT_Count-1;i++)
	{
		rt2_sprintf(tmp,"fssj_texture/compose%d.tga",i);
		m_pBackImage[i] = g_workspace.getImageFactory()->createImage(tmp);
		m_pBackImage[i]->SetBlend(true);
	}
	//m_pFrmThis = RTW_WIDGET("fmmineskill");	// 控制显示与否的大FORM
	//m_pComposeList = RTW_WIDGET_T(RtwListBox, "fmmineskill.fmmine.listfriend");	// 列表框
	//m_pMaterialTitle = RTW_WIDGET("fmmineskill.fmmine.lbtitle");				// 标题
	//m_pProductIcon = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource01.btnmineitem");	// 炼化的图标
	//m_pProductInfo = RTW_WIDGET("fmmineskill.fmsource.pansource01.lbmineinfo");	// 炼化的信息
	//// 材料的图标与信息
	//m_pMaterialIcon[0] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource03.btnmineitem");
	//m_pMaterialIcon[1] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource04.btnmineitem");
	//m_pMaterialIcon[2] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource05.btnmineitem");
	//m_pMaterialIcon[3] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource06.btnmineitem");
	//m_pMaterialIcon[4] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource07.btnmineitem");
	//m_pMaterialInfo[0] = RTW_WIDGET("fmmineskill.fmsource.pansource03.lbmineinfo");
	//m_pMaterialInfo[1] = RTW_WIDGET("fmmineskill.fmsource.pansource04.lbmineinfo");
	//m_pMaterialInfo[2] = RTW_WIDGET("fmmineskill.fmsource.pansource05.lbmineinfo");
	//m_pMaterialInfo[3] = RTW_WIDGET("fmmineskill.fmsource.pansource06.lbmineinfo");
	//m_pMaterialIcon[4] = RTW_WIDGET_T(RtwButton, "fmmineskill.fmsource.pansource07.btnmineitem");

	//m_pMake = RTW_WIDGET_T(RtwButton, "fmmineskill.btnok");	// 制作的确定按键
	//m_pCount = RTW_WIDGET_T(RtwTextBox, "fmmineskill.lbtimes");	// 制作的数量
	//m_pLeftArrow = RTW_WIDGET_T(RtwButton, "fmmineskill.btnitemleft");	// 数量的减
	//m_pRightArrow = RTW_WIDGET_T(RtwButton, "fmmineskill.btnitemright");// 数量的加
	//m_pMaterialScroll = RTW_WIDGET_T(RtwScrollBar, "fmmineskill.fmsource.scrolldialg");	// 滚动条
	//m_pFrmImage = RTW_WIDGET("fmmineskill");		// 炼化背景图
	//m_pProgressBar = RTW_WIDGET_T(RtwProgressBar, "panBar.barexp");	// 进度条

	m_pCount->SetNumberOnly(true);
	m_pCount->EvInputChar		+= RTW_CALLBACK(this,UIFormItemCompose,OnTextBoxChange);
	m_pProductIcon->ModifyFlags(wfHint, 0);

	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);
	m_pFrmThis->EvHide += RTW_CALLBACK(this, UIFormItemCompose, OnHide);
	m_pMake->EvLClick += RTW_CALLBACK(this, UIFormItemCompose, OnClicked_Make);
	m_pLeftArrow->EvLClick += RTW_CALLBACK(this, UIFormItemCompose, OnClicked_LeftArrow);
	m_pRightArrow->EvLClick += RTW_CALLBACK(this, UIFormItemCompose, OnClicked_RightArrow);
	m_pComposeList->EvSelect += RTW_CALLBACK(this, UIFormItemCompose, OnSelected_Composing);
	//m_pMaterialScroll->EvScroll += RTW_CALLBACK(this, UIFormItemCompose, OnScrollMaterials);
	m_pProductIcon->EvHint += RTW_CALLBACK(this, UIFormItemCompose, OnGetHintText);
	m_pMaxCont->EvLClick += RTW_CALLBACK(this, UIFormItemCompose, OnClicked_MaxCont);

	m_pFrmThis->Hide();

	unguard;
}

UIFormItemCompose::~UIFormItemCompose()
{
	guard;

	unguard;
}

void UIFormItemCompose::OnPreDraw(bool bComposeList/* = true*/, bool bComposingDetail/* = true*/)
{
	guard;

	// 配方标题
	m_pFrmImage->SetBackgroundImage(m_pBackImage[m_ComposeType]);
	
	switch (m_ComposeType)
	{
	case ICT_Equipment:
		m_pProductIcon->SetText("武器");
		m_pMaterialTitle->SetText("铸兵术");
		break;
	case ICT_Yaoping:
		m_pProductIcon->SetText("丹药");
		m_pMaterialTitle->SetText("炼丹术");
		break;
	case ICT_EnchantScroll:
		m_pProductIcon->SetText("符咒");
		m_pMaterialTitle->SetText("制符术");
		break;
	case ICT_Casting:
		m_pProductIcon->SetText("护甲");
		m_pMaterialTitle->SetText("铸甲术");
		break;
	default:
		break;
	}
	if ( !bComposeList )
	{
		m_pProductIcon->SetText("");
	}

	CItemManager* pItemMgr = GetWorld()->m_pItemManager;
	CItemComposeMgr* pComposeMgr = pItemMgr->m_pComposeMgr;

	CItemContainer* pItemContainer = &(GetWorld()->m_pPlayer->mItem);
	SItemBase* pItemClass = NULL;
	SItemCompose *pComposeClass = NULL;
	char strItemInfo[1024];

	// 配方列表
	if (bComposeList)
	{
		m_pComposeList->RemoveAllItems();
		std::vector<SUICompose>::iterator iterCompose = m_Composes.begin();
		for (; iterCompose!=m_Composes.end(); iterCompose++)
		{
			SUICompose& c = *iterCompose;
			m_pComposeList->AddItem(c.Name);
		}
	}

	// 配方详细
	if (bComposingDetail)
	{
		pComposeClass = pComposeMgr->FindComposing(m_SelectComposeID);
		if (!pComposeClass)
		{
			m_pProductIcon->Hide();
			m_pProductIcon->SetBackgroundImage((RtwImage*)NULL);
			m_pProductInfo->SetText("");

			for (int i=0; i<UI_ITEMCOMPOSE_MATERIAL_COUNT; i++)
			{
				m_pMaterialIcon[i]->SetBackgroundImage((RtwImage*)NULL);
				m_pMaterialIcon[i]->Hide();
				m_pMaterialIcon[i]->SetText("");
				m_pMaterialInfo[i]->SetText("");
				m_pMaterialInfo[i]->SetTextColor(RtwPixel(0xffffffff));
			}
			m_pCount->SetText("");
			m_pNeedMoney->SetText("");
			return;
		}

		// 成品
		pItemClass = pItemMgr->GetItemFromIndex(pComposeClass->Product);
		if (!pItemClass)		return;
		RtwImage* pImage = NULL;
		m_pProductIcon->Show();
		//pImage = new RtwImage(pItemClass->icon);
		pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
		m_pProductIcon->SetBackgroundImage(pImage);

		int money = pComposeClass->m_dwNeetMoney;
		rt2_sprintf(strItemInfo, "%d", money);
		m_pNeedMoney->SetText(strItemInfo);

		RtwPixel TmpColor;
		switch (m_ComposeType)
		{
		case ICT_Casting:
		case ICT_Equipment:
			rt2_sprintf(strItemInfo, "%s", pItemClass->name);
			TmpColor = RtwPixel(0xff30e512);
			break;
		case ICT_Yaoping:
			if (ItemCanPile(pItemClass->type))
				rt2_sprintf(strItemInfo, "%s(%d)", pItemClass->name, pItemClass->initCount);
			else 
				rt2_sprintf(strItemInfo, "%s", pItemClass->name);
			TmpColor = RtwPixel(0xffffffff);
			break;
		case ICT_EnchantScroll:
			if (ItemCanPile(pItemClass->type))
				rt2_sprintf(strItemInfo, "%s(%d)", pItemClass->name, pItemClass->initCount);
			else 
				rt2_sprintf(strItemInfo, "%s", pItemClass->name);
			TmpColor = RtwPixel(0xffffffff);
			break;
		default:
			break;
		}
		m_pProductInfo->SetTextColor(TmpColor);
		m_pProductInfo->SetText(strItemInfo);

		// 材料
		int nCanMakeCount = 10000;
		int nCounter = 0;
		for (int i=m_ScrollLine*2; i<pComposeClass->MaterialTypeCount && nCounter<UI_ITEMCOMPOSE_MATERIAL_COUNT; i++, nCounter++)
		{
			pItemClass = pItemMgr->GetItemFromIndex(pComposeClass->Materials[i]);
			if (!pItemClass)	return;
			m_pMaterialIcon[nCounter]->Show();
			//pImage = new RtwImage(pItemClass->icon);// change [3/17/2009 tooth.shi]
			pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
			m_pMaterialIcon[nCounter]->SetBackgroundImage(pImage);

			int nHaveCount = 0;
			int nNeedCount = pComposeClass->MaterialNums[i];
			pItemContainer->m_Bag.FindType(pComposeClass->Materials[i], ItemColor_All, &nHaveCount);
			rt2_sprintf(strItemInfo, "%s", pItemClass->name);
			m_pMaterialInfo[nCounter]->SetText(strItemInfo);
			rt2_sprintf(strItemInfo, "%d/%d", nHaveCount, nNeedCount);
			m_pMaterialIcon[nCounter]->SetText(strItemInfo);
			if (nHaveCount >= nNeedCount)
				m_pMaterialInfo[nCounter]->SetTextColor(RtwPixel(0xffffffff));
			else 
				m_pMaterialInfo[nCounter]->SetTextColor(RtwPixel(0xffff0000));

			int nTmp = nHaveCount / nNeedCount;
			if (nTmp < nCanMakeCount)
				nCanMakeCount = nTmp;
		}
		for (; nCounter<UI_ITEMCOMPOSE_MATERIAL_COUNT; nCounter++)
		{
			m_pMaterialIcon[nCounter]->Hide();
			m_pMaterialIcon[nCounter]->SetBackgroundImage((RtwImage*)NULL);
			m_pMaterialIcon[nCounter]->SetText("");
			m_pMaterialInfo[nCounter]->SetText("");
			m_pMaterialInfo[nCounter]->SetTextColor(RtwPixel(0xffffffff));
		}
		m_CanMakeCount = nCanMakeCount;

		// 制作个数
		if (m_MakeCount < 0)
		{
			m_MakeCount = 0;
		}
		m_pCount->SetText(rtFormatNumber(m_MakeCount));
	}

	unguard;
}

void UIFormItemCompose::OnClicked_MaxCont(RtwWidget* sender, void*)
{
	// 制作个数
	//if (m_MakeCount >= m_CanMakeCount)
	//{
	//	m_MakeCount = 0;
	//	m_pMaxCont->SetText("MAX");
	//}else
	//{
	//	m_MakeCount = m_CanMakeCount;
	//	m_pMaxCont->SetText("MIN");
	//}
	m_MakeCount = m_CanMakeCount;
	m_pCount->SetText(rtFormatNumber(m_MakeCount));
}

void UIFormItemCompose::OnClicked_Make(RtwWidget* sender, void*)
{
	guard;
	if (m_SelectComposeID)
	{
		if (m_MakeCount<=0)
		{
			PlayerFailOperate(R(MSG_ITEM_ZHIZAOSHUMU));
			return;
		}

		if (m_MakeCount > m_CanMakeCount)
			m_MakeCount = m_CanMakeCount;
		Refresh(false, true);
		if (m_MakeCount <= 0)
		{
			PlayerFailOperate(R(MSG_ITEM_NOTENOUGHMATERIALS));
			return;
		}
		g_layerMain->m_fromPlayerItems->LockOperate(true);
		CItemNetHelper::NetSend_c2r_compose_item(m_SelectComposeID);
		m_pComposeList->Disable();
		m_pMake->Disable();
	}
	else
	{
		PlayerFailOperate(R(MSG_ITEM_MEIXUANPEIFANG));
	}
	unguard;
}

void UIFormItemCompose::OnClicked_LeftArrow(RtwWidget* sender, void*)
{
	guard;

	m_MakeCount--;
	if (m_MakeCount < 0)
		m_MakeCount = 0;

	Refresh(false, true);

	unguard;
}

void UIFormItemCompose::OnClicked_RightArrow(RtwWidget* sender, void*)
{
	guard;

	m_MakeCount++;

	Refresh(false, true);

	unguard;
}

void UIFormItemCompose::OnSelected_Composing(RtwWidget* sender, void*)
{
	int nSel = m_pComposeList->GetSelectedItem();
	if (nSel >= 0)
	{
		if (m_SelectComposeID != m_Composes[nSel].ID)
		{
			m_SelectComposeID = m_Composes[nSel].ID;
			SItemCompose* pComposeClass = GetWorld()->m_pItemManager->m_pComposeMgr->FindComposing(m_SelectComposeID);
			CHECK(pComposeClass);
			m_bMaterialUseScroll = (pComposeClass->MaterialTypeCount > UI_ITEMCOMPOSE_MATERIAL_COUNT) ? true:false ;
			m_ScrollLine = 0;
			//m_pMaterialScroll->SetValue(0);
			m_MakeCount = -1;
			Refresh(false, true);
		}
	}
}

void UIFormItemCompose::OnScrollMaterials(RtwWidget* sender,void*)
{
	//if (m_bMaterialUseScroll)
	//{
	//	m_ScrollLine = m_pMaterialScroll->GetValue();
	//	g_workspace.ResetHintText();
	//	Refresh(false, true);
	//}
}

void UIFormItemCompose::OnHide(RtwWidget* sender,void*)
{
	if (m_bMaking)
	{
		CItemNetHelper::NetSend_c2r_compose_item_stop(m_SelectComposeID);
	}
}

void UIFormItemCompose::OnGetHintText(RtwWidget* sender, void*)
{
	guard;

	if (m_pProductIcon==sender)
	{
		if (m_SelectComposeID <= 0)
		{
			return;
		}

		SItemCompose* pComposeClass = GetWorld()->m_pItemManager->m_pComposeMgr->FindComposing(m_SelectComposeID);
		if (!pComposeClass)		return;
		SItemID item = GetWorld()->m_pItemManager->CreateItem(pComposeClass->Product, ItemColor_Green);
		item.attribID1 = 0; // Cheat
		item.attribID2 = 0; // Cheat
		if (ItemID_IsValid(item))
		{
			int nLines;
			RtString str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer);
			g_workspace.SetHintText((char*)str, true);
			g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
			g_workspace.AutoHintSize();
			g_workspace.AutoHintPosition();
		}
	}
	else 
	{
		return;
	}

	unguard;
}

void UIFormItemCompose::Show(EItemComposeType ComposeType)
{
	guard;

	m_ComposeType = ComposeType;
	m_SelectComposeID = 0;

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
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Hide();
		//if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
		//	g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}

	RefreshPlayerComposeList();

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

void UIFormItemCompose::RefreshPlayerComposeList()
{
	m_Composes.clear();

	CItemComposeMgr* pComposeMgr = GetWorld()->m_pItemManager->m_pComposeMgr;
	SItemCompose* pComposeClass = NULL;
	std::list<short>& ComposeList = GetWorld()->m_pPlayer->m_matchList;
	std::list<short>::iterator iterCompose = ComposeList.begin();
	for (; iterCompose!=ComposeList.end(); iterCompose++)
	{
		short ComposeID = *iterCompose;
		pComposeClass = pComposeMgr->FindComposing(ComposeID);
		if (pComposeClass && pComposeClass->ComposeType == m_ComposeType)
		{
			SUICompose c;
			c.ID = pComposeClass->Idx;
            c.Level = pComposeClass->Level;
			c.Name = pComposeClass->Name;
			m_Composes.push_back(c);
		}
	}

    //sort(m_Composes.begin(), m_Composes.end(), ComposingSortFunc);
}

bool UIFormItemCompose::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormItemCompose::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormItemCompose::Refresh(bool bComposeList/* = true*/, bool bComposingDetail/* = true*/)
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw(bComposeList, bComposingDetail);
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormItemCompose::OnComposeTimeOver(RtwWidget* sender, void*)
{
	m_bComposeTimeOver = true;
	if (m_bRecvComposeEndSuccess)
	{
		ComposeEndSuccess(m_SelectComposeID);
	}
}

void UIFormItemCompose::OnRecvComposeBegin(short ComposeID, short ErrorCode)
{
	switch (ErrorCode)
	{
	case itemmsg_success:
		ComposeBegin(ComposeID);
		break;
	case itemmsg_compose_bagfull:
		PlayerFailOperate(R(MSG_ITEM_LEFTONEEMPTYFIRST));
		ComposeFail(ComposeID);
		break;
	default:
		ComposeFail(ComposeID);
		break;
	}
}

void UIFormItemCompose::OnRecvComposeEnd(short ComposeID, short ErrorCode)
{
	switch (ErrorCode)
	{
	case itemmsg_success:
		m_bRecvComposeEndSuccess = true;
		if (m_bComposeTimeOver)
			ComposeEndSuccess(ComposeID);
		break;
	case itemmsg_compose_bagfull:
		PlayerFailOperate(R(MSG_ITEM_LEFTONEEMPTYFIRST));
		ComposeFail(ComposeID);
		break;
	default:
		ComposeFail(ComposeID);
		break;
	}
}

void UIFormItemCompose::ComposeBegin(short ComposeID)
{
	// 列表选中
	int counter = 0;
	std::vector<SUICompose>::iterator iterCompose = m_Composes.begin();
	for (; iterCompose!=m_Composes.end(); iterCompose++)
	{
		SUICompose& c = *iterCompose;
		if (ComposeID == c.ID)
			break;
		counter ++;
	}
	if (counter < m_Composes.size())
		m_pComposeList->SetSelectedItem(counter);

	m_bMaking = true;
	m_bRecvComposeEndSuccess = false;
	m_bComposeTimeOver = false;
	m_pMake->Disable();
	m_pComposeList->Disable();
	g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_ItemCompose, ITEMCOMPOSE_USE_TIME);
	g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this, UIFormItemCompose, OnComposeTimeOver);
	g_layerMain->m_fromPlayerItems->LockOperate(true);
}

void UIFormItemCompose::ComposeEndSuccess(short ComposeID)
{
	g_layerMain->m_fromPlayerItems->LockOperate(false);
	m_bMaking = false;
	CItemManager* pItemMgr = GetWorld()->m_pItemManager;
	SItemBase* pItemClass = NULL;
	SItemCompose* pComposeClass = GetWorld()->m_pItemManager->m_pComposeMgr->FindComposing(m_SelectComposeID);
	pItemClass = pItemMgr->GetItemFromIndex(pComposeClass->Product);
	if (!pItemClass)		return;
	char duanzao[128];
	rt2_sprintf(duanzao,R(MSG_ITEM_DUANZAO_SUCCESS),pItemClass->name);
	PlayerFailOperate(duanzao);
	/*if ()
	{
	}*/
	m_pMake->Enable();
	m_pComposeList->Enable();
	g_layerMain->m_formTimeBar->End(false);

	m_MakeCount--;
	if (m_MakeCount < 0)
		m_MakeCount = 0;

	Refresh(false, true);

	// 继续制作下一个
	if (m_MakeCount > 0)
	{
		OnClicked_Make(m_pMake, NULL);
	}
}

void UIFormItemCompose::ComposeFail(short ComposeID)
{
	g_layerMain->m_fromPlayerItems->LockOperate(false);
	m_bMaking = false;
	PlayerFailOperate(R(MSG_ITEM_MADEFAIL));
	m_pComposeList->Enable();
	m_pMake->Enable();
	Refresh(false, true);
	g_layerMain->m_formTimeBar->End(false);
}

void UIFormItemCompose::OnTextBoxChange(RtwWidget* sender, void*)
{
	guard;
	int num = atoi(m_pCount->getText().c_str());
	if(num)
	{
		if(m_CanMakeCount< num)
			m_MakeCount = m_CanMakeCount;
		else
			m_MakeCount = num;
	}
	else
		m_MakeCount = 0;
	Refresh(true,true);
	unguard;
}
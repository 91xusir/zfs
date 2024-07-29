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
#include "ui_form_char.h"

CUIForm_Gem::CUIForm_Gem(void)
{
	guard;
	m_pFrmThis				= LOAD_UI("fmgem");
	m_plbMoney				= LOAD_UI_T(RtwLabel,"fmgem.lbmoney");
	m_btnOK					= LOAD_UI_T(RtwButton,"fmgem.btnok");
	m_pLbRate				= LOAD_UI("fmgem.lbrate");
	m_btnOK->EvLClick		+= RTW_CALLBACK(this,CUIForm_Gem,OnBtnOKDown);
	m_pFrmThis->ModifyFlags(wfDragClient, 0);
	m_pFrmThis->EvFocus		+= RTW_CALLBACK(this,CUIForm_Gem,OnGetFocus);
	m_pFrmThis->EvHide		+= RTW_CALLBACK(this,CUIForm_Gem,OnHide);
	char fullName[50];
	for(int i = 0;i< 5;i++)
	{
		rt2_sprintf(fullName,"fmgem.pansource.btnmineitem%d",i);
		m_pBtnItem[i]		= LOAD_UI(fullName);
	}
	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);
	m_pBtnItem[0]->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem[0]->EvDrag		+= RTW_CALLBACK(this, CUIForm_Gem,OnDragToGem1);
	m_pBtnItem[0]->EvDragEnd	+= RTW_CALLBACK(this, CUIForm_Gem,OnDragEndGem);
	m_pBtnItem[0]->EvMouseRDown += RTW_CALLBACK(this, CUIForm_Gem,OnMouseRDown);
	m_pBtnItem[0]->EvHint		+= RTW_CALLBACK(this, CUIForm_Gem, OnGetHintText);
	((RtwButton*)m_pBtnItem[0])->SetHoverImage(m_ptrHoverImage);
	m_pBtnItem[1]->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem[1]->EvDrag		+= RTW_CALLBACK(this, CUIForm_Gem,OnDragToGem2);
	m_pBtnItem[1]->EvDragEnd	+= RTW_CALLBACK(this, CUIForm_Gem,OnDragEndGem);
	m_pBtnItem[1]->EvMouseRDown += RTW_CALLBACK(this, CUIForm_Gem,OnMouseRDown);
	m_pBtnItem[1]->EvHint		+= RTW_CALLBACK(this, CUIForm_Gem, OnGetHintText);
	((RtwButton*)m_pBtnItem[1])->SetHoverImage(m_ptrHoverImage);
	m_pBtnItem[2]->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem[2]->EvDrag		+= RTW_CALLBACK(this, CUIForm_Gem,OnDragToGem3);
	m_pBtnItem[2]->EvDragEnd	+= RTW_CALLBACK(this, CUIForm_Gem,OnDragEndGem);
	m_pBtnItem[2]->EvMouseRDown += RTW_CALLBACK(this, CUIForm_Gem,OnMouseRDown);
	m_pBtnItem[2]->EvHint		+= RTW_CALLBACK(this, CUIForm_Gem, OnGetHintText);
	((RtwButton*)m_pBtnItem[2])->SetHoverImage(m_ptrHoverImage);
	m_pBtnItem[3]->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem[3]->EvDrag		+= RTW_CALLBACK(this, CUIForm_Gem,OnDragToGem4);
	m_pBtnItem[3]->EvDragEnd	+= RTW_CALLBACK(this, CUIForm_Gem,OnDragEndGem);
	m_pBtnItem[3]->EvMouseRDown += RTW_CALLBACK(this, CUIForm_Gem,OnMouseRDown);
	m_pBtnItem[3]->EvHint		+= RTW_CALLBACK(this, CUIForm_Gem, OnGetHintText);
	((RtwButton*)m_pBtnItem[3])->SetHoverImage(m_ptrHoverImage);
	m_pBtnItem[4]->ModifyFlags(wfDragClient | wfDragServer|wfDragIn, 0);
	m_pBtnItem[4]->EvDrag		+= RTW_CALLBACK(this, CUIForm_Gem,OnDragToGem5);
	m_pBtnItem[4]->EvDragEnd	+= RTW_CALLBACK(this, CUIForm_Gem,OnDragEndGem);
	m_pBtnItem[4]->EvMouseRDown += RTW_CALLBACK(this, CUIForm_Gem,OnMouseRDown);
	m_pBtnItem[4]->EvHint		+= RTW_CALLBACK(this, CUIForm_Gem, OnGetHintText);
	((RtwButton*)m_pBtnItem[4])->SetHoverImage(m_ptrHoverImage);
	m_pageGem1 = m_iGem1 = m_jGem1 = -1;
	m_pageGem2 = m_iGem2 = m_jGem2 = -1;
	m_pageGem3 = m_iGem3 = m_jGem3 = -1;
	m_pageGem4 = m_iGem4 = m_jGem4 = -1;
	m_pageGem5 = m_iGem5 = m_jGem5 = -1;
	moneyRequired = 0;

	m_pLbRate->SetText("");
	m_plbMoney->SetText("");

	m_pFrmThis->Hide();
	unguard;
}

CUIForm_Gem::~CUIForm_Gem(void)
{
	guard;

	unguard;
}
void CUIForm_Gem::Show()
{
	guard;
	//PlayerFailOperate(R(功能未开放));
	//return;
	if (g_layerMain)
	{
		//if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
		//	g_layerMain->m_fromPlayerItems->Hide();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
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
	m_gemItem1 = ItemID_CreateInvalid();
	m_gemItem2 = ItemID_CreateInvalid();
	m_gemItem3 = ItemID_CreateInvalid();
	m_gemItem4 = ItemID_CreateInvalid();
	m_gemItem5 = ItemID_CreateInvalid();
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}
void CUIForm_Gem::Hide()
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}
void CUIForm_Gem::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}
void CUIForm_Gem::OnBtnOKDown(RtwWidget* sender, void*)
{
	guard;
	//单击合成的事件处理
	SItemID item[5];
	CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);

	if (ItemID_IsValid(m_gemItem1) &&
		ItemID_IsValid(m_gemItem2) &&
		ItemID_IsValid(m_gemItem3) &&
		ItemID_IsValid(m_gemItem4) &&
		ItemID_IsValid(m_gemItem5))
	{

		if (pBag->IsFull())
		{
			PlayerFailOperate(R(MSG_ITEM_BAGHASFULL));
			return;
		}
		if (pBag->GetMoney() < moneyRequired)
		{
			PlayerFailOperate(R(MSG_ITEM_NEEDEDMONEYNOTENOUGH));
			return;
		}


		if (ItemIsGemArmor3(m_gemItem1) || ItemIsGemWeapon3(m_gemItem1)||
			ItemIsGemArmor3(m_gemItem2) || ItemIsGemWeapon3(m_gemItem2)||
			ItemIsGemArmor3(m_gemItem3) || ItemIsGemWeapon3(m_gemItem3)||
			ItemIsGemArmor3(m_gemItem4) || ItemIsGemWeapon3(m_gemItem4)||
			ItemIsGemArmor3(m_gemItem5) || ItemIsGemWeapon3(m_gemItem5))
		{
			PlayerFailOperate(R(MSG_COMPOSE_JEWELISHIGHEST));
			return;
		}
		if (!(ItemIsGemArmor(m_gemItem1) && 
			ItemIsGemArmor(m_gemItem2) && 
			ItemIsGemArmor(m_gemItem3) && 
			ItemIsGemArmor(m_gemItem4) && 
			ItemIsGemArmor(m_gemItem5)))
		{
			if (!(ItemIsGemWeapon(m_gemItem1) && 
				ItemIsGemWeapon(m_gemItem2) && 
				ItemIsGemWeapon(m_gemItem3) && 
				ItemIsGemWeapon(m_gemItem4) && 
				ItemIsGemWeapon(m_gemItem5)))
			{
				PlayerFailOperate(R(MSG_COMPOSE_JEWELCOMPOSEWRONG));
				return;
			}
		}
		if (ItemIsGemArmor(m_gemItem1) &&
			ItemIsGemArmor(m_gemItem2) &&
			ItemIsGemArmor(m_gemItem3) &&
			ItemIsGemArmor(m_gemItem4) &&
			ItemIsGemArmor(m_gemItem5) )
		{
			if (!(ItemIsGemArmor1(m_gemItem1) &&
				ItemIsGemArmor1(m_gemItem2) && 
				ItemIsGemArmor1(m_gemItem3) && 
				ItemIsGemArmor1(m_gemItem4) && 
				ItemIsGemArmor1(m_gemItem5)))
			{
				if (!(ItemIsGemArmor2(m_gemItem1) && 
					ItemIsGemArmor2(m_gemItem2) && 
					ItemIsGemArmor2(m_gemItem3) && 
					ItemIsGemArmor2(m_gemItem4) && 
					ItemIsGemArmor2(m_gemItem5)))
				{
					PlayerFailOperate(R(MSG_COMPOSE_JEWELCOMPOSEWRONG));
					return;
				}
			}
		}
		if (ItemIsGemWeapon(m_gemItem1) &&
			ItemIsGemWeapon(m_gemItem2) &&
			ItemIsGemWeapon(m_gemItem3) &&
			ItemIsGemWeapon(m_gemItem4) &&
			ItemIsGemWeapon(m_gemItem5) )
		{
			if (!(ItemIsGemWeapon1(m_gemItem1) && 
				ItemIsGemWeapon1(m_gemItem2) && 
				ItemIsGemWeapon1(m_gemItem3) && 
				ItemIsGemWeapon1(m_gemItem4) && 
				ItemIsGemWeapon1(m_gemItem5)))
			{
				if (!(ItemIsGemWeapon2(m_gemItem1) &&
					ItemIsGemWeapon2(m_gemItem2) &&
					ItemIsGemWeapon2(m_gemItem3) &&
					ItemIsGemWeapon2(m_gemItem4) &&
					ItemIsGemWeapon2(m_gemItem5)))
				{
					PlayerFailOperate(R(MSG_COMPOSE_JEWELCOMPOSEWRONG));
					return;
				}
			}
		}

		int nHaveCount = 0;
		if (GetWorld()->m_pPlayer->mItem.m_Bag.FindType(CItemManager::GetIndexFromType(m_gemItem1.type), ItemColor_All, &nHaveCount))
		{
			if (nHaveCount < 5)
			{
				return;
			}else
			{
				CItemNetHelper::NetSend_c2r_fuse_gem(m_gemItem1.id);
				m_gemItem1 = ItemID_CreateInvalid();
				m_gemItem2 = ItemID_CreateInvalid();
				m_gemItem3 = ItemID_CreateInvalid();
				m_gemItem4 = ItemID_CreateInvalid();
				m_gemItem5 = ItemID_CreateInvalid();
				g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
				return;
			}
		}

		//CItemNetHelper::NetSend_c2r_charm_item();测试Client与Server的道具栏是否一致
		m_btnOK->Disable();
	}
	else
	{
		PlayerFailOperate(R(MSG_COMPOSE_NOTENOUGHSTONE));
		return;
	}
	unguard;
}
bool CUIForm_Gem::OnBagToGem1(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);
	if (ItemID_IsValid(item))
	{
		if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem5)
		{
			if(ItemIsGem(item.type))//此处添写宝石的类型判断
			{	
				if(ItemID_IsValid(m_gemItem1))
					return false;
				sender->ModifyFlags(wfHint, 0);
				m_gemItem1 = item;

				m_pageGem1 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iGem1 = i;
				m_jGem1 = j;
				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem1,m_iGem1,m_jGem1,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem1,m_jGem1,RtwPixel(0xa0ff0000));
				Refresh();
				return true;
				//m_needMoney = 0;//获得需求金钱
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				return false;
			}
		}else
		{
			if(m_thisItem == item)
				return false;
			m_thisItem = item;
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}
	}else
		return false;
	unguard;
}
bool CUIForm_Gem::OnBagToGem2(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item) )
	{
		if (item != m_gemItem1&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem5)
		{

			if(ItemIsGem(item.type))//此处添写宝石的类型判断
			{	
				if(ItemID_IsValid(m_gemItem2))
					return false;
				sender->ModifyFlags(wfHint, 0);
				m_gemItem2 = item;
				m_pageGem2 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iGem2 = i;
				m_jGem2 = j;

				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem2,m_iGem2,m_jGem2,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem2,m_jGem2,RtwPixel(0xa0ff0000));
				Refresh();
				return true;
				//m_needMoney = 0;//获得需求金钱
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				return false;
			}
		}else
		{
			if(m_thisItem == item)
				return false;
			m_thisItem = item;
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}
	}else
		return false;	
	unguard;
}
bool CUIForm_Gem::OnBagToGem3(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item) )
	{
		if (item != m_gemItem2&& item != m_gemItem1&& item != m_gemItem4&& item != m_gemItem5)
		{
			if(ItemIsGem(item.type))//此处添写宝石的类型判断
			{	
				if(ItemID_IsValid(m_gemItem3))
					return false;
				sender->ModifyFlags(wfHint, 0);
				m_gemItem3 = item;
				m_pageGem3 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iGem3 = i;
				m_jGem3 = j;

				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem3,m_iGem3,m_jGem3,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem3,m_jGem3,RtwPixel(0xa0ff0000));
				Refresh();
				return true;
				//m_needMoney = 0;//获得需求金钱
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				return false;
			}
		}else
		{
			if(m_thisItem == item)
				return false;
			m_thisItem = item;
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}

	}else
		return false;
	unguard;
}
bool CUIForm_Gem::OnBagToGem4(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item))
	{
		if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem1&& item != m_gemItem5)
		{
			if(ItemIsGem(item.type))//此处添写宝石的类型判断
			{	
				if(ItemID_IsValid(m_gemItem4))
					return false;
				sender->ModifyFlags(wfHint, 0);
				m_gemItem4 = item;
				m_pageGem4 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iGem4 = i;
				m_jGem4 = j;

				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem4,m_iGem4,m_jGem4,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem4,m_jGem4,RtwPixel(0xa0ff0000));
				Refresh();
				return true;
				//m_needMoney = 0;//获得需求金钱
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				return false;
			}
		}else
		{
			if(m_thisItem == item)
				return false;
			m_thisItem = item;
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}

	}else
		return false;	
	unguard;
}
bool CUIForm_Gem::OnBagToGem5(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (ItemID_IsValid(item))
	{
		if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem1)
		{
			if(ItemIsGem(item.type))//此处添写宝石的类型判断
			{	
				if(ItemID_IsValid(m_gemItem5))
					return false;
				sender->ModifyFlags(wfHint, 0);
				m_gemItem5 = item;
				m_pageGem5 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
				m_iGem5 = i;
				m_jGem5 = j;

				g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem5,m_iGem5,m_jGem5,true,1.0f,0xb5000000,128,true);
				g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem5,m_jGem5,RtwPixel(0xa0ff0000));
				Refresh();
				return true;
				//m_needMoney = 0;//获得需求金钱
			}
			else 
			{
				PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				return false;
			}
		} 
		else
		{
			if(m_thisItem == item)
				return false;
			m_thisItem = item;
			PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			return false;
		}

	}else
		return false;
	unguard;
}
void CUIForm_Gem::OnDragEndGem(RtwWidget* sender, void*)
{
	guard;
	if(!g_workspace.getMouseHover())
	{
		if (m_pBtnItem[0]==sender)
		{
			m_gemItem1 = ItemID_CreateInvalid();
			m_iGem1 = -1;
			m_jGem1 = -1;
			m_pageGem1 = -1;
		}
		else if (m_pBtnItem[1]==sender)
		{
			m_gemItem2 = ItemID_CreateInvalid();
			m_iGem2 = -1;
			m_jGem2 = -1;
			m_pageGem2 = -1;
		}
		else if (m_pBtnItem[2]==sender)
		{
			m_gemItem3 = ItemID_CreateInvalid();
			m_iGem3 = -1;
			m_jGem3 = -1;
			m_pageGem3 = -1;
		}
		else if (m_pBtnItem[3]==sender)
		{
			m_gemItem4 = ItemID_CreateInvalid();
			m_iGem4 = -1;
			m_jGem4 = -1;
			m_pageGem4 = -1;
		}
		else if (m_pBtnItem[4]==sender)
		{
			m_gemItem5 = ItemID_CreateInvalid();
			m_iGem5 = -1;
			m_jGem5 = -1;
			m_pageGem5 = -1;
		}
		else 
		{
			return;
		}
		sender->ModifyFlags(0, wfHint);
		sender->SetBackgroundImage((RtwImage*)NULL);
	}
	unguard;
}
void CUIForm_Gem::OnMouseRDown(RtwWidget* sender, void*)
{
	guard;
	if (m_pBtnItem[0]==sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_gemItem1);
		m_gemItem1 = ItemID_CreateInvalid();
		m_iGem1 = -1;
		m_jGem1 = -1;
	}
	else if (m_pBtnItem[1]==sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_gemItem2);
		m_gemItem2 = ItemID_CreateInvalid();
		m_iGem2 = -1;
		m_jGem2 = -1;
	}
	else if (m_pBtnItem[2]==sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_gemItem3);
		m_gemItem3 = ItemID_CreateInvalid();
		m_iGem3 = -1;
		m_jGem3 = -1;
	}
	else if (m_pBtnItem[3]==sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_gemItem4);
		m_gemItem4 = ItemID_CreateInvalid();
		m_iGem4 = -1;
		m_jGem4 = -1;
	}
	else if (m_pBtnItem[4]==sender)
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_gemItem5);
		m_gemItem5 = ItemID_CreateInvalid();
		m_iGem5 = -1;
		m_jGem5 = -1;
	}
	else 
	{
		return;
	}
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	unguard;
}
void CUIForm_Gem::OnDragToGem1(RtwWidget* sender, void*)
{
	guard;
	// 拖动事件的处理，以下四个相同
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem5)
			{
				if(ItemIsGem(item.type))//此处添写宝石的类型判断
				{	

					sender->ModifyFlags(wfHint, 0);
					m_gemItem1 = item;

					m_pageGem1 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
					m_iGem1 = i;
					m_jGem1 = j;
					g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem1,m_iGem1,m_jGem1,true,1.0f,0xb5000000,128,true);
					g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem1,m_jGem1,RtwPixel(0xa0ff0000));
					Refresh();
					//m_needMoney = 0;//获得需求金钱
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				}
			}
			else
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			}

		}
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
		}

	}
	unguard;
}
void CUIForm_Gem::OnDragToGem2(RtwWidget* sender, void*)
{
	guard;
	// 拖动事件的处理，以下四个相同
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item) )
		{
			if (item != m_gemItem1&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem5)
			{

				if(ItemIsGem(item.type))//此处添写宝石的类型判断
				{	
					sender->ModifyFlags(wfHint, 0);
					
					m_gemItem2 = item;

					m_pageGem2 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
					m_iGem2 = i;
					m_jGem2 = j;

					g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem2,m_iGem2,m_jGem2,true,1.0f,0xb5000000,128,true);
					g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem2,m_jGem2,RtwPixel(0xa0ff0000));
					Refresh();
					//m_needMoney = 0;//获得需求金钱
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				}
			}
			else
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			}
		}	
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
		}
	}
	unguard;
}
void CUIForm_Gem::OnDragToGem3(RtwWidget* sender, void*)
{
	guard;
	// 拖动事件的处理，以下四个相同
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item) )
		{
			if (item != m_gemItem2&& item != m_gemItem1&& item != m_gemItem4&& item != m_gemItem5)
			{
				if(ItemIsGem(item.type))//此处添写宝石的类型判断
				{	
					sender->ModifyFlags(wfHint, 0);
					
					m_gemItem3 = item;

					m_pageGem3 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
					m_iGem3 = i;
					m_jGem3 = j;

					g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem3,m_iGem3,m_jGem3,true,1.0f,0xb5000000,128,true);
					g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem3,m_jGem3,RtwPixel(0xa0ff0000));
					Refresh();
					//m_needMoney = 0;//获得需求金钱
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				}
			} 
			else
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			}
			
		}
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
		}
	}
	unguard;
}
void CUIForm_Gem::OnDragToGem4(RtwWidget* sender, void*)
{
	guard;
	// 拖动事件的处理，以下四个相同
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem1&& item != m_gemItem5)
			{
				if(ItemIsGem(item.type))//此处添写宝石的类型判断
				{	
					sender->ModifyFlags(wfHint, 0);
					
					m_gemItem4 = item;

					m_pageGem4 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
					m_iGem4 = i;
					m_jGem4 = j;

					g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem4,m_iGem4,m_jGem4,true,1.0f,0xb5000000,128,true);
					g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem4,m_jGem4,RtwPixel(0xa0ff0000));
					Refresh();
					//m_needMoney = 0;//获得需求金钱
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				}
			} 
			else
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			}
			
		}	
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
		}
	}
	unguard;
}
void CUIForm_Gem::OnDragToGem5(RtwWidget* sender, void*)
{
	guard;
	// 拖动事件的处理，以下四个相同
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	const string& WidgetID = pDragSvr->GetID();

	// 是不是道具栏的格子
	int i,j;
	if (g_layerMain->m_fromPlayerItems && g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
		SItemID item = pCon->m_Bag.GetItem(i, j);
		if (ItemID_IsValid(item))
		{
			if (item != m_gemItem2&& item != m_gemItem3&& item != m_gemItem4&& item != m_gemItem1)
			{
				if(ItemIsGem(item.type))//此处添写宝石的类型判断
				{	
					sender->ModifyFlags(wfHint, 0);
					
					m_gemItem5 = item;

					m_pageGem5 = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
					m_iGem5 = i;
					m_jGem5 = j;

					g_layerMain->m_fromPlayerItems->OnSetItemMask(m_pageGem5,m_iGem5,m_jGem5,true,1.0f,0xb5000000,128,true);
					g_workspace.ResetHintText();//g_layerMain->m_fromPlayerItems->setGridsCommonColor(m_iGem5,m_jGem5,RtwPixel(0xa0ff0000));
					Refresh();
					//m_needMoney = 0;//获得需求金钱
				}
				else 
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOTPUTIN));
				}
			} 
			else
			{
				PlayerFailOperate(R(MSG_ITEM_HASPUTIN));
			}

		}	
		else
		{
			PlayerFailOperate(R(MSG_ITEM_ITEMNOEXIST));
		}
	}
	unguard;
}

void CUIForm_Gem::OnDragEndGem1(RtwWidget* sender, void*)
{
	guard;
	
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_gemItem1 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_gemItem1 = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);
			
			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void CUIForm_Gem::OnDragEndGem2(RtwWidget* sender, void*)
{
	guard;
	
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_gemItem2 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_gemItem2 = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void CUIForm_Gem::OnDragEndGem3(RtwWidget* sender, void*)
{
	guard;
	
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_gemItem3 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_gemItem3 = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void CUIForm_Gem::OnDragEndGem4(RtwWidget* sender, void*)
{
	guard;
	
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_gemItem4 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_gemItem4 = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void CUIForm_Gem::OnDragEndGem5(RtwWidget* sender, void*)
{
	guard;
	
	RtwWidget* pClient = g_workspace.GetDragClient();
	if ( !pClient )// 把图标拖到空白处
	{
		m_gemItem5 = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
	}
	else
	{
		int i,j;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(pClient->GetID(), i, j) || 
			g_layerMain->m_fromPlayerItems->Is_Widget_Mission(pClient->GetID(), i, j))
		{
			m_gemItem5 = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);

			g_workspace.ResetHintText();
			Refresh();
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
void CUIForm_Gem::OnPreDraw()
{
	guard;
	SItemBase *pItemClass = NULL;
	GcActor* pPlayer = GetWorld()->m_pPlayer;	

	if (ItemID_IsValid( m_gemItem1))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_gemItem1.type);
		if(!pItemClass)			return;
		m_pBtnItem[0]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));

	}
	else
	{
		m_pBtnItem[0]->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid( m_gemItem2))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_gemItem2.type);
		if(!pItemClass)			return;
		m_pBtnItem[1]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));

	}
	else
	{
		m_pBtnItem[1]->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid( m_gemItem3))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_gemItem3.type);
		if(!pItemClass)			return;
		m_pBtnItem[2]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));

	}
	else
	{
		m_pBtnItem[2]->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid( m_gemItem4))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_gemItem4.type);
		if(!pItemClass)			return;
		m_pBtnItem[3]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));

	}
	else
	{
		m_pBtnItem[3]->SetBackgroundImage((RtwImage*)NULL);
	}

	if (ItemID_IsValid( m_gemItem5))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_gemItem5.type);
		if(!pItemClass)			return;
		m_pBtnItem[4]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pItemClass->icon));

	}
	else
	{
		m_pBtnItem[4]->SetBackgroundImage((RtwImage*)NULL);
	}


	SGem *pGem = (SGem*)pItemClass;
	if (ItemID_IsValid( m_gemItem1))
	{
		//ItemUpdate_GetMoneyRequirement(m_gemItem[0], moneyRequired, GetWorld()->m_pItemManager);需求银币的判断
		m_plbMoney->SetText(rtFormatNumber(pGem->lFuseMoney));
		rt2_sprintf(g_strStaticBuffer,"%d%%",(int)pGem->cFuseRate);
		m_pLbRate->SetText(g_strStaticBuffer);
		if (pPlayer->mItem.GetMoney() < moneyRequired)
			m_plbMoney->SetTextColor(RtwPixel(0xFFFF0000));
		else 
			m_plbMoney->SetTextColor(RtwPixel(0xFFFFFFFF));
	}
	else if (ItemID_IsValid( m_gemItem2))
	{
		//ItemUpdate_GetMoneyRequirement(m_gemItem[1], moneyRequired, GetWorld()->m_pItemManager);需求银币的判断
		m_plbMoney->SetText(rtFormatNumber(pGem->lFuseMoney));
		rt2_sprintf(g_strStaticBuffer,"%d%%",(int)pGem->cFuseRate);
		m_pLbRate->SetText(g_strStaticBuffer);
		if (pPlayer->mItem.GetMoney() < moneyRequired)
			m_plbMoney->SetTextColor(RtwPixel(0xFFFF0000));
		else 
			m_plbMoney->SetTextColor(RtwPixel(0xFFFFFFFF));
	}
	else if (ItemID_IsValid( m_gemItem3))
	{
		//ItemUpdate_GetMoneyRequirement(m_gemItem[2], moneyRequired, GetWorld()->m_pItemManager);需求银币的判断
		m_plbMoney->SetText(rtFormatNumber(pGem->lFuseMoney));
		rt2_sprintf(g_strStaticBuffer,"%d%%",(int)pGem->cFuseRate);
		m_pLbRate->SetText(g_strStaticBuffer);
		if (pPlayer->mItem.GetMoney() < moneyRequired)
			m_plbMoney->SetTextColor(RtwPixel(0xFFFF0000));
		else 
			m_plbMoney->SetTextColor(RtwPixel(0xFFFFFFFF));
	}
	else if (ItemID_IsValid( m_gemItem4))
	{
		//ItemUpdate_GetMoneyRequirement(m_gemItem[3], moneyRequired, GetWorld()->m_pItemManager);需求银币的判断
		m_plbMoney->SetText(rtFormatNumber(pGem->lFuseMoney));
		rt2_sprintf(g_strStaticBuffer,"%d%%",(int)pGem->cFuseRate);
		m_pLbRate->SetText(g_strStaticBuffer);
		if (pPlayer->mItem.GetMoney() < moneyRequired)
			m_plbMoney->SetTextColor(RtwPixel(0xFFFF0000));
		else 
			m_plbMoney->SetTextColor(RtwPixel(0xFFFFFFFF));
	}
	else if (ItemID_IsValid( m_gemItem5))
	{
		//ItemUpdate_GetMoneyRequirement(m_gemItem[4], moneyRequired, GetWorld()->m_pItemManager);需求银币的判断
		m_plbMoney->SetText(rtFormatNumber(pGem->lFuseMoney));
		rt2_sprintf(g_strStaticBuffer,"%d%%",(int)pGem->cFuseRate);
		m_pLbRate->SetText(g_strStaticBuffer);
		if (pPlayer->mItem.GetMoney() < moneyRequired)
			m_plbMoney->SetTextColor(RtwPixel(0xFFFF0000));
		else 
			m_plbMoney->SetTextColor(RtwPixel(0xFFFFFFFF));
	}
	else
	{
		m_pLbRate->SetText("");
		m_plbMoney->SetText("");
	}


	m_btnOK->Enable();
	unguard;
}

bool CUIForm_Gem::IsVisible()
{
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
}

bool CUIForm_Gem::SeekGemPos1()
{
	guard;
	if (!ItemID_IsValid(m_gemItem1))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_gemItem1.id, &m_pageGem1, &m_iGem1, &m_jGem1) )
		return true;
	else 
		return false;

	unguard;
}
bool CUIForm_Gem::SeekGemPos2()
{
	guard;
	if (!ItemID_IsValid(m_gemItem2))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_gemItem2.id, &m_pageGem2, &m_iGem2, &m_jGem2) )
		return true;
	else 
		return false;

	unguard;
}
bool CUIForm_Gem::SeekGemPos3()
{
	guard;
	if (!ItemID_IsValid(m_gemItem3))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_gemItem3.id, &m_pageGem3, &m_iGem3, &m_jGem3) )
		return true;
	else 
		return false;

	unguard;
}
bool CUIForm_Gem::SeekGemPos4()
{
	guard;
	if (!ItemID_IsValid(m_gemItem4))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_gemItem4.id, &m_pageGem4, &m_iGem4, &m_jGem4) )
		return true;
	else 
		return false;

	unguard;
}
bool CUIForm_Gem::SeekGemPos5()
{
	guard;
	if (!ItemID_IsValid(m_gemItem5))
		return false;

	if ( GetWorld()->m_pPlayer->mItem.m_Bag.Find(m_gemItem5.id, &m_pageGem5, &m_iGem5, &m_jGem5) )
		return true;
	else 
		return false;

	unguard;
}

void CUIForm_Gem::OnGetHintText(ui::RtwWidget *sender, void *)
{
	guard;
	SItemID itemID;
	int page, i, j;
	if (m_pBtnItem[0]==sender)
	{
		if ( !ItemID_IsValid(m_gemItem1) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageGem1, m_iGem1, m_jGem1);
		if (itemID.id != m_gemItem1.id)
		{
			if ( !SeekGemPos1() )
				return;
			itemID = m_gemItem1;
		}
		page = m_pageGem1;
		i = m_iGem1;
		j = m_jGem1;
	}
	else if (m_pBtnItem[1]==sender)
	{
		if ( !ItemID_IsValid(m_gemItem2) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageGem2, m_iGem2, m_jGem2);
		if (itemID.id != m_gemItem2.id)
		{
			if ( !SeekGemPos2() )
				return;
			itemID = m_gemItem2;
		}
		page = m_pageGem2;
		i = m_iGem2;
		j = m_jGem2;
	}
	else if (m_pBtnItem[2]==sender)
	{
		if ( !ItemID_IsValid(m_gemItem3) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageGem3, m_iGem3, m_jGem3);
		if (itemID.id != m_gemItem3.id)
		{
			if ( !SeekGemPos3() )
				return;
			itemID = m_gemItem3;
		}
		page = m_pageGem3;
		i = m_iGem3;
		j = m_jGem3;
	}
	else if (m_pBtnItem[3]==sender)
	{
		if ( !ItemID_IsValid(m_gemItem4) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageGem4, m_iGem4, m_jGem4);
		if (itemID.id != m_gemItem4.id)
		{
			if ( !SeekGemPos4() )
				return;
			itemID = m_gemItem4;
		}
		page = m_pageGem4;
		i = m_iGem4;
		j = m_jGem4;
	}
	else if (m_pBtnItem[4]==sender)
	{
		if ( !ItemID_IsValid(m_gemItem5) )
		{
			sender->ModifyFlags(0, wfHint);
			return;
		}

		itemID = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageGem5, m_iGem5, m_jGem5);
		if (itemID.id != m_gemItem5.id)
		{
			if ( !SeekGemPos5() )
				return;
			itemID = m_gemItem5;
		}
		page = m_pageGem5;
		i = m_iGem5;
		j = m_jGem5;
	}
	else 
	{
		return;
	}
	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), page, i, j);
	g_workspace.SetHintText((char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

void CUIForm_Gem::OnCommonItemDeleted(DWORD itemID)
{
	guard;

	if (m_gemItem1.id == itemID)
	{
		ItemID_SetInvalid(m_gemItem1);
		m_pageGem1 = m_iGem1 = m_jGem1 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_gemItem2.id == itemID)
	{
		ItemID_SetInvalid(m_gemItem2);
		m_pageGem2 = m_iGem2 = m_jGem2 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_gemItem3.id == itemID)
	{
		ItemID_SetInvalid(m_gemItem3);
		m_pageGem3 = m_iGem3 = m_jGem3 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_gemItem4.id == itemID)
	{
		ItemID_SetInvalid(m_gemItem4);
		m_pageGem4 = m_iGem4 = m_jGem4 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}
	else if (m_gemItem5.id == itemID)
	{
		ItemID_SetInvalid(m_gemItem5);
		m_pageGem5 = m_iGem5 = m_jGem5 = -1;

		g_workspace.ResetHintText();
		Refresh();
		return;
	}

	unguard;
}
//铁匠
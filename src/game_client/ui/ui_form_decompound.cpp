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
#include "ui_form_timebar.h"

UIFormItemDecompound::UIFormItemDecompound(void)
{
	m_pFrmThis = LOAD_UI("fmseparate");
	m_pFrmThis->EvFocus		+= RTW_CALLBACK(this,UIFormItemDecompound,OnGetFocus);
	m_pFrmThis->EvHide		+= RTW_CALLBACK(this,UIFormItemDecompound,OnHide);
	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);
	m_pDecompoundIcon				= LOAD_UI_T(RtwButton, "fmseparate.btntbc1");
	m_pDecompoundIcon->EvDragEnd	+= RTW_CALLBACK(this, UIFormItemDecompound, OnDragEndDecompound);
	m_pDecompoundIcon->EvMouseRDown	+= RTW_CALLBACK(this, UIFormItemDecompound, OnMouseRDown);
	m_pDecompoundIcon->EvHint		+= RTW_CALLBACK(this, UIFormItemDecompound, OnGetHintItemText);
	((RtwButton*)m_pDecompoundIcon)->SetHoverImage(m_ptrHoverImage);
	m_pStartDecompound = LOAD_UI_T(RtwButton, "fmseparate.btntbc3");
	m_pClose = LOAD_UI_T(RtwButton, "fmseparate.btntbc2");

	m_pStartDecompound->EvLClick += RTW_CALLBACK(this, UIFormItemDecompound, StartDecompound);
	m_pClose->EvLClick += RTW_CALLBACK(this, UIFormItemDecompound, Close);
}

UIFormItemDecompound::~UIFormItemDecompound(void)
{
}
void UIFormItemDecompound::Show()
{
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
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		//if (g_layerMain->m_formItemDecompound->IsVisible())
		//	g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		if (g_layerMain->m_formRecoverPet->GetVisible())
			g_layerMain->m_formRecoverPet->SetVisible(false);
	}


	m_pFrmThis->Show();
	if (ItemID_IsValid(m_ItemID))
	{
		SItemBase *pItemClass = NULL;
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_ItemID.type);
		if(!pItemClass) return;
		RtwImage* pImage = NULL;
		pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
		if(!pImage) return;
		m_pDecompoundIcon->SetBackgroundImage(pImage);
	}else
	{
		m_pDecompoundIcon->SetBackgroundImage((RtwImage*)NULL);
	}
}
void UIFormItemDecompound::Hide()
{
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	ItemID_SetInvalid(m_ItemID);
	m_pFrmThis->Hide();
}
bool UIFormItemDecompound::IsVisible()
{
	return m_pFrmThis->IsVisible();
}
void UIFormItemDecompound::SetItemID( SItemID itemid )
{
	m_pDecompoundIcon->ModifyFlags(wfHint, 0);
	GetPlayer()->mItem.m_Bag.Find(itemid,&m_iPage,&m_iISendItem,&m_iJSendItem);
	m_ItemID = itemid;
	Show();
}
void UIFormItemDecompound::StartDecompound(RtwWidget* sender, void*)
{
	if (!ItemID_IsValid(m_ItemID))
		return;

	SItemBase *pItemClass = NULL;
	pItemClass = GetWorld()->m_pItemManager->GetItem(m_ItemID.type);
	if(!pItemClass) return;

	if (!GetWorld()->m_pPlayer->mItem.OperateItem_CheckUIContent(m_ItemID, IO_Decompose))
		return;

	if (pItemClass->decomposeID == 0)
	{
		PlayerFailOperate(R(MSG_ITEM_CANNOT_DECOMPOSE));
		return;
	}
	else if (m_ItemID.color == ItemColor_White)
	{
		PlayerFailOperate(R(MSG_ITEM_CANNOT_DECOMPOSE_WHITE));
		return;
	}
	if (m_ItemID.cLock != 0)
	{
		PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
		return;
	}

	UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_DECOMPOSE_MAKESURE), UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormItemDecompound, OnDecomposeItem,pFrm);
}

void UIFormItemDecompound::OnDecomposeItem(void*, void*)
{
	guard;
	//added by ldr123
	//begin
	g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_ItemDecompound, 3000);
	g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this,UIFormItemDecompound,RealDecomposeItem);
	m_pStartDecompound->Disable();
	//end
	unguard;
}

void UIFormItemDecompound::Close(RtwWidget* sender, void*)
{
	guard;
	g_layerMain->m_formTimeBar->End(false);
	m_pStartDecompound->Enable();
	Hide();
	unguard;
}

//added by ldr123
//begin
void UIFormItemDecompound::RealDecomposeItem(void*, void*)
{
	guard;
	g_layerMain->m_fromPlayerItems->LockOperate(true);
	CItemNetHelper::NetSend_c2r_decompose_item(m_ItemID.id);
	g_layerMain->m_formTimeBar->End(false);
	g_layerMain->m_fromPlayerItems->OnDisableAllItemMask();
	m_pStartDecompound->Enable();
	unguard;
}
//end
bool UIFormItemDecompound::OnBagToDecompound(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 是不是道具栏的格子
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if (item.cLock != 0)
	{
		PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
		return false;
	}

	m_pDecompoundIcon->ModifyFlags(wfHint, 0);
	m_ItemID = item;
	m_iPage = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
	m_iISendItem = i;
	m_iJSendItem = j;
	g_layerMain->m_fromPlayerItems->OnSetItemMask(m_iPage,m_iISendItem,m_iJSendItem,true,1.0f,0xb5000000,128,true);
	g_workspace.ResetHintText();
	Show();
	return true;
	unguard;
}
void UIFormItemDecompound::OnDragEndDecompound(RtwWidget* sender, void*)
{
	guard;
	if(!g_workspace.getMouseHover())
	{
		g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ItemID);
		m_ItemID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		sender->SetBackgroundImage((RtwImage*)NULL);
		m_iISendItem = -1;
		m_iJSendItem = -1;
		m_iPage = -1;
	}
	unguard;
}
void UIFormItemDecompound::OnMouseRDown(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	g_layerMain->m_fromPlayerItems->OnDisableItemMask(&m_ItemID);
	m_ItemID = ItemID_CreateInvalid();
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	m_iISendItem = -1;
	m_iJSendItem = -1;
	m_iPage = -1;
	unguard;
}
void UIFormItemDecompound::OnGetHintItemText(RtwWidget* sender, void*)
{
	guard;
	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, m_ItemID, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), m_iPage, m_iISendItem, m_iJSendItem);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

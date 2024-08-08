#include "gc_include.h"
#include "ui_form_playeritems.h"
#include "ui_form_shop.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_item_update.h"
#include "ui_form_msg.h"
#include "net_command_union.h"
#include "gc_cursor.h"
#include "ui_form_sale.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "ui_form_textmsg.h"
#include "ui_form_trump.h"
#include "ui_form_decompound.h"
#include "ui_form_timebar.h"
#include "UIForm_Gem.h"
#include "ui_form_char.h"
#include "Timer_Hint.h"
#include "ui_form_pet_include.h"
#include "ui_form_playershop.h"
#include "UIForm_Char.h"

static const int s_NetDelay = 250;//550;

UIFormPlayerItems::UIFormPlayerItems()
	:m_PetLimitTime(USE_PET_BUSY)
{
	guard;
	m_bInMissionPage = false;

	m_pFrmThis = LOAD_UI("fmitem");
	m_pFrmThis->ModifyFlags(wfDragClient, 0);
	m_pTabThis = LOAD_UI_T(RtwTab,"fmitem.tabitem");
	m_pTabThis->SetSelectedItem(0);
	//LOAD_UI("fmitem.panItem")->ModifyFlags(0,wfDragOut);
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);
	m_pFrmThis->EvMouseMove += RTW_CALLBACK(this, UIFormPlayerItems, OnMouseMove);
	m_pFrmThis->EvHide		+= RTW_CALLBACK(this, UIFormPlayerItems, OnThisHide);
	m_pTabThis->EvChangeSelected+=RTW_CALLBACK(this,UIFormPlayerItems,OnTabSelect);
	LOAD_UI("fmitem.tabitem.btnitembag3")->Disable();
	LOAD_UI("fmitem.tabitem.btnitembag4")->Disable();
	LOAD_UI("fmitem.tabitem.btnitembag5")->Disable();
	LOAD_UI("fmitem.tabitem.btnitembag6")->Disable();

    //m_ptrHoverImage = new RtwImage("highlight_button.tga");// change [3/17/2009 tooth.shi]
	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/x_textures/highlight_button.tga");

	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);


	//m_btnParts[CItemContainerBase::WEAPON_1] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnRightHand");
	//m_btnParts[CItemContainerBase::WEAPON_2] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnLeftHand");
	//m_btnParts[CItemContainerBase::BODY] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnCloth");
	//m_btnParts[CItemContainerBase::CHARM] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnCharm");
	//m_btnParts[CItemContainerBase::FOOT] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnShoes");
	//m_btnParts[CItemContainerBase::NECKLACE] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnNecklace");
	//m_btnParts[CItemContainerBase::GLOVE] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnGlove");
	//m_btnParts[CItemContainerBase::RING] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnRing");

	//m_btnParts[CItemContainerBase::CAESTUS] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnCaestus");
	//m_btnParts[CItemContainerBase::RING2] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnRing2");
	//m_btnParts[CItemContainerBase::KITS] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnKits");
	//m_btnParts[CItemContainerBase::TRUMP] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnTrump");
	//m_btnParts[CItemContainerBase::FASHION] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnFashion");
	//m_btnParts[CItemContainerBase::SEALEDBOOK] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnBook");
	//m_btnParts[CItemContainerBase::AEROCRAFT] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnFly");
	//m_btnParts[CItemContainerBase::WEAPON_3] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnLeftHand");
	//m_btnParts[CItemContainerBase::PANTS] = LOAD_UI_T(RtwButton, "fmitem.panItem.btnPants");
	m_btnUpdate				= LOAD_UI_T(RtwButton,"fmitem.fmbt4.btnupdate");
	m_btnShop				= LOAD_UI_T(RtwButton,"fmitem.fmbt4.btnpshop");
	m_btnSplit				= LOAD_UI_T(RtwButton,"fmitem.fmbt4.btnchaifen");
	m_btnBank				= LOAD_UI_T(RtwButton,"fmitem.fmbt4.btnbank");
	m_btnSplit->Disable();
	m_btnBank->Disable();
	m_btnUpdate->EvLClick	+=RTW_CALLBACK(this,UIFormPlayerItems,OnItemUpdate);
	m_btnShop->EvLClick		+=RTW_CALLBACK(this,UIFormPlayerItems,OnLClick_btnShop);
	m_btnSplit->EvLClick	+=RTW_CALLBACK(this,UIFormPlayerItems,OnLClick_btnSplit);

 //   for (int nPart=0; nPart<CItemContainerBase::MAX_EQUIP_PARTS; nPart++)
 //   {
	//	m_btnParts[nPart]->SetHighlightImage(m_ptrHoverImage);
 //   }

	//for (int i=0; i<CItemContainerBase::MAX_EQUIP_PARTS; i++)
	//{
	//	m_btnParts[i]->ModifyFlags(wfDragIn|wfDragClient|wfHint|wfDClick, 0);
	//	m_btnParts[i]->EvLDClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLDClick_Parts);
	//	m_btnParts[i]->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_Parts);
	//	m_btnParts[i]->EvRClick	+= RTW_CALLBACK(this, UIFormPlayerItems, OnLDClick_Parts);
	//	m_btnParts[i]->EvDrag += RTW_CALLBACK(this, UIFormPlayerItems, OnDrag_Parts);
	//	m_btnParts[i]->EvDragEnd += RTW_CALLBACK(this, UIFormPlayerItems, OnDragEnd_Parts);
	//	m_btnParts[i]->EvHint += RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_Parts);
	//}

	m_lbMoney = LOAD_UI("fmitem.lbmoneynum");
	//m_lbPage = LOAD_UI("fmitem.lbpage");
	m_btnTrash = LOAD_UI_T(RtwButton, "fmitem.btnTrash");
    m_btnTrash->ModifyFlags(wfDragClient|wfDragIn|wfHint|wfDClick, 0);
    m_btnTrash->EvDrag += RTW_CALLBACK(this, UIFormPlayerItems, OnDrag_Trash);
	m_btnTrash->Show();
    //m_btnPass = LOAD_UI_T(RtwButton, "fmitem.btnPass");
    //m_btnPass->ModifyFlags(wfDragClient|wfDragIn, 0);//表示往里拖拽
    //m_btnPass->EvDrag += RTW_CALLBACK(this, UIFormPlayerItems, OnDrag_Pass);
	//m_btnPageLeft = LOAD_UI_T(RtwButton, "fmitem.btnitemleft");
	//m_btnPageLeft->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_LeftArrow);
	//m_btnPageRight = LOAD_UI_T(RtwButton, "fmitem.btnitemright");
	//m_btnPageRight->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_RightArrow);
	//m_btnCommonItem = LOAD_UI_T(RtwButton, "fmitem.tabitem.btnnormalitem");
	//m_btnCommonItem->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_CommonButton);
	//m_btnMissionItem = LOAD_UI_T(RtwButton, "fmitem.tabitem.fmitembag7");
	//m_btnMissionItem->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_MissionButton);
//	m_pFrmThis->EvHide += RTW_CALLBACK(this, UIFormPlayerItems, OnClose);
//	LOAD_UI("fmitem.$hide")->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnClose);

	// 创建Inventory的格子
	RtString tmp;
	char strId[32];
#define GRID_SIZE_X		36
#define GRID_SIZE_Y		36
#define GRID_ITEMSIZE_X		31
#define GRID_ITEMSIZE_Y		31
	RtwButton* widget = NULL;
	RtwImage* pImagegrids = g_workspace.getImageFactory()->createImage("ui/x_textures/gridsitem.tga");
	for(int k= 0;k< ITEM_PLAYER_INVENTORY_PAGE;k++)
	{
		tmp.Format("fmitem.tabitem.fmitembag%d",k+1);
		m_frmGridsCommon[k] = LOAD_UI(tmp.c_str());
		RtwRect rc = m_frmGridsCommon[k]->GetFrameRect();
		int left = rc.Left() +2;
		int top = rc.Top() +2;
		for (int i=0; i<ITEM_PLAYER_INVENTORY_LINE; i++)
		{
			for (int j=0; j<ITEM_PLAYER_INVENTORY_COLUMN; j++)
			{

				// 			m_btnGridsCommon[i][j] = (RtwButton*)m_frmGridsCommon->CreateWidget(strId, "Button",
				// 				RtwButton::wfDefault | wfHint | wfDragClient | wfDClick | wfGrabMouseMove);
				// 			m_btnGridsCommon[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// change [3/17/2009 tooth.shi]
				//m_btnGridsCommon[i][j]->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
				rt2_sprintf(strId, "CommlbItem_%02d_%02d", i, j);
				m_lbGridsCommon[k][i][j] = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
				m_lbGridsCommon[k][i][j]->SetName(strId);
				m_frmGridsCommon[k]->AddChild(m_lbGridsCommon[k][i][j]);
				m_lbGridsCommon[k][i][j]->SetBorderColor(0x60c0c0aa);
				m_lbGridsCommon[k][i][j]->SetBorderSize(1);
				m_lbGridsCommon[k][i][j]->SetBackgroundImage(pImagegrids);
				RtwRect sRect;
				sRect.left = (GRID_SIZE_X+4)*j+4;
				sRect.top = (GRID_SIZE_Y+4)*i+4;
				sRect.right = sRect.left + GRID_SIZE_X;
				sRect.bottom = sRect.top + GRID_SIZE_Y;
				m_lbGridsCommon[k][i][j]->Move(sRect);

				rt2_sprintf(strId, "CommItem_%02d_%02d", i, j);
				m_btnGridsCommon[k][i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
				m_btnGridsCommon[k][i][j]->SetName(strId);
				m_frmGridsCommon[k]->AddChild(m_btnGridsCommon[k][i][j]);
				m_btnGridsCommon[k][i][j]->SetHighlightImage(m_ptrHoverImage);

				sRect.left = (GRID_SIZE_Y+4)*j+6;
				sRect.top = (GRID_SIZE_X+4)*i+6;
				sRect.right = sRect.left + GRID_ITEMSIZE_X;
				sRect.bottom = sRect.top + GRID_ITEMSIZE_Y;
				m_btnGridsCommon[k][i][j]->Move(sRect);
				m_btnGridsCommon[k][i][j]->ModifyFlag(wfDragIn|wfHint|wfDClick,0);

				m_btnGridsCommon[k][i][j]->EvLClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_CommonItem);
				m_btnGridsCommon[k][i][j]->EvLDClick += RTW_CALLBACK(this, UIFormPlayerItems, OnLDClick_CommonItem);
				m_btnGridsCommon[k][i][j]->EvMouseRDown	+= RTW_CALLBACK(this, UIFormPlayerItems, OnMouseRDown_CommonButton);
				m_btnGridsCommon[k][i][j]->EvHint += RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_CommonItem);
				m_btnGridsCommon[k][i][j]->EvDrag += RTW_CALLBACK(this, UIFormPlayerItems, OnDrag_CommonItem);
				m_btnGridsCommon[k][i][j]->EvDragOut += RTW_CALLBACK(this, UIFormPlayerItems, OnDragOut_CommonItem);
				m_btnGridsCommon[k][i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormPlayerItems, OnDragEnd_CommonItem);
				m_btnGridsCommon[k][i][j]->EvMouseMove += RTW_CALLBACK(this, UIFormPlayerItems, OnMouseMove);
			}
		}
	}
	m_frmGridsMission = LOAD_UI("fmitem.tabitem.fmitembag7");
	for (int i=0; i<ITEM_PLAYER_INVENTORY_LINE; i++)
	{
		for (int j=0; j<ITEM_PLAYER_INVENTORY_COLUMN; j++)
		{

// 			m_btnGridsMission[i][j] = (RtwButton*)m_frmGridsMission->CreateWidget(strId, "Button", 
// 				RtwButton::wfDefault | wfHint | wfDragClient | wfDClick);
// 			m_btnGridsMission[i][j]->SetTextFormat(RtwStringFormat(alignFar, alignFar));// change [3/17/2009 tooth.shi]
			//m_btnGridsMission[i][j]->MoveResize(left + (GRID_SIZE_X+1)*j, top + (GRID_SIZE_Y+1)*i, GRID_SIZE_X, GRID_SIZE_Y);
			rt2_sprintf(strId, "MisslbItem_%02d_%02d", i, j);
			m_lbGridsMission[i][j] = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
			m_lbGridsMission[i][j]->SetName(strId);
			m_frmGridsMission->AddChild(m_lbGridsMission[i][j]);
			m_lbGridsMission[i][j]->SetBorderColor(0x60c0c0aa);
			m_lbGridsMission[i][j]->SetBorderSize(1);
			m_lbGridsMission[i][j]->SetBackgroundImage(pImagegrids);
			RtwRect sRect;
			sRect.left = (GRID_SIZE_Y+4)*j+4;
			sRect.top = (GRID_SIZE_X+4)*i+4;
			sRect.right = sRect.left + GRID_SIZE_X;
			sRect.bottom = sRect.top + GRID_SIZE_Y;
			m_lbGridsMission[i][j]->Move(sRect);

			rt2_sprintf(strId, "MissItem_%02d_%02d", i, j);
			m_btnGridsMission[i][j] = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			m_btnGridsMission[i][j]->SetName(strId);
			m_frmGridsMission->AddChild(m_btnGridsMission[i][j]);
			m_btnGridsMission[i][j]->SetHighlightImage(m_ptrHoverImage);
			sRect.left = (GRID_SIZE_Y+4)*j+7;
			sRect.top = (GRID_SIZE_X+4)*i+7;
			sRect.right = sRect.left + GRID_ITEMSIZE_X;
			sRect.bottom = sRect.top + GRID_ITEMSIZE_Y;
			m_btnGridsMission[i][j]->Move(sRect);
			m_btnGridsMission[i][j]->ModifyFlag(wfDragIn|wfHint|wfDClick,0);

			m_btnGridsMission[i][j]->EvHint += RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_MissionItem);
			m_btnGridsMission[i][j]->EvRClick += RTW_CALLBACK(this, UIFormPlayerItems, OnRClick_MissionItem);
			m_btnGridsMission[i][j]->EvDrag += RTW_CALLBACK(this, UIFormPlayerItems, OnDrag_MissionItem);
            m_btnGridsMission[i][j]->EvDragEnd += RTW_CALLBACK(this, UIFormPlayerItems, OnDragEnd_MissionItem);
		}
	}
	//strncpy(m_iconUnquip[CItemContainerBase::PANTS], "zb_texture/pants.dds", PATH_STRING_LEN);

	for(int i= 0;i< MAX_LIMIT_ITEM;i++)
	{
		m_vecOperateItemCD.push_back(RT_NEW vector<OPERATEITEMCD*>);
	}
	m_pPlayer = NULL;
	m_pFrmThis->Hide();

	m_bLockOperate = false;
	m_dwLastLockTick = 0;
	m_pickItemOID = 0;
	memset(m_byteBuyInfo,0,sizeof(m_byteBuyInfo));
	m_bPlayItem = true;
	m_bIsSplit = false;
	m_iLastTabSelect= -1;
	m_bIsDrag= false;

	bEnchant = false;

	unguard;
}

UIFormPlayerItems::~UIFormPlayerItems()
{
	guard;
	vector<OPERATEITEMCD*>::iterator it;
	for(int i= 0;i< MAX_LIMIT_ITEM;i++)
	{
		for(it= m_vecOperateItemCD[i]->begin();it!= m_vecOperateItemCD[i]->end();it++)
			DEL_ONE(*it);
		m_vecOperateItemCD[i]->clear();
		DEL_ONE(m_vecOperateItemCD[i]);
	}
	unguard;
}

RtwImage* UIFormPlayerItems::LoadSequenceImage(const string &strFileName)
{
	RtwImage *pImage = g_workspace.getImageFactory()->createImageSequence();

	RtwImage *pTempImage = NULL;


	for (int j=0; j<6; ++j)
	{
		for (int i=0; i<6; ++i)
		{
			pTempImage = g_workspace.getImageFactory()->createImage();
			if (pTempImage->LoadFromFile(strFileName))
			{
				pTempImage->SetSourceRect(RtwRect(i*36+2, j*36+1, (i+1)*36, (j+1)*36+1));
				pTempImage->SetBlend(true);

				((RtwComplexImage*)pImage)->AddImage(pTempImage, 100);
				DROP_TRUI_IMAGE(pTempImage);
			}
		}
	}
	return pImage;
}

void UIFormPlayerItems::OnPreDraw()
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	CItemContainer *pC = &(m_pPlayer->mItem);
	SItemID item;
	SItemBase *pItemClass = NULL;
	//int part;
	CBag *pBag;
	bool bSkipWeapon2 = false;
	char cTmp128[128];
	
	int iSelect= m_pTabThis->GetSelectedItem();
	if(iSelect>= 0 && iSelect< ITEM_PLAYER_INVENTORY_PAGE)
	{
		GetPlayer()->mItem.m_Bag.SetPageIndex(iSelect);
		m_bInMissionPage= false;
	}
	else
	{
		GetPlayer()->mItem.m_MissionBag.SetPageIndex(0);
		m_bInMissionPage= true;
	}

	if (m_bInMissionPage)		pBag = &( m_pPlayer->mItem.m_MissionBag );
	else						pBag = &( m_pPlayer->mItem.m_Bag );

	int pageIndex = pBag->GetPageIndex();
	// 身体上的装备
	//for (part=CItemContainerBase::EQUIP_DEFAULT_PART+1; part<CItemContainerBase::MAX_EQUIP_PARTS; part++)
	//{
	//	if (part==CItemContainerBase::WEAPON_2)
	//		continue;

	//	item = pC->GetEquipItem((CItemContainerBase::EEquipParts)part, true);
	//	if (ItemID_IsValid(item))
	//	{
	//		m_btnParts[part]->ModifyFlags(wfDragServer|wfHint, 0);
	//		pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
	//		if (pItemClass && strlen(pItemClass->icon) != 0)
	//		{
	//			//if (part==CItemContainerBase::WEAPON_1 && ((SWeapon*)pItemClass)->bBothHands)
	//			//{// 双手武器,右手显示为虚的
	//			//	//bSkipWeapon2 = true;
	//			//	////RtwImage* pImage1 = new RtwImage(pItemClass->icon);// change [3/17/2009 tooth.shi]
	//			//	//RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
	//			//	//m_btnParts[CItemContainerBase::WEAPON_1]->SetBackgroundImage(pImage1);

	//			//	////RtwImage* pImage2 = new RtwImage(pItemClass->icon);
	//			//	//RtwImage* pImage2 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
	//			//	//m_btnParts[CItemContainerBase::WEAPON_2]->SetBackgroundImage(pImage2);
	//			//	//pImage2->SetColor(0xff7f7f7f);
	//			//	//m_btnParts[CItemContainerBase::WEAPON_2]->EnableAlphaEffect(true);
	//			//	//m_btnParts[CItemContainerBase::WEAPON_2]->SetAlphaRef(128);//max
	//			//}
	//			//else 
	//			//{
	//			//	//RtwImage* pImage = new RtwImage(pItemClass->icon);
	//			//	RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
	//			//	m_btnParts[part]->SetBackgroundImage(pImage);
	//			//}

	//			RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
	//			m_btnParts[part]->SetBackgroundImage(pImage);
	//			switch(item.color)
	//			{
	//			case ItemColor_White:
	//				m_btnParts[part]->SetMaskImage((RtwImage*)NULL);
	//				break;
	//			case ItemColor_Green:
	//				{
	//					RtwImage* m_ptrHoverGreenImage = LoadSequenceImage("ui/ui_texture/Pic_range_green.dds");
	//					if (m_ptrHoverGreenImage)
	//					{
	//						m_btnParts[part]->SetMaskImage(m_ptrHoverGreenImage);
	//					}
	//					break;
	//				}
	//			case ItemColor_Blue:
	//				{
	//					RtwImage* m_ptrHoverBlueImage = LoadSequenceImage("ui/ui_texture/Pic_range_blue.dds");
	//					if (m_ptrHoverBlueImage)
	//					{
	//						m_btnParts[part]->SetMaskImage(m_ptrHoverBlueImage);
	//					}
	//					break;
	//				}
	//			case ItemColor_Golden:
	//				{
	//					RtwImage* m_ptrHoverGoldImage = LoadSequenceImage("ui/ui_texture/Pic_range_golden.dds");
	//					if (m_ptrHoverGoldImage)
	//					{
	//						m_btnParts[part]->SetMaskImage(m_ptrHoverGoldImage);
	//					}
	//					break;
	//				}
	//			default:
	//				break;
	//			}
	//		}

	//		// 如果道具无效，做一个效果
	//		if (!pC->IsEquip((CItemContainerBase::EEquipParts)part))
	//		{
	//			m_btnParts[part]->EnableAlphaEffect(true);
	//			m_btnParts[part]->SetAlphaRef(128);
	//			if (part==CItemContainerBase::WEAPON_1 && ((SWeapon*)pItemClass)->bBothHands)
	//			{// 双手武器,右手显示为虚的
	//				//if (m_btnParts[CItemContainerBase::WEAPON_2])
	//				//{
	//				//	m_btnParts[CItemContainerBase::WEAPON_2]->EnableAlphaEffect(true);
	//				//	m_btnParts[CItemContainerBase::WEAPON_2]->SetAlphaRef(128);//max
	//				//}
	//			}
	//		}
	//		else 
	//		{
	//			m_btnParts[part]->EnableAlphaEffect(false);
	//			m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	//			m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	//			m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	//			m_ptrHoverImage->getRenderImage()->SetBlend(true);
	//			m_btnParts[part]->SetHoverImage(m_ptrHoverImage);
	//		}
	//	}
	//	else
	//	{
	//		m_btnParts[part]->ModifyFlags(0, wfDragServer|wfHint);

	//		//RtwImage* pImage = new RtwImage(m_iconUnquip[part]);
	//		m_btnParts[part]->SetBackgroundImage((RtwImage*)NULL);
	//		m_btnParts[part]->SetMaskImage((RtwImage*)NULL);

 //           m_btnParts[part]->EnableAlphaEffect(true);

 //           RtwComplexImage hoverImg;
 //           m_btnParts[part]->SetHoverImage(hoverImg);
	//	}
	//}

	// 行囊
	for (int i=0; i<ITEM_PLAYER_INVENTORY_LINE; i++)
		for (int j=0; j<ITEM_PLAYER_INVENTORY_COLUMN; j++)
		{
			item = pBag->GetItem(pBag->GetPageIndex(), i, j);
			char *szImage;

			if ( ItemID_IsValid(item) && pBag->GetVisible(pageIndex, i, j) )
			{
				if (m_bInMissionPage)
					m_btnGridsMission[i][j]->ModifyFlags(wfDragOut|wfDragServer|wfHint, 0);
				else
					m_btnGridsCommon[iSelect][i][j]->ModifyFlags(wfDragOut|wfDragServer|wfHint, 0);
				pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
				if (!pItemClass)
				{
					RtCoreLog().Info("无法找到道具，可能是因为本地数据与服务器不一致，请先升级客户端\n");
					continue;
					CHECKEX("无法找到道具，可能是因为本地数据与服务器不一致，请先升级客户端\n");
				}
				szImage = pItemClass->icon;

				if (m_bInMissionPage)
				{
					//RtwImage* pImage = new RtwImage(szImage);
					RtwImage* pImage = g_workspace.getImageFactory()->createImage(szImage);
					m_btnGridsMission[i][j]->SetBackgroundImage(pImage);

					if (ItemCanPile(item))
					{
						rt2_sprintf(cTmp128, "%d", item.count);
						m_btnGridsMission[i][j]->SetText(cTmp128);
						m_btnGridsMission[i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsMission[i][j]->SetText("");
					}


					m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
					m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
					m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
					m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsMission[i][j]->SetHoverImage(m_ptrHoverImage);
				}
				else
				{
					//RtwImage* pImage = new RtwImage(szImage);
					RtwImage* pImage = g_workspace.getImageFactory()->createImage(szImage);
					m_btnGridsCommon[iSelect][i][j]->SetBackgroundImage(pImage);

					if (ItemCanPile(item))
					{
						rt2_sprintf(cTmp128, "%d", item.count);
						m_btnGridsCommon[iSelect][i][j]->SetText(cTmp128);
						m_btnGridsCommon[iSelect][i][j]->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
					}
					else 
					{
						m_btnGridsCommon[iSelect][i][j]->SetText("");
					}
					//任务道具的序列图
					if (ItemIsOther_Task(item.type))
					{
						RtwImage* m_ptrHoverTaskImage = LoadSequenceImage("ui/ui_texture/Pic_range_task.dds");
						if (m_ptrHoverTaskImage)
						{
							m_btnGridsCommon[iSelect][i][j]->SetMaskImage(m_ptrHoverTaskImage);
						}
					}//绿装，蓝装，金装道具的序列图
					switch(item.color)
					{
					case ItemColor_White:
						m_btnGridsCommon[iSelect][i][j]->SetMaskImage((RtwImage*)NULL);
						break;
					case ItemColor_Green:
						{
							RtwImage* m_ptrHoverGreenImage = LoadSequenceImage("ui/ui_texture/Pic_range_green.dds");
							if (m_ptrHoverGreenImage)
							{
								m_btnGridsCommon[iSelect][i][j]->SetMaskImage(m_ptrHoverGreenImage);
							}
							break;
						}
					case ItemColor_Blue:
						{
							RtwImage* m_ptrHoverBlueImage = LoadSequenceImage("ui/ui_texture/Pic_range_blue.dds");
							if (m_ptrHoverBlueImage)
							{
								m_btnGridsCommon[iSelect][i][j]->SetMaskImage(m_ptrHoverBlueImage);
							}
							break;
						}
					case ItemColor_Golden:
						{
							RtwImage* m_ptrHoverGoldImage = LoadSequenceImage("ui/ui_texture/Pic_range_golden.dds");
							if (m_ptrHoverGoldImage)
							{
								m_btnGridsCommon[iSelect][i][j]->SetMaskImage(m_ptrHoverGoldImage);
							}
							break;
						}
					default:
						break;
					}
                    m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
                    m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
                    m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
                    m_ptrHoverImage->getRenderImage()->SetBlend(true);
					m_btnGridsCommon[iSelect][i][j]->SetHoverImage(m_ptrHoverImage);
				}
			}
			else
			{
				if (m_bInMissionPage)
				{
					m_btnGridsMission[i][j]->ModifyFlags(0, wfDragServer|wfHint);
					//m_btnGridsMission[i][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsMission[i][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsMission[i][j]->SetText("");

                    RtwComplexImage hoverImg;
                    m_btnGridsMission[i][j]->SetHoverImage(hoverImg);
				}
				else
				{
					m_btnGridsCommon[iSelect][i][j]->ModifyFlags(0, wfDragOut|wfDragServer|wfHint);
					//m_btnGridsCommon[i][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsCommon[iSelect][i][j]->SetBackgroundImage((RtwImage*)NULL);
					m_btnGridsCommon[iSelect][i][j]->SetMaskImage((RtwImage*)NULL);
					m_btnGridsCommon[iSelect][i][j]->SetText("");

                    RtwComplexImage hoverImg;
                    m_btnGridsCommon[iSelect][i][j]->SetHoverImage(hoverImg);
				}
			}
		}

	// 页码
	//int iPage = pBag->GetPageIndex() +1;
	//int countPage = pBag->GetPageCount();
	char szTmp[64];
	//snprintf(szTmp, 63, "%d/%d", iPage, countPage);
 //   szTmp[63] = 0;
	//m_lbPage->SetText(szTmp);

	// 金钱
	int money = pC->m_Bag.GetMoney();
	rt2_snprintf(szTmp, 64, "%d", money );
    //szTmp[63] = 0;
	m_lbMoney->SetText(szTmp);

	unguard;
}

void UIFormPlayerItems::Show()
{
	guard;

	if (g_layerMain)
	{
		if (g_layerMain->m_formBank->IsVisible())
		{
			PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
			return;
		}
		//else if (g_layerMain->m_formPShop->IsVisible())
		//{
		//	PlayerFailOperate(R(MSG_ITEM_CLOSE_PSHOP));
		//	return;
		//}
		//else if (g_layerMain->m_formShop->IsVisible())
		//{
		//	PlayerFailOperate(R(MSG_ITEM_CLOSE_SHOP));
		//	return;
		//}
	}

	if ( !m_pPlayer)
	{
		m_pPlayer = GetWorld()->m_pPlayer;
		if (!m_pPlayer)			return;
	}
	OnPreDraw();

	m_pFrmThis->Show();

	unguard;
}

bool UIFormPlayerItems::IsShowing()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormPlayerItems::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormPlayerItems::Refresh(bool bRefreshRelated/* = false*/)
{
	guard;

	if ( IsShowing() )		Show();

	if (bRefreshRelated)
	{
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Refresh();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石融合
			g_layerMain->m_formGem->Refresh();
		if (g_layerMain->m_formShop->IsVisible())			// NPC商店
			g_layerMain->m_formShop->Refresh();
		//if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
		//	g_layerMain->m_formPShop->Show();
		//if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
		//	g_layerMain->m_formLookPlayerShop->Refresh();
		//if (g_layerMain->m_formTrade->IsShowing())			// 交易
		//	g_layerMain->m_formTrade->Refresh();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Show(g_layerMain->m_formBank->m_bStore, g_layerMain->m_formBank->m_npcID);
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Refresh();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Refresh();
	}

	unguard;
}

void UIFormPlayerItems::Run(unsigned long dwMillisecond)
{
	guard;

	int iSelect= m_pTabThis->GetSelectedItem();
	vector<OPERATEITEMCD*>::iterator it;
	for(int i= 0;i< MAX_LIMIT_ITEM;i++)
	{
		for(it= m_vecOperateItemCD[i]->begin();it!= m_vecOperateItemCD[i]->end();it++)
		{
			if(!(*it))
			{
				m_vecOperateItemCD[i]->clear();
				break;
			}
			if ((*it)->m_dwLimitTime <= 0)
				return;
			if((*it)->m_dwOperateTime!= 0)
			{
				if (!m_pPlayer->mItem.m_Bag._IsValidIndex((*it)->m_iPage, (*it)->m_iI, (*it)->m_iJ))
				{
					ERR("[UIFormPlayerItems::Run] HP Active page, i, j is not right\n");
					(*it)->m_dwOperateTime = 0;
					return;
				}

				float fRate = 1.0 - (float)(rtGetMilliseconds()- (*it)->m_dwOperateTime)/ (*it)->m_dwLimitTime;
				if (fRate> 1.0f || fRate< 0.f)
				{
					(*it)->m_dwOperateTime= 0;
					m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->SetMaskValue(0);
					if ((*it)->m_pBtnShortcut)
					{
						(*it)->m_pBtnShortcut->SetMaskValue(0);
					}
					DEL_ONE((*it));
				}else
				{
					if ((*it)->m_pBtnShortcut)
					{
						(*it)->m_pBtnShortcut->SetMaskColor(RtwPixel(0xb5000000));
						(*it)->m_pBtnShortcut->SetMaskValue(fRate);
					}

					if((*it)->m_iPage!= iSelect)
						continue;
					// Grid
					if (m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->GetBackground() != NULL)
					{
						m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->SetMaskColor(RtwPixel(0xb5000000));
						m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->SetMaskValue(fRate);
					}else 
					{
						m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->SetMaskValue(0.f);
					}
				}
			}else
			{
				DEL_ONE((*it));
			}
		}
	}
	//if (m_dwPetOperateTime!=0)
	//{
	//	if (!m_pPlayer->mItem.m_Bag._IsValidIndex(m_iPetActivePage, m_iPetActiveI, m_iPetActiveJ))
	//	{
	//		ERR("[UIFormPlayerItems::Run] Pet Active page, i, j is not right\n");
	//		m_dwPetOperateTime = 0;
	//		return;
	//	}

	//	float fRate = 1.0 - (rtGetMilliseconds() - m_dwPetOperateTime) * 1.0f / m_PetLimitTime;
	//	if (m_pBtnPetShortcut)
	//	{
	//		m_pBtnPetShortcut->SetMaskColor(RtwPixel(0xb5000000));
	//		m_pBtnPetShortcut->SetMaskValue(fRate);
	//	}
	//	if (fRate>1.0f || fRate<0.f)
	//	{
	//		m_dwPetOperateTime = 0;
	//		m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->EnableAlphaEffect(false);
	//		m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->SetMaskValue(0);
	//		if (m_pBtnPetShortcut)
	//		{
	//			m_pBtnPetShortcut->EnableAlphaEffect(false);
	//		}
	//		return;
	//	}

	//	if (m_PetLimitTime <= 0)
	//		return;

	//	// Grid
	//	if (m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->GetBackground() != NULL)
	//	{
	//		if ( (m_iPetActivePage != m_pPlayer->mItem.m_Bag.GetPageIndex()) || IsInMissionPage())
	//		{
	//			m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->EnableAlphaEffect(false);
	//			m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->SetMaskValue(0.f);
	//		}
	//		else 
	//		{
	//			m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->EnableAlphaEffect(true);
	//			//m_btnGridsCommon[m_iPetActiveI][m_iPetActiveJ]->SetAlphaRef(fRate*255);
	//			m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->SetMaskColor(RtwPixel(0xb5000000));
	//			m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->SetMaskValue(fRate);
	//		}
	//	}
	//	else 
	//	{
	//		m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->EnableAlphaEffect(false);
	//		m_btnGridsCommon[iSelect][m_iPetActiveI][m_iPetActiveJ]->SetMaskValue(0.f);
	//	}

	//	// Shortcut
	//	if (m_pBtnPetShortcut)
	//	{
	//		m_pBtnPetShortcut->EnableAlphaEffect(true);
	//		//m_pBtnPetShortcut->SetAlphaRef(fRate*255);
	//	}
	//}
	unguard;
}

//void UIFormPlayerItems::OnLClick_CommonButton(RtwWidget* sender, void*)
//{
//	guard;
//	m_bInMissionPage = false;
//	g_workspace.ResetHintText();
//	Show();
//	unguard;
//}

//void UIFormPlayerItems::OnLClick_MissionButton(RtwWidget* sender, void*)
//{
//	guard;
//	m_bInMissionPage = true;
//	g_workspace.ResetHintText();
//	Show();
//	unguard;
//}

//added by ldr123 at 2010 04 16
//发送消息到服务器实现(超链接)
//begin
void UIFormPlayerItems::ItemHyperLinkSend(DWORD itemTag, DWORD itemId, DWORD playerID, const std::string &strName, RtwPixel color)
{
	GetWorld()->m_Chat.GetEditChat().AddHyperLinkText(hlinkItem, itemTag, itemId, playerID, strName, color);
}
//end

void UIFormPlayerItems::OnLClick_CommonItem(RtwWidget* sender, void*)
{
	guard;

	m_iLastTabSelect= m_pTabThis->GetSelectedItem();
	// 取出 i,j
	const string& WidgetID = sender->GetID();
	int i,j;
	Is_Widget_Common(WidgetID, i, j);
	SItemID item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
	if (!pItemClass)	return;
	CItemContainer *pItemContainer = &(GetWorld()->m_pPlayer->mItem);
	// 判断是否被锁定
	if (OperateIsLocked())
		return;
	if (pItemContainer->m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
		return;
	//如果是按住ctrl并单击,则是超链接
	bool bKeyShift = (GetKeyState(VK_SHIFT)&0xFF00)!=0;
	if (bKeyShift)
	{
		const RtwPixel GREENCOLOR	= 0xff30e512;
		const RtwPixel BLUECOLOR	= 0xff00aaff;
		const RtwPixel GOLDENCOLOR	= 0xffffd851;
		const RtwPixel WHITECOLOR	= 0xffffffff;
		RtwPixel color = WHITECOLOR;

		if ( ItemIsGreen(item) )			color = GREENCOLOR;
		else if ( ItemIsBlue(item) )		color = BLUECOLOR;
		else if ( ItemIsGolden(item) )	color = GOLDENCOLOR;
		else								color = WHITECOLOR;

		ItemHyperLinkSend(0, item.id, GetWorld()->m_pPlayer->ID(), pItemClass->name, color);
		return;
	}

	if(g_layerMain->GetFirstWidget()== g_layerMain->m_formShop->OnGetThisForm() && g_layerMain->m_formShop->m_bIsRepair)
	{
		g_layerMain->m_formShop->OnLClickRepair(item);
	}
	// 判断鼠标类型
	if (((CGameClientFrame*)GetApp())->m_bMouseCapture)
	{
		int CursorTypeId = ((CGameClientFrame*)GetApp())->m_iMouseCaptureID;
		switch (CursorTypeId)
		{
		case cti_Decompose:
			{
				if (!GetWorld()->m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Decompose))
					return;

				if (pItemClass->decomposeID == 0)
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOT_DECOMPOSE));
					return;
				}
				else if (item.color == ItemColor_White)
				{
					PlayerFailOperate(R(MSG_ITEM_CANNOT_DECOMPOSE_WHITE));
					return;
				}
                if (item.cLock != 0)
                {
                    PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
                    return;
                }

				m_IdBackup = item.id;
				m_bBackupMission = false;
				m_pageBackup = m_iLastTabSelect;
				m_iBackup = i;
				m_jBackup = j;
				UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_DECOMPOSE_MAKESURE), UIFormMsg::TYPE_OK_CANCEL);
				pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnDecomposeItem,pFrm);
			}
			((CGameClientFrame*)GetApp())->CancelMouseCapture();
			break;
		case cti_Enchant:
			{
				if (!GetWorld()->m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_EnchantItem))
					return;

				CBag *pBag = &(pItemContainer->m_Bag);
				int EquipmentID = item.id;
				int ScrollID = m_IdBackup;
				int EquipmentPosPage = pItemContainer->m_Bag.GetPageIndex(), EquipmentPosI = i, EquipmentPosJ = j;
				int ScrollPosPage, ScrollPosI, ScrollPosJ;
				if (!pBag->Find(ScrollID, &ScrollPosPage, &ScrollPosI, &ScrollPosJ))
					return;

				SItemID ScrollItem = pBag->GetItem(ScrollPosPage, ScrollPosI, ScrollPosJ);
				SItemID EquipmentItem = pItemContainer->m_Bag.GetItem(EquipmentPosPage, EquipmentPosI, EquipmentPosJ);
				if (ScrollItem.id == EquipmentItem.id)
					return;
				SItemBase* pClassEquipment = GetWorld()->m_pItemManager->GetItem(EquipmentItem.type);
				SItemBase* pClassScroll = GetWorld()->m_pItemManager->GetItem(ScrollItem.type);
				CHECK(pClassEquipment && pClassScroll);
				if (!ItemIsWeapon(EquipmentItem) && !ItemIsArmor(EquipmentItem) && !ItemIsShipin(EquipmentItem))
				{
					PlayerFailOperate(R(MSG_FAIL_CANNOTSTRENGTHEN));
					return;
				}
				if (pClassScroll->requirement[ItemRequirement_Level] > pClassEquipment->requirement[ItemRequirement_Level])
				{
					PlayerFailOperate(R(MSG_FAIL_CANNOTSTRENGTHENLOWITEM));
					return;
				}

				m_IdBackup1 = item.id;
				bEnchant = false;

				UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_FUZHOU_JIAQIANG), UIFormMsg::TYPE_OK_CANCEL);
				pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEnchantItem,pFrm);
			}
			((CGameClientFrame*)GetApp())->CancelMouseCapture();
			break;
		case cti_Split:
			{
				break;
			}
		default:
			break;
		}
	}

	unguard;
}

void UIFormPlayerItems::OnLDClick_CommonItem(RtwWidget* sender, void*)
{
	guard;

	m_iLastTabSelect= m_pTabThis->GetSelectedItem();
	//if (!m_pPlayer)
	//	m_pPlayer = GetWorld()->m_pPlayer;
	//int i,j;
	//RtwEventDelegate e(weNone);

	//// 取出 i,j
	//const string& WidgetID = sender->GetID();

	//if (OperateIsLocked())
	//	return;
	//Is_Widget_Common(WidgetID, i, j);
	//int ii = i;
	//e.mouse.x = i;
	//e.mouse.y = j;
	//if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
	//	return;

	////if(g_layerMain->m_formItemUpdate->IsVisible())
	////{
	////	if(g_layerMain->m_formItemUpdate->OnBagToItem(sender,&e))
	////		return;
	////	if(g_layerMain->m_formItemUpdate->OnBagToScroll(sender,&e))
	////		return;
	////	if(g_layerMain->m_formItemUpdate->OnBagToLuckStone(sender,&e))
	////		return;
	////}
 //   DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);

	unguard;
}
void UIFormPlayerItems::OnMouseRDown_CommonButton(RtwWidget* sender, void*)
{
	guard;
	m_iLastTabSelect= m_pTabThis->GetSelectedItem();

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if(bEnchant)
	{
		((CGameClientFrame*)GetApp())->CancelMouseCapture();
		bEnchant= false;
		return;
	}
	// 取出 i,j
	const string& WidgetID = sender->GetID();
	int i,j;
	SItemID item;
	Is_Widget_Common(WidgetID, i, j);

	if (OperateIsLocked())
		return;
	item = m_pPlayer->mItem.m_Bag.GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
		return;

	//if(LOAD_UI("fmplayershop")->IsVisible())
	//{
	g_layerMain->m_formPlayerShop->OnDeleteItem(item);
	//}

	RtwEventDelegate e(weNone);
	e.mouse.x = i;
	e.mouse.y = j;
	if(g_layerMain->GetFirstWidget()== g_layerMain->m_formItemUpdate->OnGetThisForm())
	{
		if(g_layerMain->m_formItemUpdate->OnBagToItem(sender,&e))
			return;
		if(g_layerMain->m_formItemUpdate->OnBagToScroll(sender,&e))
			return;
		if(g_layerMain->m_formItemUpdate->OnBagToLuckStone(sender,&e))
			return;
	}else if (g_layerMain->GetFirstWidget()== g_layerMain->m_formItemFuse->OnGetThisForm())	
	{
		if(g_layerMain->m_formItemFuse->OnBagToFuseItem1(sender,&e))
			return;
		if (g_layerMain->m_formItemFuse->OnBagToFuseItem2(sender,&e))
			return;
	}else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formItemDecompound->OnGetThisForm())
	{
		if(g_layerMain->m_formItemDecompound->OnBagToDecompound(sender,&e))
			return;
	}/*else if(g_layerMain->m_formRecoverPet->GetVisible())
	{
		if(g_layerMain->m_formRecoverPet->OnBagToRecoverPet(sender,&e))
			return;
	}*/else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formGem->OnGetThisForm())
	{
		if(g_layerMain->m_formGem->OnBagToGem1(sender,&e))
			return;
		if(g_layerMain->m_formGem->OnBagToGem2(sender,&e))
			return;
		if(g_layerMain->m_formGem->OnBagToGem3(sender,&e))
			return;
		if(g_layerMain->m_formGem->OnBagToGem4(sender,&e))
			return;
		if(g_layerMain->m_formGem->OnBagToGem5(sender,&e))
			return;
	}else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formPlayerShop->OnGetThisForm())
	{
		g_layerMain->m_formPlayerShop->OnMouseRClickEvent(item);
	}
	else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formShop->OnGetThisForm())
	{
		g_layerMain->m_formShop->OnBagToShop(sender,&e);
	}
	else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formPetAbility->OnGetThisForm())
	{
		if (CItemManager::GetIndexFromType(item.type) == PET_ABILITY_NEED_ITEM1)
			g_layerMain->m_formPetAbility->SetItem1(item);
		else if (CItemManager::GetIndexFromType(item.type) == PET_ABILITY_NEED_ITEM2)
			g_layerMain->m_formPetAbility->SetItem2(item);
		else
			DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	}
	else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formPetMerge->OnGetThisForm())
	{
		if (CItemManager::GetIndexFromType(item.type) == PET_MERGE_NEED_ITEM)
			g_layerMain->m_formPetMerge->SetItem(item);
		else
			DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	}
	else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formPetMergeBeads->OnGetThisForm())
	{
		if (CItemManager::GetIndexFromType(item.type) == PET_BEADS)
			g_layerMain->m_formPetMergeBeads->SetItem(item);
		else
			DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	}
	else if(g_layerMain->GetFirstWidget()== g_layerMain->m_formPetToBeads->OnGetThisForm())
	{
		if (CItemManager::GetIndexFromType(item.type) == PET_TOBEADS_NEED_ITEM1)
			g_layerMain->m_formPetToBeads->SetItem1(item);
		else if (CItemManager::GetIndexFromType(item.type) == PET_TOBEADS_NEED_ITEM2)
			g_layerMain->m_formPetToBeads->SetItem2(item);
		else
			DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	}
	else
	    DoUseCommonBagItem(GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);

	unguard;
}

void UIFormPlayerItems::OnLClick_Parts(RtwWidget *sender, void *)
{
	//if (!m_pPlayer)
	//	m_pPlayer = GetWorld()->m_pPlayer;

	//if (OperateIsLocked())
	//	return;

	//const string& id = sender->GetID();
	//int i = 0;
	//CItemContainerBase::EEquipParts part;
	//Is_Widget_Parts(id, part);

	//SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
	//SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);

	//if (!ItemID_IsValid(item))
	//	return;

	//if (m_pPlayer->mItem.m_Bag.IsFull())
	//{
	//	PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
	//	return;
	//}

	////如果是按住ctrl并单击,则是超链接
	//bool bKeyShift = (GetKeyState(VK_SHIFT)&0xFF00)!=0;
	//if (bKeyShift)
	//{
	//	const RtwPixel GREENCOLOR	= 0xff30e512;
	//	const RtwPixel BLUECOLOR	= 0xff00aaff;
	//	const RtwPixel GOLDENCOLOR	= 0xffffd851;
	//	const RtwPixel WHITECOLOR	= 0xffffffff;
	//	RtwPixel color = WHITECOLOR;

	//	if ( ItemIsGreen(item) )			color = GREENCOLOR;
	//	else if ( ItemIsBlue(item) )		color = BLUECOLOR;
	//	else if ( ItemIsGolden(item) )	color = GOLDENCOLOR;
	//	else								color = WHITECOLOR;

	//	ItemHyperLinkSend(1, item.id, GetWorld()->m_pPlayer->ID(), pItemClass->name, color);
	//	return;
	//}
}

void UIFormPlayerItems::OnLDClick_Parts(RtwWidget* sender, void*)
{
	guard;

	//if (!m_pPlayer)
	//	m_pPlayer = GetWorld()->m_pPlayer;

	//if (OperateIsLocked())
	//	return;

	//const string& id = sender->GetID();
	//int i = 0;
	//CItemContainerBase::EEquipParts part;
	//Is_Widget_Parts(id, part);

	//SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
	//if (!ItemID_IsValid(item))
	//	return;

	//if (m_pPlayer->mItem.m_Bag.IsFull())
	//{
	//	PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
	//	return;
	//}
	//if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UnEquip))
	//{
	//	if (ItemIsTrump(item))
	//	{
	//		SSkillContinueMask scm;
	//		for (int i=TRUMP_SKILL_1;i<TRUMP_SKILL_MAX;i++)
	//		{
	//			CSkillInstance *pSkill = m_pPlayer->m_Skill.FindActiveSkill(item.params[i]);
	//			if (pSkill)
	//			{
	//				scm.wSkillClass = pSkill->m_pAttr->wClass;
	//				scm.resumeTime = pSkill->m_dwResumeTime;
	//				scm.useTime = pSkill->m_dwUserTime;
	//				m_pPlayer->m_Skill.m_listMask.push_back(scm);
	//			}
	//		}
	//	}
	//	CItemNetHelper::NetSend_UnEquipItem(item.id, part);
	//	LockOperate(true);
	//}

	unguard;
}

void UIFormPlayerItems::OnDrag_Parts(RtwWidget* sender, void*)
{
	guard;

	//RtwWidget* pDragSvr = g_workspace.GetDragServer();
	//if (!pDragSvr)		return;
	//if ( !m_pPlayer)	return;

	//if (!m_pPlayer)
	//	m_pPlayer = GetWorld()->m_pPlayer;

	//if (OperateIsLocked())
	//	return;

	//const string& WidgetID = pDragSvr->GetID();
	//CItemContainerBase::EEquipParts part;
	//Is_Widget_Parts(sender->GetID(), part);

	///// 查找DragServer是什么(支持CBag的格子，人物身上的装备，快捷栏)
	//// 是不是包裹中的格子：
	//int i,j;
	//if ( Is_Widget_Common(WidgetID, i, j) )
	//{
	//	SItemID item = m_pPlayer->mItem.m_Bag.GetItem(i, j);
	//	if ( ItemIsWeapon(item) )
	//	{
	//		// 特殊情况要处理
	//		if (part==CItemContainerBase::WEAPON_1)
	//		{
	//		}
	//		else if (part==CItemContainerBase::WEAPON_2)
	//		{
	//			SItemID item1 = m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
	//			if (ItemID_IsValid(item1))
	//			{
	//				SWeapon *pWeaClass1 = (SWeapon *)GetItemMgr()->GetItem(item1.type);
	//				if (pWeaClass1 && pWeaClass1->bBothHands)
	//					part = CItemContainerBase::WEAPON_1;
	//			}
	//			else 
	//			{
	//				part = CItemContainerBase::WEAPON_1;
	//			}
	//		}

	//		if ( !m_pPlayer->mItem.CheckEquip(part, item, m_pPlayer->m_core) )
	//		{
	//			PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL), true, "cannotpickup.wav");
 //               return;
	//		}
	//		if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Equip))
	//		{
	//			m_equipItemOID = item.id;
	//			m_equipPart = part;
	//			m_equipPage = m_pPlayer->mItem.m_Bag.GetPageIndex();
	//			m_equipGridI = i;
	//			m_equipGridJ = j;
	//			if (item.cBind == IB_BindWhenEquip)
	//			{
	//				SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
	//				if (!pItemClass)
	//					return;
	//				char cTmp128[128];
	//				sprintf(cTmp128, R(MSG_EQUIP_WILLBIND), pItemClass->name);
	//				UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
	//				pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEquipItem,pFrm);
	//			}
	//			else 
	//			{
	//				OnEquipItem(NULL, NULL);
	//			}
	//		}
	//	}
	//	else if ( ItemIsArmor(item) || ItemIsShipin(item) )
	//	{
	//		if ( !m_pPlayer->mItem.CheckEquip(part, item, m_pPlayer->m_core) )
	//		{
	//			PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL), true, "cannotpickup.wav");
 //               return;
	//		}
	//		if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Equip))
	//		{
	//			m_equipItemOID = item.id;
	//			m_equipPart = CItemContainerBase::EQUIP_DEFAULT_PART;
	//			m_equipPage = m_pPlayer->mItem.m_Bag.GetPageIndex();
	//			m_equipGridI = i;
	//			m_equipGridJ = j;
	//			if (item.cBind == IB_BindWhenEquip)
	//			{
	//				SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
	//				if (!pItemClass)
	//					return;
	//				char cTmp128[128];
	//				sprintf(cTmp128, R(MSG_EQUIP_WILLBIND), pItemClass->name);
	//				UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
	//				pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEquipItem,pFrm);
	//			}
	//			else 
	//			{
	//				OnEquipItem(NULL, NULL);
	//			}
	//		}
	//	}
	//	else
	//		PlayerFailOperate(R(MSG_ITEM_ZHUANGBEI_NOTFIT));
	//}

	unguard;
}

void UIFormPlayerItems::OnGetHintText_Parts(RtwWidget* sender, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& widgetID = sender->GetID();
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(widgetID, part);
	SItemID itemID = m_pPlayer->mItem.GetEquipItem(part, true);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}


//void UIFormPlayerItems::OnLClick_LeftArrow(void*, void*)
//{
//	guard;
//
//	if (!m_pPlayer)
//		m_pPlayer = GetWorld()->m_pPlayer;
//
//	if (!m_bInMissionPage)
//		m_pPlayer->mItem.m_Bag.MoveToPrePage();
//	else
//		m_pPlayer->mItem.m_MissionBag.MoveToPrePage();
//
//	g_workspace.ResetHintText();
//	Show();
//
//	unguard;
//}
//
//void UIFormPlayerItems::OnLClick_RightArrow(void*, void*)
//{
//	guard;
//
//	if (!m_pPlayer)
//		m_pPlayer = GetWorld()->m_pPlayer;
//
//	if (!m_bInMissionPage)
//		m_pPlayer->mItem.m_Bag.MoveToNextPage();
//	else
//		m_pPlayer->mItem.m_MissionBag.MoveToNextPage();
//
//	g_workspace.ResetHintText();
//	Show();
//
//	unguard;
//}

void UIFormPlayerItems::OnGetHintText_CommonItem(RtwWidget* sender, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& widgetID = sender->GetID();
	int i, j;
	Is_Widget_Common(widgetID, i, j);
	SItemID itemID = m_pPlayer->mItem.m_Bag.GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	int iSelect= m_pTabThis->GetSelectedItem();
	RtString str;
	int page = m_pPlayer->mItem.m_Bag.GetPageIndex();
	str = GenerateItemHtmlHint(nLines, itemID, m_pPlayer, &(m_pPlayer->mItem.m_Bag), page, i, j);

	g_layerMain->m_TimerHint->m_eNowHintFor = WIDGET_ITEAM_SHOW;
	g_layerMain->m_TimerHint->m_wdtHintFor = sender;
	RtwHtmlView* hintWidget= (RtwHtmlView*)GetWorld()->GetHintWidget();
	hintWidget->SetHtmlText(str.c_str());
	sender->SetHint(hintWidget);
	AutoResize(hintWidget,1);

	unguard;
}

void UIFormPlayerItems::OnDrag_CommonItem(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& WidgetID1 = sender->GetID();
	int iTo,jTo;
	Is_Widget_Common(WidgetID1, iTo, jTo);

	if (OperateIsLocked())
	{
		m_iLastTabSelect= -1;
		return;
	}
	if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), iTo, jTo))
	{
		m_iLastTabSelect= -1;
		return;
	}

	const string& WidgetID = pDragSvr->GetID();

	/// 查找DragServer是什么(支持CBag的格子，人物身上的装备，交易栏)
	// 是不是包裹中的格子：
	int i,j;
	if ( Is_Widget_Common(WidgetID, i, j) )
	{
		int iSelect= m_pTabThis->GetSelectedItem();
		if(!GetWorld()->m_bKeyShift)
		{
			SItemID itemFrom;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= iSelect;
			itemFrom = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			if ( !ItemID_IsValid(itemFrom) )
			{
				m_iLastTabSelect= -1;
				return;
			}
			if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
			{
				m_iLastTabSelect= -1;
				return;
			}

			SItemID itemTo = m_pPlayer->mItem.m_Bag.GetItem(iSelect, iTo, jTo);

			if (!m_pPlayer->mItem.OperateItem_CheckUIContent(itemFrom, IO_Move))
			{
				m_iLastTabSelect= -1;
				return;
			}

			CBag *pBag = &(m_pPlayer->mItem.m_Bag);
			int page = pBag->GetPageIndex();
			CItemNetHelper::NetSend_MoveItem(ITEM_CONTAINER_PLAYER_COMMON_BAG, m_iLastTabSelect, i, j, page, iTo, jTo);
			vector<OPERATEITEMCD*>::iterator it;
			for(int ii= 0;ii< MAX_LIMIT_ITEM;ii++)
			{
				for(it= m_vecOperateItemCD[ii]->begin();it!= m_vecOperateItemCD[ii]->end();it++)
				{
					if(!(*it))
					{
						m_vecOperateItemCD[ii]->clear();
						break;
					}
					if(m_iLastTabSelect== (*it)->m_iPage && i== (*it)->m_iI && j== (*it)->m_iJ)
					{
						// 清除原有的技能CD
						m_btnGridsCommon[(*it)->m_iPage][(*it)->m_iI][(*it)->m_iJ]->SetMaskValue(0);

						(*it)->m_iPage= m_iLastTabSelect;
						(*it)->m_iI= iTo;
						(*it)->m_iJ= jTo;
						goto forEnd;
					}
				}
			}
forEnd:		;
		}else
		{
			SItemID itemFrom;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= iSelect;
			itemFrom = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			if ( !ItemID_IsValid(itemFrom) )
			{
				m_iLastTabSelect= -1;
				return;
			}
			if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
			{
				m_iLastTabSelect= -1;
				return;
			}

			SItemID itemTo = m_pPlayer->mItem.m_Bag.GetItem(iSelect, iTo, jTo);
			if (ItemID_IsValid(itemTo))
			{
				m_iLastTabSelect= -1;
				return;
			}

			UIFormTextMsg *pFrm;
			pFrm = UIFormTextMsg::ShowStatic(R(""), R(MSG_ITEM_SPLIT));
			pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEditNumOK,pFrm);

			CBag *pBag = &(m_pPlayer->mItem.m_Bag);
			int page = pBag->GetPageIndex();
			m_byteBuyInfo[0] = m_iLastTabSelect;
			m_byteBuyInfo[1] = i;
			m_byteBuyInfo[2] = j;
			m_byteBuyInfo[3] = iSelect;
			m_byteBuyInfo[4] = iTo;
			m_byteBuyInfo[5] = jTo;
			//CItemNetHelper::NetSend_MoveItem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, page, iTo, jTo);
		}
		//if ( pBag->MoveItem(i, j, iTo, jTo) )
		//{
		//	CItemNetHelper::NetSend_MoveItem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, iTo, jTo);
		//          bool bVisibleFrom = pBag->GetVisible(page, i, j);
		//          bool bVisibleTo = pBag->GetVisible(page, iTo, jTo);
		//          pBag->SetVisible(page, i, j, bVisibleTo);
		//          pBag->SetVisible(page, iTo, jTo, bVisibleFrom);
		//          
		//	g_workspace.ResetHintText();
		//	Show();
		//	RefreshQuickStart();
		if(g_workspace.GetMouseDragWidget() != NULL)
			g_workspace.SetMouseDragWidget(NULL);

		//	return;
		//}
	}

	// 是不是人物的纸娃娃
//	CItemContainerBase::EEquipParts part;
//	if ( Is_Widget_Parts(WidgetID, part) )
//	{
//		SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
//		if ( !ItemID_IsValid(item) )
//		{
//			PlayerFailOperate(R(MSG_ITEM_ZHUANGBEI_NOTFIT));
//			return;
//		}
//		if (m_pPlayer->mItem.m_Bag.IsFull())
//		{
//			PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
//			return;
//		}
//		if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UnEquip))
//		{
//			CItemNetHelper::NetSend_UnEquipItem(item.id, part);
//			LockOperate(true);
////			m_pPlayer->mItem.m_Bag.SetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), iTo, jTo, true);
//		}
//	}

	// 是不是交易窗口的下面的格子
	if (g_layerMain->m_formTrade && g_layerMain->m_formTrade->Is_Widget_Down(WidgetID, i, j) )
	{
		CTrade* pTrade = &(g_layerMain->m_formTrade->m_trade);
		CBag* pBag = &(m_pPlayer->mItem.m_Bag);
		SItemID itemFrom = pTrade->GetTradeBag( &(m_pPlayer->mItem) )->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
		SItemID itemTo = pBag->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), iTo, jTo);

		if (g_layerMain->m_formTrade->m_trade.IsLocked( &(m_pPlayer->mItem) ) )
		{
			g_layerMain->m_formTrade->m_trade.UnLock();
			g_layerMain->m_formTrade->m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
			g_layerMain->m_formTrade->m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
			CItemNetHelper::NetSend_c2r_trade_unlock();
		}
		if ( g_layerMain->m_formTrade->m_trade.TakeOut( &(m_pPlayer->mItem), i, j, pBag->GetPageIndex(), iTo, jTo) )
		{
			int page1, i1, j1;
			if (ItemIsMissionItem(itemFrom))
			{
				if ( m_pPlayer->mItem.m_MissionBag.Find(itemFrom.id, &page1, &i1, &j1) )
					m_pPlayer->mItem.m_MissionBag.SetVisible(page1, i1, j1, true);
			}
			else
			{
				if ( m_pPlayer->mItem.m_Bag.Find(itemFrom.id, &page1, &i1, &j1) )
					m_pPlayer->mItem.m_Bag.SetVisible(page1, i1, j1, true);
			}
		}
		g_workspace.ResetHintText();
		Show();
		g_layerMain->m_formTrade->Refresh();
	}
	m_iLastTabSelect= -1;
	unguard;
}
void UIFormPlayerItems::OnEditNumOK(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent)
{
	guard;
	RtString Name;
	UIFormTextMsg* pForm=(UIFormTextMsg* )vpEvent->GetUser1();
	Name=pForm->GetString();
	long lNum;
	if(Name != "")
		lNum = atol(Name.c_str());
	else
		lNum = 0;
	if(lNum)
	{
		//发送相关信息
		//if(LOAD_UI("fmplayershop")->IsVisible())
		//{
		SItemID item;
		item= GetPlayer()->mItem.m_Bag.GetItem(m_byteBuyInfo[0], m_byteBuyInfo[1], m_byteBuyInfo[2]);
		g_layerMain->m_formPlayerShop->OnDeleteItem(item);
		//}
		CItemNetHelper::NetSend_DispartItem(ITEM_CONTAINER_PLAYER_COMMON_BAG, m_byteBuyInfo[0], m_byteBuyInfo[1], m_byteBuyInfo[2], m_byteBuyInfo[3], m_byteBuyInfo[4], m_byteBuyInfo[5],lNum);
		vector<OPERATEITEMCD*>::iterator it;
		for(int i= 0;i< MAX_LIMIT_ITEM;i++)
		{
			for(it= m_vecOperateItemCD[i]->begin();it!= m_vecOperateItemCD[i]->end();it++)
			{
				if(!(*it))
				{
					m_vecOperateItemCD[i]->clear();
					break;
				}
				if(m_byteBuyInfo[0]== (*it)->m_iPage && m_byteBuyInfo[1]== (*it)->m_iI && m_byteBuyInfo[2]== (*it)->m_iJ)
				{
					OPERATEITEMCD* itemCD= new OPERATEITEMCD();
					itemCD->m_dwLimitTime= (*it)->m_dwLimitTime;
					itemCD->m_dwOperateTime= (*it)->m_dwOperateTime;
					itemCD->m_iPage= m_byteBuyInfo[3];
					itemCD->m_iI= m_byteBuyInfo[4];
					itemCD->m_iJ= m_byteBuyInfo[5];
					itemCD->m_pBtnShortcut= NULL;
					m_vecOperateItemCD[i]->push_back(itemCD);
					return;
				}
			}
		}
	}
	unguard;
}

void UIFormPlayerItems::OnDragEnd_CommonItem(RtwWidget* sender, void* unused)
{
	guard;

	m_bIsDrag= false;

	RtwWidget* pClient = g_workspace.getMouseHover();
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;
	CBag *pBag = &(m_pPlayer->mItem.m_Bag);
	const string& widgetID = sender->GetID();
	int i, j;
	Is_Widget_Common(widgetID, i, j);

	if ( !pClient )// 把图标拖到空白处
	{
        if (OperateIsLocked())
        {
            g_workspace.SetDragClient(NULL);
			m_iLastTabSelect= -1;
			return;
        }
        if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
        {
            g_workspace.SetDragClient(NULL);
			m_iLastTabSelect= -1;
			return;
        }

		SItemID item;
		if(m_iLastTabSelect== -1)
			m_iLastTabSelect= GetWorld()->m_pPlayer->mItem.m_Bag.GetPageIndex();
		item = pBag->GetItem(m_iLastTabSelect, i, j);

		if ( !ItemCanDrop(item) )
		{
			PlayerFailOperate(R(MSG_ITEM_CAN_NOT_THROW));
			m_iLastTabSelect= -1;
			return;
		}
		if (item.cBind == IB_Binded)
		{
			PlayerFailOperate(R(MSG_THROW_BINDED));
			m_iLastTabSelect= -1;
			return;
		}
        if (item.cLock != 0)
        {
            PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
			m_iLastTabSelect= -1;
			return;
        }

		if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Throw))
		{
			m_iLastTabSelect= -1;
			return;
		}

        m_bBackupMission = false;
		m_pageBackup = m_iLastTabSelect;
		m_iBackup = i;
		m_jBackup = j;
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_THROW_MAKE_SURE), UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnThrowItem,pFrm);
		if(g_workspace.GetMouseDragWidget() != NULL)
			g_workspace.SetMouseDragWidget(NULL);
		m_iLastTabSelect= -1;
		return;
	}
	else
	{
		/* gao 2009.12.30
		法宝修练的鼠标拖动控制
		另程序优化，加入一else控制，当执行一步之后就不再去判断其它的
		*/
		const RtwRect rcTrump = LOAD_UI("fmborder.fmInterface.fmProps.btnBaby")->getFrameRect();

		const RtwRect rcClient =	pClient->GetFrameRect();
		const RtwRect rcHotKey =	LOAD_UI("fmhotkey")->GetFrameRect();
		const RtwRect rcHotKey1 =	LOAD_UI("fmshortcut_bar1")->GetFrameRect();
		const RtwRect rcHotKey2 =	LOAD_UI("fmshortcut_bar2")->GetFrameRect();
		const RtwRect rcAuction =	LOAD_UI("fmAuction.panOrder.pansource01.btnskill")->GetFrameRect();
		const RtwRect rcDecompound =	LOAD_UI("fmseparate.btntbc1")->GetFrameRect();

		const RtwRect rcPetAbilityItem1 =	g_layerMain->m_formPetAbility->m_pBtnItem1->GetFrameRect();
		const RtwRect rcPetAbilityItem2 =	g_layerMain->m_formPetAbility->m_pBtnItem2->GetFrameRect();
		const RtwRect rcPetMergeItem	=	g_layerMain->m_formPetMerge->m_pBtnItem->GetFrameRect();
		const RtwRect rcPetToBeadsItem1 =	g_layerMain->m_formPetToBeads->m_pBtnItem1->GetFrameRect();
		const RtwRect rcPetToBeadsItem2 =	g_layerMain->m_formPetToBeads->m_pBtnItem2->GetFrameRect();
		const RtwRect rcPetMergeBeads	=	g_layerMain->m_formPetMergeBeads->m_pBtnItem->GetFrameRect();

		if ( rcHotKey.IsContained(rcClient.left, rcClient.top) 
			|| rcHotKey1.IsContained(rcClient.left, rcClient.top) 
			|| rcHotKey2.IsContained(rcClient.left, rcClient.top))
		{
			bool	bIsInHotKey= false;
			int		iInHotKey = -1;
			RtwRect rcSmallHotKey;
			if(rcHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
				||rcHotKey1.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
				||rcHotKey2.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
			{
				for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
				{
					rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
					if(rcSmallHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
					{
						bIsInHotKey = true;
						iInHotKey = i;
						break;
					}
				}
			}
			if(!bIsInHotKey)
			{
				g_workspace.SetDragClient(NULL);
				m_iLastTabSelect= -1;
				return;
			}
			// 把图标拖到快捷栏中
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			if(!pItemBase)
			{
				m_iLastTabSelect= -1;
				return;
			}
			string strID = "QS_" + sender->GetID();
			// 先进行遍历一下快捷键，是否已经存在此快捷键，如果有，删除原来的 // gao 2010.3.30
			for(int ii= 0;ii< UI_HOT_KEY_COUNT;++ii)
			{
				if(!g_layerMain->m_hotKeyAlias[ii]->GetButton())
					continue;
				DWORD *pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[ii]->GetButton()->GetUserData();
				if(pDWord[1] == itemID.id)
				{
					if(iInHotKey == ii)
					{
						m_iLastTabSelect= -1;
						return;
					}
					else
						g_layerMain->m_hotKeyAlias[ii]->SetButton(NULL);
				}
			}

			RtwButton* pButton = NULL;
			pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strID));
			if (pButton==NULL)
			{
				// 				pButton = (RtwButton*)sender->Clone(NULL, strID);
				// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);// change [3/17/2009 tooth.shi]
				pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton,strID);
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfHint,0);

				pButton->LoadAttributesFromTemplate("IconBorder");

				DWORD *pDword = RT_NEW DWORD[3];
				pDword[0] = UIHotKey_Item;
				int itemCount = 0;
				if (ItemCanPile(itemID))
				{
					int p1st, i1st, j1st;
					m_pPlayer->mItem.m_Bag.FindType( GetWorld()->m_pItemManager->GetIndexFromType(itemID.type), ItemColor_All, &itemCount, &p1st, &i1st, &j1st);
					SItemID item1st = m_pPlayer->mItem.m_Bag.GetItem(p1st, i1st, j1st);
					pDword[1] = item1st.id;
					pDword[2] = item1st.type;
					//pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
					pButton->SetText(rtFormatNumber(itemCount));
				}
				else 
				{
					pDword[1] = itemID.id;
					pDword[2] = itemID.type;
				}
				pButton->SetBorderColor(0xee000000);
				pButton->SetBorderSize(0);
				pButton->SetWidgetSize(SSize(29,32));
				pButton->SetUserData(pDword);
				pButton->SetHint(g_workspace.getDefaultHint());
				pButton->EvClose	+= RTW_CALLBACK(this, UIFormPlayerItems, DeleteQuickStartUserData);
				pButton->EvRClick	+= RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_QuickStart);
				pButton->EvHint		+= RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_QuickStart);

				vector<OPERATEITEMCD*>::iterator it;
				for(int ii= 0;ii< MAX_LIMIT_ITEM;ii++)
				{
					for(it= m_vecOperateItemCD[ii]->begin();it!= m_vecOperateItemCD[ii]->end();it++)
					{
						if(!(*it))
						{
							m_vecOperateItemCD[ii]->clear();
							break;
						}
						if(m_iLastTabSelect== (*it)->m_iPage && i== (*it)->m_iI && j== (*it)->m_iJ)
						{
							(*it)->m_pBtnShortcut= pButton;
							goto forEnd;
						}
					}
				}
forEnd:		;
			}
			g_workspace.SetDragServer(pButton);
			if(g_workspace.getMouseHover())
			{
				if(g_workspace.getMouseHover()->getWidgetType() == wtAliasButton)
				{
					m_iLastTabSelect= -1;
					return;
				}
				//此处 由于当设置一次按钮之后，此处按钮会将原快捷键盖住，特添加此处理 gao 2010.3.30
				if(rcHotKey.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey1.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey2.IsContained(rcClient.left, rcClient.top))
				{
					for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
					{
						rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
						if(rcSmallHotKey.IsContained(rcClient.left, rcClient.top))
						{
							if(g_layerMain->m_hotKeyAlias[i]->GetButton())
							{
								g_layerMain->m_hotKeyAlias[i]->SetButton(pButton);
								break;
							}
						}
					}
				}
			}
			g_workspace.SetDragClient(NULL);
			// 			if(g_workspace.GetMouseDragWidget() != NULL)
			// 				g_workspace.SetMouseDragWidget(NULL);
		}else if ( rcAuction.IsContained(rcClient.left, rcClient.top) )
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			if(!pItemBase)
			{
				m_iLastTabSelect= -1;
				return;
			}
			string strID = "QS_" + sender->GetID();

			if (itemID.cLock != 0)
			{
				PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
				m_iLastTabSelect= -1;
				return;
			}

			RtwButton* pButton = NULL;
			pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strID));
			if (pButton==NULL)
			{
				// 				pButton = (RtwButton*)sender->Clone(NULL, strID);
				// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);// change [3/17/2009 tooth.shi]
				pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton,strID);
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfHint,0);
				pButton->LoadAttributesFromTemplate("IconBorder");
				DWORD *pDword = RT_NEW DWORD[3];
				pDword[0] = UIHotKey_Item;
				int itemCount = 0;
				if (ItemCanPile(itemID))
				{
					int p1st, i1st, j1st;
					m_pPlayer->mItem.m_Bag.FindType( GetWorld()->m_pItemManager->GetIndexFromType(itemID.type), ItemColor_All, &itemCount, &p1st, &i1st, &j1st);
					SItemID item1st = m_pPlayer->mItem.m_Bag.GetItem(p1st, i1st, j1st);
					pDword[1] = item1st.id;
					pDword[2] = item1st.type;
					//pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
					pButton->SetText(rtFormatNumber(itemCount));
				}
				else 
				{
					pDword[1] = itemID.id;
					pDword[2] = itemID.type;
				}
				pButton->SetUserData(pDword);
				pButton->EvHint		+= RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_QuickStart);
			}
			g_workspace.SetDragServer(pButton);

			g_layerMain->m_formSale->SetItemID(itemID);
			g_layerMain->m_formSale->Refresh();
			g_workspace.SetMouseDragWidget(NULL);
		}else if(rcTrump.IsContained(rcClient.left, rcClient.top))	// gao 2009.12.30 此处是新添加
		{
			// 把图标拖到法宝修炼
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);

			if (!ItemID_IsValid(itemID) || !ItemIsTrump(itemID.type))
			{
				g_workspace.SetMouseDragWidget(NULL);
				m_iLastTabSelect= -1;
				return;
			}

			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			if(!pItemBase)	return;
			string strID = "QS_" + sender->GetID();

			//g_layerMain->m_formTrump->SetItemID(itemID);
			//g_layerMain->m_formTrump->Refresh();
			g_workspace.SetMouseDragWidget(NULL);
		}else if (rcDecompound.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			if(!pItemBase)	return;
			g_layerMain->m_formItemDecompound->SetItemID(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetAbility->IsVisible() && rcPetAbilityItem1.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetAbility->SetItem1(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetAbility->IsVisible() && rcPetAbilityItem2.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetAbility->SetItem2(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetMerge->IsVisible() && rcPetMergeItem.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetMerge->SetItem(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetToBeads->IsVisible() && rcPetToBeadsItem1.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetToBeads->SetItem1(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetToBeads->IsVisible() && rcPetToBeadsItem2.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetToBeads->SetItem2(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		else if (g_layerMain->m_formPetMergeBeads->IsVisible() && rcPetMergeBeads.IsContained(rcClient.left, rcClient.top))
		{
			SItemID itemID;
			if(m_iLastTabSelect== -1)
				m_iLastTabSelect= m_pPlayer->mItem.m_Bag.GetPageIndex();
			itemID = m_pPlayer->mItem.m_Bag.GetItem(m_iLastTabSelect, i, j);
			g_layerMain->m_formPetMergeBeads->SetItem(itemID);
			g_workspace.SetMouseDragWidget(NULL);
		}
		/* end */
	}
	unguard;
}

void UIFormPlayerItems::OnDragEnd_MissionItem(RtwWidget* sender, void*)
{
    guard;

    RtwWidget* pClient = g_workspace.GetDragClient();
    if (!m_pPlayer)
        m_pPlayer = GetWorld()->m_pPlayer;
    CBag *pBag = &(m_pPlayer->mItem.m_MissionBag);
    const string& widgetID = sender->GetID();
    int i, j;
    Is_Widget_Mission(widgetID, i, j);

	if (OperateIsLocked())
		return;
	if (m_pPlayer->mItem.m_MissionBag.GetLock(m_pPlayer->mItem.m_MissionBag.GetPageIndex(), i, j))
		return;

    if ( !pClient )// 把图标拖到空白处
    {
        SItemID item = pBag->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);

        if ( !ItemCanDrop(item) )
        {
            PlayerFailOperate(R(MSG_ITEM_CAN_NOT_THROW));
            return;
        }

		if (item.cBind == IB_Binded)
		{
			PlayerFailOperate(R(MSG_THROW_BINDED));
			return;
		}

        if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Throw))
            return;

        m_bBackupMission = true;
		m_pageBackup = m_iLastTabSelect;
        m_iBackup = i;
        m_jBackup = j;
        UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_THROW_MAKE_SURE), UIFormMsg::TYPE_OK_CANCEL);
        pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnThrowItem,pFrm);
    }
    else
    {
        const RtwRect rcClient = pClient->GetFrameRect();
        const RtwRect rcHotKey = LOAD_UI("fmhotkey")->GetFrameRect();
        if ( rcHotKey.IsContained(rcClient.left, rcClient.top) )
        {
            // 把图标拖到快捷栏中
            SItemID itemID = m_pPlayer->mItem.m_MissionBag.GetItem(i, j);
            SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
            int itemCount = itemID.count;
            if(!pItemBase)	return;
            string strID = "QS_" + sender->GetID();

            RtwButton* pButton = NULL;
            pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strID));
            if (pButton==NULL)
            {
//                 pButton = (RtwButton*)sender->Clone(NULL, strID);
//                 pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);// change [3/17/2009 tooth.shi]
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfHint,0);
                pButton->LoadAttributesFromTemplate("IconBorder");

                DWORD *pDword = RT_NEW DWORD[3];
                pDword[0] = UIHotKey_Item;
                pDword[1] = itemID.id;
                pDword[2] = itemID.type;
                //pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
                pButton->SetText(rtFormatNumber(itemCount));
				pButton->SetBorderColor(0xee000000);
				pButton->SetBorderSize(0);
                pButton->SetUserData(pDword);
                pButton->EvClose	+= RTW_CALLBACK(this, UIFormPlayerItems, DeleteQuickStartUserData);
                pButton->EvHint		+= RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_QuickStart);
            }
            g_workspace.SetDragServer(pButton);
        }
    }

    unguard;
}

// 是不是人物的纸娃娃
bool UIFormPlayerItems::Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart)
{
	guard;

	//if (!m_pPlayer)
	//	m_pPlayer = GetWorld()->m_pPlayer;
	//int ii;
	//for (ii=0; ii<CItemContainerBase::MAX_EQUIP_PARTS; ii++)
	//{
	//	if (ii == CItemContainerBase::WEAPON_2)
	//	{
	//		continue;
	//	}
	//	if ( id==m_btnParts[ii]->GetID() )
	//		break;
	//}
	//if (ii<CItemContainerBase::MAX_EQUIP_PARTS)
	//{
	//	outPart = (CItemContainerBase::EEquipParts)ii;
	//	return true;
	//}
	//else
		return false;

	unguard;
}

// 是不是包裹的格子
bool UIFormPlayerItems::Is_Widget_Common(const string& id, int &outI, int &outJ)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	////// 
	// CommItem_02_03
	// MissItem_03_01
	// QS_CommItem_01_03	
	if (strncmp(id.c_str(), "CommItem", 8) == 0)
	{
		string strI = string(&id[9], 2);
		string strJ = string(&id[12], 2);
		outI = atoi(strI.c_str());
		outJ = atoi(strJ.c_str());
		return true;
	}
	else 
		return false;

	unguard;
}

bool UIFormPlayerItems::Is_Widget_Mission(const string& id, int &outI, int &outJ)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	////// 
	// CommItem_02_03
	// MissItem_03_01
	// QS_MissItem_01_03	
	if (strncmp(id.c_str(), "MissItem", 8) == 0)
	{
		string strI = string(&id[9], 2);
		string strJ = string(&id[12], 2);
		outI = atoi(strI.c_str());
		outJ = atoi(strJ.c_str());
		return true;
	}
	else 
		return false;

	unguard;
}

void UIFormPlayerItems::OnDrag_Trash(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if (OperateIsLocked())
		return;

	const string& WidgetID = pDragSvr->GetID();

    CItemContainerBase::EEquipParts part;
	int i,j;
	if ( Is_Widget_Common(WidgetID, i, j) )
	{
		if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
			return;
		CBag *pBag = &(m_pPlayer->mItem.m_Bag);
		SItemID item = pBag->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
		if ( !ItemCanDelete(item) )
		{
			PlayerFailOperate(R(MSG_ITEM_CAN_NOT_DELETE));
			return;
		}

        if (item.cLock != 0)
        {
            PlayerFailOperate(R(MSG_ITEM_PROTECT_STATE));
            return;
        }

		if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Delete))
			return;

		m_bBackupMission = false;
		m_pageBackup = m_iLastTabSelect;
		m_iBackup = i;
		m_jBackup = j;
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_DELETE_MAKE_SUER), UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnDeleteItem,pFrm);
	}
	else if ( Is_Widget_Mission(WidgetID, i, j) )
	{
		if (m_pPlayer->mItem.m_MissionBag.GetLock(m_pPlayer->mItem.m_MissionBag.GetPageIndex(), i, j))
			return;

		CBag *pBag = &(m_pPlayer->mItem.m_MissionBag);
		SItemID item = pBag->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
		if ( !ItemCanDelete(item) )
		{
			PlayerFailOperate(R(MSG_ITEM_CAN_NOT_DELETE));
			return;
		}

		if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Delete))
			return;

		m_bBackupMission = true;
		m_pageBackup = m_iLastTabSelect;
		m_iBackup = i;
		m_jBackup = j;
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_ITEM_DELETE_MAKE_SUER), UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnDeleteItem,pFrm);
	}
    else if ( Is_Widget_Parts(WidgetID, part) )
    {
        PlayerFailOperate(R(MSG_ITEM_ITEMLOCK_BAGONLY));
        return;
    }

	unguard;
}

void UIFormPlayerItems::OnDrag_Pass(RtwWidget* sender, void*)
{
    guard;

    if (!m_pPlayer)
        m_pPlayer = GetWorld()->m_pPlayer;

    RtwWidget* pDragSvr = g_workspace.GetDragServer();
    if (!pDragSvr) return;
    if (!m_pPlayer)
        m_pPlayer = GetWorld()->m_pPlayer;

    if (OperateIsLocked())
        return;

    const string& WidgetID = pDragSvr->GetID();

    int i,j;
    CItemContainerBase::EEquipParts part;
    if ( Is_Widget_Common(WidgetID, i, j) )
    {
        if (m_pPlayer->mItem.m_Bag.GetLock(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j))
            return;

        CBag *pBag = &(m_pPlayer->mItem.m_Bag);
        SItemID item = pBag->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);

        if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Lock))
            return;
        //if (ItemIsPet(item.type))
        //    return;

        if (item.cLock == 0)
        {
            CItemNetHelper::NetSend_c2r_player_itemlock(item.id, m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
        }
        else 
        {
            m_bBackupMission = false;
            m_pageBackup = m_iLastTabSelect;
            m_iBackup = i;
            m_jBackup = j;
            UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_ITEM_PROTECT), R(MSG_BANK_PASS_QUERY));
            pFrm->SetPassword(true);
            pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, cbItemUnlock, pFrm);
        }
    }
    else if ( Is_Widget_Parts(WidgetID, part) )
    {
        PlayerFailOperate(R(MSG_ITEM_ITEMLOCK_BAGONLY));
        return;
    }

    unguard;
}

void UIFormPlayerItems::cbItemUnlock(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;

    UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();//Heten 参数错误
    RtString str = pFrm->GetString();

    SItemID item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageBackup, m_iBackup, m_jBackup);
    CItemNetHelper::NetSend_c2r_player_itemunlock(item.id, m_pageBackup, m_iBackup, m_jBackup, str);

    unguard;
}

void UIFormPlayerItems::OnDrag_MissionItem(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr) return;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& WidgetID1 = sender->GetID();
	int iTo,jTo;
	Is_Widget_Mission(WidgetID1, iTo, jTo);

	if (OperateIsLocked())
		return;
	if (m_pPlayer->mItem.m_MissionBag.GetLock(m_pPlayer->mItem.m_MissionBag.GetPageIndex(), iTo, jTo))
		return;

	const string& WidgetID = pDragSvr->GetID();

	/// 查找DragServer是什么(支持:Mission格子)
	// 是不是MissionItem格子：
	int i,j;
	if ( Is_Widget_Mission(WidgetID, i, j) )
	{
		SItemID itemFrom = m_pPlayer->mItem.m_MissionBag.GetItem(i, j);
		if ( !ItemID_IsValid(itemFrom) )
			return;

		if (!m_pPlayer->mItem.OperateItem_CheckUIContent(itemFrom, IO_Move))
			return;

		CBag *pBag = &(m_pPlayer->mItem.m_MissionBag);
		int page = pBag->GetPageIndex();
        CItemNetHelper::NetSend_MoveItem(ITEM_CONTAINER_PLAYER_MISSION_BAG, page, i, j, page, iTo, jTo);
		//if ( pBag->MoveItem(i, j, iTo, jTo) )
		//{
		//	CItemNetHelper::NetSend_MoveItem(ITEM_CONTAINER_PLAYER_MISSION_BAG, page, i, j, ITEM_CONTAINER_PLAYER_MISSION_BAG, page, iTo, jTo);
  //          bool bVisibleFrom = pBag->GetVisible(page, i, j);
  //          bool bVisibleTo = pBag->GetVisible(page, iTo, jTo);
  //          pBag->SetVisible(page, i, j, bVisibleTo);
  //          pBag->SetVisible(page, iTo, jTo, bVisibleFrom);

		//	g_workspace.ResetHintText();
		//	Show();
		//	return;
		//}
	}

	// 是不是交易窗口的下面的格子
	if (g_layerMain->m_formTrade && g_layerMain->m_formTrade->Is_Widget_Down(WidgetID, i, j) )
	{
		CTrade* pTrade = &(g_layerMain->m_formTrade->m_trade);
		CBag* pBag = &(m_pPlayer->mItem.m_MissionBag);
		SItemID itemFrom = pTrade->GetTradeBag( &(m_pPlayer->mItem) )->GetItem(m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
//		SItemID itemTo = pBag->GetItem(iTo, jTo);

		if (g_layerMain->m_formTrade->m_trade.IsLocked( &(m_pPlayer->mItem) ) )
		{
			g_layerMain->m_formTrade->m_trade.UnLock();
			g_layerMain->m_formTrade->m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
			g_layerMain->m_formTrade->m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
			CItemNetHelper::NetSend_c2r_trade_unlock();
		}
		if ( g_layerMain->m_formTrade->m_trade.TakeOut( &(m_pPlayer->mItem), i, j, pBag->GetPageIndex(), iTo, jTo) )
		{
			int page1, i1, j1;
			if (ItemIsMissionItem(itemFrom))
			{
				if ( m_pPlayer->mItem.m_MissionBag.Find(itemFrom.id, &page1, &i1, &j1) )
					m_pPlayer->mItem.m_MissionBag.SetVisible(page1, i1, j1, true);
			}
			else
			{
				if ( m_pPlayer->mItem.m_Bag.Find(itemFrom.id, &page1, &i1, &j1) )
					m_pPlayer->mItem.m_Bag.SetVisible(page1, i1, j1, true);
			}
		}
		Show();
		g_layerMain->m_formTrade->Refresh();
	}

	unguard;
}

void UIFormPlayerItems::OnDragEnd_Parts(RtwWidget* sender, void*)
{
	guard;

//	RtwWidget* pClient = g_workspace.GetDragClient();
//	if (!m_pPlayer)
//		m_pPlayer = GetWorld()->m_pPlayer;
//	CBag *pBag = &(m_pPlayer->mItem.m_Bag);
//	const string& widgetID = sender->GetID();
//	CItemContainerBase::EEquipParts part;
//	Is_Widget_Parts(widgetID, part);
//
//	if ( !pClient )// 把图标拖到空白处
//	{
//		//if ( g_layerMain->m_formPShop->m_PShop.IsValid() )			return;
//		if ( g_layerMain->m_formTrade->IsShowing() )				return;
//		if ( g_layerMain->m_formShop->IsVisible() )					return;
//	}
//	else
//	{
//		const RtwRect& rcClient = pClient->GetFrameRect();
//		const RtwRect& rcHotKey = LOAD_UI("fmhotkey")->GetFrameRect();
//		if ( rcHotKey.IsContained(rcClient.left, rcClient.top) )
//		{
//			// 把图标拖到快捷栏中
//			SItemID itemID = m_pPlayer->mItem.GetEquipItem(part, true);
//			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
//			CHECK( pItemBase && ItemID_IsValid(itemID) );
//			string strID = "QS_" + sender->GetID();
//
//			RtwButton* pButton = NULL;
//			pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strID));
//			if (pButton==NULL)
//			{
//// 				pButton = (RtwButton*)sender->Clone(NULL, strID);
//// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);// change [3/17/2009 tooth.shi]
//				pButton->CloneFrom(sender);
//				pButton->ModifyFlags(wfHint,0);
//				pButton->LoadAttributesFromTemplate("IconBorder");
//
//				DWORD *pDword = new DWORD[3];
//				pDword[0] = UIHotKey_Item;
//				pDword[1] = itemID.id;
//				pDword[2] = itemID.type;
//				pButton->SetUserData(pDword);
//				pButton->EvClose	+= RTW_CALLBACK(this, UIFormPlayerItems, DeleteQuickStartUserData);
//
//				pButton->EvLClick	+= RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_QuickStart);
//				pButton->EvHint		+= RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_QuickStart);
//			}
//			g_workspace.SetDragServer(pButton);
//		}
//	}

	unguard;
}

void UIFormPlayerItems::OnGetHintText_MissionItem(RtwWidget* sender, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& widgetID = sender->GetID();
	int i, j;
	Is_Widget_Mission(widgetID, i, j);
	SItemID itemID = m_pPlayer->mItem.m_MissionBag.GetItem(i, j);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	int page = m_pPlayer->mItem.m_MissionBag.GetPageIndex();
	RtString str = GenerateItemHtmlHint(nLines, itemID, m_pPlayer, &(m_pPlayer->mItem.m_MissionBag), page, i, j);
	g_workspace.SetHintText((char*)str, true);
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}

void UIFormPlayerItems::OnRClick_MissionItem(RtwWidget* sender, void*)
{
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	// 取出 i,j
	const string& WidgetID = sender->GetID();
	int i,j;
	Is_Widget_Mission(WidgetID, i, j);

	if (OperateIsLocked())
		return;
	if (m_pPlayer->mItem.m_MissionBag.GetLock(m_pPlayer->mItem.m_MissionBag.GetPageIndex(), i, j))
		return;

	SItemID item = GetWorld()->m_pPlayer->mItem.m_MissionBag.GetItem(GetWorld()->m_pPlayer->mItem.m_MissionBag.GetPageIndex(), i, j);
	if (!ItemID_IsValid(item))   return;

	CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_MISSION_BAG, GetWorld()->m_pPlayer->mItem.m_MissionBag.GetPageIndex(), i, j);
}

void UIFormPlayerItems::OnDeleteItem(void*, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	CBag *pBag;
	if (m_bBackupMission)
		pBag = &(m_pPlayer->mItem.m_MissionBag);
	else
		pBag = &(m_pPlayer->mItem.m_Bag);
	CHECK( pBag->_IsValidIndex(m_pageBackup, m_iBackup, m_jBackup) );
	SItemID item = pBag->GetItem(m_pageBackup, m_iBackup, m_jBackup);

	if (m_pPlayer->mItem.CanItemTrade(item.id)==TR_CanNot_Pet)
	{
		PlayerFailOperate(R(MSG_STORE_PET_FIRST));
		return;
	}
	if (GetWorld()->m_pPet && m_pet.id == item.id)
	{
		PlayerFailOperate(R(MSG_CANNOT_CALLBACK_PET));
		return;
	}

	pBag->RemoveItem(m_pageBackup, m_iBackup, m_jBackup);
	int page = pBag->GetPageIndex();
	if (m_bBackupMission)
        CItemNetHelper::NetSend_DeleteItem(item.id, ITEM_CONTAINER_PLAYER_MISSION_BAG, m_pageBackup, m_iBackup, m_jBackup);
	else 
	{
		CItemNetHelper::NetSend_DeleteItem(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, m_pageBackup, m_iBackup, m_jBackup);

		//if(LOAD_UI("fmplayershop")->IsVisible())
		//{
		g_layerMain->m_formPlayerShop->OnDeleteItem(item);
		//}

		if (g_layerMain->m_formItemUpdate->IsVisible())
		{
			g_layerMain->m_formItemUpdate->OnCommonItemDeleted(item.id);
		}
		if (g_layerMain->m_formGem->IsVisible())
		{
			g_layerMain->m_formGem->OnCommonItemDeleted(item.id);
		}
		//if (g_layerMain->m_formPShop->IsVisible())
		//{
		//	g_layerMain->m_formPShop->OnCommonItemDeleted(item.id);
		//}
	}

	//if (g_layerMain->m_formPShop->IsVisible() && !g_layerMain->m_formPShop->m_PShop.IsStart())
	//{
	//	g_layerMain->m_formPShop->Show();
	//}

	g_workspace.ResetHintText();
	RefreshQuickStart();
	Show();

	unguard;
}

void UIFormPlayerItems::OnThrowItem(void*, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	CBag *pBag;
    EItemContainer ConType;
    if (m_bBackupMission)
    {
        pBag = &(m_pPlayer->mItem.m_MissionBag);
        ConType = ITEM_CONTAINER_PLAYER_MISSION_BAG;
    }
    else 
    {
        pBag = &(m_pPlayer->mItem.m_Bag);
        ConType = ITEM_CONTAINER_PLAYER_COMMON_BAG;
    }
	CHECK( pBag->_IsValidIndex(m_pageBackup, m_iBackup, m_jBackup) );
	SItemID item = pBag->GetItem(m_pageBackup, m_iBackup, m_jBackup);
	CHECK( ItemID_IsValid(item) );

	if (m_pPlayer->mItem.CanItemTrade(item.id)==TR_CanNot_Pet)
	{
		PlayerFailOperate(R(MSG_STORE_PET_FIRST));
		return;
	}
	if (item.cBind==IB_Binded)
	{
		PlayerFailOperate(R(MSG_THROW_BINDED));
		return;
	}

	//if(LOAD_UI("fmplayershop")->IsVisible())
	//{
	g_layerMain->m_formPlayerShop->OnDeleteItem(item);
	//}

	CItemNetHelper::NetSend_ThrowItem(item.id, ConType, m_pageBackup, m_iBackup, m_jBackup);
	unguard;
}

void UIFormPlayerItems::OnDecomposeItem(void*, void*)
{
	guard;

	SItemID item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(m_pageBackup, m_iBackup, m_jBackup);
	if (item.id != m_IdBackup)
		return;

	LockOperate(true);
	CItemNetHelper::NetSend_c2r_decompose_item(item.id);

	unguard;
}

void UIFormPlayerItems::OnEnchantItem(void*, void*)
{
	guard;

	CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);

	int EquipmentPosPage, EquipmentPosI, EquipmentPosJ;
	int ScrollPosPage, ScrollPosI, ScrollPosJ;
	if (!pBag->Find(m_IdBackup, &ScrollPosPage, &ScrollPosI, &ScrollPosJ))
		return;
	if (!pBag->Find(m_IdBackup1, &EquipmentPosPage, &EquipmentPosI, &EquipmentPosJ))
		return;
	SItemID EquipmentItem = pBag->GetItem(EquipmentPosPage, EquipmentPosI, EquipmentPosJ);
	SItemID ScrollItem = pBag->GetItem(ScrollPosPage, ScrollPosI, ScrollPosJ);

	LockOperate(true);
	CItemNetHelper::NetSend_c2r_enchant_item(EquipmentItem.id, ScrollItem.id);

	unguard;
}

void UIFormPlayerItems::OnLClick_QuickStart(RtwWidget* sender, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	DWORD *pItemData = (DWORD*)sender->GetUserData();
	if (!pItemData || pItemData[0]!=UIHotKey_Item)
		return;
	DWORD ItemID = pItemData[1];
	DWORD ItemType = pItemData[2];

    if (ItemIsMissionItem(ItemType))
        return;

	SItemID itemTmp;
	if ( ItemID )
	{
		int part = CItemContainerBase::EQUIP_DEFAULT_PART+1;
		for (; part < CItemContainerBase::MAX_EQUIP_PARTS; part++)
		{
			itemTmp = m_pPlayer->mItem.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
			if (itemTmp.id == ItemID)
				break;
		}
		if ( part != CItemContainerBase::MAX_EQUIP_PARTS )
		{
			SItemID item = m_pPlayer->mItem.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
			// 要做一定的检查，看是否满足条件

			if (m_pPlayer->mItem.m_Bag.IsFull())
			{
				PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
				return;
			}
			if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UnEquip))
			{
				CItemNetHelper::NetSend_UnEquipItem(item.id, (CItemContainerBase::EEquipParts)part);
			}
		}
		else
		{
			int page, i, j;
			if ( m_pPlayer->mItem.m_Bag.FindID(ItemID, &page, &i, &j) )
			{
                DoUseCommonBagItem(page, i, j);
			}
		}
	}

	unguard;
}

void UIFormPlayerItems::OnGetHintText_QuickStart(RtwWidget* sender, void*)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	DWORD *pItemData = (DWORD*)sender->GetUserData();
	if (!pItemData || pItemData[0]!=UIHotKey_Item)
		return;
	DWORD ItemID = pItemData[1];
	DWORD ItemType = pItemData[2];

	SItemID itemTmp;
	if ( ItemID )
	{
		int part = CItemContainerBase::EQUIP_DEFAULT_PART+1;
		for (; part < CItemContainerBase::MAX_EQUIP_PARTS; part++)
		{
			itemTmp = GetWorld()->m_pPlayer->mItem.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
			if (itemTmp.id == ItemID)
				break;
		}
		if ( part != CItemContainerBase::MAX_EQUIP_PARTS )
		{
			int nLines;
			RtString str = GenerateItemHtmlHint(nLines, itemTmp, GetWorld()->m_pPlayer);
			g_workspace.SetHintText((const char*)str, true);
//			g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
			g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
			g_workspace.AutoHintSize();
			g_workspace.AutoHintPosition();
		}
		else
		{
            CBag *pBag = NULL;
            if (ItemIsMissionItem(ItemType))
                pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
            else 
                pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);

			int page, i, j;
			if ( pBag->FindID(ItemID, &page, &i, &j) )
			{
				int nLines;
				itemTmp = pBag->GetItem(page, i, j);
				RtString str = GenerateItemHtmlHint(nLines, itemTmp, GetWorld()->m_pPlayer, pBag, page, i, j, -1.0f, 0, true);

				g_workspace.SetHintText((const char*)str, true);
//				g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
				g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
				g_workspace.AutoHintSize();
				g_workspace.AutoHintPosition();
			}
			else
			{   
				LOG("发现未知的快捷栏图标");
			}
		}
	}

	unguard;
}

void UIFormPlayerItems::RemoveQuickStart(DWORD itemID)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	DWORD ID;
	for (int i=0; i<UI_HOT_KEY_COUNT; i++)
	{
		RtwButton *pBtn = g_layerMain->m_hotKeyAlias[i]->GetButton();
		if (pBtn)
		{
			DWORD *pItemData = (DWORD*)pBtn->GetUserData();
			if (pItemData && pItemData[0]==UIHotKey_Item)
			{
				ID = pItemData[1];
				if (itemID == ID)
				{
					g_layerMain->m_hotKeyAlias[i]->SetButton(NULL);
				}
			}
		}
		//if (g_layerMain->m_hotKeyGroup[i])
		//{
		//	DWORD *pItemData = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
		//	if (pItemData && pItemData[0]==UIHotKey_Item)
		//	{
		//		ID = pItemData[1];
		//		if (itemID == ID)
		//		{
		//			g_layerMain->m_hotKeyGroup[i] = NULL;
		//		}
		//	}
		//}
	}

	unguard;
}

void UIFormPlayerItems::GetQuickStartButton(DWORD itemID, vector<RtwButton*>& arrBtns)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	DWORD ID;
	for (int i=0; i<UI_HOT_KEY_COUNT; i++)
	{
		RtwButton *pBtn = g_layerMain->m_hotKeyAlias[i]->GetButton();
		if (pBtn)
		{
			DWORD *pItemData = (DWORD*)pBtn->GetUserData();
			if (pItemData && pItemData[0]==UIHotKey_Item)
			{
				ID = pItemData[1];
				if (itemID == ID)
				{
					arrBtns.push_back(pBtn);
				}
			}
		}
		//if (g_layerMain->m_hotKeyGroup[i])
		//{
		//	DWORD *pItemData = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
		//	if (pItemData && pItemData[0]==UIHotKey_Item)
		//	{
		//		ID = pItemData[1];
		//		if (itemID == ID)
		//		{
		//			arrBtns.push_back(g_layerMain->m_hotKeyGroup[i]);
		//		}
		//	}
		//}
	}

	unguard;
}

void UIFormPlayerItems::DeleteQuickStartUserData(RtwWidget* sender, void*)
{
	guard;

	DWORD *pData = (DWORD*)sender->GetUserData();
	if (pData)
	{
		DEL_ARRAY(pData);
	}
	unguard;
}

void UIFormPlayerItems::RefreshQuickStart()
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	//heten
	//if (GcActor:: ==GetWorld()->m_pPlayer && g_layerMain)
	//{
	//	GetWorld()->m_pPlayer->SetAutobattle(false);
	//	g_layerMain->m_btnAutoBattle->SetChecked(false);
	//}
	//end	死亡后取消自动战斗
	
	DWORD ItemID;
	DWORD ItemType;
	for (int i=0; i<UI_HOT_KEY_COUNT; i++)
	{
		RtwButton *pBtn = g_layerMain->m_hotKeyAlias[i]->GetButton();
		if (pBtn)
		{
			DWORD *pItemData = (DWORD*)pBtn->GetUserData();
			if (pItemData && pItemData[0]==UIHotKey_Item)
			{
				ItemID = pItemData[1];
				ItemType = pItemData[2];
				if (!_RefreshQuickStart(ItemID, ItemType))
				{
					RemoveQuickStart(ItemID);
				}
			}
		}
		//if (g_layerMain->m_hotKeyGroup[i])
		//{
		//	DWORD *pItemData = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
		//	if (pItemData && pItemData[0]==UIHotKey_Item)
		//	{
		//		ItemID = pItemData[1];
		//		ItemType = pItemData[2];
		//		if (!_RefreshQuickStart(ItemID, ItemType))
		//		{
		//			RemoveQuickStart(ItemID);
		//		}
		//	}
		//}
	}
	unguard;
}

bool UIFormPlayerItems::_RefreshQuickStart(DWORD ItemID, DWORD ItemType)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if (ItemCanPile(ItemType))
	{
		CBag* pBag = NULL;
		if (ItemIsMissionItem(ItemType))
			pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
		else 
			pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
		int tmpP, tmpI, tmpJ;
		int count;
		int itemIndex = GetWorld()->m_pItemManager->GetIndexFromType(ItemType);
		bool res = pBag->FindType(itemIndex, ItemColor_All, &count, &tmpP, &tmpI, &tmpJ);
		if (res)
		{
			vector<RtwButton*> arrBtns;
			RtwButton *pButton = NULL;
			SItemID item1st_after = pBag->GetItem(tmpP, tmpI, tmpJ);
			CHECK(ItemID_IsValid(item1st_after));
			g_layerMain->m_fromPlayerItems->GetQuickStartButton(ItemID, arrBtns);
			for (size_t i=0; i<arrBtns.size(); i++)
			{
				pButton = arrBtns[i];
				DWORD* pData = (DWORD*)pButton->GetUserData();
				pData[1] = item1st_after.id;
				//pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
				pButton->SetText(rtFormatNumber(count));
			}

			return true;
		}
		else 
		{
			RemoveQuickStart(ItemID);
			return false;
		}
	}
	else if (ItemIsArmor(ItemType) || ItemIsWeapon(ItemType) || ItemIsShipin(ItemType) || ItemIsTrump(ItemType) || ItemIsFashion(ItemType))
	{
		SItemID itemTmp;
		int part = CItemContainerBase::EQUIP_DEFAULT_PART+1;
		for (; part < CItemContainerBase::MAX_EQUIP_PARTS; part++)
		{
			itemTmp = GetWorld()->m_pPlayer->mItem.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
			if (itemTmp.id == ItemID)
				break;
		}
		if ( part != CItemContainerBase::MAX_EQUIP_PARTS )
		{
			return true;
		}
		else
		{
			int page, i, j;
			if ( GetWorld()->m_pPlayer->mItem.m_Bag.FindID(ItemID, &page, &i, &j) )
			{
				return true;
			}
			else 
			{
				return false;
			}
		}
	}
	else if (ItemIsPet(ItemType))
	{
		int page, i, j;
		if ( GetWorld()->m_pPlayer->mItem.m_Bag.FindID(ItemID, &page, &i, &j) )
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	else if (ItemIsScrollHome(ItemType))
	{
		int page, i, j;
		if ( GetWorld()->m_pPlayer->mItem.m_Bag.FindID(ItemID, &page, &i, &j) )
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	else 
	{
		int page, i, j;
		if ( GetWorld()->m_pPlayer->mItem.m_Bag.FindID(ItemID, &page, &i, &j) )
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
    return false;
	unguard;
}

bool UIFormPlayerItems::GetQuickStartItemData(int pageIndex, int gridIndex, DWORD& ItemID, DWORD& ItemType)
{
	guard;

	//BOOL_ENSURE(pageIndex>=0 && pageIndex<=1)
	BOOL_ENSURE(gridIndex>=0 && gridIndex<=UI_HOT_KEY_COUNT)

	//if (pageIndex==0)
	//{
		RtwButton *pBtn = g_layerMain->m_hotKeyAlias[gridIndex]->GetButton();
		if (pBtn)
		{
			DWORD *pItemData = (DWORD*)pBtn->GetUserData();
			if (pItemData && pItemData[0]==UIHotKey_Item)
			{
				ItemID = pItemData[1];
				ItemType = pItemData[2];
				return true;
			}
			else 
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	//}
	//else if (pageIndex==1)
	//{
	//	if (g_layerMain->m_hotKeyGroup[gridIndex])
	//	{
	//		DWORD *pItemData = (DWORD*)g_layerMain->m_hotKeyGroup[gridIndex]->GetUserData();
	//		if (pItemData && pItemData[0]==UIHotKey_Item)
	//		{
	//			ItemID = pItemData[1];
	//			ItemType = pItemData[2];
	//			return true;
	//		}
	//		else 
	//		{
	//			return false;
	//		}
	//	}
	//	else 
	//	{
	//		return false;
	//	}
	//}

	return false;

	unguard;
}

void UIFormPlayerItems::OnClose(RtwWidget* sender, void*)
{
	guard;
	unguard;
}

void UIFormPlayerItems::OnMouseMove(RtwWidget* sender, void*)
{
	guard;

	/*if (((CGameClientFrame*)GetApp())->m_iMouseCaptureID == cti_Decompose)
	{
		((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Decompose, GAME_CURSOR_DEITEM, NULL, "");
	}
	else if (((CGameClientFrame*)GetApp())->m_iMouseCaptureID == cti_Enchant)
	{
		((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Enchant, GAME_CURSOR_DEITEM, NULL, "");
	}
	else
	{
	}*/

	unguard;
}

void UIFormPlayerItems::LockOperate(bool bLock)
{
	guard;

	if (bLock)
	{
		m_bLockOperate = true;
		m_dwLastLockTick = rtGetMilliseconds();
	}
	else
	{
		m_bLockOperate = false;
		m_dwLastLockTick = 0;
	}

	unguard;
}

bool UIFormPlayerItems::OperateIsLocked()
{
	guard;

	if (!m_bLockOperate || m_dwLastLockTick<=0)
		return false;

	if (rtGetMilliseconds()-m_dwLastLockTick > 4000 && !GetPlayer()->m_bPShop)
	{
		m_bLockOperate = false;
		m_dwLastLockTick = 0;
		return false;
	}
	else 
	{
		return true;
	}

	unguard;
}

void UIFormPlayerItems::OnPickItem(void*, void*)
{
	guard;

	if (m_pickItemOID)
	{
		static DWORD dwLastTick = 0;
		DWORD nowTick = rtGetMilliseconds();
		if (nowTick-dwLastTick > 1200)
		{
			CItemNetHelper::NetSend_PickItem(m_pickItemOID);
			dwLastTick = nowTick;
		}
	}

	unguard;
}

void UIFormPlayerItems::OnEquipItem(void*, void*)
{
	guard;

	if (m_equipItemOID)
	{
		CItemNetHelper::NetSend_EquipItem(m_equipItemOID, m_equipPart, ITEM_CONTAINER_PLAYER_COMMON_BAG, m_equipPage, m_equipGridI, m_equipGridJ);
		LockOperate(true);
	}

	unguard;
}

//add by yz
void UIFormPlayerItems::UseBagItem(int page, int gridI, int gridJ, const SItemID* pItemID /*= NULL*/)
{
	if (NULL != pItemID)
	{
		if (GetWorld()->m_pPlayer->mItem.m_Bag.Find(*pItemID, &page, &gridI, &gridJ))
		{
			this->DoUseCommonBagItem(page, gridI, gridJ);
			return;
		}
	}
	
	this->DoUseCommonBagItem(page, gridI, gridJ);
}
//end yz

void UIFormPlayerItems::DoUseCommonBagItem(int page, int gridI, int gridJ)
{
    if (!m_pPlayer)
        m_pPlayer = GetWorld()->m_pPlayer;

	SItemID item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(page, gridI, gridJ);

    if (!ItemID_IsValid(item))   return;

    if (!(ItemIsWeapon(item.type) || ItemIsArmor(item.type) || ItemIsShipin(item.type)) || ItemIsTrump(item.type) || ItemIsFashion(item.type) || ItemIsGemMagic(item))
    {
        if (!GetWorld()->m_pPlayer->mItem.CheckUse(item, GetWorld()->m_pPlayer->m_core))
        {
            PlayerFailOperate(R(MSG_ITEM_CANNOTUSE));
            return;
		}
		if (ItemIsTrump(item.type))
		{
			if (g_layerMain->m_formChar->m_bIsRifine)
			{
				g_layerMain->m_formChar->OnFinishRifineTrump(false);
				GetPlayer()->SendTrumpRifine(0,true);
			}
		}
    }

    if (ItemIsWeapon(item.type) || ItemIsArmor(item.type) || ItemIsShipin(item.type) || ItemIsTrump(item.type) || ItemIsFashion(item.type) || ItemIsGemMagic(item))
    {
        CItemContainerBase::EEquipParts part;

		// 有主武器,则选择装备备用武器 -add by 郑泽华
		part = m_pPlayer->mItem.GetDefaultEquipPart(item);

        if ( !m_pPlayer->mItem.CheckEquip(part, item, m_pPlayer->m_core) )
        {
            PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL), true, "cannotpickup.wav");
            return;
        }
        if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Equip))
        {
            m_equipItemOID = item.id;
            m_equipPart = part;
            m_equipPage = page;
            m_equipGridI = gridI;
            m_equipGridJ = gridJ;
            if (item.cBind == IB_BindWhenEquip)
            {
                SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
                if (!pItemClass)
                    return;
                char cTmp128[128];
                rt2_sprintf(cTmp128, R(MSG_EQUIP_WILLBIND), pItemClass->name);
                UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
                pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEquipItem,pFrm);
            }
            else 
            {
                OnEquipItem(NULL, NULL);
			}
			if (part == CItemContainerBase::TRUMP)
			{
				SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
				if (ItemID_IsValid(item) && ItemIsTrump(item))
				{
					SSkillContinueMask scm;
					for (int i=TRUMP_SKILL_1;i<TRUMP_SKILL_MAX;i++)
					{
						CSkillInstance *pSkill = m_pPlayer->m_Skill.FindActiveSkill(item.params[i]);
						if (pSkill)
						{
							scm.wSkillClass = pSkill->m_pAttr->wClass;
							scm.resumeTime = pSkill->m_dwResumeTime;
							scm.useTime = pSkill->m_dwUserTime;
							m_pPlayer->m_Skill.m_listMask.push_back(scm);
						}
					}
				}
			}
        }
    }
    else if (ItemIsYaoPing(item.type) )
    {
        SYaoPing *pYaoPingClass = (SYaoPing *)GetItemMgr()->GetItem(item.type);
        if (!pYaoPingClass)		return;

        if ( ItemIsHPBottle(item.type) )
        {
			// 注，此为几率出现，当玩家刚好在吃完药的瞬间再吃药会出现问题，以下均相同
			if (m_vecOperateItemCD[0]->size()> 0 &&( !(*m_vecOperateItemCD[0]->begin()) || (*m_vecOperateItemCD[0]->begin())->m_dwOperateTime!= 0))
			{
				PlayerFailOperate(R(MSG_ITEM_CDNOTFINISH));
				return;
			}
            if	(m_pPlayer->GetRideState()==GcActor::ERS_ACTOR)
            {
                if (m_pPlayer->GetRidePet() && m_pPlayer->GetRidePet()->m_core.GetHp()==m_pPlayer->GetRidePet()->m_core.GetMaxHp())
                    return;
            }
            else 
            {
                if (m_pPlayer->m_core.GetHp() == m_pPlayer->m_core.GetMaxHp())
				{
					PlayerFailOperate(R(MSG_ITEM_NONEEDUSELIFE));
					return;
				}
            }
			OnSetCD(0,item);
        }
        else if ( ItemIsMPBottle(item.type) )
        {
			if (m_vecOperateItemCD[1]->size()> 0 &&( !(*m_vecOperateItemCD[1]->begin()) || (*m_vecOperateItemCD[1]->begin())->m_dwOperateTime!= 0))
			{
				PlayerFailOperate(R(MSG_ITEM_CDNOTFINISH));
				return;
			}
            if	(m_pPlayer->GetRideState()==GcActor::ERS_ACTOR)
            {
                if (m_pPlayer->GetRidePet() && m_pPlayer->GetRidePet()->m_core.GetMp()==m_pPlayer->GetRidePet()->m_core.GetMaxMp())
                    return;
            }
            else 
            {
                if (m_pPlayer->m_core.GetMp() == m_pPlayer->m_core.GetMaxMp())
				{
					PlayerFailOperate(R(MSG_ITEM_NONEEDUSEMAGIC));
					return;
				}
            }
			OnSetCD(1,item);
		}
		else if ( ItemIsEnergyBottle(item.type) )
		{
			if (m_vecOperateItemCD[2]->size()> 0 &&( !(*m_vecOperateItemCD[2]->begin()) || (*m_vecOperateItemCD[2]->begin())->m_dwOperateTime!= 0))
			{
				PlayerFailOperate(R(MSG_ITEM_CDNOTFINISH));
				return;
			}
			if	(m_pPlayer->GetRideState()==GcActor::ERS_ACTOR)
			{
				if (m_pPlayer->GetRidePet() && m_pPlayer->GetRidePet()->m_core.GetMp()==m_pPlayer->GetRidePet()->m_core.GetMaxMp())
					return;
			}
			else 
			{
				if (m_pPlayer->m_core.GetEnP() == m_pPlayer->m_core.GetMaxEnP())
				{
					PlayerFailOperate(R(MSG_FABAO_ENERGYISFULL));
					return;
				}
			}
			OnSetCD(2,item);
		}
		else if ( ItemIsPetHpBottle(item.type) )
		{
			if (m_vecOperateItemCD[3]->size()> 0 &&( !(*m_vecOperateItemCD[3]->begin()) || (*m_vecOperateItemCD[3]->begin())->m_dwOperateTime!= 0))
			{
				PlayerFailOperate(R(MSG_ITEM_CDNOTFINISH));
				return;
			}
			if	(m_pPlayer->GetRideState()==GcActor::ERS_ACTOR)
			{
				if (m_pPlayer->GetRidePet() && m_pPlayer->GetRidePet()->m_core.GetHp()==m_pPlayer->GetRidePet()->m_core.GetMaxHp())
					return;
			}
			else 
			{
				if(!GetPlayerPet()->GetPetActor())
				{
					PlayerFailOperate(R(MSG_PET_NOPETFIGHT));
					return;
				}
				if (GetPlayerPet()->GetPetActor()->m_core.GetHp() == GetPlayerPet()->GetPetActor()->m_core.GetMaxHp())
				{
					PlayerFailOperate(R(MSG_PET_ITSHPISFULL));
					return;
				}
			}
			OnSetCD(3,item);
		}
		else if ( ItemIsPetMpBottle(item.type) )
		{
			if (m_vecOperateItemCD[4]->size()> 0 &&( !(*m_vecOperateItemCD[4]->begin()) || (*m_vecOperateItemCD[4]->begin())->m_dwOperateTime!= 0))
			{
				PlayerFailOperate(R(MSG_ITEM_CDNOTFINISH));
				return;
			}
			if	(m_pPlayer->GetRideState()==GcActor::ERS_ACTOR)
			{
				if (m_pPlayer->GetRidePet() && m_pPlayer->GetRidePet()->m_core.GetMp()==m_pPlayer->GetRidePet()->m_core.GetMaxMp())
					return;
			}
			else 
			{
				if(!GetPlayerPet()->GetPetActor())
				{
					PlayerFailOperate(R(MSG_PET_NOPETFIGHT));
					return;
				}
				if (GetPlayerPet()->GetPetActor()->m_core.GetMp() == GetPlayerPet()->GetPetActor()->m_core.GetMaxMp())
				{
					PlayerFailOperate(R(MSG_PET_ITSMPISFULL));
					return;
				}
			}
			OnSetCD(4,item);
		}

        if ( !m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Eat))
        {
            return;
        }

        CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
        LockOperate(true);
    }
    else if (ItemIsPet(item.type) )
    {
		//if (g_layerMain->m_formGuide3->IsVisible())
		//{
		//	g_layerMain->m_formGuide3->Hide();
		//}
  //      if (m_dwPetOperateTime!=0)
  //      {
  //          return;
  //      }
  //      else 
  //      {
  //          if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UsePet))
		//	{
  //              return;
		//	}

		//	if ( item.params[PET_PARAM_HP] <= 0  )
		//	{
		//		PlayerFailOperate(R(MSG_PET_NEEDRECOVER));
		//		return;
		//	}

		//	m_LastPetItemID = m_pet;
		//	if (GetWorld()->m_pPet && m_pet.id == item.id)
		//	{
		//		g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_CallPet,1);
		//	}
		//	else
		//	{
		//		g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_CallPet,10000);
		//	}

		//	m_pet = item;
  //          m_iPetActivePage = page;
  //          m_iPetActiveI = gridI;
  //          m_iPetActiveJ = gridJ;
  //          m_dwPetOperateTime = rtGetMilliseconds();
		//	g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this,UIFormPlayerItems,CallPet);
		//	GetPlayer()->m_bInCallPet = true;

		//	LockOperate(true);

  //          // Find the item's Shortcut
  //          DWORD ID;
  //          for (int i=0; i<UI_HOT_KEY_COUNT; i++)
  //          {
  //              RtwButton *pBtn = g_layerMain->m_hotKeyAlias[i]->GetButton();
  //              if (pBtn)
  //              {
  //                  DWORD *pItemData = (DWORD*)pBtn->GetUserData();
  //                  if (pItemData && pItemData[0]==UIHotKey_Item)
  //                  {
  //                      ID = pItemData[1];
  //                      if (ID == item.id)
  //                      {
  //                          m_pBtnPetShortcut = pBtn;
  //                      }
  //                  }
  //              }
  //              //if (g_layerMain->m_hotKeyGroup[i])
  //              //{
  //              //    DWORD *pItemData = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
  //              //    if (pItemData && pItemData[0]==UIHotKey_Item)
  //              //    {
  //              //        ID = pItemData[1];
  //              //        if (ID == item.id)
  //              //        {
  //              //            m_pBtnPetShortcut = pBtn;
  //              //        }
  //              //    }
  //              //}
  //          }

  //      }
    }
    else if (ItemIsScrollHome(item.type))
    {
        if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UseScroll))
            return;

        if (GetWorld()->IsCastleWarTime("XiQi") && (GetWorld()->m_pPlayer->m_core.Faction == FACTION_ZHOU))
        {
            PlayerFailOperate(R(MSG_CASTLETIME_OPERATE_FAIL), true);
            return;
        }
        if (GetWorld()->IsCastleWarTime("ZhaoGe") && (GetWorld()->m_pPlayer->m_core.Faction == FACTION_SHANG))
        {
            PlayerFailOperate(R(MSG_CASTLETIME_OPERATE_FAIL), true);
            return;
        }

        CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
        LockOperate(true);
    }
    else if (ItemIsOther(item.type))
    {
		//点击背包物品，打断玩家移动状态
		if(GetWorld()->m_pPlayer->OnGetPlayerIsMoving())
		{
			GetWorld()->m_pPlayer->RemoveCurCommand();
		}

        if (!m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UseOther))
            return;

        CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
        LockOperate(true);
    }
    else if (ItemIsSkill(item.type))
    {
        CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
        LockOperate(true);
    }
    else if (ItemIsScrollEnchant(item.type))
    {
        m_IdBackup = item.id;
        ((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Enchant, GAME_CURSOR_DEITEM, NULL, R(请选择该符咒要增强的装备));
		bEnchant = true;
    }
    else if (ItemIsScrollCompose(item.type))
    {
        // 判断是否满足学习配方的条件
        bool bCanStudy = false;
        SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
        if (!pItemClass)		return;
        SItemCompose* pComposeClass = GetWorld()->m_pItemManager->m_pComposeMgr->FindComposing(pItemClass->composeID);
        if (!pComposeClass)		return;
        SSkill* pSkill = NULL;
        WORD CheckSkillID = 0;
        switch (pComposeClass->ComposeType)
		{
		case ICT_Casting:
			CheckSkillID = ITEMCOMPOSE_EQUIP_SKILL;
			break;
        case ICT_Equipment:
            CheckSkillID = ITEMCOMPOSE_WEAPON_SKILL;
            break;
        case ICT_Yaoping:
            CheckSkillID = ITEMCOMPOSE_YAOPING_SKILL;
            break;
        case ICT_EnchantScroll:
            CheckSkillID = ITEMCOMPOSE_ENCHANT_SKILL;
            break;
        default:
            break;
        }
        pSkill = GetWorld()->m_pPlayer->m_Skill.FindSkillBySubID(CheckSkillID);
        if (!pSkill || (pSkill->iLevel < pComposeClass->SkillLevel) )
            bCanStudy = false;
        else
            bCanStudy = true;
        // 发送请求
        if (bCanStudy)
        {
            CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
            LockOperate(true);
        }
        else 
        {
            PlayerFailOperate(R(MSG_SKILL_SKILLLEVELLESS));
            return;
        }
    }
    else
    {
    }
}
void UIFormPlayerItems::CallPet(void*, void*)
{
	
	g_layerMain->m_formTimeBar->End(false);
	CItemNetHelper::NetSend_c2r_player_use_item(m_pet.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, m_iPetActivePage, m_iPetActiveI, m_iPetActiveJ);
	GetPlayer()->m_bInCallPet = false;
	
	if ( GetWorld()->m_pPet == NULL )
	{
		char zhaohuan[128];
		SItemPet *pItemClass = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_pet.type);
		SNpc* pNpc = g_TableNpc.FindNpc(pItemClass->petID);
		rt2_sprintf(zhaohuan,R(MSG_PET_ZHAOHUAN),pNpc->Name.c_str());
		PlayerFailOperate(zhaohuan);
	}
	else
	{
		if (!m_pPlayer)
			m_pPlayer = GetWorld()->m_pPlayer;
		if (m_pPlayer->GetRideState()!=GcActor::ERS_ACTOR)
		{
			char back[128];
			rt2_sprintf(back,R(MSG_PET_BACK),GetWorld()->m_pPet->m_pNpc->Name.c_str());
			PlayerFailOperate(back);
		}
		
	}
	
}

void UIFormPlayerItems::PlayTaskItem(int iTime)
{
	if (m_bPlayItem)
	{		
		m_bPlayItem = false;
		g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_CallTaskItem,iTime * 1000);
		g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this,UIFormPlayerItems,PlayTaskItemEnd);
		GetPlayer()->m_bInCallPet = true;
	}
}

void UIFormPlayerItems::PlayTaskItemEnd(void*, void*)
{
	g_layerMain->m_formTimeBar->End(false);
	GetPlayer()->m_bInCallPet = false;
	m_bPlayItem = true;
}

void UIFormPlayerItems::PlayActivityProgressBar(long lTime)
{
	g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_CallTaskItem,lTime);
	g_layerMain->m_formTimeBar->EvTimeOver += RTW_CALLBACK(this,UIFormPlayerItems,PlayActivityProgressBarEnd);
}

void UIFormPlayerItems::PlayActivityProgressBarEnd(void*, void*)
{
	g_layerMain->m_formTimeBar->End(false);
	GetPlayer()->SetInActivity(false);

	g_layerMain->SendScheduleMsg();
}

void UIFormPlayerItems::OnItemUpdate(RtwWidget* sender, void*)
{
	CItemNetHelper::NetSend_c2r_player_UpDateItemBag();
}

void UIFormPlayerItems::OnGetItemBaseNum(SItemID& item,int& Num,SItemID& itemBase)
{
	guard;
	Num = 0;
	SItemID myItem;
	CBag *pBag;
	pBag = &( m_pPlayer->mItem.m_Bag );
	for(int k = 0; k< ITEM_PLAYER_INVENTORY_PAGE;k++)
	{
		for (int i=0; i<ITEM_PLAYER_INVENTORY_LINE; i++)
		{
			for (int j=0; j<ITEM_PLAYER_INVENTORY_COLUMN; j++)
			{
				myItem = pBag->GetItem(k,i, j);
				if(myItem.type == itemBase.type)
				{
					item = myItem;
					if(!ItemCanPile(itemBase))
						return;
					else
						Num += myItem.count;
				}
			}
		}
	}
	unguard;
}

void UIFormPlayerItems::OnTabSelect(RtwWidget* sender, void*)
{
	guard;
	if(!m_bIsDrag)
		m_iLastTabSelect= m_pTabThis->GetSelectedItem();
	Refresh();
	unguard;
}
void UIFormPlayerItems::SetTabSelect(int i)
{
	guard;
	if(m_bInMissionPage)
		return;
	/*if(m_iLastTabSelect== -1)
		m_iLastTabSelect= m_pTabThis->GetSelectedItem();*/
	m_pTabThis->SetSelectedItem(i);
	GetPlayer()->mItem.m_Bag.SetPageIndex(i);
	//Refresh();
	unguard;
}


void UIFormPlayerItems::OnLClick_btnShop(RtwWidget* sender, void*)
{
	guard;
	//add by yz 2010-6-25: 炼化法宝时不能摆摊
	if (NULL != g_layerMain->m_formChar)
	{
		if (g_layerMain->m_formChar->m_bIsRifine)
		{
			ShowSystemMessage(R(MSG_FABAO_CANNOTBAITAN));
			return;
		}			
	}
	//end yz
	if(GetWorld()->m_pPlayer->m_core.Lev< 10)
	{
		ShowSystemMessage(R(MSG_BAITAN_LEVELNOTENOUGH));
		return;
	}
	if(GetPlayer()->GetRideState())
	{
		ShowSystemMessage(R(MSG_BAITAN_CANNOTWHILERIDE));
		return;
	}
	if(!GetPlayer()->m_bPShop && !g_layerMain->m_formPlayerShop->IsVisible())
	{
		g_layerMain->m_formPlayerShop->OnStartInitShop();
	}else if(g_layerMain->m_formPlayerShop->IsVisible())
	{
		g_layerMain->m_formPlayerShop->Refresh();
	}else
	{
		g_layerMain->m_formPlayerShop->Show();
		g_layerMain->m_formPlayerShop->Refresh();
	}
	unguard;
}

void UIFormPlayerItems::OnLClick_btnSplit(RtwWidget* sender, void*)
{
	guard;
	((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Split, GAME_CURSOR_SPLIT, NULL, R(MSG_ITEM_SELECTTAKEAPARTITEMS));
	unguard;
}

void UIFormPlayerItems::OnSetItemMask(int page, int i, int j, bool isEnable, float values, DWORD color, DWORD Alpha, bool EnableAlpha)
{
	guard;
	if (isEnable)
	{
		if (m_btnGridsCommon[page][i][j]->GetBackground() != NULL)
		{
			if ( (page != m_pPlayer->mItem.m_Bag.GetPageIndex()) || IsInMissionPage())
			{
				m_btnGridsCommon[page][i][j]->EnableAlphaEffect(false);
				m_btnGridsCommon[page][i][j]->SetMaskValue(0.f);
			}
			else 
			{
				m_btnGridsCommon[page][i][j]->EnableAlphaEffect(EnableAlpha);
				m_btnGridsCommon[page][i][j]->SetAlphaRef(Alpha);
				m_btnGridsCommon[page][i][j]->SetMaskColor(RtwPixel(color));
				m_btnGridsCommon[page][i][j]->SetMaskValue(values);
				MaskItem item;item.ItemI = i;item.ItemJ = j;item.ItemPage = page;
				m_vMaskItem.push_back(item);
				m_btnGridsCommon[page][i][j]->ModifyFlags(0,wfDragOut|wfDragServer);
			}
		}
		else 
		{
			m_btnGridsCommon[page][i][j]->EnableAlphaEffect(false);
			m_btnGridsCommon[page][i][j]->SetMaskValue(0.f);
		}
	} 
	else
	{
		m_btnGridsCommon[page][i][j]->EnableAlphaEffect(false);
		m_btnGridsCommon[page][i][j]->SetMaskValue(0.f);
	}
	unguard;
}

void UIFormPlayerItems::OnDisableAllItemMask()
{
	guard;
	vector<MaskItem>::iterator it;
	for(it = m_vMaskItem.begin(); it != m_vMaskItem.end(); ++it)
	{
		OnSetItemMask((*it).ItemPage,(*it).ItemI,(*it).ItemJ);
		m_btnGridsCommon[(*it).ItemPage][(*it).ItemI][(*it).ItemJ]->ModifyFlags(wfDragOut|wfDragServer,0);
	}
	m_vMaskItem.clear();
	unguard;
}

void UIFormPlayerItems::OnDisableItemMask(SItemID *Item)
{
	guard;
	int Page = -1,I = -1,J = -1;
	if(m_pPlayer->mItem.m_Bag.FindID(Item->id, &Page, &I, &J))
	{
		if (Page != -1 && I != -1 && J != -1)
		{
			OnSetItemMask(Page,I,J);
			m_btnGridsCommon[Page][I][J]->ModifyFlags(wfDragOut|wfDragServer,0);
		}
	}
	unguard;
}

void UIFormPlayerItems::OnSetCD(int type,SItemID& item)
{
	guard;
	SItemID myItem;
	CBag *pBag;
	pBag = &( m_pPlayer->mItem.m_Bag );
	for(int k = 0; k< ITEM_PLAYER_INVENTORY_PAGE;k++)
	{
		for (int i=0; i<ITEM_PLAYER_INVENTORY_LINE; i++)
		{
			for (int j=0; j<ITEM_PLAYER_INVENTORY_COLUMN; j++)
			{
				myItem = pBag->GetItem(k,i, j);
				if((item.type& ITEM_MASK_SUBTYPE)== (myItem.type& ITEM_MASK_SUBTYPE))
				{
					OPERATEITEMCD* itemCD= new OPERATEITEMCD();
					itemCD->m_dwOperateTime= rtGetMilliseconds();
					itemCD->m_dwLimitTime= 10000+ s_NetDelay;
					itemCD->m_iPage= k;
					itemCD->m_iI= i;
					itemCD->m_iJ= j;
					itemCD->m_pBtnShortcut= NULL;
					DWORD ID;
					for (int ii=0; ii<UI_HOT_KEY_COUNT; ii++)
					{
						RtwButton *pBtn = g_layerMain->m_hotKeyAlias[ii]->GetButton();
						if (pBtn)
						{
							DWORD *pItemData = (DWORD*)pBtn->GetUserData();
							if (pItemData && pItemData[0]==UIHotKey_Item)
							{
								ID = pItemData[1];
								if (ID == myItem.id)
								{
									itemCD->m_pBtnShortcut = pBtn;
									break;
								}
							}
						}
					}
					m_vecOperateItemCD[type]->push_back(itemCD);
				}
			}
		}
	}
	unguard;
}

void UIFormPlayerItems::OnThisHide(void*,void*)
{
	guard;
	((CGameClientFrame*)GetApp())->CancelMouseCapture();
	bEnchant= false;
	unguard;
}

void UIFormPlayerItems::OnDragOut_CommonItem(RtwWidget* sender, void*)
{
	guard;
	m_bIsDrag= true;
	m_iLastTabSelect= m_pTabThis->GetSelectedItem();
	unguard;
}
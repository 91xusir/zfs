#include "gc_include.h"
#include "item_base.h"
#include "ui_form_itemsinfo.h"

static SCreature s_creature;

UIFormItemsInfo::UIFormItemsInfo()
:m_ItemsContainter(GetItemMgr(), &s_creature)
{
	guard;

	m_pFrmThis = LOAD_UI("fmseeitem");
	RtwRect rc = m_pFrmThis->GetFrameRect();
	m_pFrmThis->Show();
	m_pFrmThis->Move(rc.left-1, rc.top-1);
	m_pFrmThis->Hide();
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

    //m_ptrHoverImage = new RtwImage("highlight_button.tga");// change [3/17/2009 tooth.shi]
	m_ptrHoverImage = g_workspace.getImageFactory()->createImage("ui/x_textures/highlight_button.tga");

	m_btnParts[CItemContainerBase::WEAPON_1] = LOAD_UI_T(RtwButton, "fmseeitem.btnRightHand");
	//m_btnParts[CItemContainerBase::WEAPON_2] = LOAD_UI_T(RtwButton, "fmseeitem.btnLeftHand");
	m_btnParts[CItemContainerBase::WEAPON_3] = LOAD_UI_T(RtwButton, "fmseeitem.btnLeftHand");
	m_btnParts[CItemContainerBase::NECKLACE] = LOAD_UI_T(RtwButton, "fmseeitem.btnNecklace");
	m_btnParts[CItemContainerBase::GLOVE] = LOAD_UI_T(RtwButton, "fmseeitem.btnGlove");
	m_btnParts[CItemContainerBase::RING] = LOAD_UI_T(RtwButton, "fmseeitem.btnRing");
	m_btnParts[CItemContainerBase::BODY] = LOAD_UI_T(RtwButton, "fmseeitem.btnCloth");
	m_btnParts[CItemContainerBase::FOOT] = LOAD_UI_T(RtwButton, "fmseeitem.btnShoes");
	m_btnParts[CItemContainerBase::CHARM] = LOAD_UI_T(RtwButton, "fmseeitem.btnCharm");

	m_btnParts[CItemContainerBase::CAESTUS] = LOAD_UI_T(RtwButton, "fmseeitem.btnCaestus");
	m_btnParts[CItemContainerBase::RING2] = LOAD_UI_T(RtwButton, "fmseeitem.btnRing2");
	m_btnParts[CItemContainerBase::KITS] = LOAD_UI_T(RtwButton, "fmseeitem.btnKits");
	m_btnParts[CItemContainerBase::TRUMP] = LOAD_UI_T(RtwButton, "fmseeitem.btnTrump");
	m_btnParts[CItemContainerBase::FASHION] = LOAD_UI_T(RtwButton, "fmseeitem.btnFashion");
	m_btnParts[CItemContainerBase::SEALEDBOOK] = LOAD_UI_T(RtwButton, "fmseeitem.btnBook");
	m_btnParts[CItemContainerBase::AEROCRAFT] = LOAD_UI_T(RtwButton, "fmseeitem.btnFly");

	m_pPlayerName = LOAD_UI("fmseeitem.lbname");

    for (int nPart=0; nPart<CItemContainerBase::MAX_EQUIP_PARTS; nPart++)
    {
		if (nPart == CItemContainerBase::WEAPON_2)
		{
			continue;
		}
        m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
        m_ptrHoverImage->getRenderImage()->GetImage()->eBlendDstFunc = RTGBLEND_ONE;
        m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
        m_ptrHoverImage->getRenderImage()->SetBlend(true);
        m_btnParts[nPart]->SetHoverImage(m_ptrHoverImage);
		m_btnParts[nPart]->ModifyFlags(wfHint,0/*, wfDragClient|wfDragServer|wfClick|wfDClick|wfHint*/);
		m_btnParts[nPart]->EvHint += RTW_CALLBACK(this, UIFormItemsInfo, OnGetHintText_Parts);
    }

	rt2_strncpy(m_iconUnquip[CItemContainerBase::WEAPON_1], "ui/zfs_texture/btnHand.tga", PATH_STRING_LEN);
	//strncpy(m_iconUnquip[CItemContainerBase::WEAPON_2], "ui/zfs_texture/btnHand.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::NECKLACE], "ui/zfs_texture/btnNecklace.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::GLOVE], "ui/zfs_texture/btnGlove.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::RING], "ui/zfs_texture/btnRing.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::BODY], "ui/zfs_texture/btnCloth.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::CHARM], "ui/zfs_texture/btnCharm.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::FOOT], "ui/zfs_texture/btnShoes.tga", PATH_STRING_LEN);

	rt2_strncpy(m_iconUnquip[CItemContainerBase::CAESTUS], "ui/zfs_texture/btnCaestus.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::RING2], "ui/zfs_texture/btnRing.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::NECKLACE], "ui/zfs_texture/btnNecklace.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::KITS], "ui/zfs_texture/btnKits.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::TRUMP], "ui/zfs_texture/btnTrump.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::FASHION], "ui/zfs_texture/btnFashion.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::SEALEDBOOK], "ui/zfs_texture/btnBook.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::AEROCRAFT], "ui/zfs_texture/btnFly.tga", PATH_STRING_LEN);
	rt2_strncpy(m_iconUnquip[CItemContainerBase::WEAPON_3], "ui/zfs_texture/btnHand.tga", PATH_STRING_LEN);

	m_PlayerID = 0;
	m_PlayerName = "";
	m_nMaxDistance = 300;

	unguard;
}

UIFormItemsInfo::~UIFormItemsInfo()
{
	guard;

	unguard;
}

void UIFormItemsInfo::OnPreDraw()
{
	guard;

	CItemContainerBase *pC = &m_ItemsContainter;
	SItemID item;
	SItemBase *pItemClass = NULL;
	int part;
	bool bSkipWeapon2 = false;

	m_pPlayerName->SetText(m_PlayerName);
	
	// 身体上的装备
	for (part=CItemContainerBase::EQUIP_DEFAULT_PART+1; part<CItemContainerBase::MAX_EQUIP_PARTS; part++)
	{
		if (/*bSkipWeapon2 && */part==CItemContainerBase::WEAPON_2)
			continue;

		item = pC->GetEquipItem((CItemContainerBase::EEquipParts)part, true);
		if (ItemID_IsValid(item))
		{
			pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
			if (pItemClass && strlen(pItemClass->icon) != 0)
			{
//				if (part==CItemContainerBase::WEAPON_1 && ((SWeapon*)pItemClass)->bBothHands)
//				{// 双手武器,右手显示为虚的
//					bSkipWeapon2 = true;
////					RtwImage* pImage1 = new RtwImage(pItemClass->icon);// change [3/17/2009 tooth.shi]
//					RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
//					pImage1->SetBlend(true);
//					m_btnParts[CItemContainerBase::WEAPON_1]->SetBackgroundImage(pImage1);
//
//					//RtwImage* pImage2 = new RtwImage(pItemClass->icon);
//					//RtwImage* pImage2 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
//					//m_btnParts[CItemContainerBase::WEAPON_2]->SetBackgroundImage(pImage2);
//					//pImage2->SetColor(0xff7f7f7f);
//				}
//				else 
				{
					//RtwImage* pImage = new RtwImage(pItemClass->icon);
					RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
					if (pImage)
					{
						pImage->SetBlend(true);
						m_btnParts[part]->SetBackgroundImage(pImage);
					}
					else
					{
						//m_btnParts[part]->SetBackgroundImage(NULL);
					}
				}
			}

			m_btnParts[part]->EnableAlphaEffect(false);
			m_btnParts[part]->ModifyFlags(wfHint, 0);
		}
		else
		{
			//RtwImage* pImage = new RtwImage(m_iconUnquip[part]);
			RtwImage* pImage = g_workspace.getImageFactory()->createImage(m_iconUnquip[part]);
			pImage->SetBlend(true);
			m_btnParts[part]->SetBackgroundImage(pImage);

            m_btnParts[part]->EnableAlphaEffect(false);
			m_btnParts[part]->ModifyFlags(0, wfHint);

            //RtwComplexImage hoverImg;
            //m_btnParts[part]->SetHoverImage(hoverImg);
		}
	}


	unguard;
}

void UIFormItemsInfo::Show()
{
	guard;

	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormItemsInfo::IsShowing()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormItemsInfo::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormItemsInfo::Refresh()
{
	guard;
	if ( IsShowing() )		Show();
	unguard;
}

void UIFormItemsInfo::OnGetHintText_Parts(RtwWidget* sender, void*)
{
	guard;

	const string& widgetID = sender->GetID();
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(widgetID, part);
	SItemID itemID = m_ItemsContainter.GetEquipItem(part, true);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, GetWorld()->m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();

	unguard;
}


bool UIFormItemsInfo::Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart)
{
	guard;
	int ii;
	for (ii=0; ii<CItemContainerBase::MAX_EQUIP_PARTS; ii++)
	{
		if (ii == CItemContainerBase::WEAPON_2)
		{
			continue;
		}
		if ( id==m_btnParts[ii]->GetID() )
			break;
	}
	if (ii<CItemContainerBase::MAX_EQUIP_PARTS)
	{
		outPart = (CItemContainerBase::EEquipParts)ii;
		return true;
	}
	else
		return false;

	unguard;
}

void UIFormItemsInfo::OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData)
{
	guard;

	if (pTargetActor==NULL) return;

	if (   pTargetActor->NpcType() != GcActor::ENT_NPC_SERVICE
		&& pTargetActor->NpcType() != GcActor::ENT_MONSTER
		&& pTargetActor->NpcType() != GcActor::ENT_NPC_COMBATIVE)
	{
		pPopupMenu->AddItem(R(G_SEEITEMS), NULL, (void*)iData);
	}

	unguard;
}

bool UIFormItemsInfo::OnCommand(GcActor* pTargetActor)
{
	guard;
	if (pTargetActor==NULL) return false;

	if (pTargetActor->NpcType()==GcActor::ENT_NPC_SERVICE ||
		pTargetActor->NpcType()==GcActor::ENT_NPC_COMBATIVE ||
		pTargetActor->NpcType()==GcActor::ENT_MONSTER)
	{
		PlayerFailOperate(R(THAT_OBJECT_NOT_PLAYER));
		return false;
	}

	int currTime = rtGetMilliseconds();

	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_query_player_info);
	pPacket->WriteLong(pTargetActor->ID());
	NetSend(pPacket);

	return true;

	unguard;
}

bool UIFormItemsInfo::MouseRClick(int x, int y)
{
	guard;
	return OnCommand(GetWorld()->HitTest(x, y, 0xFC));
	unguard;
}


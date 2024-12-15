#include "gc_include.h"
#include "gc_player_items.h"
#include "gc_world.h"
#include "ui_form_playeritems.h"
#include "ui_form_bank.h"
#include "ui_form_item_update.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_shop.h"
#include "ui_form_trade.h"
#include "trump_show.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_trump.h"
#include "UIForm_Char.h"

DWORD GetNextItemID() {
    return 1;
}

CItemContainer::CItemContainer(CItemManager* pManager, SCreature* pCre, GcActor* pActor)
    : CItemContainerBase(pManager, pCre) {
    m_pMaster = pActor;
    memset(mpDefaultItem, 0, sizeof(char*) * MAX_EQUIP_PARTS);
}

CItemContainer::~CItemContainer() {}

bool CItemContainer::OperateItem_CheckUIContent(SItemID& item, EItemOperate op) {
    guard;

    if (!g_layerMain)
        return false;

    EEquipParts part;
    int         page, gridI, gridJ;
    if (FindInEquipItems(item, part)) {
    } else if (m_Bag.Find(item, &page, &gridI, &gridJ)) {
    } else if (m_MissionBag.Find(item, &page, &gridI, &gridI)) {
    } else if (m_PetBag.Find(item, &page, &gridI, &gridI)) {
    } else {
        ERR2("[OperateItem_CheckUIContent] Can NOT find the item!  item(ID=%d, Index=%d)\n",
             item.id, m_pItemManager->GetIndexFromType(item.type));
        return false;
    }

    switch (op) {
        case IO_Throw:
            if (g_layerMain->m_formTrade->IsShowing()) {
                PlayerFailOperate(R(MSG_ITEM_THROW_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formShop->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_THROW_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formItemUpdate->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_THROW_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formGem->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_THROW_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formBank->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
                return false;
            } else if (g_layerMain->m_formItemCompose->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSEDUANZAO));
                return false;
            } else if (g_layerMain->m_formItemFuse->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_SHUTCOMPOSEFIRST));
                return false;
            }
            break;

        case IO_Delete:
            if (g_layerMain->m_formTrade->IsShowing()) {
                PlayerFailOperate(R(MSG_ITEM_DELITEM_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formShop->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_DELITEM_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formItemUpdate->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_DELITEM_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formGem->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_DELITEM_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formBank->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
                return false;
            } else if (g_layerMain->m_formItemCompose->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSEDUANZAO));
                return false;
            } else if (g_layerMain->m_formItemFuse->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_SHUTCOMPOSEFIRST));
                return false;
            }
            break;

        case IO_Move:
            break;

        case IO_Equip:
        case IO_UnEquip:
            if (g_layerMain && g_layerMain->m_formItemUpdate->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL_ITEMUPDATE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain && g_layerMain->m_formGem->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL_ITEMUPDATE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain && g_layerMain->m_formTrade->IsShowing()) {
                PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL_TRADE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain->m_formShop->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formBank->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
                return false;
            } else if (g_layerMain->m_formItemCompose->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSEDUANZAO));
                return false;
            } else if (g_layerMain->m_formItemFuse->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_SHUTCOMPOSEFIRST));
                return false;
            }
            break;

        case IO_Eat:
            if (g_layerMain->m_formItemUpdate->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_EAT_FAIL_ITEMUPDATE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain->m_formGem->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_EAT_FAIL_ITEMUPDATE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain->m_formTrade->IsShowing()) {
                PlayerFailOperate(R(MSG_ITEM_EAT_FAIL_TRADE), true, "cannotpickup.wav");
                return false;
            } else if (g_layerMain->m_formShop->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_OPERATE_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formBank->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
                return false;
            } else if (g_layerMain->m_formItemCompose->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSEDUANZAO));
                return false;
            } else if (g_layerMain->m_formItemFuse->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_SHUTCOMPOSEFIRST));
                return false;
            }
            break;

        case IO_UsePet:
        case IO_UseScroll:
        case IO_UseOther:
        case IO_EnchantItem:
        case IO_Decompose:
            if (g_layerMain->m_formTrade->IsShowing()) {
                PlayerFailOperate(R(MSG_ITEM_OPERATE_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formShop->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_OPERATE_FAIL_TRADE));
                return false;
            } else if (g_layerMain->m_formItemUpdate->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_OPERATE_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formGem->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_OPERATE_FAIL_ITEMUPDATE));
                return false;
            } else if (g_layerMain->m_formBank->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSE_BANK));
                return false;
            } else if (g_layerMain->m_formItemCompose->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_CLOSEDUANZAO));
                return false;
            } else if (g_layerMain->m_formItemFuse->IsVisible()) {
                PlayerFailOperate(R(MSG_ITEM_SHUTCOMPOSEFIRST));
                return false;
            }
            break;

        case IO_Lock:
            break;

        default:
            ERR("[OperateItem_CheckUIContent]Invalid Operate!\n");
            break;
    }

    return true;
    unguard;
}

void CItemContainer::EquipDefaultItem(EEquipParts vPart, const char vpIDName[]) {
    guard;

    if (!_IsValidPart(vPart))
        return;
    mpDefaultItem[vPart] = vpIDName;

    //	LOG1("Equip Default Skin(part=%d)", (int)vPart);

    unguard;
}

void CItemContainer::Run(unsigned long dwMillisecond) {
    guard;

    if (GetWorld() && (GetWorld()->m_pPlayer == m_pMaster)) {
        if (g_layerMain)
            g_layerMain->m_fromPlayerItems->Run(dwMillisecond);
    }

    unguard;
}

void CItemContainer::WeaponPlayPose(const char* szLinkName) {
    guard;

    if (!m_pMaster)
        return;

    GcActorGraph* pWeaponGraph = m_pMaster->GetGraph()->GetLinkActor(szLinkName);
    if (pWeaponGraph) {
        pWeaponGraph->PlayPose("effect", true);
    }

    unguard;
}

// ����ĳ����λ��ͼ��
void CItemContainer::_UpdateGraph(EEquipParts part) {
    guard;

    if (!m_pMaster || m_pMaster->m_cShapeshiftNet == 2)
        return;

    const SItemBase* pItemClass = NULL;
    SItemID          item       = GetEquipItem(part, true);
    SItemID          itemLast   = m_nLastItemID[part];

    if (item == itemLast)
        return;

    int itemLevel;
    if (!ItemID_IsValid(item)) {
        if (ItemID_IsValid(m_nLastItemID[part])) {
            pItemClass = m_pItemManager->GetItem(m_nLastItemID[part].type);
            if (!pItemClass)
                return;
            itemLevel = m_nLastItemID[part].level;
            if (itemLevel > ITEM_MAX_ITEM_LEVEL) {
                CHECKEX("���ߵȼ����Ϸ�");
                return;
            }

            ItemID_SetInvalid(m_nLastItemID[part]);
        }

        switch (part) {
            case WEAPON_1: {
                m_pMaster->EquipWeapon(false, NULL, NULL, ItemID_CreateInvalid());
            } break;
            case WEAPON_2:  //lyytodo ˫����
            {
                //if (m_pMaster->m_core.Metier==METIER_HUNTER)
                //{
                //}
                //else
                //{
                //	if (ItemID_IsValid(m_nLastItemID[WEAPON_1]))
                //	{
                //		SWeapon *pWeaClass1 = (SWeapon*)m_pItemManager->GetItem(m_nLastItemID[WEAPON_1].type);
                //		if (pWeaClass1 && (pWeaClass1->bBothHands) && (pWeaClass1->Models2[m_nLastItemID[WEAPON_1].level][0]!='\0'))
                //			break;
                //	}
                //	m_pMaster->GetGraph()->UnLink("Box01");
                //}

                ////				if (m_pMaster->IsIdel())
                //				{
                //					m_pMaster->mBaseActor.PlayPose(GcBaseActor::POSE_STAND, true);
                //				}
            } break;
            case WEAPON_3:
                break;
            case TRUMP: {
                if (ItemIsTrump(itemLast.type)) {
                    m_pMaster->EquipTrump(false, NULL, ItemID_CreateInvalid());
                }
            } break;
            case BODY:
            case FOOT:
            case GLOVE:
            case CAESTUS: {
                if (pItemClass) {
                    char Sex = m_pMaster->m_core.Sex;
                    if (Sex) {
                        // Ůģ��
                        m_pMaster->GetGraph()->UnloadSkin(
                            ((SArmor*)pItemClass)->UpdateSkins[itemLevel]);
                    } else {
                        // ��ģ��
                        m_pMaster->GetGraph()->UnloadSkin(
                            ((SArmor*)pItemClass)->UpdateSkins2[itemLevel]);
                    }
                    //m_pMaster->GetGraph()->UnloadSkin(((SArmor*)pItemClass)->UpdateSkins[itemLevel]);
                    if (((SArmor*)pItemClass)->UpdateEffects[itemLevel][0] != '\0') {
                        m_pMaster->mBaseActor.GetGraph()->RemoveEffect(
                            ((SArmor*)pItemClass)->UpdateEffects[itemLevel]);
                    }

                    if (mpDefaultItem[part] != NULL) {
                        m_pMaster->GetGraph()->LoadSkin(mpDefaultItem[part], false);
                    }
                }
            } break;
        }
    } else {
        pItemClass = m_pItemManager->GetItem(item.type);
        if (!pItemClass || !m_pMaster)
            return;
        SWeapon* pWeapon = (SWeapon*)pItemClass;
        itemLevel        = item.level;
        if (itemLevel > ITEM_MAX_ITEM_LEVEL) {
            CHECKEX("���ߵȼ����Ϸ�");
            return;
        }

        if (ItemIsWeapon(item)) {
            // ���������ְҵlink
            char Sex = m_pMaster->m_core.Sex;

            if (part == WEAPON_1) {
                // ��ԭ����ж��
                if (ItemID_IsValid(m_nLastItemID[part])) {
                    m_pMaster->EquipWeapon(false, NULL, NULL, ItemID_CreateInvalid());
                }

                if (Sex) {
                    // Ůģ��
                    m_pMaster->EquipWeapon(true, pWeapon->Models1[itemLevel], pWeapon, item);
                } else {
                    // ��ģ��
                    m_pMaster->EquipWeapon(true, pWeapon->Models2[itemLevel], pWeapon, item);
                }
            }

        } else if (ItemIsArmor(item)) {
            char Sex = m_pMaster->m_core.Sex;
            if (ItemID_IsValid(m_nLastItemID[part])) {
                SItemBase* pItemBaseLast = m_pItemManager->GetItem(m_nLastItemID[part].type);
                if (!pItemBaseLast)
                    return;
                SArmor* pArmorLast    = (SArmor*)pItemBaseLast;
                int     itemLevelLast = m_nLastItemID[part].level;
                if (Sex) {
                    // Ůģ��
                    m_pMaster->GetGraph()->UnloadSkin(pArmorLast->UpdateSkins[itemLevelLast]);
                } else {
                    // ��ģ��
                    m_pMaster->GetGraph()->UnloadSkin(pArmorLast->UpdateSkins2[itemLevelLast]);
                }
                if (pArmorLast->UpdateEffects[itemLevelLast][0] != '\0') {
                    //					m_pMaster->mBaseActor.StopEffect(pArmorLast->UpdateEffects[itemLevelLast]);
                    m_pMaster->mBaseActor.GetGraph()->RemoveEffect(
                        pArmorLast->UpdateEffects[itemLevelLast]);
                }
            } else if (mpDefaultItem[part] != NULL) {
                m_pMaster->GetGraph()->UnloadSkin(mpDefaultItem[part]);
            }

            if (Sex) {
                // Ůģ��
                m_pMaster->GetGraph()->LoadSkin(((SArmor*)pItemClass)->UpdateSkins[itemLevel],
                                                false);  //�����µ�skin
            } else {
                // ��ģ��
                m_pMaster->GetGraph()->LoadSkin(((SArmor*)pItemClass)->UpdateSkins2[itemLevel],
                                                false);  //�����µ�skin
            }

            if (((SArmor*)pItemClass)->UpdateEffects[itemLevel][0] != '\0') {
                m_pMaster->mBaseActor.GetGraph()->ApplyEffect(
                    ((SArmor*)pItemClass)->UpdateEffects[itemLevel]);
            }
        } else if (ItemIsTrump(item)) {
            STrump* pTrump = (STrump*)pItemClass;
            if (ItemID_IsValid(m_nLastItemID[part])) {
                STrump* pWea = (STrump*)m_pItemManager->GetItem(m_nLastItemID[part].type);
                if (pWea && pWea->Models1[m_nLastItemID[part].level][0] != '\0')
                    m_pMaster->EquipTrump(false, pWea->Models1[m_nLastItemID[part].level],
                                          m_nLastItemID[part]);
            }

            if (pWeapon->Models1[itemLevel][0] != '\0')
                m_pMaster->EquipTrump(true, pWeapon->Models1[itemLevel], item, pTrump);
        }

        m_nLastItemID[part] = item;
    }

    if (m_pMaster == GetPlayer() && g_layerMain->m_formChar->IsVisible() &&
        g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP) {
        g_layerMain->m_formChar->ReCreate3DViewGraph();
    }

    if (m_pMaster == GetPlayer() && part == TRUMP && g_layerMain->m_formChar->IsVisible() &&
        g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::TRUMP) {
        g_layerMain->m_formChar->OnShowTrump();
    }

    unguard;
}

bool CItemContainer::CheckEquip(EEquipParts vPart, SItemID& item, SCreature& cre) {
    guard;

    if (!CItemContainerBase::CheckEquip(vPart, item, cre))
        return false;

    // װ��˫��ʱ����Ƿ�ѧϰ�˼���
    if (vPart == WEAPON_1 && ItemIsWeapon_Pestle(item)) {
        SItemID item2 = GetEquipItem(WEAPON_2, true);
        if (ItemIsWeapon_Pestle(item2)) {
            if (!(m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_DAMAGE) ||
                  m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_ATTACK)))
                return false;
        }
    } else if (vPart == WEAPON_2 && ItemIsWeapon_Pestle(item)) {
        SItemID item1 = GetEquipItem(WEAPON_1, true);
        if (ItemIsWeapon_Pestle(item1)) {
            if (!(m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_DAMAGE) ||
                  m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_ATTACK)))
                return false;
        }
    }

    return true;

    unguard;
}

void CItemContainer::_Equip(SItemID& id, EEquipParts vPart) {
    guard;

    if (m_pMaster)
        LOG2("[_Equip] ActorName=%s, Item.id=%d\n", m_pMaster->GetName(), id.id);

    CItemContainerBase::_Equip(id, vPart);

    _UpdateGraph(vPart);

    unguard;
}

char CItemContainer::SwitchSpareWeapon() {
    guard;

    //SItemID item1 = GetEquipItem(CItemContainerBase::WEAPON_1, true);
    //if ( !ItemID_IsValid(item1) )
    //{
    //	return 1;
    //}

    //SItemID item3 = GetEquipItem(CItemContainerBase::WEAPON_3, true);
    //if ( !ItemID_IsValid(item3) )
    //{
    //	return 2;
    //}
    //_UnEquip(CItemContainerBase::WEAPON_1);
    //_UnEquip(CItemContainerBase::WEAPON_3);
    //_Equip(item3, CItemContainerBase::WEAPON_1);
    //_Equip(item1, CItemContainerBase::WEAPON_3);
    //_UpdateGraph(WEAPON_1);
    //_UpdateGraph(WEAPON_3);

    //if (g_layerMain->m_fromPlayerItems->IsShowing())
    //{
    //	g_layerMain->m_fromPlayerItems->Show();
    //}
    GetPlayer()->RemoveCurCommand();
    CItemNetHelper::NetSend_c2r_player_switchweapon();
    return 0;

    unguard;
}

bool CItemContainer::_UnEquip(EEquipParts vPart, bool bAddToBag /* = false*/) {
    guard;

    if (m_pMaster) {
        SItemID item = m_pMaster->mItem.GetEquipItem(vPart, true);
        LOG2("[_UnEquip] ActorName=%s, Item.id=%d\n", m_pMaster->GetName(), item.id);
    }

    SItemID id = GetEquipItem(vPart, true);
    if (!ItemID_IsValid(id))
        return true;

    if (!CItemContainerBase::_UnEquip(vPart, bAddToBag))
        return false;

    _UpdateGraph(vPart);

    return true;

    unguard;
}

bool CItemContainer::UnEquipAll(bool bAddToBag /* = false*/) {
    guard;

    for (int part = EQUIP_DEFAULT_PART + 1; part < MAX_EQUIP_PARTS; part++) {
        if (!UnEquip((EEquipParts)part, bAddToBag)) {
            // @@@@
            return false;
        }
    }
    return true;

    unguard;
}

bool CItemContainer::CheckUse(SItemID& item, SCreature& cre) const {
    if (!CItemContainerBase::CheckUse(item, cre))
        return false;

    SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(item.type);
    BOOL_ENSURE(pItemBase);

    switch (pItemBase->timeLimit) {
        case IUL_Free:
            break;
        case IUL_FeodWar: {
            bool bCanUse = true;
            // ��սʱ���ڣ������ڳ�ս�����ڲſ���ʹ��
            if (!GetWorld()->IsCastleWarTime(GetWorld()->m_pPlayer))
                bCanUse = false;
            else {
                if (!GetWorld()->m_pPlayer->InCastleWarArea())
                    bCanUse = false;
                else
                    bCanUse = true;
            }

            if (!bCanUse) {
                PlayerFailOperate(R(MSG_ITEM_USELIMIT_FEODWAR), true);
                return false;
            }
        } break;
        case IUL_NationalWar: {
            bool bCanUse = true;
            // ��սʱ���ڣ������ڹ�ս�����ڲſ���ʹ��

            if (!bCanUse) {
                PlayerFailOperate(R(MSG_ITEM_USINGLIMITEDINBATTLE), true);
                return false;
            }
        } break;
        default:
            break;
    }

    if (ItemIsOther_Spawn(item.type)) {
        // ʹ������
        DWORD now   = time(NULL);
        DWORD lLife = item.params[ITEMSPAWN_PARAM_LIFE];
        if (lLife != 0 && now >= lLife) {
            PlayerFailOperate(R(MSG_ITEM_ITEMDATEHASPASS), true);
            return false;
        }
    }

    return true;
}

void CItemContainer::OnAfterEquip(SItemID& item, EEquipParts vPart) {
    guard;

    GcBaseActor::EPoses pose = m_pMaster->mBaseActor.m_vPoseID;
    if (pose == GcBaseActor::POSE_STAND || pose == GcBaseActor::POSE_GUARD ||
        pose == GcBaseActor::POSE_IDLE) {
        m_pMaster->mBaseActor.PlayPose(pose);
    } else if (pose == GcBaseActor::POSE_RUN || pose == GcBaseActor::POSE_WALK) {
        m_pMaster->mBaseActor.m_fLastMoveSpeed = 0;
    }

    if (m_pSuit && !m_pLastSuit) {
        if (GetWorld()) {
            if (!m_pSuit->effect.empty() &&
                !m_pMaster->mBaseActor.FindEffect(m_pSuit->effect.c_str()))
                m_pMaster->mBaseActor.PlayEffect(m_pSuit->effect.c_str(), true, NULL,
                                                 RtgVertex3(0, 0, 0), true);
        }
    }
    if (vPart == CItemContainerBase::TRUMP && g_layerMain->m_formTrump->IsVisiable()) {
        g_layerMain->m_formTrump->OnPreDraw();
    }
    //Heten �޸���װ��������
    if (GetWorld()->m_pPlayer->GetAutobattle()) {
        GetWorld()->m_pPlayer->ResetTargetID();
    }
    //end

    unguard;
}

void CItemContainer::OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld) {
    guard;

    GcBaseActor::EPoses pose = m_pMaster->mBaseActor.m_vPoseID;
    if (pose == GcBaseActor::POSE_STAND || pose == GcBaseActor::POSE_GUARD ||
        pose == GcBaseActor::POSE_IDLE) {
        m_pMaster->mBaseActor.PlayPose(pose);
    } else if (pose == GcBaseActor::POSE_RUN || pose == GcBaseActor::POSE_WALK) {
        m_pMaster->mBaseActor.m_fLastMoveSpeed = 0;
    }

    if (!m_pSuit && m_pLastSuit) {
        if (GetWorld()) {
            if (!m_pLastSuit->effect.empty())
                m_pMaster->mBaseActor.StopEffect(m_pLastSuit->effect.c_str());
        }
    }
    if (vPart == CItemContainerBase::TRUMP && g_layerMain->m_formTrump->IsVisiable()) {
        g_layerMain->m_formTrump->OnPreDraw();
    }
    //Heten �޸���װ��������
    if (GetWorld()->m_pPlayer->GetAutobattle()) {
        GetWorld()->m_pPlayer->ResetTargetID();
    }
    //end

    unguard;
}

void CItemContainer::UpdateGraph() {
    guard;

    LOG1("[CItemContainer::UpdateGraph] ActorName=%s\n", m_pMaster->GetName());

    for (int vPart = EQUIP_DEFAULT_PART + 1; vPart < MAX_EQUIP_PARTS; vPart++) {
        _UpdateGraph((EEquipParts)vPart);
    }

    //m_pMaster->mBaseActor.PlayPose(GcBaseActor::POSE_STAND, false);

    if (m_pSuit) {
        if (!m_pSuit->effect.empty() && !m_pMaster->mBaseActor.FindEffect(m_pSuit->effect.c_str()))
            m_pMaster->mBaseActor.PlayEffect(m_pSuit->effect.c_str(), true, NULL,
                                             RtgVertex3(0, 0, 0), true);
    } else {
        if (m_pLastSuit) {
            if (!m_pLastSuit->effect.empty())
                m_pMaster->mBaseActor.StopEffect(m_pLastSuit->effect.c_str());
        }
    }

    unguard;
}

GcActorGraph* CItemContainer::GetGraph(EEquipParts vPart) {
    guard;

    if (!_IsValidPart(vPart))
        return false;

    switch (vPart) {
        case WEAPON_1:
            return m_pMaster->GetGraph()->GetLinkActor("Box02");
        case WEAPON_2:
            return m_pMaster->GetGraph()->GetLinkActor("Box01");
        default:
            return NULL;
    }

    unguard;
}

void CItemContainer::OnResetGraph() {
    guard;

    for (int i = 0; i < MAX_EQUIP_PARTS; i++) {
        m_nLastItemID[i] = ItemID_CreateInvalid();
    }

    unguard;
}

void CItemContainer::ExchangeHandWeapon() {
    guard;

    SItemID itemBak = GetEquipItem(WEAPON_1, true);
    SItemID w2      = GetEquipItem(WEAPON_3, true);
    bool    b1      = m_bEquipValid[WEAPON_1];
    bool    b2      = m_bEquipValid[WEAPON_3];

    m_nItemID[WEAPON_1]     = w2;
    m_nItemID[WEAPON_3]     = itemBak;
    m_bEquipValid[WEAPON_1] = b2;
    m_bEquipValid[WEAPON_3] = b1;

    //SetEquipItem(WEAPON_1, w2);
    //SetEquipItem(WEAPON_2, itemBak);

    _UpdateGraph(WEAPON_1);
    _UpdateGraph(WEAPON_3);
    OnAfterEquip(m_nItemID[WEAPON_1], WEAPON_1);
    OnAfterEquip(m_nItemID[WEAPON_3], WEAPON_3);

    unguard;
}

void CItemContainer::ChangeModel(SItemID& itemOrig, SItemID& itemNew, EEquipParts part) {
    if (itemNew.type != itemOrig.type)
        return;

    if (ItemIsTrump(itemNew.type)) {
        if (m_pMaster->mBaseActor.m_pTrump) {
            SWeapon* pWeaClass = (SWeapon*)m_pItemManager->GetItem(itemNew.type);
            if (pWeaClass && pWeaClass->Models1[itemNew.level][0])
                m_pMaster->mBaseActor.m_pTrump->ChangeModel(pWeaClass->Models1[itemNew.level],
                                                            itemNew);
        }
    }
}

void CItemContainer::SetEquipData(SItemID& item, EEquipParts part) {
    guard;

    m_nItemID[part] = item;

    _UpdateGraph(part);
    unguard;
}

void CItemContainer::SetEquipItemValid(EEquipParts part, bool bValid) {
    guard;

    m_bEquipValid[part] = bValid;

    unguard;
}

int CItemContainer::CalcHintLineCount(const char* str) {
    guard;

    static int FONT_WIDTH = 7;
    int        strLen     = strlen(str);
    return (strLen * FONT_WIDTH / ITEM_HINT_WIDTH) + 1;

    unguard;
}

RtString CItemContainer::GenerateAttribString(SItemBase* pItemClass, SAttribAddtion& attrib,
                                              BYTE rate, bool bItemRelated /* = true*/) {
    guard;

    if (!pItemClass)
        return "";

    if (attrib.attrib == ITEM_ATTRIB_APPENDSKILL || attrib.attrib == ITEM_ATTRIB_SKILLONE ||
        attrib.attrib == ITEM_ATTRIB_SKILLTWO) {
        RtString str = R(HINT_SKILL_STRING);
        str += ": ";
        WORD    skillID = attrib.count;
        SSkill* pSkill  = Skill()->FindSkill(skillID);
        if (!pSkill)
            return str;
        str += pSkill->szName;
        str += "(";
        str += R(Level);
        str += rtFormatNumber(pSkill->iLevel);
        str += ")";
        return str;
    } else if (attrib.attrib == ITEM_ATTRIB_RELEASEEFFECT_SELF) {
        RtString      str;
        WORD          effectID   = attrib.count;
        BYTE          effectRate = attrib.skillRate;
        SSkillEffect* pEffect    = Skill()->FindEffect(effectID);
        if (!pEffect)
            return "";
        str.Format(R(MSG_RELEASEEFFECT_SELF), effectRate, pEffect->cLevel, pEffect->szName);
        return str;
    } else if (attrib.attrib == ITEM_ATTRIB_RELEASEEFFECT_ENEMY) {
        RtString      str;
        WORD          effectID   = attrib.count;
        BYTE          effectRate = attrib.skillRate;
        SSkillEffect* pEffect    = Skill()->FindEffect(effectID);
        if (!pEffect)
            return "";
        str.Format(R(MSG_RELEASEEFFECT_ENEMY), effectRate, pEffect->cLevel, pEffect->szName);
        return str;
    }

    RtString str   = GetAttribString(attrib.attrib);
    double   fRate = rate / 100.0f;
    if (fRate <= 0) {
        ERR("[CItemContainer::GenerateAttribString] Invalid attribute's rate!\n");
        fRate = 0.0f;
    }
    str += ": +";
    int Num = 0;
    switch (attrib.attrib) {
        case ITEM_ATTRIB_RATE_FIREARMOR:
        case ITEM_ATTRIB_RATE_WATERARMOR:
        case ITEM_ATTRIB_RATE_POISONARMOR:
        case ITEM_ATTRIB_RATE_FIREDAMAGE:
        case ITEM_ATTRIB_RATE_WATERDAMAGE:
        case ITEM_ATTRIB_RATE_POISONDAMAGE:
            if (bItemRelated)
                Num =
                    (int)(fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level] / 100.0f) *
                          attrib.count);
            else
                Num = (int)(fRate * attrib.count);
            str += rtFormatNumber(Num);
            str += "%";
            break;
        case ITEM_ATTRIB_CON:
        case ITEM_ATTRIB_STR:
        case ITEM_ATTRIB_HIT:
        case ITEM_ATTRIB_INT:
        case ITEM_ATTRIB_DEX:
        case ITEM_ATTRIB_CON_POWER:
        case ITEM_ATTRIB_STR_POWER:
        case ITEM_ATTRIB_DEX_POWER:
        case ITEM_ATTRIB_INT_POWER:
        case ITEM_ATTRIB_HIT_POWER:
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count *
                              pItemClass->requirement[ItemRequirement_Level] / 100.0f);
            else
                Num = rtRound(fRate * attrib.count / 100.0f);
            str += rtFormatNumber(Num >= 1 ? Num : 1);
            break;
        case ITEM_ATTRIB_RATE_ATTACK:
        case ITEM_ATTRIB_RATE_ARMOR:
        case ITEM_ATTRIB_RATE_DAMAGE:
        case ITEM_ATTRIB_RATE_DODGE:
        case ITEM_ATTRIB_RATE_ATTACKSPEED:  //�����ٶ�
            if (bItemRelated)
                Num = rtRound(fRate *
                              (1.0f + pItemClass->requirement[ItemRequirement_Level] / 100.0f) *
                              attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num);
            str += "%";
            break;
        case ITEM_ATTRIB_ATTACKRANGE:  //��������
            if (bItemRelated)
                Num = rtRound(fRate *
                              (1.0f + pItemClass->requirement[ItemRequirement_Level] / 100.0f) *
                              attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num >= 1 ? Num : 1);
            break;
        case ITEM_ATTRIB_SKILLCRITICAL_DAM:   //������
        case ITEM_ATTRIB_SKILLCRITICAL_RATE:  //�����˺�
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num);
            str += "%";
            break;
        case ITEM_ATTRIB_CRITICALHIT:  //��ɱ��
            if (bItemRelated)
                Num = rtRound(fRate *
                              (1.0f + pItemClass->requirement[ItemRequirement_Level] / 100.0f) *
                              attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num);
            str += "%";
            break;
        case ITEM_ATTRIB_ABSORBHP:  //��Ѫ
        case ITEM_ATTRIB_ABSORBMP:  //��ħ
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num >= 1 ? Num : 1);
            break;
        case ITEM_ATTRIB_REPARRY:          // ��
        case ITEM_ATTRIB_REFAINT:          // ���λ�
        case ITEM_ATTRIB_RESKILLCRITICAL:  // ������
        case ITEM_ATTRIB_RECRITICALHIT:    // ����ɱ
        case ITEM_ATTRIB_RATE_EXP:         //������
        case ITEM_ATTRIB_RATE_GOLD:        //��Ǯ���
        case ITEM_ATTRIB_DAMAGE_ABSORB:    //�˺�����
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num);
            str += "%";
            break;
        case ITEM_ATTRIB_RATE_ABSORBMP:   // ��������ħ
        case ITEM_ATTRIB_RATE_MAXHP:      // ���HP�ٷֱ�
        case ITEM_ATTRIB_RATE_MAXMP:      // ���MP�ٷֱ�
        case ITEM_ATTRIB_RATE_MOVESPEED:  //�ƶ��ٶ�
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num >= 1 ? Num : 1);
            str += "%";
            break;
        default:
            if (bItemRelated)
                Num = rtRound(fRate * attrib.count);
            else
                Num = rtRound(fRate * attrib.count);
            str += rtFormatNumber(Num);
            break;
    }

    return str;
    unguard;
}

ETradableResult CItemContainer::CanItemTrade(char bCommonBag, int page, int i, int j) {
    SItemID item;
    if (bCommonBag == 1) {
        item = m_Bag.GetItem(page, i, j);
    } else if (bCommonBag == 2) {
        item = m_PetBag.GetItem(page, i, j);
    } else {
        item = m_MissionBag.GetItem(page, i, j);
    }

    if (!ItemID_IsValid(item))
        return TR_CanNot_Unkown;

    if (!ItemCanTrade(item))
        return TR_CanNot_Essence;

    if (ItemIsPet(item.type)) {
        //if (m_pMaster->GetRidePetID()!=0)
        //	return TR_CanNot_Pet;
    }

    return TR_Can;
}

ETradableResult CItemContainer::CanItemTrade(DWORD itemID) {
    int page, i, j;
    if (m_Bag.Find(itemID, &page, &i, &j)) {
        return CanItemTrade(1, page, i, j);
    } else if (m_MissionBag.Find(itemID, &page, &i, &j)) {
        return CanItemTrade(0, page, i, j);
    } else if (m_PetBag.Find(itemID, &page, &i, &j)) {
        return CanItemTrade(2, page, i, j);
    } else {
        return TR_CanNot_Unkown;
    }
}

static void _ItemHint_Requirement(RtString& str, int& outLineCount, SItemID& itemID,
                                  SItemBase* pItemClass, GcActor* pPlayer,
                                  const RtString& YesColorString, const RtString& NoColorString) {
    char     tmp[256];
    RtString strColorTmp;

    CHECK(ItemID_IsValid(itemID));
    CHECK(pItemClass);

    // ��������
    int iReqStr = 0, iReqDex = 0, iReqInt = 0, iReqHit = 0, iReqCon = 0, iReqLevel = 0,
        iMaxLevel = 0;
    GetItemAttribRequirement(GetWorld()->m_pItemManager, itemID, iReqStr, iReqDex, iReqInt, iReqHit,
                             iReqCon, iReqLevel, iMaxLevel);

    // �;ö�
    DWORD curWear = itemID.curWear;
    if (ItemIsArmor(itemID) || ItemIsWeapon(itemID) || ItemIsShipin(itemID)) {
        RtString strWear;
        strWear.Format("%d/%d", curWear / 100, pItemClass->maxWear / 100);
        if (curWear > 0) {
            rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %s</FONT></P>", YesColorString,
                         "�;�", strWear);
        } else {
            rt2_snprintf(tmp, 256, "<P><FONT color=\"#ff0000\">%s: %s</FONT></P>", "�;�", strWear);
        }

        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }

    if (iReqLevel != 0) {
        if (pPlayer->m_core.Lev >= iReqLevel)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d</FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_LEV), iReqLevel);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }

    if (iReqStr != 0) {
        if (pPlayer->m_core.Str.GetValue() >= iReqStr)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d </FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_STR), iReqStr);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    if (iReqInt != 0) {
        if (pPlayer->m_core.Int.GetValue() >= iReqInt)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d </FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_INT), iReqInt);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    if (iReqDex != 0) {
        if (pPlayer->m_core.Dex.GetValue() >= iReqDex)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d </FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_DEX), iReqDex);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    if (iReqHit != 0) {
        if (pPlayer->m_core.Hit.GetValue() >= iReqHit)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d </FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_HIT), iReqHit);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }

    // ��������
    if (iReqCon != 0) {
        if (pPlayer->m_core.Con.GetValue() >= iReqCon)
            strColorTmp = YesColorString;
        else
            strColorTmp = NoColorString;
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s: %d</FONT></P>", strColorTmp,
                     R(G_CRE_REQUIRE_CON), iReqCon);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }

    // ְҵ����
    BYTE cTmp   = pItemClass->requirement[ItemRequirement_Career];
    strColorTmp = "#44dd88";
    if (cTmp == 0x0F) {
        // ����ְҵͨ��
        if (ItemIsWeapon(itemID) || ItemIsArmor(itemID) || ItemIsShipin(itemID)) {
            rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s</FONT></P>", strColorTmp,
                         R(MSG_ITEM_REQUIRE_LEV_ALL));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
    } else {
        int MetierLevReq = pItemClass->requirement[ItemRequirement_Grade] - 1;
        if (MetierLevReq < 0)
            MetierLevReq = 0;
        RtString strTmp;
        if ((cTmp & 0x01) != 0x0)  //"սʿ��";
        {
            SUserActor* pActor = g_TableUserActor.FindUserActor(METIER_WARRIOR, MetierLevReq);
            if (pActor) {
                strTmp += pActor->MetierName.c_str();
                strTmp += R(G_AND);
            }
        }
        if ((cTmp & 0x02) != 0x0)  //"Ů���ֺ�";
        {
            SUserActor* pActor = g_TableUserActor.FindUserActor(METIER_HUNTER, MetierLevReq);
            if (pActor) {
                strTmp += pActor->MetierName.c_str();
                strTmp += R(G_AND);
            }
        }
        if ((cTmp & 0x04) != 0x0)  //"��ʿ��";
        {
            SUserActor* pActor = g_TableUserActor.FindUserActor(METIER_WIZARD, MetierLevReq);
            if (pActor) {
                strTmp += pActor->MetierName.c_str();
                strTmp += R(G_AND);
            }
        }
        if ((cTmp & 0x08) != 0x0)  //"��ʿ��";
        {
            SUserActor* pActor = g_TableUserActor.FindUserActor(METIER_TAOIST, MetierLevReq);
            if (pActor) {
                strTmp += pActor->MetierName.c_str();
                strTmp += R(G_AND);
            }
        }
        strTmp.TrimRight(R(G_AND));

        // �ж�ְҵ
        if (pPlayer->m_core.MetierLev < pItemClass->requirement[ItemRequirement_Grade] - 1 ||
            (pPlayer->m_core.MetierLev > pItemClass->requirement[ItemRequirement_Grade] - 1 &&
             pItemClass->requirement[ItemRequirement_Grade] - 1 == 0)) {
            strColorTmp = NoColorString;
        } else {
            if (pPlayer->m_core.Metier == METIER_WARRIOR) {
                if ((cTmp & 0x01) == 0)
                    strColorTmp = NoColorString;
                else
                    strColorTmp = "#44dd88";
            } else if (pPlayer->m_core.Metier == METIER_HUNTER) {
                if ((cTmp & 0x02) == 0)
                    strColorTmp = NoColorString;
                else
                    strColorTmp = "#44dd88";
            } else if (pPlayer->m_core.Metier == METIER_WIZARD) {
                if ((cTmp & 0x04) == 0)
                    strColorTmp = NoColorString;
                else
                    strColorTmp = "#44dd88";
            } else if (pPlayer->m_core.Metier == METIER_TAOIST) {
                if ((cTmp & 0x08) == 0)
                    strColorTmp = NoColorString;
                else
                    strColorTmp = "#44dd88";
            }
        }
        rt2_snprintf(tmp, 256, "<P><FONT color=\"%s\">%s%s</FONT></P>", strColorTmp, strTmp,
                     R(MSG_ITEM_SPECIAL));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
}

static void _ItemHint_ColorAttribs(RtString& str, int& outLineCount, SItemID& itemID,
                                   SItemBase* pItemClass, const RtString& strColor) {
    if (ItemIsGreen(itemID)) {
        vector<SAttribAddtion> arrAttrib;
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.attribID1, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i],
                                                            itemID.attribRate1);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
    } else if (ItemIsBlue(itemID)) {
        vector<SAttribAddtion> arrAttrib;
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.attribID1, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i],
                                                            itemID.attribRate1);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.attribID2, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i],
                                                            itemID.attribRate2);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
    } else if (ItemIsGolden(itemID)) {
        vector<SAttribAddtion> arrAttrib;
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.attribID1, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i],
                                                            itemID.attribRate1);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.attribID2, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i],
                                                            itemID.attribRate2);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
    }
}

static void _ItemHint_EnchantAttribs(RtString& str, int& outLineCount, SItemID& itemID,
                                     SItemBase* pItemClass, const RtString& strColor) {
    if (itemID.enchantAttrib != 0) {
        vector<SAttribAddtion> arrAttrib;
        if (GetWorld()->m_pItemManager->m_pAttribMgr->GetAttrib(itemID.enchantAttrib, arrAttrib)) {
            for (int i = 0; i < (int)arrAttrib.size(); i++) {
                str += "<P><FONT color=\"";
                str += strColor;
                str += "\">";
                str += CItemContainer::GenerateAttribString(pItemClass, arrAttrib[i], 100, false);
                str += "</FONT></P>";
                outLineCount++;
            }
        }
    }
}

static void _ItemHint_Desc(RtString& str, int& outLineCount, SItemID& itemID,
                           SItemBase* pItemClass) {
    char tmp[1024];
    if (pItemClass->pDesc) {
        rt2_snprintf(tmp, 1024, "%s", pItemClass->pDesc);
        //tmp[255] = 0;
        str += tmp;
        outLineCount += CItemContainer::CalcHintLineCount(pItemClass->pDesc);
    }
}

static char* _GetTrumpStageFormLev(int lev) {
    int iStage = lev / 20 + 1;
    switch (iStage) {
        case 1:
            return "����";
        case 2:
            return "����";
        case 3:
            return "��";
        case 4:
            return "ԪӤ";
        case 5:
            return "����";
        case 6:
            return "����";
        case 7:
            return "���";
        case 8:
            return "����";
    }
    return "";
}

RtString GenerateItemHtmlHint(int& outLineCount, SItemID& itemID, GcActor* pPlayer,
                              CBag* pBag /* = NULL */, int page /* = -1*/, int i /* = -1*/,
                              int j /* = -1*/, float fPriceRate /* = -1.0f*/, UINT price /* = 0*/,
                              bool bCountAll /* = false*/, float fTaxRate /* = -1.0f*/,
                              int tTimeLimit /*=0*/, int OtherPrice) {
    guard;

    SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
    if (!pPlayer || !pItemBase)
        return "";

    outLineCount = 0;
    char           tmp[1024];
    RtString       str;
    RtString       strColor;
    RtString       strColorTmp;
    const RtString WHITE_STRING = "#ffffff";

    //RtString b111[10] = { "#00dFeF", "#00bFcF", "#009FaF", "#008FaF", "#40F512", "#30e012", "#20c610", "#30b012" };
    //static int ccc = 0;
    //RtString BLUE_STRING = b111[ccc];
    //PlayerFailOperate(rtFormatNumber(ccc));
    //ccc++;
    //if (ccc==8)
    //	ccc = 0;

    const RtString ENCHANT_STRING = "#944cff";  //"#604090";
    const RtString GREEN_STRING   = COLOR_POISON.c_str();
    const RtString BLUE_STRING    = COLOR_WATOR.c_str();
    const RtString GOLDEN_STRING  = "#FFD851";  //"#d0c002";
    const RtString SUIT_STRING    = "#ee00ee";  //"#40F512";
    const RtString YES_STRING     = "#ffffff";
    const RtString NO_STRING      = "#ff0000";
    const RtString PURPUP_STRING  = "#ff00ff";
    if (ItemIsGreen(itemID))
        strColor = GREEN_STRING;
    else if (ItemIsBlue(itemID))
        strColor = BLUE_STRING;
    else if (ItemIsGolden(itemID))
        strColor = GOLDEN_STRING;
    else
        strColor = WHITE_STRING;

    // ���ߴ��������ӵ����Ժ�����
    int   itemLevel   = itemID.level;
    int   metier      = GetWorld()->m_pPlayer->m_core.Metier;
    float fLev_Damage = 0.0f, fLev_Attack = 0.0f, fLev_Dodge = 0.0f, fLev_Armor = 0.0f,
          fLev_ElementArmor = 0.0f;
    float fLev_Str = 0.0f, fLev_Hit = 0.0f, fLev_Int = 0.0f, fLev_Dex = 0.0f;
    ItemUpdate_GetExtraAddtion(itemID, GetItemMgr(), fLev_Damage, fLev_Attack, fLev_Dodge,
                               fLev_Armor, fLev_ElementArmor);
    ItemUpdate_GetExtraRequirement(itemID, GetItemMgr(), fLev_Str, fLev_Hit, fLev_Int, fLev_Dex);

    str += "<html><body>";
    // ��������
    str += "<P align=\"center\"><FONT color=\"";
    if (ItemIsArmor(itemID) && ((SArmor*)pItemBase)->suitID > 0)
        str += SUIT_STRING;
    else
        str += strColor;
    str += "\">";
    str += pItemBase->name;
    if (ItemCanPile(itemID))
    //if ( ItemCanPile(itemID) && pBag && i!=-1 && j!=-1)
    {
        int count;
        if (bCountAll) {
            if (pBag) {
                int itemIndex = GetWorld()->m_pItemManager->GetIndexFromType(itemID.type);
                pBag->FindType(itemIndex, ItemColor_All, &count);
            } else {
                ERR("GenerateItemHtmlHint: pBag==NULL!\n");
                count = 1;
            }
        } else {
            count = itemID.count;
        }
        rt2_sprintf(tmp, "(%d)", count);
        str += tmp;
    }
    if ((itemID.level > 0) && !ItemIsTrump(itemID.type)) {
        rt2_sprintf(tmp, "    +%d", itemID.level);
        str += tmp;
    }
    str += "</FONT></P>";
    outLineCount++;
    // ��
    if (itemID.cBind != IB_NotBind) {
        string bindColor;
        string bindString;
        if (itemID.cBind == IB_Binded) {
            bindColor  = "afafaf";
            bindString = R(MSG_ITEMBINDED);
        } else if (itemID.cBind == IB_BindWhenEquip) {
            bindColor  = "b0903a";
            bindString = R(MSG_ITEMBIND_WHENEQUIP);
        } else if (itemID.cBind == IB_BindWhenPick) {
            bindColor  = "b0903a";
            bindString = R(MSG_ITEMBIND_WHENPICK);
        } else {
            bindColor  = "afafaf";
            bindString = R(G_ERROR);
        }
        rt2_snprintf(tmp, 1024, "<P align=\"right\"><FONT color=\"#%s\">%s  </FONT></P>",
                     bindColor.c_str(), bindString.c_str());
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    // ʹ������
    DWORD now   = time(NULL);
    DWORD tLife = itemID.TimeLimit;
    if (tLife != 0) {
        RtString strLife;
        //struct tm* lTime;
        RtLocateTime lTime(
            tLife);  //tim.yang  �˴�����localtimeתʱ�亯��ʱ����ֿ�ָ�룬������RtLocateTime
        //lTime = localtime((time_t*)(&tLife));
        strLife.Format("%4d��%d��%d�� %02d:%02d", lTime.iYear, lTime.iMonth, lTime.iDay,
                       lTime.iHour, lTime.iMin);
        if (now >= tLife)
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", NO_STRING,
                         "ʹ������", strLife);
        else
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", "#108512",
                         "ʹ������", strLife);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }

    // �Ƿ���Խ���
    if (!ItemCanTrade(itemID.type)) {
        rt2_snprintf(tmp, 1024, "<P align=\"left\"><FONT color=\"#%s\">%s  </FONT></P>", "afafaf",
                     R(MSG_ITEM_CANNOT_TRADE));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    // �ֿ���������
    if (itemID.cLock != 0) {
        rt2_snprintf(tmp, 1024, "<P align=\"left\"><FONT color=\"#%s\">%s  </FONT></P>", "afafaf",
                     R(MSG_ITEM_PROTEST_ON_HINT));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    // �۸�
    if (OtherPrice) {
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: %d</FONT></P>", R(MSG_PRICE),
                     OtherPrice);
        str += tmp;
        outLineCount++;
    } else {
        if (fPriceRate > 0.0f) {
            int   price1;
            int   priceBase = GetItemMgr()->GetItemValue(itemID);
            float pointBase = GetItemMgr()->GetItemPoint(itemID);
            if (pointBase > 0.0f) {
                int point = rtRound(pointBase * fPriceRate * itemID.count);
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: %d</FONT></P>", "����",
                             point);
                //tmp[255] = 0;
            } else {
                price1 = rtRound(priceBase * fPriceRate * itemID.count);

                if (fTaxRate > 0.0f) {
                    int priceNormal = rtRound(priceBase * itemshop_buy_pricerate * itemID.count);
                    int priceTax    = price1 - priceNormal;

                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: %d (%s: %d)</FONT></P>",
                                 R(MSG_PRICE), price1, R(G_TAX), priceTax);
                    //tmp[255] = 0;
                } else {
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: %d</FONT></P>",
                                 R(MSG_PRICE), price1);
                    //tmp[255] = 0;
                }
            }

            str += tmp;
            outLineCount++;
        } else if (price > 0) {
            if ((int)price == -99) {
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s</FONT></P>",
                             R(MSG_TRADE_NOTATSELLNOW));
            } else {
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: %d</FONT></P>",
                             R(MSG_PRICE), price);
            }
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
    }

    // ʣ��ʱ�� ��������
    if (tTimeLimit) {
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#dd3399\">%s: ", R(MSG_TIMELIMIT));
        tTimeLimit /= 60;
        if (tTimeLimit > (60 * 60 * 24)) {
            rt2_snprintf(tmp + strlen(tmp), 1024 - strlen(tmp), "%d�� ", tTimeLimit / (60 * 24));
        }
        tTimeLimit %= (60 * 24);
        rt2_snprintf(tmp + strlen(tmp), 1024 - strlen(tmp), "%d:%d", tTimeLimit / 60,
                     tTimeLimit % 60);
        rt2_strcat(tmp, "</FONT></P>");
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    }
    // ����
    if (ItemIsWeapon(itemID)) {
        SWeapon* pWeapon = (SWeapon*)pItemBase;
        // ��������
        if (ItemIsShield(itemID)) {
            // ����
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_ARMOR),
                         pWeapon->nArmor + rtRound(fLev_Armor));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
            // ����
            if (pWeapon->nDodge + rtRound(fLev_Dodge) > 0) {
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>",
                             R(G_CRE_DODGE), pWeapon->nDodge + rtRound(fLev_Dodge));
                //tmp[255] = 0;
                str += tmp;
                outLineCount++;
            }
        } else {
            // �˺�
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d-%d</FONT></P>",
                         R(G_CRE_DAMAGE), pWeapon->nDamageMin + rtRound(fLev_Damage),
                         pWeapon->nDamageMax + rtRound(fLev_Damage));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
            // ����
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_ATTACK),
                         pWeapon->nAttack + rtRound(fLev_Attack));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // �����ٶ� ��������
        if (!ItemIsShield(itemID.type) && !ItemIsTrump(itemID.type)) {
            // �����ٶ�
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(AttSpeed),
                         (int)(pWeapon->fAttSpeed * ATT_SPEED_SCALE));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
            // ��������
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(AttRange),
                         pWeapon->nAttRange);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // ��˫������
        if (!ItemIsShield(itemID.type) && !ItemIsTrump(itemID.type)) {
            if (pWeapon->bBothHands) {
                str += "<P><FONT color=\"#ffffff\">";
                str += R(MSG_ITEM_WEAPON_BOTHHANDS);
                str += "</FONT></P>";
            } else {
                str += "<P><FONT color=\"#ffffff\">";
                str += R(MSG_ITEM_WEAPON_SINGLEHAND);
                str += "</FONT></P>";
            }
            outLineCount++;
        }
        // ��������
        if (!ItemIsTrump(itemID))
            _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING,
                                  NO_STRING);

        // ����װ���ӳ�
        if (!ItemIsTrump(itemID))
            _ItemHint_ColorAttribs(str, outLineCount, itemID, pItemBase, strColor);

        // ��ħ����
        if (!ItemIsTrump(itemID))
            _ItemHint_EnchantAttribs(str, outLineCount, itemID, pItemBase, ENCHANT_STRING);
        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsTrump(itemID)) {
        STrump* pTrump = (STrump*)pItemBase;
        int     lev    = itemID.params[TRUMP_LEV];
        int     VigLev = (lev / 20) * 20;

        const std::string COLOR_FIRE   = "#dd5511";
        const std::string COLOR_WATOR  = "#00aaff";
        const std::string COLOR_POISON = "#30e512";

        // �˺�
        if (pTrump->nDamageMax + rtRound(fLev_Damage) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d-%d</FONT></P>",
                         R(G_CRE_DAMAGE), pTrump->nDamageMin + rtRound(fLev_Damage),
                         pTrump->nDamageMax + rtRound(fLev_Damage));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }

        // ����
        if (pTrump->nAttack + rtRound(fLev_Attack) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_ATTACK),
                         pTrump->nAttack + rtRound(fLev_Attack));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }

        // �����ȼ�
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d </FONT></P>", "�����ȼ�",
                     itemID.params[TRUMP_LEV]);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        // ��Ϊ����
        if (pPlayer->m_core.Cul >= VigLev)
            strColorTmp = YES_STRING;
        else
            strColorTmp = NO_STRING;
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s </FONT></P>", strColorTmp, "��Ϊ����",
                     _GetTrumpStageFormLev(itemID.params[TRUMP_LEV]));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        // ����ְҵͨ��
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#44dd88\">%s</FONT></P>", "����ְҵͨ��");
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        _ItemHint_ColorAttribs(str, outLineCount, itemID, pItemBase, strColor);

        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s</FONT></P>", "");
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        // �ɳ�����
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s</FONT></P>", "�ɳ�����");
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        // ������
        int   iTemp = (float)lev / 20.f;
        float fInfo = 0.05f * (float)iTemp;
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: +%d </FONT></P>", GOLDEN_STRING,
                     "������", (int)(fInfo * 100.f));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        // ������
        iTemp = (float)lev / 20.f;
        fInfo = 0.02f * (float)iTemp;
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: +%d </FONT></P>", GOLDEN_STRING,
                     "������", (int)(fInfo * 100.f));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;

        if (itemID.params[TRUMP_ATTACH_TYPE] != -1) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s</FONT></P>", "");
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;

            // ע��Ԫ��
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s</FONT></P>", "ע��Ԫ��");
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;

            char attachType[6][32]   = {"����",       "�����Է���", "ˮ��",
                                        "ˮ���Է���", "����",       "�����Է���"};
            char attachCorlor[6][32] = {"#dd5511", "#dd5511", "#00aaff",
                                        "#00aaff", "#30e512", "#30e512"};
            // ����Ԫ������
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: +%d</FONT></P>",
                         attachCorlor[itemID.params[TRUMP_ATTACH_TYPE]],
                         attachType[itemID.params[TRUMP_ATTACH_TYPE]],
                         itemID.params[TRUMP_ATTACH_POINT]);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }

        // ����
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s</FONT></P>", "");
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
        for (int i = TRUMP_SKILL_1; i < TRUMP_SKILL_MAX; i++) {
            if (itemID.params[i] != 0) {
                SSkill* pSkill = Skill()->FindSkill(itemID.params[i]);
                if (pSkill) {
                    rt2_snprintf(tmp, 1024,
                                 "<P><FONT color=\"#ff00ff\">��������: %s��%d����</FONT></P>",
                                 pSkill->szName, pSkill->iLevel);
                    //tmp[255] = 0;
                    str += tmp;
                    outLineCount++;
                }
            }
        }
        outLineCount++;
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsSorptionPet(itemID)) {
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %.2f</FONT></P>", "Ʒ��",
                     (float)itemID.params[0] / PET_PARAM_COMPUTE_BEADS);
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
    } else if (ItemIsArmor(itemID)) {
        SArmor* pArmor = (SArmor*)pItemBase;
        // ����
        rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_ARMOR),
                     pArmor->nArmor + rtRound(fLev_Armor));
        //tmp[255] = 0;
        str += tmp;
        outLineCount++;
        // ����
        if (pArmor->nDodge + rtRound(fLev_Dodge) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_DODGE),
                         pArmor->nDodge + rtRound(fLev_Dodge));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // ����
        if (pArmor->nAttack + rtRound(fLev_Attack) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_ATTACK),
                         pArmor->nAttack + rtRound(fLev_Attack));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // �����ٶȣ����ף�
        if (pArmor->fAttSpeed != 0.f) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(AttSpeed),
                         (int)(pArmor->fAttSpeed * ATT_SPEED_SCALE));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // �ƶ��ٶȣ�Ь�ӣ�
        if (pArmor->nMoveSpeed != 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(MoveSpeed),
                         pArmor->nMoveSpeed);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // ��������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);
        // ����װ���ӳ�
        _ItemHint_ColorAttribs(str, outLineCount, itemID, pItemBase, strColor);
        // ��ħ����
        _ItemHint_EnchantAttribs(str, outLineCount, itemID, pItemBase, ENCHANT_STRING);
        // ��װ
        if (pArmor->suitID != 0) {
            // ��װ�ӳ�
            if (pPlayer && pPlayer->mItem.m_pSuit && !pBag) {
                SSuit*      pSuit      = pPlayer->mItem.m_pSuit;
                CAttribMgr* pAttribMgr = GetWorld()->m_pItemManager->m_pAttribMgr;
                CHECK(pAttribMgr);
                vector<SAttribAddtion> arrAttribs;
                if (pAttribMgr->GetAttrib(pSuit->attribID, arrAttribs)) {
                    int iTmp = 0;

                    //if (iTmp < pSuit->arrTypes.size() )
                    {
                        //strcpy(tmp, "<P>&nbsp;</P>");
                        //str += tmp;
                        //outLineCount ++;
                        for (int i = 0; i < (int)arrAttribs.size(); i++) {
                            //							str += "<P><FONT color=\"#00ff00\">";
                            str += "<P><FONT color=\"";
                            str += SUIT_STRING;
                            str += "\">";
                            str += CItemContainer::GenerateAttribString(pItemBase, arrAttribs[i],
                                                                        SUIT_DEFAULT_ATTRIB_RATE);
                            str += "</FONT></P>";
                            outLineCount++;
                        }
                    }
                }
            } else  // ��װ˵��
            {
                CSuitItemManager* pSuitMgr = GetWorld()->m_pItemManager->m_pSuitManager;
                SSuit*            pSuit    = pSuitMgr->GetSuit(pArmor->suitID);
                if (pSuit) {
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s</FONT></P>", SUIT_STRING,
                                 pSuit->desc.c_str());
                    //tmp[255] = 0;
                    str += tmp;
                    outLineCount += CItemContainer::CalcHintLineCount(pSuit->desc.c_str());
                    ;
                }
            }
        }
        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsShipin(itemID)) {
        SShiPin* pShipin = (SShiPin*)pItemBase;
        // ��������
        if (pShipin->nFireDefence + rtRound(fLev_ElementArmor) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>",
                         R(G_CRE_FIRE_ARMOR), pShipin->nFireDefence + rtRound(fLev_ElementArmor));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        if (pShipin->nWaterDefence + rtRound(fLev_ElementArmor) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>",
                         R(G_CRE_WATOR_ARMOR), pShipin->nWaterDefence + rtRound(fLev_ElementArmor));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        if (pShipin->nPoisonDefence + rtRound(fLev_ElementArmor) > 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>",
                         R(G_CRE_POISON_ARMOR),
                         pShipin->nPoisonDefence + rtRound(fLev_ElementArmor));
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        if (pShipin->nMaxHP != 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_MAX_HP),
                         pShipin->nMaxHP);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        if (pShipin->nMaxMP != 0) {
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %d</FONT></P>", R(G_CRE_MAX_MP),
                         pShipin->nMaxMP);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // ��������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);
        // ����װ���ӳ�
        _ItemHint_ColorAttribs(str, outLineCount, itemID, pItemBase, strColor);
        // ��ħ����
        _ItemHint_EnchantAttribs(str, outLineCount, itemID, pItemBase, ENCHANT_STRING);
        // ʹ������
        DWORD now   = time(NULL);
        DWORD tLife = itemID.params[ITEMSHIPIN_PARAM_LIFE];
        if (tLife != 0) {
            RtString   strLife;
            struct tm* lTime;
            lTime = localtime((time_t*)(&tLife));
            strLife.Format("%4d��%d��%d�� %02d:%02d", lTime->tm_year + 1900, lTime->tm_mon + 1,
                           lTime->tm_mday, lTime->tm_hour, lTime->tm_min);
            if (now >= tLife)
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", NO_STRING,
                             "ʹ������", strLife);
            else
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", YES_STRING,
                             "ʹ������", strLife);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }
        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsYaoPing(itemID)) {
        // ʹ������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);

        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsScroll(itemID)) {
        if (ItemIsScrollCompose(itemID.type)) {
            SItemCompose* pComposeClass =
                GetWorld()->m_pItemManager->m_pComposeMgr->FindComposing(pItemBase->composeID);
            if (!pComposeClass)
                return "";
            SItemBase* pProductClass =
                GetWorld()->m_pItemManager->GetItemFromIndex(pComposeClass->Product);
            if (!pProductClass)
                return "";
            // ������Ʒ
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %s</FONT></P>", R(������Ʒ),
                         pProductClass->name);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
            // ����
            SItemBase* pMatItemClass =
                GetWorld()->m_pItemManager->GetItemFromIndex(pComposeClass->Materials[0]);
            if (!pMatItemClass)
                return "";
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %s(%d)</FONT></P>", R(����),
                         pMatItemClass->name, pComposeClass->MaterialNums[0]);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
            for (int i = 1; i < pComposeClass->MaterialTypeCount; i++) {
                pMatItemClass =
                    GetWorld()->m_pItemManager->GetItemFromIndex(pComposeClass->Materials[i]);
                if (!pMatItemClass)
                    return "";
                rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s(%d)</FONT></P>",
                             pMatItemClass->name, pComposeClass->MaterialNums[i]);
                //tmp[255] = 0;
                str += tmp;
                outLineCount++;
            }
            // ѧϰ����
            WORD CheckSkillSubID = 0;
            WORD SkillID         = 0;
            switch (pComposeClass->ComposeType) {
                case ICT_Equipment:
                    CheckSkillSubID = ITEMCOMPOSE_WEAPON_SKILL;
                    SkillID         = ITEMCOMPOSE_WEAPON_LEVEL1_SKILL;
                    break;
                case ICT_Casting:
                    CheckSkillSubID = ITEMCOMPOSE_EQUIP_SKILL;
                    SkillID         = ITEMCOMPOSE_EQUIP_LEVEL1_SKILL;
                    break;
                case ICT_Yaoping:
                    CheckSkillSubID = ITEMCOMPOSE_YAOPING_SKILL;
                    SkillID         = ITEMCOMPOSE_YAOPING_LEVEL1_SKILL;
                    break;
                case ICT_EnchantScroll:
                    CheckSkillSubID = ITEMCOMPOSE_ENCHANT_SKILL;
                    SkillID         = ITEMCOMPOSE_ENCHANT_LEVEL1_SKILL;
                    break;
                default:
                    break;
            }
            std::string SkillName;
            SSkill*     pSkill = GetWorld()->m_pPlayer->m_Skill.FindSkillBySubID(CheckSkillSubID);
            if (!pSkill || (pSkill->iLevel < pComposeClass->SkillLevel)) {
                pSkill = Skill()->FindSkill(SkillID);
                if (!pSkill)
                    return "";
                SkillName   = pSkill->szName;
                strColorTmp = NO_STRING;
            } else {
                SkillName   = pSkill->szName;
                strColorTmp = YES_STRING;
            }

            rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s(%s%d)</FONT></P>", strColorTmp,
                         R(ѧϰ����), SkillName.c_str(), R(�ȼ�), pComposeClass->SkillLevel);
            //tmp[255] = 0;
            str += tmp;
            outLineCount++;
        }

        // ʹ������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);

        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsGem(itemID)) {
        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsOther(itemID)) {
        // ʹ������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);

        if (ItemIsOther_Dungeon(itemID.type)) {
            string strDifficulty;
            switch (itemID.params[0]) {
                case 0:
                    strDifficulty = R(DUNGEON_NORMAL);
                    break;
                case 1:
                    strDifficulty = R(DUNGEON_DIFFICULT);
                    break;
                case 2:
                    strDifficulty = R(DUNGEON_HELL);
                    break;
                case 3:
                    strDifficulty = R(DUNGEON_LEGEND);
                    break;
                case 4:
                    strDifficulty = R(DUNGEON_MYTH);
                    break;
            }
            rt2_snprintf(tmp, 1024, "<P><FONT color=\"#ffffff\">%s: %s</FONT></P>",
                         R(DUNGEON_DIFFICULTY), strDifficulty.c_str());
            //tmp[255] = 0;
            str += tmp;
            outLineCount += CItemContainer::CalcHintLineCount(strDifficulty.c_str());
        } else if (ItemIsOther_Spawn(itemID.type)) {
            DWORD now   = time(NULL);
            DWORD tLife = itemID.params[ITEMSPAWN_PARAM_LIFE];
            if (tLife != 0) {
                RtString   strLife;
                struct tm* lTime;
                lTime = localtime((time_t*)(&tLife));
                strLife.Format("%4d��%d��%d�� %02d:%02d", lTime->tm_year + 1900, lTime->tm_mon + 1,
                               lTime->tm_mday, lTime->tm_hour, lTime->tm_min);
                if (now >= tLife)
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", NO_STRING,
                                 "ʹ������", strLife);
                else
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", YES_STRING,
                                 "ʹ������", strLife);
                //tmp[255] = 0;
                str += tmp;
                outLineCount++;
            }
        } else if (ItemIsOther_Hero(itemID.type)) {
            DWORD now   = time(NULL);
            DWORD tLife = itemID.params[ITEMHERO_PARAM_LIFE];
            if (tLife != 0) {
                RtString   strLife;
                struct tm* lTime;
                lTime = localtime((time_t*)(&tLife));
                strLife.Format("%4d��%d��%d�� %02d:%02d", lTime->tm_year + 1900, lTime->tm_mon + 1,
                               lTime->tm_mday, lTime->tm_hour, lTime->tm_min);
                if (now >= tLife)
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", NO_STRING,
                                 "ʹ������", strLife);
                else
                    rt2_snprintf(tmp, 1024, "<P><FONT color=\"%s\">%s: %s</FONT></P>", YES_STRING,
                                 "ʹ������", strLife);
                //tmp[255] = 0;
                str += tmp;
                outLineCount++;
            }
        }

        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else if (ItemIsPet(itemID)) {
    } else if (ItemIsSkill(itemID.type)) {
        // ʹ������
        _ItemHint_Requirement(str, outLineCount, itemID, pItemBase, pPlayer, YES_STRING, NO_STRING);

        // ˵��
        _ItemHint_Desc(str, outLineCount, itemID, pItemBase);
    } else {
    }

    str += "</body></html>";

    return str;

    unguard;
}

CItemManager* GetItemMgr() {
    guard;
    return ((CGameClientFrame*)GetApp())->m_pItemManager;
    unguard;
}

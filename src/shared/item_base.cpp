#include "core/rt2_core.h"
#include "rt1tort2.h"

#ifdef GMSERVER
#include "../../../gm_tool/gm_server/gmserver.h"
#endif

#include "string_mapping.h"
#include "region_client.h"

extern DWORD GetNextItemID();

//char g_strItemBuffer10240[10240];
//char g_strItemBuffer20480[20480];
//char g_strItemBuffer256[256];
//char g_strItemBufferBD[256];
G_MEMDEF(g_strItemBuffer4096, 4096)
G_MEMDEF(g_strItemBuffer10240, 10240)
G_MEMDEF(g_strItemBuffer20480, 20480)
G_MEMDEF(g_strItemBuffer256, 256)
G_MEMDEF(g_strItemBufferBD, 256)

const short g_iItemBagVersion          = 11;
const short g_iItemContainerVersion    = 11;
const short g_iCreateItemStringVersion = 1;
const short g_iPetContainerVersion     = 1;

int GetItemSerializeVersion() {
    return g_iItemBagVersion;
}

bool MoneyIsValid(long money) {
    if (money >= 0 && money <= MAX_MONEY_COUNT)
        return true;
    else
        return false;
}

float GetDefaultAttackSpeed(char Metier) {
    guard;

    switch (Metier) {
        case METIER_WARRIOR:
            return 1.2;
        case METIER_HUNTER:
        case METIER_WIZARD:
        case METIER_TAOIST:
            return 0.8;
        default:
            ERR("[GetDefaultAttackSpeed]: Invalid Metier\n");
            return 0.0f;
    }

    return 0.0f;

    unguard;
}

int GetDefaultAttackRange() {
    return 40;
}

RtString GetAttribString(WORD attrib) {
    guard;
    static RtString str[ITEM_ATTRIB_COUNT] = {"",
                                              "Con",
                                              "Str",
                                              "Dex",
                                              "Int",
                                              "Hit",
                                              "G_CRE_ATTACK",
                                              "G_CRE_DODGE",
                                              "G_UNKNOWN",
                                              "G_CRE_DAMAGE",
                                              "G_CRE_ARMOR",
                                              "G_CRE_ATTACK_RATE",
                                              "G_CRE_DODGE_RATE",
                                              "G_CRE_DAMAGE_RATE",
                                              "G_CRE_ARMOR_RATE",
                                              "G_CRE_RATE_FIRE_ARMOR",
                                              "G_CRE_RATE_WATOR_ARMOR",
                                              "G_CRE_RATE_POISON_ARMOR",
                                              "G_CRE_RATE_FIRE_DAMAGE",
                                              "G_CRE_RATE_WATOR_DAMAGE",
                                              "G_CRE_RATE_POISON_DAMAGE",
                                              "G_CRE_REQUIRE_STR",
                                              "G_CRE_REQUIRE_DEX",
                                              "G_CRE_REQUIRE_INT",
                                              "G_CRE_REQUIRE_HIT",
                                              "AbsorbMp",
                                              "AttSpeed",
                                              "AbsorbHp",
                                              "AbsorbMp",
                                              "G_CRE_MAX_HP",
                                              "G_CRE_MAX_MP",
                                              "ConPower",
                                              "StrPower",
                                              "DexPower",
                                              "IntPower",
                                              "HitPower",
                                              "",
                                              "MoveSpeed",
                                              "CriticalHit",
                                              "G_CRE_DAMAGE_MIN",
                                              "G_CRE_DAMAGE_MAX",
                                              "G_CRE_DAMAGE_ABSORB",
                                              "G_CRE_LUCK",
                                              "Skill(unsupported yet)",
                                              "AttRange",
                                              "G_MAXHP_RATE",
                                              "G_MAXMP_RATE",
                                              "",
                                              "",
                                              "G_SKILL_CRITICAL_RATE",
                                              "G_SKILL_CRITICAL_DAM",
                                              "G_CRE_EXP_RATE",
                                              "G_CRE_GOLD_RATE",
                                              "G_CRE_REPARRY",
                                              "G_CRE_REFAINT",
                                              "G_CRE_RESKILLCRITICAL",
                                              "G_CRE_RECRITICALHIT",
                                              "",
                                              ""};

    if (attrib >= ITEM_ATTRIB_COUNT) {
        ERR1("[GetAttribString] attrib is INVALID! (attrib=%d)\n", attrib);
        attrib = 0;
    }
    return Rs(str[attrib]);
    unguard;
}

bool operator==(const SAttribAddtion& att1, const SAttribAddtion& att2) {
    guard;
    if (att1.attrib != att2.attrib)
        return false;
    if (att1.count != att2.count)
        return false;
    if (att1.reqStr != att2.reqStr)
        return false;
    if (att1.reqDex != att2.reqDex)
        return false;
    if (att1.reqInt != att2.reqInt)
        return false;
    if (att1.reqHit != att2.reqHit)
        return false;
    if (att1.value != att2.value)
        return false;

    return true;
    unguard;
}

bool operator!=(const SAttribAddtion& att1, const SAttribAddtion& att2) {
    guard;
    return !(att1 == att2);
    unguard;
}

bool operator==(const SItemID& item1, const SItemID& item2) {
    guard;
    if (item1.id != item2.id)
        return false;
    if (item1.type != item2.type)
        return false;
    if (item1.color != item2.color)
        return false;
    if (item1.level != item2.level)
        return false;
    if (item1.count != item2.count)
        return false;
    if (item1.cBind != item2.cBind)
        return false;
    if (item1.attribID1 != item2.attribID1)
        return false;
    if (item1.attribID2 != item2.attribID2)
        return false;
    if (item1.attribRate1 != item2.attribRate1)
        return false;
    if (item1.attribRate2 != item2.attribRate2)
        return false;
    if (item1.enchantAttrib != item2.enchantAttrib)
        return false;
    if (item1.cLock != item2.cLock)
        return false;
    if (item1.TimeLimit != item2.TimeLimit)
        return false;
    if (item1.paramCount != item2.paramCount)
        return false;
    for (int i = 0; i < item1.paramCount; i++)
        if (item1.params[i] != item2.params[i])
            return false;
    if (item1.curWear != item2.curWear)
        return false;
    if (item1.Valid != item2.Valid)
        return false;
    if (item1.name != item2.name)
        return false;

    return true;
    unguard;
}

bool operator!=(const SItemID& item1, const SItemID& item2) {
    guard;
    return !(item1 == item2);
    unguard;
}

//const SItemID& SItemID::operator=(const SItemID &item)
//{
//	guard;
//	id = item.id;
//	type = item.type;
//	color = item.color;
//	level = item.level;
//	cBind = item.cBind;
//	cLock = item.cLock;
//	count = item.count;
//	attribID1 = item.attribID1;
//	attribID2 = item.attribID2;
//	attribRate1 = item.attribRate1;
//	attribRate2 = item.attribRate2;
//	enchantAttrib = item.enchantAttrib;
//	TimeLimit = item.TimeLimit;
//	paramCount = item.paramCount;
//	for (int i=0; i<paramCount; i++)
//		params[i] = item.params[i];
//	cCountable = item.cCountable;
//	cEnchant = item.cEnchant;
//	_Reserved = item._Reserved;
//	cTimeLimit = item.cTimeLimit;
//	name = item.name;
//	curWear = item.curWear;
//	Valid = item.Valid;
//	return *this;
//	unguard;
//}

EItemType ItemID_GetItemType(DWORD type) {
    guard;

    DWORD     tmp1 = type & ITEM_MASK_TYPE;
    EItemType it;
    switch (tmp1) {
        case ITEM_MASK_WEAPON:
            it = IT_Weapon;
            break;
        case ITEM_MASK_ARMOR:
            it = IT_Armor;
            break;
        case ITEM_MASK_SHIPIN:
            it = IT_Shipin;
            break;
        case ITEM_MASK_YAOPING:
            it = IT_Yaoping;
            break;
        case ITEM_MASK_GEM:
            it = IT_Charm;
            break;
        case ITEM_MASK_SCROLL:
            it = IT_Scroll;
            break;
        case ITEM_MASK_OTHER:
            it = IT_Other;
            break;
        case ITEM_MASK_PET:
            it = IT_Pet;
            break;
        case ITEM_MASK_SKILL:
            it = IT_Skill;
            break;

        default:
            it = IT_Unknown;
            break;
    }

    return it;

    unguard;
}

/*目前已不使用*/
bool ItemID_HaveParams(DWORD type) {
    guard;

    if (ItemIsPet(type) || ItemIsWeapon_Sword(type) || ItemIsWeapon_Ball(type) ||
        ItemIsWeapon_Wand(type) || ItemIsWeapon_Wheel(type))
        return true;
    else
        return false;

    unguard;
}

bool SItemID::Serialize(CG_CmdPacket* cmd, char serializeType /* = NET_UPDATE_ALL*/) {
    guard;

    if (cmd->IsReading())
        Clear();

    switch (serializeType) {
        case NET_UPDATE_ITEM_SHOW:
            if (cmd->IsReading()) {
                id = 1;
                *cmd << type;
                *cmd << _wAtt;  //level;
            } else {
                *cmd << type;
                *cmd << _wAtt;  //level;
            }
            break;

        case NET_PLAYER_QUERY_INFO:
        case NET_UPDATE_ALL:
        default:
            *cmd << id;
            *cmd << type;
            //*cmd << color;
            //*cmd << level;
            //*cmd << cBind;
            //*cmd << cLock;
            *cmd << _wAtt;

            *cmd << attribID1;
            *cmd << attribID2;
            *cmd << attribRate1;
            *cmd << attribRate2;

            *cmd << enchantAttrib;
            *cmd << count;
            *cmd << paramCount;
            *cmd << TimeLimit;
            *cmd << curWear;
            *cmd << name;

            if (paramCount > ITEM_MAX_PARAM_COUNT) {
                RtCoreLog().Info("[SItemID::Serialize] Net Error! Param count is invalid!\n");
                return false;
            }

            for (int i = 0; i < paramCount; i++)
                *cmd << params[i];

            break;
    }

    if (cmd->IsError()) {
        RtCoreLog().Info("[SItemID::Serialize] Net Data Error!\n");
        return false;
    } else
        return true;

    unguard;
}

void SItemID::SaveToReadableString(CItemManager* pItemMgr, std::string& str) {
    guard;

    CHECK(pItemMgr);
    DWORD      itemID     = pItemMgr->GetIndexFromType(type);
    SItemBase* pItemClass = pItemMgr->GetItem(type);
    if (!pItemClass)
        return;

    rt2_sprintf(g_strItemBuffer10240,
                "id=%d;name=%s;itemID=%d;type=0x%x;color=%d;level=%d;count=%d;attr_blue=%d;attr_"
                "blueRate=%d;attr_golden=%d;attr_goldenRate=%d;attr_anchant=%d;cBind=%d;cLock=%d",
                (int)id, pItemClass->name, (int)itemID, (int)type, color, level, count,
                (short)attribID1, (short)attribRate1, (short)attribID2, (short)attribRate2,
                (short)enchantAttrib, cBind, cLock);

    char cTmp128[128];
    for (int i = 0; i < ITEM_MAX_PARAM_COUNT; i++) {
        rt2_sprintf(cTmp128, ";param%d=%d", i, params[i]);
        rt2_strcat(g_strItemBuffer10240, cTmp128);
    }

    str = g_strItemBuffer10240;

    unguard;
}

//<add by Tianh>  把整个SItemID的结构体解析成字符串
const char* SItemID::SaveToString(CItemManager* pItemMgr) {
    guard;

    char cTmp128[128];
    cTmp128[0] = '\0';

    // Version 11:
    int index = CItemManager::GetIndexFromType(type);

    // 设置cCountable, cEnchant, cTimeLimit
    if (count > 1)
        cCountable = 1;
    else
        cCountable = 0;
    if (enchantAttrib)
        cEnchant = 1;
    else
        cEnchant = 0;
    if (TimeLimit)
        cTimeLimit = 1;
    else
        cTimeLimit = 0;

    WORD wForSave = _wAtt;
    rt2_sprintf(g_strItemBuffer256, "%d,%d,%x,", id, index, wForSave);
    // 道具品质属性
    if (color == ItemColor_Green) {
        if (attribID1 == 0) {
            RtCoreLog().Info("[SItemID::SaveToString] item struct error (item id=%d)\n", id);
        }
        rt2_sprintf(cTmp128, "%d,%d,", attribID1, attribRate1);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    } else if (color == ItemColor_Blue) {
        if (attribID1 == 0 && attribID2 == 0) {
            RtCoreLog().Info("[SItemID::SaveToString] item struct error (item id=%d)\n", id);
        }
        rt2_sprintf(cTmp128, "%d,%d,%d,%d,", attribID1, attribRate1, attribID2, attribRate2);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    } else if (color == ItemColor_Golden) {
        if (attribID1 == 0 && attribID2 == 0) {
            RtCoreLog().Info("[SItemID::SaveToString] item struct error (item id=%d)\n", id);
        }
        rt2_sprintf(cTmp128, "%d,%d,%d,%d,", attribID1, attribRate1, attribID2, attribRate2);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    // 参数
    if (ItemIsPet(type)) {
        if (name.empty()) {
            name = "空";
        }
        rt2_sprintf(cTmp128, "%s,", name.c_str());
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    if (paramCount > ITEM_MAX_PARAM_COUNT) {
        paramCount = ITEM_MAX_PARAM_COUNT;
        RtCoreLog().Info("Item param count is ERROR: ItemID=%d\n", id);
    }
    rt2_sprintf(cTmp128, "%d,", paramCount);
    rt2_strcat(g_strItemBuffer256, cTmp128);
    for (int i = 0; i < paramCount; i++) {
        rt2_sprintf(cTmp128, "%d,", params[i]);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    if (cCountable)  // 数量
    {
        rt2_sprintf(cTmp128, "%d,", count);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    if (cEnchant)  // 附魔ID
    {
        rt2_sprintf(cTmp128, "%d,", enchantAttrib);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    if (cTimeLimit)  // 时间限制
    {
        rt2_sprintf(cTmp128, "%d,", TimeLimit);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }
    // 耐久
    if (ItemIsWeapon(type) || ItemIsArmor(type) || ItemIsShipin(type)) {
        rt2_sprintf(cTmp128, "%d,", curWear);
        rt2_strcat(g_strItemBuffer256, cTmp128);
    }

    CHECK(strlen(g_strItemBuffer256) < 256);
    return g_strItemBuffer256;

    unguard;
}

int SItemID::LoadFromString(int version, const char* str, CItemManager* pItemMgr) {
    guard;

    Clear();

    // do NOT use "str", because strtok will destroy "str", so copy the std::string
    int strLength = (int)strlen(str);
    if (strLength == 0)
        return 0;
    char* strCopy = RT_NEW char[strLength + 1];  //(char*)malloc(strLength+1);
    rt2_strncpy(strCopy, str, strLength + 1);

    char* pReadPtr  = strCopy;
    int   itemIndex = 0;

    if (version == 11) {
        char sep[] = ",";

        char* k = strtok(pReadPtr, sep);
        if (!k)
            return 0;
        id = atoi(k);
        k  = strtok(NULL, sep);
        if (!k)
            return 0;
        itemIndex = atoi(k);

        if (id != 0 && pItemMgr) {
            SItemBase* pItemBase = pItemMgr->GetItemFromIndex(itemIndex);
            if (pItemBase) {
                type = pItemBase->type;
            } else {
                RtCoreLog().Info("A item is INVALID, and now delete it!(item ID=%d, index=%d)\n",
                                 id, itemIndex);
                id   = 0;
                type = 0;
            }
        }

        k = strtok(NULL, sep);
        if (!k)
            return 0;
        _wAtt = (WORD)strtoul(k, NULL, 16);

        //color = atoi(k);
        //k = strtok(NULL, sep);
        //if (!k)		return 0;
        //count = atoi(k);
        //k = strtok(NULL, sep);
        //if (!k)		return 0;
        //level = atoi(k);
        //if (level<0 || level>ITEM_MAX_ITEM_LEVEL)
        //{
        //    RtCoreLog().Info("A item's level is invalid! Set its level to 0.(itemID=%d)\n", id);
        //    level = 0;
        //}
        //k = strtok(NULL, sep);
        //if (!k)		return 0;
        //cBind = atoi(k);
        //k = strtok(NULL, sep);
        //if (!k)		return 0;
        //cLock = atoi(k);
        //k = strtok(NULL, sep);

        if (color == ItemColor_Green) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            attribID1 = atoi(k);
            k         = strtok(NULL, sep);
            if (!k)
                return 0;
            attribRate1 = atoi(k);

            if (attribID1 == 0) {
                color = ItemColor_White;
            } else {
                if (!pItemMgr->m_pAttribMgr->IsAttribExist(attribID1))  // 找不到这个属性
                {
                    color = ItemColor_White;
                    RtCoreLog().Info("A green item's ATTRIB IS INVALID, and now change it white "
                                     "item!(item ID=%d, attrib=%d)\n",
                                     id, attribID1);
                }
            }

            if (attribRate1 > ATTRIB_GREEN_MAX_RATE || attribRate1 < ATTRIB_GREEN_MIN_RATE)
                attribRate1 = ATTRIB_GREEN_MIN_RATE;
        } else if (color == ItemColor_Blue) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            attribID1 = atoi(k);
            k         = strtok(NULL, sep);
            if (!k)
                return 0;
            attribRate1 = atoi(k);
            k           = strtok(NULL, sep);
            if (!k)
                return 0;
            attribID2 = atoi(k);
            k         = strtok(NULL, sep);
            if (!k)
                return 0;
            attribRate2 = atoi(k);

            if (attribID1 == 0 && attribID2 == 0) {
                color = ItemColor_White;
            } else {
                if (attribID1 != 0 && !pItemMgr->m_pAttribMgr->IsAttribExist(attribID1)) {
                    color = ItemColor_White;
                    RtCoreLog().Info("A blue item's ATTRIB(1) IS INVALID, and now change it white "
                                     "item!(item ID=%d, attrib=%d)\n",
                                     id, attribID1);
                } else if (attribID2 != 0 && !pItemMgr->m_pAttribMgr->IsAttribExist(attribID2)) {
                    RtCoreLog().Info("A blue item's ATTRIB(2) IS INVALID, and now change it white "
                                     "item!(item ID=%d, attrib=%d)\n",
                                     id, attribID2);
                    color = ItemColor_White;
                }
            }

            if (attribRate1 > ATTRIB_BLUE_MAX_RATE || attribRate1 < ATTRIB_BLUE_MIN_RATE)
                attribRate1 = ATTRIB_BLUE_MIN_RATE;
            if (attribRate2 > ATTRIB_BLUE_MAX_RATE || attribRate2 < ATTRIB_BLUE_MIN_RATE)
                attribRate2 = ATTRIB_BLUE_MIN_RATE;
        } else if (color == ItemColor_Golden) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            attribID1 = atoi(k);
            k         = strtok(NULL, sep);
            if (!k)
                return 0;
            attribRate1 = atoi(k);
            k           = strtok(NULL, sep);
            if (!k)
                return 0;
            attribID2 = atoi(k);
            k         = strtok(NULL, sep);
            if (!k)
                return 0;
            attribRate2 = atoi(k);

            if (attribID1 == 0 && attribID2 == 0) {
                color = ItemColor_White;
            } else {
                if (attribID1 != 0 && !pItemMgr->m_pAttribMgr->IsAttribExist(attribID1)) {
                    color = ItemColor_White;
                    RtCoreLog().Info("A golden item's ATTRIB(1) IS INVALID, and now change it "
                                     "white item!(item ID=%d, attrib=%d)\n",
                                     id, attribID1);
                } else if (attribID2 != 0 && !pItemMgr->m_pAttribMgr->IsAttribExist(attribID2)) {
                    RtCoreLog().Info("A golden item's ATTRIB(2) IS INVALID, and now change it "
                                     "white item!(item ID=%d, attrib=%d)\n",
                                     id, attribID2);
                    color = ItemColor_White;
                }
            }

            if (attribRate1 > ATTRIB_GOLDEN_MAX_RATE || attribRate1 < ATTRIB_GOLDEN_MIN_RATE)
                attribRate1 = ATTRIB_GOLDEN_MIN_RATE;
            if (attribRate2 > ATTRIB_GOLDEN_MAX_RATE || attribRate2 < ATTRIB_GOLDEN_MIN_RATE)
                attribRate2 = ATTRIB_GOLDEN_MIN_RATE;
        } else {
            color = ItemColor_White;
        }

        // 参数
        if (ItemIsPet(type)) {
            k = strtok(NULL, sep);
            if (!k)
                k = "";
            name = k;
        }

        k = strtok(NULL, sep);
        if (!k)
            return 0;
        paramCount = atoi(k);
        if (paramCount > ITEM_MAX_PARAM_COUNT)
            return 0;
        for (int iParam = 0; iParam < paramCount; iParam++) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            params[iParam] = atoi(k);
        }

        //// 特殊道具，法宝、令牌的参数检查
        //if(ItemIsWeapon_MagicWeapon(type))
        //{
        //    SWeapon *pWea = (SWeapon*)pItemMgr->GetItemFromIndex(itemIndex);
        //    if(params[MW_PARAM_LEV] < pWea->MagicWeaponParams[MW_PARAM_LEV])
        //        params[MW_PARAM_LEV] = pWea->MagicWeaponParams[MW_PARAM_LEV];
        //}

        if (cCountable) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            count = atoi(k);
        }
        if (cEnchant) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            enchantAttrib = atoi(k);
        }
        if (cTimeLimit) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            TimeLimit = atoi(k);
        }
        if (ItemIsWeapon(type) || ItemIsArmor(type) || ItemIsShipin(type)) {
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            curWear = atoi(k);
        }

        if (ItemIsTrump(type)) {
            STrump* pTrump = (STrump*)pItemMgr->GetItem(type);
            if (!pTrump) {
                //rt2_sprintf(cTmp128, "%d,%d,%d,%d,%d,", pTrump->nCurLevel,pTrump->TrumpCurMp,pTrump->AttachType,
                //	pTrump->AttachPiont,pTrump->AttachCurMp[pTrump->AttachType]);
                //rt2_strcat(g_strItemBuffer256, cTmp128);
            }
        }

        //// 附魔ID
        //k = strtok(NULL, sep);
        //if (!k)			return 0;
        //enchantAttrib = atoi(k);

        //if (ItemIsPet(type))
        //    cLock = 0;

        if (!k)
            return 0;
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else {
        DEL_ARRAY(strCopy);
        RtCoreLog().Info("[SItemID::LoadFromString] Invalid version!\n");
        return 0;
    }

    unguard;
}

CItemIDManager::CItemIDManager() {
    m_nMaxID = 0;
}

CItemIDManager::~CItemIDManager() {}

bool CItemIDManager::Init() {
    return true;
}

DWORD CItemIDManager::NewID() {
    guard;

    return GetNextItemID();

    unguard;
}

CItemComposeMgr::CItemComposeMgr(CItemManager* pItemMgr) {
    m_pItemMgr = pItemMgr;
}

CItemComposeMgr::~CItemComposeMgr() {
    m_Composings.clear();
}

bool CItemComposeMgr::Init() {
    if (!LoadResource(R(RES_COMPOSE)))
        return false;

    return true;
}

SItemCompose* CItemComposeMgr::FindComposing(DWORD Id) {
    if (Id == 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemCompose>::iterator iter = m_Composings.find(Id);
    if (iter == m_Composings.end()) {
        return NULL;
    } else {
        return &(iter->second);
    }
}

bool CItemComposeMgr::CheckCompose(DWORD ComposeId, CItemContainerBase* pItemContainer,
                                   int* pCanMakeCount /* = NULL*/) {
    SItemCompose* pComposeClass = FindComposing(ComposeId);
    BOOL_ENSURE(pComposeClass);
    SItemBase* pItemClass = NULL;

    int nCanMakeCount = 10000;
    for (int i = 0; i < (int)pComposeClass->MaterialTypeCount; i++) {
        pItemClass = m_pItemMgr->GetItemFromIndex(pComposeClass->Materials[i]);
        BOOL_ENSURE(pItemClass);

        int nHaveCount = 0;
        int nNeedCount = pComposeClass->MaterialNums[i];
        pItemContainer->m_Bag.FindType(pComposeClass->Materials[i], ItemColor_All, &nHaveCount);

        int nTmp = nHaveCount / nNeedCount;
        if (nTmp < nCanMakeCount)
            nCanMakeCount = nTmp;
    }
    if (pCanMakeCount)
        *pCanMakeCount = nCanMakeCount;

    if (nCanMakeCount > 0)
        return true;
    else
        return false;
}

bool CItemComposeMgr::LoadResource(const char* FileName) {
    RtCsv csv;
    if (!csv.LoadTableFile(FileName)) {
        RtCoreLog().Info("[CItemComposeMgr::LoadResource] Can NOT open compose file(%s)\n",
                         FileName);
        return false;
    }
    int LineCount = csv.GetLineCount();
    int i         = csv.FindLineByString("ID");
    if (i < 0)
        return false;

    SItemCompose ic;
    char         sep[] = ";";
    char*        Name;
    char*        Desc;
    char *       MaterialIDs, *MaterialNums;
    DWORD        MaterialID, MaterialNum;
    for (i++; i < LineCount; i++) {
        ic.Idx = csv[i]["ID"].Long();
        if (ic.Idx == 0)
            continue;
        Name           = (char*)csv[i]["名字"].Str();
        ic.Level       = csv[i]["等级"].Short();
        ic.SkillLevel  = csv[i]["技能等级"].Short();
        ic.ComposeType = csv[i]["合成类型"].Long();
        ic.Product     = csv[i]["成品"].Long();
        MaterialIDs    = (char*)csv[i]["材料"].Str();
        MaterialNums   = (char*)csv[i]["材料数量"].Str();
        Desc           = (char*)csv[i]["说明"].Str();
        ic.GreenRate   = csv[i]["绿色几率"].Double();
        ic.BlueRate    = csv[i]["蓝色几率"].Double();
        ic.GoldenRate  = csv[i]["金色几率"].Double();

        //添加失败几率 Cheney 2009.3.18
        ic.FailRate  = csv[i]["失败几率"].Double();
        ic.FaidItems = csv[i]["失败产品"].Long();

        rt2_snprintf(ic.Name, PATH_STRING_LEN, Name);
        //ic.Name[DESC_STRING_LEN-1] = '\0';
        rt2_snprintf(ic.Desc, DESC_STRING_LEN, Desc);
        //ic.Desc[DESC_STRING_LEN-1] = '\0';

        if (!ItemComposeType_IsValid((EItemComposeType)ic.ComposeType)) {
            RtCoreLog().Info("Error Compose Type!(Line %d)\n", i);
            return false;
        }

        if (!MaterialIDs || MaterialIDs[0] == '\0')
            return false;
        if (!MaterialNums || MaterialNums[0] == '\0')
            return false;

        int   counterID = 0;
        char* k         = strtok(MaterialIDs, sep);
        while (k) {
            MaterialID              = atoi(k);
            ic.Materials[counterID] = (DWORD)MaterialID;
            counterID++;
            k = strtok(NULL, sep);
        }

        int counterNum = 0;
        k              = strtok(MaterialNums, sep);
        while (k) {
            MaterialNum                 = atoi(k);
            ic.MaterialNums[counterNum] = (DWORD)MaterialNum;
            counterNum++;
            k = strtok(NULL, sep);
        }

        if (counterID != counterNum) {
            RtCoreLog().Info(
                "[CItemComposeMgr::LoadResource] Material ID and Count NOT pair! (Line %d)\n", i);
            return false;
        }
        ic.MaterialTypeCount = counterID;

        char* cT = (char*)csv[i]["强化概率"].Str();
        k        = strtok(cT, sep);
        int kkk  = 0;
        while (k) {
            int nn                      = (int)(atof(k) * 10000);
            ic.m_UpdateRate[kkk++]      = nn;
            ic.MaterialNums[counterNum] = (DWORD)MaterialNum;
            k                           = strtok(NULL, sep);
        }

        if (FindComposing(ic.Idx)) {
            RtCoreLog().Info("[CItemComposeMgr::LoadResource] Find same ID (Line %d)\n", ic.Idx);
            return false;
        }

        ic.m_dwNeetMoney = csv[i]["需求银币"].Long();

        m_Composings[ic.Idx] = ic;
    }

    return true;
}

int ItemDecompose_MaxItemLevel(int SkillLevel) {
    static int MAX_ITEM_LEVEL[6] = {0, 30, 40, 50, 60, 70};
    if (SkillLevel <= 0 || SkillLevel > 5)
        return 0;
    return MAX_ITEM_LEVEL[SkillLevel];
}

CItemDecomposeMgr::CItemDecomposeMgr() {}

CItemDecomposeMgr::~CItemDecomposeMgr() {
    m_Decomposings.clear();
}

bool CItemDecomposeMgr::Init() {
    if (!LoadResource(R(RES_DECOMPOSE)))
        return false;

    return true;
}

SItemDecompose* CItemDecomposeMgr::FindDecomposing(DWORD Id) {
    if (Id == 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemDecompose>::iterator iter = m_Decomposings.find(Id);
    if (iter == m_Decomposings.end()) {
        return NULL;
    } else {
        return &(iter->second);
    }
}

bool CItemDecomposeMgr::LoadResource(const char* FileName) {
    RtCsv csv;
    if (!csv.LoadTableFile(FileName)) {
        RtCoreLog().Info("[CItemDecomposeMgr::LoadResource] Can NOT open compose file(%s)\n",
                         FileName);
        return false;
    }
    int LineCount = csv.GetLineCount();
    int i         = csv.FindLineByString("ID");
    if (i < 0)
        return false;

    SItemDecompose dc;
    char           sep[] = ";";
    char *         MaterialIDs, *MaterialNums_Green, *MaterialNums_Blue, *MaterialNums_Golden;
    DWORD          MaterialID, MaterialNum;
    for (i++; i < LineCount; i++) {
        dc.Idx = csv[i]["ID"].Long();
        if (dc.Idx == 0)
            continue;
        MaterialIDs         = (char*)csv[i]["材料"].Str();
        MaterialNums_Green  = (char*)csv[i]["绿色材料数量"].Str();
        MaterialNums_Blue   = (char*)csv[i]["蓝色材料数量"].Str();
        MaterialNums_Golden = (char*)csv[i]["金色材料数量"].Str();

        if (!MaterialIDs || MaterialIDs[0] == '\0')
            return false;
        if ((!MaterialNums_Green || MaterialNums_Green[0] == '\0') ||
            (!MaterialNums_Blue || MaterialNums_Blue[0] == '\0') ||
            (!MaterialNums_Golden || MaterialNums_Golden[0] == '\0'))
            return false;

        int   counterID = 0;
        char* k         = strtok(MaterialIDs, sep);
        while (k) {
            MaterialID              = atoi(k);
            dc.Materials[counterID] = (DWORD)MaterialID;
            counterID++;
            k = strtok(NULL, sep);
        }

        int counterNum_Green = 0;
        k                    = strtok(MaterialNums_Green, sep);
        while (k) {
            MaterialNum                             = atoi(k);
            dc.MaterialNums_Green[counterNum_Green] = (DWORD)MaterialNum;
            counterNum_Green++;
            k = strtok(NULL, sep);
        }
        int counterNum_Blue = 0;
        k                   = strtok(MaterialNums_Blue, sep);
        while (k) {
            MaterialNum                           = atoi(k);
            dc.MaterialNums_Blue[counterNum_Blue] = (DWORD)MaterialNum;
            counterNum_Blue++;
            k = strtok(NULL, sep);
        }
        int counterNum_Golden = 0;
        k                     = strtok(MaterialNums_Golden, sep);
        while (k) {
            MaterialNum                               = atoi(k);
            dc.MaterialNums_Golden[counterNum_Golden] = (DWORD)MaterialNum;
            counterNum_Golden++;
            k = strtok(NULL, sep);
        }

        if ((counterID != counterNum_Green) || (counterID != counterNum_Blue) ||
            (counterID != counterNum_Golden)) {
            RtCoreLog().Info(
                "[CItemDecomposeMgr::LoadResource] Material ID and Count NOT pair! (Line %d)\n", i);
            return false;
        }
        dc.MaterialTypeCount = counterID;

        if (FindDecomposing(dc.Idx)) {
            RtCoreLog().Info("[CItemDecomposeMgr::LoadResource] Find same ID (Line %d)\n", dc.Idx);
            return false;
        }
        m_Decomposings[dc.Idx] = dc;
    }

    return true;
}

CSuitItemManager::CSuitItemManager(CItemManager* pMgr) {
    m_pItemMgr = pMgr;
}

CSuitItemManager::~CSuitItemManager() {
    guard;

    EXT_SPACE::unordered_map<DWORD, SSuit*>::iterator iter = m_suits.begin();
    for (; iter != m_suits.end(); iter++) {
        SSuit* pSuit = (*iter).second;
        DEL_ONE(pSuit);
        //if (pSuit)		delete pSuit;
    }

    unguard;
}

bool CSuitItemManager::Init() {
    guard;
    CHECK(m_pItemMgr);
    if (!LoadResource())
        return false;
    return true;
    unguard;
}

bool CSuitItemManager::LoadResource() {
    guard;

    RtCsv csv;
    if (!csv.LoadTableFile(R(RES_TABLE_SUIT_ITEMS)))
        return false;
    int nLineCount = csv.GetLineCount();
    //int nColumnCount = csv.GetMaxRow();

    //const char sep[] = "&";	// 分隔符

    for (int i = 1; i < nLineCount; i++) {
        SSuit* pSuit = RT_NEW SSuit;

        pSuit->id = csv[i]["ID"].Int();
        if (pSuit->id == 0) {
            RtCoreLog().Info("套装ID不合法，(第%d行)，忽略", i);
            continue;
        }
        pSuit->name     = csv[i]["名称"].Str();
        pSuit->attribID = csv[i]["追加属性ID"].Int();
        pSuit->effect   = csv[i]["特效文件"].Str();
        pSuit->desc     = csv[i]["备注"].Str();

        //pChar = (char*)csv[i]["道具类型"].Str();
        //if (pChar && strlen(pChar)>0)
        //{
        //	char *k = strtok(pChar, sep);
        //	while (k)
        //	{
        //		int n = atoi(k);
        //		SItemBase *pBase = m_pItemMgr->GetItemFromIndex(n);
        //		if ( !pBase )	return false;
        //		pSuit->arrTypes.push_back(pBase->type);
        //		k = strtok(NULL, sep);
        //	}
        //}

        // 是不是有相同的id
        EXT_SPACE::unordered_map<DWORD, SSuit*>::iterator iter = m_suits.find(pSuit->id);
        if (iter != m_suits.end()) {
            RtCoreLog().Info(
                "[CSuitItemManager::LoadResource] Same ID found in SuitItem table!(line %d)", i);
            CHECK(0);
            return false;
        }

        m_suits[pSuit->id] = pSuit;
    }

    return true;
    unguard;
}

SSuit* CSuitItemManager::CheckSuit(SItemID pItems[], int count) {
    guard;

    SSuit* pSuit = NULL;
    int    i, j;

    if (count <= 0)
        return NULL;

    for (i = 0; i < count; i++)
        if (!ItemID_IsValid(pItems[i]))
            return NULL;

    SItemBase* pItemBase = m_pItemMgr->GetItem(pItems[0].type);
    if (!pItemBase)
        return NULL;
    DWORD suitID = pItemBase->suitID;
    if (suitID != 0) {
        EXT_SPACE::unordered_map<DWORD, SSuit*>::iterator iter = m_suits.find(suitID);
        if (iter == m_suits.end()) {
            RtCoreLog().Info("[CSuitItemManager::CheckSuit] Invalid suit ID(%d)\n", suitID);
            return NULL;
        } else {
            pSuit = (*iter).second;
        }
    } else
        return NULL;

    for (j = 1; j < count; j++) {
        SItemBase* pItemBase = m_pItemMgr->GetItem(pItems[j].type);
        if (!pItemBase)
            return NULL;
        if (suitID == pItemBase->suitID)
            continue;
        else
            return NULL;
    }
    return pSuit;

    //EXT_SPACE::unordered_map<DWORD, SSuit*>::iterator iter = m_suits.begin();
    //for (; iter!=m_suits.end(); iter++)
    //{
    //	pSuit = (*iter).second;
    //	for (i=0; i < (int)pSuit->arrTypes.size(); i++)
    //	{
    //		for (j=0; j<count; j++)
    //		{
    //			if ( pItems[j].type == pSuit->arrTypes[i] )
    //				break;
    //		}
    //		if (j==count)
    //			break;
    //	}
    //	if ( i == pSuit->arrTypes.size() )
    //		return pSuit;
    //}
    //return NULL;

    unguard;
}

SSuit* CSuitItemManager::GetSuit(DWORD suitID) {
    guard;

    if (suitID <= 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SSuit*>::iterator iter = m_suits.find(suitID);
    if (iter != m_suits.end())
        return (*iter).second;
    else
        return NULL;

    unguard;
}

CAttribMgr::CAttribMgr() {}

CAttribMgr::~CAttribMgr() {}

bool CAttribMgr::Init() {
    guard;

    if (!LoadResource())
        return false;

    return true;

    unguard;
}

bool CAttribMgr::IsAttribExist(WORD id) {
    guard;

    if (id <= 0)
        return false;

    EXT_SPACE::unordered_map<WORD, std::vector<SAttribAddtion>>::iterator iter = m_attribs.find(id);
    if (iter == m_attribs.end())
        return false;
    else
        return true;

    unguard;
}

bool CAttribMgr::GetAttrib(WORD id, std::vector<SAttribAddtion>& arrAttribs) {
    guard;

    if (id <= 0)
        return false;

    EXT_SPACE::unordered_map<WORD, std::vector<SAttribAddtion>>::iterator iter = m_attribs.find(id);
    if (iter == m_attribs.end())
        return false;
    else {
        arrAttribs = iter->second;
        return true;
    }

    unguard;
}

BYTE CAttribMgr::CreateAttribRate(ColorSelector color) {
    guard;

    int iRate = 0;
    if (color == csGreen) {
        iRate =
            rand() % (ATTRIB_GREEN_MAX_RATE - ATTRIB_GREEN_MIN_RATE + 1) + ATTRIB_GREEN_MIN_RATE;
    } else if (color == csBlue1 || color == csBlue2) {
        iRate = rand() % (ATTRIB_BLUE_MAX_RATE - ATTRIB_BLUE_MIN_RATE + 1) + ATTRIB_BLUE_MIN_RATE;
    } else if (color == csGolden1 || color == csGolden2) {
        iRate =
            rand() % (ATTRIB_GOLDEN_MAX_RATE - ATTRIB_GOLDEN_MIN_RATE + 1) + ATTRIB_GOLDEN_MIN_RATE;
    } else if (color == csSuit) {
        iRate = SUIT_DEFAULT_ATTRIB_RATE;
    } else {
        iRate = 0;
    }
    return iRate;

    unguard;
}

WORD CAttribMgr::CreateAttrib(int selector, ColorSelector nColor) {
    guard;

    CHECK(selector >= 0 && selector < ITEM_MAX_GRADE_COUNT);

    std::vector<WORD>* pArray = &(m_finder[nColor][selector]);
    int                count  = (int)pArray->size();
    if (count > 0) {
        int r = rand() % count;
        return (*pArray)[r];
    } else {
        return 0;
    }

    unguard;
}

void CAttribMgr::GetPossibleAttribs(int selector, ColorSelector nColor, std::vector<WORD>& arrID) {
    guard;

    arrID.clear();

    CHECK(selector >= 0 && selector < ITEM_MAX_GRADE_COUNT);

    arrID = m_finder[nColor][selector];

    unguard;
}

bool CAttribMgr::LoadResource() {
    guard;

    std::vector<SAttribAddtion> arrAttribs;
    SAttribAddtion              att;
    DWORD                       id;

    RtCsv csv;
    if (!csv.LoadTableFile(R(RES_TABLE_ATTRIBS)))
        return false;
    int nLineCount = csv.GetLineCount();

    const char sep[] = "&";  // 分隔符
    int        nTmp;
    float      fTmp;

    int titleLine = csv.FindLineByString("ID");
    if (titleLine < 0) {
        RtCoreLog().Info("Failed [Can't find title field].\n");
        return false;
    }
    csv.SetTitleLine(titleLine);

    for (int i = titleLine + 1; i < nLineCount; i++) {
        arrAttribs.clear();

        id = csv[i]["ID"].DWord();

        if (id == 0) {
            continue;
        }

        att.value = csv[i]["附加价格"].Float();

        //att.reqStr = csv[i]["力量需求"].Int();
        //att.reqDex = csv[i]["敏捷需求"].Int();
        //att.reqInt = csv[i]["智力需求"].Int();
        //att.reqHit = csv[i]["精准需求"].Int();

        if ((nTmp = csv[i]["体质"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_CON;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["力量"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_STR;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["敏捷"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_DEX;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["智力"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_INT;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["精准"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_HIT;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        //if ( ( nTmp = csv[i]["命中"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_ATTACK;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //pChar = (char *)csv[i]["伤害"].Str();
        //if (pChar && strlen(pChar)>0)
        //{
        //	char* k = strtok(pChar, sep);
        //	nTmp = atoi(k);
        //	if (nTmp > 0)
        //	{
        //		att.attrib = ITEM_ATTRIB_DAMAGE;
        //		att.count = nTmp;
        //		arrAttribs.push_back(att);
        //	}
        //}
        //if ( ( nTmp = csv[i]["最小伤害"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_DAMAGE_MIN;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["最大伤害"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_DAMAGE_MAX;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["闪躲(%)"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_RATE_DODGE;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["防御(%)"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_RATE_ARMOR;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        if ((nTmp = csv[i]["杀怪金钱获得率"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_GOLD;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["移动速度"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_MOVESPEED;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        //if ( ( nTmp = csv[i]["幸运"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_LUCK;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["附加技能"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_SKILL;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["MaxMp(%)"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_RATE_MAXMP;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}
        //if ( ( nTmp = csv[i]["MaxHp(%)"].Int() ) != 0)
        //{
        //	att.attrib = ITEM_ATTRIB_RATE_MAXHP;
        //	att.count = nTmp;
        //	arrAttribs.push_back(att);
        //}

        if ((nTmp = csv[i]["伤害吸收"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_DAMAGE_ABSORB;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["吸血"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_ABSORBHP;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["吸魔"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_ABSORBMP;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        //if ( ( nTmp = csv[i]["比率吸魔"].Int() ) != 0)
        //{
        //    att.attrib = ITEM_ATTRIB_REFAINT;
        //    att.count = nTmp;
        //    arrAttribs.push_back(att);
        //}

        if ((fTmp = csv[i]["体质强度"].Float()) != 0) {
            att.attrib = ITEM_ATTRIB_CON_POWER;
            att.count  = fTmp * 100;
            arrAttribs.push_back(att);
        }
        if ((fTmp = csv[i]["力量强度"].Float()) != 0) {
            att.attrib = ITEM_ATTRIB_STR_POWER;
            att.count  = fTmp * 100;
            arrAttribs.push_back(att);
        }
        if ((fTmp = csv[i]["敏捷强度"].Float()) != 0) {
            att.attrib = ITEM_ATTRIB_DEX_POWER;
            att.count  = fTmp * 100;
            arrAttribs.push_back(att);
        }
        if ((fTmp = csv[i]["智力强度"].Float()) != 0) {
            att.attrib = ITEM_ATTRIB_INT_POWER;
            att.count  = fTmp * 100;
            arrAttribs.push_back(att);
        }
        if ((fTmp = csv[i]["精准强度"].Float()) != 0) {
            att.attrib = ITEM_ATTRIB_HIT_POWER;
            att.count  = fTmp * 100;
            arrAttribs.push_back(att);
        }

        if ((nTmp = csv[i]["必杀"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_CRITICALHIT;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["攻击速度"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_ATTACKSPEED;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["攻击距离"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_ATTACKRANGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["伤害"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_DAMAGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["防御"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_ARMOR;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["命中"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_ATTACK;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["闪躲"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_DODGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["水攻"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_WATERDAMAGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["火攻"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_FIREDAMAGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["毒攻"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_POISONDAMAGE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["水防"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_WATERARMOR;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["火防"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_FIREARMOR;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["毒防"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_POISONARMOR;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["hp"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_MAXHP;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["mp"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_MAXMP;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        if ((nTmp = csv[i]["抗必杀"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RECRITICALHIT;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        if ((nTmp = csv[i]["抗暴击"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RESKILLCRITICAL;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        if ((nTmp = csv[i]["抗晕击"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_REFAINT;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        if ((nTmp = csv[i]["格挡"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_REPARRY;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["装备后技能"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_APPENDSKILL;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        nTmp = csv[i]["击中后主体释放效果"].Int();
        if (nTmp != 0) {
            att.attrib    = ITEM_ATTRIB_RELEASEEFFECT_SELF;
            att.count     = nTmp;
            fTmp          = csv[i]["主体发生率"].Float();
            att.skillRate = fTmp * 100;
            if (att.skillRate > 100) {
                RtCoreLog().Info("[CAttribMgr::LoadResource] Skill Rate Error!(line %d)\n", i);
                att.skillRate = 0;
            }
            arrAttribs.push_back(att);
        }
        nTmp = csv[i]["击中后客体释放效果"].Int();
        if (nTmp != 0) {
            att.attrib    = ITEM_ATTRIB_RELEASEEFFECT_ENEMY;
            att.count     = nTmp;
            nTmp          = csv[i]["客体发生率"].Int();
            att.skillRate = fTmp * 100;
            if (att.skillRate > 100) {
                RtCoreLog().Info("[CAttribMgr::LoadResource] Skill Rate Error!(line %d)\n", i);
                att.skillRate = 0;
            }
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["爆击率"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_SKILLCRITICAL_RATE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["爆击伤害"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_SKILLCRITICAL_DAM;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["杀怪经验获得率"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_RATE_EXP;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["技能1"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_SKILLONE;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }
        if ((nTmp = csv[i]["技能2"].Int()) != 0) {
            att.attrib = ITEM_ATTRIB_SKILLTWO;
            att.count  = nTmp;
            arrAttribs.push_back(att);
        }

        int selector = csv[i]["阶段"].Int();
        int nColor   = csv[i]["品质"].Int();

        CHECK(selector >= 0 && selector < ITEM_MAX_GRADE_COUNT);
        CHECK(nColor >= 0 && nColor < csMax);
        m_finder[nColor][selector].push_back(id);

        // 确认没有相同的id
        EXT_SPACE::unordered_map<WORD, vector<SAttribAddtion>>::iterator iter = m_attribs.find(id);
        if (iter != m_attribs.end()) {
            RtCoreLog().Info(
                "[CAttribMgr::LoadResource] Same ID found in ItemAttrib table!(line %d)", i);
            CHECK(0);
            return false;
        }

        m_attribs[id] = arrAttribs;
    }

    return true;

    unguard;
}

bool AnalyseEqualClause(std::string& strClause, std::string& token, std::string& value) {
    guard;

    char cTmp1024[1024];
    if (strClause.size() >= 1024) {
        RtCoreLog().Info("[AnalyseEqualClause] Clause is too long!(MaxSize=%d)\n", 1024);
        return false;
    }
    rt2_strncpy(cTmp1024, strClause.c_str(), 1024);

    char sep[] = "=";

    char* p = strtok(cTmp1024, sep);
    if (!p)
        return false;
    token = p;
    p     = strtok(NULL, sep);
    if (!p)
        return false;
    value = p;

    return true;

    unguard;
}

bool SCreateItem::IsValid(CItemManager* pItemMgr) {
    guard;

    SItemBase* pItemClass = pItemMgr->GetItemFromIndex(index);
    if (!pItemClass)
        return false;
    if (level < 0 || level > ITEM_MAX_ITEM_LEVEL)
        return false;
    if (!ItemBind_IsValid(cBind))
        return false;
    if (!ItemColor_IsValid((EItemColor)color))
        return false;

    return true;
    unguard;
}

bool SCreateItem::SaveToString(CItemManager* pItemMgr, std::string& outStr) {
    guard;

    outStr = "";
    std::string        strTmp;
    static std::string strSep = ";";

    if (!IsValid(pItemMgr)) {
        RtCoreLog().Info("[SCreateItem::SaveToString] Struct is Not "
                         "invalid!(id=%d,color=%d,level=%d,cBind=%d)\n",
                         index, color, level, cBind);
        return false;
    }

    strTmp = "id=";
    strTmp += rtFormatNumber(index);
    strTmp += strSep;
    outStr += strTmp;

    strTmp = "color=";
    strTmp += rtFormatNumber(color);
    strTmp += strSep;
    outStr += strTmp;

    strTmp = "lev=";
    strTmp += rtFormatNumber(level);
    strTmp += strSep;
    outStr += strTmp;

    strTmp = "bind=";
    strTmp += rtFormatNumber(cBind);
    strTmp += strSep;
    outStr += strTmp;

    if (colorRate) {
        strTmp = "colRate=";
        strTmp += rtFormatNumber(colorRate);
        strTmp += strSep;
        outStr += strTmp;
    }

    if (colorAttrib1) {
        strTmp = "col1=";
        strTmp += rtFormatNumber(colorAttrib1);
        strTmp += strSep;
        outStr += strTmp;
    }

    if (colorAttrib2) {
        strTmp = "col2=";
        strTmp += rtFormatNumber(colorAttrib2);
        strTmp += strSep;
        outStr += strTmp;
    }

    if (limitTime) {
        strTmp = "t=";
        strTmp += rtFormatNumber(limitTime);
        strTmp += strSep;
        outStr += strTmp;
    }

    return true;

    unguard;
}

bool SCreateItem::LoadFromString(CItemManager* pItemMgr, std::string& CreateString) {
    guard;

    // id=2;color=1;level=2;

    CHECK(pItemMgr);

    char cTmp1024[1024];
    if (CreateString.size() >= 1024) {
        RtCoreLog().Info("[SCreateItem::LoadFromString] Clause is too long!(MaxSize=%d)\n", 1024);
        return false;
    }
    rt2_strncpy(cTmp1024, CreateString.c_str(), 1024);

    char                sep[] = ";";
    vector<std::string> arrTokens;
    char*               p = strtok(cTmp1024, sep);
    while (p) {
        arrTokens.push_back(p);
        p = strtok(NULL, sep);
    }

    for (int i = 0; i < (int)arrTokens.size(); i++) {
        std::string token;
        std::string value;
        if (!AnalyseEqualClause(arrTokens[i], token, value))
            return false;

        if (token == "id") {
            index = atoi(value.c_str());
        } else if (token == "lev") {
            level = atoi(value.c_str());
        } else if (token == "bind") {
            int BindValue = atoi(value.c_str());
            cBind         = (EItemBind)BindValue;
        } else if (token == "color") {
            color = atoi(value.c_str());
        } else if (token == "colRate") {
            colorRate = atoi(value.c_str());
        } else if (token == "col1") {
            colorAttrib1 = atoi(value.c_str());
        } else if (token == "col2") {
            colorAttrib2 = atoi(value.c_str());
        } else if (token == "t") {
            limitTime = atoi(value.c_str());
        } else {
            RtCoreLog().Info("[SCreateItem::LoadFromString] Invalid token(ignored)!(token=%s)\n",
                             token.c_str());
        }
    }

    if (!IsValid(pItemMgr))
        return false;

    return true;

    unguard;
}

CItemManager::CItemManager() {
    guard;

    m_pIDManager         = RT_NEW         CItemIDManager();
    m_pAttribMgr         = RT_NEW         CAttribMgr();
    m_pSuitManager       = RT_NEW       CSuitItemManager(this);
    m_pSelectionTable    = RT_NEW    CItemSelectionTable();
    m_pComposeMgr        = RT_NEW        CItemComposeMgr(this);
    m_pDecomposeMgr      = RT_NEW      CItemDecomposeMgr();
    m_pMagicEggRewardMgr = RT_NEW CMagicEggRewardMgr(this);
    m_classCounter       = 0;

    m_BankInitLineCount = BANK_GRID_LINE_INIT_COUNT;
    m_BankMaxLineCount  = BANK_LINE_MAX;

    unguard;
}

CItemManager::~CItemManager() {
    guard;

    DEL_ONE(m_pIDManager);
    DEL_ONE(m_pAttribMgr);
    DEL_ONE(m_pSuitManager);
    DEL_ONE(m_pSelectionTable);
    DEL_ONE(m_pComposeMgr);
    DEL_ONE(m_pDecomposeMgr);
    DEL_ONE(m_pMagicEggRewardMgr);
    //if (m_pIDManager)		delete m_pIDManager;
    //if (m_pAttribMgr)		delete m_pAttribMgr;
    //if (m_pSuitManager)		delete m_pSuitManager;
    //if (m_pSelectionTable)	delete m_pSelectionTable;
    //if (m_pComposeMgr)		delete m_pComposeMgr;
    //if (m_pDecomposeMgr)	delete m_pDecomposeMgr;
    //   if (m_pMagicEggRewardMgr)   delete m_pMagicEggRewardMgr;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapItems.begin();
    for (; iter != m_mapItems.end(); ++iter) {
        SItemBase* pI = (*iter).second;
        if (pI) {
            if (pI->pDesc != NULL) {
                DEL_ARRAY(pI->pDesc);
            }
            DEL_ONE(pI);
        }
    }
    m_mapItems.clear();

    unguard;
}

bool CItemManager::Init() {
    guard;

    // 仓库大小初始化
    const char* strBankInitLine = GetGameRule()->GetEntry("GameLevelRule", "BankInitLine");
    if (strBankInitLine == NULL) {
        RtCoreLog().Info("[CItemManager::Init] Invalid GameRule Init File\n");
        return false;
    }

    const char* strBankMaxLine = GetGameRule()->GetEntry("GameLevelRule", "BankMaxLine");
    if (strBankMaxLine == NULL) {
        RtCoreLog().Info("[CItemManager::Init] Invalid GameRule Init File\n");
        return false;
    }

    const char* strPointValue = GetGameRule()->GetEntry("GameLevelRule", "pointvalue");
    if (strPointValue == NULL) {
        RtCoreLog().Info("[CItemManager::Init] Invalid GameRule Init File\n");
        return false;
    }

    m_BankInitLineCount = atoi(strBankInitLine);
    m_BankMaxLineCount  = atoi(strBankMaxLine);
    m_PointValue        = atoi(strPointValue);

    if (m_BankInitLineCount <= 0 || m_BankMaxLineCount <= 0 ||
        m_BankInitLineCount > MAX_BAG_LINE_COUNT || m_BankMaxLineCount > MAX_BAG_LINE_COUNT) {
        RtCoreLog().Info("[CItemManager::Init] Invalid BankInitLine or BankMaxLine\n");
        return false;
    }

    if (!m_pIDManager->Init())
        return false;
    if (!m_pAttribMgr->Init())
        return false;
    if (!LoadResource())
        return false;
    if (!m_pSuitManager->Init())
        return false;
    if (!m_pSelectionTable->Init(this))
        return false;
    if (!m_pComposeMgr->Init())
        return false;
    if (!m_pDecomposeMgr->Init())
        return false;
    if (!m_pMagicEggRewardMgr->Init())
        return false;

    return true;

    unguard;
}

SItemBase* CItemManager::GetItemFromIndex(DWORD index) {
    guard;

    if (index == 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapIndexToItem.find(index);
    if (iter != m_mapIndexToItem.end()) {
        return iter->second;
    } else {
        //RtCoreLog().Info("没有找到这种类型的物品: %d\n", index);
        return NULL;
    }

    unguard;
}

SItemBase* CItemManager::GetItem(DWORD type) {
    guard;

    if (type == 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapItems.find(type);
    if (iter != m_mapItems.end()) {
        return iter->second;
    } else {
        RtCoreLog().Info("没有找到这种类型的物品   0x%x\n", type);
        return NULL;
    }

    unguard;
}

SItemBase* CItemManager::GetItem(RtString name) {
    guard;

    if (name == "")
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapItems.begin();
    for (; iter != m_mapItems.end(); iter++) {
        SItemBase* pI = (*iter).second;
        if (strncmp(name, pI->name, PATH_STRING_LEN) == 0) {
            return pI;
        }
    }
    return NULL;

    unguard;
}

SItem* CItemManager::GetItem(const SItemID& id) {
    guard;

    if (!ItemID_IsValid(id))
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapItems.find(id.type);
    if (iter != m_mapItems.end()) {
        m_ItemInstance[m_classCounter].id        = id;
        m_ItemInstance[m_classCounter].pItemBase = iter->second;

        int tmp = m_classCounter;
        m_classCounter++;
        if (m_classCounter >= ITEM_MAX_CLASS_COUNT)
            m_classCounter = 0;

        return &m_ItemInstance[tmp];
    } else {
        char tmp[128];
        rt2_sprintf(tmp, "没有找到这种类型的道具   0x%x\n", (int)id.type);
        RtCoreLog().Info(tmp);
        return NULL;
    }

    unguard;
}

SItemID CItemManager::CreateItem(RtString name, EItemColor color /* = ItemColor_White*/,
                                 BYTE level /* = 0*/, EItemBind cBind /* = IB_NotBind*/,
                                 WORD ColorAttribID1 /* = 0*/, BYTE ColorRate1 /* = 0*/,
                                 WORD ColorAttribID2 /* = 0*/, BYTE ColorRate2 /* = 0*/,
                                 DWORD TimeLimit /* = 0*/) {
    guard;
    SItemBase* pItemBase = GetItem(name);
    if (!pItemBase)
        return ItemID_CreateInvalid();
    else
        return _CreateItem(pItemBase->type, pItemBase, color, level, cBind, ColorAttribID1,
                           ColorRate1, ColorAttribID2, ColorRate2, TimeLimit);
    unguard;
}

SItemID CItemManager::CreateItem(DWORD index, EItemColor color /* = ItemColor_White*/,
                                 BYTE level /* = 0*/, EItemBind cBind /* = IB_NotBind*/,
                                 WORD ColorAttribID1 /* = 0*/, BYTE ColorRate1 /* = 0*/,
                                 WORD ColorAttribID2 /* = 0*/, BYTE ColorRate2 /* = 0*/,
                                 DWORD TimeLimit /* = 0*/) {
    guard;
    SItemBase* pItemBase = GetItemFromIndex(index);
    if (!pItemBase)
        return ItemID_CreateInvalid();
    else
        return _CreateItem(pItemBase->type, pItemBase, color, level, cBind, ColorAttribID1,
                           ColorRate1, ColorAttribID2, ColorRate2, TimeLimit);
    unguard;
}

SItemID CItemManager::CreateItem(SItemID& item) {
    guard;
    item.id = m_pIDManager->NewID();
    return item;
    unguard;
}

SItemID CItemManager::_CreateItem(DWORD type, SItemBase* pBase,
                                  EItemColor color /* = ItemColor_White */, BYTE level /* = 0*/,
                                  EItemBind cBind /* = IB_NotBind*/, WORD ColorAttribID1 /* = 0*/,
                                  BYTE ColorRate1 /* = 0*/, WORD ColorAttribID2 /* = 0*/,
                                  BYTE ColorRate2 /* = 0*/, DWORD TimeLimit /* = 0*/) {
    guard;

    if (!ItemBind_IsValid(cBind)) {
        RtCoreLog().Info("[CItemManager::_CreateItem] ItemBind value is NOT INVALID!(value=%d)\n",
                         cBind);
        return ItemID_CreateInvalid();
    }
    if (!ItemColor_IsValid(color)) {
        RtCoreLog().Info("[CItemManager::_CreateItem] ItemColor value is NOT INVALID!(value=%d)\n",
                         color);
        return ItemID_CreateInvalid();
    }

    if (cBind == IB_BindWhenEquip) {
        if (!(ItemIsWeapon(type) || ItemIsArmor(type) || ItemIsShipin(type) || ItemIsTrump(type))) {
            cBind = IB_BindWhenPick;
        }
    }

    DWORD   id = m_pIDManager->NewID();
    SItemID item;
    item.id    = id;
    item.type  = type;
    item.level = level;
    item.cBind = cBind;

    if (ItemIsWeapon(type) || ItemIsArmor(type) || ItemIsShipin(type)) {
        item.curWear = pBase->maxWear;  // 初使化耐久
    }
    if (ItemCanPile(item)) {
        item.count = pBase->initCount;
    } else {
        item.count = 0x1;
    }

    /*	if (ItemIsWeapon_MagicWeapon(type))
	{
		item.paramCount = MW_PARAM_SIZE;
		for (int i=0; i<MW_PARAM_SIZE; i++)
		{
			item.params[i] = ((SWeapon*)pBase)->MagicWeaponParams[i];
		}
	}
	else */
    if (ItemIsShipin(type)) {
        item.paramCount = MW_PARAM_SIZE;
        for (int i = 0; i < MW_PARAM_SIZE; i++) {
            item.params[i] = 0;
        }
    } else if (ItemIsPet(type)) {
        item = CreatePetItem(item, this);
        return item;
    } else if (ItemIsOther_Spawn(type)) {
        //item.paramCount = ITEMSPAWN_PARAM_SIZE;
        //item.params[ITEMSPAWN_PARAM_LIFE] = (DWORD)time(NULL) + (7*24*3600);
        item.TimeLimit = (DWORD)time(NULL) + (7 * 24 * 3600);
    } else if (ItemIsTrump(type)) {
        item.paramCount = TRUMP_SIZE;
        for (int i = 0; i < TRUMP_SKILL_1; i++) {
            item.params[i]                 = ((STrump*)pBase)->TrumpDBParams[i];
            item.params[TRUMP_ATTACH_RATE] = 100;
        }
        for (int i = TRUMP_SKILL_1; i < TRUMP_SKILL_MAX; i++) {
            item.params[i] = 0;
        }
    } else if (ItemIsSorption(type)) {
        item.paramCount = ((SSorption*)pBase)->count;
        for (int i = 0; i < item.paramCount; i++) {
            item.params[i] = ((SSorption*)pBase)->param[i];
        }
    }

    if (color != ItemColor_White) {
        InfuseColor(item, color, ColorAttribID1, ColorRate1, ColorAttribID2, ColorRate2);
    } else if (ItemIsTrump(item.type)) {
        int skillid                = 4400 + GetRand(3) * 10;
        item.params[TRUMP_SKILL_1] = skillid;
    }

    if (TimeLimit) {
        item.TimeLimit = TimeLimit;
    }

    return item;

    unguard;
}

void CItemManager::DeleteItem(SItemID& id) {}

void CItemManager::GetTableType(DWORD type, int& outTableType, int& outTableSubType) {
    outTableType    = type >> 28;
    outTableSubType = (type >> 24) & 0x0000000f;
}

float CItemManager::GetItemPoint(SItemID& item)  //heten
{
    SItemBase* pItemClass = GetItem(item.type);
    if (!pItemClass) {
        RtCoreLog().Info("Invalid Item: ID=%d\n", item.id);
        return 0;
    }
    return pItemClass->point;
}

int CItemManager::GetItemValue(SItemID& item) {
    guard;

    // 	int iIndex = GetIndexFromType(item.type);
    // 	if (iIndex >= 4496 && iIndex <= 4499)
    // 	{
    // 		return 5 * 10000;
    // 	}

    SItemBase* pItemClass = GetItem(item.type);
    if (!pItemClass) {
        RtCoreLog().Info("Invalid Item: ID=%d\n", item.id);
        return 0;
    }
    if (pItemClass->point != 0)
        return pItemClass->point * m_PointValue;

    vector<SAttribAddtion> arrAttribs;
    float                  rate1 = 0.0f;
    float                  rate2 = 0.0f;

    if (item.attribID1 != 0) {
        if (!m_pAttribMgr->GetAttrib(item.attribID1, arrAttribs)) {
            RtCoreLog().Info("Invalid item attrib!(attrib=%d)\n", item.attribRate1);
            return 0;
        }
        if (!arrAttribs.empty()) {
            rate1 = item.attribRate1 * arrAttribs[0].value / 100.0f;
        }
    }

    if (item.attribID2 != 0) {
        if (!m_pAttribMgr->GetAttrib(item.attribID2, arrAttribs)) {
            RtCoreLog().Info("Invalid item attrib!(attrib=%d)\n", item.attribID2);
            return 0;
        }
        if (!arrAttribs.empty()) {
            rate2 = item.attribRate2 * arrAttribs[0].value / 100.0f;
        }
    }

    return rtRound(pItemClass->value * (1 + rate1) * (1 + rate2));

    unguard;
}

bool CItemManager::GenerateCreateString(std::vector<SCreateItem>& arrCreateItem,
                                        std::string&              outCreateString) {
    guard;

    outCreateString = "";
    std::string        strTmp;
    static std::string strSep1 = ";";
    static std::string strSep2 = "|";

    int count = (int)arrCreateItem.size();
    if (count <= 0)
        return false;

    strTmp = "ver=";
    strTmp += rtFormatNumber(g_iCreateItemStringVersion);
    strTmp += strSep1;
    outCreateString += strTmp;

    strTmp = "count=";
    strTmp += rtFormatNumber(count);
    strTmp += strSep1;
    outCreateString += strTmp;

    outCreateString += strSep2;

    for (int i = 0; i < count; i++) {
        if (!arrCreateItem[i].SaveToString(this, strTmp)) {
            return false;
        }

        outCreateString += strTmp;
        outCreateString += strSep2;
    }

    return true;

    unguard;
}

bool CItemManager::AnalyseCreateString(std::string&              CreateString,
                                       std::vector<SCreateItem>& outArrayCreateItem) {
    guard;

    // ver=1;count=2|id=2;color=0;lev=0|id=3;color=2;lev=3|

    int  i       = 0;
    int  version = -1;
    int  count   = -1;
    char cTmp2048[2048];
    if (CreateString.size() >= 2048) {
        RtCoreLog().Info("[CItemManager::AnalyseCreateString] Clause is too long!(MaxSize=%d)\n",
                         2048);
        return false;
    }
    rt2_strncpy(cTmp2048, CreateString.c_str(), 2048);

    char sep1[] = "|";
    char sep2[] = ";";

    // 分割整个字符串
    std::string         strHeader;
    vector<std::string> arrCIs;
    char*               p = strtok(cTmp2048, sep1);
    if (!p)
        return false;
    strHeader = p;
    p         = strtok(NULL, sep1);
    while (p) {
        arrCIs.push_back(p);
        p = strtok(NULL, sep1);
    }

    // 解析头
    vector<std::string> arrHeaderTokens;
    p = strtok(cTmp2048, sep2);
    while (p) {
        arrHeaderTokens.push_back(p);
        p = strtok(NULL, sep2);
    }
    for (i = 0; i < (int)arrHeaderTokens.size(); i++) {
        std::string token;
        std::string value;
        if (!AnalyseEqualClause(arrHeaderTokens[i], token, value))
            return false;

        if (token == "ver") {
            version = atoi(value.c_str());
        } else if (token == "count") {
            count = atoi(value.c_str());
        } else {
            RtCoreLog().Info(
                "[CItemManager::AnalyseCreateString] Invalid token(ignored)!(token=%s)\n",
                token.c_str());
        }
    }

    if (version < 0) {
        RtCoreLog().Info("[CItemManager::AnalyseCreateString] Invalid version!(version=%d)\n",
                         version);
        return false;
    }
    if (count <= 0 || count != (int)arrCIs.size()) {
        RtCoreLog().Info("[CItemManager::AnalyseCreateString] Count error!(count=%d)\n", count);
        return false;
    }

    // 解析SCreateItem结构
    for (i = 0; i < (int)arrCIs.size(); i++) {
        SCreateItem ci;
        if (!ci.LoadFromString(this, arrCIs[i]))
            return false;

        outArrayCreateItem.push_back(ci);
    }

    return true;

    unguard;
}

SItemID CItemManager::CreateItemByStruct(SCreateItem& ci) {
    guard;

    DWORD TimeLimit = 0;
    if (ci.limitTime)
        TimeLimit = (DWORD)time(NULL) + ci.limitTime * 60;

    SItemID item = CreateItem(ci.index, (EItemColor)ci.color, ci.level, ci.cBind, ci.colorAttrib1,
                              ci.colorRate, ci.colorAttrib2, ci.colorRate, TimeLimit);
    return item;

    unguard;
}

bool CItemManager::InfuseColor(SItemID& item, EItemColor color, WORD attribID1 /* = 0*/,
                               BYTE ColorRate1 /* = 0*/, WORD attribID2 /* = 0*/,
                               BYTE ColorRate2 /* = 0*/) {
    guard;

    if (!ItemID_IsValid(item))
        return false;
    if (!(ItemIsWeapon(item.type) || ItemIsArmor(item.type) || ItemIsShipin(item.type) ||
          ItemIsTrump(item.type)))
        return false;
    if (item.color != ItemColor_White)
        return false;
    if (!ItemColor_IsValid(color))
        return false;
    SItemBase* pItemClass = GetItem(item.type);
    if (!pItemClass)
        return false;

    size_t i;
    int    rand;
    switch (color) {
        case ItemColor_Green: {
            if (attribID1 == 0) {
                attribID1 = m_pAttribMgr->CreateAttrib(pItemClass->grade, CAttribMgr::csGreen);
            } else {
                std::vector<WORD> possIDs;
                m_pAttribMgr->GetPossibleAttribs(pItemClass->grade, CAttribMgr::csGreen, possIDs);
                for (i = 0; i < possIDs.size(); i++) {
                    if (attribID1 == possIDs[i])
                        break;
                }
                if (i == possIDs.size())  // 传进来的属性id没有找到，不合法
                {
                    RtCoreLog().Info("[CItemManager::InfuseColor] Invalid ColorAttrib(%d)",
                                     attribID1);
                    attribID1 = 0;
                    return false;
                }
            }

            if (ColorRate1 == 0) {
                ColorRate1 = m_pAttribMgr->CreateAttribRate(CAttribMgr::csGreen);
            }

            if ((attribID1 == 0) ||
                (ColorRate1 < ATTRIB_GREEN_MIN_RATE || ColorRate1 > ATTRIB_GREEN_MAX_RATE)) {
                return false;
            } else {
                item.color       = color;
                item.attribID1   = attribID1;
                item.attribRate1 = ColorRate1;
            }
            rand = GetRand(3);
        } break;
        case ItemColor_Blue: {
            if (attribID1 == 0 && attribID2 == 0) {
                attribID1 = m_pAttribMgr->CreateAttrib(pItemClass->grade, CAttribMgr::csBlue1);
                attribID2 = m_pAttribMgr->CreateAttrib(pItemClass->grade, CAttribMgr::csBlue2);
            } else {
                std::vector<WORD> possIDs;
                if (attribID1 != 0) {
                    possIDs.clear();
                    m_pAttribMgr->GetPossibleAttribs(pItemClass->grade, CAttribMgr::csBlue1,
                                                     possIDs);
                    for (i = 0; i < possIDs.size(); i++) {
                        if (attribID1 == possIDs[i])
                            break;
                    }
                    if (i == possIDs.size())  // 传进来的属性id没有找到，不合法
                    {
                        RtCoreLog().Info("[CItemManager::InfuseColor] Invalid ColorAttrib(%d)",
                                         attribID1);
                        attribID1 = 0;
                        return false;
                    }
                }
                if (attribID2 != 0) {
                    possIDs.clear();
                    m_pAttribMgr->GetPossibleAttribs(pItemClass->grade, CAttribMgr::csBlue2,
                                                     possIDs);
                    for (i = 0; i < possIDs.size(); i++) {
                        if (attribID2 == possIDs[i])
                            break;
                    }
                    if (i == possIDs.size())  // 传进来的属性id没有找到，不合法
                    {
                        RtCoreLog().Info("[CItemManager::InfuseColor] Invalid ColorAttrib(%d)",
                                         attribID2);
                        attribID2 = 0;
                        return false;
                    }
                }
            }

            if (ColorRate1 == 0 && ColorRate2 == 0) {
                ColorRate1 = m_pAttribMgr->CreateAttribRate(CAttribMgr::csBlue1);
                ColorRate2 = m_pAttribMgr->CreateAttribRate(CAttribMgr::csBlue2);
            }

            if ((attribID1 == 0 && attribID2 == 0) ||
                ((ColorRate1 < ATTRIB_BLUE_MIN_RATE || ColorRate1 > ATTRIB_BLUE_MAX_RATE) &&
                 (ColorRate2 < ATTRIB_BLUE_MIN_RATE || ColorRate2 > ATTRIB_BLUE_MAX_RATE))) {
                return false;
            } else {
                item.color       = color;
                item.attribID1   = attribID1;
                item.attribRate1 = ColorRate1;
                item.attribID2   = attribID2;
                item.attribRate2 = ColorRate2;
            }
            rand = GetRand(6);
        } break;
        case ItemColor_Golden: {
            if (attribID1 == 0 && attribID2 == 0) {
                attribID1 = m_pAttribMgr->CreateAttrib(pItemClass->grade, CAttribMgr::csGolden1);
                attribID2 = m_pAttribMgr->CreateAttrib(pItemClass->grade, CAttribMgr::csGolden2);
            } else {
                std::vector<WORD> possIDs;
                if (attribID1 != 0) {
                    possIDs.clear();
                    m_pAttribMgr->GetPossibleAttribs(pItemClass->grade, CAttribMgr::csGolden1,
                                                     possIDs);
                    for (i = 0; i < possIDs.size(); i++) {
                        if (attribID1 == possIDs[i])
                            break;
                    }
                    if (i == possIDs.size())  // 传进来的属性id没有找到，不合法
                    {
                        RtCoreLog().Info("[CItemManager::InfuseColor] Invalid ColorAttrib(%d)",
                                         attribID1);
                        attribID1 = 0;
                        return false;
                    }
                }
                if (attribID2 != 0) {
                    possIDs.clear();
                    m_pAttribMgr->GetPossibleAttribs(pItemClass->grade, CAttribMgr::csGolden2,
                                                     possIDs);
                    for (i = 0; i < possIDs.size(); i++) {
                        if (attribID2 == possIDs[i])
                            break;
                    }
                    if (i == possIDs.size())  // 传进来的属性id没有找到，不合法
                    {
                        RtCoreLog().Info("[CItemManager::InfuseColor] Invalid ColorAttrib(%d)",
                                         attribID2);
                        attribID2 = 0;
                        return false;
                    }
                }
            }

            if (ColorRate1 == 0 && ColorRate2 == 0) {
                ColorRate1 = m_pAttribMgr->CreateAttribRate(CAttribMgr::csGolden1);
                ColorRate2 = m_pAttribMgr->CreateAttribRate(CAttribMgr::csGolden2);
            }

            if ((attribID1 == 0 && attribID2 == 0) ||
                ((ColorRate1 < ATTRIB_GOLDEN_MIN_RATE || ColorRate1 > ATTRIB_GOLDEN_MAX_RATE) &&
                 (ColorRate2 < ATTRIB_GOLDEN_MIN_RATE || ColorRate2 > ATTRIB_GOLDEN_MAX_RATE))) {
                return false;
            } else {
                item.color       = color;
                item.attribID1   = attribID1;
                item.attribRate1 = ColorRate1;
                item.attribID2   = attribID2;
                item.attribRate2 = ColorRate2;
            }
            rand = GetRand(9);
        } break;
        default:
            return false;
            break;
    }

    if (ItemIsTrump(item.type)) {
        if (rand > 4) {
            rand = GetRand(4);
        }
        int skillid                = 4400 + rand * 10;
        item.params[TRUMP_SKILL_1] = skillid;
    }

    return true;
    unguard;
}

bool CItemManager::LoadResource() {
    guard;

    SItemBase*  pItemBase = NULL;
    SWeapon*    pWeapon;
    STrump*     pTrump;
    SArmor*     pArmor;
    SShiPin*    pShipin;
    SGem*       pCharm;
    SScroll*    pScroll;
    SYaoPing*   pYaoPing;
    SOtherItem* pOther;
    SItemPet*   pPet;
    SItemSkill* pItemSkill;
    SSorption*  pSorption;

    RtCsv csv;
    if (!csv.LoadTableFile(R(RES_TABLE_ITEM)))
        return false;
    int nLineCount = csv.GetLineCount();
    //int nColumnCount = csv.GetMaxRow();
    //	csv.SetTitleLine(1);

    const char *name, *icon, *sound, *effect;
    int         index, type, subType = 0;
    int         bCanUpdate, bCanTrade, bCanDelete, bCanDrop, bMission, bCanPile, bCanNpcTrade,
        bCanNotFuse = 0;
    int         value, grade = 0;
    int         itemCredit      = 0;  // 仙誉
    int         itemContribute  = 0;  // 俗名
    int         itemPoint       = 0;  // 积分
    int         itemRecord      = 0;  // 军功
    int         itemUnionRecord = 0;  // 帮贡
    int         itemJobRecord   = 0;  // 门贡
    float       point           = 0.f;
    int         timeLimit       = 0;
    int         selectLevel     = 0;
    const char *param1, *movepos;
    int         decomposeID = 0;
    int         maxWear     = 0;  // 耐久上限

    const char sep[] = "&";  // 分隔符

    for (int i = 1; i < nLineCount; i++) {
        char* pChar;

        type  = csv[i]["类型"].Int();
        index = csv[i]["ID"].Int();
        if (index == 0 || type == 13) {
            continue;
        }

        // 确认没有相同的id
        EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = m_mapIndexToItem.find(index);
        if (iter != m_mapIndexToItem.end()) {
            RtCoreLog().Info(
                "[CItemManager::LoadResource] Same item ID found in item table!(line %d,itemid %d)",
                i, index);
            CHECK(0);
            return false;
        }

        subType         = csv[i]["子类型"].Int();
        bCanUpdate      = csv[i]["可锻造"].Int();
        bCanTrade       = csv[i]["可交易"].Int();
        bCanNpcTrade    = csv[i]["可商店"].Int();
        bCanDelete      = csv[i]["可删除"].Int();
        bCanDrop        = csv[i]["可丢弃"].Int();
        bMission        = csv[i]["是否任务道具"].Int();
        bCanPile        = csv[i]["可重叠摆放"].Int();
        bCanNotFuse     = csv[i]["不允许融合"].Int();
        name            = csv[i]["名称"].Str();
        value           = csv[i]["价值"].Int();
        itemCredit      = csv[i]["仙誉"].Int();
        itemContribute  = csv[i]["俗名"].Int();  // 俗名
        itemPoint       = csv[i]["积分"].Int();  // 积分
        itemRecord      = csv[i]["军功"].Int();  // 军功
        itemUnionRecord = csv[i]["帮贡"].Int();  // 帮贡
        itemJobRecord   = csv[i]["门贡"].Int();  // 门贡
        point           = csv[i]["点数"].Float();
        icon            = csv[i]["Icon"].Str();
        //pChar = (char *)csv[i]["Model"].Str();// 只有“双手锏”“雌雄双鞭”等道具有第2个模型
        //if ( pChar && strlen(pChar)>0 )
        //{
        //	model0 = strtok(pChar, sep);
        //	model1 = strtok(NULL, sep);
        //}
        //else
        //{
        //	model0 = model1 = NULL;
        //}
        sound       = csv[i]["Sound"].Str();
        effect      = csv[i]["特效文件"].Str();
        grade       = csv[i]["阶段"].Int();
        timeLimit   = csv[i]["使用限制"].Int();
        selectLevel = csv[i]["选择等级"].Int();
        param1      = csv[i]["参数1"].Str();
        decomposeID = csv[i]["分解ID"].Int();

        if (type == 1)  // 武器
        {
            pWeapon = RT_NEW SWeapon();
            //memset(pWeapon, 0, sizeof(SWeapon) );
            pItemBase = pWeapon;

            pWeapon->maxWear = csv[i]["耐久上限"].Int() * 100;
            pChar            = (char*)csv[i]["路径动画"].Str();
            if (pChar && pChar[0] != '\0') {
                rt2_strncpy(pWeapon->Way, pChar, 64);
            }
            pWeapon->bBothHands    = (BYTE)csv[i]["是否双手武器"].Int();
            pWeapon->nAttack       = csv[i]["命中"].Int();
            pWeapon->nAttRange     = csv[i]["攻击距离"].Int();
            pWeapon->fAttSpeed     = csv[i]["攻击速度"].Float();
            pWeapon->nCriticalRate = csv[i]["必杀率"].Int();

            pChar = (char*)csv[i]["刀光"].Str();
            if (pChar && pChar[0] != '\0') {
                rt2_strncpy(pWeapon->FlySwordRib, pChar, 64);
            }

            if (pWeapon->nCriticalRate > 100 || pWeapon->nCriticalRate < 0) {
                RtCoreLog().Info(
                    "[Item.LoadResource] A weapon's CriticalRate is wrong! line %d! Ignored.\n", i);
                pWeapon->nCriticalRate = 0;
            }
            pChar = (char*)csv[i]["伤害"].Str();
            if (pChar && strlen(pChar) > 0) {
                char* k             = strtok(pChar, sep);
                pWeapon->nDamageMin = (short)atoi(k);
                k                   = strtok(NULL, sep);
                if (k)
                    pWeapon->nDamageMax = (short)atoi(k);
                else
                    pWeapon->nDamageMax = 0;
            }
            pWeapon->nArmor = csv[i]["防御"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len     = strlen(pChar);
                pWeapon->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pWeapon->pDesc, pChar, len + 1);
            }

            pWeapon->suitID = csv[i]["套装ID"].Int();

            //if (model0)
            //	rt2_strncpy(pWeapon->Act1, model0, PATH_STRING_LEN);
            //if (model1)
            //	rt2_strncpy(pWeapon->Act2, model1, PATH_STRING_LEN);

            pChar = (char*)csv[i]["UpdateModel"].Str();
            if (pChar && pChar[0] != '\0') {
                std::string updateModel(pChar);   // 将 C 字符串转为 C++ 的 std::string
                std::vector<std::string> levels;  // 存储以 ";" 分割的各级别字符串

                // 按 ";" 分割字符串
                size_t start = 0, end;
                while ((end = updateModel.find(';', start)) != std::string::npos) {
                    if (end > start) {
                        levels.emplace_back(updateModel.substr(start, end - start));
                    }
                    start = end + 1;
                }
                if (start < updateModel.size()) {
                    levels.emplace_back(updateModel.substr(start));
                }

                // 遍历每个级别，解析编号和路径
                for (size_t i = 0; i < levels.size() && i <= ITEM_MAX_ITEM_LEVEL; ++i) {
                    std::vector<std::string> parts;  // 存储以 "&" 分割的模型信息
                    std::stringstream        ss(levels[i]);
                    std::string              part;

                    // 按 "&" 分割
                    while (std::getline(ss, part, '&')) {
                        parts.push_back(part);
                    }

                    // 处理模型编号和路径
                    if (parts.size() == 3) {
                        pWeapon->Models1Num[i] = std::stoi(parts[0]);
                        rt2_strncpy(pWeapon->Models1[i], parts[1].c_str(), PATH_STRING_LEN);
                        rt2_strncpy(pWeapon->Models2[i], parts[2].c_str(), PATH_STRING_LEN);
                    } else {
                        pWeapon->Models1Num[i] = 1;
                        rt2_strncpy(pWeapon->Models1[i], parts[0].c_str(), PATH_STRING_LEN);
                        rt2_strncpy(pWeapon->Models2[i], parts[0].c_str(), PATH_STRING_LEN);
                    }
                }
            }

        } else if (type == 2)  // 防具
        {
            pArmor = RT_NEW SArmor();
            //memset(pArmor, 0, sizeof(SArmor) );
            pItemBase = pArmor;

            pArmor->maxWear    = csv[i]["耐久上限"].Int() * 100;
            pArmor->nArmor     = csv[i]["防御"].Int();
            pArmor->nDodge     = csv[i]["闪躲"].Int();
            pArmor->nAttack    = csv[i]["命中"].Int();
            pArmor->fAttSpeed  = csv[i]["攻击速度"].Float();
            pArmor->nMoveSpeed = csv[i]["移动速度"].Int();
            pArmor->nMaxHP     = csv[i]["MaxHp"].Int();

            pChar = (char*)csv[i]["对应Skin"].Str();
            if (pChar && strlen(pChar) > 0) {
                rt2_strncpy(pArmor->Skin, pChar, 64);
            }

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len    = strlen(pChar);
                pArmor->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pArmor->pDesc, pChar, len + 1);
            } else
                pArmor->pDesc = NULL;

            pArmor->suitID = csv[i]["套装ID"].Int();

            pChar = (char*)csv[i]["UpdateModel"].Str();
            if (pChar && pChar[0] != '\0') {
                //int counter = 0;
                //const char sep1[] = ";";
                //char* k = strtok(pChar, sep1);

                char       arrTmp[ITEM_MAX_ITEM_LEVEL + 1][PATH_STRING_LEN];
                int        counter = 0;
                const char sep1[]  = ";";
                const char sep2[]  = "&";
                char*      k       = strtok(pChar, sep1);

                while (k && counter <= ITEM_MAX_ITEM_LEVEL) {
                    rt2_strncpy(arrTmp[counter], k, PATH_STRING_LEN - 1);
                    arrTmp[counter][PATH_STRING_LEN - 1] = 0;
                    counter++;
                    k = strtok(NULL, sep1);
                    //rt2_strncpy(pArmor->UpdateSkins[counter], k, PATH_STRING_LEN-1);
                    //               pArmor->UpdateSkins[counter][PATH_STRING_LEN-1];
                    //counter ++;
                    //k = strtok(NULL, sep1);
                }

                //while (k && counter<=ITEM_MAX_ITEM_LEVEL)
                //{
                //	rt2_strncpy(arrTmp[counter], k, PATH_STRING_LEN-1);
                //	arrTmp[counter][PATH_STRING_LEN-1] = 0;
                //	counter ++;
                //	k = strtok(NULL, sep1);
                //}

                for (int i = 0; i < counter; i++) {
                    char* cTmp1;
                    cTmp1 = strtok(arrTmp[i], sep2);
                    if (cTmp1)
                        rt2_strncpy(pArmor->UpdateSkins[i], cTmp1, PATH_STRING_LEN);  // 女模型
                    cTmp1 = strtok(NULL, sep2);
                    if (cTmp1)
                        rt2_strncpy(pArmor->UpdateSkins2[i], cTmp1, PATH_STRING_LEN);  // 男模型
                }
            }

            pChar = (char*)csv[i]["UpdateEffect"].Str();
            for (int i = 0; i <= ITEM_MAX_ITEM_LEVEL; i++) {
                pArmor->UpdateEffects[i][0] = '\0';
            }
            if (pChar && pChar[0] != '\0') {
                int        counter = 0;
                const char sep1[]  = ";";
                char*      k       = strtok(pChar, sep1);
                while (k && counter <= ITEM_MAX_ITEM_LEVEL - ITEM_WEAPON_EFFECT_FROM_LEVEL) {
                    rt2_strncpy(pArmor->UpdateEffects[ITEM_WEAPON_EFFECT_FROM_LEVEL + counter], k,
                                PATH_STRING_LEN - 1);
                    pArmor->UpdateEffects[ITEM_WEAPON_EFFECT_FROM_LEVEL + counter]
                                         [PATH_STRING_LEN - 1] = 0;
                    counter++;
                    k = strtok(NULL, sep1);
                }
            }
        } else if (type == 3)  // 饰品
        {
            pShipin = RT_NEW SShiPin();
            //memset(pShipin, 0, sizeof(SShiPin) );
            pItemBase = pShipin;

            pShipin->maxWear        = csv[i]["耐久上限"].Int() * 100;
            pShipin->nFireDefence   = csv[i]["抗火"].Int();
            pShipin->nWaterDefence  = csv[i]["抗水"].Int();
            pShipin->nPoisonDefence = csv[i]["抗毒"].Int();
            pShipin->nMaxHP         = csv[i]["MaxHp"].Int();
            pShipin->nMaxMP         = csv[i]["MaxMp"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len     = strlen(pChar);
                pShipin->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pShipin->pDesc, pChar, len + 1);
            }

            pShipin->suitID = csv[i]["套装ID"].Int();
        } else if (type == 4)  // 药瓶
        {
            pYaoPing = RT_NEW SYaoPing();
            //memset(pYaoPing, 0, sizeof(SYaoPing) );
            pItemBase = pYaoPing;

            pYaoPing->nHP         = csv[i]["Hp增加"].Int();
            pYaoPing->nMP         = csv[i]["Mp增加"].Int();
            pYaoPing->nHPRate     = csv[i]["Hp百分比增加"].Int();
            pYaoPing->nMPRate     = csv[i]["Mp百分比增加"].Int();
            pYaoPing->nExp        = csv[i]["Exp增加"].Int();
            pYaoPing->nEnp        = csv[i]["Enp增加"].Int();
            pYaoPing->nEnpRate    = csv[i]["Enp百分比增加"].Int();
            pYaoPing->nCardMinute = csv[i]["打卡时间增加"].Int();
            pYaoPing->nTime       = csv[i]["持续时间"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len      = strlen(pChar);
                pYaoPing->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pYaoPing->pDesc, pChar, len + 1);
            }
        } else if (type == 5)  // 护符
        {
            pCharm = RT_NEW SGem();
            //memset(pCharm, 0, sizeof(SGem) );
            pItemBase          = pCharm;
            pCharm->fRate      = atoi(param1) * 1.0f / 100.0f;
            pCharm->lFuse      = csv[i]["宝石合成"].Int();
            pCharm->cFuseRate  = csv[i]["宝石合成成功率"].Int();
            pCharm->lFuseMoney = csv[i]["宝石合成金钱"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len    = strlen(pChar);
                pCharm->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pCharm->pDesc, pChar, len + 1);
            }
        } else if (type == 6)  // 卷轴
        {
            pScroll = RT_NEW SScroll();
            //memset(pScroll, 0, sizeof(SScroll) );
            pItemBase = pScroll;

            if (subType == 1)  // 回城卷轴
            {
                movepos = csv[i]["移动坐标"].Str();
                char* x = strtok(const_cast<char*>(movepos), ";");
                if (x != 0) {
                    pScroll->MovePosX = atoi(x);
                    pScroll->MovePosY = atoi(strtok(NULL, ";"));
                }
            } else if (subType == 2)  // 配方
            {
                pScroll->composeID = atoi(param1);
            } else if (subType == 3)  // 附魔卷轴
            {
                pScroll->enchantAttribID = atoi(param1);
            }
            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len     = strlen(pChar);
                pScroll->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pScroll->pDesc, pChar, len + 1);
            }
        } else if (type == 7 || type == 10)  // 其他（基本上是任务道具）
        {
            pOther = RT_NEW SOtherItem();
            //memset(pOther, 0, sizeof(SOtherItem) );
            pItemBase = pOther;

            pOther->taskID = csv[i]["任务ID"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len    = strlen(pChar);
                pOther->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pOther->pDesc, pChar, len + 1);
            }
        } else if (type == 8)  // 神兽
        {
            pPet = RT_NEW SItemPet();
            //memset(pPet, 0, sizeof(SItemPet) );
            pItemBase = pPet;

            pPet->petID      = csv[i]["神兽ID"].Int();
            pPet->NeedCulLev = csv[i]["修为等级需求"].Int();

            // 神兽初始化值
            pChar           = (char*)csv[i]["神兽属性"].Str();
            int pet_counter = 0;
            if (pChar && pChar[0] != 0) {
                char* k = strtok(pChar, sep);
                for (; pet_counter < ITEM_MAX_PARAM_COUNT; pet_counter++) {
                    if (!k)
                        break;

                    pPet->PetParams[pet_counter] = atoi(k);
                    k                            = strtok(NULL, sep);
                }
            }
            for (; pet_counter < ITEM_MAX_PARAM_COUNT; pet_counter++)
                pPet->PetParams[pet_counter] = 0;

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len  = strlen(pChar);
                pPet->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pPet->pDesc, pChar, len + 1);
            }
        } else if (type == 9)  // 技能令牌
        {
            pItemSkill = RT_NEW SItemSkill();
            //memset(pItemSkill, 0, sizeof(SItemSkill) );
            pItemBase = pItemSkill;

            pItemSkill->skillID = csv[i]["技能ID"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len        = strlen(pChar);
                pItemSkill->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pItemSkill->pDesc, pChar, len + 1);
            }
        } else if (type == 11) {
            pTrump = RT_NEW STrump();

            pItemBase = pTrump;

            pChar = (char*)csv[i]["路径动画"].Str();
            if (pChar && pChar[0] != '\0') {
                rt2_strncpy(pTrump->Way, pChar, 64);
            }
            pTrump->nAttack = csv[i]["命中"].Int();
            pChar           = (char*)csv[i]["伤害"].Str();
            if (pChar && strlen(pChar) > 0) {
                char* k            = strtok(pChar, sep);
                pTrump->nDamageMin = (short)atoi(k);
                k                  = strtok(NULL, sep);
                if (k)
                    pTrump->nDamageMax = (short)atoi(k);
                else
                    pTrump->nDamageMax = 0;
            }
            pTrump->nArmor = csv[i]["防御"].Int();

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len    = strlen(pChar);
                pTrump->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pTrump->pDesc, pChar, len + 1);
            }

            pTrump->suitID = csv[i]["套装ID"].Int();

            pChar = (char*)csv[i]["UpdateModel"].Str();
            if (pChar && pChar[0] != '\0') {
                char       arrTmp[ITEM_MAX_ITEM_LEVEL + 1][PATH_STRING_LEN];
                int        counter = 0;
                const char sep1[]  = ";";
                const char sep2[]  = "&";
                char*      k       = strtok(pChar, sep1);
                while (k && counter <= ITEM_MAX_ITEM_LEVEL) {
                    rt2_strncpy(arrTmp[counter], k, PATH_STRING_LEN - 1);
                    rt2_strncpy(pTrump->Models1[counter], k, PATH_STRING_LEN);
                    arrTmp[counter][PATH_STRING_LEN - 1] = 0;
                    counter++;
                    k = strtok(NULL, sep1);
                }
            }
        } else if (type == 14)  // 吸附类型道具
        {
            pSorption = RT_NEW SSorption();

            pItemBase = pSorption;

            pChar = (char*)csv[i]["说明"].Str();
            if (pChar && strlen(pChar) > 0) {
                size_t len       = strlen(pChar);
                pSorption->pDesc = RT_NEW char[len + 1];
                rt2_strncpy(pSorption->pDesc, pChar, len + 1);
            }

            pChar         = (char*)csv[i]["参数1"].Str();
            char* k       = strtok(pChar, ",");
            int   counter = 0;
            if (k && strlen(k) > 0) {
                while (k && counter < MAX_SORPTION_NUM) {
                    pSorption->param[counter] = atoi(k);
                    counter++;
                    k = strtok(NULL, ",");
                }
            }
            pSorption->count = counter;
        }

        // 道具需求
        pItemBase->requirement[ItemRequirement_Level]        = csv[i]["等级需求"].Int();
        pItemBase->requirement[ItemRequirement_Strength]     = csv[i]["力量需求"].Int();
        pItemBase->requirement[ItemRequirement_Agility]      = csv[i]["敏捷需求"].Int();
        pItemBase->requirement[ItemRequirement_Intelligence] = csv[i]["智力需求"].Int();
        pItemBase->requirement[ItemRequirement_Hit]          = csv[i]["精准需求"].Int();
        pItemBase->requirement[ItemRequirement_Grade]        = csv[i]["阶段需求"].Int();
        pItemBase->requirement[ItemRequirement_MaxLevel]     = csv[i]["最大等级"].Int();
        pItemBase->requirement[ItemRequirement_Con]          = csv[i]["体质需求"].Int();

        if (pItemBase->requirement[ItemRequirement_MaxLevel] == 0)
            pItemBase->requirement[ItemRequirement_MaxLevel] = 150;
        pChar = (char*)csv[i]["职业需求"].Str();
        if (strlen(pChar) == 0)
            pItemBase->requirement[ItemRequirement_Career] = 0x0f;
        else {
            char* k = strtok(pChar, sep);
            while (k) {
                int n = atoi(k);
                if (n > CAREER_REQUIREMENT_COUNT)
                    return false;
                pItemBase->requirement[ItemRequirement_Career] |= (1 << (n - 1));
                k = strtok(NULL, sep);
            }
        }

        pItemBase->type = type << 28;
        pItemBase->type |= subType << 24;
        pItemBase->type |= index << 8;
        if (bCanUpdate)
            pItemBase->type |= ITEM_MASK_CAN_UPDATE;
        if (bCanTrade)
            pItemBase->type |= ITEM_MASK_CAN_TRADE;
        if (bCanNpcTrade)
            pItemBase->type |= ITEM_MASK_CAN_NPCTRADE;
        if (bCanDelete)
            pItemBase->type |= ITEM_MASK_CAN_DELETE;
        if (bCanDrop)
            pItemBase->type |= ITEM_MASK_CAN_DROP;
        if (bMission)
            pItemBase->type |= ITEM_MASK_MISSION_ITEM;
        if (bCanPile)
            pItemBase->type |= ITEM_MASK_CAN_PILE;
        pItemBase->bCanFuse = bCanNotFuse == 1 ? false : true;
        rt2_strncpy(pItemBase->name, name, PATH_STRING_LEN - 1);
        pItemBase->name[PATH_STRING_LEN - 1] = 0;
        pItemBase->value                     = value;
        pItemBase->itemCredit                = itemCredit;
        pItemBase->itemContribute            = itemContribute;
        pItemBase->itemPoint                 = itemPoint;
        pItemBase->itemRecord                = itemRecord;
        pItemBase->itemUnionRecord           = itemUnionRecord;
        pItemBase->itemJobRecord             = itemJobRecord;
        pItemBase->point                     = point;
        rt2_strncpy(pItemBase->icon, icon, PATH_STRING_LEN - 1);
        pItemBase->icon[PATH_STRING_LEN - 1] = 0;
        //lyymark 物体盒子模型
        rt2_strncpy(pItemBase->szModel, "iii001.act", PATH_STRING_LEN - 1);
        pItemBase->szModel[PATH_STRING_LEN - 1] = 0;
        rt2_strncpy(pItemBase->szSound, sound, PATH_STRING_LEN - 1);
        pItemBase->szSound[PATH_STRING_LEN - 1] = 0;
        pItemBase->grade                        = grade;
        pItemBase->initCount                    = csv[i]["初始使用次数"].Int();
        pItemBase->maxCount                     = csv[i]["最大使用次数"].Int();
        CHECK(pItemBase->maxCount >= pItemBase->initCount);
        pItemBase->timeLimit = (EItemUseLimit)timeLimit;
        rt2_strncpy(pItemBase->effect, effect, PATH_STRING_LEN - 1);
        pItemBase->effect[PATH_STRING_LEN - 1] = 0;
        pItemBase->selectLevel                 = selectLevel;
        pItemBase->param1                      = param1;
        pItemBase->decomposeID                 = decomposeID;
        pItemBase->bFBDrow                     = 1 == csv[i]["是否副本掉落"].Int();
        pItemBase->bStore                      = 1 == csv[i]["是否可存仓"].Int();

        // 合法性检查
        if (ItemCanPile(pItemBase->type)) {
            if (pItemBase->initCount <= 0 || pItemBase->maxCount <= 0) {
                RtCoreLog().Info("[CItemManager::LoadResource] Pile Item 初始使用次数/最大使用次数 "
                                 "Error! (Line %d)\n",
                                 i);
                return false;
            }
        }
        m_mapItems[pItemBase->type] = pItemBase;
        m_mapIndexToItem[index]     = pItemBase;
    }

    return true;

    unguard;
}

CBag::CBag(int nLineCount, int nColumnCount, int nPageCount, CItemManager* pItemMgr)
    : m_nLineCount(nLineCount), m_nColumnCount(nColumnCount), m_nPageCount(nPageCount),
      m_pIOnItemChange(NULL) {
    guard;

    if (nLineCount > MAX_BAG_LINE_COUNT || nColumnCount > MAX_BAG_COLUMN_COUNT ||
        nPageCount > MAX_BAG_PAGE_COUNT)
        CHECKEX("CBag容器的容量超过最大范围");

    m_pItemMgr = pItemMgr;
    Clear();

    unguard;
}

CBag::~CBag() {
    guard;
    unguard;
}

const char* CBag::SaveToStringbyID(CItemManager* pItemMgr, DWORD ID) {
    guard;
    g_strItemBufferBD[0] = '\0';
    //char CTemp128[128];
    //CTemp128 = '\0';

    //// 版本
    //rt2_sprintf(cTmp128, "#%04x", g_iItemBagVersion);
    //rt2_strcat(g_strItemBufferBD, cTmp128);

    SItemID id = GetItemFromID(ID);
    if (ItemID_IsValid(id)) {
        const char* pItemString = id.SaveToString(pItemMgr);
        rt2_strcat(g_strItemBufferBD, pItemString);
    }

    CHECK(strlen(g_strItemBufferBD) < 256);
    return g_strItemBufferBD;
}

const char* CBag::SaveToString(CItemManager* pItemMgr) {
    guard;

    g_strItemBuffer10240[0] = '\0';
    char cTmp128[128];
    cTmp128[0] = '\0';

    // 版本
    rt2_sprintf(cTmp128, "#%04x", g_iItemBagVersion);
    rt2_strcat(g_strItemBuffer10240, cTmp128);

    bool isHaveHuiYuanItem = FindType(4401);
    int  nItemCountTemp    = m_nItemCount;
    if (isHaveHuiYuanItem)
        nItemCountTemp--;
    //	VERSION 8
    // 道具栏
    int nItemCounter = m_nItemCount;
    rt2_sprintf(cTmp128, ",%d,%d,%d,%d,", nItemCountTemp, m_nPageCount, m_nLineCount,
                m_nColumnCount);
    rt2_strcat(g_strItemBuffer10240, cTmp128);
    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                if (nItemCounter == 0)
                    goto lbSaveMoney;
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    int index = m_pItemMgr->GetIndexFromType(id.type);
                    //tim.yang  会员道具
                    if (index == 4401) {
                        continue;
                    }
                    //end
                    nItemCounter--;
                    rt2_sprintf(cTmp128, "%d,%d,%d,", page, i, j);
                    rt2_strcat(g_strItemBuffer10240, cTmp128);
                    const char* pItemString = id.SaveToString(pItemMgr);
                    rt2_strcat(g_strItemBuffer10240, pItemString);
                }
            }
    // 金币
lbSaveMoney:
    rt2_sprintf(cTmp128, "%d,", (int)m_nMoney);
    rt2_strcat(g_strItemBuffer10240, cTmp128);

    CHECK(strlen(g_strItemBuffer10240) < 10240);
    return g_strItemBuffer10240;

    unguard;
}

//SItemID CBag::LoadFromStringbyBD(const char* str, CItemManager *pItemMgr)
//{
//	guard;
//
//	int strLength = (int)strlen(str);
//	if (strLength==0)
//		return 0;
//	char *strCopy = RT_NEW char[strLength+1];//(char*)malloc(strLength+1);
//	rt2_strcpy(strCopy, str);
//
//	char *pReadPtr = strCopy;
//
//}

int CBag::LoadFromString(const char* str, CItemManager* pItemMgr) {
    guard;

    Clear();

    // do NOT use "str", because strtok will destroy "str", so copy the std::string
    int strLength = (int)strlen(str);
    if (strLength == 0)
        return 0;
    char* strCopy = RT_NEW char[strLength + 1];  //(char*)malloc(strLength+1);
    rt2_strncpy(strCopy, str, strLength + 1);

    char* pReadPtr  = strCopy;
    int   version   = 0;
    int   itemIndex = 0;
    int   iTmp      = 0;

    // 版本
    sscanf(pReadPtr, "#%04x", &version);
    pReadPtr += 5;

    if (version == 6) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        nColCount  = atoi(k);
        k          = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            page = atoi(k);
            k    = strtok(NULL, sep);
            i    = atoi(k);
            k    = strtok(NULL, sep);
            j    = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0)
                _SetItem(page, i, j, id);

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else if (version == 7) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        if (!k)
            return 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nColCount = atoi(k);
        k         = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            if (!k)
                return 0;
            page = atoi(k);
            k    = strtok(NULL, sep);
            if (!k)
                return 0;
            i = atoi(k);
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            j = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0)
                _SetItem(page, i, j, id);

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else if (version == 8) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        if (!k)
            return 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nColCount = atoi(k);
        k         = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            if (!k)
                return 0;
            page = atoi(k);
            k    = strtok(NULL, sep);
            if (!k)
                return 0;
            i = atoi(k);
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            j = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0) {
                if (!_IsValidIndex(page, i, j))
                    return 0;
                _SetItem(page, i, j, id);
            }

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else if (version == 9) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        if (!k)
            return 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nColCount = atoi(k);
        k         = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            if (!k)
                return 0;
            page = atoi(k);
            k    = strtok(NULL, sep);
            if (!k)
                return 0;
            i = atoi(k);
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            j = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0) {
                if (!_IsValidIndex(page, i, j))
                    return 0;
                _SetItem(page, i, j, id);
            }

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else if (version == 10) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        if (!k)
            return 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nColCount = atoi(k);
        k         = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            if (!k)
                return 0;
            page = atoi(k);
            k    = strtok(NULL, sep);
            if (!k)
                return 0;
            i = atoi(k);
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            j = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0) {
                if (!_IsValidIndex(page, i, j))
                    return 0;
                _SetItem(page, i, j, id);
            }

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else if (version == 11) {
        char sep[] = ",";
        // 道具栏
        char* k          = strtok(pReadPtr, sep);
        int   nItemCount = 0, nPageCount = 0, nLineCount = 0, nColCount = 0;
        if (!k)
            return 0;
        nItemCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nPageCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nLineCount = atoi(k);
        k          = strtok(NULL, sep);
        if (!k)
            return 0;
        nColCount = atoi(k);
        k         = strtok(NULL, sep);
        if (nPageCount != m_nPageCount || nLineCount != m_nLineCount ||
            nColCount != m_nColumnCount) {
            if (!Resize(nPageCount, nLineCount, nColCount)) {
                RtCoreLog().Info("ItemContainer's page, line, column is NOT right!(%d,%d,%d)\n",
                                 nPageCount, nLineCount, nColCount);
                return 0;
            }
        }
        int page, i, j;
        for (int counter = 0; counter < nItemCount; counter++) {
            SItemID id;
            if (!k)
                return 0;
            page = atoi(k);
            k    = strtok(NULL, sep);
            if (!k)
                return 0;
            i = atoi(k);
            k = strtok(NULL, sep);
            if (!k)
                return 0;
            j = atoi(k);

            pReadPtr = k + strlen(k) + 1;
            iTmp     = id.LoadFromString(version, pReadPtr, pItemMgr);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID Error!\n");
                return 0;
            }
            pReadPtr += iTmp;
            if (id.id != 0) {
                if (!_IsValidIndex(page, i, j))
                    return 0;
                _SetItem(page, i, j, id);
            }

            k = strtok(pReadPtr, sep);
        }
        //		m_nMoney = atoi(k);
        int   tmp   = (int)strlen(k);
        char* pTemp = strCopy;
        DEL_ARRAY(strCopy);
        return abs((int)(k - pTemp) + tmp + 1);
    } else {
        //CHECKEX("Load item ERROR: invalid item data\n");
        DEL_ARRAY(strCopy);
        return 0;
    }

    return 0;

    unguard;
}

bool CBag::Serialize(char type, CG_CmdPacket* cmd) {
    guard;

    long tmp;

    if (cmd->IsWritting()) {
        switch (type) {
            case NET_UPDATE_ALL:
            default: {
                tmp = (GetPageCount() << 16) | (GetLineCount() << 8) | GetColCount();
                *cmd << tmp;

                for (int page = 0; page < GetPageCount(); page++)
                    for (int i = 0; i < GetLineCount(); i++)
                        for (int j = 0; j < GetColCount(); j++) {
                            SItemID id = GetItem(page, i, j);
                            if (ItemID_IsValid(id)) {
                                tmp = (page << 16) | (i << 8) | j;
                                *cmd << tmp;
                                id.Serialize(cmd);
                            }
                        }
                tmp = 0xffffffff;
                *cmd << tmp;
                *cmd << m_nMoney;
            } break;
        }
    } else {
        int     counterSafe = 0;
        int     page, i, j;
        long    tmp;
        SItemID id;
        Clear();

        *cmd << tmp;
        page = tmp >> 16;
        i    = tmp >> 8 & 0x000000ff;
        j    = tmp & 0x000000ff;
        if (m_nPageCount != page || m_nLineCount != i || m_nColumnCount != j) {
            if (!Resize(page, i, j)) {
                return false;
            }
        }

        *cmd << tmp;
        while (tmp != 0xffffffff && counterSafe < 200) {
            page = tmp >> 16;
            i    = tmp >> 8 & 0x000000ff;
            j    = tmp & 0x000000ff;
            if (!_IsValidIndex(page, i, j))
                return false;
            BOOL_ENSURE(id.Serialize(cmd))
            _SetItem(page, i, j, id);

            *cmd << tmp;

            counterSafe++;
        }

        if (counterSafe >= 200) {
            RtCoreLog().Info("Invalid Data in CBag::Serialize!\n");
            Clear();
            return false;
        }

        *cmd << m_nMoney;
    }
    if (cmd->IsError()) {
        RtCoreLog().Info("[CBag::Serialize] Net Data Error!\n");
        return false;
    } else
        return true;

    unguard;
}

bool CBag::Resize(int page, int line, int col) {
    if (page > MAX_BAG_PAGE_COUNT || line > MAX_BAG_LINE_COUNT || col > MAX_BAG_COLUMN_COUNT) {
        RtCoreLog().Info("[CBag::Resize] page=%d,line=%d,col=%d\n", page, line, col);
        return false;
    }

    if (page >= m_nPageCount && line >= m_nLineCount && col >= m_nColumnCount) {
        m_nPageCount   = page;
        m_nLineCount   = line;
        m_nColumnCount = col;
        return true;
    } else {
        m_nPageCount   = page;
        m_nLineCount   = line;
        m_nColumnCount = col;
        return true;
    }
}

void CBag::Clear() {
    guard;
    m_nPoint       = 0;  // 重置积分为0
    m_nMoney       = 0;  // 重置金钱数量为0
    m_nItemCount   = 0;  // 重置物品数量为0
    m_nCurrentPage = 0;  // 重置当前页面为0
    for (int p = 0; p < MAX_BAG_PAGE_COUNT; p++)
        for (int i = 0; i < MAX_BAG_LINE_COUNT; i++)
            for (int j = 0; j < MAX_BAG_COLUMN_COUNT; j++) {
                m_nItemID[p][i][j].Clear();  // 清空当前格子的物品ID

                m_AssistantAttrib[p][i][j] = 0x00 | (1 << IAA_Visible);  //辅助属性 可见
            }

    unguard;
}

int CBag::GetVisibleMaxLine() const {
    guard;

    if (m_nItemCount == 0)
        return 0;
    int lineCount = 0;
    for (int page = m_nPageCount - 1; page >= 0; page--)
        for (int i = m_nLineCount - 1; i >= 0; i--) {
            for (int j = m_nColumnCount - 1; j >= 0; j--) {
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    return m_nLineCount * m_nPageCount - lineCount;
                }
            }
            lineCount++;
        }
    return -1;

    unguard;
}

bool CBag::Find(SItemID itemID, int* pOutPage /*= NULL*/, int* pOutI /*= NULL*/,
                int* pOutJ /*= NULL*/) const {
    guard;
    return FindID(itemID.id, pOutPage, pOutI, pOutJ);
    unguard;
}

bool CBag::FindID(DWORD ID, int* pOutPage /*= NULL*/, int* pOutI /*= NULL*/,
                  int* pOutJ /*= NULL*/) const {
    guard;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (id.id == ID) {
                    if (pOutPage && pOutI && pOutJ) {
                        *pOutPage = p;
                        *pOutI    = i;
                        *pOutJ    = j;
                    }
                    return true;
                }
            }
    return false;
    unguard;
}

SItemID CBag::GetItemFromID(DWORD ID) const {
    guard;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (id.id == ID) {
                    return id;
                }
            }
    return ItemID_CreateInvalid();
    unguard;
}

SItemID* CBag::GetItemPointFromID(DWORD ID) {
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (id.id == ID) {
                    return &m_nItemID[p][i][j];
                }
            }
    return NULL;
}

bool CBag::Find(DWORD ID, int* pOutPage /*= NULL*/, int* pOutI /*= NULL*/,
                int* pOutJ /*= NULL*/) const {
    guard;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (id.id == ID) {
                    if (pOutPage && pOutI && pOutJ) {
                        *pOutPage = p;
                        *pOutI    = i;
                        *pOutJ    = j;
                    }
                    return true;
                }
            }
    return false;
    unguard;
}

bool CBag::FindType(DWORD typeID, EItemColor color /* = ItemColor_All*/, int* pOutCount /* = NULL*/,
                    int* pOutPage /* = NULL*/, int* pOutI /* = NULL*/,
                    int* pOutJ /* = NULL*/) const {
    guard;
    int  count  = 0;
    bool bFirst = true;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (CItemManager::GetIndexFromType(id.type) == typeID) {
                    if (id.color == color || color == ItemColor_All) {
                        count += id.count;
                        if (bFirst) {
                            bFirst = false;
                            if (pOutPage && pOutI && pOutJ) {
                                *pOutPage = p;
                                *pOutI    = i;
                                *pOutJ    = j;
                            }
                        }
                        //if ( ItemIsMissionItem(id) )
                        //{
                        //	if (pOutCount)			*pOutCount = id.count;
                        //	if (bFirst)
                        //	{
                        //		bFirst = false;
                        //		if (pOutPage && pOutI && pOutJ)
                        //		{
                        //			*pOutPage = p;
                        //			*pOutI = i;
                        //			*pOutJ = j;
                        //		}
                        //	}
                        //	return true;
                        //}
                        //else
                        //{
                        //	count += id.count;
                        //	if (bFirst)
                        //	{
                        //		bFirst = false;
                        //		if (pOutPage && pOutI && pOutJ)
                        //		{
                        //			*pOutPage = p;
                        //			*pOutI = i;
                        //			*pOutJ = j;
                        //		}
                        //	}
                        //}
                    }
                }
            }

    if (pOutCount)
        *pOutCount = count;

    if (count == 0)
        return false;
    else
        return true;
    unguard;
}

bool CBag::CheckIsHuiyuan(DWORD typeID) {
    guard;
    int  count  = 0;
    bool bFirst = true;
    for (int p = 0; p < m_nPageCount; p++) {
        for (int i = 0; i < m_nLineCount; i++) {
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (m_pItemMgr) {
                    int type = 0, subtype = 0;
                    m_pItemMgr->GetTableType(id.type, type, subtype);
                    if (10 == type) {
                        SOtherItem* pItem = m_pItemMgr->GetItem(id)->pOtherItem;
                        if (!pItem) {
                            continue;
                        }
                        if (!strcmp(pItem->param1.c_str(), "1958")) {
                            DWORD now = time(NULL);
                            if (now > id.TimeLimit) {
                                continue;
                            } else {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
    unguard;
}

bool CBag::FindTypeWithParams(DWORD typeID, std::vector<DWORD>& paramsIndices,
                              std::vector<DWORD>& paramsCheck,
                              EItemColor color /* = ItemColor_All*/, int* pOutCount /* = NULL*/,
                              int* pOutPage /* = NULL*/, int* pOutI /* = NULL*/,
                              int* pOutJ /* = NULL*/) const {
    guard;

    int cc = 0;
    if (pOutCount)
        *pOutCount = 0;

    // 参数合法性检查
    if (paramsIndices.size() != paramsCheck.size()) {
        RtCoreLog().Info(
            "[CBag::FindTypeWithParams] Params Indices Count != Params Check Count!\n");
        return false;
    }
    int nCheckParamsCount = (int)paramsIndices.size();
    for (cc = 0; cc < nCheckParamsCount; cc++) {
        if (paramsIndices[cc] < 0 || paramsIndices[cc] >= ITEM_MAX_PARAM_COUNT) {
            RtCoreLog().Info("[CBag::FindTypeWithParams] Params Indices Error!\n");
            return false;
        }
    }

    int  count  = 0;
    bool bFirst = true;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID item = _GetItem(p, i, j);
                if (CItemManager::GetIndexFromType(item.type) == typeID) {
                    if (item.color == color || color == ItemColor_All) {
                        bool bPassCheckParams = true;
                        for (cc = 0; cc < nCheckParamsCount; cc++) {
                            if (item.params[cc] != paramsCheck[paramsIndices[cc]]) {
                                bPassCheckParams = false;
                                break;
                            }
                        }
                        if (bPassCheckParams) {
                            count += item.count;
                            if (bFirst) {
                                bFirst = false;
                                if (pOutPage && pOutI && pOutJ) {
                                    *pOutPage = p;
                                    *pOutI    = i;
                                    *pOutJ    = j;
                                }
                            }
                            //if ( ItemIsMissionItem(item) )
                            //{
                            //	if (pOutCount)			*pOutCount = item.count;
                            //	if (bFirst)
                            //	{
                            //		bFirst = false;
                            //		if (pOutPage && pOutI && pOutJ)
                            //		{
                            //			*pOutPage = p;
                            //			*pOutI = i;
                            //			*pOutJ = j;
                            //		}
                            //	}
                            //	return true;
                            //}
                            //else
                            //{
                            //	count += item.count;
                            //}
                        }  // check params
                    }  // check color
                }  // check type
            }

    if (pOutCount)
        *pOutCount = count;

    if (count == 0)
        return false;
    else
        return true;

    unguard;
}

// 把所有物品打包成数组
void CBag::ToItemsArray(vector<SItemID>& arrItems) const {
    guard;

    arrItems.clear();

    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (ItemID_IsValid(id))
                    arrItems.push_back(id);
            }

    unguard;
}

// 把所有物品打包成数组
void CBag::ToItemIDArray(vector<DWORD>& arrItemID) const {
    guard;

    arrItemID.clear();

    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID item = _GetItem(p, i, j);
                if (ItemID_IsValid(item))
                    arrItemID.push_back(item.id);
            }

    unguard;
}

bool CBag::MoveToBag(CBag* pToBag, DWORD typeMask /*=0xffffffff*/) {
    guard;

    int tmpPage, tmpI, tmpJ;
    BOOL_ENSURE(pToBag)
    int counter = m_nItemCount;
    if (counter == 0)
        return true;
    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id) && ((id.type & typeMask) != 0x0)) {
                    BOOL_ENSURE(pToBag->AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    RemoveItem(page, i, j);
                }
                if (ItemID_IsValid(id))
                    counter--;
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CBag::MoveToBag] bag struct error\n");
    //	CHECK(0);
    return false;

    unguard;
}

bool CBag::MoveToContainer(CItemContainerBase* pCon, DWORD typeMask /*=0xffffffff*/) {
    guard;

    int tmpPage, tmpI, tmpJ;
    BOOL_ENSURE(pCon)
    int counter = m_nItemCount;
    if (counter == 0)
        return true;
    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id) && ((id.type & typeMask) != 0x0)) {
                    if (ItemIsMissionItem(id))
                        BOOL_ENSURE(pCon->m_MissionBag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    else if (ItemIsPet(id))
                        BOOL_ENSURE(pCon->m_PetBag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    else
                        BOOL_ENSURE(pCon->m_Bag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    RemoveItem(page, i, j);
                }
                if (ItemID_IsValid(id))
                    counter--;
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CBag::MoveToContainer] bag struct error\n");
    //	CHECK(0);
    return false;

    unguard;
}

bool CBag::AddToBag(CBag* pToBag, DWORD typeMask /*=0xffffffff*/) const {
    guard;

    int tmpPage, tmpI, tmpJ;
    BOOL_ENSURE(pToBag)
    int counter = m_nItemCount;
    if (counter == 0)
        return true;
    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    if ((id.type & typeMask) != 0x0) {
                        BOOL_ENSURE(pToBag->AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    }
                    counter--;
                }
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CBag::AddToBag] bag struct error\n");
    //	CHECK(0);
    return false;

    unguard;
}

bool CBag::AddToContainer(CItemContainerBase* pCon, DWORD typeMask /*=0xffffffff*/) const {
    guard;

    int tmpPage, tmpI, tmpJ;
    BOOL_ENSURE(pCon)
    int counter = m_nItemCount;
    if (counter == 0)
        return true;
    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                SItemID id = GetItem(page, i, j);
                if (ItemID_IsValid(id) && ((id.type & typeMask) != 0x0)) {
                    if (ItemIsMissionItem(id))
                        BOOL_ENSURE(pCon->m_MissionBag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    else if (ItemIsPet(id))
                        BOOL_ENSURE(pCon->m_PetBag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    else
                        BOOL_ENSURE(pCon->m_Bag.AddItem(id, &tmpPage, &tmpI, &tmpJ, false))
                    //					counter--;
                }
                if (ItemID_IsValid(id))
                    counter--;
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CBag::AddToContainer] bag struct error\n");
    //	CHECK(0);
    return false;

    unguard;
}

bool CBag::RemoveFromBag(CBag* pBag) {
    guard;

    int p1, i1, j1;
    int pageCount = pBag->GetPageCount();
    int lineCount = pBag->GetLineCount();
    int colCount  = pBag->GetColCount();
    BOOL_ENSURE(pBag)
    int counter = pBag->Count();
    for (int page = 0; page < pageCount; page++)
        for (int i = 0; i < lineCount; i++)
            for (int j = 0; j < colCount; j++) {
                SItemID id = pBag->GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    if (Find(id, &p1, &i1, &j1)) {
                        _RemoveItem(p1, i1, j1);
                    }
                    counter--;
                }
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CBag::RemoveFromBag] bag struct error\n");
    //	CHECK(0);
    return false;

    unguard;
}

// array中的所有物品，如果在对象中存在，就把它删除掉
bool CBag::RemoveFromArray(vector<SItemID>& arrItems) {
    guard;

    int arrSize = (int)arrItems.size();
    if (arrSize == 0)
        return true;

    int p1, i1, j1;
    for (int i = 0; i < arrSize; i++) {
        SItemID id = arrItems[i];
        if (Find(id, &p1, &i1, &j1)) {
            _RemoveItem(p1, i1, j1);
        }
    }
    return true;

    unguard;
}

// array中的所有物品，如果在对象中存在，就把它删除掉
bool CBag::RemoveFromArray(vector<DWORD>& arrItemID) {
    guard;

    int arrSize = (int)arrItemID.size();
    if (arrSize == 0)
        return true;

    int p1, i1, j1;
    for (int i = 0; i < arrSize; i++) {
        DWORD id = arrItemID[i];
        if (Find(id, &p1, &i1, &j1)) {
            _RemoveItem(p1, i1, j1);
        }
    }
    return true;

    unguard;
}

// 本容器是否包含pBag中的所有物品
bool CBag::IsContain(CBag* pBag) const {
    guard;

    BOOL_ENSURE(pBag)
    for (int page = 0; page < pBag->GetPageCount(); page++)
        for (int i = 0; i < pBag->GetLineCount(); i++)
            for (int j = 0; j < pBag->GetColCount(); j++) {
                SItemID id = pBag->GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    if (!Find(id)) {
                        return false;
                    }
                }
            }
    return true;

    unguard;
}

// 本容器是否包含arrItemID中的所有物品
bool CBag::IsContain(vector<DWORD>& arrItemID) const {
    guard;

    if (arrItemID.empty())
        return true;

    vector<DWORD>::iterator iter = arrItemID.begin();
    for (; iter != arrItemID.end(); iter++) {
        DWORD id = (*iter);
        if (!Find(id))
            return false;
    }
    return true;

    unguard;
}

// 本容器是否包含arrItems中的所有物品
bool CBag::IsContain(vector<SItemID>& arrItems) const {
    guard;

    if (arrItems.empty())
        return true;

    vector<SItemID>::iterator iter = arrItems.begin();
    for (; iter != arrItems.end(); iter++) {
        SItemID item = (*iter);
        if (!Find(item))
            return false;
    }
    return true;

    unguard;
}

SItemID CBag::GetItem(int i, int j) const {
    guard;
    return GetItem(m_nCurrentPage, i, j);
    unguard;
}

int CBag::GetItemCount(int i, int j) const {
    guard;
    return GetItemCount(m_nCurrentPage, i, j);
    unguard;
}

bool CBag::RemoveItem(DWORD typeID, EItemColor color /* = ItemColor_All*/, int count /*= -1*/) {
    guard;

    // CHECK
    // 删除数量不大于存在数量
    int itemCount;
    if (!FindType(typeID, color, &itemCount))
        return false;
    if (itemCount < count)
        return false;

    if (count < 0)  // 删除所有
        count = itemCount;

    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID item = _GetItem(p, i, j);
                if (CItemManager::GetIndexFromType(item.type) == typeID) {
                    if (color == item.color || color == ItemColor_All) {
                        if (item.count > count) {
                            item.count -= count;
                            _SetItem(p, i, j, item);
                            return true;
                        } else if (item.count == count) {
                            _RemoveItem(p, i, j);
                            return true;
                        } else {
                            _RemoveItem(p, i, j);
                            count -= item.count;
                        }
                        //if (ItemIsMissionItem(item))
                        //{
                        //	if (item.count < count)
                        //	{
                        //		RtCoreLog().Info("[CBag::RemoveItem] item.count < count\n");
                        //		return false;
                        //	}
                        //	item.count -= count;
                        //	if (item.count==0)
                        //	{
                        //		_RemoveItem(p, i, j);
                        //		return true;
                        //	}
                        //	else
                        //	{
                        //		_SetItem(p, i, j, item);
                        //	}
                        //	return true;
                        //}
                        //else
                        //{
                        //	_RemoveItem(p, i, j);
                        //	count--;
                        //}

                        if (count == 0)
                            return true;
                    }
                }
            }
    return false;

    unguard;
}

bool CBag::RemoveItemWithParams(DWORD typeID, std::vector<DWORD>& paramsIndices,
                                std::vector<DWORD>& paramsCheck,
                                EItemColor color /* = ItemColor_All*/, int count /* = -1*/) {
    guard;

    int cc = 0;
    // 参数合法性检查
    if (paramsIndices.size() != paramsCheck.size()) {
        RtCoreLog().Info(
            "[CBag::RemoveItemWithParams] Params Indices Count != Params Check Count!\n");
        return false;
    }
    int nCheckParamsCount = (int)paramsIndices.size();
    for (cc = 0; cc < nCheckParamsCount; cc++) {
        if (paramsIndices[cc] < 0 || paramsIndices[cc] >= ITEM_MAX_PARAM_COUNT) {
            RtCoreLog().Info("[CBag::RemoveItemWithParams] Params Indices Error!\n");
            return false;
        }
    }

    // CHECK
    // 删除数量不大于存在数量
    int itemCount;
    if (!FindTypeWithParams(typeID, paramsIndices, paramsCheck, color, &itemCount))
        return false;
    if (itemCount < count)
        return false;

    if (count < 0)  // 删除所有
        count = itemCount;

    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID item = _GetItem(p, i, j);
                if (CItemManager::GetIndexFromType(item.type) == typeID) {
                    if (color == item.color || color == ItemColor_All) {
                        bool bPassCheckParams = true;
                        for (cc = 0; cc < nCheckParamsCount; cc++) {
                            if (item.params[cc] != paramsCheck[paramsIndices[cc]]) {
                                bPassCheckParams = false;
                                break;
                            }
                        }
                        if (bPassCheckParams) {
                            if (item.count > count) {
                                item.count -= count;
                                _SetItem(p, i, j, item);
                                return true;
                            } else if (item.count == count) {
                                _RemoveItem(p, i, j);
                                return true;
                            } else {
                                _RemoveItem(p, i, j);
                                count -= item.count;
                            }
                            //if (ItemIsMissionItem(item))
                            //{
                            //	if (item.count < count)
                            //	{
                            //		RtCoreLog().Info("[CBag::RemoveItemWithParams] item.count < count\n");
                            //		return false;
                            //	}
                            //	item.count -= count;
                            //	if (item.count==0)
                            //	{
                            //		_RemoveItem(p, i, j);
                            //		return true;
                            //	}
                            //	else
                            //	{
                            //		_SetItem(p, i, j, item);
                            //	}
                            //	return true;
                            //}
                            //else
                            //{
                            //	_RemoveItem(p, i, j);
                            //	count--;
                            //}

                            if (count == 0)
                                return true;
                        }  // params
                    }  // color
                }  // type
            }
    return false;

    unguard;
}

bool CBag::RemoveItem(int i, int j) {
    guard;
    return RemoveItem(m_nCurrentPage, i, j);
    unguard;
}

bool CBag::MoveItem(int fromI, int fromJ, int toI, int toJ) {
    guard;
    return MoveItem(this, m_nCurrentPage, fromI, fromJ, this, m_nCurrentPage, toI, toJ);
    unguard;
}

bool CBag::MoveItem(CBag* fromContainer, int fromI, int fromJ, CBag* toContainer, int toI,
                    int toJ) {
    guard;
    return MoveItem(fromContainer, m_nCurrentPage, fromI, fromJ, toContainer, m_nCurrentPage, toI,
                    toJ);
    unguard;
}

bool CBag::ResolveItem(int fromPage, int fromI, int fromJ, int toPage, int toI, int toJ, int Num) {
    guard;
    return ResolveItem(this, fromPage, fromI, fromJ, this, toPage, toI, toJ, Num);
    unguard;
}

// 把一个道具添加到此容器中,放到第一个适合的格子中,如果容器已满或者非法id,则返回false
bool CBag::AddItem(SItemID& id, int* pOutPage /* = NULL*/, int* pOutI /* = NULL*/,
                   int* pOutJ /* = NULL*/, bool bPileIfPossible /* = true*/) {
    guard;

    if (!ItemID_IsValid(id))
        return false;
    if (IsFull())
        return false;

    if (ItemCanPile(id) && bPileIfPossible)
        return AddPileItem(id, pOutPage, pOutI, pOutJ);
    else {
        for (int p = 0; p < m_nPageCount; p++)
            for (int i = 0; i < m_nLineCount; i++)
                for (int j = 0; j < m_nColumnCount; j++) {
                    if (!ItemID_IsValid(_GetItem(p, i, j))) {
                        if (pOutPage && pOutI && pOutJ) {
                            *pOutPage = p;
                            *pOutI    = i;
                            *pOutJ    = j;
                        }
                        _SetItem(p, i, j, id);
                        return true;
                    }
                }

        return false;
    }

    unguard;
}

//Tianh 查找包里可以叠加的同类型的物品  然后相堆叠  直到新添加的物品被分配完毕
bool CBag::BuyTidy(SItemID& item) {

    DWORD Type = item.type;

    SItemBase* pItemClass = m_pItemMgr->GetItem(Type);
    if (!pItemClass) {
        return false;
    }

    for (int page = 0; page < m_nPageCount; page++) {
        for (int i = 0; i < m_nLineCount; i++) {
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = GetItem(page, i, j);

                if (id.type == Type) {
                    if (ItemID_IsValid(id)) {
                        if (id.count < pItemClass->maxCount) {
                            if (item.count < (pItemClass->maxCount - id.count)) {
                                id.count   = id.count + item.count;
                                item.count = 0;
                                _SetItem(page, i, j, id);
                                return true;
                            } else if (item.count == (pItemClass->maxCount - id.count)) {
                                id.count   = pItemClass->maxCount;
                                item.count = 0;
                                _SetItem(page, i, j, id);
                                return true;
                            } else if (item.count > (pItemClass->maxCount - id.count)) {
                                item.count = item.count - (pItemClass->maxCount - id.count);
                                id.count   = pItemClass->maxCount;
                                _SetItem(page, i, j, id);
                            }
                        }
                    }
                }
            }
        }
    }

    if (item.count > 0) {
        AddItem(item);
        return true;
    }
    return false;
}

bool CBag::AddPileItem(SItemID& item, int* pOutPage /* = NULL*/, int* pOutI /* = NULL*/,
                       int* pOutJ /* = NULL*/) {
    guard;

    // 查找是否可以合并
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID id = _GetItem(p, i, j);
                if (ItemCanPile(id) && id.type == item.type &&
                    id.count + item.count <= MAX_PILE_COUNT) {
                    // 如果超过一个格子的数量就不添加
                    SItemBase* pItemClass = m_pItemMgr->GetItem(id.type);
                    if (!pItemClass)
                        return false;
                    if ((pItemClass->maxCount != 0) &&
                        (id.count + item.count > pItemClass->maxCount))
                        continue;
                    // 检查参数相同才添加
                    bool bRes1 = false;
                    if (id.paramCount == item.paramCount) {
                        int tmp1 = 0;
                        for (tmp1 = 0; tmp1 < id.paramCount; tmp1++) {
                            if (id.params[tmp1] != item.params[tmp1]) {
                                break;
                            }
                        }
                        if (tmp1 == id.paramCount)
                            bRes1 = true;
                    }

                    if (bRes1) {
                        if (pOutPage && pOutI && pOutJ) {
                            *pOutPage = p;
                            *pOutI    = i;
                            *pOutJ    = j;
                        }
                        id.count += item.count;
                        _SetItem(p, i, j, id);
                        return true;
                    }
                }
            }

    // 没有找到可以合并的，添加到新格子中
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                if (!ItemID_IsValid(_GetItem(p, i, j))) {
                    if (pOutPage && pOutI && pOutJ) {
                        *pOutPage = p;
                        *pOutI    = i;
                        *pOutJ    = j;
                    }
                    _SetItem(p, i, j, item);
                    return true;
                }
            }

    return false;

    unguard;
}

/////////////////////////////////////////////////////
//add by Tianh   10.03.14    实现物品拆分
/////////////////////////////////////////////////////
bool CBag::ResolveItem(CBag* fromContainer, int fromPage, int fromI, int fromJ, CBag* toContainer,
                       int toPage, int toI, int toJ, int Num /*分离数量*/) {
    guard;
    SItemID itemFrom = fromContainer->GetItem(fromPage, fromI, fromJ);
    SItemID itemTo   = toContainer->GetItem(toPage, toI, toJ);
    SItemID item;

    bool Split    = true;
    bool Splitone = true;
    int  OldGoal  = 0;

    //如果目的拆分格里有其他不同类型物品  则取消拆分
    if (itemTo.id != 0 && (itemTo.type != itemFrom.type)) {
        return false;
    }
    //当目的格子里没有物品  则直接移动  不需要MOVE叠加
    if (itemTo.id == 0) {
        Split = false;
    }
    // 源物件非法
    if (!ItemID_IsValid(itemFrom)) {
        return false;
    }
    //保存未生成新物品前的目的格子的信息
    //必须格子里有物品   并切是同类型可以叠加的
    if (itemTo.id != 0 && (itemTo.type == itemFrom.type) && ItemCanPile(itemTo) &&
        ItemCanPile(itemFrom) && (itemFrom.cBind == itemTo.cBind)) {
        OldGoal  = itemTo.count;
        Splitone = false;
    }

    int iItemCount   = 0;  //源物品数量
    int iItemRdCount = 0;  //源物品剩余数量

    iItemCount   = fromContainer->GetItemCount(fromPage, fromI, fromJ);
    iItemRdCount = iItemCount - Num;

    if (iItemRdCount <= 0) {
        Num = itemFrom.count;
    }
    SItemBase* pItemClass = m_pItemMgr->GetItem(itemTo.type);
    if (pItemClass) {
        //如果超过了物品最大叠加值
        if (Num + OldGoal > pItemClass->maxCount) {
            iItemRdCount = (Num + OldGoal) - (pItemClass->maxCount);
            Num          = pItemClass->maxCount;
            Splitone     = true;
        }
    }
    //生成新的物品到指定格子
    SItem* pfromT = m_pItemMgr->GetItem(itemFrom);
    if (!pfromT) {
        return false;
    }
    item       = m_pItemMgr->CreateItem(pfromT->pItemBase->name);
    item.cBind = itemFrom.cBind;
    if (Splitone) {
        item.count = Num;
    } else {
        item.count = Num + OldGoal;
    }
    toContainer->SetItem(toPage, toI, toJ, item);

    if (iItemRdCount <= 0) {
        fromContainer->RemoveItem(fromPage, fromI, fromJ);
    } else {
        itemFrom.count = iItemRdCount;
        fromContainer->SetItem(fromPage, fromI, fromJ, itemFrom);
    }

    //SItemID Str = itemTo;
    //toContainer->RemoveItem(ret); //删除这件装备
    //toContainer->AddItem(Str);
    //UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, fromPage, fromI,fromJ);
    //UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, toPage, toI,toJ);

    return true;

    unguard;
}

SItemID CBag::GetItem(int page, int i, int j) const {
    guard;

    SItemID item;
    if (!_IsValidIndex(page, i, j)) {
        ItemID_SetInvalid(item);
        return item;
    } else {
        return _GetItem(page, i, j);
    }

    unguard;
}

int CBag::GetItemCount(int page, int i, int j) const {
    guard;

    SItemID item = m_nItemID[page][i][j];
    if (!ItemID_IsValid(item))
        return 0;
    if (!ItemCanPile(item))
        return 1;

    return (int)item.count;

    unguard;
}

bool CBag::RemoveItem(int page, int i, int j) {
    guard;

    if (!_IsValidIndex(page, i, j))
        return false;
    if (!ItemID_IsValid(_GetItem(page, i, j)))
        return false;
    _RemoveItem(page, i, j);
    return true;

    unguard;
}

bool CBag::RemoveItem(const SItemID& item) {
    guard;

    int page, i, j;
    if (!Find(item, &page, &i, &j))
        return false;
    return RemoveItem(page, i, j);

    unguard;
}

bool CBag::MoveItem(int fromPage, int fromI, int fromJ, int toPage, int toI, int toJ) {
    guard;
    return MoveItem(this, fromPage, fromI, fromJ, this, toPage, toI, toJ);
    unguard;
}

///////////////////////////////////////////////
//add by Tianh   完善移动物品  逻辑重写
/////////////////////////////////////////////
bool CBag::MoveItem(CBag* fromContainer, int fromPage, int fromI, int fromJ, CBag* toContainer,
                    int toPage, int toI, int toJ) {
    guard;

    SItemID itemFrom = fromContainer->GetItem(fromPage, fromI, fromJ);
    SItemID itemTo   = toContainer->GetItem(toPage, toI, toJ);
    if (!ItemID_IsValid(itemFrom))
        return false;  // 源物件非法

    //if ( !ItemID_IsValid(itemTo) )	// 目标格子没有物品，移动
    //{
    //	if ( !fromContainer->RemoveItem(fromPage, fromI, fromJ) )
    //		return false;
    //	if ( !toContainer->SetItem(toPage, toI, toJ, itemFrom) )
    //		return false;
    //}
    //else							// 目标格子有物品，交换
    //{
    //	if ( !toContainer->SetItem(toPage, toI, toJ, itemFrom) )
    //		return false;
    //	if ( !fromContainer->SetItem(fromPage, fromI, fromJ, itemTo) )
    //		return false;
    //}
    if ((fromI == toI) && (fromJ == toJ) && (fromPage == toPage)) {
        return false;
    }

    if (!ItemID_IsValid(itemTo))  // 目标格子没有物品，移动
    {
        if (!fromContainer->RemoveItem(fromPage, fromI, fromJ))
            return false;
        if (!toContainer->SetItem(toPage, toI, toJ, itemFrom))
            return false;
    } else  // 目标格子有物品，交换
    {
        if ((itemFrom.type == itemTo.type) && (itemFrom.cBind == itemTo.cBind)) {
            SItemID id = _GetItem(toPage, toI, toJ);
            if (ItemCanPile(itemFrom) && ItemCanPile(id)) {
                if (id.count + itemFrom.count <= MAX_PILE_COUNT) {
                    // 如果超过一个格子的数量就不添加
                    SItemBase* pItemClass = m_pItemMgr->GetItem(id.type);
                    if (!pItemClass)
                        return false;
                    //if ( (pItemClass->maxCount!=0) && (id.count+itemFrom.count > pItemClass->maxCount) )
                    //{
                    //
                    //	//// 没有找到可以合并的，添加到新格子中
                    //	//for (int p=0; p<m_nPageCount; p++)
                    //	//	for (int i=0; i<m_nLineCount; i++)
                    //	//		for (int j=0; j<m_nColumnCount; j++)
                    //	//		{
                    //	//			if ( !ItemID_IsValid(_GetItem(p, i, j) ) )
                    //	//			{
                    //	//				if ( pOutPage && pOutI && pOutJ)
                    //	//				{
                    //	//					*pOutPage = p;	*pOutI = i;	*pOutJ = j;
                    //	//				}
                    //	//				_SetItem(p, i, j, item);
                    //	//				return true;
                    //	//			}
                    //	//		}
                    //	//return false;
                    //}

                    // 检查参数相同才添加
                    bool bRes1 = false;
                    if (id.paramCount == itemFrom.paramCount) {
                        int tmp1 = 0;
                        for (tmp1 = 0; tmp1 < id.paramCount; tmp1++) {
                            if (id.params[tmp1] != itemFrom.params[tmp1]) {
                                break;
                            }
                        }
                        if (tmp1 == id.paramCount)
                            bRes1 = true;
                    }
                    if (bRes1) {
                        if ((pItemClass->maxCount != 0) &&
                            (id.count + itemFrom.count > pItemClass->maxCount)) {
                            //逻辑顺序不可改变
                            itemFrom.count = (id.count + itemFrom.count) - (pItemClass->maxCount);
                            id.count       = pItemClass->maxCount;
                            fromContainer->SetItem(fromPage, fromI, fromJ, itemFrom);
                            toContainer->SetItem(toPage, toI, toJ, id);
                        } else {
                            id.count += itemFrom.count;
                            fromContainer->RemoveItem(fromPage, fromI, fromJ);
                            toContainer->SetItem(toPage, toI, toJ, id);
                            return true;
                        }
                    }
                }
            }
        } else {
            if (!toContainer->SetItem(toPage, toI, toJ, itemFrom))
                return false;
            if (!fromContainer->SetItem(fromPage, fromI, fromJ, itemTo))
                return false;
        }
    }
    return true;

    unguard;
}

bool CBag::SetItem(int page, int i, int j, const SItemID& id) {
    guard;

    if (!_IsValidIndex(page, i, j))
        return false;

    _SetItem(page, i, j, id);
    return true;

    unguard;
}

//bool CBag::SetItem(int page,int i, int j, int Num)
//{
//	guard;
//
//	if ( !_IsValidIndex(page, i,j) )
//		return false;
//
//	_SetItem(page, i, j, Num);
//	return true;
//
//	unguard;
//}

//bool CBag::AddItem(int page, int i, int j, const SItemID& id)
//{
//	guard;
//
//	if ( !_IsValidIndex(page, i,j) )	return false;
//	if ( !ItemID_IsValid(id) )	return false;
//
//	SItemID itemOri = _GetItem(page, i, j);
//	if ( !ItemID_IsValid(itemOri) || !ItemCanPile(id.type) )
//		_SetItem(page, i, j, id);
//	else
//	{
//		int count1 = id.count;
//		int count2 = itemOri.count;
//		if (count1 + count2 > MAX_PILE_COUNT)
//			return false;
//		itemOri.count += count1;
//		_SetItem(page, i, j, itemOri);
//	}
//	return true;
//
//	unguard;
//}

bool CBag::SetItem(int i, int j, SItemID id) {
    guard;
    return SetItem(m_nCurrentPage, i, j, id);
    unguard;
}

int NumBytype = 0;

long CBag::GetNumBytype(DWORD type, WORD cBind) {
    guard;
    NumBytype = 0;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID vItemID;
                vItemID = _GetItem(p, i, j);
                if (ItemID_IsValid(vItemID)) {
                    if (vItemID.type == type && vItemID.cBind == cBind) {
                        NumBytype += vItemID.count;
                    }
                }
            }

    return NumBytype;

    unguard;
}

void CBag::SetSell(DWORD type, WORD cBind, long num) {
    guard;
    long Num  = num;
    long Num1 = num;
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID vItemID;
                vItemID = _GetItem(p, i, j);
                if (ItemID_IsValid(vItemID)) {
                    if (vItemID.type == type && vItemID.cBind == cBind) {
                        Num = Num - vItemID.count;
                        if (Num == 0) {
                            RemoveItem(vItemID);
                            return;
                        } else if (Num < 0) {
                            vItemID.count -= Num1;
                            SetItem(p, i, j, vItemID);
                            return;
                        } else if (Num > 0) {
                            RemoveItem(vItemID);
                        }
                    }
                }
            }
    unguard;
}

//bool CBag::SetItem(int i, int j, int Num);
//{
//	guard;
//	return SetItem(m_nCurrentPage,i, j,Num);
//	unguard;
//}

//bool CBag::AddItem(int i, int j, SItemID id)
//{
//	guard;
//	return AddItem(m_nCurrentPage, i, j, id);
//	unguard;
//}

void CBag::SetMoney(int count) {
    guard;

    if (!MoneyIsValid(count)) {
        RtCoreLog().Info("[CBag::SetMoney] Invalid money count(%d)! Set to 0.\n", count);
        return;
    }
    m_nMoney = count;

    unguard;
}

//heten
bool CBag::AddPoint(int count) {
    guard;

    if (count > MAX_POINT_COUNT || count < 0) {
        RtCoreLog().Info("[CBag::AddPoint] Invalid Point count!(%d)\n", count);
        return false;
    }

    m_nPoint += count;

    if (m_nPoint < 0) {
        m_nPoint = 0;
        RtCoreLog().Info("Player's Point < 0(%d)\n", m_nPoint);
        return false;
    }
    if (m_nPoint > MAX_POINT_COUNT) {
        m_nPoint = MAX_POINT_COUNT;
        RtCoreLog().Info("Player's Point reach max!(%d)\n", m_nPoint);
        return false;
    }

    return true;

    unguard;
}

//end

bool CBag::AddMoney(int count) {
    guard;

    if (count > MAX_MONEY_COUNT || count < 0) {
        RtCoreLog().Info("[CBag::AddMoney] Invalid money count!(%d)\n", count);
        return false;
    }

    m_nMoney += count;

    if (m_nMoney < 0) {
        m_nMoney = 0;
        RtCoreLog().Info("Player's Money < 0(%d)\n", m_nMoney);
        return false;
    }
    if (m_nMoney > MAX_MONEY_COUNT) {
        m_nMoney = MAX_MONEY_COUNT;
        RtCoreLog().Info("Player's money reach max!(%d)\n", m_nMoney);
        return false;
    }

    return true;

    unguard;
}

bool CBag::RemoveMoney(int count) {
    guard;

    if (count > MAX_MONEY_COUNT || count < 0) {
        RtCoreLog().Info("[CBag::AddMoney] Invalid money count!\n");
        return false;
    }

    m_nMoney -= count;

    if (m_nMoney < 0) {
        m_nMoney = 0;
        RtCoreLog().Info("Player's Money < 0\n");
        return false;
    }
    if (m_nMoney > MAX_MONEY_COUNT) {
        m_nMoney = MAX_MONEY_COUNT;
        RtCoreLog().Info("Player's money reach max!\n");
        return false;
    }

    return true;

    unguard;
}

//heten
bool CBag::RemovePoint(int count) {
    guard;

    if (count > MAX_POINT_COUNT || count < 0) {
        RtCoreLog().Info("[CBag::AddPoint] Invalid Point count!\n");
        return false;
    }

    m_nPoint -= count;

    if (m_nPoint < 0) {
        m_nPoint = 0;
        RtCoreLog().Info("Player's Point < 0\n");
        return false;
    }
    if (m_nPoint > MAX_POINT_COUNT) {
        m_nPoint = MAX_POINT_COUNT;
        RtCoreLog().Info("Player's Point reach max!\n");
        return false;
    }

    return true;

    unguard;
}

//end

void CBag::SetAllVisible(bool bVisible) {
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SetVisible(p, i, j, bVisible);
            }
}

void CBag::SetAllLock(bool bLock) {
    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SetLock(p, i, j, bLock);
            }
}

void CBag::Pinch() {
    guard;

    if (m_nItemCount == 0)
        return;

    int p1 = 0, i1 = 0, j1 = 0;
    int counter = m_nItemCount;

    for (int p = 0; p < m_nPageCount; p++)
        for (int i = 0; i < m_nLineCount; i++)
            for (int j = 0; j < m_nColumnCount; j++) {
                SItemID item = _GetItem(p, i, j);
                if (ItemID_IsValid(item)) {
                    if (j1 != j || i1 != i || p1 != p) {
                        MoveItem(p, i, j, p1, i1, j1);
                    }

                    j1++;
                    if (j1 == m_nColumnCount) {
                        i1++;
                        j1 = 0;
                        if (i1 == m_nLineCount) {
                            p1++;
                            i1 = 0;
                        }
                    }

                    counter--;
                    if (counter == 0)
                        return;
                }
            }

    RtCoreLog().Info("[CBag::Pinch] bag struct error\n");
    //	CHECK("CBag::Pinch Error\n");

    unguard;
}

bool CBag::CloneFrom(CBag* pBag) {
    if (!pBag || pBag->GetPageCount() != m_nPageCount || pBag->GetLineCount() != m_nLineCount ||
        pBag->GetColCount() != m_nColumnCount)
        return false;

    Clear();

    m_nCurrentPage = pBag->m_nCurrentPage;
    m_nItemCount   = pBag->m_nItemCount;
    m_nMoney       = pBag->m_nMoney;
    m_pItemMgr     = m_pItemMgr;

    for (int page = 0; page < GetPageCount(); page++)
        for (int i = 0; i < GetLineCount(); i++)
            for (int j = 0; j < GetColCount(); j++) {
                m_nItemID[page][i][j] = pBag->m_nItemID[page][i][j];
            }

    return true;
}

bool operator==(const CBag& bag1, const CBag& bag2) {
    guard;

    int     pageCount   = bag1.GetPageCount();
    int     lineCount   = bag1.GetLineCount();
    int     columnCount = bag1.GetColCount();
    SItemID item1, item2;

    if (pageCount != bag2.GetPageCount() || lineCount != bag2.GetLineCount() ||
        columnCount != bag2.GetColCount())
        return false;

    if (bag1.GetMoney() != bag2.GetMoney())
        return false;

    if (bag1.Count() != bag2.Count())
        return false;

    for (int page = 0; page < pageCount; page++)
        for (int i = 0; i < lineCount; i++)
            for (int j = 0; j < columnCount; j++) {
                item1 = bag1.GetItem(page, i, j);
                item2 = bag2.GetItem(page, i, j);
                if (item1 != item2)
                    return false;
            }

    return true;

    unguard;
}

bool operator!=(const CBag& bag1, const CBag& bag2) {
    guard;
    return !(bag1 == bag2);
    unguard;
}

CItemContainerBase::CItemContainerBase(CItemManager* pManager, SCreature* pCre)
    : m_Bag(ITEM_PLAYER_INVENTORY_LINE, ITEM_PLAYER_INVENTORY_COLUMN, ITEM_PLAYER_INVENTORY_PAGE,
            pManager),
      m_MissionBag(ITEM_PLAYER_INVENTORY_LINE, ITEM_PLAYER_INVENTORY_COLUMN,
                   ITEM_PLAYER_MISSION_PAGE, pManager),
      m_PetBag(ITEM_PET_BAG_LINE, ITEM_PET_BAG_COLUMN, ITEM_PET_BAG_PAGE, pManager) {
    guard;
    m_pCre         = pCre;
    m_pItemManager = pManager;
    for (int i = 0; i < MAX_EQUIP_PARTS; i++) {
        m_bEquipValid[i] = true;
    }
    Clear();
    unguard;
}

void CItemContainerBase::Clear() {
    guard;

    m_pSuit     = NULL;
    m_pLastSuit = NULL;
    m_Bag.Clear();
    m_MissionBag.Clear();
    m_PetBag.Clear();
    for (int part = 0; part < CItemContainerBase::MAX_EQUIP_PARTS; part++) {
        m_nItemID[part].Clear();
    }

    unguard;
}

CItemContainerBase::~CItemContainerBase() {
    guard;
    unguard;
}

const char* CItemContainerBase::SavePetBagToString() {
    guard;

    g_strItemBuffer20480[0] = '\0';
    char cTmp128[128];
    cTmp128[0] = '\0';

    // 版本
    rt2_sprintf(cTmp128, "#%04x", g_iPetContainerVersion);
    rt2_strcat(g_strItemBuffer20480, cTmp128);

    //	BEGIN OF VERSION 11
    // 宠物道具背包
    char* pPetBagString = (char*)m_PetBag.SaveToString(m_pItemManager);
    rt2_strcat(g_strItemBuffer20480, pPetBagString);

    CHECK(strlen(g_strItemBuffer20480) < 20480);
    return g_strItemBuffer20480;

    unguard;
}

int CItemContainerBase::LoadPetBagFromString(char* str) {
    guard;

    // do NOT use "str", because strtok will destroy "str", so copy the std::string
    int strLength = (int)strlen(str);
    if (strLength == 0)
        return 0;
    char* strCopy = RT_NEW char[strLength + 1];  //(char*)malloc(strLength+1);
    rt2_strncpy(strCopy, str, strLength + 1);

    int   iTmp     = 0;
    char* pReadPtr = strCopy;
    int   version;

    // 版本
    int ret = sscanf(pReadPtr, "#%04x", &version);
    pReadPtr += 5;

    if (version == 1) {
        // 宠物背包
        iTmp = m_PetBag.LoadFromString(pReadPtr, m_pItemManager);
        if (iTmp == 0) {
            RtCoreLog().Info("Load pet item data error!\n");
            return 0;
        }
        pReadPtr += iTmp;
    } else {
        DEL_ARRAY(strCopy);
        return 0;
    }

    char* pTemp = strCopy;
    DEL_ARRAY(strCopy);
    return abs(pReadPtr - pTemp);

    unguard;
}

const char* CItemContainerBase::SaveToString() {
    guard;

    g_strItemBuffer20480[0] = '\0';
    char cTmp128[128];
    cTmp128[0] = '\0';

    // 版本
    rt2_sprintf(cTmp128, "#%04x", g_iItemContainerVersion);
    rt2_strcat(g_strItemBuffer20480, cTmp128);

    //	BEGIN OF VERSION 10
    // 普通道具背包
    char* pBagString = (char*)m_Bag.SaveToString(m_pItemManager);
    rt2_strcat(g_strItemBuffer20480, pBagString);

    // 任务道具背包
    char* pMissionBagString = (char*)m_MissionBag.SaveToString(m_pItemManager);
    rt2_strcat(g_strItemBuffer20480, pMissionBagString);

    //// 宠物道具背包
    //char *pPetBagString =  (char*)m_PetBag.SaveToString(m_pItemManager);
    //rt2_strcat(g_strItemBuffer20480, pPetBagString);

    // 身上的装备
    for (int part = 0; part < MAX_EQUIP_PARTS; part++) {
        SItemID     id          = m_nItemID[part];
        const char* pItemString = id.SaveToString(m_pItemManager);
        rt2_strcat(g_strItemBuffer20480, pItemString);
    }
    //	END OF VERSION 10

    CHECK(strlen(g_strItemBuffer20480) < 20480);
    return g_strItemBuffer20480;

    unguard;
}

int CItemContainerBase::LoadFromString(char* str) {
    guard;

    Clear();

    // do NOT use "str", because strtok will destroy "str", so copy the std::string
    int strLength = (int)strlen(str);
    if (strLength == 0)
        return 0;
    char* strCopy = RT_NEW char[strLength + 1];  //(char*)malloc(strLength+1);
    rt2_strncpy(strCopy, str, strLength + 1);

    int   iTmp     = 0;
    char* pReadPtr = strCopy;
    int   version;

    // 版本
    int ret = sscanf(pReadPtr, "#%04x", &version);
    pReadPtr += 5;

    if (version == 11)  //000b
    {
        // 普通道具背包
        iTmp = m_Bag.LoadFromString(pReadPtr, m_pItemManager);
        if (iTmp == 0) {
            RtCoreLog().Info("Load bag item data error!\n");
            return 0;
        }
        pReadPtr += iTmp;

        // 任务道具背包
        iTmp = m_MissionBag.LoadFromString(pReadPtr, m_pItemManager);
        if (iTmp == 0) {
            RtCoreLog().Info("Load missionBag item data error!\n");
            return 0;
        }
        pReadPtr += iTmp;

        //// 宠物背包
        //iTmp = m_PetBag.LoadFromString(pReadPtr, m_pItemManager);
        //if (iTmp==0)
        //{
        //	RtCoreLog().Info("Load pet item data error!\n");
        //	return 0;
        //}
        //pReadPtr += iTmp;

        // 身上的装备
        char sep[] = ",";
        //		char *k = strtok(pReadPtr, sep);
        for (int part = 0; part < MAX_EQUIP_PARTS; part++) {
            SItemID id;
            iTmp = id.LoadFromString(version, pReadPtr, m_pItemManager);
            if (iTmp == 0) {
                RtCoreLog().Info("Load SItemID data error!\n");
                return 0;
            }
            pReadPtr += iTmp;

            if (id.id != 0) {
                _Equip(id, (EEquipParts)part);
                m_bEquipValid[part] = true;
            } else {
                m_nItemID[part].Clear();
                m_bEquipValid[part] = true;
            }
        }
    } else {
        //CHECKEX("[CItemContainerBase::LoadFromString]INVALID ITEM DATA!\n");
        DEL_ARRAY(strCopy);
        return 0;
    }

    char* pTemp = strCopy;
    DEL_ARRAY(strCopy);
    return abs(pReadPtr - pTemp);

    unguard;
}

bool CItemContainerBase::Serialize(char type, CG_CmdPacket* cmd) {
    guard;

    if (cmd->IsReading())
        Clear();

    switch (type) {
        case NET_UPDATE_ITEM_SHOW:
            m_nItemID[WEAPON_1].Serialize(cmd, type);
            //m_nItemID[WEAPON_2].Serialize(cmd, type);
            //m_nItemID[WEAPON_3].Serialize(cmd, type);
            m_nItemID[BODY].Serialize(cmd, type);
            m_nItemID[FOOT].Serialize(cmd, type);
            m_nItemID[GLOVE].Serialize(cmd, type);
            m_nItemID[CAESTUS].Serialize(cmd, type);
            m_nItemID[TRUMP].Serialize(cmd, type);
            break;

        case NET_PLAYER_QUERY_INFO:
            for (int i = 0; i < MAX_EQUIP_PARTS; i++) {
                m_nItemID[i].Serialize(cmd, type);
            }
            break;

        case NET_UPDATE_ALL:
        default: {
            for (int i = 0; i < MAX_EQUIP_PARTS; i++) {
                m_nItemID[i].Serialize(cmd);
            }
            m_Bag.Serialize(type, cmd);
            m_MissionBag.Serialize(type, cmd);
            m_PetBag.Serialize(type, cmd);

            SItemID arrItems[4];
            arrItems[0] = m_nItemID[GLOVE];
            arrItems[1] = m_nItemID[BODY];
            arrItems[2] = m_nItemID[FOOT];
            arrItems[3] = m_nItemID[CAESTUS];
            m_pSuit     = m_pItemManager->m_pSuitManager->CheckSuit(arrItems, 4);
        } break;
    }
    return true;

    unguard;
}

bool CItemContainerBase::Serialize(RtArchive* pFile) {
    guard;
    return false;
    unguard;
}

BOOL CItemContainerBase::ItemCanBeEquiped(SItemID& item, EEquipParts part) const {
    guard;
    if (part == WEAPON_1)
        return ItemIsWeapon(item);
    else if (part == WEAPON_2)
        return ItemIsWeapon(item);
    else if (part == WEAPON_3)
        return ItemIsWeapon(item);
    else if (part == BODY)
        return ItemIsArmor(item);
    else if (part == FOOT)
        return ItemIsArmor(item);
    else if (part == NECKLACE)
        return ItemIsShipin(item);
    else if (part == CHARM)
        return ItemIsGem(item);
    else if (part == RING)
        return ItemIsShipin(item);
    else if (part == GLOVE)
        return ItemIsArmor(item);
    else if (part == CAESTUS)
        return ItemIsArmor(item);
    else if (part == RING2)
        return ItemIsShipin(item);
    else if (part == KITS)
        return ItemIsShipin(item);
    else if (part == SEALEDBOOK)
        return ItemIsShipin(item);
    else if (part == AEROCRAFT)
        return ItemIsShipin(item);
    else if (part == TRUMP)
        return ItemIsTrump(item);
    else if (part == FASHION)
        return ItemIsFashion(item);
    else
        return FALSE;
    unguard;
}

bool CItemContainerBase::CheckEquip(EEquipParts vPart, SItemID& item, SCreature& cre) const {
    guard;

    BOOL_ENSURE(m_pItemManager)
    SItemBase* pItemBase = m_pItemManager->GetItem(item.type);
    BOOL_ENSURE(pItemBase)
    BYTE* pReq;
    if (ItemIsWeapon(item))
        pReq = pItemBase->requirement;
    else if (ItemIsArmor(item))
        pReq = pItemBase->requirement;
    else if (ItemIsShipin(item))
        pReq = pItemBase->requirement;
    else if (ItemIsTrump(item))
        pReq = pItemBase->requirement;
    else if (ItemIsGemMagic(item))
        pReq = pItemBase->requirement;
    else if (ItemIsFashion(item))
        pReq = pItemBase->requirement;
    else
        return false;

    int iReqStr = 0, iReqDex = 0, iReqInt = 0, iReqHit = 0, iReqCon = 0, iReqLevel = 0,
        iReqMaxLevel = 150;
    GetItemAttribRequirement(m_pItemManager, item, iReqStr, iReqDex, iReqInt, iReqHit, iReqCon,
                             iReqLevel, iReqMaxLevel);

    //Tianh 10.03.10
    if (cre.Lev < iReqLevel)
        return false;
    if (cre.Str.GetValue() < iReqStr)
        return false;
    if (cre.Dex.GetValue() < iReqDex)
        return false;
    if (cre.Int.GetValue() < iReqInt)
        return false;
    if (cre.Hit.GetValue() < iReqHit)
        return false;
    if (cre.Con.GetValue() < iReqCon)
        return false;
    if (pReq[ItemRequirement_Career] != 0x0f) {
        switch (cre.Metier) {
            case METIER_WARRIOR:
                if ((pReq[ItemRequirement_Career] & 0x01) == 0)
                    return false;
                break;
            case METIER_HUNTER:
                if ((pReq[ItemRequirement_Career] & 0x02) == 0)
                    return false;
                break;
            case METIER_WIZARD:
                if ((pReq[ItemRequirement_Career] & 0x04) == 0)
                    return false;
                break;
            case METIER_TAOIST:
                if ((pReq[ItemRequirement_Career] & 0x08) == 0)
                    return false;
                break;
        }
    }
    if (pReq[ItemRequirement_Grade] != 0) {
        if (cre.MetierLev < pReq[ItemRequirement_Grade] - 1)
            return false;
    }

    return true;

    unguard;
}

bool CItemContainerBase::CheckUse(SItemID& item, SCreature& cre) const {
    guard;

    BOOL_ENSURE(m_pItemManager)
    SItemBase* pItemBase = m_pItemManager->GetItem(item.type);
    BOOL_ENSURE(pItemBase)
    BYTE* pReq = pItemBase->requirement;

    int iReqStr = 0, iReqDex = 0, iReqInt = 0, iReqHit = 0, iReqCon = 0, iReqLevel = 0,
        iReqMaxLevel = 150;
    GetItemAttribRequirement(m_pItemManager, item, iReqStr, iReqDex, iReqInt, iReqHit, iReqCon,
                             iReqLevel, iReqMaxLevel);

    //if (cre.Lev > iReqMaxLevel)  //lmk取消 增加最大使用等级
    //    return false;
    if (cre.Lev < iReqLevel)
        return false;
    if (cre.Str.GetValue() < iReqStr)
        return false;
    if (cre.Dex.GetValue() < iReqDex)
        return false;
    if (cre.Int.GetValue() < iReqInt)
        return false;
    if (cre.Hit.GetValue() < iReqHit)
        return false;
    if (cre.Con.GetValue() < iReqCon)
        return false;
    if (pReq[ItemRequirement_Career] != 0x0f) {
        switch (cre.Metier) {
            case METIER_WARRIOR:
                if ((pReq[ItemRequirement_Career] & 0x01) == 0)
                    return false;
                break;
            case METIER_HUNTER:
                if ((pReq[ItemRequirement_Career] & 0x02) == 0)
                    return false;
                break;
            case METIER_WIZARD:
                if ((pReq[ItemRequirement_Career] & 0x04) == 0)
                    return false;
                break;
            case METIER_TAOIST:
                if ((pReq[ItemRequirement_Career] & 0x08) == 0)
                    return false;
                break;
        }
    }
    if (pReq[ItemRequirement_Grade] != 0) {
        if (cre.MetierLev < pReq[ItemRequirement_Grade] - 1)
            return false;
    }

    if (ItemIsWeapon(item) || ItemIsArmor(item) || ItemIsShipin(item))
        if (item.curWear <= 0)
            return false;

    return true;

    unguard;
}

void CItemContainerBase::_UpdateCreature(bool bEquip, SItemID& item, EEquipParts part) {}

void CItemContainerBase::_Equip(SItemID& id, EEquipParts vPart) {
    guard;

    if (!ItemID_IsValid(id))
        return;

    //	RtCoreLog().Info("[Equip] 装备到身上 (type=0x%x, id=%d)\n", id.type, id.id);
    m_nItemID[vPart] = id;
    m_pLastSuit      = m_pSuit;

    SItemID arrItems[4];
    arrItems[0] = m_nItemID[GLOVE];
    arrItems[1] = m_nItemID[BODY];
    arrItems[2] = m_nItemID[FOOT];
    arrItems[3] = m_nItemID[CAESTUS];
    m_pSuit     = m_pItemManager->m_pSuitManager->CheckSuit(arrItems, 4);

    m_bEquipValid[vPart] = true;
    _UpdateCreature(true, id, vPart);

    unguard;
}

// 参数注释:
// id: 要装备的物品ID
// vPart: 要装备的部位，默认为EQUIP_DEFAULT_PART
// bRemoveFromBag: 是否从背包中移除，默认为false
// pItemUnEquip1: 要卸下的物品ID，默认为NULL
// pPartUnEquip1: 要卸下的部位，默认为NULL
// pItemUnEquip2: 要卸下的物品ID，默认为NULL
// pPartUnEquip2: 要卸下的部位，默认为NULL
bool CItemContainerBase::Equip(SItemID& id, EEquipParts vPart /* = EQUIP_DEFAULT_PART*/,
                               bool bRemoveFromBag /* = false*/, SItemID* pItemUnEquip1 /* = NULL*/,
                               EEquipParts* pPartUnEquip1 /* = NULL*/,
                               SItemID*     pItemUnEquip2 /* = NULL*/,
                               EEquipParts* pPartUnEquip2 /* = NULL*/) {

    guard;
    BOOL_ENSURE(ItemID_IsValid(id) && (_IsValidPart(vPart) || vPart == EQUIP_DEFAULT_PART))
    SItemBase* pItemBase = m_pItemManager->GetItem(id.type);

    if (pItemUnEquip1)
        pItemUnEquip1->Clear();
    if (pPartUnEquip1)
        *pPartUnEquip1 = CItemContainerBase::EQUIP_DEFAULT_PART;
    if (pItemUnEquip2)
        pItemUnEquip2->Clear();
    if (pPartUnEquip2)
        *pPartUnEquip2 = CItemContainerBase::EQUIP_DEFAULT_PART;

    if (vPart == EQUIP_DEFAULT_PART) {
        vPart = GetDefaultEquipPart(id);
    }
    BOOL_ENSURE(ItemCanBeEquiped(id, vPart))

    if (vPart == WEAPON_1 || vPart == WEAPON_2) {
        if (ItemIsShield(id)) {
            bool bFlag = false;
            if (ItemID_IsValid(m_nItemID[WEAPON_1])) {
                SWeapon* pWea1 = (SWeapon*)m_pItemManager->GetItem(m_nItemID[WEAPON_1].type);
                if (pWea1->bBothHands) {
                    bFlag = true;
                    if (pItemUnEquip1)
                        *pItemUnEquip1 = m_nItemID[WEAPON_1];
                    if (pPartUnEquip1)
                        *pPartUnEquip1 = WEAPON_1;
                    BOOL_ENSURE(_UnEquip(WEAPON_1))
                }
            }
            if (ItemID_IsValid(m_nItemID[WEAPON_2])) {
                if (bFlag) {
                    if (pItemUnEquip2)
                        *pItemUnEquip2 = m_nItemID[WEAPON_2];
                    if (pPartUnEquip2)
                        *pPartUnEquip2 = WEAPON_2;
                } else {
                    if (pItemUnEquip1)
                        *pItemUnEquip1 = m_nItemID[WEAPON_2];
                    if (pPartUnEquip1)
                        *pPartUnEquip1 = WEAPON_2;
                }
                // 确保卸下装备成功
                BOOL_ENSURE(_UnEquip(WEAPON_2))
            }
            _Equip(id, WEAPON_2);
        } else {
            if (!((SWeapon*)pItemBase)->bBothHands) {
                if (ItemID_IsValid(m_nItemID[vPart])) {
                    if (pItemUnEquip1)
                        *pItemUnEquip1 = m_nItemID[vPart];
                    if (pPartUnEquip1)
                        *pPartUnEquip1 = vPart;
                    BOOL_ENSURE(_UnEquip(vPart))
                }
                _Equip(id, vPart);
            } else {
                bool bFlag = false;
                if (ItemID_IsValid(m_nItemID[WEAPON_1])) {
                    bFlag = true;
                    if (pItemUnEquip1)
                        *pItemUnEquip1 = m_nItemID[WEAPON_1];
                    if (pPartUnEquip1)
                        *pPartUnEquip1 = WEAPON_1;
                    BOOL_ENSURE(_UnEquip(WEAPON_1))
                }
                if (ItemID_IsValid(m_nItemID[WEAPON_2])) {
                    if (bFlag) {
                        if (pItemUnEquip2)
                            *pItemUnEquip2 = m_nItemID[WEAPON_2];
                        if (pPartUnEquip2)
                            *pPartUnEquip2 = WEAPON_2;
                    } else {
                        if (pItemUnEquip1)
                            *pItemUnEquip1 = m_nItemID[WEAPON_2];
                        if (pPartUnEquip1)
                            *pPartUnEquip1 = WEAPON_2;
                    }
                    BOOL_ENSURE(_UnEquip(WEAPON_2))
                }
                _Equip(id, WEAPON_1);
            }
        }

    } else {
        if (ItemID_IsValid(m_nItemID[vPart])) {
            if (pItemUnEquip1)
                *pItemUnEquip1 = m_nItemID[vPart];
            if (pPartUnEquip1)
                *pPartUnEquip1 = vPart;
            // 确保卸下装备成功
            BOOL_ENSURE(_UnEquip(vPart))
        }
        _Equip(id, vPart);
    }

    if (bRemoveFromBag) {
        m_Bag.RemoveItem(id);
    }

    OnAfterEquip(id, vPart);

    return true;

    unguard;
}

bool CItemContainerBase::_UnEquip(EEquipParts vPart, bool bAddToBag /* = false*/) {
    guard;

    SItemID itemOrig = m_nItemID[vPart];
    if (!ItemID_IsValid(itemOrig))
        return true;

    ItemID_SetInvalid(m_nItemID[vPart]);
    m_pLastSuit = m_pSuit;

    SItemID arrItems[4];
    arrItems[0] = m_nItemID[GLOVE];
    arrItems[1] = m_nItemID[BODY];
    arrItems[2] = m_nItemID[FOOT];
    arrItems[3] = m_nItemID[CAESTUS];
    m_pSuit     = m_pItemManager->m_pSuitManager->CheckSuit(arrItems, 4);

    if (bAddToBag) {
        if (!m_Bag.AddItem(itemOrig)) {
            // @@@@
            RtCoreLog().Info("[_UnEquip] UnEquip Success, But can NOT put into bag!\n");
            return false;
        }
    }

    if (m_bEquipValid[vPart])
        _UpdateCreature(false, itemOrig, vPart);

    m_bEquipValid[vPart] = true;

    return true;

    unguard;
}

bool CItemContainerBase::UnEquip(EEquipParts vPart, bool bAddToBag /* = false*/) {
    guard;

    BOOL_ENSURE(_IsValidPart(vPart))
    SItemID itemOld = m_nItemID[vPart];

    if (!_UnEquip(vPart, bAddToBag))
        return false;

    OnAfterUnEquip(vPart, itemOld);

    return true;

    unguard;
}

bool CItemContainerBase::UnEquipAll(bool bAddToBag /* = false*/) {
    guard;

    for (int part = EQUIP_DEFAULT_PART + 1; part < MAX_EQUIP_PARTS; part++) {
        if (!_UnEquip((EEquipParts)part, bAddToBag)) {
            // @@@@
            return false;
        }
    }
    return true;

    unguard;
}

void CItemContainerBase::OnAfterEquip(SItemID& item, EEquipParts vPart) {}

void CItemContainerBase::OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld) {}

CItemContainerBase::EEquipParts CItemContainerBase::GetDefaultEquipPart(SItemID& item) {
    guard;

    EEquipParts vPart     = EQUIP_DEFAULT_PART;
    SItemBase*  pItemBase = m_pItemManager->GetItem(item.type);
    if (!pItemBase)
        return EQUIP_DEFAULT_PART;

    if (ItemIsWeapon(item)) {
        SWeapon* pItemWea1 = (SWeapon*)m_pItemManager->GetItem(m_nItemID[WEAPON_1].type);
        SWeapon* pItemWea3 = (SWeapon*)m_pItemManager->GetItem(m_nItemID[WEAPON_3].type);
        if (pItemWea1 && !pItemWea3) {
            vPart = WEAPON_3;
        } else {
            vPart = WEAPON_1;
        }
    } else if (ItemIsCloth(item))
        vPart = BODY;
    else if (ItemIsGlove(item))
        vPart = GLOVE;
    else if (ItemIsShoes(item))
        vPart = FOOT;
    else if (ItemIsRing(item)) {
        if (!IsEquip(RING)) {
            vPart = RING;
        } else {
            vPart = RING2;
        }
    } else if (ItemIsNecklace(item))
        vPart = NECKLACE;
    else if (ItemIsCaestus(item))
        vPart = CAESTUS;
    else if (ItemIsKits(item))
        vPart = KITS;
    else if (ItemIsBook(item))
        vPart = SEALEDBOOK;
    else if (ItemIsFly(item))
        vPart = AEROCRAFT;
    else if (ItemIsTrump(item))
        vPart = TRUMP;
    else if (ItemIsFashion(item))
        vPart = FASHION;
    else if (ItemIsGem(item))
        vPart = CHARM;
    else
        vPart = EQUIP_DEFAULT_PART;

    return vPart;

    unguard;
}

int CItemContainerBase::GetUnEquipItemCount(SItemID&    item,
                                            EEquipParts vPart /* = EQUIP_DEFAULT_PART*/) {
    guard;

    if (vPart == EQUIP_DEFAULT_PART) {
        vPart = GetDefaultEquipPart(item);
    }

    if (!_IsValidPart(vPart))
        return 0;

    if (vPart == NECKLACE) {
        if (ItemID_IsValid(m_nItemID[NECKLACE]))
            return 1;
        else
            return 0;
    } else if (vPart == RING) {
        if (ItemID_IsValid(m_nItemID[RING]))
            return 1;
        else
            return 0;
    } else if (vPart == CHARM) {
        if (ItemID_IsValid(m_nItemID[CHARM]))
            return 1;
        else
            return 0;
    } else if (vPart == BODY) {
        if (ItemID_IsValid(m_nItemID[BODY]))
            return 1;
        else
            return 0;
    } else if (vPart == FOOT) {
        if (ItemID_IsValid(m_nItemID[FOOT]))
            return 1;
        else
            return 0;
    } else if (vPart == GLOVE) {
        if (ItemID_IsValid(m_nItemID[GLOVE]))
            return 1;
        else
            return 0;
    } else if (vPart == WEAPON_3) {
        if (ItemID_IsValid(m_nItemID[WEAPON_3]))
            return 1;
        else
            return 0;
    } else if (vPart == CAESTUS) {
        if (ItemID_IsValid(m_nItemID[CAESTUS]))
            return 1;
        else
            return 0;
    } else if (vPart == RING2) {
        if (ItemID_IsValid(m_nItemID[RING2]))
            return 1;
        else
            return 0;
    } else if (vPart == KITS) {
        if (ItemID_IsValid(m_nItemID[KITS]))
            return 1;
        else
            return 0;
    } else if (vPart == TRUMP) {
        if (ItemID_IsValid(m_nItemID[TRUMP]))
            return 1;
        else
            return 0;
    } else if (vPart == FASHION) {
        if (ItemID_IsValid(m_nItemID[FASHION]))
            return 1;
        else
            return 0;
    } else if (vPart == SEALEDBOOK) {
        if (ItemID_IsValid(m_nItemID[SEALEDBOOK]))
            return 1;
        else
            return 0;
    } else if (vPart == AEROCRAFT) {
        if (ItemID_IsValid(m_nItemID[AEROCRAFT]))
            return 1;
        else
            return 0;
    } else if (vPart == WEAPON_1) {
        SWeapon* pWea1 = (SWeapon*)m_pItemManager->GetItem(item.type);
        if (!pWea1)
            return 0;

        if (pWea1->bBothHands) {
            int num = 0;
            if (ItemID_IsValid(m_nItemID[WEAPON_1]))
                num++;
            if (ItemID_IsValid(m_nItemID[WEAPON_2]))
                num++;
            return num;
        } else {
            if (ItemID_IsValid(m_nItemID[WEAPON_1]))
                return 1;
            else
                return 0;
        }
    } else if (vPart == WEAPON_2) {
        int num = 0;
        if (ItemID_IsValid(m_nItemID[WEAPON_2])) {
            num++;
        }

        if (ItemID_IsValid(m_nItemID[WEAPON_1])) {
            SItemID  itemWea1 = m_nItemID[WEAPON_1];
            SWeapon* pWea1    = (SWeapon*)m_pItemManager->GetItem(itemWea1.type);
            if (!pWea1)
                return 0;

            if (pWea1->bBothHands) {
                num++;
            }
        }

        return num;
    } else if (vPart == WEAPON_3) {
        SWeapon* pWea1 = (SWeapon*)m_pItemManager->GetItem(item.type);
        if (!pWea1)
            return 0;
        if (ItemID_IsValid(m_nItemID[WEAPON_3]))
            return 1;
        else
            return 0;
    } else {
        RtCoreLog().Info("[CItemContainerBase::GetUnEquipItemCount] invalid part\n");
        //		CHECK(0);
        return 2;
    }

    unguard;
}

bool CItemContainerBase::UsePileItem(int page, int i, int j) {
    guard;

    if (!m_Bag._IsValidIndex(page, i, j))
        return false;
    SItemID id = m_Bag.GetItem(page, i, j);
    if (!ItemID_IsValid(id) || !ItemCanPile(id.type))
        return false;
    if (id.count == 0)
        return false;

    // 减少个数
    id.count--;
    if (id.count == 0x0) {
        m_Bag.RemoveItem(page, i, j);
    } else
        m_Bag.SetItem(page, i, j, id);

    return true;

    unguard;
}

bool CItemContainerBase::IsEquip(EEquipParts vPart) const {
    guard;

    if (!_IsValidPart(vPart))
        return false;
    if (!ItemID_IsValid(m_nItemID[vPart]))
        return false;
    if (!m_bEquipValid[vPart])
        return false;
    else
        return true;

    unguard;
}

SItemID* CItemContainerBase::GetEquipItemPoint(EEquipParts vPart, bool bIgnoreValid /* = false*/) {
    if (!_IsValidPart(vPart))
        return NULL;

    if (bIgnoreValid) {
        return &m_nItemID[vPart];
    } else {
        if (m_bEquipValid[vPart])
            return &m_nItemID[vPart];
        else
            return NULL;
    }
}

SItemID CItemContainerBase::GetEquipItem(EEquipParts vPart, bool bIgnoreValid /* = false*/) const {
    guard;

    if (!_IsValidPart(vPart))
        return ItemID_CreateInvalid();

    if (bIgnoreValid) {
        return m_nItemID[vPart];
    } else {
        if (m_bEquipValid[vPart])
            return m_nItemID[vPart];
        else
            return ItemID_CreateInvalid();
    }

    unguard;
}

bool CItemContainerBase::GetEquipItem(EEquipParts vPart, SItemBase*& vrpItem,
                                      bool bIgnoreValid /* = false*/) const {
    guard;

    if (!_IsValidPart(vPart)) {
        vrpItem = NULL;
        return false;
    }

    if (bIgnoreValid) {
        if (ItemID_IsValid(m_nItemID[vPart])) {
            vrpItem = m_pItemManager->GetItem(m_nItemID[vPart].type);
            if (vrpItem)
                return true;
            else
                return false;
        } else {
            vrpItem = NULL;
            return false;
        }
    } else {
        if (m_bEquipValid[vPart] && ItemID_IsValid(m_nItemID[vPart])) {
            vrpItem = m_pItemManager->GetItem(m_nItemID[vPart].type);
            if (vrpItem)
                return true;
            else
                return false;
        } else {
            vrpItem = NULL;
            return false;
        }
    }

    unguard;
}

bool CItemContainerBase::SetEquipItem(EEquipParts vPart, SItemID& item) {
    guard;

    BOOL_ENSURE(_IsValidPart(vPart))

    // 先把道具拿掉，然后强行装上
    _UnEquip(vPart);
    _Equip(item, vPart);

    // @@@@ 刷新状态

    return true;

    unguard;
}

bool CItemContainerBase::FindInEquipItems(DWORD id, EEquipParts& part) {
    guard;

    for (int part = 0; part < MAX_EQUIP_PARTS; part++) {
        if (m_nItemID[part].id == id)
            return true;
    }

    return false;

    unguard;
}

bool CItemContainerBase::FindInEquipItems(SItemID& item, EEquipParts& part) {
    guard;
    return FindInEquipItems(item.id, part);
    unguard;
}

bool CItemContainerBase::AddMoney(int count) {
    guard;
    return m_Bag.AddMoney(count);
    unguard;
}

bool CItemContainerBase::RemoveMoney(int count) {
    guard;
    return m_Bag.RemoveMoney(count);
    unguard;
}

bool CItemContainerBase::AddItem(SItemID& item) {
    guard;
    if (ItemIsMissionItem(item))
        return m_MissionBag.AddItem(item);
    else if (ItemIsPet(item))
        return m_PetBag.AddItem(item);
    else
        return m_Bag.AddItem(item);
    unguard;
}

bool CItemContainerBase::AddItem(vector<SItemID>& arrItems) {
    guard;
    int tmpPage, tmpI, tmpJ;
    for (int i = 0; i < (int)arrItems.size(); i++) {
        SItemID& item = arrItems[i];
        if (ItemIsMissionItem(item)) {
            BOOL_ENSURE(m_MissionBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
        } else if (ItemIsPet(item)) {
            BOOL_ENSURE(m_PetBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
        } else {
            if (ItemCanPile(item)) {
                BOOL_ENSURE(m_Bag.BuyTidy(item))
            } else {
                BOOL_ENSURE(m_Bag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
            }
        }
    }
    return true;
    unguard;
}

bool CItemContainerBase::RemoveItem(const SItemID& item) {
    guard;
    if (ItemIsMissionItem(item))
        return m_MissionBag.RemoveItem(item);
    else if (ItemIsPet(item.type))
        return m_PetBag.RemoveItem(item);
    else
        return m_Bag.RemoveItem(item);
    unguard;
}

// pBag中的所有物品，如果在对象中存在，就把它删除掉
bool CItemContainerBase::RemoveFromBag(CBag* pBag) {
    guard;

    int p1, i1, j1;
    int pageCount = pBag->GetPageCount();
    int lineCount = pBag->GetLineCount();
    int colCount  = pBag->GetColCount();
    BOOL_ENSURE(pBag)
    int counter = pBag->Count();
    if (counter == 0)
        return true;
    for (int page = 0; page < pageCount; page++)
        for (int i = 0; i < lineCount; i++)
            for (int j = 0; j < colCount; j++) {
                SItemID id = pBag->GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    if (!ItemIsMissionItem(id)) {
                        if (m_Bag.Find(id, &p1, &i1, &j1)) {
                            m_Bag.RemoveItem(p1, i1, j1);
                        }
                    } else if (ItemIsPet(id)) {
                        if (m_PetBag.Find(id, &p1, &i1, &j1)) {
                            m_PetBag.RemoveItem(p1, i1, j1);
                        }
                    } else {
                        if (m_MissionBag.Find(id, &p1, &i1, &j1)) {
                            m_MissionBag.RemoveItem(p1, i1, j1);
                        }
                    }
                    counter--;
                }
                if (counter == 0)
                    return true;
            }

    RtCoreLog().Info("[CItemContainerBase::RemoveFromBag] ItemContainer struct error!\n");
    //	CHECK(0);
    return false;

    unguard;
}

// array中的所有物品，如果在对象中存在，就把它删除掉
bool CItemContainerBase::RemoveFromArray(vector<SItemID>& arrItems) {
    guard;

    int arrSize = (int)arrItems.size();
    if (arrSize == 0)
        return true;
    int p1, i1, j1;
    for (int i = 0; i < arrSize; i++) {
        SItemID id = arrItems[i];
        if (!ItemIsMissionItem(id)) {
            if (m_Bag.Find(id, &p1, &i1, &j1)) {
                m_Bag.RemoveItem(p1, i1, j1);
            }
        } else if (ItemIsPet(id)) {
            if (m_PetBag.Find(id, &p1, &i1, &j1)) {
                m_PetBag.RemoveItem(p1, i1, j1);
            }
        } else {
            if (m_MissionBag.Find(id, &p1, &i1, &j1)) {
                m_MissionBag.RemoveItem(p1, i1, j1);
            }
        }
    }
    return true;

    unguard;
}

// array中的所有物品，如果在对象中存在，就把它删除掉
bool CItemContainerBase::RemoveFromArray(vector<DWORD>& arrItemID) {
    guard;

    int arrSize = (int)arrItemID.size();
    if (arrSize == 0)
        return true;
    int p1, i1, j1;
    for (int i = 0; i < arrSize; i++) {
        DWORD id = arrItemID[i];
        if (m_Bag.Find(id, &p1, &i1, &j1)) {
            m_Bag.RemoveItem(p1, i1, j1);
        } else if (m_PetBag.Find(id, &p1, &i1, &j1)) {
            m_PetBag.RemoveItem(p1, i1, j1);
        } else if (m_MissionBag.Find(id, &p1, &i1, &j1)) {
            m_MissionBag.RemoveItem(p1, i1, j1);
        } else
            return false;
    }
    return true;

    unguard;
}

// 本容器是否包含pBag中的所有物品
bool CItemContainerBase::IsContain(CBag* pBag) const {
    guard;

    BOOL_ENSURE(pBag)
    for (int page = 0; page < pBag->GetPageCount(); page++)
        for (int i = 0; i < pBag->GetLineCount(); i++)
            for (int j = 0; j < pBag->GetColCount(); j++) {
                SItemID id = pBag->GetItem(page, i, j);
                if (ItemID_IsValid(id)) {
                    if (!m_Bag.Find(id) && !m_MissionBag.Find(id) && !m_PetBag.Find(id)) {
                        return false;
                    }
                }
            }
    return true;

    unguard;
}

// 本容器是否包含arrItemID中的所有物品
bool CItemContainerBase::IsContain(vector<DWORD>& arrItemID) const {
    guard;

    if (arrItemID.empty())
        return true;

    vector<DWORD>::iterator iter = arrItemID.begin();
    for (; iter != arrItemID.end(); iter++) {
        DWORD id = (*iter);
        if (!m_Bag.Find(id) && !m_MissionBag.Find(id) && !m_PetBag.Find(id))
            return false;
    }
    return true;

    unguard;
}

// 本容器是否包含arrItems中的所有物品
bool CItemContainerBase::IsContain(vector<SItemID>& arrItems) const {
    guard;

    if (arrItems.empty())
        return true;

    vector<SItemID>::iterator iter = arrItems.begin();
    for (; iter != arrItems.end(); iter++) {
        SItemID item = (*iter);
        if (!m_Bag.Find(item) && !m_MissionBag.Find(item) && !m_PetBag.Find(item))
            return false;
    }
    return true;

    unguard;
}

bool CItemContainerBase::ExchangeItems(CBag* pBagFrom, CBag* pBagTo, vector<DWORD>& arrItems) {
    guard;

    if (!pBagFrom || !pBagTo)
        return false;

    CBag bagFrom1(pBagFrom->GetLineCount(), pBagFrom->GetColCount(), pBagFrom->GetPageCount(),
                  pBagFrom->m_pItemMgr);
    CBag bagTo1(pBagTo->GetLineCount(), pBagTo->GetColCount(), pBagTo->GetPageCount(),
                pBagFrom->m_pItemMgr);
    BOOL_ENSURE(bagFrom1.CloneFrom(pBagFrom))
    BOOL_ENSURE(bagTo1.CloneFrom(pBagTo))

    SItemID item;
    int     page, gridI, gridJ;
    int     tmpPage, tmpI, tmpJ;

    for (int i = 0; i < (int)arrItems.size(); i++) {
        if (!bagFrom1.Find(arrItems[i], &page, &gridI, &gridJ))
            return false;
        item = bagFrom1.GetItem(page, gridI, gridJ);
        if (!bagFrom1.RemoveItem(page, gridI, gridJ))
            return false;
        if (!bagTo1.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
            return false;
    }

    CHECK(pBagFrom->CloneFrom(&bagFrom1));
    CHECK(pBagTo->CloneFrom(&bagTo1));

    return true;

    unguard;
}

bool CItemContainerBase::ExchangeItems(CBag* pBagFrom, CItemContainerBase* pConTo,
                                       vector<DWORD>& arrItems) {
    guard;

    if (!pBagFrom || !pConTo)
        return false;

    CBag bagFrom1(pBagFrom->GetLineCount(), pBagFrom->GetColCount(), pBagFrom->GetPageCount(),
                  pBagFrom->m_pItemMgr);
    CItemContainerBase conTo1(pConTo->m_pItemManager, pConTo->m_pCre);
    conTo1.m_PetBag.Resize(pConTo->m_PetBag.GetPageCount(), pConTo->m_PetBag.GetLineCount(),
                           pConTo->m_PetBag.GetColCount());
    BOOL_ENSURE(bagFrom1.CloneFrom(pBagFrom))
    BOOL_ENSURE(conTo1.m_Bag.CloneFrom(&(pConTo->m_Bag)))
    BOOL_ENSURE(conTo1.m_MissionBag.CloneFrom(&(pConTo->m_MissionBag)))
    BOOL_ENSURE(conTo1.m_PetBag.CloneFrom(&(pConTo->m_PetBag)))

    SItemID item;
    int     page, gridI, gridJ;
    int     tmpPage, tmpI, tmpJ;

    for (int i = 0; i < (int)arrItems.size(); i++) {
        if (!bagFrom1.Find(arrItems[i], &page, &gridI, &gridJ))
            return false;
        item = bagFrom1.GetItem(page, gridI, gridJ);
        if (!bagFrom1.RemoveItem(page, gridI, gridJ))
            return false;
        if (ItemIsMissionItem(item)) {
            if (!conTo1.m_MissionBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
                return false;
        } else if (ItemIsPet(item)) {
            if (!conTo1.m_PetBag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
                return false;
        } else {
            if (!conTo1.m_Bag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
                return false;
        }
    }

    CHECK(pBagFrom->CloneFrom(&bagFrom1));
    CHECK(pConTo->m_Bag.CloneFrom(&(conTo1.m_Bag)));
    CHECK(pConTo->m_MissionBag.CloneFrom(&(conTo1.m_MissionBag)));
    CHECK(pConTo->m_PetBag.CloneFrom(&(conTo1.m_PetBag)));

    return true;

    unguard;
}

bool CItemContainerBase::ExchangeItems(CItemContainerBase* pCon1, vector<SItemID>& arrItems1,
                                       CItemContainerBase* pCon2, vector<SItemID>& arrItems2) {
    guard;

    if (!pCon1 || !pCon2)
        return false;

    CItemContainerBase conClone1(pCon1->m_pItemManager, pCon1->m_pCre);
    CItemContainerBase conClone2(pCon2->m_pItemManager, pCon2->m_pCre);
    conClone1.m_PetBag.Resize(pCon1->m_PetBag.GetPageCount(), pCon1->m_PetBag.GetLineCount(),
                              pCon1->m_PetBag.GetColCount());
    conClone2.m_PetBag.Resize(pCon2->m_PetBag.GetPageCount(), pCon2->m_PetBag.GetLineCount(),
                              pCon2->m_PetBag.GetColCount());
    BOOL_ENSURE(conClone1.m_Bag.CloneFrom(&(pCon1->m_Bag)))
    BOOL_ENSURE(conClone1.m_MissionBag.CloneFrom(&(pCon1->m_MissionBag)))
    BOOL_ENSURE(conClone1.m_PetBag.CloneFrom(&(pCon1->m_PetBag)))
    BOOL_ENSURE(conClone2.m_Bag.CloneFrom(&(pCon2->m_Bag)))
    BOOL_ENSURE(conClone2.m_MissionBag.CloneFrom(&(pCon2->m_MissionBag)))
    BOOL_ENSURE(conClone2.m_PetBag.CloneFrom(&(pCon2->m_PetBag)))

    int gridI, gridJ, page;
    int tmpPage, tmpI, tmpJ;

    for (int i = 0; i < (int)arrItems1.size(); i++) {
        SItemID item     = arrItems1[i];
        CBag*   pConBag1 = NULL;
        if (ItemIsMissionItem(item.type))
            pConBag1 = &(conClone1.m_MissionBag);
        else if (ItemIsPet(item.type))
            pConBag1 = &(conClone1.m_PetBag);
        else
            pConBag1 = &(conClone1.m_Bag);

        BOOL_ENSURE(pConBag1->Find(item.id, &page, &gridI, &gridJ))
        BOOL_ENSURE(pConBag1->RemoveItem(page, gridI, gridJ))
    }

    for (int i = 0; i < (int)arrItems2.size(); i++) {
        SItemID item     = arrItems2[i];
        CBag*   pConBag2 = NULL;
        if (ItemIsMissionItem(item.type)) {
            pConBag2 = &(conClone2.m_MissionBag);
        } else if (ItemIsPet(item.type)) {
            pConBag2 = &(conClone2.m_PetBag);
        } else {
            pConBag2 = &(conClone2.m_Bag);
        }

        BOOL_ENSURE(pConBag2->Find(item.id, &page, &gridI, &gridJ))
        BOOL_ENSURE(pConBag2->RemoveItem(page, gridI, gridJ))
    }

    for (int i = 0; i < (int)arrItems1.size(); i++) {
        SItemID item     = arrItems1[i];
        CBag*   pConBag2 = NULL;
        if (ItemIsMissionItem(item.type))
            pConBag2 = &(conClone2.m_MissionBag);
        else if (ItemIsPet(item.type))
            pConBag2 = &(conClone2.m_PetBag);
        else
            pConBag2 = &(conClone2.m_Bag);

        BOOL_ENSURE(pConBag2->AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
    }

    for (int i = 0; i < (int)arrItems2.size(); i++) {
        SItemID item     = arrItems2[i];
        CBag*   pConBag1 = NULL;
        if (ItemIsMissionItem(item.type))
            pConBag1 = &(conClone1.m_MissionBag);
        else if (ItemIsPet(item.type))
            pConBag1 = &(conClone1.m_PetBag);
        else
            pConBag1 = &(conClone1.m_Bag);

        BOOL_ENSURE(pConBag1->AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
    }

    CHECK(pCon1->m_Bag.CloneFrom(&(conClone1.m_Bag)));
    CHECK(pCon1->m_MissionBag.CloneFrom(&(conClone1.m_MissionBag)));
    CHECK(pCon1->m_PetBag.CloneFrom(&(conClone1.m_PetBag)));
    CHECK(pCon2->m_Bag.CloneFrom(&(conClone2.m_Bag)));
    CHECK(pCon2->m_MissionBag.CloneFrom(&(conClone2.m_MissionBag)));
    CHECK(pCon2->m_PetBag.CloneFrom(&(conClone2.m_PetBag)));

    return true;

    unguard;
}

//PZH
bool CItemContainerBase::LoseEquip(EEquipParts vPart, int nNum) {
    bool bRet = false;
    do {
        if (!_IsValidPart(vPart)) {
            break;
        }

        if (!ItemID_IsValid(m_nItemID[vPart])) {
            break;
        }

        if (!(ItemIsWeapon(m_nItemID[vPart]) || ItemIsArmor(m_nItemID[vPart]) ||
              ItemIsShipin(m_nItemID[vPart]))) {
            break;
        }
        if (m_nItemID[vPart].curWear > 0) {
            m_nItemID[vPart].curWear -= nNum;
            if (m_nItemID[vPart].curWear <= 0) {
                m_nItemID[vPart].curWear = 0;
                //m_nItemID[vPart].Clear();
                //m_bEquipValid[vPart] = false;
            }
        }
        bRet = true;
    } while (false);
    return bRet;
}

//

void CItemContainerBase::SetEquipValid(EEquipParts vPart, bool Valid) {
    if (!_IsValidPart(vPart)) {
        return;
    }

    if (!ItemID_IsValid(m_nItemID[vPart])) {
        return;
    }
    m_nItemID[vPart].Valid = Valid;
}

bool CItemContainerBase::SetEquipWear(EEquipParts vPart, int nNum /* = 0 */) {
    bool bRet = false;
    do {
        if (!_IsValidPart(vPart)) {
            break;
        }

        if (!ItemID_IsValid(m_nItemID[vPart])) {
            break;
        }

        if (!(ItemIsWeapon(m_nItemID[vPart]) || ItemIsArmor(m_nItemID[vPart]) ||
              ItemIsShipin(m_nItemID[vPart]))) {
            break;
        }
        m_nItemID[vPart].curWear = nNum;
        if (m_nItemID[vPart].curWear <= 0) {
            m_nItemID[vPart].curWear = 0;
        }
        bRet = true;
    } while (false);
    return bRet;
}

bool operator==(const CItemContainerBase& con1, const CItemContainerBase& con2) {
    guard;

    if (con1.m_Bag != con2.m_Bag)
        return false;
    if (con1.m_Bag != con2.m_Bag)
        return false;

    SItemID item1, item2;
    for (int part = 0; part < CItemContainerBase::MAX_EQUIP_PARTS; part++) {
        item1 = con1.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
        item2 = con2.GetEquipItem((CItemContainerBase::EEquipParts)part, true);
        if (item1 != item2)
            return false;
    }

    //if ( con1.m_pSuit || con2.m_pSuit )
    //{
    //	if (!con1.m_pSuit || !con2.m_pSuit)			return false;
    //	if (con1.m_pSuit->id != con2.m_pSuit->id)	return false;
    //}

    return true;

    unguard;
}

bool operator!=(const CItemContainerBase& con1, const CItemContainerBase& con2) {
    guard;
    return !(con1 == con2);
    unguard;
}

bool SDWORDArray::Serialize(CG_CmdPacket* cmd) {
    guard;

    if (cmd->IsReading()) {
        array.clear();

        long  size;
        DWORD tmp;
        *cmd << size;
        int counterForSafe = 0;
        for (int i = 0; i < size; i++, counterForSafe++) {
            *cmd << tmp;
            array.push_back(tmp);

            if (counterForSafe > 200)
                return false;
        }
    } else {
        long size = (long)array.size();
        *cmd << size;
        for (int i = 0; i < size; i++)
            *cmd << array[i];
    }

    if (cmd->IsError()) {
        RtCoreLog().Info("[SDWORDArray::Serialize] Net Data Error!\n");
        return false;
    } else
        return true;

    unguard;
}

void ItemUpdate_GetExtraAddtion(SItemID& item, CItemManager* pItemMgr, float& fDamage,
                                float& fAttack, float& fDodge, float& fArmor,
                                float& fElementArmor) {
    guard;

    int itemLevel = item.level;
    fDamage = fAttack = fDodge = fArmor = fElementArmor = 0.0f;

    SItemBase* pItemClass = pItemMgr->GetItem(item.type);
    if (!pItemClass)
        return;
    float fR = 1.0f;
    /*
	强化系数1=IF(强化等级<4,强化等级*0.2,IF(强化等级<7,(强化等级-3)*0.2+3*0.2,IF(强化等级<10,(强化等级-6)*0.4+3*0.2+3*0.2,2.4+3*0.4+3*0.2+3*0.2))))
	*/
    if (itemLevel < 4) {
        fR = itemLevel * 0.2f;
    } else if (itemLevel < 7) {
        fR = (itemLevel - 3) * 0.2f + 0.6;
    } else if (itemLevel < 10) {
        fR = (itemLevel - 6) * 0.4f + 1.2;
    } else {
        fR = 4.8;  //2.4+3*0.4+3*0.2+3*0.2;
    }

    if ((ItemIsWeapon(item.type) && !ItemIsShield(item.type))) {
        int nBase = pItemClass->nDamageMin + pItemClass->nDamageMax;
        nBase /= 2;
        fDamage = nBase * fR;
    } else if (ItemIsArmor(item.type) || ItemIsShield(item.type)) {
        fArmor = pItemClass->nArmor * fR;
    } else if (ItemIsTrump(item.type)) {
        int   iTemp = (float)item.params[TRUMP_LEV] / 20.f;
        float fInfo = 0.05f * (float)iTemp;

        float fBase = pItemClass->nDamageMin + pItemClass->nDamageMax;
        fBase /= 2;
        fDamage = fBase * fInfo;
        fArmor  = (float)pItemClass->nArmor * fInfo;
        fAttack = (float)pItemClass->nAttack * fInfo;
    }
    unguard;
}

void ItemUpdate_GetMoneyRequirement(SItemID& item, int& money, CItemManager* pItemMgr) {
    guard;

    if (!pItemMgr) {
        money = MAX_MONEY_COUNT;
        return;
    }
    SItemBase* pItemClass = pItemMgr->GetItem(item.type);
    if (!pItemMgr) {
        money = MAX_MONEY_COUNT;
        return;
    }
    if (item.level < 0 || item.level > ITEM_MAX_ITEM_LEVEL) {
        money = MAX_MONEY_COUNT;
        return;
    }

    //if (ItemIsWeapon(item.type) && !ItemIsShield(item.type))
    //    money = g_itemupdate_moneyRequireWeapon[item.level];
    //else if (ItemIsArmor(item.type) || ItemIsShield(item.type) || ItemIsShipin(item.type))
    //    money = g_itemupdate_moneyRequireArmor[item.level];
    //else
    //    money = MAX_MONEY_COUNT;

    if ((ItemIsWeapon(item.type) && !ItemIsShield(item.type)) || ItemIsTrump(item.type)) {
        //PZH
        //money = g_itemupdate_moneyRequireWeapon[item.level] * pItemClass->requirement[ItemRequirement_Level];
        //类型1：(100+INT(装备等级/10)*100)*装备强化等级
        money = (100 + (pItemClass->requirement[ItemRequirement_Level] / 10) * 100) * item.level;
        //
    } else if (ItemIsArmor(item.type) || ItemIsShield(item.type)) {
        //PZH
        //money = g_itemupdate_moneyRequireArmor[item.level] * pItemClass->requirement[ItemRequirement_Level];
        //类型2：(50+INT(装备等级/10)*50)*装备强化等级
        money = (50 + (pItemClass->requirement[ItemRequirement_Level] / 10) * 50) * item.level;
        //
    } else
        money = MAX_MONEY_COUNT;

    unguard;
}

void ItemUpdate_GetExtraRequirement(SItemID& item, CItemManager* pItemMgr, float& fStr, float& fHit,
                                    float& fInt, float& fDex) {
    guard;

    fStr = fHit = fInt = fDex = 0.0f;

    int itemLevel = item.level;
    if (itemLevel < 0) {
        SItemBase* pItemClass = pItemMgr->GetItem(item.type);
        if (!pItemClass)
            return;

        char cTmp   = pItemClass->requirement[ItemRequirement_Career];
        char metier = METIER_WARRIOR;
        if ((cTmp & 0x01) != 0x0) {
            metier = METIER_WARRIOR;
        }  //"战士";
        else if ((cTmp & 0x02) != 0x0) {
            metier = METIER_HUNTER;
        }  //"女猎手";
        else if ((cTmp & 0x04) != 0x0) {
            metier = METIER_WIZARD;  //"术士";
        } else if ((cTmp & 0x08) != 0x0) {
            metier = METIER_TAOIST;  //"道士";
        }

        if ((ItemIsWeapon(item.type) && !ItemIsShield(item.type)) || ItemIsTrump(item.type)) {
            float fAddtion[4] = {1.0f, 1.5f, 2.0f, 2.5f};
            if (metier == METIER_WARRIOR) {
                if (itemLevel >= 1 && itemLevel <= 3) {
                    fStr += fAddtion[0] * itemLevel;
                    fHit += fAddtion[0] * itemLevel;
                } else if (itemLevel >= 4 && itemLevel <= 6) {
                    fStr += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                } else if (itemLevel >= 7 && itemLevel <= 9) {
                    fStr += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                } else if (itemLevel == 10) {
                    fStr += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                } else {
                }
            } else if (metier == METIER_HUNTER) {
                if (itemLevel >= 1 && itemLevel <= 3) {
                    fDex += fAddtion[0] * itemLevel;
                    fHit += fAddtion[0] * itemLevel;
                } else if (itemLevel >= 4 && itemLevel <= 6) {
                    fDex += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                } else if (itemLevel >= 7 && itemLevel <= 9) {
                    fDex += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                } else if (itemLevel == 10) {
                    fDex += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                } else {
                }
            } else if (metier == METIER_TAOIST) {
                if (itemLevel >= 1 && itemLevel <= 3) {
                    fInt += fAddtion[0] * itemLevel;
                    fHit += fAddtion[0] * itemLevel;
                } else if (itemLevel >= 4 && itemLevel <= 6) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                } else if (itemLevel >= 7 && itemLevel <= 9) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                } else if (itemLevel == 10) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                } else {
                }
            } else if (metier == METIER_WIZARD) {
                if (itemLevel >= 1 && itemLevel <= 3) {
                    fInt += fAddtion[0] * itemLevel;
                    fHit += fAddtion[0] * itemLevel;
                } else if (itemLevel >= 4 && itemLevel <= 6) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
                } else if (itemLevel >= 7 && itemLevel <= 9) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
                } else if (itemLevel == 10) {
                    fInt += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                    fHit += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
                } else {
                }
            }
        } else if (ItemIsArmor(item.type) || ItemIsShield(item.type)) {
            float fAddtion[4] = {1.0f, 1.5f, 2.0f, 2.5f};
            if (itemLevel >= 1 && itemLevel <= 3) {
                fDex += fAddtion[0] * itemLevel;
            } else if (itemLevel >= 4 && itemLevel <= 6) {
                fDex += fAddtion[0] * 3 + fAddtion[1] * (itemLevel - 3);
            } else if (itemLevel >= 7 && itemLevel <= 9) {
                fDex += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * (itemLevel - 6);
            } else if (itemLevel == 10) {
                fDex += fAddtion[0] * 3 + fAddtion[1] * 3 + fAddtion[2] * 3 + fAddtion[3];
            } else {
            }
        }

        //if (ItemIsShield(item))
        //{
        //	fDex += 1.8 *itemLevel;
        //}
        //else if (ItemIsWeapon(item))
        //{
        //	if (metier==METIER_WARRIOR)
        //	{
        //		fStr += 2.34 *itemLevel;
        //		fHit += 1.8 *itemLevel;
        //	}
        //	else if (metier==METIER_HUNTER)
        //	{
        //		fStr += 1.8 *itemLevel;
        //		fHit += 1.98 *itemLevel;
        //	}
        //	else if (metier==METIER_TAOIST)
        //	{
        //		fHit += 2.25 *itemLevel;
        //		fInt += 2.25 *itemLevel;
        //	}
        //	else if (metier==METIER_WIZARD)
        //	{
        //		fHit += 2.25 *itemLevel;
        //		fInt += 2.25 *itemLevel;
        //	}
        //}
        //else if (ItemIsArmor(item))
        //{
        //	if (metier==METIER_WARRIOR)
        //	{
        //		fDex += 1.8 *itemLevel;
        //	}
        //	else if (metier==METIER_HUNTER)
        //	{
        //		fDex += 1.98 *itemLevel;
        //	}
        //	else if (metier==METIER_TAOIST)
        //	{
        //		fDex += 1.62 *itemLevel;
        //	}
        //	else if (metier==METIER_WIZARD)
        //	{
        //		fDex += 1.62 *itemLevel;
        //	}
        //}
        //else if (ItemIsShipin(item))
        //{
        //}
    }

    unguard;
}

void ColorItem_GetExtraRequirement(CItemManager* pItemMgr, SItemID& item, int& iStr, int& iDex,
                                   int& iInt, int& iHit) {
    guard;

    iStr = iDex = iInt = iHit = 0;

    unguard;
}

void GetItemAttribRequirement(CItemManager* pItemMgr, SItemID& item, int& iStr, int& iDex,
                              int& iInt, int& iHit, int& iCon, int& iLevel, int& iMaxLevel) {
    guard;

    iStr = iDex = iInt = iHit = iCon = iLevel = 0;
    iMaxLevel                                 = 150;  //MaxLevel要用最大值

    if (!pItemMgr)
        return;
    if (!ItemID_IsValid(item))
        return;

    // 道具的基础需求
    int nStr_base = 0, nHit_base = 0, nInt_base = 0, nDex_base = 0, nLevel_base = 0, nCon_base = 0;
    int nMaxLevel_base = 150;

    SItemBase* pItemBase = pItemMgr->GetItem(item.type);
    if (!pItemBase)
        return;
    if (ItemIsWeapon(item) && !ItemIsShield(item)) {
        SWeapon* pWeapon = (SWeapon*)pItemBase;
        nStr_base        = pWeapon->requirement[ItemRequirement_Strength];
        nHit_base        = pWeapon->requirement[ItemRequirement_Hit];
        nInt_base        = pWeapon->requirement[ItemRequirement_Intelligence];
        nDex_base        = pWeapon->requirement[ItemRequirement_Agility];
        nLevel_base      = pWeapon->requirement[ItemRequirement_Level];
        nCon_base        = pWeapon->requirement[ItemRequirement_Con];
    } else if (ItemIsArmor(item) || ItemIsShield(item)) {
        SArmor* pArmor = (SArmor*)pItemBase;
        nStr_base      = pArmor->requirement[ItemRequirement_Strength];
        nHit_base      = pArmor->requirement[ItemRequirement_Hit];
        nInt_base      = pArmor->requirement[ItemRequirement_Intelligence];
        nDex_base      = pArmor->requirement[ItemRequirement_Agility];
        nLevel_base    = pArmor->requirement[ItemRequirement_Level];
        nCon_base      = pArmor->requirement[ItemRequirement_Con];
    } else if (ItemIsShipin(item)) {
        SShiPin* pShipin = (SShiPin*)pItemBase;
        nStr_base        = pShipin->requirement[ItemRequirement_Strength];
        nHit_base        = pShipin->requirement[ItemRequirement_Hit];
        nInt_base        = pShipin->requirement[ItemRequirement_Intelligence];
        nDex_base        = pShipin->requirement[ItemRequirement_Agility];
        nLevel_base      = pShipin->requirement[ItemRequirement_Level];
        nCon_base        = pShipin->requirement[ItemRequirement_Con];
    } else {
        nStr_base      = pItemBase->requirement[ItemRequirement_Strength];
        nHit_base      = pItemBase->requirement[ItemRequirement_Hit];
        nInt_base      = pItemBase->requirement[ItemRequirement_Intelligence];
        nDex_base      = pItemBase->requirement[ItemRequirement_Agility];
        nLevel_base    = pItemBase->requirement[ItemRequirement_Level];
        nMaxLevel_base = pItemBase->requirement[ItemRequirement_MaxLevel];
        nCon_base      = pItemBase->requirement[ItemRequirement_Con];
    }

    // 打造等级的额外需求
    //float fStr_level=0.0f, fDex_level=0.0f, fInt_level=0.0f, fHit_level=0.0f, fCon_level=0.0f;
    //ItemUpdate_GetExtraRequirement(item, pItemMgr, fStr_level, fHit_level, fInt_level, fDex_level);

    // 蓝金装备的额外需求
    //int iStr_color=0, iDex_color=0, iInt_color=0, iHit_color=0, iCon_color=0;
    //ColorItem_GetExtraRequirement(pItemMgr, item, iStr_color, iDex_color, iInt_color, iHit_color);

    iStr      = nStr_base /* + iStr_color + rtRound(fStr_level)*/;
    iDex      = nDex_base /* + iDex_color + rtRound(fDex_level)*/;
    iInt      = nInt_base /* + iInt_color + rtRound(fInt_level)*/;
    iHit      = nHit_base /* + iHit_color + rtRound(fHit_level)*/;
    iCon      = nCon_base /* + iCon_color + rtRound(fCon_level)*/;
    iLevel    = nLevel_base;
    iMaxLevel = nMaxLevel_base;

    unguard;
}

bool ItemIsRare(SItemID& item, CItemManager* pItemMgr) {
    CHECK(pItemMgr);

    if (item.color == ItemColor_Green)
        return true;
    if (item.color == ItemColor_Blue)
        return true;
    if (item.color == ItemColor_Golden)
        return true;
    if (item.level >= 1)
        return true;
    SItemBase* pItemClass = pItemMgr->GetItem(item.type);
    if (pItemClass) {
        if (pItemClass->suitID != 0)
            return true;
    }

    return false;
}

CItemSelectionTable::CItemSelectionTable() {}

CItemSelectionTable::~CItemSelectionTable() {}

bool CItemSelectionTable::Init(CItemManager* pItemMgr) {
    guard;

    bool res = BuildTable(pItemMgr);
    if (res) {
        //#ifdef _DEBUG
        //		int levFrom, levTo;
        //		int tmpType, tmpSubtype, tmpMetier;
        //		std::string meaningLev, meaningType;
        //		std::string str = "";
        //		for (int i=0; i<ISL_COUNT; i++)
        //			for (int j=0; j<IST_COUNT; j++)
        //			{
        //				Explain_ItemSelectionLevel((EItemSelectionLevel)i, levFrom, levTo, meaningLev);
        //				Explain_ItemSelectionType((EItemSelectionType)j, tmpType, tmpSubtype, tmpMetier, meaningType);
        //				str += "[";
        //				str += meaningLev;	//rtFormatNumber(i);
        //				str += ",";
        //				str += meaningType;	//rtFormatNumber(j);
        //				str += "] ";
        //				for (int k=0; k<(int)m_Seletions[i][j].size(); k++)
        //				{
        //					str += rtFormatNumber(m_Seletions[i][j][k]);
        //					str += ",";
        //				}
        //				str += "\n";
        //			}
        //		RtCoreLog().Info("%s\n",str.c_str());
        //#endif
    }

    return res;
    unguard;
}

bool CItemSelectionTable::BuildTable(CItemManager* pItemMgr) {
    guard;

    if (!pItemMgr)
        return false;

    int         itemIndex = 0;
    SItemBase*  pItemBase = NULL;
    int         type1 = 0, subtype1 = 0;
    int         type2 = 0, subtype2 = 0;
    BYTE        cMetier1  = 0;
    int         metier2   = METIER_ALL;
    int         levelFrom = 0, levelTo = 0;
    int         i, j;
    std::string str1, str2;

    EXT_SPACE::unordered_map<DWORD, SItemBase*>::iterator iter = pItemMgr->m_mapIndexToItem.begin();
    for (; iter != pItemMgr->m_mapIndexToItem.end(); iter++) {
        itemIndex = iter->first;
        pItemBase = iter->second;
        cMetier1  = pItemBase->requirement[ItemRequirement_Career];
        CItemManager::GetTableType(pItemBase->type, type1, subtype1);

        if (pItemBase->selectLevel < 0)
            continue;

        if (!pItemBase->bFBDrow) {
            continue;
        }

        for (i = 0; i < ISL_COUNT; i++) {
            Explain_ItemSelectionLevel((EItemSelectionLevel)i, levelFrom, levelTo, str1);
            if (pItemBase->selectLevel >= levelFrom && pItemBase->selectLevel <= levelTo) {
                for (j = 0; j < IST_COUNT; j++) {
                    Explain_ItemSelectionType((EItemSelectionType)j, type2, subtype2, metier2,
                                              str2);
                    bool bMetierPass = false;
                    if (metier2 == METIER_ALL) {
                        bMetierPass = true;
                    } else {
                        if (metier2 == METIER_WARRIOR) {
                            if ((cMetier1 & CAREER_REQUIREMENT_1) != 0)
                                bMetierPass = true;
                        } else if (metier2 == METIER_HUNTER) {
                            if ((cMetier1 & CAREER_REQUIREMENT_2) != 0)
                                bMetierPass = true;
                        } else if (metier2 == METIER_TAOIST) {
                            if ((cMetier1 & CAREER_REQUIREMENT_3) != 0)
                                bMetierPass = true;
                        } else if (metier2 == METIER_WIZARD) {
                            if ((cMetier1 & CAREER_REQUIREMENT_4) != 0)
                                bMetierPass = true;
                        } else {
                        }
                    }
                    if (type2 == type1 && (subtype2 == subtype1 || subtype2 <= 0) && bMetierPass) {
                        m_Seletions[i][j].push_back(itemIndex);
                    }
                }
            }
        }
    }

    return true;
    unguard;
}

void CItemSelectionTable::Explain_ItemSelectionType(EItemSelectionType ist, int& outType,
                                                    int& outSubType, int& metier,
                                                    std::string& str) {
    guard;
    switch (ist) {
        case IST_WEAPON_WARRIOR:
            outType    = 1;
            outSubType = 0;
            metier     = METIER_WARRIOR;
            str        = "战士武器";
            break;
        case IST_WEAPON_HUNTER:
            outType    = 1;
            outSubType = 0;
            metier     = METIER_HUNTER;
            str        = "凤舞武器";
            break;
        case IST_WEAPON_TAOLIST:
            outType    = 1;
            outSubType = 0;
            metier     = METIER_TAOIST;
            str        = "道士武器";
            break;
        case IST_WEAPON_WIZARD:
            outType    = 1;
            outSubType = 0;
            metier     = METIER_WIZARD;
            str        = "术士武器";
            break;
        case IST_ARMOR_WARRIOR:
            outType    = 2;
            outSubType = 0;
            metier     = METIER_WARRIOR;
            str        = "战士防具";
            break;
        case IST_ARMOR_HUNTER:
            outType    = 2;
            outSubType = 0;
            metier     = METIER_HUNTER;
            str        = "凤舞防具";
            break;
        case IST_ARMOR_WIZARD:
            outType    = 2;
            outSubType = 0;
            metier     = METIER_WIZARD;
            str        = "术士防具";
            break;
        case IST_ARMOR_TAOLIST:
            outType    = 2;
            outSubType = 0;
            metier     = METIER_TAOIST;
            str        = "道士防具";
            break;

        case IST_SHIPIN:
            outType    = 3;
            outSubType = 0;
            metier     = METIER_ALL;
            str        = "饰    品";
            break;
        case IST_GEM:
            outType    = 5;
            outSubType = 0;
            metier     = METIER_ALL;
            str        = "宝    石";
            break;
        case IST_WEAPON_ALL:
            outType    = 1;
            outSubType = 0;
            metier     = METIER_ALL;
            str        = "所有武器";
            break;
        case IST_CLOTH_ALL:
            outType    = 2;
            outSubType = 3;
            metier     = METIER_ALL;
            str        = "所有衣服";
            break;
        case IST_SHOE_ALL:
            outType    = 2;
            outSubType = 2;
            metier     = METIER_ALL;
            str        = "所有鞋子";
            break;
        case IST_GLOVE_ALL:
            outType    = 2;
            outSubType = 1;
            metier     = METIER_ALL;
            str        = "所有手套";
            break;
        default:
            outType    = -1;
            outSubType = -1;
            str        = "Invalid";
            RtCoreLog().Info("[CItemSelectionTable::Explain_ItemSelectionType] INVALID param\n");
            break;
    }

    unguard;
}

void CItemSelectionTable::Explain_ItemSelectionLevel(EItemSelectionLevel isl, int& outLevelFrom,
                                                     int& outLevelTo, std::string& str) {
    guard;

    switch (isl) {
        case ISL_1_9:
            outLevelFrom = 1;
            outLevelTo   = 9;
            str          = " 1-9";
            break;
        case ISL_10_19:
            outLevelFrom = 10;
            outLevelTo   = 19;
            str          = "10-19";
            break;
        case ISL_20_29:
            outLevelFrom = 20;
            outLevelTo   = 29;
            str          = "20-29";
            break;
        case ISL_30_39:
            outLevelFrom = 30;
            outLevelTo   = 39;
            str          = "30-39";
            break;
        case ISL_40_49:
            outLevelFrom = 40;
            outLevelTo   = 49;
            str          = "40-49";
            break;
        case ISL_50_59:
            outLevelFrom = 50;
            outLevelTo   = 59;
            str          = "50-59";
            break;
        case ISL_60_69:
            outLevelFrom = 60;
            outLevelTo   = 69;
            str          = "60-69";
            break;
        case ISL_70_79:
            outLevelFrom = 70;
            outLevelTo   = 79;
            str          = "70-79";
            break;
        case ISL_80_89:
            outLevelFrom = 80;
            outLevelTo   = 89;
            str          = "80-89";
            break;
        case ISL_90_99:
            outLevelFrom = 90;
            outLevelTo   = 99;
            str          = "80-89";
            break;
        default:
            outLevelFrom = 0;
            outLevelTo   = 0;
            str          = "Invalid";
            RtCoreLog().Info("[CItemSelectionTable::Explain_ItemSelectionLevel] INVALID param %d\n",
                             isl);
            break;
    }

    unguard;
}

bool CItemSelectionTable::FindSelection(SItemSelector& sel, std::vector<DWORD>& outItemIDs) {
    guard;

    if (sel.level < 0 || sel.level > ISL_COUNT) {
        RtCoreLog().Info(
            "[CItemSelectionTable::FindSelection] Invalid struct SItemSelector.level\n");
        return false;
    }
    if (sel.type < 0 || sel.type > IST_COUNT) {
        RtCoreLog().Info(
            "[CItemSelectionTable::FindSelection] Invalid struct SItemSelector.type\n");
        return false;
    }

    outItemIDs = m_Seletions[sel.level][sel.type];
    return true;

    unguard;
}

CMagicEggRewardMgr::CMagicEggRewardMgr(CItemManager* pItemMgr) {
    m_pItemMgr = pItemMgr;
}

CMagicEggRewardMgr::~CMagicEggRewardMgr() {
    m_Rewards.clear();
}

bool CMagicEggRewardMgr::Init() {
    if (!LoadResource(R(RES_TABLE_EGG)))
        return false;

    return true;
}

SMagicItemReward* CMagicEggRewardMgr::FindReward(DWORD GroupID) {
    if (GroupID == 0)
        return NULL;

    EXT_SPACE::unordered_map<DWORD, SMagicItemReward>::iterator iter = m_Rewards.find(GroupID);
    if (iter == m_Rewards.end()) {
        return false;
    } else {
        return &(iter->second);
    }
}

bool CMagicEggRewardMgr::LoadResource(const char* FileName) {
    RtCsv csv;
    if (!csv.LoadTableFile(FileName)) {
        RtCoreLog().Info("[CMagicEggRewardMgr::LoadResource] Can NOT open egg file(%s)\n",
                         FileName);
        return false;
    }
    int LineCount = csv.GetLineCount();
    int iLine     = csv.FindLineByString("编号");
    if (iLine < 0)
        return false;

    char   sep[] = ";";
    int    Idx;
    DWORD  GroupID;
    int    type;
    char*  Params;
    char*  Rates;
    double GreenRate;
    double BlueRate;
    double GoldenRate;
    int    MaxCount;
    for (iLine++; iLine < LineCount; iLine++) {
        Idx = csv[iLine]["编号"].Long();
        if (Idx == 0)
            continue;
        GroupID    = csv[iLine]["分组ID"].Long();
        type       = (EMagicItemResult)csv[iLine]["类型"].Long();
        Params     = (char*)csv[iLine]["出现物品"].Str();
        MaxCount   = csv[iLine]["最大个数"].Int();
        Rates      = (char*)csv[iLine]["出现概率"].Str();
        GreenRate  = csv[iLine]["绿色几率"].Double();
        BlueRate   = csv[iLine]["蓝色几率"].Double();
        GoldenRate = csv[iLine]["金色几率"].Double();

        std::vector<DWORD> arrParams;
        if (Params && Params[0] != 0) {
            char* k = strtok(Params, sep);
            while (k) {
                arrParams.push_back(atoi(k));
                k = strtok(NULL, sep);
            }
        }

        std::vector<double> arrRates;
        if (Rates && Rates[0] != 0) {
            char* k = strtok(Rates, sep);
            while (k) {
                arrRates.push_back(atof(k));
                k = strtok(NULL, sep);
            }
        }

        if (arrParams.size() != arrRates.size()) {
            RtCoreLog().Info(
                "[CMagicEggRewardMgr::LoadResource] Param count != rate count (line %d)\n", iLine);
            return false;
        }

        SMagicItemReward* pReward;
        if ((pReward = FindReward(GroupID)) == NULL) {
            SMagicItemReward reward;
            reward.GroupID     = GroupID;
            m_Rewards[GroupID] = reward;
            pReward            = FindReward(GroupID);
        }
        CHECK(pReward);

        switch (type) {
            case MI_Item: {
                int nTmp = 0;
                for (nTmp = 0; nTmp < (int)arrParams.size(); nTmp++) {
                    pReward->AddReward(MI_Item, arrRates[nTmp], arrParams[nTmp], GreenRate,
                                       BlueRate, GoldenRate, MaxCount);
                }
            } break;
            case MI_Exp: {
                int nTmp = 0;
                for (nTmp = 0; nTmp < (int)arrParams.size(); nTmp++) {
                    pReward->AddReward(MI_Exp, arrRates[nTmp], arrParams[nTmp]);
                }
            } break;
            case MI_Money: {
                int nTmp = 0;
                for (nTmp = 0; nTmp < (int)arrParams.size(); nTmp++) {
                    pReward->AddReward(MI_Money, arrRates[nTmp], arrParams[nTmp]);
                }
            } break;
            case MI_SOUL: {
                int nTmp = 0;
                for (nTmp = 0; nTmp < (int)arrParams.size(); nTmp++) {
                    pReward->AddReward(MI_SOUL, arrRates[nTmp], arrParams[nTmp]);
                }
            } break;
            case MI_ALL: {
                int nTmp = 0;
                for (nTmp = 0; nTmp < (int)arrParams.size(); nTmp++) {
                    pReward->AddReward(MI_ALL, arrRates[nTmp], arrParams[nTmp], GreenRate, BlueRate,
                                       GoldenRate, MaxCount);
                }
            } break;
            default:
                RtCoreLog().Info(
                    "[CMagicEggRewardMgr::LoadResource] Invalid magicEgg type (line=%d)\n", iLine);
                return false;
        }
    }

    // 做有效性检查
    EXT_SPACE::unordered_map<DWORD, SMagicItemReward>::iterator iterReward = m_Rewards.begin();
    for (; iterReward != m_Rewards.end(); iterReward++) {
        SMagicItemReward& reward = iterReward->second;
        if (reward.CurrHitNum < 99.0) {
            RtCoreLog().Info(
                "[CMagicEggRewardMgr::LoadResource] 出现几率之和不等于100 (GroupID=%d,Rate=%d)\n",
                reward.GroupID, reward.CurrHitNum);
            return false;
        }
    }

    return true;
}

void CBag::ItemPile()  //背包所有物品堆叠 add by 李泽华 2010.05.10
{
    guard;
    int iii = 0;
    for (int p = 0; p < m_nPageCount; ++p) {
        for (int i = 0; i < m_nLineCount; ++i) {
            for (int j = 0; j < m_nColumnCount; ++j) {  //找出背包内可堆叠的物品A
                if (ItemCanPile(GetItem(p, i, j).type)) {
                    for (int pp = p; pp < m_nPageCount; ++pp) {
                        for (int ii = i; ii < m_nLineCount; ++ii) {
                            for (int jj = j * (!iii); jj < m_nColumnCount; ++jj) {
                                ++iii;
                                //找到A之后所有可跟A堆叠的物品
                                if (ItemCanPile(GetItem(pp, ii, jj).type) &&
                                    (GetItem(pp, ii, jj).type == GetItem(p, i, j).type) &&
                                    (GetItem(pp, ii, jj).cBind ==
                                     GetItem(p, i, j).cBind)) {  //判断叠加后是否超出堆叠上限
                                    //if (m_pItemMgr && ( ( GetItem( pp, ii, jj ).count + GetItem( p, i, j ).count) <= (m_pItemMgr->GetItem( GetItem( p, i, j ).type )->maxCount) ) )
                                    //{   //未超出上限，进行堆叠
                                    MoveItem(pp, ii, jj, p, i, j);
                                    //}
                                    //else if( m_pItemMgr )
                                    //{   //超出上限，堆叠可堆叠部分
                                    //	ResolveItem( pp, ii, jj, p, i, j,( ( m_pItemMgr->GetItem(GetItem( p, i, j ).type )->maxCount )-( GetItem( p, i, j ).count ) ) );
                                    //}
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Pinch();
    unguard;
}

void CBag::ItemUpdateQuickSort(std::vector<SItemID>& item, int left,
                               int right)  //背包整理快速排序 add by 李泽华 2010.05.10
{
    guard;
    int     i(left), j(right);
    SItemID middle;
    //middle = item[(left+right)/2];
    middle = item[(rand() % (right - left + 1)) + left];  //在left和right之间随即生成中值
    do {
        while ((item[i].type <= middle.type) && (i < right))  //从左扫描类型大于中值的item
        {
            if (item[i].type == middle.type)  //如果类型相同，判断颜色
            {
                if (item[i].color == middle.color)  //如果颜色相同，判断id
                {
                    int index = CItemManager::GetIndexFromType(item[i].type);       //获得item id
                    int middleIndex = CItemManager::GetIndexFromType(middle.type);  //获得中值 id
                    if (index > middleIndex)  //如果item id大于等于中值，跳出循环
                    {
                        break;
                    } else
                        ++i;
                } else if (item[i].color > middle.color)  //如果item颜色大于中值，跳出循环
                    break;
                else
                    ++i;
            } else
                ++i;
        }
        while ((item[j].type >= middle.type) && (j > left))  //从右扫描类型小于中值的item
        {
            if (item[j].type == middle.type)  //如果类型相同，判断颜色
            {
                if (item[j].color == middle.color)  //如果颜色相同，判断id
                {
                    int index = CItemManager::GetIndexFromType(item[j].type);       //获得item id
                    int middleIndex = CItemManager::GetIndexFromType(middle.type);  //获得中值 id
                    if (index < middleIndex)  //如果item id小于等于中值，跳出循环
                    {
                        break;
                    } else
                        --j;
                } else if (item[j].color < middle.color)  //如果item颜色小于中值，跳出循环
                    break;
                else
                    --j;
            } else
                --j;
        }
        if (i <= j)  //找到了一对值,交换
        {
            int pfrom, ifrom, jfrom, pto, ito, jto;
            Find(item[i], &pfrom, &ifrom, &jfrom);
            Find(item[j], &pto, &ito, &jto);
            SetItem(pfrom, ifrom, jfrom, item[j]);
            SetItem(pto, ito, jto, item[i]);
            ++i;
            --j;
        }
    } while (i <= j);  //如果两边扫描的下标交错，就停止（完成一次）
    if (left < j)  //当左边部分有值(left<j)，递归左半边
    {
        ItemUpdateQuickSort(item, left, j);
    }
    if (right > i)  //当右边部分有值(right>i)，递归右半边
    {
        ItemUpdateQuickSort(item, i, right);
    }
    unguard;
}

void CBag::TidyUpBag()  //背包整理 add by 李泽华 2010.05.11
{
    guard;
    if (IsEmpty())  //如果背包是空的，直接返回
    {
        return;
    } else  //非空，进行整理
    {
        ItemPile();  //先将所有可堆叠物品进行堆叠
        //vector<SItemID> item;//物品数组
        //ToItemsArray( item );//把背包内所有物品打包成数组
        //ItemUpdateQuickSort( item, 0, Count( ) - 1 );//背包整理快速排序
        Pinch();
    }
    unguard;
}

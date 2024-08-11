#include <region_client.h>
#ifndef _GC_PLAYER_ITEM
#define _GC_PLAYER_ITEM

class CItemSelectionTable;
class CItemManager;
struct SItemBase;
class CItemContainerBase;
class CRegionCreature;
class CMagicEggRewardMgr;
//extern char g_strItemBuffer4096[4096];
extern G_MEMDEF(g_strItemBuffer4096, 4096)
extern G_MEMDEF(g_strItemBuffer10240, 10240)
extern G_MEMDEF(g_strItemBuffer20480, 20480)
extern G_MEMDEF(g_strItemBuffer256, 256)
extern G_MEMDEF(g_strItemBufferBD, 256)

/// Item Type (32 bit)   ���б䶯��Ҫ�޸�LoadResource()�е���Ӧ����
//����	����������	����ID	�ֽ�ID	�ɶ���	�ɽ���	���̵�	��ɾ��	�ɶ���	���ص��ڷ�	
//	-------------------------------------------------------------------------------------------------------------------
//	(name)	|type	|subtype	|index	|bMissionItem	|bCanDelete	|bCanDrop	|bCanTrade	|bCanPile	|bCanUpdate	|bCanNPC
//	(bit)	| 4		| 4			| 16	| 1				| 1			| 1			| 1			| 1			| 1			| 1
//	-------------------------------------------------------------------------------------------------------------------
#define ITEM_MASK_TYPE					0xF0000000
#define ITEM_MASK_SUBTYPE				0xFF000000
#define ITEM_MASK_INDEX					0x00FFFF00

#define ITEM_MASK_WEAPON				0x10000000 //����
#define ITEM_MASK_ARMOR					0x20000000 //�·�
#define ITEM_MASK_SHIPIN				0x30000000 //��Ʒ
#define ITEM_MASK_YAOPING				0x40000000 //ҩƷ
#define ITEM_MASK_GEM					0x50000000 //��֪��
#define ITEM_MASK_SCROLL				0x60000000 //����
#define ITEM_MASK_OTHER					0x70000000 //����
#define ITEM_MASK_PET					0x80000000 //����
#define ITEM_MASK_SKILL					0x90000000 //����
#define ITEM_MASK_OTHER2                0xa0000000 //����

#define ITEM_MASK_TRUMP					0xb0000000					// �·���
#define ITEM_MASK_FASHION				0xc0000000					// ʱװ

#define ITEM_MASK_SORPTION				0xe0000000					// �������͵���
//#define ITEM_TYPE_COUNT					13

#define ITEM_MASK_MISSION_ITEM			0x00000080 //�������
#define ITEM_MASK_CAN_DELETE			0x00000040 //����ɾ��
#define ITEM_MASK_CAN_DROP				0x00000020 //���Զ���
#define ITEM_MASK_CAN_TRADE				0x00000010 //���Խ���
#define ITEM_MASK_CAN_PILE				0x00000008 //���Զѵ�
#define ITEM_MASK_CAN_UPDATE			0x00000004 //��������
#define ITEM_MASK_CAN_NPCTRADE			0x00000002 //����NPC����


/// ����(SubType)


#define ITEM_MASK_WEAPON_AXE			0x11000000					// �� 
#define ITEM_MASK_WEAPON_PESTLE			0x12000000					// �� //�Ļ�˫��
#define ITEM_MASK_WEAPON_HOOP			0x13000000					// ��
#define ITEM_MASK_WEAPON_RIBAND			0x14000000					// � //���Ǳ���
#define ITEM_MASK_WEAPON_WHEEL			0x15000000					// ��
#define ITEM_MASK_WEAPON_SWORD			0x16000000					// �� 
#define ITEM_MASK_WEAPON_WAND			0x17000000					// �� 
#define ITEM_MASK_WEAPON_BALL			0x18000000					// �� 

//#define ITEM_MASK_WEAPON_PESTLE			0x11000000					// ����
//
//#define ITEM_MASK_WEAPON_AXE			0x12000000					// ��
//#define ITEM_MASK_WEAPON_RIBAND			0x13000000					// ��
////#define ITEM_MASK_WEAPON_CROSSBOW		0x14000000					// ��
//#define ITEM_MASK_WEAPON_HOOP			0x15000000					// ���
//
//#define ITEM_MASK_WEAPON_BALL			0x16000000					// ������
//#define ITEM_MASK_WEAPON_SWORD			0x17000000					// ������
//#define ITEM_MASK_WEAPON_WHEEL			0x18000000					// ������
//#define ITEM_MASK_WEAPON_WAND			0x19000000					// ������

#define ITEM_MASK_WEAPON_SHIELD			0x1a000000					// ����

#define ITEM_MASK_WEAPON_MINING			0x1b000000					// �ڿ�������
#define ITEM_MASK_WEAPON_HERBALISM		0x1c000000					// ��ҩ������

/// ����Ʒ(SubType)
#define ITEM_MASK_ARMOR_GLOVE			0x21000000					// ����
#define ITEM_MASK_ARMOR_SHOES			0x22000000					// Ь��
#define ITEM_MASK_ARMOR_CLOTH			0x23000000					// �·�
#define ITEM_MASK_ARMOR_CAESTUS			0x24000000					// ����

/// ����(SubType)
#define ITEM_MASK_GEM_WEAPON1			0x51000000					// ������ǿ����1
#define ITEM_MASK_GEM_WEAPON2			0x52000000					// ������ǿ����2
#define ITEM_MASK_GEM_WEAPON3			0x53000000					// ������ǿ����3
#define ITEM_MASK_GEM_ARMOR1			0x54000000					// ������ǿ����1
#define ITEM_MASK_GEM_ARMOR2			0x55000000					// ������ǿ����2
#define ITEM_MASK_GEM_ARMOR3			0x56000000					// ������ǿ����3
#define ITEM_MASK_GEM_LUCK2			0x57000000					// ���ӳɹ��ʵ�����ʯ2(4-6)
#define ITEM_MASK_GEM_LUCK3			0x58000000					// ���ӳɹ��ʵ�����ʯ3(7-9)
#define ITEM_MASK_GEM_ANTIBROKEN		0x59000000					// ��ֹ���߱����ı�ʯ
#define ITEM_MASK_GEM_LUCK_6ONLY      0x5a000000                  // ʹ+6�ĳɹ�����ߵ�50%
#define ITEM_MASK_GEM_LUCK_7ONLY      0x5b000000                  // ʹ+7�ĳɹ�����ߵ�50% // 2006.6.27 ���Ϊ���İ�������쾧������ӦΪ+7+8+9���ɹ�����������70%
#define ITEM_MASK_GEM_MAGIC			0x5c000000					// ���ͷż��ܵķ���

/// ��Ʒ(SubType)
#define ITEM_MASK_SHIPIN_NECKLACE		0x31000000					// ����
#define ITEM_MASK_SHIPIN_RING			0x32000000					// ��ָ
#define ITEM_MASK_SHIPIN_GOD			0x33000000					// ��λ����

#define ITEM_MASK_SHIPIN_KITS			0x34000000					// ����
#define ITEM_MASK_SHIPIN_BOOK			0x35000000					// ����
#define ITEM_MASK_SHIPIN_FLY			0x36000000					// ������

/// ����(SubType)
#define ITEM_MASK_SCROLL_HOME			0x61000000					// �سǾ���
#define ITEM_MASK_SCROLL_COMPOSE		0x62000000					// �䷽
#define ITEM_MASK_SCROLL_ENCHANT		0x63000000					// ��ħ����

/// ҩƿ(SubType)
#define ITEM_MASK_YAOPING_HP			0x41000000					// Ѫƿ
#define ITEM_MASK_YAOPING_MP			0x42000000					// ����ƿ
#define ITEM_MASK_YAOPING_ENERGY    	0X43000000					// ����ƿ
#define ITEM_MASK_YAOPING_PETHP       	0X44000000					// ����Ѫƿ
#define ITEM_MASK_YAOPING_PETMP       	0X45000000					// ���﷨��ƿ
#define ITEM_MASK_YAOPING_EXP			0x46000000					// ����ƿ
#define ITEM_MASK_YAOPING_CARDMINUTE	0X47000000					// ���Ӵ�ʱ��ƿ

#define ITEM_MASK_PET_NORMAL			0x81000000					// �ճ�
#define ITEM_MASK_PET_FAIRY				0x82000000					// ����
#define ITEM_MASK_PET_SPECTRE			0x83000000					// �Ĺ�

#define ITEM_MASK_SORPTION_TRUMP		0xe1000000					// ������������
#define ITEM_MASK_SORPTION_PET			0xe2000000					// ������(�Ĺ���������)

// ����(SubType)
#define ITEM_MASK_OTHER_TASK			0x71000000                  // ����ʹ�õ��������
#define ITEM_MASK_OTHER_MAGIC			0x72000000                  // ħ���ʵ�
#define ITEM_MASK_OTHER_FLAG			0x73000000					// ��ս������
#define ITEM_MASK_OTHER_EFFECT			0x74000000					// ������Ч
#define ITEM_MASK_OTHER_DUNGEON			0x75000000					// �����������
#define ITEM_MASK_OTHER_CLEAR_STR		0x76000000
#define ITEM_MASK_OTHER_CLEAR_DEX		0x77000000
#define ITEM_MASK_OTHER_CLEAR_HIT		0x78000000
#define ITEM_MASK_OTHER_CLEAR_CON		0x79000000
#define ITEM_MASK_OTHER_CLEAR_INT		0x7a000000
#define ITEM_MASK_OTHER_CLEAR_SKILL		0x7b000000
#define ITEM_MASK_OTHER_BANKEXTEND      0x7c000000                  // �ֿ����ݾ���

#define ITEM_MASK_OTHER2_SPAWN          0xa1000000                  // �����ٻ����������
#define ITEM_MASK_OTHER2_NW_TELEPORT    0xa2000000                  // ����ս���������
#define ITEM_MASK_OTHER2_NW_CALLTEAM    0xa3000000                  // ����ս������ӳ�Ա������ʹ�������
#define ITEM_MASK_OTHER2_NW_SKILL       0xa4000000                  // ����ս�����پ���
#define ITEM_MASK_OTHER2_HERO           0xa5000000                  // ��������

/// װ������ɫ
enum EItemColor
{
	ItemColor_White		= 0,
	ItemColor_Green		= 1,
	ItemColor_Blue		= 2,
	ItemColor_Golden	= 3,
	ItemColor_Purple	= 4,
	ItemColor_All		= 5
};
inline bool ItemColor_IsValid(EItemColor ic)
{
	return (ic>=ItemColor_White && ic<ItemColor_All);
}

// ��������
enum EItemType
{
	IT_Unknown,
	IT_Weapon,
	IT_Armor,
	IT_Shipin,
	IT_Yaoping,
	IT_Charm,
	IT_Scroll,
	IT_Other,
	IT_Pet,
	IT_Skill
};


/// ����
enum EItemRequirement
{
	ItemRequirement_Level,				// �ȼ�Ҫ��
	ItemRequirement_Strength,			// ����Ҫ��
	ItemRequirement_Agility,			// ����Ҫ��
	ItemRequirement_Intelligence,		// ��������
	ItemRequirement_Hit,				// ��׼����
	ItemRequirement_Career,				// ְҵ����
	ItemRequirement_Grade,				// �׶�����
	ItemRequirement_MaxLevel,			// ���ȼ�����
	ItemRequirement_Con,                // ��������
	ItemRequirement_Count
};
#define CAREER_REQUIREMENT_1			0x01		// սʿ
#define CAREER_REQUIREMENT_2			0x02		// Ů����
#define CAREER_REQUIREMENT_3			0x04		// ��ʿ
#define CAREER_REQUIREMENT_4			0x08		// ��ʿ
#define CAREER_REQUIREMENT_COUNT		4


/// �������(������ɫװ�����ƽ�װ������װ��ѡ������)
#define	ITEM_ATTRIB_NONE				0

#define	ITEM_ATTRIB_CON					1	// ����
#define	ITEM_ATTRIB_STR					2	// ����
#define	ITEM_ATTRIB_DEX					3	// ����
#define	ITEM_ATTRIB_INT					4	// ����
#define	ITEM_ATTRIB_HIT 				5	// ��׼

//δ�õ�
#define	ITEM_ATTRIB_ATTACK				6	// ����
#define	ITEM_ATTRIB_DODGE				7	// ����
#define	ITEM_ATTRIB_DAMAGE				9	// �˺�
#define	ITEM_ATTRIB_ARMOR				10	// ����

#define	ITEM_ATTRIB_RATE_ATTACK			11	// ���аٷֱ�
#define	ITEM_ATTRIB_RATE_DODGE			12	// ����ٷֱ�
#define	ITEM_ATTRIB_RATE_DAMAGE			13	// �˺��ٷֱ�
#define	ITEM_ATTRIB_RATE_ARMOR			14	// �����ٷֱ�

#define	ITEM_ATTRIB_RATE_FIREARMOR		15	// Ԫ�ط���
#define	ITEM_ATTRIB_RATE_WATERARMOR		16	// 
#define	ITEM_ATTRIB_RATE_POISONARMOR	17	// 
#define	ITEM_ATTRIB_RATE_FIREDAMAGE		18	// Ԫ�ع���
#define	ITEM_ATTRIB_RATE_WATERDAMAGE	19	// 
#define	ITEM_ATTRIB_RATE_POISONDAMAGE	20	// 
#define	ITEM_ATTRIB_REQ_STR				21	// ������������
#define	ITEM_ATTRIB_REQ_DEX				22	// ���ݶ�������
#define	ITEM_ATTRIB_REQ_INT				23	// ������������
#define	ITEM_ATTRIB_REQ_HIT				24	// ��׼��������
#define ITEM_ATTRIB_RATE_ABSORBMP       25    // ��������ħ
#define	ITEM_ATTRIB_RATE_ATTACKSPEED	26	// �����ٶ�(%)
#define	ITEM_ATTRIB_ABSORBHP			27	// ��Ѫ
#define	ITEM_ATTRIB_ABSORBMP			28	// ��ħ
#define	ITEM_ATTRIB_MAXHP				29	// ���HP
#define	ITEM_ATTRIB_MAXMP				30	// ���MP

#define ITEM_ATTRIB_CON_POWER			31	// ����ǿ��
#define ITEM_ATTRIB_STR_POWER			32	// ����ǿ��
#define ITEM_ATTRIB_DEX_POWER			33	// ����ǿ��
#define ITEM_ATTRIB_INT_POWER			34	// ����ǿ��
#define ITEM_ATTRIB_HIT_POWER			35	// ��׼ǿ��
#define ITEM_ATTRIB_APPENDSKILL			36	// ��ӵ����＼�����еļ���

#define ITEM_ATTRIB_RATE_MOVESPEED		37	// �ƶ��ٶ�(%)
#define ITEM_ATTRIB_CRITICALHIT			38	// ��ɱ��
#define ITEM_ATTRIB_DAMAGE_MIN			39	// ��С�˺�
#define ITEM_ATTRIB_DAMAGE_MAX			40	// ����˺�
#define ITEM_ATTRIB_DAMAGE_ABSORB		41	// �����˺�����(%)
#define ITEM_ATTRIB_LUCK				42	// ����
#define ITEM_ATTRIB_SKILL				43	// ���Ӽ���
#define ITEM_ATTRIB_ATTACKRANGE			44	// ��������
#define ITEM_ATTRIB_RATE_MAXHP			45	// ���HP�ٷֱ�
#define ITEM_ATTRIB_RATE_MAXMP			46	// ���MP�ٷֱ�
#define ITEM_ATTRIB_RELEASEEFFECT_SELF	47	// ����ʱ�ͷ�������ļ���
#define ITEM_ATTRIB_RELEASEEFFECT_ENEMY	48	// ����ʱ�ͷ��ڶԷ��ļ���
#define ITEM_ATTRIB_SKILLCRITICAL_RATE	49	// ������
#define ITEM_ATTRIB_SKILLCRITICAL_DAM	50	// �����˺�
#define ITEM_ATTRIB_RATE_EXP			51	// ��þ��鱶��
#define ITEM_ATTRIB_RATE_GOLD			52	// ��ý�Ǯ����
#define ITEM_ATTRIB_REPARRY				53	// ��
#define ITEM_ATTRIB_REFAINT	            54    // ���λ�
//#define ITEM_ATTRIB_RESILIENCE			0x43 // ������
#define ITEM_ATTRIB_RESKILLCRITICAL     55    // ������
#define ITEM_ATTRIB_RECRITICALHIT	    56    // ����ɱ

//Tianh  ��װ���Ӽ���
#define ITEM_ATTRIB_SKILLONE            57
#define ITEM_ATTRIB_SKILLTWO            58

#define ITEM_ATTRIB_COUNT				60












RtString GetAttribString(WORD attrib);

struct SAttribAddtion
{
	WORD	attrib;
	short	count;
	BYTE	reqStr;
	BYTE	reqDex;
	BYTE	reqInt;
	BYTE	reqHit;
	float	value;
	BYTE	skillRate;
	SAttribAddtion() { attrib = 0; count = 0; reqStr = 0; reqDex = 0; reqInt = 0; reqHit = 0; value = 0.0f; skillRate = 0;}
	~SAttribAddtion() {}
};
bool operator==(const SAttribAddtion &att1, const SAttribAddtion &att2);
bool operator!=(const SAttribAddtion &att1, const SAttribAddtion &att2);

// ���߰�
enum EItemBind
{
	IB_NotBind			= 0,
	IB_Binded			= 1,
	IB_BindWhenPick		= 2,
	IB_BindWhenEquip	= 3,
};
inline bool ItemBind_IsValid(BYTE cBind)
{
	return (cBind==IB_NotBind || cBind==IB_Binded || cBind==IB_BindWhenPick || cBind==IB_BindWhenEquip);
}

#define ITEM_MAX_ITEM_LEVEL			10		// ���Ĵ���ȼ� //tim.yang  UpdateItem
#define ITEM_ITEM_LEVEL_STABLE		3		// �ȶ��Ĵ���ȼ�
#define ITEM_MAX_PARAM_COUNT		50		// 
#define ITEM_TRUMP_MAX_ATTACH		6		// �����������Ը���
#define ITEM_TRUMP_MAX_PARAM		8		// ������������


struct SItemID
{
    DWORD			id;				    // ����ID����Ϸ���κ��������߶�����������ͬ��ID��
    DWORD			type;			    // ��������

    union
    {
        struct
        {
            WORD			color : 3;			// ���ߵ���ɫ(��ɫװ������ɫװ���ͻƽ�װ��)
            WORD			level : 4;			// ����ȼ�(���10��,index��0��ʼ)
            WORD			cBind : 3;			// ��
            WORD            cLock : 1;          // ����ʱ�Ƿ���Ҫ����

            WORD            cCountable : 1;     // �Ƿ����(������ʱʹ��)
            WORD            cEnchant : 1;       // �Ƿ�ħ(������ʱʹ��)
            WORD            cTimeLimit : 1;     // �Ƿ���ʱ������(������ʱʹ��)
            WORD            _Reserved : 2;      // (����)
        };
        WORD _wAtt;
    };

    WORD			attribID1;		    // ��ɫװ���������ӵ�����ID1
    WORD			attribID2;		    // ��ɫװ���������ӵ�����ID2
    BYTE			attribRate1;	    // ��ɫװ������ID�ļӳɱ���1
    BYTE			attribRate2;	    // ��ɫװ������ID�ļӳɱ���2

    BYTE			count;			    // ҩƿ��ʣ������
    WORD			enchantAttrib;	    // ��ħ������ID
    DWORD           TimeLimit;          // ����ʱ��

    BYTE			paramCount;		    // ʹ�õ��Ĳ�������
    DWORD			params[ITEM_MAX_PARAM_COUNT];

	int				curWear;			// ��ǰ�;�		--add by ֣��
	char			Valid;				// �Ƿ���Լӳ�����
	std::string		name;
    void Clear()
    {
        id = 0; type = 0; color = ItemColor_White; level = 0; count = 1; cBind = IB_NotBind;
        pItem = NULL; attribID1 = 0; attribID2 = 0; attribRate1 = 0; attribRate2 = 0;
        enchantAttrib = 0;
        cLock = 0;
        paramCount = 0;
        for (int i=0; i<ITEM_MAX_PARAM_COUNT; i++)
            params[i] = 0;
        TimeLimit = 0;

        cCountable = 1;
        cEnchant = 1;
        cTimeLimit = 0;
        _Reserved = 0;
		curWear = -1;
		Valid = 1;
		name.clear();
    }

	SItemID() { Clear(); }
	~SItemID() {}
	bool Serialize(CG_CmdPacket *cmd, char serializeType = NET_UPDATE_ALL);
	void SaveToReadableString(CItemManager* pItemMgr, std::string &str);
	int LoadFromString(int version, const char* str, CItemManager *pItemMgr);
	const char* SaveToString(CItemManager *pItemMgr);
	SItemBase* GetSItemBase(){return pItem;}

	//const SItemID& operator=(const SItemID &item);
private:
	SItemBase*		pItem;
};
bool operator==(const SItemID &item1, const SItemID &item2);
bool operator!=(const SItemID &item1, const SItemID &item2);

EItemType ItemID_GetItemType(DWORD type);

/*Ŀǰ�Ѳ�ʹ��*/bool ItemID_HaveParams(DWORD type); // �ж�һ�����͵ĵ����Ƿ��в���(��params�Ƿ���ֵ)


#define PATH_STRING_LEN			64
#define DESC_STRING_LEN			256

enum EItemUseLimit
{
	IUL_Free			= 0,	// û���κ�����
	IUL_FeodWar			= 1,    // ��սʱ���ڳ�ս����������ʹ��
    IUL_NationalWar     = 2     // ��սʱ���ڹ�ս��������ʹ��
};

// ��Ʒ����
struct SItemBase
{
public:
	DWORD	type;									// һ������Ͷ������࣬����index
	//char	name[PATH_STRING_LEN];					// �����������֣��硰�������µ���...
	CM_MEMDEF(name, PATH_STRING_LEN)
	int		value;									// ��Ʒ�ļ�ֵ����۸������
	int     itemCredit;                             // ����
	int     itemContribute;                         // ����
	int     itemPoint;                              // ����
	int     itemRecord;                             // ���� 
	int     itemUnionRecord;                        // �ﹱ
	int     itemJobRecord;                          // �Ź�
	float   point;									// ��Ʒ�ĵ������̳ǵļ�ֵ
	char*	pDesc;									// ��Ʒ������Ϣ�����ö�̬����
	int		grade;									// ���ڲ�������׷������
	EItemUseLimit	timeLimit;						// ����ʹ�õ�����

	//char	icon[PATH_STRING_LEN];	
	CM_MEMDEF(icon, PATH_STRING_LEN)
	//char	szModel[PATH_STRING_LEN];				// ���ڵ��ϵ�ģ��
	CM_MEMDEF(szModel, PATH_STRING_LEN)
	//char	szSound[PATH_STRING_LEN];
	CM_MEMDEF(szSound, PATH_STRING_LEN)

	short			nAttack;			// ����
	short			nDamageMin;
	short			nDamageMax;
	short			nArmor;				// ����
	short			nDodge;				// ����


	BYTE	requirement[ItemRequirement_Count];		// װ��������
	int		selectLevel;				// ѡ��ȼ�

	DWORD	suitID;
	BYTE	initCount;					// ��ʼ����
	BYTE	maxCount;					// �������
	DWORD	taskID;						// ����ID
	//char	effect[PATH_STRING_LEN];	// ��Ч�ļ�
	CM_MEMDEF(effect, PATH_STRING_LEN)
	std::string param1;					// ����1
	DWORD	composeID;					// ���ߺϳ�ID
	DWORD	enchantAttribID;			// ��ħ����ID
	DWORD	decomposeID;				// װ���ķֽ�ID
	bool    bCanFuse;                   // �Ƿ�����ں�
	int		maxWear;					// �;�����
	bool    bFBDrow;//�Ƿ񸱱�����
	bool    bStore;//�Ƿ�ɴ��
	SItemBase()
	{
		CM_MEMPROTECTOR(name, PATH_STRING_LEN)
		CM_MEMPROTECTOR(icon, PATH_STRING_LEN)
		CM_MEMPROTECTOR(szModel, PATH_STRING_LEN)
		CM_MEMPROTECTOR(szSound, PATH_STRING_LEN)
		CM_MEMPROTECTOR(effect, PATH_STRING_LEN)

		type = 0;
		name[0] = '\0';
		value = 0;
		point = 0;
		pDesc = NULL;
		grade = 0;
		timeLimit = IUL_Free;
		icon[0] = '\0';
		szModel[0] = '\0';
		szSound[0] = '\0';
		nAttack = 0;
		nDamageMin = 0;
		nDamageMax = 0;
		nArmor = 0;
		nDodge = 0;
		for (int i=0; i<ItemRequirement_Count; i++)
			requirement[i] = 0;
		selectLevel = -1;
		suitID = 0;
		initCount = 0;
		maxCount = 0;
		taskID = 0;
		effect[0] = '\0';
		param1 = "";
		composeID = 0;
		enchantAttribID = 0;
		decomposeID = 0;
        bCanFuse = true;
		maxWear = -1;
		bFBDrow = true;
	}
	virtual ~SItemBase()
	{
		CM_MEMUNPROTECTOR(name)
		CM_MEMUNPROTECTOR(icon)
		CM_MEMUNPROTECTOR(szModel)
		CM_MEMUNPROTECTOR(szSound)
		CM_MEMUNPROTECTOR(effect)
	}
};
// ����
#define ITEM_WEAPON_EFFECT_FROM_LEVEL			7
struct SWeapon : public SItemBase
{
public:
	//char	Models1[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// ����������ģ�ͻ�仯��
	//char	Models1Num[ITEM_MAX_ITEM_LEVEL+1];						// ����������ʾ����
	//char	Models2[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// ˫������ʱ��ֵ�Ų�Ϊ��
	//char	Way[PATH_STRING_LEN];									// ������·�������ļ�	---add by ֣��
	CM_MEMDEF2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF(Models1Num, ITEM_MAX_ITEM_LEVEL+1)
	CM_MEMDEF2(Models2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF(Way, PATH_STRING_LEN)

	// Attributes
	BYTE			bBothHands;	

	float			fAttSpeed;			// �����ٶ�
	short			nAttRange;			// ��������
	DWORD			MagicWeaponParams[ITEM_MAX_PARAM_COUNT]; // ����������ʼֵ
	short 			nCriticalRate;		// ��ɱ��
	short			nMwMaxLevel;		// ���������ȼ�
	//char			FlySwordRib[PATH_STRING_LEN];		// ����
	CM_MEMDEF(FlySwordRib, PATH_STRING_LEN)

	SWeapon()
	{
		CM_MEMPROTECTOR2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Models1Num, ITEM_MAX_ITEM_LEVEL+1)
		CM_MEMPROTECTOR2(Models2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Way, PATH_STRING_LEN)
		CM_MEMPROTECTOR(FlySwordRib, PATH_STRING_LEN)

		int i = 0;
		for (i=0; i<ITEM_MAX_ITEM_LEVEL+1; i++)
		{
			Models1[i][0] = '\0';
			Models2[i][0] = '\0';
		}
		bBothHands = 0;
		fAttSpeed = 0.f;
		nAttRange = 0;
		for (i=0; i<ITEM_MAX_PARAM_COUNT; i++)
			MagicWeaponParams[i] = 0;
		nCriticalRate = 0;
		nMwMaxLevel = 0;
	}
	SWeapon(const SWeapon &object)
	{
		memcpy(this, &object, sizeof(SWeapon));
		CM_MEMPROTECTOR2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Models1Num, ITEM_MAX_ITEM_LEVEL+1)
		CM_MEMPROTECTOR2(Models2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Way, PATH_STRING_LEN)
		CM_MEMPROTECTOR(FlySwordRib, PATH_STRING_LEN)
	}
	virtual ~SWeapon()
	{
		CM_MEMUNPROTECTOR2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR2(Models2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR(Models1Num)
		CM_MEMUNPROTECTOR(Way)
		CM_MEMUNPROTECTOR(FlySwordRib)
	}
};

const int TRUMP_SIZE			= 23;
const int TRUMP_LEV				= 0;
const int TRUMP_MP				= 1;
const int TRUMP_ATTACH_TYPE		= 2;
const int TRUMP_ATTACH_POINT	= 3;
const int TRUMP_ATTACH_MP		= 4;
const int TRUMP_ATTACH_RATE		= 5;
const int TRUMP_SKILL_1			= 6;
const int TRUMP_SKILL_2			= 7;
const int TRUMP_SKILL_3			= 8;
const int TRUMP_SKILL_4			= 9;
const int TRUMP_SKILL_5			= 10;
const int TRUMP_SKILL_MAX		= 11;

const int TRUMP_WATER_ATT		= 11;
const int TRUMP_FIRE_ATT		= 12;
const int TRUMP_POISON_ATT		= 13;
const int TRUMP_WATER_DEF		= 14;
const int TRUMP_FIRE_DEF		= 15;
const int TRUMP_POISON_DEF		= 16;

const int TRUMP_WATER_ATT_EXP	= 17;
const int TRUMP_FIRE_ATT_EXP	= 18;
const int TRUMP_POISON_ATT_EXP	= 19;
const int TRUMP_WATER_DEF_EXP	= 20;
const int TRUMP_FIRE_DEF_EXP	= 21;
const int TRUMP_POISON_DEF_EXP	= 22;

// ����			--add by ֣��
struct STrump : public SItemBase
{
	//char	Models1[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// ����������ģ�ͻ�仯��
	//char	Way[PATH_STRING_LEN];									// ������·�������ļ�
	CM_MEMDEF2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF(Way, PATH_STRING_LEN)

	DWORD			TrumpDBParams[TRUMP_SIZE]; // ������洢ֵ
 
	STrump()
	{
		CM_MEMPROTECTOR2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Way, PATH_STRING_LEN)

		int i = 0;
		for (i=0; i<ITEM_MAX_ITEM_LEVEL+1; i++)
		{
			Models1[i][0] = '\0';
		}
		
		for (i = 0;i<TRUMP_SIZE;i++)
		{
			TrumpDBParams[i] = 0;
		}
		TrumpDBParams[TRUMP_ATTACH_TYPE] = -1;
		TrumpDBParams[TRUMP_LEV] = 1;
		TrumpDBParams[TRUMP_ATTACH_RATE] = 100;
	}
	void Init()
	{
	}
	virtual ~STrump()
	{
		CM_MEMUNPROTECTOR2(Models1, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR(Way)
	}
};
// ������װ��
struct SArmor : public SItemBase
{
public:
	//char	Skin[PATH_STRING_LEN];
	//char	UpdateSkins[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// ����Ʒ������Skin��仯��
	//char	UpdateSkins2[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// ����Ʒ������Skin��仯��
	//char	UpdateEffects[ITEM_MAX_ITEM_LEVEL+1][PATH_STRING_LEN];		// �������ӵ�Ч��
	CM_MEMDEF2(UpdateSkins, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF2(UpdateSkins2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF2(UpdateEffects, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
	CM_MEMDEF(Skin, PATH_STRING_LEN)

	short			fAttSpeed;			// �����ٶ�
	short			nAttRange;			// ��������
	short			nMoveSpeed;			// �ƶ��ٶ�
	int		        nMaxHP;             // ���HP�������У�

	SArmor()
	{
		CM_MEMPROTECTOR2(UpdateSkins, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR2(UpdateSkins2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR2(UpdateEffects, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Skin, PATH_STRING_LEN)

		int i = 0;
		Skin[0] = '\0';
		for (i=0; i<ITEM_MAX_ITEM_LEVEL+1; i++)
		{
			UpdateSkins[i][0] = '\0';
			UpdateEffects[i][0] = '\0';
		}
		fAttSpeed = 0;
		nAttRange = 0;
		nMoveSpeed = 0;
	}
	virtual ~SArmor()
	{
		CM_MEMUNPROTECTOR2(UpdateSkins, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR2(UpdateSkins2, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR2(UpdateEffects, ITEM_MAX_ITEM_LEVEL+1, PATH_STRING_LEN)
		CM_MEMUNPROTECTOR(Skin)
	}
};
// ��Ʒ
struct SShiPin : public SItemBase
{
public:
	int		nFireDefence;
	int		nWaterDefence;
	int		nPoisonDefence;
	int		nMaxHP;
	int		nMaxMP;

	SShiPin()
	{
		nFireDefence = 0;
		nWaterDefence = 0;
		nPoisonDefence = 0;
		nMaxMP = 0;
		nMaxHP = 0;
	}
	virtual ~SShiPin() { }
};
// ҩƿ
struct SYaoPing : public SItemBase
{
public:
	int		nHP;				// �ܹ������HP
	int		nMP;				// �ܹ������MP
	int		nHPRate;			// �ܹ������HP(%)
	int		nMPRate;			// �ܹ������MP(%)
	int		nExp;				// �ܹ������EXP
	int		nEnp;               // �ܲ����Enp
	int		nEnpRate;           // �ܲ����Enp(%)
	int     nCardMinute;		// ���ӵĴ�ʱ��
	int		nTime;				// ����ʱ��
	int		nOrigCount;			// ����

	SYaoPing()
	{
		nHP = 0;
		nMP = 0;
		nHPRate = 0;
		nMPRate = 0;
		nExp	= 0;
		nCardMinute = 0;
		nTime = 0;
		nOrigCount = 0;
	}
	virtual ~SYaoPing() { }
};
// ��ʯ(ԭ����)
struct SGem : public SItemBase
{
public:
	float	fRate;
	long	lFuse;
	char	cFuseRate;
	long	lFuseMoney;
	SGem()
	{
		fRate = 0.0f;
		lFuse = 0;
		cFuseRate = 0;
		lFuseMoney = 0;
	}
	virtual ~SGem() { }
};
// ����
struct SScroll : public SItemBase
{
public:
	int MovePosX;
	int MovePosY;
	SScroll()
	{
		MovePosX = 0;
		MovePosY = 0;
	}
	virtual ~SScroll() { }
};

// ��������
struct SOtherItem : public SItemBase
{
public:
	SOtherItem() { }
	virtual ~SOtherItem() { }
};

// ����
struct SItemPet : public SItemBase
{
public:
	short	petID;
	DWORD	PetParams[ITEM_MAX_PARAM_COUNT]; // ���޲�����ʼֵ
	long    NeedCulLev;  //������Ϊ�ȼ�

	SItemPet()
	{
		petID = 0;
		for (int i=0; i<ITEM_MAX_PARAM_COUNT; i++)
			PetParams[i] = 0;
	}
	virtual ~SItemPet() { }
};

const unsigned char MAX_SORPTION_NUM = 10;
struct SSorption : public SItemBase
{
public:
	unsigned char count;
	DWORD param[MAX_SORPTION_NUM];
	SSorption() 
	{ 
		count = 0;
		for (int i=0;i<MAX_SORPTION_NUM;i++)
			param[i] = 0;
	}
	virtual ~SSorption() { }
};

// ��������
struct SItemSkill : public SItemBase
{
public:
	DWORD skillID;

	SItemSkill()
	{
		skillID = 0;
	}
	virtual ~SItemSkill() { }
};


struct SItem
{
public:
	SItemID id;
	union
	{
		SItemBase*		pItemBase;
		SWeapon*		pWea;
		SArmor*			pArmor;
		SYaoPing*		pYaoPing;
		SScroll*		pScroll;
		SShiPin*		pShiPin;
		SOtherItem*		pOtherItem;
		SItemPet*		pPet;
		SItemSkill*		pSkill;
		/*add Tianh*/
		STrump*         pTrump;
	};
};


/// Helper Functions:
// һ������
inline BOOL ItemIsWeapon(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_WEAPON);}
inline BOOL ItemIsArmor(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_ARMOR);	}
inline BOOL ItemIsGem(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_GEM);	}
inline BOOL ItemIsScroll(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_SCROLL);}
inline BOOL ItemIsYaoPing(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_YAOPING);}
inline BOOL ItemIsShipin(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_SHIPIN);}
inline BOOL ItemIsOther(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_OTHER || (id.type & ITEM_MASK_TYPE) == ITEM_MASK_OTHER2);}
inline BOOL ItemIsPet(SItemID& id)			{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_PET);}
inline BOOL ItemIsSkill(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_SKILL);}
inline BOOL ItemIsTrump(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_TRUMP);}
inline BOOL ItemIsFashion(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_FASHION);}
inline BOOL ItemIsSorption(SItemID& id)		{	return ( (id.type & ITEM_MASK_TYPE) == ITEM_MASK_SORPTION);}

// ������������
inline BOOL ItemIsWeapon_Pestle(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_PESTLE);}
inline BOOL ItemIsWeapon_Axe(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_AXE);}
inline BOOL ItemIsWeapon_Riband(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_RIBAND);}
inline BOOL ItemIsWeapon_Hoop(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_HOOP);}
inline BOOL ItemIsWeapon_Ball(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_BALL);}
inline BOOL ItemIsWeapon_Sword(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_SWORD);}
inline BOOL ItemIsWeapon_Wheel(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_WHEEL);}
inline BOOL ItemIsWeapon_Wand(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_WAND);}
inline BOOL ItemIsShield(SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_SHIELD);}
inline BOOL ItemIsMineEquip(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_MINING);}
inline BOOL ItemIsHerbalEquip(SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_HERBALISM);}

inline BOOL ItemIsWeapon_Pestle(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_PESTLE);}
inline BOOL ItemIsWeapon_Axe(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_AXE);}
inline BOOL ItemIsWeapon_Riband(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_RIBAND);}
inline BOOL ItemIsWeapon_Hoop(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_HOOP);}
inline BOOL ItemIsWeapon_Ball(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_BALL);}
inline BOOL ItemIsWeapon_Sword(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_SWORD);}
inline BOOL ItemIsWeapon_Wheel(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_WHEEL);}
inline BOOL ItemIsWeapon_Wand(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_WAND);}
inline BOOL ItemIsShield(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_SHIELD);}
inline BOOL ItemIsMineEquip(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_MINING);}
inline BOOL ItemIsHerbalEquip(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_HERBALISM);}
inline BOOL ItemIsWeapon_MagicWeapon(DWORD type)
{
	DWORD tmp = type & ITEM_MASK_SUBTYPE;
	if (tmp==ITEM_MASK_WEAPON_SWORD || tmp==ITEM_MASK_WEAPON_WHEEL)
		return 1;
	else 
		return 0;
}
inline BOOL ItemIsWeapon_NeedLink(SItemID& id)
{
	return (ItemIsWeapon_Pestle(id) || ItemIsWeapon_Axe(id) || ItemIsWeapon_Hoop(id) || ItemIsWeapon_Wand(id) || ItemIsWeapon_Riband(id));
}

inline BOOL ItemIsCaestus(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_ARMOR_CAESTUS);}
inline BOOL ItemIsCloth(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_ARMOR_CLOTH);}
inline BOOL ItemIsGlove(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_ARMOR_GLOVE);}
inline BOOL ItemIsShoes(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_ARMOR_SHOES);}
inline BOOL ItemIsRing(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SHIPIN_RING);}
inline BOOL ItemIsNecklace(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SHIPIN_NECKLACE);}
inline BOOL ItemIsKits(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SHIPIN_KITS);}
inline BOOL ItemIsBook(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SHIPIN_BOOK);}
inline BOOL ItemIsFly(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SHIPIN_FLY);}
inline BOOL ItemIsHPBottle(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_HP);}
inline BOOL ItemIsMPBottle(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_MP);}
inline BOOL ItemIsEXPBottle(const SItemID& id)		{   return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_EXP);}
inline BOOL ItemIsCardMinuteBottle(const SItemID& id) {	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_CARDMINUTE);}
inline BOOL ItemIsBow(const SItemID& id)			{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_WEAPON_RIBAND);}
inline BOOL ItemIsPetHpBottle(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_PETHP);}
inline BOOL ItemIsPetMpBottle(const SItemID& id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_PETMP);}
inline BOOL ItemIsEnergyBottle(const SItemID& id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_ENERGY);}

inline BOOL ItemIsGemMagic(const SItemID &id)		{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_MAGIC);}
inline BOOL ItemIsGemWeapon1(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_WEAPON1);}
inline BOOL ItemIsGemWeapon2(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_WEAPON2);}
inline BOOL ItemIsGemWeapon3(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_WEAPON3);}
inline BOOL ItemIsGemArmor1(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_ARMOR1);}
inline BOOL ItemIsGemArmor2(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_ARMOR2);}
inline BOOL ItemIsGemArmor3(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_ARMOR3);}
inline BOOL ItemIsGemLuck2(DWORD type)			{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_LUCK2);}
inline BOOL ItemIsGemLuck3(DWORD type)			{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_LUCK3);}
inline BOOL ItemIsGemLuck6Only(DWORD type)        {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_LUCK_6ONLY);}
inline BOOL ItemIsGemLuck7Only(DWORD type)        {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_LUCK_7ONLY);}
inline BOOL ItemIsGemAntiBroken(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_GEM_ANTIBROKEN);}
inline BOOL ItemIsGemWeapon(const SItemID &id)
{
	DWORD tmp = id.type & ITEM_MASK_SUBTYPE;
	if (tmp==ITEM_MASK_GEM_WEAPON1 || tmp==ITEM_MASK_GEM_WEAPON2 || tmp==ITEM_MASK_GEM_WEAPON3)
		return 1;
	else 
		return 0;
}
inline BOOL ItemIsGemArmor(const SItemID &id)
{
	DWORD tmp = id.type & ITEM_MASK_SUBTYPE;
	if (tmp==ITEM_MASK_GEM_ARMOR1 || tmp==ITEM_MASK_GEM_ARMOR2 || tmp==ITEM_MASK_GEM_ARMOR3)
		return 1;
	else 
		return 0;
}
inline BOOL ItemIsGemLuck(const SItemID &id)
{
	DWORD tmp = id.type & ITEM_MASK_SUBTYPE;
	if (tmp==ITEM_MASK_GEM_LUCK2 || tmp==ITEM_MASK_GEM_LUCK3 || tmp==ITEM_MASK_GEM_LUCK_6ONLY || tmp==ITEM_MASK_GEM_LUCK_7ONLY)
		return 1;
	else 
		return 0;
}

inline BOOL ItemIsPetNormal(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_NORMAL);}
inline BOOL ItemIsPetFairy(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_FAIRY);}
inline BOOL ItemIsPetSpectre(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_SPECTRE);}

inline BOOL ItemIsSorptionTrump(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SORPTION_TRUMP);}
inline BOOL ItemIsSorptionPet(const SItemID &id)	{	return ( (id.type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SORPTION_PET);}


inline BOOL	ItemIsScrollHome(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SCROLL_HOME);}
inline BOOL ItemIsScrollCompose(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SCROLL_COMPOSE);}
inline BOOL ItemIsScrollEnchant(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SCROLL_ENCHANT);}

inline BOOL ItemIsWeapon(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_WEAPON);}
inline BOOL ItemIsArmor(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_ARMOR);	}
inline BOOL ItemIsGem(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_GEM);	}
inline BOOL ItemIsScroll(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_SCROLL);}
inline BOOL ItemIsYaoPing(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_YAOPING);}
inline BOOL ItemIsShipin(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_SHIPIN);}
inline BOOL ItemIsOther(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_OTHER) || ( (type & ITEM_MASK_TYPE) == ITEM_MASK_OTHER2);}
inline BOOL ItemIsPet(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_PET);}
inline BOOL ItemIsSkill(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_SKILL);}
inline BOOL ItemIsTrump(DWORD type)			{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_TRUMP);}
inline BOOL ItemIsFashion(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_FASHION);}
inline BOOL ItemIsSorption(DWORD type)		{	return ( (type & ITEM_MASK_TYPE) == ITEM_MASK_SORPTION);}

inline BOOL ItemIsHPBottle(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_HP);}
inline BOOL ItemIsMPBottle(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_MP);}
inline BOOL ItemIsEXPBottle(DWORD type)		{   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_EXP);}
inline BOOL ItemIsCardMinuteBottle(DWORD type){ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_CARDMINUTE);}
inline BOOL ItemIsEnergyBottle(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_ENERGY);}
inline BOOL ItemIsPetHpBottle(DWORD type)   {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_PETHP);}
inline BOOL ItemIsPetMpBottle(DWORD type)   {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_YAOPING_PETMP);}

inline BOOL ItemIsOther_Task(DWORD type)    {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_TASK);}
inline BOOL ItemIsOther_Magic(DWORD type)   {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_MAGIC);}
inline BOOL ItemIsOther_Flag(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_FLAG);}
inline BOOL	ItemIsOther_Effect(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_EFFECT);}
inline BOOL ItemIsOther_Dungeon(DWORD type) {	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_DUNGEON); }
inline BOOL	ItemIsOther_ClearStr(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_STR); }
inline BOOL	ItemIsOther_ClearDex(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_DEX); }
inline BOOL	ItemIsOther_ClearHit(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_HIT); }
inline BOOL	ItemIsOther_ClearCon(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_CON); }
inline BOOL	ItemIsOther_ClearInt(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_INT); }
inline BOOL	ItemIsOther_ClearSkill(DWORD type)	{ return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_CLEAR_SKILL); }
inline BOOL ItemIsOther_Spawn(DWORD type)       {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER2_SPAWN);}
inline BOOL ItemIsOther_NwTeleport(DWORD type)  {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER2_NW_TELEPORT);}
inline BOOL ItemIsOther_NwCallTeam(DWORD type)  {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER2_NW_CALLTEAM);}
inline BOOL ItemIsOther_NwSkill(DWORD type)     {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER2_NW_SKILL);}
inline BOOL ItemIsOther_BankExtend(DWORD type)  {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER_BANKEXTEND);}
inline BOOL ItemIsOther_Hero(DWORD type)        {   return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_OTHER2_HERO);}

inline BOOL ItemCanDelete(const SItemID& id)		{ return (id.type & ITEM_MASK_CAN_DELETE); }
inline BOOL ItemCanDrop(const SItemID& id)			{ return (id.type & ITEM_MASK_CAN_DROP); }
inline BOOL ItemIsMissionItem(const SItemID& id)	{ return (id.type & ITEM_MASK_MISSION_ITEM); }
inline BOOL ItemCanTrade(const SItemID& id)			{ return (id.type & ITEM_MASK_CAN_TRADE); }
inline BOOL	ItemCanPile(const SItemID& id)			{ return (id.type & ITEM_MASK_CAN_PILE); }
inline BOOL	ItemCanUpdate(const SItemID& id)		{ return (id.type & ITEM_MASK_CAN_UPDATE); }
inline BOOL ItemCanNpcTrade(const SItemID& id)		{ return (id.type & ITEM_MASK_CAN_NPCTRADE); } 

inline BOOL ItemCanDelete(DWORD type)		{ return (type & ITEM_MASK_CAN_DELETE); }
inline BOOL ItemCanDrop(DWORD type)			{ return (type & ITEM_MASK_CAN_DROP); }
inline BOOL ItemIsMissionItem(DWORD type)	{ return (type & ITEM_MASK_MISSION_ITEM); }
inline BOOL ItemCanTrade(DWORD type)		{ return (type & ITEM_MASK_CAN_TRADE); }
inline BOOL	ItemCanPile(DWORD type)			{ return (type & ITEM_MASK_CAN_PILE); }
inline BOOL	ItemCanUpdate(DWORD type)		{ return (type & ITEM_MASK_CAN_UPDATE); }
inline BOOL ItemCanNpcTrade(DWORD type)		{ return (type & ITEM_MASK_CAN_NPCTRADE); }

inline BOOL	ItemIsWhite(const SItemID& id)		{ return (id.color == ItemColor_White); }
inline BOOL	ItemIsGreen(const SItemID& id)		{ return (id.color == ItemColor_Green); }
inline BOOL	ItemIsBlue(const SItemID& id)		{ return (id.color == ItemColor_Blue); }
inline BOOL ItemIsGolden(const SItemID& id)		{ return (id.color == ItemColor_Golden); }

inline bool ItemID_IsValid(const SItemID& id)	{ return !(id.id==0 || id.type==0); }
inline void ItemID_SetInvalid(SItemID& id)	{ id.Clear(); }
inline SItemID ItemID_CreateInvalid()		{ SItemID id; return id; }

inline BOOL ItemIsPetNormal(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_NORMAL);}
inline BOOL ItemIsPetFairy(DWORD type)		{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_FAIRY);}
inline BOOL ItemIsPetSpectre(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_PET_SPECTRE);}

inline BOOL ItemIsSorptionTrump(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SORPTION_TRUMP);}
inline BOOL ItemIsSorptionPet(DWORD type)	{	return ( (type & ITEM_MASK_SUBTYPE) == ITEM_MASK_SORPTION_PET);}

class CItemIDManager
{
public:
	CItemIDManager();
	~CItemIDManager();
	bool	Init();
	DWORD	NewID();

private:
	DWORD	m_nMaxID;
};


// ���ߺϳ�
#define ITEMCOMPOSE_USE_TIME				3000	// �ϳ���Ʒ��ʱ(ms)
#define ITEMCOMPOSE_MAX_MATERIAL			8		// ��Ʒ�ϳ������Ҫ�����ֲ���

#define ITEMCOMPOSE_WEAPON_SKILL			583		// ������
#define ITEMCOMPOSE_EQUIP_SKILL				588		// ������
#define ITEMCOMPOSE_YAOPING_SKILL			584		// ������
#define ITEMCOMPOSE_ENCHANT_SKILL			585		// �Ʒ���
#define ITEMFUSE_SKILL						586		// �ں���
#define ITEMDECOMPOSE_SKILL					587		// �ֽ���
#define ITEMCOMPOSE_WEAPON_LEVEL1_SKILL		46
#define ITEMCOMPOSE_YAOPING_LEVEL1_SKILL	61
#define ITEMCOMPOSE_ENCHANT_LEVEL1_SKILL	76	
#define ITEMCOMPOSE_EQUIP_LEVEL1_SKILL		91	
#define ITEMFUSE_LEVEL1_SKILL				1297	// �ں���
#define ITEMDECOMPOSE_LEVEL1_SKILL			1305	// �ֽ���
#define MAX_UPDATE_ITEM_LEVEL				11

enum EItemComposeType
{
	ICT_Equipment				= 1,	// ����
	ICT_Yaoping					= 2,	// ����
	ICT_EnchantScroll			= 3,	// �Ʒ�
	ICT_Casting					= 4,	// ����
	ICT_Gem						= 5,	// ����
	ICT_Count					= 6
};
inline bool ItemComposeType_IsValid(EItemComposeType ict) { return (ict>0 && ict<ICT_Count); }

struct SItemCompose
{
	DWORD Idx;										// ID
	int ComposeType;								// �ϳ�����
	//char Name[PATH_STRING_LEN];
	CM_MEMDEF(Name, PATH_STRING_LEN)
	short Level;									// �������ߵĵȼ�
	short SkillLevel;								// ѧϰ�䷽����ļ��ܵȼ�
	DWORD Product;									// ��Ʒ����ID
	DWORD MaterialTypeCount;						// ��Ҫ���ϵ���������
	DWORD Materials[ITEMCOMPOSE_MAX_MATERIAL];		// ��Ҫ�Ĳ���item��ID
	DWORD MaterialNums[ITEMCOMPOSE_MAX_MATERIAL];	// ��Ҫ�Ĳ��ϵ�����
	double GreenRate;
	double BlueRate;
	double GoldenRate;
	//char Desc[DESC_STRING_LEN];
	CM_MEMDEF(Desc, DESC_STRING_LEN)

	//��Ӻϳ�ʧ�ܼ��� Cheney. 2009.03.18
	double FailRate;
	DWORD FaidItems;

	int    m_UpdateRate[MAX_UPDATE_ITEM_LEVEL];
	long   m_dwNeetMoney;

	SItemCompose(void)
	{
		CM_MEMPROTECTOR(Name, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Desc, DESC_STRING_LEN)
	}
	SItemCompose(const SItemCompose &object)
	{
		memcpy(this, &object, sizeof(SItemCompose));
		CM_MEMPROTECTOR(Name, PATH_STRING_LEN)
		CM_MEMPROTECTOR(Desc, DESC_STRING_LEN)
	}
	~SItemCompose(void)
	{
		CM_MEMUNPROTECTOR(Name)
		CM_MEMUNPROTECTOR(Desc)
	}
};

// ���ߺϳɱ��
class CItemComposeMgr
{
public:
	CItemComposeMgr(CItemManager* pItemMgr);
	~CItemComposeMgr();
	bool Init();
	SItemCompose* FindComposing(DWORD Id);
	bool CheckCompose(DWORD ComposeId, CItemContainerBase* pItemContainer, int* pCanMakeCount = NULL);

private:
	bool LoadResource(const char* FileName);
	EXT_SPACE::unordered_map<DWORD, SItemCompose> m_Composings;
	CItemManager* m_pItemMgr;
};


// ���߷ֽ�
#define ITEMDECOMPOSE_MAX_MATERIAL		8 // ��Ʒ�����Էֽ�Ĳ�������
int ItemDecompose_MaxItemLevel(int SkillLevel);

struct SItemDecompose
{
	DWORD Idx;												// ID
	DWORD MaterialTypeCount;								// �ֽ�Ĳ��ϵ���������
	DWORD Materials[ITEMDECOMPOSE_MAX_MATERIAL];			// �ֽ�Ĳ���item��ID
	DWORD MaterialNums_Green[ITEMDECOMPOSE_MAX_MATERIAL];	// �ֽ�Ĳ��ϵ�����
	DWORD MaterialNums_Blue[ITEMDECOMPOSE_MAX_MATERIAL];	// �ֽ�Ĳ��ϵ�����
	DWORD MaterialNums_Golden[ITEMDECOMPOSE_MAX_MATERIAL];	// �ֽ�Ĳ��ϵ�����
};

// ���߷ֽ���
class CItemDecomposeMgr
{
public:
	CItemDecomposeMgr();
	~CItemDecomposeMgr();
	bool Init();
	SItemDecompose* FindDecomposing(DWORD Id);

private:
	bool LoadResource(const char* FileName);
	EXT_SPACE::unordered_map<DWORD, SItemDecompose> m_Decomposings;
};


#define SUIT_ITEM_MAX_ITEMS			6			// ÿ����װ����м���
#define SUIT_ITEM_MAX_ATTRIBS		6			// ÿ����װ����м����ӳ�����
#define SUIT_DEFAULT_ATTRIB_RATE	100
struct SSuit
{
	DWORD			id;
	std::string		name;
	DWORD			attribID;
	std::string		effect;						// ��Ч�ļ�
	std::string		desc;						// ˵��

	SSuit()
	{
		id = 0;
		name = "";
		attribID = 0;
		effect = "";
		desc = "";
	}
};
// ��װ����
class CSuitItemManager
{
public:
	CSuitItemManager(CItemManager *pMgr);
	~CSuitItemManager();
	bool		Init();
	SSuit*		CheckSuit(SItemID pItems[], int count);
	SSuit*		GetSuit(DWORD suitID);

private:
	bool		LoadResource();

private:
	CItemManager*	m_pItemMgr;
	EXT_SPACE::unordered_map<DWORD, SSuit*> m_suits;
};

#define ATTRIB_GREEN_MIN_RATE		50
#define	ATTRIB_GREEN_MAX_RATE		100
#define ATTRIB_BLUE_MIN_RATE		75
#define	ATTRIB_BLUE_MAX_RATE		125
#define ATTRIB_GOLDEN_MIN_RATE		75
#define ATTRIB_GOLDEN_MAX_RATE		125
/// ׷�����Թ���
class CAttribMgr
{
public:
	enum ColorSelector
	{
		csGreen			= 0,
		csBlue1			= 10,
		csBlue2			= 11,
		csGolden1		= 20,
		csGolden2		= 21,
		csSuit			= 90,
//		csGolden		= ,
//		csGreen,
//		csPurple,
        csMax			= 99
	};
#define ITEM_MAX_GRADE_COUNT        99

	CAttribMgr();
	~CAttribMgr();
	bool		Init();
	bool		GetAttrib(WORD id, std::vector<SAttribAddtion>& arrAttribs);
	bool		IsAttribExist(WORD id);
	WORD		CreateAttrib(int selector, ColorSelector nColor);		// ��������ID
	BYTE		CreateAttribRate(ColorSelector color);

	void		GetPossibleAttribs(int selector, ColorSelector nColor, std::vector<WORD>& arrID);

private:
	bool		LoadResource();

	// Ϊ�˿�������һ���������ԣ� ������������
	// ��1ά�� ��ɫ����ɫ����ɫ����ɫ
	// ��2ά�� �����Զ������
	std::vector<WORD> m_finder[csMax][ITEM_MAX_GRADE_COUNT];
	EXT_SPACE::unordered_map<WORD, std::vector<SAttribAddtion> > m_attribs;		// ID(AttribID)��SAttribAddtion��ӳ��
};

// �������� "wow=World of Warcraft" �ľ���
bool AnalyseEqualClause(std::string& strClause, std::string& token, std::string& value);

/// ���ɵ��ߵĽṹ��
struct SCreateItem
{
	DWORD       index;
	int         level;
	EItemBind   cBind;
    char        color;
    BYTE        colorRate;      // ��ɫ���ߵ����Ա���
    WORD        colorAttrib1;   // ��ɫ���ߵ�����1
    WORD        colorAttrib2;   // ��ɫ���ߵ�����2
    DWORD       limitTime;      // ��Ч��(����)����SItemID�е�TimeLimit��ͬ

	SCreateItem() { index = 0; level = -1; cBind = IB_NotBind; color = -1; colorRate = 0; colorAttrib1 = 0; colorAttrib2 = 0; limitTime = 0; }
	bool IsValid(CItemManager* pItemMgr);
	bool SaveToString(CItemManager* pItemMgr, std::string& outStr);
	bool LoadFromString(CItemManager* pItemMgr, std::string& CreateString);
};

//Tianh ���ȼ������ͽ��з���ı��  ����δ���ʥ�׺�XX
enum EItemSelectionType
{
	IST_WEAPON_WARRIOR,
	IST_WEAPON_HUNTER,
	IST_WEAPON_TAOLIST,
	IST_WEAPON_WIZARD,
	IST_ARMOR_WARRIOR,
	IST_ARMOR_HUNTER,
	IST_ARMOR_TAOLIST,
	IST_ARMOR_WIZARD,
	IST_SHIPIN,
	IST_GEM,
	IST_WEAPON_ALL,
	IST_CLOTH_ALL,
	IST_GLOVE_ALL,
	IST_SHOE_ALL,
	IST_COUNT
};
enum EItemSelectionLevel
{
	ISL_1_9,
	ISL_10_19,
	ISL_20_29,
	ISL_30_39,
	ISL_40_49,
	ISL_50_59,
	ISL_60_69,
	ISL_70_79,
	ISL_80_89,
	ISL_90_99,
	ISL_COUNT
};

struct SItemSelector
{
	EItemSelectionLevel level;
	EItemSelectionType type;
};

class CItemSelectionTable
{
public:
	CItemSelectionTable();
	~CItemSelectionTable();
	bool Init(CItemManager *pItemMgr);

	void Explain_ItemSelectionType(EItemSelectionType ist, int& outType, int& outSubType, int& metier, std::string& str);
	void Explain_ItemSelectionLevel(EItemSelectionLevel isl, int& outLevelFrom, int& outLevelTo, std::string& str);

	bool FindSelection(SItemSelector& sel, std::vector<DWORD>& outItemIDs);

private:
	bool BuildTable(CItemManager *pItemMgr);
	std::vector<DWORD> m_Seletions[ISL_COUNT][IST_COUNT];
};

/// ��Ʒ����
class CItemManager
{
public:
	CItemManager();
	~CItemManager();
	bool		Init();
	SItem*		GetItem(const SItemID& id);
	SItemBase*	GetItem(RtString name);
	SItemBase*	GetItem(DWORD type);
	SItemBase*	GetItemFromIndex(DWORD index);
	SItemID		CreateItem(RtString name, EItemColor color = ItemColor_White, BYTE level = 0, EItemBind cBind = IB_NotBind, WORD ColorAttribID1 = 0, BYTE ColorRate1 = 0, WORD ColorAttribID2 = 0, BYTE ColorRate2 = 0, DWORD TimeLimit = 0);	// �������ִ���
	SItemID		CreateItem(DWORD index, EItemColor color = ItemColor_White, BYTE level = 0, EItemBind cBind = IB_NotBind, WORD ColorAttribID1 = 0, BYTE ColorRate1 = 0, WORD ColorAttribID2 = 0, BYTE ColorRate2 = 0, DWORD TimeLimit = 0);	// ���ݱ������ƷID����
	SItemID		CreateItem(SItemID& item);														// �����Ѵ��ڵ���Ʒ��¡һ���µ�
	void		DeleteItem(SItemID& id);
	static int	GetIndexFromType(DWORD type) {	return (type&ITEM_MASK_INDEX) >> 8; }
	int			GetItemValue(SItemID &item);
	float		GetItemPoint(SItemID &item);	//heten
	static void GetTableType(DWORD type, int& outTableType, int& outTableSubType);

	bool		GenerateCreateString(std::vector<SCreateItem>& arrCreateItem, std::string& outCreateString);
	bool		AnalyseCreateString(std::string& CreateString, std::vector<SCreateItem>& outArrayCreateItem);
	SItemID		CreateItemByStruct(SCreateItem& ci);

	// ע����������
	bool		InfuseColor(SItemID& item, EItemColor color, WORD attribID1 = 0, BYTE ColorRate1 = 0, WORD attribID2 = 0, BYTE ColorRate2 = 0);

	CSuitItemManager*		m_pSuitManager;
	CAttribMgr*				m_pAttribMgr;
	CItemSelectionTable*	m_pSelectionTable;
	CItemComposeMgr*		m_pComposeMgr;
	CItemDecomposeMgr*		m_pDecomposeMgr;
    CMagicEggRewardMgr*     m_pMagicEggRewardMgr;

    int         GetBankInitLineCount() { return m_BankInitLineCount; }
    int         GetBankMaxLineCount() { return m_BankMaxLineCount; }

private:
	SItemID		_CreateItem(DWORD type, SItemBase *pBase, EItemColor color = ItemColor_White, BYTE level = 0, EItemBind cBind = IB_NotBind, WORD ColorAttribID1 = 0, BYTE ColorRate1 = 0, WORD ColorAttribID2 = 0, BYTE ColorRate2 = 0, DWORD TimeLimit = 0);

	bool		LoadResource();

    int         m_BankInitLineCount;
    int         m_BankMaxLineCount;
	int			m_PointValue;

	EXT_SPACE::unordered_map<DWORD, SItemBase*> m_mapItems;			// type(����index)��SItemBase��ӳ��
	EXT_SPACE::unordered_map<DWORD, SItemBase*> m_mapIndexToItem;	// index��SItemBase��ӳ��

#define ITEM_MAX_CLASS_COUNT		2000
	SItem				m_ItemInstance[ITEM_MAX_CLASS_COUNT];
	int					m_classCounter;

	CItemIDManager*		m_pIDManager;

	friend class CItemSelectionTable;
};


enum EItemContainer
{
	ITEM_CONTAINER_PLAYER_COMMON_BAG = 0x01,			// �Լ�������
	ITEM_CONTAINER_PLAYER_MISSION_BAG = 0x02,			// �Լ�������
	ITEM_CONTAINER_TRADE_OTHER_ITEMS = 0x03,			// ��������ҽ���
	ITEM_CONTAINER_TRADE_SELF_ITEMS = 0x04,				// ��������ҽ���
	ITEM_CONTAINER_NPCSHOP_SELL_ALBUM = 0x05,			// ��NPC����--NPC�ĳ�����Ʒ
	ITEM_CONTAINER_NPCSHOP_SHOPPING_BAG = 0x06,			// ��NPC����--�����
	ITEM_CONTAINER_NPCSHOP_OWN_ALBUM = 0x07,			// ��NPC����--�Լ������пɳ�����Ʒ
	ITEM_CONTAINER_NPCSHOP_SELL_BAG = 0x08,				// ��NPC����--���۴�
	ITEM_CONTAINER_PLAYERSHOP_SELLLIST_BAG = 0x09,		// ��̯
	ITEM_CONTAINER_PLAYERSHOP_SELF_BAG = 0x0a,			// ��̯
	ITEM_CONTAINER_PLAYER_PET_BAG = 0x0b,				// �Լ��ĳ��ﱳ��
	ITEM_CONTAINER_TRADE_OTHER_PET = 0x0c,				// ��������ҽ��׳���
	ITEM_CONTAINER_TRADE_SELF_PET = 0x0d				// ��������ҽ��׳���
};

#define ITEM_BAG_SAVE_STRING_BUFFER_LENGTH	3072

struct IOnItemChange
{
	virtual void OnSetItem(const SItemID &idS, const SItemID &idD) = 0;
	virtual void OnRemoveItem(const SItemID &idS) = 0;
};

class CBag
{
public:
#define MAX_BAG_PAGE_COUNT						2
#define	MAX_BAG_LINE_COUNT						16
#define MAX_BAG_COLUMN_COUNT					16

#define MAX_PILE_COUNT							255//9		// ÿ�����ӿ����ɵĿɶѵ���Ʒ������

	enum EItemAssistantAttrib
	{
		IAA_Visible		= 0,
		IAA_Lock		= 1,
		IAA_Count				// ����С��8--sizeof(BYTE)
	};

public:
	friend class CItemContainerBase;

	CBag(int nLineCount, int nColumnCount, int nPageCount, CItemManager* pItemMgr);
	~CBag();

	struct BagIter
	{
		int p, i, j;
	};
	//BagIter		GetFirst() const;
	//bool		GetNext(BagIter& iter) const;
	//SItemID		GetItem(const BagIter iter) const;

	// ���̣�
	const char*		SaveToString(CItemManager *pItemMgr);
	int				LoadFromString(const char* str, CItemManager *pItemMgr);

	//Tianh ����װ��ID����
	const char*     SaveToStringbyID(CItemManager *pItemMgr,DWORD ID);
	/*SItemID         LoadFromStringbyBD(const char* str, CItemManager *pItemMgr);*/

	bool		Serialize(char type, CG_CmdPacket *cmd);

    bool        Resize(int page, int line, int col);

	bool		IsFull() const				// �����Ƿ���(�п��������ĸ����Ѿ�����,���ǻ����Է���Ѫƿ�ȿɶѵ���Ʒ)
	{
		if (m_nItemCount==m_nPageCount*m_nLineCount*m_nColumnCount)	return true;
		else	return false;
	}
	bool		IsEmpty() const				// �����Ƿ��
	{
		if (m_nItemCount==0)	return true;
		else	return false;
	}
	int			Count() const { return m_nItemCount; }			// �õ�������Ʒ������
	int			FreeSpace() const { return (m_nPageCount*m_nLineCount*m_nColumnCount - m_nItemCount); }		// ʣ������
	int			GetVisibleMaxLine() const;		// �е��ߴ��ڵ������
	void		Clear();					    // �������
	void        ItemUpdateQuickSort(std::vector< SItemID > & item,int left,int right);//��������������� add by ���� 2010.05.10
	void        ItemPile();//����������Ʒ�ѵ� add by ���� 2010.05.10
	void        TidyUpBag();//�������� add by ���� 2010.05.11
	bool		MoveToBag(CBag* pToBag, DWORD typeMask = 0xffffffff);	// �Ѵ������е�������Ʒ�ƶ�������һ��������
	bool		MoveToContainer(CItemContainerBase *pCon, DWORD typeMask =0xffffffff);
	bool		AddToBag(CBag* pToBag, DWORD typeMask = 0xffffffff) const;	// �Ѵ������е�������Ʒ��ӵ�����һ��������
	bool		AddToContainer(CItemContainerBase *pCon, DWORD typeMask =0xffffffff) const;
	bool		RemoveFromBag(CBag *pBag);						// pBag�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����
	bool		RemoveFromArray(vector<SItemID> &arrItems);		// array�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����
	bool		RemoveFromArray(vector<DWORD> &arrItemID);		// array�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����
	bool		IsContain(CBag *pBag) const;					// �������Ƿ����pBag�е�������Ʒ
	bool		IsContain(vector<SItemID> &arrItems) const;		// �������Ƿ����arrItems�е�������Ʒ
	bool		IsContain(vector<DWORD> &arrItemID) const;		// �������Ƿ����arrItemID�е�������Ʒ
	bool		Find(SItemID itemID, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL) const;		// ����һ����Ʒ(SItemID)
	bool		Find(DWORD ID, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL) const;			// ����һ����Ʒ(SItemID.id)
	bool		FindType(DWORD typeID, EItemColor color = ItemColor_All, int *pOutCount = NULL, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL) const;		// ����һ�����͵���Ʒ������
	bool		FindTypeWithParams(DWORD typeID, std::vector<DWORD>& paramsIndices, std::vector<DWORD>& paramsCheck, EItemColor color = ItemColor_All, int *pOutCount = NULL, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL) const;		// ����һ�����͵���Ʒ(���Բ��ҵ����ض��Ĳ���)������
	bool		FindID(DWORD ID, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL) const;			// ����һ����Ʒ(SItemID.id)
	void		ToItemsArray(vector<SItemID> &arrItems) const;				// ��������Ʒ���������
	void		ToItemIDArray(vector<DWORD> &arrItemID) const;				// ��������Ʒ���������

	SItemID		GetItemFromID(DWORD ID) const;				// ����һ����Ʒ(SItemID.id)
	SItemID*	GetItemPointFromID(DWORD ID);				// ���һ����Ʒ��ָ��

	SItemID		GetItem(int i, int j) const;
	int			GetItemCount(int i, int j) const;		// �ɶѵ���Ʒ������
	bool		RemoveItem(int i, int j);
	bool		MoveItem(int fromI, int fromJ, int toI, int toJ);
	bool		MoveItem(CBag* fromContainer, int fromI, int fromJ, CBag* toContainer, int toI, int toJ);
	bool		ResolveItem(int fromPage, int fromI, int fromJ, int toPage, int toI, int toJ,int Num);//Tianh ������Ʒ
	bool		AddItem(SItemID& id, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL, bool bPileIfPossible = true);	// ��һ��������ӵ���������,�ŵ���һ���ʺϵĸ�����
	bool		SetItem(int i, int j, SItemID id);		// ����ԭ���������Ƿ�����Ʒ���������ԭ���ģ�������Ϊ����Ʒ
	bool		AddItem(int i, int j, SItemID id);		// ���ԭ������������Ʒ�������������ԣ�������
	//bool        SetItem(int i, int j, int Num); //Tianh 10.03.12 ����������Ʒ
	long        GetNumBytype(DWORD type, WORD cBind);  //Tianh �õ�������͵�������Ʒ��������  ���ְ󶨺Ͳ���
	void        SetSell(DWORD type, WORD cBind,long num);//Tianh  ����Ʒ�����⴦��
	
	//Tianh     ��Ʒ����
	bool        ResolveItem(CBag* fromContainer,int fromPage, int fromI, int fromJ, CBag* toContainer,int toPage, int toI, int toJ, int Num/*��������*/);
	SItemID		GetItem(int page, int i, int j) const;
	int			GetItemCount(int page, int i, int j) const;
	bool		RemoveItem(int page, int i, int j);
	bool		RemoveItem(const SItemID& item);
	bool		RemoveItem(DWORD typeID, EItemColor color = ItemColor_All, int count = -1);	// ɾ���������͵�count��������Ʒ(���count=-1,��ɾ������)
	bool		RemoveItemWithParams(DWORD typeID, std::vector<DWORD>& paramsIndices, std::vector<DWORD>& paramsCheck, EItemColor color = ItemColor_All, int count = -1);	// ɾ���������͵�count��������Ʒ(���count=-1,��ɾ������)
	bool		MoveItem(int fromPage, int fromI, int fromJ, int toPage, int toI, int toJ);
	bool		MoveItem(CBag* fromContainer, int fromPage, int fromI, int fromJ, CBag* toContainer, int toPage, int toI, int toJ);
	bool		SetItem(int page, int i, int j, const SItemID& id);
	//bool      SetItem(int page,int i, int j, int Num); //Tianh 10.03.12 ����������Ʒ
	//bool		AddItem(int page, int i, int j, const SItemID& id);

	int			MoveToNextPage() { m_nCurrentPage++; if (m_nCurrentPage>=m_nPageCount) m_nCurrentPage=0; return m_nCurrentPage; }
	int			MoveToPrePage() { m_nCurrentPage--; if (m_nCurrentPage<0) m_nCurrentPage=m_nPageCount-1; return m_nCurrentPage; }
	void		SetPageIndex(int page){if(m_nCurrentPage>= 0 && m_nCurrentPage< m_nPageCount)m_nCurrentPage= page;}
	bool		_IsValidIndex(int i, int j) const { return (i>=0 && i<m_nLineCount && j>=0 && j<m_nColumnCount); }
	bool		_IsValidIndex(int page, int i, int j) const { return (page>=0 && page<m_nPageCount && i>=0 && i<m_nLineCount && j>=0 && j<m_nColumnCount); }
	int			GetPageIndex() const { return m_nCurrentPage; }
	int			GetPageCount() const { return m_nPageCount; }
	int			GetLineCount() const { return m_nLineCount; }
	int			GetColCount()  const { return m_nColumnCount; }
	int			GetMoney() const { return m_nMoney; }
	int			GetPoint() const { return m_nPoint; }	//heten
	void		SetMoney(int count);
	bool		AddMoney(int count);
	bool		AddPoint(int count);	//heten
	bool		RemoveMoney(int count);
	bool		RemovePoint(int count);	//heten

	bool        BuyTidy(SItemID& item); //Tianh ����������Ʒ��ʱ���õĵ���

	void		SetVisible(int page, int i, int j, bool bVisible)
	{
		if (bVisible)
			m_AssistantAttrib[page][i][j]	|= 1<<IAA_Visible;
		else 
			m_AssistantAttrib[page][i][j]	&= ~(1<<IAA_Visible);
	}
	bool		GetVisible(int page, int i, int j)  const { return (m_AssistantAttrib[page][i][j] >> IAA_Visible)&0x01; }
	void		SetLock(int page, int i, int j, bool bLock)
	{
		if (bLock)
			m_AssistantAttrib[page][i][j]	|= 1<<IAA_Lock;
		else 
			m_AssistantAttrib[page][i][j]	&= ~(1<<IAA_Lock);
	}
	bool		GetLock(int page, int i, int j) const { return (m_AssistantAttrib[page][i][j] >> IAA_Lock)&0x01; }
	void		SetAllVisible(bool bVisible);
	void		SetAllLock(bool bLock);

	void		Pinch();			// ���������ڷ�

	bool		CloneFrom(CBag* pBag); // 

	CItemManager* m_pItemMgr;
	
	bool CheckIsHuiyuan(DWORD typeID);
	void SetIOnItemChange(IOnItemChange* pIOnItemChange){m_pIOnItemChange = pIOnItemChange;}
	int GetBagItemNum(){ return m_nItemCount; }
private:
	bool		AddPileItem(SItemID& item, int *pOutPage = NULL, int *pOutI = NULL, int *pOutJ = NULL);

	int	        m_nLineCount;			// container������--��
	int	        m_nColumnCount;			// container������--��
	int	        m_nPageCount;			// container������--ҳ

	int			m_nCurrentPage;			// ��ǰ������һҳ
	int			m_nItemCount;			

	long		m_nMoney;
	long		m_nPoint;
	SItemID		m_nItemID[MAX_BAG_PAGE_COUNT][MAX_BAG_LINE_COUNT][MAX_BAG_COLUMN_COUNT];		// ��ɫ�����е���ƷID
	BYTE		m_AssistantAttrib[MAX_BAG_PAGE_COUNT][MAX_BAG_LINE_COUNT][MAX_BAG_COLUMN_COUNT];// ��������

	SItemID		_GetItem(int i, int j) const {	return _GetItem(m_nCurrentPage, i, j);	}
	void		_SetItem(int i, int j, const SItemID& id) { _SetItem(m_nCurrentPage, i, j, id); }
	void		_RemoveItem(int i, int j) { _RemoveItem(m_nCurrentPage, i, j); }

	SItemID		_GetItem(int page, int i, int j) const {	return m_nItemID[page][i][j];	}
	void		_SetItem(int page, int i, int j, const SItemID& id)
	{
		
		SItemID itemOrig = _GetItem(page, i, j);
		
		m_nItemID[page][i][j] = id;
		if ( !ItemID_IsValid(itemOrig) && ItemID_IsValid(id))
			m_nItemCount++;
		else if ( ItemID_IsValid(itemOrig) && !ItemID_IsValid(id) )
			m_nItemCount--;
		if (NULL != m_pIOnItemChange)
		{
			m_pIOnItemChange->OnSetItem(itemOrig, id);
		}
	}
	void		_RemoveItem(int page, int i, int j)
	{
		SItemID itemOrig = _GetItem(page, i, j);
		m_nItemCount--;
		m_nItemID[page][i][j].Clear();
		if (NULL != m_pIOnItemChange)
		{
			m_pIOnItemChange->OnRemoveItem(itemOrig);
		}
	}
	IOnItemChange* m_pIOnItemChange;
};
bool operator==(const CBag &bag1, const CBag &bag2);
bool operator!=(const CBag &bag1, const CBag &bag2);

#define ITEM_PLAYER_INVENTORY_LINE		6
#define ITEM_PLAYER_INVENTORY_COLUMN	7
#define ITEM_PLAYER_INVENTORY_PAGE		2
#define ITEM_PLAYER_MISSION_PAGE		1

#define ITEM_PET_BAG_COLUMN		11
#define ITEM_PET_BAG_LINE		1
#define ITEM_PET_BAG_PAGE		1

#define ITEM_CONTAINER_SAVE_STRING_BUFFER_LENGTH	(ITEM_BAG_SAVE_STRING_BUFFER_LENGTH*2 + 1024)
// ��ҵ�����װ��
class CItemContainerBase
{
public:
	enum EEquipParts
	{
		EQUIP_DEFAULT_PART = -1,		// Ĭ�ϵ�װ����λ
		WEAPON_1=0,	//������λ1
		WEAPON_2,	//������λ2
		WEAPON_3,	//��������λ

		NECKLACE,	//����
		GLOVE,		//����
		RING,		//��ָ
		BODY,		//����
		FOOT,		//��
		CAESTUS,	//����
		RING2,		//��ָ2
		KITS,		//����

		CHARM,		//����
		TRUMP,		//����
		FASHION,	//ʱװ
		SEALEDBOOK,	//����
		AEROCRAFT,	//������
		MAX_EQUIP_PARTS
	};

public:
	CBag			m_Bag;									// ��ɫ������ĵ���
	CBag			m_MissionBag;							// ��ɫ������ĵ��������

	CBag			m_PetBag;								// ��ɫ�ĳ��ﱳ��

	SSuit*			m_pSuit;								// ��װ�����Ϊ�գ��ͱ�ʾû����װ��

	CItemContainerBase(CItemManager *pManager, SCreature *pCre);
	virtual ~CItemContainerBase();
	bool			Serialize(char type, CG_CmdPacket *cmd);
	bool			Serialize(RtArchive* pFile);
    void            Clear();

	// ���̣�
	const char*		SaveToString();
	int				LoadFromString(char* str);
	const char*		SavePetBagToString();
	int				LoadPetBagFromString(char* str);

	bool			GetEquipItem(EEquipParts vPart, SItemBase*& vrpItem, bool bIgnoreValid = false) const;
	SItemID			GetEquipItem(EEquipParts vPart, bool bIgnoreValid = false) const;
	SItemID*		GetEquipItemPoint(EEquipParts vPart, bool bIgnoreValid = false);
	bool			SetEquipItem(EEquipParts vPart, SItemID& item);
	bool			FindInEquipItems(SItemID& item, EEquipParts &part);
	bool			FindInEquipItems(DWORD id, EEquipParts &part);

	bool			IsEquip(EEquipParts vPart) const;
	virtual bool	CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre) const;			// ���װ�����ߵ������Ƿ�����
	virtual bool	CheckUse(SItemID& item, SCreature &cre) const;								// ���ʹ�õ��ߵ������Ƿ�����
	bool			Equip(SItemID& id, EEquipParts vPart = EQUIP_DEFAULT_PART, bool bRemoveFromBag = false, SItemID *pItemUnEquip1 = NULL, EEquipParts *pPartUnEquip1 = NULL, SItemID *pItemUnEquip2 = NULL, EEquipParts *pPartUnEquip2 = NULL);
	bool			UnEquip(EEquipParts vPart, bool bAddToBag = false);
	virtual bool	UnEquipAll(bool bAddToBag = false);
	EEquipParts		GetDefaultEquipPart(SItemID &item);
	int				GetUnEquipItemCount(SItemID &item, EEquipParts vPart = EQUIP_DEFAULT_PART);	// ���װ��item��������Ҫ�ѵ�����װ����0/1/2��
    bool            GetEquipValid(EEquipParts part) { return m_bEquipValid[part]; }

	virtual bool	UsePileItem(int page, int i, int j);
	bool			AddMoney(int count);
	bool			RemoveMoney(int count);
	int				GetMoney()	{ return m_Bag.GetMoney(); }
	void			SetMoney(int count) { m_Bag.SetMoney(count); }
	bool			AddItem(SItemID& item);
	bool			AddItem(vector<SItemID>& arrItems);
	bool			RemoveItem(const SItemID& item);

	bool			_IsValidPart(EEquipParts vPart) const { return (vPart>=WEAPON_1 && vPart<MAX_EQUIP_PARTS); }

	bool			RemoveFromBag(CBag *pBag);						// pBag�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����
	bool			RemoveFromArray(vector<SItemID> &arrItems);		// array�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����
	bool			RemoveFromArray(vector<DWORD> &arrItemID);		// array�е�������Ʒ������ڶ����д��ڣ��Ͱ���ɾ����

	bool			IsContain(CBag *pBag) const;					// �������Ƿ����pBag�е�������Ʒ
	bool			IsContain(vector<SItemID> &arrItems) const;		// �������Ƿ����arrItems�е�������Ʒ
	bool			IsContain(vector<DWORD> &arrItemID) const;		// �������Ƿ����arrItemID�е�������Ʒ

	//PZH
	bool			LoseEquip(EEquipParts vPart, int nNum = 1);
	//
	bool			SetEquipWear(EEquipParts vPart, int nNum = 0);
	void			SetEquipValid(EEquipParts vPart, bool Valid);
	static bool		ExchangeItems(CBag* pBagFrom, CBag* pBagTo, vector<DWORD>& arrItems);
	static bool		ExchangeItems(CBag* pBagFrom, CItemContainerBase* pConTo, vector<DWORD>& arrItems);
	static bool		ExchangeItems(CItemContainerBase* pCon1, vector<SItemID>& arrItems1, CItemContainerBase* pCon2, vector<SItemID>& arrItems2);
	CItemManager*   GetItemManager(){return m_pItemManager;}
protected:
	BOOL			ItemCanBeEquiped(SItemID& item, EEquipParts part) const;
	virtual void	_Equip(SItemID& id, EEquipParts vPart);
	virtual bool	_UnEquip(EEquipParts vPart, bool bAddToBag = false);
	virtual void	_UpdateCreature(bool bEquip, SItemID& item, EEquipParts part);		// Equip��UnEquipʱ���ã�������������

	virtual void	OnAfterEquip(SItemID& item, EEquipParts vPart);
	virtual void	OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld);

	SCreature*		m_pCre;									// ������������
	CItemManager*	m_pItemManager;							// ��Ʒ�������Ӧ����ȫ��Ψһ�ģ�                             //
	bool			m_bEquipValid[MAX_EQUIP_PARTS];			// װ���Ƿ���Ч

	SSuit*			m_pLastSuit;							// ��װ����жװʱҪ���ô�ֵ
	SItemID			m_nItemID[MAX_EQUIP_PARTS];				// ��ɫ���ϵ���ƷID


};
bool operator==(const CItemContainerBase &con1, const CItemContainerBase &con2);
bool operator!=(const CItemContainerBase &con1, const CItemContainerBase &con2);

struct SDWORDArray
{
	vector<DWORD> array;
	bool Serialize(CG_CmdPacket *cmd);
};


// �õ�������߶������ӵ�����ֵ
void ItemUpdate_GetExtraAddtion(SItemID& item, CItemManager *pItemMgr, float& fDamage, float& fAttack, float& fDodge, float& fArmor, float& fElementArmor);
// �õ�������߶������ӵ�����
void ItemUpdate_GetExtraRequirement(SItemID& item, CItemManager *pItemMgr, float& fStr, float& fHit, float& fInt, float& fDex);
// �õ�����װ���Ķ�������
void ColorItem_GetExtraRequirement(CItemManager* pItemMgr, SItemID& item, int& iStr, int& iDex, int& iInt, int& iHit);
// �õ�һ�������ܵ�����
void GetItemAttribRequirement(CItemManager* pItemMgr, SItemID& item, int& iStr, int& iDex, int& iInt, int& iHit, int& iCon, int& iLevel, int& p_iMaxLevel);
// �õ����������Ҫ�Ľ�Ǯ
void ItemUpdate_GetMoneyRequirement(SItemID& item, int &money, CItemManager* pItemMgr);


#ifndef BOOL_ENSURE
#define BOOL_ENSURE(p)		{ if ( !(p) ) return false; }
#endif




#define BANK_GRID_LINE_COUNT		4
#define BANK_GRID_COLUMN_COUNT		7
#define BANK_GRID_LINE_INIT_COUNT   8
#define BANK_LINE_MAX				11

#define BANK_PET_COLUMN_COUNT		5
#define BANK_PET_LINE_COUNT			1


#define TRADE_GRID_LINE_COUNT		3
#define TRADE_GRID_COLUMN_COUNT		7


#define SHOP_GRID_LINE_UP_COUNT		5
#define SHOP_GRID_LINE_DOWN_COUNT	3
#define SHOP_GRID_COLUMN_COUNT		8
#define SHOP_MAX_LINE				15


#define PSHOP_GRID_LINE_COUNT		5
#define PSHOP_GRID_COLUMN_COUNT		7
#define PSHOP_LINE_MAX				5

#define PSHOP_GRID_LINE_COUNT_BUY	2
#define PSHOP_GRID_COLUMN_COUNT_BUY	7
#define PSHOP_LINE_MAX_BUY			2

const float itemshop_buy_pricerate	= 1.0000001f;
const float itemshop_sell_pricerate = 0.5000001f;


enum EItemDropStyle
{
	ItemDrop_ParabolaNear	= 1,		// С������
	ItemDrop_ParabolaFar	= 2,		// ��������
	ItemDrop_Descend		= 3			// ֱ������
};

// magic weapon in item
const int MW_MODEL_NUM		= 1;

const int MW_PARAM_SIZE			= 6;
const int MW_PARAM_LEV			= 0;
const int MW_PARAM_EXP			= 1;
const int MW_PARAM_MP			= 2;
const int MW_PARAM_ELE_WATOR	= 3;
const int MW_PARAM_ELE_FIRE		= 4;
const int MW_PARAM_ELE_POISON	= 5;

const int MW_PARAM_ELE			= 3;
const int MW_PARAM_MAX_LEV		= 4;

// �ٻ����������
const int ITEMSPAWN_PARAM_SIZE  = 1;
const int ITEMSPAWN_PARAM_LIFE  = 0; //��Ч��

// ��������
const int ITEMHERO_PARAM_SIZE   = 1;
const int ITEMHERO_PARAM_LIFE   = 0;

// 51�����������
const int ITEMSHIPIN_PARAM_SIZE = 1;
const int ITEMSHIPIN_PARAM_LIFE = 0;

const int MAX_MONEY_COUNT				= 999999999;
const int MAX_POINT_COUNT				= 999999999;
bool MoneyIsValid(long money);


#define ITEM_TWOBIAN_SKILLID_DAMAGE          104
#define ITEM_TWOBIAN_SKILLID_ATTACK          137


// (���ߴ���(����������)) �ɹ����ʣ��������ʣ���Ҫ�������
const float  g_itemupdate_successRate[ITEM_MAX_ITEM_LEVEL] = {1.0f, 1.0f, 1.0f, 0.80f, 0.80f, 0.30f, 0.11f, 0.11f, 0.11f, 0.00f};
const float  g_itemupdate_brokenRate[ITEM_MAX_ITEM_LEVEL] = {0.0f, 0.0f, 0.0f, 0.30f, 0.30f, 0.30f, 0.6f, 0.6f, 0.6f, 1.0f};
const int    g_itemupdate_moneyRequireWeapon[ITEM_MAX_ITEM_LEVEL] = {1000, 2000, 3000, 3000, 3000, 3000, 7000, 8000, 9000, 10000};
const int    g_itemupdate_moneyRequireArmor[ITEM_MAX_ITEM_LEVEL] = {500, 1000, 1500, 1500, 1500, 1500, 3500, 4000, 4500, 5000};
//const float	 g_itemupdate_addtionRate[ITEM_MAX_ITEM_LEVEL] = {1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 2.0f, 3.0f, 3.0f, 3.0f, 3.0f};

//����ƿ�Ŀ����ɵľ���
const int BOTTLE_EXP_MAX = 20000000;
const int TIANYANCHUXING = 100000000;

//�۵��������
enum  DeductType
{
	DEDUCT_TYPE_BUYITEM = 0,
	DEDUCT_TYPE_WORLD_MESSAGE = 1,
};


// �ʵ��Ľ�������
enum EMagicItemResult
{
    MI_Null,
    MI_Item,
    MI_Exp,
    MI_Money,
	MI_ALL,
	MI_SOUL
};

// �ʵ��Ľ���
struct SMagicItemReward
{
public:
    DWORD GroupID; // ����߱�Ĳ���link

    double CurrHitNum;

	SMagicItemReward() { Clear(); }
	~SMagicItemReward() { }
	void Clear()
	{
        GroupID = 0;
        CurrHitNum = 0.0;
        RewardType.clear();
        HitNum.clear();
        RewardParam.clear();
		GreenRate.clear();
		BlueRate.clear();
		GoldenRate.clear();
		vMaxCount.clear();
	}
	void AddReward(EMagicItemResult type, double rate, DWORD param, double _GreenRate = 0.0, double _BlueRate = 0.0, double _GoldenRate = 0.0 ,int MaxCount = 0)
	{
        CurrHitNum += rate;
        if (CurrHitNum < 0.0)
            CurrHitNum = 0.0;
        if (CurrHitNum > 100.0)
            CurrHitNum = 100.0;
        RewardType.push_back(type);
        HitNum.push_back(CurrHitNum);
        RewardParam.push_back(param);
		GreenRate.push_back(_GreenRate/100.0);
		BlueRate.push_back(_BlueRate/100.0);
		GoldenRate.push_back(_GoldenRate/100.0);
		vMaxCount.push_back(MaxCount);
	}

public:
    std::vector<EMagicItemResult> RewardType;
    std::vector<double> HitNum; // 0-100
	std::vector<DWORD> RewardParam;
	std::vector<double> GreenRate;
	std::vector<double> BlueRate;
	std::vector<double> GoldenRate;
	std::vector<int>    vMaxCount;
};

class CMagicEggRewardMgr
{
public:
    CMagicEggRewardMgr(CItemManager* pItemMgr);
    ~CMagicEggRewardMgr();
    bool Init();
    SMagicItemReward* FindReward(DWORD GroupID);

private:
    bool LoadResource(const char* FileName);
    EXT_SPACE::unordered_map<DWORD, SMagicItemReward> m_Rewards; // GroupID -> ����
    CItemManager* m_pItemMgr;
};


enum ETradableResult
{
	TR_Can,
	TR_CanNot_Unkown,
	TR_CanNot_Essence,
	TR_CanNot_Pet
};

// �����Ƿ�������(�������һЩ�����Ƿ���ҪLog����)
bool ItemIsRare(SItemID &item, CItemManager *pItemMgr);

int GetItemSerializeVersion();

// �ֿ�����
#define BANKPASS_MIN_COUNT			1
#define BANKPASS_MAX_COUNT			8

// �������޵ĵ���ID
#define PERFECT_REBIRTH_ITEM        6220

// �������޵ĵ���ID
#define SCAPEGOAT_ITEM              1084

// ������޵ĵ���ID
#define REFINEGOAT_ITEM             4518
#define REFINEGOAT_ITEM_SET			4519
#define REFINEGOAT_ITEM_SET_BIND	4565

//��������
#define DOBIND_ITEM					4520 //�󶨷�
#define DISBINDE_ITEM				4521 //����				
#define REBUILD_ITEM				4522 //����ȯ
#define REBUILD_ITEM_BIND			4562 //����ȯ
#define REBUILD_ITEM_SET_BIND		4625 //����ȯ
#define SUPERPLUS10_ITEM			4523 //����+10ȯ
#define PLUS10_ITEM					4524 //+10ȯ

//Ԫ��������						
#define ELE_REFINE_ITEM_MIN			4533
#define ELE_REFINE_ITEM_MAX			4535

//װ��������
#define EQUIP_LEVELUP_ITEM_MIN      20000
#define EQUIP_LEVELUP_ITEM_MAX		25000

#endif

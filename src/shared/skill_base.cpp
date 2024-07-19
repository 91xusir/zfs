
#include "core/rt2_core.h"
#include "region_client.h"
#include "string_mapping.h"
#include "rt1tort2.h"

//////////////////////////////////////////////////////////////////////////
// Filter & Effect
/* gao 2009.12.9
为方便后面的使用，将部分实现的结构体，枚举放到了头文件中
*/
//char g_szSkillErrMsg[100];
G_MEMDEF(g_szSkillErrMsg, 100)

SSkillFilterMap g_mapSkillFilter[] =
{
    { ESFT_OI, ESVF_NONE,           "none"          , "不允许的条件表达式"          , ""                            },
    { ESFT_OI, ESVF_G_LEVEL,        "Level"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_HP,           "HP"            , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_MP,           "MP"            , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_RIDE,         "IsRide"        , "%s"                          , "%s\n"                        },
    { ESFT_OI, ESVF_G_WeaponType,   "WeaponType"    , "%s [%s]"                     , "%s [%s]\n"                   },
	{ ESFT_OI, ESVF_G_WeaponEquiped,"WeaponEquiped" , "%s"                          , "%s\n"                        },
	{ ESFT_OI, ESVF_G_TrumpEquiped,	"TrumpEquiped"  , "%s"                          , "%s\n"                        },
    { ESFT_OI, ESVF_G_ModelID,      "ModelID"       , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Cloth,        "Cloth"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_ClothLev,     "ClothLev"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_ArmGuard,     "ArmGuard"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_ArmGuardLev,  "ArmGuardLev"   , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Shoes,        "Shoes"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_ShoesLev,     "ShoesLev"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Weapon,       "Weapon"        , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_WeaponLev,    "WeaponLev"     , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_MaxMP,        "MaxMP"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Attack,       "Attack"        , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Dodge,        "Dodge"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_DamageMin,    "DamageMin"     , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_DamageMax,    "DamageMax"     , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Armor,        "Armor"         , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_DamageRate,   "DamageRate"    , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Con,          "Con"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Str,          "Str"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Dex,          "Dex"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Int,          "Int"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Hit,          "Hit"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_AttSpeed,     "AttSpeed"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Metier,       "Metier"        , "%s [%s]"                     , "%s [%s]\n"                   },
    { ESFT_OI, ESVF_G_MetierLev,    "MetierLev"     , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Faction,      "Faction"       , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_Exp,          "Exp"           , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_AttrPoint,    "AttrPoint"     , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
	{ ESFT_OI, ESVF_G_SkillPoint,   "SkillPoint"    , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
	{ ESFT_OI, ESVF_G_SkillSoul,   "SkillPoint"    , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_AbsorbHp,     "AbsorbHp"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_AbsorbMp,     "AbsorbMp"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_RateGold,     "RateGold"      , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_OI, ESVF_G_RateExp,      "RateExp"       , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_CI, ESVF_G_NeedSkill,    "Need"          , ""                            , "\n"                          },
    { ESFT_CII,ESVF_G_NeedClassSP,  "NeedSP"        , ""                            , "\n"                          },
    { ESFT_OI, ESVF_G_Critical,     "CriticalHit"   , "%s%s [%s%d]"                 , "%s%s [%s%d]\n"               },
    { ESFT_CII,ESVF_G_MetierNeed,   "MetierNeed"    , ""                            , "\n"                          },
	{ ESFT_OI ,ESVF_G_ChangeBody,   "ChangeBody"    , "%s"                          , "%s\n"                        },
	{ ESFT_OI ,ESVF_G_haveAddSkillSeries,   "haveAddSkillSeries"  ,"%s"             , "%s\n"                        },
	{ ESFT_OI ,ESVF_G_IsHaveCallNpc,"IsHaveCallNpc"  ,"%s"                          , "%s\n"                        },	
};

SSkillEffectMap g_mapSkillEffect[] =
{
    { ESET_I,  ESVF_NONE,               "none"          },  // 0  
    { ESET_II, ESVF_S_HP,               "HP"            },  // 1  直接扣取HP 显示还是不显示
    { ESET_I,  ESVF_S_DAMAGE,           "Damage"        },  // 2  物理伤害
    { ESET_I,  ESVF_S_MP,               "MP"            },  // 3  直接扣取MP
    { ESET_NA, ESVF_O_RIDE,             "Ride"          },  // 4  骑兽
    { ESET_NA, ESVF_O_UNRIDE,           "Unride"        },  // 5  下兽
    { ESET_I,  ESVF_S_RANGE,            "Range"         },  // 6  设置伤害的范围
    { ESET_I,  ESVF_S_AddedCon,         "AddedCon"      },  // 7  添加体质
    { ESET_I,  ESVF_S_AddedStr,         "AddedStr"      },  // 8  添加力量
    { ESET_I,  ESVF_S_AddedDex,         "AddedDex"      },  // 9  添加敏捷
    { ESET_I,  ESVF_S_AddedInt,         "AddedInt"      },  // 10 添加智力
    { ESET_I,  ESVF_S_AddedHit,         "AddedHit"      },  // 11 添加精准
    { ESET_I,  ESVF_S_Resist,           "Resist"        },  // 12 添加所有的元素防御
    { ESET_I,  ESVF_S_MaxHP,            "MaxHP"         },  // 13 添加最大HP
    { ESET_I,  ESVF_S_MaxMP,            "MaxMP"         },  // 14 添加最大MP
    { ESET_I,  ESVF_S_PhyAr,            "PhyAr"         },  // 15 添加物理防御
    { ESET_I,  ESVF_S_PhyAb,            "PhyAb"         },  // 16 添加物理吸收
    { ESET_I,  ESVF_S_AbsorbHP,         "AbsorbHP"      },  // 17 添加吸血
    { ESET_I,  ESVF_S_AbsorbMP,         "AbsorbMP"      },  // 18 添加吸魔
    { ESET_I,  ESVF_S_DamageMin,        "DamageMin"     },  // 19 添加最小伤害
    { ESET_I,  ESVF_S_DamageMax,        "DamageMax"     },  // 20 添加最大伤害
    { ESET_I,  ESVF_S_SpeedAttack,      "SpeedAttack"   },  // 21 添加攻击速度
    { ESET_I,  ESVF_S_SpeedMove,        "SpeedMove"     },  // 22 添加移动速度
    { ESET_I,  ESVF_S_RateGold,         "RateGold"      },  // 23 添加获得金钱倍率
    { ESET_I,  ESVF_S_RateExp,          "RateExp"       },  // 24 添加获得经验倍率
    { ESET_I,  ESVF_S_WaterDm,          "WaterDm"       },  // 25 添加水伤害
    { ESET_I,  ESVF_S_FireDm,           "FireDm"        },  // 26 添加火伤害
    { ESET_I,  ESVF_S_PoisonDm,         "PoisonDm"      },  // 27 添加毒伤害
    { ESET_I,  ESVF_S_WaterAr,          "WaterAr"       },  // 28 添加水防御
    { ESET_I,  ESVF_S_FireAr,           "FireAr"        },  // 29 添加火防御
    { ESET_I,  ESVF_S_PoisonAr,         "PoisonAr"      },  // 30 添加毒防御
    { ESET_I,  ESVF_S_WaterAb,          "WaterAb"       },  // 31 添加水吸收
    { ESET_I,  ESVF_S_FireAb,           "FireAb"        },  // 32 添加火吸收
    { ESET_I,  ESVF_S_PoisonAb,         "PoisonAb"      },  // 33 添加毒吸收
    { ESET_I,  ESVF_S_AttackRate,       "AttackRate"    },  // 34 添加命中
    { ESET_I,  ESVF_S_Dodge,            "Dodge"         },  // 35 添加躲避
    { ESET_I,  ESVF_S_AttackRange,      "AttackRange"   },  // 36 添加攻击距离
    { ESET_F,  ESVF_S_HPR,              "HPR"           },  // 37 直接扣取HP Rate
    { ESET_F,  ESVF_S_MPR,              "MPR"           },  // 38 直接扣取MP Rate
    { ESET_F,  ESVF_S_AddedConR,        "AddedConR"     },  // 39 添加体质 Rate
    { ESET_F,  ESVF_S_AddedStrR,        "AddedStrR"     },  // 40 添加力量 Rate
    { ESET_F,  ESVF_S_AddedDexR,        "AddedDexR"     },  // 41 添加敏捷 Rate
    { ESET_F,  ESVF_S_AddedIntR,        "AddedIntR"     },  // 42 添加智力 Rate
    { ESET_F,  ESVF_S_AddedHitR,        "AddedHitR"     },  // 43 添加精准 Rate
    { ESET_F,  ESVF_S_ResistR,          "ResistR"       },  // 44 添加所有的元素防御 Rate
    { ESET_F,  ESVF_S_MaxHPR,           "MaxHPR"        },  // 45 添加最大HP Rate
    { ESET_F,  ESVF_S_MaxMPR,           "MaxMPR"        },  // 46 添加最大MP Rate
    { ESET_F,  ESVF_S_PhyArR,           "PhyArR"        },  // 47 添加物理防御 Rate
    { ESET_F,  ESVF_S_PhyAbR,           "PhyAbR"        },  // 48 添加物理吸收 Rate
    { ESET_F,  ESVF_S_AbsorbHPR,        "AbsorbHPR"     },  // 49 添加吸血 Rate
    { ESET_F,  ESVF_S_AbsorbMPR,        "AbsorbMPR"     },  // 50 添加吸魔 Rate
    { ESET_F,  ESVF_S_DamageMinR,       "DamageMinR"    },  // 51 添加最小伤害 Rate
    { ESET_F,  ESVF_S_DamageMaxR,       "DamageMaxR"    },  // 52 添加最大伤害 Rate
    { ESET_F,  ESVF_S_SpeedAttackR,     "SpeedAttackR"  },  // 53 添加攻击速度 Rate
    { ESET_F,  ESVF_S_SpeedMoveR,       "SpeedMoveR"    },  // 54 添加移动速度 Rate
    { ESET_F,  ESVF_S_RateGoldR,        "RateGoldR"     },  // 55 添加获得金钱倍率 Rate
    { ESET_F,  ESVF_S_RateExpR,         "RateExpR"      },  // 56 添加获得经验倍率 Rate
    { ESET_F,  ESVF_S_WaterDmR,         "WaterDmR"      },  // 57 添加水伤害 Rate
    { ESET_F,  ESVF_S_FireDmR,          "FireDmR"       },  // 58 添加火伤害 Rate
    { ESET_F,  ESVF_S_PoisonDmR,        "PoisonDmR"     },  // 59 添加毒伤害 Rate
    { ESET_F,  ESVF_S_WaterArR,         "WaterArR"      },  // 60 添加水防御 Rate
    { ESET_F,  ESVF_S_FireArR,          "FireArR"       },  // 61 添加火防御 Rate
    { ESET_F,  ESVF_S_PoisonArR,        "PoisonArR"     },  // 62 添加毒防御 Rate
    { ESET_F,  ESVF_S_WaterAbR,         "WaterAbR"      },  // 63 添加水吸收 Rate
    { ESET_F,  ESVF_S_FireAbR,          "FireAbR"       },  // 64 添加火吸收 Rate
    { ESET_F,  ESVF_S_PoisonAbR,        "PoisonAbR"     },  // 65 添加毒吸收 Rate
    { ESET_F,  ESVF_S_AttackRateR,      "AttackRateR"   },  // 66 添加命中 Rate
    { ESET_F,  ESVF_S_DodgeR,           "DodgeR"        },  // 67 添加躲避 Rate
    { ESET_F,  ESVF_S_AttackRangeR,     "AttackRangeR"  },  // 68 添加攻击距离 Rate
    { ESET_NA, ESVF_O_FB_XIULIAN,       "FbXiuLian"     },  // 69 法宝 修炼
    { ESET_NA, ESVF_O_FB_ZHURU,         "FbZhuRu"       },  // 70 法宝 法力注入
    { ESET_I,  ESVF_O_AddEffect,        "AddEff"        },  // 71 添加效果
    { ESET_I,  ESVF_S_Critical,         "Critical"      },  // 72 必杀
    { ESET_F,  ESVF_S_CriticalR,        "CriticalR"     },  // 73 必杀Rate
    { ESET_NA, ESVF_O_ClearUseful,      "ClearUseful"   },  // 74 清除有益效果
    { ESET_NA, ESVF_O_ClearBaleful,     "ClearBaleful"  },  // 75 清除有害效果
    { ESET_NA, ESVF_O_ClearAllEffect,   "ClearAllEff"   },  // 76 清除所有效果
    { ESET_I,  ESVF_O_ClearEffect,      "ClearEff"      },  // 76 清除指定效果
    { ESET_I,  ESVF_S_StateMove,        "StateMove"     },  // 78 是否可以移动
    { ESET_I,  ESVF_S_StateDodge,       "StateDodge"    },  // 79 是否可以闪避
    { ESET_I,  ESVF_S_StateUseSkill,    "StateUseSkill" },  // 80 是否可以使用技能
    { ESET_I,  ESVF_S_StateNAttack,     "StateNAttack"  },  // 81 是否可以普通攻击
    { ESET_I,  ESVF_S_StateResist,      "StateResist"   },  // 82 增加状态抵抗
    { ESET_I,  ESVF_S_Hate,             "Hate"          },  // 83 额外的仇恨度
    { ESET_I,  ESVF_S_HateRate,         "HateRate"      },  // 84 仇恨比率 [0-100]
    { ESET_I,  ESVF_S_AddHate,          "AddHate"       },  // 85 增加怪物仇恨
    { ESET_I,  ESVF_S_SkillCriRate,     "SkillCriRate"  },  // 86 技能爆击率
    { ESET_I,  ESVF_S_SkillCriDamageR,  "SkillCriDamage"},  // 87 技能爆击伤害
    { ESET_II, ESVF_O_Conjure,          "Conjure"       },  // 88 召唤
    { ESET_I,  ESVF_O_UI_Produce,       "Produce"       },  // 89 生产
    { ESET_NA, ESVF_O_UI_Syncretize,    "Syncretize"    },  // 90 融合
    { ESET_NA, ESVF_O_Item_Decompound,  "Decompound"    },  // 91 分解
	{ ESET_F,  ESVF_S_Resilience,		"Resilience"	},  // 92 抗暴击
	{ ESET_II, ESVF_S_Rebirth,			"Rebirth"		},  // 93 复活
	{ ESET_F,  ESVF_S_AddAbsorptionDamage,"AbsorptionDamage"}, //94魔法盾
	{ ESET_I,  ESVF_S_Hiding,           "Hiding"        },  //95隐身
	{ ESET_I,  ESVF_S_CriDamage,        "CriDamage"     },  //96技能爆击伤害加成
	{ ESET_I,  ESVF_S_calDamage,        "calDamage"     },  //97普通攻击爆击伤害
	{ ESET_I,  ESVF_S_calDamageR,       "calDamageR"    },  //98普通攻击爆击倍率，就是爆击伤害倍数
	{ ESET_I,  ESVF_S_ShieldMpR,        "ShieldMpR"     },  //99增加圣巫魔法盾MP吸收率
	{ ESET_II, ESVF_S_ReboundHurtR,     "ReboundHurtR"  },  //100反弹伤害率
	{ ESET_I,  ESVF_S_AbsorptionHpR,    "AbsorptionHpR" },  //101伤害转化HP
	{ ESET_FF, ESVF_S_HPMPConversion,   "HPMPConversion"},  //102减少HP 增加MP  或者增加HP 减少
	{ ESET_F , ESVF_S_AddCutMaxHP,      "AddCutMaxHP"   },  //103增加或减少最大HP的百分比
	{ ESET_F , ESVF_S_AddCutMaxMP,      "AddCutMaxMP"   },  //104增加或减少最大MP的百分比
	{ ESET_II, ESVF_S_ExtractHpR,       "ExtractHpR"    },  //105抽取对方血值  有个最大抽取量
	{ ESET_I , ESVF_S_Immuno,           "Immuno"        },  //免疫  +1 免疫有益 + 2免疫有害  +3免疫所有
	{ ESET_NA, ESVF_O_ChangeWeapon,     "ChangeWeapon"  },  // 切换武器
	{ ESET_NA, ESVF_O_BackHome,         "BackHome"      },  // 切换武器
	{ ESET_I,  ESVF_S_ChangeBody,       "ChangeBody"    },  //变身
	{ ESET_I,  ESVF_S_MomentMove,       "MomentMove"    },  //瞬间移动
	{ ESET_I,  ESVF_S_DeBufWater,       "DeBufWater"    },  
	{ ESET_I,  ESVF_S_DeBufFire,        "DeBufFire"     },  
	{ ESET_I,  ESVF_S_DeBufPoison,      "DeBufPoison"  },  
	{ ESET_I,  ESVF_S_ResistZXC,        "ResistZXC"     },  //抵抗爆击 
	{ ESET_I,  ESVF_S_ResistSwim,       "ResistSwim"    },  //抵抗眩晕 
	{ ESET_I,  ESVF_S_ResistInertia,    "ResistInertia" },  //抵抗迟钝 
	{ ESET_I,  ESVF_S_ResistTrance,     "ResistTrance"  },  //抵抗昏睡
	{ ESET_I,  ESVF_S_ResistBleed,      "ResistBleed"   },  //抵抗流血
	{ ESET_F,  ESVF_S_ReceiveDamageSkillR,"ReceiveDamageSkillR"},  //接收技能伤害倍率
	{ ESET_I,  ESVF_S_TeamShareDamage,  "TeamShareDamage"},  //接收技能伤害倍率
	{ ESET_I,  ESVF_S_MonomersCgroup,   "MonomersCgroup"},  //单体变群体普通攻击
	{ ESET_II, ESVF_S_OrdinaryAddEffect,"OrdinaryAddEffect"},  //普通攻击触发特效
	{ ESET_I,  ESVF_S_Hprelay,          "Hprelay"       },  //普通攻击触发特效
	{ ESET_I,  ESVF_S_Mprelay,          "Mprelay"       },  //普通攻击触发特效
	{ ESET_NA, ESVF_O_MoveParent,       "MoveParent"    },  //把目标瞬间移动到自己面前
	{ ESET_II, ESVF_S_SeriesDamage,     "SeriesDamage"  },  //系列伤害
	{ ESET_II, ESVF_S_BossConjure,      "BossConjure"   },  // //BOSS或者NPC的召唤     
	{ ESET_I , ESVF_S_UltimateChange,   "UltimateChange"},  // 超级变身
	{ ESET_I , ESVF_S_SkillhpRecover,   "SkillhpRecover"},  //技能每秒恢复血量
	{ ESET_I , ESVF_S_SkillmpRecover,   "SkillmpRecover"},  //技能每秒恢复蓝量
	//{ ESET_I , ESVF_S_ConsumeNpc,       "ConsumeNpc"    },  //消耗召唤NPC的个数	
};

const char* WeaponTypeToString(int i)
{
    static char const* s_szWeaponType[] = 
    {
        "G_WEAPON",         // 0  武器
        "G_WEAPON_BIAN",    // 1  刀/剑
        "G_WEAPON_AXE",     // 2  杵/斧
        "G_WEAPON_BOW",     // 3  弓
        "G_WEAPON_CROSSBOW",// 4  索/绳
        "G_WEAPON_FB_FL",   // 5  飞轮
        "G_WEAPON_FB_FZ",   // 6  法杖
        "G_WEAPON_FB_FJ",   // 7  飞剑
        "G_WEAPON_FB_BZ",   // 8  宝珠
        "G_SHIELD",         // 9  盾
        "G_HOE",            // 10 药锄
        "G_WEAPON_PICKAX",  // 11 矿镐
        "Unknowe"
    };
    if (i&0x0001)
    {
        return Rs(s_szWeaponType[1]);
    }else if (i&0x0002)
    {
        return Rs(s_szWeaponType[1]);
    }else if (i&0x0004)
    {
        return Rs(s_szWeaponType[2]);
    }else if (i&0x0008)
    {
        return Rs(s_szWeaponType[3]);
    }else if (i&0x0010)
    {
        return Rs(s_szWeaponType[4]);
    }else if (i&0x0020)
    {
        return Rs(s_szWeaponType[5]);
    }else if (i&0x0040)
    {
        return Rs(s_szWeaponType[6]);
    }else if (i&0x0080)
    {
        return Rs(s_szWeaponType[7]);
    }else if (i&0x0100)
    {
        return Rs(s_szWeaponType[8]);
    }else if (i&0x0200)
    {
        return Rs(s_szWeaponType[9]);
    }else if (i&0x0400)
    {
        return Rs(s_szWeaponType[10]);
    }else if (i&0x0800)
    {
        return Rs(s_szWeaponType[11]);
    }
    return Rs(s_szWeaponType[0]);
}

static char const* g_szOperationType[]    = {"", "G_GREAT", "G_LESS", "G_EQUAL", "G_EQUAL", "G_LE", "", "Unknowe"};
static char const* g_szMetierName[]       = {"G_MONSTER", "G_WARRIOR", "G_HUNTER", "G_WIZARD", "G_TAOIST", "Unknowe"};

//////////////////////////////////////////////////////////////////////////
// Function

char* SkillWordCopyError(char* pStr, int iLen)
{
    //static char szMsg[20];
	S_MEMDEF(szMsg, 20)
	S_MEMPROTECTOR(szMsg, 20, bDefined)

    if (iLen>19)
    {
        rt2_strncpy(szMsg, pStr, 16);
        szMsg[16] = 0;
        rt2_strcat(szMsg, "...");
    }else
    {
        rt2_strncpy2(szMsg, pStr, iLen);
        szMsg[iLen] = 0;
    }
    return szMsg;
}

int SkillWordIsNumber(char* pStr)
{
    int iDot=0;
    if (*pStr=='+' || *pStr=='-') pStr++;
    while (*pStr!=0 && *pStr!=';' && *pStr!=' ' && *pStr!='\t' && *pStr!='\n')
    {
        if (*pStr=='.') iDot ++;
        else if (*pStr<'0' || *pStr>'9') return 0;
        pStr ++;
    }
    if (iDot==0) return 1;
    else if (iDot==1) return 2;
    return 0;
}

char* SkipSkillSpace(char* pStr)
{
    while (*pStr==' ' || *pStr=='\t' || *pStr=='\n') pStr ++;
    return pStr;
}

char* FindSkillWord(char* pStr)
{
    char szSpace[] = "; \t\n";
    char szOperate[] = "+-*/=><%&";
    char* p = pStr;
    if (((p[0]=='+' || p[0]=='-')&&(p[1]>='0'&&p[1]<='9')) || (p[0]>='0'&&p[0]<='9'))
    { // IsNumber
        if (p[0]=='+' || p[0]=='-') p ++;
        while (*p)
        {
            if (!(p[0]=='.' || (p[0]>='0' && p[0]<='9')))
                break;
            p++;
        }
    }else if (strchr(szOperate, *p))
    { // IsOperate
        while (*p)
        {
            if (strchr(szSpace, *p) || strchr(szOperate, *p)==NULL)
                break;
            p++;
        }
    }else
    { // IsString
        while (*p)
        {
            if (strchr(szSpace, *p) || strchr(szOperate, *p))
                break;
            p++;
        }
    }
    return p;
}

char* GetSkillWordInt(char* pStr, int& iValue)
{
    pStr = SkipSkillSpace(pStr);
    char* p = FindSkillWord(pStr);
    if (p==pStr) return NULL;
    if (((pStr[0]=='+' || pStr[0]=='-')&&(pStr[1]>='0'&&pStr[1]<='9')) || (pStr[0]>='0'&&pStr[0]<='9'))
    {
        if (!SkillWordIsNumber(pStr))
        {
            RtCoreLog().Info("GetSkillWordInt: 错误的数字 [%s]\n", SkillWordCopyError(pStr, p-pStr));
            return NULL;
        }
        iValue = atoi(pStr);
    }else
    {
        iValue = -1;
        if ((p-pStr)==1)
        {
            if (pStr[0]=='>')       iValue = 1;
            else  if (pStr[0]=='<') iValue = 2;
            else  if (pStr[0]=='=') iValue = 3;
            else  if (pStr[0]=='+') iValue = 11;
            else  if (pStr[0]=='-') iValue = 12;
            else  if (pStr[0]=='*') iValue = 13;
            else  if (pStr[0]=='/') iValue = 14;
            else  if (pStr[0]=='%') iValue = 15;
            else  if (pStr[0]=='&') iValue = 16;
        }else if ((p-pStr)==2)
        {
            if (pStr[0]=='=' && pStr[1]=='=')       iValue = 4;
            else  if (pStr[0]=='<' && pStr[1]=='=') iValue = 5;
            else  if (pStr[0]=='>' && pStr[1]=='=') iValue = 6;
        }
        if (iValue==-1)
        {
            RtCoreLog().Info("GetSkillWordInt: 错误的操作符 [%s]\n", SkillWordCopyError(pStr, p-pStr));
            return NULL;
        }
    }
    return p;
}

char* GetSkillWordFloat(char* pStr, float& fValue)
{
    pStr = SkipSkillSpace(pStr);
    char* p = FindSkillWord(pStr);
    if (p==pStr) return NULL;
    if (((pStr[0]=='+' || pStr[0]=='-')&&(pStr[1]>='0'&&pStr[1]<='9')) || (pStr[0]>='0'&&pStr[0]<='9'))
    {
        if (!SkillWordIsNumber(pStr))
        {
            RtCoreLog().Info("GetSkillWordFloat: 错误的数字 [%s]\n", SkillWordCopyError(pStr, p-pStr));
            return NULL;
        }
        fValue = atof(pStr);
    }else
    {
        RtCoreLog().Info("GetSkillWordFloat: 错误的数字 [%s]\n", SkillWordCopyError(pStr, p-pStr));
        return NULL;
    }
    return p;
}

SSkillFilterMap* FindSkillFilterMap(char* pStr, int iLen)
{
    for (int i=0; i<(int)countof(g_mapSkillFilter); i++)
    {
        if (strnicmp(g_mapSkillFilter[i].szID, pStr, iLen)==0 && g_mapSkillFilter[i].szID[iLen]==0)
        {
            return g_mapSkillFilter+i;
        }
    }
    return NULL;
}

SSkillEffectMap* FindSkillEffectMap(char* pStr, int iLen)
{
    for (int i=0; i<(int)countof(g_mapSkillEffect); i++)
    {
        if (strnicmp(g_mapSkillEffect[i].szID, pStr, iLen)==0 && g_mapSkillEffect[i].szID[iLen]==0)
        {
            return g_mapSkillEffect+i;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
// SSkillFilter

int SSkillFilter::Analyse(const char* szStr, CSkillManage* pManage,int iOffset)
{
    int iCount = 0;
    char *pStr, *p, *pWord;
    SSkillFilterMap* pMap;
    SSkillNodeFilter* pNode;
    int iOperate, iValue, iValue2;
    float fValue;
    pStr = (char*)szStr;
    while (*pStr)
    {
        pStr = SkipSkillSpace(pStr);
        if (*pStr==0) break;
        pWord = pStr;
        p = FindSkillWord(pStr);
        pMap = FindSkillFilterMap(pStr, p-pStr);
        if (pMap==NULL)
        {
            RtCoreLog().Info("SSkillFilter::Analyse: 错误的条件名 [%s], %s\n", SkillWordCopyError(pStr, p-pStr), szStr);
            break;
        }
        pStr = p;
        switch (pMap->eType)
        {
        case ESFT_NA:
            if (pManage)
            {
                pNode = pManage->m_pSkillFilterNode+(iOffset+iCount);
                pNode->pMap = pMap;
            }
            break;
        case ESFT_OI:
            pStr = GetSkillWordInt(pStr, iOperate); if (pStr==NULL) return iCount;
            pStr = GetSkillWordInt(pStr, iValue);   if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillFilterNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iOperate;
                pNode->Param2.i = iValue;
            }
            break;
        case ESFT_OF:
            pStr = GetSkillWordInt(pStr, iOperate); if (pStr==NULL) return iCount;
            pStr = GetSkillWordFloat(pStr, fValue); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillFilterNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iOperate;
                pNode->Param2.f = fValue;
            }
            break;
        case ESFT_CI:
            pStr = GetSkillWordInt(pStr, iValue); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillFilterNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iValue;
            }
            break;
        case ESFT_CII:
            pStr = GetSkillWordInt(pStr, iValue); if (pStr==NULL) return iCount;
            pStr = GetSkillWordInt(pStr, iValue2); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillFilterNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iValue;
                pNode->Param2.i = iValue2;
            }
            break;
        default:
            RtCoreLog().Info("SSkillFilter::Analyse: 没有实现的函数类型\n");
            break;
        }
        pStr = SkipSkillSpace(pStr);
        if (pStr[0]!=';' && pStr[0]!=0)
        {
            RtCoreLog().Info("SSkillFilter::Analyse: 错误的表达式 [%s] %s\n", SkillWordCopyError(pWord, pStr-pWord), szStr);
            break;
        }
        if (pStr[0]==';') pStr ++;
        iCount ++;
    }
    if (*pStr!=0)
    {
        iCount = 0;
        RtCoreLog().Info("SSkillFilter::Analyse 找到不支持的条件类型[%s]\n", szStr);
    }
    if (pManage)
    {
        this->pNodes = pManage->m_pSkillFilterNode + iOffset;
        this->iCnt = iCount;
    }
    pFilterStr = szStr;
    return iCount;
}

const char* SSkillFilter::IsSuccess(CActorSkill *pCreature, bool bTest, bool bHtml, int* pLine)
{
    int iLine = 0;
    bool bFail = false;
    SSkill* pSkill;
    SSkillNodeFilter* pNode;
    char* pResultStr;
    char* pTestOutput;
    if (bTest)
    {
		// C [11/9/2008 Louis.Huang]
         pTestOutput = g_strStaticBuffer1024;
         memset(g_strStaticBuffer1024,0,sizeof(g_strStaticBuffer1024));
    }
    for (int i=0; i<this->iCnt; i++)
    {
        bFail = false;
        pResultStr = NULL;
        pNode = this->pNodes+i;
        switch (pNode->pMap->eType)
        {
        case ESFT_OI:
            {
                int iValue;
                switch (pNode->pMap->eFun)
                {
				case ESVF_G_LEVEL:          iValue = pCreature->GetLevel()          ; break;
                case ESVF_G_HP:             iValue = pCreature->GetHP()             ; break;
                case ESVF_G_MP:             iValue = pCreature->GetMP()             ; break;
                case ESVF_G_RIDE:           iValue = pCreature->IsRide()            ; break;
                case ESVF_G_WeaponType:     iValue = pCreature->GetWeaponType()     ; break;
				case ESVF_G_WeaponEquiped:  iValue = pCreature->GetWeaponEquiped()  ; break;
				case ESVF_G_TrumpEquiped:	iValue = pCreature->GetTrumpEquiped()	; break;
                case ESVF_G_ModelID:        iValue = pCreature->GetModelID()        ; break;
                case ESVF_G_Cloth:          iValue = pCreature->GetCloth()          ; break;
                case ESVF_G_ClothLev:       iValue = pCreature->GetClothLev()       ; break;
                case ESVF_G_ArmGuard:       iValue = pCreature->GetArmGuard()       ; break;
                case ESVF_G_ArmGuardLev:    iValue = pCreature->GetArmGuardLev()    ; break;
                case ESVF_G_Shoes:          iValue = pCreature->GetShoes()          ; break;
                case ESVF_G_ShoesLev:       iValue = pCreature->GetShoesLev()       ; break;
                case ESVF_G_Weapon:         iValue = pCreature->GetWeapon()         ; break;
                case ESVF_G_WeaponLev:      iValue = pCreature->GetWeaponLev()      ; break;
                case ESVF_G_MaxMP:          iValue = pCreature->GetMaxMP()          ; break;
                case ESVF_G_Attack:         iValue = pCreature->GetAttack()         ; break;
                case ESVF_G_Dodge:          iValue = pCreature->GetDodge()          ; break;
                case ESVF_G_DamageMin:      iValue = pCreature->GetDamageMin()      ; break;
                case ESVF_G_DamageMax:      iValue = pCreature->GetDamageMax()      ; break;
                case ESVF_G_Armor:          iValue = pCreature->GetArmor()          ; break;
                case ESVF_G_Con:            iValue = pCreature->GetCon()            ; break;
                case ESVF_G_Str:            iValue = pCreature->GetStr()            ; break;
                case ESVF_G_Dex:            iValue = pCreature->GetDex()            ; break;
                case ESVF_G_Int:            iValue = pCreature->GetInt()            ; break;
                case ESVF_G_Hit:            iValue = pCreature->GetHit()            ; break;
                case ESVF_G_Metier:         iValue = pCreature->GetMetier()         ; break;
                case ESVF_G_MetierLev:      iValue = pCreature->GetMetierLev()      ; break;
                case ESVF_G_Faction:        iValue = pCreature->GetFaction()        ; break;
                case ESVF_G_Exp:            iValue = pCreature->GetExp()            ; break;
                case ESVF_G_AttrPoint:      iValue = pCreature->GetAttrPoint()      ; break;
                case ESVF_G_SkillPoint:     iValue = pCreature->GetSkillPoint()     ; break;
				case ESVF_G_SkillSoul: iValue = pCreature->GetSkillSoul(); break;
                case ESVF_G_AbsorbHp:       iValue = pCreature->GetAbsorbHp()       ; break;
                case ESVF_G_AbsorbMp:       iValue = pCreature->GetAbsorbMp()       ; break;
                case ESVF_G_RateGold:       iValue = pCreature->GetRateGold()       ; break;
                case ESVF_G_RateExp:        iValue = pCreature->GetRateExp()        ; break;
                case ESVF_G_AttSpeed:       iValue = pCreature->GetAttSpeed()       ; break;
                case ESVF_G_Critical:       iValue = pCreature->GetCritical()       ; break;
				case ESVF_G_ChangeBody:     iValue = pCreature->IsChangeBody()      ; break;
				case ESVF_G_haveAddSkillSeries:iValue = pCreature->GetSkillSeries() ; break;
				case ESVF_G_IsHaveCallNpc:  iValue = pCreature->IsHaveCallNpc()     ; break;
                }
                switch (pNode->Param1.i)
                {
                case 1: // >
                    if (!(iValue > pNode->Param2.i)) bFail = true;
                    break;
                case 2: // <
                    if (!(iValue < pNode->Param2.i)) bFail = true;
                    break;
                case 3: // =
                case 4: // ==
                    if (!(iValue == pNode->Param2.i)) bFail = true;
                    break;
                case 5: // <=
                    if (!(iValue <= pNode->Param2.i)) bFail = true;
                    break;
                case 6: // >=
                    if (!(iValue >= pNode->Param2.i)) bFail = true;
                    break;
                case 16: // &
                    if ((iValue & pNode->Param2.i)==0) bFail = true;
                    break;
                }
                if (bFail || bTest)
                {
                    switch (pNode->pMap->eFun)
                    {
					case ESVF_G_ChangeBody:
						rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(MSG_SKILL_ONLYTRANSFORMCANUSE));
						break;
                    case ESVF_G_RIDE:
                        rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(SMSG_RIDE_NEED));
                        break;
                    case ESVF_G_Metier:
                        rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(SMSG_METIER_NEED), Rs(g_szMetierName[pNode->Param2.i]));
                        break;
                    case ESVF_G_WeaponType:
                        rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(SMSG_EQUIP_TYPE_NEED), WeaponTypeToString(pNode->Param2.i));
                        break;
                    case ESVF_G_WeaponEquiped:
                        rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(SMSG_EQUIP_WEAPON_NEED));
						break;
					case ESVF_G_TrumpEquiped:
						rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(SMSG_EQUIP_TRUMP_NEED));
						break;
                    default:
						if(bTest)
							rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(G_NEED), Rs(pNode->pMap->szID), Rs(g_szOperationType[pNode->Param1.i]), pNode->Param2.i);
						else
							rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, R(MSG_SKILL_NEED), Rs(pNode->pMap->szID), Rs(g_szOperationType[pNode->Param1.i]), pNode->Param2.i);
						break;
                    }
                }
            }
            break;
        case ESFT_OF:
            {
                float fValue;
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_DamageRate:     fValue = pCreature->GetDamageRate()     ; break;
                }
                switch (pNode->Param1.i)
                {
                case 1: // >
                    if (!(fValue > pNode->Param2.f)) bFail = true;
                    break;
                case 2: // <
                    if (!(fValue < pNode->Param2.f)) bFail = true;
                    break;
                case 3: // =
                case 4: // ==
                    if (!(fValue == pNode->Param2.f)) bFail = true;
                    break;
                case 5: // <=
                    if (!(fValue <= pNode->Param2.f)) bFail = true;
                    break;
                case 6: // >=
                    if (!(fValue >= pNode->Param2.f)) bFail = true;
                    break;
                }
                if (bFail || bTest)
                {
                    rt2_sprintf(g_szSkillErrMsg, pNode->pMap->szMsg, Rs(g_szOperationType[pNode->Param1.i]), pNode->Param2.f);
                }
            }
            break;
        case ESFT_CI:
            {
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_NeedSkill:
                    pSkill = Skill()->FindSkill(pNode->Param1.i);
                    if (pSkill)
                    {
                        if (!pCreature->FindSkillBySubID(pSkill->wSubID))
                        {
                            bFail = true;
                        }
                    }
                    break;
                }
                if (bFail || bTest)
                {
                    switch (pNode->pMap->eFun)
                    {
                    case ESVF_G_NeedSkill:
                        pSkill = Skill()->FindSkill(pNode->Param1.i);
                        if (pSkill)
                        {
                            if (pSkill->iLevel==1)
                            {
								if(bTest)
									rt2_sprintf(g_szSkillErrMsg, R(SMSG_NEED_SKILL), pSkill->szName);
								else
									rt2_sprintf(g_szSkillErrMsg, R(MSG_NEED_SKILL_NEW), pSkill->szName);
							}else
                            {
								if(bTest)
									rt2_sprintf(g_szSkillErrMsg, R(SMSG_NEED_SKILL_LEVEL), pSkill->szName, pSkill->iLevel);
								else
									rt2_sprintf(g_szSkillErrMsg, R(MSG_NEED_SKILL_LEVEL_NEW), pSkill->szName, pSkill->iLevel);
							}
                        }else
                        {
                            rt2_strncpy(g_szSkillErrMsg, g_mapSkillFilter->szMsg, 100);
                        }
                        break;
                    }
                }
            }
            break;
        case ESFT_CII:
            {
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_NeedClassSP:
                    if (pCreature->GetClassSP(pNode->Param1.i)<pNode->Param2.i)
                        bFail = true;
                    break;
                case ESVF_G_MetierNeed:
                    if (pCreature->GetMetier()!=pNode->Param1.i || pCreature->GetMetierLev()<pNode->Param2.i)
                        bFail = true;
                    break;
                }
                if (bFail || bTest)
                {
                    switch (pNode->pMap->eFun)
                    {
                    case ESVF_G_NeedClassSP:
						if(bTest)
							rt2_sprintf(g_szSkillErrMsg, R(SMSG_NEED_SKILL_CLASS_SP), Skill()->FindSkillClass(pNode->Param1.i), pNode->Param2.i);
						else
							rt2_sprintf(g_szSkillErrMsg, R(MSG_NEED_SKILL_CLASS_SP_NEW), Skill()->FindSkillClass(pNode->Param1.i), pNode->Param2.i);
						break;
                    case ESVF_G_MetierNeed:
                        {
                            SUserActor* pUserActorInfo = g_TableUserActor.FindUserActor(pNode->Param1.i, pNode->Param2.i);
                            if (pUserActorInfo)
                            {
                                rt2_sprintf(g_szSkillErrMsg, "%s [%s]", R(SMSG_METIER_NEED), pUserActorInfo->MetierName.c_str());
                            }else
                            {
                                rt2_sprintf(g_szSkillErrMsg, "%s [%s]\n", R(SMSG_METIER_NEED), R(G_ERROR));
                            }
                        }
                        break;
                    }
                }
            }
            break;
        default:
            RtCoreLog().Info("SSkillFilter::IsSuccess 没有实现的函数类型\n");
            bFail = true;
            break;
        }

        if (bTest)
        {
            iLine ++;
            if (bFail)
            {
                if (bHtml) rt2_strcat(pTestOutput, "<p><FONT color=\"#ff0000\">");
                rt2_strcat(pTestOutput, g_szSkillErrMsg);
                if (bHtml) rt2_strcat(pTestOutput, "</FONT></p>");
            }else
            {
                if (bHtml) rt2_strcat(pTestOutput, "<p><FONT color=\"#ffffff\">");
                rt2_strcat(pTestOutput, g_szSkillErrMsg);
                if (bHtml) rt2_strcat(pTestOutput, "</FONT></p>");
            }
        }else
        {
            if (bFail)
            {
                return g_szSkillErrMsg;
            }
        }
        if (!bFail)
        {
            g_szSkillErrMsg[0] = 0;
        }
    }
    if (bTest)
    {
        if (pLine)
        {
            *pLine = iLine;
        }
        return pTestOutput;
    }
    return NULL;
}

void SSkillFilter::SaveToString(char* szString,  int length)
{
    guard;
    szString[0] = 0;
    SSkillNodeFilter* pNode;
    for (int i=0; i<this->iCnt; i++)
    {
        pNode = this->pNodes+i;
        switch (pNode->pMap->eType)
        {
        case ESFT_OI:
            {
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_RIDE:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(SMSG_RIDE_NEED));
                    break;
                case ESVF_G_Metier:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(SMSG_METIER_NEED), Rs(g_szMetierName[pNode->Param2.i]));
                    break;
                case ESVF_G_WeaponType:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(SMSG_EQUIP_TYPE_NEED), WeaponTypeToString(pNode->Param2.i));
                    break;
                case ESVF_G_WeaponEquiped:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(SMSG_EQUIP_WEAPON_NEED));
					break;
				case ESVF_G_TrumpEquiped:
					rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(SMSG_EQUIP_TRUMP_NEED));
					break;
                default:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, R(G_NEED), Rs(pNode->pMap->szID), Rs(g_szOperationType[pNode->Param1.i]), pNode->Param2.i);
                    break;
                }
            }
            break;
        case ESFT_OF:
            {
                rt2_snprintf(szString+strlen(szString), length-strlen(szString), pNode->pMap->szInfo, Rs(g_szOperationType[pNode->Param1.i]), pNode->Param2.f);
            }
            break;
        case ESFT_CI:
            {
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_NeedSkill:
                    {
                        SSkill* pSkill = Skill()->FindSkill(pNode->Param1.i);
                        if (pSkill)
                        {
                            if (pSkill->iLevel==1)
                            {
                                rt2_snprintf(szString+strlen(szString), length-strlen(szString), R(SMSG_NEED_SKILL), pSkill->szName);
                            }else
                            {
                                rt2_snprintf(szString+strlen(szString), length-strlen(szString), R(SMSG_NEED_SKILL_LEVEL), pSkill->szName, pSkill->iLevel);
                            }
                            rt2_strcat(szString, "\n");
                        }
                    }
                    break;
                }
            }
            break;
        case ESFT_CII:
            {
                switch (pNode->pMap->eFun)
                {
                case ESVF_G_NeedClassSP:
                    rt2_snprintf(szString+strlen(szString), length-strlen(szString), R(SMSG_NEED_SKILL_CLASS_SP), Skill()->FindSkillClass(pNode->Param1.i), pNode->Param2.i);
                    rt2_strcat(szString, "\n");
                    break;
                case ESVF_G_MetierNeed:
                    {
                        SUserActor* pUserActorInfo = g_TableUserActor.FindUserActor(pNode->Param1.i, pNode->Param2.i);
                        if (pUserActorInfo)
                        {
                            rt2_snprintf(szString+strlen(szString), length-strlen(szString), "%s [%s]\n", R(SMSG_METIER_NEED), pUserActorInfo->MetierName.c_str());
                        }else
                        {
                            rt2_snprintf(szString+strlen(szString),length-strlen(szString),  "%s [%s]\n", R(SMSG_METIER_NEED), R(G_ERROR));
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    unguard;
}

//////////////////////////////////////////////////////////////////////////
// SSkillAction

int SSkillAction::Analyse(const char* szStr, CSkillManage* pManage, int iOffset)
{
    int iCount = 0;
    char *pStr, *p, *pWord;
    SSkillEffectMap* pMap;
    SSkillActionNode* pNode;
    int iValue1, iValue2;
    float fValue1, fValue2;
    pStr = (char*)szStr;
    while (*pStr)
    {
        pStr = SkipSkillSpace(pStr);
        if(!*pStr) break;
        pWord = pStr;
        p = FindSkillWord(pStr);
        pMap = FindSkillEffectMap(pStr, p-pStr);
        if (pMap==NULL)
        {
            RtCoreLog().Info("SSkillAction::Analyse: 错误的效果名 [%s] %s\n", SkillWordCopyError(pStr, p-pStr), szStr);
            break;
        }
        pStr = p;
        switch (pMap->eType)
        {
        case ESET_NA:
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
            }
            break;
        case ESET_I:
            pStr = GetSkillWordInt(pStr, iValue1); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iValue1;
            }
            break;
        case ESET_F:
            pStr = GetSkillWordFloat(pStr, fValue1); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.f = fValue1;
            }
            break;
        case ESET_IF:
            pStr = GetSkillWordInt(pStr, iValue1);   if (pStr==NULL) return iCount;
            pStr = GetSkillWordFloat(pStr, fValue1); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iValue1;
                pNode->Param2.f = fValue1;
            }
            break;
        case ESET_II:
            pStr = GetSkillWordInt(pStr, iValue1); if (pStr==NULL) return iCount;
            pStr = GetSkillWordInt(pStr, iValue2); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.i = iValue1;
                pNode->Param2.i = iValue2;
            }
            break;
        case ESET_FF:
            pStr = GetSkillWordFloat(pStr, fValue1); if (pStr==NULL) return iCount;
            pStr = GetSkillWordFloat(pStr, fValue2); if (pStr==NULL) return iCount;
            if (pManage)
            {
                pNode = pManage->m_pSkillEffectNode+(iOffset+iCount);
                pNode->pMap = pMap;
                pNode->Param1.f = fValue1;
                pNode->Param2.f = fValue2;
            }
            break;
        default:
            RtCoreLog().Info("SSkillAction::Analyse: 没有实现的函数类型\n");
            break;
        }
        pStr = SkipSkillSpace(pStr);
        if (pStr[0]!=';' && pStr[0]!=0)
        {
            RtCoreLog().Info("SSkillAction::Analyse: 错误的表达式 [%s] %s\n", SkillWordCopyError(pWord, pStr-pWord), szStr);
            break;
        }
        if (pStr[0]==';') pStr ++;
        iCount ++;
    }
    if(*pStr)
    {
        iCount = 0;
        RtCoreLog().Info("SSkillAction::Analyse 找到不支持的效果类型[%s]\n", szStr);
    }
    if (pManage)
    {
        this->pNodes = pManage->m_pSkillEffectNode + iOffset;
        this->iCnt = iCount;
    }
    pEffectStr = szStr;
    return iCount;
}

void SSkillAction::Apply(CActorSkill *pReleaser, CActorSkill *pTarget, DWORD dwUID, bool bReverse,int DamageMode)
{
    SSkillActionNode* pNode;
	
	int _DamageMode = DamageMode;

    for (int i=0; i<this->iCnt; i++)
    {
        pNode = this->pNodes+i;
        switch (pNode->pMap->eType)
        {
        case ESET_NA:  // 没有参数
            if (pReleaser)
            {
                switch (pNode->pMap->eFun)
                {
			    case ESVF_O_RIDE:           pReleaser->Ride();break;
				case ESVF_O_BackHome:       pReleaser->BackHome();break;//回程 回复活点
				case ESVF_O_ChangeWeapon:   pReleaser->ChangeWeapon();break;//切换武器
                case ESVF_O_UNRIDE:         pReleaser->Unride();break;
                case ESVF_O_FB_XIULIAN:     pReleaser->FbXiulian();break; // 69 法宝 修炼
                case ESVF_O_FB_ZHURU:       pReleaser->FbZhuru();break; // 70 法宝 法力注入
                case ESVF_O_UI_Syncretize:  pReleaser->UISyncretize();break; // 融合，客户端会打开融合界面
                case ESVF_O_Item_Decompound:pReleaser->ItemDecompound();break; // 道具分解，客户端会进入成道具分解状态
                default: break;
                }
            }
            if (pTarget)
            {
                switch (pNode->pMap->eFun)
                {
				case ESVF_O_ClearUseful:    pTarget->ClearUseful();break; // 74 
				case ESVF_O_ClearBaleful:   pTarget->ClearBaleful();break; // 75 
				case ESVF_O_ClearAllEffect: pTarget->ClearAllEff();break; // 76 
				case ESVF_O_MoveParent: 
					{
						if (pReleaser)
						{
							pTarget->MoveParent(pReleaser);
						}
					}break;
				default: break;
                }
            }
            break;
        case ESET_I:     // int
            if (pTarget)
            {
                int iAdd;
                if (bReverse) iAdd = -pNode->Param1.i;
                else          iAdd = pNode->Param1.i;
                switch (pNode->pMap->eFun)
                {
                case ESVF_S_MP:             pTarget->AddMP(iAdd);break;
                case ESVF_S_DAMAGE:         pTarget->SetImmDamage(iAdd);break;
                case ESVF_S_RANGE:          pTarget->SetImmDamageOffset(rtRandom(pNode->Param1.i, -pNode->Param1.i));break;
                case ESVF_S_AddedCon:       pTarget->AddAddedCon((short)iAdd);break;
                case ESVF_S_AddedStr:       pTarget->AddAddedStr((short)iAdd);break;
                case ESVF_S_AddedDex:       pTarget->AddAddedDex((short)iAdd);break;
                case ESVF_S_AddedInt:       pTarget->AddAddedInt((short)iAdd);break;
                case ESVF_S_AddedHit:       pTarget->AddAddedHit((short)iAdd);break;
                case ESVF_S_Resist:         pTarget->AddResist((short)iAdd);break;
                case ESVF_S_MaxHP:          pTarget->AddMaxHP((short)iAdd);break;
                case ESVF_S_MaxMP:          pTarget->AddMaxMP((short)iAdd);break;
                case ESVF_S_PhyAr:          pTarget->AddPhyAr((short)iAdd);break;  // 15 添加物理防御
                case ESVF_S_PhyAb:          pTarget->AddPhyAb((short)iAdd);break;  // 16 添加物理吸收
                case ESVF_S_AbsorbHP:       pTarget->AddAbsorbHP((short)iAdd);break;  // 17 添加吸血
                case ESVF_S_AbsorbMP:       pTarget->AddAbsorbMP((short)iAdd);break;  // 18 添加吸魔
                case ESVF_S_DamageMin:      pTarget->AddDamageMin((short)iAdd,(int)_DamageMode);break;  // 19 添加最小伤害
                case ESVF_S_DamageMax:      pTarget->AddDamageMax((short)iAdd,(int)_DamageMode);break;  // 20 添加最大伤害
                case ESVF_S_SpeedAttack:    pTarget->AddSpeedAttack((short)iAdd);break;  // 21 添加攻击速度
                case ESVF_S_SpeedMove:      pTarget->AddSpeedMove((short)iAdd);break;  // 22 添加移动速度
                case ESVF_S_RateGold:       pTarget->AddRateGold((short)iAdd);break;  // 23 添加获得金钱倍率
                case ESVF_S_RateExp:        pTarget->AddRateExp((short)iAdd);break;  // 24 添加获得经验倍率
                case ESVF_S_WaterDm:        pTarget->AddWaterDm((short)iAdd,(int)_DamageMode);break;  // 25 添加水伤害
                case ESVF_S_FireDm:         pTarget->AddFireDm((short)iAdd,(int)_DamageMode);break;  // 26 添加火伤害
                case ESVF_S_PoisonDm:       pTarget->AddPoisonDm((short)iAdd,(int)_DamageMode);break;  // 27 添加毒伤害
                case ESVF_S_WaterAr:        pTarget->AddWaterAr((short)iAdd);break;  // 28 添加水防御
                case ESVF_S_FireAr:         pTarget->AddFireAr((short)iAdd);break;  // 29 添加火防御
                case ESVF_S_PoisonAr:       pTarget->AddPoisonAr((short)iAdd);break;  // 30 添加毒防御
                case ESVF_S_WaterAb:        pTarget->AddWaterAb((short)iAdd);break;  // 31 添加水吸收
                case ESVF_S_FireAb:         pTarget->AddFireAb((short)iAdd);break;  // 32 添加火吸收
                case ESVF_S_PoisonAb:       pTarget->AddPoisonAb((short)iAdd);break;  // 33 添加毒吸收
                case ESVF_S_AttackRate:     pTarget->AddAttackRate((short)iAdd);break; // 34 添加命中
                case ESVF_S_Dodge:          pTarget->AddDodge((short)iAdd);break; // 35 添加躲避
                case ESVF_S_AttackRange:    pTarget->AddAttackRange((short)iAdd);break; // 36 添加攻击距离
                case ESVF_O_AddEffect:      pTarget->AddEff((short)iAdd, dwUID);break;
                case ESVF_S_Critical:       pTarget->AddCritical((short)iAdd);break;
                case ESVF_O_ClearEffect:    pTarget->ClearEff((short)iAdd);break;
                case ESVF_S_StateMove:      pTarget->StateMove((short)iAdd);break; // 78 是否可以移动
                case ESVF_S_StateDodge:     pTarget->StateDodge((short)iAdd);break; // 79 是否可以闪避
                case ESVF_S_StateUseSkill:  pTarget->StateUseSkill((short)iAdd);break; // 80 是否可以使用技能
                case ESVF_S_StateNAttack:   pTarget->StateNAttack((short)iAdd);break; // 81 是否可以普通攻击
                case ESVF_S_StateResist:    pTarget->StateResist((short)iAdd);break; // 82 增加状态抵抗
                case ESVF_S_Hate:           pTarget->Hate((short)iAdd);break; // 83 额外的仇恨度
                case ESVF_S_HateRate:       pTarget->HateRate((short)iAdd);break; // 84 仇恨比率
                case ESVF_S_AddHate:        pTarget->AddHate((short)iAdd, pReleaser);break; // 85 增加怪物仇恨
                case ESVF_S_SkillCriRate:   pTarget->AddSkillCriRate((short)iAdd, pReleaser);break; // 86 技能爆击率
                case ESVF_S_SkillCriDamageR: pTarget->AddSkillCriDamageR((short)iAdd, pReleaser);break; // 87 技能爆击伤害
                case ESVF_O_UI_Produce:     pTarget->UIProduce((short)iAdd);break; // 制作，客户端会打开制作界面
				case ESVF_S_CriDamage:      pTarget->AddSkillCriDamage((short)iAdd);break;// 技能爆击伤害加成
				case ESVF_S_calDamage:      pTarget->AddCriticalDamage((short)iAdd);break;// 普通攻击爆击伤害
				case ESVF_S_calDamageR:     pTarget->AddCriticalDamageR((short)iAdd);break;// 普通攻击爆击倍率，就是爆击伤害倍数
				case ESVF_S_ShieldMpR:      pTarget->AddShieldMpR((short)iAdd);break;// 增加圣巫魔法盾MP吸收率
				case ESVF_S_AbsorptionHpR:  pTarget->AddAbsorptionHpR((short)iAdd);break;// 增加被伤害后有几率增加HP
				case ESVF_S_Hiding:         pTarget->PlayerHiding((short)iAdd);break;//92 隐身
				case ESVF_S_Immuno:         pTarget->PlayerImmuno((short)iAdd);break;  
				case ESVF_S_ChangeBody:     pTarget->ChangeBody((int)iAdd);break;  //变身 传的是模型ID
				case ESVF_S_MomentMove:     pTarget->MomentMove((int)iAdd);break;  //变身 传的是距离
				case ESVF_S_DeBufWater:	    pTarget->AddWaterImmDm((short)iAdd);break; 
				case ESVF_S_DeBufFire:	    pTarget->AddFireImmDm((short)iAdd);break; 
				case ESVF_S_DeBufPoison:	pTarget->AddPoisonImmDm((short)iAdd);break; 
				case ESVF_S_ResistZXC:      pTarget->AddResistZXC((short)iAdd);break; //抵抗爆击 
				case ESVF_S_ResistSwim:     pTarget->AddResistSwim((short)iAdd);break; //抵抗眩晕 
				case ESVF_S_ResistInertia:  pTarget->AddResistInertia((short)iAdd);break; //抵抗迟钝 
				case ESVF_S_ResistTrance:   pTarget->AddResistTrance((short)iAdd);break; //抵抗昏睡
				case ESVF_S_ResistBleed:    pTarget->AddResistBleed((short)iAdd);break; //抵抗流血
				case ESVF_S_TeamShareDamage:pTarget->TeamShareDamage((short)iAdd);break;//伤害分摊队友
				case ESVF_S_MonomersCgroup: pTarget->MonomersCgroup((short)iAdd);break;//单体变群体普攻 传入范围
				case ESVF_S_Hprelay:        pReleaser->Hprelay((short)iAdd,pTarget);break;//把自己当前的HP传递给队友
				case ESVF_S_Mprelay:        pReleaser->Mprelay((short)iAdd,pTarget);break;//把自己当前的MP传递给队友
				case ESVF_S_UltimateChange: pTarget->UltimateChange((short)iAdd);break;//终极变身 
				case ESVF_S_SkillhpRecover: pTarget->SkillhpRecover((short)iAdd);break;       //技能每秒恢复血量
				case ESVF_S_SkillmpRecover: pTarget->SkillmpRecover((short)iAdd);break;      //技能每秒恢复蓝量
				//case ESVF_S_ConsumeNpc:     pTarget->ConsumeNpc((short)iAdd);break;  //消耗召唤NPC的个数
                default: break;
                }
            }
            break;
        case ESET_F:     // float
            if (pTarget)
            {
                float fAdd;
                if (bReverse) fAdd = -pNode->Param1.f;
                else          fAdd = pNode->Param1.f;
                switch (pNode->pMap->eFun)
                {
                case ESVF_S_HPR:         pTarget->AddHPR(fAdd);break;
                case ESVF_S_MPR:         pTarget->AddMPR(fAdd);break;
                case ESVF_S_AddedConR:   pTarget->AddAddedConR(fAdd);break;
                case ESVF_S_AddedStrR:   pTarget->AddAddedStrR(fAdd);break;
                case ESVF_S_AddedDexR:   pTarget->AddAddedDexR(fAdd);break;
                case ESVF_S_AddedIntR:   pTarget->AddAddedIntR(fAdd);break;
                case ESVF_S_AddedHitR:   pTarget->AddAddedHitR(fAdd);break;
                case ESVF_S_ResistR:     pTarget->AddResistR(fAdd);break; //增加元素抵抗率
                case ESVF_S_MaxHPR:      pTarget->AddMaxHPR(fAdd);break;
                case ESVF_S_MaxMPR:      pTarget->AddMaxMPR(fAdd);break;
                case ESVF_S_PhyArR:      pTarget->AddPhyArR(fAdd);break;  // 15 添加物理防御
                case ESVF_S_PhyAbR:      pTarget->AddPhyAbR(fAdd);break;  // 16 添加物理吸收
                case ESVF_S_AbsorbHPR:   pTarget->AddAbsorbHPR(fAdd);break;  // 17 添加吸血
                case ESVF_S_AbsorbMPR:   pTarget->AddAbsorbMPR(fAdd);break;  // 18 添加吸魔
                case ESVF_S_DamageMinR:  pTarget->AddDamageMinR(fAdd);break;  // 19 添加最小伤害
                case ESVF_S_DamageMaxR:  pTarget->AddDamageMaxR(fAdd);break;  // 20 添加最大伤害
                case ESVF_S_SpeedAttackR:pTarget->AddSpeedAttackR(fAdd);break;  // 21 添加攻击速度
                case ESVF_S_SpeedMoveR:  pTarget->AddSpeedMoveR(fAdd);break;  // 22 添加移动速度
                case ESVF_S_RateGoldR:   pTarget->AddRateGoldR(fAdd);break;  // 23 添加获得金钱倍率
                case ESVF_S_RateExpR:    pTarget->AddRateExpR(fAdd);break;  // 24 添加获得经验倍率
                case ESVF_S_WaterDmR:    pTarget->AddWaterDmR(fAdd);break;  // 25 添加水伤害
                case ESVF_S_FireDmR:     pTarget->AddFireDmR(fAdd);break;  // 26 添加火伤害
                case ESVF_S_PoisonDmR:   pTarget->AddPoisonDmR(fAdd);break;  // 27 添加毒伤害
                case ESVF_S_WaterArR:    pTarget->AddWaterArR(fAdd);break;  // 28 添加水防御
                case ESVF_S_FireArR:     pTarget->AddFireArR(fAdd);break;  // 29 添加火防御
                case ESVF_S_PoisonArR:   pTarget->AddPoisonArR(fAdd);break;  // 30 添加毒防御
                case ESVF_S_WaterAbR:    pTarget->AddWaterAbR(fAdd);break;  // 31 添加水吸收
                case ESVF_S_FireAbR:     pTarget->AddFireAbR(fAdd);break;  // 32 添加火吸收
                case ESVF_S_PoisonAbR:   pTarget->AddPoisonAbR(fAdd);break;  // 33 添加毒吸收
                case ESVF_S_AttackRateR: pTarget->AddAttackRateR(fAdd);break; // 34 添加命中
                case ESVF_S_DodgeR:      pTarget->AddDodgeR(fAdd);break; // 35 添加躲避
                case ESVF_S_AttackRangeR:pTarget->AddAttackRangeR(fAdd);break; // 36 添加攻击距离率
                case ESVF_S_CriticalR:   pTarget->AddCriticalR(fAdd);break; // 增加爆击率
				case ESVF_S_Resilience:  pTarget->AddResilience(fAdd);break;//
				case ESVF_S_AddAbsorptionDamage:pTarget->AddAbsorptionMaxDamage(fAdd);break; //最大吸收伤害(血盾--其实就是给盾加个HP)
				case ESVF_S_AddCutMaxHP: pTarget->AddCutMaxHP(fAdd);break;  //增加或减少最大HP的百分
				case ESVF_S_AddCutMaxMP: pTarget->AddCutMaxMP(fAdd);break;//增加或减少最大MP的百分比
				case ESVF_S_ReceiveDamageSkillR: pTarget->AddReceiveDamageSkillR(fAdd);break;//接收技能伤害倍率

                default: break;
                }
            }
            break;
        case ESET_IF:    // int, float
		case ESET_II:    // int, int
			{
				int irate1,irate2;
				if (bReverse) 
				{
					irate1 = -pNode->Param1.i;
					irate2 = -pNode->Param2.i;
				}
				else  
				{
					irate1 = pNode->Param1.i;
					irate2 = pNode->Param2.i;
				}
				if (pTarget)
				{
					if(pNode->pMap->eFun==ESVF_S_Rebirth)
					{
						pTarget->Rebirth(irate1,irate2);
					}
					if(pNode->pMap->eFun==ESVF_S_ExtractHpR)
					{
						pTarget->ExtractHpR(irate1,irate2);
					}
					if(pNode->pMap->eFun==ESVF_S_OrdinaryAddEffect)
					{
						pTarget->OrdinaryAddEffect(irate1,irate2);
					}
					if(pNode->pMap->eFun==ESVF_S_SeriesDamage)
					{
						pTarget->SeriesDamage(irate1,irate2);
					}
					if (pNode->pMap->eFun==ESVF_O_Conjure)
					{
						pTarget->Conjure(irate1,irate2); //召唤
					}
					if (pNode->pMap->eFun==ESVF_S_BossConjure)
					{
						pTarget->BossConjure(irate1,irate2); //BOSS召唤
					}	
					if (pNode->pMap->eFun==ESVF_S_HP)
					{
						pTarget->AddHP(irate1,irate2);
					}
					if (pNode->pMap->eFun==ESVF_S_ReboundHurtR)
					{
						pTarget->AddReboundHurtR(irate1,irate2);
					}
				}
			}
			break;

	    //Tianh   减少HP 增加MP
        case ESET_FF:    // float, float
			{
				float iHPrate,iMPrate;
				iHPrate = pNode->Param1.f;
				iMPrate = pNode->Param2.f;
				if (pTarget)
				{
					if(pNode->pMap->eFun==ESVF_S_HPMPConversion)
					{
						pTarget->HPMPConversion(iHPrate,iMPrate);break; //102减少HP 增加MP  或者增加HP 减少MP 
					}
				}	
			}
        default:
            RtCoreLog().Info("SSkillAction::Apply: 没有实现的函数类型\n");
            break;
        }
    }
}

/*----------------------------------------------------------------------------
- CSkillInstance.
----------------------------------------------------------------------------*/
CSkillInstance::CSkillInstance(unsigned short wSkillID)
{
    m_pAttr = Skill()->FindSkill(wSkillID);
    CHECK(m_pAttr!=NULL);
    m_dwResumeTime = 0;
    m_dwUserTime = 0;
    m_wExp = 0;
    m_pErrInfo = NULL;
}

CSkillInstance::~CSkillInstance()
{
}

void CSkillInstance::ContinueMask( long ResumeTime, long UseTime )
{
	m_dwMaxResumeTime = m_pAttr->iRRTime;
	m_dwMaxUserTime = m_pAttr->iRTime;
	m_dwResumeTime = ResumeTime;
	m_dwUserTime = UseTime;
}

bool CSkillInstance::DoUse(CActorSkill* pReleaser, BYTE cTargetType, DWORD dwTargetID,const float* pPos)
{
	CActorSkill* pTarget = NULL;
	if (cTargetType==SKILL_TARGET_TYPE_ACTOR)
	{
		pTarget = FindActorSkill(pReleaser->GetParent(), dwTargetID);
		if (pTarget==NULL)
		{
			MSG4("DoUse错误，没有找到目标!\n");
			return false; // 目标不存在
		}
	}else
	{
		pTarget = pReleaser;
	}

	if (!CanUse(pReleaser, cTargetType, pTarget, pPos))
	{
		// MSG4("DoUse错误，不能使用: %s\n", GetErrInfo());
		return false;
	} 


	// 扣掉MP
	int iNeedMP = m_pAttr->iMP;
	// 扣掉EP 怒气
	float iNeedEP = m_pAttr->ConsumeAnger;


	CActorSkill::SActorPassive* pStrengthenSkill = NULL;

	m_dwMaxUserTime = m_pAttr->iRTime;
	m_dwMaxResumeTime = m_pAttr->iRRTime;
	////Tianh 修改加强型技能
	if (!(m_pAttr->wManyStrSkillSub.empty()))
	{
		for(vector <int>::iterator it = m_pAttr->wManyStrSkillSub.begin() ;it!= m_pAttr->wManyStrSkillSub.end() ; it++)
		{
			pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*it);

			if (pStrengthenSkill)
			{
				if (pStrengthenSkill->pAttr->iRTime)
				{
					m_dwMaxUserTime = pStrengthenSkill->pAttr->iRTime;
				}
				if (pStrengthenSkill->pAttr->iRRTime)
				{
					m_dwMaxResumeTime = pStrengthenSkill->pAttr->iRRTime;
				}
				if (pStrengthenSkill->pAttr->dwAHitRate)
				{
					m_pAttr->dwAHitRate += pStrengthenSkill->pAttr->dwAHitRate;
				}
				if (pStrengthenSkill->pAttr->iStrStateRate)
				{
					m_pAttr->iStateRate += pStrengthenSkill->pAttr->iStateRate;
				}
				if (pStrengthenSkill->pAttr->iIntonateTime)
				{
					m_pAttr->iIntonateTime += pStrengthenSkill->pAttr->iIntonateTime;
				}
				if (pStrengthenSkill->pAttr->iBreakOdds)
				{
					m_pAttr->iBreakOdds += pStrengthenSkill->pAttr->iBreakOdds;
				}
				if (pStrengthenSkill->pAttr->iBeBreakOdds)
				{
					m_pAttr->iBeBreakOdds += pStrengthenSkill->pAttr->iBeBreakOdds;
				}
				if (pStrengthenSkill->pAttr->wHits)
				{
					m_pAttr->wHits += pStrengthenSkill->pAttr->wHits;
				}
				if (pStrengthenSkill->pAttr->bAType)
				{
					m_pAttr->bAType += pStrengthenSkill->pAttr->bAType;
				}
				if (pStrengthenSkill->pAttr->iMP)
				{
					iNeedMP += pStrengthenSkill->pAttr->iMP; //加强类技能消耗MP
				}
			    
				//Tian 如果是法宝加强技能  就消耗怒气
				if (1 == m_pAttr->dwType)
				{
					iNeedEP += pStrengthenSkill->pAttr->ConsumeAnger;
				}
				
			}
		}
	}

	pReleaser->AddMP(-iNeedMP);
	if (1 == m_pAttr->dwType)
	{
		pReleaser->AddEnP(-iNeedEP);
	}

	//Tianh 不同类型的公共CD时间  10.02.22
	if (m_pAttr->wClass == 1 || m_pAttr->wClass == 2 || m_pAttr->wClass == 3 || m_pAttr->wClass == 4 || m_pAttr->wClass == 5
		||m_pAttr->wClass == 6 || m_pAttr->wClass == 7 || m_pAttr->wClass == 8 ||m_pAttr->wClass == 9
		||m_pAttr->wClass == 10||m_pAttr->wClass == 11 ||m_pAttr->wClass == 12)
	{
		pReleaser->m_SchoolPublicCDTime = School_PublicCDMaxTime; //普通主动技能公共技能CD是1.8秒
	}
	else if (m_pAttr->wClass == 13)
	{
		pReleaser->m_WapPublicCDTime = Wap_PublicCDMaxTime; //法宝技能公共技能CD是2秒
	}
	//else if (m_pAttr->wClass == 0) //道具类
	//{
	//	pReleaser->m_PropPublicCDTime = Prop_PublicCDMaxTime;
	//}
	else if (m_pAttr->wClass == 21)  //驯兽
	{
		pReleaser->m_beastPublicCDTime = beast_PublicCDMaxTime;
	}
    m_dwResumeTime = m_dwMaxResumeTime;
    m_dwUserTime = m_dwMaxUserTime;

    OnUseBefore(pReleaser);

	if (m_pAttr->IsAddHpSkill)
	{
		if (cTargetType==SKILL_TARGET_TYPE_ACTOR || cTargetType==SKILL_TARGET_TYPE_NONE)
		{	
			if (!m_pAttr->bAType)
			{
				pTarget->OnSkillAddHP(m_pAttr, pReleaser);
			}
			else
			{
				pPos = pTarget->GetSelfPos();
				if (pPos)
				{
					if (m_pAttr->bAType) // 如果是单体攻击就直接返回，这里只支持范围攻击
					{
						pReleaser->OnUseRangeSkillAddHp(m_pAttr, pPos,pReleaser);
					}
				}
			}
		}
	}
	else
	{
		// 攻击
		if (cTargetType==SKILL_TARGET_TYPE_ACTOR || cTargetType==SKILL_TARGET_TYPE_NONE)
		{
			if (!m_pAttr->bAType) // 单体攻击
			{
				pTarget->OnSkillUse(m_pAttr, pReleaser);
				//Tianh 10.04.07做出修正

				/*m_pErrInfo = R(SMSG_SKILL_USE_SUCCEED);
				OnUseAfter(pReleaser);*/
				/* return true;*/
			}else
			{
				pPos = pTarget->GetSelfPos();
			}
		}

		if (pPos)
		{
			if (m_pAttr->bAType) // 如果是单体攻击就直接返回，这里只支持范围攻击
			{
				pReleaser->OnUseRangeSkill(m_pAttr, pPos);
			}
		}

	}
    m_pErrInfo = R(SMSG_SKILL_USE_SUCCEED);
    OnUseAfter(pReleaser);

	////Tianh 修改加强型技能 这里记得释放完技能要把修改的值改回去
	if (!(m_pAttr->wManyStrSkillSub.empty()))
	{
		for(vector <int>::iterator it = m_pAttr->wManyStrSkillSub.begin() ;it!= m_pAttr->wManyStrSkillSub.end() ; it++)
		{
			pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));

			if (pStrengthenSkill)
			{
				if (pStrengthenSkill->pAttr->dwAHitRate)
				{
					m_pAttr->dwAHitRate -= pStrengthenSkill->pAttr->dwAHitRate;
				}
				if (pStrengthenSkill->pAttr->iStrStateRate)
				{
					m_pAttr->iStateRate -= pStrengthenSkill->pAttr->iStrStateRate;
				}
				if (pStrengthenSkill->pAttr->iIntonateTime)
				{
					m_pAttr->iIntonateTime -= pStrengthenSkill->pAttr->iIntonateTime;
				}
				if (pStrengthenSkill->pAttr->iBreakOdds)
				{
					m_pAttr->iBreakOdds -= pStrengthenSkill->pAttr->iBreakOdds;
				}
				if (pStrengthenSkill->pAttr->iBeBreakOdds)
				{
					m_pAttr->iBeBreakOdds -= pStrengthenSkill->pAttr->iBeBreakOdds;
				}
				if (pStrengthenSkill->pAttr->wHits)
				{
					m_pAttr->wHits -= pStrengthenSkill->pAttr->wHits;
				}
				if (pStrengthenSkill->pAttr->bAType)
				{
					m_pAttr->bAType -= pStrengthenSkill->pAttr->bAType;
				}
			}
		}
	}

    return true;
}


void CSkillInstance::OnRun(unsigned long dwMillisecond,CActorSkill* pReleaser)
{
	if (m_dwResumeTime>dwMillisecond)
	{
		m_dwResumeTime -= dwMillisecond;
	}else
	{
		m_dwResumeTime = 0;
	}

	if (m_dwUserTime>dwMillisecond)
	{
		m_dwUserTime -= dwMillisecond;
	}else
	{
		m_dwUserTime = 0;
	}
	OnSkillRun(dwMillisecond,pReleaser); 
}

bool CSkillInstance::CanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget,const float* pPos, bool bIgnoreDistance)
{
	if (!OnCanUse(pReleaser, cTargetType, pTarget, pPos, bIgnoreDistance))
	{
		return false;
	}

    // 技能释放目标
    if (cTargetType==SKILL_TARGET_TYPE_ACTOR)
    {
		if (pTarget==NULL)
        {
            m_pErrInfo = R(MSG_CANNOT_FIND_TARGET);
            return false;
        }
        if ((m_pAttr->dwRTarget&(~SKILL_TARGET_POSITION))==0)
        {
            m_pErrInfo = R(SMSG_TARGET_ONLY_POSITION);
            return false;
        }
    }else if (cTargetType==SKILL_TARGET_TYPE_POSITION)
    {
        if (pPos==NULL)
        {
            m_pErrInfo = R(MSG_CANNOT_FIND_POSITION);
            return false;
        }
        if ((m_pAttr->dwRTarget&SKILL_TARGET_POSITION)==0)
        {
            m_pErrInfo = R(SMSG_TARGET_NOT_POSITION);
            return false;
        }
        if (!m_pAttr->bAType)
        {
            m_pErrInfo = R(SMSG_POSITIONSKILL_NEED_RANGE);
            return false;
        }
    }else if (cTargetType==SKILL_TARGET_TYPE_NONE)
    {
    }else
    {
        m_pErrInfo = R(SMSG_TARGET_ERROR);
        return false;
    }

    // 技能使用距离判断
    if (!bIgnoreDistance && cTargetType!=SKILL_TARGET_TYPE_NONE)
    {
        float fDistance;
        const float* pSelfPos, *pTargetPos;
        pSelfPos = pReleaser->GetSelfPos();
        if (cTargetType==SKILL_TARGET_TYPE_ACTOR)
        {
            pTargetPos = pTarget->GetSelfPos();
        }else if (cTargetType==SKILL_TARGET_TYPE_POSITION)
        {
            pTargetPos = pPos;
        }

        float xSq = pSelfPos[0] - pTargetPos[0];
        float ySq = pSelfPos[1] - pTargetPos[1];
        fDistance = fsqrt(xSq*xSq + ySq*ySq);

        int iMyRMinDest, iMyRMaxDest;

        CActorSkill::SActorPassive* pStrengthenSkill = NULL;
		
		iMyRMinDest = m_pAttr->iRMinDest;
		iMyRMaxDest = m_pAttr->iRMaxDest;
		////Tianh 修改加强型技能
		if (!(m_pAttr->wManyStrSkillSub.empty()))
		{
			for(vector <int>::iterator it = m_pAttr->wManyStrSkillSub.begin() ;it!= m_pAttr->wManyStrSkillSub.end() ; it++)
			{
				pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));

				if (pStrengthenSkill)
				{
					if (pStrengthenSkill->pAttr->iRMinDest)
					{
						iMyRMinDest += pStrengthenSkill->pAttr->iRMinDest;
					}
					if (pStrengthenSkill->pAttr->iRMaxDest)
					{
						iMyRMaxDest += pStrengthenSkill->pAttr->iRMaxDest;
					}

				}
			}
		}

		////Tianh 修改加强型技能
		//if (m_pAttr->wStrSkillSub)
		//{
		//	int j = 3;
		//	for (int i = 0;i <= j;i++)
		//	{
		//		if (m_pAttr->wManyStrSkillSub[i] == 0)
		//		{
		//			pStrengthenSkill = NULL;
		//			j = 0;
		//		}
		//		else
		//		{
		//			pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(m_pAttr->wManyStrSkillSub[i]);
		//		}
		//		if (pStrengthenSkill)
		//		{
		//			if (pStrengthenSkill->pAttr->iRMinDest)
		//			{
		//				iMyRMinDest = pStrengthenSkill->pAttr->iRMinDest;
		//			}else
		//			{
		//				iMyRMinDest = m_pAttr->iRMinDest;
		//			}
		//			if (pStrengthenSkill->pAttr->iRMaxDest)
		//			{
		//				iMyRMaxDest = pStrengthenSkill->pAttr->iRMaxDest;
		//			}else
		//			{
		//				iMyRMaxDest = m_pAttr->iRMaxDest;
		//			}
		//		}else
		//		{
		//			iMyRMinDest = m_pAttr->iRMinDest;
		//			iMyRMaxDest = m_pAttr->iRMaxDest;
		//		}
		//	}
		//}

#ifdef REGION_SERVER
        if (fDistance<iMyRMinDest - 60.f) // 服务器允许误差60
#else
        if (fDistance<iMyRMinDest - 60.f)
#endif
        {
            m_pErrInfo = R(SMSG_DISTANCE_TOO_NEAR);
            return false;
        }

#ifdef REGION_SERVER
        if (fDistance>iMyRMaxDest + 60.f) // 服务器允许误差40
#else
        if (fDistance>iMyRMaxDest + 60.f)
#endif
        {
            m_pErrInfo = R(SMSG_DISTANCE_TOO_FAR);
            return false;
        }
    }

    // 技能冲突判断
    if (cTargetType==SKILL_TARGET_TYPE_ACTOR)
    {
        // TODO: 察看pTarget身上的技能，如果在冲突列表中，就返回false
    }

    // 技能使用条件判断
    const char* pFailMsg = m_pAttr->filterUse.IsSuccess(pReleaser);
    if (pFailMsg==NULL) return true;
    m_pErrInfo = pFailMsg;
    return false;
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

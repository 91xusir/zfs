#ifndef ITEM_NET_DEFINE_H
#define ITEM_NET_DEFINE_H


const short r2c_item					= 1000;
const short r2g_item					= 3000;
const short c2r_item					= 9000;
const short c2g_item					= 5000;

const short r2c_item_errorcode			= r2c_item + 0;		// 错误信息
// 测试Client与Server的道具栏是否一致
const short r2c_check_item				= r2c_item + 1999;
// 玩家背包、道具操作
const short r2c_bag_additem				= r2c_item + 3;		// Bag里增加物品
const short r2c_bag_removeitem			= r2c_item + 4;		// Bag里移除物品
const short r2c_bag_addmoney			= r2c_item + 5;		// 人物背包里增加钱币   （打怪掉钱，不掉到地上，而是直接掉到人物的背包中）
const short r2c_player_usepileitem		= r2c_item + 6;		// 使用有使用次数的道具
const short r2c_player_move_item		= r2c_item + 7;		// 移动道具（在CBag与CBag之间）
const short r2c_bag_refresh				= r2c_item + 8;		// 刷新Bag
const short r2c_player_equipitem		= r2c_item + 9;
const short r2c_player_unequipitem		= r2c_item + 10;
const short r2c_player_use_pet			= r2c_item + 11;	// （反）激活宠物
const short r2c_player_itemupdate_result = r2c_item + 12;	// 锻造结果
const short r2c_player_gem_result       = r2c_item + 13;	// 宝石合成结果
const short r2c_refresh_single_item		= r2c_item + 14;	// 刷新一个道具
const short r2c_exchange_hand_weapon	= r2c_item + 15;	// 交换左右手道具
const short r2c_pickitem_fail			= r2c_item + 16;	// 
const short r2c_refresh_equip_item		= r2c_item + 17;	// 刷新一个已装备的道具
const short r2c_equip_item_state		= r2c_item + 18;	// 装备的道具是否有效
const short r2c_use_magicitem_result	= r2c_item + 19;    // 使用彩蛋的结果
const short r2c_other_additem			= r2c_item + 20;
const short r2c_peteat_result           = r2c_item + 21;    // 麒麟吞噬的结果
const short r2c_player_dispart_item		= r2c_item + 22;		// 分离道具（在CBag与CBag之间）
// 与玩家交易
const short r2c_trade_other_invite			= r2c_item + 100;	// 收到另一玩家的邀请
const short r2c_trade_other_invite_response	= r2c_item + 101;	// 收到与另一玩家交易请求的反馈
const short r2c_trade_begin					= r2c_item + 102;	// 交易开始
const short r2c_trade_lock			        = r2c_item + 103;	// 交易界面的按键:确定
const short r2c_trade_other_agree			= r2c_item + 104;	// 收到另一玩家的交易界面的按键:交易
const short r2c_trade_unlock				= r2c_item + 105;	// 由于某种原因“确定”按钮被弹起（如玩家lock后又放入了或拿走了物品）
const short r2c_trade_deal					= r2c_item + 106;	// 交易成功
const short r2c_trade_cancel				= r2c_item + 107;	// 交易取消
// 系统买卖
const short r2c_shop_buy_query_response		= r2c_item + 200;	// NPC的出售菜单
const short r2c_shop_buy_result				= r2c_item + 201;	// 购买结果
const short r2c_shop_sell_result			= r2c_item + 203;	// 出售结果
const short r2c_point_result				= r2c_item + 204;	// 商城消费后剩余点数  heten
const short r2c_requstID					= r2c_item + 205;
// 摆摊
const short r2c_pshop_start_response		= r2c_item + 302;	// 摆摊开始反馈
const short r2c_pshop_stop_response			= r2c_item + 303;	// 摆摊结束反馈
const short r2c_pshop_content				= r2c_item + 310;	// 察看另一玩家的个人商店
const short r2c_pshop_buy_result			= r2c_item + 311;	// 向另一玩家购买物品的结果
const short r2c_pshop_other_buy				= r2c_item + 313;	// 另一玩家购买了你的物品
const short r2c_pshop_other_sell         = r2c_item + 314;   // 摆摊卖的反馈
const short r2c_pshop_sell_result           = r2c_item + 315;   // 摆摊卖的反馈

// 钱庄
const short r2c_bank_query_response			= r2c_item + 401;	// 返回钱庄物品(金币)
const short r2c_bank_store_response			= r2c_item + 402;	// 寄存物品(金币)反馈
const short r2c_bank_withdraw_response		= r2c_item + 403;	// 提取物品(金币)反馈
const short r2c_bank_addpass_result			= r2c_item + 404;	// 添加密码
const short r2c_bank_removepass_result		= r2c_item + 405;	// 去除密码
const short r2c_bank_changepass_result		= r2c_item + 406;	// 
const short r2c_bank_pass_query				= r2c_item + 407;
const short r2c_bank_query_response_pet		= r2c_item + 408;	// 察看仓库的宠物反馈
const short r2c_bank_store_response_pet		= r2c_item + 409;	// 寄存宠物反馈
const short r2c_bank_withdraw_response_pet	= r2c_item + 410;	// 提取宠物反馈
// 生活技能
const short r2c_compose_item_begin			= r2c_item + 500;	// 合成道具开始
const short r2c_compose_item_end			= r2c_item + 501;	// 合成道具结束
const short r2c_decompose_item_result		= r2c_item + 502;	// 分解装备结果
const short r2c_enchant_item_result			= r2c_item + 503;	// 附魔结果
const short r2c_fuse_item_result			= r2c_item + 504;	// 融合（二合一）
const short r2c_fuse_gem_result				= r2c_item + 505;	// 宝石合成（五合一）



// 测试Client与Server的道具栏是否一致
const short c2r_check_item			= c2r_item + 1999;
// 玩家背包、道具操作
const short c2r_player_pickitem		= c2r_item + 1;	// 从地上拣起物品
const short c2r_player_throwitem	= c2r_item + 2;	// 把物品丢到地上
const short c2r_player_deleteitem	= c2r_item + 3;	// 删除物品
const short c2r_player_moveitem		= c2r_item + 4;	// 移动物品（从某一个容器的某一个格子移动到另一个容器的另一个格子）
const short c2r_player_equipitem	= c2r_item + 5;	// 把物品装备起来
const short c2r_player_unequipitem	= c2r_item + 6;	// 把装备拿下来
const short c2r_player_use_item		= c2r_item + 9; // 使用道具
const short c2r_player_itemupdate	= c2r_item + 10;// 锻造
const short c2r_player_itemlock     = c2r_item + 11;// 给道具加锁（需要仓库密码）
const short c2r_player_itemunlock   = c2r_item + 12;// 给道具解锁
const short c2r_player_peteat       = c2r_item + 13;// 麒麟吞噬
const short c2r_player_switchweapon = c2r_item + 14;// 切换武器
const short c2r_player_dispart		= c2r_item + 15;// 分离叠加物品
// 与玩家交易
const short c2r_trade_invite			= c2r_item + 100;	// 与另一玩家交易的邀请
const short c2r_trade_invite_response	= c2r_item + 101;	// 回应是否愿意与另一玩家交易
const short c2r_trade_lock				= c2r_item + 102;	// 与另一玩家交易时按"确定"
const short c2r_trade_unlock			= c2r_item + 103;	// 
const short c2r_trade_deal				= c2r_item + 104;	// 与另一玩家交易时按"交易"
const short c2r_trade_cancel			= c2r_item + 105;	// 与另一玩家交易时取消
// 系统买卖
const short c2r_shop_query			= c2r_item + 200;		// 点击与NPC交易
const short c2r_shop_buy			= c2r_item + 201;		// 与NPC交易--点击buy按钮
const short c2r_shop_sell			= c2r_item + 202;		// 与NPC交易--点击sell按钮
const short c2r_shop_autosell		= c2r_item + 203;		//tim.yang  自动交易
const short c2r_requstID			= c2r_item + 204;		//heten
// 摆摊
const short c2r_pshop_start			= c2r_item + 303;	// 摆摊-开始摆摊
const short c2r_pshop_stop			= c2r_item + 304;	// 摆摊-停止摆摊
const short c2r_pshop_query_other	= c2r_item + 305;	// 察看另一玩家的个人商店
const short c2r_pshop_buy			= c2r_item + 306;	// 在另一玩家的个人商店中买东西
const short c2r_pshop_sell			= c2r_item + 307;	// 在另一玩家的个人商店中卖东西
// 钱庄
const short c2r_bank_query			= c2r_item + 401;	// 察看钱庄物品(金币)
const short c2r_bank_store			= c2r_item + 402;	// 钱庄-寄存物品(金币)
const short c2r_bank_withdraw		= c2r_item + 403;	// 钱庄-提取物品(金币)
const short c2r_bank_addpass		= c2r_item + 404;
const short c2r_bank_removepass		= c2r_item + 405;
const short c2r_bank_changepass		= c2r_item + 406;
const short c2r_bank_store_pet		= c2r_item + 407;	// 寄存宠物
const short c2r_bank_withdraw_pet	= c2r_item + 408;	// 提取宠物
const short c2r_bank_query_pet		= c2r_item + 409;	// 察看仓库的宠物
// 生活技能
const short c2r_compose_item		= c2r_item + 500;	// 合成道具（铁匠、制药、炼金）
const short c2r_compose_item_stop	= c2r_item + 501;	// 停止合成
const short c2r_decompose_item		= c2r_item + 502;	// 分解装备
const short c2r_enchant_item		= c2r_item + 503;	// 附魔
const short c2r_fuse_item			= c2r_item + 504;	// 融合（二合一）
// 法宝
const short c2r_rifine_trump		= c2r_item + 505;	// 法宝炼化
const short c2r_fuse_gem			= c2r_item + 506;	// 宝石合成（五合一）

const short c2r_serashop_buy	    = c2r_item + 507;	// 商城的购买
const short c2r_serashop_giveaway	= c2r_item + 508;	// 商城的赠送


// ERROR_CODE
const short itemmsg_success					= 0;
const short itemmsg_deal_bagfull			= 1;	// 玩家交易不成功，因为背包放不下了
const short itemmsg_unknown_error			= 2;	// 道具合成失败
const short itemmsg_decompose_bagfull		= 3;	// 分解时背包放不下
const short itemmsg_compose_learned			= 4;	// 已经学会了该配方
const short itemmsg_compose_bagfull			= 5;	// 合成道具时背包已满
const short itemmsg_bankpass_error          = 6;    // 交易时需要仓库密码，而提供的仓库密码不正确
const short itemmsg_money_error             = 7;    // 玩家交易失败 ，金钱超过最大上限  
const short itemmsg_time_non_arrival        = 8;    // 无法交易，上线保护中，请稍后再试
const short itemmsg_time_far                = 9;    //交易距离过远


// Server回传给Client时，可能会用到以下附加常量
#define ITEM_REASON_NULL					0 
#define ITEM_REASON_THROW					1
#define ITEM_REASON_DELETE					2
#define ITEM_REASON_PICK					3
#define ITEM_REASON_TRADE					4
#define ITEM_REASON_SHOP					5
#define ITEM_REASON_EQUIP					6
#define ITEM_REASON_UNEQUIP					7
#define ITEM_REASON_PSHOP_NOITEM			8
#define ITEM_REASON_PK_DROP					9
#define ITEM_REASON_PSHOP_NOPLAYER			10
#define	ITEM_REASON_PSHOP_NOPSHOP			11
#define ITEM_REASON_TOO_FAR					12 // 距离太远
#define ITEM_REASON_CONTAINER_FULL			13 //


#endif//ITEM_NET_DEFINE_H

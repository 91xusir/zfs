#ifndef ITEM_NET_DEFINE_H
#define ITEM_NET_DEFINE_H


const short r2c_item					= 1000;
const short r2g_item					= 3000;
const short c2r_item					= 9000;
const short c2g_item					= 5000;

const short r2c_item_errorcode			= r2c_item + 0;		// ������Ϣ
// ����Client��Server�ĵ������Ƿ�һ��
const short r2c_check_item				= r2c_item + 1999;
// ��ұ��������߲���
const short r2c_bag_additem				= r2c_item + 3;		// Bag��������Ʒ
const short r2c_bag_removeitem			= r2c_item + 4;		// Bag���Ƴ���Ʒ
const short r2c_bag_addmoney			= r2c_item + 5;		// ���ﱳ��������Ǯ��   ����ֵ�Ǯ�����������ϣ�����ֱ�ӵ�������ı����У�
const short r2c_player_usepileitem		= r2c_item + 6;		// ʹ����ʹ�ô����ĵ���
const short r2c_player_move_item		= r2c_item + 7;		// �ƶ����ߣ���CBag��CBag֮�䣩
const short r2c_bag_refresh				= r2c_item + 8;		// ˢ��Bag
const short r2c_player_equipitem		= r2c_item + 9;
const short r2c_player_unequipitem		= r2c_item + 10;
const short r2c_player_use_pet			= r2c_item + 11;	// �������������
const short r2c_player_itemupdate_result = r2c_item + 12;	// ������
const short r2c_player_gem_result       = r2c_item + 13;	// ��ʯ�ϳɽ��
const short r2c_refresh_single_item		= r2c_item + 14;	// ˢ��һ������
const short r2c_exchange_hand_weapon	= r2c_item + 15;	// ���������ֵ���
const short r2c_pickitem_fail			= r2c_item + 16;	// 
const short r2c_refresh_equip_item		= r2c_item + 17;	// ˢ��һ����װ���ĵ���
const short r2c_equip_item_state		= r2c_item + 18;	// װ���ĵ����Ƿ���Ч
const short r2c_use_magicitem_result	= r2c_item + 19;    // ʹ�òʵ��Ľ��
const short r2c_other_additem			= r2c_item + 20;
const short r2c_peteat_result           = r2c_item + 21;    // �������ɵĽ��
const short r2c_player_dispart_item		= r2c_item + 22;		// ������ߣ���CBag��CBag֮�䣩
// ����ҽ���
const short r2c_trade_other_invite			= r2c_item + 100;	// �յ���һ��ҵ�����
const short r2c_trade_other_invite_response	= r2c_item + 101;	// �յ�����һ��ҽ�������ķ���
const short r2c_trade_begin					= r2c_item + 102;	// ���׿�ʼ
const short r2c_trade_lock			        = r2c_item + 103;	// ���׽���İ���:ȷ��
const short r2c_trade_other_agree			= r2c_item + 104;	// �յ���һ��ҵĽ��׽���İ���:����
const short r2c_trade_unlock				= r2c_item + 105;	// ����ĳ��ԭ��ȷ������ť�����������lock���ַ����˻���������Ʒ��
const short r2c_trade_deal					= r2c_item + 106;	// ���׳ɹ�
const short r2c_trade_cancel				= r2c_item + 107;	// ����ȡ��
// ϵͳ����
const short r2c_shop_buy_query_response		= r2c_item + 200;	// NPC�ĳ��۲˵�
const short r2c_shop_buy_result				= r2c_item + 201;	// ������
const short r2c_shop_sell_result			= r2c_item + 203;	// ���۽��
const short r2c_point_result				= r2c_item + 204;	// �̳����Ѻ�ʣ�����  heten
const short r2c_requstID					= r2c_item + 205;
// ��̯
const short r2c_pshop_start_response		= r2c_item + 302;	// ��̯��ʼ����
const short r2c_pshop_stop_response			= r2c_item + 303;	// ��̯��������
const short r2c_pshop_content				= r2c_item + 310;	// �쿴��һ��ҵĸ����̵�
const short r2c_pshop_buy_result			= r2c_item + 311;	// ����һ��ҹ�����Ʒ�Ľ��
const short r2c_pshop_other_buy				= r2c_item + 313;	// ��һ��ҹ����������Ʒ
const short r2c_pshop_other_sell         = r2c_item + 314;   // ��̯���ķ���
const short r2c_pshop_sell_result           = r2c_item + 315;   // ��̯���ķ���

// Ǯׯ
const short r2c_bank_query_response			= r2c_item + 401;	// ����Ǯׯ��Ʒ(���)
const short r2c_bank_store_response			= r2c_item + 402;	// �Ĵ���Ʒ(���)����
const short r2c_bank_withdraw_response		= r2c_item + 403;	// ��ȡ��Ʒ(���)����
const short r2c_bank_addpass_result			= r2c_item + 404;	// �������
const short r2c_bank_removepass_result		= r2c_item + 405;	// ȥ������
const short r2c_bank_changepass_result		= r2c_item + 406;	// 
const short r2c_bank_pass_query				= r2c_item + 407;
const short r2c_bank_query_response_pet		= r2c_item + 408;	// �쿴�ֿ�ĳ��ﷴ��
const short r2c_bank_store_response_pet		= r2c_item + 409;	// �Ĵ���ﷴ��
const short r2c_bank_withdraw_response_pet	= r2c_item + 410;	// ��ȡ���ﷴ��
// �����
const short r2c_compose_item_begin			= r2c_item + 500;	// �ϳɵ��߿�ʼ
const short r2c_compose_item_end			= r2c_item + 501;	// �ϳɵ��߽���
const short r2c_decompose_item_result		= r2c_item + 502;	// �ֽ�װ�����
const short r2c_enchant_item_result			= r2c_item + 503;	// ��ħ���
const short r2c_fuse_item_result			= r2c_item + 504;	// �ںϣ�����һ��
const short r2c_fuse_gem_result				= r2c_item + 505;	// ��ʯ�ϳɣ����һ��



// ����Client��Server�ĵ������Ƿ�һ��
const short c2r_check_item			= c2r_item + 1999;
// ��ұ��������߲���
const short c2r_player_pickitem		= c2r_item + 1;	// �ӵ��ϼ�����Ʒ
const short c2r_player_throwitem	= c2r_item + 2;	// ����Ʒ��������
const short c2r_player_deleteitem	= c2r_item + 3;	// ɾ����Ʒ
const short c2r_player_moveitem		= c2r_item + 4;	// �ƶ���Ʒ����ĳһ��������ĳһ�������ƶ�����һ����������һ�����ӣ�
const short c2r_player_equipitem	= c2r_item + 5;	// ����Ʒװ������
const short c2r_player_unequipitem	= c2r_item + 6;	// ��װ��������
const short c2r_player_use_item		= c2r_item + 9; // ʹ�õ���
const short c2r_player_itemupdate	= c2r_item + 10;// ����
const short c2r_player_itemlock     = c2r_item + 11;// �����߼�������Ҫ�ֿ����룩
const short c2r_player_itemunlock   = c2r_item + 12;// �����߽���
const short c2r_player_peteat       = c2r_item + 13;// ��������
const short c2r_player_switchweapon = c2r_item + 14;// �л�����
const short c2r_player_dispart		= c2r_item + 15;// ���������Ʒ
// ����ҽ���
const short c2r_trade_invite			= c2r_item + 100;	// ����һ��ҽ��׵�����
const short c2r_trade_invite_response	= c2r_item + 101;	// ��Ӧ�Ƿ�Ը������һ��ҽ���
const short c2r_trade_lock				= c2r_item + 102;	// ����һ��ҽ���ʱ��"ȷ��"
const short c2r_trade_unlock			= c2r_item + 103;	// 
const short c2r_trade_deal				= c2r_item + 104;	// ����һ��ҽ���ʱ��"����"
const short c2r_trade_cancel			= c2r_item + 105;	// ����һ��ҽ���ʱȡ��
// ϵͳ����
const short c2r_shop_query			= c2r_item + 200;		// �����NPC����
const short c2r_shop_buy			= c2r_item + 201;		// ��NPC����--���buy��ť
const short c2r_shop_sell			= c2r_item + 202;		// ��NPC����--���sell��ť
const short c2r_shop_autosell		= c2r_item + 203;		//tim.yang  �Զ�����
const short c2r_requstID			= c2r_item + 204;		//heten
// ��̯
const short c2r_pshop_start			= c2r_item + 303;	// ��̯-��ʼ��̯
const short c2r_pshop_stop			= c2r_item + 304;	// ��̯-ֹͣ��̯
const short c2r_pshop_query_other	= c2r_item + 305;	// �쿴��һ��ҵĸ����̵�
const short c2r_pshop_buy			= c2r_item + 306;	// ����һ��ҵĸ����̵�������
const short c2r_pshop_sell			= c2r_item + 307;	// ����һ��ҵĸ����̵���������
// Ǯׯ
const short c2r_bank_query			= c2r_item + 401;	// �쿴Ǯׯ��Ʒ(���)
const short c2r_bank_store			= c2r_item + 402;	// Ǯׯ-�Ĵ���Ʒ(���)
const short c2r_bank_withdraw		= c2r_item + 403;	// Ǯׯ-��ȡ��Ʒ(���)
const short c2r_bank_addpass		= c2r_item + 404;
const short c2r_bank_removepass		= c2r_item + 405;
const short c2r_bank_changepass		= c2r_item + 406;
const short c2r_bank_store_pet		= c2r_item + 407;	// �Ĵ����
const short c2r_bank_withdraw_pet	= c2r_item + 408;	// ��ȡ����
const short c2r_bank_query_pet		= c2r_item + 409;	// �쿴�ֿ�ĳ���
// �����
const short c2r_compose_item		= c2r_item + 500;	// �ϳɵ��ߣ���������ҩ������
const short c2r_compose_item_stop	= c2r_item + 501;	// ֹͣ�ϳ�
const short c2r_decompose_item		= c2r_item + 502;	// �ֽ�װ��
const short c2r_enchant_item		= c2r_item + 503;	// ��ħ
const short c2r_fuse_item			= c2r_item + 504;	// �ںϣ�����һ��
// ����
const short c2r_rifine_trump		= c2r_item + 505;	// ��������
const short c2r_fuse_gem			= c2r_item + 506;	// ��ʯ�ϳɣ����һ��

const short c2r_serashop_buy	    = c2r_item + 507;	// �̳ǵĹ���
const short c2r_serashop_giveaway	= c2r_item + 508;	// �̳ǵ�����


// ERROR_CODE
const short itemmsg_success					= 0;
const short itemmsg_deal_bagfull			= 1;	// ��ҽ��ײ��ɹ�����Ϊ�����Ų�����
const short itemmsg_unknown_error			= 2;	// ���ߺϳ�ʧ��
const short itemmsg_decompose_bagfull		= 3;	// �ֽ�ʱ�����Ų���
const short itemmsg_compose_learned			= 4;	// �Ѿ�ѧ���˸��䷽
const short itemmsg_compose_bagfull			= 5;	// �ϳɵ���ʱ��������
const short itemmsg_bankpass_error          = 6;    // ����ʱ��Ҫ�ֿ����룬���ṩ�Ĳֿ����벻��ȷ
const short itemmsg_money_error             = 7;    // ��ҽ���ʧ�� ����Ǯ�����������  
const short itemmsg_time_non_arrival        = 8;    // �޷����ף����߱����У����Ժ�����
const short itemmsg_time_far                = 9;    //���׾����Զ


// Server�ش���Clientʱ�����ܻ��õ����¸��ӳ���
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
#define ITEM_REASON_TOO_FAR					12 // ����̫Զ
#define ITEM_REASON_CONTAINER_FULL			13 //


#endif//ITEM_NET_DEFINE_H

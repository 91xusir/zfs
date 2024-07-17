
#ifndef AUCTION_NET_DEFINE_H
#define AUCTION_NET_DEFINE_H

const short c2r_auction_cmd_base = 15000;
const short r2c_auction_cmd_base = 18000;
const short r2g_auction_cmd_base = 25000;
const short g2r_auction_cmd_base = 28000;

//region只是转发

const short c2r_auction_sale		= c2r_auction_cmd_base + 1; //拍卖物品
const short c2r_auction_buy			= c2r_auction_cmd_base + 2; //购买物品
const short c2r_auction_cancel_buy	= c2r_auction_cmd_base + 3; //取消购买
const short c2r_auction_view		= c2r_auction_cmd_base + 4; //浏览
const short c2r_auction_get_item	= c2r_auction_cmd_base + 5; //领取物品
const short c2r_auction_update		= c2r_auction_cmd_base + 6; //update

const short r2g_auction_sale		= r2g_auction_cmd_base + 1; //拍卖物品
const short r2g_auction_buy			= r2g_auction_cmd_base + 2; //购买物品
const short r2g_auction_cancel_buy	= r2g_auction_cmd_base + 3; //取消购买
const short r2g_auction_view		= r2g_auction_cmd_base + 4; //浏览
const short r2g_auction_get_item	= r2g_auction_cmd_base + 5; //领取物品

const short r2c_auction_sale_rep		= r2c_auction_cmd_base + 1; //拍卖物品
const short r2c_auction_buy_rep			= r2c_auction_cmd_base + 2; //购买物品
const short r2c_auction_cancel_buy_rep	= r2c_auction_cmd_base + 3; //取消购买
const short r2c_auction_view_rep		= r2c_auction_cmd_base + 4; //浏览
const short r2c_auction_get_item_rep	= r2c_auction_cmd_base + 5; //浏览
const short r2c_auction_update			= r2c_auction_cmd_base + 6;

const short g2r_auction_sale_rep		= g2r_auction_cmd_base + 1; //拍卖物品响应
const short g2r_auction_buy_rep			= g2r_auction_cmd_base + 2; //购买物品响应
const short g2r_auction_cancel_buy_rep	= g2r_auction_cmd_base + 3; //取消购买响应
const short g2r_auction_view_rep		= g2r_auction_cmd_base + 4; //浏览响应
const short g2r_auction_data_init		= g2r_auction_cmd_base + 5; //初始化
const short g2r_auction_data_all_update	= g2r_auction_cmd_base + 6; //全部更新
const short g2r_auction_data_update		= g2r_auction_cmd_base + 7; //部分更新
const short g2r_auction_get_item_rep	= g2r_auction_cmd_base + 8; //部分更新

//-------------------------------------
// msg define
//-------------------------------------
#define AUCTION_SALE_SUCCESS				0
#define AUCTION_SALE_ITEM_EXIST				1
#define AUCTION_SALE_ITEM_CANNOT_TRADE		2
#define AUCTION_SALE_NO_MONEY				3
#define AUCTION_SALE_NUM_OVERFLOW			4
#define AUCTION_SALE_ABORT					5
#define AUCTION_SALE_UNKNOWN				6
#define AUCTION_BID_ACCEPT					7
#define AUCTION_BID_NO_MONEY				8
#define AUCTION_BID_PRICE_INVALID			9
#define AUCTION_BID_ITEM_OVERFLOW			10
#define AUCTION_BUY_SUCCESS					11
#define AUCTION_BUY_PRICE_IS_EXCEEDED		12
#define AUCTION_BUY_ERROR_SAMEACC			13
#define AUCTION_SALESROOM_FULL				14
#define AUCTION_CANCEL_BUY_NOITEM			15
#define AUCTION_CANCEL_BUY_SUCCESS			16
#define AUCTION_GET_SALE_SUCCESS_ITEM		17
#define AUCTION_GET_SALE_FAILED_ITEM		18
#define AUCTION_GET_BUY_SUCCESS_ITEM		19
#define AUCTION_GET_BUY_FAILED_ITEM			20

#endif //AUCTION_NET_DEFINE_H


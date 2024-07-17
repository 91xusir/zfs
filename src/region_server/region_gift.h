#ifndef __REGION_GIFT_H__
#define __REGION_GIFT_H__


class CRegionGiftMgr
{
public:
	CRegionGiftMgr() {}
	~CRegionGiftMgr() {}

	static bool WithdrawGift(SGift& gift, char& errorCode);

	static bool Build_r2c_get_gift_response(char errorCode, const char* giftMsg);
	static bool NetSend_r2g_query_gifts(DWORD userID, int level);
	static bool NetSend_r2g_withdraw_gift_success(DWORD userID, SGift& gift);
};

#endif // __REGION_GIFT_H__

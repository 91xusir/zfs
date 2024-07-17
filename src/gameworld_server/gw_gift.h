#ifndef _GW_GIFT_H
#define _GW_GIFT_H

enum EGiftType
{
	GT_Items			= 1,
	GT_Money			= 2,
	GT_Exp				= 3,
    GT_Exploit          = 4,
	GT_Count
};

bool GiftType_IsValid(int giftType);


class CGiftMgr
{
public:
	CGiftMgr() {}
	~CGiftMgr() {}

	// 玩家取走礼物的时候，在人物身上做标记，在他第一次存盘时调用RemoveInDB，从而可以保证人物存盘与礼物存盘同步
	// 但是要注意存盘是在另一个线程中执行的，就意味着RemoveInDB是在另一个线程中执行，不能修改变量
	// 查询道具和取道具的时候都要先检查该人物是否已经取了但是还没有存盘的情况
	bool IsGiftWithdrawed(GWUser* pUser, DWORD index);

	void GetAllGifts(DWORD userID, std::vector<SGift>& gifts);
	void GetGifts(DWORD userID, int level, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw);

	bool OnRecvWithdrawSuccess(DWORD userID, SGift& gift);
	bool RemoveInDB(DWORD index);

	bool AddGift(DWORD userID, int level, int type, const char* attrib, DWORD addTime=0, DWORD limitTime=0, const char* msg="");
	bool ClearGift(DWORD userID);

	// 参数的含义依次为：
	// 玩家id， 结果， 取得的礼品， 剩下的可以取得的礼品， 剩下的不可以取得的礼品
	void NetSend_g2r_query_gifts_response(DWORD userID, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw);
};

extern "C" CGiftMgr g_GiftMgr;

CGiftMgr* GetGiftMgr();

#endif//_GW_GIFT_H


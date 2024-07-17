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

	// ���ȡ�������ʱ����������������ǣ�������һ�δ���ʱ����RemoveInDB���Ӷ����Ա�֤����������������ͬ��
	// ����Ҫע�����������һ���߳���ִ�еģ�����ζ��RemoveInDB������һ���߳���ִ�У������޸ı���
	// ��ѯ���ߺ�ȡ���ߵ�ʱ��Ҫ�ȼ��������Ƿ��Ѿ�ȡ�˵��ǻ�û�д��̵����
	bool IsGiftWithdrawed(GWUser* pUser, DWORD index);

	void GetAllGifts(DWORD userID, std::vector<SGift>& gifts);
	void GetGifts(DWORD userID, int level, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw);

	bool OnRecvWithdrawSuccess(DWORD userID, SGift& gift);
	bool RemoveInDB(DWORD index);

	bool AddGift(DWORD userID, int level, int type, const char* attrib, DWORD addTime=0, DWORD limitTime=0, const char* msg="");
	bool ClearGift(DWORD userID);

	// �����ĺ�������Ϊ��
	// ���id�� ����� ȡ�õ���Ʒ�� ʣ�µĿ���ȡ�õ���Ʒ�� ʣ�µĲ�����ȡ�õ���Ʒ
	void NetSend_g2r_query_gifts_response(DWORD userID, std::vector<SGift>& giftsCanWithdraw, std::vector<SGift>& giftsCanNotWithdraw);
};

extern "C" CGiftMgr g_GiftMgr;

CGiftMgr* GetGiftMgr();

#endif//_GW_GIFT_H


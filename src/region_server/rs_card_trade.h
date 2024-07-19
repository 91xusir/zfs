
#ifndef _RS_CARD_TRADE_H_
#define	_RS_CARD_TRADE_H_

#include "business.h"

/*** ����˵�� *************************************************************************
    
    һ, ����
        1, ����ڼ���NPC�����뿨�š�������ۼ� (Region�ж��ۼ۷�Χ)
        2, Region�Ѹ���ҵ�ID�����š�������ۼ۴���Gameworld
        3, Gameworld�Ѹü����������һ�������������, ͬʱ��Login����Ƿ�������
        4, ���Login����������
           5, ��Ӽ�����Ϣ�����ݿ���
           6, ��Ӽ�����Ϣ���б���
           7, �������˵�����ɹ�
        8, ���Login����ʧ�ܻ򷵻س�ʱ������ӵ����ݿ���̳���
           9, �������˵����ʧ��

        * Ҫ��ֹ����������

    ��, ����
        1, ����ڹ���NPC��ѡ��
        2, Region�����ҵĽ�Ǯ��Ϣ����ʱ��ȡ��ҽ�Ǯ����֪ͨGameworld
        3, Gameworld��Login�����ֵ
        4, �����ֵ�ɹ�
           5, ��Gameworld�����������ʺ�������ӽ�Ǯ����Ϣ
           6, ɾ���������ݿ��¼
           7, ɾ�����ۼ�¼
           8, Ȼ��֪ͨRegion��Ǯ
        9, �����ֵʧ�ܣ���ֱ��֪ͨRegion�ѿ۵�Ǯ�黹

        * Ҫ��ֹ����ͬʱ����
        * Ҫ��ֹһ����ι���

    ��, ȡ������

***************************************************************************************/

class RsCardTrade
{
public:
	RsCardTrade();
	~RsCardTrade();

public:
    // �����㿨
    bool Consign(CRegionCreature* pUser, const char* szCardID, long lVerify, long lPrice);

    // ����㿨
    bool BuyCard(CRegionCreature* pUser, long lConsignationID);

    // ȡ������
    bool CancelConsign(CRegionCreature* pUser, long lConsignationID);

    // ��Gameworld����
    void RequestUpdate();

    // �ͻ�������
    void ClientRequestUpdate(CRegionCreature* pUser, short sStart, short sCount);

    void OnConsignResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szCardID, const char* szMsg);
    void OnBuyCardResult(long lUserID, char cSucceed, long lConsignationID, long lPrice, const char* szMsg);
    void OnUpdateTradeList(CG_CmdPacket* pCmd);

    void StartTrade();
    void StopTrade();

private:
    // ���׵��б�
    CCardTradeList                  m_list;
    bool                            m_bCanTrade;

};

#endif//_RS_CARD_TRADE_H_

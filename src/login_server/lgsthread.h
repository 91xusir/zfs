#ifndef __LGS_THREAD_H__
#define __LGS_THREAD_H__

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

//const int MAX_SQL_SIZE = 1024 * 16;
class CLgsThread
{
public:
	CLgsThread();
	virtual ~CLgsThread();

	void SetThreadId(int id);
	void Run();
	static size_t get_pubwin_content(void* ptr, size_t size, size_t nmemb, void* stream);
	//static char		m_pubwin_ret[100];
	S_MEMDEF(m_pubwin_ret, 100)
	std::string urlEncoding( std::string &sIn );
	inline BYTE toHex(const BYTE &x)
	{
		return x > 9 ? x + 55: x + 48;
	}

private:
	int list_cmd_user_login();
	int list_cmd_user_quit();

	//pubwin
	int pubwin_list_cmd_user_login();

    void log_concurrent();
    void log_charge_card();

    long account_create();
    bool account_lock();
    bool account_setpriv();
    bool account_setpassword();
    bool account_delete();

	void ParseCmd(DWORD dwNow);
	bool InitDB();

	void
	_query_member_center(const std::string strUserName, const std::string strPwd, const std::string strIP);
	bool Deduct(short sServer, long lAccountID, long lUserID, short sPoint, long lDeductOrderID);
	void OnDeduct(short sServer,long lAccountID,long lUserID,long lDeductOrderID,char* cResult,short sPoint,short curPoint);
	bool PreDeduct(short sServer, long lAccountID, long lUserID, short sPoint, long lDeductOrderID,int event,long lAuctionId);
	void OnPreDeduct(short sServer,long lAccountID,long lUserID,long lDeductOrderID,char* cResult,short sPoint,short curPoint,char cSucceed,int event,long lAuctionId);

	void AddPoint(short sServer, long lAccountID, long lUserID, short sPoint);
	void AddPointRet(short sServer, long lAccountID, long lUserID, short sPoint,char const* cResult,char cSucceed);

	void ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName,
		long lSalesmanAccountID, const char* szSalesmanAccountName, short sServer,
		long lUserID, const char* szUserName, short sLevel, char cMetier, char cMetierLev,
		char cCardType, const char* szCardID, const char* szCardPwd, long lUserParam, const char* userip);
	void ChargeCardRet(short sServer, long lUserID, char* szCardID, char* msg, long lUserParam,char szSucceed = 0);
	void AddGiftRet(long lSnCard, const char* szUsername,
		short sServer, long lUserID, short sLevel, short sType,
		const char* szGiftString, long tLimitTime, char cFromCardID, char* szCardID);
	void CheckCanTrade(long lAccountID,const char* szAccountName,long lUserID,char cCardType,short sServer,
		const char* szCardID,const char* szCardPwd,long lUserParam,long lTime);
	void CheckCanTradeResult(long lSn, short sServer, long lUserID,	const char* szCardID, char cSucceeded, const char* szMsg, long lCardGroupType = 0);
	//tim.yang  VIP¿¨
	void AddCardTime(long lUserid,long time);
	void AddCardTimeRet(short gwsId,long lUserid,char isRight,std::string time);
	//end
	int			m_tid;
	bool		m_binitdb;
	CDBMySql	m_db;
	CDBMySql	m_logDB;
	CDBMySql	m_memDB;

	SListCmd	m_recvCmd;
	SListCmd	m_sendCmd;
	//char		m_sql[MAX_SQL_SIZE];
	CM_MEMDEF(m_sql, MAX_SQL_SIZE)

	int			m_cmdNum;

	bool       bUnlock;
};

#endif

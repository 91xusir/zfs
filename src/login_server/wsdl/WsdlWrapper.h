#pragma once
#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <queue>
#include "wsdlparser/WsdlInvoker.h"
#include "CBlowFishSimple.h"
#include "core/rt2_core.h"
#include "../dbmysql.h"

using namespace WsdlPull;

const int OP_REGISTER = 0;
const int OP_CHK_MAIL = 1;
const int OP_CHK_NAME = 2;
const int OP_CHK_LOGIN = 3; 
const int OP_EDIT_INFO = 4; 
const int OP_CHK_RSTANSWER =5;
const int OP_CHK_GUID=6; 
const int OP_GET_USERINFO= 7;
const int OP_TEST =8;
const int OP_GET_POINT = 9;
const int OP_EXCHANGE_ITEM  = 10;
const int OP_INTRODUCE = 11;
const int OP_SENDPLURK = 12;

//const string dominName_dianxin = "http://member.yqidea.com/";
//const string dominName_wangtong = "http://member2.yqidea.com/";



struct SJob
{	
	long id;                      //job id
	int opType;
	long pfn;	             //回调函数
	std::map<std::string, std::string> map_in;	 //参数	
	DWORD AddTime;
};

struct SResult
{
	long id;                   //id	== job id
	int opType;
	long pfn;             //回调函数
	std::map<std::string, std::string> map_out;  //返回的结果
	bool isJobFail;
};

//typedef void (*pBrokenJobFunc)(SJob &job);
typedef bool (*pFunc)(bool isJobFail, long id, int opType, std::map<std::string,std::string> &map_out);

class CWsdlWrapper
{
public:
	CWsdlWrapper(void);
	~CWsdlWrapper(void);

	bool _init();
	bool InitDB();
	void Run();		// Thread entry point!
	static bool wsdlCallback(bool isJobFail, long linkID, int opType, std::map<string, string> &map_in);
    
	void addJob_register     (long id, long pfn, const std::map<std::string, std::string> map_in);
	//1.LoginMail
	void addJob_chkMail      (long id, long pfn, const std::map<std::string, std::string> map_in);
	//1.NickName
	void addJob_chkName      (long id, long pfn, const std::map<std::string, std::string> map_in);
	//1.LoginMail 2.LoginPass 3.LoginIP 4.Ver
	void addJob_chkLogin     (long id, long pfn, const std::map<std::string, std::string> map_in);
	void addJob_editInfo     (long id, long pfn, const std::map<std::string, std::string> map_in);
	void addJob_chkRstAnsewer(long id, long pfn, const std::map<std::string, std::string> map_in);
	void addJob_chkGuid      (long id, long pfn, const std::map<std::string, std::string> map_in);
	//1.ServiceID 2.UserID
	void addJob_getUserInfo  (long id, long pfn, const std::map<std::string, std::string> map_in);
	void addJob_test(long id, long pfn, const std::map<std::string, std::string> map_in);
    
	//add by charley
	void addjob_deductpoint     (long id, long pfn, const std::map<std::string,std::string> map_in);
	void addjob_getpoint        (long id, long pfn, const std::map<std::string,std::string> map_in);
	void addjob_openItemEmissary(long id, long pfn, const std::map<std::string,std::string> map_in);

	//jin 080625 介绍人
	void addJob_Introduce(long id, long pfn, const std::map<std::string, std::string> map_in);
	void addJob_SendPlurk(long id, long pfn, const std::map<std::string, std::string> map_in);


	void setUri(int opType, std::string uri);
	void addJob(long id, long pfn,int opType, const std::map<std::string, std::string> map_in);
	bool tick();
	bool initial(const std::string &key, const std::string &serviceID);
	void setInvokerTimeOut(unsigned long time);
	void setEachJobSleepTime(unsigned long time);
	void setNoJobSleepTime(unsigned long time);
	void setJobTimeOut(unsigned long time);
	unsigned long m_wsdl_on_off; //1 开,0 关
	unsigned long m_wsdl_is_pubwin; // 1开 0关
	//char	m_pubwin_address[100];
	//char    m_pubwin_keyaddress[100];
	CM_MEMDEF(m_pubwin_address, 100)
	CM_MEMDEF(m_pubwin_keyaddress, 100)
	//void setBrokenJobHander(long brokenJobHander);
	
private:

	/*added by afeng  2008-07-09,增加可以选择会员认证URL模块*/
	enum ENUM_WIRE_PROVIDER{
		dianxin=1,
		wangtong=2
	} WRIE_PROVIDER;
	
	
	string	m_strDominName;
	bool SetWireProviderDomain();
	RtIni	m_config;		//读取配置文件中网络提供商的类型
	/*added by afeng  2008-07-09,增加可以选择会员认证URL模块*/
	
	bool initWsdl();
	//bool setWsdlPara(int invokerID, const std::vector<std::string> &vector_in);
	bool setWsdlPara(int invokerID, const std::map<std::string, std::string> &map_in);
	bool getWsdlResult(int invokerID, std::map<std::string, std::string> &map_out);

	static void staticWorker(CWsdlWrapper *wsdlWrapper);
	void worker();
	bool doJob(const SJob &job);
	void recordBrokenJob(const SJob &job);


private:
	std::map<int, WsdlPull::WsdlInvoker*> map_invoker;

	RT_TYPE_CS m_jobCS;
	std::queue<SJob> m_jobList;
	RT_TYPE_CS m_resCS;
	std::queue<SResult> m_resultList;
	//std::queue<SJob> m_brokenList;

	unsigned long m_fTimeOut;
	unsigned long m_nEachJobSleepTime;
	unsigned long m_nNoJobSleepTime;
	unsigned long m_nJobTimeOut;
	//long m_lBrokenJobHander;
	CBlowFishSimple *pbf;
	std::string m_strKey;
	std::string m_strServiceID;
	std::map<int, std::string> m_strUri;
	static CDBMySql	m_db;
};

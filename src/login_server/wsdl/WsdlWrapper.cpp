#include "../login.h"
#include "WsdlWrapper.h"

#include <sstream>
#include <list>
#include <time.h>
#include <cctype> // for toupper
#include <algorithm>

CDBMySql CWsdlWrapper::m_db;


CWsdlWrapper::CWsdlWrapper(void)
:m_fTimeOut(0),m_nEachJobSleepTime(1000),m_nNoJobSleepTime(1000),m_nJobTimeOut(30000)
{
	CM_MEMPROTECTOR(m_pubwin_address, 100)
	CM_MEMPROTECTOR(m_pubwin_keyaddress, 100)

	RT_INIT_CS(&m_jobCS);
	RT_INIT_CS(&m_resCS);
	m_wsdl_on_off = 1;
	m_wsdl_is_pubwin = 0;
}

CWsdlWrapper::~CWsdlWrapper(void)
{
	CM_MEMUNPROTECTOR(m_pubwin_address)
	CM_MEMUNPROTECTOR(m_pubwin_keyaddress)

	RT_DELETE_CS(&m_jobCS);
	RT_DELETE_CS(&m_resCS);
	if(pbf != NULL)
		delete pbf;
	std::map<int, WsdlPull::WsdlInvoker*>::iterator it;
	for(it=map_invoker.begin(); it !=map_invoker.end(); ++it)
	{
		delete it->second;        
	}
}

bool CWsdlWrapper::InitDB()
{
	bool ret; 

	// try connect to login db
	ret = m_db.Connect(
		g_cfg.accountDB.ip.c_str(),
		g_cfg.accountDB.port,
		g_cfg.accountDB.username.c_str(),
		g_cfg.accountDB.password.c_str(),
		g_cfg.accountDB.database.c_str()
		);

	if(!ret) 
	{
		ERR1("thread WSDL : connect login db failed\n");
		return false;
	}
	else
	{
		LOG1("thread WSDL : connect login db ok\n");
	}
	return true;
}


void CWsdlWrapper::Run()
{
	worker();
}

bool CWsdlWrapper::_init()
{
	RtIni config;

	if (!config.OpenFile("member_center.ini"))
		return false;
	config.GetEntry("wsdlInfo", "is_pubwin", &m_wsdl_is_pubwin);
	config.GetEntry("wsdlInfo", "pubwinaddress",m_pubwin_address,100);
	config.GetEntry("wsdlInfo", "pubwinaddressKey",m_pubwin_keyaddress,100);
	//是否连接会员中心的开关
	config.GetEntry("wsdlInfo", "on_off", &m_wsdl_on_off);
	if(m_wsdl_on_off == 0) return true;

	char buffer[100];

	unsigned long wsdlInvokerTimeot;
	config.GetEntry("wsdlInfo", "wsdlInvokerTimeot", &wsdlInvokerTimeot);
	setInvokerTimeOut(wsdlInvokerTimeot);

	unsigned long eachJobSleepTime;
	config.GetEntry("wsdlInfo", "eachJobSleepTime", &eachJobSleepTime);
	setEachJobSleepTime(eachJobSleepTime);

	unsigned long noJobSleepTime;
	config.GetEntry("wsdlInfo", "noJobSleepTime", &noJobSleepTime);
	setNoJobSleepTime(noJobSleepTime);

	unsigned long JobTimeOut;
	config.GetEntry("wsdlInfo", "JobTimeOut", &JobTimeOut);
	setJobTimeOut(JobTimeOut);

	config.GetEntry("wsdlInfo", "OP_CHK_LOGIN", buffer, 100);
	setUri(OP_CHK_LOGIN, string(buffer));

	config.GetEntry("wsdlInfo", "OP_GET_USERINFO", buffer, 100);	
	setUri(OP_GET_USERINFO, string(buffer));

	//add by charley
	config.GetEntry("wsdlInfo", "OP_GET_POINT", buffer, 100);	
	setUri(OP_GET_POINT, string(buffer));

	config.GetEntry("wsdlInfo", "OP_EXCHANGE_ITEM", buffer, 100);
	setUri(OP_EXCHANGE_ITEM, string(buffer));

	config.GetEntry("wsdlInfo", "OP_INTRODUCE", buffer, 100);
	setUri(OP_INTRODUCE, string(buffer));

	config.GetEntry("wsdlInfo", "OP_SENDPLURK", buffer, 100);
	setUri(OP_SENDPLURK, string(buffer));

	char buffer2[100];
	config.GetEntry("wsdlInfo", "key", buffer, 100);
	config.GetEntry("wsdlInfo", "serviceID", buffer2, 100);

	//int nWireProvider;
	//config.GetEntry("WireProvider","Provider",buffer,100);

	if (!initial(string(buffer), string(buffer2)))  //add 20080403
	{
		RtCoreLog().Error("Initial wsdl server error\n");
		return false;
	}

	return InitDB();
}

bool CWsdlWrapper::wsdlCallback(bool isJobFail, long linkID, int opType, std::map<string, string> &map_in)
{
	if (isJobFail)
		return false;

	std::map<string, string> map_temp;
	char		sql[MAX_SQL_SIZE];

	if (opType == OP_CHK_LOGIN)
	{
		std::string retValue = map_in["ChkUserLoginResult"];
		long nReturnValue = atol(retValue.c_str());
		RtCoreLog().Info("On ChkLogin get user id: %d\n", nReturnValue);
		if (nReturnValue > 0)
		{
			map_temp.insert(make_pair("UserID", retValue));
			g_pWSDL->addJob_getUserInfo(nReturnValue, 0, map_temp);
		}
	}

	if (opType == OP_GET_USERINFO)
	{
		long account_id = linkID;
		RtCoreLog().Info("On GetUserInfo get user id: %d\n", linkID);
		std::string strAccountName = map_in["LoginMail"];
		std::string strPWD = map_in["LoginPass"];

		std::string strRealPwd = strPWD.substr(8, 16);
		std::transform(strRealPwd.begin(), strRealPwd.end(), strRealPwd.begin(), (int (*)(int))std::tolower);

		sprintf(sql,"select `idx` from `user` where `idx`='%d'", account_id);
		if (!m_db.Query(sql)) return false;

		if (m_db.GetRowCount() > 0)
		{
			sprintf(sql,
				"UPDATE `user` SET `username`='%s',`password`='%s' WHERE `idx` = %d;",
				m_db.EscapeString(strAccountName.c_str()).c_str(),
				m_db.EscapeString(strRealPwd.c_str()).c_str(),
				account_id
				);
			if (!m_db.Query(sql)) return false;
		}
		else
		{
			sprintf(sql,
				"INSERT INTO `user`(`idx`, `username`,`password`,`user_priv`,`locked`,`lock_begin`,`lock_end`,`create_date`,`from`,`datetime`) "
				"VALUES(%d, '%s','%s',%d,%d,FROM_UNIXTIME(%u),FROM_UNIXTIME(%u),NOW(),7,DATE_ADD(NOW(), INTERVAL 7 DAY))",
				account_id,
				m_db.EscapeString(strAccountName.c_str()).c_str(),
				m_db.EscapeString(strRealPwd.c_str()).c_str(),
				1,0,0,0);
			if (!m_db.Query(sql)) 
				return false;
		}
	}

	return true;	
}

void CWsdlWrapper::setInvokerTimeOut(unsigned long time)
{
	m_fTimeOut = time;
}
void CWsdlWrapper::setEachJobSleepTime(unsigned long time)
{
	m_nEachJobSleepTime =time;
}
void CWsdlWrapper::setNoJobSleepTime(unsigned long time)
{
	m_nNoJobSleepTime = time;
}

void CWsdlWrapper::setJobTimeOut(unsigned long time)
{
    m_nJobTimeOut = time;
}
bool CWsdlWrapper::setWsdlPara(int optype, const std::map<std::string,std::string> &map_in)
{	
	map_invoker[optype]->SetStatus();

	for(std::map<std::string,std::string>::const_iterator it = map_in.begin(); it != map_in.end(); ++it)
	{
		if (!map_invoker[optype]->setValue(it->first, it->second))
		{
			RtCoreLog().Info("WSDL:incorrect input value\n");
			return false;
		}   
	}
	return true;
}
/*
bool CWsdlWrapper::setWsdlPara(int invokeID, const std::vector<std::string> &vector_in)
{
	if (!vector_invoker[invokeID].status())
	{
		return false;
	}
	long id =0,min,max;
	Schema::Type type;
	std::string param;	
	std::vector<std::string> parents;

	std::vector<std::string>::const_iterator it;
	for(it = vector_in.begin(); it != vector_in.end(); ++it)
	{
		id = vector_invoker[invokeID].getNextInput(param,type,min,max,parents);
		if(id == -1)
		{
			break;
		}
		if (!vector_invoker[invokeID].setInputValue(id, *it))
		{
			RtCoreLog().Info("Incorrect input value\n");
			return false;
		}		
	}
	return true;

}
*/

void CWsdlWrapper::addJob_register(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_REGISTER;
	addJob(id, pfn, opType, map_in);
}
//1.LoginMail
void CWsdlWrapper::addJob_chkMail(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_CHK_MAIL;

	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));
		
	if(it != map_in.end())
	{
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}
	addJob(id, pfn, opType, map_temp);
}
////1.NickName
void CWsdlWrapper::addJob_chkName(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_CHK_NAME;

	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));
		
	if(it != map_in.end())
	{
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}
	addJob(id, pfn, opType, map_temp);
}
//1.LoginMail 2.LoginPass 3.LoginIP 4.Ver
void CWsdlWrapper::addJob_chkLogin(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_CHK_LOGIN;

	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));
		
	for(std::map<std::string, std::string>::const_iterator it=map_in.begin(); it!=map_in.end(); ++it)
	{
		if(it->first == "LoginMail")
		{			
			map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
		}
		else if(it->first == "LoginPass")
		{
			map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
		}
		else if(it->first == "LoginIP")
		{
			map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
		}
		else if(it->first == "Ver")
		{
			map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
		}
	}
	
	addJob(id, pfn, opType, map_temp);
}
void CWsdlWrapper::addJob_editInfo(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_EDIT_INFO;
	addJob(id, pfn, opType, map_in);
}
void CWsdlWrapper::addJob_chkRstAnsewer(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_CHK_RSTANSWER;
	addJob(id, pfn, opType, map_in);
}
void CWsdlWrapper::addJob_chkGuid(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_CHK_GUID;
	addJob(id, pfn, opType, map_in);
}

//map_in输入参数1UserID
void CWsdlWrapper::addJob_getUserInfo(long id, long pfn, const std::map<std::string, std::string> map_in)
{	
	int opType = OP_GET_USERINFO;

	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));
		
	if(it != map_in.end())
	{		
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}	
	addJob(id, pfn, opType, map_temp);
}
void CWsdlWrapper::addJob_test(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_TEST;
	addJob(id, pfn, opType, map_in);
}

void CWsdlWrapper::addJob_SendPlurk(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_SENDPLURK;
	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));

	for(std::map<std::string, std::string>::const_iterator it=map_in.begin(); it!=map_in.end(); ++it)
	{			
		map_temp.insert(make_pair(it->first, it->second));
	}
	addJob(id, pfn, opType, map_temp);
}

//jin 080625 介绍人
void CWsdlWrapper::addJob_Introduce(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_INTRODUCE;
	std::map<std::string, std::string> map_temp;
	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));

	for(std::map<std::string, std::string>::const_iterator it=map_in.begin(); it!=map_in.end(); ++it)
	{			
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}
	addJob(id, pfn, opType, map_temp);
}


void CWsdlWrapper::addjob_deductpoint(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_GET_POINT;

	std::map<std::string, std::string> map_temp;

	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));


	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	for(; it!=map_in.end(); ++it)
	{		
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}	
	addJob(id, pfn, opType, map_temp);

}


void CWsdlWrapper::addjob_getpoint(long id, long pfn, const std::map<std::string, std::string> map_in)
{
	int opType = OP_GET_POINT;

	std::map<std::string, std::string> map_temp;
	
	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));
    

	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	for(; it!=map_in.end(); ++it)
	{		
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}	
	addJob(id, pfn, opType, map_temp);

}

void CWsdlWrapper::addjob_openItemEmissary(long id, long pfn, const std::map<std::string,std::string> map_in)
{
	int opType = OP_EXCHANGE_ITEM;

	std::map<std::string, std::string> map_temp;

	map_temp.insert(make_pair("ServiceID", m_strServiceID));
	map_temp.insert(make_pair("EncrServiceID", pbf->Encrypt(m_strServiceID)));


	std::map<std::string, std::string>::const_iterator it = map_in.begin();
	for(; it!=map_in.end(); ++it)
	{		
		map_temp.insert(make_pair(it->first, pbf->Encrypt(it->second)));
	}	
	addJob(id, pfn, opType, map_temp);
}

void CWsdlWrapper::addJob(long id, long pfn, int opType, const std::map<std::string, std::string> map_in)
{
	if(m_wsdl_on_off == 0)	return;
	if (pfn == 0)
		pfn = (long)&wsdlCallback;

	SJob job;
	job.id = id;
	job.pfn = pfn;
	job.opType = opType;
	job.map_in = map_in;
	job.AddTime = time(NULL);

//	boost::mutex::scoped_lock lock(m_jobMutex, false);
	RT_LOCK_CS(&m_jobCS);

	m_jobList.push(job);

	RT_UNLOCK_CS(&m_jobCS);
}

bool CWsdlWrapper::getWsdlResult(int optype, std::map<std::string, std::string> &map_out)
{
	if(!map_invoker[optype]->invoke(m_fTimeOut))   //timeout
	{
		RtCoreLog().Info("WSDL:invoke web service fail\n ");
		return false;
	}

	std::string name;
	
	TypeContainer* tc = 0;
	std::ostringstream oss;
	std::string temp, first, second;
	int npos;
	//jin 080625 OP_INTRODUCE没有返回值
	if (optype == OP_INTRODUCE)
		return true;
	

	while (map_invoker[optype]->getNextOutput(name,tc))
	{
		tc->print(oss);		
		temp = oss.str();	
		if(temp.empty())
		{
			RtCoreLog().Info("WSDL:getWsdlResult 取的值为空");
			return false;
		}
		npos = temp.find(':');
		first = temp.substr(0, npos);		
		second = temp.substr(npos+1, temp.size() - npos);

		//second包含的是个string数组,获取用户信息时
		int pos1,pos2;		
		if((pos1 = second.find(':')) != std::string::npos)
		{
			std::vector<std::string> vector_str;		
			vector_str.reserve(8);
			vector_str.push_back("LoginMail");
			vector_str.push_back("LoginPass");
			vector_str.push_back("icard");
			vector_str.push_back("Gender");
			vector_str.push_back("UserName");
			vector_str.push_back("Mobile");
			vector_str.push_back("RegDate");			
			vector_str.push_back("enthrallment");
			vector_str.push_back("Ver");

			temp = second;
			std::vector<std::string>::iterator it = vector_str.begin();

			while((pos2 = temp.find(':', pos1+1)) != std::string::npos)
			{	
				second = temp.substr(pos1+1, pos2-pos1-7);
				map_out.insert(make_pair(*it, pbf->Decrypt(second)));
				//map_out.insert(make_pair(*it, second));
				++it;
				if (it == vector_str.end())
				{
					break;
				}				
				pos1 = pos2;
			}
			if (it != vector_str.end())
			{
				second = temp.substr(pos1+1, temp.size()-pos1);
				map_out.insert(make_pair(*it, pbf->Decrypt(second)));
			}
			//map_out.insert(make_pair(*it, second));

		}
		else
		{
			//map_out.insert(make_pair(first, pbf->Decrypt(second)));
			    std::string temp;
				if(second.size()<7)  //小于8位为明文
				{
				//	RtCoreLog().Info("WSDL:可能密钥错误\n");
					temp = second;					
				}
				else
				{
					temp = pbf->Decrypt(second);
				}
				
				map_out.insert(make_pair(first, temp));
			
		}		
				
				
	}
	return true;	
}

void CWsdlWrapper::setUri(int opType, std::string uri)
{
	m_strUri.insert(make_pair(opType, uri));
}

bool CWsdlWrapper::initWsdl()
{		
	std::map<int, std::string> map_opt;	
	/*
	const int OP_REGISTER = 0;
	const int OP_CHK_MAIL = 1;
	const int OP_CHK_NAME = 2;
	const int OP_CHK_LOGIN = 3; 
	const int OP_EDIT_INFO = 4; 
	const int OP_CHK_RSTANSWER =5;
	const int OP_CHK_GUID=6; 
	const int OP_GET_USERINFO= 7;
	const int OP_TEST =8;
	*/
	map_opt.insert(make_pair(OP_REGISTER, "UserRegister"));
	map_opt.insert(make_pair(OP_CHK_MAIL, "ChkLoginMail"));
	map_opt.insert(make_pair(OP_CHK_NAME, "ChkNickName"));
	map_opt.insert(make_pair(OP_CHK_LOGIN, "ChkUserLogin"));
	map_opt.insert(make_pair(OP_EDIT_INFO, "EditUserInfo"));
	map_opt.insert(make_pair(OP_CHK_RSTANSWER, "ChkRestAnswer"));
	map_opt.insert(make_pair(OP_CHK_GUID, "ChkRandom"));
	map_opt.insert(make_pair(OP_GET_USERINFO, "UserInfoList"));
	map_opt.insert(make_pair(OP_GET_POINT,"UpdateUserPoint"));//add by charley
	map_opt.insert(make_pair(OP_EXCHANGE_ITEM,"GetItem"));     //add by charley
	map_opt.insert(make_pair(OP_TEST, "DesTest"));
	map_opt.insert(make_pair(OP_INTRODUCE, "UserIntroduce"));
	map_opt.insert(make_pair(OP_SENDPLURK, "Plurk")); //add by tooth.shi

	
	try
	{
		if(!SetWireProviderDomain())	//add by afeng.wu,2007-07-09
		{
			return false;
		}

		for(std::map<int,string>::iterator it=m_strUri.begin(); it!=m_strUri.end(); ++it)
		{
			WsdlPull::WsdlInvoker* pInvoker = new WsdlPull::WsdlInvoker;

			/*add by afeng.wu, 2007-07-09*/
			//string strDominName;
			//
			//if( m_nWireProvider == dianxin)

			//	strUri = dominName_dianxin + it->second;
			//	
			//else if(m_nWireProvider == wangtong)
			//	strUri = dominName_wangtong + it->second;
	
			string dominName = m_strDominName;

			string strUri = dominName + it->second;
			if (it->first == OP_SENDPLURK)
			{
				strUri = it->second;
			}
			
			if(!pInvoker->setWSDLUri(strUri)) 
			{
				RtCoreLog().Info("WSDL:set uri:%s fail\n", strUri.c_str());
				return false;
			}
			if(!pInvoker->setOperation(map_opt[it->first]))
			{	
				RtCoreLog().Info("WSDL:Unknow operation name operation is:%s\n", map_opt[it->first].c_str());
				return false;
			}	
			map_invoker.insert(make_pair(it->first, pInvoker));
		}
	}
	catch (WsdlException we)
    {	
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", we.line, we.col, we.description.c_str());
		return false;   
    }
	catch (SchemaParserException spe)
    {
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", spe.line, spe.col, spe.description.c_str());
		return false;
    }
	catch (XmlPullParserException xpe)
    {
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", xpe.line, xpe.col, xpe.description.c_str());
		return false;
    }

	return true;		

}
bool CWsdlWrapper::initial(const std::string &key, const std::string &serviceID)
{

	if(!initWsdl())
	{
		return false;
	}
	m_strKey = key;
	m_strServiceID = serviceID;

	pbf = new CBlowFishSimple(m_strKey);
// 	m_pThread = new boost::thread(
// 		boost::bind(&CWsdlWrapper::staticWorker, this) );
	RtCoreLog().Info("WSDL:wsdl thread is running now!\n");
	return true;
	//m_pThread->join();
}

void CWsdlWrapper::staticWorker(CWsdlWrapper *wsdlWrapper)
{
	if(wsdlWrapper)
		wsdlWrapper->worker();
}
void CWsdlWrapper::worker()
{
//	boost::mutex::scoped_lock lock(m_jobMutex, false);
	while(1)
	{
		if(!m_jobList.empty())
		{

// 			if(!lock.locked())
// 				lock.lock();

			RT_LOCK_CS(&m_jobCS);

			DWORD TimeNow = time(NULL);
			SJob job = m_jobList.front();
			m_jobList.pop();
			RT_UNLOCK_CS(&m_jobCS);			

			if(TimeNow - job.AddTime > m_nJobTimeOut/1000)
			{
				recordBrokenJob(job);
				continue;
			}



// 			if(lock.locked())
// 				lock.unlock();
			if(!doJob(job))
			{
				recordBrokenJob(job);				
			}
			else
			{
				RtCoreLog().Info("WSDL:do job succ, link id:%d,optype:%d\n", job.id, job.opType);
			}
			rtSleep(m_nEachJobSleepTime);
		}
		else
		{
			rtSleep(m_nNoJobSleepTime);
		}
	}
	
}

bool CWsdlWrapper::doJob(const SJob &job)
{
	std::string uri, opt;
	std::map<std::string, std::string> map_out;		
	try
	{
		if(!setWsdlPara(job.opType, job.map_in))
		{
			return false;
		}
		if(!getWsdlResult(job.opType, map_out))
		{
			return false;
		}
		//copy(job.map_in.begin(), job.map_in.end(), insert_iterator<std::string, std::string>(map_out));
		//加入
		if(job.opType != OP_SENDPLURK)
		{
			std::map<std::string, std::string>::const_iterator it = job.map_in.begin();
			for (; it != job.map_in.end(); ++it)
			{
				if (it->first == "ServiceID" || it->first == "EncrServiceID")
					continue;
				map_out.insert( make_pair(it->first, pbf->Decrypt(it->second)) );			
			}

		}
		else
		{
			std::map<std::string, std::string>::const_iterator it = job.map_in.begin();
			for (; it != job.map_in.end(); ++it)
			{
				if (it->first == "ServiceID" || it->first == "EncrServiceID")
					continue;
				map_out.insert( make_pair(it->first, it->second) );			
			}

		}
		
		SResult result;
		result.id = job.id;		
		result.pfn = job.pfn;
		result.opType = job.opType;
		result.map_out = map_out;
		result.isJobFail = false;
//		boost::mutex::scoped_lock lock(m_resultMutex, false);
// 		if(!lock.locked())
// 			lock.lock();
		RT_LOCK_CS(&m_resCS);
		m_resultList.push(result);
		RT_UNLOCK_CS(&m_resCS);
// 
// 		if(lock.locked())
// 			lock.unlock();		

	}
	catch (WsdlException we)
    {	
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", we.line, we.col, we.description.c_str());
		return false;   
    }
	catch (SchemaParserException spe)
    {
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", spe.line, spe.col, spe.description.c_str());
		return false;
    }
	catch (XmlPullParserException xpe)
    {
		RtCoreLog().Error("WSDL:An Exception occurred at line:%d,col:%d\n%s\n", xpe.line, xpe.col, xpe.description.c_str());
		return false;
    }
	return true;

}

void CWsdlWrapper::recordBrokenJob(const SJob &job)
{
	SResult result;
	result.id = job.id;		
	result.pfn = job.pfn;
	result.opType = result.opType;
	result.map_out = job.map_in;
	result.isJobFail = true;
// 	boost::mutex::scoped_lock lock(m_resultMutex, false);
// 	if(!lock.locked())
// 		lock.lock();
	RT_LOCK_CS(&m_resCS);
	m_resultList.push(result);
	RT_UNLOCK_CS(&m_resCS);

// 	if(lock.locked())
// 		lock.unlock();	
	RtCoreLog().Info("WSDL:A broken job, client link id:%d,optype:%d\n", job.id, job.opType);
}

bool CWsdlWrapper::tick()
{
	while(!m_resultList.empty())
	{
// 		boost::mutex::scoped_lock lock(m_resultMutex, false);
// 		if(!lock.locked())
// 			lock.lock();
		RT_LOCK_CS(&m_resCS);
		SResult res = m_resultList.front();
        m_resultList.pop();
		RT_UNLOCK_CS(&m_resCS);
// 		if(lock.locked())
// 			lock.unlock();
		long pfn = res.pfn;
		(*((pFunc)pfn))(res.isJobFail,res.id, res.opType, res.map_out);		
	}

	/*
	if(m_lBrokenJobHander != 0)
	{
		while(!m_brokenList.empty())
		{
		
			boost::mutex::scoped_lock lock(m_brokenMutex, false);
			if(!lock.locked())
				lock.lock();
			SJob job = m_brokenList.front();
			m_brokenList.pop();
			if(lock.locked())
				lock.unlock();			
			(*((pBrokenJobFunc)m_lBrokenJobHander))(job);
			
		}
	}
	*/
	return true;
}

/*
void CWsdlWrapper::setBrokenJobHander(long brokenJobHander)
{
	m_lBrokenJobHander = brokenJobHander;
}
*/
/****************************************
author	:	afeng.wu
date	:	2007-07-09
using	:	设置网络服务商类型
input	:	
output	:	
global	:	m_nWireProvider		服务商类型
*****************************************/
bool CWsdlWrapper::SetWireProviderDomain()
{
	char const *strDomainFormat="http://%s.yqidea.com/";
	char szTmpBuf[100];
	RtIni config;
	if (!config.OpenFile("member_center.ini"))
	{
		//log.Error("Open srv_login.ini config file error\n");
		return false;
	}
	char buffer[100];
	config.GetEntry("wireprovider","provider",buffer,100);
	
	sprintf(szTmpBuf, strDomainFormat, buffer);
	m_strDominName=szTmpBuf;

	return true;

}

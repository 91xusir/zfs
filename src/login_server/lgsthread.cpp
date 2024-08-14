
#include "login.h"
#include <cctype>

G_MEMDEF(CLgsThread::m_pubwin_ret, 100)

CLgsThread::CLgsThread(void) {
    S_MEMPROTECTOR(m_pubwin_ret, 100, bMP)
    CM_MEMPROTECTOR(m_sql, MAX_SQL_SIZE)

    m_cmdNum  = 0;
    m_binitdb = false;
    bUnlock   = false;
}

CLgsThread::~CLgsThread(void) {
    CM_MEMUNPROTECTOR(m_sql)
}

void CLgsThread::SetThreadId(int id) {
    m_tid = id;
}

void CLgsThread::Run() {
    bool  bGetCmd;
    DWORD startTime, now, lastCmdTime = rtGetMilliseconds();
    InitDB();
    while (1) {
        while (!m_db.IsActive()) {
            rtSleep(5000);
            InitDB();
        }

        startTime = rtGetMilliseconds();
        bGetCmd   = g_recvCmdList.GetCmd(m_recvCmd);
        now       = rtGetMilliseconds();
        if (now - startTime > 1000) {
            ERR2("thread %d: frame time = %d [g_recvCmdList.GetCmd]\n", m_tid, now - startTime);
        }

        if (bGetCmd) {
            ParseCmd(now);
            m_cmdNum++;
            lastCmdTime = now;
        } else {
            rtSleep(100);
            if (now - lastCmdTime > 50 * 1000) {
                lastCmdTime = now;
                // m_db.Query("select 0");
            }
        }
    }
}

bool CLgsThread::InitDB() {
    bool ret;

    // try connect to login db
    ret = m_db.Connect(g_cfg.accountDB.ip.c_str(), g_cfg.accountDB.port,
                       g_cfg.accountDB.username.c_str(), g_cfg.accountDB.password.c_str(),
                       g_cfg.accountDB.database.c_str());

    if (!ret) {
        ERR1("thread %d : connect login db failed\n", m_tid);
        m_binitdb = false;
        return false;
    } else {
        LOG1("thread %d : connect login db ok\n", m_tid);
        m_binitdb = true;
    }
    return true;
}

void CLgsThread::ParseCmd(DWORD dwNow) {
    long  ret;
    DWORD nowSql, nowCmd;
    m_sendCmd.pStub = m_recvCmd.pStub;
    switch (m_recvCmd.cmdId) {
        case LIST_CMD_USER_LOGIN:
            m_sendCmd.cmdId                       = LIST_CMD_USER_LOGIN_RET;
            m_sendCmd.userloginRet.dwLoginOrderID = m_recvCmd.userLogin.dwLoginOrderID;
            m_sendCmd.userloginRet.t.iStep        = m_recvCmd.userLogin.t.iStep;
            m_sendCmd.userloginRet.lSeed          = m_recvCmd.userLogin.lSeed;
            m_sendCmd.userLogin.bPowerLogin       = m_recvCmd.userLogin.bPowerLogin;  //PZH
            if (g_pWSDL->m_wsdl_is_pubwin == 1)
                ret = pubwin_list_cmd_user_login();
            else
                ret = list_cmd_user_login();

            nowSql = rtGetMilliseconds();
            if (ret == RET_ADD_CMD) {
                if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                    ERR("ERROR: Add Login ret LIST_CMD_USER_LOGIN failed 1\n");
                }
            } else {
                m_sendCmd.userloginRet.lSeed      = m_recvCmd.userLogin.lSeed;
                m_sendCmd.accountInfo.accountName = m_recvCmd.userLogin.szUsername;
                m_sendCmd.accountInfo.password    = m_recvCmd.userLogin.szPassword;
                m_sendCmd.accountInfo.ip          = m_recvCmd.userLogin.szUserIP;
                if (ret == RET_GET_FIELD_ERROR) {
                    m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_UNKNOWN;
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_DatabaseError;
                } else if (ret == RET_GET_ROW_ERROR) {
                    m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_NOT_FOUND;
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_NotFound;
                } else if (ret == RET_PUBWIN_ERROR) {
                    m_sendCmd.userloginRet.lRet =
                        LOGIN_RET_FAILED_PUBWIN_CENTER;  //  [8/25/2009 tim.yang] 同步pubwin失败
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_AccountPaused;
                } else if (ret == LOGIN_RET_FAILED_ACCOUNT_PAUSE)  //PZH
                {
                    m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_ACCOUNT_PAUSE;  //PZH
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_FAILED_ACCOUNT_PAUSE;  //PZH
                } else if (ret == LOGIN_RET_CODE_PasswordWrong) {
                    m_sendCmd.userloginRet.lRet     = LOGIN_RET_CODE_PasswordWrong;
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_PasswordWrong;
                } else {
                    m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_NOT_FOUND;
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_NotFound;
                }
                if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                    ERR("ERROR: Add Login ret LIST_CMD_USER_LOGIN failed 2\n");
                }
            }
            break;

        case LIST_CMD_USER_QUIT:
            m_sendCmd.logoutRet.dwLogoutOrderID = list_cmd_user_quit();
            nowSql                              = rtGetMilliseconds();
            m_sendCmd.cmdId                     = LIST_CMD_LOGOUT_RET;
            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_USER_QUIT failed\n");
            }
            break;

        case LIST_CMD_LOG_CONCURRENT:
            log_concurrent();
            nowSql = rtGetMilliseconds();
            break;

        case LIST_CMD_CHARGE_CARD_LOG:
            log_charge_card();
            nowSql = rtGetMilliseconds();
            break;

        case LIST_CMD_ACCOUNT_CREATE:
            m_sendCmd.accountCreateRet.lAccountID = account_create();
            nowSql                                = rtGetMilliseconds();

            m_sendCmd.accountCreateRet.lLoginOrderID = m_recvCmd.accountCreate.lLoginOrderID;
            m_sendCmd.cmdId                          = LIST_CMD_ACCOUNT_CREATE_RET;
            if (m_sendCmd.accountCreateRet.lAccountID == -1) {
                m_sendCmd.accountCreateRet.bSucceed = false;
            } else {
                m_sendCmd.accountCreateRet.bSucceed = true;
            }
            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_ACCOUNT_CREATE failed\n");
            }
            break;

        case LIST_CMD_ACCOUNT_LOCK:
            m_sendCmd.cmdId                      = LIST_CMD_RET;
            m_sendCmd.cmdRet.bSucceed            = account_lock();
            m_sendCmd.cmdRet.bShowMessage        = true;
            m_sendCmd.cmdRet.lShowMessageAccount = m_recvCmd.accountInfo.accountId;
            nowSql                               = rtGetMilliseconds();

            strcpy(m_sendCmd.cmdRet.szInfo, m_recvCmd.accountLock.sLocked ? "Lock" : "Unlock");
            strcat(m_sendCmd.cmdRet.szInfo, " account [");
            if (!m_recvCmd.accountLock.bUseUsername)
                sprintf(m_recvCmd.accountLock.szUsername, "ID:%d",
                        m_recvCmd.accountLock.lAccountID);
            strcat(m_sendCmd.cmdRet.szInfo, m_recvCmd.accountLock.szUsername);
            if (m_sendCmd.cmdRet.bSucceed) {
                strcat(m_sendCmd.cmdRet.szInfo, "] succeeded");
            } else {
                strcat(m_sendCmd.cmdRet.szInfo, "] failed");
            }
            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_ACCOUNT_LOCK failed\n");
            }
            break;

        case LIST_CMD_ACCOUNT_SETPRIV:
            m_sendCmd.cmdId                      = LIST_CMD_RET;
            m_sendCmd.cmdRet.bSucceed            = account_setpriv();
            m_sendCmd.cmdRet.bShowMessage        = true;
            m_sendCmd.cmdRet.lShowMessageAccount = m_recvCmd.accountInfo.accountId;
            nowSql                               = rtGetMilliseconds();

            strcpy(m_sendCmd.cmdRet.szInfo, "Set [");
            if (!m_recvCmd.accountSetPriv.bUseUsername)
                sprintf(m_recvCmd.accountSetPriv.szUsername, "ID:%d",
                        m_recvCmd.accountSetPriv.lAccountID);
            strcat(m_sendCmd.cmdRet.szInfo, m_recvCmd.accountSetPriv.szUsername);
            if (m_sendCmd.cmdRet.bSucceed) {
                strcat(m_sendCmd.cmdRet.szInfo, "] privileges succeeded");
            } else {
                strcat(m_sendCmd.cmdRet.szInfo, "] privileges failed");
            }
            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_ACCOUNT_SETPRIV failed\n");
            }
            break;

        case LIST_CMD_ACCOUNT_SETPWD: {
            bool bSucceed = account_setpassword();
            if (m_recvCmd.accountSetPwd
                    .lLoginOrderID)  // 如果是计费发回来的修改密码，就按照创建帐号处理
            {
                m_sendCmd.cmdId                          = LIST_CMD_ACCOUNT_CREATE_RET;
                m_sendCmd.accountCreateRet.lLoginOrderID = m_recvCmd.accountSetPwd.lLoginOrderID;
                m_sendCmd.accountCreateRet.lAccountID    = m_recvCmd.accountSetPwd.lAccountID;
                m_sendCmd.accountCreateRet.bSucceed      = bSucceed;
            } else {
                m_sendCmd.cmdId                      = LIST_CMD_RET;
                m_sendCmd.cmdRet.bSucceed            = bSucceed;
                m_sendCmd.cmdRet.bShowMessage        = true;
                m_sendCmd.cmdRet.lShowMessageAccount = m_recvCmd.accountInfo.accountId;
                strcpy(m_sendCmd.cmdRet.szInfo, "Set [");
                if (!m_recvCmd.accountSetPwd.bUseUsername)
                    sprintf(m_recvCmd.accountSetPwd.szUsername, "ID:%d",
                            m_recvCmd.accountSetPwd.lAccountID);
                strcat(m_sendCmd.cmdRet.szInfo, m_recvCmd.accountSetPwd.szUsername);
                if (m_sendCmd.cmdRet.bSucceed) {
                    strcat(m_sendCmd.cmdRet.szInfo, "] password succeeded");
                } else {
                    strcat(m_sendCmd.cmdRet.szInfo, "] password failed");
                }
            }
            nowSql = rtGetMilliseconds();

            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_ACCOUNT_SETPWD failed\n");
            }
        } break;

        case LIST_CMD_ACCOUNT_DELETE:
            m_sendCmd.cmdId                      = LIST_CMD_RET;
            m_sendCmd.cmdRet.bSucceed            = account_delete();
            m_sendCmd.cmdRet.bShowMessage        = true;
            m_sendCmd.cmdRet.lShowMessageAccount = m_recvCmd.accountInfo.accountId;
            nowSql                               = rtGetMilliseconds();

            strcpy(m_sendCmd.cmdRet.szInfo, "Delete account [");
            if (!m_recvCmd.accountDelete.bUseUsername)
                sprintf(m_recvCmd.accountDelete.szUsername, "ID:%d",
                        m_recvCmd.accountDelete.lAccountID);
            strcat(m_sendCmd.cmdRet.szInfo, m_recvCmd.accountDelete.szUsername);
            if (m_sendCmd.cmdRet.bSucceed) {
                strcat(m_sendCmd.cmdRet.szInfo, "] succeeded");
            } else {
                strcat(m_sendCmd.cmdRet.szInfo, "] failed");
            }
            if (!g_sendCmdList.AddCmd(m_sendCmd)) {
                ERR("ERROR: Add Login ret LIST_CMD_ACCOUNT_DELETE failed\n");
            }
            break;
        case LIST_CMD_USER_POINT: {
            Deduct(m_recvCmd.chargePoint.sServer, m_recvCmd.chargePoint.lAccountID,
                   m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.sPoint,
                   m_recvCmd.chargePoint.lDeductOrderID);

            //	g_lgs->Deduct((short)m_recvCmd.chargePoint.sServer, m_recvCmd.chargePoint.lAccountID, m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.sPoint, m_recvCmd.chargePoint.lDeductOrderID);
            nowSql = rtGetMilliseconds();
        } break;
        case LIST_CMD_PRE_USER_POINT: {
            PreDeduct(m_recvCmd.chargePoint.sServer, m_recvCmd.chargePoint.lAccountID,
                      m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.sPoint,
                      m_recvCmd.chargePoint.lDeductOrderID, m_recvCmd.chargePoint.event,
                      m_recvCmd.chargePoint.lAuctionId);
            nowSql = rtGetMilliseconds();
        }
        case LIST_CMD_CHARGE_CARD: {
            ChargeCard(m_recvCmd.chargePoint.lBuyerAccountID,
                       m_recvCmd.chargePoint.szBuyerAccountName, m_recvCmd.chargePoint.lAccountID,
                       m_recvCmd.chargePoint.szAccountName, m_recvCmd.chargePoint.sServer,
                       m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.szUserName,
                       m_recvCmd.chargePoint.sUserLev, m_recvCmd.chargePoint.cMetier,
                       m_recvCmd.chargePoint.cMetierLev, m_recvCmd.chargePoint.cCardType,
                       m_recvCmd.chargePoint.szCardID, m_recvCmd.chargePoint.szCardPassWord,
                       m_recvCmd.chargePoint.param, m_recvCmd.chargePoint.cUserIp);
            nowSql = rtGetMilliseconds();
        } break;
        case LIST_CMD_ADD_ACCOUNT_TIME: {
            AddCardTime(m_recvCmd.accountInfo.accountId, m_recvCmd.VIPRegionTime);
            nowSql = rtGetMilliseconds();
        } break;
        case LIST_CMD_CARD_TRADE: {
            CheckCanTrade(m_recvCmd.chargePoint.lAccountID, m_recvCmd.chargePoint.szAccountName,
                          m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.cCardType,
                          m_recvCmd.chargePoint.sServer, m_recvCmd.chargePoint.szCardID,
                          m_recvCmd.chargePoint.szCardPassWord, m_recvCmd.chargePoint.param,
                          m_recvCmd.chargePoint.tTime);
            nowSql = rtGetMilliseconds();
        } break;
        case LIST_CMD_CARD_TRADE_FAILD: {
            sprintf(m_sql,
                    "update `card_number` "
                    "set `cardtrade` = 0 "
                    "where `cardidMD5` = '%s' ",
                    m_db.EscapeString(m_recvCmd.chargePoint.szCardID).c_str());
            nowSql = rtGetMilliseconds();
            if (!m_db.Query(m_sql)) {
                ERR2("card trade failed,set `cardtrade`= 0, where cardidMD5 = [%s]",
                     m_recvCmd.chargePoint.szCardID);
            }
        } break;
        case LIST_CMD_ADD_POINT: {
            AddPoint(m_recvCmd.chargePoint.sServer, m_recvCmd.chargePoint.lAccountID,
                     m_recvCmd.chargePoint.lUserID, m_recvCmd.chargePoint.sPoint);
            nowSql = rtGetMilliseconds();
        } break;
    }
    nowCmd = rtGetMilliseconds();
    if (nowSql - dwNow > 1000) {
        ERR3("thread %d: frame time = %d [SQL=%d]\n", m_tid, nowSql - dwNow, m_recvCmd.cmdId);
    }
    if (nowCmd - nowSql > 1000) {
        ERR2("thread %d: frame time = %d [g_sendCmdList.AddCmd]\n", m_tid, nowCmd - nowSql);
    }
}

int CLgsThread::list_cmd_user_login() {
    int   iType;
    char* ret = nullptr;

    SAccountInfo info;
    info.privileges = 0;

    sprintf(
        m_sql,
        //       0          1          2           3        4     5								6         7			8		9
        "select `username`,`password`,`user_priv`,`locked`,`idx`,UNIX_TIMESTAMP(`lock_end`), "
        "`datetime`,`point`, `isVip`, `badult` "
        "from `user` "
        "where `username`='%s' and `deleted`=0",  // "where `username` LIKE CONVERT( _utf8 '%s' USING gb2312 ) and `deleted`=0"
        m_db.EscapeString(m_recvCmd.userLogin.szUsername).c_str());

    // strcpy(info.ip,m_recvCmd.userLogin.ip);

    // LOG2("thread %d: query db ,sql = %s",m_tid,m_sql);
    if (!m_db.Query(m_sql)) {
        ERR2("thread %d:query db failed,sql = %s", m_tid, m_sql);
        return RET_GET_FIELD_ERROR;
    }

    _query_member_center(m_recvCmd.userLogin.szUsername, m_recvCmd.userLogin.szPassword,
                         m_recvCmd.userLogin.szUserIP);

    // LOG(LV_M,"thread %d:query db ok",arg);
    if (!m_db.GetRow())
        return RET_GET_ROW_ERROR;
    bool  validate;
    char* realname;
    if (g_pWSDL->m_wsdl_is_pubwin == 1) {
        if (!m_db.GetFieldContent(0, &iType, &realname))
            return RET_PUBWIN_ERROR;
        validate = (stricmp(m_recvCmd.userLogin.szUsername, realname) == 0);
    } else {
        if (!m_db.GetFieldContent(0, &iType, &realname))
            return RET_GET_FIELD_ERROR;
        validate = (stricmp(m_recvCmd.userLogin.szUsername, realname) == 0 &&
                    strcmp(m_recvCmd.userLogin.szPassword, ret) == 0);
    }

    if (!m_db.GetFieldContent(1, &iType, &ret))
        return RET_GET_FIELD_ERROR;

    if (validate) {
        // check pwd ok get point card and gift info
        if (!m_db.GetFieldContent(4, &iType, &ret))  // idx
            return RET_GET_FIELD_ERROR;

        info.accountId   = atol(ret);
        info.accountName = m_recvCmd.userLogin.szUsername;
        info.password    = m_recvCmd.userLogin.szPassword;
        info.ip          = m_recvCmd.userLogin.szUserIP;
        // info.port = ???

        //tim.yang  读取会员时间
        char* timehuiyuan;
        if (!m_db.GetFieldContent(6, &iType, &timehuiyuan))  // "datetime"
            return RET_GET_FIELD_ERROR;
        info.huiYuanTime = timehuiyuan;
        //end

        //ac.ma    bl Adult
        char* blAdult;
        if (!m_db.GetFieldContent(9, &iType, &blAdult))
            return RET_GET_FIELD_ERROR;
        info.bAdult = atoi(blAdult);
        //end

        char* point;
        if (!m_db.GetFieldContent(7, &iType, &point))  // "point"
            return RET_GET_FIELD_ERROR;
        info.totalPot = atol(point);

        char* isVip;
        if (!m_db.GetFieldContent(8, &iType, &isVip))  // "point"
            return RET_GET_FIELD_ERROR;
        info.isVip = atol(isVip);

        m_sendCmd.userloginRet.lRet     = LOGIN_RET_SUCCESS;
        m_sendCmd.userloginRet.sRetCode = 0;
        m_sendCmd.accountInfo           = info;

        if (!m_db.GetFieldContent(3, &iType, &ret))  // "locked"
            return RET_GET_FIELD_ERROR;
        if (atol(ret) != 0) {
            if (!m_db.GetFieldContent(5, &iType, &ret))  // "lock_end"
                return RET_GET_FIELD_ERROR;
            long lLockEndTime = (long)atol(ret);
            if (lLockEndTime > time(NULL))  // 如果有时间，并且时间在未来的话，就说明是正在Lock状态
            {
                m_sendCmd.userloginRet.lRet =
                    LOGIN_RET_ACCOUNT_IS_LOCKER;  //  [8/25/2009 tim.yang] 玩家帐号被锁
                m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_AccountPaused;
                return RET_ADD_CMD;
            } else {
                bUnlock = true;  // 进行解锁动作
            }
        }

        if (!m_db.GetFieldContent(2, &iType, &ret))  // "privileges"
            return RET_GET_FIELD_ERROR;
        m_sendCmd.accountInfo.privileges = atol(ret);

        // 如果这个账号有权限，就比较IP地址列表，在列表外就不允许访问
        if (m_sendCmd.accountInfo.privileges != gwupLoginNormal) {
            if ((m_sendCmd.accountInfo.privileges & gwupDeveloper) == 0) {
                unsigned long dwIP = InvertIPOrder(StrToIP(m_recvCmd.userLogin.szUserIP));
                if (!g_cfg.gmAllowIP.CheckAllow(dwIP)) {
                    ERR2("GM Access denied for session [%s] user [%s]\n",
                         m_recvCmd.userLogin.szUserIP, m_sendCmd.accountInfo.accountName.c_str());
                    m_sendCmd.userloginRet.lRet = LOGIN_RET_FAILED_ACCOUNT_PAUSE;  // 显示为账号停用
                    m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_GMPaused;
                    return RET_ADD_CMD;
                }
            }
        }

        bool bAccept = false;
        if (m_sendCmd.accountInfo.privileges & gwupLoginNormal)  // 如果允许普通登陆
        {
            if (g_lgs->m_lStatus & lssLoginNormal)  // 允许普通用户
            {
                bAccept = true;
            }
        }
        if (!bAccept) {
            if (m_sendCmd.accountInfo.privileges & gwupGM_Mask)  // 允许所有GM权限登录
            {
                if (g_lgs->m_lStatus & lssLoginGM)  // 允许GM用户
                {
                    bAccept = true;
                }
            }
        }

        if (!bAccept)  // 如果服务器不允许
        {
            m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_SERVER_STOP;
            m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_LoginPadlock;
            m_sendCmd.accountInfo           = info;
        } else {
            if (bUnlock) {
                sprintf(m_sql,
                        "UPDATE `user` SET `locked`=%d,`latest_login_date` = NOW(), "
                        "`latest_login_ip` = '%s' WHERE `idx` = '%d'",
                        0, m_db.EscapeString(m_recvCmd.userLogin.szUserIP).c_str(),
                        m_sendCmd.accountInfo.accountId);
            } else {
                sprintf(m_sql,
                        "UPDATE `user` SET `latest_login_date` = NOW(), `latest_login_ip` = '%s' "
                        "WHERE `idx` = '%d'",
                        m_db.EscapeString(m_recvCmd.userLogin.szUserIP).c_str(),
                        m_sendCmd.accountInfo.accountId);
            }
            m_db.Query(m_sql);
        }
    } else {
        info.accountName                = m_recvCmd.userLogin.szUsername;
        info.password                   = m_recvCmd.userLogin.szPassword;
        m_sendCmd.userloginRet.lRet     = LOGIN_RET_FAILED_PWD_WRONG;
        m_sendCmd.userloginRet.sRetCode = LOGIN_RET_CODE_PasswordWrong;
        m_sendCmd.accountInfo           = info;

        _query_member_center(m_recvCmd.userLogin.szUsername, m_recvCmd.userLogin.szPassword,
                             m_recvCmd.userLogin.szUserIP);
    }
    return RET_ADD_CMD;
}

int CLgsThread::list_cmd_user_quit() {
    sprintf(
        m_sql,
        "insert into `consume`(`idx`,`accountID`,`ip`,`enter_time`,`leave_time`,`Point`,`Server`) "
        "values(0,%d,%d,FROM_UNIXTIME(%u),FROM_UNIXTIME(%u),%f,%d)",
        m_recvCmd.accountInfo.accountId, m_recvCmd.userLogout.dwIP, m_recvCmd.accountInfo.beginTime,
        m_recvCmd.userLogout.tLogoutTime, m_recvCmd.userLogout.fPoint,
        m_recvCmd.userLogout.sServer);
    m_db.Query(m_sql);
    return RET_NONE;
}

void CLgsThread::log_concurrent() {
    sprintf(m_sql,
            "INSERT INTO concurrent_log(log_time,gw1,gw2,gw3,gw4,gw5,gw6,gw7,gw8) "
            "VALUES(NOW(),%d,%d,%d,%d,%d,%d,%d,%d)",
            m_recvCmd.gwConcurrent[0], m_recvCmd.gwConcurrent[1], m_recvCmd.gwConcurrent[2],
            m_recvCmd.gwConcurrent[3], m_recvCmd.gwConcurrent[4], m_recvCmd.gwConcurrent[5],
            m_recvCmd.gwConcurrent[6], m_recvCmd.gwConcurrent[7]);
    m_db.Query(m_sql);
}

void CLgsThread::log_charge_card() {
    sprintf(m_sql,
            "INSERT INTO "
            "log_charge_card(`log_time`,`cmdSN`,`op`,`cardType`,`server`,`accountID`,`userID`,`"
            "cardID`) "
            "VALUES(NOW(),%d,%d,%d,%d,%d,%d,%s)",
            m_recvCmd.logCardCharge.lSn, m_recvCmd.logCardCharge.cOP,
            m_recvCmd.logCardCharge.cCardType, m_recvCmd.logCardCharge.sServer,
            m_recvCmd.logCardCharge.lAccountID, m_recvCmd.logCardCharge.lUserID,
            m_recvCmd.logCardCharge.szCardID);
    m_db.Query(m_sql);
}

long CLgsThread::account_create() {
    sprintf(m_sql,
            "INSERT INTO "
            "`user`(`username`,`password`,`user_priv`,`locked`,`lock_begin`,`lock_end`,`create_"
            "date`,`from`) "
            "VALUES('%s','%s',%d,%d,FROM_UNIXTIME(%u),FROM_UNIXTIME(%u),NOW(),7)",
            m_db.EscapeString(m_recvCmd.accountCreate.szUsername).c_str(),
            m_db.EscapeString(m_recvCmd.accountCreate.szPassword).c_str(),
            m_recvCmd.accountCreate.lPriv, m_recvCmd.accountCreate.sLocked,
            m_recvCmd.accountCreate.tLockBegin, m_recvCmd.accountCreate.tLockEnd);
    if (!m_db.Query(m_sql)) {
        return -1;
    }

    //sprintf(m_sql,"select `idx` from `user` where `username` LIKE CONVERT( _utf8 '%s' USING gb2312 )",
    sprintf(m_sql, "select `idx` from `user` where `username`='%s'",
            m_db.EscapeString(m_recvCmd.accountCreate.szUsername).c_str());

    if (!m_db.Query(m_sql)) {
        return -1;
    }

    if (!m_db.GetRow())
        return -1;

    int   iType;
    char* szIdx;

    if (!m_db.GetFieldContent(0, &iType, &szIdx))
        return -1;

    return atol(szIdx);
}

bool CLgsThread::account_lock() {
    if (m_recvCmd.accountLock.bUseUsername) {
        sprintf(m_sql,
                "UPDATE `user` SET "
                "`locked`=%d,`lock_begin`=FROM_UNIXTIME(%u),`lock_end`=FROM_UNIXTIME(%u) "
                "WHERE `username` LIKE CONVERT( _utf8 '%s' USING gb2312 )",
                m_recvCmd.accountLock.sLocked, m_recvCmd.accountLock.tLockBegin,
                m_recvCmd.accountLock.tLockEnd,
                m_db.EscapeString(m_recvCmd.accountLock.szUsername).c_str());
    } else {
        sprintf(m_sql,
                "UPDATE `user` SET "
                "`locked`=%d,`lock_begin`=FROM_UNIXTIME(%u),`lock_end`=FROM_UNIXTIME(%u) WHERE "
                "`idx` = '%d'",
                m_recvCmd.accountLock.sLocked, m_recvCmd.accountLock.tLockBegin,
                m_recvCmd.accountLock.tLockEnd, m_recvCmd.accountLock.lAccountID);
    }
    return m_db.Query(m_sql);
}

bool CLgsThread::account_setpriv() {
    if (m_recvCmd.accountSetPriv.bUseUsername) {
        sprintf(m_sql,
                "UPDATE `user` SET `user_priv`=%d "
                "WHERE `username` LIKE CONVERT( _utf8 '%s' USING gb2312 )",
                m_recvCmd.accountSetPriv.lPriv,
                m_db.EscapeString(m_recvCmd.accountSetPriv.szUsername).c_str());
    } else {
        sprintf(m_sql, "UPDATE `user` SET `user_priv`=%d WHERE `idx` = '%d'",
                m_recvCmd.accountSetPriv.lPriv, m_recvCmd.accountSetPriv.lAccountID);
    }
    return m_db.Query(m_sql);
}

bool CLgsThread::account_setpassword() {
    if (m_recvCmd.accountSetPwd.bUseUsername) {
        sprintf(m_sql,
                "UPDATE `user` SET `password`='%s' WHERE `username` LIKE CONVERT( _utf8 '%s' USING "
                "gb2312 )",
                m_db.EscapeString(m_recvCmd.accountSetPwd.szPassword).c_str(),
                m_db.EscapeString(m_recvCmd.accountSetPwd.szUsername).c_str());

        if (!m_db.Query(m_sql)) {
            return false;
        }

        if (m_recvCmd.accountSetPwd.lLoginOrderID != 0) {
            sprintf(m_sql, "select `idx` from `user` where `username`='%s'",
                    m_db.EscapeString(m_recvCmd.accountSetPwd.szUsername).c_str());

            if (!m_db.Query(m_sql)) {
                return false;
            }

            if (!m_db.GetRow())
                return false;

            int   iType;
            char* szIdx;

            if (!m_db.GetFieldContent(0, &iType, &szIdx))
                return false;

            m_recvCmd.accountSetPwd.lAccountID = atol(szIdx);
        }
    } else {
        sprintf(m_sql, "UPDATE `user` SET `password`='%s' WHERE `idx` = '%d'",
                m_db.EscapeString(m_recvCmd.accountSetPwd.szPassword).c_str(),
                m_recvCmd.accountSetPwd.lAccountID);
        return m_db.Query(m_sql);
    }
    return false;
}

bool CLgsThread::account_delete() {
    if (m_recvCmd.accountDelete.bUseUsername) {
        sprintf(m_sql,
                "UPDATE `user` SET `deleted`='%d' WHERE `username` LIKE CONVERT( _utf8 '%s' USING "
                "gb2312 )",
                m_recvCmd.accountDelete.sDeleted,
                m_db.EscapeString(m_recvCmd.accountDelete.szUsername).c_str());
    } else {
        sprintf(m_sql, "UPDATE `user` SET `deleted`='%d' WHERE `idx` = '%d'",
                m_recvCmd.accountDelete.sDeleted, m_recvCmd.accountDelete.lAccountID);
    }
    return m_db.Query(m_sql);
}

void CLgsThread::_query_member_center(const std::string strUserName, const std::string strPwd,
                                      const std::string strIP) {
    if (g_pWSDL->m_wsdl_on_off == 0)
        return;
    std::map<std::string, std::string> map_in;
    map_in.insert(make_pair("LoginMail", strUserName));
    map_in.insert(make_pair("LoginPass", strPwd));
    map_in.insert(make_pair("LoginIP", strIP));
    map_in.insert(make_pair("Ver", "0"));
    g_pWSDL->addJob_chkLogin(0, 0, map_in);
}

void CLgsThread::AddPoint(short sServer, long lAccountID, long lUserID, short sPoint) {
    char* curPoint;
    int   iType;
    char  ret[100];
    sprintf(m_sql,
            "update `user` "
            "set `point` = `point` + '%d' , `isVip` = 1 "
            "where `idx`='%d'",
            sPoint, lAccountID);
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        ERR1("user :query db failed,sql = %s", m_sql);
        AddPointRet(sServer, lAccountID, lUserID, sPoint, "点数中心正在维护，请稍候重试！", 0);
        return;
    }
    sprintf(m_sql,
            //        0
            "select `point` "
            "from `user` "
            "where `idx`='%d'",
            lAccountID);
    if (!m_db.Query(m_sql)) {
        ERR1("user :query db failed,sql = %s", m_sql);
        AddPointRet(sServer, lAccountID, lUserID, sPoint, "点数中心正在维护，请稍候重试！", 0);
        return;
    } else {
        if (!m_db.GetRow() || !m_db.GetFieldContent(0, &iType, &curPoint))
            AddPointRet(sServer, lAccountID, lUserID, sPoint, "没找到用户信息！", 0);
        sprintf(ret, "您获得了 %d 点数,当前点数为 %d。", sPoint, atoi(curPoint));
        LOG1("%d addpoint %d\n", lAccountID, sPoint);
        AddPointRet(sServer, lAccountID, lUserID, sPoint, ret, 1);
    }
}

bool CLgsThread::Deduct(short sServer, long lAccountID, long lUserID, short sPoint,
                        long lDeductOrderID) {
    //m_chargeSession.Deduct(sServer, lAccountID, lUserID, sPoint, lDeductOrderID);
    int   iType;
    char* Mysqlpoint;
    int   curPoint = 0;
    sprintf(m_sql,
            //        0
            "select `point` "
            "from `user` "
            "where `idx`='%d'",
            lAccountID);
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        ERR1("user :query db failed,sql = %s", m_sql);
        OnDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                 (char*)"点数中心正在维护，请稍候重试！", sPoint, curPoint);
        return false;
    }
    if (!m_db.GetRow()) {
        OnDeduct(sServer, lAccountID, lUserID, lDeductOrderID, (char*)"没找到用户信息！", sPoint,
                 0);
        return false;
    }
    if (!m_db.GetFieldContent(0, &iType, &Mysqlpoint))
        return false;
    curPoint = atoi(Mysqlpoint) - sPoint;
    if (curPoint < 0) {
        OnDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                 (char*)"您当前的点数不够，请去充值！", sPoint, curPoint);
        return false;
    }
    sprintf(m_sql,
            "update `user` "
            "set `point`= `point` - %d "
            "where `idx` = '%d'",
            sPoint, lAccountID);
    if (!m_db.Query(m_sql)) {
        ERR1("user : query db failed, sql = %s", m_sql);
        OnDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                 (char*)"点数中心正在维护，请稍候重试！", sPoint, curPoint);
        return false;
    }
    LOG1("%d deduct %d\n", lAccountID, sPoint);
    OnDeduct(sServer, lAccountID, lUserID, lDeductOrderID, (char*)"", sPoint, curPoint);
    return true;
}

void CLgsThread::OnDeduct(short sServer, long lAccountID, long lUserID, long lDeductOrderID,
                          char* cResult, short sPoint, short curPoint) {
    m_sendCmd.cmdId                      = LIST_CMD_USER_POINT_RET;
    m_sendCmd.chargePoint.sServer        = m_recvCmd.chargePoint.sServer;
    m_sendCmd.chargePoint.lAccountID     = m_recvCmd.chargePoint.lAccountID;
    m_sendCmd.chargePoint.lUserID        = lUserID;
    m_sendCmd.chargePoint.lDeductOrderID = lDeductOrderID;
    m_sendCmd.chargePoint.sPoint         = sPoint;
    m_sendCmd.chargePoint.sCurPoint      = curPoint;
    strcpy(m_sendCmd.chargePoint.cResult, cResult);

    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

bool CLgsThread::PreDeduct(short sServer, long lAccountID, long lUserID, short sPoint,
                           long lDeductOrderID, int event, long lAuctionId) {
    //m_chargeSession.Deduct(sServer, lAccountID, lUserID, sPoint, lDeductOrderID);
    int   iType;
    char* Mysqlpoint;
    int   curPoint = 0;
    char  ret[100];
    sprintf(m_sql,
            //        0
            "select `point` "
            "from `user` "
            "where `idx`='%d'",
            lAccountID);
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        ERR1("user :query db failed,sql = %s", m_sql);
        OnPreDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                    (char*)"点数中心正在维护，请稍候重试！", sPoint, curPoint, 0, event,
                    lAuctionId);
        return false;
    }
    if (!m_db.GetRow()) {
        OnPreDeduct(sServer, lAccountID, lUserID, lDeductOrderID, (char*)"没找到用户信息！", sPoint,
                    0, 0, event, lAuctionId);
        return false;
    }
    if (!m_db.GetFieldContent(0, &iType, &Mysqlpoint))
        return false;
    curPoint = atoi(Mysqlpoint) - sPoint;
    if (curPoint < 0) {
        OnPreDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                    (char*)"您当前的点数不够，请去充值！", sPoint, curPoint, 0, event, lAuctionId);
        return false;
    }
    sprintf(m_sql,
            "update `user` "
            "set `point`= `point` - %d "
            "where `idx` = '%d'",
            sPoint, lAccountID);
    if (!m_db.Query(m_sql)) {
        ERR1("user : query db failed, sql = %s", m_sql);
        OnPreDeduct(sServer, lAccountID, lUserID, lDeductOrderID,
                    (char*)"点数中心正在维护，请稍候重试！", sPoint, curPoint, 0, event,
                    lAuctionId);
        return false;
    }
    sprintf(ret, "您失去了 %d 点数,当前点数为 %d。", sPoint, curPoint);
    OnPreDeduct(sServer, lAccountID, lUserID, lDeductOrderID, ret, sPoint, curPoint, 1, event,
                lAuctionId);
    return true;
}

void CLgsThread::OnPreDeduct(short sServer, long lAccountID, long lUserID, long lDeductOrderID,
                             char* cResult, short sPoint, short curPoint, char cSucceed, int event,
                             long lAuctionId) {
    m_sendCmd.cmdId                      = LIST_CMD_PRE_USER_POINT_RET;
    m_sendCmd.chargePoint.sServer        = m_recvCmd.chargePoint.sServer;
    m_sendCmd.chargePoint.lAccountID     = m_recvCmd.chargePoint.lAccountID;
    m_sendCmd.chargePoint.lUserID        = lUserID;
    m_sendCmd.chargePoint.lDeductOrderID = lDeductOrderID;
    m_sendCmd.chargePoint.sPoint         = sPoint;
    m_sendCmd.chargePoint.sCurPoint      = curPoint;
    m_sendCmd.chargePoint.bSucceed       = cSucceed;
    m_sendCmd.chargePoint.event          = event;
    m_sendCmd.chargePoint.lAuctionId     = lAuctionId;
    strcpy(m_sendCmd.chargePoint.cResult, cResult);

    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

void CLgsThread::AddPointRet(short sServer, long lAccountID, long lUserID, short sPoint,
                             char const* cResult, char cSucceed) {
    m_sendCmd.cmdId                  = LIST_CMD_ADD_POINT_RET;
    m_sendCmd.chargePoint.sServer    = m_recvCmd.chargePoint.sServer;
    m_sendCmd.chargePoint.lAccountID = m_recvCmd.chargePoint.lAccountID;
    m_sendCmd.chargePoint.lUserID    = lUserID;
    m_sendCmd.chargePoint.sPoint     = sPoint;
    strcpy(m_sendCmd.chargePoint.cResult, cResult);
    m_sendCmd.chargePoint.bSucceed = cSucceed;

    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

//tim.yang  VIP卡
void CLgsThread::AddCardTime(long accountId, long time) {
    int   iType = 0;
    char* cdifference;
    long  ldifference = 0;
    sprintf(m_sql,
            "select DATEDIFF(`datetime`,NOW()) "
            "from `user` "
            "where `idx` = %d",
            accountId);  //去会员结束时间与当前时间差，结果可为负数。
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    if (!m_db.GetRow()) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }

    if (!m_db.GetFieldContent(0, &iType, &cdifference)) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    if (cdifference) {
        ldifference = atoi(cdifference);
        if (ldifference > 0) {
            sprintf(m_sql,
                    "update `user` "
                    "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                    "where `idx` = %d",
                    time, accountId);
        } else {
            sprintf(m_sql,
                    "update `user` "
                    "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                    "where `idx` = %d",
                    time - ldifference, accountId);
        }
    } else {
        sprintf(m_sql,
                "update `user` "
                "set `datetime` = NOW() "
                "where `idx` = %d",
                accountId);
        if (!m_db.Query(m_sql)) {
            AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
            return;
        }
        sprintf(m_sql,
                "update `user` "
                "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                "where `idx` = %d",
                time, accountId);
    }
    if (!m_db.Query(m_sql)) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    sprintf(m_sql,
            //         0
            "select `datetime` "
            "from `user` "
            "where `idx` = %d",
            accountId);
    if (!m_db.Query(m_sql)) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    if (!m_db.GetRow()) {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    char* viptime;
    if (!m_db.GetFieldContent(0, &iType, &viptime))  // "datetime"
    {
        AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 0, "");
        return;
    }
    std::string temptime = viptime;
    AddCardTimeRet(m_recvCmd.GWSId, m_recvCmd.userId, 1, temptime);
    //sprintf( m_sql,
    //	"update `user` "
    //	"set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
    //	"where `idx` = %d",
    //	time,accountId);
    ////check info
    //if(!m_binitdb)
    //	InitDB();
    //if (!m_db.Query(m_sql))
    //{
    //	ERR1("VIPCard :query db failed,sql = %s",m_sql);
    //	AddCardTimeRet(m_recvCmd.GWSId,m_recvCmd.userId,0,"");
    //	return;
    //}
    //sprintf( m_sql,
    //	//         0
    //	"select `datetime` "
    //	"from `user` "
    //	"where `idx` = %d",
    //	accountId);
    //if (!m_db.Query(m_sql))
    //{
    //	ERR1("VIPCard :query db failed,sql = %s",m_sql);
    //	AddCardTimeRet(m_recvCmd.GWSId,m_recvCmd.userId,0,"");
    //	return;
    //}
    //if(!m_db.GetRow())
    //{
    //	AddCardTimeRet(m_recvCmd.GWSId,m_recvCmd.userId,0,"");
    //	return ;
    //}
    //char *viptime;
    //int iType = 0;
    //if (!m_db.GetFieldContent(0, &iType, &viptime)) // "datetime"
    //{
    //	AddCardTimeRet(m_recvCmd.GWSId,m_recvCmd.userId,0,"");
    //	return;
    //}
    //std::string temptime = viptime;
    //AddCardTimeRet(m_recvCmd.GWSId,m_recvCmd.userId,1,temptime);
}

void CLgsThread::AddCardTimeRet(short gwsId, long lUserid, char isRight, std::string time) {
    m_sendCmd.cmdId                 = LIST_CMD_ADD_ACCOUNT_TIME_RET;
    m_sendCmd.isVIPCardAddTimeRight = isRight;
    m_sendCmd.VIPTime               = time;
    m_sendCmd.GWSId                 = gwsId;
    m_sendCmd.userId                = m_recvCmd.userId;
    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ADD_ACCOUNT_TIME_RET failed\n");
    }
}

//end
void CLgsThread::ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName,
                            long lSalesmanAccountID, const char* szSalesmanAccountName,
                            short sServer, long lUserID, const char* szUserName, short sLevel,
                            char cMetier, char cMetierLev, char cCardType, const char* szCardID,
                            const char* szCardPwd, long lUserParam, const char* userip) {
    //激活码。。。。。。。by tooth.shi 09/02/16
    char* type;
    char* attribute;
    char* userid;
    char* istrade;
    char* cdifference;
    int   iType;

    if (szSalesmanAccountName[0] != '\0')  //购买道具码
    {
        sprintf(
            m_sql,
            //       0          1          2           3        4     5								6
            "select `type`,`attribute`,`userid`,`cardtrade` "
            "from `card_number` "
            "where `cardidMD5`='%s'",  //
            szCardID);
    } else  //直接使用道具码
    {
        sprintf(
            m_sql,
            //       0          1          2           3        4     5								6
            "select `type`,`attribute`,`userid`,`cardtrade` "
            "from `card_number` "
            "where `cardid`='%s'",  //
            szCardID);
    }
    //check info
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        ERR1("card_number :query db failed,sql = %s", m_sql);
        ChargeCardRet(sServer, lUserID, (char*)szCardID, (char*)"点数系统暂时维护，请稍后重试！",
                      lUserParam);
        return;
    }
    if (!m_db.GetRow()) {
        ChargeCardRet(sServer, lUserID, (char*)szCardID, (char*)"您输入的道具码不存在，请检查！！",
                      lUserParam);
        return;
    }
    if (!m_db.GetFieldContent(2, &iType, &userid))
        return;
    if (strcmp(userid, "0")) {
        ChargeCardRet(sServer, lUserID, (char*)szCardID,
                      (char*)"您输入的道具码已被使用，请重新购买！", lUserParam);
        return;
    }
    if (!m_db.GetFieldContent(3, &iType, &istrade))
        return;
    if (strcmp(istrade, "0")) {
        if (szSalesmanAccountName[0] == '\0')  //直接使用道具码
        {
            ChargeCardRet(sServer, lUserID, (char*)szCardID,
                          (char*)"您输入的道具码目前存在交易状态，不能使用！", lUserParam);
            return;
        }
    }

    if (!m_db.GetFieldContent(0, &iType, &type))
        return;

    if (!m_db.GetFieldContent(1, &iType, &attribute))
        return;
    int  itype = atoi(type);
    int  iAttribute;
    long ldifference;
    char str[200];
    switch (itype) {
        case 5:  //point gift
            iAttribute = atoi(attribute);
            sprintf(m_sql,
                    "update `user`"
                    "set `point`= `point` + %d , `isVip` = 1 "
                    "where `idx` = %d",
                    iAttribute, lBuyerAccountID);
            if (!m_db.Query(m_sql)) {
                ERR1("user : query db failed, sql = %s", m_sql);
                ChargeCardRet(sServer, lUserID, (char*)szCardID,
                              (char*)"点数系统暂时维护，请稍后重试！", lUserParam);
                return;
            }
            if (szBuyerAccountName == NULL)
                sprintf(str, "恭喜您获得 %d 的点数！", iAttribute);
            else
                sprintf(str, " %d 点的道具卡！", iAttribute);
            ChargeCardRet(sServer, lUserID, (char*)szCardID, (char*)str, lUserParam, 1);
            break;
        case 6:  // vip time gift
            iAttribute = atoi(attribute);
            //去当前数据库会员截止日期与当前数据库日期差
            sprintf(m_sql,
                    "select DATEDIFF(`datetime`,NOW()) "
                    "from `user` "
                    "where `idx` = %d",
                    lBuyerAccountID);  //去会员结束时间与当前时间差，结果可为负数。
            if (!m_db.Query(m_sql)) {
                ERR1("user : query db failed, sql = %s", m_sql);
                ChargeCardRet(sServer, lUserID, (char*)szCardID,
                              (char*)"点数系统暂时维护，请稍后重试！", lUserParam);
                return;
            }
            if (!m_db.GetRow()) {
                ChargeCardRet(sServer, lUserID, (char*)szCardID,
                              (char*)"点数系统暂时维护，请稍后重试！！", lUserParam);
                return;
            }
            if (!m_db.GetFieldContent(0, &iType, &cdifference))
                return;
            if (cdifference) {
                ldifference = atoi(cdifference);
                if (ldifference > 0) {
                    sprintf(m_sql,
                            "update `user` "
                            "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                            "where `idx` = %d",
                            iAttribute, lBuyerAccountID);
                } else {
                    sprintf(m_sql,
                            "update `user` "
                            "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                            "where `idx` = %d",
                            iAttribute - ldifference, lBuyerAccountID);
                }
            } else {

                sprintf(m_sql,
                        "update `user` "
                        "set `datetime` = NOW() "
                        "where `idx` = %d",
                        lBuyerAccountID);
                if (!m_db.Query(m_sql)) {
                    ERR1("user : query db failed, sql = %s", m_sql);
                    ChargeCardRet(sServer, lUserID, (char*)szCardID,
                                  (char*)"点数系统暂时维护，请稍后重试！", lUserParam);
                    return;
                }
                sprintf(m_sql,
                        "update `user` "
                        "set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY ) "
                        "where `idx` = %d",
                        iAttribute, lBuyerAccountID);
            }
            if (!m_db.Query(m_sql)) {
                ERR1("user : query db failed, sql = %s", m_sql);
                ChargeCardRet(sServer, lUserID, (char*)szCardID,
                              (char*)"点数系统暂时维护，请稍后重试！", lUserParam);
                return;
            }
            if (szBuyerAccountName == NULL)
                sprintf(str, "恭喜您获得 %d 的天的VIP！", iAttribute);
            else
                sprintf(str, " %d 天的VIP卡！", iAttribute);
            ChargeCardRet(sServer, lUserID, (char*)szCardID, (char*)str, lUserParam, 1);
            break;
        default:
            AddGiftRet(1, szUserName, sServer, lUserID, sLevel, atoi(type), attribute, 0,
                       lSalesmanAccountID, const_cast<char*>(szCardID));
            break;
    }
    if (szSalesmanAccountName[0] != '\0')  //购买道具码
    {
        sprintf(m_sql,
                "update `card_number` "
                "set `userid`= %d ,`accountid` = %d ,`usetime` = NOW(),`userip` = '%s' "
                "where `cardidMD5` = '%s'",
                lUserID, lBuyerAccountID, userip, szCardID);

    } else  //直接使用道具码
    {
        sprintf(m_sql,
                "update `card_number` "
                "set `userid`= %d ,`accountid` = %d ,`usetime` = NOW(),`userip` = '%s' "
                "where `cardid` = '%s'",
                lUserID, lBuyerAccountID, userip, szCardID);
    }

    //update card_number
    if (!m_db.Query(m_sql)) {
        ERR1("card_number : query db failed, sql = %s", m_sql);
        return;
    }
}

void CLgsThread::ChargeCardRet(short sServer, long lUserID, char* szCardID, char* msg,
                               long lUserParam, char szSucceed /* = 0*/) {
    m_sendCmd.cmdId               = LIST_CMD_CHARGE_CARD_RET;
    m_sendCmd.chargePoint.sServer = sServer;
    m_sendCmd.chargePoint.lUserID = lUserID;
    strcpy(m_sendCmd.chargePoint.szCardID, szCardID);
    strcpy(m_sendCmd.chargePoint.cResult, msg);
    m_sendCmd.chargePoint.param    = lUserParam;
    m_sendCmd.chargePoint.bSucceed = szSucceed;

    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

void CLgsThread::AddGiftRet(long lSnCard, const char* szUsername, short sServer, long lUserID,
                            short sLevel, short sType, const char* szGiftString, long tLimitTime,
                            char cFromCardID, char* szCardID) {
    m_sendCmd.cmdId = LIST_CMD_ADD_GIFT_RET;
    strcpy(m_sendCmd.chargePoint.szUserName, szUsername);
    m_sendCmd.chargePoint.sServer   = sServer;
    m_sendCmd.chargePoint.lUserID   = lUserID;
    m_sendCmd.chargePoint.sUserLev  = sLevel;
    m_sendCmd.chargePoint.cCardType = sType;
    strcpy(m_sendCmd.chargePoint.cResult, szGiftString);
    m_sendCmd.chargePoint.tTime          = tLimitTime;
    m_sendCmd.chargePoint.lDeductOrderID = cFromCardID;
    strcpy(m_sendCmd.chargePoint.szCardID, szCardID);
    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

void CLgsThread::CheckCanTrade(long lAccountID, const char* szAccountName, long lUserID,
                               char cCardType, short sServer, const char* szCardID,
                               const char* szCardPwd, long lUserParam, long lTime) {
    char* userid;
    char* type;
    char* attribute;
    char* istrade;
    int   iType;

    sprintf(
        m_sql,
        //       0          1          2           3        4     5								6
        "select `type`,`attribute`,`userid`,`cardtrade` "
        "from `card_number` "
        "where `cardidMD5`='%s'",  // md5 id [3/19/2009 tooth.shi]
        szCardID);

    //check info
    if (!m_binitdb)
        InitDB();
    if (!m_db.Query(m_sql)) {
        ERR1("card_number :query db failed,sql = %s", m_sql);
        CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 0,
                            (char*)"点数系统暂时维护，请稍后重试！");
        return;
    }
    if (!m_db.GetRow()) {
        CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 0,
                            (char*)"您输入的道具码不存在，请检查！！");
        return;
    }
    if (!m_db.GetFieldContent(2, &iType, &userid))
        return;
    if (!m_db.GetFieldContent(0, &iType, &type))
        return;
    int itype = atoi(type);
    if (itype != 5 && itype != 6) {
        CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 0,
                            (char*)"您输入的道具码类型不符合交易要求，请重新购买！");
    }
    if (strcmp(userid, "0")) {
        CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 0,
                            (char*)"您输入的道具码已被使用，请重新购买！");
        return;
    }
    if (!m_db.GetFieldContent(3, &iType, &istrade))
        return;
    if (strcmp(istrade, "0")) {
        CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 0,
                            (char*)"您输入的道具码已经处于交易状态，不能寄卖！");
        return;
    }
    char ret[60];
    int  iAttribute;
    int  cardGroup = 0;
    if (!m_db.GetFieldContent(1, &iType, &attribute))
        return;
    iAttribute = atoi(attribute);
    if (itype == 5) {
        sprintf(ret, "一张获得 %d 点数的卡", iAttribute);
        cardGroup = 21;
    } else if (itype == 6) {
        sprintf(ret, "一张获得 %d 天VIP的卡", iAttribute);
        cardGroup = 22;
    }
    CheckCanTradeResult(lUserParam, sServer, lUserID, szCardID, 1, (char*)&ret, cardGroup);
    sprintf(m_sql,
            "update `card_number` "
            "set `cardtrade`= %d  "
            "where `cardidMD5` = '%s'",
            1, szCardID);
    //update card_number
    if (!m_db.Query(m_sql)) {
        ERR1("card_number : query db failed, sql = %s", m_sql);
        return;
    }
}

void CLgsThread::CheckCanTradeResult(long lSn, short sServer, long lUserID, const char* szCardID,
                                     char cSucceeded, const char* szMsg, long lCardGroupType) {
    m_sendCmd.cmdId               = LIST_CMD_CARD_TRADE_RET;
    m_sendCmd.chargePoint.param   = lSn;
    m_sendCmd.chargePoint.sServer = sServer;
    m_sendCmd.chargePoint.lUserID = lUserID;
    strcpy(m_sendCmd.chargePoint.szCardID, szCardID);
    m_sendCmd.chargePoint.bSucceed = cSucceeded;
    strcpy(m_sendCmd.chargePoint.cResult, szMsg);
    m_sendCmd.chargePoint.lCardGroupType = lCardGroupType;
    if (!g_sendCmdList.AddCmd(m_sendCmd)) {
        ERR("ERROR: Add deduct ret LIST_CMD_ACCOUNT_DELETE failed\n");
    }
}

int CLgsThread::pubwin_list_cmd_user_login() {
    int   iType;
    char* activation;
    int   ret;

    ret = list_cmd_user_login();
    if (ret != RET_ADD_CMD) {
        return ret;
    }

    sprintf(
        m_sql,
        //       0          1          2           3        4     5								6         7			8
        "select `activation`,`password` "
        "from `user` "
        "where `username`='%s' and `deleted`=0",  // "where `username` LIKE CONVERT( _utf8 '%s' USING gb2312 ) and `deleted`=0"
        m_db.EscapeString(m_recvCmd.userLogin.szUsername).c_str());

    if (!m_db.Query(m_sql)) {
        ERR2("thread %d:query db failed,sql = %s", m_tid, m_sql);
        return RET_GET_FIELD_ERROR;
    }
    // LOG(LV_M,"thread %d:query db ok",arg);
    if (!m_db.GetRow())
        return RET_GET_ROW_ERROR;

    char* password;
    if (!m_db.GetFieldContent(0, &iType, &activation))
        return RET_GET_FIELD_ERROR;
    if (!m_db.GetFieldContent(1, &iType, &password))
        return RET_GET_FIELD_ERROR;
    if (strcmp(activation, "0") == 0)
        return RET_GET_ROW_ERROR;
    if (strcmp(password, m_db.EscapeString(m_recvCmd.userLogin.glomPubwinPWD).c_str()) == 0)
        return ret;

    return LOGIN_RET_FAILED_ACCOUNT_PAUSE;  //PZH
    //验证密码， 如果正确， 则不再去pubwin进行验证
    //lyymark 登录验证
    CURL* curl;
    char  errorBuffer[300];
    memset(errorBuffer, 0, sizeof(errorBuffer));
    CURLcode    cCode;
    std::string postcontent;
    std::string pubwinaddress;
    std::string urlname  = m_recvCmd.userLogin.szPubWinUsername;
    std::string urlname1 = m_recvCmd.userLogin.szUsername;
    std::string urlpwd   = m_recvCmd.userLogin.szPassword;
    std::string urlKey   = m_recvCmd.userLogin.pubwinKey;
    curl                 = curl_easy_init();
    if (urlKey != "0") {
        postcontent += "gameUser=";
        postcontent += urlname1;
        postcontent += "&gameType=FSSJ&key=";
        postcontent += urlpwd;
        curl_easy_setopt(curl, CURLOPT_URL, g_pWSDL->m_pubwin_keyaddress);  //设置连接网络
    } else {
        postcontent += "client=true&username=";
        postcontent += urlEncoding(urlname);
        postcontent += "&password=";
        postcontent += urlEncoding(urlpwd);
        postcontent += "&gameType=FSSJ";
        curl_easy_setopt(curl, CURLOPT_URL, g_pWSDL->m_pubwin_address);  //设置连接网络
    }
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 25);  //设置连接网络等待回应30s
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postcontent.c_str());  //设置POST数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     get_pubwin_content);  //设置响应处理函数， 为了接收从网站获取的数据
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);  //获取失败信息
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);               //获取失败信息
    memset(m_pubwin_ret, 0, sizeof(m_pubwin_ret));
    cCode = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (cCode != 0) {
        ERR2("pubwinLoginError:%s,username=%s\n", errorBuffer, m_recvCmd.userLogin.szUsername);
    }
    m_pubwin_ret[sizeof(m_pubwin_ret) - 1] = '\0';
    if (strcmp(m_pubwin_ret, "40000") == 0) {
        sprintf(m_sql, "update `user` set `password` = '%s' where `username`='%s'",
                m_db.EscapeString(m_recvCmd.userLogin.glomPubwinPWD).c_str(),
                m_db.EscapeString(m_recvCmd.userLogin.szUsername).c_str());
        if (!m_db.Query(m_sql)) {
            ERR2("thread %d:query db failed,sql = %s", m_tid, m_sql);
            return RET_GET_FIELD_ERROR;
        }
        ERR("%s login successful\n", m_recvCmd.userLogin.szUsername);
        return ret;
    }
    return RET_PUBWIN_ERROR;
}

size_t CLgsThread::get_pubwin_content(void* ptr, size_t size, size_t nmemb, void* stream) {
    if (size * nmemb < 99) {
        memcpy(m_pubwin_ret, ptr, size * nmemb);
        m_pubwin_ret[size * nmemb + 1] = '\0';
        return size * nmemb;
    }
    // 	char xxxx[1024];
    // 	if (size*nmemb <= 1024)
    // 	{
    // 		memcpy(xxxx,ptr,size*nmemb);
    // 		return size*nmemb;
    // 	}
    return 0;
}

std::string CLgsThread::urlEncoding(std::string& sIn) {
    //cout << "size: " << sIn.size() << endl;
    std::string sOut;
    for (unsigned int ix = 0; ix < sIn.size(); ix++) {
        BYTE buf[4];
        memset(buf, 0, 4);
        if (isalnum((BYTE)sIn[ix])) {
            buf[0] = sIn[ix];
        } else if (isspace((BYTE)sIn[ix])) {
            buf[0] = '+';
        } else {
            buf[0] = '%';
            buf[1] = toHex((BYTE)sIn[ix] >> 4);
            buf[2] = toHex((BYTE)sIn[ix] % 16);
        }
        sOut += (char*)buf;
    }
    return sOut;
}

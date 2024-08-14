#ifndef __LGS_GWS_H__
#define __LGS_GWS_H__

const int MAX_USER_NAME_SIZE = 20;
const int MAX_USER_PWD_SIZE  = 32;

struct SAccountInfo {
    DWORD       accountId;
    std::string accountName;  // 	帐号
    std::string password;
    DWORD       beginTime;   // 	消费开始时间,1970年到现在的秒数
    long        privileges;  // 权限 (enum EUserPrivileges [gw_protocol.h])
    long        totalPot;    // 	帐号剩余微点数
    char        chargeType;  // 	计费方式

    bool bAdult;  //是否成人	ac.ma

    std::string huiYuanTime;  //tim.yang  会员时间
    std::string ip;
    short       port;
    char        isVip;  //是否是会员（冲过点数就是会员）

    /*
	//	包月
	ULONG month_etime;			// 	包月到期时间

	//	时段
	ULONG period_time;			// 	持续时间(秒)
	ULONG period_pot;			// 	花费微点数

	ULONG p_pot_used;			// 	已花费微点数
	ULONG p_end_time;			// 	已预扣到期时间
	*/

    friend CG_CmdPacket& operator<<(CG_CmdPacket& cmd, SAccountInfo& p) {
        cmd << p.accountId << p.accountName << p.password << p.beginTime << p.totalPot
            << p.chargeType << p.privileges << p.huiYuanTime << p.bAdult;
        cmd << p.ip << p.port << p.isVip;
        return cmd;
    }
};

// GameWorld Server State

const char gwssNone        = 0;
const char gwssWaitRegion  = 1;
const char gwssAcceptLogin = 2;

// login server status 登陆服务器的状态

const long lssLoginNormal = 0x0001;  // 用户允许登陆
const long lssLoginGM     = 0x0002;  // GM允许登陆
const long lssUpdateList  = 0x0004;  // 发送UpdateList回去

// account user privileges 帐号权限 [注意和gw_protocol.h中的EUserPrivileges同步更新]

const long gwupLoginNormal = 0x0001;  // 普通登陆

const long gwupLoginGM      = 0x0010;  // 高级登录
const long gwupFastMove     = 0x0020;  // 快速移动
const long gwupMoveUser     = 0x0040;  // 抓回玩家
const long gwupAddItems     = 0x0080;  // 添加道具
const long gwupModifyAttrib = 0x0100;  // 修改属性
const long gwupReloadScript = 0x0400;  // 重新载入脚本
const long gwupDeveloper    = 0x0800;  // 开发者专用
const long gwupGM_Mask      = 0x0FF0;  // 所有GM命令

const long gwupMonitorState  = 0x1000;  // 监控状态
const long gwupSendBillboard = 0x2000;  // 发送公告
const long gwupServerOperate = 0x4000;  // 关闭或重启服务器

// Command
const short lgs2gws_base = 1000;  // 基础消息类型起始编号

const short lgs2gws_accept_login            = lgs2gws_base + 1;   // 接受登录请求
const short lgs2gws_user_login_ret          = lgs2gws_base + 2;   // 用户登录返回
const short lgs2gws_kick_user               = lgs2gws_base + 3;   // 踢出用户
const short lgs2gws_shutdown_gws            = lgs2gws_base + 4;   // 关闭游戏世界服务器
const short lgs2gws_bulletin                = lgs2gws_base + 6;   // 发送公告
const short lgs2gws_bulletin_shutdown       = lgs2gws_base + 7;   // 关闭公告功能
const short lgs2gws_check_version_ret       = lgs2gws_base + 8;   // 检查版本返回
const short lgs2gws_check_sub_version_ret   = lgs2gws_base + 9;   // 检查子版本返回
const short lgs2gws_register_ret            = lgs2gws_base + 10;  // 注册请求返回
const short lgs2gws_tester                  = lgs2gws_base + 11;  // 测试消息
const short lgs2gws_user_message            = lgs2gws_base + 12;  // 用户消息
const short lgs2gws_add_gift                = lgs2gws_base + 13;  // 添加礼物
const short lgs2gws_clear_gift              = lgs2gws_base + 14;  // 清除礼物
const short lgs2gws_charge_card_ret         = lgs2gws_base + 15;  // 充值卡返回
const short lgs2gws_query_point_ret         = lgs2gws_base + 16;  // 查询积分返回
const short lgs2gws_broadcast_to_all_region = lgs2gws_base + 17;  // 广播到所有区域
const short lgs2gws_broadcast_to_gameworld  = lgs2gws_base + 18;  // 广播到游戏世界
const short lgs2gws_card_can_trade_ret      = lgs2gws_base + 19;  // 卡片是否可以交易
const short lgs2gws_deduct_ret              = lgs2gws_base + 20;  // 扣除操作返回
const short lgs2gws_prededuct_ret           = lgs2gws_base + 21;  // 预扣操作返回
const short lgs2gws_license_check_ret       = lgs2gws_base + 22;  // 许可检查返回
const short lgs2gws_license_check_rs_ret    = lgs2gws_base + 23;  // 许可检查结果返回
const short lgs2gws_card_charged_event      = lgs2gws_base + 24;  // 卡片充值事件
const short lgs2gws_kick_all_user           = lgs2gws_base + 25;  // 踢出所有用户
const short lgs2gws_update_point_ret        = lgs2gws_base + 26;  // 更新积分返回
const short lgs2gws_add_point_ret           = lgs2gws_base + 27;  // 添加积分返回
const short lgs2gws_add_account_time_ret = lgs2gws_base + 28;  // 添加账户时间返回（VIP卡相关）
const short gws2lgs_base = 2000;                               // 基础消息类型起始编号

const short gws2lgs_user_login              = gws2lgs_base + 1;   // 用户登录
const short gws2lgs_user_quit               = gws2lgs_base + 2;   // 用户退出
const short gws2lgs_shutdown_ok             = gws2lgs_base + 3;   // 关闭确认
const short gws2lgs_update_state            = gws2lgs_base + 4;   // 更新状态
const short gws2lgs_query_state             = gws2lgs_base + 6;   // 查询状态
const short gws2lgs_update_gws_state        = gws2lgs_base + 7;   // 更新游戏世界状态
const short gws2lgs_check_version           = gws2lgs_base + 8;   // 检查版本
const short gws2lgs_check_sub_version       = gws2lgs_base + 9;   // 检查子版本
const short gws2lgs_register                = gws2lgs_base + 10;  // 注册
const short gws2lgs_tester_ret              = gws2lgs_base + 11;  // 测试返回
const short gws2lgs_user_enter_game         = gws2lgs_base + 12;  // 用户进入游戏
const short gws2lgs_gm_command              = gws2lgs_base + 13;  // GM命令
const short gws2lgs_charge_card             = gws2lgs_base + 14;  // 充值卡
const short gws2lgs_query_point             = gws2lgs_base + 15;  // 查询积分
const short gws2lgs_keep_online             = gws2lgs_base + 16;  // 保持在线
const short gws2lgs_broadcast_to_all_region = gws2lgs_base + 17;  // 广播到所有区域
const short gws2lgs_broadcast_to_gameworld  = gws2lgs_base + 18;  // 广播到游戏世界
const short gws2lgs_card_can_trade          = gws2lgs_base + 19;  // 卡片是否可以交易
const short gws2lgs_deduct                  = gws2lgs_base + 20;  // 扣除操作
const short gws2lgs_prededuct               = gws2lgs_base + 21;  // 预扣操作
const short gws2lgs_lock_account            = gws2lgs_base + 22;  // 锁定账户
const short gws2lgs_license_check           = gws2lgs_base + 23;  // 许可检查
const short gws2lgs_license_check_rs        = gws2lgs_base + 24;  // 许可检查结果
const short gws2lgs_user_change_block       = gws2lgs_base + 25;  // 用户变更区块
const short gws2lgs_card_failed             = gws2lgs_base + 26;  // 卡片失败
const short gws2lgs_add_point               = gws2lgs_base + 27;  // 添加积分
const short gws2lgs_add_account_time = gws2lgs_base + 28;  // 添加账户时间（VIP卡相关）


const long LOGIN_RET_SUCCESS               = 0;    // 登录成功
const long LOGIN_RET_FAILED_UNKNOWN        = -1;   // 登录失败：未知错误
const long LOGIN_RET_FAILED_USER_ONLINE    = -2;   // 登录失败：用户已在线
const long LOGIN_RET_FAILED_ACCOUNT_STOP   = -3;   // 登录失败：账户已停用
const long LOGIN_RET_FAILED_ACCOUNT_PAUSE  = -4;   // 登录失败：账户已暂停
const long LOGIN_RET_FAILED_POT_NOT_ENOUGH = -5;   // 登录失败：积分不足
const long LOGIN_RET_FAILED_PWD_WRONG      = -6;   // 登录失败：密码错误
const long LOGIN_RET_FAILED_NOT_FOUND      = -7;   // 登录失败：账户不存在
const long LOGIN_RET_FAILED_SERVER_FULL    = -8;   // 登录失败：服务器已满
const long LOGIN_RET_FAILED_SERVER_STOP    = -9;   // 登录失败：服务器维护或关闭
const long LOGIN_RET_FAILED_NEW_CARD       = -10;  // 登录失败：使用的是新手卡
const long LOGIN_RET_FAILED_USERDEFINE     = -11;  // 登录失败：自定义错误信息
const long LOGIN_RET_FAILED_PUBWIN_CENTER  = -12;  // 登录失败：同步pubwin失败
const long LOGIN_RET_ACCOUNT_IS_LOCKER     = -13;  // 登录失败：账户被锁定
const long LOGIN_RET_FAILED_ACCOUNT_WRONG5 = -14;  // 登录失败：连续错误，账户锁定10分钟

const short LOGIN_RET_CODE_Unknown         = 100;  //
const short LOGIN_RET_CODE_ShutdownTime    = 101;  // 服务器处在准备关闭状态
const short LOGIN_RET_CODE_LoginPadlock    = 102;  // 服务器禁止玩家进入
const short LOGIN_RET_CODE_DataError       = 103;  // 数值错误
const short LOGIN_RET_CODE_ChargeTimeout   = 104;  // 计费服务器登陆超时
const short LOGIN_RET_CODE_ActivingTimeout = 105;  // 激活超时
const short LOGIN_RET_CODE_CreatingTimeout = 106;  // 创建角色超时
const short LOGIN_RET_CODE_CreatingFailed  = 107;  // 创建角色失败
const short LOGIN_RET_CODE_ActivingFailed  = 108;  // 激活失败
const short LOGIN_RET_CODE_NewCard         = 109;  // 新手卡
const short LOGIN_RET_CODE_UserOnlined     = 110;  // 用户已经在线
const short LOGIN_RET_CODE_PotNotEnough    = 111;  // 点数不足
const short LOGIN_RET_CODE_AccountStoped   = 112;  // 帐号停止
const short LOGIN_RET_CODE_AccountPaused   = 113;  // 帐号暂停
const short LOGIN_RET_CODE_PasswordWrong   = 114;  // 密码错误
const short LOGIN_RET_CODE_DatabaseError   = 115;  // 数据库错误
const short LOGIN_RET_CODE_NotFound        = 116;  // 没有找到角色
const short LOGIN_RET_CODE_GMPaused        = 117;  // GM在非法区域登陆

const char KICK_USER_UNKNOWN        = 0;  // 踢出原因：未知
const char KICK_USER_MULTI_LOGIN    = 1;  // 踢出原因：多重登录
const char KICK_USER_POT_NOT_ENOUGH = 2;  // 踢出原因：积分不足
const char KICK_USER_GM             = 3;  // 踢出原因：GM命令
const char KICK_USER_USERDEFINE     = 4;  // 踢出原因：自定义


#endif

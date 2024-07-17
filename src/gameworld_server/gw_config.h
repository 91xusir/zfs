#ifndef _INC_GW_CONFIG_H_
#define _INC_GW_CONFIG_H_

#define GW_LOGIN_TIMEOUT				600000

#define	GW_MAX_DB_COMMAND_LENGTH		20480 /* 20k */
#define GW_MAX_USER_PER_ACCOUNT			4
#define GW_MAX_MAIL						30

#define GW_DEFAULT_CLIENT_CAPACITY		100
#define GW_DEFAULT_CLIENT_LISTEN_PORT	9999
#define GW_DEFAULT_REGION_CAPACITY		32
#define GW_DEFAULT_REGION_LISTEN_PORT	5555

#define GW_DEFAULT_DB_QUEUE_CAPACITY	100

#define GW_SEND_REGION_LIST_INTERVAL	60

#define GW_SAVE_USER_DATA_INTERVAL		30

#define GW_LINESYS_PENDING_CHECK_INTERVAL  60

#define GW_MEM_CHECK_INTERVAL  60

#define GW_MIN_NAME_LENGTH				3
#define GW_MAX_NAME_LENGTH				32

#define GW_SWITCH_REGION_TIMEOUT		30

// switch region timeout,in gameworld is 90s,in region is 60s
// region的时间可以在region.ini中修改，但game world的超时在代码中，需要特别小心
#define GW_USER_SWITCH_REGION_TIMEOUT	90

#define GW_REGION_LOST_TIMEOUT			30

#define GW_CONNECT_DB_INTERVAL			10
#define GW_CONNECT_LOGIN_INTERVAL		10

#define GW_CMD_PACKET_SIZE				(1024*16)

// #define GW_KICK_COUNT				3
// #define GW_KICK_COUNT_CAP				8

#define GW_SQL_LOG_FILE					"gameworld_server_sql.log"

#endif // _INC_GW_CONFIG_H_

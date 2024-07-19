
#ifndef GC_CHAT_CONST_H 
#define GC_CHAT_CONST_H

typedef unsigned long       DWORD;

const int EACH_FORFEND_TIME				=5*1000;							//每次违规增加的禁言时间[毫秒]
const int CHECK_OFFEND_TIME				=5*1000;							//检查违规的时间[毫秒]
const int CHAT_MESSAGE_MAX_COUNT		=1024;								//聊天的最大字数[英文]

const int CHECK_TIME					=2*1000;							//检查刷屏的时间[毫秒]
const int SPEAKER_CHECK_TIME			=30*1000;							//喇叭的使用间隙【毫秒】
const int TIME_BUFFER					=6;									//保存各聊天频道的上一次发言时间

const int COUNTY_CHECK_TIME				=30*1000;							//国战检查刷屏的时间[毫秒]
const int COUNTY_LIMIT_COUNT			=5;									//国战规定时间内的限制次数
const int COUNTY_TIME_BUFFER			=COUNTY_LIMIT_COUNT-1;				//保存国战刷屏时间的缓存

const DWORD SYSTEM_INFO_SCROLL_TIME		=300;								//系统信息滚动时间[单位：毫秒]
const float SYSTEM_INFO_CLEAR_TIME		=100.0f;							//系统清除信息时间间隔[单位：秒] // 此处改为动态刷新任务提示

const int MAX_HEIGHT_CHANGE_SIZE		=240;								//聊天框最大可变高度
const int MAX_HEIGHT_STATE				=5;									//聊天框最大改变档数//至少要有两档
const int EACH_HEIGHT					=MAX_HEIGHT_CHANGE_SIZE/MAX_HEIGHT_STATE;	//每档的改变高度值



//const char* LIMIT_MESSAGE;//="您说话太快了，请稍候。";

const char UI_CHANNEL_WHOLE_ID[]		="fmchannel";						//复选择框的全名
const char UI_CHANNEL_ID[]				="fmchannel";						//复选择框


const char UI_ALL_LIST_WHOLE_ID[]		="fmdialg.channelall";				//所有频道列表
const char UI_ALL_LIST_ID[]				="channelall";						//所有频道列表
const char UI_TEAM_LIST_WHOLE_ID[]		="fmdialg.channelteam";				//队伍频道列表
const char UI_TEAM_LIST_ID[]			="channelteam";						//队伍频道列表
const char UI_GUILD_LIST_WHOLE_ID[]		="fmdialg.channelguild";			//诸侯频道列表
const char UI_GUILD_LIST_ID[]			="channelguild";					//诸侯频道列表
const char UI_COUNTRY_LIST_WHOLE_ID[]	="fmdialg.channelcountry";			//国家频道列表
const char UI_COUNTRY_LIST_ID[]			="channelcountry";					//国家频道列表


const char UI_FEEDBACK_SWITCH_WHOLE_ID[] 
										= "fmdialg.btnFeedbackclose";		//改变对话背景框的开关
const char UI_FEEDBACK_SWITCH_ID[]		= "btnFeedbackclose";				//改变对话背景框的开关


const char UI_CHAT_FACE_SWITCH_WHOLE_ID[]="fmdialg.btnchatface";			//聊天表情框的开关
const char UI_CHAT_FACE_SWITCH_ID[]		="btnchatface";						//聊天表情框的开关

const char UI_CHANNEL_BUTTON_ID[]		="btnchanel";						//复选框打开关闭的按钮
const char UI_CHANNEL_BUTTON_WHOLE_ID[]	="fmdialg.btnchanel";				//复选框打开关闭的按钮的全名
const char UI_DOSKEY_LIST_WHOLE_ID[]	="listchater";						//Doskey的list的全名
const char UI_INPUT_WHOLE_ID[]			="fmdialg.fmtxtdialog.txtdialog";	//输入框的全名

const char UI_SYSTEM_FORM_ALL_WHOLE_ID[]="panSysteminfo";					//系统信息框的全名
const char UI_SYSTEM_FORM_BOX_WHOLE_ID[]="panSysteminfo.txtFeedback";		//系统信息框的上字部分的全名

const char UI_CHAT_FACE_FORM_WHOLE_ID[]	="fmchatface";						//聊天表情框
const char UI_CHAT_FACE_FORM_ID[]		="fmchatface";						//聊天表情框
const char UI_CHAT_FACE_ICON_LIST_WHOLE_ID[]="fmchatface.iconlist";			//聊天表情框中的图标列表框
const char UI_CHAT_FACE_ICON_LIST_ID[]	="iconlist";						//聊天表情框中的图标列表框



//聊天频道复选框的UI标识
const char UI_PERSONAL_CHANNEL_WHOLE_ID[]="layworld.fmchannel.ckpersoncl";
const char UI_TEAM_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckgroupcl";
const char UI_GUILD_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckguildcl";
const char UI_NORMAL_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.cknormalcl";
const char UI_PUBLIC_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckallcl";
const char UI_COUNTRY_CHANNEL_WHOLE_ID[]="layworld.fmchannel.ckcountrycl";
const char UI_SYSTEM_CHANNEL_ID[]		="";

//聊天频道接收时的前缀的描述
/*
const char* PERSONAL_CHANNEL_DESCRIPTION;//[]="[密语]";
const char* TEAM_CHANNEL_DESCRIPTION;//[]="[队伍]";
const char* GUILD_CHANNEL_DESCRIPTION;//[]="[诸侯]";
const char*	NORMAL_CHANNEL_DESCRIPTION;//[]="";
const char* PUBLIC_CHANNEL_DESCRIPTION;//[]="[公开]";
const char* COUNTRY_CHANNEL_DESCRIPTION;//[]="[国家]";
const char* SYSTEM_CHANNEL_DESCRIPTION;//[]="[系统]";
*/

//聊天频道接收时的颜色
const DWORD PERSONAL_CHANNEL_COLOR		=0xffDDA0DD;		//私人频道
const DWORD TEAM_CHANNEL_COLOR			=0xff90EE90;		//队伍频道
const DWORD UNION_CHANNEL_COLOR			=0xffEEEE00;		//帮派频道
const DWORD BATTLE_CHANNEL_COLOR		=0xffCD3700;		//战场频道
const DWORD NEAR_CHANNEL_COLOR			=0xffD1EEEE;		//附近频道
const DWORD WORLD_CHANNEL_COLOR			=0xFF00E5EE;		//世界频道
const DWORD SYSTEM_CHANNEL_COLOR		=0xFFEEC900;		//系统频道

//聊天频道的MASK[用于界面]
const int PERSONAL_CHANNEL_MASK			=0x01;				//私人频道
const int TEAM_CHANNEL_MASK				=0x02;				//队伍频道
const int UNION_CHANNEL_MASK			=0x04;				//帮派频道
const int BATTLE_CHANNEL_MASK			=0x08;				//战场频道
const int NEAR_CHANNEL_MASK				=0x10;				//附近频道
const int WORLD_CHANNEL_MASK			=0x20;				//世界频道
const int BATTLEINFO_CHANNEL_MASK		=0x40;				//战斗信息
const int SYSTEM_CHANNEL_MASK			=0x80;				//系统频道
const int SPEAKER_CHANNEL_MASK			=0x100;				//喇叭频道  heten

//聊天频道的前缀
const char SPACE_FLAG1[]				=" ";				//空格
const char SPACE_FLAG2[]				="　";				//空格

const char PERSONAL_CHANNEL_FLAG1[]		="/";				//密语频道
const char PERSONAL_CHANNEL_FLAG2[]		="／";				//密语频道

const char TEAM_CHANNEL_FLAG1[]			="#";				//队伍频道
const char TEAM_CHANNEL_FLAG2[]			="＃";				//队伍频道

const char UNION_CHANNEL_FLAG1[]		="@";				//帮派频道
const char UNION_CHANNEL_FLAG2[]		="＠";				//帮派频道
const char UNION_CHANNEL_FLAG3[]		="·";				//帮派频道

const char BATTLE_CHANNEL_FLAG1[]		="$";				//战场频道
const char BATTLE_CHANNEL_FLAG2[]		="￥";				//战场频道
const char BATTLE_CHANNEL_FLAG3[]		="￥";				//战场频道

const char NORMAL_CHANNEL_FLAG1[]		="";				//一般频道

const char WORLD_CHANNEL_FLAG1[]		="!";				//世界频道
const char WORLD_CHANNEL_FLAG2[]		="！";				//世界频道

const char SYSTEM_CHANNEL_FLAG1[]		="";				//系统频道


#endif//GC_CHAT_CONST_H

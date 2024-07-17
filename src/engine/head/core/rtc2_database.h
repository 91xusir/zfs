#pragma once
/**
* @file rtc2_file.h 负责文件系统和流式文件操作.
* @author janhail
*/

//#include <winsock2.h>
#include <mysql.h>


using namespace std;



namespace rt2_core {


const short		RT_MYSQL_TYPE_FUNCTION		=	-1;
const short		RT_MYSQL_TYPE_DECIMAL		=	0;
const short		RT_MYSQL_TYPE_TINY			=	1;
const short		RT_MYSQL_TYPE_SHORT			=	2; 
const short		RT_MYSQL_TYPE_LONG			=	3;
const short		RT_MYSQL_TYPE_FLOAT			=	4;  
const short		RT_MYSQL_TYPE_DOUBLE		=	5;
const short		RT_MYSQL_TYPE_NULL			=	6;  
const short		RT_MYSQL_TYPE_TIMESTAMP		=	7;
const short		RT_MYSQL_TYPE_LONGLONG		=	8;
const short		RT_MYSQL_TYPE_INT24			=	9;
const short		RT_MYSQL_TYPE_DATE			=	10;   
const short		RT_MYSQL_TYPE_TIME			=	11;
const short		RT_MYSQL_TYPE_DATETIME		=	12; 
const short		RT_MYSQL_TYPE_YEAR			=	13;
const short		RT_MYSQL_TYPE_NEWDATE		=	14;
const short		RT_MYSQL_TYPE_ENUM			=	247;
const short		RT_MYSQL_TYPE_SET			=	248;
const short		RT_MYSQL_TYPE_TINY_BLOB		=	249;
const short		RT_MYSQL_TYPE_MEDIUM_BLOB	=	250;
const short		RT_MYSQL_TYPE_LONG_BLOB		=	251;
const short		RT_MYSQL_TYPE_BLOB			=	252;
const short		RT_MYSQL_TYPE_VAR_STRING	=	253;
const short		RT_MYSQL_TYPE_STRING		=	254;
const short		RT_MYSQL_TYPE_GEOMETRY		=	255;


struct RtDataField
{
	short				type;
	string				name;
	string				value;
	short				length;
};

typedef map<string,RtDataField> RtDataRow;
typedef vector<RtDataRow> RtDataTable;

/*-----------------------------------------------------------------------------
-     RtDatabase
-----------------------------------------------------------------------------*/

/** 流式文件读取基础类.
* 并不包含实际的读取，需要由其他类继承
*/
class RT2_CORE_API RtDatabase
{
public:
	RtDatabase(void);
	~RtDatabase(void);
};

/*-----------------------------------------------------------------------------
-     RtDatabaseMysql
-----------------------------------------------------------------------------*/
/*mysql操作类，封装了c语言相关的api，可实现基本的查询、插入、修改和删除动作*/
class RT2_CORE_API RtDatabaseMysql
{
protected:
 MYSQL *mysql; //代表一个到数据库的连接
private:
 string host; //连接的服务器
 string user; //用户名
 string password; //连接密码
 string initstatement;
 unsigned int port; //连接端口
 string db; //操作的数据库的名称
 MYSQL_RES *result; //操作的结果
 string query; //sql语句
 unsigned long num; //返回查询得到的结果数
 string error; //错误提示信息
 unsigned int debug; //是否显示调试信息
 RtDataRow info ; //查询语句返回一条结果
 RtDataTable arrInfo; //查询语句可能会返回多条结果
 vector<MYSQL_FIELD> fields; //返回查询结果的列
 void disPlayError();
public:
 RtDatabaseMysql(string host,string user,string password,string init="",unsigned int port=3306);// 构造函数
 RtDatabaseMysql(); //构造函数 
 void SetConnect(string host,string user,string password,string init="",unsigned int port=3306);//确定连接参数
 unsigned int DBConnect();//连接数据库
 unsigned int DBSelect(string db); //连接一个数据库
 void SetQuery(string q); //设定查询语句
 unsigned int DBQuery(); //查询数据库
 RtDataRow GetInfo(); //返回查询得到的一条结果
 RtDataTable GetArray(); //返回查询得到的结果
 string GetError(); //返回错误信息
 vector<MYSQL_FIELD> GetFields();//返回查询后的列值
 unsigned int InsertData(string table,RtDataRow *data); //向数据库中插入一条数据
 unsigned long GetLastID(); //返回最后一个自动增量的值
 unsigned long GetNum(); //返回一条sql语句影响的行数
 unsigned int UpdateData(string table,RtDataRow *data,string condition); //根据条件修改一条数据
 unsigned int DeleteData(string table,string condition); //根据条件删除数据
 ~RtDatabaseMysql();//析构函数
};



}
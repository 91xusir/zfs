#pragma once
/**
* @file rtc2_file.h �����ļ�ϵͳ����ʽ�ļ�����.
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

/** ��ʽ�ļ���ȡ������.
* ��������ʵ�ʵĶ�ȡ����Ҫ��������̳�
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
/*mysql�����࣬��װ��c������ص�api����ʵ�ֻ����Ĳ�ѯ�����롢�޸ĺ�ɾ������*/
class RT2_CORE_API RtDatabaseMysql
{
protected:
 MYSQL *mysql; //����һ�������ݿ������
private:
 string host; //���ӵķ�����
 string user; //�û���
 string password; //��������
 string initstatement;
 unsigned int port; //���Ӷ˿�
 string db; //���������ݿ������
 MYSQL_RES *result; //�����Ľ��
 string query; //sql���
 unsigned long num; //���ز�ѯ�õ��Ľ����
 string error; //������ʾ��Ϣ
 unsigned int debug; //�Ƿ���ʾ������Ϣ
 RtDataRow info ; //��ѯ��䷵��һ�����
 RtDataTable arrInfo; //��ѯ�����ܻ᷵�ض������
 vector<MYSQL_FIELD> fields; //���ز�ѯ�������
 void disPlayError();
public:
 RtDatabaseMysql(string host,string user,string password,string init="",unsigned int port=3306);// ���캯��
 RtDatabaseMysql(); //���캯�� 
 void SetConnect(string host,string user,string password,string init="",unsigned int port=3306);//ȷ�����Ӳ���
 unsigned int DBConnect();//�������ݿ�
 unsigned int DBSelect(string db); //����һ�����ݿ�
 void SetQuery(string q); //�趨��ѯ���
 unsigned int DBQuery(); //��ѯ���ݿ�
 RtDataRow GetInfo(); //���ز�ѯ�õ���һ�����
 RtDataTable GetArray(); //���ز�ѯ�õ��Ľ��
 string GetError(); //���ش�����Ϣ
 vector<MYSQL_FIELD> GetFields();//���ز�ѯ�����ֵ
 unsigned int InsertData(string table,RtDataRow *data); //�����ݿ��в���һ������
 unsigned long GetLastID(); //�������һ���Զ�������ֵ
 unsigned long GetNum(); //����һ��sql���Ӱ�������
 unsigned int UpdateData(string table,RtDataRow *data,string condition); //���������޸�һ������
 unsigned int DeleteData(string table,string condition); //��������ɾ������
 ~RtDatabaseMysql();//��������
};



}
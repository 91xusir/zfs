

#ifndef GAME_FILTER_H
#define GAME_FILTER_H

#include <string>
#include <list>
using std::string;
using std::list;



//过滤规则

class CFilterRule
{
public:
	virtual bool operator ()(const string& vrString1,const list<string>& vrList)=0;
private:
};

class CFilterNull:public CFilterRule
{
public:
	virtual bool operator ()(const string& vrString,const list<string>& vrList){return false;};
};

//严格相等
class CFilterEqual:public CFilterRule
{
public:
	virtual bool operator ()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString==(*It))
			{
				//找到一个相等的值//返回成功
				return true;
			}
		}

		//字符串同列表中所有的值都不相等//返回失败
		return false;
	};
};

//严格不等
class CFilterNotEqual:public CFilterRule
{
public:
	virtual bool operator()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString==(*It))
			{
				//有一个值相等//返回失败
				return false;
			}
		}

		//字符串同列表中所有的值都不相等//返回成功
		return true;
	}
};

//字符串包含列表中任意一字符串//true表示包含//false表示不包含
class CFilterInclude:public CFilterRule
{
public:
	virtual bool operator()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString.find((*It))!=string::npos)
			{//包含一个字符串//成功
				return true;
			}
		}

		//不包含任意一个字符串//失败
		return false;
	}
};

//字符串不包含列表中任意一字符串//true表示不包含//false表示包含
class CFilterNotInclude:public CFilterRule//vrMain不包含vrAssistant
{
public:
	virtual bool operator()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString.find((*It))!=string::npos)
			{//包含一个字符串//失败
				return false;
			}
		}

		//不包含任意一个字符串//成功
		return true;
	}
};







template<typename TRule>
class CFilter//过滤器
{
public:
	void Add(const string& vrAdd)
	{
		list<string>::iterator It;
		for(It=mFilter.begin();It!=mFilter.end();++It)
		{
			if( (*It)==vrAdd )
			{
				//已经有了//不用再添加
				return;
			}
		}
		mFilter.push_back(vrAdd);
	};
	void Del(const string& vrDel)
	{
		list<string>::iterator It;
		for(It=mFilter.begin();It!=mFilter.end();++It)
		{
			if(vrDel==(*It))
			{
				mFilter.erase(It);
				break;
			}
		}
	};

	list<string>& GetList()
	{
		return mFilter;
	};


	bool Filter(const string& vrString)//是否被过滤//true表示被过滤//false 表示不被过滤
	{
		return mRule(vrString,mFilter);
	}


private:
	list<string>	mFilter;
	TRule			mRule;
};


















#endif//#define GAME_FILTER_H

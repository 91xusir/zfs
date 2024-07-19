

#ifndef GAME_FILTER_H
#define GAME_FILTER_H

#include <string>
#include <list>
using std::string;
using std::list;



//���˹���

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

//�ϸ����
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
				//�ҵ�һ����ȵ�ֵ//���سɹ�
				return true;
			}
		}

		//�ַ���ͬ�б������е�ֵ�������//����ʧ��
		return false;
	};
};

//�ϸ񲻵�
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
				//��һ��ֵ���//����ʧ��
				return false;
			}
		}

		//�ַ���ͬ�б������е�ֵ�������//���سɹ�
		return true;
	}
};

//�ַ��������б�������һ�ַ���//true��ʾ����//false��ʾ������
class CFilterInclude:public CFilterRule
{
public:
	virtual bool operator()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString.find((*It))!=string::npos)
			{//����һ���ַ���//�ɹ�
				return true;
			}
		}

		//����������һ���ַ���//ʧ��
		return false;
	}
};

//�ַ����������б�������һ�ַ���//true��ʾ������//false��ʾ����
class CFilterNotInclude:public CFilterRule//vrMain������vrAssistant
{
public:
	virtual bool operator()(const string& vrString,const list<string>& vrList)
	{
		list<string>::const_iterator It;
		for(It=vrList.begin();It!=vrList.end();++It)
		{
			if(vrString.find((*It))!=string::npos)
			{//����һ���ַ���//ʧ��
				return false;
			}
		}

		//����������һ���ַ���//�ɹ�
		return true;
	}
};







template<typename TRule>
class CFilter//������
{
public:
	void Add(const string& vrAdd)
	{
		list<string>::iterator It;
		for(It=mFilter.begin();It!=mFilter.end();++It)
		{
			if( (*It)==vrAdd )
			{
				//�Ѿ�����//���������
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


	bool Filter(const string& vrString)//�Ƿ񱻹���//true��ʾ������//false ��ʾ��������
	{
		return mRule(vrString,mFilter);
	}


private:
	list<string>	mFilter;
	TRule			mRule;
};


















#endif//#define GAME_FILTER_H

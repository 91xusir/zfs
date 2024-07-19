

#ifndef UI_PROJECT_H 
#define UI_PROJECT_H 

#include "gc_include.h"
#include <map>


#define MAKE_SHORTCUT(key, alt, ctrl, shift) (DWORD)((key&0x0000FFFF)|((alt&0x01)<<16)|((ctrl&0x01)<<17)|((shift&0x01)<<18))

const char UI_CHAT_DIRECT_INPUT_PROJECT[]="DIRECT_CHAT";//直接聊天
const char UI_CHAT_ENTER_INPUT_PROJECT[]="ENTER_KEY_TO_CHAT";//enter键聊天

const char UI_CHAT_NORMAL_SHOW_PROJECT[]="NORMAL_SHOW";//正常的显示
const char UI_CHAT_TRANSPARENT_SHOW_PROJECT[]="TRANSPARENT_SHOW";//透明的显示



//----------------------------------------
//内部使用的类
//ui的显示配置
class CUIShowProject
{
public:
	CUIShowProject(){};
	virtual ~CUIShowProject(){};
public:
	static RtwImage::Ptr		mpBackGround[MAX_CHANNEL_CHATBOX];
	static RtwImage::Ptr		mpBorder[MAX_CHANNEL_CHATBOX];
};



class CUINormalShowProject:public CUIShowProject
{
public:
	CUINormalShowProject();//构造//初始化
	virtual ~CUINormalShowProject(){};

};


class CUITransparentShowProject:public CUIShowProject
{
public:
	CUITransparentShowProject();//构造//初始化
	~CUITransparentShowProject(){};
};




//ui的配置方案
class CUIProject
{
public:
	CUIProject(){};
	virtual ~CUIProject(){};

	void KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift);//按键按下
	
	virtual void KeyChar(int vChar)=0;//字符信息

protected:
	void Shortcut(bool vAlt,bool vCtrl,bool vShift); //设置快捷
	void ShortcutHint(bool vAlt,bool vCtrl,bool vShift);//hint的设置

private:
	virtual bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)=0;//按键按下//true截获//false不截获
	std::map<DWORD, RtwEventDispatcher> mShortcutList; // 快捷输入

};






//方案一//直接输入
class CUIProject1:public CUIProject
{
public:
	CUIProject1();
	virtual ~CUIProject1(){};

	virtual void KeyChar(int vChar);//字符信息

private:
	virtual  bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift);//按键按下
};




//方案二//回车输入
class CUIProject2:public CUIProject
{
public:
	CUIProject2();
	virtual ~CUIProject2(){};

	virtual void KeyChar(int vChar);//字符信息
private:
	virtual  bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift);//按键按下
};


#endif//UI_PROJECT_H 

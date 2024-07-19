

#ifndef UI_PROJECT_H 
#define UI_PROJECT_H 

#include "gc_include.h"
#include <map>


#define MAKE_SHORTCUT(key, alt, ctrl, shift) (DWORD)((key&0x0000FFFF)|((alt&0x01)<<16)|((ctrl&0x01)<<17)|((shift&0x01)<<18))

const char UI_CHAT_DIRECT_INPUT_PROJECT[]="DIRECT_CHAT";//ֱ������
const char UI_CHAT_ENTER_INPUT_PROJECT[]="ENTER_KEY_TO_CHAT";//enter������

const char UI_CHAT_NORMAL_SHOW_PROJECT[]="NORMAL_SHOW";//��������ʾ
const char UI_CHAT_TRANSPARENT_SHOW_PROJECT[]="TRANSPARENT_SHOW";//͸������ʾ



//----------------------------------------
//�ڲ�ʹ�õ���
//ui����ʾ����
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
	CUINormalShowProject();//����//��ʼ��
	virtual ~CUINormalShowProject(){};

};


class CUITransparentShowProject:public CUIShowProject
{
public:
	CUITransparentShowProject();//����//��ʼ��
	~CUITransparentShowProject(){};
};




//ui�����÷���
class CUIProject
{
public:
	CUIProject(){};
	virtual ~CUIProject(){};

	void KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift);//��������
	
	virtual void KeyChar(int vChar)=0;//�ַ���Ϣ

protected:
	void Shortcut(bool vAlt,bool vCtrl,bool vShift); //���ÿ��
	void ShortcutHint(bool vAlt,bool vCtrl,bool vShift);//hint������

private:
	virtual bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)=0;//��������//true�ػ�//false���ػ�
	std::map<DWORD, RtwEventDispatcher> mShortcutList; // �������

};






//����һ//ֱ������
class CUIProject1:public CUIProject
{
public:
	CUIProject1();
	virtual ~CUIProject1(){};

	virtual void KeyChar(int vChar);//�ַ���Ϣ

private:
	virtual  bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift);//��������
};




//������//�س�����
class CUIProject2:public CUIProject
{
public:
	CUIProject2();
	virtual ~CUIProject2(){};

	virtual void KeyChar(int vChar);//�ַ���Ϣ
private:
	virtual  bool Capture(int vKey,bool vAlt,bool vCtrl,bool vShift);//��������
};


#endif//UI_PROJECT_H 


#ifndef GC_EDIT_CHAT_H
#define GC_EDIT_CHAT_H


#include "gc_chat_assistant.h"
#include "gc_doskeywithui.h"



class CEditChat
{
public:
	CEditChat();
	~CEditChat(){};

	
	void KeyDownEvent(int key);
	void KeyCharDownEvent();
	void ReplacePrivateName(string& vrName);
	void AddName(string& vrAddName);
	void AddText(string& vrAddText);
	CDoskeyWithUI GetmDoskey(){return mDoskey;}
	void DoskeySelect()
	{
		mDoskey.UserInterfaceSelect();
		mName=mDoskey.Get();
		Show();
		return;
	};
	void AddHyperLinkText(EChildHyperLinkType eChildType, DWORD itemTag, DWORD itemId, DWORD playerID, const std::string &strName, RtwPixel color);
	bool Send(char vMessage[]);
	void Receive(SChannelAtom* vpChannel,long vID,char vName[],char vMessage[]);
	std::string GetTestindex(int index);
	void Change();//输入框文字发生改变
	void Setlastindex(int index){ mlastindex=index; }
	//tim.yang do client message
	bool DoClientCommand(std::string message);
	std::list< std::string >    mTextlist;
private:
	void DealText();
	bool ParseText();
	void Show();

	CDoskeyWithUI				mDoskey;
	int							mlastindex;     //上一次信息的记数
	string						mLastText;		//上一次的文本信息
	
	string						mName;			//发送给谁的名字[如果有名字]
	string						mText;			//文本信息的正文
};


#endif//GC_EDIT_CHAT_H

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
	void Change();//��������ַ����ı�
	void Setlastindex(int index){ mlastindex=index; }
	//tim.yang do client message
	bool DoClientCommand(std::string message);
	std::list< std::string >    mTextlist;
private:
	void DealText();
	bool ParseText();
	void Show();

	CDoskeyWithUI				mDoskey;
	int							mlastindex;     //��һ����Ϣ�ļ���
	string						mLastText;		//��һ�ε��ı���Ϣ
	
	string						mName;			//���͸�˭������[���������]
	string						mText;			//�ı���Ϣ������
};


#endif//GC_EDIT_CHAT_H
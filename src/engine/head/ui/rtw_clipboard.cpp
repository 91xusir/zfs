#include "ui/rtw_ui.h"

bool CUiClipboardImpl_Windows::Copy(const std::string& str)
{
	if(str.empty())
	{
		return false;//û������
	}

	HANDLE hText;
	LPTSTR pText;

	hText=GlobalAlloc( GHND,str.size()+1);// ������ƶ��ͳ�ʼ��Ϊ����ڴ�
	if (hText==NULL) 
	{
		//�޷������ڴ�
		return false;
	}

	pText=(LPTSTR)GlobalLock(hText);
	if(pText==NULL)
	{//�޷�����
		GlobalFree(hText);
		return false;
	}

	if(lstrcpy(pText,LPCSTR(str.c_str()))==NULL)
	{//���󿽱�ʧ��
		GlobalUnlock(hText);
		GlobalFree(hText);
		return false;
	}

	GlobalUnlock(hText);

	if(!OpenClipboard(NULL))
	{//�򿪼�����ʧ��
		GlobalFree(hText);
		return false;
	}

	if(!EmptyClipboard())
	{//��ռ�����ʧ��
		GlobalFree(hText);
		return false;
	}

	if(SetClipboardData(CF_TEXT,hText)==NULL)
	{//���ü���������ʧ��
		GlobalFree(hText);
		return false;
	}

	if(!CloseClipboard())
	{//�رռ�����ʧ��
		return false;
	}
	return true;
};

bool CUiClipboardImpl_Windows::Paste(std::string& outStr)
{
	HANDLE hText;
	LPSTR pText;

	if(!IsClipboardFormatAvailable(CF_TEXT))
	{//��������û��text 
		return false;
	}

	if(!OpenClipboard(NULL))
	{//�򿪼�����ʧ��
		return false;
	}

	hText=GetClipboardData(CF_TEXT);

	if(hText==NULL)
	{//ȡ������ʧ��
		CloseClipboard();
		return false;
	}

	pText=(LPSTR)GlobalLock(hText);
	if(pText==NULL)
	{//lockʧ��
		CloseClipboard();
		return false;
	}

	outStr=pText;//�õ�����
	GlobalUnlock(hText);//unlock
	CloseClipboard();//�رռ�����
	return true;
}

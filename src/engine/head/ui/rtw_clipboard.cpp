#include "ui/rtw_ui.h"

bool CUiClipboardImpl_Windows::Copy(const std::string& str)
{
	if(str.empty())
	{
		return false;//没有内容
	}

	HANDLE hText;
	LPTSTR pText;

	hText=GlobalAlloc( GHND,str.size()+1);// 申请可移动和初始化为零的内存
	if (hText==NULL) 
	{
		//无法分配内存
		return false;
	}

	pText=(LPTSTR)GlobalLock(hText);
	if(pText==NULL)
	{//无法锁定
		GlobalFree(hText);
		return false;
	}

	if(lstrcpy(pText,LPCSTR(str.c_str()))==NULL)
	{//错误拷贝失败
		GlobalUnlock(hText);
		GlobalFree(hText);
		return false;
	}

	GlobalUnlock(hText);

	if(!OpenClipboard(NULL))
	{//打开剪贴板失败
		GlobalFree(hText);
		return false;
	}

	if(!EmptyClipboard())
	{//清空剪贴板失败
		GlobalFree(hText);
		return false;
	}

	if(SetClipboardData(CF_TEXT,hText)==NULL)
	{//设置剪贴板数据失败
		GlobalFree(hText);
		return false;
	}

	if(!CloseClipboard())
	{//关闭剪贴板失败
		return false;
	}
	return true;
};

bool CUiClipboardImpl_Windows::Paste(std::string& outStr)
{
	HANDLE hText;
	LPSTR pText;

	if(!IsClipboardFormatAvailable(CF_TEXT))
	{//剪贴板中没有text 
		return false;
	}

	if(!OpenClipboard(NULL))
	{//打开剪贴板失败
		return false;
	}

	hText=GetClipboardData(CF_TEXT);

	if(hText==NULL)
	{//取出数据失败
		CloseClipboard();
		return false;
	}

	pText=(LPSTR)GlobalLock(hText);
	if(pText==NULL)
	{//lock失败
		CloseClipboard();
		return false;
	}

	outStr=pText;//得到文字
	GlobalUnlock(hText);//unlock
	CloseClipboard();//关闭剪贴板
	return true;
}

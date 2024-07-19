#ifndef _RTUI_CLIPBOARD_H
#define _RTUI_CLIPBOARD_H

namespace ui
{
	class CClipboard
	{
	public:
		CClipboard() { }
		virtual ~CClipboard() { }

	public:
		virtual bool Copy(const std::string& str) = 0;
		virtual bool Paste(std::string& outStr) = 0;
	};

	class CUiClipboardImpl_Windows : public CClipboard
	{
	public:
		CUiClipboardImpl_Windows() { }
		virtual ~CUiClipboardImpl_Windows() { }

	public:
		virtual bool Copy(const std::string& str);
		virtual bool Paste(std::string& outStr);
	};
} 
#endif 

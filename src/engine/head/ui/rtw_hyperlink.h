#ifndef _RTUI_HYPER_LINK_H
#define _RTUI_HYPER_LINK_H

namespace ui
{
	// begin rtui_struct.h [3/17/2009 tooth.shi]
	// ³¬Á´½Ó
	class RtwWidget;
	class RtwHyperlink
	{
		RT_DECLARE_DYNCREATE(RtwHyperlink, RtwWidget, 0, "ui")
	public:
		RtwHyperlink()
		{
			Reset();
		}

		RtwHyperlink(EHyperLinkType _Type, const std::string& _Text)
		{
			Type = _Type;
			Text = _Text;
		}

		~RtwHyperlink(){}
	public:
		void Reset()
		{
			Type = hlinkInvalid;
			Text = "";
		}

		bool FromString(const std::string& str);
		std::string ToString();
		// end rtui_struct.h [3/17/2009 tooth.shi]

		std::string GetURL();
	public:
		EHyperLinkType  Type;
		std::string     Text;
		std::string		m_sUrl;
	};
}
#endif
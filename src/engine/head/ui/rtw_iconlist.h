#ifndef _RTUI_ICONLIST_H
#define _RTUI_ICONLIST_H

namespace ui
{
	class RtwIconList
	{
	public:
		RtwIconList(){}
		~RtwIconList(){}

		char* GetSelectedKey();
	protected:
	public:
		RtwEventDispatcher EvSelect;

	};
}
#endif

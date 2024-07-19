#ifndef _RTUI_EVENT_H
#define _RTUI_EVENT_H

namespace ui 
{
	class RtwWidget;

	enum EUiButtonBits
	{
		buttonLeft		= 1,
		buttonRight		= 2,
		buttonMiddle	= 4,
	};

	enum EUiVirtualKey
	{	
		vkEscape		= 0x1B,
		vkBack			= 8,
		vkTab			= 9,
		vkEnter			= 13,
		vkShift			= 16,
		vkCtrl			= 17,
		vkSpace			= 32,
		vkEnd			= 35,
		vkHome			= 36,
		vkLeft			= 37,
		vkUp			= 38,
		vkRight			= 39,
		vkDown			= 40,
		vkDelete		= 46,	
		vkPageUp        = 0x21,
		vkPageDown      = 0x22
	};

	enum EUiEventType
	{
		weNone,
		etKeyDown,
		etKeyUp,
		etKeyChar,
		weMouseDown,
		etMouseUp,
		weMouseClick,
		etMouseDClick,
		etMouseMove,
		etMouseEnter,
		etMouseLeave,
		etMouseWheel,
		//etDragBegin,
		//etDragEnd,
		//etDragEnter,
		//etDragLeave,
		//etDrag,
		//etDragOut,
		etDragIn,
		etDragNull,      // Drag到一个位置
		//etDragCancel,   // 取消Drag
		etShow,
		etHide,
		weFocusIn,            // 激活
		etUnFocus,          // 反激活
		etEnable,
		etDisable,
		etMove,
		etOffset,           // 大小没有变化，仅仅位置变化
		etResize,
		etCreate,
		etClose,
		etScroll,
		etVScroll,
		etHScroll,
		etSelect,
		etDropDown,         // ComboBox 按下拉按钮
		weUpdateText,       // EditBox,ComboBox更新文本
		etHint,             // 显示hint前
		etSetCursor,
		etDraw,
		etMenuSelect,
		etHyperLink,
		etButtonPressed,    //控件被鼠标左健选中，但是没有右健释放的状态。
		etPostDraw,
		etInputChar,
		etLast
	};

	struct RtwEventDelegate
	{
		int type;
		bool cancle; // if set true, we will stop the event travell
		bool del_me; // if set true, we will delete this delegate
		void* param1;
		union
		{
			struct Mouse
			{
				int button;
				int x;
				int y;
			} mouse;

			struct MouseWheel
			{
				int delta;
			} mouseWheel;

			struct Key
			{
				int code;
			} key;

			struct DragIn
			{
				RtwWidget* pDragOutWidget;
			} dragIn;

			struct MenuSelect
			{
				int index;
			} menuSelect;
			struct Offset
			{
				int width;
				int height;
			} offset;

			struct Move
			{
				int OrigFrame_Left;
				int OrigFrame_Right;
				int OrigFrame_Top;
				int OrigFrame_Bottom;
			} move;

			struct Scroll
			{
				int OrigValue;
			} scroll;

			struct HyperLink
			{
				SHyperLink* pHLink;
			} hyperLink;


			struct Activate
			{
				RtwWidget* widget;
			} activate;

			struct Action
			{
				const char* descriptor;
			}action;

			struct Focus
			{
				RtwWidget* widget;
			}focus;

		};

		RtwEventDelegate()
		{
			type = weNone;
			cancle = false;
			del_me = false;
			param1 = NULL;
			scroll.OrigValue = 0;
		}

		RtwEventDelegate(int t)
		{
			type = t;
			cancle = false;
			del_me = false;		
			scroll.OrigValue = 0;
		}
		void* GetUser1() { return param1; }
		void  SetUser1(void* v) { param1 = v; }
	};

	class RtwEventDispatcher
	{
	private:
		void Dispatch(RtwWidget* sender, RtwEventDelegate* e);

	public:
		RtwEventDispatcher::RtwEventDispatcher()
		{
			Clear();
		}

		void operator () (RtwWidget* sender, const RtwEventDelegate& e)
		{
			if (!m_delegates.empty())
			{
				Dispatch(sender, (RtwEventDelegate*)&e);
			}
		}

		operator bool ()
		{
			return !m_delegates.empty();
		}

		RtwEventDispatcher& operator += (const CUiDelegate& delegate)
		{
			if (m_delegates.size() > 200)
				CHECK(0); // @@@@@

			foreach(std::list<CUiDelegate>, dele, m_delegates)
			{
				if (delegate.IsEqual(*dele))
					return *this;
			}

			m_delegates.push_back(delegate);
			param.param1 = delegate.m_param;
			return *this;
		}

//add = from += by tooth.shi
		RtwEventDispatcher& operator = (const CUiDelegate& delegate)
		{
			if (m_delegates.size() > 200)
				CHECK(0); // @@@@@

			foreach(std::list<CUiDelegate>, dele, m_delegates)
			{
				if (delegate.IsEqual(*dele))
					return *this;
			}

			m_delegates.push_back(delegate);
			param.param1 = delegate.m_param;
			return *this;
		}


		RtwEventDispatcher& operator -= (const CUiDelegate& delegate)
		{
			std::list<CUiDelegate>::iterator dele = m_delegates.begin();
			for(; dele != m_delegates.end() ;)
			{
				if (!m_delegates.empty())
				{
					if (delegate.IsEqual(*dele))
					{
						dele = m_delegates.erase(dele);
						param.param1 = 0;
					}
					else
						++dele;
				}
			}

			return *this;
		}

		void Reset()
		{
			m_delegates.clear();
			param.param1 = 0;
		}

		// begin rtui_event.h [3/15/2009 tooth.shi]
		void ClearAndSetDelegate(const CUiDelegate& delegate)
		{
			m_delegates.clear();
			m_delegates.push_back(delegate);
		}
		// end rtui_event.h [3/15/2009 tooth.shi]

		//RtwEventDispatcher& operator = (const CUiDelegate& delegate)
		//{
		//	m_delegates.clear();
		//	m_delegates.push_back(delegate);
		//	return *this;
		//}
		// add [3/18/2009 tooth.shi]
		void Clear(){	m_delegates.clear(); param.param1 = 0; }
		void* m_param1;

		std::list<CUiDelegate> m_delegates;
		RtwEventDelegate	param;
	};


} // namespace ui



#endif // _RTUI_EVENT_H

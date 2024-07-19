#ifndef _RTUI_CHATBOX_H
#define _RTUI_CHATBOX_H

namespace ui 
{
	class RtwChatBox : public RtwTextBox
	{
	public:
		RtwChatBox();
		RtwChatBox(const RtwChatBox &object);
		virtual ~RtwChatBox();

		enum piecetype
		{
			ptPrefix	= 1,
			ptText		= 2,
			ptImage		= 3,
			ptSeparator	= 4,
			ptBackslash	= 5,
			ptName		= 6,
		};

		struct Item
		{
			std::string text;
			char*		sender;
		};

		struct Piece 
		{
			piecetype type;
		};

		Item*	GetSelectItem();
		Piece*	GetSelectPiece();
		void	ClearByTime();
		void	SetFilter(int filter);
		void	SetClearTime(DWORD time);
		std::string GetLast(int iMask);
/*建议使用GetSelectedText替换GetLast函数*/
//		std::string GetSelectedText() const{return m_curItem?m_curItem->text:"";};

		int GetScrollnum();
		int GetScrollRange();
		int GetScrollPosition();
		void SetScrollnum(int num);
		void SetScrollPosition(int pos);
		void Add(int mask, DWORD color,const char* description,char* fromname,char* message,
			long vFromRegionID=0,bool ischange=true);	//heten  添加换行判定与玩家ID
		void WDScroll(DWORD delta, int nStartCount, int &nResultCount);		//heten
		bool InsertChatMessage(const std::string& channel,const std::string& sendname,
			const std::string& text, const RtwPixel color = RtwPixel(0xffffffff), 
			int indexBefore = -1, bool bRefresh = true, void* param = NULL);
	public:
		RtwEventDispatcher EvLSelect;	//左键选择
		RtwEventDispatcher EvRSelect;	//右键选额
		RtwEventDispatcher EvScroll;

	private:
		int m_position;
		int m_range;
		int m_Scrollnum;
		Item*  m_curItem;
		Piece* m_curPiece;
		DWORD  m_ClearSystime;
		DWORD  m_currentTime;
		//char   m_buf[128];
		CM_MEMDEF(m_buf, 128)
	};
} 
#endif 
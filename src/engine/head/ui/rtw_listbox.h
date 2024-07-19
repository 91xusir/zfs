#ifndef _RTUI_LISTBOX_H
#define _RTUI_LISTBOX_H

//********************************************************************
//	created:	2010.05.06 10:00
//	filename: 	rtw_listbox.h
//	author:		ldr123
//	purpose:	列表框
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	class RtwLabel;
	class RtwListBox : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwListBox, RtwWidget, 0, "ui")
		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	public:
		struct SItem
		{
			RtwLabel* pWidget;

			SItem() : pWidget(NULL)	{}

			SItem(const SItem& Other) : pWidget(NULL)
			{
				if (Other.pWidget)
				{
					pWidget = Other.pWidget;
					pWidget->grab();
				}
			}

			~SItem()
			{
				DROP_RTUI_OBJECT(pWidget);
			}

			const SItem& operator=(const SItem& Other)
			{
				DROP_RTUI_OBJECT(pWidget);
				if (Other.pWidget)
				{
					pWidget = Other.pWidget;
					pWidget->grab();
				}
				return *this;
			}

			bool CloneFrom(const SItem* pItem)
			{
				if (pItem->pWidget)
				{
					if (!pWidget)
					{
						pWidget = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
						UI_ENSURE_B(pWidget);
					}
					pWidget->CloneFrom(pItem->pWidget);
				}
				else 
				{
					DROP_RTUI_OBJECT(pWidget);
				}
				return true;
			}
		};

		//行
		struct SLine
		{
			bool	bDraw;
			int		height;
			void *	param;
			int		PosY;
			std::vector<SItem> items;
		};

		struct SColumn
		{
			std::string	name;
			int			PosX;
			int			width;
			RtwButton* pBtnHeader;

			SColumn(const std::string& _name, const int _width)
				: name(_name)
				, width(_width),
				pBtnHeader(NULL){}

			SColumn(const SColumn& Other)
			{
				name		= Other.name;
				PosX		= Other.PosX;
				width		= Other.width;
				pBtnHeader = NULL;

				if (Other.pBtnHeader)
				{
					pBtnHeader = Other.pBtnHeader;
					pBtnHeader->grab();
				}
			}

			~SColumn()
			{
				DROP_RTUI_OBJECT(pBtnHeader);
			}

			const SColumn& operator=(const SColumn& Other)
			{
				name = Other.name;
				PosX = Other.PosX;
				width = Other.width;
				DROP_RTUI_OBJECT(pBtnHeader);
				if (Other.pBtnHeader)
				{
					pBtnHeader = Other.pBtnHeader;
					pBtnHeader->grab();
				}
				return *this;
			}
			bool CloneFrom(const SColumn* pCol)
			{
				name = pCol->name;
				PosX = pCol->PosX;
				width = pCol->width;
				if (pCol->pBtnHeader)
				{
					if (!pBtnHeader)
					{
						pBtnHeader = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
						UI_ENSURE_B(pBtnHeader);
					}
					pBtnHeader->CloneFrom(pCol->pBtnHeader);
				}
				else 
				{
					pBtnHeader = NULL;
				}
				return true;
			}
		};

	protected:
		RtwListBox();
		virtual ~RtwListBox();
	public:
		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		int		InsertColumn(const std::string& name, const int width, const RtwPixel color = RtwPixel(0xffffffff), int indexBefore = -1, bool bRefresh = true);
		bool	InsertLine(void* param = NULL, int indexBefore = -1, bool bRefresh = true);
		bool	InsertLine(const std::string& text, const RtwPixel color = RtwPixel(0xffffffff), int indexBefore = -1, bool bRefresh = true, void* param = NULL);
		bool	AddItem(int indexBefore = -1, bool bRefresh = true);
		bool	AddItem(const std::string& text, int indexBefore = -1, bool bRefresh = true);
		bool	SetLineItem(
									int line, int col, const std::string& text, const RtwPixel color = RtwPixel(0xffffffff),
									SUiTextAlignment align = SUiTextAlignment(alignNear, alignCenter)
							);
		bool	SetLineItemImage(int line,int col,RtwImage* pImage);

		bool	RemoveLine(int line, bool bRefresh = true);
		bool	RemoveCol(int col, bool bRefresh = true);
		void	RemoveAllLines(bool bRefresh = true);
		void	RemoveAllItems(bool bRefresh = true);
		void	RemoveAllColumns(bool bRefresh = true);
		void	SetSelect(int index = -1);
		void	SetSelectedItem(int index = -1);
		void	SetDrawHeader(bool bDraw, bool bRefresh = true);
		void	SetDrawScrollBarV(bool bDraw, bool bRefresh = true);
		void	SetAutoResize(bool bAutoResize, bool bRefresh = true);
		void	SetMultiSelect(bool bMultiSelect);
		void	SetHoverSelect(bool bHoverSelect);
		void	SetColumnWidth(int index, int width, bool bRefresh = true);
		void	SetLineColor(int index = -1, const RtwPixel color = RtwPixel(0xffffffff));
		void	SetLineParam(int index, void* param);
		void	SetbScroll(bool bRefresh){ m_bScroll=bRefresh; }
		int		getSelectLine() { return m_SelectLines.empty() ? -1 : m_SelectLines.front(); }
		int		GetSelectedItem() { return m_SelectLines.empty() ? -1 : m_SelectLines.front(); }
		std::list<int> getAllSelectLines() { return m_SelectLines; }
		int		getLineCount() { return (int)m_Lines.size(); }
		int		getColCount() { return (int)m_Columns.size(); }
		int		GetItemHeight(int index = 0);
		int		getLineHeight(int index);
		int		getColWidth(int index);
		std::string getLineItemText(int line, int col);
		int		getLineItemIndex(std::string mm);
		bool	getHoverSelect() { return m_bHoverSelect; }
		bool	getMultiSelect() { return m_bMultiSelect; }
		bool	getDrawHeader() { return m_bDrawHeader; }
		RtwButton* getHeaderButton(int index);
		int		getFirstViewLine() { return m_ScrollLineOffset; }
		int		getLastViewLine() { return m_ScrollLineLast; }
		void	SetLastViewLine(int line); 
		void	SetFirstViewLine(int line);
		void*	getLineParam(int index);
		SLine*	getSelectLineItem() { return _getLine(getSelectLine()); }
		SLine*	GetLine(int index) { return (index >= 0 && index < (int)m_Lines.size()) ? &(m_Lines[index]) : NULL; }

		std::string getLastLineText() { return getLineItemText(getLineCount() - 1, 0); }

		void	MoveResize(RtwRect& rect){	Move(rect);	}
		int		GetItemCount()	{	return getLineCount();	}
		RtwButton* GetColumnHeader(int index) { return getHeaderButton(index); }
		void	SetItemText(
									int line, int col, const std::string& text, const RtwPixel color = RtwPixel(0xffffffff),
									SUiTextAlignment align = SUiTextAlignment(alignNear, alignCenter)
							) 
		{ 
			SetLineItem(line,col,text, color, align);
		}

		int GetTrackItem() { return	GetSelectedItem(); }
		std::string GetItemText(int index, int col) { return getLineItemText(index,col); }
		int IndexFromPoint(int x,int y, bool bIndex);
		void* GetItemData(int index) {	return getLineParam(index);	}
		void SetItemData(int index, void* param);
		void SetItemIcon(int index, RtwImage*);
		
		
		void SetItemColor(int index,RtwPixel color);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove;
		static const int m_HeaderHeight = 20;
		void DefaultKeyChar(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		int GetLineFromPosition(int nPointY){return _GetLineFromPosition(nPointY);}
		void SetLineHeight(int nLineHeight) {m_nLineHeight = nLineHeight;}
		bool SetLineNameByParam(void *pParam,string name,int col = 0);
		bool RemoveLineByParam(void *pParam,bool refresh = true);
		bool SetSelectLineByParam(DWORD pParam);
		void ClearSelectLine() { _ClearSelectLines(); }
	protected:
		virtual void CalcClient();

		void OnLButtonDown_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnRButtonDown_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnLDClick(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnHint(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseMove_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseWheel_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_Header(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		void CalcItemsPos();

		SLine*		_getLine(int index) { return (index >= 0 && index < (int)m_Lines.size()) ? &(m_Lines[index]) : NULL; }
		SColumn*	_getColumn(int index) { return (index >= 0 && index < (int)m_Columns.size()) ? &(m_Columns[index]) : NULL; }
		bool		_CreateItem(SItem& item);
		bool		_DestroyItem(SItem& item);

		int			_GetLineFromPosition(int PointY);
		int			_GetColFromPosition(int PointX);
		RtwRect		_GetLineRect(int line);

		void		_AddSelectLine(int index);
		void		_RemoveSelectLine(int index);
		bool		_FindInSelectLines(int index);
		void		_ClearSelectLines();
	private:
		void _ResetSelf();

		bool	m_bDrawHeader;
		bool	m_bDrawScrollV;            // 是否显示垂直滚动条
		std::list<int> m_SelectLines;
		int		m_ScrollLineOffset;         // 偏移的行数
		int		m_ScrollLineLast;           // 可见的最后一行

		int		m_nLineHeight;

		bool	m_bMultiSelect;
		bool	m_bHoverSelect;            // 是否鼠标悬停就选中
		bool	m_bAutoResize;             // 自动调整大小
		bool	m_bScroll;				//是否有滚动条
		std::vector<SLine> m_Lines;
		std::vector<SColumn> m_Columns;

		RtwRect m_rcSbv;
	public:
		RtwEventDispatcher EvSelect;
		RtwEventDispatcher EvLClickHeader;
		RtwEventDispatcher EvRClickItem;
		RtwEventDispatcher EvMoveItem;
		RtwEventDispatcher EvHintItem;
		RtwEventDispatcher EvLDClick;
	};
} 
#endif 

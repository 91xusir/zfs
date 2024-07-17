#ifndef _RTUI_MENU_H
#define _RTUI_MENU_H

namespace ui 
{
	class RtwPopupMenu : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwPopupMenu, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

	protected:
		RtwPopupMenu();
		virtual ~RtwPopupMenu();

		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		bool AddMenuItem(const std::string& text, bool bEnable = true, void* param = NULL, const std::string& type = "");
		int FindMenuItemIndex(const std::string& text);
		int GetMenuItemsCount(){return m_pList->getLineCount();};
		bool RemoveMenuItem(const std::string& text);
		bool RemoveMenuItem(int index);
		void RemoveAll();

		void EnableIndex(int index, bool bEnable = true);
		void EnableIndex(const std::string &itemName, bool bEnable = true);

		void Popup(int x,int y){const SPoint pos(x,y); Move(pos);};
		void AddItem(const std::string& text,void* lparam = NULL, void* param = NULL, const std::string& type = "");
		int	 GetLastSelectItem();
		int	 GetItemData(int select);
		std::string GetItemUserString(int select);
		std::string	GetItemText(int select){	return "";	}
		void RemoveAllItems();
		int GetID() {return m_id;}
		void SetID(long id) {m_id = id;}
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove;
		static const int m_DefaultWidth = 100;
		std::string getMenuText(int index);
	protected:
		virtual void CalcClient();

	private:
		void _ResetSelf();

		RtwListBox*         m_pList;
		std::list<bool>     m_lstEnable;
		std::list<int>		m_lstDisableList;
		int                 m_MenuTextColIndex;          // ListBox中显示text的列的index
		int					m_id;		//heten  保存玩家ID
		EXT_SPACE::unordered_map<int,std::string>  m_itemString;

		// Events
	public:
		void OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnLClick_List(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		RtwEventDispatcher EvMenuSelect;
		RtwEventDispatcher EvSelect;
	};


} // namespace ui



#endif // _RTUI_MENU_H

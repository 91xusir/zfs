#ifndef _RTUI_COMBOBOX_H
#define _RTUI_COMBOBOX_H

namespace ui 
{
	class RtwComboBox : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwComboBox, RtwWidget, 0, "ui")
		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwComboBox();
		virtual ~RtwComboBox();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		virtual void SetText(const std::string& text);
		void SetEnableInput(bool bEnableInput);

		void AddItem(const std::string& text);
		void RemoveAllItems();

		bool	AddListItem(const std::string& text);
		void	SetLineParam(int index, void* param);
		void*	GetLineParam(int index);
		RtwListBox::SLine*	GetSelectLine();

		virtual std::string GetText() const;
		virtual std::string getText() const;
		std::string getSelectItem() { return m_pEdtText->GetText(); }
		bool getEnableInput() { return m_bEnableInput; }
		RtwListBox* getDropList() { return m_pLstDropDown; } 
		RtwTextBox* getEditBox() { return m_pEdtText; }
		RtwButton* getDropdownButton() { return m_pBtnArrow; }
		int	GetSelectedItem();
		void SetSelectedItem(int index = -1);
		bool SetLineNameByParam(void *pParam,string name,int col = 0);
		bool RemoveLineByParam(void *pParam,bool refresh = true);
		bool SetSelectLineByParam(DWORD pParam);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfGrabMouse | wfMouseMove;
		void SwapDropDownState();
		void HideDrop();
	protected:
		virtual void CalcClient();

		bool            m_bEnableInput;     // 是否允许输入
		bool            m_bDrop;            // list是否已经展开
		RtwTextBox*     m_pEdtText;
		RtwButton*      m_pBtnArrow;
		RtwListBox*     m_pLstDropDown;
		RtwImage*       m_NorImage;

		RtwRect           m_rcText;
		RtwRect           m_rcArrow;
		RtwRect           m_rcList;

	private:
		void _ResetSelf();

		void OnClick_Arrow(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_List(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnUnFocus_List(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	public:
		RtwEventDispatcher  EvUpdateText;
		RtwEventDispatcher  EvDropDown;
		RtwEventDispatcher  EvSelect;
	};
} 
#endif 

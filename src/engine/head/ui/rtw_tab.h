//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_TABCTRL_H
#define _RTUI_TABCTRL_H


namespace ui {

	class RtwTab : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwTab, RtwWidget, 0, "ui")

			friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

	public:
		struct SItem
		{
			CUiCheckButton* pBtn;
			RtwForm*        pForm;

			SItem()
			{
				pBtn = NULL;
				pForm = NULL;
			}
			SItem(const SItem& Other)
			{
				pBtn = NULL;
				if (Other.pBtn)
				{
					pBtn = Other.pBtn;
					pBtn->grab();
				}
				pForm = NULL;
				if (Other.pForm)
				{
					pForm = Other.pForm;
					pForm->grab();
				}
			}
			~SItem()
			{
				DROP_RTUI_OBJECT(pBtn);
				DROP_RTUI_OBJECT(pForm);
			}
			bool CloneFrom(const SItem* pItem)
			{
				if (pItem->pBtn)
				{
					if (!pBtn)
					{
						pBtn = (CUiCheckButton*)g_workspace.getWidgetFactory()->createWidget(wtCheckButton);
						UI_CHECK_B(pBtn);
					}
					pBtn->CloneFrom(pItem->pBtn);
				}
				else 
				{
					DROP_RTUI_OBJECT(pBtn);
				}

				if (pItem->pForm)
				{
					if (!pForm)
					{
						pForm = (RtwForm*)g_workspace.getWidgetFactory()->createWidget(wtForm);
						UI_CHECK_B(pForm);
					}
					pForm->CloneFrom(pItem->pForm);
				}
				else 
				{
					DROP_RTUI_OBJECT(pForm);
				}

				return true;
			}
			SItem& operator=(const SItem& Other)
			{
				DROP_RTUI_OBJECT(pBtn);
				if (Other.pBtn)
				{
					pBtn = Other.pBtn;
					pBtn->grab();
				}
				DROP_RTUI_OBJECT(pForm);
				if (Other.pForm)
				{
					pForm = Other.pForm;
					pForm->grab();
				}
				return *this;
			}
			void SetButton(CUiCheckButton* _pBtn)
			{
				DROP_RTUI_OBJECT(pBtn);
				if (_pBtn)
				{
					pBtn = _pBtn;
					pBtn->grab();
				}
			}
			void SetForm(RtwForm* _pFrm)
			{
				DROP_RTUI_OBJECT(pForm);
				if (_pFrm)
				{
					pForm = _pFrm;
					pForm->grab();
				}
			}
			CUiCheckButton* getButton() { return pBtn; }
			RtwForm* getForm() { return pForm; }

		};

	protected:
		RtwTab();
		virtual ~RtwTab();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		bool AddTabItem(CUiCheckButton* pBtn, RtwForm* pForm);
		bool RemoveTabItem(int index);
		void SetSelectedItem(int index);
		void SetCurrentSelect(int index);

		int GetSelectedItem() { return m_CurrentSelectItem; }
		int getCurrentSelectItem() { return m_CurrentSelectItem; }
		const RtwForm* getTabItemForm(int index);
		const RtwButton* getTabItemButton(int index);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove | wfInput | wfFocus;
		void OnLClick_Button(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	protected:
		virtual void CalcClient();

		std::vector<SItem> m_Items;
		int m_CurrentSelectItem;
	private:
		void _ResetSelf();
	public:
		RtwEventDispatcher EvChangeSelected;
	};
}
#endif
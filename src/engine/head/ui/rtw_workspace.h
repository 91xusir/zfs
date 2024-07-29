#ifndef _RTUI_DESKTOP_H
#define _RTUI_DESKTOP_H
#include <WinUser.h>

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_workspace.h
//	author:		ldr123
//	purpose:	uiϵͳ��Ҫ��Ϣ���߼�������
//  other:		�ֲ��ع����ע��
//********************************************************************

namespace ui 
{
	class IUiRenderDevice;
	class IUiSound;
	class CClipboard;
	class CUiThemeManager;
	class RtwXmlLoader;

	enum EMessageBoxResult
	{
		mbr_Ok,		//ȷ��
		mbr_Cancel	//ȡ��
	};

	class CUiDesktop
	{
		friend class CUiLayer;

	public:
		enum EUiDesktopState
		{
			dsSuspend,	//����
			dsRun,		//����
			dsDestroy	//�ͷ�
		};
	public:
		CUiDesktop();
		virtual ~CUiDesktop();

	public:
		bool Create(IUiRenderDevice* pRenderer, IUiImageFactory* pImageFactory, IUiFontManager* pFontManager);
		bool Destroy();
		void Run(DWORD dwDelta);
		void Render();
		void SetSound(IUiSound* pSound)
		{
			m_pSound = pSound;
		}
		void SetClipboard(CClipboard* pClipboard)
		{
			m_pClipboard = pClipboard;
		}
		void OnInvalidateDevice();
		void OnRestoreDevice();

		void RemoveAll(bool bIncludeSystemWidgets = false);

		bool FindLayer(DWORD LayerId, CUiLayer** ppLayer = NULL);
		bool FindLayer(const std::string& Name, CUiLayer** ppLayer = NULL);
		bool AddLayer(CUiLayer* pLayer, bool bAddToFront = false);
		bool RemoveLayer(DWORD LayerId);
		bool RemoveLayer(const std::string& Name);

		bool FindWidget(DWORD WidgetId, bool bRecursive = false, RtwWidget** ppWidget = NULL, CUiLayer** ppInLayer = NULL);
		bool FindWidget(const std::string& FullName, RtwWidget** ppWidget = NULL);
		bool AddWidget(RtwWidget* pWidget);
		bool RemoveWidget(DWORD WidgetId); // Always Recursive

		bool PickWidget(
							RtwWidget** ppWidget = NULL,
							CUiLayer** ppLayer = NULL,
							DWORD FlagMask = 0x00000000,
							UiLayerList* pExcludeLayers = NULL,
							CUiWidgetContainer* pExcludeWidgets = NULL
						);

		void SetFocusWidget(RtwWidget* pWidget);
		void SetHintWidget(RtwWidget* pWidget);

		void PrintWidgetTree();
		void PrintWidgetTree(const std::string& LayerName);

		void AddTopRenderWidget(RtwWidget* pWidget);
		void RemoveTopRenderWidget(DWORD WidgetId);
		bool FindInTopRenderContainer(DWORD WidgetId);

		void AddModalWidget(RtwWidget* pWidget);
		void RemoveModalWidget(DWORD WidgetId);
		bool FindInModalContainer(DWORD WidgetId);

		void AddGarbageWidget(RtwWidget* pWidget);
		void DoGarbageClear();

		void SetMouseDragWidget(RtwWidget* pWidget);
		void SetMouseHoverWidget(RtwWidget* pWidget);
		void ResetHintTargetWidget();
		void ResetHintText() { DROP_RTUI_OBJECT(m_pHintTargetWidget); }

		void ShowPopupMenu();
		void ShowPopupMenu(RtwPopupMenu* menu,void* xx,int x,int y);
		void HidePopupMenu();

		void SetDefaultFocusWidget(RtwWidget* pDefault); // ��û�н���ʱ���Ͱѽ������ڴ˴�����
		void SetDebugMode(bool bDebugMode) { m_bDebugMode = bDebugMode; }
		void SetRefreshMediaFrame(CUiMediaFrame* pFrame);

		void BringWidgetToTop(RtwWidget* pWidget);
		void BringWidgetToBottom(RtwWidget* pWidget);

		bool RegisterTexturePath(const std::string& TexturePath);
		bool RegisterXmlPath(const std::string& XmlPath);

		RtwWidget* getDefaultFocusWidget()
		{
			return m_pFocusWidget;
		}
	public:
		bool MouseDown(int button);
		bool MouseUp(int button);
		bool MouseDClick(int button);
		bool MouseMove(int x, int y);
		bool MouseWheel(int delta, int x, int y);
		bool KeyChar(int ch);
		bool KeyDown(int key);
		bool KeyUp(int key);

	public:
		void				SetMouseCapture( RtwWidget* pkWidget );
		EWidgetType			getWidgetTypeFromTypeName(const std::string& TypeName);
		EWidgetType			getWidgetTypeFromTypeShortName(const std::string& TypeShortName);
		RtwRect				getViewportRect();
		void				SetHintText(const std::string& HintText,bool b = true);
		void				AutoHintSize();
		void				SetHintSize(int x,int y);
		void				SetHintPosition(int x, int y);
		int					GetMousePosX();
		int					GetMousePosY();
		void				ShowHint(RtwWidget* pWidget,int x = 0);
		void				HideHint(int x);
		void				SetActiveWidget(RtwWidget* pWidget);
		RtwWidget*			LookupWidget(const std::string name);
		bool				Load(const char* xmlName);

		void				Clear()				{}
		void				AutoHintPosition()	{ SetHintPosition(0,0);	}
		RtwWidget*			GetDragClient()		{ return m_pMouseDragWidget;}
		RtwWidget*			GetDragServer()		{ return m_pMouseDragWidget;	}
		bool				IsMouseMonopolist()	{return false;}
		bool				GetLayer(int index)	{return false;}
		bool				GetMouseLDown()		{ return m_bMouseLDown;	}
		EUiDesktopState		getRunState() const	{ return m_State; }
		IUiRenderDevice*	getRenderDevice()	{ return m_pRenderDevice; }
		CUiWidgetFactory*	getWidgetFactory()	{ return m_pWidgetFactory; }
		IUiImageFactory*	getImageFactory()	{ return m_pImageFactory; }
		IUiFontManager*		getFontManager()	{ return m_pFontManager; }
		CUiThemeManager*	getThemeManager()	{ return m_pThemeManager; }
		IUiSound*			getSound()			{ return m_pSound; }
		CClipboard*			getClipboard()		{ return m_pClipboard; }
		const SPoint&		getMousePos()		{ return m_MousePos; }
		const SSize&		getMouseDelta()		{ return m_MouseDelta; }
		RtwWidget*			GetFocusWidget()	{ return m_pFocusWidget; }
		RtwWidget*			getFocusWidget()	{ return m_pFocusWidget; }
		RtwWidget*			getMouseCapture()	{ return m_pMouseCaptureWidget; }
		RtwWidget*			getMouseHover()		{ return m_pMouseHoverWidget; }
		RtwWidget*			getDefaultHint()	{ return m_pDefaultHint; }
		RtwWidget*			getDefaultHLWidget(){ return m_pDefaultHLWidget;}
		RtwXmlLoader*		getXmlLoader()		{ return m_pUiXmlLoader; }
		CUiLayer*			getSystemLayer()	{ return m_pSystemLayer; }
		bool				getDebugMode()		{ return m_bDebugMode; }
		RtwWidget*			GetMouseDragWidget(){return m_pMouseDragWidget;};		// add by chib to get capture state

		RtwMailBox*			getDefaultMessageBox()		{ return m_pDefaultMessageBox; }
		RtwChatInputBox*	getDefaultInputBox()		{ return m_pDefaultInputBox; }
		int					getDefaultScrollVWidth()	{ return 10; }
		int					getDefaultScrollHHeight()	{ return 10; }
		RtwPopupMenu*		getDefaultPopupMenu()		{ return m_pDefaultPopupMenu; }
		void				AdjustLayer(int x,int y)	{}
		void				CloseLayer(int index)		{}

		void						SetSysEditName(std::string name){}
		void						SetDragClient(RtwWidget* pWidget){ m_pMouseDragWidget = pWidget;	}
		void						SetDragServer(RtwWidget* pWidget){ SetMouseDragWidget(pWidget);	}
		SHyperLink_DefaultStyle&	getHyperLinkDefaultStyle()		{ return m_HyperLinkDefaultStyle; }
		bool						isKeyPressed(EUiVirtualKey vk)	{ return (GetKeyState(vk) & 0xFF00) != 0; }
		void						SetModeWnd(bool bFlag = false)	{ m_bModeWnd = bFlag; };			// ������������Χ [chib] [2006-10-24]
		void						SetDefaultPopupMenu(RtwPopupMenu* Popup){ m_pDefaultPopupMenu=Popup; } //max 
		std::list<std::string>&		getDefaultTexturePaths()		{ return m_DefaultTexturePaths; }
		std::list<std::string>&		getDefaultXmlPaths()			{ return m_DefaultXmlPaths; }
	private:
		bool _InitSystemWidgets();

		bool _ProcessXmlFile(RtsXmlDoc::NodePtr* pRoot);
		bool _ProcessXml_Loader(RtsXmlDoc::NodePtr* pNode);
		bool _ProcessXml_Config(RtsXmlDoc::NodePtr* pNode);

		bool _SplitWidgetName(const std::string& FullName, std::vector<std::string>& arrShortName);
		bool _ProcessModalWidget_MouseEvent();
	private:
		EUiDesktopState         m_State;

		IUiRenderDevice*        m_pRenderDevice;        // ��ȾDevice
		IUiImageFactory*        m_pImageFactory;        // Image����
		IUiFontManager*         m_pFontManager;         // �������
		IUiSound*               m_pSound;               // ����������
		CClipboard*             m_pClipboard;           // 

		CUiWidgetFactory*       m_pWidgetFactory;       // ���ڲ�������
		CUiThemeManager*        m_pThemeManager;        // �������

		CUiWidgetContainer*     m_pModalWidgetContainer;
		CUiWidgetContainer*     m_pDefaultWidgetContainer;
		UiLayerList             m_Layers;               // ����������Layer
		CUiWidgetContainer*     m_pTopRenderWidgetContainer;
		CUiLayer*               m_pSystemLayer;         // ϵͳLayer
		CUiWidgetContainer*     m_pGarbageContainer;    // ����Widgets(Ʃ�类������Child,���ǻ������������ã��ͷ������������)
		CUiMediaFrame*          m_pRefreshMediaFrame;   // ��Ҫ���µ�MediaFrame

		RtwWidget*              m_pDefaultHint;         // Ĭ�ϵ�HintWidget(TopRenderLayer)
		RtwWidget*              m_pDefaultHLWidget;     // Ĭ�ϵ�HighLightWidget(TopRenderLayer)  
 		RtwPopupMenu*           m_pDefaultPopupMenu;    // Ĭ�ϵ�PopupMenu(TopRenderLayer)
 		RtwMailBox*				m_pDefaultMessageBox;   // Ĭ�ϵ�MessageBox(TopRenderLayer)
 		RtwChatInputBox*        m_pDefaultInputBox;     // Ĭ�ϵ�InputBox(TopRenderLayer)

		RtwXmlLoader*           m_pUiXmlLoader;         // Xml�ļ�Loader

		Real                    m_rTransparent;         // �����͸����

		RtwWidget*              m_pFocusWidget;         // ����ͣ����Widget
		RtwWidget*              m_pMouseCaptureWidget;  // ������ڲ����Widget
		RtwWidget*              m_pMouseHoverWidget;    // ���������ͣ��Widget
		RtwWidget*              m_pMouseDragWidget;     // ��������϶���Widget
		RtwWidget*              m_pHintWidget;          // ��ʾ�����ϲ������Hint��Widget

		RtwWidget*              m_pDefaultFocusWidget;  // ��û�н���ʱ���Ͱѽ������ڴ˴�����
		RtwWidget*              m_pHintTargetWidget;    // ��ʾHintĿ���Widget

		SPoint                  m_MousePos;             // ���λ��
		SSize                   m_MouseDelta;           // ���ƫ��

		SHyperLink_DefaultStyle m_HyperLinkDefaultStyle;    // �����ӵ�Ĭ����ʾģʽ

		RtwEventDispatcher      m_MessageBox_Ok_Delegate;   // ���MessageBox��OK��ť�Ļص�����
		RtwEventDispatcher      m_InputBox_Ok_Delegate;     // ���InputBox��OK��ť�Ļص�����

		std::list<std::string>  m_DefaultTexturePaths;  // Ĭ�ϵ���ͼ·��
		std::list<std::string>  m_DefaultXmlPaths;      // Ĭ�ϵ�Xml�ļ�·��

		bool                    m_bDebugMode;           // �Ƿ��ڵ���ģʽ
		bool					m_bModeWnd;				//	����¼��Ƿ񴫵ݸ����� [chib] [2006-10-24]
		bool                    m_bMouseLDown;         // �������Ƿ񱻰���
		bool                    m_bMouseRDown;         // ����Ҽ��Ƿ񱻰���
	public:
		RtwEventDispatcher      EvMove;                 // Viewport�����仯ʱ���¼�

		/*
		2010 04 12���
		�洢�����б�,����escʱ,˳��رմ���
		*/
		//begin
	public:
		void InsertFocusWidget(RtwForm *pWidget);	//����
		void DeleteFocusWidget(RtwForm *pWidget);	//ɾ��
		void SetWidgetWhenNoFocus(RtwWidget *pWidget){m_pNoFocusWidget = pWidget;}
	private:
		std::list<RtwForm*>   m_lstFocusWidgetList;
		RtwWidget *			  m_pNoFocusWidget;
		//end

		/*
		2010 04 14���
		�����ļ��Ƿ����
		*/
		//begin
	public:
		void SetFontFileOk() {m_bFontFileOk = true;}
		bool GetFontFileOk() {return m_bFontFileOk;}
		std::string GetDefaultFontName() {return m_strDefaultFontName;}
		void SetDefaultFontName(const std::string &strDefaultFontName) {m_strDefaultFontName = strDefaultFontName;}
	private:
		bool m_bFontFileOk;
		std::string m_strDefaultFontName;
		//end
	};
} 
extern ui::CUiDesktop g_workspace;
#endif 

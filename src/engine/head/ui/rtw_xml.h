#ifndef _RTUI_XML_H
#define _RTUI_XML_H
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_xml.h
//	author:		ldr123
//	purpose:	ui系统通过xml加载控件的处理类
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	class IUiXmlSerialize
	{
	public:
		IUiXmlSerialize() { }
		virtual ~IUiXmlSerialize() { }

		virtual bool UiSerializeToXml(std::string& str, bool bLoad) = 0;
	};

	// Inherit 是一个特殊的属性
	// _Process_Attrib_打头的函数只能处理自身的属性，不能处理子Node，否则可能会创建Widget
	class RtwXmlLoader
	{
		friend class CUiTheme;

	public:
		RtwXmlLoader();
		virtual ~RtwXmlLoader();

	public:
		bool Reset();
		bool LoadFromFile(const std::string& FileName, RtwWidget* pParent = NULL, std::list<DWORD>* pWidgetsCreated = NULL);
		bool LoadFromString(const std::string& strXml, RtwWidget* pParent = NULL, std::list<DWORD>* pWidgetsCreated = NULL);

		RtsXmlDoc* getXmlDoc() { return m_pXmlDoc; }
		std::list<RtwButton*> m_lFaceButton;

	protected:
		RtsXmlDoc* m_pXmlDoc;

	public:
		bool _Process_Template(RtsXmlDoc::NodePtr* pNode, RtwWidget* pWidget, CUiTemplate* pTempl);
		bool _ProcessAttrib(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild(RtwWidget* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);

		bool _CreateRtXmlNode(RtsXmlDoc::NodePtr** ppRtXmlNode);
		bool _DeleteRtXmlNode(RtsXmlDoc::NodePtr** ppRtXmlNode);
		bool MergeRtXmlNode(RtsXmlDoc::NodePtr* pNodeTo, RtsXmlDoc::NodePtr* pNodeFrom);

	private:
		bool _ProcessNode(RtsXmlDoc::NodePtr* pNode, RtwWidget* pParent = NULL, std::list<DWORD>* pWidgetsCreated = NULL);
		bool _ProcessNode_AddWidget(RtwWidget* pParent, CUiLayer* pLayer, RtwWidget* pWidget);

		bool _MergeRtXmlNode(RtsXmlDoc::NodePtr* pNodeTo, RtsXmlDoc::NodePtr* pNodeFrom);

		bool _ProcessChild_Widget(RtwWidget* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode, std::list<DWORD>* pWidgetsCreated = NULL);
		bool _ProcessChild_Layer(CUiLayer* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_Form(RtwForm* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_Button(RtwButton* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_Label(RtwLabel* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_EditBox(RtwTextBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ScrollBarV(RtwVScrollBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ScrollBarH(RtwHScrollBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ListBox(RtwListBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ProgressBar(RtwProgressBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ComboBox(RtwComboBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_TabCtrl(RtwTab* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_CheckButton(CUiCheckButton* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_Tree(RtwTree* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_PopupMenu(RtwPopupMenu* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_MediaFrame(CUiMediaFrame* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_HtmlView(RtwHtmlView* pWidget,CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ChatInputBox(RtwChatInputBox* pWidget,CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_ChatBox(RtwChatBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessChild_3dView(Rtw3DView* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);

		bool _ProcessAttrib_Widget(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_Layer(CUiLayer* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_Form(RtwForm* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_Button(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_Label(RtwLabel* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_EditBox(RtwTextBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ScrollBarV(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ScrollBarH(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ListBox(RtwListBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ProgressBar(RtwProgressBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ComboBox(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_TabCtrl(RtwTab* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_CheckButton(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_Tree(RtwTree* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_PopupMenu(RtwPopupMenu* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_MediaFrame(CUiMediaFrame* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_HtmlView(RtwHtmlView* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ChatInputBox(RtwChatInputBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_ChatBox(RtwChatBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _ProcessAttrib_3dView(Rtw3DView* pWidget, RtsXmlDoc::NodePtr* pNode);

		RtwWidget*          _Process_Widget(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		CUiLayer*           _Process_Layer(RtsXmlDoc::NodePtr* pNode);
		RtwForm*            _Process_Form(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwButton*          _Process_Button(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwLabel*           _Process_Label(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwTextBox*         _Process_EditBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwVScrollBar*      _Process_ScrollBarV(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwHScrollBar*      _Process_ScrollBarH(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwListBox*         _Process_ListBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwProgressBar*     _Process_ProgressBar(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwComboBox*        _Process_ComboBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwTab*				_Process_TabCtrl(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		CUiCheckButton*     _Process_CheckButton(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwTree*            _Process_Tree(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwPopupMenu*       _Process_PopupMenu(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		CUiMediaFrame*      _Process_MediaFrame(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwAliasButton*		_Process_AliasButton(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwHtmlView*		_Process_HtmlView(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwChatInputBox*	_Process_ChatInputBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		RtwChatBox*			_Process_ChatBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);
		Rtw3DView*			_Process_3dView(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode);

		RtwImage* _Process_Image(RtsXmlDoc::NodePtr* pNode); // 返回一个RtwImage的指针，需要释放！！
		RtwComplexImage* _Process_ComplexImage(RtsXmlDoc::NodePtr* pNode);// 返回一个IUiImage的指针，需要释放！！

		bool _Process_BackgroundImage(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_ImageSequence(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_Hint(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_Anchors(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_Anchor(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_Border(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode);

		bool _Process_NormalImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_PushedImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_DisabledImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_HighlightImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_CheckedImage(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_CheckHighlightImage(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_FaceButton(RtwButton*	pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_TreeNode(RtwTree* pWidget, RtsXmlDoc::NodePtr* pNode);

		bool _Process_TabItem(RtwTab* pWidget, RtsXmlDoc::NodePtr* pNode);

		bool _Process_TitleBar(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode);
		bool _Process_TitleImage(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode);
		bool _Process_CloseButton(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode);

		bool _Process_DropdownButton(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_ComboEditor(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_ComboList(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode);

		bool _Process_HandleImage(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_TopButton(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_BottomButton(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_HandleImage(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_LeftButton(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);
		bool _Process_RightButton(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode);

		bool _Process_ProgressBarImage(RtwProgressBar* pWidget, RtsXmlDoc::NodePtr* pNode);
	};
}
#endif 

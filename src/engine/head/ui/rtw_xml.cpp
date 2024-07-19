#include "ui/rtw_ui.h"
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_xml.cpp
//	author:		ldr123
//	purpose:	uiϵͳͨ��xml���ؿؼ��Ĵ�����
//  other:		�ֲ��ع����ע��
//********************************************************************

RtwXmlLoader::RtwXmlLoader()
{
	//xml������
	m_pXmlDoc = RT_NEW RtsXmlDoc();
}

RtwXmlLoader::~RtwXmlLoader()
{
	DEL_ONE(m_pXmlDoc);
}

bool RtwXmlLoader::Reset()
{//���ýڵ�
	if (m_pXmlDoc)
		m_pXmlDoc->Cleanup();
	return true;
}

//����xml�ļ�
bool RtwXmlLoader::LoadFromFile(const std::string& FileName, RtwWidget* pParent, std::list<DWORD>* pWidgetsCreated)
{
	Reset();

	//�����ļ�������, ����0��ȷ����������.
	if (m_pXmlDoc->ParserFile(FileName.c_str()) != 0)
	{
		//��������,������·��������,����·��
		bool bSuccess = false;
		stlforeach (std::list<std::string>, iterPath, g_workspace.getDefaultXmlPaths())
		{
			const std::string& strPath = *iterPath;
			std::string strTmp = strPath + FileName;
			if (m_pXmlDoc->ParserFile(strTmp.c_str()) == 0)
			{
				bSuccess = true;
				break;
			}
		}

		if (!bSuccess)
		{
			RtCoreLog().Error("[RtwXmlLoader::LoadFromFile] Parse file \'%s\'error\n", FileName.c_str());
			return false;
		}
	}

	if (!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot(), pParent, pWidgetsCreated))
	{//��������
		RtCoreLog().Error("[RtwXmlLoader::LoadFromFile] Process file \'%s\'error\n", FileName.c_str());
		return false;
	}

	return true;
}

//���ַ�������xml
bool RtwXmlLoader::LoadFromString(const std::string& strXml, RtwWidget* pParent/* = NULL*/, std::list<DWORD>* pWidgetsCreated/* = NULL*/)
{
	Reset();

	if (m_pXmlDoc->ParserString(strXml.c_str()))
	{
		return false;
	}

	if (!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
		return false;

	Reset();

	return true;
}

//����xmlԪ��
bool RtwXmlLoader::_ProcessNode(RtsXmlDoc::NodePtr* pNode, RtwWidget* pParent/* = NULL*/, std::list<DWORD>* pWidgetsCreated/* = NULL*/)
{
	if (!pNode)
		return false;

	// a sample xml file
	// 
	//<?xml version="1.0" encoding="UTF-8"?>
	//<Ui>
	//    <Widget Name="XmlWidget" Inherit="" Location="100,100,200,200" Show="true" >
	//      <Form Name="XmlForm" Inherit="" Location="100,100,200,200" Text="This is a form" />
	//      <Button Name="XmlButton" Inherit="" Text="B111" Location="100,100,200,200" />
	//      <Label Name="XmlLabel1" Inherit="" Text="L111" Location="100,100,200,200" />
	//      <Label Name="XmlLabel2" Inherit="" Location="100,100,200,200" Text="L222" />
	//    </Widget>
	//</Ui>
	//

	RtwWidget* _pParent = NULL;
	CUiLayer* _pLayer = NULL;
	if (pParent)
	{
		if (pParent->getWidgetType() == wtLayer)
			_pLayer = (CUiLayer*)pParent;
		else 
			_pParent = pParent;
	}

	const std::string& TagName = pNode->strName;
	if (
		TagName == "Ui"				/*��ͨui���*/ 
		||  TagName == "SystemWidgets"	/*ϵͳ���*/
		)
	{
		UI_ENSURE_B(_ProcessChild_Widget(_pParent, _pLayer, pNode, pWidgetsCreated));
	}
	//if (TagName == "Ui")
	//{
	//	UI_ENSURE_B(_ProcessChild_Widget(_pParent, _pLayer, pNode, pWidgetsCreated));
	//}
	//else if (TagName == "SystemWidgets")
	//{
	//	UI_ENSURE_B(_ProcessChild_Widget(_pParent, _pLayer, pNode, pWidgetsCreated));
	//}
	else
	{
		RtCoreLog().Error("Invalid Tag Type \'%s\'\n", TagName.c_str());
		return false;
	}

	return true;
}

bool RtwXmlLoader::_ProcessNode_AddWidget(RtwWidget* pParent, CUiLayer* pLayer, RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget);

	if (pParent)
	{
		UI_ENSURE_B(pParent->AddChild(pWidget)); //����ӿؼ�
	}
	else if (pLayer)
	{
		UI_ENSURE_B(pLayer->AddWidget(pWidget)); //���widget
	}
	else 
	{
		UI_ENSURE_B(g_workspace.AddWidget(pWidget));
	}

	return true;
}

//��������
bool RtwXmlLoader::_ProcessAttrib(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	switch (pWidget->getWidgetType())
	{
	case wtWidget:
		UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode))
			break;
	case wtLayer:
		UI_ENSURE_B(_ProcessAttrib_Layer((CUiLayer*)pWidget, pNode))
			break;
	case wtLabel:
		UI_ENSURE_B(_ProcessAttrib_Label((RtwLabel*)pWidget, pNode))
			break;
	case wtButton:
		UI_ENSURE_B(_ProcessAttrib_Button((RtwButton*)pWidget, pNode))
			break;
	case wtForm:
		UI_ENSURE_B(_ProcessAttrib_Form((RtwForm*)pWidget, pNode))
			break;
	case wtEditBox:
		UI_ENSURE_B(_ProcessAttrib_EditBox((RtwTextBox*)pWidget, pNode))
			break;
	case wtScrollBarV:
		UI_ENSURE_B(_ProcessAttrib_ScrollBarV((RtwVScrollBar*)pWidget, pNode))
			break;
	case wtScrollBarH:
		UI_ENSURE_B(_ProcessAttrib_ScrollBarH((RtwHScrollBar*)pWidget, pNode))
			break;
	case wtListBox:
		UI_ENSURE_B(_ProcessAttrib_ListBox((RtwListBox*)pWidget, pNode))
			break;
	case wtProgressBar:
		UI_ENSURE_B(_ProcessAttrib_ProgressBar((RtwProgressBar*)pWidget, pNode))
			break;
	case wtComboBox:
		UI_ENSURE_B(_ProcessAttrib_ComboBox((RtwComboBox*)pWidget, pNode))
			break;
	case wtTabCtrl:
		UI_ENSURE_B(_ProcessAttrib_TabCtrl((RtwTab*)pWidget, pNode))
			break;
	case wtCheckButton:
		UI_ENSURE_B(_ProcessAttrib_CheckButton((CUiCheckButton*)pWidget, pNode))
			break;
	case wtTree:
		UI_ENSURE_B(_ProcessAttrib_Tree((RtwTree*)pWidget, pNode))
			break;
	case wtPopupMenu:
		UI_ENSURE_B(_ProcessAttrib_PopupMenu((RtwPopupMenu*)pWidget, pNode))
			break;
	case wtMediaFrame:
		UI_ENSURE_B(_ProcessAttrib_MediaFrame((CUiMediaFrame*)pWidget, pNode))
			break;
	case wtHtmlView:
		UI_ENSURE_B(_ProcessAttrib_HtmlView((RtwHtmlView*)pWidget, pNode))
			break;
	case wtChatBox:
		UI_ENSURE_B(_ProcessAttrib_ChatBox((RtwChatBox*)pWidget, pNode))
			break;
	case wtChatInputBox:
		UI_ENSURE_B(_ProcessAttrib_ChatInputBox((RtwChatInputBox*)pWidget, pNode))
			break;
	case wt3dview:
		UI_ENSURE_B(_ProcessAttrib_3dView((Rtw3DView*)pWidget, pNode))
		break;
	default:
		break;
	}

	return true;
}

//����widget����
bool RtwXmlLoader::_ProcessAttrib_Widget(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// Reset First
	pWidget->Reset();

	// ���ȼ��̳й�ϵ
	std::string strTemplate = pNode->GetProp_s("Inherit");
	if (!strTemplate.empty())
	{
		CUiTemplate* pTempl = g_workspace.getThemeManager()->getCurrentTheme()->FindTemplate(strTemplate);
		if (pTempl)
			_Process_Template(pNode, pWidget, pTempl);
		else 
			RtCoreLog().Error("[RtwXmlLoader::_ProcessAttrib_Widget] Template \'%s\' not found!\n", strTemplate.c_str());
	}

	std::string strName			= pNode->GetProp_s("Name");
	std::string strLocation		= pNode->GetProp_s("Location");
	std::string strText			= pNode->GetProp_s("Text");
	std::string strTextColor	= pNode->GetProp_s("TextColor");
	std::string strTextFont		= pNode->GetProp_s("TextFont");
	int iFontSize				= pNode->GetPropInt("FontSize");
	std::string strAlignCol		= pNode->GetProp_s("Alignment");
	std::string strAlignLine	= pNode->GetProp_s("AlignLine");
	std::string strShow			= pNode->GetProp_s("Show");
	std::string strActive		= pNode->GetProp_s("Active");
	std::string strMouseEvent	= pNode->GetProp_s("MouseEvent");
	std::string strMouseMove	= pNode->GetProp_s("MouseMove");
	std::string strBkColor		= pNode->GetProp_s("BackgroundColor");
	std::string strDragOut		= pNode->GetProp_s("DragOut");
	std::string strDragIn		= pNode->GetProp_s("DragIn");
	std::string strEnable		= pNode->GetProp_s("Enable");
	std::string strSize			= pNode->GetProp_s("Size");
	std::string strSound		= pNode->GetProp_s("Sound");
	std::string strBorderCorlor = pNode->GetProp_s("BorderCorlor");
	std::string strBorderSize	= pNode->GetProp_s("BorderSize");
	std::string strPadding		= pNode->GetProp_s("padding");
	std::string strPenetrate	= pNode->GetProp_s("Penetrate");
	std::string strMouseHoverImage	= pNode->GetProp_s("MouseHoverImage");
	std::string strMouseHoverSize	= pNode->GetProp_s("MouseHoverSize");
	std::string strMouseHoverImgRect= pNode->GetProp_s("MouseHoverImgRect");

	if(!strBorderSize.empty())
	{
		int Bsize=atoi(strBorderSize.c_str());
		pWidget->SetBorderSize(Bsize);
		pWidget->SetBorderColor(0xFFC28D3B); //Ӳ����߿���ɫ(Ĭ��)
	}

	if (!strPadding.empty())
	{
		SMargin sm(strPadding.c_str());
		pWidget->SetPadding(sm);
	}

	if(!strBorderCorlor.empty())
	{
		DWORD color = strtoul(strBorderCorlor.c_str()+1, NULL, 16); //�߿���ɫ
		pWidget->SetBorderColor(color);
	}

	if (!strShow.empty())		//�Ƿ���ʾ
	{
		if (strShow == "true")              pWidget->Show();
		else if (strShow == "false")        pWidget->Hide();
	}

	if (!strActive.empty())		//����(Ӧ���ǿ��õ�?)
	{
		if (strActive == "true")            pWidget->ModifyFlags(wfFocus, 0);
		else if (strActive == "false")      pWidget->ModifyFlags(0, wfFocus);
	}

	if (!strMouseEvent.empty())		//����¼����÷�
	{
		if (strMouseEvent == "true")        pWidget->ModifyFlags(wfGrabMouse, 0);
		else if (strMouseEvent == "false")  pWidget->ModifyFlags(0, wfGrabMouse);
	}

	if (!strMouseMove.empty())		//����ƶ�
	{
		if (strMouseMove == "true")         pWidget->ModifyFlags(wfMouseMove, 0);
		else if (strMouseMove == "false")  pWidget->ModifyFlags(0, wfMouseMove);
	}

	if (!strPenetrate.empty())		//�ɴ�͸
	{
		if (strPenetrate == "true")
		{
			pWidget->ModifyFlags(wfPenetrate, 0);
		}
		else if (strPenetrate == "false")
		{
			pWidget->ModifyFlags(0, wfPenetrate);
		}
	}

	if (!strName.empty())pWidget->SetName(strName);//�ؼ���

	if (!strText.empty())pWidget->SetText(strText);//��ʾ����

	RtwRect rcLocation;
	rcLocation.FromString(strLocation); //�ؼ���ʾ����
	if (!strLocation.empty())
	{//�ؼ�λ��
		pWidget->Move(rcLocation);
		pWidget->SetWidgetSize(SSize(rcLocation.right-rcLocation.left,rcLocation.bottom-rcLocation.top));
	}

	//����ɫ
	if (!strBkColor.empty())
	{
		DWORD color = strtoul(strBkColor.c_str()+1, NULL, 16); // "#bbffffff"
		pWidget->SetBackgroundColor(color);
	}

	//�ı���ɫ
	if (!strTextColor.empty())
	{
		DWORD color = strtoul(strTextColor.c_str()+1, NULL, 16); // "#bbffffff"
		pWidget->SetTextColor(color);
	}

	//�ı�����
	if (!strTextFont.empty()) 
	{
		pWidget->SetTextFont(strTextFont);
	}

	//�����С(�ֺ�)
	if(iFontSize)
	{
		pWidget->SetFontSize(iFontSize);

	}

	//���뷽ʽ
	if (!strAlignCol.empty())
	{
		SUiTextAlignment Alignment = pWidget->getTextAlignment();
		if (strAlignCol == "LEFT")
		{
			Alignment.align = alignNear;
			pWidget->SetTextAlignment(Alignment);
		}
		else if (strAlignCol == "center")
		{
			Alignment.align = alignCenter;
			pWidget->SetTextAlignment(Alignment);
		}
		else if (strAlignCol == "RIGHT")
		{
			Alignment.align = alignFar;
			pWidget->SetTextAlignment(Alignment);
		}
	}
	if (!strAlignLine.empty())
	{
		SUiTextAlignment Alignment = pWidget->getTextAlignment();
		if (strAlignLine == "TOP")
		{
			Alignment.lineAlign = alignNear;
			pWidget->SetTextAlignment(Alignment);
		}
		else if (strAlignLine == "CENTER")
		{
			Alignment.lineAlign = alignCenter;
			pWidget->SetTextAlignment(Alignment);
		}
		else if (strAlignLine == "BOTTOM")
		{
			Alignment.lineAlign = alignFar;
			pWidget->SetTextAlignment(Alignment);
		}
	}

	//������(��ק��)
	if (!strDragOut.empty())
	{
		if (strDragOut == "true")           pWidget->ModifyFlags(wfDragOut, 0);
		else if (strDragOut == "false")     pWidget->ModifyFlags(0, wfDragOut);
	}

	//������(��ק��)
	if (!strDragIn.empty())
	{
		if (strDragIn == "true")            pWidget->ModifyFlags(wfDragIn, 0);
		else if (strDragIn == "false")      pWidget->ModifyFlags(0, wfDragIn);
	}

	//����
	if (!strEnable.empty())
	{
		if (strEnable == "true")            pWidget->Enable();
		else if (strEnable == "false")      pWidget->Disable();
	}

	//�ؼ���С(ò��û�õ�)
	if (!strSize.empty())
	{
		SSize WidgetSize;
		if (WidgetSize.FromString(strSize))
		{
			pWidget->SetWidgetSize(WidgetSize);
		}
	}

	//����
	if (!strSound.empty())
	{
		pWidget->SetSoundFile(strSound);
	}

	//����¼�����?
	if (!strMouseHoverImage.empty())
	{
		RtwRect src;
		SMargin tmp;
		//���������ƶ���ȥ��߿�ı仯
		if (strMouseHoverSize.empty())
		{
			tmp = SMargin(2,2,2,2);
		}
		else
		{
			tmp = SMargin(strMouseHoverSize.c_str());
		}

		if (!strMouseHoverImgRect.empty())
		{
			src.FromString(strMouseHoverImgRect);
		}
		else
		{
			src = RtwRect(0, 0, 0, 0);
		}

		pWidget->SetHoverBoderImage(strMouseHoverImage, src, tmp);
	}

	return true;
}

//��(��)�����Դ���(ò��ֻ��Ϊ��������,�ص���ui�༭����ʹ��)
bool RtwXmlLoader::_ProcessAttrib_Layer(CUiLayer* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	std::string strName = pNode->GetProp_s("Name");
	std::string strShow = pNode->GetProp_s("Show");
	if (!strShow.empty())
	{
		if (strShow == "true") 
			pWidget->Show();
		else if (strShow == "false")
			pWidget->Hide();
	}
	pWidget->SetName(strName);
	return true;
}

//form�����Դ���
bool RtwXmlLoader::_ProcessAttrib_Form(RtwForm* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	//���ȴ���widget�ؼ������Լ���
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	//���ƶ�?
	std::string strMove = pNode->GetProp_s("Movable");
	if (!strMove.empty())
	{
		if (strMove == "true")         
		{
			pWidget->SetMovable(true);
		}
		else
		{
			pWidget->SetMovable(false);
		}
	}
	else
	{
		pWidget->SetMovable(false);
	}

	//�Զ����������
	std::string strAutoScrollV = pNode->GetProp_s("AutoScrollV");
	if (!strAutoScrollV.empty())
	{
		if (strAutoScrollV == "true")          pWidget->SetAutoScrollV(true);
		else if (strAutoScrollV == "false")    pWidget->SetAutoScrollV(false);
	}

	//�Զ����������
	std::string strAutoScrollH = pNode->GetProp_s("AutoScrollH");
	if (!strAutoScrollH.empty())
	{
		if (strAutoScrollH == "true")          pWidget->SetAutoScrollH(true);
		else if (strAutoScrollH == "false")    pWidget->SetAutoScrollH(false);
	}

	//esc�������ش���
	std::string strEscapeHide = pNode->GetProp_s("EscapeHide");
	if (!strEscapeHide.empty())
	{
		if (strEscapeHide == "true")            pWidget->SetEscapeHide(true);
		else if (strEscapeHide == "false")      pWidget->SetEscapeHide(false);
	}

	//�߿�����?
	std::string strBorderpadding = pNode->GetProp_s("Borderpadding");
	if (!strBorderpadding.empty())
	{
		if (strBorderpadding == "true")            pWidget->SetBorderpadding(true);
		else if (strBorderpadding == "false")      pWidget->SetBorderpadding(false);
	}
	return true;
}

//��ť���Դ���
bool RtwXmlLoader::_ProcessAttrib_Button(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));
	return true;
}

//��̬�����Դ���
bool RtwXmlLoader::_ProcessAttrib_Label(RtwLabel* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B (_ProcessAttrib_Widget(pWidget, pNode));
	std::string strHyperLink = pNode->GetProp_s("HyperLink");
	//�����Ӵ���
	if (!strHyperLink.empty())
	{
		SHyperLink HLink;
		if (HLink.FromString(strHyperLink))
		{
			pWidget->SetHyperLink(&HLink);
		}
	}
	return true;
}

//�ı�������
bool RtwXmlLoader::_ProcessAttrib_EditBox(RtwTextBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	// ������
	std::string strEnableInput = pNode->GetProp_s("EnableInput");
	if (!strEnableInput.empty())
	{
		if (strEnableInput == "true")
			pWidget->SetEnableInput(true);
		else if (strEnableInput == "false")
			pWidget->SetEnableInput(false);
	}

	// ������ʾ
	std::string strMultiLine = pNode->GetProp_s("MultiLine");
	if (!strMultiLine.empty())
	{
		if (strMultiLine == "true")
			pWidget->SetMultiLine(true);
		else if (strMultiLine == "false")
			pWidget->SetMultiLine(false);
	}

	// ���ֻ���
	std::string strWordWrap = pNode->GetProp_s("WordWrap");
	if (!strWordWrap.empty())
	{
		if (strWordWrap == "true")
			pWidget->SetWordWrap(true);
		else if (strWordWrap == "false")
			pWidget->SetWordWrap(false);
	}

	// �Զ����������
	std::string strAutoScrollV = pNode->GetProp_s("AutoScrollV");
	if (!strAutoScrollV.empty())
	{
		if (strAutoScrollV == "true")
			pWidget->SetAutoScrollV(true);
		else if (strAutoScrollV == "false")
			pWidget->SetAutoScrollV(false);
	}

	//�Ƿ񵱳���������(��ʾָ�����ַ������û�������ı�)
	std::string strPassword = pNode->GetProp_s("Password");
	if (!strPassword.empty())
	{
		if (strPassword == "true")
			pWidget->SetPassword(true);
		else if (strPassword == "false")
			pWidget->SetPassword(false);
	}

	//ָ��һ���ַ�,��password����ʱ,�ô��ַ������û�������ַ�
	std::string strPasswordChar = pNode->GetProp_s("PasswordChar");
	if (!strPasswordChar.empty())
	{
		pWidget->SetPasswordChar(strPasswordChar[0]);
	}

	//�����Զ���С(�Զ������߶�)
	std::string strAutoSizeV = pNode->GetProp_s("AutoSizeV");
	if (!strAutoSizeV.empty())
	{
		if (strAutoSizeV == "true")
			pWidget->SetAutoSizeV(true);
		else if (strAutoSizeV == "false")
			pWidget->SetAutoSizeV(false);
	}

	//�Զ��������
	std::string strAutoSizeH = pNode->GetProp_s("AutoSizeH");
	if (!strAutoSizeH.empty())
	{
		if (strAutoSizeH == "true")
			pWidget->SetAutoSizeV(true);
		else if (strAutoSizeH == "false")
			pWidget->SetAutoSizeH(false);
	}

	//���ø��ı�
	std::string strRichText = pNode->GetProp_s("RichText");
	if (!strRichText.empty())
	{
		pWidget->SetRichText(strRichText);
	}

	return true;
}

//���������
bool RtwXmlLoader::_ProcessAttrib_ChatInputBox(RtwChatInputBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//html������
bool RtwXmlLoader::_ProcessAttrib_HtmlView(RtwHtmlView* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//���������
bool RtwXmlLoader::_ProcessAttrib_ScrollBarV(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//���������
bool RtwXmlLoader::_ProcessAttrib_ScrollBarH(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));
	std::string strEnableLeftRightBtn = pNode->GetProp_s("EnableLRBtn");
	if (strEnableLeftRightBtn == "false" || strEnableLeftRightBtn == "False")
		pWidget->SetUseLeftRightButton(false);

	return true;
}

//�б��
bool RtwXmlLoader::_ProcessAttrib_ListBox(RtwListBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//������ʾ��
bool RtwXmlLoader::_ProcessAttrib_ChatBox(RtwChatBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessAttrib_3dView(Rtw3DView* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//������
bool RtwXmlLoader::_ProcessAttrib_ProgressBar(RtwProgressBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//������
bool RtwXmlLoader::_ProcessAttrib_ComboBox(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	// ������
	std::string strEnableInput = pNode->GetProp_s("EnableInput");
	if (!strEnableInput.empty())
	{
		if (strEnableInput == "true")
			pWidget->SetEnableInput(true);
		else if (strEnableInput == "false")
			pWidget->SetEnableInput(false);
	}

	return true;
}

//��ҳ
bool RtwXmlLoader::_ProcessAttrib_TabCtrl(RtwTab* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	//Ĭ��ѡ��Ĳ�ҳid
	std::string strSelectTab = pNode->GetProp_s("SelectTab");
	if (!strSelectTab.empty())
	{
		int iTabIndex = atoi(strSelectTab.c_str());
		pWidget->SetCurrentSelect(iTabIndex);
	}

	return true;
}

//��ѡ��
bool RtwXmlLoader::_ProcessAttrib_CheckButton(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Button(pWidget, pNode));

	std::string strRadioID = pNode->GetProp_s("RadioGroupID");
	if (strRadioID != "")
	{
		pWidget->setRadioID(strRadioID);
	}

	return true;
}

//���οؼ�
bool RtwXmlLoader::_ProcessAttrib_Tree(RtwTree* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	//�Զ����������
	std::string strAutoScrollV = pNode->GetProp_s("AutoScrollV");
	if (!strAutoScrollV.empty())
	{
		if (strAutoScrollV == "true")          pWidget->SetAutoScrollV(true);
		else if (strAutoScrollV == "false")    pWidget->SetAutoScrollV(false);
	}

	//�Զ����������
	std::string strAutoScrollH = pNode->GetProp_s("AutoScrollH");
	if (!strAutoScrollH.empty())
	{
		if (strAutoScrollH == "true")          pWidget->SetAutoScrollH(true);
		else if (strAutoScrollH == "false")    pWidget->SetAutoScrollH(false);
	}

	return true;
}

//����ʽ�˵�
bool RtwXmlLoader::_ProcessAttrib_PopupMenu(RtwPopupMenu* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	return true;
}

//ò��δ�õ�
bool RtwXmlLoader::_ProcessAttrib_MediaFrame(CUiMediaFrame* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	std::string strAutoScrollV = pNode->GetProp_s("AutoScrollV");
	if (!strAutoScrollV.empty())
	{
		if (strAutoScrollV == "true")          pWidget->SetAutoScrollV(true);
		else if (strAutoScrollV == "false")    pWidget->SetAutoScrollV(false);
	}

	std::string strAutoScrollH = pNode->GetProp_s("AutoScrollH");
	if (!strAutoScrollH.empty())
	{
		if (strAutoScrollH == "true")          pWidget->SetAutoScrollH(true);
		else if (strAutoScrollH == "false")    pWidget->SetAutoScrollH(false);
	}

	return true;
}

//�����ӿؼ�
bool RtwXmlLoader::_ProcessChild(RtwWidget* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	switch (pWidget->getWidgetType())
	{
	case wtWidget:
		UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode))
			break;
	case wtLayer:
		UI_ENSURE_B(_ProcessChild_Layer((CUiLayer*)pWidget, pLayer, pNode))
			break;
	case wtLabel:
		UI_ENSURE_B(_ProcessChild_Label((RtwLabel*)pWidget, pLayer, pNode))
			break;
	case wtButton:
		UI_ENSURE_B(_ProcessChild_Button((RtwButton*)pWidget, pLayer, pNode))
			break;
	case wtForm:
		UI_ENSURE_B(_ProcessChild_Form((RtwForm*)pWidget, pLayer, pNode))
			break;
	case wtEditBox:
		UI_ENSURE_B(_ProcessChild_EditBox((RtwTextBox*)pWidget, pLayer, pNode))
			break;
	case wtScrollBarV:
		UI_ENSURE_B(_ProcessChild_ScrollBarV((RtwVScrollBar*)pWidget, pLayer, pNode))
			break;
	case wtScrollBarH:
		UI_ENSURE_B(_ProcessChild_ScrollBarH((RtwHScrollBar*)pWidget, pLayer, pNode))
			break;
	case wtListBox:
		UI_ENSURE_B(_ProcessChild_ListBox((RtwListBox*)pWidget, pLayer, pNode))
			break;
	case wtProgressBar:
		UI_ENSURE_B(_ProcessChild_ProgressBar((RtwProgressBar*)pWidget, pLayer, pNode))
			break;
	case wtComboBox:
		UI_ENSURE_B(_ProcessChild_ComboBox((RtwComboBox*)pWidget, pLayer, pNode))
			break;
	case wtTabCtrl:
		UI_ENSURE_B(_ProcessChild_TabCtrl((RtwTab*)pWidget, pLayer, pNode))
			break;
	case wtCheckButton:
		UI_ENSURE_B(_ProcessChild_CheckButton((CUiCheckButton*)pWidget, pLayer, pNode))
			break;
	case wtTree:
		UI_ENSURE_B(_ProcessChild_Tree((RtwTree*)pWidget, pLayer, pNode))
			break;
	case wtPopupMenu:
		UI_ENSURE_B(_ProcessChild_PopupMenu((RtwPopupMenu*)pWidget, pLayer, pNode))
			break;
	case wtMediaFrame:
		UI_ENSURE_B(_ProcessChild_MediaFrame((CUiMediaFrame*)pWidget, pLayer, pNode))
			break;
	case wtHtmlView:
		UI_ENSURE_B(_ProcessChild_HtmlView((RtwHtmlView*)pWidget,pLayer,pNode))
			break;
	case wtChatInputBox:
		UI_ENSURE_B(_ProcessChild_ChatInputBox((RtwChatInputBox*)pWidget, pLayer, pNode))
			break;
	case wtChatBox:
		UI_ENSURE_B(_ProcessChild_ChatBox((RtwChatBox*)pWidget, pLayer, pNode))
	case wt3dview:
		UI_ENSURE_B(_ProcessChild_3dView((Rtw3DView*)pWidget, pLayer, pNode))
			break;
	default:
		break;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_Widget(
											RtwWidget* pWidget, CUiLayer* pLayer, 
											RtsXmlDoc::NodePtr* pNode, 
											std::list<DWORD>* pWidgetsCreated
									   )
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "BackgroundImage")
			UI_ENSURE_B(_Process_BackgroundImage(pWidget, pChildNode))
		else if (pChildNode->strName == "ImageSequence")
		UI_ENSURE_B(_Process_ImageSequence(pWidget, pChildNode))
		else if (pChildNode->strName == "Hint")
		UI_ENSURE_B(_Process_Hint(pWidget, pChildNode))
		else if (pChildNode->strName == "Anchors")
		UI_ENSURE_B(_Process_Anchors(pWidget, pChildNode))
		else if (pChildNode->strName == "Border")
		UI_ENSURE_B(_Process_Border(pWidget, pChildNode))
		else if (pChildNode->strName == "FaceImage")
		UI_ENSURE_B(_Process_FaceButton((RtwButton*)pWidget,pChildNode))

		// Layer�������
		else if (pChildNode->strName == "Layer")
		{
			RtwWidget* pWidget = _Process_Layer(pChildNode);
			UI_ENSURE_B(pWidget);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pWidget->getId());
		}

		else if (pChildNode->strName == "Widget")
		{
			RtwWidget* pTmp = _Process_Widget(pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "Form")
		{
			RtwWidget* pTmp = _Process_Form((RtwForm*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "Button")
		{
			RtwWidget* pTmp = _Process_Button((RtwButton*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "Label")
		{
			RtwWidget* pTmp = _Process_Label((RtwLabel*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "EditBox")
		{
			RtwWidget* pTmp = _Process_EditBox((RtwTextBox*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ScrollBarV")
		{
			RtwWidget* pTmp = _Process_ScrollBarV((RtwVScrollBar*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ScrollBarH")
		{
			RtwWidget* pTmp = _Process_ScrollBarH((RtwHScrollBar*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ListBox")
		{
			RtwWidget* pTmp = _Process_ListBox((RtwListBox*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ProgressBar")
		{
			RtwWidget* pTmp = _Process_ProgressBar((RtwProgressBar*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ComboBox")
		{
			RtwWidget* pTmp = _Process_ComboBox((RtwComboBox*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "TabCtrl")
		{
			RtwWidget* pTmp = _Process_TabCtrl((RtwTab*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "CheckButton")
		{
			RtwWidget* pTmp = _Process_CheckButton((CUiCheckButton*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "Tree")
		{
			RtwWidget* pTmp = _Process_Tree((RtwTree*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "PopupMenu")
		{
			RtwWidget* pTmp = _Process_PopupMenu((RtwPopupMenu*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "MediaFrame")
		{
			RtwWidget* pTmp = _Process_MediaFrame((CUiMediaFrame*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "AliasButton")
		{
			RtwWidget* pTmp = _Process_AliasButton((RtwAliasButton*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "HtmlView")
		{
			RtwWidget* pTmp = _Process_HtmlView((RtwHtmlView*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ChatInputBox")
		{
			RtwWidget* pTmp = _Process_ChatInputBox((RtwChatInputBox*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "ChatBox")
		{
			RtwWidget* pTmp = _Process_ChatBox((RtwChatBox*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		else if (pChildNode->strName == "D3DView")
		{
			RtwWidget* pTmp = _Process_3dView((Rtw3DView*)pWidget, pLayer, pChildNode);
			UI_ENSURE_B(pTmp);
			if (pWidgetsCreated)
				pWidgetsCreated->push_back(pTmp->getId());
		}
		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_Layer(CUiLayer* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_Form(RtwForm* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	pWidget->getCloseButton()->ModifyFlags(0,wfVisible);	//Ĭ�Ϲرմ���رհ�ť����ʾ
	while (pChildNode)
	{
		if (pChildNode->strName == "TitleBar")
			UI_ENSURE_B(_Process_TitleBar(pWidget, pChildNode))
		else if (pChildNode->strName == "CloseButton")
		UI_ENSURE_B(_Process_CloseButton(pWidget, pChildNode))

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_Button(RtwButton* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "NormalImage")		//����
			UI_ENSURE_B(_Process_NormalImage(pWidget, pChildNode))
		else if (pChildNode->strName == "PushedImage")	//����
		UI_ENSURE_B(_Process_PushedImage(pWidget, pChildNode))
		else if (pChildNode->strName == "DisabledImage")//����
		UI_ENSURE_B(_Process_DisabledImage(pWidget, pChildNode))
		else if (pChildNode->strName == "HighlightImage")//����
		UI_ENSURE_B(_Process_HighlightImage(pWidget, pChildNode))
		else if (pChildNode->strName == "BackgroundImage")//����
		UI_ENSURE_B(_Process_BackgroundImage(pWidget, pChildNode))

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_Label(RtwLabel* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_EditBox(RtwTextBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_ChatInputBox(RtwChatInputBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_HtmlView(RtwHtmlView* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_ScrollBarV(RtwVScrollBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		//�м�����ͼƬ
		if (pChildNode->strName == "HandleImage")
			UI_ENSURE_B(_Process_HandleImage(pWidget, pChildNode))
		else if (pChildNode->strName == "TopButton")	//����ť
		UI_ENSURE_B(_Process_TopButton(pWidget, pChildNode))
		else if (pChildNode->strName == "BottomButton")	//�װ�ť
		UI_ENSURE_B(_Process_BottomButton(pWidget, pChildNode))

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_ScrollBarH(RtwHScrollBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		//�м�����ͼƬ
		if (pChildNode->strName == "HandleImage")
			UI_ENSURE_B(_Process_HandleImage(pWidget, pChildNode))
		else if (pChildNode->strName == "LeftButton")	//��߰�ť
		UI_ENSURE_B(_Process_LeftButton(pWidget, pChildNode))
		else if (pChildNode->strName == "RightButton")	//�ұ߰�ť
		UI_ENSURE_B(_Process_RightButton(pWidget, pChildNode))

		pChildNode = pChildNode->pNext;
	}

	return true;
}
bool RtwXmlLoader::_ProcessChild_ChatBox(RtwChatBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));
	return true;
}

bool RtwXmlLoader::_ProcessChild_3dView(Rtw3DView* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));
	return true;
}

bool RtwXmlLoader::_ProcessChild_ListBox(RtwListBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Column")	//��
		{
			std::string strName = pChildNode->GetProp_s("Name");	//����
			std::string	strwidth = pChildNode->GetProp_s("Width");	//�п�
			int index = pWidget->InsertColumn(strName, atoi(strwidth.c_str()));
			RtwButton* pBtn = pWidget->getHeaderButton(index);
			if (pBtn)
			{
				// ����
				UI_ENSURE_P(_ProcessAttrib_Button(pBtn, pChildNode));
				// ���е���Widget
				UI_ENSURE_P(_ProcessChild_Button(pBtn, NULL, pChildNode));
			}
		}

		pChildNode = pChildNode->pNext;
	}
	pWidget->Refresh();

	return true;
}

bool RtwXmlLoader::_ProcessChild_ProgressBar(RtwProgressBar* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "ProgressBarImage")	//������ͼƬ
			UI_ENSURE_B(_Process_ProgressBarImage(pWidget, pChildNode))

			pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_ComboBox(RtwComboBox* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode))

		RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "DropdownButton")	//������������ť
			UI_ENSURE_B(_Process_DropdownButton(pWidget, pChildNode))
		else if (pChildNode->strName == "Editor")		//�������е������
		UI_ENSURE_B(_Process_ComboEditor(pWidget, pChildNode))
		else if (pChildNode->strName == "List")
		UI_ENSURE_B(_Process_ComboList(pWidget,pChildNode))
		else if (pChildNode->strName == "DropdownBox")	
		UI_ENSURE_B(_ProcessChild_ListBox(pWidget->getDropList(),pLayer,pChildNode))

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_TabCtrl(RtwTab* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	// TabItem
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "TabItem")	//��ѡ��
			UI_ENSURE_B(_Process_TabItem(pWidget, pChildNode))

			pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_CheckButton(CUiCheckButton* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Button(pWidget, pLayer, pNode));

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "CheckedImage")	//ѡ�а�ť
			UI_ENSURE_B(_Process_CheckedImage(pWidget, pChildNode));

		if (pChildNode->strName == "HighlightImage")//������ť
			UI_ENSURE_B(_Process_CheckHighlightImage(pWidget, pChildNode));

		if (pChildNode->strName == "RadioGroupID")	//����(���ڵ�ѡť,ͬһ������ֻ��һ����ť��ѡ��)
		{
			std::string strRadioID = pChildNode->GetProp_s("RadioGroupID");

			pWidget->setRadioID(strRadioID);
		}

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_Tree(RtwTree* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Node")	//tree�Ľڵ�
			UI_ENSURE_B(_Process_TreeNode(pWidget, pChildNode))
			pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_ProcessChild_PopupMenu(RtwPopupMenu* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

bool RtwXmlLoader::_ProcessChild_MediaFrame(CUiMediaFrame* pWidget, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, pLayer, pNode));

	return true;
}

//��ģ���д���̳е�Ԫ��
bool RtwXmlLoader::_Process_Template(RtsXmlDoc::NodePtr* pNode, RtwWidget* pWidget, CUiTemplate* pTempl)
{
	if (pTempl)
	{
		UI_ENSURE_B(pTempl->IsCompatibleWith(pWidget->getWidgetType()));
		MergeRtXmlNode(pNode, pTempl->getNode()); //�ϲ��ڵ�
	}

	return true;
}

RtwWidget* RtwXmlLoader::_Process_Widget(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwWidget* pWidget = g_workspace.getWidgetFactory()->createWidget(wtWidget);
	UI_ENSURE_B(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_B(_ProcessAttrib_Widget(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_B(_ProcessChild_Widget(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

CUiLayer* RtwXmlLoader::_Process_Layer(RtsXmlDoc::NodePtr* pNode)
{
	CUiLayer* pLayer = NULL;
	std::string LayerName = pNode->GetProp_s("Name");
	if (!LayerName.empty() && !g_workspace.FindLayer(LayerName, &pLayer))
	{
		pLayer = (CUiLayer*)g_workspace.getWidgetFactory()->createWidget(wtLayer);
		UI_ENSURE_B(g_workspace.AddLayer(pLayer));
	}

	// ����
	UI_ENSURE_B(_ProcessAttrib_Layer(pLayer, pNode));

	// ����Widget����
	UI_ENSURE_B(_ProcessChild_Layer(NULL, pLayer, pNode));

	pLayer->drop();
	return pLayer;
}

RtwForm* RtwXmlLoader::_Process_Form(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwForm* pWidget = (RtwForm*)g_workspace.getWidgetFactory()->createWidget(wtForm);
	UI_ENSURE_B(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_B(_ProcessAttrib_Form(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_B(_ProcessChild_Form(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwButton* RtwXmlLoader::_Process_Button(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwButton* pWidget = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwAliasButton* RtwXmlLoader::_Process_AliasButton(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwAliasButton* pWidget = (RtwAliasButton*)g_workspace.getWidgetFactory()->createWidget(wtAliasButton);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwLabel* RtwXmlLoader::_Process_Label(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwLabel* pWidget = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_Label(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Label(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwTextBox* RtwXmlLoader::_Process_EditBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwTextBox* pWidget = (RtwTextBox*)g_workspace.getWidgetFactory()->createWidget(wtEditBox);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_EditBox(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_EditBox(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwChatInputBox* RtwXmlLoader::_Process_ChatInputBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwChatInputBox* pWidget = (RtwChatInputBox*)g_workspace.getWidgetFactory()->createWidget(wtChatInputBox);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));
	// ����
	UI_ENSURE_P(_ProcessAttrib_ChatInputBox(pWidget, pNode));
	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ChatInputBox(pWidget, NULL, pNode));
	pWidget->drop();
	return pWidget;
}

RtwHtmlView* RtwXmlLoader::_Process_HtmlView(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwHtmlView* pWidget = (RtwHtmlView*)g_workspace.getWidgetFactory()->createWidget(wtHtmlView);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));
	// ����
	UI_ENSURE_P(_ProcessAttrib_HtmlView(pWidget, pNode));
	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_HtmlView(pWidget, NULL, pNode));
	pWidget->drop();
	return pWidget;
}

RtwVScrollBar* RtwXmlLoader::_Process_ScrollBarV(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwVScrollBar* pWidget = (RtwVScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarV);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_ScrollBarV(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ScrollBarV(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwHScrollBar* RtwXmlLoader::_Process_ScrollBarH(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwHScrollBar* pWidget = (RtwHScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarH);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_ScrollBarH(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ScrollBarH(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwListBox* RtwXmlLoader::_Process_ListBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwListBox* pWidget = (RtwListBox*)g_workspace.getWidgetFactory()->createWidget(wtListBox);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_ListBox(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ListBox(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

Rtw3DView* RtwXmlLoader::_Process_3dView(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	Rtw3DView* pWidget = (Rtw3DView*)g_workspace.getWidgetFactory()->createWidget(wt3dview);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));
	// ����
	UI_ENSURE_P(_ProcessAttrib_3dView(pWidget, pNode));
	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_3dView(pWidget, NULL, pNode));
	pWidget->drop();
	return pWidget;
}

RtwChatBox* RtwXmlLoader::_Process_ChatBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwChatBox* pWidget = (RtwChatBox*)g_workspace.getWidgetFactory()->createWidget(wtChatBox);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));
	// ����
	UI_ENSURE_P(_ProcessAttrib_ChatBox(pWidget, pNode));
	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ChatBox(pWidget, NULL, pNode));
	pWidget->drop();
	return pWidget;
}

RtwProgressBar* RtwXmlLoader::_Process_ProgressBar(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwProgressBar* pWidget = (RtwProgressBar*)g_workspace.getWidgetFactory()->createWidget(wtProgressBar);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_ProgressBar(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ProgressBar(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwComboBox* RtwXmlLoader::_Process_ComboBox(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwComboBox* pWidget = (RtwComboBox*)g_workspace.getWidgetFactory()->createWidget(wtComboBox);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_ComboBox(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_ComboBox(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwTab* RtwXmlLoader::_Process_TabCtrl(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwTab* pWidget = (RtwTab*)g_workspace.getWidgetFactory()->createWidget(wtTabCtrl);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_TabCtrl(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_TabCtrl(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

CUiCheckButton* RtwXmlLoader::_Process_CheckButton(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	CUiCheckButton* pWidget = (CUiCheckButton*)g_workspace.getWidgetFactory()->createWidget(wtCheckButton);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_CheckButton(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_CheckButton(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwTree* RtwXmlLoader::_Process_Tree(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwTree* pWidget = (RtwTree*)g_workspace.getWidgetFactory()->createWidget(wtTree);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_Tree(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Tree(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

RtwPopupMenu* RtwXmlLoader::_Process_PopupMenu(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	RtwPopupMenu* pWidget = (RtwPopupMenu*)g_workspace.getWidgetFactory()->createWidget(wtPopupMenu);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_PopupMenu(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_PopupMenu(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

CUiMediaFrame* RtwXmlLoader::_Process_MediaFrame(RtwWidget* pParent, CUiLayer* pLayer, RtsXmlDoc::NodePtr* pNode)
{
	CUiMediaFrame* pWidget = (CUiMediaFrame*)g_workspace.getWidgetFactory()->createWidget(wtMediaFrame);
	UI_ENSURE_P(_ProcessNode_AddWidget(pParent, pLayer, pWidget));

	// ����
	UI_ENSURE_P(_ProcessAttrib_MediaFrame(pWidget, pNode));

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_MediaFrame(pWidget, NULL, pNode));

	pWidget->drop();
	return pWidget;
}

//ͼƬ����
RtwImage* RtwXmlLoader::_Process_Image(RtsXmlDoc::NodePtr* pNode)
{
	std::string strFile = pNode->GetProp_s("File");					//ͼƬ�ļ�
	std::string strSource = pNode->GetProp_s("Source");				//ͼƬԴ����
	std::string strBlend = pNode->GetProp_s("Blend");				//�����Ⱦ
	std::string strTransparency = pNode->GetProp_s("Transparency");	//͸����

	bool bBlend;
	//�����gif��swf,ʹ�û����Ⱦ
	if (strFile.rfind(".gif") != string::npos || strFile.rfind(".swf") != string::npos)
	{
		bBlend = true;
	}
	else
	{
		bBlend = false;
		if (strBlend == "true")
			bBlend = true;
		else if (strBlend == "false")
			bBlend = false;
	}

	Real transparency = 0;
	if (!strTransparency.empty())
		transparency = atof(strTransparency.c_str());

	RtwImage* pImage = g_workspace.getImageFactory()->createImage(strFile);
	if (pImage)
	{
		RtwRect rcSource;
		if (!strSource.empty())
		{
			if (rcSource.FromString(strSource))
			{
				pImage->SetSourceRect(rcSource);//modify by fox
			}
		}
		pImage->SetBlend(bBlend);
		pImage->SetTransparency(transparency);
	}

	return pImage;
}

//����ͼƬ����
bool RtwXmlLoader::_Process_ImageSequence(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwComplexImage* pImageSequence = g_workspace.getImageFactory()->createImageSequence();

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Image")	//ͼƬ
		{
			RtwImage* pImage = _Process_Image(pChildNode);
			pImageSequence->AddImage(pImage);
			DROP_TRUI_IMAGE(pImage);
		}

		pChildNode = pChildNode->pNext;
	}

	//����ͼƬ����
	pWidget->SetImageSequence(pImageSequence);

	DROP_TRUI_IMAGE(pImageSequence);

	return true;
}

//������ͼƬ
RtwComplexImage* RtwXmlLoader::_Process_ComplexImage(RtsXmlDoc::NodePtr* pNode)
{
	std::string strFile				= pNode->GetProp_s("File");			//ͼƬ�ļ���
	std::string strSource			= pNode->GetProp_s("Source");		//ͼƬԴ����
	std::string strBlend			= pNode->GetProp_s("Blend");		//�����Ⱦ
	std::string strTransparency		= pNode->GetProp_s("Transparency");	//͸����
	std::string strBorder			= pNode->GetProp_s("Border");		//�߿�
	std::string strBorderFile		= pNode->GetProp_s("BorderFile");	//�߿��ļ�
	std::string strBorderSource		= pNode->GetProp_s("BorderSource");	//�߿�Դ����
	std::string strBorderBlend		= pNode->GetProp_s("BorderBlend");	//�߿�����Ⱦ
	std::string strBorderTransparency = pNode->GetProp_s("BorderTransparency");//�߿�͸����

	RtwComplexImage* pOutputImage = g_workspace.getImageFactory()->createImageSequence();

	SMargin sm(strBorder.c_str());

	pOutputImage->borderSize  = sm;

	bool bBlend;
	if (strFile.rfind(".gif") != string::npos)
	{
		bBlend = true;
	}
	else
	{
		bBlend = false;
		if (strBlend == "true")
			bBlend = true;
		else if (strBlend == "false")
			bBlend = false;
	}

	Real transparency = 0;
	if (!strTransparency.empty())
		transparency = atof(strTransparency.c_str());

	RtwImage* pImage = g_workspace.getImageFactory()->createImage(strFile);
	if (pImage)
	{
		RtwRect rcSource;
		if (!strSource.empty())
		{
			if (rcSource.FromString(strSource))
			{
				pImage->SetSourceRect(rcSource);//modify by fox
			}
		}
		pImage->SetBlend(bBlend);
		pImage->SetTransparency(transparency);
		pOutputImage = (RtwComplexImage*)pImage;
	}

	bBlend = false;
	if (strBorderBlend == "true")
		bBlend = true;
	else if (strBorderBlend == "false")
		bBlend = false;

	transparency = 0;
	if (!strBorderTransparency.empty())
		transparency = atof(strBorderTransparency.c_str());

	RtwImage* pBorderImage = g_workspace.getImageFactory()->createImage(strBorderFile);
	if (pBorderImage)
	{
		RtwRect rcBorderSource;
		if (!strBorderSource.empty())
		{
			if (rcBorderSource.FromString(strBorderSource))
			{
				pBorderImage->SetSourceRect(rcBorderSource);//modify by fox
			}
		}
		pBorderImage->SetBlend(bBlend);
		pBorderImage->SetTransparency(transparency);
		pOutputImage->border = pBorderImage;
	}

	return pOutputImage;
}

//����ͼ
bool RtwXmlLoader::_Process_BackgroundImage(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwComplexImage* pImage = _Process_ComplexImage(pNode);
	//UI_ENSURE_B(pImage);
	pWidget->SetBackgroundImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

//��ʾ����
bool RtwXmlLoader::_Process_Hint(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	std::string strForm = pNode->GetProp_s("Form");
	std::string strText = pNode->GetProp_s("Text");

	// �Զ���Hint��־
	pWidget->ModifyFlags(wfHint, 0);

	if (!strForm.empty())	//���form��Ϊ��,��ʹ���ṩ��form����Ϊhint�Ĵ���
	{
		RtwWidget* pHint = NULL;
		if (!g_workspace.FindWidget(strForm, &pHint))
		{
			RtCoreLog().Error("[RtwXmlLoader::_Process_Hint] Cann't find hint form(Name=%s)\n", strForm.c_str());
			return false;
		}
		pWidget->SetHint(pHint);
		DROP_RTUI_OBJECT(pHint);
	}
	else 
	{
		//Ĭ��hint
		pWidget->SetHint(g_workspace.getDefaultHint());
	}

	if (!strText.empty())
	{
		pWidget->SetHintText(strText);
	}

	return true;
}

//����ê��
bool RtwXmlLoader::_Process_Anchors(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Anchor")
		{
			_Process_Anchor(pWidget, pChildNode);
		}

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_Process_Anchor(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	SUiAnchor Anchor;
	RtwWidget* pRelativeWidget = NULL;

	std::string strPoint		= pNode->GetProp_s("point");
	std::string strRelativeTo	= pNode->GetProp_s("relativeTo");
	std::string strRelativePoint= pNode->GetProp_s("relativePoint");
	std::string strOffset		= pNode->GetProp_s("offset");

	if (strPoint == "LEFT")
		Anchor.Point = anchorLeft;
	else if (strPoint == "RIGHT")
		Anchor.Point = anchorRight;
	else if (strPoint == "TOP")
		Anchor.Point = anchorTop;
	else if (strPoint == "BOTTOM")
		Anchor.Point = anchorBottom;
	else if (strPoint == "CENTERV")
		Anchor.Point = anchorCenterV;
	else if (strPoint == "CENTERH")
		Anchor.Point = anchorCenterH;
	else 
		return false;

	if (strRelativePoint.empty())
		strRelativePoint = strPoint;

	if (strRelativePoint == "LEFT")
		Anchor.RelativePoint = anchorLeft;
	else if (strRelativePoint == "RIGHT")
		Anchor.RelativePoint = anchorRight;
	else if (strRelativePoint == "TOP")
		Anchor.RelativePoint = anchorTop;
	else if (strRelativePoint == "BOTTOM")
		Anchor.RelativePoint = anchorBottom;
	else if (strRelativePoint == "CENTERV")
		Anchor.RelativePoint = anchorCenterV;
	else if (strRelativePoint == "CENTERH")
		Anchor.RelativePoint = anchorCenterH;
	else 
		return false;

	if (!strRelativeTo.empty())
	{
		if (!g_workspace.FindWidget(strRelativeTo, &pRelativeWidget))
			return false;

		Anchor.RelativeWidgetName = strRelativeTo;
	}

	if (!strOffset.empty())
	{
		SSize offset;
		if (!offset.FromString(strOffset))
			return false;
		Anchor.Offset = offset;
	}

	pWidget->AddAnchor(Anchor, pRelativeWidget);
	DROP_RTUI_OBJECT(pRelativeWidget);

	return true;
}

//����߿�(��ɫ)
bool RtwXmlLoader::_Process_Border(RtwWidget* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	std::string strSize		= pNode->GetProp_s("size");
	std::string strColor	= pNode->GetProp_s("color");

	if (!strSize.empty())
	{
		int BorderSize = atoi(strSize.c_str());
		pWidget->SetBorderSize(BorderSize);
	}

	if (!strColor.empty())
	{
		DWORD color = strtoul(strColor.c_str()+1, NULL, 16); // "#bbffffff"
		pWidget->SetBorderColor(color);
	}

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		pChildNode = pChildNode->pNext;
	}

	return true;
}

//@��԰�ť�����⴦��
//begin
bool RtwXmlLoader::_Process_NormalImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->SetNormalImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_PushedImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->SetPushedImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_DisabledImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->SetDisabledImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_HighlightImage(RtwButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_ComplexImage(pNode);
	pWidget->SetHoverImage(pImage);
	DROP_RTUI_OBJECT(pImage);

	return true;
}

bool RtwXmlLoader::_Process_CheckedImage(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwComplexImage* pImage = _Process_ComplexImage(pNode);
	pWidget->SetCheckedImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_CheckHighlightImage(CUiCheckButton* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwComplexImage* pImage = _Process_ComplexImage(pNode);
	pWidget->SetHighlightImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_FaceButton(RtwButton* pParent, RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	m_lFaceButton.clear();
	while (pChildNode)
	{
		RtwButton* facebutton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
		if (pChildNode->strName == "Icon") //ͷ��?
		{
			RtwComplexImage* pImage = _Process_ComplexImage(pChildNode);
			facebutton->SetBackgroundImage(pImage);
			string hint = pChildNode->GetProp_s("ID");
			facebutton->SetHintText(hint);
		}
		m_lFaceButton.push_back(facebutton);
		pChildNode = pChildNode->pNext;
	}
	return true;
}
//end

bool RtwXmlLoader::_Process_TreeNode(RtwTree* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	std::string strText		= pNode->GetProp_s("Text");	
	std::string strLink		= pNode->GetProp_s("Link");
	std::string strRootText = "";

	if (pNode->GetParent())
	{
		strRootText = pNode->GetParent()->GetProp_s("Text");
	}

	if (strText != "")
	{
		if(strRootText == "")
			pWidget->InsertTreeItem(NULL, strText.c_str(), strLink.c_str());
		else
			pWidget->InsertTreeItem(strRootText.c_str(),strText.c_str(),strLink.c_str());
	}

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		_Process_TreeNode(pWidget,pChildNode);
		pChildNode = pChildNode->pNext;
	}

	return true;

}
bool RtwXmlLoader::_Process_TabItem(RtwTab* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	CUiCheckButton* pCheckButton = NULL;
	RtwForm* pForm = NULL;

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "CheckButton")
		{
			UI_ENSURE_B(pCheckButton = _Process_CheckButton(pWidget, NULL, pChildNode))
		}
		else if (pChildNode->strName == "Form")
		{
			UI_ENSURE_B(pForm = _Process_Form(pWidget, NULL, pChildNode))
		}

		pChildNode = pChildNode->pNext;
	}

	UI_ENSURE_B(pCheckButton && pForm);
	UI_ENSURE_B(pWidget->AddTabItem(pCheckButton, pForm));

	return true;
}

bool RtwXmlLoader::_Process_TitleBar(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode)
{
	pForm->SetTitleText(pNode->GetProp_s("Text"));

	std::string strTitleBarHeight	= pNode->GetProp_s("Height");	//�������߶�
	std::string strShow				= pNode->GetProp_s("Show");		//�Ƿ���ʾ��

	int TitleBarHeight = atoi(strTitleBarHeight.c_str());
	if (TitleBarHeight > 0)
		pForm->SetTitleBarHeight(TitleBarHeight);

	if (strShow == "true")
		pForm->SetShowTitleBar(true, false);
	else if (strShow == "false")
		pForm->SetShowTitleBar(false, false);

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "TitleImage")
		{
			_Process_TitleImage(pForm, pChildNode);
		}

		pChildNode = pChildNode->pNext;
	}

	return true;
}

bool RtwXmlLoader::_Process_TitleImage(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pForm->SetTitleImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_CloseButton(RtwForm* pForm, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pForm->getCloseButton(), pNode));

	// ˢ��һ�£�����CloseButton�ȵ�λ�ó���
	pForm->Refresh();

	pForm->SetShowCloseButton(pForm->getCloseButton()->getVisible());

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pForm->getCloseButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_DropdownButton(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget->getDropdownButton(), pNode));

	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget->getDropdownButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_ComboEditor(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	RtwTextBox* pTmp = pWidget->getEditBox();
	UI_ENSURE_P(_ProcessAttrib_EditBox(pWidget->getEditBox(), pNode));

	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_EditBox(pWidget->getEditBox(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_ComboList(RtwComboBox* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Item")
		{
			std::string text = pChildNode->GetProp_s("Text");
			if (text != "")
				pWidget->AddItem(text);
		}
		if (pChildNode->strName == "CheckButton")
		{
			std::string text = pChildNode->GetProp_s("Text");
			if (text != "")
				pWidget->AddItem(text);
		}
		pChildNode = pChildNode->pNext;

	}
	return true;
}

//���������м����
bool RtwXmlLoader::_Process_HandleImage(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->getScrollHandle()->SetBackgroundImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_ProgressBarImage(RtwProgressBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->SetBarImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_TopButton(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget->getStepUpButton(), pNode));

	// ˢ��һ�£�����Buttonλ�ó���
	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget->getStepUpButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_BottomButton(RtwVScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget->getStepDownButton(), pNode));

	// ˢ��һ�£�����Buttonλ�ó���
	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget->getStepDownButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_HandleImage(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	RtwImage* pImage = _Process_Image(pNode);
	pWidget->getScrollHandle()->SetBackgroundImage(pImage);
	DROP_TRUI_IMAGE(pImage);

	return true;
}

bool RtwXmlLoader::_Process_LeftButton(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget->getStepLeftButton(), pNode));

	// ˢ��һ�£�����Buttonλ�ó���
	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget->getStepLeftButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::_Process_RightButton(RtwHScrollBar* pWidget, RtsXmlDoc::NodePtr* pNode)
{
	// ����
	UI_ENSURE_P(_ProcessAttrib_Button(pWidget->getStepRightButton(), pNode));

	// ˢ��һ�£�����Buttonλ�ó���
	pWidget->Refresh();

	// ���е���Widget
	UI_ENSURE_P(_ProcessChild_Button(pWidget->getStepRightButton(), NULL, pNode));

	return true;
}

bool RtwXmlLoader::MergeRtXmlNode(RtsXmlDoc::NodePtr* pNodeTo, RtsXmlDoc::NodePtr* pNodeFrom)
{
	UI_ENSURE_B(pNodeFrom && pNodeTo);

	UI_ENSURE_B(_MergeRtXmlNode(pNodeTo, pNodeFrom));

	return true;
}

bool RtwXmlLoader::_MergeRtXmlNode(RtsXmlDoc::NodePtr* pNodeTo, RtsXmlDoc::NodePtr* pNodeFrom)
{
	// ������������
	std::map<std::string, std::string>::iterator iterAttrib = pNodeFrom->mapAttribute.begin();
	for (; iterAttrib != pNodeFrom->mapAttribute.end(); ++iterAttrib)
	{
		const std::string& AttName = iterAttrib->first;
		const std::string& AttValue = iterAttrib->second;
		std::map<std::string, std::string>::iterator iterAttFind = pNodeTo->mapAttribute.find(AttName);
		if (iterAttFind == pNodeTo->mapAttribute.end())
			pNodeTo->mapAttribute[AttName] = AttValue;
	}

	// �������ж���Node
	RtsXmlDoc::NodePtr* pFromChild = pNodeFrom->pChildren;
	while (pFromChild)
	{
		RtsXmlDoc::NodePtr* pToChild = pNodeTo->pChildren;
		// �����Ƿ����ͬ��Node
		RtsXmlDoc::NodePtr* pToChildLast = pToChild;
		while (pToChild)
		{
			if (pToChild->strName == pFromChild->strName)
			{
				break;
			}

			pToChildLast = pToChild;

			pToChild = pToChild->pNext;
		}
		// ���δ�ҵ��ʹ���
		if (!pToChild)
		{
			_CreateRtXmlNode(&pToChild);
			pToChild->strName = pFromChild->strName;
			pToChild->pParent = pNodeTo;
			if (pToChildLast)
				pToChildLast->pNext = pToChild;
			else 
				pNodeTo->pChildren = pToChild;
		}
		_MergeRtXmlNode(pToChild, pFromChild);

		pFromChild = pFromChild->pNext;
	}

	return true;
}

bool RtwXmlLoader::_CreateRtXmlNode(RtsXmlDoc::NodePtr** ppRtXmlNode)
{
	UI_CHECK_B(ppRtXmlNode && !*ppRtXmlNode);
	(*ppRtXmlNode) = RT_NEW RtsXmlDoc::NodePtr();
	(*ppRtXmlNode)->strName		= "";
	(*ppRtXmlNode)->pNext		= NULL;
	(*ppRtXmlNode)->pParent		= NULL;
	(*ppRtXmlNode)->pChildren	= NULL;
	return true;
}

bool RtwXmlLoader::_DeleteRtXmlNode(RtsXmlDoc::NodePtr** ppRtXmlNode)
{
	if (!ppRtXmlNode || !(*ppRtXmlNode))
		return true;

	if ((*ppRtXmlNode)->pChildren)
	{
		_DeleteRtXmlNode(&((*ppRtXmlNode)->pChildren));
		(*ppRtXmlNode)->pChildren = NULL;
	}

	if ((*ppRtXmlNode)->pNext)
	{
		_DeleteRtXmlNode(&((*ppRtXmlNode)->pNext));
		(*ppRtXmlNode)->pNext = NULL;
	}

	DEL_ONE(*ppRtXmlNode);
	return true;
}

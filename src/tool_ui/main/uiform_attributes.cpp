#include "studio.h"


#define   BUTTON_OFFSET_CUSTOM   ( 100 )

static RtwTree::Item*  gspkLocationOwner = NULL;


UiForm_Attributes::UiForm_Attributes()
                 :m_pCmb_TextureSelect( NULL ),
				  m_pEdt_TextureSelect( NULL ),
				  m_pEdt_SaveLastInstead(NULL),
				  m_pEdt_SaveTextureSelect( NULL )
{
	/*LOG("");*/
   m_pFrm_This = NULL;
    for (int i = 0; i < m_AttribCountPerPage; i++)
    {
        m_pCmb_AttName[i] = NULL;
        m_pEdt_AttValue[i] = NULL;
        m_pBtn_Del[i] = NULL;
    }
}

UiForm_Attributes::~UiForm_Attributes()
{
	/*LOG("");*/
    DROP_RTUI_OBJECT(m_pFrm_This);
    for (int i = 0; i < m_AttribCountPerPage; i++)
    {
        DROP_RTUI_OBJECT(m_pCmb_AttName[i]);
        DROP_RTUI_OBJECT(m_pEdt_AttValue[i]);
        DROP_RTUI_OBJECT(m_pBtn_Del[i]);
    }	
	DROP_RTUI_OBJECT( m_pEdt_SaveTextureSelect );
	DROP_RTUI_OBJECT( m_pCmb_TextureSelect );
	DROP_RTUI_OBJECT( m_pEdt_TextureSelect );
	DROP_RTUI_OBJECT( m_pEdt_SaveLastInstead );
	gspkLocationOwner = NULL;
	
}

bool UiForm_Attributes::Init()
{
	/*LOG("");*/
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes", (RtwWidget**)&m_pFrm_This));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName1", (RtwWidget**)&m_pCmb_AttName[0]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName2", (RtwWidget**)&m_pCmb_AttName[1]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName3", (RtwWidget**)&m_pCmb_AttName[2]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName4", (RtwWidget**)&m_pCmb_AttName[3]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName5", (RtwWidget**)&m_pCmb_AttName[4]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName6", (RtwWidget**)&m_pCmb_AttName[5]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName7", (RtwWidget**)&m_pCmb_AttName[6]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName8", (RtwWidget**)&m_pCmb_AttName[7]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName9", (RtwWidget**)&m_pCmb_AttName[8]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName10", (RtwWidget**)&m_pCmb_AttName[9]));
	CHECK(g_workspace.FindWidget("layStudio.frmAttributes.AttName100", (RtwWidget**)&m_pCmb_TextureSelect));
	m_pCmb_TextureSelect->SetAutoScrollV( true );
	m_pCmb_TextureSelect->SetAutoScrollH( true );
	CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att100", (RtwWidget**)&m_pEdt_TextureSelect));
	m_pEdt_SaveTextureSelect = m_pEdt_TextureSelect;
	CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att1", (RtwWidget**)&m_pEdt_AttValue[0]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att2", (RtwWidget**)&m_pEdt_AttValue[1]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att3", (RtwWidget**)&m_pEdt_AttValue[2]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att4", (RtwWidget**)&m_pEdt_AttValue[3]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att5", (RtwWidget**)&m_pEdt_AttValue[4]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att6", (RtwWidget**)&m_pEdt_AttValue[5]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att7", (RtwWidget**)&m_pEdt_AttValue[6]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att8", (RtwWidget**)&m_pEdt_AttValue[7]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att9", (RtwWidget**)&m_pEdt_AttValue[8]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.Att10", (RtwWidget**)&m_pEdt_AttValue[9]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel1", (RtwWidget**)&m_pBtn_Del[0]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel2", (RtwWidget**)&m_pBtn_Del[1]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel3", (RtwWidget**)&m_pBtn_Del[2]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel4", (RtwWidget**)&m_pBtn_Del[3]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel5", (RtwWidget**)&m_pBtn_Del[4]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel6", (RtwWidget**)&m_pBtn_Del[5]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel7", (RtwWidget**)&m_pBtn_Del[6]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel8", (RtwWidget**)&m_pBtn_Del[7]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel9", (RtwWidget**)&m_pBtn_Del[8]));
    CHECK(g_workspace.FindWidget("layStudio.frmAttributes.BtnDel10", (RtwWidget**)&m_pBtn_Del[9]));

    for (int i = 0; i < m_AttribCountPerPage; i++)
    {
        m_pCmb_AttName[i]->EvUpdateText += RTW_CALLBACK(this, UiForm_Attributes, OnUpdate_AttribName);
        m_pCmb_AttName[i]->EvDropDown += RTW_CALLBACK(this, UiForm_Attributes, OnDropDown_AttribName);
        m_pEdt_AttValue[i]->EvUpdateText += RTW_CALLBACK(this, UiForm_Attributes, OnUpdate_AttribValue);
        m_pEdt_AttValue[i]->EvUnFocus += RTW_CALLBACK(this, UiForm_Attributes, OnUnFocus_AttribValue);
        m_pCmb_AttName[i]->getEditBox()->EvFocus += RTW_CALLBACK(this, UiForm_Attributes, OnFocus_AttribNameEdit);
        m_pCmb_AttName[i]->getEditBox()->EvUnFocus += RTW_CALLBACK(this, UiForm_Attributes, OnUnFocus_AttribNameEdit);
        m_pBtn_Del[i]->EvLClick += RTW_CALLBACK(this, UiForm_Attributes, OnLClick_DelBtn);
    }

	m_pCmb_TextureSelect->EvUpdateText += RTW_CALLBACK(this, UiForm_Attributes, OnSelectTextureList);
	m_pEdt_TextureSelect->EvUpdateText += RTW_CALLBACK(this, UiForm_Attributes, OnUpdate_AttribValue);

    return true;
}

void UiForm_Attributes::Refresh()
{
	/*LOG("");*/
    
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    if (!pSelectTreeItem)
    {
        for (int i = 0; i < m_AttribCountPerPage; i++)
        {
            m_pCmb_AttName[i]->Disable();
            m_pCmb_AttName[i]->SetText("");
            m_pEdt_AttValue[i]->Disable();
            m_pEdt_AttValue[i]->SetText("");
        }
        return;
    }

    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    if (!pElement)
    {
        for (int i = 0; i < m_AttribCountPerPage; i++)
        {
            m_pCmb_AttName[i]->Disable();
            m_pCmb_AttName[i]->SetText("");
            m_pEdt_AttValue[i]->Disable();
            m_pEdt_AttValue[i]->SetText("");
        }
        return;
    }    	
    const xmlpp::Element::AttributeList AttribList = pElement->get_attributes();
    int i = 0;
    ///!it's look ugly , but it works,let it go now.
	for (xmlpp::Element::AttributeList::const_iterator iterAttrib = AttribList.begin(); iterAttrib != AttribList.end(); iterAttrib++)
	{
		xmlpp::Attribute* pAttrib = *iterAttrib;
		m_pCmb_AttName[i]->Enable();
		m_pCmb_AttName[i]->SetText(pAttrib->get_name());
		if( pAttrib->get_name() == "Location" )
		{
			gspkLocationOwner = pSelectTreeItem;
		}
		m_pEdt_AttValue[i]->Enable();
		m_pEdt_AttValue[i]->SetText(pAttrib->get_value());
		i++;
	}
	i = 0;
	_updateTextureSelect();
    for (xmlpp::Element::AttributeList::const_iterator iterAttrib = AttribList.begin(); iterAttrib != AttribList.end(); iterAttrib++)
    {
        xmlpp::Attribute* pAttrib = *iterAttrib;
        m_pCmb_AttName[i]->Enable();
        m_pCmb_AttName[i]->SetText(pAttrib->get_name());
        m_pEdt_AttValue[i]->Enable();
        m_pEdt_AttValue[i]->SetText(pAttrib->get_value());
        i++;
    }
	
    if (i != m_AttribCountPerPage)
    {
        m_pCmb_AttName[i]->Enable();
        m_pCmb_AttName[i]->SetText("");
        m_pEdt_AttValue[i]->Disable();
        m_pEdt_AttValue[i]->SetText("");
        i++;
    }
    for (; i < m_AttribCountPerPage; i++)
    {
        m_pCmb_AttName[i]->Disable();
        m_pCmb_AttName[i]->SetText("");
        m_pEdt_AttValue[i]->Disable();
        m_pEdt_AttValue[i]->SetText("");
    }
	
	
	
}

void UiForm_Attributes::Show()
{
	/*LOG("");*/
    m_pFrm_This->Show();
}

void UiForm_Attributes::Hide()
{
	/*LOG("");*/
    m_pFrm_This->Hide();
}

void UiForm_Attributes::Run(DWORD dwDelta)
{	/*LOG("");*/

}

bool UiForm_Attributes::IsVisible()
{
	/*LOG("");*/
    return m_pFrm_This->getVisible();
}

void UiForm_Attributes::OnUpdate_AttribValue(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem);
    UI_ENSURE(pElement);
    RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem); //(RtwWidget*)pSelectTreeItem->Param1;
	int i=0;
    for (; i < m_AttribCountPerPage; i++)
    {
        if (pWidget == m_pEdt_AttValue[i])
            break;
    }

    // Remove Original Attrib
    g_pUiMain->m_pLay_Studio->m_UiDocument.RemoveAttribute(m_OperateAttName, pSelectTreeItem, pOperateWidget);

    std::string AttribName = m_pCmb_AttName[i]->getText();
    std::string AttribValue = pWidget->getText();

	g_pUiMain->m_pLay_Studio->m_UiDocument.SetAttribute(AttribName, AttribValue, pSelectTreeItem);

	std::string  sSelectTreeItemName = pSelectTreeItem->text;
	_detectAndAddTextureFile( sSelectTreeItemName , AttribName , AttribValue );
}

void UiForm_Attributes::OnDropDown_AttribName(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    UI_ENSURE(pElement);

    std::vector<std::string> arrAttribs;
    g_pUiMain->m_pLay_Studio->m_UiDocument.getSupportAttribs(g_pUiMain->m_pFrm_WidgetTree->GetWidgetTypeNameFromTreeItemText(pSelectTreeItem->text), arrAttribs);

    RtwComboBox* pComboBox = (RtwComboBox*)pWidget;
    pComboBox->RemoveAllItems();
    for (int i = 0; i < (int)arrAttribs.size(); i++)
    {		
        pComboBox->AddItem(arrAttribs[i]);
    }
}

void UiForm_Attributes::OnUpdate_AttribName(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{	/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    UI_ENSURE(pElement);
    RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem); //(RtwWidget*)pSelectTreeItem->Param1;

	int   i = 0;
    for ( i = 0; i < m_AttribCountPerPage; i++)
    {
        if (pWidget == m_pCmb_AttName[i])
            break;
    }
    UI_ENSURE(i < m_AttribCountPerPage);

    if (!g_pUiMain->m_pLay_Studio->m_UiDocument.FindInSupportAttribs(pElement->get_name(), pWidget->getText()))
    {
        pWidget->SetText(m_OperateAttName);
        Refresh();
        return;
    }
    else 
    {
        g_pUiMain->m_pLay_Studio->m_UiDocument.RemoveAttribute(m_OperateAttName, pSelectTreeItem, pOperateWidget);

        m_pEdt_AttValue[i]->Enable();
        m_pEdt_AttValue[i]->SetFocus();

        g_pUiMain->m_pLay_Studio->m_UiDocument.SetAttribute(m_pCmb_AttName[i]->getText(), m_pEdt_AttValue[i]->getText(), pSelectTreeItem);

        if (i + 1 < m_AttribCountPerPage)
            m_pCmb_AttName[i+1]->Enable();
    }	
}

void UiForm_Attributes::OnUnFocus_AttribValue(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    UI_ENSURE(pElement);
	int i=0;
    for (; i < m_AttribCountPerPage; i++)
    {
        if (pWidget == m_pEdt_AttValue[i])
            break;
    }
    UI_ENSURE(i < m_AttribCountPerPage);

    xmlpp::Attribute* pAttrib = pElement->get_attribute(m_pCmb_AttName[i]->getText());
    if (pAttrib)
    {
        m_pEdt_AttValue[i]->SetText(pAttrib->get_value());
    }	
}

void UiForm_Attributes::OnFocus_AttribNameEdit(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_OperateAttName = pWidget->getText();
}

void UiForm_Attributes::OnUnFocus_AttribNameEdit(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    UI_ENSURE(pElement);
	int i = 0;
    for (; i < m_AttribCountPerPage; i++)
    {
        if (pWidget == m_pCmb_AttName[i]->getEditBox())
            break;
    }
    UI_ENSURE(i < m_AttribCountPerPage);	

    if (!g_pUiMain->m_pLay_Studio->m_UiDocument.FindInSupportAttribs(pElement->get_name(), pWidget->getText()))
    {
        pWidget->SetText(m_OperateAttName);
        Refresh();
        return;
    }
}

void UiForm_Attributes::OnLClick_DelBtn(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE(pSelectTreeItem);
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem); //(xmlpp::Element*)pSelectTreeItem->Param2;
    UI_ENSURE(pElement);
    RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem); //(RtwWidget*)pSelectTreeItem->Param1;
	int i = 0;
    for (; i < m_AttribCountPerPage; i++)
    {
        if (pWidget == m_pBtn_Del[i])
            break;
    }
    UI_ENSURE(i < m_AttribCountPerPage);

    g_pUiMain->m_pLay_Studio->m_UiDocument.RemoveAttribute(m_pCmb_AttName[i]->getText(), pSelectTreeItem, pOperateWidget);

    Refresh();
	
}

void  UiForm_Attributes::_removeTextureCombobox()
{/*LOG("");*/
	for( int k = 0 ; k < m_AttribCountPerPage; k ++ )
	{
		if( m_pEdt_TextureSelect == m_pEdt_AttValue[ k ] )
		{
			m_pEdt_AttValue[ k ] = m_pEdt_SaveLastInstead;				
			m_pEdt_TextureSelect = m_pEdt_SaveTextureSelect;
			m_pEdt_TextureSelect->Hide();
		}
		m_pEdt_AttValue[ k ]->Show();
	}
	m_pCmb_TextureSelect->Hide();
	m_pEdt_SaveTextureSelect->Hide();
}

void  UiForm_Attributes::_updateTextureSelect( )
{/*LOG("");*/
	RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    _removeTextureCombobox();
	bool  bShowComBox = pSelectTreeItem->text == "BackgroundImage" ||
						pSelectTreeItem->text == "NormalImage"     ||
						pSelectTreeItem->text == "HighlightImage"  ||
						pSelectTreeItem->text == "PushedImage"  ||
						pSelectTreeItem->text == "ProgressBarImage" ||
						pSelectTreeItem->text == "CheckedImage"    ||
						pSelectTreeItem->text == "DisabledImage";
		                
	if( pSelectTreeItem && bShowComBox )
	{		
		for( int j = 0 ; j < m_AttribCountPerPage; j ++ )
		{
			std::string  sStringName = m_pCmb_AttName[ j ]->getText();
			if( m_pCmb_AttName[ j ]->getText() == "Source" )
			{	
				m_pEdt_SaveLastInstead = m_pEdt_AttValue[ j ];
				m_pEdt_AttValue[ j ]   = m_pEdt_TextureSelect;  
				m_pEdt_SaveLastInstead->Hide();
				m_pCmb_TextureSelect->Show();
				m_pEdt_TextureSelect->Show();				
				m_pEdt_TextureSelect->SetEnableInput( true );
				m_pEdt_TextureSelect->SetFocus();
				RtwRect  kFrameRect      = m_pEdt_SaveLastInstead->getFrameRect();		
				m_pEdt_TextureSelect->Move( SPoint(kFrameRect.left,kFrameRect.top));
				m_pCmb_TextureSelect->Move( SPoint(kFrameRect.left + BUTTON_OFFSET_CUSTOM,kFrameRect.top) );			
			}				
		}			
	}
}

void UiForm_Attributes::OnSelectTextureList( RtwWidget* pWidget, RtwEventDelegate* pEvent )
{/*LOG("");*/
	RtwComboBox* pComboBox   = (RtwComboBox*)pWidget;	
	std::string  sSelectName = pComboBox->getSelectItem();
	if( sSelectName.size() )
	{
		for( int i = 0 ; i < (int)m_kvTextureInforArray.size() ; i ++ )
		{
            STextureInfor&  kTextInfo  = m_kvTextureInforArray[ i ];
			if( kTextInfo.msTextureName == sSelectName )
			{
                char  cTextureUV[ 128 ];
				memset( cTextureUV , NULL , 128 );
				sprintf( cTextureUV ,"%d,%d,%d,%d" , 
					     kTextInfo.msTextureMinU,
					     kTextInfo.msTextureMinV,
						 kTextInfo.msTextureMaxU - kTextInfo.msTextureMinU,
						 kTextInfo.msTextureMaxV - kTextInfo.msTextureMinV);
				RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
				std::string  sAttribName            = "Source";
				std::string  sAttribValue( cTextureUV );				

				memset( cTextureUV , NULL , 128 );
				sprintf( cTextureUV ,"%d,%d,%d,%d" , 
					     100,
					     100,
						 kTextInfo.msTextureMaxU - kTextInfo.msTextureMinU,
						 kTextInfo.msTextureMaxV - kTextInfo.msTextureMinV);
				if( gspkLocationOwner )
				{
                    g_pUiMain->m_pLay_Studio->m_UiDocument.SetAttribute( "Location",cTextureUV,gspkLocationOwner, false);
				}				
				g_pUiMain->m_pLay_Studio->m_UiDocument.SetAttribute( sAttribName,sAttribValue,pSelectTreeItem, false);
			}
		}
	}
}

static std::string SplitFileName(const std::string& sName)
{/*LOG("");*/
	int nIndex = (int)sName.rfind( '.' ,sName.size() - 1 );
	if( nIndex != -1 && nIndex >= 0 && nIndex <= (int)sName.size() )
	{		
		return std::string( sName.data() , nIndex );
	}
	return std::string("");
}

void  UiForm_Attributes::_processTextureInformation( std::string& AttribValue )
{/*LOG("");*/
	std::string  sFileName= SplitFileName( AttribValue );

	// Modified by Wayne Wong 2010-12-10
	/*
	sFileName   += ".sg";
	FILE* pkFile = fopen( sFileName.data() , "rb" );
	if( pkFile == NULL )
	{
        std::string  kAnother = "ui\\textures\\"; 
		kAnother += sFileName.data();
		pkFile = fopen( kAnother.data() , "rb" );
	}
	m_kvTextureInforArray.clear();
	if( pkFile )
	{
		fread( &m_kTExtureInforHeader , sizeof( STextureInforHeader ) , 1 , pkFile );
		STextureInfor  kTexInfor;
		for( int i = 0 ; i < m_kTExtureInforHeader.msTotalTexture ; i ++ )
		{
            memset( &kTexInfor , 0 , sizeof( STextureInfor ));
			fread( &kTexInfor , sizeof( STextureInfor ) ,1 , pkFile );
			m_kvTextureInforArray.push_back( kTexInfor );
			m_pCmb_TextureSelect->AddItem( kTexInfor.msTextureName );
		}
		fclose( pkFile );
	}
	*/

	RtsXmlDoc doc;
	m_kTExtureInforHeader.msVersion = 0x0101;
	m_kTExtureInforHeader.msTotalTexture = 0;
	m_kvTextureInforArray.clear();
	sFileName   += ".imageset";
	if (doc.ParserFile(sFileName.data()) != 0)
	{
		std::string  kAnother = "ui\\textures\\"; 
		kAnother += sFileName.data();
		if (doc.ParserFile(kAnother.data()) != 0)
			return;
	}

	STextureInfor  kTexInfor;
	RtsXmlDoc::NodePtr* pRootNode = doc.GetRoot();
	RtsXmlDoc::NodePtr* pChildNode = pRootNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Image")
		{
			memset(&kTexInfor, 0, sizeof( STextureInfor ));
			rt2_strncpy(kTexInfor.msTextureName, pChildNode->GetProp_s("Name"), MAX_TEXTURE_NAME);
			kTexInfor.msTextureMaxU = kTexInfor.msTextureMinU = pChildNode->GetPropInt("XPos");
			kTexInfor.msTextureMaxV = kTexInfor.msTextureMinV = pChildNode->GetPropInt("YPos");
			kTexInfor.msTextureMaxU += pChildNode->GetPropInt("Width");
			kTexInfor.msTextureMaxV += pChildNode->GetPropInt("Height");
			m_kvTextureInforArray.push_back( kTexInfor );
			++m_kTExtureInforHeader.msTotalTexture;

			m_pCmb_TextureSelect->AddItem( kTexInfor.msTextureName );
		}
		pChildNode = pChildNode->pNext;
	}
}

void  UiForm_Attributes::_detectAndAddTextureFile( std::string& sSelectTreeItemName, 
												   std::string& AttribName, 
												   std::string& AttribValue)
{/*LOG("");*/
		bool  bDetectedEnable = sSelectTreeItemName == "BackgroundImage" ||
								 sSelectTreeItemName == "NormalImage"    ||
						        sSelectTreeItemName == "HighlightImage" ||
								 sSelectTreeItemName == "PushedImage"    ||
								  sSelectTreeItemName == "ProgressBarImage" ||
								   sSelectTreeItemName == "CheckedImage"    ||
						        sSelectTreeItemName == "DisabledImage";
	if( bDetectedEnable && AttribName == "File" )
	{
		m_pCmb_TextureSelect->RemoveAllItems();
		_processTextureInformation( AttribValue );		
	}
}

#pragma once

class UiForm_Attributes : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiForm_Attributes();
	~UiForm_Attributes();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

private:
    static const int m_AttribCountPerPage = 10;

    RtwForm*        m_pFrm_This;
    RtwComboBox*    m_pCmb_AttName[m_AttribCountPerPage];
    RtwTextBox*     m_pEdt_AttValue[m_AttribCountPerPage];
    RtwButton*      m_pBtn_Del[m_AttribCountPerPage];

	RtwComboBox*    m_pCmb_TextureSelect;
	RtwTextBox*     m_pEdt_TextureSelect;
	RtwTextBox*     m_pEdt_SaveLastInstead;
	RtwTextBox*     m_pEdt_SaveTextureSelect;

    void OnUpdate_AttribValue(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnDropDown_AttribName(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnUpdate_AttribName(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnUnFocus_AttribValue(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnFocus_AttribNameEdit(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnUnFocus_AttribNameEdit(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_DelBtn(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnSelectTextureList( RtwWidget* pWidget, RtwEventDelegate* pEvent );

    std::string m_OperateAttName;
    std::string m_OperateAttValue;
protected:
	void  _updateTextureSelect( );
	void  _detectAndAddTextureFile( std::string& , std::string& , std::string& );
	void  _processTextureInformation( std::string& );
	void  _removeTextureCombobox();
protected:
#define  MAX_TEXTURE_NAME   ( 64 )
	struct STextureInforHeader
	{
		short    msVersion;
		short    msTotalTexture;	
	};
	struct STextureInfor
	{
		char     msTextureName[ MAX_TEXTURE_NAME ];
		short    msTextureMinU;
		short    msTextureMinV;
		short    msTextureMaxU;
		short    msTextureMaxV;
	};
	typedef  std::vector<STextureInfor>  STextureInforArray;
	STextureInforHeader   m_kTExtureInforHeader;    
	STextureInforArray    m_kvTextureInforArray;
};


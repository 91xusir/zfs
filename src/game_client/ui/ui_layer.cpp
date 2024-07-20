#include "gc_include.h"
#include "ui_layer.h"

#include "gc_cursor.h"
#include "ui_form_textMsg.h"
#include "ui_form_msg.h"

UILayerLogin*		g_layerLogin;
UILayerSelectChar*	g_layerSelectChar;
UILayerMain*		g_layerMain;

// UiSystem:
CUiRenderDeviceImpl_Rt*     g_pUiRenderer = NULL;
IUiImageFactory*            g_pUiImageFactory = NULL;
IUiFontManager*             g_pUiFont = NULL;
IUiSound*                   g_pUiSound = NULL;
CClipboard*               g_pUiClipboard = NULL;


void UILayer::Initialize()
{
	guard;

	// 初始化UI
	g_pUiRenderer = RT_NEW CUiRenderDeviceImpl_Rt();
	g_pUiRenderer->Init(GetDevice());
	g_pUiImageFactory = RT_NEW CUiImageFactoryImpl_Rt(GetDevice());
	g_pUiFont = RT_NEW CUiFontManagerImpl_Rt(GetDevice());
	if (!g_workspace.Create(g_pUiRenderer, g_pUiImageFactory, g_pUiFont))
	{
		RtCoreLog().Error("创建UI失败\n");
		return;
	}
	g_pUiSound = RT_NEW CUiSoundImpl_Rt();
	g_workspace.SetSound(g_pUiSound);
	g_pUiClipboard = RT_NEW CUiClipboardImpl_Windows();
	g_workspace.SetClipboard(g_pUiClipboard);


/* 注释 [3/18/2009 tooth.shi]    g_workspace.SetFontName(R(RES_FONT));

	g_workspace.RegisterClass("Form", RT_RUNTIME_CLASS(RtwForm));
	g_workspace.RegisterClass("Label", RT_RUNTIME_CLASS(RtwLabel));
    g_workspace.RegisterClass("Hyperlink", RT_RUNTIME_CLASS(RtwHyperlink));
	g_workspace.RegisterClass("ProgressBar", RT_RUNTIME_CLASS(RtwProgressBar));
	g_workspace.RegisterClass("Panel", RT_RUNTIME_CLASS(RtwPanel));
	g_workspace.RegisterClass("VScrollBar", RT_RUNTIME_CLASS(RtwVScrollBar));
	g_workspace.RegisterClass("HScrollBar", RT_RUNTIME_CLASS(RtwHScrollBar));
	g_workspace.RegisterClass("ChatBox", RT_RUNTIME_CLASS(RtwChatBox));
	g_workspace.RegisterClass("ChatInputBox", RT_RUNTIME_CLASS(RtwChatInputBox));
	g_workspace.RegisterClass("Button", RT_RUNTIME_CLASS(RtwButton));
	g_workspace.RegisterClass("ListBox", RT_RUNTIME_CLASS(RtwListBox));
	g_workspace.RegisterClass("Tree", RT_RUNTIME_CLASS(RtwTree));
	g_workspace.RegisterClass("ComboBox", RT_RUNTIME_CLASS(RtwComboBox));
	g_workspace.RegisterClass("TextBox", RT_RUNTIME_CLASS(RtwTextBox));
	g_workspace.RegisterClass("MailBox", RT_RUNTIME_CLASS(RtwMailBox));
	g_workspace.RegisterClass("Tab", RT_RUNTIME_CLASS(RtwTab));
	g_workspace.RegisterClass("HtmlView", RT_RUNTIME_CLASS(RtwHtmlView));
	g_workspace.RegisterClass("AliasButton", RT_RUNTIME_CLASS(RtwAliasButton));
	g_workspace.RegisterClass("IconList", RT_RUNTIME_CLASS(RtwIconList));
	g_workspace.RegisterClass("HUD", RT_RUNTIME_CLASS(RtwHUD));
    g_workspace.RegisterClass("PopupMenu", RT_RUNTIME_CLASS(RtwPopupMenu));

	g_pDevice2D->CreateFont(R(RES_FONT), 12, 256);

	g_workspace.CreateLayer(3);
	g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);
	g_workspace.LoadTheme("ui/ui_theme.xml");	

	g_workspace.EvSetCursor += RTW_CALLBACK_F(OnSetCursor);*/

	EnterLoading();

	unguard;
}

void UILayer::EnterLoading()
{
	guard;

	Reset();	

	int iIndex = rtRandom()%4;
	float fScale = GetDevice()->m_iWndWidth / 1024.0f;
	float fHScale = GetDevice()->m_iWndHeight / 768.0f;

    float fWidth = 256*fScale;
    float fHeight = 256*fHScale;

    static string s_LoadingFiles1[] = {"ui_texture/loading/1",  "ui_texture/loading/2", "ui_texture/loading/3", "ui_texture/loading/4", "ui_texture/loading/5", "ui_texture/loading/6", \
                        "ui_texture/loading/7", "ui_texture/loading/8", "ui_texture/loading/9", "ui_texture/loading/10", "ui_texture/loading/11", "ui_texture/loading/12"};
    static string s_LoadingFiles2[] = {"ui_texture/loading/b01",  "ui_texture/loading/b02", "ui_texture/loading/b03", "ui_texture/loading/b04", "ui_texture/loading/b05", "ui_texture/loading/b06", \
                        "ui_texture/loading/b07", "ui_texture/loading/b08", "ui_texture/loading/b09", "ui_texture/loading/b10", "ui_texture/loading/b11", "ui_texture/loading/b12"};
    static string s_LoadingFiles3[] = {"ui_texture/loading/g01",  "ui_texture/loading/g02", "ui_texture/loading/g03", "ui_texture/loading/g04", "ui_texture/loading/g05", "ui_texture/loading/g06", \
                        "ui_texture/loading/g07", "ui_texture/loading/g08", "ui_texture/loading/g09", "ui_texture/loading/g10", "ui_texture/loading/g11", "ui_texture/loading/g12"};
    static string s_LoadingFiles4[] = {"ui_texture/loading/y01",  "ui_texture/loading/y02", "ui_texture/loading/y03", "ui_texture/loading/y04", "ui_texture/loading/y05", "ui_texture/loading/y06", \
                        "ui_texture/loading/y07", "ui_texture/loading/y08", "ui_texture/loading/y09", "ui_texture/loading/y10", "ui_texture/loading/y11", "ui_texture/loading/y12"};

    string *arrFiles;
    if (iIndex==0)          arrFiles = s_LoadingFiles1;
    else if (iIndex==1)     arrFiles = s_LoadingFiles2;
    else if (iIndex==2)     arrFiles = s_LoadingFiles3;
    else                    arrFiles = s_LoadingFiles4;

	vector<SequenceImageItem> arrImages;
	SequenceImageItem image;
	image = SequenceImageItem(arrFiles[0].c_str(),  0,			 0,			fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[1].c_str(),  256*fScale,  0,			fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[2].c_str(),  512*fScale,  0,			fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[3].c_str(),  768*fScale,  0,			fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[4].c_str(),  0,			256*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[5].c_str(),  256*fScale, 256*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[6].c_str(),  512*fScale, 256*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[7].c_str(),  768*fScale, 256*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[8].c_str(),  0,			512*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[9].c_str(),  256*fScale, 512*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[10].c_str(), 512*fScale, 512*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	image = SequenceImageItem(arrFiles[11].c_str(), 768*fScale, 512*fHScale, fWidth, fHeight);
	arrImages.push_back(image);
	g_LoadingMapRenderer.SetImages(arrImages);
	g_LoadingMapRenderer.SetRenderFlag(true);

	unguard;
}

void UILayer::EnterSelectServer()
{
    guard;

    //Reset();

    //g_layerLogin = new UILayerLogin;

    //g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);

    //LeaveLoading();

    unguard;
}

void UILayer::EnterLogin()
{
	guard;

	Reset();

	g_layerLogin = RT_NEW UILayerLogin;

	g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);

	LeaveLoading();

	unguard;
}

void UILayer::EnterSelectChar()
{
	guard;

	Reset();

	g_layerSelectChar = RT_NEW UILayerSelectChar;
	g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);

	LeaveLoading();

	unguard;
}

void UILayer::EnterMain()
{
	guard;

	Reset();

	g_layerMain = RT_NEW UILayerMain;

	g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);

	unguard;
}

void UILayer::Clear()
{
	Reset();
	//g_workspace.Destroy();

 //   SafeDelete(g_pUiRenderer);
 //   SafeDelete(g_pUiImageFactory);
 //   SafeDelete(g_pUiFont);
 //   SafeDelete(g_pUiSound);
 //   SafeDelete(g_pUiClipboard);
}

void UILayer::Reset()
{
	guard;

	if (g_layerLogin)
	{
		DEL_ONE(g_layerLogin);
	}

	if (g_layerSelectChar)
	{
		DEL_ONE(g_layerSelectChar);
	}

	if (g_layerMain)
	{
		DEL_ONE(g_layerMain);
	}

	//<add by fox>
	UIFormTextMsg::Clear();
	UIFormMsg::Clear(false);
	//<add by fox>

	unguard;
}

void UILayer::OnSetCursor(void*, void*)
{
	guard;

	if (g_workspace.IsMouseMonopolist())
		GameSetCursor(GAME_CURSOR_NORMAL);
	else
		GameSetCursor();

	unguard;
}

void UILayer::LeaveLoading()
{
	g_LoadingMapRenderer.SetRenderFlag(false);

	if (g_workspace.GetLayer(2))
		g_workspace.CloseLayer(2);
}

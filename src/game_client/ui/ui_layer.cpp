#include "gc_include.h"
#include "ui_layer.h"
#include "gc_cursor.h"
#include "ui_form_textMsg.h"
#include "ui_form_msg.h"

static void Quit() {
    PostQuitMessage(0);
}

void global_closeApp(void*, void*, void*) {
    UIFormMsg* pConfirm = UIFormMsg::ShowStatic("确定退出游戏吗?", UIFormMsg::TYPE_OK_CANCEL);
    pConfirm->EvOK += (UI_DELEGATE_F(Quit));
}



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

	EnterLoading();
	unguard;
}

void UILayer::EnterLoading()
{
	guard;
	//重置UILayer
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

	static string* s_LoadingFiles[] = { s_LoadingFiles1, s_LoadingFiles2, s_LoadingFiles3, s_LoadingFiles4 };
	string* arrFiles = s_LoadingFiles[iIndex];


	// 初始化图片序列
	vector<SequenceImageItem> arrImages;
	for (int i = 0; i < 12; ++i)
	{
		float x = (i % 4) * 256 * fScale;
		float y = (i / 4) * 256 * fHScale;
		SequenceImageItem image(arrFiles[i].c_str(), x, y, fWidth, fHeight);
		arrImages.push_back(image);
	}

	g_LoadingMapRenderer.SetImages(arrImages);
	g_LoadingMapRenderer.SetRenderFlag(true);
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
    //重置删除之前的UI
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

//lyytodo 似乎没用到
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

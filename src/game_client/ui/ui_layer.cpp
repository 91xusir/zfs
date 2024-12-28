#include "gc_include.h"
#include "ui_layer.h"
#include "gc_cursor.h"
#include "ui_form_textMsg.h"
#include "ui_form_msg.h"
#include <WinUser.h>
#include <thread>
#include <atomic>
#include <chrono>

UILayerLogin*      g_layerLogin;
UILayerSelectChar* g_layerSelectChar;
UILayerMain*       g_layerMain;
// UiSystem:
CUiRenderDeviceImpl_Rt* g_pUiRenderer     = NULL;
IUiImageFactory*        g_pUiImageFactory = NULL;
IUiFontManager*         g_pUiFont         = NULL;
IUiSound*               g_pUiSound        = NULL;
CClipboard*             g_pUiClipboard    = NULL;

CUiLayer* g_loadingLayer = nullptr;

namespace {
static std::atomic<double> g_dLoadingValue(0.0);  // �����̰߳�ȫ�ظ��¼���ֵ

static void UpdateLoadingBar(RtwProgressBar* pBar, double increment) {

    const std::chrono::milliseconds updateInterval(50);  // ����Ƶ�ʸ��ߣ�ʱ�����϶�
    while (g_dLoadingValue < 1.0) {
        if (!pBar || !g_loadingLayer)
            return;
        g_dLoadingValue = g_dLoadingValue + increment;  // ÿ�����ӵ�ֵ�����Ը�����Ҫ������ֵ�ٶ�
        pBar->SetValue(g_dLoadingValue.load());       // ���ý�����ֵ
        std::this_thread::sleep_for(updateInterval);  // ���Ƹ���Ƶ��
    }
    pBar->SetText("�������");
    g_dLoadingValue = 1.0;
    pBar->SetValue(g_dLoadingValue.load());
    std::this_thread::sleep_for(updateInterval);

    g_loadingLayer->Hide();
}
}  // namespace

static void Quit() {
    PostMessage(GetDevice()->GetHWND(), WM_USER_QUIT, 0, 0);
}

void global_closeApp(void*, void*, void*) {
    if (g_dLoadingValue.load() != 1.0) {
        Quit();
        return;
    }
    UIFormMsg* pConfirm = UIFormMsg::ShowStatic("ȷ���˳���Ϸ��?", UIFormMsg::TYPE_OK_CANCEL);
    pConfirm->EvOK += (UI_DELEGATE_F(Quit));
}

void UILayer::Initialize() {
    guard;
    // ��ʼ��UI
    g_pUiRenderer = RT_NEW CUiRenderDeviceImpl_Rt();
    g_pUiRenderer->Init(GetDevice());
    g_pUiImageFactory = RT_NEW CUiImageFactoryImpl_Rt(GetDevice());
    g_pUiFont         = RT_NEW         CUiFontManagerImpl_Rt(GetDevice());
    if (!g_workspace.Create(g_pUiRenderer, g_pUiImageFactory, g_pUiFont)) {
        RtCoreLog().Error("����UIʧ��\n");
        return;
    }
    g_pUiSound = RT_NEW CUiSoundImpl_Rt();
    g_workspace.SetSound(g_pUiSound);
    g_pUiClipboard = RT_NEW CUiClipboardImpl_Windows();
    g_workspace.SetClipboard(g_pUiClipboard);
    EnterLoading(0.03);
    unguard;
}

//-----------------����ҳ��-----------------add by lyy

void UILayer::EnterLoading(double increment /* = 0.01*/) {
    guard;
    auto iWndWidth  = GetDevice()->m_iWndWidth;
    auto iWndHeight = GetDevice()->m_iWndHeight;

    if (!g_loadingLayer) {
        // ����������������ͼ��
        RtwImage* progress_bk =
            g_workspace.getImageFactory()->createImage("ui/loading_bg/progress_bk.dds");
        RtwImage* progress_fk =
            g_workspace.getImageFactory()->createImage("ui/loading_bg/progress_fk.dds");
        progress_bk->SetBlend(true);
        progress_fk->SetBlend(true);
        progress_fk->SetSourceRect(RtwRect(113, 43, 1128, 54));
        RtwProgressBar* pBar = static_cast<RtwProgressBar*>(
            g_workspace.getWidgetFactory()->createWidget(wtProgressBar, "ldBar"));
        RtwLabel* pBarBG = static_cast<RtwLabel*>(
            g_workspace.getWidgetFactory()->createWidget(wtLabel, "ldBarBG"));
        pBarBG->SetBackgroundImage(progress_bk);
        pBar->SetBarImage(progress_fk);
        pBarBG->MoveResize(0, iWndHeight - 100, iWndWidth, iWndHeight);
        pBar->MoveResize(85, iWndHeight - 60, 950, iWndHeight - 35);

        DROP_RTUI_OBJECT(progress_bk);
        DROP_RTUI_OBJECT(progress_fk);
        RtwForm* bgFrom =
            (RtwForm*)g_workspace.getWidgetFactory()->createWidget(wtForm, "LdBGForm");
        bgFrom->MoveResize(0, 0, iWndWidth, iWndHeight);
        g_loadingLayer =
            (CUiLayer*)g_workspace.getWidgetFactory()->createWidget(wtLayer, "ldLayer");
        g_workspace.AddLayer(g_loadingLayer, false);

        bgFrom->AddChild(pBarBG);
        bgFrom->AddChild(pBar);
        bgFrom->SetMovable(false);
        g_loadingLayer->AddChild(bgFrom);
    }

    int   pcCount = 1;
    RtIni lodeIni;
    if (lodeIni.OpenFile("ui/loading_bg/loading.ini")) {
        lodeIni.CloseFile();
        lodeIni.GetEntry("main", "pc_count", &pcCount);
    }
    std::srand(std::time(0));
    int iIndex = std::rand() % pcCount + 1;

    std::string pcName = "ui/loading_bg/r/" + std::to_string(iIndex) + ".jpg";
    RtwImage*   pImage = g_workspace.getImageFactory()->createImage(pcName);
    pImage->SetBlend(true);

    RtwWidget* bgFrom = nullptr;
    g_workspace.FindWidget("ldLayer.LdBGForm", &bgFrom);
    if (bgFrom) {
        ((RtwForm*)bgFrom)->SetBackgroundImage(pImage);
    }
    DROP_RTUI_OBJECT(pImage);

    RtwWidget* pBar = nullptr;
    g_dLoadingValue = 0.0;
    g_workspace.FindWidget("ldLayer.LdBGForm.ldBar", &pBar);
    if (pBar) {
        ((RtwProgressBar*)pBar)->SetValue(0.0);
        ((RtwProgressBar*)pBar)->SetText("���ڼ�����...");
    }

    std::thread loadingThread(UpdateLoadingBar, (RtwProgressBar*)pBar, increment);
    loadingThread.detach();

    g_loadingLayer->Show();

    unguard;
}

void UILayer::LeaveLoading() {}

void UILayer::EnterLogin() {
    guard;
    Reset();
    g_layerLogin = RT_NEW UILayerLogin;
    g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);
    unguard;
}

void UILayer::EnterSelectChar() {
    guard;
    //����ɾ��֮ǰ��UI
    Reset();
    g_layerSelectChar = RT_NEW UILayerSelectChar;
    g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);
    unguard;
}

void UILayer::EnterMain() {
    guard;
    Reset();
    g_layerMain = RT_NEW UILayerMain;
    g_workspace.AdjustLayer(GetDevice()->m_iWndWidth, GetDevice()->m_iWndHeight);
    unguard;
}

void UILayer::Clear() {
    Reset();
    DROP_RTUI_OBJECT(g_loadingLayer);
    //g_workspace.Destroy();
    //   SafeDelete(g_pUiRenderer);
    //   SafeDelete(g_pUiImageFactory);
    //   SafeDelete(g_pUiFont);
    //   SafeDelete(g_pUiSound);
    //   SafeDelete(g_pUiClipboard);
}

void UILayer::Reset() {
    guard;

    if (g_layerLogin) {
        DEL_ONE(g_layerLogin);
    }

    if (g_layerSelectChar) {
        DEL_ONE(g_layerSelectChar);
    }

    if (g_layerMain) {
        DEL_ONE(g_layerMain);
    }

    //<add by fox>
    UIFormTextMsg::Clear();
    UIFormMsg::Clear(false);
    //<add by fox>

    unguard;
}

//lyytodo �ƺ�û�õ�
void UILayer::OnSetCursor(void*, void*) {
    guard;

    if (g_workspace.IsMouseMonopolist())
        GameSetCursor(GAME_CURSOR_NORMAL);
    else
        GameSetCursor();

    unguard;
}

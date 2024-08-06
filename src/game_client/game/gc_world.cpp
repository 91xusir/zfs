
#include "gc_include.h"
#include "region_client.h"
#include "game_assistant.h"
#include "game_states_machine.h"
#include "game_script_interface.h"
#include "gc_cursor.h"
#include "gc_beeline_missile.h"
#include "gc_track_missile.h"
#include "gc_bomb_missile.h"
#include "gc_track_bomb_missile.h"
#include "item_net_define.h"
#include "net_command_union.h"
#include "gc_constant.h"
#include "gc_login_session.h"
#include "lgs_gws.h"
#include "ui_form_trade.h"
#include "ui_form_union.h"
#include "ui_form_minimap.h"
#include "ui_form_char.h"
#include "ui_form_task.h"
#include "ui_form_item_update.h"
#include "UIForm_Gem.h"
#include "ui_form_shop.h"
#include "ui_form_bank.h"
#include "ui_form_union_create.h"
#include "ui_form_feod_bank.h"
#include "ui_form_feod_content.h"
#include "ui_form_feod_tax.h"
#include "ui_form_pkmode.h"
#include "ui_form_itemsinfo.h"
#include "ui_form_cre_info.h"
#include <tlhelp32.h>
#include "ui_form_msg.h"
#include "ui_form_playeritems.h"
#include "ui_form_dupe.h"
#include "ui_form_analyse_image.h"
#include "ui_form_compose.h"
#include "ui_form_timebar.h"
#include "ui_form_fuse.h"
#include "ui_form_vendor.h"
#include "ui_form_bid.h"
#include "ui_form_sale.h"
#include "ui_form_playershop.h"
#include "ui_form_lookplayershop.h"
#include "gc_bat.h"
#include "ui_form_peteat.h"
#include "ui_form_trump.h"
#include "UIForm_MiddleMap.h"
#include "ui_form_pet_include.h"
#include "UIForm_Char.h"
#include "ui_form_skill.h"
#include "ui_form_setting.h"
#include <string>
#include <mmsystem.h> 
extern RtsPathFind* g_pPathFind;
extern BOOL         bRenderUI;

static RT_TYPE_CS m_csCheckProcess;
static HANDLE     s_hGetProcessModuleThread = NULL;  // �߳̾��
//static char s_strCheckProcessName[200];
G_MEMDEF(s_strCheckProcessName, 200)
static bool  s_bFoundData = false;
static bool  s_bFoundProcess = false;
int          m_lastTouchCursor;
static float fMouseSpeed = 10.0f;
/* gao 2009.12.2 */
static float nowY = 45;
static float fps = 0.0;
static bool  bCanHideUI = false;
static bool  bShowFps = false;

static float dwShowTime = 0.f;  //������ʾ��ʾʱ��
static int   hitCount = 0;      //�������������

void GetHideConfig() {
    RtIni iniGame;
    if (!iniGame.OpenFile("clt_graph.ini")) {
        bCanHideUI = false;
        return;
    }

    if (!iniGame.GetEntry("Graph", "EnableUI", &bCanHideUI)) {
        bCanHideUI = false;
    }

    iniGame.CloseFile();
}

/* gao */
extern BOOL bRenderUI;

extern "C" unsigned long GM_PacketLength(unsigned char* gamePacket) {
    unsigned short wLen1, wLen2;
    memcpy(&wLen1, gamePacket, 2);
    memcpy(&wLen2, gamePacket + wLen1 + 2, 2);
    return wLen1 + BitToByte(wLen2) + 4;
}

extern "C" unsigned long GM_PacketCommand(unsigned char* gamePacket) {
    unsigned short cmdID;
    memcpy(&cmdID, gamePacket + 2, 2);
    return cmdID;
}

extern "C" bool GM_IsPacketMustEncrypt(unsigned char* gamePacket) {
    unsigned short cmdID;
    memcpy(&cmdID, gamePacket + 2, 2);
    return (cmdID == c2r_update_position || cmdID == c2r_attack);
}

bool PlayerCannotDoAction(GcActor* pActor) {
    bool bResult = pActor->m_bPShop != 0;  // ��̯
    if (!bResult && g_layerMain) {
        RtwWidget* pCapture = g_workspace.getMouseCapture();
        if (pCapture) {
            if (pCapture->GetFlags(wfPenetrate)) {
                bResult = false;
            } else {
                bResult = g_layerMain->m_formTrade->IsShowing()           // ����
                          || g_layerMain->m_formMwAddMp->GetVisible()     // ������MP
                          || g_layerMain->m_formMwAddEle->GetVisible()    // ������������
                          || g_layerMain->m_formTrump->IsVisiable()       // ������������
                          || g_layerMain->m_formRecoverPet->GetVisible()  // �������
                          || g_layerMain->m_formItemUpdate->IsVisible()   // �������
                          || g_layerMain->m_formGem->IsVisible()          // ��ʯ�ϳ�
                          || g_layerMain->m_formShop->IsVisible()         // NPC����
                          || g_layerMain->m_formBank->IsVisible()         // �ֿ�
                          || g_layerMain->m_formUnionCreate->IsVisible()  // ��������
                          || g_layerMain->m_formFeodBank->IsVisible()     // ���ع���
                          || g_layerMain->m_formFeodContent->IsVisible()  // ������֧��
                          || g_layerMain->m_formFeodTax->IsVisible()      // ����˰��
                          || g_layerMain->m_formItemCompose->IsVisible()  // ���ߺϳ�
                          || g_layerMain->m_formVendor->IsVisible() ||
                          g_layerMain->m_formBid->IsVisible() ||
                          g_layerMain->m_formSale->IsVisible() ||
                          g_layerMain->m_formHelp->IsVisible()  // ����Ի�
                          || g_layerMain->m_formPetEat->IsVisible() ||
                          LOAD_UI("forautostate")->IsVisible()  //ac.ma  �Զ�ս��״̬������
                          || g_workspace.getMouseCapture();     //������϶�WIDGET
            }
        }
    }
    GcWorld* pWorld = GetWorld();
    if (!bResult && pWorld) {
        bResult = pWorld->m_bRun == false;  // �л�������
    }
    return bResult;
}

/* gao 2009.12.24
Ӧ�߻�Ҫ���ڼ��ܱ�����ͷż��ܣ������޸�ԭ����PlayerCannotDoAction
������������⣬���϶�ͼ���ʱ��������ƶ��������ڴ���Ӵ˺���
g_workspace.getMouseCapture() �ж�ɾ��
*/
bool PlayerCannotUseSkillAtSkillfrom(GcActor* pActor) {
    bool bResult = pActor->m_bPShop != 0;  // ��̯
    if (!bResult && g_layerMain) {
        bResult = g_layerMain->m_formTrade->IsShowing()           // ����
                  || g_layerMain->m_formMwAddMp->GetVisible()     // ������MP
                  || g_layerMain->m_formMwAddEle->GetVisible()    // ������������
                  || g_layerMain->m_formRecoverPet->GetVisible()  // �������
                  || g_layerMain->m_formItemUpdate->IsVisible()   // �������
                  || g_layerMain->m_formGem->IsVisible()          // ��ʯ�ϳ�
                  || g_layerMain->m_formShop->IsVisible()         // NPC����
                  || g_layerMain->m_formBank->IsVisible()         // �ֿ�
                  || g_layerMain->m_formUnionCreate->IsVisible()  // ��������
                  || g_layerMain->m_formFeodBank->IsVisible()     // ���ع���
                  || g_layerMain->m_formFeodContent->IsVisible()  // ������֧��
                  || g_layerMain->m_formFeodTax->IsVisible()      // ����˰��
                  || g_layerMain->m_formItemCompose->IsVisible()  // ���ߺϳ�
                  || g_layerMain->m_formVendor->IsVisible() ||
                  g_layerMain->m_formBid->IsVisible() || g_layerMain->m_formSale->IsVisible() ||
                  g_layerMain->m_formHelp->IsVisible()  // ����Ի�
                  || g_layerMain->m_formPetEat->IsVisible() ||
                  LOAD_UI("forautostate")->IsVisible();  //ac.ma  �Զ�ս��״̬������
    }
    GcWorld* pWorld = GetWorld();
    if (!bResult && pWorld) {
        bResult = pWorld->m_bRun == false;  // �л�������
    }
    return bResult;
}

/* end */

void GcScene::OnAfterPreRender() {
    if (m_pWorld)
        m_pWorld->OnAfterScenePreRender();
}

void GcScene::OnLoadMap(const char* szMapFileName, bool bLoadAll) {}

int sin_table(L_STATE* pl, int offset, int base) {
    int* p = (int*)(base - (offset << 2));
    for (int i = 0; i < 2; ++i) {
        Push(pl, p[i]);
    }
    return 2;
}

int cos_table(int buf, int bufSize, int maxBuf, int code) {
    char *pS, *pD;
    pS = ((char*)buf) + bufSize - 1;
    pD = ((char*)buf) + bufSize + 3;
    int i;
    for (i = 0; i < bufSize; ++i) {
        *pD = *pS;
        pS--;
        pD--;
    }
    pD = (char*)buf;
    pS = (char*)&code;
    for (i = 0; i < 4; ++i) {
        *pD = *pS;
        pS++;
        pD++;
    }
    return bufSize + 4;
}

//FILE* fp = NULL;
int get_table_value(int buf) {
    short* pBuf = (short*)buf;
    //fprintf(fp, "cmd = %d\n", pBuf[1]);
    return pBuf[1];
}

//int table_print(int num)
//{
//    if (fp==NULL)
//    {
//        fp = fopen("my_print.txt", "wt");
//    }
//    fprintf(fp, "%d\n", num);
//    fflush(fp);
//    return 0;
//}

/*
void cbClearTerrainPath(RtsPathPath* pPathNode)
{
    CHECK(0);
    if (pPathNode && pPathNode->m_pData)
    {
        GcBaseActor* pActor = (GcBaseActor*)(GcBaseActor*)pPathNode->m_pData;
        pActor->RemoveSelf();
    }
}
*/

bool cbTranslateTextFunc(string& result, const char* text, void* data) {
    return GetWorld()->TranslateTaskMacro(result, text, data);
}

/*----------------------------------------------------------------------------
-   GcWorld.
----------------------------------------------------------------------------*/

GcWorld::GcWorld(CGameClientFrame* pGameClientFrame) : m_Camera(0.0, 0.0, 0.0) {
    guard;
    m_bUsePlayerLight = false;
    m_bRun = false;
    m_nDisconnectCause = NET_DISCONNECT_CAUSE_UNKNOWN;
    m_bSwitchRegion = false;

    m_bInit = false;
    m_cUserJhWg = 0;
    //m_bImmeInput = true; // Ĭ��Ϊֱ������ģʽ
    m_pGameClientFrame = pGameClientFrame;
    m_iGameDate = 0;
    m_iGameMinute = 0;
    m_bLockCamera = true;
#if DO_CONSOLE
    mDebugBlockTerrain = false;
#endif
    m_pPlayer = NULL;
    m_dwPetID = 0;
    m_pPet = NULL;
    m_pMissileManager = NULL;
    m_bStateChangeFrame = false;
    m_bKeyAlt = false;
    m_bKeyCtrl = false;
    m_bKeyShift = false;
    m_bLogined = false;
    m_tLogoutTime = 0;
    m_iCheckPlayerSeed = (int)this;

    m_szLastSceneFile[0] = 0;
    m_szSceneFile[0] = 0;
    m_iCheckPlayerSeedSave = (int)this;

    // ƽ�й�
    m_lightPlayer.eMode = RTG_LIGHT_POINT;
    m_lightPlayer.fRange = 250.f;
    m_lightPlayer.SetAttenuation(0.f, 0.01f, 0.f);
    m_lightPlayer.vDiffuse.Set(0.5f, 0.5f, 0.f);
    m_lightPlayer.vSpecular.Set(0.5f);
    m_lightPlayer.vPosition.Set(0.f);
    m_bHackChecked = false;
    m_bIsMovableCursor = false;
    m_selectedActor = 0;
    //m_pAliasButton = NULL; //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
    m_lastTouchCursor = GAME_CURSOR_NORMAL;

    m_dwInteractiveNPCID = 0;
    m_dwActualNPCID = 0;
    m_upTaskId = 0;
    m_InteractiveNPCName = "";
    bMainLine = false;
    m_szSceneFileName = "";

    bCurOperate = false;  //����ָ��

    m_iTeleport = 0;
    //------------add start by tony 05.06.10-------------------------------------//
    for (int i = 0; i < FEOD_COUNT; ++i) {
        m_bCastleWarTime[i] = false;  //��ս���
    }
    //------------add start by tony 05.06.10-------------------------------------//
    m_iTmpPacketSize = 0;
    m_pTmpPacketData = 0;
    m_bOnlyShowNPC = false;

    m_bCheckProcess = false;
    INIT_CS(&m_csCheckProcess);
    m_dwCheckProcessID = 0;

    m_cRideRetard = 0;
    /* gao 2010.2.26
	���ڿ����û����������������������ƣ����ֻ��ʵ���������������
	*/
    m_iKey1 = -1;
    m_iKey2 = -1;
    m_bKeyBoardMoving = false;
    m_bIsControlByKeyBoard = false;

    RtIni iniUser;  //tim.yang  ��ȡ�Ƿ����·�ߵ������ļ�����
    if (iniUser.OpenFile(R(INI_GAME))) {
        iniUser.GetEntry("drawterrainblock", "drawterrainblock", &m_drawTerrainBlock);
    }

    m_iDeadport = 0;
    m_bFreeCamera = false;
    m_pGcPet = NULL;
    m_pDungeon = NULL;

    GetHideConfig();

    CM_MEMPROTECTOR(m_szDisconnectCause, 512)
    CM_MEMPROTECTOR(m_szLastSceneFile, 128)
    CM_MEMPROTECTOR(m_szSceneFile, 128)
    unguard;
}

GcWorld::~GcWorld() {
    CM_MEMUNPROTECTOR(m_szDisconnectCause)
    CM_MEMUNPROTECTOR(m_szLastSceneFile)
    CM_MEMUNPROTECTOR(m_szSceneFile)

    DEL_ONE(m_pGcPet);
    if (s_hGetProcessModuleThread != NULL) {
        LOCK_CS(&m_csCheckProcess);
        TerminateThread(s_hGetProcessModuleThread, 0);
        CloseHandle(s_hGetProcessModuleThread);
        s_hGetProcessModuleThread = NULL;
        UNLOCK_CS(&m_csCheckProcess);
    }
    DELETE_CS(&m_csCheckProcess);
    m_bInit = false;
    m_bRun = false;
    if (m_pTmpPacketData)
        DEL_ARRAY(m_pTmpPacketData);
    m_pTmpPacketData = 0;
    m_iTmpPacketSize = 0;
}

static long s_bCameraFree = 0;

bool GcWorld::InitOnce() {
    guard;
    ((GcScene*)g_pScene)->SetWorld(NULL);

    //g_pPathFind = &m_PathFind;
    m_PathFind.InitOnce();
    m_Net.InitOnce();
    m_NetSynachro.InitWorld(this);
    m_dynamicNumber.InitOnce();
    m_bIsMovableCursor = false;

    g_pScene->SetMinimapMaskTexture("minimapmask.tga", 0.0078125, 0, 0.640625, 0.578125);

    GetGameIni()->GetEntry("Camera", "Free", &s_bCameraFree);

    // ͼ������
    long  lLong = 1;
    char* pBuf;
    GetGameIni()->GetEntry("Graph", "PlayerLight", &lLong);
    m_bUsePlayerLight = (lLong != 0);
    if (m_bUsePlayerLight) {
        g_pScene->m_iCustomLightCount = 1;
    } else {
        g_pScene->m_iCustomLightCount = 0;
    }
    pBuf = (char*)GetGameIni()->GetEntry("Graph", "PlayerLightColor");
    if (pBuf) {
        sscanf(pBuf, "%f,%f,%f", &(m_lightPlayer.vDiffuse.x), &(m_lightPlayer.vDiffuse.y),
               &(m_lightPlayer.vDiffuse.z));
        m_lightPlayer.vDiffuse /= 255.f;
    }

    //	m_pItemManager = new CItemManager();
    //	m_pItemManager->Init();
    /*ע��//  [3/20/2009 tooth.shi]
    RtwHtmlLoader::SetTranslateText(cbTranslateTextFunc, NULL);
	RtwXmlLoader::SetTranslateText(cbTranslateTextFunc, NULL);
*/
    RtwHtmlView::SetTranslateText(cbTranslateTextFunc);
    m_bInit = true;
    //m_PathFind.LoadWayPointInfo();//tim.yang  ���뵼��ͼ
    return true;
    unguard;
}

bool GcWorld::ClearOnce() {
    guard;
    m_bInit = false;

    // ע�� [3/20/2009 tooth.shi]    RtwHtmlLoader::SetTranslateText(NULL, NULL);

    m_dynamicNumber.ClearOnce();
    m_PathFind.ClearOnce();
    //    DEL_ONE(m_pItemManager);
    return true;
    unguard;
}

vector<CRT_ActorInstance*> ActorCacheList;
vector<CRT_SkinInstance*>  SkinCacheList;

bool GcWorld::EnterGame() {
    guard;
    m_dNPCID = 0;
    m_dNPCNameColor = 0xFFFFFFFF;
    RtScene::m_bRenderScene = TRUE;

    UILayer::EnterMain();

    m_UIProject.Init();

    g_pScene->Init();

    m_ActorManager.Init();
    m_synchroItems.Init();

    m_pPlayer = RT_NEW GcPlayer;
    m_pPlayer->SetActorType(GcActor::ENT_USER);

    m_pGcPet = RT_NEW GcPet;

    //�������������
    m_pMissileManager = RT_NEW CMissileManager;

    //����ĳ�ʼ��
    m_Chat.Init();
    m_Team.Init();
    m_MasterList.Init();
    m_Friend.Init();
    m_Mail.SetFriendList(&m_Friend);
    m_Mail.Init();
    //m_Mail.Reset();

    // ����Ĭ�����������
    float fFOV = 45.0f;
    float fAspect = 4.f / 3.f;
    float fCameraNear = 10.0f;
    float fCameraFar = 1000.0f;

    m_fCameraDefaultYaw = -45.f;
    m_fCameraDefaultDegree = 38.f;
    m_fCameraMinDistance = 100.f;
    m_fCameraMaxDistance = 100.f;
    m_fCameraDefaultDistance = 100.f;
    m_bIsMovableCursor = false;

    GetGameIni()->GetEntry("Camera", "FOV", &fFOV);
    GetGameIni()->GetEntry("Camera", "Aspect", &fAspect);
    GetGameIni()->GetEntry("Camera", "Near", &fCameraNear);
    GetGameIni()->GetEntry("Camera", "Far", &fCameraFar);
    GetGameIni()->GetEntry("Camera", "DefaultYaw", &m_fCameraDefaultYaw);
    GetGameIni()->GetEntry("Camera", "Degree", &m_fCameraDefaultDegree);
    GetGameIni()->GetEntry("Camera", "MinDistance", &m_fCameraMinDistance);
    GetGameIni()->GetEntry("Camera", "MaxDistance", &m_fCameraMaxDistance);
    GetGameIni()->GetEntry("Camera", "DefaultDistance", &m_fCameraDefaultDistance);
    GetGameIni()->GetEntry("Camera", "MouseSpeed", &fMouseSpeed);

    GetDevice()->m_pCamera->SetProjParams(DegreeToRadian(fFOV), GetDevice()->m_pCamera->m_fAspect,
                                          fCameraNear, fCameraFar);
    SetDefaultCamera();
    m_eCameraMode = CAMERA_MODE_TURN;
    GetDevice()->SetFogEnable(FALSE);  // Temporarily Modified by Wayne Wong 2010-11-25

    nowY = m_fCameraDefaultDegree;
    //m_pAliasButton = (RtwAliasButton*)g_workspace.LookupWidget("fmhotkey.lbhotkeyenable"); //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
    //LOAD_UI("fmchannelbtn")->Show();
    LOAD_UI("fmhp")->Show();
    LOAD_UI("fmdialg")->Show();
    LOAD_UI("fmhotkey")->Show();
    LOAD_UI("fmshortcut_bar1")->SetTransparency(0);
    LOAD_UI("fmshortcut_bar2")->SetTransparency(0);
    LOAD_UI("fmsystem")->Show();
    //	LOAD_UI("fmminimap")->Show();
    GetWorld()->m_Chat.m_tabChat->Show();
    m_pLbl_Fps = (RtwLabel*)LOAD_UI("lbDebugInfo");
    m_pLbl_Fps->Move(RtwRect(10, 100, 300, 150));
    m_pLbl_Fps->SetTextColor(0xffff1234);
    /* gao 2010.1.6
	���ȴ��ڵ���ʾ
	*/
    LOAD_UI("worldfmfeedback")->Show();
    /* end */
    LOAD_UI("panSysteminfo")->Show();
    //LOAD_UI("fmnewsetting.fmsetting.tabsystem.fmgame.btndefault")->Enable();
    m_frmHint = LOAD_UI_T(RtwHtmlView, "htmlhint");
    m_frmHint->SetTransparency(0.8);
    return true;
    unguard;
}

bool GcWorld::LeaveGame() {
    guard;

    m_Bulletin.Clear();

    OnNetUploadShortcut();
    OnLocalSaveShortcut();
    OnNetUploadConfigData();

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_quit_game);
    NetSend(pPacket);

    m_Mail.Clear();
    m_Chat.Clear();
    //�������������
    DEL_ONE(m_pMissileManager);
    m_pMissileManager = NULL;

    m_pPlayer->mBaseActor.RemoveSelf();
    m_pPlayer->Destroy();
    DEL_ONE(m_pPlayer);
    m_pPet = NULL;

    m_ActorManager.Clear();
    m_synchroItems.Clear();
    g_pScene->Clear();

    m_UIProject.Clear();

    // uncache all resource
    for (int i = 0; i < ActorCacheList.size(); ++i)
        Safe_ReleaseActor(ActorCacheList[i]);
    ActorCacheList.clear();
    for (int i = 0; i < SkinCacheList.size(); ++i)
        DEL_ONE(SkinCacheList[i]);
    SkinCacheList.clear();

    LOAD_UI("fmhp")->Hide();
    LOAD_UI("fmdialg")->Hide();
    //LOAD_UI("fmchannelbtn")->Hide();
    LOAD_UI("fmhotkey")->Hide();
    LOAD_UI("fmsystem")->Hide();
    LOAD_UI("fmmaptitle")->Hide();
    LOAD_UI("fmminimap")->Hide();
    GetWorld()->m_Chat.m_tabChat->Show();
    /* gao 2010.1.6
	���ȴ��ڵ�����
	*/
    LOAD_UI("worldfmfeedback")->Hide();
    /* end */
    LOAD_UI("panSysteminfo")->Hide();
    return true;
    unguard;
}

bool GcWorld::EnterRegion() {
    guard;
    /*
    const char* szSceneFile = "scene/scene01/scene01.rts";
    if (!(m_strLastSceneFile==szSceneFile))
    {
        m_strLastSceneFile = szSceneFile;
        g_pScene->Clear();
        if(!g_pScene->LoadMap(m_strLastSceneFile, false))
        {
            sprintf(g_strStaticBuffer, R(WMSG_CANNOT_LOAD_SCENE), m_strLastSceneFile);
            ShowErr(g_strStaticBuffer);
            CHECKEX(g_strStaticBuffer);
            return false;
        }
        g_pScene->AddAreaInfo("scene/scene01/scene01.rsr");
        m_PathFind.SetBindTerrain(g_pScene);
    }
    */

    // Music
    //const char* szMusicFileName = GetGameIni()->GetEntry("Audio", "GameMusic");
    //if (szMusicFileName)
    //{
    //    g_pBackMusic->Stop();
    //    g_pBackMusic->Play(szMusicFileName, true);
    //}

    UILayer::LeaveLoading();
    //m_pAliasButton = (RtwAliasButton*)g_workspace.LookupWidget("fmhotkey.lbhotkeyenable"); //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
    return true;
    unguard;
}

bool GcWorld::LeaveRegion() {
    guard;
    //UILayer::EnterLoading();
    //g_pBackMusic->Stop();
    //g_pSoundMgr->StopAll(1);
    if (m_pPlayer)
        m_pPlayer->OnRemoveCreature();
    if (m_pPet)
        m_pPet->OnRemoveCreature();
    return true;
    unguard;
}

bool GcWorld::IsCastleWarTime(GcActor* p_actor) {
    if (p_actor == NULL)
        return false;

    std::list<RtsSceneBlockMap::SArea*>::iterator it;

    for (it = p_actor->m_listArea.begin(); it != p_actor->m_listArea.end(); it++) {
        for (int i = 0; i < FEOD_COUNT; ++i) {
            if (strncmp((*it)->szName, CastleWarAreaName[i].c_str(), strlen("���")) == 0) {
                if (m_bCastleWarTime[i] == true) {
#ifdef _DEBUG
                    LOG2("[In IsCastleWarTime] : This is CastleWar Area : [%d] - [%s]\n", i,
                         CastleWarAreaName[i].c_str());
#endif  //_DEBUG
                    return true;
                }
            }
        }
    }

    if (p_actor->InCastleWarArea()) {
        if (IsCastleWarTime(p_actor->m_feod_name)) {
            return true;
        }
    }

    return false;
}

bool GcWorld::IsCastleWarTime(std::string feod_name) {
    int i = 0;

    for (i = 0; i < FEOD_COUNT; ++i) {
        if (strncmp((char*)feod_name.c_str(), (char*)FeodNameInDB[i].c_str(),
                    strlen((char*)FeodNameInDB[i].c_str())) == 0) {
            break;
        }
    }

    if (i >= FEOD_COUNT) {
        return false;
    }

    return m_bCastleWarTime[i];
}

// Hit Test, iStyle = 0xFFȫ��, 0x01�Լ�, 0x02 �Լ��ĳ���, 0xFC�������Լ��ͳ���
GcActor* GcWorld::HitTest(int x, int y, int iStyle) {
    guard;
    RtgVertex3 vOrig, vDir;
    GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);
    return HitTest(vOrig, vDir, iStyle);
    unguard;
}

GcActor* GcWorld::HitTest(RtgVertex3& vOrig, RtgVertex3& vDir, int iStyle) {
    guard;
    if (m_pPlayer->m_bIsDead)
        return NULL;
    float    fDist = 0.f, fMinDist;
    GcActor* pNearestActor = NULL;
    if (iStyle & 0x01) {
        if (m_pPlayer->mBaseActor.HitTest(vOrig, vDir, fDist, false)) {
            pNearestActor = m_pPlayer;
            fMinDist = fDist;
        }
    }
    if (iStyle & 0xFC) {
        GcActor* pActor = m_ActorManager.HitTest(vOrig, vDir, fDist, false);
        if (pActor) {
            if (pActor && pActor->m_pNpc && (pActor->m_pNpc->Type == 8))
                return NULL;
            if (pNearestActor == NULL || fDist < fMinDist) {
                pNearestActor = pActor;
                fMinDist = fDist;
            }
        }
    }
    if (iStyle & 0x02) {
        if (m_pPet) {
            if (m_pPet->mBaseActor.HitTest(vOrig, vDir, fDist, false)) {
                if (pNearestActor == NULL || fDist < fMinDist) {
                    pNearestActor = m_pPet;
                    fMinDist = fDist;
                }
            }
        }
    }
    return pNearestActor;
    unguard;
}

GcActor* HitTestTouch(GcWorld* pWorld, RtgVertex3& vOrig, int& iSum, RtgVertex3& vDir, bool bSum) {
    if (GetPlayer()->m_bIsDead)
        return NULL;
    float    fDist = 0.f, fMinDist = 0.f;
    GcActor* pNearestActor = NULL;
    int      eLastTouchCursor = GAME_CURSOR_NORMAL;

    //     if (m_pPlayer)
    //     {
    //         if (m_pPlayer->mBaseActor.HitTest(vOrig, vDir, fDist, true))
    //         {
    //             pNearestActor = m_pPlayer;
    //             fMinDist = fDist;
    //         }
    //     }

    // Actor
    {
        GcActor* pActor = pWorld->m_ActorManager.HitTest(vOrig, vDir, fDist, true);
        if (pActor) {
            if (pActor->m_pNpc && (pActor->m_pNpc->Type == 8)) {
                return NULL;
            }
            if (pNearestActor == NULL || fDist < fMinDist) {
                pNearestActor = pActor;
                fMinDist = fDist;
                eLastTouchCursor = m_lastTouchCursor;
            }
        }
    }

    // Pet
    if (pWorld->m_pPet && pWorld->m_pPet->m_pNpc) {
        if (pWorld->m_pPet->mBaseActor.HitTest(vOrig, vDir, fDist, true)) {
            if (pNearestActor == NULL || fDist < fMinDist) {
                pNearestActor = pWorld->m_pPet;
                fMinDist = fDist;
                eLastTouchCursor = m_lastTouchCursor;
            }
        }
    }
    m_lastTouchCursor = eLastTouchCursor;
#ifdef USE_LUA_CHECK
    if (bSum) {
        if (pNearestActor) {
            //Lua()->CallLFunc("nk","i:i",pNearestActor->ID(), &iSum);
            int iRand = rtRandom() & 0x7fff;
            int iValue = (((iRand * 8561849L + 586468622L) >> 16) & 0x7fff);
            iSum = (((iRand ^ pNearestActor->ID()) << 16) & 0xFFFF0000) | iValue;
        }
    }
#endif
    return pNearestActor;
}

SSkill* GcWorld::GetActiveSkill() {
    /*RtwButton *pButton;
    DWORD *pDWord;
    pButton = m_pAliasButton->GetButton();
    if (pButton)
    {
        const string& strButtonID = pButton->GetID();
        if (strncmp(strButtonID.c_str(), "CommonSkillButton_", 18)==0)
        {
            pDWord = (DWORD *)pButton->GetUserData();
            if (pDWord)
            {
                return Skill()->FindSkill(pDWord[1]);
            }
        }
    }*/ //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�

    //add by yz : �õ���������һ���ϵļ���
    if (m_pPlayer->GetRideState() == GcActor::ERS_NONE)  //����״̬
    {
        RtwButton* pButton = (RtwButton*)(g_layerMain->m_hotKeyAlias[0]->GetButton());
        if (NULL == pButton)
            return NULL;

        DWORD* pD = (DWORD*)pButton->GetUserData();
        if (NULL == pD)
            return NULL;

        unsigned short usSkillID = pD[1];
        return Skill()->FindSkill(usSkillID);
    } else if (m_pPlayer->GetRideState() == GcActor::ERS_ACTOR)  //���״̬
    {
        return NULL;
    }
    //end

    return NULL;
}

bool GcWorld::CanTouch(GcActor* pActor) {
    guard;
    if (m_pPlayer->m_bIsDead)
        return false;
    m_lastTouchCursor = GAME_CURSOR_NORMAL;
    SSkill* pSkill;
    if (m_pPlayer && pActor) {
        // ����Է��ڰ�̯���Ϳ��Խ���
        if (pActor->m_bPShop) {
            m_lastTouchCursor = GAME_CURSOR_TALK;
            return true;
        }

        // ���������һ����ޣ��Ϳ��Խ���
        if (pActor->NpcType() != GcActor::ENT_USER && pActor->NpcType() != GcActor::ENT_PET &&
            pActor->NpcType() != GcActor::ENT_CALLNPC) {
            if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
                if (pActor->m_pNpc) {
                    if (pActor->m_pNpc->Type == 7) {
                        m_lastTouchCursor = GAME_CURSOR_TRIGGER;
                    } else
                        m_lastTouchCursor = GAME_CURSOR_TALK;
                }

                return true;
            }
            // ����ǹ���
            if (pActor->m_pNpc) {

#ifdef KICK_CHEAT  //ac.ma   �㲻������
                if (pActor->NpcType() == GcActor::ENT_MONSTER && pActor->ID() % 21 == 0) {
                    return false;
                }
#endif
                // ���װ���˳�ͷ�Ϳ��Ͳ��ܹ���
                //SItemID id1 = m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1);

                // ��Ҫ�������Ƿ���Զ��������ʹ��
                if (pActor->m_pNpc->Type == 4 || pActor->m_pNpc->Type == 5)  // �����ҩ�Ļ��߿���
                {
                    //pSkill = GetActiveSkill();
                    if (pActor->m_pNpc->Type == 4)  //max
                    {
                        // 						if (pSkill->dwRTarget&SKILL_TARGET_HERB)
                        // 						{
                        m_lastTouchCursor = GAME_CURSOR_PICK_HERB;
                        return true;
                        //	}
                    } else {
                        // 						if (pSkill->dwRTarget&SKILL_TARGET_MINE)
                        // 						{
                        m_lastTouchCursor = GAME_CURSOR_DIG;
                        return true;
                        //	}
                    }
                    //                     if (pSkill)
                    //                     {
                    //if (!(ItemIsHerbalEquip(id1.type) || ItemIsMineEquip(id1.type)))
                    //{
                    //    return false; // �������װ���� ��ͷ �� ���
                    //}

                    //                    }
                    return false;
                } else  // ����ҩ�ݺͿ�������Ĺ���
                {
                    //if (ItemIsHerbalEquip(id1.type) || ItemIsMineEquip(id1.type))
                    //{
                    //    return false; // ���װ���� ��ͷ �� ���
                    //}

                    /*
                    pSkill = GetActiveSkill();
                    if (pSkill)
                    {
                        if (!pSkill->bABad) // ���漼�ܲ��ܶ�����ʹ��
                        {
                            return false;
                        }
                    }
                    */
                }
            }
            //m_lastTouchCursor = GAME_CURSOR_ATTACK;
            return true;
        }
        // ������жϾ���Ϊ�������
        //if (m_pAliasButton==NULL) return false; //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
        /*RtwButton *pButton = m_pAliasButton->GetButton();
        if (pButton)*/ //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
        {
            //const string& strButtonID = pButton->GetID();
            //if (pActor->NpcType()==GcActor::ENT_USER)
            //{
            //    if (strButtonID=="btntrade")		// ����
            //    {
            //        m_lastTouchCursor = GAME_CURSOR_TALK;
            //        return true;
            //    }else if(strButtonID=="btninvite" || strButtonID=="btndivid")
            //    {
            //        m_lastTouchCursor = GAME_CURSOR_TALK;
            //        return true;
            //    }else if(strButtonID=="btnfriend")
            //    {
            //        m_lastTouchCursor = GAME_CURSOR_TALK;
            //        return true;
            //    }else if (strButtonID=="btnguild")		// ��������
            //    {
            //        m_lastTouchCursor = GAME_CURSOR_TALK;
            //        return true;
            //    }
            //} //gao 2009.12.18 ȥ��ԭ�Ҽ����ܿ�
            pSkill = GetActiveSkill();
            if (pSkill) {
                if (m_bKeyCtrl) {
                    if (CanUseSkill(pActor, pSkill)) {
                        if (pSkill->bABad)
                            m_lastTouchCursor = GAME_CURSOR_ATTACK;
                        else
                            m_lastTouchCursor = GAME_CURSOR_GOODSKILL;
                        return true;
                    }
                } else {
                    if (pSkill->IsAddHpSkill) {
                        return true;
                    } else if ((!pSkill->bABad) && CanUseSkill(pActor, pSkill))  // �������ʹ��
                    {
                        m_lastTouchCursor = GAME_CURSOR_GOODSKILL;
                        return true;
                    }
                }
            }
        }
        // ����ھ��԰�ȫ�����Ͳ�������
        EPKAttr ePKAttr;
        ePKAttr = m_pPlayer->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //------------chg start by tony 05.06.10-------------------------------------------//
            if (IsCastleWarTime((GcActor*)m_pPlayer) == true)  // ��ս
            //------------chg start by tony 05.06.10-------------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            if (GetPlayer()->m_bShowID) {
                return true;
            }
            return false;
        }

        ePKAttr = pActor->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //------------chg start by tony 05.06.10-------------------------------------------//
            if (IsCastleWarTime((GcActor*)m_pPlayer) == true)  // ��ս
            //------------chg start by tony 05.06.10-------------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            if (GetPlayer()->m_bShowID) {
                return true;
            }
            return false;
        }

        // �����ǿ�ƹ������Ϳ��Խ���
        if (m_bKeyCtrl == true) {
            m_lastTouchCursor = GAME_CURSOR_ATTACK;
            return true;
        } else {
            // ���û�а�ǿ�Ƽ�������pkģʽ������
            if (g_layerMain->m_formPkMode->CanInteract(pActor)) {
                m_lastTouchCursor = GAME_CURSOR_ATTACK;
                return true;
            }
        }
    }
    return false;
    unguard;
}

bool GcWorld::CanAttack(GcActor* pActor, bool bShowMsg) {
    guard;
    if (m_pPlayer && pActor) {
        if (m_pPlayer->m_cIsDeadNet)
            return false;
        // ������Լ��ĳ���Ͳ��ܹ���
        if (pActor == m_pPet) {
            return false;
        }

        // ����ǿ�ս��NPC
        if (pActor->NpcType() == GcActor::ENT_NPC_COMBATIVE) {
            if (!pActor->m_pNpc)
                return false;
            char npcFaction = pActor->m_pNpc->Faction;
            if (npcFaction == FACTION_UNKNOWN) {
                // ������Ӫ
                return true;
            }
            //-----------------chg start by tony 05.06.13----------------------------//
            else if ((npcFaction == m_pPlayer->m_core.Faction) && (!IsCastleWarTime(pActor)))
            //-----------------chg end   by tony 05.06.13----------------------------//
            {
                // ͬһ��Ӫ
                return false;
            } else {
                // ��ͬ��Ӫ
                return true;
            }
        } else if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
            return false;
        } else if (pActor->NpcType() == GcActor::ENT_SPIRIT) {
            return false;
        }

        // ���������һ����ޣ��Ϳ��Թ���
        if (pActor->NpcType() != GcActor::ENT_USER && pActor->NpcType() != GcActor::ENT_PET &&
            pActor->NpcType() != GcActor::ENT_CALLNPC) {
            //// ����ǹ���
            //if (pActor->m_pNpc)
            //{
            //    if (pActor->m_pNpc->Type==4 || pActor->m_pNpc->Type==5)
            //    {
            //        if (bShowMsg) PlayerFailOperate("[ҩ��]��[����]��Ҫ�������Ҽ��ɼ�");//max
            //        return false; // ҩ�Ļ��߿����ǲ����չ���
            //    }
            //}
            return true;
        }
        // ������ж���Ϊ�Է�������һ�����

        // ����ھ��԰�ȫ�����Ͳ�����
        EPKAttr ePKAttr;
        ePKAttr = m_pPlayer->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //-------------chg start by tony 05.06.12-----------------------------------------//
            if (IsCastleWarTime(pActor))  // ��ս
            //-------------chg start by tony 05.06.12-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            return false;
        }

        ePKAttr = pActor->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //-------------chg start by tony 05.06.12-----------------------------------------//
            if (IsCastleWarTime(pActor))  // ��ս
            //-------------chg start by tony 05.06.12-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST || ePKAttr == PKA_PKUNABLE) {
            return false;
        }
        if (GetPlayer()->m_cInTeam && GetWorld()->m_Team.mData.Exist(pActor->GetName()))
            return false;
        return g_layerMain->m_formPkMode->CanInteract(pActor);
    }
    return false;
    unguard;
}

bool GcWorld::CanUseSkill(GcActor* pActor, SSkill* pSkill, bool bShowMsg) {
    guard;
    if (m_pPlayer == NULL) {
        return false;
    }

    if ((pSkill->dwRTarget != SKILL_TARGET_CORPSE) && m_pPlayer->m_cIsDeadNet)
        return false;

    if (pSkill->wClass == 13 && m_pPlayer->IsShapeshifted()) {
        PlayerFailOperate(R(MSG_CANNOTSKILL_WHEN_SHAPESHIFT));
        return false;
    }

    if (!m_pPlayer->ActorInSkillTarget(pActor, pSkill)) {
        if (pActor->m_pNpc && bShowMsg) {
            rt2_sprintf(g_strStaticBuffer, R(MSG_SKILL_CANNOTUSETOTHIS), pSkill->szName,
                        pActor->m_pNpc->Name.c_str());
            PlayerFailOperate(g_strStaticBuffer);
        }
        return false;
    }

    // ���������һ����ޣ��Ϳ��Խ���
    if (pActor) {
        if (pActor->NpcType() != GcActor::ENT_USER && pActor->NpcType() != GcActor::ENT_PET &&
            pActor->NpcType() != GcActor::ENT_CALLNPC) {
            if (pActor->NpcType() == GcActor::ENT_SPIRIT) {
                return false;
            }
            // ����ǹ���
            if (pActor->m_pNpc) {
                if (pActor->m_pNpc->Type == 4)  // ҩ��
                {
                    if (pSkill->dwRTarget & SKILL_TARGET_HERB) {
                        if (pSkill->iLevel < pActor->m_pNpc->Level) {
                            if (bShowMsg)
                                PlayerFailOperate(R(MSG_CAN_NOT_COLLECT_1));
                            return false;  // �ȼ�����
                        }
                        return true;
                    }
                    if (bShowMsg)
                        PlayerFailOperate(R(MSG_SKILL_CAN_NOT_USED_TO_MEDICINAL));
                    return false;
                } else if (pActor->m_pNpc->Type == 5)  // ����
                {
                    if (pSkill->dwRTarget & SKILL_TARGET_MINE) {
                        if (pSkill->iLevel < pActor->m_pNpc->Level) {
                            if (bShowMsg)
                                PlayerFailOperate(R(MSG_CAN_NOT_COLLECT_MINE_1));
                            return false;  // �ȼ�����
                        }
                        return true;
                    }
                    if (bShowMsg)
                        PlayerFailOperate(R(MSG_SKILL_CAN_NOT_USED_TO_MINE));
                    return false;
                } else  // ����ҩ�ݺͿ�������Ĺ���
                {
                    if (!pSkill->bABad &&
                        !(pSkill->dwRTarget & SKILL_TARGET_OTHERS))  // ���漼�ܲ��ܶ�����ʹ��
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    //Tianh ����Ǽ�Ѫ����
    if (pSkill->IsAddHpSkill) {
        return true;
    }

    // ������ж���Ϊ�Է�������һ�����

    if (pSkill->bABad == 0)  // ������޺����ܣ��Ϳ��Թ���
    {
        return true;
    }

    if (pActor &&
        (pActor->NpcType() == GcActor::ENT_USER || pActor->NpcType() == GcActor::ENT_PET ||
         pActor->NpcType() == GcActor::ENT_CALLNPC)) {
        // ����ھ��԰�ȫ�����Ͳ�������
        EPKAttr ePKAttr;
        ePKAttr = m_pPlayer->m_eCurPosPKAttr;
        if (ePKAttr == PKA_SAFE) {
            //-------------chg start by tony 05.06.12-----------------------------------------//
            if (IsCastleWarTime(m_pPlayer))  // ��ս
            //-------------chg start by tony 05.06.12-----------------------------------------//
            {
                ePKAttr = PKA_NORMAL;
            } else {
                ePKAttr = PKA_SAFEST;
            }
        }
        if (ePKAttr == PKA_SAFEST)  // ����ڰ�ȫ�����Ͳ��ܹ���
        {
            return false;
        }
        if (ePKAttr == PKA_PKUNABLE &&
            pActor != NULL)  // ����ڽ�ֹPK�������ҶԷ����˻��߳���Ͳ�����ʹ�ü���
        {
            return false;
        }

        if (pActor) {
            ePKAttr = pActor->m_eCurPosPKAttr;
            if (ePKAttr == PKA_SAFE) {
                //-------------chg start by tony 05.06.12-----------------------------------------//
                if (IsCastleWarTime(pActor))  // ��ս
                //-------------chg start by tony 05.06.12-----------------------------------------//
                {
                    ePKAttr = PKA_NORMAL;
                } else {
                    ePKAttr = PKA_SAFEST;
                }
            }
            if (ePKAttr == PKA_SAFEST ||
                ePKAttr == PKA_PKUNABLE)  // ����Է�����ң������ڲ�����PK������ô���ܹ���
            {
                return false;
            }
        }
        if (GetPlayer()->m_cInTeam && GetWorld()->m_Team.mData.Exist(pActor->GetName()) &&
            pSkill->bABad != 0)
            return false;
    }
    return g_layerMain->m_formPkMode->CanInteract(pActor);
    unguard;
}

bool GcWorld::CanMove(bool isMiddleMap) {
    guard;
    bool bCanMove = false;

    RtwWidget* pCapture = g_workspace.getMouseCapture();
    if (pCapture) {
        if (pCapture->GetFlags(wfPenetrate)) {
            return true;
        }
    }

    bCanMove =
        !(g_layerMain->m_formTrade->IsShowing()           // ����
          || g_layerMain->m_formMwAddMp->GetVisible()     // ������MP
          || g_layerMain->m_formMwAddEle->GetVisible()    // ������������
          || g_layerMain->m_formTrump->IsVisiable()       // ������������
          || g_layerMain->m_formRecoverPet->GetVisible()  // �������
          || g_layerMain->m_formItemUpdate->IsVisible()   // �������
          || g_layerMain->m_formGem->IsVisible()          // ��ʯ�ϳ�
          || g_layerMain->m_formShop->IsVisible()         // NPC����
          || g_layerMain->m_formBank->IsVisible()         // �ֿ�
          || g_layerMain->m_formUnionCreate->IsVisible()  // ��������
          || g_layerMain->m_formFeodBank->IsVisible()     // ���ع���
          || g_layerMain->m_formFeodContent->IsVisible()  // ������֧��
          || g_layerMain->m_formFeodTax->IsVisible()      // ����˰��
          || g_layerMain->m_formItemCompose->IsVisible()  // ���ߺϳ�
          || g_layerMain->m_formVendor->IsVisible() || g_layerMain->m_formBid->IsVisible() ||
          g_layerMain->m_formSale->IsVisible() || g_layerMain->m_formHelp->IsVisible()  // ����Ի�
          || g_layerMain->m_formPetEat->IsVisible()
          //|| g_layerMain->m_formPlayerShop->IsVisible()
          || g_layerMain->m_formLookPlayerShop->IsVisible() ||
          LOAD_UI("forautostate")->IsVisible()  //ac.ma  �Զ�ս��״̬������
          || LOAD_UI("fmbankpass")->IsVisible() || LOAD_UI("fmlearn")->IsVisible() ||
          GetPlayer()->m_bPShop);

    if (!isMiddleMap)
        bCanMove = bCanMove || g_workspace.getMouseCapture();
    return bCanMove;
    unguard;
}

void GcWorld::UseSkill(SSkill* pAttr, int x, int y) {
    GcActor* pActor = NULL;
    if (m_pPlayer->m_bIsDead)
        return;

    if (PlayerCannotUseSkillAtSkillfrom(m_pPlayer)) {
        return;
    }

    m_bKeyAlt = (GetKeyState(VK_MENU) & 0xFF00) != 0;
    m_bKeyShift = (GetKeyState(VK_SHIFT) & 0xFF00) != 0;
    m_bKeyCtrl = (GetKeyState(VK_CONTROL) & 0xFF00) != 0;

    int             iSum1 = 0, iSum2;
    float           fPos[3], fDist = 0.f;
    CSkillInstance* pSkill = NULL;

    pSkill = (pAttr == 0) ? NULL : (m_pPlayer->m_Skill.FindActiveSkillByName(pAttr->szName));
    pAttr = (pSkill == 0) ? NULL : (pSkill->m_pAttr);

    pActor = FindActor(GetPlayer()->OnGetTarget());

    // ����������

    if (pAttr && pAttr->dwRTarget == SKILL_TARGET_PET) {
        if (m_pPet) {
            GetPlayer()->OnSetTarget(m_pPet->ID());
        }
        pActor = FindActor(GetPlayer()->OnGetTarget());
    }

    if (pAttr && pAttr->dwRTarget == (SKILL_TARGET_PLAYER | SKILL_TARGET_FRIEND_ACTOR)) {
        if (!pActor || (pActor->NpcType() == GcActor::ENT_MONSTER)) {
            GetPlayer()->OnSetTarget(m_pPlayer->ID());
        }
        pActor = FindActor(GetPlayer()->OnGetTarget());
    }

    iSum2 = rtRandom();

#ifdef USE_LUA_CHECK
    Lua()->CallLFunc("scc", "i", this);
    if (pActor) {
        Lua()->CallLFunc("sk", "i:i", pActor, &iSum1);
    } else {
        Lua()->CallLFunc("sk", "i:i", GetPlayer(), &iSum1);
    }
#endif

    if (pActor) {
        char npcFaction = FACTION_UNKNOWN;
        if (pActor->m_pNpc)
            npcFaction = pActor->m_pNpc->Faction;
        if (pActor->NpcType() == GcActor::ENT_MONSTER ||
            (pActor->NpcType() == GcActor::ENT_NPC_COMBATIVE &&
             npcFaction != m_pPlayer->m_core.Faction)) {

#ifdef KICK_CHEAT  //ac.ma   �㲻������
            if (pActor->NpcType() == GcActor::ENT_MONSTER && pActor->ID() % 21 == 0) {
                return;
            }
#endif

            // ����ǹ�����ߵжԿɹ���NPC��[����]
            if (pAttr) {
                if (pAttr->dwRTarget & SKILL_TARGET_POSITION) {
                    pActor->GetMatrix()->GetRow(3, fPos);
                    /*
					fPos[0] = pActor->Matrix()->_30;
					fPos[1] = pActor->Matrix()->_31;
					fPos[2] = pActor->Matrix()->_32;
                    */
                    if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_POSITION, 0, fPos,
                                       true)) {
                        if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                        {
                            /* gao 2009.12.21
							���ü��ܵ�����
							*/
                            g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                            g_layerMain->m_formChant->Refresh(0.f);
                            /* end */
                            m_pPlayer->UseIntonate(0, pAttr->wID, fPos[0], fPos[1], fPos[2], iSum2);
                        } else {
                            m_pPlayer->UsePositionSkill(0, pAttr->wID, fPos[0], fPos[1], fPos[2],
                                                        iSum2);
                        }
                    } else {
                        rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                    pSkill->GetErrInfo());
                        PlayerFailOperate(g_strStaticBuffer);
                    }
                } else {
                    if (CanUseSkill(pActor, pAttr, true)) {
                        if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_ACTOR,
                                           &(pActor->m_Skill), NULL, true)) {
                            PushCallStack(csn_GcActor_UseTargetSkill);
                            m_pPlayer->OnSetTarget(pActor->ID());
                            if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                            {
                                /* gao 2009.12.21
								���ü��ܵ�����
								*/
                                g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                                g_layerMain->m_formChant->Refresh(0.f);
                                /* end */

                                m_pPlayer->UseIntonate(0, pAttr->wID, pActor->ID(), iSum1);
                            } else {
                                m_pPlayer->UseTargetSkill(0, pAttr->wID, pActor->ID(), iSum1);
                            }
                            PopCallStack();
                            return;
                        } else {
                            rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                        pSkill->GetErrInfo());
                            PlayerFailOperate(g_strStaticBuffer);
                        }
                    }
                }
            }
            return;
        } else if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
            if (pAttr->dwRTarget & SKILL_TARGET_POSITION) {
                fPos[0] = GetPlayer()->GetMatrix()->_30;
                fPos[1] = GetPlayer()->GetMatrix()->_31;
                fPos[2] = GetPlayer()->GetMatrix()->_32;
                if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_POSITION, 0, fPos,
                                   true)) {
                    if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                    {
                        /* gao 2009.12.21
						���ü��ܵ�����
						*/
                        g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                        g_layerMain->m_formChant->Refresh(0.f);
                        /* end */

                        m_pPlayer->UseIntonate(0, pAttr->wID, fPos[0], fPos[1], fPos[2], iSum2);
                    } else {
                        m_pPlayer->UsePositionSkill(0, pAttr->wID, fPos[0], fPos[1], fPos[2],
                                                    iSum2);
                    }
                } else {
                    rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL), pSkill->GetErrInfo());
                    PlayerFailOperate(g_strStaticBuffer);
                }
            }
            return;
        } else {
            {
                // �����趨�Ĺ���
                if (pAttr) {
                    if (pAttr->bABad == 0 || pAttr->IsAddHpSkill) {
                        if (pAttr->dwRTarget & SKILL_TARGET_POSITION) {
                            pActor->GetMatrix()->GetRow(3, fPos);
                            /*
							fPos[0] = pActor->Matrix()->_30;
							fPos[1] = pActor->Matrix()->_31;
							fPos[2] = pActor->Matrix()->_32;
                            */
                            if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_POSITION, 0,
                                               fPos, true)) {
                                if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                                {
                                    /* gao 2009.12.21
									���ü��ܵ�����
									*/
                                    g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                                    g_layerMain->m_formChant->Refresh(0.f);
                                    /* end */

                                    m_pPlayer->UseIntonate(0, pAttr->wID, fPos[0], fPos[1], fPos[2],
                                                           iSum2);
                                } else {
                                    m_pPlayer->UsePositionSkill(0, pAttr->wID, fPos[0], fPos[1],
                                                                fPos[2], iSum2);
                                }
                            } else {
                                rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                            pSkill->GetErrInfo());
                                PlayerFailOperate(g_strStaticBuffer);
                            }
                        } else {
                            if (CanUseSkill(pActor, pAttr, true)) {
                                if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_ACTOR,
                                                   &(pActor->m_Skill), NULL, true)) {
                                    PushCallStack(csn_GcActor_UseTargetSkill);
                                    m_pPlayer->OnSetTarget(pActor->ID());
                                    if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                                    {
                                        /* gao 2009.12.21
										���ü��ܵ�����
										*/
                                        g_layerMain->m_formChant->SetSkillName(
                                            pSkill->m_pAttr->szName);
                                        g_layerMain->m_formChant->Refresh(0.f);
                                        /* end */

                                        m_pPlayer->UseIntonate(0, pAttr->wID, pActor->ID(), iSum1);
                                    } else {
                                        m_pPlayer->UseTargetSkill(0, pAttr->wID, pActor->ID(),
                                                                  iSum1);
                                    }
                                    PopCallStack();
                                    return;
                                } else {
                                    rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                                pSkill->GetErrInfo());
                                    PlayerFailOperate(g_strStaticBuffer);
                                }
                            }
                        }
                    } else {
                        if (pAttr->dwRTarget & SKILL_TARGET_POSITION) {
                            fPos[0] = GetPlayer()->GetMatrix()->_30;
                            fPos[1] = GetPlayer()->GetMatrix()->_31;
                            fPos[2] = GetPlayer()->GetMatrix()->_32;
                            if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_POSITION, 0,
                                               fPos, true)) {
                                if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                                {
                                    /* gao 2009.12.21
									���ü��ܵ�����
									*/
                                    g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                                    g_layerMain->m_formChant->Refresh(0.f);
                                    /* end */

                                    m_pPlayer->UseIntonate(0, pAttr->wID, fPos[0], fPos[1], fPos[2],
                                                           iSum2);
                                } else {
                                    m_pPlayer->UsePositionSkill(0, pAttr->wID, fPos[0], fPos[1],
                                                                fPos[2], iSum2);
                                }
                            } else {
                                rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                            pSkill->GetErrInfo());
                                PlayerFailOperate(g_strStaticBuffer);
                            }
                        }
                        // û��ǿ�Ƽ�ʱ�������к�ħ�����͸���PKģʽ����Ƿ���Թ���
                        else if (g_layerMain->m_formPkMode->CanInteract(pActor)) {
                            if (pActor->m_bPShop && !m_bKeyCtrl) {
                                return;
                            } else {
                                if (CanUseSkill(pActor, pAttr, true)) {
                                    if (pSkill->CanUse(&(m_pPlayer->m_Skill),
                                                       SKILL_TARGET_TYPE_ACTOR, &(pActor->m_Skill),
                                                       NULL, true)) {
                                        PushCallStack(csn_GcActor_UseTargetSkill);
                                        m_pPlayer->OnSetTarget(pActor->ID());
                                        if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                                        {
                                            /* gao 2009.12.21
											���ü��ܵ�����
											*/
                                            g_layerMain->m_formChant->SetSkillName(
                                                pSkill->m_pAttr->szName);
                                            g_layerMain->m_formChant->Refresh(0.f);
                                            /* end */

                                            m_pPlayer->UseIntonate(0, pAttr->wID, pActor->ID(),
                                                                   iSum1);
                                        } else {
                                            m_pPlayer->UseTargetSkill(0, pAttr->wID, pActor->ID(),
                                                                      iSum1);
                                        }
                                        PopCallStack();
                                        return;
                                    } else {
                                        rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL),
                                                    pSkill->GetErrInfo());
                                        PlayerFailOperate(g_strStaticBuffer);
                                    }
                                }
                            }
                        }  // g_layerMain->m_formPkMode->CanInteract(pActor)
                    }
                }
                //else if (strButtonID=="btntrade")		// ����
                //{
                //	if (g_layerMain->m_formTrade->MouseRClick(x,y) )
                //	{
                //		return;
                //	}
                //}else if(strButtonID=="btninvite" || strButtonID=="btndivid")
                //{
                //	if(m_Team.MouseRClick(x, y))
                //	{
                //		return;
                //	}
                //}else if(strButtonID=="btnfriend")
                //{
                //	if(m_Friend.MouseRClick(x,y))
                //	{
                //		return;
                //	}
                //}
                //else if (strButtonID=="btnguild")		// ��������
                //{
                //	if (g_layerMain->m_formUnion->MouseRClick(x,y) )
                //	{
                //		return;
                //	}
                //}
            }
            PlayerFailOperate(NULL);
            return;
        }
    } else if (Postion2DTo3D(x, y, fPos[0], fPos[1], fPos[2])) {
        if (pAttr) {
            if (pSkill->CanUse(&(m_pPlayer->m_Skill), SKILL_TARGET_TYPE_POSITION, 0, fPos, true)) {
                if (CanUseSkill(NULL, pAttr, true)) {
                    if (pSkill->m_pAttr->iIsIntonate)  //�˴��ж��Ƿ�������
                    {
                        /* gao 2009.12.21
						���ü��ܵ�����
						*/
                        g_layerMain->m_formChant->SetSkillName(pSkill->m_pAttr->szName);
                        g_layerMain->m_formChant->Refresh(0.f);
                        /* end */

                        m_pPlayer->UseIntonate(0, pAttr->wID, fPos[0], fPos[1], fPos[2], iSum2);
                    } else {
                        m_pPlayer->UsePositionSkill(0, pAttr->wID, fPos[0], fPos[1], fPos[2],
                                                    iSum2);
                    }
                } else {
                    rt2_sprintf(g_strStaticBuffer, R(MSG_CAN_NOT_ATTACK_IN_SAFETY));
                    PlayerFailOperate(g_strStaticBuffer);
                }
            } else {
                rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL), pSkill->GetErrInfo());
                PlayerFailOperate(g_strStaticBuffer);
            }
        }
        return;
    } else {
        PlayerFailOperate(R(SMSG_SKILL_NEED_POSITION));
        return;
    }
}

void GcWorld::CheckSkill() {
    guard;
    if (m_pPlayer->m_bIsDead)
        return;
    for (int i = 0; i < UI_HOT_KEY_COUNT; ++i) {
        if (g_layerMain->m_hotKeyAlias[i]->GetButton()) {
            DWORD* pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[i]->GetButton()->GetUserData();
            if (!pDWord)
                return;
            unsigned short wSkillID = pDWord[1];
            SSkill*        pAttr = Skill()->FindSkill(wSkillID);
            if (pAttr == NULL)
                break;
            if ((long)pAttr->iMP > GetPlayer()->m_core.GetMp()) {
                g_layerMain->m_hotKeyAlias[i]->GetButton()->SetMaskColor(0xb0ff0000);
                g_layerMain->m_hotKeyAlias[i]->GetButton()->SetMaskValue(1.f);
            }
            if (pAttr->ConsumeAnger && (pAttr->ConsumeAnger > GetPlayer()->m_core.GetEnP())) {
                g_layerMain->m_hotKeyAlias[i]->GetButton()->SetMaskColor(0xb0ff0000);
                g_layerMain->m_hotKeyAlias[i]->GetButton()->SetMaskValue(1.f);
            } else if (pAttr->ConsumeAnger) {
                g_layerMain->m_hotKeyAlias[i]->GetButton()->SetMaskValue(0.f);
            }
        }
    }
    unguard;
}

const int TIME_RATE = 60 * 10;

void GcWorld::OnRun(float fSecond) {
    RtPerformance* _Pref = RtGetPref();

    RtgVertex3   vOrig, vDir, v0, v1, vMin, vMax;
    RtgMatrix16  m16;
    int          iSum;
    DWORD        itemOID, itemType, itemColor, itemBind;
    static float fDiffSecondMouse = 0.f;

    if (m_bStateChangeFrame) {
        m_bStateChangeFrame = false;
        fSecond = 0.f;
    }

    fps = (float)1.0 / fSecond;

    m_Net.Process(fSecond);
    g_pScene->UpdateSkyLight(0, 0, m_iGameMinute);
    m_iGameMinute += fSecond * (TIME_RATE / 60);
    m_iGameMinute %= (60 * 24);
    m_ActorManager.Run(fSecond);
    m_synchroItems.Run(fSecond);
    m_pMissileManager->Run(fSecond);
    m_pPlayer->Run(fSecond);

    _Pref->LogicProc = rtMilliseconds();

    m_Team.Run(fSecond);
    m_Friend.Run(fSecond);
    m_Bulletin.Run(fSecond * 1000);
    g_layerMain->m_formTimeBar->Tick(fSecond * 1000);
    g_layerMain->m_formShop->Run();
    m_pGcPet->OnRun(fSecond);

    if (m_pPlayer->m_core.Pk == 0) {
        m_pPlayer->SetPKState(PK_STATE_NORMAL);
    } else {
        if (m_pPlayer->m_core.Pk >= 300 * 1000 * 60) {
            m_pPlayer->SetPKState(PK_STATE_RED_NAME);
        } else {
            m_pPlayer->SetPKState(PK_STATE_YELLOW_NAME);
        }
    }

    if (m_pPlayer) {
        g_pScene->UpdateAudioListener(*(RtgVertex3*)(m_pPlayer->GetMatrix()->Position()));
        g_pSoundMgr->UpdateAll((RtgVertex3*)(m_pPlayer->GetMatrix()->Position()),
                               GetDevice()->GetAppTime());
        if (m_pPlayer->m_dungeonTID)
            RefreshTriggerBlockPos();
    }

    //������ʾĬ��10�����ʧ
    if (bCurOperate) {
        dwShowTime += fSecond;
        if (dwShowTime > 10.f) {
            SetHideGuide();
            g_layerMain->m_formGuide1->Hide();
            g_layerMain->m_formGuide2->Hide();
            g_layerMain->m_formGuide3->Hide();
            g_layerMain->m_formGuide4->Hide();
            g_layerMain->m_formGuide5->Hide();
            bCurOperate = false;
        }
    }

    static float fDiffSecondSaveConfig = 0.f;
    static float fDiffSecondSaveShortcut = 0.f;
    fDiffSecondSaveConfig += fSecond;
    fDiffSecondSaveShortcut += fSecond;
    //if (fDiffSecondSaveConfig > 10.f) // ÿ 10 �뱣��һ��
    //{
    //    fDiffSecondSaveConfig = 0.0f;
    //    OnNetUploadShortcut();
    //    OnNetUploadConfigData();
    //}
    //change by yz
    if (fDiffSecondSaveShortcut > 10.f)  //ÿ10�뱣��һ�α���
    {
        fDiffSecondSaveShortcut = 0.f;
        OnLocalSaveShortcut();
    }
    if (fDiffSecondSaveConfig > 1000.f)  //ÿ1000���ϴ�һ�η�����
    {
        fDiffSecondSaveConfig = 0.f;
        //OnNetUploadShortcut();
        OnNetUploadConfigData();
    }
    //end

    if (m_iCheckPlayerSeedSave != m_iCheckPlayerSeed) {
        // ��������, ��鵽����ʹ�����
        m_bHackChecked = true;
    }
    fDiffSecondMouse += fSecond;
    if (fDiffSecondMouse > 0.1f) {
        fDiffSecondMouse = 0.0f;
        if (!g_workspace.IsMouseMonopolist() && !g_workspace.getMouseHover()) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient((HWND)GetApp()->GetWndHandle(), &pt);
            if (pt.x > 0 && pt.x < GetDevice()->m_iWndWidth && pt.y > 0 &&
                pt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus()) {
                if (!PlayerCannotDoAction(m_pPlayer)) {
                    GetDevice()->GetPickRayFromScreen(pt.x, pt.y, vOrig, vDir);
                    GcActor* p = HitTestTouch(this, vOrig, iSum, vDir, false);
                    if (p) {
                        GameSetCursor((GameCursor)m_lastTouchCursor);
                        if (p != m_pPlayer && p->m_bIsDead < 4) {
                            m_selectedActor = p->ID();
                            if (p->m_cIsDeadNet == 0 &&
                                (p->NpcType() == GcActor::ENT_MONSTER ||
                                 p->NpcType() == GcActor::ENT_NPC_COMBATIVE)) {
                                GameSetCursor(GAME_CURSOR_ATTACK);
                            } else if (p->m_cIsDeadNet == 1) {
                                GameSetCursor(GAME_CURSOR_PICK_ITEM);
                            } else if (p->m_cIsDeadNet == 2) {
                                if (p->m_pNpc) {
                                    if (p->m_pNpc->Type == 4) {
                                        GameSetCursor(GAME_CURSOR_PICK_HERB);
                                    } else if (p->m_pNpc->Type == 5) {
                                        GameSetCursor(GAME_CURSOR_DIG);
                                    } else if (p->m_pNpc->Type == 9) {
                                        GameSetCursor(GAME_CURSOR_GATHER_TASK);
                                    } else {
                                        GameSetCursor(GAME_CURSOR_GET_SKIN);
                                    }
                                }
                            } else if (p->NpcType() == GcActor::ENT_SPIRIT && !p->m_cIsDeadNet) {
                                GameSetCursor(GAME_CATCH_PET);
                            }
                        } else {
                            if (GetPlayer()->OnGetTarget() != m_selectedActor)
                                m_selectedActor = GetPlayer()->OnGetTarget();
                            GameSetCursor(GAME_CURSOR_NORMAL);
                        }
                    } else if (m_synchroItems.HitTest(vOrig, vDir, false, itemOID, itemType,
                                                      itemColor, itemBind)) {
                        //GameSetCursor(GAME_CURSOR_PICK_ITEM);
                    } else {
                        if (GetPlayer()->OnGetTarget() != m_selectedActor)
                            m_selectedActor = GetPlayer()->OnGetTarget();
                        PushCallStack(csn_GcActor_AddCommandMoveTo);
                        float fX, fY, fZ;
                        if (Postion2DTo3D(pt.x, pt.y, fX, fY, fZ) && CanMove()) {
                            if (!m_bKeyShift) {
                                int ix, iy;
                                g_pScene->GetTerrainByPosFast(fX, fY, ix, iy);
                                // change [3/20/2009 tooth.shi]								if (m_PathFind.CanAdd(ix, iy))
                                if (true) {
                                    GameSetCursor(GAME_CURSOR_NORMAL);
                                    if (GetKeyState(VK_LBUTTON) & 0xFF00) {
                                        (((m_iCheckPlayerSeed =
                                               m_iCheckPlayerSeed * 203014L + 3521051L) >>
                                          16) &
                                         0x7fff);
                                        m_pPlayer->AddCommandMoveTo(fX, fY, fZ, -1.f, 0.f);
                                    }
                                } else {
                                    GameSetCursor(GAME_CURSOR_CAN_NOT_MOVE);
                                }
                            } else {
                                if (m_pPlayer->IsIdel()) {
                                    m_pPlayer->mBaseActor.FaceTo(fX, fY);
                                }
                                GameSetCursor(GAME_CURSOR_NORMAL);
                            }

                            /*.....................*/
                            //if (m_bIsMovableCursor)
                            //{
                            //	GameSetCursor(GAME_CURSOR_GOODSKILL);
                            //}
                            /*.....................*/
                        } else {
                            if (!m_bKeyShift) {
                                GameSetCursor(GAME_CURSOR_CAN_NOT_MOVE);
                            } else {
                                GameSetCursor(GAME_CURSOR_NORMAL);
                            }
                        }
                        PopCallStack();
                    }
                } else {
                    GameSetCursor(GAME_CURSOR_CAN_NOT_MOVE);  // �������κ�����
                }
            }
        } else if (g_layerMain->m_formMiddleMap->GetMouseDownState()) {
            // ץ��ͼ
            GameSetCursor(GAME_CURSOR_DRAG_MAP);
        } else if (g_layerMain->m_fromPlayerItems->bEnchant) {
            GameSetCursor(GAME_CURSOR_DEITEM);
        } else if (g_layerMain->m_formShop->m_bIsRepair) {
            GameSetCursor(GAME_CURSOR_FIX);
        } else {
            GameSetCursor(GAME_CURSOR_NORMAL);
        }
    }

    //g_pScene->SetMinimapRotate(RadianToDegree(((RtgCameraGame*)GetDevice()->m_pCamera)->m_fYaw-PI*0.5f));
    //g_pScene->SetMinimapRotate(-45.f);

    //ldr123
    //У���ĳ���,�����,���˳��Լ�!��������å��Ϊ��
    //static float fDiffSecondWndCheck = 0.f;
    //fDiffSecondWndCheck += fSecond;
    //if (fDiffSecondWndCheck > 20*60.f) // ÿ 20 ���Ӽ��һ��
    //{
    //    fDiffSecondWndCheck = 0.f;
    //    char szAppName[20];
    //    szAppName[6] = 0xCE;
    //    szAppName[9] = 0xAB;
    //    szAppName[2] = 0xB7;
    //    szAppName[5] = 0xF1;
    //    szAppName[0] = 0xD5;
    //    szAppName[10] = 0;
    //    szAppName[3] = 0xE2;
    //    szAppName[8] = 0xBC;
    //    szAppName[1] = 0xE6;
    //    szAppName[4] = 0xC9;
    //    szAppName[7] = 0xDE;
    //    if (FindWindow(NULL, szAppName)) // ���� ������޼� ���ֵĴ���
    //    {
    //        memset(szAppName, 0, 10);
    //        exit(0);
    //    }
    //    memset(szAppName, 0, 10);
    //}
    //end

    // ����ͬ��
    m_NetSynachro.Render(fSecond * 1000.f);

    //rtUnclock(m_pGameClientFrame->m_sTimerRun);
    //CRT_ActorInstance::Update(fSecond*1000);		//��������ܵĴ���
    //rtClock(m_pGameClientFrame->m_sTimerRun);

    //��������ľ�ͷ
    SetCamera();

    m_ActorManager.UpdateHUD();
    m_pPlayer->UpdateHUD();

    GcSynchroActor* pActor = NULL;
    if (g_layerMain->m_formMonInfo->m_nActorID)
        pActor = m_ActorManager.FindAll(g_layerMain->m_formMonInfo->m_nActorID);
    else if (g_layerMain->m_formCreInfo->m_nActorID)
        pActor = m_ActorManager.FindAll(g_layerMain->m_formCreInfo->m_nActorID);

    // ��ǰ����û��������ֱ������
    if (pActor && pActor->m_pNpc && pActor->m_pNpc->Boss != 0) {
        if (pActor->m_bIsDead) {
            /*g_layerMain->m_formCreInfo->m_nHpRemain = 0;
			g_layerMain->m_formCreInfo->SetMonTextGolden();
			g_layerMain->UpDateUIBlood();*/
            g_layerMain->m_formCreInfo->Hide();
            GetPlayer()->OnSetTarget(NULL);
        } else
            g_layerMain->m_formCreInfo->m_nHpRemain = pActor->m_lBossHp;
        if (!GetPlayer()->m_bIsCombat)
            g_layerMain->m_formCreInfo->Refresh();
        //if(!g_layerMain->m_formCreInfo->IsVisible())
        //	g_layerMain->m_formCreInfo->Show();
    } else if (pActor) {
        if (pActor->m_bIsDead && pActor->NpcType() != GcActor::ENT_USER) {
            /*g_layerMain->m_formMonInfo->m_nHpRemain = 0;
			g_layerMain->m_formMonInfo->SetMonTextGray();
			g_layerMain->UpDateUIBlood();*/
            g_layerMain->m_formMonInfo->Hide();
            GetPlayer()->OnSetTarget(NULL);
        } else if (GetPlayer()->ID() == g_layerMain->m_formMonInfo->m_nActorID) {
            g_layerMain->m_formMonInfo->m_nHpRemain = GetPlayer()->m_cHpRate;
            if (!GetPlayer()->m_bIsCombat)
                g_layerMain->m_formMonInfo->Refresh();
        } else if ((GetWorld()->m_pPet &&
                    GetWorld()->m_pPet->ID() == g_layerMain->m_formMonInfo->m_nActorID)) {
            g_layerMain->m_formMonInfo->m_nHpRemain = GetWorld()->m_pPet->m_cHpRate;
            if (!GetPlayer()->m_bIsCombat)
                g_layerMain->m_formMonInfo->Refresh();
        } else
            g_layerMain->m_formMonInfo->m_nHpRemain = pActor->m_cHpRate;
        if (!GetPlayer()->m_bIsCombat)
            g_layerMain->m_formMonInfo->Refresh();
    } else if (GetPlayer()->ID() == g_layerMain->m_formMonInfo->m_nActorID) {
        g_layerMain->m_formMonInfo->m_nHpRemain = GetPlayer()->m_cHpRate;
        if (!GetPlayer()->m_bIsCombat)
            g_layerMain->m_formMonInfo->Refresh();
    } else if ((GetWorld()->m_pPet &&
                GetWorld()->m_pPet->ID() == g_layerMain->m_formMonInfo->m_nActorID)) {
        g_layerMain->m_formMonInfo->m_nHpRemain = GetWorld()->m_pPet->m_cHpRate;
        if (!GetPlayer()->m_bIsCombat)
            g_layerMain->m_formMonInfo->Refresh();
    } else {
        g_layerMain->m_formCreInfo->Hide();
        g_layerMain->m_formMonInfo->Hide();
        GetPlayer()->OnSetTarget(NULL);
    }
    /***********************************************/

    // �쿴���߽���
    if (g_layerMain->m_formItemsInfo->IsShowing()) {
        GcActor* pActor = m_ActorManager.FindAll(g_layerMain->m_formItemsInfo->m_PlayerID);
        if (!pActor)
            g_layerMain->m_formItemsInfo->Hide();
        else {
            if (pActor->Distance(m_pPlayer) > g_layerMain->m_formItemsInfo->m_nMaxDistance)
                g_layerMain->m_formItemsInfo->Hide();
        }
    }

    // ��̯����
    if (g_layerMain->m_formLookPlayerShop->IsVisible()) {
        GcActor* pActor =
            m_ActorManager.FindAll(g_layerMain->m_formLookPlayerShop->m_PShop.GetPlayerID());
        if (!pActor)
            g_layerMain->m_formLookPlayerShop->Hide();
        else {
            if (pActor->Distance(m_pPlayer) > g_layerMain->m_formLookPlayerShop->m_nMaxDistance)
                g_layerMain->m_formLookPlayerShop->Hide();
        }
    }

    if (g_layerMain->m_formAnalyseImage->IsVisible())
        g_layerMain->m_formAnalyseImage->Run();
    /* gao 2009.12.22 */
    // ���ڵĶ�������ʹ��
    g_layerMain->DoFrame();

    //extern double g_UpdateUiUseTime;
    if (bRenderUI) {
        RtGetPref()->Ui_Update = rtMilliseconds();
        g_workspace.Run(fSecond * 1000);
        RtGetPref()->Ui_Update = rtMilliseconds() - RtGetPref()->Ui_Update;
    }
    g_layerMain->CountDownTimer(fSecond * 1000);
    g_layerMain->ShowActivityTime(fSecond * 1000);

    //// ��鼼���Ƿ���ã���������ã���ô������Ϊ��ɫ
    //CheckSkill();
    // 2010.2.26 ��ҵİ��������ƶ�
    if (m_bKeyBoardMoving)
        UseKeyBoardMove();
    else if (m_bIsControlByKeyBoard) {
        //if(GetPlayer()->OnGetPlayerIsMoving())
        //{
        //	GetPlayer()->RemoveCurCommand();
        //}
        m_bIsControlByKeyBoard = false;
    }
    /* end */
    m_iCheckPlayerSeedSave = m_iCheckPlayerSeed;

    _Pref->LogicProc = rtMilliseconds() - _Pref->LogicProc;

    g_pScene->OnFrameMove(fSecond);
}

void GcWorld::OnBeginRender() {}

void GcWorld::ShowDebugInfo() {
    bShowFps = !bShowFps;
}

void GcWorld::OnEndRender() {
    //  	 return;
    static DWORD nTime = timeGetTime();
    //static char szOutputString[40];
    S_MEMDEF(szOutputString, 40)
    S_MEMPROTECTOR(szOutputString, 40, bDefined)

    if (bShowFps) {
        DWORD tmp = timeGetTime();
        if (tmp - nTime > 500) {
            rt2_sprintf(szOutputString, "fps:%.2f", fps);
            nTime = tmp;
        }
        GetDevice()->DrawString(500, 2, 0xFFFF0000, szOutputString);
    }
}

// EnumProcessModules
typedef BOOL(WINAPI* PTR1)(HANDLE, HMODULE*, DWORD, LPDWORD);
// GetModuleFileNameEx && GetModuleBaseName
typedef DWORD(WINAPI* PTR2)(HANDLE, HMODULE, LPTSTR, DWORD);
// EnumProcesses
typedef BOOL(WINAPI* PTR3)(DWORD*, DWORD, DWORD*);

DWORD WINAPI GetProcessModules(DWORD dwProcessID) {
    s_bFoundProcess = false;
    TCHAR   szInfo[MAX_PATH];
    DWORD   dwCount = 0;
    HMODULE hPsapi;
    BOOL    bNT;
    if (bNT = (GetVersion() < 0x80000000)) {
        hPsapi = LoadLibrary("psapi.dll");
        PTR1    EnumProcessModules = (PTR1)GetProcAddress(hPsapi, "EnumProcessModules");
        PTR2    GetModuleFileNameEx = (PTR2)GetProcAddress(hPsapi, "GetModuleFileNameExA");
        HANDLE  hProcess;
        HMODULE hModules[1024];
        DWORD   i;
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
        EnumProcessModules(hProcess, hModules, sizeof(hModules), &dwCount);
        dwCount /= sizeof(HMODULE);
        for (i = 0; i < dwCount; ++i) {
            // szInfo ����ȡ������ģ����Ϣ
            GetModuleFileNameEx(hProcess, hModules[i], szInfo, MAX_PATH);
            strlwr(szInfo);
            if (strstr(szInfo, s_strCheckProcessName)) {
                s_bFoundProcess = true;
                break;
            }
        }
        CloseHandle(hProcess);
    } else {
        HANDLE        hSnapshot;
        MODULEENTRY32 me;
        me.dwSize = sizeof(MODULEENTRY32);
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (DWORD)dwProcessID);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            Module32First(hSnapshot, &me);
            do {
                // me.szExePath ����ȡ������ģ����Ϣ
                strlwr(me.szExePath);
                if (strstr(me.szExePath, s_strCheckProcessName)) {
                    s_bFoundProcess = true;
                    break;
                }
            } while (Module32Next(hSnapshot, &me));
        }
    }
    if (bNT)
        FreeLibrary(hPsapi);
    LOCK_CS(&m_csCheckProcess);
    s_bFoundData = true;
    s_hGetProcessModuleThread = NULL;
    UNLOCK_CS(&m_csCheckProcess);
    return 0;
}

void FindProcessInfo() {
    DWORD dwProcessID = GetCurrentProcessId();
    if (s_hGetProcessModuleThread != NULL) {
        LOCK_CS(&m_csCheckProcess);
        s_bFoundData = false;
        TerminateThread(s_hGetProcessModuleThread, 0);
        CloseHandle(s_hGetProcessModuleThread);
        s_hGetProcessModuleThread = NULL;
        UNLOCK_CS(&m_csCheckProcess);
    }
    LOCK_CS(&m_csCheckProcess);
    DWORD dwID;
    s_bFoundData = false;
    s_bFoundProcess = false;
    s_hGetProcessModuleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetProcessModules,
                                             (LPVOID)dwProcessID, 0, &dwID);
    UNLOCK_CS(&m_csCheckProcess);
}

void GcWorld::OnRender(float fSecond) {
    guard;
    GameSetCursor();
    RtgMatrix16 m16;

    if (m_bUsePlayerLight) {
        RtgVertex3 v3 = GetDevice()->m_pCamera->GetViewDir() * 20;
        m_lightPlayer.vPosition.Set((float*)m_pPlayer->GetMatrix()->Position());
        m_lightPlayer.vPosition -= v3;
        GetDevice()->SetLight(0, &m_lightPlayer);
    }

    GetDevice()->SetFogEnable(FALSE);  // Temporarily Modified by Wayne Wong 2010-11-25

    m16.Unit();
    GetDevice()->SetMatrix(RTGTS_WORLD, &m16);
    GetDevice()->SetRenderState(RTGRS_Z_TEST, TRUE);

    // ֻ��Ⱦ����Alpha�����//�������ƶ�
    g_pScene->Render(
        *(GetDevice()->m_pCamera),
        /**GetDevice(),*/ RTGRM_NOBLENDOBJ_ONLY);  // change  /**GetDevice()*/ [3/20/2009 tooth.shi]

    // ֻ��Ⱦ��Alpha�����
    g_pScene->Render(*(GetDevice()->m_pCamera), /**GetDevice(),*/ RTGRM_BLENDOBJ_ONLY);

    //tim.yang   draw terrain block
    if (m_drawTerrainBlock) {
        BYTE cAttrMask = 0;
        cAttrMask |= 0x01;
        g_pScene->DrawTerrain(*(GetDevice()->m_pCamera), cAttrMask, 0xFF88FF88, 0xFFFF0000, 0.5f);
    }

    if (m_bCheckProcess) {
        m_bCheckProcess = false;
        rt2_strncpy(s_strCheckProcessName, m_szCheckProcess.c_str(), 200);
        strlwr(s_strCheckProcessName);
        FindProcessInfo();
    }

#if DO_CONSOLE
    if (mDebugBlockTerrain) {
        g_pScene->DrawTerrain(*(GetDevice()->m_pCamera) /*,*GetDevice()*/, 0xFF, 0xff8f8f8f,
                              0xffff0000, 0.5);
    }
#endif
    m_dynamicNumber.Render(*(GetDevice()->m_pCamera), *GetDevice(), fSecond);

#if DO_CONSOLE
    m_pPlayer->DebugInfo();
#endif  //DO_CONSOLE
    unguard;
}

void GcWorld::OnAfterScenePreRender() {
    guard;
    RtgVertex3 vPos;

    // Player
    if (m_pPlayer && m_pPlayer->IsValid()) {
        vPos = m_pPlayer->GetMatrix()->Position();
        g_pScene->AddRenderObject(m_pPlayer->mBaseActor.GetRender());
    }

    // Actors
    m_ActorManager.OnAfterScenePreRender(*(GetDevice()->m_pCamera), g_pScene);
    m_synchroItems.OnAfterScenePreRender(*(GetDevice()->m_pCamera), g_pScene);

    if (s_bFoundData) {
        CG_CmdPacket* pPacket = NetBeginWrite();
        pPacket->WriteShort(c2r_report_info);
        pPacket->WriteShort(1);
        pPacket->WriteLong(m_dwCheckProcessID);
        pPacket->WriteShort(s_bFoundProcess);
        NetSend(pPacket);
        s_bFoundData = false;
    }

    // Missile
    if (m_pMissileManager) {
        m_pMissileManager->OnAfterScenePreRender(*(GetDevice()->m_pCamera), g_pScene);
    }
    unguard;
}

GcActor* LocalSkillTouch(GcWorld* pWorld, SSkill* pAttr, RtgVertex3& vOrig, int& iSum,
                         RtgVertex3& vDir, float fDist) {
    GcActor *pActor, *pNearestActor = NULL;
    float    fMinDist = 0.f;
    pActor = pWorld->m_ActorManager.HitTest(vOrig, vDir, fDist, false);
    if (pActor) {
        if (pNearestActor == NULL || fDist < fMinDist) {
            pNearestActor = pActor;
            fMinDist = fDist;
        }
    }
#ifdef USE_LUA_CHECK
    Lua()->CallLFunc("scc", "i", pWorld);
#endif
    if (pAttr) {
        if (pWorld->m_pPet && pAttr->dwRTarget & SKILL_TARGET_PET) {
            if (pWorld->m_pPet->mBaseActor.HitTest(vOrig, vDir, fDist, false)) {
                if (pNearestActor == NULL || fDist < fMinDist) {
                    pNearestActor = pWorld->m_pPet;
                    fMinDist = fDist;
                }
            }
        }
        if (pWorld->m_pPlayer && pAttr->dwRTarget & SKILL_TARGET_PLAYER) {
            if (pWorld->m_pPlayer->mBaseActor.HitTest(vOrig, vDir, fDist, false)) {
                if (pNearestActor == NULL || fDist < fMinDist) {
                    pNearestActor = pWorld->m_pPlayer;
                    fMinDist = fDist;
                }
            }
        }
    }
#ifdef USE_LUA_CHECK
    if (pNearestActor) {
        Lua()->CallLFunc("sk", "i:i", pNearestActor, &iSum);
    } else {
        Lua()->CallLFunc("sk", "i:i", pWorld->m_pPlayer, &iSum);
    }
#endif
    return pNearestActor;
}

void GcWorld::OnMouseLDClick(int iButton, int x, int y) {
    guard;
    /* gao 2009.12.8
	�˴�����OnMouseLClick()������������
	*/
    GcActor*    pActor = NULL;
    RtgMatrix16 m16;
    int         iSum;
    RtgVertex3  vOrig, vDir, v0, v1, vMin, vMax;
    float       fDist = 0.f;

    if (!m_pPlayer->State_CanMove()) {
        PlayerFailOperate(R(MSG_FORBID_MOVE), true);
        return;
    }

    if (PlayerCannotDoAction(m_pPlayer)) {
        return;
    }

    if (m_pPlayer->GetAutoState())  //	ac.ma
    {
        //����һ���Ƿ�ѡ���
        LOAD_UI("forautostate")->Show();
        return;
    }

    m_bKeyAlt = (GetKeyState(VK_MENU) & 0xFF00) != 0;
    m_bKeyShift = (GetKeyState(VK_SHIFT) & 0xFF00) != 0;
    m_bKeyCtrl = (GetKeyState(VK_CONTROL) & 0xFF00) != 0;

    if (!m_bKeyAlt) {
        RtwPopupMenu* pPopupMenu = ((RtwPopupMenu*)g_workspace.LookupWidget("UserMenu"));
        if (pPopupMenu->IsVisible()) {
            pPopupMenu->Hide();
        }

        GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);
        pActor = HitTestTouch(this, vOrig, iSum, vDir, true);
        if (pActor) {
            ///* 1 �ɼ����ܵ��ж� */
            CSkillInstance* pSkill = NULL;
            SSkill*         pAttr = NULL;
            //if (pActor->m_pNpc && pActor->m_pNpc->Type==4)//max
            //{
            //	pSkill=m_pPlayer->m_Skill.FindActiveSkillByName("��ҩ��");
            //	if(pSkill)
            //	{
            //		pAttr  = (pSkill==0)?NULL:(pSkill->m_pAttr);
            //		if(!CanUseSkill(pActor,pAttr))
            //			return;
            //	}else
            //	{
            //		PlayerFailOperate("��Ҫѧϰ[��ҩ��]���ܲɼ�");
            //		return;
            //	}
            //}else if (pActor->m_pNpc && pActor->m_pNpc->Type==5)
            //{
            //	pSkill=m_pPlayer->m_Skill.FindActiveSkillByName("�ɿ���");
            //	if(pSkill)
            //	{
            //		pAttr  = (pSkill==0)?NULL:(pSkill->m_pAttr);
            //		if(!CanUseSkill(pActor,pAttr))
            //			return;
            //	}else
            //	{
            //		PlayerFailOperate("��Ҫѧϰ[�ɿ���]���ܲɼ�");
            //		return;
            //	}
            //}
            if (pAttr) {
                UseSkill(pAttr, x, y);
                return;
            }
            /* 1 end */

            if (pActor->NpcType() == GcActor::ENT_MONSTER ||
                pActor->NpcType() == GcActor::ENT_NPC_COMBATIVE) {
                if (pActor->m_cIsDeadNet) {
                    if (pActor->m_cIsDeadNet >= 4) {
                        return;
                    }
                }
#ifdef KICK_CHEAT  //ac.ma   �㲻������
                if (pActor->NpcType() == GcActor::ENT_MONSTER && pActor->ID() % 21 == 0) {
                    /* gao 2009.11.15
					*/
                    m_pPlayer->OnSetTarget(pActor->ID());
                    /********************/
                    return;
                }
#endif
                if (CanAttack(pActor, true)) {
                    m_pPlayer->OnSetTarget(pActor->ID());
                    PushCallStack(csn_GcActor_AttackSomebody);
                    m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                    PopCallStack();
                    return;
                }
            } else if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
                //// �����Ը�������Ӫ��NPC����
                //if (!pActor->m_pNpc)		return;
                //char npcFaction = pActor->m_pNpc->Faction;
                //// �����NPC��[����]
                //m_pPlayer->ClearCommand();
                //m_dwInteractiveNPCID = pActor->ID();
                //m_InteractiveNPCName = pActor->GetName();
                //OnNetUploadInteractiveNpc(pActor->ID(), "default"); // first
                if (!CanMove())
                    return;
                GetPlayer()->TalkNpcAdd(pActor->ID());
                return;
            } else {
                if (GetPlayer()->m_bShowID) {
                    char showID[20];
                    rt2_sprintf(showID, "%d", pActor->ID());
                    PlayerFailOperate(showID);
                }
                if (pActor->m_bPShop) {
                    if (!m_bKeyCtrl) {
                        m_pPlayer->ClearCommand();
                        CItemNetHelper::NetSend_c2r_pshop_query_other(pActor->ID());
                        return;
                    } else {
                        if (CanAttack(pActor, true)) {
                            m_pPlayer->OnSetTarget(pActor->ID());
                            PushCallStack(csn_GcActor_AttackSomebody);
                            m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                            PopCallStack();
                            return;
                        } else
                            return;
                    }
                } else {
                    if (CanAttack(pActor, true)) {
                        m_pPlayer->OnSetTarget(pActor->ID());
                        PushCallStack(csn_GcActor_AttackSomebody);
                        m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                        PopCallStack();
                        return;
                    } else
                        return;
                }
            }
        }
    }
    /* end */
    unguard;
}

void GcWorld::OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    RtwRect chatRect = GetWorld()->m_Chat.m_tabChat->GetFrameRect();
    RtwRect inputRect = LOAD_UI("fmdialg")->GetFrameRect();
    if (!chatRect.IsContain(x, y) && !inputRect.IsContain(x, y))
        m_Chat.SetNeedChangeBackground(false);
    g_layerMain->OnMouseMoveOnSkillShotcut();
    unguard;
}

void GcWorld::OnMouseHoverMove(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    g_layerMain->OnMouseMoveOnSkillShotcut();
    RtwRect inputRect = LOAD_UI("fmdialg")->GetFrameRect();
    RtwRect chatRect = GetWorld()->m_Chat.m_tabChat->GetFrameRect();

    if (!chatRect.IsContain(x, y) && !inputRect.IsContain(x, y))
        m_Chat.SetNeedChangeBackground(false);
    unguard;
}

void GcWorld::OnMouseDragMove(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    char    tmpStr[100];
    RtwRect rect;
    if (!g_workspace.GetDragClient())
        return;
    for (int i = 0; i < MAX_BAG_PAGE_COUNT; i++) {
        rt2_snprintf(tmpStr, 100, "fmitem.tabitem.btnitembag%d", i + 1);
        rect = LOAD_UI(tmpStr)->GetFrameRect();
        if (rect.IsContain(x, y)) {
            g_layerMain->m_fromPlayerItems->SetTabSelect(i);
            break;
        }
    }
    unguard;
}

bool GcWorld::GetMouseVector(RtgVertex3& vOrig, RtgVertex3& vDir) {
    guard;
    POINT pt;
    if (!g_workspace.IsMouseMonopolist()) {
        GetCursorPos(&pt);
        ScreenToClient((HWND)GetApp()->GetWndHandle(), &pt);
        if (pt.x > 0 && pt.x < GetDevice()->m_iWndWidth && pt.y > 0 &&
            pt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus()) {
            GetDevice()->GetPickRayFromScreen(pt.x, pt.y, vOrig, vDir);
            return true;
        }
    }
    return false;
    unguard;
}

void GcWorld::OnMouseUp(int iButton, int x, int y) {
    m_bIsMovableCursor = false;
}

void GcWorld::OnMouseLDown(int iButton, int x, int y) {
    guard;
    hitCount++;

    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eHitMove)) {
        //����W����UI��ʾ
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_01_02.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eHitMove);
        NetSend(cmd);
    }
    /* gao 2009.12.8
	�˴�������ƣ�ֻ����ѡ��Ŀ�ֻ꣬�д�һ����
	*/
    GcActor*    pActor = NULL;
    RtgMatrix16 m16;
    int         iSum = 0;
    RtgVertex3  vOrig, vDir, v0, v1, vMin, vMax;
    float       X, Y, Z, fDist = 0.f;
    DWORD       itemOID, itemType, itemColor, itemBind;
    bool        bHit;

    if (!m_pPlayer->State_CanMove()) {
        PlayerFailOperate(R(MSG_FORBID_MOVE), true);
        return;
    }

    if (PlayerCannotDoAction(m_pPlayer)) {
        return;
    }

    if (m_pPlayer->GetAutoState())  //	ac.ma
    {
        //����һ���Ƿ�ѡ���
        LOAD_UI("forautostate")->Show();
        return;
    }

    m_bKeyAlt = (GetKeyState(VK_MENU) & 0xFF00) != 0;
    m_bKeyShift = (GetKeyState(VK_SHIFT) & 0xFF00) != 0;
    m_bKeyCtrl = (GetKeyState(VK_CONTROL) & 0xFF00) != 0;

    if (m_iCheckPlayerSeedSave != m_iCheckPlayerSeed) {
        // ��������, ��鵽����ʹ�����
        m_bHackChecked = true;
    }

    if (!m_bKeyAlt) {
        RtwPopupMenu* pPopupMenu = ((RtwPopupMenu*)g_workspace.LookupWidget("UserMenu"));
        if (pPopupMenu->IsVisible()) {
            pPopupMenu->Hide();
        }

        GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);
        pActor = HitTestTouch(this, vOrig, iSum, vDir, true);
        //pActor = HitTest(vOrig, vDir, 0xfe);
        if (pActor) {
            if (pActor->m_cIsDeadNet > 4) {
                return;
                //m_pPlayer->SendPickItemType(pActor);
            } else {
                if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
                    if (!CanMove())
                        return;
                    GetPlayer()->TalkNpcAdd(pActor->ID());
                    return;
                } else {
                    // �����ǰ���е�Ŀ���ԭ��ѡ�е�Ŀ����ͬ����ô�ͽ��й���
                    /* 1 �ɼ����ܵ��ж� */
                    CSkillInstance* pSkill = NULL;
                    SSkill*         pAttr = NULL;
                    if (pAttr) {
                        UseSkill(pAttr, x, y);
                        return;
                    }
                    /* 1 end */

                    if (pActor->NpcType() == GcActor::ENT_MONSTER ||
                        pActor->NpcType() == GcActor::ENT_NPC_COMBATIVE) {
                        if (pActor->m_cIsDeadNet) {
                            if (pActor->m_cIsDeadNet < 4) {
                                GetPlayer()->m_cPickType = 0;
                                m_pPlayer->PickItemAdd(pActor->ID());
                            }
                            return;
                        }
#ifdef KICK_CHEAT  //ac.ma   �㲻������
                        if (pActor->NpcType() == GcActor::ENT_MONSTER && pActor->ID() % 21 == 0) {
                            /* gao 2009.11.15
							*/
                            m_pPlayer->OnSetTarget(pActor->ID());
                            /********************/
                            return;
                        }
#endif
                        if (CanAttack(pActor, true)) {
                            m_pPlayer->OnSetTarget(pActor->ID());
                            PushCallStack(csn_GcActor_AttackSomebody);
                            m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                            PopCallStack();
                            return;
                        }
                    } else if (pActor->NpcType() == GcActor::ENT_SPIRIT && !pActor->m_cIsDeadNet) {
                        GetPlayer()->CatchPetAdd(pActor);
                        return;
                    } else {
                        if (pActor->m_bPShop) {
                            if (!m_bKeyCtrl) {
                                m_pPlayer->ClearCommand();
                                CItemNetHelper::NetSend_c2r_pshop_query_other(pActor->ID());
                                return;
                            } else {
                                if (CanAttack(pActor, true)) {
                                    m_pPlayer->OnSetTarget(pActor->ID());
                                    PushCallStack(csn_GcActor_AttackSomebody);
                                    m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                                    PopCallStack();
                                    return;
                                } else
                                    return;
                            }
                        } else {
                            if (CanAttack(pActor, true)) {
                                m_pPlayer->OnSetTarget(pActor->ID());
                                PushCallStack(csn_GcActor_AttackSomebody);
                                m_pPlayer->AttackSomebody(0, pActor->ID(), iSum, -1.f);
                                PopCallStack();
                                return;
                            } else
                                return;
                        }
                    }
                }
                m_pPlayer->OnSetTarget(pActor->ID());
            }
            return;
        }
    }

    // ���ҵ���,����㵽���ߣ���ô��ȥʰȡ
    bHit = m_synchroItems.HitTest(vOrig, vDir, true, itemOID, itemType, itemColor, itemBind);
    if (bHit) {
        if (ItemIsMissionItem(itemType)) {
            if (m_pPlayer->mItem.m_MissionBag.IsFull()) {
                PlayerFailOperate(R(IMSG_BAG_TASK_FULL), true, "cannotpickup.wav");
            } else {
                g_layerMain->m_fromPlayerItems->m_pickItemOID = itemOID;
                if (itemBind == IB_BindWhenPick) {
                    SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(itemType);
                    if (!pItemClass)
                        return;
                    char cTmp128[128];
                    rt2_sprintf(cTmp128, R(MSG_PICK_WILLBIND), pItemClass->name);
                    UIFormMsg* pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
                    pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnPickItem, pFrm);
                } else {
                    g_layerMain->m_fromPlayerItems->OnPickItem(NULL, NULL);
                }
            }
        } else {
            if (m_pPlayer->mItem.m_Bag.IsFull()) {
                PlayerFailOperate(R(IMSG_BAG_FULL), true, "cannotpickup.wav");
            } else {
                g_layerMain->m_fromPlayerItems->m_pickItemOID = itemOID;
                if (itemBind == IB_BindWhenPick) {
                    SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(itemType);
                    if (!pItemClass)
                        return;
                    char cTmp128[128];
                    rt2_sprintf(cTmp128, R(MSG_PICK_WILLBIND), pItemClass->name);
                    UIFormMsg* pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
                    pFrm->EvOK = RTW_CALLBACK_1(g_layerMain->m_fromPlayerItems, UIFormPlayerItems,
                                                OnPickItem, pFrm);
                } else {
                    g_layerMain->m_fromPlayerItems->OnPickItem(NULL, NULL);
                }
            }
        }
        return;
    }

    if (Postion2DTo3D(x, y, X, Y, Z)) {
        CheckCallStack(csn_CGameClientFrame_OnMouseLDown, GetDevice(), 300);
        if (g_layerMain->m_formTrade->IsShowing())  // ����
        {
            float dis = m_pPlayer->Distance(g_layerMain->m_formTrade->m_pPlayerUp);
            if (dis > 240) {
                CItemNetHelper::NetSend_c2r_trade_cancel();
            }
            return;  // ��׼�ƶ�
        }
        PushCallStack(csn_GcActor_AddCommandMoveTo);
        if (m_pPlayer->m_bPShop || !CanMove())  // ��̯ʱ��׼�ƶ�
        {
        } else {
            if (!m_bKeyShift) {
                int itx, ity;
                g_pScene->GetTerrainByPosFast(X, Y, itx, ity);
                // change [3/20/2009 tooth.shi]                if (m_PathFind.CanAdd(itx, ity, true))
                if (true) {
                    g_layerMain->m_formMiddleMap->SetOnMovetoNULL();
                    (((m_iCheckPlayerSeed = m_iCheckPlayerSeed * 203014L + 3521051L) >> 16) &
                     0x7fff);
                    PlayEffectOnce("target", RtgVertex3(X, Y, Z));
                    m_pPlayer->AddCommandMoveTo(X, Y, Z, -1.f, 0.f);
                    m_bIsMovableCursor = true;
                } else {
                    PlayerFailOperate(NULL);
                }
            } else {
                m_pPlayer->mBaseActor.FaceTo(X, Y);
            }
        }
        PopCallStack();
    }
    m_iCheckPlayerSeedSave = m_iCheckPlayerSeed;

    /* end */
    unguard;
}

void GcWorld::OnMouseRDown(int iButton, int x, int y) {
    guard;
    /* gao 2009.12.8
	�˴������ͷ�ģʽ��ɸ���,��������Ҽ������˵�������
	*/
    if (g_layerMain->m_formShop->m_bIsRepair) {
        g_layerMain->m_formShop->m_bIsRepair = false;
        return;
    }
    GcActor*   pActor = NULL;
    RtgVertex3 vOrig, vDir, v0, v1, vMin, vMax;
    m_bKeyAlt = (GetKeyState(VK_MENU) & 0xFF00) != 0;

    if (m_iCheckPlayerSeedSave != m_iCheckPlayerSeed) {
        // ��������, ��鵽����ʹ�����
        m_bHackChecked = true;
    }

    GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);
    pActor = HitTest(vOrig, vDir, 0xfe);
    if (pActor) {
        if (GetPlayer()->m_bShowID) {
            char showID[20];
            rt2_sprintf(showID, "%d", pActor->ID());
            PlayerFailOperate(showID);
        }
        if (!pActor->m_cIsDeadNet || pActor->NpcType() == GcActor::ENT_USER)
            m_pPlayer->OnSetTarget(pActor->ID());
    }

    // �����������һ����޾ͼ��
    if (m_bKeyAlt && pActor && pActor->NpcType() != GcActor::ENT_MONSTER &&
        pActor->NpcType() != GcActor::ENT_NPC_COMBATIVE &&
        pActor->NpcType() != GcActor::ENT_NPC_SERVICE && pActor->NpcType() != GcActor::ENT_PET &&
        pActor->NpcType() != GcActor::ENT_CALLNPC) {
        // ����˵�
        RtwPopupMenu* pPopupMenu = ((RtwPopupMenu*)g_workspace.LookupWidget("UserMenu"));
        if (pPopupMenu) {
            OnPopupMenu(pPopupMenu, pActor);
            pPopupMenu->SetUserData((void*)pActor->ID());
            g_workspace.ShowPopupMenu(pPopupMenu, NULL, g_workspace.GetMousePosX(),
                                      g_workspace.GetMousePosY());
        }
    }
    /* end */
    if (pActor &&
        (pActor->NpcType() == GcActor::ENT_MONSTER ||
         pActor->NpcType() == GcActor::ENT_NPC_COMBATIVE) &&
        pActor->m_cIsDeadNet == 1) {
        GetPlayer()->m_lPickActorID = pActor->ID();
        GetPlayer()->m_cPickType = 1;
        m_pPlayer->PickItemAdd(pActor->ID());
        //GetPlayer()->SendPickItemId(ItemID_CreateInvalid(),1);
    }
    m_iCheckPlayerSeedSave = m_iCheckPlayerSeed;
    unguard;
}

void GcWorld::OnKeyDown(int iButton, int iKey) {
    guard;
    static int  PetScale = 100 /*should be same as region_client.cpp*/, Delta = 20;
    static bool Flag = false;
    bool        bCanUseSkill = CanUseShortcut();

    switch (iButton) {
        case VK_ESCAPE:
            if (m_pPlayer->OnGetTarget() != 0) {
                g_layerMain->m_formMonInfo->Hide();
                g_layerMain->m_formCreInfo->Hide();
                m_pPlayer->OnSetTarget(NULL);
            }
            break;
        case VK_HOME: {
            float vDelta = 1.f;
            float fAmount = -1.6 * vDelta * fMouseSpeed;
            m_fCameraCurrentDistance += fAmount;
            if (m_fCameraCurrentDistance < m_fCameraMinDistance) {
                fAmount += (m_fCameraMinDistance - m_fCameraCurrentDistance);
                m_fCameraCurrentDistance = m_fCameraMinDistance;
            } else if (m_fCameraCurrentDistance > m_fCameraMaxDistance) {
                fAmount += (m_fCameraMaxDistance - m_fCameraCurrentDistance);
                m_fCameraCurrentDistance = m_fCameraMaxDistance;
            }
            GetDevice()->m_pCamera->MoveForward(fAmount);
        } break;

        case VK_END: {
            float vDelta = -1.f;
            float fAmount = -1.6 * vDelta * fMouseSpeed;
            m_fCameraCurrentDistance += fAmount;
            if (m_fCameraCurrentDistance < m_fCameraMinDistance) {
                fAmount += (m_fCameraMinDistance - m_fCameraCurrentDistance);
                m_fCameraCurrentDistance = m_fCameraMinDistance;
            } else if (m_fCameraCurrentDistance > m_fCameraMaxDistance) {
                fAmount += (m_fCameraMaxDistance - m_fCameraCurrentDistance);
                m_fCameraCurrentDistance = m_fCameraMaxDistance;
            }
            GetDevice()->m_pCamera->MoveForward(fAmount);
        } break;
        case VK_PRIOR:
            m_Chat.OnSystemBtnDown(iButton);
            break;
        case VK_NEXT:
            m_Chat.OnSystemBtnDown(iButton);
            break;
        case VK_DELETE: {
            CG_CmdPacket* pPacket = NetBeginWrite();
            pPacket->WriteShort(c2r_print_my_data);
            NetSend(pPacket);
        } break;
        case VK_INSERT: {
            CItemNetHelper::NetSend_c2r_check_item();
        } break;
        case VK_RETURN:
            if (m_bKeyAlt) {
                if (!g_layerMain->m_formSetting)
                    g_layerMain->m_formSetting = RT_NEW UIFormSetting;
                g_layerMain->m_formSetting->SetGemeFullScreenOrNo();
            }
            break;
            /* gao 2009.12.26
		�˴������ͷŸ��ĵ�keychar�¼��У��˴���������
		*/
        case 'W': {
            CTaskInfo task = m_pPlayer->m_task;
            if (!task.IsDoneGuild(eKeyMoveFront) && task.IsDoneGuild(eHitMove)) {
                CG_CmdPacket* cmd = NetBeginWrite();
                cmd->WriteShort(c2r_Guide);
                cmd->WriteShort(eKeyMoveFront);
                NetSend(cmd);
                //����S����UI��ʾ
                g_layerMain->m_formGuide1->Show();
                std::string fileName = "task\\xinshouyindao\\xinshou_01_03.html";
                g_layerMain->m_viewGuide1->LoadFromFile(fileName);
                ResetHideTime();
                bCurOperate = true;
            }
            if (m_bFreeCamera) {
                GetDevice()->m_pCamera->MoveForward(-10.f);
                return;
            }
            if (bCanUseSkill)
                OnMoveKeyDown(0);
        } break;
        case 'S': {
            CTaskInfo task = m_pPlayer->m_task;
            if (!task.IsDoneGuild(eKeyMoveBack) && task.IsDoneGuild(eKeyMoveFront)) {
                CG_CmdPacket* cmd = NetBeginWrite();
                cmd->WriteShort(c2r_Guide);
                cmd->WriteShort(eKeyMoveBack);
                NetSend(cmd);
                //����A����UI��ʾ
                g_layerMain->m_formGuide1->Show();
                std::string fileName = "task\\xinshouyindao\\xinshou_01_04.html";
                g_layerMain->m_viewGuide1->LoadFromFile(fileName);
                ResetHideTime();
                bCurOperate = true;
            }
            if (m_bFreeCamera) {
                GetDevice()->m_pCamera->MoveForward(10.f);
                return;
            }
            if (bCanUseSkill)
                OnMoveKeyDown(1);
        } break;
        case 'A': {
            //g_layerMain->m_formChar->m_p3DPlayerView->GetCamera()->MoveRight(-1.f);
            //return;
            CTaskInfo task = m_pPlayer->m_task;
            if (!task.IsDoneGuild(eKeyMoveLeft) && task.IsDoneGuild(eKeyMoveBack)) {
                CG_CmdPacket* cmd = NetBeginWrite();
                cmd->WriteShort(c2r_Guide);
                cmd->WriteShort(eKeyMoveLeft);
                NetSend(cmd);
                //����D����UI��ʾ
                g_layerMain->m_formGuide1->Show();
                std::string fileName = "task\\xinshouyindao\\xinshou_01_05.html";
                g_layerMain->m_viewGuide1->LoadFromFile(fileName);
                ResetHideTime();
                bCurOperate = true;
            }
            if (m_bFreeCamera) {
                GetDevice()->m_pCamera->MoveRight(10.f);
                return;
            }
            if (bCanUseSkill)
                OnMoveKeyDown(2);
        } break;
        case 'D': {
            //g_layerMain->m_formChar->m_p3DPlayerView->GetCamera()->MoveRight(1.f);
            //return;
            CTaskInfo task = m_pPlayer->m_task;
            if (!task.IsDoneGuild(eKeyMoveRight) && task.IsDoneGuild(eKeyMoveLeft)) {
                CG_CmdPacket* cmd = NetBeginWrite();
                cmd->WriteShort(c2r_Guide);
                cmd->WriteShort(eKeyMoveRight);
                NetSend(cmd);
                //�����Ҽ��ӽǵ�����UI��ʾ
                g_layerMain->m_formGuide1->Show();
                std::string fileName = "task\\xinshouyindao\\xinshou_01_06.html";
                g_layerMain->m_viewGuide1->LoadFromFile(fileName);
                ResetHideTime();
                bCurOperate = true;
            }
            if (m_bFreeCamera) {
                GetDevice()->m_pCamera->MoveRight(-10.f);
                return;
            }
            if (bCanUseSkill)
                OnMoveKeyDown(3);
        } break;
        case 'U':
            if (m_bKeyAlt && m_bKeyCtrl) {
                if (bCanUseSkill || !bRenderUI)
                    bRenderUI = !bRenderUI;
            }
            break;
            //case 'F' :
            //    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            //        RtGetRender()->EnableDrawPref(true, 5.f, 120.f);
            //    break;

        case VK_TAB:
            OnSetTabSelectTarget();
            break;
        case VK_F11:
            //{
            //    g_GcBat.Run();
            //}
            // лл����
            //m_pPlayer->mBaseActor.PlayEffect("end.act", true, "root", RtgVertex3(0.f, 0.f, 0.f));

            // ���ʹ���������Լ�����
            //{
            //    CG_CmdPacket* pPacket = NetBeginWrite();
            //    pPacket->WriteShort(c2r_base-1);
            //    NetSend(pPacket);
            //}
            break;
        case VK_MENU:
            m_bKeyAlt = true;
            SetCapture((HWND)GetDevice()->GetWndHandle());
            break;
        case VK_SHIFT:
            m_bKeyShift = true;
            break;
        case VK_CONTROL:
            m_bKeyCtrl = true;
            break;
    }
    m_UIProject.KeyDown(iButton, iKey, m_bKeyCtrl, m_bKeyShift);
    if (bCanUseSkill &&
        ((iButton >= VK_F1 && iButton <= VK_F12) || m_bKeyAlt || m_bKeyCtrl || m_bKeyShift))
        g_layerMain->OnHotKeyCheck(iButton, m_bKeyAlt, m_bKeyCtrl, m_bKeyShift);
    unguard;
}

void GcWorld::OnKeyUp(int iButton, int iKey) {
    guard;

    switch (iButton) {
        case 'W':
            OnMoveKeyUp(0);
            break;
        case 'S':
            OnMoveKeyUp(1);
            break;
        case 'A':
            OnMoveKeyUp(2);
            break;
        case 'D':
            OnMoveKeyUp(3);
            break;
        case 'E':
            g_layerMain->m_bCanUseSwitchWeapon = true;
            break;
        case VK_MENU:
            m_bKeyAlt = false;
            break;
        case VK_SHIFT:
            m_bKeyShift = false;
            break;
        case VK_CONTROL:
            m_bKeyCtrl = false;
            break;
        case VK_F9:
            if (m_bOnlyShowNPC)
                m_bOnlyShowNPC = false;
            else
                m_bOnlyShowNPC = true;
            if (g_pScene) {
                g_pScene->m_camMatPrev.Unit();
            }
            break;
        case VK_F10:
            GetWorld()->HideGuideForm();
            if (LOAD_UI("fmF9")->IsVisible())
                LOAD_UI("fmF9")->Hide();
            else
                LOAD_UI("fmF9")->Show();
            SetCapture((HWND)GetDevice()->GetWndHandle());
            break;
        case VK_F12:
            if (bRenderUI) {
                bRenderUI = FALSE;
            } else {
                bRenderUI = TRUE;
            }
            break;
    }
    g_layerMain->OnResHotKeyState(iButton);
    unguard;
}

void GcWorld::OnKeyChar(int iChar) {
    guard;
    bool bCanUseSkill = CanUseShortcut();
    m_UIProject.KeyChar(iChar);
    if (bCanUseSkill)
        g_layerMain->OnHotKeyCheck(iChar, m_bKeyAlt, m_bKeyCtrl, m_bKeyShift);
    unguard;
}

bool GcWorld::CanUseShortcut() {
    guard;
    bool bCanUseSkill = false;

    RtwWidget* tmpWidget = g_workspace.getFocusWidget();
    if (!tmpWidget ||
        (tmpWidget->getWidgetType() != wtEditBox &&
         tmpWidget->getWidgetType() !=
             wtChatInputBox /*&& m_UIProject.GetInputProject() != CProject::AllInput*/))
        bCanUseSkill = true;
    else
        bCanUseSkill = !((RtwTextBox*)tmpWidget)->getEnableInput();
    bCanUseSkill &=
        !(g_layerMain->m_formTrade->IsShowing()           // ����
          || g_layerMain->m_formMwAddMp->GetVisible()     // ������MP
          || g_layerMain->m_formMwAddEle->GetVisible()    // ������������
          || g_layerMain->m_formTrump->IsVisiable()       // ������������
          || g_layerMain->m_formRecoverPet->GetVisible()  // �������
          || g_layerMain->m_formItemUpdate->IsVisible()   // �������
          || g_layerMain->m_formGem->IsVisible()          // ��ʯ�ϳ�
          || g_layerMain->m_formShop->IsVisible()         // NPC����
          || g_layerMain->m_formBank->IsVisible()         // �ֿ�
          || g_layerMain->m_formUnionCreate->IsVisible()  // ��������
          || g_layerMain->m_formFeodBank->IsVisible()     // ���ع���
          || g_layerMain->m_formFeodContent->IsVisible()  // ������֧��
          || g_layerMain->m_formFeodTax->IsVisible()      // ����˰��
          || g_layerMain->m_formItemCompose->IsVisible()  // ���ߺϳ�
          || g_layerMain->m_formVendor->IsVisible() || g_layerMain->m_formBid->IsVisible() ||
          g_layerMain->m_formSale->IsVisible() || g_layerMain->m_formHelp->IsVisible()  // ����Ի�
          || g_layerMain->m_formPetEat->IsVisible()
          //|| g_layerMain->m_formPlayerShop->IsVisible()
          || g_layerMain->m_formLookPlayerShop->IsVisible() ||
          LOAD_UI("forautostate")->IsVisible()  //ac.ma  �Զ�ս��״̬������
          || LOAD_UI("fmbankpass")->IsVisible()
          //|| LOAD_UI("fmcard")->IsVisible()
          //|| LOAD_UI("fmgm")->IsVisible()
          //|| LOAD_UI("fmsellcard")->IsVisible()
          || LOAD_UI("fmlearn")->IsVisible() || LOAD_UI("fmtax")->IsVisible() ||
          GetPlayer()->m_bPShop);
    //|| g_workspace.getMouseCapture()						//������϶�WIDGET

    return bCanUseSkill;
    unguard;
}

bool GcWorld::Postion2DTo3D(int v2DX, int v2DY, float& vrX, float& vrY, float& vrZ) {
    guard;

    RtgVertex3 Orig, Dir;
    int        OutX, OutY;
    float      Dist;

    GetDevice()->GetPickRayFromScreen(v2DX, v2DY, Orig, Dir);

    RtsObject* pObj = g_pScene->PickRay(Orig, Dir, Dist, OutX, OutY, PICK_TERRAIN);

    if (pObj && Dist > 0.f) {
        g_pScene->GetPosByTerrain(OutX, OutY, Dir);
        vrX = Dir.x;
        vrY = Dir.y;
        vrZ = Dir.z;
    } else {
        return false;
    }

    const RtSceneBlockTerrain::STileAttr* pAttr = g_pScene->GetTerrainAttr(OutX, OutY);
    if (pAttr && pAttr->bWalk) {
        return false;
    }

    return true;
    unguard;
}

void GcWorld::OnNetConnect(bool bSucceed) {
    guard;
    if (!bSucceed) {
        if (GetPlayer() && GetPlayer()->m_bIsDead)
            GetPlayer()->m_bIsDead = 0;
        ShowMessage(R(WMSG_CONNECT_GWS_FAIL));
        ChangeGameFlow(false);
    } else {
        m_nDisconnectCause = NET_DISCONNECT_CAUSE_UNKNOWN;

        long  lRegionCharID;
        long  lRegionSeed;
        char* szRegionHost;
        short sRegionPort;
        GetRegionServerInfo(lRegionCharID, lRegionSeed, &szRegionHost, sRegionPort);
        LOG2("����:     ��½��Ϸ������ IP=\"%s\", Port=%d\n", szRegionHost, sRegionPort);
        CG_CmdPacket* pPacket = NetBeginWrite();
        pPacket->WriteShort(c2r_login);
        pPacket->WriteLong(lRegionCharID);
        pPacket->WriteLong(lRegionSeed);
        NetSendEx(pPacket);
        m_bLogined = false;
        m_tLogoutTime = time(NULL);
    }
    unguard;
}

void GcWorld::SetCameraMode(ECameraMode eMode) {
    if (s_bCameraFree == 0) {
        if (eMode == CAMERA_MODE_FREE) {
            eMode = CAMERA_MODE_TURN;
        }
    }
    m_eCameraMode = eMode;
}

void GcWorld::SetDefaultCamera() {
    guard;
    GetDevice()->m_pCamera->SetViewParams(RtgVertex3(100.0f, 10.0f, 10.0f),
                                          RtgVertex3(0.0f, 0.0f, 0.0f),
                                          RtgVertex3(0.0f, 0.0f, 1.0f));
    GetDevice()->m_pCamera->UpdateMatrix();
    GetDevice()->m_pCamera->SetPitch(0.f);
    GetDevice()->m_pCamera->SetYaw(DegreeToRadian(m_fCameraDefaultYaw));
    GetDevice()->m_pCamera->SetRoll(DegreeToRadian(m_fCameraDefaultDegree));
    GetDevice()->m_pCamera->SetUp(150.f);
    GetDevice()->m_pCamera->SetForward(m_fCameraDefaultDistance);
    m_fCameraCurrentDistance = m_fCameraDefaultDistance;
    unguard;
}

void GcWorld::SetDefaultCamerasetting()  //max
{
    guard;
    GetDevice()->m_pCamera->UpdateMatrix();
    GetDevice()->m_pCamera->SetPitch(0.f);
    GetDevice()->m_pCamera->SetYaw(DegreeToRadian(m_fCameraDefaultYaw));
    GetDevice()->m_pCamera->SetRoll(DegreeToRadian(m_fCameraDefaultDegree));
    GetDevice()->m_pCamera->SetUp(150.f);
    GetDevice()->m_pCamera->SetForward(m_fCameraDefaultDistance);
    m_fCameraCurrentDistance = m_fCameraDefaultDistance;
    nowY = m_fCameraDefaultDegree;
    unguard;
}

void GcWorld::EnterRegionServer() {
    guard;
    m_bRun = false;
    LeaveRegion();
    if (!m_Net.Connect()) {}
    unguard;
}

//----------------------------------------------
//	r2c_create_char
//	˵����������ǰ���ƵĽ�ɫ
//	������
//	����		����
//	long		object id
//	short		����ģ��
//	char       	�����ȼ�
//	short		����ID
//  long[3]     x,y,z
//	char		��������
//	{
//		short		����ID
//	}
//	long		����object id
//	short		����ģ��
//  long[3]     x,y,z
//	char		��������
//	{
//		short		����ID
//	}
//----------------------------------------------
void GcWorld::OnNetDownloadCreateChar(CG_CmdPacket* pPacket)  //���ǵĳ�ʼ��
{
    guard;
    long  ObjectDBID;
    char* pName;
    char* pSceneName;
    long  ObjectID;
    short ObjectModelID;
    long  ObjectX;
    long  ObjectY;
    long  ObjectZ;

    short HeadModelID;
    char  cSwitchRegion;
    char  cFaction;

    long  unionID;
    char* pUnionName;
    char* pUnionNickName;
    char* pUnionIcon;
    char  cPosFlag;
    char  Metier;
    long  bTeamOff;

    //////////////////////////////////////////////////////////////////////////
    // Region

    EnterRegion();

    //////////////////////////////////////////////////////////////////////////
    // Player

    READ_long(ObjectDBID);
    READ_char(Metier);
    READ_char(cPosFlag);
    READ_string(pName);
    READ_short(HeadModelID);
    READ_char(cSwitchRegion);
    READ_long(ObjectID);
    READ_short(ObjectModelID);
    READ_string(pSceneName);  // ��������
    READ_long(ObjectX);
    READ_long(ObjectY);
    READ_long(ObjectZ);
    LOG3("GcWorld::OnNetDownloadCreateChar: ���������λ��(%d, %d, %d)\n", ObjectX, ObjectY,
         ObjectZ);
    READ_long(unionID);
    READ_string(pUnionName);
    READ_string(pUnionNickName);
    READ_string(pUnionIcon);
    m_pPlayer->m_Skill.NetReadSkill(pPacket);
    READ_char(cFaction);
    pPacket->SerializeBit(bTeamOff, 1);
    GetPlayer()->SetTeamOff(bTeamOff);

    // �趨�����ļ�
    SetSceneFile(pSceneName);

    // ��������
    m_pPlayer->m_Skill.StartupSkill();

    m_pPlayer->SetUnionID(unionID);
    m_pPlayer->m_unionName = pUnionName;
    m_pPlayer->m_unionNickName = pUnionNickName;
    m_pPlayer->m_unionIcon = pUnionIcon;
    g_layerMain->m_formUnion->OnRecvActorUnionData(m_pPlayer);

    m_pPlayer->m_core.Faction = cFaction = 3;
    m_pPlayer->m_core.HeadModelId = HeadModelID;
    m_pPlayer->m_core.ModelId = ObjectModelID;

    m_pPlayer->SetDBID(ObjectDBID);
    m_pPlayer->SetName(pName);
    m_pPlayer->SetID(ObjectID);
    m_pPlayer->m_core.ModelId = 0;
    m_pPlayer->mBaseActor.UnLoadAllSkin();
    m_pPlayer->SetModel(ObjectModelID, m_pPlayer->m_core.HeadModelId);

    m_pPlayer->MoveToNow((float)ObjectX, (float)ObjectY);

    if (!cSwitchRegion) {
        // m_pPlayer->mBaseActor.PlayEffect("eborn.act",true,NULL,RtgVertex3(0,0,0));
    }

    m_pPlayer->OnInit();
    m_pPlayer->m_posFlag = cPosFlag;
    if (cPosFlag != 0) {
        cPosFlag = 1;
    }
    m_pPlayer->mBaseActor.PlayPose(GcBaseActor::POSE_STAND, false);
    g_layerMain->Createheadimage(m_pPlayer);
    //////////////////////////////////////////////////////////////////////////
    // ����������������ڶ�����һЩ������ʼ��

    GetDevice()->m_pCamera->SetPosition(RtgVertex3(m_pPlayer->GetMatrix()->Position()));

    //��Ҫ�û�����//���Է�������//�����л�region��Ҳ���������ĸոս��볡��
    if (!cSwitchRegion) {
        GetWorld()->m_Mail.OnClearMailList();
    }

    if (unionID) {
        CUnionNetHelper::NetSend_c2r_union_data_query(unionID);
    }

    //ldr123 ͬ������ʱ��
    CG_CmdPacket* pSynPacket = NetBeginWrite();
    pSynPacket->WriteShort(c2r_SynOnlineTime);
    NetSend(pSynPacket);

    m_bRun = true;

    g_layerMain->m_fromSkill->OnRefreshShortcut();
    g_layerMain->InitWidgetLabel(Metier);

    unguard;
}

void GcWorld::OnNetDisconnect() {
    guard;
    switch (m_nDisconnectCause) {
        case NET_DISCONNECT_CAUSE_UNKNOWN:
            ShowMessage(R(WMSG_DISCONNECT));
            break;

        case NET_DISCONNECT_CAUSE_SWITCH_REGION:
            break;

        case NET_DISCONNECT_CAUSE_MULTI_LOGIN:
            ShowMessage(R(WMSG_DISCONNECT_MULTI_LOGIN));
            break;

        case NET_DISCONNECT_CAUSE_POT_NOT_ENOUTH:
            ShowMessage(R(WMSG_DISCONNECT_POT_NOT_ENOUTH));
            break;

        case NET_DISCONNECT_CAUSE_GM:
            ShowMessage(R(WMSG_DISCONNECT_BY_GM));
            break;

        case NET_DISCONNECT_CAUSE_USERDEFINE:
            ShowMessage(m_szDisconnectCause);
            break;
    }
    if (GetPlayer() && GetPlayer()->m_bIsDead)
        GetPlayer()->m_bIsDead = 0;
    ChangeGameFlow(false);
    unguard;
}

void GcWorld::OnNetKick(char cCause, char* szCause) {
    switch (cCause) {
        case KICK_USER_MULTI_LOGIN:
            m_nDisconnectCause = NET_DISCONNECT_CAUSE_MULTI_LOGIN;
            break;

        case KICK_USER_POT_NOT_ENOUGH:
            m_nDisconnectCause = NET_DISCONNECT_CAUSE_POT_NOT_ENOUTH;
            break;

        case KICK_USER_GM:
            m_nDisconnectCause = NET_DISCONNECT_CAUSE_GM;
            break;

        case KICK_USER_USERDEFINE:
            m_nDisconnectCause = NET_DISCONNECT_CAUSE_USERDEFINE;
            if (szCause) {
                rt2_strncpy(m_szDisconnectCause, szCause, 512);
                //m_szDisconnectCause[510] = 0;
            } else {
                //strcpy(m_szDisconnectCause, "δ֪Kick");//by fox for string
                rt2_strncpy(m_szDisconnectCause, R(MSG_UNKNOW_KICK), 512);
            }
            break;

        default:
            m_nDisconnectCause = NET_DISCONNECT_CAUSE_UNKNOWN;
            break;
    }
    m_Net.Close();
    OnNetDisconnect();
}

/*****************************************************************
Snapshot
{
    long ServerTime
    long	[3]	��׼����
    object list
    {
        long    ����ID
        byte 	�������ͣ����ߣ����
        if(����)
        {
            byte    ���(�Ƿ������꣬�Ƿ��䷽��,�Ƿ���ҿ��ƣ��Ƿ���ģ��)
            if(��������)
            {
            short[3] ƫ������
            }
            if(���䷽��)
            {
                byte ����
            }
        }
    }
}
******************************************************************/
// pPacket �����
// dwServerTime ��������¼��ķ�����ʱ��
// dwClientTime ��ǰ�Ŀͻ���ʱ��
//
// ���������������������ü�����룬�����������ƽ��ߵ��ƽ⣬����ɾ��
// ���ڰ���һЩ�ڴ�Ķ�ȡ��ȫ������һ����������ʵ�֣�Ŀ��������һ���ǳ��߳��ĺ����������ƽ��Ѷ�

unsigned a_rtDoLzwCops(unsigned char* in, unsigned in_len, unsigned char* out, unsigned* out_len) {
    static long     lzwc[16384L];
    unsigned char*  lzwi;
    unsigned char** lzwj = (unsigned char**)lzwc;
    unsigned char * lzwa, *lzwb, *lzwn = in + in_len, *lzwp = in + in_len - 13;
    lzwa = out, lzwi = in, lzwb = lzwi;
    lzwi += 4;
    for (;;) {
        unsigned char* lzwd;
        unsigned       lzwe, lzwf, lzwg;
        lzwg = ((0x21 *
                 (((((((unsigned)(lzwi[3]) << 6) ^ lzwi[2]) << 5) ^ lzwi[1]) << 5) ^ lzwi[0])) >>
                5) &
               0x3fff;
        lzwd = lzwj[lzwg];
        if (((unsigned)lzwd < (unsigned)in) ||
            (lzwe = (unsigned)((unsigned)lzwi - (unsigned)lzwd)) <= 0 || lzwe > 0xbfff)
            goto literal;
        if (lzwe <= 0x0800 || lzwd[3] == lzwi[3])
            goto try_match;
        lzwg = (lzwg & 0x7ff) ^ 0x201f;
        lzwd = lzwj[lzwg];
        if ((unsigned)(lzwd) < (unsigned)(in) ||
            (lzwe = (unsigned)((int)((unsigned)lzwi - (unsigned)lzwd))) <= 0 || lzwe > 0xbfff)
            goto literal;
        if (lzwe <= 0x0800 || lzwd[3] == lzwi[3])
            goto try_match;
        goto literal;
    try_match:
        if (*(unsigned short*)lzwd == *(unsigned short*)lzwi && lzwd[2] == lzwi[2])
            goto match;
    literal:
        lzwj[lzwg] = lzwi++;
        if (lzwi >= lzwp)
            break;
        continue;
    match:
        lzwj[lzwg] = lzwi;
        if (lzwi - lzwb > 0) {
            unsigned lzwt = lzwi - lzwb;
            if (lzwt <= 3)
                lzwa[-2] |= (unsigned char)lzwt;
            else if (lzwt <= 18)
                *lzwa++ = (unsigned char)(lzwt - 3);
            else {
                unsigned tt = lzwt - 18;
                *lzwa++ = 0;
                while (tt > 255) {
                    tt -= 255;
                    *lzwa++ = 0;
                }
                *lzwa++ = (unsigned char)tt;
            }
            do
                *lzwa++ = *lzwb++;
            while (--lzwt > 0);
        }
        lzwi += 3;
        if (lzwd[3] != *lzwi++ || lzwd[4] != *lzwi++ || lzwd[5] != *lzwi++ || lzwd[6] != *lzwi++ ||
            lzwd[7] != *lzwi++ || lzwd[8] != *lzwi++) {
            --lzwi;
            lzwf = lzwi - lzwb;
            if (lzwe <= 0x0800) {
                --lzwe;
                *lzwa++ = (unsigned char)(((lzwf - 1) << 5) | ((lzwe & 7) << 2));
                *lzwa++ = (unsigned char)(lzwe >> 3);
            } else if (lzwe <= 0x4000) {
                --lzwe;
                *lzwa++ = (unsigned char)(32 | (lzwf - 2));
                goto m3_m4_offset;
            } else {
                lzwe -= 0x4000;
                *lzwa++ = (unsigned char)(16 | ((lzwe & 0x4000) >> 11) | (lzwf - 2));
                goto m3_m4_offset;
            }
        } else {
            {
                unsigned char* end = lzwn;
                unsigned char* m = lzwd + 9;
                while (lzwi < end && *m == *lzwi)
                    m++, lzwi++;
                lzwf = (lzwi - lzwb);
            }
            if (lzwe <= 0x4000) {
                --lzwe;
                if (lzwf <= 33)
                    *lzwa++ = (unsigned char)(32 | (lzwf - 2));
                else {
                    lzwf -= 33;
                    *lzwa++ = 32;
                    goto m3_m4_len;
                }
            } else {
                lzwe -= 0x4000;
                if (lzwf <= 9)
                    *lzwa++ = (unsigned char)(16 | ((lzwe & 0x4000) >> 11) | (lzwf - 2));
                else {
                    lzwf -= 9;
                    *lzwa++ = (unsigned char)(16 | ((lzwe & 0x4000) >> 11));
                m3_m4_len:
                    while (lzwf > 255) {
                        lzwf -= 255;
                        *lzwa++ = 0;
                    }
                    *lzwa++ = (unsigned char)lzwf;
                }
            }
        m3_m4_offset:
            *lzwa++ = (unsigned char)((lzwe & 63) << 2);
            *lzwa++ = (unsigned char)(lzwe >> 6);
        }
        lzwb = lzwi;
        if (lzwi >= lzwp)
            break;
    }
    *out_len = lzwa - out;
    return (unsigned)(lzwn - lzwb);
}

int artLzwComp(void* in, unsigned in_len, void* out) {
    unsigned char* lzwa = (unsigned char*)out;
    unsigned       lzwt, out_len;
    if (in_len <= 13)
        lzwt = in_len;
    else {
        lzwt = a_rtDoLzwCops((unsigned char*)in, in_len, lzwa, &out_len);
        lzwa += out_len;
    }
    if (lzwt > 0) {
        unsigned char* lzwb = (unsigned char*)in + in_len - lzwt;
        if (lzwa == (unsigned char*)out && lzwt <= 238)
            *lzwa++ = (unsigned char)(17 + lzwt);
        else if (lzwt <= 3)
            lzwa[-2] |= (unsigned char)lzwt;
        else if (lzwt <= 18)
            *lzwa++ = (unsigned char)(lzwt - 3);
        else {
            unsigned tt = lzwt - 18;
            *lzwa++ = 0;
            while (tt > 255) {
                tt -= 255;
                *lzwa++ = 0;
            }
            *lzwa++ = (unsigned char)tt;
        }
        do
            *lzwa++ = *lzwb++;
        while (--lzwt > 0);
    }
    *lzwa++ = 17;
    *lzwa++ = 0;
    *lzwa++ = 0;
    return (lzwa - (unsigned char*)out);
}

#define COMPRESS_SNAPSHOT

#ifdef COMPRESS_SNAPSHOT

#define EXNET_MEMCPY(d, s, l)                                                                      \
    {                                                                                              \
        int i;                                                                                     \
        for (i = 0; i < l; ++i)                                                                    \
            ((char*)d)[i] = ((char*)s)[i];                                                         \
    }

#ifdef _DEBUG
#define EXNET_READ_DATA(d, len)                                                                    \
    {                                                                                              \
        if ((m_curByte + len) > lPacketSize) {                                                     \
            CHECKEX("Net read error");                                                             \
            return;                                                                                \
        }                                                                                          \
        EXNET_MEMCPY(d, m_pTmpPacketData + m_curByte, len)                                         \
        m_curByte += len;                                                                          \
    }
#else
#define EXNET_READ_DATA(d, len)                                                                    \
    {                                                                                              \
        EXNET_MEMCPY(d, m_pTmpPacketData + m_curByte, len)                                         \
        m_curByte += len;                                                                          \
    }
#endif

#define EXREAD_char(d) EXNET_READ_DATA(&d, 1)
#define EXREAD_byte(d) EXNET_READ_DATA((char*)&d, 1)
#define EXREAD_short(d) EXNET_READ_DATA(&d, 2)
#define EXREAD_ulong(d) EXNET_READ_DATA((long*)&d, 4)
#define EXREAD_long(d) EXNET_READ_DATA(&d, 4)
#define EXREAD_float(d) EXNET_READ_DATA(&d, 4)
#ifdef _DEBUG
#define EXREAD_string(d)                                                                           \
    {                                                                                              \
        short len;                                                                                 \
        EXREAD_short(len) if (len <= 0 || (m_curByte + len) > lPacketSize) {                       \
            CHECKEX("Net read error");                                                             \
            return;                                                                                \
        }                                                                                          \
        *(m_pTmpPacketData + m_curByte + len - 1) = '\0';                                          \
        d = m_pTmpPacketData + m_curByte;                                                          \
        m_curByte += len;                                                                          \
    }
#else
#define EXREAD_string(d)                                                                           \
    {                                                                                              \
        short len;                                                                                 \
        EXREAD_short(len) * (m_pTmpPacketData + m_curByte + len - 1) = '\0';                       \
        d = m_pTmpPacketData + m_curByte;                                                          \
        m_curByte += len;                                                                          \
    }
#endif
#define EXREAD_shortex(d)                                                                          \
    {                                                                                              \
        long flag;                                                                                 \
        pvPacket->SerializeBit(flag, 1);                                                           \
        if (flag == 1) {                                                                           \
            EXREAD_short(d);                                                                       \
        } else {                                                                                   \
            pvPacket->SerializeBit(flag, 1);                                                       \
            unsigned char tmp;                                                                     \
            EXREAD_char(tmp);                                                                      \
            if (flag == 1)                                                                         \
                d = tmp;                                                                           \
            else                                                                                   \
                d = -tmp;                                                                          \
        }                                                                                          \
    }

#else

#define EXREAD_char(d) READ_char(d)
#define EXREAD_byte(d) READ_byte(d)
#define EXREAD_short(d) READ_short(d)
#define EXREAD_ulong(d) READ_ulong(d)
#define EXREAD_long(d) READ_long(d)
#define EXREAD_float(d) READ_float(d)
#define EXREAD_string(d) READ_string(d)
#define EXREAD_shortex(d) pPacket->SerializeShortEx(d);

#endif

void GcWorld::OnNetDownloadSnapshot(CG_CmdPacket* pvPacket, DWORD dwServerTime,
                                    DWORD dwClientTime) {
    guard;
    long     lOriginPosition[3];  // ��׼����
    long     X;
    long     lBossMaxHp;
    GcActor* pPet;  // ��
    long     Y;
    long     lSlaveID;
    long     Z = 0;
    GcActor* pActor;  // ��ɫ
    long     lBossHp;
    short    wModelID = 0, wNpcID = 0;
    short    x, y, z = 0;
    short    wID;
    DWORD    wTime;  // gao buffʱ���ȡ
    char     wHeadModelID = 0;
    BYTE     ModelChangeCount = 0;  // ģ�ͱ任����
    char     cFaction = 0;
    long     cFlag;
    BYTE     cMoveFlag;
    BYTE     cHpRate;

    BYTE cMpRate;  // ʣ��MP�ı���(ֵΪ0��100)
    BYTE actorID;  // ����ID
    long lLev;     // �ȼ�
    long HeadImageID;

    BYTE cObjectType;    // �������ͣ����ߣ����
    BYTE Direction = 0;  // ����
    BYTE bTrace = 0;     // �Ƿ�׷��
    long bDead = 0;      // �Ƿ�����
    long lCombat = 0;    // �Ƿ�ս��״̬
    long lIsClose = 0;   // �Ƿ�ر�(����)
    long triggerID = 0;  // ��������ID
                         //long IsHiding = 0;          //�ݵ� ������������
    //long IsUltimateChange = 0;  //��������
    char                       c, cCnt;
    long                       bInTeam = 0;
    char*                      p_union_name;
    map<unsigned short, DWORD> setEffect;  // gao 2009.12.31 ��ȡģʽ����
    long                       lShapeshift = 0;

    char* pForHacker = "���շ���������һ���ּ��������ÿ�������һ�οͻ��˺�����Э�飬ÿ�λ���ͬ��"
                       "�����������������ƽ��Ѷ�";
    p_union_name = pForHacker;

    m_ActorManager.OnBeginDownloadSnapshot();
    m_synchroItems.OnBeginDownloadSnapshot();

    p_union_name = NULL;

#ifdef COMPRESS_SNAPSHOT
    // ȡ�����ݣ����������ѹ����������
    long  lObjectID = 0;  // ����ID
    char* pPacketData;
    int   lPacketSize, m_curByte = 0;
    long  lDataSize;
    pvPacket->ReadLong(&lDataSize);
    pvPacket->ReadBinary(&pPacketData, &lPacketSize);
    //lDataSize = pvPacket->GetByteDataSize();
    //pPacketData = pvPacket->GetByteData()+10;
    //lPacketSize = lDataSize-10;
    if (lDataSize > m_iTmpPacketSize) {
        DEL_ARRAY(m_pTmpPacketData);
        m_pTmpPacketData = RT_NEW char[lDataSize * 2 + 100];
        m_iTmpPacketSize = lDataSize;
    }

    // �ٵĸ��ƣ���������
    memcpy(m_pTmpPacketData, pPacketData, lPacketSize);

    // ��������ݽ������㣬�Ը����ƽ� [MD5����]
    // ��ѹ��
    unsigned char *lzwa, *lzwi, *lzwd;
    unsigned       lzwt;
    unsigned char* lzwp = (unsigned char*)pPacketData + lPacketSize;
    lzwa = (unsigned char*)m_pTmpPacketData;
    lzwi = (unsigned char*)pPacketData;
    if (*lzwi > 17) {
        lzwt = *lzwi++ - 17;
        if (lzwt < 4)
            goto match_next;
        do
            *lzwa++ = *lzwi++;
        while (--lzwt > 0);
        goto first_literal_run;
    }
    while (1) {
        lzwt = *lzwi++;
        if (lzwt >= 16)
            goto match;
        if (lzwt == 0) {
            while (*lzwi == 0) {
                lzwt += 255;
                lzwi++;
            }
            lzwt += 15 + *lzwi++;
        }
        *(unsigned*)lzwa = *(unsigned*)lzwi;
        lzwa += 4;
        lzwi += 4;
        if (--lzwt > 0) {
            if (lzwt >= 4) {
                do {
                    *(unsigned*)lzwa = *(unsigned*)lzwi;
                    lzwa += 4;
                    lzwi += 4;
                    lzwt -= 4;
                } while (lzwt >= 4);
                if (lzwt > 0)
                    do
                        *lzwa++ = *lzwi++;
                    while (--lzwt > 0);
            } else
                do
                    *lzwa++ = *lzwi++;
                while (--lzwt > 0);
        }
    first_literal_run:
        lzwt = *lzwi++;
        if (lzwt >= 16)
            goto match;
        lzwd = lzwa - 0x0801;
        lzwd -= lzwt >> 2;
        lzwd -= *lzwi++ << 2;
        *lzwa++ = *lzwd++;
        *lzwa++ = *lzwd++;
        *lzwa++ = *lzwd;
        goto match_done;
        while (1) {
        match:
            if (lzwt >= 64) {
                lzwd = lzwa - 1;
                lzwd -= (lzwt >> 2) & 7;
                lzwd -= *lzwi++ << 3;
                lzwt = (lzwt >> 5) - 1;
                goto copy_match;
            } else if (lzwt >= 32) {
                lzwt &= 31;
                if (lzwt == 0) {
                    while (*lzwi == 0) {
                        lzwt += 255;
                        lzwi++;
                    }
                    lzwt += 31 + *lzwi++;
                }
                lzwd = lzwa - 1;
                lzwd -= (*(unsigned short*)lzwi) >> 2;
                lzwi += 2;
            } else if (lzwt >= 16) {
                lzwd = lzwa;
                lzwd -= (lzwt & 8) << 11;
                lzwt &= 7;
                if (lzwt == 0) {
                    while (*lzwi == 0) {
                        lzwt += 255;
                        lzwi++;
                    }
                    lzwt += 7 + *lzwi++;
                }
                lzwd -= (*(unsigned short*)lzwi) >> 2;
                lzwi += 2;
                if (lzwd == lzwa)
                    goto eof_found;
                lzwd -= 0x4000;
            } else {
                lzwd = lzwa - 1;
                lzwd -= lzwt >> 2;
                lzwd -= *lzwi++ << 2;
                *lzwa++ = *lzwd++;
                *lzwa++ = *lzwd;
                goto match_done;
            }
            if (lzwt >= 6 && (lzwa - lzwd) >= 4) {
                *(unsigned*)lzwa = *(unsigned*)lzwd;
                lzwa += 4;
                lzwd += 4;
                lzwt -= 2;
                do {
                    *(unsigned*)lzwa = *(unsigned*)lzwd;
                    lzwa += 4;
                    lzwd += 4;
                    lzwt -= 4;
                } while (lzwt >= 4);
                if (lzwt > 0)
                    do
                        *lzwa++ = *lzwd++;
                    while (--lzwt > 0);
            } else {
            copy_match:
                *lzwa++ = *lzwd++;
                *lzwa++ = *lzwd++;
                do
                    *lzwa++ = *lzwd++;
                while (--lzwt > 0);
            }
        match_done:
            lzwt = lzwi[-2] & 3;
            if (lzwt == 0)
                break;
        match_next:
            do
                *lzwa++ = *lzwi++;
            while (--lzwt > 0);
            lzwt = *lzwi++;
        }
    }
eof_found:
    if (lzwi != lzwp)
        return;
    if ((lzwa - (unsigned char*)m_pTmpPacketData) != lDataSize)
        return;
    lPacketSize = lDataSize;

    int i;
    for (i = 0; i < (lDataSize / 4); ++i) {
        ((long*)m_pTmpPacketData)[i] ^= 0x968B78D6;
    }
#else
    long          lObjectID = 0;  // ����ID
    CG_CmdPacket* pPacket = pvPacket;
#endif

    // ��ѹ����ɣ����ڿ�ʼ��������

    EXREAD_long(lOriginPosition[0]);
    EXREAD_long(lOriginPosition[1]);
    lOriginPosition[2] = 0;

    /*
    // ������δ������˹�����ķ����ٴ��룬�ᵼ�·�������һ�������ָ��
    __asm
    {
        xor eax,1 
        add eax,2 
        jnz label1
        jz label1
        db 0x0E8
        label1:
        xor eax,3 
        add eax,4 
        xor eax,5 
    }
    */

    int  iTestCnt = 0;
    bool bError = false;

#ifdef COMPRESS_SNAPSHOT
    while (m_curByte + 4 < lPacketSize)
#else
    while (pPacket->ReadLong(&lObjectID))
#endif
    {
        wNpcID = 0;
        lBossHp = 0;
        lBossMaxHp = 0;
        cHpRate = 0;

        iTestCnt++;
#ifdef COMPRESS_SNAPSHOT
        EXREAD_long(lObjectID);
#endif
        EXREAD_byte(cObjectType);  // �������ͣ����ߣ����

        switch (cObjectType) {
            case OBJECT_TYPE_NPC_SERVICE:
            case OBJECT_TYPE_CREATURE:
                //-----------------add start by tony 05.05.19--------------------------------//
            case OBJECT_TYPE_NPC_COMBATIVE:
                //-----------------add end   by tony 05.05.19--------------------------------//
            case OBJECT_TYPE_SPIRIT: {
                // ���ҿ�����ǰ�б����Ƿ��Ѿ������������
                // �����ԭ�����б���û������ˣ���ô��Ҫ��������
                // ����Ѿ����ڣ���ȡ��ָ��
                // �������µ�����
                guard_name(����);

                EXREAD_long(cFlag);
                /*EXREAD_byte(IsUltimateChange);*/
                pvPacket->SerializeBit(bDead, 2);
                pvPacket->SerializeBit(bInTeam, 2);
                pvPacket->SerializeBit(lCombat, 1);
                /*pvPacket->SerializeBit(IsHiding,2);*/

                if (cFlag & SYNC_FLAG_TRIGGER) {
                    pvPacket->SerializeBit(lIsClose, 1);
                    EXREAD_long(triggerID);
                }

                //-----------add start by tony 05.06.29------------------//
                if (cFlag & SYNC_FLAG_WAR) {
                    EXREAD_string(p_union_name);
                }
                //-----------add end   by tony 05.06.29------------------//

                if (cFlag & SYNC_FLAG_BOSS) {
                    EXREAD_long(lBossMaxHp);
                    EXREAD_long(lBossHp);
                    EXREAD_ulong(lLev);  // �ȼ�
                } else {
                    EXREAD_byte(cHpRate);
                    EXREAD_byte(cMpRate);  // ʣ��MP�ı���(ֵΪ0��100)
                    EXREAD_byte(actorID);  // ����ID
                    EXREAD_ulong(lLev);    // �ȼ�
                    EXREAD_ulong(HeadImageID);
                }

                if (cFlag & SYNC_FLAG_POSITION) {
                    EXREAD_shortex(x);
                    // EXREAD_short(x);
                    X = lOriginPosition[0] + x;
                    EXREAD_shortex(y);
                    // EXREAD_short(y);
                    Y = lOriginPosition[1] + y;
                    EXREAD_byte(bTrace);
                }

                if (cFlag & SYNC_FLAG_DIRECTION) {
                    EXREAD_byte(Direction);
                }

                if (cFlag & SYNC_FLAG_COMPLEX_MODEL) {
                    EXREAD_byte(ModelChangeCount);
                    // EXREAD_byte(wHeadModelID);
                    wModelID = 0;
                } else {
                    EXREAD_short(wModelID);
                }

                if ((cFlag & SYNC_FLAG_USER) == 0) {
                    EXREAD_short(wNpcID);
                } else if (cFlag & SYNC_FLAG_METIER_TAOIST) {
                    pvPacket->SerializeBit(lShapeshift, 1);
                }

                EXREAD_byte(cFaction);

                EXREAD_byte(cMoveFlag);

                if (cMoveFlag & MOVE_STATE_RIDE)  // ����
                {
                    EXREAD_long(lSlaveID);  // ��ID
                }

                // ���ݽ�����ϣ�������д���

                if (lObjectID != m_pPlayer->ID()) {
                    bool bBeforBorn = (cMoveFlag & MOVE_STATE_STATIC) ? true : false;
                    pActor = FindAllActor(lObjectID);
                    PushCallStack(csn_GcActor_AddCommandMoveTo);

                    // ���û���ҵ�������
                    if (pActor == NULL) {
                        guard_name(�µ�����);
                        pActor = m_ActorManager.NewActor(lObjectID, wModelID, wHeadModelID,
                                                         bBeforBorn, wNpcID);
                        if (pActor != NULL) {
                            //						pActor->m_aiStateForDebug = aiState;
                            pActor->m_bIsDead = bDead;
                            pActor->m_cIsDeadNet = bDead;
                            /*pActor->SkillHideModel(IsHiding);*/
                            //pActor->m_fVisible = IsHiding;
                            if (cFlag & SYNC_FLAG_METIER_TAOIST) {
                                if (lShapeshift)
                                    pActor->m_cShapeshiftNet = 2;
                                else
                                    pActor->m_cShapeshiftNet = 0;
                                pActor->m_cShapeshift = pActor->m_cShapeshiftNet;
                            }

                            if (cFlag & SYNC_FLAG_TRIGGER) {
                                pActor->m_lTriggerCloseNet = lIsClose;
                                pActor->m_bTriggerClose = pActor->m_lTriggerCloseNet;
                                pActor->triggerID = triggerID;
                            }

                            switch (cObjectType) {
                                case OBJECT_TYPE_NPC_SERVICE:
                                    pActor->SetActorType(GcActor::ENT_NPC_SERVICE);
                                    break;
                                case OBJECT_TYPE_CREATURE:
                                    if (cFlag & SYNC_FLAG_USER)
                                        pActor->SetActorType(GcActor::ENT_USER);
                                    else if (cFlag & SYNC_FLAG_PET)
                                        pActor->SetActorType(GcActor::ENT_PET);
                                    else if (cFlag & SYNC_FLAG_CALL)
                                        pActor->SetActorType(GcActor::ENT_CALLNPC);
                                    else
                                        pActor->SetActorType(GcActor::ENT_MONSTER);
                                    break;
                                case OBJECT_TYPE_NPC_COMBATIVE:
                                    pActor->SetActorType(GcActor::ENT_NPC_COMBATIVE);
                                    break;
                                case OBJECT_TYPE_SPIRIT:
                                    pActor->SetActorType(GcActor::ENT_SPIRIT);
                                    break;
                            }
                            //����NPCͷ����̬��ʾ
                            if (pActor->NpcType() == GcActor::ENT_NPC_SERVICE) {
                                if (pActor->m_pNpc) {
                                    pActor->mBaseActor.m_pActorOverHead
                                        ? pActor->mBaseActor.m_pActorOverHead->NewActorOverHead(
                                              pActor->m_pNpc->mOverHead)
                                        : 0;
                                }
                            }
                            if (cFaction == FACTION_ZHOU) {
                                pActor->m_core.Faction = FACTION_ZHOU;
                            } else if (cFaction == FACTION_SHANG) {
                                pActor->m_core.Faction = FACTION_SHANG;
                            } else if (cFaction == FACTION_NEUTRAL) {
                                pActor->m_core.Faction = FACTION_NEUTRAL;
                            }

                            if ((cFlag & SYNC_FLAG_USER) == 0) {
                                if (pActor->m_pNpc) {
                                    //ɽ��������������
                                    std::string taskName = "";
                                    if (pActor->m_pNpc->Name == "ɽ��") {
                                        if (m_pPlayer) {
                                            taskName += m_pPlayer->GetName();
                                            taskName += "��";
                                            taskName += pActor->m_pNpc->Name;
                                        }
                                        pActor->SetName(taskName.c_str());
                                    } else {
                                        pActor->SetName(pActor->m_pNpc->Name.c_str());
                                    }

                                    if (pActor->IsTrigger()) {
                                        pActor->m_pHUD->Hide();
                                    }
                                }
                            }

                            if (cFlag & SYNC_FLAG_DIRECTION) {
                                RtgMatrix16 _m16 = *pActor->GetMatrix();
                                rtgDecodeUprightDirection(_m16, Direction);
                                pActor->SetMatrix(_m16);
                            }

                            if (lObjectID == m_dwPetID) {
                                m_pGcPet->CallOutPet(m_nPetItemID, pActor);

                                if (m_cRideRetard == 2) {
                                    m_cRideRetard = 0;
                                    if (m_pPlayer->GetRideState() == GcActor::ERS_NONE)
                                        m_pPlayer->Ride(m_pPet, true);
                                }
                                //m_nPetItemID = 0;
                            }

                            pActor->MoveToNow((float)X, (float)Y);

                            if (pActor->NpcType() == GcActor::ENT_USER) {
                                CG_CmdPacket* pPacket = NetBeginWrite();
                                pPacket->WriteShort(c2r_get_alterable);
                                pPacket->WriteLong(lObjectID);
                                NetSend(pPacket);
                            }

                            /*if (pActor->NpcType() == GcActor::ENT_PET)
							{
								CG_CmdPacket* pPacket = NetBeginWrite();
								pPacket->WriteShort(c2r_get_model_info);
								pPacket->WriteLong(lObjectID);
								NetSend(pPacket);
							}
							else */
                            if (pActor->NpcType() == GcActor::ENT_SPIRIT) {
                                std::string name = pActor->m_pNpc->Name + "-Ԫ��";
                                pActor->SetName(name.c_str());
                                pActor->GetGraph()->SetVisible(0.2f);
                                pActor->mBaseActor.PlayEffect("spiritEffect", true, "root",
                                                              RtgVertex3(0.f, 0.f, 0.f), true);
                            }

                            if (cMoveFlag & MOVE_STATE_STATIC) {
                                // pActor->Command(GcActor::READY_BORN_COMMAND);
                            }

                            if (!m_listPlayEffectActor.empty()) {
                                std::list<PlayEffectActorList>::iterator it =
                                    m_listPlayEffectActor.begin();
                                DWORD dwNowTime = rtGetMilliseconds();
                                for (; it != m_listPlayEffectActor.end();) {
                                    if (dwNowTime - (*it).dwStartTime > (*it).dwWaitTime) {
                                        it = m_listPlayEffectActor.erase(it);
                                        continue;
                                    } else if ((*it).dwUserID == pActor->ID()) {
                                        pActor->mBaseActor.PlayEffect(
                                            (*it).szEffect.c_str(), (*it).bLink,
                                            (*it).szLinkSlot.c_str(), (*it).vctOffset, (*it).bLoop,
                                            (*it).fSpeed, (*it).szPoseName.c_str());
                                        it = m_listPlayEffectActor.erase(it);
                                        continue;
                                    } else {
                                        it++;
                                    }
                                }
                            }
                        }
                        unguard;
                    } else {
                        //					pActor->m_aiStateForDebug = aiState;

                        if (cFlag & SYNC_FLAG_DIRECTION) {
                            //if (strcmp(pActor->GetName(), "����ɽ����")==0)
                            //{
                            //	LOG1("Recv the guarder's dir: %d", Direction);
                            //}
                            //rtgDecodeUprightDirection(*(pActor->Matrix()),Direction);
                        }

                        pActor->m_cInTeam = bInTeam;
                        pActor->m_bIsCombat = lCombat;
                        pActor->m_cIsDeadNet = bDead;
                        /*pActor->SkillHideModel(IsHiding);*/
                        //pActor->m_fVisible = IsHiding;
                        if (cFlag & SYNC_FLAG_METIER_TAOIST) {
                            if (pActor->m_cShapeshiftNet != lShapeshift) {
                                if (lShapeshift)
                                    pActor->m_cShapeshiftNet = 2;
                                else
                                    pActor->m_cShapeshiftNet = 0;
                                if (!pActor->IsShapeshifting()) {
                                    pActor->m_cShapeshift = pActor->m_cShapeshiftNet;
                                }
                            }
                        }

                        if (cFlag & SYNC_FLAG_TRIGGER) {
                            pActor->m_lTriggerCloseNet = lIsClose;
                            pActor->triggerID = triggerID;
                        }

                        if ((cMoveFlag & MOVE_STATE_STATIC) == 0) {
                            if (pActor != m_pPlayer && pActor->GetRideState() != GcActor::ERS_PET) {
                                pActor->m_bTrace = bTrace ? true : false;
                                pActor->AddCommandMoveTo(
                                    float(X), float(Y), float(Z),
                                    (dwServerTime - dwClientTime + 35) / 1000.f, 0.f);
                            }
                        }
                        if (pActor->NpcType() == GcActor::ENT_PET &&
                            pActor->m_ePetType == PET_TYPE_NULL) {
                            CG_CmdPacket* pPacket = NetBeginWrite();
                            pPacket->WriteShort(c2r_get_model_info);
                            pPacket->WriteLong(lObjectID);
                            NetSend(pPacket);
                        }
                    }
                    PopCallStack();

                    // ��⵱ǰ����״̬�Ƿ��clientһ���������һ������Ҫ����
                    PushCallStack(csn_GcActor_AddCommandMoveTo);
                    if (pActor && pActor->IsValid()) {
                        if (cMoveFlag & MOVE_STATE_RIDE) {
                            if (pActor->GetRidePetID() != lSlaveID) {
                                pPet = m_ActorManager.FindAll(lSlaveID);
                                if (pPet && pActor->IsValid()) {
                                    pActor->Ride(pPet);
                                    pActor->AddCommandMoveTo(float(X), float(Y), float(Z),
                                                             (dwServerTime - dwClientTime) / 1000.f,
                                                             0.f);
                                }
                            }
                        } else {
                            if (pActor->GetRideState() == GcActor::ERS_ACTOR) {
                                pActor->UnRide();
                                pActor->AddCommandMoveTo(float(X), float(Y), float(Z),
                                                         (dwServerTime - dwClientTime) / 1000.f,
                                                         0.f);
                            }
                        }
                        if (cMoveFlag & (PK_STATE_RED_NAME | PK_STATE_YELLOW_NAME)) {
                            if (cMoveFlag & PK_STATE_RED_NAME) {
                                pActor->SetPKState(PK_STATE_RED_NAME);
                            } else {
                                pActor->SetPKState(PK_STATE_YELLOW_NAME);
                            }
                        } else {
                            pActor->SetPKState(PK_STATE_NORMAL);
                        }
                    }

                    bool          bRequestModelInfo = false;
                    static string strUnionID = "-1";
                    static DWORD  nOldTime = GetTickCount();
                    // ����ģ����Ϣ
                    if (cFlag & SYNC_FLAG_COMPLEX_MODEL) {
                        if (pActor == NULL)  //�����岻��������
                        {
                            bRequestModelInfo = true;
                        } else {
                            if (pActor->mItemChangeCount !=
                                ModelChangeCount)  //�������ĸ�����������Ƿ����
                            {
                                pActor->mItemChangeCount = ModelChangeCount;
                                bRequestModelInfo = true;
                            }

                            if (pActor->m_unionIcon != strUnionID.c_str()) {
                                strUnionID = pActor->m_unionIcon;
                                bRequestModelInfo = true;
                            }
                        }

                        if (GetTickCount() - nOldTime > 5000)  //5���ȡһ��
                        {
                            bRequestModelInfo = true;
                            nOldTime = GetTickCount();
                        }

                        if (bRequestModelInfo) {
                            guard;
                            CG_CmdPacket* pPacket = NetBeginWrite();
                            pPacket->WriteShort(c2r_get_model_info);
                            pPacket->WriteLong(lObjectID);
                            NetSend(pPacket);
                            unguard;
                        }
                    } else {
                        pActor->SetModel(wModelID, 0);
                    }
                    PopCallStack();

                    pActor = m_ActorManager.FindAll(lObjectID);
                    if (pActor) {
                        m_ActorManager.OnDownloadActorSnapshot((GcSynchroActor*)pActor, bBeforBorn);
                        setEffect.clear();
                        EXREAD_char(cCnt);
                        for (c = 0; c < cCnt; c++) {
                            EXREAD_short(wID);
                            EXREAD_ulong(wTime);  //gao buff ʱ���ȡ
                            setEffect.insert(
                                make_pair(wID, wTime));  // gao 2009.12.31 ��ȡ����ģʽ����
                        }
                        if (bRequestModelInfo) {
                            pActor->m_mapSetEffect = setEffect;
                        } else {
                            if (pActor->m_Skill.NetReadEffect(setEffect)) {
                                if (pActor->ID() == g_layerMain->m_formMonInfo->GetActorID())
                                    pActor->m_Skill.UpdateEffectUIForActor(0);
                                else if (pActor->ID() == g_layerMain->m_formCreInfo->GetActorID())
                                    pActor->m_Skill.UpdateEffectUIForActor(1);
                                if (GetWorld()->m_pPet && pActor->ID() == GetWorld()->m_pPet->ID())
                                    pActor->m_Skill.UpdateEffectUIForActor(2);
                            }
                        }
                        if (cFlag & SYNC_FLAG_BOSS) {
                            pActor->m_lBossHp = lBossHp;
                            pActor->m_lBossMaxHp = lBossMaxHp;
                            if (g_layerMain->m_formCreInfo->m_nActorID == pActor->ID()) {
                                g_layerMain->m_formCreInfo->m_nHpRemain = lBossHp;
                            }
                            pActor->m_lLev = lLev;
                        } else {
                            if (m_pPet == pActor || pActor->NpcType() == GcActor::ENT_CALLNPC) {
                                if (pActor->GetRideState() == GcActor::ERS_PET) {
                                    if (pActor->GetMaster() && pActor->GetMaster()->m_pHUD)
                                        pActor->GetMaster()->m_cHpRate = cHpRate;
                                } else {
                                    pActor->m_cHpRate = cHpRate;
                                    pActor->m_cMpRate = cMpRate;  // ʣ��MP�ı���(ֵΪ0��100)
                                    pActor->m_lLev = lLev;        // �ȼ�
                                }
                            } else {
                                pActor->m_cHpRate = cHpRate;
                                pActor->m_cMpRate = cMpRate;
                                pActor->m_actorID = actorID;
                                pActor->m_lLev = lLev;
                                pActor->m_HeadImageID = HeadImageID;
                            }
                        }

                        //----------add start by tony 05.06.29----------------//
                        if (cFlag & SYNC_FLAG_WAR) {
                            if (pActor->NpcType() == GcActor::ENT_MONSTER) {
                                if (pActor->m_pHUD) {
                                    pActor->m_pHUD->SetGuildText(std::string(p_union_name));
                                }
                            }
                        }
                        //----------add end   by tony 05.06.29----------------//
                    }
                } else {
                    m_pPlayer->m_bIsDead = bDead;
                    m_pPlayer->m_cInTeam = bInTeam;
                    m_pPlayer->m_bIsCombat = lCombat;
                    m_pPlayer->m_cIsDeadNet = bDead;
                    /*m_pPlayer->SkillHideModel(IsHiding);*/
                    if (cFlag & SYNC_FLAG_METIER_TAOIST) {
                        if (m_pPlayer->m_cShapeshiftNet != lShapeshift) {
                            if (lShapeshift)
                                m_pPlayer->m_cShapeshiftNet = 2;
                            else
                                m_pPlayer->m_cShapeshiftNet = 0;
                            if (!m_pPlayer->IsShapeshifting()) {
                                m_pPlayer->m_cShapeshift = m_pPlayer->m_cShapeshiftNet;
                            }
                        }
                    }
                    if (bDead == 0) {
                        m_pPlayer->m_cRebirthType = 0;
                    }
                    setEffect.clear();
                    EXREAD_char(cCnt);
                    for (c = 0; c < cCnt; c++) {
                        EXREAD_short(wID);
                        EXREAD_ulong(wTime);
                        setEffect.insert(make_pair(wID, wTime));  // gao 2009.12.31 ��ȡ����ģʽ����
                    }
                    if (m_pPlayer->m_Skill.NetReadEffect(setEffect)) {
                        m_pPlayer->m_Skill.UpdateEffectUI();
                    }
                }

                unguard;
            } break;

            // ����
            case OBJECT_TYPE_ITEM: {
                guard_name(����);
                long lItemID;
                BYTE itemColor;
                BYTE dropStyle;
                BYTE itemBind;
                EXREAD_long(lItemID);
                EXREAD_byte(itemColor);
                EXREAD_byte(itemBind);
                // EXREAD_short(x);
                // EXREAD_short(y);
                EXREAD_shortex(x);
                EXREAD_shortex(y);
                X = lOriginPosition[0] + x;
                Y = lOriginPosition[1] + y;
                Z = lOriginPosition[2] + z;
                EXREAD_byte(Direction);
                EXREAD_byte(dropStyle);
                // add to client object list and create model
                m_synchroItems.OnDownloadActorSnapshot(lObjectID, lItemID, itemColor, itemBind, X,
                                                       Y, Z, Direction, dropStyle);
                unguard;
            } break;

            default:
                LOG1("�������: ����ʶ���������: %d\n", cObjectType);
                bError = true;
                break;
        }
        if (bError)
            break;
    }
    m_ActorManager.OnEndDownloadSnapshot();
    m_synchroItems.OnEndDownloadSnapshot();

    unguard;
}

void GcWorld::OnNetDownloadEvent(CG_CmdPacket* pPacket, DWORD dwClientTime) {
    guard;
    long  lID;
    long  ServerTime;
    short EventType;

    READ_long(ServerTime);
    READ_short(EventType);

    switch (EventType) {
        // �����¼�
        case EVENT_STATIC_DIALOG:
            OnNetDownloadHelp(pPacket, ServerTime, dwClientTime);
            break;
        case EVENT_ATTACK:
            OnNetDownloadAttack(pPacket, ServerTime, dwClientTime);
            break;
        case EVENT_SET_POSITION:
            CHECKEX("Unknowe net event");
            break;
        case EVENT_LEVEL_UP: {
            READ_long(lID);
            GcActor* pActor = FindActor((DWORD)lID);
            if (pActor) {
                pActor->mBaseActor.PlayEffect("elevup", true, NULL, RtgVertex3(0, 0, 0));

                if (lID == m_pPlayer->ID()) {
                    g_layerMain->AddRemindIcon(UIRemind_LevelUp);
                }
            }
        } break;
        case EVENT_BORN: {
            READ_long(lID);  // ������ID
            m_ActorManager.ActorBorn((DWORD)lID);
        } break;
        case EVENT_DEAD: {
            READ_long(lID);  // ������ID
            long dead;
            pPacket->SerializeBit(dead, 3);
            m_ActorManager.ActorDie((DWORD)lID, dead);
        } break;
        case EVENT_PLAY_POSE:
            if (m_pPlayer) {
                DWORD dwUserID;
                short sIdx;
                char  cDir;
                READ_ulong(dwUserID);
                READ_short(sIdx);
                READ_char(cDir);
                GcActor* pActor = FindActor(dwUserID);
                if (pActor) {
                    if (pActor->IsIdel()) {
                        DWORD       dwIdx = sIdx << 16 | (cDir & 0x0000FFFF);
                        RtgMatrix16 _m16 = *pActor->GetMatrix();
                        rtgDecodeUprightDirection(_m16, (BYTE)cDir);
                        pActor->SetMatrix(_m16);
                        pActor->mBaseActor.PlayPose(GcBaseActor::POSE_FUNACTION, false,
                                                    (SSkill*)dwIdx);
                    }
                }
            }
            break;
        case EVENT_CHECK_PROCESS: {
            m_bCheckProcess = true;
            char* pStr;
            READ_long(m_dwCheckProcessID);
            READ_string(pStr);
            m_szCheckProcess = pStr;
        } break;
    }
    unguard;
}

void GcWorld::OnNetDownloadHelp(CG_CmdPacket* pPacket, DWORD dwServerTime, DWORD dwClientTime) {
    guard;
    char* szFileName;
    char  i, cCnt;
    char *szName, *szValue;

    READ_string(szFileName);
    if (pPacket->ReadByte(&cCnt)) {
        for (i = 0; i < cCnt; ++i) {
            READ_string(szName);
            READ_string(szValue);
            m_mapTranslateMacro[szName] = szValue;
        }
    }

    if (g_layerMain) {
        if (g_layerMain->m_formHelp)
            g_layerMain->m_formHelp->Show();
        if (g_layerMain->m_viewHelp)
            g_layerMain->m_viewHelp->LoadFromFile(szFileName);
    }
    unguard;
}

/*
long		����ID
short		Hp
short		MaxHp
short		Mp
short		MaxMp
char		Lev
long		Exp
long		NextLevelExp
*/
void GcWorld::OnNetDownloadShowInfo(CG_CmdPacket* pPacket) {
    // delete by carl
}

void GcWorld::OnNetDownloadRidePet(CG_CmdPacket* pPacket) {
    guard;
    char cOp;
    long lObjectID;

    READ_byte(cOp);  // 1�� 0��
    READ_long(lObjectID);

    if (m_pPet == NULL) {
        m_cRideRetard = cOp + 1;
        return;
    }

    if (cOp == 1) {
        if (m_pPlayer->GetRideState() != GcActor::ERS_NONE)
            return;
        m_pPlayer->Ride(m_pPet);
    } else {
        if (m_pPlayer->GetRideState() == GcActor::ERS_NONE)
            return;
        m_pPlayer->UnRide();
    }
    //Heten �޸��������ﲻ����
    if (m_pPlayer->GetAutobattle()) {
        m_pPlayer->ResetTargetID();
    }
    //end
    unguard;
}

void GcWorld::OnNetDownloadDialogHTML(DWORD dwNpcID, const char* szString, const char* szName,
                                      const char* szValue, DWORD taskid, char cFinished) {
    guard;
    //���ʯ��������ʾ
    if (dwNpcID == 74) {
        CTaskInfo task = m_pPlayer->m_task;
        if (!task.IsDoneGuild(eUsePet2) && task.IsDoneGuild(eUsePet1)) {
            RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
            RtwRect rectDest = g_layerMain->m_btnAccept->GetClientRect();
            rectDest.left -= rectSrc.getWidth() / 2 + 110;
            rectDest.top += (rectDest.getHeight() / 2 - 70);
            g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide3->SetFocus();
            g_layerMain->m_formGuide3->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_03_02.html";
            g_layerMain->m_viewGuide3->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eNPCAcceptTask);
            NetSend(cmd);
        }
    }
    m_dwInteractiveNPCID = dwNpcID;
    if (cFinished == 1) {
        m_upTaskId = taskid;
    } else if (cFinished == 2) {
        m_upTaskId = taskid;
    } else {
        m_upTaskId = 0;
    }

    if (g_layerMain->m_formHelp)
        g_layerMain->m_formHelp->Show();
    std::string taskPicture = "ui_texture/";
    SNpc*       pNpc = g_TableNpc.FindNpc(GetWorld()->GetActualNPCId());
    taskPicture += pNpc->taskPic;

    RtwImage* pImage = NULL;
    if (!pNpc->taskPic.empty()) {
        pImage = g_workspace.getImageFactory()->createImage(taskPicture);
        pImage->SetBlend(true);
    }

    if (szName && szValue) {
        m_mapTranslateMacro[szName] = szValue;
    }

    //��ʾҳ��
    if (g_layerMain) {
        if (!taskid) {
            //����NPC�Ի�ҳ���UI��ʾ
            if (g_layerMain->m_formGuide1->IsVisible()) {
                g_layerMain->m_formGuide1->Hide();
            }
            if (g_layerMain->m_formGuide2->IsVisible()) {
                g_layerMain->m_formGuide2->Hide();
            }
            CTaskInfo task = m_pPlayer->m_task;
            if (!task.IsDoneGuild(eNPCTalk)) {
                if (g_layerMain->m_formGuide1->IsVisible()) {
                    g_layerMain->m_formGuide1->Hide();
                }
                CG_CmdPacket* cmd = NetBeginWrite();
                cmd->WriteShort(c2r_Guide);
                cmd->WriteShort(eNPCTalk);
                NetSend(cmd);
            }
            if (g_layerMain->m_formHelp2->IsVisible()) {
                g_layerMain->m_formHelp2->Hide();
            }
            LOAD_UI("fmhtmlhelp.lbborder4")->SetBackgroundImage(pImage);
            if (g_layerMain->m_formHelp)
                g_layerMain->m_formHelp->Show();
            if (g_layerMain->m_viewHelp) {
                char cTmp1024[1024], path[100];
                if (strncmp(szString, "<html>", 6) == 0) {
                    g_layerMain->m_viewHelp->SetUserData(dwNpcID);
                    g_layerMain->m_viewHelp->LoadFromString(szString);
                } else {
                    if (cFinished == 3) {
                        if (g_layerMain->m_formHelp->IsVisible()) {
                            g_layerMain->m_formHelp->Hide();
                        }
                        if (g_layerMain->m_formMainLineTask) {
                            g_layerMain->m_formMainLineTask->Show();
                            g_layerMain->m_formBtn1->Hide();
                            g_layerMain->m_formBtn2->Hide();
                            g_layerMain->BuildMainTaskList();
                        }
                        char* token = NULL;
                        int   sep;
                        rt2_strncpy(cTmp1024, szString, 1024);
                        if (strrchr(cTmp1024, '/')) {
                            g_layerMain->m_viewDesc->ClearPath();
                            std::string s1, s2;
                            s1 = cTmp1024;
                            s2 = strrchr(cTmp1024, '/');
                            sep = s1.size() - s2.size();
                            //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                            path[0] = '\0';
                            rt2_strncpy2(path, cTmp1024, sep);
                            path[sep] = '\0';
                        }
                        g_layerMain->m_viewDesc->AddPath(path);
                        g_layerMain->m_viewDesc->LoadFromFile(szString);
                        g_layerMain->m_viewDesc->SetUserData(dwNpcID);
                    } else if (cFinished == 4) {
                        char* token = NULL;
                        int   sep;
                        rt2_strncpy(cTmp1024, szString, 1024);
                        if (strrchr(cTmp1024, '/')) {
                            g_layerMain->m_viewHelp->ClearPath();
                            std::string s1, s2;
                            s1 = cTmp1024;
                            s2 = strrchr(cTmp1024, '/');
                            sep = s1.size() - s2.size();
                            //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                            path[0] = '\0';
                            rt2_strncpy2(path, cTmp1024, sep);
                            path[sep] = '\0';
                        }
                        g_layerMain->m_viewHelp->AddPath(path);
                        g_layerMain->m_viewHelp->LoadFromFile(szString);
                        g_layerMain->m_viewHelp->SetUserData(dwNpcID);
                    }
                }
            }
        } else {
            //�ж�����������֧������
            STaskDesc* desc = g_TableTask.FindTask(taskid);
            if (!desc)
                return;
            if (desc->m_TaskGroupID >= 0 && desc->m_TaskGroupID <= 39) {
                bMainLine = true;
            } else {
                bMainLine = false;
            }
            //������Ķ�������
            if (cFinished == 1) {
                if (bMainLine) {
                    if (g_layerMain->m_formHelp->IsVisible()) {
                        g_layerMain->m_formHelp->Hide();
                    }
                    if (g_layerMain->m_formMainLineTask) {
                        g_layerMain->m_formMainLineTask->Show();
                        g_layerMain->m_formBtn1->Show();
                        g_layerMain->m_formBtn2->Hide();
                        g_layerMain->BuildMainTaskList();
                    }
                    if (g_layerMain->m_formMainLineTask) {
                        char cTmp1024[1024], path[100];
                        if (strncmp(szString, "<html>", 6) == 0) {
                            g_layerMain->m_viewDesc->SetUserData(dwNpcID);
                            g_layerMain->m_viewDesc->LoadFromString(szString);
                        } else {
                            char* token = NULL;
                            int   sep;
                            rt2_strncpy(cTmp1024, szString, 1024);
                            if (strrchr(cTmp1024, '/')) {
                                g_layerMain->m_viewDesc->ClearPath();
                                std::string s1, s2;
                                s1 = cTmp1024;
                                s2 = strrchr(cTmp1024, '/');
                                sep = s1.size() - s2.size();
                                //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                                path[0] = '\0';
                                rt2_strncpy2(path, cTmp1024, sep);
                                path[sep] = '\0';
                            }
                            g_layerMain->m_viewDesc->AddPath(path);
                            g_layerMain->m_viewDesc->LoadFromFile(szString);
                            g_layerMain->m_viewDesc->SetUserData(dwNpcID);
                        }
                    }
                } else {
                    CTaskInfo task = m_pPlayer->m_task;
                    if (!task.IsDoneGuild(eNPCAcceptTask) && task.IsDoneGuild(eNPCTaskTalk)) {
                        //�������������UI��ʾ
                        if (g_layerMain->m_formGuide4->IsVisible()) {
                            g_layerMain->m_formGuide4->Hide();
                        }
                        RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
                        RtwRect rectDest = g_layerMain->m_btnAccept->GetClientRect();
                        rectDest.left -= rectSrc.getWidth() / 2 + 110;
                        rectDest.top += (rectDest.getHeight() / 2 - 70);
                        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
                        g_layerMain->m_formGuide3->SetFocus();
                        g_layerMain->m_formGuide3->Show();
                        std::string fileName = "task\\xinshouyindao\\xinshou_03_02.html";
                        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
                        ResetHideTime();
                        bCurOperate = true;
                        CG_CmdPacket* cmd = NetBeginWrite();
                        cmd->WriteShort(c2r_Guide);
                        cmd->WriteShort(eNPCAcceptTask);
                        NetSend(cmd);
                    }
                    if (g_layerMain->m_formHelp->IsVisible()) {
                        g_layerMain->m_formHelp->Hide();
                    }
                    LOAD_UI("fmhtmlhelp2.lbborder4")->SetBackgroundImage(pImage);
                    if (g_layerMain->m_formHelp2)
                        g_layerMain->m_formHelp2->Show();
                    if (g_layerMain->m_viewHelp2) {
                        char cTmp1024[1024], path[100];
                        if (strncmp(szString, "<html>", 6) == 0) {
                            g_layerMain->m_viewHelp->SetUserData(dwNpcID);
                            g_layerMain->m_viewHelp->LoadFromString(szString);
                        } else {
                            char* token = NULL;
                            int   sep;
                            rt2_strncpy(cTmp1024, szString, 1024);
                            if (strrchr(cTmp1024, '/')) {
                                g_layerMain->m_viewHelp->ClearPath();
                                std::string s1, s2;
                                s1 = cTmp1024;
                                s2 = strrchr(cTmp1024, '/');
                                sep = s1.size() - s2.size();
                                //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                                path[0] = '\0';
                                rt2_strncpy2(path, cTmp1024, sep);
                                path[sep] = '\0';
                            }
                            g_layerMain->m_viewHelp2->AddPath(path);
                            g_layerMain->m_viewHelp2->LoadFromFile(szString);
                            g_layerMain->m_viewHelp2->SetUserData(dwNpcID);
                        }
                    }
                }
            }
            //���������Ķ�������
            else if (cFinished == 2) {
                if (bMainLine) {
                    if (g_layerMain->m_formHelp->IsVisible()) {
                        g_layerMain->m_formHelp->Hide();
                    }
                    if (g_layerMain->m_formMainLineTask) {
                        g_layerMain->m_formMainLineTask->Show();
                        g_layerMain->m_formBtn1->Hide();
                        g_layerMain->m_formBtn2->Show();
                        g_layerMain->BuildMainTaskList();
                    }
                    if (g_layerMain->m_formMainLineTask) {
                        char cTmp1024[1024], path[100];
                        if (strncmp(szString, "<html>", 6) == 0) {
                            g_layerMain->m_viewDesc->SetUserData(dwNpcID);
                            g_layerMain->m_viewDesc->LoadFromString(szString);
                        } else {
                            char* token = NULL;
                            int   sep;
                            rt2_strncpy(cTmp1024, szString, 1024);
                            if (strrchr(cTmp1024, '/')) {
                                g_layerMain->m_viewDesc->ClearPath();
                                std::string s1, s2;
                                s1 = cTmp1024;
                                s2 = strrchr(cTmp1024, '/');
                                sep = s1.size() - s2.size();
                                //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                                path[0] = '\0';
                                rt2_strncpy2(path, cTmp1024, sep);
                                path[sep] = '\0';
                            }
                            g_layerMain->m_viewDesc->AddPath(path);
                            g_layerMain->m_viewDesc->LoadFromFile(szString);
                            g_layerMain->m_viewDesc->SetUserData(dwNpcID);
                        }
                    }
                } else {
                    if (g_layerMain->m_formHelp->IsVisible()) {
                        g_layerMain->m_formHelp->Hide();
                    }
                    LOAD_UI("fmhtmlhelp3.lbborder4")->SetBackgroundImage(pImage);
                    if (g_layerMain->m_formHelp3)
                        g_layerMain->m_formHelp3->Show();
                    if (g_layerMain->m_viewHelp3) {
                        char cTmp1024[1024], path[100];
                        if (strncmp(szString, "<html>", 6) == 0) {
                            g_layerMain->m_viewHelp->SetUserData(dwNpcID);
                            g_layerMain->m_viewHelp->LoadFromString(szString);
                        } else {
                            char* token = NULL;
                            int   sep;
                            rt2_strncpy(cTmp1024, szString, 1024);
                            if (strrchr(cTmp1024, '/')) {
                                g_layerMain->m_viewHelp->ClearPath();
                                std::string s1, s2;
                                s1 = cTmp1024;
                                s2 = strrchr(cTmp1024, '/');
                                sep = s1.size() - s2.size();
                                //sep = strcspn(cTmp1024,strrchr(cTmp1024,'/'));
                                path[0] = '\0';
                                rt2_strncpy2(path, cTmp1024, sep);
                                path[sep] = '\0';
                            }
                            g_layerMain->m_viewHelp3->AddPath(path);
                            g_layerMain->m_viewHelp3->LoadFromFile(szString);
                            g_layerMain->m_viewHelp3->SetUserData(dwNpcID);
                        }
                    }
                }
            }
        }
    }
    unguard;
}

void GcWorld::OnNetDownloadDialogScript(short sType, char* sShowText, char* sCommandText) {
    switch (sType) {
        case 1:
            if (g_layerMain->m_formMessageBox1)
                g_layerMain->m_formMessageBox1->Show();
            if (g_layerMain->m_viewMessage1) {
                char cTmp1024[1024], path[100];
                if (strncmp(sShowText, "<html>", 6) == 0) {
                    g_layerMain->m_viewMessage1->LoadFromString(sShowText);
                } else {
                    char* token = NULL;
                    int   sep;
                    rt2_strncpy(cTmp1024, sShowText, 1024);
                    if (strrchr(cTmp1024, '/')) {
                        g_layerMain->m_viewMessage1->ClearPath();
                        std::string s1, s2;
                        s1 = cTmp1024;
                        s2 = strrchr(cTmp1024, '/');
                        sep = s1.size() - s2.size();
                        path[0] = '\0';
                        rt2_strncpy2(path, cTmp1024, sep);
                        path[sep] = '\0';
                    }
                    g_layerMain->m_viewMessage1->AddPath(path);
                    g_layerMain->m_viewMessage1->LoadFromFile(sShowText);
                }
            }
            SetCommandText(sCommandText);
            break;
        case 2:
            if (g_layerMain->m_formMessageBox2)
                g_layerMain->m_formMessageBox2->Show();
            if (g_layerMain->m_viewMessage2) {
                char cTmp1024[1024], path[100];
                if (strncmp(sShowText, "<html>", 6) == 0) {
                    g_layerMain->m_viewMessage2->LoadFromString(sShowText);
                } else {
                    char* token = NULL;
                    int   sep;
                    rt2_strncpy(cTmp1024, sShowText, 1024);
                    if (strrchr(cTmp1024, '/')) {
                        g_layerMain->m_viewMessage2->ClearPath();
                        std::string s1, s2;
                        s1 = cTmp1024;
                        s2 = strrchr(cTmp1024, '/');
                        sep = s1.size() - s2.size();
                        path[0] = '\0';
                        rt2_strncpy2(path, cTmp1024, sep);
                        path[sep] = '\0';
                    }
                    g_layerMain->m_viewMessage2->AddPath(path);
                    g_layerMain->m_viewMessage2->LoadFromFile(sShowText);
                }
            }
            SetCommandText(sCommandText);
            break;
        default:
            break;
    }
}

bool GcWorld::TranslateTaskMacro(string& result, const char* text, void* data) {
    guard;
    if (strcmp(text, "NPC_NAME") == 0) {
        result = this->m_InteractiveNPCName;
        return true;
    } else if (strcmp(text, "USER_NAME") == 0) {
        result = this->m_pPlayer->GetName();
        return true;
    } else if (strcmp(text, "GOAL") == 0) {
        int        task_id = g_layerMain->m_formTask->GetSelectedTaskID();
        STaskDesc* desc = g_TableTask.FindTask(task_id);
        if (!desc)
            return false;

        result = "";
        result += "<p>";
        result += "<img src=\"ui/textures/28_b.tga\" width=\"420\" height=\"18\" source=\"\"/>";
        result += "</p>";

        int                            bDoing = 0;
        CTaskInfo*                     task = &GetWorld()->m_pPlayer->m_task;
        std::list<STaskInfo>::iterator it;
        for (it = task->m_taskList.begin(); it != task->m_taskList.end(); it++) {
            if ((*it).Id == task_id) {
                bDoing = (*it).IsDoing;
                break;
            }
        }

        if (bDoing == 3) {
            for (size_t i = 0; i < desc->dynamicText2.size(); ++i) {
                result += "<p>";
                result += desc->dynamicText2[i].key;
                result += " ";
                for (size_t j = 0; j < MAX_TASK_PARAM_SIZE; ++j) {
                    result += desc->dynamicText2[i].param[j];
                    if (!desc->dynamicText2[i].param[j].empty()) {
                        result += " ";
                    }
                }
                result += "</p>";
            }
        } else {
            for (size_t i = 0; i < desc->dynamicText.size(); ++i) {
                result += "<p>";
                result += desc->dynamicText[i].key;
                result += " ";
                for (size_t j = 0; j < MAX_TASK_PARAM_SIZE; ++j) {
                    result += desc->dynamicText[i].param[j];
                    if (!desc->dynamicText[i].param[j].empty()) {
                        result += " ";
                    }
                }
                int      num = 0;
                RtString tmp;
                if (desc->dynamicNum.empty()) {
                    if (desc->taskReq[i].key == "killmonster") {
                        num = atoi(desc->taskReq[i].param[0].c_str());
                        if (num != 0) {
                            int monNum = it->GetCurKillMonsterNum();
                            tmp.Format(":%d/%d", monNum, num);
                        }
                    } else if (desc->taskReq[i].key == "useitem") {
                        num = atoi(desc->taskReq[i].param[0].c_str());
                        if (it->IsUseItemByID(num)) {
                            tmp.Format(":1/1");
                        } else {
                            tmp.Format(":0/1");
                        }
                    }
                } else {
                    result += ":";
                    GetWorld()->m_pPlayer->mItem.m_MissionBag.FindType(
                        atoi(desc->dynamicItem[i].key.c_str()), ItemColor_All, &num);
                    if (num == 0)
                        GetWorld()->m_pPlayer->mItem.m_Bag.FindType(
                            atoi(desc->dynamicItem[i].key.c_str()), ItemColor_All, &num);
                    tmp.Format("%d/%d", num, atoi(desc->dynamicNum[i].key.c_str()));
                }
                result += tmp;
                result += "</p>";
            }
        }
        return true;
    } else if (strcmp(text, "REWARD") == 0) {
        result = "";
        return true;
        int        task_id = g_layerMain->m_formTask->GetSelectedTaskID();
        STaskDesc* desc = g_TableTask.FindTask(task_id);
        if (!desc)
            return false;

        std::vector<STaskKey>& table = desc->fixReward;

        result = "";
        result += "<p><font color =\"#00FF00\">[";
        result += R(MSG_TASK_PEG_HORTATION);
        result += "]</font></p>";

        for (size_t i = 0; i < table.size(); ++i) {
            int         gold = 0;
            int         exp = 0;
            DWORD       taskId = 0;
            DWORD       itemId = 0;
            int         itemNum = 0;
            DWORD       skillId = 0;
            std::string wenReward = "";
            std::string gongReward = "";
            int         jobReward = 0;
            int         creditNum1;
            int         creditNum2;
            int         credit0 = 0;
            int         credit1 = 0;
            int         credit2 = 0;
            int         credit3 = 0;
            int         credit40 = 0;
            int         credit41 = 0;
            int         credit42 = 0;
            int         credit43 = 0;
            int         credit44 = 0;
            int         credit45 = 0;

            if (table[i].key == "gold") {
                gold = atol(table[i].param[0].c_str());
                if (gold != 0) {
                    result = result + "<p>" + R(MSG_TASK_MONEY) +
                             std::string(rtFormatNumber(gold)) + "</p>";
                }
            } else if (table[i].key == "exp") {
                exp = atol(table[i].param[0].c_str());
                if (exp != 0) {
                    result = result + "<p>" + R(MSG_TASK_EXP) + std::string(rtFormatNumber(exp)) +
                             "</p>";
                }
            } else if (table[i].key == "task") {
                taskId = atol(table[i].param[0].c_str());
                if (taskId != 0) {
                    STaskDesc* desc = g_TableTask.FindTask(taskId);
                    if (desc) {
                        result = result + "<p>" + R(MSG_TASK_REWARDTASK) + desc->name + "</p>";
                    }
                }
            } else if (table[i].key == "item") {
                itemId = atol(table[i].param[0].c_str());
                itemNum = atol(table[i].param[1].c_str());
                if (itemId != 0) {
                    SItemBase* pItemClass = m_pItemManager->GetItemFromIndex(itemId);
                    if (pItemClass) {
                        result += "<p>";
                        result += R(MSG_TASK_REWARDITEM) + std::string(pItemClass->name);
                        if (itemNum != 0) {
                            result += " " + std::string(rtFormatNumber(itemNum));
                        }
                        result += "</p>";
                    }
                }
            } else if (table[i].key == "skill") {
                skillId = atol(table[i].param[0].c_str());
                if (skillId != 0) {
                    SSkill* pSkill = Skill()->FindSkill(skillId);
                    if (pSkill) {
                        result = result + "<p>" + R(MSG_TASK_REWARDSKILL) + pSkill->szName + "</p>";
                    }
                }
            } else if (table[i].key == "wenreward") {
                result = result + "<p>" + R(MSG_TASK_XIUWEI) + "</p>";
            } else if (table[i].key == "gongreward") {
                result = result + "<p>" + R(MSG_TASK_MENGONG) + "</p>";
            } else if (table[i].key == "jobrecord") {
                jobReward = atol(table[i].param[0].c_str());
                if (jobReward != 0) {
                    result = result + "<p>" + R(MSG_TASK_REWARDMENGONG) +
                             std::string(rtFormatNumber(jobReward)) + "</p>";
                }
            } else if (table[i].key == "credit") {
                creditNum1 = atol(table[i].param[0].c_str());
                if (creditNum1 == 0) {
                    credit0 = atol(table[i].param[1].c_str());
                    result = result + "<p>" + R(MSG_TASK_REWARDBANGGONG) +
                             std::string(rtFormatNumber(credit0)) + "</p>";
                } else if (creditNum1 == 1) {
                    credit1 = atol(table[i].param[1].c_str());
                    result = result + "<p>" + R(MSG_TASK_REWARDSUMING) +
                             std::string(rtFormatNumber(credit1)) + "</p>";
                } else if (creditNum1 == 2) {
                    credit2 = atol(table[i].param[1].c_str());
                    result = result + "<p>" + R(MSG_TASK_REWARDSOUL) +
                             std::string(rtFormatNumber(credit2)) + "</p>";
                } else if (creditNum1 == 3) {
                    credit3 = atol(table[i].param[1].c_str());
                    result = result + "<p>" + R(MSG_TASK_REWARDXIUWEI) +
                             std::string(rtFormatNumber(credit3)) + "</p>";
                } else if (creditNum1 == 4) {
                    creditNum2 = atol(table[i].param[1].c_str());
                    if (creditNum2 == 0) {
                        credit40 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDXIANYU) +
                                 std::string(rtFormatNumber(credit40)) + "</p>";
                    } else if (creditNum2 == 1) {
                        credit41 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDGUANFUSHENGWANG) +
                                 std::string(rtFormatNumber(credit41)) + "</p>";
                    } else if (creditNum2 == 2) {
                        credit42 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDQIANGMANSHENGWANG) +
                                 std::string(rtFormatNumber(credit42)) + "</p>";
                    } else if (creditNum2 == 3) {
                        credit43 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDZHENYAOSHENGWANG) +
                                 std::string(rtFormatNumber(credit43)) + "</p>";
                    } else if (creditNum2 == 4) {
                        credit44 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDFANSUISHENGWANG) +
                                 std::string(rtFormatNumber(credit44)) + "</p>";
                    } else if (creditNum2 == 5) {
                        credit45 = atol(table[i].param[2].c_str());
                        result = result + "<p>" + R(MSG_TASK_REWARDXIANMOSHENGWANG) +
                                 std::string(rtFormatNumber(credit45)) + "</p>";
                    }
                }
            }
        }

        for (size_t i = 0; i < table.size(); ++i) {
            int  id[5];
            bool bw = false;
            memset((void*)id, 0, sizeof(id));

            GetWorld()->m_pPlayer->ProcessRandomTaskReward(table[i], id, 5);

            for (int cnt = 0; (cnt < 5) && (id[cnt] != 0); cnt++) {
                SFBReward* pReward = g_TableFBReword.Find(id[cnt]);
                if (!pReward)
                    return false;

                if (!bw) {
                    result += "<p><font color =\"#00FF00\">";
                    //result += "[���н����п�ѡȡһ��]��";//by fox for string
                    result += R(MSG_TASK_HORTATION_CHOICE);
                    result += "</font></p>";
                    bw = true;
                    g_layerMain->m_formDupe->SetHaveSReward(true);
                }

                g_layerMain->m_formDupe->AddReward(id[cnt]);

                if (pReward->exp != 0) {
                    //result += "<p>" + std::string(rtFormatNumber(pReward->exp)) + "����ֵ" + "</p>";//by fox for string
                    result += "<p>" + std::string(rtFormatNumber(pReward->exp)) + R(MSG_TASK_EXP) +
                              "</p>";
                }
                if (pReward->gold != 0) {
                    //result += "<p>" + std::string(rtFormatNumber(pReward->gold)) + "��Ǯ</p>";//by fox for string
                    result += "<p>" + std::string(rtFormatNumber(pReward->gold)) +
                              R(MSG_TASK_MONEY) + "</p>";
                }
                if (pReward->itemId != 0) {
                    SItemBase* pItemClass = m_pItemManager->GetItemFromIndex(pReward->itemId);
                    if (pItemClass) {
                        //result += "<img src=\"/" + std::string(pItemClass->icon) +".bmp\"" " width=\"32\" height=\"32\"/>";
                        result += "<p>" + std::string(pItemClass->name) + "</p>";
                    }
                }
            }
            if (bw) {
                result +=
                    "<p><a href = \"client://viewsreward" + std::string(rtFormatNumber(task_id)) +
                    "," + std::string(rtFormatNumber(id[0])) + "," +
                    std::string(rtFormatNumber(id[1])) + "," + std::string(rtFormatNumber(id[2])) +
                    "," + std::string(rtFormatNumber(id[3])) + "," +
                    std::string(rtFormatNumber(id[4])) + "\"" +
                    ">"
                    //+ "�鿴��ϸ��Ϣ" + "</a></p>";//by fox for string
                    + R(MSG_TASK_MORE_INFO) + "</a></p>";
            }
        }
        return true;
    } else {
        // ������������ɷ���������
        HASH_STR(std::string, std::string)::iterator itTranslateMacro;
        itTranslateMacro = m_mapTranslateMacro.find(text);
        if (itTranslateMacro != m_mapTranslateMacro.end()) {
            result = (*itTranslateMacro).second;
        } else {
            result = "";
        }
        return true;
    }

    return false;
    unguard;
}

void GcWorld::OnNetUploadInteractiveNpc(DWORD dwNpcID, const char* szString) {
    guard;
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_interactive_npc);
    pPacket->WriteLong(dwNpcID);
    pPacket->WriteString((char*)szString);
    NetSend(pPacket);
    unguard;
}

void GcWorld::OnNetDownloadDialogByType(DWORD dwNpcID, WORD wType) {}

void GcWorld::OnNetDownloadSwitchRegion(CG_CmdPacket* pPacket) {
    guard;

    long  charID;
    long  seed;
    char* regionHost;
    short regionPort;

    if (!pPacket->ReadLong(&charID))
        return;

    if (!pPacket->ReadLong(&seed))
        return;

    if (!pPacket->ReadString(&regionHost))
        return;

    if (!pPacket->ReadShort(&regionPort))
        return;

    SetRegionServerInfo(charID, seed, regionHost, regionPort);

    LOG2("����: �Ͽ� [��Ϸ������] �������µ� [��Ϸ������ IP=%s,Port=%d]\n", regionHost, regionPort);

    // ���û���������Ķ��ߣ���ֻ�Ǳ���IP��ַ���ж��û����룬Ȼ��ȴ�����
    //     �����������ƻ�ʹ���������ӷ�����

    CG_CmdPacket* pSendPacket = NetBeginWrite();
    pSendPacket->WriteShort(c2r_confirm_switch_region);
    NetSend(pSendPacket);

    //m_nDisconnectCause = NET_DISCONNECT_CAUSE_SWITCH_REGION;
    //m_bSwitchRegion = true;
    //m_Net.Close();
    //m_bSwitchRegion = false;
    //EnterRegionServer();
    unguard;
}

void GcWorld::OnNetDownloadAttack(CG_CmdPacket* pPacket, DWORD dwServerTime, DWORD dwClientTime) {
    guard;
    long  lAttackerID;          // ������ID
    long  lUnderAttackerID;     // Ŀ��ID
    long  lSubUnderAttackerID;  // �ܻ���ID (Ӧ���ڹ������˵�ʱ��)
    long  lPos[3];
    short sAttackID;
    short sSkillID;     // ����ID(������ͨ����)
    short sDamage = 0;  // �˺�ֵ
    short eleDmg[MAX_ELEMENT_ATTR] = {0, 0, 0};
    char  ret;
    char  IsAddHPSkill = 0;
    //BYTE	    cUnderAttackerCnt;      // ������������
    char              cAttackTarget;
    GcActor*          pAttacker;
    GcActor*          pActor;
    GcActor::SAttack* pAttack = NULL;
    long              dead = 0;
    long              bCriDamage = 0;
    char              cIsIntonate = 0;
    char              cIsBreak = 0;

    // LOG1("attack packet size = %d",pPacket->GetByteDataSize());
    READ_long(lAttackerID);
    READ_short(sAttackID);
    READ_short(sSkillID);
    READ_byte(cAttackTarget);
    READ_byte(cIsIntonate);  // �Ƿ�����
    switch (cAttackTarget) {
        case SKILL_TARGET_TYPE_NONE:
            break;
        case SKILL_TARGET_TYPE_ACTOR:
            READ_long(lUnderAttackerID);
            break;
        case SKILL_TARGET_TYPE_POSITION:
            READ_long(lPos[0]);
            READ_long(lPos[1]);
            READ_long(lPos[2]);
            break;
    }
    //READ_byte(cUnderAttackerCnt);

    //MSG2("Attack ID = %d, Skill = %d\n", lAttackerID, sSkillID);

    pAttacker = FindActor(lAttackerID);
    if (pAttacker) {
        if (pAttacker->GetRideState() == GcActor::ERS_PET) {
            pAttacker = pAttacker->GetMaster();
        }
    }

    if (pAttacker == m_pPlayer) {
        pAttack = pAttacker->FindAttackByAttackID(sAttackID);
        if (pAttack) {
            if (pAttack->cAnswer == 0)
                pAttack->cAnswer = 1;
            else if (pAttack->cAnswer == 2)
                pAttack->cAnswer = 3;
            if (pAttack->pSkill) {
                if (pAttack->pSkill->wSubID == SKILL_HIDE_SUBID) {
                    //pAttacker->SkillHideModel();
                } else if (pAttack->pSkill->wSubID == SKILL_LURKER_SUBID) {
                    if (pAttack->pSkill->szChangModel) {
                        pAttacker->ChangeActorToEffect(pAttack->pSkill->szChangModel);
                    }
                }
            }
        }
    } else {
        if (pAttacker) {
            pAttack = RT_NEW GcActor::SAttack;
            pAttack->wAttackID = sAttackID;
            pAttack->pSkill = Skill()->FindSkill(sSkillID);
            pAttack->cTarget = cAttackTarget;
            if (pAttack->cTarget == SKILL_TARGET_TYPE_ACTOR) {
                pAttack->dwTargetID = lUnderAttackerID;
            } else if (pAttack->cTarget == SKILL_TARGET_TYPE_POSITION) {
                pAttack->f[0] = lPos[0];
                pAttack->f[1] = lPos[1];
                pAttack->f[2] = lPos[2];
            } else if (pAttack->cTarget == SKILL_TARGET_TYPE_NONE) {
                pAttack->dwTargetID = lAttackerID;
                if (pAttack->pSkill->wSubID == SKILL_HIDE_SUBID) {
                    //pAttacker->SkillHideModel();
                } else if (pAttack->pSkill->wSubID == SKILL_LURKER_SUBID) {
                    if (pAttack->pSkill->szChangModel) {
                        pAttacker->ChangeActorToEffect(pAttack->pSkill->szChangModel);
                    }
                }
            } else {
                CHECKEX("error");
            }
            pAttack->fTime = (dwServerTime - dwClientTime) / 1000.f;
            if (cIsIntonate == 0)  //��������
            {
                pAttacker->m_bIntonate = false;
                pAttacker->ClearCommand();
                pAttacker->AttackAdd(pAttack);
            } else if (cIsIntonate == 1) {  // ����
                pAttacker->m_bIntonate = true;
                pAttacker->ClearCommand();
                pAttacker->UseIntonate(pAttack->wAttackID, sSkillID, pAttack->dwTargetID, 1,
                                       pAttack->fTime);
            }
        }
    }

    bool criHit = false;
    long eleSpe = 0;
    if (sSkillID == 0) {
        /*
		long tmp = 0;
		pPacket->SerializeBit(tmp,1);
		if(tmp)
			criHit = true;
		pPacket->SerializeBit(eleSpe,4);
		*/
    }
    while (pPacket->ReadLong(&lSubUnderAttackerID))  // �ܻ���ID
    {
        READ_byte(ret);
        if (ret == 5)  //��Ч�ı���
        {
            return;
        }
        READ_byte(cIsBreak);  // �Ƿ����������	0��ʾ������,1��ʾ�����������,2��ʾδ�����.

        if (ret == USE_SKILL_OK) {
            pPacket->SerializeBit(dead, 3);
            pPacket->SerializeBit(bCriDamage, 1);
            if (bCriDamage)
                criHit = true;
            READ_short(sDamage);                        // �˺�
            for (int j = 0; j < MAX_ELEMENT_ATTR; ++j)  // Ԫ���˺�
            {
                READ_short(eleDmg[j]);
            }
        }

        if (pAttacker == NULL || pAttack == NULL) {
            pActor = FindActor(lSubUnderAttackerID);
            if (pActor) {
                if (cIsBreak == 1) {
                    pActor->ClearCommand();
                    pActor->m_bIntonate = false;
                    OnNetUploadBreakIntonate();
                    pActor->AddCommand(GcActor::ACMD_UNDER_ATTACK);
                }

                pActor->ShowDamageNumber(ret, sDamage, eleDmg, criHit, eleSpe);
                pActor->m_bIsDead = Max((char)dead, pActor->m_cIsDeadNet);
                RtgVertex3 pos;
                if (dead) {
                    if (pAttacker) {
                        pAttacker->ClearCommand();
                        pAttacker->m_bIsIdle = true;
                    }
                    //GetWorld()->m_ActorManager.ActorDie(lSubUnderAttackerID,dead);
                    GetWorld()->m_ActorManager.HideHudHp(lSubUnderAttackerID);
                }
                //if(criHit)
                //	g_pSoundMgr->PlayOnce("cri_hit_2.wav",false,0,pos);
            }
        } else {
            switch (cIsBreak) {
                case 0:
                    break;
                case 1:  // �������
                    if (pActor) {
                        pActor = FindActor(lSubUnderAttackerID);
                        pActor->ClearCommand();
                        pActor->m_bIntonate = false;
                        OnNetUploadBreakIntonate();
                        pActor->AddCommand(GcActor::ACMD_UNDER_ATTACK);
                    }
                    break;
                case 2:  // û���
                    break;
            }
            if (lSubUnderAttackerID == GetPlayer()->ID() && pAttacker &&
                pAttacker->NpcType() == GcActor::ENT_USER) {
                GetPlayer()->AddPkTempUser(lAttackerID);
            }
            pAttack->AddDamage(lSubUnderAttackerID, ret, sDamage, eleDmg, criHit, eleSpe,
                               dead != 0);
            //ac.ma			Ѫ���ӳ���ʧ����ʱ�������
            if (dead) {
                //pActor = FindActor(lSubUnderAttackerID);
                //if (pActor)
                //{
                //	pActor->m_bIsDead = dead;
                //}
                if (pAttacker) {
                    //pAttacker->ClearCommand();
                    pAttacker->m_bIsIdle = true;
                }
                //GetWorld()->m_ActorManager.ActorDie(lSubUnderAttackerID,dead);
                GetWorld()->m_ActorManager.HideHudHp(lSubUnderAttackerID);
            }
            //end
        }
    }
    unguard;
}

void GcWorld::OnNetUploadPosition(DWORD dwClientTime) {
    guard;

    if (!m_bLogined) {
        return;
    }

    if (m_iTeleport > 0)  //ac.ma  ���ͺ󷵻ص�����
    {
        m_iTeleport++;
        if (m_iTeleport >= 10) {
            m_iTeleport = 0;
        }
        return;
    }

    //heten
    if (m_iDeadport > 0) {
        m_iDeadport++;
        if (m_iDeadport >= 10) {
            m_iDeadport = 0;
        }
        return;
    }
    //end

    const RtgMatrix16* pM16 = m_pPlayer->GetMatrix();
    CG_CmdPacket*      pPacket = NetBeginWrite();

    pPacket->WriteShort(c2r_update_position);
    pPacket->WriteByte(m_pPlayer->m_posFlag);
    pPacket->WriteLong((long)dwClientTime);

    RtgMatrix16 m16 = *pM16;
    m16.TranslateZ(30.f);

    //��ǰ�����ߵ�λ��
    pPacket->WriteLong((long)m16._30);
    pPacket->WriteLong((long)m16._31);
    pPacket->WriteLong((long)m16._32);
    pPacket->WriteByte((char)rtgEncodeUprightDirection(*pM16));

    // LOG3("Upload Position (%.2f, %.2f, %.2f)\n", pM16->_30, pM16->_31, pM16->_32);
    NetSend(pPacket);
    unguard;
}

void GcWorld::OnNetUploadRidePet(char op) {
    guard;
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_ride);
    pPacket->WriteByte(op);
    NetSend(pPacket);
    unguard;
}

static unsigned short crctableWorld[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
    0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
    0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
    0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
    0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
    0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
    0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
    0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

#define CRC16_INIT_VALUE 0xffff
#define CRC16_XOR_VALUE 0x0000

void GcWorld::OnNetUploadAttackPosition(WORD wSkillID, WORD wAttackID, const DWORD* pPos,
                                        GcActor::SCmd* pCmd, int sum) {
    guard;
    /*
	unsigned short crcvalue = CRC16_INIT_VALUE;
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( wSkillID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wSkillID&0x0FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( wAttackID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wAttackID&0x0FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( ((DWORD*)pPos)[0]&0x000000FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[0]&0x0000FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[0]&0x00FF0000)>>16) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[0]&0xFF000000)>>24) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( ((DWORD*)pPos)[1]&0x000000FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[1]&0x0000FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[1]&0x00FF0000)>>16) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[1]&0xFF000000)>>24) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( ((DWORD*)pPos)[2]&0x000000FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[2]&0x0000FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[2]&0x00FF0000)>>16) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((((DWORD*)pPos)[2]&0xFF000000)>>24) ];
	crcvalue ^= CRC16_XOR_VALUE;
    if (pCmd->wCheck==crcvalue)
    {
    */
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_attack);
    pPacket->WriteShort(wAttackID);
    pPacket->WriteShort(pCmd->wCheck);
    pPacket->WriteShort(wSkillID);  //0 ��ͨ����
    pPacket->WriteByte(SKILL_TARGET_TYPE_POSITION);
    pPacket->WriteLong(sum);
    pPacket->WriteLong(pPos[0]);
    pPacket->WriteLong(pPos[1]);
    pPacket->WriteLong(pPos[2]);
    NetSend(pPacket);
    /*
    }
    */
    unguard;
}

void GcWorld::OnNetUploadBreakIntonate() {
    guard;
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_attack);
    pPacket->WriteShort(0);
    pPacket->WriteShort(0);
    pPacket->WriteShort(0);
    pPacket->WriteByte(SKILL_TARGET_TYPE_BREAK);
    pPacket->WriteLong(0);
    NetSend(pPacket);
    unguard;
}

void GcWorld::OnNetUploadAttackTarget(WORD wSkillID, WORD wAttackID, DWORD dwObjectID,
                                      GcActor::SCmd* pCmd, int sum) {
    guard;

    /*
	unsigned short crcvalue = CRC16_INIT_VALUE;
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ (wSkillID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wSkillID&0x0FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ (wAttackID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wAttackID&0x0FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ( dwObjectID&0x000000FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((dwObjectID&0x0000FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((dwObjectID&0x00FF0000)>>16) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((dwObjectID&0xFF000000)>>24) ];
	crcvalue ^= CRC16_XOR_VALUE;
    if (pCmd->wCheck==crcvalue)
    {
    */
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_attack);
    pPacket->WriteShort(wAttackID);
    pPacket->WriteShort(pCmd->wCheck);
    pPacket->WriteShort(wSkillID);  //0 ��ͨ����
    pPacket->WriteByte(SKILL_TARGET_TYPE_ACTOR);
    pPacket->WriteLong(sum);
    pPacket->WriteLong(dwObjectID);
    NetSend(pPacket);
    /*
    }
	*/
    //static DWORD dwLastAttTime = rtGetMilliseconds();
    //if (rtGetMilliseconds() < dwLastAttTime)
    //{
    //NOTICE("check attack speed. interval:%d, Normal:%d\n",
    //(int)-(rtGetMilliseconds() - dwLastAttTime),(int)GetPlayer()->m_core.GetAttackInterval());
    //}
    //dwLastAttTime = rtGetMilliseconds() + GetPlayer()->m_core.GetAttackInterval();
    unguard;
}

void GcWorld::OnNetUploadAttackSelf(WORD wSkillID, WORD wAttackID, GcActor::SCmd* pCmd, int sum) {
    guard;
    /*
	unsigned short crcvalue = CRC16_INIT_VALUE;
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ (wSkillID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wSkillID&0x0FF00)>>8) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ (wAttackID&0x00FF) ];
	crcvalue = ( crcvalue << 8 ) ^ crctableWorld[ ( crcvalue >> 8 ) ^ ((wAttackID&0x0FF00)>>8) ];
	crcvalue ^= CRC16_XOR_VALUE;
    if (pCmd->wCheck==crcvalue)
    {
    */
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_attack);
    pPacket->WriteShort(wAttackID);
    pPacket->WriteShort(pCmd->wCheck);
    pPacket->WriteShort(wSkillID);  //0 ��ͨ����
    pPacket->WriteByte(SKILL_TARGET_TYPE_NONE);
    pPacket->WriteLong(sum);
    NetSend(pPacket);
    /*
    }
    */
    unguard;
}

void GcWorld::OnNetCurrentProgress(float fProgress) {
    m_fNetCurrentProgress = fProgress;
}

void GcWorld::OnMouseWheel(int iButton, long vDelta) {
    guard;
    /* gao 2009.11.19*/
    //if (m_eCameraMode==CAMERA_MODE_LOCK) return;
    /*end*/
    if (g_workspace.GetFocusWidget() || g_workspace.getMouseHover()) {
        return;
    }

    if (m_bKeyCtrl) {
        GetDevice()->m_pCamera->AddYaw(DegreeToRadian(vDelta * fMouseSpeed));
    } else {
        float fAmount = -1.6 * vDelta * fMouseSpeed;
        m_fCameraCurrentDistance += fAmount;
        if (m_fCameraCurrentDistance < m_fCameraMinDistance) {
            fAmount += (m_fCameraMinDistance - m_fCameraCurrentDistance);
            m_fCameraCurrentDistance = m_fCameraMinDistance;
        } else if (m_fCameraCurrentDistance > m_fCameraMaxDistance) {
            fAmount += (m_fCameraMaxDistance - m_fCameraCurrentDistance);
            m_fCameraCurrentDistance = m_fCameraMaxDistance;
        }
        GetDevice()->m_pCamera->MoveForward(fAmount);
    }
    unguard;
}

/* gao 2009.12.2
��ͷ�Ŀ��Ƶ�һЩ�궨�壬�ֱ�Ϊ���Ƕȣ����ٽǶȣ��Լ���ͷ����ת�ٶ�
*/
#ifdef _DEBUG
#define CAMERAMIN -180
#define CAMERAMAX 180
#define CAMERASPEED 8.f
#else
// Modified by Wayne Wong 2010-12-30
//#define CAMERAMIN 30
#define CAMERAMIN 10
#define CAMERAMAX 60
#define CAMERASPEED 4.f
#endif
/* end */
void GcWorld::OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eViewChange) && task.IsDoneGuild(eKeyMoveRight)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eViewChange);
        NetSend(cmd);
        //�����ƶ���NPCС��������UI��ʾ
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_02_01.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;
    }
    if (!m_bLockCamera) {
        m_Camera.x += (-increaseX);
        m_Camera.y += increaseY;
    } else {
        /* gao 2009.12.2 */
        if (m_eCameraMode == CAMERA_MODE_TURN || m_eCameraMode == CAMERA_MODE_FREE) {
            if (g_layerMain->m_formMiddleMap->IsVisible() &&
                g_workspace.GetFocusWidget() == g_layerMain->m_formMiddleMap->GetRealMapWidget())
                g_layerMain->m_formMiddleMap->OnMouseRDrag(x, y, increaseX, increaseY);
            else {
                GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX) / CAMERASPEED);
                float tmpRoll = (-increaseY) / CAMERASPEED;
                if (nowY + tmpRoll > CAMERAMAX) {
                    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(CAMERAMAX - nowY));
                    nowY = CAMERAMAX;
                } else if (nowY + tmpRoll < CAMERAMIN) {
                    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(CAMERAMIN - nowY));
                    nowY = CAMERAMIN;
                } else {
                    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(tmpRoll));
                    nowY += tmpRoll;
                }
            }
        }
        /* end */
    }
    return;
    unguard;
}

void GcWorld::OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    /* gao
	*/
    unguard;
}

void GcWorld::SetCamera() {
    guard;
    if (m_bFreeCamera) {
        m_Camera = GetDevice()->m_pCamera->GetEyePt();
        GetDevice()->m_pCamera->SetPosition(m_Camera);
        GetDevice()->m_pCamera->SetForward(0.f);
        //GetDevice()->m_pCamera->UpdateMatrix();
        //GetDevice()->m_pCamera->m_vLookatPt = m_Camera;
        return;
    }
    static RtgVertex3 LastVertex3;

    if (m_bLockCamera) {
        GcActor* pLockActor = NULL;
        if (m_pPlayer->GetRideState() == GcActor::ERS_ACTOR) {
            pLockActor = m_pPet;
        } else {
            pLockActor = m_pPlayer;
        }
        if (pLockActor == NULL) {
            pLockActor = m_pPlayer;
        }
        if (pLockActor == NULL) {
            return;
        }
        RtgVertex3 Vertex3 = RtgVertex3(pLockActor->GetMatrix()->Position());
        if (pLockActor->IsValid()) {
            Vertex3 = pLockActor->mBaseActor.GetGraph()->p()->GetWorldMatrix()->Position();
        }
        if (Vertex3 != LastVertex3) {
            GetDevice()->m_pCamera->SetPosition(Vertex3);
            GetDevice()->m_pCamera->UpdateMatrix();
            LastVertex3 = Vertex3;
        }
    } else {
        if (m_Camera != LastVertex3) {
            GetDevice()->m_pCamera->SetPosition(m_Camera);
            LastVertex3 = LastVertex3;
        }
    }
    GetDevice()->SetMatrixFromCamera();
    unguard;
}

void GcWorld::OnPetActive(DWORD dwPetID, DWORD itemid) {
    guard;
    m_dwPetID = dwPetID;
    g_layerMain->m_fromPlayerItems->m_pet.id = itemid;
    if (m_dwPetID == 0 && m_pPet) {
        GetPlayerPet()->CallBackPet();
    } else {
        m_pPet = FindActor(m_dwPetID);
    }
    unguard;
}

void GcWorld::OnNetDownloadShortcut(CG_CmdPacket* pPacket) {
    guard;

    //change by yz
    /*char* shortcut;
	pPacket->ReadString(&shortcut);*/

    g_layerMain->LoadShortcut(pPacket);
    //end
    GetWorld()->m_Chat.LoadChatSet();

    unguard;
}

void GcWorld::OnNetUploadShortcut() {
    guard;

    //if (g_layerMain)
    //{
    //    // string shortcut;
    //    // g_layerMain->SaveShortcut(shortcut);
    //    if (m_bLogined)
    //    {
    //        CG_CmdPacket* pPacket = NetBeginWrite();
    //        pPacket->WriteShort(c2r_shortcut);
    //        g_layerMain->SaveShortcut();
    //        pPacket->WriteString((char*)shortcut.c_str());
    //        NetSend(pPacket);
    //    }
    //}

    //change by yz
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_shortcut);
    g_layerMain->SaveShortcutToNet(pPacket);
    unguard;
}

//add by yz : ������������ڱ����ļ���
void GcWorld::OnLocalSaveShortcut() {
    guard;
    g_layerMain->SaveShortcut();
    unguard;
}

enum ESaveConfigData {
    Config_Camera = 1,
    Config_MaxActorCount = 2,
    Config_HUD = 3,
    Config_InputMode = 4,
    Config_ChatPlan = 5,
    Config_PkMode = 6,
    Hacker_checked = 7,   // ��������Ƿ����ʹ�����
    Hacker_checked2 = 8,  // ��������Ƿ����ʹ�����
};

void GcWorld::OnNetDownloadConfigData(CG_CmdPacket* pPacket) {
    guard;

    /*
    char cVersion, cConfigID;
    short sValue;

    if (!pPacket->ReadByte(&cVersion))
        return;

    if (cVersion==1)
    {
        while (pPacket->ReadByte(&cConfigID))
        {
            if (!pPacket->ReadShort(&sValue))
                return;

            switch (cConfigID)
            {
            case Config_Camera:
                SetCameraMode((ECameraMode)sValue);
                break;

            case Config_MaxActorCount:
                m_ActorManager.SetMaxDisplayChar(sValue);
                break;

            case Config_HUD:
                RtwHUD::s_cShowItem = sValue;
                OnHUDConfigChanged();
                break;

            case Config_InputMode:
                {
                    UIFormSetting::sUIInputProject = sValue;
                    if (UIFormSetting::sUIInputProject<0 || UIFormSetting::sUIInputProject>1)
                    {
                        UIFormSetting::sUIInputProject = 0;
                    }
                    //���ý�������ģʽ
                    switch(UIFormSetting::sUIInputProject)
                    {
                    case 0://ֱ��ģʽ
                        m_UIProject.SetInputProject("DIRECT_CHAT");
                        break;
                    case 1://�س�ģʽ
                        m_UIProject.SetInputProject("ENTER_KEY_TO_CHAT");
                        break;
                    default:
                        m_UIProject.SetInputProject();
                        break;
                    }
                }
                break;
            case Config_ChatPlan:
                {
                    UIFormSetting::sUIShowProject = sValue;
                    if (UIFormSetting::sUIShowProject<0 || UIFormSetting::sUIShowProject>1)
                    {
                        UIFormSetting::sUIShowProject = 0;
                    }
                    //���ý�����ʾģʽ
                    switch(UIFormSetting::sUIShowProject)
                    {
                    case 0://����ģʽ
                        m_UIProject.SetShowProject("NORMAL_SHOW");
                        break;
                    case 1://͸��ģʽ
                        m_UIProject.SetShowProject("TRANSPARENT_SHOW");
                        break;
                    default:
                        m_UIProject.SetShowProject();
                        break;
                    }
                }
                break;
			case Config_PkMode:
				if (m_pPlayer)
				{
					m_pPlayer->m_PkMode = (EPkMode)sValue;
				}
				break;
            default:
                return;
                break;
            }
        }
    }
    /**/

    char* data;
    pPacket->ReadString(&data);

    if (data[0] == '\0')
        return;

    char* config = strdup(data);
    char  sep[] = ",";
    char* k = strtok(config, sep);

    if (!k)
        return;

    int version = atoi(k);
    if (version == 1) {
        k = strtok(NULL, sep);
        while (k) {
            int configID = atoi(k);
            switch (configID) {
                case Config_Camera: {
                    k = strtok(NULL, sep);
                    if (k) {
                        ECameraMode camMode = (ECameraMode)atoi(k);
                        SetCameraMode(camMode);
                    } else {
                        ERR("Load config data form network error(Config_Camera)\n");
                    }
                } break;
                case Config_MaxActorCount: {
                    k = strtok(NULL, sep);
                    if (k) {
                        //heten  ͬ������С��50��Ϊ50
                        short i = atoi(k);
                        if (i < 50) {
                            i = 50;
                        }
                        //end
                        m_ActorManager.SetMaxDisplayChar(i);
                    } else {
                        ERR("Load config data form network error(Config_MaxActorCount)\n");
                        return;
                    }
                } break;
                case Config_HUD: {
                    k = strtok(NULL, sep);
                    if (k) {
                        //RtwHUD::s_cShowItem = atoi(k);
                        OnHUDConfigChanged();
                    } else {
                        ERR("Load config data form network error(Config_HUD)\n");
                        return;
                    }
                } break;
                case Config_InputMode: {
                    // gao 2010.1.9 ��ʱ�������⣬ֻ�����س�����
                    //              k = strtok(NULL, sep);
                    //              if (k)
                    //              {
                    //                  UIFormSetting::sUIInputProject = atoi(k);
                    //                  if (UIFormSetting::sUIInputProject<0 || UIFormSetting::sUIInputProject>1)
                    //                  {
                    //                      UIFormSetting::sUIInputProject = 0;
                    //                  }
                    //                  {//���ý�������ģʽ
                    //                      switch(UIFormSetting::sUIInputProject)
                    //                      {
                    //                      case 0://ֱ��ģʽ
                    //                          m_UIProject.SetInputProject("DIRECT_CHAT");
                    //                          break;
                    //                      case 1://�س�ģʽ
                    //                          m_UIProject.SetInputProject("ENTER_KEY_TO_CHAT");
                    //                          break;
                    //                      default:
                    //                          m_UIProject.SetInputProject();
                    //                          break;
                    //                      }
                    //}
                    //              }
                    //              else
                    //              {
                    //                  ERR("Load config data form network error(Config_InputMode)\n");
                    //                  return;
                    //              }
                    m_UIProject.SetInputProject("ENTER_KEY_TO_CHAT");
                } break;
                case Config_ChatPlan: {
                    /* gao 2010.1.9
					���ｫ���촰������Ϊ��͸����
					*/
                    k = strtok(NULL, sep);
                    if (k) {
                        UIFormSetting::sUIShowProject = atoi(k);
                        if (UIFormSetting::sUIShowProject < 0 ||
                            UIFormSetting::sUIShowProject > 1) {
                            UIFormSetting::sUIShowProject = 0;
                        }
                        {  //���ý�����ʾģʽ
                            switch (UIFormSetting::sUIShowProject) {
                                case 0:  //����ģʽ
                                    m_UIProject.SetShowProject("NORMAL_SHOW");
                                    break;
                                //case 1://͸��ģʽ
                                //    m_UIProject.SetShowProject("TRANSPARENT_SHOW");
                                //    break;
                                default:
                                    m_UIProject.SetShowProject();
                                    break;
                            }
                        }
                    } else {
                        ERR("Load config data form network error(Config_ChatPlan)\n");
                        return;
                    }
                    /* end */
                } break;
                case Config_PkMode: {
                    k = strtok(NULL, sep);
                    if (k) {
                        if (m_pPlayer) {
                            m_pPlayer->m_PkMode = (EPkMode)atoi(k);
                            g_layerMain->m_formPkMode->OnChangePkModeImage(m_pPlayer->m_PkMode);
                        }
                    } else {
                        ERR("Load config data form network error(Config_PkMode)\n");
                        return;
                    }
                } break;
                default:
                    return;
                    break;
            }
            k = strtok(NULL, sep);
        }
    }

    //<add by fox for memory leak>
    free(config);
    //</add by fox for memory leak>

    /***/

    unguard;
}

void GcWorld::OnNetUploadConfigData() {
    guard;

    /*
    string configData;

    configData += "1,"; // version
    configData += rtFormatNumber(Config_Camera);
    configData += ",";
    configData += rtFormatNumber(GetCameraMode());

    configData += ",";
    configData += rtFormatNumber(Config_MaxActorCount);
    configData += ",";
    configData += rtFormatNumber(m_ActorManager.GetMaxDisplayChar());

    configData += ",";
    configData += rtFormatNumber(Config_HUD);
    configData += ",";
    configData += rtFormatNumber(RtwHUD::s_cShowItem);

	if (m_pPlayer)
	{
		configData += ",";
		configData += rtFormatNumber(Config_PkMode);
		configData += ",";
		configData += rtFormatNumber(m_pPlayer->m_PkMode);
	}

    if (g_layerMain)
    {
        configData += ",";
        configData += rtFormatNumber(Config_InputMode);
        configData += ",";
        configData += rtFormatNumber(UIFormSetting::sUIInputProject);

        configData += ",";
        configData += rtFormatNumber(Config_ChatPlan);
        configData += ",";
        configData += rtFormatNumber(UIFormSetting::sUIShowProject);
    }

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_save_config);	
    pPacket->WriteString((char*)configData.c_str());
    NetSend(pPacket);
    */

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_save_config);
    pPacket->WriteByte(1);  // version

    pPacket->WriteByte(Config_Camera);
    pPacket->WriteShort(GetCameraMode());

    pPacket->WriteByte(Config_MaxActorCount);
    pPacket->WriteShort(m_ActorManager.GetMaxDisplayChar());

    pPacket->WriteByte(Config_HUD);
    pPacket->WriteShort(RtwHUD::s_cShowItem);

    if (m_pPlayer) {
        pPacket->WriteByte(Config_PkMode);
        pPacket->WriteShort(m_pPlayer->m_PkMode);

        if (m_pPlayer->m_iCheckSeed != m_iCheckPlayerSeed) {
            pPacket->WriteByte(Hacker_checked2);
            pPacket->WriteShort(m_bLockCamera);  // ��㷢��һ������
        }
    }

    if (g_layerMain) {
        pPacket->WriteByte(Config_InputMode);
        pPacket->WriteShort(UIFormSetting::sUIInputProject);

        pPacket->WriteByte(Config_ChatPlan);
        pPacket->WriteShort(UIFormSetting::sUIShowProject);
    }

    if (m_bHackChecked) {
        pPacket->WriteByte(Hacker_checked);
        pPacket->WriteShort(m_bStateChangeFrame);  // ��㷢��һ������
    }

    NetSend(pPacket);

    unguard;
}

bool GcWorld::OnMemberRClick(DWORD vDBID, const char* vpName)  //�����һ����Ա��ѡ��
{
    return false;  //����Ϣ���ػ�
}

void GcWorld::OnPopupMenuOnlyByName(RtwPopupMenu* pPopupMenu, string& rActorName) {
    guard;
    pPopupMenu->RemoveAllItems();  //�������
    pPopupMenu->AddItem(R(POP_MENU_PRIVATE_CHAT), 0, (void*)1, rActorName);
    pPopupMenu->AddItem(R(POP_MENU_COPY), 0, (void*)2, rActorName);
    pPopupMenu->AddItem(R(POP_MENU_BLACKLIST), 0, (void*)3, rActorName);
    pPopupMenu->AddItem(R(G_SEEITEMS), NULL, (void*)4);
    pPopupMenu->AddItem(R(MSG_FRIEND_INVITE), 0, (void*)5);
    pPopupMenu->AddItem(R(POP_MENU_INVITE_STRING), NULL, (void*)6, "invite");  //����
    pPopupMenu->AddItem(R(POP_MENU_APPLYTEAM), NULL, (void*)7, "apply");       //����
    pPopupMenu->AddItem(R(POP_MENU_MASKPLAYER), NULL, (void*)8);               //����
    unguard;
}

void GcWorld::OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor) {
    guard;
    pPopupMenu->RemoveAllItems();
    g_layerMain->m_fromMiniMap->PopMenuReset(NULL, NULL);  //pop�˵�������
    pPopupMenu->AddItem(R(POP_MENU_PRIVATE_CHAT), 0, (void*)1);
    g_layerMain->m_formTrade->OnPopupMenu(pPopupMenu, pTargetActor, 2);
    m_Team.OnPopupMenu(pPopupMenu, pTargetActor, 3);
    m_Friend.OnPopupMenu(pPopupMenu, pTargetActor, 4);
    g_layerMain->m_formUnion->OnPopupMenu(pPopupMenu, pTargetActor, 5);
    pPopupMenu->AddItem(R(POP_MENU_COPY), 0, (void*)6);
    g_layerMain->m_formItemsInfo->OnPopupMenu(pPopupMenu, pTargetActor, 7);
    pPopupMenu->AddItem(R(POP_MENU_BLACKLIST), 0, (void*)8);
    pPopupMenu->AddItem(R(POP_MENU_DUEL), 0, (void*)9);
    pPopupMenu->EnableIndex(8, false);
    pPopupMenu->AddItem(R(POP_MENU_FOLLOW), 0, (void*)10);
    pPopupMenu->EnableIndex(9, false);
    unguard;
}

void GcWorld::OnHUDConfigChanged() {
    guard;
    if (m_pPlayer && m_pPlayer->m_pHUD) {
        m_pPlayer->m_pHUD->ApplyFlag();
    }

    if (m_pPet && m_pPet->m_pHUD) {
        m_pPet->m_pHUD->ApplyFlag();
    }

    m_ActorManager.OnHUDConfigChanged();
    unguard;
}

bool GcWorld::SetSceneFile(const char* szFilename) {
    guard;
    if (szFilename == NULL) {
        szFilename = "scene01";
    }
    //g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbut_CallTaskItem,3000);
    m_szSceneFileName = szFilename;
    rt2_sprintf(m_szSceneFile, "scene/scene01/%s.rts", szFilename);
    if (strcmp(m_szLastSceneFile, m_szSceneFile)) {
        rt2_strncpy(m_szLastSceneFile, m_szSceneFile, 128);

        this->m_ActorManager.OnMapChanged();
        if (m_pPlayer) {
            m_pPlayer->OnMapChanged();
        }
        if (m_pPet) {
            m_pPet->OnMapChanged();
            GetPlayerPet()->ProcessSwitchScene();
        }
        //Ϊ�����m_poolPath����
        m_ActorManager.Clear();
        m_pPlayer->mBaseActor.RemoveSelf();

        g_pScene->Clear();
        if (!g_pScene->LoadMap(m_szLastSceneFile, false)) {
            rt2_sprintf(g_strStaticBuffer, R(WMSG_CANNOT_LOAD_SCENE), m_szLastSceneFile);
            ShowErr(g_strStaticBuffer);
            CHECKEX(g_strStaticBuffer);
            return false;
        }
        char szAreaFile[128], szWayPointFile[128];
        rt2_sprintf(szAreaFile, "scene/scene01/%s.rsr", szFilename);
        g_pScene->AddAreaInfo(szAreaFile);
        m_PathFind.SetBindTerrain(g_pScene);
        //���г���ʱ�����Ӧ·���ļ�
        rt2_sprintf(szWayPointFile, "scene/scene01/%s.wp", szFilename);
        m_PathFind.LoadWayPointInfo(szWayPointFile);
    }
    //g_layerMain->m_formTimeBar->End(true);
    return true;
    unguard;
}

void GcWorld::OnInputCardID(long lNpcID, long lType, const char* szCardID, const char* szCardPwd) {
    CG_CmdPacket* pPacket;
    switch (lType) {
        case 1:  // ChargeCard
            pPacket = NetBeginWrite();
            pPacket->WriteShort(c2r_charge_card);
            pPacket->WriteLong(lNpcID);
            pPacket->WriteByte(1);                   // Card Type
            pPacket->WriteString((char*)szCardID);   // ���������м�鳤�� �� ����
            pPacket->WriteString((char*)szCardPwd);  // ���������м�鳤�� �� ����
            NetSend(pPacket);
            break;
    }
}

void GcWorld::OnBagSkillUsed(void* pData) {}

void GcWorld::OnMouseRightDown(RtwWidget* vpSender, RtwEventDelegate* e) {
    guard;
    GcActor* pTargetActor =
        GetWorld()->m_ActorManager.FindAll(g_layerMain->m_formMonInfo->GetActorID());
    if (pTargetActor && pTargetActor->NpcType() == GcActor::ENT_USER) {
        // ����˵�
        RtwPopupMenu* pPopupMenu = ((RtwPopupMenu*)g_workspace.LookupWidget("UserMenu"));
        if (pPopupMenu) {
            GetWorld()->OnPopupMenu(pPopupMenu, pTargetActor);
            pPopupMenu->SetUserData((void*)pTargetActor->ID());
            g_workspace.ShowPopupMenu(pPopupMenu, NULL, g_workspace.GetMousePosX(),
                                      g_workspace.GetMousePosY());
        }
    }
    unguard;
}

void GcWorld::OnNetDownloadCharGuide(short sType) {
    switch (sType) {
        case eHitMove: {
            //������������UI��ʾ
            g_layerMain->m_formGuide1->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_01_01.html";
            g_layerMain->m_viewGuide1->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;
        } break;
        case eNPCTaskTalk: {
            if (g_layerMain->m_formGuide1->IsVisible()) {
                g_layerMain->m_formGuide1->Hide();
            }
            //�״δ򿪵�ǰ�пɽ�������NPC�Ի�ҳ��֮���UI��ʾ
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = g_layerMain->m_formHelp->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += rectDest.getHeight() / 2 + 30;
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_03_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eNPCTaskTalk);
            NetSend(cmd);
        } break;
        case eNPCFinishTask: {
            if (g_layerMain->m_formGuide2->IsVisible()) {
                g_layerMain->m_formGuide2->Hide();
            }
            //�״δ��������NPC�Ի�ҳ��֮���UI��ʾ
            RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
            RtwRect rectDest = g_layerMain->m_btnFinished->GetClientRect();
            rectDest.left -= (rectSrc.getWidth() / 2 + 110);
            rectDest.top -= (rectDest.getHeight() / 2 + 50);
            g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide3->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_04_01.html";
            g_layerMain->m_viewGuide3->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eNPCFinishTask);
            NetSend(cmd);
        } break;
        case eAttrButFlash: {
            //�������2��֮��
            if (g_layerMain->m_formGuide3->IsVisible()) {
                g_layerMain->m_formGuide3->Hide();
            }
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmsystem.btnsyschar")->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += (rectDest.getHeight() / 2 - 10);
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_05_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eAttrButFlash);
            NetSend(cmd);
        } break;
        case eFirstAttackMonster: {
            //����״ι��������������ʾ
            RtwRect rectSrc = g_layerMain->m_formGuide2->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmmonhp")->GetClientRect();
            rectDest.left += 30;
            rectDest.top += rectDest.getHeight() / 2;
            g_layerMain->m_formGuide2->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide2->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_07_01.html";
            g_layerMain->m_viewGuide2->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eFirstAttackMonster);
            NetSend(cmd);
        } break;
        case eEatHP: {
            //����״�hp����50%
            RtwRect rectSrc = g_layerMain->m_formGuide2->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmhp")->GetClientRect();
            rectDest.left += 40;
            rectDest.top += rectDest.getHeight() / 2 + 40;
            g_layerMain->m_formGuide2->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide2->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_08_01.html";
            g_layerMain->m_viewGuide2->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eEatHP);
            NetSend(cmd);
        } break;
        case eEatMP: {
            //����״�mp����50%
            RtwRect rectSrc = g_layerMain->m_formGuide2->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmhp")->GetClientRect();
            rectDest.top += rectDest.getHeight() / 2 + 40;
            g_layerMain->m_formGuide2->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide2->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_09_01.html";
            g_layerMain->m_viewGuide2->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eEatMP);
            NetSend(cmd);
        } break;
        case eUsePet1: {
            //��ҽ�������11001�����ұ�������ID16000Ϊ����Ʒ
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmsystem.btnpet")->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += (rectDest.getHeight() / 2 - 10);
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_10_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eUsePet1);
            NetSend(cmd);
        } break;
        case eBuyItem1: {
            //��������11004����Ҵ�npc��ʵ��һ���Ի�ҳ��֮��
            RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
            RtwRect rectDest = g_layerMain->m_formHelp->GetClientRect();
            rectDest.left -= (rectSrc.getWidth() / 2 + 110);
            rectDest.top += 190;
            g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide3->SetFocus();
            g_layerMain->m_formGuide3->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_11_01.html";
            g_layerMain->m_viewGuide3->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eBuyItem1);
            NetSend(cmd);
        } break;
        case eFaBao1: {
            //��������11005֮��,���ܰ�ť��˸
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmsystem.btnsyschar")->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += (rectDest.getHeight() / 2 - 10);
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_12_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eFaBao1);
            NetSend(cmd);
        } break;
        case eMail1: {
            //�ӵ����ʼ���������
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmsystem.btnsysmail")->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += (rectDest.getHeight() / 2 - 10);
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_13_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eMail1);
            NetSend(cmd);
        } break;
        case eLifeSkill1: {
            //����״δ�npc³�ҵ�һ���Ի�ҳ��֮��
            RtwRect rectSrc = g_layerMain->m_formGuide4->GetClientRect();
            RtwRect rectDest = g_layerMain->m_formHelp->GetClientRect();
            rectDest.left -= rectSrc.getWidth();
            rectDest.top += rectDest.getHeight() / 2 + 55;
            g_layerMain->m_formGuide4->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide4->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_14_01.html";
            g_layerMain->m_viewGuide4->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eLifeSkill1);
            NetSend(cmd);
        } break;
        case eAreaEnter: {
            HideGuideForm();
            //�������������UI��ʾ
            g_layerMain->m_formGuide1->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_19_01.html";
            g_layerMain->m_viewGuide1->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eAreaEnter);
            NetSend(cmd);
        } break;
    }
}

void GcWorld::ResetHideTime() {
    dwShowTime = 0.f;
}

void GcWorld::HideGuideForm() {
    g_layerMain->m_formGuide1->Hide();
    g_layerMain->m_formGuide2->Hide();
    g_layerMain->m_formGuide3->Hide();
    g_layerMain->m_formGuide4->Hide();
    g_layerMain->m_formGuide5->Hide();
}

void GcWorld::SetHideGuide() {
    //W��
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eKeyMoveFront) && task.IsDoneGuild(eHitMove)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveFront);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveBack);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveLeft);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveRight);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eViewChange);
            NetSend(cmd);
        }
    }
    //S��
    if (!task.IsDoneGuild(eKeyMoveBack) && task.IsDoneGuild(eKeyMoveFront)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveBack);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveLeft);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveRight);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eViewChange);
            NetSend(cmd);
        }
    }
    //A��
    if (!task.IsDoneGuild(eKeyMoveLeft) && task.IsDoneGuild(eKeyMoveBack)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveLeft);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveRight);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eViewChange);
            NetSend(cmd);
        }
    }
    //D��
    if (!task.IsDoneGuild(eKeyMoveRight) && task.IsDoneGuild(eKeyMoveLeft)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eKeyMoveRight);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eViewChange);
            NetSend(cmd);
        }
    }
    //�Ҽ�ת�ӽ�
    if (!task.IsDoneGuild(eViewChange) && task.IsDoneGuild(eKeyMoveRight)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eViewChange);
        NetSend(cmd);
    }
    //�ƶ���NPCС������
    if (!task.IsDoneGuild(eNPCTalk)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eNPCTalk);
        NetSend(cmd);
    }
    //���ܸ�����
    if (!task.IsDoneGuild(eNPCAcceptTask) && task.IsDoneGuild(eNPCTaskTalk)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eNPCAcceptTask);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eMoveToNPC);
            NetSend(cmd);
        }
    }
    //�Զ��ƶ�����NPC��
    if (!task.IsDoneGuild(eMoveToNPC) && task.IsDoneGuild(eNPCAcceptTask)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMoveToNPC);
        NetSend(cmd);
    }
    //��Ҵ�����ҳ��֮��
    if (!task.IsDoneGuild(eAddAttr) && task.IsDoneGuild(eAttrButFlash)) {
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eAddAttr);
            NetSend(cmd);
        }
        {
            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eFinishAddAttr);
            NetSend(cmd);
        }
    }
    //����ȷ����ť��������Ե�����
    if (!task.IsDoneGuild(eFinishAddAttr) && task.IsDoneGuild(eAddAttr)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eFinishAddAttr);
        NetSend(cmd);
    }
    //��ҽ���ҳ���л�����5��֮�������һ��ʾ
    if (!task.IsDoneGuild(eSkillChangePane) && task.IsDoneGuild(eOpenSkillUI)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eSkillChangePane);
        NetSend(cmd);
    }
}

void GcWorld::ShowTaskGuide() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eMoveToNPC) && task.IsDoneGuild(eNPCAcceptTask)) {
        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMoveToNPC);
        NetSend(cmd);
        //�������������UI��ʾ
        RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
        RtwRect rectDest = g_layerMain->m_formCharSimple->m_dynTaskTrack->GetClientRect();
        rectDest.left -= rectDest.getWidth() / 2 + 120;
        rectDest.top += (rectDest.getHeight() / 2 - 140);
        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide3->SetFocus();
        g_layerMain->m_formGuide3->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_03_03.html";
        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;
    }
}

void GcWorld::ShowKeyboardGuide() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eShowKeyBoard) && task.IsDoneGuild(eMoveToNPC)) {
        //����W����UI��ʾ
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_03_04.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eShowKeyBoard);
        NetSend(cmd);
    }
}

void GcWorld::ShowAttrPoint() {
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eAddAttr) && task.IsDoneGuild(eAttrButFlash)) {
        if (g_layerMain->m_formGuide4->IsVisible()) {
            g_layerMain->m_formGuide4->Hide();
        }
        //�������������UI��ʾ
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmnewchar.fmproperty1.fmxmm3.btnhitadd")->GetClientRect();
        rectDest.left += 20;
        rectDest.top += rectDest.getHeight() / 2;
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_05_02.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eAddAttr);
        NetSend(cmd);
    }
}

void GcWorld::ShowAttrPointAllocate() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eFinishAddAttr) && task.IsDoneGuild(eAddAttr)) {
        //�������������UI��ʾ
        RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmnewchar.fmproperty1.fmxmm6.btnok")->GetClientRect();
        rectDest.left -= rectSrc.getWidth() / 2 + 120;
        rectDest.top += (rectDest.getHeight() / 2 - 60);
        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide3->SetFocus();
        g_layerMain->m_formGuide3->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_05_03.html";
        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eFinishAddAttr);
        NetSend(cmd);
    }
}

void GcWorld::ShowSkillPage() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eOpenSkillUI)) {
        //����״δ򿪼���ҳ��
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest =
            LOAD_UI("fmborder_skill.fmskillall.tabskillall.fmsecond")->GetClientRect();
        rectDest.left += (rectSrc.getWidth() / 2 - 30);
        rectDest.top -= (rectDest.getHeight() / 2 - 165);
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_06_01.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eOpenSkillUI);
        NetSend(cmd);
    }
}

void GcWorld::ShowSkillPoint() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eSkillChangePane) && task.IsDoneGuild(eOpenSkillUI)) {
        //����״δ򿪼���ҳ��
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest =
            LOAD_UI("fmborder_skill.fmskillall.tabskillall.fmsecond")->GetClientRect();
        rectDest.left += (rectSrc.getWidth() / 2 - 40);
        rectDest.top -= (rectDest.getHeight() / 2 - 220);
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_06_02.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eSkillChangePane);
        NetSend(cmd);
    }
}

void GcWorld::ShowCallPet() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eUsePet2) && task.IsDoneGuild(eUsePet1)) {
        //����ٻ�����
        RtwRect rectSrc = g_layerMain->m_formGuide2->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmconjure.fightblt")->GetClientRect();
        rectDest.left += rectSrc.getWidth() / 2 - 140;
        rectDest.top += (rectDest.getHeight() / 2 + 5);
        g_layerMain->m_formGuide2->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide2->SetFocus();
        g_layerMain->m_formGuide2->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_10_02.html";
        g_layerMain->m_viewGuide2->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eUsePet2);
        NetSend(cmd);
    }
}

void GcWorld::ShowPetInfo() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eUsePet3) && task.IsDoneGuild(eUsePet2)) {
        //��ҵ�ǰ�ٻ�������
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmhpc")->GetClientRect();
        rectDest.left += 140;
        rectDest.top += 20;
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_10_03.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eUsePet3);
        NetSend(cmd);
    }
}

void GcWorld::ShowShopUI() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eBuyItem2) && task.IsDoneGuild(eBuyItem1)) {
        //��Ҵ��̵�ҳ��
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmnewshop.fmshop.fmsellitem")->GetClientRect();
        rectDest.left += 3 * 36;
        rectDest.top += 2 * 36;
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_11_02.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eBuyItem2);
        NetSend(cmd);
    }
}

void GcWorld::ShowShopTrade() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eBuyItem3) && task.IsDoneGuild(eBuyItem2)) {
        //��ҵ����Ʒ���ֹ���ҳ��
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmnewshop.fmshop.tbctrading.fmcontent1.btnok")->GetClientRect();
        rectDest.left -= 120;
        rectDest.top -= 55;
        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide3->SetFocus();
        g_layerMain->m_formGuide3->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_11_03.html";
        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eBuyItem3);
        NetSend(cmd);
    }
}

void GcWorld::ShowLifePage() {
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eFaBao2) && task.IsDoneGuild(eFaBao1)) {
        HideGuideForm();
        //��ҵ����Ʒ���ֹ���ҳ��
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmplaymain.tabitem.btnfabao")->GetClientRect();
        rectDest.left += rectDest.getWidth();
        rectDest.top += 5;
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_12_02.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eFaBao2);
        NetSend(cmd);
    }
}

void GcWorld::ShowViewMail() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eMail3) && task.IsDoneGuild(eMail1)) {
        //��Ҳ鿴�ʼ�
        RtwRect rectSrc = g_layerMain->m_formGuide5->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmmail2.fmcontent.btnxmm3")->GetClientRect();
        rectDest.left += 70;
        rectDest.top -= 0;
        g_layerMain->m_formGuide5->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide5->SetFocus();
        g_layerMain->m_formGuide5->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_13_02.html";
        g_layerMain->m_viewGuide5->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMail3);
        NetSend(cmd);
    }
}

void GcWorld::ShowExtraMail() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eMail5) && task.IsDoneGuild(eMail3)) {
        //��Ҳ鿴����
        RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmmail1.fmcontent.btnview")->GetClientRect();
        rectDest.left -= 260;
        rectDest.top -= 55;
        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide3->SetFocus();
        g_layerMain->m_formGuide3->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_13_04.html";
        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMail5);
        NetSend(cmd);
    }
}

void GcWorld::ShowStudySkill() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eLifeSkill2) && task.IsDoneGuild(eLifeSkill1)) {
        //��Ҳ鿴����
        RtwRect rectSrc = g_layerMain->m_formGuide3->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmlearn.fmcontent.fmzoning1.btulearn1")->GetClientRect();
        rectDest.left -= 130;
        rectDest.top -= 60;
        g_layerMain->m_formGuide3->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide3->SetFocus();
        g_layerMain->m_formGuide3->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_14_02.html";
        g_layerMain->m_viewGuide3->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eLifeSkill2);
        NetSend(cmd);
    }
}

void GcWorld::ShowFriendUI() {
    HideGuideForm();
    CTaskInfo* task = &GetWorld()->m_pPlayer->m_task;
    if (!task->IsDoneGuild(eFriend1)) {
        bool                           bFirstFriend = false;
        std::list<STaskInfo>::iterator it;
        for (it = task->m_taskList.begin(); it != task->m_taskList.end(); it++) {
            if (it->IsDoing == 0)
                continue;
            if (it->IsDoing == 2)
                continue;
            STaskDesc* desc = g_TableTask.FindTask((*it).Id);
            if (!desc)
                continue;

            if (desc->id == 11007) {
                bFirstFriend = true;
                break;
            }
        }
        if (bFirstFriend) {
            RtwRect rectSrc = g_layerMain->m_formGuide2->GetClientRect();
            RtwRect rectDest = LOAD_UI("fmmonhp")->GetClientRect();
            rectDest.left -= 30;
            rectDest.top += rectDest.getHeight() / 2 + 30;
            g_layerMain->m_formGuide2->Move(SPoint(rectDest.left, rectDest.top));
            g_layerMain->m_formGuide2->Show();
            std::string fileName = "task\\xinshouyindao\\xinshou_15_01.html";
            g_layerMain->m_viewGuide2->LoadFromFile(fileName);
            ResetHideTime();
            bCurOperate = true;

            CG_CmdPacket* cmd = NetBeginWrite();
            cmd->WriteShort(c2r_Guide);
            cmd->WriteShort(eFriend1);
            NetSend(cmd);
        }
    }
}

void GcWorld::ShowMiddleMap() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eMidMap1)) {
        //��Ҳ鿴����
        RtwRect rectSrc = g_layerMain->m_formGuide1->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmsysmap")->GetClientRect();
        rectDest.left += 100;
        rectDest.top -= 110;
        g_layerMain->m_formGuide1->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide1->SetFocus();
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_17_01.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMidMap1);
        NetSend(cmd);
    }
}

void GcWorld::ShowMiddleMapAI() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eMidMap2) && task.IsDoneGuild(eMidMap1)) {
        //��Ҳ鿴����
        RtwRect rectSrc = g_layerMain->m_formGuide1->GetClientRect();
        RtwRect rectDest = LOAD_UI("fmsysmap")->GetClientRect();
        rectDest.left += 100;
        rectDest.top -= 110;
        g_layerMain->m_formGuide1->Move(SPoint(rectDest.left, rectDest.top));
        g_layerMain->m_formGuide1->SetFocus();
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_17_02.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eMidMap2);
        NetSend(cmd);
    }
}

void GcWorld::ShowPlayerInScreen() {
    HideGuideForm();
    CTaskInfo task = m_pPlayer->m_task;
    if (!task.IsDoneGuild(eHidePlayer)) {
        //����ͬ����ҵ�UI��ʾ
        g_layerMain->m_formGuide1->Show();
        std::string fileName = "task\\xinshouyindao\\xinshou_18_01.html";
        g_layerMain->m_viewGuide1->LoadFromFile(fileName);
        ResetHideTime();
        bCurOperate = true;

        CG_CmdPacket* cmd = NetBeginWrite();
        cmd->WriteShort(c2r_Guide);
        cmd->WriteShort(eHidePlayer);
        NetSend(cmd);
    }
}

// -----------------------------------------------------------------------------------
// lua c++ suport function
// -----------------------------------------------------------------------------------
long CreateActor(int i) {
    return NULL;
}

// -----------------------------------------------------------------------------------
// lua
// -----------------------------------------------------------------------------------
const char* te = "aidx = {}\n"
                 "ccheck = 100\n"
                 "poffset = 121\n"
                 "idoffset = 91\n"
                 ""
                 "function te(x)\n"
                 "print('ttt')\n"
                 "return CreateActor()\n"
                 "end\n"
                 ""
                 "--set call check\n"
                 "function scc(x)\n"
                 "ccheck = ccheck + 111\n"
                 "end\n"
                 ""
                 "--get next check\n"
                 "function ecc(x)\n"
                 "return 211\n"
                 "end\n"
                 ""
                 "--reset call check\n"
                 "function fcc(x)\n"
                 "ccheck = ccheck - 111\n"
                 "end\n"
                 ""
                 "--find actor id by point\n"
                 "function fa(p)\n"
                 "return aidx[p+poffset] - idoffset\n"
                 "end\n"
                 ""
                 "--check if dummy id\n"
                 "function dm(id)\n"
                 "return math.mod(id,11)==0\n"
                 "end"
                 ""
                 "--create actor\n"
                 "function ca(p,id)\n"
                 "aidx[p+poffset] = id + idoffset\n"
                 "print('ca'..aidx[p+poffsdt])\n"
                 "end\n"
                 ""
                 "--destroy actor\n"
                 "function da(p)\n"
                 "print('da'..aidx[p+poffset])\n    "
                 "aidx[p+poffset] = 0\n"
                 "end\n"
                 ""
                 "--make normal attack command checksum\n"
                 "function nk(p)\n"
                 "return aidx[p+poffset]\n"
                 "end\n"
                 ""
                 "--make skill attack command checksum\n"
                 "function sk(p)\n"
                 "tid = sidx[p+poffset]\n"
                 "if ecc(1) == ccheck then\n"
                 "fcc(1)\n"
                 "return 111\n"
                 "else\n"
                 "return 222\n"
                 "end\n"
                 "end\n"
                 ""
                 "--make string checksum\n"
                 "function stringcs(s)\n"
                 "if math.mod(string.len(s),3) == 0 then\n"
                 "print('p3')\n"
                 "return ha_P3(s,string.len(s),88)\n"
                 "else\n"
                 "print('p2')\n"
                 "return ha_P2(s,string.len(s))\n"
                 "end\n"
                 "end\n";

const char* td = "function as(s)\n"
                 "s = '1zql'..s..'check'\n"
                 "stringcs(s)\n"
                 "end\n";

CILua* g_lua;

CILua* Lua() {
    return g_lua;
}

/*
int s_lHoldRand2 = 19790213L;
int s_lHoldRand1 = 19790213L;
int abcRandom()
{
    s_lHoldRand1 = ((s_lHoldRand1 * 205839L + 2835234L) / 17)%65536;
    return s_lHoldRand1;
}
*/

bool LuaInit() {
    g_lua = RT_NEW CILua;
    bool           ret;
    int            cret;

    // load script
    REG_CFUNC(*Lua(), CreateActor)
    REG_CFUNC(*Lua(), ha_P2);
    REG_CFUNC(*Lua(), ha_P3);
    REG_CFUNC(*Lua(), sin_table);
    REG_CFUNC(*Lua(), cos_table);
    REG_CFUNC(*Lua(), get_table_value);
    //REG_CFUNC(*Lua(), table_print);

    RtArchive* ar = RtCoreFile().CreateFileReader(R(RES_LUA_INIT_HTML));
    int        total = ar->TotalSize();
    char*      tmpBuf = RT_NEW char[total + 1];
    ar->Serialize(tmpBuf, total);
    tmpBuf[total] = 0;

    if (!Lua()->LoadScriptByString(tmpBuf)) {
        CHECK(0);
    }

    for (int i = 0; i < total; ++i)
        tmpBuf[i] = 0;
    DEL_ONE(tmpBuf);
    DEL_ONE(ar);

    /*
    void** pStackBase;
    _asm
    {
        mov dword ptr [pStackBase],ebp 
    }
    {
        void *ppEBP, *ppRet;
        int iLev, iMaxLev;
        unsigned long iCode = 0;
        iMaxLev = 10;

        LOG2("BaseEBP  : %08X %d\n", (unsigned long)pStackBase, (unsigned long)pStackBase);
        pStackBase = (void**)pStackBase[0];
        pStackBase = (void**)pStackBase[0];
        for (iLev=0; iLev<iMaxLev; iLev++)
        {
            ppEBP = pStackBase[0];
            ppRet = pStackBase[1];
            LOG2("  EBP    : %08X %d\n", (unsigned long)ppEBP, (unsigned long)ppEBP);
            LOG2("  RetAddr: %08X %d\n", (unsigned long)ppRet, (unsigned long)ppRet);
            iCode += (unsigned long)ppRet;
            pStackBase = (void**)ppEBP;
            if (pStackBase==0)
            {
                break;
            }
        }
        LOG1("DebugCheck = %08X\n", iCode);
    }
    short szSendBuf[100] = {22, 33};
    int iRet, iCode;
    _asm
    {
        mov dword ptr [pStackBase],ebp 
    }
    Lua()->CallLFunc("c", "iiii:ii", szSendBuf, 4, (int)pStackBase, 1, &iRet, &iCode);
    LOG2("DebugCheck2 = %d, %08X\n", iRet, iCode);

    // �����������
    int r1, r2, r3=0;
    Lua()->CallLFunc("setrs", "i:", s_lHoldRand2);
    for (int iii=0; iii<10000; i++ii)
    {
        r1 = abcRandom();
        Lua()->CallLFunc("jr", ":i", &r2);
        if (r1!=r2)
        {
            r3 ++;
        }
        MSG3("%5d %5d %2d\n", r1, r2, r3);
    }
    */

    Lua()->CallLFunc("scc", "i", 1);
    Lua()->CallLFunc("ca", "ii", 1000, 1000);
    if (!Lua()->CallLFunc("sk", "i:i", 1000, &cret))
        LOG1("%d\n", cret);
    if (!Lua()->CallLFunc("sk", "i:i", 1000, &cret))
        LOG1("%d\n", cret);
    if (!Lua()->CallLFunc("sk", "i:i", 1000, &cret))
        LOG1("%d\n", cret);

    if (!Lua()->CallLFunc("stringcs", "s:i", "ccc", &cret))
        LOG1("%d\n", cret);

    if (!Lua()->CallLFunc("stringcs", "s:i", "cccc", &cret))
        LOG1("%d\n", cret);

    ret = true;
    return ret;
}

bool LuaExit() {
    DEL_ONE(g_lua);
    return true;
}

void GcWorld::RefreshTriggerBlockPos() {
    if (g_pScene) {
        std::list<RtgVertex3>::iterator iter;
        if (m_listBlockAdd.size()) {
            iter = m_listBlockAdd.begin();
            for (iter = m_listBlockAdd.begin(); iter != m_listBlockAdd.end();) {
                if (m_pPlayer->Distance(iter->x, iter->y) < 250) {
                    if (g_pScene->SetTerrainAttr(iter->x / 20, iter->y / 20, 1)) {
                        iter = m_listBlockAdd.erase(iter);
                    } else
                        ++iter;
                } else
                    ++iter;
            }
        }

        if (m_listBlockKill.size()) {
            for (iter = m_listBlockKill.begin(); iter != m_listBlockKill.end();) {
                if (m_pPlayer->Distance(iter->x, iter->y) < 250) {
                    if (g_pScene->RemoveTerrainAttr(iter->x / 20, iter->y / 20, 1)) {
                        iter = m_listBlockKill.erase(iter);
                    } else
                        ++iter;
                } else
                    ++iter;
            }
        }
    }
}

void GcWorld::SetTriggerBlock(int id) {
    if (!GetWorld()->GetDungeon()) {
        ERR("�ͻ��˸�����Ϣ������\n");
        return;
    }

    STrigger* pTrigger = GetDungeon()->GetTrigger(id);

    if (!pTrigger) {
        ERR("������Ϣ������\n");
        return;
    }

    if (pTrigger->type == 3) {
        return;
    }
    if (pTrigger->close) {
        int x, y;
        for (int i = 0; i < pTrigger->blockCount; ++i) {
            x = pTrigger->blockPos[i].x / 20;
            y = pTrigger->blockPos[i].y / 20;
            if (m_pPlayer->Distance(pTrigger->blockPos[i].x, pTrigger->blockPos[i].y) > 250 ||
                !g_pScene->SetTerrainAttr(x, y, 1)) {
                RtgVertex3 pos;
                pos.x = pTrigger->blockPos[i].x;
                pos.y = pTrigger->blockPos[i].y;
                pos.z = pTrigger->id;
                m_listBlockAdd.push_back(pos);
            }
        }
        std::list<RtgVertex3>::iterator iter;
        if (m_listBlockKill.size()) {
            for (iter = m_listBlockKill.begin(); iter != m_listBlockKill.end();) {
                if (iter->z == pTrigger->id) {
                    iter = m_listBlockKill.erase(iter);
                } else
                    ++iter;
            }
            m_listBlockKill.swap(m_listBlockKill);
        }
    } else {
        int x, y;
        for (int i = 0; i < pTrigger->blockCount; ++i) {
            x = pTrigger->blockPos[i].x / 20;
            y = pTrigger->blockPos[i].y / 20;
            if (m_pPlayer->Distance(pTrigger->blockPos[i].x, pTrigger->blockPos[i].y) > 250 ||
                !g_pScene->RemoveTerrainAttr(x, y, 1)) {
                RtgVertex3 pos;
                pos.x = pTrigger->blockPos[i].x;
                pos.y = pTrigger->blockPos[i].y;
                pos.z = pTrigger->id;
                m_listBlockKill.push_back(pos);
            }
        }
        std::list<RtgVertex3>::iterator iter;
        if (m_listBlockAdd.size()) {
            for (iter = m_listBlockAdd.begin(); iter != m_listBlockAdd.end();) {
                if (iter->z == pTrigger->id) {
                    iter = m_listBlockAdd.erase(iter);
                } else
                    ++iter;
            }
            m_listBlockAdd.swap(m_listBlockAdd);
        }
    }
}

void GcWorld::UseKeyBoardMove() {
    guard;
    static long lLastMoveTime = rtGetMilliseconds();
    if (rtGetMilliseconds() - lLastMoveTime < 100 || m_pPlayer->GetMoveCmdNum() > 2) {
        return;
    }
    float pPos[3];
    m_pPlayer->GetMatrix()->GetRow(3, pPos);
    RtgMatrix16 playerMatrix = *m_pPlayer->GetMatrix();
    RtgVertex3  eyeDirection = GetDevice()->m_pCamera->GetViewDir();
    playerMatrix._10 = eyeDirection.x;
    playerMatrix._11 = eyeDirection.y;
    if (m_iKey1 != -1 && m_iKey2 == -1) {
        // �����ǰ��ʱ��û�а��°���
        switch (m_iKey1) {
            case 0:
                playerMatrix.RotateZ(DegreeToRadian(0.0f));
                break;
            case 1:
                playerMatrix.RotateZ(DegreeToRadian(180.0f));
                break;
            case 2:
                playerMatrix.RotateZ(DegreeToRadian(90.0f));
                break;
            case 3:
                playerMatrix.RotateZ(DegreeToRadian(270.0f));
                break;
            default:
                return;
        }
        RtgVertex3 eyeDir = *(RtgVertex3*)&playerMatrix._10;

        (((m_iCheckPlayerSeed = m_iCheckPlayerSeed * 203014L + 3521051L) >> 16) & 0x7fff);
        float X = pPos[0] + eyeDir.x * 25.f;
        float Y = pPos[1] + eyeDir.y * 25.f;
        float Z;
        GetScene()->GetTerrainHeight(X, Y, Z);
        m_pPlayer->AddCommandMoveTo(X, Y, Z, -1.f, 0.f);
        m_bIsMovableCursor = true;
        m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
    } else if (m_iKey2 != -1 && m_iKey1 != -1) {
        // �ж��Ƿ���Ч�����
        switch (m_iKey1) {
            case 0:
                switch (m_iKey2) {
                    case 2:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(45));
                        break;
                    case 3:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(-45));
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        return;
                }
                break;
            case 1:
                switch (m_iKey2) {
                    case 2:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(135));
                        break;
                    case 3:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(-135));
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        return;
                }
                break;
            case 2:
                switch (m_iKey2) {
                    case 0:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(45));
                        break;
                    case 1:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(135));
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        return;
                }
                break;
            case 3:
                switch (m_iKey2) {
                    case 0:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(-45));
                        break;
                    case 1:
                        // ����
                        playerMatrix.RotateZ(DegreeToRadian(-135));
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        return;
                }
                break;
            default:
                break;
        }
        RtgVertex3 eyeDir = *(RtgVertex3*)&playerMatrix._10;

        (((m_iCheckPlayerSeed = m_iCheckPlayerSeed * 203014L + 3521051L) >> 16) & 0x7fff);
        float X = pPos[0] + eyeDir.x * 100;
        float Y = pPos[1] + eyeDir.y * 100;
        float Z;
        GetScene()->GetTerrainHeight(X, Y, Z);
        m_pPlayer->AddCommandMoveTo(X, Y, Z, -1.f, 0.f);
        m_bIsMovableCursor = true;
        m_iCheckPlayerSeedSave = m_iCheckPlayerSeed;
    }
    lLastMoveTime = rtGetMilliseconds();
    unguard;
}

void GcWorld::OnMoveKeyDown(int direction) {
    guard;

    //add by yz 2010-6-10: �һ�ʱ�������ܿ����ƶ�
    if (m_pPlayer->GetAutoState())  //	ac.ma
    {
        //����һ���Ƿ�ѡ���
        LOAD_UI("forautostate")->Show();
        return;
    }
    //end yz

    /* ˵����
	0 ����
	1 ����
	2 ����
	3 ����
	*/
    if (m_iKey1 == -1 || m_iKey1 == direction) {
        // �����ǰ��ʱ��û�а��°���
        switch (direction) {
            case 0:
            case 1:
            case 2:
            case 3:
                break;
            default:
                return;
        }
        m_iKey1 = direction;
        m_bKeyBoardMoving = true;
    } else if (m_iKey2 == -1 || m_iKey2 == direction) {
        // �ж��Ƿ���Ч�����
        switch (m_iKey1) {
            case 0:
                switch (direction) {
                    case 2:
                        // ����
                    case 3:
                        // ����
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        m_iKey1 = direction;
                        m_iKey2 = -1;
                        return;
                }
                break;
            case 1:
                switch (direction) {
                    case 2:
                        // ����
                    case 3:
                        // ����
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        m_iKey1 = direction;
                        m_iKey2 = -1;
                        return;
                }
                break;
            case 2:
                switch (direction) {
                    case 0:
                        // ����
                    case 1:
                        // ����
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        m_iKey1 = direction;
                        m_iKey2 = -1;
                        return;
                }
                break;
            case 3:
                switch (direction) {
                    case 0:
                        // ����
                    case 1:
                        // ����
                        break;
                    default:
                        // ��Ч��ϣ�������ͬ
                        m_iKey1 = direction;
                        m_iKey2 = -1;
                        return;
                }
                break;
            default:
                break;
        }
        m_iKey2 = direction;
        m_bKeyBoardMoving = true;
    } else {
        m_iKey1 = m_iKey2;
        m_iKey2 = direction;
        m_bKeyBoardMoving = true;
    }
    g_layerMain->m_formMiddleMap->m_bNeedShowPoint = false;
    unguard;
}

void GcWorld::OnMoveKeyUp(int direction) {
    guard;
    if (m_iKey2 == direction)
        m_iKey2 = -1;
    else if (m_iKey1 == direction) {
        m_iKey1 = m_iKey2;
        m_iKey2 = -1;
    }
    if (m_iKey1 == -1) {
        m_bKeyBoardMoving = false;
        m_bIsControlByKeyBoard = true;
    }
    unguard;
}

static float fVolume = 0.f;

void GcWorld::OnKillFocus() {
    fVolume = g_pBackMusic->GetVolume();
    g_pBackMusic->SetVolume(0.f);
    return;
}

void GcWorld::OnSetFocus() {
    g_pBackMusic->SetVolume(fVolume);
    if (g_layerMain) {
        // �ڻָ����ڽ���ʱ �ָ����м��ܿ���
        for (size_t i = 0; i < UI_HOT_KEY_COUNT; i++) {
            g_layerMain->m_bHotKeyState[i] = false;
        }
    }
    return;
}

void GcWorld::OnSetTabSelectTarget() {
    guard;
    GcActor* pLastTargetActor = NULL;
    GcActor* pTargetActor = NULL;
    int      iDistance = 999999;
    if (g_layerMain->m_formMonInfo->GetActorID()) {
        pLastTargetActor =
            GetWorld()->m_ActorManager.FindAll(g_layerMain->m_formMonInfo->GetActorID());
        pTargetActor = pLastTargetActor;
    }
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>& rPlayerMap =
        GetWorld()->m_ActorManager.m_ActorMap;  // ���н�ɫ����
    EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator It;
    for (It = rPlayerMap.begin(); It != rPlayerMap.end(); ++It) {
        GcSynchroActor* pActor = (It->second);
        if (!pActor->IsActive() && pActor->NpcType() != GcActor::ENT_USER) {
            continue;  //����һ����Ķ���
        }

        if (pActor->NpcType() == GcActor::ENT_MONSTER && pActor->IsValid() &&
            pActor != pLastTargetActor) {
            if (GetPlayer()->Distance(pActor) < iDistance) {
                if (pActor->m_pNpc->Type == 4 || pActor->m_pNpc->Type == 5)
                    continue;
                pTargetActor = pActor;
                iDistance = GetPlayer()->Distance(pActor);
            }
        }
    }
    if (pTargetActor) {
        m_pPlayer->OnSetTarget(pTargetActor->ID());
        GetWorld()->m_selectedActor = pTargetActor->ID();
    }
    unguard;
}

GcActorSkill* GcWorld::OnGetBuffHintForSkill() {
    guard;
    GcActor* pTargetActor;
    if (g_layerMain->m_formCreInfo->IsVisible())
        pTargetActor = FindActor(g_layerMain->m_formCreInfo->m_nActorID);
    else if (g_layerMain->m_formMonInfo->IsVisible())
        pTargetActor = FindActor(g_layerMain->m_formMonInfo->m_nActorID);
    else
        return NULL;
    if (pTargetActor)
        return &(pTargetActor->m_Skill);
    else
        return NULL;
    unguard;
}

void GcWorld::PlayActorEffect(DWORD userID, string effect, bool link /* = true*/,
                              string     linkSlot /* = "root"*/,
                              RtgVertex3 offset /* = RtgVertex3(0.f,0.f,0.f)*/,
                              string posename /* = "effect"*/, bool loop /* = false*/,
                              float speed /* = 1.f*/, DWORD waitTime /* = 5000*/) {
    GcActor* pActor = FindAllActor(userID);
    if (pActor) {
        pActor->mBaseActor.PlayEffect(effect.c_str(), link, linkSlot.c_str(), offset, loop, speed,
                                      posename.c_str());
    } else {
        m_listPlayEffectActor.push_back(
            PlayEffectActorList(userID, effect.c_str(), link, linkSlot.c_str(), offset,
                                posename.c_str(), loop, speed, waitTime));
    }
}

void GcWorld::EnterDungon(DWORD UID, DWORD TID, DWORD process) {
    DEL_ONE(m_pDungeon);

    m_pDungeon = RT_NEW SGcDungeon;
    if (!m_pDungeon)
        return;

    m_pDungeon->lDungeonUID = UID;
    m_pDungeon->lDungeonTID = TID;
    m_pDungeon->process = process;

    if (m_pDungeon) {
        std::map<int, STrigger>&          triggerMap = g_TableFB.m_trigger;
        std::map<int, STrigger>::iterator iter;
        for (iter = triggerMap.begin(); iter != triggerMap.end(); ++iter) {
            if (iter->second.fbID == TID && iter->second.id != 0 && iter->second.active) {
                STrigger trigger = (iter->second);
                m_pDungeon->trigger.push_back(trigger);
                if (iter->second.process > process) {
                    GetWorld()->SetTriggerBlock(iter->second.id);
                }
            }
        }
    }
    g_layerMain->m_fromSkill->OnAddCanNotUseSkill(503);
}

void GcWorld::LeaveDungon() {
    DEL_ONE(m_pDungeon);
    g_layerMain->m_fromSkill->OnDelCanNotUseSkill(503);
}

SGcDungeon* GcWorld::GetDungeon() {
    return m_pDungeon;
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
